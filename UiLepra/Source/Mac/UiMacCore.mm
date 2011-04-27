
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../../Include/Mac/UiMacCore.h"
#include "../../../Lepra/Include/Log.h"
#include "../../../Lepra/Include/SystemManager.h"
#include "../../../Lepra/Include/Thread.h"
#include "../../Include/Mac/UiMacDisplayManager.h"



static Lepra::Application* gApplication = 0;



@interface NSApplication(UiLepraCoreMenuExtension)
- (void)setAppleMenu:(NSMenu*)menu;
@end



@interface NSApplication(UiLepraTerminateExtension)
@end

@implementation NSApplication(UiLepraTerminateExtension)
- (void)terminate:(id)sender
{
	Lepra::SystemManager::AddQuitRequest(+1);
}
@end



@interface UiLepraLoadedDispatcher: NSObject
@end

@implementation UiLepraLoadedDispatcher
- (void) applicationDidFinishLaunching: (NSNotification *) note
{
	// Hand off to main application code.
	gApplication->Init();
	const int lExitStatus = gApplication->Run();
	delete gApplication;
	exit(lExitStatus);
}
@end



namespace UiLepra
{



static NSString* getApplicationName(void)
{
	const NSDictionary* dict;
	NSString* appName = 0;

	// Determine the application name.
	dict = (const NSDictionary *)CFBundleGetInfoDictionary(CFBundleGetMainBundle());
	if (dict)
		appName = [dict objectForKey: @"CFBundleName"];

	if (![appName length])
		appName = [[NSProcessInfo processInfo] processName];

	return appName;
}

static void setApplicationMenu(void)
{
	// Warning: this code is very odd.
	NSMenu *appleMenu;
	NSMenuItem *menuItem;
	NSString *title;
	NSString *appName;

	appName = getApplicationName();
	appleMenu = [[NSMenu alloc] initWithTitle:@""];
    
	// Add menu items.
	title = [@"About " stringByAppendingString:appName];
	[appleMenu addItemWithTitle:title action:@selector(orderFrontStandardAboutPanel:) keyEquivalent:@""];

	[appleMenu addItem:[NSMenuItem separatorItem]];

	title = [@"Hide " stringByAppendingString:appName];
	[appleMenu addItemWithTitle:title action:@selector(hide:) keyEquivalent:@"h"];

	menuItem = (NSMenuItem *)[appleMenu addItemWithTitle:@"Hide Others" action:@selector(hideOtherApplications:) keyEquivalent:@"h"];
	[menuItem setKeyEquivalentModifierMask:(NSAlternateKeyMask|NSCommandKeyMask)];

	[appleMenu addItemWithTitle:@"Show All" action:@selector(unhideAllApplications:) keyEquivalent:@""];

	[appleMenu addItem:[NSMenuItem separatorItem]];

	title = [@"Quit " stringByAppendingString:appName];
	[appleMenu addItemWithTitle:title action:@selector(terminate:) keyEquivalent:@"q"];

	// Put menu into the menubar.
	menuItem = [[NSMenuItem alloc] initWithTitle:@"" action:nil keyEquivalent:@""];
	[menuItem setSubmenu:appleMenu];
	[[NSApp mainMenu] addItem:menuItem];

	// Tell the application object that this is now the application menu.
	[NSApp setAppleMenu:appleMenu];

	[appleMenu release];
	[menuItem release];
}

static void setupWindowMenu(void)
{
	NSMenu      *windowMenu;
	NSMenuItem  *windowMenuItem;
	NSMenuItem  *menuItem;

	windowMenu = [[NSMenu alloc] initWithTitle:@"Window"];

	// "Minimize" item.
	menuItem = [[NSMenuItem alloc] initWithTitle:@"Minimize" action:@selector(performMiniaturize:) keyEquivalent:@"m"];
	[windowMenu addItem:menuItem];
	[menuItem release];

	// Put menu into the menubar.
	windowMenuItem = [[NSMenuItem alloc] initWithTitle:@"Window" action:nil keyEquivalent:@""];
	[windowMenuItem setSubmenu:windowMenu];
	[[NSApp mainMenu] addItem:windowMenuItem];

	// Tell the application object that this is now the window menu.
	[NSApp setWindowsMenu:windowMenu];

	[windowMenu release];
	[windowMenuItem release];
}

int UiMain(Application& pApplication)
{
	NSAutoreleasePool* lPool = [[NSAutoreleasePool alloc] init];

	// Ensure the application object is initialised.
	[NSApplication sharedApplication];
    
#ifdef UILEPRA_USE_CPS
	{
		CPSProcessSerNum PSN;
		// Tell the Dock about us.
		if (!CPSGetCurrentProcess(&PSN))
			if (!CPSEnableForegroundOperation(&PSN,0x03,0x3C,0x2C,0x1103))
				if (!CPSSetFrontProcess(&PSN))
					[NSApplication sharedApplication];
	}
#endif // UILEPRA_USE_CPS

	MacCore::mApplication = NSApp;

	// Setup menu.
	[NSApp setMainMenu:[[NSMenu alloc] init]];
	setApplicationMenu();
	setupWindowMenu();

	UiLepraLoadedDispatcher* lLoadedDispatcher = [[UiLepraLoadedDispatcher alloc] init];
	[NSApp setDelegate:lLoadedDispatcher];

	gApplication = &pApplication;

	// Start the main event loop which will get us a loaded dispatch call.
	[NSApp run];

	[lLoadedDispatcher release];
	[lPool release];
	return 0;
}



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

	//mApplication = [NSApplication sharedApplication];

	//[NSThread detachNewThreadSelector: @selector(startDummyThread:) toTarget: mApplication withObject: nil];

	//[[NSNotificationCenter defaultCenter] postNotificationName:NSApplicationWillFinishLaunchingNotification object: NSApp];
	//[[NSNotificationCenter defaultCenter] postNotificationName:NSApplicationDidFinishLaunchingNotification object: NSApp];
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
NSApplication* MacCore::mApplication;



}
