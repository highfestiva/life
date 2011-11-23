
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../../Include/GUI/UiTextComponent.h"
#include "../../Include/UiPainter.h"



namespace UiTbc
{



TextComponent::TextComponent():
	mTextColor(OFF_BLACK),
	mFontId(FontManager::INVALID_FONTID),
	mIsFontActive(false)
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

void TextComponent::SetFontId(const FontManager::FontId& pFontId)
{
	assert(!mIsFontActive);
	mFontId = pFontId;
	ForceRepaint();
}

void TextComponent::ActivateFont(Painter* pPainter)
{
	assert(!mIsFontActive);
	if (mIsFontActive)
	{
		return;
	}
	if (mFontId != FontManager::INVALID_FONTID)
	{
		const FontManager::FontId lOldFontId = pPainter->GetFontManager()->GetActiveFontId();
		pPainter->GetFontManager()->SetActiveFont(mFontId);
		mFontId = lOldFontId;
		mIsFontActive = true;
	}
}

void TextComponent::DeactivateFont(Painter* pPainter)
{
	assert(mIsFontActive || mFontId == FontManager::INVALID_FONTID);
	if (mIsFontActive && mFontId != FontManager::INVALID_FONTID)
	{
		const FontManager::FontId lOurFontId = pPainter->GetFontManager()->GetActiveFontId();
		pPainter->GetFontManager()->SetActiveFont(mFontId);
		mFontId = lOurFontId;
		mIsFontActive = false;
	}
}

void TextComponent::DoPrintText(Painter* pPainter, const str& pText, int x, int y)
{
	pPainter->SetColor(mTextColor, 0);
	pPainter->PrintText(pText.c_str(), x, y);
}

void TextComponent::PrintTextDeactivate(Painter* pPainter, const str& pText, int x, int y)
{
	DoPrintText(pPainter, pText, x, y);
	DeactivateFont(pPainter);
}



Color TextComponent::GetTextColor() const
{
	return (mTextColor);
}



}
