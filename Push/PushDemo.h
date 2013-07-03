
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "PushManager.h"
#include "RoadSignButton.h"



#ifdef PUSH_DEMO
namespace Push
{



// This is just a simple viewer that shows a background for menus and shows off in demo mode.
class PushDemo: public PushManager
{
	typedef PushManager Parent;
public:
	PushDemo(GameClientMasterTicker* pMaster, const Cure::TimeManager* pTime,
		Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager,
		UiCure::GameUiManager* pUiManager, int pSlaveIndex, const PixelRect& pRenderArea);
	virtual ~PushDemo();

private:
	virtual bool Paint();
	virtual void TickUiInput();
	virtual void TickUiUpdate();
	virtual void CreateLoginView();
	virtual bool InitializeUniverse();
	virtual void OnLoadCompleted(Cure::ContextObject* pObject, bool pOk);
	void BrowseFullInfo(UiTbc::Button*);

	virtual bool OnKeyDown(UiLepra::InputManager::KeyCode pKeyCode);
	virtual bool OnKeyUp(UiLepra::InputManager::KeyCode pKeyCode);
	virtual void OnInput(UiLepra::InputElement* pElement);

	UiTbc::FontManager::FontId PushDemo::SetFontHeight(double pHeight);

	float mCameraAngle;

	float mInfoTextX;
	float mInfoTextTargetY;
	float mInfoTextSlideY;
	str mInfoText;
	int mCurrentInfoTextIndex;
	static const tchar* mInfoTextArray[6];
};



}
#endif // Demo
