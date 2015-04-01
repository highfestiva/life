/*
	Class:  CenterLayout
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#ifndef UICENTERLAYOUT_H
#define UICENTERLAYOUT_H

#include "UiLayout.h"

namespace UiTbc
{

class CenterLayout : public Layout
{
public:
	
	CenterLayout();
	virtual ~CenterLayout();

	virtual Type GetType() const;

	virtual void Add(Component* pComponent, int pParam1, int pParam2);
	virtual void Remove(Component* pComponent);
	virtual Component* Find(const str& pComponentName);
	virtual int GetNumComponents() const;

	virtual Component* GetFirst();
	virtual Component* GetNext();
	virtual Component* GetLast();
	virtual Component* GetPrev();

	virtual void UpdateLayout();

	virtual PixelCoord GetPreferredSize(bool pForceAdaptive);
	virtual PixelCoord GetMinSize() const;
	virtual PixelCoord GetContentSize() const;

protected:
private:

	Component* mChild;
};

} // End namespace.

#endif
