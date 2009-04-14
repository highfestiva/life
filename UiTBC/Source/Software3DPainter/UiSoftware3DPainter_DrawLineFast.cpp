/*
	Lepra::File:   Software3DPainter_DrawLineFast.cpp
	Class:  Software3DPainter
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand
*/

#include "../../Include/UiTBC.h"
#include "../../Include/UiSoftware3DPainter.h"
#include "../../Include/UiVertex.h"
#include "../../../Lepra/Include/FixedPointMath.h"

namespace UiTbc
{

void Software3DPainter::DrawSingleColorLine8BitFast(const Vertex3D* pVertices, const Lepra::Color& pColor, Software3DPainter* pThis)
{
	int lScreenStep = (int)pThis->mScreen->GetPixelByteSize();
	int lScreenStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;
		lScreenStep = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned char* lScreen = (unsigned char*)pThis->mScreen->GetBuffer() +
							   lStartY * pThis->mScreen->GetPitch() + lStartX;

	for (int i = 0; i <= lLength; i++)
	{
		*lScreen = pColor.mAlpha;

		lScreen += lScreenStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
		}
	}
}

void Software3DPainter::DrawSingleColorLine15BitFast(const Vertex3D* pVertices, const Lepra::Color& pColor, Software3DPainter* pThis)
{
	int lScreenStep = 1;
	int lScreenStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;
		lScreenStep = pThis->mScreen->GetPitch();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned short* lScreen = (unsigned short*)pThis->mScreen->GetBuffer() +
							   lStartY * pThis->mScreen->GetPitch() + lStartX;

	unsigned short lColor = ((pColor.mRed >> 3) << 10) | 
								((pColor.mGreen >> 3) << 5) | 
								(pColor.mBlue >> 3);
	for (int i = 0; i <= lLength; i++)
	{
		*lScreen = lColor;

		lScreen += lScreenStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
		}
	}
}

void Software3DPainter::DrawSingleColorLine16BitFast(const Vertex3D* pVertices, const Lepra::Color& pColor, Software3DPainter* pThis)
{
	int lScreenStep = 1;
	int lScreenStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;
		lScreenStep = pThis->mScreen->GetPitch();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned short* lScreen = (unsigned short*)pThis->mScreen->GetBuffer() +
							   lStartY * pThis->mScreen->GetPitch() + lStartX;

	unsigned short lColor = ((pColor.mRed >> 3) << 11) | 
								((pColor.mGreen >> 2) << 5) | 
								(pColor.mBlue >> 3);
	for (int i = 0; i <= lLength; i++)
	{
		*lScreen = lColor;

		lScreen += lScreenStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
		}
	}
}

void Software3DPainter::DrawSingleColorLine32BitFast(const Vertex3D* pVertices, const Lepra::Color& pColor, Software3DPainter* pThis)
{
	int lScreenStep = (int)pThis->mScreen->GetPixelByteSize();
	int lScreenStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;
		lScreenStep = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned char* lScreen = (unsigned char*)pThis->mScreen->GetBuffer() +
							   (lStartY * pThis->mScreen->GetPitch() + lStartX) * pThis->mScreen->GetPixelByteSize();

	for (int i = 0; i <= lLength; i++)
	{
		lScreen[0] = pColor.mBlue;
		lScreen[1] = pColor.mGreen;
		lScreen[2] = pColor.mRed;

		lScreen += lScreenStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
		}
	}
}

void Software3DPainter::DrawSingleColorLine8BitFastZT(const Vertex3D* pVertices, const Lepra::Color& pColor, Software3DPainter* pThis)
{
	int lScreenStep = (int)pThis->mScreen->GetPixelByteSize();
	int lScreenStepExtra;
	int lZBufStep = 1;
	int lZBufStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
		lZBufStep = -lZBufStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();
		lZBufStepExtra = pThis->mZBuffer->GetPitch();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;
		lZBufStepExtra = lZBufStep;

		lScreenStep = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();
		lZBufStep = pThis->mZBuffer->GetPitch();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned char* lScreen = (unsigned char*)pThis->mScreen->GetBuffer() +
							   lStartY * pThis->mScreen->GetPitch() + lStartX;
	float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
						(lStartY - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lStartX - pThis->mZBufferX;

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();

	double lOneOverLength = 1.0f / (double)(lLength + 1);
	double lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;

	for (int i = 0; i <= lLength; i++)
	{
		if (lOneOverZ > *lZBuffer)
		{
			*lScreen = pColor.mAlpha;
		}

		lOneOverZ += (float)lOneOverZStep;

		lScreen += lScreenStep;
		lZBuffer += lZBufStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
			lZBuffer += lZBufStepExtra;
		}
	}
}

void Software3DPainter::DrawSingleColorLine15BitFastZT(const Vertex3D* pVertices, const Lepra::Color& pColor, Software3DPainter* pThis)
{
	int lScreenStep = 1;
	int lScreenStepExtra;
	int lZBufStep = 1;
	int lZBufStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
		lZBufStep = -lZBufStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch();
		lZBufStepExtra = pThis->mZBuffer->GetPitch();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;
		lZBufStepExtra = lZBufStep;

		lScreenStep = pThis->mScreen->GetPitch();
		lZBufStep = pThis->mZBuffer->GetPitch();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned short* lScreen = (unsigned short*)pThis->mScreen->GetBuffer() +
							   lStartY * pThis->mScreen->GetPitch() + lStartX;
	float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
						(lStartY - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lStartX - pThis->mZBufferX;

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();

	double lOneOverLength = 1.0f / (double)(lLength + 1);
	double lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;

	unsigned short lColor = ((pColor.mRed >> 3) << 10) | 
								((pColor.mGreen >> 3) << 5) | 
								(pColor.mBlue >> 3);

	for (int i = 0; i <= lLength; i++)
	{
		if (lOneOverZ > *lZBuffer)
		{
			*lScreen = lColor;
		}

		lOneOverZ += (float)lOneOverZStep;

		lScreen += lScreenStep;
		lZBuffer += lZBufStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
			lZBuffer += lZBufStepExtra;
		}
	}
}

void Software3DPainter::DrawSingleColorLine16BitFastZT(const Vertex3D* pVertices, const Lepra::Color& pColor, Software3DPainter* pThis)
{
	int lScreenStep = 1;
	int lScreenStepExtra;
	int lZBufStep = 1;
	int lZBufStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
		lZBufStep = -lZBufStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch();
		lZBufStepExtra = pThis->mZBuffer->GetPitch();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;
		lZBufStepExtra = lZBufStep;

		lScreenStep = pThis->mScreen->GetPitch();
		lZBufStep = pThis->mZBuffer->GetPitch();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned short* lScreen = (unsigned short*)pThis->mScreen->GetBuffer() +
							   lStartY * pThis->mScreen->GetPitch() + lStartX;
	float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
						(lStartY - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lStartX - pThis->mZBufferX;

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();

	double lOneOverLength = 1.0f / (double)(lLength + 1);
	double lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;

	unsigned short lColor = ((pColor.mRed >> 3) << 11) | 
								((pColor.mGreen >> 2) << 5) | 
								(pColor.mBlue >> 3);

	for (int i = 0; i <= lLength; i++)
	{
		if (lOneOverZ > *lZBuffer)
		{
			*lScreen = lColor;
		}

		lOneOverZ += (float)lOneOverZStep;

		lScreen += lScreenStep;
		lZBuffer += lZBufStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
			lZBuffer += lZBufStepExtra;
		}
	}
}

void Software3DPainter::DrawSingleColorLine32BitFastZT(const Vertex3D* pVertices, const Lepra::Color& pColor, Software3DPainter* pThis)
{
	int lScreenStep = (int)pThis->mScreen->GetPixelByteSize();
	int lScreenStepExtra;
	int lZBufStep = 1;
	int lZBufStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
		lZBufStep = -lZBufStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();
		lZBufStepExtra = pThis->mZBuffer->GetPitch();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;
		lZBufStepExtra = lZBufStep;

		lScreenStep = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();
		lZBufStep = pThis->mZBuffer->GetPitch();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned char* lScreen = (unsigned char*)pThis->mScreen->GetBuffer() +
							   (lStartY * pThis->mScreen->GetPitch() + lStartX) * pThis->mScreen->GetPixelByteSize();
	float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
						(lStartY - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lStartX - pThis->mZBufferX;

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();

	double lOneOverLength = 1.0f / (double)(lLength + 1);
	double lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;

	for (int i = 0; i <= lLength; i++)
	{
		if (lOneOverZ > *lZBuffer)
		{
			lScreen[0] = pColor.mBlue;
			lScreen[1] = pColor.mGreen;
			lScreen[2] = pColor.mRed;
		}

		lOneOverZ += (float)lOneOverZStep;

		lScreen += lScreenStep;
		lZBuffer += lZBufStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
			lZBuffer += lZBufStepExtra;
		}
	}
}

void Software3DPainter::DrawSingleColorLine8BitFastZW(const Vertex3D* pVertices, const Lepra::Color& pColor, Software3DPainter* pThis)
{
	int lScreenStep = (int)pThis->mScreen->GetPixelByteSize();
	int lScreenStepExtra;
	int lZBufStep = 1;
	int lZBufStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
		lZBufStep = -lZBufStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();
		lZBufStepExtra = pThis->mZBuffer->GetPitch();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;
		lZBufStepExtra = lZBufStep;

		lScreenStep = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();
		lZBufStep = pThis->mZBuffer->GetPitch();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned char* lScreen = (unsigned char*)pThis->mScreen->GetBuffer() +
							   lStartY * pThis->mScreen->GetPitch() + lStartX;
	float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
						(lStartY - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lStartX - pThis->mZBufferX;

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();

	double lOneOverLength = 1.0f / (double)(lLength + 1);
	double lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;

	for (int i = 0; i <= lLength; i++)
	{
		*lScreen = pColor.mAlpha;
		*lZBuffer = lOneOverZ;

		lOneOverZ += (float)lOneOverZStep;

		lScreen += lScreenStep;
		lZBuffer += lZBufStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
			lZBuffer += lZBufStepExtra;
		}
	}
}

void Software3DPainter::DrawSingleColorLine15BitFastZW(const Vertex3D* pVertices, const Lepra::Color& pColor, Software3DPainter* pThis)
{
	int lScreenStep = 1;
	int lScreenStepExtra;
	int lZBufStep = 1;
	int lZBufStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
		lZBufStep = -lZBufStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch();
		lZBufStepExtra = pThis->mZBuffer->GetPitch();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;
		lZBufStepExtra = lZBufStep;

		lScreenStep = pThis->mScreen->GetPitch();
		lZBufStep = pThis->mZBuffer->GetPitch();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned short* lScreen = (unsigned short*)pThis->mScreen->GetBuffer() +
							   lStartY * pThis->mScreen->GetPitch() + lStartX;
	float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
						(lStartY - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lStartX - pThis->mZBufferX;

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();

	double lOneOverLength = 1.0f / (double)(lLength + 1);
	double lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;

	unsigned short lColor = ((pColor.mRed >> 3) << 10) | 
								((pColor.mGreen >> 3) << 5) | 
								(pColor.mBlue >> 3);

	for (int i = 0; i <= lLength; i++)
	{
		*lScreen = lColor;
		*lZBuffer = lOneOverZ;

		lOneOverZ += (float)lOneOverZStep;

		lScreen += lScreenStep;
		lZBuffer += lZBufStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
			lZBuffer += lZBufStepExtra;
		}
	}
}

void Software3DPainter::DrawSingleColorLine16BitFastZW(const Vertex3D* pVertices, const Lepra::Color& pColor, Software3DPainter* pThis)
{
	int lScreenStep = 1;
	int lScreenStepExtra;
	int lZBufStep = 1;
	int lZBufStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
		lZBufStep = -lZBufStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch();
		lZBufStepExtra = pThis->mZBuffer->GetPitch();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;
		lZBufStepExtra = lZBufStep;

		lScreenStep = pThis->mScreen->GetPitch();
		lZBufStep = pThis->mZBuffer->GetPitch();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned short* lScreen = (unsigned short*)pThis->mScreen->GetBuffer() +
							   lStartY * pThis->mScreen->GetPitch() + lStartX;
	float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
						(lStartY - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lStartX - pThis->mZBufferX;

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();

	double lOneOverLength = 1.0f / (double)(lLength + 1);
	double lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;

	unsigned short lColor = ((pColor.mRed >> 3) << 11) | 
								((pColor.mGreen >> 2) << 5) | 
								(pColor.mBlue >> 3);

	for (int i = 0; i <= lLength; i++)
	{
		*lScreen = lColor;
		*lZBuffer = lOneOverZ;

		lOneOverZ += (float)lOneOverZStep;

		lScreen += lScreenStep;
		lZBuffer += lZBufStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
			lZBuffer += lZBufStepExtra;
		}
	}
}

void Software3DPainter::DrawSingleColorLine32BitFastZW(const Vertex3D* pVertices, const Lepra::Color& pColor, Software3DPainter* pThis)
{
	int lScreenStep = (int)pThis->mScreen->GetPixelByteSize();
	int lScreenStepExtra;
	int lZBufStep = 1;
	int lZBufStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
		lZBufStep = -lZBufStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();
		lZBufStepExtra = pThis->mZBuffer->GetPitch();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;
		lZBufStepExtra = lZBufStep;

		lScreenStep = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();
		lZBufStep = pThis->mZBuffer->GetPitch();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned char* lScreen = (unsigned char*)pThis->mScreen->GetBuffer() +
							   (lStartY * pThis->mScreen->GetPitch() + lStartX) * pThis->mScreen->GetPixelByteSize();
	float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
						(lStartY - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lStartX - pThis->mZBufferX;

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();

	double lOneOverLength = 1.0f / (double)(lLength + 1);
	double lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;

	for (int i = 0; i <= lLength; i++)
	{
		lScreen[0] = pColor.mBlue;
		lScreen[1] = pColor.mGreen;
		lScreen[2] = pColor.mRed;
		
		*lZBuffer = lOneOverZ;

		lOneOverZ += (float)lOneOverZStep;

		lScreen += lScreenStep;
		lZBuffer += lZBufStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
			lZBuffer += lZBufStepExtra;
		}
	}
}

void Software3DPainter::DrawSingleColorLine8BitFastZTZW(const Vertex3D* pVertices, const Lepra::Color& pColor, Software3DPainter* pThis)
{
	int lScreenStep = 1;
	int lScreenStepExtra;
	int lZBufStep = 1;
	int lZBufStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
		lZBufStep = -lZBufStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch();
		lZBufStepExtra = pThis->mZBuffer->GetPitch();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;
		lZBufStepExtra = lZBufStep;

		lScreenStep = pThis->mScreen->GetPitch();
		lZBufStep = pThis->mZBuffer->GetPitch();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned char* lScreen = (unsigned char*)pThis->mScreen->GetBuffer() +
							   lStartY * pThis->mScreen->GetPitch() + lStartX;
	float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
						(lStartY - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lStartX - pThis->mZBufferX;

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();

	double lOneOverLength = 1.0f / (double)(lLength + 1);
	double lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;

	for (int i = 0; i <= lLength; i++)
	{
		if (lOneOverZ > *lZBuffer)
		{
			*lScreen = pColor.mAlpha;
			*lZBuffer = lOneOverZ;
		}

		lOneOverZ += (float)lOneOverZStep;

		lScreen += lScreenStep;
		lZBuffer += lZBufStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
			lZBuffer += lZBufStepExtra;
		}
	}
}

void Software3DPainter::DrawSingleColorLine15BitFastZTZW(const Vertex3D* pVertices, const Lepra::Color& pColor, Software3DPainter* pThis)
{
	int lScreenStep = 1;
	int lScreenStepExtra;
	int lZBufStep = 1;
	int lZBufStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
		lZBufStep = -lZBufStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch();
		lZBufStepExtra = pThis->mZBuffer->GetPitch();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;
		lZBufStepExtra = lZBufStep;

		lScreenStep = pThis->mScreen->GetPitch();
		lZBufStep = pThis->mZBuffer->GetPitch();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned short* lScreen = (unsigned short*)pThis->mScreen->GetBuffer() +
							   lStartY * pThis->mScreen->GetPitch() + lStartX;
	float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
						(lStartY - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lStartX - pThis->mZBufferX;

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();

	double lOneOverLength = 1.0f / (double)(lLength + 1);
	double lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;

	unsigned short lColor = ((pColor.mRed >> 3) << 10) | 
								((pColor.mGreen >> 3) << 5) | 
								(pColor.mBlue >> 3);

	for (int i = 0; i <= lLength; i++)
	{
		if (lOneOverZ > *lZBuffer)
		{
			*lScreen = lColor;
			*lZBuffer = lOneOverZ;
		}

		lOneOverZ += (float)lOneOverZStep;

		lScreen += lScreenStep;
		lZBuffer += lZBufStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
			lZBuffer += lZBufStepExtra;
		}
	}
}

void Software3DPainter::DrawSingleColorLine16BitFastZTZW(const Vertex3D* pVertices, const Lepra::Color& pColor, Software3DPainter* pThis)
{
	int lScreenStep = 1;
	int lScreenStepExtra;
	int lZBufStep = 1;
	int lZBufStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
		lZBufStep = -lZBufStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch();
		lZBufStepExtra = pThis->mZBuffer->GetPitch();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;
		lZBufStepExtra = lZBufStep;

		lScreenStep = pThis->mScreen->GetPitch();
		lZBufStep = pThis->mZBuffer->GetPitch();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned short* lScreen = (unsigned short*)pThis->mScreen->GetBuffer() +
							   lStartY * pThis->mScreen->GetPitch() + lStartX;
	float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
						(lStartY - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lStartX - pThis->mZBufferX;

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();

	double lOneOverLength = 1.0f / (double)(lLength + 1);
	double lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;

	unsigned short lColor = ((pColor.mRed >> 3) << 11) | 
								((pColor.mGreen >> 2) << 5) | 
								(pColor.mBlue >> 3);

	for (int i = 0; i <= lLength; i++)
	{
		if (lOneOverZ > *lZBuffer)
		{
			*lScreen = lColor;
			*lZBuffer = lOneOverZ;
		}

		lOneOverZ += (float)lOneOverZStep;

		lScreen += lScreenStep;
		lZBuffer += lZBufStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
			lZBuffer += lZBufStepExtra;
		}
	}
}

void Software3DPainter::DrawSingleColorLine32BitFastZTZW(const Vertex3D* pVertices, const Lepra::Color& pColor, Software3DPainter* pThis)
{
	int lScreenStep = (int)pThis->mScreen->GetPixelByteSize();
	int lScreenStepExtra;
	int lZBufStep = 1;
	int lZBufStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
		lZBufStep = -lZBufStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();
		lZBufStepExtra = pThis->mZBuffer->GetPitch();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;
		lZBufStepExtra = lZBufStep;

		lScreenStep = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();
		lZBufStep = pThis->mZBuffer->GetPitch();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned char* lScreen = (unsigned char*)pThis->mScreen->GetBuffer() +
							   (lStartY * pThis->mScreen->GetPitch() + lStartX) * pThis->mScreen->GetPixelByteSize();
	float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
						(lStartY - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lStartX - pThis->mZBufferX;

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();

	double lOneOverLength = 1.0f / (double)(lLength + 1);
	double lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;

	for (int i = 0; i <= lLength; i++)
	{
		if (lOneOverZ > *lZBuffer)
		{
			lScreen[0] = pColor.mBlue;
			lScreen[1] = pColor.mGreen;
			lScreen[2] = pColor.mRed;
			*lZBuffer = lOneOverZ;
		}

		lOneOverZ += (float)lOneOverZStep;

		lScreen += lScreenStep;
		lZBuffer += lZBufStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
			lZBuffer += lZBufStepExtra;
		}
	}
}

void Software3DPainter::DrawRGBLine8BitFast(const Vertex3DRGB* pVertices, Software3DPainter* pThis)
{
	int lScreenStep = (int)pThis->mScreen->GetPixelByteSize();
	int lScreenStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;

		lScreenStep = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned char* lScreen = (unsigned char*)pThis->mScreen->GetBuffer() +
							   lStartY * pThis->mScreen->GetPitch() + lStartX;

	Lepra::FixedPointMath::Fixed16_16 lR = Lepra::FixedPointMath::FloatToFixed16_16((pVertices[lTop].GetROverZ() / pVertices[lTop].GetOneOverZ()) * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lREnd = Lepra::FixedPointMath::FloatToFixed16_16((pVertices[lBottom].GetROverZ() / pVertices[lBottom].GetOneOverZ()) * 255.0f);

	Lepra::FixedPointMath::Fixed16_16 lRStep = (lREnd - lR) / (lLength + 1);

	for (int i = 0; i <= lLength; i++)
	{
		*lScreen = (unsigned char)(lR >> 16);

		lR += lRStep;

		lScreen += lScreenStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
		}
	}
}

void Software3DPainter::DrawRGBLine15BitFast(const Vertex3DRGB* pVertices, Software3DPainter* pThis)
{
	int lScreenStep = 1;
	int lScreenStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;

		lScreenStep = pThis->mScreen->GetPitch();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned short* lScreen = (unsigned short*)pThis->mScreen->GetBuffer() +
							   lStartY * pThis->mScreen->GetPitch() + lStartX;

	double lZ = 1.0f / pVertices[lTop].GetOneOverZ();
	Lepra::FixedPointMath::Fixed16_16 lR = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetROverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lG = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetGOverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lB = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetBOverZ() * lZ * 255.0f);

	lZ = 1.0f / pVertices[lBottom].GetOneOverZ();
	Lepra::FixedPointMath::Fixed16_16 lREnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetROverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lGEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetGOverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lBEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetBOverZ() * lZ * 255.0f);

	Lepra::FixedPointMath::Fixed16_16 lRStep = (lREnd - lR) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lGStep = (lGEnd - lG) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lBStep = (lBEnd - lB) / (lLength + 1);

	for (int i = 0; i <= lLength; i++)
	{
		*lScreen = (unsigned short)(((lR >> 19) << 10) | ((lG >> 19) << 5) | (lB >> 19));

		lR += lRStep;
		lG += lGStep;
		lB += lBStep;

		lScreen += lScreenStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
		}
	}
}

void Software3DPainter::DrawRGBLine16BitFast(const Vertex3DRGB* pVertices, Software3DPainter* pThis)
{
	int lScreenStep = 1;
	int lScreenStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;

		lScreenStep = pThis->mScreen->GetPitch();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned short* lScreen = (unsigned short*)pThis->mScreen->GetBuffer() +
							   lStartY * pThis->mScreen->GetPitch() + lStartX;

	double lZ = 1.0f / pVertices[lTop].GetOneOverZ();
	Lepra::FixedPointMath::Fixed16_16 lR = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetROverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lG = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetGOverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lB = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetBOverZ() * lZ * 255.0f);

	lZ = 1.0f / pVertices[lBottom].GetOneOverZ();
	Lepra::FixedPointMath::Fixed16_16 lREnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetROverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lGEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetGOverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lBEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetBOverZ() * lZ * 255.0f);

	Lepra::FixedPointMath::Fixed16_16 lRStep = (lREnd - lR) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lGStep = (lGEnd - lG) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lBStep = (lBEnd - lB) / (lLength + 1);

	for (int i = 0; i <= lLength; i++)
	{
		*lScreen = (unsigned short)(((lR >> 19) << 11) | ((lG >> 18) << 5) | (lB >> 19));

		lR += lRStep;
		lG += lGStep;
		lB += lBStep;

		lScreen += lScreenStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
		}
	}
}

void Software3DPainter::DrawRGBLine32BitFast(const Vertex3DRGB* pVertices, Software3DPainter* pThis)
{
	int lScreenStep = (int)pThis->mScreen->GetPixelByteSize();
	int lScreenStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;

		lScreenStep = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned char* lScreen = (unsigned char*)pThis->mScreen->GetBuffer() +
							   (lStartY * pThis->mScreen->GetPitch() + lStartX) * pThis->mScreen->GetPixelByteSize();

	double lZ = 1.0f / pVertices[lTop].GetOneOverZ();
	Lepra::FixedPointMath::Fixed16_16 lR = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetROverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lG = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetGOverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lB = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetBOverZ() * lZ * 255.0f);

	lZ = 1.0f / pVertices[lBottom].GetOneOverZ();
	Lepra::FixedPointMath::Fixed16_16 lREnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetROverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lGEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetGOverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lBEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetBOverZ() * lZ * 255.0f);

	Lepra::FixedPointMath::Fixed16_16 lRStep = (lREnd - lR) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lGStep = (lGEnd - lG) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lBStep = (lBEnd - lB) / (lLength + 1);

	for (int i = 0; i <= lLength; i++)
	{
		lScreen[0] = (unsigned char)(lB >> 16);
		lScreen[1] = (unsigned char)(lG >> 16);
		lScreen[2] = (unsigned char)(lR >> 16);

		lR += lRStep;
		lG += lGStep;
		lB += lBStep;

		lScreen += lScreenStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
		}
	}
}

void Software3DPainter::DrawRGBLine8BitFastZT(const Vertex3DRGB* pVertices, Software3DPainter* pThis)
{
	int lScreenStep = (int)pThis->mScreen->GetPixelByteSize();
	int lScreenStepExtra;
	int lZBufStep = 1;
	int lZBufStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
		lZBufStep = -lZBufStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();
		lZBufStepExtra = pThis->mZBuffer->GetPitch();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;
		lZBufStepExtra = lZBufStep;

		lScreenStep = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();
		lZBufStep = pThis->mZBuffer->GetPitch();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned char* lScreen = (unsigned char*)pThis->mScreen->GetBuffer() +
							   lStartY * pThis->mScreen->GetPitch() + lStartX;
	float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
						(lStartY - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lStartX - pThis->mZBufferX;

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();

	double lOneOverLength = 1.0f / (double)(lLength + 1);
	double lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;

	Lepra::FixedPointMath::Fixed16_16 lR = Lepra::FixedPointMath::FloatToFixed16_16((pVertices[lTop].GetROverZ() / pVertices[lTop].GetOneOverZ()) * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lREnd = Lepra::FixedPointMath::FloatToFixed16_16((pVertices[lBottom].GetROverZ() / pVertices[lBottom].GetOneOverZ()) * 255.0f);

	Lepra::FixedPointMath::Fixed16_16 lRStep = (lREnd - lR) / (lLength + 1);

	for (int i = 0; i <= lLength; i++)
	{
		if (lOneOverZ > *lZBuffer)
		{
			*lScreen = (unsigned char)(lR >> 16);
		}

		lR += lRStep;

		lOneOverZ += (float)lOneOverZStep;

		lScreen += lScreenStep;
		lZBuffer += lZBufStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
			lZBuffer += lZBufStepExtra;
		}
	}
}

void Software3DPainter::DrawRGBLine15BitFastZT(const Vertex3DRGB* pVertices, Software3DPainter* pThis)
{
	int lScreenStep = 1;
	int lScreenStepExtra;
	int lZBufStep = 1;
	int lZBufStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
		lZBufStep = -lZBufStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch();
		lZBufStepExtra = pThis->mZBuffer->GetPitch();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;
		lZBufStepExtra = lZBufStep;

		lScreenStep = pThis->mScreen->GetPitch();
		lZBufStep = pThis->mZBuffer->GetPitch();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned short* lScreen = (unsigned short*)pThis->mScreen->GetBuffer() +
							   lStartY * pThis->mScreen->GetPitch() + lStartX;
	float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
						(lStartY - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lStartX - pThis->mZBufferX;

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();

	double lOneOverLength = 1.0f / (double)(lLength + 1);
	double lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;

	double lZ = 1.0f / pVertices[lTop].GetOneOverZ();
	Lepra::FixedPointMath::Fixed16_16 lR = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetROverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lG = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetGOverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lB = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetBOverZ() * lZ * 255.0f);

	lZ = 1.0f / pVertices[lBottom].GetOneOverZ();
	Lepra::FixedPointMath::Fixed16_16 lREnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetROverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lGEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetGOverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lBEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetBOverZ() * lZ * 255.0f);

	Lepra::FixedPointMath::Fixed16_16 lRStep = (lREnd - lR) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lGStep = (lGEnd - lG) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lBStep = (lBEnd - lB) / (lLength + 1);

	for (int i = 0; i <= lLength; i++)
	{
		if (lOneOverZ > *lZBuffer)
		{
			*lScreen = (unsigned short)(((lR >> 19) << 10) | ((lG >> 19) << 5) | (lB >> 19));
		}

		lR += lRStep;
		lG += lGStep;
		lB += lBStep;

		lOneOverZ += (float)lOneOverZStep;

		lScreen += lScreenStep;
		lZBuffer += lZBufStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
			lZBuffer += lZBufStepExtra;
		}
	}
}

void Software3DPainter::DrawRGBLine16BitFastZT(const Vertex3DRGB* pVertices, Software3DPainter* pThis)
{
	int lScreenStep = 1;
	int lScreenStepExtra;
	int lZBufStep = 1;
	int lZBufStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
		lZBufStep = -lZBufStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch();
		lZBufStepExtra = pThis->mZBuffer->GetPitch();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;
		lZBufStepExtra = lZBufStep;

		lScreenStep = pThis->mScreen->GetPitch();
		lZBufStep = pThis->mZBuffer->GetPitch();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned short* lScreen = (unsigned short*)pThis->mScreen->GetBuffer() +
							   lStartY * pThis->mScreen->GetPitch() + lStartX;
	float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
						(lStartY - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lStartX - pThis->mZBufferX;

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();

	double lOneOverLength = 1.0f / (double)(lLength + 1);
	double lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;

	double lZ = 1.0f / pVertices[lTop].GetOneOverZ();
	Lepra::FixedPointMath::Fixed16_16 lR = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetROverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lG = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetGOverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lB = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetBOverZ() * lZ * 255.0f);

	lZ = 1.0f / pVertices[lBottom].GetOneOverZ();
	Lepra::FixedPointMath::Fixed16_16 lREnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetROverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lGEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetGOverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lBEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetBOverZ() * lZ * 255.0f);

	Lepra::FixedPointMath::Fixed16_16 lRStep = (lREnd - lR) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lGStep = (lGEnd - lG) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lBStep = (lBEnd - lB) / (lLength + 1);

	for (int i = 0; i <= lLength; i++)
	{
		if (lOneOverZ > *lZBuffer)
		{
			*lScreen = (unsigned short)(((lR >> 19) << 11) | ((lG >> 18) << 5) | (lB >> 19));
		}

		lR += lRStep;
		lG += lGStep;
		lB += lBStep;

		lOneOverZ += (float)lOneOverZStep;

		lScreen += lScreenStep;
		lZBuffer += lZBufStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
			lZBuffer += lZBufStepExtra;
		}
	}
}

void Software3DPainter::DrawRGBLine32BitFastZT(const Vertex3DRGB* pVertices, Software3DPainter* pThis)
{
	int lScreenStep = (int)pThis->mScreen->GetPixelByteSize();
	int lScreenStepExtra;
	int lZBufStep = 1;
	int lZBufStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
		lZBufStep = -lZBufStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();
		lZBufStepExtra = pThis->mZBuffer->GetPitch();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;
		lZBufStepExtra = lZBufStep;

		lScreenStep = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();
		lZBufStep = pThis->mZBuffer->GetPitch();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned char* lScreen = (unsigned char*)pThis->mScreen->GetBuffer() +
							   (lStartY * pThis->mScreen->GetPitch() + lStartX) * pThis->mScreen->GetPixelByteSize();
	float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
						(lStartY - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lStartX - pThis->mZBufferX;

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();

	double lOneOverLength = 1.0f / (double)(lLength + 1);
	double lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;

	double lZ = 1.0f / pVertices[lTop].GetOneOverZ();
	Lepra::FixedPointMath::Fixed16_16 lR = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetROverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lG = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetGOverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lB = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetBOverZ() * lZ * 255.0f);

	lZ = 1.0f / pVertices[lBottom].GetOneOverZ();
	Lepra::FixedPointMath::Fixed16_16 lREnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetROverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lGEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetGOverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lBEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetBOverZ() * lZ * 255.0f);

	Lepra::FixedPointMath::Fixed16_16 lRStep = (lREnd - lR) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lGStep = (lGEnd - lG) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lBStep = (lBEnd - lB) / (lLength + 1);

	for (int i = 0; i <= lLength; i++)
	{
		if (lOneOverZ > *lZBuffer)
		{
			lScreen[0] = (unsigned char)(lB >> 16);
			lScreen[1] = (unsigned char)(lG >> 16);
			lScreen[2] = (unsigned char)(lR >> 16);
		}

		lR += lRStep;
		lG += lGStep;
		lB += lBStep;

		lOneOverZ += (float)lOneOverZStep;

		lScreen += lScreenStep;
		lZBuffer += lZBufStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
			lZBuffer += lZBufStepExtra;
		}
	}
}

void Software3DPainter::DrawRGBLine8BitFastZW(const Vertex3DRGB* pVertices, Software3DPainter* pThis)
{
	int lScreenStep = (int)pThis->mScreen->GetPixelByteSize();
	int lScreenStepExtra;
	int lZBufStep = 1;
	int lZBufStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
		lZBufStep = -lZBufStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();
		lZBufStepExtra = pThis->mZBuffer->GetPitch();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;
		lZBufStepExtra = lZBufStep;

		lScreenStep = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();
		lZBufStep = pThis->mZBuffer->GetPitch();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned char* lScreen = (unsigned char*)pThis->mScreen->GetBuffer() +
							   lStartY * pThis->mScreen->GetPitch() + lStartX;
	float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
						(lStartY - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lStartX - pThis->mZBufferX;

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();

	double lOneOverLength = 1.0f / (double)(lLength + 1);
	double lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;

	Lepra::FixedPointMath::Fixed16_16 lR = Lepra::FixedPointMath::FloatToFixed16_16((pVertices[lTop].GetROverZ() / pVertices[lTop].GetOneOverZ()) * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lREnd = Lepra::FixedPointMath::FloatToFixed16_16((pVertices[lBottom].GetROverZ() / pVertices[lBottom].GetOneOverZ()) * 255.0f);

	Lepra::FixedPointMath::Fixed16_16 lRStep = (lREnd - lR) / (lLength + 1);

	for (int i = 0; i <= lLength; i++)
	{
		*lScreen = (unsigned char)(lR >> 16);
		*lZBuffer = lOneOverZ;

		lR += lRStep;

		lOneOverZ += (float)lOneOverZStep;

		lScreen += lScreenStep;
		lZBuffer += lZBufStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
			lZBuffer += lZBufStepExtra;
		}
	}
}

void Software3DPainter::DrawRGBLine15BitFastZW(const Vertex3DRGB* pVertices, Software3DPainter* pThis)
{
	int lScreenStep = 1;
	int lScreenStepExtra;
	int lZBufStep = 1;
	int lZBufStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
		lZBufStep = -lZBufStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch();
		lZBufStepExtra = pThis->mZBuffer->GetPitch();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;
		lZBufStepExtra = lZBufStep;

		lScreenStep = pThis->mScreen->GetPitch();
		lZBufStep = pThis->mZBuffer->GetPitch();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned short* lScreen = (unsigned short*)pThis->mScreen->GetBuffer() +
							   lStartY * pThis->mScreen->GetPitch() + lStartX;
	float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
						(lStartY - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lStartX - pThis->mZBufferX;

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();

	double lOneOverLength = 1.0f / (double)(lLength + 1);
	double lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;

	double lZ = 1.0f / pVertices[lTop].GetOneOverZ();
	Lepra::FixedPointMath::Fixed16_16 lR = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetROverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lG = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetGOverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lB = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetBOverZ() * lZ * 255.0f);

	lZ = 1.0f / pVertices[lBottom].GetOneOverZ();
	Lepra::FixedPointMath::Fixed16_16 lREnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetROverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lGEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetGOverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lBEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetBOverZ() * lZ * 255.0f);

	Lepra::FixedPointMath::Fixed16_16 lRStep = (lREnd - lR) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lGStep = (lGEnd - lG) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lBStep = (lBEnd - lB) / (lLength + 1);

	for (int i = 0; i <= lLength; i++)
	{
		*lScreen = (unsigned short)(((lR >> 19) << 10) | ((lG >> 19) << 5) | (lB >> 19));
		*lZBuffer = lOneOverZ;

		lR += lRStep;
		lG += lGStep;
		lB += lBStep;

		lOneOverZ += (float)lOneOverZStep;

		lScreen += lScreenStep;
		lZBuffer += lZBufStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
			lZBuffer += lZBufStepExtra;
		}
	}
}

void Software3DPainter::DrawRGBLine16BitFastZW(const Vertex3DRGB* pVertices, Software3DPainter* pThis)
{
	int lScreenStep = 1;
	int lScreenStepExtra;
	int lZBufStep = 1;
	int lZBufStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
		lZBufStep = -lZBufStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch();
		lZBufStepExtra = pThis->mZBuffer->GetPitch();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;
		lZBufStepExtra = lZBufStep;

		lScreenStep = pThis->mScreen->GetPitch();
		lZBufStep = pThis->mZBuffer->GetPitch();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned short* lScreen = (unsigned short*)pThis->mScreen->GetBuffer() +
							   lStartY * pThis->mScreen->GetPitch() + lStartX;
	float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
						(lStartY - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lStartX - pThis->mZBufferX;

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();

	double lOneOverLength = 1.0f / (double)(lLength + 1);
	double lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;

	double lZ = 1.0f / pVertices[lTop].GetOneOverZ();
	Lepra::FixedPointMath::Fixed16_16 lR = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetROverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lG = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetGOverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lB = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetBOverZ() * lZ * 255.0f);

	lZ = 1.0f / pVertices[lBottom].GetOneOverZ();
	Lepra::FixedPointMath::Fixed16_16 lREnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetROverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lGEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetGOverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lBEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetBOverZ() * lZ * 255.0f);

	Lepra::FixedPointMath::Fixed16_16 lRStep = (lREnd - lR) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lGStep = (lGEnd - lG) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lBStep = (lBEnd - lB) / (lLength + 1);

	for (int i = 0; i <= lLength; i++)
	{
		*lScreen = (unsigned short)(((lR >> 19) << 11) | ((lG >> 18) << 5) | (lB >> 19));
		*lZBuffer = lOneOverZ;

		lR += lRStep;
		lG += lGStep;
		lB += lBStep;

		lOneOverZ += (float)lOneOverZStep;

		lScreen += lScreenStep;
		lZBuffer += lZBufStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
			lZBuffer += lZBufStepExtra;
		}
	}
}

void Software3DPainter::DrawRGBLine32BitFastZW(const Vertex3DRGB* pVertices, Software3DPainter* pThis)
{
	int lScreenStep = (int)pThis->mScreen->GetPixelByteSize();
	int lScreenStepExtra;
	int lZBufStep = 1;
	int lZBufStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
		lZBufStep = -lZBufStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();
		lZBufStepExtra = pThis->mZBuffer->GetPitch();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;
		lZBufStepExtra = lZBufStep;

		lScreenStep = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();
		lZBufStep = pThis->mZBuffer->GetPitch();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned char* lScreen = (unsigned char*)pThis->mScreen->GetBuffer() +
							   (lStartY * pThis->mScreen->GetPitch() + lStartX) * pThis->mScreen->GetPixelByteSize();
	float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
						(lStartY - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lStartX - pThis->mZBufferX;

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();

	double lOneOverLength = 1.0f / (double)(lLength + 1);
	double lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;

	double lZ = 1.0f / pVertices[lTop].GetOneOverZ();
	Lepra::FixedPointMath::Fixed16_16 lR = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetROverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lG = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetGOverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lB = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetBOverZ() * lZ * 255.0f);

	lZ = 1.0f / pVertices[lBottom].GetOneOverZ();
	Lepra::FixedPointMath::Fixed16_16 lREnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetROverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lGEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetGOverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lBEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetBOverZ() * lZ * 255.0f);

	Lepra::FixedPointMath::Fixed16_16 lRStep = (lREnd - lR) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lGStep = (lGEnd - lG) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lBStep = (lBEnd - lB) / (lLength + 1);

	for (int i = 0; i <= lLength; i++)
	{
		lScreen[0] = (unsigned char)(lB >> 16);
		lScreen[1] = (unsigned char)(lG >> 16);
		lScreen[2] = (unsigned char)(lR >> 16);
		*lZBuffer = lOneOverZ;

		lR += lRStep;
		lG += lGStep;
		lB += lBStep;

		lOneOverZ += (float)lOneOverZStep;

		lScreen += lScreenStep;
		lZBuffer += lZBufStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
			lZBuffer += lZBufStepExtra;
		}
	}
}

void Software3DPainter::DrawRGBLine8BitFastZTZW(const Vertex3DRGB* pVertices, Software3DPainter* pThis)
{
	int lScreenStep = (int)pThis->mScreen->GetPixelByteSize();
	int lScreenStepExtra;
	int lZBufStep = 1;
	int lZBufStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
		lZBufStep = -lZBufStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();
		lZBufStepExtra = pThis->mZBuffer->GetPitch();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;
		lZBufStepExtra = lZBufStep;

		lScreenStep = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();
		lZBufStep = pThis->mZBuffer->GetPitch();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned char* lScreen = (unsigned char*)pThis->mScreen->GetBuffer() +
							   lStartY * pThis->mScreen->GetPitch() + lStartX;
	float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
						(lStartY - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lStartX - pThis->mZBufferX;

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();

	double lOneOverLength = 1.0f / (double)(lLength + 1);
	double lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;

	Lepra::FixedPointMath::Fixed16_16 lR = Lepra::FixedPointMath::FloatToFixed16_16((pVertices[lTop].GetROverZ() / pVertices[lTop].GetOneOverZ()) * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lREnd = Lepra::FixedPointMath::FloatToFixed16_16((pVertices[lBottom].GetROverZ() / pVertices[lBottom].GetOneOverZ()) * 255.0f);

	Lepra::FixedPointMath::Fixed16_16 lRStep = (lREnd - lR) / (lLength + 1);

	for (int i = 0; i <= lLength; i++)
	{
		if (lOneOverZ > *lZBuffer)
		{
			*lScreen = (unsigned char)(lR >> 16);
			*lZBuffer = lOneOverZ;
		}

		lR += lRStep;

		lOneOverZ += (float)lOneOverZStep;

		lScreen += lScreenStep;
		lZBuffer += lZBufStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
			lZBuffer += lZBufStepExtra;
		}
	}
}

void Software3DPainter::DrawRGBLine15BitFastZTZW(const Vertex3DRGB* pVertices, Software3DPainter* pThis)
{
	int lScreenStep = 1;
	int lScreenStepExtra;
	int lZBufStep = 1;
	int lZBufStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
		lZBufStep = -lZBufStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch();
		lZBufStepExtra = pThis->mZBuffer->GetPitch();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;
		lZBufStepExtra = lZBufStep;

		lScreenStep = pThis->mScreen->GetPitch();
		lZBufStep = pThis->mZBuffer->GetPitch();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned short* lScreen = (unsigned short*)pThis->mScreen->GetBuffer() +
							   lStartY * pThis->mScreen->GetPitch() + lStartX;
	float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
						(lStartY - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lStartX - pThis->mZBufferX;

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();

	double lOneOverLength = 1.0f / (double)(lLength + 1);
	double lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;

	double lZ = 1.0f / pVertices[lTop].GetOneOverZ();
	Lepra::FixedPointMath::Fixed16_16 lR = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetROverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lG = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetGOverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lB = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetBOverZ() * lZ * 255.0f);

	lZ = 1.0f / pVertices[lBottom].GetOneOverZ();
	Lepra::FixedPointMath::Fixed16_16 lREnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetROverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lGEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetGOverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lBEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetBOverZ() * lZ * 255.0f);

	Lepra::FixedPointMath::Fixed16_16 lRStep = (lREnd - lR) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lGStep = (lGEnd - lG) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lBStep = (lBEnd - lB) / (lLength + 1);

	for (int i = 0; i <= lLength; i++)
	{
		if (lOneOverZ > *lZBuffer)
		{
			*lScreen = (unsigned short)(((lR >> 19) << 10) | ((lG >> 19) << 5) | (lB >> 19));
			*lZBuffer = lOneOverZ;
		}

		lR += lRStep;
		lG += lGStep;
		lB += lBStep;

		lOneOverZ += (float)lOneOverZStep;

		lScreen += lScreenStep;
		lZBuffer += lZBufStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
			lZBuffer += lZBufStepExtra;
		}
	}
}

void Software3DPainter::DrawRGBLine16BitFastZTZW(const Vertex3DRGB* pVertices, Software3DPainter* pThis)
{
	int lScreenStep = 1;
	int lScreenStepExtra;
	int lZBufStep = 1;
	int lZBufStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
		lZBufStep = -lZBufStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch();
		lZBufStepExtra = pThis->mZBuffer->GetPitch();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;
		lZBufStepExtra = lZBufStep;

		lScreenStep = pThis->mScreen->GetPitch();
		lZBufStep = pThis->mZBuffer->GetPitch();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned short* lScreen = (unsigned short*)pThis->mScreen->GetBuffer() +
							   lStartY * pThis->mScreen->GetPitch() + lStartX;
	float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
						(lStartY - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lStartX - pThis->mZBufferX;

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();

	double lOneOverLength = 1.0f / (double)(lLength + 1);
	double lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;

	double lZ = 1.0f / pVertices[lTop].GetOneOverZ();
	Lepra::FixedPointMath::Fixed16_16 lR = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetROverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lG = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetGOverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lB = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetBOverZ() * lZ * 255.0f);

	lZ = 1.0f / pVertices[lBottom].GetOneOverZ();
	Lepra::FixedPointMath::Fixed16_16 lREnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetROverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lGEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetGOverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lBEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetBOverZ() * lZ * 255.0f);

	Lepra::FixedPointMath::Fixed16_16 lRStep = (lREnd - lR) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lGStep = (lGEnd - lG) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lBStep = (lBEnd - lB) / (lLength + 1);

	for (int i = 0; i <= lLength; i++)
	{
		if (lOneOverZ > *lZBuffer)
		{
			*lScreen = (unsigned short)(((lR >> 19) << 11) | ((lG >> 18) << 5) | (lB >> 19));
			*lZBuffer = lOneOverZ;
		}

		lR += lRStep;
		lG += lGStep;
		lB += lBStep;

		lOneOverZ += (float)lOneOverZStep;

		lScreen += lScreenStep;
		lZBuffer += lZBufStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
			lZBuffer += lZBufStepExtra;
		}
	}
}

void Software3DPainter::DrawRGBLine32BitFastZTZW(const Vertex3DRGB* pVertices, Software3DPainter* pThis)
{
	int lScreenStep = (int)pThis->mScreen->GetPixelByteSize();
	int lScreenStepExtra;
	int lZBufStep = 1;
	int lZBufStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
		lZBufStep = -lZBufStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();
		lZBufStepExtra = pThis->mZBuffer->GetPitch();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;
		lZBufStepExtra = lZBufStep;

		lScreenStep = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();
		lZBufStep = pThis->mZBuffer->GetPitch();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned char* lScreen = (unsigned char*)pThis->mScreen->GetBuffer() +
							   (lStartY * pThis->mScreen->GetPitch() + lStartX) * pThis->mScreen->GetPixelByteSize();
	float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
						(lStartY - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lStartX - pThis->mZBufferX;

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();

	double lOneOverLength = 1.0f / (double)(lLength + 1);
	double lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;

	double lZ = 1.0f / pVertices[lTop].GetOneOverZ();
	Lepra::FixedPointMath::Fixed16_16 lR = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetROverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lG = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetGOverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lB = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetBOverZ() * lZ * 255.0f);

	lZ = 1.0f / pVertices[lBottom].GetOneOverZ();
	Lepra::FixedPointMath::Fixed16_16 lREnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetROverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lGEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetGOverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lBEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetBOverZ() * lZ * 255.0f);

	Lepra::FixedPointMath::Fixed16_16 lRStep = (lREnd - lR) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lGStep = (lGEnd - lG) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lBStep = (lBEnd - lB) / (lLength + 1);

	for (int i = 0; i <= lLength; i++)
	{
		if (lOneOverZ > *lZBuffer)
		{
			lScreen[0] = (unsigned char)(lB >> 16);
			lScreen[1] = (unsigned char)(lG >> 16);
			lScreen[2] = (unsigned char)(lR >> 16);
			*lZBuffer = lOneOverZ;
		}

		lR += lRStep;
		lG += lGStep;
		lB += lBStep;

		lOneOverZ += (float)lOneOverZStep;

		lScreen += lScreenStep;
		lZBuffer += lZBufStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
			lZBuffer += lZBufStepExtra;
		}
	}
}

void Software3DPainter::DrawUVLine8BitFast(const Vertex3DUV* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis)
{
	int lScreenStep = (int)pThis->mScreen->GetPixelByteSize();
	int lScreenStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;

		lScreenStep = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned char* lScreen = (unsigned char*)pThis->mScreen->GetBuffer() +
							   lStartY * pThis->mScreen->GetPitch() + lStartX;
	unsigned char* lTextureBits = (unsigned char*)pTexture->GetBuffer();

	Lepra::FixedPointMath::Fixed16_16 lU = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetUOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lV = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetVOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lUEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetUOverZ() / pVertices[lBottom].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lVEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetVOverZ() / pVertices[lBottom].GetOneOverZ());

	Lepra::FixedPointMath::Fixed16_16 lUStep = (lUEnd - lU) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lVStep = (lVEnd - lV) / (lLength + 1);

	for (int i = 0; i <= lLength; i++)
	{
		*lScreen = lTextureBits[(lV >> 16) * pTexture->GetPitch() + (lU >> 16)];

		lU += lUStep;
		lV += lVStep;

		lScreen += lScreenStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
		}
	}
}

void Software3DPainter::DrawUVLine16BitFast(const Vertex3DUV* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis)
{
	int lScreenStep = 1;
	int lScreenStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;

		lScreenStep = pThis->mScreen->GetPitch();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned short* lScreen = (unsigned short*)pThis->mScreen->GetBuffer() +
							   lStartY * pThis->mScreen->GetPitch() + lStartX;
	unsigned short* lTextureBits = (unsigned short*)pTexture->GetBuffer();

	Lepra::FixedPointMath::Fixed16_16 lU = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetUOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lV = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetVOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lUEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetUOverZ() / pVertices[lBottom].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lVEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetVOverZ() / pVertices[lBottom].GetOneOverZ());

	Lepra::FixedPointMath::Fixed16_16 lUStep = (lUEnd - lU) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lVStep = (lVEnd - lV) / (lLength + 1);

	for (int i = 0; i <= lLength; i++)
	{
		*lScreen = lTextureBits[(lV >> 16) * pTexture->GetPitch() + (lU >> 16)];

		lU += lUStep;
		lV += lVStep;

		lScreen += lScreenStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
		}
	}
}

void Software3DPainter::DrawUVLine32BitFast(const Vertex3DUV* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis)
{
	int lScreenStep = (int)pThis->mScreen->GetPixelByteSize();
	int lScreenStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;

		lScreenStep = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned char* lScreen = (unsigned char*)pThis->mScreen->GetBuffer() +
							   (lStartY * pThis->mScreen->GetPitch() + lStartX) * pThis->mScreen->GetPixelByteSize();
	unsigned char* lTextureBits = (unsigned char*)pTexture->GetBuffer();

	Lepra::FixedPointMath::Fixed16_16 lU = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetUOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lV = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetVOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lUEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetUOverZ() / pVertices[lBottom].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lVEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetVOverZ() / pVertices[lBottom].GetOneOverZ());

	Lepra::FixedPointMath::Fixed16_16 lUStep = (lUEnd - lU) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lVStep = (lVEnd - lV) / (lLength + 1);

	for (int i = 0; i <= lLength; i++)
	{
		unsigned char* lT = lTextureBits + ((lV >> 16) * pTexture->GetPitch() + (lU >> 16)) * pTexture->GetPixelByteSize();
		lScreen[0] = lT[0];
		lScreen[1] = lT[1];
		lScreen[2] = lT[2];

		lU += lUStep;
		lV += lVStep;

		lScreen += lScreenStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
		}
	}
}

void Software3DPainter::DrawUVLine8BitFastZT(const Vertex3DUV* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis)
{
	int lScreenStep = (int)pThis->mScreen->GetPixelByteSize();
	int lScreenStepExtra;
	int lZBufStep = 1;
	int lZBufStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
		lZBufStep = -lZBufStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();
		lZBufStepExtra = pThis->mZBuffer->GetPitch();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;
		lZBufStepExtra = lZBufStep;

		lScreenStep = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();
		lZBufStep = pThis->mZBuffer->GetPitch();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned char* lScreen = (unsigned char*)pThis->mScreen->GetBuffer() +
							   lStartY * pThis->mScreen->GetPitch() + lStartX;
	unsigned char* lTextureBits = (unsigned char*)pTexture->GetBuffer();
	float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
						(lStartY - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lStartX - pThis->mZBufferX;

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();

	double lOneOverLength = 1.0f / (double)(lLength + 1);
	double lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;

	Lepra::FixedPointMath::Fixed16_16 lU = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetUOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lV = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetVOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lUEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetUOverZ() / pVertices[lBottom].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lVEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetVOverZ() / pVertices[lBottom].GetOneOverZ());

	Lepra::FixedPointMath::Fixed16_16 lUStep = (lUEnd - lU) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lVStep = (lVEnd - lV) / (lLength + 1);

	for (int i = 0; i <= lLength; i++)
	{
		if (lOneOverZ > *lZBuffer)
		{
			*lScreen = lTextureBits[(lV >> 16) * pTexture->GetPitch() + (lU >> 16)];
		}

		lU += lUStep;
		lV += lVStep;

		lOneOverZ += (float)lOneOverZStep;

		lScreen += lScreenStep;
		lZBuffer += lZBufStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
			lZBuffer += lZBufStepExtra;
		}
	}
}

void Software3DPainter::DrawUVLine16BitFastZT(const Vertex3DUV* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis)
{
	int lScreenStep = 1;
	int lScreenStepExtra;
	int lZBufStep = 1;
	int lZBufStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
		lZBufStep = -lZBufStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch();
		lZBufStepExtra = pThis->mZBuffer->GetPitch();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;
		lZBufStepExtra = lZBufStep;

		lScreenStep = pThis->mScreen->GetPitch();
		lZBufStep = pThis->mZBuffer->GetPitch();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned short* lScreen = (unsigned short*)pThis->mScreen->GetBuffer() +
							   lStartY * pThis->mScreen->GetPitch() + lStartX;
	unsigned short* lTextureBits = (unsigned short*)pTexture->GetBuffer();
	float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
						(lStartY - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lStartX - pThis->mZBufferX;

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();

	double lOneOverLength = 1.0f / (double)(lLength + 1);
	double lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;

	Lepra::FixedPointMath::Fixed16_16 lU = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetUOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lV = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetVOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lUEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetUOverZ() / pVertices[lBottom].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lVEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetVOverZ() / pVertices[lBottom].GetOneOverZ());

	Lepra::FixedPointMath::Fixed16_16 lUStep = (lUEnd - lU) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lVStep = (lVEnd - lV) / (lLength + 1);

	for (int i = 0; i <= lLength; i++)
	{
		if (lOneOverZ > *lZBuffer)
		{
			*lScreen = lTextureBits[(lV >> 16) * pTexture->GetPitch() + (lU >> 16)];
		}

		lU += lUStep;
		lV += lVStep;

		lOneOverZ += (float)lOneOverZStep;

		lScreen += lScreenStep;
		lZBuffer += lZBufStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
			lZBuffer += lZBufStepExtra;
		}
	}
}

void Software3DPainter::DrawUVLine32BitFastZT(const Vertex3DUV* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis)
{
	int lScreenStep = (int)pThis->mScreen->GetPixelByteSize();
	int lScreenStepExtra;
	int lZBufStep = 1;
	int lZBufStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
		lZBufStep = -lZBufStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();
		lZBufStepExtra = pThis->mZBuffer->GetPitch();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;
		lZBufStepExtra = lZBufStep;

		lScreenStep = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();
		lZBufStep = pThis->mZBuffer->GetPitch();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned char* lScreen = (unsigned char*)pThis->mScreen->GetBuffer() +
							   (lStartY * pThis->mScreen->GetPitch() + lStartX) * pThis->mScreen->GetPixelByteSize();
	unsigned char* lTextureBits = (unsigned char*)pTexture->GetBuffer();
	float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
						(lStartY - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lStartX - pThis->mZBufferX;

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();

	double lOneOverLength = 1.0f / (double)(lLength + 1);
	double lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;

	Lepra::FixedPointMath::Fixed16_16 lU = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetUOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lV = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetVOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lUEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetUOverZ() / pVertices[lBottom].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lVEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetVOverZ() / pVertices[lBottom].GetOneOverZ());

	Lepra::FixedPointMath::Fixed16_16 lUStep = (lUEnd - lU) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lVStep = (lVEnd - lV) / (lLength + 1);

	for (int i = 0; i <= lLength; i++)
	{
		if (lOneOverZ > *lZBuffer)
		{
			unsigned char* lT = lTextureBits + ((lV >> 16) * pTexture->GetPitch() + (lU >> 16)) * pTexture->GetPixelByteSize();
			lScreen[0] = lT[0];
			lScreen[1] = lT[1];
			lScreen[2] = lT[2];
		}

		lU += lUStep;
		lV += lVStep;

		lOneOverZ += (float)lOneOverZStep;

		lScreen += lScreenStep;
		lZBuffer += lZBufStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
			lZBuffer += lZBufStepExtra;
		}
	}
}

void Software3DPainter::DrawUVLine8BitFastZW(const Vertex3DUV* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis)
{
	int lScreenStep = (int)pThis->mScreen->GetPixelByteSize();
	int lScreenStepExtra;
	int lZBufStep = 1;
	int lZBufStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
		lZBufStep = -lZBufStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();
		lZBufStepExtra = pThis->mZBuffer->GetPitch();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;
		lZBufStepExtra = lZBufStep;

		lScreenStep = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();
		lZBufStep = pThis->mZBuffer->GetPitch();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned char* lScreen = (unsigned char*)pThis->mScreen->GetBuffer() +
							   lStartY * pThis->mScreen->GetPitch() + lStartX;
	unsigned char* lTextureBits = (unsigned char*)pTexture->GetBuffer();
	float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
						(lStartY - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lStartX - pThis->mZBufferX;

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();

	double lOneOverLength = 1.0f / (double)(lLength + 1);
	double lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;

	Lepra::FixedPointMath::Fixed16_16 lU = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetUOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lV = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetVOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lUEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetUOverZ() / pVertices[lBottom].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lVEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetVOverZ() / pVertices[lBottom].GetOneOverZ());

	Lepra::FixedPointMath::Fixed16_16 lUStep = (lUEnd - lU) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lVStep = (lVEnd - lV) / (lLength + 1);

	for (int i = 0; i <= lLength; i++)
	{
		*lScreen = lTextureBits[(lV >> 16) * pTexture->GetPitch() + (lU >> 16)];
		*lZBuffer = lOneOverZ;

		lU += lUStep;
		lV += lVStep;

		lOneOverZ += (float)lOneOverZStep;

		lScreen += lScreenStep;
		lZBuffer += lZBufStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
			lZBuffer += lZBufStepExtra;
		}
	}
}

void Software3DPainter::DrawUVLine16BitFastZW(const Vertex3DUV* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis)
{
	int lScreenStep = 1;
	int lScreenStepExtra;
	int lZBufStep = 1;
	int lZBufStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
		lZBufStep = -lZBufStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch();
		lZBufStepExtra = pThis->mZBuffer->GetPitch();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;
		lZBufStepExtra = lZBufStep;

		lScreenStep = pThis->mScreen->GetPitch();
		lZBufStep = pThis->mZBuffer->GetPitch();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned short* lScreen = (unsigned short*)pThis->mScreen->GetBuffer() +
							   lStartY * pThis->mScreen->GetPitch() + lStartX;
	unsigned short* lTextureBits = (unsigned short*)pTexture->GetBuffer();
	float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
						(lStartY - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lStartX - pThis->mZBufferX;

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();

	double lOneOverLength = 1.0f / (double)(lLength + 1);
	double lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;

	Lepra::FixedPointMath::Fixed16_16 lU = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetUOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lV = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetVOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lUEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetUOverZ() / pVertices[lBottom].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lVEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetVOverZ() / pVertices[lBottom].GetOneOverZ());

	Lepra::FixedPointMath::Fixed16_16 lUStep = (lUEnd - lU) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lVStep = (lVEnd - lV) / (lLength + 1);

	for (int i = 0; i <= lLength; i++)
	{
		*lScreen = lTextureBits[(lV >> 16) * pTexture->GetPitch() + (lU >> 16)];
		*lZBuffer = lOneOverZ;

		lU += lUStep;
		lV += lVStep;

		lOneOverZ += (float)lOneOverZStep;

		lScreen += lScreenStep;
		lZBuffer += lZBufStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
			lZBuffer += lZBufStepExtra;
		}
	}
}

void Software3DPainter::DrawUVLine32BitFastZW(const Vertex3DUV* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis)
{
	int lScreenStep = (int)pThis->mScreen->GetPixelByteSize();
	int lScreenStepExtra;
	int lZBufStep = 1;
	int lZBufStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
		lZBufStep = -lZBufStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();
		lZBufStepExtra = pThis->mZBuffer->GetPitch();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;
		lZBufStepExtra = lZBufStep;

		lScreenStep = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();
		lZBufStep = pThis->mZBuffer->GetPitch();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned char* lScreen = (unsigned char*)pThis->mScreen->GetBuffer() +
							   (lStartY * pThis->mScreen->GetPitch() + lStartX) * pThis->mScreen->GetPixelByteSize();
	unsigned char* lTextureBits = (unsigned char*)pTexture->GetBuffer();
	float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
						(lStartY - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lStartX - pThis->mZBufferX;

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();

	double lOneOverLength = 1.0f / (double)(lLength + 1);
	double lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;

	Lepra::FixedPointMath::Fixed16_16 lU = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetUOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lV = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetVOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lUEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetUOverZ() / pVertices[lBottom].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lVEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetVOverZ() / pVertices[lBottom].GetOneOverZ());

	Lepra::FixedPointMath::Fixed16_16 lUStep = (lUEnd - lU) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lVStep = (lVEnd - lV) / (lLength + 1);

	for (int i = 0; i <= lLength; i++)
	{
		unsigned char* lT = lTextureBits + ((lV >> 16) * pTexture->GetPitch() + (lU >> 16)) * pTexture->GetPixelByteSize();
		lScreen[0] = lT[0];
		lScreen[1] = lT[1];
		lScreen[2] = lT[2];
		*lZBuffer = lOneOverZ;

		lU += lUStep;
		lV += lVStep;

		lOneOverZ += (float)lOneOverZStep;

		lScreen += lScreenStep;
		lZBuffer += lZBufStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
			lZBuffer += lZBufStepExtra;
		}
	}
}

void Software3DPainter::DrawUVLine8BitFastZTZW(const Vertex3DUV* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis)
{
	int lScreenStep = (int)pThis->mScreen->GetPixelByteSize();
	int lScreenStepExtra;
	int lZBufStep = 1;
	int lZBufStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
		lZBufStep = -lZBufStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();
		lZBufStepExtra = pThis->mZBuffer->GetPitch();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;
		lZBufStepExtra = lZBufStep;

		lScreenStep = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();
		lZBufStep = pThis->mZBuffer->GetPitch();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned char* lScreen = (unsigned char*)pThis->mScreen->GetBuffer() +
							   lStartY * pThis->mScreen->GetPitch() + lStartX;
	unsigned char* lTextureBits = (unsigned char*)pTexture->GetBuffer();
	float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
						(lStartY - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lStartX - pThis->mZBufferX;

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();

	double lOneOverLength = 1.0f / (double)(lLength + 1);
	double lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;

	Lepra::FixedPointMath::Fixed16_16 lU = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetUOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lV = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetVOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lUEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetUOverZ() / pVertices[lBottom].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lVEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetVOverZ() / pVertices[lBottom].GetOneOverZ());

	Lepra::FixedPointMath::Fixed16_16 lUStep = (lUEnd - lU) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lVStep = (lVEnd - lV) / (lLength + 1);

	for (int i = 0; i <= lLength; i++)
	{
		if (lOneOverZ > *lZBuffer)
		{
			*lScreen = lTextureBits[(lV >> 16) * pTexture->GetPitch() + (lU >> 16)];
			*lZBuffer = lOneOverZ;
		}

		lU += lUStep;
		lV += lVStep;

		lOneOverZ += (float)lOneOverZStep;

		lScreen += lScreenStep;
		lZBuffer += lZBufStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
			lZBuffer += lZBufStepExtra;
		}
	}
}

void Software3DPainter::DrawUVLine16BitFastZTZW(const Vertex3DUV* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis)
{
	int lScreenStep = 1;
	int lScreenStepExtra;
	int lZBufStep = 1;
	int lZBufStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
		lZBufStep = -lZBufStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch();
		lZBufStepExtra = pThis->mZBuffer->GetPitch();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;
		lZBufStepExtra = lZBufStep;

		lScreenStep = pThis->mScreen->GetPitch();
		lZBufStep = pThis->mZBuffer->GetPitch();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned short* lScreen = (unsigned short*)pThis->mScreen->GetBuffer() +
							   lStartY * pThis->mScreen->GetPitch() + lStartX;
	unsigned short* lTextureBits = (unsigned short*)pTexture->GetBuffer();
	float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
						(lStartY - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lStartX - pThis->mZBufferX;

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();

	double lOneOverLength = 1.0f / (double)(lLength + 1);
	double lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;

	Lepra::FixedPointMath::Fixed16_16 lU = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetUOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lV = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetVOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lUEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetUOverZ() / pVertices[lBottom].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lVEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetVOverZ() / pVertices[lBottom].GetOneOverZ());

	Lepra::FixedPointMath::Fixed16_16 lUStep = (lUEnd - lU) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lVStep = (lVEnd - lV) / (lLength + 1);

	for (int i = 0; i <= lLength; i++)
	{
		if (lOneOverZ > *lZBuffer)
		{
			*lScreen = lTextureBits[(lV >> 16) * pTexture->GetPitch() + (lU >> 16)];
			*lZBuffer = lOneOverZ;
		}

		lU += lUStep;
		lV += lVStep;

		lOneOverZ += (float)lOneOverZStep;

		lScreen += lScreenStep;
		lZBuffer += lZBufStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
			lZBuffer += lZBufStepExtra;
		}
	}
}

void Software3DPainter::DrawUVLine32BitFastZTZW(const Vertex3DUV* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis)
{
	int lScreenStep = (int)pThis->mScreen->GetPixelByteSize();
	int lScreenStepExtra;
	int lZBufStep = 1;
	int lZBufStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
		lZBufStep = -lZBufStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();
		lZBufStepExtra = pThis->mZBuffer->GetPitch();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;
		lZBufStepExtra = lZBufStep;

		lScreenStep = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();
		lZBufStep = pThis->mZBuffer->GetPitch();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned char* lScreen = (unsigned char*)pThis->mScreen->GetBuffer() +
							   (lStartY * pThis->mScreen->GetPitch() + lStartX) * pThis->mScreen->GetPixelByteSize();
	unsigned char* lTextureBits = (unsigned char*)pTexture->GetBuffer();
	float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
						(lStartY - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lStartX - pThis->mZBufferX;

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();

	double lOneOverLength = 1.0f / (double)(lLength + 1);
	double lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;

	Lepra::FixedPointMath::Fixed16_16 lU = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetUOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lV = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetVOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lUEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetUOverZ() / pVertices[lBottom].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lVEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetVOverZ() / pVertices[lBottom].GetOneOverZ());

	Lepra::FixedPointMath::Fixed16_16 lUStep = (lUEnd - lU) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lVStep = (lVEnd - lV) / (lLength + 1);

	for (int i = 0; i <= lLength; i++)
	{
		if (lOneOverZ > *lZBuffer)
		{
			unsigned char* lT = lTextureBits + ((lV >> 16) * pTexture->GetPitch() + (lU >> 16)) * pTexture->GetPixelByteSize();
			lScreen[0] = lT[0];
			lScreen[1] = lT[1];
			lScreen[2] = lT[2];
			*lZBuffer = lOneOverZ;
		}

		lU += lUStep;
		lV += lVStep;

		lOneOverZ += (float)lOneOverZStep;

		lScreen += lScreenStep;
		lZBuffer += lZBufStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
			lZBuffer += lZBufStepExtra;
		}
	}
}

void Software3DPainter::DrawUVRGBLine8BitFast(const Vertex3DUVRGB* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis)
{
	int lScreenStep = (int)pThis->mScreen->GetPixelByteSize();
	int lScreenStepExtra;
	int lZBufStep = 1;
	int lZBufStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
		lZBufStep = -lZBufStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();
		lZBufStepExtra = pThis->mZBuffer->GetPitch();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;
		lZBufStepExtra = lZBufStep;

		lScreenStep = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();
		lZBufStep = pThis->mZBuffer->GetPitch();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned char* lScreen = (unsigned char*)pThis->mScreen->GetBuffer() +
							   lStartY * pThis->mScreen->GetPitch() + lStartX;
	unsigned char* lTextureBits = (unsigned char*)pTexture->GetBuffer();
	float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
						(lStartY - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lStartX - pThis->mZBufferX;

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();

	double lOneOverLength = 1.0f / (double)(lLength + 1);
	double lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;

	Lepra::FixedPointMath::Fixed16_16 lU = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetUOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lV = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetVOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lR = Lepra::FixedPointMath::FloatToFixed16_16((pVertices[lTop].GetROverZ() / pVertices[lTop].GetOneOverZ()) * 255.0f);

	Lepra::FixedPointMath::Fixed16_16 lUEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetUOverZ() / pVertices[lBottom].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lVEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetVOverZ() / pVertices[lBottom].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lREnd = Lepra::FixedPointMath::FloatToFixed16_16((pVertices[lBottom].GetROverZ() / pVertices[lBottom].GetOneOverZ()) * 255.0f);

	Lepra::FixedPointMath::Fixed16_16 lUStep = (lUEnd - lU) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lVStep = (lVEnd - lV) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lRStep = (lREnd - lR) / (lLength + 1);

	unsigned char* l8BitLightBuffer = pThis->m8BitLightBuffer;

	for (int i = 0; i <= lLength; i++)
	{
		*lScreen = l8BitLightBuffer[((lR >> 16) << 8) + lTextureBits[(lV >> 16) * pTexture->GetPitch() + (lU >> 16)]];
		*lZBuffer = lOneOverZ;

		lU += lUStep;
		lV += lVStep;
		lR += lRStep;

		lOneOverZ += (float)lOneOverZStep;

		lScreen += lScreenStep;
		lZBuffer += lZBufStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
			lZBuffer += lZBufStepExtra;
		}
	}
}

void Software3DPainter::DrawUVRGBLine15BitFast(const Vertex3DUVRGB* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis)
{
	int lScreenStep = 1;
	int lScreenStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;

		lScreenStep = pThis->mScreen->GetPitch();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned short* lScreen = (unsigned short*)pThis->mScreen->GetBuffer() +
							   (lStartY * pThis->mScreen->GetPitch() + lStartX);
	unsigned short* lTextureBits = (unsigned short*)pTexture->GetBuffer();

	double lZ = 1.0f / pVertices[lTop].GetOneOverZ();
	Lepra::FixedPointMath::Fixed16_16 lU = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetUOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lV = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetVOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lR = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetROverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lG = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetGOverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lB = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetBOverZ() * lZ * 255.0f);

	lZ = 1.0f / pVertices[lBottom].GetOneOverZ();
	Lepra::FixedPointMath::Fixed16_16 lUEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetUOverZ() * lZ);
	Lepra::FixedPointMath::Fixed16_16 lVEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetVOverZ() * lZ);
	Lepra::FixedPointMath::Fixed16_16 lREnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetROverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lGEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetGOverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lBEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetBOverZ() * lZ * 255.0f);

	Lepra::FixedPointMath::Fixed16_16 lUStep = (lUEnd - lU) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lVStep = (lVEnd - lV) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lRStep = (lREnd - lR) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lGStep = (lGEnd - lG) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lBStep = (lBEnd - lB) / (lLength + 1);

	for (int i = 0; i <= lLength; i++)
	{
		unsigned short lColor = lTextureBits[((lV >> 16) * pTexture->GetPitch() + (lU >> 16))];

		unsigned lRed   = ((lR * (((lColor >> 10) & 31) << 3)) >> 24) & 255;
		unsigned lGreen = ((lG * (((lColor >> 5) & 31) << 3)) >> 24) & 255;
		unsigned lBlue  = ((lB * (((lColor >> 0) & 31) << 3)) >> 24) & 255;

		*lScreen = (Lepra::uint16)(((lRed >> 3) << 10) | ((lGreen >> 3) << 5) | (lBlue >> 3));

		lU += lUStep;
		lV += lVStep;
		lR += lRStep;
		lG += lGStep;
		lB += lBStep;

		lScreen += lScreenStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
		}
	}
}

void Software3DPainter::DrawUVRGBLine16BitFast(const Vertex3DUVRGB* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis)
{
	int lScreenStep = 1;
	int lScreenStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;

		lScreenStep = pThis->mScreen->GetPitch();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned short* lScreen = (unsigned short*)pThis->mScreen->GetBuffer() +
							   (lStartY * pThis->mScreen->GetPitch() + lStartX);
	unsigned short* lTextureBits = (unsigned short*)pTexture->GetBuffer();

	double lZ = 1.0f / pVertices[lTop].GetOneOverZ();
	Lepra::FixedPointMath::Fixed16_16 lU = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetUOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lV = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetVOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lR = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetROverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lG = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetGOverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lB = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetBOverZ() * lZ * 255.0f);

	lZ = 1.0f / pVertices[lBottom].GetOneOverZ();
	Lepra::FixedPointMath::Fixed16_16 lUEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetUOverZ() * lZ);
	Lepra::FixedPointMath::Fixed16_16 lVEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetVOverZ() * lZ);
	Lepra::FixedPointMath::Fixed16_16 lREnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetROverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lGEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetGOverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lBEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetBOverZ() * lZ * 255.0f);

	Lepra::FixedPointMath::Fixed16_16 lUStep = (lUEnd - lU) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lVStep = (lVEnd - lV) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lRStep = (lREnd - lR) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lGStep = (lGEnd - lG) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lBStep = (lBEnd - lB) / (lLength + 1);

	for (int i = 0; i <= lLength; i++)
	{
		unsigned short lColor = lTextureBits[((lV >> 16) * pTexture->GetPitch() + (lU >> 16))];

		unsigned lRed   = ((lR * (((lColor >> 11) & 31) << 3)) >> 24) & 255;
		unsigned lGreen = ((lG * (((lColor >> 5) & 63) << 2)) >> 24) & 255;
		unsigned lBlue  = ((lB * (((lColor >> 0) & 31) << 3)) >> 24) & 255;

		*lScreen = (Lepra::uint16)(((lRed >> 3) << 11) | ((lGreen >> 2) << 5) | (lBlue >> 3));

		lU += lUStep;
		lV += lVStep;
		lR += lRStep;
		lG += lGStep;
		lB += lBStep;

		lScreen += lScreenStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
		}
	}
}

void Software3DPainter::DrawUVRGBLine32BitFast(const Vertex3DUVRGB* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis)
{
	int lScreenStep = (int)pThis->mScreen->GetPixelByteSize();
	int lScreenStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;

		lScreenStep = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned char* lScreen = (unsigned char*)pThis->mScreen->GetBuffer() +
							   (lStartY * pThis->mScreen->GetPitch() + lStartX) * pThis->mScreen->GetPixelByteSize();
	unsigned char* lTextureBits = (unsigned char*)pTexture->GetBuffer();

	double lZ = 1.0f / pVertices[lTop].GetOneOverZ();
	Lepra::FixedPointMath::Fixed16_16 lU = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetUOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lV = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetVOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lR = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetROverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lG = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetGOverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lB = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetBOverZ() * lZ * 255.0f);

	lZ = 1.0f / pVertices[lBottom].GetOneOverZ();
	Lepra::FixedPointMath::Fixed16_16 lUEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetUOverZ() * lZ);
	Lepra::FixedPointMath::Fixed16_16 lVEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetVOverZ() * lZ);
	Lepra::FixedPointMath::Fixed16_16 lREnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetROverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lGEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetGOverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lBEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetBOverZ() * lZ * 255.0f);

	Lepra::FixedPointMath::Fixed16_16 lUStep = (lUEnd - lU) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lVStep = (lVEnd - lV) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lRStep = (lREnd - lR) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lGStep = (lGEnd - lG) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lBStep = (lBEnd - lB) / (lLength + 1);

	for (int i = 0; i <= lLength; i++)
	{
		unsigned char* lT = lTextureBits + ((lV >> 16) * pTexture->GetPitch() + (lU >> 16)) * pTexture->GetPixelByteSize();

		lScreen[0] = (Lepra::uint8)(((unsigned)lB * (unsigned)lT[0]) >> 24);
		lScreen[1] = (Lepra::uint8)(((unsigned)lG * (unsigned)lT[1]) >> 24);
		lScreen[2] = (Lepra::uint8)(((unsigned)lR * (unsigned)lT[2]) >> 24);
	
		lU += lUStep;
		lV += lVStep;
		lR += lRStep;
		lG += lGStep;
		lB += lBStep;

		lScreen += lScreenStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
		}
	}
}

void Software3DPainter::DrawUVRGBLine8BitFastZT(const Vertex3DUVRGB* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis)
{
	int lScreenStep = (int)pThis->mScreen->GetPixelByteSize();
	int lScreenStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;

		lScreenStep = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned char* lScreen = (unsigned char*)pThis->mScreen->GetBuffer() +
							   lStartY * pThis->mScreen->GetPitch() + lStartX;
	unsigned char* lTextureBits = (unsigned char*)pTexture->GetBuffer();

	Lepra::FixedPointMath::Fixed16_16 lU = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetUOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lV = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetVOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lR = Lepra::FixedPointMath::FloatToFixed16_16((pVertices[lTop].GetROverZ() / pVertices[lTop].GetOneOverZ()) * 255.0f);

	Lepra::FixedPointMath::Fixed16_16 lUEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetUOverZ() / pVertices[lBottom].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lVEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetVOverZ() / pVertices[lBottom].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lREnd = Lepra::FixedPointMath::FloatToFixed16_16((pVertices[lBottom].GetROverZ() / pVertices[lBottom].GetOneOverZ()) * 255.0f);

	Lepra::FixedPointMath::Fixed16_16 lUStep = (lUEnd - lU) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lVStep = (lVEnd - lV) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lRStep = (lREnd - lR) / (lLength + 1);

	unsigned char* l8BitLightBuffer = pThis->m8BitLightBuffer;

	for (int i = 0; i <= lLength; i++)
	{
		*lScreen = l8BitLightBuffer[((lR >> 16) << 8) + lTextureBits[(lV >> 16) * pTexture->GetPitch() + (lU >> 16)]];

		lU += lUStep;
		lV += lVStep;
		lR += lRStep;

		lScreen += lScreenStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
		}
	}
}

void Software3DPainter::DrawUVRGBLine15BitFastZT(const Vertex3DUVRGB* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis)
{
	int lScreenStep = 1;
	int lScreenStepExtra;
	int lZBufStep = 1;
	int lZBufStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
		lZBufStep = -lZBufStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch();
		lZBufStepExtra = pThis->mZBuffer->GetPitch();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;
		lZBufStepExtra = lZBufStep;

		lScreenStep = pThis->mScreen->GetPitch();
		lZBufStep = pThis->mZBuffer->GetPitch();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned short* lScreen = (unsigned short*)pThis->mScreen->GetBuffer() +
							   (lStartY * pThis->mScreen->GetPitch() + lStartX);
	unsigned short* lTextureBits = (unsigned short*)pTexture->GetBuffer();
	float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
						(lStartY - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lStartX - pThis->mZBufferX;

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();

	double lOneOverLength = 1.0f / (double)(lLength + 1);
	double lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;

	double lZ = 1.0f / pVertices[lTop].GetOneOverZ();
	Lepra::FixedPointMath::Fixed16_16 lU = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetUOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lV = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetVOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lR = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetROverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lG = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetGOverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lB = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetBOverZ() * lZ * 255.0f);

	lZ = 1.0f / pVertices[lBottom].GetOneOverZ();
	Lepra::FixedPointMath::Fixed16_16 lUEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetUOverZ() * lZ);
	Lepra::FixedPointMath::Fixed16_16 lVEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetVOverZ() * lZ);
	Lepra::FixedPointMath::Fixed16_16 lREnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetROverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lGEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetGOverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lBEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetBOverZ() * lZ * 255.0f);

	Lepra::FixedPointMath::Fixed16_16 lUStep = (lUEnd - lU) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lVStep = (lVEnd - lV) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lRStep = (lREnd - lR) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lGStep = (lGEnd - lG) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lBStep = (lBEnd - lB) / (lLength + 1);

	for (int i = 0; i <= lLength; i++)
	{
		if (lOneOverZ > *lZBuffer)
		{
			unsigned short lColor = lTextureBits[((lV >> 16) * pTexture->GetPitch() + (lU >> 16))];

			unsigned lRed   = ((lR * (((lColor >> 10) & 31) << 3)) >> 24) & 255;
			unsigned lGreen = ((lG * (((lColor >> 5) & 31) << 3)) >> 24) & 255;
			unsigned lBlue  = ((lB * (((lColor >> 0) & 31) << 3)) >> 24) & 255;

			*lScreen = (Lepra::uint16)(((lRed >> 3) << 10) | ((lGreen >> 3) << 5) | (lBlue >> 3));
		}

		lU += lUStep;
		lV += lVStep;
		lR += lRStep;
		lG += lGStep;
		lB += lBStep;

		lOneOverZ += (float)lOneOverZStep;

		lScreen += lScreenStep;
		lZBuffer += lZBufStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
			lZBuffer += lZBufStepExtra;
		}
	}
}

void Software3DPainter::DrawUVRGBLine16BitFastZT(const Vertex3DUVRGB* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis)
{
	int lScreenStep = 1;
	int lScreenStepExtra;
	int lZBufStep = 1;
	int lZBufStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
		lZBufStep = -lZBufStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch();
		lZBufStepExtra = pThis->mZBuffer->GetPitch();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;
		lZBufStepExtra = lZBufStep;

		lScreenStep = pThis->mScreen->GetPitch();
		lZBufStep = pThis->mZBuffer->GetPitch();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned short* lScreen = (unsigned short*)pThis->mScreen->GetBuffer() +
							   (lStartY * pThis->mScreen->GetPitch() + lStartX);
	unsigned short* lTextureBits = (unsigned short*)pTexture->GetBuffer();
	float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
						(lStartY - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lStartX - pThis->mZBufferX;

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();

	double lOneOverLength = 1.0f / (double)(lLength + 1);
	double lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;

	double lZ = 1.0f / pVertices[lTop].GetOneOverZ();
	Lepra::FixedPointMath::Fixed16_16 lU = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetUOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lV = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetVOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lR = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetROverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lG = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetGOverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lB = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetBOverZ() * lZ * 255.0f);

	lZ = 1.0f / pVertices[lBottom].GetOneOverZ();
	Lepra::FixedPointMath::Fixed16_16 lUEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetUOverZ() * lZ);
	Lepra::FixedPointMath::Fixed16_16 lVEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetVOverZ() * lZ);
	Lepra::FixedPointMath::Fixed16_16 lREnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetROverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lGEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetGOverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lBEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetBOverZ() * lZ * 255.0f);

	Lepra::FixedPointMath::Fixed16_16 lUStep = (lUEnd - lU) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lVStep = (lVEnd - lV) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lRStep = (lREnd - lR) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lGStep = (lGEnd - lG) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lBStep = (lBEnd - lB) / (lLength + 1);

	for (int i = 0; i <= lLength; i++)
	{
		if (lOneOverZ > *lZBuffer)
		{
			unsigned short lColor = lTextureBits[((lV >> 16) * pTexture->GetPitch() + (lU >> 16))];

			unsigned lRed   = ((lR * (((lColor >> 11) & 31) << 3)) >> 24) & 255;
			unsigned lGreen = ((lG * (((lColor >> 5) & 63) << 2)) >> 24) & 255;
			unsigned lBlue  = ((lB * (((lColor >> 0) & 31) << 3)) >> 24) & 255;

			*lScreen = (Lepra::uint16)(((lRed >> 3) << 11) | ((lGreen >> 2) << 5) | (lBlue >> 3));
		}

		lU += lUStep;
		lV += lVStep;
		lR += lRStep;
		lG += lGStep;
		lB += lBStep;

		lOneOverZ += (float)lOneOverZStep;

		lScreen += lScreenStep;
		lZBuffer += lZBufStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
			lZBuffer += lZBufStepExtra;
		}
	}
}

void Software3DPainter::DrawUVRGBLine32BitFastZT(const Vertex3DUVRGB* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis)
{
	int lScreenStep = (int)pThis->mScreen->GetPixelByteSize();
	int lScreenStepExtra;
	int lZBufStep = 1;
	int lZBufStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
		lZBufStep = -lZBufStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();
		lZBufStepExtra = pThis->mZBuffer->GetPitch();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;
		lZBufStepExtra = lZBufStep;

		lScreenStep = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();
		lZBufStep = pThis->mZBuffer->GetPitch();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned char* lScreen = (unsigned char*)pThis->mScreen->GetBuffer() +
							   (lStartY * pThis->mScreen->GetPitch() + lStartX) * pThis->mScreen->GetPixelByteSize();
	unsigned char* lTextureBits = (unsigned char*)pTexture->GetBuffer();
	float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
						(lStartY - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lStartX - pThis->mZBufferX;

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();

	double lOneOverLength = 1.0f / (double)(lLength + 1);
	double lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;

	double lZ = 1.0f / pVertices[lTop].GetOneOverZ();
	Lepra::FixedPointMath::Fixed16_16 lU = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetUOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lV = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetVOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lR = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetROverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lG = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetGOverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lB = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetBOverZ() * lZ * 255.0f);

	lZ = 1.0f / pVertices[lBottom].GetOneOverZ();
	Lepra::FixedPointMath::Fixed16_16 lUEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetUOverZ() * lZ);
	Lepra::FixedPointMath::Fixed16_16 lVEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetVOverZ() * lZ);
	Lepra::FixedPointMath::Fixed16_16 lREnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetROverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lGEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetGOverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lBEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetBOverZ() * lZ * 255.0f);

	Lepra::FixedPointMath::Fixed16_16 lUStep = (lUEnd - lU) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lVStep = (lVEnd - lV) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lRStep = (lREnd - lR) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lGStep = (lGEnd - lG) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lBStep = (lBEnd - lB) / (lLength + 1);

	for (int i = 0; i <= lLength; i++)
	{
		if (lOneOverZ > *lZBuffer)
		{
			unsigned char* lT = lTextureBits + ((lV >> 16) * pTexture->GetPitch() + (lU >> 16)) * pTexture->GetPixelByteSize();

			lScreen[0] = (Lepra::uint8)(((unsigned)lB * (unsigned)lT[0]) >> 24);
			lScreen[1] = (Lepra::uint8)(((unsigned)lG * (unsigned)lT[1]) >> 24);
			lScreen[2] = (Lepra::uint8)(((unsigned)lR * (unsigned)lT[2]) >> 24);
		}

		lU += lUStep;
		lV += lVStep;
		lR += lRStep;
		lG += lGStep;
		lB += lBStep;

		lOneOverZ += (float)lOneOverZStep;

		lScreen += lScreenStep;
		lZBuffer += lZBufStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
			lZBuffer += lZBufStepExtra;
		}
	}
}

void Software3DPainter::DrawUVRGBLine8BitFastZW(const Vertex3DUVRGB* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis)
{
	int lScreenStep = (int)pThis->mScreen->GetPixelByteSize();
	int lScreenStepExtra;
	int lZBufStep = 1;
	int lZBufStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
		lZBufStep = -lZBufStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();
		lZBufStepExtra = pThis->mZBuffer->GetPitch();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;
		lZBufStepExtra = lZBufStep;

		lScreenStep = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();
		lZBufStep = pThis->mZBuffer->GetPitch();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned char* lScreen = (unsigned char*)pThis->mScreen->GetBuffer() +
							   lStartY * pThis->mScreen->GetPitch() + lStartX;
	unsigned char* lTextureBits = (unsigned char*)pTexture->GetBuffer();
	float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
						(lStartY - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lStartX - pThis->mZBufferX;

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();

	double lOneOverLength = 1.0f / (double)(lLength + 1);
	double lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;

	Lepra::FixedPointMath::Fixed16_16 lU = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetUOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lV = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetVOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lR = Lepra::FixedPointMath::FloatToFixed16_16((pVertices[lTop].GetROverZ() / pVertices[lTop].GetOneOverZ()) * 255.0f);

	Lepra::FixedPointMath::Fixed16_16 lUEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetUOverZ() / pVertices[lBottom].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lVEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetVOverZ() / pVertices[lBottom].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lREnd = Lepra::FixedPointMath::FloatToFixed16_16((pVertices[lBottom].GetROverZ() / pVertices[lBottom].GetOneOverZ()) * 255.0f);

	Lepra::FixedPointMath::Fixed16_16 lUStep = (lUEnd - lU) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lVStep = (lVEnd - lV) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lRStep = (lREnd - lR) / (lLength + 1);

	unsigned char* l8BitLightBuffer = pThis->m8BitLightBuffer;

	for (int i = 0; i <= lLength; i++)
	{
		*lScreen = l8BitLightBuffer[((lR >> 16) << 8) + lTextureBits[(lV >> 16) * pTexture->GetPitch() + (lU >> 16)]];
		*lZBuffer = lOneOverZ;

		lU += lUStep;
		lV += lVStep;
		lR += lRStep;

		lOneOverZ += (float)lOneOverZStep;

		lScreen += lScreenStep;
		lZBuffer += lZBufStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
			lZBuffer += lZBufStepExtra;
		}
	}
}

void Software3DPainter::DrawUVRGBLine15BitFastZW(const Vertex3DUVRGB* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis)
{
	int lScreenStep = 1;
	int lScreenStepExtra;
	int lZBufStep = 1;
	int lZBufStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
		lZBufStep = -lZBufStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch();
		lZBufStepExtra = pThis->mZBuffer->GetPitch();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;
		lZBufStepExtra = lZBufStep;

		lScreenStep = pThis->mScreen->GetPitch();
		lZBufStep = pThis->mZBuffer->GetPitch();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned short* lScreen = (unsigned short*)pThis->mScreen->GetBuffer() +
							   (lStartY * pThis->mScreen->GetPitch() + lStartX);
	unsigned short* lTextureBits = (unsigned short*)pTexture->GetBuffer();
	float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
						(lStartY - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lStartX - pThis->mZBufferX;

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();

	double lOneOverLength = 1.0f / (double)(lLength + 1);
	double lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;

	double lZ = 1.0f / pVertices[lTop].GetOneOverZ();
	Lepra::FixedPointMath::Fixed16_16 lU = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetUOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lV = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetVOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lR = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetROverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lG = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetGOverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lB = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetBOverZ() * lZ * 255.0f);

	lZ = 1.0f / pVertices[lBottom].GetOneOverZ();
	Lepra::FixedPointMath::Fixed16_16 lUEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetUOverZ() * lZ);
	Lepra::FixedPointMath::Fixed16_16 lVEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetVOverZ() * lZ);
	Lepra::FixedPointMath::Fixed16_16 lREnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetROverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lGEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetGOverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lBEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetBOverZ() * lZ * 255.0f);

	Lepra::FixedPointMath::Fixed16_16 lUStep = (lUEnd - lU) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lVStep = (lVEnd - lV) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lRStep = (lREnd - lR) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lGStep = (lGEnd - lG) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lBStep = (lBEnd - lB) / (lLength + 1);

	for (int i = 0; i <= lLength; i++)
	{
		unsigned short lColor = lTextureBits[((lV >> 16) * pTexture->GetPitch() + (lU >> 16))];

		unsigned lRed   = ((lR * (((lColor >> 10) & 31) << 3)) >> 24) & 255;
		unsigned lGreen = ((lG * (((lColor >> 5) & 31) << 3)) >> 24) & 255;
		unsigned lBlue  = ((lB * (((lColor >> 0) & 31) << 3)) >> 24) & 255;

		*lScreen = (Lepra::uint16)(((lRed >> 3) << 10) | ((lGreen >> 3) << 5) | (lBlue >> 3));
		*lZBuffer = lOneOverZ;

		lU += lUStep;
		lV += lVStep;
		lR += lRStep;
		lG += lGStep;
		lB += lBStep;

		lOneOverZ += (float)lOneOverZStep;

		lScreen += lScreenStep;
		lZBuffer += lZBufStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
			lZBuffer += lZBufStepExtra;
		}
	}
}

void Software3DPainter::DrawUVRGBLine16BitFastZW(const Vertex3DUVRGB* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis)
{
	int lScreenStep = 1;
	int lScreenStepExtra;
	int lZBufStep = 1;
	int lZBufStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
		lZBufStep = -lZBufStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch();
		lZBufStepExtra = pThis->mZBuffer->GetPitch();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;
		lZBufStepExtra = lZBufStep;

		lScreenStep = pThis->mScreen->GetPitch();
		lZBufStep = pThis->mZBuffer->GetPitch();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned short* lScreen = (unsigned short*)pThis->mScreen->GetBuffer() +
							   (lStartY * pThis->mScreen->GetPitch() + lStartX);
	unsigned short* lTextureBits = (unsigned short*)pTexture->GetBuffer();
	float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
						(lStartY - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lStartX - pThis->mZBufferX;

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();

	double lOneOverLength = 1.0f / (double)(lLength + 1);
	double lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;

	double lZ = 1.0f / pVertices[lTop].GetOneOverZ();
	Lepra::FixedPointMath::Fixed16_16 lU = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetUOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lV = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetVOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lR = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetROverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lG = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetGOverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lB = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetBOverZ() * lZ * 255.0f);

	lZ = 1.0f / pVertices[lBottom].GetOneOverZ();
	Lepra::FixedPointMath::Fixed16_16 lUEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetUOverZ() * lZ);
	Lepra::FixedPointMath::Fixed16_16 lVEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetVOverZ() * lZ);
	Lepra::FixedPointMath::Fixed16_16 lREnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetROverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lGEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetGOverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lBEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetBOverZ() * lZ * 255.0f);

	Lepra::FixedPointMath::Fixed16_16 lUStep = (lUEnd - lU) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lVStep = (lVEnd - lV) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lRStep = (lREnd - lR) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lGStep = (lGEnd - lG) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lBStep = (lBEnd - lB) / (lLength + 1);

	for (int i = 0; i <= lLength; i++)
	{
		unsigned short lColor = lTextureBits[((lV >> 16) * pTexture->GetPitch() + (lU >> 16))];

		unsigned lRed   = ((lR * (((lColor >> 11) & 31) << 3)) >> 24) & 255;
		unsigned lGreen = ((lG * (((lColor >> 5) & 63) << 2)) >> 24) & 255;
		unsigned lBlue  = ((lB * (((lColor >> 0) & 31) << 3)) >> 24) & 255;

		*lScreen = (Lepra::uint16)(((lRed >> 3) << 11) | ((lGreen >> 2) << 5) | (lBlue >> 3));
		*lZBuffer = lOneOverZ;

		lU += lUStep;
		lV += lVStep;
		lR += lRStep;
		lG += lGStep;
		lB += lBStep;

		lOneOverZ += (float)lOneOverZStep;

		lScreen += lScreenStep;
		lZBuffer += lZBufStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
			lZBuffer += lZBufStepExtra;
		}
	}
}

void Software3DPainter::DrawUVRGBLine32BitFastZW(const Vertex3DUVRGB* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis)
{
	int lScreenStep = (int)pThis->mScreen->GetPixelByteSize();
	int lScreenStepExtra;
	int lZBufStep = 1;
	int lZBufStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
		lZBufStep = -lZBufStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();
		lZBufStepExtra = pThis->mZBuffer->GetPitch();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;
		lZBufStepExtra = lZBufStep;

		lScreenStep = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();
		lZBufStep = pThis->mZBuffer->GetPitch();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned char* lScreen = (unsigned char*)pThis->mScreen->GetBuffer() +
							   (lStartY * pThis->mScreen->GetPitch() + lStartX) * pThis->mScreen->GetPixelByteSize();
	unsigned char* lTextureBits = (unsigned char*)pTexture->GetBuffer();
	float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
						(lStartY - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lStartX - pThis->mZBufferX;

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();

	double lOneOverLength = 1.0f / (double)(lLength + 1);
	double lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;

	double lZ = 1.0f / pVertices[lTop].GetOneOverZ();
	Lepra::FixedPointMath::Fixed16_16 lU = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetUOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lV = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetVOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lR = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetROverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lG = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetGOverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lB = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetBOverZ() * lZ * 255.0f);

	lZ = 1.0f / pVertices[lBottom].GetOneOverZ();
	Lepra::FixedPointMath::Fixed16_16 lUEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetUOverZ() * lZ);
	Lepra::FixedPointMath::Fixed16_16 lVEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetVOverZ() * lZ);
	Lepra::FixedPointMath::Fixed16_16 lREnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetROverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lGEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetGOverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lBEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetBOverZ() * lZ * 255.0f);

	Lepra::FixedPointMath::Fixed16_16 lUStep = (lUEnd - lU) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lVStep = (lVEnd - lV) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lRStep = (lREnd - lR) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lGStep = (lGEnd - lG) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lBStep = (lBEnd - lB) / (lLength + 1);

	for (int i = 0; i <= lLength; i++)
	{
		unsigned char* lT = lTextureBits + ((lV >> 16) * pTexture->GetPitch() + (lU >> 16)) * pTexture->GetPixelByteSize();

		lScreen[0] = (Lepra::uint8)(((unsigned)lB * (unsigned)lT[0]) >> 24);
		lScreen[1] = (Lepra::uint8)(((unsigned)lG * (unsigned)lT[1]) >> 24);
		lScreen[2] = (Lepra::uint8)(((unsigned)lR * (unsigned)lT[2]) >> 24);
		*lZBuffer = lOneOverZ;

		lU += lUStep;
		lV += lVStep;
		lR += lRStep;
		lG += lGStep;
		lB += lBStep;

		lOneOverZ += (float)lOneOverZStep;

		lScreen += lScreenStep;
		lZBuffer += lZBufStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
			lZBuffer += lZBufStepExtra;
		}
	}
}

void Software3DPainter::DrawUVRGBLine8BitFastZTZW(const Vertex3DUVRGB* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis)
{
	int lScreenStep = (int)pThis->mScreen->GetPixelByteSize();
	int lScreenStepExtra;
	int lZBufStep = 1;
	int lZBufStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
		lZBufStep = -lZBufStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();
		lZBufStepExtra = pThis->mZBuffer->GetPitch();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;
		lZBufStepExtra = lZBufStep;

		lScreenStep = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();
		lZBufStep = pThis->mZBuffer->GetPitch();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned char* lScreen = (unsigned char*)pThis->mScreen->GetBuffer() +
							   lStartY * pThis->mScreen->GetPitch() + lStartX;
	unsigned char* lTextureBits = (unsigned char*)pTexture->GetBuffer();
	float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
						(lStartY - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lStartX - pThis->mZBufferX;

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();

	double lOneOverLength = 1.0f / (double)(lLength + 1);
	double lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;

	Lepra::FixedPointMath::Fixed16_16 lU = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetUOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lV = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetVOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lR = Lepra::FixedPointMath::FloatToFixed16_16((pVertices[lTop].GetROverZ() / pVertices[lTop].GetOneOverZ()) * 255.0f);

	Lepra::FixedPointMath::Fixed16_16 lUEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetUOverZ() / pVertices[lBottom].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lVEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetVOverZ() / pVertices[lBottom].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lREnd = Lepra::FixedPointMath::FloatToFixed16_16((pVertices[lBottom].GetROverZ() / pVertices[lBottom].GetOneOverZ()) * 255.0f);

	Lepra::FixedPointMath::Fixed16_16 lUStep = (lUEnd - lU) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lVStep = (lVEnd - lV) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lRStep = (lREnd - lR) / (lLength + 1);

	unsigned char* l8BitLightBuffer = pThis->m8BitLightBuffer;

	for (int i = 0; i <= lLength; i++)
	{
		if (lOneOverZ > *lZBuffer)
		{
			*lScreen = l8BitLightBuffer[((lR >> 16) << 8) + lTextureBits[(lV >> 16) * pTexture->GetPitch() + (lU >> 16)]];
			*lZBuffer = lOneOverZ;
		}

		lU += lUStep;
		lV += lVStep;
		lR += lRStep;

		lOneOverZ += (float)lOneOverZStep;

		lScreen += lScreenStep;
		lZBuffer += lZBufStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
			lZBuffer += lZBufStepExtra;
		}
	}
}

void Software3DPainter::DrawUVRGBLine15BitFastZTZW(const Vertex3DUVRGB* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis)
{
	int lScreenStep = 1;
	int lScreenStepExtra;
	int lZBufStep = 1;
	int lZBufStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
		lZBufStep = -lZBufStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch();
		lZBufStepExtra = pThis->mZBuffer->GetPitch();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;
		lZBufStepExtra = lZBufStep;

		lScreenStep = pThis->mScreen->GetPitch();
		lZBufStep = pThis->mZBuffer->GetPitch();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned short* lScreen = (unsigned short*)pThis->mScreen->GetBuffer() +
							   (lStartY * pThis->mScreen->GetPitch() + lStartX);
	unsigned short* lTextureBits = (unsigned short*)pTexture->GetBuffer();
	float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
						(lStartY - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lStartX - pThis->mZBufferX;

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();

	double lOneOverLength = 1.0f / (double)(lLength + 1);
	double lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;

	double lZ = 1.0f / pVertices[lTop].GetOneOverZ();
	Lepra::FixedPointMath::Fixed16_16 lU = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetUOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lV = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetVOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lR = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetROverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lG = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetGOverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lB = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetBOverZ() * lZ * 255.0f);

	lZ = 1.0f / pVertices[lBottom].GetOneOverZ();
	Lepra::FixedPointMath::Fixed16_16 lUEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetUOverZ() * lZ);
	Lepra::FixedPointMath::Fixed16_16 lVEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetVOverZ() * lZ);
	Lepra::FixedPointMath::Fixed16_16 lREnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetROverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lGEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetGOverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lBEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetBOverZ() * lZ * 255.0f);

	Lepra::FixedPointMath::Fixed16_16 lUStep = (lUEnd - lU) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lVStep = (lVEnd - lV) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lRStep = (lREnd - lR) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lGStep = (lGEnd - lG) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lBStep = (lBEnd - lB) / (lLength + 1);

	for (int i = 0; i <= lLength; i++)
	{
		if (lOneOverZ > *lZBuffer)
		{
			unsigned short lColor = lTextureBits[((lV >> 16) * pTexture->GetPitch() + (lU >> 16))];

			unsigned lRed   = ((lR * (((lColor >> 10) & 31) << 3)) >> 24) & 255;
			unsigned lGreen = ((lG * (((lColor >> 5) & 31) << 3)) >> 24) & 255;
			unsigned lBlue  = ((lB * (((lColor >> 0) & 31) << 3)) >> 24) & 255;

			*lScreen = (Lepra::uint16)(((lRed >> 3) << 10) | ((lGreen >> 3) << 5) | (lBlue >> 3));
			*lZBuffer = lOneOverZ;
		}

		lU += lUStep;
		lV += lVStep;
		lR += lRStep;
		lG += lGStep;
		lB += lBStep;

		lOneOverZ += (float)lOneOverZStep;

		lScreen += lScreenStep;
		lZBuffer += lZBufStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
			lZBuffer += lZBufStepExtra;
		}
	}
}

void Software3DPainter::DrawUVRGBLine16BitFastZTZW(const Vertex3DUVRGB* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis)
{
	int lScreenStep = 1;
	int lScreenStepExtra;
	int lZBufStep = 1;
	int lZBufStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
		lZBufStep = -lZBufStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch();
		lZBufStepExtra = pThis->mZBuffer->GetPitch();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;
		lZBufStepExtra = lZBufStep;

		lScreenStep = pThis->mScreen->GetPitch();
		lZBufStep = pThis->mZBuffer->GetPitch();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned short* lScreen = (unsigned short*)pThis->mScreen->GetBuffer() +
							   (lStartY * pThis->mScreen->GetPitch() + lStartX);
	unsigned short* lTextureBits = (unsigned short*)pTexture->GetBuffer();
	float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
						(lStartY - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lStartX - pThis->mZBufferX;

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();

	double lOneOverLength = 1.0f / (double)(lLength + 1);
	double lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;

	double lZ = 1.0f / pVertices[lTop].GetOneOverZ();
	Lepra::FixedPointMath::Fixed16_16 lU = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetUOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lV = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetVOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lR = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetROverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lG = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetGOverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lB = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetBOverZ() * lZ * 255.0f);

	lZ = 1.0f / pVertices[lBottom].GetOneOverZ();
	Lepra::FixedPointMath::Fixed16_16 lUEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetUOverZ() * lZ);
	Lepra::FixedPointMath::Fixed16_16 lVEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetVOverZ() * lZ);
	Lepra::FixedPointMath::Fixed16_16 lREnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetROverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lGEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetGOverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lBEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetBOverZ() * lZ * 255.0f);

	Lepra::FixedPointMath::Fixed16_16 lUStep = (lUEnd - lU) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lVStep = (lVEnd - lV) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lRStep = (lREnd - lR) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lGStep = (lGEnd - lG) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lBStep = (lBEnd - lB) / (lLength + 1);

	for (int i = 0; i <= lLength; i++)
	{
		if (lOneOverZ > *lZBuffer)
		{
			unsigned short lColor = lTextureBits[((lV >> 16) * pTexture->GetPitch() + (lU >> 16))];

			unsigned lRed   = ((lR * (((lColor >> 11) & 31) << 3)) >> 24) & 255;
			unsigned lGreen = ((lG * (((lColor >> 5) & 63) << 2)) >> 24) & 255;
			unsigned lBlue  = ((lB * (((lColor >> 0) & 31) << 3)) >> 24) & 255;

			*lScreen = (Lepra::uint16)(((lRed >> 3) << 11) | ((lGreen >> 2) << 5) | (lBlue >> 3));
			*lZBuffer = lOneOverZ;
		}

		lU += lUStep;
		lV += lVStep;
		lR += lRStep;
		lG += lGStep;
		lB += lBStep;

		lOneOverZ += (float)lOneOverZStep;

		lScreen += lScreenStep;
		lZBuffer += lZBufStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
			lZBuffer += lZBufStepExtra;
		}
	}
}

void Software3DPainter::DrawUVRGBLine32BitFastZTZW(const Vertex3DUVRGB* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis)
{
	int lScreenStep = (int)pThis->mScreen->GetPixelByteSize();
	int lScreenStepExtra;
	int lZBufStep = 1;
	int lZBufStepExtra;

	int lTop	  = 0;
	int lBottom = 1;

	if (pVertices[1].GetY28_4() < pVertices[0].GetY28_4())
	{
		lTop    = 1;
		lBottom = 0;
	}

	int lStartY = ((pVertices[lTop].GetY28_4() + 8) >> 4);
	int lStartX = ((pVertices[lTop].GetX28_4() + 8) >> 4);

	int lDy = ((pVertices[lBottom].GetY28_4() + 8) >> 4) - lStartY;
	int lDx = ((pVertices[lBottom].GetX28_4() + 8) >> 4) - lStartX;

	if (lDx < 0)
	{
		lDx = -lDx;
		lScreenStep = -lScreenStep;
		lZBufStep = -lZBufStep;
	}

	int lLength = 0;
	int lErrorAdd;
	int lErrorCmp;
	int lError = 0;

	if (lDx > lDy)
	{
		lScreenStepExtra = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();
		lZBufStepExtra = pThis->mZBuffer->GetPitch();

		lLength = lDx;

		lErrorAdd = lDy;
		lErrorCmp = lDx;
	}
	else
	{
		lScreenStepExtra = lScreenStep;
		lZBufStepExtra = lZBufStep;

		lScreenStep = pThis->mScreen->GetPitch() * pThis->mScreen->GetPixelByteSize();
		lZBufStep = pThis->mZBuffer->GetPitch();

		lLength = lDy;

		lErrorAdd = lDx;
		lErrorCmp = lDy;
	}

	unsigned char* lScreen = (unsigned char*)pThis->mScreen->GetBuffer() +
							   (lStartY * pThis->mScreen->GetPitch() + lStartX) * pThis->mScreen->GetPixelByteSize();
	unsigned char* lTextureBits = (unsigned char*)pTexture->GetBuffer();
	float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
						(lStartY - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lStartX - pThis->mZBufferX;

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();

	double lOneOverLength = 1.0f / (double)(lLength + 1);
	double lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;

	double lZ = 1.0f / pVertices[lTop].GetOneOverZ();
	Lepra::FixedPointMath::Fixed16_16 lU = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetUOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lV = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetVOverZ() / pVertices[lTop].GetOneOverZ());
	Lepra::FixedPointMath::Fixed16_16 lR = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetROverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lG = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetGOverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lB = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lTop].GetBOverZ() * lZ * 255.0f);

	lZ = 1.0f / pVertices[lBottom].GetOneOverZ();
	Lepra::FixedPointMath::Fixed16_16 lUEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetUOverZ() * lZ);
	Lepra::FixedPointMath::Fixed16_16 lVEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetVOverZ() * lZ);
	Lepra::FixedPointMath::Fixed16_16 lREnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetROverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lGEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetGOverZ() * lZ * 255.0f);
	Lepra::FixedPointMath::Fixed16_16 lBEnd = Lepra::FixedPointMath::FloatToFixed16_16(pVertices[lBottom].GetBOverZ() * lZ * 255.0f);

	Lepra::FixedPointMath::Fixed16_16 lUStep = (lUEnd - lU) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lVStep = (lVEnd - lV) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lRStep = (lREnd - lR) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lGStep = (lGEnd - lG) / (lLength + 1);
	Lepra::FixedPointMath::Fixed16_16 lBStep = (lBEnd - lB) / (lLength + 1);

	for (int i = 0; i <= lLength; i++)
	{
		if (lOneOverZ > *lZBuffer)
		{
			unsigned char* lT = lTextureBits + ((lV >> 16) * pTexture->GetPitch() + (lU >> 16)) * pTexture->GetPixelByteSize();

			lScreen[0] = (Lepra::uint8)(((unsigned)lB * (unsigned)lT[0]) >> 24);
			lScreen[1] = (Lepra::uint8)(((unsigned)lG * (unsigned)lT[1]) >> 24);
			lScreen[2] = (Lepra::uint8)(((unsigned)lR * (unsigned)lT[2]) >> 24);
			*lZBuffer = lOneOverZ;
		}

		lU += lUStep;
		lV += lVStep;
		lR += lRStep;
		lG += lGStep;
		lB += lBStep;

		lOneOverZ += (float)lOneOverZStep;

		lScreen += lScreenStep;
		lZBuffer += lZBufStep;

		lError += lErrorAdd;

		if (lError >= lErrorCmp)
		{
			lError -= lErrorCmp;
			lScreen += lScreenStepExtra;
			lZBuffer += lZBufStepExtra;
		}
	}
}

} // End namespace.
