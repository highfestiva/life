/*
	Class:  GeometrySeparator
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games
*/

#include "../Include/UiGeometrySeparator.h"
#include "../Include/UiTriangleBasedGeometry.h"

namespace UiTbc
{

TriangleBasedGeometry* UiGeometrySeparator::GetSeparateObjects(TBC::GeometryBase& pGeom, int& pNumObjects)
{
	pNumObjects = 0;

	if (pGeom.GetPrimitiveType() != TBC::GeometryBase::TRIANGLES)
	{
		return 0;
	}

	unsigned int lVertexCount = pGeom.GetVertexCount();
	unsigned int lTriangleCount = pGeom.GetTriangleCount();
	const Lepra::uint32* lIndices   = pGeom.GetIndexData();
	if (lIndices == 0 || lTriangleCount == 0)
	{
		return 0;
	}

	if (pGeom.GetEdgeData() == 0)
	{
		pGeom.GenerateEdgeData();
	}

	// Triangle-to-edge lookup.
	int* lEdgeIndices = new int[lTriangleCount * 3];

	// Need this to fill the lEdgeIndices correctly.
	int* lEdgeCount = new int[lTriangleCount];

	unsigned int i;
	for (i = 0; i < lTriangleCount; i++)
	{
		lEdgeCount[i] = 0;
	}

	// Setup triangle-to-edge lookup.
	TBC::GeometryBase::Edge* lEdge = pGeom.GetEdgeData();
	for (i = 0; i < pGeom.GetEdgeCount(); i++)
	{
		for (int j = 0; j < lEdge[i].mTriangleCount; j++)
		{
			int lT = lEdge[i].mTriangle[j];
			int lTriangleIndex = lT * 3;
			int lTriangleEdgeCount = lEdgeCount[lT]++;
			lEdgeIndices[lTriangleIndex + lTriangleEdgeCount] = i;
		}
	}

	// One flag per triangle... If this is a single object, all flags
	// will be set to true.
	bool* lTriangleChecked = new bool[lTriangleCount];
	int* lEdgeToCheck = new int[lTriangleCount];
	for (i = 0; i < lTriangleCount; i++)
	{
		lTriangleChecked[i] = false;
		lEdgeToCheck[i] = 0;
	}

	//
	// Count how many objects there are.
	//

	int lFirstNonTestedTriangle = 0;
	int lTrianglesLeft = lTriangleCount;
	int lNumObjects = 0;

	int* lTriangleStack = new int[lTriangleCount];
	int lStackIndex = 0;

	while (lTrianglesLeft > 0)
	{
		// This is a flood fill. Starting from triangle 0, we set its flag to true,
		// and continue doing the same procedure with its neighbour triangles.
		// If we can fill all triangles (setting all flags to 'true'),
		// they are all connected, which means that this is all one object.
		lTriangleStack[0] = lFirstNonTestedTriangle;

		int lCurrentTriangle;

		do
		{
			lCurrentTriangle = lTriangleStack[lStackIndex];

			if (lTriangleChecked[lCurrentTriangle] == false)
			{
				lTriangleChecked[lCurrentTriangle] = true;
				lTrianglesLeft--;
			}

			if (lEdgeToCheck[lCurrentTriangle] >= 3)
			{
				// All edges are already checked, let's fall back to the previous
				// triangle.
				lStackIndex--;
			}
			else
			{
				// Get the edge.
				int lEdgeIndex = lEdgeIndices[lCurrentTriangle * 3 + lEdgeToCheck[lCurrentTriangle]];
				lEdgeToCheck[lCurrentTriangle]++;
				TBC::GeometryBase::Edge* lEdge = &pGeom.GetEdgeData()[lEdgeIndex];

				for (i = 0; i < (unsigned int)lEdge->mTriangleCount; i++)
				{
					int lTriangle = lEdge->mTriangle[i];
					// Check if this isn't the current triangle.
					if (lTriangle != lCurrentTriangle &&
					   lTriangleChecked[lTriangle] == false)
					{
						// It's a neighbour triangle and it's not checked. Put it on the stack.
						lTriangleStack[++lStackIndex] = lTriangle;
					}
				}
			}
		}while(lStackIndex >= 0);	// Continue until we get back to start.

		lNumObjects++;
		lStackIndex = 0;

		if (lTrianglesLeft > 0)
		{
			for (i = 0; i < lTriangleCount; i++)
			{
				if (lTriangleChecked[i] == false)
				{
					lFirstNonTestedTriangle = i;
					break;
				}
			}
		}
	}

	TriangleBasedGeometry* lGeom = new TriangleBasedGeometry[lNumObjects];

	//
	// Now we have to do this all over again to find the right vertices and triangles.
	//

	// Use these arrays to know exactly what vertices and triangles that is used
	// per object. (Reset between objects).
	bool* lVertexChecked = new bool[lVertexCount];
	bool* lTrianglesChecked2 = new bool[lTriangleCount];
	for (i = 0; i < lVertexCount; i++)
	{
		lVertexChecked[i] = false;
	}
	for (i = 0; i < lTriangleCount; i++)
	{
		lTriangleChecked[i] = false;
		lTrianglesChecked2[i] = false;
		lEdgeToCheck[i] = 0;
	}

	// Used to convert the vertex indices correctly when creating new objects.
	int* lIndexConvertionTable = new int[lVertexCount];

	lFirstNonTestedTriangle = 0;
	lTrianglesLeft = lTriangleCount;
	lStackIndex = 0;

	int lCurrentObject = 0;

	while (lTrianglesLeft > 0)
	{
		lTriangleStack[0] = lFirstNonTestedTriangle;

		int lCurrentTriangle;

		do
		{
			lCurrentTriangle = lTriangleStack[lStackIndex];

			if (lTriangleChecked[lCurrentTriangle] == false)
			{
				lTriangleChecked[lCurrentTriangle] = true;
				lTrianglesChecked2[lCurrentTriangle] = true;
				lTrianglesLeft--;

				int lV0 = lIndices[lCurrentTriangle * 3 + 0];
				int lV1 = lIndices[lCurrentTriangle * 3 + 1];
				int lV2 = lIndices[lCurrentTriangle * 3 + 2];

				lVertexChecked[lV0] = true;
				lVertexChecked[lV1] = true;
				lVertexChecked[lV2] = true;
			}

			if (lEdgeToCheck[lCurrentTriangle] >= 3)
			{
				// All edges are already checked, let's fall back to the previous
				// triangle.
				lStackIndex--;
			}
			else
			{
				// Get the edge.
				int lEdgeIndex = lEdgeIndices[lCurrentTriangle * 3 + lEdgeToCheck[lCurrentTriangle]];
				lEdgeToCheck[lCurrentTriangle]++;
				TBC::GeometryBase::Edge* lEdge = &pGeom.GetEdgeData()[lEdgeIndex];

				for (i = 0; i < (unsigned int)lEdge->mTriangleCount; i++)
				{
					int lTriangle = lEdge->mTriangle[i];
					// Check if this isn't the current triangle.
					if (lTriangle != lCurrentTriangle &&
					   lTriangleChecked[lTriangle] == false)
					{
						// It's a neighbour triangle and it's not checked. Put it on the stack.
						lTriangleStack[++lStackIndex] = lTriangle;
					}
				}
			}
		}while(lStackIndex >= 0);	// Continue until we get back to start.

		//
		// Count triangles and vertices in this object.
		//

		int lObjTriangleCount = 0;
		int lObjVertexCount = 0;

		for (i = 0; i < lVertexCount; i++)
		{
			if (lVertexChecked[i] == true)
			{
				lObjVertexCount++;
			}
		}
		for (i = 0; i < lTriangleCount; i++)
		{
			if (lTrianglesChecked2[i] == true)
			{
				lObjTriangleCount++;
			}
		}

		//
		// Create and copy vertex- and triangle data.
		//

		float* lNewVertexData = new float[lObjVertexCount * 3];
		Lepra::uint32* lNewIndices = new Lepra::uint32[lObjTriangleCount * 3];

		float** lNewUVData = 0;
		Lepra::uint8* lNewColorData = 0;

		if (pGeom.GetUVData(0) != 0)
		{
			lNewUVData = new float*[pGeom.GetUVSetCount()];
			for (i = 0; i < pGeom.GetUVSetCount(); i++)
			{
				lNewUVData[i] = new float[lVertexCount * 2];
			}
		}
		if (pGeom.GetColorData() != 0)
		{
			int lSize = 3;
			if (pGeom.GetColorFormat() == TBC::GeometryBase::COLOR_RGBA)
				lSize = 4;
			lNewColorData = new Lepra::uint8[lVertexCount * lSize];
		}

		int lWriteIndex = 0;
		for (i = 0; i < pGeom.GetVertexCount(); i++)
		{
			if (lVertexChecked[i] == true)
			{
				float* lVertexData = pGeom.GetVertexData();

				lNewVertexData[lWriteIndex * 3 + 0] = lVertexData[i * 3 + 0];
				lNewVertexData[lWriteIndex * 3 + 1] = lVertexData[i * 3 + 1];
				lNewVertexData[lWriteIndex * 3 + 2] = lVertexData[i * 3 + 2];

				if (pGeom.GetUVData(0) != 0)
				{
					for (unsigned int j = 0; j < pGeom.GetUVSetCount(); j++)
					{
						lNewUVData[j][lWriteIndex * 2 + 0] = pGeom.GetUVData(j)[i * 2 + 0];
						lNewUVData[j][lWriteIndex * 2 + 1] = pGeom.GetUVData(j)[i * 2 + 1];
					}
				}

				if (pGeom.GetColorData() != 0)
				{
					int lSize = 3;
					if (pGeom.GetColorFormat() == TBC::GeometryBase::COLOR_RGBA)
						lSize = 4;

					lNewColorData[lWriteIndex * lSize + 0] = pGeom.GetColorData()[i * lSize + 0];
					lNewColorData[lWriteIndex * lSize + 1] = pGeom.GetColorData()[i * lSize + 1];
					lNewColorData[lWriteIndex * lSize + 2] = pGeom.GetColorData()[i * lSize + 2];

					if (lSize == 4)
						lNewColorData[lWriteIndex * lSize + 3] = pGeom.GetColorData()[i * lSize + 3];
				}

				lIndexConvertionTable[i] = lWriteIndex;
				lWriteIndex++;
			}
		}

		lWriteIndex = 0;
		for (i = 0; i < pGeom.GetTriangleCount(); i++)
		{
			if (lTrianglesChecked2[i] == true)
			{
				int lI0 = pGeom.GetIndexData()[i * 3 + 0];
				int lI1 = pGeom.GetIndexData()[i * 3 + 1];
				int lI2 = pGeom.GetIndexData()[i * 3 + 2];

				lNewIndices[lWriteIndex * 3 + 0] = lIndexConvertionTable[lI0];
				lNewIndices[lWriteIndex * 3 + 1] = lIndexConvertionTable[lI1];
				lNewIndices[lWriteIndex * 3 + 2] = lIndexConvertionTable[lI2];
				lWriteIndex++;
			}
		}

		lGeom[lCurrentObject].Set(lNewVertexData, 0, 
					      pGeom.GetUVData(0) == 0 ? 0 : lNewUVData[0],
					      lNewColorData,
					      pGeom.GetColorFormat(),
					      lNewIndices,
					      lObjVertexCount,
					      lObjTriangleCount,
					      TBC::GeometryBase::TRIANGLES,
					      pGeom.GetGeometryVolatility());

		for (i = 1; i < pGeom.GetUVSetCount(); i++)
		{
			lGeom[lCurrentObject].AddUVSet(lNewUVData[i]);
		}

		delete[] lNewVertexData;
		delete[] lNewIndices;

		if (lNewUVData != 0)
		{
			for (i = 0; i < pGeom.GetUVSetCount(); i++)
			{
				delete[] lNewUVData[i];
			}
			delete[] lNewUVData;
		}
		
		if (lNewColorData != 0)
		{
			delete[] lNewColorData;
		}


		//
		// Reset everything and go on with the next object.
		//

		lStackIndex = 0;

		for (i = 0; i < pGeom.GetVertexCount(); i++)
		{
			lVertexChecked[i] = false;
		}
		for (i = 0; i < pGeom.GetTriangleCount(); i++)
		{
			lTrianglesChecked2[i] = false;
		}

		if (lTrianglesLeft > 0)
		{
			for (i = 0; i < pGeom.GetTriangleCount(); i++)
			{
				if (lTriangleChecked[i] == false)
				{
					lFirstNonTestedTriangle = i;
					break;
				}
			}
		}

		lCurrentObject++;
	}

	delete[] lTriangleChecked;
	delete[] lEdgeIndices;
	delete[] lEdgeCount;
	delete[] lEdgeToCheck;
	delete[] lTriangleStack;
	delete[] lVertexChecked;
	delete[] lTrianglesChecked2;
	delete[] lIndexConvertionTable;

	pNumObjects = lNumObjects;
	return lGeom;
}

} // End namespace.
