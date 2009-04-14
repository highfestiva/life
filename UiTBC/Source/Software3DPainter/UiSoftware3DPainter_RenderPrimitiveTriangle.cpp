/*
	Lepra::File:   Software3DPainter_RenderPrimitiveTriangle.cpp
	Class:  Software3DPainter
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand
*/

#include "../../Include/UiVertex.h"
#include "../../Include/UiSoftware3DPainter.h"
#include "../../Include/UiTBC.h"
#include "UiSoftware3DPainter_Macros.h"

#include <math.h>

#define MACRO_RESCALE_UV_COORDS(vertexCount, mipmapLevel) \
{ \
	for (int i = 0; i < vertexCount; i++) \
	{ \
		float lU = lProjectedVertex3D[i].GetUOverZ() * lProjectedVertex3D[i].GetZ(); \
		float lV = lProjectedVertex3D[i].GetVOverZ() * lProjectedVertex3D[i].GetZ(); \
 \
		lProjectedVertex3D[i].SetU(lU * (pThis->mTexture->GetColorMap(mipmapLevel)->GetWidth() - 1)); \
		lProjectedVertex3D[i].SetV(lV * (pThis->mTexture->GetColorMap(mipmapLevel)->GetHeight() - 1)); \
	} \
}

#define MACRO_RESCALE_UVM_COORDS(vertexCount, mipmapLevel) \
{ \
	for (int i = 0; i < vertexCount; i++) \
	{ \
		float lU = lProjectedVertex3D[i].GetUOverZ() * lProjectedVertex3D[i].GetZ(); \
		float lV = lProjectedVertex3D[i].GetVOverZ() * lProjectedVertex3D[i].GetZ(); \
 \
		lProjectedVertex3D[i].SetU(lU * (pThis->mTexture->GetColorMap(mipmapLevel)->GetWidth() - 1)); \
		lProjectedVertex3D[i].SetV(lV * (pThis->mTexture->GetColorMap(mipmapLevel)->GetHeight() - 1)); \
		lProjectedVertex3D[i].SetM(pThis->GetNicestMipMapLevel(lProjectedVertex3D[i].GetZ())); \
	} \
}

#define MACRO_DRAW_SINGLE_COLOR_TRIANGLE(a, b, c, projectedVertex3D) \
{ \
	int ia = a; \
	int ib = b; \
	int ic = c; \
 \
	if (pThis->mFaceOrientation == FACE_CCW) \
	{ \
		ib = c;	/* Change order. */\
		ic = b; \
	} \
 \
	Gradients lGradients(&projectedVertex3D[ia], \
						  &projectedVertex3D[ib], \
						  &projectedVertex3D[ic]); \
	EdgeZ* lEdge0 = &pThis->mEdgeZ[0]; \
	EdgeZ* lEdge1 = &pThis->mEdgeZ[1]; \
	EdgeZ* lEdge2 = &pThis->mEdgeZ[2]; \
	lEdge0->Init(&projectedVertex3D[ia], &projectedVertex3D[ib], &lGradients); \
	lEdge1->Init(&projectedVertex3D[ib], &projectedVertex3D[ic], &lGradients); \
	lEdge2->Init(&projectedVertex3D[ic], &projectedVertex3D[ia], &lGradients); \
\
	EdgeZ* lTopToBottomEdge; \
	EdgeZ* lTopToMiddleEdge; \
	EdgeZ* lMiddleToBottomEdge; \
	bool lMiddleIsLeft; \
	MACRO_SORT_EDGES(ia, ib, ic, projectedVertex3D); \
\
	pThis->DrawSingleColorTriangle(lTopToBottomEdge, lTopToMiddleEdge, lMiddleToBottomEdge, \
									 &lGradients, lMiddleIsLeft, lColor); \
}

// MACRO_DRAW_SHADED_TRIANGLE() is used by many different primitive renderers.
// It is used to render RGB-, UVRGB- and UVRGBM-triangles. All types that has the
// "gammaConvert"-option, that is.
#define MACRO_DRAW_SHADED_TRIANGLE(a, b, c, projectedVertex3D, gammaConvert, gradientsType, edgeType, edgeMember, triangleFunc) \
{ \
	int ia = a; \
	int ib = b; \
	int ic = c; \
 \
	if (pThis->mFaceOrientation == FACE_CCW) \
	{ \
		ib = c;	/* Change order. */\
		ic = b; \
	} \
 \
	gradientsType lGradients(&projectedVertex3D[ia], \
							  &projectedVertex3D[ib], \
							  &projectedVertex3D[ic], \
							  gammaConvert); \
	edgeType* lEdge0 = &pThis->edgeMember[0]; \
	edgeType* lEdge1 = &pThis->edgeMember[1]; \
	edgeType* lEdge2 = &pThis->edgeMember[2]; \
	lEdge0->Init(&projectedVertex3D[ia], &projectedVertex3D[ib], &lGradients, gammaConvert); \
	lEdge1->Init(&projectedVertex3D[ib], &projectedVertex3D[ic], &lGradients, gammaConvert); \
	lEdge2->Init(&projectedVertex3D[ic], &projectedVertex3D[ia], &lGradients, gammaConvert); \
\
	edgeType* lTopToBottomEdge; \
	edgeType* lTopToMiddleEdge; \
	edgeType* lMiddleToBottomEdge; \
	bool lMiddleIsLeft; \
	MACRO_SORT_EDGES(ia, ib, ic, projectedVertex3D); \
\
	pThis->triangleFunc(lTopToBottomEdge, lTopToMiddleEdge, lMiddleToBottomEdge, \
							 &lGradients, lMiddleIsLeft); \
}

// MACRO_DRAW_TEXTURED_TRIANGLE is used rendering UV-, and UVM-triangles.
#define MACRO_DRAW_TEXTURED_TRIANGLE(a, b, c, projectedVertex3D, gradientsType, edgeType, edgeMember, triangleFunc) \
{ \
	int ia = a; \
	int ib = b; \
	int ic = c; \
 \
	if (pThis->mFaceOrientation == FACE_CCW) \
	{ \
		ib = c;	/* Change order. */\
		ic = b; \
	} \
 \
	gradientsType lGradients(&projectedVertex3D[ia], \
							  &projectedVertex3D[ib], \
							  &projectedVertex3D[ic]); \
	edgeType* lEdge0 = &pThis->edgeMember[0]; \
	edgeType* lEdge1 = &pThis->edgeMember[1]; \
	edgeType* lEdge2 = &pThis->edgeMember[2]; \
	lEdge0->Init(&projectedVertex3D[ia], &projectedVertex3D[ib], &lGradients); \
	lEdge1->Init(&projectedVertex3D[ib], &projectedVertex3D[ic], &lGradients); \
	lEdge2->Init(&projectedVertex3D[ic], &projectedVertex3D[ia], &lGradients); \
\
	edgeType* lTopToBottomEdge; \
	edgeType* lTopToMiddleEdge; \
	edgeType* lMiddleToBottomEdge; \
	bool lMiddleIsLeft; \
	MACRO_SORT_EDGES(ia, ib, ic, projectedVertex3D); \
\
	pThis->triangleFunc(lTopToBottomEdge, lTopToMiddleEdge, lMiddleToBottomEdge, \
						  &lGradients, lMiddleIsLeft); \
}

#define MACRO_DRAW_UV_TRIANGLE_FAST(a, b, c, projectedVertex3D, m) \
{ \
	int ia = a; \
	int ib = b; \
	int ic = c; \
 \
	if (pThis->mFaceOrientation == FACE_CCW) \
	{ \
		ib = c;	/* Change order. */\
		ic = b; \
	} \
 \
	GradientsUV lGradients(&projectedVertex3D[ia], \
							 &projectedVertex3D[ib], \
							 &projectedVertex3D[ic]); \
	EdgeUV* lEdge0 = &pThis->mEdgeUV[0]; \
	EdgeUV* lEdge1 = &pThis->mEdgeUV[1]; \
	EdgeUV* lEdge2 = &pThis->mEdgeUV[2]; \
	lEdge0->Init(&projectedVertex3D[ia], &projectedVertex3D[ib], &lGradients); \
	lEdge1->Init(&projectedVertex3D[ib], &projectedVertex3D[ic], &lGradients); \
	lEdge2->Init(&projectedVertex3D[ic], &projectedVertex3D[ia], &lGradients); \
\
	EdgeUV* lTopToBottomEdge; \
	EdgeUV* lTopToMiddleEdge; \
	EdgeUV* lMiddleToBottomEdge; \
	bool lMiddleIsLeft; \
	MACRO_SORT_EDGES(ia, ib, ic, projectedVertex3D); \
\
	pThis->DrawUVTriangleFast(lTopToBottomEdge, lTopToMiddleEdge, lMiddleToBottomEdge, \
								&lGradients, lMiddleIsLeft, m); \
}

#define MACRO_DRAW_UVRGB_TRIANGLE_FAST(a, b, c, projectedVertex3D, m) \
{ \
	int ia = a; \
	int ib = b; \
	int ic = c; \
 \
	if (pThis->mFaceOrientation == FACE_CCW) \
	{ \
		ib = c;	/* Change order. */\
		ic = b; \
	} \
 \
	GradientsUVRGB lGradients(&projectedVertex3D[ia], \
							 &projectedVertex3D[ib], \
							 &projectedVertex3D[ic], \
							 false); \
	EdgeUVRGB* lEdge0 = &pThis->mEdgeUVRGB[0]; \
	EdgeUVRGB* lEdge1 = &pThis->mEdgeUVRGB[1]; \
	EdgeUVRGB* lEdge2 = &pThis->mEdgeUVRGB[2]; \
	lEdge0->Init(&projectedVertex3D[ia], &projectedVertex3D[ib], &lGradients, false); \
	lEdge1->Init(&projectedVertex3D[ib], &projectedVertex3D[ic], &lGradients, false); \
	lEdge2->Init(&projectedVertex3D[ic], &projectedVertex3D[ia], &lGradients, false); \
\
	EdgeUVRGB* lTopToBottomEdge; \
	EdgeUVRGB* lTopToMiddleEdge; \
	EdgeUVRGB* lMiddleToBottomEdge; \
	bool lMiddleIsLeft; \
	MACRO_SORT_EDGES(ia, ib, ic, projectedVertex3D); \
\
	pThis->DrawUVRGBTriangleFast(lTopToBottomEdge, lTopToMiddleEdge, lMiddleToBottomEdge, \
								   &lGradients, lMiddleIsLeft, m); \
}









namespace UiTbc
{

void Software3DPainter::RenderTrianglesSingleColor(Software3DPainter* pThis)
{
	bool lStatusOk = (pThis->mCurrentXYZIndex >= 3 && pThis->mScreen != 0);

	if (lStatusOk)
	{
		pThis->ProcessLights();

		Lepra::Color lColor;
		if (pThis->mCurrentRGBIndex >= 1)
		{
			lColor.mRed   = (Lepra::uint8)(pThis->mPoint[0].mRGB->mLightProcessedR * 255.0f);
			lColor.mGreen = (Lepra::uint8)(pThis->mPoint[0].mRGB->mLightProcessedG * 255.0f);
			lColor.mBlue  = (Lepra::uint8)(pThis->mPoint[0].mRGB->mLightProcessedB * 255.0f);
		}

		Vertex3D lProjectedVertex3D[8];
		Vertex3D* lV[8];
		lV[0] = &lProjectedVertex3D[0];
		lV[1] = &lProjectedVertex3D[1];
		lV[2] = &lProjectedVertex3D[2];
		lV[3] = &lProjectedVertex3D[3];
		lV[4] = &lProjectedVertex3D[4];
		lV[5] = &lProjectedVertex3D[5];
		lV[6] = &lProjectedVertex3D[6];
		lV[7] = &lProjectedVertex3D[7];

		int lVertex3DCount = ClipAndProjectTriangle(lV, pThis, (ClipFlags)0, SetVertex3DData);

		if (lVertex3DCount >= 3)
		{
			bool lVisible = CheckVisible(lV, lVertex3DCount, pThis);

			if (lVisible == true)
			{
				for (int i = 0; i < lVertex3DCount - 2; i++)
				{
					MACRO_DRAW_SINGLE_COLOR_TRIANGLE(0, i + 1, i + 2, lProjectedVertex3D);
				}
			}
		}
	}

	if(pThis->mCurrentXYZIndex >= 3)
	{
		// Empty the vertex buffers...
		pThis->mCurrentXYZIndex -= 3;
	}
}





void Software3DPainter::RenderTrianglesRGB(Software3DPainter* pThis)
{
	bool lStatusOk = (pThis->mCurrentXYZIndex >= 3);
	bool lEmptyVertexBuffers = false;
	if (lStatusOk)
	{
		pThis->ProcessLights();
		lStatusOk = (pThis->mCurrentRGBIndex >= 3);

		if(!lStatusOk)
		{
			RenderTrianglesSingleColor(pThis);
		}
		else
		{
			lEmptyVertexBuffers = true;
		}
	}

	if (lStatusOk)
	{
		lStatusOk = (pThis->mScreen != 0);
	}

	Vertex3DRGB lProjectedVertex3D[8];
	Vertex3D* lV[8];
	int lVertex3DCount = 0;

	if (lStatusOk)
	{
		lV[0] = &lProjectedVertex3D[0];
		lV[1] = &lProjectedVertex3D[1];
		lV[2] = &lProjectedVertex3D[2];
		lV[3] = &lProjectedVertex3D[3];
		lV[4] = &lProjectedVertex3D[4];
		lV[5] = &lProjectedVertex3D[5];
		lV[6] = &lProjectedVertex3D[6];
		lV[7] = &lProjectedVertex3D[7];
		lVertex3DCount = ClipAndProjectTriangle(lV, pThis, CLIP_RGB, SetVertex3DRGBData);

		lStatusOk = (lVertex3DCount >= 3);
	}

	if (lStatusOk)
	{
		lStatusOk = CheckVisible(lV, lVertex3DCount, pThis);
	}

	if (lStatusOk)
	{
		if (pThis->mRenderingHint == RENDER_NICEST)
		{
			for (int i = 0; i < lVertex3DCount - 2; i++)
			{
				MACRO_DRAW_SHADED_TRIANGLE(0, i + 1, i + 2, lProjectedVertex3D, true, GradientsRGB, EdgeRGB, mEdgeRGB, DrawRGBTriangle);
			}
		}
		else
		{
			for (int i = 0; i < lVertex3DCount - 2; i++)
			{
				MACRO_DRAW_SHADED_TRIANGLE(0, i + 1, i + 2, lProjectedVertex3D, false, GradientsRGB, EdgeRGB, mEdgeRGB, DrawRGBTriangle);
			}
		}
	}

	if (lEmptyVertexBuffers)
	{
		// Empty the vertex buffers...
		pThis->mCurrentXYZIndex -= 3;
		pThis->mCurrentRGBIndex -= 3;
		pThis->mCurrentUVIndex = 0;
		pThis->mCurrentNormalIndex = 0;
	}
}





void Software3DPainter::RenderTrianglesUV(Software3DPainter* pThis)
{
	bool lStatusOk = (pThis->mCurrentXYZIndex >= 3);
	bool lEmptyVertexBuffers = false;

	if (lStatusOk)
	{
		lStatusOk = (pThis->mCurrentUVIndex >= 3);

		if(!lStatusOk)
		{
			RenderTrianglesSingleColor(pThis);
		}
		else
		{
			lEmptyVertexBuffers = true;
		}
	}

	if (lStatusOk)
	{
		lStatusOk = (pThis->mTexture != 0);

		if(!lStatusOk)
		{
			RenderTrianglesSingleColor(pThis);
		}
	}

	if (lStatusOk)
	{
		lStatusOk = (pThis->mScreen != 0);
	}

	Vertex3DUV lProjectedVertex3D[8];
	Vertex3D* lV[8];
	int lVertex3DCount = 0;

	if (lStatusOk)
	{
		lV[0] = &lProjectedVertex3D[0];
		lV[1] = &lProjectedVertex3D[1];
		lV[2] = &lProjectedVertex3D[2];
		lV[3] = &lProjectedVertex3D[3];
		lV[4] = &lProjectedVertex3D[4];
		lV[5] = &lProjectedVertex3D[5];
		lV[6] = &lProjectedVertex3D[6];
		lV[7] = &lProjectedVertex3D[7];
		int lVertex3DCount = ClipAndProjectTriangle(lV, pThis, CLIP_UV, SetVertex3DUVData);

		lStatusOk = (lVertex3DCount >= 3);
	}
	if (lStatusOk)
	{
		lStatusOk = CheckVisible(lV, lVertex3DCount, pThis);
	}

	if (lStatusOk)
	{
		int i;
		MACRO_RESCALE_UV_COORDS(lVertex3DCount, 0);

		if (pThis->mRenderingHint == RENDER_NICEST)
		{
			for (i = 0; i < lVertex3DCount - 2; i++)
			{
				MACRO_DRAW_TEXTURED_TRIANGLE(0, i + 1, i + 2, lProjectedVertex3D, GradientsUV, EdgeUV, mEdgeUV, DrawUVTriangle);
			}
		}
		else
		{
			for (i = 0; i < lVertex3DCount - 2; i++)
			{
				MACRO_DRAW_UV_TRIANGLE_FAST(0, i + 1, i + 2, lProjectedVertex3D, 0);
			}
		}
	}

	if (lEmptyVertexBuffers)
	{
		// Empty the vertex buffers...
		pThis->mCurrentXYZIndex -= 3;
		pThis->mCurrentUVIndex -= 3;
	}
}





void Software3DPainter::RenderTrianglesUVM(Software3DPainter* pThis)
{
	bool lStatusOk = (pThis->mCurrentXYZIndex >= 3);
	bool lEmptyVertexBuffers = false;

	if (lStatusOk)
	{
		lStatusOk = (pThis->mCurrentUVIndex >= 3);

		if(!lStatusOk)
		{
			RenderTrianglesSingleColor(pThis);
		}
		else
		{
			lEmptyVertexBuffers = true;
		}
	}

	if (lStatusOk)
	{
		lStatusOk = (pThis->mTexture != 0);

		if(!lStatusOk)
		{
			RenderTrianglesSingleColor(pThis);
		}
	}

	if (lStatusOk)
	{
		lStatusOk = (pThis->mScreen != 0);
	}

	if (pThis->mRenderingHint == RENDER_NICEST)
	{
		Vertex3DUVM lProjectedVertex3D[8];
		Vertex3D* lV[8];
		int lVertex3DCount = 0;

		if (lStatusOk)
		{
			lV[0] = &lProjectedVertex3D[0];
			lV[1] = &lProjectedVertex3D[1];
			lV[2] = &lProjectedVertex3D[2];
			lV[3] = &lProjectedVertex3D[3];
			lV[4] = &lProjectedVertex3D[4];
			lV[5] = &lProjectedVertex3D[5];
			lV[6] = &lProjectedVertex3D[6];
			lV[7] = &lProjectedVertex3D[7];
			int lVertex3DCount = ClipAndProjectTriangle(lV, pThis, CLIP_UV, SetVertex3DUVMData);

			lStatusOk = (lVertex3DCount >= 3);
		}

		if (lStatusOk)
		{
			lStatusOk = CheckVisible(lV, lVertex3DCount, pThis);
		}

		if (lStatusOk)
		{
			int i;

			MACRO_RESCALE_UVM_COORDS(lVertex3DCount, 0);

			for (i = 0; i < lVertex3DCount - 2; i++)
			{
				MACRO_DRAW_TEXTURED_TRIANGLE(0, i + 1, i + 2, lProjectedVertex3D, GradientsUVM, EdgeUVM, mEdgeUVM, DrawUVMTriangle);
			}
		}
	}
	else
	{
		Vertex3DUV lProjectedVertex3D[8];
		Vertex3D* lV[8];
		int lVertex3DCount = 0;

		if (lStatusOk)
		{
			lV[0] = &lProjectedVertex3D[0];
			lV[1] = &lProjectedVertex3D[1];
			lV[2] = &lProjectedVertex3D[2];
			lV[3] = &lProjectedVertex3D[3];
			lV[4] = &lProjectedVertex3D[4];
			lV[5] = &lProjectedVertex3D[5];
			lV[6] = &lProjectedVertex3D[6];
			lV[7] = &lProjectedVertex3D[7];
			lVertex3DCount = ClipAndProjectTriangle(lV, pThis, CLIP_UV, SetVertex3DUVData);

			lStatusOk = (lVertex3DCount >= 3);
		}

		if (lStatusOk)
		{
			lStatusOk = CheckVisible(lV, lVertex3DCount, pThis);
		}

		if (lStatusOk)
		{
			int lMipMapLevel = pThis->GetFastMipMapLevel(lV, lVertex3DCount, GetU_UV, GetV_UV);

			MACRO_RESCALE_UV_COORDS(lVertex3DCount, lMipMapLevel);

			for (int i = 0; i < lVertex3DCount - 2; i++)
			{
				MACRO_DRAW_UV_TRIANGLE_FAST(0, i + 1, i + 2, lProjectedVertex3D, lMipMapLevel);
			}
		}
	}

	if (lEmptyVertexBuffers)
	{
		// Empty the vertex buffers...
		pThis->mCurrentXYZIndex -= 3;
		pThis->mCurrentUVIndex -= 3;
	}
}





void Software3DPainter::RenderTrianglesUVRGB(Software3DPainter* pThis)
{
	bool lStatusOk = (pThis->mCurrentXYZIndex >= 3);
	bool lEmptyVertexBuffers = false;

	if (lStatusOk)
	{
		lStatusOk = (pThis->mCurrentUVIndex >= 3);

		if (!lStatusOk)
		{
			RenderTrianglesRGB(pThis);
		}
	}

	if (lStatusOk)
	{
		pThis->ProcessLights();
		lStatusOk = (pThis->mCurrentRGBIndex >= 3);

		if (!lStatusOk)
		{
			RenderTrianglesUV(pThis);
		}
		else
		{
			lEmptyVertexBuffers = true;
		}
	}

	if (lStatusOk)
	{
		lStatusOk = (pThis->mTexture != 0);
	}

	if (lStatusOk)
	{
		lStatusOk = (pThis->mScreen != 0);
	}

	Vertex3DUVRGB lProjectedVertex3D[8];
	Vertex3D* lV[8];
	int lVertex3DCount = 0;
	if (lStatusOk)
	{
		lV[0] = &lProjectedVertex3D[0];
		lV[1] = &lProjectedVertex3D[1];
		lV[2] = &lProjectedVertex3D[2];
		lV[3] = &lProjectedVertex3D[3];
		lV[4] = &lProjectedVertex3D[4];
		lV[5] = &lProjectedVertex3D[5];
		lV[6] = &lProjectedVertex3D[6];
		lV[7] = &lProjectedVertex3D[7];
		lVertex3DCount = ClipAndProjectTriangle(lV, pThis, (ClipFlags)(CLIP_UV | CLIP_RGB), SetVertex3DUVRGBData);

		lStatusOk = (lVertex3DCount >= 3);
	}

	if (lStatusOk)
	{
		lStatusOk = CheckVisible(lV, lVertex3DCount, pThis);
	}

	if (lStatusOk)
	{
		int i;

		MACRO_RESCALE_UV_COORDS(lVertex3DCount, 0);

		if (pThis->mRenderingHint == RENDER_NICEST)
		{
			for (i = 0; i < lVertex3DCount - 2; i++)
			{
				MACRO_DRAW_SHADED_TRIANGLE(0, i + 1, i + 2, lProjectedVertex3D, true, GradientsUVRGB, EdgeUVRGB, mEdgeUVRGB, DrawUVRGBTriangle);
			}
		}
		else
		{
			for (i = 0; i < lVertex3DCount - 2; i++)
			{
				MACRO_DRAW_UVRGB_TRIANGLE_FAST(0, i + 1, i + 2, lProjectedVertex3D, 0);
			}
		}
	}

	if (lEmptyVertexBuffers)
	{
		// Empty the vertex buffers...
		pThis->mCurrentXYZIndex -= 3;
		pThis->mCurrentUVIndex -= 3;
		pThis->mCurrentRGBIndex -= 3;
		pThis->mCurrentNormalIndex = 0;
	}
}





void Software3DPainter::RenderTrianglesUVRGBM(Software3DPainter* pThis)
{
	bool lStatusOk = (pThis->mCurrentXYZIndex >= 3);
	bool lEmptyVertexBuffers = false;

	if (lStatusOk)
	{
		lStatusOk = (pThis->mCurrentUVIndex >= 3);

		if (!lStatusOk)
		{
			RenderTrianglesRGB(pThis);
		}
	}

	if (lStatusOk)
	{
		pThis->ProcessLights();

		lStatusOk = (pThis->mCurrentRGBIndex >= 3);

		if (!lStatusOk)
		{
			RenderTrianglesUVM(pThis);
		}
		else
		{
			lEmptyVertexBuffers = true;
		}
	}

	if (lStatusOk)
	{
		lStatusOk = (pThis->mTexture != 0 && pThis->mScreen != 0);
	}

	if (lStatusOk)
	{
		if (pThis->mRenderingHint == RENDER_NICEST)
		{
			Vertex3DUVRGBM lProjectedVertex3D[8];
			Vertex3D* lV[8];
			lV[0] = &lProjectedVertex3D[0];
			lV[1] = &lProjectedVertex3D[1];
			lV[2] = &lProjectedVertex3D[2];
			lV[3] = &lProjectedVertex3D[3];
			lV[4] = &lProjectedVertex3D[4];
			lV[5] = &lProjectedVertex3D[5];
			lV[6] = &lProjectedVertex3D[6];
			lV[7] = &lProjectedVertex3D[7];

			int lVertex3DCount = ClipAndProjectTriangle(lV, pThis, (ClipFlags)(CLIP_UV | CLIP_RGB), SetVertex3DUVRGBMData);
			lStatusOk = (lVertex3DCount >= 3);

			if(lStatusOk)
			{
				lStatusOk = CheckVisible(lV, lVertex3DCount, pThis);
			}

			if (lStatusOk)
			{
				MACRO_RESCALE_UVM_COORDS(lVertex3DCount, 0);

				for (int i = 0; i < lVertex3DCount - 2; i++)
				{
					MACRO_DRAW_SHADED_TRIANGLE(0, i + 1, i + 2, lProjectedVertex3D, true, GradientsUVRGBM, EdgeUVRGBM, mEdgeUVRGBM, DrawUVRGBMTriangle);
				}
			}
		}
		else
		{
			Vertex3DUVRGB lProjectedVertex3D[8];
			Vertex3D* lV[8];
			lV[0] = &lProjectedVertex3D[0];
			lV[1] = &lProjectedVertex3D[1];
			lV[2] = &lProjectedVertex3D[2];
			lV[3] = &lProjectedVertex3D[3];
			lV[4] = &lProjectedVertex3D[4];
			lV[5] = &lProjectedVertex3D[5];
			lV[6] = &lProjectedVertex3D[6];
			lV[7] = &lProjectedVertex3D[7];

			int lVertex3DCount = ClipAndProjectTriangle(lV, pThis, (ClipFlags)(CLIP_UV | CLIP_RGB), SetVertex3DUVRGBData);

			lStatusOk = (lVertex3DCount >= 3);

			if (lStatusOk)
			{
				lStatusOk = CheckVisible(lV, lVertex3DCount, pThis);
			}

			if (lStatusOk)
			{
				int lMipMapLevel = pThis->GetFastMipMapLevel(lV, lVertex3DCount, GetU_UVRGB, GetV_UVRGB);

				MACRO_RESCALE_UV_COORDS(lVertex3DCount, lMipMapLevel);

				for (int i = 0; i < lVertex3DCount - 2; i++)
				{
					MACRO_DRAW_UVRGB_TRIANGLE_FAST(0, i + 1, i + 2, lProjectedVertex3D, lMipMapLevel);
				}
			}
		}
	}


	if (lEmptyVertexBuffers)
	{
		// Empty the vertex buffers...
		pThis->mCurrentXYZIndex -= 3;
		pThis->mCurrentUVIndex -= 3;
		pThis->mCurrentRGBIndex -= 3;
		pThis->mCurrentNormalIndex = 0;
	}
}










void Software3DPainter::RenderTriangleStripSingleColor(Software3DPainter* pThis)
{
	if (pThis->mCurrentXYZIndex >= 3)
	{
		if (pThis->mScreen != 0)
		{
			pThis->ProcessLights();

			Lepra::Color lColor;
			if (pThis->mCurrentRGBIndex >= 1)
			{
				lColor.mRed   = (Lepra::uint8)(pThis->mPoint[0].mRGB->mLightProcessedR * 255.0f);
				lColor.mGreen = (Lepra::uint8)(pThis->mPoint[0].mRGB->mLightProcessedG * 255.0f);
				lColor.mBlue  = (Lepra::uint8)(pThis->mPoint[0].mRGB->mLightProcessedB * 255.0f);
			}

			Vertex3D lProjectedVertex3D[8];
			Vertex3D* lV[8];
			lV[0] = &lProjectedVertex3D[0];
			lV[1] = &lProjectedVertex3D[1];
			lV[2] = &lProjectedVertex3D[2];
			lV[3] = &lProjectedVertex3D[3];
			lV[4] = &lProjectedVertex3D[4];
			lV[5] = &lProjectedVertex3D[5];
			lV[6] = &lProjectedVertex3D[6];
			lV[7] = &lProjectedVertex3D[7];
			int lVertex3DCount = ClipAndProjectTriangle(lV, pThis, (ClipFlags)0, SetVertex3DData);

			if (lVertex3DCount >= 3)
			{
				bool lVisible = CheckVisible(lV, lVertex3DCount, pThis);

				if (lVisible == true)
				{
					if (pThis->mTriangleStripInvert == true)
					{
						for (int i = 0; i < lVertex3DCount - 2; i++)
						{
							MACRO_DRAW_SINGLE_COLOR_TRIANGLE(i + 1, 0, i + 2, lProjectedVertex3D);
						}

						pThis->mTriangleStripInvert = false;
					}
					else
					{
						for (int i = 0; i < lVertex3DCount - 2; i++)
						{
							MACRO_DRAW_SINGLE_COLOR_TRIANGLE(0, i + 1, i + 2, lProjectedVertex3D);
						}

						pThis->mTriangleStripInvert = true;
					}
				}
			}
		}

		// Empty the vertex buffers...
		pThis->PrepareNextTriangleStripPointDataXYZ();
	}
}





void Software3DPainter::RenderTriangleStripRGB(Software3DPainter* pThis)
{
	if (pThis->mCurrentXYZIndex >= 3)
	{
		pThis->ProcessLights();

		if (pThis->mCurrentRGBIndex >= 3)
		{
			if (pThis->mScreen != 0)
			{
				Vertex3DRGB lProjectedVertex3D[8];
				Vertex3D* lV[8];
				lV[0] = &lProjectedVertex3D[0];
				lV[1] = &lProjectedVertex3D[1];
				lV[2] = &lProjectedVertex3D[2];
				lV[3] = &lProjectedVertex3D[3];
				lV[4] = &lProjectedVertex3D[4];
				lV[5] = &lProjectedVertex3D[5];
				lV[6] = &lProjectedVertex3D[6];
				lV[7] = &lProjectedVertex3D[7];
				int lVertex3DCount = ClipAndProjectTriangle(lV, pThis, CLIP_RGB, SetVertex3DRGBData);

				if (lVertex3DCount >= 3)
				{
					bool lVisible = CheckVisible(lV, lVertex3DCount, pThis);

					if (lVisible == true)
					{
						bool lGammaConvert = (pThis->mRenderingHint == RENDER_NICEST);

						if (pThis->mTriangleStripInvert == true)
						{
							for (int i = 0; i < lVertex3DCount - 2; i++)
							{
								MACRO_DRAW_SHADED_TRIANGLE(i + 1, 0, i + 2, lProjectedVertex3D, lGammaConvert, GradientsRGB, EdgeRGB, mEdgeRGB, DrawRGBTriangle);
							}
							pThis->mTriangleStripInvert = false;
						}
						else
						{
							for (int i = 0; i < lVertex3DCount - 2; i++)
							{
								MACRO_DRAW_SHADED_TRIANGLE(0, i + 1, i + 2, lProjectedVertex3D, lGammaConvert, GradientsRGB, EdgeRGB, mEdgeRGB, DrawRGBTriangle);
							}
							pThis->mTriangleStripInvert = true;
						}
					}
				}
			}

			// Empty the vertex buffers...
			pThis->PrepareNextTriangleStripPointDataXYZ();
			pThis->PrepareNextTriangleStripPointDataRGB();
			pThis->PrepareNextTriangleStripPointDataNormal();
		}
		else
		{
			RenderTriangleStripSingleColor(pThis);
		}
	}
}





void Software3DPainter::RenderTriangleStripUV(Software3DPainter* pThis)
{
	if (pThis->mCurrentXYZIndex >= 3)
	{
		if (pThis->mCurrentUVIndex >= 3)
		{
			if (pThis->mScreen != 0)
			{
				Vertex3DUV lProjectedVertex3D[8];
				Vertex3D* lV[8];
				lV[0] = &lProjectedVertex3D[0];
				lV[1] = &lProjectedVertex3D[1];
				lV[2] = &lProjectedVertex3D[2];
				lV[3] = &lProjectedVertex3D[3];
				lV[4] = &lProjectedVertex3D[4];
				lV[5] = &lProjectedVertex3D[5];
				lV[6] = &lProjectedVertex3D[6];
				lV[7] = &lProjectedVertex3D[7];
				int lVertex3DCount = ClipAndProjectTriangle(lV, pThis, CLIP_UV, SetVertex3DUVData);

				if (lVertex3DCount >= 3)
				{
					bool lVisible = CheckVisible(lV, lVertex3DCount, pThis);

					if (lVisible == true)
					{
						int i;

						MACRO_RESCALE_UV_COORDS(lVertex3DCount, 0);

						if (pThis->mTriangleStripInvert == true)
						{
							if (pThis->mRenderingHint == RENDER_NICEST)
							{
								for (i = 0; i < lVertex3DCount - 2; i++)
								{
									MACRO_DRAW_TEXTURED_TRIANGLE(i + 1, 0, i + 2, lProjectedVertex3D, GradientsUV, EdgeUV, mEdgeUV, DrawUVTriangle);
								}
							}
							else
							{
								for (i = 0; i < lVertex3DCount - 2; i++)
								{
									MACRO_DRAW_UV_TRIANGLE_FAST(i + 1, 0, i + 2, lProjectedVertex3D, 0);
								}
							}
							pThis->mTriangleStripInvert = false;
						}
						else
						{
							if (pThis->mRenderingHint == RENDER_NICEST)
							{
								for (i = 0; i < lVertex3DCount - 2; i++)
								{
									MACRO_DRAW_TEXTURED_TRIANGLE(0, i + 1, i + 2, lProjectedVertex3D, GradientsUV, EdgeUV, mEdgeUV, DrawUVTriangle);
								}
							}
							else
							{
								for (i = 0; i < lVertex3DCount - 2; i++)
								{
									MACRO_DRAW_UV_TRIANGLE_FAST(0, i + 1, i + 2, lProjectedVertex3D, 0);
								}
							}

							pThis->mTriangleStripInvert = true;
						}
					}
				}
			}

			// Empty the vertex buffers...
			pThis->PrepareNextTriangleStripPointDataXYZ();
			pThis->PrepareNextTriangleStripPointDataUVM();
		}
		else
		{
			RenderTriangleStripSingleColor(pThis);
		}
	}
}





void Software3DPainter::RenderTriangleStripUVM(Software3DPainter* pThis)
{
	if (pThis->mCurrentXYZIndex >= 3)
	{
		if (pThis->mCurrentUVIndex >= 3)
		{
			if (pThis->mScreen != 0)
			{
				if (pThis->mRenderingHint == RENDER_NICEST)
				{
					Vertex3DUVM lProjectedVertex3D[8];
					Vertex3D* lV[8];
					lV[0] = &lProjectedVertex3D[0];
					lV[1] = &lProjectedVertex3D[1];
					lV[2] = &lProjectedVertex3D[2];
					lV[3] = &lProjectedVertex3D[3];
					lV[4] = &lProjectedVertex3D[4];
					lV[5] = &lProjectedVertex3D[5];
					lV[6] = &lProjectedVertex3D[6];
					lV[7] = &lProjectedVertex3D[7];
					int lVertex3DCount = ClipAndProjectTriangle(lV, pThis, CLIP_UV, SetVertex3DUVMData);

					if (lVertex3DCount >= 3)
					{
						bool lVisible = CheckVisible(lV, lVertex3DCount, pThis);

						if (lVisible == true)
						{
							int i;

							MACRO_RESCALE_UVM_COORDS(lVertex3DCount, 0);

							if (pThis->mTriangleStripInvert == true)
							{
								for (i = 0; i < lVertex3DCount - 2; i++)
								{
									MACRO_DRAW_TEXTURED_TRIANGLE(i + 1, 0, i + 2, lProjectedVertex3D, GradientsUVM, EdgeUVM, mEdgeUVM, DrawUVMTriangle);
								}
			
								pThis->mTriangleStripInvert = false;
							}
							else
							{
								for (i = 0; i < lVertex3DCount - 2; i++)
								{
									MACRO_DRAW_TEXTURED_TRIANGLE(0, i + 1, i + 2, lProjectedVertex3D, GradientsUVM, EdgeUVM, mEdgeUVM, DrawUVMTriangle);
								}
								pThis->mTriangleStripInvert = true;
							}
						}
					}
				}// End if(RENDER_NICEST)
				else
				{
					Vertex3DUV lProjectedVertex3D[8];
					Vertex3D* lV[8];
					lV[0] = &lProjectedVertex3D[0];
					lV[1] = &lProjectedVertex3D[1];
					lV[2] = &lProjectedVertex3D[2];
					lV[3] = &lProjectedVertex3D[3];
					lV[4] = &lProjectedVertex3D[4];
					lV[5] = &lProjectedVertex3D[5];
					lV[6] = &lProjectedVertex3D[6];
					lV[7] = &lProjectedVertex3D[7];
					int lVertex3DCount = ClipAndProjectTriangle(lV, pThis, CLIP_UV, SetVertex3DUVData);
					if (lVertex3DCount >= 3)
					{
						bool lVisible = CheckVisible(lV, lVertex3DCount, pThis);

						if (lVisible == true)
						{
							int i;
							int lMipMapLevel = pThis->GetFastMipMapLevel(lV, lVertex3DCount, GetU_UV, GetV_UV);

							MACRO_RESCALE_UV_COORDS(lVertex3DCount, lMipMapLevel);

							if (pThis->mTriangleStripInvert == true)
							{
								for (i = 0; i < lVertex3DCount - 2; i++)
								{
									MACRO_DRAW_UV_TRIANGLE_FAST(i + 1, 0, i + 2, lProjectedVertex3D, lMipMapLevel);
								}
			
								pThis->mTriangleStripInvert = false;
							}
							else
							{
								for (i = 0; i < lVertex3DCount - 2; i++)
								{
									MACRO_DRAW_UV_TRIANGLE_FAST(0, i + 1, i + 2, lProjectedVertex3D, lMipMapLevel);
								}
								pThis->mTriangleStripInvert = true;
							}
						}
					}
				}//End else
			}

			// Empty the vertex buffers...
			pThis->PrepareNextTriangleStripPointDataXYZ();
			pThis->PrepareNextTriangleStripPointDataUVM();
		}
		else
		{
			RenderTriangleStripSingleColor(pThis);
		}
	}
}





void Software3DPainter::RenderTriangleStripUVRGB(Software3DPainter* pThis)
{
	if (pThis->mCurrentXYZIndex >= 3)
	{
		if (pThis->mCurrentUVIndex >= 3)
		{
			pThis->ProcessLights();

			if (pThis->mCurrentRGBIndex >= 3)
			{
				if (pThis->mTexture != 0)
				{
					if (pThis->mScreen != 0)
					{
						Vertex3DUVRGB lProjectedVertex3D[8];
						Vertex3D* lV[8];
						lV[0] = &lProjectedVertex3D[0];
						lV[1] = &lProjectedVertex3D[1];
						lV[2] = &lProjectedVertex3D[2];
						lV[3] = &lProjectedVertex3D[3];
						lV[4] = &lProjectedVertex3D[4];
						lV[5] = &lProjectedVertex3D[5];
						lV[6] = &lProjectedVertex3D[6];
						lV[7] = &lProjectedVertex3D[7];
						int lVertex3DCount = ClipAndProjectTriangle(lV, pThis, (ClipFlags)(CLIP_UV | CLIP_RGB), SetVertex3DUVRGBData);

						if (lVertex3DCount >= 3)
						{
							bool lVisible = CheckVisible(lV, lVertex3DCount, pThis);

							if (lVisible == true)
							{
								int i;

								MACRO_RESCALE_UV_COORDS(lVertex3DCount, 0);

								if (pThis->mTriangleStripInvert == true)
								{
									if (pThis->mRenderingHint == RENDER_NICEST)
									{
										for (i = 0; i < lVertex3DCount - 2; i++)
										{
											MACRO_DRAW_SHADED_TRIANGLE(i + 1, 0, i + 2, lProjectedVertex3D, true, GradientsUVRGB, EdgeUVRGB, mEdgeUVRGB, DrawUVRGBTriangle);
										}
									}
									else
									{
										for (i = 0; i < lVertex3DCount - 2; i++)
										{
											MACRO_DRAW_UVRGB_TRIANGLE_FAST(i + 1, 0, i + 2, lProjectedVertex3D, 0);
										}
									}

									pThis->mTriangleStripInvert = false;
								}
								else
								{
									if (pThis->mRenderingHint == RENDER_NICEST)
									{
										for (i = 0; i < lVertex3DCount - 2; i++)
										{
											MACRO_DRAW_SHADED_TRIANGLE(0, i + 1, i + 2, lProjectedVertex3D, true, GradientsUVRGB, EdgeUVRGB, mEdgeUVRGB, DrawUVRGBTriangle);
										}
									}
									else
									{
										for (i = 0; i < lVertex3DCount - 2; i++)
										{
											MACRO_DRAW_UVRGB_TRIANGLE_FAST(0, i + 1, i + 2, lProjectedVertex3D, 0);
										}
									}

									pThis->mTriangleStripInvert = true;
								}
							}
						}
					}

					// Empty the vertex buffers...
					pThis->PrepareNextTriangleStripPointDataXYZ();
					pThis->PrepareNextTriangleStripPointDataUVM();
					pThis->PrepareNextTriangleStripPointDataRGB();
					pThis->PrepareNextTriangleStripPointDataNormal();
				}
			}
			else
			{
				RenderTriangleStripUV(pThis);
			}
		}
		else
		{
			RenderTriangleStripRGB(pThis);
		}
	}
}





void Software3DPainter::RenderTriangleStripUVRGBM(Software3DPainter* pThis)
{
	if (pThis->mCurrentXYZIndex >= 3)
	{
		if (pThis->mCurrentUVIndex >= 3)
		{
			pThis->ProcessLights();

			if (pThis->mCurrentRGBIndex >= 3)
			{
				if (pThis->mTexture != 0)
				{
					if (pThis->mScreen != 0)
					{
						if (pThis->mRenderingHint == RENDER_NICEST)
						{
							Vertex3DUVRGBM lProjectedVertex3D[8];
							Vertex3D* lV[8];
							lV[0] = &lProjectedVertex3D[0];
							lV[1] = &lProjectedVertex3D[1];
							lV[2] = &lProjectedVertex3D[2];
							lV[3] = &lProjectedVertex3D[3];
							lV[4] = &lProjectedVertex3D[4];
							lV[5] = &lProjectedVertex3D[5];
							lV[6] = &lProjectedVertex3D[6];
							lV[7] = &lProjectedVertex3D[7];
							int lVertex3DCount = ClipAndProjectTriangle(lV, pThis, (ClipFlags)(CLIP_UV | CLIP_RGB), SetVertex3DUVRGBMData);
							if (lVertex3DCount >= 3)
							{
								bool lVisible = CheckVisible(lV, lVertex3DCount, pThis);

								if (lVisible == true)
								{
									int i;

									MACRO_RESCALE_UVM_COORDS(lVertex3DCount, 0);

									if (pThis->mTriangleStripInvert == true)
									{
										for (i = 0; i < lVertex3DCount - 2; i++)
										{
											MACRO_DRAW_SHADED_TRIANGLE(i + 1, 0, i + 2, lProjectedVertex3D, true, GradientsUVRGBM, EdgeUVRGBM, mEdgeUVRGBM, DrawUVRGBMTriangle);
										}
					
										pThis->mTriangleStripInvert = false;
									}
									else
									{
										for (i = 0; i < lVertex3DCount - 2; i++)
										{
											MACRO_DRAW_SHADED_TRIANGLE(0, i + 1, i + 2, lProjectedVertex3D, true, GradientsUVRGBM, EdgeUVRGBM, mEdgeUVRGBM, DrawUVRGBMTriangle);
										}
										pThis->mTriangleStripInvert = true;
									}
								}
							}
						}// End if(RENDER_NICEST)
						else
						{
							Vertex3DUVRGB lProjectedVertex3D[8];
							Vertex3D* lV[8];
							lV[0] = &lProjectedVertex3D[0];
							lV[1] = &lProjectedVertex3D[1];
							lV[2] = &lProjectedVertex3D[2];
							lV[3] = &lProjectedVertex3D[3];
							lV[4] = &lProjectedVertex3D[4];
							lV[5] = &lProjectedVertex3D[5];
							lV[6] = &lProjectedVertex3D[6];
							lV[7] = &lProjectedVertex3D[7];
							int lVertex3DCount = ClipAndProjectTriangle(lV, pThis, (ClipFlags)(CLIP_UV | CLIP_RGB), SetVertex3DUVRGBData);
							if (lVertex3DCount >= 3)
							{
								bool lVisible = CheckVisible(lV, lVertex3DCount, pThis);

								if (lVisible == true)
								{
									int i;

									int lMipMapLevel = pThis->GetFastMipMapLevel(lV, lVertex3DCount, GetU_UVRGB, GetV_UVRGB);

									MACRO_RESCALE_UV_COORDS(lVertex3DCount, lMipMapLevel);

									if (pThis->mTriangleStripInvert == true)
									{
										for (i = 0; i < lVertex3DCount - 2; i++)
										{
											MACRO_DRAW_UVRGB_TRIANGLE_FAST(i + 1, 0, i + 2, lProjectedVertex3D, lMipMapLevel);
										}
					
										pThis->mTriangleStripInvert = false;
									}
									else
									{
										for (i = 0; i < lVertex3DCount - 2; i++)
										{
											MACRO_DRAW_UVRGB_TRIANGLE_FAST(0, i + 1, i + 2, lProjectedVertex3D, lMipMapLevel);
										}
										pThis->mTriangleStripInvert = true;
									}
								}
							}
						}//End else
					}

					// Empty the vertex buffers...
					pThis->PrepareNextTriangleStripPointDataXYZ();
					pThis->PrepareNextTriangleStripPointDataUVM();
					pThis->PrepareNextTriangleStripPointDataRGB();
					pThis->PrepareNextTriangleStripPointDataNormal();
				}
			}
			else
			{
				RenderTriangleStripUVM(pThis);
			}
		}
		else
		{
			RenderTriangleStripRGB(pThis);
		}
	}
}





void Software3DPainter::RenderTriangleFanSingleColor(Software3DPainter* pThis)
{
	if (pThis->mCurrentXYZIndex >= 3)
	{
		if (pThis->mScreen != 0)
		{
			pThis->ProcessLights();

			Lepra::Color lColor;
			if (pThis->mCurrentRGBIndex >= 1)
			{
				lColor.mRed   = (Lepra::uint8)(pThis->mPoint[0].mRGB->mLightProcessedR * 255.0f);
				lColor.mGreen = (Lepra::uint8)(pThis->mPoint[0].mRGB->mLightProcessedG * 255.0f);
				lColor.mBlue  = (Lepra::uint8)(pThis->mPoint[0].mRGB->mLightProcessedB * 255.0f);
			}

			Vertex3D lProjectedVertex3D[8];
			Vertex3D* lV[8];
			lV[0] = &lProjectedVertex3D[0];
			lV[1] = &lProjectedVertex3D[1];
			lV[2] = &lProjectedVertex3D[2];
			lV[3] = &lProjectedVertex3D[3];
			lV[4] = &lProjectedVertex3D[4];
			lV[5] = &lProjectedVertex3D[5];
			lV[6] = &lProjectedVertex3D[6];
			lV[7] = &lProjectedVertex3D[7];
			int lVertex3DCount = ClipAndProjectTriangle(lV, pThis, (ClipFlags)0, SetVertex3DData);
			if (lVertex3DCount >= 3)
			{
				bool lVisible = CheckVisible(lV, lVertex3DCount, pThis);

				if (lVisible == true)
				{
					int i;

					for (i = 0; i < lVertex3DCount - 2; i++)
					{
						MACRO_DRAW_SINGLE_COLOR_TRIANGLE(0, i + 1, i + 2, lProjectedVertex3D);
					}
				}
			}
		}

		// Empty the vertex buffers...
		pThis->PrepareNextTriangleFanPointDataXYZ();
	}
}





void Software3DPainter::RenderTriangleFanRGB(Software3DPainter* pThis)
{
	if (pThis->mCurrentXYZIndex >= 3)
	{
		pThis->ProcessLights();

		if (pThis->mCurrentRGBIndex >= 3)
		{
			if (pThis->mScreen != 0)
			{
				Vertex3DRGB lProjectedVertex3D[8];
				Vertex3D* lV[8];
				lV[0] = &lProjectedVertex3D[0];
				lV[1] = &lProjectedVertex3D[1];
				lV[2] = &lProjectedVertex3D[2];
				lV[3] = &lProjectedVertex3D[3];
				lV[4] = &lProjectedVertex3D[4];
				lV[5] = &lProjectedVertex3D[5];
				lV[6] = &lProjectedVertex3D[6];
				lV[7] = &lProjectedVertex3D[7];
				int lVertex3DCount = ClipAndProjectTriangle(lV, pThis, CLIP_RGB, SetVertex3DRGBData);
				if (lVertex3DCount >= 3)
				{
					bool lVisible = CheckVisible(lV, lVertex3DCount, pThis);

					if (lVisible == true)
					{
						int i;

						if (pThis->mRenderingHint == RENDER_NICEST)
						{
							for (i = 0; i < lVertex3DCount - 2; i++)
							{
								MACRO_DRAW_SHADED_TRIANGLE(0, i + 1, i + 2, lProjectedVertex3D, true, GradientsRGB, EdgeRGB, mEdgeRGB, DrawRGBTriangle);
							}
						}
						else
						{
							for (i = 0; i < lVertex3DCount - 2; i++)
							{
								MACRO_DRAW_SHADED_TRIANGLE(0, i + 1, i + 2, lProjectedVertex3D, false, GradientsRGB, EdgeRGB, mEdgeRGB, DrawRGBTriangle);
							}
						}
					}
				}
			}

			// Empty the vertex buffers...
			pThis->PrepareNextTriangleFanPointDataXYZ();
			pThis->PrepareNextTriangleFanPointDataRGB();
			pThis->PrepareNextTriangleFanPointDataNormal();
		}
		else
		{
			RenderTriangleFanSingleColor(pThis);
		}
	}
}





void Software3DPainter::RenderTriangleFanUV(Software3DPainter* pThis)
{
	if (pThis->mCurrentXYZIndex >= 3)
	{
		if (pThis->mCurrentUVIndex >= 3)
		{
			if (pThis->mTexture != 0)
			{
				if (pThis->mScreen != 0)
				{
					Vertex3DUV lProjectedVertex3D[8];
					Vertex3D* lV[8];
					lV[0] = &lProjectedVertex3D[0];
					lV[1] = &lProjectedVertex3D[1];
					lV[2] = &lProjectedVertex3D[2];
					lV[3] = &lProjectedVertex3D[3];
					lV[4] = &lProjectedVertex3D[4];
					lV[5] = &lProjectedVertex3D[5];
					lV[6] = &lProjectedVertex3D[6];
					lV[7] = &lProjectedVertex3D[7];
					int lVertex3DCount = ClipAndProjectTriangle(lV, pThis, CLIP_UV, SetVertex3DUVData);

					if (lVertex3DCount >= 3)
					{
						bool lVisible = CheckVisible(lV, lVertex3DCount, pThis);

						if (lVisible == true)
						{
							int i;

							MACRO_RESCALE_UV_COORDS(lVertex3DCount, 0);

							if (pThis->mRenderingHint == RENDER_NICEST)
							{
								for (i = 0; i < lVertex3DCount - 2; i++)
								{
									MACRO_DRAW_TEXTURED_TRIANGLE(0, i + 1, i + 2, lProjectedVertex3D, GradientsUV, EdgeUV, mEdgeUV, DrawUVTriangle);
								}
							}
							else
							{
								for (i = 0; i < lVertex3DCount - 2; i++)
								{
									MACRO_DRAW_UV_TRIANGLE_FAST(0, i + 1, i + 2, lProjectedVertex3D, 0);
								}
							}
						}
					}
				}

				// Empty the vertex buffers...
				pThis->PrepareNextTriangleFanPointDataXYZ();
				pThis->PrepareNextTriangleFanPointDataUVM();
			}
		}
		else
		{
			RenderTriangleFanSingleColor(pThis);
		}
	}
}





void Software3DPainter::RenderTriangleFanUVM(Software3DPainter* pThis)
{
	if (pThis->mCurrentXYZIndex >= 3)
	{
		if (pThis->mCurrentUVIndex >= 3)
		{
			if (pThis->mTexture != 0)
			{
				if (pThis->mScreen != 0)
				{
					if (pThis->mRenderingHint == RENDER_NICEST)
					{
						Vertex3DUVM lProjectedVertex3D[8];
						Vertex3D* lV[8];
						lV[0] = &lProjectedVertex3D[0];
						lV[1] = &lProjectedVertex3D[1];
						lV[2] = &lProjectedVertex3D[2];
						lV[3] = &lProjectedVertex3D[3];
						lV[4] = &lProjectedVertex3D[4];
						lV[5] = &lProjectedVertex3D[5];
						lV[6] = &lProjectedVertex3D[6];
						lV[7] = &lProjectedVertex3D[7];
						int lVertex3DCount = ClipAndProjectTriangle(lV, pThis, CLIP_UV, SetVertex3DUVMData);

						if (lVertex3DCount >= 3)
						{
							bool lVisible = CheckVisible(lV, lVertex3DCount, pThis);

							if (lVisible == true)
							{
								int i;

								MACRO_RESCALE_UVM_COORDS(lVertex3DCount, 0);

								for (i = 0; i < lVertex3DCount - 2; i++)
								{
									MACRO_DRAW_TEXTURED_TRIANGLE(0, i + 1, i + 2, lProjectedVertex3D, GradientsUVM, EdgeUVM, mEdgeUVM, DrawUVMTriangle);
								}
							}
						}
					}
					else
					{
						Vertex3DUV lProjectedVertex3D[8];
						Vertex3D* lV[8];
						lV[0] = &lProjectedVertex3D[0];
						lV[1] = &lProjectedVertex3D[1];
						lV[2] = &lProjectedVertex3D[2];
						lV[3] = &lProjectedVertex3D[3];
						lV[4] = &lProjectedVertex3D[4];
						lV[5] = &lProjectedVertex3D[5];
						lV[6] = &lProjectedVertex3D[6];
						lV[7] = &lProjectedVertex3D[7];
						int lVertex3DCount = ClipAndProjectTriangle(lV, pThis, CLIP_UV, SetVertex3DUVData);

						if (lVertex3DCount >= 3)
						{
							bool lVisible = CheckVisible(lV, lVertex3DCount, pThis);

							if (lVisible == true)
							{
								int i;

								int lMipMapLevel = pThis->GetFastMipMapLevel(lV, lVertex3DCount, GetU_UV, GetV_UV);

								MACRO_RESCALE_UV_COORDS(lVertex3DCount, lMipMapLevel);

								for (i = 0; i < lVertex3DCount - 2; i++)
								{
									MACRO_DRAW_UV_TRIANGLE_FAST(0, i + 1, i + 2, lProjectedVertex3D, lMipMapLevel);
								}
							}
						}
					}
				}

				// Empty the vertex buffers...
				pThis->PrepareNextTriangleFanPointDataXYZ();
				pThis->PrepareNextTriangleFanPointDataUVM();
			}
		}
		else
		{
			RenderTriangleFanSingleColor(pThis);
		}
	}
}





void Software3DPainter::RenderTriangleFanUVRGB(Software3DPainter* pThis)
{
	if (pThis->mCurrentXYZIndex >= 3)
	{
		if (pThis->mCurrentUVIndex >= 3)
		{
			pThis->ProcessLights();

			if (pThis->mCurrentRGBIndex >= 3)
			{
				if (pThis->mTexture != 0)
				{
					if (pThis->mScreen != 0)
					{
						Vertex3DUVRGB lProjectedVertex3D[8];
						Vertex3D* lV[8];
						lV[0] = &lProjectedVertex3D[0];
						lV[1] = &lProjectedVertex3D[1];
						lV[2] = &lProjectedVertex3D[2];
						lV[3] = &lProjectedVertex3D[3];
						lV[4] = &lProjectedVertex3D[4];
						lV[5] = &lProjectedVertex3D[5];
						lV[6] = &lProjectedVertex3D[6];
						lV[7] = &lProjectedVertex3D[7];
						int lVertex3DCount = ClipAndProjectTriangle(lV, pThis, (ClipFlags)(CLIP_UV | CLIP_RGB), SetVertex3DUVRGBData);
						if (lVertex3DCount >= 3)
						{
							bool lVisible = CheckVisible(lV, lVertex3DCount, pThis);

							if (lVisible == true)
							{
								int i;

								MACRO_RESCALE_UV_COORDS(lVertex3DCount, 0);

								if (pThis->mRenderingHint == RENDER_NICEST)
								{
									for (i = 0; i < lVertex3DCount - 2; i++)
									{
										MACRO_DRAW_SHADED_TRIANGLE(0, i + 1, i + 2, lProjectedVertex3D, true, GradientsUVRGB, EdgeUVRGB, mEdgeUVRGB, DrawUVRGBTriangle);
									}
								}
								else
								{
									for (i = 0; i < lVertex3DCount - 2; i++)
									{
										MACRO_DRAW_UVRGB_TRIANGLE_FAST(0, i + 1, i + 2, lProjectedVertex3D, 0);
									}
								}
							}
						}
					}

					// Empty the vertex buffers...
					pThis->PrepareNextTriangleFanPointDataXYZ();
					pThis->PrepareNextTriangleFanPointDataUVM();
					pThis->PrepareNextTriangleFanPointDataRGB();
					pThis->PrepareNextTriangleFanPointDataNormal();
				}
			}
			else
			{
				RenderTriangleFanUV(pThis);
			}
		}
		else
		{
			RenderTriangleFanRGB(pThis);
		}
	}
}






void Software3DPainter::RenderTriangleFanUVRGBM(Software3DPainter* pThis)
{
	if (pThis->mCurrentXYZIndex >= 3)
	{
		if (pThis->mCurrentUVIndex >= 3)
		{
			pThis->ProcessLights();

			if (pThis->mCurrentRGBIndex >= 3)
			{
				if (pThis->mTexture != 0)
				{
					if (pThis->mScreen != 0)
					{
						if (pThis->mRenderingHint == RENDER_NICEST)
						{
							Vertex3DUVRGBM lProjectedVertex3D[8];
							Vertex3D* lV[8];
							lV[0] = &lProjectedVertex3D[0];
							lV[1] = &lProjectedVertex3D[1];
							lV[2] = &lProjectedVertex3D[2];
							lV[3] = &lProjectedVertex3D[3];
							lV[4] = &lProjectedVertex3D[4];
							lV[5] = &lProjectedVertex3D[5];
							lV[6] = &lProjectedVertex3D[6];
							lV[7] = &lProjectedVertex3D[7];
							int lVertex3DCount = ClipAndProjectTriangle(lV, pThis, (ClipFlags)(CLIP_UV | CLIP_RGB), SetVertex3DUVRGBMData);

							if (lVertex3DCount >= 3)
							{
								bool lVisible = CheckVisible(lV, lVertex3DCount, pThis);

								if (lVisible == true)
								{
									int i;

									MACRO_RESCALE_UVM_COORDS(lVertex3DCount, 0);

									for (i = 0; i < lVertex3DCount - 2; i++)
									{
										MACRO_DRAW_SHADED_TRIANGLE(0, i + 1, i + 2, lProjectedVertex3D, true, GradientsUVRGBM, EdgeUVRGBM, mEdgeUVRGBM, DrawUVRGBMTriangle);
									}
								}
							}
						}
						else
						{
							Vertex3DUVRGB lProjectedVertex3D[8];
							Vertex3D* lV[8];
							lV[0] = &lProjectedVertex3D[0];
							lV[1] = &lProjectedVertex3D[1];
							lV[2] = &lProjectedVertex3D[2];
							lV[3] = &lProjectedVertex3D[3];
							lV[4] = &lProjectedVertex3D[4];
							lV[5] = &lProjectedVertex3D[5];
							lV[6] = &lProjectedVertex3D[6];
							lV[7] = &lProjectedVertex3D[7];
							int lVertex3DCount = ClipAndProjectTriangle(lV, pThis, (ClipFlags)(CLIP_UV | CLIP_RGB), SetVertex3DUVRGBData);
							if (lVertex3DCount >= 3)
							{
								bool lVisible = CheckVisible(lV, lVertex3DCount, pThis);

								if (lVisible == true)
								{
									int i;
									int lMipMapLevel = pThis->GetFastMipMapLevel(lV, lVertex3DCount, GetU_UVRGB, GetV_UVRGB);

									MACRO_RESCALE_UV_COORDS(lVertex3DCount, lMipMapLevel);

									for (i = 0; i < lVertex3DCount - 2; i++)
									{
										MACRO_DRAW_UVRGB_TRIANGLE_FAST(0, i + 1, i + 2, lProjectedVertex3D, lMipMapLevel);
									}
								}
							}
						}
					}

					// Empty the vertex buffers...
					pThis->PrepareNextTriangleFanPointDataXYZ();
					pThis->PrepareNextTriangleFanPointDataUVM();
					pThis->PrepareNextTriangleFanPointDataRGB();
					pThis->PrepareNextTriangleFanPointDataNormal();
				}
			}
			else
			{
				RenderTriangleFanUVM(pThis);
			}
		}
		else
		{
			RenderTriangleFanRGB(pThis);
		}
	}
}

} // End namespace.
