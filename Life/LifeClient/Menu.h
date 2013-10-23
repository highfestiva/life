
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../UiCure/Include/UiResourceManager.h"
#include "../../UiTBC/Include/GUI/UiButton.h"
#include "../../UiTBC/Include/GUI/UiDialog.h"
#include "../Life.h"

#define ICONBTN(i,n)		new UiCure::IconButton(mUiManager, GetResourceManager(), i, n)
#define ICONBTNA(i,n)		ICONBTN(_T(i), _T(n))



namespace UiCure
{
class GameUiManager;
}



namespace Life
{



class Menu
{
public:
	typedef UiTbc::Dialog::Action ButtonAction;

	Menu(UiCure::GameUiManager* pUiManager, Cure::ResourceManager* pResourceManager);
	virtual ~Menu();

	void SetButtonTapSound(const str& pSoundName, float pTapPitchOffset);

	UiTbc::Dialog* CreateTestDialog(const ButtonAction& pAction);
	UiTbc::Dialog* CreateTbcDialog(const ButtonAction& pAction, float pWidth, float pHeight);
	UiTbc::Button* CreateButton(const str& pText, const Color& pColor) const;
	void InitButton(UiTbc::Button* pButton) const;
	void DismissDialog();
	UiTbc::Dialog* GetDialog();

	Cure::ResourceManager* GetResourceManager() const;

	void OnAction(UiTbc::Button* pButton);
	void OnTapSound(UiTbc::Button* pButton);

	void SoundLoadCallback(UiCure::UserSound2dResource* pResource);

protected:
	UiCure::GameUiManager* mUiManager;
	Cure::ResourceManager* mResourceManager;
	UiTbc::Dialog* mDialog;
	UiTbc::Dialog::Action mButtonDelegate;
	UiCure::UserSound2dResource* mTapClick;
	float mTapPitchOffset;
};



}
