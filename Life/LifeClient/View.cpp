
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games



#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Lepra/Include/Network.h"
#include "../../UiTbc/Include/GUI/UiCaption.h"
#include "../../UiTbc/Include/GUI/UiTextField.h"
#include "View.h"



namespace Life
{



View::View(const Lepra::String& pTitle, UiTbc::Layout* pLayout):
	UiTbc::Window(UiTbc::Window::BORDER_LINEARSHADING, 2, Lepra::WHITE, pTitle, pLayout)
{
	UiTbc::Caption* lCaption = new UiTbc::Caption(Lepra::DARK_GRAY, Lepra::GRAY, Lepra::GRAY, Lepra::LIGHT_GRAY,
		Lepra::BLACK, Lepra::DARK_GRAY, Lepra::DARK_GRAY, Lepra::GRAY, 20);
	lCaption->SetText(pTitle, Lepra::WHITE, Lepra::BLACK, Lepra::GRAY, Lepra::BLACK);
	SetCaption(lCaption);

	UiTbc::Button* lCloseButton = new UiTbc::Button(UiTbc::BorderComponent::ZIGZAG, 2, Lepra::LIGHT_GRAY, pTitle);
	lCloseButton->SetText(_T("x"), Lepra::WHITE, Lepra::BLACK);
	lCloseButton->SetPreferredSize(16, 16);
	lCloseButton->SetMinSize(16, 16);
	lCloseButton->SetOnUnclickedFunc(View, OnExit);
	lCaption->SetRightButton(lCloseButton);
}



UiTbc::Label* View::AddLabel(const Lepra::String& pText, const Lepra::Color& pColor)
{
	UiTbc::Label* lLabel = new UiTbc::Label(Lepra::DARK_GRAY, Lepra::RED);
	lLabel->SetPreferredSize(0, 24);
	lLabel->SetText(pText, pColor, Lepra::RED);
	AddChild(lLabel);
	return (lLabel);
}

UiTbc::TextField* View::AddTextField(const Lepra::String& pDefaultText, const Lepra::String& pName)
{
	UiTbc::TextField* lTextField = new UiTbc::TextField(GetClientRectComponent(), UiTbc::Window::BORDER_SUNKEN | UiTbc::Window::BORDER_LINEARSHADING,
		3, Lepra::BLACK, pName);
	lTextField->SetPreferredSize(0, 24);
	lTextField->SetFontColor(Lepra::WHITE);
	lTextField->SetText(pDefaultText);
	AddChild(lTextField);
	return (lTextField);
}

UiTbc::Button* View::AddButton(const Lepra::String& pText)
{
	UiTbc::Button* lButton = new UiTbc::Button(UiTbc::BorderComponent::ZIGZAG, 3, Lepra::DARK_GREEN, pText);
	lButton->SetText(pText, Lepra::WHITE, Lepra::WHITE);
	lButton->SetPreferredSize(0, 24);
	lButton->SetMinSize(20, 20);
	AddChild(lButton);
	return (lButton);
}

void View::OnExit(UiTbc::Button*)
{
	OnExit();
}



}
