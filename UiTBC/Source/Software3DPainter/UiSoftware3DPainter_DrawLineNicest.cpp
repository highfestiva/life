/*
	Lepra::File:   Software3DPainter_DrawLineNicest.cpp
	Class:  Software3DPainter
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand
*/

#include "../../Include/UiTBC.h"
#include "../../Include/UiSoftware3DPainter.h"
#include "../../Include/UiVertex.h"
#include "../../../Lepra/Include/FixedPointMath.h"
#include "../../../Lepra/Include/GammaLookup.h"

#include <math.h>

namespace UiTbc
{

void Software3DPainter::DrawSingleColorLine8BitNicest(const Vertex3D* pVertices, const Lepra::Color& pColor, Software3DPainter* pThis)
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
	int lError = 0;;

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

	bool lZBufferTest = pThis->CheckFlag(FLAG_ZTEST_ACTIVE);
	bool lZBufferWrite = pThis->CheckFlag(FLAG_ZWRITE_ACTIVE);

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();

	float lOneOverLength = 1.0f / (float)(lLength + 1);
	float lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;

	for (int i = 0; i <= lLength; i++)
	{
		if (lZBufferTest == false || lOneOverZ > *lZBuffer)
		{
			pThis->Write8BitColor(lScreen, pColor.mRed, pThis->mAlpha);
		}

		if (lZBufferWrite == true)
		{
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

void Software3DPainter::DrawSingleColorLineNicest(const Vertex3D* pVertices, const Lepra::Color& pColor, Software3DPainter* pThis)
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
	int lError = 0;;

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
							   (lStartY * pThis->mScreen->GetPitch() + lStartX) * (int)pThis->mScreen->GetPixelByteSize();
	float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
						(lStartY - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lStartX - pThis->mZBufferX;

	bool lZBufferTest = pThis->CheckFlag(FLAG_ZTEST_ACTIVE);
	bool lZBufferWrite = pThis->CheckFlag(FLAG_ZWRITE_ACTIVE);

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();

	float lOneOverLength = 1.0f / (float)(lLength + 1);
	float lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;

	for (int i = 0; i <= lLength; i++)
	{
		if (lZBufferTest == false || lOneOverZ > *lZBuffer)
		{
			pThis->WriteColor(lScreen, pColor.mRed, pColor.mGreen, pColor.mBlue, pThis->mAlpha, pThis);
			if (lZBufferWrite == true)
			{
				*lZBuffer = lOneOverZ;
			}
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

void Software3DPainter::DrawRGBLine8BitNicest(const Vertex3DRGB* pVertices, Software3DPainter* pThis)
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
	int lError = 0;;

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

	bool lZBufferTest = pThis->CheckFlag(FLAG_ZTEST_ACTIVE);
	bool lZBufferWrite = pThis->CheckFlag(FLAG_ZWRITE_ACTIVE);

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();
	float lROverZ = pVertices[lTop].GetROverZ();

	float lOneOverLength = 1.0f / (float)(lLength + 1);
	float lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;
	float lROverZStep = (pVertices[lBottom].GetROverZ() - pVertices[lTop].GetROverZ()) * lOneOverLength;

	for (int i = 0; i <= lLength; i++)
	{
		if (lZBufferTest == false || lOneOverZ > *lZBuffer)
		{
			float lZ = 1.0f / lOneOverZ;
			Lepra::uint8 lRed = (Lepra::uint8)(lROverZ * lZ * 255.0f);
			pThis->Write8BitColor(lScreen, lRed, pThis->mAlpha);

			if (lZBufferWrite == true)
			{
				*lZBuffer = lOneOverZ;
			}
		}

		lOneOverZ += (float)lOneOverZStep;
		lROverZ += lROverZStep;

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

void Software3DPainter::DrawRGBLineNicest(const Vertex3DRGB* pVertices, Software3DPainter* pThis)
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

	bool lZBufferTest = pThis->CheckFlag(FLAG_ZTEST_ACTIVE);
	bool lZBufferWrite = pThis->CheckFlag(FLAG_ZWRITE_ACTIVE);

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();
	float lROverZ = pVertices[lTop].GetROverZ();
	float lGOverZ = pVertices[lTop].GetGOverZ();
	float lBOverZ = pVertices[lTop].GetBOverZ();

	float lOneOverLength = 1.0f / (float)(lLength + 1);
	float lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;
	float lROverZStep = (pVertices[lBottom].GetROverZ() - pVertices[lTop].GetROverZ()) * lOneOverLength;
	float lGOverZStep = (pVertices[lBottom].GetGOverZ() - pVertices[lTop].GetGOverZ()) * lOneOverLength;
	float lBOverZStep = (pVertices[lBottom].GetBOverZ() - pVertices[lTop].GetBOverZ()) * lOneOverLength;

	for (int i = 0; i <= lLength; i++)
	{
		if (lZBufferTest == false || lOneOverZ > *lZBuffer)
		{
			float lZ = 1.0f / lOneOverZ;
			Lepra::uint8 lRed   = (Lepra::uint8)(lROverZ * lZ * 255.0f);
			Lepra::uint8 lGreen = (Lepra::uint8)(lGOverZ * lZ * 255.0f);
			Lepra::uint8 lBlue  = (Lepra::uint8)(lBOverZ * lZ * 255.0f);
			pThis->WriteColor(lScreen, lRed, lGreen, lBlue, pThis->mAlpha, pThis);

			if (lZBufferWrite == true)
			{
				*lZBuffer = lOneOverZ;
			}
		}

		lOneOverZ += (float)lOneOverZStep;
		lROverZ += lROverZStep;
		lGOverZ += lGOverZStep;
		lBOverZ += lBOverZStep;

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

void Software3DPainter::DrawUVLine8BitNicest(const Vertex3DUV* pVertices, Software3DPainter* pThis)
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
	int lError = 0;;

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

	bool lZBufferTest = pThis->CheckFlag(FLAG_ZTEST_ACTIVE);
	bool lZBufferWrite = pThis->CheckFlag(FLAG_ZWRITE_ACTIVE);

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();
	float lUOverZ = pVertices[lTop].GetUOverZ();
	float lVOverZ = pVertices[lTop].GetVOverZ();

	float lOneOverLength = 1.0f / (float)(lLength + 1);
	float lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;
	float lUOverZStep = (pVertices[lBottom].GetUOverZ() - pVertices[lTop].GetUOverZ()) * lOneOverLength;
	float lVOverZStep = (pVertices[lBottom].GetVOverZ() - pVertices[lTop].GetVOverZ()) * lOneOverLength;

	for (int i = 0; i <= lLength; i++)
	{
		if (lZBufferTest == false || lOneOverZ > *lZBuffer)
		{
			Lepra::Color lCol;

			float lZ = 1.0f / lOneOverZ;
			pThis->GetTextureColor(lUOverZ * lZ, lVOverZ * lZ, 0, pThis, lCol);

			pThis->Write8BitColor(lScreen, lCol.mRed, lCol.mAlpha);

			if (lZBufferWrite == true)
			{
				*lZBuffer = lOneOverZ;
			}
		}

		lOneOverZ += (float)lOneOverZStep;
		lUOverZ += lUOverZStep;
		lVOverZ += lVOverZStep;

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

void Software3DPainter::DrawUVLineNicest(const Vertex3DUV* pVertices, Software3DPainter* pThis)
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
	int lError = 0;;

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

	bool lZBufferTest = pThis->CheckFlag(FLAG_ZTEST_ACTIVE);
	bool lZBufferWrite = pThis->CheckFlag(FLAG_ZWRITE_ACTIVE);

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();
	float lUOverZ = pVertices[lTop].GetUOverZ();
	float lVOverZ = pVertices[lTop].GetVOverZ();

	float lOneOverLength = 1.0f / (float)(lLength + 1);
	float lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;
	float lUOverZStep = (pVertices[lBottom].GetUOverZ() - pVertices[lTop].GetUOverZ()) * lOneOverLength;
	float lVOverZStep = (pVertices[lBottom].GetVOverZ() - pVertices[lTop].GetVOverZ()) * lOneOverLength;

	for (int i = 0; i <= lLength; i++)
	{
		if (lZBufferTest == false || lOneOverZ > *lZBuffer)
		{
			Lepra::Color lCol;

			float lZ = 1.0f / lOneOverZ;
			pThis->GetTextureColor(lUOverZ * lZ, lVOverZ * lZ, 0, pThis, lCol);

			pThis->WriteColor(lScreen, lCol.mRed, lCol.mGreen, lCol.mBlue, lCol.mAlpha, pThis);

			if (lZBufferWrite == true)
			{
				*lZBuffer = lOneOverZ;
			}
		}

		lOneOverZ += (float)lOneOverZStep;
		lUOverZ += lUOverZStep;
		lVOverZ += lVOverZStep;

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

void Software3DPainter::DrawUVRGBLine8BitNicest(const Vertex3DUVRGB* pVertices, Software3DPainter* pThis)
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
	int lError = 0;;

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

	bool lZBufferTest = pThis->CheckFlag(FLAG_ZTEST_ACTIVE);
	bool lZBufferWrite = pThis->CheckFlag(FLAG_ZWRITE_ACTIVE);

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();
	float lUOverZ = pVertices[lTop].GetUOverZ();
	float lVOverZ = pVertices[lTop].GetVOverZ();
	float lROverZ = pVertices[lTop].GetROverZ();
	float lGOverZ = pVertices[lTop].GetGOverZ();
	float lBOverZ = pVertices[lTop].GetBOverZ();

	float lOneOverLength = 1.0f / (float)(lLength + 1);
	float lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;
	float lUOverZStep = (pVertices[lBottom].GetUOverZ() - pVertices[lTop].GetUOverZ()) * lOneOverLength;
	float lVOverZStep = (pVertices[lBottom].GetVOverZ() - pVertices[lTop].GetVOverZ()) * lOneOverLength;
	float lROverZStep = (pVertices[lBottom].GetROverZ() - pVertices[lTop].GetROverZ()) * lOneOverLength;
	float lGOverZStep = (pVertices[lBottom].GetGOverZ() - pVertices[lTop].GetGOverZ()) * lOneOverLength;
	float lBOverZStep = (pVertices[lBottom].GetBOverZ() - pVertices[lTop].GetBOverZ()) * lOneOverLength;

	for (int i = 0; i <= lLength; i++)
	{
		if (lZBufferTest == false || lOneOverZ > *lZBuffer)
		{
			Lepra::Color lCol;

			float lZ = 1.0f / lOneOverZ;
			pThis->GetTextureColor(lUOverZ * lZ, lVOverZ * lZ, 0, pThis, lCol);

			int lLight = Lepra::GammaLookup::LinearToGamma((unsigned short)floor((lROverZ * lZ * 0.3f + lGOverZ * lZ * 0.59f + lBOverZ * lZ * 0.11f) * 65535.0f));

			pThis->Write8BitColor(lScreen, pThis->m8BitLightBuffer[(lLight << 8) + lCol.mRed], lCol.mAlpha);

			if (lZBufferWrite == true)
			{
				*lZBuffer = lOneOverZ;
			}
		}

		lOneOverZ += (float)lOneOverZStep;
		lUOverZ += lUOverZStep;
		lVOverZ += lVOverZStep;
		lROverZ += lROverZStep;
		lGOverZ += lGOverZStep;
		lBOverZ += lBOverZStep;

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

void Software3DPainter::DrawUVRGBLineNicest(const Vertex3DUVRGB* pVertices, Software3DPainter* pThis)
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
	int lError = 0;;

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

	bool lZBufferTest = pThis->CheckFlag(FLAG_ZTEST_ACTIVE);
	bool lZBufferWrite = pThis->CheckFlag(FLAG_ZWRITE_ACTIVE);

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();
	float lUOverZ = pVertices[lTop].GetUOverZ();
	float lVOverZ = pVertices[lTop].GetVOverZ();
	float lROverZ = pVertices[lTop].GetROverZ();
	float lGOverZ = pVertices[lTop].GetGOverZ();
	float lBOverZ = pVertices[lTop].GetBOverZ();

	float lOneOverLength = 1.0f / (float)(lLength + 1);
	float lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;
	float lUOverZStep = (pVertices[lBottom].GetUOverZ() - pVertices[lTop].GetUOverZ()) * lOneOverLength;
	float lVOverZStep = (pVertices[lBottom].GetVOverZ() - pVertices[lTop].GetVOverZ()) * lOneOverLength;
	float lROverZStep = (pVertices[lBottom].GetROverZ() - pVertices[lTop].GetROverZ()) * lOneOverLength;
	float lGOverZStep = (pVertices[lBottom].GetGOverZ() - pVertices[lTop].GetGOverZ()) * lOneOverLength;
	float lBOverZStep = (pVertices[lBottom].GetBOverZ() - pVertices[lTop].GetBOverZ()) * lOneOverLength;

	for (int i = 0; i <= lLength; i++)
	{
		if (lZBufferTest == false || lOneOverZ > *lZBuffer)
		{
			Lepra::Color lColor;

			float lZ = 1.0f / lOneOverZ;
			pThis->GetTextureColor(lUOverZ * lZ, lVOverZ * lZ, 0, pThis, lColor);

			unsigned int lColRed   = (unsigned int)floor(lROverZ * lZ * 65535.0f);
			unsigned int lColGreen = (unsigned int)floor(lGOverZ * lZ * 65535.0f);
			unsigned int lColBlue  = (unsigned int)floor(lBOverZ * lZ * 65535.0f);

			unsigned int lRed   = (lColRed   * (unsigned int)Lepra::GammaLookup::GammaToLinear(lColor.mRed)) >> 16;
			unsigned int lGreen = (lColGreen * (unsigned int)Lepra::GammaLookup::GammaToLinear(lColor.mGreen)) >> 16;
			unsigned int lBlue  = (lColBlue  * (unsigned int)Lepra::GammaLookup::GammaToLinear(lColor.mBlue)) >> 16;

			lRed   = ((lColRed   * pThis->mSpecularity) >> 16) + ((lRed   * (0xFFFF - pThis->mSpecularity)) >> 16);
			lGreen = ((lColGreen * pThis->mSpecularity) >> 16) + ((lGreen * (0xFFFF - pThis->mSpecularity)) >> 16);
			lBlue  = ((lColBlue  * pThis->mSpecularity) >> 16) + ((lBlue  * (0xFFFF - pThis->mSpecularity)) >> 16);

			lRed   = Lepra::GammaLookup::LinearToGamma((Lepra::uint16)lRed);
			lGreen = Lepra::GammaLookup::LinearToGamma((Lepra::uint16)lGreen);
			lBlue  = Lepra::GammaLookup::LinearToGamma((Lepra::uint16)lBlue);

			pThis->WriteColor(lScreen, (Lepra::uint8)lRed, (Lepra::uint8)lGreen, (Lepra::uint8)lBlue, lColor.mAlpha, pThis);

			if (lZBufferWrite == true)
			{
				*lZBuffer = lOneOverZ;
			}
		}

		lOneOverZ += (float)lOneOverZStep;
		lUOverZ += lUOverZStep;
		lVOverZ += lVOverZStep;
		lROverZ += lROverZStep;
		lGOverZ += lGOverZStep;
		lBOverZ += lBOverZStep;

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

void Software3DPainter::DrawUVMLine8BitNicest(const Vertex3DUVM* pVertices, Software3DPainter* pThis)
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
	int lError = 0;;

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

	bool lZBufferTest = pThis->CheckFlag(FLAG_ZTEST_ACTIVE);
	bool lZBufferWrite = pThis->CheckFlag(FLAG_ZWRITE_ACTIVE);

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();
	float lUOverZ = pVertices[lTop].GetUOverZ();
	float lVOverZ = pVertices[lTop].GetVOverZ();
	float lMOverZ = pVertices[lTop].GetMOverZ();

	float lOneOverLength = 1.0f / (float)(lLength + 1);
	float lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;
	float lUOverZStep = (pVertices[lBottom].GetUOverZ() - pVertices[lTop].GetUOverZ()) * lOneOverLength;
	float lVOverZStep = (pVertices[lBottom].GetVOverZ() - pVertices[lTop].GetVOverZ()) * lOneOverLength;
	float lMOverZStep = (pVertices[lBottom].GetMOverZ() - pVertices[lTop].GetMOverZ()) * lOneOverLength;

	for (int i = 0; i <= lLength; i++)
	{
		if (lZBufferTest == false || lOneOverZ > *lZBuffer)
		{
			Lepra::Color lCol;

			float lZ = 1.0f / lOneOverZ;
			pThis->GetTextureColor(lUOverZ * lZ, lVOverZ * lZ, lMOverZ * lZ, pThis, lCol);

			pThis->Write8BitColor(lScreen, lCol.mRed, lCol.mAlpha);

			if (lZBufferWrite == true)
			{
				*lZBuffer = lOneOverZ;
			}
		}

		lOneOverZ += (float)lOneOverZStep;
		lUOverZ += lUOverZStep;
		lVOverZ += lVOverZStep;
		lMOverZ += lMOverZStep;

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

void Software3DPainter::DrawUVMLineNicest(const Vertex3DUVM* pVertices, Software3DPainter* pThis)
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
	int lError = 0;;

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

	bool lZBufferTest = pThis->CheckFlag(FLAG_ZTEST_ACTIVE);
	bool lZBufferWrite = pThis->CheckFlag(FLAG_ZWRITE_ACTIVE);

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();
	float lUOverZ = pVertices[lTop].GetUOverZ();
	float lVOverZ = pVertices[lTop].GetVOverZ();
	float lMOverZ = pVertices[lTop].GetMOverZ();

	float lOneOverLength = 1.0f / (float)(lLength + 1);
	float lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;
	float lUOverZStep = (pVertices[lBottom].GetUOverZ() - pVertices[lTop].GetUOverZ()) * lOneOverLength;
	float lVOverZStep = (pVertices[lBottom].GetVOverZ() - pVertices[lTop].GetVOverZ()) * lOneOverLength;
	float lMOverZStep = (pVertices[lBottom].GetMOverZ() - pVertices[lTop].GetMOverZ()) * lOneOverLength;

	for (int i = 0; i <= lLength; i++)
	{
		if (lZBufferTest == false || lOneOverZ > *lZBuffer)
		{
			Lepra::Color lCol;

			float lZ = 1.0f / lOneOverZ;
			pThis->GetTextureColor(lUOverZ * lZ, lVOverZ * lZ, lMOverZ * lZ, pThis, lCol);

			pThis->WriteColor(lScreen, lCol.mRed, lCol.mGreen, lCol.mBlue, lCol.mAlpha, pThis);

			if (lZBufferWrite == true)
			{
				*lZBuffer = lOneOverZ;
			}
		}

		lOneOverZ += (float)lOneOverZStep;
		lUOverZ += lUOverZStep;
		lVOverZ += lVOverZStep;
		lMOverZ += lMOverZStep;

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


void Software3DPainter::DrawUVRGBMLine8BitNicest(const Vertex3DUVRGBM* pVertices, Software3DPainter* pThis)
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
	int lError = 0;;

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

	bool lZBufferTest = pThis->CheckFlag(FLAG_ZTEST_ACTIVE);
	bool lZBufferWrite = pThis->CheckFlag(FLAG_ZWRITE_ACTIVE);

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();
	float lUOverZ = pVertices[lTop].GetUOverZ();
	float lVOverZ = pVertices[lTop].GetVOverZ();
	float lROverZ = pVertices[lTop].GetROverZ();
	float lGOverZ = pVertices[lTop].GetGOverZ();
	float lBOverZ = pVertices[lTop].GetBOverZ();
	float lMOverZ = pVertices[lTop].GetMOverZ();

	float lOneOverLength = 1.0f / (float)(lLength + 1);
	float lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;
	float lUOverZStep = (pVertices[lBottom].GetUOverZ() - pVertices[lTop].GetUOverZ()) * lOneOverLength;
	float lVOverZStep = (pVertices[lBottom].GetVOverZ() - pVertices[lTop].GetVOverZ()) * lOneOverLength;
	float lROverZStep = (pVertices[lBottom].GetROverZ() - pVertices[lTop].GetROverZ()) * lOneOverLength;
	float lGOverZStep = (pVertices[lBottom].GetGOverZ() - pVertices[lTop].GetGOverZ()) * lOneOverLength;
	float lBOverZStep = (pVertices[lBottom].GetBOverZ() - pVertices[lTop].GetBOverZ()) * lOneOverLength;
	float lMOverZStep = (pVertices[lBottom].GetMOverZ() - pVertices[lTop].GetMOverZ()) * lOneOverLength;

	for (int i = 0; i <= lLength; i++)
	{
		if (lZBufferTest == false || lOneOverZ > *lZBuffer)
		{
			Lepra::Color lCol;

			float lZ = 1.0f / lOneOverZ;
			pThis->GetTextureColor(lUOverZ * lZ, lVOverZ * lZ, lMOverZ * lZ, pThis, lCol);

			int lLight = Lepra::GammaLookup::LinearToGamma((unsigned short)floor((lROverZ * lZ * 0.3f + lGOverZ * lZ * 0.59f + lBOverZ * lZ * 0.11f) * 65535.0f));

			pThis->Write8BitColor(lScreen, pThis->m8BitLightBuffer[(lLight << 8) + lCol.mRed], lCol.mAlpha);

			if (lZBufferWrite == true)
			{
				*lZBuffer = lOneOverZ;
			}
		}

		lOneOverZ += (float)lOneOverZStep;
		lUOverZ += lUOverZStep;
		lVOverZ += lVOverZStep;
		lROverZ += lROverZStep;
		lGOverZ += lGOverZStep;
		lBOverZ += lBOverZStep;
		lMOverZ += lMOverZStep;

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

void Software3DPainter::DrawUVRGBMLineNicest(const Vertex3DUVRGBM* pVertices, Software3DPainter* pThis)
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
	int lError = 0;;

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

	bool lZBufferTest = pThis->CheckFlag(FLAG_ZTEST_ACTIVE);
	bool lZBufferWrite = pThis->CheckFlag(FLAG_ZWRITE_ACTIVE);

	float lOneOverZ = (float)pVertices[lTop].GetOneOverZ();
	float lUOverZ = pVertices[lTop].GetUOverZ();
	float lVOverZ = pVertices[lTop].GetVOverZ();
	float lROverZ = pVertices[lTop].GetROverZ();
	float lGOverZ = pVertices[lTop].GetGOverZ();
	float lBOverZ = pVertices[lTop].GetBOverZ();
	float lMOverZ = pVertices[lTop].GetMOverZ();

	float lOneOverLength = 1.0f / (float)(lLength + 1);
	float lOneOverZStep = (pVertices[lBottom].GetOneOverZ() - pVertices[lTop].GetOneOverZ()) * lOneOverLength;
	float lUOverZStep = (pVertices[lBottom].GetUOverZ() - pVertices[lTop].GetUOverZ()) * lOneOverLength;
	float lVOverZStep = (pVertices[lBottom].GetVOverZ() - pVertices[lTop].GetVOverZ()) * lOneOverLength;
	float lROverZStep = (pVertices[lBottom].GetROverZ() - pVertices[lTop].GetROverZ()) * lOneOverLength;
	float lGOverZStep = (pVertices[lBottom].GetGOverZ() - pVertices[lTop].GetGOverZ()) * lOneOverLength;
	float lBOverZStep = (pVertices[lBottom].GetBOverZ() - pVertices[lTop].GetBOverZ()) * lOneOverLength;
	float lMOverZStep = (pVertices[lBottom].GetMOverZ() - pVertices[lTop].GetMOverZ()) * lOneOverLength;

	for (int i = 0; i <= lLength; i++)
	{
		if (lZBufferTest == false || lOneOverZ > *lZBuffer)
		{
			Lepra::Color lColor;

			float lZ = 1.0f / lOneOverZ;
			pThis->GetTextureColor(lUOverZ * lZ, lVOverZ * lZ, lMOverZ * lZ, pThis, lColor);

			unsigned int lColRed   = (unsigned int)floor(lROverZ * lZ * 65535.0f);
			unsigned int lColGreen = (unsigned int)floor(lGOverZ * lZ * 65535.0f);
			unsigned int lColBlue  = (unsigned int)floor(lBOverZ * lZ * 65535.0f);

			unsigned int lRed   = (lColRed   * (unsigned int)Lepra::GammaLookup::GammaToLinear(lColor.mRed)) >> 16;
			unsigned int lGreen = (lColGreen * (unsigned int)Lepra::GammaLookup::GammaToLinear(lColor.mGreen)) >> 16;
			unsigned int lBlue  = (lColBlue  * (unsigned int)Lepra::GammaLookup::GammaToLinear(lColor.mBlue)) >> 16;

			lRed   = ((lColRed   * pThis->mSpecularity) >> 16) + ((lRed   * (0xFFFF - pThis->mSpecularity)) >> 16);
			lGreen = ((lColGreen * pThis->mSpecularity) >> 16) + ((lGreen * (0xFFFF - pThis->mSpecularity)) >> 16);
			lBlue  = ((lColBlue  * pThis->mSpecularity) >> 16) + ((lBlue  * (0xFFFF - pThis->mSpecularity)) >> 16);

			lRed   = Lepra::GammaLookup::LinearToGamma((Lepra::uint16)lRed);
			lGreen = Lepra::GammaLookup::LinearToGamma((Lepra::uint16)lGreen);
			lBlue  = Lepra::GammaLookup::LinearToGamma((Lepra::uint16)lBlue);

			pThis->WriteColor(lScreen, (Lepra::uint8)lRed, (Lepra::uint8)lGreen, (Lepra::uint8)lBlue, lColor.mAlpha, pThis);

			if (lZBufferWrite == true)
			{
				*lZBuffer = lOneOverZ;
			}
		}

		lOneOverZ += (float)lOneOverZStep;
		lUOverZ += lUOverZStep;
		lVOverZ += lVOverZStep;
		lROverZ += lROverZStep;
		lGOverZ += lGOverZStep;
		lBOverZ += lBOverZStep;
		lMOverZ += lMOverZStep;

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
