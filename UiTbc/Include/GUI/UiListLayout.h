/*
	Class:  ListLayout
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/



#pragma once

#include "UiLayout.h"
#include <list>
#include "../../../Lepra/Include/HashTable.h"
#include "../../../Lepra/Include/LooseBinTree.h"



namespace UiTbc
{



class ListLayout: public Layout
{
public:
	enum ListType
	{
		COLUMN = 0,
		ROW,
	};

	ListLayout(ListType pListType = COLUMN);
	virtual ~ListLayout();

	virtual Type GetType() const;
	
	ListType GetListType() const;

	typedef std::list<Component*> ComponentList;

	// pParam1 is the indentation level, starting at 0. For normal lists
	//           this is always 0. This parameter can be used to create tree
	//           views.
	// pParam2 is not used.
	virtual void Add(Component* pComponent, int pParam1, int pParam2);
	virtual void Remove(Component* pComponent);

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

	virtual PixelCoord GetPreferredSize(bool pForceAdaptive);
	virtual PixelCoord GetMinSize() const;
	virtual PixelCoord GetContentSize() const;

	void SetPosOffset(int pDX, int pDY);
	int GetPosDX() const;
	int GetPosDY() const;

	// Indentation is given in pixels. This is the indentation per "level".
	// See Add(Component* , ...) for more details.
	void SetIndentationSize(int pIndentationSize);
	int GetIndentationSize() const;

	// HW = Height or Width. Depends on ListType:
	// COLUMN -> Height
	// ROW -> Width
	float64 GetAverageComponentHW() const;

	bool IsEmpty() const;

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
	typedef LooseBinTree<Component*, Component*, float64, LEPRA_VOIDP_HASHER> ComponentTree;
	typedef HashTable<Component*, NodeList::iterator, LEPRA_VOIDP_HASHER> ComponentTable;

	NodeList::iterator mIter;
	NodeList mNodeList;
	ComponentTree mComponentTree;
	ComponentTable mComponentTable;

	PixelCoord mContentSize;

	int mPosDX;
	int mPosDY;
	int mListHW; // Height or Width in pixels.

	int mIndentationSize;

	ListType mListType;
};



}
