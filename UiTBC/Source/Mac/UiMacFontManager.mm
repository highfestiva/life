
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../../Include/Mac/UiMacFontManager.h"
#ifdef LEPRA_IOS
#import <UIKit/UIKit.h>
#define TBC_APPLE_FONT UIFont
#else // !iOS
#import <Cocoa/Cocoa.h>
#define TBC_APPLE_FONT NSFont
#endif // iOS/!iOS
#include "../../../UiLepra/Include/Mac/UiMacDisplayManager.h"
#include "../../../Lepra/Include/Posix/MacLog.h"



#define FONT_SIZE_FACTOR 0.8f



namespace UiTbc
{



FontManager* FontManager::Create(UiLepra::DisplayManager* pDisplayManager)
{
	return (new MacFontManager((UiLepra::MacDisplayManager*)pDisplayManager));
}



MacFontManager::MacFontManager(UiLepra::MacDisplayManager* pDisplayManager):
	mDisplayManager(pDisplayManager)
{
}

MacFontManager::~MacFontManager()
{
	FontTable::iterator x = mFontTable.begin();
	if (x != mFontTable.end())
	{
		MacFont* lFont = (MacFont*)x->second;
		delete (lFont);
	}
	mFontTable.clear();

	mDisplayManager = 0;
}



void MacFontManager::SetColor(const Color& pColor, unsigned pColorIndex)
{
	deb_assert(pColorIndex <= 1);
}

MacFontManager::FontId MacFontManager::AddFont(const str& pFontName, double pSize, int pFlags, CharacterSet pCharSet)
{
	FontId lId = INVALID_FONTID;
	{
		MacFont* lFont = new MacFont();
		lFont->mName = pFontName;
		lFont->mSize = pSize * FONT_SIZE_FACTOR;
		lFont->mActualSize = pSize;
		lFont->mFlags = pFlags;
		if (!InternalAddFont(lFont))
		{
			delete (lFont);
		}
		else
		{
			lId = lFont->mFontId;
		}
	}

	return lId;
}

bool MacFontManager::RenderGlyph(tchar pChar, Canvas& pImage, const PixelRect& pRect)
{
	pImage.Reset(pRect.GetWidth(), pRect.GetHeight(), Canvas::BITDEPTH_32_BIT);
	pImage.CreateBuffer();
	::memset(pImage.GetBuffer(), 0, pImage.GetWidth()*pImage.GetPixelByteSize()*pImage.GetHeight());

	const float lCorrectedFontSize = mCurrentFont->mSize;	// Similar to other platforms...
	CGContextRef textcontext; // This is our rendering context.
	CGColorSpaceRef colorspace = CGColorSpaceCreateDeviceRGB(); // We need some colorspace.
	// Create a bitmap context.
	textcontext = CGBitmapContextCreate(pImage.GetBuffer(), pImage.GetWidth(), pImage.GetHeight(), 8,
		pImage.GetWidth()*pImage.GetPixelByteSize(), colorspace, kCGImageAlphaPremultipliedLast);
	CGFloat rect[4] = { pRect.mLeft, pRect.mTop, pRect.mRight, pRect.mBottom };
	CGFloat transparent[4] = { 1, 1, 1, 0 };
	CGFloat text_color[4] = { 1, 1, 1, 1 };
	// if you do this a lot store the color somewhere and release it when you are done with it.
	CGColorRef backgroundColor = CGColorCreate(colorspace, transparent);
	CGContextSetFillColorWithColor(textcontext, backgroundColor); 
	CGContextFillRect(textcontext, *(CGRect*)rect);
	CGColorRef foregroundColor = CGColorCreate(colorspace, text_color);
	CGContextSetFillColorWithColor(textcontext, foregroundColor);
	CGContextSelectFont(textcontext, astrutil::Encode(mCurrentFont->mName).c_str(), lCorrectedFontSize, kCGEncodingMacRoman);
	CGContextSetLineWidth(textcontext, 0.3);
	CGContextSetTextDrawingMode(textcontext, kCGTextFillStroke);
	CGContextSetRGBStrokeColor(textcontext, 1,1,1,0.5);
#ifdef LEPRA_IOS
	int y = (pRect.GetHeight() - ((MacFont*)mCurrentFont)->mActualSize) / 2;
	UIGraphicsPushContext(textcontext);
	tchar lTmpString[2] = {pChar, 0};
	NSString* lChar = MacLog::Encode(lTmpString);
	NSString* lFontName = MacLog::Encode(mCurrentFont->mName);
	[lChar drawAtPoint:CGPointMake(0, y) withFont:[TBC_APPLE_FONT fontWithName:lFontName size:lCorrectedFontSize]];
	CGContextFlush(textcontext);
	UIGraphicsPopContext();
#else // !iOS
	int y = pRect.GetHeight() - mCurrentFont->mSize;
	CGContextShowTextAtPoint(textcontext, 0, y, &pChar, 1);
	CGContextFlush(textcontext);
	pImage.FlipVertical();
#endif // iOS / Mac
	CFRelease(backgroundColor);
	CFRelease(foregroundColor);
	CFRelease(colorspace);
	CFRelease(textcontext);

	// Strengthen the colors!
	for (int y = 0; y < (int)pImage.GetHeight(); ++y)
	{
		for (int x = 0; x < (int)pImage.GetWidth(); ++x)
		{
			Color lColor = pImage.GetPixelColor(x, y);
			if (lColor.mAlpha)
			{
				lColor.mRed	= 255;
				lColor.mGreen	= 255;
				lColor.mBlue	= 255;
				pImage.SetPixelColor(x, y, lColor);
			}
		}
	}

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
}



int MacFontManager::GetCharWidth(const tchar pChar) const
{
	NSString* lFontName = MacLog::Encode(mCurrentFont->mName);
	const float lCorrectedFontSize = mCurrentFont->mSize;	// Similar to other platforms...
	TBC_APPLE_FONT* lFont = [TBC_APPLE_FONT fontWithName:lFontName size:lCorrectedFontSize];
#ifdef LEPRA_IOS
	wchar_t lTempBigString[2] = {pChar, 0};
	NSString* tmpString = MacLog::Encode(wstr(lTempBigString));
	CGSize lSize = [tmpString sizeWithFont:lFont];
	--lSize.width;
#else // !iOS
	UniChar lTempBigString[2] = {pChar, 0};
	CGGlyph lGlyph;
	CTFontGetGlyphsForCharacters((CTFontRef)lFont, lTempBigString, &lGlyph, 1);
	NSSize lSize = [lFont advancementForGlyph:lGlyph];
#endif // iOS/!iOS
	return lSize.width+1;
}



}
