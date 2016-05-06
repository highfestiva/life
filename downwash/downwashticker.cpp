
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../lepra/include/lepratarget.h"
#include "downwashticker.h"
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
#include "downwashmanager.h"



namespace Downwash {



DownwashTicker::DownwashTicker(UiCure::GameUiManager* ui_manager, cure::ResourceManager* resource_manager, float physics_radius, int physics_levels, float physics_sensitivity):
	Parent(ui_manager, resource_manager, physics_radius, physics_levels, physics_sensitivity),
	is_player_count_view_active_(false),
	music_player_(0),
	env_map_(0) {
	v_override(UiCure::GetSettings(), kRtvarUi3DEnablemassobjectfading, false);
	v_set(UiCure::GetSettings(), kRtvarPhysicsIsfixedfps, true);
	v_set(UiCure::GetSettings(), kRtvarUi2DFontheight, 30.0);

	AddBackedRtvar(kRtvarGameStartLevel);
	AddBackedRtvar(kRtvarGameChildishness);
	for (int x = 0; x < 20; ++x) {
		const str wr = strutil::Format(str(kRtvarGameWorldrecordLevel "_%i").c_str(), x);
		AddBackedRtvar(wr);
		const str pr = strutil::Format(str(kRtvarGamePersonalrecordLevel "_%i").c_str(), x);
		AddBackedRtvar(pr);
	}
	AddBackedRtvar(kRtvarPhysicsRtrOffset);
	AddBackedRtvar(kRtvarGameAllowtoymode);
	AddBackedRtvar(kRtvarGamePilotname);
	AddBackedRtvar(kRtvarUiSoundMastervolume);
}

DownwashTicker::~DownwashTicker() {
	CloseMainMenu();
	delete music_player_;
	music_player_ = 0;
	delete env_map_;
	env_map_ = 0;
}



void DownwashTicker::Suspend(bool hard) {
	Parent::Suspend(hard);

	if (music_player_) {
		music_player_->Pause();
	}
}

void DownwashTicker::Resume(bool hard) {
	Parent::Resume(hard);

	double rtr_offset;
	v_get(rtr_offset, =, slave_array_[0]->GetVariableScope(), kRtvarPhysicsRtrOffset, 0.0);
	v_set(slave_array_[0]->GetVariableScope(), kRtvarPhysicsRtr, 1.0+rtr_offset);

	if (music_player_) {
		music_player_->Stop();
		music_player_->Playback();
	}
}

bool DownwashTicker::CreateSlave() {
	return (Parent::CreateSlave(&DownwashTicker::CreateSlaveManager));
}

void DownwashTicker::OnSlavesKilled() {
	/*DeleteServer();
	//deb_assert(!is_player_count_view_active_);
	is_player_count_view_active_ = true;
	slave_top_split_ = 1.0f;
	Parent::CreateSlave(&DownwashTicker::CreateViewer);*/
	CreateSlave();
}

void DownwashTicker::OnServerCreated(life::UiGameServerManager*) {
}



bool DownwashTicker::OpenUiManager() {
	bool ok = true;
	if (ok) {
		ok = ui_manager_->OpenDraw();
	}
	if (ok) {
		uilepra::Core::ProcessMessages();
		//ui_manager_->GetPainter()->ResetClippingRect();
		//ui_manager_->GetPainter()->Clear(BLACK);
		//DisplaySplashLogo();
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
		music_player_->AddSong("dub feral.ogg");
		music_player_->AddSong("easy jam.ogg");
		music_player_->AddSong("firmament.ogg");
		music_player_->AddSong("mandeville.ogg");
		music_player_->AddSong("slow ska game loop.ogg");
		music_player_->AddSong("stealth groover.ogg");
		music_player_->AddSong("yallahs.ogg");
		music_player_->Shuffle();
		ok = music_player_->Playback();
	}
	if (ok) {
		ui_manager_->GetDesktopWindow()->CreateLayer(new uitbc::FloatingLayout());
	}
	return ok;
}

void DownwashTicker::BeginRender(vec3& color) {
	Parent::BeginRender(color);
	ui_manager_->GetRenderer()->SetOutlineFillColor(OFF_BLACK);
}

void DownwashTicker::PreWaitPhysicsTick() {
	Parent::PreWaitPhysicsTick();
	if (music_player_) {
		music_player_->Update();
	}
}



void DownwashTicker::CloseMainMenu() {
	if (is_player_count_view_active_) {
		DeleteSlave(slave_array_[0], false);
		is_player_count_view_active_ = false;
	}
}

bool DownwashTicker::QueryQuit() {
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



life::GameClientSlaveManager* DownwashTicker::CreateSlaveManager(life::GameClientMasterTicker* pMaster,
	cure::TimeManager* time, cure::RuntimeVariableScope* variable_scope,
	cure::ResourceManager* resource_manager, UiCure::GameUiManager* ui_manager,
	int slave_index, const PixelRect& render_area) {
	return new DownwashManager(pMaster, time, variable_scope, resource_manager, ui_manager, slave_index, render_area);
}

/*life::GameClientSlaveManager* DownwashTicker::CreateViewer(life::GameClientMasterTicker* pMaster,
	cure::TimeManager* time, cure::RuntimeVariableScope* variable_scope,
	cure::ResourceManager* resource_manager, UiCure::GameUiManager* ui_manager,
	int slave_index, const PixelRect& render_area) {
	return new DownwashViewer(pMaster, time, variable_scope, resource_manager, ui_manager, slave_index, render_area);
}*/



loginstance(kGame, DownwashTicker);



}
