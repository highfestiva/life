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

#pragma once

#include "uicomponent.h"
#include "uicheckbutton.h"
#include "uilabel.h"
#include "../uipainter.h"
#include <list>

namespace uitbc {

class ListControl;

class TreeNode: public Component {
	typedef Component Parent;
public:
	// Uses the default icons.
	TreeNode(const wstr& text);

	// User specifies his own node icons. The default
	// expand/collapse icons are still used.
	TreeNode(Painter::ImageID collapsed_icon_id,
		 Painter::ImageID expanded_icon_id,
		 const wstr& text);

	// User defines all icons.
	TreeNode(Painter::ImageID collapsed_icon_id,
		 Painter::ImageID expanded_icon_id,
		 Painter::ImageID collapse_icon_id,
		 Painter::ImageID expande_icon_id,
		 const wstr& text);

	~TreeNode();

	// Use this to create the tree structure.
	virtual void AddChildNode(TreeNode* child_node);
	inline TreeNode* GetParentNode() const;

	inline bool IsExpandable();
	inline bool IsExpanded() const;
	void SetExpanded(bool expanded);

	virtual void SetSelected(bool selected);

	// Specifies the font to be used when creating new TreeNodes. Doesn't affect
	// currently existing nodes.
	static void UseFont(const Color& text_color,
			    const Color& text_backg_color,
			    const Color& text_selected_color,
			    const Color& text_selected_backg_color);


	virtual bool OnDoubleClick(int mouse_x, int mouse_y);
	virtual bool OnKeyDown(uilepra::InputManager::KeyCode key_code);

	virtual void SetKeyboardFocus();

protected:
	void SetExpanded(bool expanded, ListControl* list_control);
private:
	// This list type only takes TreeNode*, but is declared as
	// if it is taking Component*. This way the NodeList is compatible
	// with ListControl::AddChildrenAfter().
	typedef std::list<Component*> NodeList;

	void Init(const wstr& text);
	void ValidatePainterAndIcons();
	bool ValidateExpandButton();
	bool ValidateIconRect(const wstr& text);

	void AddDefaultIconIfNeeded(Painter::ImageID& local_icon_id, Painter::ImageID& static_icon_id, void* buffer, int x_offset, int y_offset);
	void ResetUserIcons();

	void OnExpandButtonUnclicked(Button* button);

	inline int GetIndentationLevel() const;
	inline void SetIndentationLevel(int indentation_level);

	static Painter::ImageID expand_icon_id__;
	static Painter::ImageID collapse_icon_id__;
	static Painter::ImageID expanded_icon_id__;
	static Painter::ImageID collapsed_icon_id__;
	static uint8 icon_expand_[];
	static uint8 icon_collapse_[];
	static uint8 icon_collapsed_[];
	static uint8 icon_expanded_[];
	static Painter* prev_painter_;

	// Factory members.
	static Color factory_text_color_;
	static Color factory_text_backg_color_;
	static Color factory_text_selected_color_;
	static Color factory_text_selected_backg_color_;
	static uint8 factory_alpha_treshold_;

	TreeNode* parent_node_;
	NodeList* child_nodes_;

	CheckButton* expand_button_;
	Label* icon_label_;

	Painter::ImageID collapse_icon_id_;
	Painter::ImageID expand_icon_id_;
	Painter::ImageID collapsed_icon_id_;
	Painter::ImageID expanded_icon_id_;

	int indentation_level_;
	int child_indentation_;
};

TreeNode* TreeNode::GetParentNode() const {
	return parent_node_;
}

bool TreeNode::IsExpandable() {
	return (child_nodes_ != 0);
}

bool TreeNode::IsExpanded() const {
	return expand_button_->GetPressed();
}

int TreeNode::GetIndentationLevel() const {
	return indentation_level_;
}

}
