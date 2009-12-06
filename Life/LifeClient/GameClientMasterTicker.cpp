
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games



#include "../../Cure/Include/ResourceManager.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Lepra/Include/AntiCrack.h"
#include "../../Lepra/Include/Performance.h"
#include "../../Lepra/Include/SystemManager.h"
#include "../../UiCure/Include/UiGameUiManager.h"
#include "../../UiCure/Include/UiCppContextObject.h"
#include "../../UiCure/Include/UiSoundManager.h"
#include "../../UiTbc/Include/GUI/UiCenterLayout.h"
#include "../../UiTbc/Include/GUI/UiDesktopWindow.h"
#include "../../UiTbc/Include/GUI/UiFloatingLayout.h"
#include "../../UiTbc/Include/UiRenderer.h"
#include "../RtVar.h"
#include "GameClientMasterTicker.h"
#include "GameClientSlaveManager.h"



// Run before main() is started.
AntiCrack _r__;



namespace Life
{



#define SET_PLAYER_COUNT	_T("set-player-count")



GameClientMasterTicker::GameClientMasterTicker(UiCure::GameUiManager* pUiManager, Cure::ResourceManager* pResourceManager):
	mUiManager(pUiManager),
	mResourceManager(pResourceManager),
	mPlayerCountView(0),
	mRestartUi(false),
	mInitialized(false),
	mActiveWidth(0),
	mActiveHeight(0)
{
	UiLepra::DisplayManager::EnableScreensaver(false);

	mConsole = new ConsoleManager(0, UiCure::GetSettings(), 0, 0);
	mConsole->Init();
	mConsole->GetConsoleCommandManager()->AddExecutor(
		new Lepra::ConsoleExecutor<GameClientMasterTicker>(
			this, &GameClientMasterTicker::OnCommandLocal, &GameClientMasterTicker::OnCommandError));
	mConsole->GetConsoleCommandManager()->AddCommand(SET_PLAYER_COUNT);
	mConsole->ExecuteCommand(_T("execute-file -i ClientBase.lsh"));
}

GameClientMasterTicker::~GameClientMasterTicker()
{
	delete (mConsole);
	mConsole = 0;

	UiLepra::DisplayManager::EnableScreensaver(true);

	{
		ConsoleManager lConsole(0, UiCure::GetSettings(), 0, 0);
		lConsole.Init();
		lConsole.ExecuteCommand(_T("save-system-config-file 0 ClientBase.lsh"));
	}

	mUiManager->GetInputManager()->ReleaseAll();
	mUiManager->GetInputManager()->RemoveKeyCodeInputObserver(this);

	SlaveMap::Iterator x;
	for (x = mSlaveSet.First(); x != mSlaveSet.End(); ++x)
	{
		GameClientSlaveManager* lSlave = x.GetObject();
		delete (lSlave);
	}
	mSlaveSet.RemoveAll();

	delete (mPlayerCountView);
	mPlayerCountView = 0;

	mResourceManager = 0;
	mUiManager = 0;
}



bool GameClientMasterTicker::CreateSlave()
{
	const Lepra::PixelRect lRenderArea(0, 0, mUiManager->GetDisplayManager()->GetWidth(), mUiManager->GetDisplayManager()->GetHeight());
	Lepra::ScopeLock lLock(&mLock);
	bool lOk = (mSlaveSet.GetCount() < 4);
	if (lOk)
	{
		int lFirstFreeSlaveMask = 0;
		SlaveMap::Iterator x;
		for (x = mSlaveSet.First(); x != mSlaveSet.End(); ++x)
		{
			lFirstFreeSlaveMask |= (1<<x.GetObject()->GetSlaveIndex());
		}
		int lFirstFreeSlaveIndex = 0;
		for (lFirstFreeSlaveIndex = 0; lFirstFreeSlaveIndex < sizeof(lFirstFreeSlaveMask)*8; ++lFirstFreeSlaveIndex)
		{
			if (!(lFirstFreeSlaveMask&(1<<lFirstFreeSlaveIndex)))
			{
				break;
			}
		}
		Cure::RuntimeVariableScope* lVariables = new Cure::RuntimeVariableScope(UiCure::GetSettings());
		GameClientSlaveManager* lSlave = new GameClientSlaveManager(this, lVariables, mResourceManager,
			mUiManager, lFirstFreeSlaveIndex, lRenderArea);
		AddSlave(lSlave);
		if (mInitialized)
		{
			lOk = lSlave->Open();
		}
		UpdateSlaveLayout();
	}
	else
	{
		mLog.AError("Could not create another split screen player - not supported.");
	}
	return (lOk);
}



bool GameClientMasterTicker::Tick()
{
	LEPRA_MEASURE_SCOPE(MasterTicker);

	bool lOk = true;

	Lepra::ScopeLock lLock(&mLock);

	{
		mUiManager->BeginRender();
	}

	{
		mUiManager->InputTick();
	}

	SlaveMap::Iterator x;
	for (x = mSlaveSet.First(); x != mSlaveSet.End();)
	{
		bool lDropSlave = x.GetObject()->IsQuitting();
		if (lDropSlave)
		{
			GameClientSlaveManager* lSlave = x.GetObject();
			++x;
			RemoveSlave(lSlave);
			delete (lSlave);
			if (mSlaveSet.IsEmpty())
			{
				CreatePlayerCountWindow();
			}
		}
		else
		{
			++x;
		}
	}

	{
		LEPRA_MEASURE_SCOPE(RenderSlaves);

		for (x = mSlaveSet.First(); lOk && x != mSlaveSet.End(); ++x)
		{
			lOk = x.GetObject()->BeginTick();
			if (lOk)
			{
				lOk = x.GetObject()->Render();
			}
		}
	}

	{
		LEPRA_MEASURE_SCOPE(UiPaint);
		mUiManager->Paint();

	}

	{
		int lSlaveIndex = 0;
		for (x = mSlaveSet.First(); lOk && x != mSlaveSet.End(); ++x, ++lSlaveIndex)
		{
			mUiManager->GetSoundManager()->SetActiveListener(lSlaveIndex);
			lOk = x.GetObject()->EndTick();
		}
	}

	{
		LEPRA_MEASURE_SCOPE(DrawGraph);
		DrawFps();
		DrawPerformanceLineGraph2d();
	}

	{
		LEPRA_MEASURE_SCOPE(ResourceTick);
		// This must be synchronous. The reason is that it may add objects to "script" or "physics" enginges,
		// which are run by a separate thread; parallelization here will certainly cause threading errors.
		mResourceManager->Tick();
	}

	{
		LEPRA_MEASURE_SCOPE(UiEndRender);
		mUiManager->EndRender();
	}

	if (mActiveWidth != mUiManager->GetDisplayManager()->GetWidth() ||
		mActiveHeight != mUiManager->GetDisplayManager()->GetHeight())
	{
		mActiveWidth = mUiManager->GetDisplayManager()->GetWidth();
		mActiveHeight = mUiManager->GetDisplayManager()->GetHeight();
		UpdateSlaveLayout();
	}

	if (mRestartUi)
	{
		if (!Reinitialize())
		{
			mLog.Fatal(_T("Failure to re-initialize UI! Going down now!"));
			Lepra::SystemManager::SetQuitRequest(true);
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



bool GameClientMasterTicker::StartResetUi()
{
	mRestartUi = true;
	return (true);
}

bool GameClientMasterTicker::WaitResetUi()
{
	for (int x = 0; mRestartUi && x < 50; ++x)
	{
		Lepra::Thread::Sleep(0.1);
	}
	return (!mRestartUi);
}



void GameClientMasterTicker::AddSlave(GameClientSlaveManager* pSlave)
{
	{
		Lepra::ScopeLock lLock(&mLock);
		pSlave->LoadSettings();
		mSlaveSet.PushBack(pSlave, pSlave);
	}
	if (mUiManager->GetSoundManager())
	{
		mUiManager->GetSoundManager()->SetListenerCount((int)mSlaveSet.GetCount());
	}
}

void GameClientMasterTicker::RemoveSlave(GameClientSlaveManager* pSlave)
{
	{
		Lepra::ScopeLock lLock(&mLock);
		mSlaveSet.Remove(pSlave);
		UpdateSlaveLayout();
	}
	if (mUiManager->GetSoundManager())
	{
		mUiManager->GetSoundManager()->SetListenerCount((int)mSlaveSet.GetCount());
	}
}



bool GameClientMasterTicker::Initialize()
{
	bool lOk = Reinitialize();
	if (lOk)
	{
		CreatePlayerCountWindow();
	}
	return (lOk);
}

void GameClientMasterTicker::CreatePlayerCountWindow()
{
	assert(!mPlayerCountView);
	mPlayerCountView = new PlayerCountView(mUiManager->GetPainter(), this);
	mUiManager->AssertDesktopLayout(new UiTbc::CenterLayout());
	mUiManager->GetDesktopWindow()->AddChild(mPlayerCountView);
	mUiManager->GetDesktopWindow()->UpdateLayout();
}

bool GameClientMasterTicker::Reinitialize()
{
	mInitialized = false;

	// Close all gfx/snd stuff.
	if (mUiManager->GetInputManager())
	{
		mUiManager->GetInputManager()->RemoveKeyCodeInputObserver(this);
	}
	SlaveMap::Iterator x;
	for (x = mSlaveSet.First(); x != mSlaveSet.End(); ++x)
	{
		x.GetObject()->Close();
	}
	mResourceManager->StopClear();
	bool lQuit = Lepra::SystemManager::GetQuitRequest();
	mUiManager->Close();
	Lepra::SystemManager::SetQuitRequest(lQuit);

	// Reopen.
	bool lOk = mResourceManager->InitDefault();
	if (lOk)
	{
		lOk = mUiManager->Open();
	}
	if (lOk)
	{
		mUiManager->GetDesktopWindow()->CreateLayer(new UiTbc::FloatingLayout());

		mUiManager->GetSoundManager()->SetListenerCount((int)mSlaveSet.GetCount());

		// TODO: replace with world-load.
		mUiManager->GetRenderer()->AddDirectionalLight(
			UiTbc::Renderer::LIGHT_STATIC, Lepra::Vector3DF(0, 0.5f, -1),
			Lepra::Color::Color(255, 255, 255), 1, 100);
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
					Lepra::String lInterpretation;
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

		SlaveMap::Iterator x;
		for (x = mSlaveSet.First(); lOk && x != mSlaveSet.End(); ++x)
		{
			lOk = x.GetObject()->Open();
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

	const Lepra::PixelRect lRenderArea(0, 0, mUiManager->GetDisplayManager()->GetWidth(), mUiManager->GetDisplayManager()->GetHeight());
	switch (mSlaveSet.GetCount())
	{
		case 0:
		{
			// Do nothing; no client to move.
		}
		break;
		case 1:
		{
			mSlaveSet.First().GetObject()->SetRenderArea(lRenderArea);
		}
		break;
		case 2:
		{
			Lepra::PixelRect lSideRenderArea(lRenderArea);
			lSideRenderArea.mRight /= 2;
			mSlaveSet.First().GetObject()->SetRenderArea(lSideRenderArea);
			lSideRenderArea.Offset(lSideRenderArea.GetWidth(), 0);
			(++mSlaveSet.First()).GetObject()->SetRenderArea(lSideRenderArea);
		}
		break;
		case 3:
		{
			Lepra::PixelRect lTopRenderArea(lRenderArea);
			lTopRenderArea.mRight /= 2;
			lTopRenderArea.mBottom = (int)(lTopRenderArea.mBottom*0.6);
			mSlaveSet.First().GetObject()->SetRenderArea(lTopRenderArea);
			lTopRenderArea.Offset(lTopRenderArea.GetWidth(), 0);
			(++mSlaveSet.First()).GetObject()->SetRenderArea(lTopRenderArea);
			Lepra::PixelRect lBottomRenderArea(lRenderArea);
			lBottomRenderArea.mTop = lTopRenderArea.mBottom;
			(++(++mSlaveSet.First())).GetObject()->SetRenderArea(lBottomRenderArea);
		}
		break;
		case 4:
		{
			Lepra::PixelRect lPartRenderArea(lRenderArea);
			lPartRenderArea.mRight /= 2;
			lPartRenderArea.mBottom /= 2;
			mSlaveSet.First().GetObject()->SetRenderArea(lPartRenderArea);
			lPartRenderArea.Offset(lPartRenderArea.GetWidth(), 0);
			(++mSlaveSet.First()).GetObject()->SetRenderArea(lPartRenderArea);
			lPartRenderArea.Offset(-lPartRenderArea.GetWidth(), lPartRenderArea.GetHeight());
			(++(++mSlaveSet.First())).GetObject()->SetRenderArea(lPartRenderArea);
			lPartRenderArea.Offset(lPartRenderArea.GetWidth(), 0);
			(++(++(++mSlaveSet.First()))).GetObject()->SetRenderArea(lPartRenderArea);
		}
		break;
		default:
		{
			mLog.AError("Too many clients, unable to layout.");
		}
		break;
	}
}

void GameClientMasterTicker::Profile()
{
	const bool lDebugGraph = CURE_RTVAR_TRYGET(UiCure::GetSettings(), RTVAR_DEBUG_PERFORMANCE_GRAPH, false);
	if (!lDebugGraph)
	{
		return;
	}

	const int lHeight = 100;
	typedef Lepra::ScopePerformanceData::NodeArray ScopeArray;
	ScopeArray lRoots = Lepra::ScopePerformanceData::GetRoots();
	ScopeArray lNodes;
	lNodes.reserve(100);
	for (size_t lRootIndex = 0; lRootIndex < lRoots.size(); ++lRootIndex)
	{
		if (mPerformanceGraphList.size() <= lRootIndex)
		{
			mPerformanceGraphList.push_back(UiCure::LineGraph2d());
		}
		mPerformanceGraphList[lRootIndex].TickLine(lHeight);

		lNodes.clear();
		lNodes.push_back(lRoots[lRootIndex]);
		const double lRootStart = lNodes[0]->GetTimeOfLastMeasure();
		for (size_t x = 0; x < lNodes.size(); ++x)
		{
			const Lepra::ScopePerformanceData* lNode = lNodes[x];
			const ScopeArray& lChildren = lNode->GetChildren();
			lNodes.insert(lNodes.end(), lChildren.begin(), lChildren.end());

			const double lStart = lNode->GetTimeOfLastMeasure() - lRootStart;
			mPerformanceGraphList[lRootIndex].AddSegment(lNode->GetName(), lStart, lStart + lNode->GetLast());
		}
	}
}

void GameClientMasterTicker::DrawFps() const
{
	const bool lDebugging = CURE_RTVAR_TRYGET(UiCure::GetSettings(), RTVAR_DEBUG_ENABLE, false);
	if (!lDebugging)
	{
		return;
	}

	Lepra::ScopePerformanceData* lMainLoop = Lepra::ScopePerformanceData::GetRoots()[0];
	Lepra::String lFps = Lepra::StringUtility::Format(_T("%.1f"), 1/lMainLoop->GetSlidingAverage());
	mUiManager->GetPainter()->SetColor(Lepra::Color(0, 0, 0));
	const int lRight = mUiManager->GetDisplayManager()->GetWidth();
	mUiManager->GetPainter()->FillRect(lRight-45, 3, lRight-5, 20);
	mUiManager->GetPainter()->SetColor(Lepra::Color(200, 200, 0));
	mUiManager->GetPainter()->PrintText(lFps, lRight-40, 5);
}

void GameClientMasterTicker::DrawPerformanceLineGraph2d() const
{
	const bool lDebugGraph = CURE_RTVAR_TRYGET(UiCure::GetSettings(), RTVAR_DEBUG_ENABLE, false) &&
		CURE_RTVAR_TRYGET(UiCure::GetSettings(), RTVAR_DEBUG_PERFORMANCE_GRAPH, false);
	if (!lDebugGraph)
	{
		return;
	}

	// Draw all scope nodes as line segments in one hunky graph.
	typedef Lepra::ScopePerformanceData::NodeArray ScopeArray;

	float lLongestRootTime = 1e-15f;
	ScopeArray lRoots = Lepra::ScopePerformanceData::GetRoots();
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
	int lY = lMargin;
	for (size_t lRootIndex = 0; lRootIndex < lRoots.size(); ++lRootIndex)
	{
		if (mPerformanceGraphList.size() <= lRootIndex)
		{
			return;
		}

		mPerformanceGraphList[lRootIndex].Render(mUiManager->GetPainter(), lMargin, lScale, lY);

		const bool lDebugNames = CURE_RTVAR_TRYGET(UiCure::GetSettings(), RTVAR_DEBUG_PERFORMANCE_NAMES, false);
		if (lDebugNames)
		{
			mPerformanceGraphList[lRootIndex].RenderNames(mUiManager->GetPainter(), lMargin, lY);
		}
	}

}



float GameClientMasterTicker::GetPowerSaveAmount() const
{
	bool lIsMinimized = !mUiManager->GetDisplayManager()->IsVisible();
	return (lIsMinimized? 0.4f : 0);
}



int GameClientMasterTicker::OnCommandLocal(const Lepra::String& pCommand, const Lepra::StringUtility::StringVector& pParameterVector)
{
	if (pCommand == SET_PLAYER_COUNT)
	{
		int lPlayerCount = 0;
		if (pParameterVector.size() == 1 && Lepra::StringUtility::StringToInt(pParameterVector[0], lPlayerCount))
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

void GameClientMasterTicker::OnCommandError(const Lepra::String&, const Lepra::StringUtility::StringVector&, int)
{
}



bool GameClientMasterTicker::OnKeyDown(UiLepra::InputManager::KeyCode pKeyCode)
{
	bool lConsumed = false;
	SlaveMap::Iterator x = mSlaveSet.First();
	for (; !lConsumed && x != mSlaveSet.End(); ++x)
	{
		lConsumed = x.GetObject()->OnKeyDown(pKeyCode);
	}
	return (lConsumed);
}

bool GameClientMasterTicker::OnKeyUp(UiLepra::InputManager::KeyCode pKeyCode)
{
	bool lConsumed = false;
	SlaveMap::Iterator x = mSlaveSet.First();
	for (; !lConsumed && x != mSlaveSet.End(); ++x)
	{
		lConsumed = x.GetObject()->OnKeyUp(pKeyCode);
	}
	return (lConsumed);
}

void GameClientMasterTicker::OnInput(UiLepra::InputElement* pElement)
{
	log_volatile(mLog.Tracef(_T("Got input from element '%s' / '%s' = %f.\n"),
		pElement->GetFullName().c_str(),
		pElement->GetIdentifier().c_str(),
		pElement->GetValue()));

	SlaveMap::Iterator x = mSlaveSet.First();
	for (; x != mSlaveSet.End(); ++x)
	{
		x.GetObject()->OnInput(pElement);
	}
}



void GameClientMasterTicker::OnExit()
{
	mLog.Headline(_T("Number of players not picked, quitting."));
	Lepra::SystemManager::SetQuitRequest(true);
	mUiManager->GetDesktopWindow()->RemoveChild(mPlayerCountView, 0);
	delete (mPlayerCountView);
	mPlayerCountView = 0;
}

void GameClientMasterTicker::OnSetPlayerCount(int pPlayerCount)
{
	if (mPlayerCountView)
	{
		mUiManager->GetDesktopWindow()->RemoveChild(mPlayerCountView, 0);
		delete (mPlayerCountView);
		mPlayerCountView = 0;
	}

	for (int x = 0; x < pPlayerCount; ++x)
	{
		CreateSlave();
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
