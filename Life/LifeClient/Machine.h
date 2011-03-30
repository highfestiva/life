
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../../Lepra/Include/HiResTimer.h"
#include "../../UiCure/Include/UiCppContextObject.h"
#include "../../UiTBC/Include/UiChunkyClass.h"
#include "../Life.h"



namespace Life
{



class Machine: public UiCure::CppContextObject
{
public:
	typedef UiCure::CppContextObject Parent;

	Machine(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager);
	virtual ~Machine();

protected:
	void OnTick();

private:
	void OnForceApplied(TBC::PhysicsManager::ForceFeedbackListener* pOtherObject,
		TBC::PhysicsManager::BodyID pOwnBodyId, TBC::PhysicsManager::BodyID pOtherBodyId,
		const Vector3DF& pForce, const Vector3DF& pTorque,
		const Vector3DF& pPosition, const Vector3DF& pRelativeVelocity);

	void LoadPlaySound3d(UiCure::UserSound3dResource* pSoundResource);

	float GetAttributeFloatValue(const str& pAttributeName) const;

	typedef std::hash_map<const UiTbc::ChunkyClass::Tag*, UiCure::UserSound3dResource*, LEPRA_VOIDP_HASHER> TagSoundTable;

	TagSoundTable mEngineSoundTable;
	HiResTimer mParticleTimer;
	float mExhaustTimeout;
	bool mCreatedParticles;

	LOG_CLASS_DECLARE();
};



}
