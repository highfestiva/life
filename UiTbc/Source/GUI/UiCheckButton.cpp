
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../Include/GUI/UiCheckButton.h"



namespace UiTbc
{



CheckButton::CheckButton(const Color& pBodyColor, const wstr& pText):
	Parent(pBodyColor, pText),
	mCheckedIconId(Painter::INVALID_IMAGEID)
{
}

CheckButton::~CheckButton()
{
}

void CheckButton::Repaint(Painter* pPainter)
{
	Parent::Repaint(pPainter);
}

bool CheckButton::OnLButtonUp(int pMouseX, int pMouseY)
{
	if (IsOver(pMouseX, pMouseY) == true)
	{
		switch(GetState())
		{
		case RELEASING:
			SetPressed(false);
			break;
		case PRESSING:
			SetPressed(true);
			break;
		default:
			break;
		}
		if (mOnClick != 0)
		{
			(*mOnClick)(this);
		}
	}
	else
	{
		// Go back to previous state.
		switch(GetState())
		{
		case RELEASING:
			SetState(PRESSED);
			break;
		case PRESSING:
			SetState(RELEASED);
			break;
		default:
			break;
		}
	}

	ReleaseMouseFocus();
	return true;
}

void CheckButton::SetCheckedIcon(Painter::ImageID pIconId)
{
	mCheckedIconId = pIconId;
}

Painter::ImageID CheckButton::GetCurrentIcon() const
{
	State lState = GetState();
	if (mCheckedIconId != Painter::INVALID_IMAGEID && (lState == PRESSED || lState == PRESSED_HOOVER))
	{
		return mCheckedIconId;
	}
	return Parent::GetCurrentIcon();
}



}
