
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "UiButton.h"



namespace UiTbc
{



class RadioButton : public Button
{
	typedef Button Parent;
public:

	RadioButton(const Color& pBodyColor, const str& pName);
	virtual ~RadioButton();

	virtual void SetBaseColor(const Color& pColor);
	virtual bool OnLButtonUp(int pMouseX, int pMouseY);
	virtual Type GetType() const;
};



}
