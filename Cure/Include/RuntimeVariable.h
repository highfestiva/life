
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



#pragma once

#include "../../Lepra/Include/ConsoleCommandManager.h"
#include "../../Lepra/Include/HashTable.h"
#include "../../Lepra/Include/Log.h"
#include "../../Lepra/Include/String.h"
#include "../../Lepra/Include/Thread.h"
#include "RuntimeVariableName.h"



namespace Cure
{



class RuntimeVariable
{
public:
	RuntimeVariable(const Lepra::String& pName, const Lepra::String& pValue);
	~RuntimeVariable();
	const Lepra::String& GetName() const;
	bool operator==(const Lepra::String& pValue);
	const Lepra::String& GetValue() const;
	void SetValue(const Lepra::String& pValue);
	const Lepra::String& GetDefaultValue() const;

private:
	void operator=(const RuntimeVariable&);

	Lepra::String mName;
	Lepra::String mValue;
	const Lepra::String mDefaultValue;
};



class RuntimeVariableScope
{
public:
	enum GetMode
	{
		READ_WRITE	= 1,
		READ_ONLY,
		READ_DEFAULT,
	};

	RuntimeVariableScope(RuntimeVariableScope* pParentScope);
	virtual ~RuntimeVariableScope();

	static Lepra::String GetType(const Lepra::String& pValue);
	static Lepra::String Cast(const Lepra::String& pValue);
	bool IsDefined(const Lepra::String& pName);
	bool SetValue(const Lepra::String& pName, const Lepra::String& pValue);
	bool SetValue(const Lepra::String& pName, const Lepra::tchar* pValue);	// TRICKY: required for _T("") parameter to work.
	bool SetValue(const Lepra::String& pName, double pValue);
	bool SetValue(const Lepra::String& pName, int pValue);
	bool SetValue(const Lepra::String& pName, bool pValue);
	// Returns the parameter default value if the runtime variable is not found.
	const Lepra::String& GetDefaultValue(const Lepra::String& pName, GetMode pMode, const Lepra::String& pDefaultValue = Lepra::EmptyString);
	const Lepra::String GetDefaultValue(const Lepra::String& pName, GetMode pMode, const Lepra::tchar* pDefaultValue);	// TRICKY: required for _T("") parameter to work.
	double GetDefaultValue(const Lepra::String& pName, GetMode pMode, double pDefaultValue);
	int GetDefaultValue(const Lepra::String& pName, GetMode pMode, int pDefaultValue);
	bool GetDefaultValue(const Lepra::String& pName, GetMode pMode, bool pDefaultValue);

	bool EraseVariable(const Lepra::String& pName);

	RuntimeVariableScope* LockParentScope(RuntimeVariableScope* pParentScope);

	std::list<Lepra::String> GetVariableNameList(int pScopeCount = -1);

private:
	void CreateLocalVariable(const Lepra::String& pName, const Lepra::String& pValue);
	bool DeleteLocalVariable(const Lepra::String& pName);

	RuntimeVariable* GetVariable(const Lepra::String& pName) const;

	typedef std::hash_map<Lepra::String, RuntimeVariable*> VariableTable;
	typedef std::pair<Lepra::String, RuntimeVariable*> VariablePair;
	RuntimeVariableScope* mParentScope;
	mutable Lepra::Lock mLock;
	VariableTable mVariableTable;

	LOG_CLASS_DECLARE();
};



// For adding variables to console completions.
class RuntimeVariableCompleter: public Lepra::CommandCompleter
{
public:
	RuntimeVariableCompleter(RuntimeVariableScope* pVariableScope, const Lepra::String& pPrefix);
	virtual ~RuntimeVariableCompleter();

private:
	std::list<Lepra::String> CompleteCommand(const Lepra::String& pPartialCommand) const;

	RuntimeVariableScope* mVariableScope;
	Lepra::String mPrefix;
};



#define CURE_RTVAR_GETSET(scope, name, def)		(scope)->GetDefaultValue(_T(name), Cure::RuntimeVariableScope::READ_WRITE, def)
#define CURE_RTVAR_GET(scope, name, def)		(scope)->GetDefaultValue(_T(name), Cure::RuntimeVariableScope::READ_ONLY, def)
#define CURE_RTVAR_GET_DEFAULT(scope, name, def)	(scope)->GetDefaultValue(_T(name), Cure::RuntimeVariableScope::READ_DEFAULT, def)
#define CURE_RTVAR_SET(scope, name, value)		(scope)->SetValue(_T(name), value)



}
