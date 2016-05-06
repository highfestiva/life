/*
	Class:  GridLayout
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#pragma once

#include "uilayout.h"

namespace uitbc {

class GridLayout : public Layout {
public:

	GridLayout(int rows, int cols);
	virtual ~GridLayout();

	virtual Type GetType() const;

	virtual void Add(Component* component, int param1, int param2);
	virtual void Remove(Component* component);
	virtual int GetNumComponents() const;

	virtual Component* GetComponentAt(int row, int col) const;

	virtual Component* GetFirst();
	virtual Component* GetNext();
	virtual Component* GetLast();
	virtual Component* GetPrev();

	virtual void UpdateLayout();

	virtual PixelCoord GetPreferredSize(bool force_adaptive);
	virtual PixelCoord GetMinSize() const;
	virtual PixelCoord GetContentSize() const;

	inline int GetNumRows() const;
	inline int GetNumCols() const;

	void InsertRow(int row);
	void InsertColumn(int column);
	void DeleteRow(int row);
	void DeleteColumn(int column);

	// Debug function...
	bool HaveDoubles() const;
protected:
private:

	Component** AllocComponentGrid(int rows, int cols);

	Component** component_;

	int num_rows_;
	int num_cols_;
	int current_index_;
	int num_components_;
};

int GridLayout::GetNumRows() const {
	return num_rows_;
}

int GridLayout::GetNumCols() const {
	return num_cols_;
}

}
