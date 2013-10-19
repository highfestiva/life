
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
		//NSLog(@"Touch: %f, %f", lTapPosition.x, lTapPosition.y);
		PixelCoord lPreviousTap(lPrevTapPosition.y, pCanvas->GetActualHeight() - lPrevTapPosition.x);
		PixelCoord lThisTap(lTapPosition.y, pCanvas->GetActualHeight() - lTapPosition.x);
		if (pCanvas && pCanvas->GetDeviceOutputRotation() != 0)
		{
			lPreviousTap = PixelCoord(pCanvas->GetActualWidth() - lPrevTapPosition.y, lPrevTapPosition.x);
			lThisTap = PixelCoord(pCanvas->GetActualWidth() - lTapPosition.y, lTapPosition.x);
		}
		pDragManager.UpdateDrag(lPreviousTap, lThisTap, lIsPressed);
	}
}
};



}
}

#endif // iOS
