
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once
#include "uibutton.h"



namespace uitbc {



class CustomButton;


#define SetOnRender(_class, _func) \
	SetOnRenderDelegate(uitbc::ButtonType<uitbc::CustomButton>::Delegate(this, &_class::_func))

#define SetOnIsOver(_class, _func) \
	SetIsOverDelegate(uitbc::ButtonType<uitbc::CustomButton>::DelegateXY(this, &_class::_func))



class CustomButton: public Button {
	typedef Button Parent;
public:
	typedef ButtonType<CustomButton>::Delegate Delegate;
	typedef ButtonType<CustomButton>::DelegateXY DelegateXY;

	CustomButton(const wstr& text);
	virtual ~CustomButton();

	void SetOnRenderDelegate(const Delegate& on_renderer);
	void SetIsOverDelegate(const DelegateXY& is_over);

	virtual void Repaint(Painter*);
	virtual bool IsOver(int screen_x, int screen_y);

private:
	Delegate* on_render_;
	DelegateXY* is_over_;
};



}
