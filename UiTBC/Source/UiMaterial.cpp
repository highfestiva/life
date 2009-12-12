/*
	Class:  Material
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games
*/

#include "../Include/UiMaterial.h"
#include "../../TBC/Include/GeometryBase.h"
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
}

void GeometryGroup::AddGeometry(TBC::GeometryBase* pGeometry)
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
	assert(lGeomData != 0);
	lGeomData->mGeometryGroup = this;
	mGeomArray[mGeometryCount++].mGeometry = pGeometry;
}

bool GeometryGroup::RemoveGeometry(TBC::GeometryBase* pGeometry)
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

	const Lepra::TransformationF& lCam = mParentMaterial->GetRenderer()->GetCameraTransformation();
	int lInversionCount = 0;

	// The first depth goes outside the loop...
	mGeomArray[0].mDepth = lCam.InverseTransform(mGeomArray[0].mGeometry->GetTransformation()).GetPosition().y;
	mMeanDepth += mGeomArray[0].mDepth;

	int i;
	for (i = 1; i < mGeometryCount; i++)
	{
		mGeomArray[i].mDepth = lCam.InverseTransform(mGeomArray[i].mGeometry->GetTransformation()).GetPosition().y;
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

	if (mGeometryCount < 4 || lInversionCount < Lepra::Math::Log2(mGeometryCount))
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

	if (mGeometryCount < 4 || lInversionCount < Lepra::Math::Log2(mGeometryCount))
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

void Material::SetDepthSortingEnabled(bool pEnabled)
{
	smDepthSortEnabled = pEnabled;
}

bool Material::AddGeometry(TBC::GeometryBase* pGeometry)
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

Material::RemoveStatus Material::RemoveGeometry(TBC::GeometryBase* pGeometry)
{
	Renderer::GeometryData* lGeomData = (Renderer::GeometryData*)pGeometry->GetRendererData();
	RemoveStatus lStatus = (lGeomData->mGeometryGroup->RemoveGeometry(pGeometry) ? REMOVED : NOT_REMOVED);

	if (lGeomData->mGeometryGroup->GetGeometryCount() == 0)
	{
		mGeometryGroupList.remove(lGeomData->mGeometryGroup);
		delete lGeomData->mGeometryGroup;
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

void Material::RenderAllGeometry(unsigned int pCurrentFrame)
{
	GeometryGroupList::iterator lIter;
	for (lIter = mGeometryGroupList.begin(); lIter != mGeometryGroupList.end(); ++lIter)
	{
		GeometryGroup* lGroup = *lIter;

		if (smDepthSortEnabled == true)
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

		for (int i = 0; i < lGroup->GetGeometryCount(); i++)
		{
			TBC::GeometryBase* lGeometry = lGroup->GetGeometry(i);
			if (lGeometry->GetAlwaysVisible() == true ||
			   lGeometry->GetLastFrameVisible() == pCurrentFrame)
			{
				if (mRenderer->PreRender(lGeometry))
				{
					RenderGeometry(lGeometry);
				}
				mRenderer->PostRender(lGeometry);
			}
		}
	}
}

TBC::GeometryBase* Material::GetFirstGeometry()
{
	mGroupIter = mGeometryGroupList.begin();
	mIndex = 0;
	TBC::GeometryBase* lGeometry = 0;
	if(mGroupIter != mGeometryGroupList.end())
		lGeometry = (*mGroupIter)->GetGeometry(mIndex);
	return lGeometry;
}

TBC::GeometryBase* Material::GetNextGeometry()
{
	TBC::GeometryBase* lGeometry = 0;

	if(mGroupIter != mGeometryGroupList.end())
	{
		++mIndex;
		if(mIndex >= (*mGroupIter)->GetGeometryCount())
		{
			mIndex = 0;
			++mGroupIter;
		}
	}

	if(mGroupIter != mGeometryGroupList.end())
		lGeometry = (*mGroupIter)->GetGeometry(mIndex);

	return lGeometry;
}

TBC::GeometryBase* Material::GetFirstVisibleGeometry()
{
	unsigned int lCurrentFrame = mRenderer->GetCurrentFrame();
	TBC::GeometryBase* lGeometry = GetFirstGeometry();
	while(lGeometry != 0 && (lGeometry->GetAlwaysVisible() == false || lGeometry->GetLastFrameVisible() != lCurrentFrame))
	{
		lGeometry = GetNextGeometry();
	}
	return lGeometry;
}

TBC::GeometryBase* Material::GetNextVisibleGeometry()
{
	unsigned int lCurrentFrame = mRenderer->GetCurrentFrame();
	TBC::GeometryBase* lGeometry = GetNextGeometry();
	while(lGeometry != 0 && (lGeometry->GetAlwaysVisible() == false || lGeometry->GetLastFrameVisible() != lCurrentFrame))
	{
		lGeometry = GetNextGeometry();
	}
	return lGeometry;
}

Renderer::TextureID Material::GetGroupTextureID(TBC::GeometryBase* pGeometry)
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

bool Material::smDepthSortEnabled = false;

} // End namespace.
