
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../../Include/GUI/UiDesktopWindow.h"
#include "../../Include/GUI/UiDialog.h"

#define SPEED		30
#define ACCELERATION	20
#define MARGIN		0



namespace UiTbc
{



Dialog::Dialog(Component* pParent, Action pTarget):
	Parent(WHITE, _T("Dialog"), new FloatingLayout),
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
}

void Dialog::Dismiss()
{
	OnClick(0);
}



void Dialog::SetColor(const Color& pTopLeftColor, const Color& pTopRightColor, const Color& pBottomRightColor, const Color& pBottomLeftColor)
{
	Parent::SetColor(pTopLeftColor, pTopRightColor, pBottomRightColor, pBottomLeftColor);
	UiTbc::Painter* lPainter = ((DesktopWindow*)GetParentOfType(DESKTOPWINDOW))->GetPainter();
	if (mLabel)
	{
		mLabel->SetText(mLabel->GetText(), mColor[1], CLEAR_COLOR, lPainter);
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

Label* Dialog::SetQueryLabel(const str& pText, UiTbc::FontManager::FontId pFontId)
{
	if (!mLabel)
	{
		mLabel = new UiTbc::Label;
	}
	UiTbc::Painter* lPainter = ((DesktopWindow*)GetParentOfType(DESKTOPWINDOW))->GetPainter();
	mLabel->SetFontId(pFontId);
	//mLabel->ActivateFont(lPainter);
	mLabel->SetText(pText, mColor[1], CLEAR_COLOR, lPainter);
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

void Dialog::AddButton(int pTag, const str& pText)
{
	Button* lButton = new Button(BorderComponent::ZIGZAG, 3, Color(mColor[0], mColor[1], 0.2f), pText);
	lButton->SetText(pText, mColor[1]);
	lButton->SetPreferredSize(57, 57);
	AddButton(pTag, lButton);
}

void Dialog::AddButton(int pTag, Button* pButton)
{
	//pButton->SetBaseColor(Color(0, 0, 0, 0));
	pButton->SetText(pButton->GetText(), mColor[1]);
	pButton->SetTag(pTag);
	AddChild(pButton);
	mButtonList.push_back(pButton);
	pButton->SetOnClick(Dialog, OnClick);
	UpdateLayout();
}

void Dialog::SetOffset(PixelCoord pOffset)
{
	mOffset = pOffset;
	UpdateLayout();
}

void Dialog::UpdateLayout()
{
	const PixelCoord& lSize = GetSize();

	if (mLabel)
	{
		PixelCoord lLabelSize = mLabel->GetPreferredSize(true);
		PixelCoord lCoord(lSize.x/2 - lLabelSize.x/2, lSize.y/3 - lLabelSize.y);
		mLabel->SetPos(lCoord + mOffset);
	}

	const int lButtonCount = (int)mButtonList.size();
	if (lButtonCount)
	{
		Button* lButton = mButtonList[0];
		const PixelCoord lButtonSize = lButton->GetPreferredSize();
		if (lSize.x > lSize.y)
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
				mTarget(mClickedButton);
				((DesktopWindow*)GetParentOfType(DESKTOPWINDOW))->PostDeleteComponent(this, 0);
			}
		}
	}
}

void Dialog::OnClick(Button* pButton)
{
	if (!mClickedButton)
	{
		if (mPreClickTarget)
		{
			mPreClickTarget(pButton);
		}
		GetTopParent()->ReleaseKeyboardFocus(RECURSE_DOWN);
		mClickedButton = pButton;
		mAnimationStep = -SPEED * mDirection;
		mIsClosing = true;
	}
}




}
