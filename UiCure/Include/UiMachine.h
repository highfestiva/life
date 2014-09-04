
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "UiCppContextObject.h"
#include "../../Lepra/Include/GameTimer.h"
#include "../../UiTbc/Include/UiChunkyClass.h"



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
	void HandleTagAmbientSounds(const UiTbc::ChunkyClass::Tag& pTag, float pRealTimeRatio);
	void HandleTagEye(const UiTbc::ChunkyClass::Tag& pTag, const Tbc::PhysicsManager* pPhysicsManager, bool pIsChild);
	void HandleTagBrakeLight(const UiTbc::ChunkyClass::Tag& pTag);
	void HandleTagEngineLight(const UiTbc::ChunkyClass::Tag& pTag, float pFrameTime);
	void HandleTagBlinkLight(const UiTbc::ChunkyClass::Tag& pTag, float pFrameTime);
	void HandleTagEngineSound(const UiTbc::ChunkyClass::Tag& pTag, const Tbc::PhysicsManager* pPhysicsManager, const vec3& pVelocity,
		float pFrameTime, float pRealTimeRatio, size_t& pEngineSoundIndex);
	void HandleTagEngineMeshOffset(const UiTbc::ChunkyClass::Tag& pTag, float pFrameTime);
	void HandleTagMeshOffset(const UiTbc::ChunkyClass::Tag& pTag, float pFrameTime);
	void HandleTagMeshRotate(const UiTbc::ChunkyClass::Tag& pTag, float pFrameTime);

	float CalculateMeshOffset(const str& pFunction, float lStartTime, float pDuration, float pFrequency, float pFrameTime, float& pMeshTime) const;

	void LoadPlaySound3d(UserSound3dResource* pSoundResource);

	typedef std::unordered_map<const UiTbc::ChunkyClass::Tag*, UserSound3dResource*, LEPRA_VOIDP_HASHER> TagSoundTable;
	typedef std::vector<float> TagSoundIntensityArray;

	JetEngineEmitter* mJetEngineEmitter;
	ExhaustEmitter* mExhaustEmitter;
	BurnEmitter* mBurnEmitter;
	TagSoundTable mEngineSoundTable;
	TagSoundIntensityArray mEngineSoundIntensity;
	GameTimer mAmbientSoundTimer;
	float mBlinkTime;
	float mMeshOffsetTime;
	float mMeshRotateTime;
	bool mPreventEngineSounds;

	logclass();
};



}
