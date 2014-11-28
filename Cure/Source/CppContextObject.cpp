
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/CppContextObject.h"
#include "../../Lepra/Include/LepraAssert.h"
#include "../../Lepra/Include/DiskFile.h"
#include "../../Lepra/Include/HashUtil.h"
#include "../../Tbc/Include/ChunkyPhysics.h"
#include "../../Tbc/Include/PhysicsEngine.h"
#include "../../Tbc/Include/PhysicsTrigger.h"
#include "../Include/ContextManager.h"
#include "../Include/GameManager.h"
#include "../Include/RuntimeVariable.h"
#include "../Include/TimeManager.h"



namespace Cure
{



CppContextObject::CppContextObject(ResourceManager* pResourceManager, const str& pClassId):
	ContextObject(pResourceManager, pClassId),
	mClassResource(0),
	mPhysicsResource(0),
	mAllowNetworkLogic(true),
	mForceLoadUnique(false)
{
}

CppContextObject::~CppContextObject()
{
	ClearPhysics();
	delete (mPhysicsResource);
	mPhysicsResource = 0;
	delete (mClassResource);
	mClassResource = 0;
}



Tbc::ChunkyPhysics::GuideMode CppContextObject::GetGuideMode() const
{
	if (GetPhysics())
	{
		return GetPhysics()->GetGuideMode();
	}
	return Tbc::ChunkyPhysics::GUIDE_EXTERNAL;
}

void CppContextObject::StabilizeTick()
{
	const Tbc::ChunkyPhysics* lPhysics = GetPhysics();
	const Tbc::ChunkyClass* lClass = GetClass();
	if (!lPhysics || !lClass)
	{
		return;
	}
	bool lIsPhysicsStopped;
	v_get(lIsPhysicsStopped, =, GetSettings(), RTVAR_PHYSICS_HALT, false);
	if (lIsPhysicsStopped)
	{
		return;
	}
	if (lPhysics->GetPhysicsType() == Tbc::ChunkyPhysics::DYNAMIC && lPhysics->GetGuideMode() >= Tbc::ChunkyPhysics::GUIDE_EXTERNAL)
	{
		float lStabilityFactor = 1;
		int lBodyIndex = 0;
		const float lMicroStepFactor = mManager->GetGameManager()->GetTimeManager()->GetDesiredMicroSteps() / 18.0f;
		for (size_t x = 0; x < lClass->GetTagCount(); ++x)
		{
			const Tbc::ChunkyClass::Tag& lTag = lClass->GetTag(x);
			if (lTag.mTagName == _T("upright_stabilizer"))
			{
				if (lTag.mFloatValueList.size() != 1 ||
					lTag.mStringValueList.size() != 0 ||
					lTag.mBodyIndexList.size() != 1 ||
					lTag.mEngineIndexList.size() != 0 ||
					lTag.mMeshIndexList.size() != 0)
				{
					mLog.Errorf(_T("The upright_stabilizer tag '%s' has the wrong # of parameters."), lTag.mTagName.c_str());
					deb_assert(false);
					return;
				}
				lStabilityFactor = lTag.mFloatValueList[0];
				lBodyIndex = lTag.mBodyIndexList[0];
				Tbc::PhysicsEngine::UprightStabilize(mManager->GetGameManager()->GetPhysicsManager(),
					lPhysics, lPhysics->GetBoneGeometry(lBodyIndex), GetMass()*lStabilityFactor*lMicroStepFactor, 1);
			}
			else if (lTag.mTagName == _T("forward_stabilizer"))
			{
				if (lTag.mFloatValueList.size() != 1 ||
					lTag.mStringValueList.size() != 0 ||
					lTag.mBodyIndexList.size() != 1 ||
					lTag.mEngineIndexList.size() != 0 ||
					lTag.mMeshIndexList.size() != 0)
				{
					mLog.Errorf(_T("The forward_stabilizer tag '%s' has the wrong # of parameters."), lTag.mTagName.c_str());
					deb_assert(false);
					return;
				}
				lStabilityFactor = lTag.mFloatValueList[0];
				lBodyIndex = lTag.mBodyIndexList[0];
				Tbc::PhysicsEngine::ForwardStabilize(mManager->GetGameManager()->GetPhysicsManager(),
					lPhysics, lPhysics->GetBoneGeometry(lBodyIndex), GetMass()*lStabilityFactor*lMicroStepFactor, 1);
			}
		}
	}
}



void CppContextObject::StartLoading()
{
	deb_assert(mClassResource == 0);
	mClassResource = new UserClassResource();
	const str lAssetName = GetClassId()+_T(".class");
	mClassResource->Load(GetResourceManager(), lAssetName,
		UserClassResource::TypeLoadCallback(this, &CppContextObject::OnLoadClass));
}



void CppContextObject::SetAllowNetworkLogic(bool pAllow)
{
	mAllowNetworkLogic = pAllow;
}



Tbc::ChunkyPhysics* CppContextObject::GetPhysics() const
{
	if (mPhysicsResource && mPhysicsResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		return (mPhysicsResource->GetData());
	}
	return (0);
}

void CppContextObject::CreatePhysics(Tbc::ChunkyPhysics* pPhysics)
{
	deb_assert(mPhysicsResource == 0);
	static int lPhysicsCounter = 0;
	str lPhysicsName = strutil::Format(_T("TestPhysics%i"), lPhysicsCounter);
	str lPhysicsRefName = lPhysicsName+_T("Ref");
	PhysicsSharedInitData lInitData(mPosition.mPosition.mTransformation, mPosition.mPosition.mVelocity, mPhysicsOverride,
		mManager->GetGameManager()->GetPhysicsManager(), mManager->GetGameManager()->GetTimeManager()->GetDesiredMicroSteps(), GetInstanceId());
	mPhysicsResource = new UserPhysicsReferenceResource(lInitData);
	UserPhysicsReferenceResource* lPhysicsRef = mPhysicsResource;
	PhysicsSharedResource* lPhysicsRefResource = (PhysicsSharedResource*)lPhysicsRef->CreateResource(GetResourceManager(), lPhysicsRefName);
	lPhysicsRef->SetResource(lPhysicsRefResource);
	Cure::UserResource::LoadCallback lCallbackCast;
	lCallbackCast.SetMemento(UserPhysicsReferenceResource::TypeLoadCallback(this, &CppContextObject::OnLoadPhysics).GetMemento());
	lPhysicsRefResource->AddCaller(lPhysicsRef, lCallbackCast);
	PhysicsSharedResource::ClassResource* lPhysics = lPhysicsRefResource->GetParent();
	PhysicsResource* lPhysicsResource = (PhysicsResource*)lPhysics->CreateResource(GetResourceManager(), lPhysicsName);
	lPhysicsResource->SetIsUnique(true);
	lPhysics->SetResource(lPhysicsResource);
	lPhysicsResource->SetRamDataType(pPhysics);
	Tbc::ChunkyPhysics* lCopy = new Tbc::ChunkyPhysics(*pPhysics);
	lPhysicsRefResource->SetRamDataType(lCopy);
	lPhysicsResource->SetLoadState(Cure::RESOURCE_LOAD_IN_PROGRESS);	// Handle pushing to physics engine in postprocessing by some other thread at a later stage.
	lPhysicsRefResource->SetLoadState(Cure::RESOURCE_LOAD_IN_PROGRESS);	// We're waiting for the root resource to get loaded.
	GetResourceManager()->AddLoaded(lPhysics);
	GetResourceManager()->AddLoaded(lPhysicsRef);
}

const Tbc::ChunkyClass* CppContextObject::GetClass() const
{
	if (mClassResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		return (mClassResource->GetRamData());
	}
	return (0);
}

const Tbc::ChunkyClass::Tag* CppContextObject::FindTag(const str& pTagType, int pFloatValueCount, int pStringValueCount, const std::vector<int>* pTriggerIndexArray) const
{
	const Tbc::ChunkyClass* lClass = GetClass();
	for (size_t x = 0; x < lClass->GetTagCount(); ++x)
	{
		const Tbc::ChunkyClass::Tag& lTag = lClass->GetTag(x);
		if (lTag.mTagName == pTagType &&
			(pFloatValueCount < 0 || lTag.mFloatValueList.size() == (size_t)pFloatValueCount) &&
			(pStringValueCount < 0 || lTag.mStringValueList.size() == (size_t)pStringValueCount) &&
			(!pTriggerIndexArray || (lTag.mBodyIndexList.size() == pTriggerIndexArray->size() &&
				std::equal(lTag.mBodyIndexList.begin(), lTag.mBodyIndexList.end(), pTriggerIndexArray->begin()))))
		{
			return &lTag;
		}
	}
	return 0;
}

void CppContextObject::SetTagIndex(int pIndex)
{
	(void)pIndex;
}



void CppContextObject::SetForceLoadUnique(bool pLoadUnique)
{
	mForceLoadUnique = pLoadUnique;
}

void CppContextObject::StartLoadingPhysics(const str& pPhysicsName)
{
	deb_assert(mPhysicsResource == 0);
	const str lInstanceId = strutil::IntToString(GetInstanceId(), 10);
	const str lAssetName = pPhysicsName + _T(".phys;") + lInstanceId.c_str();
	PhysicsSharedInitData lInitData(mPosition.mPosition.mTransformation, mPosition.mPosition.mVelocity, mPhysicsOverride,
		mManager->GetGameManager()->GetPhysicsManager(), mManager->GetGameManager()->GetTimeManager()->GetDesiredMicroSteps(), GetInstanceId());
	mPhysicsResource = new UserPhysicsReferenceResource(lInitData);
	if (!mForceLoadUnique)
	{
		mPhysicsResource->Load(GetResourceManager(), lAssetName,
			UserPhysicsReferenceResource::TypeLoadCallback(this, &CppContextObject::OnLoadPhysics), false);
	}
	else
	{
		mPhysicsResource->LoadUnique(GetResourceManager(), lAssetName,
			UserPhysicsReferenceResource::TypeLoadCallback(this, &CppContextObject::OnLoadPhysics));
	}
}

bool CppContextObject::TryComplete()
{
	if (!mPhysicsResource || IsLoaded())
	{
		return (false);
	}

	// This is the way to post-process physics (create physical bodies) only after the rest have been created, so
	// that they are not created on different frames.
	if (mPhysicsResource->GetLoadState() == RESOURCE_LOAD_COMPLETE)
	{
		PhysicsSharedResource* lPhysicsResource = (PhysicsSharedResource*)mPhysicsResource->GetConstResource();
		if (lPhysicsResource->InjectPostProcess() == RESOURCE_LOAD_COMPLETE)
		{
			SetPhysics(mPhysicsResource->GetData());
			if (GetAllowNetworkLogic())
			{
				SetupChildHandlers();
			}
		}
	}

	if (mPhysicsResource->GetLoadState() == RESOURCE_LOAD_COMPLETE)
	{
		for (Array::iterator x = mChildArray.begin(); x != mChildArray.end(); ++x)
		{
			CppContextObject* lChild = (CppContextObject*)*x;
			lChild->TryComplete();
		}
		if (GetPhysics() && GetPhysics()->GetEngineCount() > 0 && GetManager())
		{
			GetManager()->EnableMicroTickCallback(this);	// Used for engine force applications each micro frame.
		}
		SetLoadResult(true);
		return (true);
	}
	else if (mPhysicsResource->GetLoadState() != RESOURCE_LOAD_IN_PROGRESS)
	{
		SetLoadResult(false);
		return (true);
	}
	return (false);
}

void CppContextObject::SetupChildHandlers()
{
	Parent::SetupChildHandlers();

	if (!GetClass())
	{
		return;
	}
	const int lTagCount = GetClass()->GetTagCount();
	for (int x = 0; x < lTagCount; ++x)
	{
		const Tbc::ChunkyClass::Tag& lTag = GetClass()->GetTag(x);
		CppContextObject* lHandlerChild = (CppContextObject*)GetManager()->GetGameManager()->CreateLogicHandler(lTag.mTagName);
		if (!lHandlerChild)
		{
			continue;
		}
		AddChild(lHandlerChild);
		lHandlerChild->SetTagIndex(x);
	}
}



void CppContextObject::OnMicroTick(float pFrameTime)
{
	if (mPhysics && GetManager())
	{
		const bool lNeedsSteeringHelp = (GetAttributeFloatValue(_T("float_is_child")) > 0.75f);
		int lAccIndex = GetPhysics()->GetEngineIndexFromControllerIndex(GetPhysics()->GetEngineCount()-1, -1, 0);
		const int lTurnIndex = GetPhysics()->GetEngineIndexFromControllerIndex(0, 1, 1);
		if (lNeedsSteeringHelp && lAccIndex >= 0 && lTurnIndex >= 0)
		{
			// Young children have the possibility of just pressing left/right which will cause
			// a forward motion in the currently used vehicle.
			Tbc::PhysicsEngine* lAcc = GetPhysics()->GetEngine(lAccIndex);
			const Tbc::PhysicsEngine* lTurn = GetPhysics()->GetEngine(lTurnIndex);
			const float lPowerFwdRev = lAcc->GetValue();
			const float lPowerLR = lTurn->GetValue();
			float lAutoTurnAccValue = 0;
			if (Math::IsEpsEqual(lPowerFwdRev, 0.0f, 0.05f) && !Math::IsEpsEqual(lPowerLR, 0.0f, 0.05f))
			{
				const float lIntensity = lAcc->GetIntensity();
				lAutoTurnAccValue = Math::Clamp(10.0f*(0.2f-lIntensity), 0.0f, 1.0f);
			}
			// Throttle up all relevant acc engines.
			for (;;)
			{
				lAcc->ForceSetValue(Tbc::PhysicsEngine::ASPECT_LOCAL_PRIMARY, lAutoTurnAccValue);
				lAccIndex = GetPhysics()->GetEngineIndexFromControllerIndex(lAccIndex-1, -1, 0);
				if (lAccIndex < 0)
				{
					break;
				}
				lAcc = GetPhysics()->GetEngine(lAccIndex);
			}
		}
		mPhysics->OnMicroTick(GetManager()->GetGameManager()->GetPhysicsManager(), pFrameTime);
	}
}

void CppContextObject::OnAlarm(int /*pAlarmId*/, void* /*pExtraData*/)
{
}

void CppContextObject::OnTrigger(Tbc::PhysicsManager::TriggerID pTriggerId, ContextObject* pOtherObject, Tbc::PhysicsManager::BodyID pBodyId, const vec3& pNormal)
{
	if (!GetAllowNetworkLogic())
	{
		return;
	}

	ContextObject* lChild = (ContextObject*)GetTrigger(pTriggerId);
	if (lChild)
	{
		lChild->OnTrigger(pTriggerId, pOtherObject, pBodyId, pNormal);
	}
	else
	{
		//mLog.Errorf(_T("Physical trigger not configured for logical trigging on %s."), GetClassId().c_str());
	}

	/*
	TODO: put this back when attaching objects to each other is working.
	ContextObject* lObject2 = mManager->GetGameManager()->GetPhysicsManager()->GetForceFeedbackListenerId(pBody2);
	if (mManager->GetGameManager()->IsServer() && lObject2)
	{
		AttachToObject(pBody1, lObject2, pBody2);
	}*/
}



void CppContextObject::OnForceApplied(ContextObject* pOtherObject,
	Tbc::PhysicsManager::BodyID pOwnBodyId, Tbc::PhysicsManager::BodyID pOtherBodyId,
	const vec3& pForce, const vec3& pTorque,
	const vec3& pPosition, const vec3& pRelativeVelocity)
{
	(void)pPosition;
	(void)pRelativeVelocity;

	if (!IsAttachedTo((ContextObject*)pOtherObject))
	{
		// TODO: replace by sensible values. Like dividing by mass, for instance.
		//if (pForce.GetLengthSquared() > 100 || pTorque.GetLengthSquared() > 10)
		{
			mManager->GetGameManager()->OnCollision(pForce, pTorque, pPosition, this, (ContextObject*)pOtherObject,
				pOwnBodyId, pOtherBodyId);
		}
	}
}



void CppContextObject::OnLoadClass(UserClassResource* pClassResource)
{
	Tbc::ChunkyClass* lClass = pClassResource->GetData();
	if (pClassResource->GetLoadState() != Cure::RESOURCE_LOAD_COMPLETE)
	{
		mLog.Errorf(_T("Could not load class '%s'."), pClassResource->GetName().c_str());
		deb_assert(false);
		GetManager()->PostKillObject(GetInstanceId());
		return;
	}
	else
	{
		StartLoadingPhysics(lClass->GetPhysicsBaseName());
	}
}

void CppContextObject::OnLoadPhysics(UserPhysicsReferenceResource* pPhysicsResource)
{
	if (pPhysicsResource->GetLoadState() != RESOURCE_LOAD_COMPLETE)
	{
		mLog.Errorf(_T("Could not load physics class '%s'."), pPhysicsResource->GetName().c_str());
		deb_assert(false);
		GetManager()->PostKillObject(GetInstanceId());
		return;
	}

	TryComplete();
}



bool CppContextObject::GetAllowNetworkLogic() const
{
	return mAllowNetworkLogic;
}



loginstance(GAME_CONTEXT_CPP, CppContextObject);



}
