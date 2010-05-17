
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include <assert.h>
#include "../../Lepra/Include/HashUtil.h"
#include "../Include/RuntimeVariable.h"



namespace Cure
{



RuntimeVariable::RuntimeVariable(const str& pName, const str& pValue, Type pType):
	mName(pName),
	mValue(pValue),
	mDefaultValue(pValue),
	mType(pType)
{
}

RuntimeVariable::~RuntimeVariable()
{
}

const str& RuntimeVariable::GetName() const
{
	return (mName);
}

bool RuntimeVariable::operator==(const str& pValue)
{
	return (mValue == pValue);
}

const str& RuntimeVariable::GetValue() const
{
	return (mValue);
}

void RuntimeVariable::SetValue(const str& pValue, Type pType)
{
	mValue = pValue;
	if (pType != TYPE_NORMAL)
	{
		mDefaultValue = pValue;
	}
	mType = pType;
}

const str& RuntimeVariable::GetDefaultValue() const
{
	return (mDefaultValue);
}

void RuntimeVariable::SetDefaultValue(const str& pDefaultValue)
{
	mDefaultValue = pDefaultValue;
}

RuntimeVariable::Type RuntimeVariable::GetType() const
{
	return (mType);
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

str RuntimeVariableScope::GetType(const str& pValue)
{
	bool lDummyBool;
	if (strutil::StringToBool(pValue, lDummyBool))
	{
		return (_T("boolean"));
	}
	int lDummyInt;
	if (strutil::StringToInt(pValue, lDummyInt))
	{
		return (_T("int"));
	}
	double lDummyDouble;
	if (strutil::StringToDouble(pValue, lDummyDouble))
	{
		return (_T("real"));
	}
	return (_T("string"));
}

str RuntimeVariableScope::Cast(const str& pValue)
{
	str lValue = pValue;
	if (pValue.substr(0, 9) == _T("(boolean)"))
	{
		bool lBool = false;
		size_t lConstantStart = strutil::FindNextWord(pValue, _T(" \t\r\n)"), 9-1);
		if (lConstantStart < pValue.length())
		{
			lValue = pValue.substr(lConstantStart);
			int lInt;
			double lDouble;
			if (strutil::StringToBool(lValue, lBool))
			{
				// Redundant cast.
			}
			else if (strutil::StringToInt(lValue, lInt))
			{
				lBool = (lInt != 0);
			}
			else if (strutil::StringToDouble(lValue, lDouble))
			{
				lBool = (lDouble <= -0.5 || lDouble >= 0.5);
			}
			else
			{
				mLog.Warningf(_T("Uncastable bool value: %s."), lValue.c_str());
				return (pValue);
			}
		}
		if (lBool)
		{
			lValue = _T("true");
		}
		else
		{
			lValue = _T("false");
		}
	}
	else if (pValue.substr(0, 5) == _T("(int)"))
	{
		int lInt = 0;
		size_t lConstantStart = strutil::FindNextWord(pValue, _T(" \t\r\n)"), 5-1);
		if (lConstantStart < pValue.length())
		{
			lValue = pValue.substr(lConstantStart);
			bool lBool;
			double lDouble;
			if (strutil::StringToBool(lValue, lBool))
			{
				lInt = (int)lBool;
			}
			else if (strutil::StringToInt(lValue, lInt))
			{
				// Redundant cast.
			}
			else if (strutil::StringToDouble(lValue, lDouble))
			{
				lInt = (int)lDouble;
			}
			else if (lValue.length() == 3 && lValue[0] == '\'' && lValue[2] == '\'')
			{
				lInt = (unsigned)lValue[1];
			}
			else
			{
				mLog.Warningf(_T("Uncastable integer value: %s."), lValue.c_str());
				return (pValue);
			}
		}
		lValue = strutil::IntToString(lInt, 10);
	}
	else if (pValue.substr(0, 6) == _T("(real)"))
	{
		double lDouble = 0;
		size_t lConstantStart = strutil::FindNextWord(pValue, _T(" \t\r\n)"), 6-1);
		if (lConstantStart < pValue.length())
		{
			lValue = pValue.substr(lConstantStart);
			bool lBool;
			int lInt;
			if (strutil::StringToBool(lValue, lBool))
			{
				lDouble = (double)lBool;
			}
			else if (strutil::StringToInt(lValue, lInt))
			{
				lDouble = lInt;
			}
			else if (strutil::StringToDouble(lValue, lDouble))
			{
				// Redundant cast.
			}
			else
			{
				mLog.Warningf(_T("Uncastable real value: %s."), lValue.c_str());
				return (pValue);
			}
		}
		lValue = strutil::DoubleToString(lDouble, 16);
	}
	return (lValue);
}

bool RuntimeVariableScope::IsDefined(const str& pName)
{
	bool lFound = (GetVariable(pName) != 0);
	return (lFound);
}

bool RuntimeVariableScope::SetValue(SetMode pSetMode, const str& pName, const str& pValue)
{
	bool lTypeOk = true;

	RuntimeVariable* lVariable = GetVariable(pName, pSetMode != RuntimeVariable::TYPE_OVERRIDE);
	str lValue = Cast(pValue);
	if (lVariable)
	{
		if (GetType(lValue) == GetType(lVariable->GetValue()))
		{
			lVariable->SetValue(lValue, pSetMode);
		}
		else
		{
			lTypeOk = false;
			mLog.Warningf(_T("Could not set variable %s; type should be %s, not %s!"),
				pName.c_str(), GetType(lVariable->GetValue()).c_str(), GetType(lValue).c_str());
		}
	}
	else
	{
		CreateLocalVariable(pName, lValue, pSetMode);
	}
	return (lTypeOk);
}

bool RuntimeVariableScope::SetValue(SetMode pSetMode, const str& pName, const tchar* pValue)
{
	return (SetValue(pSetMode, pName, str(pValue)));
}

bool RuntimeVariableScope::SetValue(SetMode pSetMode, const str& pName, double pValue)
{
	// We want the real format to be differentiatable from the int format.
	str lStringValue = strutil::Format(_T("%g"), pValue);
	if (lStringValue.find(_T('.')) == str::npos &&
		lStringValue.find(_T('e')) == str::npos)
	{
		lStringValue = strutil::Format(_T("%.1f"), pValue);
	}
	return (SetValue(pSetMode, pName, lStringValue));
}

bool RuntimeVariableScope::SetValue(SetMode pSetMode, const str& pName, int pValue)
{
	return (SetValue(pSetMode, pName, strutil::IntToString(pValue, 10)));
}

bool RuntimeVariableScope::SetValue(SetMode pSetMode, const str& pName, bool pValue)
{
	return (SetValue(pSetMode, pName, strutil::BoolToString(pValue)));
}

const str& RuntimeVariableScope::GetDefaultValue(GetMode pMode, const str& pName, const str& pDefaultValue)
{
	RuntimeVariable* lVariable = GetVariable(pName);
	if (lVariable)
	{
		if (pMode == READ_DEFAULT)
		{
			return (lVariable->GetDefaultValue());
		}
		else if (pMode == READ_WRITE)
		{
			lVariable->SetDefaultValue(pDefaultValue);
		}
		return (lVariable->GetValue());
	}
	else if (pMode == READ_WRITE)
	{
		SetValue(RuntimeVariable::TYPE_NORMAL, pName, pDefaultValue);
	}
	else if (pMode != READ_IGNORE)
	{
		mLog.Warningf(_T("Variable %s not found."), pName.c_str());
	}
	return (pDefaultValue);
}

const str RuntimeVariableScope::GetDefaultValue(GetMode pMode, const str& pName, const tchar* pDefaultValue)
{
	return (GetDefaultValue(pMode, pName, str(pDefaultValue)));
}

double RuntimeVariableScope::GetDefaultValue(GetMode pMode, const str& pName, double pDefaultValue)
{
	str lDefaultValue = strutil::DoubleToString(pDefaultValue, 6);
	str lValueString = GetDefaultValue(pMode, pName, lDefaultValue);
	double lValue = pDefaultValue;
	if (!strutil::StringToDouble(lValueString, lValue))
	{
		mLog.Errorf(_T("RT variable %s is not of type double (value is %s)."), pName.c_str(), lValueString.c_str());
		assert(false);
	}
	return (lValue);
}

int RuntimeVariableScope::GetDefaultValue(GetMode pMode, const str& pName, int pDefaultValue)
{
	str lDefaultValue = strutil::IntToString(pDefaultValue, 10);
	str lValueString = GetDefaultValue(pMode, pName, lDefaultValue);
	int lValue = pDefaultValue;
	if (!strutil::StringToInt(lValueString, lValue))
	{
		mLog.Errorf(_T("RT variable %s is not of type int (value is %s)."), pName.c_str(), lValueString.c_str());
		assert(false);
	}
	return (lValue);
}

bool RuntimeVariableScope::GetDefaultValue(GetMode pMode, const str& pName, bool pDefaultValue)
{
	str lDefaultValue = strutil::BoolToString(pDefaultValue);
	str lValueString = GetDefaultValue(pMode, pName, lDefaultValue);
	bool lValue = pDefaultValue;
	if (!strutil::StringToBool(lValueString, lValue))
	{
		mLog.Errorf(_T("RT variable %s is not of type bool (value is %s)."), pName.c_str(), lValueString.c_str());
		assert(false);
	}
	return (lValue);
}

bool RuntimeVariableScope::EraseVariable(const str& pName)
{
	ScopeLock lLock(&mLock);
	bool lDeleted = DeleteLocalVariable(pName);
	if (!lDeleted && mParentScope)
	{
		lDeleted = mParentScope->DeleteLocalVariable(pName);
	}
	return (lDeleted);
}



RuntimeVariableScope* RuntimeVariableScope::LockParentScope(RuntimeVariableScope* pParentScope)
{
	if (!pParentScope)
	{
		mLock.Acquire();
	}
	RuntimeVariableScope* lParentScope = mParentScope;
	mParentScope = pParentScope;
	if (pParentScope)
	{
		mLock.Release();
	}
	return (lParentScope);
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
				x->second->GetType() != RuntimeVariable::TYPE_INTERNAL)
			{
				lVariableNameList.push_back(x->first);
			}
		}
	}
	return (lVariableNameList);
}



void RuntimeVariableScope::CreateLocalVariable(const str& pName, const str& pValue, SetMode pSetMode)
{
	RuntimeVariable* lVariable = new RuntimeVariable(pName, pValue, pSetMode);
	ScopeLock lLock(&mLock);
	mVariableTable.insert(VariablePair(pName, lVariable));
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

RuntimeVariable* RuntimeVariableScope::GetVariable(const str& pName, bool pRecursive) const
{
	ScopeLock lLock(&mLock);
	RuntimeVariable* lVariable = HashUtil::FindMapObject(mVariableTable, pName);
	if (pRecursive && !lVariable && mParentScope)
	{
		lVariable = mParentScope->GetVariable(pName, pRecursive);
	}
	return (lVariable);
}



LOG_CLASS_DEFINE(GENERAL, RuntimeVariableScope);



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
		std::list<str> lVariableList = mVariableScope->GetVariableNameList(RuntimeVariableScope::SEARCH_EXPORTABLE);
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



}
