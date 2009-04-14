/*
	Lepra::File:   Software3DPainter_ScanLineFast.cpp
	Class:  Software3DPainter
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand
*/

///////////////////////////////////////////////////////////////////////////////////
//	
//	The fast scan line renderers.
//	
///////////////////////////////////////////////////////////////////////////////////

#include "../../Include/UiTBC.h"
#include "../../Include/UiSoftware3DPainter.h"
#include "../../Include/UiGradients.h"
#include "../../Include/UiEdge.h"
#include "../../../Lepra/Include/GammaLookup.h"
#include "../../../Lepra/Include/FixedPointMath.h"
#include <math.h>
#include <memory.h>



#define MACRO_CHECK_RGB(red, green, blue, max) \
{ \
	if (red < 0.0f) \
	{ \
		red = 0.0f; \
	} \
	if (green < 0.0f) \
	{ \
		green = 0.0f; \
	} \
	if (blue < 0.0f) \
	{ \
		blue = 0.0f; \
	} \
 \
	if (red > max) \
	{ \
		red = max; \
	} \
	if (green > max) \
	{ \
		green = max; \
	} \
	if (blue > max) \
	{ \
		blue = max; \
	} \
}

#define MACRO_CHECK_RGB_16BIT(red, green, blue) \
{ \
	if (red < 0.0f) \
	{ \
		red = 0.0f; \
	} \
	if (green < 0.0f) \
	{ \
		green = 0.0f; \
	} \
	if (blue < 0.0f) \
	{ \
		blue = 0.0f; \
	} \
 \
	if (red > 31.0f) \
	{ \
		red = 31.0f; \
	} \
	if (green > 63.0f) \
	{ \
		green = 63.0f; \
	} \
	if (blue > 31.0f) \
	{ \
		blue = 31.0f; \
	} \
}


namespace UiTbc
{

///////////////////////////////////////////////////////////////////////////////////
//	
//	Single color scan line renderers.
//	
///////////////////////////////////////////////////////////////////////////////////





void Software3DPainter::DrawScanLineSingleColor8BitFast(const Gradients* /*pGradients*/, EdgeZ* pLeft,
	EdgeZ* pRight, const Lepra::Color& pColor, Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		unsigned char* lDestBits = (unsigned char*)pThis->mScreen->GetBuffer() +
									 pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart;

		memset(lDestBits, pColor.mRed, lWidth);
	}
}

void Software3DPainter::DrawScanLineSingleColor15BitFast(const Gradients* /*pGradients*/, EdgeZ* pLeft,
	EdgeZ* pRight, const Lepra::Color& pColor, Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		unsigned short lColor = ((unsigned short)(pColor.mRed   >> 3) << 10) |
								   ((unsigned short)(pColor.mGreen >> 3) << 5) |
								   ((unsigned short)(pColor.mBlue  >> 3));

		unsigned short* lDestBits = (unsigned short*)pThis->mScreen->GetBuffer() +
									 pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart;

		while (lWidth--)
		{
			*lDestBits = lColor;
			lDestBits++;
		}
	}
}

void Software3DPainter::DrawScanLineSingleColor16BitFast(const Gradients* /*pGradients*/, EdgeZ* pLeft,
	EdgeZ* pRight, const Lepra::Color& pColor, Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		unsigned short lColor = ((unsigned short)(pColor.mRed   >> 3) << 11) |
								   ((unsigned short)(pColor.mGreen >> 2) << 5) |
								   ((unsigned short)(pColor.mBlue  >> 3));

		unsigned short* lDestBits = (unsigned short*)pThis->mScreen->GetBuffer() +
									 pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart;
		while (lWidth--)
		{
			*lDestBits = lColor;
			lDestBits++;
		}
	}
}

void Software3DPainter::DrawScanLineSingleColor24BitFast(const Gradients* /*pGradients*/, EdgeZ* pLeft,
	EdgeZ* pRight, const Lepra::Color& pColor, Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		unsigned char* lDestBits = (unsigned char*)pThis->mScreen->GetBuffer() +
									 (pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart) * 3;
		while (lWidth--)
		{
			lDestBits[0] = pColor.mBlue;
			lDestBits[1] = pColor.mGreen;
			lDestBits[2] = pColor.mRed;

			lDestBits += 3;
		}
	}
}

void Software3DPainter::DrawScanLineSingleColor32BitFast(const Gradients* /*pGradients*/, EdgeZ* pLeft,
	EdgeZ* pRight, const Lepra::Color& pColor, Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		unsigned int lColor = ((unsigned int)pColor.mRed << 16) |
								 ((unsigned int)pColor.mGreen << 8) |
								 (unsigned int)pColor.mBlue;

		unsigned int* lDestBits = (unsigned int*)pThis->mScreen->GetBuffer() +
									 pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart;
		while (lWidth--)
		{
			*lDestBits = lColor;
			lDestBits++;
		}
	}
}

void Software3DPainter::DrawScanLineSingleColor8BitFastZT(const Gradients* pGradients,
														   EdgeZ* pLeft, 
														   EdgeZ* pRight,
														   const Lepra::Color& pColor,
														   Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		unsigned char* lDestBits = (unsigned char*)pThis->mScreen->GetBuffer() +
									 pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart;

		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;
		float lOneOverZ = (float)pLeft->GetOneOverZ();

		unsigned char lColor = pColor.mRed;
		while (lWidth--)
		{
			if (lOneOverZ > *lZBuffer)
			{
				*lDestBits = lColor;
			}

			lDestBits++;
			lZBuffer++;
			lOneOverZ += (float)pGradients->GetOneOverZXStep();
		}
	}
}

void Software3DPainter::DrawScanLineSingleColor15BitFastZT(const Gradients* pGradients,
															EdgeZ* pLeft, 
															EdgeZ* pRight,
															const Lepra::Color& pColor,
															Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		unsigned short lColor = ((unsigned short)(pColor.mRed   >> 3) << 10) |
								   ((unsigned short)(pColor.mGreen >> 3) << 5) |
								   ((unsigned short)(pColor.mBlue  >> 3));

		unsigned short* lDestBits = (unsigned short*)pThis->mScreen->GetBuffer() +
									 pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart;
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;
		float lOneOverZ = (float)pLeft->GetOneOverZ();

		while (lWidth--)
		{
			if (lOneOverZ > *lZBuffer)
			{
				*lDestBits = lColor;
			}

			lDestBits++;
			lZBuffer++;
			lOneOverZ += (float)pGradients->GetOneOverZXStep();
		}
	}
}

void Software3DPainter::DrawScanLineSingleColor16BitFastZT(const Gradients* pGradients,
															EdgeZ* pLeft, 
															EdgeZ* pRight,
															const Lepra::Color& pColor,
															Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		unsigned short lColor = ((unsigned short)(pColor.mRed   >> 3) << 11) |
								   ((unsigned short)(pColor.mGreen >> 2) << 5) |
								   ((unsigned short)(pColor.mBlue  >> 3));

		unsigned short* lDestBits = (unsigned short*)pThis->mScreen->GetBuffer() +
									 pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart;
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;
		float lOneOverZ = (float)pLeft->GetOneOverZ();

		while (lWidth--)
		{
			if (lOneOverZ > *lZBuffer)
			{
				*lDestBits = lColor;
			}

			lDestBits++;
			lZBuffer++;
			lOneOverZ += (float)pGradients->GetOneOverZXStep();
		}
	}
}

void Software3DPainter::DrawScanLineSingleColor24BitFastZT(const Gradients* pGradients,
															EdgeZ* pLeft, 
															EdgeZ* pRight,
															const Lepra::Color& pColor,
															Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		unsigned char* lDestBits = (unsigned char*)pThis->mScreen->GetBuffer() +
									 (pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart) * 3;
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;
		float lOneOverZ = (float)pLeft->GetOneOverZ();

		while (lWidth--)
		{
			if (lOneOverZ > *lZBuffer)
			{
				lDestBits[0] = pColor.mBlue;
				lDestBits[1] = pColor.mGreen;
				lDestBits[2] = pColor.mRed;
			}

			lDestBits += 3;
			lZBuffer++;
			lOneOverZ += (float)pGradients->GetOneOverZXStep();
		}
	}
}

void Software3DPainter::DrawScanLineSingleColor32BitFastZT(const Gradients* pGradients,
															EdgeZ* pLeft, 
															EdgeZ* pRight,
															const Lepra::Color& pColor,
															Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		unsigned int lColor = ((unsigned int)pColor.mRed << 16) |
								 ((unsigned int)pColor.mGreen << 8) |
								 (unsigned int)pColor.mBlue;

		unsigned int* lDestBits = (unsigned int*)pThis->mScreen->GetBuffer() +
									 pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart;
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;
		float lOneOverZ = (float)pLeft->GetOneOverZ();

		while (lWidth--)
		{
			if (lOneOverZ > *lZBuffer)
			{
				*lDestBits = lColor;
			}

			lDestBits++;
			lZBuffer++;
			lOneOverZ += (float)pGradients->GetOneOverZXStep();
		}
	}
}


void Software3DPainter::DrawScanLineSingleColor8BitFastZW(const Gradients* pGradients,
														   EdgeZ* pLeft, 
														   EdgeZ* pRight,
														   const Lepra::Color& pColor,
														   Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		unsigned char* lDestBits = (unsigned char*)pThis->mScreen->GetBuffer() +
									 pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart;

		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;
		float lOneOverZ = (float)pLeft->GetOneOverZ();

		unsigned char lColor = pColor.mRed;
		while (lWidth--)
		{
			*lDestBits = lColor;
			*lZBuffer = lOneOverZ;

			lDestBits++;
			lZBuffer++;
			lOneOverZ += (float)pGradients->GetOneOverZXStep();
		}
	}
}

void Software3DPainter::DrawScanLineSingleColor15BitFastZW(const Gradients* pGradients,
															EdgeZ* pLeft, 
															EdgeZ* pRight,
															const Lepra::Color& pColor,
															Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		unsigned short lColor = ((unsigned short)(pColor.mRed   >> 3) << 10) |
								   ((unsigned short)(pColor.mGreen >> 3) << 5) |
								   ((unsigned short)(pColor.mBlue  >> 3));

		unsigned short* lDestBits = (unsigned short*)pThis->mScreen->GetBuffer() +
									 pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart;
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;
		float lOneOverZ = (float)pLeft->GetOneOverZ();

		while (lWidth--)
		{
			*lDestBits = lColor;
			*lZBuffer = lOneOverZ;

			lDestBits++;
			lZBuffer++;
			lOneOverZ += (float)pGradients->GetOneOverZXStep();
		}
	}
}

void Software3DPainter::DrawScanLineSingleColor16BitFastZW(const Gradients* pGradients,
															EdgeZ* pLeft, 
															EdgeZ* pRight,
															const Lepra::Color& pColor,
															Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		unsigned short lColor = ((unsigned short)(pColor.mRed   >> 3) << 11) |
								   ((unsigned short)(pColor.mGreen >> 2) << 5) |
								   ((unsigned short)(pColor.mBlue  >> 3));

		unsigned short* lDestBits = (unsigned short*)pThis->mScreen->GetBuffer() +
									 pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart;
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;
		float lOneOverZ = (float)pLeft->GetOneOverZ();

		while (lWidth--)
		{
			*lDestBits = lColor;
			*lZBuffer = lOneOverZ;

			lDestBits++;
			lZBuffer++;
			lOneOverZ += (float)pGradients->GetOneOverZXStep();
		}
	}
}

void Software3DPainter::DrawScanLineSingleColor24BitFastZW(const Gradients* pGradients,
															EdgeZ* pLeft, 
															EdgeZ* pRight,
															const Lepra::Color& pColor,
															Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		unsigned char* lDestBits = (unsigned char*)pThis->mScreen->GetBuffer() +
									 (pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart) * 3;
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;
		float lOneOverZ = (float)pLeft->GetOneOverZ();

		while (lWidth--)
		{
			lDestBits[0] = pColor.mBlue;
			lDestBits[1] = pColor.mGreen;
			lDestBits[2] = pColor.mRed;
			*lZBuffer = lOneOverZ;

			lDestBits += 3;
			lZBuffer++;
			lOneOverZ += (float)pGradients->GetOneOverZXStep();
		}
	}
}

void Software3DPainter::DrawScanLineSingleColor32BitFastZW(const Gradients* pGradients,
															EdgeZ* pLeft, 
															EdgeZ* pRight,
															const Lepra::Color& pColor,
															Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		unsigned int lColor = ((unsigned int)pColor.mRed << 16) |
								 ((unsigned int)pColor.mGreen << 8) |
								 (unsigned int)pColor.mBlue;

		unsigned int* lDestBits = (unsigned int*)pThis->mScreen->GetBuffer() +
									 pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart;
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;
		float lOneOverZ = (float)pLeft->GetOneOverZ();

		while (lWidth--)
		{
			*lDestBits = lColor;
			*lZBuffer = lOneOverZ;

			lDestBits++;
			lZBuffer++;
			lOneOverZ += (float)pGradients->GetOneOverZXStep();
		}
	}
}



void Software3DPainter::DrawScanLineSingleColor8BitFastZTZW(const Gradients* pGradients,
														   EdgeZ* pLeft, 
														   EdgeZ* pRight,
														   const Lepra::Color& pColor,
														   Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		unsigned char* lDestBits = (unsigned char*)pThis->mScreen->GetBuffer() +
									 pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart;

		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;
		float lOneOverZ = (float)pLeft->GetOneOverZ();

		unsigned char lColor = pColor.mRed;
		while (lWidth--)
		{
			if (lOneOverZ > *lZBuffer)
			{
				*lDestBits = lColor;
				*lZBuffer = lOneOverZ;
			}

			lDestBits++;
			lZBuffer++;
			lOneOverZ += (float)pGradients->GetOneOverZXStep();
		}
	}
}

void Software3DPainter::DrawScanLineSingleColor15BitFastZTZW(const Gradients* pGradients,
															EdgeZ* pLeft, 
															EdgeZ* pRight,
															const Lepra::Color& pColor,
															Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		unsigned short lColor = ((unsigned short)(pColor.mRed   >> 3) << 10) |
								   ((unsigned short)(pColor.mGreen >> 3) << 5) |
								   ((unsigned short)(pColor.mBlue  >> 3));

		unsigned short* lDestBits = (unsigned short*)pThis->mScreen->GetBuffer() +
									 pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart;
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;
		float lOneOverZ = (float)pLeft->GetOneOverZ();

		while (lWidth--)
		{
			if (lOneOverZ > *lZBuffer)
			{
				*lDestBits = lColor;
				*lZBuffer = lOneOverZ;
			}

			lDestBits++;
			lZBuffer++;
			lOneOverZ += (float)pGradients->GetOneOverZXStep();
		}
	}
}

void Software3DPainter::DrawScanLineSingleColor16BitFastZTZW(const Gradients* pGradients,
															EdgeZ* pLeft, 
															EdgeZ* pRight,
															const Lepra::Color& pColor,
															Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		unsigned short lColor = ((unsigned short)(pColor.mRed   >> 3) << 11) |
								   ((unsigned short)(pColor.mGreen >> 2) << 5) |
								   ((unsigned short)(pColor.mBlue  >> 3));

		unsigned short* lDestBits = (unsigned short*)pThis->mScreen->GetBuffer() +
									 pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart;
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;
		float lOneOverZ = (float)pLeft->GetOneOverZ();

		while (lWidth--)
		{
			if (lOneOverZ > *lZBuffer)
			{
				*lDestBits = lColor;
				*lZBuffer = lOneOverZ;
			}

			lDestBits++;
			lZBuffer++;
			lOneOverZ += (float)pGradients->GetOneOverZXStep();
		}
	}
}

void Software3DPainter::DrawScanLineSingleColor24BitFastZTZW(const Gradients* pGradients,
															EdgeZ* pLeft, 
															EdgeZ* pRight,
															const Lepra::Color& pColor,
															Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		unsigned char* lDestBits = (unsigned char*)pThis->mScreen->GetBuffer() +
									 (pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart) * 3;
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;
		float lOneOverZ = (float)pLeft->GetOneOverZ();

		while (lWidth--)
		{
			if (lOneOverZ > *lZBuffer)
			{
				lDestBits[0] = pColor.mBlue;
				lDestBits[1] = pColor.mGreen;
				lDestBits[2] = pColor.mRed;
				*lZBuffer = lOneOverZ;
			}

			lDestBits += 3;
			lZBuffer++;
			lOneOverZ += (float)pGradients->GetOneOverZXStep();
		}
	}
}

void Software3DPainter::DrawScanLineSingleColor32BitFastZTZW(const Gradients* pGradients,
															EdgeZ* pLeft, 
															EdgeZ* pRight,
															const Lepra::Color& pColor,
															Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		unsigned int lColor = ((unsigned int)pColor.mRed << 16) |
								 ((unsigned int)pColor.mGreen << 8) |
								 (unsigned int)pColor.mBlue;

		unsigned int* lDestBits = (unsigned int*)pThis->mScreen->GetBuffer() +
									 pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart;
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;
		float lOneOverZ = (float)pLeft->GetOneOverZ();

		while (lWidth--)
		{
			if (lOneOverZ > *lZBuffer)
			{
				*lDestBits = lColor;
				*lZBuffer = lOneOverZ;
			}

			lDestBits++;
			lZBuffer++;
			lOneOverZ += (float)pGradients->GetOneOverZXStep();
		}
	}
}







///////////////////////////////////////////////////////////////////////////////////
//	
//	RGB scan line renderers.
//	
///////////////////////////////////////////////////////////////////////////////////








void Software3DPainter::DrawScanLineRGB8BitFast(const GradientsRGB* pGradients, EdgeRGB* pLeft,
	EdgeRGB* pRight, Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned char* lDestBits = (unsigned char*)pThis->mScreen->GetBuffer() +
									 pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lColOverZLeft = pLeft->GetROverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lColOverZStepAff = pGradients->GetROverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lColOverZRight = lColOverZLeft + lColOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lColLeft = lColOverZLeft * lZLeft * 255.0f;

		double lZRight;
		double lColRight;
		Lepra::FixedPointMath::Fixed16_16 lCol;
		Lepra::FixedPointMath::Fixed16_16 lDeltaCol = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lColRight = lColOverZRight * lZRight * 255.0f;

			lCol = Lepra::FixedPointMath::FloatToFixed16_16(lColLeft) + pGradients->GetDRDXModifier();

			lDeltaCol = Lepra::FixedPointMath::FloatToFixed16_16(lColRight - lColLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				*(lDestBits++) = (unsigned char)(lCol >> 16);
				lCol += lDeltaCol;
			}

			lZLeft = lZRight;
			lColLeft = lColRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lColOverZRight += lColOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lColRight = lZRight * (pRight->GetROverZ() - pGradients->GetROverZXStep()) * 255.0f;

			lCol = Lepra::FixedPointMath::FloatToFixed16_16(lColLeft) + pGradients->GetDRDXModifier();

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaCol = Lepra::FixedPointMath::FloatToFixed16_16(lColRight - lColLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				*(lDestBits++) = (unsigned char)(lCol >> 16);
				lCol += lDeltaCol;
			}
		}
	}
}

void Software3DPainter::DrawScanLineRGB15BitFast(const GradientsRGB* pGradients, 
														   EdgeRGB* pLeft, 
														   EdgeRGB* pRight,
														   Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned short* lDestBits = (unsigned short*)pThis->mScreen->GetBuffer() +
									 (pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart);

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lROverZLeft = pLeft->GetROverZ();
		double lGOverZLeft = pLeft->GetGOverZ();
		double lBOverZLeft = pLeft->GetBOverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lROverZStepAff = pGradients->GetROverZXStep() * lAffineLengthFloat;
		double lGOverZStepAff = pGradients->GetGOverZXStep() * lAffineLengthFloat;
		double lBOverZStepAff = pGradients->GetBOverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lROverZRight = lROverZLeft + lROverZStepAff;
		double lGOverZRight = lGOverZLeft + lGOverZStepAff;
		double lBOverZRight = lBOverZLeft + lBOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lRLeft = lROverZLeft * lZLeft * 31.0f;
		double lGLeft = lGOverZLeft * lZLeft * 31.0f;
		double lBLeft = lBOverZLeft * lZLeft * 31.0f;

		MACRO_CHECK_RGB(lRLeft, lGLeft, lBLeft, 31.0f);

		double lZRight;
		double lRRight;
		double lGRight;
		double lBRight;
		Lepra::FixedPointMath::Fixed16_16 lR;
		Lepra::FixedPointMath::Fixed16_16 lG;
		Lepra::FixedPointMath::Fixed16_16 lB;
		Lepra::FixedPointMath::Fixed16_16 lDeltaR = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaG = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaB = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lRRight = lROverZRight * lZRight * 31.0f;
			lGRight = lGOverZRight * lZRight * 31.0f;
			lBRight = lBOverZRight * lZRight * 31.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 31.0f);

			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) >> pThis->mAffineExp;
			lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) >> pThis->mAffineExp;
			lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				*lDestBits = (unsigned short)(((lR >> 16) << 10) | ((lG >> 16) << 5) | (lB >> 16));
				
				lDestBits++;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}

			lZLeft = lZRight;
			lRLeft = lRRight;
			lGLeft = lGRight;
			lBLeft = lBRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lROverZRight += lROverZStepAff;
			lGOverZRight += lGOverZStepAff;
			lBOverZRight += lBOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lRRight = lZRight * (pRight->GetROverZ() - pGradients->GetROverZXStep()) * 31.0f;
			lGRight = lZRight * (pRight->GetGOverZ() - pGradients->GetGOverZXStep()) * 31.0f;
			lBRight = lZRight * (pRight->GetBOverZ() - pGradients->GetBOverZXStep()) * 31.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 31.0f);

			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) / lRestWidth;
				lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) / lRestWidth;
				lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				*lDestBits = (unsigned short)(((lR >> 16) << 10) | ((lG >> 16) << 5) | (lB >> 16));
				
				lDestBits++;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}
		}
	}
}

void Software3DPainter::DrawScanLineRGB16BitFast(const GradientsRGB* pGradients, 
														   EdgeRGB* pLeft, 
														   EdgeRGB* pRight,
														   Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned short* lDestBits = (unsigned short*)pThis->mScreen->GetBuffer() +
									 (pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart);

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lROverZLeft = pLeft->GetROverZ();
		double lGOverZLeft = pLeft->GetGOverZ();
		double lBOverZLeft = pLeft->GetBOverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lROverZStepAff = pGradients->GetROverZXStep() * lAffineLengthFloat;
		double lGOverZStepAff = pGradients->GetGOverZXStep() * lAffineLengthFloat;
		double lBOverZStepAff = pGradients->GetBOverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lROverZRight = lROverZLeft + lROverZStepAff;
		double lGOverZRight = lGOverZLeft + lGOverZStepAff;
		double lBOverZRight = lBOverZLeft + lBOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lRLeft = lROverZLeft * lZLeft * 31.0f;
		double lGLeft = lGOverZLeft * lZLeft * 63.0f;
		double lBLeft = lBOverZLeft * lZLeft * 31.0f;

		MACRO_CHECK_RGB_16BIT(lRLeft, lGLeft, lBLeft);

		double lZRight;
		double lRRight;
		double lGRight;
		double lBRight;
		Lepra::FixedPointMath::Fixed16_16 lR;
		Lepra::FixedPointMath::Fixed16_16 lG;
		Lepra::FixedPointMath::Fixed16_16 lB;
		Lepra::FixedPointMath::Fixed16_16 lDeltaR = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaG = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaB = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lRRight = lROverZRight * lZRight * 31.0f;
			lGRight = lGOverZRight * lZRight * 63.0f;
			lBRight = lBOverZRight * lZRight * 31.0f;

			MACRO_CHECK_RGB_16BIT(lRRight, lGRight, lBRight);

			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) >> pThis->mAffineExp;
			lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) >> pThis->mAffineExp;
			lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				*lDestBits = (unsigned short)(((lR >> 16) << 11) | ((lG >> 16) << 5) | (lB >> 16));
				
				lDestBits++;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}

			lZLeft = lZRight;
			lRLeft = lRRight;
			lGLeft = lGRight;
			lBLeft = lBRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lROverZRight += lROverZStepAff;
			lGOverZRight += lGOverZStepAff;
			lBOverZRight += lBOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lRRight = lZRight * (pRight->GetROverZ() - pGradients->GetROverZXStep()) * 31.0f;
			lGRight = lZRight * (pRight->GetGOverZ() - pGradients->GetGOverZXStep()) * 63.0f;
			lBRight = lZRight * (pRight->GetBOverZ() - pGradients->GetBOverZXStep()) * 31.0f;

			MACRO_CHECK_RGB_16BIT(lRRight, lGRight, lBRight);

			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) / lRestWidth;
				lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) / lRestWidth;
				lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				*lDestBits = (unsigned short)(((lR >> 16) << 11) | ((lG >> 16) << 5) | (lB >> 16));
				
				lDestBits++;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}
		}
	}
}

void Software3DPainter::DrawScanLineRGB24BitFast(const GradientsRGB* pGradients, 
														   EdgeRGB* pLeft, 
														   EdgeRGB* pRight,
														   Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned char* lDestBits = (unsigned char*)pThis->mScreen->GetBuffer() +
									 (pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart) * 3;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lROverZLeft = pLeft->GetROverZ();
		double lGOverZLeft = pLeft->GetGOverZ();
		double lBOverZLeft = pLeft->GetBOverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lROverZStepAff = pGradients->GetROverZXStep() * lAffineLengthFloat;
		double lGOverZStepAff = pGradients->GetGOverZXStep() * lAffineLengthFloat;
		double lBOverZStepAff = pGradients->GetBOverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lROverZRight = lROverZLeft + lROverZStepAff;
		double lGOverZRight = lGOverZLeft + lGOverZStepAff;
		double lBOverZRight = lBOverZLeft + lBOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lRLeft = lROverZLeft * lZLeft * 255.0f;
		double lGLeft = lGOverZLeft * lZLeft * 255.0f;
		double lBLeft = lBOverZLeft * lZLeft * 255.0f;

		MACRO_CHECK_RGB(lRLeft, lGLeft, lBLeft, 255.0f);

		double lZRight;
		double lRRight;
		double lGRight;
		double lBRight;
		Lepra::FixedPointMath::Fixed16_16 lR;
		Lepra::FixedPointMath::Fixed16_16 lG;
		Lepra::FixedPointMath::Fixed16_16 lB;
		Lepra::FixedPointMath::Fixed16_16 lDeltaR = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaG = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaB = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lRRight = lROverZRight * lZRight * 255.0f;
			lGRight = lGOverZRight * lZRight * 255.0f;
			lBRight = lBOverZRight * lZRight * 255.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 255.0f);

			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) >> pThis->mAffineExp;
			lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) >> pThis->mAffineExp;
			lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				lDestBits[0] = (unsigned char)(lB >> 16);
				lDestBits[1] = (unsigned char)(lG >> 16);
				lDestBits[2] = (unsigned char)(lR >> 16);
				
				lDestBits += 3;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}

			lZLeft = lZRight;
			lRLeft = lRRight;
			lGLeft = lGRight;
			lBLeft = lBRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lROverZRight += lROverZStepAff;
			lGOverZRight += lGOverZStepAff;
			lBOverZRight += lBOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lRRight = lZRight * (pRight->GetROverZ() - pGradients->GetROverZXStep()) * 255.0f;
			lGRight = lZRight * (pRight->GetGOverZ() - pGradients->GetGOverZXStep()) * 255.0f;
			lBRight = lZRight * (pRight->GetBOverZ() - pGradients->GetBOverZXStep()) * 255.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 255.0f);

			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) / lRestWidth;
				lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) / lRestWidth;
				lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				lDestBits[0] = (unsigned char)(lB >> 16);
				lDestBits[1] = (unsigned char)(lG >> 16);
				lDestBits[2] = (unsigned char)(lR >> 16);
				
				lDestBits += 3;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}
		}
	}
}

void Software3DPainter::DrawScanLineRGB32BitFast(const GradientsRGB* pGradients, 
														   EdgeRGB* pLeft, 
														   EdgeRGB* pRight,
														   Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned char* lDestBits = (unsigned char*)pThis->mScreen->GetBuffer() +
									 ((pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart) << 2);

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lROverZLeft = pLeft->GetROverZ();
		double lGOverZLeft = pLeft->GetGOverZ();
		double lBOverZLeft = pLeft->GetBOverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lROverZStepAff = pGradients->GetROverZXStep() * lAffineLengthFloat;
		double lGOverZStepAff = pGradients->GetGOverZXStep() * lAffineLengthFloat;
		double lBOverZStepAff = pGradients->GetBOverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lROverZRight = lROverZLeft + lROverZStepAff;
		double lGOverZRight = lGOverZLeft + lGOverZStepAff;
		double lBOverZRight = lBOverZLeft + lBOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lRLeft = lROverZLeft * lZLeft * 255.0f;
		double lGLeft = lGOverZLeft * lZLeft * 255.0f;
		double lBLeft = lBOverZLeft * lZLeft * 255.0f;

		MACRO_CHECK_RGB(lRLeft, lGLeft, lBLeft, 255.0f);

		double lZRight;
		double lRRight;
		double lGRight;
		double lBRight;
		Lepra::FixedPointMath::Fixed16_16 lR;
		Lepra::FixedPointMath::Fixed16_16 lG;
		Lepra::FixedPointMath::Fixed16_16 lB;
		Lepra::FixedPointMath::Fixed16_16 lDeltaR = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaG = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaB = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lRRight = lROverZRight * lZRight * 255.0f;
			lGRight = lGOverZRight * lZRight * 255.0f;
			lBRight = lBOverZRight * lZRight * 255.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 255.0f);

			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) >> pThis->mAffineExp;
			lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) >> pThis->mAffineExp;
			lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				lDestBits[0] = (unsigned char)(lB >> 16);
				lDestBits[1] = (unsigned char)(lG >> 16);
				lDestBits[2] = (unsigned char)(lR >> 16);
				
				lDestBits += 4;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}

			lZLeft = lZRight;
			lRLeft = lRRight;
			lGLeft = lGRight;
			lBLeft = lBRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lROverZRight += lROverZStepAff;
			lGOverZRight += lGOverZStepAff;
			lBOverZRight += lBOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lRRight = lZRight * (pRight->GetROverZ() - pGradients->GetROverZXStep()) * 255.0f;
			lGRight = lZRight * (pRight->GetGOverZ() - pGradients->GetGOverZXStep()) * 255.0f;
			lBRight = lZRight * (pRight->GetBOverZ() - pGradients->GetBOverZXStep()) * 255.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 255.0f);

			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) / lRestWidth;
				lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) / lRestWidth;
				lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				lDestBits[0] = (unsigned char)(lB >> 16);
				lDestBits[1] = (unsigned char)(lG >> 16);
				lDestBits[2] = (unsigned char)(lR >> 16);
				
				lDestBits += 4;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}
		}
	}
}






void Software3DPainter::DrawScanLineRGB8BitFastZT(const GradientsRGB* pGradients, 
														  EdgeRGB* pLeft, 
														  EdgeRGB* pRight,
														  Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned char* lDestBits = (unsigned char*)pThis->mScreen->GetBuffer() +
									 pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart;
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lColOverZLeft = pLeft->GetROverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lColOverZStepAff = pGradients->GetROverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lColOverZRight = lColOverZLeft + lColOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lColLeft = lColOverZLeft * lZLeft * 255.0f;

		double lZRight;
		double lColRight;
		Lepra::FixedPointMath::Fixed16_16 lCol;
		Lepra::FixedPointMath::Fixed16_16 lDeltaCol = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//

		float lOneOverZ = (float)lOneOverZLeft;

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lColRight = lColOverZRight * lZRight * 255.0f;

			lCol = Lepra::FixedPointMath::FloatToFixed16_16(lColLeft) + pGradients->GetDRDXModifier();

			lDeltaCol = Lepra::FixedPointMath::FloatToFixed16_16(lColRight - lColLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				if (lOneOverZ > *lZBuffer)
				{
					*lDestBits = (unsigned char)(lCol >> 16);
				}

				lDestBits++;
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lCol += lDeltaCol;
			}

			lZLeft = lZRight;
			lColLeft = lColRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lColOverZRight += lColOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lColRight = lZRight * (pRight->GetROverZ() - pGradients->GetROverZXStep()) * 255.0f;

			lCol = Lepra::FixedPointMath::FloatToFixed16_16(lColLeft) + pGradients->GetDRDXModifier();

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaCol = Lepra::FixedPointMath::FloatToFixed16_16(lColRight - lColLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				if (lOneOverZ > *lZBuffer)
				{
					*lDestBits = (unsigned char)(lCol >> 16);
				}

				lDestBits++;
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lCol += lDeltaCol;
			}
		}
	}
}

void Software3DPainter::DrawScanLineRGB15BitFastZT(const GradientsRGB* pGradients, 
														   EdgeRGB* pLeft, 
														   EdgeRGB* pRight,
														   Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned short* lDestBits = (unsigned short*)pThis->mScreen->GetBuffer() +
									 (pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart);
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lROverZLeft = pLeft->GetROverZ();
		double lGOverZLeft = pLeft->GetGOverZ();
		double lBOverZLeft = pLeft->GetBOverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lROverZStepAff = pGradients->GetROverZXStep() * lAffineLengthFloat;
		double lGOverZStepAff = pGradients->GetGOverZXStep() * lAffineLengthFloat;
		double lBOverZStepAff = pGradients->GetBOverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lROverZRight = lROverZLeft + lROverZStepAff;
		double lGOverZRight = lGOverZLeft + lGOverZStepAff;
		double lBOverZRight = lBOverZLeft + lBOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lRLeft = lROverZLeft * lZLeft * 31.0f;
		double lGLeft = lGOverZLeft * lZLeft * 31.0f;
		double lBLeft = lBOverZLeft * lZLeft * 31.0f;

		MACRO_CHECK_RGB(lRLeft, lGLeft, lBLeft, 31.0f);

		double lZRight;
		double lRRight;
		double lGRight;
		double lBRight;
		Lepra::FixedPointMath::Fixed16_16 lR;
		Lepra::FixedPointMath::Fixed16_16 lG;
		Lepra::FixedPointMath::Fixed16_16 lB;
		Lepra::FixedPointMath::Fixed16_16 lDeltaR = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaG = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaB = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//
		float lOneOverZ = (float)lOneOverZLeft;

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lRRight = lROverZRight * lZRight * 31.0f;
			lGRight = lGOverZRight * lZRight * 31.0f;
			lBRight = lBOverZRight * lZRight * 31.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 31.0f);

			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) >> pThis->mAffineExp;
			lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) >> pThis->mAffineExp;
			lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				if (lOneOverZ > *lZBuffer)
				{
					*lDestBits = (unsigned short)(((lR >> 16) << 10) | ((lG >> 16) << 5) | (lB >> 16));
				}
				
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lDestBits++;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}

			lZLeft = lZRight;
			lRLeft = lRRight;
			lGLeft = lGRight;
			lBLeft = lBRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lROverZRight += lROverZStepAff;
			lGOverZRight += lGOverZStepAff;
			lBOverZRight += lBOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lRRight = lZRight * (pRight->GetROverZ() - pGradients->GetROverZXStep()) * 31.0f;
			lGRight = lZRight * (pRight->GetGOverZ() - pGradients->GetGOverZXStep()) * 31.0f;
			lBRight = lZRight * (pRight->GetBOverZ() - pGradients->GetBOverZXStep()) * 31.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 31.0f);

			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) / lRestWidth;
				lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) / lRestWidth;
				lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				if (lOneOverZ > *lZBuffer)
				{
					*lDestBits = (unsigned short)(((lR >> 16) << 10) | ((lG >> 16) << 5) | (lB >> 16));
				}
				
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lDestBits++;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}
		}
	}
}

void Software3DPainter::DrawScanLineRGB16BitFastZT(const GradientsRGB* pGradients, 
														   EdgeRGB* pLeft, 
														   EdgeRGB* pRight,
														   Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned short* lDestBits = (unsigned short*)pThis->mScreen->GetBuffer() +
									 (pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart);
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lROverZLeft = pLeft->GetROverZ();
		double lGOverZLeft = pLeft->GetGOverZ();
		double lBOverZLeft = pLeft->GetBOverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lROverZStepAff = pGradients->GetROverZXStep() * lAffineLengthFloat;
		double lGOverZStepAff = pGradients->GetGOverZXStep() * lAffineLengthFloat;
		double lBOverZStepAff = pGradients->GetBOverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lROverZRight = lROverZLeft + lROverZStepAff;
		double lGOverZRight = lGOverZLeft + lGOverZStepAff;
		double lBOverZRight = lBOverZLeft + lBOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lRLeft = lROverZLeft * lZLeft * 31.0f;
		double lGLeft = lGOverZLeft * lZLeft * 63.0f;
		double lBLeft = lBOverZLeft * lZLeft * 31.0f;

		MACRO_CHECK_RGB_16BIT(lRLeft, lGLeft, lBLeft);

		double lZRight;
		double lRRight;
		double lGRight;
		double lBRight;
		Lepra::FixedPointMath::Fixed16_16 lR;
		Lepra::FixedPointMath::Fixed16_16 lG;
		Lepra::FixedPointMath::Fixed16_16 lB;
		Lepra::FixedPointMath::Fixed16_16 lDeltaR = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaG = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaB = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//
		float lOneOverZ = (float)lOneOverZLeft;

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lRRight = lROverZRight * lZRight * 31.0f;
			lGRight = lGOverZRight * lZRight * 63.0f;
			lBRight = lBOverZRight * lZRight * 31.0f;

			MACRO_CHECK_RGB_16BIT(lRRight, lGRight, lBRight);

			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) >> pThis->mAffineExp;
			lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) >> pThis->mAffineExp;
			lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				if (lOneOverZ > *lZBuffer)
				{
					*lDestBits = (unsigned short)(((lR >> 16) << 11) | ((lG >> 16) << 5) | (lB >> 16));
				}
				
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lDestBits++;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}

			lZLeft = lZRight;
			lRLeft = lRRight;
			lGLeft = lGRight;
			lBLeft = lBRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lROverZRight += lROverZStepAff;
			lGOverZRight += lGOverZStepAff;
			lBOverZRight += lBOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lRRight = lZRight * (pRight->GetROverZ() - pGradients->GetROverZXStep()) * 31.0f;
			lGRight = lZRight * (pRight->GetGOverZ() - pGradients->GetGOverZXStep()) * 63.0f;
			lBRight = lZRight * (pRight->GetBOverZ() - pGradients->GetBOverZXStep()) * 31.0f;

			MACRO_CHECK_RGB_16BIT(lRRight, lGRight, lBRight);
		
			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) / lRestWidth;
				lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) / lRestWidth;
				lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				if (lOneOverZ > *lZBuffer)
				{
					*lDestBits = (unsigned short)(((lR >> 16) << 11) | ((lG >> 16) << 5) | (lB >> 16));
				}
				
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lDestBits++;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}
		}
	}
}

void Software3DPainter::DrawScanLineRGB24BitFastZT(const GradientsRGB* pGradients, 
														   EdgeRGB* pLeft, 
														   EdgeRGB* pRight,
														   Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned char* lDestBits = (unsigned char*)pThis->mScreen->GetBuffer() +
									 (pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart) * 3;
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lROverZLeft = pLeft->GetROverZ();
		double lGOverZLeft = pLeft->GetGOverZ();
		double lBOverZLeft = pLeft->GetBOverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lROverZStepAff = pGradients->GetROverZXStep() * lAffineLengthFloat;
		double lGOverZStepAff = pGradients->GetGOverZXStep() * lAffineLengthFloat;
		double lBOverZStepAff = pGradients->GetBOverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lROverZRight = lROverZLeft + lROverZStepAff;
		double lGOverZRight = lGOverZLeft + lGOverZStepAff;
		double lBOverZRight = lBOverZLeft + lBOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lRLeft = lROverZLeft * lZLeft * 255.0f;
		double lGLeft = lGOverZLeft * lZLeft * 255.0f;
		double lBLeft = lBOverZLeft * lZLeft * 255.0f;

		MACRO_CHECK_RGB(lRLeft, lGLeft, lBLeft, 255.0f);

		double lZRight;
		double lRRight;
		double lGRight;
		double lBRight;
		Lepra::FixedPointMath::Fixed16_16 lR;
		Lepra::FixedPointMath::Fixed16_16 lG;
		Lepra::FixedPointMath::Fixed16_16 lB;
		Lepra::FixedPointMath::Fixed16_16 lDeltaR = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaG = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaB = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//

		float lOneOverZ = (float)lOneOverZLeft;

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lRRight = lROverZRight * lZRight * 255.0f;
			lGRight = lGOverZRight * lZRight * 255.0f;
			lBRight = lBOverZRight * lZRight * 255.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 255.0f);

			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) >> pThis->mAffineExp;
			lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) >> pThis->mAffineExp;
			lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				if (lOneOverZ > *lZBuffer)
				{
					lDestBits[0] = (unsigned char)(lB >> 16);
					lDestBits[1] = (unsigned char)(lG >> 16);
					lDestBits[2] = (unsigned char)(lR >> 16);
				}
				
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lDestBits += 3;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}

			lZLeft = lZRight;
			lRLeft = lRRight;
			lGLeft = lGRight;
			lBLeft = lBRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lROverZRight += lROverZStepAff;
			lGOverZRight += lGOverZStepAff;
			lBOverZRight += lBOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lRRight = lZRight * (pRight->GetROverZ() - pGradients->GetROverZXStep()) * 255.0f;
			lGRight = lZRight * (pRight->GetGOverZ() - pGradients->GetGOverZXStep()) * 255.0f;
			lBRight = lZRight * (pRight->GetBOverZ() - pGradients->GetBOverZXStep()) * 255.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 255.0f);

			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) / lRestWidth;
				lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) / lRestWidth;
				lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				if (lOneOverZ > *lZBuffer)
				{
					lDestBits[0] = (unsigned char)(lB >> 16);
					lDestBits[1] = (unsigned char)(lG >> 16);
					lDestBits[2] = (unsigned char)(lR >> 16);
				}
				
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lDestBits += 3;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}
		}
	}
}

void Software3DPainter::DrawScanLineRGB32BitFastZT(const GradientsRGB* pGradients, 
														   EdgeRGB* pLeft, 
														   EdgeRGB* pRight,
														   Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned char* lDestBits = (unsigned char*)pThis->mScreen->GetBuffer() +
									 ((pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart) << 2);
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lROverZLeft = pLeft->GetROverZ();
		double lGOverZLeft = pLeft->GetGOverZ();
		double lBOverZLeft = pLeft->GetBOverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lROverZStepAff = pGradients->GetROverZXStep() * lAffineLengthFloat;
		double lGOverZStepAff = pGradients->GetGOverZXStep() * lAffineLengthFloat;
		double lBOverZStepAff = pGradients->GetBOverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lROverZRight = lROverZLeft + lROverZStepAff;
		double lGOverZRight = lGOverZLeft + lGOverZStepAff;
		double lBOverZRight = lBOverZLeft + lBOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lRLeft = lROverZLeft * lZLeft * 255.0f;
		double lGLeft = lGOverZLeft * lZLeft * 255.0f;
		double lBLeft = lBOverZLeft * lZLeft * 255.0f;

		MACRO_CHECK_RGB(lRLeft, lGLeft, lBLeft, 255.0f);

		double lZRight;
		double lRRight;
		double lGRight;
		double lBRight;
		Lepra::FixedPointMath::Fixed16_16 lR;
		Lepra::FixedPointMath::Fixed16_16 lG;
		Lepra::FixedPointMath::Fixed16_16 lB;
		Lepra::FixedPointMath::Fixed16_16 lDeltaR = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaG = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaB = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//

		float lOneOverZ = (float)lOneOverZLeft;

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lRRight = lROverZRight * lZRight * 255.0f;
			lGRight = lGOverZRight * lZRight * 255.0f;
			lBRight = lBOverZRight * lZRight * 255.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 255.0f);

			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) >> pThis->mAffineExp;
			lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) >> pThis->mAffineExp;
			lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				if (lOneOverZ > *lZBuffer)
				{
					lDestBits[0] = (unsigned char)(lB >> 16);
					lDestBits[1] = (unsigned char)(lG >> 16);
					lDestBits[2] = (unsigned char)(lR >> 16);
				}
				
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lDestBits += 4;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}

			lZLeft = lZRight;
			lRLeft = lRRight;
			lGLeft = lGRight;
			lBLeft = lBRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lROverZRight += lROverZStepAff;
			lGOverZRight += lGOverZStepAff;
			lBOverZRight += lBOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lRRight = lZRight * (pRight->GetROverZ() - pGradients->GetROverZXStep()) * 255.0f;
			lGRight = lZRight * (pRight->GetGOverZ() - pGradients->GetGOverZXStep()) * 255.0f;
			lBRight = lZRight * (pRight->GetBOverZ() - pGradients->GetBOverZXStep()) * 255.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 255.0f);

			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) / lRestWidth;
				lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) / lRestWidth;
				lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				if (lOneOverZ > *lZBuffer)
				{
					lDestBits[0] = (unsigned char)(lB >> 16);
					lDestBits[1] = (unsigned char)(lG >> 16);
					lDestBits[2] = (unsigned char)(lR >> 16);
				}
				
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lDestBits += 4;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}
		}
	}
}







void Software3DPainter::DrawScanLineRGB8BitFastZW(const GradientsRGB* pGradients, 
														  EdgeRGB* pLeft, 
														  EdgeRGB* pRight,
														  Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned char* lDestBits = (unsigned char*)pThis->mScreen->GetBuffer() +
									 pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart;
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lColOverZLeft = pLeft->GetROverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lColOverZStepAff = pGradients->GetROverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lColOverZRight = lColOverZLeft + lColOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lColLeft = lColOverZLeft * lZLeft * 255.0f;

		double lZRight;
		double lColRight;
		Lepra::FixedPointMath::Fixed16_16 lCol;
		Lepra::FixedPointMath::Fixed16_16 lDeltaCol = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//

		float lOneOverZ = (float)lOneOverZLeft;

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lColRight = lColOverZRight * lZRight * 255.0f;

			lCol = Lepra::FixedPointMath::FloatToFixed16_16(lColLeft) + pGradients->GetDRDXModifier();

			lDeltaCol = Lepra::FixedPointMath::FloatToFixed16_16(lColRight - lColLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				*(lDestBits++) = (unsigned char)(lCol >> 16);
				*lZBuffer = lOneOverZ;

				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lCol += lDeltaCol;
			}

			lZLeft = lZRight;
			lColLeft = lColRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lColOverZRight += lColOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lColRight = lZRight * (pRight->GetROverZ() - pGradients->GetROverZXStep()) * 255.0f;

			lCol = Lepra::FixedPointMath::FloatToFixed16_16(lColLeft) + pGradients->GetDRDXModifier();

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaCol = Lepra::FixedPointMath::FloatToFixed16_16(lColRight - lColLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				*(lDestBits++) = (unsigned char)(lCol >> 16);
				*lZBuffer = lOneOverZ;

				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lCol += lDeltaCol;
			}
		}
	}
}

void Software3DPainter::DrawScanLineRGB15BitFastZW(const GradientsRGB* pGradients, 
														   EdgeRGB* pLeft, 
														   EdgeRGB* pRight,
														   Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned short* lDestBits = (unsigned short*)pThis->mScreen->GetBuffer() +
									 (pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart);
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lROverZLeft = pLeft->GetROverZ();
		double lGOverZLeft = pLeft->GetGOverZ();
		double lBOverZLeft = pLeft->GetBOverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lROverZStepAff = pGradients->GetROverZXStep() * lAffineLengthFloat;
		double lGOverZStepAff = pGradients->GetGOverZXStep() * lAffineLengthFloat;
		double lBOverZStepAff = pGradients->GetBOverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lROverZRight = lROverZLeft + lROverZStepAff;
		double lGOverZRight = lGOverZLeft + lGOverZStepAff;
		double lBOverZRight = lBOverZLeft + lBOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lRLeft = lROverZLeft * lZLeft * 31.0f;
		double lGLeft = lGOverZLeft * lZLeft * 31.0f;
		double lBLeft = lBOverZLeft * lZLeft * 31.0f;

		MACRO_CHECK_RGB(lRLeft, lGLeft, lBLeft, 31.0f);

		double lZRight;
		double lRRight;
		double lGRight;
		double lBRight;
		Lepra::FixedPointMath::Fixed16_16 lR;
		Lepra::FixedPointMath::Fixed16_16 lG;
		Lepra::FixedPointMath::Fixed16_16 lB;
		Lepra::FixedPointMath::Fixed16_16 lDeltaR = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaG = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaB = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//
		float lOneOverZ = (float)lOneOverZLeft;

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lRRight = lROverZRight * lZRight * 31.0f;
			lGRight = lGOverZRight * lZRight * 31.0f;
			lBRight = lBOverZRight * lZRight * 31.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 31.0f);

			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) >> pThis->mAffineExp;
			lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) >> pThis->mAffineExp;
			lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				*lDestBits = (unsigned short)(((lR >> 16) << 10) | ((lG >> 16) << 5) | (lB >> 16));
				*lZBuffer = lOneOverZ;
				
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lDestBits++;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}

			lZLeft = lZRight;
			lRLeft = lRRight;
			lGLeft = lGRight;
			lBLeft = lBRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lROverZRight += lROverZStepAff;
			lGOverZRight += lGOverZStepAff;
			lBOverZRight += lBOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lRRight = lZRight * (pRight->GetROverZ() - pGradients->GetROverZXStep()) * 31.0f;
			lGRight = lZRight * (pRight->GetGOverZ() - pGradients->GetGOverZXStep()) * 31.0f;
			lBRight = lZRight * (pRight->GetBOverZ() - pGradients->GetBOverZXStep()) * 31.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 31.0f);

			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) / lRestWidth;
				lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) / lRestWidth;
				lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				*lDestBits = (unsigned short)(((lR >> 16) << 10) | ((lG >> 16) << 5) | (lB >> 16));
				*lZBuffer = lOneOverZ;
				
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lDestBits++;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}
		}
	}
}

void Software3DPainter::DrawScanLineRGB16BitFastZW(const GradientsRGB* pGradients, 
														   EdgeRGB* pLeft, 
														   EdgeRGB* pRight,
														   Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned short* lDestBits = (unsigned short*)pThis->mScreen->GetBuffer() +
									 (pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart);
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lROverZLeft = pLeft->GetROverZ();
		double lGOverZLeft = pLeft->GetGOverZ();
		double lBOverZLeft = pLeft->GetBOverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lROverZStepAff = pGradients->GetROverZXStep() * lAffineLengthFloat;
		double lGOverZStepAff = pGradients->GetGOverZXStep() * lAffineLengthFloat;
		double lBOverZStepAff = pGradients->GetBOverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lROverZRight = lROverZLeft + lROverZStepAff;
		double lGOverZRight = lGOverZLeft + lGOverZStepAff;
		double lBOverZRight = lBOverZLeft + lBOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lRLeft = lROverZLeft * lZLeft * 31.0f;
		double lGLeft = lGOverZLeft * lZLeft * 63.0f;
		double lBLeft = lBOverZLeft * lZLeft * 31.0f;

		MACRO_CHECK_RGB_16BIT(lRLeft, lGLeft, lBLeft);

		double lZRight;
		double lRRight;
		double lGRight;
		double lBRight;
		Lepra::FixedPointMath::Fixed16_16 lR;
		Lepra::FixedPointMath::Fixed16_16 lG;
		Lepra::FixedPointMath::Fixed16_16 lB;
		Lepra::FixedPointMath::Fixed16_16 lDeltaR = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaG = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaB = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//
		float lOneOverZ = (float)lOneOverZLeft;

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lRRight = lROverZRight * lZRight * 31.0f;
			lGRight = lGOverZRight * lZRight * 63.0f;
			lBRight = lBOverZRight * lZRight * 31.0f;

			MACRO_CHECK_RGB_16BIT(lRRight, lGRight, lBRight);

			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) >> pThis->mAffineExp;
			lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) >> pThis->mAffineExp;
			lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				*lDestBits = (unsigned short)(((lR >> 16) << 11) | ((lG >> 16) << 5) | (lB >> 16));
				*lZBuffer = lOneOverZ;
				
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lDestBits++;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}

			lZLeft = lZRight;
			lRLeft = lRRight;
			lGLeft = lGRight;
			lBLeft = lBRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lROverZRight += lROverZStepAff;
			lGOverZRight += lGOverZStepAff;
			lBOverZRight += lBOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lRRight = lZRight * (pRight->GetROverZ() - pGradients->GetROverZXStep()) * 31.0f;
			lGRight = lZRight * (pRight->GetGOverZ() - pGradients->GetGOverZXStep()) * 63.0f;
			lBRight = lZRight * (pRight->GetBOverZ() - pGradients->GetBOverZXStep()) * 31.0f;

			MACRO_CHECK_RGB_16BIT(lRRight, lGRight, lBRight);

			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) / lRestWidth;
				lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) / lRestWidth;
				lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				*lDestBits = (unsigned short)(((lR >> 16) << 11) | ((lG >> 16) << 5) | (lB >> 16));
				*lZBuffer = lOneOverZ;
				
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lDestBits++;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}
		}
	}
}

void Software3DPainter::DrawScanLineRGB24BitFastZW(const GradientsRGB* pGradients, 
														   EdgeRGB* pLeft, 
														   EdgeRGB* pRight,
														   Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned char* lDestBits = (unsigned char*)pThis->mScreen->GetBuffer() +
									 (pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart) * 3;
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lROverZLeft = pLeft->GetROverZ();
		double lGOverZLeft = pLeft->GetGOverZ();
		double lBOverZLeft = pLeft->GetBOverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lROverZStepAff = pGradients->GetROverZXStep() * lAffineLengthFloat;
		double lGOverZStepAff = pGradients->GetGOverZXStep() * lAffineLengthFloat;
		double lBOverZStepAff = pGradients->GetBOverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lROverZRight = lROverZLeft + lROverZStepAff;
		double lGOverZRight = lGOverZLeft + lGOverZStepAff;
		double lBOverZRight = lBOverZLeft + lBOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lRLeft = lROverZLeft * lZLeft * 255.0f;
		double lGLeft = lGOverZLeft * lZLeft * 255.0f;
		double lBLeft = lBOverZLeft * lZLeft * 255.0f;

		MACRO_CHECK_RGB(lRLeft, lGLeft, lBLeft, 255.0f);

		double lZRight;
		double lRRight;
		double lGRight;
		double lBRight;
		Lepra::FixedPointMath::Fixed16_16 lR;
		Lepra::FixedPointMath::Fixed16_16 lG;
		Lepra::FixedPointMath::Fixed16_16 lB;
		Lepra::FixedPointMath::Fixed16_16 lDeltaR = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaG = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaB = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//

		float lOneOverZ = (float)lOneOverZLeft;

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lRRight = lROverZRight * lZRight * 255.0f;
			lGRight = lGOverZRight * lZRight * 255.0f;
			lBRight = lBOverZRight * lZRight * 255.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 255.0f);

			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) >> pThis->mAffineExp;
			lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) >> pThis->mAffineExp;
			lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				lDestBits[0] = (unsigned char)(lB >> 16);
				lDestBits[1] = (unsigned char)(lG >> 16);
				lDestBits[2] = (unsigned char)(lR >> 16);
				*lZBuffer = lOneOverZ;
				
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lDestBits += 3;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}

			lZLeft = lZRight;
			lRLeft = lRRight;
			lGLeft = lGRight;
			lBLeft = lBRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lROverZRight += lROverZStepAff;
			lGOverZRight += lGOverZStepAff;
			lBOverZRight += lBOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lRRight = lZRight * (pRight->GetROverZ() - pGradients->GetROverZXStep()) * 255.0f;
			lGRight = lZRight * (pRight->GetGOverZ() - pGradients->GetGOverZXStep()) * 255.0f;
			lBRight = lZRight * (pRight->GetBOverZ() - pGradients->GetBOverZXStep()) * 255.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 255.0f);

			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) / lRestWidth;
				lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) / lRestWidth;
				lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				lDestBits[0] = (unsigned char)(lB >> 16);
				lDestBits[1] = (unsigned char)(lG >> 16);
				lDestBits[2] = (unsigned char)(lR >> 16);
				*lZBuffer = lOneOverZ;
				
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lDestBits += 3;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}
		}
	}
}

void Software3DPainter::DrawScanLineRGB32BitFastZW(const GradientsRGB* pGradients, 
														   EdgeRGB* pLeft, 
														   EdgeRGB* pRight,
														   Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned char* lDestBits = (unsigned char*)pThis->mScreen->GetBuffer() +
									 ((pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart) << 2);
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lROverZLeft = pLeft->GetROverZ();
		double lGOverZLeft = pLeft->GetGOverZ();
		double lBOverZLeft = pLeft->GetBOverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lROverZStepAff = pGradients->GetROverZXStep() * lAffineLengthFloat;
		double lGOverZStepAff = pGradients->GetGOverZXStep() * lAffineLengthFloat;
		double lBOverZStepAff = pGradients->GetBOverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lROverZRight = lROverZLeft + lROverZStepAff;
		double lGOverZRight = lGOverZLeft + lGOverZStepAff;
		double lBOverZRight = lBOverZLeft + lBOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lRLeft = lROverZLeft * lZLeft * 255.0f;
		double lGLeft = lGOverZLeft * lZLeft * 255.0f;
		double lBLeft = lBOverZLeft * lZLeft * 255.0f;

		MACRO_CHECK_RGB(lRLeft, lGLeft, lBLeft, 255.0f);

		double lZRight;
		double lRRight;
		double lGRight;
		double lBRight;
		Lepra::FixedPointMath::Fixed16_16 lR;
		Lepra::FixedPointMath::Fixed16_16 lG;
		Lepra::FixedPointMath::Fixed16_16 lB;
		Lepra::FixedPointMath::Fixed16_16 lDeltaR = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaG = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaB = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//

		float lOneOverZ = (float)lOneOverZLeft;

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lRRight = lROverZRight * lZRight * 255.0f;
			lGRight = lGOverZRight * lZRight * 255.0f;
			lBRight = lBOverZRight * lZRight * 255.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 255.0f);

			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) >> pThis->mAffineExp;
			lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) >> pThis->mAffineExp;
			lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				lDestBits[0] = (unsigned char)(lB >> 16);
				lDestBits[1] = (unsigned char)(lG >> 16);
				lDestBits[2] = (unsigned char)(lR >> 16);
				*lZBuffer = lOneOverZ;
				
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lDestBits += 4;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}

			lZLeft = lZRight;
			lRLeft = lRRight;
			lGLeft = lGRight;
			lBLeft = lBRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lROverZRight += lROverZStepAff;
			lGOverZRight += lGOverZStepAff;
			lBOverZRight += lBOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lRRight = lZRight * (pRight->GetROverZ() - pGradients->GetROverZXStep()) * 255.0f;
			lGRight = lZRight * (pRight->GetGOverZ() - pGradients->GetGOverZXStep()) * 255.0f;
			lBRight = lZRight * (pRight->GetBOverZ() - pGradients->GetBOverZXStep()) * 255.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 255.0f);

			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) / lRestWidth;
				lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) / lRestWidth;
				lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				lDestBits[0] = (unsigned char)(lB >> 16);
				lDestBits[1] = (unsigned char)(lG >> 16);
				lDestBits[2] = (unsigned char)(lR >> 16);
				*lZBuffer = lOneOverZ;
				
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lDestBits += 4;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}
		}
	}
}









void Software3DPainter::DrawScanLineRGB8BitFastZTZW(const GradientsRGB* pGradients, 
														  EdgeRGB* pLeft, 
														  EdgeRGB* pRight,
														  Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned char* lDestBits = (unsigned char*)pThis->mScreen->GetBuffer() +
									 pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart;
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lColOverZLeft = pLeft->GetROverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lColOverZStepAff = pGradients->GetROverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lColOverZRight = lColOverZLeft + lColOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lColLeft = lColOverZLeft * lZLeft * 255.0f;

		double lZRight;
		double lColRight;
		Lepra::FixedPointMath::Fixed16_16 lCol;
		Lepra::FixedPointMath::Fixed16_16 lDeltaCol = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//

		float lOneOverZ = (float)lOneOverZLeft;

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lColRight = lColOverZRight * lZRight * 255.0f;

			lCol = Lepra::FixedPointMath::FloatToFixed16_16(lColLeft) + pGradients->GetDRDXModifier();

			lDeltaCol = Lepra::FixedPointMath::FloatToFixed16_16(lColRight - lColLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				if (lOneOverZ > *lZBuffer)
				{
					*lDestBits = (unsigned char)(lCol >> 16);
					*lZBuffer = lOneOverZ;
				}

				lDestBits++;
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lCol += lDeltaCol;
			}

			lZLeft = lZRight;
			lColLeft = lColRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lColOverZRight += lColOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lColRight = lZRight * (pRight->GetROverZ() - pGradients->GetROverZXStep()) * 255.0f;

			lCol = Lepra::FixedPointMath::FloatToFixed16_16(lColLeft) + pGradients->GetDRDXModifier();

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaCol = Lepra::FixedPointMath::FloatToFixed16_16(lColRight - lColLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				if (lOneOverZ > *lZBuffer)
				{
					*lDestBits = (unsigned char)(lCol >> 16);
					*lZBuffer = lOneOverZ;
				}

				lDestBits++;
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lCol += lDeltaCol;
			}
		}
	}
}

void Software3DPainter::DrawScanLineRGB15BitFastZTZW(const GradientsRGB* pGradients, 
														   EdgeRGB* pLeft, 
														   EdgeRGB* pRight,
														   Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned short* lDestBits = (unsigned short*)pThis->mScreen->GetBuffer() +
									 (pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart);
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lROverZLeft = pLeft->GetROverZ();
		double lGOverZLeft = pLeft->GetGOverZ();
		double lBOverZLeft = pLeft->GetBOverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lROverZStepAff = pGradients->GetROverZXStep() * lAffineLengthFloat;
		double lGOverZStepAff = pGradients->GetGOverZXStep() * lAffineLengthFloat;
		double lBOverZStepAff = pGradients->GetBOverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lROverZRight = lROverZLeft + lROverZStepAff;
		double lGOverZRight = lGOverZLeft + lGOverZStepAff;
		double lBOverZRight = lBOverZLeft + lBOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lRLeft = lROverZLeft * lZLeft * 31.0f;
		double lGLeft = lGOverZLeft * lZLeft * 31.0f;
		double lBLeft = lBOverZLeft * lZLeft * 31.0f;

		MACRO_CHECK_RGB(lRLeft, lGLeft, lBLeft, 31.0f);

		double lZRight;
		double lRRight;
		double lGRight;
		double lBRight;
		Lepra::FixedPointMath::Fixed16_16 lR;
		Lepra::FixedPointMath::Fixed16_16 lG;
		Lepra::FixedPointMath::Fixed16_16 lB;
		Lepra::FixedPointMath::Fixed16_16 lDeltaR = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaG = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaB = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//
		float lOneOverZ = (float)lOneOverZLeft;

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lRRight = lROverZRight * lZRight * 31.0f;
			lGRight = lGOverZRight * lZRight * 31.0f;
			lBRight = lBOverZRight * lZRight * 31.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 31.0f);

			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) >> pThis->mAffineExp;
			lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) >> pThis->mAffineExp;
			lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				if (lOneOverZ > *lZBuffer)
				{
					*lDestBits = (unsigned short)(((lR >> 16) << 10) | ((lG >> 16) << 5) | (lB >> 16));
					*lZBuffer = lOneOverZ;
				}
				
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lDestBits++;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}

			lZLeft = lZRight;
			lRLeft = lRRight;
			lGLeft = lGRight;
			lBLeft = lBRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lROverZRight += lROverZStepAff;
			lGOverZRight += lGOverZStepAff;
			lBOverZRight += lBOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lRRight = lZRight * (pRight->GetROverZ() - pGradients->GetROverZXStep()) * 31.0f;
			lGRight = lZRight * (pRight->GetGOverZ() - pGradients->GetGOverZXStep()) * 31.0f;
			lBRight = lZRight * (pRight->GetBOverZ() - pGradients->GetBOverZXStep()) * 31.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 31.0f);

			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) / lRestWidth;
				lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) / lRestWidth;
				lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				if (lOneOverZ > *lZBuffer)
				{
					*lDestBits = (unsigned short)(((lR >> 16) << 10) | ((lG >> 16) << 5) | (lB >> 16));
					*lZBuffer = lOneOverZ;
				}
				
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lDestBits++;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}
		}
	}
}

void Software3DPainter::DrawScanLineRGB16BitFastZTZW(const GradientsRGB* pGradients, 
														   EdgeRGB* pLeft, 
														   EdgeRGB* pRight,
														   Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned short* lDestBits = (unsigned short*)pThis->mScreen->GetBuffer() +
									 (pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart);
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lROverZLeft = pLeft->GetROverZ();
		double lGOverZLeft = pLeft->GetGOverZ();
		double lBOverZLeft = pLeft->GetBOverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lROverZStepAff = pGradients->GetROverZXStep() * lAffineLengthFloat;
		double lGOverZStepAff = pGradients->GetGOverZXStep() * lAffineLengthFloat;
		double lBOverZStepAff = pGradients->GetBOverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lROverZRight = lROverZLeft + lROverZStepAff;
		double lGOverZRight = lGOverZLeft + lGOverZStepAff;
		double lBOverZRight = lBOverZLeft + lBOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lRLeft = lROverZLeft * lZLeft * 31.0f;
		double lGLeft = lGOverZLeft * lZLeft * 63.0f;
		double lBLeft = lBOverZLeft * lZLeft * 31.0f;

		MACRO_CHECK_RGB_16BIT(lRLeft, lGLeft, lBLeft);

		double lZRight;
		double lRRight;
		double lGRight;
		double lBRight;
		Lepra::FixedPointMath::Fixed16_16 lR;
		Lepra::FixedPointMath::Fixed16_16 lG;
		Lepra::FixedPointMath::Fixed16_16 lB;
		Lepra::FixedPointMath::Fixed16_16 lDeltaR = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaG = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaB = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//
		float lOneOverZ = (float)lOneOverZLeft;

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lRRight = lROverZRight * lZRight * 31.0f;
			lGRight = lGOverZRight * lZRight * 63.0f;
			lBRight = lBOverZRight * lZRight * 31.0f;

			MACRO_CHECK_RGB_16BIT(lRRight, lGRight, lBRight);

			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) >> pThis->mAffineExp;
			lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) >> pThis->mAffineExp;
			lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				if (lOneOverZ > *lZBuffer)
				{
					*lDestBits = (unsigned short)(((lR >> 16) << 11) | ((lG >> 16) << 5) | (lB >> 16));
					*lZBuffer = lOneOverZ;
				}
				
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lDestBits++;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}

			lZLeft = lZRight;
			lRLeft = lRRight;
			lGLeft = lGRight;
			lBLeft = lBRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lROverZRight += lROverZStepAff;
			lGOverZRight += lGOverZStepAff;
			lBOverZRight += lBOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lRRight = lZRight * (pRight->GetROverZ() - pGradients->GetROverZXStep()) * 31.0f;
			lGRight = lZRight * (pRight->GetGOverZ() - pGradients->GetGOverZXStep()) * 63.0f;
			lBRight = lZRight * (pRight->GetBOverZ() - pGradients->GetBOverZXStep()) * 31.0f;

			MACRO_CHECK_RGB_16BIT(lRRight, lGRight, lBRight);

			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) / lRestWidth;
				lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) / lRestWidth;
				lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				if (lOneOverZ > *lZBuffer)
				{
					*lDestBits = (unsigned short)(((lR >> 16) << 11) | ((lG >> 16) << 5) | (lB >> 16));
					*lZBuffer = lOneOverZ;
				}
				
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lDestBits++;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}
		}
	}
}

void Software3DPainter::DrawScanLineRGB24BitFastZTZW(const GradientsRGB* pGradients, 
														   EdgeRGB* pLeft, 
														   EdgeRGB* pRight,
														   Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned char* lDestBits = (unsigned char*)pThis->mScreen->GetBuffer() +
									 (pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart) * 3;
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lROverZLeft = pLeft->GetROverZ();
		double lGOverZLeft = pLeft->GetGOverZ();
		double lBOverZLeft = pLeft->GetBOverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lROverZStepAff = pGradients->GetROverZXStep() * lAffineLengthFloat;
		double lGOverZStepAff = pGradients->GetGOverZXStep() * lAffineLengthFloat;
		double lBOverZStepAff = pGradients->GetBOverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lROverZRight = lROverZLeft + lROverZStepAff;
		double lGOverZRight = lGOverZLeft + lGOverZStepAff;
		double lBOverZRight = lBOverZLeft + lBOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lRLeft = lROverZLeft * lZLeft * 255.0f;
		double lGLeft = lGOverZLeft * lZLeft * 255.0f;
		double lBLeft = lBOverZLeft * lZLeft * 255.0f;

		MACRO_CHECK_RGB(lRLeft, lGLeft, lBLeft, 255.0f);

		double lZRight;
		double lRRight;
		double lGRight;
		double lBRight;
		Lepra::FixedPointMath::Fixed16_16 lR;
		Lepra::FixedPointMath::Fixed16_16 lG;
		Lepra::FixedPointMath::Fixed16_16 lB;
		Lepra::FixedPointMath::Fixed16_16 lDeltaR = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaG = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaB = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//

		float lOneOverZ = (float)lOneOverZLeft;

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lRRight = lROverZRight * lZRight * 255.0f;
			lGRight = lGOverZRight * lZRight * 255.0f;
			lBRight = lBOverZRight * lZRight * 255.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 255.0f);

			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) >> pThis->mAffineExp;
			lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) >> pThis->mAffineExp;
			lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				if (lOneOverZ > *lZBuffer)
				{
					lDestBits[0] = (unsigned char)(lB >> 16);
					lDestBits[1] = (unsigned char)(lG >> 16);
					lDestBits[2] = (unsigned char)(lR >> 16);
					*lZBuffer = lOneOverZ;
				}
				
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lDestBits += 3;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}

			lZLeft = lZRight;
			lRLeft = lRRight;
			lGLeft = lGRight;
			lBLeft = lBRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lROverZRight += lROverZStepAff;
			lGOverZRight += lGOverZStepAff;
			lBOverZRight += lBOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lRRight = lZRight * (pRight->GetROverZ() - pGradients->GetROverZXStep()) * 255.0f;
			lGRight = lZRight * (pRight->GetGOverZ() - pGradients->GetGOverZXStep()) * 255.0f;
			lBRight = lZRight * (pRight->GetBOverZ() - pGradients->GetBOverZXStep()) * 255.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 255.0f);

			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) / lRestWidth;
				lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) / lRestWidth;
				lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				if (lOneOverZ > *lZBuffer)
				{
					lDestBits[0] = (unsigned char)(lB >> 16);
					lDestBits[1] = (unsigned char)(lG >> 16);
					lDestBits[2] = (unsigned char)(lR >> 16);
					*lZBuffer = lOneOverZ;
				}
				
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lDestBits += 3;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}
		}
	}
}

void Software3DPainter::DrawScanLineRGB32BitFastZTZW(const GradientsRGB* pGradients, 
														   EdgeRGB* pLeft, 
														   EdgeRGB* pRight,
														   Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned char* lDestBits = (unsigned char*)pThis->mScreen->GetBuffer() +
									 ((pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart) << 2);
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lROverZLeft = pLeft->GetROverZ();
		double lGOverZLeft = pLeft->GetGOverZ();
		double lBOverZLeft = pLeft->GetBOverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lROverZStepAff = pGradients->GetROverZXStep() * lAffineLengthFloat;
		double lGOverZStepAff = pGradients->GetGOverZXStep() * lAffineLengthFloat;
		double lBOverZStepAff = pGradients->GetBOverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lROverZRight = lROverZLeft + lROverZStepAff;
		double lGOverZRight = lGOverZLeft + lGOverZStepAff;
		double lBOverZRight = lBOverZLeft + lBOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lRLeft = lROverZLeft * lZLeft * 255.0f;
		double lGLeft = lGOverZLeft * lZLeft * 255.0f;
		double lBLeft = lBOverZLeft * lZLeft * 255.0f;

		MACRO_CHECK_RGB(lRLeft, lGLeft, lBLeft, 255.0f);

		double lZRight;
		double lRRight;
		double lGRight;
		double lBRight;
		Lepra::FixedPointMath::Fixed16_16 lR;
		Lepra::FixedPointMath::Fixed16_16 lG;
		Lepra::FixedPointMath::Fixed16_16 lB;
		Lepra::FixedPointMath::Fixed16_16 lDeltaR = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaG = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaB = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//

		float lOneOverZ = (float)lOneOverZLeft;

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lRRight = lROverZRight * lZRight * 255.0f;
			lGRight = lGOverZRight * lZRight * 255.0f;
			lBRight = lBOverZRight * lZRight * 255.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 255.0f);

			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) >> pThis->mAffineExp;
			lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) >> pThis->mAffineExp;
			lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				if (lOneOverZ > *lZBuffer)
				{
					lDestBits[0] = (unsigned char)(lB >> 16);
					lDestBits[1] = (unsigned char)(lG >> 16);
					lDestBits[2] = (unsigned char)(lR >> 16);
					*lZBuffer = lOneOverZ;
				}
				
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lDestBits += 4;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}

			lZLeft = lZRight;
			lRLeft = lRRight;
			lGLeft = lGRight;
			lBLeft = lBRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lROverZRight += lROverZStepAff;
			lGOverZRight += lGOverZStepAff;
			lBOverZRight += lBOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lRRight = lZRight * (pRight->GetROverZ() - pGradients->GetROverZXStep()) * 255.0f;
			lGRight = lZRight * (pRight->GetGOverZ() - pGradients->GetGOverZXStep()) * 255.0f;
			lBRight = lZRight * (pRight->GetBOverZ() - pGradients->GetBOverZXStep()) * 255.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 255.0f);

			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) / lRestWidth;
				lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) / lRestWidth;
				lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				if (lOneOverZ > *lZBuffer)
				{
					lDestBits[0] = (unsigned char)(lB >> 16);
					lDestBits[1] = (unsigned char)(lG >> 16);
					lDestBits[2] = (unsigned char)(lR >> 16);
					*lZBuffer = lOneOverZ;
				}
				
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lDestBits += 4;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}
		}
	}
}








///////////////////////////////////////////////////////////////////////////////////
//	
//	UV scan line renderers.
//	
///////////////////////////////////////////////////////////////////////////////////









void Software3DPainter::DrawScanLineUV8BitFast(const GradientsUV* pGradients, 
														  EdgeUV* pLeft, 
														  EdgeUV* pRight,
														  const Lepra::Canvas* pTexture,
														  Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned char* lDestBits = (unsigned char*)pThis->mScreen->GetBuffer() +
									 pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart;
		unsigned char* lTextureBits = (unsigned char*)pTexture->GetBuffer();

		int lTexturePitch = pTexture->GetPitch();
		unsigned int lTWMask = pTexture->GetWidth() - 1;
		unsigned int lTHMask = pTexture->GetHeight() - 1;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lUOverZLeft = pLeft->GetUOverZ();
		double lVOverZLeft = pLeft->GetVOverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lUOverZStepAff = pGradients->GetUOverZXStep() * lAffineLengthFloat;
		double lVOverZStepAff = pGradients->GetVOverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lUOverZRight = lUOverZLeft + lUOverZStepAff;
		double lVOverZRight = lVOverZLeft + lVOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lULeft = lUOverZLeft * lZLeft;
		double lVLeft = lVOverZLeft * lZLeft;

		double lZRight;
		double lURight;
		double lVRight;
		Lepra::FixedPointMath::Fixed16_16 lU;
		Lepra::FixedPointMath::Fixed16_16 lV;
		Lepra::FixedPointMath::Fixed16_16 lDeltaU = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaV = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lURight = lUOverZRight * lZRight;
			lVRight = lVOverZRight * lZRight;

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();

			lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) >> pThis->mAffineExp;
			lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				*(lDestBits++) = lTextureBits[((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)];
				lU += lDeltaU;
				lV += lDeltaV;
			}

			lZLeft = lZRight;
			lULeft = lURight;
			lVLeft = lVRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lUOverZRight += lUOverZStepAff;
			lVOverZRight += lVOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lURight = lZRight * (pRight->GetUOverZ() - pGradients->GetUOverZXStep());
			lVRight = lZRight * (pRight->GetVOverZ() - pGradients->GetVOverZXStep());

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) / lRestWidth;
				lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				*(lDestBits++) = lTextureBits[((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)];
				lU += lDeltaU;
				lV += lDeltaV;
			}
		}
	}
}

void Software3DPainter::DrawScanLineUV16BitFast(const GradientsUV* pGradients, 
														   EdgeUV* pLeft, 
														   EdgeUV* pRight,
														   const Lepra::Canvas* pTexture,
														   Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned short* lDestBits = (unsigned short*)pThis->mScreen->GetBuffer() +
									 pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart;
		unsigned short* lTextureBits = (unsigned short*)pTexture->GetBuffer();

		int lTexturePitch = pTexture->GetPitch();
		unsigned int lTWMask = pTexture->GetWidth() - 1;
		unsigned int lTHMask = pTexture->GetHeight() - 1;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lUOverZLeft = pLeft->GetUOverZ();
		double lVOverZLeft = pLeft->GetVOverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lUOverZStepAff = pGradients->GetUOverZXStep() * lAffineLengthFloat;
		double lVOverZStepAff = pGradients->GetVOverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lUOverZRight = lUOverZLeft + lUOverZStepAff;
		double lVOverZRight = lVOverZLeft + lVOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lULeft = lUOverZLeft * lZLeft;
		double lVLeft = lVOverZLeft * lZLeft;

		double lZRight;
		double lURight;
		double lVRight;
		Lepra::FixedPointMath::Fixed16_16 lU;
		Lepra::FixedPointMath::Fixed16_16 lV;
		Lepra::FixedPointMath::Fixed16_16 lDeltaU = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaV = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lURight = lUOverZRight * lZRight;
			lVRight = lVOverZRight * lZRight;

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();

			lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) >> pThis->mAffineExp;
			lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				*(lDestBits++) = lTextureBits[((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)];
				lU += lDeltaU;
				lV += lDeltaV;
			}

			lZLeft = lZRight;
			lULeft = lURight;
			lVLeft = lVRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lUOverZRight += lUOverZStepAff;
			lVOverZRight += lVOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lURight = lZRight * (pRight->GetUOverZ() - pGradients->GetUOverZXStep());
			lVRight = lZRight * (pRight->GetVOverZ() - pGradients->GetVOverZXStep());

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) / lRestWidth;
				lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				*(lDestBits++) = lTextureBits[((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)];
				lU += lDeltaU;
				lV += lDeltaV;
			}
		}
	}
}

void Software3DPainter::DrawScanLineUV24BitFast(const GradientsUV* pGradients, 
														   EdgeUV* pLeft, 
														   EdgeUV* pRight,
														   const Lepra::Canvas* pTexture,
														   Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned char* lDestBits = (unsigned char*)pThis->mScreen->GetBuffer() +
									 (pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart) * 3;
		unsigned char* lTextureBits = (unsigned char*)pTexture->GetBuffer();

		int lTexturePitch = pTexture->GetPitch();
		unsigned int lTWMask = pTexture->GetWidth() - 1;
		unsigned int lTHMask = pTexture->GetHeight() - 1;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lUOverZLeft = pLeft->GetUOverZ();
		double lVOverZLeft = pLeft->GetVOverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lUOverZStepAff = pGradients->GetUOverZXStep() * lAffineLengthFloat;
		double lVOverZStepAff = pGradients->GetVOverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lUOverZRight = lUOverZLeft + lUOverZStepAff;
		double lVOverZRight = lVOverZLeft + lVOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lULeft = lUOverZLeft * lZLeft;
		double lVLeft = lVOverZLeft * lZLeft;

		double lZRight;
		double lURight;
		double lVRight;
		Lepra::FixedPointMath::Fixed16_16 lU;
		Lepra::FixedPointMath::Fixed16_16 lV;
		Lepra::FixedPointMath::Fixed16_16 lDeltaU = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaV = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lURight = lUOverZRight * lZRight;
			lVRight = lVOverZRight * lZRight;

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();

			lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) >> pThis->mAffineExp;
			lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				int lTextureOffset = (((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)) * 3;
				lDestBits[0] = lTextureBits[lTextureOffset + 0];
				lDestBits[1] = lTextureBits[lTextureOffset + 1];
				lDestBits[2] = lTextureBits[lTextureOffset + 2];
				
				lDestBits += 3;
				lU += lDeltaU;
				lV += lDeltaV;
			}

			lZLeft = lZRight;
			lULeft = lURight;
			lVLeft = lVRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lUOverZRight += lUOverZStepAff;
			lVOverZRight += lVOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lURight = lZRight * (pRight->GetUOverZ() - pGradients->GetUOverZXStep());
			lVRight = lZRight * (pRight->GetVOverZ() - pGradients->GetVOverZXStep());

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) / lRestWidth;
				lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				int lTextureOffset = (((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)) * 3;
				lDestBits[0] = lTextureBits[lTextureOffset + 0];
				lDestBits[1] = lTextureBits[lTextureOffset + 1];
				lDestBits[2] = lTextureBits[lTextureOffset + 2];
				
				lDestBits += 3;
				lU += lDeltaU;
				lV += lDeltaV;
			}
		}
	}
}

void Software3DPainter::DrawScanLineUV32BitFast(const GradientsUV* pGradients, 
														   EdgeUV* pLeft, 
														   EdgeUV* pRight,
														   const Lepra::Canvas* pTexture,
														   Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned int* lDestBits = (unsigned int*)pThis->mScreen->GetBuffer() +
									 pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart;
		unsigned int* lTextureBits = (unsigned int*)pTexture->GetBuffer();

		int lTexturePitch = pTexture->GetPitch();
		unsigned int lTWMask = pTexture->GetWidth() - 1;
		unsigned int lTHMask = pTexture->GetHeight() - 1;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lUOverZLeft = pLeft->GetUOverZ();
		double lVOverZLeft = pLeft->GetVOverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lUOverZStepAff = pGradients->GetUOverZXStep() * lAffineLengthFloat;
		double lVOverZStepAff = pGradients->GetVOverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lUOverZRight = lUOverZLeft + lUOverZStepAff;
		double lVOverZRight = lVOverZLeft + lVOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lULeft = lUOverZLeft * lZLeft;
		double lVLeft = lVOverZLeft * lZLeft;

		double lZRight;
		double lURight;
		double lVRight;
		Lepra::FixedPointMath::Fixed16_16 lU;
		Lepra::FixedPointMath::Fixed16_16 lV;
		Lepra::FixedPointMath::Fixed16_16 lDeltaU = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaV = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lURight = lUOverZRight * lZRight;
			lVRight = lVOverZRight * lZRight;

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();

			lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) >> pThis->mAffineExp;
			lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				*(lDestBits++) = lTextureBits[((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)];
				lU += lDeltaU;
				lV += lDeltaV;
			}

			lZLeft = lZRight;
			lULeft = lURight;
			lVLeft = lVRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lUOverZRight += lUOverZStepAff;
			lVOverZRight += lVOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lURight = lZRight * (pRight->GetUOverZ() - pGradients->GetUOverZXStep());
			lVRight = lZRight * (pRight->GetVOverZ() - pGradients->GetVOverZXStep());

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) / lRestWidth;
				lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				*(lDestBits++) = lTextureBits[((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)];;
				lU += lDeltaU;
				lV += lDeltaV;
			}
		}
	}
}





void Software3DPainter::DrawScanLineUV8BitFastZT(const GradientsUV* pGradients, 
														  EdgeUV* pLeft, 
														  EdgeUV* pRight,
														  const Lepra::Canvas* pTexture,
														  Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned char* lDestBits = (unsigned char*)pThis->mScreen->GetBuffer() +
									 pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart;
		unsigned char* lTextureBits = (unsigned char*)pTexture->GetBuffer();
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		int lTexturePitch = pTexture->GetPitch();
		unsigned int lTWMask = pTexture->GetWidth() - 1;
		unsigned int lTHMask = pTexture->GetHeight() - 1;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lUOverZLeft = pLeft->GetUOverZ();
		double lVOverZLeft = pLeft->GetVOverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lUOverZStepAff = pGradients->GetUOverZXStep() * lAffineLengthFloat;
		double lVOverZStepAff = pGradients->GetVOverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lUOverZRight = lUOverZLeft + lUOverZStepAff;
		double lVOverZRight = lVOverZLeft + lVOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lULeft = lUOverZLeft * lZLeft;
		double lVLeft = lVOverZLeft * lZLeft;

		double lZRight;
		double lURight;
		double lVRight;
		Lepra::FixedPointMath::Fixed16_16 lU;
		Lepra::FixedPointMath::Fixed16_16 lV;
		Lepra::FixedPointMath::Fixed16_16 lDeltaU = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaV = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//

		float lOneOverZ = (float)lOneOverZLeft;

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lURight = lUOverZRight * lZRight;
			lVRight = lVOverZRight * lZRight;

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();

			lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) >> pThis->mAffineExp;
			lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				if (lOneOverZ > *lZBuffer)
				{
					*lDestBits = lTextureBits[((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)];
				}

				lDestBits++;
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lU += lDeltaU;
				lV += lDeltaV;
			}

			lZLeft = lZRight;
			lULeft = lURight;
			lVLeft = lVRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lUOverZRight += lUOverZStepAff;
			lVOverZRight += lVOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lURight = lZRight * (pRight->GetUOverZ() - pGradients->GetUOverZXStep());
			lVRight = lZRight * (pRight->GetVOverZ() - pGradients->GetVOverZXStep());

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) / lRestWidth;
				lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				if (lOneOverZ > *lZBuffer)
				{
					*lDestBits = lTextureBits[((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)];
				}

				lDestBits++;
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lU += lDeltaU;
				lV += lDeltaV;
			}
		}
	}
}

void Software3DPainter::DrawScanLineUV16BitFastZT(const GradientsUV* pGradients, 
														   EdgeUV* pLeft, 
														   EdgeUV* pRight,
														   const Lepra::Canvas* pTexture,
														   Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned short* lDestBits = (unsigned short*)pThis->mScreen->GetBuffer() +
									 pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart;
		unsigned short* lTextureBits = (unsigned short*)pTexture->GetBuffer();
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		int lTexturePitch = pTexture->GetPitch();
		unsigned int lTWMask = pTexture->GetWidth() - 1;
		unsigned int lTHMask = pTexture->GetHeight() - 1;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lUOverZLeft = pLeft->GetUOverZ();
		double lVOverZLeft = pLeft->GetVOverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lUOverZStepAff = pGradients->GetUOverZXStep() * lAffineLengthFloat;
		double lVOverZStepAff = pGradients->GetVOverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lUOverZRight = lUOverZLeft + lUOverZStepAff;
		double lVOverZRight = lVOverZLeft + lVOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lULeft = lUOverZLeft * lZLeft;
		double lVLeft = lVOverZLeft * lZLeft;

		double lZRight;
		double lURight;
		double lVRight;
		Lepra::FixedPointMath::Fixed16_16 lU;
		Lepra::FixedPointMath::Fixed16_16 lV;
		Lepra::FixedPointMath::Fixed16_16 lDeltaU = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaV = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//

		float lOneOverZ = (float)lOneOverZLeft;

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lURight = lUOverZRight * lZRight;
			lVRight = lVOverZRight * lZRight;

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();

			lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) >> pThis->mAffineExp;
			lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				if (lOneOverZ > *lZBuffer)
				{
					*lDestBits = lTextureBits[((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)];
				}

				lDestBits++;
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lU += lDeltaU;
				lV += lDeltaV;
			}

			lZLeft = lZRight;
			lULeft = lURight;
			lVLeft = lVRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lUOverZRight += lUOverZStepAff;
			lVOverZRight += lVOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lURight = lZRight * (pRight->GetUOverZ() - pGradients->GetUOverZXStep());
			lVRight = lZRight * (pRight->GetVOverZ() - pGradients->GetVOverZXStep());

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) / lRestWidth;
				lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				if (lOneOverZ > *lZBuffer)
				{
					*lDestBits = lTextureBits[((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)];
				}

				lDestBits++;
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lU += lDeltaU;
				lV += lDeltaV;
			}
		}
	}
}

void Software3DPainter::DrawScanLineUV24BitFastZT(const GradientsUV* pGradients, 
														   EdgeUV* pLeft, 
														   EdgeUV* pRight,
														   const Lepra::Canvas* pTexture,
														   Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned char* lDestBits = (unsigned char*)pThis->mScreen->GetBuffer() +
									 (pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart) * 3;
		unsigned char* lTextureBits = (unsigned char*)pTexture->GetBuffer();
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		int lTexturePitch = pTexture->GetPitch();
		unsigned int lTWMask = pTexture->GetWidth() - 1;
		unsigned int lTHMask = pTexture->GetHeight() - 1;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lUOverZLeft = pLeft->GetUOverZ();
		double lVOverZLeft = pLeft->GetVOverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lUOverZStepAff = pGradients->GetUOverZXStep() * lAffineLengthFloat;
		double lVOverZStepAff = pGradients->GetVOverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lUOverZRight = lUOverZLeft + lUOverZStepAff;
		double lVOverZRight = lVOverZLeft + lVOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lULeft = lUOverZLeft * lZLeft;
		double lVLeft = lVOverZLeft * lZLeft;

		double lZRight;
		double lURight;
		double lVRight;
		Lepra::FixedPointMath::Fixed16_16 lU;
		Lepra::FixedPointMath::Fixed16_16 lV;
		Lepra::FixedPointMath::Fixed16_16 lDeltaU = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaV = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//

		float lOneOverZ = (float)lOneOverZLeft;

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lURight = lUOverZRight * lZRight;
			lVRight = lVOverZRight * lZRight;

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();

			lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) >> pThis->mAffineExp;
			lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				if (lOneOverZ > *lZBuffer)
				{
					int lTextureOffset = (((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)) * 3;
					lDestBits[0] = lTextureBits[lTextureOffset + 0];
					lDestBits[1] = lTextureBits[lTextureOffset + 1];
					lDestBits[2] = lTextureBits[lTextureOffset + 2];
				}
				
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lDestBits += 3;
				lU += lDeltaU;
				lV += lDeltaV;
			}

			lZLeft = lZRight;
			lULeft = lURight;
			lVLeft = lVRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lUOverZRight += lUOverZStepAff;
			lVOverZRight += lVOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lURight = lZRight * (pRight->GetUOverZ() - pGradients->GetUOverZXStep());
			lVRight = lZRight * (pRight->GetVOverZ() - pGradients->GetVOverZXStep());

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) / lRestWidth;
				lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				if (lOneOverZ > *lZBuffer)
				{
					int lTextureOffset = (((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)) * 3;
					lDestBits[0] = lTextureBits[lTextureOffset + 0];
					lDestBits[1] = lTextureBits[lTextureOffset + 1];
					lDestBits[2] = lTextureBits[lTextureOffset + 2];
				}
				
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lDestBits += 3;
				lU += lDeltaU;
				lV += lDeltaV;
			}
		}
	}
}

void Software3DPainter::DrawScanLineUV32BitFastZT(const GradientsUV* pGradients, 
														   EdgeUV* pLeft, 
														   EdgeUV* pRight,
														   const Lepra::Canvas* pTexture,
														   Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned int* lDestBits = (unsigned int*)pThis->mScreen->GetBuffer() +
									 pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart;
		unsigned int* lTextureBits = (unsigned int*)pTexture->GetBuffer();
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		int lTexturePitch = pTexture->GetPitch();
		unsigned int lTWMask = pTexture->GetWidth() - 1;
		unsigned int lTHMask = pTexture->GetHeight() - 1;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lUOverZLeft = pLeft->GetUOverZ();
		double lVOverZLeft = pLeft->GetVOverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lUOverZStepAff = pGradients->GetUOverZXStep() * lAffineLengthFloat;
		double lVOverZStepAff = pGradients->GetVOverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lUOverZRight = lUOverZLeft + lUOverZStepAff;
		double lVOverZRight = lVOverZLeft + lVOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lULeft = lUOverZLeft * lZLeft;
		double lVLeft = lVOverZLeft * lZLeft;

		double lZRight;
		double lURight;
		double lVRight;
		Lepra::FixedPointMath::Fixed16_16 lU;
		Lepra::FixedPointMath::Fixed16_16 lV;
		Lepra::FixedPointMath::Fixed16_16 lDeltaU = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaV = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//

		float lOneOverZ = (float)lOneOverZLeft;

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lURight = lUOverZRight * lZRight;
			lVRight = lVOverZRight * lZRight;

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();

			lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) >> pThis->mAffineExp;
			lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				if (lOneOverZ > *lZBuffer)
				{
					*lDestBits = lTextureBits[((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)];
				}
				
				lDestBits++;
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lU += lDeltaU;
				lV += lDeltaV;
			}

			lZLeft = lZRight;
			lULeft = lURight;
			lVLeft = lVRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lUOverZRight += lUOverZStepAff;
			lVOverZRight += lVOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lURight = lZRight * (pRight->GetUOverZ() - pGradients->GetUOverZXStep());
			lVRight = lZRight * (pRight->GetVOverZ() - pGradients->GetVOverZXStep());

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) / lRestWidth;
				lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				if (lOneOverZ > *lZBuffer)
				{
					*lDestBits = lTextureBits[((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)];
				}
				
				lDestBits++;
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lU += lDeltaU;
				lV += lDeltaV;
			}
		}
	}
}







void Software3DPainter::DrawScanLineUV8BitFastZW(const GradientsUV* pGradients, 
														  EdgeUV* pLeft, 
														  EdgeUV* pRight,
														  const Lepra::Canvas* pTexture,
														  Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned char* lDestBits = (unsigned char*)pThis->mScreen->GetBuffer() +
									 pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart;
		unsigned char* lTextureBits = (unsigned char*)pTexture->GetBuffer();
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		int lTexturePitch = pTexture->GetPitch();
		unsigned int lTWMask = pTexture->GetWidth() - 1;
		unsigned int lTHMask = pTexture->GetHeight() - 1;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lUOverZLeft = pLeft->GetUOverZ();
		double lVOverZLeft = pLeft->GetVOverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lUOverZStepAff = pGradients->GetUOverZXStep() * lAffineLengthFloat;
		double lVOverZStepAff = pGradients->GetVOverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lUOverZRight = lUOverZLeft + lUOverZStepAff;
		double lVOverZRight = lVOverZLeft + lVOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lULeft = lUOverZLeft * lZLeft;
		double lVLeft = lVOverZLeft * lZLeft;

		double lZRight;
		double lURight;
		double lVRight;
		Lepra::FixedPointMath::Fixed16_16 lU;
		Lepra::FixedPointMath::Fixed16_16 lV;
		Lepra::FixedPointMath::Fixed16_16 lDeltaU = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaV = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//

		float lOneOverZ = (float)lOneOverZLeft;

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lURight = lUOverZRight * lZRight;
			lVRight = lVOverZRight * lZRight;

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();

			lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) >> pThis->mAffineExp;
			lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				*(lDestBits++) = lTextureBits[((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)];
				*lZBuffer = lOneOverZ;

				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lU += lDeltaU;
				lV += lDeltaV;
			}

			lZLeft = lZRight;
			lULeft = lURight;
			lVLeft = lVRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lUOverZRight += lUOverZStepAff;
			lVOverZRight += lVOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lURight = lZRight * (pRight->GetUOverZ() - pGradients->GetUOverZXStep());
			lVRight = lZRight * (pRight->GetVOverZ() - pGradients->GetVOverZXStep());

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) / lRestWidth;
				lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				*(lDestBits++) = lTextureBits[((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)];
				*lZBuffer = lOneOverZ;

				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lU += lDeltaU;
				lV += lDeltaV;
			}
		}
	}
}

void Software3DPainter::DrawScanLineUV16BitFastZW(const GradientsUV* pGradients, 
														   EdgeUV* pLeft, 
														   EdgeUV* pRight,
														   const Lepra::Canvas* pTexture,
														   Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned short* lDestBits = (unsigned short*)pThis->mScreen->GetBuffer() +
									 pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart;
		unsigned short* lTextureBits = (unsigned short*)pTexture->GetBuffer();
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		int lTexturePitch = pTexture->GetPitch();
		unsigned int lTWMask = pTexture->GetWidth() - 1;
		unsigned int lTHMask = pTexture->GetHeight() - 1;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lUOverZLeft = pLeft->GetUOverZ();
		double lVOverZLeft = pLeft->GetVOverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lUOverZStepAff = pGradients->GetUOverZXStep() * lAffineLengthFloat;
		double lVOverZStepAff = pGradients->GetVOverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lUOverZRight = lUOverZLeft + lUOverZStepAff;
		double lVOverZRight = lVOverZLeft + lVOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lULeft = lUOverZLeft * lZLeft;
		double lVLeft = lVOverZLeft * lZLeft;

		double lZRight;
		double lURight;
		double lVRight;
		Lepra::FixedPointMath::Fixed16_16 lU;
		Lepra::FixedPointMath::Fixed16_16 lV;
		Lepra::FixedPointMath::Fixed16_16 lDeltaU = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaV = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//

		float lOneOverZ = (float)lOneOverZLeft;

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lURight = lUOverZRight * lZRight;
			lVRight = lVOverZRight * lZRight;

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();

			lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) >> pThis->mAffineExp;
			lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				*(lDestBits++) = lTextureBits[((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)];
				*lZBuffer = lOneOverZ;

				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lU += lDeltaU;
				lV += lDeltaV;
			}

			lZLeft = lZRight;
			lULeft = lURight;
			lVLeft = lVRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lUOverZRight += lUOverZStepAff;
			lVOverZRight += lVOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lURight = lZRight * (pRight->GetUOverZ() - pGradients->GetUOverZXStep());
			lVRight = lZRight * (pRight->GetVOverZ() - pGradients->GetVOverZXStep());

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) / lRestWidth;
				lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				*(lDestBits++) = lTextureBits[((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)];
				*lZBuffer = lOneOverZ;

				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lU += lDeltaU;
				lV += lDeltaV;
			}
		}
	}
}

void Software3DPainter::DrawScanLineUV24BitFastZW(const GradientsUV* pGradients, 
														   EdgeUV* pLeft, 
														   EdgeUV* pRight,
														   const Lepra::Canvas* pTexture,
														   Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned char* lDestBits = (unsigned char*)pThis->mScreen->GetBuffer() +
									 (pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart) * 3;
		unsigned char* lTextureBits = (unsigned char*)pTexture->GetBuffer();
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		int lTexturePitch = pTexture->GetPitch();
		unsigned int lTWMask = pTexture->GetWidth() - 1;
		unsigned int lTHMask = pTexture->GetHeight() - 1;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lUOverZLeft = pLeft->GetUOverZ();
		double lVOverZLeft = pLeft->GetVOverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lUOverZStepAff = pGradients->GetUOverZXStep() * lAffineLengthFloat;
		double lVOverZStepAff = pGradients->GetVOverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lUOverZRight = lUOverZLeft + lUOverZStepAff;
		double lVOverZRight = lVOverZLeft + lVOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lULeft = lUOverZLeft * lZLeft;
		double lVLeft = lVOverZLeft * lZLeft;

		double lZRight;
		double lURight;
		double lVRight;
		Lepra::FixedPointMath::Fixed16_16 lU;
		Lepra::FixedPointMath::Fixed16_16 lV;
		Lepra::FixedPointMath::Fixed16_16 lDeltaU = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaV = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//

		float lOneOverZ = (float)lOneOverZLeft;

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lURight = lUOverZRight * lZRight;
			lVRight = lVOverZRight * lZRight;

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();

			lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) >> pThis->mAffineExp;
			lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				int lTextureOffset = (((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)) * 3;
				lDestBits[0] = lTextureBits[lTextureOffset + 0];
				lDestBits[1] = lTextureBits[lTextureOffset + 1];
				lDestBits[2] = lTextureBits[lTextureOffset + 2];

				*lZBuffer = lOneOverZ;
				
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lDestBits += 3;
				lU += lDeltaU;
				lV += lDeltaV;
			}

			lZLeft = lZRight;
			lULeft = lURight;
			lVLeft = lVRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lUOverZRight += lUOverZStepAff;
			lVOverZRight += lVOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lURight = lZRight * (pRight->GetUOverZ() - pGradients->GetUOverZXStep());
			lVRight = lZRight * (pRight->GetVOverZ() - pGradients->GetVOverZXStep());

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) / lRestWidth;
				lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				int lTextureOffset = (((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)) * 3;
				lDestBits[0] = lTextureBits[lTextureOffset + 0];
				lDestBits[1] = lTextureBits[lTextureOffset + 1];
				lDestBits[2] = lTextureBits[lTextureOffset + 2];

				*lZBuffer = lOneOverZ;
				
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lDestBits += 3;
				lU += lDeltaU;
				lV += lDeltaV;
			}
		}
	}
}

void Software3DPainter::DrawScanLineUV32BitFastZW(const GradientsUV* pGradients, 
														   EdgeUV* pLeft, 
														   EdgeUV* pRight,
														   const Lepra::Canvas* pTexture,
														   Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned int* lDestBits = (unsigned int*)pThis->mScreen->GetBuffer() +
									 pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart;
		unsigned int* lTextureBits = (unsigned int*)pTexture->GetBuffer();
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		int lTexturePitch = pTexture->GetPitch();
		unsigned int lTWMask = pTexture->GetWidth() - 1;
		unsigned int lTHMask = pTexture->GetHeight() - 1;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lUOverZLeft = pLeft->GetUOverZ();
		double lVOverZLeft = pLeft->GetVOverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lUOverZStepAff = pGradients->GetUOverZXStep() * lAffineLengthFloat;
		double lVOverZStepAff = pGradients->GetVOverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lUOverZRight = lUOverZLeft + lUOverZStepAff;
		double lVOverZRight = lVOverZLeft + lVOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lULeft = lUOverZLeft * lZLeft;
		double lVLeft = lVOverZLeft * lZLeft;

		double lZRight;
		double lURight;
		double lVRight;
		Lepra::FixedPointMath::Fixed16_16 lU;
		Lepra::FixedPointMath::Fixed16_16 lV;
		Lepra::FixedPointMath::Fixed16_16 lDeltaU = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaV = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//

		float lOneOverZ = (float)lOneOverZLeft;

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lURight = lUOverZRight * lZRight;
			lVRight = lVOverZRight * lZRight;

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();

			lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) >> pThis->mAffineExp;
			lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				*(lDestBits++) = lTextureBits[((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)];
				*lZBuffer = lOneOverZ;
				
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lU += lDeltaU;
				lV += lDeltaV;
			}

			lZLeft = lZRight;
			lULeft = lURight;
			lVLeft = lVRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lUOverZRight += lUOverZStepAff;
			lVOverZRight += lVOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lURight = lZRight * (pRight->GetUOverZ() - pGradients->GetUOverZXStep());
			lVRight = lZRight * (pRight->GetVOverZ() - pGradients->GetVOverZXStep());

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) / lRestWidth;
				lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				*(lDestBits++) = lTextureBits[((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)];
				*lZBuffer = lOneOverZ;
				
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lU += lDeltaU;
				lV += lDeltaV;
			}
		}
	}
}






void Software3DPainter::DrawScanLineUV8BitFastZTZW(const GradientsUV* pGradients, 
														  EdgeUV* pLeft, 
														  EdgeUV* pRight,
														  const Lepra::Canvas* pTexture,
														  Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned char* lDestBits = (unsigned char*)pThis->mScreen->GetBuffer() +
									 pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart;
		unsigned char* lTextureBits = (unsigned char*)pTexture->GetBuffer();
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		int lTexturePitch = pTexture->GetPitch();
		unsigned int lTWMask = pTexture->GetWidth() - 1;
		unsigned int lTHMask = pTexture->GetHeight() - 1;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lUOverZLeft = pLeft->GetUOverZ();
		double lVOverZLeft = pLeft->GetVOverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lUOverZStepAff = pGradients->GetUOverZXStep() * lAffineLengthFloat;
		double lVOverZStepAff = pGradients->GetVOverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lUOverZRight = lUOverZLeft + lUOverZStepAff;
		double lVOverZRight = lVOverZLeft + lVOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lULeft = lUOverZLeft * lZLeft;
		double lVLeft = lVOverZLeft * lZLeft;

		double lZRight;
		double lURight;
		double lVRight;
		Lepra::FixedPointMath::Fixed16_16 lU;
		Lepra::FixedPointMath::Fixed16_16 lV;
		Lepra::FixedPointMath::Fixed16_16 lDeltaU = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaV = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//

		float lOneOverZ = (float)lOneOverZLeft;

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lURight = lUOverZRight * lZRight;
			lVRight = lVOverZRight * lZRight;

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();

			lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) >> pThis->mAffineExp;
			lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				if (lOneOverZ > *lZBuffer)
				{
					*lDestBits = lTextureBits[((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)];
					*lZBuffer = lOneOverZ;
				}

				lDestBits++;
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lU += lDeltaU;
				lV += lDeltaV;
			}

			lZLeft = lZRight;
			lULeft = lURight;
			lVLeft = lVRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lUOverZRight += lUOverZStepAff;
			lVOverZRight += lVOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lURight = lZRight * (pRight->GetUOverZ() - pGradients->GetUOverZXStep());
			lVRight = lZRight * (pRight->GetVOverZ() - pGradients->GetVOverZXStep());

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) / lRestWidth;
				lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				if (lOneOverZ > *lZBuffer)
				{
					*lDestBits = lTextureBits[((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)];
					*lZBuffer = lOneOverZ;
				}

				lDestBits++;
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lU += lDeltaU;
				lV += lDeltaV;
			}
		}
	}
}

void Software3DPainter::DrawScanLineUV16BitFastZTZW(const GradientsUV* pGradients, 
														   EdgeUV* pLeft, 
														   EdgeUV* pRight,
														   const Lepra::Canvas* pTexture,
														   Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned short* lDestBits = (unsigned short*)pThis->mScreen->GetBuffer() +
									 pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart;
		unsigned short* lTextureBits = (unsigned short*)pTexture->GetBuffer();
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		int lTexturePitch = pTexture->GetPitch();
		unsigned int lTWMask = pTexture->GetWidth() - 1;
		unsigned int lTHMask = pTexture->GetHeight() - 1;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lUOverZLeft = pLeft->GetUOverZ();
		double lVOverZLeft = pLeft->GetVOverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lUOverZStepAff = pGradients->GetUOverZXStep() * lAffineLengthFloat;
		double lVOverZStepAff = pGradients->GetVOverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lUOverZRight = lUOverZLeft + lUOverZStepAff;
		double lVOverZRight = lVOverZLeft + lVOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lULeft = lUOverZLeft * lZLeft;
		double lVLeft = lVOverZLeft * lZLeft;

		double lZRight;
		double lURight;
		double lVRight;
		Lepra::FixedPointMath::Fixed16_16 lU;
		Lepra::FixedPointMath::Fixed16_16 lV;
		Lepra::FixedPointMath::Fixed16_16 lDeltaU = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaV = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//

		float lOneOverZ = (float)lOneOverZLeft;

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lURight = lUOverZRight * lZRight;
			lVRight = lVOverZRight * lZRight;

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();

			lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) >> pThis->mAffineExp;
			lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				if (lOneOverZ > *lZBuffer)
				{
					*lDestBits = lTextureBits[((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)];
					*lZBuffer = lOneOverZ;
				}

				lDestBits++;
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lU += lDeltaU;
				lV += lDeltaV;
			}

			lZLeft = lZRight;
			lULeft = lURight;
			lVLeft = lVRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lUOverZRight += lUOverZStepAff;
			lVOverZRight += lVOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lURight = lZRight * (pRight->GetUOverZ() - pGradients->GetUOverZXStep());
			lVRight = lZRight * (pRight->GetVOverZ() - pGradients->GetVOverZXStep());

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) / lRestWidth;
				lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				if (lOneOverZ > *lZBuffer)
				{
					*lDestBits = lTextureBits[((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)];
					*lZBuffer = lOneOverZ;
				}

				lDestBits++;
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lU += lDeltaU;
				lV += lDeltaV;
			}
		}
	}
}

void Software3DPainter::DrawScanLineUV24BitFastZTZW(const GradientsUV* pGradients, 
														   EdgeUV* pLeft, 
														   EdgeUV* pRight,
														   const Lepra::Canvas* pTexture,
														   Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned char* lDestBits = (unsigned char*)pThis->mScreen->GetBuffer() +
									 (pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart) * 3;
		unsigned char* lTextureBits = (unsigned char*)pTexture->GetBuffer();
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		int lTexturePitch = pTexture->GetPitch();
		unsigned int lTWMask = pTexture->GetWidth() - 1;
		unsigned int lTHMask = pTexture->GetHeight() - 1;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lUOverZLeft = pLeft->GetUOverZ();
		double lVOverZLeft = pLeft->GetVOverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lUOverZStepAff = pGradients->GetUOverZXStep() * lAffineLengthFloat;
		double lVOverZStepAff = pGradients->GetVOverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lUOverZRight = lUOverZLeft + lUOverZStepAff;
		double lVOverZRight = lVOverZLeft + lVOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lULeft = lUOverZLeft * lZLeft;
		double lVLeft = lVOverZLeft * lZLeft;

		double lZRight;
		double lURight;
		double lVRight;
		Lepra::FixedPointMath::Fixed16_16 lU;
		Lepra::FixedPointMath::Fixed16_16 lV;
		Lepra::FixedPointMath::Fixed16_16 lDeltaU = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaV = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//

		float lOneOverZ = (float)lOneOverZLeft;

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lURight = lUOverZRight * lZRight;
			lVRight = lVOverZRight * lZRight;

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();

			lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) >> pThis->mAffineExp;
			lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				if (lOneOverZ > *lZBuffer)
				{
					int lTextureOffset = (((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)) * 3;
					lDestBits[0] = lTextureBits[lTextureOffset + 0];
					lDestBits[1] = lTextureBits[lTextureOffset + 1];
					lDestBits[2] = lTextureBits[lTextureOffset + 2];

					*lZBuffer = lOneOverZ;
				}
				
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lDestBits += 3;
				lU += lDeltaU;
				lV += lDeltaV;
			}

			lZLeft = lZRight;
			lULeft = lURight;
			lVLeft = lVRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lUOverZRight += lUOverZStepAff;
			lVOverZRight += lVOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lURight = lZRight * (pRight->GetUOverZ() - pGradients->GetUOverZXStep());
			lVRight = lZRight * (pRight->GetVOverZ() - pGradients->GetVOverZXStep());

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) / lRestWidth;
				lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				if (lOneOverZ > *lZBuffer)
				{
					int lTextureOffset = (((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)) * 3;
					lDestBits[0] = lTextureBits[lTextureOffset + 0];
					lDestBits[1] = lTextureBits[lTextureOffset + 1];
					lDestBits[2] = lTextureBits[lTextureOffset + 2];

					*lZBuffer = lOneOverZ;
				}
				
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lDestBits += 3;
				lU += lDeltaU;
				lV += lDeltaV;
			}
		}
	}
}

void Software3DPainter::DrawScanLineUV32BitFastZTZW(const GradientsUV* pGradients, 
														   EdgeUV* pLeft, 
														   EdgeUV* pRight,
														   const Lepra::Canvas* pTexture,
														   Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned int* lDestBits = (unsigned int*)pThis->mScreen->GetBuffer() +
									 pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart;
		unsigned int* lTextureBits = (unsigned int*)pTexture->GetBuffer();
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		int lTexturePitch = pTexture->GetPitch();
		unsigned int lTWMask = pTexture->GetWidth() - 1;
		unsigned int lTHMask = pTexture->GetHeight() - 1;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lUOverZLeft = pLeft->GetUOverZ();
		double lVOverZLeft = pLeft->GetVOverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lUOverZStepAff = pGradients->GetUOverZXStep() * lAffineLengthFloat;
		double lVOverZStepAff = pGradients->GetVOverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lUOverZRight = lUOverZLeft + lUOverZStepAff;
		double lVOverZRight = lVOverZLeft + lVOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lULeft = lUOverZLeft * lZLeft;
		double lVLeft = lVOverZLeft * lZLeft;

		double lZRight;
		double lURight;
		double lVRight;
		Lepra::FixedPointMath::Fixed16_16 lU;
		Lepra::FixedPointMath::Fixed16_16 lV;
		Lepra::FixedPointMath::Fixed16_16 lDeltaU = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaV = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//

		float lOneOverZ = (float)lOneOverZLeft;

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lURight = lUOverZRight * lZRight;
			lVRight = lVOverZRight * lZRight;

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();

			lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) >> pThis->mAffineExp;
			lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				if (lOneOverZ > *lZBuffer)
				{
					*lDestBits = lTextureBits[((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)];
					*lZBuffer = lOneOverZ;
				}
				
				lDestBits++;
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lU += lDeltaU;
				lV += lDeltaV;
			}

			lZLeft = lZRight;
			lULeft = lURight;
			lVLeft = lVRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lUOverZRight += lUOverZStepAff;
			lVOverZRight += lVOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lURight = lZRight * (pRight->GetUOverZ() - pGradients->GetUOverZXStep());
			lVRight = lZRight * (pRight->GetVOverZ() - pGradients->GetVOverZXStep());

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) / lRestWidth;
				lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				if (lOneOverZ > *lZBuffer)
				{
					*lDestBits = lTextureBits[((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)];
					*lZBuffer = lOneOverZ;
				}
				
				lDestBits++;
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lU += lDeltaU;
				lV += lDeltaV;
			}
		}
	}
}










///////////////////////////////////////////////////////////////////////////////////
//	
//	UVRGB scan line renderers.
//	
///////////////////////////////////////////////////////////////////////////////////











void Software3DPainter::DrawScanLineUVRGB8BitFast(const GradientsUVRGB* pGradients, 
														  EdgeUVRGB* pLeft, 
														  EdgeUVRGB* pRight,
														  const Lepra::Canvas* pTexture,
														  Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned char* lDestBits = (unsigned char*)pThis->mScreen->GetBuffer() +
									 pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart;
		unsigned char* lTextureBits = (unsigned char*)pTexture->GetBuffer();

		int lTexturePitch = pTexture->GetPitch();
		unsigned int lTWMask = pTexture->GetWidth() - 1;
		unsigned int lTHMask = pTexture->GetHeight() - 1;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lUOverZLeft = pLeft->GetUOverZ();
		double lVOverZLeft = pLeft->GetVOverZ();
		double lLightOverZLeft = pLeft->GetROverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lUOverZStepAff = pGradients->GetUOverZXStep() * lAffineLengthFloat;
		double lVOverZStepAff = pGradients->GetVOverZXStep() * lAffineLengthFloat;
		double lLightOverZStepAff = pGradients->GetROverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lUOverZRight = lUOverZLeft + lUOverZStepAff;
		double lVOverZRight = lVOverZLeft + lVOverZStepAff;
		double lLightOverZRight = lLightOverZLeft + lLightOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lULeft = lUOverZLeft * lZLeft;
		double lVLeft = lVOverZLeft * lZLeft;
		double lLightLeft = lLightOverZLeft * lZLeft;

		double lZRight;
		double lURight;
		double lVRight;
		double lLightRight;
		Lepra::FixedPointMath::Fixed16_16 lU;
		Lepra::FixedPointMath::Fixed16_16 lV;
		Lepra::FixedPointMath::Fixed16_16 lL;
		Lepra::FixedPointMath::Fixed16_16 lDeltaU = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaV = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaL = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//

		unsigned char* l8BitLightBuffer = pThis->m8BitLightBuffer;

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lURight = lUOverZRight * lZRight;
			lVRight = lVOverZRight * lZRight;
			lLightRight = lLightOverZRight * lZRight;

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();
			lL = Lepra::FixedPointMath::FloatToFixed16_16(lLightLeft * 0.9999999f);

			lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) >> pThis->mAffineExp;
			lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) >> pThis->mAffineExp;
			lDeltaL = Lepra::FixedPointMath::FloatToFixed16_16(lLightRight - lLightLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				*(lDestBits++) = l8BitLightBuffer[(lL & 0xFF00) + lTextureBits[((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)]];

				lU += lDeltaU;
				lV += lDeltaV;
				lL += lDeltaL;
			}

			lZLeft = lZRight;
			lULeft = lURight;
			lVLeft = lVRight;
			lLightLeft = lLightRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lUOverZRight += lUOverZStepAff;
			lVOverZRight += lVOverZStepAff;
			lLightOverZRight += lLightOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lURight = lZRight * (pRight->GetUOverZ() - pGradients->GetUOverZXStep());
			lVRight = lZRight * (pRight->GetVOverZ() - pGradients->GetVOverZXStep());
			lLightRight = lZRight * (pRight->GetROverZ() - pGradients->GetROverZXStep());

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();
			lL = Lepra::FixedPointMath::FloatToFixed16_16(lLightLeft * 0.9999999f);

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) / lRestWidth;
				lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) / lRestWidth;
				lDeltaL = Lepra::FixedPointMath::FloatToFixed16_16(lLightRight - lLightLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				*(lDestBits++) = l8BitLightBuffer[(lL & 0xFF00) + lTextureBits[((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)]];

				lU += lDeltaU;
				lV += lDeltaV;
				lL += lDeltaL;
			}
		}
	}
}

void Software3DPainter::DrawScanLineUVRGB15BitFast(const GradientsUVRGB* pGradients, 
														   EdgeUVRGB* pLeft, 
														   EdgeUVRGB* pRight,
														   const Lepra::Canvas* pTexture,
														   Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned short* lDestBits = (unsigned short*)pThis->mScreen->GetBuffer() +
									 (pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart);
		unsigned short* lTextureBits = (unsigned short*)pTexture->GetBuffer();

		int lTexturePitch = pTexture->GetPitch();
		unsigned int lTWMask = pTexture->GetWidth() - 1;
		unsigned int lTHMask = pTexture->GetHeight() - 1;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lUOverZLeft = pLeft->GetUOverZ();
		double lVOverZLeft = pLeft->GetVOverZ();
		double lROverZLeft = pLeft->GetROverZ();
		double lGOverZLeft = pLeft->GetGOverZ();
		double lBOverZLeft = pLeft->GetBOverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lUOverZStepAff = pGradients->GetUOverZXStep() * lAffineLengthFloat;
		double lVOverZStepAff = pGradients->GetVOverZXStep() * lAffineLengthFloat;
		double lROverZStepAff = pGradients->GetROverZXStep() * lAffineLengthFloat;
		double lGOverZStepAff = pGradients->GetGOverZXStep() * lAffineLengthFloat;
		double lBOverZStepAff = pGradients->GetBOverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lUOverZRight = lUOverZLeft + lUOverZStepAff;
		double lVOverZRight = lVOverZLeft + lVOverZStepAff;
		double lROverZRight = lROverZLeft + lROverZStepAff;
		double lGOverZRight = lGOverZLeft + lGOverZStepAff;
		double lBOverZRight = lBOverZLeft + lBOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lULeft = lUOverZLeft * lZLeft;
		double lVLeft = lVOverZLeft * lZLeft;
		double lRLeft = lROverZLeft * lZLeft * 255.0f;
		double lGLeft = lGOverZLeft * lZLeft * 255.0f;
		double lBLeft = lBOverZLeft * lZLeft * 255.0f;

		MACRO_CHECK_RGB(lRLeft, lGLeft, lBLeft, 255.0f);

		double lZRight;
		double lURight;
		double lVRight;
		double lRRight;
		double lGRight;
		double lBRight;
		Lepra::FixedPointMath::Fixed16_16 lU;
		Lepra::FixedPointMath::Fixed16_16 lV;
		Lepra::FixedPointMath::Fixed16_16 lR;
		Lepra::FixedPointMath::Fixed16_16 lG;
		Lepra::FixedPointMath::Fixed16_16 lB;
		Lepra::FixedPointMath::Fixed16_16 lDeltaU = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaV = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaR = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaG = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaB = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lURight = lUOverZRight * lZRight;
			lVRight = lVOverZRight * lZRight;
			lRRight = lROverZRight * lZRight * 255.0f;
			lGRight = lGOverZRight * lZRight * 255.0f;
			lBRight = lBOverZRight * lZRight * 255.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 255.0f);

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();
			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) >> pThis->mAffineExp;
			lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) >> pThis->mAffineExp;
			lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) >> pThis->mAffineExp;
			lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) >> pThis->mAffineExp;
			lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				unsigned int lCol = lTextureBits[((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)];
				
				int lRed   = (lR * (((lCol >> 10) & 31) << 3)) >> 24;
				int lGreen = (lG * (((lCol >> 5) & 31) << 3)) >> 24;
				int lBlue  = (lB * (((lCol >> 0) & 31) << 3)) >> 24;

				*lDestBits = (Lepra::uint16)(((lRed >> 3) << 10) | ((lGreen >> 3) << 5) | (lBlue >> 3));

				lDestBits++;

				lU += lDeltaU;
				lV += lDeltaV;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}

			lZLeft = lZRight;
			lULeft = lURight;
			lVLeft = lVRight;
			lRLeft = lRRight;
			lGLeft = lGRight;
			lBLeft = lBRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lUOverZRight += lUOverZStepAff;
			lVOverZRight += lVOverZStepAff;
			lROverZRight += lROverZStepAff;
			lGOverZRight += lGOverZStepAff;
			lBOverZRight += lBOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lURight = lZRight * (pRight->GetUOverZ() - pGradients->GetUOverZXStep());
			lVRight = lZRight * (pRight->GetVOverZ() - pGradients->GetVOverZXStep());
			lRRight = lZRight * (pRight->GetROverZ() - pGradients->GetROverZXStep()) * 255.0f;
			lGRight = lZRight * (pRight->GetGOverZ() - pGradients->GetGOverZXStep()) * 255.0f;
			lBRight = lZRight * (pRight->GetBOverZ() - pGradients->GetBOverZXStep()) * 255.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 255.0f);

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();
			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) / lRestWidth;
				lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) / lRestWidth;
				lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) / lRestWidth;
				lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) / lRestWidth;
				lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				unsigned int lCol = lTextureBits[((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)];
				
				int lRed   = (lR * (((lCol >> 10) & 31) << 3)) >> 24;
				int lGreen = (lG * (((lCol >> 5) & 31) << 3)) >> 24;
				int lBlue  = (lB * (((lCol >> 0) & 31) << 3)) >> 24;

				*lDestBits = (Lepra::uint16)(((lRed >> 3) << 10) | ((lGreen >> 3) << 5) | (lBlue >> 3));

				lDestBits++;

				lU += lDeltaU;
				lV += lDeltaV;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}
		}
	}
}

void Software3DPainter::DrawScanLineUVRGB16BitFast(const GradientsUVRGB* pGradients, 
														   EdgeUVRGB* pLeft, 
														   EdgeUVRGB* pRight,
														   const Lepra::Canvas* pTexture,
														   Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned short* lDestBits = (unsigned short*)pThis->mScreen->GetBuffer() +
									 (pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart);
		unsigned short* lTextureBits = (unsigned short*)pTexture->GetBuffer();

		int lTexturePitch = pTexture->GetPitch();
		unsigned int lTWMask = pTexture->GetWidth() - 1;
		unsigned int lTHMask = pTexture->GetHeight() - 1;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lUOverZLeft = pLeft->GetUOverZ();
		double lVOverZLeft = pLeft->GetVOverZ();
		double lROverZLeft = pLeft->GetROverZ();
		double lGOverZLeft = pLeft->GetGOverZ();
		double lBOverZLeft = pLeft->GetBOverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lUOverZStepAff = pGradients->GetUOverZXStep() * lAffineLengthFloat;
		double lVOverZStepAff = pGradients->GetVOverZXStep() * lAffineLengthFloat;
		double lROverZStepAff = pGradients->GetROverZXStep() * lAffineLengthFloat;
		double lGOverZStepAff = pGradients->GetGOverZXStep() * lAffineLengthFloat;
		double lBOverZStepAff = pGradients->GetBOverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lUOverZRight = lUOverZLeft + lUOverZStepAff;
		double lVOverZRight = lVOverZLeft + lVOverZStepAff;
		double lROverZRight = lROverZLeft + lROverZStepAff;
		double lGOverZRight = lGOverZLeft + lGOverZStepAff;
		double lBOverZRight = lBOverZLeft + lBOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lULeft = lUOverZLeft * lZLeft;
		double lVLeft = lVOverZLeft * lZLeft;
		double lRLeft = lROverZLeft * lZLeft * 255.0f;
		double lGLeft = lGOverZLeft * lZLeft * 255.0f;
		double lBLeft = lBOverZLeft * lZLeft * 255.0f;

		MACRO_CHECK_RGB(lRLeft, lGLeft, lBLeft, 255);

		double lZRight;
		double lURight;
		double lVRight;
		double lRRight;
		double lGRight;
		double lBRight;
		Lepra::FixedPointMath::Fixed16_16 lU;
		Lepra::FixedPointMath::Fixed16_16 lV;
		Lepra::FixedPointMath::Fixed16_16 lR;
		Lepra::FixedPointMath::Fixed16_16 lG;
		Lepra::FixedPointMath::Fixed16_16 lB;
		Lepra::FixedPointMath::Fixed16_16 lDeltaU = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaV = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaR = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaG = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaB = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lURight = lUOverZRight * lZRight;
			lVRight = lVOverZRight * lZRight;
			lRRight = lROverZRight * lZRight * 255.0f;
			lGRight = lGOverZRight * lZRight * 255.0f;
			lBRight = lBOverZRight * lZRight * 255.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 255.0f);

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();
			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) >> pThis->mAffineExp;
			lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) >> pThis->mAffineExp;
			lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) >> pThis->mAffineExp;
			lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) >> pThis->mAffineExp;
			lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				unsigned int lCol = lTextureBits[((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)];
				
				int lRed   = (lR * (((lCol >> 11) & 31) << 3)) >> 24;
				int lGreen = (lG * (((lCol >> 5) & 63) << 2)) >> 24;
				int lBlue  = (lB * (((lCol >> 0) & 31) << 3)) >> 24;

				*lDestBits = (Lepra::uint16)(((lRed >> 3) << 11) | ((lGreen >> 2) << 5) | (lBlue >> 3));

				lDestBits++;

				lU += lDeltaU;
				lV += lDeltaV;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}

			lZLeft = lZRight;
			lULeft = lURight;
			lVLeft = lVRight;
			lRLeft = lRRight;
			lGLeft = lGRight;
			lBLeft = lBRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lUOverZRight += lUOverZStepAff;
			lVOverZRight += lVOverZStepAff;
			lROverZRight += lROverZStepAff;
			lGOverZRight += lGOverZStepAff;
			lBOverZRight += lBOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lURight = lZRight * (pRight->GetUOverZ() - pGradients->GetUOverZXStep());
			lVRight = lZRight * (pRight->GetVOverZ() - pGradients->GetVOverZXStep());
			lRRight = lZRight * (pRight->GetROverZ() - pGradients->GetROverZXStep()) * 255.0f;
			lGRight = lZRight * (pRight->GetGOverZ() - pGradients->GetGOverZXStep()) * 255.0f;
			lBRight = lZRight * (pRight->GetBOverZ() - pGradients->GetBOverZXStep()) * 255.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 255.0f);

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();
			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) / lRestWidth;
				lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) / lRestWidth;
				lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) / lRestWidth;
				lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) / lRestWidth;
				lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				unsigned int lCol = lTextureBits[((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)];
				
				int lRed   = (lR * (((lCol >> 11) & 31) << 3)) >> 24;
				int lGreen = (lG * (((lCol >> 5) & 63) << 2)) >> 24;
				int lBlue  = (lB * (((lCol >> 0) & 31) << 3)) >> 24;

				*lDestBits = (Lepra::uint16)(((lRed >> 3) << 11) | ((lGreen >> 2) << 5) | (lBlue >> 3));

				lDestBits++;

				lU += lDeltaU;
				lV += lDeltaV;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}
		}
	}
}

void Software3DPainter::DrawScanLineUVRGB24BitFast(const GradientsUVRGB* pGradients, 
														   EdgeUVRGB* pLeft, 
														   EdgeUVRGB* pRight,
														   const Lepra::Canvas* pTexture,
														   Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned char* lDestBits = (unsigned char*)pThis->mScreen->GetBuffer() +
									 (pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart) * 3;
		unsigned char* lTextureBits = (unsigned char*)pTexture->GetBuffer();

		int lTexturePitch = pTexture->GetPitch();
		unsigned int lTWMask = pTexture->GetWidth() - 1;
		unsigned int lTHMask = pTexture->GetHeight() - 1;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lUOverZLeft = pLeft->GetUOverZ();
		double lVOverZLeft = pLeft->GetVOverZ();
		double lROverZLeft = pLeft->GetROverZ();
		double lGOverZLeft = pLeft->GetGOverZ();
		double lBOverZLeft = pLeft->GetBOverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lUOverZStepAff = pGradients->GetUOverZXStep() * lAffineLengthFloat;
		double lVOverZStepAff = pGradients->GetVOverZXStep() * lAffineLengthFloat;
		double lROverZStepAff = pGradients->GetROverZXStep() * lAffineLengthFloat;
		double lGOverZStepAff = pGradients->GetGOverZXStep() * lAffineLengthFloat;
		double lBOverZStepAff = pGradients->GetBOverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lUOverZRight = lUOverZLeft + lUOverZStepAff;
		double lVOverZRight = lVOverZLeft + lVOverZStepAff;
		double lROverZRight = lROverZLeft + lROverZStepAff;
		double lGOverZRight = lGOverZLeft + lGOverZStepAff;
		double lBOverZRight = lBOverZLeft + lBOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lULeft = lUOverZLeft * lZLeft;
		double lVLeft = lVOverZLeft * lZLeft;
		double lRLeft = lROverZLeft * lZLeft * 255.0f;
		double lGLeft = lGOverZLeft * lZLeft * 255.0f;
		double lBLeft = lBOverZLeft * lZLeft * 255.0f;

		MACRO_CHECK_RGB(lRLeft, lGLeft, lBLeft, 255.0f);

		double lZRight;
		double lURight;
		double lVRight;
		double lRRight;
		double lGRight;
		double lBRight;
		Lepra::FixedPointMath::Fixed16_16 lU;
		Lepra::FixedPointMath::Fixed16_16 lV;
		Lepra::FixedPointMath::Fixed16_16 lR;
		Lepra::FixedPointMath::Fixed16_16 lG;
		Lepra::FixedPointMath::Fixed16_16 lB;
		Lepra::FixedPointMath::Fixed16_16 lDeltaU = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaV = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaR = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaG = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaB = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lURight = lUOverZRight * lZRight;
			lVRight = lVOverZRight * lZRight;
			lRRight = lROverZRight * lZRight * 255.0f;
			lGRight = lGOverZRight * lZRight * 255.0f;
			lBRight = lBOverZRight * lZRight * 255.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 255.0f);

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();
			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) >> pThis->mAffineExp;
			lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) >> pThis->mAffineExp;
			lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) >> pThis->mAffineExp;
			lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) >> pThis->mAffineExp;
			lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				unsigned char* lT = &lTextureBits[(((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)) * 3];
				
				lDestBits[0] = (unsigned char)((lB * (unsigned int)lT[0]) >> 24);
				lDestBits[1] = (unsigned char)((lG * (unsigned int)lT[1]) >> 24);
				lDestBits[2] = (unsigned char)((lR * (unsigned int)lT[2]) >> 24);

				lDestBits += 3;

				lU += lDeltaU;
				lV += lDeltaV;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}

			lZLeft = lZRight;
			lULeft = lURight;
			lVLeft = lVRight;
			lRLeft = lRRight;
			lGLeft = lGRight;
			lBLeft = lBRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lUOverZRight += lUOverZStepAff;
			lVOverZRight += lVOverZStepAff;
			lROverZRight += lROverZStepAff;
			lGOverZRight += lGOverZStepAff;
			lBOverZRight += lBOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lURight = lZRight * (pRight->GetUOverZ() - pGradients->GetUOverZXStep());
			lVRight = lZRight * (pRight->GetVOverZ() - pGradients->GetVOverZXStep());
			lRRight = lZRight * (pRight->GetROverZ() - pGradients->GetROverZXStep()) * 255.0f;
			lGRight = lZRight * (pRight->GetGOverZ() - pGradients->GetGOverZXStep()) * 255.0f;
			lBRight = lZRight * (pRight->GetBOverZ() - pGradients->GetBOverZXStep()) * 255.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 255.0f);

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();
			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) / lRestWidth;
				lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) / lRestWidth;
				lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) / lRestWidth;
				lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) / lRestWidth;
				lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				unsigned char* lT = &lTextureBits[(((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)) * 3];
				
				lDestBits[0] = (unsigned char)((lB * (unsigned int)lT[0]) >> 24);
				lDestBits[1] = (unsigned char)((lG * (unsigned int)lT[1]) >> 24);
				lDestBits[2] = (unsigned char)((lR * (unsigned int)lT[2]) >> 24);

				lDestBits += 3;

				lU += lDeltaU;
				lV += lDeltaV;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}
		}
	}
}

void Software3DPainter::DrawScanLineUVRGB32BitFast(const GradientsUVRGB* pGradients, 
														   EdgeUVRGB* pLeft, 
														   EdgeUVRGB* pRight,
														   const Lepra::Canvas* pTexture,
														   Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned char* lDestBits = (unsigned char*)pThis->mScreen->GetBuffer() +
									 ((pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart) << 2);
		unsigned int* lTextureBits = (unsigned int*)pTexture->GetBuffer();

		int lTexturePitch = pTexture->GetPitch();
		unsigned int lTWMask = pTexture->GetWidth() - 1;
		unsigned int lTHMask = pTexture->GetHeight() - 1;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lUOverZLeft = pLeft->GetUOverZ();
		double lVOverZLeft = pLeft->GetVOverZ();
		double lROverZLeft = pLeft->GetROverZ();
		double lGOverZLeft = pLeft->GetGOverZ();
		double lBOverZLeft = pLeft->GetBOverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lUOverZStepAff = pGradients->GetUOverZXStep() * lAffineLengthFloat;
		double lVOverZStepAff = pGradients->GetVOverZXStep() * lAffineLengthFloat;
		double lROverZStepAff = pGradients->GetROverZXStep() * lAffineLengthFloat;
		double lGOverZStepAff = pGradients->GetGOverZXStep() * lAffineLengthFloat;
		double lBOverZStepAff = pGradients->GetBOverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lUOverZRight = lUOverZLeft + lUOverZStepAff;
		double lVOverZRight = lVOverZLeft + lVOverZStepAff;
		double lROverZRight = lROverZLeft + lROverZStepAff;
		double lGOverZRight = lGOverZLeft + lGOverZStepAff;
		double lBOverZRight = lBOverZLeft + lBOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lULeft = lUOverZLeft * lZLeft;
		double lVLeft = lVOverZLeft * lZLeft;
		double lRLeft = lROverZLeft * lZLeft * 255.0f;
		double lGLeft = lGOverZLeft * lZLeft * 255.0f;
		double lBLeft = lBOverZLeft * lZLeft * 255.0f;

		MACRO_CHECK_RGB(lRLeft, lGLeft, lBLeft, 255.0f);

		double lZRight;
		double lURight;
		double lVRight;
		double lRRight;
		double lGRight;
		double lBRight;
		Lepra::FixedPointMath::Fixed16_16 lU;
		Lepra::FixedPointMath::Fixed16_16 lV;
		Lepra::FixedPointMath::Fixed16_16 lR;
		Lepra::FixedPointMath::Fixed16_16 lG;
		Lepra::FixedPointMath::Fixed16_16 lB;
		Lepra::FixedPointMath::Fixed16_16 lDeltaU = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaV = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaR = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaG = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaB = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lURight = lUOverZRight * lZRight;
			lVRight = lVOverZRight * lZRight;
			lRRight = lROverZRight * lZRight * 255.0f;
			lGRight = lGOverZRight * lZRight * 255.0f;
			lBRight = lBOverZRight * lZRight * 255.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 255.0f);

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();
			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) >> pThis->mAffineExp;
			lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) >> pThis->mAffineExp;
			lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) >> pThis->mAffineExp;
			lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) >> pThis->mAffineExp;
			lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				unsigned char* lT = (unsigned char*)(&lTextureBits[((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)]);
				
				lDestBits[0] = (unsigned char)((lB * (unsigned int)lT[0]) >> 24);
				lDestBits[1] = (unsigned char)((lG * (unsigned int)lT[1]) >> 24);
				lDestBits[2] = (unsigned char)((lR * (unsigned int)lT[2]) >> 24);

				lDestBits += 4;

				lU += lDeltaU;
				lV += lDeltaV;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}

			lZLeft = lZRight;
			lULeft = lURight;
			lVLeft = lVRight;
			lRLeft = lRRight;
			lGLeft = lGRight;
			lBLeft = lBRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lUOverZRight += lUOverZStepAff;
			lVOverZRight += lVOverZStepAff;
			lROverZRight += lROverZStepAff;
			lGOverZRight += lGOverZStepAff;
			lBOverZRight += lBOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lURight = lZRight * (pRight->GetUOverZ() - pGradients->GetUOverZXStep());
			lVRight = lZRight * (pRight->GetVOverZ() - pGradients->GetVOverZXStep());
			lRRight = lZRight * (pRight->GetROverZ() - pGradients->GetROverZXStep()) * 255.0f;
			lGRight = lZRight * (pRight->GetGOverZ() - pGradients->GetGOverZXStep()) * 255.0f;
			lBRight = lZRight * (pRight->GetBOverZ() - pGradients->GetBOverZXStep()) * 255.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 255.0f);

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();
			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) / lRestWidth;
				lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) / lRestWidth;
				lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) / lRestWidth;
				lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) / lRestWidth;
				lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				unsigned char* lT = (unsigned char*)(&lTextureBits[((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)]);
				
				lDestBits[0] = (unsigned char)((lB * (unsigned int)lT[0]) >> 24);
				lDestBits[1] = (unsigned char)((lG * (unsigned int)lT[1]) >> 24);
				lDestBits[2] = (unsigned char)((lR * (unsigned int)lT[2]) >> 24);

				lDestBits += 4;

				lU += lDeltaU;
				lV += lDeltaV;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}
		}
	}
}





void Software3DPainter::DrawScanLineUVRGB8BitFastZT(const GradientsUVRGB* pGradients, 
														  EdgeUVRGB* pLeft, 
														  EdgeUVRGB* pRight,
														  const Lepra::Canvas* pTexture,
														  Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned char* lDestBits = (unsigned char*)pThis->mScreen->GetBuffer() +
									 pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart;
		unsigned char* lTextureBits = (unsigned char*)pTexture->GetBuffer();
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		int lTexturePitch = pTexture->GetPitch();
		unsigned int lTWMask = pTexture->GetWidth() - 1;
		unsigned int lTHMask = pTexture->GetHeight() - 1;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lUOverZLeft = pLeft->GetUOverZ();
		double lVOverZLeft = pLeft->GetVOverZ();
		double lLightOverZLeft = pLeft->GetROverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lUOverZStepAff = pGradients->GetUOverZXStep() * lAffineLengthFloat;
		double lVOverZStepAff = pGradients->GetVOverZXStep() * lAffineLengthFloat;
		double lLightOverZStepAff = pGradients->GetROverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lUOverZRight = lUOverZLeft + lUOverZStepAff;
		double lVOverZRight = lVOverZLeft + lVOverZStepAff;
		double lLightOverZRight = lLightOverZLeft + lLightOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lULeft = lUOverZLeft * lZLeft;
		double lVLeft = lVOverZLeft * lZLeft;
		double lLightLeft = lLightOverZLeft * lZLeft;

		double lZRight;
		double lURight;
		double lVRight;
		double lLightRight;
		Lepra::FixedPointMath::Fixed16_16 lU;
		Lepra::FixedPointMath::Fixed16_16 lV;
		Lepra::FixedPointMath::Fixed16_16 lL;
		Lepra::FixedPointMath::Fixed16_16 lDeltaU = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaV = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaL = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//

		float lOneOverZ = (float)pLeft->GetOneOverZ();
		unsigned char* l8BitLightBuffer = pThis->m8BitLightBuffer;

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lURight = lUOverZRight * lZRight;
			lVRight = lVOverZRight * lZRight;
			lLightRight = lLightOverZRight * lZRight;

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();
			lL = Lepra::FixedPointMath::FloatToFixed16_16(lLightLeft * 0.9999999f);

			lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) >> pThis->mAffineExp;
			lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) >> pThis->mAffineExp;
			lDeltaL = Lepra::FixedPointMath::FloatToFixed16_16(lLightRight - lLightLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				if (lOneOverZ > *lZBuffer)
				{
					*lDestBits = l8BitLightBuffer[(lL & 0xFF00) + lTextureBits[((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)]];
				}

				lDestBits++;
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lU += lDeltaU;
				lV += lDeltaV;
				lL += lDeltaL;
			}

			lZLeft = lZRight;
			lULeft = lURight;
			lVLeft = lVRight;
			lLightLeft = lLightRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lUOverZRight += lUOverZStepAff;
			lVOverZRight += lVOverZStepAff;
			lLightOverZRight += lLightOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lURight = lZRight * (pRight->GetUOverZ() - pGradients->GetUOverZXStep());
			lVRight = lZRight * (pRight->GetVOverZ() - pGradients->GetVOverZXStep());
			lLightRight = lZRight * (pRight->GetROverZ() - pGradients->GetROverZXStep());

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();
			lL = Lepra::FixedPointMath::FloatToFixed16_16(lLightLeft * 0.9999999f);

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) / lRestWidth;
				lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) / lRestWidth;
				lDeltaL = Lepra::FixedPointMath::FloatToFixed16_16(lLightRight - lLightLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				if (lOneOverZ > *lZBuffer)
				{
					*lDestBits = l8BitLightBuffer[(lL & 0xFF00) + lTextureBits[((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)]];
				}

				lDestBits++;
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lU += lDeltaU;
				lV += lDeltaV;
				lL += lDeltaL;
			}
		}
	}
}

void Software3DPainter::DrawScanLineUVRGB15BitFastZT(const GradientsUVRGB* pGradients, 
														   EdgeUVRGB* pLeft, 
														   EdgeUVRGB* pRight,
														   const Lepra::Canvas* pTexture,
														   Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned short* lDestBits = (unsigned short*)pThis->mScreen->GetBuffer() +
									 (pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart);
		unsigned short* lTextureBits = (unsigned short*)pTexture->GetBuffer();
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		int lTexturePitch = pTexture->GetPitch();
		unsigned int lTWMask = pTexture->GetWidth() - 1;
		unsigned int lTHMask = pTexture->GetHeight() - 1;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lUOverZLeft = pLeft->GetUOverZ();
		double lVOverZLeft = pLeft->GetVOverZ();
		double lROverZLeft = pLeft->GetROverZ();
		double lGOverZLeft = pLeft->GetGOverZ();
		double lBOverZLeft = pLeft->GetBOverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lUOverZStepAff = pGradients->GetUOverZXStep() * lAffineLengthFloat;
		double lVOverZStepAff = pGradients->GetVOverZXStep() * lAffineLengthFloat;
		double lROverZStepAff = pGradients->GetROverZXStep() * lAffineLengthFloat;
		double lGOverZStepAff = pGradients->GetGOverZXStep() * lAffineLengthFloat;
		double lBOverZStepAff = pGradients->GetBOverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lUOverZRight = lUOverZLeft + lUOverZStepAff;
		double lVOverZRight = lVOverZLeft + lVOverZStepAff;
		double lROverZRight = lROverZLeft + lROverZStepAff;
		double lGOverZRight = lGOverZLeft + lGOverZStepAff;
		double lBOverZRight = lBOverZLeft + lBOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lULeft = lUOverZLeft * lZLeft;
		double lVLeft = lVOverZLeft * lZLeft;
		double lRLeft = lROverZLeft * lZLeft * 255.0f;
		double lGLeft = lGOverZLeft * lZLeft * 255.0f;
		double lBLeft = lBOverZLeft * lZLeft * 255.0f;

		MACRO_CHECK_RGB(lRLeft, lGLeft, lBLeft, 255.0f);

		double lZRight;
		double lURight;
		double lVRight;
		double lRRight;
		double lGRight;
		double lBRight;
		Lepra::FixedPointMath::Fixed16_16 lU;
		Lepra::FixedPointMath::Fixed16_16 lV;
		Lepra::FixedPointMath::Fixed16_16 lR;
		Lepra::FixedPointMath::Fixed16_16 lG;
		Lepra::FixedPointMath::Fixed16_16 lB;
		Lepra::FixedPointMath::Fixed16_16 lDeltaU = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaV = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaR = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaG = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaB = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//
		float lOneOverZ = (float)pLeft->GetOneOverZ();

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lURight = lUOverZRight * lZRight;
			lVRight = lVOverZRight * lZRight;
			lRRight = lROverZRight * lZRight * 255.0f;
			lGRight = lGOverZRight * lZRight * 255.0f;
			lBRight = lBOverZRight * lZRight * 255.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 255.0f);

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();
			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) >> pThis->mAffineExp;
			lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) >> pThis->mAffineExp;
			lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) >> pThis->mAffineExp;
			lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) >> pThis->mAffineExp;
			lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				if (lOneOverZ > *lZBuffer)
				{
					unsigned int lCol = lTextureBits[((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)];
					
					int lRed   = (lR * (((lCol >> 10) & 31) << 3)) >> 24;
					int lGreen = (lG * (((lCol >> 5) & 31) << 3)) >> 24;
					int lBlue  = (lB * (((lCol >> 0) & 31) << 3)) >> 24;

					*lDestBits = (Lepra::uint16)(((lRed >> 3) << 10) | ((lGreen >> 3) << 5) | (lBlue >> 3));
				}

				lDestBits++;
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lU += lDeltaU;
				lV += lDeltaV;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}

			lZLeft = lZRight;
			lULeft = lURight;
			lVLeft = lVRight;
			lRLeft = lRRight;
			lGLeft = lGRight;
			lBLeft = lBRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lUOverZRight += lUOverZStepAff;
			lVOverZRight += lVOverZStepAff;
			lROverZRight += lROverZStepAff;
			lGOverZRight += lGOverZStepAff;
			lBOverZRight += lBOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lURight = lZRight * (pRight->GetUOverZ() - pGradients->GetUOverZXStep());
			lVRight = lZRight * (pRight->GetVOverZ() - pGradients->GetVOverZXStep());
			lRRight = lZRight * (pRight->GetROverZ() - pGradients->GetROverZXStep()) * 255.0f;
			lGRight = lZRight * (pRight->GetGOverZ() - pGradients->GetGOverZXStep()) * 255.0f;
			lBRight = lZRight * (pRight->GetBOverZ() - pGradients->GetBOverZXStep()) * 255.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 255.0f);

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();
			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) / lRestWidth;
				lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) / lRestWidth;
				lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) / lRestWidth;
				lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) / lRestWidth;
				lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				if (lOneOverZ > *lZBuffer)
				{
					unsigned int lCol = lTextureBits[((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)];
					
					int lRed   = (lR * (((lCol >> 10) & 31) << 3)) >> 24;
					int lGreen = (lG * (((lCol >> 5) & 31) << 3)) >> 24;
					int lBlue  = (lB * (((lCol >> 0) & 31) << 3)) >> 24;

					*lDestBits = (Lepra::uint16)(((lRed >> 3) << 10) | ((lGreen >> 3) << 5) | (lBlue >> 3));
				}

				lDestBits++;
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lU += lDeltaU;
				lV += lDeltaV;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}
		}
	}
}

void Software3DPainter::DrawScanLineUVRGB16BitFastZT(const GradientsUVRGB* pGradients, 
														   EdgeUVRGB* pLeft, 
														   EdgeUVRGB* pRight,
														   const Lepra::Canvas* pTexture,
														   Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned short* lDestBits = (unsigned short*)pThis->mScreen->GetBuffer() +
									 (pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart);
		unsigned short* lTextureBits = (unsigned short*)pTexture->GetBuffer();
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		int lTexturePitch = pTexture->GetPitch();
		unsigned int lTWMask = pTexture->GetWidth() - 1;
		unsigned int lTHMask = pTexture->GetHeight() - 1;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lUOverZLeft = pLeft->GetUOverZ();
		double lVOverZLeft = pLeft->GetVOverZ();
		double lROverZLeft = pLeft->GetROverZ();
		double lGOverZLeft = pLeft->GetGOverZ();
		double lBOverZLeft = pLeft->GetBOverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lUOverZStepAff = pGradients->GetUOverZXStep() * lAffineLengthFloat;
		double lVOverZStepAff = pGradients->GetVOverZXStep() * lAffineLengthFloat;
		double lROverZStepAff = pGradients->GetROverZXStep() * lAffineLengthFloat;
		double lGOverZStepAff = pGradients->GetGOverZXStep() * lAffineLengthFloat;
		double lBOverZStepAff = pGradients->GetBOverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lUOverZRight = lUOverZLeft + lUOverZStepAff;
		double lVOverZRight = lVOverZLeft + lVOverZStepAff;
		double lROverZRight = lROverZLeft + lROverZStepAff;
		double lGOverZRight = lGOverZLeft + lGOverZStepAff;
		double lBOverZRight = lBOverZLeft + lBOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lULeft = lUOverZLeft * lZLeft;
		double lVLeft = lVOverZLeft * lZLeft;
		double lRLeft = lROverZLeft * lZLeft * 255.0f;
		double lGLeft = lGOverZLeft * lZLeft * 255.0f;
		double lBLeft = lBOverZLeft * lZLeft * 255.0f;

		MACRO_CHECK_RGB(lRLeft, lGLeft, lBLeft, 255);

		double lZRight;
		double lURight;
		double lVRight;
		double lRRight;
		double lGRight;
		double lBRight;
		Lepra::FixedPointMath::Fixed16_16 lU;
		Lepra::FixedPointMath::Fixed16_16 lV;
		Lepra::FixedPointMath::Fixed16_16 lR;
		Lepra::FixedPointMath::Fixed16_16 lG;
		Lepra::FixedPointMath::Fixed16_16 lB;
		Lepra::FixedPointMath::Fixed16_16 lDeltaU = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaV = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaR = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaG = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaB = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//
		float lOneOverZ = (float)pLeft->GetOneOverZ();

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lURight = lUOverZRight * lZRight;
			lVRight = lVOverZRight * lZRight;
			lRRight = lROverZRight * lZRight * 255.0f;
			lGRight = lGOverZRight * lZRight * 255.0f;
			lBRight = lBOverZRight * lZRight * 255.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 255.0f);

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();
			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) >> pThis->mAffineExp;
			lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) >> pThis->mAffineExp;
			lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) >> pThis->mAffineExp;
			lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) >> pThis->mAffineExp;
			lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				if (lOneOverZ > *lZBuffer)
				{
					unsigned int lCol = lTextureBits[((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)];
					
					int lRed   = (lR * (((lCol >> 11) & 31) << 3)) >> 24;
					int lGreen = (lG * (((lCol >> 5) & 63) << 2)) >> 24;
					int lBlue  = (lB * (((lCol >> 0) & 31) << 3)) >> 24;

					*lDestBits = (Lepra::uint16)(((lRed >> 3) << 11) | ((lGreen >> 2) << 5) | (lBlue >> 3));
				}

				lDestBits++;
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lU += lDeltaU;
				lV += lDeltaV;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}

			lZLeft = lZRight;
			lULeft = lURight;
			lVLeft = lVRight;
			lRLeft = lRRight;
			lGLeft = lGRight;
			lBLeft = lBRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lUOverZRight += lUOverZStepAff;
			lVOverZRight += lVOverZStepAff;
			lROverZRight += lROverZStepAff;
			lGOverZRight += lGOverZStepAff;
			lBOverZRight += lBOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lURight = lZRight * (pRight->GetUOverZ() - pGradients->GetUOverZXStep());
			lVRight = lZRight * (pRight->GetVOverZ() - pGradients->GetVOverZXStep());
			lRRight = lZRight * (pRight->GetROverZ() - pGradients->GetROverZXStep()) * 255.0f;
			lGRight = lZRight * (pRight->GetGOverZ() - pGradients->GetGOverZXStep()) * 255.0f;
			lBRight = lZRight * (pRight->GetBOverZ() - pGradients->GetBOverZXStep()) * 255.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 255.0f);

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();
			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) / lRestWidth;
				lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) / lRestWidth;
				lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) / lRestWidth;
				lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) / lRestWidth;
				lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				if (lOneOverZ > *lZBuffer)
				{
					unsigned int lCol = lTextureBits[((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)];
					
					int lRed   = (lR * (((lCol >> 11) & 31) << 3)) >> 24;
					int lGreen = (lG * (((lCol >> 5) & 63) << 2)) >> 24;
					int lBlue  = (lB * (((lCol >> 0) & 31) << 3)) >> 24;

					*lDestBits = (Lepra::uint16)(((lRed >> 3) << 11) | ((lGreen >> 2) << 5) | (lBlue >> 3));
				}

				lDestBits++;
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lU += lDeltaU;
				lV += lDeltaV;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}
		}
	}
}

void Software3DPainter::DrawScanLineUVRGB24BitFastZT(const GradientsUVRGB* pGradients, 
														   EdgeUVRGB* pLeft, 
														   EdgeUVRGB* pRight,
														   const Lepra::Canvas* pTexture,
														   Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned char* lDestBits = (unsigned char*)pThis->mScreen->GetBuffer() +
									 (pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart) * 3;
		unsigned char* lTextureBits = (unsigned char*)pTexture->GetBuffer();
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		int lTexturePitch = pTexture->GetPitch();
		unsigned int lTWMask = pTexture->GetWidth() - 1;
		unsigned int lTHMask = pTexture->GetHeight() - 1;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lUOverZLeft = pLeft->GetUOverZ();
		double lVOverZLeft = pLeft->GetVOverZ();
		double lROverZLeft = pLeft->GetROverZ();
		double lGOverZLeft = pLeft->GetGOverZ();
		double lBOverZLeft = pLeft->GetBOverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lUOverZStepAff = pGradients->GetUOverZXStep() * lAffineLengthFloat;
		double lVOverZStepAff = pGradients->GetVOverZXStep() * lAffineLengthFloat;
		double lROverZStepAff = pGradients->GetROverZXStep() * lAffineLengthFloat;
		double lGOverZStepAff = pGradients->GetGOverZXStep() * lAffineLengthFloat;
		double lBOverZStepAff = pGradients->GetBOverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lUOverZRight = lUOverZLeft + lUOverZStepAff;
		double lVOverZRight = lVOverZLeft + lVOverZStepAff;
		double lROverZRight = lROverZLeft + lROverZStepAff;
		double lGOverZRight = lGOverZLeft + lGOverZStepAff;
		double lBOverZRight = lBOverZLeft + lBOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lULeft = lUOverZLeft * lZLeft;
		double lVLeft = lVOverZLeft * lZLeft;
		double lRLeft = lROverZLeft * lZLeft * 255.0f;
		double lGLeft = lGOverZLeft * lZLeft * 255.0f;
		double lBLeft = lBOverZLeft * lZLeft * 255.0f;

		MACRO_CHECK_RGB(lRLeft, lGLeft, lBLeft, 255.0f);

		double lZRight;
		double lURight;
		double lVRight;
		double lRRight;
		double lGRight;
		double lBRight;
		Lepra::FixedPointMath::Fixed16_16 lU;
		Lepra::FixedPointMath::Fixed16_16 lV;
		Lepra::FixedPointMath::Fixed16_16 lR;
		Lepra::FixedPointMath::Fixed16_16 lG;
		Lepra::FixedPointMath::Fixed16_16 lB;
		Lepra::FixedPointMath::Fixed16_16 lDeltaU = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaV = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaR = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaG = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaB = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//
		float lOneOverZ = (float)pLeft->GetOneOverZ();

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lURight = lUOverZRight * lZRight;
			lVRight = lVOverZRight * lZRight;
			lRRight = lROverZRight * lZRight * 255.0f;
			lGRight = lGOverZRight * lZRight * 255.0f;
			lBRight = lBOverZRight * lZRight * 255.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 255.0f);

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();
			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) >> pThis->mAffineExp;
			lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) >> pThis->mAffineExp;
			lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) >> pThis->mAffineExp;
			lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) >> pThis->mAffineExp;
			lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				if (lOneOverZ > *lZBuffer)
				{
					unsigned char* lT = &lTextureBits[(((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)) * 3];

					lDestBits[0] = (unsigned char)((lB * (unsigned int)lT[0]) >> 24);
					lDestBits[1] = (unsigned char)((lG * (unsigned int)lT[1]) >> 24);
					lDestBits[2] = (unsigned char)((lR * (unsigned int)lT[2]) >> 24);
				}

				lDestBits += 3;
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lU += lDeltaU;
				lV += lDeltaV;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}

			lZLeft = lZRight;
			lULeft = lURight;
			lVLeft = lVRight;
			lRLeft = lRRight;
			lGLeft = lGRight;
			lBLeft = lBRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lUOverZRight += lUOverZStepAff;
			lVOverZRight += lVOverZStepAff;
			lROverZRight += lROverZStepAff;
			lGOverZRight += lGOverZStepAff;
			lBOverZRight += lBOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lURight = lZRight * (pRight->GetUOverZ() - pGradients->GetUOverZXStep());
			lVRight = lZRight * (pRight->GetVOverZ() - pGradients->GetVOverZXStep());
			lRRight = lZRight * (pRight->GetROverZ() - pGradients->GetROverZXStep()) * 255.0f;
			lGRight = lZRight * (pRight->GetGOverZ() - pGradients->GetGOverZXStep()) * 255.0f;
			lBRight = lZRight * (pRight->GetBOverZ() - pGradients->GetBOverZXStep()) * 255.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 255.0f);

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();
			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) / lRestWidth;
				lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) / lRestWidth;
				lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) / lRestWidth;
				lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) / lRestWidth;
				lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				if (lOneOverZ > *lZBuffer)
				{
					unsigned char* lT = &lTextureBits[(((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)) * 3];

					lDestBits[0] = (unsigned char)((lB * (unsigned int)lT[0]) >> 24);
					lDestBits[1] = (unsigned char)((lG * (unsigned int)lT[1]) >> 24);
					lDestBits[2] = (unsigned char)((lR * (unsigned int)lT[2]) >> 24);
				}

				lDestBits += 3;
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lU += lDeltaU;
				lV += lDeltaV;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}
		}
	}
}

void Software3DPainter::DrawScanLineUVRGB32BitFastZT(const GradientsUVRGB* pGradients, 
														   EdgeUVRGB* pLeft, 
														   EdgeUVRGB* pRight,
														   const Lepra::Canvas* pTexture,
														   Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned char* lDestBits = (unsigned char*)pThis->mScreen->GetBuffer() +
									 ((pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart) << 2);
		unsigned int* lTextureBits = (unsigned int*)pTexture->GetBuffer();
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		int lTexturePitch = pTexture->GetPitch();
		unsigned int lTWMask = pTexture->GetWidth() - 1;
		unsigned int lTHMask = pTexture->GetHeight() - 1;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lUOverZLeft = pLeft->GetUOverZ();
		double lVOverZLeft = pLeft->GetVOverZ();
		double lROverZLeft = pLeft->GetROverZ();
		double lGOverZLeft = pLeft->GetGOverZ();
		double lBOverZLeft = pLeft->GetBOverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lUOverZStepAff = pGradients->GetUOverZXStep() * lAffineLengthFloat;
		double lVOverZStepAff = pGradients->GetVOverZXStep() * lAffineLengthFloat;
		double lROverZStepAff = pGradients->GetROverZXStep() * lAffineLengthFloat;
		double lGOverZStepAff = pGradients->GetGOverZXStep() * lAffineLengthFloat;
		double lBOverZStepAff = pGradients->GetBOverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lUOverZRight = lUOverZLeft + lUOverZStepAff;
		double lVOverZRight = lVOverZLeft + lVOverZStepAff;
		double lROverZRight = lROverZLeft + lROverZStepAff;
		double lGOverZRight = lGOverZLeft + lGOverZStepAff;
		double lBOverZRight = lBOverZLeft + lBOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lULeft = lUOverZLeft * lZLeft;
		double lVLeft = lVOverZLeft * lZLeft;
		double lRLeft = lROverZLeft * lZLeft * 255.0f;
		double lGLeft = lGOverZLeft * lZLeft * 255.0f;
		double lBLeft = lBOverZLeft * lZLeft * 255.0f;

		MACRO_CHECK_RGB(lRLeft, lGLeft, lBLeft, 255.0f);

		double lZRight;
		double lURight;
		double lVRight;
		double lRRight;
		double lGRight;
		double lBRight;
		Lepra::FixedPointMath::Fixed16_16 lU;
		Lepra::FixedPointMath::Fixed16_16 lV;
		Lepra::FixedPointMath::Fixed16_16 lR;
		Lepra::FixedPointMath::Fixed16_16 lG;
		Lepra::FixedPointMath::Fixed16_16 lB;
		Lepra::FixedPointMath::Fixed16_16 lDeltaU = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaV = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaR = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaG = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaB = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//
		float lOneOverZ = (float)pLeft->GetOneOverZ();

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lURight = lUOverZRight * lZRight;
			lVRight = lVOverZRight * lZRight;
			lRRight = lROverZRight * lZRight * 255.0f;
			lGRight = lGOverZRight * lZRight * 255.0f;
			lBRight = lBOverZRight * lZRight * 255.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 255.0f);

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();
			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) >> pThis->mAffineExp;
			lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) >> pThis->mAffineExp;
			lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) >> pThis->mAffineExp;
			lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) >> pThis->mAffineExp;
			lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				if (lOneOverZ > *lZBuffer)
				{
					unsigned char* lT = (unsigned char*)&lTextureBits[(((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask))];

					lDestBits[0] = (unsigned char)((lB * (unsigned int)lT[0]) >> 24);
					lDestBits[1] = (unsigned char)((lG * (unsigned int)lT[1]) >> 24);
					lDestBits[2] = (unsigned char)((lR * (unsigned int)lT[2]) >> 24);
				}

				lDestBits += 4;
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lU += lDeltaU;
				lV += lDeltaV;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}

			lZLeft = lZRight;
			lULeft = lURight;
			lVLeft = lVRight;
			lRLeft = lRRight;
			lGLeft = lGRight;
			lBLeft = lBRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lUOverZRight += lUOverZStepAff;
			lVOverZRight += lVOverZStepAff;
			lROverZRight += lROverZStepAff;
			lGOverZRight += lGOverZStepAff;
			lBOverZRight += lBOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lURight = lZRight * (pRight->GetUOverZ() - pGradients->GetUOverZXStep());
			lVRight = lZRight * (pRight->GetVOverZ() - pGradients->GetVOverZXStep());
			lRRight = lZRight * (pRight->GetROverZ() - pGradients->GetROverZXStep()) * 255.0f;
			lGRight = lZRight * (pRight->GetGOverZ() - pGradients->GetGOverZXStep()) * 255.0f;
			lBRight = lZRight * (pRight->GetBOverZ() - pGradients->GetBOverZXStep()) * 255.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 255.0f);

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();
			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) / lRestWidth;
				lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) / lRestWidth;
				lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) / lRestWidth;
				lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) / lRestWidth;
				lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				if (lOneOverZ > *lZBuffer)
				{
					unsigned char* lT = (unsigned char*)&lTextureBits[(((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask))];

					lDestBits[0] = (unsigned char)((lB * (unsigned int)lT[0]) >> 24);
					lDestBits[1] = (unsigned char)((lG * (unsigned int)lT[1]) >> 24);
					lDestBits[2] = (unsigned char)((lR * (unsigned int)lT[2]) >> 24);
				}

				lDestBits += 4;
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lU += lDeltaU;
				lV += lDeltaV;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}
		}
	}
}










void Software3DPainter::DrawScanLineUVRGB8BitFastZW(const GradientsUVRGB* pGradients, 
														  EdgeUVRGB* pLeft, 
														  EdgeUVRGB* pRight,
														  const Lepra::Canvas* pTexture,
														  Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned char* lDestBits = (unsigned char*)pThis->mScreen->GetBuffer() +
									 pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart;
		unsigned char* lTextureBits = (unsigned char*)pTexture->GetBuffer();
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		int lTexturePitch = pTexture->GetPitch();
		unsigned int lTWMask = pTexture->GetWidth() - 1;
		unsigned int lTHMask = pTexture->GetHeight() - 1;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lUOverZLeft = pLeft->GetUOverZ();
		double lVOverZLeft = pLeft->GetVOverZ();
		double lLightOverZLeft = pLeft->GetROverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lUOverZStepAff = pGradients->GetUOverZXStep() * lAffineLengthFloat;
		double lVOverZStepAff = pGradients->GetVOverZXStep() * lAffineLengthFloat;
		double lLightOverZStepAff = pGradients->GetROverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lUOverZRight = lUOverZLeft + lUOverZStepAff;
		double lVOverZRight = lVOverZLeft + lVOverZStepAff;
		double lLightOverZRight = lLightOverZLeft + lLightOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lULeft = lUOverZLeft * lZLeft;
		double lVLeft = lVOverZLeft * lZLeft;
		double lLightLeft = lLightOverZLeft * lZLeft;

		double lZRight;
		double lURight;
		double lVRight;
		double lLightRight;
		Lepra::FixedPointMath::Fixed16_16 lU;
		Lepra::FixedPointMath::Fixed16_16 lV;
		Lepra::FixedPointMath::Fixed16_16 lL;
		Lepra::FixedPointMath::Fixed16_16 lDeltaU = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaV = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaL = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//

		float lOneOverZ = (float)pLeft->GetOneOverZ();
		unsigned char* l8BitLightBuffer = pThis->m8BitLightBuffer;

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lURight = lUOverZRight * lZRight;
			lVRight = lVOverZRight * lZRight;
			lLightRight = lLightOverZRight * lZRight;

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();
			lL = Lepra::FixedPointMath::FloatToFixed16_16(lLightLeft * 0.9999999f);

			lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) >> pThis->mAffineExp;
			lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) >> pThis->mAffineExp;
			lDeltaL = Lepra::FixedPointMath::FloatToFixed16_16(lLightRight - lLightLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				*lDestBits = l8BitLightBuffer[(lL & 0xFF00) + lTextureBits[((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)]];
				*lZBuffer = lOneOverZ;

				lDestBits++;
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lU += lDeltaU;
				lV += lDeltaV;
				lL += lDeltaL;
			}

			lZLeft = lZRight;
			lULeft = lURight;
			lVLeft = lVRight;
			lLightLeft = lLightRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lUOverZRight += lUOverZStepAff;
			lVOverZRight += lVOverZStepAff;
			lLightOverZRight += lLightOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lURight = lZRight * (pRight->GetUOverZ() - pGradients->GetUOverZXStep());
			lVRight = lZRight * (pRight->GetVOverZ() - pGradients->GetVOverZXStep());
			lLightRight = lZRight * (pRight->GetROverZ() - pGradients->GetROverZXStep());

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();
			lL = Lepra::FixedPointMath::FloatToFixed16_16(lLightLeft * 0.9999999f);

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) / lRestWidth;
				lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) / lRestWidth;
				lDeltaL = Lepra::FixedPointMath::FloatToFixed16_16(lLightRight - lLightLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				*lDestBits = l8BitLightBuffer[(lL & 0xFF00) + lTextureBits[((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)]];
				*lZBuffer = lOneOverZ;

				lDestBits++;
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lU += lDeltaU;
				lV += lDeltaV;
				lL += lDeltaL;
			}
		}
	}
}

void Software3DPainter::DrawScanLineUVRGB15BitFastZW(const GradientsUVRGB* pGradients, 
														   EdgeUVRGB* pLeft, 
														   EdgeUVRGB* pRight,
														   const Lepra::Canvas* pTexture,
														   Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned short* lDestBits = (unsigned short*)pThis->mScreen->GetBuffer() +
									 (pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart);
		unsigned short* lTextureBits = (unsigned short*)pTexture->GetBuffer();
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		int lTexturePitch = pTexture->GetPitch();
		unsigned int lTWMask = pTexture->GetWidth() - 1;
		unsigned int lTHMask = pTexture->GetHeight() - 1;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lUOverZLeft = pLeft->GetUOverZ();
		double lVOverZLeft = pLeft->GetVOverZ();
		double lROverZLeft = pLeft->GetROverZ();
		double lGOverZLeft = pLeft->GetGOverZ();
		double lBOverZLeft = pLeft->GetBOverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lUOverZStepAff = pGradients->GetUOverZXStep() * lAffineLengthFloat;
		double lVOverZStepAff = pGradients->GetVOverZXStep() * lAffineLengthFloat;
		double lROverZStepAff = pGradients->GetROverZXStep() * lAffineLengthFloat;
		double lGOverZStepAff = pGradients->GetGOverZXStep() * lAffineLengthFloat;
		double lBOverZStepAff = pGradients->GetBOverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lUOverZRight = lUOverZLeft + lUOverZStepAff;
		double lVOverZRight = lVOverZLeft + lVOverZStepAff;
		double lROverZRight = lROverZLeft + lROverZStepAff;
		double lGOverZRight = lGOverZLeft + lGOverZStepAff;
		double lBOverZRight = lBOverZLeft + lBOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lULeft = lUOverZLeft * lZLeft;
		double lVLeft = lVOverZLeft * lZLeft;
		double lRLeft = lROverZLeft * lZLeft * 255.0f;
		double lGLeft = lGOverZLeft * lZLeft * 255.0f;
		double lBLeft = lBOverZLeft * lZLeft * 255.0f;

		MACRO_CHECK_RGB(lRLeft, lGLeft, lBLeft, 255.0f);

		double lZRight;
		double lURight;
		double lVRight;
		double lRRight;
		double lGRight;
		double lBRight;
		Lepra::FixedPointMath::Fixed16_16 lU;
		Lepra::FixedPointMath::Fixed16_16 lV;
		Lepra::FixedPointMath::Fixed16_16 lR;
		Lepra::FixedPointMath::Fixed16_16 lG;
		Lepra::FixedPointMath::Fixed16_16 lB;
		Lepra::FixedPointMath::Fixed16_16 lDeltaU = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaV = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaR = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaG = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaB = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//
		float lOneOverZ = (float)pLeft->GetOneOverZ();

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lURight = lUOverZRight * lZRight;
			lVRight = lVOverZRight * lZRight;
			lRRight = lROverZRight * lZRight * 255.0f;
			lGRight = lGOverZRight * lZRight * 255.0f;
			lBRight = lBOverZRight * lZRight * 255.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 255.0f);

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();
			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) >> pThis->mAffineExp;
			lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) >> pThis->mAffineExp;
			lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) >> pThis->mAffineExp;
			lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) >> pThis->mAffineExp;
			lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				unsigned int lCol = lTextureBits[((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)];
				
				int lRed   = (lR * (((lCol >> 10) & 31) << 3)) >> 24;
				int lGreen = (lG * (((lCol >> 5) & 31) << 3)) >> 24;
				int lBlue  = (lB * (((lCol >> 0) & 31) << 3)) >> 24;

				*lDestBits = (Lepra::uint16)(((lRed >> 3) << 10) | ((lGreen >> 3) << 5) | (lBlue >> 3));
				*lZBuffer = lOneOverZ;

				lDestBits++;
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lU += lDeltaU;
				lV += lDeltaV;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}

			lZLeft = lZRight;
			lULeft = lURight;
			lVLeft = lVRight;
			lRLeft = lRRight;
			lGLeft = lGRight;
			lBLeft = lBRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lUOverZRight += lUOverZStepAff;
			lVOverZRight += lVOverZStepAff;
			lROverZRight += lROverZStepAff;
			lGOverZRight += lGOverZStepAff;
			lBOverZRight += lBOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lURight = lZRight * (pRight->GetUOverZ() - pGradients->GetUOverZXStep());
			lVRight = lZRight * (pRight->GetVOverZ() - pGradients->GetVOverZXStep());
			lRRight = lZRight * (pRight->GetROverZ() - pGradients->GetROverZXStep()) * 255.0f;
			lGRight = lZRight * (pRight->GetGOverZ() - pGradients->GetGOverZXStep()) * 255.0f;
			lBRight = lZRight * (pRight->GetBOverZ() - pGradients->GetBOverZXStep()) * 255.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 255.0f);

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();
			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) / lRestWidth;
				lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) / lRestWidth;
				lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) / lRestWidth;
				lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) / lRestWidth;
				lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				unsigned int lCol = lTextureBits[((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)];
				
				int lRed   = (lR * (((lCol >> 10) & 31) << 3)) >> 24;
				int lGreen = (lG * (((lCol >> 5) & 31) << 3)) >> 24;
				int lBlue  = (lB * (((lCol >> 0) & 31) << 3)) >> 24;

				*lDestBits = (Lepra::uint16)(((lRed >> 3) << 10) | ((lGreen >> 3) << 5) | (lBlue >> 3));
				*lZBuffer = lOneOverZ;

				lDestBits++;
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lU += lDeltaU;
				lV += lDeltaV;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}
		}
	}
}

void Software3DPainter::DrawScanLineUVRGB16BitFastZW(const GradientsUVRGB* pGradients, 
														   EdgeUVRGB* pLeft, 
														   EdgeUVRGB* pRight,
														   const Lepra::Canvas* pTexture,
														   Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned short* lDestBits = (unsigned short*)pThis->mScreen->GetBuffer() +
									 (pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart);
		unsigned short* lTextureBits = (unsigned short*)pTexture->GetBuffer();
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		int lTexturePitch = pTexture->GetPitch();
		unsigned int lTWMask = pTexture->GetWidth() - 1;
		unsigned int lTHMask = pTexture->GetHeight() - 1;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lUOverZLeft = pLeft->GetUOverZ();
		double lVOverZLeft = pLeft->GetVOverZ();
		double lROverZLeft = pLeft->GetROverZ();
		double lGOverZLeft = pLeft->GetGOverZ();
		double lBOverZLeft = pLeft->GetBOverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lUOverZStepAff = pGradients->GetUOverZXStep() * lAffineLengthFloat;
		double lVOverZStepAff = pGradients->GetVOverZXStep() * lAffineLengthFloat;
		double lROverZStepAff = pGradients->GetROverZXStep() * lAffineLengthFloat;
		double lGOverZStepAff = pGradients->GetGOverZXStep() * lAffineLengthFloat;
		double lBOverZStepAff = pGradients->GetBOverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lUOverZRight = lUOverZLeft + lUOverZStepAff;
		double lVOverZRight = lVOverZLeft + lVOverZStepAff;
		double lROverZRight = lROverZLeft + lROverZStepAff;
		double lGOverZRight = lGOverZLeft + lGOverZStepAff;
		double lBOverZRight = lBOverZLeft + lBOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lULeft = lUOverZLeft * lZLeft;
		double lVLeft = lVOverZLeft * lZLeft;
		double lRLeft = lROverZLeft * lZLeft * 255.0f;
		double lGLeft = lGOverZLeft * lZLeft * 255.0f;
		double lBLeft = lBOverZLeft * lZLeft * 255.0f;

		MACRO_CHECK_RGB(lRLeft, lGLeft, lBLeft, 255.0f);

		double lZRight;
		double lURight;
		double lVRight;
		double lRRight;
		double lGRight;
		double lBRight;
		Lepra::FixedPointMath::Fixed16_16 lU;
		Lepra::FixedPointMath::Fixed16_16 lV;
		Lepra::FixedPointMath::Fixed16_16 lR;
		Lepra::FixedPointMath::Fixed16_16 lG;
		Lepra::FixedPointMath::Fixed16_16 lB;
		Lepra::FixedPointMath::Fixed16_16 lDeltaU = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaV = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaR = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaG = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaB = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//
		float lOneOverZ = (float)pLeft->GetOneOverZ();

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lURight = lUOverZRight * lZRight;
			lVRight = lVOverZRight * lZRight;
			lRRight = lROverZRight * lZRight * 255.0f;
			lGRight = lGOverZRight * lZRight * 255.0f;
			lBRight = lBOverZRight * lZRight * 255.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 255.0f);

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();
			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) >> pThis->mAffineExp;
			lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) >> pThis->mAffineExp;
			lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) >> pThis->mAffineExp;
			lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) >> pThis->mAffineExp;
			lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				unsigned int lCol = lTextureBits[((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)];
				
				int lRed   = (lR * (((lCol >> 11) & 31) << 3)) >> 24;
				int lGreen = (lG * (((lCol >> 5) & 63) << 2)) >> 24;
				int lBlue  = (lB * (((lCol >> 0) & 31) << 3)) >> 24;

				*lDestBits = (Lepra::uint16)(((lRed >> 3) << 11) | ((lGreen >> 2) << 5) | (lBlue >> 3));
				*lZBuffer = lOneOverZ;

				lDestBits++;
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lU += lDeltaU;
				lV += lDeltaV;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}

			lZLeft = lZRight;
			lULeft = lURight;
			lVLeft = lVRight;
			lRLeft = lRRight;
			lGLeft = lGRight;
			lBLeft = lBRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lUOverZRight += lUOverZStepAff;
			lVOverZRight += lVOverZStepAff;
			lROverZRight += lROverZStepAff;
			lGOverZRight += lGOverZStepAff;
			lBOverZRight += lBOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lURight = lZRight * (pRight->GetUOverZ() - pGradients->GetUOverZXStep());
			lVRight = lZRight * (pRight->GetVOverZ() - pGradients->GetVOverZXStep());
			lRRight = lZRight * (pRight->GetROverZ() - pGradients->GetROverZXStep()) * 255.0f;
			lGRight = lZRight * (pRight->GetGOverZ() - pGradients->GetGOverZXStep()) * 255.0f;
			lBRight = lZRight * (pRight->GetBOverZ() - pGradients->GetBOverZXStep()) * 255.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 255.0f);

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();
			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) / lRestWidth;
				lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) / lRestWidth;
				lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) / lRestWidth;
				lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) / lRestWidth;
				lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				unsigned int lCol = lTextureBits[((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)];
				
				int lRed   = (lR * (((lCol >> 11) & 31) << 3)) >> 24;
				int lGreen = (lG * (((lCol >> 5) & 63) << 2)) >> 24;
				int lBlue  = (lB * (((lCol >> 0) & 31) << 3)) >> 24;

				*lDestBits = (Lepra::uint16)(((lRed >> 3) << 11) | ((lGreen >> 2) << 5) | (lBlue >> 3));
				*lZBuffer = lOneOverZ;

				lDestBits++;
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lU += lDeltaU;
				lV += lDeltaV;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}
		}
	}
}

void Software3DPainter::DrawScanLineUVRGB24BitFastZW(const GradientsUVRGB* pGradients, 
														   EdgeUVRGB* pLeft, 
														   EdgeUVRGB* pRight,
														   const Lepra::Canvas* pTexture,
														   Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned char* lDestBits = (unsigned char*)pThis->mScreen->GetBuffer() +
									 (pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart) * 3;
		unsigned char* lTextureBits = (unsigned char*)pTexture->GetBuffer();
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		int lTexturePitch = pTexture->GetPitch();
		unsigned int lTWMask = pTexture->GetWidth() - 1;
		unsigned int lTHMask = pTexture->GetHeight() - 1;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lUOverZLeft = pLeft->GetUOverZ();
		double lVOverZLeft = pLeft->GetVOverZ();
		double lROverZLeft = pLeft->GetROverZ();
		double lGOverZLeft = pLeft->GetGOverZ();
		double lBOverZLeft = pLeft->GetBOverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lUOverZStepAff = pGradients->GetUOverZXStep() * lAffineLengthFloat;
		double lVOverZStepAff = pGradients->GetVOverZXStep() * lAffineLengthFloat;
		double lROverZStepAff = pGradients->GetROverZXStep() * lAffineLengthFloat;
		double lGOverZStepAff = pGradients->GetGOverZXStep() * lAffineLengthFloat;
		double lBOverZStepAff = pGradients->GetBOverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lUOverZRight = lUOverZLeft + lUOverZStepAff;
		double lVOverZRight = lVOverZLeft + lVOverZStepAff;
		double lROverZRight = lROverZLeft + lROverZStepAff;
		double lGOverZRight = lGOverZLeft + lGOverZStepAff;
		double lBOverZRight = lBOverZLeft + lBOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lULeft = lUOverZLeft * lZLeft;
		double lVLeft = lVOverZLeft * lZLeft;
		double lRLeft = lROverZLeft * lZLeft * 255.0f;
		double lGLeft = lGOverZLeft * lZLeft * 255.0f;
		double lBLeft = lBOverZLeft * lZLeft * 255.0f;

		MACRO_CHECK_RGB(lRLeft, lGLeft, lBLeft, 255.0f);

		double lZRight;
		double lURight;
		double lVRight;
		double lRRight;
		double lGRight;
		double lBRight;
		Lepra::FixedPointMath::Fixed16_16 lU;
		Lepra::FixedPointMath::Fixed16_16 lV;
		Lepra::FixedPointMath::Fixed16_16 lR;
		Lepra::FixedPointMath::Fixed16_16 lG;
		Lepra::FixedPointMath::Fixed16_16 lB;
		Lepra::FixedPointMath::Fixed16_16 lDeltaU = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaV = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaR = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaG = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaB = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//
		float lOneOverZ = (float)pLeft->GetOneOverZ();

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lURight = lUOverZRight * lZRight;
			lVRight = lVOverZRight * lZRight;
			lRRight = lROverZRight * lZRight * 255.0f;
			lGRight = lGOverZRight * lZRight * 255.0f;
			lBRight = lBOverZRight * lZRight * 255.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 255.0f);

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();
			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) >> pThis->mAffineExp;
			lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) >> pThis->mAffineExp;
			lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) >> pThis->mAffineExp;
			lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) >> pThis->mAffineExp;
			lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				unsigned char* lT = &lTextureBits[(((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)) * 3];
				
				lDestBits[0] = (unsigned char)((lB * (unsigned int)lT[0]) >> 24);
				lDestBits[1] = (unsigned char)((lG * (unsigned int)lT[1]) >> 24);
				lDestBits[2] = (unsigned char)((lR * (unsigned int)lT[2]) >> 24);

				*lZBuffer = lOneOverZ;

				lDestBits += 3;
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lU += lDeltaU;
				lV += lDeltaV;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}

			lZLeft = lZRight;
			lULeft = lURight;
			lVLeft = lVRight;
			lRLeft = lRRight;
			lGLeft = lGRight;
			lBLeft = lBRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lUOverZRight += lUOverZStepAff;
			lVOverZRight += lVOverZStepAff;
			lROverZRight += lROverZStepAff;
			lGOverZRight += lGOverZStepAff;
			lBOverZRight += lBOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lURight = lZRight * (pRight->GetUOverZ() - pGradients->GetUOverZXStep());
			lVRight = lZRight * (pRight->GetVOverZ() - pGradients->GetVOverZXStep());
			lRRight = lZRight * (pRight->GetROverZ() - pGradients->GetROverZXStep()) * 255.0f;
			lGRight = lZRight * (pRight->GetGOverZ() - pGradients->GetGOverZXStep()) * 255.0f;
			lBRight = lZRight * (pRight->GetBOverZ() - pGradients->GetBOverZXStep()) * 255.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 255.0f);

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();
			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) / lRestWidth;
				lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) / lRestWidth;
				lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) / lRestWidth;
				lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) / lRestWidth;
				lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				unsigned char* lT = &lTextureBits[(((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)) * 3];
				
				lDestBits[0] = (unsigned char)((lB * (unsigned int)lT[0]) >> 24);
				lDestBits[1] = (unsigned char)((lG * (unsigned int)lT[1]) >> 24);
				lDestBits[2] = (unsigned char)((lR * (unsigned int)lT[2]) >> 24);

				*lZBuffer = lOneOverZ;

				lDestBits += 3;
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lU += lDeltaU;
				lV += lDeltaV;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}
		}
	}
}

void Software3DPainter::DrawScanLineUVRGB32BitFastZW(const GradientsUVRGB* pGradients, 
														   EdgeUVRGB* pLeft, 
														   EdgeUVRGB* pRight,
														   const Lepra::Canvas* pTexture,
														   Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned char* lDestBits = (unsigned char*)pThis->mScreen->GetBuffer() +
									 ((pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart) << 2);
		unsigned int* lTextureBits = (unsigned int*)pTexture->GetBuffer();
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		int lTexturePitch = pTexture->GetPitch();
		unsigned int lTWMask = pTexture->GetWidth() - 1;
		unsigned int lTHMask = pTexture->GetHeight() - 1;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lUOverZLeft = pLeft->GetUOverZ();
		double lVOverZLeft = pLeft->GetVOverZ();
		double lROverZLeft = pLeft->GetROverZ();
		double lGOverZLeft = pLeft->GetGOverZ();
		double lBOverZLeft = pLeft->GetBOverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lUOverZStepAff = pGradients->GetUOverZXStep() * lAffineLengthFloat;
		double lVOverZStepAff = pGradients->GetVOverZXStep() * lAffineLengthFloat;
		double lROverZStepAff = pGradients->GetROverZXStep() * lAffineLengthFloat;
		double lGOverZStepAff = pGradients->GetGOverZXStep() * lAffineLengthFloat;
		double lBOverZStepAff = pGradients->GetBOverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lUOverZRight = lUOverZLeft + lUOverZStepAff;
		double lVOverZRight = lVOverZLeft + lVOverZStepAff;
		double lROverZRight = lROverZLeft + lROverZStepAff;
		double lGOverZRight = lGOverZLeft + lGOverZStepAff;
		double lBOverZRight = lBOverZLeft + lBOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lULeft = lUOverZLeft * lZLeft;
		double lVLeft = lVOverZLeft * lZLeft;
		double lRLeft = lROverZLeft * lZLeft * 255.0f;
		double lGLeft = lGOverZLeft * lZLeft * 255.0f;
		double lBLeft = lBOverZLeft * lZLeft * 255.0f;

		MACRO_CHECK_RGB(lRLeft, lGLeft, lBLeft, 255.0f);

		double lZRight;
		double lURight;
		double lVRight;
		double lRRight;
		double lGRight;
		double lBRight;
		Lepra::FixedPointMath::Fixed16_16 lU;
		Lepra::FixedPointMath::Fixed16_16 lV;
		Lepra::FixedPointMath::Fixed16_16 lR;
		Lepra::FixedPointMath::Fixed16_16 lG;
		Lepra::FixedPointMath::Fixed16_16 lB;
		Lepra::FixedPointMath::Fixed16_16 lDeltaU = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaV = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaR = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaG = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaB = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//
		float lOneOverZ = (float)pLeft->GetOneOverZ();

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lURight = lUOverZRight * lZRight;
			lVRight = lVOverZRight * lZRight;
			lRRight = lROverZRight * lZRight * 255.0f;
			lGRight = lGOverZRight * lZRight * 255.0f;
			lBRight = lBOverZRight * lZRight * 255.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 255.0f);

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();
			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) >> pThis->mAffineExp;
			lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) >> pThis->mAffineExp;
			lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) >> pThis->mAffineExp;
			lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) >> pThis->mAffineExp;
			lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				unsigned char* lT = (unsigned char*)&lTextureBits[(((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask))];
				
				lDestBits[0] = (unsigned char)((lB * (unsigned int)lT[0]) >> 24);
				lDestBits[1] = (unsigned char)((lG * (unsigned int)lT[1]) >> 24);
				lDestBits[2] = (unsigned char)((lR * (unsigned int)lT[2]) >> 24);

				*lZBuffer = lOneOverZ;

				lDestBits += 4;
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lU += lDeltaU;
				lV += lDeltaV;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}

			lZLeft = lZRight;
			lULeft = lURight;
			lVLeft = lVRight;
			lRLeft = lRRight;
			lGLeft = lGRight;
			lBLeft = lBRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lUOverZRight += lUOverZStepAff;
			lVOverZRight += lVOverZStepAff;
			lROverZRight += lROverZStepAff;
			lGOverZRight += lGOverZStepAff;
			lBOverZRight += lBOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lURight = lZRight * (pRight->GetUOverZ() - pGradients->GetUOverZXStep());
			lVRight = lZRight * (pRight->GetVOverZ() - pGradients->GetVOverZXStep());
			lRRight = lZRight * (pRight->GetROverZ() - pGradients->GetROverZXStep()) * 255.0f;
			lGRight = lZRight * (pRight->GetGOverZ() - pGradients->GetGOverZXStep()) * 255.0f;
			lBRight = lZRight * (pRight->GetBOverZ() - pGradients->GetBOverZXStep()) * 255.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 255.0f);

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();
			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) / lRestWidth;
				lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) / lRestWidth;
				lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) / lRestWidth;
				lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) / lRestWidth;
				lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				unsigned char* lT = (unsigned char*)&lTextureBits[(((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask))];
				
				lDestBits[0] = (unsigned char)((lB * (unsigned int)lT[0]) >> 24);
				lDestBits[1] = (unsigned char)((lG * (unsigned int)lT[1]) >> 24);
				lDestBits[2] = (unsigned char)((lR * (unsigned int)lT[2]) >> 24);

				*lZBuffer = lOneOverZ;

				lDestBits += 4;
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lU += lDeltaU;
				lV += lDeltaV;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}
		}
	}
}




void Software3DPainter::DrawScanLineUVRGB8BitFastZTZW(const GradientsUVRGB* pGradients, 
														  EdgeUVRGB* pLeft, 
														  EdgeUVRGB* pRight,
														  const Lepra::Canvas* pTexture,
														  Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned char* lDestBits = (unsigned char*)pThis->mScreen->GetBuffer() +
									 pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart;
		unsigned char* lTextureBits = (unsigned char*)pTexture->GetBuffer();
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		int lTexturePitch = pTexture->GetPitch();
		unsigned int lTWMask = pTexture->GetWidth() - 1;
		unsigned int lTHMask = pTexture->GetHeight() - 1;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lUOverZLeft = pLeft->GetUOverZ();
		double lVOverZLeft = pLeft->GetVOverZ();
		double lLightOverZLeft = pLeft->GetROverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lUOverZStepAff = pGradients->GetUOverZXStep() * lAffineLengthFloat;
		double lVOverZStepAff = pGradients->GetVOverZXStep() * lAffineLengthFloat;
		double lLightOverZStepAff = pGradients->GetROverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lUOverZRight = lUOverZLeft + lUOverZStepAff;
		double lVOverZRight = lVOverZLeft + lVOverZStepAff;
		double lLightOverZRight = lLightOverZLeft + lLightOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lULeft = lUOverZLeft * lZLeft;
		double lVLeft = lVOverZLeft * lZLeft;
		double lLightLeft = lLightOverZLeft * lZLeft;

		double lZRight;
		double lURight;
		double lVRight;
		double lLightRight;
		Lepra::FixedPointMath::Fixed16_16 lU;
		Lepra::FixedPointMath::Fixed16_16 lV;
		Lepra::FixedPointMath::Fixed16_16 lL;
		Lepra::FixedPointMath::Fixed16_16 lDeltaU = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaV = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaL = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//

		float lOneOverZ = (float)pLeft->GetOneOverZ();
		unsigned char* l8BitLightBuffer = pThis->m8BitLightBuffer;

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lURight = lUOverZRight * lZRight;
			lVRight = lVOverZRight * lZRight;
			lLightRight = lLightOverZRight * lZRight;

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();
			lL = Lepra::FixedPointMath::FloatToFixed16_16(lLightLeft * 0.9999999f);

			lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) >> pThis->mAffineExp;
			lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) >> pThis->mAffineExp;
			lDeltaL = Lepra::FixedPointMath::FloatToFixed16_16(lLightRight - lLightLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				if (lOneOverZ > *lZBuffer)
				{
					*lDestBits = l8BitLightBuffer[(lL & 0xFF00) + lTextureBits[((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)]];
					*lZBuffer = lOneOverZ;
				}

				lDestBits++;
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lU += lDeltaU;
				lV += lDeltaV;
				lL += lDeltaL;
			}

			lZLeft = lZRight;
			lULeft = lURight;
			lVLeft = lVRight;
			lLightLeft = lLightRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lUOverZRight += lUOverZStepAff;
			lVOverZRight += lVOverZStepAff;
			lLightOverZRight += lLightOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lURight = lZRight * (pRight->GetUOverZ() - pGradients->GetUOverZXStep());
			lVRight = lZRight * (pRight->GetVOverZ() - pGradients->GetVOverZXStep());
			lLightRight = lZRight * (pRight->GetROverZ() - pGradients->GetROverZXStep());

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();
			lL = Lepra::FixedPointMath::FloatToFixed16_16(lLightLeft * 0.9999999f);

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) / lRestWidth;
				lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) / lRestWidth;
				lDeltaL = Lepra::FixedPointMath::FloatToFixed16_16(lLightRight - lLightLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				if (lOneOverZ > *lZBuffer)
				{
					*lDestBits = l8BitLightBuffer[(lL & 0xFF00) + lTextureBits[((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)]];
					*lZBuffer = lOneOverZ;
				}

				lDestBits++;
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lU += lDeltaU;
				lV += lDeltaV;
				lL += lDeltaL;
			}
		}
	}
}

void Software3DPainter::DrawScanLineUVRGB15BitFastZTZW(const GradientsUVRGB* pGradients, 
														   EdgeUVRGB* pLeft, 
														   EdgeUVRGB* pRight,
														   const Lepra::Canvas* pTexture,
														   Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned short* lDestBits = (unsigned short*)pThis->mScreen->GetBuffer() +
									 (pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart);
		unsigned short* lTextureBits = (unsigned short*)pTexture->GetBuffer();
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		int lTexturePitch = pTexture->GetPitch();
		unsigned int lTWMask = pTexture->GetWidth() - 1;
		unsigned int lTHMask = pTexture->GetHeight() - 1;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lUOverZLeft = pLeft->GetUOverZ();
		double lVOverZLeft = pLeft->GetVOverZ();
		double lROverZLeft = pLeft->GetROverZ();
		double lGOverZLeft = pLeft->GetGOverZ();
		double lBOverZLeft = pLeft->GetBOverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lUOverZStepAff = pGradients->GetUOverZXStep() * lAffineLengthFloat;
		double lVOverZStepAff = pGradients->GetVOverZXStep() * lAffineLengthFloat;
		double lROverZStepAff = pGradients->GetROverZXStep() * lAffineLengthFloat;
		double lGOverZStepAff = pGradients->GetGOverZXStep() * lAffineLengthFloat;
		double lBOverZStepAff = pGradients->GetBOverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lUOverZRight = lUOverZLeft + lUOverZStepAff;
		double lVOverZRight = lVOverZLeft + lVOverZStepAff;
		double lROverZRight = lROverZLeft + lROverZStepAff;
		double lGOverZRight = lGOverZLeft + lGOverZStepAff;
		double lBOverZRight = lBOverZLeft + lBOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lULeft = lUOverZLeft * lZLeft;
		double lVLeft = lVOverZLeft * lZLeft;
		double lRLeft = lROverZLeft * lZLeft * 255.0f;
		double lGLeft = lGOverZLeft * lZLeft * 255.0f;
		double lBLeft = lBOverZLeft * lZLeft * 255.0f;

		MACRO_CHECK_RGB(lRLeft, lGLeft, lBLeft, 255.0f);

		double lZRight;
		double lURight;
		double lVRight;
		double lRRight;
		double lGRight;
		double lBRight;
		Lepra::FixedPointMath::Fixed16_16 lU;
		Lepra::FixedPointMath::Fixed16_16 lV;
		Lepra::FixedPointMath::Fixed16_16 lR;
		Lepra::FixedPointMath::Fixed16_16 lG;
		Lepra::FixedPointMath::Fixed16_16 lB;
		Lepra::FixedPointMath::Fixed16_16 lDeltaU = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaV = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaR = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaG = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaB = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//
		float lOneOverZ = (float)pLeft->GetOneOverZ();

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lURight = lUOverZRight * lZRight;
			lVRight = lVOverZRight * lZRight;
			lRRight = lROverZRight * lZRight * 255.0f;
			lGRight = lGOverZRight * lZRight * 255.0f;
			lBRight = lBOverZRight * lZRight * 255.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 255.0f);

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();
			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) >> pThis->mAffineExp;
			lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) >> pThis->mAffineExp;
			lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) >> pThis->mAffineExp;
			lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) >> pThis->mAffineExp;
			lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				if (lOneOverZ > *lZBuffer)
				{
					unsigned int lCol = lTextureBits[((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)];
					
					int lRed   = (lR * (((lCol >> 10) & 31) << 3)) >> 24;
					int lGreen = (lG * (((lCol >> 5) & 31) << 3)) >> 24;
					int lBlue  = (lB * (((lCol >> 0) & 31) << 3)) >> 24;

					*lDestBits = (Lepra::uint16)(((lRed >> 3) << 10) | ((lGreen >> 3) << 5) | (lBlue >> 3));
					*lZBuffer = lOneOverZ;
				}

				lDestBits++;
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lU += lDeltaU;
				lV += lDeltaV;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}

			lZLeft = lZRight;
			lULeft = lURight;
			lVLeft = lVRight;
			lRLeft = lRRight;
			lGLeft = lGRight;
			lBLeft = lBRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lUOverZRight += lUOverZStepAff;
			lVOverZRight += lVOverZStepAff;
			lROverZRight += lROverZStepAff;
			lGOverZRight += lGOverZStepAff;
			lBOverZRight += lBOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lURight = lZRight * (pRight->GetUOverZ() - pGradients->GetUOverZXStep());
			lVRight = lZRight * (pRight->GetVOverZ() - pGradients->GetVOverZXStep());
			lRRight = lZRight * (pRight->GetROverZ() - pGradients->GetROverZXStep()) * 255.0f;
			lGRight = lZRight * (pRight->GetGOverZ() - pGradients->GetGOverZXStep()) * 255.0f;
			lBRight = lZRight * (pRight->GetBOverZ() - pGradients->GetBOverZXStep()) * 255.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 255.0f);

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();
			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) / lRestWidth;
				lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) / lRestWidth;
				lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) / lRestWidth;
				lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) / lRestWidth;
				lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				if (lOneOverZ > *lZBuffer)
				{
					unsigned int lCol = lTextureBits[((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)];
					
					int lRed   = (lR * (((lCol >> 10) & 31) << 3)) >> 24;
					int lGreen = (lG * (((lCol >> 5) & 31) << 3)) >> 24;
					int lBlue  = (lB * (((lCol >> 0) & 31) << 3)) >> 24;

					*lDestBits = (Lepra::uint16)(((lRed >> 3) << 10) | ((lGreen >> 3) << 5) | (lBlue >> 3));
					*lZBuffer = lOneOverZ;
				}

				lDestBits++;
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lU += lDeltaU;
				lV += lDeltaV;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}
		}
	}
}

void Software3DPainter::DrawScanLineUVRGB16BitFastZTZW(const GradientsUVRGB* pGradients, 
														   EdgeUVRGB* pLeft, 
														   EdgeUVRGB* pRight,
														   const Lepra::Canvas* pTexture,
														   Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned short* lDestBits = (unsigned short*)pThis->mScreen->GetBuffer() +
									 (pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart);
		unsigned short* lTextureBits = (unsigned short*)pTexture->GetBuffer();
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		int lTexturePitch = pTexture->GetPitch();
		unsigned int lTWMask = pTexture->GetWidth() - 1;
		unsigned int lTHMask = pTexture->GetHeight() - 1;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lUOverZLeft = pLeft->GetUOverZ();
		double lVOverZLeft = pLeft->GetVOverZ();
		double lROverZLeft = pLeft->GetROverZ();
		double lGOverZLeft = pLeft->GetGOverZ();
		double lBOverZLeft = pLeft->GetBOverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lUOverZStepAff = pGradients->GetUOverZXStep() * lAffineLengthFloat;
		double lVOverZStepAff = pGradients->GetVOverZXStep() * lAffineLengthFloat;
		double lROverZStepAff = pGradients->GetROverZXStep() * lAffineLengthFloat;
		double lGOverZStepAff = pGradients->GetGOverZXStep() * lAffineLengthFloat;
		double lBOverZStepAff = pGradients->GetBOverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lUOverZRight = lUOverZLeft + lUOverZStepAff;
		double lVOverZRight = lVOverZLeft + lVOverZStepAff;
		double lROverZRight = lROverZLeft + lROverZStepAff;
		double lGOverZRight = lGOverZLeft + lGOverZStepAff;
		double lBOverZRight = lBOverZLeft + lBOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lULeft = lUOverZLeft * lZLeft;
		double lVLeft = lVOverZLeft * lZLeft;
		double lRLeft = lROverZLeft * lZLeft * 255.0f;
		double lGLeft = lGOverZLeft * lZLeft * 255.0f;
		double lBLeft = lBOverZLeft * lZLeft * 255.0f;

		MACRO_CHECK_RGB(lRLeft, lGLeft, lBLeft, 255.0f);

		double lZRight;
		double lURight;
		double lVRight;
		double lRRight;
		double lGRight;
		double lBRight;
		Lepra::FixedPointMath::Fixed16_16 lU;
		Lepra::FixedPointMath::Fixed16_16 lV;
		Lepra::FixedPointMath::Fixed16_16 lR;
		Lepra::FixedPointMath::Fixed16_16 lG;
		Lepra::FixedPointMath::Fixed16_16 lB;
		Lepra::FixedPointMath::Fixed16_16 lDeltaU = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaV = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaR = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaG = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaB = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//
		float lOneOverZ = (float)pLeft->GetOneOverZ();

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lURight = lUOverZRight * lZRight;
			lVRight = lVOverZRight * lZRight;
			lRRight = lROverZRight * lZRight * 255.0f;
			lGRight = lGOverZRight * lZRight * 255.0f;
			lBRight = lBOverZRight * lZRight * 255.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 255.0f);

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();
			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) >> pThis->mAffineExp;
			lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) >> pThis->mAffineExp;
			lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) >> pThis->mAffineExp;
			lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) >> pThis->mAffineExp;
			lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				if (lOneOverZ > *lZBuffer)
				{
					unsigned int lCol = lTextureBits[((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)];
					
					int lRed   = (lR * (((lCol >> 11) & 31) << 3)) >> 24;
					int lGreen = (lG * (((lCol >> 5) & 63) << 2)) >> 24;
					int lBlue  = (lB * (((lCol >> 0) & 31) << 3)) >> 24;

					*lDestBits = (Lepra::uint16)(((lRed >> 3) << 11) | ((lGreen >> 2) << 5) | (lBlue >> 3));
					*lZBuffer = lOneOverZ;
				}

				lDestBits++;
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lU += lDeltaU;
				lV += lDeltaV;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}

			lZLeft = lZRight;
			lULeft = lURight;
			lVLeft = lVRight;
			lRLeft = lRRight;
			lGLeft = lGRight;
			lBLeft = lBRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lUOverZRight += lUOverZStepAff;
			lVOverZRight += lVOverZStepAff;
			lROverZRight += lROverZStepAff;
			lGOverZRight += lGOverZStepAff;
			lBOverZRight += lBOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lURight = lZRight * (pRight->GetUOverZ() - pGradients->GetUOverZXStep());
			lVRight = lZRight * (pRight->GetVOverZ() - pGradients->GetVOverZXStep());
			lRRight = lZRight * (pRight->GetROverZ() - pGradients->GetROverZXStep()) * 255.0f;
			lGRight = lZRight * (pRight->GetGOverZ() - pGradients->GetGOverZXStep()) * 255.0f;
			lBRight = lZRight * (pRight->GetBOverZ() - pGradients->GetBOverZXStep()) * 255.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 255.0f);

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();
			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) / lRestWidth;
				lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) / lRestWidth;
				lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) / lRestWidth;
				lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) / lRestWidth;
				lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				if (lOneOverZ > *lZBuffer)
				{
					unsigned int lCol = lTextureBits[((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)];
					
					int lRed   = (lR * (((lCol >> 11) & 31) << 3)) >> 24;
					int lGreen = (lG * (((lCol >> 5) & 63) << 2)) >> 24;
					int lBlue  = (lB * (((lCol >> 0) & 31) << 3)) >> 24;

					*lDestBits = (Lepra::uint16)(((lRed >> 3) << 11) | ((lGreen >> 2) << 5) | (lBlue >> 3));
					*lZBuffer = lOneOverZ;
				}

				lDestBits++;
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lU += lDeltaU;
				lV += lDeltaV;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}
		}
	}
}

void Software3DPainter::DrawScanLineUVRGB24BitFastZTZW(const GradientsUVRGB* pGradients, 
														   EdgeUVRGB* pLeft, 
														   EdgeUVRGB* pRight,
														   const Lepra::Canvas* pTexture,
														   Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned char* lDestBits = (unsigned char*)pThis->mScreen->GetBuffer() +
									 (pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart) * 3;
		unsigned char* lTextureBits = (unsigned char*)pTexture->GetBuffer();
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		int lTexturePitch = pTexture->GetPitch();
		unsigned int lTWMask = pTexture->GetWidth() - 1;
		unsigned int lTHMask = pTexture->GetHeight() - 1;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lUOverZLeft = pLeft->GetUOverZ();
		double lVOverZLeft = pLeft->GetVOverZ();
		double lROverZLeft = pLeft->GetROverZ();
		double lGOverZLeft = pLeft->GetGOverZ();
		double lBOverZLeft = pLeft->GetBOverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lUOverZStepAff = pGradients->GetUOverZXStep() * lAffineLengthFloat;
		double lVOverZStepAff = pGradients->GetVOverZXStep() * lAffineLengthFloat;
		double lROverZStepAff = pGradients->GetROverZXStep() * lAffineLengthFloat;
		double lGOverZStepAff = pGradients->GetGOverZXStep() * lAffineLengthFloat;
		double lBOverZStepAff = pGradients->GetBOverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lUOverZRight = lUOverZLeft + lUOverZStepAff;
		double lVOverZRight = lVOverZLeft + lVOverZStepAff;
		double lROverZRight = lROverZLeft + lROverZStepAff;
		double lGOverZRight = lGOverZLeft + lGOverZStepAff;
		double lBOverZRight = lBOverZLeft + lBOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lULeft = lUOverZLeft * lZLeft;
		double lVLeft = lVOverZLeft * lZLeft;
		double lRLeft = lROverZLeft * lZLeft * 255.0f;
		double lGLeft = lGOverZLeft * lZLeft * 255.0f;
		double lBLeft = lBOverZLeft * lZLeft * 255.0f;

		MACRO_CHECK_RGB(lRLeft, lGLeft, lBLeft, 255.0f);

		double lZRight;
		double lURight;
		double lVRight;
		double lRRight;
		double lGRight;
		double lBRight;
		Lepra::FixedPointMath::Fixed16_16 lU;
		Lepra::FixedPointMath::Fixed16_16 lV;
		Lepra::FixedPointMath::Fixed16_16 lR;
		Lepra::FixedPointMath::Fixed16_16 lG;
		Lepra::FixedPointMath::Fixed16_16 lB;
		Lepra::FixedPointMath::Fixed16_16 lDeltaU = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaV = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaR = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaG = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaB = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//
		float lOneOverZ = (float)pLeft->GetOneOverZ();

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lURight = lUOverZRight * lZRight;
			lVRight = lVOverZRight * lZRight;
			lRRight = lROverZRight * lZRight * 255.0f;
			lGRight = lGOverZRight * lZRight * 255.0f;
			lBRight = lBOverZRight * lZRight * 255.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 255.0f);

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();
			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) >> pThis->mAffineExp;
			lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) >> pThis->mAffineExp;
			lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) >> pThis->mAffineExp;
			lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) >> pThis->mAffineExp;
			lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				if (lOneOverZ > *lZBuffer)
				{
					unsigned char* lT = &lTextureBits[(((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)) * 3];

					lDestBits[0] = (unsigned char)((lB * (unsigned int)lT[0]) >> 24);
					lDestBits[1] = (unsigned char)((lG * (unsigned int)lT[1]) >> 24);
					lDestBits[2] = (unsigned char)((lR * (unsigned int)lT[2]) >> 24);
					*lZBuffer = lOneOverZ;
				}

				lDestBits += 3;
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lU += lDeltaU;
				lV += lDeltaV;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}

			lZLeft = lZRight;
			lULeft = lURight;
			lVLeft = lVRight;
			lRLeft = lRRight;
			lGLeft = lGRight;
			lBLeft = lBRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lUOverZRight += lUOverZStepAff;
			lVOverZRight += lVOverZStepAff;
			lROverZRight += lROverZStepAff;
			lGOverZRight += lGOverZStepAff;
			lBOverZRight += lBOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lURight = lZRight * (pRight->GetUOverZ() - pGradients->GetUOverZXStep());
			lVRight = lZRight * (pRight->GetVOverZ() - pGradients->GetVOverZXStep());
			lRRight = lZRight * (pRight->GetROverZ() - pGradients->GetROverZXStep()) * 255.0f;
			lGRight = lZRight * (pRight->GetGOverZ() - pGradients->GetGOverZXStep()) * 255.0f;
			lBRight = lZRight * (pRight->GetBOverZ() - pGradients->GetBOverZXStep()) * 255.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 255.0f);

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();
			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) / lRestWidth;
				lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) / lRestWidth;
				lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) / lRestWidth;
				lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) / lRestWidth;
				lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				if (lOneOverZ > *lZBuffer)
				{
					unsigned char* lT = &lTextureBits[(((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask)) * 3];

					lDestBits[0] = (unsigned char)((lB * (unsigned int)lT[0]) >> 24);
					lDestBits[1] = (unsigned char)((lG * (unsigned int)lT[1]) >> 24);
					lDestBits[2] = (unsigned char)((lR * (unsigned int)lT[2]) >> 24);
					*lZBuffer = lOneOverZ;
				}

				lDestBits += 3;
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lU += lDeltaU;
				lV += lDeltaV;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}
		}
	}
}

void Software3DPainter::DrawScanLineUVRGB32BitFastZTZW(const GradientsUVRGB* pGradients, 
														   EdgeUVRGB* pLeft, 
														   EdgeUVRGB* pRight,
														   const Lepra::Canvas* pTexture,
														   Software3DPainter* pThis)
{
	int lXStart = pLeft->GetX();
	int lWidth = pRight->GetX() - lXStart;

	if (lWidth > 0)
	{
		//
		// Step 1: Set up all kind of values.
		//

		unsigned char* lDestBits = (unsigned char*)pThis->mScreen->GetBuffer() +
									 ((pLeft->GetY() * pThis->mScreen->GetPitch() + lXStart) << 2);
		unsigned int* lTextureBits = (unsigned int*)pTexture->GetBuffer();
		float* lZBuffer = (float*)pThis->mZBuffer->GetBuffer() +
							(pLeft->GetY() - pThis->mZBufferY) * pThis->mZBuffer->GetPitch() + lXStart - pThis->mZBufferX;

		int lTexturePitch = pTexture->GetPitch();
		unsigned int lTWMask = pTexture->GetWidth() - 1;
		unsigned int lTHMask = pTexture->GetHeight() - 1;

		double lOneOverZLeft = pLeft->GetOneOverZ();
		double lUOverZLeft = pLeft->GetUOverZ();
		double lVOverZLeft = pLeft->GetVOverZ();
		double lROverZLeft = pLeft->GetROverZ();
		double lGOverZLeft = pLeft->GetGOverZ();
		double lBOverZLeft = pLeft->GetBOverZ();

		double lAffineLengthFloat = pThis->mAffineLengthFloat;
		double lOneOverZStepAff = pGradients->GetOneOverZXStep() * lAffineLengthFloat;
		double lUOverZStepAff = pGradients->GetUOverZXStep() * lAffineLengthFloat;
		double lVOverZStepAff = pGradients->GetVOverZXStep() * lAffineLengthFloat;
		double lROverZStepAff = pGradients->GetROverZXStep() * lAffineLengthFloat;
		double lGOverZStepAff = pGradients->GetGOverZXStep() * lAffineLengthFloat;
		double lBOverZStepAff = pGradients->GetBOverZXStep() * lAffineLengthFloat;

		double lOneOverZRight = lOneOverZLeft + lOneOverZStepAff;
		double lUOverZRight = lUOverZLeft + lUOverZStepAff;
		double lVOverZRight = lVOverZLeft + lVOverZStepAff;
		double lROverZRight = lROverZLeft + lROverZStepAff;
		double lGOverZRight = lGOverZLeft + lGOverZStepAff;
		double lBOverZRight = lBOverZLeft + lBOverZStepAff;

		double lZLeft = 1.0f / lOneOverZLeft;
		double lULeft = lUOverZLeft * lZLeft;
		double lVLeft = lVOverZLeft * lZLeft;
		double lRLeft = lROverZLeft * lZLeft * 255.0f;
		double lGLeft = lGOverZLeft * lZLeft * 255.0f;
		double lBLeft = lBOverZLeft * lZLeft * 255.0f;

		MACRO_CHECK_RGB(lRLeft, lGLeft, lBLeft, 255.0f);

		double lZRight;
		double lURight;
		double lVRight;
		double lRRight;
		double lGRight;
		double lBRight;
		Lepra::FixedPointMath::Fixed16_16 lU;
		Lepra::FixedPointMath::Fixed16_16 lV;
		Lepra::FixedPointMath::Fixed16_16 lR;
		Lepra::FixedPointMath::Fixed16_16 lG;
		Lepra::FixedPointMath::Fixed16_16 lB;
		Lepra::FixedPointMath::Fixed16_16 lDeltaU = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaV = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaR = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaG = 0;
		Lepra::FixedPointMath::Fixed16_16 lDeltaB = 0;

		const unsigned int lAffineLength = pThis->mAffineLength;

		int lNumSubdivisions = (lWidth >> pThis->mAffineExp);	// Width / AffineLength
		int lRestWidth = lWidth & (lAffineLength - 1);	// Width % AffineLength

		if (lRestWidth == 0)
		{
			lNumSubdivisions--;
			lRestWidth = lAffineLength;
		}

		//
		// Step 2: Render all subdivisions.
		//
		float lOneOverZ = (float)pLeft->GetOneOverZ();

		while (lNumSubdivisions-- > 0)
		{
			lZRight = 1.0f / lOneOverZRight;
			lURight = lUOverZRight * lZRight;
			lVRight = lVOverZRight * lZRight;
			lRRight = lROverZRight * lZRight * 255.0f;
			lGRight = lGOverZRight * lZRight * 255.0f;
			lBRight = lBOverZRight * lZRight * 255.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 255.0f);

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();
			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) >> pThis->mAffineExp;
			lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) >> pThis->mAffineExp;
			lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) >> pThis->mAffineExp;
			lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) >> pThis->mAffineExp;
			lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) >> pThis->mAffineExp;

			for (unsigned int i = lAffineLength; i > 0; i--)
			{
				if (lOneOverZ > *lZBuffer)
				{
					unsigned char* lT = (unsigned char*)&lTextureBits[(((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask))];

					lDestBits[0] = (unsigned char)((lB * (unsigned int)lT[0]) >> 24);
					lDestBits[1] = (unsigned char)((lG * (unsigned int)lT[1]) >> 24);
					lDestBits[2] = (unsigned char)((lR * (unsigned int)lT[2]) >> 24);
					*lZBuffer = lOneOverZ;
				}

				lDestBits += 4;
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lU += lDeltaU;
				lV += lDeltaV;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}

			lZLeft = lZRight;
			lULeft = lURight;
			lVLeft = lVRight;
			lRLeft = lRRight;
			lGLeft = lGRight;
			lBLeft = lBRight;
	
			lOneOverZRight += lOneOverZStepAff;
			lUOverZRight += lUOverZStepAff;
			lVOverZRight += lVOverZStepAff;
			lROverZRight += lROverZStepAff;
			lGOverZRight += lGOverZStepAff;
			lBOverZRight += lBOverZStepAff;
		}

		//
		// Step 3: Render the rest.
		//
		
		if (lRestWidth > 0)
		{
			lZRight = 1.0f / (pRight->GetOneOverZ() - pGradients->GetOneOverZXStep());
			lURight = lZRight * (pRight->GetUOverZ() - pGradients->GetUOverZXStep());
			lVRight = lZRight * (pRight->GetVOverZ() - pGradients->GetVOverZXStep());
			lRRight = lZRight * (pRight->GetROverZ() - pGradients->GetROverZXStep()) * 255.0f;
			lGRight = lZRight * (pRight->GetGOverZ() - pGradients->GetGOverZXStep()) * 255.0f;
			lBRight = lZRight * (pRight->GetBOverZ() - pGradients->GetBOverZXStep()) * 255.0f;

			MACRO_CHECK_RGB(lRRight, lGRight, lBRight, 255.0f);

			lU = Lepra::FixedPointMath::FloatToFixed16_16(lULeft) + pGradients->GetDUDXModifier();
			lV = Lepra::FixedPointMath::FloatToFixed16_16(lVLeft) + pGradients->GetDVDXModifier();
			lR = Lepra::FixedPointMath::FloatToFixed16_16(lRLeft) + pGradients->GetDRDXModifier();
			lG = Lepra::FixedPointMath::FloatToFixed16_16(lGLeft) + pGradients->GetDGDXModifier();
			lB = Lepra::FixedPointMath::FloatToFixed16_16(lBLeft) + pGradients->GetDBDXModifier();

			if (--lRestWidth > 0)
			{
				// Guard against div-by-0 for 1 pixel lines.
				lDeltaU = Lepra::FixedPointMath::FloatToFixed16_16(lURight - lULeft) / lRestWidth;
				lDeltaV = Lepra::FixedPointMath::FloatToFixed16_16(lVRight - lVLeft) / lRestWidth;
				lDeltaR = Lepra::FixedPointMath::FloatToFixed16_16(lRRight - lRLeft) / lRestWidth;
				lDeltaG = Lepra::FixedPointMath::FloatToFixed16_16(lGRight - lGLeft) / lRestWidth;
				lDeltaB = Lepra::FixedPointMath::FloatToFixed16_16(lBRight - lBLeft) / lRestWidth;
			}

			for (int i = 0; i <= lRestWidth; i++)
			{
				if (lOneOverZ > *lZBuffer)
				{
					unsigned char* lT = (unsigned char*)&lTextureBits[(((lV >> 16) & lTHMask) * lTexturePitch + ((lU >> 16) & lTWMask))];

					lDestBits[0] = (unsigned char)((lB * (unsigned int)lT[0]) >> 24);
					lDestBits[1] = (unsigned char)((lG * (unsigned int)lT[1]) >> 24);
					lDestBits[2] = (unsigned char)((lR * (unsigned int)lT[2]) >> 24);
					*lZBuffer = lOneOverZ;
				}

				lDestBits += 4;
				lZBuffer++;
				lOneOverZ += (float)pGradients->GetOneOverZXStep();

				lU += lDeltaU;
				lV += lDeltaV;
				lR += lDeltaR;
				lG += lDeltaG;
				lB += lDeltaB;
			}
		}
	}
}

} // End namespace.
