
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "uibutton.h"



namespace uitbc {



class RadioButton : public Button {
	typedef Button Parent;
public:

	RadioButton(const Color& body_color, const wstr& text);
	virtual ~RadioButton();

	virtual void SetBaseColor(const Color& color);
	virtual bool OnLButtonUp(int mouse_x, int mouse_y);
	virtual Type GetType() const;
};



}
