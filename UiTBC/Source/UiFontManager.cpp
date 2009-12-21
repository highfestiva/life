
// Author: Alexander Hugestrand, Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/UiFontManager.h"



namespace UiTbc
{



FontManager::FontManager():
	mFontIdManager(1, 10000, 0),
	mCurrentFont(0)
{
}

FontManager::~FontManager()
{
}


void FontManager::SetActiveFont(FontId pFontId)
{
	if (mCurrentFont && mCurrentFont->mFontId == pFontId)
	{
		return;
	}

	FontTable::iterator x = mFontTable.find(pFontId);
	if (x != mFontTable.end())
	{
		mCurrentFont = x->second;
	}
}

FontManager::FontId FontManager::GetActiveFont() const
{
	if (mCurrentFont)
	{
		return (mCurrentFont->mFontId);
	}
	return (INVALID_FONTID);
}



int FontManager::GetStringWidth(const str& pString) const
{
	int lMaxX = 0;
	int lCurrentX = 0;
	for (size_t i = 0; i < pString.length(); i++)
	{
		tchar lChar = pString[i];

		if (lChar == _T('\n'))
		{
			lCurrentX = 0;
		}
		else if(lChar != _T('\r') && 
			lChar != _T('\b') &&
			lChar != _T('\t'))
		{
			lCurrentX += GetCharWidth(lChar);
		}

		if (lCurrentX > lMaxX)
		{
			lMaxX = lCurrentX;
		}
	}
	return (lMaxX);
}

int FontManager::GetFontHeight() const
{
	if (!mCurrentFont)
	{
		return (0);
	}
	return (int)(mCurrentFont->mSize + 0.5);
}

int FontManager::GetLineHeight() const
{
	if (!mCurrentFont)
	{
		return (0);
	}
	return (int)(mCurrentFont->mSize + 3.5);	// Add some extra pixels for distance to next line.
}



bool FontManager::InternalAddFont(Font* pFont)
{
	const int lId = mFontIdManager.GetFreeId();
	bool lOk = (lId != 0);
	if (lOk)
	{
		pFont->mFontId = (FontId)lId;
		mFontTable.insert(FontTable::value_type(lId, pFont));
	}
	return (lOk);
}



}
