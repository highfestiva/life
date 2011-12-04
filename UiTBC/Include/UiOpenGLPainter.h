
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games
//
// NOTES: 
//
// This class can only render bitmaps with dimensions of a power of 2.
// The openGL texture "names" (or IDs) between 1-10000 are reserved to bitmap rendering.
// 1 is reserved for text, and 2 is reserved for the default mouse cursor.



#pragma once

#include "UiTBC.h"
#include "../../Lepra/Include/Graphics2D.h"
#include "../../Lepra/Include/Canvas.h"
#include "../../Lepra/Include/HashTable.h"
#include "../../Lepra/Include/IdManager.h"
#include "../../Lepra/Include/Vector3D.h"
#include "../../UiLepra/Include/UiOpenGLExtensions.h"	// Included to get the gl-headers.
#include "UiPainter.h"
#include <list>



namespace UiTbc
{



class FontTexture;



class OpenGLPainter: public Painter
{
public:
	OpenGLPainter();
	virtual ~OpenGLPainter();

	// Set the drawing surface.
	void SetDestCanvas(Canvas* pCanvas);
	void SetRenderMode(RenderMode pRM);
	virtual void Clear(const Color& pColor);
	virtual void PrePaint(bool pClearDepthBuffer);

	// Set the current alpha value. Overrides alpha buffer...
	// In 8-bit color mode, this is a specific color that will "be" transparent.
	void SetAlphaValue(Lepra::uint8 pAlpha);

	void SetClippingRect(int pLeft, int pTop, int pRight, int pBottom);
	void ResetClippingRect();

	void SetColor(const Color& pColor, unsigned pColorIndex = 0);

	ImageID AddImage(const Canvas* pImage, const Canvas* pAlphaBuffer);
	void UpdateImage(ImageID pImageID, 
			 const Canvas* pImage, 
			 const Canvas* pAlphaBuffer, 
			 UpdateHint pHint = UPDATE_ACCURATE);
	void RemoveImage(ImageID pImageID);

	void ReadPixels(Canvas& pDestCanvas, const PixelRect& pRect);

	RGBOrder GetRGBOrder() const;

protected:
	void DoSetRenderMode() const;

	void DoDrawPixel(int x, int y);
	void DoDrawLine(int pX1, int pY1, int pX2, int pY2);
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
	void DoDrawRect(int pLeft, int pTop, int pRight, int pBottom, int pWidth);
	void DoFillRect(int pLeft, int pTop, int pRight, int pBottom);
	void DoDraw3DRect(int pLeft, int pTop, int pRight, int pBottom, int pWidth, bool pSunken);
	void DoFillShadedRect(int pLeft, int pTop, int pRight, int pBottom);
	void DrawFan(const std::vector<Vector2DF> pCoords, bool pFill);
	void DrawImageFan(ImageID pImageID, const std::vector<Vector2DF> pCoords, const std::vector<Vector2DF> pTexCoords);
	void DoDrawImage(ImageID pImageID, int x, int y);
	void DoDrawImage(ImageID pImageID, int x, int y, const PixelRect& pSubpatchRect);
	void DoDrawImage(ImageID pImageID, const PixelRect& pRect, const PixelRect& pSubpatchRect);
	void DoDrawImage(ImageID pImageID, const PixelRect& pRect);
	void DoDrawAlphaImage(ImageID pImageID, int x, int y);

	void GetImageSize(ImageID pImageID, int& pWidth, int& pHeight) const;
	void PrintText(const str& pString, int x, int y);
	void SetFontSmoothness(bool pSmooth);

	virtual void AdjustVertexFormat(unsigned& pVertexFormat);
	void DoRenderDisplayList(std::vector<DisplayEntity*>* pDisplayList);

	virtual void ClearFontBuffers();
	FontTexture* SelectGlyphs(uint32 pFontHash, int pFontHeight, const str& pString);

private:
	Vector3DF mRCol[4];

	class Texture
	{
	public:
		Texture() :
			mWidth(0),
			mHeight(0)
		{
		}

		int mWidth;
		int mHeight;
	};

	typedef HashTable<int, Texture*> TextureTable;
	typedef std::hash_map<uint32, FontTexture*> FontTextureTable;

	IdManager<int> mTextureIDManager;
	TextureTable mTextureTable;
	FontTextureTable mFontTextureTable;

	bool mSmoothFont;
};



}
