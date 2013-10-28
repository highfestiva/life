
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../../Lepra/Include/LepraTarget.h"
#include "../../Lepra/Include/Canvas.h"
#include "../UiTouchDrag.h"
#ifdef LEPRA_IOS



namespace UiLepra
{
namespace Touch
{



class TouchHandler
{
public:
static void HandleTouches(NSSet* pTouches, const Canvas* pCanvas, DragManager& pDragManager)
{
	NSEnumerator* e = [pTouches objectEnumerator];
	UITouch* lTouch;
	while ((lTouch = (UITouch*)[e nextObject]))
	{
		CGPoint lTapPosition = [lTouch locationInView:nil];
		CGPoint lPrevTapPosition = [lTouch previousLocationInView:nil];
		bool lIsPressed = (lTouch.phase != UITouchPhaseEnded && lTouch.phase != UITouchPhaseCancelled);
		const int s = [[UIScreen mainScreen] scale];
		//NSLog(@"Touch: %f, %f", lTapPosition.x*s, lTapPosition.y*s);
		PixelCoord lPreviousTap(lPrevTapPosition.y*s, pCanvas->GetActualHeight() - lPrevTapPosition.x*s);
		PixelCoord lThisTap(lTapPosition.y*s, pCanvas->GetActualHeight() - lTapPosition.x*s);
		if (pCanvas && pCanvas->GetDeviceOutputRotation() != 0)
		{
			lPreviousTap = PixelCoord(pCanvas->GetActualWidth() - lPrevTapPosition.y*s, lPrevTapPosition.x*s);
			lThisTap = PixelCoord(pCanvas->GetActualWidth() - lTapPosition.y*s, lTapPosition.x*s);
		}
		pDragManager.UpdateDrag(lPreviousTap, lThisTap, lIsPressed);
	}
}
};



}
}

#endif // iOS
