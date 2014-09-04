
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



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
	void GetAngles(const vec3& pTargetPosition, const vec3& pTargetVelocity,
		float& pPitch, float& pGuidePitch, float& pYaw, float& pGuideYaw) const;

	void CreateEngines();

private:
	void GetBallisticData(const vec3& pPosition1, const vec3& pPosition2,
		float pPitch, float& pGuidePitch, float& pGuideYaw, float &pTime) const;
	void DispatchOnLoadMesh(UiCure::UserGeometryReferenceResource* pMeshResource);

	Game* mGame;
};



}
