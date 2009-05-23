
// Author: Alexander Hugestrand
// Copyright (c) 2002-2008, Alexander Hugestrand

#include <math.h>
#include "../../../Lepra/Include/FixedPointMath.h"
#include "../../../Lepra/Include/GammaLookup.h"
#include "../../../Lepra/Include/Math.h"
#include "../../Include/UiEdge.h"
#include "../../Include/UiGradients.h"
#include "../../Include/UiSoftware3DPainter.h"
#include "../../Include/UiTBC.h"
#include "UiSoftware3DPainter_Macros.h"

#include <math.h>

namespace UiTbc
{

Software3DPainter::Software3DPainter()
{
	mFlags				= 0;
	mKBuffer				= 0;
	mZBuffer				= 0;
	mScreen				= 0;
	mTexture				= 0;
	m8BitLightBuffer	= 0;
	mSpecularity			= 0;
	mCurrentXYZIndex		= 0;
	mCurrentUVIndex		= 0;
	mCurrentRGBIndex		= 0;
	mCurrentNormalIndex	= 0;

	mVertexData           = 0;
	mUVData               = 0;
	mColorDataFloat       = 0;
	mColorData            = 0;
	mNormalData           = 0;
	mTranslatedVertexData = 0;
	mTranslatedNormalData = 0;

	mTextureTransformIsIdentity = true;

	mOrthographicProjection = false;

	mFaceOrientation        = FACE_CW;
	mMipMapReferenceFactor = 1.0f;

	mPoint[0].mXYZ = &mXYZ[0];
	mPoint[0].mUVM = &mUVM[0];
	mPoint[0].mRGB = &mRGB[0];
	mPoint[0].mNormal = &mNormal[0];

	mPoint[1].mXYZ = &mXYZ[1];
	mPoint[1].mUVM = &mUVM[1];
	mPoint[1].mRGB = &mRGB[1];
	mPoint[1].mNormal = &mNormal[1];

	mPoint[2].mXYZ = &mXYZ[2];
	mPoint[2].mUVM = &mUVM[2];
	mPoint[2].mRGB = &mRGB[2];
	mPoint[2].mNormal = &mNormal[2];

	int i;
	for (i = 0; i < 8; i++)
	{
		mClipPoly1[i].mXYZ = &mClipXYZ[i + 0];
		mClipPoly2[i].mXYZ = &mClipXYZ[i + 8];
		mClipPoly1[i].mRGB = &mClipRGB[i + 0];
		mClipPoly2[i].mRGB = &mClipRGB[i + 8];
		mClipPoly1[i].mUVM = &mClipUVM[i + 0];
		mClipPoly2[i].mUVM = &mClipUVM[i + 8];
		mClipPoly1[i].mNormal = &mClipNormal[i + 0];
		mClipPoly2[i].mNormal = &mClipNormal[i + 8];
	}

	SetViewFrustum(90.0f, 0.1f, 1000.0f);
	SetAffineLengthExponent(3);
	
	mRenderingHint = RENDER_NICEST;
	mAlphaSource = ALPHA_VALUE;

	UpdateScanLineFuncs();
	UpdateTextureReaderFuncs();
	UpdatePixelWriterFuncs();
	UpdateDrawLineFuncs();

	mStateChanged = false;
	mIsRendering = false;
	mHaveCallBack = false;

	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		mLights[i].mType = LIGHT_NONE;
	}

	SetAmbientLight(1.0f, 1.0f, 1.0f);
	SetLightModel(LIGHT_FLAT);
	mNumLights = 0;
}




Software3DPainter::~Software3DPainter()
{
	if (mZBuffer != 0)
	{
		delete mZBuffer;
	}

	if (mKBuffer != 0)
	{
		delete mKBuffer;
	}

	if (m8BitLightBuffer != 0)
	{
		delete[] m8BitLightBuffer;
	}

	if (mTranslatedVertexData != 0)
	{
		delete[] mTranslatedVertexData;
	}

	if (mTranslatedNormalData != 0)
	{
		delete[] mTranslatedNormalData;
	}
}


void Software3DPainter::SetFrontFace(FaceOrientation pFO)
{
	mFaceOrientation = pFO;
}


void Software3DPainter::SetZTestEnabled(bool pEnabled)
{
	SetFlag(FLAG_ZTEST_ACTIVE, pEnabled);
	mStateChanged = true;
}

void Software3DPainter::SetZWriteEnabled(bool pEnabled)
{
	SetFlag(FLAG_ZWRITE_ACTIVE, pEnabled);
	mStateChanged = true;
}

void Software3DPainter::SetBilinearFilteringEnabled(bool pEnabled)
{
	SetFlag(FLAG_BILINEAR_ACTIVE, pEnabled);
	mStateChanged = true;
}

void Software3DPainter::SetTrilinearFilteringEnabled(bool pEnabled)
{
	SetFlag(FLAG_TRILINEAR_ACTIVE, pEnabled);
	mStateChanged = true;
}

void Software3DPainter::SetAlphaTestEnabled(bool pEnabled)
{
	SetFlag(FLAG_ALPHATEST_ACTIVE, pEnabled);
	mStateChanged = true;
}

void Software3DPainter::SetAlphaBlendEnabled(bool pEnabled)
{
	SetFlag(FLAG_ALPHABLEND_ACTIVE, pEnabled);
	mStateChanged = true;
}

void Software3DPainter::SetVertexColorsEnabled(bool pEnabled)
{
	SetFlag(FLAG_VERTEXCOLORS_ACTIVE, pEnabled);
	mStateChanged = true;
}

void Software3DPainter::SetTextureMappingEnabled(bool pEnabled)
{
	SetFlag(FLAG_TEXTUREMAPPING_ACTIVE, pEnabled);
	mStateChanged = true;
}

void Software3DPainter::SetMipMappingEnabled(bool pEnabled)
{
	SetFlag(FLAG_MIPMAPPING_ACTIVE, pEnabled);
//	mStateChanged = true;
}


void Software3DPainter::SetLightsEnabled(bool pEnabled)
{
	SetFlag(FLAG_LIGHTS_ACTIVE, pEnabled);
}

void Software3DPainter::SetRenderingHint(RenderingHint pHint)
{
	mRenderingHint = pHint;
	mStateChanged = true;
}




void Software3DPainter::SetDestCanvas(Lepra::Canvas* pScreen, bool pReset)
{
	mScreen = pScreen;

	mPainter.SetDestCanvas(mScreen);

	if (pReset == true)
	{
		SetViewport(0, 0, mScreen->GetWidth(), mScreen->GetHeight());

		if (mScreen->GetBitDepth() == Lepra::Canvas::BITDEPTH_8_BIT)
		{
			// Precalculate lookup table for lightlevels of every color.
			// This scale will go from totally black to the palette's regular colors.

			if (m8BitLightBuffer == 0)
			{
				m8BitLightBuffer = new unsigned char[256 * 256];
			}

			int lRedLUT[256];
			int lGreenLUT[256];
			int lBlueLUT[256];
			for (int i = 0; i < 256; i++)
			{
				lRedLUT[i]   = Lepra::GammaLookup::GammaToLinear(mScreen->GetPalette()[i].mRed);
				lGreenLUT[i] = Lepra::GammaLookup::GammaToLinear(mScreen->GetPalette()[i].mGreen);
				lBlueLUT[i]  = Lepra::GammaLookup::GammaToLinear(mScreen->GetPalette()[i].mBlue);
			}

			for (int lLight = 0; lLight < 256; lLight++)
			{
				for (int lColor = 0; lColor < 256; lColor++)
				{
					int lRed   = lRedLUT[lColor]   - ((255 - lLight) << 8);
					int lGreen = lGreenLUT[lColor] - ((255 - lLight) << 8);
					int lBlue  = lBlueLUT[lColor]  - ((255 - lLight) << 8);

					if (lRed < 0)
						lRed = 0;
					if (lGreen < 0)
						lGreen = 0;
					if (lBlue < 0)
						lBlue = 0;
					
					// Now find the color in the palette that matches this color best.
					int lBestMatch = 0;
					float lMinError = 3e5;
					for (int i = 0; i < 256; i++)
					{
						float lRDiff = (float)(lRed   - lRedLUT[i]);
						float lGDiff = (float)(lGreen - lGreenLUT[i]);
						float lBDiff = (float)(lBlue  - lBlueLUT[i]);
						float lError = lRDiff * lRDiff + lGDiff * lGDiff + lBDiff * lBDiff;

						if (i == 0 || lError < lMinError)
						{
							lMinError = lError;
							lBestMatch = i;
						}
					}

					m8BitLightBuffer[(lLight << 8) + lColor] = (Lepra::uint8)lBestMatch;
				}
			}
		}
		else
		{
			if (m8BitLightBuffer != 0)
			{
				delete m8BitLightBuffer;
				m8BitLightBuffer = 0;
			}
		}
	}

	mStateChanged = true;
}




void Software3DPainter::SetClearColor(const Lepra::Color& pColor)
{
	mPainter.SetColor(pColor);
}




void Software3DPainter::SetMipMapReferenceFactor(float pReferenceFactor)
{
	mMipMapReferenceFactor = pReferenceFactor;
}




void Software3DPainter::SetSpecularity(float pSpecularity)
{
	//mSpecularity = Lepra::FixedPointMath::FloatToFixed16_16(pSpecularity) & 0xFFFF;
	mSpecularity = Lepra::FixedPointMath::FloatToFixed16_16(pow(pSpecularity, 2.2f)) & 0xFFFF;
}




void Software3DPainter::SetViewport(int pLeft, int pTop, int pRight, int pBottom)
{
	if (pLeft > pRight)
	{
		int lT = pLeft;
		pLeft = pRight;
		pRight = lT;
	}

	if (pTop > pBottom)
	{
		int lT = pTop;
		pTop = pBottom;
		pBottom = lT;
	}

	mViewportLeft   = (float)pLeft;
	mViewportTop    = (float)pTop;
	mViewportRight  = (float)pRight;
	mViewportBottom = (float)pBottom;

	mPerspectiveCenterX = mViewportLeft + (mViewportRight - mViewportLeft) / 2.0f;
	mPerspectiveCenterY = mViewportTop  + (mViewportBottom - mViewportTop) / 2.0f;

	SetClippingRect(pLeft, pTop, pRight, pBottom);

	// Use the clipping rect to get the correct dimensions.
	mZBufferX = (int)mClipLeft;
	mZBufferY = (int)mClipTop;

	unsigned int lZWidth = (unsigned int)mClipRight - mZBufferX + 1;
	unsigned int lZHeight = (unsigned int)mClipBottom - mZBufferY + 1;

	// Create new Z-Buffer.
	if (mZBuffer == 0 ||
	   mZBuffer->GetWidth() != lZWidth ||
	   mZBuffer->GetHeight() != lZHeight)
	{
		if (mZBuffer != 0)
		{
			delete mZBuffer;
			mZBuffer = 0;
		}

		mZBuffer = new Lepra::Canvas(lZWidth, lZHeight, Lepra::Canvas::BITDEPTH_32_BIT);
		mZBuffer->CreateBuffer();

		ClearZBuffer();
	}
}

void Software3DPainter::GetViewport(int& pLeft, int& pTop, int& pRight, int& pBottom)
{
	pLeft   = (int)mViewportLeft;
	pTop    = (int)mViewportTop;
	pRight  = (int)mViewportRight;
	pBottom = (int)mViewportBottom;
}

void Software3DPainter::SetClippingRect(int pLeft, int pTop, int pRight, int pBottom)
{
	if (pLeft > pRight)
	{
		int lT = pLeft;
		pLeft = pRight;
		pRight = lT;
	}

	if (pTop > pBottom)
	{
		int lT = pTop;
		pTop = pBottom;
		pBottom = lT;
	}

	// First check boundaries against the screen.
	if (mScreen != 0)
	{
		if (pLeft < 0)
		{
			pLeft = 0;
		}
		if (pTop < 0)
		{
			pTop = 0;
		}
		if (pRight < 0)
		{
			pRight = 0;
		}
		if (pBottom < 0)
		{
			pBottom = 0;
		}
		if (pLeft >= (int)mScreen->GetWidth())
		{
			pLeft = mScreen->GetWidth() - 1;
		}
		if (pTop >= (int)mScreen->GetHeight())
		{
			pTop = mScreen->GetHeight() - 1;
		}
		if (pRight >= (int)mScreen->GetWidth())
		{
			pRight = mScreen->GetWidth() - 1;
		}
		if (pBottom >= (int)mScreen->GetHeight())
		{
			pBottom = mScreen->GetHeight() - 1;
		}
	}

	// Then check against the viewport.
	if (pLeft < mViewportLeft)
	{
		pLeft = (int)mViewportLeft;
	}
	if (pTop < mViewportTop)
	{
		pTop = (int)mViewportTop;
	}
	if (pRight < mViewportLeft)
	{
		pRight = (int)mViewportLeft;
	}
	if (pBottom < mViewportTop)
	{
		pBottom = (int)mViewportTop;
	}
	if (pLeft >= mViewportRight)
	{
		pLeft = (int)mViewportRight - 1;
	}
	if (pTop >= mViewportBottom)
	{
		pTop = (int)mViewportBottom - 1;
	}
	if (pRight >= mViewportRight)
	{
		pRight = (int)mViewportRight - 1;
	}
	if (pBottom >= mViewportBottom)
	{
		pBottom = (int)mViewportBottom - 1;
	}

	mClipLeft   = (float)pLeft;
	mClipTop    = (float)pTop;
	mClipRight  = (float)pRight;
	mClipBottom = (float)pBottom;

	Edge::SetClipLeftRight((int)mClipLeft, (int)mClipRight + 1);
}




void Software3DPainter::ResetClippingRect()
{
	SetClippingRect((int)mViewportLeft,
			(int)mViewportRight,
			(int)mViewportTop,
			(int)mViewportBottom);
}




void Software3DPainter::SetDefaultTransform(const Lepra::TransformationF& pTransform)
{
	mDefaultOrientation = pTransform.GetOrientationAsMatrix();
	mDefaultTranslation = pTransform.GetPosition();
}

void Software3DPainter::SetDefaultTransform(const Lepra::RotationMatrixF& pOrientation,
					    const Lepra::Vector3DF& pPosition)
{
	mDefaultOrientation = pOrientation;
	mDefaultTranslation = pPosition;
}




void Software3DPainter::SetViewFrustum(float pFOVAngle, float pNearZ, float pFarZ)
{
	mNearZ = (float)fabs(pNearZ);
	mFarZ = (float)fabs(pFarZ);

	const float lEpsilon = 1e-6f;	// Very close to 0.

	mOrthographicProjection = false;

	if (pFOVAngle < lEpsilon)
	{
		mScreenDistToOrigin = 1.0f;
		mOrthographicProjection = true;
		return;
	}
	else if(pFOVAngle > (180.0f - lEpsilon))
	{
		pFOVAngle = (180.0f - lEpsilon);
	}

	mFOVAngle = pFOVAngle;

	// Convert to radians.
	pFOVAngle = (pFOVAngle / 180.0f) * Lepra::PIF;

	mScreenDistToOrigin = (float)(1.0 / tan(pFOVAngle / 2.0f)) * 0.5f;
}

void Software3DPainter::GetViewFrustum(float& pFOVAngle, float& pNearZ, float& pFarZ)
{
	pFOVAngle = mFOVAngle;
	pNearZ = mNearZ;
	pFarZ = mFarZ;
}

void Software3DPainter::ClearZBuffer()
{
	if (mZBuffer != 0)
	{
		float* lZBuffer = (float*)mZBuffer->GetBuffer();
		const int lLength = mZBuffer->GetWidth() * mZBuffer->GetHeight();
		for (int i = lLength; i > 0; i--)
		{
			*lZBuffer++ = 0.0f;
		}
	}
}




void Software3DPainter::SetTexture(Texture* pTexture)
{
	mTexture = pTexture;
	
	mStateChanged = true;
}






void Software3DPainter::CheckStateChanges()
{
	if (mStateChanged == true)
	{
		UpdateScanLineFuncs();
		UpdateTextureReaderFuncs();
		UpdatePixelWriterFuncs();
		UpdateDrawLineFuncs();

		mStateChanged = false;
	}
}




void Software3DPainter::UpdateRenderPrimitiveFunc()
{
	if (CheckFlag(FLAG_VERTEXCOLORS_ACTIVE) == true)
	{
		if (CheckFlag(FLAG_TEXTUREMAPPING_ACTIVE) == true)
		{
			if (CheckFlag(FLAG_MIPMAPPING_ACTIVE) == true ||
			   CheckFlag(FLAG_TRILINEAR_ACTIVE) == true)
			{
				switch(mCurrentPrimitive)
				{
				case RENDER_LINES:
					RenderPrimitive = RenderLinesUVRGB;
					break;
				case RENDER_LINE_STRIP:
					RenderPrimitive = RenderLineStripUVRGB;
					break;
				case RENDER_TRIANGLES:
					RenderPrimitive = RenderTrianglesUVRGBM;
					break;
				case RENDER_TRIANGLE_STRIP:
					RenderPrimitive = RenderTriangleStripUVRGBM;
					break;
				case RENDER_TRIANGLE_FAN:
					RenderPrimitive = RenderTriangleFanUVRGBM;
					break;
				};
			}
			else
			{
				switch(mCurrentPrimitive)
				{
				case RENDER_LINES:
					RenderPrimitive = RenderLinesUVRGB;
					break;
				case RENDER_LINE_STRIP:
					RenderPrimitive = RenderLineStripUVRGB;
					break;
				case RENDER_TRIANGLES:
					RenderPrimitive = RenderTrianglesUVRGB;
					break;
				case RENDER_TRIANGLE_STRIP:
					RenderPrimitive = RenderTriangleStripUVRGB;
					break;
				case RENDER_TRIANGLE_FAN:
					RenderPrimitive = RenderTriangleFanUVRGB;
					break;
				};
			}
		}
		else
		{
			switch(mCurrentPrimitive)
			{
			case RENDER_LINES:
				RenderPrimitive = RenderLinesRGB;
				break;
			case RENDER_LINE_STRIP:
				RenderPrimitive = RenderLineStripRGB;
				break;
			case RENDER_TRIANGLES:
				RenderPrimitive = RenderTrianglesRGB;
				break;
			case RENDER_TRIANGLE_STRIP:
				RenderPrimitive = RenderTriangleStripRGB;
				break;
			case RENDER_TRIANGLE_FAN:
				RenderPrimitive = RenderTriangleFanRGB;
				break;
			};
		}
	}
	else
	{
		if (CheckFlag(FLAG_TEXTUREMAPPING_ACTIVE) == true)
		{
			if (CheckFlag(FLAG_MIPMAPPING_ACTIVE) == true ||
			   CheckFlag(FLAG_TRILINEAR_ACTIVE) == true)
			{
				switch(mCurrentPrimitive)
				{
				case RENDER_LINES:
					RenderPrimitive = RenderLinesUV;
					break;
				case RENDER_LINE_STRIP:
					RenderPrimitive = RenderLineStripUV;
					break;
				case RENDER_TRIANGLES:
					RenderPrimitive = RenderTrianglesUVM;
					break;
				case RENDER_TRIANGLE_STRIP:
					RenderPrimitive = RenderTriangleStripUVM;
					break;
				case RENDER_TRIANGLE_FAN:
					RenderPrimitive = RenderTriangleFanUVM;
					break;
				};
			}
			else
			{
				switch(mCurrentPrimitive)
				{
				case RENDER_LINES:
					RenderPrimitive = RenderLinesUV;
					break;
				case RENDER_LINE_STRIP:
					RenderPrimitive = RenderLineStripUV;
					break;
				case RENDER_TRIANGLES:
					RenderPrimitive = RenderTrianglesUV;
					break;
				case RENDER_TRIANGLE_STRIP:
					RenderPrimitive = RenderTriangleStripUV;
					break;
				case RENDER_TRIANGLE_FAN:
					RenderPrimitive = RenderTriangleFanUV;
					break;
				};
			}
		}
		else
		{
			switch(mCurrentPrimitive)
			{
			case RENDER_LINES:
				RenderPrimitive = RenderLinesSingleColor;
				break;
			case RENDER_LINE_STRIP:
				RenderPrimitive = RenderLineStripSingleColor;
				break;
			case RENDER_TRIANGLES:
				RenderPrimitive = RenderTrianglesSingleColor;
				break;
			case RENDER_TRIANGLE_STRIP:
				RenderPrimitive = RenderTriangleStripSingleColor;
				break;
			case RENDER_TRIANGLE_FAN:
				RenderPrimitive = RenderTriangleFanSingleColor;
				break;
			};
		}
	}
}




void Software3DPainter::UpdateScanLineFuncs()
{
	if (mScreen == 0)
	{
		return;	
	}

	// Always default to the "nice" functions.
	if (mScreen->GetBitDepth() == Lepra::Canvas::BITDEPTH_8_BIT )
	{
		DrawSingleColorScanLineFunc	= DrawScanLineSingleColor8BitNicest;
		DrawUVScanLineFunc			= DrawScanLineUV8BitNicest;
		DrawUVScanLineFuncFast		= DrawScanLineUV8BitFast;
		DrawRGBScanLineFunc			= DrawScanLineRGB8BitNicest;
		DrawUVRGBScanLineFunc		= DrawScanLineUVRGB8BitNicest;
		DrawUVRGBScanLineFuncFast	= DrawScanLineUVRGB8BitFast;
		DrawUVMScanLineFunc			= DrawScanLineUVM8BitNicest;
		DrawUVRGBMScanLineFunc		= DrawScanLineUVRGBM8BitNicest;
	}
	else
	{
		DrawSingleColorScanLineFunc	= DrawScanLineSingleColorNicest;
		DrawUVScanLineFunc			= DrawScanLineUVNicest;
		DrawUVScanLineFuncFast		= DrawScanLineUV8BitFast;
		DrawRGBScanLineFunc			= DrawScanLineRGBNicest;
		DrawUVRGBScanLineFunc		= DrawScanLineUVRGBNicest;
		DrawUVRGBScanLineFuncFast	= DrawScanLineUVRGB8BitFast;
		DrawUVMScanLineFunc			= DrawScanLineUVMNicest;
		DrawUVRGBMScanLineFunc		= DrawScanLineUVRGBMNicest;
	}

	switch(mRenderingHint)
	{
	case RENDER_NICEST:
		break;
	case RENDER_FAST:

		if (CheckFlag(FLAG_ZTEST_ACTIVE) == true)
		{
			if (CheckFlag(FLAG_ZWRITE_ACTIVE) == true)
			{
				switch(mScreen->GetBitDepth())
				{
				case Lepra::Canvas::BITDEPTH_15_BIT:
					DrawSingleColorScanLineFunc	= DrawScanLineSingleColor15BitFastZTZW;
					DrawRGBScanLineFunc			= DrawScanLineRGB15BitFastZTZW;
					DrawUVScanLineFuncFast		= DrawScanLineUV16BitFastZTZW;
					DrawUVRGBScanLineFuncFast	= DrawScanLineUVRGB15BitFastZTZW;
					break;
				case Lepra::Canvas::BITDEPTH_16_BIT:
					DrawSingleColorScanLineFunc	= DrawScanLineSingleColor16BitFastZTZW;
					DrawRGBScanLineFunc			= DrawScanLineRGB16BitFastZTZW;
					DrawUVScanLineFuncFast		= DrawScanLineUV16BitFastZTZW;
					DrawUVRGBScanLineFuncFast	= DrawScanLineUVRGB16BitFastZTZW;
					break;
				case Lepra::Canvas::BITDEPTH_24_BIT:
					DrawSingleColorScanLineFunc	= DrawScanLineSingleColor24BitFastZTZW;
					DrawRGBScanLineFunc			= DrawScanLineRGB24BitFastZTZW;
					DrawUVScanLineFuncFast		= DrawScanLineUV24BitFastZTZW;
					DrawUVRGBScanLineFuncFast	= DrawScanLineUVRGB24BitFastZTZW;
					break;
				case Lepra::Canvas::BITDEPTH_32_BIT:
					DrawSingleColorScanLineFunc	= DrawScanLineSingleColor32BitFastZTZW;
					DrawRGBScanLineFunc			= DrawScanLineRGB32BitFastZTZW;
					DrawUVScanLineFuncFast		= DrawScanLineUV32BitFastZTZW;
					DrawUVRGBScanLineFuncFast	= DrawScanLineUVRGB32BitFastZTZW;
					break;
				case Lepra::Canvas::BITDEPTH_8_BIT:
				default:
					DrawSingleColorScanLineFunc	= DrawScanLineSingleColor8BitFastZTZW;
					DrawRGBScanLineFunc			= DrawScanLineRGB8BitFastZTZW;
					DrawUVScanLineFuncFast		= DrawScanLineUV8BitFastZTZW;
					DrawUVRGBScanLineFuncFast	= DrawScanLineUVRGB8BitFastZTZW;
					break;
				};
			}
			else
			{
				switch(mScreen->GetBitDepth())
				{
				case Lepra::Canvas::BITDEPTH_15_BIT:
					DrawSingleColorScanLineFunc	= DrawScanLineSingleColor15BitFastZT;
					DrawRGBScanLineFunc			= DrawScanLineRGB15BitFastZT;
					DrawUVScanLineFuncFast		= DrawScanLineUV16BitFastZT;
					DrawUVRGBScanLineFuncFast	= DrawScanLineUVRGB15BitFastZT;
					break;
				case Lepra::Canvas::BITDEPTH_16_BIT:
					DrawSingleColorScanLineFunc	= DrawScanLineSingleColor16BitFastZT;
					DrawRGBScanLineFunc			= DrawScanLineRGB16BitFastZT;
					DrawUVScanLineFuncFast		= DrawScanLineUV16BitFastZT;
					DrawUVRGBScanLineFuncFast	= DrawScanLineUVRGB16BitFastZT;
					break;
				case Lepra::Canvas::BITDEPTH_24_BIT:
					DrawSingleColorScanLineFunc	= DrawScanLineSingleColor24BitFastZT;
					DrawRGBScanLineFunc			= DrawScanLineRGB24BitFastZT;
					DrawUVScanLineFuncFast		= DrawScanLineUV24BitFastZT;
					DrawUVRGBScanLineFuncFast	= DrawScanLineUVRGB24BitFastZT;
					break;
				case Lepra::Canvas::BITDEPTH_32_BIT:
					DrawSingleColorScanLineFunc	= DrawScanLineSingleColor32BitFastZT;
					DrawRGBScanLineFunc			= DrawScanLineRGB32BitFastZT;
					DrawUVScanLineFuncFast		= DrawScanLineUV32BitFastZT;
					DrawUVRGBScanLineFuncFast	= DrawScanLineUVRGB32BitFastZT;
					break;
				case Lepra::Canvas::BITDEPTH_8_BIT:
				default:
					DrawSingleColorScanLineFunc	= DrawScanLineSingleColor8BitFastZT;
					DrawRGBScanLineFunc			= DrawScanLineRGB8BitFastZT;
					DrawUVScanLineFuncFast		= DrawScanLineUV8BitFastZT;
					DrawUVRGBScanLineFuncFast	= DrawScanLineUVRGB8BitFastZT;
					break;
				};
			}
		}
		else
		{
			if (CheckFlag(FLAG_ZWRITE_ACTIVE) == true)
			{
				switch(mScreen->GetBitDepth())
				{
				case Lepra::Canvas::BITDEPTH_15_BIT:
					DrawSingleColorScanLineFunc	= DrawScanLineSingleColor15BitFastZW;
					DrawRGBScanLineFunc			= DrawScanLineRGB15BitFastZW;
					DrawUVScanLineFuncFast		= DrawScanLineUV16BitFastZW;
					DrawUVRGBScanLineFuncFast	= DrawScanLineUVRGB15BitFastZW;
					break;
				case Lepra::Canvas::BITDEPTH_16_BIT:
					DrawSingleColorScanLineFunc	= DrawScanLineSingleColor16BitFastZW;
					DrawRGBScanLineFunc			= DrawScanLineRGB16BitFastZW;
					DrawUVScanLineFuncFast		= DrawScanLineUV16BitFastZW;
					DrawUVRGBScanLineFuncFast	= DrawScanLineUVRGB16BitFastZW;
					break;
				case Lepra::Canvas::BITDEPTH_24_BIT:
					DrawSingleColorScanLineFunc	= DrawScanLineSingleColor24BitFastZW;
					DrawRGBScanLineFunc			= DrawScanLineRGB24BitFastZW;
					DrawUVScanLineFuncFast		= DrawScanLineUV24BitFastZW;
					DrawUVRGBScanLineFuncFast	= DrawScanLineUVRGB24BitFastZW;
					break;
				case Lepra::Canvas::BITDEPTH_32_BIT:
					DrawSingleColorScanLineFunc	= DrawScanLineSingleColor32BitFastZW;
					DrawRGBScanLineFunc			= DrawScanLineRGB32BitFastZW;
					DrawUVScanLineFuncFast		= DrawScanLineUV32BitFastZW;
					DrawUVRGBScanLineFuncFast	= DrawScanLineUVRGB32BitFastZW;
					break;
				case Lepra::Canvas::BITDEPTH_8_BIT:
				default:
					DrawSingleColorScanLineFunc	= DrawScanLineSingleColor8BitFastZW;
					DrawRGBScanLineFunc			= DrawScanLineRGB8BitFastZW;
					DrawUVScanLineFuncFast		= DrawScanLineUV8BitFastZW;
					DrawUVRGBScanLineFuncFast	= DrawScanLineUVRGB8BitFastZW;
					break;
				}
			}
			else
			{
				switch(mScreen->GetBitDepth())
				{
				case Lepra::Canvas::BITDEPTH_15_BIT:
					DrawSingleColorScanLineFunc	= DrawScanLineSingleColor15BitFast;
					DrawRGBScanLineFunc			= DrawScanLineRGB15BitFast;
					DrawUVScanLineFuncFast		= DrawScanLineUV16BitFast;
					DrawUVRGBScanLineFuncFast	= DrawScanLineUVRGB15BitFast;
					break;
				case Lepra::Canvas::BITDEPTH_16_BIT:
					DrawSingleColorScanLineFunc	= DrawScanLineSingleColor16BitFast;
					DrawRGBScanLineFunc			= DrawScanLineRGB16BitFast;
					DrawUVScanLineFuncFast		= DrawScanLineUV16BitFast;
					DrawUVRGBScanLineFuncFast	= DrawScanLineUVRGB16BitFast;
					break;
				case Lepra::Canvas::BITDEPTH_24_BIT:
					DrawSingleColorScanLineFunc	= DrawScanLineSingleColor24BitFast;
					DrawRGBScanLineFunc			= DrawScanLineRGB24BitFast;
					DrawUVScanLineFuncFast		= DrawScanLineUV24BitFast;
					DrawUVRGBScanLineFuncFast	= DrawScanLineUVRGB24BitFast;
					break;
				case Lepra::Canvas::BITDEPTH_32_BIT:
					DrawSingleColorScanLineFunc	= DrawScanLineSingleColor32BitFast;
					DrawRGBScanLineFunc			= DrawScanLineRGB32BitFast;
					DrawUVScanLineFuncFast		= DrawScanLineUV32BitFast;
					DrawUVRGBScanLineFuncFast	= DrawScanLineUVRGB32BitFast;
					break;
				case Lepra::Canvas::BITDEPTH_8_BIT:
				default:
					DrawSingleColorScanLineFunc	= DrawScanLineSingleColor8BitFast;
					DrawRGBScanLineFunc			= DrawScanLineRGB8BitFast;
					DrawUVScanLineFuncFast		= DrawScanLineUV8BitFast;
					DrawUVRGBScanLineFuncFast	= DrawScanLineUVRGB8BitFast;
					break;
				}
			}
		}

		break;
	}

}




void Software3DPainter::UpdateTextureReaderFuncs()
{
	if (mTexture == 0)
	{
		GetTextureColor = GetTextureColor8BitStandard;
		return;
	}

	if (CheckFlag(FLAG_TRILINEAR_ACTIVE) == true)
	{
		switch(mTexture->GetColorMap(0)->GetBitDepth())
		{
		case Lepra::Canvas::BITDEPTH_15_BIT:
			GetTextureColor = GetTextureColor15BitTrilinear;
			break;
		case Lepra::Canvas::BITDEPTH_16_BIT:
			GetTextureColor = GetTextureColor16BitTrilinear;
			break;
		case Lepra::Canvas::BITDEPTH_24_BIT:
			GetTextureColor = GetTextureColor24BitTrilinear;
			break;
		case Lepra::Canvas::BITDEPTH_32_BIT:
			GetTextureColor = GetTextureColor32BitTrilinear;
			break;
		case Lepra::Canvas::BITDEPTH_8_BIT:
		default:
			GetTextureColor = GetTextureColor8BitStandard;
			break;
		}
	}
	else
	{
		if (CheckFlag(FLAG_MIPMAPPING_ACTIVE) == true)
		{
			if (CheckFlag(FLAG_BILINEAR_ACTIVE) == true)
			{
				switch(mTexture->GetColorMap(0)->GetBitDepth())
				{
				case Lepra::Canvas::BITDEPTH_15_BIT:
					GetTextureColor = GetTextureColor15BitBilinear;
					break;
				case Lepra::Canvas::BITDEPTH_16_BIT:
					GetTextureColor = GetTextureColor16BitBilinear;
					break;
				case Lepra::Canvas::BITDEPTH_24_BIT:
					GetTextureColor = GetTextureColor24BitBilinear;
					break;
				case Lepra::Canvas::BITDEPTH_32_BIT:
					GetTextureColor = GetTextureColor32BitBilinear;
					break;
				case Lepra::Canvas::BITDEPTH_8_BIT:
				default:
					GetTextureColor = GetTextureColor8BitStandard;
					break;
				}
			}
			else
			{
				switch(mTexture->GetColorMap(0)->GetBitDepth())
				{
				case Lepra::Canvas::BITDEPTH_15_BIT:
					GetTextureColor = GetTextureColor15BitStandard;
					break;
				case Lepra::Canvas::BITDEPTH_16_BIT:
					GetTextureColor = GetTextureColor16BitStandard;
					break;
				case Lepra::Canvas::BITDEPTH_24_BIT:
					GetTextureColor = GetTextureColor24BitStandard;
					break;
				case Lepra::Canvas::BITDEPTH_32_BIT:
					GetTextureColor = GetTextureColor32BitStandard;
					break;
				case Lepra::Canvas::BITDEPTH_8_BIT:
				default:
					GetTextureColor = GetTextureColor8BitStandard;
					break;
				}
			}
		}
		else
		{
			if (CheckFlag(FLAG_BILINEAR_ACTIVE) == true)
			{
				switch(mTexture->GetColorMap(0)->GetBitDepth())
				{
				case Lepra::Canvas::BITDEPTH_15_BIT:
					GetTextureColor = GetTextureColor15BitBilinear;
					break;
				case Lepra::Canvas::BITDEPTH_16_BIT:
					GetTextureColor = GetTextureColor16BitBilinear;
					break;
				case Lepra::Canvas::BITDEPTH_24_BIT:
					GetTextureColor = GetTextureColor24BitBilinear;
					break;
				case Lepra::Canvas::BITDEPTH_32_BIT:
					GetTextureColor = GetTextureColor32BitBilinear;
					break;
				case Lepra::Canvas::BITDEPTH_8_BIT:
				default:
					GetTextureColor = GetTextureColor8BitStandard;
					break;
				}
			}
			else
			{
				switch(mTexture->GetColorMap(0)->GetBitDepth())
				{
				case Lepra::Canvas::BITDEPTH_15_BIT:
					GetTextureColor = GetTextureColor15BitStandard;
					break;
				case Lepra::Canvas::BITDEPTH_16_BIT:
					GetTextureColor = GetTextureColor16BitStandard;
					break;
				case Lepra::Canvas::BITDEPTH_24_BIT:
					GetTextureColor = GetTextureColor24BitStandard;
					break;
				case Lepra::Canvas::BITDEPTH_32_BIT:
					GetTextureColor = GetTextureColor32BitStandard;
					break;
				case Lepra::Canvas::BITDEPTH_8_BIT:
				default:
					GetTextureColor = GetTextureColor8BitStandard;
					break;
				}
			}
		}
	}
}




void Software3DPainter::UpdatePixelWriterFuncs()
{
	if (mScreen == 0)
	{
		Write8BitColor  = Write8BitColorStandard;
		WriteColor = Write15BitColorStandard;
		return;
	}

	if (CheckFlag(FLAG_ALPHABLEND_ACTIVE) == true)
	{
		Write8BitColor  = Write8BitColorAlphaTest;

		switch(mScreen->GetBitDepth())
		{
		case Lepra::Canvas::BITDEPTH_15_BIT:
			WriteColor = Write15BitColorAlphaBlend;
			break;
		case Lepra::Canvas::BITDEPTH_16_BIT:
			WriteColor = Write16BitColorAlphaBlend;
			break;
		case Lepra::Canvas::BITDEPTH_24_BIT:
			WriteColor = Write24BitColorAlphaBlend;
			break;
		case Lepra::Canvas::BITDEPTH_32_BIT:
			WriteColor = Write32BitColorAlphaBlend;
			break;
		};
	}
	else if(CheckFlag(FLAG_ALPHATEST_ACTIVE) == true)
	{
		Write8BitColor  = Write8BitColorAlphaTest;

		switch(mScreen->GetBitDepth())
		{
		case Lepra::Canvas::BITDEPTH_15_BIT:
			WriteColor = Write15BitColorAlphaTest;
			break;
		case Lepra::Canvas::BITDEPTH_16_BIT:
			WriteColor = Write16BitColorAlphaTest;
			break;
		case Lepra::Canvas::BITDEPTH_24_BIT:
			WriteColor = Write24BitColorAlphaTest;
			break;
		case Lepra::Canvas::BITDEPTH_32_BIT:
			WriteColor = Write32BitColorAlphaTest;
			break;
		};
	}
	else
	{
		Write8BitColor  = Write8BitColorStandard;
		switch(mScreen->GetBitDepth())
		{
		case Lepra::Canvas::BITDEPTH_15_BIT:
			WriteColor = Write15BitColorStandard;
			break;
		case Lepra::Canvas::BITDEPTH_16_BIT:
			WriteColor = Write16BitColorStandard;
			break;
		case Lepra::Canvas::BITDEPTH_24_BIT:
			WriteColor = Write24BitColorStandard;
			break;
		case Lepra::Canvas::BITDEPTH_32_BIT:
			WriteColor = Write32BitColorStandard;
			break;
		};
	}
}




void Software3DPainter::UpdateDrawLineFuncs()
{
	if (mScreen == 0)
	{
		return;	
	}

	// Always default to the "nice" functions.
	DrawUVMLineFunc			= DrawUVMLineNicest;
	DrawUVRGBMLineFunc		= DrawUVRGBMLineNicest;

	if (mScreen->GetBitDepth() == Lepra::Canvas::BITDEPTH_8_BIT)
	{
		DrawSingleColorLineFunc = DrawSingleColorLine8BitNicest;
		DrawRGBLineFunc			= DrawRGBLine8BitNicest;
		DrawUVLineFunc			= DrawUVLine8BitNicest;
		DrawUVRGBLineFunc		= DrawUVRGBLine8BitNicest;
	}
	else
	{
		DrawSingleColorLineFunc = DrawSingleColorLineNicest;
		DrawRGBLineFunc			= DrawRGBLineNicest;
		DrawUVLineFunc			= DrawUVLineNicest;
		DrawUVRGBLineFunc		= DrawUVRGBLineNicest;
	}

	switch(mRenderingHint)
	{
	case RENDER_NICEST:
		break;
	case RENDER_FAST:
		if (CheckFlag(FLAG_ZTEST_ACTIVE) == true)
		{
			if (CheckFlag(FLAG_ZWRITE_ACTIVE) == true)
			{
				switch(mScreen->GetBitDepth())
				{
				case Lepra::Canvas::BITDEPTH_8_BIT:
					DrawSingleColorLineFunc = DrawSingleColorLine8BitFastZTZW;
					DrawRGBLineFunc			= DrawRGBLine8BitFastZTZW;
					DrawUVLineFuncFast		= DrawUVLine8BitFastZTZW;
					DrawUVRGBLineFuncFast	= DrawUVRGBLine8BitFastZTZW;
					break;
				case Lepra::Canvas::BITDEPTH_15_BIT:
					DrawSingleColorLineFunc = DrawSingleColorLine15BitFastZTZW;
					DrawRGBLineFunc			= DrawRGBLine15BitFastZTZW;
					DrawUVLineFuncFast		= DrawUVLine16BitFastZTZW;
					DrawUVRGBLineFuncFast	= DrawUVRGBLine15BitFastZTZW;
					break;
				case Lepra::Canvas::BITDEPTH_16_BIT:
					DrawSingleColorLineFunc = DrawSingleColorLine16BitFastZTZW;
					DrawRGBLineFunc			= DrawRGBLine16BitFastZTZW;
					DrawUVLineFuncFast		= DrawUVLine16BitFastZTZW;
					DrawUVRGBLineFuncFast	= DrawUVRGBLine16BitFastZTZW;
					break;
				case Lepra::Canvas::BITDEPTH_24_BIT:
				case Lepra::Canvas::BITDEPTH_32_BIT:
					DrawSingleColorLineFunc = DrawSingleColorLine32BitFastZTZW;
					DrawRGBLineFunc			= DrawRGBLine32BitFastZTZW;
					DrawUVLineFuncFast		= DrawUVLine32BitFastZTZW;
					DrawUVRGBLineFuncFast	= DrawUVRGBLine32BitFastZTZW;
					break;
				};
			} // End if(ZWRITE)
			else
			{
				switch(mScreen->GetBitDepth())
				{
				case Lepra::Canvas::BITDEPTH_8_BIT:
					DrawSingleColorLineFunc = DrawSingleColorLine8BitFastZT;
					DrawRGBLineFunc			= DrawRGBLine8BitFastZT;
					DrawUVLineFuncFast		= DrawUVLine8BitFastZT;
					DrawUVRGBLineFuncFast	= DrawUVRGBLine8BitFastZT;
					break;
				case Lepra::Canvas::BITDEPTH_15_BIT:
					DrawSingleColorLineFunc = DrawSingleColorLine15BitFastZT;
					DrawRGBLineFunc			= DrawRGBLine15BitFastZT;
					DrawUVLineFuncFast		= DrawUVLine16BitFastZT;
					DrawUVRGBLineFuncFast	= DrawUVRGBLine15BitFastZT;
					break;
				case Lepra::Canvas::BITDEPTH_16_BIT:
					DrawSingleColorLineFunc = DrawSingleColorLine16BitFastZT;
					DrawRGBLineFunc			= DrawRGBLine16BitFastZT;
					DrawUVLineFuncFast		= DrawUVLine16BitFastZT;
					DrawUVRGBLineFuncFast	= DrawUVRGBLine16BitFastZT;
					break;
				case Lepra::Canvas::BITDEPTH_24_BIT:
				case Lepra::Canvas::BITDEPTH_32_BIT:
					DrawSingleColorLineFunc = DrawSingleColorLine32BitFastZT;
					DrawRGBLineFunc			= DrawRGBLine32BitFastZT;
					DrawUVLineFuncFast		= DrawUVLine32BitFastZT;
					DrawUVRGBLineFuncFast	= DrawUVRGBLine32BitFastZT;
					break;
				};
			} // End if(!ZWRITE)
		} // End if(!ZTEST)
		else
		{
			if (CheckFlag(FLAG_ZWRITE_ACTIVE) == true)
			{
				switch(mScreen->GetBitDepth())
				{
				case Lepra::Canvas::BITDEPTH_8_BIT:
					DrawSingleColorLineFunc = DrawSingleColorLine8BitFastZW;
					DrawRGBLineFunc			= DrawRGBLine8BitFastZW;
					DrawUVLineFuncFast		= DrawUVLine8BitFastZW;
					DrawUVRGBLineFuncFast	= DrawUVRGBLine8BitFastZW;
					break;
				case Lepra::Canvas::BITDEPTH_15_BIT:
					DrawSingleColorLineFunc = DrawSingleColorLine15BitFastZW;
					DrawRGBLineFunc			= DrawRGBLine15BitFastZW;
					DrawUVLineFuncFast		= DrawUVLine16BitFastZW;
					DrawUVRGBLineFuncFast	= DrawUVRGBLine15BitFastZW;
					break;
				case Lepra::Canvas::BITDEPTH_16_BIT:
					DrawSingleColorLineFunc = DrawSingleColorLine16BitFastZW;
					DrawRGBLineFunc			= DrawRGBLine16BitFastZW;
					DrawUVLineFuncFast		= DrawUVLine16BitFastZW;
					DrawUVRGBLineFuncFast	= DrawUVRGBLine16BitFastZW;
					break;
				case Lepra::Canvas::BITDEPTH_24_BIT:
				case Lepra::Canvas::BITDEPTH_32_BIT:
					DrawSingleColorLineFunc = DrawSingleColorLine32BitFastZW;
					DrawRGBLineFunc			= DrawRGBLine32BitFastZW;
					DrawUVLineFuncFast		= DrawUVLine32BitFastZW;
					DrawUVRGBLineFuncFast	= DrawUVRGBLine32BitFastZW;
					break;
				};
			} // End if(ZWRITE)
			else
			{
				switch(mScreen->GetBitDepth())
				{
				case Lepra::Canvas::BITDEPTH_8_BIT:
					DrawSingleColorLineFunc = DrawSingleColorLine8BitFast;
					DrawRGBLineFunc			= DrawRGBLine8BitFast;
					DrawUVLineFuncFast		= DrawUVLine8BitFast;
					DrawUVRGBLineFuncFast	= DrawUVRGBLine8BitFast;
					break;
				case Lepra::Canvas::BITDEPTH_15_BIT:
					DrawSingleColorLineFunc = DrawSingleColorLine15BitFast;
					DrawRGBLineFunc			= DrawRGBLine15BitFast;
					DrawUVLineFuncFast		= DrawUVLine16BitFast;
					DrawUVRGBLineFuncFast	= DrawUVRGBLine15BitFast;
					break;
				case Lepra::Canvas::BITDEPTH_16_BIT:
					DrawSingleColorLineFunc = DrawSingleColorLine16BitFast;
					DrawRGBLineFunc			= DrawRGBLine16BitFast;
					DrawUVLineFuncFast		= DrawUVLine16BitFast;
					DrawUVRGBLineFuncFast	= DrawUVRGBLine16BitFast;
					break;
				case Lepra::Canvas::BITDEPTH_24_BIT:
				case Lepra::Canvas::BITDEPTH_32_BIT:
					DrawSingleColorLineFunc = DrawSingleColorLine32BitFast;
					DrawRGBLineFunc			= DrawRGBLine32BitFast;
					DrawUVLineFuncFast		= DrawUVLine32BitFast;
					DrawUVRGBLineFuncFast	= DrawUVRGBLine32BitFast;
					break;
				};
			} // End if(!ZWRITE)
		}
		break;
	};
}




unsigned char Software3DPainter::GetAlpha(int pM, int pPixelIndex)
{
	switch(mAlphaSource)
	{
	case ALPHA_BUFFER:
		if (mTexture->GetAlphaMap(0) != 0 &&
		   pM < mTexture->GetNumMipMapLevels())
		{
			const Lepra::Canvas* lAlphaBuffer = mTexture->GetAlphaMap(pM);
			return ((unsigned char*)lAlphaBuffer->GetBuffer())[pPixelIndex];
		}
		else
		{
			return mAlpha;
		}
		break;
	case ALPHA_CHANNEL:

		if (mTexture != 0 &&
		   mTexture->GetColorMap(0)->GetBitDepth() == 32 &&
		   pM < mTexture->GetNumMipMapLevels())
		{
			const Lepra::Canvas* lTexture = mTexture->GetColorMap(pM);
			return ((unsigned char*)lTexture->GetBuffer())[(pPixelIndex << 2) + 3];
		}
		else
		{
			return mAlpha;
		}
		break;
	case ALPHA_VALUE:
	default:
		return mAlpha;
		break;
	}
}




void Software3DPainter::SetAffineLengthExponent(unsigned int pAffineExp)
{
	mAffineExp = pAffineExp;
	mAffineLength = (1 << pAffineExp);
	mAffineLengthFloat = (float)mAffineLength;
}




void Software3DPainter::Begin(RenderingPrimitive pPrimitive)
{
	if (mIsRendering == true)
	{
		return;
	}

	CheckStateChanges();

	mCurrentXYZIndex = 0;
	mCurrentUVIndex  = 0;
	mCurrentRGBIndex = 0;
	mCurrentPrimitive = pPrimitive;

	UpdateRenderPrimitiveFunc();

	switch(mCurrentPrimitive)
	{
	case RENDER_LINES:
	case RENDER_LINE_STRIP:
		mIndexLimit = 2;
		break;
	case RENDER_TRIANGLES:
	case RENDER_TRIANGLE_STRIP:
	case RENDER_TRIANGLE_FAN:
	default:
		mIndexLimit = 3;
		break;
	};

	mIsRendering = true;
	mTriangleStripInvert = false;

	if (mTexture != 0)
	{
		int lWidth = mTexture->GetColorMap(0)->GetWidth();
		int lHeight = mTexture->GetColorMap(0)->GetHeight();
		mMipMapMidValue = (float)(lHeight + ((lWidth - lHeight) >> 1));

		mMipMapScale = (mViewportRight - mViewportLeft) * mScreenDistToOrigin *
						  mMipMapMidValue * mMipMapReferenceFactor;
	}
}




void Software3DPainter::End()
{
	RenderPrimitive(this);

	mCurrentXYZIndex = 0;
	mCurrentUVIndex  = 0;
	mCurrentRGBIndex = 0;
	mCurrentNormalIndex = 0;

	mIsRendering = false;
}



bool Software3DPainter::GetScreenCoordinates(float pX, float pY, float pZ, int& pScreenX, int& pScreenY)
{
	Lepra::Vector3DF lVector(pX, pY, pZ);

	// Inverse transform the vector.
	lVector = mCameraOrientation.GetInverseRotatedVector(lVector - mCameraPosition);

	if (lVector.z >= mNearZ)
	{
		const float lT = (mViewportRight - mViewportLeft) * mScreenDistToOrigin / lVector.z;
		pScreenX = (int)(mPerspectiveCenterX + lVector.x * lT);
		pScreenY = (int)(mPerspectiveCenterY - lVector.y * lT);

		return true;
	}
	else
	{
		pScreenX = -1;
		pScreenY = -1;

		return false;
	}
}




void Software3DPainter::Vertex(float pX, float pY, float pZ)
{
	if (mIsRendering == false)
	{
		return;
	}

	if (mCurrentXYZIndex >= mIndexLimit)
	{
		// If there is enough information to render the primitive, 
		// this function call will reduce the current index value.
		RenderPrimitive(this);
		CheckVertexData();
	}

	mPoint[mCurrentXYZIndex].mXYZ->x = pX;
	mPoint[mCurrentXYZIndex].mXYZ->y = pY;
	mPoint[mCurrentXYZIndex].mXYZ->z = pZ;

	Lepra::Vector3DF lVector(pX, pY, pZ);
	lVector = mCamSpaceOrientation.GetRotatedVector(lVector) + mCamSpaceTranslation;

	mPoint[mCurrentXYZIndex].mXYZ->mTranslatedX = lVector.x;
	mPoint[mCurrentXYZIndex].mXYZ->mTranslatedY = lVector.y;
	mPoint[mCurrentXYZIndex].mXYZ->mTranslatedZ = lVector.z;

	mCurrentXYZIndex++;
}




void Software3DPainter::UV(float pU, float pV)
{
	if (mIsRendering == false)
	{
		return;
	}

	if (mCurrentUVIndex >= mIndexLimit)
	{
		// If there is enough information to render the primitive, 
		// this function call will reduce the current index value.
		RenderPrimitive(this);
		CheckVertexData();
	}

	if(mTextureTransformIsIdentity == true)
	{
		mPoint[mCurrentUVIndex].mUVM->u = pU;
		mPoint[mCurrentUVIndex].mUVM->v = pV;
	}
	else
	{
		Lepra::Vector3DF lUVCoords(pU, pV, 0);
		lUVCoords = mTextureTransform.Transform(lUVCoords);

		mPoint[mCurrentUVIndex].mUVM->u = lUVCoords.x;
		mPoint[mCurrentUVIndex].mUVM->v = lUVCoords.y;
	}

	mCurrentUVIndex++;
}




void Software3DPainter::Color(float pRed, float pGreen, float pBlue)
{
	if (mIsRendering == false)
	{
		return;
	}

	int lIndex = 0;

	if (CheckFlag(FLAG_VERTEXCOLORS_ACTIVE) == true)
	{
		if (mCurrentRGBIndex >= mIndexLimit)
		{
			// If there is enough information to render the primitive, 
			// this function call will reduce the current index value.
			RenderPrimitive(this);
			CheckVertexData();
		}

		lIndex = mCurrentRGBIndex;
		mCurrentRGBIndex++;
	}
	else
	{
		if (mCurrentRGBIndex > 0)
		{
			RenderPrimitive(this);
			CheckVertexData();
		}

		lIndex = 0;
		mCurrentRGBIndex = 1;
	}

	mPoint[lIndex].mRGB->r = pRed;
	mPoint[lIndex].mRGB->g = pGreen;
	mPoint[lIndex].mRGB->b = pBlue;
}



void Software3DPainter::Normal(float pX, float pY, float pZ)
{
	if (mIsRendering == false)
	{
		return;
	}

	if (CheckFlag(FLAG_LIGHTS_ACTIVE) == false)
	{
		return;
	}

	int lLimit = 3;
	if (mLightModel == LIGHT_FLAT)
	{
		lLimit = 1;
	}

	if (mCurrentNormalIndex >= lLimit)
	{
		RenderPrimitive(this);
		CheckVertexData();
	}

	Lepra::Vector3DF lVector(pX, pY, pZ);
	lVector = mModelOrientation.GetRotatedVector(lVector);

	mPoint[mCurrentXYZIndex].mNormal->x = lVector.x;
	mPoint[mCurrentXYZIndex].mNormal->y = lVector.y;
	mPoint[mCurrentXYZIndex].mNormal->z = lVector.z;

	mCurrentNormalIndex++;
}




void Software3DPainter::CheckVertexData()
{
	if (mCurrentXYZIndex >= mIndexLimit)
	{
		DataXYZ* lTemp = mPoint[0].mXYZ;
		mPoint[0].mXYZ = mPoint[1].mXYZ;
		mPoint[1].mXYZ = mPoint[2].mXYZ;
		mPoint[2].mXYZ = lTemp;

		mCurrentXYZIndex--;
	}

	if (CheckFlag(FLAG_VERTEXCOLORS_ACTIVE) == true)
	{
		if (mCurrentRGBIndex >= mIndexLimit)
		{
			DataRGB* lTemp = mPoint[0].mRGB;
			mPoint[0].mRGB = mPoint[1].mRGB;
			mPoint[1].mRGB = mPoint[2].mRGB;
			mPoint[2].mRGB = lTemp;

			mCurrentRGBIndex--;
		}
	}

	if (mCurrentUVIndex >= mIndexLimit)
	{
		DataUVM* lTemp = mPoint[0].mUVM;
		mPoint[0].mUVM= mPoint[1].mUVM;
		mPoint[1].mUVM= mPoint[2].mUVM;
		mPoint[2].mUVM= lTemp;

		mCurrentUVIndex--;
	}

	if (mLightModel == LIGHT_FLAT)
	{
		mCurrentNormalIndex = 0;
	}
	else
	{
		if (mCurrentNormalIndex >= mIndexLimit)
		{
			DataXYZ* lTemp = mPoint[0].mNormal;
			mPoint[0].mNormal = mPoint[1].mNormal;
			mPoint[1].mNormal = mPoint[2].mNormal;
			mPoint[2].mNormal = lTemp;

			mCurrentNormalIndex--;
		}
	}
}




void Software3DPainter::SetLight(int pLightID,
									LightType pType,
									float pX, float pY, float pZ,
									float pRed, float pGreen, float pBlue)
{
	if (pLightID >= 0 && pLightID < MAX_LIGHTS)
	{
		if (mLights[pLightID].mType != LIGHT_NONE &&
		   pType == LIGHT_NONE)
		{
			// The light is being removed, "defrag" the indices lookup..
			for (int i = mLights[pLightID].mIndex; i < (mNumLights - 1); i++)
			{
				mLights[mLightIndices[i]].mIndex--;
				mLightIndices[i] = mLightIndices[i + 1];
			}
			mNumLights--;
		}
		if (mLights[pLightID].mType == LIGHT_NONE &&
		   pType != LIGHT_NONE)
		{
			// A new light is being added.
			mLightIndices[mNumLights] = pLightID;
			mLights[pLightID].mIndex = mNumLights;
			mNumLights++;
		}

		mLights[pLightID].mType = pType;
		mLights[pLightID].mVector.x = pX;
		mLights[pLightID].mVector.y = pY;
		mLights[pLightID].mVector.z = pZ;
		mLights[pLightID].mRed   = pRed;
		mLights[pLightID].mGreen = pGreen;
		mLights[pLightID].mBlue  = pBlue;
	}
}

void Software3DPainter::SetLightPosition(int pLightIndex, float pX, float pY, float pZ)
{
	if (pLightIndex >= 0 && pLightIndex < MAX_LIGHTS && mLights[pLightIndex].mType == LIGHT_POINT)
	{
		mLights[pLightIndex].mVector.x = pX;
		mLights[pLightIndex].mVector.y = pY;
		mLights[pLightIndex].mVector.z = pZ;
	}
}

void Software3DPainter::SetLightDirection(int pLightIndex, float pX, float pY, float pZ)
{
	if (pLightIndex >= 0 && pLightIndex < MAX_LIGHTS && mLights[pLightIndex].mType == LIGHT_DIRECTIONAL)
	{
		mLights[pLightIndex].mVector.x = pX;
		mLights[pLightIndex].mVector.y = pY;
		mLights[pLightIndex].mVector.z = pZ;
	}
}

void Software3DPainter::GetLightPosition(int pLightIndex, float& pX, float& pY, float& pZ)
{
	if (pLightIndex >= 0 && pLightIndex < MAX_LIGHTS && mLights[pLightIndex].mType == LIGHT_POINT)
	{
		pX = mLights[pLightIndex].mVector.x;
		pY = mLights[pLightIndex].mVector.y;
		pZ = mLights[pLightIndex].mVector.z;
	}
}

void Software3DPainter::GetLightDirection(int pLightIndex, float& pX, float& pY, float& pZ)
{
	if (pLightIndex >= 0 && pLightIndex < MAX_LIGHTS && mLights[pLightIndex].mType == LIGHT_DIRECTIONAL)
	{
		pX = mLights[pLightIndex].mVector.x;
		pY = mLights[pLightIndex].mVector.y;
		pZ = mLights[pLightIndex].mVector.z;
	}
}

void Software3DPainter::GetLightColor(int pLightIndex, float& pR, float& pG, float& pB)
{
	if (pLightIndex >= 0 && pLightIndex < MAX_LIGHTS)
	{
		pR = mLights[pLightIndex].mRed;
		pG = mLights[pLightIndex].mGreen;
		pB = mLights[pLightIndex].mBlue;
	}
}

void Software3DPainter::ProcessLights()
{
	int i;

	if (CheckFlag(FLAG_LIGHTS_ACTIVE) == true)
	{
		// We can handle the RGB:s as we like, since this function is called
		// from RenderPrimitive(), when we are sure there is enough data
		// to really render a triangle. The RGB:s will be cleared later anyway...

		// Make sure there are 3 color values...
		if (mCurrentRGBIndex == 0)
		{
			mPoint[0].mRGB->r = 1.0f;
			mPoint[0].mRGB->g = 1.0f;
			mPoint[0].mRGB->b = 1.0f;
			mCurrentRGBIndex++;
		}

		for (i = mCurrentRGBIndex; i < 3; i++)
		{
			mPoint[i].mRGB->r = mPoint[i-1].mRGB->r;
			mPoint[i].mRGB->g = mPoint[i-1].mRGB->g;
			mPoint[i].mRGB->b = mPoint[i-1].mRGB->b;
		}
		mCurrentRGBIndex = 3;

		// Make sure that at least the ambient light is calculated.
		for (i = 0; i < 3; i++)
		{
			mPoint[i].mRGB->mLightProcessedR = mPoint[i].mRGB->r * mAmbientRed;
			mPoint[i].mRGB->mLightProcessedG = mPoint[i].mRGB->g * mAmbientGreen;
			mPoint[i].mRGB->mLightProcessedB = mPoint[i].mRGB->b * mAmbientBlue;
		}

		if (mCurrentNormalIndex == 0)
		{
			return;
		}

		// Add all lights.
		switch(mLightModel)
		{
		case LIGHT_FLAT:
			{
				for (int j = 0; j < mNumLights; j++)
				{
					float lDotProduct = 0.0f;
					int lLightID = mLightIndices[j];

					bool lValidType = false;

					switch(mLights[lLightID].mType)
					{
					case LIGHT_DIRECTIONAL:
						{
							DataXYZ* lV = &mLights[lLightID].mVector;

							if (mCurrentNormalIndex == 1)
							{
								DataXYZ* lN = mPoint[0].mNormal;
								lDotProduct = lV->x * lN->x + lV->y * lN->y + lV->z * lN->z;
							}
							else
							{
								// Use the "emergency" function.
								lDotProduct = FlatModeDotProduct(lV);
							}

							lValidType = true;
						}
						break;
					case LIGHT_POINT:
						{
							DataXYZ* lV = &mLights[lLightID].mVector;
							DataXYZ* lP = mPoint[0].mXYZ;

							DataXYZ lDiff;
							lDiff.x = lP->x - lV->x;
							lDiff.y = lP->y - lV->y;
							lDiff.z = lP->z - lV->z;
							float lLength = (float)sqrt(lDiff.x * lDiff.x + lDiff.y * lDiff.y + lDiff.z * lDiff.z);

							if (lLength > 1e-6f)
							{
								float lOneOverLength = 1.0f / lLength;
								lDiff.x *= lOneOverLength;
								lDiff.y *= lOneOverLength;
								lDiff.z *= lOneOverLength;

								if (mCurrentNormalIndex == 1)
								{
									DataXYZ* lN = mPoint[0].mNormal;

									lDotProduct = lDiff.x * lN->x + 
											lDiff.y * lN->y + 
											lDiff.z * lN->z;
								}
								else
								{
									// Use the "emergency" function.
									lDotProduct = FlatModeDotProduct(lV);
								}

								// The light fade with the distance...
								lDotProduct /= (lLength * lLength * 0.001f);

								lValidType = true;
							}
						}
						break;
					};

					if (lValidType == true)
					{
						float lR = 0.0f;
						float lG = 0.0f;
						float lB = 0.0f;

						if (lDotProduct < 0.0f)
						{
							lR = mLights[lLightID].mRed   * -lDotProduct;
							lG = mLights[lLightID].mGreen * -lDotProduct;
							lB = mLights[lLightID].mBlue  * -lDotProduct;
						}

						for (i = 0; i < 3; i++)
						{
							DataRGB* lRGB = mPoint[i].mRGB;
							lRGB->mLightProcessedR += lRGB->r * lR;
							lRGB->mLightProcessedG += lRGB->g * lG;
							lRGB->mLightProcessedB += lRGB->b * lB;
						}
					}
				} // End for(i < MAX_LIGHTS)
			}
			break;
		case LIGHT_GOURAUD:
		case LIGHT_PHONG:
			{
				for (int j = 0; j < mNumLights; j++)
				{
					int lLightID = mLightIndices[j];

					switch(mLights[lLightID].mType)
					{
					case LIGHT_DIRECTIONAL:
						{
							DataXYZ* lV = &mLights[lLightID].mVector;

							for (int i = 0; i < 3; i++)
							{
								DataXYZ* lN = mPoint[i].mNormal;
								float lDotProduct = lV->x * lN->x + lV->y * lN->y + lV->z * lN->z;

								float lR = 0.0f;
								float lG = 0.0f;
								float lB = 0.0f;

								if (lDotProduct < 0.0f)
								{
									lR = mLights[lLightID].mRed   * -lDotProduct;
									lG = mLights[lLightID].mGreen * -lDotProduct;
									lB = mLights[lLightID].mBlue  * -lDotProduct;
								}

								DataRGB* lRGB = mPoint[i].mRGB;
								lRGB->mLightProcessedR += lRGB->r * lR;
								lRGB->mLightProcessedG += lRGB->g * lG;
								lRGB->mLightProcessedB += lRGB->b * lB;
							}
						}
						break;
					case LIGHT_POINT:
						{
							DataXYZ* lV = &mLights[lLightID].mVector;

							for (int i = 0; i < 3; i++)
							{
								DataXYZ* lN = mPoint[i].mNormal;
								DataXYZ* lP = mPoint[i].mXYZ;
								DataXYZ lDiff;
								lDiff.x = lP->x - lV->x;
								lDiff.y = lP->y - lV->y;
								lDiff.z = lP->z - lV->z;
								float lLength = (float)sqrt(lDiff.x * lDiff.x + lDiff.y * lDiff.y + lDiff.z * lDiff.z);

								if (lLength > 1e-6f)
								{
									float lOneOverLength = 1.0f / lLength;
									lDiff.x *= lOneOverLength;
									lDiff.y *= lOneOverLength;
									lDiff.z *= lOneOverLength;

									float lDotProduct = lDiff.x * lN->x + 
											      lDiff.y * lN->y + 
											      lDiff.z * lN->z;

									lDotProduct /= (lLength * lLength * 0.001f);

									float lR = 0.0f;
									float lG = 0.0f;
									float lB = 0.0f;

									if (lDotProduct < 0.0f)
									{
										lR = mLights[lLightID].mRed   * -lDotProduct;
										lG = mLights[lLightID].mGreen * -lDotProduct;
										lB = mLights[lLightID].mBlue  * -lDotProduct;
									}

									DataRGB* lRGB = mPoint[i].mRGB;
									lRGB->mLightProcessedR += lRGB->r * lR;
									lRGB->mLightProcessedG += lRGB->g * lG;
									lRGB->mLightProcessedB += lRGB->b * lB;
								}
							}
						}
						break;
					};
				} // End for(i < MAX_LIGHTS)
			}
			break;
		}; // End switch(mLightModel)

		// Clamp the color values to the interval [0, 1].
		for (i = 0; i < 3; i++)
		{
			DataRGB* lRGB = mPoint[i].mRGB;
			lRGB->mLightProcessedR = std::min(lRGB->mLightProcessedR, 1.0f);
			lRGB->mLightProcessedG = std::min(lRGB->mLightProcessedG, 1.0f);
			lRGB->mLightProcessedB = std::min(lRGB->mLightProcessedB, 1.0f);
		}
	} // End if(LIGHTS_ACTIVE)
	else
	{
		// At least copy the original rgb to the processed ones.
		for (i = 0; i < 3; i++)
		{
			mPoint[i].mRGB->mLightProcessedR = mPoint[i].mRGB->r;
			mPoint[i].mRGB->mLightProcessedG = mPoint[i].mRGB->g;
			mPoint[i].mRGB->mLightProcessedB = mPoint[i].mRGB->b;
		}
	}
}

float Software3DPainter::FlatModeDotProduct(DataXYZ* pV)
{
	// It is flat-mode, but we have three normals, as if we were
	// doing gouraud-shading. The result will look ugly if we 
	// use these normals. Therefore, calculate the normal of this 
	// triangle. 
	// 
	// This is truly slow, I know, but if the user want speed, 
	// he/she shall give us one normal (the correct normal) per
	// triangle.

	DataXYZ lT1;
	DataXYZ lT2;
	DataXYZ lN;

	lT1.x = mPoint[1].mXYZ->x - mPoint[0].mXYZ->x;
	lT1.y = mPoint[1].mXYZ->y - mPoint[0].mXYZ->y;
	lT1.z = mPoint[1].mXYZ->z - mPoint[0].mXYZ->z;

	lT2.x = mPoint[2].mXYZ->x - mPoint[0].mXYZ->x;
	lT2.y = mPoint[2].mXYZ->y - mPoint[0].mXYZ->y;
	lT2.z = mPoint[2].mXYZ->z - mPoint[0].mXYZ->z;

	lN.x = lT1.y * lT2.z - lT1.z * lT2.y;
	lN.y = lT1.z * lT2.x - lT1.x * lT2.z;
	lN.z = lT1.x * lT2.y - lT1.y * lT2.x;

	float lInvLength = 1.0f / (float)sqrt(lN.x * lN.x + lN.y * lN.y + lN.z * lN.z);
	lN.x *= lInvLength;
	lN.y *= lInvLength;
	lN.z *= lInvLength;

	return (pV->x * lN.x + pV->y * lN.y + pV->z * lN.z);
}

void Software3DPainter::SetCameraTransform(const Lepra::TransformationF& pTransform)
{
	mCameraTransformOrg = pTransform;

	mCameraOrientation = pTransform.GetOrientationAsMatrix();
	mCameraPosition    = pTransform.GetPosition();

	mNormalRotation = mCameraOrientation * mModelOrientation;

	// Calculate the camera space transformation.
	mCamSpaceOrientation.Set(mCameraOrientation);
	mCamSpaceOrientation.InvAMulB(mModelOrientation);
	mCamSpaceOrientation.BMulA(mDefaultOrientation);

	mCamSpaceTranslation = mDefaultOrientation * mCameraOrientation.GetInverseRotatedVector(mModelPosition - mCameraPosition);
}

const Lepra::TransformationF& Software3DPainter::GetCameraTransform()
{
	return mCameraTransformOrg;
}

void Software3DPainter::SetModelTransform(const Lepra::TransformationF& pTransform)
{
	mModelOrientation = pTransform.GetOrientationAsMatrix();
	mModelPosition    = pTransform.GetPosition();

	mNormalRotation = mCameraOrientation * mModelOrientation;

	// Calculate the camera space transformation.
	mCamSpaceOrientation.Set(mCameraOrientation);
	mCamSpaceOrientation.InvAMulB(mModelOrientation);
	mCamSpaceOrientation.BMulA(mDefaultOrientation);

	mCamSpaceTranslation = mDefaultOrientation * mCameraOrientation.GetInverseRotatedVector(mModelPosition - mCameraPosition);
}

void Software3DPainter::SetTextureTransform(const Lepra::TransformationF& pTransform)
{
	mTextureTransform = pTransform;
	mTextureTransformIsIdentity = (mTextureTransform == Lepra::gIdentityTransformationF);
}




void Software3DPainter::PostRenderFocalBlur(float pSharpNearZ, float pSharpFarZ)
{
	if (mScreen == 0)
	{
		return;
	}

	switch(mScreen->GetBitDepth())
	{
	case Lepra::Canvas::BITDEPTH_8_BIT:
		PostRenderFocalBlur8Bit(1.0f / pSharpNearZ, 1.0f / pSharpFarZ);
		break;
	case Lepra::Canvas::BITDEPTH_15_BIT:
		PostRenderFocalBlur15Bit(1.0f / pSharpNearZ, 1.0f / pSharpFarZ);
		break;
	case Lepra::Canvas::BITDEPTH_16_BIT:
		PostRenderFocalBlur16Bit(1.0f / pSharpNearZ, 1.0f / pSharpFarZ);
		break;
	case Lepra::Canvas::BITDEPTH_24_BIT:
		PostRenderFocalBlur24Bit(1.0f / pSharpNearZ, 1.0f / pSharpFarZ);
		break;
	case Lepra::Canvas::BITDEPTH_32_BIT:
		PostRenderFocalBlur32Bit(1.0f / pSharpNearZ, 1.0f / pSharpFarZ);
		break;
	};
}

void Software3DPainter::PostRenderFocalBlur8Bit(float pSharpNearZ, float pSharpFarZ)
{
	int x, y;

	int lLeft   = (int)mClipLeft;
	int lRight  = (int)mClipRight;
	int lTop    = (int)mClipTop;
	int lBottom = (int)mClipBottom;
	unsigned int lKK = (unsigned int)(0.9f * 65535.0f);

	if (mKBuffer == 0 ||
	   (int)mKBuffer->GetWidth() != (lRight - lLeft + 1) ||
	   (int)mKBuffer->GetHeight() != (lBottom - lTop + 1))
	{
		if (mKBuffer == 0)
		{
			mKBuffer = new Lepra::Canvas((lRight - lLeft + 1), (lBottom - lTop + 1), Lepra::Canvas::BITDEPTH_32_BIT);
		}
		else
		{
			mKBuffer->Reset((lRight - lLeft + 1), (lBottom - lTop + 1), Lepra::Canvas::BITDEPTH_32_BIT);
		}

		mKBuffer->CreateBuffer();
	}

	// Precalculate k-values.
	float* lZBuffer = (float*)mZBuffer->GetBuffer() + (lTop - mZBufferY) * mZBuffer->GetPitch() + (lLeft - mZBufferX);
	unsigned int* lKBuffer = (unsigned int*)mKBuffer->GetBuffer() + (lTop - mZBufferY) * mKBuffer->GetPitch() + (lLeft - mZBufferX);

	for (y = lTop; y <= lBottom; y++)
	{
		float* lZBuf = lZBuffer;
		unsigned int* lKBuf = lKBuffer;
		for (x = lLeft; x <= lRight; x++)
		{
			float lOneOverZ = (float)*lZBuf++;
			*lKBuf++ = (unsigned int)(CalculateBlurConstant(pSharpNearZ, pSharpFarZ, lOneOverZ) * 65535.0f);
		}

		lZBuffer += mZBuffer->GetPitch();
		lKBuffer += mKBuffer->GetPitch();
	}

	unsigned char* lScreen = (unsigned char*)mScreen->GetBuffer() + lTop * mScreen->GetPitch();
	lKBuffer = (unsigned int*)mKBuffer->GetBuffer() + ((lTop - mZBufferY) * mKBuffer->GetPitch() + (lLeft - mZBufferX));

	for (y = lTop; y <= lBottom; y++)
	{
		// Blur right...
		unsigned int lPrevRed   = 0;
		unsigned int lPrevGreen = 0;
		unsigned int lPrevBlue  = 0;
		unsigned int lPrevK = 0;

		unsigned char* lScr = lScreen + lLeft;
		unsigned int* lKBuf = lKBuffer;

		for (x = lLeft; x <= lRight; x++)
		{
			unsigned int lBlue  = 0;
			unsigned int lGreen = 0;
			unsigned int lRed   = *lScr;

			MACRO_BLUR(lRed, lGreen, lBlue, lPrevK, lKK, lPrevRed, lPrevGreen, lPrevBlue, lKBuf)

			*lScr = (unsigned char)lPrevRed;

			lKBuf++;
			lScr++;
		}

		// Blur left...
		for (x = lRight; x >= lLeft; x--)
		{
			lScr--;
			lKBuf--;

			unsigned int lBlue  = 0;
			unsigned int lGreen = 0;
			unsigned int lRed   = *lScr;

			MACRO_BLUR(lRed, lGreen, lBlue, lPrevK, lKK, lPrevRed, lPrevGreen, lPrevBlue, lKBuf)

			*lScr = (unsigned char)lPrevRed;
		}

		lScreen += mScreen->GetPitch();
		lKBuffer += mKBuffer->GetPitch();
	}

	lScreen = (unsigned char*)mScreen->GetBuffer() + lTop * mScreen->GetPitch() + lLeft;
	lKBuffer = (unsigned int*)mKBuffer->GetBuffer() + ((lTop - mZBufferY) * mKBuffer->GetPitch() + (lLeft - mZBufferX));
	for (x = lLeft; x <= lRight; x++)
	{
		// Blur down...
		unsigned int lPrevRed   = 0;
		unsigned int lPrevGreen = 0;
		unsigned int lPrevBlue  = 0;
		unsigned int lPrevK = 0;

		unsigned int lScrPitch = mScreen->GetPitch();
		unsigned int lKPitch = mKBuffer->GetPitch();

		unsigned char* lScr = lScreen;
		unsigned int* lKBuf = lKBuffer;
		for (y = lTop; y <= lBottom; y++)
		{
			unsigned int lBlue  = 0;
			unsigned int lGreen = 0;
			unsigned int lRed   = *lScr;

			MACRO_BLUR(lRed, lGreen, lBlue, lPrevK, lKK, lPrevRed, lPrevGreen, lPrevBlue, lKBuf)

			*lScr = (unsigned char)lPrevRed;

			lScr += lScrPitch;
			lKBuf += lKPitch;
		}

		for (y = lBottom; y >= lTop; y--)
		{
			lScr -= lScrPitch;
			lKBuf -= lKPitch;

			unsigned int lBlue  = 0;
			unsigned int lGreen = 0;
			unsigned int lRed   = *lScr;

			MACRO_BLUR(lRed, lGreen, lBlue, lPrevK, lKK, lPrevRed, lPrevGreen, lPrevBlue, lKBuf)

			*lScr = (unsigned char)lPrevRed;
		}


		lScreen++;
		lKBuffer++;
	}
}

void Software3DPainter::PostRenderFocalBlur15Bit(float pSharpNearZ, float pSharpFarZ)
{
	int x, y;

	int lLeft   = (int)mClipLeft;
	int lRight  = (int)mClipRight;
	int lTop    = (int)mClipTop;
	int lBottom = (int)mClipBottom;
	unsigned int lKK = (unsigned int)(0.9f * 65535.0f);

	if (mKBuffer == 0 ||
	   (int)mKBuffer->GetWidth() != (lRight - lLeft + 1) ||
	   (int)mKBuffer->GetHeight() != (lBottom - lTop + 1))
	{
		if (mKBuffer == 0)
		{
			mKBuffer = new Lepra::Canvas((lRight - lLeft + 1), (lBottom - lTop + 1), Lepra::Canvas::BITDEPTH_32_BIT);
		}
		else
		{
			mKBuffer->Reset((lRight - lLeft + 1), (lBottom - lTop + 1), Lepra::Canvas::BITDEPTH_32_BIT);
		}

		mKBuffer->CreateBuffer();
	}

	// Precalculate k-values.
	float* lZBuffer = (float*)mZBuffer->GetBuffer() + (lTop - mZBufferY) * mZBuffer->GetPitch() + (lLeft - mZBufferX);
	unsigned int* lKBuffer = (unsigned int*)mKBuffer->GetBuffer() + (lTop - mZBufferY) * mKBuffer->GetPitch() + (lLeft - mZBufferX);

	for (y = lTop; y <= lBottom; y++)
	{
		float* lZBuf = lZBuffer;
		unsigned int* lKBuf = lKBuffer;
		for (x = lLeft; x <= lRight; x++)
		{
			float lOneOverZ = (float)*lZBuf++;
			*lKBuf++ = (unsigned int)(CalculateBlurConstant(pSharpNearZ, pSharpFarZ, lOneOverZ) * 65535.0f);
		}

		lZBuffer += mZBuffer->GetPitch();
		lKBuffer += mKBuffer->GetPitch();
	}

	unsigned short* lScreen = (unsigned short*)mScreen->GetBuffer() + lTop * mScreen->GetPitch();
	lKBuffer = (unsigned int*)mKBuffer->GetBuffer() + ((lTop - mZBufferY) * mKBuffer->GetPitch() + (lLeft - mZBufferX));

	for (y = lTop; y <= lBottom; y++)
	{
		// Blur right...
		unsigned int lPrevRed   = 0;
		unsigned int lPrevGreen = 0;
		unsigned int lPrevBlue  = 0;
		unsigned int lPrevK = 0;

		unsigned short* lScr = lScreen + lLeft;
		unsigned int* lKBuf = lKBuffer;

		for (x = lLeft; x <= lRight; x++)
		{
			unsigned int lBlue  = ((*lScr) & 31) << 3;
			unsigned int lGreen = (((*lScr) >> 5) & 31) << 3;
			unsigned int lRed   = (((*lScr) >> 10) & 31) << 3;

			MACRO_BLUR(lRed, lGreen, lBlue, lPrevK, lKK, lPrevRed, lPrevGreen, lPrevBlue, lKBuf)

			*lScr = (Lepra::uint16)(((lPrevRed >> 3) << 10) | ((lPrevGreen >> 3) << 5) | (lPrevBlue >> 3));

			lKBuf++;
			lScr++;
		}

		// Blur left...
		for (x = lRight; x >= lLeft; x--)
		{
			lScr--;
			lKBuf--;

			unsigned int lBlue  = ((*lScr) & 31) << 3;
			unsigned int lGreen = (((*lScr) >> 5) & 31) << 3;
			unsigned int lRed   = (((*lScr) >> 10) & 31) << 3;

			MACRO_BLUR(lRed, lGreen, lBlue, lPrevK, lKK, lPrevRed, lPrevGreen, lPrevBlue, lKBuf)

			*lScr = (Lepra::uint16)(((lPrevRed >> 3) << 10) | ((lPrevGreen >> 3) << 5) | (lPrevBlue >> 3));
		}

		lScreen += mScreen->GetPitch();
		lKBuffer += mKBuffer->GetPitch();
	}

	lScreen = (unsigned short*)mScreen->GetBuffer() + lTop * mScreen->GetPitch() + lLeft;
	lKBuffer = (unsigned int*)mKBuffer->GetBuffer() + ((lTop - mZBufferY) * mKBuffer->GetPitch() + (lLeft - mZBufferX));
	for (x = lLeft; x <= lRight; x++)
	{
		// Blur down...
		unsigned int lPrevRed   = 0;
		unsigned int lPrevGreen = 0;
		unsigned int lPrevBlue  = 0;
		unsigned int lPrevK = 0;

		unsigned int lScrPitch = mScreen->GetPitch();
		unsigned int lKPitch = mKBuffer->GetPitch();

		unsigned short* lScr = lScreen;
		unsigned int* lKBuf = lKBuffer;
		for (y = lTop; y <= lBottom; y++)
		{
			unsigned int lBlue  = ((*lScr) & 31) << 3;
			unsigned int lGreen = (((*lScr) >> 5) & 31) << 3;
			unsigned int lRed   = (((*lScr) >> 10) & 31) << 3;

			MACRO_BLUR(lRed, lGreen, lBlue, lPrevK, lKK, lPrevRed, lPrevGreen, lPrevBlue, lKBuf)

			*lScr = (Lepra::uint16)(((lPrevRed >> 3) << 10) | ((lPrevGreen >> 3) << 5) | (lPrevBlue >> 3));

			lScr += lScrPitch;
			lKBuf += lKPitch;
		}

		for (y = lBottom; y >= lTop; y--)
		{
			lScr -= lScrPitch;
			lKBuf -= lKPitch;

			unsigned int lBlue  = ((*lScr) & 31) << 3;
			unsigned int lGreen = (((*lScr) >> 5) & 31) << 3;
			unsigned int lRed   = (((*lScr) >> 10) & 31) << 3;

			MACRO_BLUR(lRed, lGreen, lBlue, lPrevK, lKK, lPrevRed, lPrevGreen, lPrevBlue, lKBuf)

			*lScr = (Lepra::uint16)(((lPrevRed >> 3) << 10) | ((lPrevGreen >> 3) << 5) | (lPrevBlue >> 3));
		}


		lScreen++;
		lKBuffer++;
	}
}

void Software3DPainter::PostRenderFocalBlur16Bit(float pSharpNearZ, float pSharpFarZ)
{
	int x, y;

	int lLeft   = (int)mClipLeft;
	int lRight  = (int)mClipRight;
	int lTop    = (int)mClipTop;
	int lBottom = (int)mClipBottom;
	unsigned int lKK = (unsigned int)(0.9f * 65535.0f);

	if (mKBuffer == 0 ||
	   (int)mKBuffer->GetWidth() != (lRight - lLeft + 1) ||
	   (int)mKBuffer->GetHeight() != (lBottom - lTop + 1))
	{
		if (mKBuffer == 0)
		{
			mKBuffer = new Lepra::Canvas((lRight - lLeft + 1), (lBottom - lTop + 1), Lepra::Canvas::BITDEPTH_32_BIT);
		}
		else
		{
			mKBuffer->Reset((lRight - lLeft + 1), (lBottom - lTop + 1), Lepra::Canvas::BITDEPTH_32_BIT);
		}

		mKBuffer->CreateBuffer();
	}

	// Precalculate k-values.
	float* lZBuffer = (float*)mZBuffer->GetBuffer() + (lTop - mZBufferY) * mZBuffer->GetPitch() + (lLeft - mZBufferX);
	unsigned int* lKBuffer = (unsigned int*)mKBuffer->GetBuffer() + (lTop - mZBufferY) * mKBuffer->GetPitch() + (lLeft - mZBufferX);

	for (y = lTop; y <= lBottom; y++)
	{
		float* lZBuf = lZBuffer;
		unsigned int* lKBuf = lKBuffer;
		for (x = lLeft; x <= lRight; x++)
		{
			float lOneOverZ = (float)*lZBuf++;
			*lKBuf++ = (unsigned int)(CalculateBlurConstant(pSharpNearZ, pSharpFarZ, lOneOverZ) * 65535.0f);
		}

		lZBuffer += mZBuffer->GetPitch();
		lKBuffer += mKBuffer->GetPitch();
	}

	unsigned short* lScreen = (unsigned short*)mScreen->GetBuffer() + lTop * mScreen->GetPitch();
	lKBuffer = (unsigned int*)mKBuffer->GetBuffer() + ((lTop - mZBufferY) * mKBuffer->GetPitch() + (lLeft - mZBufferX));

	for (y = lTop; y <= lBottom; y++)
	{
		// Blur right...
		unsigned int lPrevRed   = 0;
		unsigned int lPrevGreen = 0;
		unsigned int lPrevBlue  = 0;
		unsigned int lPrevK = 0;

		unsigned short* lScr = lScreen + lLeft;
		unsigned int* lKBuf = lKBuffer;

		for (x = lLeft; x <= lRight; x++)
		{
			unsigned int lBlue  = ((*lScr) & 31) << 3;
			unsigned int lGreen = (((*lScr) >> 5) & 63) << 2;
			unsigned int lRed   = (((*lScr) >> 11) & 31) << 3;

			MACRO_BLUR(lRed, lGreen, lBlue, lPrevK, lKK, lPrevRed, lPrevGreen, lPrevBlue, lKBuf)

			*lScr = (Lepra::uint16)(((lPrevRed >> 3) << 11) | ((lPrevGreen >> 2) << 5) | (lPrevBlue >> 3));

			lKBuf++;
			lScr++;
		}

		// Blur left...
		for (x = lRight; x >= lLeft; x--)
		{
			lScr--;
			lKBuf--;

			unsigned int lBlue  = ((*lScr) & 31) << 3;
			unsigned int lGreen = (((*lScr) >> 5) & 63) << 2;
			unsigned int lRed   = (((*lScr) >> 11) & 31) << 3;

			MACRO_BLUR(lRed, lGreen, lBlue, lPrevK, lKK, lPrevRed, lPrevGreen, lPrevBlue, lKBuf)

			*lScr = (Lepra::uint16)(((lPrevRed >> 3) << 11) | ((lPrevGreen >> 2) << 5) | (lPrevBlue >> 3));
		}

		lScreen += mScreen->GetPitch();
		lKBuffer += mKBuffer->GetPitch();
	}

	lScreen = (unsigned short*)mScreen->GetBuffer() + lTop * mScreen->GetPitch() + lLeft;
	lKBuffer = (unsigned int*)mKBuffer->GetBuffer() + ((lTop - mZBufferY) * mKBuffer->GetPitch() + (lLeft - mZBufferX));
	for (x = lLeft; x <= lRight; x++)
	{
		// Blur down...
		unsigned int lPrevRed   = 0;
		unsigned int lPrevGreen = 0;
		unsigned int lPrevBlue  = 0;
		unsigned int lPrevK = 0;

		unsigned int lScrPitch = mScreen->GetPitch();
		unsigned int lKPitch = mKBuffer->GetPitch();

		unsigned short* lScr = lScreen;
		unsigned int* lKBuf = lKBuffer;
		for (y = lTop; y <= lBottom; y++)
		{
			unsigned int lBlue  = ((*lScr) & 31) << 3;
			unsigned int lGreen = (((*lScr) >> 5) & 63) << 2;
			unsigned int lRed   = (((*lScr) >> 11) & 31) << 3;

			MACRO_BLUR(lRed, lGreen, lBlue, lPrevK, lKK, lPrevRed, lPrevGreen, lPrevBlue, lKBuf)

			*lScr = (Lepra::uint16)(((lPrevRed >> 3) << 11) | ((lPrevGreen >> 2) << 5) | (lPrevBlue >> 3));

			lScr += lScrPitch;
			lKBuf += lKPitch;
		}

		for (y = lBottom; y >= lTop; y--)
		{
			lScr -= lScrPitch;
			lKBuf -= lKPitch;

			unsigned int lBlue  = ((*lScr) & 31) << 3;
			unsigned int lGreen = (((*lScr) >> 5) & 63) << 2;
			unsigned int lRed   = (((*lScr) >> 11) & 31) << 3;

			MACRO_BLUR(lRed, lGreen, lBlue, lPrevK, lKK, lPrevRed, lPrevGreen, lPrevBlue, lKBuf)

			*lScr = (Lepra::uint16)(((lPrevRed >> 3) << 11) | ((lPrevGreen >> 2) << 5) | (lPrevBlue >> 3));
		}


		lScreen++;
		lKBuffer++;
	}
}

void Software3DPainter::PostRenderFocalBlur24Bit(float pSharpNearZ, float pSharpFarZ)
{
	int x, y;

	int lLeft   = (int)mClipLeft;
	int lRight  = (int)mClipRight;
	int lTop    = (int)mClipTop;
	int lBottom = (int)mClipBottom;
	unsigned int lKK = (unsigned int)(0.9f * 65535.0f);

	if (mKBuffer == 0 ||
	   (int)mKBuffer->GetWidth() != (lRight - lLeft + 1) ||
	   (int)mKBuffer->GetHeight() != (lBottom - lTop + 1))
	{
		if (mKBuffer == 0)
		{
			mKBuffer = new Lepra::Canvas((lRight - lLeft + 1), (lBottom - lTop + 1), Lepra::Canvas::BITDEPTH_32_BIT);
		}
		else
		{
			mKBuffer->Reset((lRight - lLeft + 1), (lBottom - lTop + 1), Lepra::Canvas::BITDEPTH_32_BIT);
		}

		mKBuffer->CreateBuffer();
	}

	// Precalculate k-values.
	float* lZBuffer = (float*)mZBuffer->GetBuffer() + (lTop - mZBufferY) * mZBuffer->GetPitch() + (lLeft - mZBufferX);
	unsigned int* lKBuffer = (unsigned int*)mKBuffer->GetBuffer() + (lTop - mZBufferY) * mKBuffer->GetPitch() + (lLeft - mZBufferX);

	for (y = lTop; y <= lBottom; y++)
	{
		float* lZBuf = lZBuffer;
		unsigned int* lKBuf = lKBuffer;
		for (x = lLeft; x <= lRight; x++)
		{
			float lOneOverZ = (float)*lZBuf++;
			*lKBuf++ = (unsigned int)(CalculateBlurConstant(pSharpNearZ, pSharpFarZ, lOneOverZ) * 65535.0f);
		}

		lZBuffer += mZBuffer->GetPitch();
		lKBuffer += mKBuffer->GetPitch();
	}

	unsigned char* lScreen = (unsigned char*)mScreen->GetBuffer() + lTop * mScreen->GetPitch() * 3;
	lKBuffer = (unsigned int*)mKBuffer->GetBuffer() + ((lTop - mZBufferY) * mKBuffer->GetPitch() + (lLeft - mZBufferX));

	for (y = lTop; y <= lBottom; y++)
	{
		// Blur right...
		unsigned int lPrevRed   = 0;
		unsigned int lPrevGreen = 0;
		unsigned int lPrevBlue  = 0;
		unsigned int lPrevK = 0;

		unsigned char* lScr = (unsigned char*)(lScreen + lLeft * 3);
		unsigned int* lKBuf = lKBuffer;

		for (x = lLeft; x <= lRight; x++)
		{
			unsigned int lBlue  = *(lScr + 0);
			unsigned int lGreen = *(lScr + 1);
			unsigned int lRed   = *(lScr + 2);

			MACRO_BLUR(lRed, lGreen, lBlue, lPrevK, lKK, lPrevRed, lPrevGreen, lPrevBlue, lKBuf)

			*(lScr + 0) = (unsigned char)lPrevBlue;
			*(lScr + 1) = (unsigned char)lPrevGreen;
			*(lScr + 2) = (unsigned char)lPrevRed;

			lKBuf++;
			lScr += 3;
		}

		// Blur left...
		for (x = lRight; x >= lLeft; x--)
		{
			lScr -= 3;
			lKBuf--;

			unsigned int lBlue  = *(lScr + 0);
			unsigned int lGreen = *(lScr + 1);
			unsigned int lRed   = *(lScr + 2);

			MACRO_BLUR(lRed, lGreen, lBlue, lPrevK, lKK, lPrevRed, lPrevGreen, lPrevBlue, lKBuf)

			*(lScr + 0) = (unsigned char)lPrevBlue;
			*(lScr + 1) = (unsigned char)lPrevGreen;
			*(lScr + 2) = (unsigned char)lPrevRed;
		}

		lScreen += mScreen->GetPitch() * 3;
		lKBuffer += mKBuffer->GetPitch();
	}

	lScreen = (unsigned char*)mScreen->GetBuffer() + (lTop * mScreen->GetPitch() + lLeft) * 3;
	lKBuffer = (unsigned int*)mKBuffer->GetBuffer() + ((lTop - mZBufferY) * mKBuffer->GetPitch() + (lLeft - mZBufferX));
	for (x = lLeft; x <= lRight; x++)
	{
		// Blur down...
		unsigned int lPrevRed   = 0;
		unsigned int lPrevGreen = 0;
		unsigned int lPrevBlue  = 0;
		unsigned int lPrevK = 0;

		unsigned int lScrPitch = (mScreen->GetPitch() * 3);
		unsigned int lKPitch = mKBuffer->GetPitch();

		unsigned char* lScr = (unsigned char*)lScreen;
		unsigned int* lKBuf = lKBuffer;
		for (y = lTop; y <= lBottom; y++)
		{
			unsigned int lBlue  = *(lScr + 0);
			unsigned int lGreen = *(lScr + 1);
			unsigned int lRed   = *(lScr + 2);

			MACRO_BLUR(lRed, lGreen, lBlue, lPrevK, lKK, lPrevRed, lPrevGreen, lPrevBlue, lKBuf)

			*(lScr + 0) = (unsigned char)lPrevBlue;
			*(lScr + 1) = (unsigned char)lPrevGreen;
			*(lScr + 2) = (unsigned char)lPrevRed;

			lScr += lScrPitch;
			lKBuf += lKPitch;
		}

		for (y = lBottom; y >= lTop; y--)
		{
			lScr -= lScrPitch;
			lKBuf -= lKPitch;

			unsigned int lBlue  = *(lScr + 0);
			unsigned int lGreen = *(lScr + 1);
			unsigned int lRed   = *(lScr + 2);

			MACRO_BLUR(lRed, lGreen, lBlue, lPrevK, lKK, lPrevRed, lPrevGreen, lPrevBlue, lKBuf)

			*(lScr + 0) = (unsigned char)lPrevBlue;
			*(lScr + 1) = (unsigned char)lPrevGreen;
			*(lScr + 2) = (unsigned char)lPrevRed;
		}


		lScreen += 3;
		lKBuffer++;
	}
}

void Software3DPainter::PostRenderFocalBlur32Bit(float pSharpNearZ, float pSharpFarZ)
{
	int x, y;

	int lLeft   = (int)mClipLeft;
	int lRight  = (int)mClipRight;
	int lTop    = (int)mClipTop;
	int lBottom = (int)mClipBottom;
	unsigned int lKK = (unsigned int)(0.9f * 65535.0f);

	if (mKBuffer == 0 ||
	   (int)mKBuffer->GetWidth() != (lRight - lLeft + 1) ||
	   (int)mKBuffer->GetHeight() != (lBottom - lTop + 1))
	{
		if (mKBuffer == 0)
		{
			mKBuffer = new Lepra::Canvas((lRight - lLeft + 1), (lBottom - lTop + 1), Lepra::Canvas::BITDEPTH_32_BIT);
		}
		else
		{
			mKBuffer->Reset((lRight - lLeft + 1), (lBottom - lTop + 1), Lepra::Canvas::BITDEPTH_32_BIT);
		}

		mKBuffer->CreateBuffer();
	}

	// Precalculate k-values.
	float* lZBuffer = (float*)mZBuffer->GetBuffer() + (lTop - mZBufferY) * mZBuffer->GetPitch() + (lLeft - mZBufferX);
	unsigned int* lKBuffer = (unsigned int*)mKBuffer->GetBuffer() + (lTop - mZBufferY) * mKBuffer->GetPitch() + (lLeft - mZBufferX);

	for (y = lTop; y <= lBottom; y++)
	{
		float* lZBuf = lZBuffer;
		unsigned int* lKBuf = lKBuffer;
		for (x = lLeft; x <= lRight; x++)
		{
			float lOneOverZ = (float)*lZBuf++;
			*lKBuf++ = (unsigned int)(CalculateBlurConstant(pSharpNearZ, pSharpFarZ, lOneOverZ) * 65535.0f);
		}

		lZBuffer += mZBuffer->GetPitch();
		lKBuffer += mKBuffer->GetPitch();
	}

	unsigned int* lScreen = (unsigned int*)mScreen->GetBuffer() + lTop * mScreen->GetPitch();
	lKBuffer = (unsigned int*)mKBuffer->GetBuffer() + ((lTop - mZBufferY) * mKBuffer->GetPitch() + (lLeft - mZBufferX));

	for (y = lTop; y <= lBottom; y++)
	{
		// Blur right...
		unsigned int lPrevRed   = 0;
		unsigned int lPrevGreen = 0;
		unsigned int lPrevBlue  = 0;
		unsigned int lPrevK = 0;

		unsigned char* lScr = (unsigned char*)(lScreen + lLeft);
		unsigned int* lKBuf = lKBuffer;

		for (x = lLeft; x <= lRight; x++)
		{
			unsigned int lBlue  = *(lScr + 0);
			unsigned int lGreen = *(lScr + 1);
			unsigned int lRed   = *(lScr + 2);

			MACRO_BLUR(lRed, lGreen, lBlue, lPrevK, lKK, lPrevRed, lPrevGreen, lPrevBlue, lKBuf)

			*(lScr + 0) = (unsigned char)lPrevBlue;
			*(lScr + 1) = (unsigned char)lPrevGreen;
			*(lScr + 2) = (unsigned char)lPrevRed;

			lKBuf++;
			lScr += 4;
		}

		// Blur left...
		for (x = lRight; x >= lLeft; x--)
		{
			lScr -= 4;
			lKBuf--;

			unsigned int lBlue  = *(lScr + 0);
			unsigned int lGreen = *(lScr + 1);
			unsigned int lRed   = *(lScr + 2);

			MACRO_BLUR(lRed, lGreen, lBlue, lPrevK, lKK, lPrevRed, lPrevGreen, lPrevBlue, lKBuf)

			*(lScr + 0) = (unsigned char)lPrevBlue;
			*(lScr + 1) = (unsigned char)lPrevGreen;
			*(lScr + 2) = (unsigned char)lPrevRed;
		}

		lScreen += mScreen->GetPitch();
		lKBuffer += mKBuffer->GetPitch();
	}

	lScreen = (unsigned int*)mScreen->GetBuffer() + lTop * mScreen->GetPitch() + lLeft;
	lKBuffer = (unsigned int*)mKBuffer->GetBuffer() + ((lTop - mZBufferY) * mKBuffer->GetPitch() + (lLeft - mZBufferX));
	for (x = lLeft; x <= lRight; x++)
	{
		// Blur down...
		unsigned int lPrevRed   = 0;
		unsigned int lPrevGreen = 0;
		unsigned int lPrevBlue  = 0;
		unsigned int lPrevK = 0;

		unsigned int lScrPitch = (mScreen->GetPitch()<<2);
		unsigned int lKPitch = mKBuffer->GetPitch();

		unsigned char* lScr = (unsigned char*)lScreen;
		unsigned int* lKBuf = lKBuffer;
		for (y = lTop; y <= lBottom; y++)
		{
			unsigned int lBlue  = *(lScr + 0);
			unsigned int lGreen = *(lScr + 1);
			unsigned int lRed   = *(lScr + 2);

			MACRO_BLUR(lRed, lGreen, lBlue, lPrevK, lKK, lPrevRed, lPrevGreen, lPrevBlue, lKBuf)

			*(lScr + 0) = (unsigned char)lPrevBlue;
			*(lScr + 1) = (unsigned char)lPrevGreen;
			*(lScr + 2) = (unsigned char)lPrevRed;

			lScr += lScrPitch;
			lKBuf += lKPitch;
		}

		for (y = lBottom; y >= lTop; y--)
		{
			lScr -= lScrPitch;
			lKBuf -= lKPitch;

			unsigned int lBlue  = *(lScr + 0);
			unsigned int lGreen = *(lScr + 1);
			unsigned int lRed   = *(lScr + 2);

			MACRO_BLUR(lRed, lGreen, lBlue, lPrevK, lKK, lPrevRed, lPrevGreen, lPrevBlue, lKBuf)

			*(lScr + 0) = (unsigned char)lPrevBlue;
			*(lScr + 1) = (unsigned char)lPrevGreen;
			*(lScr + 2) = (unsigned char)lPrevRed;
		}


		lScreen++;
		lKBuffer++;
	}
}

float Software3DPainter::CalculateBlurConstant(float pSharpNearZ, float pSharpFarZ, float pOneOverZ)
{
	float lK = 0;

	if (pOneOverZ > pSharpNearZ)
	{
		// Too close to the camera...
		float lX = pOneOverZ - pSharpNearZ;
		lK = 1.0f - 1.0f / (lX * 200.0f + 1.0f);
		if (lK > 1.0f)
		{
			lK = 1.0f;
		}
	}

	if (pOneOverZ < pSharpFarZ)
	{
		// Too far from the camera...
		// 1 - 1 / (kx + 1)
		float lX = pSharpFarZ - pOneOverZ;
		lK = 1.0f - 1.0f / (lX * 200.0f + 1.0f);
		if (lK > 1.0f)
		{
			lK = 1.0f;
		}
	}

	return lK;
}

void Software3DPainter::DrawArrayElements(const Lepra::uint32* pIndices, int pCount, RenderingPrimitive pPrimitive)
{
	if (mVertexData == 0)
		return;

	Begin(pPrimitive);

	// Transform vertices and normals.
	int i;
	for (i = 0; i < mVertexCount; i++)
	{
		int lOffset = i * 3;

		Lepra::Vector3DF lVector(mVertexData[lOffset + 0],
					  mVertexData[lOffset + 1],
					  mVertexData[lOffset + 2]);

		lVector = mCamSpaceOrientation.GetRotatedVector(lVector) + mCamSpaceTranslation;

		mTranslatedVertexData[lOffset + 0] = (float)lVector.x;
		mTranslatedVertexData[lOffset + 1] = (float)lVector.y;
		mTranslatedVertexData[lOffset + 2] = (float)lVector.z;

		if (mNormalData != 0)
		{
			Lepra::Vector3DF lNormal(mNormalData[lOffset + 0],
						  mNormalData[lOffset + 1],
						  mNormalData[lOffset + 2]);

			lNormal = mModelOrientation.GetRotatedVector(lNormal);
			mTranslatedNormalData[lOffset + 0] = (float)lNormal.x;
			mTranslatedNormalData[lOffset + 1] = (float)lNormal.y;
			mTranslatedNormalData[lOffset + 2] = (float)lNormal.z;
		}
	}

	for (int i = 0; i < pCount; i++)
	{
		if (mCurrentXYZIndex >= mIndexLimit)
		{
			RenderPrimitive(this);
			switch(pPrimitive)
			{
				case RENDER_LINES:
				case RENDER_TRIANGLES:
				{
					mCurrentXYZIndex = 0;
					mCurrentNormalIndex = 0;
					mCurrentRGBIndex = 0;
					mCurrentUVIndex = 0;
				} break;
				case RENDER_LINE_STRIP:
				case RENDER_TRIANGLE_STRIP:
				case RENDER_TRIANGLE_FAN:
				{
					// TODO: Verify that this works. Probably not...
					CheckVertexData();
				} break;
			}
		}
		
		unsigned long lOffset = pIndices[i] * 3;

		mPoint[mCurrentXYZIndex].mXYZ->x = mVertexData[lOffset + 0];
		mPoint[mCurrentXYZIndex].mXYZ->y = mVertexData[lOffset + 1];
		mPoint[mCurrentXYZIndex].mXYZ->z = mVertexData[lOffset + 2];
		mPoint[mCurrentXYZIndex].mXYZ->mTranslatedX = mTranslatedVertexData[lOffset + 0];
		mPoint[mCurrentXYZIndex].mXYZ->mTranslatedY = mTranslatedVertexData[lOffset + 1];
		mPoint[mCurrentXYZIndex].mXYZ->mTranslatedZ = mTranslatedVertexData[lOffset + 2];

		if (mNormalData != 0)
		{
			mPoint[mCurrentNormalIndex].mNormal->x = mTranslatedNormalData[lOffset + 0];
			mPoint[mCurrentNormalIndex].mNormal->y = mTranslatedNormalData[lOffset + 1];
			mPoint[mCurrentNormalIndex].mNormal->z = mTranslatedNormalData[lOffset + 2];
			mCurrentNormalIndex++;
		}

		if (mColorData != 0)
		{
			mPoint[mCurrentRGBIndex].mRGB->r = mColorData[lOffset + 0];
			mPoint[mCurrentRGBIndex].mRGB->g = mColorData[lOffset + 1];
			mPoint[mCurrentRGBIndex].mRGB->b = mColorData[lOffset + 2];
			mCurrentRGBIndex++;
		}
		else if(mColorData != 0)
		{
			mPoint[mCurrentRGBIndex].mRGB->r = (float)mColorData[lOffset + 0] / 255.0f;
			mPoint[mCurrentRGBIndex].mRGB->g = (float)mColorData[lOffset + 1] / 255.0f;
			mPoint[mCurrentRGBIndex].mRGB->b = (float)mColorData[lOffset + 2] / 255.0f;
			mCurrentRGBIndex++;
		}
		else
		{
			mPoint[mCurrentRGBIndex].mRGB->r = mRed;
			mPoint[mCurrentRGBIndex].mRGB->g = mGreen;
			mPoint[mCurrentRGBIndex].mRGB->b = mBlue;
			mCurrentRGBIndex++;
		}

		if (mUVData != 0)
		{
			mPoint[mCurrentUVIndex].mUVM->u = mUVData[pIndices[i] * 2 + 0];
			mPoint[mCurrentUVIndex].mUVM->v = mUVData[pIndices[i] * 2 + 1];

			if(mTextureTransformIsIdentity == false)
			{
				Lepra::Vector3DF lUVCoords(mPoint[mCurrentUVIndex].mUVM->u, mPoint[mCurrentUVIndex].mUVM->v, 0);
				lUVCoords = mTextureTransform.Transform(lUVCoords);
				mPoint[mCurrentUVIndex].mUVM->u = lUVCoords.x;
				mPoint[mCurrentUVIndex].mUVM->v = lUVCoords.y;
			}

			mCurrentUVIndex++;
		}
		mCurrentXYZIndex++;
	}

	End();
}


float Software3DPainter::GetU_UV(Vertex3D* pV)
{
	return ((Vertex3DUV*)pV)->GetUOverZ() * ((Vertex3DUV*)pV)->GetZ();
}

float Software3DPainter::GetV_UV(Vertex3D* pV)
{
	return ((Vertex3DUV*)pV)->GetVOverZ() * ((Vertex3DUV*)pV)->GetZ();
}

float Software3DPainter::GetU_UVM(Vertex3D* pV)
{
	return ((Vertex3DUVM*)pV)->GetUOverZ() * ((Vertex3DUVM*)pV)->GetZ();
}

float Software3DPainter::GetV_UVM(Vertex3D* pV)
{
	return ((Vertex3DUVM*)pV)->GetVOverZ() * ((Vertex3DUVM*)pV)->GetZ();
}

float Software3DPainter::GetU_UVRGB(Vertex3D* pV)
{
	return ((Vertex3DUVRGB*)pV)->GetUOverZ() * ((Vertex3DUVRGB*)pV)->GetZ();
}

float Software3DPainter::GetV_UVRGB(Vertex3D* pV)
{
	return ((Vertex3DUVRGB*)pV)->GetVOverZ() * ((Vertex3DUVRGB*)pV)->GetZ();
}

float Software3DPainter::GetU_UVRGBM(Vertex3D* pV)
{
	return ((Vertex3DUVRGBM*)pV)->GetUOverZ() * ((Vertex3DUVRGBM*)pV)->GetZ();
}

float Software3DPainter::GetV_UVRGBM(Vertex3D* pV)
{
	return ((Vertex3DUVRGBM*)pV)->GetVOverZ() * ((Vertex3DUVRGBM*)pV)->GetZ();
}

int Software3DPainter::GetFastMipMapLevel(Vertex3D** pVertex, int pNumVertex, GetUFunc pGetU, GetVFunc pGetV)
{
	float lTextureArea = 0;
	float lArea = 0;

	float lTextureWidth  = (float)mTexture->GetWidth();
	float lTextureHeight = (float)mTexture->GetHeight();

	for (int i = 1; i < pNumVertex - 1; i++)
	{
		Lepra::Vector3DF lV0(pVertex[i]->GetX() - pVertex[0]->GetX(),
				      pVertex[i]->GetY() - pVertex[0]->GetY(),
				      0);

		Lepra::Vector3DF lV1(pVertex[i + 1]->GetX() - pVertex[0]->GetX(),
				      pVertex[i + 1]->GetY() - pVertex[0]->GetY(),
				      0);

		lArea += lV1.Cross(lV0).GetLength() * 0.5f;



		Lepra::Vector3DF lUV0((pGetU(pVertex[i]) - pGetU(pVertex[0])) * lTextureWidth,
				       (pGetV(pVertex[i]) - pGetV(pVertex[0])) * lTextureHeight,
				       0);

		Lepra::Vector3DF lUV1((pGetU(pVertex[i + 1]) - pGetU(pVertex[0])) * lTextureWidth,
				       (pGetV(pVertex[i + 1]) - pGetV(pVertex[0])) * lTextureHeight,
				       0);

		lTextureArea += lUV1.Cross(lUV0).GetLength() * 0.5f;
	}

	if(lArea < 1e-15)
	{
		return mTexture->GetNumMipMapLevels() - 1;
	}

	float lMipMapLevel = log(lTextureArea / lArea) / 1.3862943611198906188344642429164f; //log(4.0);

	if (lMipMapLevel < 0)
	{
		lMipMapLevel = 0;
	}

	// This check is performed to make sure that we don't get too large values
	// that can wrap around when converted to an integer.
	if (lMipMapLevel >= mTexture->GetNumMipMapLevels())
	{
		lMipMapLevel = (float)(mTexture->GetNumMipMapLevels() - 1);
	}

	int lMipMapLevelInt = (int)floor(lMipMapLevel + 0.5);
	if (lMipMapLevelInt >= mTexture->GetNumMipMapLevels())
	{
		lMipMapLevelInt = mTexture->GetNumMipMapLevels() - 1;
	}

	return lMipMapLevelInt;
}

float Software3DPainter::GetNicestMipMapLevel(float pZ)
{
	float lM = (mMipMapMidValue * pZ / mMipMapScale);

	if (lM <= 1.0f)
	{
		return 0.0f;
	}

	// Now calculate the log2 of lM.
	float lPowOf2 = 1.0f;
	float lExponent = 0.0f;
	while (lPowOf2 < lM)
	{
		lExponent += 1.0f;
		lPowOf2 *= 2.0f;
	}

	// Step back to the nearest integer below...
	lExponent -= 1.0f;
	lPowOf2 *= 0.5f;

	// Now we know that the exponent is somewhere between lExponent and lExponent + 1.

	float lError;
	float lExpAdd = 0.5f;
	float lTestExpAdd = lExpAdd;

	do
	{
		float lTestPowOf2 = lPowOf2 * pow(2.0f, lTestExpAdd);
		lError = lM - lTestPowOf2;
		
		lExpAdd *= 0.5f;

		if (lError > 0.0f)
		{
			lTestExpAdd += lExpAdd;
		}
		else
		{
			lTestExpAdd -= lExpAdd;
		}

	}while(fabs(lError) > 1e-6f);

	return lExponent + lTestExpAdd;
}

} // End namespace.
