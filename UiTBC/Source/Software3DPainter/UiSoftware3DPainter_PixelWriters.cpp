/*
	Lepra::File:   Software3DPainter_PixelWriters.cpp
	Class:  Software3DPainter
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand
*/

#include "../../Include/UiTBC.h"
#include "../../Include/UiSoftware3DPainter.h"

namespace UiTbc
{

void Software3DPainter::Write8BitColorStandard(unsigned char* pDest, unsigned char pColor, unsigned char pAlpha)
{
	pAlpha;
	*pDest = pColor;
}

void Software3DPainter::Write15BitColorStandard(void* pDest, unsigned char pRed, unsigned char pGreen, unsigned char pBlue, unsigned char pAlpha, Software3DPainter* pThis)
{
	if (pThis->mHaveCallBack == true)
	{
		pThis->PixelShaderCallBack(pRed, pGreen, pBlue, pThis);
	}

	pAlpha;
	*((unsigned short*)pDest) = (((unsigned short)pRed >> 3) << 10) | (((unsigned short)pGreen >> 3) << 5) | ((unsigned short)pBlue >> 3);
}

void Software3DPainter::Write16BitColorStandard(void* pDest, unsigned char pRed, unsigned char pGreen, unsigned char pBlue, unsigned char pAlpha, Software3DPainter* pThis)
{
	if (pThis->mHaveCallBack == true)
	{
		pThis->PixelShaderCallBack(pRed, pGreen, pBlue, pThis);
	}

	pAlpha;
	*((unsigned short*)pDest) = (((unsigned short)pRed >> 3) << 11) | (((unsigned short)pGreen >> 2) << 5) | ((unsigned short)pBlue >> 3);
}

void Software3DPainter::Write24BitColorStandard(void* pDest, unsigned char pRed, unsigned char pGreen, unsigned char pBlue, unsigned char pAlpha, Software3DPainter* pThis)
{
	if (pThis->mHaveCallBack == true)
	{
		pThis->PixelShaderCallBack(pRed, pGreen, pBlue, pThis);
	}

	pAlpha;
	((unsigned char*)pDest)[0] = pBlue;
	((unsigned char*)pDest)[1] = pGreen;
	((unsigned char*)pDest)[2] = pRed;
}

void Software3DPainter::Write32BitColorStandard(void* pDest, unsigned char pRed, unsigned char pGreen, unsigned char pBlue, unsigned char pAlpha, Software3DPainter* pThis)
{
	if (pThis->mHaveCallBack == true)
	{
		pThis->PixelShaderCallBack(pRed, pGreen, pBlue, pThis);
	}

	pAlpha;
	*((unsigned int*)pDest) = ((unsigned int)pRed << 16) | ((unsigned int)pGreen << 8) | (unsigned int)pBlue;
}

void Software3DPainter::Write8BitColorAlphaTest(unsigned char* pDest, unsigned char pColor, unsigned char pAlpha)
{
	if (pAlpha != 0)
	{
		*pDest = pColor;
	}
}

void Software3DPainter::Write15BitColorAlphaTest(void* pDest, unsigned char pRed, unsigned char pGreen, unsigned char pBlue, unsigned char pAlpha, Software3DPainter* pThis)
{
	if (pAlpha != 0)
	{
		if (pThis->mHaveCallBack == true)
		{
			pThis->PixelShaderCallBack(pRed, pGreen, pBlue, pThis);
		}
		*((unsigned short*)pDest) = (((unsigned short)pRed >> 3) << 10) | (((unsigned short)pGreen >> 3) << 5) | ((unsigned short)pBlue >> 3);
	}
}

void Software3DPainter::Write16BitColorAlphaTest(void* pDest, unsigned char pRed, unsigned char pGreen, unsigned char pBlue, unsigned char pAlpha, Software3DPainter* pThis)
{
	if (pAlpha != 0)
	{
		if (pThis->mHaveCallBack == true)
		{
			pThis->PixelShaderCallBack(pRed, pGreen, pBlue, pThis);
		}
		*((unsigned short*)pDest) = (((unsigned short)pRed >> 3) << 11) | (((unsigned short)pGreen >> 2) << 5) | ((unsigned short)pBlue >> 3);
	}
}

void Software3DPainter::Write24BitColorAlphaTest(void* pDest, unsigned char pRed, unsigned char pGreen, unsigned char pBlue, unsigned char pAlpha, Software3DPainter* pThis)
{
	if (pAlpha != 0)
	{
		if (pThis->mHaveCallBack == true)
		{
			pThis->PixelShaderCallBack(pRed, pGreen, pBlue, pThis);
		}
		((unsigned char*)pDest)[0] = pBlue;
		((unsigned char*)pDest)[1] = pGreen;
		((unsigned char*)pDest)[2] = pRed;
	}
}

void Software3DPainter::Write32BitColorAlphaTest(void* pDest, unsigned char pRed, unsigned char pGreen, unsigned char pBlue, unsigned char pAlpha, Software3DPainter* pThis)
{
	if (pAlpha != 0)
	{
		if (pThis->mHaveCallBack == true)
		{
			pThis->PixelShaderCallBack(pRed, pGreen, pBlue, pThis);
		}
		*((unsigned int*)pDest) = ((unsigned int)pRed << 16) | ((unsigned int)pGreen << 8) | (unsigned int)pBlue;
	}
}

void Software3DPainter::Write15BitColorAlphaBlend(void* pDest, unsigned char pRed, unsigned char pGreen, unsigned char pBlue, unsigned char pAlpha, Software3DPainter* pThis)
{
	if (pThis->mHaveCallBack == true)
	{
		pThis->PixelShaderCallBack(pRed, pGreen, pBlue, pThis);
	}

	unsigned short lColor = *((unsigned short*)pDest);
	unsigned short lR = ((lColor >> 10) & 31) << 3;
	unsigned short lG = ((lColor >> 5) & 31) << 3;
	unsigned short lB = (lColor & 31) << 3;

	lR = (((unsigned short)pRed   * (unsigned short)pAlpha) >> 8) + ((lR * (0xFF - (unsigned short)pAlpha)) >> 8);
	lG = (((unsigned short)pGreen * (unsigned short)pAlpha) >> 8) + ((lG * (0xFF - (unsigned short)pAlpha)) >> 8);
	lB = (((unsigned short)pBlue  * (unsigned short)pAlpha) >> 8) + ((lB * (0xFF - (unsigned short)pAlpha)) >> 8);

	*((unsigned short*)pDest) = ((lR >> 3) << 10) | ((lG >> 3) << 5) | (lB >> 3);
}

void Software3DPainter::Write16BitColorAlphaBlend(void* pDest, unsigned char pRed, unsigned char pGreen, unsigned char pBlue, unsigned char pAlpha, Software3DPainter* pThis)
{
	if (pThis->mHaveCallBack == true)
	{
		pThis->PixelShaderCallBack(pRed, pGreen, pBlue, pThis);
	}

	unsigned short lColor = *((unsigned short*)pDest);
	unsigned short lR = ((lColor >> 11) & 31) << 3;
	unsigned short lG = ((lColor >> 5) & 63) << 2;
	unsigned short lB = (lColor & 31) << 3;

	lR = (((unsigned short)pRed   * (unsigned short)pAlpha) >> 8) + ((lR * (0xFF - (unsigned short)pAlpha)) >> 8);
	lG = (((unsigned short)pGreen * (unsigned short)pAlpha) >> 8) + ((lG * (0xFF - (unsigned short)pAlpha)) >> 8);
	lB = (((unsigned short)pBlue  * (unsigned short)pAlpha) >> 8) + ((lB * (0xFF - (unsigned short)pAlpha)) >> 8);

	*((unsigned short*)pDest) = ((lR >> 3) << 11) | ((lG >> 2) << 5) | (lB >> 3);
}

void Software3DPainter::Write24BitColorAlphaBlend(void* pDest, unsigned char pRed, unsigned char pGreen, unsigned char pBlue, unsigned char pAlpha, Software3DPainter* pThis)
{
	if (pThis->mHaveCallBack == true)
	{
		pThis->PixelShaderCallBack(pRed, pGreen, pBlue, pThis);
	}

	unsigned short lB = ((unsigned char*)pDest)[0];
	unsigned short lG = ((unsigned char*)pDest)[1];
	unsigned short lR = ((unsigned char*)pDest)[2];

	lR = (((unsigned short)pRed   * (unsigned short)pAlpha) >> 8) + ((lR * (0xFF - (unsigned short)pAlpha)) >> 8);
	lG = (((unsigned short)pGreen * (unsigned short)pAlpha) >> 8) + ((lG * (0xFF - (unsigned short)pAlpha)) >> 8);
	lB = (((unsigned short)pBlue  * (unsigned short)pAlpha) >> 8) + ((lB * (0xFF - (unsigned short)pAlpha)) >> 8);

	((unsigned char*)pDest)[0] = (unsigned char)lB;
	((unsigned char*)pDest)[1] = (unsigned char)lG;
	((unsigned char*)pDest)[2] = (unsigned char)lR;
}

void Software3DPainter::Write32BitColorAlphaBlend(void* pDest, unsigned char pRed, unsigned char pGreen, unsigned char pBlue, unsigned char pAlpha, Software3DPainter* pThis)
{
	if (pThis->mHaveCallBack == true)
	{
		pThis->PixelShaderCallBack(pRed, pGreen, pBlue, pThis);
	}

	unsigned short lB = ((unsigned char*)pDest)[0];
	unsigned short lG = ((unsigned char*)pDest)[1];
	unsigned short lR = ((unsigned char*)pDest)[2];

	lR = (((unsigned short)pRed   * (unsigned short)pAlpha) >> 8) + ((lR * (0xFF - (unsigned short)pAlpha)) >> 8);
	lG = (((unsigned short)pGreen * (unsigned short)pAlpha) >> 8) + ((lG * (0xFF - (unsigned short)pAlpha)) >> 8);
	lB = (((unsigned short)pBlue  * (unsigned short)pAlpha) >> 8) + ((lB * (0xFF - (unsigned short)pAlpha)) >> 8);

	((unsigned char*)pDest)[0] = (unsigned char)lB;
	((unsigned char*)pDest)[1] = (unsigned char)lG;
	((unsigned char*)pDest)[2] = (unsigned char)lR;
}

} // End namespace.
