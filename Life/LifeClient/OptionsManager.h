
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
	typedef std::vector<float*> ValueArray;

	OptionsManager(Cure::RuntimeVariableScope* pVariableScope, int pPriority);

	void RefreshConfiguration();
	virtual void DoRefreshConfiguration();
	bool UpdateInput(UiLepra::InputManager::KeyCode pKeyCode, bool pActive);
	virtual bool UpdateInput(UiLepra::InputElement* pElement);
	void ResetToggles();
	bool IsToggleConsole() const;

	ValueArray* GetValuePointers(const str& pKey, bool& pIsAnySteeringValue);

protected:
	typedef std::pair<const str, float*> KeyValue;

	virtual bool SetDefault(int pPriority);

	static const str ConvertToString(UiLepra::InputManager::KeyCode pKeyCode);
	bool SetValue(const str& pKey, float pValue, bool pAdd);
	void SetValuePointers(const KeyValue pEntries[], size_t pEntryCount);

	struct InputEntry
	{
		bool mIsAnySteeringValue;
		ValueArray mValuePointerArray;
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
