/*
	Class:  ListLayout
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand
*/

#ifndef UILISTLAYOUT_H
#define UILISTLAYOUT_H

#include "UiLayout.h"
#include <list>
#include "../../../Lepra/Include/HashTable.h"
#include "../../../Lepra/Include/LooseBinTree.h"

namespace UiTbc
{

class ListLayout : public Layout
{
public:
	enum ListType
	{
		COLUMN = 0,
		ROW,
	};

	ListLayout(ListType pListType = COLUMN);
	virtual ~ListLayout();

	virtual Type GetType();
	
	inline ListType GetListType() const;

	typedef std::list<Component*> ComponentList;

	// pParam1 is the indentation level, starting at 0. For normal lists
	//           this is always 0. This parameter can be used to create tree
	//           views.
	// pParam2 is not used.
	virtual void Add(Component* pComponent, int pParam1, int pParam2);
	virtual void Remove(Component* pComponent);
	virtual Component* Find(const Lepra::String& pComponentName);

	void AddChildAfter(Component* pChild, Component* pAfterThis, int pIndentationLevel);
	void AddChildrenAfter(std::list<Component*>& pChildList, Component* pAfterThis, int pIndentationLevel);

	// XY... If ListType is COLUMN, then this is the Y-coordinate, 
	// X-coordinate otherwise.
	virtual Component* Find(int pScreenXY);

	// XY... Same as above.
	virtual void Find(ComponentList& pComponents, int pScreenXY1, int pScreenXY2);

	virtual Component* FindIndex(int pIndex);

	virtual int GetNumComponents() const;

	virtual Component* GetFirst();
	virtual Component* GetNext();

	virtual Component* GetLast();
	virtual Component* GetPrev();

	virtual Component* GetNext(Component* pCurrent);
	virtual Component* GetPrev(Component* pCurrent);

	virtual void UpdateLayout();

	virtual Lepra::PixelCoords GetPreferredSize(bool pForceAdaptive);
	virtual Lepra::PixelCoords GetMinSize();
	virtual Lepra::PixelCoords GetContentSize();

	inline void SetPosOffset(int pDX, int pDY);
	inline int GetPosDX() const;
	inline int GetPosDY() const;

	// Indentation is given in pixels. This is the indentation per "level".
	// See Add(Component* , ...) for more details.
	inline void SetIndentationSize(int pIndentationSize);
	inline int GetIndentationSize() const;

	// HW = Height or Width. Depends on ListType:
	// COLUMN -> Height
	// ROW -> Width
	inline Lepra::float64 GetAverageComponentHW();

	inline bool IsEmpty();

protected:
private:
	class Node
	{
	public:
		Node() :
			mComponent(0),
			mIndentationLevel(0)
		{
		}

		Node(const Node& pNode) :
			mComponent(pNode.mComponent),
			mIndentationLevel(pNode.mIndentationLevel)
		{
		}

		Node(Component* pComponent, int pIndentationLevel) :
			mComponent(pComponent),
			mIndentationLevel(pIndentationLevel)
		{
		}

		Component* mComponent;
		int mIndentationLevel;
	};

	int GetPreferredHW(Component* pComponent);

	typedef std::list<Node> NodeList;
	typedef Lepra::LooseBinTree<Component*, Component*, Lepra::float64, std::hash<void*> > ComponentTree;
	typedef Lepra::HashTable<Component*, NodeList::iterator, std::hash<void*> > ComponentTable;

	NodeList::iterator mIter;
	NodeList mNodeList;
	ComponentTree mComponentTree;
	ComponentTable mComponentTable;

	Lepra::PixelCoords mContentSize;

	int mPosDX;
	int mPosDY;
	int mListHW; // Height or Width in pixels.

	int mIndentationSize;

	ListType mListType;
};

ListLayout::ListType ListLayout::GetListType() const
{
	return mListType;
}

void ListLayout::SetPosOffset(int pDX, int pDY)
{
	mPosDX = pDX;
	mPosDY = pDY;
}

int ListLayout::GetPosDX() const
{
	return mPosDX;
}

int ListLayout::GetPosDY() const
{
	return mPosDY;
}

Lepra::float64 ListLayout::GetAverageComponentHW()
{
	Lepra::float64 lHW = 0;
	if (mListType == COLUMN)
	{
		lHW = (Lepra::float64)mContentSize.y / (Lepra::float64)mNodeList.size();
	}
	else // if (mListType == ROW)
	{
		lHW = (Lepra::float64)mContentSize.x / (Lepra::float64)mNodeList.size();
	}

	return lHW;
}

bool ListLayout::IsEmpty()
{
	return mNodeList.empty();
}

void ListLayout::SetIndentationSize(int pIndentationSize)
{
	mIndentationSize = pIndentationSize;
}

int ListLayout::GetIndentationSize() const
{
	return mIndentationSize;
}

} // End namespace.

#endif