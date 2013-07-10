
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../Include/CppContextObject.h"
#include "../../Lepra/Include/LepraAssert.h"
#include "../../Lepra/Include/DiskFile.h"
#include "../../Lepra/Include/HashUtil.h"
#include "../../TBC/Include/ChunkyPhysics.h"
#include "../../TBC/Include/PhysicsEngine.h"
#include "../../TBC/Include/PhysicsTrigger.h"
#include "../Include/ContextManager.h"
#include "../Include/GameManager.h"
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



TBC::ChunkyPhysics::GuideMode CppContextObject::GetGuideMode() const
{
	if (GetPhysics())
	{
		return GetPhysics()->GetGuideMode();
	}
	return TBC::ChunkyPhysics::GUIDE_EXTERNAL;
}

void CppContextObject::StabilizeTick()
{
	const TBC::ChunkyPhysics* lPhysics = GetPhysics();
	const TBC::ChunkyClass* lClass = GetClass();
	if (!lPhysics || !lClass)
	{
		return;
	}
	if (lPhysics->GetPhysicsType() == TBC::ChunkyPhysics::DYNAMIC && lPhysics->GetGuideMode() >= TBC::ChunkyPhysics::GUIDE_EXTERNAL)
	{
		float lStabilityFactor = 1;
		int lBodyIndex = 0;
		for (size_t x = 0; x < lClass->GetTagCount(); ++x)
		{
			const TBC::ChunkyClass::Tag& lTag = lClass->GetTag(x);
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
				TBC::PhysicsEngine::UprightStabilize(mManager->GetGameManager()->GetPhysicsManager(),
					lPhysics, lPhysics->GetBoneGeometry(lBodyIndex), GetMass()*lStabilityFactor*5, 1);
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
				TBC::PhysicsEngine::ForwardStabilize(mManager->GetGameManager()->GetPhysicsManager(),
					lPhysics, lPhysics->GetBoneGeometry(lBodyIndex), GetMass()*lStabilityFactor*5, 1);
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



TBC::ChunkyPhysics* CppContextObject::GetPhysics() const
{
	if (mPhysicsResource && mPhysicsResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		return (mPhysicsResource->GetData());
	}
	return (0);
}

const TBC::ChunkyClass* CppContextObject::GetClass() const
{
	if (mClassResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		return (mClassResource->GetRamData());
	}
	return (0);
}

const TBC::ChunkyClass::Tag* CppContextObject::FindTag(const str& pTagType, int pFloatValueCount, int pStringValueCount, const std::vector<int>* pTriggerIndexArray) const
{
	const TBC::ChunkyClass* lClass = GetClass();
	for (size_t x = 0; x < lClass->GetTagCount(); ++x)
	{
		const TBC::ChunkyClass::Tag& lTag = lClass->GetTag(x);
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
	PhysicsSharedInitData lInitData(mPosition.mPosition.mTransformation, mPhysicsOverride, mManager->GetGameManager()->GetPhysicsManager(),
		mManager->GetGameManager()->GetTimeManager()->GetDesiredMicroSteps(), GetInstanceId());
	lInitData.mTransformation.SetOrientation(QuaternionF());
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
		const TBC::ChunkyClass::Tag& lTag = GetClass()->GetTag(x);
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
			TBC::PhysicsEngine* lAcc = GetPhysics()->GetEngine(lAccIndex);
			const TBC::PhysicsEngine* lTurn = GetPhysics()->GetEngine(lTurnIndex);
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
				lAcc->ForceSetValue(TBC::PhysicsEngine::ASPECT_LOCAL_PRIMARY, lAutoTurnAccValue);
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

void CppContextObject::OnTrigger(TBC::PhysicsManager::TriggerID pTriggerId, ContextObject* pBody, const Vector3DF& pNormal)
{
	if (!GetAllowNetworkLogic())
	{
		return;
	}

	ContextObject* lChild = (ContextObject*)GetTrigger(pTriggerId);
	if (lChild)
	{
		lChild->OnTrigger(pTriggerId, pBody, pNormal);
	}
	else
	{
		mLog.Errorf(_T("Physical trigger not configured for logical trigging on %s."), GetClassId().c_str());
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
	TBC::PhysicsManager::BodyID pOwnBodyId, TBC::PhysicsManager::BodyID pOtherBodyId,
	const Vector3DF& pForce, const Vector3DF& pTorque,
	const Vector3DF& pPosition, const Vector3DF& pRelativeVelocity)
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
	TBC::ChunkyClass* lClass = pClassResource->GetData();
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

	SetPhysics(pPhysicsResource->GetData());
	if (GetAllowNetworkLogic())
	{
		SetupChildHandlers();
	}
	TryComplete();
}



bool CppContextObject::GetAllowNetworkLogic() const
{
	return mAllowNetworkLogic;
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, CppContextObject);



}
