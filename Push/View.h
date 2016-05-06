
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../uitbc/include/gui/uiwindow.h"
#include "push.h"



namespace uitbc {
class Button;
class Label;
class TextField;
}



namespace Push {



class View: public uitbc::Window {
public:
	View(const wstr& title, uitbc::Layout* layout);

protected:
	uitbc::RectComponent* AddRow(const Color& color, int column_count);
	uitbc::RectComponent* AddCentering(int layer, uitbc::Component* parent);
	uitbc::Label* AddLabel(const wstr& text, const Color& color, int preferred_width = 0, uitbc::Component* parent = 0, int layer = 0);
	uitbc::TextField* AddTextField(const wstr& default_text, const str& name);
	uitbc::Button* AddButton(const wstr& text, void* extra_data, uitbc::Component* parent = 0);

private:
	void OnExit(uitbc::Button*);
	virtual void OnExit() = 0;
};



}
