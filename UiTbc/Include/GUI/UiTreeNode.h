/*
	Class:  TreeNode
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

	NOTES:

	After days of headache trying to figure out how to design a
	general treeview component in a way such that as much code as
	possible is reused, I concluded that the best way to do it is
	by not implementing a tree view at all! The ListControl already
	offers most of the needed functionality.

	This class (TreeNode) is supposed to be used together with a
	ListControl in order to create a tree view. It manages the
	tree structure and all GUI-logic needed.

	The TreeNode consists of a GridLayout of one row and two columns:

	[ a | b ]

	'a' is the expand/collaps icon (if used) which normally consists
	of a small button with a '+' or '-' sign.

	'b' is a label, which in turn consists of an icon and a string.
*/

#ifndef UITREENODE_H
#define UITREENODE_H

#include "UiComponent.h"
#include "UiCheckButton.h"
#include "UiLabel.h"
#include "../UiPainter.h"
#include <list>

namespace UiTbc
{

class ListControl;

class TreeNode: public Component
{
	typedef Component Parent;
public:
	// Uses the default icons.
	TreeNode(const wstr& pText);

	// User specifies his own node icons. The default 
	// expand/collapse icons are still used.
	TreeNode(Painter::ImageID pCollapsedIconID,
		 Painter::ImageID pExpandedIconID,
		 const wstr& pText);

	// User defines all icons.
	TreeNode(Painter::ImageID pCollapsedIconID,
		 Painter::ImageID pExpandedIconID,
		 Painter::ImageID pCollapseIconID,
		 Painter::ImageID pExpandeIconID,
		 const wstr& pText);

	~TreeNode();

	// Use this to create the tree structure.
	virtual void AddChildNode(TreeNode* pChildNode);
	inline TreeNode* GetParentNode() const;

	inline bool IsExpandable();
	inline bool IsExpanded() const;
	void SetExpanded(bool pExpanded);

	virtual void SetSelected(bool pSelected);

	// Specifies the font to be used when creating new TreeNodes. Doesn't affect
	// currently existing nodes.
	static void UseFont(const Color& pTextColor,
			    const Color& pTextBackgColor,
			    const Color& pTextSelectedColor,
			    const Color& pTextSelectedBackgColor);


	virtual bool OnDoubleClick(int pMouseX, int pMouseY);
	virtual bool OnKeyDown(UiLepra::InputManager::KeyCode pKeyCode);

	virtual void SetKeyboardFocus();

protected:
	void SetExpanded(bool pExpanded, ListControl* pListControl);
private:
	// This list type only takes TreeNode*, but is declared as
	// if it is taking Component*. This way the NodeList is compatible
	// with ListControl::AddChildrenAfter().
	typedef std::list<Component*> NodeList;

	void Init(const wstr& pText);
	void ValidatePainterAndIcons();
	bool ValidateExpandButton();
	bool ValidateIconRect(const wstr& pText);

	void AddDefaultIconIfNeeded(Painter::ImageID& pLocalIconID, Painter::ImageID& pStaticIconID, void* pBuffer, int pXOffset, int pYOffset);
	void ResetUserIcons();

	void OnExpandButtonUnclicked(Button* pButton);

	inline int GetIndentationLevel() const;
	inline void SetIndentationLevel(int pIndentationLevel);

	static Painter::ImageID smExpandIconID;
	static Painter::ImageID smCollapseIconID;
	static Painter::ImageID smExpandedIconID;
	static Painter::ImageID smCollapsedIconID;
	static uint8 smIconExpand[];
	static uint8 smIconCollapse[];
	static uint8 smIconCollapsed[];
	static uint8 smIconExpanded[];
	static Painter* smPrevPainter;

	// Factory members.
	static Color smFactoryTextColor;
	static Color smFactoryTextBackgColor;
	static Color smFactoryTextSelectedColor;
	static Color smFactoryTextSelectedBackgColor;
	static uint8 smFactoryAlphaTreshold;

	TreeNode* mParentNode;
	NodeList* mChildNodes;

	CheckButton* mExpandButton;
	Label* mIconLabel;

	Painter::ImageID mCollapseIconID;
	Painter::ImageID mExpandIconID;
	Painter::ImageID mCollapsedIconID;
	Painter::ImageID mExpandedIconID;

	int mIndentationLevel;
	int mChildIndentation;
};

TreeNode* TreeNode::GetParentNode() const
{
	return mParentNode;
}

bool TreeNode::IsExpandable()
{
	return (mChildNodes != 0);
}

bool TreeNode::IsExpanded() const
{
	return mExpandButton->GetPressed();
}

int TreeNode::GetIndentationLevel() const
{
	return mIndentationLevel;
}

} // End namespace.

#endif
