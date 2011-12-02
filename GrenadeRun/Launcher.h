
// Author: Jonas Byström
// Copyright (c) 2002-2011, Pixel Doctrine



#pragma once

#include "../UiCure/Include/UiProps.h"
#include "Game.h"



namespace GrenadeRun
{



class Launcher: public UiCure::Props
{
	typedef UiCure::Props Parent;
public:
	Launcher(Game* pGame);
	virtual ~Launcher();

	void SetBarrelAngle(float pYaw, float pPitch);

	void GetAngles(const Cure::ContextObject* pTarget, float& pPitch, float& pGuidePitch,
		float& pYaw, float& pGuideYaw) const;
	void GetAngles(const Vector3DF& pTargetPosition, const Vector3DF& pTargetVelocity,
		float& pPitch, float& pGuidePitch, float& pYaw, float& pGuideYaw) const;

private:
	void GetBallisticData(const Vector3DF& pPosition1, const Vector3DF& pPosition2,
		float pPitch, float& pGuidePitch, float& pGuideYaw, float &pTime) const;
	void DispatchOnLoadMesh(UiCure::UserGeometryReferenceResource* pMeshResource);

	Game* mGame;
};



}
