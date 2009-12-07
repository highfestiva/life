
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../../Include/GUI/UiTextComponent.h"



namespace UiTbc
{



TextComponent::TextComponent():
	mTextColor(Lepra::BLACK),
	mTextBlendFunc(Component::NO_BLEND),
	mTextAlpha(128)
{
}

TextComponent::~TextComponent()
{
}

void TextComponent::SetFont(const Lepra::Color& pTextColor,
	Component::BlendFunc pTextBlendFunc, Lepra::uint8 pTextAlpha)
{
	mTextColor = pTextColor;
	mTextBlendFunc = pTextBlendFunc;
	mTextAlpha = pTextAlpha;
	ForceRepaint();
}



Lepra::Color TextComponent::GetTextColor() const
{
	return (mTextColor);
}

Component::BlendFunc TextComponent::GetTextBlendFunc() const
{
	return (mTextBlendFunc);
}

Lepra::uint8 TextComponent::GetTextAlpha() const
{
	return (mTextAlpha);
}



}
