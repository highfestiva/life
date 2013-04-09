
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "../Include/UiFireEmitter.h"
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



FireEmitter::FireEmitter(Cure::ResourceManager* pResourceManager, GameUiManager* pUiManager):
	mResourceManager(pResourceManager),
	mUiManager(pUiManager),
	mInterleaveTimeout(0)
{
}

FireEmitter::~FireEmitter()
{
	mResourceManager = 0;
	mUiManager = 0;
}



void FireEmitter::EmitFromTag(const CppContextObject* pObject, const UiTbc::ChunkyClass::Tag& pTag, float pFrameTime)
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
		FV_SIZE_X,
		FV_SIZE_Y,
		FV_SIZE_Z,
		FV_GAUSS_X,
		FV_GAUSS_Y,
		FV_GAUSS_Z,
		FV_VX,
		FV_VY,
		FV_VZ,
		FV_DENSITY,
		FV_OPACITY,
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
	float lExhaustIntensity;
	CURE_RTVAR_GET(lExhaustIntensity, =(float), UiCure::GetSettings(), RTVAR_UI_3D_EXHAUSTINTENSITY, 1.0);
	const float lThrottleUpSpeed = Math::GetIterateLerpTime(pTag.mFloatValueList[FV_ENGINE_FACTOR_BASE]*0.5f, pFrameTime);
	const float lThrottleDownSpeed = Math::GetIterateLerpTime(pTag.mFloatValueList[FV_ENGINE_FACTOR_BASE], pFrameTime);
	const float lEngineThrottle = lEngine->GetLerpThrottle(lThrottleUpSpeed, lThrottleDownSpeed, true);
	mInterleaveTimeout -= std::max(0.15f, lEngineThrottle) * lExhaustIntensity * pFrameTime;
	if (mInterleaveTimeout > 0)
	{
		return;
	}
	const float lDensity = pTag.mFloatValueList[FV_DENSITY];
	mInterleaveTimeout = 0.05f / lDensity;

	const QuaternionF lOriginalOrientation = pObject->GetOrientation();
	Vector3DF lOffset(pTag.mFloatValueList[FV_X], pTag.mFloatValueList[FV_Y], pTag.mFloatValueList[FV_Z]);
	lOffset = lOriginalOrientation * lOffset;
	const float dx = pTag.mFloatValueList[FV_SIZE_X];
	const float dy = pTag.mFloatValueList[FV_SIZE_Y];
	const float dz = pTag.mFloatValueList[FV_SIZE_Z];
	const float sx = Random::Normal(0.0f, pTag.mFloatValueList[FV_GAUSS_X]*dx, -dx, +dx);
	const float sy = Random::Normal(0.0f, pTag.mFloatValueList[FV_GAUSS_Y]*dy, -dy, +dy);
	const float sz = Random::Normal(0.0f, pTag.mFloatValueList[FV_GAUSS_Z]*dz, -dz, +dz);
	lOffset += lOriginalOrientation * Vector3DF(sx, sy, sz);
	Vector3DF lVelocity(pTag.mFloatValueList[FV_VX], pTag.mFloatValueList[FV_VY], pTag.mFloatValueList[FV_VZ]);
	lVelocity = lOriginalOrientation*lVelocity;
	lVelocity += pObject->GetVelocity();
	const Vector3DF lStartColor(pTag.mFloatValueList[FV_START_R], pTag.mFloatValueList[FV_START_B], pTag.mFloatValueList[FV_START_B]);
	const Vector3DF lColor(pTag.mFloatValueList[FV_END_R], pTag.mFloatValueList[FV_END_B], pTag.mFloatValueList[FV_END_B]);
	const float lOpacity = pTag.mFloatValueList[FV_OPACITY];
	UiTbc::ParticleRenderer* lParticleRenderer = (UiTbc::ParticleRenderer*)mUiManager->GetRenderer()->GetDynamicRenderer(_T("particle"));
	const float lTime = 0.5f * lDensity;
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
	lSize *= 0.1f;
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
	}
}



LOG_CLASS_DEFINE(GAME_CONTEXT, FireEmitter);



}
