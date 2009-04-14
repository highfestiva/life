/*
	Class:  CenterLayout
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand
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

protected:
private:

	Component* mChild;
};

} // End namespace.

#endif
