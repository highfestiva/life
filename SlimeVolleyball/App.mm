
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Lepra/Include/LepraTarget.h"




#ifdef LEPRA_IOS
@interface AnimatedApp: UIResponder
{
@private
	NSTimer* animationTimer;
}

-(void) startTick;
-(void) stopTick;
-(void) tick;
@end
#endif // iOS



#include "App.cxx"



#ifdef LEPRA_IOS

#import "../UiLepra/Include/Mac/EAGLView.h"

@implementation AnimatedApp
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
		Slime::App::PollApp();
	}
}

-(Slime::FingerMovement&) getFingerMovement:(UITouch*)touch
{
	CGPoint lThis = [touch locationInView:nil];
	CGPoint lPrevious = [touch previousLocationInView:nil];
	Slime::FingerMoveList::iterator i = Slime::gFingerMoveList.begin();
	for (; i != Slime::gFingerMoveList.end(); ++i)
	{
		if (i->Update(lPrevious.x, lPrevious.y, lThis.x, lThis.y))
		{
			return *i;
		}
	}
	Slime::gFingerMoveList.push_back(Slime::FingerMovement(lThis.x, lThis.y));
	return Slime::gFingerMoveList.back();
}

-(void) dropFingerMovement:(UITouch*)touch
{
	CGPoint lThis = [touch locationInView:nil];
	CGPoint lPrevious = [touch previousLocationInView:nil];
	Slime::FingerMoveList::iterator i = Slime::gFingerMoveList.begin();
	for (; i != Slime::gFingerMoveList.end(); ++i)
	{
		if (i->Update(lPrevious.x, lPrevious.y, lThis.x, lThis.y))
		{
			Slime::gFingerMoveList.erase(i);
			return;
		}
	}
	// Oops! This happens, but not sure why.
	Slime::gFingerMoveList.clear();
}

- (void)touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event
{
	NSEnumerator* e = [touches objectEnumerator];
	UITouch* lTouch;
	while ((lTouch = (UITouch*)[e nextObject]))
	{
		CGPoint lTapPosition = [lTouch locationInView:nil];
		bool lIsPressed = (lTouch.phase != UITouchPhaseEnded && lTouch.phase != UITouchPhaseEnded);
		Slime::FingerMovement& lMove = [self getFingerMovement:lTouch];
		lMove.mIsPress = lIsPressed;
		Slime::App::OnTap(lMove);
		if (!lIsPressed)
		{
			[self dropFingerMovement:lTouch];
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
