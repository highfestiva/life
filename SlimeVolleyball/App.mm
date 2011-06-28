
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

- (void)touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event
{
	NSEnumerator* e = [touches objectEnumerator];
	int lLowest = -1;
	UITouch* lTouch;
	while ((lTouch = (UITouch*)[e nextObject]))
	{
		CGPoint lTapPosition = [lTouch locationInView:nil];
		if (lLowest < lTapPosition.y)
		{
			lLowest = lTapPosition.y;
		}
	}
	e = [touches objectEnumerator];
	while ((lTouch = (UITouch*)[e nextObject]))
	{
		CGPoint lTapPosition = [lTouch locationInView:nil];
		Slime::App::OnTap(lTapPosition.x, lTapPosition.y, (lTapPosition.y == lLowest));
	}
}

@end

#endif // iOS
