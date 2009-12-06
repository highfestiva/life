
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#include <assert.h>
#include "../../Lepra/Include/Canvas.h"
#include "../Include/UiSystemPainter.h"



namespace UiTbc
{



int Painter::smFont1CharWidth[256];
int Painter::smFont2CharWidth[256];
bool Painter::smCharWidthsCalculated = false;

#include "UiStandardFont1.inl"
#include "UiStandardFont2.inl"

Lepra::uint8 Painter::smStandardMouseCursor[] = 
{
	1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //1
	1, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //2
	1, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //3
	1, 2, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //4
	1, 2, 2, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //5
	1, 2, 2, 2, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, //6
	1, 2, 2, 2, 2, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, //7
	1, 2, 2, 2, 2, 2, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, //8
	1, 2, 2, 2, 2, 2, 2, 2, 2, 1, 0, 0, 0, 0, 0, 0, //9
	1, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, //10
	1, 2, 2, 1, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, //11
	1, 2, 1, 0, 1, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, //12
	1, 1, 0, 0, 1, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, //13
	1, 0, 0, 0, 0, 1, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, //14
	0, 0, 0, 0, 0, 1, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, //15
	0, 0, 0, 0, 0, 0, 1, 2, 2, 1, 0, 0, 0, 0, 0, 0, //16
	0, 0, 0, 0, 0, 0, 1, 2, 2, 1, 0, 0, 0, 0, 0, 0, //17
	0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, //18
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //19
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //20
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //21
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //22
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //23
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //24
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //25
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //26
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //27
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //28
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //29
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //30
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //31
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //32
};



Painter::Painter() :
	mFontPainter(0),
	mDisplayListIDManager(1, 100000, 0),
	mCurrentDisplayList(0),
	mTabOriginX(0),
	mFontsAndCursorInitialized(false),
	mRenderMode(RM_NORMAL),
	mOrigoX(0),
	mOrigoY(0),
	mXDir(X_RIGHT),
	mYDir(Y_DOWN),
	mFontIDManager(1, 10000, 0),
	mCurrentFont(0),
	mStandardFontID0(INVALID_FONTID),
	mStandardFontID1(INVALID_FONTID),
	mMouseCursorID(INVALID_IMAGEID),
	mCanvas(0),
	mAlphaValue(255)
{
	if (smCharWidthsCalculated == false)
	{
		CalcCharWidths(smStandardFont1, smFont1CharWidth);
		CalcCharWidths(smStandardFont2, smFont2CharWidth);
		smCharWidthsCalculated = true;
	}

	mDefaultDisplayList = NewDisplayList();
}

Painter::~Painter()
{
	DeleteDisplayList(mDefaultDisplayList);

	FontTable::Iterator lFontIter = mFontTable.First();
	while (lFontIter != mFontTable.End())
	{
		Font* lFont = *lFontIter;
		delete lFont;
		++lFontIter;
	}

	mFontPainter = 0;
}

void Painter::DefineCoordinates(int pOrigoX, int pOrigoY, XDir pXDir, YDir pYDir)
{
	mOrigoX = pOrigoX;
	mOrigoY = pOrigoY;
	mXDir = pXDir;
	mYDir = pYDir;
}

int Painter::CalcAverageWidth(int pCharCount, int* pCharWidthArray)
{
	int lSum = 0;
	for (int x = 0; x < pCharCount; ++x)
	{
		lSum += pCharWidthArray[x];
	}
	return (lSum/pCharCount);
}

void Painter::CalcCharWidths(Lepra::uint8* pFont, int* pWidth)
{
	const int lPitch = 16;

	// Calculate width of each character in the font.
	// (This function should only be used with the standard fonts.)
	for (int i = 0; i < 256; i++)
	{
		// Character x and y.
		int cx = i % 2;
		int cy = i / 2;
		Lepra::uint8* lCharData = &pFont[(cy * lPitch + cx) * 8];

		// Find maximum x where we have a pixel of a value
		// other than zero.

		int lMaxX = -1;

		for (int y = 0; y < 8; y++)
		{
			for (int x = 0; x < 8; x++)
			{
				if (lCharData[y * lPitch + x] != 0 && x > lMaxX)
				{
					lMaxX = x;
				}
			}
		}

		if (lMaxX == -1)
		{
			lMaxX = 0;
		}
		else
		{
			lMaxX++;

			if (lMaxX > 8)
			{
				lMaxX = 8;
			}
		}

		pWidth[i] = lMaxX;
	}
}

void Painter::SetDestCanvas(Lepra::Canvas* pCanvas)
{
	mCanvas = pCanvas;

	// Initialize fonts and mouse cursor here. Can't do it in the constructor 
	// since we need to call virtual functions.
	if (mFontsAndCursorInitialized == false)
	{
		// Grayscale palette.
		Lepra::Color lPalette[256];
		for (int i = 0; i < 256; i++)
		{
			lPalette[i].Set(i, i, i, 0);
		}

		// Create standard fonts.
		Lepra::Canvas lFont0(16, 8 * 128, Lepra::Canvas::BITDEPTH_8_BIT);
		Lepra::Canvas lFont1(16, 8 * 128, Lepra::Canvas::BITDEPTH_8_BIT);

		lFont0.SetBuffer(smStandardFont1);
		lFont1.SetBuffer(smStandardFont2);

		lFont0.SetPalette(lPalette);
		lFont1.SetPalette(lPalette);

		mStandardFontID0 = AddImageFont(lFont0, 8, 8, smFont1CharWidth, 8, 0, 255, -1, 2, 1, 32);
		mStandardFontID1 = AddImageFont(lFont1, 8, 8, smFont2CharWidth, 8, 0, 255, -1, 2, 1, 32);

		SetActiveFont(GetStandardFont(0));

		// Black & white palette.
		lPalette[0].Set(0, 0, 0, 0);
		lPalette[1].Set(255, 255, 255, 0);

		// Mouse cursor canvas.
		Lepra::Canvas lCursor(16, 32, Lepra::Canvas::BITDEPTH_8_BIT);
		lCursor.SetBuffer(smStandardMouseCursor);
		lCursor.SetPalette(lPalette);

		// Create a correct alpha buffer.
		Lepra::Canvas lCursorAlpha(16, 32, Lepra::Canvas::BITDEPTH_8_BIT);
		lCursorAlpha.SetBuffer(smStandardMouseCursor, true);

		Lepra::uint8* lAlpha = (Lepra::uint8*)lCursorAlpha.GetBuffer();
		for (int i = 0; i < 16 * 32; i++)
		{
			lAlpha[i] = (smStandardMouseCursor[i] == 0) ? 0 : 255;
		}

		// Merge cursor bitmap with alpha buffer.
		lCursor.ConvertTo32BitWithAlpha(lCursorAlpha);

		mMouseCursorID = AddImage(&lCursor, 0);

		mFontsAndCursorInitialized = true;
	}
}

void Painter::BeginPaint()
{
	//BeginDisplayList(mDefaultDisplayList);
}

void Painter::EndPaint()
{
	//RenderDisplayList(mDefaultDisplayList);
	//EndDisplayList();
}

bool Painter::PushAttrib(unsigned pAttrib)
{
	bool pReturnValue = true;

	const size_t lMaxCount = 1024;

	if ((pAttrib & ATTR_RENDERMODE) != 0)
	{
		mAttribRMStack.push_back(mRenderMode);
		if (mAttribRMStack.size() > lMaxCount)
		{
			mAttribRMStack.pop_front();
			pReturnValue = false;
		}
	}

	if ((pAttrib & ATTR_ALPHAVALUE) != 0)
	{
		mAttribAlphaStack.push_back(GetAlphaValue());
		if (mAttribAlphaStack.size() > lMaxCount)
		{
			mAttribAlphaStack.pop_front();
			pReturnValue = false;
		}
	}

	if ((pAttrib & ATTR_COLOR0) != 0)
	{
		mAttribColor0Stack.push_back(GetColor(0));
		if (mAttribColor0Stack.size() > lMaxCount)
		{
			mAttribColor0Stack.pop_front();
			pReturnValue = false;
		}
	}

	if ((pAttrib & ATTR_COLOR1) != 0)
	{
		mAttribColor1Stack.push_back(GetColor(1));
		if (mAttribColor1Stack.size() > lMaxCount)
		{
			mAttribColor1Stack.pop_front();
			pReturnValue = false;
		}
	}

	if ((pAttrib & ATTR_COLOR2) != 0)
	{
		mAttribColor2Stack.push_back(GetColor(2));
		if (mAttribColor2Stack.size() > lMaxCount)
		{
			mAttribColor2Stack.pop_front();
			pReturnValue = false;
		}
	}

	if ((pAttrib & ATTR_COLOR3) != 0)
	{
		mAttribColor3Stack.push_back(GetColor(3));
		if (mAttribColor3Stack.size() > lMaxCount)
		{
			mAttribColor3Stack.pop_front();
			pReturnValue = false;
		}
	}

	if ((pAttrib & ATTR_CLIPRECT) != 0)
	{
		Lepra::PixelRect lClippingRect;
		GetClippingRect(lClippingRect);
		mAttribClipRectStack.push_back(lClippingRect);
		if (mAttribClipRectStack.size() > lMaxCount)
		{
			mAttribClipRectStack.pop_front();
			pReturnValue = false;
		}
	}

	mAttribStack.push_back(pAttrib);

	return pReturnValue;
}

bool Painter::PopAttrib()
{
	if (mAttribStack.empty())
	{
		return false;
	}

	unsigned lAttrib = mAttribStack.back();
	mAttribStack.pop_back();

	if ((lAttrib & ATTR_RENDERMODE) != 0)
	{
		if (!mAttribRMStack.empty())
		{
			SetRenderMode(mAttribRMStack.back());
			mAttribRMStack.pop_back();
		}
		else
		{
			return false;
		}
	}

	if ((lAttrib & ATTR_ALPHAVALUE) != 0)
	{
		if (!mAttribAlphaStack.empty())
		{
			SetAlphaValue(mAttribAlphaStack.back());
			mAttribAlphaStack.pop_back();
		}
		else
		{
			return false;
		}
	}

	if ((lAttrib & ATTR_COLOR0) != 0)
	{
		if (!mAttribColor0Stack.empty())
		{
			SetColor(mAttribColor0Stack.back(), 0);
			mAttribColor0Stack.pop_back();
		}
		else
		{
			return false;
		}
	}

	if ((lAttrib & ATTR_COLOR1) != 0)
	{
		if (!mAttribColor1Stack.empty())
		{
			SetColor(mAttribColor1Stack.back(), 1);
			mAttribColor1Stack.pop_back();
		}
		else
		{
			return false;
		}
	}

	if ((lAttrib & ATTR_COLOR2) != 0)
	{
		if (!mAttribColor2Stack.empty())
		{
			SetColor(mAttribColor2Stack.back(), 2);
			mAttribColor2Stack.pop_back();
		}
		else
		{
			return false;
		}
	}

	if ((lAttrib & ATTR_COLOR3) != 0)
	{
		if (!mAttribColor3Stack.empty())
		{
			SetColor(mAttribColor3Stack.back(), 3);
			mAttribColor3Stack.pop_back();
		}
		else
		{
			return false;
		}
	}

	if ((lAttrib & ATTR_CLIPRECT) != 0)
	{
		if (!mAttribClipRectStack.empty())
		{
			Lepra::PixelRect lRect = mAttribClipRectStack.back();
			mAttribClipRectStack.pop_back();
			SetClippingRect(lRect.mLeft, lRect.mTop, lRect.mRight, lRect.mBottom);
		}
		else
		{
			return false;
		}
	}

	return true;
}

void Painter::SetRenderMode(RenderMode pRM)
{
	mRenderMode = pRM;
}

void Painter::SetAlphaValue(Lepra::uint8 pAlpha)
{
	mAlphaValue = pAlpha;
}

void Painter::SetClippingRect(int pLeft, int pTop, int pRight, int pBottom)
{
	mClippingRect.Set(pLeft, pTop, pRight, pBottom);
}

void Painter::ReduceClippingRect(int pLeft, int pTop, int pRight, int pBottom)
{
	Lepra::PixelRect lClippingRect(mClippingRect);

	if (XLT(lClippingRect.mLeft, pLeft) == true)
	{
		lClippingRect.mLeft = pLeft;
	}

	if (XGT(lClippingRect.mRight, pRight) == true)
	{
		lClippingRect.mRight = pRight;
	}

	if (YLT(lClippingRect.mTop, pTop) == true)
	{
		lClippingRect.mTop = pTop;
	}

	if (YGT(lClippingRect.mBottom, pBottom) == true)
	{
		lClippingRect.mBottom = pBottom;
	}

	SetClippingRect(lClippingRect);
}

void Painter::SetColor(const Lepra::Color& pColor, unsigned pColorIndex)
{
	mColor[pColorIndex] = pColor;
}

void Painter::SetFontPainter(SystemPainter* pFontPainter)
{
	mFontPainter = pFontPainter;
}

SystemPainter* Painter::GetFontPainter() const
{
	return (mFontPainter);
}

int Painter::PrintText(const Lepra::String& pString, int x, int y)
{
	return (DoPrintText(pString, x, y));

	/*int lCurrentX = x;
	int lCurrentY = y;

	for (size_t z = 0; z < pString.length(); ++z)
	{
		Lepra::tchar lChar = pString[z];

		if (lChar == _T('\n'))
		{
			lCurrentY += GetLineHeight();
			lCurrentX = x;
		}
		else if(lChar != _T('\r') && 
			lChar != _T('\b') &&
			lChar != _T('\t'))
		{
			Lepra::PixelRect lCharSquare;
			ImageID lCharImageId;
			if (CacheCharImage(lChar, lCharImageId, lCharSquare))
			{
				DrawAlphaImage(lCharImageId, lCurrentX, lCurrentY, lCharSquare);
				//DrawImage(lCharImageId, lCurrentX, lCurrentY, lCharSquare);
				lCurrentX += lCharSquare.GetWidth();
			}
		}
	}
	return (lCurrentY+GetLineHeight());*/
}

void Painter::SetActiveFont(FontID pFontId)
{
	if (mFontPainter)
	{
		// TODO: implement well when the system font rendering is tidied up.
	}
	FontTable::Iterator lIter = mFontTable.Find(pFontId);
	if (lIter != mFontTable.End())
	{
		mCurrentFont = *lIter;
	}
}

Painter::FontID Painter::GetCurrentFont()
{
	if (mFontPainter)
	{
		// TODO: implement well when the system font rendering is tidied up.
		return (mFontPainter->GetCurrentFont());
	}
	return mCurrentFont->mFontID;
}

int Painter::GetCharWidth(const Lepra::tchar pChar)
{
	if (mFontPainter)
	{
		return (mFontPainter->GetCharWidth(pChar));
	}

	unsigned int lChar = (unsigned int)((Lepra::utchar)pChar) - mCurrentFont->mFirstChar;

	if (pChar == _T(' '))
	{
		return mCurrentFont->mCharWidth[lChar] + mCurrentFont->mDefaultSpaceWidth + mCurrentFont->mCharOffset;
	}
	else if(pChar == _T('\t'))
	{
		return mCurrentFont->mTabWidth;
	}

	return mCurrentFont->mCharWidth[lChar] + mCurrentFont->mCharOffset;
}

int Painter::GetStringWidth(const Lepra::String& pString)
{
	if (mFontPainter)
	{
		return (mFontPainter->GetStringWidth(pString));
	}

	int lCurrentX = 0;
	int lMaxX = 0;

	for (size_t i = 0; i < pString.length(); i++)
	{
		Lepra::utchar lChar = (Lepra::utchar)pString[i];
		assert(lChar >= mCurrentFont->mFirstChar && lChar <= mCurrentFont->mLastChar);

		if (lChar == _T('\n'))
		{
			lCurrentX = 0;
		}
		else if(lChar != _T('\r') && 
			lChar != _T('\b'))
		{
			lCurrentX += mCurrentFont->mCharWidth[lChar - mCurrentFont->mFirstChar] + mCurrentFont->mCharOffset;
		}

		if (lChar == _T(' '))
		{
			lCurrentX += mCurrentFont->mDefaultSpaceWidth;
		}
		else if(lChar == _T('\t'))
		{
			lCurrentX = ((lCurrentX / mCurrentFont->mTabWidth) + 1) * mCurrentFont->mTabWidth;
		}

		if (lCurrentX > lMaxX)
		{
			lMaxX = lCurrentX;
		}
	}

	return lMaxX;
}

int Painter::GetFontHeight()
{
	if (mFontPainter)
	{
		return (mFontPainter->GetFontHeight());
	}
	
	return mCurrentFont->mCharHeight;
}

int Painter::GetLineHeight()
{
	if (mFontPainter)
	{
		return (mFontPainter->GetLineHeight());
	}

	return mCurrentFont->mCharHeight + mCurrentFont->mNewLineOffset;
}

Painter::FontID Painter::AddImageFont(const Lepra::Canvas& pFont,
				      int pTileWidth,
				      int pTileHeight,
				      int pCharWidth,
				      int pCharHeight,
				      int pFirstChar,
				      int pLastChar,
				      int pDefaultSpaceWidth,
				      int pNewLineOffset,
				      int pCharOffset,
				      int pTabWidth)
{
	int lNumChars = pLastChar - pFirstChar + 1;
	int* lCharWidth = new int[lNumChars];
	for (int i = 0; i < lNumChars; i++)
	{
		lCharWidth[i] = pCharWidth;
	}

	FontID lFontID = AddImageFont(pFont, 
				       pTileWidth, pTileHeight, 
				       lCharWidth, pCharHeight,
				       pFirstChar, pLastChar,
				       pDefaultSpaceWidth,
				       pNewLineOffset,
				       pCharOffset,
				       pTabWidth);

	delete[] lCharWidth;

	return lFontID;
}

Painter::FontID Painter::AddImageFont(const Lepra::Canvas& pFont, 
				      int pTileWidth,
				      int pTileHeight,
				      int* pCharWidth, 
				      int pCharHeight,
				      int pFirstChar,
				      int pLastChar,
				      int pDefaultSpaceWidth,
				      int pNewLineOffset,
				      int pCharOffset,
				      int pTabWidth)
{
	const int lNumChars = pLastChar - pFirstChar + 1;
	pDefaultSpaceWidth = (pDefaultSpaceWidth < 0)? CalcAverageWidth(lNumChars, pCharWidth) : pDefaultSpaceWidth;

	int lID = mFontIDManager.GetFreeId();

	if (lID != 0)
	{
		Font* lFont = NewFont(pFirstChar, pLastChar);

		lFont->mFontID = (Painter::FontID)lID;

		if (Lepra::Canvas::BitDepthToInt(pFont.GetBitDepth()) > 8 ||
		   (mCanvas != 0 && Lepra::Canvas::BitDepthToInt(mCanvas->GetBitDepth()) == 8))
		{
			lFont->mTextureID = AddImage(&pFont, 0);
			lFont->mAlphaImage = false;
		}
		else
		{
			lFont->mTextureID = AddImage(0, &pFont);
			lFont->mAlphaImage = true;
		}

		if (lFont->mTextureID == 0)
		{
			delete lFont;
			return Painter::INVALID_FONTID;
		}

		// Set font data.
		lFont->mTileWidth  = pTileWidth;
		lFont->mTileHeight = pTileHeight;
		lFont->mCharHeight = pCharHeight;

		lFont->mDefaultSpaceWidth = pDefaultSpaceWidth;
		lFont->mNewLineOffset = pNewLineOffset;
		lFont->mCharOffset = pCharOffset;
		lFont->mTabWidth = pTabWidth;

		::memcpy(lFont->mCharWidth, pCharWidth, (pLastChar - pFirstChar + 1) * sizeof(int));

		// Let the child class do whatever it has to do.
		InitFont(lFont, pFont);

		mFontTable.Insert(lID, lFont);
	}

	return (FontID)lID;
}

Painter::FontID Painter::GetStandardFont(int pFontIndex)
{
	switch(pFontIndex)
	{
	case 0:
		return mStandardFontID0;
	case 1:
		return mStandardFontID1;
	default:
		return mStandardFontID0;
	}
}

bool Painter::Font::IsSystemFont()
{
	return false;
}

Painter::DisplayListID Painter::NewDisplayList()
{
	int lID = mDisplayListIDManager.GetFreeId();
	if(lID != mDisplayListIDManager.GetInvalidId())
	{
		mDisplayListMap.insert(std::pair<unsigned, std::vector<DisplayEntity*>*>(lID, new std::vector<DisplayEntity*>));
	}
	return (DisplayListID)lID;
}

void Painter::DeleteDisplayList(DisplayListID pDisplayListID)
{
	DisplayListMap::iterator it = mDisplayListMap.find(pDisplayListID);
	if(it != mDisplayListMap.end())
	{
		std::vector<DisplayEntity*>* lDisplayList = (*it).second;
		mDisplayListMap.erase(it);

		std::vector<DisplayEntity*>::iterator lListIter;
		for(lListIter = lDisplayList->begin(); lListIter != lDisplayList->end(); ++lListIter)
		{
			delete *lListIter;
		}
		delete lDisplayList;
	}
}

void Painter::BeginDisplayList(DisplayListID pDisplayListID)
{
	DisplayListMap::iterator it = mDisplayListMap.find(pDisplayListID);
	if (it != mDisplayListMap.end())
	{
		mCurrentDisplayList = (*it).second;
		mDisplayListIter = mCurrentDisplayList->begin();
	}
}

void Painter::EndDisplayList()
{
	if (mCurrentDisplayList)
	{
		std::vector<DisplayEntity*>::iterator lListIter;
		for (lListIter = mCurrentDisplayList->begin(); lListIter != mCurrentDisplayList->end(); ++lListIter)
		{
			delete (*lListIter);
		}
		mCurrentDisplayList->clear();
		mDisplayListIter = mCurrentDisplayList->begin();
		mCurrentDisplayList = 0;
	}
}

void Painter::RenderDisplayList(DisplayListID pDisplayListID)
{
	DisplayListMap::iterator it = mDisplayListMap.find(pDisplayListID);
	if(it != mDisplayListMap.end())
	{
		//std::vector<DisplayEntity*>* lDisplayList = (*it).second;
		//DoRenderDisplayList(lDisplayList);
	}
}

Geometry2D* Painter::FetchDisplayEntity(Lepra::uint16 pVertexFormat, ImageID pImageID, FontID pFontID)
{
	AdjustVertexFormat(pVertexFormat);

	DisplayEntity* lEntity = 0;
	if(mDisplayListIter != mCurrentDisplayList->end())
	{
		lEntity = *mDisplayListIter;
	}

	// Find the image ID connected to the font.
	if(pFontID != INVALID_FONTID)
	{
		FontTable::Iterator lIter(mFontTable.Find((int)pFontID));
		if(lIter != mFontTable.End())
		{
			Font* lFont = *lIter;
			pImageID = (ImageID)lFont->mTextureID;
		}
	}


	Lepra::PixelRect lClippingRect;
	GetClippingRect(lClippingRect);
	if(lEntity == 0 ||
	   lEntity->mRM != mRenderMode ||
	   lEntity->mGeometry.GetVertexFormat() != pVertexFormat ||
	   lEntity->mAlpha != GetAlphaValue() ||
	   lEntity->mImageID != pImageID ||
	   lEntity->mClippingRect.mTop != lClippingRect.mTop ||
	   lEntity->mClippingRect.mBottom != lClippingRect.mBottom ||
	   lEntity->mClippingRect.mLeft != lClippingRect.mLeft ||
	   lEntity->mClippingRect.mRight != lClippingRect.mRight)
	{
		if(mDisplayListIter != mCurrentDisplayList->end())
			++mDisplayListIter;
		if(mDisplayListIter != mCurrentDisplayList->end())
		{
			lEntity = *mDisplayListIter;
			lEntity->Init(mRenderMode, GetAlphaValue(), pImageID, lClippingRect, pVertexFormat);
		}
		else
		{
			lEntity = new DisplayEntity(mRenderMode, GetAlphaValue(), pImageID, lClippingRect, pVertexFormat);
			mCurrentDisplayList->push_back(lEntity);
			mDisplayListIter = mCurrentDisplayList->end();
			--mDisplayListIter;
		}
	}
	return &lEntity->mGeometry;
}

void Painter::CreateText(const Lepra::String& pString, int x, int y)
{
	ToScreenCoords(x, y);
	int lCurrentX = x;
	int lCurrentY = y;

	Geometry2D* lGeometry = FetchDisplayEntity(Geometry2D::VTX_UV, INVALID_IMAGEID, GetCurrentFont());

	for (size_t i = 0; i < pString.length(); i++)
	{
		Lepra::tchar lChar = pString[i];
		assert(lChar >= mCurrentFont->mFirstChar && lChar <= mCurrentFont->mLastChar);

		if (lChar == _T('\n'))
		{
			lCurrentY += (mCurrentFont->mCharHeight + mCurrentFont->mNewLineOffset);
			lCurrentX = x;
		}
		else if(lChar != _T('\r') && 
			lChar != _T('\b') &&
			lChar != _T('\t'))
		{
			float lCharWidth = (float)mCurrentFont->mTileWidth;
			float lLeft   = (float)lCurrentX - 0.5f;
			float lRight  = (float)(lCurrentX + lCharWidth) - 0.5f;
			float lTop    = (float)lCurrentY - 0.5f;
			float lBottom = (float)(lCurrentY + mCurrentFont->mCharHeight) - 0.5f;

			float lU1;
			float lV1;
			float lU2;
			float lV2;
			mCurrentFont->GetUVRect(lChar, lU1, lV1, lU2, lV2);
			Lepra::uint32 lVtx1 = lGeometry->SetVertex(lLeft, lTop, lU1, lV1);
			Lepra::uint32 lVtx2 = lGeometry->SetVertex(lRight, lTop, lU2, lV1);
			Lepra::uint32 lVtx3 = lGeometry->SetVertex(lRight, lBottom, lU2, lV2);
			Lepra::uint32 lVtx4 = lGeometry->SetVertex(lLeft, lBottom, lU1, lV2);
			lGeometry->SetTriangle(lVtx1, lVtx2, lVtx3);
			lGeometry->SetTriangle(lVtx1, lVtx3, lVtx4);

			lCurrentX += mCurrentFont->mCharWidth[lChar - mCurrentFont->mFirstChar] + mCurrentFont->mCharOffset;
		}

		if (lChar == _T(' '))
		{
			lCurrentX += mCurrentFont->mDefaultSpaceWidth;
		}
		else if(lChar == _T('\t'))
		{
			lCurrentX = mTabOriginX + (((lCurrentX - mTabOriginX) / mCurrentFont->mTabWidth) + 1) * mCurrentFont->mTabWidth;
		}
	}
}

void Painter::CreateLine(int pX1, int pY1, int pX2, int pY2)
{
	Geometry2D* lGeometry = FetchDisplayEntity(Geometry2D::VTX_RGB);

	float lX1 = (float)pX1 - 0.5f;
	float lY1 = (float)pY1 - 0.5f;
	float lX2 = (float)pX2 - 0.5f;
	float lY2 = (float)pY2 - 0.5f;

	float r = (float)mColor[0].mRed / 255.0f;
	float g = (float)mColor[0].mGreen / 255.0f;
	float b = (float)mColor[0].mBlue / 255.0f;

	Lepra::Vector2DF lNormal(lY2 - lY1, lX1 - lX2);
	lNormal.Normalize();

	Lepra::uint32 lV0 = lGeometry->SetVertex(lX1 - lNormal.x, lY1 - lNormal.y, r, g, b);
	Lepra::uint32 lV1 = lGeometry->SetVertex(lX1 + lNormal.x, lY1 + lNormal.y, r, g, b);
	Lepra::uint32 lV2 = lGeometry->SetVertex(lX2 + lNormal.x, lY2 + lNormal.y, r, g, b);
	Lepra::uint32 lV3 = lGeometry->SetVertex(lX2 - lNormal.x, lY2 - lNormal.y, r, g, b);

	lGeometry->SetTriangle(lV0, lV1, lV2);
	lGeometry->SetTriangle(lV0, lV2, lV3);
}

void Painter::CreateRectFrame(int pLeft, int pTop, int pRight, int pBottom, int pWidth)
{
	Geometry2D* lGeometry = FetchDisplayEntity(Geometry2D::VTX_RGB);

	float lLeft   = (float)pLeft - 0.5f;
	float lRight  = (float)pRight - 0.5f;
	float lTop    = (float)pTop - 0.5f;
	float lBottom = (float)pBottom - 0.5f;

	float r = (float)mColor[0].mRed / 255.0f;
	float g = (float)mColor[0].mGreen / 255.0f;
	float b = (float)mColor[0].mBlue / 255.0f;

	Lepra::uint32 lV0 = lGeometry->SetVertex(lLeft, lTop, r, g, b);     // Outer top left.
	Lepra::uint32 lV1 = lGeometry->SetVertex(lRight, lTop, r, g, b);    // Outer top right.
	Lepra::uint32 lV2 = lGeometry->SetVertex(lRight, lBottom, r, g, b); // Outer bottom right.
	Lepra::uint32 lV3 = lGeometry->SetVertex(lLeft, lBottom, r, g, b);  // Outer bottom left.

	lLeft += pWidth;
	lTop += pWidth;
	lRight -= pWidth;
	lBottom -= pWidth;

	Lepra::uint32 lV4 = lGeometry->SetVertex(lLeft, lTop, r, g, b);     // Inner top left.
	Lepra::uint32 lV5 = lGeometry->SetVertex(lRight, lTop, r, g, b);    // Inner top right.
	Lepra::uint32 lV6 = lGeometry->SetVertex(lRight, lBottom, r, g, b); // Inner bottom right.
	Lepra::uint32 lV7 = lGeometry->SetVertex(lLeft, lBottom, r, g, b);  // Inner bottom left.

	lGeometry->SetTriangle(lV0, lV4, lV7);
	lGeometry->SetTriangle(lV0, lV7, lV3);
	lGeometry->SetTriangle(lV0, lV1, lV5);
	lGeometry->SetTriangle(lV0, lV5, lV4);
	lGeometry->SetTriangle(lV1, lV2, lV6);
	lGeometry->SetTriangle(lV1, lV6, lV5);
	lGeometry->SetTriangle(lV7, lV6, lV2);
	lGeometry->SetTriangle(lV7, lV2, lV3);
}

void Painter::Create3DRectFrame(int pLeft, int pTop, int pRight, int pBottom, int pWidth, bool pSunken)
{
	Geometry2D* lGeometry = FetchDisplayEntity(Geometry2D::VTX_RGB);

	float lLeft   = (float)pLeft - 0.5f;
	float lRight  = (float)pRight - 0.5f;
	float lTop    = (float)pTop - 0.5f;
	float lBottom = (float)pBottom - 0.5f;

	float r[4];
	float g[4];
	float b[4];
	int lI[4] = {0, 1, 2, 3};
	
	if(pSunken)
	{
		lI[0] = 1;
		lI[1] = 0;
		lI[2] = 3;
		lI[3] = 2;
	}
	
	for(int i = 0; i < 4; i++)
	{
		r[i] = (float)mColor[lI[i]].mRed / 255.0f;
		g[i] = (float)mColor[lI[i]].mGreen / 255.0f;
		b[i] = (float)mColor[lI[i]].mBlue / 255.0f;
	}

	Lepra::uint32 lV0 = lGeometry->SetVertex(lLeft, lTop, r[0], g[0], b[0]);     // Outer top left.
	Lepra::uint32 lV1 = lGeometry->SetVertex(lRight, lTop, r[0], g[0], b[0]);    // Outer top right #1.
	Lepra::uint32 lV2 = lGeometry->SetVertex(lRight, lTop, r[1], g[1], b[1]);    // Outer top right #2.
	Lepra::uint32 lV3 = lGeometry->SetVertex(lRight, lBottom, r[1], g[1], b[1]); // Outer bottom right.
	Lepra::uint32 lV4 = lGeometry->SetVertex(lLeft, lBottom, r[0], g[0], b[0]);  // Outer bottom left #1.
	Lepra::uint32 lV5 = lGeometry->SetVertex(lLeft, lBottom, r[1], g[1], b[1]);  // Outer bottom left #2.

	lLeft += pWidth;
	lTop += pWidth;
	lRight -= pWidth;
	lBottom -= pWidth;

	Lepra::uint32 lV6 = lGeometry->SetVertex(lLeft, lTop, r[2], g[2], b[2]);     // Inner top left.
	Lepra::uint32 lV7 = lGeometry->SetVertex(lRight, lTop, r[2], g[2], b[2]);    // Inner top right #1.
	Lepra::uint32 lV8 = lGeometry->SetVertex(lRight, lTop, r[3], g[3], b[3]);    // Inner top right #2.
	Lepra::uint32 lV9 = lGeometry->SetVertex(lRight, lBottom, r[3], g[3], b[3]); // Inner bottom right.
	Lepra::uint32 lV10 = lGeometry->SetVertex(lLeft, lBottom, r[2], g[2], b[2]);  // Inner bottom left #1.
	Lepra::uint32 lV11 = lGeometry->SetVertex(lLeft, lBottom, r[3], g[3], b[3]);  // Inner bottom left #2.

	//     0--------------------------1,2
	//     |                           |
	//     |   6------------------7,8  |
	//     |   |                   |   |
	//     |   |                   |   |
	//     |   |                   |   |
	//     |   |                   |   |
	//     |   |                   |   |
	//     |   |                   |   |
	//     |   |                   |   |
	//     | 10,11-----------------9   |
	//     |                           |
	//    4,5--------------------------3

	lGeometry->SetTriangle(lV0, lV1, lV7);  // Top
	lGeometry->SetTriangle(lV0, lV7, lV6);  // ...
	lGeometry->SetTriangle(lV0, lV6, lV10); // Left
	lGeometry->SetTriangle(lV0, lV10, lV4); // ...
	lGeometry->SetTriangle(lV8, lV2, lV3);  // Right
	lGeometry->SetTriangle(lV8, lV3, lV9);  // ...
	lGeometry->SetTriangle(lV11, lV9, lV3); // Bottom
	lGeometry->SetTriangle(lV11, lV3, lV5); // ...
}

void Painter::CreateRect(int pLeft, int pTop, int pRight, int pBottom)
{
	Geometry2D* lGeometry = FetchDisplayEntity(Geometry2D::VTX_RGB);

	float lLeft   = (float)pLeft - 0.5f;
	float lRight  = (float)pRight - 0.5f;
	float lTop    = (float)pTop - 0.5f;
	float lBottom = (float)pBottom - 0.5f;

	float r = (float)mColor[0].mRed / 255.0f;
	float g = (float)mColor[0].mGreen / 255.0f;
	float b = (float)mColor[0].mBlue / 255.0f;

	Lepra::uint32 lV0 = lGeometry->SetVertex(lLeft, lTop, r, g, b);
	Lepra::uint32 lV1 = lGeometry->SetVertex(lRight, lTop, r, g, b);
	Lepra::uint32 lV2 = lGeometry->SetVertex(lRight, lBottom, r, g, b);
	Lepra::uint32 lV3 = lGeometry->SetVertex(lLeft, lBottom, r, g, b);

	lGeometry->SetTriangle(lV0, lV1, lV2);
	lGeometry->SetTriangle(lV0, lV2, lV3);
}

void Painter::CreateShadedRect(int pLeft, int pTop, int pRight, int pBottom)
{
	Geometry2D* lGeometry = FetchDisplayEntity(Geometry2D::VTX_RGB);

	float lLeft   = (float)pLeft - 0.5f;
	float lRight  = (float)pRight - 0.5f;
	float lTop    = (float)pTop - 0.5f;
	float lBottom = (float)pBottom - 0.5f;

	float r[4];
	float g[4];
	float b[4];
	for(int i = 0; i < 4; i++)
	{
		r[i] = (float)mColor[i].mRed / 255.0f;
		g[i] = (float)mColor[i].mGreen / 255.0f;
		b[i] = (float)mColor[i].mBlue / 255.0f;
	}

	Lepra::uint32 lV0 = lGeometry->SetVertex(lLeft, lTop, r[0], g[0], b[0]);
	Lepra::uint32 lV1 = lGeometry->SetVertex(lRight, lTop, r[1], g[1], b[1]);
	Lepra::uint32 lV2 = lGeometry->SetVertex(lRight, lBottom, r[2], g[2], b[2]);
	Lepra::uint32 lV3 = lGeometry->SetVertex(lLeft, lBottom, r[3], g[3], b[3]);
	Lepra::uint32 lV4 = lGeometry->SetVertex((lLeft + lRight) * 0.5f, (lTop + lBottom) * 0.5f, 
		(r[0] + r[1] + r[2] + r[3]) * 0.25f, 
		(g[0] + g[1] + g[2] + g[3]) * 0.25f,
		(b[0] + b[1] + b[2] + b[3]) * 0.25f);

	lGeometry->SetTriangle(lV0, lV1, lV4);
	lGeometry->SetTriangle(lV1, lV2, lV4);
	lGeometry->SetTriangle(lV2, lV3, lV4);
	lGeometry->SetTriangle(lV3, lV0, lV4);
}

void Painter::CreateTriangle(float pX1, float pY1, float pX2, float pY2, float pX3, float pY3)
{
	Geometry2D* lGeometry = FetchDisplayEntity(Geometry2D::VTX_RGB);

	float r = (float)mColor[0].mRed / 255.0f;
	float g = (float)mColor[0].mGreen / 255.0f;
	float b = (float)mColor[0].mBlue / 255.0f;
	
	Lepra::uint32 lV0 = lGeometry->SetVertex(pX1, pY1, r, g, b);
	Lepra::uint32 lV1 = lGeometry->SetVertex(pX2, pY2, r, g, b);
	Lepra::uint32 lV2 = lGeometry->SetVertex(pX3, pY3, r, g, b);
	lGeometry->SetTriangle(lV0, lV1, lV2);
}

void Painter::CreateShadedTriangle(float pX1, float pY1, float pX2, float pY2, float pX3, float pY3)
{
	Geometry2D* lGeometry = FetchDisplayEntity(Geometry2D::VTX_RGB);

	float r[3];
	float g[3];
	float b[3];
	for(int i = 0; i < 3; i++)
	{
		r[i] = (float)mColor[i].mRed / 255.0f;
		g[i] = (float)mColor[i].mGreen / 255.0f;
		b[i] = (float)mColor[i].mBlue / 255.0f;
	}

	Lepra::uint32 lV0 = lGeometry->SetVertex(pX1, pY1, r[0], g[0], b[0]);
	Lepra::uint32 lV1 = lGeometry->SetVertex(pX2, pY2, r[1], g[1], b[1]);
	Lepra::uint32 lV2 = lGeometry->SetVertex(pX3, pY3, r[2], g[2], b[2]);
	lGeometry->SetTriangle(lV0, lV1, lV2);
}

void Painter::CreateTriangle(float pX1, float pY1, float pU1, float pV1,
                             float pX2, float pY2, float pU2, float pV2,
                             float pX3, float pY3, float pU3, float pV3,
                             ImageID pImageID)
{
	Geometry2D* lGeometry = FetchDisplayEntity(Geometry2D::VTX_UV, pImageID);

	Lepra::uint32 lV0 = lGeometry->SetVertex(pX1, pY1, pU1, pV1);
	Lepra::uint32 lV1 = lGeometry->SetVertex(pX2, pY2, pU2, pV2);
	Lepra::uint32 lV2 = lGeometry->SetVertex(pX3, pY3, pU3, pV3);
	lGeometry->SetTriangle(lV0, lV1, lV2);
}

void Painter::CreateImage(ImageID pImageID, int x, int y)
{
	int w;
	int h;
	GetImageSize(pImageID, w, h);

	Lepra::PixelRect lRect(x, y, x + w, y + h);
	Lepra::PixelRect lSubpatchRect(0, 0, w, h);
	CreateImage(pImageID, lRect, lSubpatchRect);
}

void Painter::CreateImage(ImageID pImageID, int x, int y, const Lepra::PixelRect& pSubpatchRect)
{
	Lepra::PixelRect lRect(x, y, x + pSubpatchRect.GetWidth(), y + pSubpatchRect.GetHeight());
	CreateImage(pImageID, lRect, pSubpatchRect);
}

void Painter::CreateImage(ImageID pImageID, const Lepra::PixelRect& pRect)
{
	Lepra::PixelRect lSubpatchRect(0, 0, 0, 0);
	GetImageSize(pImageID, lSubpatchRect.mRight, lSubpatchRect.mBottom);
	CreateImage(pImageID, pRect, lSubpatchRect);
}

void Painter::CreateImage(ImageID pImageID, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubpatchRect)
{
	Geometry2D* lGeometry = FetchDisplayEntity(Geometry2D::VTX_UV, pImageID);

	float lLeft   = (float)pRect.mLeft - 0.5f;
	float lRight  = (float)pRect.mRight - 0.5f;
	float lTop    = (float)pRect.mTop - 0.5f;
	float lBottom = (float)pRect.mBottom - 0.5f;

	int lWidth;
	int lHeight;
	GetImageSize(pImageID, lWidth, lHeight);

	float lU1 = (float)pSubpatchRect.mLeft / (float)lWidth;
	float lV1 = (float)pSubpatchRect.mTop / (float)lHeight;
	float lU2 = (float)pSubpatchRect.mRight / (float)lWidth;
	float lV2 = (float)pSubpatchRect.mBottom / (float)lHeight;

	Lepra::uint32 lVtx0 = lGeometry->SetVertex(lLeft, lTop, lU1, lV1);
	Lepra::uint32 lVtx1 = lGeometry->SetVertex(lRight, lTop, lU2, lV1);
	Lepra::uint32 lVtx2 = lGeometry->SetVertex(lRight, lBottom, lU2, lV2);
	Lepra::uint32 lVtx3 = lGeometry->SetVertex(lLeft, lBottom, lU1, lV2);

	lGeometry->SetTriangle(lVtx0, lVtx1, lVtx2);
	lGeometry->SetTriangle(lVtx0, lVtx2, lVtx3);	
}

unsigned Painter::GetClosestPowerOf2(unsigned pNumber, bool pGreater)
{
	if (pNumber == 0)
	{
		return 0;
	}

	unsigned lExp = GetExponent(pNumber);
	unsigned lPow = 1 << lExp;
	
	if(pGreater && lPow < pNumber)
		return (lPow << 1);
	else
		return lPow;
}

unsigned Painter::GetExponent(unsigned pPowerOf2)
{
	if (pPowerOf2 == 0)
	{
		// Error.
		return (unsigned)-1;
	}

	unsigned lExp = 0;
	
	while ((pPowerOf2 >> lExp) > 1)
	{
		lExp++;
	}

	return lExp;
}

Lepra::uint8 Painter::FindMatchingColor(const Lepra::Color& pColor)
{
	long lTargetR = pColor.mRed;
	long lTargetG = pColor.mGreen;
	long lTargetB = pColor.mBlue;

	long lMinError = 0x7FFFFFFF;
	Lepra::uint8 lBestMatch = 0;

	const Lepra::Color* lPalette = GetCanvas()->GetPalette();

	for (int i = 0; i < 256; i++)
	{
		long lDR = lTargetR - lPalette[i].mRed;
		long lDG = lTargetG - lPalette[i].mGreen;
		long lDB = lTargetB - lPalette[i].mBlue;

		long lError = lDR * lDR + lDG * lDG + lDB * lDB;

		if (i == 0 || lError < lMinError)
		{
			lMinError = lError;
			lBestMatch = (Lepra::uint8)i;
		}
	}

	return lBestMatch;
}

void Painter::GetScreenCoordClippingRect(Lepra::PixelRect& pClippingRect) const
{
	pClippingRect = mClippingRect;
	ToScreenCoords(pClippingRect.mLeft, pClippingRect.mTop);
	ToScreenCoords(pClippingRect.mRight, pClippingRect.mBottom);
}

void Painter::AddFont(Font* pFont)
{
	pFont->mFontID = (FontID)mFontIDManager.GetFreeId();
	if(pFont->mFontID != INVALID_FONTID)
	{
		mFontTable.Insert((int)pFont->mFontID, pFont);
	}
}

void Painter::AdjustVertexFormat(Lepra::uint16&)
{
	// Default behaviour. Do nothing.
}


int Painter::GetOrigoX()
{
	return mOrigoX;
}

int Painter::GetOrigoY()
{
	return mOrigoY;
}

Painter::XDir Painter::GetXDir()
{
	return mXDir;
}

Painter::YDir Painter::GetYDir()
{
	return mYDir;
}

Lepra::Canvas* Painter::GetCanvas()
{
	return mCanvas;
}

Painter::RenderMode Painter::GetRenderMode()
{
	return mRenderMode;
}

Lepra::uint8 Painter::GetAlphaValue()
{
	return mAlphaValue;
}

void Painter::SetClippingRect(const Lepra::PixelRect& pClippingRect)
{
	SetClippingRect(pClippingRect.mLeft, pClippingRect.mTop, pClippingRect.mRight, pClippingRect.mBottom);
}

void Painter::ReduceClippingRect(const Lepra::PixelRect& pClippingRect)
{
	ReduceClippingRect(pClippingRect.mLeft, pClippingRect.mTop, pClippingRect.mRight, pClippingRect.mBottom);
}

void Painter::GetClippingRect(Lepra::PixelRect& pClippingRect)
{
	pClippingRect = mClippingRect;
}

void Painter::SetColor(Lepra::uint8 pRed, Lepra::uint8 pGreen, Lepra::uint8 pBlue, Lepra::uint8 pPaletteIndex, unsigned pColorIndex)
{
	SetColor(Lepra::Color(pRed, pGreen, pBlue, pPaletteIndex), pColorIndex);
}

Lepra::Color Painter::GetColor(unsigned pColorIndex)
{
	return mColor[pColorIndex];
}

void Painter::DrawPixel(int x, int y)
{
	if(mCurrentDisplayList == 0)
		DoDrawPixel(x, y);
	else
		CreateRect(x, y, x, y);
}

void Painter::DrawPixel(const Lepra::PixelCoords& pCoords)
{
	DrawPixel(pCoords.x, pCoords.y);
}

void Painter::DrawLine(int pX1, int pY1, int pX2, int pY2)
{
	if(mCurrentDisplayList == 0)
		DoDrawLine(pX1, pY1, pX2, pY2);
	else
		CreateLine(pX1, pY1, pX2, pY2);
}

void Painter::DrawLine(const Lepra::PixelCoords& pPoint1, const Lepra::PixelCoords& pPoint2)
{
	DrawLine(pPoint1.x, pPoint1.y, pPoint2.x, pPoint2.y);
}

void Painter::DrawRect(int pLeft, int pTop, int pRight, int pBottom, int pWidth)
{
	if(mCurrentDisplayList == 0)
		DoDrawRect(pLeft, pTop, pRight, pBottom, pWidth);
	else
		CreateRectFrame(pLeft, pTop, pRight, pBottom, pWidth);
}

void Painter::DrawRect(const Lepra::PixelCoords& pTopLeft, const Lepra::PixelCoords& pBottomRight, int pWidth)
{
	DrawRect(pTopLeft.x, pTopLeft.y, pBottomRight.x, pBottomRight.y, pWidth);
}

void Painter::DrawRect(const Lepra::PixelRect& pRect, int pWidth)
{
	DrawRect(pRect.mLeft, pRect.mTop, pRect.mRight, pRect.mBottom, pWidth);
}

void Painter::FillRect(int pLeft, int pTop, int pRight, int pBottom)
{
	if(mCurrentDisplayList == 0)
		DoFillRect(pLeft, pTop, pRight, pBottom);
	else
		CreateRect(pLeft, pTop, pRight, pBottom);
}

void Painter::FillRect(const Lepra::PixelCoords& pTopLeft, const Lepra::PixelCoords& pBottomRight)
{
	FillRect(pTopLeft.x, pTopLeft.y, pBottomRight.x, pBottomRight.y);
}

void Painter::FillRect(const Lepra::PixelRect& pRect)
{
	FillRect(pRect.mLeft, pRect.mTop, pRect.mRight, pRect.mBottom);
}

void Painter::Draw3DRect(int pLeft, int pTop, int pRight, int pBottom, int pWidth, bool pSunken)
{
	//if(mCurrentDisplayList == 0)
		DoDraw3DRect(pLeft, pTop, pRight, pBottom, pWidth, pSunken);
	//else
	//	Create3DRectFrame(pLeft, pTop, pRight, pBottom, pWidth, pSunken);
}

void Painter::Draw3DRect(const Lepra::PixelCoords& pTopLeft, const Lepra::PixelCoords& pBottomRight, int pWidth, bool pSunken)
{
	Draw3DRect(pTopLeft.x, pTopLeft.y, pBottomRight.x, pBottomRight.y, pWidth, pSunken);
}

void Painter::Draw3DRect(const Lepra::PixelRect& pRect, int pWidth, bool pSunken)
{
	Draw3DRect(pRect.mLeft, pRect.mTop, pRect.mRight, pRect.mBottom, pWidth, pSunken);
}

void Painter::FillShadedRect(int pLeft, int pTop, int pRight, int pBottom)
{
	if(mCurrentDisplayList == 0)
		DoFillShadedRect(pLeft, pTop, pRight, pBottom);
	else
		CreateShadedRect(pLeft, pTop, pRight, pBottom);
}

void Painter::FillShadedRect(const Lepra::PixelCoords& pTopLeft, const Lepra::PixelCoords& pBottomRight)
{
	FillShadedRect(pTopLeft.x, pTopLeft.y, pBottomRight.x, pBottomRight.y);
}

void Painter::FillShadedRect(const Lepra::PixelRect& pRect)
{
	FillShadedRect(pRect.mLeft, pRect.mTop, pRect.mRight, pRect.mBottom);
}

void Painter::FillTriangle(float pX1, float pY1,
			   float pX2, float pY2,
			   float pX3, float pY3)
{
	if(mCurrentDisplayList == 0)
		DoFillTriangle(pX1, pY1, pX2, pY2, pX3, pY3);
	else
		CreateTriangle(pX1, pY1, pX2, pY2, pX3, pY3);
}

void Painter::FillTriangle(const Lepra::PixelCoords& pPoint1,
			   const Lepra::PixelCoords& pPoint2,
			   const Lepra::PixelCoords& pPoint3)
{
	FillTriangle((float)pPoint1.x, (float)pPoint1.y,
	             (float)pPoint2.x, (float)pPoint2.y,
	             (float)pPoint3.x, (float)pPoint3.y);
}

void Painter::FillShadedTriangle(float pX1, float pY1,
				 float pX2, float pY2,
				 float pX3, float pY3)
{
	if(mCurrentDisplayList == 0)
		DoFillShadedTriangle(pX1, pY1, pX2, pY2, pX3, pY3);
	else
		CreateShadedTriangle(pX1, pY1, pX2, pY2, pX3, pY3);
}

void Painter::FillShadedTriangle(const Lepra::PixelCoords& pPoint1,
			         const Lepra::PixelCoords& pPoint2,
			         const Lepra::PixelCoords& pPoint3)
{
	FillShadedTriangle((float)pPoint1.x, (float)pPoint1.y, 
	                   (float)pPoint2.x, (float)pPoint2.y, 
	                   (float)pPoint3.x, (float)pPoint3.y);
}

void Painter::FillTriangle(float pX1, float pY1, float pU1, float pV1,
			   float pX2, float pY2, float pU2, float pV2,
			   float pX3, float pY3, float pU3, float pV3,
			   ImageID pImageID)
{
	if(mCurrentDisplayList == 0)
		DoFillTriangle(pX1, pY1, pU1, pV1, pX2, pY2, pU2, pV2, pX3, pY3, pU3, pV3, pImageID);
	else
		CreateTriangle(pX1, pY1, pU1, pV1, pX2, pY2, pU2, pV2, pX3, pY3, pU3, pV3, pImageID);
}

void Painter::FillTriangle(const Lepra::PixelCoords& pPoint1, float pU1, float pV1,
			   const Lepra::PixelCoords& pPoint2, float pU2, float pV2,
			   const Lepra::PixelCoords& pPoint3, float pU3, float pV3,
			   ImageID pImageID)
{
	FillTriangle((float)pPoint1.x, (float)pPoint1.y, pU1, pV1, 
	             (float)pPoint2.x, (float)pPoint2.y, pU2, pV2, 
	             (float)pPoint3.x, (float)pPoint3.y, pU3, pV3, pImageID);
}

void Painter::DrawImage(ImageID pImageID, int x, int y)
{
	if(mCurrentDisplayList == 0)
		DoDrawImage(pImageID, x, y);
	else
		CreateImage(pImageID, x, y);
}

void Painter::DrawImage(ImageID pImageID, const Lepra::PixelCoords& pTopLeft)
{
	DrawImage(pImageID, pTopLeft.x, pTopLeft.y);
}

void Painter::DrawImage(ImageID pImageID, int x, int y, const Lepra::PixelRect& pSubpatchRect)
{
	if(mCurrentDisplayList == 0)
		DoDrawImage(pImageID, x, y, pSubpatchRect);
	else
		CreateImage(pImageID, x, y, pSubpatchRect);
}

void Painter::DrawImage(ImageID pImageID, const Lepra::PixelCoords& pTopLeft, const Lepra::PixelRect& pSubpatchRect)
{
	DrawImage(pImageID, pTopLeft.x, pTopLeft.y, pSubpatchRect);
}

void Painter::DrawImage(ImageID pImageID, const Lepra::PixelRect& pRect)
{
	if(mCurrentDisplayList == 0)
		DoDrawImage(pImageID, pRect);
	else
		CreateImage(pImageID, pRect);
}

void Painter::DrawImage(ImageID pImageID, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubpatchRect)
{
	if(mCurrentDisplayList == 0)
		DoDrawImage(pImageID, pRect, pSubpatchRect);
	else
		CreateImage(pImageID, pRect, pSubpatchRect);
}

void Painter::DrawAlphaImage(ImageID pImageID, int x, int y)
{
	if(mCurrentDisplayList == 0)
		DoDrawAlphaImage(pImageID, x, y);
	else
		CreateImage(pImageID, x, y);
}

void Painter::DrawAlphaImage(ImageID pImageID, const Lepra::PixelCoords& pTopLeft)
{
	DrawAlphaImage(pImageID, pTopLeft.x, pTopLeft.y);
}

void Painter::DrawDefaultMouseCursor(int x, int y)
{
	DrawImage(mMouseCursorID, x, y);
}

void Painter::SetTabOriginX(int pTabOriginX)
{
	mTabOriginX = pTabOriginX;
}

Painter::RenderMode Painter::DisplayEntity::GetRenderMode() const
{
	return mRM;
}

Lepra::uint8 Painter::DisplayEntity::GetAlpha() const
{
	return mAlpha;
}

Painter::ImageID Painter::DisplayEntity::GetImageID() const
{
	return mImageID;
}

const Lepra::PixelRect& Painter::DisplayEntity::GetClippingRect() const
{
	return mClippingRect;
}

Geometry2D& Painter::DisplayEntity::GetGeometry()
{
	return mGeometry;
}

bool Painter::IsPowerOf2(unsigned pNumber)
{
	return (pNumber == GetClosestPowerOf2(pNumber));
}

Lepra::uint8* Painter::GetStandardFont1()
{
	return smStandardFont1;
}

Lepra::uint8* Painter::GetStandardFont2()
{
	return smStandardFont2;
}

int* Painter::GetCharWidthStdFont1()
{
	return smFont1CharWidth;
}

int* Painter::GetCharWidthStdFont2()
{
	return smFont2CharWidth;
}

Lepra::uint8* Painter::GetStandardMouseCursor()
{
	return smStandardMouseCursor;
}

void Painter::ToScreenCoords(int& x, int& y) const
{
	x = x * (int)mXDir + mOrigoX;
	y = y * (int)mYDir + mOrigoY;
}

void Painter::ToUserCoords(int& x, int& y) const
{
	x = (x - mOrigoX) * (int)mXDir;
	y = (y - mOrigoY) * (int)mYDir;
}

void Painter::ToScreenCoords(float& x, float& y) const
{
	x = x * (float)mXDir + (float)mOrigoX;
	y = y * (float)mYDir + (float)mOrigoY;
}

void Painter::ToUserCoords(float& x, float& y) const
{
	x = (x - (float)mOrigoX) * (float)mXDir;
	y = (y - (float)mOrigoY) * (float)mYDir;
}

bool Painter::XLT(int x1, int x2)
{
	return (x1 * (int)mXDir) <  (x2 * (int)mXDir);
}

bool Painter::XLE(int x1, int x2)
{
	return (x1 * (int)mXDir) <= (x2 * (int)mXDir);
}

bool Painter::XGT(int x1, int x2)
{
	return (x1 * (int)mXDir) >  (x2 * (int)mXDir);
}

bool Painter::XGE(int x1, int x2)
{
	return (x1 * (int)mXDir) >= (x2 * (int)mXDir);
}

bool Painter::YLT(int y1, int y2)
{
	return (y1 * (int)mYDir) <  (y2 * (int)mYDir);
}

bool Painter::YLE(int y1, int y2)
{
	return (y1 * (int)mYDir) <= (y2 * (int)mYDir);
}

bool Painter::YGT(int y1, int y2)
{
	return (y1 * (int)mYDir) >  (y2 * (int)mYDir);
}

bool Painter::YGE(int y1, int y2)
{
	return (y1 * (int)mYDir) >= (y2 * (int)mYDir);
}

Painter::Font* Painter::GetCurrentFontInternal() const
{
	return mCurrentFont;
}

int Painter::GetTabOriginX() const
{
	return mTabOriginX;
}

Lepra::Color& Painter::GetColorInternal(int pColorIndex)
{
	return mColor[pColorIndex];
}



}
