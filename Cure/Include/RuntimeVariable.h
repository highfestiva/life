
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
	enum Type
	{
		TYPE_NORMAL	= 1,
		TYPE_OVERRIDE,
		TYPE_INTERNAL,
	};

	RuntimeVariable(const str& pName, const str& pValue, Type pType);
	~RuntimeVariable();
	const str& GetName() const;
	bool operator==(const str& pValue);
	const str& GetValue() const;
	void SetValue(const str& pValue, Type pType);
	const str& GetDefaultValue() const;
	void SetDefaultValue(const str& pDefaultValue);
	Type GetType() const;

private:
	void operator=(const RuntimeVariable&);

	str mName;
	str mValue;
	str mDefaultValue;
	Type mType;
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
	typedef RuntimeVariable::Type SetMode;

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
	const str& GetDefaultValue(GetMode pMode, const str& pName, const str& pDefaultValue = EmptyString);
	const str GetDefaultValue(GetMode pMode, const str& pName, const tchar* pDefaultValue);	// TRICKY: required for _T("") parameter to work.
	double GetDefaultValue(GetMode pMode, const str& pName, double pDefaultValue);
	int GetDefaultValue(GetMode pMode, const str& pName, int pDefaultValue);
	bool GetDefaultValue(GetMode pMode, const str& pName, bool pDefaultValue);

	bool EraseVariable(const str& pName);

	std::list<str> GetVariableNameList(SearchType pSearchType, int pStartScopeIndex = 0, int pEndScopeIndex = 1000);

private:
	void CreateLocalVariable(const str& pName, const str& pValue, SetMode pSetMode);
	bool DeleteLocalVariable(const str& pName);

	RuntimeVariable* GetVariable(const str& pName, bool pRecursive = true) const;

	typedef std::hash_map<str, RuntimeVariable*> VariableTable;
	typedef std::pair<str, RuntimeVariable*> VariablePair;
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



#define CURE_RTVAR_GETSET(scope, name, def)		(scope)->GetDefaultValue(Cure::RuntimeVariableScope::READ_WRITE, _T(name), def)
#define CURE_RTVAR_GET(scope, name, def)		(scope)->GetDefaultValue(Cure::RuntimeVariableScope::READ_ONLY, _T(name), def)
#define CURE_RTVAR_TRYGET(scope, name, def)		(scope)->GetDefaultValue(Cure::RuntimeVariableScope::READ_IGNORE, _T(name), def)
#define CURE_RTVAR_GET_DEFAULT(scope, name, def)	(scope)->GetDefaultValue(Cure::RuntimeVariableScope::READ_DEFAULT, _T(name), def)
#define CURE_RTVAR_SET(scope, name, value)		(scope)->SetValue(Cure::RuntimeVariable::TYPE_NORMAL, _T(name), value)
#define CURE_RTVAR_OVERRIDE(scope, name, value)		(scope)->SetValue(Cure::RuntimeVariable::TYPE_OVERRIDE, _T(name), value)
#define CURE_RTVAR_INTERNAL(scope, name, value)		(scope)->SetValue(Cure::RuntimeVariable::TYPE_INTERNAL, _T(name), value)
#define CURE_RTVAR_INTERNAL_ARITHMETIC(scope, name, type, arith, value, min)	\
{										\
	type _new_val = CURE_RTVAR_TRYGET(scope, name, min) arith value;	\
	_new_val = (_new_val >= min)? _new_val : min;				\
	CURE_RTVAR_INTERNAL(scope, name, _new_val);				\
}



}
