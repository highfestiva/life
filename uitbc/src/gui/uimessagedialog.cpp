
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../include/gui/uimessagedialog.h"



namespace uitbc {



MessageDialog::MessageDialog(Component* parent, Action ok, const wstr& message):
	Parent(parent, ok) {
	SetPreferredSize(PixelCoord(310, 170));
	SetSize(GetPreferredSize());
	SetQueryLabel(message, FontManager::kInvalidFontid);
}

MessageDialog::~MessageDialog() {
}



void MessageDialog::Repaint(Painter* painter) {
	if (button_list_.empty()) {
		AddButton(100, L"OK", true);
	}
	Parent::Repaint(painter);
}



}
