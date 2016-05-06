/*
	Class:  GridLayout
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#include "pch.h"
#include "../../include/gui/uigridlayout.h"
#include "../../include/gui/uicomponent.h"
#include <list>

#include <math.h>

namespace uitbc {

GridLayout::GridLayout(int rows, int cols) :
	component_(0),
	num_rows_(rows),
	num_cols_(cols),
	current_index_(0),
	num_components_(0) {
	if (num_rows_ < 0) {
		num_rows_ = 0;
	}
	if (num_cols_ < 0) {
		num_cols_ = 0;
	}

	component_ = AllocComponentGrid(num_rows_, num_cols_);
}

GridLayout::~GridLayout() {
	delete[] component_;
	component_ = 0;
}

Component** GridLayout::AllocComponentGrid(int rows, int cols) {
	Component** _component = 0;

	if (rows != 0 && cols != 0) {
		_component = new Component*[rows * cols];
		for (int i = 0; i < rows * cols; i++) {
			_component[i] = 0;
		}
	}

	return _component;
}

Layout::Type GridLayout::GetType() const {
	return Layout::kGridlayout;
}

void GridLayout::Add(Component* component, int param1, int param2) {
	if (component_ == 0) {
		return;
	}

	// param1 = Row
	// param2 = Column
	int index = param2 + param1 * num_cols_;
	for (; index < num_rows_ * num_cols_; index++) {
		if (component_[index] == 0) {
			component_[index] = component;
			num_components_++;
			return;
		}
	}
}

void GridLayout::Remove(Component* component) {
	if (component_ == 0) {
		return;
	}

	for (int i = 0; i < num_rows_ * num_cols_; i++) {
		if (component_[i] == component) {
			component_[i] = 0;
			num_components_--;
		}
	}
}

int GridLayout::GetNumComponents() const {
	return num_components_;
}

Component* GridLayout::GetComponentAt(int row, int col) const {
	Component* comp = 0;
	if (component_ != 0 && row >= 0 && row < num_rows_ && col >= 0 && col < num_cols_) {
		comp = component_[row * num_cols_ + col];
	}
	return comp;
}

Component* GridLayout::GetFirst() {
	if (component_ == 0) {
		return 0;
	}

	for (current_index_ = 0; current_index_ < num_rows_ * num_cols_; current_index_++) {
		if (component_[current_index_] != 0) {
			return component_[current_index_];
		}
	}
	return 0;
}

Component* GridLayout::GetNext() {
	if (component_ == 0 || current_index_ < 0 || current_index_ >= num_rows_ * num_cols_) {
		return 0;
	}

	current_index_++;

	for (; current_index_ < num_rows_ * num_cols_; current_index_++) {
		if (component_[current_index_] != 0) {
			return component_[current_index_];
		}
	}
	return 0;
}

Component* GridLayout::GetLast() {
	if (component_ == 0) {
		return 0;
	}

	for (current_index_ = num_rows_ * num_cols_ - 1; current_index_ >= 0; current_index_--) {
		if (component_[current_index_] != 0) {
			return component_[current_index_];
		}
	}
	return 0;
}

Component* GridLayout::GetPrev() {
	if (component_ == 0 || current_index_ < 0 || current_index_ >= num_rows_ * num_cols_) {
		return 0;
	}

	current_index_--;

	for (; current_index_ >= 0; current_index_--) {
		if (component_[current_index_] != 0) {
			return component_[current_index_];
		}
	}
	return 0;
}

void GridLayout::UpdateLayout() {
	if (component_ == 0) {
		return;
	}

	PixelCoord owner_size(GetOwner()->GetSize());

	std::list<int> width_list;
	std::list<int> height_list;

	int x, y;

	int num_expandable_rows = 0;
	int num_expandable_cols = 0;

	int height_sum = 0;
	int width_sum = 0;

	// Find the maximum height of each row, and store it in the list.
	for (y = 0; y < num_rows_; y++) {
		int max_height = 0;
		bool expandable = true;
		bool empty_row = true;
		for (x = 0; x < num_cols_; x++) {
			int index = y * num_cols_ + x;

			if (component_[index] != 0 && component_[index]->IsLocalVisible() == true) {
				empty_row = false;

				PixelCoord size(component_[index]->GetPreferredSize());

				if (size.y > max_height) {
					max_height = size.y;
					expandable = false;
				}
			}
		}

		if (empty_row == true) {
			height_list.push_back(-1);
		} else {
			height_list.push_back(max_height);
		}

		height_sum += max_height;

		if (expandable == true && empty_row == false) {
			num_expandable_rows++;
		}
	}

	// Find the maximum width of each column, and store it in the list.
	for (x = 0; x < num_cols_; x++) {
		int max_width = 0;
		bool expandable = true;
		bool empty_col = true;
		for (y = 0; y < num_rows_; y++) {
			int index = y * num_cols_ + x;

			if (component_[index] != 0 && component_[index]->IsLocalVisible() == true) {
				empty_col = false;

				PixelCoord size(component_[index]->GetPreferredSize());

				if (size.x > max_width) {
					max_width = size.x;
					expandable = false;
				}
			}
		}

		if (empty_col == true) {
			width_list.push_back(-1);
		} else {
			width_list.push_back(max_width);
		}

		width_sum += max_width;

		if (expandable == true && empty_col == false) {
			num_expandable_cols++;
		}
	}

	double height_factor = 1.0;
	double width_factor = 1.0;

	if(height_sum > owner_size.y) {
		height_factor = (double)owner_size.y / (double)height_sum;
		height_sum = owner_size.y;
	} else if(num_expandable_rows == 0) {
		height_factor = (double)owner_size.y / (double)height_sum;
	}

	if(width_sum > owner_size.x) {
		width_factor = (double)owner_size.x / (double)width_sum;
		width_sum = owner_size.x;
	} else if(num_expandable_cols == 0) {
		width_factor = (double)owner_size.x / (double)width_sum;
	}

	double rest_height = 0;
	double rest_width  = 0;

	if (num_expandable_rows != 0) {
		rest_height = (double)(owner_size.y - height_sum) / (double)num_expandable_rows;
	}

	if (num_expandable_cols != 0) {
		rest_width  = (double)(owner_size.x - width_sum)  / (double)num_expandable_cols;
	}

	double pos_y = 0;
	double pos_x = 0;
	double diff = 0;

	std::list<int>::iterator iter;

	for (y = 0, iter = height_list.begin(); y < num_rows_; ++y, ++iter) {
		double height = (double)(*iter);

		if (height >= 0) {
			if (height == 0) {
				height = rest_height;
			} else {
				height *= height_factor;
			}

			height += diff;

			int pos_y_int = (int)floor(pos_y + 0.5);
			int size_y = (int)floor(pos_y + height + 0.5) - pos_y_int;

			diff = height - (double)size_y;

			// Set the height on the entire row.
			for (x = 0; x < num_cols_; x++) {
				int index = y * num_cols_ + x;

				if (component_[index] != 0 && component_[index]->IsLocalVisible() == true) {
					PixelCoord pos(component_[index]->GetPos());
					PixelCoord size(component_[index]->GetSize());
					pos.y  = pos_y_int;
					size.y = size_y;
					component_[index]->SetPos(pos);
					component_[index]->SetSize(size);
				}
			}

			pos_y += height;
		}
	}

	diff = 0;

	for (x = 0, iter = width_list.begin(); x < num_cols_; ++x, ++iter) {
		double width = (double)(*iter);

		if (width >= 0) {
			if (width == 0) {
				width = rest_width;
			} else {
				width *= width_factor;
			}

			width += diff;

			int pos_x_int = (int)floor(pos_x + 0.5);
			int size_x = (int)floor(pos_x + width + 0.5) - pos_x_int;

			diff = width - (double)size_x;

			// Set the width on the entire column.
			for (y = 0; y < num_rows_; y++) {
				int index = y * num_cols_ + x;

				if (component_[index] != 0 && component_[index]->IsLocalVisible() == true) {
					PixelCoord pos(component_[index]->GetPos());
					PixelCoord size(component_[index]->GetSize());
					pos.x  = pos_x_int;
					size.x = size_x;
					component_[index]->SetPos(pos);
					component_[index]->SetSize(size);
				}
			}

			pos_x += width;
		}
	}
}

PixelCoord GridLayout::GetPreferredSize(bool force_adaptive) {
	PixelCoord size(0, 0);

	int x, y;

	for (y = 0; y < num_rows_; y++) {
		int width_sum = 0;

		for (x = 0; x < num_cols_; x++) {
			int index = y * num_cols_ + x;

			if (component_[index] != 0) {
				width_sum += component_[index]->GetPreferredSize(force_adaptive).x;
			}
		}

		if (width_sum > size.x) {
			size.x = width_sum;
		}
	}

	for (x = 0; x < num_cols_; x++) {
		int height_sum = 0;

		for (y = 0; y < num_rows_; y++) {
			int index = y * num_cols_ + x;

			if (component_[index] != 0) {
				height_sum += component_[index]->GetPreferredSize(force_adaptive).y;
			}
		}

		if (height_sum > size.y) {
			size.y = height_sum;
		}
	}

	return size;
}

PixelCoord GridLayout::GetContentSize() const {
	return GetOwner()->GetSize();
}

PixelCoord GridLayout::GetMinSize() const {
	PixelCoord size(0, 0);

	int x, y;

	for (y = 0; y < num_rows_; y++) {
		int width_sum = 0;

		for (x = 0; x < num_cols_; x++) {
			int index = y * num_cols_ + x;

			if (component_[index] != 0) {
				width_sum += component_[index]->GetMinSize().x;
			}
		}

		if (width_sum > size.x) {
			size.x = width_sum;
		}
	}

	for (x = 0; x < num_cols_; x++) {
		int height_sum = 0;

		for (y = 0; y < num_rows_; y++) {
			int index = y * num_cols_ + x;

			if (component_[index] != 0) {
				height_sum += component_[index]->GetMinSize().y;
			}
		}

		if (height_sum > size.y) {
			size.y = height_sum;
		}
	}

	return size;
}

void GridLayout::InsertRow(int row) {
	if (row < 0) {
		row = 0;
	}

	if (row > num_rows_) {
		row = num_rows_;
	}

	Component** _component = AllocComponentGrid(num_rows_ + 1, num_cols_);

	int i;

	// Copy all rows before row.
	for (i = 0; i < row; i++) {
		int row_index = i * num_cols_;
		for (int j = 0; j < num_cols_; j++) {
			_component[row_index + j] = component_[row_index + j];
		}
	}

	// Copy all rows after row.
	for (i = row; i < num_rows_; i++) {
		int row_index_src = i * num_cols_;
		int row_index_dst = (i + 1) * num_cols_;
		for (int j = 0; j < num_cols_; j++) {
			_component[row_index_dst + j] = component_[row_index_src + j];
		}
	}

	delete[] component_;
	component_ = _component;
	num_rows_++;
}

void GridLayout::InsertColumn(int column) {
	if (column < 0) {
		column = 0;
	}

	if (column > num_cols_) {
		column = num_cols_;
	}

	Component** _component = AllocComponentGrid(num_rows_, num_cols_ + 1);

	for (int j = 0; j < num_rows_; j++) {
		int i;
		int row_index = j * num_cols_;

		// Copy all columns before column.
		for (i = 0; i < column; i++) {
			_component[row_index + i] = component_[row_index + i];
		}

		// Copy all columns after column.
		for (i = column; i < num_cols_; i++) {
			_component[row_index + i + 1] = component_[row_index + i];
		}
	}

	delete[] component_;
	component_ = _component;
	num_cols_++;
}

void GridLayout::DeleteRow(int row) {
	bool ok = (row >= 0 && row < num_rows_);
	if (ok) {
		Component** _component = AllocComponentGrid(num_rows_ - 1, num_cols_);

		int i;

		// Copy all rows before row.
		for (i = 0; i < row; i++) {
			int row_index = i * num_cols_;
			for (int j = 0; j < num_cols_; j++) {
				_component[row_index + j] = component_[row_index + j];
			}
		}

		// Copy all rows after row.
		for (i = row + 1; i < num_rows_; i++) {
			int row_index_src = i * num_cols_;
			int row_index_dst = (i - 1) * num_cols_;
			for (int j = 0; j < num_cols_; j++) {
				_component[row_index_dst + j] = component_[row_index_src + j];
			}
		}

		delete[] component_;
		component_ = _component;
		num_rows_--;
	}
}

void GridLayout::DeleteColumn(int column) {
	bool ok = (column >= 0 && column < num_cols_);

	if (ok) {
		Component** _component = AllocComponentGrid(num_rows_, num_cols_ - 1);

		for (int j = 0; j < num_rows_; j++) {
			int i;
			int row_index = j * num_cols_;

			// Copy all columns before column.
			for (i = 0; i < column; i++) {
				_component[row_index + i] = component_[row_index + i];
			}

			// Copy all columns after column.
			for (i = column + 1; i < num_cols_; i++) {
				_component[row_index + i - 1] = component_[row_index + i];
			}
		}

		delete[] component_;
		component_ = _component;
		num_cols_--;
	}
}

bool GridLayout::HaveDoubles() const {
	for (int i = 0; i < num_rows_ * num_cols_; i++) {
		for (int j = i + 1; j < num_rows_ * num_cols_; j++) {
			if (component_[i] == component_[j]) {
				return true;
			}
		}
	}

	return false;
}


}
