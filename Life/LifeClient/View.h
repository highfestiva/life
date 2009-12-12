
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games



#pragma once

#include "../../UiTbc/Include/GUI/UiWindow.h"



namespace UiTbc
{
class Button;
class Label;
class TextField;
}



namespace Life
{



class View: public UiTbc::Window
{
public:
	View(const Lepra::String& pTitle, UiTbc::Layout* pLayout);

protected:
	UiTbc::Label* AddLabel(const Lepra::String& pText, const Lepra::Color& pColor);
	UiTbc::TextField* AddTextField(const Lepra::String& pDefaultText, const Lepra::String& pName);
	UiTbc::Button* AddButton(const Lepra::String& pText);

private:
	void OnExit(UiTbc::Button*);
	virtual void OnExit() = 0;
};



}
