
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once
#include "UiButton.h"



namespace UiTbc
{



class CustomButton;


#define SetOnRender(_class, _func) \
	SetOnRenderDelegate(UiTbc::ButtonType<UiTbc::CustomButton>::Delegate(this, &_class::_func))

#define SetOnIsOver(_class, _func) \
	SetIsOverDelegate(UiTbc::ButtonType<UiTbc::CustomButton>::DelegateXY(this, &_class::_func))



class CustomButton: public Button
{
	typedef Button Parent;
public:
	typedef ButtonType<CustomButton>::Delegate Delegate;
	typedef ButtonType<CustomButton>::DelegateXY DelegateXY;

	CustomButton(const str& pName);
	virtual ~CustomButton();

	void SetOnRenderDelegate(const Delegate& pOnRenderer);
	void SetIsOverDelegate(const DelegateXY& pIsOver);

	virtual void Repaint(Painter*);
	virtual bool IsOver(int pScreenX, int pScreenY);

private:
	Delegate* mOnRender;
	DelegateXY* mIsOver;
};



}
