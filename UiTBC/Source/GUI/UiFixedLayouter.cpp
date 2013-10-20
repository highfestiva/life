
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../../Include/GUI/UiFixedLayouter.h"
#include "../../Include/GUI/UiDialog.h"



namespace UiTbc
{



FixedLayouter::FixedLayouter(Dialog* pParent):
	mDialog(pParent),
	mContentWidthPart(0.8f),
	mContentHeightPart(0.8f),
	mMaxRowCount(1),
	mContentXMargin(10),
	mContentYMargin(10)
{
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

void FixedLayouter::SetContentXMargin(int pContentXMargin)
{
	mContentXMargin = pContentXMargin;
}

void FixedLayouter::SetContentYMargin(int pContentYMargin)
{
	mContentYMargin = pContentYMargin;
}

void FixedLayouter::AddComponent(Component* pComponent, int r, int rc, int c, int cc)
{
	mMaxRowCount = std::max(mMaxRowCount, rc);

	pComponent->UpdateLayout();
	mDialog->AddChild(pComponent);

	const float lFullWidth  = mContentWidthPart  * mDialog->GetPreferredWidth();
	const float lFullHeight = mContentHeightPart * mDialog->GetPreferredHeight();
	const float lLeft = mDialog->GetPreferredWidth()/2 - lFullWidth/2;
	const float lTop = mDialog->GetPreferredHeight()/2 - lFullHeight/2;
	const float lFullColWidth  = lFullWidth  / cc;
	const float lFullRowHeight = lFullHeight / rc;
	const float lXMarginCount = cc-1.0f;
	const float lYMarginCount = rc-1.0f;
	const float lComponentWidth  = lFullColWidth  - lXMarginCount*mContentXMargin/cc;
	const float lComponentHeight = lFullRowHeight - lYMarginCount*mContentYMargin/rc;
	pComponent->SetPos(int(lLeft + c*lFullColWidth), int(lTop + r*lFullRowHeight));
	pComponent->SetSize((int)lComponentWidth, (int)lComponentHeight);
	pComponent->SetPreferredSize((int)lComponentWidth, (int)lComponentHeight);
}

void FixedLayouter::AddWindow(Window* pWindow, int r, int rc, int c, int cc)
{
	AddComponent(pWindow, r, rc, c, cc);
	pWindow->SetRoundedRadius(pWindow->GetPreferredHeight() / 3);
}

void FixedLayouter::AddButton(Button* pButton, int pTag, int r, int rc, int c, int cc, bool pAutoDismiss)
{
	deb_assert(pTag < 0);	// Otherwise dialog auto-layouts.
	AddWindow(pButton, r, rc, c, cc);
	pButton->SetHorizontalMargin(pButton->GetPreferredHeight() / 3);
	mDialog->SetButtonHandler(pTag, pButton, pAutoDismiss);
}

void FixedLayouter::AddCornerButton(Button* pCornerButton, int pTag)
{
	AddButton(pCornerButton, pTag, 0, mMaxRowCount, 0, 1, true);
	const int lMinSize = pCornerButton->GetRoundedRadius()*2+1;
	pCornerButton->SetPreferredSize(lMinSize, lMinSize);
	PixelCoord lCloseButtonPos = mDialog->GetSize();
	const int r = mDialog->GetCornerRadius();
	lCloseButtonPos.x += -lMinSize/2 - int(0.293*r);	// 0.293 ~ 1-1/sqrt(2)
	lCloseButtonPos.y  = -lMinSize/2 + int(0.293*r);
	pCornerButton->SetPos(lCloseButtonPos);
}



}
