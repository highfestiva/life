
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#define SPEED		30
#define ACCELERATION	20
#define MARGIN		0



template<class _Target>
Dialog<_Target>::Dialog(Component* pParent, Action pTarget):
	Parent(WHITE, _T("Dialog"), new FloatingLayout),
	mLabel(0),
	mTarget(pTarget),
	mClickedButton(0),
	mIsClosing(false),
	mAnimationStep(-SPEED)
{
	SetCornerRadius(20);
	mColor[1] = DARK_GRAY;
	pParent->AddChild(this);

	const PixelCoord& lParentSize = pParent->GetSize();
	PixelCoord lSize = lParentSize / 2;
	SetPos(lParentSize.x+MARGIN, lParentSize.y/2 - lSize.y/2);
	SetSize(lSize);
	SetPreferredSize(lSize);

	// Calculate what animation speed is required to get right end speed using the acceleration.
	const int lTargetX = lParentSize.x/2 - lSize.x/2;
	const int lStartX = GetPos().x;
	int x = lTargetX;
	for (int i = 0; i < 1000; ++i)
	{
		x -= mAnimationStep;
		if (x >= lStartX)
		{
			break;
		}
		mAnimationStep -= ACCELERATION;
	}
	SetPos(x, GetPos().y);
}

template<class _Target>
Dialog<_Target>::~Dialog()
{
	mTarget.clear();
}

template<class _Target>
void Dialog<_Target>::Center()
{
	const PixelCoord& lParentSize = GetParent()->GetSize();
	const PixelCoord& lSize = GetSize();
	SetPos(lParentSize.x/2 - lSize.x/2, lParentSize.y/2 - lSize.y/2);
}

template<class _Target>
Label* Dialog<_Target>::QueryLabel(const str& pText, UiTbc::FontManager::FontId pFontId)
{
	if (!mLabel)
	{
		mLabel = new UiTbc::Label;
	}
	UiTbc::Painter* lPainter = ((DesktopWindow*)GetTopParent())->GetPainter();
	mLabel->SetFontId(pFontId);
	mLabel->ActivateFont(lPainter);
	mLabel->SetText(pText, mColor[1], Lepra::Color(0,0,0,0), lPainter);
	const int w = lPainter->GetStringWidth(pText);
	const int h = lPainter->GetFontHeight();
	mLabel->SetPreferredSize(w+20, h);
	AddChild(mLabel);
	// Set position.
	PixelCoord lCoord;
	const PixelCoord lSize = GetSize();
	lCoord.x = lSize.x/2 - w/2;
	lCoord.y = lSize.y/3 - h;
	mLabel->SetPos(lCoord);
	mLabel->DeactivateFont(lPainter);
	return mLabel;
}

template<class _Target>
void Dialog<_Target>::AddButton(int pTag, const str& pText)
{
	Button* lButton = new Button(BorderComponent::ZIGZAG, 1, Color(mColor[0], mColor[1], 0.3f), _T("DialogButton"));
	lButton->SetText(pText, mColor[1]);
	lButton->SetPreferredSize(57, 57);
	AddButton(pTag, lButton);
}

template<class _Target>
void Dialog<_Target>::AddButton(int pTag, Button* pButton)
{
	//pButton->SetBaseColor(Color(0, 0, 0, 0));
	pButton->SetText(pButton->GetText(), mColor[1]);
	pButton->GetClientRectComponent()->SetIsHollow(true);
	pButton->SetTag(pTag);
	AddChild(pButton);
	mButtonList.push_back(pButton);
	pButton->UpdateLayout();
	pButton->SetOnClick(Dialog, OnClick);
	UpdateLayout();
}



template<class _Target>
void Dialog<_Target>::Repaint(Painter* pPainter)
{
	Animate();	// Slides dialog in on create and out on destroy.
	Parent::Repaint(pPainter);
}

template<class _Target>
void Dialog<_Target>::UpdateLayout()
{
	const int lCount = (int)mButtonList.size();
	const PixelCoord& lSize = GetSize();

	Button* lButton = mButtonList[0];
	PixelCoord lButtonSize = lButton->GetPreferredSize();
	const int lSpacePerEach = lButtonSize.x*3/2;
	const int lHalfGap = (lSpacePerEach - lButtonSize.x)/2;
	int x = lSize.x/2 + lHalfGap - lSpacePerEach*lCount/2;
	const int y = mLabel? lSize.y/2 : lSize.y/2-lButtonSize.y/2;
	for (int i = 0; i < lCount; ++i)
	{
		Button* lButton = mButtonList[i];
		lButton->SetPos(x, y);
		x +=  lSpacePerEach;
	}

	Parent::UpdateLayout();
}

template<class _Target>
void Dialog<_Target>::Animate()
{
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
			mAnimationStep += ACCELERATION;
			const int x = lParentSize.x/2-lSize.x/2;
			if (lPos.x <= x || mAnimationStep >= 0)
			{
				mAnimationStep = 0;
				Center();
			}
		}
		else
		{
			// Move out to left.
			mAnimationStep -= ACCELERATION;
			if (lPos.x+GetSize().x < -MARGIN || mAnimationStep >= 0)
			{
				mTarget(mClickedButton);
				((DesktopWindow*)GetTopParent())->PostDeleteComponent(this, 0);
			}
		}
	}
}

template<class _Target>
void Dialog<_Target>::OnClick(Button* pButton)
{
	if (!mClickedButton)
	{
		mClickedButton = pButton;
		mAnimationStep = -SPEED;
		mIsClosing = true;
	}
}
