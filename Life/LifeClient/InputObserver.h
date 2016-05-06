
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../uilepra/include/uiinput.h"



namespace life {



class InputObserver: public uilepra::KeyCodeInputObserver {
public:
	virtual void OnInput(uilepra::InputElement* element) = 0;
};



}
