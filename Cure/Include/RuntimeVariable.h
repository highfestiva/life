
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
	RuntimeVariable(const Lepra::String& pName, const Lepra::String& pValue, bool pExport);
	~RuntimeVariable();
	const Lepra::String& GetName() const;
	bool operator==(const Lepra::String& pValue);
	const Lepra::String& GetValue() const;
	void SetValue(const Lepra::String& pValue, bool pOverwriteDefault, bool pExport);
	const Lepra::String& GetDefaultValue() const;
	void SetDefaultValue(const Lepra::String& pDefaultValue);
	bool IsExportable() const;

private:
	void operator=(const RuntimeVariable&);

	Lepra::String mName;
	Lepra::String mValue;
	Lepra::String mDefaultValue;
	bool mIsExportable;
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
	enum SetMode
	{
		SET_OVERWRITE	= 1,
		SET_OVERRIDE,
		SET_INTERNAL,
	};

	RuntimeVariableScope(RuntimeVariableScope* pParentScope);
	virtual ~RuntimeVariableScope();

	static Lepra::String GetType(const Lepra::String& pValue);
	static Lepra::String Cast(const Lepra::String& pValue);
	bool IsDefined(const Lepra::String& pName);
	bool SetValue(SetMode pSetMode, const Lepra::String& pName, const Lepra::String& pValue);
	bool SetValue(SetMode pSetMode, const Lepra::String& pName, const Lepra::tchar* pValue);	// TRICKY: required for _T("") parameter to work.
	bool SetValue(SetMode pSetMode, const Lepra::String& pName, double pValue);
	bool SetValue(SetMode pSetMode, const Lepra::String& pName, int pValue);
	bool SetValue(SetMode pSetMode, const Lepra::String& pName, bool pValue);
	// Returns the parameter default value if the runtime variable is not found.
	const Lepra::String& GetDefaultValue(GetMode pMode, const Lepra::String& pName, const Lepra::String& pDefaultValue = Lepra::EmptyString);
	const Lepra::String GetDefaultValue(GetMode pMode, const Lepra::String& pName, const Lepra::tchar* pDefaultValue);	// TRICKY: required for _T("") parameter to work.
	double GetDefaultValue(GetMode pMode, const Lepra::String& pName, double pDefaultValue);
	int GetDefaultValue(GetMode pMode, const Lepra::String& pName, int pDefaultValue);
	bool GetDefaultValue(GetMode pMode, const Lepra::String& pName, bool pDefaultValue);

	bool EraseVariable(const Lepra::String& pName);

	RuntimeVariableScope* LockParentScope(RuntimeVariableScope* pParentScope);

	std::list<Lepra::String> GetVariableNameList(int pStartScopeIndex = 0, int pEndScopeIndex = 1000);

private:
	void CreateLocalVariable(const Lepra::String& pName, const Lepra::String& pValue, bool pExport);
	bool DeleteLocalVariable(const Lepra::String& pName);

	RuntimeVariable* GetVariable(const Lepra::String& pName, bool pRecursive = true) const;

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



#define CURE_RTVAR_GETSET(scope, name, def)		(scope)->GetDefaultValue(Cure::RuntimeVariableScope::READ_WRITE, _T(name), def)
#define CURE_RTVAR_GET(scope, name, def)		(scope)->GetDefaultValue(Cure::RuntimeVariableScope::READ_ONLY, _T(name), def)
#define CURE_RTVAR_TRYGET(scope, name, def)		(scope)->GetDefaultValue(Cure::RuntimeVariableScope::READ_IGNORE, _T(name), def)
#define CURE_RTVAR_GET_DEFAULT(scope, name, def)	(scope)->GetDefaultValue(Cure::RuntimeVariableScope::READ_DEFAULT, _T(name), def)
#define CURE_RTVAR_SET(scope, name, value)		(scope)->SetValue(Cure::RuntimeVariableScope::SET_OVERWRITE, _T(name), value)
#define CURE_RTVAR_OVERRIDE(scope, name, value)		(scope)->SetValue(Cure::RuntimeVariableScope::SET_OVERRIDE, _T(name), value)
#define CURE_RTVAR_INTERNAL(scope, name, value)		(scope)->SetValue(Cure::RuntimeVariableScope::SET_INTERNAL, _T(name), value)
#define CURE_RTVAR_INTERNAL_ARITHMETIC(scope, name, type, arith, value, min)	\
{										\
	type _new_val = CURE_RTVAR_TRYGET(scope, name, min) arith value;	\
	_new_val = (_new_val >= min)? _new_val : min;				\
	CURE_RTVAR_INTERNAL(scope, name, _new_val);				\
}



}
