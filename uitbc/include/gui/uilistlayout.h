/*
	Class:  ListLayout
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/



#pragma once

#include "uilayout.h"
#include <list>
#include "../../../lepra/include/hashtable.h"
#include "../../../lepra/include/loosebintree.h"



namespace uitbc {



class ListLayout: public Layout {
public:
	enum ListType {
		kColumn = 0,
		kRow,
	};

	ListLayout(ListType list_type = kColumn);
	virtual ~ListLayout();

	virtual Type GetType() const;

	ListType GetListType() const;

	typedef std::list<Component*> ComponentList;

	// param1 is the indentation level, starting at 0. For normal lists
	//           this is always 0. This parameter can be used to create tree
	//           views.
	// param2 is not used.
	virtual void Add(Component* component, int param1, int param2);
	virtual void Remove(Component* component);

	void AddChildAfter(Component* child, Component* after_this, int indentation_level);
	void AddChildrenAfter(std::list<Component*>& child_list, Component* after_this, int indentation_level);

	// XY... If ListType is kColumn, then this is the Y-coordinate,
	// X-coordinate otherwise.
	virtual Component* Find(int screen_xy);

	// XY... Same as above.
	virtual void Find(ComponentList& components, int screen_x_y1, int screen_x_y2);

	virtual Component* FindIndex(int index);

	virtual int GetNumComponents() const;

	virtual Component* GetFirst();
	virtual Component* GetNext();
	virtual Component* GetLast();
	virtual Component* GetPrev();

	virtual Component* GetNext(Component* current);
	virtual Component* GetPrev(Component* current);

	virtual void UpdateLayout();

	virtual PixelCoord GetPreferredSize(bool force_adaptive);
	virtual PixelCoord GetMinSize() const;
	virtual PixelCoord GetContentSize() const;

	void SetPosOffset(int dx, int dy);
	int GetPosDX() const;
	int GetPosDY() const;

	// Indentation is given in pixels. This is the indentation per "level".
	// See Add(Component* , ...) for more details.
	void SetIndentationSize(int indentation_size);
	int GetIndentationSize() const;

	// HW = Height or Width. Depends on ListType:
	// kColumn -> Height
	// kRow -> Width
	float64 GetAverageComponentHW() const;

	bool IsEmpty() const;

protected:
private:
	class Node {
	public:
		Node() :
			component_(0),
			indentation_level_(0) {
		}

		Node(const Node& node) :
			component_(node.component_),
			indentation_level_(node.indentation_level_) {
		}

		Node(Component* component, int indentation_level) :
			component_(component),
			indentation_level_(indentation_level) {
		}

		Component* component_;
		int indentation_level_;
	};

	int GetPreferredHW(Component* component);

	typedef std::list<Node> NodeList;
	typedef LooseBinTree<Component*, Component*, float64, LEPRA_VOIDP_HASHER> ComponentTree;
	typedef HashTable<Component*, NodeList::iterator, LEPRA_VOIDP_HASHER> ComponentTable;

	NodeList::iterator iter_;
	NodeList node_list_;
	ComponentTree component_tree_;
	ComponentTable component_table_;

	PixelCoord content_size_;

	int pos_dx_;
	int pos_dy_;
	int list_hw_; // Height or Width in pixels.

	int indentation_size_;

	ListType list_type_;
};



}
