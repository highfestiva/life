
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../../../Lepra/Include/HashTable.h"
#include "../../../Lepra/Include/Thread.h"
#include "../UiCore.h"
#include "../UiLepra.h"

// Must be after OS includes to get #defines below.
#ifdef LEPRA_IOS
#import <UIKit/UIKit.h>
#define LEPRA_APPLE_APP          UIApplication
#define LEPRA_APPLE_APP_DELEGATE UIApplicationDelegate
#define LEPRA_APPLE_WINDOW       UIWindow
#define LEPRA_APPLE_VIEW         UIView
#define LEPRA_APPLE_GL_VIEW      EAGLView
#define LEPRA_APPLE_GL_CONTEXT   EAGLContext
#define LEPRA_APPLE_EVENT        UIEvent
#else // !iOS
#import <AppKit/AppKit.h>
#define LEPRA_APPLE_APP          NSApplication
#define LEPRA_APPLE_APP_DELEGATE NSApplicationDelegate
#define LEPRA_APPLE_WINDOW       NSWindow
#define LEPRA_APPLE_VIEW         NSView
#define LEPRA_APPLE_GL_VIEW      NSOpenGLView
#define LEPRA_APPLE_GL_CONTEXT   NSOpenGLContext
#define LEPRA_APPLE_EVENT        NSEvent
#endif // iOS/!iOS



namespace UiLepra
{



class MacDisplayManager;



class MacCore
{
public:
	static void Init();
	static void Shutdown();

	// Takes care of all incoming window messages.
	// Must be called at least once "per frame".
	static void ProcessMessages();

	static void AddDisplayManager(MacDisplayManager* pDisplayManager);
	static void RemoveDisplayManager(MacDisplayManager* pDisplayManager);
	static MacDisplayManager* GetDisplayManager(LEPRA_APPLE_WINDOW* pWindowHandle);

	static LEPRA_APPLE_APP* mApplication;

private:
	static Lock* mLock;
	typedef HashTable<LEPRA_APPLE_WINDOW*, MacDisplayManager*, LEPRA_VOIDP_HASHER> WindowTable;
	static WindowTable mWindowTable;
};



}
