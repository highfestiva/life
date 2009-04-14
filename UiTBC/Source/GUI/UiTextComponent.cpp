
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games



#include "../../Include/GUI/UiTextComponent.h"



namespace UiTbc
{



TextComponent::TextComponent():
	m_FontID(Painter::INVALID_FONTID),
	m_TextColor(Lepra::BLACK),
	m_TextBlendFunc(Component::NO_BLEND),
	m_TextAlpha(128)
{
}

TextComponent::~TextComponent()
{
}

void TextComponent::SetFont(Painter::FontID p_Font, const Lepra::Color& p_TextColor,
	Component::BlendFunc p_TextBlendFunc, Lepra::uint8 p_TextAlpha)
{
	m_FontID = p_Font;
	m_TextColor = p_TextColor;
	m_TextBlendFunc = p_TextBlendFunc;
	m_TextAlpha = p_TextAlpha;
	ForceRepaint();
}



Painter::FontID TextComponent::GetFontId() const
{
	return m_FontID;
}

Lepra::Color TextComponent::GetTextColor() const
{
	return m_TextColor;
}

Component::BlendFunc TextComponent::GetTextBlendFunc() const
{
	return m_TextBlendFunc;
}

Lepra::uint8 TextComponent::GetTextAlpha() const
{
	return m_TextAlpha;
}



}
