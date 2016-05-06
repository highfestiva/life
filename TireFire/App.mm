
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../lepra/include/lepraos.h"
#ifdef LEPRA_IOS
#import <StoreKit/StoreKit.h>
#endif // iOS


namespace lepra {
class Canvas;
}
using namespace lepra;



#ifdef LEPRA_IOS
@interface AnimatedApp: UIResponder <SKProductsRequestDelegate, SKPaymentTransactionObserver, UIAlertViewDelegate>
{
@private
	Canvas* _canvas;
	NSTimer* _animationTimer;
	SKProduct* _requestedProduct;
}

@property(nonatomic, retain) SKProduct* requestedProduct;

+(void) updateContent;
+(void) hiscore_name_;
-(id) init:(Canvas*)_canvas;
-(void) dealloc;
-(void) tick_;
-(void) tick_;
-(void) tick;
-(void) dropFingerMovements;

-(void) startPurchase:(NSString*)productName;
-(void) completeTransaction:(SKPaymentTransaction*)transaction;
-(void) restoreTransaction:(SKPaymentTransaction*)transaction;
-(void) failedTransaction: (SKPaymentTransaction*)transaction;
-(void) provideContent:(NSString*)productIdentifier confirm:(BOOL)confirm;
@end
#endif // iOS



#include "app.cxx"



#ifdef LEPRA_IOS

#import "../uilepra/include/mac/eaglview.h"

#define HISCORE_NAME_KEY @"HiscoreName"

@implementation AnimatedApp

@synthesize requestedProduct = _requestedProduct;

+(void) updateContent {
	NSUserDefaults* defaults = [NSUserDefaults standardUserDefaults];

	NSInteger hasLevels = [defaults integerForKey:@ kContentLevels];
	v_set(tirefire::App::GetApp()->variable_scope_, kRtvarContentLevels, (hasLevels == 1));

	NSInteger hasVehicles = [defaults integerForKey:@ kContentVehicles];
	v_set(tirefire::App::GetApp()->variable_scope_, kRtvarContentVehicles, (hasVehicles == 1));

	NSString* objcHiscoreName = [defaults stringForKey:HISCORE_NAME_KEY];
	const str hiscoreName = MacLog::Decode(objcHiscoreName);
	v_set(tirefire::App::GetApp()->variable_scope_, kRtvarHiscoreName, hiscoreName);
}

+(void) hiscore_name_
{
	str last_hiscore_name;
	v_get(last_hiscore_name, =, tirefire::App::GetApp()->variable_scope_, kRtvarHiscoreName, "");
	NSString* name = [MacLog::Encode(last_hiscore_name) retain];
	NSUserDefaults* defaults = [NSUserDefaults standardUserDefaults];
	[defaults setObject:name forKey:HISCORE_NAME_KEY];
	[name release];
}

-(id) init:(Canvas*)_canvas {
	[UIApplication sharedApplication].statusBarOrientation_ = UIInterfaceOrientationLandscapeRight;
	_canvas = _canvas;
	_animationTimer = nil;
	[[SKPaymentQueue defaultQueue] addTransactionObserver:self];
	return self;
}

-(void) dealloc {
	self.requestedProduct = nil;
        [super dealloc];
}

-(void) tick_
{
	_animationTimer = [NSTimer scheduledTimerWithTimeInterval:0.005 target:self selector:@selector(tick) userInfo:nil repeats:YES];
	[EAGLView sharedView_].responder = self;
	[[EAGLView sharedView_] up_acc];
}

-(void) tick_
{
	[[EAGLView sharedView_] down_acc];
	[_animationTimer invalidate];
	_animationTimer = nil;
}

-(void) tick {
	EAGLView* gl_view = [EAGLView sharedView_];
	if (!gl_view.isOpen) {
		[gl_view framebuffer_];
	} else {
		gl_view.canvas = _canvas;
		tirefire::App::GetApp()->Poll();
		[self dropFingerMovements];
	}
}

-(tirefire::FingerMovement&) getFingerMovement:(const CGPoint&)location previous:(const CGPoint&)_previous {
	tirefire::FingerMoveList::iterator i = tirefire::g_finger_move_list.begin();
	for (; i != tirefire::g_finger_move_list.end(); ++i) {
		//NSLog(@"get: (%i; %i) ==? (%i; %i)", (int)i->last_x_, (int)i->last_y_, (int)location.x, (int)location.y);
		if (i->Update(_previous.x, _previous.y, location.x, location.y)) {
			//NSLog(@"get: Match!");
			return *i;
		}
	}
	tirefire::g_finger_move_list.push_back(tirefire::FingerMovement(location.x, location.y));
	return tirefire::g_finger_move_list.back();
}

-(void) dropFingerMovements {
	tirefire::FingerMoveList::iterator i = tirefire::g_finger_move_list.begin();
	for (; i != tirefire::g_finger_move_list.end();) {
		if (!i->is_press_) {
			tirefire::g_finger_move_list.erase(i++);
			//return;
		} else {
			++i;
		}

	}
}

-(CGPoint) xform:(const CGPoint&)location {
	if (_canvas->GetDeviceOutputRotation() == 90) {
		return location;
	}
	CGPoint _location;
	const CGSize& __size = [UIScreen mainScreen].bounds.size;
	_location.x = __size.width  - location.x;
	_location.y = __size.height - location.y;
	return _location;
}

-(void) touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event {
	NSEnumerator* e = [touches objectEnumerator];
	UITouch* touch;
	while ((touch = (UITouch*)[e nextObject])) {
		CGPoint tap_position = [self xform:[touch locationInView:nil]];
		CGPoint prev_tap_position = [self xform:[touch previousLocationInView:nil]];
		bool is_pressed = (touch.phase != UITouchPhaseEnded && touch.phase != UITouchPhaseCancelled);
		tirefire::FingerMovement& move = [self getFingerMovement:tap_position previous:prev_tap_position];
		move.is_press_ = is_pressed;
		/*tirefire::App::OnTap(move);
		if (!is_pressed) {
			[self dropFingerMovement:tap_position previous:prev_tap_position];
		}*/

		//tirefire::App::OnMouseTap(tap_position.x, tap_position.y, is_pressed);
	}
}

-(void) touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
	[self touchesMoved:touches withEvent:event];
}

-(void) touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event {
	[self touchesMoved:touches withEvent:event];
}

-(void) startPurchase:(NSString*)productName {
	if ([SKPaymentQueue canMakePayments]) {
		tirefire::App::GetApp()->SetIsPurchasing(true);
		SKProductsRequest* request = [[SKProductsRequest alloc] initWithProductIdentifiers:[NSSet setWithObject:productName]];
		request.delegate = self;
		[request start];
	} else {
		UIAlertView* alertView = [[UIAlertView alloc] initWithTitle:@"Disabled"
			message:@"You have disabled purchases in settings." delegate:self
			cancelButtonTitle:@"OK" otherButtonTitles:nil];
		[alertView show];
		[alertView release];
	}
}

-(void) productsRequest:(SKProductsRequest*)request didReceiveResponse:(SKProductsResponse*)response {
	NSArray* products = response.products;
	self.requestedProduct = [products objectAtIndex:0];

	NSNumberFormatter* formatter = [[NSNumberFormatter alloc] init];
	[formatter setFormatterBehavior:NSNumberFormatterBehavior10_4];
	[formatter setNumberStyle:NSNumberFormatterCurrencyStyle];
	[formatter setLocale:self.requestedProduct.priceLocale];
	NSString* price = [formatter stringFromNumber:self.requestedProduct.price];
	NSString* message = [self.requestedProduct.localizedDescription stringByAppendingFormat:@"\n\n%@\n\nInterested?", price];

	UIAlertView* alertView = [[UIAlertView alloc] initWithTitle:self.requestedProduct.localizedTitle
		message:message delegate:self
		cancelButtonTitle:@"Cancel" otherButtonTitles:@"OK", nil];
	[alertView show];
	[alertView release];

	[request autorelease];
}

- (void)request:(SKRequest *)request didFailWithError:(NSError *)error {
	UIAlertView* alertView = [[UIAlertView alloc] initWithTitle:@"Error"
		message:@"Unable to contact App Store." delegate:self
		cancelButtonTitle:@"OK" otherButtonTitles:nil];
	[alertView show];
	[alertView release];
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
		[alertView release];
	}
	[[SKPaymentQueue defaultQueue] finishTransaction:transaction];
}

-(void) provideContent:(NSString*)productIdentifier confirm:(BOOL)confirm {
	NSUserDefaults* __defaults = [NSUserDefaults standardUserDefaults];
	[__defaults setInteger:1 forKey:productIdentifier];
	[AnimatedApp updateContent];

	if (confirm) {
		UIAlertView* alertView = [[UIAlertView alloc] initWithTitle:@"Thanks!"
			message:@"Content purchased and unlocked. (The author may well invest in a chewing-gum.)"
			delegate:self cancelButtonTitle:@"Chew away!" otherButtonTitles:nil];
		[alertView show];
		[alertView release];
	} else {
		[self alertViewCancel:nil];
	}

}

-(void) alertView:(UIAlertView*)alertView clickedButtonAtIndex:(NSInteger)buttonIndex {
	if (buttonIndex < 1 || self.requestedProduct == nil) {
		[self alertViewCancel:alertView];
		return;
	}
	SKPayment *payment = [SKPayment paymentWithProduct:self.requestedProduct];
	[[SKPaymentQueue defaultQueue] addPayment:payment];
}

-(void) alertViewCancel:(UIAlertView*)alertView {
	self.requestedProduct = nil;
	tirefire::App::GetApp()->SetIsPurchasing(false);
}

@end

#endif // iOS
