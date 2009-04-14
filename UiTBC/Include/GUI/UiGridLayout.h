/*
	Class:  GridLayout
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand
*/

#ifndef UIGRIDLAYOUT_H
#define UIGRIDLAYOUT_H

#include "UiLayout.h"

namespace UiTbc
{

class GridLayout : public Layout
{
public:
	
	GridLayout(int pRows, int pCols);
	virtual ~GridLayout();
	
	virtual Type GetType();

	virtual void Add(Component* pComponent, int pParam1, int pParam2);
	virtual void Remove(Component* pComponent);
	virtual Component* Find(const Lepra::String& pComponentName);
	virtual int GetNumComponents() const;

	virtual Component* GetComponentAt(int pRow, int pCol);

	virtual Component* GetFirst();
	virtual Component* GetNext();

	virtual Component* GetLast();
	virtual Component* GetPrev();

	virtual void UpdateLayout();

	virtual Lepra::PixelCoords GetPreferredSize(bool pForceAdaptive);
	virtual Lepra::PixelCoords GetMinSize();
	virtual Lepra::PixelCoords GetContentSize();

	inline int GetNumRows() const;
	inline int GetNumCols() const;

	void InsertRow(int pRow);
	void InsertColumn(int pColumn);
	void DeleteRow(int pRow);
	void DeleteColumn(int pColumn);

	// Debug function...
	bool HaveDoubles();
protected:
private:

	Component** AllocComponentGrid(int pRows, int pCols);

	Component** mComponent;

	int mNumRows;
	int mNumCols;
	int mCurrentIndex;
	int mNumComponents;
};

int GridLayout::GetNumRows() const
{
	return mNumRows;
}

int GridLayout::GetNumCols() const
{
	return mNumCols;
}

} // End namespace.

#endif
