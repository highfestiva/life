
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../../Include/GUI/UiTextComponent.h"
#include "../../Include/UiPainter.h"



namespace UiTbc
{



TextComponent::TextComponent():
	mTextColor(OFF_BLACK),
	mFontId(FontManager::INVALID_FONTID),
	mIsFontActive(false),
	mVAlignment(VALIGN_CENTER),
	mHorizontalMargin(3)
{
}

TextComponent::~TextComponent()
{
}

void TextComponent::SetFontColor(const Color& pTextColor)
{
	deb_assert(pTextColor != BLACK);
	mTextColor = pTextColor;
	ForceRepaint();
}

void TextComponent::SetFontId(const FontManager::FontId& pFontId)
{
	deb_assert(!mIsFontActive);
	mFontId = pFontId;
	ForceRepaint();
}

void TextComponent::ActivateFont(Painter* pPainter)
{
	deb_assert(!mIsFontActive);
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
	deb_assert(mIsFontActive || mFontId == FontManager::INVALID_FONTID);
	if (mIsFontActive && mFontId != FontManager::INVALID_FONTID)
	{
		const FontManager::FontId lOurFontId = pPainter->GetFontManager()->GetActiveFontId();
		pPainter->GetFontManager()->SetActiveFont(mFontId);
		mFontId = lOurFontId;
		mIsFontActive = false;
	}
}

void TextComponent::SetHorizontalMargin(int pHorizontalMargin)
{
	mHorizontalMargin = pHorizontalMargin;
}

void TextComponent::SetVericalAlignment(VAlign pAlignment)
{
	mVAlignment = pAlignment;
}

void TextComponent::DoPrintText(Painter* pPainter, const str& pText, int x, int y)
{
	pPainter->SetColor(mTextColor, 0);
	pPainter->PrintText(pText, x, y);
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

TextComponent::VAlign TextComponent::GetVAlign() const
{
	return mVAlignment;
}



}
