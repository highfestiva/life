
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "GameClientDemo.h"
//#ifdef LIFE_DEMO
#include "../../UiTBC/Include/GUI/UiDesktopWindow.h"
#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Cure/Include/TimeManager.h"
#include "../../Lepra/Include/Random.h"
#include "../../Lepra/Include/SystemManager.h"
#include "../../TBC/Include/ChunkyPhysics.h"
#include "../../UiCure/Include/UiRuntimeVariableName.h"
#include "../../UiTBC/Include/UiFontManager.h"
#include "GameClientMasterTicker.h"
#include "Vehicle.h"



namespace Life
{



GameClientDemo::GameClientDemo(GameClientMasterTicker* pMaster, Cure::RuntimeVariableScope* pVariableScope,
	Cure::ResourceManager* pResourceManager, UiCure::GameUiManager* pUiManager, int pSlaveIndex,
	const PixelRect& pRenderArea):
	Parent(pMaster, pVariableScope, pResourceManager, pUiManager, pSlaveIndex, pRenderArea),
	mCameraAngle(0),
	mInfoTextX(-10000),
	mInfoTextTargetY(-100),
	mInfoTextSlideY(-100),
	mCurrentInfoTextIndex(0)
{
	pUiManager->GetPainter()->ClearFontBuffers();
}

GameClientDemo::~GameClientDemo()
{
}



bool GameClientDemo::Paint()
{
	const double lFontHeight = 70.0;
	const UiTbc::FontManager::FontId lOldFontId = SetFontHeight(lFontHeight);

	if (mUiManager->GetFontManager()->GetStringWidth(mInfoText)+mInfoTextX < 0)
	{
		mInfoTextX = mUiManager->GetDisplayManager()->GetWidth() * 1.5f;
		mInfoTextTargetY = (float)Random::Uniform(20, mUiManager->GetDisplayManager()->GetHeight()-lFontHeight-60);
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
	const float lFrameTime = GetConstTimeManager()->GetNormalFrameTime();
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

void GameClientDemo::TickUiInput()
{
}

void GameClientDemo::TickUiUpdate()
{
	mCameraPreviousPosition = mCameraPosition;
	Cure::ContextObject* lObject = GetContext()->GetObject(mAvatarId);
	if (!lObject)
	{
		return;
	}
	mCameraPivotPosition = lObject->GetPosition();
	QuaternionF lRotation;
	lRotation.RotateAroundWorldZ(mCameraAngle);
	mCameraPosition = mCameraPivotPosition - lRotation*Vector3DF(10+sin(mCameraAngle*4.3f), 0, 0);
	mCameraPreviousPosition = mCameraPosition;
	mCameraOrientation = Vector3DF(mCameraAngle, PIF/2, 0);
	mCameraAngle += GetConstTimeManager()->GetNormalFrameTime();
}

void GameClientDemo::CreateLoginView()
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
	lButton->SetOnClick(GameClientDemo, BrowseFullInfo);
	mUiManager->GetDesktopWindow()->AddChild(lButton);
	lButton->SetPos(lMargin, mUiManager->GetDisplayManager()->GetHeight()-lButtonHeight-lMargin);
}

bool GameClientDemo::InitializeTerrain()
{
	Cure::ContextObject* lVehicle = new Vehicle(GetResourceManager(), _T("saucer_01"), mUiManager);
	GetContext()->AddLocalObject(lVehicle);
	mAvatarId = lVehicle->GetInstanceId();
	lVehicle->SetPhysicsTypeOverride(Cure::ContextObject::PHYSICS_OVERRIDE_STATIC);
	lVehicle->StartLoading();
	return (true);
}

void GameClientDemo::OnLoadCompleted(Cure::ContextObject* pObject, bool pOk)
{
	if (pOk)
	{
	}
	else
	{
		Parent::OnLoadCompleted(pObject, pOk);
	}
}

void GameClientDemo::BrowseFullInfo(UiTbc::Button*)
{
	SystemManager::WebBrowseTo(_T("http://trialepicfail.blogspot.com"));
	SystemManager::AddQuitRequest(+1);
}



bool GameClientDemo::OnKeyDown(UiLepra::InputManager::KeyCode)
{
	return (false);
}

bool GameClientDemo::OnKeyUp(UiLepra::InputManager::KeyCode)
{
	return (false);
}

void GameClientDemo::OnInput(UiLepra::InputElement*)
{
}



const tchar* GameClientDemo::mInfoTextArray[6] =
{
	_T("The vehicle on display is only playable in the full version."),
	_T("In the full version you can play online. Playing with your kids from the other side of the planet has never been easier."),
	_T("Play with up to four simultanous players ON ONE SCREEN in the full version - this free demo version adds annoying texts for 3/4 players."),
	_T("Ten more groovy vehicles are available in the full version."),
	_T("Host a dedicated game server for family and friends. Only in the full version."),
	_T("The development of this game was a huge undertaking by a single individual. Hope you enjoyed it!"),
};



}
//#endif //Demo
