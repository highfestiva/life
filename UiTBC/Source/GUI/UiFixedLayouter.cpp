
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../Include/GUI/UiFixedLayouter.h"
#include "../../Include/GUI/UiDialog.h"



namespace UiTbc
{



FixedLayouter::FixedLayouter(Dialog* pParent):
	mDialog(pParent),
	mContentWidthPart(0.7f),
	mContentHeightPart(0.7f),
	mMaxRowCount(1),
	mContentXMargin(0),
	mContentYMargin(0)
{
	const int lMargin = pParent->GetPreferredHeight() / 30;
	SetContentMargin(lMargin);
}

FixedLayouter::~FixedLayouter()
{
	mDialog = 0;
}

void FixedLayouter::SetContentWidthPart(float pContentWidthPart)
{
	mContentWidthPart = pContentWidthPart;
}

void FixedLayouter::SetContentHeightPart(float pContentHeightPart)
{
	mContentHeightPart = pContentHeightPart;
}

void FixedLayouter::SetContentMargin(int pContentMargin)
{
	SetContentXMargin(pContentMargin);
	SetContentYMargin(pContentMargin);
}

int FixedLayouter::GetContentXMargin() const
{
	return mContentXMargin;
}

void FixedLayouter::SetContentXMargin(int pContentXMargin)
{
	mContentXMargin = pContentXMargin;
}

int FixedLayouter::GetContentYMargin() const
{
	return mContentYMargin;
}

void FixedLayouter::SetContentYMargin(int pContentYMargin)
{
	mContentYMargin = pContentYMargin;
}

void FixedLayouter::AddComponent(Component* pComponent, int r, int rc, int c, int cw, int cc)
{
	mMaxRowCount = std::max(mMaxRowCount, rc);

	pComponent->UpdateLayout();
	mDialog->AddChild(pComponent);

	const float lFullWidth  = mContentWidthPart  * mDialog->GetPreferredWidth();
	const float lFullHeight = mContentHeightPart * mDialog->GetPreferredHeight();
	const float lLeft = mDialog->GetPreferredWidth()/2 - lFullWidth/2;
	const float lTop = mDialog->GetPreferredHeight()/2 - lFullHeight/2;
	const float lSplitColWidth  = lFullWidth  / cc;
	const float lSplitRowHeight = lFullHeight / rc;
	const float lXMarginCount = float(cc-cw);
	const float lYMarginCount = rc-1.0f;
	const float lComponentWidth  = lSplitColWidth  - lXMarginCount*mContentXMargin/cc;
	const float lComponentHeight = lSplitRowHeight - lYMarginCount*mContentYMargin/rc;
	const float lColWidthAndMargin  = lSplitColWidth  + mContentXMargin/2;	// Divide by two as half of the margin is already included in the evenly split size.
	const float lRowHeightAndMargin = lSplitRowHeight + mContentYMargin/2;	// Divide by two as half of the margin is already included in the evenly split size.
	pComponent->SetPos(int(lLeft + c*lColWidthAndMargin), int(lTop + r*lRowHeightAndMargin));
	pComponent->SetSize(int(cw*lComponentWidth), (int)lComponentHeight);
	pComponent->SetPreferredSize(pComponent->GetSize(), false);
}

void FixedLayouter::AddWindow(Window* pWindow, int r, int rc, int c, int cw, int cc)
{
	AddComponent(pWindow, r, rc, c, cw, cc);
	pWindow->SetRoundedRadius(pWindow->GetPreferredHeight() / 3);
}

void FixedLayouter::AddButton(Button* pButton, int pTag, int r, int rc, int c, int cw, int cc, bool pAutoDismiss)
{
	deb_assert(pTag < 0);	// Otherwise dialog auto-layouts.
	AddWindow(pButton, r, rc, c, cw, cc);
	pButton->SetHorizontalMargin(pButton->GetPreferredHeight() / 3);
	mDialog->SetButtonHandler(pTag, pButton, pAutoDismiss);
}

void FixedLayouter::AddCornerButton(Button* pCornerButton, int pTag)
{
	AddButton(pCornerButton, pTag, 0, 2, 0, 1, 1, true);
	const int lMinSize = pCornerButton->GetRoundedRadius()*2+1;
	pCornerButton->SetPreferredSize(lMinSize, lMinSize);
	PixelCoord lCloseButtonPos = mDialog->GetSize();
	const int r = mDialog->GetCornerRadius();
	lCloseButtonPos.x += -lMinSize/2 - int(0.293*r);	// 0.293 ~ 1-1/sqrt(2)
	lCloseButtonPos.y  = -lMinSize/2 + int(0.293*r);
	pCornerButton->SetPos(lCloseButtonPos);
}



}
