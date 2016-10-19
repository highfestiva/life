
// Author: Jonas Bystrˆm
// Copyright (c) Pixel Doctrine



#pragma once

#include "../life/lifeapplication.h"
#include "../uicure/include/uigameuimanager.h"
#include "../uilepra/include/uitouchdrag.h"
#include "rtvar.h"
#include "trabantsim.h"
#include "version.h"



#ifdef LEPRA_IOS
#import <UIKit/UIKit.h>
#import <StoreKit/StoreKit.h>
#import <CoreMotion/CoreMotion.h>
//#import <iAd/ADInterstitialAd.h>
#include "listviewcontroller.h"



namespace lepra {
class Canvas;
}
using namespace lepra;



@interface AnimatedApp: UIResponder <SKProductsRequestDelegate, SKPaymentTransactionObserver, UIAlertViewDelegate>
{
@private
	Canvas* _canvas;
	NSTimer* _animationTimer;
	CMMotionManager* _motionManager;
	NSString* _alertHost;
}

@property (nonatomic, strong) UIWindow* window;
@property (nonatomic, strong) ListViewController* controller;
@property (nonatomic, strong) UIAlertController* alert;
@property (nonatomic, strong) UIAlertView* alertView;

-(id) init:(Canvas*)canvas;
-(void) dealloc;
-(void) startTick;
-(void) stopTick;
-(void) pushSimulatorController:(UIViewController*)viewController;
-(void) pushViewController:(UIViewController*)viewController animated:(BOOL)animated;
-(void) popViewControllerAnimated:(BOOL)animated;
-(void) if_game;
-(bool) showingSimulator_;
-(void) handleStdOut:(const str&)std_out;
-(void) showNetworkControlFor:(NSString*)hostname;
//-(void) ad_;
-(void) tick;

/*-(void) startPurchase:(NSString*)productName;
-(void) completeTransaction:(SKPaymentTransaction*)transaction;
-(void) restoreTransaction:(SKPaymentTransaction*)transaction;
-(void) failedTransaction: (SKPaymentTransaction*)transaction;
-(void) provideContent:(NSString*)productIdentifier confirm:(BOOL)confirm;*/
@end



#endif // iOS



namespace TrabantSim {



class TrabantSim: public life::Application {
	typedef life::Application Parent;
public:
	static TrabantSim* GetApp();
	TrabantSim(const strutil::strvec& argument_list);

	virtual ~TrabantSim();
	virtual void Init();
	virtual void Destroy();
	virtual bool MainLoop();
	virtual bool Tick();

	virtual void Resume(bool hard);
	virtual void Suspend(bool hard);
	void FoldSimulator();
	void UnfoldSimulator();
	void DidSyncFiles();
	bool ConnectQuery(const str& _hostname);

	void SavePurchase();
	str GetTypeName() const;
	str GetVersion() const;

	cure::ApplicationTicker* CreateTicker() const;

	static TrabantSim* app_;
#ifdef LEPRA_TOUCH
	AnimatedApp* animated_app_;
#endif // touch
	int active_counter_;
	bool is_in_tick_;

	UiCure::GameUiManager* ui_manager_;
	uilepra::touch::DragManager drag_manager_;

	logclass();
};



}
