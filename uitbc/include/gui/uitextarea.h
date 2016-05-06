
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include <list>
#include "../../../lepra/include/thread.h"
#include "uitextcomponent.h"
#include "uiwindow.h"



namespace uitbc {



// Currently a non-editable text area (split in lines). Works good for consoles.
class TextArea: public Window, public TextComponent {
public:
	typedef Window Parent;

	enum FocusAnchor {
		kAnchorTopLine,
		kAnchorBottomLine,
	};

	TextArea(const Color& color);
	TextArea(Painter::ImageID image_id);
	virtual ~TextArea();

	void Clear();
	bool InsertLine(unsigned line_index, const wstr& text, Color* color = 0);
	unsigned AddLine(const wstr& text, Color* color = 0);
	void AddText(const wstr& text, Color* color = 0);
	bool RemoveLine(unsigned line_index);
	unsigned GetLineCount() const;
	unsigned GetFirstVisibleLineIndex() const;
	void SetFirstVisibleLineIndex(unsigned line_index);
	unsigned GetVisibleLineCount() const;

	// Set a top anchor for normal text viewing, set bottom anchor for console listings.
	void SetFocusAnchor(FocusAnchor anchor);
	void SetScrollLock(bool scroll_lock);
	void SetMaxLineCount(unsigned max_line_count);

	void Repaint(Painter* painter);

protected:
	void ForceRepaint();

private:
	struct LineInfo {
		wstr text_;
		Color color_;
	};
	typedef std::list<LineInfo> TextLineList;

	TextLineList::iterator GetIterator(unsigned line_index);

	virtual void DoSetSize(int width, int height);
	void UpdateVisibleSize();
	void ScrollToLastLine();

	mutable Lock lock_;
	TextLineList line_list_;
	unsigned first_visible_line_;
	unsigned visible_line_count_;
	unsigned line_height_;
	FocusAnchor focus_anchor_;
	bool scroll_lock_;
	unsigned max_line_count_;
};



}
