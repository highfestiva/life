
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "menu.h"
#include "../../lepra/include/random.h"
#include "../../uicure/include/uigameuimanager.h"
#include "../../uitbc/include/gui/uidesktopwindow.h"



namespace life {



#define BGCOLOR_DIALOG		Color(5, 20, 30, 192)
#define FGCOLOR_DIALOG		Color(255, 255, 255, 255)



Menu::Menu(UiCure::GameUiManager* ui_manager, cure::ResourceManager* resource_manager):
	ui_manager_(ui_manager),
	resource_manager_(resource_manager),
	dialog_(0),
	tap_click_(0),
	tap_volume_(1),
	tap_pitch_offset_(0) {
}

Menu::~Menu() {
	delete dialog_;
	dialog_ = 0;
	resource_manager_ = 0;
	ui_manager_ = 0;
}

void Menu::SetButtonTapSound(const str& sound_name, float tap_volume, float tap_pitch_offset) {
	if (tap_click_) {
		delete tap_click_;
	}
	tap_volume_ = tap_volume;
	tap_pitch_offset_ = tap_pitch_offset;
	tap_click_ = new UiCure::UserSound2dResource(ui_manager_, uilepra::SoundManager::kLoopNone);
	tap_click_->Load(resource_manager_, sound_name, UiCure::UserSound2dResource::TypeLoadCallback(this, &Menu::SoundLoadCallback));
}

uitbc::Dialog* Menu::CreateTestDialog(const ButtonAction& action) {
	uitbc::Dialog* d = CreateTbcDialog(action, 0.8f, 0.8f);
	if (!d) {
		return 0;
	}
	d->AddButton(1, CreateButton(L"Test", Color(40, 210, 40)), false);
	d->AddButton(2, CreateButton(L"Eminency", Color(50, 90, 210)), false);
	d->AddButton(3, CreateButton(L"Thirst", Color(210, 50, 40)), true);
	return d;
}

uitbc::Dialog* Menu::CreateTbcDialog(const ButtonAction& action, float width, float height) {
	if (dialog_) {
		return 0;
	}
	button_delegate_ = action;
	uitbc::Dialog* d = new uitbc::Dialog(ui_manager_->GetDesktopWindow(), uitbc::Dialog::Action(this, &Menu::OnAction));
	d->SetPostClickTarget(uitbc::Dialog::Action(this, &Menu::OnTapSound));
	d->SetSize((int)(width*ui_manager_->GetCanvas()->GetWidth()), (int)(height*ui_manager_->GetCanvas()->GetHeight()));
	d->SetPreferredSize(d->GetSize());
	d->SetColor(BGCOLOR_DIALOG, FGCOLOR_DIALOG, BLACK, BLACK);
	d->SetCornerRadius(d->GetSize().y/6);
	dialog_ = d;
	return d;
}

uitbc::Button* Menu::CreateButton(const wstr& text, const Color& color) const {
	uitbc::Button* _button = new uitbc::Button(color, text);
	//_button->SetText(text);
	InitButton(_button);
	return _button;
}

void Menu::InitButton(uitbc::Button* button) const {
	const int w = dialog_->GetSize().x*3/4;
	const int h = dialog_->GetSize().y/5;
	button->SetPreferredSize(w, h);
	button->SetRoundedRadius(h/3);
	button->UpdateLayout();
}

void Menu::DismissDialog() {
	if (dialog_) {
		dialog_->Dismiss();
		dialog_ = 0;
	}
}

uitbc::Dialog* Menu::GetDialog() {
	return dialog_;
}

cure::ResourceManager* Menu::GetResourceManager() const {
	return resource_manager_;
}

void Menu::OnAction(uitbc::Button* button) {
	if (!dialog_) {
		return;
	}
	uitbc::Dialog* d = dialog_;
	button_delegate_(button);
	if (d == dialog_ && (!button || d->IsAutoDismissButton(button))) {
		button_delegate_.clear();
		dialog_ = 0;
	}
}

void Menu::OnTapSound(uitbc::Button*) {
	if (tap_click_ && tap_click_->GetLoadState() == cure::kResourceLoadComplete) {
		ui_manager_->GetSoundManager()->Play(tap_click_->GetData(), tap_volume_, Random::Uniform(1-tap_pitch_offset_, 1+tap_pitch_offset_));
	}
}

void Menu::SoundLoadCallback(UiCure::UserSound2dResource*) {
}



loginstance(kUiGfx2D, Menu);



}
