/*
	Class:  FloatingLayout
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games
*/

#ifndef UIFLOATINGLAYOUT_H
#define UIFLOATINGLAYOUT_H

#include "UiComponent.h"
#include "UiLayout.h"
#include <list>

namespace UiTbc
{

class FloatingLayout : public Layout
{
public:
	FloatingLayout();
	virtual ~FloatingLayout();

	virtual Type GetType();

	virtual void Add(Component* pComponent, int pParam1, int pParam2);
	virtual void Remove(Component* pComponent);
	virtual Component* Find(const Lepra::String& pComponentName);
	virtual int GetNumComponents() const;

	virtual Component* GetFirst();
	virtual Component* GetNext();

	virtual Component* GetLast();
	virtual Component* GetPrev();

	virtual void UpdateLayout();

	virtual Lepra::PixelCoords GetPreferredSize(bool pForceAdaptive);
	virtual Lepra::PixelCoords GetMinSize();
	virtual Lepra::PixelCoords GetContentSize();

	// Functions that are unique to the floating layout.
	void MoveToTop(Component* pComponent);

protected:
private:

	enum
	{
		POSOFFSET = 20
	};

	typedef std::list<Component*> ComponentList;

	ComponentList mChildList;
	ComponentList::iterator mIter;

	int mX;
	int mY;
	int mPosCount;
};

} // End namespace.

#endif