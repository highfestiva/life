
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



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
	void StartParticle(ParticleType pParticleType, const Vector3DF& pStartVelocity, float pScale);

protected:
	void DispatchOnLoadMesh(UserGeometryReferenceResource* pMeshResource);
	virtual void TryAddTexture();
	void OnTick();
	void OnAlarm(int pAlarmId, void* pExtraData);

private:
	Vector3DF mVelocity;
	ParticleType mParticleType;
	float mScale;
	float mTime;
	float mOpacity;
	Vector3DF mAngularVelocity;

	LOG_CLASS_DECLARE();
};



}
