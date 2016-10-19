
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../lepra/include/lepraos.h"
#include "../lepra/include/lepratypes.h"
#include "../uicure/include/uicure.h"
#include "../uilepra/include/mac/uimactouchhandler.h"
#include "animatedapp.h"
#include "trabantsim.h"
#include "trabantsim.cxx"



#ifdef LEPRA_IOS

#import "../uilepra/include/mac/eaglview.h"
#include "editviewcontroller.h"
#include "listviewcontroller.h"
#include "stdoutviewcontroller.h"

@implementation AnimatedApp

-(id) init:(Canvas*)__canvas {
	// Init stuff for future simulation.
	//[UIApplication sharedApplication].statusBarOrientation_ = UIInterfaceOrientationLandscapeRight;
	_canvas = __canvas;
	_canvas->SetDeviceRotation(0);
	/*if ([[[UIDevice currentDevice] systemVersion] floatValue] < 8.0) {
		_canvas->SetOutputRotation(90);
	}*/
	_animationTimer = nil;
	_motionManager = [[CMMotionManager alloc] init];

	// Prepare background ad and payment stuff.
	//[self createAd];
	[[SKPaymentQueue defaultQueue] addTransactionObserver:self];

	// Initialize the IDE.
	UIWindow* window = [[UIWindow alloc] initWithFrame:[UIScreen mainScreen].bounds];
	self.window = window;	// Retain it!
	self.controller = [ListViewController new];
	self.controller.title = @"Prototypes";
	UINavigationController* navigationController = [[RotatingController alloc] initWithRootViewController:self.controller];
	if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad) {
		EditViewController* editController = [EditViewController new];
		editController.title = @"";
		self.controller.editController = editController;
		editController.controller = self.controller;
		UINavigationController* editHeadController = [[UINavigationController alloc] initWithRootViewController:editController];
		UISplitViewController* controller_ = [[UISplitViewController alloc] init];
		//controller_.delegate = self;
		controller_.viewControllers = @[navigationController, editHeadController];
		controller_.preferredPrimaryColumnWidthFraction = 0.3;
		window.rootViewController = controller_;
	} else {
		window.rootViewController = navigationController;
	}
	[window makeKeyAndVisible];

	return self;
}

-(void) dealloc {
        //[super dealloc];
}

-(void) startTick
{
	self.alert = nil;
	if (!_animationTimer) {
		_animationTimer = [NSTimer scheduledTimerWithTimeInterval:0.0001 target:self selector:@selector(tick) userInfo:nil repeats:YES];
		[EAGLView sharedView_].responder = self;
		[[EAGLView sharedView_] up_acc];
		[_motionManager startAccelerometerUpdates];
	}
}

-(void) stopTick
{
	[_motionManager stopAccelerometerUpdates];
	[[EAGLView sharedView_] down_acc];
	[_animationTimer invalidate];
	_animationTimer = nil;
}

-(void) pushSimulatorController:(UIViewController*)viewController {
	[[UIApplication sharedApplication] setStatusBarHidden:YES withAnimation:UIStatusBarAnimationNone];
	[self pushViewController:viewController animated:YES];
}

-(void) pushViewController:(UIViewController*)viewController animated:(BOOL)animated {
	if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad) {
		const bool isRight = self.window.rootViewController.interfaceOrientation == UIInterfaceOrientationLandscapeRight;
		[EAGLView sharedView_].preferredRotation = isRight? UIInterfaceOrientationLandscapeRight : UIInterfaceOrientationLandscapeLeft;
		[(UISplitViewController*)self.window.rootViewController presentViewController:viewController animated:animated completion:nil];
	} else {
		if ([UIApplication sharedApplication].isStatusBarHidden) {
			((UINavigationController*)self.window.rootViewController).navigationBarHidden = YES;
			const bool isRight = self.window.rootViewController.interfaceOrientation == UIInterfaceOrientationLandscapeRight;
			[EAGLView sharedView_].preferredRotation = isRight? UIInterfaceOrientationLandscapeRight : UIInterfaceOrientationLandscapeLeft;
		}
		[(UINavigationController*)self.window.rootViewController pushViewController:viewController animated:animated];
	}
	//[UIViewController attemptRotationToDeviceOrientation];
}

-(void) popViewControllerAnimated:(BOOL)animated {
	if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad) {
		self.window.rootViewController.presentedViewController.view = [UIView new];
		[(UISplitViewController*)self.window.rootViewController dismissViewControllerAnimated:animated completion:nil];
	} else {
		if (((UINavigationController*)self.window.rootViewController).visibleViewController.view == [EAGLView sharedView_]) {
			((UINavigationController*)self.window.rootViewController).visibleViewController.view = [UIView new];
		}
		[(UINavigationController*)self.window.rootViewController popViewControllerAnimated:animated];
		((UINavigationController*)self.window.rootViewController).navigationBarHidden = NO;
	}
	//[UIViewController attemptRotationToDeviceOrientation];
}

-(void) if_game {
	if ([self showingSimulator_]) {
		[self popViewControllerAnimated:YES];
	}
}

-(bool) showingSimulator_
{
	if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad) {
		return (((UISplitViewController*)self.window.rootViewController).presentedViewController.view == [EAGLView sharedView_]);
	} else {
		return (((UINavigationController*)self.window.rootViewController).visibleViewController.view == [EAGLView sharedView_]);
	}
}

-(void) handleStdOut:(const str&)std_out {
	NSString* text = [NSString stringWithCString:std_out.c_str() encoding:NSUTF8StringEncoding];
	[self popViewControllerAnimated:NO];
	StdOutViewController* out_controller_ = [StdOutViewController new];
	out_controller_.title = @"Output";
	out_controller_.text = text;
	if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad) {
		out_controller_.modalPresentationStyle = UIModalPresentationPopover;
		UIPopoverController* popover = [[UIPopoverController alloc] initWithContentViewController:out_controller_];
		CGSize fit = [PythonTextView slowFitTextSize:text];
		popover.popoverContentSize = fit;
		[popover presentPopoverFromBarButtonItem:self.controller.editController.navigationItem.rightBarButtonItem permittedArrowDirections:UIPopoverArrowDirectionAny animated:YES];
	} else {
		[self pushViewController:out_controller_ animated:NO];
	}
}

-(void) showNetworkControlFor:(NSString*)hostname {
	if (self.alert || self.alertView) {
		return;
	}

	NSString* message = [NSString stringWithFormat:@"Grant '%@' access to simulation and prototypes?", hostname];
	self.alert = [UIAlertController alertControllerWithTitle:@"Network control" message:message preferredStyle:UIAlertControllerStyleAlert];
	if (!self.alert) {
		_alertHost = hostname;
		self.alertView = [[UIAlertView alloc] initWithTitle:@"Network control" message:message delegate:self cancelButtonTitle:@"No" otherButtonTitles:@"Always", @"Never", nil];
		self.alertView.delegate = self;
		[self.alertView show];
		return;
	}
	UIAlertAction* grantAction = [UIAlertAction actionWithTitle:@"Permanently"
							      style:UIAlertActionStyleDefault
							    handler:^(UIAlertAction*){
								    self.alert = nil;
								    [self setAccessFor:hostname allow:YES];
							    }];
	UIAlertAction* denyAction = [UIAlertAction actionWithTitle:@"No"
							     style:UIAlertActionStyleCancel
							   handler:^(UIAlertAction*){
								   self.alert = nil;
							   }];

	UIAlertAction* banAction = [UIAlertAction actionWithTitle:@"Never"
							    style:UIAlertActionStyleDestructive
							  handler:^(UIAlertAction*){
								  self.alert = nil;
								  [self setAccessFor:hostname allow:NO];
							  }];

	[self.alert addAction:grantAction];
	[self.alert addAction:denyAction];
	[self.alert addAction:banAction];
	[self.window.rootViewController presentViewController:self.alert animated:YES completion:nil];
}

-(void) ad_
{
	/*if (_ad.loaded) {
		[_ad presentFromViewController:[EAGLView sharedView_].window.rootViewController];
	}*/
}

-(void) tick {
	EAGLView* gl_view = [EAGLView sharedView_];
	if (!gl_view.isOpen) {
		[gl_view framebuffer_];
	} else {
		gl_view.canvas = _canvas;
		float x = _motionManager.accelerometerData.acceleration.x;
		float y = _motionManager.accelerometerData.acceleration.y;
		float z = _motionManager.accelerometerData.acceleration.z;
		if (_canvas->GetDeviceOutputRotation() == 180) {
			x = -x;
			y = -y;
		}
		v_internal(UiCure::GetSettings(), kRtvarCtrlAccelerometerX, -y);
		v_internal(UiCure::GetSettings(), kRtvarCtrlAccelerometerY, -z);
		v_internal(UiCure::GetSettings(), kRtvarCtrlAccelerometerZ, +x);
		TrabantSim::TrabantSim::GetApp()->Tick();
	}
}

-(void) touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event {
	uilepra::touch::TouchHandler::HandleTouches(touches, _canvas, TrabantSim::TrabantSim::GetApp()->drag_manager_);
}

-(void) touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
	[self touchesMoved:touches withEvent:event];
}

-(void) touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event {
	[self touchesMoved:touches withEvent:event];
}

/*-(void) interstitialAdActionDidFinish:(ADInterstitialAd*)interstitialAd {
	[self tick_];
}

-(BOOL) interstitialAdActionShouldBegin:(ADInterstitialAd*)interstitialAd willLeaveApplication:(BOOL)willLeave {
	[self tick_];
	return TRUE;
}

-(void) interstitialAdDidUnload:(ADInterstitialAd*)interstitialAd {
	//[interstitialAd release];
	_ad = nil;
	[self createAd];
}

-(void) interstitialAd:(ADInterstitialAd*)interstitialAd didFailWithError:(NSError*)error {
	//[interstitialAd release];
	_ad = nil;
	[self createAd];
}

-(void) createAd {
	if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad) {
		_ad = [[ADInterstitialAd alloc] init];
		_ad.delegate = self;
	}
}*/

-(void) startPurchase:(NSString*)productName {
	if ([SKPaymentQueue canMakePayments]) {
		//TrabantSim::TrabantSim::GetApp()->SetIsPurchasing(true);
		SKProductsRequest* request = [[SKProductsRequest alloc] initWithProductIdentifiers:[NSSet setWithObject:productName]];
		request.delegate = self;
		[request start];
	} else {
		UIAlertView* alertView = [[UIAlertView alloc] initWithTitle:@"Disabled"
			message:@"You have disabled purchases in settings." delegate:self
			cancelButtonTitle:@"OK" otherButtonTitles:nil];
		[alertView show];
		//[alertView release];
	}
}

-(void) productsRequest:(SKProductsRequest*)request didReceiveResponse:(SKProductsResponse*)response {
	NSArray* products = response.products;
	if ([products count] <= 0) {
		return;
	}
	SKProduct* requestedProduct = [products objectAtIndex:0];
	SKPayment *payment = [SKPayment paymentWithProduct:requestedProduct];
	[[SKPaymentQueue defaultQueue] addPayment:payment];
	//[request autorelease];
}

- (void)request:(SKRequest *)request didFailWithError:(NSError *)error {
	UIAlertView* alertView = [[UIAlertView alloc] initWithTitle:@"Error"
		message:@"Unable to contact App Store." delegate:self
		cancelButtonTitle:@"OK" otherButtonTitles:nil];
	[alertView show];
	//[alertView release];
}

-(void) paymentQueue:(SKPaymentQueue*)queue updatedTransactions:(NSArray*)transactions {
	for (SKPaymentTransaction *transaction in transactions) {
		switch (transaction.transactionState) {
			case SKPaymentTransactionStatePurchased:	[self completeTransaction:transaction];	break;
			case SKPaymentTransactionStateFailed:		[self failedTransaction:transaction];	break;
			case SKPaymentTransactionStateRestored:		[self restoreTransaction:transaction];	break;
		}
	}
}

-(void) completeTransaction:(SKPaymentTransaction*)transaction {
	[self provideContent:transaction.payment.productIdentifier confirm:YES];
	[[SKPaymentQueue defaultQueue] finishTransaction:transaction];
}

-(void) restoreTransaction:(SKPaymentTransaction*)transaction {
	[self provideContent:transaction.originalTransaction.payment.productIdentifier confirm:NO];
	[[SKPaymentQueue defaultQueue] finishTransaction: transaction];
}

-(void) failedTransaction:(SKPaymentTransaction *)transaction {
	if (transaction.error.code != SKErrorPaymentCancelled) {
		UIAlertView* alertView = [[UIAlertView alloc] initWithTitle:@"Warning"
			message:@"Purchase failed. No money deducted, no content unlocked." delegate:self
			cancelButtonTitle:@"OK" otherButtonTitles:nil];
		[alertView show];
		//[alertView release];
	}
	[[SKPaymentQueue defaultQueue] finishTransaction:transaction];
}

-(void) provideContent:(NSString*)productIdentifier confirm:(BOOL)confirm {
	TrabantSim::TrabantSim::GetApp()->SavePurchase();

	if (confirm) {
		UIAlertView* alertView = [[UIAlertView alloc] initWithTitle:@"Thanks!"
			message:@"Content purchased and unlocked."
			delegate:self cancelButtonTitle:@"Great!" otherButtonTitles:nil];
		[alertView show];
		//[alertView release];
	} else {
		[self alertViewCancel:nil];
	}
}

-(void) setAccessFor:(NSString*)hostname allow:(BOOL)allow {
	str hosts = allow? v_slowget(UiCure::GetSettings(), "Simulator.AllowedHosts", "") : v_slowget(UiCure::GetSettings(), "Simulator.DeniedHosts", "");
	strutil::strvec hostnames = strutil::Split(hosts, ":");
	hostnames.push_back(MacLog::Decode(hostname));
	hosts = strutil::Join(hostnames, ":");
	allow? v_override(UiCure::GetSettings(), "Simulator.AllowedHosts", hosts) : v_override(UiCure::GetSettings(), "Simulator.DeniedHosts", hosts);
}

-(void) alertView:(UIAlertView*)alertView clickedButtonAtIndex:(NSInteger)buttonIndex {
	if (buttonIndex == 1) {
		[self setAccessFor:_alertHost allow:YES];
	} else {
		[self setAccessFor:_alertHost allow:NO];
	}
	[self alertViewCancel:alertView];
}

-(void) alertViewCancel:(UIAlertView*)alertView {
	self.alertView = nil;
	//TrabantSim::TrabantSim::GetApp()->SetIsPurchasing(false);
}

@end

#endif // iOS
