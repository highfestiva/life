
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "ClientOptions.h"
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



struct ClientOptionsManager
{
	ClientOptionsManager(Cure::RuntimeVariableScope* pVariableScope, int pPriority);

	bool UpdateInput(UiLepra::InputManager::KeyCode pKeyCode, bool pActive);
	bool UpdateInput(UiLepra::InputElement* pElement);
	void ResetToggles();

	const ClientOptions& GetOptions() const;

private:
	bool SetDefault(int pPriority);

	const str ConvertToString(UiLepra::InputManager::KeyCode pKeyCode) const;
	bool SetValue(const str& pKey, float pValue);
	std::vector<float*> GetValuePointers(const str& pKey, bool& pIsAnySteeringValue);

	Cure::RuntimeVariableScope* mVariableScope;
	ClientOptions mOptions;

	void operator=(const ClientOptionsManager&);

	LOG_CLASS_DECLARE();
};



}
}
