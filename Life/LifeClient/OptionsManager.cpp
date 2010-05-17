
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "OptionsManager.h"
#include <assert.h>
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Lepra/Include/Math.h"
#include "../RtVar.h"



namespace Life
{
namespace Options
{



OptionsManager::OptionsManager(Cure::RuntimeVariableScope* pVariableScope, int pPriority):
	mVariableScope(pVariableScope),
	mConsoleToggle(0)
{
	SetDefault(pPriority);
}


	
bool OptionsManager::UpdateInput(UiLepra::InputManager::KeyCode pKeyCode, bool pActive)
{
	const str lInputElementName = ConvertToString(pKeyCode);
	return (SetValue(lInputElementName, pActive? 1.0f : 0.0f));
}

bool OptionsManager::UpdateInput(UiLepra::InputElement* pElement)
{
	bool lValueSet;
	float lValue = (float)pElement->GetValue();
	const str lInputElementName = pElement->GetFullName();
	if (pElement->GetType() == UiLepra::InputElement::ANALOGUE)
	{
		// Clamp analogue to neutral when close enough.
		if (Math::IsEpsEqual(lValue, 0.0f, 0.1f))
		{
			lValue = 0;
		}
		// Update both sides for analogue input.
		if (lValue >= 0)
		{
			lValueSet = SetValue(lInputElementName+_T("+"), lValue);
			SetValue(lInputElementName+_T("-"), 0);
		}
		else
		{
			lValueSet = SetValue(lInputElementName+_T("-"), -lValue);
			SetValue(lInputElementName+_T("+"), 0);
		}
	}
	else
	{
		lValueSet = SetValue(lInputElementName, lValue);
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

const str OptionsManager::ConvertToString(UiLepra::InputManager::KeyCode pKeyCode)
{
	return (_T("Key.")+UiLepra::InputManager::GetKeyName(pKeyCode));
}

bool OptionsManager::SetValue(const str& pKey, float pValue)
{
	bool lIsAnySteeringValue;
	bool lInputChanged = false;
	std::vector<float*> lValuePointers = GetValuePointers(pKey, lIsAnySteeringValue);
	for (std::vector<float*>::iterator x = lValuePointers.begin(); x != lValuePointers.end(); ++x)
	{
		if (!Math::IsEpsEqual(*(*x), pValue, 0.06f))
		{
			lInputChanged = true;
			*(*x) = pValue;
		}
	}
	return (lIsAnySteeringValue && lInputChanged);
}

std::vector<float*> OptionsManager::GetValuePointers(const str& pKey, bool& pIsAnySteeringValue)
{
	const KeyValue lEntries[] =
	{
		KeyValue(_T(RTVAR_CTRL_UI_CONTOGGLE), &mConsoleToggle),
	};
	return (DoGetValuePointers(pKey, pIsAnySteeringValue, lEntries, sizeof(lEntries)/sizeof(lEntries[0])));
}

std::vector<float*> OptionsManager::DoGetValuePointers(const str& pKey, bool& pIsAnySteeringValue,
	const KeyValue pEntries[], size_t pEntryCount)
{
	std::vector<float*> lPointers;
	pIsAnySteeringValue = false;
	for (size_t x = 0; x < pEntryCount; ++x)
	{
		const str lKeys = mVariableScope->GetDefaultValue(Cure::RuntimeVariableScope::READ_ONLY,
			pEntries[x].first);
		typedef strutil::strvec SV;
		SV lKeyArray = strutil::Split(lKeys, _T(", \t"));
		for (SV::iterator y = lKeyArray.begin(); y != lKeyArray.end(); ++y)
		{
			if ((*y) == pKey)
			{
				if (pEntries[x].first.find(_T("Steer")) != str::npos)
				{
					pIsAnySteeringValue = true;
				}
				lPointers.push_back(pEntries[x].second);
			}
		}
	}
	return (lPointers);
}



void OptionsManager::operator=(const OptionsManager&)
{
	assert(false);
}



LOG_CLASS_DEFINE(UI_INPUT, OptionsManager);



}
}
