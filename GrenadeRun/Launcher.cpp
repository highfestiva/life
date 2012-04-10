
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "Launcher.h"
#include "../TBC/Include/PhysicsEngine.h"
#include "../UiCure/Include/UiGameUiManager.h"



namespace GrenadeRun
{



Launcher::Launcher(Game* pGame):
	Parent(pGame->GetResourceManager(), _T("Launcher"), pGame->GetUiManager()),
	mGame(pGame)
{
	SetForceLoadUnique(true);
}

Launcher::~Launcher()
{
}



void Launcher::SetBarrelAngle(float pYaw, float pPitch)
{
	QuaternionF lQuaternion;
	lQuaternion.RotateAroundWorldZ(pYaw);
	lQuaternion.RotateAroundOwnX(pPitch);
	SetRootOrientation(lQuaternion);

	// Keep supportive thingie on ground.
	TBC::GeometryReference* lMesh = (TBC::GeometryReference*)GetMesh(1);
	static bool lHasStoredTransformation = false;
	static QuaternionF lOriginalOrientation;
	if (!lHasStoredTransformation)
	{
		lOriginalOrientation = lMesh->GetOffsetTransformation().GetOrientation();
		lHasStoredTransformation = true;
	}
	lQuaternion = lOriginalOrientation;
	lQuaternion.RotateAroundOwnX(-pPitch);
	TransformationF lTransform = lMesh->GetOffsetTransformation();
	lTransform.SetOrientation(lQuaternion);
	lMesh->SetOffsetTransformation(lTransform);
}



void Launcher::GetAngles(const Cure::ContextObject* pTarget, float& pPitch, float& pGuidePitch,
	float& pYaw, float& pGuideYaw) const
{
	GetAngles(pTarget->GetPosition(), pTarget->GetVelocity(), pPitch, pGuidePitch, pYaw, pGuideYaw);
}

void Launcher::GetAngles(const Vector3DF& pTargetPosition, const Vector3DF& pTargetVelocity,
	float& pPitch, float& pGuidePitch, float& pYaw, float& pGuideYaw) const
{
	// GetBallisticData calculates the trajectory by polynome approximation (don't remember
	// the math any more), but calling it twice gets us pretty close to the sweet spot.
	Vector3DF lPosition1 = pTargetPosition;
	const Vector3DF lPosition2 = this->GetPosition();
	float lRoll;
	this->GetOrientation().GetEulerAngles(pYaw, pPitch, lRoll);
	float lTime = 10.0f;
	GetBallisticData(lPosition1, lPosition2, pPitch, pGuidePitch, pGuideYaw, lTime);
	float lBetterPitch = pGuidePitch;
	{
		Vector3DF p = lPosition1 + pTargetVelocity * lTime;
		GetBallisticData(p, lPosition2, lBetterPitch, pGuidePitch, pGuideYaw, lTime);
	}
	lBetterPitch = pGuidePitch;
	lPosition1 = lPosition1 + pTargetVelocity * lTime;
	GetBallisticData(lPosition1, lPosition2, lBetterPitch, pGuidePitch, pGuideYaw, lTime);
	pGuidePitch = Math::Clamp(pGuidePitch, -PIF/2, 0.0f);
	pGuideYaw = Math::Clamp(pGuideYaw, -PIF/2, PIF/2);
}

void Launcher::CreateEngines()
{
	assert(GetPhysics()->GetEngineCount() == 0);
	TBC::PhysicsEngine* lPitchEngine = new TBC::PhysicsEngine(TBC::PhysicsEngine::ENGINE_TILTER, 1, 1, 1, 1, 0);
	GetPhysics()->AddEngine(lPitchEngine);
	TBC::PhysicsEngine* lYawEngine = new TBC::PhysicsEngine(TBC::PhysicsEngine::ENGINE_HINGE_ROLL, 1, 1, 1, 1, 1);
	GetPhysics()->AddEngine(lYawEngine);
}

void Launcher::GetBallisticData(const Vector3DF& pPosition1, const Vector3DF& pPosition2,
	float pPitch, float& pGuidePitch, float& pGuideYaw, float &pTime) const
{
	const Vector3DF lDelta = pPosition1 - pPosition2;
	const Vector2DF lYawVector(lDelta.x, lDelta.y);
	pGuideYaw = Vector2DF(0, 1).GetAngle(lYawVector);

	const float h = lDelta.z;
	const float v = mGame->GetMuzzleVelocity();
	const float vup = v * ::cos(pPitch);
	// g*t^2/2 - vup*t + h = 0
	//
	// Quaderatic formula:
	// ax^2 + bx + c = 0
	// =>
	//     -b +- sqrt(b^2 - 4ac)
	// x = ---------------------
	//             2a
	const float a = +9.82f/2;
	const float b = -vup;
	const float c = +h;
	const float b2 = b*b;
	const float _4ac = 4*a*c;
	if (b2 < _4ac)	// Does not compute.
	{
		pGuidePitch = -PIF/4;
	}
	else
	{
		const float t = (-b + sqrt(b2 - _4ac)) / (2*a);
		//assert(t > 0);
		pTime = t;
		const float vfwd = lYawVector.GetLength() / t;
		pGuidePitch = -::atan(vfwd/vup);
		if (pGuidePitch < pPitch)	// Aiming downwards?
		{
			pGuidePitch += (pGuidePitch-pPitch);	// Tss! Homebrew... seems to be working somewhat! :)
		}
	}
}



void Launcher::DispatchOnLoadMesh(UiCure::UserGeometryReferenceResource* pMeshResource)
{
	mUiManager->GetRenderer()->SetShadows(pMeshResource->GetData(), UiTbc::Renderer::FORCE_NO_SHADOWS);
	Parent::DispatchOnLoadMesh(pMeshResource);
}



}
