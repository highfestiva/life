
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../../Include/Win32/UiWin32Core.h"
#include "../../../Lepra/Include/HiResTimer.h"
#include "../../../Lepra/Include/Log.h"
#include "../../../Lepra/Include/SystemManager.h"
#include "../../../Lepra/Include/Thread.h"
#include "../../Include/Win32/UiWin32DisplayManager.h"



extern HINSTANCE ghInstance;



namespace UiLepra
{



int UiMain(Lepra::Application& pApplication)
{
	pApplication.Init();
	return pApplication.Run();
}



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
	HiResTimer::StepCounterShadow();
	Win32Core::ProcessMessages();
}



void Win32Core::Init()
{
	mLock = new Lock();
}

void Win32Core::Shutdown()
{
	delete (mLock);
	mLock = 0;
}

HINSTANCE Win32Core::GetAppInstance()
{
	return ghInstance;
}

void Win32Core::ProcessMessages()
{
	ScopeLock lLock(mLock);
	for (WindowTable::Iterator x = mWindowTable.First(); x != mWindowTable.End(); ++x)
	{
		Win32DisplayManager* lDisplayManager = x.GetObject();
		lDisplayManager->ProcessMessages();
	}
}

void Win32Core::AddDisplayManager(Win32DisplayManager* pDisplayManager)
{
	ScopeLock lLock(mLock);
	mWindowTable.Insert(pDisplayManager->GetHWND(), pDisplayManager);
}

void Win32Core::RemoveDisplayManager(Win32DisplayManager* pDisplayManager)
{
	ScopeLock lLock(mLock);
	mWindowTable.Remove(pDisplayManager->GetHWND());
}

Win32DisplayManager* Win32Core::GetDisplayManager(HWND pWindowHandle)
{
	ScopeLock lLock(mLock);
	return (mWindowTable.FindObject(pWindowHandle));
}



Lock* Win32Core::mLock = 0;
Win32Core::WindowTable Win32Core::mWindowTable;



}
