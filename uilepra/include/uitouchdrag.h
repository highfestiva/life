
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include <list>
#include "../../lepra/include/graphics2d.h"
#include "uilepra.h"



namespace lepra {
class Lock;
}


namespace uilepra {



class InputManager;



namespace touch {



using namespace lepra;



enum DragFlags {
	kDraggingStick	= (1<<0),
	kDraggingUi	= (1<<1),
	kDraggingOther	= 0xFFFF,
	kDragUser	= (1<<16)
};

struct Drag {
	Drag(int x, int y, bool isPress, int button_mask);
	void Update(const PixelCoord& coord, bool is_press, int button_mask);
	int GetDiamondDistanceTo(const PixelCoord& coord) const;

	PixelCoord start_;
	PixelCoord last_;
	bool is_press_;
	bool is_new_;
	int button_mask_;
	uint32 flags_;
	intptr_t extra_;
};



class DragManager {
public:
	typedef std::vector<Drag> DragList;

	DragManager();
	virtual ~DragManager();

	void SetMaxDragDistance(int max_drag_distance);
	void UpdateDrag(const PixelCoord& previous, const PixelCoord& location, bool is_pressed, int button_mask);
	void UpdateDragByMouse(const InputManager* input_manager);
	void UpdateMouseByDrag(InputManager* input_manager);
	bool UpdateTouchsticks(InputManager* input_manager) const;
	void SetDraggingUi(bool is_ui);
	void DropReleasedDrags();
	void ClearDrags(InputManager* input_manager);
	DragList GetDragList();
	void SetDragList(const DragList& drag_list);

private:
	Lock* lock_;
	DragList drag_list_;
	PixelCoord last_mouse_;
	bool mouse_last_pressed_;
	int max_drag_diamond_distance_;

	logclass();
};



}
}
