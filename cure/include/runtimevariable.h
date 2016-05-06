
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../lepra/include/consolecommandmanager.h"
#include "../../lepra/include/hashtable.h"
#include "../../lepra/include/log.h"
#include "../../lepra/include/spinlock.h"
#include "../../lepra/include/string.h"
#include "cure.h"
#include "runtimevariablename.h"



namespace cure {



class RuntimeVariable {
public:
	enum DataType {
		kDatatypeString,
		kDatatypeBool,
		kDatatypeInt,
		kDatatypeReal,
	};
	enum Usage {
		kUsageNormal	= 1,
		kUsageUserOverride,
		kUsageSysOverride,
		kUsageInternal,
	};

	RuntimeVariable(const str& _name, const str& _value, Usage pUsage);
	RuntimeVariable(const str& _name, bool _value, Usage pUsage);
	RuntimeVariable(const str& _name, int _value, Usage pUsage);
	RuntimeVariable(const str& _name, double _value, Usage pUsage);
	~RuntimeVariable();
	const str& GetName() const;
	//bool operator==(const str& _value);
	DataType GetType() const;
	const str& GetStrValue() const;
	bool GetBoolValue() const;
	int GetIntValue() const;
	double GetRealValue() const;
	bool SetStrValue(const str& _value, Usage pUsage);
	bool SetBoolValue(bool _value, Usage pUsage);
	bool SetIntValue(int _value, Usage pUsage);
	bool SetRealValue(double _value, Usage pUsage);
	const str& GetDefaultStrValue() const;
	bool GetDefaultBoolValue() const;
	int GetDefaultIntValue() const;
	double GetDefaultRealValue() const;
	void SetDefaultStrValue(const str& default_value);
	void SetDefaultBoolValue(bool _value);
	void SetDefaultIntValue(int _value);
	void SetDefaultRealValue(double _value);
	Usage GetUsage() const;
	void Reset();
	static str GetTypeName(DataType _type);

private:
	bool CheckType(DataType _type) const;

	void operator=(const RuntimeVariable&);

	str name_;
	Usage usage_;
	DataType data_type_;
	str str_value_;
	bool bool_value_;
	int int_value_;
	double real_value_;
	str default_str_value_;
	bool default_bool_value_;
	int default_int_value_;
	double default_real_value_;

	logclass();
};



class RuntimeVariableScope {
public:
	enum GetMode {
		kReadWrite = 1,
		kReadOnly,
		kReadDefault,
		kReadIgnore,
	};
	enum SearchType {
		kSearchExportable = 1,
		kSearchAll,
	};
	typedef RuntimeVariable::Usage SetMode;
	typedef RuntimeVariable::DataType DataType;

	RuntimeVariableScope(RuntimeVariableScope* parent_scope);
	virtual ~RuntimeVariableScope();

	bool IsDefined(const str& _name);
	bool SetUntypedValue(SetMode set_mode, const str& _name, const str& _value);
	bool SetValue(SetMode set_mode, const str& _name, const str& _value);
	bool SetValue(SetMode set_mode, const str& _name, const char* _value);	// TRICKY: required for "" parameter to work.
	bool SetValue(SetMode set_mode, const str& _name, bool _value);
	bool SetValue(SetMode set_mode, const str& _name, int _value);
	bool SetValue(SetMode set_mode, const str& _name, double _value);
	// Returns the parameter default value if the runtime variable is not found.
	str GetUntypedDefaultValue(GetMode mode, const str& _name);
	const str& GetDefaultValue(GetMode mode, const HashedString& _name, const str& default_value = EmptyString);
	const str GetDefaultValue(GetMode mode, const HashedString& _name, const char* default_value);	// TRICKY: required for "" parameter to work.
	bool GetDefaultValue(GetMode mode, const HashedString& _name, bool default_value);
	int GetDefaultValue(GetMode mode, const HashedString& _name, int default_value);
	double GetDefaultValue(GetMode mode, const HashedString& _name, double default_value);

	bool EraseVariable(const str& _name);
	bool ResetDefaultValue(const str& _name);

	std::list<str> GetVariableNameList(SearchType search_type, int start_scope_index = 0, int end_scope_index = 1000);

	static DataType GetUntypedType(const str& _value);

private:
	static DataType GetType(const str& _value, str& out_str, bool& out_bool, int& out_int, double& out_double);
	static DataType Cast(const str& _value, str& out_str, bool& out_bool, int& out_int, double& out_double);

	const str& GetDefaultValue(GetMode mode, const str& _name, RuntimeVariable* variable, const str& default_value);
	bool GetDefaultValue(GetMode mode, const str& _name, RuntimeVariable* variable, bool default_value);
	int GetDefaultValue(GetMode mode, const str& _name, RuntimeVariable* variable, int default_value);
	double GetDefaultValue(GetMode mode, const str& _name, RuntimeVariable* variable, double default_value);

	void CreateLocalVariable(const str& _name, DataType _type, const str& str_value, bool bool_value, int int_value, double double_value, SetMode set_mode);
	bool DeleteLocalVariable(const str& _name);

	RuntimeVariable* GetVariable(const HashedString& _name, bool recursive = true) const;

	typedef std::unordered_map<HashedString, RuntimeVariable*, HashedStringHasher> VariableTable;
	RuntimeVariableScope* parent_scope_;
	mutable SpinLock lock_;
	VariableTable variable_table_;
	int owner_seed_;

	logclass();
};



// For adding variables to console completions.
class RuntimeVariableCompleter: public CommandCompleter {
public:
	RuntimeVariableCompleter(RuntimeVariableScope* variable_scope, const str& prefix);
	virtual ~RuntimeVariableCompleter();

private:
	std::list<str> CompleteCommand(const str& partial_command) const;

	RuntimeVariableScope* variable_scope_;
	str prefix_;
};



#define v_token(var, name)			static const HashedString hs_##var(name);
#define v_slowget(scope, name, def)		(scope)->GetDefaultValue(cure::RuntimeVariableScope::kReadOnly, name, def)
#define v_slowtryget(scope, name, def)		(scope)->GetDefaultValue(cure::RuntimeVariableScope::kReadIgnore, name, def)
#define v_get(var, op, scope, name, def)	v_token(var, name); var op (scope)->GetDefaultValue(cure::RuntimeVariableScope::kReadOnly, hs_##var, def)
#define v_tryget(var, op, scope, name, def)	v_token(t##var, name); var op (scope)->GetDefaultValue(cure::RuntimeVariableScope::kReadIgnore, hs_t##var, def)
#define v_set(scope, name, value)		(scope)->SetValue(cure::RuntimeVariable::kUsageNormal, name, value)
#define v_override(scope, name, value)		(scope)->SetValue(cure::RuntimeVariable::kUsageSysOverride, name, value)
#define v_internal(scope, name, value)		(scope)->SetValue(cure::RuntimeVariable::kUsageInternal, name, value)
#define v_base_arithmetic(OP_GET, OP_SET, scope, name, type, arith, value, vmin, vmax)	\
{											\
	type _new_val;									\
	OP_GET(_new_val, =, scope, name, vmin) arith value;				\
	_new_val = std::min(_new_val, vmax);						\
	_new_val = std::max(_new_val, vmin);						\
	OP_SET(scope, name, _new_val);							\
}
#define v_arithmetic(scope, name, type, arith, value, min, max)			v_base_arithmetic(v_get, v_set, scope, name, type, arith, value, min, max)
#define v_internal_arithmetic(scope, name, type, arith, value, min, max)	v_base_arithmetic(v_tryget, v_internal, scope, name, type, arith, value, min, max)



}
