
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "Vehicle.h"
#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/GameManager.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Cure/Include/TimeManager.h"
#include "../../Lepra/Include/HashUtil.h"
#include "../../TBC/Include/ChunkyBoneGeometry.h"
#include "../../TBC/Include/ChunkyPhysics.h"
#include "../../TBC/Include/PhysicsEngine.h"
#include "RtVar.h"



namespace Life
{



Vehicle::Vehicle(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager):
	Parent(pResourceManager, pClassId, pUiManager)
{
}

Vehicle::~Vehicle()
{
	TagSoundTable::iterator x = mEngineSoundTable.begin();
	for (; x != mEngineSoundTable.end(); ++x)
	{
		delete (x->second);
	}
	mEngineSoundTable.clear();
}



void Vehicle::OnPhysicsTick()
{
	Parent::OnPhysicsTick();

	const TBC::ChunkyPhysics* lPhysics = GetPhysics();
	const UiTbc::ChunkyClass* lClass = GetClass();
	if (!lPhysics || !lClass)
	{
		return;
	}
	const float lFrameTime = std::min(0.1f, GetManager()->GetGameManager()->GetTimeManager()->GetNormalFrameTime());
	bool lIsChild = CURE_RTVAR_GET(mManager->GetGameManager()->GetVariableScope(), RTVAR_GAME_ISCHILD, true);
	const TBC::PhysicsManager* lPhysicsManager = mManager->GetGameManager()->GetPhysicsManager();
	Vector3DF lVelocity;
	lPhysicsManager->GetBodyVelocity(lPhysics->GetBoneGeometry(lPhysics->GetRootBone())->GetBodyId(), lVelocity);
	for (size_t x = 0; x < lClass->GetTagCount(); ++x)
	{
		const UiTbc::ChunkyClass::Tag& lTag = lClass->GetTag(x);
		if (lTag.mTagName == _T("eye"))
		{
			// Eyes follow steered wheels. Get wheel corresponding to eye and
			// move eye accordingly á là Lightning McQueen.

			if (GetManager()->GetGameManager()->IsUiMoveForbidden(GetInstanceId()))
			{
				continue;
			}
			if (lTag.mFloatValueList.size() != 1 ||
				lTag.mStringValueList.size() != 0 ||
				lTag.mBodyIndexList.size() != 1 ||
				lTag.mMeshIndexList.size() < 1)
			{
				mLog.Errorf(_T("The eye tag '%s' has the wrong # of parameters."), lTag.mTagName.c_str());
				assert(false);
				continue;
			}

			float lJointValue = 0;
			int lBodyIndex = lTag.mBodyIndexList[0];
			TBC::ChunkyBoneGeometry* lBone = lPhysics->GetBoneGeometry(lBodyIndex);
			TBC::PhysicsManager::JointID lJoint = lBone->GetJointId();
			switch (lBone->GetJointType())
			{
				case TBC::ChunkyBoneGeometry::JOINT_HINGE2:
				{
					TBC::PhysicsManager::Joint3Diff lDiff;
					lPhysicsManager->GetJoint3Diff(lBone->GetBodyId(), lJoint, lDiff);
					float lLowStop = 0;
					float lHighStop = 0;
					float lBounce = 0;
					lPhysicsManager->GetJointParams(lJoint, lLowStop, lHighStop, lBounce);
					lJointValue = lDiff.mAngle1 * 2 / (lHighStop-lLowStop);
				}
				break;
				default:
				{
					mLog.Errorf(_T("Joint type %i not implemented for tag type %s."), lBone->GetJointType(), lTag.mTagName.c_str());
					assert(false);
				}
				break;
			}
			const float lScale = lTag.mFloatValueList[0];
			const float lJointRightValue = lJointValue * lScale;
			const float lJointDownValue = (::cos(lJointValue)-1) * lScale * 0.5f;
			for (size_t y = 0; y < lTag.mMeshIndexList.size(); ++y)
			{
				TBC::GeometryBase* lMesh = GetMesh(lTag.mMeshIndexList[y]);
				if (lMesh)
				{
					TransformationF lTransform = lMesh->GetBaseTransformation();
					lTransform.MoveRight(lJointRightValue);
					lTransform.MoveBackward(lJointDownValue);
					lMesh->SetTransformation(lTransform);
					lMesh->SetAlwaysVisible(lIsChild);
				}
			}
		}
		else if (lTag.mTagName == _T("engine_sound"))
		{
			// Sound controlled by engine.

			if (lTag.mFloatValueList.size() != 9 ||
				lTag.mStringValueList.size() != 1 ||
				lTag.mBodyIndexList.size() != 1 ||
				lTag.mMeshIndexList.size() != 0)
			{
				mLog.Errorf(_T("The engine_sound tag '%s' has the wrong # of parameters."), lTag.mTagName.c_str());
				assert(false);
				continue;
			}
			UiCure::UserSound3dResource* lEngineSound = HashUtil::FindMapObject(mEngineSoundTable, &lTag);
			if (lEngineSound == 0)
			{
				const str lSoundName = _T("Data/")+lTag.mStringValueList[0];
				lEngineSound = new UiCure::UserSound3dResource(GetUiManager(), UiLepra::SoundManager::LOOP_FORWARD);
				mEngineSoundTable.insert(TagSoundTable::value_type(&lTag, lEngineSound));
				lEngineSound->Load(GetResourceManager(), lSoundName,
					UiCure::UserSound3dResource::TypeLoadCallback(this, &Vehicle::LoadPlaySound3d));
			}
			if (lEngineSound->GetLoadState() != Cure::RESOURCE_LOAD_COMPLETE)
			{
				continue;
			}

			int lBodyIndex = lTag.mBodyIndexList[0];
			TBC::ChunkyBoneGeometry* lBone = lPhysics->GetBoneGeometry(lBodyIndex);
			const Vector3DF lPosition = lPhysicsManager->GetBodyPosition(lBone->GetBodyId());
			const TBC::PhysicsEngine* lEngine = mPhysics->GetEngine(lTag.mEngineIndexList[0]);
			float lIntensity = ::fabs(lEngine->GetIntensity());
			const float lThrottleUpSpeed = Math::GetIterateLerpTime(0.2f, lFrameTime);
			const float lThrottleDownSpeed = Math::GetIterateLerpTime(0.1f, lFrameTime);
			const float lThrottle = ::fabs(lEngine->GetLerpThrottle(lThrottleUpSpeed, lThrottleDownSpeed));
			if (lEngine->GetEngineType() != TBC::PhysicsEngine::ENGINE_HINGE_GYRO)
			{
				lIntensity *= lThrottle;
			}
			enum FloatValue
			{
				FV_PITCH_LOW = 0,
				FV_PITCH_HIGH,
				FV_PITCH_EXPONENT,
				FV_VOLUME_LOW,
				FV_VOLUME_HIGH,
				FV_VOLUME_EXPONENT,
				FV_INTENSITY_LOW,
				FV_INTENSITY_HIGH,
				FV_INTENSITY_EXPONENT,
			};
			float lVolume = ::pow(lIntensity, lTag.mFloatValueList[FV_VOLUME_EXPONENT]);
			lVolume = Math::Lerp(lTag.mFloatValueList[FV_VOLUME_LOW], lTag.mFloatValueList[FV_VOLUME_HIGH], lVolume);
			const float lPitch = Math::Lerp(lTag.mFloatValueList[FV_PITCH_LOW], lTag.mFloatValueList[FV_PITCH_HIGH], lIntensity);
			mUiManager->GetSoundManager()->SetSoundPosition(lEngineSound->GetData(), lPosition, lVelocity);
			mUiManager->GetSoundManager()->SetVolume(lEngineSound->GetData(), lVolume);
			mUiManager->GetSoundManager()->SetPitch(lEngineSound->GetData(), lPitch);
		}
	}
}



void Vehicle::LoadPlaySound3d(UiCure::UserSound3dResource* pSoundResource)
{
	assert(pSoundResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE);
	if (pSoundResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		mUiManager->GetSoundManager()->Play(pSoundResource->GetData(), 0, 1.0);
	}
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, Vehicle);



}
