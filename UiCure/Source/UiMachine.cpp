
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
#include "../Include/UiProps.h"
#include "../Include/UiRuntimeVariableName.h"



namespace UiCure
{



Machine::Machine(Cure::ResourceManager* pResourceManager, const str& pClassId, GameUiManager* pUiManager):
	Parent(pResourceManager, pClassId, pUiManager),
	mExhaustTimeout(0),
	mCreatedParticles(false)
{
}

Machine::~Machine()
{
	DeleteEngineSounds();
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
	mParticleTimer.UpdateTimer();
	if (mCreatedParticles)
	{
		mCreatedParticles = false;
		mParticleTimer.ClearTimeDiff();
	}

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
	lPhysicsManager->GetBodyVelocity(lPhysics->GetBoneGeometry(lPhysics->GetRootBone())->GetBodyId(), lVelocity);
	size_t lEngineSoundIndex = 0;
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
				lTag.mBodyIndexList.size()+lTag.mEngineIndexList.size() != 1 ||
				lTag.mMeshIndexList.size() < 1)
			{
				mLog.Errorf(_T("The eye tag '%s' has the wrong # of parameters."), lTag.mTagName.c_str());
				assert(false);
				continue;
			}

			float lJointValue = 0;
			if (!lTag.mBodyIndexList.empty())
			{
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
					case TBC::ChunkyBoneGeometry::JOINT_HINGE:
					{
						TBC::PhysicsManager::Joint1Diff lDiff;
						lPhysicsManager->GetJoint1Diff(lBone->GetBodyId(), lJoint, lDiff);
						float lLowStop = 0;
						float lHighStop = 0;
						float lBounce = 0;
						lPhysicsManager->GetJointParams(lJoint, lLowStop, lHighStop, lBounce);
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
						mLog.Errorf(_T("Joint type %i not implemented for tag type %s."), lBone->GetJointType(), lTag.mTagName.c_str());
						assert(false);
					}
					break;
				}
			}
			else
			{
				const int lEngineIndex = lTag.mEngineIndexList[0];
				if (lEngineIndex >= mPhysics->GetEngineCount())
				{
					continue;
				}
				TBC::PhysicsEngine* lEngine = lPhysics->GetEngine(lEngineIndex);
				lJointValue = lEngine->GetLerpThrottle(0.1f, 0.1f);
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
					lMesh->SetTransformationChanged(true);
					lMesh->SetAlwaysVisible(lIsChild);
				}
			}
		}
		else if (lTag.mTagName == _T("brake_light"))
		{
			if (GetManager()->GetGameManager()->IsUiMoveForbidden(GetInstanceId()))
			{
				continue;
			}
			if (lTag.mFloatValueList.size() != 3 ||
				lTag.mStringValueList.size() != 0 ||
				lTag.mBodyIndexList.size() != 0 ||
				lTag.mEngineIndexList.size() != 1 ||
				lTag.mMeshIndexList.size() < 1)
			{
				mLog.Errorf(_T("The brake_light tag '%s' has the wrong # of parameters."), lTag.mTagName.c_str());
				assert(false);
				continue;
			}
			const int lEngineIndex = lTag.mEngineIndexList[0];
			if (lEngineIndex >= mPhysics->GetEngineCount())
			{
				continue;
			}
			const TBC::PhysicsEngine* lEngine = mPhysics->GetEngine(lEngineIndex);
			for (size_t y = 0; y < lTag.mMeshIndexList.size(); ++y)
			{
				TBC::GeometryBase* lMesh = GetMesh(lTag.mMeshIndexList[y]);
				if (lMesh)
				{
					Vector3DF& lAmbient = lMesh->GetBasicMaterialSettings().mAmbient;
					if (lEngine->GetValue() != 0)
					{
						lAmbient.Set(lTag.mFloatValueList[0], lTag.mFloatValueList[1], lTag.mFloatValueList[2]);
					}
					else
					{
						lAmbient.Set(0, 0, 0);
					}
				}
			}
		}
		else if (lTag.mTagName == _T("engine_sound"))
		{
			// Sound controlled by engine.

			if (lTag.mFloatValueList.size() != 1+9+lTag.mEngineIndexList.size() ||
				lTag.mStringValueList.size() != 1 ||
				lTag.mBodyIndexList.size() != 1 ||
				lTag.mEngineIndexList.size() < 1 ||
				lTag.mMeshIndexList.size() != 0)
			{
				mLog.Errorf(_T("The engine_sound tag '%s' has the wrong # of parameters."), lTag.mTagName.c_str());
				assert(false);
				continue;
			}
			UserSound3dResource* lEngineSound = HashUtil::FindMapObject(mEngineSoundTable, &lTag);
			if (lEngineSound == 0)
			{
				const str lSoundName = lTag.mStringValueList[0];
				lEngineSound = new UserSound3dResource(GetUiManager(), UiLepra::SoundManager::LOOP_FORWARD);
				mEngineSoundTable.insert(TagSoundTable::value_type(&lTag, lEngineSound));
				lEngineSound->Load(GetResourceManager(), lSoundName,
					UserSound3dResource::TypeLoadCallback(this, &Machine::LoadPlaySound3d));
			}
			if (lEngineSound->GetLoadState() != Cure::RESOURCE_LOAD_COMPLETE)
			{
				continue;
			}

			int lBodyIndex = lTag.mBodyIndexList[0];
			TBC::ChunkyBoneGeometry* lBone = lPhysics->GetBoneGeometry(lBodyIndex);
			const Vector3DF lPosition = lPhysicsManager->GetBodyPosition(lBone->GetBodyId());

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
			const float lThrottleUpSpeed = Math::GetIterateLerpTime(0.2f, lFrameTime);
			const float lThrottleDownSpeed = Math::GetIterateLerpTime(0.1f, lFrameTime);
			float lIntensity = 0;
			float lLowVolume = lTag.mFloatValueList[FV_VOLUME_LOW];
			for (size_t y = 0; y < lTag.mEngineIndexList.size(); ++y)
			{
				const int lEngineIndex = lTag.mEngineIndexList[y];
				if (lEngineIndex >= mPhysics->GetEngineCount())
				{
					continue;
				}
				const TBC::PhysicsEngine* lEngine = mPhysics->GetEngine(lEngineIndex);
				float lEngineIntensity = Math::Clamp(lEngine->GetIntensity(), 0.0f, 1.0f);
				if (lTag.mFloatValueList[FV_THROTTLE_FACTOR] > 0)
				{
					const float lThrottle =
						::fabs(lEngine->GetLerpThrottle(lThrottleUpSpeed, lThrottleDownSpeed)) *
						lTag.mFloatValueList[FV_THROTTLE_FACTOR];
					lEngineIntensity *= lThrottle;
					const float lCarburetorSound = ::fabs(lEngine->GetValue()) * 0.7f;
					lLowVolume = Math::Lerp(lLowVolume, lTag.mFloatValueList[FV_VOLUME_HIGH], lCarburetorSound);
				}
				lEngineIntensity *= lTag.mFloatValueList[FV_ENGINE_FACTOR_BASE+y];
				lIntensity += lEngineIntensity;
			}
			if (lTag.mFloatValueList[FV_THROTTLE_FACTOR] <= 0)
			{
				// If motor is on/off type (electric for instance), we smooth out the
				// intensity, or it will become very jerky as wheels wobble along.
				if (mEngineSoundIntensity.size() <= lEngineSoundIndex)
				{
					mEngineSoundIntensity.resize(lEngineSoundIndex+1);
				}
				const float lSmooth = std::min(lFrameTime*8.0f, 0.5f);
				lIntensity = mEngineSoundIntensity[lEngineSoundIndex] = Math::Lerp(mEngineSoundIntensity[lEngineSoundIndex], lIntensity, lSmooth);
				++lEngineSoundIndex;
			}
			//lIntensity = Math::Clamp(lIntensity, 0, 1);
			const float lVolumeLerp = ::pow(lIntensity, lTag.mFloatValueList[FV_VOLUME_EXPONENT]);
			const float lVolume = Math::Lerp(lLowVolume, lTag.mFloatValueList[FV_VOLUME_HIGH], lVolumeLerp);
			const float lPitchExp = lTag.mFloatValueList[FV_PITCH_EXPONENT];
			const float lPitchLerp = ::pow(lIntensity, lPitchExp);
			const float lPitch = Math::Lerp(lTag.mFloatValueList[FV_PITCH_LOW], lTag.mFloatValueList[FV_PITCH_HIGH], lPitchLerp);
			const float lRtrPitch = (lRealTimeRatio > 1)? ::pow(lRealTimeRatio, lPitchExp * lPitchExp) : lRealTimeRatio;
			mUiManager->GetSoundManager()->SetSoundPosition(lEngineSound->GetData(), lPosition, lVelocity);
			mUiManager->GetSoundManager()->SetVolume(lEngineSound->GetData(), lVolume);
			mUiManager->GetSoundManager()->SetPitch(lEngineSound->GetData(), lPitch * lRtrPitch);
		}
		else if (lTag.mTagName == _T("exhaust"))
		{
			// Particles coming out of exhaust.
			if (GetManager()->GetGameManager()->IsUiMoveForbidden(GetInstanceId()))
			{
				continue;
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
			if (lTag.mFloatValueList.size() != FV_COUNT ||
				lTag.mStringValueList.size() != 0 ||
				lTag.mEngineIndexList.size() != 1 ||
				lTag.mBodyIndexList.size() != 0 ||
				lTag.mMeshIndexList.size() < 1)
			{
				mLog.Errorf(_T("The exhaust tag '%s' has the wrong # of parameters."), lTag.mTagName.c_str());
				assert(false);
				continue;
			}
			const int lEngineIndex = lTag.mEngineIndexList[0];
			if (lEngineIndex >= mPhysics->GetEngineCount())
			{
				continue;
			}
			const TBC::PhysicsEngine* lEngine = mPhysics->GetEngine(lEngineIndex);
			const float lDensity = lTag.mFloatValueList[FV_DENSITY];
			mExhaustTimeout -= std::max(0.15f, lEngine->GetIntensity() * lDensity) * lFrameTime * 25;
			if (mExhaustTimeout > 0)
			{
				continue;
			}
			mExhaustTimeout = 1.51f;

			const QuaternionF lOriginalOrientation = GetOrientation();
			Vector3DF lOffset(lTag.mFloatValueList[FV_X], lTag.mFloatValueList[FV_Y], lTag.mFloatValueList[FV_Z]);
			lOffset = lOriginalOrientation*lOffset;
			Vector3DF lVelocity(lTag.mFloatValueList[FV_VX], lTag.mFloatValueList[FV_VY], lTag.mFloatValueList[FV_VZ]);
			const float lOpacity = lTag.mFloatValueList[FV_OPACITY];
			lVelocity = lOriginalOrientation*lVelocity;
			lVelocity += GetVelocity()*0.5f;
			for (size_t y = 0; y < lTag.mMeshIndexList.size(); ++y)
			{
				TBC::GeometryBase* lMesh = GetMesh(lTag.mMeshIndexList[y]);
				if (lMesh)
				{
					int lPhysIndex = -1;
					str lMeshName;
					TransformationF lTransform;
					((UiTbc::ChunkyClass*)GetClass())->GetMesh(lTag.mMeshIndexList[y], lPhysIndex, lMeshName, lTransform);
					lTransform = lMesh->GetBaseTransformation() * lTransform;
					lTransform.GetPosition() += lOffset;
					Props* lPuff = new Props(GetResourceManager(), _T("mud_particle_01"), mUiManager);
					GetManager()->GetGameManager()->AddContextObject(lPuff, Cure::NETWORK_OBJECT_LOCAL_ONLY, 0);
					//mLog.Infof(_T("Machine %i creates fume particle %i."), GetInstanceId(), lPuff->GetInstanceId());
					lPuff->DisableRootShadow();
					lPuff->SetInitialTransform(lTransform);
					lPuff->SetOpacity(lOpacity);
					lPuff->StartParticle(Props::PARTICLE_GAS, lVelocity, 3, 0.5f, 2.0f);
					lPuff->StartLoading();
				}
			}
		}
	}

	if (lIsChild || lPhysics->GetGuideMode() == TBC::ChunkyPhysics::GUIDE_ALWAYS)
	{
		StabilizeTick();
	}
}



void Machine::OnForceApplied(TBC::PhysicsManager::ForceFeedbackListener* pOtherObject,
	TBC::PhysicsManager::BodyID pOwnBodyId, TBC::PhysicsManager::BodyID pOtherBodyId,
	const Vector3DF& pForce, const Vector3DF& pTorque,
	const Vector3DF& pPosition, const Vector3DF& pRelativeVelocity)
{
	Parent::OnForceApplied(pOtherObject, pOwnBodyId, pOtherBodyId, pForce, pTorque, pPosition, pRelativeVelocity);

	bool lParticlesEnabled;
	CURE_RTVAR_GET(lParticlesEnabled, =, UiCure::GetSettings(), RTVAR_UI_3D_ENABLEPARTICLES, false);
	if (!lParticlesEnabled)
	{
		return;
	}
	// Particle emitter code. TODO: separate somewhat. Cleanup.
	if (mParticleTimer.GetTimeDiff() < 0.3f)
	{
		return;
	}
	const float lRelativeSpeedLimit = 3;
	if (pRelativeVelocity.GetLengthSquared() < lRelativeSpeedLimit*lRelativeSpeedLimit)
	{
		return;
	}
	const Cure::ContextObject* lOtherObject = (Cure::ContextObject*)pOtherObject;
	if (!lOtherObject)
	{
		return;
	}
	if (lOtherObject->GetPhysics()->GetBoneGeometry(pOtherBodyId)->GetMaterial() != _T("grass"))
	{
		return;
	}
	const float lDistance = 100;	// Only show gravel particles inside this distance.
	if (!GetManager()->GetGameManager()->IsObjectRelevant(pPosition, lDistance))
	{
		return;
	}
	const float lImpactFactor = GetPhysics()->GetBoneGeometry(pOwnBodyId)->GetImpactFactor();
	const float lImpact = GetImpact(GetManager()->GetGameManager()->GetPhysicsManager()->GetGravity(),
		pForce, pTorque, 0, 11) * lImpactFactor;
	if (lImpact < 0.4f)
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
	const float lMassFactor = 1/GetMass();
	lRelativeVelocity += lTorque * lMassFactor * 0.1f;
	Vector3DF lRotationSpeed;
	GetManager()->GetGameManager()->GetPhysicsManager()->GetBodyAngularVelocity(pOwnBodyId, lRotationSpeed);
	const Vector3DF lRadius = pPosition - GetManager()->GetGameManager()->GetPhysicsManager()->GetBodyPosition(pOwnBodyId);
	const Vector3DF lRollSpeed(lRotationSpeed.Cross(lRadius) * 0.2f);
	lPosition += lRollSpeed.GetNormalized(0.3f);
	const float lRollLength = lRollSpeed.GetLength();
	const float lCollisionLength = lRelativeVelocity.GetLength();
	lRelativeVelocity += lRollSpeed;
	lRelativeVelocity.z += lCollisionLength*0.2f + lRollLength*0.3f;
	lRelativeVelocity.x += (float)Random::Uniform(-lCollisionLength*0.05f, +lCollisionLength*0.05f);
	lRelativeVelocity.y += (float)Random::Uniform(-lCollisionLength*0.05f, +lCollisionLength*0.05f);
	lRelativeVelocity.z += (float)Random::Uniform(-lCollisionLength*0.02f, +lCollisionLength*0.05f);
	if (lRelativeVelocity.GetLengthSquared() < pRelativeVelocity.GetLengthSquared()*200*200)
	{
		Props* lPuff = new Props(GetResourceManager(), _T("mud_particle_01"), mUiManager);
		GetManager()->GetGameManager()->AddContextObject(lPuff, Cure::NETWORK_OBJECT_LOCAL_ONLY, 0);
		lPuff->SetInitialTransform(TransformationF(gIdentityQuaternionF, lPosition));
		lPuff->StartParticle(Props::PARTICLE_SOLID, lRelativeVelocity, 1, 2, 2);
		lPuff->StartLoading();
		mCreatedParticles = true;
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
