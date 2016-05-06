/*
	Class:  FloatingLayout
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#pragma once

#include "uicomponent.h"
#include "uilayout.h"
#include <list>

namespace uitbc {

class FloatingLayout : public Layout {
public:
	FloatingLayout();
	virtual ~FloatingLayout();

	virtual Type GetType() const;

	virtual void Add(Component* component, int param1, int param2);
	virtual void Remove(Component* component);
	virtual int GetNumComponents() const;

	virtual Component* GetFirst();
	virtual Component* GetNext();
	virtual Component* GetLast();
	virtual Component* GetPrev();

	virtual void UpdateLayout();

	virtual PixelCoord GetPreferredSize(bool force_adaptive);
	virtual PixelCoord GetMinSize() const;
	virtual PixelCoord GetContentSize() const;

	// Functions that are unique to the floating layout.
	void MoveToTop(Component* component);

protected:
private:

	enum {
		kPosoffset = 20
	};

	typedef std::list<Component*> ComponentList;

	ComponentList child_list_;
	ComponentList::iterator iter_;

	int x_;
	int y_;
	int pos_count_;
};

}
