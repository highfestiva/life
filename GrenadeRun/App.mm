
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



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
-(void) dropFingerMovements;
@end
#endif // iOS



#include "App.cxx"



#ifdef LEPRA_IOS

#import "../UiLepra/Include/Mac/EAGLView.h"

@implementation AnimatedApp
-(id) init:(Canvas*)pCanvas
{
	canvas = pCanvas;
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
		GrenadeRun::App::PollApp();
		[self dropFingerMovements];
	}
}

-(GrenadeRun::FingerMovement&) getFingerMovement:(const CGPoint&)pLocation previous:(const CGPoint&)pPrevious
{
	GrenadeRun::FingerMoveList::iterator i = GrenadeRun::gFingerMoveList.begin();
	for (; i != GrenadeRun::gFingerMoveList.end(); ++i)
	{
		NSLog(@"get: (%i; %i) ==? (%i; %i)", (int)i->mLastX, (int)i->mLastY, (int)pLocation.x, (int)pLocation.y);
		if (i->Update(pPrevious.x, pPrevious.y, pLocation.x, pLocation.y))
		{
			NSLog(@"get: Match!");
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

- (void)touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event
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
