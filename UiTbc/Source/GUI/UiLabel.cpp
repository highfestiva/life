
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../include/gui/uilabel.h"
#include <algorithm>
#include "../../include/gui/uidesktopwindow.h"
#include "../../include/uipainter.h"



namespace uitbc {



Label::Label(const Color& color, const wstr& text):
	Parent(color),
	icon_id_(Painter::kInvalidImageid),
	icon_alignment_(kIconRight),
	text_(text),
	text_width_(0),
	text_height_(0),
	selectable_(false) {
	SetFontColor(color);
	SetIsHollow(true);
	SetName(strutil::Encode(text));
}

Label::~Label() {
}

void Label::SetIcon(Painter::ImageID icon_id, IconAlignment alignment) {
	icon_id_ = icon_id;
	icon_alignment_ = alignment;
}

void Label::SetText(const wstr& text) {
	text_ = text;
}

const wstr& Label::GetText() const {
	return text_;
}

void Label::SetSelected(bool selected) {
	if (selectable_ == true && selected != GetSelected()) {
		Parent::SetSelected(selected);
		UpdateBackground();
	}
}

void Label::Repaint(Painter* painter) {
	SetNeedsRepaint(false);

	ActivateFont(painter);
	const int text_width  = painter->GetStringWidth(text_);
	const int text_height = painter->GetLineHeight() * (std::count(text_.begin(), text_.end(), '\n') + 1);
	if (text_width_ != text_width || text_height_ != text_height) {
		text_width_ = text_width;
		text_height_ = text_height;
		GetParent()->UpdateLayout();
	}

	Parent::Repaint(painter);

	GUIImageManager* i_man = GetImageManager();

	PixelCoord pos(GetScreenPos());
	PixelRect rect(pos, pos + GetSize());
/*#ifndef LEPRA_TOUCH
	painter->ReduceClippingRect(rect);
#endif // !touch*/

	int text_y = 0;
	switch (GetVAlign()) {
		case kValignTop:	text_y = rect.top_;						break;
		case kValignCenter:	text_y = rect.top_ + (rect.GetHeight() - text_height_) / 2;	break;
		case kValignBottom:	text_y = rect.bottom_ - text_height_;				break;
	}

	int text_x = rect.left_ + horizontal_margin_;
	switch (icon_alignment_) {
		case kIconCenter:
			text_x = rect.GetCenterX() - painter->GetStringWidth(text_)/2;
		break;
		case kIconLeft:
			if (icon_id_ == Painter::kInvalidImageid) {
				// No icon, but left-aligned indication means text should be right-aligned.
				text_x = rect.right_ - painter->GetStringWidth(text_);
			} break;
	}

	if (icon_id_ != Painter::kInvalidImageid) {
		PixelCoord image_size(i_man->GetImageSize(icon_id_));

		int x = 0;
		int y = 0;
		switch (icon_alignment_) {
			case kIconLeft:
				x = rect.left_;
				y = rect.top_ + (rect.GetHeight() - image_size.y) / 2;
				text_x = rect.left_ + image_size.x + horizontal_margin_;
			break;
			case kIconCenter:
				x = rect.left_ + (rect.GetWidth()  - image_size.x) / 2;
				if (!text_.empty()) {
					y = rect.top_;
				} else {
					y = rect.GetCenterY() - image_size.y/2;
				} break;
			case kIconRight:
				x = rect.right_ - image_size.x - horizontal_margin_;
				y = rect.top_ + (rect.GetHeight() - image_size.y) / 2;
			break;
		}
		i_man->DrawImage(icon_id_, x, y);
	}

	RepaintComponents(painter);

	painter->SetColor(GetTextColor(), 0);
	painter->PrintText(text_, text_x, text_y);

	DeactivateFont(painter);
}

void Label::ForceRepaint() {
	SetNeedsRepaint(true);
}

PixelCoord Label::GetPreferredSize(bool force_adaptive) {
	GUIImageManager* i_man = GetImageManager();

	PixelCoord lIconSize(0, 0);
	if (icon_id_ != Painter::kInvalidImageid) {
		lIconSize = i_man->GetImageSize(icon_id_);
	}

	PixelCoord size(Parent::GetPreferredSize());

	if (force_adaptive == true || IsAdaptive() == true) {
		size.x = lIconSize.x + text_width_;
		size.y = std::max(lIconSize.y, text_height_);
	}

	return size;
}

Component::Type Label::GetType() const {
	return Component::kLabel;
}

void Label::UpdateBackground() {
}



}
