/*
	Lepra::File:   Software3DPainter_ScanLineNicest.cpp
	Class:  Software3DPainter
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand
*/

///////////////////////////////////////////////////////////////////////////////////
//	
//	The nicest scan line renderers.
//	
///////////////////////////////////////////////////////////////////////////////////

#include "../../Include/UiTBC.h"
#include "../../Include/UiSoftware3DPainter.h"
#include "../../Include/UiGradients.h"
#include "../../Include/UiEdge.h"
#include "../../../Lepra/Include/GammaLookup.h"
#include <math.h>

namespace UiTbc
{

void Software3DPainter::DrawScanLineSingleColor8BitNicest(const Gradients* pGradients,
														  EdgeZ* pLeft, 
														  EdgeZ* pRight,
														  const Lepra::Color& pColor,
														  Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		unsigned char lColor = pColor.mRed;

		float lOneOverZ = (float)pLeft->GetOneOverZ();

		unsigned char* lDestBits = (unsigned char*)pThis->mScreen->GetBuffer() +
									 pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart;
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		bool lZBufferTest = pThis->CheckFlag(FLAG_ZTEST_ACTIVE);
		bool lZBufferWrite = pThis->CheckFlag(FLAG_ZWRITE_ACTIVE);

		while (lWidth--)
		{
			if (lZBufferTest == false || lOneOverZ > *lZBuffer)
			{
				pThis->Write8BitColor(lDestBits, lColor, pThis->mAlpha);

				if (lZBufferWrite == true)
				{
					*lZBuffer = lOneOverZ;
				}
			}

			lDestBits++;
			lZBuffer++;

			lOneOverZ += (float)pGradients->GetOneOverZXStep();
		}
	}
}

void Software3DPainter::DrawScanLineSingleColorNicest(const Gradients* pGradients,
														 EdgeZ* pLeft, 
														 EdgeZ* pRight,
														 const Lepra::Color& pColor,
														 Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		float lOneOverZ = (float)pLeft->GetOneOverZ();

		unsigned char* lDestBits = (unsigned char*)pThis->mScreen->GetBuffer() +
									 (pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart) * pThis->mScreen->GetPixelByteSize();
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		bool lZBufferTest = pThis->CheckFlag(FLAG_ZTEST_ACTIVE);
		bool lZBufferWrite = pThis->CheckFlag(FLAG_ZWRITE_ACTIVE);

		while (lWidth--)
		{
			if (lZBufferTest == false || lOneOverZ > *lZBuffer)
			{
				pThis->WriteColor(lDestBits, pColor.mRed, pColor.mGreen, pColor.mBlue, pThis->mAlpha, pThis);

				if (lZBufferWrite == true)
				{
					*lZBuffer = lOneOverZ;
				}
			}

			lDestBits += pThis->mScreen->GetPixelByteSize();
			lZBuffer++;

			lOneOverZ += (float)pGradients->GetOneOverZXStep();
		}
	}
}


void Software3DPainter::DrawScanLineRGB8BitNicest(const GradientsRGB* pGradients, 
														  EdgeRGB* pLeft, 
														  EdgeRGB* pRight,
														  Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		float lOneOverZ = (float)pLeft->GetOneOverZ();
		float lROverZ = pLeft->GetROverZ();

		unsigned char* lDestBits = (unsigned char*)pThis->mScreen->GetBuffer() +
									 pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart;
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		bool lZBufferTest = pThis->CheckFlag(FLAG_ZTEST_ACTIVE);
		bool lZBufferWrite = pThis->CheckFlag(FLAG_ZWRITE_ACTIVE);

		while (lWidth--)
		{
			if (lZBufferTest == false || lOneOverZ > *lZBuffer)
			{
				float lZ = 1.0f / lOneOverZ;

				pThis->Write8BitColor(lDestBits, Lepra::GammaLookup::LinearToGamma((unsigned short)floor(lROverZ * lZ)), pThis->mAlpha);

				if (lZBufferWrite == true)
				{
					*lZBuffer = lOneOverZ;
				}
			}

			lDestBits++;
			lZBuffer++;

			lOneOverZ += (float)pGradients->GetOneOverZXStep();
			lROverZ += pGradients->GetROverZXStep();
		}
	}
}

void Software3DPainter::DrawScanLineRGBNicest(const GradientsRGB* pGradients, 
												 EdgeRGB* pLeft, 
												 EdgeRGB* pRight,
												 Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		float lOneOverZ = (float)pLeft->GetOneOverZ();
		float lROverZ = pLeft->GetROverZ();
		float lGOverZ = pLeft->GetGOverZ();
		float lBOverZ = pLeft->GetBOverZ();

		unsigned char* lDestBits = (unsigned char*)pThis->mScreen->GetBuffer() +
									 (pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart) * pThis->mScreen->GetPixelByteSize();
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		bool lZBufferTest = pThis->CheckFlag(FLAG_ZTEST_ACTIVE);
		bool lZBufferWrite = pThis->CheckFlag(FLAG_ZWRITE_ACTIVE);

		while (lWidth--)
		{
			if (lZBufferTest == false || lOneOverZ > *lZBuffer)
			{
				float lZ = 1.0f / lOneOverZ;

				int lRed   = (unsigned int)floor(lROverZ * lZ);
				int lGreen = (unsigned int)floor(lGOverZ * lZ);
				int lBlue  = (unsigned int)floor(lBOverZ * lZ);

				if (lRed < 0)
				{
					lRed = 0;
				}
				if (lGreen < 0)
				{
					lGreen = 0;
				}
				if (lBlue < 0)
				{
					lBlue = 0;
				}

				if (lRed > 0xFFFF)
				{
					lRed = 0xFFFF;
				}
				if (lGreen > 0xFFFF)
				{
					lGreen = 0xFFFF;
				}
				if (lBlue > 0xFFFF)
				{
					lBlue = 0xFFFF;
				}

				lRed   = Lepra::GammaLookup::LinearToGamma((Lepra::uint16)lRed);
				lGreen = Lepra::GammaLookup::LinearToGamma((Lepra::uint16)lGreen);
				lBlue  = Lepra::GammaLookup::LinearToGamma((Lepra::uint16)lBlue);

				pThis->WriteColor(lDestBits, (Lepra::uint8)lRed, (Lepra::uint8)lGreen, (Lepra::uint8)lBlue, pThis->mAlpha, pThis);

				if (lZBufferWrite == true)
				{
					*lZBuffer = lOneOverZ;
				}
			}

			lDestBits += pThis->mScreen->GetPixelByteSize();
			lZBuffer++;

			lOneOverZ += (float)pGradients->GetOneOverZXStep();
			lROverZ += pGradients->GetROverZXStep();
			lGOverZ += pGradients->GetGOverZXStep();
			lBOverZ += pGradients->GetBOverZXStep();
		}
	}
}

void Software3DPainter::DrawScanLineUV8BitNicest(const GradientsUV* pGradients, 
														  EdgeUV* pLeft, 
														  EdgeUV* pRight,
														  Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		float lOneOverZ = (float)pLeft->GetOneOverZ();
		float lUOverZ = pLeft->GetUOverZ();
		float lVOverZ = pLeft->GetVOverZ();

		unsigned char* lDestBits = (unsigned char*)pThis->mScreen->GetBuffer() +
									 pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart;
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		bool lZBufferTest = pThis->CheckFlag(FLAG_ZTEST_ACTIVE);
		bool lZBufferWrite = pThis->CheckFlag(FLAG_ZWRITE_ACTIVE);

		while (lWidth--)
		{
			if (lZBufferTest == false || lOneOverZ > *lZBuffer)
			{
				float lZ = 1.0f / lOneOverZ;

				Lepra::Color lColor;
				pThis->GetTextureColor(lUOverZ * lZ, lVOverZ * lZ, 0, pThis, lColor);

				pThis->Write8BitColor(lDestBits, lColor.mRed, lColor.mAlpha);

				if (lZBufferWrite == true)
				{
					*lZBuffer = lOneOverZ;
				}
			}

			lDestBits++;
			lZBuffer++;

			lOneOverZ += (float)pGradients->GetOneOverZXStep();
			lUOverZ += pGradients->GetUOverZXStep();
			lVOverZ += pGradients->GetVOverZXStep();
		}
	}
}

void Software3DPainter::DrawScanLineUVNicest(const GradientsUV* pGradients, 
												EdgeUV* pLeft, 
												EdgeUV* pRight,
												Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		float lOneOverZ = (float)pLeft->GetOneOverZ();
		float lUOverZ = pLeft->GetUOverZ();
		float lVOverZ = pLeft->GetVOverZ();

		unsigned char* lDestBits = (unsigned char*)pThis->mScreen->GetBuffer() +
									 (pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart) * pThis->mScreen->GetPixelByteSize();
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		bool lZBufferTest = pThis->CheckFlag(FLAG_ZTEST_ACTIVE);
		bool lZBufferWrite = pThis->CheckFlag(FLAG_ZWRITE_ACTIVE);

		while (lWidth--)
		{
			if (lZBufferTest == false || lOneOverZ > *lZBuffer)
			{
				float lZ = 1.0f / lOneOverZ;

				Lepra::Color lColor;
				pThis->GetTextureColor(lUOverZ * lZ, lVOverZ * lZ, 0, pThis, lColor);

				pThis->WriteColor(lDestBits, lColor.mRed, lColor.mGreen, lColor.mBlue, lColor.mAlpha, pThis);

				if (lZBufferWrite == true)
				{
					*lZBuffer = lOneOverZ;
				}
			}

			lDestBits += pThis->mScreen->GetPixelByteSize();
			lZBuffer++;

			lOneOverZ += (float)pGradients->GetOneOverZXStep();
			lUOverZ += pGradients->GetUOverZXStep();
			lVOverZ += pGradients->GetVOverZXStep();
		}
	}
}


void Software3DPainter::DrawScanLineUVRGB8BitNicest(const GradientsUVRGB* pGradients, 
														  EdgeUVRGB* pLeft, 
														  EdgeUVRGB* pRight,
														  Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		float lOneOverZ = (float)pLeft->GetOneOverZ();
		float lUOverZ = pLeft->GetUOverZ();
		float lVOverZ = pLeft->GetVOverZ();
		float lROverZ = pLeft->GetROverZ();
		float lGOverZ = pLeft->GetGOverZ();
		float lBOverZ = pLeft->GetBOverZ();

		unsigned char* lDestBits = (unsigned char*)pThis->mScreen->GetBuffer() +
									 pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart;
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		bool lZBufferTest = pThis->CheckFlag(FLAG_ZTEST_ACTIVE);
		bool lZBufferWrite = pThis->CheckFlag(FLAG_ZWRITE_ACTIVE);

		while (lWidth--)
		{
			if (lZBufferTest == false || lOneOverZ > *lZBuffer)
			{
				float lZ = 1.0f / lOneOverZ;

				Lepra::Color lColor;
				pThis->GetTextureColor(lUOverZ * lZ, lVOverZ * lZ, 0, pThis, lColor);

				int lLight = Lepra::GammaLookup::LinearToGamma((unsigned short)floor(lROverZ * lZ * 0.3f + lGOverZ * lZ * 0.59f + lBOverZ * lZ * 0.11f));

				pThis->Write8BitColor(lDestBits, pThis->m8BitLightBuffer[(lLight << 8) + lColor.mRed], lColor.mAlpha);

				if (lZBufferWrite == true)
				{
					*lZBuffer = lOneOverZ;
				}
			}

			lDestBits++;
			lZBuffer++;

			lOneOverZ += (float)pGradients->GetOneOverZXStep();
			lUOverZ += pGradients->GetUOverZXStep();
			lVOverZ += pGradients->GetVOverZXStep();
			lROverZ += pGradients->GetROverZXStep();
			lGOverZ += pGradients->GetGOverZXStep();
			lBOverZ += pGradients->GetBOverZXStep();
		}
	}
}

void Software3DPainter::DrawScanLineUVRGBNicest(const GradientsUVRGB* pGradients, 
												   EdgeUVRGB* pLeft, 
												   EdgeUVRGB* pRight,
												   Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		float lOneOverZ = (float)pLeft->GetOneOverZ();
		float lUOverZ = pLeft->GetUOverZ();
		float lVOverZ = pLeft->GetVOverZ();
		float lROverZ = pLeft->GetROverZ();
		float lGOverZ = pLeft->GetGOverZ();
		float lBOverZ = pLeft->GetBOverZ();

		unsigned char* lDestBits = (unsigned char*)pThis->mScreen->GetBuffer() +
									 (pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart) * pThis->mScreen->GetPixelByteSize();
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		bool lZBufferTest = pThis->CheckFlag(FLAG_ZTEST_ACTIVE);
		bool lZBufferWrite = pThis->CheckFlag(FLAG_ZWRITE_ACTIVE);

		while (lWidth--)
		{
			if (lZBufferTest == false || lOneOverZ > *lZBuffer)
			{
				float lZ = 1.0f / lOneOverZ;
				Lepra::Color lColor;
				pThis->GetTextureColor(lUOverZ * lZ, lVOverZ * lZ, 0, pThis, lColor);

				unsigned int lColRed   = (unsigned int)floor(lROverZ * lZ);
				unsigned int lColGreen = (unsigned int)floor(lGOverZ * lZ);
				unsigned int lColBlue  = (unsigned int)floor(lBOverZ * lZ);

				if (lColRed < 0)
				{
					lColRed = 0;
				}
				if (lColGreen < 0)
				{
					lColGreen = 0;
				}
				if (lColBlue < 0)
				{
					lColBlue = 0;
				}

				if (lColRed > 0xFFFF)
				{
					lColRed = 0xFFFF;
				}
				if (lColGreen > 0xFFFF)
				{
					lColGreen = 0xFFFF;
				}
				if (lColBlue > 0xFFFF)
				{
					lColBlue = 0xFFFF;
				}

				unsigned int lRed   = (lColRed   * (unsigned int)Lepra::GammaLookup::GammaToLinear(lColor.mRed)) >> 16;
				unsigned int lGreen = (lColGreen * (unsigned int)Lepra::GammaLookup::GammaToLinear(lColor.mGreen)) >> 16;
				unsigned int lBlue  = (lColBlue  * (unsigned int)Lepra::GammaLookup::GammaToLinear(lColor.mBlue)) >> 16;

				lRed   = ((lColRed   * pThis->mSpecularity) >> 16) + ((lRed   * (0xFFFF - pThis->mSpecularity)) >> 16);
				lGreen = ((lColGreen * pThis->mSpecularity) >> 16) + ((lGreen * (0xFFFF - pThis->mSpecularity)) >> 16);
				lBlue  = ((lColBlue  * pThis->mSpecularity) >> 16) + ((lBlue  * (0xFFFF - pThis->mSpecularity)) >> 16);

				lRed   = Lepra::GammaLookup::LinearToGamma((Lepra::uint16)lRed);
				lGreen = Lepra::GammaLookup::LinearToGamma((Lepra::uint16)lGreen);
				lBlue  = Lepra::GammaLookup::LinearToGamma((Lepra::uint16)lBlue);

				pThis->WriteColor(lDestBits, (Lepra::uint8)lRed, (Lepra::uint8)lGreen, (Lepra::uint8)lBlue, lColor.mAlpha, pThis);

				if (lZBufferWrite == true)
				{
					*lZBuffer = lOneOverZ;
				}
			}

			lDestBits += pThis->mScreen->GetPixelByteSize();
			lZBuffer++;

			lOneOverZ += (float)pGradients->GetOneOverZXStep();
			lUOverZ += pGradients->GetUOverZXStep();
			lVOverZ += pGradients->GetVOverZXStep();
			lROverZ += pGradients->GetROverZXStep();
			lGOverZ += pGradients->GetGOverZXStep();
			lBOverZ += pGradients->GetBOverZXStep();
		}
	}
}


void Software3DPainter::DrawScanLineUVM8BitNicest(const GradientsUVM* pGradients, 
														  EdgeUVM* pLeft, 
														  EdgeUVM* pRight,
														  Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		float lOneOverZ = (float)pLeft->GetOneOverZ();
		float lUOverZ = pLeft->GetUOverZ();
		float lVOverZ = pLeft->GetVOverZ();
		float lMOverZ = pLeft->GetMOverZ();

		unsigned char* lDestBits = (unsigned char*)pThis->mScreen->GetBuffer() +
									 pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart;
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		bool lZBufferTest = pThis->CheckFlag(FLAG_ZTEST_ACTIVE);
		bool lZBufferWrite = pThis->CheckFlag(FLAG_ZWRITE_ACTIVE);

		while (lWidth--)
		{
			if (lZBufferTest == false || lOneOverZ > *lZBuffer)
			{
				float lZ = 1.0f / lOneOverZ;

				Lepra::Color lColor;
				pThis->GetTextureColor(lUOverZ * lZ, 
										 lVOverZ * lZ, 
										 lMOverZ * lZ, pThis, lColor);

				pThis->Write8BitColor(lDestBits, lColor.mRed, lColor.mAlpha);

				if (lZBufferWrite == true)
				{
					*lZBuffer = lOneOverZ;
				}
			}

			lDestBits++;
			lZBuffer++;

			lOneOverZ += (float)pGradients->GetOneOverZXStep();
			lUOverZ += pGradients->GetUOverZXStep();
			lVOverZ += pGradients->GetVOverZXStep();
			lMOverZ += pGradients->GetMOverZXStep();
		}
	}
}

void Software3DPainter::DrawScanLineUVMNicest(const GradientsUVM* pGradients, 
												 EdgeUVM* pLeft, 
												 EdgeUVM* pRight,
												 Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		float lOneOverZ = (float)pLeft->GetOneOverZ();
		float lUOverZ = pLeft->GetUOverZ();
		float lVOverZ = pLeft->GetVOverZ();
		float lMOverZ = pLeft->GetMOverZ();

		unsigned char* lDestBits = (unsigned char*)pThis->mScreen->GetBuffer() +
									 (pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart) * pThis->mScreen->GetPixelByteSize();
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		bool lZBufferTest = pThis->CheckFlag(FLAG_ZTEST_ACTIVE);
		bool lZBufferWrite = pThis->CheckFlag(FLAG_ZWRITE_ACTIVE);

		while (lWidth--)
		{
			if (lZBufferTest == false || lOneOverZ > *lZBuffer)
			{
				float lZ = 1.0f / lOneOverZ;

				Lepra::Color lColor;
				pThis->GetTextureColor(lUOverZ * lZ, 
										 lVOverZ * lZ, 
										 lMOverZ * lZ, pThis, lColor);

				pThis->WriteColor(lDestBits, lColor.mRed, lColor.mGreen, lColor.mBlue, lColor.mAlpha, pThis);

				if (lZBufferWrite == true)
				{
					*lZBuffer = lOneOverZ;
				}
			}

			lDestBits += pThis->mScreen->GetPixelByteSize();
			lZBuffer++;

			lOneOverZ += (float)pGradients->GetOneOverZXStep();
			lUOverZ += pGradients->GetUOverZXStep();
			lVOverZ += pGradients->GetVOverZXStep();
			lMOverZ += pGradients->GetMOverZXStep();
		}
	}
}


void Software3DPainter::DrawScanLineUVRGBM8BitNicest(const GradientsUVRGBM* pGradients, 
														  EdgeUVRGBM* pLeft, 
														  EdgeUVRGBM* pRight,
														  Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		float lOneOverZ = (float)pLeft->GetOneOverZ();
		float lUOverZ = pLeft->GetUOverZ();
		float lVOverZ = pLeft->GetVOverZ();
		float lROverZ = pLeft->GetROverZ();
		float lGOverZ = pLeft->GetGOverZ();
		float lBOverZ = pLeft->GetBOverZ();
		float lMOverZ = pLeft->GetMOverZ();

		unsigned char* lDestBits = (unsigned char*)pThis->mScreen->GetBuffer() +
									 pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart;
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		bool lZBufferTest = pThis->CheckFlag(FLAG_ZTEST_ACTIVE);
		bool lZBufferWrite = pThis->CheckFlag(FLAG_ZWRITE_ACTIVE);

		while (lWidth--)
		{
			if (lZBufferTest == false || lOneOverZ > *lZBuffer)
			{
				float lZ = 1.0f / lOneOverZ;

				Lepra::Color lColor;
				pThis->GetTextureColor(lUOverZ * lZ, lVOverZ * lZ, lMOverZ * lZ, pThis, lColor);

				int lLight = Lepra::GammaLookup::LinearToGamma((unsigned short)floor(lROverZ * lZ * 0.3f + lGOverZ * lZ * 0.59f + lBOverZ * lZ * 0.11f));

				pThis->Write8BitColor(lDestBits, pThis->m8BitLightBuffer[(lLight << 8) + lColor.mRed], lColor.mAlpha);

				if (lZBufferWrite == true)
				{
					*lZBuffer = lOneOverZ;
				}
			}

			lDestBits++;
			lZBuffer++;

			lOneOverZ += (float)pGradients->GetOneOverZXStep();
			lUOverZ += pGradients->GetUOverZXStep();
			lVOverZ += pGradients->GetVOverZXStep();
			lROverZ += pGradients->GetROverZXStep();
			lGOverZ += pGradients->GetGOverZXStep();
			lBOverZ += pGradients->GetBOverZXStep();
			lMOverZ += pGradients->GetMOverZXStep();
		}
	}
}

void Software3DPainter::DrawScanLineUVRGBMNicest(const GradientsUVRGBM* pGradients, 
													EdgeUVRGBM* pLeft, 
													EdgeUVRGBM* pRight,
													Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		float lOneOverZ = (float)pLeft->GetOneOverZ();
		float lUOverZ = pLeft->GetUOverZ();
		float lVOverZ = pLeft->GetVOverZ();
		float lROverZ = pLeft->GetROverZ();
		float lGOverZ = pLeft->GetGOverZ();
		float lBOverZ = pLeft->GetBOverZ();
		float lMOverZ = pLeft->GetMOverZ();

		unsigned char* lDestBits = (unsigned char*)pThis->mScreen->GetBuffer() +
									 (pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart) * pThis->mScreen->GetPixelByteSize();
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		bool lZBufferTest = pThis->CheckFlag(FLAG_ZTEST_ACTIVE);
		bool lZBufferWrite = pThis->CheckFlag(FLAG_ZWRITE_ACTIVE);

		while (lWidth--)
		{
			if (lZBufferTest == false || lOneOverZ > *lZBuffer)
			{
				float lZ = 1.0f / lOneOverZ;

				Lepra::Color lColor;
				pThis->GetTextureColor(lUOverZ * lZ, 
										 lVOverZ * lZ, 
										 lMOverZ * lZ, pThis, lColor);

				int lColRed   = (unsigned int)floor(lROverZ * lZ + 0.5f);
				int lColGreen = (unsigned int)floor(lGOverZ * lZ + 0.5f);
				int lColBlue  = (unsigned int)floor(lBOverZ * lZ + 0.5f);

				if (lColRed < 0)
				{
					lColRed = 0;
				}
				if (lColGreen < 0)
				{
					lColGreen = 0;
				}
				if (lColBlue < 0)
				{
					lColBlue = 0;
				}

				if (lColRed > 0xFFFF)
				{
					lColRed = 0xFFFF;
				}
				if (lColGreen > 0xFFFF)
				{
					lColGreen = 0xFFFF;
				}
				if (lColBlue > 0xFFFF)
				{
					lColBlue = 0xFFFF;
				}

				unsigned int lRed   = (lColRed   * (unsigned int)Lepra::GammaLookup::GammaToLinear(lColor.mRed)) >> 16;
				unsigned int lGreen = (lColGreen * (unsigned int)Lepra::GammaLookup::GammaToLinear(lColor.mGreen)) >> 16;
				unsigned int lBlue  = (lColBlue  * (unsigned int)Lepra::GammaLookup::GammaToLinear(lColor.mBlue)) >> 16;

				lRed   = ((lColRed   * pThis->mSpecularity) >> 16) + ((lRed   * (0xFFFF - pThis->mSpecularity)) >> 16);
				lGreen = ((lColGreen * pThis->mSpecularity) >> 16) + ((lGreen * (0xFFFF - pThis->mSpecularity)) >> 16);
				lBlue  = ((lColBlue  * pThis->mSpecularity) >> 16) + ((lBlue  * (0xFFFF - pThis->mSpecularity)) >> 16);

				lRed   = Lepra::GammaLookup::LinearToGamma((Lepra::uint16)lRed);
				lGreen = Lepra::GammaLookup::LinearToGamma((Lepra::uint16)lGreen);
				lBlue  = Lepra::GammaLookup::LinearToGamma((Lepra::uint16)lBlue);

				pThis->WriteColor(lDestBits, (Lepra::uint8)lRed, (Lepra::uint8)lGreen, (Lepra::uint8)lBlue, lColor.mAlpha, pThis);

				if (lZBufferWrite == true)
				{
					*lZBuffer = lOneOverZ;
				}
			}

			lDestBits += pThis->mScreen->GetPixelByteSize();
			lZBuffer++;

			lOneOverZ += (float)pGradients->GetOneOverZXStep();
			lUOverZ += pGradients->GetUOverZXStep();
			lVOverZ += pGradients->GetVOverZXStep();
			lROverZ += pGradients->GetROverZXStep();
			lGOverZ += pGradients->GetGOverZXStep();
			lBOverZ += pGradients->GetBOverZXStep();
			lMOverZ += pGradients->GetMOverZXStep();
		}
	}
}

} // End namespace.
