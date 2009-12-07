
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "UiComponent.h"



namespace UiTbc
{



class TextComponent
{
public:
	TextComponent();
	virtual ~TextComponent();

	void SetFont(const Lepra::Color& pTextColor, Component::BlendFunc pTextBlendFunc, Lepra::uint8 pTextAlpha);

protected:
	virtual void ForceRepaint() = 0;

	Lepra::Color GetTextColor() const;
	Component::BlendFunc GetTextBlendFunc() const;
	Lepra::uint8 GetTextAlpha() const;

private:
	Lepra::Color mTextColor;
	Component::BlendFunc mTextBlendFunc;
	Lepra::uint8 mTextAlpha;
};



}
