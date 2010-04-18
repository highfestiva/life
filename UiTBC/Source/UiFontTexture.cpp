
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "../Include/UiFontTexture.h"
#include "../Include/UiFontManager.h"



namespace UiTbc
{



FontTexture::FontTexture(uint32 pFontHash, int pFontHeight):
	mCanvas(64, Canvas::PowerUp(pFontHeight), Canvas::BITDEPTH_32_BIT),
	mFontHash(pFontHash),
	mFreeXOffset(0),
	mIsResized(false)
{
	mCanvas.CreateBuffer();
}

FontTexture::~FontTexture()
{
}

uint32 FontTexture::GetFontHash() const
{
	return (mFontHash);
}

void FontTexture::StoreGlyph(tchar pChar, FontManager* pFontManager)
{
	if (pChar == ' ' || pChar == '\r' || pChar == '\n' || pChar == '\t')
	{
		return;
	}

	GlyphXOffsetMap::const_iterator x = mGlyphXOffsetMap.find(pChar);
	if (x == mGlyphXOffsetMap.end())
	{
		mGlyphXOffsetMap.insert(GlyphXOffsetMap::value_type(pChar, mFreeXOffset));
		const int lCharStartX = mFreeXOffset;
		const int lWidth = pFontManager->GetCharWidth(pChar);
		mFreeXOffset += lWidth;
		if (mFreeXOffset > (int)mCanvas.GetWidth())
		{
			mCanvas.Crop(0, 0, mCanvas.GetWidth()*2, mCanvas.GetHeight()*2);
			mIsResized = true;
		}
		Canvas lCharCanvas;
		PixelRect lCanvasRect(0, 0, lWidth, mCanvas.GetHeight());
		pFontManager->RenderGlyph(pChar, lCharCanvas, lCanvasRect);
		mCanvas.PartialCopy(lCharStartX, 0, lCharCanvas);
	}
}

int FontTexture::GetGlyphXOffset(tchar pChar)
{
	GlyphXOffsetMap::const_iterator x = mGlyphXOffsetMap.find(pChar);
	assert(x != mGlyphXOffsetMap.end());
	return (x->second);
}

bool FontTexture::IsResized() const
{
	return (mIsResized);
}

void FontTexture::ResetIsResized()
{
	mIsResized = false;
}

int FontTexture::GetWidth() const
{
	return (mCanvas.GetWidth());
}

void* FontTexture::GetBuffer() const
{
	return (mCanvas.GetBuffer());
}



}
