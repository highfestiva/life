
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "GameClientMasterTicker.h"
#include <algorithm>
#include "../../Cure/Include/ResourceManager.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Cure/Include/TimeManager.h"
#include "../../Lepra/Include/Network.h"
#include "../../Lepra/Include/Performance.h"
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
#include "../../UiTBC/Include/UiRenderer.h"
#include "../LifeApplication.h"
#include "GameClientDemo.h"
#include "GameClientSlaveManager.h"
#include "GameClientViewer.h"
#include "RoadSignButton.h"
#include "RtVar.h"
#include "UiGameServerManager.h"

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
	mRestartUi(false),
	mInitialized(false),
	mActiveWidth(0),
	mActiveHeight(0),
	mActiveSlaveCount(0),
	mSlaveTopSplit(1),
	mSlaveBottomSplit(1),
	mSlaveVSplit(1),
	mSlaveFade(0),
	mDemoTime(0)
{
	mSlaveArray.resize(4, 0);

	UiLepra::DisplayManager::EnableScreensaver(false);

	mConsole = new ConsoleManager(0, UiCure::GetSettings(), 0, 0);
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
}



bool GameClientMasterTicker::CreateSlave()
{
	if (!mServer)
	{
		bool lIsLocalServer = false;
		const str lServerUrl = strutil::Split(CURE_RTVAR_GET(UiCure::GetSettings(), RTVAR_NETWORK_SERVERADDRESS, _T("0.0.0.0:16650")), _T(":"), 1)[0];
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
		if (lIsLocalServer)
		{
			Cure::RuntimeVariableScope* lVariableScope = new Cure::RuntimeVariableScope(Cure::GetSettings());
			UiGameServerManager* lServer = new UiGameServerManager(lVariableScope, mResourceManager, mUiManager, PixelRect(0, 0, 100, 100));
			lServer->StartConsole(new UiTbc::ConsoleLogListener, new UiTbc::ConsolePrompt);
			if (!lServer->Initialize())
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
	}
	return (CreateSlave(&GameClientMasterTicker::CreateSlaveManager));
}



bool GameClientMasterTicker::Tick()
{
	LEPRA_MEASURE_SCOPE(MasterTicker);

	bool lOk = true;

	ScopeLock lLock(&mLock);

	SlaveArray::iterator x;

	{
		LEPRA_MEASURE_SCOPE(BeginRenderAndInput);

		mLocalObjectSet.clear();
		for (x = mSlaveArray.begin(); x != mSlaveArray.end(); ++x)
		{
			GameClientSlaveManager* lSlave = *x;
			if (lSlave)
			{
				lSlave->AddLocalObjects(mLocalObjectSet);
			}
		}

		mUiManager->BeginRender();
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
		LEPRA_MEASURE_SCOPE(RenderSlaves);

		// Start rendering machine directly afterwards.
		int lSlaveIndex = 0;
		for (x = mSlaveArray.begin(); lOk && x != mSlaveArray.end(); ++x)
		{
			GameClientSlaveManager* lSlave = *x;
			if (lSlave)
			{
				mUiManager->GetSoundManager()->SetCurrentListener(lSlaveIndex, mActiveSlaveCount);
				++lSlaveIndex;
				lOk = lSlave->Render();
			}
		}
	}

	{
		LEPRA_MEASURE_SCOPE(Paint);
		mUiManager->Paint();
		for (x = mSlaveArray.begin(); lOk && x != mSlaveArray.end(); ++x)
		{
			GameClientSlaveManager* lSlave = *x;
			if (lSlave)
			{
				lOk = lSlave->Paint();
			}
		}
	}

	{
		LEPRA_MEASURE_SCOPE(DrawGraph);
		DrawFps();
		DrawPerformanceLineGraph2d();
	}

	if (mServer)
	{
		LEPRA_MEASURE_SCOPE(ServerEndTick);
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
		LEPRA_MEASURE_SCOPE(UpdateSlaveLayout);
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
		LEPRA_MEASURE_SCOPE(RunYieldCommand);
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
			lSlave->TickNetworkOutput();
		}
	}
	if (mServer)
	{
		mServer->GameServerManager::TickInput();
		mServer->TickNetworkOutput();
	}
	for (x = mSlaveArray.begin(); x != mSlaveArray.end(); ++x)
	{
		GameClientSlaveManager* lSlave = *x;
		if (lSlave)
		{
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

float GameClientMasterTicker::UpdateFrustum()
{
	const float lFov = (float)CURE_RTVAR_GET(UiCure::GetSettings(), RTVAR_UI_3D_FOV, 45.0);
	const float lClipNear = (float)CURE_RTVAR_GET(UiCure::GetSettings(), RTVAR_UI_3D_CLIPNEAR, 0.1);
	const float lClipFar = (float)CURE_RTVAR_GET(UiCure::GetSettings(), RTVAR_UI_3D_CLIPFAR, 1000.0);
	mUiManager->GetRenderer()->SetViewFrustum(lFov, lClipNear, lClipFar);
	return (lFov);
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
}



GameClientSlaveManager* GameClientMasterTicker::CreateSlaveManager(GameClientMasterTicker* pMaster,
	Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager,
	UiCure::GameUiManager* pUiManager, int pSlaveIndex, const PixelRect& pRenderArea)
{
	return new GameClientSlaveManager(pMaster, pVariableScope, pResourceManager, pUiManager, pSlaveIndex, pRenderArea);
}

GameClientSlaveManager* GameClientMasterTicker::CreateViewer(GameClientMasterTicker* pMaster,
	Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager,
	UiCure::GameUiManager* pUiManager, int pSlaveIndex, const PixelRect& pRenderArea)
{
	return new GameClientViewer(pMaster, pVariableScope, pResourceManager, pUiManager, pSlaveIndex, pRenderArea);
}

GameClientSlaveManager* GameClientMasterTicker::CreateDemo(GameClientMasterTicker* pMaster,
	Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager,
	UiCure::GameUiManager* pUiManager, int pSlaveIndex, const PixelRect& pRenderArea)
{
#ifdef LIFE_DEMO
	return new GameClientDemo(pMaster, pVariableScope, pResourceManager, pUiManager, pSlaveIndex, pRenderArea);
#else // !Demo
	return new GameClientViewer(pMaster, pVariableScope, pResourceManager, pUiManager, pSlaveIndex, pRenderArea);
#endif // Demo / !Demo
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
		GameClientSlaveManager* lSlave = pCreate(this, lVariables, mResourceManager,
			mUiManager, lFreeSlaveIndex, lRenderArea);
		AddSlave(lSlave);
		if (mInitialized)
		{
			lOk = lSlave->Open();
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
	if (--mActiveSlaveCount == 0 && pAllowMainMenu)
	{
		CreatePlayerCountWindow();
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

		// TODO: replace with world-load.
		mUiManager->GetRenderer()->AddDirectionalLight(
			UiTbc::Renderer::LIGHT_STATIC, Vector3DF(0, 0.5f, -1),
			Color::Color(255, 255, 255), 1.5f, 20);
		mUiManager->GetInputManager()->AddKeyCodeInputObserver(this);
	}
	if (lOk)
	{
		DeleteServer();
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
	float lFrameTime = 1/(float)CURE_RTVAR_GET(Cure::GetSettings(), RTVAR_PHYSICS_FPS, 30.0);
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
		if (lPartRenderArea.mLeft <= REQUIRED_PIXELS)
		{
			lPartRenderArea.mLeft = 0;
		}
		if (lPartRenderArea.GetWidth() >= lRenderArea.GetWidth()-REQUIRED_PIXELS)
		{
			lPartRenderArea.mRight = lRenderArea.mRight;
		}
		if (lPartRenderArea.mTop <= REQUIRED_PIXELS)
		{
			lPartRenderArea.mTop = 0;
		}
		if (lPartRenderArea.GetHeight() >= lRenderArea.GetHeight()-REQUIRED_PIXELS)
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
	const bool lDebugGraph = CURE_RTVAR_GET(UiCure::GetSettings(), RTVAR_DEBUG_PERFORMANCE_GRAPH, false);
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
		if (!mUiManager->GetDisplayManager()->IsVisible())
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

void GameClientMasterTicker::DrawFps() const
{
	const bool lDebugging = CURE_RTVAR_GET(UiCure::GetSettings(), RTVAR_DEBUG_ENABLE, false);
	if (!lDebugging)
	{
		return;
	}

	ScopePerformanceData* lMainLoop = ScopePerformanceData::GetRoots()[0];
	str lFps = strutil::Format(_T("%.1f"), 1/lMainLoop->GetSlidingAverage());
	mUiManager->GetPainter()->SetColor(Color(0, 0, 0));
	const int lRight = mUiManager->GetDisplayManager()->GetWidth();
	mUiManager->GetPainter()->FillRect(lRight-45, 3, lRight-5, 20);
	mUiManager->GetPainter()->SetColor(Color(200, 200, 0));
	mUiManager->GetPainter()->PrintText(lFps, lRight-40, 5);
}

void GameClientMasterTicker::DrawPerformanceLineGraph2d() const
{
	const bool lDebugGraph = CURE_RTVAR_GET(UiCure::GetSettings(), RTVAR_DEBUG_ENABLE, false) &&
		CURE_RTVAR_GET(UiCure::GetSettings(), RTVAR_DEBUG_PERFORMANCE_GRAPH, false);
	if (!lDebugGraph)
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
	int lY = lMargin + CURE_RTVAR_GET(UiCure::GetSettings(), RTVAR_DEBUG_PERFORMANCE_YOFFSET, 0);
	for (size_t lRootIndex = 0; lRootIndex < lRoots.size(); ++lRootIndex)
	{
		if (mPerformanceGraphList.size() <= lRootIndex)
		{
			return;
		}

		mPerformanceGraphList[lRootIndex].Render(lMargin, lScale, lY);

		const bool lDebugNames = CURE_RTVAR_GET(UiCure::GetSettings(), RTVAR_DEBUG_PERFORMANCE_NAMES, false);
		if (lDebugNames)
		{
			mPerformanceGraphList[lRootIndex].RenderNames(lMargin, lY);
		}
	}

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
			UiCure::GetSettings()->SetValue(Cure::RuntimeVariable::TYPE_NORMAL,
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
