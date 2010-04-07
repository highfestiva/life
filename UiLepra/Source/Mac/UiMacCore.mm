
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../../Include/Mac/UiMacCore.h"
#include "../../../Lepra/Include/Log.h"
#include "../../../Lepra/Include/SystemManager.h"
#include "../../../Lepra/Include/Thread.h"
#include "../../Include/Mac/UiMacDisplayManager.h"



NSAutoreleasePool* gPool = [[NSAutoreleasePool alloc] init];



/*int MyApplicationMain(int argc, const char **argv)
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
	NSDictionary *infoDictionary = [[NSBundle mainBundle] infoDictionary];
	Class principalClass =
		NSClassFromString([infoDictionary objectForKey:@"NSPrincipalClass"]);
	NSApplication *applicationObject = [principalClass sharedApplication];

	NSString *mainNibName = [infoDictionary objectForKey:@"NSMainNibFile"];
	NSNib *mainNib = [[NSNib alloc] initWithNibNamed:mainNibName bundle:[NSBundle mainBundle]];
	[mainNib instantiateNibWithOwner:applicationObject topLevelObjects:nil];

	if ([applicationObject respondsToSelector:@selector(run)])
	{
		[applicationObject
			performSelectorOnMainThread:@selector(run)
			withObject:nil
			waitUntilDone:YES];
	}
	
	[mainNib release];
	[pool release];
	
	return 0;
}*/

@implementation UiMacApplication

- (void)run
{
}

- (void)terminate:(id)sender
{
	Lepra::SystemManager::AddQuitRequest(1);
}

- (void)startDummyThread:(id)sender
{
	printf("startDummyThread!\n");
}


@end



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

	mApplication = (UiMacApplication*)[UiMacApplication sharedApplication];

	[NSThread detachNewThreadSelector: @selector(startDummyThread:) toTarget: mApplication withObject: nil];

	[[NSNotificationCenter defaultCenter] postNotificationName:NSApplicationWillFinishLaunchingNotification object: NSApp];
	[[NSNotificationCenter defaultCenter] postNotificationName:NSApplicationDidFinishLaunchingNotification object: NSApp];
}

void MacCore::Shutdown()
{
	delete (mLock);
	mLock = 0;
}

void MacCore::ProcessMessages()
{
	ScopeLock lLock(mLock);
	if (mWindowTable.IsEmpty())
	{
		return;
	}

	NSEvent* event = [mApplication	nextEventMatchingMask:	NSAnyEventMask
					untilDate:		nil
					inMode:			NSDefaultRunLoopMode
					dequeue:		YES];

	[mApplication sendEvent: event];
	[mApplication updateWindows];

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

MacDisplayManager* MacCore::GetDisplayManager(NSWindow* pWindowHandle)
{
	ScopeLock lLock(mLock);
	return (mWindowTable.FindObject(pWindowHandle));
}



Lock* MacCore::mLock = 0;
MacCore::WindowTable MacCore::mWindowTable;
UiMacApplication* MacCore::mApplication;



}
