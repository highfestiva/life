
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../../Lepra/Include/Log.h"
#include "../../UiLepra/Include/UiInput.h"
#include "../Life.h"



namespace Cure
{
class RuntimeVariableScope;
}



namespace Life
{
namespace Options
{



struct OptionsManager
{
	OptionsManager(Cure::RuntimeVariableScope* pVariableScope, int pPriority);

	bool UpdateInput(UiLepra::InputManager::KeyCode pKeyCode, bool pActive);
	float UpdateInput(UiLepra::InputElement* pElement);
	void ResetToggles();
	float GetConsoleToggle() const;

protected:
	typedef std::pair<const str, float*> KeyValue;

	virtual bool SetDefault(int pPriority);

	static const str ConvertToString(UiLepra::InputManager::KeyCode pKeyCode);
	bool SetValue(const str& pKey, float pValue);
	virtual std::vector<float*> GetValuePointers(const str& pKey, bool& pIsAnySteeringValue);
	std::vector<float*> DoGetValuePointers(const str& pKey, bool& pIsAnySteeringValue, const KeyValue pEntries[], size_t pEntryCount);

	Cure::RuntimeVariableScope* mVariableScope;
	float mConsoleToggle;

	const static KeyValue mKeyValues[];

	void operator=(const OptionsManager&);

	LOG_CLASS_DECLARE();
};



}
}
