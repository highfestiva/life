
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/UiBasicMeshCreator.h"
#include <assert.h>
#include <list>
#include "../../Lepra/Include/Math.h"
#include "../../Lepra/Include/Vector3D.h"
#include "../Include/UiAnimatedGeometry.h"
#include "../Include/UiTBC.h"
#include "../Include/UiTriangleBasedGeometry.h"



namespace UiTbc
{



TriangleBasedGeometry* BasicMeshCreator::CreateFlatBox(float pXSize, float pYSize, float pZSize,
						   unsigned pXSegments,
						   unsigned pYSegments,
						   unsigned pZSegments)
{
	int lVertexCount = (pXSegments + 1) * (pYSegments + 1) * 2 +  // Top & bottom plane.
			    (pXSegments + 1) * (pZSegments + 1) * 2 +  // Front and back plane.
			    (pYSegments + 1) * (pZSegments + 1) * 2;   // Left and right plane.

	int lNumTriangles = pXSegments * pYSegments * 4 +
			     pXSegments * pZSegments * 4 +
			     pYSegments * pZSegments * 4;

	Vector3DF* lV = new Vector3DF[lVertexCount];
	Vector3DF* lN = new Vector3DF[lVertexCount];
	uint32* lIndices = new uint32[lNumTriangles * 3];

	::memset(lIndices, 0, lNumTriangles * 3 * sizeof(uint32));

	float lXStep = pXSize / (float)pXSegments;
	float lYStep = pYSize / (float)pYSegments;
	float lZStep = pZSize / (float)pZSegments;

	float lHalfXSize = pXSize * 0.5f;
	float lHalfYSize = pYSize * 0.5f;
	float lHalfZSize = pZSize * 0.5f;

	float lX, lY, lZ;
	unsigned x, y, z;

	int lVIndex = 0;
	int lTIndex = 0;

	// TOP AND BOTTOM PLANES.
	int lVIndexOffset = (pXSegments + 1) * (pYSegments  + 1);
	lY = -lHalfYSize;
	for (y = 0; y < pYSegments + 1; y++)
	{
		lX = -lHalfXSize;
		for (x = 0; x < pXSegments + 1; x++)
		{
			// Top plane vertex and normal.
			lV[lVIndex].Set(lX, lY, lHalfZSize);
			lN[lVIndex].Set(0, 0, 1);

			// Bottom plane vertex and normal.
			lV[lVIndex + lVIndexOffset].Set(lX, lY, -lHalfZSize);
			lN[lVIndex + lVIndexOffset].Set(0, 0, -1);

			lVIndex++;
			lX += lXStep;
		}
		lY += lYStep;
	}
	int lBottomPlaneIndexOffset = lVIndex;
	lVIndex += lVIndexOffset;

	// FRONT AND BACK PLANES.
	int lFrontPlaneIndexOffset = lVIndex;
	lVIndexOffset = (pXSegments + 1) * (pZSegments + 1);
	lZ = -lHalfZSize;
	for (z = 0; z < pZSegments + 1; z++)
	{
		lX = -lHalfXSize;
		for (x = 0; x < pXSegments + 1; x++)
		{
			// Front plane vertex and normal.
			lV[lVIndex].Set(lX, -lHalfYSize, lZ);
			lN[lVIndex].Set(0, -1, 0);

			// Back plane vertex and normal.
			lV[lVIndex + lVIndexOffset].Set(lX, lHalfYSize, lZ);
			lN[lVIndex + lVIndexOffset].Set(0, 1, 0);

			lVIndex++;
			lX += lXStep;
		}
		lZ += lZStep;
	}
	int lBackPlaneIndexOffset = lVIndex;
	lVIndex += lVIndexOffset;

	// LEFT AND RIGHT PLANES.
	int lLeftPlaneIndexOffset = lVIndex;
	lVIndexOffset = (pYSegments + 1) * (pZSegments + 1);
	lZ = -lHalfZSize;
	for (z = 0; z < pZSegments + 1; z++)
	{
		lY = -lHalfYSize;
		for (y = 0; y < pYSegments + 1; y++)
		{
			// Left plane vertex and normal.
			lV[lVIndex].Set(-lHalfXSize, lY, lZ);
			lN[lVIndex].Set(-1, 0, 0);

			// Right plane vertex and normal.
			lV[lVIndex + lVIndexOffset].Set(lHalfXSize, lY, lZ);
			lN[lVIndex + lVIndexOffset].Set(1, 0, 0);

			lVIndex++;
			lY += lYStep;
		}
		lZ += lZStep;
	}
	int lRightPlaneIndexOffset = lVIndex;
	lVIndex += lVIndexOffset;

	// Setup top plane triangles.
	for (y = 0; y < pYSegments; y++)
	{
		for (x = 0; x < pXSegments; x++)
		{
			// First triangle.
			lIndices[lTIndex++] = (y + 0) * (pXSegments + 1) + x + 0;
			lIndices[lTIndex++] = (y + 0) * (pXSegments + 1) + x + 1;
			lIndices[lTIndex++] = (y + 1) * (pXSegments + 1) + x + 1;

			// Second triangle.
			lIndices[lTIndex++] = (y + 1) * (pXSegments + 1) + x + 0;
			lIndices[lTIndex++] = (y + 0) * (pXSegments + 1) + x + 0;
			lIndices[lTIndex++] = (y + 1) * (pXSegments + 1) + x + 1;

			assert(lTIndex <= lNumTriangles * 3);
		}
	}

	// Setup bottom plane triangles.
	for (z = 0; z < pYSegments; z++)
	{
		for (x = 0; x < pXSegments; x++)
		{
			// First triangle.
			lIndices[lTIndex++] = (z + 0) * (pXSegments + 1) + x + 1 + lBottomPlaneIndexOffset;
			lIndices[lTIndex++] = (z + 0) * (pXSegments + 1) + x + 0 + lBottomPlaneIndexOffset;
			lIndices[lTIndex++] = (z + 1) * (pXSegments + 1) + x + 1 + lBottomPlaneIndexOffset;

			// Second triangle.
			lIndices[lTIndex++] = (z + 0) * (pXSegments + 1) + x + 0 + lBottomPlaneIndexOffset;
			lIndices[lTIndex++] = (z + 1) * (pXSegments + 1) + x + 0 + lBottomPlaneIndexOffset;
			lIndices[lTIndex++] = (z + 1) * (pXSegments + 1) + x + 1 + lBottomPlaneIndexOffset;

			assert(lTIndex <= lNumTriangles * 3);
		}
	}

	// Setup front plane triangles.
	for (z = 0; z < pZSegments; z++)
	{
		for (x = 0; x < pXSegments; x++)
		{
			// First triangle.
			lIndices[lTIndex++] = (z + 0) * (pXSegments + 1) + x + 0 + lFrontPlaneIndexOffset;
			lIndices[lTIndex++] = (z + 0) * (pXSegments + 1) + x + 1 + lFrontPlaneIndexOffset;
			lIndices[lTIndex++] = (z + 1) * (pXSegments + 1) + x + 1 + lFrontPlaneIndexOffset;

			// Second triangle.
			lIndices[lTIndex++] = (z + 1) * (pXSegments + 1) + x + 0 + lFrontPlaneIndexOffset;
			lIndices[lTIndex++] = (z + 0) * (pXSegments + 1) + x + 0 + lFrontPlaneIndexOffset;
			lIndices[lTIndex++] = (z + 1) * (pXSegments + 1) + x + 1 + lFrontPlaneIndexOffset;

			assert(lTIndex <= lNumTriangles * 3);
		}
	}

	// Setup back plane triangles.
	for (z = 0; z < pZSegments; z++)
	{
		for (x = 0; x < pXSegments; x++)
		{
			// First triangle.
			lIndices[lTIndex++] = (z + 0) * (pXSegments + 1) + x + 1 + lBackPlaneIndexOffset;
			lIndices[lTIndex++] = (z + 0) * (pXSegments + 1) + x + 0 + lBackPlaneIndexOffset;
			lIndices[lTIndex++] = (z + 1) * (pXSegments + 1) + x + 1 + lBackPlaneIndexOffset;

			// Second triangle.
			lIndices[lTIndex++] = (z + 0) * (pXSegments + 1) + x + 0 + lBackPlaneIndexOffset;
			lIndices[lTIndex++] = (z + 1) * (pXSegments + 1) + x + 0 + lBackPlaneIndexOffset;
			lIndices[lTIndex++] = (z + 1) * (pXSegments + 1) + x + 1 + lBackPlaneIndexOffset;

			assert(lTIndex <= lNumTriangles * 3);
		}
	}

	// Setup left plane triangles.
	for (z = 0; z < pZSegments; z++)
	{
		for (y = 0; y < pYSegments; y++)
		{
			// First triangle.
			lIndices[lTIndex++] = (z + 0) * (pYSegments + 1) + y + 1 + lLeftPlaneIndexOffset;
			lIndices[lTIndex++] = (z + 0) * (pYSegments + 1) + y + 0 + lLeftPlaneIndexOffset;
			lIndices[lTIndex++] = (z + 1) * (pYSegments + 1) + y + 1 + lLeftPlaneIndexOffset;

			// Second triangle.
			lIndices[lTIndex++] = (z + 0) * (pYSegments + 1) + y + 0 + lLeftPlaneIndexOffset;
			lIndices[lTIndex++] = (z + 1) * (pYSegments + 1) + y + 0 + lLeftPlaneIndexOffset;
			lIndices[lTIndex++] = (z + 1) * (pYSegments + 1) + y + 1 + lLeftPlaneIndexOffset;

			assert(lTIndex <= lNumTriangles * 3);
		}
	}

	// Setup right plane triangles.
	for (z = 0; z < pZSegments; z++)
	{
		for (y = 0; y < pYSegments; y++)
		{
			// First triangle.
			lIndices[lTIndex++] = (z + 0) * (pYSegments + 1) + y + 0 + lRightPlaneIndexOffset;
			lIndices[lTIndex++] = (z + 0) * (pYSegments + 1) + y + 1 + lRightPlaneIndexOffset;
			lIndices[lTIndex++] = (z + 1) * (pYSegments + 1) + y + 1 + lRightPlaneIndexOffset;

			// Second triangle.
			lIndices[lTIndex++] = (z + 1) * (pYSegments + 1) + y + 0 + lRightPlaneIndexOffset;
			lIndices[lTIndex++] = (z + 0) * (pYSegments + 1) + y + 0 + lRightPlaneIndexOffset;
			lIndices[lTIndex++] = (z + 1) * (pYSegments + 1) + y + 1 + lRightPlaneIndexOffset;

			assert(lTIndex <= lNumTriangles * 3);
		}
	}

	TriangleBasedGeometry* lGeometry;
	lGeometry = new TriangleBasedGeometry(lV, lN, 0, 0,
					    TriangleBasedGeometry::COLOR_RGB,
					    lIndices, lVertexCount, lNumTriangles * 3,
					    TBC::GeometryBase::TRIANGLES,
					    TBC::GeometryBase::GEOM_STATIC);

	delete[] lV;
	delete[] lN;
	delete[] lIndices;

	return lGeometry;
}

TriangleBasedGeometry* BasicMeshCreator::CreateEllipsoid(float pXRadius,
							 float pYRadius, 
							 float pZRadius,
							 unsigned pNumLatitudeSegments,
							 unsigned pNumLongitudeSegments)
{
	int lVertexCount = pNumLongitudeSegments * (pNumLatitudeSegments - 1) + 2;
	int lNumTriangles = pNumLongitudeSegments * (pNumLatitudeSegments - 2) * 2 + pNumLongitudeSegments * 2;

	Vector3D<float>* lV = new Vector3D<float>[lVertexCount];
	Vector3D<float>* lN = new Vector3D<float>[lVertexCount];
	uint32* lI = new uint32[lNumTriangles * 3];

	const float lLatAngleStep = PIF / (float)pNumLatitudeSegments;
	const float lLongAngleStep = PIF * 2.0f / (float)pNumLongitudeSegments;

	// Setup top and bottom vertex.
	lV[0].Set(0, pYRadius, 0);
	lV[1].Set(0, -pYRadius, 0);
	lN[0].Set(0, 1.0f, 0);
	lN[1].Set(0, -1.0f, 0);

	// Setup the rest of the vertices.
	int lIndex = 2;
	float lLatAngle = lLatAngleStep;
	unsigned lLatSegmentCount;
	for (lLatSegmentCount = 1; lLatSegmentCount < pNumLatitudeSegments; lLatSegmentCount++)
	{
		float lCircleRadius = (float)sin(lLatAngle);
		float lY = (float)cos(lLatAngle) * pYRadius;
		float lNormY = (float)cos(lLatAngle) / pYRadius;

		float lLongAngle = 0.0f;
		unsigned lLongSegmentCount;
		for (lLongSegmentCount = 0; lLongSegmentCount < pNumLongitudeSegments; lLongSegmentCount++)
		{
			float lCosA = (float)cos(lLongAngle) * lCircleRadius;
			float lSinA = (float)sin(lLongAngle) * lCircleRadius;

			lV[lIndex].x = lCosA * pXRadius;
			lV[lIndex].y = lY;
			lV[lIndex].z = lSinA * pZRadius;

			lN[lIndex].x = lCosA / pXRadius;
			lN[lIndex].y = lNormY;
			lN[lIndex].z = lSinA / pZRadius;
			lN[lIndex].Normalize();

			lLongAngle += lLongAngleStep;
			lIndex++;
		}

		lLatAngle += lLatAngleStep;
	}

	// Setup "middle" triangles.
	lIndex = 0;
	for (lLatSegmentCount = 1; lLatSegmentCount < pNumLatitudeSegments - 1; lLatSegmentCount++)
	{
		unsigned lLatOffset0 = (lLatSegmentCount - 1) * pNumLongitudeSegments;
		unsigned lLatOffset1 = lLatOffset0 + pNumLongitudeSegments;

		unsigned lLongSegmentCount;
		for (lLongSegmentCount = 0; lLongSegmentCount < pNumLongitudeSegments; lLongSegmentCount++)
		{
			lI[lIndex + 0] = 2 + lLatOffset0 + lLongSegmentCount;
			lI[lIndex + 1] = 2 + lLatOffset0 + (lLongSegmentCount + 1) % pNumLongitudeSegments;
			lI[lIndex + 2] = 2 + lLatOffset1 + (lLongSegmentCount + 1) % pNumLongitudeSegments;
			lI[lIndex + 3] = 2 + lLatOffset0 + lLongSegmentCount;
			lI[lIndex + 4] = 2 + lLatOffset1 + (lLongSegmentCount + 1) % pNumLongitudeSegments;
			lI[lIndex + 5] = 2 + lLatOffset1 + lLongSegmentCount;

			lIndex += 6;
		}
	}

	// Setup "top" triangles.
	unsigned lLongSegmentCount;
	for (lLongSegmentCount = 0; lLongSegmentCount < pNumLongitudeSegments; lLongSegmentCount++)
	{
		lI[lIndex + 0] = 0;
		lI[lIndex + 1] = 2 + (lLongSegmentCount + 1) % pNumLongitudeSegments;
		lI[lIndex + 2] = 2 + lLongSegmentCount;

		lIndex += 3;
	}

	// Setup "bottom" triangles.
	unsigned lOffset = 2 + (pNumLatitudeSegments - 2) * pNumLongitudeSegments;
	for (lLongSegmentCount = 0; lLongSegmentCount < pNumLongitudeSegments; lLongSegmentCount++)
	{
		lI[lIndex + 0] = 1;
		lI[lIndex + 1] = lOffset + lLongSegmentCount;
		lI[lIndex + 2] = lOffset + (lLongSegmentCount + 1) % pNumLongitudeSegments;

		lIndex += 3;
	}

	TriangleBasedGeometry* lEllipsoid = 
		new TriangleBasedGeometry(lV, lN, 0, 0,
					  TriangleBasedGeometry::COLOR_RGB,
					  lI, lVertexCount, lNumTriangles * 3, 
					  TBC::GeometryBase::TRIANGLES,
					  TBC::GeometryBase::GEOM_STATIC);
	delete[] lV;
	delete[] lN;
	delete[] lI;

	return lEllipsoid;
}

TriangleBasedGeometry* BasicMeshCreator::CreateCone(float pBaseRadius, 
						    float pHeight, 
						    unsigned pNumSegments)
{
	int lVertexCount = pNumSegments + 1;
	int lNumTriangles = pNumSegments * 2 - 2;

	Vector3D<float>* lV = new Vector3D<float>[lVertexCount];
	Vector3D<float>* lN = new Vector3D<float>[lVertexCount];
	uint32* lI = new uint32[lNumTriangles * 3];

	// Set the top vertex.
	lV[0].Set(0, 0, pHeight);
	lN[0].Set(0, 0, 1.0f);

	const float lAngleStep = (2.0f * PIF) / (float)pNumSegments;
	float lAngle = 0.0f;

	int lIndex = 0;
	unsigned i;
	for (i = 0; i < pNumSegments; i++, lIndex += 3)
	{
		float lCosA = (float)cos(lAngle);
		float lSinA = -(float)sin(lAngle);

		// Setup vertex
		lV[i + 1].x = lCosA * pBaseRadius;
		lV[i + 1].y = lSinA * pBaseRadius;
		lV[i + 1].z = 0.0f;

		// Setup normal
		lN[i + 1].x = lCosA;
		if (pBaseRadius > pHeight)
		{
			lN[i + 1].z = ((float)cos(atan(pHeight / pBaseRadius)) - 1.0f) * 0.5f;
		}
		else
		{
			lN[i + 1].z = ((float)sin(atan(pBaseRadius / pHeight)) - 1.0f) * 0.5f;
		}

		lN[i + 1].y = lSinA;
		lN[i + 1].Normalize();

		// Setup triangle index.
		lI[lIndex + 0] = i + 1;
		lI[lIndex + 1] = 0;
		lI[lIndex + 2] = ((i + 1) % pNumSegments) + 1;

		lAngle += lAngleStep;
	}

	// Setup the bottom triangles...
	for (i = 0; i < pNumSegments - 2; i++, lIndex += 3)
	{
		lI[lIndex + 0] = ((i + 1) % pNumSegments) + 2;
		lI[lIndex + 1] = 1;
		lI[lIndex + 2] = (i + 2) % pNumSegments;
	}

	TriangleBasedGeometry* lCone = 
		new TriangleBasedGeometry(lV, lN, 0, 0,
					  TriangleBasedGeometry::COLOR_RGB,
					  lI, lVertexCount, lNumTriangles * 3,
					  TBC::GeometryBase::TRIANGLES,
					  TBC::GeometryBase::GEOM_STATIC);

	delete[] lV;
	delete[] lN;
	delete[] lI;

	return lCone;
}

TriangleBasedGeometry* BasicMeshCreator::CreateCylinder(float pBaseRadius,
							float pTopRadius,
							float pHeight,
							unsigned pNumSegments)
{
	int lVertexCount = pNumSegments * 2;
	int lNumTriangles = pNumSegments * 2 + (pNumSegments - 2) * 2;

	Vector3D<float>* lV = new Vector3D<float>[lVertexCount];
	Vector3D<float>* lN = new Vector3D<float>[lVertexCount];
	uint32* lI = new uint32[lNumTriangles * 3];

	const float lAngleStep = (2.0f * PIF) / (float)pNumSegments;
	float lAngle = 0.0f;

	float lRadiusDiff = pBaseRadius - pTopRadius;
	float lNormYAdd;

	if (fabs(lRadiusDiff) > pHeight)
	{
		if (lRadiusDiff > 0.0f)
		{
			lNormYAdd = (float)cos(atan(pHeight / lRadiusDiff));
		}
		else
		{
			lNormYAdd = -(float)cos(atan(pHeight / lRadiusDiff));
		}
	}
	else
	{
		lNormYAdd = (float)sin(atan(lRadiusDiff / pHeight));
	}

	// Setup vertices.
	unsigned i;
	for (i = 0; i < pNumSegments; i++)
	{
		float lCosA = (float)cos(lAngle);
		float lSinA = (float)sin(lAngle);

		lV[i].x = lCosA * pTopRadius;
		lV[i].y = pHeight * 0.5f;
		lV[i].z = lSinA * pTopRadius;

		lN[i].x = lCosA;
		lN[i].y = 1.0f + lNormYAdd;
		lN[i].z = lSinA;
		lN[i].Normalize();

		lV[pNumSegments + i].x = lCosA * pBaseRadius;
		lV[pNumSegments + i].y = -pHeight * 0.5f;
		lV[pNumSegments + i].z = lSinA * pBaseRadius;

		lN[pNumSegments + i].x = lCosA;
		lN[pNumSegments + i].y = -(1.0f - lNormYAdd);
		lN[pNumSegments + i].z = lSinA;
		lN[pNumSegments + i].Normalize();

		lAngle += lAngleStep;
	}

	// Setup triangles.
	int lIndex = 0;
	for (i = 0; i < pNumSegments; i++)
	{
		lI[lIndex + 0] = i;
		lI[lIndex + 1] = (i + 1) % pNumSegments;
		lI[lIndex + 2] = (i + 1) % pNumSegments + pNumSegments;
		lI[lIndex + 3] = i;
		lI[lIndex + 4] = (i + 1) % pNumSegments + pNumSegments;
		lI[lIndex + 5] = i + pNumSegments;
		
		lIndex += 6;
	}

	// Setup top triangles.
	for (i = 0; i < pNumSegments - 2; i++)
	{
		lI[lIndex + 0] = 0;
		lI[lIndex + 1] = i + 2;
		lI[lIndex + 2] = i + 1;
		
		lIndex += 3;
	}

	// Setup bottom triangles.
	for (i = 0; i < pNumSegments - 2; i++)
	{
		lI[lIndex + 0] = pNumSegments;
		lI[lIndex + 1] = pNumSegments + i + 1;
		lI[lIndex + 2] = pNumSegments + i + 2;
		
		lIndex += 3;
	}

	TriangleBasedGeometry* lCylinder = 
		new TriangleBasedGeometry(lV, lN, 0, 0,
					  TriangleBasedGeometry::COLOR_RGB,
					  lI, lVertexCount, lNumTriangles * 3,
					  TBC::GeometryBase::TRIANGLES,
					  TBC::GeometryBase::GEOM_STATIC);

	delete[] lV;
	delete[] lN;
	delete[] lI;

	return lCylinder;
}

TriangleBasedGeometry* BasicMeshCreator::CreateTorus(float pRadius, 
						     float pRingRadiusX,
						     float pRingRadiusY,
						     unsigned pNumSegments,
						     unsigned pNumRingSegments)
{
	int lVertexCount = pNumSegments * pNumRingSegments;
	int lNumTriangles = pNumSegments * pNumRingSegments * 2;

	Vector3D<float>* lV = new Vector3D<float>[lVertexCount];
	Vector3D<float>* lN = new Vector3D<float>[lVertexCount];
	uint32* lI = new uint32[lNumTriangles * 3];

	const float lAngleStep = 2.0f * PIF / (float)pNumSegments;
	const float lRingAngleStep = 2.0f * PIF / (float)pNumRingSegments;

	float lAngle = 0.0f;

	int lIndex = 0;
	unsigned i;
	for (i = 0; i < pNumSegments; i++)
	{
		float lCosA = (float)cos(lAngle);
		float lSinA = (float)sin(lAngle);

		float lRingAngle = 0.0f;
		for (unsigned j = 0; j < pNumRingSegments; j++)
		{
			float lCosRingA = (float)cos(lRingAngle);
			float lSinRingA = (float)sin(lRingAngle);

			float lX = lCosRingA * pRingRadiusX;

			lV[lIndex].x = lCosA * (lX + pRadius);
			lV[lIndex].y = lSinRingA * pRingRadiusY;
			lV[lIndex].z = lSinA * (lX + pRadius);

			lN[lIndex].x = lCosRingA * lCosA;
			lN[lIndex].y = lSinRingA;
			lN[lIndex].z = lCosRingA * lSinA;
			//lN[lIndex].Normalize();

			lRingAngle += lRingAngleStep;
			lIndex++;
		}

		lAngle += lAngleStep;
	}

	lIndex = 0;
	for (i = 0; i < pNumSegments; i++)
	{
		unsigned lOffset0 = i * pNumRingSegments;
		unsigned lOffset1 = ((i + 1) % pNumSegments) * pNumRingSegments;

		for (unsigned j = 0; j < pNumRingSegments; j++)
		{
			lI[lIndex + 0] = lOffset0 + j;
			lI[lIndex + 1] = lOffset0 + (j + 1) % pNumRingSegments;
			lI[lIndex + 2] = lOffset1 + (j + 1) % pNumRingSegments;
			lI[lIndex + 3] = lOffset0 + j;
			lI[lIndex + 4] = lOffset1 + (j + 1) % pNumRingSegments;
			lI[lIndex + 5] = lOffset1 + j;

			lIndex += 6;
		}
	}

	TriangleBasedGeometry* lTorus = 
		new TriangleBasedGeometry(lV, lN, 0, 0,
					  TriangleBasedGeometry::COLOR_RGB,
					  lI, lVertexCount, lNumTriangles * 3,
					  TBC::GeometryBase::TRIANGLES,
					  TBC::GeometryBase::GEOM_STATIC);

	delete[] lV;
	delete[] lN;
	delete[] lI;

	return lTorus;
}

void BasicMeshCreator::CreateYBonedSkin(float pStartY, float pEndY, const TriangleBasedGeometry* pGeometry,
	AnimatedGeometry* pSkin, int pBoneCount, float pBoneStiffness)
{
	// Collect the Y minimas and maximas.
	std::list<int> lMinimaList;
	std::list<int> lMaximaList;
	unsigned v;
	for (v = 0; v < pGeometry->GetVertexCount(); ++v)
	{
		float y = pGeometry->GetVertexData()[v*3+1];
		if (y <= pStartY+eps_f)
		{
			lMinimaList.push_back(v);
		}
		else if (y >= pEndY-eps_f)
		{
			lMaximaList.push_back(v);
		}
	}

	UiTbc::AnimatedGeometry::BoneWeightGroup lWeightGroup;

	if (!lMinimaList.empty())
	{
		// Create weights for proximal end cap of skinned mesh.
		lWeightGroup.mBoneCount = 1;
		lWeightGroup.mBoneIndexArray[0] = 0;
		lWeightGroup.mVectorIndexCount = (int)lMinimaList.size();
		lWeightGroup.mVectorIndexArray = new int[lWeightGroup.mVectorIndexCount];
		lWeightGroup.mVectorWeightArray = new float[lWeightGroup.mVectorIndexCount*lWeightGroup.mBoneCount];
		std::list<int>::iterator i = lMinimaList.begin();
		for (v = 0; i != lMinimaList.end(); ++i, ++v)
		{
			lWeightGroup.mVectorIndexArray[v] = *i;
			lWeightGroup.mVectorWeightArray[v] = 1;
		}
		pSkin->AddBoneWeights(lWeightGroup);
	}

	// The middle part. Segment the geometry into (bone count - 1) groups; i.e. two bones per vertex.
	const int lSegments = pBoneCount-1;
	for (int y = 0; y < lSegments; ++y)
	{
		const float lPartMin = Math::Lerp(pStartY, pEndY, (float)y/lSegments);
		const float lPartMax = Math::Lerp(pStartY, pEndY, (float)(y+1)/lSegments);
		std::list<int> lSegmentList;
		for (v = 0; v < pGeometry->GetVertexCount(); ++v)
		{
			float yp = pGeometry->GetVertexData()[v*3+1];
			if (yp >= lPartMin && yp < lPartMax)
			{
				// Make sure it's not part of the caps.
				if (std::find(lMinimaList.begin(), lMinimaList.end(), (int)v) == lMinimaList.end() &&
					std::find(lMaximaList.begin(), lMaximaList.end(), (int)v) == lMaximaList.end())
				{
					lSegmentList.push_back(v);
				}
			}
		}
		if (!lSegmentList.empty())
		{
			lWeightGroup.mBoneCount = 2;
			lWeightGroup.mBoneIndexArray[0] = y;
			lWeightGroup.mBoneIndexArray[1] = y+1;
			lWeightGroup.mVectorIndexCount = (int)lSegmentList.size();
			lWeightGroup.mVectorIndexArray = new int[lWeightGroup.mVectorIndexCount];
			lWeightGroup.mVectorWeightArray = new float[lWeightGroup.mVectorIndexCount*lWeightGroup.mBoneCount];
			std::list<int>::iterator i = lSegmentList.begin();
			for (v = 0; i != lSegmentList.end(); ++i, ++v)
			{
				int idx = *i;
				lWeightGroup.mVectorIndexArray[v] = idx;
				float yp = pGeometry->GetVertexData()[idx*3+1];
				// Scale down to [0, 1].
				yp = (yp-lPartMin)/(lPartMax-lPartMin);
				// Scale up to [0, 2];
				yp *= 2;
				// Move by the power of n towards the closest part.
				const float n = pBoneStiffness;
				yp = ::pow(yp, n);
				// Scale down to [0, 1] again.
				yp /= ::pow(2, n);
				lWeightGroup.mVectorWeightArray[v*2+0] = 1-yp;
				lWeightGroup.mVectorWeightArray[v*2+1] = yp;
			}
			pSkin->AddBoneWeights(lWeightGroup);
		}
	}	

	if (!lMaximaList.empty())
	{
		// Create weights for distal end cap of skinned mesh.
		lWeightGroup.mBoneCount = 1;
		lWeightGroup.mBoneIndexArray[0] = pBoneCount-1;
		lWeightGroup.mVectorIndexCount = (int)lMaximaList.size();
		lWeightGroup.mVectorIndexArray = new int[lWeightGroup.mVectorIndexCount];
		lWeightGroup.mVectorWeightArray = new float[lWeightGroup.mVectorIndexCount*lWeightGroup.mBoneCount];
		std::list<int>::iterator i = lMaximaList.begin();
		for (v = 0; i != lMaximaList.end(); ++i, ++v)
		{
			lWeightGroup.mVectorIndexArray[v] = *i;
			lWeightGroup.mVectorWeightArray[v] = 1;
		}
		pSkin->AddBoneWeights(lWeightGroup);
	}
}



}
