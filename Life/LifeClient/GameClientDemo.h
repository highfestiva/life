
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "GameClientSlaveManager.h"
#include "RoadSignButton.h"



#ifdef LIFE_DEMO
namespace Life
{



// This is just a simple viewer that shows a background for menus and shows off in demo mode.
class GameClientDemo: public GameClientSlaveManager
{
	typedef GameClientSlaveManager Parent;
public:
	GameClientDemo(GameClientMasterTicker* pMaster, Cure::RuntimeVariableScope* pVariableScope,
		Cure::ResourceManager* pResourceManager, UiCure::GameUiManager* pUiManager, int pSlaveIndex,
		const PixelRect& pRenderArea);
	virtual ~GameClientDemo();

private:
	virtual bool Paint();
	virtual void TickUiInput();
	virtual void TickUiUpdate();
	virtual void CreateLoginView();
	virtual bool InitializeTerrain();
	virtual void OnLoadCompleted(Cure::ContextObject* pObject, bool pOk);
	void BrowseFullInfo(UiTbc::Button*);

	virtual bool OnKeyDown(UiLepra::InputManager::KeyCode pKeyCode);
	virtual bool OnKeyUp(UiLepra::InputManager::KeyCode pKeyCode);
	virtual void OnInput(UiLepra::InputElement* pElement);

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
