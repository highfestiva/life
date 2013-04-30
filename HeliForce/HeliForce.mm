
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Lepra/Include/LepraTarget.h"
#ifdef LEPRA_IOS
#import <StoreKit/StoreKit.h>
#endif // iOS


namespace Lepra
{
class Canvas;
}
using namespace Lepra;



#ifdef LEPRA_IOS
//@interface AnimatedApp: UIResponder <SKProductsRequestDelegate, SKPaymentTransactionObserver, UIAlertViewDelegate>
@interface AnimatedApp: UIResponder
{
@private
	Canvas* _canvas;
	NSTimer* _animationTimer;
	//SKProduct* _requestedProduct;
}

//@property(nonatomic, retain) SKProduct* requestedProduct;

+(void) updateContent;
+(void) storeHiscoreName;
-(id) init:(Canvas*)pCanvas;
-(void) dealloc;
-(void) startTick;
-(void) stopTick;
-(void) tick;

//-(void) startPurchase:(NSString*)productName;
//-(void) completeTransaction:(SKPaymentTransaction*)transaction;
//-(void) restoreTransaction:(SKPaymentTransaction*)transaction;
//-(void) failedTransaction: (SKPaymentTransaction*)transaction;
//-(void) provideContent:(NSString*)productIdentifier confirm:(BOOL)confirm;
@end
#endif // iOS



#include "HeliForce.cxx"



#ifdef LEPRA_IOS

#import "../UiLepra/Include/Mac/EAGLView.h"

//#define HISCORE_NAME_KEY @"HiscoreName"

@implementation AnimatedApp

//@synthesize requestedProduct = _requestedProduct;

+(void) updateContent
{
	//NSUserDefaults* defaults = [NSUserDefaults standardUserDefaults];

	//NSInteger hasLevels = [defaults integerForKey:@ CONTENT_LEVELS];
	//CURE_RTVAR_SET(HeliForce::HeliForce::GetApp()->mVariableScope, RTVAR_CONTENT_LEVELS, (hasLevels == 1));

	//NSInteger hasVehicles = [defaults integerForKey:@ CONTENT_VEHICLES];
	//CURE_RTVAR_SET(HeliForce::HeliForce::GetApp()->mVariableScope, RTVAR_CONTENT_VEHICLES, (hasVehicles == 1));

	//NSString* objcHiscoreName = [defaults stringForKey:HISCORE_NAME_KEY];
	//const str hiscoreName = MacLog::Decode(objcHiscoreName);
	//CURE_RTVAR_SET(HeliForce::HeliForce::GetApp()->mVariableScope, RTVAR_HISCORE_NAME, hiscoreName);
}

+(void) storeHiscoreName
{
	//str lLastHiscoreName;
	//CURE_RTVAR_GET(lLastHiscoreName, =, HeliForce::HeliForce::GetApp()->mVariableScope, RTVAR_HISCORE_NAME, _T(""));
	//NSString* name = [MacLog::Encode(lLastHiscoreName) retain];
	//NSUserDefaults* defaults = [NSUserDefaults standardUserDefaults];
	//[defaults setObject:name forKey:HISCORE_NAME_KEY];
	//[name release];
}

-(id) init:(Canvas*)pCanvas
{
	[UIApplication sharedApplication].statusBarOrientation = UIInterfaceOrientationLandscapeRight;
	_canvas = pCanvas;
	_animationTimer = nil;
	//[[SKPaymentQueue defaultQueue] addTransactionObserver:self];
	return self;
}

-(void) dealloc
{
	//self.requestedProduct = nil;
        [super dealloc];
}

-(void) startTick
{
	_animationTimer = [NSTimer scheduledTimerWithTimeInterval:0.0001 target:self selector:@selector(tick) userInfo:nil repeats:YES];
	[EAGLView sharedView].responder = self;
	[[EAGLView sharedView] powerUpAcc];
}

-(void) stopTick
{
	[[EAGLView sharedView] powerDownAcc];
	[_animationTimer invalidate];
	_animationTimer = nil;
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
		HeliForce::HeliForce::GetApp()->Tick();
	}
}

//-(CGPoint) xform:(const CGPoint&)pLocation
//{
//	if (_canvas->GetOutputRotation() == 90)
//	{
//		return pLocation;
//	}
//	CGPoint lLocation;
//	const CGSize& lSize = [UIScreen mainScreen].bounds.size;
//	lLocation.x = lSize.width  - pLocation.x;
//	lLocation.y = lSize.height - pLocation.y;
//	return lLocation;
//}

-(void) touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event
{
	NSEnumerator* e = [touches objectEnumerator];
	UITouch* lTouch;
	while ((lTouch = (UITouch*)[e nextObject]))
	{
		CGPoint lTapPosition = [lTouch locationInView:nil];
		CGPoint lPrevTapPosition = [lTouch previousLocationInView:nil];
		bool lIsPressed = (lTouch.phase != UITouchPhaseEnded && lTouch.phase != UITouchPhaseCancelled);
		PixelCoord lPreviousTap(lPrevTapPosition.y, _canvas->GetActualWidth() - lPrevTapPosition.x);
		PixelCoord lThisTap(lTapPosition.y, _canvas->GetActualWidth() - lTapPosition.x);
		if (_canvas && _canvas->GetOutputRotation() != 90)
		{
			lPreviousTap = PixelCoord(_canvas->GetActualHeight() - lPrevTapPosition.y, lPrevTapPosition.x);
			lThisTap = PixelCoord(_canvas->GetActualHeight() - lTapPosition.y, lTapPosition.x);
		}
		HeliForce::HeliForce::GetApp()->mDragManager.UpdateDrag(lPreviousTap, lThisTap, lIsPressed);
	}
}

-(void) touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	[self touchesMoved:touches withEvent:event];
}

-(void) touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
	[self touchesMoved:touches withEvent:event];
}

//-(void) startPurchase:(NSString*)productName
//{
//	if ([SKPaymentQueue canMakePayments])
//	{
//		HeliForce::HeliForce::GetApp()->SetIsPurchasing(true);
//		SKProductsRequest* request = [[SKProductsRequest alloc] initWithProductIdentifiers:[NSSet setWithObject:productName]];
//		request.delegate = self;
//		[request start];
//	}
//	else
//	{
//		UIAlertView* alertView = [[UIAlertView alloc] initWithTitle:@"Disabled"
//			message:@"You have disabled purchases in settings." delegate:self
//			cancelButtonTitle:@"OK" otherButtonTitles:nil];
//		[alertView show];
//		[alertView release];
//	}
//}
//
//-(void) productsRequest:(SKProductsRequest*)request didReceiveResponse:(SKProductsResponse*)response
//{
//	NSArray* products = response.products;
//	self.requestedProduct = [products objectAtIndex:0];
//
//	NSNumberFormatter* priceFormatter = [[NSNumberFormatter alloc] init];
//	[priceFormatter setFormatterBehavior:NSNumberFormatterBehavior10_4];
//	[priceFormatter setNumberStyle:NSNumberFormatterCurrencyStyle];
//	[priceFormatter setLocale:self.requestedProduct.priceLocale];
//	NSString* price = [priceFormatter stringFromNumber:self.requestedProduct.price];
//	NSString* message = [self.requestedProduct.localizedDescription stringByAppendingFormat:@"\n\n%@\n\nInterested?", price];
//
//	UIAlertView* alertView = [[UIAlertView alloc] initWithTitle:self.requestedProduct.localizedTitle
//		message:message delegate:self
//		cancelButtonTitle:@"Cancel" otherButtonTitles:@"OK", nil];
//	[alertView show];
//	[alertView release];
//
//	[request autorelease];
//}
//
//- (void)request:(SKRequest *)request didFailWithError:(NSError *)error
//{
//	UIAlertView* alertView = [[UIAlertView alloc] initWithTitle:@"Error"
//		message:@"Unable to contact App Store." delegate:self
//		cancelButtonTitle:@"OK" otherButtonTitles:nil];
//	[alertView show];
//	[alertView release];
//}
//
//-(void) paymentQueue:(SKPaymentQueue*)queue updatedTransactions:(NSArray*)transactions
//{
//	for (SKPaymentTransaction *transaction in transactions)
//	{
//		switch (transaction.transactionState)
//		{
//			case SKPaymentTransactionStatePurchased:	[self completeTransaction:transaction];	break;
//			case SKPaymentTransactionStateFailed:		[self failedTransaction:transaction];	break;
//			case SKPaymentTransactionStateRestored:		[self restoreTransaction:transaction];	break;
//		}
//	}
//}
//
//-(void) completeTransaction:(SKPaymentTransaction*)transaction
//{
//	[self provideContent:transaction.payment.productIdentifier confirm:YES];
//	[[SKPaymentQueue defaultQueue] finishTransaction:transaction];
//}
//
//-(void) restoreTransaction:(SKPaymentTransaction*)transaction
//{
//	[self provideContent:transaction.originalTransaction.payment.productIdentifier confirm:NO];
//	[[SKPaymentQueue defaultQueue] finishTransaction: transaction];
//}
//
//-(void) failedTransaction:(SKPaymentTransaction *)transaction
//{
//	if (transaction.error.code != SKErrorPaymentCancelled)
//	{
//		UIAlertView* alertView = [[UIAlertView alloc] initWithTitle:@"Warning"
//			message:@"Purchase failed. No money deducted, no content unlocked." delegate:self
//			cancelButtonTitle:@"OK" otherButtonTitles:nil];
//		[alertView show];
//		[alertView release];
//	}
//	[[SKPaymentQueue defaultQueue] finishTransaction:transaction];
//}
//
//-(void) provideContent:(NSString*)productIdentifier confirm:(BOOL)confirm
//{
//	NSUserDefaults* lDefaults = [NSUserDefaults standardUserDefaults];
//	[lDefaults setInteger:1 forKey:productIdentifier];
//	[AnimatedApp updateContent];
//
//	if (confirm)
//	{
//		UIAlertView* alertView = [[UIAlertView alloc] initWithTitle:@"Thanks!"
//			message:@"Content purchased and unlocked. (The author may well invest in a chewing-gum.)"
//			delegate:self cancelButtonTitle:@"Chew away!" otherButtonTitles:nil];
//		[alertView show];
//		[alertView release];
//	}
//	else
//	{
//		[self alertViewCancel:nil];
//	}
//
//}
//
//-(void) alertView:(UIAlertView*)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
//{
//	if (buttonIndex < 1 || self.requestedProduct == nil)
//	{
//		[self alertViewCancel:alertView];
//		return;
//	}
//	SKPayment *payment = [SKPayment paymentWithProduct:self.requestedProduct];
//	[[SKPaymentQueue defaultQueue] addPayment:payment];
//}
//
//-(void) alertViewCancel:(UIAlertView*)alertView
//{
//	self.requestedProduct = nil;
//	HeliForce::HeliForce::GetApp()->SetIsPurchasing(false);
//}

@end

#endif // iOS
