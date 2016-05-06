
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../../include/mac/uimaccore.h"
#include "../../../lepra/include/log.h"
#include "../../../lepra/include/systemmanager.h"
#include "../../../lepra/include/thread.h"
#include "../../include/mac/uimacdisplaymanager.h"



static lepra::Application* g_application = 0;



#ifndef LEPRA_IOS
@interface LEPRA_APPLE_APP(UiLepraCoreMenuExtension)
- (void)setAppleMenu:(NSMenu*)menu;
@end
#endif // !iOS



@interface LEPRA_APPLE_APP(UiLepraTerminateExtension)
@end

@implementation LEPRA_APPLE_APP(UiLepraTerminateExtension)
- (void)terminate:(id)sender {
	lepra::SystemManager::AddQuitRequest(+1);
}
@end



@interface UiLepraLoadedDispatcher: NSObject <LEPRA_APPLE_APP_DELEGATE>
@end

@implementation UiLepraLoadedDispatcher
#ifdef LEPRA_IOS
-(void) applicationDidFinishLaunching:(UIApplication*)application
#else // !iOS
-(void) applicationDidFinishLaunching:(NSNotification*)note
#endif // iOS/!iOS
{
	// Hand off to main application code.
	g_application->Init();
#ifndef LEPRA_IOS
	const int exit_status = g_application->Run();
	delete g_application;
	exit(exit_status);
#else // iOS
	g_application->Run();
#endif // !iOS/iOS
}

#ifdef LEPRA_IOS
- (void)applicationWillResignActive:(UIApplication *)application {
	g_application->Suspend(true);
}
- (void)applicationDidBecomeActive:(UIApplication *)application {
	g_application->Resume(true);
}
#endif // iOS
@end



namespace uilepra {



#ifndef LEPRA_IOS
static NSString* getApplicationName(void) {
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

static void setApplicationMenu(void) {
	// Warning: this code is very odd.
	NSMenu *appleMenu;
	NSMenuItem *item_;
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

	item_ = (NSMenuItem *)[appleMenu addItemWithTitle:@"Hide Others" action:@selector(hideOtherApplications:) keyEquivalent:@"h"];
	[item_ setKeyEquivalentModifierMask:(NSAlternateKeyMask|NSCommandKeyMask)];

	[appleMenu addItemWithTitle:@"Show All" action:@selector(unhideAllApplications:) keyEquivalent:@""];

	[appleMenu addItem:[NSMenuItem separatorItem]];

	title = [@"Quit " stringByAppendingString:appName];
	[appleMenu addItemWithTitle:title action:@selector(terminate:) keyEquivalent:@"q"];

	// Put menu into the menubar.
	item_ = [[NSMenuItem alloc] initWithTitle:@"" action:nil keyEquivalent:@""];
	[item_ setSubmenu:appleMenu];
	[[[NSApplication sharedApplication] mainMenu] addItem:item_];

	// Tell the application object that this is now the application menu.
	[[NSApplication sharedApplication] setAppleMenu:appleMenu];

	[appleMenu release];
	[item_ release];
}

static void setupWindowMenu(void) {
	NSMenu      *windowMenu;
	NSMenuItem  *windowMenuItem;
	NSMenuItem  *item_;

	windowMenu = [[NSMenu alloc] initWithTitle:@"Window"];

	// "Minimize" item.
	item_ = [[NSMenuItem alloc] initWithTitle:@"Minimize" action:@selector(performMiniaturize:) keyEquivalent:@"m"];
	[windowMenu addItem:item_];
	[item_ release];

	// Put menu into the menubar.
	windowMenuItem = [[NSMenuItem alloc] initWithTitle:@"Window" action:nil keyEquivalent:@""];
	[windowMenuItem setSubmenu:windowMenu];
	[[[NSApplication sharedApplication] mainMenu] addItem:windowMenuItem];

	// Tell the application object that this is now the window menu.
	[[NSApplication sharedApplication] setWindowsMenu:windowMenu];

	[windowMenu release];
	[windowMenuItem release];
}
#endif // !iOS



int UiMain(Application& _application) {
#ifndef LEPRA_IOS
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
#endif // Mac

	g_application = &_application;

#ifndef LEPRA_IOS
	// Ensure the application object is initialised.
	[NSApplication sharedApplication];

#if defined(UILEPRA_USE_CPS)

	{
		CPSProcessSerNum PSN;
		// Tell the Dock about us.
		if (!CPSGetCurrentProcess(&PSN))
			if (!CPSEnableForegroundOperation(&PSN,0x03,0x3C,0x2C,0x1103))
				if (!CPSSetFrontProcess(&PSN))
					[NSApplication sharedApplication];
	}
#endif // CPS

	MacCore::application_ = [NSApplication sharedApplication];

	// Setup menu.
	[[NSApplication sharedApplication] setMainMenu:[[NSMenu alloc] init]];
	setApplicationMenu();
	setupWindowMenu();

	UiLepraLoadedDispatcher* loaded_dispatcher = [[UiLepraLoadedDispatcher alloc] init];
	[[NSApplication sharedApplication] setDelegate:loaded_dispatcher];
	[[NSApplication sharedApplication] run];
	[loaded_dispatcher release];

#else // iOS
	MacCore::application_ = [UIApplication sharedApplication];
	UIApplicationMain(0, 0, nil, @"UiLepraLoadedDispatcher");
#endif // !iOS/iOS

#ifndef LEPRA_IOS
	[pool release];
#endif // Mac
	return 0;
}



void Core::Init() {
	MacCore::Init();
}

void Core::Shutdown() {
	MacCore::Shutdown();
}

void Core::ProcessMessages() {
	MacCore::ProcessMessages();
}



void MacCore::Init() {
	if (!lock_) {
		lock_ = new Lock();
	}
}

void MacCore::Shutdown() {
	delete (lock_);
	lock_ = 0;
}

void MacCore::ProcessMessages() {
	ScopeLock lock(lock_);
	if (window_table_.IsEmpty()) {
		return;
	}

#ifndef LEPRA_IOS
	LEPRA_APPLE_EVENT* event;
	do {
		event = [application_ nextEventMatchingMask:NSAnyEventMask
						   untilDate:nil
						      inMode:NSDefaultRunLoopMode
						     dequeue:YES];
		if (event) {
			[application_ sendEvent:event];
		}
	} while (event);
	//[application_ updateWindows];
#endif // !iOS

	for (WindowTable::Iterator x = window_table_.First(); x != window_table_.End(); ++x) {
		MacDisplayManager* _display_manager = x.GetObject();
		_display_manager->ProcessMessages();
	}
}

void MacCore::AddDisplayManager(MacDisplayManager* display_manager) {
	ScopeLock lock(lock_);
	window_table_.Insert((__bridge void*)display_manager->GetWindow(), display_manager);
}

void MacCore::RemoveDisplayManager(MacDisplayManager* display_manager) {
	ScopeLock lock(lock_);
	window_table_.Remove((__bridge void*)display_manager->GetWindow());
}

MacDisplayManager* MacCore::GetDisplayManager(LEPRA_APPLE_WINDOW* window_handle) {
	ScopeLock lock(lock_);
	return (window_table_.FindObject((__bridge void*)window_handle));
}



LEPRA_APPLE_APP* MacCore::application_;
Lock* MacCore::lock_ = 0;
MacCore::WindowTable MacCore::window_table_;



}
