
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../Include/GUI/UiDialog.h"
#include "../../Include/GUI/UiDesktopWindow.h"

#define SPEED		30
#define ACCELERATION	20
#define MARGIN		0



namespace UiTbc
{



Dialog::Dialog(Component* pParent, Action pTarget):
	Parent(WHITE, new FloatingLayout),
	mLabel(0),
	mTarget(pTarget),
	mClickedButton(0),
	mIsClosing(false),
	mAnimationStep(0),
	mDirection(0)
{
	SetCornerRadius(20);
	mColor[1] = DARK_GRAY;
	pParent->AddChild(this);

	const PixelCoord& lParentSize = GetParent()->GetSize();
	PixelCoord lSize = lParentSize - PixelCoord(100, 100);
	SetSize(lSize);
	SetPreferredSize(lSize);

	SetDirection(+1, true);
}

Dialog::~Dialog()
{
	mTarget.clear();
	mPreClickTarget.clear();
	mPostClickTarget.clear();
}

void Dialog::Dismiss()
{
	if (!mIsClosing)
	{
		GetTopParent()->ReleaseKeyboardFocus(RECURSE_DOWN);
		mAnimationStep = -SPEED * mDirection;
		mIsClosing = true;
	}
}



void Dialog::SetColor(const Color& pTopLeftColor, const Color& pTopRightColor, const Color& pBottomRightColor, const Color& pBottomLeftColor)
{
	Parent::SetColor(pTopLeftColor, pTopRightColor, pBottomRightColor, pBottomLeftColor);
	if (mLabel)
	{
		mLabel->SetText(mLabel->GetText());
	}
}

void Dialog::SetDirection(int pDirection, bool pSetPos)
{
	mDirection = pDirection;

	const PixelCoord& lParentSize = GetParent()->GetSize();
	const PixelCoord& lSize = GetSize();
	const int lStartX = (mDirection > 0)? lParentSize.x+MARGIN : -lSize.x-MARGIN;

	// Calculate what animation speed is required to get right end speed using the acceleration.
	const int lTargetX = lParentSize.x/2 - lSize.x/2;
	int x = lTargetX;
	mAnimationStep = -SPEED * mDirection;
	for (int i = 0; i < 1000; ++i)
	{
		x -= mAnimationStep;
		if ((mDirection > 0)? (x >= lStartX) : (x <= lStartX))
		{
			break;
		}
		mAnimationStep -= ACCELERATION * mDirection;
	}
	if (pSetPos)
	{
		SetPos(x, lParentSize.y/2 - lSize.y/2);
	}
}

void Dialog::SetPreClickTarget(Action pPreClickTarget)
{
	mPreClickTarget = pPreClickTarget;
}

void Dialog::SetPostClickTarget(Action pPostClickTarget)
{
	mPostClickTarget = pPostClickTarget;
}

void Dialog::Center()
{
	if (mAnimationStep)
	{
		return;
	}
	const PixelCoord& lParentSize = GetParent()->GetSize();
	const PixelCoord& lSize = GetSize();
	SetPos(lParentSize.x/2 - lSize.x/2, lParentSize.y/2 - lSize.y/2);
}

Label* Dialog::SetQueryLabel(const wstr& pText, UiTbc::FontManager::FontId pFontId)
{
	if (!mLabel)
	{
		mLabel = new UiTbc::Label(mColor[1], pText);
	}
	mLabel->SetFontId(pFontId);
	//mLabel->ActivateFont(lPainter);
	mLabel->SetText(pText);
	UiTbc::Painter* lPainter = ((DesktopWindow*)GetParentOfType(DESKTOPWINDOW))->GetPainter();
	const int w = lPainter->GetStringWidth(pText);
	const int h = lPainter->GetFontHeight();
	mLabel->SetPreferredSize(w+2, h);
	AddChild(mLabel);
	// Set position.
	PixelCoord lCoord;
	const PixelCoord lSize = GetSize();
	lCoord.x = lSize.x/2 - w/2;
	lCoord.y = lSize.y/3 - h;
	mLabel->SetPos(lCoord+mOffset);
	//mLabel->DeactivateFont(lPainter);
	return mLabel;
}

void Dialog::SetQueryLabel(Label* pLabel)
{
	delete mLabel;
	mLabel = pLabel;
}

void Dialog::UpdateQueryLabel(const wstr& pText, const Color& pColor)
{
	deb_assert(mLabel);
	if (mLabel)
	{
		mLabel->SetText(pText);
		mLabel->SetFontColor(pColor);
	}
}

void Dialog::AddButton(int pTag, const wstr& pText, bool pAutoDismiss)
{
	Button* lButton = new Button(BorderComponent::ZIGZAG, 3, Color(mColor[0], mColor[1], 0.2f), pText);
	lButton->SetText(pText, mColor[1]);
	lButton->SetPreferredSize(57, 57);
	AddButton(pTag, lButton, pAutoDismiss);
}

void Dialog::AddButton(int pTag, Button* pButton, bool pAutoDismiss)
{
	//pButton->SetBaseColor(Color(0, 0, 0, 0));
	pButton->SetText(pButton->GetText(), mColor[1]);
	AddChild(pButton);
	mButtonList.push_back(pButton);
	SetButtonHandler(pTag, pButton, pAutoDismiss);
	UpdateLayout();
}

void Dialog::SetButtonHandler(int pTag, Button* pButton, bool pAutoDismiss)
{
	pButton->SetTag(pTag);
	if (pAutoDismiss)
	{
		pButton->SetOnClick(Dialog, OnDismissClick);
	}
	else
	{
		pButton->SetOnClick(Dialog, OnClick);
	}
}

bool Dialog::IsAutoDismissButton(Button* pButton) const
{
	return *pButton->mOnClick == UiTbc::ButtonType<UiTbc::Button>::Delegate((UiTbc::Dialog*)this, &Dialog::OnDismissClick);
}

void Dialog::SetOffset(PixelCoord pOffset)
{
	mOffset = pOffset;
	UpdateLayout();
}

void Dialog::UpdateLayout()
{
	const int lButtonCount = (int)mButtonList.size();
	if (!lButtonCount)
	{
		Parent::UpdateLayout();
		return;
	}

	const PixelCoord& lSize = GetSize();

	if (mLabel)
	{
		PixelCoord lLabelSize = mLabel->GetPreferredSize(true);
		PixelCoord lCoord(lSize.x/2 - lLabelSize.x/2, lSize.y/3 - lLabelSize.y);
		mLabel->SetPos(lCoord + mOffset);
	}

	// Find first auto-layouted button.
	Button* lButton = 0;
	for (int i = 0; i < lButtonCount; ++i)
	{
		lButton = mButtonList[i];
		if (lButton->GetTag() >= 0)
		{
			break;
		}
	}
	const PixelCoord lButtonSize = lButton->GetPreferredSize();
	const bool lLayoutX = ((lSize.x - lButtonSize.x*lButtonCount) > (lSize.y - lButtonSize.y*lButtonCount));
	if (lLayoutX)
	{
		const int lSpacePerEach = lButtonSize.x*3/2;
		const int lHalfGap = (lSpacePerEach - lButtonSize.x)/2;
		int x = lSize.x/2 + lHalfGap - lSpacePerEach*lButtonCount/2;
		const int y = mLabel? lSize.y/2 : lSize.y/2-lButtonSize.y/2;
		for (int i = 0; i < lButtonCount; ++i)
		{
			Button* lButton = mButtonList[i];
			if (lButton->GetTag() >= 0)
			{
				lButton->SetPos(PixelCoord(x, y) + mOffset);
				x += lSpacePerEach;
			}
		}
	}
	else
	{
		const int lSpacePerEach = lButtonSize.y*3/2;
		const int lHalfGap = (lSpacePerEach - lButtonSize.y)/2;
		const int x = lSize.x/2 - lButtonSize.x/2;
		int y = lSize.y/2 + lHalfGap - lSpacePerEach*lButtonCount/2;
		y += mLabel? lButtonSize.y/2 : 0;
		for (int i = 0; i < lButtonCount; ++i)
		{
			Button* lButton = mButtonList[i];
			if (lButton->GetTag() >= 0)
			{
				lButton->SetPos(PixelCoord(x, y) + mOffset);
				y += lSpacePerEach;
			}
		}
	}

	Parent::UpdateLayout();
}



void Dialog::Repaint(Painter* pPainter)
{
	Animate();	// Slides dialog in on create and out on destroy.
	Parent::Repaint(pPainter);
}

void Dialog::Animate()
{
	if (!IsComplete())
	{
		return;
	}
	if (mAnimationStep)
	{
		const PixelCoord& lParentSize = GetParent()->GetSize();
		const PixelCoord& lSize = GetSize();
		PixelCoord lPos = GetPos();
		lPos.x += mAnimationStep;
		lPos.y = lParentSize.y/2 - lSize.y/2;
		SetPos(lPos);
		if (!mIsClosing)
		{
			// Move in from right.
			mAnimationStep += ACCELERATION * mDirection;
			const int x = lParentSize.x/2-lSize.x/2;
			if ((mDirection > 0 && (lPos.x <= x || mAnimationStep >= 0)) ||
				(mDirection < 0 && (lPos.x >= x || mAnimationStep <= 0)))
			{
				mAnimationStep = 0;
				Center();
			}
		}
		else
		{
			// Move out to left.
			mAnimationStep -= ACCELERATION * mDirection;
			if ((mDirection > 0 && (lPos.x+GetSize().x < -MARGIN || mAnimationStep >= 0)) ||
				(mDirection < 0 && (lPos.x > lParentSize.x+MARGIN || mAnimationStep <= 0)))
			{
				DoClick(mClickedButton);
				((DesktopWindow*)GetParentOfType(DESKTOPWINDOW))->PostDeleteComponent(this, 0);
			}
		}
	}
}

void Dialog::OnDismissClick(Button* pButton)
{
	if (!mClickedButton)
	{
		if (mPreClickTarget)
		{
			mPreClickTarget(pButton);
		}
		mClickedButton = pButton;
		Dismiss();
	}
}

void Dialog::OnClick(Button* pButton)
{
	if (mPreClickTarget)
	{
		mPreClickTarget(pButton);
	}
	DoClick(pButton);
}

void Dialog::DoClick(Button* pButton)
{
	Action lPostClickTarget = mPostClickTarget;	// Save in case of destruction.
	if (pButton)
	{
		mTarget(pButton);
	}
	if (lPostClickTarget)
	{
		lPostClickTarget(pButton);
	}
}



}
