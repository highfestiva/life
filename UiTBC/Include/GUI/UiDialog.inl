
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games


template<class _Target>
Dialog<_Target>::Dialog(Component* pParent, const str& pText, Action pTarget):
	Parent(_T("Dialog"), new FloatingLayout),
	mBackground(WHITE),
	mForeground(DARK_GRAY),
	mText(pText),
	mTarget(pTarget)
{
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
	Button* lButton = new Button(BorderComponent::ZIGZAG, 1, Color(mBackground, mForeground, 0.8f), _T("DialogButton"));
	lButton->SetText(pText, mForeground);
	lButton->SetTag(pTag);
	AddChild(lButton);
	mButtonList.push_back(lButton);
	lButton->UpdateLayout();
	lButton->SetOnClick(Dialog, OnClick);
}

template<class _Target>
void Dialog<_Target>::FireAndForget()
{
}



template<class _Target>
void Dialog<_Target>::Repaint(Painter* pPainter)
{
	PixelRect lRect(GetScreenPos(), GetScreenPos() + GetSize());
	const float x = (float)lRect.GetCenterX();
	const float y = (float)lRect.GetCenterY();
	const float dx = (float)lRect.GetWidth()/2;
	const float dy = (float)lRect.GetHeight()/2;
	const float rdx = dx * 0.04f;
	const float rdy = dy * 0.04f;
	std::vector<Vector2DF> lCoords;
	lCoords.push_back(Vector2DF(x, y));
	// Upper left.
	lCoords.push_back(Vector2DF(x-dx-rdx,   y-dy+rdy*2));
	lCoords.push_back(Vector2DF(x-dx,       y-dy));
	lCoords.push_back(Vector2DF(x-dx+rdx*2, y-dy-rdy));
	// Upper right.
	lCoords.push_back(Vector2DF(x+dx-rdx*2, y-dy-rdy));
	lCoords.push_back(Vector2DF(x+dx,       y-dy));
	lCoords.push_back(Vector2DF(x+dx+rdx,   y-dy+rdy*2));
	// Lower right.
	lCoords.push_back(Vector2DF(x+dx+rdx,   y+dy-rdy*2));
	lCoords.push_back(Vector2DF(x+dx,       y+dy));
	lCoords.push_back(Vector2DF(x+dx-rdx*2, y+dy+rdy));
	// Lower left.
	lCoords.push_back(Vector2DF(x-dx+rdx*2, y+dy+rdy));
	lCoords.push_back(Vector2DF(x-dx,       y+dy));
	lCoords.push_back(Vector2DF(x-dx-rdx,   y+dy-rdy*2));
	// Back to start.
	lCoords.push_back(lCoords[1]);
	pPainter->SetColor(mBackground, 0);
	pPainter->DrawFan(lCoords, true);

	Parent::Repaint(pPainter);
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
