
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../lepra/include/lepratarget.h"
#include "fireticker.h"
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
#include "firemanager.h"



namespace Fire {



FireTicker::FireTicker(UiCure::GameUiManager* ui_manager, cure::ResourceManager* resource_manager, float physics_radius, int physics_levels, float physics_sensitivity):
	Parent(ui_manager, resource_manager, physics_radius, physics_levels, physics_sensitivity),
	env_map_(0) {
	v_override(UiCure::GetSettings(), kRtvarUi3DEnablemassobjectfading, false);
	v_set(UiCure::GetSettings(), kRtvarPhysicsIsfixedfps, true);
	v_set(UiCure::GetSettings(), kRtvarUi2DFontheight, 48.0);

	AddBackedRtvar(kRtvarGameFirstRun);
	AddBackedRtvar(kRtvarGameStartLevel);
	AddBackedRtvar(kRtvarUiSoundMastervolume);
}

FireTicker::~FireTicker() {
	CloseMainMenu();
	delete env_map_;
	env_map_ = 0;
}



void FireTicker::Suspend(bool hard) {
	Parent::Suspend(hard);
}

void FireTicker::Resume(bool hard) {
	Parent::Resume(hard);
}

bool FireTicker::CreateSlave() {
	return (Parent::CreateSlave(&FireTicker::CreateSlaveManager));
}

void FireTicker::OnSlavesKilled() {
	CreateSlave();
}

void FireTicker::OnServerCreated(life::UiGameServerManager*) {
}



bool FireTicker::OpenUiManager() {
	bool ok = true;
	if (ok) {
		ok = ui_manager_->OpenDraw();
	}
	if (ok) {
		uilepra::Core::ProcessMessages();
	}
	if (ok) {
		ui_manager_->UpdateSettings();
		uitbc::Renderer* renderer = ui_manager_->GetRenderer();
		renderer->AddDynamicRenderer("particle", new uitbc::ParticleRenderer(renderer, 0));
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
		if (ui_manager_->GetCanvas()->GetHeight() < 600) {
			double font_height;
			v_get(font_height, =, UiCure::GetSettings(), kRtvarUi2DFontheight, 48.0);
			if (font_height > 47.0) {
				font_height *= ui_manager_->GetCanvas()->GetHeight()/600.0;
				v_set(UiCure::GetSettings(), kRtvarUi2DFontheight, font_height);
			}
		}
		ok = ui_manager_->OpenRest();
	}
	if (ok) {
		ui_manager_->GetDesktopWindow()->CreateLayer(new uitbc::FloatingLayout());
	}
	return ok;
}

void FireTicker::BeginRender(vec3& color) {
	Parent::BeginRender(color);
	ui_manager_->GetRenderer()->SetOutlineFillColor(OFF_BLACK);
}

void FireTicker::PreWaitPhysicsTick() {
	Parent::PreWaitPhysicsTick();
}



void FireTicker::CloseMainMenu() {
}

bool FireTicker::QueryQuit() {
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



life::GameClientSlaveManager* FireTicker::CreateSlaveManager(life::GameClientMasterTicker* pMaster,
	cure::TimeManager* time, cure::RuntimeVariableScope* variable_scope,
	cure::ResourceManager* resource_manager, UiCure::GameUiManager* ui_manager,
	int slave_index, const PixelRect& render_area) {
	return new FireManager(pMaster, time, variable_scope, resource_manager, ui_manager, slave_index, render_area);
}



loginstance(kGame, FireTicker);



}
