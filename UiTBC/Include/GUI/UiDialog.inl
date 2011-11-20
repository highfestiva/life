
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games


template<class _Target>
Dialog<_Target>::Dialog(Component* pParent, const str& pText, Action pTarget):
	Parent(WHITE, _T("Dialog"), new FloatingLayout),
	mText(pText),
	mTarget(pTarget)
{
	SetCornerRadius(20);
	mColor[1] = DARK_GRAY;
	const PixelCoord& lSize = pParent->GetSize();
	const int w = std::max(lSize.x/2, 200);
	const int h = std::max(lSize.y/2, 100);
	SetPreferredSize(w, h);
	pParent->AddChild(this);
	SetPos(lSize.x/2 - w/2, lSize.y/2 - h/2);
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
	lButton->SetTag(pTag);
	AddChild(lButton);
	mButtonList.push_back(lButton);
	lButton->UpdateLayout();
	lButton->SetOnClick(Dialog, OnClick);
}



template<class _Target>
void Dialog<_Target>::UpdateLayout()
{
	const int lCount = (int)mButtonList.size();
	const PixelCoord& lSize = GetSize();
	const int lSpacePerEach = lSize.x / (lCount + 1);

	for (int i = 0; i < lCount; ++i)
	{
		Button* lButton = mButtonList[i];
		const int h = std::max(lSize.y/4, 44);
		const int x = lSpacePerEach/2 + i*lSpacePerEach;
		const int y = mText.empty()? lSize.y/2-h/2 : lSize.y/2;
		lButton->SetPos(x+lSpacePerEach*1/6, y);
		lButton->SetPreferredSize(lSpacePerEach*4/6, h);
	}

	Parent::UpdateLayout();
}

template<class _Target>
void Dialog<_Target>::OnClick(Button* pButton)
{
	mTarget(pButton);
	delete (this);
}
