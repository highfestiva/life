
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include <assert.h>
#include "../../Lepra/Include/HashUtil.h"
#include "../Include/RuntimeVariable.h"



namespace Cure
{



RuntimeVariable::RuntimeVariable(const str& pName, const str& pValue, Usage pUsage):
	mName(pName)
{
	SetValue(pValue, pUsage);
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

bool RuntimeVariable::GetBoolValue() const
{
	if (mDataType == DATATYPE_BOOL)
	{
		return mBoolValue;
	}
	mLog.Errorf(_T("RT variable %s is not of type int (value is %s)."), mName.c_str(), mValue.c_str());
	assert(false);
	return 0;
}

int RuntimeVariable::GetIntValue() const
{
	if (mDataType == DATATYPE_INT)
	{
		return mIntValue;
	}
	mLog.Errorf(_T("RT variable %s is not of type int (value is %s)."), mName.c_str(), mValue.c_str());
	assert(false);
	return 0;
}

double RuntimeVariable::GetRealValue() const
{
	if (mDataType == DATATYPE_REAL)
	{
		return mRealValue;
	}
	mLog.Errorf(_T("RT variable %s is not of type double (value is %s)."), mName.c_str(), mValue.c_str());
	assert(false);
	return 0;
}

void RuntimeVariable::SetValue(const str& pValue, Usage pUsage)
{
	mValue = pValue;
	if (pUsage != USAGE_NORMAL)
	{
		mDefaultValue = pValue;
	}
	mUsage = pUsage;

	if (strutil::StringToBool(pValue, mBoolValue))
	{
		mDataType = DATATYPE_BOOL;
	}
	else if (strutil::StringToInt(pValue, mIntValue))
	{
		mDataType = DATATYPE_INT;
	}
	else if (strutil::StringToDouble(pValue, mRealValue))
	{
		mDataType = DATATYPE_REAL;
	}
	else
	{
		mDataType = DATATYPE_STRING;
	}
}

const str& RuntimeVariable::GetDefaultValue() const
{
	return (mDefaultValue);
}

void RuntimeVariable::SetDefaultValue(const str& pDefaultValue)
{
	mDefaultValue = pDefaultValue;
}

RuntimeVariable::Usage RuntimeVariable::GetUsage() const
{
	return (mUsage);
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

	RuntimeVariable* lVariable = GetVariable(pName, pSetMode != RuntimeVariable::USAGE_OVERRIDE);
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

const str& RuntimeVariableScope::GetDefaultValue(GetMode pMode, const HashedString& pName, const str& pDefaultValue)
{
	return GetDefaultValue(pMode, pName, GetVariable(pName), pDefaultValue);
}

const str RuntimeVariableScope::GetDefaultValue(GetMode pMode, const HashedString& pName, const tchar* pDefaultValue)
{
	return GetDefaultValue(pMode, pName, GetVariable(pName), str(pDefaultValue));
}

double RuntimeVariableScope::GetDefaultValue(GetMode pMode, const HashedString& pName, double pDefaultValue)
{
	return GetDefaultValue(pMode, pName, GetVariable(pName), pDefaultValue);
}

int RuntimeVariableScope::GetDefaultValue(GetMode pMode, const HashedString& pName, int pDefaultValue)
{
	return GetDefaultValue(pMode, pName, GetVariable(pName), pDefaultValue);
}

bool RuntimeVariableScope::GetDefaultValue(GetMode pMode, const HashedString& pName, bool pDefaultValue)
{
	return GetDefaultValue(pMode, pName, GetVariable(pName), pDefaultValue);
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
	return (lVariableNameList);
}



const str& RuntimeVariableScope::GetDefaultValue(GetMode pMode, const str& pName, RuntimeVariable* pVariable, const str& pDefaultValue)
{
	if (pVariable)
	{
		if (pMode == READ_DEFAULT)
		{
			return (pVariable->GetDefaultValue());
		}
		else if (pMode == READ_WRITE)
		{
			pVariable->SetDefaultValue(pDefaultValue);
		}
		return (pVariable->GetValue());
	}
	else if (pMode == READ_WRITE)
	{
		SetValue(RuntimeVariable::USAGE_NORMAL, pName, pDefaultValue);
	}
	else if (pMode != READ_IGNORE)
	{
		mLog.Warningf(_T("Variable %s not found."), pName.c_str());
	}
	return (pDefaultValue);
}

double RuntimeVariableScope::GetDefaultValue(GetMode pMode, const str& pName, RuntimeVariable* pVariable, double pDefaultValue)
{
	if (pVariable && (pMode == READ_ONLY || pMode == READ_IGNORE))	// Optimization.
	{
		return pVariable->GetRealValue();
	}
	str lDefaultValue = strutil::FastDoubleToString(pDefaultValue);
	str lValueString = GetDefaultValue(pMode, pName, lDefaultValue);
	double lValue = pDefaultValue;
	if (!strutil::StringToDouble(lValueString, lValue))
	{
		mLog.Errorf(_T("RT variable %s is not of type double (value is %s)."), pName.c_str(), lValueString.c_str());
		assert(false);
	}
	return (lValue);
}

int RuntimeVariableScope::GetDefaultValue(GetMode pMode, const str& pName, RuntimeVariable* pVariable, int pDefaultValue)
{
	if (pVariable && (pMode == READ_ONLY || pMode == READ_IGNORE))	// Optimization.
	{
		return pVariable->GetIntValue();
	}
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

bool RuntimeVariableScope::GetDefaultValue(GetMode pMode, const str& pName, RuntimeVariable* pVariable, bool pDefaultValue)
{
	if (pVariable && (pMode == READ_ONLY || pMode == READ_IGNORE))	// Optimization.
	{
		return pVariable->GetBoolValue();
	}
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



LOG_CLASS_DEFINE(GENERAL, RuntimeVariable);
LOG_CLASS_DEFINE(GENERAL, RuntimeVariableScope);



}
