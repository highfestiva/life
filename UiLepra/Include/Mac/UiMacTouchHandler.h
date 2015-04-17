
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../../Lepra/Include/LepraOS.h"
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
		PixelCoord lPreviousTap;
		PixelCoord lThisTap;
		switch (pCanvas->GetOutputRotation())
		{
			case 0:
			{
				lPreviousTap = PixelCoord(lPrevTapPosition.x*s, lPrevTapPosition.y*s);
				lThisTap = PixelCoord(lTapPosition.x*s, lTapPosition.y*s);
			}
			break;
			case 90:
			{
				lPreviousTap = PixelCoord(lPrevTapPosition.y*s, pCanvas->GetActualHeight() - lPrevTapPosition.x*s);
				lThisTap = PixelCoord(lTapPosition.y*s, pCanvas->GetActualHeight() - lTapPosition.x*s);
			}
			break;
			case 180:
			{
				lPreviousTap = PixelCoord(pCanvas->GetActualWidth() - lPrevTapPosition.x*s, pCanvas->GetActualHeight() - lPrevTapPosition.y*s);
				lThisTap = PixelCoord(pCanvas->GetActualWidth() - lTapPosition.x*s, pCanvas->GetActualHeight() - lTapPosition.y*s);
			}
			break;
			default:
			{
				lPreviousTap = PixelCoord(pCanvas->GetActualWidth() - lPrevTapPosition.y*s, lPrevTapPosition.x*s);
				lThisTap = PixelCoord(pCanvas->GetActualWidth() - lTapPosition.y*s, lTapPosition.x*s);
			}
			break;
		}
		pDragManager.UpdateDrag(lPreviousTap, lThisTap, lIsPressed, lIsPressed? 1 : 0);
	}
}
};



}
}

#endif // iOS
