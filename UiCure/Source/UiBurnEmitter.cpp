
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../Include/UiBurnEmitter.h"
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



BurnEmitter::BurnEmitter(Cure::ResourceManager* pResourceManager, GameUiManager* pUiManager):
	mResourceManager(pResourceManager),
	mUiManager(pUiManager),
	mBurnTimeout(0),
	mFreeFlow(false)
{
}

BurnEmitter::~BurnEmitter()
{
	mResourceManager = 0;
	mUiManager = 0;
}



void BurnEmitter::EmitFromTag(const CppContextObject* pObject, const UiTbc::ChunkyClass::Tag& pTag, float pFrameTime, float pIntensity)
{
	bool lParticlesEnabled;
	CURE_RTVAR_GET(lParticlesEnabled, =, UiCure::GetSettings(), RTVAR_UI_3D_ENABLEPARTICLES, false);
	if (!lParticlesEnabled || pIntensity <= 0)
	{
		return;
	}

	enum FloatValue
	{
		FV_X = 0,
		FV_Y,
		FV_Z,
		FV_VX,
		FV_VY,
		FV_VZ,
		FV_SCALE,
		FV_DENSITY,
		FV_OPACITY,
		FV_TTL,
		FV_COUNT
	};
	if (pTag.mFloatValueList.size() != FV_COUNT ||
		pTag.mStringValueList.size() != 0 ||
		pTag.mEngineIndexList.size() != 0 ||
		pTag.mBodyIndexList.size() != 0 ||
		pTag.mMeshIndexList.size() < 1)
	{
		mLog.Errorf(_T("The burn tag '%s' has the wrong # of parameters."), pTag.mTagName.c_str());
		deb_assert(false);
		return;
	}
	float lBurnIntensity;
	CURE_RTVAR_GET(lBurnIntensity, =(float), UiCure::GetSettings(), RTVAR_UI_3D_EXHAUSTINTENSITY, 1.0);
	mBurnTimeout -= pIntensity * lBurnIntensity * pFrameTime * 25;
	if (mBurnTimeout > 0)
	{
		return;
	}
	const float lDensity = pTag.mFloatValueList[FV_DENSITY];
	mBurnTimeout = 1/lDensity;

	const float lScale = pTag.mFloatValueList[FV_SCALE];
	const float lLifeTime = pTag.mFloatValueList[FV_TTL];

	const QuaternionF lOriginalOrientation = pObject->GetOrientation();
	Vector3DF lOffset(pTag.mFloatValueList[FV_X], pTag.mFloatValueList[FV_Y], pTag.mFloatValueList[FV_Z]);
	lOffset = lOriginalOrientation*lOffset;
	Vector3DF lVelocity(pTag.mFloatValueList[FV_VX], pTag.mFloatValueList[FV_VY], pTag.mFloatValueList[FV_VZ]);
	const float lOpacity = pTag.mFloatValueList[FV_OPACITY];
	if (!mFreeFlow)
	{
		lVelocity = lOriginalOrientation*lVelocity;
	}
	else
	{
		lVelocity.Set(0, 0, lVelocity.GetLength()*0.6f);
	}
	lVelocity += pObject->GetVelocity();
	UiTbc::ParticleRenderer* lParticleRenderer = (UiTbc::ParticleRenderer*)mUiManager->GetRenderer()->GetDynamicRenderer(_T("particle"));
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

			const float lAngularVelocity = Random::Uniform(-15.5f, +15.5f);
			lParticleRenderer->CreateFume(lLifeTime, lScale, lAngularVelocity, lOpacity, lTransform.GetPosition(), lVelocity);
		}
	}
}

void BurnEmitter::SetFreeFlow()
{
	mFreeFlow = true;
}



LOG_CLASS_DEFINE(GAME_CONTEXT, BurnEmitter);



}
