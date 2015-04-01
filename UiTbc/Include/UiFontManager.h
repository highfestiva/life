
// Author: Jonas Bystr�m
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../Lepra/Include/LepraTarget.h"	// Must be first.
#include "../../Lepra/Include/Unordered.h"
#include "../../Lepra/Include/Canvas.h"
#include "../../Lepra/Include/IdManager.h"
#include "../../Lepra/Include/String.h"
#include "../Include/UiTbc.h"



namespace UiLepra
{
class DisplayManager;
}



namespace UiTbc
{



class FontManager
{
public:
	enum FontId
	{
		INVALID_FONTID = 0,
	};
	enum FontFlags
	{
		NORMAL		= 0,
		BOLD		= (1 << 0),
		ITALIC		= (1 << 1),
		UNDERLINE	= (1 << 2),
		STRIKEOUT	= (1 << 3),
	};
	enum CharacterSet
	{
		NATIVE = 1,
		ANSI,
	};

	static FontManager* Create(UiLepra::DisplayManager* pDisplayManager);
	FontManager();
	virtual ~FontManager();

	virtual void SetColor(const Color& pColor, unsigned pColorIndex = 0) = 0;

	FontId QueryAddFont(const str& pFontName, double pSize, int pFlags = NORMAL, CharacterSet pCharSet = NATIVE);
	virtual FontId AddFont(const str& pFontName, double pSize, int pFlags = NORMAL, CharacterSet pCharSet = NATIVE) = 0;
	void SetActiveFont(FontId pFontId);
	FontId GetActiveFontId() const;
	str GetActiveFontName() const;

	virtual bool RenderGlyph(tchar pChar, Canvas& pImage, const PixelRect& pRect) = 0;

	virtual int GetCharWidth(const tchar pChar) const = 0;
	int GetStringWidth(const str& pString) const;
	int GetStringHeight(const str& pString) const;
	int GetFontHeight() const;
	int GetLineHeight() const;	// Font height + some extra pixels as distance to next line.

protected:
	struct Font
	{
		str mName;
		double mSize;
		int mFlags;
		FontId mFontId;
	};
	typedef std::unordered_map<int, Font*> FontTable;

	bool InternalAddFont(Font* pFont);

	FontTable mFontTable;
	Font* mCurrentFont;

private:
	IdManager<int> mFontIdManager;
};



}