
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "MassObject.h"
#include "../../Cure/Include/ContextManager.h"
#include "../../Lepra/Include/Random.h"
#include "../../UiTbc/Include/UiGeometryBatch.h"



namespace Life
{



MassObject::MassObject(Cure::ResourceManager* pResourceManager, const str& pClassResourceName,
	UiCure::GameUiManager* pUiManager, size_t pCopyCount):
	Parent(pResourceManager, pClassResourceName, pUiManager),
	mCopyCount(pCopyCount)
{
	SetPhysicsTypeOverride(PHYSICS_OVERRIDE_BONES);
}

MassObject::~MassObject()
{
	for (MassMeshArray::iterator x = mMassMeshArray.begin(); x != mMassMeshArray.end(); ++x)
	{
		GetUiManager()->GetRenderer()->RemoveGeometry(x->second);
		delete (x->first);
	}
	mMassMeshArray.clear();
}



void MassObject::OnLoaded()
{
	std::vector<TransformationF> lDisplacementArray;
	for (size_t x = 0; x < mCopyCount; ++x)
	{
		const float lDistance = 20.0f;
		QuaternionF lRotation;
		lRotation.RotateAroundOwnY((float)Random::Uniform(0, PI*2));
		lRotation.RotateAroundOwnX((float)Random::Uniform(0, PI/8));
		lRotation.RotateAroundOwnZ((float)Random::Uniform(0, PI/8));
		lDisplacementArray.push_back(TransformationF(lRotation,
			Vector3DF((float)Random::Uniform(-lDistance, lDistance), 0,
				(float)Random::Uniform(-lDistance, lDistance))));
	}
	QuaternionF lRotation;
	lRotation.RotateAroundWorldX(PIF*0.5f);
	for (MeshArray::iterator y = mMeshResourceArray.begin(); y != mMeshResourceArray.end(); ++y)
	{
		TBC::GeometryReference* lMesh = (TBC::GeometryReference*)(*y)->GetRamData();
		lMesh->SetAlwaysVisible(false);

		UiTbc::GeometryBatch* lBatch = new UiTbc::GeometryBatch(lMesh);
		lBatch->SetAlwaysVisible(true);
		lBatch->SetBasicMaterialSettings(lMesh->GetBasicMaterialSettings());
		lBatch->SetTransformation(TransformationF(lRotation, Vector3DF(0, 30, 43)));
		lBatch->SetInstances(&lDisplacementArray[0], lMesh->GetTransformation().GetPosition(), mCopyCount,
			123456, 0.5, 2, 0.5, 3, 0.5, 2);
		typedef UiTbc::Renderer R;
		R::GeometryID lGeometryId = GetUiManager()->GetRenderer()->
			AddGeometry(lBatch, R::MAT_SINGLE_COLOR_SOLID, R::NO_SHADOWS);
		mMassMeshArray.push_back(MassMeshPair(lBatch, lGeometryId));
	}

	Parent::OnLoaded();
}

void MassObject::UiMove()
{
	/*const Vector3DF lPosition(0, 50, 150);
	QuaternionF lRotation(PIF*0.5f, Vector3DF(1, 0, 0));
	const float lDistance = 20;
	const float lDistanceSquare = lDistance*lDistance;
	const size_t lMeshesPerObject = mMeshResourceArray.size();
	for (size_t x = 0; x < mMassMeshArray.size(); x += lMeshesPerObject)
	{
		if (mMassMeshArray[x].first->GetTransformation().GetPosition().GetDistanceSquared(lPosition) > lDistanceSquare)
		{
			const float lAngle = (float)Random::Uniform(0, PI*2);
			const Vector3DF lOffset(lDistance*cos(lAngle), lDistance*sin(lAngle), 0);
			for (size_t y = 0; y < lMeshesPerObject; ++y)
			{
				TBC::GeometryReference* lMesh = mMassMeshArray[x+y].first;
				lMesh->SetTransformation(TransformationF(lRotation, lOffset+lPosition));
			}
		}
	}*/
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, MassObject);



}
