
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "UiCppContextObject.h"
#include "../../Lepra/Include/HiResTimer.h"
#include "../../UiTBC/Include/UiChunkyClass.h"



namespace UiCure
{



class Machine: public CppContextObject
{
public:
	typedef CppContextObject Parent;

	Machine(Cure::ResourceManager* pResourceManager, const str& pClassId, GameUiManager* pUiManager);
	virtual ~Machine();
	void DeleteEngineSounds();

protected:
	void OnTick();

private:
	virtual void OnForceApplied(Cure::ContextObject* pOtherObject,
		TBC::PhysicsManager::BodyID pOwnBodyId, TBC::PhysicsManager::BodyID pOtherBodyId,
		const Vector3DF& pForce, const Vector3DF& pTorque,
		const Vector3DF& pPosition, const Vector3DF& pRelativeVelocity);

	void LoadPlaySound3d(UserSound3dResource* pSoundResource);

	typedef std::hash_map<const UiTbc::ChunkyClass::Tag*, UserSound3dResource*, LEPRA_VOIDP_HASHER> TagSoundTable;
	typedef std::vector<float> TagSoundIntensityArray;

	TagSoundTable mEngineSoundTable;
	TagSoundIntensityArray mEngineSoundIntensity;
	HiResTimer mParticleTimer;
	float mExhaustTimeout;
	bool mCreatedParticles;

	LOG_CLASS_DECLARE();
};



}
