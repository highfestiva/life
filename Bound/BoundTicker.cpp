
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../lepra/include/lepratarget.h"
#include "boundticker.h"
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
#include "boundmanager.h"



namespace Bound {



BoundTicker::BoundTicker(UiCure::GameUiManager* ui_manager, cure::ResourceManager* resource_manager, float physics_radius, int physics_levels, float physics_sensitivity):
	Parent(ui_manager, resource_manager, physics_radius, physics_levels, physics_sensitivity),
	is_player_count_view_active_(false),
	music_player_(0),
	env_map_(0) {
	v_override(UiCure::GetSettings(), kRtvarUi3DEnablemassobjectfading, false);
	v_set(UiCure::GetSettings(), kRtvarPhysicsFps, 45);
	v_set(UiCure::GetSettings(), kRtvarPhysicsIsfixedfps, true);
	v_set(UiCure::GetSettings(), kRtvarUi2DFontheight, 30.0);

	AddBackedRtvar(kRtvarGameFirsttime);
	AddBackedRtvar(kRtvarGameLevel);
	AddBackedRtvar(kRtvarGameLevelshapealternate);
	AddBackedRtvar(kRtvarGameRunads);
	AddBackedRtvar(kRtvarGameScore);
}

BoundTicker::~BoundTicker() {
	CloseMainMenu();
	delete music_player_;
	music_player_ = 0;
	delete env_map_;
	env_map_ = 0;
}



void BoundTicker::Suspend(bool hard) {
	Parent::Suspend(hard);

	if (music_player_) {
		music_player_->Pause();
	}
}

void BoundTicker::Resume(bool hard) {
	Parent::Resume(hard);

	if (music_player_) {
		music_player_->Stop();
		music_player_->Playback();
	}
}

bool BoundTicker::CreateSlave() {
	return (Parent::CreateSlave(&BoundTicker::CreateSlaveManager));
}

void BoundTicker::OnSlavesKilled() {
	CreateSlave();
}

void BoundTicker::OnServerCreated(life::UiGameServerManager*) {
}



bool BoundTicker::OpenUiManager() {
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
			v_get(font_height, =, UiCure::GetSettings(), kRtvarUi2DFontheight, 30.0);
			if (font_height > 29.0) {
				font_height *= ui_manager_->GetCanvas()->GetHeight()/600.0;
				v_set(UiCure::GetSettings(), kRtvarUi2DFontheight, font_height);
			}
		}
		ok = ui_manager_->OpenRest();
	}
	if (ok) {
		music_player_ = new UiCure::MusicPlayer(ui_manager_->GetSoundManager());
		music_player_->SetVolume(0.5f);
		music_player_->SetSongPauseTime(2, 6);
		/*music_player_->AddSong("Dub Feral.ogg");
		music_player_->AddSong("Easy Jam.ogg");
		music_player_->AddSong("Firmament.ogg");
		music_player_->AddSong("Mandeville.ogg");
		music_player_->AddSong("Slow Ska Game Loop.ogg");
		music_player_->AddSong("Stealth Groover.ogg");
		music_player_->AddSong("Yallahs.ogg");*/
		music_player_->Shuffle();
		ok = music_player_->Playback();
	}
	if (ok) {
		ui_manager_->GetDesktopWindow()->CreateLayer(new uitbc::FloatingLayout());
	}
	return ok;
}

void BoundTicker::BeginRender(vec3& color) {
	Parent::BeginRender(color);
	ui_manager_->GetRenderer()->SetOutlineFillColor(OFF_BLACK);
}

void BoundTicker::PreWaitPhysicsTick() {
	Parent::PreWaitPhysicsTick();
	if (music_player_) {
		music_player_->Update();
	}
}



void BoundTicker::CloseMainMenu() {
	if (is_player_count_view_active_) {
		DeleteSlave(slave_array_[0], false);
		is_player_count_view_active_ = false;
	}
}

bool BoundTicker::QueryQuit() {
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



life::GameClientSlaveManager* BoundTicker::CreateSlaveManager(life::GameClientMasterTicker* pMaster,
	cure::TimeManager* time, cure::RuntimeVariableScope* variable_scope,
	cure::ResourceManager* resource_manager, UiCure::GameUiManager* ui_manager,
	int slave_index, const PixelRect& render_area) {
	return new BoundManager(pMaster, time, variable_scope, resource_manager, ui_manager, slave_index, render_area);
}



loginstance(kGame, BoundTicker);



}
