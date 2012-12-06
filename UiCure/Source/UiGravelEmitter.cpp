
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "../Include/UiGravelEmitter.h"
#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/GameManager.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Lepra/Include/Random.h"
#include "../../TBC/Include/ChunkyBoneGeometry.h"
#include "../../TBC/Include/ChunkyPhysics.h"
#include "../Include/UiGameUiManager.h"
#include "../Include/UiProps.h"
#include "../Include/UiRuntimeVariableName.h"



namespace UiCure
{



GravelEmitter::GravelEmitter(Cure::ResourceManager* pResourceManager, GameUiManager* pUiManager, const str& pParticleClass, float pSensitivity, float pScale, float pAmount, float pLifeTime):
	mResourceManager(pResourceManager),
	mUiManager(pUiManager),
	mParticleClass(pParticleClass),
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
	TBC::PhysicsManager::BodyID pOwnBodyId, TBC::PhysicsManager::BodyID pOtherBodyId,
	const Vector3DF& pForce, const Vector3DF& pTorque,
	const Vector3DF& pPosition, const Vector3DF& pRelativeVelocity)
{
	bool lParticlesEnabled;
	CURE_RTVAR_GET(lParticlesEnabled, =, UiCure::GetSettings(), RTVAR_UI_3D_ENABLEPARTICLES, false);
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
	Vector3DF lPosition(pPosition);
	const float lAngle = (float)Random::Uniform(0, PI*2);
	lPosition.x += 0.2f * cos(lAngle);
	lPosition.y += 0.2f * sin(lAngle);
	lPosition.z += (float)Random::Uniform(+0.1f, +0.2f);
	Vector3DF lRelativeVelocity(pRelativeVelocity);
	const Vector3DF lUp(0, 0, 1);
	Vector3DF lTorque(pTorque.Cross(lUp));
	const float lMassFactor = 1/pOtherObject->GetMass();
	lRelativeVelocity += lTorque * lMassFactor * 0.1f;
	Vector3DF lRotationSpeed;
	pOtherObject->GetManager()->GetGameManager()->GetPhysicsManager()->GetBodyAngularVelocity(pOtherBodyId, lRotationSpeed);
	const Vector3DF lRadius = pPosition - pOtherObject->GetManager()->GetGameManager()->GetPhysicsManager()->GetBodyPosition(pOtherBodyId);
	const Vector3DF lRollSpeed(lRotationSpeed.Cross(lRadius) * 0.2f);
	lPosition += lRollSpeed.GetNormalized(0.3f);
	const float lRollLength = lRollSpeed.GetLength();
	const float lCollisionLength = lRelativeVelocity.GetLength();
	lRelativeVelocity += lRollSpeed;
	lRelativeVelocity.z += lCollisionLength*0.2f + lRollLength*0.3f;
	lRelativeVelocity.x += (float)Random::Uniform(-lCollisionLength*0.05f, +lCollisionLength*0.05f);
	lRelativeVelocity.y += (float)Random::Uniform(-lCollisionLength*0.05f, +lCollisionLength*0.05f);
	lRelativeVelocity.z += (float)Random::Uniform(-lCollisionLength*0.02f, +lCollisionLength*0.05f);
	bool lEnableGravelFading;
	CURE_RTVAR_GET(lEnableGravelFading, =, UiCure::GetSettings(), RTVAR_UI_3D_ENABLEGRAVELFADING, false);
	if (lRelativeVelocity.GetLengthSquared() < pRelativeVelocity.GetLengthSquared()*200*200)
	{
		Props* lPuff = new Props(mResourceManager, mParticleClass, mUiManager);
		pObject->GetManager()->GetGameManager()->AddContextObject(lPuff, Cure::NETWORK_OBJECT_LOCAL_ONLY, 0);
		lPuff->SetInitialTransform(TransformationF(gIdentityQuaternionF, lPosition));
		lPuff->StartParticle(Props::PARTICLE_SOLID, lRelativeVelocity, mScale, 2, mLifeTime);
		if (!lEnableGravelFading)
		{
			lPuff->SetFadeOutTime(-10.0f);
		}
		lPuff->StartLoading();
		mParticleTimer.ClearTimeDiff();
	}
}



LOG_CLASS_DEFINE(GAME_CONTEXT, GravelEmitter);



}
