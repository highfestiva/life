
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include <list>
#include "../../../Lepra/Include/Thread.h"
#include "UiTextComponent.h"
#include "UiWindow.h"



namespace UiTbc
{



// Currently a non-editable text area (split in lines). Works good for consoles.
class TextArea: public Window, public TextComponent
{
public:
	typedef Window Parent;

	enum FocusAnchor
	{
		ANCHOR_TOP_LINE,
		ANCHOR_BOTTOM_LINE,
	};

	TextArea(const Color& pColor);
	TextArea(Painter::ImageID pImageId);
	virtual ~TextArea();

	void Clear();
	bool InsertLine(unsigned pLineIndex, const wstr& pText, Color* pColor = 0);
	unsigned AddLine(const wstr& pText, Color* pColor = 0);
	void AddText(const wstr& pText, Color* pColor = 0);
	bool RemoveLine(unsigned pLineIndex);
	unsigned GetLineCount() const;
	unsigned GetFirstVisibleLineIndex() const;
	void SetFirstVisibleLineIndex(unsigned pLineIndex);
	unsigned GetVisibleLineCount() const;

	// Set a top anchor for normal text viewing, set bottom anchor for console listings.
	void SetFocusAnchor(FocusAnchor pAnchor);
	void SetScrollLock(bool pScrollLock);
	void SetMaxLineCount(unsigned pMaxLineCount);

	void Repaint(Painter* pPainter);

protected:
	void ForceRepaint();

private:
	struct LineInfo
	{
		wstr mText;
		Color mColor;
	};
	typedef std::list<LineInfo> TextLineList;

	TextLineList::iterator GetIterator(unsigned pLineIndex);

	virtual void DoSetSize(int pWidth, int pHeight);
	void UpdateVisibleSize();
	void ScrollToLastLine();

	mutable Lock mLock;
	TextLineList mLineList;
	unsigned mFirstVisibleLine;
	unsigned mVisibleLineCount;
	unsigned mLineHeight;
	FocusAnchor mFocusAnchor;
	bool mScrollLock;
	unsigned mMaxLineCount;
};



}
