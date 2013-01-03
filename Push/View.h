
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../UiTBC/Include/GUI/UiWindow.h"
#include "Push.h"



namespace UiTbc
{
class Button;
class Label;
class TextField;
}



namespace Push
{



class View: public UiTbc::Window
{
public:
	View(const str& pTitle, UiTbc::Layout* pLayout);

protected:
	UiTbc::RectComponent* AddRow(const Color& pColor, int pColumnCount);
	UiTbc::RectComponent* AddCentering(int pLayer, UiTbc::Component* pParent);
	UiTbc::Label* AddLabel(const str& pText, const Color& pColor, int pPreferredWidth = 0, UiTbc::Component* pParent = 0, int pLayer = 0);
	UiTbc::TextField* AddTextField(const str& pDefaultText, const str& pName);
	UiTbc::Button* AddButton(const str& pText, void* pExtraData, UiTbc::Component* pParent = 0);

private:
	void OnExit(UiTbc::Button*);
	virtual void OnExit() = 0;
};



}
