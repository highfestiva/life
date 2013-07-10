
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "Level.h"



namespace Life
{



Level::Level(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager, Cure::ContextForceListener* pGravelEmitter):
	Parent(pResourceManager, pClassId, pUiManager),
	mGravelEmitter(pGravelEmitter)
{
}

Level::~Level()
{
	delete mGravelEmitter;
	mGravelEmitter = 0;
}

void Level::OnLoaded()
{
	Parent::OnLoaded();

	const TBC::ChunkyClass::Tag* lTag = FindTag(_T("mass_objects"), -1, -1);
	if (lTag)
	{
		deb_assert(lTag->mStringValueList.size() == lTag->mFloatValueList.size());
		deb_assert(lTag->mStringValueList.size() == lTag->mBodyIndexList.size());
		const size_t lCount = lTag->mBodyIndexList.size();
		for (size_t x = 0; x < lCount; ++x)
		{
			MassObjectInfo lInfo;
			lInfo.mClassId = lTag->mStringValueList[x];
			lInfo.mGroundBodyIndex = lTag->mBodyIndexList[x];
			lInfo.mCount = (int)lTag->mFloatValueList[x];
			mMassObjects.push_back(lInfo);
		}
	}
}



Level::MassObjectList Level::GetMassObjects() const
{
	return mMassObjects;
}



void Level::OnForceApplied(Cure::ContextObject* pOtherObject,
	TBC::PhysicsManager::BodyID pOwnBodyId, TBC::PhysicsManager::BodyID pOtherBodyId,
	const Vector3DF& pForce, const Vector3DF& pTorque,
	const Vector3DF& pPosition, const Vector3DF& pRelativeVelocity)
{
	Parent::OnForceApplied(pOtherObject, pOwnBodyId, pOtherBodyId, pForce, pTorque, pPosition, pRelativeVelocity);

	mGravelEmitter->OnForceApplied(this, pOtherObject, pOwnBodyId, pOtherBodyId, pForce, pTorque, pPosition, pRelativeVelocity);
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, Level);



}
