
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "GameClientMasterTicker.h"
#include "../../Cure/Include/NetworkClient.h"
#include "../../Cure/Include/NetworkFreeAgent.h"
#include "../../Cure/Include/ResourceManager.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Cure/Include/TimeManager.h"
#include "../../Lepra/Include/Network.h"
#include "../../Lepra/Include/Number.h"
#include "../../Lepra/Include/Performance.h"
#include "../../Lepra/Include/SystemManager.h"
#include "../../UiCure/Include/UiGameUiManager.h"
#include "../../UiCure/Include/UiResourceManager.h"
#include "../../UiCure/Include/UiRuntimeVariableName.h"
#include "../../UiTBC/Include/GUI/UiConsoleLogListener.h"
#include "../../UiTBC/Include/GUI/UiConsolePrompt.h"
#include "../LifeServer/MasterServerConnection.h"
#include "../ConsoleManager.h"
#include "../LifeApplication.h"
#include "GameClientSlaveManager.h"
#include "RtVar.h"
#include "UiGameServerManager.h"
#include "UiRaceScore.h"



namespace Life
{



#define SET_PLAYER_COUNT	_T("set-player-count")



GameClientMasterTicker::GameClientMasterTicker(UiCure::GameUiManager* pUiManager, Cure::ResourceManager* pResourceManager, float pPhysicsRadius, int pPhysicsLevels, float pPhysicsSensitivity):
	Parent(pPhysicsRadius, pPhysicsLevels, pPhysicsSensitivity),
	mUiManager(pUiManager),
	mResourceManager(pResourceManager),
	mServer(0),
	mMasterConnection(0),
	mFreeNetworkAgent(new Cure::NetworkFreeAgent),
	mRestartUi(false),
	mInitialized(false),
	mActiveWidth(0),
	mActiveHeight(0),
	mActiveSlaveCount(0),
	mSlaveTopSplit(1),
	mSlaveBottomSplit(1),
	mSlaveVSplit(1),
	mSlaveFade(0)
{
	mSlaveArray.resize(4, 0);
	mSlaveArray[0] = 0;
	mSlaveArray[1] = 0;
	mSlaveArray[2] = 0;
	mSlaveArray[3] = 0;

	UiLepra::DisplayManager::EnableScreensaver(false);

	Cure::ContextObjectAttribute::SetCreator(Cure::ContextObjectAttribute::Factory(
		this, &GameClientMasterTicker::CreateObjectAttribute));

	CURE_RTVAR_SET(UiCure::GetSettings(), RTVAR_DEBUG_PERFORMANCE_COUNT, true);
	CURE_RTVAR_SET(UiCure::GetSettings(), RTVAR_DEBUG_PERFORMANCE_GRAPH, false);
	CURE_RTVAR_SET(UiCure::GetSettings(), RTVAR_DEBUG_PERFORMANCE_NAMES, true);
	CURE_RTVAR_SET(UiCure::GetSettings(), RTVAR_DEBUG_PERFORMANCE_YOFFSET, 10);
	CURE_RTVAR_SET(UiCure::GetSettings(), RTVAR_DEBUG_3D_DRAWLOCALSERVER, true);

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

	SetMasterServerConnection(0);
	delete mFreeNetworkAgent;
	mFreeNetworkAgent = 0;
}

void GameClientMasterTicker::AddBackedRtvar(const str& pRtvarName)
{
	mRtvars.push_back(pRtvarName);
}

void GameClientMasterTicker::Suspend()
{
	strutil::strvec::iterator x = mRtvars.begin();
	for (; x != mRtvars.end(); ++x)
	{
		SystemHelper::SaveRtvar(*x);
	}
}

void GameClientMasterTicker::Resume()
{
	strutil::strvec::iterator x = mRtvars.begin();
	for (; x != mRtvars.end(); ++x)
	{
		SystemHelper::LoadRtvar(*x);
	}
}

MasterServerConnection* GameClientMasterTicker::GetMasterServerConnection() const
{
	return mMasterConnection;
}

void GameClientMasterTicker::SetMasterServerConnection(MasterServerConnection* pConnection)
{
	delete mMasterConnection;
	mMasterConnection = pConnection;
}



void GameClientMasterTicker::PrepareQuit()
{
	if (mUiManager->GetSoundManager())
	{
		// Keep quiet so there won't be a lot of noise when destroying explosives, and so forth.
		mUiManager->GetSoundManager()->SetMasterVolume(0);
	}
}



bool GameClientMasterTicker::Tick()
{
	LEPRA_MEASURE_SCOPE(MasterTicker);

	GetTimeManager()->Tick();

	bool lOk = true;

	ScopeLock lLock(&mLock);

	SlaveArray::iterator x;

	if (mMasterConnection)
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
		bool lAllowOnline;
		CURE_RTVAR_GET(lAllowOnline, =, UiCure::GetSettings(), RTVAR_NETWORK_ENABLEONLINEMASTER, false);
		if (lAllowOnline)
		{
			mMasterConnection->Tick();
		}
	}

	{
		LEPRA_MEASURE_SCOPE(BeginRenderAndInput);

		float r, g, b;
		CURE_RTVAR_GET(r, =(float), UiCure::GetSettings(), RTVAR_UI_3D_CLEARRED, 0.75);
		CURE_RTVAR_GET(g, =(float), UiCure::GetSettings(), RTVAR_UI_3D_CLEARGREEN, 0.80);
		CURE_RTVAR_GET(b, =(float), UiCure::GetSettings(), RTVAR_UI_3D_CLEARBLUE, 0.85);
		Vector3DF lColor(r, g, b);
		BeginRender(lColor);

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

	StartPhysicsTick();

	{
		MeasureLoad();
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
				UiTbc::Renderer::LIGHT_STATIC, Vector3DF(0, 1, -1), Vector3DF(1.5f, 1.5f, 1.5f), 160);
			mUiManager->Paint(true);
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
		mServer->PreEndTick();
	}

	{
		for (x = mSlaveArray.begin(); lOk && x != mSlaveArray.end(); ++x)
		{
			GameClientSlaveManager* lSlave = *x;
			if (lSlave)
			{
				lSlave->PreEndTick();
			}
		}
	}

	PreWaitPhysicsTick();
	WaitPhysicsTick();

	if (mServer)
	{
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
		mUiManager->EndRender(GetTimeManager()->GetNormalFrameTime());
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

	Repair();

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

void GameClientMasterTicker::PreWaitPhysicsTick()
{
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
	deb_assert(false);
	return false;
}

void GameClientMasterTicker::GetSlaveInfo(const GameClientSlaveManager* pSlave, int& pIndex, int& pCount) const
{
	int lIndex = 0;
	pCount = mActiveSlaveCount;
	SlaveArray::const_iterator x = mSlaveArray.begin();
	for (; x != mSlaveArray.end(); ++x)
	{
		GameClientSlaveManager* lSlave = *x;
		if (lSlave == pSlave)
		{
			pIndex = lIndex;
			return;
		}
		if (lSlave)
		{
			++lIndex;
		}
	}
	pIndex = 0;
}

bool GameClientMasterTicker::IsLocalObject(Cure::GameObjectId pInstanceId) const
{
	return (mLocalObjectSet.find(pInstanceId) != mLocalObjectSet.end());
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
	const bool lIsLocalServer = Network::IsLocalAddress(pServerAddress);
	if (lIsLocalServer && !mServer)
	{
		Cure::RuntimeVariableScope* lVariableScope = new Cure::RuntimeVariableScope(UiCure::GetSettings());
		UiGameServerManager* lServer = new UiGameServerManager(GetTimeManager(), lVariableScope, mResourceManager, mUiManager, PixelRect(0, 0, 100, 100));
		lServer->SetTicker(this);
		OnServerCreated(lServer);
		lServer->StartConsole(new UiTbc::ConsoleLogListener, new UiTbc::ConsolePrompt);
		if (!lServer->Initialize(mMasterConnection, pServerAddress))
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

UiGameServerManager* GameClientMasterTicker::GetLocalServer() const
{
	return mServer;
}

void GameClientMasterTicker::OnExit()
{
	mLog.Headline(_T("Number of players not picked, quitting."));
	SystemManager::AddQuitRequest(+1);
	CloseMainMenu();
}

void GameClientMasterTicker::OnSetPlayerCount(int pPlayerCount)
{
	CloseMainMenu();

	for (int x = 0; x < pPlayerCount; ++x)
	{
		CreateSlave();
	}

	if (!mServer && mMasterConnection)
	{
		// If client descided we won't run local server, then we have no need for
		// a master connection any more. Was only used to download server list anyway.
		mMasterConnection->GraceClose(0.1, false);
	}
}

void GameClientMasterTicker::DownloadServerList()
{
	if (mMasterConnection)
	{
		mMasterConnection->RequestServerList(_T(""));
	}
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
		deb_assert(lFreeSlaveIndex < 4);
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

void GameClientMasterTicker::AddSlave(GameClientSlaveManager* pSlave)
{
	{
		ScopeLock lLock(&mLock);
		pSlave->LoadSettings();
		pSlave->RefreshOptions();
		deb_assert(mSlaveArray[pSlave->GetSlaveIndex()] == 0);
		mSlaveArray[pSlave->GetSlaveIndex()] = pSlave;
		pSlave->GetContext()->SetLocalRange(pSlave->GetSlaveIndex()+1, mSlaveArray.size()+1);	// Local server is first one, so skip that part in the rance.
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
	deb_assert(mSlaveArray[pSlave->GetSlaveIndex()] == pSlave);
	pSlave->SaveSettings();
	mSlaveArray[pSlave->GetSlaveIndex()] = 0;
	delete (pSlave);
	if (mConsole && mConsole->GetGameManager() == pSlave)
	{
		mConsole->SetGameManager(0);
	}
	if (--mActiveSlaveCount == 0)
	{
		//mResourceManager->ForceFreeCache();
		if (pAllowMainMenu)
		{
			OnSlavesKilled();
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

		OnSlavesKilled();
	}
	return (lOk);
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
	mConsole->SetGameManager(0);
	mResourceManager->StopClear();
	mUiManager->Close();
	SystemManager::AddQuitRequest(-1);

	// (Re)open.
	bool lOk = mResourceManager->InitDefault();
	if (lOk)
	{
		lOk = OpenUiManager();
	}
	if (lOk)
	{
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
		mUiManager->GetInputManager()->AddFunctor(new MasterInputFunctor(this, &GameClientMasterTicker::OnInput));

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

bool GameClientMasterTicker::OpenUiManager()
{
	return mUiManager->Open();
}

void GameClientMasterTicker::Repair()
{
	// OpenAL/iOS bug: after some time playback stops working. We solve this by locking down the whole system,
	// re-initialize the sound system, loading all the sound resources again, then unlocking.
	if (mUiManager->GetSoundManager()->IsIrreparableErrorState())
	{
		typedef Cure::ResourceManager::ResourceList ResourceList;
		ResourceList lResourceList = mResourceManager->HookAllResourcesOfType(_T("Sound"));
		for (ResourceList::iterator x = lResourceList.begin(); x != lResourceList.end(); ++x)
		{
			UiCure::SoundResource* lSound = (UiCure::SoundResource*)*x;
			lSound->Release();
		}
		mUiManager->GetSoundManager()->Close();
		mUiManager->GetSoundManager()->Open();
		for (ResourceList::iterator x = lResourceList.begin(); x != lResourceList.end(); ++x)
		{
			UiCure::SoundResource* lSound = (UiCure::SoundResource*)*x;
			lSound->Load();
		}
		mResourceManager->UnhookResources(lResourceList);
	}
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
			lFrameTime += GetTimeManager()->GetNormalFrameTime();
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
	deb_assert(pSlaveIndex >= 0 && pSlaveIndex < 4);
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

void GameClientMasterTicker::MeasureLoad()
{
	const ScopePerformanceData* lMainLoop = ScopePerformanceData::GetRoots()[0];
	const ScopePerformanceData* lAppSleep = lMainLoop->GetChild(_T("AppSleep"));
	if (lAppSleep)
	{
		int lTargetFrameRate;
		CURE_RTVAR_GET(lTargetFrameRate, =, Cure::GetSettings(), RTVAR_PHYSICS_FPS, 2);
		const double lCurrentPerformanceLoad = (lMainLoop->GetSlidingAverage()-lAppSleep->GetSlidingAverage()) * lTargetFrameRate;
		double lAveragePerformanceLoad;
		CURE_RTVAR_TRYGET(lAveragePerformanceLoad, =, UiCure::GetSettings(), RTVAR_DEBUG_PERFORMANCE_LOAD, 0.95);
		if (lCurrentPerformanceLoad < 20)
		{
			lAveragePerformanceLoad = Math::Lerp(lAveragePerformanceLoad, lCurrentPerformanceLoad, 0.05);
		}
		CURE_RTVAR_INTERNAL(UiCure::GetSettings(), RTVAR_DEBUG_PERFORMANCE_LOAD, lAveragePerformanceLoad);
	}
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

void GameClientMasterTicker::PhysicsTick()
{
	Parent::PhysicsTick();

	for (SlaveArray::iterator x = mSlaveArray.begin(); x != mSlaveArray.end(); ++x)
	{
		GameClientSlaveManager* lSlave = *x;
		if (lSlave)
		{
			lSlave->TickNetworkOutputGhosts();
		}
	}
}

void GameClientMasterTicker::WillMicroTick(float pTimeDelta)
{
	for (SlaveArray::iterator x = mSlaveArray.begin(); x != mSlaveArray.end(); ++x)
	{
		GameClientSlaveManager* lSlave = *x;
		if (lSlave)
		{
			lSlave->MicroTick(pTimeDelta);
		}
	}
	if (mServer)
	{
		mServer->MicroTick(pTimeDelta);
	}
}

void GameClientMasterTicker::DidPhysicsTick()
{
	for (SlaveArray::iterator x = mSlaveArray.begin(); x != mSlaveArray.end(); ++x)
	{
		GameClientSlaveManager* lSlave = *x;
		if (lSlave)
		{
			lSlave->PostPhysicsTick();
		}
	}
	if (mServer)
	{
		mServer->PostPhysicsTick();
	}
}

void GameClientMasterTicker::BeginRender(Vector3DF& pColor)
{
	mUiManager->BeginRender(pColor);

	mLocalObjectSet.clear();
	SlaveArray::iterator x;
	for (x = mSlaveArray.begin(); x != mSlaveArray.end(); ++x)
	{
		GameClientSlaveManager* lSlave = *x;
		if (lSlave)
		{
			lSlave->AddLocalObjects(mLocalObjectSet);
		}
	}
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
	double lLoad;
	CURE_RTVAR_GET(lLoad, =, UiCure::GetSettings(), RTVAR_DEBUG_PERFORMANCE_LOAD, 0.95);
	lInfo += strutil::Format(_T("\nUsedPerf %2.f %%"), 100 * lLoad + 0.5f);
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
	return GetTimeManager()->GetTickLoopTimeReduction();
}

float GameClientMasterTicker::GetPowerSaveAmount() const
{
	bool lIsMinimized = !mUiManager->GetDisplayManager()->IsVisible();
	return (lIsMinimized? 0.4f : 0);
}



void GameClientMasterTicker::OnTrigger(TBC::PhysicsManager::TriggerID pTrigger, int pTriggerListenerId, int pOtherBodyId, const Vector3DF& pNormal)
{
	for (SlaveArray::iterator x = mSlaveArray.begin(); x != mSlaveArray.end(); ++x)
	{
		GameClientSlaveManager* lSlave = *x;
		if (lSlave)
		{
			lSlave->OnTrigger(pTrigger, pTriggerListenerId, pOtherBodyId, pNormal);
		}
	}
	if (mServer)
	{
		mServer->OnTrigger(pTrigger, pTriggerListenerId, pOtherBodyId, pNormal);
	}
}

void GameClientMasterTicker::OnForceApplied(int pObjectId, int pOtherObjectId, TBC::PhysicsManager::BodyID pBodyId, TBC::PhysicsManager::BodyID pOtherBodyId,
		const Vector3DF& pForce, const Vector3DF& pTorque, const Vector3DF& pPosition, const Vector3DF& pRelativeVelocity)
{
	for (SlaveArray::iterator x = mSlaveArray.begin(); x != mSlaveArray.end(); ++x)
	{
		GameClientSlaveManager* lSlave = *x;
		if (lSlave)
		{
			lSlave->OnForceApplied(pObjectId, pOtherObjectId, pBodyId, pOtherBodyId, pForce, pTorque, pPosition, pRelativeVelocity);
		}
	}
	if (mServer)
	{
		mServer->OnForceApplied(pObjectId, pOtherObjectId, pBodyId, pOtherBodyId, pForce, pTorque, pPosition, pRelativeVelocity);
	}
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
				str lValue = UiCure::GetSettings()->GetUntypedDefaultValue(Cure::RuntimeVariableScope::READ_ONLY, lVarName);
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



LOG_CLASS_DEFINE(GAME, GameClientMasterTicker);



}
