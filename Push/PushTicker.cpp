
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "PushTicker.h"
#include "../Lepra/Include/SystemManager.h"
#include "../Life/LifeServer/MasterServerConnection.h"
#include "../UiCure/Include/UiGameUiManager.h"
#include "RtVar.h"
#include "PushDemo.h"
#include "PushViewer.h"



namespace Push
{



PushTicker::PushTicker(UiCure::GameUiManager* pUiManager, Cure::ResourceManager* pResourceManager, float pPhysicsRadius, int pPhysicsLevels, float pPhysicsSensitivity):
	Parent(pUiManager, pResourceManager, pPhysicsRadius, pPhysicsLevels, pPhysicsSensitivity),
	mIsPlayerCountViewActive(false),
	mDemoTime(0),
	mSunlight(0)
{
}

PushTicker::~PushTicker()
{
	CloseMainMenu();

	delete mSunlight;
	mSunlight = 0;
}



Sunlight* PushTicker::GetSunlight() const
{
	return mSunlight;
}



bool PushTicker::CreateSlave()
{
	return (Parent::CreateSlave(&PushTicker::CreateSlaveManager));
}

void PushTicker::OnSlavesKilled()
{
	DeleteServer();
	//assert(!mIsPlayerCountViewActive);
	mIsPlayerCountViewActive = true;
	mSlaveTopSplit = 1.0f;
	Parent::CreateSlave(&PushTicker::CreateViewer);
}



bool PushTicker::Reinitialize()
{
	delete mSunlight;
	mSunlight = 0;
	bool lOk = Parent::Reinitialize();
	mSunlight = new Sunlight(mUiManager);
	return lOk;
}



void PushTicker::BeginRender(Vector3DF& pColor)
{
	float lRealTimeRatio;
	CURE_RTVAR_GET(lRealTimeRatio, =(float), UiCure::GetSettings(), RTVAR_PHYSICS_RTR, 1.0);
	float lTimeOfDayFactor;
	CURE_RTVAR_GET(lTimeOfDayFactor, =(float), UiCure::GetSettings(), RTVAR_GAME_TIMEOFDAYFACTOR, 1.0);
	mSunlight->Tick(lRealTimeRatio * lTimeOfDayFactor);

	mSunlight->AddSunColor(pColor, 2);
	Parent::BeginRender(pColor);

	Vector3DF lColor(1.2f, 1.2f, 1.2f);
	mSunlight->AddSunColor(lColor, 1);
	Color lFillColor;
	lFillColor.Set(lColor.x, lColor.y, lColor.z, 1.0f);
	mUiManager->GetRenderer()->SetOutlineFillColor(lFillColor);
}

void PushTicker::CloseMainMenu()
{
	if (mIsPlayerCountViewActive)
	{
		DeleteSlave(mSlaveArray[0], false);
		mIsPlayerCountViewActive = false;
	}
}

bool PushTicker::QueryQuit()
{
	if (mDemoTime)
	{
		// We quit if user tried quitting twice or more, or demo time is over.
		return (SystemManager::GetQuitRequest() >= 2 || mDemoTime->QueryTimeDiff() > 30.0f);
	}

	if (Parent::QueryQuit())
	{
		for (int x = 0; x < 4; ++x)
		{
			DeleteSlave(mSlaveArray[x], false);
		}
		DeleteServer();
#ifdef LIFE_DEMO
		if (!mUiManager->CanRender())
		{
			return true;
		}
		CreateSlave(&PushTicker::CreateDemo);
		mDemoTime = new HiResTimer;
#else // !Demo
		return (true);
#endif // Demo / !Demo
	}
	return (false);
}



Life::GameClientSlaveManager* PushTicker::CreateSlaveManager(Life::GameClientMasterTicker* pMaster,
	Cure::TimeManager* pTime, Cure::RuntimeVariableScope* pVariableScope,
	Cure::ResourceManager* pResourceManager, UiCure::GameUiManager* pUiManager,
	int pSlaveIndex, const PixelRect& pRenderArea)
{
	PushManager* lGameManager = new PushManager(pMaster, pTime, pVariableScope, pResourceManager, pUiManager, pSlaveIndex, pRenderArea);
	lGameManager->SetMasterServerConnection(new Life::MasterServerConnection(pMaster->GetMasterServerConnection()->GetMasterAddress()));
	return lGameManager;
}

Life::GameClientSlaveManager* PushTicker::CreateViewer(Life::GameClientMasterTicker* pMaster,
	Cure::TimeManager* pTime, Cure::RuntimeVariableScope* pVariableScope,
	Cure::ResourceManager* pResourceManager, UiCure::GameUiManager* pUiManager,
	int pSlaveIndex, const PixelRect& pRenderArea)
{
	return new PushViewer(pMaster, pTime, pVariableScope, pResourceManager, pUiManager, pSlaveIndex, pRenderArea);
}

Life::GameClientSlaveManager* PushTicker::CreateDemo(Life::GameClientMasterTicker* pMaster,
	Cure::TimeManager* pTime, Cure::RuntimeVariableScope* pVariableScope,
	Cure::ResourceManager* pResourceManager, UiCure::GameUiManager* pUiManager,
	int pSlaveIndex, const PixelRect& pRenderArea)
{
#ifdef LIFE_DEMO
	return new PushDemo(pMaster, pTime, pVariableScope, pResourceManager, pUiManager, pSlaveIndex, pRenderArea);
#else // !Demo
	return new PushViewer(pMaster, pTime, pVariableScope, pResourceManager, pUiManager, pSlaveIndex, pRenderArea);
#endif // Demo / !Demo
}



LOG_CLASS_DEFINE(GAME, PushTicker);



}
