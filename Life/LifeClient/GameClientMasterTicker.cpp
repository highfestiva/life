
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "GameClientMasterTicker.h"
#include <algorithm>
#include "../../Cure/Include/ResourceManager.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Cure/Include/TimeManager.h"
#include "../../Lepra/Include/AntiCrack.h"
#include "../../Lepra/Include/Performance.h"
#include "../../Lepra/Include/SystemManager.h"
#include "../../UiCure/Include/UiGameUiManager.h"
#include "../../UiCure/Include/UiCppContextObject.h"
#include "../../UiCure/Include/UiRuntimeVariableName.h"
#include "../../UiTBC/Include/GUI/UiCenterLayout.h"
#include "../../UiTBC/Include/GUI/UiDesktopWindow.h"
#include "../../UiTBC/Include/GUI/UiFloatingLayout.h"
#include "../../UiTBC/Include/UiRenderer.h"
#include "../LifeApplication.h"
#include "../RtVar.h"
#include "GameClientSlaveManager.h"
#include "GameClientViewer.h"
#include "RoadSignButton.h"



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
	mActiveHeight(0),
	mActiveSlaveCount(0),
	mSlaveTopSplit(1),
	mSlaveBottomSplit(1),
	mSlaveVSplit(1),
	mSlaveFade(0)
{
	mSlaveArray.resize(4, 0);

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
	return (CreateSlave(&GameClientMasterTicker::CreateSlaveManager));
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
			GameClientSlaveManager* lSlave = *x;
			if (lSlave)
			{
				lSlave->AddLocalObjects(mLocalObjectSet);
			}
		}

		mUiManager->BeginRender();
		mUiManager->InputTick();
	}

	{
		LEPRA_MEASURE_SCOPE(RenderSlaves);

		int lSlaveIndex;
		// Kickstart physics so no slaves have to wait too long for completion.
		lSlaveIndex = 0;
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
		// Start rendering machine directly afterwards.
		lSlaveIndex = 0;
		for (x = mSlaveArray.begin(); lOk && x != mSlaveArray.end(); ++x)
		{
			GameClientSlaveManager* lSlave = *x;
			if (lSlave)
			{
				mUiManager->GetSoundManager()->SetCurrentListener(lSlaveIndex, mActiveSlaveCount);
				lOk = lSlave->Render();
			}
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

	if (mActiveWidth != mUiManager->GetDisplayManager()->GetWidth() ||
		mActiveHeight != mUiManager->GetDisplayManager()->GetHeight())
	{
		mActiveWidth = mUiManager->GetDisplayManager()->GetWidth();
		mActiveHeight = mUiManager->GetDisplayManager()->GetHeight();
	}
	UpdateSlaveLayout();

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
	float lFov = (float)CURE_RTVAR_GET(UiCure::GetSettings(), RTVAR_UI_3D_FOV, 90.0);
	float lClipNear = (float)CURE_RTVAR_GET(UiCure::GetSettings(), RTVAR_UI_3D_CLIPNEAR, 0.1);
	float lClipFar = (float)CURE_RTVAR_GET(UiCure::GetSettings(), RTVAR_UI_3D_CLIPFAR, 1000.0);
	mUiManager->GetRenderer()->SetViewFrustum(lFov, lClipNear, lClipFar);
	return (lFov);
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

void GameClientMasterTicker::DeleteSlave(GameClientSlaveManager* pSlave)
{
	{
		ScopeLock lLock(&mLock);
		assert(mSlaveArray[pSlave->GetSlaveIndex()]);
		mSlaveArray[pSlave->GetSlaveIndex()] = 0;
		delete (pSlave);
		if (--mActiveSlaveCount == 0)
		{
			CreatePlayerCountWindow();
		}
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
	mUiManager->AssertDesktopLayout(new UiTbc::FloatingLayout());
	mUiManager->GetDesktopWindow()->AddChild(mPlayerCountView);
	mUiManager->GetDesktopWindow()->UpdateLayout();

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
	const float lLayoutSpeed = Math::GetIterateLerpTime(0.98f, lFrameTime);

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
			DeleteSlave(lLastSlave);
		}
	}
}

void GameClientMasterTicker::SlideSlaveLayout()
{
	const PixelRect lRenderArea(0, 0, mUiManager->GetDisplayManager()->GetWidth(), mUiManager->GetDisplayManager()->GetHeight());
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
				DeleteSlave(mSlaveArray[x]);
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
	log_volatile(mLog.Tracef(_T("Got input from element '%s' / '%s' = %f.\n"),
		pElement->GetFullName().c_str(),
		pElement->GetIdentifier().c_str(),
		pElement->GetValue()));

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
