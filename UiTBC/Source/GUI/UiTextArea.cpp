
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../../../Lepra/Include/Math.h"
#include "../../../Lepra/Include/LepraAssert.h"
#include "../../../Lepra/Include/ListUtil.h"
#include "../../Include/GUI/UiTextArea.h"


namespace UiTbc
{



TextArea::TextArea(const Color& pColor, const str& pName):
	Parent(0, 1, pColor, pName),
	mFirstVisibleLine(0),
	mVisibleLineCount(0),
	mLineHeight(0),
	mFocusAnchor(ANCHOR_TOP_LINE),
	mScrollLock(false),
	mMaxLineCount(50)
{
}

TextArea::TextArea(Painter::ImageID pImageId, const str& pName):
	Parent(0, 1, pImageId, pName),
	mFirstVisibleLine(0),
	mVisibleLineCount(0),
	mLineHeight(0),
	mFocusAnchor(ANCHOR_TOP_LINE),
	mScrollLock(false),
	mMaxLineCount(50)
{
}

TextArea::~TextArea()
{
}



void TextArea::Clear()
{
	ScopeLock lLock(&mLock);
	mFirstVisibleLine = 0;
	mVisibleLineCount = 0;
	mLineHeight = 0;
	mLineList.clear();
	SetNeedsRepaint(true);
}

bool TextArea::InsertLine(unsigned pLineIndex, const str& pText, Color* pColor)
{
	ScopeLock lLock(&mLock);
	bool lOk = false;
	if (pLineIndex <= GetLineCount())
	{
		lOk = true;
		LineInfo lLineInfo;
		lLineInfo.mText = pText;
		if (pColor)
		{
			lLineInfo.mColor = *pColor;
		}
		else
		{
			lLineInfo.mColor = GetTextColor();
		}
		mLineList.insert(GetIterator(pLineIndex), lLineInfo);
		if (GetLineCount() > mMaxLineCount)
		{
			mLineList.pop_front();
		}
		// Not scroll lock and added the last line.
		if (!mScrollLock && pLineIndex >= GetLineCount()-1)
		{
			ScrollToLastLine();
		}
	}
	RequestRepaint();
	return (lOk);
}

unsigned TextArea::AddLine(const str& pText, Color* pColor)
{
	InsertLine(GetLineCount(), pText, pColor);
	return (GetLineCount()-1);
}

void TextArea::AddText(const str& pText, Color* pColor)
{
	ScopeLock lLock(&mLock);
	bool lFirstLine = true;
	strutil::strvec lLines = strutil::Split(pText, _T("\n"));
	for (unsigned x = 0; x < lLines.size(); ++x)
	{
		if (lFirstLine)
		{
			lFirstLine = false;
			if (lLines[x].empty())
			{
			}
			else if (GetLineCount() <= 0)
			{
				AddLine(pText, pColor);
			}
			else
			{
				mLineList.back().mText += lLines[x];
				if (pColor)
				{
					mLineList.back().mColor = *pColor;
				}
			}
		}
		else
		{
			AddLine(lLines[x], pColor);
		}
	}
	RequestRepaint();
}

bool TextArea::RemoveLine(unsigned pLineIndex)
{
	ScopeLock lLock(&mLock);
	bool lOk = (pLineIndex < GetLineCount());
	if (lOk)
	{
		mLineList.erase(GetIterator(pLineIndex));
		if (pLineIndex < mFirstVisibleLine)
		{
			--mFirstVisibleLine;
		}
		RequestRepaint();
	}
	return (lOk);
}

unsigned TextArea::GetLineCount() const
{
	ScopeLock lLock(&mLock);
	return ((unsigned)mLineList.size());
}

unsigned TextArea::GetFirstVisibleLineIndex() const
{
	return (mFirstVisibleLine);
}

void TextArea::SetFirstVisibleLineIndex(unsigned pLineIndex)
{
	int lMaxDownIndex = Math::Clamp(
		(int)(GetLineCount()-GetVisibleLineCount()), 0, (int)mMaxLineCount);
	pLineIndex = Math::Clamp((int)pLineIndex, 0, lMaxDownIndex);
	if (pLineIndex != mFirstVisibleLine)
	{
		mFirstVisibleLine = pLineIndex;
		RequestRepaint();
	}
}

unsigned TextArea::GetVisibleLineCount() const
{
	return (mVisibleLineCount);
}



void TextArea::SetFocusAnchor(FocusAnchor pAnchor)
{
	mFocusAnchor = pAnchor;
}

void TextArea::SetScrollLock(bool pScrollLock)
{
	mScrollLock = pScrollLock;
}

void TextArea::SetMaxLineCount(unsigned pMaxLineCount)
{
	mMaxLineCount = pMaxLineCount;
	while (mMaxLineCount > GetLineCount())
	{
		RemoveLine(0);
	}
	RequestRepaint();
}



void TextArea::Repaint(Painter* pPainter)
{
	Parent::Repaint(pPainter);

	ActivateFont(pPainter);
	mLineHeight = pPainter->GetLineHeight();
	UpdateVisibleSize();

	pPainter->PushAttrib(Painter::ATTR_ALL);

	PixelRect lRect(GetClientRect());
	pPainter->ReduceClippingRect(lRect);

	int lBeginY;
	int lEndY;
	int lPixelOffsetY;
	int lPrintedLineCount = Math::Clamp((int)GetVisibleLineCount(), 0, (int)GetLineCount());
	if (mFocusAnchor == ANCHOR_TOP_LINE)
	{
		lBeginY = 0;
		lEndY = lPrintedLineCount;
		lPixelOffsetY = 0;
	}
	else
	{
		lBeginY = -lPrintedLineCount;
		lEndY = 0;
		lPixelOffsetY = lRect.GetHeight();
	}
	ScopeLock lLock(&mLock);
	TextLineList::iterator s = GetIterator(GetFirstVisibleLineIndex());
	for (int y = lBeginY; s != mLineList.end() && y < lEndY; ++y, ++s)
	{
		pPainter->SetColor(s->mColor);
		pPainter->PrintText(s->mText, lRect.mLeft, lRect.mTop+y*mLineHeight+lPixelOffsetY);
	}

	pPainter->PopAttrib();
	DeactivateFont(pPainter);
}



void TextArea::ForceRepaint()
{
	SetNeedsRepaint(true);
}



TextArea::TextLineList::iterator TextArea::GetIterator(unsigned pLineIndex)
{
	deb_assert(pLineIndex <= GetLineCount());
	return ListUtil::FindByIndex(mLineList, pLineIndex);
}



void TextArea::DoSetSize(int pWidth, int pHeight)
{
	Parent::DoSetSize(pWidth, pHeight);
	UpdateVisibleSize();
}

void TextArea::UpdateVisibleSize()
{
	mVisibleLineCount = 0;
	if (mLineHeight)
	{
		mVisibleLineCount = (GetSize().y+mLineHeight-1)/mLineHeight;
		if (!mScrollLock)
		{
			ScrollToLastLine();
		}
	}
}

void TextArea::ScrollToLastLine()
{
	int lScrollDownToIndex = Math::Clamp((int)(GetLineCount()-GetVisibleLineCount()), 0, (int)mMaxLineCount);
	SetFirstVisibleLineIndex(lScrollDownToIndex);
}



}
