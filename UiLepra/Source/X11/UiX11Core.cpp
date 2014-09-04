
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../Include/X11/UiX11Core.h"
#include "../../../Lepra/Include/Log.h"
#include "../../../Lepra/Include/SystemManager.h"
#include "../../../Lepra/Include/Thread.h"
#include "../../Include/X11/UiX11DisplayManager.h"



namespace UiLepra
{



int UiMain(Lepra::Application& pApplication)
{
	pApplication.Init();
	return pApplication.Run();
}



void Core::Init()
{
	X11Core::Init();
}

void Core::Shutdown()
{
	X11Core::Shutdown();
}

void Core::ProcessMessages()
{
	X11Core::ProcessMessages();
}



void X11Core::Init()
{
	mLock = new Lock();
}

void X11Core::Shutdown()
{
	delete (mLock);
	mLock = 0;
}

void X11Core::ProcessMessages()
{
	ScopeLock lLock(mLock);
	for (WindowTable::Iterator x = mWindowTable.First(); x != mWindowTable.End(); ++x)
	{
		X11DisplayManager* lDisplayManager = x.GetObject();
		lDisplayManager->ProcessMessages();
	}
}

void X11Core::AddDisplayManager(X11DisplayManager* pDisplayManager)
{
	ScopeLock lLock(mLock);
	mWindowTable.Insert(pDisplayManager->GetWindow(), pDisplayManager);
}

void X11Core::RemoveDisplayManager(X11DisplayManager* pDisplayManager)
{
	ScopeLock lLock(mLock);
	mWindowTable.Remove(pDisplayManager->GetWindow());
}

X11DisplayManager* X11Core::GetDisplayManager(Window pWindowHandle)
{
	ScopeLock lLock(mLock);
	return (mWindowTable.FindObject(pWindowHandle));
}



Lock* X11Core::mLock = 0;
X11Core::WindowTable X11Core::mWindowTable;



}
