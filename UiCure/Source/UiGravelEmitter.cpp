
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/UiGravelEmitter.h"
#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/GameManager.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Lepra/Include/Random.h"
#include "../../Tbc/Include/ChunkyBoneGeometry.h"
#include "../../Tbc/Include/ChunkyPhysics.h"
#include "../../UiTbc/Include/UiParticleRenderer.h"
#include "../Include/UiGameUiManager.h"
#include "../Include/UiProps.h"
#include "../Include/UiRuntimeVariableName.h"



namespace UiCure
{



GravelEmitter::GravelEmitter(Cure::ResourceManager* pResourceManager, GameUiManager* pUiManager, float pSensitivity, float pScale, float pAmount, float pLifeTime):
	mResourceManager(pResourceManager),
	mUiManager(pUiManager),
	mSensitivityFactor(1/pSensitivity),
	mScale(pScale),
	mDelay(1/pAmount),
	mLifeTime(pLifeTime)
{
}

GravelEmitter::~GravelEmitter()
{
	mResourceManager = 0;
	mUiManager = 0;
}



void GravelEmitter::OnForceApplied(Cure::ContextObject* pObject, Cure::ContextObject* pOtherObject,
	Tbc::PhysicsManager::BodyID pOwnBodyId, Tbc::PhysicsManager::BodyID pOtherBodyId,
	const vec3& pForce, const vec3& pTorque,
	const vec3& pPosition, const vec3& pRelativeVelocity)
{
	bool lParticlesEnabled;
	v_get(lParticlesEnabled, =, UiCure::GetSettings(), RTVAR_UI_3D_ENABLEPARTICLES, false);
	if (!lParticlesEnabled)
	{
		return;
	}
	if (mParticleTimer.QueryTimeDiff() < mDelay)
	{
		return;
	}
	const float lRelativeSpeedLimit = 2 * mSensitivityFactor;
	if (pRelativeVelocity.GetLengthSquared() < lRelativeSpeedLimit*lRelativeSpeedLimit)
	{
		return;
	}
	if (!pOtherObject)
	{
		return;
	}
	if (pObject->GetPhysics()->GetBoneGeometry(pOwnBodyId)->GetMaterial() != _T("grass"))
	{
		return;
	}
	const float lDistance = 100;	// Only show gravel particles inside this distance.
	if (!pObject->GetManager()->GetGameManager()->IsObjectRelevant(pPosition, lDistance))
	{
		return;
	}
	const float lImpactFactor = pOtherObject->GetPhysics()->GetBoneGeometry(pOtherBodyId)->GetImpactFactor();
	const float lImpact = pOtherObject->GetImpact(pOtherObject->GetManager()->GetGameManager()->GetPhysicsManager()->GetGravity(),
		pForce, pTorque, 0, 11) * lImpactFactor;
	if (lImpact < 0.1f*mSensitivityFactor)
	{
		return;
	}
	vec3 lPosition(pPosition);
	const float lAngle = Random::Uniform(0.0f, PIF*2);
	lPosition.x += 0.2f * cos(lAngle);
	lPosition.y += 0.2f * sin(lAngle);
	lPosition.z += Random::Uniform(+0.1f, +0.2f);
	vec3 lRelativeVelocity(pRelativeVelocity);
	const vec3 lUp(0, 0, 1);
	vec3 lTorque(pTorque.Cross(lUp));
	const float lMassFactor = 1/pOtherObject->GetMass();
	lRelativeVelocity += lTorque * lMassFactor * 0.1f;
	vec3 lRotationSpeed;
	pOtherObject->GetManager()->GetGameManager()->GetPhysicsManager()->GetBodyAngularVelocity(pOtherBodyId, lRotationSpeed);
	const vec3 lRadius = pPosition - pOtherObject->GetManager()->GetGameManager()->GetPhysicsManager()->GetBodyPosition(pOtherBodyId);
	const vec3 lRollSpeed(lRotationSpeed.Cross(lRadius) * 0.2f);
	lPosition += lRollSpeed.GetNormalized(0.3f);
	const float lRollLength = lRollSpeed.GetLength();
	const float lCollisionLength = lRelativeVelocity.GetLength();
	lRelativeVelocity += lRollSpeed;
	lRelativeVelocity.z += lCollisionLength*0.2f + lRollLength*0.2f;
	lRelativeVelocity += RNDVEC(lCollisionLength*0.2f);
	if (lRelativeVelocity.GetLengthSquared() < pRelativeVelocity.GetLengthSquared()*200*200)
	{
		UiTbc::ParticleRenderer* lParticleRenderer = (UiTbc::ParticleRenderer*)mUiManager->GetRenderer()->GetDynamicRenderer(_T("particle"));
		const float lAngularVelocity = Random::Uniform(-5.0f, 5.0f);
		lParticleRenderer->CreatePebble(mLifeTime, mScale, lAngularVelocity, vec3(0.3f, 0.15f, 0.0f), lPosition, lRelativeVelocity);
		mParticleTimer.ClearTimeDiff();
	}
}



loginstance(GAME_CONTEXT, GravelEmitter);



}
