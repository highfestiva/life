
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/GeometryBase.h"
#include "../../Lepra/Include/LepraAssert.h"
#include "../../Lepra/Include/ResourceTracker.h"
#include "../../Lepra/Include/Vector2D.h"
#include "../Include/Bones.h"



namespace Tbc
{



GeometryBase::Edge::Edge()
{
	//mTriangle = 0;
	mTriangleCount = 0;
	//mTriangleElementCount = 0;

	mVertex[0] = INVALID_INDEX;
	mVertex[1] = INVALID_INDEX;

	mReserved = 0;
}

GeometryBase::Edge::Edge(const Edge& pEdge)
{
	//mTriangle = 0;
	mTriangleCount = 0;
	//mTriangleElementCount = 0;

	mVertex[0] = INVALID_INDEX;
	mVertex[1] = INVALID_INDEX;

	mReserved = 0;

	Copy(&pEdge);
}

GeometryBase::Edge::Edge(const Edge* pEdge)
{
	//mTriangle = 0;
	mTriangleCount = 0;
	//mTriangleElementCount = 0;

	mVertex[0] = INVALID_INDEX;
	mVertex[1] = INVALID_INDEX;

	mReserved = 0;

	Copy(pEdge);
}

GeometryBase::Edge::~Edge()
{
	ClearAll();
}

void GeometryBase::Edge::ClearAll()
{
	/*if (mTriangle != 0)
	{
		delete[] mTriangle;
		mTriangle = 0;
	}*/

	mTriangleCount = 0;
	//mTriangleElementCount = 0;

	mVertex[0] = INVALID_INDEX;
	mVertex[1] = INVALID_INDEX;

	mReserved = 0;
}

void GeometryBase::Edge::Copy(const Edge* pEdge)
{
	ClearAll();
	mVertex[0] = pEdge->mVertex[0];
	mVertex[1] = pEdge->mVertex[1];

	mTriangleCount = pEdge->mTriangleCount;
	//mTriangleElementCount = pEdge->mTriangleCount;
	//mTriangle = new int[mTriangleCount];
	::memcpy(mTriangle, pEdge->mTriangle, mTriangleCount * sizeof(int));
}

void GeometryBase::Edge::AddTriangle(int pTriangleIndex)
{
	if (HaveTriangle(pTriangleIndex) || mTriangleCount >= 2)
	{
		return;
	}

	/*if ((mTriangleCount + 1) > mTriangleElementCount)
	{
		mTriangleElementCount += (mTriangleElementCount >> 2) + 1;
		int* lTemp = new int[mTriangleElementCount];
		::memcpy(lTemp, mTriangle, mTriangleCount * sizeof(int));
		
		if (mTriangle != 0)
		{
			delete[] mTriangle;
		}
		mTriangle = lTemp;
	}*/
	mTriangle[mTriangleCount] = pTriangleIndex;
	mTriangleCount++;
}

void GeometryBase::Edge::RemoveTriangle(int pTriangleIndex)
{
	// Find the triangle.
	int i;
	int lTriangleToRemove = -1;
	for (i = 0; i < mTriangleCount; i++)
	{
		if (mTriangle[i] == pTriangleIndex)
		{
			lTriangleToRemove = i;
			break;
		}
	}

	if (lTriangleToRemove != -1)
	{
		for (i = lTriangleToRemove; i < (mTriangleCount - 1); i++)
		{
			mTriangle[i] = mTriangle[i + 1];
		}
		--mTriangleCount;
	}
}

bool GeometryBase::Edge::HaveTriangle(int pTriangleIndex)
{
	for (int i = 0; i < mTriangleCount; i++)
	{
		if (mTriangle[i] == pTriangleIndex)
		{
			return true;
		}
	}

	return false;
}

bool GeometryBase::Edge::HaveTriangle(int pTriangleIndex, int& pTrianglePos)
{
	pTrianglePos = -1;

	for (int i = 0; i < mTriangleCount; i++)
	{
		if (mTriangle[i] == pTriangleIndex)
		{
			pTrianglePos = i;
			return true;
		}
	}

	return false;
}

bool GeometryBase::Edge::IsSameEdge(int pVertexIndex1, int pVertexIndex2)
{
	return (mVertex[0] == pVertexIndex1 && mVertex[1] == pVertexIndex2);
}

bool GeometryBase::Edge::IsSameEdge(int pVertexIndex1, 
				    int pVertexIndex2,
				    int* pTriangles, 
				    int pTriangleCount)
{
	if (pTriangleCount != mTriangleCount)
	{
		return false;
	}

	bool lVerticesMatch = false;

	if (mVertex[0] == pVertexIndex1)
	{
		lVerticesMatch = (mVertex[1] == pVertexIndex2);
	}
	else if(mVertex[0] == pVertexIndex2)
	{
		lVerticesMatch = (mVertex[1] == pVertexIndex1);
	}

	if (mTriangleCount == 0 ||
		lVerticesMatch == false)
	{
		return lVerticesMatch;
	}

	// Vertices match. Now we have to test all triangles too.
	// Start by finding the maximum triangle index.
	int lThisMaxTriangleIndex = -1;
	int lTestMaxTriangleIndex = -1;
	int i;

	for (i = 0; i < mTriangleCount; i++)
	{
		if (mTriangle[i] > lThisMaxTriangleIndex)
		{
			lThisMaxTriangleIndex = mTriangle[i];
		}
		if (pTriangles[i] > lTestMaxTriangleIndex)
		{
			lTestMaxTriangleIndex = pTriangles[i];
		}
	}

	if (lTestMaxTriangleIndex != lThisMaxTriangleIndex)
	{
		return false;
	}
	if (mTriangleCount == 1 || lTestMaxTriangleIndex == 0)
	{
		return true;
	}

	// The order of triangles in the arrays mustn't matter,
	// that's why we have to setup the follwing array.

	bool* lTriangleCheck = new bool[lThisMaxTriangleIndex];
	for (i = 0; i < mTriangleCount; i++)
	{
		lTriangleCheck[i] = false;
	}

	for (i = 0; i < mTriangleCount; i++)
	{
		int lTriangle0 = mTriangle[i];

		bool lMatch = false;

		for (int j = 0; j < mTriangleCount; j++)
		{
			int lTriangle1 = pTriangles[j];

			if (lTriangleCheck[lTriangle1] == false && lTriangle1 == lTriangle0)
			{
				lTriangleCheck[lTriangle1] = true;
				lMatch = true;
			}
		}

		if (lMatch == false)
		{
			// There was no match for this triangle. The edges aren't the same.
			delete[] lTriangleCheck;
			return false;
		}
	}

	delete[] lTriangleCheck;
	return true;
}


GeometryBase::GeometryBase() :
	mFlags(mDefaultFlags),
	mPrimitiveType(TRIANGLES),
	mBoundingRadius(0),
	mScale(1),
	mSurfaceNormalData(0),
	mSurfaceNormalCount(0),
	mVertexNormalData(0),
	mUVCountPerVertex(2),
	mEdgeData(0),
	mEdgeCount(0),
	mTangentData(0),
	mBitangentData(0),
	mTangentsUVSet(0),
	mRendererData(0),
	mParentCell(0),
	mLastFrameVisible(0),
	mUVAnimator(0),
	mPreRenderCallback(0),
	mPostRenderCallback(0),
	mExtraData(0),
	mBigOrientationThreshold(mDefaultBigOrientationThreshold)
{
	LEPRA_ACQUIRE_RESOURCE(GeometryBase);
}

GeometryBase::~GeometryBase()
{
	ListenerList::iterator lIter;
	for(lIter = mListenerList.begin(); lIter != mListenerList.end(); )
	{
		Listener* lListener = *lIter;
		++lIter;
		lListener->DeletingGeometry(this);
	}

	ClearSurfaceNormalData();
	ClearVertexNormalData();
	ClearTangentAndBitangentData();
	ClearEdgeData();

	LEPRA_RELEASE_RESOURCE(GeometryBase);
}

void GeometryBase::AddListener(Listener* pListener)
{
	mListenerList.push_back(pListener);
}

void GeometryBase::RemoveListener(Listener* pListener)
{
	mListenerList.remove(pListener);
}

const GeometryBase::PreRenderCallback& GeometryBase::GetPreRenderCallback() const
{
	return mPreRenderCallback;
}

void GeometryBase::SetPreRenderCallback(const PreRenderCallback& pCallback)
{
	mPreRenderCallback = pCallback;
}

const GeometryBase::PostRenderCallback& GeometryBase::GetPostRenderCallback() const
{
	return mPostRenderCallback;
}

void GeometryBase::SetPostRenderCallback(const PostRenderCallback& pCallback)
{
	mPostRenderCallback = pCallback;
}

bool GeometryBase::IsGeometryReference()
{
	return false;
}

void GeometryBase::SetExcludeCulling()
{
	SetFlag(EXCLUDE_CULLING);
}

bool GeometryBase::IsExcludeCulling() const
{
	return CheckFlag(EXCLUDE_CULLING);
}

float GeometryBase::GetScale() const
{
	return mScale;
}

void GeometryBase::SetScale(float pScale)
{
	mScale = pScale;
	if (mScale != 1)
	{
		LEPRA_DEBUG_CODE(mLog.Debugf(_T("Setting scale %f on mesh %s."), mScale, mName.c_str()));
	}
}

void GeometryBase::SetPrimitiveType(PrimitiveType pType)
{
	mPrimitiveType = pType;
}

GeometryBase::PrimitiveType GeometryBase::GetPrimitiveType() const
{
	return mPrimitiveType;
}

unsigned GeometryBase::GetMaxTriangleCount() const
{
	switch (mPrimitiveType)
	{
		case TRIANGLES:
		case LINE_LOOP:
			return GetMaxIndexCount() / 3;
		case TRIANGLE_STRIP:
			return GetMaxIndexCount() - 2;
		case LINES:
			return GetMaxIndexCount() / 2;
		case QUADS:
			return GetMaxIndexCount() / 4;
		default:
			deb_assert(false);
			return (0);
	}
}

unsigned GeometryBase::GetTriangleCount() const
{
	switch (mPrimitiveType)
	{
		case TRIANGLES:
		case LINE_LOOP:
			return GetIndexCount() / 3;
		case TRIANGLE_STRIP:
			return GetIndexCount() - 2;
		case LINES:
			return GetIndexCount() / 2;
		case QUADS:
			return GetIndexCount() / 4;
		default:
			deb_assert(false);
			return (0);
	}
}

void GeometryBase::GetTriangleIndices(int pTriangle, uint32 pIndices[4]) const
{
	const vtx_idx_t* lIndices = GetIndexData();
	switch (mPrimitiveType)
	{
		case TRIANGLES:
		case LINE_LOOP:
		{
			const int lOffset = pTriangle * 3;
			pIndices[0] = lIndices[lOffset + 0];
			pIndices[1] = lIndices[lOffset + 1];
			pIndices[2] = lIndices[lOffset + 2];
		}
		break;
		case TRIANGLE_STRIP:
		{
			if ((pTriangle & 1) == 0)
			{
				pIndices[0] = lIndices[pTriangle + 0];
				pIndices[1] = lIndices[pTriangle + 1];
				pIndices[2] = lIndices[pTriangle + 2];
			}
			else
			{
				// Flipped.
				pIndices[0] = lIndices[pTriangle + 0];
				pIndices[1] = lIndices[pTriangle + 2];
				pIndices[2] = lIndices[pTriangle + 1];
			}
		}
		break;
		case LINES:
		{
			const int lOffset = pTriangle * 2;
			pIndices[0] = lIndices[lOffset + 0];
			pIndices[1] = lIndices[lOffset + 1];
		}
		break;
		case QUADS:
		{
			const int lOffset = pTriangle * 3;
			pIndices[0] = lIndices[lOffset + 0];
			pIndices[1] = lIndices[lOffset + 1];
			pIndices[2] = lIndices[lOffset + 2];
			pIndices[3] = lIndices[lOffset + 3];
		}
		break;
		default:
		{
			deb_assert(false);
			// These values should hopefully cause a crash.
			pIndices[0] = (uint32)-100000;
			pIndices[1] = (uint32)-100000;
			pIndices[2] = (uint32)-100000;
		}
		break;
	}
}

void GeometryBase::SetUVCountPerVertex(int pUVCountPerVertex)
{
	mUVCountPerVertex = pUVCountPerVertex;
}

int GeometryBase::GetUVCountPerVertex() const
{
	return mUVCountPerVertex;
}

unsigned GeometryBase::GetEdgeCount() const
{
	return mEdgeCount;
}

void GeometryBase::SetTangentsUVSet(unsigned pUVSet)
{
	if (pUVSet < GetUVSetCount())
	{
		mTangentsUVSet = pUVSet;
	}
}

const GeometryBase::BasicMaterialSettings& GeometryBase::GetBasicMaterialSettings() const
{
	return (mMaterialSettings);
}

GeometryBase::BasicMaterialSettings& GeometryBase::GetBasicMaterialSettings()
{
	return mMaterialSettings;
}

void GeometryBase::SetBasicMaterialSettings(const BasicMaterialSettings& pMatSettings)
{
	mMaterialSettings = pMatSettings;
}

void GeometryBase::SetVertexDataChanged(bool pChanged)
{
	SetFlag(VERTEX_DATA_CHANGED, pChanged);

	if (pChanged == true)
	{
		ClearFlag(SURFACE_NORMALS_VALID);
		ClearFlag(VERTEX_NORMALS_VALID);
		ClearFlag(TANGENTS_VALID);
		ClearFlag(BOUNDING_RADIUS_VALID);
	}
}

void GeometryBase::SetUVDataChanged(bool pChanged)
{
	SetFlag(UV_DATA_CHANGED, pChanged);

	if (pChanged == true)
	{
		ClearFlag(TANGENTS_VALID);
	}
}

void GeometryBase::SetColorDataChanged(bool pChanged)
{
	SetFlag(COLOR_DATA_CHANGED, pChanged);
}

void GeometryBase::SetIndexDataChanged(bool pChanged)
{
	SetFlag(INDEX_DATA_CHANGED, pChanged);

	if (pChanged == true)
	{
		ClearFlag(SURFACE_NORMALS_VALID);
		ClearFlag(VERTEX_NORMALS_VALID);
		ClearFlag(TANGENTS_VALID);
	}
}

void GeometryBase::SetRendererData(void* pRendererData)
{
	deb_assert(!mRendererData || !pRendererData || mRendererData == pRendererData);
	mRendererData = pRendererData;
}

void* GeometryBase::GetRendererData() const
{
	return mRendererData;
}

size_t GeometryBase::GetExtraData() const
{
	return (mExtraData);
}

void GeometryBase::SetExtraData(size_t pExtraData)
{
	mExtraData = pExtraData;
}

float GeometryBase::GetBoundingRadius() const
{
	return mBoundingRadius;
}

float GeometryBase::GetBoundingRadius()
{
	if (CheckFlag(BOUNDING_RADIUS_ALWAYS_VALID) == false && 
	    CheckFlag(BOUNDING_RADIUS_VALID) == false)
	{
		CalculateBoundingRadius();
	}

	return mBoundingRadius * mScale;
}

void GeometryBase::SetBoundingRadius(float pBoundingRadius)
{
	mBoundingRadius = pBoundingRadius / mScale;
	SetFlag(BOUNDING_RADIUS_ALWAYS_VALID, mBoundingRadius > 0);
	SetFlag(BOUNDING_RADIUS_VALID, mBoundingRadius > 0);
}

float* GeometryBase::GetNormalData() const
{
	return mVertexNormalData;
}

float* GeometryBase::GetSurfaceNormalData() const
{
	return mSurfaceNormalData;
}

GeometryBase::Edge* GeometryBase::GetEdgeData() const
{
	return mEdgeData;
}

float* GeometryBase::GetTangentData() const
{
	return mTangentData;
}

float* GeometryBase::GetBitangentData() const
{
	return mBitangentData;
}

void GeometryBase::ClearAll()
{
	ClearVertexNormalData();
	ClearSurfaceNormalData();
	ClearEdgeData();
	ClearTangentAndBitangentData();
	mFlags = 0;
	mSurfaceNormalCount   = 0;
	SetBoundingRadius(0.0f);
}

void GeometryBase::ClearVertexNormalData()
{
	delete[] mVertexNormalData;
	mVertexNormalData = 0;
	ClearFlag(VERTEX_NORMALS_VALID);
}

void GeometryBase::ClearSurfaceNormalData()
{
	delete[] mSurfaceNormalData;
	mSurfaceNormalData = 0;
	ClearFlag(SURFACE_NORMALS_VALID);
	mSurfaceNormalCount = 0;
}


void GeometryBase::ClearEdgeData()
{
	delete[] mEdgeData;
	mEdgeData = 0;
	mEdgeCount = 0;
}

void GeometryBase::ClearTangentAndBitangentData()
{
	delete[] mTangentData;
	mTangentData = 0;
	delete[] mBitangentData;
	mBitangentData = 0;

	ClearFlag(TANGENTS_VALID);
}

void GeometryBase::AllocVertexNormalData()
{
	if (mVertexNormalData == 0)
	{
		mVertexNormalData = new float[GetMaxVertexCount() * 3];
	}
}

void GeometryBase::AllocSurfaceNormalData()
{
	if (mSurfaceNormalData == 0)
	{
		unsigned lTriangleCount = GetMaxTriangleCount();
		mSurfaceNormalData = new float[lTriangleCount * 3];
		mSurfaceNormalCount = lTriangleCount;
	}
}

void GeometryBase::AllocTangentAndBitangentData()
{
	int lVertexCount = GetMaxVertexCount();
	if (mTangentData == 0)
	{
		mTangentData = new float[lVertexCount * 3];
	}
	if (mBitangentData == 0)
	{
		mBitangentData = new float[lVertexCount * 3];
	}
}

void GeometryBase::SetLastFrameVisible(unsigned pLastFrameVisible)
{
	mLastFrameVisible = pLastFrameVisible;
}

unsigned GeometryBase::GetLastFrameVisible() const
{
	return mLastFrameVisible;
}

void GeometryBase::SetTransformation(const xform& pTransformation)
{
	mTransformation = pTransformation;
	SetTransformationChanged(true);

	quat q(mBigOrientation.mData);
	q.Sub(GetTransformation().mOrientation.mData);	// Must let overrides go to work, so we can store full update.
	if (q.GetNorm() > mBigOrientationThreshold)
	{
		mBigOrientation = q;
		SetBigOrientationChanged(true);
	}
}

const xform& GeometryBase::GetBaseTransformation() const
{
	return mTransformation;
}

const xform& GeometryBase::GetTransformation()
{
	return mTransformation;
}

const quat& GeometryBase::GetLastBigOrientation() const
{
	return mBigOrientation;
}

float GeometryBase::GetBigOrientationThreshold() const
{
	return mBigOrientationThreshold;
}

void GeometryBase::SetBigOrientationThreshold(float pBigOrientationThreshold)
{
	mBigOrientationThreshold = pBigOrientationThreshold;
}

void GeometryBase::SetDefaultBigOrientationThreshold(float pBigOrientationThreshold)
{
	mDefaultBigOrientationThreshold = pBigOrientationThreshold;
}

void GeometryBase::SetParentCell(PortalManager::Cell* pCell)
{
	mParentCell = pCell;
}

PortalManager::Cell* GeometryBase::GetParentCell()
{
	return mParentCell;
}

void GeometryBase::SetUVAnimator(BoneAnimator* pUVAnimator)
{
	mUVAnimator = pUVAnimator;
}

BoneAnimator* GeometryBase::GetUVAnimator()
{
	return mUVAnimator;
}

void GeometryBase::Copy(GeometryBase* pGeometry)
{
	ClearAll();

	SetPrimitiveType(pGeometry->GetPrimitiveType());
	SetUVCountPerVertex(pGeometry->GetUVCountPerVertex());
	SetBigOrientationThreshold(pGeometry->GetBigOrientationThreshold());
	mMaterialSettings = pGeometry->GetBasicMaterialSettings();
	mFlags = pGeometry->mFlags;
	mBoundingRadius    = pGeometry->mBoundingRadius;

	mSurfaceNormalCount = pGeometry->mSurfaceNormalCount;
	if (pGeometry->GetSurfaceNormalData() != 0)
	{
		mSurfaceNormalData = new float[mSurfaceNormalCount * 3];
		::memcpy(mSurfaceNormalData, pGeometry->GetSurfaceNormalData(), mSurfaceNormalCount * 3 * sizeof(float));
	}

	if (pGeometry->mVertexNormalData != 0)
	{
		unsigned lVertexCount = pGeometry->GetVertexCount();
		mVertexNormalData = new float[lVertexCount * 3];
		::memcpy(mVertexNormalData, pGeometry->mVertexNormalData, lVertexCount * 3 * sizeof(float));
	}

	mEdgeCount = pGeometry->mEdgeCount;
	if (pGeometry->mEdgeData != 0)
	{
		// Copy edge data.
		mEdgeData = new Edge[mEdgeCount];
		for (unsigned i = 0; i < mEdgeCount; i++)
		{
			mEdgeData[i].Copy(&pGeometry->mEdgeData[i]);
		}
	}

	mTangentsUVSet = pGeometry->mTangentsUVSet;
	if (pGeometry->mTangentData != 0 && pGeometry->mBitangentData)
	{
		unsigned lVertexCount = pGeometry->GetVertexCount();
		mTangentData = new float[lVertexCount * 3];
		mBitangentData = new float[lVertexCount * 3];
		::memcpy(mTangentData, pGeometry->mTangentData, lVertexCount * 3 * sizeof(float));
		::memcpy(mBitangentData, pGeometry->mBitangentData, lVertexCount * 3 * sizeof(float));
	}

	mRendererData = pGeometry->mRendererData;
	mParentCell = pGeometry->mParentCell;
	mLastFrameVisible = pGeometry->mLastFrameVisible;
	mTransformation = pGeometry->mTransformation;
	mUVAnimator = pGeometry->mUVAnimator;
}

bool GeometryBase::IsSolidVolume()
{
	if (CheckFlag(SOLID_VOLUME_VALID) == false)
	{
		if (mEdgeData == 0)
		{
			GenerateEdgeData();
		}

		SetFlag(IS_SOLID_VOLUME);

		for (unsigned i = 0; i < mEdgeCount; i++)
		{
			if (mEdgeData[i].mTriangleCount != 2)
			{
				ClearFlag(IS_SOLID_VOLUME);
				break;
			}
		}

		SetFlag(SOLID_VOLUME_VALID);
	}

	return CheckFlag(IS_SOLID_VOLUME);
}

bool GeometryBase::IsSingleObject()
{
	if (CheckFlag(SINGLE_OBJECT_VALID) == true)
	{
		return CheckFlag(IS_SINGLE_OBJECT);
	}

	unsigned lTriangleCount = GetTriangleCount();
	vtx_idx_t* lIndices   = GetIndexData();

	if (lIndices == 0 || lTriangleCount == 0)
	{
		return false;
	};

	if (mEdgeData == 0)
	{
		GenerateEdgeData();
	}

	// Triangle-to-edge lookup.
	int* lEdgeIndices = new int[lTriangleCount * 3];

	// Need this to fill the lEdgeIndices correctly.
	int* lEdgeCount = new int[lTriangleCount];

	unsigned i;
	for (i = 0; i < lTriangleCount; i++)
	{
		lEdgeCount[i] = 0;
	}

	// Setup triangle-to-edge lookup.
	for (i = 0; i < mEdgeCount; i++)
	{
		Edge* lEdge = &mEdgeData[i];

		for (int j = 0; j < lEdge->mTriangleCount; j++)
		{
			int lT = lEdge->mTriangle[j];
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

	// This is a flood fill. Starting from triangle 0, we set its flag to true,
	// and continue doing the same procedure with its neighbour triangles.
	// If we can fill all triangles (setting all flags to 'true'),
	// they are all connected, which means that this is all one object.
	int* lTriangleStack = new int[lTriangleCount];
	int lStackIndex = 0;
	lTriangleStack[0] = 0;

	int lTrianglesLeft = lTriangleCount;
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
			Edge* lEdge = &mEdgeData[lEdgeIndex];

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

	SetFlag(IS_SINGLE_OBJECT);

	// Check if there is any flag that is still set to false.
	if (lTrianglesLeft > 0)
	{
		ClearFlag(IS_SINGLE_OBJECT);
	}

	delete[] lTriangleChecked;
	delete[] lEdgeIndices;
	delete[] lEdgeCount;
	delete[] lEdgeToCheck;
	delete[] lTriangleStack;

	SetFlag(SINGLE_OBJECT_VALID);
	return CheckFlag(IS_SINGLE_OBJECT);
}

bool GeometryBase::IsConvexVolume()
{
	if (CheckFlag(CONVEX_VOLUME_VALID) == true)
	{
		return CheckFlag(IS_CONVEX_VOLUME);
	}

	unsigned lTriangleCount = GetTriangleCount();
	float* lVertexData = GetVertexData();

	if (lTriangleCount == 0)
	{
		return false;
	};

	if (mEdgeData == 0)
	{
		GenerateEdgeData();
	}

	if (IsSolidVolume() == false)
	{
		ClearFlag(IS_CONVEX_VOLUME);
		SetFlag(CONVEX_VOLUME_VALID);
		return false;
	}

	// Since we got here, IsSolidVolume() returned true. And it will only return
	// true if all edges have exactly 2 triangles connected. Assume convex.
	SetFlag(IS_CONVEX_VOLUME);

	bool lClearSurfaceNormals = (mSurfaceNormalData == 0);
	GenerateSurfaceNormalData();

	const float lEpsilon = 1e-6f;
	for (unsigned i = 0; i < mEdgeCount; i++)
	{
		int lT0 = mEdgeData[i].mTriangle[0];
		int lT1 = mEdgeData[i].mTriangle[1];
		int lT0Index = lT0 * 3;
		//int lT1Index = lT1 * 3;

		vec3 lT0Normal(mSurfaceNormalData[lT0Index + 0],
					    mSurfaceNormalData[lT0Index + 1],
					    mSurfaceNormalData[lT0Index + 2]);

		uint32 lT0TriIndex[4];
		uint32 lT1TriIndex[4];
		GetTriangleIndices(lT0, lT0TriIndex);
		GetTriangleIndices(lT1, lT1TriIndex);

		// Get an edge, no matter which one, but not the current edge.
		// lT1V0 must be the same vertex as one of the edge's, and
		// lT1V1 mustn't be the other.
		int lT1V0 = mEdgeData[i].mVertex[0];
		int lT1V1 = lT1TriIndex[0];

		if (lT1V1 == mEdgeData[i].mVertex[0] ||
		   lT1V1 == mEdgeData[i].mVertex[1])
		{
			lT1V1 = lT1TriIndex[1];
		}
		if (lT1V1 == mEdgeData[i].mVertex[0] ||
		   lT1V1 == mEdgeData[i].mVertex[1])
		{
			lT1V1 = lT1TriIndex[2];
		}

		lT1V0 *= 3;
		lT1V1 *= 3;

		vec3 lT1Edge(lVertexData[lT1V1 + 0] - lVertexData[lT1V0 + 0],
					  lVertexData[lT1V1 + 1] - lVertexData[lT1V0 + 1],
					  lVertexData[lT1V1 + 2] - lVertexData[lT1V0 + 2]);
		lT1Edge.Normalize();

		float lDot = lT1Edge.Dot(lT0Normal);

		if (lDot > lEpsilon)
		{
			// We found a concave angle.
			ClearFlag(IS_CONVEX_VOLUME);
			break;
		}
	}

	if (lClearSurfaceNormals == true)
	{
		GeometryBase::ClearSurfaceNormalData();
	}

	SetFlag(CONVEX_VOLUME_VALID);

	return CheckFlag(IS_CONVEX_VOLUME);
}

bool GeometryBase::IsTwoSided() const
{
	return CheckFlag(IS_TWO_SIDED);
}

void GeometryBase::SetTwoSided(bool pIsTwoSided)
{
	SetFlag(IS_TWO_SIDED, pIsTwoSided);
}

bool GeometryBase::IsRecvNoShadows() const
{
	return CheckFlag(RECV_NO_SHADOWS);
}

void GeometryBase::SetRecvNoShadows(bool pRecvNoShadows)
{
	SetFlag(RECV_NO_SHADOWS, pRecvNoShadows);
}

void GeometryBase::CalculateBoundingRadius()
{
	float* lVertexData = GetVertexData();
	int lVertexCount = GetVertexCount();

	float lMaxDistance = 0.0f;
	for (int i = 0; i < lVertexCount; i++)
	{
		int lIndex = i * 3;
		float lDistance = lVertexData[lIndex + 0] * lVertexData[lIndex + 0] +
				   lVertexData[lIndex + 1] * lVertexData[lIndex + 1] +
				   lVertexData[lIndex + 2] * lVertexData[lIndex + 2];

		if (lDistance > lMaxDistance)
			lMaxDistance = lDistance;
	}

	mBoundingRadius = sqrtf(lMaxDistance);

	SetFlag(BOUNDING_RADIUS_VALID);
}

void GeometryBase::SetSurfaceNormalData(const float* pSurfaceNormalData)
{
	unsigned lTriangleCount = GetTriangleCount();
	AllocSurfaceNormalData();
	::memcpy(mSurfaceNormalData, pSurfaceNormalData, lTriangleCount * 3 * sizeof(float));
	SetFlag(SURFACE_NORMALS_VALID);
}

void GeometryBase::SetVertexNormalData(const float* pVertexNormalData, unsigned pVertexCount)
{
	if (CheckFlag(VERTEX_NORMALS_VALID) == false)
	{
		ClearVertexNormalData();
		AllocVertexNormalData();
	}

	::memcpy(mVertexNormalData, pVertexNormalData, pVertexCount * 3 * sizeof(float));

	SetFlag(VERTEX_NORMALS_VALID);
}

void GeometryBase::SetTangentAndBitangentData(const float* pTangentData, const float* pBitangentData, unsigned pVertexCount)
{
	if (CheckFlag(TANGENTS_VALID) == false)
	{
		ClearTangentAndBitangentData();
		mTangentData   = new float[GetMaxVertexCount() * 3];
		mBitangentData = new float[GetMaxVertexCount() * 3];
	}

	::memcpy(mTangentData, pTangentData, pVertexCount * 3 * sizeof(float));
	::memcpy(mBitangentData, pBitangentData, pVertexCount * 3 * sizeof(float));

	SetFlag(TANGENTS_VALID);
}

void GeometryBase::GenerateVertexNormalData()
{
	if (CheckFlag(VERTEX_NORMALS_VALID) == true || GetIndexData() == 0)
	{
		return;
	}

	unsigned lVertexCount  = GetMaxVertexCount();
	unsigned lTriangleCount = GetTriangleCount();
	if (lVertexCount == 0 || lTriangleCount == 0)
	{
		return;
	}

	AllocVertexNormalData();
	::memset(mVertexNormalData, 0, lVertexCount * 3 * sizeof(float));

	bool lClearSurfaceNormals = (mSurfaceNormalData == 0);
	GenerateSurfaceNormalData();

	unsigned i;
	unsigned lIndex = 0;

	// Now calculate the vertex normals.
	for (i = 0; i < lTriangleCount; i++, lIndex += 3)
	{
		uint32 lT[4];
		GetTriangleIndices(i, lT);
		int lI0 = lT[0] * 3;
		int lI1 = lT[1] * 3;
		int lI2 = lT[2] * 3;

		mVertexNormalData[lI0 + 0] += mSurfaceNormalData[lIndex + 0];
		mVertexNormalData[lI0 + 1] += mSurfaceNormalData[lIndex + 1];
		mVertexNormalData[lI0 + 2] += mSurfaceNormalData[lIndex + 2];

		mVertexNormalData[lI1 + 0] += mSurfaceNormalData[lIndex + 0];
		mVertexNormalData[lI1 + 1] += mSurfaceNormalData[lIndex + 1];
		mVertexNormalData[lI1 + 2] += mSurfaceNormalData[lIndex + 2];

		mVertexNormalData[lI2 + 0] += mSurfaceNormalData[lIndex + 0];
		mVertexNormalData[lI2 + 1] += mSurfaceNormalData[lIndex + 1];
		mVertexNormalData[lI2 + 2] += mSurfaceNormalData[lIndex + 2];
	}

	// Normalize the normals.
	const float lEpsilon = 1e-6f;
	lIndex = 0;
	for (i = 0; i < lVertexCount; i++, lIndex += 3)
	{
		float lDx = mVertexNormalData[lIndex + 0];
		float lDy = mVertexNormalData[lIndex + 1];
		float lDz = mVertexNormalData[lIndex + 2];
		float lLength = (float)sqrt(lDx * lDx + lDy * lDy+ lDz * lDz);

		if (lLength > lEpsilon)
		{
			float lOneOverLength = 1.0f / lLength;
			mVertexNormalData[lIndex + 0] *= lOneOverLength;
			mVertexNormalData[lIndex + 1] *= lOneOverLength;
			mVertexNormalData[lIndex + 2] *= lOneOverLength;
		}
	}

	if (lClearSurfaceNormals == true)
	{
		GeometryBase::ClearSurfaceNormalData();
	}

	SetFlag(VERTEX_NORMALS_VALID);
}

void GeometryBase::GenerateSurfaceNormalData()
{
	if (CheckFlag(SURFACE_NORMALS_VALID) == true)
	{
		return;
	}

	unsigned lTriangleCount = GetTriangleCount();

	if (mSurfaceNormalData != 0 && mSurfaceNormalCount < lTriangleCount)
	{
		ClearSurfaceNormalData();
	}

	AllocSurfaceNormalData();

	float* lVertexData = GetVertexData();

	int lIndex = 0;
	for (unsigned i = 0; i < lTriangleCount; i++, lIndex += 3)
	{
		uint32 lT[4];
		GetTriangleIndices(i, lT);

		int lI0 = lT[0] * 3;
		int lI1 = lT[1] * 3;
		int lI2 = lT[2] * 3;

		vec3 lV0(lVertexData[lI1 + 0] - lVertexData[lI0 + 0],
				      lVertexData[lI1 + 1] - lVertexData[lI0 + 1],
				      lVertexData[lI1 + 2] - lVertexData[lI0 + 2]);
		vec3 lV1(lVertexData[lI2 + 0] - lVertexData[lI0 + 0],
				      lVertexData[lI2 + 1] - lVertexData[lI0 + 1],
				      lVertexData[lI2 + 2] - lVertexData[lI0 + 2]);
		vec3 lC;
		lC.CrossUnit(lV0, lV1);

		mSurfaceNormalData[lIndex + 0] = lC.x;
		mSurfaceNormalData[lIndex + 1] = lC.y;
		mSurfaceNormalData[lIndex + 2] = lC.z;
	}

	SetFlag(SURFACE_NORMALS_VALID);
}

void GeometryBase::GenerateEdgeData()
{
	if (GetVertexCount() == 0 ||
	   GetTriangleCount() == 0)
	{
		return;
	}

	unsigned lVertexCount  = GetVertexCount();
	unsigned lTriangleCount = GetTriangleCount();

	ClearEdgeData();

	// Create buckets to store the edges in to optimize search.
	// It's almost like a hash table, with as many buckets as there
	// are vertex indices.
	std::vector<Edge*> lEdgeBucket(lVertexCount);
	::memset(&lEdgeBucket[0], 0, lVertexCount*sizeof(Edge*));
	std::vector<Edge> lEdgePool(lTriangleCount*3);	// For avoiding repeated allocations.
	size_t lEdgePoolIndex = 0;

	// Create edges.
	for (unsigned i = 0; i < lTriangleCount; i++)
	{
		// Get vertex indices.
		uint32 lV[4];
		GetTriangleIndices(i, lV);

		// Note:
		// Edge0 = (lV[2], lV[0])
		// Edge1 = (lV[0], lV[1])
		// Edge2 = (lV[1], lV[2])

		int lStart = 2;
		int lEnd = 0;
		while (lEnd < 3)
		{
			unsigned lV0 = lV[lStart];
			unsigned lV1 = lV[lEnd];
			if (lV0 > lV1)	// Sorting.
			{
				std::swap(lV0, lV1);
			}

			// Now check if the edge (lV0, lV1) is already in the table.
			if (!lEdgeBucket[lV0])
			{
				// This bucket was empty, create a new edge.
				lEdgeBucket[lV0] = &lEdgePool[lEdgePoolIndex++];
				lEdgeBucket[lV0]->mVertex[0] = lV0;
				lEdgeBucket[lV0]->mVertex[1] = lV1;
				lEdgeBucket[lV0]->AddTriangle(i);

				mEdgeCount++;
			}
			else
			{
				// This bucket contains some edges, let's search for (lV0, lV1).
				bool lFound = false;
				Edge* lPreviousEdge = 0;
				Edge* lCurrentEdge = lEdgeBucket[lV0];
				while (lCurrentEdge && !lFound)
				{
					lPreviousEdge = lCurrentEdge;
					lCurrentEdge = lCurrentEdge->mReserved;
					lFound = lPreviousEdge->IsSameEdge(lV0, lV1);
				}
				if (lFound)
				{
					// We found the edge. Since there can only be two triangles
					// sharing one edge, the second triangle index must be invalid.
					// Set it now.
					lPreviousEdge->AddTriangle(i);
				}
				else
				{
					// Create new and put in linked list.
					Edge& lEdge = lEdgePool[lEdgePoolIndex++];
					lEdge.mVertex[0] = lV0;
					lEdge.mVertex[1] = lV1;
					lEdge.AddTriangle(i);
					lPreviousEdge->mReserved = &lEdge;

					mEdgeCount++;
				}
			}

			lStart = lEnd;
			lEnd++;
		} // End while(lEnd < 3)
	}

	// Create the actual edge array and copy the data.
	// Delete all temporarily created edges.
	mEdgeData = new Edge[mEdgeCount];
	int lEdgeIndex = 0;
	for (unsigned i = 0; i < lVertexCount; i++)
	{
		Edge* lCurrentEdge = lEdgeBucket[i];
		while (lCurrentEdge)
		{
			mEdgeData[lEdgeIndex++].Copy(lCurrentEdge);
			lCurrentEdge = lCurrentEdge->mReserved;
		}
	}

	ClearFlag(SOLID_VOLUME_VALID);
	ClearFlag(SINGLE_OBJECT_VALID);
	ClearFlag(CONVEX_VOLUME_VALID);
}

void GeometryBase::GenerateTangentAndBitangentData()
{
	float* lVertexData = GetVertexData();
	float* lUVData = GetUVData(mTangentsUVSet);

	if (lVertexData == 0 || lUVData == 0)
		return;

	int lVertexCount = GetMaxVertexCount();

	AllocTangentAndBitangentData();
	::memset(mTangentData, 0, sizeof(float) * lVertexCount * 3);
	::memset(mBitangentData, 0, sizeof(float) * lVertexCount * 3);

	int lTriangleCount = GetTriangleCount();
	int i;

	for (i = 0; i < lTriangleCount; i++)
	{
		uint32 lTriIndex[4];
		GetTriangleIndices(i, lTriIndex);

		int lV1  = lTriIndex[0] * 3;
		int lV2  = lTriIndex[1] * 3;
		int lV3  = lTriIndex[2] * 3;
		int lUV1 = lTriIndex[0] * 2;
		int lUV2 = lTriIndex[1] * 2;
		int lUV3 = lTriIndex[2] * 2;

		vec3 lEdge1(lVertexData[lV2 + 0] - lVertexData[lV1 + 0],
					 lVertexData[lV2 + 1] - lVertexData[lV1 + 1],
					 lVertexData[lV2 + 2] - lVertexData[lV1 + 2]);
		vec3 lEdge2(lVertexData[lV3 + 0] - lVertexData[lV1 + 0],
					 lVertexData[lV3 + 1] - lVertexData[lV1 + 1],
					 lVertexData[lV3 + 2] - lVertexData[lV1 + 2]);

		vec2 lEdge1UV(lUVData[lUV2 + 0] - lUVData[lUV1 + 0],
					   lUVData[lUV2 + 1] - lUVData[lUV1 + 1]);
		vec2 lEdge2UV(lUVData[lUV3 + 0] - lUVData[lUV1 + 0],
					   lUVData[lUV3 + 1] - lUVData[lUV1 + 1]);

		float lCP = lEdge1UV.y * lEdge2UV.x - lEdge1UV.x * lEdge2UV.y;
		//float lCP = lEdge1UV.x * lEdge2UV.y - lEdge2UV.x * lEdge1UV.y;

		if (lCP != 0.0f)
		{
			float lCPRecip = 1.0f / lCP;

			vec3 lTangent((lEdge1 * -lEdge2UV.y + lEdge2 * lEdge1UV.y) * lCPRecip);
			vec3 lBitangent((lEdge1 * -lEdge2UV.x + lEdge2 * lEdge1UV.x) * lCPRecip);
			//vec3 lTangent((lEdge2UV.y * lEdge1 - lEdge1UV.y * lEdge2) * lCPRecip);
			//vec3 lBitangent((lEdge1UV.x * lEdge2 - lEdge2UV.x * lEdge1) * lCPRecip);

			lTangent.Normalize();
			lBitangent.Normalize();

			// Add the tangent to all three vertices.
			mTangentData[lV1 + 0] += lTangent.x;
			mTangentData[lV1 + 1] += lTangent.y;
			mTangentData[lV1 + 2] += lTangent.z;
			mTangentData[lV2 + 0] += lTangent.x;
			mTangentData[lV2 + 1] += lTangent.y;
			mTangentData[lV2 + 2] += lTangent.z;
			mTangentData[lV3 + 0] += lTangent.x;
			mTangentData[lV3 + 1] += lTangent.y;
			mTangentData[lV3 + 2] += lTangent.z;

			// Add the bitangent to all three vertices.
			mBitangentData[lV1 + 0] += lBitangent.x;
			mBitangentData[lV1 + 1] += lBitangent.y;
			mBitangentData[lV1 + 2] += lBitangent.z;
			mBitangentData[lV2 + 0] += lBitangent.x;
			mBitangentData[lV2 + 1] += lBitangent.y;
			mBitangentData[lV2 + 2] += lBitangent.z;
			mBitangentData[lV3 + 0] += lBitangent.x;
			mBitangentData[lV3 + 1] += lBitangent.y;
			mBitangentData[lV3 + 2] += lBitangent.z;
		}
	}

	// Normalize all vectors.
	lVertexCount = GetVertexCount();
	int lVIndex = 0;
	for (i = 0; i < lVertexCount; i++, lVIndex += 3)
	{
		float x = mTangentData[lVIndex + 0];
		float y = mTangentData[lVIndex + 1];
		float z = mTangentData[lVIndex + 2];
		float lLength = x*x + y*y + z*z;
		if (lLength != 0)
		{
			float lRSQ = (float)(1.0 / sqrt(lLength));
			mTangentData[lVIndex + 0] *= lRSQ;
			mTangentData[lVIndex + 1] *= lRSQ;
			mTangentData[lVIndex + 2] *= lRSQ;
		}

		x = mBitangentData[lVIndex + 0];
		y = mBitangentData[lVIndex + 1];
		z = mBitangentData[lVIndex + 2];
		lLength = x*x + y*y + z*z;
		if (lLength != 0)
		{
			float lRSQ = (float)(1.0 / sqrt(lLength));
			mBitangentData[lVIndex + 0] *= lRSQ;
			mBitangentData[lVIndex + 1] *= lRSQ;
			mBitangentData[lVIndex + 2] *= lRSQ;
		}
	}
}

bool GeometryBase::VerifyIndexData()
{
	vtx_idx_t* lIndex = GetIndexData();
	unsigned lNumIndex = GetTriangleCount() * 3;
	unsigned lNumVertex = GetVertexCount();

	bool lOk = true;
	for (unsigned i = 0; lOk && i < lNumIndex; i++)
	{
		lOk = ((unsigned int)lIndex[i] < lNumVertex);
	}

	return lOk;
}

const xform& GeometryBase::GetUVTransform() const
{
	if(mUVAnimator != 0)
	{
		return mUVAnimator->GetBones()[0].GetRelativeBoneTransformation(0);
	}
	else
	{
		return gIdentityTransformationF;
	}
}



bool GeometryBase::GetVertexDataChanged() const
{
	return CheckFlag(VERTEX_DATA_CHANGED);
}

bool GeometryBase::GetUVDataChanged() const
{
	return CheckFlag(UV_DATA_CHANGED);
}

bool GeometryBase::GetColorDataChanged() const
{
	return CheckFlag(COLOR_DATA_CHANGED);
}

bool GeometryBase::GetIndexDataChanged() const
{
	return CheckFlag(INDEX_DATA_CHANGED);
}

void GeometryBase::SetVertexNormalsValid()
{
	SetFlag(VERTEX_NORMALS_VALID);
}

void GeometryBase::SetSurfaceNormalsValid()
{
	SetFlag(SURFACE_NORMALS_VALID);
}

GeometryBase::ColorFormat GeometryBase::GetColorFormat() const
{
	return COLOR_RGB;
}

bool GeometryBase::GetTransformationChanged() const
{
	return CheckFlag(TRANSFORMATION_CHANGED);
}

void GeometryBase::SetTransformationChanged(bool pTransformationChanged)
{
	pTransformationChanged? SetFlag(TRANSFORMATION_CHANGED|REF_TRANSFORMATION_CHANGED) : ClearFlag(TRANSFORMATION_CHANGED);
}

bool GeometryBase::GetBigOrientationChanged() const
{
	return CheckFlag(BIG_ORIENTATION_CHANGED);
}

void GeometryBase::SetBigOrientationChanged(bool pOrientationChanged)
{
	SetFlag(BIG_ORIENTATION_CHANGED, pOrientationChanged);
}

void GeometryBase::SetAlwaysVisible(bool pAlwaysVisible)
{
	SetFlag(ALWAYS_VISIBLE, pAlwaysVisible);
}

bool GeometryBase::GetAlwaysVisible() const
{
	return CheckFlag(ALWAYS_VISIBLE);
}

void GeometryBase::SetSolidVolumeCheckValid(bool pValid)
{
	SetFlag(SOLID_VOLUME_VALID, pValid);
}

void GeometryBase::SetSingleObjectCheckValid(bool pValid)
{
	SetFlag(SINGLE_OBJECT_VALID, pValid);
}

void GeometryBase::SetConvexVolumeCheckValid(bool pValid)
{
	SetFlag(CONVEX_VOLUME_VALID, pValid);
}

void GeometryBase::SetFlag(Lepra::uint32 pFlag, bool pValue)
{
	if (pValue) SetFlag(pFlag); else ClearFlag(pFlag);
}

void GeometryBase::SetFlag(Lepra::uint32 pFlag)
{
	mFlags |= pFlag;
}

void GeometryBase::ClearFlag(Lepra::uint32 pFlag)
{
	mFlags &= (~pFlag);
}

bool GeometryBase::CheckFlag(Lepra::uint32 pFlag) const
{
	return (mFlags & pFlag) != 0;
}

uint32 GeometryBase::GetFlags() const
{
	return mFlags;
}

void GeometryBase::SetDefaultFlags(Lepra::uint32 pFlags)
{
	mDefaultFlags = pFlags;
}



GeometryBase::BasicMaterialSettings::BasicMaterialSettings():
	mAmbient(0,0,0),
	mDiffuse(1,0,1),
	mSpecular(0,0,0),
	mShininess(0),
	mAlpha(1),
	mSmooth(true)
{
}

GeometryBase::BasicMaterialSettings::BasicMaterialSettings(const vec3& pAmbient, const vec3& pDiffuse,
	const vec3& pSpecular, float pShininess,
	float pAlpha, bool pSmooth):
	mAmbient(pAmbient),
	mDiffuse(pDiffuse),
	mSpecular(pSpecular),
	mShininess(pShininess),
	mAlpha(pAlpha),
	mSmooth(pSmooth)
{
}

void GeometryBase::BasicMaterialSettings::SetColor(float pRed, float pGreen, float pBlue)
{
	mDiffuse.Set(pRed, pGreen, pBlue);
}

void GeometryBase::BasicMaterialSettings::Set(const vec3& pAmbient, const vec3& pDiffuse,
	const vec3& pSpecular, float pShininess,
	float pAlpha, bool pSmooth)
{
	mAmbient	= pAmbient;
	mDiffuse	= pDiffuse;
	mSpecular	= pSpecular;
	mShininess	= pShininess;
	mAlpha		= pAlpha;
	mSmooth		= pSmooth;
}



Lepra::uint32 GeometryBase::mDefaultFlags = 0;
float GeometryBase::mDefaultBigOrientationThreshold = 1e-3f;
loginstance(UI_GFX, GeometryBase);



}
