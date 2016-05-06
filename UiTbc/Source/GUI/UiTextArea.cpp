
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../../lepra/include/math.h"
#include "../../../lepra/include/lepraassert.h"
#include "../../../lepra/include/listutil.h"
#include "../../include/gui/uitextarea.h"


namespace uitbc {



TextArea::TextArea(const Color& color):
	Parent(0, 1, color),
	first_visible_line_(0),
	visible_line_count_(0),
	line_height_(0),
	focus_anchor_(kAnchorTopLine),
	scroll_lock_(false),
	max_line_count_(50) {
	horizontal_margin_ = 0;
	Init();
}

TextArea::TextArea(Painter::ImageID image_id):
	Parent(0, 1, image_id),
	first_visible_line_(0),
	visible_line_count_(0),
	line_height_(0),
	focus_anchor_(kAnchorTopLine),
	scroll_lock_(false),
	max_line_count_(50) {
	horizontal_margin_ = 0;
	Init();
}

TextArea::~TextArea() {
}



void TextArea::Clear() {
	ScopeLock lock(&lock_);
	first_visible_line_ = 0;
	visible_line_count_ = 0;
	line_height_ = 0;
	line_list_.clear();
	SetNeedsRepaint(true);
}

bool TextArea::InsertLine(unsigned line_index, const wstr& text, Color* color) {
	ScopeLock lock(&lock_);
	bool ok = false;
	if (line_index <= GetLineCount()) {
		ok = true;
		LineInfo line_info;
		line_info.text_ = text;
		if (color) {
			line_info.color_ = *color;
		} else {
			line_info.color_ = GetTextColor();
		}
		line_list_.insert(GetIterator(line_index), line_info);
		if (GetLineCount() > max_line_count_) {
			line_list_.pop_front();
		}
		// Not scroll lock and added the last line.
		if (!scroll_lock_ && line_index >= GetLineCount()-1) {
			ScrollToLastLine();
		}
	}
	RequestRepaint();
	return (ok);
}

unsigned TextArea::AddLine(const wstr& text, Color* color) {
	InsertLine(GetLineCount(), text, color);
	return (GetLineCount()-1);
}

void TextArea::AddText(const wstr& text, Color* color) {
	ScopeLock lock(&lock_);
	bool first_line = true;
	wstrutil::strvec lines = wstrutil::Split(text, L"\n");
	for (unsigned x = 0; x < lines.size(); ++x) {
		if (first_line) {
			first_line = false;
			if (lines[x].empty()) {
			} else if (GetLineCount() <= 0) {
				AddLine(text, color);
			} else {
				line_list_.back().text_ += lines[x];
				if (color) {
					line_list_.back().color_ = *color;
				}
			}
		} else {
			AddLine(lines[x], color);
		}
	}
	RequestRepaint();
}

bool TextArea::RemoveLine(unsigned line_index) {
	ScopeLock lock(&lock_);
	bool ok = (line_index < GetLineCount());
	if (ok) {
		line_list_.erase(GetIterator(line_index));
		if (line_index < first_visible_line_) {
			--first_visible_line_;
		}
		RequestRepaint();
	}
	return (ok);
}

unsigned TextArea::GetLineCount() const {
	ScopeLock lock(&lock_);
	return ((unsigned)line_list_.size());
}

unsigned TextArea::GetFirstVisibleLineIndex() const {
	return (first_visible_line_);
}

void TextArea::SetFirstVisibleLineIndex(unsigned line_index) {
	int max_down_index = Math::Clamp(
		(int)(GetLineCount()-GetVisibleLineCount()), 0, (int)max_line_count_);
	line_index = Math::Clamp((int)line_index, 0, max_down_index);
	if (line_index != first_visible_line_) {
		first_visible_line_ = line_index;
		RequestRepaint();
	}
}

unsigned TextArea::GetVisibleLineCount() const {
	return (visible_line_count_);
}



void TextArea::SetFocusAnchor(FocusAnchor anchor) {
	focus_anchor_ = anchor;
}

void TextArea::SetScrollLock(bool scroll_lock) {
	scroll_lock_ = scroll_lock;
}

void TextArea::SetMaxLineCount(unsigned max_line_count) {
	max_line_count_ = max_line_count;
	while (max_line_count_ > GetLineCount()) {
		RemoveLine(0);
	}
	RequestRepaint();
}



void TextArea::Repaint(Painter* painter) {
	Parent::Repaint(painter);

	ActivateFont(painter);
	line_height_ = painter->GetLineHeight();
	UpdateVisibleSize();

	painter->PushAttrib(Painter::kAttrAll);

	PixelRect rect(GetClientRect());
	painter->ReduceClippingRect(rect);

	int begin_y;
	int end_y;
	int pixel_offset_y;
	int printed_line_count = Math::Clamp((int)GetVisibleLineCount(), 0, (int)GetLineCount());
	if (focus_anchor_ == kAnchorTopLine) {
		begin_y = 0;
		end_y = printed_line_count;
		pixel_offset_y = 0;
	} else {
		begin_y = -printed_line_count;
		end_y = 0;
		pixel_offset_y = rect.GetHeight();
	}
	ScopeLock lock(&lock_);
	TextLineList::iterator s = GetIterator(GetFirstVisibleLineIndex());
	for (int y = begin_y; s != line_list_.end() && y < end_y; ++y, ++s) {
		painter->SetColor(s->color_);
		painter->PrintText(s->text_, rect.left_+horizontal_margin_, rect.top_+y*line_height_+pixel_offset_y);
	}

	painter->PopAttrib();
	DeactivateFont(painter);
}



void TextArea::ForceRepaint() {
	SetNeedsRepaint(true);
}



TextArea::TextLineList::iterator TextArea::GetIterator(unsigned line_index) {
	deb_assert(line_index <= GetLineCount());
	return ListUtil::FindByIndex(line_list_, line_index);
}



void TextArea::DoSetSize(int width, int height) {
	Parent::DoSetSize(width, height);
	UpdateVisibleSize();
}

void TextArea::UpdateVisibleSize() {
	visible_line_count_ = 0;
	if (line_height_) {
		visible_line_count_ = (GetSize().y+line_height_-1)/line_height_;
		if (!scroll_lock_) {
			ScrollToLastLine();
		}
	}
}

void TextArea::ScrollToLastLine() {
	int scroll_down_to_index = Math::Clamp((int)(GetLineCount()-GetVisibleLineCount()), 0, (int)max_line_count_);
	SetFirstVisibleLineIndex(scroll_down_to_index);
}



}
