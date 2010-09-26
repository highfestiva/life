
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "Vehicle.h"



namespace Life
{



class Props: public Vehicle
{
public:
	typedef Vehicle Parent;

	Props(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager);
	virtual ~Props();

	void StartParticle(const Vector3DF& pStartVelocity);

protected:
	void DispatchOnLoadMesh(UiCure::UserGeometryReferenceResource* pMeshResource);
	void OnPhysicsTick();
	void OnAlarm(int pAlarmId, void* pExtraData);

private:
	Vector3DF mVelocity;
	bool mIsParticle;
};



}
