/*
	Class:  Layout
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games

	NOTES:

	The base class of all layout classes. A layout class is responsible of 
	moving and resizing a bunch of components that share the same screen area.
*/

#ifndef UILAYOUT_H
#define UILAYOUT_H

#include "../UiTBC.h"
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

	virtual Type GetType() = 0;

	virtual void Add(Component* pComponent, int pParam1, int pParam2) = 0;
	virtual void Remove(Component* pComponent) = 0;
	virtual Component* Find(const str& pComponentName) = 0;
	virtual int GetNumComponents() const = 0;

	virtual Component* GetFirst() = 0;
	virtual Component* GetNext() = 0;

	virtual Component* GetLast() = 0;
	virtual Component* GetPrev() = 0;

	virtual void UpdateLayout() = 0;

	virtual PixelCoords GetPreferredSize(bool pForceAdaptive) = 0;
	virtual PixelCoords GetMinSize() = 0;
	virtual PixelCoords GetContentSize() = 0;

protected:
	inline Component* GetOwner();
private:
	inline void SetOwner(Component* pOwner);

	Component* mOwner;
};

Layout::Layout() :
	mOwner(0)
{
}

Layout::~Layout()
{
}

Component* Layout::GetOwner()
{
	return mOwner;
}

void Layout::SetOwner(Component* pOwner)
{
	mOwner = pOwner;
}

} // End namespace.

#endif
