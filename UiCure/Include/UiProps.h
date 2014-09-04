
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "UiCppContextObject.h"



namespace UiCure
{



class Props: public CppContextObject
{
public:
	typedef CppContextObject Parent;

	enum ParticleType
	{
		PARTICLE_NONE = 1,
		PARTICLE_SOLID,
		PARTICLE_GAS,
	};

	Props(Cure::ResourceManager* pResourceManager, const str& pClassId, GameUiManager* pUiManager);
	virtual ~Props();

	void SetOpacity(float pOpacity);
	void StartParticle(ParticleType pParticleType, const vec3& pStartVelocity, float pScale, float pAngularRange, float pTime);
	void SetFadeOutTime(float pTime);

protected:
	void DispatchOnLoadMesh(UserGeometryReferenceResource* pMeshResource);
	virtual void TryAddTexture();
	void OnTick();
	void OnAlarm(int pAlarmId, void* pExtraData);

private:
	vec3 mVelocity;
	ParticleType mParticleType;
	float mScale;
	float mTime;
	float mLifeTime;
	float mFadeOutTime;
	float mOpacity;
	bool mIsFadingOut;
	vec3 mAngularVelocity;

	logclass();
};



}
