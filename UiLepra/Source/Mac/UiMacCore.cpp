
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../../Include/Mac/UiMacCore.h"
#include "../../../Lepra/Include/Log.h"
#include "../../../Lepra/Include/SystemManager.h"
#include "../../../Lepra/Include/Thread.h"
#include "../../Include/Mac/UiMacDisplayManager.h"



namespace UiLepra
{



void Core::Init()
{
	MacCore::Init();
}

void Core::Shutdown()
{
	MacCore::Shutdown();
}

void Core::ProcessMessages()
{
	MacCore::ProcessMessages();
}



void MacCore::Init()
{
	mLock = new Lock();
}

void MacCore::Shutdown()
{
	delete (mLock);
	mLock = 0;
}

void MacCore::ProcessMessages()
{
	ScopeLock lLock(mLock);
	for (WindowTable::Iterator x = mWindowTable.First(); x != mWindowTable.End(); ++x)
	{
		MacDisplayManager* lDisplayManager = x.GetObject();
		lDisplayManager->ProcessMessages();
	}
}

void MacCore::AddDisplayManager(MacDisplayManager* pDisplayManager)
{
	ScopeLock lLock(mLock);
	mWindowTable.Insert(pDisplayManager->GetWindow(), pDisplayManager);
}

void MacCore::RemoveDisplayManager(MacDisplayManager* pDisplayManager)
{
	ScopeLock lLock(mLock);
	mWindowTable.Remove(pDisplayManager->GetWindow());
}

MacDisplayManager* MacCore::GetDisplayManager(Window pWindowHandle)
{
	ScopeLock lLock(mLock);
	return (mWindowTable.FindObject(pWindowHandle));
}



Lock* MacCore::mLock = 0;
MacCore::WindowTable MacCore::mWindowTable;



}
