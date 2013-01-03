
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "View.h"
#include "../UiTBC/Include/GUI/UiCaption.h"
#include "../UiTBC/Include/GUI/UiCenterLayout.h"
#include "../UiTBC/Include/GUI/UiTextField.h"
#include "RtVar.h"



namespace Push
{



View::View(const str& pTitle, UiTbc::Layout* pLayout):
	UiTbc::Window(UiTbc::Window::BORDER_LINEARSHADING, 2, DARK_GRAY, pTitle, pLayout)
{
	UiTbc::Caption* lCaption = new UiTbc::Caption(DARK_GRAY, GRAY, GRAY, LIGHT_GRAY,
		BLACK, DARK_GRAY, DARK_GRAY, GRAY, 20);
	lCaption->SetText(pTitle, WHITE, BLACK, GRAY, BLACK);
	SetCaption(lCaption);

	UiTbc::Button* lCloseButton = new UiTbc::Button(UiTbc::BorderComponent::ZIGZAG, 1, LIGHT_RED, pTitle);
	//lCloseButton->SetText(_T("x"), RED, BLACK);
	lCloseButton->SetPreferredSize(10, 10);
	lCloseButton->SetMinSize(10, 10);
	lCloseButton->SetOnClick(View, OnExit);
	lCaption->SetRightButton(lCloseButton);
}



UiTbc::RectComponent* View::AddRow(const Color& pColor, int pColumnCount)
{
	UiTbc::RectComponent* lRowLayer = new UiTbc::RectComponent(pColor, _T("Row"), new UiTbc::GridLayout(1, pColumnCount));
	lRowLayer->SetIsHollow(false);
	AddChild(lRowLayer);
	return lRowLayer;
}

UiTbc::RectComponent* View::AddCentering(int pLayer, UiTbc::Component* pParent)
{
	UiTbc::RectComponent* lCenterLayer = new UiTbc::RectComponent(_T("Centering"), new UiTbc::CenterLayout);
	pParent->AddChild(lCenterLayer, 0, 0, pLayer);
	return lCenterLayer;
}

UiTbc::Label* View::AddLabel(const str& pText, const Color& pColor, int pPreferredWidth, UiTbc::Component* pParent, int pLayer)
{
	UiTbc::Label* lLabel = new UiTbc::Label;
	lLabel->SetIsHollow(true);
	lLabel->SetPreferredSize(pPreferredWidth, 24, (pPreferredWidth == 0)? false : true);
	lLabel->SetText(pText, pColor, RED);
	if (pParent)
	{
		pParent->AddChild(lLabel, 0, 0, pLayer);
	}
	else if (pLayer)
	{
		Component::AddChild(lLabel, 0, 0, pLayer);
	}
	else
	{
		AddChild(lLabel);
	}
	return (lLabel);
}

UiTbc::TextField* View::AddTextField(const str& pDefaultText, const str& pName)
{
	UiTbc::TextField* lTextField = new UiTbc::TextField(GetClientRectComponent(), UiTbc::Window::BORDER_SUNKEN | UiTbc::Window::BORDER_LINEARSHADING,
		3, BLACK, pName);
	lTextField->SetPreferredSize(0, 24);
	lTextField->SetFontColor(WHITE);
	lTextField->SetText(pDefaultText);
	AddChild(lTextField);
	return (lTextField);
}

UiTbc::Button* View::AddButton(const str& pText, void* pExtraData, UiTbc::Component* pParent)
{
	UiTbc::Button* lButton = new UiTbc::Button(UiTbc::BorderComponent::ZIGZAG, 3, LIGHT_GRAY, pText);
	lButton->SetText(pText, OFF_BLACK, LIGHT_RED);
	lButton->SetPreferredSize(0, 24);
	lButton->SetMinSize(50, 10);
	lButton->SetExtraData(pExtraData);
	if (pParent)
	{
		pParent->AddChild(lButton);
	}
	else
	{
		AddChild(lButton);
	}
	return (lButton);
}

void View::OnExit(UiTbc::Button*)
{
	OnExit();
}



}
