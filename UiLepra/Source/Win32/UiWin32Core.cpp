
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#include "../../../Lepra/Include/Log.h"
#include "../../../Lepra/Include/SystemManager.h"
#include "../../../Lepra/Include/Thread.h"
#include "../../Include/Win32/UiWin32Core.h"
#include "../../Include/Win32/UiWin32DisplayManager.h"



namespace UiLepra
{



void Core::Init()
{
	Win32Core::Init();
}

void Core::Shutdown()
{
	Win32Core::Shutdown();
}

void Core::ProcessMessages()
{
	Win32Core::ProcessMessages();
}



void Win32Core::Init()
{
	mLock = new Lepra::Lock();
}

void Win32Core::Shutdown()
{
	delete (mLock);
	mLock = 0;
}

HINSTANCE Win32Core::GetAppInstance()
{
	return (HINSTANCE)::GetModuleHandle(0);
}

void Win32Core::ProcessMessages()
{
	Lepra::ScopeLock lLock(mLock);
	for (WindowTable::Iterator x = mWindowTable.First(); x != mWindowTable.End(); ++x)
	{
		Win32DisplayManager* lDisplayManager = x.GetObject();
		lDisplayManager->ProcessMessages();
	}
}

void Win32Core::AddDisplayManager(Win32DisplayManager* pDisplayManager)
{
	Lepra::ScopeLock lLock(mLock);
	mWindowTable.Insert(pDisplayManager->GetHWND(), pDisplayManager);
}

void Win32Core::RemoveDisplayManager(Win32DisplayManager* pDisplayManager)
{
	Lepra::ScopeLock lLock(mLock);
	mWindowTable.Remove(pDisplayManager->GetHWND());
}

Win32DisplayManager* Win32Core::GetDisplayManager(HWND pWindowHandle)
{
	Lepra::ScopeLock lLock(mLock);
	return (mWindowTable.FindObject(pWindowHandle));
}



Lepra::Lock* Win32Core::mLock = 0;
Win32Core::WindowTable Win32Core::mWindowTable;



}
