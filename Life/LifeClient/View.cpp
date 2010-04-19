
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Lepra/Include/Network.h"
#include "../../UiTBC/Include/GUI/UiCaption.h"
#include "../../UiTBC/Include/GUI/UiTextField.h"
#include "View.h"



namespace Life
{



View::View(const str& pTitle, UiTbc::Layout* pLayout):
	UiTbc::Window(UiTbc::Window::BORDER_LINEARSHADING, 2, WHITE, pTitle, pLayout)
{
	UiTbc::Caption* lCaption = new UiTbc::Caption(DARK_GRAY, GRAY, GRAY, LIGHT_GRAY,
		BLACK, DARK_GRAY, DARK_GRAY, GRAY, 20);
	lCaption->SetText(pTitle, WHITE, BLACK, GRAY, BLACK);
	SetCaption(lCaption);

	UiTbc::Button* lCloseButton = new UiTbc::Button(UiTbc::BorderComponent::ZIGZAG, 2, LIGHT_GRAY, pTitle);
	lCloseButton->SetText(_T("x"), WHITE, BLACK);
	lCloseButton->SetPreferredSize(16, 16);
	lCloseButton->SetMinSize(16, 16);
	lCloseButton->SetOnClick(View, OnExit);
	lCaption->SetRightButton(lCloseButton);
}



UiTbc::Label* View::AddLabel(const str& pText, const Color& pColor)
{
	UiTbc::Label* lLabel = new UiTbc::Label(DARK_GRAY, RED);
	lLabel->SetPreferredSize(0, 24);
	lLabel->SetText(pText, pColor, RED);
	AddChild(lLabel);
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

UiTbc::Button* View::AddButton(const str& pText, void* pExtraData)
{
	UiTbc::Button* lButton = new UiTbc::Button(UiTbc::BorderComponent::ZIGZAG, 3, DARK_GREEN, pText);
	lButton->SetText(pText, WHITE, WHITE);
	lButton->SetPreferredSize(0, 24);
	lButton->SetMinSize(20, 20);
	lButton->SetExtraData(pExtraData);
	AddChild(lButton);
	return (lButton);
}

void View::OnExit(UiTbc::Button*)
{
	OnExit();
}



}
