
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/UiJetEngineEmitter.h"
#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/GameManager.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Lepra/Include/Random.h"
#include "../../Tbc/Include/PhysicsEngine.h"
#include "../../UiCure/Include/UiGameUiManager.h"
#include "../../UiTbc/Include/UiParticleRenderer.h"
#include "../Include/UiProps.h"
#include "../Include/UiRuntimeVariableName.h"



namespace UiCure
{



JetEngineEmitter::JetEngineEmitter(Cure::ResourceManager* pResourceManager, GameUiManager* pUiManager):
	mResourceManager(pResourceManager),
	mUiManager(pUiManager),
	mInterleaveTimeout(0)
{
}

JetEngineEmitter::~JetEngineEmitter()
{
	mResourceManager = 0;
	mUiManager = 0;
}



void JetEngineEmitter::EmitFromTag(const CppContextObject* pObject, const UiTbc::ChunkyClass::Tag& pTag, float pFrameTime)
{
	bool lParticlesEnabled;
	v_get(lParticlesEnabled, =, UiCure::GetSettings(), RTVAR_UI_3D_ENABLEPARTICLES, false);
	if (!lParticlesEnabled)
	{
		return;
	}

	enum FloatValue
	{
		FV_START_R = 0,
		FV_START_G,
		FV_START_B,
		FV_END_R,
		FV_END_G,
		FV_END_B,
		FV_X,
		FV_Y,
		FV_Z,
		FV_RADIUS_X,
		FV_RADIUS_Y,
		FV_RADIUS_Z,
		FV_SCALE_X,
		FV_SCALE_Y,
		FV_SCALE_Z,
		FV_DIRECTION_X,
		FV_DIRECTION_Y,
		FV_DIRECTION_Z,
		FV_DENSITY,
		FV_OPACITY,
		FV_OVERSHOOT_OPACITY,
		FV_OVERSHOOT_CUTOFF_DOT,
		FV_OVERSHOOT_DISTANCE_UPSCALE,
		FV_OVERSHOOT_ENGINE_FACTOR_BASE,
		FV_COUNT
	};
	if (pTag.mFloatValueList.size() != FV_COUNT ||
		pTag.mStringValueList.size() != 0 ||
		pTag.mBodyIndexList.size() != 0 ||
		pTag.mEngineIndexList.size() != 1 ||
		pTag.mMeshIndexList.size() < 1)
	{
		mLog.Errorf(_T("The fire tag '%s' has the wrong # of parameters."), pTag.mTagName.c_str());
		deb_assert(false);
		return;
	}
	const int lEngineIndex = pTag.mEngineIndexList[0];
	if (lEngineIndex >= pObject->GetPhysics()->GetEngineCount())
	{
		return;
	}
	const Tbc::PhysicsEngine* lEngine = pObject->GetPhysics()->GetEngine(lEngineIndex);
	const float lThrottleUpSpeed = Math::GetIterateLerpTime(pTag.mFloatValueList[FV_OVERSHOOT_ENGINE_FACTOR_BASE]*0.5f, pFrameTime);
	const float lThrottleDownSpeed = Math::GetIterateLerpTime(pTag.mFloatValueList[FV_OVERSHOOT_ENGINE_FACTOR_BASE], pFrameTime);
	const float lEngineThrottle = lEngine->GetLerpThrottle(lThrottleUpSpeed, lThrottleDownSpeed, true);
	const quat lOrientation = pObject->GetOrientation();
	vec3 lRadius(pTag.mFloatValueList[FV_RADIUS_X], pTag.mFloatValueList[FV_RADIUS_Y], pTag.mFloatValueList[FV_RADIUS_Z]);
	lRadius.x *= Math::Lerp(1.0f, pTag.mFloatValueList[FV_SCALE_X], lEngineThrottle);
	lRadius.y *= Math::Lerp(1.0f, pTag.mFloatValueList[FV_SCALE_Y], lEngineThrottle);
	lRadius.z *= Math::Lerp(1.0f, pTag.mFloatValueList[FV_SCALE_Z], lEngineThrottle);
	vec3 lPosition(pTag.mFloatValueList[FV_X], pTag.mFloatValueList[FV_Y], pTag.mFloatValueList[FV_Z]);
	lPosition = lOrientation * lPosition;
	const vec3 lColor(pTag.mFloatValueList[FV_END_R], pTag.mFloatValueList[FV_END_B], pTag.mFloatValueList[FV_END_B]);

	bool lCreateParticle = false;
	const float lDensity = pTag.mFloatValueList[FV_DENSITY];
	float lExhaustIntensity;
	v_get(lExhaustIntensity, =(float), UiCure::GetSettings(), RTVAR_UI_3D_EXHAUSTINTENSITY, 1.0);
	mInterleaveTimeout -= Math::Lerp(0.3f, 1.0f, lEngineThrottle) * lExhaustIntensity * pFrameTime;
	if (mInterleaveTimeout <= 0)	// Release particle this frame?
	{
		lCreateParticle = true;
		mInterleaveTimeout = 0.05f / lDensity;
	}
	else
	{
		lCreateParticle = false;
	}

	const float dx = pTag.mFloatValueList[FV_RADIUS_X];
	const float dy = pTag.mFloatValueList[FV_RADIUS_Y];
	const float dz = pTag.mFloatValueList[FV_RADIUS_Z];
	const vec3 lStartColor(pTag.mFloatValueList[FV_START_R], pTag.mFloatValueList[FV_START_B], pTag.mFloatValueList[FV_START_B]);
	const float lOpacity = pTag.mFloatValueList[FV_OPACITY];
	const vec3 lDirection = lOrientation * vec3(pTag.mFloatValueList[FV_DIRECTION_X], pTag.mFloatValueList[FV_DIRECTION_Y], pTag.mFloatValueList[FV_DIRECTION_Z]);
	const vec3 lVelocity = lDirection + pObject->GetVelocity();
	UiTbc::ParticleRenderer* lParticleRenderer = (UiTbc::ParticleRenderer*)mUiManager->GetRenderer()->GetDynamicRenderer(_T("particle"));
	const float lParticleTime = lDensity;
	float lParticleSize;	// Pick second size.
	if (dx > dy && dy > dz)
	{
		lParticleSize = dy;
	}
	else if (dy > dx && dx > dz)
	{
		lParticleSize = dx;
	}
	else
	{
		lParticleSize = dz;
	}
	lParticleSize *= 0.2f;

	const float lDistanceScaleFactor = pTag.mFloatValueList[FV_OVERSHOOT_DISTANCE_UPSCALE];
	for (size_t y = 0; y < pTag.mMeshIndexList.size(); ++y)
	{
		Tbc::GeometryBase* lMesh = pObject->GetMesh(pTag.mMeshIndexList[y]);
		if (lMesh)
		{
			int lPhysIndex = -1;
			str lMeshName;
			xform lTransform;
			float lMeshScale;
			((UiTbc::ChunkyClass*)pObject->GetClass())->GetMesh(pTag.mMeshIndexList[y], lPhysIndex, lMeshName, lTransform, lMeshScale);
			lTransform = lMesh->GetBaseTransformation() * lTransform;
			vec3 lMeshPos = lTransform.GetPosition() + lPosition;

			const vec3 lCamDistance = lMeshPos - mUiManager->GetRenderer()->GetCameraTransformation().GetPosition();
			const float lDistance = lCamDistance.GetLength();
			const vec3 lCamDirection(lCamDistance / lDistance);
			float lOvershootFactor = -(lCamDirection*lDirection);
			if (lOvershootFactor > pTag.mFloatValueList[FV_OVERSHOOT_CUTOFF_DOT])
			{
				lOvershootFactor = Math::Lerp(lOvershootFactor*0.5f, lOvershootFactor, lEngineThrottle);
				const float lOpacity = (lOvershootFactor+0.6f) * pTag.mFloatValueList[FV_OVERSHOOT_OPACITY];
				DrawOvershoot(lMeshPos, lDistanceScaleFactor*lDistance, lRadius, lColor, lOpacity, lCamDirection);
			}

			if (lCreateParticle)
			{
				const float sx = Random::Normal(0.0f, dx*0.5f, -dx, +dx);
				const float sy = Random::Normal(0.0f, dy*0.5f, -dy, +dy);
				const float sz = Random::Normal(0.0f, dz*0.5f, -dz, +dz);
				lMeshPos += lOrientation * vec3(sx, sy, sz);
				lParticleRenderer->CreateGlow(lParticleTime, lParticleSize, lStartColor, lColor, lOpacity, lMeshPos, lVelocity);
			}
		}
	}
}



void JetEngineEmitter::DrawOvershoot(const vec3& pPosition, float pDistanceScaleFactor, const vec3& pRadius, const vec3& pColor, float pOpacity, const vec3& pCameraDirection)
{
	float s = std::max(std::max(pRadius.x, pRadius.y), pRadius.z);
	UiTbc::ParticleRenderer* lParticleRenderer = (UiTbc::ParticleRenderer*)mUiManager->GetRenderer()->GetDynamicRenderer(_T("particle"));
	const float lMaxFlameDistance = 3 * s;
	s += s * pDistanceScaleFactor * 0.1f;
	lParticleRenderer->RenderFireBillboard(0, s, pColor, pOpacity, pPosition-pCameraDirection*lMaxFlameDistance);
}



loginstance(GAME_CONTEXT, JetEngineEmitter);



}
