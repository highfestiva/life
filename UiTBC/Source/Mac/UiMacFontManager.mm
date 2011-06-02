
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
	assert(pColorIndex <= 1);
}

MacFontManager::FontId MacFontManager::AddFont(const str& pFontName, double pSize, int pFlags, CharacterSet pCharSet)
{
	FontId lId = INVALID_FONTID;
	{
		MacFont* lFont = new MacFont();
		lFont->mName = pFontName;
		lFont->mSize = pSize;
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
	CGFloat transparent[4] = { 1, 1, 1, 0 };
	CGFloat text_color[4] = { 1, 1, 1, 1 };
	// if you do this a lot store the color somewhere and release it when you are done with it
	CGContextSetFillColorWithColor(textcontext, CGColorCreate(colorspace, transparent)); 
	CGContextFillRect(textcontext, *(CGRect*)rect);
	CGContextSetFillColorWithColor(textcontext, CGColorCreate(colorspace, text_color));
	CGContextSelectFont(textcontext, mCurrentFont->mName.c_str(), mCurrentFont->mSize, kCGEncodingMacRoman);
	CGContextSetLineWidth(textcontext, 0.3);
	//CGContextSetCharacterSpacing(textcontext, 1);
	CGContextSetTextDrawingMode(textcontext, kCGTextFillStroke);
	CGContextSetRGBStrokeColor(textcontext, 1,1,1,0.5);
	CGContextShowTextAtPoint(textcontext, 0, pRect.GetHeight()/4, &pChar, 1);
	CGContextFlush(textcontext);

	pImage.FlipVertical();	// Upside down.

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
	// TODO: convert to i18n though utf8-lib.
	tchar lTempString[2] = {0, 0};
	lTempString[0] = pChar;
	NSString* lFontName = [NSString stringWithUTF8String:mCurrentFont->mName.c_str()];
	NSFont* lFont = [NSFont fontWithName:lFontName size:mCurrentFont->mSize];
	NSGlyph* lGlyph = (NSGlyph*)malloc(sizeof(NSGlyph)); 
	const wstr lString = wstrutil::Encode(str(lTempString));
	CTFontGetGlyphsForCharacters((CTFontRef)lFont, (const UniChar*)lString.c_str(), (CGGlyph*)lGlyph, 1);
	NSSize lSize = [lFont advancementForGlyph:*lGlyph];
	free(lGlyph);
	return lSize.width+1;
}



}
