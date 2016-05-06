
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../include/x11/uix11core.h"
#include "../../../lepra/include/log.h"
#include "../../../lepra/include/systemmanager.h"
#include "../../../lepra/include/thread.h"
#include "../../include/x11/uix11displaymanager.h"



namespace uilepra {



int UiMain(lepra::Application& application) {
	application.Init();
	return application.Run();
}



void Core::Init() {
	X11Core::Init();
	SystemManager::ResetTerminal();
}

void Core::Shutdown() {
	X11Core::Shutdown();
}

void Core::ProcessMessages() {
	X11Core::ProcessMessages();
}



void X11Core::Init() {
	lock_ = new Lock();
}

void X11Core::Shutdown() {
	delete (lock_);
	lock_ = 0;
}

void X11Core::ProcessMessages() {
	ScopeLock lock(lock_);
	for (WindowTable::Iterator x = window_table_.First(); x != window_table_.End(); ++x) {
		X11DisplayManager* _display_manager = x.GetObject();
		_display_manager->ProcessMessages();
	}
}

void X11Core::AddDisplayManager(X11DisplayManager* display_manager) {
	ScopeLock lock(lock_);
	window_table_.Insert(display_manager->GetWindow(), display_manager);
}

void X11Core::RemoveDisplayManager(X11DisplayManager* display_manager) {
	ScopeLock lock(lock_);
	window_table_.Remove(display_manager->GetWindow());
}

X11DisplayManager* X11Core::GetDisplayManager(Window window_handle) {
	ScopeLock lock(lock_);
	return (window_table_.FindObject(window_handle));
}



Lock* X11Core::lock_ = 0;
X11Core::WindowTable X11Core::window_table_;



}
