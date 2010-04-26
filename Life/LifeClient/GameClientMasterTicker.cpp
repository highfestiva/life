
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "GameClientMasterTicker.h"
#include <algorithm>
#include "../../Cure/Include/ResourceManager.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Lepra/Include/AntiCrack.h"
#include "../../Lepra/Include/Performance.h"
#include "../../Lepra/Include/SystemManager.h"
#include "../../UiCure/Include/UiGameUiManager.h"
#include "../../UiCure/Include/UiCppContextObject.h"
#include "../../UiTBC/Include/GUI/UiCenterLayout.h"
#include "../../UiTBC/Include/GUI/UiDesktopWindow.h"
#include "../../UiTBC/Include/GUI/UiFloatingLayout.h"
#include "../../UiTBC/Include/UiRenderer.h"
#include "../LifeApplication.h"
#include "../RtVar.h"
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
		new ConsoleExecutor<GameClientMasterTicker>(
			this, &GameClientMasterTicker::OnCommandLocal, &GameClientMasterTicker::OnCommandError));
	mConsole->GetConsoleCommandManager()->AddCommand(SET_PLAYER_COUNT);

	mConsole->ExecuteCommand(_T("execute-file -i ClientDefault.lsh"));
	mConsole->ExecuteCommand(_T("execute-file -i ") + Application::GetIoFile(_T("Base"), _T("lsh")));
}

GameClientMasterTicker::~GameClientMasterTicker()
{
	delete (mConsole);
	mConsole = 0;

	UiLepra::DisplayManager::EnableScreensaver(true);

	{
		StashCalibration();

		ConsoleManager lConsole(0, UiCure::GetSettings(), 0, 0);
		lConsole.Init();
		lConsole.ExecuteCommand(_T("save-system-config-file 0 " + Application::GetIoFile(_T("Base"), _T("lsh"))));
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

	ClosePlayerCountGui();

	mResourceManager = 0;
	mUiManager = 0;
}



bool GameClientMasterTicker::CreateSlave()
{
	const PixelRect lRenderArea(0, 0, mUiManager->GetDisplayManager()->GetWidth(), mUiManager->GetDisplayManager()->GetHeight());
	ScopeLock lLock(&mLock);
	bool lOk = (mSlaveArray.size() < 4);
	if (lOk)
	{
		int lFirstFreeSlaveMask = 0;
		SlaveArray::iterator x;
		for (x = mSlaveArray.begin(); x != mSlaveArray.end(); ++x)
		{
			lFirstFreeSlaveMask |= (1<<(*x)->GetSlaveIndex());
		}
		int lFirstFreeSlaveIndex = 0;
		for (lFirstFreeSlaveIndex = 0; lFirstFreeSlaveIndex < (int)sizeof(lFirstFreeSlaveMask)*8; ++lFirstFreeSlaveIndex)
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

	ScopeLock lLock(&mLock);

	SlaveArray::iterator x;

	{
		mLocalObjectSet.clear();
		for (x = mSlaveArray.begin(); x != mSlaveArray.end(); ++x)
		{
			(*x)->AddLocalObjects(mLocalObjectSet);
		}

		mUiManager->BeginRender();
		mUiManager->InputTick();
	}

	for (x = mSlaveArray.begin(); x != mSlaveArray.end();)
	{
		bool lDropSlave = (*x)->IsQuitting();
		if (lDropSlave)
		{
			GameClientSlaveManager* lSlave = *x;
			++x;
			RemoveSlave(lSlave);
			delete (lSlave);
			if (mSlaveArray.empty())
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

		int lSlaveIndex;
		// Kickstart physics so no slaves have to wait too long for completion.
		lSlaveIndex = 0;
		for (x = mSlaveArray.begin(); lOk && x != mSlaveArray.end(); ++x, ++lSlaveIndex)
		{
			mUiManager->GetSoundManager()->SetCurrentListener(lSlaveIndex, (int)mSlaveArray.size());
			lOk = (*x)->BeginTick();
		}
		// Start rendering machine directly afterwards.
		lSlaveIndex = 0;
		for (x = mSlaveArray.begin(); lOk && x != mSlaveArray.end(); ++x, ++lSlaveIndex)
		{
			mUiManager->GetSoundManager()->SetCurrentListener(lSlaveIndex, (int)mSlaveArray.size());
			lOk = (*x)->Render();
		}
	}

	{
		LEPRA_MEASURE_SCOPE(UiPaint);
		mUiManager->Paint();
	}

	{
		LEPRA_MEASURE_SCOPE(DrawGraph);
		DrawFps();
		DrawPerformanceLineGraph2d();
	}

	{
		for (x = mSlaveArray.begin(); lOk && x != mSlaveArray.end(); ++x)
		{
			lOk = (*x)->EndTick();
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
		(*x)->DoGetSiblings(pObjectId, pSiblingArray);
	}
}



void GameClientMasterTicker::AddSlave(GameClientSlaveManager* pSlave)
{
	{
		ScopeLock lLock(&mLock);
		pSlave->LoadSettings();
		mSlaveArray.push_back(pSlave);
	}
}

void GameClientMasterTicker::RemoveSlave(GameClientSlaveManager* pSlave)
{
	{
		ScopeLock lLock(&mLock);
		std::remove(mSlaveArray.begin(), mSlaveArray.end(), pSlave);
		UpdateSlaveLayout();
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
	assert(!mPlayerCountView);
	mPlayerCountView = new PlayerCountView(this);
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
	SlaveArray::iterator x;
	for (x = mSlaveArray.begin(); x != mSlaveArray.end(); ++x)
	{
		(*x)->Close();
	}
	mResourceManager->StopClear();
	mUiManager->Close();
	SystemManager::AddQuitRequest(-1);

	// Reopen.
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
			Color::Color(255, 255, 255), 1, 20);
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
			lOk = (*x)->Open();
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

	const PixelRect lRenderArea(0, 0, mUiManager->GetDisplayManager()->GetWidth(), mUiManager->GetDisplayManager()->GetHeight());
	switch (mSlaveArray.size())
	{
		case 0:
		{
			// Do nothing; no client to move.
		}
		break;
		case 1:
		{
			mSlaveArray[0]->SetRenderArea(lRenderArea);
		}
		break;
		case 2:
		{
			PixelRect lSideRenderArea(lRenderArea);
			lSideRenderArea.mRight /= 2;
			mSlaveArray[0]->SetRenderArea(lSideRenderArea);
			lSideRenderArea.Offset(lSideRenderArea.GetWidth(), 0);
			mSlaveArray[1]->SetRenderArea(lSideRenderArea);
		}
		break;
		case 3:
		{
			PixelRect lTopRenderArea(lRenderArea);
			lTopRenderArea.mRight /= 2;
			lTopRenderArea.mBottom = (int)(lTopRenderArea.mBottom*0.6);
			mSlaveArray[0]->SetRenderArea(lTopRenderArea);
			lTopRenderArea.Offset(lTopRenderArea.GetWidth(), 0);
			mSlaveArray[1]->SetRenderArea(lTopRenderArea);
			PixelRect lBottomRenderArea(lRenderArea);
			lBottomRenderArea.mTop = lTopRenderArea.mBottom;
			mSlaveArray[2]->SetRenderArea(lBottomRenderArea);
		}
		break;
		case 4:
		{
			PixelRect lPartRenderArea(lRenderArea);
			lPartRenderArea.mRight /= 2;
			lPartRenderArea.mBottom /= 2;
			mSlaveArray[0]->SetRenderArea(lPartRenderArea);
			lPartRenderArea.Offset(lPartRenderArea.GetWidth(), 0);
			mSlaveArray[0]->SetRenderArea(lPartRenderArea);
			lPartRenderArea.Offset(-lPartRenderArea.GetWidth(), lPartRenderArea.GetHeight());
			mSlaveArray[0]->SetRenderArea(lPartRenderArea);
			lPartRenderArea.Offset(lPartRenderArea.GetWidth(), 0);
			mSlaveArray[0]->SetRenderArea(lPartRenderArea);
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
	typedef ScopePerformanceData::NodeArray ScopeArray;
	ScopeArray lRoots = ScopePerformanceData::GetRoots();
	ScopeArray lNodes;
	lNodes.reserve(100);
	for (size_t lRootIndex = 0; lRootIndex < lRoots.size(); ++lRootIndex)
	{
		if (mPerformanceGraphList.size() <= lRootIndex)
		{
			mPerformanceGraphList.push_back(UiCure::LineGraph2d(mUiManager->GetPainter()));
		}
		mPerformanceGraphList[lRootIndex].TickLine(lHeight);

		lNodes.clear();
		lNodes.push_back(lRoots[lRootIndex]);
		const double lRootStart = lNodes[0]->GetTimeOfLastMeasure();
		for (size_t x = 0; x < lNodes.size(); ++x)
		{
			const ScopePerformanceData* lNode = lNodes[x];
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
	const bool lDebugGraph = CURE_RTVAR_TRYGET(UiCure::GetSettings(), RTVAR_DEBUG_ENABLE, false) &&
		CURE_RTVAR_TRYGET(UiCure::GetSettings(), RTVAR_DEBUG_PERFORMANCE_GRAPH, false);
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
	int lY = lMargin;
	for (size_t lRootIndex = 0; lRootIndex < lRoots.size(); ++lRootIndex)
	{
		if (mPerformanceGraphList.size() <= lRootIndex)
		{
			return;
		}

		mPerformanceGraphList[lRootIndex].Render(lMargin, lScale, lY);

		const bool lDebugNames = CURE_RTVAR_TRYGET(UiCure::GetSettings(), RTVAR_DEBUG_PERFORMANCE_NAMES, false);
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
	SlaveArray::iterator x = mSlaveArray.begin();
	for (; !lConsumed && x != mSlaveArray.end(); ++x)
	{
		lConsumed = (*x)->OnKeyDown(pKeyCode);
	}
	return (lConsumed);
}

bool GameClientMasterTicker::OnKeyUp(UiLepra::InputManager::KeyCode pKeyCode)
{
	bool lConsumed = false;
	SlaveArray::iterator x = mSlaveArray.begin();
	for (; !lConsumed && x != mSlaveArray.end(); ++x)
	{
		lConsumed = (*x)->OnKeyUp(pKeyCode);
	}
	return (lConsumed);
}

void GameClientMasterTicker::OnInput(UiLepra::InputElement* pElement)
{
	log_volatile(mLog.Tracef(_T("Got input from element '%s' / '%s' = %f.\n"),
		pElement->GetFullName().c_str(),
		pElement->GetIdentifier().c_str(),
		pElement->GetValue()));

	SlaveArray::iterator x = mSlaveArray.begin();
	for (; x != mSlaveArray.end(); ++x)
	{
		(*x)->OnInput(pElement);
	}
}



void GameClientMasterTicker::ClosePlayerCountGui()
{
	if (mPlayerCountView)
	{
		mUiManager->GetDesktopWindow()->RemoveChild(mPlayerCountView, 0);
		delete (mPlayerCountView);
		mPlayerCountView = 0;
	}
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
