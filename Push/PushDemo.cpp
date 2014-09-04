
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "PushDemo.h"
#ifdef PUSH_DEMO
#include "../../UiTbc/Include/GUI/UiDesktopWindow.h"
#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Cure/Include/TimeManager.h"
#include "../../Lepra/Include/Random.h"
#include "../../Lepra/Include/SystemManager.h"
#include "../../Tbc/Include/ChunkyPhysics.h"
#include "../../UiCure/Include/UiRuntimeVariableName.h"
#include "../../UiTbc/Include/UiFontManager.h"
#include "GameClientMasterTicker.h"
#include "Machine.h"



namespace Push
{



PushDemo::PushDemo(Life::GameClientMasterTicker* pMaster, const Cure::TimeManager* pTime,
	Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager,
	UiCure::GameUiManager* pUiManager, int pSlaveIndex, const PixelRect& pRenderArea):
	Parent(pMaster, pTime, pVariableScope, pResourceManager, pUiManager, pSlaveIndex, pRenderArea),
	mCameraAngle(0),
	mInfoTextX(-10000),
	mInfoTextTargetY(-100),
	mInfoTextSlideY(-100),
	mCurrentInfoTextIndex(0)
{
	pUiManager->GetPainter()->ClearFontBuffers();
}

PushDemo::~PushDemo()
{
}



Merge OnOpen stuff from PushViewer.cpp.



bool PushDemo::Paint()
{
	if (!mUiManager->GetDisplayManager()->IsVisible())
	{
		return true;
	}

	const double lFontHeight = 70.0;
	const UiTbc::FontManager::FontId lOldFontId = SetFontHeight(lFontHeight);

	if (mUiManager->GetFontManager()->GetStringWidth(mInfoText)+mInfoTextX < 0)
	{
		mInfoTextX = mUiManager->GetDisplayManager()->GetWidth() * 1.5f;
		mInfoTextTargetY = Random::Uniform(20.0f, (float)(mUiManager->GetDisplayManager()->GetHeight()-lFontHeight-60));
		const int lTextCount = sizeof(mInfoTextArray)/sizeof(void*);
		if (mInfoText.empty())
		{
			mCurrentInfoTextIndex = Random::GetRandomNumber() % lTextCount;
		}
		if (++mCurrentInfoTextIndex >= lTextCount)
		{
			mCurrentInfoTextIndex = 0;
		}
		mInfoText = mInfoTextArray[mCurrentInfoTextIndex];
	}
	const float lFrameTime = GetTimeManager()->GetRealNormalFrameTime();
	mInfoTextSlideY = Math::Lerp(mInfoTextSlideY, mInfoTextTargetY, Math::GetIterateLerpTime(0.8f, lFrameTime));

	const int lWidth = mUiManager->GetDisplayManager()->GetWidth();
	const int lBarMargin = 5;
	mUiManager->GetPainter()->PushAttrib(UiTbc::Painter::ATTR_RENDERMODE);
	mUiManager->GetPainter()->SetRenderMode(UiTbc::Painter::RM_ADD);
	mUiManager->GetPainter()->SetColor(Color(160, 160, 160, 180), 0);
	mUiManager->GetPainter()->FillRect(0, (int)mInfoTextSlideY-lBarMargin, lWidth-1, (int)(mInfoTextSlideY+mUiManager->GetFontManager()->GetFontHeight()+lBarMargin*2));
	mUiManager->GetPainter()->PopAttrib();
	mInfoTextX -= lFrameTime * 640;
	//mInfoTextX -= 12;
	mUiManager->GetPainter()->SetColor(Color(30, 10, 20, 220), 0);
	mUiManager->GetPainter()->SetColor(Color(0, 0, 0, 0), 1);
	mUiManager->GetPainter()->PrintText(mInfoText, (int)mInfoTextX, (int)mInfoTextTargetY);

	mUiManager->GetFontManager()->SetActiveFont(lOldFontId);

	return (true);
}

void PushDemo::TickUiInput()
{
}

void PushDemo::TickUiUpdate()
{
	mCameraPreviousPosition = mCameraPosition;
	Cure::ContextObject* lObject = GetContext()->GetObject(mAvatarId);
	if (!lObject)
	{
		return;
	}
	mCameraPivotPosition = lObject->GetPosition();
	quat lRotation;
	lRotation.RotateAroundWorldZ(mCameraAngle);
	mCameraPosition = mCameraPivotPosition - lRotation*vec3(10+sin(mCameraAngle*4.3f), 0, 0);
	mCameraPreviousPosition = mCameraPosition;
	mCameraOrientation = vec3(mCameraAngle, PIF/2, 0);
	mCameraAngle += GetTimeManager()->GetNormalFrameTime();
}

void PushDemo::CreateLoginView()
{
	UiTbc::Button* lButton = new UiTbc::Button(UiTbc::BorderComponent::LINEAR, 3, GRAY, _T("FullInfo"));
	lButton->SetText(_T("Click here for more information on the full version"), BLUE, BLUE);
	const int lButtonWidth = 370;
	const int lButtonHeight = 30;
	const int lMargin = 20;
	lButton->SetPreferredSize(lButtonWidth, lButtonHeight);
	lButton->SetMinSize(lButtonWidth, lButtonHeight);
	lButton->SetSize(lButtonWidth, lButtonHeight);
	lButton->UpdateLayout();
	lButton->SetOnClick(PushDemo, BrowseFullInfo);
	GetUiManager()->AssertDesktopLayout(new UiTbc::FloatingLayout, 0);
	mUiManager->GetDesktopWindow()->AddChild(lButton, 0, 0, 0);
	lButton->SetPos(lMargin, mUiManager->GetDisplayManager()->GetHeight()-lButtonHeight-lMargin);
}

bool PushDemo::InitializeUniverse()
{
	Merge level stuff from PushViewer.cpp.

	Cure::ContextObject* lVehicle = new Machine(GetResourceManager(), _T("saucer_01"), mUiManager);
	GetContext()->AddLocalObject(lVehicle);
	mAvatarId = lVehicle->GetInstanceId();
	lVehicle->SetPhysicsTypeOverride(Cure::PHYSICS_OVERRIDE_STATIC);
	lVehicle->StartLoading();
	return (true);
}

void PushDemo::OnLoadCompleted(Cure::ContextObject* pObject, bool pOk)
{
	if (pOk)
	{
	}
	else
	{
		Parent::OnLoadCompleted(pObject, pOk);
	}
}

void PushDemo::BrowseFullInfo(UiTbc::Button*)
{
	SystemManager::WebBrowseTo(_T("http://trialepicfail.blogspot.com"));
	SystemManager::AddQuitRequest(+1);
}



bool PushDemo::OnKeyDown(UiLepra::InputManager::KeyCode)
{
	return (false);
}

bool PushDemo::OnKeyUp(UiLepra::InputManager::KeyCode)
{
	return (false);
}

void PushDemo::OnInput(UiLepra::InputElement*)
{
}



UiTbc::FontManager::FontId PushDemo::SetFontHeight(double pHeight)
{
	const UiTbc::FontManager::FontId lPreviousFontId = mUiManager->GetFontManager()->GetActiveFontId();
	const str lFontName = mUiManager->GetFontManager()->GetActiveFontName();
	mUiManager->GetFontManager()->QueryAddFont(lFontName, pHeight, UiTbc::FontManager::BOLD);
	return (lPreviousFontId);
}



const tchar* PushDemo::mInfoTextArray[6] =
{
	_T("The vehicle on display is only playable in the full version."),
	_T("In the full version you can play online. Playing with your kids from the other side of the planet has never been easier."),
	_T("Play with up to four simultanous players ON ONE SCREEN in the full version - this free demo version adds annoying texts for 3/4 players."),
	_T("Ten more groovy vehicles are available in the full version."),
	_T("Host a dedicated game server for family and friends. Only in the full version."),
	_T("The development of this game was a huge undertaking by a single individual. Hope you enjoyed it!"),
};



}
#endif //Demo
