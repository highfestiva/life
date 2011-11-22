
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#define SPEED		30
#define ACCELERATION	20
#define MARGIN		0



template<class _Target>
Dialog<_Target>::Dialog(Component* pParent, const str& pText, Action pTarget):
	Parent(WHITE, _T("Dialog"), new FloatingLayout),
	mText(pText),
	mTarget(pTarget),
	mClickedButton(0),
	mIsClosing(false),
	mAnimationStep(-SPEED)
{
	SetCornerRadius(20);
	mColor[1] = DARK_GRAY;
	const PixelCoord& lSize = pParent->GetSize();
	const int w = std::max(lSize.x/2, 200);
	const int h = std::max(lSize.y/2, 100);
	SetPreferredSize(w, h);
	pParent->AddChild(this);
	SetPos(lSize.x+MARGIN, lSize.y/2 - h/2);

	// Calculate what animation speed is required to get right end speed using the acceleration.
	const int lTargetX = GetParent()->GetSize().x/2 - w/2;
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
	if (mAnimationStep)
	{
		PixelCoord lPos = GetPos();
		lPos.x += mAnimationStep;
		SetPos(lPos);
		const PixelCoord& lSize = GetSize();
		const PixelCoord& lParentSize = GetParent()->GetSize();
		if (!mIsClosing)
		{
			// Move in from right.
			mAnimationStep += ACCELERATION;
			const int x = lParentSize.x/2-lSize.x/2;
			if (lPos.x <= x)
			{
				lPos.x = x;
				SetPos(lPos);
				mAnimationStep = 0;
			}
		}
		else
		{
			// Move out to left.
			mAnimationStep -= ACCELERATION;
			if (lPos.x+GetSize().x < -MARGIN)
			{
				mTarget(mClickedButton);
				((DesktopWindow*)GetTopParent())->PostDeleteComponent(this, 0);
			}
		}
	}
	Parent::Repaint(pPainter);

	pPainter->SetColor(mColor[1], 0);
	PixelCoord lCoord = GetScreenPos();
	const PixelCoord lSize = GetSize();
	lCoord.x += lSize.x/2 - pPainter->GetStringWidth(mText)/2;
	lCoord.y += lSize.y/3 - pPainter->GetFontHeight();
	pPainter->PrintText(mText, lCoord.x, lCoord.y);
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
	const int y = mText.empty()? lSize.y/2-lButtonSize.y/2 : lSize.y/2;
	for (int i = 0; i < lCount; ++i)
	{
		Button* lButton = mButtonList[i];
		lButton->SetPos(x, y);
		x +=  lSpacePerEach;
	}

	Parent::UpdateLayout();
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
