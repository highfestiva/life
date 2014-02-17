
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../Include/UiMachine.h"
#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/GameManager.h"
#include "../../Cure/Include/Health.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Cure/Include/TimeManager.h"
#include "../../Lepra/Include/HashUtil.h"
#include "../../Lepra/Include/Random.h"
#include "../../TBC/Include/ChunkyBoneGeometry.h"
#include "../../TBC/Include/ChunkyPhysics.h"
#include "../../TBC/Include/PhysicsEngine.h"
#include "../Include/UiBurnEmitter.h"
#include "../Include/UiExhaustEmitter.h"
#include "../Include/UiGameUiManager.h"
#include "../Include/UiJetEngineEmitter.h"
#include "../Include/UiProps.h"
#include "../Include/UiRuntimeVariableName.h"
#include "../Include/UiSoundReleaser.h"



namespace UiCure
{



Machine::Machine(Cure::ResourceManager* pResourceManager, const str& pClassId, GameUiManager* pUiManager):
	Parent(pResourceManager, pClassId, pUiManager),
	mJetEngineEmitter(0),
	mExhaustEmitter(0),
	mBurnEmitter(0),
	mBlinkTime(0),
	mMeshOffsetTime(0),
	mMeshRotateTime(0)
{
	EnableMeshSlide(true);
}

Machine::~Machine()
{
	DeleteEngineSounds();
	delete mJetEngineEmitter;
	mJetEngineEmitter = 0;
	delete mExhaustEmitter;
	mExhaustEmitter = 0;
	delete mBurnEmitter;
	mBurnEmitter = 0;
}

void Machine::SetJetEngineEmitter(JetEngineEmitter* pEmitter)
{
	mJetEngineEmitter = pEmitter;
}

void Machine::SetExhaustEmitter(ExhaustEmitter* pEmitter)
{
	mExhaustEmitter = pEmitter;
}

void Machine::SetBurnEmitter(BurnEmitter* pEmitter)
{
	mBurnEmitter = pEmitter;
}

BurnEmitter* Machine::GetBurnEmitter() const
{
	return mBurnEmitter;
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
	if (GetManager()->GetGameManager()->IsUiMoveForbidden(GetInstanceId()))
	{
		return;
	}

	const Cure::TimeManager* lTimeManager = GetManager()->GetGameManager()->GetTimeManager();
	float lRealTimeRatio = lTimeManager->GetRealTimeRatio();
	const float lFrameTime = lTimeManager->GetNormalGameFrameTime();
	const bool lIsChild = IsAttributeTrue(_T("float_childishness"));
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
		if (lTag.mTagName == _T("ambient_sounds"))
		{
			HandleTagAmbientSounds(lTag, lRealTimeRatio);
		}
		else if (lTag.mTagName == _T("eye"))
		{
			HandleTagEye(lTag, lPhysicsManager, lIsChild);
		}
		else if (lTag.mTagName == _T("brake_light"))
		{
			HandleTagBrakeLight(lTag);
		}
		else if (lTag.mTagName == _T("engine_light"))
		{
			HandleTagEngineLight(lTag, lFrameTime*lRealTimeRatio);
		}
		else if (lTag.mTagName == _T("blink_light"))
		{
			HandleTagBlinkLight(lTag, lFrameTime*lRealTimeRatio);
		}
		else if (lTag.mTagName == _T("jet_engine_emitter"))
		{
			// Faijah!
			if (mJetEngineEmitter)
			{
				mJetEngineEmitter->EmitFromTag(this, lTag, lFrameTime*lRealTimeRatio);
			}
		}
		else if (lTag.mTagName == _T("engine_sound"))
		{
			HandleTagEngineSound(lTag, lPhysicsManager, lVelocity, lFrameTime*lRealTimeRatio, lRealTimeRatio, lEngineSoundIndex);
		}
		else if (lTag.mTagName == _T("engine_mesh_offset"))
		{
			HandleTagEngineMeshOffset(lTag, lFrameTime);
		}
		else if (lTag.mTagName == _T("mesh_offset"))
		{
			HandleTagMeshOffset(lTag, lFrameTime*lRealTimeRatio);
		}
		else if (lTag.mTagName == _T("mesh_rotate"))
		{
			HandleTagMeshRotate(lTag, lFrameTime*lRealTimeRatio);
		}
		else if (lTag.mTagName == _T("exhaust"))
		{
			// Particles coming out of exhaust.
			if (mExhaustEmitter)
			{
				mExhaustEmitter->EmitFromTag(this, lTag, lFrameTime);
			}
		}
		else if (lTag.mTagName == _T("burn"))
		{
			// Particles caused by burning.
			if (mBurnEmitter)
			{
				const float lHealth = Math::Clamp(Cure::Health::Get(this), 0.0f, 1.0f);
				mBurnEmitter->EmitFromTag(this, lTag, lFrameTime, (-lHealth + 0.7f)/0.7f);
			}
		}
	}

	if (lIsChild || lPhysics->GetGuideMode() == TBC::ChunkyPhysics::GUIDE_ALWAYS)
	{
		StabilizeTick();
	}
}



void Machine::HandleTagAmbientSounds(const UiTbc::ChunkyClass::Tag& pTag, float pRealTimeRatio)
{
	if (pTag.mFloatValueList.size() != 6 ||
		pTag.mStringValueList.size() < 1 ||
		pTag.mBodyIndexList.size() != 0 ||
		pTag.mEngineIndexList.size() != 0 ||
		pTag.mMeshIndexList.size() != 0)
	{
		mLog.Errorf(_T("The ambient_sounds tag '%s' has the wrong # of parameters."), pTag.mTagName.c_str());
		deb_assert(false);
		return;
	}
	enum
	{
		MIN_DELAY,	MAX_DELAY,
		MIN_PITCH,	MAX_PITCH,
		MIN_VOLUME,	MAX_VOLUME,
	};
	if (!mAmbientSoundTimer.IsStarted())
	{
		mAmbientSoundTimer.Start(-pTag.mFloatValueList[MIN_DELAY]);
	}
	else if (mAmbientSoundTimer.QueryTimeDiff() >= 0)
	{
		mAmbientSoundTimer.Start(-Random::Uniform(pTag.mFloatValueList[MIN_DELAY], pTag.mFloatValueList[MAX_DELAY]));
		const size_t lRandomSoundIndex = Random::GetRandomNumber() % pTag.mStringValueList.size();
		const str lSoundName = pTag.mStringValueList[lRandomSoundIndex];
		UiCure::UserSound2dResource* lSound = new UiCure::UserSound2dResource(GetUiManager(), UiLepra::SoundManager::LOOP_NONE);
		const float lPitch = Random::Uniform(pTag.mFloatValueList[MIN_PITCH], pTag.mFloatValueList[MAX_PITCH]);
		const float lVolume = Random::Uniform(pTag.mFloatValueList[MIN_VOLUME], pTag.mFloatValueList[MAX_VOLUME]);
		new UiCure::SoundReleaser(GetResourceManager(), mUiManager, GetManager(), lSoundName, lSound, lVolume, lPitch*pRealTimeRatio);
	}
}

void Machine::HandleTagEye(const UiTbc::ChunkyClass::Tag& pTag, const TBC::PhysicsManager* pPhysicsManager, bool pIsChild)
{
	// Eyes follow steered wheels. Get wheel corresponding to eye and
	// move eye accordingly á là Lightning McQueen.

	if (pTag.mFloatValueList.size() != 1 ||
		pTag.mStringValueList.size() != 0 ||
		pTag.mBodyIndexList.size()+pTag.mEngineIndexList.size() != 1 ||
		pTag.mMeshIndexList.size() < 1)
	{
		mLog.Errorf(_T("The eye tag '%s' has the wrong # of parameters."), pTag.mTagName.c_str());
		deb_assert(false);
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
				deb_assert(false);
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
	if (pTag.mFloatValueList.size() != 3 ||
		pTag.mStringValueList.size() != 0 ||
		pTag.mBodyIndexList.size() != 0 ||
		pTag.mEngineIndexList.size() != 1 ||
		pTag.mMeshIndexList.size() < 1)
	{
		mLog.Errorf(_T("The brake_light tag '%s' has the wrong # of parameters."), pTag.mTagName.c_str());
		deb_assert(false);
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

void Machine::HandleTagEngineLight(const UiTbc::ChunkyClass::Tag& pTag, float pFrameTime)
{
	if (pTag.mFloatValueList.size() != 2 ||
		pTag.mStringValueList.size() != 0 ||
		pTag.mBodyIndexList.size() != 0 ||
		pTag.mEngineIndexList.size() != 1 ||
		pTag.mMeshIndexList.size() < 1)
	{
		mLog.Errorf(_T("The engine_light tag '%s' has the wrong # of parameters."), pTag.mTagName.c_str());
		deb_assert(false);
		return;
	}
	const int lEngineIndex = pTag.mEngineIndexList[0];
	if (lEngineIndex >= mPhysics->GetEngineCount())
	{
		return;
	}
	const float lGlowFactor = pTag.mFloatValueList[0];
	const float lThrottleUpSpeed = Math::GetIterateLerpTime(pTag.mFloatValueList[1]*0.5f, pFrameTime);
	const float lThrottleDownSpeed = Math::GetIterateLerpTime(pTag.mFloatValueList[1], pFrameTime);
	const TBC::PhysicsEngine* lEngine = mPhysics->GetEngine(lEngineIndex);
	const float lEngineThrottle = lEngine->GetLerpThrottle(lThrottleUpSpeed, lThrottleDownSpeed, true);
	const float lAmbientChannel = Math::Lerp(lGlowFactor, 1.0f, lEngineThrottle);
	for (size_t y = 0; y < pTag.mMeshIndexList.size(); ++y)
	{
		TBC::GeometryBase* lMesh = GetMesh(pTag.mMeshIndexList[y]);
		if (lMesh)
		{
			lMesh->GetBasicMaterialSettings().mAmbient.Set(lAmbientChannel, lAmbientChannel, lAmbientChannel);
		}
	}
}

void Machine::HandleTagBlinkLight(const UiTbc::ChunkyClass::Tag& pTag, float pFrameTime)
{
	if (pTag.mFloatValueList.size() != 9 ||
		pTag.mStringValueList.size() != 0 ||
		pTag.mBodyIndexList.size() != 0 ||
		pTag.mEngineIndexList.size() != 0 ||
		pTag.mMeshIndexList.size() < 1)
	{
		mLog.Errorf(_T("The blink_light tag '%s' has the wrong # of parameters."), pTag.mTagName.c_str());
		deb_assert(false);
		return;
	}
	enum
	{
		R_OFF = 0, R_AMP, R_SPEED,
		G_OFF,     G_AMP, G_SPEED,
		B_OFF,     B_AMP, B_SPEED,
	};
#define V(i)	pTag.mFloatValueList[i]
	const float r = V(R_OFF) + V(R_AMP) * sin(mBlinkTime*2*PIF*V(R_SPEED));
	const float g = V(G_OFF) + V(G_AMP) * sin(mBlinkTime*2*PIF*V(G_SPEED));
	const float b = V(B_OFF) + V(B_AMP) * sin(mBlinkTime*2*PIF*V(B_SPEED));
	for (size_t y = 0; y < pTag.mMeshIndexList.size(); ++y)
	{
		TBC::GeometryBase* lMesh = GetMesh(pTag.mMeshIndexList[y]);
		if (lMesh)
		{
			lMesh->GetBasicMaterialSettings().mAmbient.Set(r, g, b);
		}
	}
	mBlinkTime += pFrameTime;
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
		deb_assert(false);
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
			const float lCarburetorSound = std::min(std::fabs(lEngine->GetValue()) * 0.7f, 1.0f);
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
	const UiLepra::SoundManager::SoundInstanceID lSoundId = lEngineSound->GetData();
	mUiManager->GetSoundManager()->SetSoundPosition(lSoundId, lPosition, pVelocity);
	mUiManager->GetSoundManager()->SetVolume(lSoundId, lVolume);
	mUiManager->GetSoundManager()->SetPitch(lSoundId, lPitch * lRtrPitch);
}

void Machine::HandleTagEngineMeshOffset(const UiTbc::ChunkyClass::Tag& pTag, float pFrameTime)
{
	// Mesh offset controlled by engine.

	if (pTag.mFloatValueList.size() != 10 ||
		pTag.mStringValueList.size() != 0 ||
		pTag.mBodyIndexList.size() != 0 ||
		pTag.mEngineIndexList.size() != 1 ||
		pTag.mMeshIndexList.size() < 1)
	{
		mLog.Errorf(_T("The engine_mesh_offset tag '%s' has the wrong # of parameters."), pTag.mTagName.c_str());
		deb_assert(false);
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
		FV_PRIMARY_FACTOR,
		FV_SECONDARY_FACTOR,
	};

	TBC::PhysicsEngine* lEngine = GetPhysics()->GetEngine(lEngineIndex);
	const float lEngineFactor = lEngine->GetValues()[TBC::PhysicsEngine::ASPECT_PRIMARY] * pTag.mFloatValueList[FV_PRIMARY_FACTOR] +
			lEngine->GetValues()[TBC::PhysicsEngine::ASPECT_SECONDARY] * pTag.mFloatValueList[FV_SECONDARY_FACTOR];
	const float lEngineAbsFactor = std::abs(lEngineFactor);

	const Vector3DF lOffsetPosition(Vector3DF(pTag.mFloatValueList[FV_X], pTag.mFloatValueList[FV_Y], pTag.mFloatValueList[FV_Z]) * lEngineAbsFactor);
	const float a = pTag.mFloatValueList[FV_ROTATION_ANGLE] * lEngineFactor;
	QuaternionF lOffsetOrientation(a, Vector3DF(pTag.mFloatValueList[FV_ROTATION_AXIS_X], pTag.mFloatValueList[FV_ROTATION_AXIS_Y], pTag.mFloatValueList[FV_ROTATION_AXIS_Z]));
	const TransformationF lOffset(lOffsetOrientation, lOffsetPosition);
	const float t = Math::GetIterateLerpTime(1/pTag.mFloatValueList[FV_INERTIA], pFrameTime);

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

void Machine::HandleTagMeshOffset(const UiTbc::ChunkyClass::Tag& pTag, float pFrameTime)
{
	// Mesh offset controlled by engine.

	if (pTag.mFloatValueList.size() != 6 ||
		pTag.mStringValueList.size() != 1 ||
		pTag.mBodyIndexList.size() != 0 ||
		pTag.mEngineIndexList.size() != 0 ||
		pTag.mMeshIndexList.size() < 1)
	{
		mLog.Errorf(_T("The mesh_offset tag '%s' has the wrong # of parameters."), pTag.mTagName.c_str());
		deb_assert(false);
		return;
	}

	const str lFunction = pTag.mStringValueList[0];
	enum FloatValue
	{
		FV_INITIAL_DELAY,
		FV_DURATION,
		FV_FREQUENCY,
		FV_X,
		FV_Y,
		FV_Z,
	};
#define V(i)	pTag.mFloatValueList[i]

	const float lStartTime = V(FV_INITIAL_DELAY);
	float lAmplitude = CalculateMeshOffset(lFunction, lStartTime, V(FV_DURATION), V(FV_FREQUENCY), pFrameTime, mMeshOffsetTime);

	const Vector3DF lOffsetPosition(GetOrientation().GetInverseRotatedVector(Vector3DF(V(FV_X), V(FV_Y), V(FV_Z)) * lAmplitude));
	for (size_t y = 0; y < pTag.mMeshIndexList.size(); ++y)
	{
		TBC::GeometryBase* lMesh = GetMesh(pTag.mMeshIndexList[y]);
		if (!lMesh)
		{
			continue;
		}
		TBC::GeometryReference* lMeshRef = (TBC::GeometryReference*)lMesh;
		TransformationF lExtraOffset = lMeshRef->GetExtraOffsetTransformation();
		lExtraOffset.mPosition = lOffsetPosition;
		lMeshRef->SetExtraOffsetTransformation(lExtraOffset);
	}
}

void Machine::HandleTagMeshRotate(const UiTbc::ChunkyClass::Tag& pTag, float pFrameTime)
{
	// Mesh offset controlled by engine.

	if (pTag.mFloatValueList.size() != 6 ||
		pTag.mStringValueList.size() != 1 ||
		pTag.mBodyIndexList.size() != 0 ||
		pTag.mEngineIndexList.size() != 0 ||
		pTag.mMeshIndexList.size() < 1)
	{
		mLog.Errorf(_T("The mesh_rotate tag '%s' has the wrong # of parameters."), pTag.mTagName.c_str());
		deb_assert(false);
		return;
	}

	const str lFunction = pTag.mStringValueList[0];
	enum FloatValue
	{
		FV_INITIAL_DELAY,
		FV_DURATION,
		FV_FREQUENCY,
		FV_X,
		FV_Y,
		FV_Z,
	};
#define V(i)	pTag.mFloatValueList[i]

	const float lStartTime = V(FV_INITIAL_DELAY);
	float lAmplitude = CalculateMeshOffset(lFunction, lStartTime, V(FV_DURATION), V(FV_FREQUENCY), pFrameTime, mMeshRotateTime);

	QuaternionF lOffsetOrientation;
	lOffsetOrientation.SetEulerAngles(V(FV_Z)*lAmplitude, V(FV_X)*lAmplitude, V(FV_Y)*lAmplitude);
	for (size_t y = 0; y < pTag.mMeshIndexList.size(); ++y)
	{
		TBC::GeometryBase* lMesh = GetMesh(pTag.mMeshIndexList[y]);
		if (!lMesh)
		{
			continue;
		}
		TBC::GeometryReference* lMeshRef = (TBC::GeometryReference*)lMesh;
		TransformationF lExtraOffset = lMeshRef->GetExtraOffsetTransformation();
		lExtraOffset.mOrientation = lOffsetOrientation;
		lMeshRef->SetExtraOffsetTransformation(lExtraOffset);
	}
}



float Machine::CalculateMeshOffset(const str& pFunction, float lStartTime, float pDuration, float pFrequency, float pFrameTime, float& pMeshTime) const
{
	if (pMeshTime < lStartTime || pMeshTime > lStartTime+pDuration)
	{
		return 0;
	}
	float lAmplitude = 0;
	float x = pMeshTime-lStartTime;
	if (pFunction == _T("|sin|"))
	{
		lAmplitude = std::abs(::sin(x*2*PIF*pFrequency));
	}
	else if (pFunction == _T("linear"))
	{
		lAmplitude = x;
	}
	else
	{
		mLog.Errorf(_T("Uknown mesh_xxx function '%s'."), pFunction.c_str());
		deb_assert(false);
	}
	pMeshTime += pFrameTime;
	return lAmplitude;
}



void Machine::LoadPlaySound3d(UserSound3dResource* pSoundResource)
{
	deb_assert(pSoundResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE);
	if (pSoundResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		mUiManager->GetSoundManager()->Play(pSoundResource->GetData(), 0, 1.0);
	}
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, Machine);



}
