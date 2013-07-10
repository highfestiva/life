
// Author: Jonas BystrÃ¶m
// Copyright (c) Pixel Doctrine



#include "../Lepra/Include/LepraTarget.h"
#ifdef LEPRA_IOS
#import <StoreKit/StoreKit.h>
#import <iAd/ADBannerView.h>
#include "../Lepra/Include/Vector3D.h"
#endif // iOS


namespace Lepra
{
class Canvas;
}
using namespace Lepra;



#ifdef LEPRA_IOS
@interface AnimatedApp: UIResponder <ADBannerViewDelegate, UIAccelerometerDelegate>
{
@private
	Canvas* _canvas;
	NSTimer* _animationTimer;
	bool _adInitialized;
	float _identityFactor;
}

+(void) storeHiscoreName;
-(id) init:(Canvas*)pCanvas;
-(void) dealloc;
-(void) startTick;
-(void) stopTick;
-(void) tick;
-(void) dropFingerMovements;
@end
#endif // iOS



#include "App.cxx"



#ifdef LEPRA_IOS

#import "../UiLepra/Include/Mac/EAGLView.h"

#define HISCORE_NAME_KEY @"HiscoreName"

@implementation AnimatedApp

+(void) storeHiscoreName
{
	str lLastHiscoreName;
	CURE_RTVAR_GET(lLastHiscoreName, =, Bounce::App::GetApp()->mVariableScope, RTVAR_HISCORE_NAME, _T(""));
	NSString* name = [MacLog::Encode(lLastHiscoreName) retain];
	NSUserDefaults* defaults = [NSUserDefaults standardUserDefaults];
	[defaults setObject:name forKey:HISCORE_NAME_KEY];
	[name release];
}

-(id) init:(Canvas*)pCanvas
{
	_canvas = pCanvas;
	_animationTimer = nil;
	_adInitialized = false;
	_identityFactor = 1.0f;

	NSUserDefaults* defaults = [NSUserDefaults standardUserDefaults];
	double time = [defaults doubleForKey:@"FirstStartTime"];
	if (!time)
	{
		time = HiResTimer::GetSystemCounter() * HiResTimer::GetPeriod();
		[defaults setDouble:time forKey:@"FirstStartTime"];
	}

	[[UIAccelerometer sharedAccelerometer] setUpdateInterval:1.0/FPS/2];
	[[UIAccelerometer sharedAccelerometer] setDelegate:self];

	return self;
}

-(void) dealloc
{
        [super dealloc];
}

-(void) startTick
{
	_animationTimer = [NSTimer scheduledTimerWithTimeInterval:0.005 target:self selector:@selector(tick) userInfo:nil repeats:YES];
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
		if (!_adInitialized)
		{
			_adInitialized = true;
			ADBannerView* adView = [[ADBannerView alloc] initWithFrame:CGRectZero];
			adView.currentContentSizeIdentifier = ADBannerContentSizeIdentifierPortrait;
			adView.delegate = self;
			[lGlView addSubview:adView];
		}
		lGlView.canvas = _canvas;
		Bounce::App::GetApp()->Poll();
		[self dropFingerMovements];
	}
}

-(void) accelerometer:(UIAccelerometer*)accelerometer didAccelerate:(UIAcceleration*)acceleration
{
	Vector3DF lAcceleration(acceleration.x, acceleration.y, acceleration.z);
	const float lLength = std::max(0.1f, lAcceleration.GetLength());
	_identityFactor = Math::Lerp(_identityFactor, 1/lLength, 0.005f);
	lAcceleration *= _identityFactor;
	//NSLog(@"mom.acc: %f, sliding avg.inv.acc: %f", lLength, _identityFactor);
	float lLiftFactor = lAcceleration.GetLength() - 1;
	Bounce::App::GetApp()->SetRacketForce(lLiftFactor, lAcceleration);
}

-(Bounce::FingerMovement&) getFingerMovement:(const CGPoint&)pLocation previous:(const CGPoint&)pPrevious
{
	Bounce::FingerMoveList::iterator i = Bounce::gFingerMoveList.begin();
	for (; i != Bounce::gFingerMoveList.end(); ++i)
	{
		//NSLog(@"get: (%i; %i) ==? (%i; %i)", (int)i->mLastX, (int)i->mLastY, (int)pLocation.x, (int)pLocation.y);
		if (i->Update(pPrevious.x, pPrevious.y, pLocation.x, pLocation.y))
		{
			//NSLog(@"get: Match!");
			return *i;
		}
	}
	Bounce::gFingerMoveList.push_back(Bounce::FingerMovement(pLocation.x, pLocation.y));
	return Bounce::gFingerMoveList.back();
}

-(void) dropFingerMovements
{
	Bounce::FingerMoveList::iterator i = Bounce::gFingerMoveList.begin();
	for (; i != Bounce::gFingerMoveList.end();)
	{
		if (!i->mIsPress)
		{
			Bounce::gFingerMoveList.erase(i++);
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
	if (_canvas->GetOutputRotation() == 90)
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
		Bounce::FingerMovement& lMove = [self getFingerMovement:lTapPosition previous:lPrevTapPosition];
		lMove.mIsPress = lIsPressed;
		/*Bounce::App::OnTap(lMove);
		if (!lIsPressed)
		{
			[self dropFingerMovement:lTapPosition previous:lPrevTapPosition];
		}*/

		//Bounce::App::OnMouseTap(lTapPosition.x, lTapPosition.y, lIsPressed);
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

- (BOOL)bannerViewActionShouldBegin:(ADBannerView*)banner willLeaveApplication:(BOOL)willLeave {
	return YES;
}

- (void)bannerViewActionDidFinish:(ADBannerView*)banner {
}

@end

#endif // iOS
