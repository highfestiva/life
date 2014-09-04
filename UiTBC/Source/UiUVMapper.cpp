/*
	Class:  UVMapper
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#include "pch.h"
#include "../Include/UiUVMapper.h"
#include "../../Tbc/Include/../../Tbc/Include/GeometryBase.h"
#include <math.h>


namespace UiTbc
{

bool UVMapper::ApplyPlanarMapping(Tbc::GeometryBase* pGeometry, unsigned int pUVSet, 
				  const Vector2DD& pUVOffset,
				  const Vector3DD& pPlaneX, 
				  const Vector3DD& pPlaneY)
{
	const double lEpsilon = 1e-15;

	// Check if vectors are parallel.
	double lDot = pPlaneX.Dot(pPlaneY);
	lDot /= (pPlaneX.GetLength() * pPlaneY.GetLength());

	bool lStatusOK = (fabs(lDot) < 1.0 - lEpsilon);
	float* lVertexData = pGeometry->GetVertexData();
	float* lUVData = pGeometry->GetUVData(pUVSet);

	if (lStatusOK)
	{
		lStatusOK = (lVertexData != 0 && lUVData != 0);
	}

	if (lStatusOK)
	{
		unsigned int i;
		for (i = 0; i < pGeometry->GetVertexCount(); i++)
		{
			int lVIndex = i * 3;
			int lUVIndex = i * 2;
			Vector3DD lV((double)lVertexData[lVIndex + 0],
					     (double)lVertexData[lVIndex + 1],
					     (double)lVertexData[lVIndex + 2]);
			
			lUVData[lUVIndex + 0] = (float)pPlaneX.Dot(lV) + (float)pUVOffset.x;
			lUVData[lUVIndex + 1] = (float)pPlaneY.Dot(lV) + (float)pUVOffset.y;
		}
	}

	return lStatusOK;
}


bool UVMapper::ApplyCubeMapping(Tbc::GeometryBase* pGeometry, unsigned int pUVSet, 
				float pScale,
				const Vector2DD& pUVOffsetLeft,
				const Vector2DD& pUVOffsetRight,
				const Vector2DD& pUVOffsetTop,
				const Vector2DD& pUVOffsetBottom,
				const Vector2DD& pUVOffsetFront,
				const Vector2DD& pUVOffsetBack)
{
	pGeometry->GenerateSurfaceNormalData();

	float* lVertexData = pGeometry->GetVertexData();
	float* lSurfaceNormal = pGeometry->GetSurfaceNormalData();
	vtx_idx_t* lIndex = pGeometry->GetIndexData();
	float* lUVData = pGeometry->GetUVData(pUVSet);

	pScale = 1.0f / pScale;

	bool lStatusOk = (lVertexData != 0 && lSurfaceNormal != 0 && lIndex != 0 && lUVData != 0);

	if (lStatusOk)
	{
		unsigned int i;
		for (i = 0; i < pGeometry->GetTriangleCount(); i++)
		{
			int lTriIndex = i * 3;
			int lV1Index = lIndex[lTriIndex + 0] * 3;
			int lV2Index = lIndex[lTriIndex + 1] * 3;
			int lV3Index = lIndex[lTriIndex + 2] * 3;
			int lUV1 = lIndex[lTriIndex + 0] * 2;
			int lUV2 = lIndex[lTriIndex + 1] * 2;
			int lUV3 = lIndex[lTriIndex + 2] * 2;

			float x = lSurfaceNormal[lTriIndex + 0];
			float y = lSurfaceNormal[lTriIndex + 1];
			float z = lSurfaceNormal[lTriIndex + 2];

			float lAbsX = abs(x);
			float lAbsY = abs(y);
			float lAbsZ = abs(z);

			vec3 lXAxis;
			vec3 lYAxis;
			vec2 lUVOffset;

			if (lAbsX > lAbsY && lAbsX > lAbsZ)
			{
				// Left or right.
				if (x > 0)
				{
					lXAxis.Set(0, 1, 0);
					lUVOffset.Set((float)pUVOffsetRight.x, (float)pUVOffsetRight.y);
				}
				else
				{
					lXAxis.Set(0, -1, 0);
					lUVOffset.Set((float)pUVOffsetLeft.x, (float)pUVOffsetLeft.y);
				}
				lYAxis.Set(0, 0, 1.0f);
			}
			else if(lAbsY > lAbsX && lAbsY > lAbsZ)
			{
				// Front or back.
				if (y < 0)
				{
					lXAxis.Set(1, 0, 0);
					lUVOffset.Set((float)pUVOffsetFront.x, (float)pUVOffsetFront.y);
				}
				else
				{
					lXAxis.Set(-1, 0, 0);
					lUVOffset.Set((float)pUVOffsetBack.x, (float)pUVOffsetBack.y);
				}
				lYAxis.Set(0, 0, 1.0f);
			}
			else
			{
				// Top or bottom.
				if (z > 0)
				{
					lYAxis.Set(0, 1, 0);
					lUVOffset.Set((float)pUVOffsetTop.x, (float)pUVOffsetTop.y);
				}
				else
				{
					lYAxis.Set(0, -1, 0);
					lUVOffset.Set((float)pUVOffsetBottom.x, (float)pUVOffsetBottom.y);
				}
				lXAxis.Set(1.0f, 0, 0);
			}

			vec3 lV1(lVertexData[lV1Index + 0],
			                     lVertexData[lV1Index + 1],
			                     lVertexData[lV1Index + 2]);
			vec3 lV2(lVertexData[lV2Index + 0],
			                     lVertexData[lV2Index + 1],
			                     lVertexData[lV2Index + 2]);
			vec3 lV3(lVertexData[lV3Index + 0],
			                     lVertexData[lV3Index + 1],
			                     lVertexData[lV3Index + 2]);
			
			lUVData[lUV1 + 0] = (float)lXAxis.Dot(lV1) * pScale + lUVOffset.x;
			lUVData[lUV1 + 1] = (float)lYAxis.Dot(-lV1) * pScale + lUVOffset.y;

			lUVData[lUV2 + 0] = (float)lXAxis.Dot(lV2) * pScale + lUVOffset.x;
			lUVData[lUV2 + 1] = (float)lYAxis.Dot(-lV2) * pScale + lUVOffset.y;

			lUVData[lUV3 + 0] = (float)lXAxis.Dot(lV3) * pScale + lUVOffset.x;
			lUVData[lUV3 + 1] = (float)lYAxis.Dot(-lV3) * pScale + lUVOffset.y;
		}
	}

	return lStatusOk;
}

} // End namespace.
