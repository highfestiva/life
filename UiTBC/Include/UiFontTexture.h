
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "UiTBC.h"
#include <hash_map>
#include "../../Lepra/Include/Canvas.h"



namespace UiTbc
{


class FontManager;




class FontTexture
{
public:
	FontTexture(uint32 pFontHash, int pFontHeight);
	virtual ~FontTexture();

	uint32 GetFontHash() const;

	void StoreGlyph(tchar pChar, FontManager* pFontManager);
	int GetGlyphXOffset(tchar pChar);

	bool IsResized() const;
	void ResetIsResized();

	int GetWidth() const;
	void* GetBuffer() const;

private:
	typedef std::hash_map<int32, int> GlyphXOffsetMap;
	GlyphXOffsetMap mGlyphXOffsetMap;

	Canvas mCanvas;
	uint32 mFontHash;
	int mFreeXOffset;
	bool mIsResized;
};



}
