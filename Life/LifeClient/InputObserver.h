
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



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
