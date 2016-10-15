
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../uicure/include/uiresourcemanager.h"
#include "../../uitbc/include/gui/uibutton.h"
#include "../../uitbc/include/gui/uidialog.h"
#include "../life.h"

#define ICONBTN(i,n)		new UiCure::IconButton(ui_manager_, GetResourceManager(), i, n)
#define ICONBTNA(i,n)		ICONBTN(i, n)



namespace UiCure {
class GameUiManager;
}



namespace life {



class Menu {
public:
	typedef uitbc::Dialog::Action ButtonAction;

	Menu(UiCure::GameUiManager* ui_manager, cure::ResourceManager* resource_manager);
	virtual ~Menu();

	void SetButtonTapSound(const str& sound_name, float tap_volume, float tap_pitch_offset);

	uitbc::Dialog* CreateTestDialog(const ButtonAction& action);
	uitbc::Dialog* CreateTbcDialog(const ButtonAction& action, float width, float height);
	uitbc::Button* CreateButton(const wstr& text, const Color& color) const;
	void InitButton(uitbc::Button* button) const;
	void DismissDialog();
	uitbc::Dialog* GetDialog();

	cure::ResourceManager* GetResourceManager() const;

	void OnAction(uitbc::Button* button);
	void OnTapSound(uitbc::Button* button);

	void SoundLoadCallback(UiCure::UserSound2dResource* resource);

protected:
	UiCure::GameUiManager* ui_manager_;
	cure::ResourceManager* resource_manager_;
	uitbc::Dialog* dialog_;
	uitbc::Dialog::Action button_delegate_;
	UiCure::UserSound2dResource* tap_click_;
	float tap_volume_;
	float tap_pitch_offset_;

	logclass();
};



}
