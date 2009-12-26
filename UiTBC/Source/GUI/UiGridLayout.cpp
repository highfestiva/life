/*
	Class:  GridLayout
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games
*/

#include "../../Include/GUI/UiGridLayout.h"
#include "../../Include/GUI/UiComponent.h"
#include <list>

#include <math.h>

namespace UiTbc
{

GridLayout::GridLayout(int pRows, int pCols) :
	mComponent(0),
	mNumRows(pRows),
	mNumCols(pCols),
	mCurrentIndex(0),
	mNumComponents(0)
{
	if (mNumRows < 0)
	{
		mNumRows = 0;
	}
	if (mNumCols < 0)
	{
		mNumCols = 0;
	}

	mComponent = AllocComponentGrid(mNumRows, mNumCols);
}

GridLayout::~GridLayout()
{
	if (mComponent != 0)
	{
		delete[] mComponent;
	}
}

Component** GridLayout::AllocComponentGrid(int pRows, int pCols)
{
	Component** lComponent = 0;

	if (pRows != 0 && pCols != 0)
	{
		lComponent = new Component*[pRows * pCols];
		for (int i = 0; i < pRows * pCols; i++)
		{
			lComponent[i] = 0;
		}
	}

	return lComponent;
}

Layout::Type GridLayout::GetType()
{
	return Layout::GRIDLAYOUT;
}

void GridLayout::Add(Component* pComponent, int pParam1, int pParam2)
{
	if (mComponent == 0)
	{
		return;
	}

	// pParam1 = Row
	// pParam2 = Column
	int lIndex = pParam2 + pParam1 * mNumCols;
	for (; lIndex < mNumRows * mNumCols; lIndex++)
	{
		if (mComponent[lIndex] == 0)
		{
			mComponent[lIndex] = pComponent;
			mNumComponents++;
			return;
		}
	}
}

void GridLayout::Remove(Component* pComponent)
{
	if (mComponent == 0)
	{
		return;
	}

	for (int i = 0; i < mNumRows * mNumCols; i++)
	{
		if (mComponent[i] == pComponent)
		{
			mComponent[i] = 0;
			mNumComponents--;
		}
	}
}

Component* GridLayout::Find(const str& pComponentName)
{
	if (mComponent == 0)
	{
		return 0;
	}

	for (int i = 0; i < mNumRows * mNumCols; i++)
	{
		if (mComponent[i] != 0 && mComponent[i]->GetName() == pComponentName)
		{
			return mComponent[i];
		}
	}

	return 0;
}

int GridLayout::GetNumComponents() const
{
	return mNumComponents;
}

Component* GridLayout::GetComponentAt(int pRow, int pCol)
{
	Component* lComp = 0;
	if (mComponent != 0 && pRow >= 0 && pRow < mNumRows && pCol >= 0 && pCol < mNumCols)
	{
		lComp = mComponent[pRow * mNumCols + pCol];
	}
	return lComp;
}

Component* GridLayout::GetFirst()
{
	if (mComponent == 0)
	{
		return 0;
	}

	for (mCurrentIndex = 0; mCurrentIndex < mNumRows * mNumCols; mCurrentIndex++)
	{
		if (mComponent[mCurrentIndex] != 0)
		{
			return mComponent[mCurrentIndex];
		}
	}
	return 0;
}

Component* GridLayout::GetNext()
{
	if (mComponent == 0 || mCurrentIndex < 0 || mCurrentIndex >= mNumRows * mNumCols)
	{
		return 0;
	}

	mCurrentIndex++;

	for (; mCurrentIndex < mNumRows * mNumCols; mCurrentIndex++)
	{
		if (mComponent[mCurrentIndex] != 0)
		{
			return mComponent[mCurrentIndex];
		}
	}
	return 0;
}

Component* GridLayout::GetLast()
{
	if (mComponent == 0)
	{
		return 0;
	}

	for (mCurrentIndex = mNumRows * mNumCols - 1; mCurrentIndex >= 0; mCurrentIndex--)
	{
		if (mComponent[mCurrentIndex] != 0)
		{
			return mComponent[mCurrentIndex];
		}
	}
	return 0;
}

Component* GridLayout::GetPrev()
{
	if (mComponent == 0 || mCurrentIndex < 0 || mCurrentIndex >= mNumRows * mNumCols)
	{
		return 0;
	}

	mCurrentIndex--;

	for (; mCurrentIndex >= 0; mCurrentIndex--)
	{
		if (mComponent[mCurrentIndex] != 0)
		{
			return mComponent[mCurrentIndex];
		}
	}
	return 0;
}

void GridLayout::UpdateLayout()
{
	if (mComponent == 0)
	{
		return;
	}

	PixelCoords lOwnerSize(GetOwner()->GetSize());

	std::list<int> lWidthList;
	std::list<int> lHeightList;

	int x, y;

	int lNumExpandableRows = 0;
	int lNumExpandableCols = 0;
	
	int lHeightSum = 0;
	int lWidthSum = 0;

	// Find the maximum height of each row, and store it in the list.
	for (y = 0; y < mNumRows; y++)
	{
		int lMaxHeight = 0;
		bool lExpandable = true;
		bool lEmptyRow = true;
		for (x = 0; x < mNumCols; x++)
		{
			int lIndex = y * mNumCols + x;
				
			if (mComponent[lIndex] != 0 && mComponent[lIndex]->IsVisible() == true)
			{
				lEmptyRow = false;

				PixelCoords lSize(mComponent[lIndex]->GetPreferredSize());

				if (lSize.y > lMaxHeight)
				{
					lMaxHeight = lSize.y;
					lExpandable = false;
				}
			}
		}

		if (lEmptyRow == true)
		{
			lHeightList.push_back(-1);
		}
		else
		{
			lHeightList.push_back(lMaxHeight);
		}

		lHeightSum += lMaxHeight;

		if (lExpandable == true && lEmptyRow == false)
		{
			lNumExpandableRows++;
		}
	}

	// Find the maximum width of each column, and store it in the list.
	for (x = 0; x < mNumCols; x++)
	{
		int lMaxWidth = 0;
		bool lExpandable = true;
		bool lEmptyCol = true;
		for (y = 0; y < mNumRows; y++)
		{
			int lIndex = y * mNumCols + x;
				
			if (mComponent[lIndex] != 0 && mComponent[lIndex]->IsVisible() == true)
			{
				lEmptyCol = false;

				PixelCoords lSize(mComponent[lIndex]->GetPreferredSize());

				if (lSize.x > lMaxWidth)
				{
					lMaxWidth = lSize.x;
					lExpandable = false;
				}
			}
		}

		if (lEmptyCol == true)
		{
			lWidthList.push_back(-1);
		}
		else
		{
			lWidthList.push_back(lMaxWidth);
		}

		lWidthSum += lMaxWidth;

		if (lExpandable == true && lEmptyCol == false)
		{
			lNumExpandableCols++;
		}
	}

	double lHeightFactor = 1.0;
	double lWidthFactor = 1.0;

	if(lHeightSum > lOwnerSize.y)
	{
		lHeightFactor = (double)lOwnerSize.y / (double)lHeightSum;
		lHeightSum = lOwnerSize.y;
	}
	else if(lNumExpandableRows == 0)
	{
		lHeightFactor = (double)lOwnerSize.y / (double)lHeightSum;
	}

	if(lWidthSum > lOwnerSize.x)
	{
		lWidthFactor = (double)lOwnerSize.x / (double)lWidthSum;
		lWidthSum = lOwnerSize.x;
	}
	else if(lNumExpandableCols == 0)
	{
		lWidthFactor = (double)lOwnerSize.x / (double)lWidthSum;
	}

	double lRestHeight = 0;
	double lRestWidth  = 0;

	if (lNumExpandableRows != 0)
	{
		lRestHeight = (double)(lOwnerSize.y - lHeightSum) / (double)lNumExpandableRows;
	}

	if (lNumExpandableCols != 0)
	{
		lRestWidth  = (double)(lOwnerSize.x - lWidthSum)  / (double)lNumExpandableCols;
	}

	double lPosY = 0;
	double lPosX = 0;
	double lDiff = 0;

	std::list<int>::iterator lIter;

	for (y = 0, lIter = lHeightList.begin(); y < mNumRows; ++y, ++lIter)
	{
		double lHeight = (double)(*lIter);

		if (lHeight >= 0)
		{
			if (lHeight == 0)
			{
				lHeight = lRestHeight;
			}
			else
			{
				lHeight *= lHeightFactor;
			}

			lHeight += lDiff;

			int lPosYInt = (int)floor(lPosY + 0.5);
			int lSizeY = (int)floor(lPosY + lHeight + 0.5) - lPosYInt;

			lDiff = lHeight - (double)lSizeY;

			// Set the height on the entire row.
			for (x = 0; x < mNumCols; x++)
			{
				int lIndex = y * mNumCols + x;
					
				if (mComponent[lIndex] != 0 && mComponent[lIndex]->IsVisible() == true)
				{
					PixelCoords lPos(mComponent[lIndex]->GetPos());
					PixelCoords lSize(mComponent[lIndex]->GetSize());
					lPos.y  = lPosYInt;
					lSize.y = lSizeY;
					mComponent[lIndex]->SetPos(lPos);
					mComponent[lIndex]->SetSize(lSize);
				}
			}

			lPosY += lHeight;
		}
	}

	lDiff = 0;

	for (x = 0, lIter = lWidthList.begin(); x < mNumCols; ++x, ++lIter)
	{
		double lWidth = (double)(*lIter);

		if (lWidth >= 0)
		{
			if (lWidth == 0)
			{
				lWidth = lRestWidth;
			}
			else
			{
				lWidth *= lWidthFactor;
			}

			lWidth += lDiff;

			int lPosXInt = (int)floor(lPosX + 0.5);
			int lSizeX = (int)floor(lPosX + lWidth + 0.5) - lPosXInt;

			lDiff = lWidth - (double)lSizeX;

			// Set the width on the entire column.
			for (y = 0; y < mNumRows; y++)
			{
				int lIndex = y * mNumCols + x;
					
				if (mComponent[lIndex] != 0 && mComponent[lIndex]->IsVisible() == true)
				{
					PixelCoords lPos(mComponent[lIndex]->GetPos());
					PixelCoords lSize(mComponent[lIndex]->GetSize());
					lPos.x  = lPosXInt;
					lSize.x = lSizeX;
					mComponent[lIndex]->SetPos(lPos);
					mComponent[lIndex]->SetSize(lSize);
				}
			}

			lPosX += lWidth;
		}
	}
}

PixelCoords GridLayout::GetPreferredSize(bool pForceAdaptive)
{
	PixelCoords lSize(0, 0);

	int x, y;

	for (y = 0; y < mNumRows; y++)
	{
		int lWidthSum = 0;

		for (x = 0; x < mNumCols; x++)
		{
			int lIndex = y * mNumCols + x;
				
			if (mComponent[lIndex] != 0)
			{
				lWidthSum += mComponent[lIndex]->GetPreferredSize(pForceAdaptive).x;
			}
		}

		if (lWidthSum > lSize.x)
		{
			lSize.x = lWidthSum;
		}
	}

	for (x = 0; x < mNumCols; x++)
	{
		int lHeightSum = 0;

		for (y = 0; y < mNumRows; y++)
		{
			int lIndex = y * mNumCols + x;
				
			if (mComponent[lIndex] != 0)
			{
				lHeightSum += mComponent[lIndex]->GetPreferredSize(pForceAdaptive).y;
			}
		}

		if (lHeightSum > lSize.y)
		{
			lSize.y = lHeightSum;
		}
	}

	return lSize;
}

PixelCoords GridLayout::GetContentSize()
{
	return GetOwner()->GetSize();
}

PixelCoords GridLayout::GetMinSize()
{
	PixelCoords lSize(0, 0);

	int x, y;

	for (y = 0; y < mNumRows; y++)
	{
		int lWidthSum = 0;

		for (x = 0; x < mNumCols; x++)
		{
			int lIndex = y * mNumCols + x;
				
			if (mComponent[lIndex] != 0)
			{
				lWidthSum += mComponent[lIndex]->GetMinSize().x;
			}
		}

		if (lWidthSum > lSize.x)
		{
			lSize.x = lWidthSum;
		}
	}

	for (x = 0; x < mNumCols; x++)
	{
		int lHeightSum = 0;

		for (y = 0; y < mNumRows; y++)
		{
			int lIndex = y * mNumCols + x;
				
			if (mComponent[lIndex] != 0)
			{
				lHeightSum += mComponent[lIndex]->GetMinSize().y;
			}
		}

		if (lHeightSum > lSize.y)
		{
			lSize.y = lHeightSum;
		}
	}

	return lSize;
}

void GridLayout::InsertRow(int pRow)
{
	if (pRow < 0)
	{
		pRow = 0;
	}

	if (pRow > mNumRows)
	{
		pRow = mNumRows;
	}

	Component** lComponent = AllocComponentGrid(mNumRows + 1, mNumCols);

	int i;

	// Copy all rows before pRow.
	for (i = 0; i < pRow; i++)
	{
		int lRowIndex = i * mNumCols;
		for (int j = 0; j < mNumCols; j++)
		{
			lComponent[lRowIndex + j] = mComponent[lRowIndex + j];
		}
	}

	// Copy all rows after pRow.
	for (i = pRow; i < mNumRows; i++)
	{
		int lRowIndexSrc = i * mNumCols;
		int lRowIndexDst = (i + 1) * mNumCols;
		for (int j = 0; j < mNumCols; j++)
		{
			lComponent[lRowIndexDst + j] = mComponent[lRowIndexSrc + j];
		}
	}

	if (mComponent != 0)
	{
		delete[] mComponent;
	}

	mComponent = lComponent;
	mNumRows++;
}

void GridLayout::InsertColumn(int pColumn)
{
	if (pColumn < 0)
	{
		pColumn = 0;
	}

	if (pColumn > mNumCols)
	{
		pColumn = mNumCols;
	}

	Component** lComponent = AllocComponentGrid(mNumRows, mNumCols + 1);

	for (int j = 0; j < mNumRows; j++)
	{
		int i;
		int lRowIndex = j * mNumCols;

		// Copy all columns before pColumn.
		for (i = 0; i < pColumn; i++)
		{
			lComponent[lRowIndex + i] = mComponent[lRowIndex + i];
		}

		// Copy all columns after pColumn.
		for (i = pColumn; i < mNumCols; i++)
		{
			lComponent[lRowIndex + i + 1] = mComponent[lRowIndex + i];
		}
	}

	if (mComponent != 0)
	{
		delete[] mComponent;
	}

	mComponent = lComponent;
	mNumCols++;
}

void GridLayout::DeleteRow(int pRow)
{
	bool lOk = (pRow >= 0 && pRow < mNumRows);
	if (lOk)
	{
		Component** lComponent = AllocComponentGrid(mNumRows - 1, mNumCols);

		int i;

		// Copy all rows before pRow.
		for (i = 0; i < pRow; i++)
		{
			int lRowIndex = i * mNumCols;
			for (int j = 0; j < mNumCols; j++)
			{
				lComponent[lRowIndex + j] = mComponent[lRowIndex + j];
			}
		}

		// Copy all rows after pRow.
		for (i = pRow + 1; i < mNumRows; i++)
		{
			int lRowIndexSrc = i * mNumCols;
			int lRowIndexDst = (i - 1) * mNumCols;
			for (int j = 0; j < mNumCols; j++)
			{
				lComponent[lRowIndexDst + j] = mComponent[lRowIndexSrc + j];
			}
		}

		if (mComponent != 0)
		{
			delete[] mComponent;
		}

		mComponent = lComponent;
		mNumRows--;
	}
}

void GridLayout::DeleteColumn(int pColumn)
{
	bool lOk = (pColumn >= 0 && pColumn < mNumCols);

	if (lOk)
	{
		Component** lComponent = AllocComponentGrid(mNumRows, mNumCols - 1);

		for (int j = 0; j < mNumRows; j++)
		{
			int i;
			int lRowIndex = j * mNumCols;

			// Copy all columns before pColumn.
			for (i = 0; i < pColumn; i++)
			{
				lComponent[lRowIndex + i] = mComponent[lRowIndex + i];
			}

			// Copy all columns after pColumn.
			for (i = pColumn + 1; i < mNumCols; i++)
			{
				lComponent[lRowIndex + i - 1] = mComponent[lRowIndex + i];
			}
		}

		if (mComponent != 0)
		{
			delete[] mComponent;
		}

		mComponent = lComponent;
		mNumCols--;
	}
}

bool GridLayout::HaveDoubles()
{
	for (int i = 0; i < mNumRows * mNumCols; i++)
	{
		for (int j = i + 1; j < mNumRows * mNumCols; j++)
		{
			if (mComponent[i] == mComponent[j])
			{
				return true;
			}
		}
	}

	return false;
}


} // End namespace.
