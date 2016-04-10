
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once
#include "UiButton.h"



namespace UiTbc
{



class CheckButton: public Button
{
	typedef Button Parent;
public:
	CheckButton(const Color& pBodyColor, const wstr& pText);
	virtual ~CheckButton();

	virtual void Repaint(Painter* pPainter);
	virtual bool OnLButtonUp(int pMouseX, int pMouseY);

	void SetCheckedIcon(Painter::ImageID pIconId);
	virtual Painter::ImageID GetCurrentIcon() const;

protected:
	Painter::ImageID mCheckedIconId;
};



}
