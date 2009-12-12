
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../../Include/GUI/UiTextComponent.h"



namespace UiTbc
{



TextComponent::TextComponent():
	mTextColor(Lepra::OFF_BLACK)
{
}

TextComponent::~TextComponent()
{
}

void TextComponent::SetFontColor(const Lepra::Color& pTextColor)
{
	assert(pTextColor != Lepra::BLACK);
	mTextColor = pTextColor;
	ForceRepaint();
}



Lepra::Color TextComponent::GetTextColor() const
{
	return (mTextColor);
}



}
