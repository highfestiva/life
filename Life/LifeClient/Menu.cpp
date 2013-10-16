
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "Menu.h"
#include "../../Lepra/Include/Random.h"
#include "../../UiCure/Include/UiGameUiManager.h"
#include "../../UiTBC/Include/GUI/UiDesktopWindow.h"



namespace Life
{



#define BGCOLOR_DIALOG		Color(5, 20, 30, 192)
#define FGCOLOR_DIALOG		Color(255, 255, 255, 255)



Menu::Menu(UiCure::GameUiManager* pUiManager, Cure::ResourceManager* pResourceManager):
	mUiManager(pUiManager),
	mResourceManager(pResourceManager),
	mDialog(0),
	mTapClick(0),
	mTapPitchOffset(0)
{
}

Menu::~Menu()
{
	mDialog = 0;
	mResourceManager = 0;
	mUiManager = 0;
}

void Menu::SetButtonTapSound(const str& pSoundName, float pTapPitchOffset)
{
	if (mTapClick)
	{
		delete mTapClick;
	}
	mTapPitchOffset = pTapPitchOffset;
	mTapClick = new UiCure::UserSound2dResource(mUiManager, UiLepra::SoundManager::LOOP_NONE);
	mTapClick->Load(mResourceManager, pSoundName, UiCure::UserSound2dResource::TypeLoadCallback(this, &Menu::SoundLoadCallback));
}

UiTbc::Dialog* Menu::CreateTestDialog(const ButtonAction& pAction)
{
	UiTbc::Dialog* d = CreateTbcDialog(pAction, 0.8f, 0.8f);
	if (!d)
	{
		return 0;
	}
	d->AddButton(1, CreateButton(_T("Test"), Color(40, 210, 40)), false);
	d->AddButton(2, CreateButton(_T("Eminency"), Color(50, 90, 210)), false);
	d->AddButton(3, CreateButton(_T("Thirst"), Color(210, 50, 40)), true);
	return d;
}

UiTbc::Dialog* Menu::CreateTbcDialog(const ButtonAction& pAction, float pWidth, float pHeight)
{
	if (mDialog)
	{
		return 0;
	}
	mButtonDelegate = pAction;
	UiTbc::Dialog* d = new UiTbc::Dialog(mUiManager->GetDesktopWindow(), UiTbc::Dialog::Action(this, &Menu::OnAction));
	d->SetPreClickTarget(UiTbc::Dialog::Action(this, &Menu::OnTapSound));
	d->SetSize((int)(pWidth*mUiManager->GetCanvas()->GetWidth()), (int)(pHeight*mUiManager->GetCanvas()->GetHeight()));
	d->SetPreferredSize(d->GetSize());
	d->SetColor(BGCOLOR_DIALOG, FGCOLOR_DIALOG, BLACK, BLACK);
	mDialog = d;
	return d;
}

UiTbc::Button* Menu::CreateButton(const str& pText, const Color& pColor)
{
	UiTbc::Button* lButton = new UiTbc::Button(pColor, pText);
	lButton->SetText(pText);
	const float lTouchScale = 1.28f * mUiManager->GetCanvas()->GetWidth() / (float)mUiManager->GetDisplayManager()->GetPhysicalScreenSize();
	lButton->SetPreferredSize(int(6*lTouchScale), int(1.5f*lTouchScale));
	lButton->SetRoundedStyle(int(0.32f*lTouchScale));
	lButton->UpdateLayout();
	return lButton;
}

void Menu::DismissDialog()
{
	if (mDialog)
	{
		mDialog->Dismiss();
		mDialog = 0;
	}
}

Cure::ResourceManager* Menu::GetResourceManager() const
{
	return mResourceManager;
}

void Menu::OnAction(UiTbc::Button* pButton)
{
	UiTbc::Dialog* d = mDialog;
	mButtonDelegate(pButton);
	if (d->IsAutoDismissButton(pButton))
	{
		mButtonDelegate.clear();
		mDialog = 0;
	}
}

void Menu::OnTapSound(UiTbc::Button*)
{
	if (mTapClick && mTapClick->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		mUiManager->GetSoundManager()->Play(mTapClick->GetData(), 1, Random::Uniform(1-mTapPitchOffset, 1+mTapPitchOffset));
	}
}

void Menu::SoundLoadCallback(UiCure::UserSound2dResource*)
{
}



}
