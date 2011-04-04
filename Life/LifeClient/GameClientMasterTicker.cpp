
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "GameClientMasterTicker.h"
#include <algorithm>
#include "../../Cure/Include/NetworkClient.h"
#include "../../Cure/Include/NetworkFreeAgent.h"
#include "../../Cure/Include/ResourceManager.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Cure/Include/TimeManager.h"
#include "../../Lepra/Include/Network.h"
#include "../../Lepra/Include/Number.h"
#include "../../Lepra/Include/Performance.h"
#include "../../Lepra/Include/PngLoader.h"
#include "../../Lepra/Include/SystemManager.h"
#include "../../UiCure/Include/UiGameUiManager.h"
#include "../../UiCure/Include/UiCppContextObject.h"
#include "../../UiCure/Include/UiRuntimeVariableName.h"
#include "../../UiTBC/Include/GUI/UiCenterLayout.h"
#include "../../UiTBC/Include/GUI/UiCenterLayout.h"
#include "../../UiTBC/Include/GUI/UiConsoleLogListener.h"
#include "../../UiTBC/Include/GUI/UiConsolePrompt.h"
#include "../../UiTBC/Include/GUI/UiDesktopWindow.h"
#include "../../UiTBC/Include/GUI/UiFloatingLayout.h"
#include "../LifeServer/MasterServerConnection.h"
#include "../LifeApplication.h"
#include "GameClientDemo.h"
#include "GameClientSlaveManager.h"
#include "GameClientViewer.h"
#include "RtVar.h"
#include "Sunlight.h"
#include "UiGameServerManager.h"
#include "UiRaceScore.h"

// TODO: remove!
#include "UiConsole.h"
#include "UiServerConsoleManager.h"



namespace Life
{



#define SET_PLAYER_COUNT	_T("set-player-count")



GameClientMasterTicker::GameClientMasterTicker(UiCure::GameUiManager* pUiManager, Cure::ResourceManager* pResourceManager):
	mUiManager(pUiManager),
	mResourceManager(pResourceManager),
	mIsPlayerCountViewActive(false),
	mServer(0),
	mMasterConnection(new MasterServerConnection),
	mFreeNetworkAgent(new Cure::NetworkFreeAgent),
	mRestartUi(false),
	mInitialized(false),
	mActiveWidth(0),
	mActiveHeight(0),
	mActiveSlaveCount(0),
	mSlaveTopSplit(1),
	mSlaveBottomSplit(1),
	mSlaveVSplit(1),
	mSlaveFade(0),
	mDemoTime(0),
	mSunlight(0)
{
	mSlaveArray.resize(4, 0);

	UiLepra::DisplayManager::EnableScreensaver(false);

	Cure::ContextObjectAttribute::SetCreator(Cure::ContextObjectAttribute::Factory(
		this, &GameClientMasterTicker::CreateObjectAttribute));

	mConsole = new ConsoleManager(mResourceManager, 0, UiCure::GetSettings(), 0, 0);
	mConsole->InitCommands();
	mConsole->GetConsoleCommandManager()->AddExecutor(
		new ConsoleExecutor<GameClientMasterTicker>(
			this, &GameClientMasterTicker::OnCommandLocal, &GameClientMasterTicker::OnCommandError));
	mConsole->GetConsoleCommandManager()->AddCommand(SET_PLAYER_COUNT);

	mConsole->ExecuteCommand(_T("execute-file -i Default.lsh"));
	mConsole->ExecuteCommand(_T("execute-file -i ") + Application::GetIoFile(_T("ClientBase"), _T("lsh")));
}

GameClientMasterTicker::~GameClientMasterTicker()
{
	UiLepra::DisplayManager::EnableScreensaver(true);

	{
		StashCalibration();

		mConsole->ExecuteCommand(_T("save-system-config-file 0 " + Application::GetIoFile(_T("ClientBase"), _T("lsh"))));
		delete (mConsole);
		mConsole = 0;
	}

	if (mUiManager->GetInputManager())
	{
		mUiManager->GetInputManager()->ReleaseAll();
		mUiManager->GetInputManager()->RemoveKeyCodeInputObserver(this);
	}

	ClosePlayerCountGui();

	SlaveArray::iterator x;
	for (x = mSlaveArray.begin(); x != mSlaveArray.end(); ++x)
	{
		GameClientSlaveManager* lSlave = *x;
		delete (lSlave);
	}
	mSlaveArray.clear();

	DeleteServer();

	mResourceManager = 0;
	mUiManager = 0;

	delete mMasterConnection;
	mMasterConnection = 0;
	delete mFreeNetworkAgent;
	mFreeNetworkAgent = 0;
}



bool GameClientMasterTicker::CreateSlave()
{
	return (CreateSlave(&GameClientMasterTicker::CreateSlaveManager));
}



bool GameClientMasterTicker::Tick()
{
	LEPRA_MEASURE_SCOPE(MasterTicker);

	GetTimeManager()->Tick();

	bool lOk = true;

	ScopeLock lLock(&mLock);

	SlaveArray::iterator x;

	{
		LEPRA_MEASURE_SCOPE(MasterServerConnectionTick);
		// If we're not running server: use free network interface for obtaining server list.
		if (!mServer)
		{
			mFreeNetworkAgent->Tick();
			float lConnectTimeout;
			CURE_RTVAR_GET(lConnectTimeout, =(float), UiCure::GetSettings(), RTVAR_NETWORK_CONNECT_TIMEOUT, 3.0);
			mMasterConnection->SetSocketInfo(mFreeNetworkAgent, lConnectTimeout);
		}
		mMasterConnection->Tick();
	}

	{
		LEPRA_MEASURE_SCOPE(BeginRenderAndInput);

		float lRealTimeRatio;
		CURE_RTVAR_GET(lRealTimeRatio, =(float), UiCure::GetSettings(), RTVAR_PHYSICS_RTR, 1.0);
		float lTimeOfDayFactor;
		CURE_RTVAR_GET(lTimeOfDayFactor, =(float), UiCure::GetSettings(), RTVAR_GAME_TIMEOFDAYFACTOR, 1.0);
		mSunlight->Tick(lRealTimeRatio * lTimeOfDayFactor);

		mLocalObjectSet.clear();
		for (x = mSlaveArray.begin(); x != mSlaveArray.end(); ++x)
		{
			GameClientSlaveManager* lSlave = *x;
			if (lSlave)
			{
				lSlave->AddLocalObjects(mLocalObjectSet);
			}
		}

		float r, g, b;
		CURE_RTVAR_GET(r, =(float), UiCure::GetSettings(), RTVAR_UI_3D_CLEARRED, 0.75);
		CURE_RTVAR_GET(g, =(float), UiCure::GetSettings(), RTVAR_UI_3D_CLEARGREEN, 0.80);
		CURE_RTVAR_GET(b, =(float), UiCure::GetSettings(), RTVAR_UI_3D_CLEARBLUE, 0.85);
		Vector3DF lColor(r, g, b);
		mSunlight->AddSunColor(lColor, 2);
		mUiManager->BeginRender(lColor);
		lColor.Set(1.2f, 1.2f, 1.2f);
		mSunlight->AddSunColor(lColor, 1);
		Color lFillColor;
		lFillColor.Set(lColor.x, lColor.y, lColor.z, 1.0f);
		mUiManager->GetRenderer()->SetOutlineFillColor(lFillColor);
		mUiManager->InputTick();
	}

	if (mServer)
	{
		LEPRA_MEASURE_SCOPE(ServerBeginTick);
		mServer->BeginTick();
	}

	{
		LEPRA_MEASURE_SCOPE(BeginTickSlaves);

		// Kickstart physics so no slaves have to wait too long for completion.
		int lSlaveIndex = 0;
		for (x = mSlaveArray.begin(); lOk && x != mSlaveArray.end(); ++x)
		{
			GameClientSlaveManager* lSlave = *x;
			if (lSlave)
			{
				mUiManager->GetSoundManager()->SetCurrentListener(lSlaveIndex, mActiveSlaveCount);
				lOk = lSlave->BeginTick();
				++lSlaveIndex;
			}
		}
	}

	{
		//LEPRA_MEASURE_SCOPE(RenderSlaves);

		mUiManager->GetRenderer()->ClearDebugInfo();

		// Start rendering machine directly afterwards.
		int lSlaveIndex = 0;
		for (x = mSlaveArray.begin(); lOk && x != mSlaveArray.end(); ++x)
		{
			GameClientSlaveManager* lSlave = *x;
			if (lSlave)
			{
				mUiManager->GetSoundManager()->SetCurrentListener(lSlaveIndex, mActiveSlaveCount);
				++lSlaveIndex;
				if (mUiManager->CanRender())
				{
					lOk = lSlave->Render();
				}
			}
		}
	}

	{
		LEPRA_MEASURE_SCOPE(Paint);
		if (mUiManager->CanRender())
		{
			mUiManager->GetRenderer()->EnableAllLights(false);
			UiTbc::Renderer::LightID lLightId = mUiManager->GetRenderer()->AddDirectionalLight(
				UiTbc::Renderer::LIGHT_STATIC, Vector3DF(0, 1, -1), WHITE, 1.5f, 160);
			mUiManager->Paint();
			for (x = mSlaveArray.begin(); lOk && x != mSlaveArray.end(); ++x)
			{
				GameClientSlaveManager* lSlave = *x;
				if (lSlave)
				{
					lOk = lSlave->Paint();
				}
			}
			mUiManager->GetRenderer()->RemoveLight(lLightId);
			mUiManager->GetRenderer()->EnableAllLights(true);
		}
	}

	{
		LEPRA_MEASURE_SCOPE(DrawGraph);
		if (mUiManager->CanRender())
		{
			DrawDebugData();
			DrawPerformanceLineGraph2d();
		}
	}

	if (mServer)
	{
		//LEPRA_MEASURE_SCOPE(ServerEndTick);
		mServer->EndTick();
	}

	{
		LEPRA_MEASURE_SCOPE(SlavesEndTick);
		for (x = mSlaveArray.begin(); lOk && x != mSlaveArray.end(); ++x)
		{
			GameClientSlaveManager* lSlave = *x;
			if (lSlave)
			{
				lOk = lSlave->EndTick();
			}
		}
	}

	{
		LEPRA_MEASURE_SCOPE(ResourceTick);
		// This must be synchronous. The reason is that it may add objects to "script" or "physics" enginges,
		// as well as upload data to the GPU and so forth; parallelization here will certainly cause threading
		// errors.
		mResourceManager->Tick();
	}

	{
		LEPRA_MEASURE_SCOPE(UiEndRender);
		mUiManager->EndRender();
	}

	{
		//LEPRA_MEASURE_SCOPE(UpdateSlaveLayout);
		if (mActiveWidth != mUiManager->GetDisplayManager()->GetWidth() ||
			mActiveHeight != mUiManager->GetDisplayManager()->GetHeight())
		{
			mActiveWidth = mUiManager->GetDisplayManager()->GetWidth();
			mActiveHeight = mUiManager->GetDisplayManager()->GetHeight();
		}
		UpdateSlaveLayout();
	}

	if (mRestartUi)
	{
		if (!Reinitialize())
		{
			mLog.Fatal(_T("Failure to re-initialize UI! Going down now!"));
			SystemManager::AddQuitRequest(+1);
		}
		mRestartUi = false;
	}
	else
	{
		//LEPRA_MEASURE_SCOPE(RunYieldCommand);
		mConsole->ExecuteYieldCommand();
	}

	return (lOk);
}

void GameClientMasterTicker::PollRoundTrip()
{
	// 1. Client push on network.
	// 2. Server pull on network.
	// 3. Server push on network.
	// 4. Client pull on network.
	SlaveArray::iterator x;
	for (x = mSlaveArray.begin(); x != mSlaveArray.end(); ++x)
	{
		GameClientSlaveManager* lSlave = *x;
		if (lSlave)
		{
			ScopeLock lLock(lSlave->GetTickLock());
			lSlave->TickNetworkOutput();
		}
	}
	if (mServer)
	{
		ScopeLock lLock(mServer->GetTickLock());
		mServer->GameServerManager::TickInput();
		mServer->TickNetworkOutput();
	}
	for (x = mSlaveArray.begin(); x != mSlaveArray.end(); ++x)
	{
		GameClientSlaveManager* lSlave = *x;
		if (lSlave)
		{
			ScopeLock lLock(lSlave->GetTickLock());
			lSlave->TickNetworkInput();
		}
	}
}



bool GameClientMasterTicker::StartResetUi()
{
	mRestartUi = true;
	return (true);
}

bool GameClientMasterTicker::WaitResetUi()
{
	for (int x = 0; mRestartUi && x < 50; ++x)
	{
		Thread::Sleep(0.1);
	}
	return (!mRestartUi);
}

bool GameClientMasterTicker::IsFirstSlave(const GameClientSlaveManager* pSlave) const
{
	if (mSlaveArray.empty())
	{
		return false;
	}
	SlaveArray::const_iterator x = mSlaveArray.begin();
	for (; x != mSlaveArray.end(); ++x)
	{
		GameClientSlaveManager* lSlave = *x;
		if (lSlave == pSlave)
		{
			return true;
		}
		if (lSlave)
		{
			return false;
		}
	}
	assert(false);
	return true;
}

bool GameClientMasterTicker::IsLocalObject(Cure::GameObjectId pInstanceId) const
{
	return (mLocalObjectSet.find(pInstanceId) != mLocalObjectSet.end());
}

void GameClientMasterTicker::GetSiblings(Cure::GameObjectId pObjectId, Cure::ContextObject::Array& pSiblingArray) const
{
	SlaveArray::const_iterator x;
	for (x = mSlaveArray.begin(); x != mSlaveArray.end(); ++x)
	{
		GameClientSlaveManager* lSlave = *x;
		if (lSlave)
		{
			lSlave->DoGetSiblings(pObjectId, pSiblingArray);
		}
	}
}



PixelRect GameClientMasterTicker::GetRenderArea() const
{
	return (PixelRect(0, 0, mUiManager->GetDisplayManager()->GetWidth()-1,
		mUiManager->GetDisplayManager()->GetHeight()-1));
}

float GameClientMasterTicker::UpdateFrustum(float pFov)
{
	const PixelRect lRenderArea(0, 0, mUiManager->GetDisplayManager()->GetWidth(), mUiManager->GetDisplayManager()->GetHeight());
	return UpdateFrustum(pFov, lRenderArea);
}

float GameClientMasterTicker::UpdateFrustum(float pFov, const PixelRect& pRenderArea)
{
	pFov *= 1/3.0f;
	pFov = pFov*2 + pFov*pRenderArea.GetWidth()/pRenderArea.GetHeight();

	float lClipNear;
	float lClipFar;
	CURE_RTVAR_GET(lClipNear, =(float), UiCure::GetSettings(), RTVAR_UI_3D_CLIPNEAR, 0.1);
	CURE_RTVAR_GET(lClipFar, =(float), UiCure::GetSettings(), RTVAR_UI_3D_CLIPFAR, 3000.0);
	mUiManager->GetRenderer()->SetViewFrustum(pFov, lClipNear, lClipFar);
	return (pFov);
}



void GameClientMasterTicker::PreLogin(const str& pServerAddress)
{
	bool lIsLocalServer = false;
	const str lServerUrl = strutil::Split(pServerAddress, _T(":"), 1)[0];
	IPAddress lServerIpAddress;
	IPAddress lExternalIpAddress;
	if (Network::ResolveHostname(lServerUrl, lServerIpAddress) && Network::ResolveHostname(_T(""), lExternalIpAddress))
	{
		const str lServerIp = lServerIpAddress.GetAsString();
		if (lServerIp == _T("127.0.0.1") ||
			lServerIp == _T("0.0.0.0") ||
			lServerIpAddress == lExternalIpAddress)
		{
			lIsLocalServer = true;
		}
	}
	if (lIsLocalServer && !mServer)
	{
		Cure::RuntimeVariableScope* lVariableScope = new Cure::RuntimeVariableScope(UiCure::GetSettings());
		UiGameServerManager* lServer = new UiGameServerManager(GetTimeManager(), lVariableScope, mResourceManager, mUiManager, PixelRect(0, 0, 100, 100));
		lServer->StartConsole(new UiTbc::ConsoleLogListener, new UiTbc::ConsolePrompt);
		if (!lServer->Initialize(mMasterConnection))
		{
			delete lServer;
			lServer = 0;
		}
		if (lServer)
		{
			ScopeLock lLock(&mLock);
			if (!mServer)
			{
				mServer = lServer;
			}
			else
			{
				delete lServer;
			}
		}
	}
	else if (!lIsLocalServer && mServer)
	{
		DeleteServer();
	}
}

bool GameClientMasterTicker::IsLocalServer() const
{
	return mServer != 0;
}

void GameClientMasterTicker::OnExit()
{
	mLog.Headline(_T("Number of players not picked, quitting."));
	SystemManager::AddQuitRequest(+1);
	ClosePlayerCountGui();
}

void GameClientMasterTicker::OnSetPlayerCount(int pPlayerCount)
{
	ClosePlayerCountGui();

	for (int x = 0; x < pPlayerCount; ++x)
	{
		CreateSlave();
	}

	if (!mServer)
	{
		// If client descided we won't run local server, then we have no need for
		// a master connection any more. Was only used to download server list anyway.
		mMasterConnection->GraceClose(0.1, false);
	}
}

void GameClientMasterTicker::DownloadServerList()
{
	mMasterConnection->RequestServerList(_T(""));
}

const MasterServerConnection* GameClientMasterTicker::GetMasterConnection() const
{
	return mMasterConnection;
}



Sunlight* GameClientMasterTicker::GetSunlight() const
{
	return mSunlight;
}



GameClientSlaveManager* GameClientMasterTicker::CreateSlaveManager(GameClientMasterTicker* pMaster,
	Cure::TimeManager* pTime, Cure::RuntimeVariableScope* pVariableScope,
	Cure::ResourceManager* pResourceManager, UiCure::GameUiManager* pUiManager,
	int pSlaveIndex, const PixelRect& pRenderArea)
{
	return new GameClientSlaveManager(pMaster, pTime, pVariableScope, pResourceManager, pUiManager, pSlaveIndex, pRenderArea);
}

GameClientSlaveManager* GameClientMasterTicker::CreateViewer(GameClientMasterTicker* pMaster,
	Cure::TimeManager* pTime, Cure::RuntimeVariableScope* pVariableScope,
	Cure::ResourceManager* pResourceManager, UiCure::GameUiManager* pUiManager,
	int pSlaveIndex, const PixelRect& pRenderArea)
{
	return new GameClientViewer(pMaster, pTime, pVariableScope, pResourceManager, pUiManager, pSlaveIndex, pRenderArea);
}

GameClientSlaveManager* GameClientMasterTicker::CreateDemo(GameClientMasterTicker* pMaster,
	Cure::TimeManager* pTime, Cure::RuntimeVariableScope* pVariableScope,
	Cure::ResourceManager* pResourceManager, UiCure::GameUiManager* pUiManager,
	int pSlaveIndex, const PixelRect& pRenderArea)
{
#ifdef LIFE_DEMO
	return new GameClientDemo(pMaster, pTime, pVariableScope, pResourceManager, pUiManager, pSlaveIndex, pRenderArea);
#else // !Demo
	return new GameClientViewer(pMaster, pTime, pVariableScope, pResourceManager, pUiManager, pSlaveIndex, pRenderArea);
#endif // Demo / !Demo
}

Cure::ContextObjectAttribute* GameClientMasterTicker::CreateObjectAttribute(Cure::ContextObject* pObject, const str& pAttributeName)
{
	ScreenPart* lScreenPart = this;
	SlaveArray::iterator x;
	for (x = mSlaveArray.begin(); x != mSlaveArray.end(); ++x)
	{
		GameClientSlaveManager* lSlave = *x;
		if (lSlave)
		{
			if (lSlave->IsOwned(pObject->GetInstanceId()))
			{
				lScreenPart = lSlave;
				break;
			}
		}
	}
	if (strutil::StartsWith(pAttributeName, _T("race_timer_")))
	{
		return new UiRaceScore(pObject, pAttributeName, lScreenPart, mUiManager, pAttributeName);
	}
	return Life::CreateObjectAttribute(pObject, pAttributeName);
}

bool GameClientMasterTicker::CreateSlave(SlaveFactoryMethod pCreate)
{
	const PixelRect lRenderArea(0, 0, mUiManager->GetDisplayManager()->GetWidth(), mUiManager->GetDisplayManager()->GetHeight());
	ScopeLock lLock(&mLock);
	bool lOk = (mActiveSlaveCount < 4);
	if (lOk)
	{
		int lFreeSlaveIndex = 0;
		for (; lFreeSlaveIndex < 4; ++lFreeSlaveIndex)
		{
			if (!mSlaveArray[lFreeSlaveIndex])
			{
				break;
			}
		}
		assert(lFreeSlaveIndex < 4);
		Cure::RuntimeVariableScope* lVariables = new Cure::RuntimeVariableScope(UiCure::GetSettings());
		GameClientSlaveManager* lSlave = pCreate(this, GetTimeManager(), lVariables, mResourceManager,
			mUiManager, lFreeSlaveIndex, lRenderArea);
		AddSlave(lSlave);
		if (mInitialized)
		{
			lOk = lSlave->Open();
			if (lOk && mConsole->GetGameManager() == 0)
			{
				mConsole->SetGameManager(lSlave);
			}
		}
	}
	else
	{
		mLog.AError("Could not create another split screen player - not supported.");
	}
	return (lOk);
}

void GameClientMasterTicker::AddSlave(GameClientSlaveManager* pSlave)
{
	{
		ScopeLock lLock(&mLock);
		pSlave->LoadSettings();
		assert(mSlaveArray[pSlave->GetSlaveIndex()] == 0);
		mSlaveArray[pSlave->GetSlaveIndex()] = pSlave;
		pSlave->GetContext()->SetLocalRange(pSlave->GetSlaveIndex(), mSlaveArray.size());
		++mActiveSlaveCount;
	}
}

void GameClientMasterTicker::DeleteSlave(GameClientSlaveManager* pSlave, bool pAllowMainMenu)
{
	if (!pSlave)
	{
		return;
	}

	ScopeLock lLock(&mLock);
	assert(mSlaveArray[pSlave->GetSlaveIndex()]);
	mSlaveArray[pSlave->GetSlaveIndex()] = 0;
	delete (pSlave);
	if (mConsole->GetGameManager() == pSlave)
	{
		mConsole->SetGameManager(0);
	}
	if (--mActiveSlaveCount == 0)
	{
		//mResourceManager->ForceFreeCache();
		if (pAllowMainMenu)
		{
			CreatePlayerCountWindow();
		}
	}
}

void GameClientMasterTicker::DeleteServer()
{
	if (mServer)
	{
		GameServerManager* lServer = mServer;
		mServer = 0;
		delete lServer;
	}
}


bool GameClientMasterTicker::Initialize()
{
	bool lOk = Reinitialize();
	if (lOk)
	{
		if (!ApplyCalibration())
		{
			mLog.AError("An error ocurred when applying calibration.");
		}

		bool lShowLogo;
		CURE_RTVAR_GET(lShowLogo, =, UiCure::GetSettings(), RTVAR_GAME_ENABLESTARTLOGO, true);
		if (lShowLogo)
		{
			Cure::UserRamImageResource* lLogo = new Cure::UserRamImageResource;
			Cure::UserResourceOwner<Cure::UserRamImageResource> lLogoHolder(lLogo, mResourceManager, _T("Data/megaphone.png"));
			UiCure::UserSound2dResource* lLogoSound = new UiCure::UserSound2dResource(mUiManager, UiLepra::SoundManager::LOOP_NONE);
			Cure::UserResourceOwner<UiCure::UserSound2dResource> lLogoSoundHolder(lLogoSound, mResourceManager, _T("Data/logo_trumpet.wav"));
			for (int x = 0; x < 1000; ++x)
			{
				mResourceManager->Tick();
				if (lLogo->GetLoadState() != Cure::RESOURCE_LOAD_IN_PROGRESS &&
					lLogoSound->GetLoadState() != Cure::RESOURCE_LOAD_IN_PROGRESS)
				{
					break;
				}
				Thread::Sleep(0.001);
			}
			if (lLogo->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE &&
				lLogoSound->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
			{
				mUiManager->GetSoundManager()->Play(lLogoSound->GetData(), 1, 1);

				UiLepra::Canvas& lCanvas = *lLogo->GetRamData();
				const UiTbc::Painter::ImageID lImageId = mUiManager->GetDesktopWindow()->GetImageManager()->AddImage(lCanvas, UiTbc::GUIImageManager::STRETCHED, UiTbc::GUIImageManager::NO_BLEND, 255);
				UiTbc::RectComponent lRect(lImageId, _T("logo"));
				mUiManager->AssertDesktopLayout(new UiTbc::FloatingLayout, 0);
				mUiManager->GetDesktopWindow()->AddChild(&lRect, 0, 0, 0);
				const unsigned lWidth = mUiManager->GetDisplayManager()->GetWidth();
				const unsigned lHeight = mUiManager->GetDisplayManager()->GetHeight();
				lRect.SetPreferredSize(lCanvas.GetWidth(), lCanvas.GetHeight());
				const unsigned lTargetX = lWidth/2 - lCanvas.GetWidth()/2;
				const unsigned lTargetY = lHeight/2 - lCanvas.GetHeight()/2;
				mUiManager->GetRenderer()->ResetClippingRect();
				Color lColor;
				mUiManager->GetRenderer()->SetClearColor(Color());
				mUiManager->GetDisplayManager()->SetVSyncEnabled(true);

				const float lMin = 0;
				const float lMax = 26;
				const int lStepCount = 50;
				const float lBaseStep = (lMax-lMin)/(float)lStepCount;
			        float lBase = -lMax;
			        int lCount = 0;
				const int lTotalFrameCount = 600;
				for (lCount = 0; lCount <= lTotalFrameCount && SystemManager::GetQuitRequest() == 0; ++lCount)
		                {
					if (lCount < lStepCount || lCount > lTotalFrameCount-lStepCount)
					{
			                        lBase += lBaseStep;
					}
					int lMovement = (int)(::fabs(lBase)*lBase*3);
					lRect.SetPos(lTargetX+lMovement, lTargetY);

					mUiManager->GetRenderer()->Clear();
					mUiManager->Paint();
					mUiManager->GetDisplayManager()->UpdateScreen();

					if (SystemManager::GetSleepResolution() <= 0.01)
					{
						Thread::Sleep(0.01);
					}
					else
					{
						HiResTimer lTimer;
						while (lTimer.QueryTimeDiff() < 0.01)
						{
							Thread::YieldCpu();
						}
					}
					mUiManager->InputTick();
					lOk = (SystemManager::GetQuitRequest() <= 0);

					if (lCount == lStepCount)
					{
						lBase = 0;
					}
					else if (lCount == lTotalFrameCount-lStepCount)
					{
						lBase = lMin;
					}
				}
				mUiManager->GetDesktopWindow()->RemoveChild(&lRect, 0);
				mUiManager->GetDesktopWindow()->GetImageManager()->RemoveImage(lImageId);
			}
		}
		mResourceManager->ForceFreeCache();

		CreatePlayerCountWindow();
	}
	return (lOk);
}

void GameClientMasterTicker::CreatePlayerCountWindow()
{
	DeleteServer();
	//assert(!mIsPlayerCountViewActive);
	mIsPlayerCountViewActive = true;
	mSlaveTopSplit = 1.0f;
	CreateSlave(&GameClientMasterTicker::CreateViewer);
}

bool GameClientMasterTicker::Reinitialize()
{
	mInitialized = false;

	// Close all gfx/snd stuff.
	if (mUiManager->GetInputManager())
	{
		mUiManager->GetInputManager()->RemoveKeyCodeInputObserver(this);
	}
	SlaveArray::iterator x;
	for (x = mSlaveArray.begin(); x != mSlaveArray.end(); ++x)
	{
		GameClientSlaveManager* lSlave = *x;
		if (lSlave)
		{
			lSlave->Close();
		}
	}
	DeleteServer();
	delete mSunlight;
	mSunlight = 0;
	mConsole->SetGameManager(0);
	mResourceManager->StopClear();
	mUiManager->Close();
	SystemManager::AddQuitRequest(-1);

	// (Re)open.
	bool lOk = mResourceManager->InitDefault();
	if (lOk)
	{
		lOk = mUiManager->Open();
	}
	if (lOk)
	{
		mUiManager->GetDesktopWindow()->CreateLayer(new UiTbc::FloatingLayout());

		mSunlight = new Sunlight(mUiManager);
		mUiManager->GetInputManager()->AddKeyCodeInputObserver(this);
	}
	if (lOk)
	{
		/*mUiManager->GetInputManager()->ActivateAll();
		mUiManager->GetInputManager()->PollEvents();
		const UiLepra::InputManager::DeviceList& lDeviceList = mUiManager->GetInputManager()->GetDeviceList();
		UiLepra::InputManager::DeviceList::const_iterator y = lDeviceList.begin();
		for (; y != lDeviceList.end(); ++y)
		{
			UiLepra::InputDevice* lDevice = (*y);
			mLog.Infof(_T("Found input device %s with %u digital and %u analogue input elements."),
				lDevice->GetIdentifier().c_str(), lDevice->GetNumDigitalElements(),
				lDevice->GetNumAnalogueElements());
			if (lDevice->GetNumElements() < 30)
			{
				mLog.AInfo(_T("Elements are:"));
				for (unsigned e = 0; e < lDevice->GetNumElements(); ++e)
				{
					UiLepra::InputElement* lElement = lDevice->GetElement(e);
					str lInterpretation;
					switch (lElement->GetInterpretation())
					{
						case UiLepra::InputElement::ABSOLUTE_AXIS:	lInterpretation += _T("AbsoluteAxis");	break;
						case UiLepra::InputElement::RELATIVE_AXIS:	lInterpretation += _T("RelativeAxis");	break;
						default:					lInterpretation += _T("Button");	break;
					}
					mLog.Infof(_T("  - '%s' of type '%s' with value '%f'"),
						lElement->GetIdentifier().c_str(),
						lInterpretation.c_str(),
						lElement->GetValue());
				}
			}
			lDevice->AddFunctor(new MasterInputFunctor(this));
		}*/
		mUiManager->GetInputManager()->AddFunctor(new MasterInputFunctor(this));

		SlaveArray::iterator x;
		for (x = mSlaveArray.begin(); lOk && x != mSlaveArray.end(); ++x)
		{
			GameClientSlaveManager* lSlave = *x;
			if (lSlave)
			{
				lOk = lSlave->Open();
			}
		}
	}
	mInitialized = lOk;
	if (!lOk)
	{
		mLog.AError("Could not initialize game!");
	}
	return (lOk);
}

void GameClientMasterTicker::UpdateSlaveLayout()
{
	if (!mInitialized)
	{
		return;
	}

	int lAveragedSlaves = 1;
	float lFps;
	CURE_RTVAR_GET(lFps, =(float), UiCure::GetSettings(), RTVAR_PHYSICS_FPS, 30);
	float lFrameTime = 1/lFps;
	for (int x = 0; x < 4; ++x)
	{
		if (mSlaveArray[x])
		{
			lFrameTime += mSlaveArray[x]->GetTimeManager()->GetNormalFrameTime();
			++lAveragedSlaves;
		}
	}
	lFrameTime /= lAveragedSlaves;
	const float lLayoutSpeed = Math::GetIterateLerpTime(0.7f, lFrameTime);

	GameClientSlaveManager* lLastSlave = 0;
	if (mActiveSlaveCount == 1)
	{
		for (int x = 0; x < 4; ++x)
		{
			if (mSlaveArray[x] && mSlaveArray[x]->IsQuitting())
			{
				lLastSlave = mSlaveArray[x];
				break;
			}
		}
	}
	if (!lLastSlave)
	{
		mSlaveTopSplit = Math::Lerp(mSlaveTopSplit, (GetSlaveAnimationTarget(0)-GetSlaveAnimationTarget(1)) * 0.5f + 0.5f, lLayoutSpeed);
		mSlaveBottomSplit = Math::Lerp(mSlaveBottomSplit, (GetSlaveAnimationTarget(2)-GetSlaveAnimationTarget(3)) * 0.5f + 0.5f, lLayoutSpeed);
		mSlaveVSplit = Math::Lerp(mSlaveVSplit, GetSlavesVerticalAnimationTarget(), lLayoutSpeed);
		mSlaveFade = 0;
		SlideSlaveLayout();
	}
	else
	{
		mSlaveFade = Math::Lerp(mSlaveFade, 1.0f, lLayoutSpeed);
		if (mSlaveFade < 1 - 1e-5f)
		{
			lLastSlave->SetFade(mSlaveFade);
		}
		else
		{
			DeleteSlave(lLastSlave, true);
		}
	}
}

void GameClientMasterTicker::SlideSlaveLayout()
{
	const PixelRect lRenderArea(0, 0, mUiManager->GetDisplayManager()->GetWidth(), mUiManager->GetDisplayManager()->GetHeight());
	if (mServer)
	{
		mServer->SetRenderArea(lRenderArea);
	}
	const float lRenderAreas[][4] =
	{
		{ 0, 0, mSlaveTopSplit, mSlaveVSplit, },
		{ mSlaveTopSplit, 0, 1, mSlaveVSplit, },
		{ 0, mSlaveVSplit, mSlaveBottomSplit, 1, },
		{ mSlaveBottomSplit, mSlaveVSplit, 1, 1, },
	};
	for (int x = 0; x < 4; ++x)
	{
		if (mSlaveArray[x] == 0)
		{
			continue;
		}

		PixelRect lPartRenderArea(
			(int)(lRenderArea.GetWidth()  * lRenderAreas[x][0]),
			(int)(lRenderArea.GetHeight() * lRenderAreas[x][1]),
			(int)(lRenderArea.GetWidth()  * lRenderAreas[x][2]),
			(int)(lRenderArea.GetHeight() * lRenderAreas[x][3])
			);
		const int REQUIRED_PIXELS = 4;
		if (lPartRenderArea.mLeft <= REQUIRED_PIXELS)	// Hatch onto edge?
		{
			lPartRenderArea.mLeft = 0;
		}
		else	// Do we want a border to the left?
		{
			lPartRenderArea.mLeft += (int)(lRenderAreas[x][0] * 4);
		}
		if (lPartRenderArea.GetWidth() >= lRenderArea.GetWidth()-REQUIRED_PIXELS)		// Hatch onto edge?
		{
			lPartRenderArea.mRight = lRenderArea.mRight;
		}
		if (lPartRenderArea.mTop <= REQUIRED_PIXELS)	// Hatch onto edge?
		{
			lPartRenderArea.mTop = 0;
		}
		else	// Do we want a border to the right?
		{
			lPartRenderArea.mTop += (int)(lRenderAreas[x][1] * 4);
		}
		if (lPartRenderArea.GetHeight() >= lRenderArea.GetHeight()-REQUIRED_PIXELS)	// Hatch onto edge?
		{
			lPartRenderArea.mBottom = lRenderArea.mBottom;
		}

		if (lPartRenderArea.GetWidth() < REQUIRED_PIXELS || lPartRenderArea.GetHeight() < REQUIRED_PIXELS)
		{
			if (mSlaveArray[x]->IsQuitting())
			{
				DeleteSlave(mSlaveArray[x], true);
			}
		}
		else
		{
			mSlaveArray[x]->SetRenderArea(lPartRenderArea);
		}
	}
}

int GameClientMasterTicker::GetSlaveAnimationTarget(int pSlaveIndex) const
{
	assert(pSlaveIndex >= 0 && pSlaveIndex < 4);
	GameClientSlaveManager* lSlave = mSlaveArray[pSlaveIndex];
	return (lSlave && !lSlave->IsQuitting())? 1 : 0;
}

float GameClientMasterTicker::GetSlavesVerticalAnimationTarget() const
{
	const int lTop = GetSlaveAnimationTarget(0) + GetSlaveAnimationTarget(1);
	const int lBottom = GetSlaveAnimationTarget(2) + GetSlaveAnimationTarget(3);
	if (lTop == 0 && lBottom == 0)
	{
		return (1.0f);
	}

	float lTopScale = 0;
	if (lTop == 1)
	{
		lTopScale = 0.6f;
	}
	else if (lTop == 2)
	{
		lTopScale = 1.0f;
	}
	float lBottomScale = 0;
	if (lBottom == 1)
	{
		lBottomScale = 0.6f;
	}
	else if (lBottom == 2)
	{
		lBottomScale = 1.0f;
	}
	return (lTopScale / (lTopScale+lBottomScale));
}

void GameClientMasterTicker::Profile()
{
	bool lDebugGraph;
	CURE_RTVAR_GET(lDebugGraph, =, UiCure::GetSettings(), RTVAR_DEBUG_PERFORMANCE_GRAPH, false);
	if (!lDebugGraph)
	{
		return;
	}

	const int lHeight = 100;
	typedef ScopePerformanceData::NodeArray ScopeArray;
	typedef std::pair<ScopePerformanceData*, int> ScopeLevel;
	typedef std::vector<ScopeLevel> ScopeLevelArray;
	ScopeArray lRoots = ScopePerformanceData::GetRoots();
	ScopeLevelArray lStackedNodes;
	lStackedNodes.reserve(100);	// Optimization.
	str lName;
	lName.reserve(200);	// Optimization;
	for (size_t lRootIndex = 0; lRootIndex < lRoots.size(); ++lRootIndex)
	{
		if (mPerformanceGraphList.size() <= lRootIndex)
		{
			mPerformanceGraphList.push_back(UiCure::LineGraph2d(mUiManager->GetPainter()));
		}
		mPerformanceGraphList[lRootIndex].TickLine(lHeight);

		const double lTotalPercentFactor = 100/lRoots[lRootIndex]->GetSlidingAverage();
		ScopeLevel lCurrentNode(lRoots[lRootIndex], 0);
		const double lRootStart = lCurrentNode.first->GetTimeOfLastMeasure();
		for (;;)
		{
			ScopePerformanceData* lNode = lCurrentNode.first;
			const ScopeArray& lChildren = lNode->GetChildren();
			for (size_t y = 0; y < lChildren.size(); ++y)
			{
				lStackedNodes.push_back(ScopeLevel(lChildren[y], lCurrentNode.second+1));
			}

			// Just multiply by number of times hit. Not perfect. Good enough but best?
			const double lTotalEstimatedDuration = lNode->GetLast() * lNode->GetHitCount();
			const double lPreviousEstimatedDuration = lNode->GetLast() * (lNode->GetHitCount() - 1);
			double lStart = lNode->GetTimeOfLastMeasure() - lRootStart - lPreviousEstimatedDuration;
			lStart = (lStart < 0)? 0 : lStart;
			lName  = str(lCurrentNode.second, ' ');
			lName += lNode->GetName();
			lName += _T(" (");
			lName += strutil::DoubleToString(lNode->GetRangeFactor()*100, 1);
			lName += _T(" % fluctuation, ");
			lName += strutil::DoubleToString(lNode->GetSlidingAverage()*lNode->GetHitCount()*lTotalPercentFactor, 1);
			lName += _T(" % total time)");
			mPerformanceGraphList[lRootIndex].AddSegment(lName, lStart, lStart + lTotalEstimatedDuration);
			lNode->ResetHitCount();

			if (!lStackedNodes.empty())
			{
				lCurrentNode = lStackedNodes.back();
				lStackedNodes.pop_back();
			}
			else
			{
				break;
			}
		}
	}
}

bool GameClientMasterTicker::QueryQuit()
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
		CreateSlave(&GameClientMasterTicker::CreateDemo);
		mDemoTime = new HiResTimer;
#else // !Demo
		return (true);
#endif // Demo / !Demo
	}
	return (false);
}

void GameClientMasterTicker::DrawDebugData() const
{
	bool lDebugging;
	CURE_RTVAR_GET(lDebugging, =, UiCure::GetSettings(), RTVAR_DEBUG_ENABLE, false);
	if (!lDebugging)
	{
		return;
	}

	const ScopePerformanceData* lMainLoop = ScopePerformanceData::GetRoots()[0];
	str lInfo = strutil::Format(_T("FPS %.1f"), 1/lMainLoop->GetSlidingAverage());
	const ScopePerformanceData* lAppSleep = lMainLoop->GetChild(_T("AppSleep"));
	const double lPercent = 100 * (lMainLoop->GetSlidingAverage()-lAppSleep->GetSlidingAverage()) / lMainLoop->GetSlidingAverage() + 0.5f;
	lInfo += strutil::Format(_T("\nUsedPerf %2.f %%"), lPercent);
	int w = 80;
	int h = 37;
	bool lShowPerformanceCounters;
	CURE_RTVAR_GET(lShowPerformanceCounters, =, UiCure::GetSettings(), RTVAR_DEBUG_PERFORMANCE_COUNT, false);
	if (lShowPerformanceCounters)
	{
		w = 120;
		double lUpBandwidth = 0;
		double lDownBandwidth = 0;
		for (int x = 0; x < 4; ++x)
		{
			if (mSlaveArray[x])
			{
				Cure::GameManager::BandwidthData lUp;
				Cure::GameManager::BandwidthData lDown;
				mSlaveArray[x]->GetBandwidthData(lUp, lDown);
				lUpBandwidth += lUp.GetLast();
				lDownBandwidth += lDown.GetLast();
			}
		}
		
		lInfo += strutil::Format(_T("\nvTRI %i\ncTRI %i\nUpload %sB/s\nDownload %sB/s"),
			mUiManager->GetRenderer()->GetTriangleCount(true),
			mUiManager->GetRenderer()->GetTriangleCount(false),
			Number::ConvertToPostfixNumber(lUpBandwidth, 1).c_str(),
			Number::ConvertToPostfixNumber(lDownBandwidth, 1).c_str());
		h += 17*4;
	}
	mUiManager->GetPainter()->SetColor(Color(0, 0, 0));
	const int lRight = mUiManager->GetDisplayManager()->GetWidth();
	mUiManager->GetPainter()->FillRect(lRight-w, 3, lRight-5, h);
	mUiManager->GetPainter()->SetColor(Color(200, 200, 0));
	mUiManager->GetPainter()->PrintText(lInfo, lRight-w+5, 5);
}

void GameClientMasterTicker::DrawPerformanceLineGraph2d() const
{
	bool lDebug;
	bool lDebugGraph;
	CURE_RTVAR_GET(lDebug, =, UiCure::GetSettings(), RTVAR_DEBUG_ENABLE, false);
	CURE_RTVAR_GET(lDebugGraph, =, UiCure::GetSettings(), RTVAR_DEBUG_PERFORMANCE_GRAPH, false);
	if (!lDebug || !lDebugGraph)
	{
		return;
	}

	// Draw all scope nodes as line segments in one hunky graph.
	typedef ScopePerformanceData::NodeArray ScopeArray;

	float lLongestRootTime = 1e-15f;
	ScopeArray lRoots = ScopePerformanceData::GetRoots();
	for (size_t lRootIndex = 0; lRootIndex < lRoots.size(); ++lRootIndex)
	{
		const float lRootDelta = (float)lRoots[lRootIndex]->GetMaximum();
		if (lRootDelta > lLongestRootTime)
		{
			lLongestRootTime = lRootDelta;
		}
	}

	const int lMargin = 10;
	const float lScale = (mUiManager->GetDisplayManager()->GetWidth() - lMargin*2)/lLongestRootTime;
	int lYOffset;
	CURE_RTVAR_GET(lYOffset, =, UiCure::GetSettings(), RTVAR_DEBUG_PERFORMANCE_YOFFSET, 0);
	int lY = lMargin + lYOffset;
	for (size_t lRootIndex = 0; lRootIndex < lRoots.size(); ++lRootIndex)
	{
		if (mPerformanceGraphList.size() <= lRootIndex)
		{
			return;
		}

		mPerformanceGraphList[lRootIndex].Render(lMargin, lScale, lY);

		bool lDebugNames;
		CURE_RTVAR_GET(lDebugNames, =, UiCure::GetSettings(), RTVAR_DEBUG_PERFORMANCE_NAMES, false);
		if (lDebugNames)
		{
			mPerformanceGraphList[lRootIndex].RenderNames(lMargin, lY);
		}
	}

}



float GameClientMasterTicker::GetTickTimeReduction() const
{
	for (int x = 0; x < 4; ++x)
	{
		if (mSlaveArray[x])
		{
			return mSlaveArray[x]->GetTimeManager()->GetTickLoopTimeReduction();
		}
	}
	return 0;
}

float GameClientMasterTicker::GetPowerSaveAmount() const
{
	bool lIsMinimized = !mUiManager->GetDisplayManager()->IsVisible();
	return (lIsMinimized? 0.4f : 0);
}



int GameClientMasterTicker::OnCommandLocal(const str& pCommand, const strutil::strvec& pParameterVector)
{
	if (pCommand == SET_PLAYER_COUNT)
	{
		int lPlayerCount = 0;
		if (pParameterVector.size() == 1 && strutil::StringToInt(pParameterVector[0], lPlayerCount))
		{
			if (lPlayerCount >= 1 && lPlayerCount <= 4)
			{
				OnSetPlayerCount(lPlayerCount);
				return (0);
			}
			else
			{
				mLog.AError("player count must lie between 1 and 4");
			}
		}
		else
		{
			mLog.Warningf(_T("usage: %s <no. of players>"), pCommand.c_str());
		}
		return (1);
	}
	return (-1);
}

void GameClientMasterTicker::OnCommandError(const str&, const strutil::strvec&, int)
{
}



bool GameClientMasterTicker::OnKeyDown(UiLepra::InputManager::KeyCode pKeyCode)
{
	bool lConsumed = false;

	if (mServer)
	{
		lConsumed = mServer->OnKeyDown(pKeyCode);
	}

	SlaveArray::iterator x = mSlaveArray.begin();
	for (; !lConsumed && x != mSlaveArray.end(); ++x)
	{
		GameClientSlaveManager* lSlave = *x;
		if (lSlave)
		{
			lConsumed = lSlave->OnKeyDown(pKeyCode);
		}
	}
	return (lConsumed);
}

bool GameClientMasterTicker::OnKeyUp(UiLepra::InputManager::KeyCode pKeyCode)
{
	bool lConsumed = false;
	if (mServer)
	{
		lConsumed = mServer->OnKeyUp(pKeyCode);
	}
	SlaveArray::iterator x = mSlaveArray.begin();
	for (; !lConsumed && x != mSlaveArray.end(); ++x)
	{
		GameClientSlaveManager* lSlave = *x;
		if (lSlave)
		{
			lConsumed = lSlave->OnKeyUp(pKeyCode);
		}
	}
	return (lConsumed);
}

void GameClientMasterTicker::OnInput(UiLepra::InputElement* pElement)
{
	/*log_volatile(mLog.Tracef(_T("Got input from element '%s' / '%s' = %f.\n"),
		pElement->GetFullName().c_str(),
		pElement->GetIdentifier().c_str(),
		pElement->GetValue()));

	if (mServer)
	{
		mServer->OnInput(pElement);
	}*/
	SlaveArray::iterator x = mSlaveArray.begin();
	for (; x != mSlaveArray.end(); ++x)
	{
		GameClientSlaveManager* lSlave = *x;
		if (lSlave)
		{
			lSlave->OnInput(pElement);
		}
	}
}



void GameClientMasterTicker::ClosePlayerCountGui()
{
	if (mIsPlayerCountViewActive)
	{
		DeleteSlave(mSlaveArray[0], false);
		mIsPlayerCountViewActive = false;
	}
}



bool GameClientMasterTicker::ApplyCalibration()
{
	bool lOk = true;
	const UiLepra::InputManager::DeviceList& lDevices = mUiManager->GetInputManager()->GetDeviceList();
	UiLepra::InputManager::DeviceList::const_iterator x = lDevices.begin();
	for (; x != lDevices.end(); ++x)
	{
		UiLepra::InputDevice* lDevice = *x;
		str lDeviceId = strutil::ReplaceAll(lDevice->GetIdentifier(), ' ', '_');
		lDeviceId = strutil::ReplaceAll(lDeviceId, '.', '_');
		UiLepra::InputDevice::CalibrationData lCalibration;

		const std::list<str> lVariableNames = UiCure::GetSettings()->GetVariableNameList(Cure::RuntimeVariableScope::SEARCH_ALL);
		std::list<str>::const_iterator y = lVariableNames.begin();
		for (; y != lVariableNames.end(); ++y)
		{
			const str& lVarName = *y;
			const strutil::strvec lVarNames = strutil::Split(lVarName, _T("."), 2);
			if (lVarNames.size() != 3)
			{
				continue;
			}
			if (lVarNames[0] == _T("Calibration") && lVarNames[1] == lDeviceId)
			{
				str lValue = UiCure::GetSettings()->GetDefaultValue(
					Cure::RuntimeVariableScope::READ_ONLY, lVarName, _T(""));
				lCalibration.push_back(UiLepra::InputDevice::CalibrationElement(lVarNames[2], lValue));
			}
		}
		lOk &= lDevice->SetCalibration(lCalibration);
	}
	return (lOk);
}

void GameClientMasterTicker::StashCalibration()
{
	if (!mUiManager->GetInputManager())
	{
		return;
	}

	const UiLepra::InputManager::DeviceList& lDevices = mUiManager->GetInputManager()->GetDeviceList();
	UiLepra::InputManager::DeviceList::const_iterator x = lDevices.begin();
	for (; x != lDevices.end(); ++x)
	{
		UiLepra::InputDevice* lDevice = *x;
		str lDeviceId = strutil::ReplaceAll(lDevice->GetIdentifier(), ' ', '_');
		lDeviceId = strutil::ReplaceAll(lDeviceId, '.', '_');
		const UiLepra::InputDevice::CalibrationData lCalibration = lDevice->GetCalibration();

		UiLepra::InputDevice::CalibrationData::const_iterator y = lCalibration.begin();
		for (; y != lCalibration.end(); ++y)
		{
			const UiLepra::InputDevice::CalibrationElement& lElement = *y;
			UiCure::GetSettings()->SetValue(Cure::RuntimeVariable::USAGE_NORMAL,
				_T("Calibration.")+lDeviceId+_T(".")+lElement.first, lElement.second);
		}
	}
}



GameClientMasterTicker::MasterInputFunctor::MasterInputFunctor(GameClientMasterTicker* pManager):
	mManager(pManager)
{
}

void GameClientMasterTicker::MasterInputFunctor::Call(UiLepra::InputElement* pElement)
{
	mManager->OnInput(pElement);
}

UiLepra::InputFunctor* GameClientMasterTicker::MasterInputFunctor::CreateCopy() const
{
	return (new MasterInputFunctor(mManager));
}



LOG_CLASS_DEFINE(GAME, GameClientMasterTicker);



}
