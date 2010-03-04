
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "../../Include/GUI/UiCustomButton.h"



namespace UiTbc
{



CustomButton::CustomButton(const str& pText):
	Parent(pText),
	mOnRender(0),
	mIsOver(0)
{
}

CustomButton::~CustomButton()
{
	delete (mOnRender);
	delete (mIsOver);
}

void CustomButton::SetOnRenderDelegate(const Delegate& pOnRenderer)
{
	mOnRender = new Delegate(pOnRenderer);
}

void CustomButton::SetIsOverDelegate(const DelegateXY& pIsOver)
{
	mIsOver = new DelegateXY(pIsOver);
}

void CustomButton::Repaint(Painter*)
{
	if (mOnRender)
	{
		(*mOnRender)(this);
	}
}

bool CustomButton::IsOver(int pScreenX, int pScreenY)
{
	if (mIsOver)
	{
		return (*mIsOver)(this, pScreenX, pScreenY);
	}
	return (false);
}



}
