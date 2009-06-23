
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../../Lepra/Include/Log.h"
#include "../../UiLepra/Include/UiInput.h"
#include "ClientOptions.h"



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

	const ClientOptions& GetOptions() const;

private:
	bool SetDefault(int pPriority);

	const Lepra::String ConvertToString(UiLepra::InputManager::KeyCode pKeyCode) const;
	const Lepra::String ConvertToString(UiLepra::InputElement* pElement) const;
	bool SetValue(const Lepra::String& pKey, float pValue);
	float* GetValuePointer(const Lepra::String& pKey);

	Cure::RuntimeVariableScope* mVariableScope;
	ClientOptions mOptions;

	void operator=(const ClientOptionsManager&);

	LOG_CLASS_DECLARE();
};



}
}
