
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../Lepra/Include/LepraOS.h"
#include "../Lepra/Include/LepraTypes.h"
#include "../UiCure/Include/UiCure.h"
#include "../UiLepra/Include/Mac/UiMacTouchHandler.h"
#include "AnimatedApp.h"
#include "TrabantSim.h"
#include "TrabantSim.cxx"



#ifdef LEPRA_IOS

#import "../UiLepra/Include/Mac/EAGLView.h"
#include "EditViewController.h"
#include "ListViewController.h"
#include "StdOutViewController.h"

@implementation AnimatedApp

-(id) init:(Canvas*)pCanvas
{
	// Init stuff for future simulation.
	//[UIApplication sharedApplication].statusBarOrientation = UIInterfaceOrientationLandscapeRight;
	_canvas = pCanvas;
	_canvas->SetDeviceRotation(0);
	_animationTimer = nil;
	_motionManager = [[CMMotionManager alloc] init];

	// Prepare background ad and payment stuff.
	//[self createAd];
	[[SKPaymentQueue defaultQueue] addTransactionObserver:self];

	// Initialize the IDE.
	UIWindow* window = [[UIWindow alloc] initWithFrame:[UIScreen mainScreen].bounds];
	self.window = window;	// Retain it!
	self.listController = [ListViewController new];
	self.listController.title = @"Prototypes";
	UINavigationController* navigationController = [[RotatingController alloc] initWithRootViewController:self.listController];
	if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
	{
		EditViewController* editController = [EditViewController new];
		editController.title = @"";
		self.listController.editController = editController;
		UINavigationController* editHeadController = [[UINavigationController alloc] initWithRootViewController:editController];
		UISplitViewController* splitController = [[UISplitViewController alloc] init];
		//splitController.delegate = self;
		splitController.viewControllers = @[navigationController, editHeadController];
		window.rootViewController = splitController;
	}
	else
	{
		window.rootViewController = navigationController;
	}
	[window makeKeyAndVisible];

	return self;
}

-(void) dealloc
{
        //[super dealloc];
}

-(void) startTick
{
	self.alert = nil;
	if (!_animationTimer)
	{
		_animationTimer = [NSTimer scheduledTimerWithTimeInterval:0.0001 target:self selector:@selector(tick) userInfo:nil repeats:YES];
		[EAGLView sharedView].responder = self;
		[[EAGLView sharedView] powerUpAcc];
		[_motionManager startAccelerometerUpdates];
	}
}

-(void) stopTick
{
	[_motionManager stopAccelerometerUpdates];
	[[EAGLView sharedView] powerDownAcc];
	[_animationTimer invalidate];
	_animationTimer = nil;
}

-(void) pushSimulatorController:(UIViewController*)viewController
{
	[[UIApplication sharedApplication] setStatusBarHidden:YES withAnimation:UIStatusBarAnimationNone];
	[self pushViewController:viewController animated:YES];
}

-(void) pushViewController:(UIViewController*)viewController animated:(BOOL)animated
{
	if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
	{
		const bool isRight = self.window.rootViewController.interfaceOrientation == UIInterfaceOrientationLandscapeRight;
		[EAGLView sharedView].preferredRotation = isRight? UIInterfaceOrientationLandscapeRight : UIInterfaceOrientationLandscapeLeft;
		[(UISplitViewController*)self.window.rootViewController presentViewController:viewController animated:animated completion:nil];
	}
	else
	{
		if ([UIApplication sharedApplication].isStatusBarHidden)
		{
			((UINavigationController*)self.window.rootViewController).navigationBarHidden = YES;
			const bool isRight = self.window.rootViewController.interfaceOrientation == UIInterfaceOrientationLandscapeRight;
			[EAGLView sharedView].preferredRotation = isRight? UIInterfaceOrientationLandscapeRight : UIInterfaceOrientationLandscapeLeft;
		}
		[(UINavigationController*)self.window.rootViewController pushViewController:viewController animated:animated];
	}
	//[UIViewController attemptRotationToDeviceOrientation];
}

-(void) popViewControllerAnimated:(BOOL)animated
{
	if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
	{
		[(UISplitViewController*)self.window.rootViewController dismissViewControllerAnimated:animated completion:nil];
	}
	else
	{
		[(UINavigationController*)self.window.rootViewController popViewControllerAnimated:animated];
		((UINavigationController*)self.window.rootViewController).navigationBarHidden = NO;
	}
	//[UIViewController attemptRotationToDeviceOrientation];
}

-(void) popIfGame
{
	if ([self showingSimulator])
	{
		[self popViewControllerAnimated:YES];
	}
}

-(bool) showingSimulator
{
	if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
	{
		return (((UISplitViewController*)self.window.rootViewController).presentedViewController.view == [EAGLView sharedView]);
	}
	else
	{
		return (((UINavigationController*)self.window.rootViewController).visibleViewController.view == [EAGLView sharedView]);
	}
}

-(void) handleStdOut:(const astr&)pStdOut
{
	NSString* text = [NSString stringWithCString:pStdOut.c_str() encoding:NSUTF8StringEncoding];
	[self popViewControllerAnimated:NO];
	StdOutViewController* stdOutController = [StdOutViewController new];
	stdOutController.title = @"Output";
	stdOutController.text = text;
	if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
	{
		stdOutController.modalPresentationStyle = UIModalPresentationPopover;
		UIPopoverController* popover = [[UIPopoverController alloc] initWithContentViewController:stdOutController];
		CGSize fit = [PythonTextView slowFitTextSize:text];
		popover.popoverContentSize = fit;
		[popover presentPopoverFromBarButtonItem:self.listController.editController.navigationItem.rightBarButtonItem permittedArrowDirections:UIPopoverArrowDirectionAny animated:YES];
	}
	else
	{
		[self pushViewController:stdOutController animated:NO];
	}
}

-(void) showNetworkControlFor:(NSString*)hostname
{
	if (self.alert)
	{
		return;
	}

	NSString* message = [NSString stringWithFormat:@"Do you wish to grant '%@' access to simulation and prototypes?", hostname];
	self.alert = [UIAlertController alertControllerWithTitle:@"Network control" message:message preferredStyle:UIAlertControllerStyleAlert];
	UIAlertAction* grantAction = [UIAlertAction actionWithTitle:@"Permanently"
							      style:UIAlertActionStyleDefault
							    handler:^(UIAlertAction*){
								    self.alert = nil;
								    str lHosts;
								    v_get(lHosts, =, UiCure::GetSettings(), "Simulator.AllowedHosts", "");
								    strutil::strvec lHostnames = strutil::Split(lHosts, _T(":"));
								    lHostnames.push_back(MacLog::Decode(hostname));
								    lHosts = strutil::Join(lHostnames, _T(":"));
								    v_override(UiCure::GetSettings(), "Simulator.AllowedHosts", lHosts);
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
								  str lHosts;
								  v_get(lHosts, =, UiCure::GetSettings(), "Simulator.DeniedHosts", "");
								  strutil::strvec lHostnames = strutil::Split(lHosts, _T(":"));
								  lHostnames.push_back(MacLog::Decode(hostname));
								  lHosts = strutil::Join(lHostnames, _T(":"));
								  v_override(UiCure::GetSettings(), "Simulator.DeniedHosts", lHosts);
							  }];

	[self.alert addAction:grantAction];
	[self.alert addAction:denyAction];
	[self.alert addAction:banAction];
	[self.window.rootViewController presentViewController:self.alert animated:YES completion:nil];
}

-(void) showAd
{
	/*if (_ad.loaded)
	{
		[_ad presentFromViewController:[EAGLView sharedView].window.rootViewController];
	}*/
}

-(void) tick
{
	EAGLView* lGlView = [EAGLView sharedView];
	if (!lGlView.isOpen)
	{
		[lGlView setFramebuffer];
	}
	else
	{
		lGlView.canvas = _canvas;
		float x = _motionManager.accelerometerData.acceleration.x;
		float y = _motionManager.accelerometerData.acceleration.y;
		float z = _motionManager.accelerometerData.acceleration.z;
		if (_canvas->GetDeviceOutputRotation() == 180)
		{
			x = -x;
			y = -y;
		}
		v_internal(UiCure::GetSettings(), RTVAR_CTRL_ACCELEROMETER_X, -y);
		v_internal(UiCure::GetSettings(), RTVAR_CTRL_ACCELEROMETER_Y, -z);
		v_internal(UiCure::GetSettings(), RTVAR_CTRL_ACCELEROMETER_Z, +x);
		TrabantSim::TrabantSim::GetApp()->Tick();
	}
}

-(void) touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event
{
	UiLepra::Touch::TouchHandler::HandleTouches(touches, _canvas, TrabantSim::TrabantSim::GetApp()->mDragManager);
}

-(void) touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	[self touchesMoved:touches withEvent:event];
}

-(void) touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
	[self touchesMoved:touches withEvent:event];
}

/*-(void) interstitialAdActionDidFinish:(ADInterstitialAd*)interstitialAd
{
	[self startTick];
}

-(BOOL) interstitialAdActionShouldBegin:(ADInterstitialAd*)interstitialAd willLeaveApplication:(BOOL)willLeave
{
	[self stopTick];
	return TRUE;
}

-(void) interstitialAdDidUnload:(ADInterstitialAd*)interstitialAd
{
	//[interstitialAd release];
	_ad = nil;
	[self createAd];
}

-(void) interstitialAd:(ADInterstitialAd*)interstitialAd didFailWithError:(NSError*)error
{
	//[interstitialAd release];
	_ad = nil;
	[self createAd];
}

-(void) createAd
{
	if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
	{
		_ad = [[ADInterstitialAd alloc] init];
		_ad.delegate = self;
	}
}*/

-(void) startPurchase:(NSString*)productName
{
	if ([SKPaymentQueue canMakePayments])
	{
		//TrabantSim::TrabantSim::GetApp()->SetIsPurchasing(true);
		SKProductsRequest* request = [[SKProductsRequest alloc] initWithProductIdentifiers:[NSSet setWithObject:productName]];
		request.delegate = self;
		[request start];
	}
	else
	{
		UIAlertView* alertView = [[UIAlertView alloc] initWithTitle:@"Disabled"
			message:@"You have disabled purchases in settings." delegate:self
			cancelButtonTitle:@"OK" otherButtonTitles:nil];
		[alertView show];
		//[alertView release];
	}
}

-(void) productsRequest:(SKProductsRequest*)request didReceiveResponse:(SKProductsResponse*)response
{
	NSArray* products = response.products;
	if ([products count] <= 0)
	{
		return;
	}
	SKProduct* requestedProduct = [products objectAtIndex:0];
	SKPayment *payment = [SKPayment paymentWithProduct:requestedProduct];
	[[SKPaymentQueue defaultQueue] addPayment:payment];
	//[request autorelease];
}

- (void)request:(SKRequest *)request didFailWithError:(NSError *)error
{
	UIAlertView* alertView = [[UIAlertView alloc] initWithTitle:@"Error"
		message:@"Unable to contact App Store." delegate:self
		cancelButtonTitle:@"OK" otherButtonTitles:nil];
	[alertView show];
	//[alertView release];
}

-(void) paymentQueue:(SKPaymentQueue*)queue updatedTransactions:(NSArray*)transactions
{
	for (SKPaymentTransaction *transaction in transactions)
	{
		switch (transaction.transactionState)
		{
			case SKPaymentTransactionStatePurchased:	[self completeTransaction:transaction];	break;
			case SKPaymentTransactionStateFailed:		[self failedTransaction:transaction];	break;
			case SKPaymentTransactionStateRestored:		[self restoreTransaction:transaction];	break;
		}
	}
}

-(void) completeTransaction:(SKPaymentTransaction*)transaction
{
	[self provideContent:transaction.payment.productIdentifier confirm:YES];
	[[SKPaymentQueue defaultQueue] finishTransaction:transaction];
}

-(void) restoreTransaction:(SKPaymentTransaction*)transaction
{
	[self provideContent:transaction.originalTransaction.payment.productIdentifier confirm:NO];
	[[SKPaymentQueue defaultQueue] finishTransaction: transaction];
}

-(void) failedTransaction:(SKPaymentTransaction *)transaction
{
	if (transaction.error.code != SKErrorPaymentCancelled)
	{
		UIAlertView* alertView = [[UIAlertView alloc] initWithTitle:@"Warning"
			message:@"Purchase failed. No money deducted, no content unlocked." delegate:self
			cancelButtonTitle:@"OK" otherButtonTitles:nil];
		[alertView show];
		//[alertView release];
	}
	[[SKPaymentQueue defaultQueue] finishTransaction:transaction];
}

-(void) provideContent:(NSString*)productIdentifier confirm:(BOOL)confirm
{
	TrabantSim::TrabantSim::GetApp()->SavePurchase();

	if (confirm)
	{
		UIAlertView* alertView = [[UIAlertView alloc] initWithTitle:@"Thanks!"
			message:@"Content purchased and unlocked."
			delegate:self cancelButtonTitle:@"Great!" otherButtonTitles:nil];
		[alertView show];
		//[alertView release];
	}
	else
	{
		[self alertViewCancel:nil];
	}
}

-(void) alertView:(UIAlertView*)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
	[self alertViewCancel:alertView];
}

-(void) alertViewCancel:(UIAlertView*)alertView
{
	//TrabantSim::TrabantSim::GetApp()->SetIsPurchasing(false);
}

@end

#endif // iOS
