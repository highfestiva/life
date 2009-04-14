/*
	Class:  OpenGLPainter
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand

	NOTES: 
	
	This class can only render bitmaps with dimensions of a power of 2.
	The openGL texture "names" (or IDs) between 1-10000 are reserved to bitmap rendering.
	1 is reserved for text, and 2 is reserved for the default mouse cursor.
*/

#ifndef UIOPENGLPAINTER_H
#define UIOPENGLPAINTER_H

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

class OpenGLPainter : public Painter
{
public:
	OpenGLPainter();
	virtual ~OpenGLPainter();

	// Set the drawing surface.
	void SetDestCanvas(Lepra::Canvas* pCanvas);
	void SetRenderMode(RenderMode pRM);

	// Set the current alpha value. Overrides alpha buffer...
	// In 8-bit color mode, this is a specific color that will "be" transparent.
	void SetAlphaValue(Lepra::uint8 pAlpha);

	void SetClippingRect(int pLeft, int pTop, int pRight, int pBottom);
	void ResetClippingRect();

	void SetColor(const Lepra::Color& pColor, unsigned pColorIndex = 0);

	ImageID AddImage(const Lepra::Canvas* pImage, const Lepra::Canvas* pAlphaBuffer);
	void UpdateImage(ImageID pImageID, 
			 const Lepra::Canvas* pImage, 
			 const Lepra::Canvas* pAlphaBuffer, 
			 UpdateHint pHint = UPDATE_ACCURATE);
	void RemoveImage(ImageID pImageID);

	void ReadPixels(Lepra::Canvas& pDestCanvas, const Lepra::PixelRect& pRect);

	RGBOrder GetRGBOrder();

protected:
	/*
		Rendering functions.
	*/

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
	void DoDrawImage(ImageID pImageID, int x, int y);
	void DoDrawImage(ImageID pImageID, int x, int y, const Lepra::PixelRect& pSubpatchRect);
	void DoDrawImage(ImageID pImageID, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubpatchRect);
	void DoDrawImage(ImageID pImageID, const Lepra::PixelRect& pRect);
	void DoDrawAlphaImage(ImageID pImageID, int x, int y);

	Font* NewFont(int pFirstChar, int pLastChar) const { return new OpenGLFont(pFirstChar, pLastChar); }
	void InitFont(Font* pFont, const Lepra::Canvas& pFontImage);
	void GetImageSize(ImageID pImageID, int& pWidth, int& pHeight);
	int DoPrintText(const Lepra::String& pString, int x, int y);

	void DoRenderDisplayList(std::vector<DisplayEntity*>* pDisplayList);
private:

	Lepra::Vector3DF mRCol[4];

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

	class OpenGLFont : public Painter::Font
	{
	public:
		OpenGLFont(int pFirstChar, int pLastChar) :
			Font(pFirstChar, pLastChar),
			mCharRect(new FRect[pLastChar - pFirstChar + 1]),
			mTexture(0)
		{
		}

		~OpenGLFont()
		{
			delete[] mCharRect;
		}

		void GetUVRect(const Lepra::tchar& pChar, float& pU1, float& pV1, float& pU2, float& pV2) const
		{
			FRect& lFRect = mCharRect[pChar - mFirstChar];
			pU1 = lFRect.mLeft;
			pV1 = lFRect.mTop;
			pU2 = lFRect.mRight;
			pV2 = lFRect.mBottom;
		}

		struct FRect
		{
			float mLeft;
			float mRight;
			float mTop;
			float mBottom;
		};

		FRect* mCharRect;
		Texture* mTexture;
	};

	typedef Lepra::HashTable<int, Texture*> TextureTable;

	void UpdateRenderMode();

	Lepra::IdManager<int> mTextureIDManager;
	TextureTable mTextureTable;

	bool mRenderModeChanged;
};


} // End namespace.

#endif