/*
	Class:  SoftwarePainter
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand
*/

#ifndef UISOFTWAREPAINTER_H
#define UISOFTWAREPAINTER_H

#include "../../Lepra/Include/HashTable.h"
#include "../../Lepra/Include/IdManager.h"
#include "UiGradients.h"
#include "UiEdge.h"
#include "UiPainter.h"
#include "UiTBC.h"
#include <list>

namespace UiTbc
{

class SoftwarePainter : public Painter
{
public:
	SoftwarePainter();
	virtual ~SoftwarePainter();

	// Set the drawing surface.
	void SetDestCanvas(Lepra::Canvas* pCanvas);
	void SetRenderMode(RenderMode pRM);

	void ResetClippingRect();
	void SetClippingRect(int pLeft, int pTop, int pRight, int pBottom);

	void SetColor(const Lepra::Color& pColor, unsigned pColorIndex = 0);

	ImageID AddImage(const Lepra::Canvas* pImage, const Lepra::Canvas* pAlphaBuffer);
	void UpdateImage(ImageID pImageID, 
			 const Lepra::Canvas* pImage, 
			 const Lepra::Canvas* pAlphaBuffer,
			 UpdateHint pHint = UPDATE_ACCURATE);
	void RemoveImage(ImageID pImageID);

	void ReadPixels(Lepra::Canvas& pDestCanvas, const Lepra::PixelRect& pRect);

	RGBOrder GetRGBOrder();

	// Software painter specific functions.
	
	// Makes the red and blue component swap in all calls to SetColor().
	inline void SwapRGB();

	// SetIncrementalAlpha() only has effect in 32 bit bit depths.
	// If set to false, the alpha channel on the destination canvas
	// won't be modified. Otherwise it will be modified if and only if
	// the written alpha is greater than the current one.
	inline void SetIncrementalAlpha(bool pIncrementalAlpha);

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

	// Draw3DRect uses Lepra::Color[0] and Lepra::Color[1]
	void DoDraw3DRect(int pLeft, int pTop, int pRight, int pBottom, int pWidth, bool pSunken);

	// FillShadedRect uses all colors...
	void DoFillShadedRect(int pLeft, int pTop, int pRight, int pBottom);

	void DoDrawImage(ImageID pImageID, int x, int y);
	void DoDrawImage(ImageID pImageID, int x, int y, const Lepra::PixelRect& pSubpatchRect);
	void DoDrawImage(ImageID pImageID, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubpatchRect);
	void DoDrawImage(ImageID pImageID, const Lepra::PixelRect& pRect);

	void DoDrawAlphaImage(ImageID pImageID, int x, int y);

	void DoRenderDisplayList(std::vector<DisplayEntity*>* pDisplayList);
private:

	// A color class used to optimize alpha blending when painting
	// single-colored things. (Precalculates half of the blending).
	class BlendColor
	{
	public:

		BlendColor(Lepra::Color& pColor, Lepra::uint8 pAlpha)
		{
			mA = (Lepra::uint16)pAlpha;
			mR = (((Lepra::uint16)pColor.mRed   * mA) >> 8);
			mG = (((Lepra::uint16)pColor.mGreen * mA) >> 8);
			mB = (((Lepra::uint16)pColor.mBlue  * mA) >> 8);
			mA = 255 - mA;
		}

		Lepra::uint16 mR;
		Lepra::uint16 mG;
		Lepra::uint16 mB;
		Lepra::uint16 mA;
	};

	// Function pointers. Used to minimize the amount of code.
	// Instead of duplicating each rendering function for each rendering
	// mode, we just set these function pointers to be used by a much
	// smaller amount of rendering functions.

	void (*DrawHLineFunc) (int pX1, int pX2, int pY, int pColorIndex,
						   SoftwarePainter* pThis);

	void (*DrawVLineFunc) (int pX1, int pX2, int pY, int pColorIndex,
						   SoftwarePainter* pThis);

	void (*DrawShadedHLineFunc) (int pX1, int pX2, int pY,
				     float r1, float g1, float b1, float a1,
				     float r2, float g2, float b2, float a2,
				     SoftwarePainter* pThis);

	void (*DrawPixelFunc)          (int x,  int y,  SoftwarePainter* pThis);
	void (*DrawLineFunc)           (int x1, int y1, int x2, int y2,  SoftwarePainter* pThis);
	void (*FillShadedRectFunc)     (int pLeft, int pTop, int pRight, int pBottom, SoftwarePainter* pThis);

	// Fallbacks are used if the required data doesn't exist.
	void (*DrawImageFunc)           (const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);
	void (*DrawImageFallback)       (const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);
	void (*DrawImageStretchFunc)    (const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubRect, SoftwarePainter* pThis);
	void (*DrawImageStretchFallback)(const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubRect, SoftwarePainter* pThis);
	void (*DrawAlphaImageFunc)      (const Lepra::Canvas& pImage, int x, int y, SoftwarePainter* pThis);

	void (*DrawScanLineFunc) (const Gradients2DUV*, Edge2DUV*, Edge2DUV*, const Lepra::Canvas*, const Lepra::Canvas*, SoftwarePainter*);
	void (*DrawScanLineFuncFallback) (const Gradients2DUV*, Edge2DUV*, Edge2DUV*, const Lepra::Canvas*, const Lepra::Canvas*, SoftwarePainter*);
	void (*DrawScanLinePow2Func) (const Gradients2DUV*, Edge2DUV*, Edge2DUV*, const Lepra::Canvas*, const Lepra::Canvas*, unsigned, SoftwarePainter*);
	void (*DrawScanLinePow2FuncFallback) (const Gradients2DUV*, Edge2DUV*, Edge2DUV*, const Lepra::Canvas*, const Lepra::Canvas*, unsigned, SoftwarePainter*);

	void UpdateFunctionPointers();

	static void DrawPixel8BitNormal (int x, int y, SoftwarePainter* pThis);
	static void DrawPixel16BitNormal(int x, int y, SoftwarePainter* pThis);
	static void DrawPixel24BitNormal(int x, int y, SoftwarePainter* pThis);
	static void DrawPixel32BitNormal(int x, int y, SoftwarePainter* pThis);
	static void DrawPixel15BitBlend (int x, int y, SoftwarePainter* pThis);
	static void DrawPixel16BitBlend (int x, int y, SoftwarePainter* pThis);
	static void DrawPixel24BitBlend (int x, int y, SoftwarePainter* pThis);
	static void DrawPixel32BitBlend (int x, int y, SoftwarePainter* pThis);
	static void DrawPixel8BitXor    (int x, int y, SoftwarePainter* pThis);
	static void DrawPixel16BitXor   (int x, int y, SoftwarePainter* pThis);
	static void DrawPixel24BitXor   (int x, int y, SoftwarePainter* pThis);
	static void DrawPixel32BitXor   (int x, int y, SoftwarePainter* pThis);
	static void DrawPixel8BitAdd    (int x, int y, SoftwarePainter* pThis);
	static void DrawPixel15BitAdd   (int x, int y, SoftwarePainter* pThis);
	static void DrawPixel16BitAdd   (int x, int y, SoftwarePainter* pThis);
	static void DrawPixel24BitAdd   (int x, int y, SoftwarePainter* pThis);
	static void DrawPixel32BitAdd   (int x, int y, SoftwarePainter* pThis);

	static void DrawHLine8BitNormal (int x1, int x2, int y, int pColorIndex, SoftwarePainter* pThis);
	static void DrawHLine16BitNormal(int x1, int x2, int y, int pColorIndex, SoftwarePainter* pThis);
	static void DrawHLine24BitNormal(int x1, int x2, int y, int pColorIndex, SoftwarePainter* pThis);
	static void DrawHLine32BitNormal(int x1, int x2, int y, int pColorIndex, SoftwarePainter* pThis);
	static void DrawHLine15BitBlend (int x1, int x2, int y, int pColorIndex, SoftwarePainter* pThis);
	static void DrawHLine16BitBlend (int x1, int x2, int y, int pColorIndex, SoftwarePainter* pThis);
	static void DrawHLine24BitBlend (int x1, int x2, int y, int pColorIndex, SoftwarePainter* pThis);
	static void DrawHLine32BitBlend (int x1, int x2, int y, int pColorIndex, SoftwarePainter* pThis);
	static void DrawHLine8BitXor    (int x1, int x2, int y, int pColorIndex, SoftwarePainter* pThis);
	static void DrawHLine16BitXor   (int x1, int x2, int y, int pColorIndex, SoftwarePainter* pThis);
	static void DrawHLine24BitXor   (int x1, int x2, int y, int pColorIndex, SoftwarePainter* pThis);
	static void DrawHLine32BitXor   (int x1, int x2, int y, int pColorIndex, SoftwarePainter* pThis);
	static void DrawHLine8BitAdd    (int x1, int x2, int y, int pColorIndex, SoftwarePainter* pThis);
	static void DrawHLine15BitAdd   (int x1, int x2, int y, int pColorIndex, SoftwarePainter* pThis);
	static void DrawHLine16BitAdd   (int x1, int x2, int y, int pColorIndex, SoftwarePainter* pThis);
	static void DrawHLine24BitAdd   (int x1, int x2, int y, int pColorIndex, SoftwarePainter* pThis);
	static void DrawHLine32BitAdd   (int x1, int x2, int y, int pColorIndex, SoftwarePainter* pThis);

	static void DrawShadedHLine8BitNormal (int x1, int x2, int y, float r1, float g1, float b1, float a1, float r2, float g2, float b2, float a2, SoftwarePainter* pThis);
	static void DrawShadedHLine15BitNormal(int x1, int x2, int y, float r1, float g1, float b1, float a1, float r2, float g2, float b2, float a2, SoftwarePainter* pThis);
	static void DrawShadedHLine16BitNormal(int x1, int x2, int y, float r1, float g1, float b1, float a1, float r2, float g2, float b2, float a2, SoftwarePainter* pThis);
	static void DrawShadedHLine24BitNormal(int x1, int x2, int y, float r1, float g1, float b1, float a1, float r2, float g2, float b2, float a2, SoftwarePainter* pThis);
	static void DrawShadedHLine32BitNormal(int x1, int x2, int y, float r1, float g1, float b1, float a1, float r2, float g2, float b2, float a2, SoftwarePainter* pThis);
	static void DrawShadedHLine15BitBlend (int x1, int x2, int y, float r1, float g1, float b1, float a1, float r2, float g2, float b2, float a2, SoftwarePainter* pThis);
	static void DrawShadedHLine16BitBlend (int x1, int x2, int y, float r1, float g1, float b1, float a1, float r2, float g2, float b2, float a2, SoftwarePainter* pThis);
	static void DrawShadedHLine24BitBlend (int x1, int x2, int y, float r1, float g1, float b1, float a1, float r2, float g2, float b2, float a2, SoftwarePainter* pThis);
	static void DrawShadedHLine32BitBlend (int x1, int x2, int y, float r1, float g1, float b1, float a1, float r2, float g2, float b2, float a2, SoftwarePainter* pThis);
	static void DrawShadedHLine15BitXor   (int x1, int x2, int y, float r1, float g1, float b1, float a1, float r2, float g2, float b2, float a2, SoftwarePainter* pThis);
	static void DrawShadedHLine16BitXor   (int x1, int x2, int y, float r1, float g1, float b1, float a1, float r2, float g2, float b2, float a2, SoftwarePainter* pThis);
	static void DrawShadedHLine24BitXor   (int x1, int x2, int y, float r1, float g1, float b1, float a1, float r2, float g2, float b2, float a2, SoftwarePainter* pThis);
	static void DrawShadedHLine32BitXor   (int x1, int x2, int y, float r1, float g1, float b1, float a1, float r2, float g2, float b2, float a2, SoftwarePainter* pThis);
	static void DrawShadedHLine15BitAdd   (int x1, int x2, int y, float r1, float g1, float b1, float a1, float r2, float g2, float b2, float a2, SoftwarePainter* pThis);
	static void DrawShadedHLine16BitAdd   (int x1, int x2, int y, float r1, float g1, float b1, float a1, float r2, float g2, float b2, float a2, SoftwarePainter* pThis);
	static void DrawShadedHLine24BitAdd   (int x1, int x2, int y, float r1, float g1, float b1, float a1, float r2, float g2, float b2, float a2, SoftwarePainter* pThis);
	static void DrawShadedHLine32BitAdd   (int x1, int x2, int y, float r1, float g1, float b1, float a1, float r2, float g2, float b2, float a2, SoftwarePainter* pThis);

	static void DrawVLine8BitNormal (int y1, int y2, int x, int pColorIndex, SoftwarePainter* pThis);
	static void DrawVLine16BitNormal(int y1, int y2, int x, int pColorIndex, SoftwarePainter* pThis);
	static void DrawVLine24BitNormal(int y1, int y2, int x, int pColorIndex, SoftwarePainter* pThis);
	static void DrawVLine32BitNormal(int y1, int y2, int x, int pColorIndex, SoftwarePainter* pThis);
	static void DrawVLine15BitBlend (int y1, int y2, int x, int pColorIndex, SoftwarePainter* pThis);
	static void DrawVLine16BitBlend (int y1, int y2, int x, int pColorIndex, SoftwarePainter* pThis);
	static void DrawVLine24BitBlend (int y1, int y2, int x, int pColorIndex, SoftwarePainter* pThis);
	static void DrawVLine32BitBlend (int y1, int y2, int x, int pColorIndex, SoftwarePainter* pThis);
	static void DrawVLine8BitXor    (int y1, int y2, int x, int pColorIndex, SoftwarePainter* pThis);
	static void DrawVLine16BitXor   (int y1, int y2, int x, int pColorIndex, SoftwarePainter* pThis);
	static void DrawVLine24BitXor   (int y1, int y2, int x, int pColorIndex, SoftwarePainter* pThis);
	static void DrawVLine32BitXor   (int y1, int y2, int x, int pColorIndex, SoftwarePainter* pThis);
	static void DrawVLine8BitAdd    (int y1, int y2, int x, int pColorIndex, SoftwarePainter* pThis);
	static void DrawVLine15BitAdd   (int y1, int y2, int x, int pColorIndex, SoftwarePainter* pThis);
	static void DrawVLine16BitAdd   (int y1, int y2, int x, int pColorIndex, SoftwarePainter* pThis);
	static void DrawVLine24BitAdd   (int y1, int y2, int x, int pColorIndex, SoftwarePainter* pThis);
	static void DrawVLine32BitAdd   (int y1, int y2, int x, int pColorIndex, SoftwarePainter* pThis);

	static void DrawLine8BitNormal (int x1, int y1, int x2, int y2, SoftwarePainter* pThis);
	static void DrawLine16BitNormal(int x1, int y1, int x2, int y2, SoftwarePainter* pThis);
	static void DrawLine24BitNormal(int x1, int y1, int x2, int y2, SoftwarePainter* pThis);
	static void DrawLine32BitNormal(int x1, int y1, int x2, int y2, SoftwarePainter* pThis);
	static void DrawLine15BitBlend (int x1, int y1, int x2, int y2, SoftwarePainter* pThis);
	static void DrawLine16BitBlend (int x1, int y1, int x2, int y2, SoftwarePainter* pThis);
	static void DrawLine24BitBlend (int x1, int y1, int x2, int y2, SoftwarePainter* pThis);
	static void DrawLine32BitBlend (int x1, int y1, int x2, int y2, SoftwarePainter* pThis);
	static void DrawLine8BitXor    (int x1, int y1, int x2, int y2, SoftwarePainter* pThis);
	static void DrawLine16BitXor   (int x1, int y1, int x2, int y2, SoftwarePainter* pThis);
	static void DrawLine24BitXor   (int x1, int y1, int x2, int y2, SoftwarePainter* pThis);
	static void DrawLine32BitXor   (int x1, int y1, int x2, int y2, SoftwarePainter* pThis);
	static void DrawLine8BitAdd    (int x1, int y1, int x2, int y2, SoftwarePainter* pThis);
	static void DrawLine15BitAdd   (int x1, int y1, int x2, int y2, SoftwarePainter* pThis);
	static void DrawLine16BitAdd   (int x1, int y1, int x2, int y2, SoftwarePainter* pThis);
	static void DrawLine24BitAdd   (int x1, int y1, int x2, int y2, SoftwarePainter* pThis);
	static void DrawLine32BitAdd   (int x1, int y1, int x2, int y2, SoftwarePainter* pThis);

	static void FillShadedRect8BitNormal (int pLeft, int pTop, int pRight, int pBottom, SoftwarePainter* pThis);
	static void FillShadedRect15BitNormal(int pLeft, int pTop, int pRight, int pBottom, SoftwarePainter* pThis);
	static void FillShadedRect16BitNormal(int pLeft, int pTop, int pRight, int pBottom, SoftwarePainter* pThis);
	static void FillShadedRect24BitNormal(int pLeft, int pTop, int pRight, int pBottom, SoftwarePainter* pThis);
	static void FillShadedRect32BitNormal(int pLeft, int pTop, int pRight, int pBottom, SoftwarePainter* pThis);
	static void FillShadedRect15BitBlend (int pLeft, int pTop, int pRight, int pBottom, SoftwarePainter* pThis);
	static void FillShadedRect16BitBlend (int pLeft, int pTop, int pRight, int pBottom, SoftwarePainter* pThis);
	static void FillShadedRect24BitBlend (int pLeft, int pTop, int pRight, int pBottom, SoftwarePainter* pThis);
	static void FillShadedRect32BitBlend (int pLeft, int pTop, int pRight, int pBottom, SoftwarePainter* pThis);
	static void FillShadedRect15BitXor   (int pLeft, int pTop, int pRight, int pBottom, SoftwarePainter* pThis);
	static void FillShadedRect16BitXor   (int pLeft, int pTop, int pRight, int pBottom, SoftwarePainter* pThis);
	static void FillShadedRect24BitXor   (int pLeft, int pTop, int pRight, int pBottom, SoftwarePainter* pThis);
	static void FillShadedRect32BitXor   (int pLeft, int pTop, int pRight, int pBottom, SoftwarePainter* pThis);
	static void FillShadedRect15BitAdd   (int pLeft, int pTop, int pRight, int pBottom, SoftwarePainter* pThis);
	static void FillShadedRect16BitAdd   (int pLeft, int pTop, int pRight, int pBottom, SoftwarePainter* pThis);
	static void FillShadedRect24BitAdd   (int pLeft, int pTop, int pRight, int pBottom, SoftwarePainter* pThis);
	static void FillShadedRect32BitAdd   (int pLeft, int pTop, int pRight, int pBottom, SoftwarePainter* pThis);

	// Blend2 is blending without an alpha buffer.
	static void DrawImage8BitNormal (const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);
	static void DrawImage16BitNormal(const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);
	static void DrawImage24BitNormal(const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);
	static void DrawImage32BitNormal(const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);
	static void DrawImage8BitTest   (const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);
	static void DrawImage16BitTest  (const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);
	static void DrawImage24BitTest  (const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);
	static void DrawImage32BitTest  (const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);
	static void DrawImage15BitBlend (const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);
	static void DrawImage16BitBlend (const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);
	static void DrawImage24BitBlend (const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);
	static void DrawImage32BitBlend (const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);
	static void DrawImage15BitBlend2(const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);
	static void DrawImage16BitBlend2(const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);
	static void DrawImage24BitBlend2(const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);
	static void DrawImage32BitBlend2(const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);
	static void DrawImage8BitXor    (const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);
	static void DrawImage15BitXor   (const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);
	static void DrawImage16BitXor   (const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);
	static void DrawImage24BitXor   (const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);
	static void DrawImage32BitXor   (const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);
	static void DrawImage8BitAdd    (const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);
	static void DrawImage15BitAdd   (const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);
	static void DrawImage16BitAdd   (const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);
	static void DrawImage24BitAdd   (const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);
	static void DrawImage32BitAdd   (const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);

	// S = Stretched
	static void DrawSImage8BitNormal (const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);
	static void DrawSImage16BitNormal(const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);
	static void DrawSImage24BitNormal(const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);
	static void DrawSImage32BitNormal(const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);
	static void DrawSImage8BitTest   (const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);
	static void DrawSImage16BitTest  (const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);
	static void DrawSImage24BitTest  (const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);
	static void DrawSImage32BitTest  (const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);
	static void DrawSImage15BitBlend (const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);
	static void DrawSImage16BitBlend (const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);
	static void DrawSImage24BitBlend (const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);
	static void DrawSImage32BitBlend (const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);
	static void DrawSImage15BitBlend2(const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);
	static void DrawSImage16BitBlend2(const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);
	static void DrawSImage24BitBlend2(const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);
	static void DrawSImage32BitBlend2(const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);
	static void DrawSImage8BitXor    (const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);
	static void DrawSImage15BitXor   (const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);
	static void DrawSImage16BitXor   (const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);
	static void DrawSImage24BitXor   (const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);
	static void DrawSImage32BitXor   (const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);
	static void DrawSImage8BitAdd    (const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);
	static void DrawSImage15BitAdd   (const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);
	static void DrawSImage16BitAdd   (const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);
	static void DrawSImage24BitAdd   (const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);
	static void DrawSImage32BitAdd   (const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis);

	static void DrawAlphaImage8BitNormal (const Lepra::Canvas& pCharacter, int x, int y, SoftwarePainter* pThis);
	static void DrawAlphaImage15BitNormal(const Lepra::Canvas& pCharacter, int x, int y, SoftwarePainter* pThis);
	static void DrawAlphaImage16BitNormal(const Lepra::Canvas& pCharacter, int x, int y, SoftwarePainter* pThis);
	static void DrawAlphaImage24BitNormal(const Lepra::Canvas& pCharacter, int x, int y, SoftwarePainter* pThis);
	static void DrawAlphaImage32BitNormal(const Lepra::Canvas& pCharacter, int x, int y, SoftwarePainter* pThis);
	static void DrawAlphaImage15BitBlend (const Lepra::Canvas& pCharacter, int x, int y, SoftwarePainter* pThis);
	static void DrawAlphaImage16BitBlend (const Lepra::Canvas& pCharacter, int x, int y, SoftwarePainter* pThis);
	static void DrawAlphaImage24BitBlend (const Lepra::Canvas& pCharacter, int x, int y, SoftwarePainter* pThis);
	static void DrawAlphaImage32BitBlend (const Lepra::Canvas& pCharacter, int x, int y, SoftwarePainter* pThis);
	static void DrawAlphaImage8BitTest   (const Lepra::Canvas& pCharacter, int x, int y, SoftwarePainter* pThis);
	static void DrawAlphaImage16BitTest  (const Lepra::Canvas& pCharacter, int x, int y, SoftwarePainter* pThis);
	static void DrawAlphaImage24BitTest  (const Lepra::Canvas& pCharacter, int x, int y, SoftwarePainter* pThis);
	static void DrawAlphaImage32BitTest  (const Lepra::Canvas& pCharacter, int x, int y, SoftwarePainter* pThis);
	static void DrawAlphaImage8BitXor    (const Lepra::Canvas& pCharacter, int x, int y, SoftwarePainter* pThis);
	static void DrawAlphaImage16BitXor   (const Lepra::Canvas& pCharacter, int x, int y, SoftwarePainter* pThis);
	static void DrawAlphaImage24BitXor   (const Lepra::Canvas& pCharacter, int x, int y, SoftwarePainter* pThis);
	static void DrawAlphaImage32BitXor   (const Lepra::Canvas& pCharacter, int x, int y, SoftwarePainter* pThis);
	static void DrawAlphaImage8BitAdd    (const Lepra::Canvas& pCharacter, int x, int y, SoftwarePainter* pThis);
	static void DrawAlphaImage15BitAdd   (const Lepra::Canvas& pCharacter, int x, int y, SoftwarePainter* pThis);
	static void DrawAlphaImage16BitAdd   (const Lepra::Canvas& pCharacter, int x, int y, SoftwarePainter* pThis);
	static void DrawAlphaImage24BitAdd   (const Lepra::Canvas& pCharacter, int x, int y, SoftwarePainter* pThis);
	static void DrawAlphaImage32BitAdd   (const Lepra::Canvas& pCharacter, int x, int y, SoftwarePainter* pThis);

	static void DrawScanLine8BitNormal (const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, SoftwarePainter* pThis);
	static void DrawScanLine16BitNormal(const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, SoftwarePainter* pThis);
	static void DrawScanLine24BitNormal(const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, SoftwarePainter* pThis);
	static void DrawScanLine32BitNormal(const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, SoftwarePainter* pThis);
	static void DrawScanLine8BitTest   (const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, SoftwarePainter* pThis);
	static void DrawScanLine16BitTest  (const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, SoftwarePainter* pThis);
	static void DrawScanLine24BitTest  (const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, SoftwarePainter* pThis);
	static void DrawScanLine32BitTest  (const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, SoftwarePainter* pThis);
	static void DrawScanLine15BitBlend (const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, SoftwarePainter* pThis);
	static void DrawScanLine16BitBlend (const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, SoftwarePainter* pThis);
	static void DrawScanLine24BitBlend (const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, SoftwarePainter* pThis);
	static void DrawScanLine32BitBlend (const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, SoftwarePainter* pThis);
	static void DrawScanLine15BitBlend2(const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, SoftwarePainter* pThis);
	static void DrawScanLine16BitBlend2(const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, SoftwarePainter* pThis);
	static void DrawScanLine24BitBlend2(const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, SoftwarePainter* pThis);
	static void DrawScanLine32BitBlend2(const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, SoftwarePainter* pThis);
	static void DrawScanLine8BitXor    (const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, SoftwarePainter* pThis);
	static void DrawScanLine15BitXor   (const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, SoftwarePainter* pThis);
	static void DrawScanLine16BitXor   (const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, SoftwarePainter* pThis);
	static void DrawScanLine24BitXor   (const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, SoftwarePainter* pThis);
	static void DrawScanLine32BitXor   (const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, SoftwarePainter* pThis);
	static void DrawScanLine8BitAdd    (const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, SoftwarePainter* pThis);
	static void DrawScanLine15BitAdd   (const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, SoftwarePainter* pThis);
	static void DrawScanLine16BitAdd   (const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, SoftwarePainter* pThis);
	static void DrawScanLine24BitAdd   (const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, SoftwarePainter* pThis);
	static void DrawScanLine32BitAdd   (const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, SoftwarePainter* pThis);

	static void DrawScanLinePow28BitNormal (const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, unsigned pShift, SoftwarePainter* pThis);
	static void DrawScanLinePow216BitNormal(const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, unsigned pShift, SoftwarePainter* pThis);
	static void DrawScanLinePow224BitNormal(const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, unsigned pShift, SoftwarePainter* pThis);
	static void DrawScanLinePow232BitNormal(const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, unsigned pShift, SoftwarePainter* pThis);
	static void DrawScanLinePow28BitTest   (const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, unsigned pShift, SoftwarePainter* pThis);
	static void DrawScanLinePow216BitTest  (const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, unsigned pShift, SoftwarePainter* pThis);
	static void DrawScanLinePow224BitTest  (const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, unsigned pShift, SoftwarePainter* pThis);
	static void DrawScanLinePow232BitTest  (const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, unsigned pShift, SoftwarePainter* pThis);
	static void DrawScanLinePow215BitBlend (const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, unsigned pShift, SoftwarePainter* pThis);
	static void DrawScanLinePow216BitBlend (const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, unsigned pShift, SoftwarePainter* pThis);
	static void DrawScanLinePow224BitBlend (const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, unsigned pShift, SoftwarePainter* pThis);
	static void DrawScanLinePow232BitBlend (const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, unsigned pShift, SoftwarePainter* pThis);
	static void DrawScanLinePow215BitBlend2(const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, unsigned pShift, SoftwarePainter* pThis);
	static void DrawScanLinePow216BitBlend2(const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, unsigned pShift, SoftwarePainter* pThis);
	static void DrawScanLinePow224BitBlend2(const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, unsigned pShift, SoftwarePainter* pThis);
	static void DrawScanLinePow232BitBlend2(const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, unsigned pShift, SoftwarePainter* pThis);
	static void DrawScanLinePow28BitXor    (const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, unsigned pShift, SoftwarePainter* pThis);
	static void DrawScanLinePow215BitXor   (const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, unsigned pShift, SoftwarePainter* pThis);
	static void DrawScanLinePow216BitXor   (const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, unsigned pShift, SoftwarePainter* pThis);
	static void DrawScanLinePow224BitXor   (const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, unsigned pShift, SoftwarePainter* pThis);
	static void DrawScanLinePow232BitXor   (const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, unsigned pShift, SoftwarePainter* pThis);
	static void DrawScanLinePow28BitAdd    (const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, unsigned pShift, SoftwarePainter* pThis);
	static void DrawScanLinePow215BitAdd   (const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, unsigned pShift, SoftwarePainter* pThis);
	static void DrawScanLinePow216BitAdd   (const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, unsigned pShift, SoftwarePainter* pThis);
	static void DrawScanLinePow224BitAdd   (const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, unsigned pShift, SoftwarePainter* pThis);
	static void DrawScanLinePow232BitAdd   (const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, unsigned pShift, SoftwarePainter* pThis);

	Lepra::uint8 mColor8[4];	// 8 bit colors.
	Lepra::uint16 mColor16[4];	// 15 and 16 bit colors.
	unsigned mColor32[4];	// 32 bit colors

	bool mSwapRGB;
	bool mIncrementalAlpha;

	class Texture
	{
	public:
		Texture() :
			mColorMap(0),
			mAlphaBuffer(0)
		{
		}

		Lepra::Canvas* mColorMap;
		Lepra::Canvas* mAlphaBuffer;
	};

	class SoftwareFont : public Painter::Font
	{
	public:
		SoftwareFont(int pFirstChar, int pLastChar) :
			Font(pFirstChar, pLastChar),
			mAlphaImage(false),
			mCharRect(new Lepra::PixelRect[pLastChar - pFirstChar + 1]),
			mChar(new Lepra::Canvas[pLastChar - pFirstChar + 1])
		{
		}

		~SoftwareFont()
		{
			delete[] mCharRect;
			delete[] mChar;
		}

		void GetUVRect(const Lepra::tchar& pChar, float& pU1, float& pV1, float& pU2, float& pV2) const
		{
			float lTWidthRecip;
			float lTHeightRecip;
			if(mAlphaImage)
			{
				lTWidthRecip = 1.0f / (float)mChar->GetWidth();
				lTHeightRecip = 1.0f / (float)mChar->GetHeight();
			}
			else
			{
				lTWidthRecip = 1.0f / (float)mTexture->mColorMap->GetWidth();
				lTHeightRecip = 1.0f / (float)mTexture->mColorMap->GetHeight();
			}

			Lepra::PixelRect& lCharRect = mCharRect[pChar];
			pU1 = (float)lCharRect.mLeft * lTWidthRecip;
			pV1 = (float)lCharRect.mTop * lTHeightRecip;
			pU2 = (float)lCharRect.mRight * lTWidthRecip;
			pV2 = (float)lCharRect.mBottom * lTHeightRecip;
		}

		Texture* mTexture;
		bool mAlphaImage;

		Lepra::PixelRect* mCharRect;
		Lepra::Canvas*    mChar;
	};

	typedef Lepra::HashTable<int, Texture*> TextureTable;

	Font* NewFont(int pFirstChar, int pLastChar) const;
	void InitFont(Font* pFont, const Lepra::Canvas& pFontImage);
	void GetImageSize(ImageID pImageID, int& pWidth, int& pHeight);
	int DoPrintText(const Lepra::String& pString, int x, int y);

	Lepra::IdManager<int> mTextureIDManager;
	TextureTable mTextureTable;
};

void SoftwarePainter::SwapRGB()
{
	mSwapRGB = !mSwapRGB;
}

void SoftwarePainter::SetIncrementalAlpha(bool pIncrementalAlpha)
{
	mIncrementalAlpha = pIncrementalAlpha;
}

} // End namespace.

#endif