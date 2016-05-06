
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once
#include "uibutton.h"



namespace uitbc {



class CheckButton: public Button {
	typedef Button Parent;
public:
	CheckButton(const Color& body_color, const wstr& text);
	virtual ~CheckButton();

	virtual void Repaint(Painter* painter);
	virtual bool OnLButtonUp(int mouse_x, int mouse_y);

	void SetCheckedIcon(Painter::ImageID icon_id);
	virtual Painter::ImageID GetCurrentIcon() const;

protected:
	Painter::ImageID checked_icon_id_;
};



}
