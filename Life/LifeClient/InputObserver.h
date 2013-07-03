
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../UiLepra/Include/UiInput.h"



namespace Life
{



class InputObserver: public UiLepra::KeyCodeInputObserver
{
public:
	virtual void OnInput(UiLepra::InputElement* pElement) = 0;
};



}
