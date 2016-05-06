
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../../../lepra/include/lepraos.h"
#include "../../../lepra/include/canvas.h"
#include "../uitouchdrag.h"
#ifdef LEPRA_IOS



namespace uilepra {
namespace touch {



class TouchHandler {
public:
static void HandleTouches(NSSet* touches, const Canvas* canvas, DragManager& drag_manager) {
	NSEnumerator* e = [touches objectEnumerator];
	UITouch* touch;
	while ((touch = (UITouch*)[e nextObject])) {
		CGPoint tap_position = [touch locationInView:nil];
		CGPoint prev_tap_position = [touch previousLocationInView:nil];
		bool is_pressed = (touch.phase != UITouchPhaseEnded && touch.phase != UITouchPhaseCancelled);
		const int s = [[UIScreen mainScreen] scale];
		PixelCoord previous_tap;
		PixelCoord this_tap;
		static const bool is_io_s7 = ([[[UIDevice currentDevice] systemVersion] floatValue] < 8.0);
		const int rotation = is_io_s7? (canvas->GetDeviceOutputRotation()+90)%360 : canvas->GetOutputRotation();
		switch (rotation) {
			case 0: {
				previous_tap = PixelCoord(prev_tap_position.x*s, prev_tap_position.y*s);
				this_tap = PixelCoord(tap_position.x*s, tap_position.y*s);
			} break;
			case 90: {
				previous_tap = PixelCoord(prev_tap_position.y*s, canvas->GetActualHeight() - prev_tap_position.x*s);
				this_tap = PixelCoord(tap_position.y*s, canvas->GetActualHeight() - tap_position.x*s);
			} break;
			case 180: {
				previous_tap = PixelCoord(canvas->GetActualWidth() - prev_tap_position.x*s, canvas->GetActualHeight() - prev_tap_position.y*s);
				this_tap = PixelCoord(canvas->GetActualWidth() - tap_position.x*s, canvas->GetActualHeight() - tap_position.y*s);
			} break;
			default: {
				previous_tap = PixelCoord(canvas->GetActualWidth() - prev_tap_position.y*s, prev_tap_position.x*s);
				this_tap = PixelCoord(canvas->GetActualWidth() - tap_position.y*s, tap_position.x*s);
			} break;
		}
		drag_manager.UpdateDrag(previous_tap, this_tap, is_pressed, is_pressed? 1 : 0);
	}
}
};



}
}

#endif // iOS
