/*
	Class:  TreeNode
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#include "pch.h"
#include "../../include/gui/uitreenode.h"
#include "../../include/gui/uigridlayout.h"
#include "../../include/gui/uicenterlayout.h"
#include "../../include/gui/uilistcontrol.h"
#include "../../include/uipainter.h"
#include "../../../lepra/include/listutil.h"

namespace uitbc {

TreeNode::TreeNode(const wstr& text):
	Component(new GridLayout(1, 1)),
	parent_node_(0),
	child_nodes_(0),
	expand_button_(0),
	icon_label_(0),
	collapse_icon_id_(Painter::kInvalidImageid),
	expand_icon_id_(Painter::kInvalidImageid),
	collapsed_icon_id_(Painter::kInvalidImageid),
	expanded_icon_id_(Painter::kInvalidImageid),
	indentation_level_(0),
	child_indentation_(16) {
	Init(text);
}

TreeNode::TreeNode(Painter::ImageID collapsed_icon_id,
		   Painter::ImageID expanded_icon_id,
		   const wstr& text) :
	Component(new GridLayout(1, 1)),
	parent_node_(0),
	child_nodes_(0),
	expand_button_(0),
	icon_label_(0),
	collapse_icon_id_(Painter::kInvalidImageid),
	expand_icon_id_(Painter::kInvalidImageid),
	collapsed_icon_id_(collapsed_icon_id),
	expanded_icon_id_(expanded_icon_id),
	indentation_level_(0),
	child_indentation_(16) {
	Init(text);
}

TreeNode::TreeNode(Painter::ImageID collapsed_icon_id,
		   Painter::ImageID expanded_icon_id,
		   Painter::ImageID collapse_icon_id,
		   Painter::ImageID expande_icon_id,
		   const wstr& text):
	Component(new GridLayout(1, 1)),
	parent_node_(0),
	child_nodes_(0),
	expand_button_(0),
	icon_label_(0),
	collapse_icon_id_(collapse_icon_id),
	expand_icon_id_(expande_icon_id),
	collapsed_icon_id_(collapsed_icon_id),
	expanded_icon_id_(expanded_icon_id),
	indentation_level_(0),
	child_indentation_(0) {
	Init(text);

	GUIImageManager* i_man = GetImageManager();
	child_indentation_ = i_man->GetImageSize(expande_icon_id).x;
}

TreeNode::~TreeNode() {
	if (child_nodes_ != 0) {
		// If expanded, the ListControl will delete these.
		if (expand_button_->GetPressed() == false) {
			ListUtil::DeleteAll(*child_nodes_);
		}

		delete child_nodes_;
		child_nodes_ = 0;
	}
}

void TreeNode::Init(const wstr& text) {
	ValidatePainterAndIcons();
	ValidateExpandButton();
	ValidateIconRect(text);

	SetPreferredSize(0, 0, true);
	int preferred_height = 0;

	if (expand_button_ && expand_button_->GetPreferredHeight() > preferred_height) {
		preferred_height = expand_button_->GetPreferredHeight();
	}

	if (icon_label_ && icon_label_->GetPreferredHeight() > preferred_height) {
		preferred_height = icon_label_->GetPreferredHeight();
	}

	SetPreferredHeight(preferred_height);
}

void TreeNode::OnExpandButtonUnclicked(Button* /*button*/) {
	SetExpanded(expand_button_->GetPressed());
}

void TreeNode::AddChildNode(TreeNode* child_node) {
	if (child_nodes_ == 0) {
		child_nodes_ = new NodeList();
		SetNeedsRepaint(true);
	}
	child_nodes_->push_back(child_node);

	child_node->SetIndentationLevel(GetIndentationLevel() + child_indentation_);
	ValidateExpandButton();
}

void TreeNode::SetIndentationLevel(int indentation_level) {
	int diff = indentation_level - indentation_level_;
	indentation_level_ = indentation_level;

	if (child_nodes_ != 0) {
		NodeList::iterator iter;
		for (iter = child_nodes_->begin(); iter != child_nodes_->end(); ++iter) {
			TreeNode* node = (TreeNode*)(*iter);
			node->SetIndentationLevel(node->GetIndentationLevel() + diff);
		}
	}
}

void TreeNode::SetExpanded(bool expanded) {
	ListControl* _list_control = (ListControl*)Parent::GetParentOfType(Parent::kListcontrol);
	if (_list_control != 0) {
		SetExpanded(expanded, _list_control);
	}
}

void TreeNode::SetExpanded(bool expanded, ListControl* list_control) {
	SetNeedsRepaint(true);

	if (child_nodes_ != 0) {
		int h_offset;
		int v_offset;
		list_control->GetScrollOffsets(h_offset, v_offset);

		if (expanded) {
			// Insert all the children in the parent's list control.
			list_control->AddChildrenAfter(*child_nodes_, this, GetIndentationLevel() + child_indentation_);
			NodeList::iterator iter;
			for (iter = child_nodes_->begin(); iter != child_nodes_->end(); ++iter) {
				TreeNode* node = (TreeNode*)(*iter);
				if (node->IsExpandable() && node->IsExpanded()) {
					node->SetExpanded(true, list_control);
				}
			}

			icon_label_->SetIcon(expanded_icon_id_, Label::kIconLeft);
		} else {
			// Remove all the children from the parent's list control.
			NodeList::iterator iter;
			for (iter = child_nodes_->begin(); iter != child_nodes_->end(); ++iter) {
				list_control->RemoveChild(*iter, 0);
			}
			for (iter = child_nodes_->begin(); iter != child_nodes_->end(); ++iter) {
				TreeNode* node = (TreeNode*)(*iter);
				if (node->IsExpandable() && node->IsExpanded()) {
					node->SetExpanded(false, list_control);
				}
			}

			icon_label_->SetIcon(collapsed_icon_id_, Label::kIconLeft);
		}

		list_control->UpdateLayout();
		list_control->SetScrollOffsets(h_offset, v_offset);
	}
}

void TreeNode::SetSelected(bool selected) {
	Parent::SetSelected(selected);

	if (icon_label_ != 0) {
		icon_label_->SetSelected(selected);
	}
}

void TreeNode::SetKeyboardFocus() {
	if (icon_label_ != 0) {
		//GetTopParent()->ReleaseKeyboardFocus(kRecurseDown, icon_label_);
		icon_label_->SetKeyboardFocus();
	} else {
		Parent::SetKeyboardFocus();
	}
}

void TreeNode::ValidatePainterAndIcons() {
	ResetUserIcons();
	AddDefaultIconIfNeeded(expand_icon_id_, expand_icon_id__, icon_expand_, 3, 3);
	AddDefaultIconIfNeeded(collapse_icon_id_, collapse_icon_id__, icon_collapse_, 3, 3);
	AddDefaultIconIfNeeded(expanded_icon_id_, expanded_icon_id__, icon_expanded_, 2, 3);
	AddDefaultIconIfNeeded(collapsed_icon_id_, collapsed_icon_id__, icon_collapsed_, 2, 3);
	ResetUserIcons();
}

void TreeNode::AddDefaultIconIfNeeded(Painter::ImageID& local_icon_id, Painter::ImageID& static_icon_id, void* buffer, int x_offset, int y_offset) {
	if (local_icon_id == Painter::kInvalidImageid ||
	    static_icon_id != Painter::kInvalidImageid) {
		Color palette[256];
		palette[0].Set(0, 0, 0, 0);
		palette[1].Set(0, 0, 0, 255);
		palette[2].Set(251, 233, 160, 255);
		palette[3].Set(255, 255, 255, 255);
		palette[4].Set(255, 0, 0, 255);
		palette[106].Set(149, 149, 149, 255);
		palette[255].Set(0, 0, 0, 255);

		Canvas canvas(16, 16, Canvas::kBitdepth8Bit);
		canvas.SetPalette(palette);
		canvas.SetBuffer(buffer);

		Canvas alpha;
		uint8 transparent_color = 0;
		canvas.GetAlphaChannel(alpha, &transparent_color, 1);
		canvas.ConvertTo32BitWithAlpha(alpha);

		GUIImageManager* i_man = GetImageManager();
		static_icon_id = i_man->AddImage(canvas, GUIImageManager::kCentered, GUIImageManager::kAlphatest, 128);
		i_man->SetImageOffset(static_icon_id, x_offset, y_offset);
	}
}

void TreeNode::ResetUserIcons() {
	//
	// Use the default icons if not set by user.
	//

	if (expand_icon_id_ == Painter::kInvalidImageid) {
		expand_icon_id_ = expand_icon_id__;
	}
	if (collapse_icon_id_ == Painter::kInvalidImageid) {
		collapse_icon_id_ = collapse_icon_id__;
	}
	if (expanded_icon_id_ == Painter::kInvalidImageid) {
		expanded_icon_id_ = expanded_icon_id__;
	}
	if (collapsed_icon_id_ == Painter::kInvalidImageid) {
		collapsed_icon_id_ = collapsed_icon_id__;
	}
}

bool TreeNode::ValidateExpandButton() {
	bool repaint = false;

	if (child_nodes_ != 0 && expand_button_ == 0) {
		GUIImageManager* i_man = GetImageManager();

		/*expand_button_ = new CheckButton(expand_icon_id_, collapse_icon_id_,
						 expand_icon_id_, collapse_icon_id_,
						 collapse_icon_id_, expand_icon_id_,
						 "ExpandButton");*/

		expand_button_->SetPreferredSize(i_man->GetImageSize(expand_icon_id_));
		expand_button_->SetMinSize(expand_button_->GetPreferredSize());

		expand_button_->SetOnClick(TreeNode, OnExpandButtonUnclicked);

		// Create a rect with a CenterLayout in order to keep the button centered.
		RectComponent* rect = new RectComponent(new CenterLayout());
		rect->AddChild(expand_button_);

		rect->SetPreferredSize(expand_button_->GetPreferredSize());
		rect->SetMinSize(expand_button_->GetMinSize());

		if (((GridLayout*)GetLayout())->GetNumCols() > 1) {
			Component* comp = ((GridLayout*)GetLayout())->GetComponentAt(0, 0);
			((GridLayout*)GetLayout())->DeleteColumn(0);
			delete comp;
		}
		((GridLayout*)GetLayout())->InsertColumn(0);
		AddChild(rect, 0, 0);

		repaint = true;
	} else if (child_nodes_ == 0 && expand_button_ == 0) {
		GUIImageManager* i_man = GetImageManager();

		RectComponent* rect = new RectComponent(new CenterLayout());

		rect->SetPreferredSize(i_man->GetImageSize(expand_icon_id_));
		rect->SetMinSize(rect->GetPreferredSize());

		((GridLayout*)GetLayout())->InsertColumn(0);
		AddChild(rect, 0, 0);
	}

	return repaint;
}

bool TreeNode::ValidateIconRect(const wstr& text) {
	bool repaint = false;

	if (icon_label_ == 0) {
		GUIImageManager* i_man = GetImageManager();

		//icon_label_ = new Label(factory_text_backg_color_, factory_text_selected_backg_color_);
		icon_label_ = new Label(factory_text_color_, text);
		icon_label_->SetIcon(collapsed_icon_id_, Label::kIconLeft);

		/*icon_label_->SetText(text,
			factory_text_color_, factory_text_backg_color_,
			factory_text_selected_color_, factory_text_selected_backg_color_);*/

		int image_height = i_man->GetImageSize(collapsed_icon_id_).y;
		int font_height = i_man->GetPainter()->GetFontHeight();
		icon_label_->SetPreferredHeight(image_height > font_height ? image_height : font_height);

		if (expand_button_) {
			// The layout is 1x2.
			AddChild(icon_label_, 0, 1);
		} else {
			// The layout is 1x1.
			AddChild(icon_label_, 0, 0);
		}

		repaint = true;
	}

	return repaint;
}

bool TreeNode::OnDoubleClick(int mouse_x, int mouse_y) {
	if (IsExpandable() && icon_label_->IsOver(mouse_x, mouse_y) == true) {
		bool _expanded = !IsExpanded();
		expand_button_->SetPressed(_expanded);
		SetExpanded(_expanded);
	}
	return (false);
}

bool TreeNode::OnKeyDown(uilepra::InputManager::KeyCode key_code) {
	Parent::OnKeyDown(key_code);

	switch(key_code) {
		case uilepra::InputManager::kInKbdLeft: {
			if (IsExpandable() && IsExpanded()) {
				expand_button_->SetPressed(false);
				SetExpanded(false);
			}
		} break;
		case uilepra::InputManager::kInKbdRight: {
			if (IsExpandable() && IsExpanded() == false) {
				expand_button_->SetPressed(true);
				SetExpanded(true);
			}
		} break;
		default: break;
	}
	return (false);
}


void TreeNode::UseFont(const Color& text_color,
		       const Color& text_backg_color,
		       const Color& text_selected_color,
		       const Color& text_selected_backg_color) {
	factory_text_color_ = text_color;
	factory_text_backg_color_ = text_backg_color;
	factory_text_selected_color_ = text_selected_color;
	factory_text_selected_backg_color_ = text_selected_backg_color;
}


Painter::ImageID TreeNode::expand_icon_id__ = Painter::kInvalidImageid;
Painter::ImageID TreeNode::collapse_icon_id__ = Painter::kInvalidImageid;
Painter::ImageID TreeNode::expanded_icon_id__ = Painter::kInvalidImageid;
Painter::ImageID TreeNode::collapsed_icon_id__ = Painter::kInvalidImageid;
Painter* TreeNode::prev_painter_ = 0;

uint8 TreeNode::icon_expand_[] =
{
	106, 106, 106, 106, 106, 106, 106, 106, 106,   0,   0,   0,   0,   0,   0,   0,
	106,   3,   3,   3,   3,   3,   3,   3, 106,   0,   0,   0,   0,   0,   0,   0,
	106,   3,   3,   3, 255,   3,   3,   3, 106,   0,   0,   0,   0,   0,   0,   0,
	106,   3,   3,   3, 255,   3,   3,   3, 106,   0,   0,   0,   0,   0,   0,   0,
	106,   3, 255, 255, 255, 255, 255,   3, 106,   0,   0,   0,   0,   0,   0,   0,
	106,   3,   3,   3, 255,   3,   3,   3, 106,   0,   0,   0,   0,   0,   0,   0,
	106,   3,   3,   3, 255,   3,   3,   3, 106,   0,   0,   0,   0,   0,   0,   0,
	106,   3,   3,   3,   3,   3,   3,   3, 106,   0,   0,   0,   0,   0,   0,   0,
	106, 106, 106, 106, 106, 106, 106, 106, 106,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
};

uint8 TreeNode::icon_collapse_[] =
{
	106, 106, 106, 106, 106, 106, 106, 106, 106,   0,   0,   0,   0,   0,   0,   0,
	106,   3,   3,   3,   3,   3,   3,   3, 106,   0,   0,   0,   0,   0,   0,   0,
	106,   3,   3,   3,   3,   3,   3,   3, 106,   0,   0,   0,   0,   0,   0,   0,
	106,   3,   3,   3,   3,   3,   3,   3, 106,   0,   0,   0,   0,   0,   0,   0,
	106,   3, 255, 255, 255, 255, 255,   3, 106,   0,   0,   0,   0,   0,   0,   0,
	106,   3,   3,   3,   3,   3,   3,   3, 106,   0,   0,   0,   0,   0,   0,   0,
	106,   3,   3,   3,   3,   3,   3,   3, 106,   0,   0,   0,   0,   0,   0,   0,
	106,   3,   3,   3,   3,   3,   3,   3, 106,   0,   0,   0,   0,   0,   0,   0,
	106, 106, 106, 106, 106, 106, 106, 106, 106,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
};

uint8 TreeNode::icon_collapsed_[] =
{
	  0,   1,   1,   1,   1,   1,   1,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  1,   2,   2,   2,   2,   2,   2,   1,   0,   0,   0,   0,   0,   0,   0,   0,
	  1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
	  1,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   1,
	  1,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   1,
	  1,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   1,
	  1,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   1,
	  1,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   1,
	  1,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   1,
	  1,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   1,
	  1,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   1,
	  1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
};

uint8 TreeNode::icon_expanded_[] =
{
	  0,   1,   1,   1,   1,   1,   1,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  1,   2,   2,   2,   2,   2,   2,   1,   0,   0,   0,   0,   0,   0,   0,   0,
	  1,   2,   2,   2,   2,   2,   2,   1,   1,   1,   1,   1,   1,   0,   0,   0,
	  1,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   1,   0,   0,   0,
	  1,   2,   2,   2,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
	  1,   2,   2,   2,   1,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   1,
	  1,   2,   2,   1,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   1,   0,
	  1,   2,   2,   1,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   1,   0,
	  1,   2,   1,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   1,   0,   0,
	  1,   2,   1,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   1,   0,   0,
	  1,   1,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   1,   0,   0,   0,
	  1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
};

Color TreeNode::factory_text_color_              = BLACK;
Color TreeNode::factory_text_backg_color_         = WHITE;
Color TreeNode::factory_text_selected_color_      = BLACK;
Color TreeNode::factory_text_selected_backg_color_ = LIGHT_BLUE;


}
