
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "../Include/UiJetEngineEmitter.h"
#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/GameManager.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Lepra/Include/Random.h"
#include "../../TBC/Include/PhysicsEngine.h"
#include "../../UiCure/Include/UiGameUiManager.h"
#include "../../UiTBC/Include/UiParticleRenderer.h"
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
	CURE_RTVAR_GET(lParticlesEnabled, =, UiCure::GetSettings(), RTVAR_UI_3D_ENABLEPARTICLES, false);
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
		FV_OVERSHOOT_CUTOFF_DOT,
		FV_ENGINE_FACTOR_BASE,
		FV_COUNT
	};
	if (pTag.mFloatValueList.size() != FV_COUNT ||
		pTag.mStringValueList.size() != 0 ||
		pTag.mBodyIndexList.size() != 0 ||
		pTag.mEngineIndexList.size() != 1 ||
		pTag.mMeshIndexList.size() < 1)
	{
		mLog.Errorf(_T("The fire tag '%s' has the wrong # of parameters."), pTag.mTagName.c_str());
		assert(false);
		return;
	}
	const int lEngineIndex = pTag.mEngineIndexList[0];
	if (lEngineIndex >= pObject->GetPhysics()->GetEngineCount())
	{
		return;
	}
	const TBC::PhysicsEngine* lEngine = pObject->GetPhysics()->GetEngine(lEngineIndex);
	const float lThrottleUpSpeed = Math::GetIterateLerpTime(pTag.mFloatValueList[FV_ENGINE_FACTOR_BASE]*0.5f, pFrameTime);
	const float lThrottleDownSpeed = Math::GetIterateLerpTime(pTag.mFloatValueList[FV_ENGINE_FACTOR_BASE], pFrameTime);
	const float lEngineThrottle = lEngine->GetLerpThrottle(lThrottleUpSpeed, lThrottleDownSpeed, true);
	const QuaternionF lOrientation = pObject->GetOrientation();
	Vector3DF lRadius(pTag.mFloatValueList[FV_RADIUS_X], pTag.mFloatValueList[FV_RADIUS_Y], pTag.mFloatValueList[FV_RADIUS_Z]);
	lRadius.x *= Math::Lerp(1.0f, pTag.mFloatValueList[FV_RADIUS_X], lEngineThrottle);
	lRadius.y *= Math::Lerp(1.0f, pTag.mFloatValueList[FV_RADIUS_Y], lEngineThrottle);
	lRadius.z *= Math::Lerp(1.0f, pTag.mFloatValueList[FV_RADIUS_Z], lEngineThrottle);
	Vector3DF lPosition(pTag.mFloatValueList[FV_X], pTag.mFloatValueList[FV_Y], pTag.mFloatValueList[FV_Z]);
	lPosition = lOrientation * lPosition + pObject->GetPosition();
	const Vector3DF lColor(pTag.mFloatValueList[FV_END_R], pTag.mFloatValueList[FV_END_B], pTag.mFloatValueList[FV_END_B]);

	Vector3DF lDirection = lOrientation * Vector3DF(pTag.mFloatValueList[FV_DIRECTION_X], pTag.mFloatValueList[FV_DIRECTION_Y], pTag.mFloatValueList[FV_DIRECTION_Z]);
	const Vector3DF lCamDirection = mUiManager->GetRenderer()->GetCameraTransformation().GetOrientation() * Vector3DF(0,1,0);
	float lOvershootFactor = -(lCamDirection*lDirection);
	if (lOvershootFactor > pTag.mFloatValueList[FV_OVERSHOOT_CUTOFF_DOT])
	{
		lOvershootFactor = Math::Lerp(lOvershootFactor*0.5f, lOvershootFactor, lEngineThrottle);
		DrawOvershoot(lPosition, lOrientation, lRadius, lColor, lOvershootFactor, lCamDirection);
	}

	/*float lExhaustIntensity;
	CURE_RTVAR_GET(lExhaustIntensity, =(float), UiCure::GetSettings(), RTVAR_UI_3D_EXHAUSTINTENSITY, 1.0);
	mInterleaveTimeout -= std::max(1.0f, lEngineThrottle) * lExhaustIntensity * pFrameTime;
	if (mInterleaveTimeout > 0)	// Release particle this frame?
	{
		return;
	}
	const float lDensity = pTag.mFloatValueList[FV_DENSITY];
	mInterleaveTimeout = 0.05f / lDensity;

	const float dx = pTag.mFloatValueList[FV_RADIUS_X];
	const float dy = pTag.mFloatValueList[FV_RADIUS_Y];
	const float dz = pTag.mFloatValueList[FV_RADIUS_Z];
	const float sx = Random::Normal(0.0f, pTag.mFloatValueList[FV_SCALE_X]*dx, -dx, +dx);
	const float sy = Random::Normal(0.0f, pTag.mFloatValueList[FV_SCALE_Y]*dy, -dy, +dy);
	const float sz = Random::Normal(0.0f, pTag.mFloatValueList[FV_SCALE_Z]*dz, -dz, +dz);
	lOffset += lOrientation * Vector3DF(sx, sy, sz);
	const Vector3DF lVelocity += pObject->GetVelocity();
	const Vector3DF lStartColor(pTag.mFloatValueList[FV_START_R], pTag.mFloatValueList[FV_START_B], pTag.mFloatValueList[FV_START_B]);
	const float lOpacity = pTag.mFloatValueList[FV_OPACITY];
	UiTbc::ParticleRenderer* lParticleRenderer = (UiTbc::ParticleRenderer*)mUiManager->GetRenderer()->GetDynamicRenderer(_T("particle"));
	const float lTime = 0.25f * lDensity;
	float lSize;	// Pick second size.
	if (dx > dy && dy > dz)
	{
		lSize = dy;
	}
	else if (dy > dx && dx > dz)
	{
		lSize = dx;
	}
	else
	{
		lSize = dz;
	}
	lSize *= 0.2f;
	for (size_t y = 0; y < pTag.mMeshIndexList.size(); ++y)
	{
		TBC::GeometryBase* lMesh = pObject->GetMesh(pTag.mMeshIndexList[y]);
		if (lMesh)
		{
			int lPhysIndex = -1;
			str lMeshName;
			TransformationF lTransform;
			((UiTbc::ChunkyClass*)pObject->GetClass())->GetMesh(pTag.mMeshIndexList[y], lPhysIndex, lMeshName, lTransform);
			lTransform = lMesh->GetBaseTransformation() * lTransform;
			lTransform.GetPosition() += lOffset;

			lParticleRenderer->CreateGlow(lTime, lSize, lStartColor, lColor, lOpacity, lTransform.GetPosition(), lVelocity);
		}
	}*/
}



void JetEngineEmitter::DrawOvershoot(const Vector3DF& pPosition, const QuaternionF& pOrientation, const Vector3DF& pRadius, const Vector3DF& pColor, float pOpacity, const Vector3DF& pCameraDirection)
{
	const QuaternionF& lCam = mUiManager->GetRenderer()->GetCameraTransformation().GetOrientation();
	const QuaternionF& lCamInverse = mUiManager->GetRenderer()->GetCameraOrientationInverse();
	Vector3DF x;
	Vector3DF y;
	Vector3DF z;
	lCam.FastInverseRotatedVector(lCamInverse, x, pOrientation*Vector3DF(pRadius.x, 0, 0));
	lCam.FastInverseRotatedVector(lCamInverse, y, pOrientation*Vector3DF(0, pRadius.y, 0));
	lCam.FastInverseRotatedVector(lCamInverse, z, pOrientation*Vector3DF(0, 0, pRadius.z));
	Vector2DF x2d(x.x, x.z);
	Vector2DF y2d(y.x, y.z);
	Vector2DF z2d(z.x, z.z);
	const float x2 = x.GetLengthSquared();
	const float y2 = y.GetLengthSquared();
	const float z2 = z.GetLengthSquared();
	float s;
	if (x2 > y2 && x2 > z2)
	{
		s = ::sqrt(x2);
		x2d /= s;
	}
	else if (y2 > z2)
	{
		s = ::sqrt(y2);
		x2d = y2d/s;
	}
	else
	{
		s = ::sqrt(z2);
		x2d = z2d/s;
	}
	UiTbc::ParticleRenderer* lParticleRenderer = (UiTbc::ParticleRenderer*)mUiManager->GetRenderer()->GetDynamicRenderer(_T("particle"));
	const float lMaxFlameDistance = std::max(std::max(pRadius.x, pRadius.y), pRadius.z);
	lParticleRenderer->RenderFireBillboard(0, s*s, pColor, pOpacity+0.6f, pPosition-(pCameraDirection*lMaxFlameDistance));
}



LOG_CLASS_DEFINE(GAME_CONTEXT, JetEngineEmitter);



}
