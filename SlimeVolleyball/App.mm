
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../lepra/include/lepraos.h"



namespace lepra {
class Canvas;
}
using namespace lepra;



#ifdef LEPRA_IOS
@interface AnimatedApp: UIResponder {
@private
	Canvas* canvas;
	NSTimer* animationTimer;
}

-(id) init:(Canvas*)_canvas;
-(void) tick_;
-(void) tick_;
-(void) tick;
@end
#endif // iOS



#include "app.cxx"



#ifdef LEPRA_IOS

#import "../uilepra/include/mac/eaglview.h"

@implementation AnimatedApp
-(id) init:(Canvas*)_canvas {
	[UIApplication sharedApplication].statusBarOrientation_ = UIInterfaceOrientationLandscapeRight;
	canvas = _canvas;
	return self;
}

-(void) tick_
{
	animationTimer = [NSTimer scheduledTimerWithTimeInterval:0.0225 target:self selector:@selector(tick) userInfo:nil repeats:YES];
	[EAGLView sharedView_].responder = self;
	[[EAGLView sharedView_] up_acc];
}

-(void) tick_
{
	[[EAGLView sharedView_] down_acc];
	[animationTimer invalidate];
	animationTimer = nil;
}

-(void) tick {
	EAGLView* gl_view = [EAGLView sharedView_];
	if (!gl_view.isOpen) {
		[gl_view framebuffer_];
	} else {
		gl_view.canvas = canvas;
		slime::App::PollApp();
	}
}

-(slime::FingerMovement&) getFingerMovement:(const CGPoint&)location previous:(const CGPoint&)_previous {
	slime::FingerMoveList::iterator i = slime::g_finger_move_list.begin();
	for (; i != slime::g_finger_move_list.end(); ++i) {
		if (i->Update(_previous.x, _previous.y, location.x, location.y)) {
			return *i;
		}
	}
	slime::g_finger_move_list.push_back(slime::FingerMovement(location.x, location.y));
	return slime::g_finger_move_list.back();
}

-(void) dropFingerMovement:(const CGPoint&)location previous:(const CGPoint&)_previous {
	slime::FingerMoveList::iterator i = slime::g_finger_move_list.begin();
	for (; i != slime::g_finger_move_list.end(); ++i) {
		if (i->Update(_previous.x, _previous.y, location.x, location.y)) {
			slime::g_finger_move_list.erase(i);
			return;
		}
	}
	// Oops! This happens, but not sure why.
	slime::g_finger_move_list.clear();
}

-(CGPoint) xform:(const CGPoint&)location {
	if (canvas->GetDeviceOutputRotation() == 90) {
		return location;
	}
	CGPoint _location;
	const CGSize& __size = [UIScreen mainScreen].bounds.size;
	_location.x = __size.width  - location.x;
	_location.y = __size.height - location.y;
	return _location;
}

- (void)touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event {
	NSEnumerator* e = [touches objectEnumerator];
	UITouch* touch;
	while ((touch = (UITouch*)[e nextObject])) {
		CGPoint tap_position = [self xform:[touch locationInView:nil]];
		CGPoint prev_tap_position = [self xform:[touch previousLocationInView:nil]];
		bool is_pressed = (touch.phase != UITouchPhaseEnded && touch.phase != UITouchPhaseEnded);
		slime::FingerMovement& move = [self getFingerMovement:tap_position previous:prev_tap_position];
		move.is_press_ = is_pressed;
		slime::App::OnTap(move);
		if (!is_pressed) {
			[self dropFingerMovement:tap_position previous:prev_tap_position];
		}

		slime::App::OnMouseTap(tap_position.x, tap_position.y, is_pressed);
	}
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
	[self touchesMoved:touches withEvent:event];
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event {
	[self touchesMoved:touches withEvent:event];
}

@end

#endif // iOS
