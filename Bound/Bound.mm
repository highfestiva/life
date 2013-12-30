
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../Lepra/Include/LepraTarget.h"
#include "../UiLepra/Include/Mac/UiMacTouchHandler.h"
#ifdef LEPRA_IOS
#import <StoreKit/StoreKit.h>
#import <CoreMotion/CoreMotion.h>
#import <iAd/ADInterstitialAd.h>
#endif // iOS


namespace Lepra
{
class Canvas;
}
using namespace Lepra;



#ifdef LEPRA_IOS
@interface AnimatedApp: UIResponder <ADInterstitialAdDelegate, SKProductsRequestDelegate, SKPaymentTransactionObserver, UIAlertViewDelegate>
{
@private
	Canvas* _canvas;
	NSTimer* _animationTimer;
	CMMotionManager* _motionManager;
	ADInterstitialAd* _ad;
}

-(id) init:(Canvas*)pCanvas;
-(void) dealloc;
-(void) startTick;
-(void) stopTick;
-(void) showAd;
-(void) tick;

-(void) startPurchase:(NSString*)productName;
-(void) completeTransaction:(SKPaymentTransaction*)transaction;
-(void) restoreTransaction:(SKPaymentTransaction*)transaction;
-(void) failedTransaction: (SKPaymentTransaction*)transaction;
-(void) provideContent:(NSString*)productIdentifier confirm:(BOOL)confirm;
@end
#endif // iOS



#include "Bound.h"
#include "Bound.cxx"



#ifdef LEPRA_IOS

#import "../UiLepra/Include/Mac/EAGLView.h"

@implementation AnimatedApp

-(id) init:(Canvas*)pCanvas
{
	[UIApplication sharedApplication].statusBarOrientation = UIInterfaceOrientationLandscapeRight;
	_canvas = pCanvas;
	_canvas->SetDeviceRotation(+90);
	_animationTimer = nil;
	_motionManager = [[CMMotionManager alloc] init];
	[self createAd];
	[[SKPaymentQueue defaultQueue] addTransactionObserver:self];
	return self;
}

-(void) dealloc
{
        [super dealloc];
}

-(void) startTick
{
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

-(void) showAd
{
	if (_ad.loaded)
	{
		[_ad presentFromViewController:[EAGLView sharedView].window.rootViewController];
	}
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
		const float x = _motionManager.accelerometerData.acceleration.x;
		const float y = _motionManager.accelerometerData.acceleration.y;
		const float z = _motionManager.accelerometerData.acceleration.z;
		CURE_RTVAR_SET(UiCure::GetSettings(), RTVAR_CTRL_ACCELEROMETER_X,  y);
		CURE_RTVAR_SET(UiCure::GetSettings(), RTVAR_CTRL_ACCELEROMETER_Y, -z);
		CURE_RTVAR_SET(UiCure::GetSettings(), RTVAR_CTRL_ACCELEROMETER_Z, -x);
		Bound::Bound::GetApp()->Tick();
	}
}

-(void) touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event
{
	UiLepra::Touch::TouchHandler::HandleTouches(touches, _canvas, Bound::Bound::GetApp()->mDragManager);
}

-(void) touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	[self touchesMoved:touches withEvent:event];
}

-(void) touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
	[self touchesMoved:touches withEvent:event];
}

-(void) interstitialAdActionDidFinish:(ADInterstitialAd*)interstitialAd
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
	[interstitialAd release];
	_ad = nil;
	[self createAd];
}

-(void) interstitialAd:(ADInterstitialAd*)interstitialAd didFailWithError:(NSError*)error
{
	[interstitialAd release];
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
}

-(void) startPurchase:(NSString*)productName
{
	if ([SKPaymentQueue canMakePayments])
	{
		//Bound::Bound::GetApp()->SetIsPurchasing(true);
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
		[alertView release];
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
	[request autorelease];
}

- (void)request:(SKRequest *)request didFailWithError:(NSError *)error
{
	UIAlertView* alertView = [[UIAlertView alloc] initWithTitle:@"Error"
		message:@"Unable to contact App Store." delegate:self
		cancelButtonTitle:@"OK" otherButtonTitles:nil];
	[alertView show];
	[alertView release];
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
		[alertView release];
	}
	[[SKPaymentQueue defaultQueue] finishTransaction:transaction];
}

-(void) provideContent:(NSString*)productIdentifier confirm:(BOOL)confirm
{
	Bound::Bound::GetApp()->SavePurchase();

	if (confirm)
	{
		UIAlertView* alertView = [[UIAlertView alloc] initWithTitle:@"Thanks!"
			message:@"Content purchased and unlocked. (The author may well invest in a chewing-gum.)"
			delegate:self cancelButtonTitle:@"Chew away!" otherButtonTitles:nil];
		[alertView show];
		[alertView release];
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
	//Bound::Bound::GetApp()->SetIsPurchasing(false);
}

@end

#endif // iOS
