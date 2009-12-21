/*
	Class:  CenterLayout
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games
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

	virtual Type GetType();

	virtual void Add(Component* pComponent, int pParam1, int pParam2);
	virtual void Remove(Component* pComponent);
	virtual Component* Find(const str& pComponentName);
	virtual int GetNumComponents() const;

	virtual Component* GetFirst();
	virtual Component* GetNext();

	virtual Component* GetLast();
	virtual Component* GetPrev();

	virtual void UpdateLayout();

	virtual PixelCoords GetPreferredSize(bool pForceAdaptive);
	virtual PixelCoords GetMinSize();
	virtual PixelCoords GetContentSize();

protected:
private:

	Component* mChild;
};

} // End namespace.

#endif
