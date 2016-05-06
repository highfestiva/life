
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../lepra/include/lepratarget.h"
#include "pushticker.h"
#include "../lepra/include/systemmanager.h"
#include "../life/lifeclient/uigameservermanager.h"
#include "../life/lifeserver/masterserverconnection.h"
#include "../life/lifeserver/servermessageprocessor.h"
#include "../uicure/include/uigameuimanager.h"
#include "../uicure/include/uiparticleloader.h"
#include "../uilepra/include/uicore.h"
#include "../uitbc/include/gui/uidesktopwindow.h"
#include "../uitbc/include/gui/uifloatinglayout.h"
#include "../uitbc/include/uiparticlerenderer.h"
#include "pushserver/pushserverdelegate.h"
#include "rtvar.h"
#include "pushdemo.h"
#include "pushviewer.h"



namespace Push {



PushTicker::PushTicker(UiCure::GameUiManager* ui_manager, cure::ResourceManager* resource_manager, float physics_radius, int physics_levels, float physics_sensitivity):
	Parent(ui_manager, resource_manager, physics_radius, physics_levels, physics_sensitivity),
	is_player_count_view_active_(false),
	demo_time_(0),
	sunlight_(0) {
	v_override(UiCure::GetSettings(), kRtvarGameTimeofdayfactor, 1.0);
	v_override(UiCure::GetSettings(), kRtvarUi3DEnablemassobjectfading, false);
}

PushTicker::~PushTicker() {
	CloseMainMenu();

	delete sunlight_;
	sunlight_ = 0;
}



Sunlight* PushTicker::GetSunlight() const {
	return sunlight_;
}



bool PushTicker::CreateSlave() {
	return (Parent::CreateSlave(&PushTicker::CreateSlaveManager));
}

void PushTicker::OnSlavesKilled() {
	DeleteServer();
	//deb_assert(!is_player_count_view_active_);
	is_player_count_view_active_ = true;
	slave_top_split_ = 1.0f;
	Parent::CreateSlave(&PushTicker::CreateViewer);
}

void PushTicker::OnServerCreated(life::UiGameServerManager* server) {
	PushServerDelegate* delegate = new PushServerDelegate(server);
	server->SetDelegate(delegate);
	server->SetMessageProcessor(new life::ServerMessageProcessor(server));
}



bool PushTicker::Reinitialize() {
	delete sunlight_;
	sunlight_ = 0;
	bool ok = Parent::Reinitialize();
	sunlight_ = new Sunlight(ui_manager_);
	return ok;
}

bool PushTicker::OpenUiManager() {
	bool ok = ui_manager_->OpenDraw();
	if (ok) {
		uilepra::Core::ProcessMessages();
		ui_manager_->GetPainter()->ResetClippingRect();
		ui_manager_->GetPainter()->Clear(BLACK);
		DisplaySplashLogo();
	}
	if (ok) {
		ui_manager_->UpdateSettings();
		uitbc::Renderer* renderer = ui_manager_->GetRenderer();
		renderer->AddDynamicRenderer("particle", new uitbc::ParticleRenderer(renderer, 1));
		UiCure::ParticleLoader loader(resource_manager_, renderer, "explosion.png", 4, 5);
	}
	if (ok) {
		UiCure::RendererImageResource* env_map = new UiCure::RendererImageResource(ui_manager_, resource_manager_, "env.png", UiCure::ImageProcessSettings(Canvas::kResizeFast, true));
		if (env_map->Load()) {
			if (env_map->PostProcess() == cure::kResourceLoadComplete) {
				uitbc::Renderer::TextureID texture_id = env_map->GetUserData(0);
				ui_manager_->GetRenderer()->SetEnvironmentMap(texture_id);
			}
		}
	}
	if (ok) {
		ok = ui_manager_->OpenRest();
	}
	if (ok) {
		ui_manager_->GetDesktopWindow()->CreateLayer(new uitbc::FloatingLayout());
		DisplayCompanyLogo();
	}
	return ok;
}

void PushTicker::DisplaySplashLogo() {
	UiCure::PainterImageResource* logo = new UiCure::PainterImageResource(ui_manager_, resource_manager_, "logo.png", UiCure::PainterImageResource::kReleaseFreeBuffer);
	if (logo->Load()) {
		if (logo->PostProcess() == cure::kResourceLoadComplete) {
			//ui_manager_->BeginRender(vec3(0, 1, 0));
			ui_manager_->PreparePaint(true);
			const Canvas* canvas = ui_manager_->GetCanvas();
			const Canvas* image = logo->GetRamData();
			ui_manager_->GetPainter()->DrawImage(logo->GetUserData(0), canvas->GetWidth()/2 - image->GetWidth()/2, canvas->GetHeight()/2 - image->GetHeight()/2);
			ui_manager_->GetDisplayManager()->UpdateScreen();
		}
	}
	delete logo;
}

void PushTicker::DisplayCompanyLogo() {
	bool show_logo;
	v_get(show_logo, =, UiCure::GetSettings(), kRtvarGameEnablestartlogo, true);
	if (show_logo) {
		cure::UserRamImageResource* logo = new cure::UserRamImageResource;
		cure::UserResourceOwner<cure::UserRamImageResource> logo_holder(logo, resource_manager_, "megaphone.png");
		UiCure::UserSound2dResource* logo_sound = new UiCure::UserSound2dResource(ui_manager_, uilepra::SoundManager::kLoopNone);
		cure::UserResourceOwner<UiCure::UserSound2dResource> logo_sound_holder(logo_sound, resource_manager_, "logo_trumpet.wav");
		for (int x = 0; x < 1000; ++x) {
			resource_manager_->Tick();
			if (logo->GetLoadState() != cure::kResourceLoadInProgress &&
				logo_sound->GetLoadState() != cure::kResourceLoadInProgress) {
				break;
			}
			Thread::Sleep(0.001);
		}
		if (logo->GetLoadState() == cure::kResourceLoadComplete &&
			logo_sound->GetLoadState() == cure::kResourceLoadComplete) {
			ui_manager_->GetSoundManager()->Play(logo_sound->GetData(), 1, 1);

			uilepra::Canvas& canvas = *logo->GetRamData();
			const uitbc::Painter::ImageID image_id = ui_manager_->GetDesktopWindow()->GetImageManager()->AddImage(canvas, uitbc::GUIImageManager::kStretched, uitbc::GUIImageManager::kNoBlend, 255);
			uitbc::RectComponent rect(image_id);
			ui_manager_->AssertDesktopLayout(new uitbc::FloatingLayout, 0);
			ui_manager_->GetDesktopWindow()->AddChild(&rect, 0, 0, 0);
			const unsigned width = ui_manager_->GetDisplayManager()->GetWidth();
			const unsigned height = ui_manager_->GetDisplayManager()->GetHeight();
			rect.SetPreferredSize(canvas.GetWidth(), canvas.GetHeight());
			const unsigned target_x = width/2 - canvas.GetWidth()/2;
			const unsigned target_y = height/2 - canvas.GetHeight()/2;
			ui_manager_->GetRenderer()->ResetClippingRect();
			Color _color;
			ui_manager_->GetRenderer()->SetClearColor(Color());
			ui_manager_->GetDisplayManager()->SetVSyncEnabled(true);

			const float min = 0;
			const float max = 26;
			const int step_count = 50;
			const float base_step = (max-min)/(float)step_count;
			float base = -max;
			int count = 0;
			const int total_frame_count = 600;
			for (count = 0; count <= total_frame_count && SystemManager::GetQuitRequest() == 0; ++count) {
				if (count < step_count || count > total_frame_count-step_count) {
					base += base_step;
				}
				int movement = (int)(::fabs(base)*base*3);
				rect.SetPos(target_x+movement, target_y);

				ui_manager_->GetRenderer()->Clear();
				ui_manager_->Paint(false);
				ui_manager_->GetDisplayManager()->UpdateScreen();

				Thread::Sleep(0.01);
				ui_manager_->InputTick();
				//ok = (SystemManager::GetQuitRequest() <= 0);

				if (count == step_count) {
					base = 0;
				} else if (count == total_frame_count-step_count) {
					base = min;
				}
			}
			ui_manager_->GetDesktopWindow()->RemoveChild(&rect, 0);
			ui_manager_->GetDesktopWindow()->GetImageManager()->RemoveImage(image_id);
		}
	}
	resource_manager_->ForceFreeCache();

}

void PushTicker::BeginRender(vec3& color) {
	float real_time_ratio;
	v_get(real_time_ratio, =(float), UiCure::GetSettings(), kRtvarPhysicsRtr, 1.0);
	float time_of_day_factor;
	v_get(time_of_day_factor, =(float), UiCure::GetSettings(), kRtvarGameTimeofdayfactor, 1.0);
	sunlight_->Tick(real_time_ratio * time_of_day_factor);

	sunlight_->AddSunColor(color, 2);
	Parent::BeginRender(color);

	vec3 _color(1.2f, 1.2f, 1.2f);
	sunlight_->AddSunColor(_color, 1);
	Color fill_color;
	fill_color.Set(_color.x, _color.y, _color.z, 1.0f);
	ui_manager_->GetRenderer()->SetOutlineFillColor(fill_color);
}



void PushTicker::CloseMainMenu() {
	if (is_player_count_view_active_) {
		DeleteSlave(slave_array_[0], false);
		is_player_count_view_active_ = false;
	}
}

bool PushTicker::QueryQuit() {
	if (demo_time_) {
		// We quit if user tried quitting twice or more, or demo time is over.
		return (SystemManager::GetQuitRequest() >= 2 || demo_time_->QueryTimeDiff() > 30.0f);
	}

	if (Parent::QueryQuit()) {
		PrepareQuit();
		for (int x = 0; x < 4; ++x) {
			DeleteSlave(slave_array_[x], false);
		}
		DeleteServer();
#ifdef PUSH_DEMO
		if (!ui_manager_->CanRender()) {
			return true;
		}
		CreateSlave(&PushTicker::CreateDemo);
		demo_time_ = new HiResTimer;
#else // !Demo
		return (true);
#endif // Demo / !Demo
	}
	return (false);
}



life::GameClientSlaveManager* PushTicker::CreateSlaveManager(life::GameClientMasterTicker* pMaster,
	cure::TimeManager* time, cure::RuntimeVariableScope* variable_scope,
	cure::ResourceManager* resource_manager, UiCure::GameUiManager* ui_manager,
	int slave_index, const PixelRect& render_area) {
	PushManager* game_manager = new PushManager(pMaster, time, variable_scope, resource_manager, ui_manager, slave_index, render_area);
	game_manager->SetMasterServerConnection(new life::MasterServerConnection(pMaster->GetMasterServerConnection()->GetMasterAddress()));
	return game_manager;
}

life::GameClientSlaveManager* PushTicker::CreateViewer(life::GameClientMasterTicker* pMaster,
	cure::TimeManager* time, cure::RuntimeVariableScope* variable_scope,
	cure::ResourceManager* resource_manager, UiCure::GameUiManager* ui_manager,
	int slave_index, const PixelRect& render_area) {
	return new PushViewer(pMaster, time, variable_scope, resource_manager, ui_manager, slave_index, render_area);
}

life::GameClientSlaveManager* PushTicker::CreateDemo(life::GameClientMasterTicker* pMaster,
	cure::TimeManager* time, cure::RuntimeVariableScope* variable_scope,
	cure::ResourceManager* resource_manager, UiCure::GameUiManager* ui_manager,
	int slave_index, const PixelRect& render_area) {
#ifdef PUSH_DEMO
	return new PushDemo(pMaster, time, variable_scope, resource_manager, ui_manager, slave_index, render_area);
#else // !Demo
	return new PushViewer(pMaster, time, variable_scope, resource_manager, ui_manager, slave_index, render_area);
#endif // Demo / !Demo
}



loginstance(kGame, PushTicker);



}
