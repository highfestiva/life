
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "GameClientSlaveManager.h"
#include "RoadSignButton.h"
#include "ServerListView.h"



namespace Life
{



// This is just a simple viewer that shows a background for menus and shows off in demo mode.
class GameClientViewer: public GameClientSlaveManager, public ServerSelectObserver
{
	typedef GameClientSlaveManager Parent;
public:
	GameClientViewer(GameClientMasterTicker* pMaster, const Cure::TimeManager* pTime,
		Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager,
		UiCure::GameUiManager* pUiManager, int pSlaveIndex, const PixelRect& pRenderArea);
	virtual ~GameClientViewer();

private:
	virtual void TickUiInput();
	virtual void TickUiUpdate();
	virtual void CreateLoginView();
	virtual bool InitializeTerrain();
	virtual void OnLoadCompleted(Cure::ContextObject* pObject, bool pOk);
	virtual void OnCancelJoinServer();
	virtual void OnRequestJoinServer(const str& pServerAddress);
	virtual bool UpdateServerList(ServerInfoList& pServerList) const;
	virtual bool IsMasterServerConnectError() const;
	void CloseJoinServerView();
	RoadSignButton* CreateButton(float x, float y, float z, const str& pName, const str& pClass, const str& pTexture, RoadSignButton::Shape pShape);
	void OnButtonClick(UiTbc::Button* pButton);

	ServerListView* mServerListView;

	LOG_CLASS_DECLARE();
};



}
