
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "UiCppContextObject.h"
#include "../../Lepra/Include/HiResTimer.h"
#include "../../UiTBC/Include/UiChunkyClass.h"



namespace UiCure
{



class BurnEmitter;
class ExhaustEmitter;
class JetEngineEmitter;



class Machine: public CppContextObject
{
public:
	typedef CppContextObject Parent;

	Machine(Cure::ResourceManager* pResourceManager, const str& pClassId, GameUiManager* pUiManager);
	virtual ~Machine();
	void SetJetEngineEmitter(JetEngineEmitter* pEmitter);
	void SetExhaustEmitter(ExhaustEmitter* pEmitter);
	void SetBurnEmitter(BurnEmitter* pEmitter);
	BurnEmitter* GetBurnEmitter() const;
	void DeleteEngineSounds();

protected:
	void OnTick();

private:
	void HandleTagEye(const UiTbc::ChunkyClass::Tag& pTag, const TBC::PhysicsManager* pPhysicsManager, bool pIsChild);
	void HandleTagBrakeLight(const UiTbc::ChunkyClass::Tag& pTag);
	void HandleTagEngineLight(const UiTbc::ChunkyClass::Tag& pTag, float pFrameTime);
	void HandleTagBlinkLight(const UiTbc::ChunkyClass::Tag& pTag, float pFrameTime);
	void HandleTagEngineSound(const UiTbc::ChunkyClass::Tag& pTag, const TBC::PhysicsManager* pPhysicsManager, const Vector3DF& pVelocity,
		float pFrameTime, float pRealTimeRatio, size_t& pEngineSoundIndex);
	void HandleTagEngineMeshOffset(const UiTbc::ChunkyClass::Tag& pTag, float pFrameTime);

	void LoadPlaySound3d(UserSound3dResource* pSoundResource);

	typedef std::hash_map<const UiTbc::ChunkyClass::Tag*, UserSound3dResource*, LEPRA_VOIDP_HASHER> TagSoundTable;
	typedef std::vector<float> TagSoundIntensityArray;

	JetEngineEmitter* mJetEngineEmitter;
	ExhaustEmitter* mExhaustEmitter;
	BurnEmitter* mBurnEmitter;
	TagSoundTable mEngineSoundTable;
	TagSoundIntensityArray mEngineSoundIntensity;
	float mBlinkTime;

	LOG_CLASS_DECLARE();
};



}
