
// Author: Jonas Bystrˆm
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Life/LifeApplication.h"
#include "../UiCure/Include/UiGameUiManager.h"
#include "../UiLepra/Include/UiTouchDrag.h"
#include "ListViewController.h"
#include "RtVar.h"
#include "TrabantSim.h"
#include "Version.h"



#ifdef LEPRA_IOS
#import <UIKit/UIKit.h>
#import <StoreKit/StoreKit.h>
#import <CoreMotion/CoreMotion.h>
#import <iAd/ADInterstitialAd.h>



namespace Lepra
{
class Canvas;
}
using namespace Lepra;



@interface AnimatedApp: UIResponder <ADInterstitialAdDelegate, SKProductsRequestDelegate, SKPaymentTransactionObserver, UIAlertViewDelegate>
{
@private
	Canvas* _canvas;
	NSTimer* _animationTimer;
	CMMotionManager* _motionManager;
	ADInterstitialAd* _ad;
}

@property (nonatomic, strong) UIWindow* window;
@property (nonatomic, strong) ListViewController* listController;

-(id) init:(Canvas*)pCanvas;
-(void) dealloc;
-(void) startTick;
-(void) stopTick;
-(void) handleStdOut:(const astr&)pStdOut;
-(void) showAd;
-(void) tick;

-(void) startPurchase:(NSString*)productName;
-(void) completeTransaction:(SKPaymentTransaction*)transaction;
-(void) restoreTransaction:(SKPaymentTransaction*)transaction;
-(void) failedTransaction: (SKPaymentTransaction*)transaction;
-(void) provideContent:(NSString*)productIdentifier confirm:(BOOL)confirm;
@end



#endif // iOS



namespace TrabantSim
{



class TrabantSim: public Life::Application
{
	typedef Life::Application Parent;
public:
	static TrabantSim* GetApp();
	TrabantSim(const strutil::strvec& pArgumentList);

	virtual ~TrabantSim();
	virtual void Init();
	virtual void Destroy();
	virtual bool MainLoop();
	virtual bool Tick();

	virtual void Resume(bool pHard);
	virtual void Suspend(bool pHard);
	void FoldSimulator();
	void UnfoldSimulator();
	void DidSyncFiles();

	void SavePurchase();
	str GetTypeName() const;
	str GetVersion() const;

	Cure::ApplicationTicker* CreateTicker() const;

	static TrabantSim* mApp;
#ifdef LEPRA_TOUCH
	AnimatedApp* mAnimatedApp;
#endif // Touch
	int mActiveCounter;
	bool mIsInTick;

	UiCure::GameUiManager* mUiManager;
	UiLepra::Touch::DragManager mDragManager;
		
	logclass();
};



}
