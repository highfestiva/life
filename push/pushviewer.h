
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "pushmanager.h"
#include "roadsignbutton.h"
#include "serverlistview.h"



namespace Push {



// This is just a simple viewer that shows a background for menus and shows off in demo mode.
class PushViewer: public PushManager, public ServerSelectObserver {
	typedef PushManager Parent;
public:
	PushViewer(life::GameClientMasterTicker* pMaster, const cure::TimeManager* time,
		cure::RuntimeVariableScope* variable_scope, cure::ResourceManager* resource_manager,
		UiCure::GameUiManager* ui_manager, int slave_index, const PixelRect& render_area);
	virtual ~PushViewer();

private:
	virtual void LoadSettings();
	virtual void SaveSettings();
	virtual bool Open();
	virtual void TickUiInput();
	virtual void TickUiUpdate();
	virtual void CreateLoginView();
	virtual bool InitializeUniverse();
	virtual void OnLoadCompleted(cure::ContextObject* object, bool ok);
	virtual void OnCancelJoinServer();
	virtual void OnRequestJoinServer(const str& server_address);
	virtual bool UpdateServerList(life::ServerInfoList& server_list) const;
	virtual bool IsMasterServerConnectError() const;
	void CloseJoinServerView();
	RoadSignButton* CreateButton(float x, float y, float z, const str& name, const str& clazz, const str& texture, RoadSignButton::Shape shape);
	void OnButtonClick(uitbc::Button* button);

	ServerListView* server_list_view_;

	logclass();
};



}
