
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../Include/GUI/UiCustomButton.h"



namespace UiTbc
{



CustomButton::CustomButton(const wstr& pText):
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
	return Parent::IsOver(pScreenX, pScreenY);
}



}
