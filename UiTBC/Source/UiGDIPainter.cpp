/*
	Class:  GDIPainter
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand
*/

#include <Windows.h>
#include "../../Lepra/Include/Canvas.h"
#include "../../Lepra/Include/DllLoader.h"
#include "../../Lepra/Include/Math.h"
#include "../../UiLepra/Include/UiDisplayManager.h"
#include "../../UiLepra/Include/Win32/UiWin32DisplayManager.h"
#include "../Include/UiGDIPainter.h"

#pragma comment(lib, "msimg32.lib")

namespace UiTbc
{

GDIPainter::GDIPainter(UiLepra::Win32DisplayManager* pDisplayManager) :
	mDisplayManager(pDisplayManager),
	SystemPainter(),
	mTextureIDManager(1, 10000, 0)
{
	HWND lWnd = mDisplayManager->GetHWND();
	mDC = ::GetDC(lWnd);

	COLORREF lColor = 0x00FFFFFF;

	mColorRef[0] = lColor;
	mColorRef[1] = lColor;
	mColorRef[2] = lColor;
	mColorRef[3] = lColor;

	mBrush[0] = ::CreateSolidBrush(lColor);
	mBrush[1] = ::CreateSolidBrush(lColor);
	mBrush[2] = ::CreateSolidBrush(lColor);
	mBrush[3] = ::CreateSolidBrush(lColor);

	mPen[0] = ::CreatePen(PS_SOLID, 1, lColor);
	mPen[1] = ::CreatePen(PS_SOLID, 1, lColor);
	mPen[2] = ::CreatePen(PS_SOLID, 1, lColor);
	mPen[3] = ::CreatePen(PS_SOLID, 1, lColor);

	LOGBRUSH lBrushData;
	lBrushData.lbStyle = BS_HOLLOW;

	mNullBrush = ::CreateBrushIndirect(&lBrushData);
	mNullPen = ::CreatePen(PS_NULL, 0, lColor);
}

GDIPainter::~GDIPainter()
{
	::DeleteObject(mBrush[0]);
	::DeleteObject(mBrush[1]);
	::DeleteObject(mBrush[2]);
	::DeleteObject(mBrush[3]);

	::DeleteObject(mPen[0]);
	::DeleteObject(mPen[1]);
	::DeleteObject(mPen[2]);
	::DeleteObject(mPen[3]);

	::DeleteObject(mNullBrush);
	::DeleteObject(mNullPen);

	while (mBitmapTable.IsEmpty() == false)
	{
		BitmapTable::Iterator lIter = mBitmapTable.First();
		HGDIOBJ lBitmap = (*lIter).mBitmap;
		HGDIOBJ lBitmap2 = (*lIter).mBitmap2;

		mBitmapTable.Remove(lIter);
		::DeleteObject(lBitmap);

		if (lBitmap2 != lBitmap)
		{
			::DeleteObject(lBitmap2);
		}
	}

	::ReleaseDC(mDisplayManager->GetHWND(), mDC);

	mDisplayManager = 0;
}

void GDIPainter::SetRenderMode(RenderMode pRM)
{
	Painter::SetRenderMode(pRM);

	switch(pRM)
	{
		case RM_NORMAL:
			::SetBkMode(mDC, OPAQUE);
			break;
		case RM_ALPHATEST:
		case RM_ALPHABLEND:
			::SetBkMode(mDC, TRANSPARENT);
			break;
		case RM_XOR:
			::SetBkMode(mDC, OPAQUE);
			break;
		case RM_ADD:
			::SetBkMode(mDC, OPAQUE);
			break;
	}
}

void GDIPainter::SetClippingRect(int pLeft, int pTop, int pRight, int pBottom)
{
	Painter::SetClippingRect(pLeft, pTop, pRight, pBottom);
}

void GDIPainter::ResetClippingRect()
{
}

void GDIPainter::SetColor(const Lepra::Color& pColor, unsigned pColorIndex)
{
	Painter::SetColor(pColor, pColorIndex);
	
	DeleteObject(mBrush[pColorIndex]);
	DeleteObject(mPen[pColorIndex]);

	COLORREF lColor = (((unsigned)pColor.mBlue) << 16) |
			   (((unsigned)pColor.mGreen) << 8) |
			   (((unsigned)pColor.mRed));

	mColorRef[pColorIndex] = lColor;
	mBrush[pColorIndex] = CreateSolidBrush(lColor);
	mPen[pColorIndex] = CreatePen(PS_SOLID, 1, lColor);
}

void GDIPainter::DoDrawPixel(int x, int y)
{
	::SetPixel(mDC, x, y, mColorRef[0]);
}

void GDIPainter::DoDrawLine(int pX1, int pY1, int pX2, int pY2)
{
	::SelectObject(mDC, mPen[0]);
	POINT lPoints[2];

	lPoints[0].x = pX1;
	lPoints[0].y = pY1;
	lPoints[1].x = pX2;
	lPoints[1].y = pY2;

	::Polyline(mDC, lPoints, 2);
}

void GDIPainter::_DrawLine(int pX1, int pY1, int pX2, int pY2)
{
	POINT lPoints[2];

	lPoints[0].x = pX1;
	lPoints[0].y = pY1;
	lPoints[1].x = pX2;
	lPoints[1].y = pY2;

	::Polyline(mDC, lPoints, 2);
}

void GDIPainter::DoDrawRect(int pLeft, int pTop, int pRight, int pBottom, int pWidth)
{
	RECT lRect;
	lRect.left   = pLeft;
	lRect.top    = pTop;
	lRect.right  = pRight;
	lRect.bottom = pBottom;

	int lRed   = GetColor(0).mRed;
	int lGreen = GetColor(0).mGreen;
	int lBlue  = GetColor(0).mBlue;

	// Use fixed point arithmetic with 9 bits integer part and 23 bits fractional.
	int lRStep = ((GetColor(1).mRed   - lRed)   << 23) / pWidth;
	int lGStep = ((GetColor(1).mGreen - lGreen) << 23) / pWidth;
	int lBStep = ((GetColor(1).mBlue  - lBlue)  << 23) / pWidth;

	lRed   <<= 23;
	lGreen <<= 23;
	lBlue  <<= 23;

	for(int i = 0; i < pWidth; i++)
	{
		HBRUSH lBrush = ::CreateSolidBrush(RGB((lRed >> 23), (lGreen >> 23), (lBlue >> 23)));

		::FrameRect(mDC, &lRect, lBrush);

		lRect.left++;
		lRect.top++;
		lRect.right--;
		lRect.bottom--;

		lRed   += lRStep;
		lGreen += lGStep;
		lBlue  += lBStep;

		::DeleteObject(lBrush);
	}
}

void GDIPainter::DoFillRect(int pLeft, int pTop, int pRight, int pBottom)
{
	RECT lRect;
	lRect.left   = pLeft;
	lRect.top    = pTop;
	lRect.right  = pRight;
	lRect.bottom = pBottom;

	::FillRect(mDC, &lRect, mBrush[0]);
}

void GDIPainter::DoDraw3DRect(int pLeft, int pTop, int pRight, int pBottom, int pWidth, bool pSunken)
{
	int lZero  = 0;
	int lOne   = 1;
	int lTwo   = 2;
	int lThree = 3;

	if(pSunken)
	{
		lZero  = 2;
		lOne   = 3;
		lTwo   = 0;
		lThree = 1;
	}

	// Top left outer color.
	int lRTL = GetColor(lZero).mRed;
	int lGTL = GetColor(lZero).mGreen;
	int lBTL = GetColor(lZero).mBlue;

	// Bottom right outer color.
	int lRBR = GetColor(lOne).mRed;
	int lGBR = GetColor(lOne).mGreen;
	int lBBR = GetColor(lOne).mBlue;

	// Use fixed point arithmetic with 9 bits integer part and 23 bits fractional.
	int lRTLStep = ((GetColor(lTwo).mRed   - lRTL) << 23) / pWidth;
	int lGTLStep = ((GetColor(lTwo).mGreen - lGTL) << 23) / pWidth;
	int lBTLStep = ((GetColor(lTwo).mBlue  - lBTL) << 23) / pWidth;

	int lRBRStep = ((GetColor(lThree).mRed   - lRBR) << 23) / pWidth;
	int lGBRStep = ((GetColor(lThree).mGreen - lGBR) << 23) / pWidth;
	int lBBRStep = ((GetColor(lThree).mBlue  - lBBR) << 23) / pWidth;

	lRTL <<= 23;
	lGTL <<= 23;
	lBTL <<= 23;

	lRBR <<= 23;
	lGBR <<= 23;
	lBBR <<= 23;

	for(int i = 0; i < pWidth; i++)
	{
		HPEN lPenTL = CreatePen(PS_SOLID, 1, RGB((lRTL >> 23), (lGTL >> 23), (lBTL >> 23)));
		HPEN lPenBR = CreatePen(PS_SOLID, 1, RGB((lRBR >> 23), (lGBR >> 23), (lBBR >> 23)));

		::SelectObject(mDC, lPenTL);
		_DrawLine(pLeft, pTop, pRight, pTop);
		_DrawLine(pLeft, pTop + 1, pLeft, pBottom);

		::SelectObject(mDC, lPenBR);
		_DrawLine(pLeft, pBottom, pRight, pBottom);
		_DrawLine(pRight, pTop, pRight, pBottom + 1);

		pLeft++;
		pTop++;
		pRight--;
		pBottom--;

		lRTL += lRTLStep;
		lGTL += lGTLStep;
		lBTL += lBTLStep;

		lRBR += lRBRStep;
		lGBR += lGBRStep;
		lBBR += lBBRStep;

		::DeleteObject(lPenTL);
		::DeleteObject(lPenBR);
	}
}

void GDIPainter::DoFillShadedRect(int pLeft, int pTop, int pRight, int pBottom)
{
	int lMiddleX = pLeft + ((pRight  - pLeft) >> 1);
	int lMiddleY = pTop  + ((pBottom - pTop)  >> 1);

	TRIVERTEX lP[5];
	lP[0].x = pLeft;
	lP[0].y = pTop;
	lP[1].x = pRight;
	lP[1].y = pTop;
	lP[2].x = pRight;
	lP[2].y = pBottom;
	lP[3].x = pLeft;
	lP[3].y = pBottom;
	lP[4].x = lMiddleX;
	lP[4].y = lMiddleY;

	Lepra::Color* lColor = &GetColorInternal(0);
	lP[0].Red   = (((Lepra::uint16)lColor[0].mRed) << 8);
	lP[0].Green = (((Lepra::uint16)lColor[0].mGreen) << 8);
	lP[0].Blue  = (((Lepra::uint16)lColor[0].mBlue) << 8);
	lP[0].Alpha = 0xFFFF;

	lP[1].Red   = (((Lepra::uint16)lColor[1].mRed) << 8);
	lP[1].Green = (((Lepra::uint16)lColor[1].mGreen) << 8);
	lP[1].Blue  = (((Lepra::uint16)lColor[1].mBlue) << 8);
	lP[1].Alpha = 0xFFFF;

	lP[2].Red   = (((Lepra::uint16)lColor[2].mRed) << 8);
	lP[2].Green = (((Lepra::uint16)lColor[2].mGreen) << 8);
	lP[2].Blue  = (((Lepra::uint16)lColor[2].mBlue) << 8);
	lP[2].Alpha = 0xFFFF;

	lP[3].Red   = (((Lepra::uint16)lColor[3].mRed) << 8);
	lP[3].Green = (((Lepra::uint16)lColor[3].mGreen) << 8);
	lP[3].Blue  = (((Lepra::uint16)lColor[3].mBlue) << 8);
	lP[3].Alpha = 0xFFFF;

	// Calculate the color in the middle of the rect.
	float lMiddleUpperRed   = (float)lP[0].Red   + (float)(lP[1].Red   - lP[0].Red)   * 0.5f;
	float lMiddleUpperGreen = (float)lP[0].Green + (float)(lP[1].Green - lP[0].Green) * 0.5f;
	float lMiddleUpperBlue  = (float)lP[0].Blue  + (float)(lP[1].Blue  - lP[0].Blue)  * 0.5f;

	float lMiddleLowerRed   = (float)lP[3].Red   + (float)(lP[2].Red   - lP[3].Red)   * 0.5f;
	float lMiddleLowerGreen = (float)lP[3].Green + (float)(lP[2].Green - lP[3].Green) * 0.5f;
	float lMiddleLowerBlue  = (float)lP[3].Blue  + (float)(lP[2].Blue  - lP[3].Blue)  * 0.5f;

	lP[4].Red   = (Lepra::uint16)(lMiddleUpperRed   + (lMiddleLowerRed   - lMiddleUpperRed) * 0.5f);
	lP[4].Green = (Lepra::uint16)(lMiddleUpperGreen + (lMiddleLowerGreen - lMiddleUpperGreen) * 0.5f);
	lP[4].Blue  = (Lepra::uint16)(lMiddleUpperBlue  + (lMiddleLowerBlue  - lMiddleUpperBlue) * 0.5f);
	lP[4].Alpha = 0xFFFF;

	GRADIENT_TRIANGLE lIndices[4];
	lIndices[0].Vertex1 = 0;
	lIndices[0].Vertex2 = 1;
	lIndices[0].Vertex3 = 4;
	lIndices[1].Vertex1 = 1;
	lIndices[1].Vertex2 = 2;
	lIndices[1].Vertex3 = 4;
	lIndices[2].Vertex1 = 2;
	lIndices[2].Vertex2 = 3;
	lIndices[2].Vertex3 = 4;
	lIndices[3].Vertex1 = 3;
	lIndices[3].Vertex2 = 0;
	lIndices[3].Vertex3 = 4;

	::GradientFill(mDC, lP, 5, &lIndices, 4, GRADIENT_FILL_TRIANGLE);
}

void GDIPainter::DoFillTriangle(float pX1, float pY1,
			      float pX2, float pY2,
			      float pX3, float pY3)
{
	::SelectObject(mDC, mBrush[0]);
	::SelectObject(mDC, mNullPen);

	POINT lPoints[3];

	lPoints[0].x = (int)pX1;
	lPoints[0].y = (int)pY1;
	lPoints[1].x = (int)pX2;
	lPoints[1].y = (int)pY2;
	lPoints[2].x = (int)pX3;
	lPoints[2].y = (int)pY3;

	::Polygon(mDC, lPoints, 3);
}

void GDIPainter::DoFillShadedTriangle(float pX1, float pY1,
				    float pX2, float pY2,
				    float pX3, float pY3)
{
	DoFillTriangle(pX1, pY1, pX2, pY2, pX3, pY3);
}

void GDIPainter::DoFillTriangle(float pX1, float pY1, float, float,
			      float pX2, float pY2, float, float,
			      float pX3, float pY3, float, float,
			      ImageID)
{
	// GDI doesn't support textured triangles.
	DoFillTriangle(pX1, pY1, pX2, pY2, pX3, pY3);
}

Painter::ImageID GDIPainter::AddImage(const Lepra::Canvas* pBitmap, const Lepra::Canvas* pAlphaBuffer)
{
	if (pBitmap == 0 && (pAlphaBuffer == 0 || pAlphaBuffer->GetBitDepth() != Lepra::Canvas::BITDEPTH_8_BIT))
	{
		return (ImageID)mTextureIDManager.GetInvalidId();
	}

	int lID = mTextureIDManager.GetFreeId();

	if (lID == mTextureIDManager.GetInvalidId())
	{
		return (ImageID)mTextureIDManager.GetInvalidId();
	}

	_AddImage((ImageID)lID, pBitmap, pAlphaBuffer);

	return (ImageID)lID;
}

void GDIPainter::_AddImage(ImageID pImageID, const Lepra::Canvas* pBitmap, const Lepra::Canvas* pAlphaBuffer)
{
	const Lepra::Canvas* lBitmap = pBitmap;
	Lepra::Canvas lCopy;

	if (pBitmap != 0 && pAlphaBuffer != 0)
	{
		lCopy.Copy(*pBitmap);
		lCopy.ConvertTo32BitWithAlpha(*pAlphaBuffer);
		lBitmap = &lCopy;
	}
	else if(pBitmap != 0)
	{
		// Convert it to 32 bit, since that's the only format that seems to work correctly.
		if (pBitmap->GetBitDepth() != Lepra::Canvas::BITDEPTH_32_BIT)
		{
			lCopy.Copy(*pBitmap);
			lCopy.ConvertBitDepth(Lepra::Canvas::BITDEPTH_32_BIT);
			lBitmap = &lCopy;
		}
	}
	else
	{
		// Create a grayscale palette.
		Lepra::Color lPalette[256];
		for (int i = 0; i < 256; i++)
		{
			lPalette[i].Set(i, i, i, i);
		}

		lCopy.Copy(*pAlphaBuffer);
		lCopy.SetPalette(lPalette);
		lCopy.ConvertTo32BitWithAlpha(*pAlphaBuffer);
		lBitmap = &lCopy;
	}

	Win32BitmapInfo lBitmapInfo;

	memset(&lBitmapInfo, 0, sizeof(lBitmapInfo));

	lBitmapInfo.bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
	lBitmapInfo.bmiHeader.biWidth         = lBitmap->GetWidth();
	lBitmapInfo.bmiHeader.biHeight        = -(int)lBitmap->GetHeight(); // Top-down bitmap
	lBitmapInfo.bmiHeader.biPlanes        = 1;
	lBitmapInfo.bmiHeader.biBitCount      = (WORD)32;
	lBitmapInfo.bmiHeader.biCompression   = BI_RGB;
	lBitmapInfo.bmiHeader.biSizeImage     = NULL;
	lBitmapInfo.bmiHeader.biXPelsPerMeter = NULL;
	lBitmapInfo.bmiHeader.biYPelsPerMeter = NULL;
	lBitmapInfo.bmiHeader.biClrUsed       = 256;
	lBitmapInfo.bmiHeader.biClrImportant  = 256;

	HGDIOBJ lBitmap1;
	HGDIOBJ lBitmap2 = 0;

	lBitmap1 = ::CreateDIBitmap(mDC,
				     &lBitmapInfo.bmiHeader,
				     CBM_INIT,
				     lBitmap->GetBuffer(),
				     (BITMAPINFO*)&lBitmapInfo,
				     DIB_RGB_COLORS);

	if (pAlphaBuffer != 0)
	{
		Lepra::Canvas lTemp(*lBitmap, true);
		lTemp.PremultiplyAlpha();

		lBitmap2 = ::CreateDIBitmap(mDC,
					      &lBitmapInfo.bmiHeader,
					      CBM_INIT,
					      lTemp.GetBuffer(),
					      (BITMAPINFO*)&lBitmapInfo,
					      DIB_RGB_COLORS);
	}
	else
	{
		lBitmap2 = lBitmap1;
	}

	mBitmapTable.Insert(pImageID, TextureInfo(lBitmap1,
						    lBitmap2,
						    lBitmap->GetWidth(),
						    lBitmap->GetHeight(),
						    Lepra::Canvas::BitDepthToInt(lBitmap->GetBitDepth()),
						    (Lepra::uint8*)lBitmap->GetBuffer()));
}

void GDIPainter::UpdateImage(ImageID pImageID, const Lepra::Canvas* pBitmap, const Lepra::Canvas* pAlphaBuffer, Painter::UpdateHint)
{
	BitmapTable::Iterator lIter = mBitmapTable.Find(pImageID);

	if (lIter != mBitmapTable.End())
	{
		RemoveImage(pImageID);
		_AddImage(pImageID, pBitmap, pAlphaBuffer);
	}
}

void GDIPainter::RemoveImage(ImageID pImageID)
{
	BitmapTable::Iterator lIter = mBitmapTable.Find(pImageID);

	if (lIter != mBitmapTable.End())
	{
		HGDIOBJ lBitmap = (*lIter).mBitmap;
		HGDIOBJ lBitmap2 = (*lIter).mBitmap2;
		mBitmapTable.Remove(lIter);
		::DeleteObject(lBitmap);

		if (lBitmap2 != lBitmap)
		{
			::DeleteObject(lBitmap2);
		}
	}
}

void GDIPainter::DoDrawImage(ImageID pImageID, int x, int y)
{
	HWND lWnd = mDisplayManager->GetHWND();
	mDC = ::GetDC(lWnd);

	BitmapTable::Iterator lIter = mBitmapTable.Find(pImageID);

	if (lIter != mBitmapTable.End())
	{
		HDC lContext = ::CreateCompatibleDC(mDC);

		if (GetRenderMode() != Painter::RM_ALPHABLEND && GetRenderMode() != Painter::RM_ALPHATEST)
		{
			HGDIOBJ lDefaultBitmap = ::SelectObject(lContext, (*lIter).mBitmap);
			::BitBlt(mDC, x, y, (*lIter).mWidth, (*lIter).mHeight, lContext, 0, 0, SRCCOPY);
			::SelectObject(lContext, lDefaultBitmap);
		}
		else
		{
			HGDIOBJ lDefaultBitmap = ::SelectObject(lContext, (*lIter).mBitmap2);
			BLENDFUNCTION lBlendFunc;
			
			lBlendFunc.BlendOp = AC_SRC_OVER;
			lBlendFunc.BlendFlags = 0;
			lBlendFunc.SourceConstantAlpha = GetAlphaValue();
			lBlendFunc.AlphaFormat = AC_SRC_ALPHA;

			::AlphaBlend(mDC, 
				     x, y, 
				     (*lIter).mWidth, (*lIter).mHeight, 
				     lContext, 
				     0, 0, 
				     (*lIter).mWidth, (*lIter).mHeight, 
				     lBlendFunc);
			::SelectObject(lContext, lDefaultBitmap);
		}

		::DeleteDC(lContext);
	}
}

void GDIPainter::DoDrawImage(ImageID pImageID, int x, int y, const Lepra::PixelRect& pSubpatchRect)
{
	BitmapTable::Iterator lIter = mBitmapTable.Find(pImageID);

	if (lIter != mBitmapTable.End())
	{
		HDC lContext = ::CreateCompatibleDC(mDC);

		if (GetRenderMode() != Painter::RM_ALPHABLEND && GetRenderMode() != Painter::RM_ALPHATEST)
		{
			HGDIOBJ lDefaultBitmap = ::SelectObject(lContext, (*lIter).mBitmap);
			::BitBlt(mDC, x, y, pSubpatchRect.GetWidth(), pSubpatchRect.GetHeight(), lContext, pSubpatchRect.mLeft, pSubpatchRect.mTop, SRCCOPY);
			::SelectObject(lContext, lDefaultBitmap);
		}
		else
		{
			HGDIOBJ lDefaultBitmap = ::SelectObject(lContext, (*lIter).mBitmap2);
			BLENDFUNCTION lBlendFunc;
			
			lBlendFunc.BlendOp = AC_SRC_OVER;
			lBlendFunc.BlendFlags = 0;
			lBlendFunc.SourceConstantAlpha = GetAlphaValue();
			lBlendFunc.AlphaFormat = AC_SRC_ALPHA;

			::AlphaBlend(mDC,
				     x, y,
				     pSubpatchRect.GetWidth(), pSubpatchRect.GetHeight(),
				     lContext,
				     pSubpatchRect.mLeft, pSubpatchRect.mTop,
				     pSubpatchRect.GetWidth(), pSubpatchRect.GetHeight(),
				     lBlendFunc);
			::SelectObject(lContext, lDefaultBitmap);
		}

		::DeleteDC(lContext);
	}
}

void GDIPainter::DoDrawImage(ImageID pImageID, const Lepra::PixelRect& pRect)
{
	BitmapTable::Iterator lIter = mBitmapTable.Find(pImageID);

	if (lIter != mBitmapTable.End())
	{
		HDC lContext = ::CreateCompatibleDC(mDC);

		if (GetRenderMode() != Painter::RM_ALPHABLEND && GetRenderMode() != Painter::RM_ALPHATEST)
		{
			HGDIOBJ lDefaultBitmap = ::SelectObject(lContext, (*lIter).mBitmap);
			::StretchBlt(mDC, 
				     pRect.mLeft, pRect.mTop, 
				     pRect.GetWidth(), pRect.GetHeight(),
				     lContext, 
				     0, 0, 
				     (*lIter).mWidth, (*lIter).mHeight,
				     SRCCOPY);
			::SelectObject(lContext, lDefaultBitmap);
		}
		else
		{
			HGDIOBJ lDefaultBitmap = ::SelectObject(lContext, (*lIter).mBitmap2);
			BLENDFUNCTION lBlendFunc;
			
			lBlendFunc.BlendOp = AC_SRC_OVER;
			lBlendFunc.BlendFlags = 0;
			lBlendFunc.SourceConstantAlpha = GetAlphaValue();
			lBlendFunc.AlphaFormat = AC_SRC_ALPHA;

			::AlphaBlend(mDC, 
				     pRect.mLeft, pRect.mTop,
				     pRect.GetWidth(), pRect.GetHeight(),
				     lContext, 
				     0, 0,
				     pRect.GetWidth(), pRect.GetHeight(), 
				     lBlendFunc);
			::SelectObject(lContext, lDefaultBitmap);
		}

		::DeleteDC(lContext);
	}
}

void GDIPainter::DoDrawImage(ImageID pImageID, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubpatchRect)
{
	BitmapTable::Iterator lIter = mBitmapTable.Find(pImageID);

	if (lIter != mBitmapTable.End())
	{
		HDC lContext = ::CreateCompatibleDC(mDC);

		if (GetRenderMode() != Painter::RM_ALPHABLEND && GetRenderMode() != Painter::RM_ALPHATEST)
		{
			HGDIOBJ lDefaultBitmap = ::SelectObject(lContext, (*lIter).mBitmap);
			::StretchBlt(mDC, 
				     pRect.mLeft, pRect.mTop, 
				     pRect.GetWidth(), pRect.GetHeight(),
				     lContext, 
				     pSubpatchRect.mLeft, pSubpatchRect.mTop, 
				     pSubpatchRect.GetWidth(), pSubpatchRect.GetHeight(),
				     SRCCOPY);
			::SelectObject(lContext, lDefaultBitmap);
		}
		else
		{
			HGDIOBJ lDefaultBitmap = ::SelectObject(lContext, (*lIter).mBitmap2);
			BLENDFUNCTION lBlendFunc;
			
			lBlendFunc.BlendOp = AC_SRC_OVER;
			lBlendFunc.BlendFlags = 0;
			lBlendFunc.SourceConstantAlpha = GetAlphaValue();
			lBlendFunc.AlphaFormat = AC_SRC_ALPHA;

			::AlphaBlend(mDC, 
				     pRect.mLeft, pRect.mTop,
				     pRect.GetWidth(), pRect.GetHeight(),
				     lContext, 
				     pSubpatchRect.mLeft, pSubpatchRect.mTop, 
				     pSubpatchRect.GetWidth(), pSubpatchRect.GetHeight(),
				     lBlendFunc);
			::SelectObject(lContext, lDefaultBitmap);
		}

		::DeleteDC(lContext);
	}
}

void GDIPainter::DoDrawAlphaImage(ImageID pImageID, int x, int y)
{
	BitmapTable::Iterator lIter = mBitmapTable.Find(pImageID);

	if (lIter != mBitmapTable.End())
	{
		HDC lContext = ::CreateCompatibleDC(mDC);

		if (GetRenderMode() != Painter::RM_ALPHABLEND && GetRenderMode() != Painter::RM_ALPHATEST)
		{
			HGDIOBJ lDefaultBitmap = ::SelectObject(lContext, (*lIter).mBitmap);
			::BitBlt(mDC, x, y, (*lIter).mWidth, (*lIter).mHeight, lContext, 0, 0, SRCCOPY);
			::SelectObject(lContext, lDefaultBitmap);
		}
		else
		{
			HGDIOBJ lDefaultBitmap = ::SelectObject(lContext, (*lIter).mBitmap2);
			BLENDFUNCTION lBlendFunc;
			
			lBlendFunc.BlendOp = AC_SRC_OVER;
			lBlendFunc.BlendFlags = 0;
			lBlendFunc.SourceConstantAlpha = GetAlphaValue();
			lBlendFunc.AlphaFormat = AC_SRC_ALPHA;

			::AlphaBlend(mDC, x, y, (*lIter).mWidth, (*lIter).mHeight, lContext, 0, 0, (*lIter).mWidth, (*lIter).mHeight, lBlendFunc);
			::SelectObject(lContext, lDefaultBitmap);
		}

		::DeleteDC(lContext);
	}
}

void GDIPainter::DoDrawAlphaImage(ImageID pImageID, int x, int y, const Lepra::PixelRect& pSubpatchRect)
{
	BitmapTable::Iterator lIter = mBitmapTable.Find(pImageID);

	if (lIter != mBitmapTable.End())
	{
		HDC lContext = ::CreateCompatibleDC(mDC);

		if (GetRenderMode() != Painter::RM_ALPHABLEND && GetRenderMode() != Painter::RM_ALPHATEST)
		{
			HGDIOBJ lDefaultBitmap = ::SelectObject(lContext, (*lIter).mBitmap);
			::BitBlt(mDC, x, y, pSubpatchRect.GetWidth(), pSubpatchRect.GetHeight(), lContext, pSubpatchRect.mLeft, pSubpatchRect.mTop, SRCCOPY);
			::SelectObject(lContext, lDefaultBitmap);
		}
		else
		{
			HGDIOBJ lDefaultBitmap = ::SelectObject(lContext, (*lIter).mBitmap2);
			BLENDFUNCTION lBlendFunc;
			
			lBlendFunc.BlendOp = AC_SRC_OVER;
			lBlendFunc.BlendFlags = 0;
			lBlendFunc.SourceConstantAlpha = GetAlphaValue();
			lBlendFunc.AlphaFormat = AC_SRC_ALPHA;

			::AlphaBlend(mDC, 
				     x, y,
				     pSubpatchRect.GetWidth(), pSubpatchRect.GetHeight(),
				     lContext, 
				     pSubpatchRect.mLeft, pSubpatchRect.mTop, 
				     pSubpatchRect.GetWidth(), pSubpatchRect.GetHeight(),
				     lBlendFunc);
			::SelectObject(lContext, lDefaultBitmap);
		}

		::DeleteDC(lContext);
	}
}


Painter::Font* GDIPainter::NewFont(int pFirstChar, int pLastChar) const
{
	return new ImageFont(pFirstChar, pLastChar);
}

void GDIPainter::InitFont(Font* pFont, const Lepra::Canvas&)
{
	assert(pFont->IsSystemFont() == false); // Can't happen.
	ImageFont* lImageFont = (ImageFont*)pFont;
	BitmapTable::Iterator lIter = mBitmapTable.Find((int)lImageFont->mTextureID);
	if(lIter != mBitmapTable.End())
	{
		lImageFont->mTexture = &(*lIter);
	}
}

void GDIPainter::GetImageSize(ImageID pImageID, int& pWidth, int& pHeight)
{
	pWidth = 0;
	pHeight = 0;
	BitmapTable::Iterator lIter = mBitmapTable.Find((int)pImageID);
	if(lIter != mBitmapTable.End())
	{
		TextureInfo& lTInfo = *lIter;
		pWidth = lTInfo.mWidth;
		pHeight = lTInfo.mHeight;
	}
}

Painter::FontID GDIPainter::AddSystemFont(const Lepra::String& pFont, double pSize, Lepra::uint32 pFlags, CharacterSet pCharSet)
{
	int lWeight  = ((pFlags & BOLD) != 0) ? FW_BOLD : FW_NORMAL;
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
					  pFont.c_str());

	Painter::FontID lID = INVALID_FONTID;
	if (lFontHandle != NULL)
	{
		SystemFont* lFont = new SystemFont();
		lFont->mFont = lFontHandle;
		lFont->mSize = pSize;
		Painter::AddFont(lFont);

		if(lFont->mFontID == INVALID_FONTID)
		{
			delete lFont;
			::DeleteObject(lFontHandle);
		}
		else
		{
			lID = lFont->mFontID;
		}
	}

	return (Painter::FontID)lID;
}

bool GDIPainter::RenderGlyph(Lepra::tchar pChar, Lepra::Canvas& pImage, const Lepra::PixelRect& pRect)
{
	bool lOk = GetCurrentFontInternal()->IsSystemFont();
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
		HGDIOBJ lDefaultObject = ::SelectObject(lRamDc, ((SystemFont*)GetCurrentFontInternal())->mFont);
		::SetTextColor(lRamDc, mColorRef[0]);
		::SetBkColor(lRamDc, mColorRef[1]);
		lOk = (::TextOut(lRamDc, 0, 0, &pChar, 1) != FALSE);
		::SelectObject(lRamDc, lDefaultObject);
	}
	Lepra::uint8* lBitmap = 0;
	int lBpp = -1;
	if (lOk)
	{
		lBitmapInfo.bmiHeader.biHeight = -pRect.GetHeight();
		lBitmapInfo.bmiHeader.biCompression = 0;
		lBpp = lBitmapInfo.bmiHeader.biBitCount;
		int lBytePerPixel = (lBpp+7)/8;
		int lByteCount = pRect.GetWidth()*pRect.GetHeight()*lBytePerPixel;
		lBitmap = new Lepra::uint8[lByteCount];
		lOk = (::GetDIBits(lRamDc, lRamBitmap, 0, pRect.GetHeight(), lBitmap, &lBitmapInfo, DIB_RGB_COLORS) == pRect.GetHeight());
		::SelectObject(lRamDc, lDefaultBitmap);
	}
	if (lOk)
	{
		Lepra::Canvas::BitDepth lBitDepth = Lepra::Canvas::IntToBitDepth(lBpp);
		Lepra::Canvas lBitmapImage(pRect.GetWidth(), pRect.GetHeight(), lBitDepth);
		lBitmapImage.SetBuffer(lBitmap, false);
		lBitmapImage.ConvertBitDepth(pImage.GetBitDepth());
		for (int y = 0; y < pRect.GetHeight(); ++y)
		{
			for (int x = 0; x < pRect.GetWidth(); ++x)
			{
				Lepra::Color lColor;
				lBitmapImage.GetPixelColor(x, y, lColor);
				if (lColor.SumRgb() > 0)
				{
					lColor.mAlpha = 255;
				}
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
	return (lOk);
}
void GDIPainter::SetActiveFont(FontID pFontID)
{
	Painter::SetActiveFont(pFontID);

	if (GetCurrentFontInternal()->IsSystemFont())
	{
		::SetTextAlign(mDC, TA_TOP | TA_LEFT);
	}
}

int GDIPainter::GetCharWidth(const Lepra::tchar pChar)
{
	int lCharWidth = 0;
	
	if (GetCurrentFontInternal()->IsSystemFont())
	{
		SystemFont* lFont = (SystemFont*)GetCurrentFontInternal();
		HGDIOBJ lDefaultObject = ::SelectObject(mDC, lFont->mFont);

		ABC lABC;
		INT lWidth;
		if (::GetCharABCWidths(mDC, (Lepra::utchar)pChar, (Lepra::utchar)pChar, &lABC) != FALSE)
		{
			lCharWidth = (int)lABC.abcA + (int)lABC.abcB + (int)lABC.abcC;
		}
		else if (::GetCharWidth32(mDC, (Lepra::utchar)pChar, (Lepra::utchar)pChar, &lWidth) != FALSE)
		{
			lCharWidth = (int)lWidth;
		}

		::SelectObject(mDC, lDefaultObject);
	}
	else
	{
		ImageFont* lFont = (ImageFont*)GetCurrentFontInternal();
		Lepra::utchar lChar = (Lepra::utchar)pChar - (Lepra::utchar)lFont->mFirstChar;
		lCharWidth = lFont->mCharWidth[lChar];

		if (pChar == ' ')
		{
			return lCharWidth + lFont->mDefaultSpaceWidth + lFont->mCharOffset;
		}
		else if(pChar == '\t')
		{
			return lFont->mTabWidth;
		}

		lCharWidth += lFont->mCharOffset;
	}

	return lCharWidth;
}

int GDIPainter::GetStringWidth(const Lepra::String& pString)
{
	int lCurrentX = 0;
	int lMaxX = 0;

	for (size_t i = 0; i < pString.length(); i++)
	{
		Lepra::tchar lChar = pString[i];

		if (lChar == _T('\n'))
		{
			lCurrentX = 0;
		}
		else if(lChar != _T('\r') && 
			lChar != _T('\b') &&
			lChar != _T('\t'))
		{
			lCurrentX += GetCharWidth(lChar);
		}

		if (GetCurrentFontInternal()->IsSystemFont() == false)
		{
			ImageFont* lFont = (ImageFont*)GetCurrentFontInternal();
			if (lChar == _T(' '))
			{
				lCurrentX += lFont->mDefaultSpaceWidth;
			}
			else if(lChar == _T('\t'))
			{
				lCurrentX = ((lCurrentX / lFont->mTabWidth) + 1) * lFont->mTabWidth;
			}
		}

		if (lCurrentX > lMaxX)
		{
			lMaxX = lCurrentX;
		}
	}
	return lMaxX;
}

int GDIPainter::DoPrintText(const Lepra::String& pString, int x, int y)
{
	if (GetCurrentFontInternal()->IsSystemFont())
	{
		SystemFont* lFont = (SystemFont*)GetCurrentFontInternal();
		HGDIOBJ lDefaultObject = ::SelectObject(mDC, lFont->mFont);

		::SetTextColor(mDC, mColorRef[0]);
		::SetBkColor(mDC, mColorRef[1]);
		::TextOut(mDC, x, y, pString.c_str(), (int)pString.length());
		::SelectObject(mDC, lDefaultObject);

		return x;
	}

	ImageFont* lFont = (ImageFont*)GetCurrentFontInternal();
	int lCurrentX = x;
	int lCurrentY = y;

	for (size_t i = 0; i < pString.length(); i++)
	{
		Lepra::tchar lChar = pString[i];

		if (lChar == _T('\n'))
		{
			lCurrentY += (lFont->mCharHeight + lFont->mNewLineOffset);
			lCurrentX = x;
		}
		else if(lChar != _T('\r') && 
			lChar != _T('\b') &&
			lChar != _T('\t'))
		{
			if (lFont->mAlphaImage == true)
			{
				DoDrawAlphaImage((ImageID)lFont->mTextureID, lCurrentX, lCurrentY, lFont->mCharRect[(Lepra::utchar)lChar - lFont->mFirstChar]);
			}
			else
			{
				DoDrawImage((ImageID)lFont->mTextureID, lCurrentX, lCurrentY, lFont->mCharRect[(Lepra::utchar)lChar - lFont->mFirstChar]);
			}

			lCurrentX += lFont->mCharWidth[(Lepra::utchar)lChar - lFont->mFirstChar] + lFont->mCharOffset;
		}

		if (lChar == _T(' '))
		{
			lCurrentX += lFont->mDefaultSpaceWidth;
		}
		else if(lChar == _T('\t'))
		{
			lCurrentX = GetTabOriginX() + (((lCurrentX - GetTabOriginX()) / lFont->mTabWidth) + 1) * lFont->mTabWidth;
		}
	}

	return lCurrentX;
}

int GDIPainter::GetFontHeight()
{
	if (GetCurrentFontInternal()->IsSystemFont())
	{
		SystemFont* lFont = (SystemFont*)GetCurrentFontInternal();
		return (int)(lFont->mSize + 0.5);
	}
	else
	{
		ImageFont* lFont = (ImageFont*)GetCurrentFontInternal();
		return lFont->mCharHeight;
	}
}

int GDIPainter::GetLineHeight()
{
	if (GetCurrentFontInternal()->IsSystemFont())
	{
		// TODO: Check if this gives a correct result.
		SystemFont* lFont = (SystemFont*)GetCurrentFontInternal();
		return (int)(lFont->mSize + 0.5);
	}
	else
	{
		ImageFont* lFont = (ImageFont*)GetCurrentFontInternal();
		return lFont->mCharHeight + lFont->mNewLineOffset;
	}
}

void GDIPainter::ReadPixels(Lepra::Canvas& pDestCanvas, const Lepra::PixelRect& pRect)
{
	pDestCanvas.Reset(pRect.GetWidth(), pRect.GetHeight(), Lepra::Canvas::BITDEPTH_24_BIT);
	pDestCanvas.SetPalette(GetCanvas()->GetPalette());
	pDestCanvas.CreateBuffer();

	if (pDestCanvas.GetBitDepth() == Lepra::Canvas::BITDEPTH_8_BIT)
	{
		for (int y = pRect.mTop; y < pRect.mBottom; y++)
		{
			for (int x = pRect.mLeft; x < pRect.mRight; x++)
			{
				COLORREF lCol = ::GetPixel(mDC, x, y);
				Lepra::Color lColor(GetRValue(lCol), GetGValue(lCol), GetBValue(lCol), 0);

				Lepra::uint8 lIndex = UiLepra::DisplayManager::GetPaletteColor(GetRValue(lCol), GetGValue(lCol), GetBValue(lCol), pDestCanvas.GetPalette());
				lColor.mAlpha = (Lepra::uint8)lIndex;

				pDestCanvas.SetPixelColor(x, y, lColor);
			}
		}
	}
	else
	{
		for (int y = pRect.mTop; y < pRect.mBottom; y++)
		{
			for (int x = pRect.mLeft; x < pRect.mRight; x++)
			{
				COLORREF lCol = ::GetPixel(mDC, x, y);
				Lepra::Color lColor(GetRValue(lCol), GetGValue(lCol), GetBValue(lCol), 0);
				pDestCanvas.SetPixelColor(x, y, lColor);
			}
		}
	}
}

Painter::RGBOrder GDIPainter::GetRGBOrder()
{
	return BGR;
}

void GDIPainter::DoRenderDisplayList(std::vector<DisplayEntity*>* pDisplayList)
{
	PushAttrib(ATTR_ALL);

	std::vector<DisplayEntity*>::iterator it;
	for(it = pDisplayList->begin(); it != pDisplayList->end(); ++it)
	{
		DisplayEntity* lSE = *it;
		Painter::SetClippingRect(lSE->GetClippingRect());
		SetAlphaValue(lSE->GetAlpha());
		SetRenderMode(lSE->GetRenderMode());

		const Lepra::Vector2DF& lPos = lSE->GetGeometry().GetPos();
		float* lVertex = (float*)lSE->GetGeometry().GetVertexData();
		float* lColor = lSE->GetGeometry().GetColorData();
		float* lUV = lSE->GetGeometry().GetUVData();
		Lepra::uint32* lTriangle = lSE->GetGeometry().GetTriangleData();
		int lTriangleMax = lSE->GetGeometry().GetTriangleCount() * 3;

		for(int i = 0; i < lTriangleMax; i += 3)
		{
			int lV1 = lTriangle[i + 0];
			int lV2 = lTriangle[i + 1];
			int lV3 = lTriangle[i + 2];

			if(lSE->GetImageID() == INVALID_IMAGEID)
			{
				if(lColor != 0)
				{
					int lC1 = lV1 * 3;
					int lC2 = lV2 * 3;
					int lC3 = lV3 * 3;
					Painter::SetColor((Lepra::uint8)(lColor[lC1 + 0] * 255.0f),
							  (Lepra::uint8)(lColor[lC1 + 1] * 255.0f),
							  (Lepra::uint8)(lColor[lC1 + 2] * 255.0f),
							  255, 0);
					Painter::SetColor((Lepra::uint8)(lColor[lC2 + 0] * 255.0f),
							  (Lepra::uint8)(lColor[lC2 + 1] * 255.0f),
							  (Lepra::uint8)(lColor[lC2 + 2] * 255.0f),
							  255, 1);
					Painter::SetColor((Lepra::uint8)(lColor[lC3 + 0] * 255.0f),
							  (Lepra::uint8)(lColor[lC3 + 1] * 255.0f),
							  (Lepra::uint8)(lColor[lC3 + 2] * 255.0f),
							  255, 2);
					lV1 *= 2;
					lV2 *= 2;
					lV3 *= 2;
					FillShadedTriangle(lVertex[lV1 + 0] + lPos.x, lVertex[lV1 + 1] + lPos.y,
					                   lVertex[lV2 + 0] + lPos.x, lVertex[lV2 + 1] + lPos.y,
					                   lVertex[lV3 + 0] + lPos.x, lVertex[lV3 + 1] + lPos.y);
				}
				else
				{
					lV1 *= 2;
					lV2 *= 2;
					lV3 *= 2;
					FillTriangle(lVertex[lV1 + 0] + lPos.x, lVertex[lV1 + 1] + lPos.y,
					             lVertex[lV2 + 0] + lPos.x, lVertex[lV2 + 1] + lPos.y,
					             lVertex[lV3 + 0] + lPos.x, lVertex[lV3 + 1] + lPos.y);
				}
			}
			else
			{
				lV1 *= 2;
				lV2 *= 2;
				lV3 *= 2;
				FillTriangle(lVertex[lV1 + 0] + lPos.x, lVertex[lV1 + 1] + lPos.y, lUV[lV1 + 0], lUV[lV1 + 1],
					     lVertex[lV2 + 0] + lPos.x, lVertex[lV2 + 1] + lPos.y, lUV[lV2 + 0], lUV[lV2 + 1],
					     lVertex[lV3 + 0] + lPos.x, lVertex[lV3 + 1] + lPos.y, lUV[lV3 + 0], lUV[lV3 + 1],
					     lSE->GetImageID());
			}
		}
	}

	PopAttrib();
}

} // End namespace.
