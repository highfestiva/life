
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "view.h"
#include "../uitbc/include/gui/uicaption.h"
#include "../uitbc/include/gui/uicenterlayout.h"
#include "../uitbc/include/gui/uitextfield.h"
#include "rtvar.h"



namespace HoverTank {



View::View(const str& title, uitbc::Layout* layout):
	uitbc::Window(uitbc::Window::kBorderLinearshading, 2, DARK_GRAY, title, layout) {
	Init();
	uitbc::Caption* caption = new uitbc::Caption(DARK_GRAY, GRAY, GRAY, LIGHT_GRAY,
		BLACK, DARK_GRAY, DARK_GRAY, GRAY, 20);
	caption->SetText(title, WHITE, BLACK, GRAY, BLACK);
	SetCaption(caption);

	uitbc::Button* close_button = new uitbc::Button(uitbc::BorderComponent::kZigzag, 1, LIGHT_RED, title);
	//close_button->SetText("x", RED, BLACK);
	close_button->SetPreferredSize(10, 10);
	close_button->SetMinSize(10, 10);
	close_button->SetOnClick(View, OnExit);
	caption->SetRightButton(close_button);
}



uitbc::RectComponent* View::AddRow(const Color& color, int column_count) {
	uitbc::RectComponent* row_layer = new uitbc::RectComponent(color, "Row", new uitbc::GridLayout(1, column_count));
	row_layer->SetIsHollow(false);
	AddChild(row_layer);
	return row_layer;
}

uitbc::RectComponent* View::AddCentering(int layer, uitbc::Component* parent) {
	uitbc::RectComponent* center_layer = new uitbc::RectComponent("Centering", new uitbc::CenterLayout);
	parent->AddChild(center_layer, 0, 0, layer);
	return center_layer;
}

uitbc::Label* View::AddLabel(const str& text, const Color& color, int preferred_width, uitbc::Component* parent, int layer) {
	uitbc::Label* label = new uitbc::Label(color, text);
	label->SetIsHollow(true);
	label->SetPreferredSize(preferred_width, 24, (preferred_width == 0)? false : true);
	if (parent) {
		parent->AddChild(label, 0, 0, layer);
	} else if (layer) {
		Component::AddChild(label, 0, 0, layer);
	} else {
		AddChild(label);
	}
	return (label);
}

uitbc::TextField* View::AddTextField(const str& default_text, const str& name) {
	uitbc::TextField* text_field = new uitbc::TextField(GetClientRectComponent(), uitbc::Window::kBorderSunken | uitbc::Window::kBorderLinearshading,
		3, BLACK, name);
	text_field->SetPreferredSize(0, 24);
	text_field->SetFontColor(WHITE);
	text_field->SetText(default_text);
	AddChild(text_field);
	return (text_field);
}

uitbc::Button* View::AddButton(const str& text, void* extra_data, uitbc::Component* parent) {
	uitbc::Button* button = new uitbc::Button(uitbc::BorderComponent::kZigzag, 3, LIGHT_GRAY, text);
	button->SetText(text, OFF_BLACK, LIGHT_RED);
	button->SetPreferredSize(0, 24);
	button->SetMinSize(50, 10);
	button->SetExtraData(extra_data);
	if (parent) {
		parent->AddChild(button);
	} else {
		AddChild(button);
	}
	return (button);
}

void View::OnExit(uitbc::Button*) {
	OnExit();
}



}
