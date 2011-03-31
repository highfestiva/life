
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../../UiCure/Include/UiCppContextObject.h"
#include "../Life.h"



namespace Life
{



class Props: public UiCure::CppContextObject
{
public:
	typedef UiCure::CppContextObject Parent;

	enum ParticleType
	{
		PARTICLE_NONE = 1,
		PARTICLE_SOLID,
		PARTICLE_GAS,
	};

	Props(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager);
	virtual ~Props();

	void SetOpacity(float pOpacity);
	void StartParticle(ParticleType pParticleType, const Vector3DF& pStartVelocity, float pScale);

protected:
	void DispatchOnLoadMesh(UiCure::UserGeometryReferenceResource* pMeshResource);
	virtual void TryAddTexture();
	void OnTick();
	void OnAlarm(int pAlarmId, void* pExtraData);

private:
	Vector3DF mVelocity;
	ParticleType mParticleType;
	float mScale;
	float mTime;
	float mOpacity;

	LOG_CLASS_DECLARE();
};



}
