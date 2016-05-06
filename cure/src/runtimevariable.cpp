
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

// Warning: optimized code.



#include "pch.h"
#include "../../lepra/include/lepraassert.h"
#include "../../lepra/include/hashutil.h"
#include "../../lepra/include/thread.h"
#include "../../lepra/include/random.h"
#include "../include/runtimevariable.h"



namespace cure {



RuntimeVariable::RuntimeVariable(const str& name, const str& value, Usage pUsage):
	name_(name),
	real_value_(0),
	default_str_value_(value),
	default_real_value_(0) {
	data_type_ = RuntimeVariable::kDatatypeString;
	SetStrValue(value, pUsage);
}

RuntimeVariable::RuntimeVariable(const str& name, bool value, Usage pUsage):
	name_(name),
	real_value_(0),
	default_bool_value_(value),
	default_real_value_(0) {
	data_type_ = RuntimeVariable::kDatatypeBool;
	SetBoolValue(value, pUsage);
}

RuntimeVariable::RuntimeVariable(const str& name, int value, Usage pUsage):
	name_(name),
	real_value_(0),
	default_int_value_(value),
	default_real_value_(0) {
	data_type_ = RuntimeVariable::kDatatypeInt;
	SetIntValue(value, pUsage);
}

RuntimeVariable::RuntimeVariable(const str& name, double value, Usage pUsage):
	name_(name),
	default_real_value_(value) {
	data_type_ = RuntimeVariable::kDatatypeReal;
	SetRealValue(value, pUsage);
}

RuntimeVariable::~RuntimeVariable() {
}

const str& RuntimeVariable::GetName() const {
	return name_;
}

RuntimeVariable::DataType RuntimeVariable::GetType() const {
	return data_type_;
}

//bool RuntimeVariable::operator==(const str& value)
//{
//	return (str_value_ == value);
//}

const str& RuntimeVariable::GetStrValue() const {
	CheckType(kDatatypeString);
	return str_value_;
}

bool RuntimeVariable::GetBoolValue() const {
	CheckType(kDatatypeBool);
	return bool_value_;
}

int RuntimeVariable::GetIntValue() const {
	CheckType(kDatatypeInt);
	return int_value_;
}

double RuntimeVariable::GetRealValue() const {
	CheckType(kDatatypeReal);
	return real_value_;
}

bool RuntimeVariable::SetStrValue(const str& value, Usage pUsage) {
	if (!CheckType(kDatatypeString)) {
		return false;
	}
	str_value_ = value;
	if (pUsage >= kUsageSysOverride) {
		default_str_value_ = value;
	}
	usage_ = pUsage;
	return true;
}

bool RuntimeVariable::SetBoolValue(bool value, Usage pUsage) {
	if (!CheckType(kDatatypeBool)) {
		return false;
	}
	bool_value_ = value;
	if (pUsage >= kUsageSysOverride) {
		default_bool_value_ = value;
	}
	usage_ = pUsage;
	return true;
}

bool RuntimeVariable::SetIntValue(int value, Usage pUsage) {
	if (!CheckType(kDatatypeInt)) {
		return false;
	}
	int_value_ = value;
	if (pUsage >= kUsageSysOverride) {
		default_int_value_ = value;
	}
	usage_ = pUsage;
	return true;
}

bool RuntimeVariable::SetRealValue(double value, Usage pUsage) {
	if (!CheckType(kDatatypeReal)) {
		return false;
	}
	real_value_ = value;
	if (pUsage >= kUsageSysOverride) {
		default_real_value_ = value;
	}
	usage_ = pUsage;
	return true;
}

const str& RuntimeVariable::GetDefaultStrValue() const {
	CheckType(kDatatypeString);
	return default_str_value_;
}

bool RuntimeVariable::GetDefaultBoolValue() const {
	CheckType(kDatatypeBool);
	return default_bool_value_;
}

int RuntimeVariable::GetDefaultIntValue() const {
	CheckType(kDatatypeInt);
	return default_int_value_;
}

double RuntimeVariable::GetDefaultRealValue() const {
	CheckType(kDatatypeReal);
	return default_real_value_;
}

void RuntimeVariable::SetDefaultStrValue(const str& default_value) {
	if (CheckType(kDatatypeString)) {
		default_str_value_ = default_value;
	}
}

void RuntimeVariable::SetDefaultBoolValue(bool default_value) {
	if (CheckType(kDatatypeBool)) {
		default_bool_value_ = default_value;
	}
}

void RuntimeVariable::SetDefaultIntValue(int default_value) {
	if (CheckType(kDatatypeInt)) {
		default_int_value_ = default_value;
	}
}

void RuntimeVariable::SetDefaultRealValue(double default_value) {
	if (CheckType(kDatatypeReal)) {
		default_real_value_ = default_value;
	}
}

RuntimeVariable::Usage RuntimeVariable::GetUsage() const {
	return (usage_);
}

void RuntimeVariable::Reset() {
	switch (data_type_) {
		case RuntimeVariable::kDatatypeString:	str_value_ = default_str_value_;
		case RuntimeVariable::kDatatypeBool:	bool_value_ = default_bool_value_;
		case RuntimeVariable::kDatatypeInt:	int_value_ = default_int_value_;
		case RuntimeVariable::kDatatypeReal:	real_value_ = default_real_value_;
	}
}

str RuntimeVariable::GetTypeName(DataType type) {
	switch (type) {
		case RuntimeVariable::kDatatypeString:	return "string";
		case RuntimeVariable::kDatatypeBool:	return "bool";
		case RuntimeVariable::kDatatypeInt:	return "int";
		case RuntimeVariable::kDatatypeReal:	return "real";
	}
	deb_assert(false);
	return "void";
}

bool RuntimeVariable::CheckType(DataType type) const {
	if (data_type_ == type) {
		return true;
	}
	log_.Warningf("Type error using variable %s; type should be %s, not %s!",
		name_.c_str(), GetTypeName(data_type_).c_str(), GetTypeName(type).c_str());
	//deb_assert(false);
	return false;
}


RuntimeVariableScope::RuntimeVariableScope(RuntimeVariableScope* parent_scope):
	parent_scope_(parent_scope),
	owner_seed_(Random::GetRandomNumber()) {
	variable_table_.rehash(1024);
}

RuntimeVariableScope::~RuntimeVariableScope() {
	VariableTable::iterator x = variable_table_.begin();
	for (; x != variable_table_.end(); ++x) {
		RuntimeVariable* _variable = x->second;
		delete _variable;
	}
	variable_table_.clear();
	parent_scope_ = 0;
}

bool RuntimeVariableScope::IsDefined(const str& name) {
	bool found = (GetVariable(name) != 0);
	return (found);
}

bool RuntimeVariableScope::SetUntypedValue(SetMode set_mode, const str& name, const str& value) {
	bool is_recursive = (set_mode == RuntimeVariable::kUsageNormal || set_mode == RuntimeVariable::kUsageInternal);
	RuntimeVariable* _variable = GetVariable(name, is_recursive);
	str _str_value;
	bool _bool_value;
	int _int_value;
	double real_value;
	DataType _type = Cast(value, _str_value, _bool_value, _int_value, real_value);
	if (_variable) {
		switch (_type) {
			case RuntimeVariable::kDatatypeString:	return _variable->SetStrValue(_str_value, set_mode);
			case RuntimeVariable::kDatatypeBool:	return _variable->SetBoolValue(_bool_value, set_mode);
			case RuntimeVariable::kDatatypeInt:	return _variable->SetIntValue(_int_value, set_mode);
			case RuntimeVariable::kDatatypeReal:	return _variable->SetRealValue(real_value, set_mode);
		}
	} else {
		CreateLocalVariable(name, _type, _str_value, _bool_value, _int_value, real_value, set_mode);
	}
	return true;
}

bool RuntimeVariableScope::SetValue(SetMode set_mode, const str& name, const str& value) {
	bool is_recursive = (set_mode == RuntimeVariable::kUsageNormal || set_mode == RuntimeVariable::kUsageInternal);
	RuntimeVariable* _variable = GetVariable(name, is_recursive);
	if (_variable) {
		return _variable->SetStrValue(value, set_mode);
	} else {
		CreateLocalVariable(name, RuntimeVariable::kDatatypeString, value, false, 0, 0, set_mode);
	}
	return true;
}

bool RuntimeVariableScope::SetValue(SetMode set_mode, const str& name, const char* value) {
	return (SetValue(set_mode, name, str(value)));
}

bool RuntimeVariableScope::SetValue(SetMode set_mode, const str& name, bool value) {
	bool is_recursive = (set_mode == RuntimeVariable::kUsageNormal || set_mode == RuntimeVariable::kUsageInternal);
	RuntimeVariable* _variable = GetVariable(name, is_recursive);
	if (_variable) {
		return _variable->SetBoolValue(value, set_mode);
	} else {
		CreateLocalVariable(name, RuntimeVariable::kDatatypeBool, str(), value, 0, 0, set_mode);
	}
	return true;
}

bool RuntimeVariableScope::SetValue(SetMode set_mode, const str& name, int value) {
	bool is_recursive = (set_mode == RuntimeVariable::kUsageNormal || set_mode == RuntimeVariable::kUsageInternal);
	RuntimeVariable* _variable = GetVariable(name, is_recursive);
	if (_variable) {
		return _variable->SetIntValue(value, set_mode);
	} else {
		CreateLocalVariable(name, RuntimeVariable::kDatatypeInt, str(), false, value, 0, set_mode);
	}
	return true;
}

bool RuntimeVariableScope::SetValue(SetMode set_mode, const str& name, double value) {
	bool is_recursive = (set_mode == RuntimeVariable::kUsageNormal || set_mode == RuntimeVariable::kUsageInternal);
	RuntimeVariable* _variable = GetVariable(name, is_recursive);
	if (_variable) {
		return _variable->SetRealValue(value, set_mode);
	} else {
		CreateLocalVariable(name, RuntimeVariable::kDatatypeReal, str(), false, 0, value, set_mode);
	}
	return true;
}

str RuntimeVariableScope::GetUntypedDefaultValue(GetMode mode, const str& name) {
	RuntimeVariable* _variable = GetVariable(HashedString(name));
	if (!_variable) {
		return EmptyString;
	}
	switch (_variable->GetType()) {
		case RuntimeVariable::kDatatypeString: {
			return GetDefaultValue(mode, name, _variable, EmptyString);
		}
		case RuntimeVariable::kDatatypeBool: {
			return GetDefaultValue(mode, name, _variable, false) ? "true" : "false";
		}
		case RuntimeVariable::kDatatypeInt: {
			int i = GetDefaultValue(mode, name, _variable, 0);
			return strutil::IntToString(i, 10);
		}
		case RuntimeVariable::kDatatypeReal: {
			double d = GetDefaultValue(mode, name, _variable, 0.0);
			return strutil::Format("%.4f", d);
		}
	}
	return EmptyString;
}

const str& RuntimeVariableScope::GetDefaultValue(GetMode mode, const HashedString& name, const str& default_value) {
	return GetDefaultValue(mode, name, GetVariable(name), default_value);
}

const str RuntimeVariableScope::GetDefaultValue(GetMode mode, const HashedString& name, const char* default_value) {
	return GetDefaultValue(mode, name, GetVariable(name), str(default_value));
}

bool RuntimeVariableScope::GetDefaultValue(GetMode mode, const HashedString& name, bool default_value) {
	return GetDefaultValue(mode, name, GetVariable(name), default_value);
}

int RuntimeVariableScope::GetDefaultValue(GetMode mode, const HashedString& name, int default_value) {
	return GetDefaultValue(mode, name, GetVariable(name), default_value);
}

double RuntimeVariableScope::GetDefaultValue(GetMode mode, const HashedString& name, double default_value) {
	return GetDefaultValue(mode, name, GetVariable(name), default_value);
}

bool RuntimeVariableScope::EraseVariable(const str& name) {
	bool deleted;
	{
		ScopeSpinLock lock(&lock_);
		deleted = DeleteLocalVariable(name);
	}
	if (!deleted && parent_scope_) {
		deleted = parent_scope_->EraseVariable(name);
	}
	return deleted;
}

bool RuntimeVariableScope::ResetDefaultValue(const str& name) {
	RuntimeVariable* _variable = GetVariable(HashedString(name));
	if (!_variable) {
		return false;
	}
	_variable->Reset();
	return true;
}



std::list<str> RuntimeVariableScope::GetVariableNameList(SearchType search_type, int start_scope_index, int end_scope_index) {
	std::list<str> variable_name_list;
	ScopeSpinLock lock(&lock_);
	if (end_scope_index > 0 && start_scope_index < end_scope_index && parent_scope_) {
		variable_name_list = parent_scope_->GetVariableNameList(search_type, start_scope_index-1, end_scope_index-1);
	}
	if (start_scope_index <= 0) {
		VariableTable::iterator x = variable_table_.begin();
		for (; x != variable_table_.end(); ++x) {
			if (search_type == kSearchAll ||
				x->second->GetUsage() != RuntimeVariable::kUsageInternal) {
				variable_name_list.push_back(x->first);
			}
		}
	}
	return variable_name_list;
}



RuntimeVariableScope::DataType RuntimeVariableScope::GetUntypedType(const str& value) {
	str s;
	bool b;
	int i;
	double d;
	return Cast(value, s, b, i, d);
}

RuntimeVariableScope::DataType RuntimeVariableScope::GetType(const str& value, str& out_str, bool& out_bool, int& out_int, double& out_double) {
	if (strutil::StringToBool(value, out_bool)) {
		return RuntimeVariable::kDatatypeBool;
	}
	if (strutil::StringToInt(value, out_int)) {
		return RuntimeVariable::kDatatypeInt;
	}
	if (strutil::StringToDouble(value, out_double)) {
		return RuntimeVariable::kDatatypeReal;
	}
	out_str = value;
	return RuntimeVariable::kDatatypeString;
}

RuntimeVariableScope::DataType RuntimeVariableScope::Cast(const str& value, str& out_str, bool& out_bool, int& out_int, double& out_double) {
	if (value.substr(0, 9) == "(boolean)") {
		size_t constant_start = strutil::FindNextWord(value, " \t\r\n)", 9-1);
		if (constant_start < value.length()) {
			const str _value = value.substr(constant_start);
			bool __bool;
			int __i;
			double __d;
			if (strutil::StringToBool(_value, __bool)) {
				// Redundant cast.
				out_bool = __bool;
				return RuntimeVariable::kDatatypeBool;
			} else if (strutil::StringToInt(_value, __i)) {
				out_bool = (__i != 0);
				return RuntimeVariable::kDatatypeBool;
			} else if (strutil::StringToDouble(_value, __d)) {
				out_bool = (__d >= -0.5 && __d < 0.5);
				return RuntimeVariable::kDatatypeBool;
			}
			log_.Warningf("Uncastable bool value: %s.", _value.c_str());
			out_str = value;
			return RuntimeVariable::kDatatypeString;
		}
	} else if (value.substr(0, 5) == "(int)") {
		size_t constant_start = strutil::FindNextWord(value, " \t\r\n)", 5-1);
		if (constant_start < value.length()) {
			const str _value = value.substr(constant_start);
			bool __bool;
			int __i = 0;
			double __d;
			if (strutil::StringToBool(_value, __bool)) {
				out_int = (int)__bool;
				return RuntimeVariable::kDatatypeInt;
			} else if (strutil::StringToInt(_value, __i)) {
				// Redundant cast.
				out_int = __i;
				return RuntimeVariable::kDatatypeInt;
			} else if (strutil::StringToDouble(_value, __d)) {
				out_int = (int)__d;
				return RuntimeVariable::kDatatypeInt;
			} else if (_value.length() == 3 && _value[0] == '\'' && _value[2] == '\'') {
				out_int = (unsigned)_value[1];
				return RuntimeVariable::kDatatypeInt;
			}
			log_.Warningf("Uncastable integer value: %s.", _value.c_str());
			out_str = value;
			return RuntimeVariable::kDatatypeString;
		}
	} else if (value.substr(0, 6) == "(real)") {
		size_t constant_start = strutil::FindNextWord(value, " \t\r\n)", 6-1);
		if (constant_start < value.length()) {
			const str _value = value.substr(constant_start);
			bool __bool;
			int __i;
			double __d = 0;
			if (strutil::StringToBool(_value, __bool)) {
				out_double = (double)__bool;
				return RuntimeVariable::kDatatypeReal;
			} else if (strutil::StringToInt(_value, __i)) {
				out_double = __i;
				return RuntimeVariable::kDatatypeReal;
			} else if (strutil::StringToDouble(_value, __d)) {
				// Redundant cast.
				out_double = __d;
				return RuntimeVariable::kDatatypeReal;
			}
			log_.Warningf("Uncastable real value: %s.", _value.c_str());
			out_str = value;
			return RuntimeVariable::kDatatypeString;
		}
	}
	return GetType(value, out_str, out_bool, out_int, out_double);
}



const str& RuntimeVariableScope::GetDefaultValue(GetMode mode, const str& name, RuntimeVariable* variable, const str& default_value) {
	if (variable) {
		if (mode == kReadDefault) {
			return variable->GetDefaultStrValue();
		} else if (mode == kReadWrite) {
			variable->SetDefaultStrValue(default_value);
		}
		return variable->GetStrValue();
	} else if (mode == kReadWrite) {
		SetValue(RuntimeVariable::kUsageNormal, name, default_value);
	} else if (mode != kReadIgnore) {
		log_.Warningf("Variable %s not found.", name.c_str());
	}
	return default_value;
}

bool RuntimeVariableScope::GetDefaultValue(GetMode mode, const str& name, RuntimeVariable* variable, bool default_value) {
	if (variable) {
		if (mode == kReadDefault) {
			return variable->GetDefaultBoolValue();
		} else if (mode == kReadWrite) {
			variable->SetDefaultBoolValue(default_value);
		}
		return variable->GetBoolValue();
	} else if (mode == kReadWrite) {
		SetValue(RuntimeVariable::kUsageNormal, name, default_value);
	} else if (mode != kReadIgnore) {
		log_.Warningf("Variable %s not found.", name.c_str());
	}
	return default_value;
}

int RuntimeVariableScope::GetDefaultValue(GetMode mode, const str& name, RuntimeVariable* variable, int default_value) {
	if (variable) {
		if (mode == kReadDefault) {
			return variable->GetDefaultIntValue();
		} else if (mode == kReadWrite) {
			variable->SetDefaultIntValue(default_value);
		}
		return variable->GetIntValue();
	} else if (mode == kReadWrite) {
		SetValue(RuntimeVariable::kUsageNormal, name, default_value);
	} else if (mode != kReadIgnore) {
		log_.Warningf("Variable %s not found.", name.c_str());
	}
	return default_value;
}

double RuntimeVariableScope::GetDefaultValue(GetMode mode, const str& name, RuntimeVariable* variable, double default_value) {
	if (variable) {
		if (mode == kReadDefault) {
			return variable->GetDefaultRealValue();
		} else if (mode == kReadWrite) {
			variable->SetDefaultRealValue(default_value);
		}
		return variable->GetRealValue();
	} else if (mode == kReadWrite) {
		SetValue(RuntimeVariable::kUsageNormal, name, default_value);
	} else if (mode != kReadIgnore) {
		log_.Warningf("Variable %s not found.", name.c_str());
	}
	return default_value;
}



void RuntimeVariableScope::CreateLocalVariable(const str& name, DataType type, const str& str_value, bool bool_value, int int_value, double double_value, SetMode set_mode) {
	RuntimeVariable* _variable = 0;
	switch (type) {
		case RuntimeVariable::kDatatypeString:	_variable = new RuntimeVariable(name, str_value, set_mode);	break;
		case RuntimeVariable::kDatatypeBool:	_variable = new RuntimeVariable(name, bool_value, set_mode);	break;
		case RuntimeVariable::kDatatypeInt:	_variable = new RuntimeVariable(name, int_value, set_mode);	break;
		case RuntimeVariable::kDatatypeReal:	_variable = new RuntimeVariable(name, double_value, set_mode);	break;
	}
	ScopeSpinLock lock(&lock_);
	variable_table_.insert(VariableTable::value_type(name, _variable));
}

bool RuntimeVariableScope::DeleteLocalVariable(const str& name) {
	bool deleted = false;
	ScopeSpinLock lock(&lock_);
	VariableTable::iterator x = variable_table_.find(name);
	if (x != variable_table_.end()) {
		RuntimeVariable* _variable = x->second;
		variable_table_.erase(x);
		delete (_variable);
		deleted = true;
	}
	return (deleted);
}

RuntimeVariable* RuntimeVariableScope::GetVariable(const HashedString& name, bool recursive) const {
	if (name.value_ && name.value_expire_count_ == owner_seed_) {
		return (RuntimeVariable*)name.value_;
	}

	// We must assume the stored value in the HashedString can be found in the parent class (with a different "owner seed"),
	// or we're dealing with either a rebooted application or a variable that simply can't be found.
	{
		ScopeSpinLock lock(&lock_);
		VariableTable::const_iterator x = variable_table_.find(name);
		if (x != variable_table_.end()) {
			name.value_ = x->second;
			name.value_expire_count_ = owner_seed_;
			return x->second;
		}
	}
	if (recursive && parent_scope_) {
		RuntimeVariable* _variable = parent_scope_->GetVariable(name, recursive);
		if (_variable) {
			return _variable;
		}
	}
	return 0;
}



RuntimeVariableCompleter::RuntimeVariableCompleter(RuntimeVariableScope* variable_scope, const str& prefix):
	variable_scope_(variable_scope),
	prefix_(prefix) {
}

RuntimeVariableCompleter::~RuntimeVariableCompleter() {
	variable_scope_ = 0;
}

std::list<str> RuntimeVariableCompleter::CompleteCommand(const str& partial_command) const {
	std::list<str> completion_list;
	if (partial_command.compare(0, prefix_.length(), prefix_) == 0) {
		const str partial_variable_name = partial_command.substr(prefix_.length());
		std::list<str> variable_list = variable_scope_->GetVariableNameList(RuntimeVariableScope::kSearchExportable);
		std::list<str>::const_iterator x = variable_list.begin();
		for (; x != variable_list.end(); ++x) {
			const str& completion = *x;
			if (strutil::StartsWith(completion, partial_variable_name)) {
				// Add to list.
				completion_list.push_back(prefix_+completion);
			}
		}
	}
	return (completion_list);
}



loginstance(kGeneral, RuntimeVariable);
loginstance(kGeneral, RuntimeVariableScope);



}
