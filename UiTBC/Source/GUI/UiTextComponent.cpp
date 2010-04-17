
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../../Include/GUI/UiTextComponent.h"



namespace UiTbc
{



TextComponent::TextComponent():
	mTextColor(OFF_BLACK)
{
}

TextComponent::~TextComponent()
{
}

void TextComponent::SetFontColor(const Color& pTextColor)
{
	assert(pTextColor != BLACK);
	mTextColor = pTextColor;
	ForceRepaint();
}



Color TextComponent::GetTextColor() const
{
	return (mTextColor);
}



}
