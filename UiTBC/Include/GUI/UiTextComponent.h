
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games



#pragma once

#include "UiComponent.h"



namespace UiTbc
{

class TextComponent
{
public:
	TextComponent();
	virtual ~TextComponent();

	void SetFont(Painter::FontID p_Font, const Lepra::Color& p_TextColor,
		Component::BlendFunc p_TextBlendFunc, Lepra::uint8 p_TextAlpha);

protected:
	virtual void ForceRepaint() = 0;

	Painter::FontID GetFontId() const;
	Lepra::Color GetTextColor() const;
	Component::BlendFunc GetTextBlendFunc() const;
	Lepra::uint8 GetTextAlpha() const;

private:
	Painter::FontID m_FontID;
	Lepra::Color m_TextColor;
	Component::BlendFunc m_TextBlendFunc;
	Lepra::uint8 m_TextAlpha;
};



}
