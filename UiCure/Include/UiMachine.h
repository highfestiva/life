
// Author: Jonas Bystr�m
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "UiCppContextObject.h"
#include "../../Lepra/Include/HiResTimer.h"
#include "../../UiTBC/Include/UiChunkyClass.h"



namespace UiCure
{



class ExhaustEmitter;
class FireEmitter;



class Machine: public CppContextObject
{
public:
	typedef CppContextObject Parent;

	Machine(Cure::ResourceManager* pResourceManager, const str& pClassId, GameUiManager* pUiManager);
	virtual ~Machine();
	void SetFireEmitter(FireEmitter* pEmitter);
	void SetExhaustEmitter(ExhaustEmitter* pEmitter);
	void DeleteEngineSounds();

protected:
	void OnTick();

private:
	void HandleTagEye(const UiTbc::ChunkyClass::Tag& pTag, const TBC::PhysicsManager* pPhysicsManager, bool pIsChild);
	void HandleTagBrakeLight(const UiTbc::ChunkyClass::Tag& pTag);
	void HandleTagEngineLight(const UiTbc::ChunkyClass::Tag& pTag, float pFrameTime);
	void HandleTagEngineSound(const UiTbc::ChunkyClass::Tag& pTag, const TBC::PhysicsManager* pPhysicsManager, const Vector3DF& pVelocity,
		float pFrameTime, float pRealTimeRatio, size_t& pEngineSoundIndex);
	void HandleTagEngineMeshOffset(const UiTbc::ChunkyClass::Tag& pTag, float pFrameTime);

	void LoadPlaySound3d(UserSound3dResource* pSoundResource);

	typedef std::hash_map<const UiTbc::ChunkyClass::Tag*, UserSound3dResource*, LEPRA_VOIDP_HASHER> TagSoundTable;
	typedef std::vector<float> TagSoundIntensityArray;

	FireEmitter* mFireEmitter;
	ExhaustEmitter* mExhaustEmitter;
	TagSoundTable mEngineSoundTable;
	TagSoundIntensityArray mEngineSoundIntensity;

	LOG_CLASS_DECLARE();
};



}
