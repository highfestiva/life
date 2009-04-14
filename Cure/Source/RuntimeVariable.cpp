
// Author: Jonas Bystr�m
// Copyright (c) 2002-2007, Righteous Games



#include <assert.h>
#include "../../Lepra/Include/HashUtil.h"
#include "../Include/RuntimeVariable.h"



namespace Cure
{



RuntimeVariable::RuntimeVariable(const Lepra::String& pName, const Lepra::String& pValue):
	mName(pName),
	mValue(pValue),
	mDefaultValue(pValue)
{
}

RuntimeVariable::~RuntimeVariable()
{
}

const Lepra::String& RuntimeVariable::GetName() const
{
	return (mName);
}

bool RuntimeVariable::operator==(const Lepra::String& pValue)
{
	return (mValue == pValue);
}

const Lepra::String& RuntimeVariable::GetValue() const
{
	return (mValue);
}

void RuntimeVariable::SetValue(const Lepra::String& pValue)
{
	mValue = pValue;
}

const Lepra::String& RuntimeVariable::GetDefaultValue() const
{
	return (mDefaultValue);
}



RuntimeVariableScope::RuntimeVariableScope(RuntimeVariableScope* pParentScope):
	mParentScope(pParentScope)
{
}

RuntimeVariableScope::~RuntimeVariableScope()
{
	Lepra::ScopeLock lLock(&mLock);
	VariableTable::iterator x = mVariableTable.begin();
	while (!mVariableTable.empty())
	{
		RuntimeVariable* lVariable = mVariableTable.begin()->second;
		DeleteLocalVariable(lVariable->GetName());
	}
	mParentScope = 0;
}

Lepra::String RuntimeVariableScope::GetType(const Lepra::String& pValue)
{
	bool lDummyBool;
	if (Lepra::StringUtility::StringToBool(pValue, lDummyBool))
	{
		return (_T("boolean"));
	}
	int lDummyInt;
	if (Lepra::StringUtility::StringToInt(pValue, lDummyInt))
	{
		return (_T("int"));
	}
	double lDummyDouble;
	if (Lepra::StringUtility::StringToDouble(pValue, lDummyDouble))
	{
		return (_T("real"));
	}
	return (_T("string"));
}

Lepra::String RuntimeVariableScope::Cast(const Lepra::String& pValue)
{
	Lepra::String lValue = pValue;
	if (pValue.substr(0, 9) == _T("(boolean)"))
	{
		bool lBool = false;
		size_t lConstantStart = Lepra::StringUtility::FindNextWord(pValue, _T(" \t\r\n)"), 9-1);
		if (lConstantStart < pValue.length())
		{
			lValue = pValue.substr(lConstantStart);
			int lInt;
			double lDouble;
			if (Lepra::StringUtility::StringToBool(lValue, lBool))
			{
				// Redundant cast.
			}
			else if (Lepra::StringUtility::StringToInt(lValue, lInt))
			{
				lBool = (lInt != 0);
			}
			else if (Lepra::StringUtility::StringToDouble(lValue, lDouble))
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
		size_t lConstantStart = Lepra::StringUtility::FindNextWord(pValue, _T(" \t\r\n)"), 5-1);
		if (lConstantStart < pValue.length())
		{
			lValue = pValue.substr(lConstantStart);
			bool lBool;
			double lDouble;
			if (Lepra::StringUtility::StringToBool(lValue, lBool))
			{
				lInt = (int)lBool;
			}
			else if (Lepra::StringUtility::StringToInt(lValue, lInt))
			{
				// Redundant cast.
			}
			else if (Lepra::StringUtility::StringToDouble(lValue, lDouble))
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
		lValue = Lepra::StringUtility::IntToString(lInt, 10);
	}
	else if (pValue.substr(0, 6) == _T("(real)"))
	{
		double lDouble = 0;
		size_t lConstantStart = Lepra::StringUtility::FindNextWord(pValue, _T(" \t\r\n)"), 6-1);
		if (lConstantStart < pValue.length())
		{
			lValue = pValue.substr(lConstantStart);
			bool lBool;
			int lInt;
			if (Lepra::StringUtility::StringToBool(lValue, lBool))
			{
				lDouble = (double)lBool;
			}
			else if (Lepra::StringUtility::StringToInt(lValue, lInt))
			{
				lDouble = lInt;
			}
			else if (Lepra::StringUtility::StringToDouble(lValue, lDouble))
			{
				// Redundant cast.
			}
			else
			{
				mLog.Warningf(_T("Uncastable real value: %s."), lValue.c_str());
				return (pValue);
			}
		}
		lValue = Lepra::StringUtility::DoubleToString(lDouble, 16);
	}
	return (lValue);
}

bool RuntimeVariableScope::IsDefined(const Lepra::String& pName)
{
	bool lFound = (GetVariable(pName) != 0);
	return (lFound);
}

bool RuntimeVariableScope::SetValue(const Lepra::String& pName, const Lepra::String& pValue)
{
	bool lTypeOk = true;

	RuntimeVariable* lVariable = GetVariable(pName);
	Lepra::String lValue = Cast(pValue);
	if (lVariable)
	{
		if (GetType(lValue) == GetType(lVariable->GetValue()))
		{
			lVariable->SetValue(lValue);
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
		CreateLocalVariable(pName, lValue);
	}
	return (lTypeOk);
}

bool RuntimeVariableScope::SetValue(const Lepra::String& pName, const Lepra::tchar* pValue)
{
	return (SetValue(pName, Lepra::String(pValue)));
}

bool RuntimeVariableScope::SetValue(const Lepra::String& pName, double pValue)
{
	// We want the real format to be differentiatable from the int format.
	Lepra::String lStringValue = Lepra::StringUtility::Format(_T("%g"), pValue);
	if (lStringValue.find(_T('.')) == Lepra::String::npos &&
		lStringValue.find(_T('e')) == Lepra::String::npos)
	{
		lStringValue = Lepra::StringUtility::Format(_T("%.1f"), pValue);
	}
	return (SetValue(pName, lStringValue));
}

bool RuntimeVariableScope::SetValue(const Lepra::String& pName, int pValue)
{
	return (SetValue(pName, Lepra::StringUtility::IntToString(pValue, 10)));
}

bool RuntimeVariableScope::SetValue(const Lepra::String& pName, bool pValue)
{
	return (SetValue(pName, Lepra::StringUtility::BoolToString(pValue)));
}

const Lepra::String& RuntimeVariableScope::GetDefaultValue(const Lepra::String& pName, GetMode pMode, const Lepra::String& pDefaultValue)
{
	const RuntimeVariable* lVariable = GetVariable(pName);
	if (lVariable)
	{
		if (pMode == READ_DEFAULT)
		{
			return (lVariable->GetDefaultValue());
		}
		return (lVariable->GetValue());
	}
	else if (pMode == READ_WRITE)
	{
		SetValue(pName, pDefaultValue);
	}
	mLog.Warningf(_T("Variable %s not found."), pName.c_str());
	return (pDefaultValue);
}

const Lepra::String RuntimeVariableScope::GetDefaultValue(const Lepra::String& pName, GetMode pMode, const Lepra::tchar* pDefaultValue)
{
	return (GetDefaultValue(pName, pMode, Lepra::String(pDefaultValue)));
}

double RuntimeVariableScope::GetDefaultValue(const Lepra::String& pName, GetMode pMode, double pDefaultValue)
{
	Lepra::String lDefaultValue = Lepra::StringUtility::DoubleToString(pDefaultValue, 6);
	Lepra::String lValueString = GetDefaultValue(pName, pMode, lDefaultValue);
	double lValue = pDefaultValue;
	if (!Lepra::StringUtility::StringToDouble(lValueString, lValue))
	{
		mLog.Errorf(_T("RT variable %s is not int (value is %s)."), pName.c_str(), lValueString.c_str());
		assert(false);
	}
	return (lValue);
}

int RuntimeVariableScope::GetDefaultValue(const Lepra::String& pName, GetMode pMode, int pDefaultValue)
{
	Lepra::String lDefaultValue = Lepra::StringUtility::IntToString(pDefaultValue, 10);
	Lepra::String lValueString = GetDefaultValue(pName, pMode, lDefaultValue);
	int lValue = pDefaultValue;
	if (!Lepra::StringUtility::StringToInt(lValueString, lValue))
	{
		mLog.Errorf(_T("RT variable %s is not int (value is %s)."), pName.c_str(), lValueString.c_str());
		assert(false);
	}
	return (lValue);
}

bool RuntimeVariableScope::GetDefaultValue(const Lepra::String& pName, GetMode pMode, bool pDefaultValue)
{
	Lepra::String lDefaultValue = Lepra::StringUtility::BoolToString(pDefaultValue);
	Lepra::String lValueString = GetDefaultValue(pName, pMode, lDefaultValue);
	bool lValue = pDefaultValue;
	if (!Lepra::StringUtility::StringToBool(lValueString, lValue))
	{
		mLog.Errorf(_T("RT variable %s is not bool (value is %s)."), pName.c_str(), lValueString.c_str());
		assert(false);
	}
	return (lValue);
}

bool RuntimeVariableScope::EraseVariable(const Lepra::String& pName)
{
	Lepra::ScopeLock lLock(&mLock);
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



std::list<Lepra::String> RuntimeVariableScope::GetVariableNameList(int pScopeCount)
{
	std::list<Lepra::String> lVariableNameList;
	Lepra::ScopeLock lLock(&mLock);
	if ((pScopeCount < 0 || pScopeCount > 1) && mParentScope)
	{
		lVariableNameList = mParentScope->GetVariableNameList(pScopeCount-1);
	}
	if (pScopeCount != 0)
	{
		VariableTable::iterator x = mVariableTable.begin();
		for (; x != mVariableTable.end(); ++x)
		{
			lVariableNameList.push_back(x->first);
		}
	}
	return (lVariableNameList);
}



void RuntimeVariableScope::CreateLocalVariable(const Lepra::String& pName, const Lepra::String& pValue)
{
	RuntimeVariable* lVariable = new RuntimeVariable(pName, pValue);
	Lepra::ScopeLock lLock(&mLock);
	mVariableTable.insert(VariablePair(pName, lVariable));
}

bool RuntimeVariableScope::DeleteLocalVariable(const Lepra::String& pName)
{
	bool lDeleted = false;
	Lepra::ScopeLock lLock(&mLock);
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

RuntimeVariable* RuntimeVariableScope::GetVariable(const Lepra::String& pName) const
{
	Lepra::ScopeLock lLock(&mLock);
	RuntimeVariable* lVariable = Lepra::HashUtil::FindMapObject(mVariableTable, pName);
	if (!lVariable && mParentScope)
	{
		lVariable = mParentScope->GetVariable(pName);
	}
	return (lVariable);
}



LOG_CLASS_DEFINE(GENERAL, RuntimeVariableScope);



RuntimeVariableCompleter::RuntimeVariableCompleter(RuntimeVariableScope* pVariableScope, const Lepra::String& pPrefix):
	mVariableScope(pVariableScope),
	mPrefix(pPrefix)
{
}

RuntimeVariableCompleter::~RuntimeVariableCompleter()
{
	mVariableScope = 0;
}

std::list<Lepra::String> RuntimeVariableCompleter::CompleteCommand(const Lepra::String& pPartialCommand) const
{
	std::list<Lepra::String> lCompletionList;
	if (pPartialCommand.compare(0, mPrefix.length(), mPrefix) == 0)
	{
		const Lepra::String lPartialVariableName = pPartialCommand.substr(mPrefix.length());
		std::list<Lepra::String> lVariableList = mVariableScope->GetVariableNameList();
		std::list<Lepra::String>::const_iterator x = lVariableList.begin();
		for (; x != lVariableList.end(); ++x)
		{
			const Lepra::String& lCompletion = *x;
			if (lCompletion.substr(0, lPartialVariableName.length()) == lPartialVariableName)
			{
				// Add to list.
				lCompletionList.push_back(mPrefix+lCompletion);
			}
		}
	}
	return (lCompletionList);
}



}
