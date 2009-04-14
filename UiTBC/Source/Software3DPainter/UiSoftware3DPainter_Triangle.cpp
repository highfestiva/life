/*
	Lepra::File:   Software3DPainter_Triangle.cpp
	Class:  Software3DPainter
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand
*/

///////////////////////////////////////////////////////////////////////////////////
//	
//	The triangle renderers.
//	
///////////////////////////////////////////////////////////////////////////////////


#include "../../Include/UiTBC.h"
#include "../../Include/UiSoftware3DPainter.h"
#include "../../Include/UiGradients.h"
#include "../../Include/UiEdge.h"
#include "../../Include/UiTexture.h"
#include "../../Include/UiVertex.h"

#define MACRO_PREPARE_TOP_TRIANGLE() \
	if (pMiddleIsLeft == true) \
	{ \
		lLeftEdge = pTopToMiddleEdge; \
		lRightEdge = pTopToBottomEdge; \
	} \
	else \
	{ \
		lLeftEdge = pTopToBottomEdge; \
		lRightEdge = pTopToMiddleEdge; \
	} \
 \
	if (lLeftEdge->GetY() < mClipTop) \
	{ \
		int lStepCount = (int)(mClipTop - lLeftEdge->GetY()); \
		lLeftEdge->Step(lStepCount); \
		lRightEdge->Step(lStepCount); \
	} \
 \
	int lHeight = pTopToMiddleEdge->GetHeight(); \
 \
	if ((lLeftEdge->GetY() + lHeight) > (int)mClipBottom) \
	{ \
		lHeight -= ((lLeftEdge->GetY() + lHeight) - ((int)mClipBottom + 1)); \
	} \


#define MACRO_PREPARE_BOTTOM_TRIANGLE() \
	if (pMiddleIsLeft == true) \
	{ \
		lLeftEdge = pMiddleToBottomEdge; \
		lRightEdge = pTopToBottomEdge; \
	} \
	else \
	{ \
		lLeftEdge = pTopToBottomEdge; \
		lRightEdge = pMiddleToBottomEdge; \
	} \
 \
	if (lLeftEdge->GetY() < mClipTop) \
	{ \
		int lStepCount = (int)(mClipTop - lLeftEdge->GetY()); \
		lLeftEdge->Step(lStepCount); \
	} \
	if (lRightEdge->GetY() < mClipTop) \
	{ \
		int lStepCount = (int)(mClipTop - lRightEdge->GetY()); \
		lRightEdge->Step(lStepCount); \
	} \
 \
	lHeight = pMiddleToBottomEdge->GetHeight(); \
 \
	if ((lLeftEdge->GetY() + lHeight) > (int)mClipBottom) \
	{ \
		lHeight -= ((lLeftEdge->GetY() + lHeight) - ((int)mClipBottom + 1)); \
	} \







namespace UiTbc
{

void Software3DPainter::DrawSingleColorTriangle(EdgeZ* pTopToBottomEdge,
												EdgeZ* pTopToMiddleEdge,
												EdgeZ* pMiddleToBottomEdge,
												Gradients* pGradients,
												bool pMiddleIsLeft,
												const Lepra::Color& pColor)
{
	Lepra::Color lColor(pColor);
	if (mScreen->GetBitDepth() == Lepra::Canvas::BITDEPTH_8_BIT &&
	   m8BitColorSearchMode == true)
	{
		lColor.mAlpha = mPainter.FindMatchingColor(pColor);
	}

	EdgeZ* lLeftEdge;
	EdgeZ* lRightEdge;

	MACRO_PREPARE_TOP_TRIANGLE();

	while (lHeight-- > 0)
	{
		DrawSingleColorScanLineFunc(pGradients, lLeftEdge, lRightEdge, lColor, this);
		lLeftEdge->Step();
		lRightEdge->Step();
	}

	MACRO_PREPARE_BOTTOM_TRIANGLE();

	while (lHeight-- > 0)
	{
		DrawSingleColorScanLineFunc(pGradients, lLeftEdge, lRightEdge, lColor, this);
		lLeftEdge->Step();
		lRightEdge->Step();
	}
}

void Software3DPainter::DrawRGBTriangle(EdgeRGB* pTopToBottomEdge,
										EdgeRGB* pTopToMiddleEdge,
										EdgeRGB* pMiddleToBottomEdge,
										GradientsRGB* pGradients,
										bool pMiddleIsLeft)
{
	EdgeRGB* lLeftEdge;
	EdgeRGB* lRightEdge;

	MACRO_PREPARE_TOP_TRIANGLE();

	while (lHeight-- > 0)
	{
		DrawRGBScanLineFunc(pGradients, lLeftEdge, lRightEdge, this);
		lLeftEdge->Step();
		lRightEdge->Step();
	}

	MACRO_PREPARE_BOTTOM_TRIANGLE();

	while (lHeight-- > 0)
	{
		DrawRGBScanLineFunc(pGradients, lLeftEdge, lRightEdge, this);
		lLeftEdge->Step();
		lRightEdge->Step();
	}
}

void Software3DPainter::DrawUVTriangle(EdgeUV* pTopToBottomEdge,
									   EdgeUV* pTopToMiddleEdge,
									   EdgeUV* pMiddleToBottomEdge,
									   GradientsUV* pGradients,
									   bool pMiddleIsLeft)
{
	EdgeUV* lLeftEdge;
	EdgeUV* lRightEdge;

	MACRO_PREPARE_TOP_TRIANGLE();

	while (lHeight-- > 0)
	{
		DrawUVScanLineFunc(pGradients, lLeftEdge, lRightEdge, this);
		lLeftEdge->Step();
		lRightEdge->Step();
	}

	MACRO_PREPARE_BOTTOM_TRIANGLE();

	while (lHeight-- > 0)
	{
		DrawUVScanLineFunc(pGradients, lLeftEdge, lRightEdge, this);
		lLeftEdge->Step();
		lRightEdge->Step();
	}
}

void Software3DPainter::DrawUVRGBTriangle(EdgeUVRGB* pTopToBottomEdge,
										  EdgeUVRGB* pTopToMiddleEdge,
										  EdgeUVRGB* pMiddleToBottomEdge,
										  GradientsUVRGB* pGradients,
										  bool pMiddleIsLeft)
{
	EdgeUVRGB* lLeftEdge;
	EdgeUVRGB* lRightEdge;

	MACRO_PREPARE_TOP_TRIANGLE();

	while (lHeight-- > 0)
	{
		DrawUVRGBScanLineFunc(pGradients, lLeftEdge, lRightEdge, this);
		lLeftEdge->Step();
		lRightEdge->Step();
	}

	MACRO_PREPARE_BOTTOM_TRIANGLE();

	while (lHeight-- > 0)
	{
		DrawUVRGBScanLineFunc(pGradients, lLeftEdge, lRightEdge, this);
		lLeftEdge->Step();
		lRightEdge->Step();
	}
}

void Software3DPainter::DrawUVMTriangle(EdgeUVM* pTopToBottomEdge,
										EdgeUVM* pTopToMiddleEdge,
										EdgeUVM* pMiddleToBottomEdge,
										GradientsUVM* pGradients,
										bool pMiddleIsLeft)
{
	EdgeUVM* lLeftEdge;
	EdgeUVM* lRightEdge;

	MACRO_PREPARE_TOP_TRIANGLE();

	while (lHeight-- > 0)
	{
		DrawUVMScanLineFunc(pGradients, lLeftEdge, lRightEdge, this);
		lLeftEdge->Step();
		lRightEdge->Step();
	}

	MACRO_PREPARE_BOTTOM_TRIANGLE();

	while (lHeight-- > 0)
	{
		DrawUVMScanLineFunc(pGradients, lLeftEdge, lRightEdge, this);
		lLeftEdge->Step();
		lRightEdge->Step();
	}
}

void Software3DPainter::DrawUVRGBMTriangle(EdgeUVRGBM* pTopToBottomEdge,
										   EdgeUVRGBM* pTopToMiddleEdge,
										   EdgeUVRGBM* pMiddleToBottomEdge,
										   GradientsUVRGBM* pGradients,
										   bool pMiddleIsLeft)
{
	EdgeUVRGBM* lLeftEdge;
	EdgeUVRGBM* lRightEdge;

	MACRO_PREPARE_TOP_TRIANGLE();

	while (lHeight-- > 0)
	{
		DrawUVRGBMScanLineFunc(pGradients, lLeftEdge, lRightEdge, this);
		lLeftEdge->Step();
		lRightEdge->Step();
	}

	MACRO_PREPARE_BOTTOM_TRIANGLE();

	while (lHeight-- > 0)
	{
		DrawUVRGBMScanLineFunc(pGradients, lLeftEdge, lRightEdge, this);
		lLeftEdge->Step();
		lRightEdge->Step();
	}
}


void Software3DPainter::DrawUVTriangleFast(EdgeUV* pTopToBottomEdge,
										   EdgeUV* pTopToMiddleEdge,
										   EdgeUV* pMiddleToBottomEdge,
										   GradientsUV* pGradients,
										   bool pMiddleIsLeft,
										   int pMipMapLevel)
{
	const Lepra::Canvas* lTexture = mTexture->GetColorMap(pMipMapLevel);

	EdgeUV* lLeftEdge;
	EdgeUV* lRightEdge;

	MACRO_PREPARE_TOP_TRIANGLE();

	while (lHeight-- > 0)
	{
		DrawUVScanLineFuncFast(pGradients, lLeftEdge, lRightEdge, lTexture, this);
		lLeftEdge->Step();
		lRightEdge->Step();
	}

	MACRO_PREPARE_BOTTOM_TRIANGLE();

	while (lHeight-- > 0)
	{
		DrawUVScanLineFuncFast(pGradients, lLeftEdge, lRightEdge, lTexture, this);
		lLeftEdge->Step();
		lRightEdge->Step();
	}
}

void Software3DPainter::DrawUVRGBTriangleFast(EdgeUVRGB* pTopToBottomEdge,
											  EdgeUVRGB* pTopToMiddleEdge,
											  EdgeUVRGB* pMiddleToBottomEdge,
											  GradientsUVRGB* pGradients,
											  bool pMiddleIsLeft,
											  int pMipMapLevel)
{
	const Lepra::Canvas* lTexture = mTexture->GetColorMap(pMipMapLevel);

	EdgeUVRGB* lLeftEdge;
	EdgeUVRGB* lRightEdge;

	MACRO_PREPARE_TOP_TRIANGLE();

	while (lHeight-- > 0)
	{
		DrawUVRGBScanLineFuncFast(pGradients, lLeftEdge, lRightEdge, lTexture, this);
		lLeftEdge->Step();
		lRightEdge->Step();
	}

	MACRO_PREPARE_BOTTOM_TRIANGLE();

	while (lHeight-- > 0)
	{
		DrawUVRGBScanLineFuncFast(pGradients, lLeftEdge, lRightEdge, lTexture, this);
		lLeftEdge->Step();
		lRightEdge->Step();
	}
}

} // End namespace.
