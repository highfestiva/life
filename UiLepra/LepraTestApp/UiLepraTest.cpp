
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#ifndef CURE_TEST_WITHOUT_UI

#include <assert.h>
#include <math.h>
#include "../../Lepra/Include/Log.h"
#include "../../Lepra/Include/SystemManager.h"
#include "../../Lepra/Include/Thread.h"
#include "../../Lepra/Include/Timer.h"
#include "../Include/UiCore.h"
#include "../Include/UiDisplayManager.h"
#include "../Include/UiLepra.h"
#include "../Include/UiSoundManager.h"
#include "../Include/UiInput.h"



using namespace Lepra;
class UiLepraTest{};
static Lepra::LogDecorator gUiLLog(Lepra::LogType::GetLog(Lepra::LogType::SUB_TEST), typeid(UiLepraTest));
void ReportTestResult(const Lepra::LogDecorator& pLog, const str& pTestName, const str& pContext, bool pResult);
bool TestLepra();



bool TestInput(const Lepra::LogDecorator& pLog)
{
	str lContext;
	bool lTestOk = true;
	const UiLepra::InputManager* lInputManager = 0;

	if (lTestOk)
	{
		lInputManager = UiLepra::InputManager::CreateInputManager(0);
		lTestOk = (lInputManager != 0);
	}
	if (lTestOk)
	{
		lContext = _T("no keyboard available");
		lTestOk = (lInputManager->GetKeyboard() != 0);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("no mouse available");
		lTestOk = (lInputManager->GetMouse() != 0);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("number of devices");
		lTestOk = (lInputManager->GetDeviceList().size() >= 2 &&
			lInputManager->GetDeviceList().size() <= 7);
		assert(lTestOk);
	}
	if (lInputManager)
	{
		delete (lInputManager);
		lInputManager = 0;
	}

	ReportTestResult(pLog, _T("Input"), lContext, lTestOk);
	return (lTestOk);
}

bool TestGraphics(const Lepra::LogDecorator& pLog)
{
	str lContext;
	bool lTestOk = true;

	UiLepra::DisplayManager* lDisplay = UiLepra::DisplayManager::CreateDisplayManager(UiLepra::DisplayManager::OPENGL_CONTEXT);
	assert(lDisplay);

	UiLepra::DisplayMode lDisplayMode;
	if (lTestOk)
	{
		lContext = _T("find display mode");
		lTestOk = lDisplay->FindDisplayMode(lDisplayMode, 640, 480);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("open screen");
		lTestOk = lDisplay->OpenScreen(lDisplayMode, UiLepra::DisplayManager::WINDOWED);
		assert(lTestOk);
	}

	if (lTestOk)
	{
		Lepra::Canvas lScreen(lDisplay->GetWidth(), lDisplay->GetHeight(), Lepra::Canvas::IntToBitDepth(lDisplay->GetBitDepth()));
		Lepra::Timer lTimer;
		lTimer.UpdateTimer();
		lTimer.ClearTimeDiff();
		while (lTimer.GetTimeDiff() < 0.5 && !Lepra::SystemManager::GetQuitRequest())
		{
			UiLepra::Core::ProcessMessages();
			Lepra::Thread::YieldCpu();
			lScreen.SetBuffer(0);
			lDisplay->UpdateScreen();
			lTimer.UpdateTimer();
		}
		lDisplay->CloseScreen();
	}

	delete (lDisplay);

	ReportTestResult(pLog, _T("Graphics"), lContext, lTestOk);
	return (lTestOk);
}

bool TestSound(const Lepra::LogDecorator& pLog)
{
	str lContext;
	bool lTestOk = true;

	UiLepra::SoundManager::SoundID lSound = UiLepra::INVALID_SOUNDID;
	UiLepra::SoundManager* lSoundManager = UiLepra::SoundManager::CreateSoundManager(UiLepra::SoundManager::CONTEXT_OPENAL);
	assert(lSoundManager);
	if (lTestOk)
	{
		lContext = _T("load sound");
		lSound = lSoundManager->LoadSound3D(_T("Bark.wav"), UiLepra::SoundManager::LOOP_FORWARD, 0);
		lTestOk = (lSound != UiLepra::INVALID_SOUNDID);
		assert(lTestOk);
	}
	UiLepra::SoundManager::SoundInstanceID lSoundInstance = UiLepra::INVALID_SOUNDINSTANCEID;
	if (lTestOk)
	{
		lContext = _T("create sound instance");
		lSoundInstance = lSoundManager->CreateSoundInstance(lSound);
		lTestOk = (lSoundInstance != UiLepra::INVALID_SOUNDID);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("play sound");
		lSoundManager->Play(lSoundInstance, 1, 1);
		for (float x = 0; x < 6*5; x += 2.0f)
		{
			Lepra::Vector3DF lPosition(::sinf(x)*10, ::cosf(x)*10, 0);
			Lepra::Vector3DF lVelocity;
			lSoundManager->SetSoundPosition(lSoundInstance, lPosition, lVelocity);
			Lepra::Thread::Sleep(0.3);
		}
	}
	if (lSoundInstance != UiLepra::INVALID_SOUNDINSTANCEID)
	{
		lSoundManager->DeleteSoundInstance(lSoundInstance);
	}
	if (lSound != UiLepra::INVALID_SOUNDID)
	{
		lSoundManager->Release(lSound);
	}
	delete (lSoundManager);

	ReportTestResult(pLog, _T("Sound"), lContext, lTestOk);
	return (lTestOk);
}

bool TestUiLepra()
{
	bool lTestOk = true;

	if (lTestOk)
	{
		lTestOk = TestLepra();
	}
	if (lTestOk)
	{
		lTestOk = TestInput(gUiLLog);
	}
	if (lTestOk)
	{
		lTestOk = TestGraphics(gUiLLog);
	}
	if (lTestOk)
	{
		lTestOk = TestSound(gUiLLog);
	}

	return (lTestOk);
}

#endif //!CURE_TEST_WITHOUT_UI
