
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "pushviewer.h"
#include "../cure/include/contextmanager.h"
#include "../cure/include/runtimevariable.h"
#include "../tbc/include/chunkyphysics.h"
#include "../uitbc/include/gui/uidesktopwindow.h"
#include "../uitbc/include/gui/uicenterlayout.h"
#include "../uicure/include/uigameuimanager.h"
#include "../uicure/include/uigravelemitter.h"
#include "../uicure/include/uimachine.h"
#include "../life/lifeclient/clientconsolemanager.h"
#include "../life/lifeclient/gameclientmasterticker.h"
#include "../life/lifeclient/level.h"
#include "../life/lifeclient/uiconsole.h"
#include "../life/lifeserver/masterserverconnection.h"
#include "rtvar.h"



namespace Push {



PushViewer::PushViewer(life::GameClientMasterTicker* pMaster, const cure::TimeManager* time,
	cure::RuntimeVariableScope* variable_scope, cure::ResourceManager* resource_manager,
	UiCure::GameUiManager* ui_manager, int slave_index, const PixelRect& render_area):
	Parent(pMaster, time, variable_scope, resource_manager, ui_manager, slave_index, render_area),
	server_list_view_(0) {
	camera_position_ = vec3(-22, -5, 43.1f);
	camera_orientation_ = vec3(-PIF*1.1f/2, PIF*0.86f/2, 0.05f);
}

PushViewer::~PushViewer() {
	CloseJoinServerView();
}



void PushViewer::LoadSettings() {
	v_set(GetVariableScope(), kRtvarDebugInputPrint, false);

	v_internal(GetVariableScope(), kRtvarUi3DCamdistance, 20.0);
	v_internal(GetVariableScope(), kRtvarUi3DCamheight, 10.0);
	v_internal(GetVariableScope(), kRtvarUi3DCamrotate, 0.0);
	v_internal(GetVariableScope(), kRtvarSteeringPlaybackmode, kPlaybackNone);
}

void PushViewer::SaveSettings() {
}

bool PushViewer::Open() {
	return life::GameClientSlaveManager::Open();
}

void PushViewer::TickUiInput() {
}

void PushViewer::TickUiUpdate() {
	((life::ClientConsoleManager*)GetConsoleManager())->GetUiConsole()->Tick();

	if (server_list_view_) {
		server_list_view_->Tick();
	}

	/*camera_previous_position_ = camera_position_;
	cure::ContextObject* _object = GetContext()->GetObject(mBackdropVehicleId);
	if (!_object) {
		return;
	}
	camera_pivot_position_ = _object->GetPosition();
	camera_position_ = camera_pivot_position_ - vec3(10, 0, 0);
	camera_previous_position_ = camera_position_;
	camera_orientation_ = vec3(0, PIF/2, 0);*/
}

void PushViewer::CreateLoginView() {
	quat flip;
	flip.RotateAroundOwnZ(PIF);
	CreateButton(-0.2f, +0.2f,  6.0f, "1",	"road_sign_02", "road_sign_1p.png", RoadSignButton::kShapeBox);
	RoadSignButton* _button = CreateButton(+0.2f, +0.2f,  6.0f, "2",	"road_sign_02", "road_sign_2p.png", RoadSignButton::kShapeBox);
	_button->SetOrientation(flip);
	CreateButton(-0.2f, -0.2f,  6.0f, "3",	"road_sign_02", "road_sign_3p.png", RoadSignButton::kShapeBox);
	_button = CreateButton(+0.2f, -0.2f,  6.0f, "4",	"road_sign_02", "road_sign_4p.png", RoadSignButton::kShapeBox);
	_button->SetOrientation(flip);

	CreateButton(-0.4f, +0.4f, 12.0f, "server",	"road_sign_01", "road_sign_roundabout.png", RoadSignButton::kShapeRound);

	CreateButton(+0.4f, +0.4f, 12.0f, "quit",	"road_sign_01", "road_sign_nostop.png", RoadSignButton::kShapeRound);
}

bool PushViewer::InitializeUniverse() {
	if (!Parent::InitializeUniverse()) {
		return (false);
	}

	UiCure::GravelEmitter* gravel_particle_emitter = new UiCure::GravelEmitter(GetResourceManager(), ui_manager_, 0.5f, 1, 10, 2);
	level_ = new life::Level(GetResourceManager(), "level_01", ui_manager_, gravel_particle_emitter);
	AddContextObject(level_, cure::kNetworkObjectRemoteControlled, 0);
	level_->EnableRootShadow(false);
	level_->SetAllowNetworkLogic(false);
	level_->StartLoading();

	cure::ContextObject* vehicle = new UiCure::Machine(GetResourceManager(), "monster_02", ui_manager_);
	GetContext()->AddLocalObject(vehicle);
	vehicle->SetInitialTransform(xform(kIdentityQuaternionF, vec3(-23, -80, 53)));
	vehicle->StartLoading();
	avatar_id_ = vehicle->GetInstanceId();
	GetConsoleManager()->ExecuteCommand("fork execute-file data/steering.rec");
	return (true);
}

void PushViewer::OnLoadCompleted(cure::ContextObject* object, bool ok) {
	if (ok) {
	} else {
		Parent::OnLoadCompleted(object, ok);
	}
}

void PushViewer::OnCancelJoinServer() {
	CloseJoinServerView();
}

void PushViewer::OnRequestJoinServer(const str& server_address) {
	v_set(GetVariableScope(), kRtvarNetworkServeraddress, server_address);
	v_internal(UiCure::GetSettings(), kRtvarLoginIsserverselected, true);
	log_.Infof("Will use server %s when logging in.", server_address.c_str());
	CloseJoinServerView();
}

bool PushViewer::UpdateServerList(life::ServerInfoList& server_list) const {
	if (GetMaster()->GetMasterServerConnection()) {	// TRICKY: uses master ticker's connection, as this is the one that downloads server lists!
		return GetMaster()->GetMasterServerConnection()->UpdateServerList(server_list);
	}
	return false;
}

bool PushViewer::IsMasterServerConnectError() const {
	if (GetMaster()->GetMasterServerConnection()) {	// TRICKY: uses master ticker's connection, as this is the one that downloads server lists!
		return GetMaster()->GetMasterServerConnection()->IsConnectError();
	}
	return true;
}

void PushViewer::CloseJoinServerView() {
	if (server_list_view_) {
		ui_manager_->GetDesktopWindow()->RemoveChild(server_list_view_, 1);
		delete (server_list_view_);
		server_list_view_ = 0;
	}
}

RoadSignButton* PushViewer::CreateButton(float x, float y, float z, const str& name, const str& clazz, const str& texture, RoadSignButton::Shape shape) {
	RoadSignButton* _button = new RoadSignButton(this, GetResourceManager(), ui_manager_, name, clazz, texture, shape);
	GetContext()->AddLocalObject(_button);
	_button->SetTrajectory(vec2(x, y), z);
	_button->GetButton().SetOnClick(PushViewer, OnButtonClick);
	road_sign_map_.insert(RoadSignMap::value_type(_button->GetInstanceId(), _button));
	_button->StartLoading();
	return (_button);
}

void PushViewer::OnButtonClick(uitbc::Button* button) {
	if (button->GetName() == "server") {
		if (!server_list_view_) {
			GetMaster()->DownloadServerList();
			server_list_view_ = new ServerListView(this);
			ui_manager_->AssertDesktopLayout(new uitbc::CenterLayout, 1);
			ui_manager_->GetDesktopWindow()->AddChild(server_list_view_, 0, 0, 1);
		}
		v_set(GetVariableScope(), kRtvarNetworkEnableonlinemaster, true);
		return;
	}
	if (button->GetName() == "quit") {
		GetMaster()->OnExit();
		return;
	}
	int value = 0;
	if (strutil::StringToInt(button->GetName(), value)) {
		GetMaster()->OnSetPlayerCount(value);
	} else {
		deb_assert(false);
	}
}



loginstance(kGame, PushViewer);



}
