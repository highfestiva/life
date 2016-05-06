
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#ifndef CURE_TEST_WITHOUT_UI
#include "../../lepra/include/lepraassert.h"
#include <math.h>
#include "../../lepra/include/log.h"
#include "../../lepra/include/systemmanager.h"
#include "../../lepra/include/thread.h"
#include "../../lepra/include/timer.h"
#include "../include/uicore.h"
#include "../include/uidisplaymanager.h"
#include "../include/uilepra.h"
#include "../include/uisoundmanager.h"
#include "../include/uiinput.h"



using namespace lepra;
class UiLepraTest{};
static lepra::LogDecorator gUiLLog(lepra::LogType::GetLogger(lepra::LogType::kTest), typeid(UiLepraTest));
void ReportTestResult(const lepra::LogDecorator& log, const str& test_name, const str& context, bool result);
bool TestLepra();



bool TestInput(const lepra::LogDecorator& log) {
	str _context;
	bool test_ok = true;
	const uilepra::InputManager* input_manager = 0;

	if (test_ok) {
		input_manager = uilepra::InputManager::CreateInputManager(0);
		test_ok = (input_manager != 0);
	}
	if (test_ok) {
		_context = "no keyboard available";
		test_ok = (input_manager->GetKeyboard() != 0);
		deb_assert(test_ok);
	}
	if (test_ok) {
		_context = "no mouse available";
		test_ok = (input_manager->GetMouse() != 0);
		deb_assert(test_ok);
	}
	if (test_ok) {
		_context = "number of devices";
		test_ok = (input_manager->GetDeviceList().size() >= 2 &&
			input_manager->GetDeviceList().size() <= 7);
		deb_assert(test_ok);
	}
	if (input_manager) {
		delete (input_manager);
		input_manager = 0;
	}

	ReportTestResult(log, "Input", _context, test_ok);
	return (test_ok);
}

bool TestGraphics(const lepra::LogDecorator& log) {
	str _context;
	bool test_ok = true;

	uilepra::DisplayManager* display = uilepra::DisplayManager::CreateDisplayManager(uilepra::DisplayManager::kOpenglContext);
	deb_assert(display);

	uilepra::DisplayMode display_mode;
	if (test_ok) {
		_context = "find display mode";
		test_ok = display->FindDisplayMode(display_mode, 640, 480) || display->FindDisplayMode(display_mode, 1920, 1080);
		deb_assert(test_ok);
	}
	if (test_ok) {
		_context = "open screen";
		test_ok = display->OpenScreen(display_mode, uilepra::DisplayManager::kWindowed, uilepra::DisplayManager::kOrientationAllowAny);
		deb_assert(test_ok);
	}

	if (test_ok) {
		lepra::Canvas screen(display->GetWidth(), display->GetHeight(), lepra::Canvas::IntToBitDepth(display->GetBitDepth()));
		lepra::Timer timer;
		timer.UpdateTimer();
		timer.ClearTimeDiff();
		while (timer.GetTimeDiff() < 0.5 && !lepra::SystemManager::GetQuitRequest()) {
			uilepra::Core::ProcessMessages();
			lepra::Thread::YieldCpu();
			screen.SetBuffer(0);
			display->UpdateScreen();
			timer.UpdateTimer();
		}
		display->CloseScreen();
	}

	delete (display);

	ReportTestResult(log, "Graphics", _context, test_ok);
	return (test_ok);
}

bool TestSound(const lepra::LogDecorator& log) {
	str _context;
	bool test_ok = true;

	uilepra::SoundManager::SoundID sound = uilepra::INVALID_SOUNDID;
	uilepra::SoundManager* sound_manager = uilepra::SoundManager::CreateSoundManager(uilepra::SoundManager::kContextOpenal);
	deb_assert(sound_manager);
	if (test_ok) {
		_context = "load sound";
		sound = sound_manager->LoadSound3D("data/logo_trumpet.wav", uilepra::SoundManager::kLoopForward, 0);
		test_ok = (sound != uilepra::INVALID_SOUNDID);
		deb_assert(test_ok);
	}
	uilepra::SoundManager::SoundInstanceID sound_instance = uilepra::INVALID_SOUNDINSTANCEID;
	if (test_ok) {
		_context = "create sound instance";
		sound_instance = sound_manager->CreateSoundInstance(sound);
		test_ok = (sound_instance != uilepra::INVALID_SOUNDID);
		deb_assert(test_ok);
	}
	if (test_ok) {
		_context = "play sound";
		sound_manager->Play(sound_instance, 1, 1);
		for (float x = 0; x < 6*5; x += 2.0f) {
			lepra::vec3 position(::sinf(x)*10, ::cosf(x)*10, 0);
			lepra::vec3 velocity;
			sound_manager->SetSoundPosition(sound_instance, position, velocity);
			lepra::Thread::Sleep(0.3);
		}
	}
	if (sound_instance != uilepra::INVALID_SOUNDINSTANCEID) {
		sound_manager->DeleteSoundInstance(sound_instance);
	}
	if (sound != uilepra::INVALID_SOUNDID) {
		sound_manager->Release(sound);
	}
	delete (sound_manager);

	ReportTestResult(log, "Sound", _context, test_ok);
	return (test_ok);
}

bool TestUiLepra() {
	bool test_ok = true;

	if (test_ok) {
		test_ok = TestLepra();
	}
	if (test_ok) {
		test_ok = TestInput(gUiLLog);
	}
	if (test_ok) {
		test_ok = TestGraphics(gUiLLog);
	}
	if (test_ok) {
		test_ok = TestSound(gUiLLog);
	}

	return (test_ok);
}

#endif //!CURE_TEST_WITHOUT_UI
