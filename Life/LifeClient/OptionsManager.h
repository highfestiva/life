
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../../Lepra/Include/HiResTimer.h"
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

	void RefreshConfiguration();
	bool UpdateInput(UiLepra::InputManager::KeyCode pKeyCode, bool pActive);
	float UpdateInput(UiLepra::InputElement* pElement);
	void ResetToggles();
	float GetConsoleToggle() const;

protected:
	typedef std::pair<const str, float*> KeyValue;

	virtual bool SetDefault(int pPriority);
	virtual void DoRefreshConfiguration();

	static const str ConvertToString(UiLepra::InputManager::KeyCode pKeyCode);
	bool SetValue(const str& pKey, float pValue, bool pAdd);
	std::vector<float*>* GetValuePointers(const str& pKey, bool& pIsAnySteeringValue);
	void SetValuePointers(const KeyValue pEntries[], size_t pEntryCount);

	struct InputEntry
	{
		bool mIsAnySteeringValue;
		std::vector<float*> mValuePointerArray;
	};
	typedef std::hash_map<str, InputEntry> InputMap;

	Cure::RuntimeVariableScope* mVariableScope;
	float mConsoleToggle;
	HiResTimer mLastConfigRefresh;
	InputMap mInputMap;

	void operator=(const OptionsManager&);

	LOG_CLASS_DECLARE();
};



}
}
