// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/UiMaterial.h"
#include "../../Tbc/Include/GeometryBase.h"
#include "../../Lepra/Include/ListUtil.h"
#include "../../Lepra/Include/Math.h"



namespace UiTbc
{



GeometryGroup::GeometryGroup(Material* pMaterial, int pAllocSize) :
	mParentMaterial(pMaterial),
	mGeomArray(new Pair[pAllocSize]),
	mGeometryCount(0),
	mArrayLength(pAllocSize),
	mMeanDepth(0),
	mGroupTextureID(Renderer::INVALID_TEXTURE)
{
}

GeometryGroup::~GeometryGroup()
{
	delete[] mGeomArray;
	mGeomArray = 0;
}

void GeometryGroup::AddGeometry(Tbc::GeometryBase* pGeometry)
{
	if (mGeometryCount >= mArrayLength)
	{
		// Realloc... Increase memory usage by a constant
		// to avoid excessive use of memory.
		mArrayLength += 8;
		Pair* lNewArray = new Pair[mArrayLength];
		::memcpy(lNewArray, mGeomArray, mGeometryCount * sizeof(Pair));
		delete[] mGeomArray;
		mGeomArray = lNewArray;
	}

	Renderer::GeometryData* lGeomData = (Renderer::GeometryData*)pGeometry->GetRendererData();
	deb_assert(lGeomData != 0);
	lGeomData->mGeometryGroup = this;
	mGeomArray[mGeometryCount++].mGeometry = pGeometry;
}

bool GeometryGroup::RemoveGeometry(Tbc::GeometryBase* pGeometry)
{
	int i;

	// Search for the geometry...
	for (i = 0; i < mGeometryCount && mGeomArray[i].mGeometry != pGeometry; i++) {}

	bool lRemoved = false;
	if (i < mGeometryCount)
	{
		// The geometry has been found.
		lRemoved = true;
		mGeometryCount--;
		for (int j = i; j < mGeometryCount; j++)
		{
			mGeomArray[j] = mGeomArray[j + 1];
		}
	}
	return lRemoved;
}

int GeometryGroup::CalculateDepths(bool pF2B)
{
	mMeanDepth = 0.0f;

	const xform& lCam = mParentMaterial->GetRenderer()->GetCameraTransformation();
	const quat& lCamOrientation = lCam.GetOrientation();
	const quat& lCamOrientationInverse = mParentMaterial->GetRenderer()->GetCameraOrientationInverse();
	const vec3& lCamPosition = lCam.GetPosition();
	int lInversionCount = 0;

	// The first depth goes outside the loop...
	vec3 lTemp;
	lCamOrientation.FastInverseRotatedVector(lCamOrientationInverse, lTemp, mGeomArray[0].mGeometry->GetTransformation().GetPosition() - lCamPosition);
	mGeomArray[0].mDepth = lTemp.y;
	mMeanDepth += mGeomArray[0].mDepth;

	int i;
	for (i = 1; i < mGeometryCount; i++)
	{
		lCamOrientation.FastInverseRotatedVector(lCamOrientationInverse, lTemp, mGeomArray[i].mGeometry->GetTransformation().GetPosition() - lCamPosition);
		mGeomArray[i].mDepth = lTemp.y;
		mMeanDepth += mGeomArray[i].mDepth;

		if(pF2B)
		{
			if (mGeomArray[i - 1].mDepth > mGeomArray[i].mDepth)
				++lInversionCount;
		}
		else
		{
			if (mGeomArray[i - 1].mDepth < mGeomArray[i].mDepth)
				++lInversionCount;
		}
	}

	mMeanDepth /= (float)mGeometryCount;

	return lInversionCount;
}

void GeometryGroup::F2BSortGroup()
{
	int lInversionCount = CalculateDepths(true);

	if (lInversionCount == 0)
	{
		// Already sorted.
		return;
	}

	if (mGeometryCount < 4 || lInversionCount < Math::Log2(mGeometryCount))
	{
		// Bubble sorting is faster for almost sorted lists.
		BubbleSort(F2BCompare);
	}
	else
	{
		::qsort(mGeomArray, mGeometryCount, sizeof(Pair), F2BCompare);
	}
}

void GeometryGroup::B2FSortGroup()
{
	int lInversionCount = CalculateDepths(false);

	if (lInversionCount == 0)
	{
		// Already sorted.
		return;
	}

	if (mGeometryCount < 4 || lInversionCount < Math::Log2(mGeometryCount))
	{
		// Bubble sorting is faster for almost sorted lists.
		BubbleSort(B2FCompare);
	}
	else
	{
		::qsort(mGeomArray, mGeometryCount, sizeof(Pair), B2FCompare);
	}
}

void GeometryGroup::BubbleSort(int (*Cmp)(const void* pGeom1, const void* pGeom2))
{
	bool lDone = false;
	while (lDone == false)
	{
		lDone = true;
		for (int i = 1; i < mGeometryCount; i++)
		{
			if (Cmp(&mGeomArray[i - 1], &mGeomArray[i]) > 0)
			{
				Pair lTmp = mGeomArray[i - 1];
				mGeomArray[i - 1] = mGeomArray[i];
				mGeomArray[i] = lTmp;
				lDone = false;
			}
		}
	}
}

int GeometryGroup::F2BCompare(const void* pPair1, const void* pPair2)
{
	if (((Pair*)pPair1)->mDepth < ((Pair*)pPair2)->mDepth)
	{
		return -1;
	}
	else if (((Pair*)pPair1)->mDepth > ((Pair*)pPair2)->mDepth)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int GeometryGroup::B2FCompare(const void* pPair1, const void* pPair2)
{
	if (((Pair*)pPair1)->mDepth > ((Pair*)pPair2)->mDepth)
	{
		return -1;
	}
	else if (((Pair*)pPair1)->mDepth < ((Pair*)pPair2)->mDepth)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}



Material::Material(Renderer* pRenderer, DepthSortHint pSortHint, Material* pFallBackMaterial):
	mRenderer(pRenderer),
	mSortHint(pSortHint),
	mFallBackMaterial(pFallBackMaterial)
{
}

Material::~Material()
{
	RemoveAllGeometry();
}



void Material::EnableWireframe(bool pEnabled)
{
	mEnableWireframe = pEnabled;
}

void Material::SetEnableDepthSorting(bool pEnabled)
{
	mEnableDepthSort = pEnabled;
}

void Material::EnableDrawMaterial(bool pEnabled)
{
	mEnableDrawMaterial = pEnabled;
}



Renderer* Material::GetRenderer()
{
	return mRenderer;
}

bool Material::AddGeometry(Tbc::GeometryBase* pGeometry)
{
	if (pGeometry == 0)
		return false;


	GeometryGroupList::iterator lIter;
	for (lIter = mGeometryGroupList.begin(); lIter != mGeometryGroupList.end(); ++lIter)
	{
		if ((*lIter)->GetGroupTextureID() == GetGroupTextureID(pGeometry))
		{
			break;
		}
	}

	GeometryGroup* lGroup = 0;
	if (lIter != mGeometryGroupList.end())
	{
		lGroup = *lIter;
	}
	else
	{
		lGroup = new GeometryGroup(this);
		mGeometryGroupList.push_back(lGroup);
	}

	lGroup->AddGeometry(pGeometry);

	return true;
}

Material::RemoveStatus Material::RemoveGeometry(Tbc::GeometryBase* pGeometry)
{
	Renderer::GeometryData* lGeomData = (Renderer::GeometryData*)pGeometry->GetRendererData();
	RemoveStatus lStatus = (lGeomData->mGeometryGroup->RemoveGeometry(pGeometry) ? REMOVED : NOT_REMOVED);

	if (lGeomData->mGeometryGroup->GetGeometryCount() == 0)
	{
		Material* lMaterial = this;
		while (lMaterial && !ListUtil::Contains(lMaterial->mGeometryGroupList, lGeomData->mGeometryGroup))
		{
			lMaterial = lMaterial->mFallBackMaterial;
		}
		deb_assert(lMaterial);
		if (lMaterial)
		{
			lMaterial->mGeometryGroupList.remove(lGeomData->mGeometryGroup);
			delete lGeomData->mGeometryGroup;
			lGeomData->mGeometryGroup = 0;
		}
	}

	return lStatus;
}

void Material::RemoveAllGeometry()
{
	GeometryGroupList::iterator lIter;
	while(mGeometryGroupList.empty() == false)
	{
		GeometryGroup* lGroup = mGeometryGroupList.front();
		mGeometryGroupList.pop_front();
		delete lGroup;
	}
}

void Material::PreRender()
{
}

void Material::PostRender()
{
}

void Material::RenderAllGeometry(unsigned pCurrentFrame, Material* pGeometryContainer, Material* pRenderer)
{
	const GeometryGroupList& lGeometries = pGeometryContainer->GetGeometryGroupList();
	if (lGeometries.empty())
	{
		return;
	}
	if (!pRenderer)
	{
		pRenderer = pGeometryContainer;
	}
	pRenderer->RenderAllGeometry(pCurrentFrame, lGeometries);
}

Tbc::GeometryBase* Material::GetFirstGeometry()
{
	mGroupIter = mGeometryGroupList.begin();
	mIndex = 0;
	Tbc::GeometryBase* lGeometry = 0;
	if(mGroupIter != mGeometryGroupList.end())
	{
		lGeometry = (*mGroupIter)->GetGeometry(mIndex);
	}
	return lGeometry;
}

Tbc::GeometryBase* Material::GetNextGeometry()
{
	Tbc::GeometryBase* lGeometry = 0;
	if (mGroupIter != mGeometryGroupList.end())
	{
		++mIndex;
		if(mIndex >= (*mGroupIter)->GetGeometryCount())
		{
			mIndex = 0;
			++mGroupIter;
		}
	}
	if(mGroupIter != mGeometryGroupList.end())
	{
		lGeometry = (*mGroupIter)->GetGeometry(mIndex);
	}
	return lGeometry;
}

void Material::RenderAllGeometry(unsigned pCurrentFrame, const GeometryGroupList& pGeometryGroupList)
{
	if (mEnableDrawMaterial)
	{
		DoRenderAllGeometry(pCurrentFrame, pGeometryGroupList);
	}
	else
	{
		Material::DoRenderAllGeometry(pCurrentFrame, pGeometryGroupList);
	}
}

void Material::RenderAllBlendedGeometry(unsigned pCurrentFrame, const GeometryGroupList& pGeometryGroupList)
{
	Tbc::GeometryBase::BasicMaterialSettings lPreviousMaterial = mCurrentMaterial;
	bool lOldEnableDrawMaterial = mEnableDrawMaterial;
	mEnableDrawMaterial = true;
	mEnableDepthSort = true;

	DoRenderAllGeometry(pCurrentFrame, pGeometryGroupList);

	SetBasicMaterial(lPreviousMaterial);
	GetRenderer()->ResetAmbientLight(true);
	mEnableDrawMaterial = lOldEnableDrawMaterial;
	mEnableDepthSort = false;
}

void Material::DoRenderAllGeometry(unsigned pCurrentFrame, const GeometryGroupList& pGeometryGroupList)
{
	PreRender();

	GeometryGroupList::const_iterator lIter;
	for (lIter = pGeometryGroupList.begin(); lIter != pGeometryGroupList.end(); ++lIter)
	{
		GeometryGroup* lGroup = *lIter;

		if (mEnableDepthSort == true)
		{
			if (mSortHint == DEPTHSORT_F2B)
			{
				lGroup->F2BSortGroup();
			}
			else if (mSortHint == DEPTHSORT_B2F)
			{
				lGroup->B2FSortGroup();
			}
		}

		const int lGeometryCount = lGroup->GetGeometryCount();
		for (int i = 0; i < lGeometryCount; i++)
		{
			Tbc::GeometryBase* lGeometry = lGroup->GetGeometry(i);
			if (lGeometry->GetAlwaysVisible() || lGeometry->GetLastFrameVisible() == pCurrentFrame)
			{
				if (mRenderer->PreRender(lGeometry))
				{
					if (mEnableDrawMaterial)
					{
						RenderGeometry(lGeometry);
					}
					else
					{
						RenderBaseGeometry(lGeometry);
					}
				}
				mRenderer->PostRender(lGeometry);
			}
		}
	}

	PostRender();
}

Renderer::TextureID Material::GetGroupTextureID(Tbc::GeometryBase* pGeometry) const
{
	Renderer::TextureID lTextureID = Renderer::INVALID_TEXTURE;

	Renderer::GeometryData* lGeomData = (Renderer::GeometryData*)pGeometry->GetRendererData();
	if (lGeomData->mTA && lGeomData->mTA->mNumTextures > 0)
	{
		// Select the first texture for grouping.
		lTextureID = lGeomData->mTA->mTextureID[0];
	}

	return lTextureID;
}

const Material::GeometryGroupList& Material::GetGeometryGroupList() const
{
	return mGeometryGroupList;
}



Tbc::GeometryBase::BasicMaterialSettings Material::mCurrentMaterial;
bool Material::mEnableWireframe = false;
bool Material::mEnableDepthSort = false;
bool Material::mEnableDrawMaterial = true;



}
