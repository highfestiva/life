
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "UiTbc.h"
#include "../../Lepra/Include/Unordered.h"
#include "../../Lepra/Include/Canvas.h"



namespace UiTbc
{


class FontManager;




// Stores glyphs in a texture along side each other. All glyphs appear at y=0.
class FontTexture
{
public:
	FontTexture(uint32 pFontHash, int pFontHeight);
	virtual ~FontTexture();

	uint32 GetFontHash() const;

	void StoreGlyph(wchar_t pChar, FontManager* pFontManager);
	bool GetGlyphX(wchar_t pChar, int& pX, int& pWidth, int& pPlacementOffset) const;

	bool IsUpdated() const;
	void ResetIsUpdated();

	int GetWidth() const;
	int GetHeight() const;
	void* GetBuffer() const;

private:
	struct GlyphX
	{
		int mStartX;
		int mWidth;
		int mPlacementOffset;
	};
	typedef std::unordered_map<int32, GlyphX> GlyphXMap;
	GlyphXMap mGlyphXOffsetMap;

	Canvas mCanvas;
	uint32 mFontHash;
	int mFreeXOffset;
	bool mIsUpdated;
};



}
