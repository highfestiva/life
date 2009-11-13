// Author: Alexander Hugestrand
// Copyright (c) 2002-2006, Alexander Hugestrand
//
// NOTES: 
//
// 1. GDIPainter must be created AFTER the screen/window is opened/created.
// 2. Requires link with msimg32.lib, and msimg32.dll installed on the system.
// 3. Does not support additive, xor or alpha tested materials.
// 4. Only supports alpha blending when rendering bitmaps, not solid geometry.
// 5. Renders directly to the front buffer (to the screen), so watch out for
//    flickering!
// 6. Since GDI is part of Microsoft Windows, the mouse is rendered by the OS
//    and cannot be rendered explicitly by calling DrawDefaultMouseCursor().
// 7. Recommendation: If you want to do something more advanced, used SoftwarePainter
//    instead.
// 8. What use do you have for GDIPainter? You can render text using the system fonts!



#pragma once

#include "../../Lepra/Include/HashTable.h"
#include "../../Lepra/Include/IdManager.h"
#include "../../Lepra/Include/LepraTarget.h"
#include "UiSystemPainter.h"
#include "UiTBC.h"



namespace UiLepra
{
class Win32DisplayManager;
}

namespace UiTbc
{

class GDIPainter : public SystemPainter
{
public:
	GDIPainter(UiLepra::Win32DisplayManager* pDisplayManager);
	 ~GDIPainter();

	 void SetRenderMode(RenderMode pRM);

	 void ResetClippingRect();
	 void SetClippingRect(int pLeft, int pTop, int pRight, int pBottom);

	 void SetColor(const Lepra::Color& pColor, unsigned pColorIndex = 0);

	// Returns an ID to the added bitmap. Returns 0 if error. 
	// Alpha buffer or bitmap may be NULL.
	 ImageID AddImage(const Lepra::Canvas* pBitmap, const Lepra::Canvas* pAlphaBuffer);
	 void UpdateImage(ImageID pImageID, const Lepra::Canvas* pBitmap, const Lepra::Canvas* pAlphaBuffer, Painter::UpdateHint pHint);
	 void RemoveImage(ImageID pImageID);

	 Painter::FontID AddSystemFont(const Lepra::String& pFont, double pSize, Lepra::uint32 pFlags, CharacterSet pCharSet);

	 void SetActiveFont(FontID pFontID);

	 // TODO: Remove. GDIPainter should be able to render text to a Canvas through
	 // the functions SetDestCanvas() and PrintText().
	virtual bool RenderGlyph(Lepra::tchar pChar, Lepra::Canvas& pImage, const Lepra::PixelRect& pRect);

	// Returns the width of the rendered string in pixels.
	 int GetCharWidth(const Lepra::tchar pChar);
	 int GetStringWidth(const Lepra::String& pString);
	 int GetFontHeight();
	 int GetLineHeight();

	 void ReadPixels(Lepra::Canvas& pDestCanvas, const Lepra::PixelRect& pRect);

	 Painter::RGBOrder GetRGBOrder();

protected:
	void DoDrawPixel(int x, int y);
	void DoDrawLine(int pX1, int pY1, int pX2, int pY2);
	void DoDrawRect(int pLeft, int pTop, int pRight, int pBottom, int pWidth);
	void DoFillRect(int pLeft, int pTop, int pRight, int pBottom);
	void DoDraw3DRect(int pLeft, int pTop, int pRight, int pBottom, int pWidth, bool pSunken);
	void DoFillShadedRect(int pLeft, int pTop, int pRight, int pBottom);
	void DoFillTriangle(float pX1, float pY1,
			  float pX2, float pY2,
			  float pX3, float pY3);
	void DoFillShadedTriangle(float pX1, float pY1,
				float pX2, float pY2,
				float pX3, float pY3);
	void DoFillTriangle(float pX1, float pY1, float pU1, float pV1,
			  float pX2, float pY2, float pU2, float pV2,
			  float pX3, float pY3, float pU3, float pV3,
			  ImageID pImageID);
	void DoDrawImage(ImageID pImageID, int x, int y);
	void DoDrawImage(ImageID pImageID, int x, int y, const Lepra::PixelRect& pSubpatchRect);
	void DoDrawImage(ImageID pImageID, const Lepra::PixelRect& pRect);
	void DoDrawImage(ImageID pImageID, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubpatchRect);
	void DoDrawAlphaImage(ImageID pImageID, int x, int y);
	void DoDrawAlphaImage(ImageID pImageID, int x, int y, const Lepra::PixelRect& pSubpatchRect);
	Font* NewFont(int pFirstChar, int pLastChar) const;
	void InitFont(Font* pFont, const Lepra::Canvas& pFontImage);
	void GetImageSize(ImageID pImageID, int& pWidth, int& pHeight);

	void DoRenderDisplayList(std::vector<DisplayEntity*>* pDisplayList);
private:

	struct Win32BitmapInfo
	{
		BITMAPINFOHEADER bmiHeader;
		RGBQUAD			 bmiColors[256];
	};

	struct Win32LogPalette 
	{
		WORD         palVersion;
		WORD         palNumEntries;
		PALETTEENTRY palPalEntry[256];
	};

	class TextureInfo
	{
	public:

		TextureInfo() :
			mBitmap(NULL),
			mBitmap2(NULL),
			mWidth(0),
			mHeight(0)
		{
		}

		TextureInfo(HGDIOBJ pBitmap,
					HGDIOBJ pBitmapPreMultAlpha,
					int pWidth, 
					int pHeight, 
					int pBitsPerPixel, 
					Lepra::uint8* pBits) :
			mBitmap(pBitmap),
			mBitmap2(pBitmapPreMultAlpha),
			mWidth(pWidth),
			mHeight(pHeight),
			mBitsPerPixel(pBitsPerPixel),
			mBits(pBits)
		{
		}

		TextureInfo(const TextureInfo& pTextureInfo) :
			mBitmap(pTextureInfo.mBitmap),
			mBitmap2(pTextureInfo.mBitmap2),
			mWidth(pTextureInfo.mWidth),
			mHeight(pTextureInfo.mHeight),
			mBitsPerPixel(pTextureInfo.mBitsPerPixel),
			mBits(pTextureInfo.mBits)
		{
		}

		HGDIOBJ mBitmap;
		HGDIOBJ mBitmap2;	// With premultiplied alpha.
		int mWidth;
		int mHeight;
		int mBitsPerPixel;
		Lepra::uint8* mBits;
	};

	class ImageFont : public Painter::Font
	{
	public:
		ImageFont(int pFirstChar, int pLastChar) :
			Font(pFirstChar, pLastChar),
			mCharRect(0),
			mTexture(0)
		{
			int lCharCount = mLastChar - mFirstChar + 1;
			mCharRect = new Lepra::PixelRect[lCharCount];
		}

		~ImageFont()
		{
			delete[] mCharRect;
		}

		void GetUVRect(const Lepra::tchar& pChar, float& pU1, float& pV1, float& pU2, float& pV2) const
		{
			int lIndex = pChar - mFirstChar;
			pU1 = (float)mCharRect[lIndex].mLeft / (float)mTexture->mWidth;
			pV1 = (float)mCharRect[lIndex].mTop / (float)mTexture->mHeight;
			pU2 = (float)mCharRect[lIndex].mRight / (float)mTexture->mWidth;
			pV2 = (float)mCharRect[lIndex].mBottom / (float)mTexture->mHeight;
		}

		Lepra::PixelRect* mCharRect;
		TextureInfo* mTexture;
	};

	class SystemFont : public Painter::Font
	{
	public:
		SystemFont() :
			Font(0, 0),
			mFont(0),
			mSize(0)
		{
		}

		void GetUVRect(const Lepra::tchar&, float& pU1, float& pV1, float& pU2, float& pV2) const
		{
			pU1 = 0;
			pV1 = 0;
			pU2 = 0;
			pV2 = 0;
		}

		bool IsSystemFont() { return true; }

		HFONT mFont;
		double mSize;
	};

	typedef Lepra::HashTable<int, TextureInfo> BitmapTable;

	void _AddImage(ImageID pImageID, const Lepra::Canvas* pBitmap, const Lepra::Canvas* pAlphaBuffer);
	void _DrawLine(int pX1, int pY1, int pX2, int pY2);
	int DoPrintText(const Lepra::String& pString, int x, int y);

	UiLepra::Win32DisplayManager* mDisplayManager;

	Lepra::IdManager<int> mTextureIDManager;
	BitmapTable mBitmapTable;

	HDC mDC;
	HBRUSH mBrush[4];
	HPEN mPen[4];
	COLORREF mColorRef[4];

	HBRUSH mNullBrush;
	HPEN mNullPen;
};

}
