/*
	Lepra::File:   Software3DPainter_RenderPrimitiveLines.cpp
	Class:  Software3DPainter
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand
*/

#include "../../Include/UiVertex.h"
#include "../../Include/UiSoftware3DPainter.h"
#include "../../Include/UiTBC.h"

namespace UiTbc
{

void Software3DPainter::RenderLinesSingleColor(Software3DPainter* pThis)
{
	if (pThis->mCurrentXYZIndex >= 2)
	{
		if (pThis->mScreen != 0)
		{
			if (pThis->mOrthographicProjection == true ||
				pThis->ClipVectorNearZ(&pThis->mPoint[0],
							 &pThis->mPoint[1],
							 (ClipFlags)0,
							 pThis) != 3)
			{
				// The scale factor scales the coordinates to the viewport resolution.
				// This way, things will keep the same size no matter what screen resolution
				// you use. (And as you can see, the scale factor is actually the viewport width).
				const float lScale = (pThis->mViewportRight - pThis->mViewportLeft) * pThis->mScreenDistToOrigin;

				// Project the two points on the screen.
				float lP;
				if (pThis->mOrthographicProjection == true)
				{
					lP = lScale / pThis->mNearZ;
				}
				else
				{
					lP = lScale / pThis->mPoint[0].mXYZ->mTranslatedZ;
				}
				
				DataXYZ lProjectedXYZ[2];

				lProjectedXYZ[0].x = pThis->mPerspectiveCenterX + pThis->mPoint[0].mXYZ->mTranslatedX * lP;
				lProjectedXYZ[0].y = pThis->mPerspectiveCenterY - pThis->mPoint[0].mXYZ->mTranslatedY * lP;
				lProjectedXYZ[0].z = pThis->mPoint[0].mXYZ->mTranslatedZ;

				if (pThis->mOrthographicProjection == false)
				{
					lP = lScale / pThis->mPoint[1].mXYZ->mTranslatedZ;
				}
				lProjectedXYZ[1].x = pThis->mPerspectiveCenterX + pThis->mPoint[1].mXYZ->mTranslatedX * lP;
				lProjectedXYZ[1].y = pThis->mPerspectiveCenterY - pThis->mPoint[1].mXYZ->mTranslatedY * lP;
				lProjectedXYZ[1].z = pThis->mPoint[1].mXYZ->mTranslatedZ;

				// Setup the color.
				Lepra::Color lColor(0, 0, 0);
				if (pThis->mCurrentRGBIndex >= 1)
				{
					lColor.mRed   = (Lepra::uint8)(pThis->mPoint[0].mRGB->r * 255.0f);
					lColor.mGreen = (Lepra::uint8)(pThis->mPoint[0].mRGB->g * 255.0f);
					lColor.mBlue  = (Lepra::uint8)(pThis->mPoint[0].mRGB->b * 255.0f);

					if (pThis->mScreen->GetBitDepth() == Lepra::Canvas::BITDEPTH_8_BIT && 
					   pThis->m8BitColorSearchMode == true)
					{
						lColor.mAlpha = pThis->mPainter.FindMatchingColor(lColor);
					}
				}

				// Clip the projected points against the clipping rect.
				PointData lProjectedPoint[2];
				lProjectedPoint[0].mXYZ = &lProjectedXYZ[0];
				lProjectedPoint[1].mXYZ = &lProjectedXYZ[1];

				DataXYZ lClippedXYZ[2];
				PointData lClippedPoint[2];
				lClippedPoint[0].mXYZ = &lClippedXYZ[0];
				lClippedPoint[1].mXYZ = &lClippedXYZ[1];

				if (pThis->ClipLine(&lProjectedPoint[0], 
									 &lProjectedPoint[1], 
									 &lClippedPoint[0], 
									 &lClippedPoint[1], 
									 (ClipFlags)0) != 0)
				{
					// Render the line.
					Vertex3D lV[2];
					
					lV[0].SetX(lClippedXYZ[0].x);
					lV[0].SetY(lClippedXYZ[0].y);
					lV[0].SetZ(lClippedXYZ[0].z);
					lV[1].SetX(lClippedXYZ[1].x);
					lV[1].SetY(lClippedXYZ[1].y);
					lV[1].SetZ(lClippedXYZ[1].z);

					pThis->DrawSingleColorLineFunc(lV, lColor, pThis);
				}
			}
		}

		// Empty the vertex buffers...
		if (pThis->mCurrentXYZIndex >= 2)
		{
			DataXYZ* lTempXYZ = pThis->mPoint[0].mXYZ;
			pThis->mPoint[0].mXYZ = pThis->mPoint[2].mXYZ;
			pThis->mPoint[2].mXYZ = pThis->mPoint[1].mXYZ;
			pThis->mPoint[1].mXYZ = lTempXYZ;
			pThis->mCurrentXYZIndex -= 2;
		}
	}
}

void Software3DPainter::RenderLinesRGB(Software3DPainter* pThis)
{
	if (pThis->mCurrentXYZIndex >= 2)
	{
		if (pThis->mCurrentRGBIndex < 2)
		{
			RenderLinesSingleColor(pThis);
		}
		else
		{
			if (pThis->mScreen != 0)
			{
				if (pThis->mOrthographicProjection == true ||
					pThis->ClipVectorNearZ(&pThis->mPoint[0],
								 &pThis->mPoint[1],
								 CLIP_RGB,
								 pThis) != 3)
				{
					// The scale factor scales the coordinates to the viewport resolution.
					// This way, things will keep the same size no matter what screen resolution
					// you use. (And as you can see, the scale factor is actually the viewport width).
					const float lScale = (pThis->mViewportRight - pThis->mViewportLeft) * pThis->mScreenDistToOrigin;

					// Project the two points on the screen.
					float lP;
					if (pThis->mOrthographicProjection == true)
					{
						lP = lScale / pThis->mNearZ;
					}
					else
					{
						lP = lScale / pThis->mPoint[0].mXYZ->mTranslatedZ;
					}

					DataXYZ lProjectedXYZ[2];

					lProjectedXYZ[0].x = pThis->mPerspectiveCenterX + pThis->mPoint[0].mXYZ->mTranslatedX * lP;
					lProjectedXYZ[0].y = pThis->mPerspectiveCenterY - pThis->mPoint[0].mXYZ->mTranslatedY * lP;
					lProjectedXYZ[0].z = pThis->mPoint[0].mXYZ->mTranslatedZ;

					if (pThis->mOrthographicProjection == false)
					{
						lP = lScale / pThis->mPoint[1].mXYZ->mTranslatedZ;
					}
					lProjectedXYZ[1].x = pThis->mPerspectiveCenterX + pThis->mPoint[1].mXYZ->mTranslatedX * lP;
					lProjectedXYZ[1].y = pThis->mPerspectiveCenterY - pThis->mPoint[1].mXYZ->mTranslatedY * lP;
					lProjectedXYZ[1].z = pThis->mPoint[1].mXYZ->mTranslatedZ;

					// Clip the projected points against the clipping rect.
					PointData lProjectedPoint[2];
					lProjectedPoint[0].mXYZ = &lProjectedXYZ[0];
					lProjectedPoint[0].mRGB = pThis->mPoint[0].mRGB;
					lProjectedPoint[1].mXYZ = &lProjectedXYZ[1];
					lProjectedPoint[1].mRGB = pThis->mPoint[1].mRGB;

					DataXYZ lClippedXYZ[2];
					DataRGB lClippedRGB[2];
					PointData lClippedPoint[2];
					lClippedPoint[0].mXYZ = &lClippedXYZ[0];
					lClippedPoint[0].mRGB = &lClippedRGB[0];
					lClippedPoint[1].mXYZ = &lClippedXYZ[1];
					lClippedPoint[1].mRGB = &lClippedRGB[1];

					if (pThis->ClipLine(&lProjectedPoint[0],
										 &lProjectedPoint[1], 
										 &lClippedPoint[0],
										 &lClippedPoint[1],
										 CLIP_RGB) != 0)
					{
						// Render the line.
						Vertex3DRGB lV[2];
						lV[0].SetX(lClippedPoint[0].mXYZ->x);
						lV[0].SetY(lClippedPoint[0].mXYZ->y);
						lV[0].SetZ(lClippedPoint[0].mXYZ->z);
						lV[0].SetR(lClippedPoint[0].mRGB->r);
						lV[0].SetG(lClippedPoint[0].mRGB->g);
						lV[0].SetB(lClippedPoint[0].mRGB->b);

						lV[1].SetX(lClippedPoint[1].mXYZ->x);
						lV[1].SetY(lClippedPoint[1].mXYZ->y);
						lV[1].SetZ(lClippedPoint[1].mXYZ->z);
						lV[1].SetR(lClippedPoint[1].mRGB->r);
						lV[1].SetG(lClippedPoint[1].mRGB->g);
						lV[1].SetB(lClippedPoint[1].mRGB->b);
						pThis->DrawRGBLineFunc(lV, pThis);
					}
				}
			}
		}

		// Empty the vertex buffers...
		if (pThis->mCurrentXYZIndex >= 2)
		{
			DataXYZ* lTempXYZ = pThis->mPoint[0].mXYZ;
			pThis->mPoint[0].mXYZ = pThis->mPoint[2].mXYZ;
			pThis->mPoint[2].mXYZ = pThis->mPoint[1].mXYZ;
			pThis->mPoint[1].mXYZ = lTempXYZ;
			pThis->mCurrentXYZIndex -= 2;
		}

		if (pThis->mCurrentRGBIndex >= 2)
		{
			DataRGB* lTempRGB = pThis->mPoint[0].mRGB;
			pThis->mPoint[0].mRGB = pThis->mPoint[2].mRGB;
			pThis->mPoint[2].mRGB = pThis->mPoint[1].mRGB;
			pThis->mPoint[1].mRGB = lTempRGB;
			pThis->mCurrentRGBIndex -= 2;
		}
	}
}

void Software3DPainter::RenderLinesUV(Software3DPainter* pThis)
{
	if (pThis->mCurrentXYZIndex >= 2)
	{
		if (pThis->mCurrentUVIndex < 2)
		{
			RenderLinesSingleColor(pThis);
		}
		else
		{
			if (pThis->mScreen != 0)
			{
				if (pThis->mTexture != 0)
				{
					if (pThis->mOrthographicProjection == true ||
						pThis->ClipVectorNearZ(&pThis->mPoint[0],
									 &pThis->mPoint[1],
									 CLIP_RGB,
									 pThis) != 3)
					{
						// The scale factor scales the coordinates to the viewport resolution.
						// This way, things will keep the same size no matter what screen resolution
						// you use. (And as you can see, the scale factor is actually the viewport width).
						const float lScale = (pThis->mViewportRight - pThis->mViewportLeft) * pThis->mScreenDistToOrigin;

						// Project the two points on the screen.
						float lP;
						if (pThis->mOrthographicProjection == true)
						{
							lP = lScale / pThis->mNearZ;
						}
						else
						{
							lP = lScale / pThis->mPoint[0].mXYZ->mTranslatedZ;
						}

						DataXYZ lProjectedXYZ[2];

						lProjectedXYZ[0].x = pThis->mPerspectiveCenterX + pThis->mPoint[0].mXYZ->mTranslatedX * lP;
						lProjectedXYZ[0].y = pThis->mPerspectiveCenterY - pThis->mPoint[0].mXYZ->mTranslatedY * lP;
						lProjectedXYZ[0].z = pThis->mPoint[0].mXYZ->mTranslatedZ;

						if (pThis->mOrthographicProjection == false)
						{
							lP = lScale / pThis->mPoint[1].mXYZ->mTranslatedZ;
						}
						lProjectedXYZ[1].x = pThis->mPerspectiveCenterX + pThis->mPoint[1].mXYZ->mTranslatedX * lP;
						lProjectedXYZ[1].y = pThis->mPerspectiveCenterY - pThis->mPoint[1].mXYZ->mTranslatedY * lP;
						lProjectedXYZ[1].z = pThis->mPoint[1].mXYZ->mTranslatedZ;

						// Clip the projected points against the clipping rect.
						PointData lProjectedPoint[2];
						lProjectedPoint[0].mXYZ = &lProjectedXYZ[0];
						lProjectedPoint[0].mUVM = pThis->mPoint[0].mUVM;
						lProjectedPoint[1].mXYZ = &lProjectedXYZ[1];
						lProjectedPoint[1].mUVM = pThis->mPoint[1].mUVM;

						DataXYZ lClippedXYZ[2];
						DataUVM lClippedUVM[2];
						PointData lClippedPoint[2];
						lClippedPoint[0].mXYZ = &lClippedXYZ[0];
						lClippedPoint[0].mUVM = &lClippedUVM[0];
						lClippedPoint[1].mXYZ = &lClippedXYZ[1];
						lClippedPoint[1].mUVM = &lClippedUVM[1];

						if (pThis->ClipLine(&lProjectedPoint[0],
											 &lProjectedPoint[1],
											 &lClippedPoint[0],
											 &lClippedPoint[1],
											 CLIP_UV) != 0)
						{
							// Render the line.
							Vertex3DUV lV[2];
							lV[0].SetX(lClippedPoint[0].mXYZ->x);
							lV[0].SetY(lClippedPoint[0].mXYZ->y);
							lV[0].SetZ(lClippedPoint[0].mXYZ->z);
							lV[0].SetU(lClippedPoint[1].mUVM->u * ((float)pThis->mTexture->GetColorMap(0)->GetWidth() - 1));
							lV[0].SetV(lClippedPoint[1].mUVM->v * ((float)pThis->mTexture->GetColorMap(0)->GetHeight() - 1));

							lV[1].SetX(lClippedPoint[1].mXYZ->x);
							lV[1].SetY(lClippedPoint[1].mXYZ->y);
							lV[1].SetZ(lClippedPoint[1].mXYZ->z);
							lV[1].SetU(lClippedPoint[1].mUVM->u * ((float)pThis->mTexture->GetColorMap(0)->GetWidth() - 1));
							lV[1].SetV(lClippedPoint[1].mUVM->v * ((float)pThis->mTexture->GetColorMap(0)->GetHeight() - 1));
							
							switch(pThis->mRenderingHint)
							{
							case RENDER_FAST:
								// TODO: Calculate the mipmap level.
								pThis->DrawUVLineFuncFast(lV, pThis->mTexture->GetColorMap(0), pThis);
								break;
							case RENDER_NICEST:
							default:
								pThis->DrawUVLineFunc(lV, pThis);
								break;
							};
						}
					}
				}
				else
				{
					RenderLinesSingleColor(pThis);
				}
			}
		}

		// Empty the vertex buffers...
		if (pThis->mCurrentXYZIndex >= 2)
		{
			DataXYZ* lTempXYZ = pThis->mPoint[0].mXYZ;
			pThis->mPoint[0].mXYZ = pThis->mPoint[2].mXYZ;
			pThis->mPoint[2].mXYZ = pThis->mPoint[1].mXYZ;
			pThis->mPoint[1].mXYZ = lTempXYZ;
			pThis->mCurrentXYZIndex -= 2;
		}

		if (pThis->mCurrentUVIndex >= 2)
		{
			DataUVM* lTempUVM = pThis->mPoint[0].mUVM;
			pThis->mPoint[0].mUVM= pThis->mPoint[2].mUVM;
			pThis->mPoint[2].mUVM= pThis->mPoint[1].mUVM;
			pThis->mPoint[1].mUVM= lTempUVM;
			pThis->mCurrentUVIndex -= 2;
		}
	}
}

void Software3DPainter::RenderLinesUVRGB(Software3DPainter* pThis)
{
	if (pThis->mCurrentXYZIndex >= 2)
	{
		if (pThis->mCurrentRGBIndex < 2)
		{
			RenderLinesUV(pThis);
		}
		else if(pThis->mCurrentUVIndex < 2)
		{
			RenderLinesRGB(pThis);
		}
		else
		{
			if (pThis->mScreen != 0)
			{
				if (pThis->mTexture != 0)
				{
					if (pThis->mOrthographicProjection == true ||
						pThis->ClipVectorNearZ(&pThis->mPoint[0],
									 &pThis->mPoint[1],
									 CLIP_RGB,
									 pThis) != 3)
					{
						// The scale factor scales the coordinates to the viewport resolution.
						// This way, things will keep the same size no matter what screen resolution
						// you use. (And as you can see, the scale factor is actually the viewport width).
						const float lScale = (pThis->mViewportRight - pThis->mViewportLeft) * pThis->mScreenDistToOrigin;

						// Project the two points on the screen.
						float lP;
						if (pThis->mOrthographicProjection == true)
						{
							lP = lScale / pThis->mNearZ;
						}
						else
						{
							lP = lScale / pThis->mPoint[0].mXYZ->mTranslatedZ;
						}

						DataXYZ lProjectedXYZ[2];

						lProjectedXYZ[0].x = pThis->mPerspectiveCenterX + pThis->mPoint[0].mXYZ->mTranslatedX * lP;
						lProjectedXYZ[0].y = pThis->mPerspectiveCenterY - pThis->mPoint[0].mXYZ->mTranslatedY * lP;
						lProjectedXYZ[0].z = pThis->mPoint[0].mXYZ->mTranslatedZ;

						if (pThis->mOrthographicProjection == false)
						{
							lP = lScale / pThis->mPoint[1].mXYZ->mTranslatedZ;
						}
						lProjectedXYZ[1].x = pThis->mPerspectiveCenterX + pThis->mPoint[1].mXYZ->mTranslatedX * lP;
						lProjectedXYZ[1].y = pThis->mPerspectiveCenterY - pThis->mPoint[1].mXYZ->mTranslatedY * lP;
						lProjectedXYZ[1].z = pThis->mPoint[1].mXYZ->mTranslatedZ;

						// Clip the projected points against the clipping rect.
						PointData lProjectedPoint[2];
						lProjectedPoint[0].mXYZ = &lProjectedXYZ[0];
						lProjectedPoint[0].mUVM = pThis->mPoint[0].mUVM;
						lProjectedPoint[0].mRGB = pThis->mPoint[0].mRGB;
						lProjectedPoint[1].mXYZ = &lProjectedXYZ[1];
						lProjectedPoint[1].mUVM = pThis->mPoint[1].mUVM;
						lProjectedPoint[1].mRGB = pThis->mPoint[1].mRGB;

						DataXYZ lClippedXYZ[2];
						DataRGB lClippedRGB[2];
						DataUVM lClippedUVM[2];
						PointData lClippedPoint[2];
						lClippedPoint[0].mXYZ = &lClippedXYZ[0];
						lClippedPoint[0].mUVM = &lClippedUVM[0];
						lClippedPoint[0].mRGB = &lClippedRGB[0];
						lClippedPoint[1].mXYZ = &lClippedXYZ[1];
						lClippedPoint[1].mUVM = &lClippedUVM[1];
						lClippedPoint[1].mRGB = &lClippedRGB[1];

						if (pThis->ClipLine(&lProjectedPoint[0],
											 &lProjectedPoint[1],
											 &lClippedPoint[0],
											 &lClippedPoint[1],
											 (ClipFlags)(CLIP_UV | CLIP_RGB)) != 0)
						{
							// Render the line.
							Vertex3DUVRGB lV[2];
							lV[0].SetX(lClippedPoint[0].mXYZ->x);
							lV[0].SetY(lClippedPoint[0].mXYZ->y);
							lV[0].SetZ(lClippedPoint[0].mXYZ->z);
							lV[0].SetU(lClippedPoint[0].mUVM->u * ((float)pThis->mTexture->GetColorMap(0)->GetWidth() - 1));
							lV[0].SetV(lClippedPoint[0].mUVM->v * ((float)pThis->mTexture->GetColorMap(0)->GetHeight() - 1));
							lV[0].SetR(lClippedPoint[0].mRGB->r);
							lV[0].SetG(lClippedPoint[0].mRGB->g);
							lV[0].SetB(lClippedPoint[0].mRGB->b);

							lV[1].SetX(lClippedPoint[1].mXYZ->x);
							lV[1].SetY(lClippedPoint[1].mXYZ->y);
							lV[1].SetZ(lClippedPoint[1].mXYZ->z);
							lV[1].SetU(lClippedPoint[1].mUVM->u * ((float)pThis->mTexture->GetColorMap(0)->GetWidth() - 1));
							lV[1].SetV(lClippedPoint[1].mUVM->v * ((float)pThis->mTexture->GetColorMap(0)->GetHeight() - 1));
							lV[1].SetR(lClippedPoint[1].mRGB->r);
							lV[1].SetG(lClippedPoint[1].mRGB->g);
							lV[1].SetB(lClippedPoint[1].mRGB->b);

							switch(pThis->mRenderingHint)
							{
							case RENDER_FAST:
								// TODO: Calculate the mipmap level.
								pThis->DrawUVRGBLineFuncFast(lV, pThis->mTexture->GetColorMap(0), pThis);
								break;
							case RENDER_NICEST:
							default:
								pThis->DrawUVRGBLineFunc(lV, pThis);
								break;
							};
						}
					}
				}
				else
				{
					RenderLinesSingleColor(pThis);
				}
			}
		}
		// Empty the vertex buffers...
		if (pThis->mCurrentXYZIndex >= 2)
		{
			DataXYZ* lTempXYZ = pThis->mPoint[0].mXYZ;
			pThis->mPoint[0].mXYZ = pThis->mPoint[2].mXYZ;
			pThis->mPoint[2].mXYZ = pThis->mPoint[1].mXYZ;
			pThis->mPoint[1].mXYZ = lTempXYZ;
			pThis->mCurrentXYZIndex -= 2;
		}

		if (pThis->mCurrentUVIndex >= 2)
		{
			DataUVM* lTempUVM = pThis->mPoint[0].mUVM;
			pThis->mPoint[0].mUVM= pThis->mPoint[2].mUVM;
			pThis->mPoint[2].mUVM= pThis->mPoint[1].mUVM;
			pThis->mPoint[1].mUVM= lTempUVM;
			pThis->mCurrentUVIndex -= 2;
		}

		if (pThis->mCurrentRGBIndex >= 2)
		{
			DataRGB* lTempRGB = pThis->mPoint[0].mRGB;
			pThis->mPoint[0].mRGB = pThis->mPoint[2].mRGB;
			pThis->mPoint[2].mRGB = pThis->mPoint[1].mRGB;
			pThis->mPoint[1].mRGB = lTempRGB;
			pThis->mCurrentRGBIndex -= 2;
		}
	}
}

void Software3DPainter::RenderLineStripSingleColor(Software3DPainter* pThis)
{
	if (pThis->mCurrentXYZIndex >= 2)
	{
		if (pThis->mScreen != 0)
		{
			DataXYZ lXYZ;
			DataUVM lUV;
			DataRGB lRGB;
			PointData lPoint;

			lXYZ.mTranslatedX = pThis->mPoint[1].mXYZ->mTranslatedX;
			lXYZ.mTranslatedY = pThis->mPoint[1].mXYZ->mTranslatedY;
			lXYZ.mTranslatedZ = pThis->mPoint[1].mXYZ->mTranslatedZ;
			lRGB.r = pThis->mPoint[1].mRGB->r;
			lRGB.g = pThis->mPoint[1].mRGB->g;
			lRGB.b = pThis->mPoint[1].mRGB->b;

			lPoint.mXYZ = &lXYZ;
			lPoint.mUVM = &lUV;
			lPoint.mRGB = &lRGB;

			if (pThis->mOrthographicProjection == true ||
				pThis->ClipVectorNearZ(&pThis->mPoint[0],
							 &lPoint,
							 (ClipFlags)0,
							 pThis) != 3)
			{
				// The scale factor scales the coordinates to the viewport resolution.
				// This way, things will keep the same size no matter what screen resolution
				// you use. (And as you can see, the scale factor is actually the viewport width).
				const float lScale = (pThis->mViewportRight - pThis->mViewportLeft) * pThis->mScreenDistToOrigin;

				// Project the two points on the screen.
				float lP;
				if (pThis->mOrthographicProjection == true)
				{
					lP = lScale / pThis->mNearZ;
				}
				else
				{
					lP = lScale / pThis->mPoint[0].mXYZ->mTranslatedZ;
				}
				DataXYZ lProjectedXYZ[2];

				lProjectedXYZ[0].x = pThis->mPerspectiveCenterX + pThis->mPoint[0].mXYZ->mTranslatedX * lP;
				lProjectedXYZ[0].y = pThis->mPerspectiveCenterY - pThis->mPoint[0].mXYZ->mTranslatedY * lP;
				lProjectedXYZ[0].z = pThis->mPoint[0].mXYZ->mTranslatedZ;

				if (pThis->mOrthographicProjection == false)
				{
					lP = lScale / lPoint.mXYZ->mTranslatedZ;
				}
				lProjectedXYZ[1].x = pThis->mPerspectiveCenterX + lPoint.mXYZ->mTranslatedX * lP;
				lProjectedXYZ[1].y = pThis->mPerspectiveCenterY - lPoint.mXYZ->mTranslatedY * lP;
				lProjectedXYZ[1].z = lPoint.mXYZ->mTranslatedZ;

				// Setup the color.
				Lepra::Color lColor(0, 0, 0);
				if (pThis->mCurrentRGBIndex >= 1)
				{
					lColor.mRed   = (Lepra::uint8)(pThis->mPoint[0].mRGB->r * 255.0f);
					lColor.mGreen = (Lepra::uint8)(pThis->mPoint[0].mRGB->g * 255.0f);
					lColor.mBlue  = (Lepra::uint8)(pThis->mPoint[0].mRGB->b * 255.0f);

					if (pThis->mScreen->GetBitDepth() == Lepra::Canvas::BITDEPTH_8_BIT && 
					   pThis->m8BitColorSearchMode == true)
					{
						lColor.mAlpha = pThis->mPainter.FindMatchingColor(lColor);
					}
				}

				// Clip the projected points against the clipping rect.
				PointData lProjectedPoint[2];
				lProjectedPoint[0].mXYZ = &lProjectedXYZ[0];
				lProjectedPoint[1].mXYZ = &lProjectedXYZ[1];

				DataXYZ lClippedXYZ[2];
				PointData lClippedPoint[2];
				lClippedPoint[0].mXYZ = &lClippedXYZ[0];
				lClippedPoint[1].mXYZ = &lClippedXYZ[1];

				if (pThis->ClipLine(&lProjectedPoint[0], 
									 &lProjectedPoint[1], 
									 &lClippedPoint[0], 
									 &lClippedPoint[1], 
									 (ClipFlags)0) != 0)
				{
					// Render the line.
					Vertex3D lV[2];
					lV[0].SetX(lClippedXYZ[0].x);
					lV[0].SetY(lClippedXYZ[0].y);
					lV[0].SetZ(lClippedXYZ[0].z);
					lV[1].SetX(lClippedXYZ[1].x);
					lV[1].SetY(lClippedXYZ[1].y);
					lV[1].SetZ(lClippedXYZ[1].z);
					pThis->DrawSingleColorLineFunc(lV, lColor, pThis);
				}
			}
		}

		// Empty the vertex buffers...
		if (pThis->mCurrentXYZIndex >= 2)
		{
			DataXYZ* lTempXYZ = pThis->mPoint[0].mXYZ;
			pThis->mPoint[0].mXYZ = pThis->mPoint[1].mXYZ;
			pThis->mPoint[1].mXYZ = pThis->mPoint[2].mXYZ;
			pThis->mPoint[2].mXYZ = lTempXYZ;
			pThis->mCurrentXYZIndex -= 1;
		}
	}
}

void Software3DPainter::RenderLineStripRGB(Software3DPainter* pThis)
{
	if (pThis->mCurrentXYZIndex >= 2)
	{
		if (pThis->mCurrentRGBIndex < 2)
		{
			RenderLinesSingleColor(pThis);
		}
		else
		{
			if (pThis->mScreen != 0)
			{
				DataXYZ lXYZ;
				DataRGB lRGB;
				PointData lPoint;

				lXYZ.mTranslatedX = pThis->mPoint[1].mXYZ->mTranslatedX;
				lXYZ.mTranslatedY = pThis->mPoint[1].mXYZ->mTranslatedY;
				lXYZ.mTranslatedZ = pThis->mPoint[1].mXYZ->mTranslatedZ;
				lRGB.r = pThis->mPoint[1].mRGB->r;
				lRGB.g = pThis->mPoint[1].mRGB->g;
				lRGB.b = pThis->mPoint[1].mRGB->b;

				lPoint.mXYZ = &lXYZ;
				lPoint.mRGB = &lRGB;

				if (pThis->mOrthographicProjection == true ||
					pThis->ClipVectorNearZ(&pThis->mPoint[0],
								 &lPoint,
								 CLIP_RGB,
								 pThis) != 3)
				{
					// The scale factor scales the coordinates to the viewport resolution.
					// This way, things will keep the same size no matter what screen resolution
					// you use. (And as you can see, the scale factor is actually the viewport width).
					const float lScale = (pThis->mViewportRight - pThis->mViewportLeft) * pThis->mScreenDistToOrigin;

					// Project the two points on the screen.
					float lP;
					if (pThis->mOrthographicProjection == true)
					{
						lP = lScale / pThis->mNearZ;
					}
					else
					{
						lP = lScale / pThis->mPoint[0].mXYZ->mTranslatedZ;
					}

					DataXYZ lProjectedXYZ[2];

					lProjectedXYZ[0].x = pThis->mPerspectiveCenterX + pThis->mPoint[0].mXYZ->mTranslatedX * lP;
					lProjectedXYZ[0].y = pThis->mPerspectiveCenterY - pThis->mPoint[0].mXYZ->mTranslatedY * lP;
					lProjectedXYZ[0].z = pThis->mPoint[0].mXYZ->mTranslatedZ;

					if (pThis->mOrthographicProjection == false)
					{
						lP = lScale / lPoint.mXYZ->mTranslatedZ;
					}
					lProjectedXYZ[1].x = pThis->mPerspectiveCenterX + lPoint.mXYZ->mTranslatedX * lP;
					lProjectedXYZ[1].y = pThis->mPerspectiveCenterY - lPoint.mXYZ->mTranslatedY * lP;
					lProjectedXYZ[1].z = lPoint.mXYZ->mTranslatedZ;

					// Clip the projected points against the clipping rect.
					PointData lProjectedPoint[2];
					lProjectedPoint[0].mXYZ = &lProjectedXYZ[0];
					lProjectedPoint[0].mRGB = pThis->mPoint[0].mRGB;
					lProjectedPoint[1].mXYZ = &lProjectedXYZ[1];
					lProjectedPoint[1].mRGB = lPoint.mRGB;

					DataXYZ lClippedXYZ[2];
					DataRGB lClippedRGB[2];
					PointData lClippedPoint[2];
					lClippedPoint[0].mXYZ = &lClippedXYZ[0];
					lClippedPoint[0].mRGB = &lClippedRGB[0];
					lClippedPoint[1].mXYZ = &lClippedXYZ[1];
					lClippedPoint[1].mRGB = &lClippedRGB[1];

					if (pThis->ClipLine(&lProjectedPoint[0],
										 &lProjectedPoint[1], 
										 &lClippedPoint[0],
										 &lClippedPoint[1],
										 CLIP_RGB) != 0)
					{
						// Render the line.
						Vertex3DRGB lV[2];
						lV[0].SetX(lClippedPoint[0].mXYZ->x);
						lV[0].SetY(lClippedPoint[0].mXYZ->y);
						lV[0].SetZ(lClippedPoint[0].mXYZ->z);
						lV[0].SetR(lClippedPoint[0].mRGB->r);
						lV[0].SetG(lClippedPoint[0].mRGB->g);
						lV[0].SetB(lClippedPoint[0].mRGB->b);

						lV[1].SetX(lClippedPoint[1].mXYZ->x);
						lV[1].SetY(lClippedPoint[1].mXYZ->y);
						lV[1].SetZ(lClippedPoint[1].mXYZ->z);
						lV[1].SetR(lClippedPoint[1].mRGB->r);
						lV[1].SetG(lClippedPoint[1].mRGB->g);
						lV[1].SetB(lClippedPoint[1].mRGB->b);
						pThis->DrawRGBLineFunc(lV, pThis);
					}
				}
			}
		}

		// Empty the vertex buffers...
		if (pThis->mCurrentXYZIndex >= 2)
		{
			DataXYZ* lTempXYZ = pThis->mPoint[0].mXYZ;
			pThis->mPoint[0].mXYZ = pThis->mPoint[1].mXYZ;
			pThis->mPoint[1].mXYZ = pThis->mPoint[2].mXYZ;
			pThis->mPoint[2].mXYZ = lTempXYZ;
			pThis->mCurrentXYZIndex -= 1;
		}

		if (pThis->mCurrentRGBIndex >= 2)
		{
			DataRGB* lTempRGB = pThis->mPoint[0].mRGB;
			pThis->mPoint[0].mRGB = pThis->mPoint[1].mRGB;
			pThis->mPoint[1].mRGB = pThis->mPoint[2].mRGB;
			pThis->mPoint[2].mRGB = lTempRGB;
			pThis->mCurrentRGBIndex -= 1;
		}
	}
}

void Software3DPainter::RenderLineStripUV(Software3DPainter* pThis)
{
	if (pThis->mCurrentXYZIndex >= 2)
	{
		if (pThis->mCurrentUVIndex < 2)
		{
			RenderLinesSingleColor(pThis);
		}
		else
		{
			if (pThis->mScreen != 0)
			{
				if (pThis->mTexture != 0)
				{
					DataXYZ lXYZ;
					DataUVM lUV;
					PointData lPoint;

					lXYZ.mTranslatedX = pThis->mPoint[1].mXYZ->mTranslatedX;
					lXYZ.mTranslatedY = pThis->mPoint[1].mXYZ->mTranslatedY;
					lXYZ.mTranslatedZ = pThis->mPoint[1].mXYZ->mTranslatedZ;
					lUV.u  = pThis->mPoint[1].mUVM->u;
					lUV.v  = pThis->mPoint[1].mUVM->v;

					lPoint.mXYZ = &lXYZ;
					lPoint.mUVM = &lUV;

					if (pThis->mOrthographicProjection == true ||
						pThis->ClipVectorNearZ(&pThis->mPoint[0],
									 &lPoint,
									 CLIP_UV,
									 pThis) != 3)
					{
						// The scale factor scales the coordinates to the viewport resolution.
						// This way, things will keep the same size no matter what screen resolution
						// you use. (And as you can see, the scale factor is actually the viewport width).
						const float lScale = (pThis->mViewportRight - pThis->mViewportLeft) * pThis->mScreenDistToOrigin;

						// Project the two points on the screen.
						float lP;
						if (pThis->mOrthographicProjection == true)
						{
							lP = lScale / pThis->mNearZ;
						}
						else
						{
							lP = lScale / pThis->mPoint[0].mXYZ->mTranslatedZ;
						}

						DataXYZ lProjectedXYZ[2];

						lProjectedXYZ[0].x = pThis->mPerspectiveCenterX + pThis->mPoint[0].mXYZ->mTranslatedX * lP;
						lProjectedXYZ[0].y = pThis->mPerspectiveCenterY - pThis->mPoint[0].mXYZ->mTranslatedY * lP;
						lProjectedXYZ[0].z = pThis->mPoint[0].mXYZ->mTranslatedZ;

						if (pThis->mOrthographicProjection == false)
						{
							lP = lScale / lPoint.mXYZ->mTranslatedZ;
						}
						lProjectedXYZ[1].x = pThis->mPerspectiveCenterX + lPoint.mXYZ->mTranslatedX * lP;
						lProjectedXYZ[1].y = pThis->mPerspectiveCenterY - lPoint.mXYZ->mTranslatedY * lP;
						lProjectedXYZ[1].z = lPoint.mXYZ->mTranslatedZ;

						// Clip the projected points against the clipping rect.
						PointData lProjectedPoint[2];
						lProjectedPoint[0].mXYZ = &lProjectedXYZ[0];
						lProjectedPoint[0].mUVM = pThis->mPoint[0].mUVM;
						lProjectedPoint[1].mXYZ = &lProjectedXYZ[1];
						lProjectedPoint[1].mUVM= lPoint.mUVM;

						DataXYZ lClippedXYZ[2];
						DataUVM lClippedUVM[2];
						PointData lClippedPoint[2];
						lClippedPoint[0].mXYZ = &lClippedXYZ[0];
						lClippedPoint[0].mUVM = &lClippedUVM[0];
						lClippedPoint[1].mXYZ = &lClippedXYZ[1];
						lClippedPoint[1].mUVM = &lClippedUVM[1];

						if (pThis->ClipLine(&lProjectedPoint[0],
											 &lProjectedPoint[1],
											 &lClippedPoint[0],
											 &lClippedPoint[1],
											 CLIP_UV) != 0)
						{
							// Render the line.
							Vertex3DUV lV[2];
							lV[0].SetX(lClippedPoint[0].mXYZ->x);
							lV[0].SetY(lClippedPoint[0].mXYZ->y);
							lV[0].SetZ(lClippedPoint[0].mXYZ->z);
							lV[0].SetU(lClippedPoint[0].mUVM->u * ((float)pThis->mTexture->GetColorMap(0)->GetWidth() - 1));
							lV[0].SetV(lClippedPoint[0].mUVM->v * ((float)pThis->mTexture->GetColorMap(0)->GetHeight() - 1));

							lV[1].SetX(lClippedPoint[1].mXYZ->x);
							lV[1].SetY(lClippedPoint[1].mXYZ->y);
							lV[1].SetZ(lClippedPoint[1].mXYZ->z);
							lV[1].SetU(lClippedPoint[1].mUVM->u * ((float)pThis->mTexture->GetColorMap(0)->GetWidth() - 1));
							lV[1].SetV(lClippedPoint[1].mUVM->v * ((float)pThis->mTexture->GetColorMap(0)->GetHeight() - 1));
							
							switch(pThis->mRenderingHint)
							{
							case RENDER_FAST:
								// TODO: Calculate the mipmap level.
								pThis->DrawUVLineFuncFast(lV, pThis->mTexture->GetColorMap(0), pThis);
								break;
							case RENDER_NICEST:
							default:
								pThis->DrawUVLineFunc(lV, pThis);
								break;
							};
						}
					}
				}
				else
				{
					RenderLinesSingleColor(pThis);
				}
			}
		}

		// Empty the vertex buffers...
		if (pThis->mCurrentXYZIndex >= 2)
		{
			DataXYZ* lTempXYZ = pThis->mPoint[0].mXYZ;
			pThis->mPoint[0].mXYZ = pThis->mPoint[1].mXYZ;
			pThis->mPoint[1].mXYZ = pThis->mPoint[2].mXYZ;
			pThis->mPoint[2].mXYZ = lTempXYZ;
			pThis->mCurrentXYZIndex -= 1;
		}

		if (pThis->mCurrentUVIndex >= 2)
		{
			DataUVM* lTempUVM = pThis->mPoint[0].mUVM;
			pThis->mPoint[0].mUVM= pThis->mPoint[1].mUVM;
			pThis->mPoint[1].mUVM= pThis->mPoint[2].mUVM;
			pThis->mPoint[2].mUVM= lTempUVM;
			pThis->mCurrentUVIndex -= 1;
		}
	}
}

void Software3DPainter::RenderLineStripUVRGB(Software3DPainter* pThis)
{
	if (pThis->mCurrentXYZIndex >= 2)
	{
		if (pThis->mCurrentRGBIndex < 2)
		{
			RenderLinesUV(pThis);
		}
		else if(pThis->mCurrentUVIndex < 2)
		{
			RenderLinesRGB(pThis);
		}
		else
		{
			if (pThis->mScreen != 0)
			{
				if (pThis->mTexture != 0)
				{
					DataXYZ lXYZ;
					DataUVM lUV;
					DataRGB lRGB;
					PointData lPoint;

					lXYZ.mTranslatedX = pThis->mPoint[1].mXYZ->mTranslatedX;
					lXYZ.mTranslatedY = pThis->mPoint[1].mXYZ->mTranslatedY;
					lXYZ.mTranslatedZ = pThis->mPoint[1].mXYZ->mTranslatedZ;
					lUV.u  = pThis->mPoint[1].mUVM->u;
					lUV.v  = pThis->mPoint[1].mUVM->v;
					lRGB.r = pThis->mPoint[1].mRGB->r;
					lRGB.g = pThis->mPoint[1].mRGB->g;
					lRGB.b = pThis->mPoint[1].mRGB->b;

					lPoint.mXYZ = &lXYZ;
					lPoint.mUVM = &lUV;
					lPoint.mRGB = &lRGB;

					if (pThis->mOrthographicProjection == true ||
						pThis->ClipVectorNearZ(&pThis->mPoint[0],
									 &lPoint,
									 (ClipFlags)(CLIP_RGB | CLIP_UV),
									 pThis) != 3)
					{
						// The scale factor scales the coordinates to the viewport resolution.
						// This way, things will keep the same size no matter what screen resolution
						// you use. (And as you can see, the scale factor is actually the viewport width).
						const float lScale = (pThis->mViewportRight - pThis->mViewportLeft) * pThis->mScreenDistToOrigin;

						// Project the two points on the screen.
						float lP;
						if (pThis->mOrthographicProjection == true)
						{
							lP = lScale / pThis->mNearZ;
						}
						else
						{
							lP = lScale / pThis->mPoint[0].mXYZ->mTranslatedZ;
						}

						DataXYZ lProjectedXYZ[2];

						lProjectedXYZ[0].x = pThis->mPerspectiveCenterX + pThis->mPoint[0].mXYZ->mTranslatedX * lP;
						lProjectedXYZ[0].y = pThis->mPerspectiveCenterY - pThis->mPoint[0].mXYZ->mTranslatedY * lP;
						lProjectedXYZ[0].z = pThis->mPoint[0].mXYZ->mTranslatedZ;

						if (pThis->mOrthographicProjection == false)
						{
							lP = lScale / lPoint.mXYZ->mTranslatedZ;
						}
						lProjectedXYZ[1].x = pThis->mPerspectiveCenterX + lPoint.mXYZ->mTranslatedX * lP;
						lProjectedXYZ[1].y = pThis->mPerspectiveCenterY - lPoint.mXYZ->mTranslatedY * lP;
						lProjectedXYZ[1].z = lPoint.mXYZ->mTranslatedZ;

						// Clip the projected points against the clipping rect.
						PointData lProjectedPoint[2];
						lProjectedPoint[0].mXYZ = &lProjectedXYZ[0];
						lProjectedPoint[0].mUVM = pThis->mPoint[0].mUVM;
						lProjectedPoint[0].mRGB = pThis->mPoint[0].mRGB;
						lProjectedPoint[1].mXYZ = &lProjectedXYZ[1];
						lProjectedPoint[1].mUVM = lPoint.mUVM;
						lProjectedPoint[1].mRGB = lPoint.mRGB;

						DataXYZ lClippedXYZ[2];
						DataRGB lClippedRGB[2];
						DataUVM lClippedUVM[2];
						PointData lClippedPoint[2];
						lClippedPoint[0].mXYZ = &lClippedXYZ[0];
						lClippedPoint[0].mUVM = &lClippedUVM[0];
						lClippedPoint[0].mRGB = &lClippedRGB[0];
						lClippedPoint[1].mXYZ = &lClippedXYZ[1];
						lClippedPoint[1].mUVM = &lClippedUVM[1];
						lClippedPoint[1].mRGB = &lClippedRGB[1];

						if (pThis->ClipLine(&lProjectedPoint[0],
											 &lProjectedPoint[1],
											 &lClippedPoint[0],
											 &lClippedPoint[1],
											 (ClipFlags)(CLIP_UV | CLIP_RGB)) != 0)
						{
							// Render the line.
							Vertex3DUVRGB lV[2];
							lV[0].SetX(lClippedPoint[0].mXYZ->x);
							lV[0].SetY(lClippedPoint[0].mXYZ->y);
							lV[0].SetZ(lClippedPoint[0].mXYZ->z);
							lV[0].SetU(lClippedPoint[0].mUVM->u * ((float)pThis->mTexture->GetColorMap(0)->GetWidth() - 1));
							lV[0].SetV(lClippedPoint[0].mUVM->v * ((float)pThis->mTexture->GetColorMap(0)->GetHeight() - 1));
							lV[0].SetR(lClippedPoint[0].mRGB->r);
							lV[0].SetG(lClippedPoint[0].mRGB->g);
							lV[0].SetB(lClippedPoint[0].mRGB->b);

							lV[1].SetX(lClippedPoint[1].mXYZ->x);
							lV[1].SetY(lClippedPoint[1].mXYZ->y);
							lV[1].SetZ(lClippedPoint[1].mXYZ->z);
							lV[1].SetU(lClippedPoint[1].mUVM->u * ((float)pThis->mTexture->GetColorMap(0)->GetWidth() - 1));
							lV[1].SetV(lClippedPoint[1].mUVM->v * ((float)pThis->mTexture->GetColorMap(0)->GetHeight() - 1));
							lV[1].SetR(lClippedPoint[1].mRGB->r);
							lV[1].SetG(lClippedPoint[1].mRGB->g);
							lV[1].SetB(lClippedPoint[1].mRGB->b);

							switch(pThis->mRenderingHint)
							{
							case RENDER_FAST:
								// TODO: Calculate the mipmap level.
								pThis->DrawUVRGBLineFuncFast(lV, pThis->mTexture->GetColorMap(0), pThis);
								break;
							case RENDER_NICEST:
							default:
								pThis->DrawUVRGBLineFunc(lV, pThis);
								break;
							};
						}
					}
				}
				else
				{
					RenderLinesSingleColor(pThis);
				}
			}
		}
		// Empty the vertex buffers...
		if (pThis->mCurrentXYZIndex >= 2)
		{
			DataXYZ* lTempXYZ = pThis->mPoint[0].mXYZ;
			pThis->mPoint[0].mXYZ = pThis->mPoint[1].mXYZ;
			pThis->mPoint[1].mXYZ = pThis->mPoint[2].mXYZ;
			pThis->mPoint[2].mXYZ = lTempXYZ;
			pThis->mCurrentXYZIndex -= 1;
		}

		if (pThis->mCurrentUVIndex >= 2)
		{
			DataUVM* lTempUVM = pThis->mPoint[0].mUVM;
			pThis->mPoint[0].mUVM= pThis->mPoint[1].mUVM;
			pThis->mPoint[1].mUVM= pThis->mPoint[2].mUVM;
			pThis->mPoint[2].mUVM= lTempUVM;
			pThis->mCurrentUVIndex -= 1;
		}

		if (pThis->mCurrentRGBIndex >= 2)
		{
			DataRGB* lTempRGB = pThis->mPoint[0].mRGB;
			pThis->mPoint[0].mRGB = pThis->mPoint[1].mRGB;
			pThis->mPoint[1].mRGB = pThis->mPoint[2].mRGB;
			pThis->mPoint[2].mRGB = lTempRGB;
			pThis->mCurrentRGBIndex -= 1;
		}
	}
}

} // End namespace.
