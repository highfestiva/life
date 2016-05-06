
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "serverlistview.h"
#include "../cure/include/runtimevariable.h"
#include "../cure/include/useraccount.h"
#include "../lepra/include/network.h"
#include "../lepra/include/number.h"
#include "../uitbc/include/gui/uicaption.h"
#include "../uitbc/include/gui/uicenterlayout.h"
#include "../uitbc/include/gui/uitextfield.h"
#include "rtvar.h"



namespace HoverTank {



ServerListView::ServerListView(ServerSelectObserver* select_observer):
	View("Join Online Game", new uitbc::GridLayout(11, 1)),
	select_observer_(select_observer),
	is_master_connect_error_(false) {
	server_list_.push_back(life::ServerInfo());

	SetPreferredSize(500, 350);

	uitbc::Component* row_layer = AddRow(Color(0, 0, 0), 4);
	AddLabel("Server Name", WHITE, 250, row_layer);
	AddLabel("Players", WHITE, 0, row_layer);
	AddLabel("Ping (ms"), WHITE, 0, row_layer);
	AddLabel("Select", WHITE, 0, row_layer);

	CreateLayer(new uitbc::CenterLayout);
}

void ServerListView::Tick() {
	// TRICKY! Gaah! Code is worse than it looks.
	if (!select_observer_->UpdateServerList(server_list_)) {
		if (!is_master_connect_error_ && select_observer_->IsMasterServerConnectError()) {
			is_master_connect_error_ = true;
			server_list_.clear();
			ReplaceLayer(1, new uitbc::CenterLayout);
			AddLabel("Problem connecting to master server.\nCheck your network cable and/or firewall settings.", RED, 0, this, 1)->SetPreferredSize(0, 0, true);
		}
		if (select_observer_->IsMasterServerConnectError() == is_master_connect_error_) {
			return;
		}
	}
	if (!select_observer_->IsMasterServerConnectError()) {
		is_master_connect_error_ = false;

		if (server_list_.size() == 0) {	// No response yet!
			ReplaceLayer(1, new uitbc::CenterLayout);
			AddLabel("Please wait while refreshing list...", GRAY, 0, this, 1)->SetAdaptive(true);
		} else if (server_list_.size() == 1) {	// Only the status message, no servers online!
			ReplaceLayer(1, new uitbc::CenterLayout);
			AddLabel("I'm sorry, but no-one else seems to be running a public server.\n"
				"Perhaps you would like to be the first to start one? Click here and there...", GRAY, 0, this, 1)->SetPreferredSize(0, 0, true);
		} else if (server_list_.size() > 1) {
			DeleteLayer(1);
		}
	}

	const int server_count = (int)server_list_.size() - 1;	// Ignore the status message.

	const uitbc::GridLayout* grid = (uitbc::GridLayout*)GetClientRectComponent()->GetLayout();
	for (int x = 1; x < 1+10; ++x) {
		uitbc::RectComponent* row_layer = (uitbc::RectComponent*)grid->GetComponentAt(x, 0);
		if (row_layer) {
			row_layer->DeleteChildrenInLayer(0);
		} else {
			row_layer = AddRow(DARK_GRAY, 4);
		}
		const bool more_servers = (x-1 < server_count);
		if (!more_servers) {
			row_layer->SetIsHollow(true);
		} else if (x&1) {
			row_layer->SetColor(Color(90, 90, 90));
			row_layer->SetIsHollow(false);
		} else {
			row_layer->SetColor(Color(96, 96, 96));
			row_layer->SetIsHollow(false);
		}
		if (more_servers) {
			const life::ServerInfo& server = server_list_[x-1];
			AddLabel(server.name_, GRAY, 250, row_layer)->SetAdaptive(false);
			AddLabel(strutil::IntToString(server.player_count_, 10), GRAY, 0, row_layer);
			const str ping = (server.ping_ > 0)? Number::ConvertToPostfixNumber(server.ping_, 0)+"s" : "";
			AddLabel(ping, GRAY, 0, row_layer);
			uitbc::RectComponent* center_layer = AddCentering(0, row_layer);
			uitbc::Button* _button = AddButton("Pick", (void*)(x-1), center_layer);
			_button->SetOnClick(ServerListView, OnSelect);
			_button->SetPreferredSize(100, 23, false);
		}
	}

	GetClientRectComponent()->UpdateLayout();
	//UpdateLayout();	TRICKY!
}



void ServerListView::OnExit() {
	select_observer_->OnCancelJoinServer();
}

void ServerListView::OnSelect(uitbc::Button* button) {
	const size_t server_index = (intptr_t)button->GetExtraData();
	if (server_index < server_list_.size()) {
		select_observer_->OnRequestJoinServer(server_list_[server_index].given_ip_address_ +
			strutil::Format(":%i", server_list_[server_index].given_port_));
	}
}



}
