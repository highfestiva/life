
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../../Lepra/Include/ConsoleCommandManager.h"
#include "../../Lepra/Include/HashTable.h"
#include "../../Lepra/Include/Log.h"
#include "../../Lepra/Include/String.h"
#include "../../Lepra/Include/Thread.h"
#include "Cure.h"
#include "RuntimeVariableName.h"



namespace Cure
{



class RuntimeVariable
{
public:
	enum DataType
	{
		DATATYPE_STRING,
		DATATYPE_BOOL,
		DATATYPE_INT,
		DATATYPE_REAL,
	};
	enum Usage
	{
		USAGE_NORMAL	= 1,
		USAGE_USER_OVERRIDE,
		USAGE_SYS_OVERRIDE,
		USAGE_INTERNAL,
	};

	RuntimeVariable(const str& pName, const str& pValue, Usage pUsage);
	RuntimeVariable(const str& pName, bool pValue, Usage pUsage);
	RuntimeVariable(const str& pName, int pValue, Usage pUsage);
	RuntimeVariable(const str& pName, double pValue, Usage pUsage);
	~RuntimeVariable();
	const str& GetName() const;
	//bool operator==(const str& pValue);
	DataType GetType() const;
	const str& GetStrValue() const;
	bool GetBoolValue() const;
	int GetIntValue() const;
	double GetRealValue() const;
	bool SetStrValue(const str& pValue, Usage pUsage);
	bool SetBoolValue(bool pValue, Usage pUsage);
	bool SetIntValue(int pValue, Usage pUsage);
	bool SetRealValue(double pValue, Usage pUsage);
	const str& GetDefaultStrValue() const;
	bool GetDefaultBoolValue() const;
	int GetDefaultIntValue() const;
	double GetDefaultRealValue() const;
	void SetDefaultStrValue(const str& pDefaultValue);
	void SetDefaultBoolValue(bool pValue);
	void SetDefaultIntValue(int pValue);
	void SetDefaultRealValue(double pValue);
	Usage GetUsage() const;
	static str GetTypeName(DataType pType);

private:
	bool CheckType(DataType pType) const;

	void operator=(const RuntimeVariable&);

	str mName;
	Usage mUsage;
	DataType mDataType;
	str mStrValue;
	bool mBoolValue;
	int mIntValue;
	double mRealValue;
	str mDefaultStrValue;
	bool mDefaultBoolValue;
	int mDefaultIntValue;
	double mDefaultRealValue;

	LOG_CLASS_DECLARE();
};



class RuntimeVariableScope
{
public:
	enum GetMode
	{
		READ_WRITE = 1,
		READ_ONLY,
		READ_DEFAULT,
		READ_IGNORE,
	};
	enum SearchType
	{
		SEARCH_EXPORTABLE = 1,
		SEARCH_ALL,
	};
	typedef RuntimeVariable::Usage SetMode;
	typedef RuntimeVariable::DataType DataType;

	RuntimeVariableScope(RuntimeVariableScope* pParentScope);
	virtual ~RuntimeVariableScope();

	bool IsDefined(const str& pName);
	bool SetUntypedValue(SetMode pSetMode, const str& pName, const str& pValue);
	bool SetValue(SetMode pSetMode, const str& pName, const str& pValue);
	bool SetValue(SetMode pSetMode, const str& pName, const tchar* pValue);	// TRICKY: required for _T("") parameter to work.
	bool SetValue(SetMode pSetMode, const str& pName, bool pValue);
	bool SetValue(SetMode pSetMode, const str& pName, int pValue);
	bool SetValue(SetMode pSetMode, const str& pName, double pValue);
	// Returns the parameter default value if the runtime variable is not found.
	str GetUntypedDefaultValue(GetMode pMode, const str& pName);
	const str& GetDefaultValue(GetMode pMode, const HashedString& pName, const str& pDefaultValue = EmptyString);
	const str GetDefaultValue(GetMode pMode, const HashedString& pName, const tchar* pDefaultValue);	// TRICKY: required for _T("") parameter to work.
	bool GetDefaultValue(GetMode pMode, const HashedString& pName, bool pDefaultValue);
	int GetDefaultValue(GetMode pMode, const HashedString& pName, int pDefaultValue);
	double GetDefaultValue(GetMode pMode, const HashedString& pName, double pDefaultValue);

	bool EraseVariable(const str& pName);

	std::list<str> GetVariableNameList(SearchType pSearchType, int pStartScopeIndex = 0, int pEndScopeIndex = 1000);

	static DataType GetUntypedType(const str& pValue);

private:
	static DataType GetType(const str& pValue, str& pOutStr, bool& pOutBool, int& pOutInt, double& pOutDouble);
	static DataType Cast(const str& pValue, str& pOutStr, bool& pOutBool, int& pOutInt, double& pOutDouble);

	const str& GetDefaultValue(GetMode pMode, const str& pName, RuntimeVariable* pVariable, const str& pDefaultValue);
	bool GetDefaultValue(GetMode pMode, const str& pName, RuntimeVariable* pVariable, bool pDefaultValue);
	int GetDefaultValue(GetMode pMode, const str& pName, RuntimeVariable* pVariable, int pDefaultValue);
	double GetDefaultValue(GetMode pMode, const str& pName, RuntimeVariable* pVariable, double pDefaultValue);

	void CreateLocalVariable(const str& pName, DataType pType, const str& pStrValue, bool pBoolValue, int pIntValue, double pDoubleValue, SetMode pSetMode);
	bool DeleteLocalVariable(const str& pName);

	RuntimeVariable* GetVariable(const HashedString& pName, bool pRecursive = true) const;

	typedef std::hash_map<HashedString, RuntimeVariable*> VariableTable;
	RuntimeVariableScope* mParentScope;
	mutable Lock mLock;
	VariableTable mVariableTable;

	LOG_CLASS_DECLARE();
};



// For adding variables to console completions.
class RuntimeVariableCompleter: public CommandCompleter
{
public:
	RuntimeVariableCompleter(RuntimeVariableScope* pVariableScope, const str& pPrefix);
	virtual ~RuntimeVariableCompleter();

private:
	std::list<str> CompleteCommand(const str& pPartialCommand) const;

	RuntimeVariableScope* mVariableScope;
	str mPrefix;
};



#define CURE_RTVAR_TOKEN(var, name)				static const HashedString hs_##var(_T(name));
#define CURE_RTVAR_SLOW_GET(scope, name, def)			(scope)->GetDefaultValue(Cure::RuntimeVariableScope::READ_ONLY, _T(name), def)
#define CURE_RTVAR_SLOW_TRYGET(scope, name, def)		(scope)->GetDefaultValue(Cure::RuntimeVariableScope::READ_IGNORE, _T(name), def)
#define CURE_RTVAR_GET(var, op, scope, name, def)		CURE_RTVAR_TOKEN(var, name); var op (scope)->GetDefaultValue(Cure::RuntimeVariableScope::READ_ONLY, hs_##var, def)
#define CURE_RTVAR_TRYGET(var, op, scope, name, def)		CURE_RTVAR_TOKEN(t##var, name); var op (scope)->GetDefaultValue(Cure::RuntimeVariableScope::READ_IGNORE, hs_t##var, def)
#define CURE_RTVAR_SET(scope, name, value)			(scope)->SetValue(Cure::RuntimeVariable::USAGE_NORMAL, _T(name), value)
#define CURE_RTVAR_SET_IF_NOT_SET(scope, name, value)		CURE_RTVAR_SET(scope, name, CURE_RTVAR_SLOW_TRYGET(scope, name, value))
#define CURE_RTVAR_SYS_OVERRIDE(scope, name, value)		(scope)->SetValue(Cure::RuntimeVariable::USAGE_SYS_OVERRIDE, _T(name), value)
#define CURE_RTVAR_INTERNAL(scope, name, value)			(scope)->SetValue(Cure::RuntimeVariable::USAGE_INTERNAL, _T(name), value)
#define CURE_RTVAR_BASE_ARITHMETIC(OP_GET, OP_SET, scope, name, type, arith, value, vmin, vmax)	\
{												\
	type _new_val;										\
	OP_GET(_new_val, =, scope, name, vmin) arith value;					\
	_new_val = std::min(_new_val, vmax);							\
	_new_val = std::max(_new_val, vmin);							\
	OP_SET(scope, name, _new_val);								\
}
#define CURE_RTVAR_ARITHMETIC(scope, name, type, arith, value, min, max)		CURE_RTVAR_BASE_ARITHMETIC(CURE_RTVAR_GET, CURE_RTVAR_SET, scope, name, type, arith, value, min, max)
#define CURE_RTVAR_INTERNAL_ARITHMETIC(scope, name, type, arith, value, min, max)	CURE_RTVAR_BASE_ARITHMETIC(CURE_RTVAR_TRYGET, CURE_RTVAR_INTERNAL, scope, name, type, arith, value, min, max)



}
