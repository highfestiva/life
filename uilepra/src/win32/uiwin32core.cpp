
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../include/win32/uiwin32core.h"
#include "../../../lepra/include/hirestimer.h"
#include "../../../lepra/include/log.h"
#include "../../../lepra/include/systemmanager.h"
#include "../../../lepra/include/thread.h"
#include "../../include/win32/uiwin32displaymanager.h"



extern HINSTANCE ghInstance;



namespace uilepra {



int UiMain(lepra::Application& application) {
	application.Init();
	return application.Run();
}



void Core::Init() {
	Win32Core::Init();
}

void Core::Shutdown() {
	Win32Core::Shutdown();
}

void Core::ProcessMessages() {
	Win32Core::ProcessMessages();
}



void Win32Core::Init() {
	if (!lock_) {
		lock_ = new Lock();
	}
}

void Win32Core::Shutdown() {
	delete (lock_);
	lock_ = 0;
}

HINSTANCE Win32Core::GetAppInstance() {
	return ghInstance;
}

void Win32Core::ProcessMessages() {
	ScopeLock lock(lock_);
	for (WindowTable::Iterator x = window_table_.First(); x != window_table_.End(); ++x) {
		Win32DisplayManager* _display_manager = x.GetObject();
		_display_manager->ProcessMessages();
	}
}

void Win32Core::AddDisplayManager(Win32DisplayManager* display_manager) {
	ScopeLock lock(lock_);
	window_table_.Insert(display_manager->GetHWND(), display_manager);
}

void Win32Core::RemoveDisplayManager(Win32DisplayManager* display_manager) {
	ScopeLock lock(lock_);
	window_table_.Remove(display_manager->GetHWND());
}

Win32DisplayManager* Win32Core::GetDisplayManager(HWND window_handle) {
	ScopeLock lock(lock_);
	return (window_table_.FindObject(window_handle));
}



Lock* Win32Core::lock_ = 0;
Win32Core::WindowTable Win32Core::window_table_;



}
