
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/UiChunkyClass.h"
#include <assert.h>
#include "../../TBC/Include/ChunkyPhysics.h"



namespace UiTbc
{



ChunkyClass::Material::Material():
	mAmbient(0,0,0),
	mDiffuse(1,1,1),
	mSpecular(0.1f,0.1f,0.1f),
	mShininess(0),
	mAlpha(1)
{
}



ChunkyClass::ChunkyClass()
{
}

ChunkyClass::~ChunkyClass()
{
}



/*		int lPhysIndex = -1;
		Lepra::String lName;
		Lepra::TransformationF lTransform;
		GetRamData()->GetMesh(x, lPhysIndex, lName, lTransform);
		UserGeometryReferenceResource* lMesh;
		lMesh = new UserGeometryReferenceResource(mUiManager, GeometryOffset(lPhysIndex, lTransform));
		lMesh->SetParentResource(GetFirstUserResource());
		lMesh->Load(lName+_T(".mesh"),
			UiCure::UserGeometryReferenceResource::TypeLoadCallback(this,
				&ResourceTest::MeshLoadCallback));*/



void ChunkyClass::AddMesh(int pPhysIndex, const Lepra::String& pMeshBaseName, const Lepra::TransformationF& pTransform)
{
	mMeshArray.push_back(PhysMeshEntry(pPhysIndex, pMeshBaseName, pTransform));
}

size_t ChunkyClass::GetMeshCount() const
{
	return (mMeshArray.size());
}

void ChunkyClass::GetMesh(size_t pIndex, int& pPhysIndex, Lepra::String& pMeshBaseName,
	Lepra::TransformationF& pTransform) const
{
	assert(pIndex < mMeshArray.size());
	pPhysIndex = mMeshArray[pIndex].mPhysIndex;
	pMeshBaseName = mMeshArray[pIndex].mMeshBaseName;
	pTransform = mMeshArray[pIndex].mTransform;
}

void ChunkyClass::SetLastMeshMaterial(const Material& pMaterial)
{
	assert(!mMeshArray.empty());
	const size_t lLastMesh = mMeshArray.size()-1;
	mMeshArray[lLastMesh].mMaterial = pMaterial;
}

const ChunkyClass::Material& ChunkyClass::GetMaterial(size_t pMeshIndex) const
{
	assert(pMeshIndex < mMeshArray.size());
	return (mMeshArray[pMeshIndex].mMaterial);
}



LOG_CLASS_DEFINE(GAME_CONTEXT, ChunkyClass);



}
