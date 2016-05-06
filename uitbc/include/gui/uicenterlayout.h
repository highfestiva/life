/*
	Class:  CenterLayout
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#pragma once

#include "uilayout.h"

namespace uitbc {

class CenterLayout : public Layout {
public:

	CenterLayout();
	virtual ~CenterLayout();

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

protected:
private:

	Component* child_;
};

}
