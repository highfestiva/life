
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../Lepra/Include/LepraTarget.h"



namespace Lepra
{
class Canvas;
}
using namespace Lepra;



#ifdef LEPRA_IOS
@interface AnimatedApp: UIResponder
{
@private
	Canvas* canvas;
	NSTimer* animationTimer;
}

-(id) init:(Canvas*)pCanvas;
-(void) startTick;
-(void) stopTick;
-(void) tick;
@end
#endif // iOS



#include "App.cxx"



#ifdef LEPRA_IOS

#import "../UiLepra/Include/Mac/EAGLView.h"

@implementation AnimatedApp
-(id) init:(Canvas*)pCanvas
{
	[UIApplication sharedApplication].statusBarOrientation = UIInterfaceOrientationLandscapeRight;
	canvas = pCanvas;
	return self;
}

-(void) startTick
{
	animationTimer = [NSTimer scheduledTimerWithTimeInterval:0.0225 target:self selector:@selector(tick) userInfo:nil repeats:YES];
	[EAGLView sharedView].responder = self;
	[[EAGLView sharedView] powerUpAcc];
}

-(void) stopTick
{
	[[EAGLView sharedView] powerDownAcc];
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
		lGlView.canvas = canvas;
		Slime::App::PollApp();
	}
}

-(Slime::FingerMovement&) getFingerMovement:(const CGPoint&)pLocation previous:(const CGPoint&)pPrevious
{
	Slime::FingerMoveList::iterator i = Slime::gFingerMoveList.begin();
	for (; i != Slime::gFingerMoveList.end(); ++i)
	{
		if (i->Update(pPrevious.x, pPrevious.y, pLocation.x, pLocation.y))
		{
			return *i;
		}
	}
	Slime::gFingerMoveList.push_back(Slime::FingerMovement(pLocation.x, pLocation.y));
	return Slime::gFingerMoveList.back();
}

-(void) dropFingerMovement:(const CGPoint&)pLocation previous:(const CGPoint&)pPrevious
{
	Slime::FingerMoveList::iterator i = Slime::gFingerMoveList.begin();
	for (; i != Slime::gFingerMoveList.end(); ++i)
	{
		if (i->Update(pPrevious.x, pPrevious.y, pLocation.x, pLocation.y))
		{
			Slime::gFingerMoveList.erase(i);
			return;
		}
	}
	// Oops! This happens, but not sure why.
	Slime::gFingerMoveList.clear();
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

- (void)touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event
{
	NSEnumerator* e = [touches objectEnumerator];
	UITouch* lTouch;
	while ((lTouch = (UITouch*)[e nextObject]))
	{
		CGPoint lTapPosition = [self xform:[lTouch locationInView:nil]];
		CGPoint lPrevTapPosition = [self xform:[lTouch previousLocationInView:nil]];
		bool lIsPressed = (lTouch.phase != UITouchPhaseEnded && lTouch.phase != UITouchPhaseEnded);
		Slime::FingerMovement& lMove = [self getFingerMovement:lTapPosition previous:lPrevTapPosition];
		lMove.mIsPress = lIsPressed;
		Slime::App::OnTap(lMove);
		if (!lIsPressed)
		{
			[self dropFingerMovement:lTapPosition previous:lPrevTapPosition];
		}

		Slime::App::OnMouseTap(lTapPosition.x, lTapPosition.y, lIsPressed);
	}
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	[self touchesMoved:touches withEvent:event];
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
	[self touchesMoved:touches withEvent:event];
}

@end

#endif // iOS
