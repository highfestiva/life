
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
	enum Usage
	{
		USAGE_NORMAL	= 1,
		USAGE_OVERRIDE,
		USAGE_INTERNAL,
	};

	RuntimeVariable(const str& pName, const str& pValue, Usage pUsage);
	~RuntimeVariable();
	const str& GetName() const;
	bool operator==(const str& pValue);
	const str& GetValue() const;
	bool GetBoolValue() const;
	int GetIntValue() const;
	double GetRealValue() const;
	void SetValue(const str& pValue, Usage pUsage);
	const str& GetDefaultValue() const;
	void SetDefaultValue(const str& pDefaultValue);
	Usage GetUsage() const;

private:
	void operator=(const RuntimeVariable&);

	enum DataType
	{
		DATATYPE_STRING,
		DATATYPE_BOOL,
		DATATYPE_INT,
		DATATYPE_REAL,
	};

	str mName;
	Usage mUsage;
	DataType mDataType;
	str mValue;
	bool mBoolValue;
	int mIntValue;
	double mRealValue;
	str mDefaultValue;

	LOG_CLASS_DECLARE();
};



class RuntimeVariableScope
{
public:
	enum GetMode
	{
		READ_WRITE	= 1,
		READ_ONLY,
		READ_DEFAULT,
		READ_IGNORE,
	};
	enum SearchType
	{
		SEARCH_EXPORTABLE	= 1,
		SEARCH_ALL,
	};
	typedef RuntimeVariable::Usage SetMode;

	RuntimeVariableScope(RuntimeVariableScope* pParentScope);
	virtual ~RuntimeVariableScope();

	static str GetType(const str& pValue);
	static str Cast(const str& pValue);
	bool IsDefined(const str& pName);
	bool SetValue(SetMode pSetMode, const str& pName, const str& pValue);
	bool SetValue(SetMode pSetMode, const str& pName, const tchar* pValue);	// TRICKY: required for _T("") parameter to work.
	bool SetValue(SetMode pSetMode, const str& pName, double pValue);
	bool SetValue(SetMode pSetMode, const str& pName, int pValue);
	bool SetValue(SetMode pSetMode, const str& pName, bool pValue);
	// Returns the parameter default value if the runtime variable is not found.
	const str& GetDefaultValue(GetMode pMode, const HashedString& pName, const str& pDefaultValue = EmptyString);
	const str GetDefaultValue(GetMode pMode, const HashedString& pName, const tchar* pDefaultValue);	// TRICKY: required for _T("") parameter to work.
	double GetDefaultValue(GetMode pMode, const HashedString& pName, double pDefaultValue);
	int GetDefaultValue(GetMode pMode, const HashedString& pName, int pDefaultValue);
	bool GetDefaultValue(GetMode pMode, const HashedString& pName, bool pDefaultValue);

	bool EraseVariable(const str& pName);

	std::list<str> GetVariableNameList(SearchType pSearchType, int pStartScopeIndex = 0, int pEndScopeIndex = 1000);

private:
	const str& GetDefaultValue(GetMode pMode, const str& pName, RuntimeVariable* pVariable, const str& pDefaultValue);
	double GetDefaultValue(GetMode pMode, const str& pName, RuntimeVariable* pVariable, double pDefaultValue);
	int GetDefaultValue(GetMode pMode, const str& pName, RuntimeVariable* pVariable, int pDefaultValue);
	bool GetDefaultValue(GetMode pMode, const str& pName, RuntimeVariable* pVariable, bool pDefaultValue);

	void CreateLocalVariable(const str& pName, const str& pValue, SetMode pSetMode);
	bool DeleteLocalVariable(const str& pName);

	RuntimeVariable* GetVariable(const HashedString& pName, bool pRecursive = true) const;

	typedef std::hash_map<HashedString, RuntimeVariable*> VariableTable;
	typedef std::pair<HashedString, RuntimeVariable*> VariablePair;
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



#define CURE_RTVAR_TOKEN(var, name)				static const HashedString __HashedString_##var(_T(name));
#define CURE_RTVAR_SLOW_GET(scope, name, def)			(scope)->GetDefaultValue(Cure::RuntimeVariableScope::READ_ONLY, _T(name), def)
#define CURE_RTVAR_SLOW_TRYGET(scope, name, def)		(scope)->GetDefaultValue(Cure::RuntimeVariableScope::READ_IGNORE, _T(name), def)
#define CURE_RTVAR_GET(var, op, scope, name, def)		CURE_RTVAR_TOKEN(var, name); var op (scope)->GetDefaultValue(Cure::RuntimeVariableScope::READ_ONLY, _T(name), def)
#define CURE_RTVAR_TRYGET(var, op, scope, name, def)		CURE_RTVAR_TOKEN(var, name); var op (scope)->GetDefaultValue(Cure::RuntimeVariableScope::READ_IGNORE, _T(name), def)
#define CURE_RTVAR_SET(scope, name, value)			(scope)->SetValue(Cure::RuntimeVariable::USAGE_NORMAL, _T(name), value)
#define CURE_RTVAR_OVERRIDE(scope, name, value)			(scope)->SetValue(Cure::RuntimeVariable::USAGE_OVERRIDE, _T(name), value)
#define CURE_RTVAR_INTERNAL(scope, name, value)			(scope)->SetValue(Cure::RuntimeVariable::USAGE_INTERNAL, _T(name), value)
#define CURE_RTVAR_INTERNAL_ARITHMETIC(scope, name, type, arith, value, min)	\
{										\
	type _new_val;								\
	CURE_RTVAR_TRYGET(_new_val, =, scope, name, min) arith value;		\
	_new_val = (_new_val >= min)? _new_val : min;				\
	CURE_RTVAR_INTERNAL(scope, name, _new_val);				\
}



}
