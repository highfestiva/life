
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

// Warning: optimized code.



#include "../../Lepra/Include/LepraAssert.h"
#include "../../Lepra/Include/HashUtil.h"
#include "../Include/RuntimeVariable.h"



namespace Cure
{



RuntimeVariable::RuntimeVariable(const str& pName, const str& pValue, Usage pUsage):
	mName(pName),
	mRealValue(0),
	mDefaultStrValue(pValue),
	mDefaultRealValue(0)
{
	mDataType = RuntimeVariable::DATATYPE_STRING;
	SetStrValue(pValue, pUsage);
}

RuntimeVariable::RuntimeVariable(const str& pName, bool pValue, Usage pUsage):
	mName(pName),
	mRealValue(0),
	mDefaultBoolValue(pValue),
	mDefaultRealValue(0)
{
	mDataType = RuntimeVariable::DATATYPE_BOOL;
	SetBoolValue(pValue, pUsage);
}

RuntimeVariable::RuntimeVariable(const str& pName, int pValue, Usage pUsage):
	mName(pName),
	mRealValue(0),
	mDefaultIntValue(pValue),
	mDefaultRealValue(0)
{
	mDataType = RuntimeVariable::DATATYPE_INT;
	SetIntValue(pValue, pUsage);
}

RuntimeVariable::RuntimeVariable(const str& pName, double pValue, Usage pUsage):
	mName(pName),
	mDefaultRealValue(pValue)
{
	mDataType = RuntimeVariable::DATATYPE_REAL;
	SetRealValue(pValue, pUsage);
}

RuntimeVariable::~RuntimeVariable()
{
}

const str& RuntimeVariable::GetName() const
{
	return mName;
}

RuntimeVariable::DataType RuntimeVariable::GetType() const
{
	return mDataType;
}

//bool RuntimeVariable::operator==(const str& pValue)
//{
//	return (mStrValue == pValue);
//}

const str& RuntimeVariable::GetStrValue() const
{
	CheckType(DATATYPE_STRING);
	return mStrValue;
}

bool RuntimeVariable::GetBoolValue() const
{
	CheckType(DATATYPE_BOOL);
	return mBoolValue;
}

int RuntimeVariable::GetIntValue() const
{
	CheckType(DATATYPE_INT);
	return mIntValue;
}

double RuntimeVariable::GetRealValue() const
{
	CheckType(DATATYPE_REAL);
	return mRealValue;
}

bool RuntimeVariable::SetStrValue(const str& pValue, Usage pUsage)
{
	if (!CheckType(DATATYPE_STRING))
	{
		return false;
	}
	mStrValue = pValue;
	if (pUsage >= USAGE_SYS_OVERRIDE)
	{
		mDefaultStrValue = pValue;
	}
	mUsage = pUsage;
	return true;
}

bool RuntimeVariable::SetBoolValue(bool pValue, Usage pUsage)
{
	if (!CheckType(DATATYPE_BOOL))
	{
		return false;
	}
	mBoolValue = pValue;
	if (pUsage >= USAGE_SYS_OVERRIDE)
	{
		mDefaultBoolValue = pValue;
	}
	mUsage = pUsage;
	return true;
}

bool RuntimeVariable::SetIntValue(int pValue, Usage pUsage)
{
	if (!CheckType(DATATYPE_INT))
	{
		return false;
	}
	mIntValue = pValue;
	if (pUsage >= USAGE_SYS_OVERRIDE)
	{
		mDefaultIntValue = pValue;
	}
	mUsage = pUsage;
	return true;
}

bool RuntimeVariable::SetRealValue(double pValue, Usage pUsage)
{
	if (!CheckType(DATATYPE_REAL))
	{
		return false;
	}
	mRealValue = pValue;
	if (pUsage >= USAGE_SYS_OVERRIDE)
	{
		mDefaultRealValue = pValue;
	}
	mUsage = pUsage;
	return true;
}

const str& RuntimeVariable::GetDefaultStrValue() const
{
	CheckType(DATATYPE_STRING);
	return mDefaultStrValue;
}

bool RuntimeVariable::GetDefaultBoolValue() const
{
	CheckType(DATATYPE_BOOL);
	return mDefaultBoolValue;
}

int RuntimeVariable::GetDefaultIntValue() const
{
	CheckType(DATATYPE_INT);
	return mDefaultIntValue;
}

double RuntimeVariable::GetDefaultRealValue() const
{
	CheckType(DATATYPE_REAL);
	return mDefaultRealValue;
}

void RuntimeVariable::SetDefaultStrValue(const str& pDefaultValue)
{
	if (CheckType(DATATYPE_STRING))
	{
		mDefaultStrValue = pDefaultValue;
	}
}

void RuntimeVariable::SetDefaultBoolValue(bool pDefaultValue)
{
	if (CheckType(DATATYPE_BOOL))
	{
		mDefaultBoolValue = pDefaultValue;
	}
}

void RuntimeVariable::SetDefaultIntValue(int pDefaultValue)
{
	if (CheckType(DATATYPE_INT))
	{
		mDefaultIntValue = pDefaultValue;
	}
}

void RuntimeVariable::SetDefaultRealValue(double pDefaultValue)
{
	if (CheckType(DATATYPE_REAL))
	{
		mDefaultRealValue = pDefaultValue;
	}
}

RuntimeVariable::Usage RuntimeVariable::GetUsage() const
{
	return (mUsage);
}

str RuntimeVariable::GetTypeName(DataType pType)
{
	switch (pType)
	{
		case RuntimeVariable::DATATYPE_STRING:	return _T("string");
		case RuntimeVariable::DATATYPE_BOOL:	return _T("bool");
		case RuntimeVariable::DATATYPE_INT:	return _T("int");
		case RuntimeVariable::DATATYPE_REAL:	return _T("real");
	}
	deb_assert(false);
	return _T("void");
}

bool RuntimeVariable::CheckType(DataType pType) const
{
	if (mDataType == pType)
	{
		return true;
	}
	mLog.Warningf(_T("Type error using variable %s; type should be %s, not %s!"),
		mName.c_str(), GetTypeName(mDataType).c_str(), GetTypeName(pType).c_str());
	//deb_assert(false);
	return false;
}


RuntimeVariableScope::RuntimeVariableScope(RuntimeVariableScope* pParentScope):
	mParentScope(pParentScope)
{
}

RuntimeVariableScope::~RuntimeVariableScope()
{
	ScopeLock lLock(&mLock);
	VariableTable::iterator x = mVariableTable.begin();
	while (!mVariableTable.empty())
	{
		RuntimeVariable* lVariable = mVariableTable.begin()->second;
		DeleteLocalVariable(lVariable->GetName());
	}
	mParentScope = 0;
}

bool RuntimeVariableScope::IsDefined(const str& pName)
{
	bool lFound = (GetVariable(pName) != 0);
	return (lFound);
}

bool RuntimeVariableScope::SetUntypedValue(SetMode pSetMode, const str& pName, const str& pValue)
{
	bool lIsRecursive = (pSetMode == RuntimeVariable::USAGE_NORMAL || pSetMode == RuntimeVariable::USAGE_INTERNAL);
	RuntimeVariable* lVariable = GetVariable(pName, lIsRecursive);
	str lStrValue;
	bool lBoolValue;
	int lIntValue;
	double lRealValue;
	DataType lType = Cast(pValue, lStrValue, lBoolValue, lIntValue, lRealValue);
	if (lVariable)
	{
		switch (lType)
		{
			case RuntimeVariable::DATATYPE_STRING:	return lVariable->SetStrValue(lStrValue, pSetMode);
			case RuntimeVariable::DATATYPE_BOOL:	return lVariable->SetBoolValue(lBoolValue, pSetMode);
			case RuntimeVariable::DATATYPE_INT:	return lVariable->SetIntValue(lIntValue, pSetMode);
			case RuntimeVariable::DATATYPE_REAL:	return lVariable->SetRealValue(lRealValue, pSetMode);
		}
	}
	else
	{
		CreateLocalVariable(pName, lType, lStrValue, lBoolValue, lIntValue, lRealValue, pSetMode);
	}
	return true;
}

bool RuntimeVariableScope::SetValue(SetMode pSetMode, const str& pName, const str& pValue)
{
	bool lIsRecursive = (pSetMode == RuntimeVariable::USAGE_NORMAL || pSetMode == RuntimeVariable::USAGE_INTERNAL);
	RuntimeVariable* lVariable = GetVariable(pName, lIsRecursive);
	if (lVariable)
	{
		return lVariable->SetStrValue(pValue, pSetMode);
	}
	else
	{
		CreateLocalVariable(pName, RuntimeVariable::DATATYPE_STRING, pValue, false, 0, 0, pSetMode);
	}
	return true;
}

bool RuntimeVariableScope::SetValue(SetMode pSetMode, const str& pName, const tchar* pValue)
{
	return (SetValue(pSetMode, pName, str(pValue)));
}

bool RuntimeVariableScope::SetValue(SetMode pSetMode, const str& pName, bool pValue)
{
	bool lIsRecursive = (pSetMode == RuntimeVariable::USAGE_NORMAL || pSetMode == RuntimeVariable::USAGE_INTERNAL);
	RuntimeVariable* lVariable = GetVariable(pName, lIsRecursive);
	if (lVariable)
	{
		return lVariable->SetBoolValue(pValue, pSetMode);
	}
	else
	{
		CreateLocalVariable(pName, RuntimeVariable::DATATYPE_BOOL, str(), pValue, 0, 0, pSetMode);
	}
	return true;
}

bool RuntimeVariableScope::SetValue(SetMode pSetMode, const str& pName, int pValue)
{
	bool lIsRecursive = (pSetMode == RuntimeVariable::USAGE_NORMAL || pSetMode == RuntimeVariable::USAGE_INTERNAL);
	RuntimeVariable* lVariable = GetVariable(pName, lIsRecursive);
	if (lVariable)
	{
		return lVariable->SetIntValue(pValue, pSetMode);
	}
	else
	{
		CreateLocalVariable(pName, RuntimeVariable::DATATYPE_INT, str(), false, pValue, 0, pSetMode);
	}
	return true;
}

bool RuntimeVariableScope::SetValue(SetMode pSetMode, const str& pName, double pValue)
{
	bool lIsRecursive = (pSetMode == RuntimeVariable::USAGE_NORMAL || pSetMode == RuntimeVariable::USAGE_INTERNAL);
	RuntimeVariable* lVariable = GetVariable(pName, lIsRecursive);
	if (lVariable)
	{
		return lVariable->SetRealValue(pValue, pSetMode);
	}
	else
	{
		CreateLocalVariable(pName, RuntimeVariable::DATATYPE_REAL, str(), false, 0, pValue, pSetMode);
	}
	return true;
}

str RuntimeVariableScope::GetUntypedDefaultValue(GetMode pMode, const str& pName)
{
	RuntimeVariable* lVariable = GetVariable(HashedString(pName));
	if (!lVariable)
	{
		return EmptyString;
	}
	switch (lVariable->GetType())
	{
		case RuntimeVariable::DATATYPE_STRING:
		{
			return GetDefaultValue(pMode, pName, lVariable, EmptyString);
		}
		case RuntimeVariable::DATATYPE_BOOL:
		{
			return GetDefaultValue(pMode, pName, lVariable, false) ? _T("true") : _T("false");
		}
		case RuntimeVariable::DATATYPE_INT:
		{
			int i = GetDefaultValue(pMode, pName, lVariable, 0);
			return strutil::IntToString(i, 10);
		}
		case RuntimeVariable::DATATYPE_REAL:
		{
			double d = GetDefaultValue(pMode, pName, lVariable, 0.0);
			return strutil::Format(_T("%.1f"), d);
		}
	}
	return EmptyString;
}

const str& RuntimeVariableScope::GetDefaultValue(GetMode pMode, const HashedString& pName, const str& pDefaultValue)
{
	return GetDefaultValue(pMode, pName, GetVariable(pName), pDefaultValue);
}

const str RuntimeVariableScope::GetDefaultValue(GetMode pMode, const HashedString& pName, const tchar* pDefaultValue)
{
	return GetDefaultValue(pMode, pName, GetVariable(pName), str(pDefaultValue));
}

bool RuntimeVariableScope::GetDefaultValue(GetMode pMode, const HashedString& pName, bool pDefaultValue)
{
	return GetDefaultValue(pMode, pName, GetVariable(pName), pDefaultValue);
}

int RuntimeVariableScope::GetDefaultValue(GetMode pMode, const HashedString& pName, int pDefaultValue)
{
	return GetDefaultValue(pMode, pName, GetVariable(pName), pDefaultValue);
}

double RuntimeVariableScope::GetDefaultValue(GetMode pMode, const HashedString& pName, double pDefaultValue)
{
	return GetDefaultValue(pMode, pName, GetVariable(pName), pDefaultValue);
}

bool RuntimeVariableScope::EraseVariable(const str& pName)
{
	bool lDeleted;
	{
		ScopeLock lLock(&mLock);
		lDeleted = DeleteLocalVariable(pName);
	}
	if (!lDeleted && mParentScope)
	{
		lDeleted = mParentScope->EraseVariable(pName);
	}
	return lDeleted;
}



std::list<str> RuntimeVariableScope::GetVariableNameList(SearchType pSearchType, int pStartScopeIndex, int pEndScopeIndex)
{
	std::list<str> lVariableNameList;
	ScopeLock lLock(&mLock);
	if (pEndScopeIndex > 0 && pStartScopeIndex < pEndScopeIndex && mParentScope)
	{
		lVariableNameList = mParentScope->GetVariableNameList(pSearchType, pStartScopeIndex-1, pEndScopeIndex-1);
	}
	if (pStartScopeIndex <= 0)
	{
		VariableTable::iterator x = mVariableTable.begin();
		for (; x != mVariableTable.end(); ++x)
		{
			if (pSearchType == SEARCH_ALL ||
				x->second->GetUsage() != RuntimeVariable::USAGE_INTERNAL)
			{
				lVariableNameList.push_back(x->first);
			}
		}
	}
	return lVariableNameList;
}



RuntimeVariableScope::DataType RuntimeVariableScope::GetUntypedType(const str& pValue)
{
	str s;
	bool b;
	int i;
	double d;
	return Cast(pValue, s, b, i, d);
}

RuntimeVariableScope::DataType RuntimeVariableScope::GetType(const str& pValue, str& pOutStr, bool& pOutBool, int& pOutInt, double& pOutDouble)
{
	if (strutil::StringToBool(pValue, pOutBool))
	{
		return RuntimeVariable::DATATYPE_BOOL;
	}
	if (strutil::StringToInt(pValue, pOutInt))
	{
		return RuntimeVariable::DATATYPE_INT;
	}
	if (strutil::StringToDouble(pValue, pOutDouble))
	{
		return RuntimeVariable::DATATYPE_REAL;
	}
	pOutStr = pValue;
	return RuntimeVariable::DATATYPE_STRING;
}

RuntimeVariableScope::DataType RuntimeVariableScope::Cast(const str& pValue, str& pOutStr, bool& pOutBool, int& pOutInt, double& pOutDouble)
{
	if (pValue.substr(0, 9) == _T("(boolean)"))
	{
		size_t lConstantStart = strutil::FindNextWord(pValue, _T(" \t\r\n)"), 9-1);
		if (lConstantStart < pValue.length())
		{
			const str lValue = pValue.substr(lConstantStart);
			bool lBool;
			int lInt;
			double lDouble;
			if (strutil::StringToBool(lValue, lBool))
			{
				// Redundant cast.
				pOutBool = lBool;
				return RuntimeVariable::DATATYPE_BOOL;
			}
			else if (strutil::StringToInt(lValue, lInt))
			{
				pOutBool = (lInt != 0);
				return RuntimeVariable::DATATYPE_BOOL;
			}
			else if (strutil::StringToDouble(lValue, lDouble))
			{
				pOutBool = (lDouble >= -0.5 && lDouble < 0.5);
				return RuntimeVariable::DATATYPE_BOOL;
			}
			mLog.Warningf(_T("Uncastable bool value: %s."), lValue.c_str());
			pOutStr = pValue;
			return RuntimeVariable::DATATYPE_STRING;
		}
	}
	else if (pValue.substr(0, 5) == _T("(int)"))
	{
		size_t lConstantStart = strutil::FindNextWord(pValue, _T(" \t\r\n)"), 5-1);
		if (lConstantStart < pValue.length())
		{
			const str lValue = pValue.substr(lConstantStart);
			bool lBool;
			int lInt = 0;
			double lDouble;
			if (strutil::StringToBool(lValue, lBool))
			{
				pOutInt = (int)lBool;
				return RuntimeVariable::DATATYPE_INT;
			}
			else if (strutil::StringToInt(lValue, lInt))
			{
				// Redundant cast.
				pOutInt = lInt;
				return RuntimeVariable::DATATYPE_INT;
			}
			else if (strutil::StringToDouble(lValue, lDouble))
			{
				pOutInt = (int)lDouble;
				return RuntimeVariable::DATATYPE_INT;
			}
			else if (lValue.length() == 3 && lValue[0] == '\'' && lValue[2] == '\'')
			{
				pOutInt = (unsigned)lValue[1];
				return RuntimeVariable::DATATYPE_INT;
			}
			mLog.Warningf(_T("Uncastable integer value: %s."), lValue.c_str());
			pOutStr = pValue;
			return RuntimeVariable::DATATYPE_STRING;
		}
	}
	else if (pValue.substr(0, 6) == _T("(real)"))
	{
		size_t lConstantStart = strutil::FindNextWord(pValue, _T(" \t\r\n)"), 6-1);
		if (lConstantStart < pValue.length())
		{
			const str lValue = pValue.substr(lConstantStart);
			bool lBool;
			int lInt;
			double lDouble = 0;
			if (strutil::StringToBool(lValue, lBool))
			{
				pOutDouble = (double)lBool;
				return RuntimeVariable::DATATYPE_REAL;
			}
			else if (strutil::StringToInt(lValue, lInt))
			{
				pOutDouble = lInt;
				return RuntimeVariable::DATATYPE_REAL;
			}
			else if (strutil::StringToDouble(lValue, lDouble))
			{
				// Redundant cast.
				pOutDouble = lDouble;
				return RuntimeVariable::DATATYPE_REAL;
			}
			mLog.Warningf(_T("Uncastable real value: %s."), lValue.c_str());
			pOutStr = pValue;
			return RuntimeVariable::DATATYPE_STRING;
		}
	}
	return GetType(pValue, pOutStr, pOutBool, pOutInt, pOutDouble);
}



const str& RuntimeVariableScope::GetDefaultValue(GetMode pMode, const str& pName, RuntimeVariable* pVariable, const str& pDefaultValue)
{
	if (pVariable)
	{
		if (pMode == READ_DEFAULT)
		{
			return pVariable->GetDefaultStrValue();
		}
		else if (pMode == READ_WRITE)
		{
			pVariable->SetDefaultStrValue(pDefaultValue);
		}
		return pVariable->GetStrValue();
	}
	else if (pMode == READ_WRITE)
	{
		SetValue(RuntimeVariable::USAGE_NORMAL, pName, pDefaultValue);
	}
	else if (pMode != READ_IGNORE)
	{
		mLog.Warningf(_T("Variable %s not found."), pName.c_str());
	}
	return pDefaultValue;
}

bool RuntimeVariableScope::GetDefaultValue(GetMode pMode, const str& pName, RuntimeVariable* pVariable, bool pDefaultValue)
{
	if (pVariable)
	{
		if (pMode == READ_DEFAULT)
		{
			return pVariable->GetDefaultBoolValue();
		}
		else if (pMode == READ_WRITE)
		{
			pVariable->SetDefaultBoolValue(pDefaultValue);
		}
		return pVariable->GetBoolValue();
	}
	else if (pMode == READ_WRITE)
	{
		SetValue(RuntimeVariable::USAGE_NORMAL, pName, pDefaultValue);
	}
	else if (pMode != READ_IGNORE)
	{
		mLog.Warningf(_T("Variable %s not found."), pName.c_str());
	}
	return pDefaultValue;
}

int RuntimeVariableScope::GetDefaultValue(GetMode pMode, const str& pName, RuntimeVariable* pVariable, int pDefaultValue)
{
	if (pVariable)
	{
		if (pMode == READ_DEFAULT)
		{
			return pVariable->GetDefaultIntValue();
		}
		else if (pMode == READ_WRITE)
		{
			pVariable->SetDefaultIntValue(pDefaultValue);
		}
		return pVariable->GetIntValue();
	}
	else if (pMode == READ_WRITE)
	{
		SetValue(RuntimeVariable::USAGE_NORMAL, pName, pDefaultValue);
	}
	else if (pMode != READ_IGNORE)
	{
		mLog.Warningf(_T("Variable %s not found."), pName.c_str());
	}
	return pDefaultValue;
}

double RuntimeVariableScope::GetDefaultValue(GetMode pMode, const str& pName, RuntimeVariable* pVariable, double pDefaultValue)
{
	if (pVariable)
	{
		if (pMode == READ_DEFAULT)
		{
			return pVariable->GetDefaultRealValue();
		}
		else if (pMode == READ_WRITE)
		{
			pVariable->SetDefaultRealValue(pDefaultValue);
		}
		return pVariable->GetRealValue();
	}
	else if (pMode == READ_WRITE)
	{
		SetValue(RuntimeVariable::USAGE_NORMAL, pName, pDefaultValue);
	}
	else if (pMode != READ_IGNORE)
	{
		mLog.Warningf(_T("Variable %s not found."), pName.c_str());
	}
	return pDefaultValue;
}



void RuntimeVariableScope::CreateLocalVariable(const str& pName, DataType pType, const str& pStrValue, bool pBoolValue, int pIntValue, double pDoubleValue, SetMode pSetMode)
{
	RuntimeVariable* lVariable = 0;
	switch (pType)
	{
		case RuntimeVariable::DATATYPE_STRING:	lVariable = new RuntimeVariable(pName, pStrValue, pSetMode);	break;
		case RuntimeVariable::DATATYPE_BOOL:	lVariable = new RuntimeVariable(pName, pBoolValue, pSetMode);	break;
		case RuntimeVariable::DATATYPE_INT:	lVariable = new RuntimeVariable(pName, pIntValue, pSetMode);	break;
		case RuntimeVariable::DATATYPE_REAL:	lVariable = new RuntimeVariable(pName, pDoubleValue, pSetMode);	break;
	}
	ScopeLock lLock(&mLock);
	mVariableTable.insert(VariableTable::value_type(pName, lVariable));
}

bool RuntimeVariableScope::DeleteLocalVariable(const str& pName)
{
	bool lDeleted = false;
	ScopeLock lLock(&mLock);
	VariableTable::iterator x = mVariableTable.find(pName);
	if (x != mVariableTable.end())
	{
		RuntimeVariable* lVariable = x->second;
		mVariableTable.erase(x);
		delete (lVariable);
		lDeleted = true;
	}
	return (lDeleted);
}

RuntimeVariable* RuntimeVariableScope::GetVariable(const HashedString& pName, bool pRecursive) const
{
	ScopeLock lLock(&mLock);
	RuntimeVariable* lVariable;
	VariableTable::const_iterator x = mVariableTable.find(pName);
	if (x != mVariableTable.end())
	{
		lVariable = x->second;
	}
	else if (pRecursive && mParentScope)
	{
		lVariable = mParentScope->GetVariable(pName, pRecursive);
	}
	else
	{
		lVariable = 0;
	}
	if (lVariable)
	{
		deb_assert(lVariable->GetName() == pName);
	}
	return (lVariable);
}



RuntimeVariableCompleter::RuntimeVariableCompleter(RuntimeVariableScope* pVariableScope, const str& pPrefix):
	mVariableScope(pVariableScope),
	mPrefix(pPrefix)
{
}

RuntimeVariableCompleter::~RuntimeVariableCompleter()
{
	mVariableScope = 0;
}

std::list<str> RuntimeVariableCompleter::CompleteCommand(const str& pPartialCommand) const
{
	std::list<str> lCompletionList;
	if (pPartialCommand.compare(0, mPrefix.length(), mPrefix) == 0)
	{
		const str lPartialVariableName = pPartialCommand.substr(mPrefix.length());
		std::list<str> lVariableList = mVariableScope->GetVariableNameList(RuntimeVariableScope::SEARCH_ALL);
		std::list<str>::const_iterator x = lVariableList.begin();
		for (; x != lVariableList.end(); ++x)
		{
			const str& lCompletion = *x;
			if (strutil::StartsWith(lCompletion, lPartialVariableName))
			{
				// Add to list.
				lCompletionList.push_back(mPrefix+lCompletion);
			}
		}
	}
	return (lCompletionList);
}



LOG_CLASS_DEFINE(GENERAL, RuntimeVariable);
LOG_CLASS_DEFINE(GENERAL, RuntimeVariableScope);



}
