
// Author: Jonas BystrÃ¶m
// Copyright (c) Pixel Doctrine



#include "../lepra/include/lepraos.h"
#ifdef LEPRA_IOS
#import <StoreKit/StoreKit.h>
#import <iAd/ADBannerView.h>
#include "../lepra/include/vector3d.h"
#endif // iOS


namespace lepra {
class Canvas;
}
using namespace lepra;



#ifdef LEPRA_IOS
@interface AnimatedApp: UIResponder <ADBannerViewDelegate, UIAccelerometerDelegate>
{
@private
	Canvas* _canvas;
	NSTimer* _animationTimer;
	bool _adInitialized;
	float _identityFactor;
}

+(void) hiscore_name_;
-(id) init:(Canvas*)_canvas;
-(void) dealloc;
-(void) tick_;
-(void) tick_;
-(void) tick;
-(void) dropFingerMovements;
@end
#endif // iOS



#include "app.cxx"



#ifdef LEPRA_IOS

#import "../uilepra/include/mac/eaglview.h"

#define HISCORE_NAME_KEY @"HiscoreName"

@implementation AnimatedApp

+(void) hiscore_name_
{
	str last_hiscore_name;
	v_get(last_hiscore_name, =, bounce::App::GetApp()->variable_scope_, kRtvarHiscoreName, "");
	NSString* name = [MacLog::Encode(last_hiscore_name) retain];
	NSUserDefaults* defaults = [NSUserDefaults standardUserDefaults];
	[defaults setObject:name forKey:HISCORE_NAME_KEY];
	[name release];
}

-(id) init:(Canvas*)_canvas {
	_canvas = _canvas;
	_animationTimer = nil;
	_adInitialized = false;
	_identityFactor = 1.0f;

	NSUserDefaults* defaults = [NSUserDefaults standardUserDefaults];
	double time = [defaults doubleForKey:@"FirstStartTime"];
	if (!time) {
		time = HiResTimer::GetSystemCounter() * HiResTimer::GetPeriod();
		[defaults setDouble:time forKey:@"FirstStartTime"];
	}

	[[UIAccelerometer sharedAccelerometer] setUpdateInterval:1.0/kFps/2];
	[[UIAccelerometer sharedAccelerometer] setDelegate:self];

	return self;
}

-(void) dealloc {
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
		if (!_adInitialized) {
			_adInitialized = true;
			ADBannerView* adView = [[ADBannerView alloc] initWithFrame:CGRectZero];
			adView.currentContentSizeIdentifier = ADBannerContentSizeIdentifierPortrait;
			adView.delegate = self;
			[gl_view addSubview:adView];
		}
		gl_view.canvas = _canvas;
		bounce::App::GetApp()->Poll();
		[self dropFingerMovements];
	}
}

-(void) accelerometer:(UIAccelerometer*)accelerometer didAccelerate:(UIAcceleration*)acceleration {
	vec3 __acceleration(acceleration.x, acceleration.y, acceleration.z);
	const float length = std::max(0.1f, __acceleration.GetLength());
	_identityFactor = Math::Lerp(_identityFactor, 1/length, 0.005f);
	__acceleration *= _identityFactor;
	//NSLog(@"mom.acc: %f, sliding avg.inv.acc: %f", length, _identityFactor);
	float lift_factor = __acceleration.GetLength() - 1;
	bounce::App::GetApp()->SetRacketForce(lift_factor, __acceleration);
}

-(bounce::FingerMovement&) getFingerMovement:(const CGPoint&)location previous:(const CGPoint&)_previous {
	bounce::FingerMoveList::iterator i = bounce::g_finger_move_list.begin();
	for (; i != bounce::g_finger_move_list.end(); ++i) {
		//NSLog(@"get: (%i; %i) ==? (%i; %i)", (int)i->last_x_, (int)i->last_y_, (int)location.x, (int)location.y);
		if (i->Update(_previous.x, _previous.y, location.x, location.y)) {
			//NSLog(@"get: Match!");
			return *i;
		}
	}
	bounce::g_finger_move_list.push_back(bounce::FingerMovement(location.x, location.y));
	return bounce::g_finger_move_list.back();
}

-(void) dropFingerMovements {
	bounce::FingerMoveList::iterator i = bounce::g_finger_move_list.begin();
	for (; i != bounce::g_finger_move_list.end();) {
		if (!i->is_press_) {
			bounce::g_finger_move_list.erase(i++);
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
		bounce::FingerMovement& move = [self getFingerMovement:tap_position previous:prev_tap_position];
		move.is_press_ = is_pressed;
		/*bounce::App::OnTap(move);
		if (!is_pressed) {
			[self dropFingerMovement:tap_position previous:prev_tap_position];
		}*/

		//bounce::App::OnMouseTap(tap_position.x, tap_position.y, is_pressed);
	}
}

-(void) touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
	[self touchesMoved:touches withEvent:event];
}

-(void) touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event {
	[self touchesMoved:touches withEvent:event];
}

- (BOOL)bannerViewActionShouldBegin:(ADBannerView*)banner willLeaveApplication:(BOOL)willLeave {
	return YES;
}

- (void)bannerViewActionDidFinish:(ADBannerView*)banner {
}

@end

#endif // iOS
