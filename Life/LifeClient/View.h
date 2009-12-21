
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



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
	View(const str& pTitle, UiTbc::Layout* pLayout);

protected:
	UiTbc::Label* AddLabel(const str& pText, const Color& pColor);
	UiTbc::TextField* AddTextField(const str& pDefaultText, const str& pName);
	UiTbc::Button* AddButton(const str& pText);

private:
	void OnExit(UiTbc::Button*);
	virtual void OnExit() = 0;
};



}
