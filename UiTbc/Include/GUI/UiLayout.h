/*
	Class:  Layout
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

	NOTES:

	The base class of all layout classes. A layout class is responsible of 
	moving and resizing a bunch of components that share the same screen area.
*/



#pragma once

#include "../UiTbc.h"
#include "../../../Lepra/Include/String.h"
#include "../../../Lepra/Include/Graphics2D.h"



namespace UiTbc
{



class Component;



class Layout
{
public:
	
	friend class Component;

	enum Type
	{
		LAYOUT = 0,
		GRIDLAYOUT,
		CENTERLAYOUT,
		STACKLAYOUT,
		FLOATINGLAYOUT,
		LISTLAYOUT,
	};

	inline Layout();
	inline virtual ~Layout();

	virtual Type GetType() const = 0;

	virtual void Add(Component* pComponent, int pParam1, int pParam2) = 0;
	virtual void Remove(Component* pComponent) = 0;
	virtual int GetNumComponents() const = 0;

	virtual Component* GetFirst() = 0;
	virtual Component* GetNext() = 0;
	virtual Component* GetLast() = 0;
	virtual Component* GetPrev() = 0;

	virtual void UpdateLayout() = 0;

	virtual PixelCoord GetPreferredSize(bool pForceAdaptive) = 0;
	virtual PixelCoord GetMinSize() const = 0;
	virtual PixelCoord GetContentSize() const = 0;

protected:
	inline Component* GetOwner() const;

private:
	inline void SetOwner(Component* pOwner);

	Component* mOwner;
};



Layout::Layout():
	mOwner(0)
{
}

Layout::~Layout()
{
}

Component* Layout::GetOwner() const
{
	return mOwner;
}

void Layout::SetOwner(Component* pOwner)
{
	mOwner = pOwner;
}



}
