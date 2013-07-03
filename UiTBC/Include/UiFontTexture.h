
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "UiTBC.h"
#include <hash_map>
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

	void StoreGlyph(tchar pChar, FontManager* pFontManager);
	bool GetGlyphX(tchar pChar, int& pX, int& pWidth) const;

	bool IsUpdated() const;
	void ResetIsUpdated();

	int GetWidth() const;
	int GetHeight() const;
	void* GetBuffer() const;

private:
	typedef std::pair<int, int> GlyphX;
	typedef std::hash_map<int32, GlyphX> GlyphXMap;
	GlyphXMap mGlyphXOffsetMap;

	Canvas mCanvas;
	uint32 mFontHash;
	int mFreeXOffset;
	bool mIsUpdated;
};



}
