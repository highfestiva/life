/*
	Class:  GraphicalModel
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games
*/

#include "../Include/UiGraphicalModel.h"
#include "../Include/UiTriangleBasedGeometry.h"
#include "../Include/UiAnimatedGeometry.h"
#include "../Include/UiProgressiveTriangleGeometry.h"

namespace UiTbc
{

GraphicalModel::GraphicalModel() :
	mLevelOfDetail(1)
{
}

GraphicalModel::~GraphicalModel()
{
}

void GraphicalModel::AddGeometry(const str& pName, GeometryHandler* pGeometry, const str& pTransformAnimator)
{
	pGeometry->SetTransformAnimator(Model::GetAnimator(pTransformAnimator));
	pGeometry->UpdateGeometry((float)mLevelOfDetail);
	mGeometryTable.Insert(pName, pGeometry);
}

TBC::GeometryBase* GraphicalModel::GetGeometry(const str& pName)
{
	TBC::GeometryBase* lGeometry = 0;
	GeometryTable::Iterator lIter = mGeometryTable.Find(pName);
	if (lIter != mGeometryTable.End())
	{
		lGeometry = (*lIter)->GetGeometry();
	}

	return lGeometry;
}

void GraphicalModel::Update(double pDeltaTime)
{
	Model::Update(pDeltaTime);

	GeometryTable::Iterator lIter;
	for (lIter = mGeometryTable.First(); lIter != mGeometryTable.End(); ++lIter)
	{
		GeometryHandler* lGeometryHandler = *lIter;
		lGeometryHandler->UpdateGeometry((float)mLevelOfDetail);

		TBC::GeometryBase* lGeometry = lGeometryHandler->GetGeometry();
		TBC::BoneAnimator* lAnimator = lGeometryHandler->GetTransformAnimator();

		// Set transformation.
		TransformationF lTransform;
		if (lAnimator != 0)
		{
			lTransform = mTransformation.Transform(lAnimator->GetBones()->GetRelativeBoneTransformation(0));
		}
		else
		{
			lTransform = mTransformation;
		}

		lGeometry->SetTransformation(lTransform);

		if(lGeometry->GetUVAnimator() != 0)
		{
			lGeometry->GetUVAnimator()->Step((float)pDeltaTime);
		}
	}
}

void GraphicalModel::SetDetailLevel(double pLevelOfDetail)
{
	mLevelOfDetail = pLevelOfDetail;
}

void GraphicalModel::SetLastFrameVisible(unsigned int pLastFrameVisible)
{
	GeometryTable::Iterator lIter;
	for (lIter = mGeometryTable.First(); lIter != mGeometryTable.End(); ++lIter)
	{
		(*lIter)->GetGeometry()->SetLastFrameVisible(pLastFrameVisible);
	}
}

unsigned int GraphicalModel::GetLastFrameVisible() const
{
	unsigned int lLastFrameVisible = 0;

	if (mGeometryTable.IsEmpty() == false)
	{
		lLastFrameVisible = (*mGeometryTable.First())->GetGeometry()->GetLastFrameVisible();
	}
	return lLastFrameVisible;
}

void GraphicalModel::SetAlwaysVisible(bool pAlwaysVisible)
{
	GeometryTable::Iterator lIter;
	for (lIter = mGeometryTable.First(); lIter != mGeometryTable.End(); ++lIter)
	{
		(*lIter)->GetGeometry()->SetAlwaysVisible(pAlwaysVisible);
	}
}

bool GraphicalModel::GetAlwaysVisible()
{
	bool lAlwaysVisible = false;

	if (mGeometryTable.IsEmpty() == false)
	{
		lAlwaysVisible = (*mGeometryTable.First())->GetGeometry()->GetAlwaysVisible();
	}
	return lAlwaysVisible;
}








DefaultStaticGeometryHandler::DefaultStaticGeometryHandler(TriangleBasedGeometry* pGeometry,
							   int pNumLODLevels,
							   Renderer::TextureID* pTextureID,
							   int pNumTextures,
							   Renderer::MaterialType pMaterial, 
							   Renderer::Shadows pShadows,
							   Renderer* pRenderer) :
	mGeometry(pGeometry),
	mNumLODLevels(pNumLODLevels),
	mTextureID(pTextureID),
	mNumTextures(pNumTextures),
	mMaterial(pMaterial),
	mShadows(pShadows),
	mCurrentLODLevel(-1),
	mGeomID(Renderer::INVALID_GEOMETRY),
	mRenderer(pRenderer)
{
}

void DefaultStaticGeometryHandler::UpdateGeometry(float pLODLevel)
{
	int lNewLODIndex = (int)std::min(mNumLODLevels - 1, (int)floor(pLODLevel * (mNumLODLevels - 1) + 0.5));

	if (lNewLODIndex != mCurrentLODLevel)
	{
		mCurrentLODLevel = lNewLODIndex;
		mRenderer->RemoveGeometry(mGeomID);
		mGeomID = mRenderer->AddGeometry(&mGeometry[lNewLODIndex], 
						   mMaterial,
						   mShadows);
		if (mGeomID != Renderer::INVALID_GEOMETRY)
		{
			for (int x = 0; x < mNumTextures; ++x)
			{
				mRenderer->TryAddGeometryTexture(mGeomID, mTextureID[x]);
			}
		}
	}
}

TBC::GeometryBase* DefaultStaticGeometryHandler::GetGeometry()
{
	TBC::GeometryBase* lGeometry = 0;
	if (mCurrentLODLevel >= 0 && mCurrentLODLevel < mNumLODLevels)
	{
		lGeometry = &mGeometry[mCurrentLODLevel];
	}
	return lGeometry;
}




DefaultProgressiveGeometryHandler::DefaultProgressiveGeometryHandler(ProgressiveTriangleGeometry* pGeometry, 
								     Renderer::MaterialType pMaterial,
								     Renderer::TextureID* pTextureID,
								     int pNumTextures,
								     Renderer::Shadows pShadows,
								     Renderer* pRenderer) :
	mGeometry(pGeometry),
	mRenderer(pRenderer)
{
	mGeomID = mRenderer->AddGeometry(mGeometry, pMaterial, pShadows);
	if (mGeomID != Renderer::INVALID_GEOMETRY)
	{
		for (int x = 0; x < pNumTextures; ++x)
		{
			mRenderer->TryAddGeometryTexture(mGeomID, pTextureID[x]);
		}
	}
}

void DefaultProgressiveGeometryHandler::UpdateGeometry(float pLODLevel)
{
	mGeometry->SetDetailLevel(pLODLevel);
}

TBC::GeometryBase* DefaultProgressiveGeometryHandler::GetGeometry()
{
	return mGeometry;
}




DefaultAnimatedStaticGeometryHandler::DefaultAnimatedStaticGeometryHandler(AnimatedGeometry* pGeometry,
									   int pNumLODLevels,
									   Renderer::TextureID* pTextureID,
									   int pNumTextures,
									   Renderer::MaterialType pMaterial, 
									   Renderer::Shadows pShadows,
									   Renderer* pRenderer) :
	mGeometry(pGeometry),
	mNumLODLevels(pNumLODLevels),
	mTextureID(pTextureID),
	mNumTextures(pNumTextures),
	mMaterial(pMaterial),
	mShadows(pShadows),
	mCurrentLODLevel(-1),
	mGeomID(Renderer::INVALID_GEOMETRY),
	mRenderer(pRenderer)
{
}

void DefaultAnimatedStaticGeometryHandler::UpdateGeometry(float pLODLevel)
{
	int lNewLODIndex = (int)std::min(mNumLODLevels - 1, (int)floor(pLODLevel * (mNumLODLevels - 1) + 0.5));

	if (lNewLODIndex != mCurrentLODLevel)
	{
		mCurrentLODLevel = lNewLODIndex;
		mRenderer->RemoveGeometry(mGeomID);
		mGeomID = mRenderer->AddGeometry(&mGeometry[lNewLODIndex], 
						   mMaterial,
						   mShadows);
		if (mGeomID != Renderer::INVALID_GEOMETRY)
		{
			for (int x = 0; x < mNumTextures; ++x)
			{
				mRenderer->TryAddGeometryTexture(mGeomID, mTextureID[x]);
			}
		}
	}

	mGeometry[mCurrentLODLevel].UpdateAnimatedGeometry();
}

TBC::GeometryBase* DefaultAnimatedStaticGeometryHandler::GetGeometry()
{
	TBC::GeometryBase* lGeometry = 0;
	if (mCurrentLODLevel >= 0 && mCurrentLODLevel < mNumLODLevels)
	{
		lGeometry = &mGeometry[mCurrentLODLevel];
	}
	return lGeometry;
}




DefaultAnimatedProgressiveGeometryHandler::DefaultAnimatedProgressiveGeometryHandler(AnimatedGeometry* pGeometry,
										     Renderer::TextureID* pTextureID,
										     int pNumTextures,
										     Renderer::MaterialType pMaterial, 
										     Renderer::Shadows pShadows,
										     Renderer* pRenderer) :
	mGeometry(pGeometry),
	mTextureID(pTextureID),
	mNumTextures(pNumTextures),
	mMaterial(pMaterial),
	mShadows(pShadows),
	mGeomID(Renderer::INVALID_GEOMETRY),
	mRenderer(pRenderer)
{
}

void DefaultAnimatedProgressiveGeometryHandler::UpdateGeometry(float pLODLevel)
{
	// TRICKY: This typecast is the reason why the user needs to be extra careful
	//         using this class.
	((ProgressiveTriangleGeometry*)mGeometry->GetOriginalGeometry())->SetDetailLevel(pLODLevel);
	mGeometry->UpdateAnimatedGeometry();
}

TBC::GeometryBase* DefaultAnimatedProgressiveGeometryHandler::GetGeometry()
{
	return mGeometry;
}

} // End namespace.
