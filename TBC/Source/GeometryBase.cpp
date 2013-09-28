
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../Include/GeometryBase.h"
#include "../../Lepra/Include/LepraAssert.h"
#include "../../Lepra/Include/ResourceTracker.h"
#include "../../Lepra/Include/Vector2D.h"
#include "../Include/Bones.h"



namespace TBC
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
	if (mVertex[0] == pVertexIndex1)
	{
		return (mVertex[1] == pVertexIndex2);
	}
	else if(mVertex[0] == pVertexIndex2)
	{
		return (mVertex[1] == pVertexIndex1);
	}
	
	return false;
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
	mFlags(0),
	mBoundingRadius(0),
	mScale(1),
	mXformUpdateFactor(0),
	mSurfaceNormalData(0),
	mSurfaceNormalCount(0),
	mVertexNormalData(0),
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
	mExtraData(0)
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

const GeometryBase::RenderCallback& GeometryBase::GetPreRenderCallback() const
{
	return mPreRenderCallback;
}

void GeometryBase::SetPreRenderCallback(const RenderCallback& pCallback)
{
	mPreRenderCallback = pCallback;
}

const GeometryBase::RenderCallback& GeometryBase::GetPostRenderCallback() const
{
	return mPostRenderCallback;
}

void GeometryBase::SetPostRenderCallback(const RenderCallback& pCallback)
{
	mPostRenderCallback = pCallback;
}

bool GeometryBase::IsGeometryReference()
{
	return false;
}

void GeometryBase::SetIsSimpleObject()
{
	SetFlag(IS_SIMPLE_OBJECT);
}

bool GeometryBase::IsSimpleObject() const
{
	return CheckFlag(IS_SIMPLE_OBJECT);
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

unsigned int GeometryBase::GetMaxTriangleCount() const
{
	switch (GetPrimitiveType())
	{
		case TRIANGLES:
		case LINE_LOOP:
			return GetMaxIndexCount() / 3;
		case TRIANGLE_STRIP:
			return GetMaxIndexCount() - 2;
		case LINES:
			return GetMaxIndexCount() / 2;
		default:
			deb_assert(false);
			return (0);
	}
}

unsigned int GeometryBase::GetTriangleCount() const
{
	switch (GetPrimitiveType())
	{
		case TRIANGLES:
		case LINE_LOOP:
			return GetIndexCount() / 3;
		case TRIANGLE_STRIP:
			return GetIndexCount() - 2;
		case LINES:
			return GetIndexCount() / 2;
		default:
			deb_assert(false);
			return (0);
	}
}

void GeometryBase::GetTriangleIndices(int pTriangle, uint32 pIndices[3])
{
	const vtx_idx_t* lIndices = GetIndexData();
	switch (GetPrimitiveType())
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

unsigned int GeometryBase::GetEdgeCount() const
{
	return mEdgeCount;
}

void GeometryBase::SetTangentsUVSet(unsigned int pUVSet)
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
	deb_assert(!mRendererData || !pRendererData);
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

	mBoundingRadius      = 0.0f;
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
		unsigned int lTriangleCount = GetMaxTriangleCount();
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

void GeometryBase::SetLastFrameVisible(unsigned int pLastFrameVisible)
{
	mLastFrameVisible = pLastFrameVisible;
}

unsigned int GeometryBase::GetLastFrameVisible() const
{
	return mLastFrameVisible;
}

void GeometryBase::SetTransformation(const TransformationF& pTransformation)
{
	QuaternionF lOrientation = mTransformation.GetOrientation();
	lOrientation.Sub(pTransformation.GetOrientation());
	mXformUpdateFactor += lOrientation.GetNorm() * 25000 + 
		mTransformation.GetPosition().GetDistanceSquared(pTransformation.GetPosition()) * 2500;
	mXformUpdateFactor *= 2;
	if (mXformUpdateFactor >= 1)
	{
		SetFlag(TRANSFORMATION_CHANGED);
		mXformUpdateFactor = 0;
	}
	mTransformation = pTransformation;
}

const TransformationF& GeometryBase::GetBaseTransformation() const
{
	return mTransformation;
}

const TransformationF& GeometryBase::GetTransformation()
{
	return mTransformation;
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
		unsigned int lVertexCount = pGeometry->GetVertexCount();
		mVertexNormalData = new float[lVertexCount * 3];
		::memcpy(mVertexNormalData, pGeometry->mVertexNormalData, lVertexCount * 3 * sizeof(float));
	}

	mEdgeCount = pGeometry->mEdgeCount;
	if (pGeometry->mEdgeData != 0)
	{
		// Copy edge data.
		mEdgeData = new Edge[mEdgeCount];
		for (unsigned int i = 0; i < mEdgeCount; i++)
		{
			mEdgeData[i].Copy(&pGeometry->mEdgeData[i]);
		}
	}

	mTangentsUVSet = pGeometry->mTangentsUVSet;
	if (pGeometry->mTangentData != 0 && pGeometry->mBitangentData)
	{
		unsigned int lVertexCount = pGeometry->GetVertexCount();
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

		for (unsigned int i = 0; i < mEdgeCount; i++)
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

	unsigned int lTriangleCount = GetTriangleCount();
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

	unsigned int i;
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

	unsigned int lTriangleCount = GetTriangleCount();
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
	for (unsigned int i = 0; i < mEdgeCount; i++)
	{
		int lT0 = mEdgeData[i].mTriangle[0];
		int lT1 = mEdgeData[i].mTriangle[1];
		int lT0Index = lT0 * 3;
		//int lT1Index = lT1 * 3;

		Vector3DF lT0Normal(mSurfaceNormalData[lT0Index + 0],
					    mSurfaceNormalData[lT0Index + 1],
					    mSurfaceNormalData[lT0Index + 2]);

		uint32 lT0TriIndex[3];
		uint32 lT1TriIndex[3];
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

		Vector3DF lT1Edge(lVertexData[lT1V1 + 0] - lVertexData[lT1V0 + 0],
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

void GeometryBase::SetSurfaceNormalData(float* pSurfaceNormalData)
{
	unsigned int lTriangleCount = GetTriangleCount();
	AllocSurfaceNormalData();
	::memcpy(mSurfaceNormalData, pSurfaceNormalData, lTriangleCount * 3 * sizeof(float));
	SetFlag(SURFACE_NORMALS_VALID);
}

void GeometryBase::SetVertexNormalData(float* pVertexNormalData, unsigned int pVertexCount)
{
	if (CheckFlag(VERTEX_NORMALS_VALID) == false)
	{
		ClearVertexNormalData();
		AllocVertexNormalData();
	}

	::memcpy(mVertexNormalData, pVertexNormalData, pVertexCount * 3 * sizeof(float));

	SetFlag(VERTEX_NORMALS_VALID);
}

void GeometryBase::SetTangentAndBitangentData(float* pTangentData, float* pBitangentData, unsigned int pVertexCount)
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

	unsigned int lVertexCount  = GetMaxVertexCount();
	unsigned int lTriangleCount = GetTriangleCount();
	if (lVertexCount == 0 || lTriangleCount == 0)
	{
		return;
	}

	AllocVertexNormalData();
	::memset(mVertexNormalData, 0, lVertexCount * 3 * sizeof(float));

	bool lClearSurfaceNormals = (mSurfaceNormalData == 0);
	GenerateSurfaceNormalData();

	unsigned int i;
	unsigned int lIndex = 0;

	// Now calculate the vertex normals.
	for (i = 0; i < lTriangleCount; i++, lIndex += 3)
	{
		uint32 lT[3];
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

	unsigned int lTriangleCount = GetTriangleCount();

	if (mSurfaceNormalData != 0 && mSurfaceNormalCount < lTriangleCount)
	{
		ClearSurfaceNormalData();
	}

	AllocSurfaceNormalData();

	float* lVertexData = GetVertexData();

	int lIndex = 0;
	for (unsigned int i = 0; i < lTriangleCount; i++, lIndex += 3)
	{
		uint32 lT[3];
		GetTriangleIndices(i, lT);

		int lI0 = lT[0] * 3;
		int lI1 = lT[1] * 3;
		int lI2 = lT[2] * 3;

		Vector3DF lV0(lVertexData[lI1 + 0] - lVertexData[lI0 + 0],
				      lVertexData[lI1 + 1] - lVertexData[lI0 + 1],
				      lVertexData[lI1 + 2] - lVertexData[lI0 + 2]);
		Vector3DF lV1(lVertexData[lI2 + 0] - lVertexData[lI0 + 0],
				      lVertexData[lI2 + 1] - lVertexData[lI0 + 1],
				      lVertexData[lI2 + 2] - lVertexData[lI0 + 2]);
		Vector3DF lC;
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

	unsigned int lVertexCount  = GetVertexCount();
	unsigned int lTriangleCount = GetTriangleCount();

	ClearEdgeData();

	// Create buckets to store the edges in to optimize search.
	// It's almost like a hash table, with as many buckets as there
	// are vertex indices.
	Edge** lEdgeBucket = new Edge*[lVertexCount];

	unsigned int i;
	for (i = 0; i < lVertexCount; i++)
	{
		lEdgeBucket[i] = 0;
	}

	// Create edges.
	int lIndex = 0;
	for (i = 0; i < lTriangleCount; i++, lIndex += 3)
	{
		// Get vertex indices.
		uint32 lV[3];
		GetTriangleIndices(i, lV);

		// Sort them.
		if (lV[0] > lV[1])
		{
			uint32 lT = lV[0];
			lV[0] = lV[1];
			lV[1] = lT;
		}
		if (lV[1] > lV[2])
		{
			uint32 lT = lV[1];
			lV[1] = lV[2];
			lV[2] = lT;
		}
		if (lV[0] > lV[1])
		{
			uint32 lT = lV[0];
			lV[0] = lV[1];
			lV[1] = lT;
		}

		// Note:
		// Edge0 = (lV[0], lV[1])
		// Edge1 = (lV[1], lV[2])
		// Edge2 = (lV[0], lV[2])

		int lStart = 2;
		int lEnd = 0;

		while (lEnd < 3)
		{
			unsigned int lV0 = lV[lStart];
			unsigned int lV1 = lV[lEnd];

			if (lStart > lEnd)
			{
				lV0 = lV[lEnd];
				lV1 = lV[lStart];
			}

			// Now check if the edge (lV0, lV1) is already in the table.
			if (lEdgeBucket[lV0] == 0)
			{
				// This bucket was empty, create a new edge.
				lEdgeBucket[lV0] = new Edge;
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
				
				while (lCurrentEdge != 0 && lFound == false)
				{
					if (lCurrentEdge->IsSameEdge(lV0, lV1) == true)
					{
						lFound = true;
					}

					lPreviousEdge = lCurrentEdge;
					lCurrentEdge = lCurrentEdge->mReserved;
				}

				if (lFound == true)
				{
					// We found the edge. Since there can only be two triangles
					// sharing one edge, the second triangle index must be invalid.
					// Set it now.
					lPreviousEdge->AddTriangle(i);
				}
				else
				{
					// Create new and put in linked list.
					Edge* lEdge = new Edge;
					lEdge->mVertex[0] = lV0;
					lEdge->mVertex[1] = lV1;
					lEdge->AddTriangle(i);
					lPreviousEdge->mReserved = lEdge;
	
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
	for (i = 0; i < lVertexCount; i++)
	{
		Edge* lCurrentEdge = lEdgeBucket[i];
		while (lCurrentEdge != 0)
		{
			mEdgeData[lEdgeIndex++].Copy(lCurrentEdge);

			Edge* lPreviousEdge = lCurrentEdge;
			lCurrentEdge = lCurrentEdge->mReserved;
			delete lPreviousEdge;
		}
	}

	delete[] lEdgeBucket;

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
		uint32 lTriIndex[3];
		GetTriangleIndices(i, lTriIndex);

		int lV1  = lTriIndex[0] * 3;
		int lV2  = lTriIndex[1] * 3;
		int lV3  = lTriIndex[2] * 3;
		int lUV1 = lTriIndex[0] * 2;
		int lUV2 = lTriIndex[1] * 2;
		int lUV3 = lTriIndex[2] * 2;

		Vector3DF lEdge1(lVertexData[lV2 + 0] - lVertexData[lV1 + 0],
					 lVertexData[lV2 + 1] - lVertexData[lV1 + 1],
					 lVertexData[lV2 + 2] - lVertexData[lV1 + 2]);
		Vector3DF lEdge2(lVertexData[lV3 + 0] - lVertexData[lV1 + 0],
					 lVertexData[lV3 + 1] - lVertexData[lV1 + 1],
					 lVertexData[lV3 + 2] - lVertexData[lV1 + 2]);

		Vector2DF lEdge1UV(lUVData[lUV2 + 0] - lUVData[lUV1 + 0],
					   lUVData[lUV2 + 1] - lUVData[lUV1 + 1]);
		Vector2DF lEdge2UV(lUVData[lUV3 + 0] - lUVData[lUV1 + 0],
					   lUVData[lUV3 + 1] - lUVData[lUV1 + 1]);

		float lCP = lEdge1UV.y * lEdge2UV.x - lEdge1UV.x * lEdge2UV.y;
		//float lCP = lEdge1UV.x * lEdge2UV.y - lEdge2UV.x * lEdge1UV.y;

		if (lCP != 0.0f)
		{
			float lCPRecip = 1.0f / lCP;

			Vector3DF lTangent((lEdge1 * -lEdge2UV.y + lEdge2 * lEdge1UV.y) * lCPRecip);
			Vector3DF lBitangent((lEdge1 * -lEdge2UV.x + lEdge2 * lEdge1UV.x) * lCPRecip);
			//Vector3DF lTangent((lEdge2UV.y * lEdge1 - lEdge1UV.y * lEdge2) * lCPRecip);
			//Vector3DF lBitangent((lEdge1UV.x * lEdge2 - lEdge2UV.x * lEdge1) * lCPRecip);

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
	unsigned int lNumIndex = GetTriangleCount() * 3;
	unsigned int lNumVertex = GetVertexCount();

	bool lOk = true;
	for (unsigned int i = 0; lOk && i < lNumIndex; i++)
	{
		lOk = ((unsigned int)lIndex[i] < lNumVertex);
	}

	return lOk;
}

const TransformationF& GeometryBase::GetUVTransform()
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



LOG_CLASS_DEFINE(UI_GFX, GeometryBase);



}
