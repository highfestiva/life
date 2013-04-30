/*
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "HeliForceManager.h"
#include "RoadSignButton.h"
#include "ServerListView.h"



namespace HeliForce
{



// This is just a simple viewer that shows a background for menus and shows off in demo mode.
class HeliForceViewer: public HeliForceManager, public ServerSelectObserver
{
	typedef HeliForceManager Parent;
public:
	HeliForceViewer(Life::GameClientMasterTicker* pMaster, const Cure::TimeManager* pTime,
		Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager,
		UiCure::GameUiManager* pUiManager, int pSlaveIndex, const PixelRect& pRenderArea);
	virtual ~HeliForceViewer();

private:
	virtual void LoadSettings();
	virtual void SaveSettings();
	virtual bool Open();
	virtual void TickUiInput();
	virtual void TickUiUpdate();
	virtual void CreateLoginView();
	virtual bool InitializeUniverse();
	virtual void OnLoadCompleted(Cure::ContextObject* pObject, bool pOk);
	virtual void OnCancelJoinServer();
	virtual void OnRequestJoinServer(const str& pServerAddress);
	virtual bool UpdateServerList(Life::ServerInfoList& pServerList) const;
	virtual bool IsMasterServerConnectError() const;
	void CloseJoinServerView();
	RoadSignButton* CreateButton(float x, float y, float z, const str& pName, const str& pClass, const str& pTexture, RoadSignButton::Shape pShape);
	void OnButtonClick(UiTbc::Button* pButton);

	ServerListView* mServerListView;

	LOG_CLASS_DECLARE();
};



}
*/