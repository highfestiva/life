
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "OptionsManager.h"
#include <assert.h>
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Lepra/Include/CyclicArray.h"
#include "../../Lepra/Include/Math.h"
#include "RtVar.h"



namespace Life
{
namespace Options
{



OptionsManager::OptionsManager(Cure::RuntimeVariableScope* pVariableScope, int pPriority):
	mVariableScope(pVariableScope),
	mConsoleToggle(0)
{
	SetDefault(pPriority);
	DoRefreshConfiguration();
}



void OptionsManager::RefreshConfiguration()
{
	if (mLastConfigRefresh.QueryTimeDiff() > 5.0)
	{
		mLastConfigRefresh.ClearTimeDiff();
		mInputMap.clear();
		DoRefreshConfiguration();
	}
}

bool OptionsManager::UpdateInput(UiLepra::InputManager::KeyCode pKeyCode, bool pActive)
{
	const str lInputElementName = ConvertToString(pKeyCode);
	return (SetValue(lInputElementName, pActive? 1.0f : 0.0f, false));
}

float OptionsManager::UpdateInput(UiLepra::InputElement* pElement)
{
	bool lValueSet;
	float lValue = (float)pElement->GetValue();
	const str lInputElementName = pElement->GetFullName();
	if (pElement->GetType() == UiLepra::InputElement::ANALOGUE)
	{
		const bool lIsRelative = (pElement->GetInterpretation() == UiLepra::InputElement::RELATIVE_AXIS);
		// Update both sides for analogue input.
		if (lIsRelative)
		{
			lValue *= 0.1f;	// Relative values are more sensitive.
		}
		else if (Math::IsEpsEqual(lValue, 0.0f, 0.1f))
		{
			lValue = 0;	// Clamp absolute+analogue to neutral when close enough.
		}
		if (lValue == 0)
		{
			lValueSet = SetValue(lInputElementName+_T("+"), 0, false);
			SetValue(lInputElementName+_T("-"), 0, false);
		}
		else if (lValue >= 0)
		{
			lValueSet = SetValue(lInputElementName+_T("+"), lValue, lIsRelative);
			SetValue(lInputElementName+_T("-"), 0, false);
		}
		else
		{
			lValueSet = SetValue(lInputElementName+_T("-"), -lValue, lIsRelative);
			SetValue(lInputElementName+_T("+"), 0, false);
		}
	}
	else
	{
		lValueSet = SetValue(lInputElementName, lValue, false);
	}
	return (lValueSet);
}

void OptionsManager::ResetToggles()
{
	mConsoleToggle = 0;
}

float OptionsManager::GetConsoleToggle() const
{
	return (mConsoleToggle);
}



bool OptionsManager::SetDefault(int)
{
	CURE_RTVAR_OVERRIDE(mVariableScope, RTVAR_CTRL_UI_CONTOGGLE, _T("Key.F11"));
	return (true);
}

void OptionsManager::DoRefreshConfiguration()
{
	const KeyValue lEntries[] =
	{
		KeyValue(_T(RTVAR_CTRL_UI_CONTOGGLE), &mConsoleToggle),
	};
	SetValuePointers(lEntries, LEPRA_ARRAY_COUNT(lEntries));
}

const str OptionsManager::ConvertToString(UiLepra::InputManager::KeyCode pKeyCode)
{
	return (_T("Key.")+UiLepra::InputManager::GetKeyName(pKeyCode));
}

bool OptionsManager::SetValue(const str& pKey, float pValue, bool pAdd)
{
	log_volatile(mLog.Tracef(_T("Got input %s: %g"), pKey.c_str(), pValue));

	bool lIsAnySteeringValue;
	std::vector<float*>* lValuePointers = GetValuePointers(pKey, lIsAnySteeringValue);
	if (!lValuePointers)
	{
		return false;
	}

	bool lInputChanged = false;
	for (std::vector<float*>::const_iterator x = lValuePointers->begin(); x != lValuePointers->end(); ++x)
	{
		if (pAdd)
		{
			lInputChanged = true;
			*(*x) = Math::Clamp(*(*x) + pValue, 0.0f, 1.0f);
		}
		else if (!Math::IsEpsEqual(*(*x), pValue, 0.06f))
		{
			lInputChanged = true;
			*(*x) = pValue;
		}
	}
	return (lIsAnySteeringValue && lInputChanged);
}

std::vector<float*>* OptionsManager::GetValuePointers(const str& pKey, bool& pIsAnySteeringValue)
{
	InputMap::iterator x = mInputMap.find(pKey);
	if (x == mInputMap.end())
	{
		return 0;
	}
	InputEntry& lEntry = x->second;
	pIsAnySteeringValue = lEntry.mIsAnySteeringValue;
	return &lEntry.mValuePointerArray;
}

void OptionsManager::SetValuePointers(const KeyValue pEntries[], size_t pEntryCount)
{
	for (size_t x = 0; x < pEntryCount; ++x)
	{
		const str lKeys = mVariableScope->GetDefaultValue(Cure::RuntimeVariableScope::READ_ONLY, pEntries[x].first);
		typedef strutil::strvec SV;
		SV lKeyArray = strutil::Split(lKeys, _T(", \t"));
		for (SV::iterator y = lKeyArray.begin(); y != lKeyArray.end(); ++y)
		{
			const bool lIsSteeringValue = (pEntries[x].first.find(_T("Steer")) != str::npos);
			const str& lKey = *y;
			InputMap::iterator z = mInputMap.find(lKey);
			if (z == mInputMap.end())
			{
				InputEntry lEntry;
				lEntry.mIsAnySteeringValue = lIsSteeringValue;
				lEntry.mValuePointerArray.push_back(pEntries[x].second);
				mInputMap.insert(InputMap::value_type(lKey, lEntry));
			}
			else
			{
				InputEntry& lEntry = z->second;
				lEntry.mIsAnySteeringValue |= lIsSteeringValue;
				lEntry.mValuePointerArray.push_back(pEntries[x].second);
			}
		}
	}
}



void OptionsManager::operator=(const OptionsManager&)
{
	assert(false);
}



LOG_CLASS_DEFINE(UI_INPUT, OptionsManager);



}
}
