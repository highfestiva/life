
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "GameClientSlaveManager.h"
#include "RoadSignButton.h"



namespace Life
{



// This is just a simple viewer that shows a background for menus and shows off in demo mode.
class GameClientViewer: public GameClientSlaveManager
{
	typedef GameClientSlaveManager Parent;
public:
	GameClientViewer(GameClientMasterTicker* pMaster, Cure::RuntimeVariableScope* pVariableScope,
		Cure::ResourceManager* pResourceManager, UiCure::GameUiManager* pUiManager, int pSlaveIndex,
		const PixelRect& pRenderArea);
	virtual ~GameClientViewer();

private:
	virtual void TickUiUpdate();
	virtual void CreateLoginView();
	virtual bool InitializeTerrain();
	virtual void OnLoadCompleted(Cure::ContextObject* pObject, bool pOk);
	void CreateButton(float x, float y, float z, const str& pName, const str& pClass, const str& pTexture, RoadSignButton::Shape pShape);
	void OnButtonClick(UiTbc::Button* pButton);

	virtual bool OnKeyDown(UiLepra::InputManager::KeyCode);
	virtual bool OnKeyUp(UiLepra::InputManager::KeyCode);
	virtual void OnInput(UiLepra::InputElement*);

	Cure::GameObjectId mBackdropVehicleId;
};



}
