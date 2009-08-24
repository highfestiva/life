
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games



#include "../../Cure/Include/ResourceManager.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Lepra/Include/AntiCrack.h"
#include "../../Lepra/Include/SystemManager.h"
#include "../../UiCure/Include/UiGameUiManager.h"
#include "../../UiCure/Include/UiCppContextObject.h"
#include "../../UiCure/Include/UiSoundManager.h"
#include "../../UiTbc/Include/GUI/UiCenterLayout.h"
#include "../../UiTbc/Include/GUI/UiDesktopWindow.h"
#include "../../UiTbc/Include/GUI/UiFloatingLayout.h"
#include "../../UiTbc/Include/UiRenderer.h"
#include "GameClientMasterTicker.h"
#include "GameClientSlaveManager.h"



// Run before main() is started.
AntiCrack _r__;



namespace Life
{



GameClientMasterTicker::GameClientMasterTicker(UiCure::GameUiManager* pUiManager, Cure::ResourceManager* pResourceManager):
	mUiManager(pUiManager),
	mResourceManager(pResourceManager),
	mRestartUi(false),
	mInitialized(false),
	mActiveWidth(0),
	mActiveHeight(0)
{
	UiLepra::DisplayManager::EnableScreensaver(false);

	ConsoleManager lConsole(0, UiCure::GetSettings(), 0, 0);
	lConsole.Init();
	lConsole.ExecuteCommand(_T("execute-file -i ClientBase.lsh"));
}

GameClientMasterTicker::~GameClientMasterTicker()
{
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
	bool lOk = true;

	Lepra::ScopeLock lLock(&mLock);

	mUiManager->BeginRender();

	mUiManager->InputTick();

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

	for (x = mSlaveSet.First(); lOk && x != mSlaveSet.End(); ++x)
	{
		lOk = x.GetObject()->BeginTick();
		if (lOk)
		{
			lOk = x.GetObject()->Render();
		}
	}

	mUiManager->Paint();

	int lSlaveIndex = 0;
	for (x = mSlaveSet.First(); lOk && x != mSlaveSet.End(); ++x, ++lSlaveIndex)
	{
		mUiManager->GetSoundManager()->SetActiveListener(lSlaveIndex);
		lOk = x.GetObject()->EndTick();
	}

	{
		Lepra::ScopeTimer lTime(&mResourceTime);
		// This must be synchronous. The reason is that it may add objects to "script" or "physics" enginges,
		// which are run by a separate thread; parallelization here will certainly cause threading errors.
		mResourceManager->Tick();
	}

	mUiManager->EndRender();

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
	UiTbc::Window* lWindow = new PlayerCountView(mUiManager->GetPainter(), this);
	mUiManager->AssertDesktopLayout(new UiTbc::CenterLayout());
	mUiManager->GetDesktopWindow()->AddChild(lWindow);
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



float GameClientMasterTicker::GetPowerSaveAmount() const
{
	bool lIsMinimized = !mUiManager->GetDisplayManager()->IsVisible();
	return (lIsMinimized? 0.4f : 0);
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



void GameClientMasterTicker::OnExit(View* pPlayerCountView)
{
	mLog.Headline(_T("Number of players not picked, quitting."));
	Lepra::SystemManager::SetQuitRequest(true);
	mUiManager->GetDesktopWindow()->RemoveChild(pPlayerCountView, 0);
	delete (pPlayerCountView);
}

void GameClientMasterTicker::OnSetPlayerCount(View* pPlayerCountView, int pPlayerCount)
{
	mUiManager->GetDesktopWindow()->RemoveChild(pPlayerCountView, 0);
	delete (pPlayerCountView);

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
