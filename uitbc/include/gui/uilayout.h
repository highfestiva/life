/*
	Class:  Layout
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

	NOTES:

	The base class of all layout classes. A layout class is responsible of
	moving and resizing a bunch of components that share the same screen area.
*/



#pragma once

#include "../uitbc.h"
#include "../../../lepra/include/string.h"
#include "../../../lepra/include/graphics2d.h"



namespace uitbc {



class Component;



class Layout {
public:

	friend class Component;

	enum Type {
		kLayout = 0,
		kGridlayout,
		kCenterlayout,
		kStacklayout,
		kFloatinglayout,
		kListlayout,
	};

	inline Layout();
	inline virtual ~Layout();

	virtual Type GetType() const = 0;

	virtual void Add(Component* component, int param1, int param2) = 0;
	virtual void Remove(Component* component) = 0;
	virtual int GetNumComponents() const = 0;

	virtual Component* GetFirst() = 0;
	virtual Component* GetNext() = 0;
	virtual Component* GetLast() = 0;
	virtual Component* GetPrev() = 0;

	virtual void UpdateLayout() = 0;

	virtual PixelCoord GetPreferredSize(bool force_adaptive) = 0;
	virtual PixelCoord GetMinSize() const = 0;
	virtual PixelCoord GetContentSize() const = 0;

protected:
	inline Component* GetOwner() const;

private:
	inline void SetOwner(Component* owner);

	Component* owner_;
};



Layout::Layout():
	owner_(0) {
}

Layout::~Layout() {
}

Component* Layout::GetOwner() const {
	return owner_;
}

void Layout::SetOwner(Component* owner) {
	owner_ = owner;
}



}
