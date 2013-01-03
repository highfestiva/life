
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "../Include/UiExhaustEmitter.h"
#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/GameManager.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../TBC/Include/PhysicsEngine.h"
#include "../Include/UiProps.h"
#include "../Include/UiRuntimeVariableName.h"



namespace UiCure
{



ExhaustEmitter::ExhaustEmitter(Cure::ResourceManager* pResourceManager, GameUiManager* pUiManager, const str& pParticleClass, float pScale, float pAmount, float pLifeTime):
	mResourceManager(pResourceManager),
	mUiManager(pUiManager),
	mParticleClass(pParticleClass),
	mScale(pScale),
	mDelay(1/pAmount),
	mLifeTime(pLifeTime)
{
}

ExhaustEmitter::~ExhaustEmitter()
{
	mResourceManager = 0;
	mUiManager = 0;
}



void ExhaustEmitter::EmitFromTag(const CppContextObject* pObject, const UiTbc::ChunkyClass::Tag& pTag, float pFrameTime)
{
	bool lParticlesEnabled;
	CURE_RTVAR_GET(lParticlesEnabled, =, UiCure::GetSettings(), RTVAR_UI_3D_ENABLEPARTICLES, false);
	if (!lParticlesEnabled)
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
		FV_DENSITY,
		FV_OPACITY,
		FV_COUNT
	};
	if (pTag.mFloatValueList.size() != FV_COUNT ||
		pTag.mStringValueList.size() != 0 ||
		pTag.mEngineIndexList.size() != 1 ||
		pTag.mBodyIndexList.size() != 0 ||
		pTag.mMeshIndexList.size() < 1)
	{
		mLog.Errorf(_T("The exhaust tag '%s' has the wrong # of parameters."), pTag.mTagName.c_str());
		assert(false);
		return;
	}
	const int lEngineIndex = pTag.mEngineIndexList[0];
	if (lEngineIndex >= pObject->GetPhysics()->GetEngineCount())
	{
		return;
	}
	const TBC::PhysicsEngine* lEngine = pObject->GetPhysics()->GetEngine(lEngineIndex);
	const float lDensity = pTag.mFloatValueList[FV_DENSITY];
	float lExhaustIntensity;
	CURE_RTVAR_GET(lExhaustIntensity, =(float), UiCure::GetSettings(), RTVAR_UI_3D_EXHAUSTINTENSITY, 1.0);
	mExhaustTimeout -= std::max(0.15f, lEngine->GetIntensity() * lDensity) * lExhaustIntensity * pFrameTime * 25;
	if (mExhaustTimeout > 0)
	{
		return;
	}
	mExhaustTimeout = mDelay;

	const QuaternionF lOriginalOrientation = pObject->GetOrientation();
	Vector3DF lOffset(pTag.mFloatValueList[FV_X], pTag.mFloatValueList[FV_Y], pTag.mFloatValueList[FV_Z]);
	lOffset = lOriginalOrientation*lOffset;
	Vector3DF lVelocity(pTag.mFloatValueList[FV_VX], pTag.mFloatValueList[FV_VY], pTag.mFloatValueList[FV_VZ]);
	const float lOpacity = pTag.mFloatValueList[FV_OPACITY];
	lVelocity = lOriginalOrientation*lVelocity;
	lVelocity += pObject->GetVelocity()*0.5f;
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
			Props* lPuff = new Props(mResourceManager, mParticleClass, mUiManager);
			pObject->GetManager()->GetGameManager()->AddContextObject(lPuff, Cure::NETWORK_OBJECT_LOCAL_ONLY, 0);
			//mLog.Infof(_T("ExhaustEmitter %i creates fume particle %i."), GetInstanceId(), lPuff->GetInstanceId());
			lPuff->DisableRootShadow();
			lPuff->SetInitialTransform(lTransform);
			lPuff->SetOpacity(lOpacity);
			lPuff->StartParticle(Props::PARTICLE_GAS, lVelocity, mScale, 0.5f, mLifeTime);
			lPuff->StartLoading();
		}
	}
}



LOG_CLASS_DEFINE(GAME_CONTEXT, ExhaustEmitter);



}
