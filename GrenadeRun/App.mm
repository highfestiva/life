
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Lepra/Include/LepraTarget.h"
#import <StoreKit/StoreKit.h>



namespace Lepra
{
class Canvas;
}
using namespace Lepra;



#ifdef LEPRA_IOS
@interface AnimatedApp: UIResponder <SKProductsRequestDelegate, SKPaymentTransactionObserver, UIAlertViewDelegate>
{
@private
	Canvas* canvas;
	NSTimer* animationTimer;
}

-(id) init:(Canvas*)pCanvas;
-(void) startTick;
-(void) stopTick;
-(void) tick;
-(void) dropFingerMovements;

-(void) startPurchase:(NSString*)productName;
-(void) completeTransaction:(SKPaymentTransaction*)transaction;
-(void) restoreTransaction:(SKPaymentTransaction*)transaction;
-(void) failedTransaction: (SKPaymentTransaction*)transaction;
-(void) provideContent:(NSString*)productIdentifier;
-(void) updateContent;
@end
#endif // iOS



#include "App.cxx"



#ifdef LEPRA_IOS

#import "../UiLepra/Include/Mac/EAGLView.h"

@implementation AnimatedApp
-(id) init:(Canvas*)pCanvas
{
	canvas = pCanvas;
	animationTimer = nil;
	[self updateContent];
	[[SKPaymentQueue defaultQueue] addTransactionObserver:self];
	return self;
}

-(void) startTick
{
	animationTimer = [NSTimer scheduledTimerWithTimeInterval:0.0225 target:self selector:@selector(tick) userInfo:nil repeats:YES];
	[EAGLView sharedView].responder = self;
}

-(void) stopTick
{
	[animationTimer invalidate];
	animationTimer = nil;
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
		GrenadeRun::App::GetApp()->Poll();
		[self dropFingerMovements];
	}
}

-(GrenadeRun::FingerMovement&) getFingerMovement:(const CGPoint&)pLocation previous:(const CGPoint&)pPrevious
{
	GrenadeRun::FingerMoveList::iterator i = GrenadeRun::gFingerMoveList.begin();
	for (; i != GrenadeRun::gFingerMoveList.end(); ++i)
	{
		//NSLog(@"get: (%i; %i) ==? (%i; %i)", (int)i->mLastX, (int)i->mLastY, (int)pLocation.x, (int)pLocation.y);
		if (i->Update(pPrevious.x, pPrevious.y, pLocation.x, pLocation.y))
		{
			//NSLog(@"get: Match!");
			return *i;
		}
	}
	GrenadeRun::gFingerMoveList.push_back(GrenadeRun::FingerMovement(pLocation.x, pLocation.y));
	return GrenadeRun::gFingerMoveList.back();
}

-(void) dropFingerMovements
{
	GrenadeRun::FingerMoveList::iterator i = GrenadeRun::gFingerMoveList.begin();
	for (; i != GrenadeRun::gFingerMoveList.end();)
	{
		if (!i->mIsPress)
		{
			GrenadeRun::gFingerMoveList.erase(i++);
			//return;
		}
		else
		{
			++i;
		}

	}
}

-(CGPoint) xform:(const CGPoint&)pLocation
{
	if (canvas->GetOutputRotation() == 90)
	{
		return pLocation;
	}
	CGPoint lLocation;
	const CGSize& lSize = [UIScreen mainScreen].bounds.size;
	lLocation.x = lSize.width  - pLocation.x;
	lLocation.y = lSize.height - pLocation.y;
	return lLocation;
}

-(void) touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event
{
	NSEnumerator* e = [touches objectEnumerator];
	UITouch* lTouch;
	while ((lTouch = (UITouch*)[e nextObject]))
	{
		CGPoint lTapPosition = [self xform:[lTouch locationInView:nil]];
		CGPoint lPrevTapPosition = [self xform:[lTouch previousLocationInView:nil]];
		bool lIsPressed = (lTouch.phase != UITouchPhaseEnded && lTouch.phase != UITouchPhaseCancelled);
		GrenadeRun::FingerMovement& lMove = [self getFingerMovement:lTapPosition previous:lPrevTapPosition];
		lMove.mIsPress = lIsPressed;
		/*GrenadeRun::App::OnTap(lMove);
		if (!lIsPressed)
		{
			[self dropFingerMovement:lTapPosition previous:lPrevTapPosition];
		}*/

		//GrenadeRun::App::OnMouseTap(lTapPosition.x, lTapPosition.y, lIsPressed);
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

-(void) startPurchase:(NSString*)productName
{
	if ([SKPaymentQueue canMakePayments])
	{
		GrenadeRun::App::GetApp()->SetIsPurchasing(true);
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
	SKProduct* product = [products objectAtIndex:0];

	NSNumberFormatter* priceFormatter = [[NSNumberFormatter alloc] init];
	[priceFormatter setFormatterBehavior:NSNumberFormatterBehavior10_4];
	[priceFormatter setNumberStyle:NSNumberFormatterCurrencyStyle];
	[priceFormatter setLocale:product.priceLocale];
	NSString* price = [priceFormatter stringFromNumber:product.price];
	NSString* message = [product.localizedDescription stringByAppendingFormat:@"\n\nPrice: %@\n\nInterested?", price];

	UIAlertView* alertView = [[UIAlertView alloc] initWithTitle:product.localizedTitle
		message:message delegate:self
		cancelButtonTitle:@"Cancel" otherButtonTitles:@"OK", nil];
	[alertView show];
	[alertView release];

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
	[self provideContent:transaction.payment.productIdentifier];
	[[SKPaymentQueue defaultQueue] finishTransaction:transaction];
}

-(void) restoreTransaction:(SKPaymentTransaction*)transaction
{
	[self provideContent:transaction.originalTransaction.payment.productIdentifier];
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

-(void) provideContent:(NSString*)productIdentifier
{
	NSUserDefaults* lDefaults = [NSUserDefaults standardUserDefaults];
	[lDefaults setInteger:1 forKey:productIdentifier];
	[self updateContent];

	UIAlertView* alertView = [[UIAlertView alloc] initWithTitle:@"Thanks!"
		message:@"Content purchased and unlocked. (The author may well invest in a chewing-gum.)"
		delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
	[alertView show];
	[alertView release];
}

-(void) updateContent
{
	NSUserDefaults* lDefaults = [NSUserDefaults standardUserDefaults];

	NSInteger hasLevels = [lDefaults integerForKey:@ CONTENT_LEVELS];
	CURE_RTVAR_SET(GrenadeRun::App::GetApp()->mVariableScope, RTVAR_CONTENT_LEVELS, (hasLevels != 0));

	NSInteger hasVehicles = [lDefaults integerForKey:@ CONTENT_VEHICLES];
	CURE_RTVAR_SET(GrenadeRun::App::GetApp()->mVariableScope, RTVAR_CONTENT_LEVELS, (hasVehicles != 0));
}

-(void) alertView:(UIAlertView*)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
}

-(void) alertViewCancel:(UIAlertView*)alertView
{
	GrenadeRun::App::GetApp()->SetIsPurchasing(false);
}

@end

#endif // iOS
