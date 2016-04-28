
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../UiTbc/Include/GUI/UiWindow.h"
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
	View(const wstr& pTitle, UiTbc::Layout* pLayout);

protected:
	UiTbc::RectComponent* AddRow(const Color& pColor, int pColumnCount);
	UiTbc::RectComponent* AddCentering(int pLayer, UiTbc::Component* pParent);
	UiTbc::Label* AddLabel(const wstr& pText, const Color& pColor, int pPreferredWidth = 0, UiTbc::Component* pParent = 0, int pLayer = 0);
	UiTbc::TextField* AddTextField(const wstr& pDefaultText, const str& pName);
	UiTbc::Button* AddButton(const wstr& pText, void* pExtraData, UiTbc::Component* pParent = 0);

private:
	void OnExit(UiTbc::Button*);
	virtual void OnExit() = 0;
};



}
