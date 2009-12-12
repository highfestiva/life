/*
	Class:  ListLayout
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games
*/

#include "../../Include/GUI/UiListLayout.h"
#include "../../Include/GUI/UiComponent.h"
#include "../../../Lepra/Include/ListUtil.h"

namespace UiTbc
{

ListLayout::ListLayout(ListType pListType) :
	mComponentTree(0, (Lepra::float64)(1 << 30), 1.0),
	mContentSize(0,0),
	mPosDX(0),
	mPosDY(0),
	mListHW(0),
	mIndentationSize(1),
	mListType(pListType)
{
	mIter = mNodeList.end();
}

ListLayout::~ListLayout()
{
}

Layout::Type ListLayout::GetType()
{
	return Layout::LISTLAYOUT;
}

void ListLayout::Add(Component* pComponent, int pParam1, int /*pParam2*/)
{
	Lepra::float64 lHalfHW = (Lepra::float64)GetPreferredHW(pComponent) / 2.0;
	Lepra::float64 lPos    = (Lepra::float64)mListHW + lHalfHW;
	mComponentTree.InsertObject(pComponent, pComponent, lPos, lHalfHW);

	mNodeList.push_back(Node(pComponent, pParam1));
	mComponentTable.Insert(pComponent, --mNodeList.end());

	mListHW += GetPreferredHW(pComponent);
}

void ListLayout::AddChildAfter(Component* pChild, Component* pAfterThis, int pIndentationLevel)
{
	ComponentTable::Iterator lTIter = mComponentTable.Find(pAfterThis);

	if (lTIter != mComponentTable.End())
	{
		NodeList::iterator lBeforeThisIter(*lTIter);
		++lBeforeThisIter;
		NodeList::iterator lChildIter = mNodeList.insert(lBeforeThisIter, Node(pChild, pIndentationLevel));
		mComponentTable.Insert(pChild, lChildIter);

		int lHW = GetPreferredHW(pChild);
		mListHW += lHW;

		// Get the size and position of the existing child.
		Lepra::float64 lPrevHalfHW;
		Lepra::float64 lPrevPos;
		mComponentTree.GetObjectSizeAndPos((*(*lTIter)).mComponent, lPrevPos, lPrevHalfHW);

		// The position and the size of the new child.
		Lepra::float64 lHalfHW = (Lepra::float64)lHW / 2.0;
		Lepra::float64 lPos    = lPrevPos + lPrevHalfHW + lHalfHW;
		
		Lepra::float64 lNewPos = lPos + lHalfHW;

		// Move all affected objects in the component tree.
		NodeList::iterator lIter2(lChildIter);
		++lIter2;
		for (lIter2; lIter2 != mNodeList.end(); ++lIter2)
		{
			Lepra::float64 lNewHW = GetPreferredHW((*lIter2).mComponent);
			mComponentTree.MoveObject((*lIter2).mComponent, lNewPos + lNewHW / 2.0, lNewHW / 2.0);
			lNewPos += lNewHW;
		}

		// Insert the new child in the component tree.
		mComponentTree.InsertObject(pChild, pChild, lPos, lHalfHW);
	}
}

void ListLayout::AddChildrenAfter(std::list<Component*>& pChildList, Component* pAfterThis, int pIndentationLevel)
{
	ComponentTable::Iterator lTIter = mComponentTable.Find(pAfterThis);

	if (lTIter != mComponentTable.End())
	{
		NodeList::iterator lAfterThisIter(*lTIter);

		// Get the size and position of the existing child.
		Lepra::float64 lHalfHW;
		Lepra::float64 lPos;
		mComponentTree.GetObjectSizeAndPos((*lAfterThisIter).mComponent, lPos, lHalfHW);
		lPos += lHalfHW;

		NodeList::iterator lNext(lAfterThisIter);
		++lNext;

		std::list<Component*>::iterator lCLIter;
		for (lCLIter = pChildList.begin(); lCLIter != pChildList.end(); ++lCLIter)
		{
			Component* lChild = *lCLIter;

			NodeList::iterator lChildIter = mNodeList.insert(++lAfterThisIter, Node(lChild, pIndentationLevel));
			lAfterThisIter = lChildIter;
			mComponentTable.Insert(lChild, lChildIter);

			int lHW = GetPreferredHW(lChild);
			mListHW += lHW;

			lHalfHW = (Lepra::float64)lHW / 2.0;

			// Insert the new child in the component tree.
			mComponentTree.InsertObject(lChild, lChild, lPos + lHalfHW, lHalfHW);

			lPos += (Lepra::float64)lHW;
		}

		
		// Move all affected objects in the component tree.
		for (lNext; lNext != mNodeList.end(); ++lNext)
		{
			Lepra::float64 lNewHW = GetPreferredHW((*lNext).mComponent);
			mComponentTree.MoveObject((*lNext).mComponent, lPos + lNewHW / 2.0, lNewHW / 2.0);
			lPos += lNewHW;
		}
	}
}

void ListLayout::Remove(Component* pComponent)
{
	ComponentTable::Iterator lTIter = mComponentTable.Find(pComponent);
	if (lTIter != mComponentTable.End())
	{
		NodeList::iterator lIter = (*lTIter);
		mNodeList.erase(lIter);
		mComponentTree.RemoveObject(pComponent);
		mComponentTable.Remove(lTIter);

		// We need to loop through the entire list and update the layout.
		
		mListHW = 0;
		int i = 0;
		
		for (lIter = mNodeList.begin(); lIter != mNodeList.end(); ++lIter)
		{
			Node& lNode = *lIter;
			int lHW = GetPreferredHW(lNode.mComponent);
			
			Lepra::float64 lHalfHW = (Lepra::float64)lHW / 2.0;
			Lepra::float64 lPos    = (Lepra::float64)mListHW + lHalfHW;
			mComponentTree.MoveObject(lNode.mComponent, lPos, lHalfHW);
			mListHW += lHW;
			i++;
		}
	}
}

Component* ListLayout::Find(const Lepra::String& pComponentName)
{
	NodeList::iterator lIter;
	for (lIter = mNodeList.begin(); lIter != mNodeList.end(); ++lIter)
	{
		Node& lNode = *lIter;
		if (lNode.mComponent->GetName() == pComponentName)
		{
			return lNode.mComponent;
		}
	}

	return 0;
}

int ListLayout::GetNumComponents() const
{
	return (int)mNodeList.size();
}

Component* ListLayout::Find(int pScreenXY)
{
	Lepra::PixelCoords lOwnerPos(GetOwner()->GetScreenPos());
	Lepra::float64 lPos = 0;
	
	if (mListType == COLUMN)
	{
		lPos = (Lepra::float64)(pScreenXY - lOwnerPos.y - mPosDY);
	}
	else // if (mListType == ROW)
	{
		lPos = (Lepra::float64)(pScreenXY - lOwnerPos.x - mPosDX);
	}

	ComponentTree::ObjectList lList;
	mComponentTree.GetObjects(lList, lPos, 0.5);

	if (lList.empty() == true)
	{
		return 0;
	}

	if (lList.size() == 1)
	{
		return lList.front();
	}

	// The list shouldn't contain more than one component.
	// But if it does, somehow, let's find the correct one.
	ComponentTree::ObjectList::iterator lIter;
	for (lIter = lList.begin(); lIter != lList.end(); ++lIter)
	{
		Component* lComp = *lIter;
		Lepra::PixelRect lRect(lComp->GetScreenRect());

		if (mListType == COLUMN && lRect.IsInside(lRect.GetCenterX(), pScreenXY) == true ||
		    mListType == ROW && lRect.IsInside(pScreenXY, lRect.GetCenterY()) == true)
		{
			return lComp;
		}
	}

	return 0;
}

void ListLayout::Find(ComponentList& pComponents, int pScreenXY1, int pScreenXY2)
{
	Lepra::PixelCoords lOwnerPos(GetOwner()->GetScreenPos());

	if (pScreenXY1 > pScreenXY2)
	{
		int lTemp = pScreenXY1;
		pScreenXY1 = pScreenXY2;
		pScreenXY2 = lTemp;
	}

	Lepra::float64 lUpper = 0;
	Lepra::float64 lLower = 0;
	
	if (mListType == COLUMN)
	{
		lUpper = (Lepra::float64)(pScreenXY1 - lOwnerPos.y - mPosDY);
		lLower = (Lepra::float64)(pScreenXY2 - lOwnerPos.y - mPosDY);
	}
	else // if (mListType == ROW)
	{
		lUpper = (Lepra::float64)(pScreenXY1 - lOwnerPos.x - mPosDX);
		lLower = (Lepra::float64)(pScreenXY2 - lOwnerPos.x - mPosDX);
	}

	mComponentTree.GetObjects(pComponents, (lLower + lUpper) * 0.5, (lLower - lUpper) * 0.5);
}

Component* ListLayout::FindIndex(int pIndex)
{
	Component* lComponent = 0;

	if (pIndex >= 0 && pIndex < (int)mNodeList.size())
	{
		lComponent = (*Lepra::ListUtil::FindByIndex(mNodeList, pIndex)).mComponent;
	}

	return lComponent;
}

Component* ListLayout::GetFirst()
{
	if (mNodeList.empty() == true)
	{
		return 0;
	}

	mIter = mNodeList.begin();
	return (*mIter).mComponent;
}

Component* ListLayout::GetNext()
{
	++mIter;
	if (mIter == mNodeList.end())
	{
		return 0;
	}

	return (*mIter).mComponent;
}

Component* ListLayout::GetLast()
{
	if (mNodeList.empty() == true)
	{
		return 0;
	}

	mIter = --mNodeList.end();
	return (*mIter).mComponent;
}

Component* ListLayout::GetPrev()
{
	--mIter;
	if (mIter == mNodeList.end())
	{
		return 0;
	}

	return (*mIter).mComponent;
}

Component* ListLayout::GetNext(Component* pCurrent)
{
	Component* lNext = 0;
	ComponentTable::Iterator lIter = mComponentTable.Find(pCurrent);

	if (lIter != mComponentTable.End())
	{
		NodeList::iterator lListIter = (*lIter);
		++lListIter;

		if (lListIter != mNodeList.end())
		{
			lNext = (*lListIter).mComponent;
		}
	}

	return lNext;
}

Component* ListLayout::GetPrev(Component* pCurrent)
{
	Component* lPrev = 0;
	ComponentTable::Iterator lIter = mComponentTable.Find(pCurrent);

	if (lIter != mComponentTable.End())
	{
		NodeList::iterator lListIter = (*lIter);
		--lListIter;

		if (lListIter != mNodeList.end())
		{
			lPrev = (*lListIter).mComponent;
		}
	}

	return lPrev;
}

void ListLayout::UpdateLayout()
{
	mContentSize.x = 0;
	mContentSize.y = 0;

	Lepra::PixelCoords lOwnerSize(GetOwner()->GetSize());
	NodeList::iterator lIter;

	if (mListType == COLUMN)
	{
		for (lIter = mNodeList.begin(); lIter != mNodeList.end(); ++lIter)
		{
			Node& lNode = *lIter;
			Lepra::PixelCoords lSize(lNode.mComponent->GetPreferredWidth(true), lNode.mComponent->GetPreferredHeight());

			lSize.x += lNode.mIndentationLevel * mIndentationSize;

			if (lSize.x < lOwnerSize.x)
			{
				lSize.x = lOwnerSize.x;
			}

			if (mContentSize.x < lSize.x)
			{
				mContentSize.x = lSize.x;
			}

			mContentSize.y += lSize.y;
		}

		int y = mPosDY;
		for (lIter = mNodeList.begin(); lIter != mNodeList.end(); ++lIter)
		{
			Node& lNode = *lIter;
			lNode.mComponent->SetPos(mPosDX + lNode.mIndentationLevel * mIndentationSize, y);
			Lepra::PixelCoords lSize(lNode.mComponent->GetPreferredSize());

			lSize.x = mContentSize.x;
			lNode.mComponent->SetSize(lSize);

			y += lSize.y;
		}
	}
	else // if (mListType == ROW)
	{
		for (lIter = mNodeList.begin(); lIter != mNodeList.end(); ++lIter)
		{
			Node& lNode = *lIter;
			Lepra::PixelCoords lSize(lNode.mComponent->GetPreferredWidth(), lNode.mComponent->GetPreferredHeight(true));

			lSize.y += lNode.mIndentationLevel * mIndentationSize;

			if (lSize.y < lOwnerSize.y)
			{
				lSize.y = lOwnerSize.y;
			}

			if (mContentSize.y < lSize.y)
			{
				mContentSize.y = lSize.y;
			}

			mContentSize.x += lSize.x;
		}

		int x = mPosDX;
		for (lIter = mNodeList.begin(); lIter != mNodeList.end(); ++lIter)
		{
			Node& lNode = *lIter;
			lNode.mComponent->SetPos(x, mPosDY + lNode.mIndentationLevel * mIndentationSize);
			Lepra::PixelCoords lSize(lNode.mComponent->GetPreferredSize());

			lSize.y = mContentSize.y;
			lNode.mComponent->SetSize(lSize);

			x += lSize.x;
		}
	}
}

Lepra::PixelCoords ListLayout::GetPreferredSize(bool /*pForceAdaptive*/)
{
	return Lepra::PixelCoords(0, 0);
}

Lepra::PixelCoords ListLayout::GetMinSize()
{
	return Lepra::PixelCoords(0, 0);
}

Lepra::PixelCoords ListLayout::GetContentSize()
{
	return mContentSize;
}

int ListLayout::GetPreferredHW(Component* pComponent)
{
	return mListType == COLUMN ? pComponent->GetPreferredHeight() : pComponent->GetPreferredWidth();
}

} // End namespace.
