
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "../Include/UiMachine.h"
#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/GameManager.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Cure/Include/TimeManager.h"
#include "../../Lepra/Include/HashUtil.h"
#include "../../Lepra/Include/Random.h"
#include "../../TBC/Include/ChunkyBoneGeometry.h"
#include "../../TBC/Include/ChunkyPhysics.h"
#include "../../TBC/Include/PhysicsEngine.h"
#include "../Include/UiGameUiManager.h"
#include "../Include/UiExhaustEmitter.h"
#include "../Include/UiProps.h"
#include "../Include/UiRuntimeVariableName.h"



namespace UiCure
{



Machine::Machine(Cure::ResourceManager* pResourceManager, const str& pClassId, GameUiManager* pUiManager):
	Parent(pResourceManager, pClassId, pUiManager),
	mExhaustEmitter(0)
{
	EnableMeshSlide(true);
}

Machine::~Machine()
{
	DeleteEngineSounds();
}

void Machine::SetExhaustEmitter(ExhaustEmitter* pEmitter)
{
	mExhaustEmitter = pEmitter;
}

void Machine::DeleteEngineSounds()
{
	TagSoundTable::iterator x = mEngineSoundTable.begin();
	for (; x != mEngineSoundTable.end(); ++x)
	{
		delete (x->second);
	}
	mEngineSoundTable.clear();
}



void Machine::OnTick()
{
	Parent::OnTick();

	const TBC::ChunkyPhysics* lPhysics = GetPhysics();
	const TBC::ChunkyClass* lClass = GetClass();
	if (!lPhysics || !lClass)
	{
		return;
	}
	const Cure::TimeManager* lTimeManager = GetManager()->GetGameManager()->GetTimeManager();
	const float lFrameTime = std::min(0.1f, lTimeManager->GetNormalFrameTime());
	const bool lIsChild = IsAttributeTrue(_T("float_childishness"));
	float lRealTimeRatio;
	CURE_RTVAR_GET(lRealTimeRatio, =(float), Cure::GetSettings(), RTVAR_PHYSICS_RTR, 1.0);
	const TBC::PhysicsManager* lPhysicsManager = mManager->GetGameManager()->GetPhysicsManager();
	Vector3DF lVelocity;
	TBC::PhysicsManager::BodyID lBodyId = lPhysics->GetBoneGeometry(lPhysics->GetRootBone())->GetBodyId();
	if (lBodyId != TBC::INVALID_BODY)
	{
		lPhysicsManager->GetBodyVelocity(lBodyId, lVelocity);
	}
	size_t lEngineSoundIndex = 0;
	for (size_t x = 0; x < lClass->GetTagCount(); ++x)
	{
		const UiTbc::ChunkyClass::Tag& lTag = lClass->GetTag(x);
		if (lTag.mTagName == _T("eye"))
		{
			HandleTagEye(lTag, lPhysicsManager, lIsChild);
		}
		else if (lTag.mTagName == _T("brake_light"))
		{
			HandleTagBrakeLight(lTag);
		}
		else if (lTag.mTagName == _T("engine_sound"))
		{
			HandleTagEngineSound(lTag, lPhysicsManager, lVelocity, lFrameTime, lRealTimeRatio, lEngineSoundIndex);
		}
		else if (lTag.mTagName == _T("engine_mesh_offset"))
		{
			HandleTagEngineMeshOffset(lTag, lFrameTime, lRealTimeRatio);
		}
		else if (lTag.mTagName == _T("exhaust"))
		{
			// Particles coming out of exhaust.
			if (GetManager()->GetGameManager()->IsUiMoveForbidden(GetInstanceId()))
			{
				continue;
			}

			if (mExhaustEmitter)
			{
				mExhaustEmitter->EmitFromTag(this, lTag, lFrameTime);
			}
		}
	}

	if (lIsChild || lPhysics->GetGuideMode() == TBC::ChunkyPhysics::GUIDE_ALWAYS)
	{
		StabilizeTick();
	}
}



void Machine::HandleTagEye(const UiTbc::ChunkyClass::Tag& pTag, const TBC::PhysicsManager* pPhysicsManager, bool pIsChild)
{
	// Eyes follow steered wheels. Get wheel corresponding to eye and
	// move eye accordingly á là Lightning McQueen.

	if (GetManager()->GetGameManager()->IsUiMoveForbidden(GetInstanceId()))
	{
		return;
	}
	if (pTag.mFloatValueList.size() != 1 ||
		pTag.mStringValueList.size() != 0 ||
		pTag.mBodyIndexList.size()+pTag.mEngineIndexList.size() != 1 ||
		pTag.mMeshIndexList.size() < 1)
	{
		mLog.Errorf(_T("The eye tag '%s' has the wrong # of parameters."), pTag.mTagName.c_str());
		assert(false);
		return;
	}

	float lJointValue = 0;
	if (!pTag.mBodyIndexList.empty())
	{
		const int lBodyIndex = pTag.mBodyIndexList[0];
		TBC::ChunkyBoneGeometry* lBone = GetPhysics()->GetBoneGeometry(lBodyIndex);
		TBC::PhysicsManager::JointID lJoint = lBone->GetJointId();
		switch (lBone->GetJointType())
		{
			case TBC::ChunkyBoneGeometry::JOINT_HINGE2:
			{
				TBC::PhysicsManager::Joint3Diff lDiff;
				pPhysicsManager->GetJoint3Diff(lBone->GetBodyId(), lJoint, lDiff);
				float lLowStop = 0;
				float lHighStop = 0;
				float lBounce = 0;
				pPhysicsManager->GetJointParams(lJoint, lLowStop, lHighStop, lBounce);
				lJointValue = lDiff.mAngle1 * 2 / (lHighStop-lLowStop);
			}
			break;
			case TBC::ChunkyBoneGeometry::JOINT_HINGE:
			{
				TBC::PhysicsManager::Joint1Diff lDiff;
				pPhysicsManager->GetJoint1Diff(lBone->GetBodyId(), lJoint, lDiff);
				float lLowStop = 0;
				float lHighStop = 0;
				float lBounce = 0;
				pPhysicsManager->GetJointParams(lJoint, lLowStop, lHighStop, lBounce);
				lJointValue = lDiff.mValue * 2 / (lHighStop-lLowStop);
			}
			break;
			case TBC::ChunkyBoneGeometry::JOINT_EXCLUDE:
			{
				// Simple, dead eyes.
				lJointValue = 0;
			}
			break;
			default:
			{
				mLog.Errorf(_T("Joint type %i not implemented for tag type %s."), lBone->GetJointType(), pTag.mTagName.c_str());
				assert(false);
			}
			break;
		}
	}
	else
	{
		const int lEngineIndex = pTag.mEngineIndexList[0];
		if (lEngineIndex >= mPhysics->GetEngineCount())
		{
			return;
		}
		TBC::PhysicsEngine* lEngine = GetPhysics()->GetEngine(lEngineIndex);
		lJointValue = lEngine->GetLerpThrottle(0.1f, 0.1f, false);
	}
	const float lScale = pTag.mFloatValueList[0];
	const float lJointRightValue = lJointValue * lScale;
	const float lJointDownValue = (::cos(lJointValue)-1) * lScale * 0.5f;
	for (size_t y = 0; y < pTag.mMeshIndexList.size(); ++y)
	{
		TBC::GeometryBase* lMesh = GetMesh(pTag.mMeshIndexList[y]);
		if (lMesh)
		{
			TransformationF lTransform = lMesh->GetBaseTransformation();
			lTransform.MoveRight(lJointRightValue);
			lTransform.MoveBackward(lJointDownValue);
			lMesh->SetTransformation(lTransform);
			lMesh->SetTransformationChanged(true);
			lMesh->SetAlwaysVisible(pIsChild);
		}
	}
}

void Machine::HandleTagBrakeLight(const UiTbc::ChunkyClass::Tag& pTag)
{
	if (GetManager()->GetGameManager()->IsUiMoveForbidden(GetInstanceId()))
	{
		return;
	}
	if (pTag.mFloatValueList.size() != 3 ||
		pTag.mStringValueList.size() != 0 ||
		pTag.mBodyIndexList.size() != 0 ||
		pTag.mEngineIndexList.size() != 1 ||
		pTag.mMeshIndexList.size() < 1)
	{
		mLog.Errorf(_T("The brake_light tag '%s' has the wrong # of parameters."), pTag.mTagName.c_str());
		assert(false);
		return;
	}
	const int lEngineIndex = pTag.mEngineIndexList[0];
	if (lEngineIndex >= mPhysics->GetEngineCount())
	{
		return;
	}
	const TBC::PhysicsEngine* lEngine = mPhysics->GetEngine(lEngineIndex);
	for (size_t y = 0; y < pTag.mMeshIndexList.size(); ++y)
	{
		TBC::GeometryBase* lMesh = GetMesh(pTag.mMeshIndexList[y]);
		if (lMesh)
		{
			Vector3DF& lAmbient = lMesh->GetBasicMaterialSettings().mAmbient;
			if (::fabs(lEngine->GetValue()) > 0.001f)
			{
				lAmbient.Set(pTag.mFloatValueList[0], pTag.mFloatValueList[1], pTag.mFloatValueList[2]);
			}
			else
			{
				lAmbient.Set(0, 0, 0);
			}
		}
	}
}

void Machine::HandleTagEngineSound(const UiTbc::ChunkyClass::Tag& pTag, const TBC::PhysicsManager* pPhysicsManager, const Vector3DF& pVelocity,
	float pFrameTime, float pRealTimeRatio, size_t& pEngineSoundIndex)
{
	// Sound controlled by engine.

	if (pTag.mFloatValueList.size() != 1+9+pTag.mEngineIndexList.size() ||
		pTag.mStringValueList.size() != 1 ||
		pTag.mBodyIndexList.size() != 1 ||
		pTag.mEngineIndexList.size() < 1 ||
		pTag.mMeshIndexList.size() != 0)
	{
		mLog.Errorf(_T("The engine_sound tag '%s' has the wrong # of parameters."), pTag.mTagName.c_str());
		assert(false);
		return;
	}
	UserSound3dResource* lEngineSound = HashUtil::FindMapObject(mEngineSoundTable, &pTag);
	if (lEngineSound == 0)
	{
		const str lSoundName = pTag.mStringValueList[0];
		lEngineSound = new UserSound3dResource(GetUiManager(), UiLepra::SoundManager::LOOP_FORWARD);
		mEngineSoundTable.insert(TagSoundTable::value_type(&pTag, lEngineSound));
		lEngineSound->Load(GetResourceManager(), lSoundName,
			UserSound3dResource::TypeLoadCallback(this, &Machine::LoadPlaySound3d));
	}
	if (lEngineSound->GetLoadState() != Cure::RESOURCE_LOAD_COMPLETE)
	{
		return;
	}

	int lBodyIndex = pTag.mBodyIndexList[0];
	TBC::ChunkyBoneGeometry* lBone = GetPhysics()->GetBoneGeometry(lBodyIndex);
	const Vector3DF lPosition = pPhysicsManager->GetBodyPosition(lBone->GetBodyId());

	enum FloatValue
	{
		FV_THROTTLE_FACTOR = 0,
		FV_PITCH_LOW,
		FV_PITCH_HIGH,
		FV_PITCH_EXPONENT,
		FV_VOLUME_LOW,
		FV_VOLUME_HIGH,
		FV_VOLUME_EXPONENT,
		FV_INTENSITY_LOW,
		FV_INTENSITY_HIGH,
		FV_INTENSITY_EXPONENT,
		FV_ENGINE_FACTOR_BASE,
	};
	const float lThrottleUpSpeed = Math::GetIterateLerpTime(0.05f, pFrameTime);
	const float lThrottleDownSpeed = Math::GetIterateLerpTime(0.02f, pFrameTime);
	float lIntensity = 0;
	float lLowVolume = pTag.mFloatValueList[FV_VOLUME_LOW];
	for (size_t y = 0; y < pTag.mEngineIndexList.size(); ++y)
	{
		const int lEngineIndex = pTag.mEngineIndexList[y];
		if (lEngineIndex >= mPhysics->GetEngineCount())
		{
			continue;
		}
		const TBC::PhysicsEngine* lEngine = mPhysics->GetEngine(lEngineIndex);
		float lEngineIntensity = Math::Clamp(lEngine->GetIntensity(), 0.0f, 1.0f);
		if (pTag.mFloatValueList[FV_THROTTLE_FACTOR] > 0)
		{
			const float lThrottle =
				lEngine->GetLerpThrottle(lThrottleUpSpeed, lThrottleDownSpeed, true) *
				pTag.mFloatValueList[FV_THROTTLE_FACTOR];
			lEngineIntensity *= lThrottle;
			const float lCarburetorSound = ::fabs(lEngine->GetValue()) * 0.7f;
			lLowVolume = Math::Lerp(lLowVolume, pTag.mFloatValueList[FV_VOLUME_HIGH], lCarburetorSound);
		}
		lEngineIntensity *= pTag.mFloatValueList[FV_ENGINE_FACTOR_BASE+y];
		lIntensity += lEngineIntensity;
	}
	if (pTag.mFloatValueList[FV_THROTTLE_FACTOR] <= 0)
	{
		// If motor is on/off type (electric for instance), we smooth out the
		// intensity, or it will become very jerky as wheels wobble along.
		if (mEngineSoundIntensity.size() <= pEngineSoundIndex)
		{
			mEngineSoundIntensity.resize(pEngineSoundIndex+1);
		}
		const float lSmooth = std::min(pFrameTime*8.0f, 0.5f);
		lIntensity = mEngineSoundIntensity[pEngineSoundIndex] = Math::Lerp(mEngineSoundIntensity[pEngineSoundIndex], lIntensity, lSmooth);
		++pEngineSoundIndex;
	}
	//lIntensity = Math::Clamp(lIntensity, 0, 1);
	const float lVolumeLerp = ::pow(lIntensity, pTag.mFloatValueList[FV_VOLUME_EXPONENT]);
	const float lVolume = Math::Lerp(lLowVolume, pTag.mFloatValueList[FV_VOLUME_HIGH], lVolumeLerp);
	const float lPitchExp = pTag.mFloatValueList[FV_PITCH_EXPONENT];
	const float lPitchLerp = ::pow(lIntensity, lPitchExp);
	const float lPitch = Math::Lerp(pTag.mFloatValueList[FV_PITCH_LOW], pTag.mFloatValueList[FV_PITCH_HIGH], lPitchLerp);
	const float lRtrPitch = (pRealTimeRatio > 1)? ::pow(pRealTimeRatio, lPitchExp * lPitchExp) : pRealTimeRatio;
	mUiManager->GetSoundManager()->SetSoundPosition(lEngineSound->GetData(), lPosition, pVelocity);
	mUiManager->GetSoundManager()->SetVolume(lEngineSound->GetData(), lVolume);
	mUiManager->GetSoundManager()->SetPitch(lEngineSound->GetData(), lPitch * lRtrPitch);
}

void Machine::HandleTagEngineMeshOffset(const UiTbc::ChunkyClass::Tag& pTag, float pFrameTime, float pRealTimeRatio)
{
	// Mesh offset controlled by engine.

	if (pTag.mFloatValueList.size() != 9 ||
		pTag.mStringValueList.size() != 0 ||
		pTag.mBodyIndexList.size() != 0 ||
		pTag.mEngineIndexList.size() != 1 ||
		pTag.mMeshIndexList.size() < 1)
	{
		mLog.Errorf(_T("The engine_sound tag '%s' has the wrong # of parameters."), pTag.mTagName.c_str());
		assert(false);
		return;
	}

	int lEngineIndex = pTag.mEngineIndexList[0];
	if (lEngineIndex >= GetPhysics()->GetEngineCount())
	{
		return;
	}

	enum FloatValue
	{
		FV_X = 0,
		FV_Y,
		FV_Z,
		FV_ROTATION_AXIS_X,
		FV_ROTATION_AXIS_Y,
		FV_ROTATION_AXIS_Z,
		FV_ROTATION_ANGLE,
		FV_INERTIA,
		FV_SECONDARY_FACTOR,
	};

	TBC::PhysicsEngine* lEngine = GetPhysics()->GetEngine(lEngineIndex);
	const float lEngineFactor = lEngine->GetValues()[TBC::PhysicsEngine::ASPECT_PRIMARY] + std::abs(lEngine->GetValues()[TBC::PhysicsEngine::ASPECT_SECONDARY]) * pTag.mFloatValueList[FV_SECONDARY_FACTOR];
	const float lEngineAbsFactor = std::abs(lEngineFactor);

	const QuaternionF lOrientation = GetOrientation();
	const Vector3DF lOffsetPosition(Vector3DF(pTag.mFloatValueList[FV_X], pTag.mFloatValueList[FV_Y], pTag.mFloatValueList[FV_Z]) * lEngineAbsFactor);
	const float a = pTag.mFloatValueList[FV_ROTATION_ANGLE] * lEngineFactor;
	QuaternionF lOffsetOrientation(a, Vector3DF(pTag.mFloatValueList[FV_ROTATION_AXIS_X], pTag.mFloatValueList[FV_ROTATION_AXIS_Y], pTag.mFloatValueList[FV_ROTATION_AXIS_Z]));
	const TransformationF lOffset(lOffsetOrientation, lOffsetPosition);
	const float t = Math::GetIterateLerpTime(pTag.mFloatValueList[FV_INERTIA], pFrameTime*pRealTimeRatio);

	for (size_t y = 0; y < pTag.mMeshIndexList.size(); ++y)
	{
		TBC::GeometryBase* lMesh = GetMesh(pTag.mMeshIndexList[y]);
		if (!lMesh)
		{
			continue;
		}
		TBC::GeometryReference* lMeshRef = (TBC::GeometryReference*)lMesh;
		TransformationF lCurrentOffset;
		lCurrentOffset.Interpolate(lMeshRef->GetExtraOffsetTransformation(), lOffset, t);
		lMeshRef->SetExtraOffsetTransformation(lCurrentOffset);
	}
}



void Machine::LoadPlaySound3d(UserSound3dResource* pSoundResource)
{
	assert(pSoundResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE);
	if (pSoundResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		mUiManager->GetSoundManager()->Play(pSoundResource->GetData(), 0, 1.0);
	}
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, Machine);



}
