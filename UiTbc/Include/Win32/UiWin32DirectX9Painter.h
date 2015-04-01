/*
	Class:  DirectX9Painter
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

*/



#pragma once

#include <d3d9.h>
#include <list>
#include "../UiTbc.h"
#include "../../../Lepra/Include/Graphics2D.h"
#include "../../../Lepra/Include/Canvas.h"
#include "../../../Lepra/Include/HashTable.h"
#include "../../../Lepra/Include/String.h"
#include "../../../Lepra/Include/IdManager.h"
#include "../UiPainter.h"



namespace UiLepra
{
class DisplayManager;
}



namespace UiTbc
{



class DirectX9Painter: public Painter
{
public:
	DirectX9Painter(UiLepra::DisplayManager* pDisplayManager);
	virtual ~DirectX9Painter();

	void SetDestCanvas(Canvas* pCanvas);
	void SetRenderMode(RenderMode pRM);
	virtual void Clear(const Color& pColor);
	virtual void PrePaint(bool pClearDepthBuffer);

	void SetAlphaValue(uint8 pAlpha);

	void ResetClippingRect();
	void SetClippingRect(int pLeft, int pTop, int pRight, int pBottom);

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
	void DoDrawRect(int pLeft, int pTop, int pRight, int pBottom);
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
	void DrawFan(const std::vector<vec2>& pCoords, bool pFill);
	void DoDrawImage(ImageID pImageID, int x, int y);
	void DoDrawImage(ImageID pImageID, int x, int y, const PixelRect& pSubpatchRect);
	void DoDrawImage(ImageID pImageID, const PixelRect& pRect);
	void DoDrawImage(ImageID pImageID, const PixelRect& pRect, const PixelRect& pSubpatchRect);
	void DoDrawAlphaImage(ImageID pImageID, int x, int y);

	void DoRenderDisplayList(std::vector<DisplayEntity*>* pDisplayList);
private:
	class Texture
	{
	public:
		Texture() :
			mWidth(0),
			mHeight(0),
			mD3DTexture(0)
		{
		}

		int mWidth;
		int mHeight;
		IDirect3DTexture9* mD3DTexture;
	};

	typedef HashTable<int, Texture*> TextureTable;

	void GetImageSize(ImageID pImageID, int& pWidth, int& pHeight) const;
	void PrintText(const str& pString, int x, int y);
	void SetFontSmoothness(bool pSmooth);

	void AdjustVertexFormat(uint16& pVertexFormat);

	IdManager<int> mTextureIDManager;
	TextureTable mTextureTable;

	IDirect3DDevice9* mD3DDevice;
	IDirect3DTexture9* mD3DDefaultMouseCursor;

	logclass();
};



}
