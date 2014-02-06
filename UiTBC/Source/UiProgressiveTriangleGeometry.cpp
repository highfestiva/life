
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../Include/UiProgressiveTriangleGeometry.h"
#include "../../Lepra/Include/LepraAssert.h"
#include "../../Lepra/Include/ListUtil.h"



namespace UiTbc
{



ProgressiveTriangleGeometry::ProgressiveTriangleGeometry():
	mBaseVertexCount(0),
	mBaseTriangleCount(0),

	mCurrentVertexCount(0),
	mCurrentTriangleCount(0),

	mMaxVertexCount(0),
	mMaxTriangleCount(0),

	mCurrentVertexData(0),
	mCurrentUVData(0),
	mCurrentColorData(0),
	mCurrentNormalData(0),
	mCurrentColorData8(0),
	mCurrentIndices(0),

	mBaseVertexData(0),
	mBaseUVData(0),
	mBaseColorData(0),
	mBaseNormalData(0),
	mBaseIndices(0),

	mCurrentVSplit(0),
	mNumVertexSplits(0),
	mVertexSplit(0),

	mColorFormat(TBC::GeometryBase::COLOR_RGBA)
{
}

ProgressiveTriangleGeometry::ProgressiveTriangleGeometry(ProgressiveTriangleGeometry& pProgressiveGeometry):
	mBaseVertexCount(0),
	mBaseTriangleCount(0),

	mCurrentVertexCount(0),
	mCurrentTriangleCount(0),

	mMaxVertexCount(0),
	mMaxTriangleCount(0),

	mCurrentVertexData(0),
	mCurrentUVData(0),
	mCurrentColorData(0),
	mCurrentNormalData(0),
	mCurrentColorData8(0),
	mCurrentIndices(0),

	mBaseVertexData(0),
	mBaseUVData(0),
	mBaseColorData(0),
	mBaseNormalData(0),
	mBaseIndices(0),

	mCurrentVSplit(0),
	mNumVertexSplits(0),
	mVertexSplit(0),

	mColorFormat(TBC::GeometryBase::COLOR_RGBA)
{
	Copy(pProgressiveGeometry);
}

ProgressiveTriangleGeometry::ProgressiveTriangleGeometry(TriangleBasedGeometry& pGeometry):
	mBaseVertexCount(0),
	mBaseTriangleCount(0),

	mCurrentVertexCount(0),
	mCurrentTriangleCount(0),

	mMaxVertexCount(0),
	mMaxTriangleCount(0),

	mCurrentVertexData(0),
	mCurrentUVData(0),
	mCurrentColorData(0),
	mCurrentNormalData(0),
	mCurrentColorData8(0),
	mCurrentIndices(0),

	mBaseVertexData(0),
	mBaseUVData(0),
	mBaseColorData(0),
	mBaseNormalData(0),
	mBaseIndices(0),

	mCurrentVSplit(0),
	mNumVertexSplits(0),
	mVertexSplit(0),

	mColorFormat(TBC::GeometryBase::COLOR_RGBA)
{
	Set(pGeometry);
}

ProgressiveTriangleGeometry::~ProgressiveTriangleGeometry()
{
	ClearAll();
}

void ProgressiveTriangleGeometry::ClearAll()
{
	delete[] mCurrentVertexData;
	delete[] mCurrentUVData;
	delete[] mCurrentColorData;
	delete[] mCurrentColorData8;
	delete[] mCurrentIndices;
	delete[] mBaseVertexData;
	delete[] mBaseUVData;
	delete[] mBaseColorData;
	delete[] mBaseIndices;
	delete[] mVertexSplit;

	mCurrentVertexData = 0;
	mCurrentUVData = 0;
	mCurrentColorData = 0;
	mCurrentColorData8 = 0;
	mCurrentIndices = 0;
	mBaseVertexData = 0;
	mBaseUVData = 0;
	mBaseColorData = 0;
	mBaseIndices = 0;
	mVertexSplit = 0;

	mMaxVertexCount       = 0;
	mMaxTriangleCount     = 0;

	mBaseVertexCount      = 0;
	mBaseTriangleCount    = 0;

	mCurrentVertexCount   = 0;
	mCurrentTriangleCount = 0;

	mNumVertexSplits      = 0;

	TBC::GeometryBase::SetBoundingRadius(0.0f);
}

void ProgressiveTriangleGeometry::Copy(ProgressiveTriangleGeometry& pProgressiveGeometry)
{
	mMaxVertexCount	      = pProgressiveGeometry.mMaxVertexCount;
	mMaxTriangleCount	  = pProgressiveGeometry.mMaxTriangleCount;

	mBaseVertexCount	  = pProgressiveGeometry.mBaseVertexCount;
	mBaseTriangleCount	  = pProgressiveGeometry.mBaseTriangleCount;

	mCurrentVertexCount	  = pProgressiveGeometry.mCurrentVertexCount;
	mCurrentTriangleCount = pProgressiveGeometry.mCurrentTriangleCount;

	mCurrentVertexData	  = 0;
	mCurrentUVData	      = 0;
	mCurrentColorData	  = 0;
	mCurrentColorData8	  = 0;
	mCurrentIndices	      = 0;

	mBaseVertexData	      = 0;
	mBaseUVData		      = 0;
	mBaseColorData	      = 0;
	mBaseIndices	      = 0;

	mVertexSplit		  = 0;
	mCurrentVSplit	      = pProgressiveGeometry.mCurrentVSplit;
	mNumVertexSplits	  = pProgressiveGeometry.mNumVertexSplits;

	mColorFormat		  = pProgressiveGeometry.mColorFormat;

	SetTransformation(pProgressiveGeometry.GetTransformation());

	const TBC::GeometryBase::BasicMaterialSettings& lMatSettings = pProgressiveGeometry.GetBasicMaterialSettings();
	TBC::GeometryBase::SetBasicMaterialSettings(lMatSettings);

	SetLastFrameVisible(pProgressiveGeometry.GetLastFrameVisible());

	TBC::GeometryBase::SetRendererData(pProgressiveGeometry.GetRendererData());

	int i;
	// Copy the current state data.

	if (pProgressiveGeometry.mCurrentVertexData != 0)
	{
		int lNumElements = (mBaseVertexCount + mNumVertexSplits) * 3;
		mCurrentVertexData = new float[lNumElements];
		for (i = 0; i < lNumElements; i++)
		{
			mCurrentVertexData[i] = pProgressiveGeometry.mCurrentVertexData[i];
		}
	}

	if (pProgressiveGeometry.mCurrentUVData != 0)
	{
		int lNumElements = (mBaseVertexCount + mNumVertexSplits) * 2;
		mCurrentUVData = new float[lNumElements];
		for (i = 0; i < lNumElements; i++)
		{
			mCurrentUVData[i] = pProgressiveGeometry.mCurrentUVData[i];
		}
	}

	if (pProgressiveGeometry.mCurrentColorData != 0)
	{
		int lNumElements = (mBaseVertexCount + mNumVertexSplits) * 4;
		mCurrentColorData = new float[lNumElements];
		for (i = 0; i < lNumElements; i++)
		{
			mCurrentColorData[i] = pProgressiveGeometry.mCurrentColorData[i];
		}
	}

	if (pProgressiveGeometry.mCurrentColorData8 != 0)
	{
		int lNumElements = (mBaseVertexCount + mNumVertexSplits) * 4;
		mCurrentColorData8 = new uint8[lNumElements];
		for (i = 0; i < lNumElements; i++)
		{
			mCurrentColorData8[i] = pProgressiveGeometry.mCurrentColorData8[i];
		}
	}

	if (pProgressiveGeometry.mCurrentIndices != 0)
	{
		int lNumElements = (mBaseTriangleCount + mNumVertexSplits * 2) * 3;
		mCurrentIndices = new uint32[lNumElements];
		for (i = 0; i < lNumElements; i++)
		{
			mCurrentIndices[i] = pProgressiveGeometry.mCurrentIndices[i];
		}
	}

	// Copy the base mesh data.
	if (pProgressiveGeometry.mBaseVertexData != 0)
	{
		int lNumElements = mBaseVertexCount * 3;
		mBaseVertexData = new float[lNumElements];
		for (i = 0; i < lNumElements; i++)
		{
			mBaseVertexData[i] = pProgressiveGeometry.mBaseVertexData[i];
		}
	}

	if (pProgressiveGeometry.mBaseUVData != 0)
	{
		int lNumElements = mBaseVertexCount * 2;
		mBaseUVData = new float[lNumElements];
		for (i = 0; i < lNumElements; i++)
		{
			mBaseUVData[i] = pProgressiveGeometry.mBaseUVData[i];
		}
	}

	if (pProgressiveGeometry.mBaseColorData != 0)
	{
		int lNumElements = mBaseVertexCount * 4;
		mBaseColorData = new float[lNumElements];
		for (i = 0; i < lNumElements; i++)
		{
			mBaseColorData[i] = pProgressiveGeometry.mBaseColorData[i];
		}
	}

	if (pProgressiveGeometry.mBaseIndices != 0)
	{
		int lNumElements = mBaseTriangleCount * 3;
		mBaseIndices = new uint32[lNumElements];
		for (i = 0; i < lNumElements; i++)
		{
			mBaseIndices[i] = pProgressiveGeometry.mBaseIndices[i];
		}
	}

	if (pProgressiveGeometry.mVertexSplit != 0)
	{
		int lNumElements = mNumVertexSplits;
		mVertexSplit = new VertexSplit[lNumElements];
		for (i = 0; i < lNumElements; i++)
		{
			mVertexSplit[i].Copy(pProgressiveGeometry.mVertexSplit[i]);
		}
	}

	TBC::GeometryBase::SetBoundingRadius(pProgressiveGeometry.GetBoundingRadius());
}

void ProgressiveTriangleGeometry::FindEdgeToCollapse(VertexList& /*pOrgVertexList*/,
	TriangleList& /*pOrgTriangleList*/, VertexList& pVertexList, TriangleList& pTriangleList, Edge& pEdge)
{
	if (pTriangleList.empty())
		return;

	float lCurrentError = 0.0f;
	float lCurrentMaxError = 0.0f;

	pEdge.mV1 = 0;
	pEdge.mV2 = 0;
	pEdge.mTriangleList.clear();

	// Calculate mean error.
	VertexList::iterator lVIter;
	for (lVIter = pVertexList.begin();
	     lVIter != pVertexList.end();
	     ++lVIter)
	{
		if (lVIter == pVertexList.begin())
		{
			lCurrentMaxError = (*lVIter)->mError;
		}
		else if((*lVIter)->mError > lCurrentMaxError)
		{
			lCurrentMaxError = (*lVIter)->mError;
		}

		lCurrentError += (*lVIter)->mError;
	}

	lCurrentError /= (float)pVertexList.size();

	float lMinError = 0.0f;
	float lMinError1 = 0.0f;	// Temporary error value storage for lEdge->mV1.
	float lMinError2 = 0.0f;	// Temporary error value storage for lEdge->mV2.
	bool lFirst = true;

	// Test vertex collaps on all triangles.
	TriangleList::iterator lTriIter;
	for (lTriIter = pTriangleList.begin(); 
	     lTriIter != pTriangleList.end(); 
	     ++lTriIter)
	{
		Triangle* lTriangle = *lTriIter;
		Vector3DF lDiff1;
		Vector3DF lDiff2;
		Vertex lMid;

		// Test V1 and V2.
		lMid.x() = (lTriangle->mV1->x() + lTriangle->mV2->x()) * 0.5f;
		lMid.y() = (lTriangle->mV1->y() + lTriangle->mV2->y()) * 0.5f;
		lMid.z() = (lTriangle->mV1->z() + lTriangle->mV2->z()) * 0.5f;

		// Calculate the distance between the new, merged position,
		// and the original vertex position.
		lDiff1.Set(lMid.x() - lTriangle->mV1->mTwin->x(),
		            lMid.y() - lTriangle->mV1->mTwin->y(),
		            lMid.z() - lTriangle->mV1->mTwin->z());
		lDiff2.Set(lMid.x() - lTriangle->mV2->mTwin->x(),
		            lMid.y() - lTriangle->mV2->mTwin->y(),
		            lMid.z() - lTriangle->mV2->mTwin->z());

		float lError1 = lDiff1.GetLength() + lTriangle->mV1->mError;
		float lError2 = lDiff2.GetLength() + lTriangle->mV2->mError;
		float lError = (lError1 + lError2 + lCurrentError) / 3.0f;

		if (lFirst == true || lError < lMinError)
		{
			pEdge.mV1 = lTriangle->mV1;
			pEdge.mV2 = lTriangle->mV2;
			lMinError1 = lError1;
			lMinError2 = lError2;
			lMinError = lError;
			lFirst = false;
		}

		// Test V2 and V3.
		lMid.x() = (lTriangle->mV2->x() + lTriangle->mV3->x()) * 0.5f;
		lMid.y() = (lTriangle->mV2->y() + lTriangle->mV3->y()) * 0.5f;
		lMid.z() = (lTriangle->mV2->z() + lTriangle->mV3->z()) * 0.5f;

		// Calculate the distance between the new, merged position,
		// and the original vertex position.
		lDiff1.Set(lMid.x() - lTriangle->mV2->mTwin->x(),
		            lMid.y() - lTriangle->mV2->mTwin->y(),
		            lMid.z() - lTriangle->mV2->mTwin->z());
		lDiff2.Set(lMid.x() - lTriangle->mV3->mTwin->x(),
		            lMid.y() - lTriangle->mV3->mTwin->y(),
		            lMid.z() - lTriangle->mV3->mTwin->z());
		
		lError1 = lDiff1.GetLength() + lTriangle->mV1->mError;
		lError2 = lDiff2.GetLength() + lTriangle->mV2->mError;
		lError = (lError1 + lError2 + lCurrentError) / 3.0f;

		if (lError < lMinError)
		{
			pEdge.mV1 = lTriangle->mV1;
			pEdge.mV2 = lTriangle->mV2;
			lMinError = lError;
			lMinError1 = lError1;
			lMinError2 = lError2;
		}

		// Test V3 and V1.
		lMid.x() = (lTriangle->mV3->x() + lTriangle->mV1->x()) * 0.5f;
		lMid.y() = (lTriangle->mV3->y() + lTriangle->mV1->y()) * 0.5f;
		lMid.z() = (lTriangle->mV3->z() + lTriangle->mV1->z()) * 0.5f;

		// Calculate the distance between the new, merged position,
		// and the original vertex position.
		lDiff1.Set(lMid.x() - lTriangle->mV3->mTwin->x(),
		            lMid.y() - lTriangle->mV3->mTwin->y(),
		            lMid.z() - lTriangle->mV3->mTwin->z());
		lDiff2.Set(lMid.x() - lTriangle->mV1->mTwin->x(),
		            lMid.y() - lTriangle->mV1->mTwin->y(),
		            lMid.z() - lTriangle->mV1->mTwin->z());
		
		lError1 = lDiff1.GetLength() + lTriangle->mV1->mError;
		lError2 = lDiff2.GetLength() + lTriangle->mV2->mError;
		lError = (lError1 + lError2 + lCurrentError) / 3.0f;

		if (lError < lMinError)
		{
			pEdge.mV1 = lTriangle->mV1;
			pEdge.mV2 = lTriangle->mV2;
			lMinError = lError;
			lMinError1 = lError1;
			lMinError2 = lError2;
		}

		if (lMinError == 0.0f && pEdge.mV1 != 0 && pEdge.mV2 != 0)
			break;
	}
	
	pEdge.mV1->mError = lMinError1;
	pEdge.mV2->mError = lMinError2;

	// Now add all triangles to lEdge that share the two vertices
	// lEdge->mV1 and lEdge->mV2.
	for (lTriIter = pTriangleList.begin(); 
		lTriIter != pTriangleList.end(); 
		++lTriIter)
	{
		Triangle* lTriangle = *lTriIter;
		if (lTriangle->HaveVertex(pEdge.mV1) &&
			lTriangle->HaveVertex(pEdge.mV2))
		{
			pEdge.mTriangleList.push_back(lTriangle);
		}
	}
}

void ProgressiveTriangleGeometry::Set(TriangleBasedGeometry& pGeometry)
{
	int i;
	ClearAll();

	mMaxVertexCount  = pGeometry.GetMaxVertexCount();
	mMaxTriangleCount = pGeometry.GetMaxTriangleCount();

	TBC::GeometryBase::SetBoundingRadius(pGeometry.GetBoundingRadius());

	TBC::GeometryBase::SetBasicMaterialSettings(pGeometry.GetBasicMaterialSettings());

	SetLastFrameVisible(pGeometry.GetLastFrameVisible());
	SetTransformation(pGeometry.GetTransformation());

	VertexList   lVertexList;
	TriangleList lTriangleList;
	VertexList   lOrgVertexList;
	TriangleList lOrgTriangleList;
	VertexSplitList lVertexSplitList;

	bool lClearNormalData = (pGeometry.GetNormalData() != 0);
	pGeometry.GenerateVertexNormalData();

	/*
		Copy vertex, uv and color data.
	*/

	for (i = 0; i < (int)pGeometry.GetVertexCount(); i++)
	{
		Vertex* lVertex = new Vertex;
		lVertex->x()  = pGeometry.GetVertexData()[i * 3 + 0];
		lVertex->y()  = pGeometry.GetVertexData()[i * 3 + 1];
		lVertex->z()  = pGeometry.GetVertexData()[i * 3 + 2];
		lVertex->nx() = pGeometry.GetNormalData()[i * 3 + 0];
		lVertex->ny() = pGeometry.GetNormalData()[i * 3 + 1];
		lVertex->nz() = pGeometry.GetNormalData()[i * 3 + 2];

		if (pGeometry.GetUVData() != 0)
		{
			lVertex->u() = pGeometry.GetUVData()[i * 2 + 0];
			lVertex->v() = pGeometry.GetUVData()[i * 2 + 1];
		}
		else
		{
			lVertex->u() = 0.0f;
			lVertex->v() = 0.0f;
		}

		if (pGeometry.GetColorData() != 0)
		{
			int lSize = 4;
			if (pGeometry.GetColorFormat() == TBC::GeometryBase::COLOR_RGB)
				lSize = 3;

			lVertex->r() = (float)pGeometry.GetColorData()[i * lSize + 0] / 255.0f;
			lVertex->g() = (float)pGeometry.GetColorData()[i * lSize + 1] / 255.0f;
			lVertex->b() = (float)pGeometry.GetColorData()[i * lSize + 2] / 255.0f;
			lVertex->a() = 1.0f;

			if (lSize == 4)
			{
				lVertex->a() = (float)pGeometry.GetColorData()[i * lSize + 3] / 255.0f;
			}
		}
		else
		{
			lVertex->r() = 0.0f;
			lVertex->g() = 0.0f;
			lVertex->b() = 0.0f;
			lVertex->a() = 1.0f;
		}

		lVertexList.push_back(lVertex);

		Vertex* lVCopy = new Vertex(lVertex);
		lVertex->mTwin = lVCopy;
		// Keep a copy of the original mesh.
		lOrgVertexList.push_back(lVCopy);
	}

	/*
		Copy triangles.
	*/

	int lTriangleCount = (int)pGeometry.GetTriangleCount();
	for (i = 0; i < lTriangleCount; i++)
	{
		Triangle* lTriangle = new Triangle;
		Triangle* lTriangle2 = new Triangle;

		uint32 lIndices[4];
		pGeometry.GetTriangleIndices(i, lIndices);

		lTriangle->mV1 = *ListUtil::FindByIndex(lVertexList, lIndices[0]);
		lTriangle->mV2 = *ListUtil::FindByIndex(lVertexList, lIndices[1]);
		lTriangle->mV3 = *ListUtil::FindByIndex(lVertexList, lIndices[2]);

		lTriangle2->mV1 = *ListUtil::FindByIndex(lOrgVertexList, lIndices[0]);
		lTriangle2->mV2 = *ListUtil::FindByIndex(lOrgVertexList, lIndices[1]);
		lTriangle2->mV3 = *ListUtil::FindByIndex(lOrgVertexList, lIndices[2]);

		lTriangleList.push_back(lTriangle);
		lOrgTriangleList.push_back(lTriangle2);
	}

	/*
		The main loop.
	*/

	Edge lEdgeToCollapse;

	while (!lTriangleList.empty())
	{
		// FindEdgeToCollapse() will put the collapsing edge last in the array.
		FindEdgeToCollapse(lOrgVertexList,
				   lOrgTriangleList,
				   lVertexList,
				   lTriangleList,
				   lEdgeToCollapse);

		VertexSplit* lVertexSplit = new VertexSplit;
		lVertexSplitList.push_back(lVertexSplit);

		lVertexSplit->mVertexToSplit = lEdgeToCollapse.mV2;
		lVertexSplit->mNewVertex = lEdgeToCollapse.mV1;

		lVertexSplit->mNumOldTriangles = (int)(lTriangleList.size() - lEdgeToCollapse.mTriangleList.size());
		lVertexSplit->mNumNewTriangles = (int)lEdgeToCollapse.mTriangleList.size();

		SetDeltaScalars(lVertexSplit->mDeltaScalars,
				lEdgeToCollapse.mV1->mScalars,
				lEdgeToCollapse.mV2->mScalars);

		CopyAddScalars(lVertexSplit->mPivotScalars,
				lEdgeToCollapse.mV2->mScalars,
				lVertexSplit->mDeltaScalars);

		CopyScalars(lEdgeToCollapse.mV2->mScalars,
				lVertexSplit->mPivotScalars);

		TriangleList::iterator lTriIter;

		for (lTriIter = lEdgeToCollapse.mTriangleList.begin();
			lTriIter != lEdgeToCollapse.mTriangleList.end();
			++lTriIter)
		{
			Triangle* lTriangle = *lTriIter;
			lTriangleList.remove(lTriangle);
			lVertexSplit->mNewTriangles.push_back(lTriangle);
		}

		// Setup the affected triangles. Affected triangles are those that
		// are sharing the vertex that will be removed. We need to change the pointer that
		// points at the removed vertex to point to the other vertex instead.
		for (lTriIter =  lTriangleList.begin();
			lTriIter != lTriangleList.end();
			++lTriIter)
		{
			Triangle* lTriangle = *lTriIter;

			if (lTriangle->HaveVertex(lEdgeToCollapse.mV1) == true)
			{
				lVertexSplit->mFixTriangles.push_back(lTriangle);
				lTriangle->ReplaceVertex(lEdgeToCollapse.mV1, lVertexSplit->mVertexToSplit);

				// If two vertices are the same.
				if (lTriangle->mV1 == lTriangle->mV2 ||
				   lTriangle->mV1 == lTriangle->mV3 ||
				   lTriangle->mV2 == lTriangle->mV3)
				{
					deb_assert(false);
				}
			}
		}

		// Place V2 last in the list, and remove V1 completely.
		lVertexList.remove(lEdgeToCollapse.mV1);
		lVertexList.remove(lEdgeToCollapse.mV2);
		lVertexList.push_back(lEdgeToCollapse.mV2);
	} // End while(lVertexCount > 1)


	// Now we are all done calculating the vertex splits.
	// It's time to generate the final data.

	// Start with creating the lowest level mesh data.
	mBaseVertexCount = (int)lVertexList.size();
	mBaseTriangleCount = (int)lTriangleList.size();
	mNumVertexSplits = (int)lVertexSplitList.size();

	mCurrentVertexCount = mBaseVertexCount;
	mCurrentTriangleCount = mBaseTriangleCount;

	mBaseVertexData = new float[mBaseVertexCount * 3];
	mBaseNormalData = new float[mBaseVertexCount * 3];
	mCurrentVertexData = new float[(mCurrentVertexCount + mNumVertexSplits) * 3];
	mCurrentNormalData = new float[(mCurrentVertexCount + mNumVertexSplits) * 3];

	if (pGeometry.GetUVData() != 0)
	{
		mBaseUVData = new float[mBaseVertexCount * 2];
		mCurrentUVData = new float[(mCurrentVertexCount + mNumVertexSplits) * 2];
	}
	if (pGeometry.GetColorData() != 0)
	{
		mBaseColorData = new float[mBaseVertexCount * 4];
		mCurrentColorData = new float[(mCurrentVertexCount + mNumVertexSplits) * 4];
		mCurrentColorData8 = new uint8[(mCurrentVertexCount + mNumVertexSplits) * 4];
	}

	VertexList::iterator lVertexIter;

	for (lVertexIter = lVertexList.begin(), i = 0; 
		lVertexIter != lVertexList.end(); 
		++lVertexIter, ++i)
	{
		Vertex* lVertex = *lVertexIter;

		mBaseVertexData[i * 3 + 0] = lVertex->x();
		mBaseVertexData[i * 3 + 1] = lVertex->y();
		mBaseVertexData[i * 3 + 2] = lVertex->z();
		mCurrentVertexData[i * 3 + 0] = lVertex->x();
		mCurrentVertexData[i * 3 + 1] = lVertex->y();
		mCurrentVertexData[i * 3 + 2] = lVertex->z();

		mBaseNormalData[i * 3 + 0] = lVertex->nx();
		mBaseNormalData[i * 3 + 1] = lVertex->ny();
		mBaseNormalData[i * 3 + 2] = lVertex->nz();
		mCurrentNormalData[i * 3 + 0] = lVertex->nx();
		mCurrentNormalData[i * 3 + 1] = lVertex->ny();
		mCurrentNormalData[i * 3 + 2] = lVertex->nz();

		if (pGeometry.GetUVData() != 0)
		{
			mBaseUVData[i * 2 + 0] = lVertex->u();
			mBaseUVData[i * 2 + 1] = lVertex->v();

			mCurrentUVData[i * 2 + 0] = lVertex->u();
			mCurrentUVData[i * 2 + 1] = lVertex->v();
		}

		if (pGeometry.GetColorData() != 0)
		{
			mBaseColorData[i * 4 + 0] = lVertex->r();
			mBaseColorData[i * 4 + 1] = lVertex->g();
			mBaseColorData[i * 4 + 2] = lVertex->b();
			mBaseColorData[i * 4 + 3] = lVertex->a();

			mCurrentColorData[i * 4 + 0] = lVertex->r();
			mCurrentColorData[i * 4 + 1] = lVertex->g();
			mCurrentColorData[i * 4 + 2] = lVertex->b();
			mCurrentColorData[i * 4 + 3] = lVertex->a();

			mCurrentColorData8[i * 4 + 0] = (uint8)(lVertex->r() * 255.0f);
			mCurrentColorData8[i * 4 + 1] = (uint8)(lVertex->g() * 255.0f);
			mCurrentColorData8[i * 4 + 2] = (uint8)(lVertex->b() * 255.0f);
			mCurrentColorData8[i * 4 + 3] = (uint8)(lVertex->a() * 255.0f);
		}
	}

	mCurrentIndices = new uint32[lTriangleList.size() * 3];

	if (lTriangleList.empty() == false)
	{
		mBaseIndices = new uint32[lTriangleList.size() * 3];

		TriangleList::iterator lTriIter;
		for (lTriIter = lTriangleList.begin(), i = 0;
			lTriIter != lTriangleList.end();
			++lTriIter, ++i)
		{
			Triangle* lTriangle = *lTriIter;

			unsigned long lIndex0 = ListUtil::FindIndexOf(lVertexList, lTriangle->mV1);
			unsigned long lIndex1 = ListUtil::FindIndexOf(lVertexList, lTriangle->mV2);
			unsigned long lIndex2 = ListUtil::FindIndexOf(lVertexList, lTriangle->mV3);

			deb_assert(lIndex0 >= 0);
			deb_assert(lIndex1 >= 0);
			deb_assert(lIndex2 >= 0);

			mBaseIndices[i * 3 + 0] = lIndex0;
			mBaseIndices[i * 3 + 1] = lIndex1;
			mBaseIndices[i * 3 + 2] = lIndex2;
			mCurrentIndices[i * 3 + 0] = lIndex0;
			mCurrentIndices[i * 3 + 1] = lIndex1;
			mCurrentIndices[i * 3 + 2] = lIndex2;
		}
	}

	// Now let's create the rest of the data needed.

	if (lVertexSplitList.size() > 0)
	{
		int lVertexIndex = (int)lVertexList.size();

		mVertexSplit = new VertexSplit[lVertexSplitList.size()];

		VertexSplitList::reverse_iterator lVSIter;

		for (lVSIter = lVertexSplitList.rbegin(), i = 0; 
			lVSIter != lVertexSplitList.rend(); 
			++lVSIter, i++)
		{
			VertexSplit* lVS = *lVSIter;
			mVertexSplit[i].LightCopy(*lVS);
			mVertexSplit[i].mNumNewTriangles    = (int)lVS->mNewTriangles.size();
			mVertexSplit[i].mNumOldTriangles    = (int)lTriangleList.size();
			mVertexSplit[i].mNumOldVertices     = (int)lVertexList.size();
			mVertexSplit[i].mVertexToSplitIndex = ListUtil::FindIndexOf(lVertexList, lVS->mVertexToSplit);
			mVertexSplit[i].mNumIndexFixes      = (int)lVS->mFixTriangles.size();
			mVertexSplit[i].mIndexFix           = new int[mVertexSplit[i].mNumIndexFixes];
			mVertexSplit[i].mIndexFixIndex      = new int[mVertexSplit[i].mNumIndexFixes * 2];

			// Write new vertex data.
			lVertexList.push_back(lVS->mNewVertex);

			mCurrentVertexData[lVertexIndex * 3 + 0] = lVS->mNewVertex->x();
			mCurrentVertexData[lVertexIndex * 3 + 1] = lVS->mNewVertex->y();
			mCurrentVertexData[lVertexIndex * 3 + 2] = lVS->mNewVertex->z();

			mCurrentNormalData[lVertexIndex * 3 + 0] = lVS->mNewVertex->nx();
			mCurrentNormalData[lVertexIndex * 3 + 1] = lVS->mNewVertex->ny();
			mCurrentNormalData[lVertexIndex * 3 + 2] = lVS->mNewVertex->nz();

			if (pGeometry.GetUVData() != 0)
			{
				mCurrentUVData[lVertexIndex * 2 + 0] = lVS->mNewVertex->u();
				mCurrentUVData[lVertexIndex * 2 + 1] = lVS->mNewVertex->v();
			}

			if (pGeometry.GetColorData() != 0)
			{
				mCurrentColorData[lVertexIndex * 4 + 0] = lVS->mNewVertex->r();
				mCurrentColorData[lVertexIndex * 4 + 1] = lVS->mNewVertex->g();
				mCurrentColorData[lVertexIndex * 4 + 2] = lVS->mNewVertex->b();
				mCurrentColorData[lVertexIndex * 4 + 3] = lVS->mNewVertex->a();

				mCurrentColorData8[lVertexIndex * 4 + 0] = (uint8)(lVS->mNewVertex->r() * 255.0f);
				mCurrentColorData8[lVertexIndex * 4 + 1] = (uint8)(lVS->mNewVertex->g() * 255.0f);
				mCurrentColorData8[lVertexIndex * 4 + 2] = (uint8)(lVS->mNewVertex->b() * 255.0f);
				mCurrentColorData8[lVertexIndex * 4 + 3] = (uint8)(lVS->mNewVertex->a() * 255.0f);
			}

			lVertexIndex++;

			// Write index fixes. (Fix the "old" triangles).
			int j;
			TriangleList::iterator lTriIter;

			for (lTriIter = lVS->mFixTriangles.begin(), j = 0;
				lTriIter != lVS->mFixTriangles.end();
				++lTriIter, ++j)
			{
				Triangle* lTriangle = *lTriIter;

				deb_assert(lTriangle->mVertexIndexHistory.size() > 0);
				deb_assert(lTriangle->mVertexHistory.size() > 0);

				int lTriIndex = ListUtil::FindIndexOf(lTriangleList, lTriangle);
				int lVertex = lTriangle->mVertexIndexHistory.back();
				lTriangle->mVertexIndexHistory.pop_back();
				int lIndex = lTriIndex * 3 + lVertex;
				mVertexSplit[i].mIndexFix[j] = lIndex;

				int lOldValue = 0;
				int lNewValue = 0;

				switch(lVertex)
				{
				case 0:
					lOldValue = ListUtil::FindIndexOf(lVertexList, lTriangle->mV1);
					lTriangle->mV1 = lTriangle->mVertexHistory.back();
					lTriangle->mVertexHistory.pop_back();
					lNewValue = ListUtil::FindIndexOf(lVertexList, lTriangle->mV1);
					break;
				case 1:
					lOldValue = ListUtil::FindIndexOf(lVertexList, lTriangle->mV2);
					lTriangle->mV2 = lTriangle->mVertexHistory.back();
					lTriangle->mVertexHistory.pop_back();
					lNewValue = ListUtil::FindIndexOf(lVertexList, lTriangle->mV2);
					break;
				case 2:
					lOldValue = ListUtil::FindIndexOf(lVertexList, lTriangle->mV3);
					lTriangle->mV3 = lTriangle->mVertexHistory.back();
					lTriangle->mVertexHistory.pop_back();
					lNewValue = ListUtil::FindIndexOf(lVertexList, lTriangle->mV3);
					break;
				};

				mVertexSplit[i].mIndexFixIndex[j * 2 + 0] = lOldValue;
				mVertexSplit[i].mIndexFixIndex[j * 2 + 1] = lNewValue;
			}

			// Write all new triangles.
			for (lTriIter = lVS->mNewTriangles.begin();
				lTriIter != lVS->mNewTriangles.end();
				++lTriIter)
			{
				Triangle* lTriangle = *lTriIter;
				lTriangleList.push_back(lTriangle);
				int lTriIndex = (int)lTriangleList.size() - 1;

				unsigned long lIndex0 = ListUtil::FindIndexOf(lVertexList, lTriangle->mV1);
				unsigned long lIndex1 = ListUtil::FindIndexOf(lVertexList, lTriangle->mV2);
				unsigned long lIndex2 = ListUtil::FindIndexOf(lVertexList, lTriangle->mV3);

				mCurrentIndices[lTriIndex * 3 + 0] = lIndex0;
				mCurrentIndices[lTriIndex * 3 + 1] = lIndex1;
				mCurrentIndices[lTriIndex * 3 + 2] = lIndex2;
			}
		}
	}

	// Delete all memory allocated.
	ListUtil::DeleteAll(lTriangleList);
	ListUtil::DeleteAll(lVertexList);
	ListUtil::DeleteAll(lVertexSplitList);

	if (lClearNormalData == true)
	{
		pGeometry.ClearVertexNormalData();
	}
}

void ProgressiveTriangleGeometry::SetDetailLevel(float pLevelOfDetail)
{
	if (mNumVertexSplits == 0)
	{
		return;
	}

	if (pLevelOfDetail < 0.0f)
	{
		pLevelOfDetail = 0.0f;
	}
	if (pLevelOfDetail > 1.0f)
	{
		pLevelOfDetail = 1.0f;
	}

	unsigned int lTargetVSplit = (unsigned int)(pLevelOfDetail * (float)mNumVertexSplits);
	float lFrac = pLevelOfDetail * (float)mNumVertexSplits - (float)lTargetVSplit;

	if (lTargetVSplit >= mNumVertexSplits)
	{
		lTargetVSplit = mNumVertexSplits - 1;
		lFrac = 1.0f;
	}

	if (lTargetVSplit >= mCurrentVSplit)
	{
		float lF = lFrac;
		lFrac = 1.0f;

		// Expand some edges.
		for (int i = mCurrentVSplit; i <= (int)lTargetVSplit; i++)
		{
			if (i == (int)lTargetVSplit)
				lFrac = lF;

			// Setup the new vertices.
			float lNewScalars0[V_NUMSCALARS];
			float lNewScalars1[V_NUMSCALARS];

			CopyAddScalars(lNewScalars0,
						   mVertexSplit[i].mPivotScalars,
						   mVertexSplit[i].mDeltaScalars,
						   -lFrac);

			CopyAddScalars(lNewScalars1,
						   mVertexSplit[i].mPivotScalars,
						   mVertexSplit[i].mDeltaScalars,
						   lFrac);

			int lIndex0 = mVertexSplit[i].mVertexToSplitIndex * 3;
			int lIndex1 = mVertexSplit[i].mNumOldVertices * 3; // The newly created vertex.

			CopyScalars(&mCurrentVertexData[lIndex0], &lNewScalars0[VX], 3);
			CopyScalars(&mCurrentVertexData[lIndex1], &lNewScalars1[VX], 3);

			float lScale0 = 1.0f / sqrt(lNewScalars0[VNX] * lNewScalars0[VNX] + 
										  lNewScalars0[VNY] * lNewScalars0[VNY] + 
										  lNewScalars0[VNZ] * lNewScalars0[VNZ]);
			float lScale1 = 1.0f / sqrt(lNewScalars1[VNX] * lNewScalars1[VNX] + 
										  lNewScalars1[VNY] * lNewScalars1[VNY] + 
										  lNewScalars1[VNZ] * lNewScalars1[VNZ]);
			CopyScalars(&mCurrentNormalData[lIndex0], &lNewScalars0[VNX], 3, lScale0);
			CopyScalars(&mCurrentNormalData[lIndex1], &lNewScalars1[VNX], 3, lScale1);

			if (mCurrentUVData != 0)
			{
				int lUVIndex0 = mVertexSplit[i].mVertexToSplitIndex * 2;
				int lUVIndex1 = mVertexSplit[i].mNumOldVertices * 2;
				CopyScalars(&mCurrentUVData[lUVIndex0], &lNewScalars0[VU], 2);
				CopyScalars(&mCurrentUVData[lUVIndex1], &lNewScalars1[VU], 2);
			}

			if (mCurrentColorData != 0)
			{
				int lColorIndex0 = mVertexSplit[i].mVertexToSplitIndex * 3;
				int lColorIndex1 = mVertexSplit[i].mNumOldVertices * 3;
				CopyScalars(&mCurrentColorData[lColorIndex0], &lNewScalars0[VR], 3);
				CopyScalars(&mCurrentColorData[lColorIndex1], &lNewScalars1[VR], 3);
				CopyScalarsUC(&mCurrentColorData8[lColorIndex0], &lNewScalars0[VR], 3);
				CopyScalarsUC(&mCurrentColorData8[lColorIndex1], &lNewScalars1[VR], 3);
			}

			// New triangles.
			mCurrentTriangleCount = mVertexSplit[i].mNumOldTriangles +
									mVertexSplit[i].mNumNewTriangles;

			mCurrentVertexCount = mVertexSplit[i].mNumOldVertices + 1;

			// Fix old triangles that should share the new vertex.
			for (int j = 0; j < mVertexSplit[i].mNumIndexFixes; j++)
			{
				mCurrentIndices[mVertexSplit[i].mIndexFix[j]] = mVertexSplit[i].mIndexFixIndex[j * 2 + 1];
			}
		}

		mCurrentVSplit = lTargetVSplit;
	}
	else if(lTargetVSplit < mCurrentVSplit)
	{
		float lF = lFrac;
		lFrac = 1.0f;

		// Collapse some edges.
		for (int i = mCurrentVSplit - 1; i >= (int)lTargetVSplit; i--)
		{
			if (i == (int)lTargetVSplit)
				lFrac = lF;

			// Remove triangles.
			mCurrentTriangleCount = mVertexSplit[i].mNumOldTriangles +
									mVertexSplit[i].mNumNewTriangles;

			mCurrentVertexCount = mVertexSplit[i].mNumOldVertices + 1;

			// Setup the new vertex.
			float lNewScalars0[V_NUMSCALARS];
			float lNewScalars1[V_NUMSCALARS];

			CopyAddScalars(lNewScalars0,
						   mVertexSplit[i].mPivotScalars,
						   mVertexSplit[i].mDeltaScalars,
						   -lFrac);

			CopyAddScalars(lNewScalars1,
						   mVertexSplit[i].mPivotScalars,
						   mVertexSplit[i].mDeltaScalars,
						   lFrac);

			int lIndex0 = mVertexSplit[i].mVertexToSplitIndex * 3;
			int lIndex1 = mVertexSplit[i].mNumOldVertices * 3; // The newly created vertex.

			CopyScalars(&mCurrentVertexData[lIndex0], &lNewScalars0[VX], 3);
			CopyScalars(&mCurrentVertexData[lIndex1], &lNewScalars1[VX], 3);

			float lScale0 = 1.0f / sqrt(lNewScalars0[VNX] * lNewScalars0[VNX] + 
										  lNewScalars0[VNY] * lNewScalars0[VNY] + 
										  lNewScalars0[VNZ] * lNewScalars0[VNZ]);
			float lScale1 = 1.0f / sqrt(lNewScalars1[VNX] * lNewScalars1[VNX] + 
										  lNewScalars1[VNY] * lNewScalars1[VNY] + 
										  lNewScalars1[VNZ] * lNewScalars1[VNZ]);
			CopyScalars(&mCurrentNormalData[lIndex0], &lNewScalars0[VNX], 3, lScale0);
			CopyScalars(&mCurrentNormalData[lIndex1], &lNewScalars1[VNX], 3, lScale1);

			if (mCurrentUVData != 0)
			{
				int lUVIndex0 = mVertexSplit[i].mVertexToSplitIndex * 2;
				int lUVIndex1 = mVertexSplit[i].mNumOldVertices * 2;
				CopyScalars(&mCurrentUVData[lUVIndex0], &lNewScalars0[VU], 2);
				CopyScalars(&mCurrentUVData[lUVIndex1], &lNewScalars1[VU], 2);
			}

			if (mCurrentColorData != 0)
			{
				int lColorIndex0 = mVertexSplit[i].mVertexToSplitIndex * 3;
				int lColorIndex1 = mVertexSplit[i].mNumOldVertices * 3;
				CopyScalars(&mCurrentColorData[lColorIndex0], &lNewScalars0[VR], 3);
				CopyScalars(&mCurrentColorData[lColorIndex1], &lNewScalars1[VR], 3);
				CopyScalarsUC(&mCurrentColorData8[lColorIndex0], &lNewScalars0[VR], 3);
				CopyScalarsUC(&mCurrentColorData8[lColorIndex1], &lNewScalars1[VR], 3);
			}

			for (int j = 0; j < mVertexSplit[i + 1].mNumIndexFixes; j++)
			{
				mCurrentIndices[mVertexSplit[i + 1].mIndexFix[j]] = 
					mVertexSplit[i + 1].mIndexFixIndex[j * 2 + 0];
			}
		}

		mCurrentVSplit = lTargetVSplit;
	}

	TBC::GeometryBase::SetVertexDataChanged(true);
	TBC::GeometryBase::SetColorDataChanged(true);
	TBC::GeometryBase::SetUVDataChanged(true);
	TBC::GeometryBase::SetIndexDataChanged(true);
}

void ProgressiveTriangleGeometry::GetCurrentState(TriangleBasedGeometry& pGeometry)
{
	pGeometry.Set(mCurrentVertexData,
	              mCurrentNormalData,
	              mCurrentUVData,
	              mCurrentColorData8,
	              TriangleBasedGeometry::COLOR_RGBA,
	              mCurrentIndices,
	              mCurrentVertexCount,
	              mCurrentTriangleCount * 3,
	              TBC::GeometryBase::TRIANGLES,
	              TBC::GeometryBase::GEOM_DYNAMIC);
}



}
