
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../../Include/Mac/UiMacFontManager.h"
#import <Cocoa/Cocoa.h>
#include "../../../UiLepra/Include/Mac/UiMacDisplayManager.h"



namespace UiTbc
{



FontManager* FontManager::Create(UiLepra::DisplayManager* pDisplayManager)
{
	return (new MacFontManager((UiLepra::MacDisplayManager*)pDisplayManager));
}



MacFontManager::MacFontManager(UiLepra::MacDisplayManager* pDisplayManager):
	mDisplayManager(pDisplayManager)
{
	/*HWND lWnd = mDisplayManager->GetHWND();
	mDC = ::GetDC(lWnd);

	COLORREF lColor = RGB(255, 255, 255);
	mColorRef[0] = lColor;
	lColor = RGB(0, 0, 0);
	mColorRef[1] = lColor;
	mColorRef[2] = lColor;
	mColorRef[3] = lColor;*/
}

MacFontManager::~MacFontManager()
{
	FontTable::iterator x = mFontTable.begin();
	if (x != mFontTable.end())
	{
		MacFont* lFont = (MacFont*)x->second;
		//::DeleteObject(lFont->mMacFontHandle);
		delete (lFont);
	}
	mFontTable.clear();

	//::ReleaseDC(mDisplayManager->GetHWND(), mDC);
	mDisplayManager = 0;
}



void MacFontManager::SetColor(const Color& pColor, unsigned pColorIndex)
{
	assert(pColorIndex <= 1);

	/*COLORREF lColor = RGB(pColor.mRed, pColor.mGreen, pColor.mBlue);
	mColorRef[pColorIndex] = lColor;*/
}

MacFontManager::FontId MacFontManager::AddFont(const str& pFontName, double pSize, int pFlags, CharacterSet pCharSet)
{
	/*int lWeight  = ((pFlags & BOLD) != 0) ? FW_BOLD : FW_NORMAL;
	DWORD lItalic = ((pFlags & ITALIC) != 0) ? TRUE : FALSE;
	DWORD lUnderline = ((pFlags & UNDERLINE) != 0) ? TRUE : FALSE;
	DWORD lStrikeOut = ((pFlags & STRIKEOUT) != 0) ? TRUE : FALSE;

	HFONT lFontHandle = ::CreateFont((int)(pSize + 0.5),
					  0,
					  0, 0,
					  lWeight,
					  lItalic,
					  lUnderline,
					  lStrikeOut,
					  (pCharSet == NATIVE) ? DEFAULT_CHARSET : ANSI_CHARSET,
					  OUT_DEFAULT_PRECIS,
					  CLIP_DEFAULT_PRECIS,
					  DEFAULT_QUALITY,
					  DEFAULT_PITCH | FF_DONTCARE,
					  pFontName.c_str());*/

	FontId lId = INVALID_FONTID;
	//if (lFontHandle != NULL)
	{
		MacFont* lFont = new MacFont();
		//lFont->mMacFontHandle = lFontHandle;
		lFont->mName = pFontName;
		lFont->mSize = pSize;
		lFont->mFlags = pFlags;
		if (!InternalAddFont(lFont))
		{
			delete (lFont);
			//::DeleteObject(lFontHandle);
		}
		else
		{
			lId = lFont->mFontId;
		}
	}

	return ((FontId)lId);
}

bool MacFontManager::RenderGlyph(tchar pChar, Canvas& pImage, const PixelRect& pRect)
{
	pImage.Reset(pRect.GetWidth(), pRect.GetHeight(), Canvas::BITDEPTH_32_BIT);
	pImage.CreateBuffer();
	::memset(pImage.GetBuffer(), 0, pImage.GetWidth()*pImage.GetPixelByteSize()*pImage.GetHeight());

	CGContextRef textcontext; // this is our rendering context
	CGColorSpaceRef colorspace = CGColorSpaceCreateDeviceRGB(); // we need some colorspace
	// we create a bitmap context
	textcontext = CGBitmapContextCreate(pImage.GetBuffer(), pImage.GetWidth(), pImage.GetHeight(), 8,
		pImage.GetWidth()*pImage.GetPixelByteSize(), colorspace, kCGImageAlphaPremultipliedLast);
	CGFloat rect[4] = { pRect.mLeft, pRect.mTop, pRect.mRight, pRect.mBottom };
	CGFloat transparent[4] = { 0, 0, 0, 0 };
	CGFloat text_color[4] = { 1, 1, 1, 1 };
	// if you do this a lot store the color somewhere and release it when you are done with it
	CGContextSetFillColorWithColor(textcontext, CGColorCreate(colorspace, transparent)); 
	CGContextFillRect(textcontext, *(CGRect*)rect);
	CGContextSetFillColorWithColor(textcontext, CGColorCreate(colorspace, text_color));
	CGContextSelectFont(textcontext, "Arial", 10.0f, kCGEncodingMacRoman);
	CGContextShowTextAtPoint(textcontext, 0, pRect.GetHeight()/4, &pChar, 1);
	CGContextFlush(textcontext);

	pImage.FlipVertical();	// Upside down. TODO: verify... We currently do this on both Win and Mac...

	/*printf("Rendering glyph '%c' (height=%i):\n", pChar, pImage.GetHeight());
	for (int y = 0; y < (int)pImage.GetHeight(); ++y)
	{
		for (int x = 0; x < (int)pImage.GetWidth(); ++x)
		{
			if (pImage.GetPixelColor(x, y).To32() == 0)
			{
				printf("         ");
			}
			else
			{
				printf("%8.8X ", pImage.GetPixelColor(x, y).To32());
			}
		}
		printf("\n");
	}*/

	return (true);

	/*bool lOk = (mCurrentFont != 0);
	if (lOk)
	{
		lOk = (pRect.mTop >= 0 && pRect.mLeft >= 0 &&
			pRect.GetWidth() >= 1 && pRect.GetHeight() >= 1 &&
			pRect.mRight <= (int)pImage.GetWidth() && pRect.mBottom <= (int)pImage.GetHeight());
	}
	HDC lRamDc = 0;
	if (lOk)
	{
		lRamDc = ::CreateCompatibleDC(mDC);
		lOk = (lRamDc != 0);
	}
	HBITMAP lRamBitmap = 0;
	if (lOk)
	{
		lRamBitmap = ::CreateCompatibleBitmap(mDC, pRect.GetWidth(), pRect.GetHeight());
		lOk = (lRamBitmap != 0);
	}
	HGDIOBJ lDefaultBitmap = 0;
	BITMAPINFO lBitmapInfo;
	::memset(&lBitmapInfo, 0, sizeof(lBitmapInfo));
	if (lOk)
	{
		lDefaultBitmap = ::SelectObject(lRamDc, lRamBitmap);
		lBitmapInfo.bmiHeader.biSize = sizeof(lBitmapInfo.bmiHeader);
		::GetDIBits(lRamDc, lRamBitmap, 0, 0, 0, &lBitmapInfo, DIB_RGB_COLORS);
		lOk = (lBitmapInfo.bmiHeader.biWidth == pRect.GetWidth() &&
			lBitmapInfo.bmiHeader.biHeight == pRect.GetHeight());
	}
	if (lOk)
	{
		HGDIOBJ lDefaultObject = ::SelectObject(lRamDc, ((MacFont*)mCurrentFont)->mMacFontHandle);
		::SetTextColor(lRamDc, mColorRef[0]);
		::SetBkColor(lRamDc, mColorRef[1]);
		::SetTextAlign(mDC, TA_BASELINE | TA_LEFT);
		lOk = (::TextOut(lRamDc, 0, 0, &pChar, 1) != FALSE);
		::SelectObject(lRamDc, lDefaultObject);
	}
	uint8* lBitmap = 0;
	int lBpp = -1;
	if (lOk)
	{
		lBitmapInfo.bmiHeader.biHeight = -pRect.GetHeight();
		lBitmapInfo.bmiHeader.biCompression = 0;
		lBpp = lBitmapInfo.bmiHeader.biBitCount;
		int lBytePerPixel = (lBpp+7)/8;
		int lByteCount = pRect.GetWidth()*pRect.GetHeight()*lBytePerPixel;
		lBitmap = new uint8[lByteCount];
		lOk = (::GetDIBits(lRamDc, lRamBitmap, 0, pRect.GetHeight(), lBitmap, &lBitmapInfo, DIB_RGB_COLORS) == pRect.GetHeight());
		::SelectObject(lRamDc, lDefaultBitmap);
	}
	if (lOk)
	{
		Canvas::BitDepth lSourceBitDepth = Canvas::IntToBitDepth(lBpp);
		Canvas::BitDepth lTargetBitDepth = pImage.GetBitDepth();
		pImage.Reset(pRect.GetWidth(), pRect.GetHeight(), lSourceBitDepth);
		pImage.SetBuffer(lBitmap, true);
		lBitmap = 0;	// Don't delete it, ownership now taken by calling canvas.
		pImage.ConvertBitDepth(lTargetBitDepth);
		pImage.FlipVertical();	// Bitmap is upside down...
		pImage.SwapRGBOrder();	// ... and BGR.
		for (int y = 0; y < pRect.GetHeight(); ++y)
		{
			for (int x = 0; x < pRect.GetWidth(); ++x)
			{
				Color lColor;
				pImage.GetPixelColor(x, y, lColor);
				int lIntensity = lColor.SumRgb();
				//// Sum controls biggest part of light intensity, average controls a minor part.
				lIntensity = (lIntensity*70 + lIntensity/3*30) / 100;
				lIntensity = (lIntensity > 255)? 255 : lIntensity;
				lColor.mAlpha = (uint8)(lIntensity);
				pImage.SetPixelColor(x, y, lColor);
			}
		}
	}
	delete (lBitmap);
	lBitmap = 0;
	if (lRamBitmap)
	{
		::DeleteObject(lRamBitmap);
		lRamBitmap = 0;
	}
	if (lRamDc)
	{
		::DeleteDC(lRamDc);
		lRamDc = 0;
	}
	return (lOk);*/
	return (true);
}



int MacFontManager::GetCharWidth(const tchar pChar) const
{
	//MacFont* lFont = (MacFont*)mCurrentFont;
	// TODO: convert to i18n though utf8-lib.
	tchar lTempString[2] = {0, 0};
	lTempString[0] = pChar;
	NSString* someString = [NSString stringWithUTF8String:lTempString];
	NSFont* lFont = [NSFont fontWithName:@"Arial" size:10.0f];
	NSSize lSize = [lFont advancementForGlyph:[lFont glyphWithName:someString]];
	return lSize.width+1;
	/*CGFloat lStartX = CGContextGetTextPosition();
	Set the text drawing mode to kCGTextInvisible using the function CGContextSetTextDrawingMode.
	Draw the text by calling the function CGContextShowText to draw the text at the current text position.
	Determine the final text position by calling the function CGContextGetTextPosition.
	Subtract the starting position from the ending position to determine th*/
}



}
