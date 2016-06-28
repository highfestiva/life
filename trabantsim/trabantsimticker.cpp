
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../lepra/include/lepratarget.h"
#include "trabantsimticker.h"
#include "../lepra/include/systemmanager.h"
#include "../uicure/include/uigameuimanager.h"
#include "../uicure/include/uimusicplayer.h"
#include "../uicure/include/uiparticleloader.h"
#include "../uicure/include/uiresourcemanager.h"
#include "../uilepra/include/uicore.h"
#include "../uitbc/include/gui/uidesktopwindow.h"
#include "../uitbc/include/gui/uifloatinglayout.h"
#include "../uitbc/include/uiparticlerenderer.h"
#include "rtvar.h"
#include "trabantsimmanager.h"



namespace TrabantSim {



TrabantSimTicker::TrabantSimTicker(UiCure::GameUiManager* ui_manager, cure::ResourceManager* resource_manager, float physics_radius, int physics_levels, float physics_sensitivity):
	Parent(ui_manager, resource_manager, physics_radius, physics_levels, physics_sensitivity),
	env_map_(0) {
	v_override(UiCure::GetSettings(), kRtvarCtrlDisableWinMgr, false);
	v_override(UiCure::GetSettings(), kRtvarUi3DCamlookatx, 0.0);
	v_override(UiCure::GetSettings(), kRtvarUi3DCamlookaty, 0.0);
	v_override(UiCure::GetSettings(), kRtvarUi3DCamlookatz, 0.0);
	v_override(UiCure::GetSettings(), kRtvarUi3DCamslide, 0.5);
	v_override(UiCure::GetSettings(), kRtvarUi3DCamdistance, 3.0);
	v_override(UiCure::GetSettings(), kRtvarUi3DCamanglex, 0.0);
	v_override(UiCure::GetSettings(), kRtvarUi3DCamangley, 0.0);
	v_override(UiCure::GetSettings(), kRtvarUi3DCamanglez, 0.0);
	v_override(UiCure::GetSettings(), kRtvarUi3DCamrotatex, 0.0);
	v_override(UiCure::GetSettings(), kRtvarUi3DCamrotatey, 0.0);
	v_override(UiCure::GetSettings(), kRtvarUi3DCamrotatez, 0.0);
	v_override(UiCure::GetSettings(), kRtvarUi3DCamsmooth, 0.0);
	v_override(UiCure::GetSettings(), kRtvarUi3DCamtargetobject, 0);
	v_override(UiCure::GetSettings(), kRtvarUi3DCamanglerelative, false);
	v_override(UiCure::GetSettings(), kRtvarUi3DLightanglex, -1.4);
	v_override(UiCure::GetSettings(), kRtvarUi3DLightanglez, 0.1);
	v_override(UiCure::GetSettings(), kRtvarUiPenred, 0.7);
	v_override(UiCure::GetSettings(), kRtvarUiPengreen, 0.3);
	v_override(UiCure::GetSettings(), kRtvarUiPenblue, 0.6);
	v_override(UiCure::GetSettings(), kRtvarUiPenalpha, 1.0);
	v_override(UiCure::GetSettings(), kRtvarUi3DEnableclear, true);
	v_override(UiCure::GetSettings(), kRtvarUi3DClearred, 1.0);
	v_override(UiCure::GetSettings(), kRtvarUi3DCleargreen, 1.0);
	v_override(UiCure::GetSettings(), kRtvarUi3DClearblue, 1.0);
	v_override(UiCure::GetSettings(), kRtvarUi3DOutlinemode, true);

	//v_override(UiCure::GetSettings(), kRtvarUi2DFontheight, 30.0);

	v_override(UiCure::GetSettings(), kRtvarGameAllowpowerdown, true);
	v_override(UiCure::GetSettings(), kRtvarGameUsermessage, "");

	v_override(UiCure::GetSettings(), kRtvarPhysicsFps, 60);
	v_override(UiCure::GetSettings(), kRtvarPhysicsIsfixedfps, true);
	v_override(UiCure::GetSettings(), kRtvarPhysicsBounce, 0.2);
	v_override(UiCure::GetSettings(), kRtvarPhysicsFriction, 0.5);
	v_override(UiCure::GetSettings(), kRtvarPhysicsGravityx, 0.0);
	v_override(UiCure::GetSettings(), kRtvarPhysicsGravityy, 0.0);
	v_override(UiCure::GetSettings(), kRtvarPhysicsGravityz, -9.8);

	v_set(UiCure::GetSettings(), "Simulator.DeniedHosts", "");
	v_set(UiCure::GetSettings(), "Simulator.AllowedHosts", "");
	v_set(UiCure::GetSettings(), "Simulator.AllowRemoteSync", false);

	AddBackedRtvar("Simulator.DeniedHosts");
	AddBackedRtvar("Simulator.AllowedHosts");
	AddBackedRtvar("Simulator.AllowRemoteSync");
}

TrabantSimTicker::~TrabantSimTicker() {
	CloseMainMenu();
	delete env_map_;
	env_map_ = 0;
}



bool TrabantSimTicker::CreateSlave() {
	return (Parent::CreateSlave(&TrabantSimTicker::CreateSlaveManager));
}

void TrabantSimTicker::OnSlavesKilled() {
	CreateSlave();
}

void TrabantSimTicker::OnServerCreated(life::UiGameServerManager*) {
}

float TrabantSimTicker::GetPowerSaveAmount() const {
	TrabantSimManager* manager = (TrabantSimManager*)slave_array_[0];
	if (!manager) {
		return 0;
	}
	return manager->IsControlled()? Parent::GetPowerSaveAmount() : 0.2f;
}



bool TrabantSimTicker::OpenUiManager() {
	bool ok = true;
	if (ok) {
		ok = ui_manager_->OpenDraw();
	}
	if (ok) {
		ui_manager_->GetDisplayManager()->SetCaption("Trabant Simulator");
		uilepra::Core::ProcessMessages();
	}
	if (ok) {
		ui_manager_->UpdateSettings();
		uitbc::Renderer* renderer = ui_manager_->GetRenderer();
		renderer->AddDynamicRenderer("particle", new uitbc::ParticleRenderer(renderer, 1));
		UiCure::ParticleLoader loader(resource_manager_, renderer, "explosion.png", 4, 5);
	}
	if (ok) {
		env_map_ = new UiCure::RendererImageResource(ui_manager_, resource_manager_, "env.png", UiCure::ImageProcessSettings(Canvas::kResizeFast, true));
		if (env_map_->Load()) {
			if (env_map_->PostProcess() == cure::kResourceLoadComplete) {
				uitbc::Renderer::TextureID texture_id = env_map_->GetUserData(0);
				ui_manager_->GetRenderer()->SetEnvironmentMap(texture_id);
			}
		}
	}
	if (ok) {
		ok = ui_manager_->OpenRest();
	}
	if (ok) {
		ui_manager_->GetDesktopWindow()->CreateLayer(new uitbc::FloatingLayout());
	}
	return ok;
}

void TrabantSimTicker::BeginRender(vec3& color) {
	Parent::BeginRender(color);
	ui_manager_->GetRenderer()->SetOutlineFillColor(ui_manager_->GetRenderer()->GetClearColor());
}



void TrabantSimTicker::CloseMainMenu() {
}

bool TrabantSimTicker::QueryQuit() {
	if (Parent::QueryQuit()) {
		PrepareQuit();
		for (int x = 0; x < 4; ++x) {
			DeleteSlave(slave_array_[x], false);
		}
		DeleteServer();
		return (true);
	}
	return (false);
}



life::GameClientSlaveManager* TrabantSimTicker::CreateSlaveManager(life::GameClientMasterTicker* pMaster,
	cure::TimeManager* time, cure::RuntimeVariableScope* variable_scope,
	cure::ResourceManager* resource_manager, UiCure::GameUiManager* ui_manager,
	int slave_index, const PixelRect& render_area) {
	return new TrabantSimManager(pMaster, time, variable_scope, resource_manager, ui_manager, slave_index, render_area);
}



loginstance(kGame, TrabantSimTicker);



}
