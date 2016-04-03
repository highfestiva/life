
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/UiFontTexture.h"
#include "../../Lepra/Include/ResourceTracker.h"
#include "../Include/UiFontManager.h"



namespace UiTbc
{



FontTexture::FontTexture(uint32 pFontHash, int pFontHeight):
	mCanvas(64, Canvas::PowerUp(pFontHeight), Canvas::BITDEPTH_32_BIT),
	mFontHash(pFontHash),
	mFreeXOffset(0),
	mIsUpdated(false)
{
	LEPRA_ACQUIRE_RESOURCE(FontTexture);
	mCanvas.CreateBuffer();
}

FontTexture::~FontTexture()
{
	LEPRA_RELEASE_RESOURCE(FontTexture);
}

uint32 FontTexture::GetFontHash() const
{
	return (mFontHash);
}

void FontTexture::StoreGlyph(wchar_t pChar, FontManager* pFontManager)
{
	if (pChar == ' ' || pChar == '\r' || pChar == '\n' || pChar == '\t' || pChar == '\b')
	{
		return;
	}

	GlyphXMap::const_iterator x = mGlyphXOffsetMap.find(pChar);
	if (x == mGlyphXOffsetMap.end())
	{
		const int lCharStartX = mFreeXOffset;
		const int lWidth = pFontManager->GetCharWidth(pChar);
		const int lOffset = pFontManager->GetCharOffset(pChar);
		mGlyphXOffsetMap.insert(GlyphXMap::value_type(pChar, GlyphX{lCharStartX, lWidth, lOffset}));
		mFreeXOffset += lWidth;
		if (mFreeXOffset > (int)mCanvas.GetWidth())
		{
			mCanvas.Crop(0, 0, mCanvas.GetWidth()*2, mCanvas.GetHeight());
		}
		Canvas lCharCanvas(lWidth, mCanvas.GetHeight(), mCanvas.GetBitDepth());
		PixelRect lCanvasRect(0, 0, lWidth, mCanvas.GetHeight());
		pFontManager->RenderGlyph(pChar, lCharCanvas, lCanvasRect);
		mCanvas.PartialCopy(lCharStartX, 0, lCharCanvas);

		/*printf("Updated texture with glyph '%c' (height=%i):\n", pChar, mCanvas.GetHeight());
		const int lMaxWidth = (mCanvas.GetWidth() > 80)? 80 : mCanvas.GetWidth();
		for (int y = 0; y < (int)mCanvas.GetHeight(); ++y)
		{
			for (int x = 0; x < lMaxWidth; ++x)
			{
				if (mCanvas.GetPixelColor(x, y).To32() == 0)
				{
					printf(" ");
				}
				else
				{
					printf("*");
				}
			}
			printf("\n");
		}*/

		mIsUpdated = true;
	}
}

bool FontTexture::GetGlyphX(wchar_t pChar, int& pX, int& pWidth, int& pPlacementOffset) const
{
	GlyphXMap::const_iterator x = mGlyphXOffsetMap.find(pChar);
	deb_assert(x != mGlyphXOffsetMap.end());
	if (x != mGlyphXOffsetMap.end())
	{
		pX = x->second.mStartX;
		pWidth = x->second.mWidth;
		pPlacementOffset = x->second.mPlacementOffset;
		return (true);
	}
	return (false);
}

bool FontTexture::IsUpdated() const
{
	return (mIsUpdated);
}

void FontTexture::ResetIsUpdated()
{
	mIsUpdated = false;
}

int FontTexture::GetWidth() const
{
	return (mCanvas.GetWidth());
}

int FontTexture::GetHeight() const
{
	return (mCanvas.GetHeight());
}

void* FontTexture::GetBuffer() const
{
	return (mCanvas.GetBuffer());
}



}
