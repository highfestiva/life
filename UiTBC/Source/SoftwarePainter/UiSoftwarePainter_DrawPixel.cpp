/*
	Class:  SoftwarePainter
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand
*/

#include "../../Include/UiSoftwarePainter.h"

namespace UiTbc
{

void SoftwarePainter::DrawPixel8BitNormal(int x, int y, SoftwarePainter* pThis)
{
	Lepra::Canvas* lCanvas = pThis->GetCanvas();
	((Lepra::uint8*)lCanvas->GetBuffer())[y * lCanvas->GetPitch() + x] = pThis->mColor8[0];
}

void SoftwarePainter::DrawPixel16BitNormal(int x, int y, SoftwarePainter* pThis)
{
	Lepra::Canvas* lCanvas = pThis->GetCanvas();
	((Lepra::uint16*)lCanvas->GetBuffer())[y * lCanvas->GetPitch() + x] = pThis->mColor16[0];
}

void SoftwarePainter::DrawPixel24BitNormal(int x, int y, SoftwarePainter* pThis)
{
	Lepra::Canvas* lCanvas = pThis->GetCanvas();
	Lepra::uint8* lBuffer = (Lepra::uint8*)lCanvas->GetBuffer();
	int lOffset = (y * lCanvas->GetPitch() + x) * 3;
	Lepra::Color& lColor = pThis->GetColorInternal(0);

	lBuffer[lOffset + 0] = lColor.mBlue;
	lBuffer[lOffset + 1] = lColor.mGreen;
	lBuffer[lOffset + 2] = lColor.mRed;
}

void SoftwarePainter::DrawPixel32BitNormal(int x, int y, SoftwarePainter* pThis)
{
	Lepra::Canvas* lCanvas = pThis->GetCanvas();

	if (pThis->mIncrementalAlpha == true)
	{
		Lepra::Color& lColor = pThis->GetColorInternal(0);
		Lepra::uint8* lBuffer = &((Lepra::uint8*)lCanvas->GetBuffer())[(y * lCanvas->GetPitch() + x) * 4];
		lBuffer[0] = lColor.mBlue;
		lBuffer[1] = lColor.mGreen;
		lBuffer[2] = lColor.mRed;

		if (lColor.mAlpha > lBuffer[3])
		{
			lBuffer[3] = lColor.mAlpha;
		}
	}
	else
	{
		((unsigned*)lCanvas->GetBuffer())[y * lCanvas->GetPitch() + x] = pThis->mColor32[0];
	}
}

void SoftwarePainter::DrawPixel15BitBlend(int x, int y, SoftwarePainter* pThis)
{
	Lepra::Canvas* lCanvas = pThis->GetCanvas();
	Lepra::uint16* lBuffer = (Lepra::uint16*)lCanvas->GetBuffer();
	lBuffer += y * lCanvas->GetPitch() + x;

	Lepra::Color& lColor = pThis->GetColorInternal(0);
	Lepra::uint16 lSR = lColor.mRed;
	Lepra::uint16 lSG = lColor.mGreen;
	Lepra::uint16 lSB = lColor.mBlue;
	Lepra::uint16 lSA = (Lepra::uint16)pThis->GetAlphaValue();

	Lepra::uint16 lDColor = *lBuffer;
	Lepra::uint16 lDR = ((lDColor >> 10) & 31) << 3;
	Lepra::uint16 lDG = ((lDColor >> 5) & 31) << 3;
	Lepra::uint16 lDB = (lDColor & 31) << 3;

	lDR = ((lSR * lSA) >> 8) + ((lDR * (0xFF - lSA)) >> 8);
	lDG = ((lSG * lSA) >> 8) + ((lDG * (0xFF - lSA)) >> 8);
	lDB = ((lSB * lSA) >> 8) + ((lDB * (0xFF - lSA)) >> 8);

	*lBuffer = ((lDR >> 3) << 10) | ((lDG >> 3) << 5) | (lDB >> 3);
}

void SoftwarePainter::DrawPixel16BitBlend(int x, int y, SoftwarePainter* pThis)
{
	Lepra::Canvas* lCanvas = pThis->GetCanvas();
	Lepra::uint16* lBuffer = (Lepra::uint16*)lCanvas->GetBuffer();
	lBuffer += y * lCanvas->GetPitch() + x;

	Lepra::Color& lColor = pThis->GetColorInternal(0);
	Lepra::uint16 lSR = lColor.mRed;
	Lepra::uint16 lSG = lColor.mGreen;
	Lepra::uint16 lSB = lColor.mBlue;
	Lepra::uint16 lSA = (Lepra::uint16)pThis->GetAlphaValue();

	Lepra::uint16 lDColor = *lBuffer;
	Lepra::uint16 lDR = ((lDColor >> 11) & 31) << 3;
	Lepra::uint16 lDG = ((lDColor >> 5) & 63) << 2;
	Lepra::uint16 lDB = (lDColor & 31) << 3;

	lDR = ((lSR * lSA) >> 8) + ((lDR * (0xFF - lSA)) >> 8);
	lDG = ((lSG * lSA) >> 8) + ((lDG * (0xFF - lSA)) >> 8);
	lDB = ((lSB * lSA) >> 8) + ((lDB * (0xFF - lSA)) >> 8);

	*lBuffer = ((lDR >> 3) << 11) | ((lDG >> 2) << 5) | (lDB >> 3);
}

void SoftwarePainter::DrawPixel24BitBlend(int x, int y, SoftwarePainter* pThis)
{
	Lepra::Canvas* lCanvas = pThis->GetCanvas();
	Lepra::uint8* lBuffer = (Lepra::uint8*)lCanvas->GetBuffer();
	lBuffer += (y * lCanvas->GetPitch() + x) * 3;

	Lepra::Color& lColor = pThis->GetColorInternal(0);
	Lepra::uint16 lSR = lColor.mRed;
	Lepra::uint16 lSG = lColor.mGreen;
	Lepra::uint16 lSB = lColor.mBlue;
	Lepra::uint16 lSA = (Lepra::uint16)pThis->GetAlphaValue();

	Lepra::uint16 lDB = lBuffer[0];
	Lepra::uint16 lDG = lBuffer[1];
	Lepra::uint16 lDR = lBuffer[2];

	lBuffer[0] = (Lepra::uint8)(((lSB * lSA) >> 8) + ((lDB * (0xFF - lSA)) >> 8));
	lBuffer[1] = (Lepra::uint8)(((lSG * lSA) >> 8) + ((lDG * (0xFF - lSA)) >> 8));
	lBuffer[2] = (Lepra::uint8)(((lSR * lSA) >> 8) + ((lDR * (0xFF - lSA)) >> 8));
}

void SoftwarePainter::DrawPixel32BitBlend(int x, int y, SoftwarePainter* pThis)
{
	Lepra::Canvas* lCanvas = pThis->GetCanvas();
	Lepra::uint8* lBuffer = (Lepra::uint8*)lCanvas->GetBuffer();
	lBuffer += (y * lCanvas->GetPitch() + x) * 4;

	Lepra::Color& lColor = pThis->GetColorInternal(0);
	Lepra::uint16 lSR = lColor.mRed;
	Lepra::uint16 lSG = lColor.mGreen;
	Lepra::uint16 lSB = lColor.mBlue;
	Lepra::uint16 lSA = (Lepra::uint16)pThis->GetAlphaValue();

	Lepra::uint16 lDB = lBuffer[0];
	Lepra::uint16 lDG = lBuffer[1];
	Lepra::uint16 lDR = lBuffer[2];

	lBuffer[0] = (Lepra::uint8)(((lSB * lSA) >> 8) + ((lDB * (0xFF - lSA)) >> 8));
	lBuffer[1] = (Lepra::uint8)(((lSG * lSA) >> 8) + ((lDG * (0xFF - lSA)) >> 8));
	lBuffer[2] = (Lepra::uint8)(((lSR * lSA) >> 8) + ((lDR * (0xFF - lSA)) >> 8));

	if (pThis->mIncrementalAlpha == true && lSA > lBuffer[3])
	{
		lBuffer[3] = (Lepra::uint8)lSA;
	}
}

void SoftwarePainter::DrawPixel8BitXor(int x, int y, SoftwarePainter* pThis)
{
	Lepra::Canvas* lCanvas = pThis->GetCanvas();
	((Lepra::uint8*)lCanvas->GetBuffer())[y * lCanvas->GetPitch() + x] ^= pThis->mColor8[0];
}

void SoftwarePainter::DrawPixel16BitXor(int x, int y, SoftwarePainter* pThis)
{
	Lepra::Canvas* lCanvas = pThis->GetCanvas();
	((Lepra::uint16*)lCanvas->GetBuffer())[y * lCanvas->GetPitch() + x] ^= pThis->mColor16[0];
}

void SoftwarePainter::DrawPixel24BitXor(int x, int y, SoftwarePainter* pThis)
{
	Lepra::Canvas* lCanvas = pThis->GetCanvas();
	Lepra::uint8* lBuffer = (Lepra::uint8*)lCanvas->GetBuffer();
	lBuffer += (y * lCanvas->GetPitch() + x) * 3;
	Lepra::Color& lColor = pThis->GetColorInternal(0);

	lBuffer[0] ^= lColor.mBlue;
	lBuffer[1] ^= lColor.mGreen;
	lBuffer[2] ^= lColor.mRed;
}

void SoftwarePainter::DrawPixel32BitXor(int x, int y, SoftwarePainter* pThis)
{
	Lepra::Canvas* lCanvas = pThis->GetCanvas();

	if (pThis->mIncrementalAlpha == true)
	{
		Lepra::Color& lColor = pThis->GetColorInternal(0);
		Lepra::uint8* lBuffer = &((Lepra::uint8*)lCanvas->GetBuffer())[(y * lCanvas->GetPitch() + x) * 4];
		lBuffer[0] ^= lColor.mBlue;
		lBuffer[1] ^= lColor.mGreen;
		lBuffer[2] ^= lColor.mRed;

		if (lColor.mAlpha > lBuffer[3])
		{
			lBuffer[3] = lColor.mAlpha;
		}
	}
	else
	{
		((unsigned*)lCanvas->GetBuffer())[y * lCanvas->GetPitch() + x] ^= pThis->mColor32[0];
	}
}

void SoftwarePainter::DrawPixel8BitAdd(int x, int y, SoftwarePainter* pThis)
{
	Lepra::Canvas* lCanvas = pThis->GetCanvas();
	Lepra::uint8* lBuffer = (Lepra::uint8*)lCanvas->GetBuffer();
	lBuffer += y * lCanvas->GetPitch() + x;

	Lepra::uint16 lColor = (Lepra::uint16)*lBuffer + (Lepra::uint16)pThis->mColor8[0];

	*lBuffer = lColor > 255 ? 255 : (Lepra::uint8)lColor;
}

void SoftwarePainter::DrawPixel15BitAdd(int x, int y, SoftwarePainter* pThis)
{
	Lepra::Canvas* lCanvas = pThis->GetCanvas();
	Lepra::uint16* lBuffer = (Lepra::uint16*)lCanvas->GetBuffer();
	lBuffer += y * lCanvas->GetPitch() + x;

	Lepra::Color& lColor = pThis->GetColorInternal(0);
	Lepra::uint16 lSR = (Lepra::uint16)(lColor.mRed >> 3);
	Lepra::uint16 lSG = (Lepra::uint16)(lColor.mGreen >> 3);
	Lepra::uint16 lSB = (Lepra::uint16)(lColor.mBlue >> 3);

	Lepra::uint16 lColor16 = *lBuffer;
	Lepra::uint16 lR = ((lColor16 >> 10) & 31) + lSR;
	Lepra::uint16 lG = ((lColor16 >> 5) & 31) + lSG;
	Lepra::uint16 lB = (lColor16 & 31) + lSB;

	lR = lR > 31 ? 31 : lR;
	lG = lG > 31 ? 31 : lG;
	lB = lB > 31 ? 31 : lB;

	*((Lepra::uint16*)lBuffer) = (lR << 10) | (lG << 5) | lB;
}

void SoftwarePainter::DrawPixel16BitAdd(int x, int y, SoftwarePainter* pThis)
{
	Lepra::Canvas* lCanvas = pThis->GetCanvas();
	Lepra::uint16* lBuffer = (Lepra::uint16*)lCanvas->GetBuffer();
	lBuffer += y * lCanvas->GetPitch() + x;

	Lepra::Color& lColor = pThis->GetColorInternal(0);
	Lepra::uint16 lSR = (Lepra::uint16)(lColor.mRed >> 3);
	Lepra::uint16 lSG = (Lepra::uint16)(lColor.mGreen >> 2);
	Lepra::uint16 lSB = (Lepra::uint16)(lColor.mBlue >> 3);

	Lepra::uint16 lColor16 = *lBuffer;
	Lepra::uint16 lR = ((lColor16 >> 11) & 31) + lSR;
	Lepra::uint16 lG = ((lColor16 >> 5) & 63) + lSG;
	Lepra::uint16 lB = (lColor16 & 31) + lSB;

	lR = lR > 31 ? 31 : lR;
	lG = lG > 63 ? 63 : lG;
	lB = lB > 31 ? 31 : lB;

	*((Lepra::uint16*)lBuffer) = (lR << 11) | (lG << 5) | lB;
}

void SoftwarePainter::DrawPixel24BitAdd(int x, int y, SoftwarePainter* pThis)
{
	Lepra::Canvas* lCanvas = pThis->GetCanvas();
	Lepra::uint8* lBuffer = (Lepra::uint8*)lCanvas->GetBuffer();
	lBuffer += (y * lCanvas->GetPitch() + x) * 3;

	Lepra::Color& lColor = pThis->GetColorInternal(0);
	Lepra::uint16 lSR = lColor.mRed;
	Lepra::uint16 lSG = lColor.mGreen;
	Lepra::uint16 lSB = lColor.mBlue;

	int lDB = lBuffer[0];
	int lDG = lBuffer[1];
	int lDR = lBuffer[2];

	lDR += lSR;
	lDG += lSG;
	lDB += lSB;

	lBuffer[0] = lDB > 255 ? 255 : (Lepra::uint8)lDB;
	lBuffer[1] = lDG > 255 ? 255 : (Lepra::uint8)lDG;
	lBuffer[2] = lDR > 255 ? 255 : (Lepra::uint8)lDR;
}

void SoftwarePainter::DrawPixel32BitAdd(int x, int y, SoftwarePainter* pThis)
{
	Lepra::Canvas* lCanvas = pThis->GetCanvas();
	Lepra::uint8* lBuffer = (Lepra::uint8*)lCanvas->GetBuffer();
	lBuffer += (y * lCanvas->GetPitch() + x) * 4;

	Lepra::Color& lColor = pThis->GetColorInternal(0);
	Lepra::uint16 lSR = lColor.mRed;
	Lepra::uint16 lSG = lColor.mGreen;
	Lepra::uint16 lSB = lColor.mBlue;

	int lDB = lBuffer[0];
	int lDG = lBuffer[1];
	int lDR = lBuffer[2];

	lDR += lSR;
	lDG += lSG;
	lDB += lSB;

	lBuffer[0] = lDB > 255 ? 255 : (Lepra::uint8)lDB;
	lBuffer[1] = lDG > 255 ? 255 : (Lepra::uint8)lDG;
	lBuffer[2] = lDR > 255 ? 255 : (Lepra::uint8)lDR;
	
	if (pThis->mIncrementalAlpha == true && lColor.mAlpha > lBuffer[3])
	{
		lBuffer[3] = lColor.mAlpha;
	}
}

} // End namespace.
