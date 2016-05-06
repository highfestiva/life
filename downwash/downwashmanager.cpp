
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "downwashmanager.h"
#include <algorithm>
#include "../cure/include/contextmanager.h"
#include "../cure/include/contextpath.h"
#include "../cure/include/driver.h"
#include "../cure/include/elevator.h"
#include "../cure/include/health.h"
#include "../cure/include/hiscoreagent.h"
#include "../cure/include/floatattribute.h"
#include "../cure/include/intattribute.h"
#include "../cure/include/networkclient.h"
#include "../cure/include/spawner.h"
#include "../cure/include/timemanager.h"
#include "../lepra/include/math.h"
#include "../lepra/include/obfuxator.h"
#include "../lepra/include/random.h"
#include "../lepra/include/systemmanager.h"
#include "../lepra/include/time.h"
#include "../life/lifeclient/explodingmachine.h"
#include "../life/lifeclient/fastprojectile.h"
#include "../life/lifeclient/homingprojectile.h"
#include "../life/lifeclient/level.h"
#include "../life/lifeclient/massobject.h"
#include "../life/lifeclient/menu.h"
#include "../life/lifeclient/mine.h"
#include "../life/lifeclient/projectile.h"
#include "../life/lifeclient/uiconsole.h"
#include "../life/explosion.h"
#include "../life/projectileutil.h"
#include "../life/spawner.h"
#include "../tbc/include/physicstrigger.h"
#include "../uicure/include/uiburnemitter.h"
#include "../uicure/include/uicollisionsoundmanager.h"
#include "../uicure/include/uidebugrenderer.h"
#include "../uicure/include/uigameuimanager.h"
#include "../uicure/include/uiiconbutton.h"
#include "../uicure/include/uijetengineemitter.h"
#include "../uicure/include/uigravelemitter.h"
#include "../uicure/include/uisoundreleaser.h"
#include "../uilepra/include/uitouchstick.h"
#include "../uitbc/include/gui/uicheckbutton.h"
#include "../uitbc/include/gui/uidesktopwindow.h"
#include "../uitbc/include/gui/uifixedlayouter.h"
#include "../uitbc/include/gui/uiradiobutton.h"
#include "../uitbc/include/gui/uitextarea.h"
#include "../uitbc/include/gui/uitextfield.h"
#include "../uitbc/include/uibillboardgeometry.h"
#include "../uitbc/include/uiparticlerenderer.h"
#include "../uitbc/include/uirenderer.h"
#include "airballoonpilot.h"
#include "automan.h"
#include "autopathdriver.h"
#include "autopilot.h"
#include "centeredmachine.h"
#include "canondriver.h"
#include "downwash.h"
#include "downwashconsolemanager.h"
#include "downwashticker.h"
#include "landingtrigger.h"
#include "level.h"
#include "rtvar.h"
#include "eater.h"
#include "simulatordriver.h"
#include "sunlight.h"
#include "version.h"

#define kStillFramesUntilCamPans	4
#define kBgColor			Color(110, 110, 110, 160)



namespace Downwash {



const int ORDERED_LEVELNO[]	= { 6, 7, 8, 9, 5, 10, 0, 4, 11, 2, 1, 3, 12, 13 };
const int REVERSED_LEVELNO[]	= { 6, 10, 9, 11, 7, 4, 0, 1, 2, 3, 5, 8, 12, 13 };

const str kPlatform = "any_system";
const str kVehicleName = "helicopter_01";
const str kDefaultPilotName = "Anonymous pilot";



DownwashManager::DownwashManager(life::GameClientMasterTicker* pMaster, const cure::TimeManager* time,
	cure::RuntimeVariableScope* variable_scope, cure::ResourceManager* resource_manager,
	UiCure::GameUiManager* ui_manager, int slave_index, const PixelRect& render_area):
	Parent(pMaster, time, variable_scope, resource_manager, ui_manager, slave_index, render_area),
	collision_sound_manager_(0),
	menu_(0),
	avatar_id_(0),
	set_random_chopper_color_(false),
	had_avatar_(false),
	update_camera_for_avatar_(false),
	active_weapon_(0),
	level_(0),
	old_level_(0),
	autopilot_(0),
	hemisphere_(0),
	hemisphere_uv_transform_(0),
	render_hemisphere_(true),
	sunlight_(0),
	camera_transform_(quat(), vec3(0, -200, 40)),
	helicopter_position_(0, 0, 30),
	camera_speed_(0),
	zoom_platform_(false),
	post_zoom_platform_frame_count_(100),
	hit_ground_frame_count_(kStillFramesUntilCamPans),
	is_hit_this_frame_(false),
	level_completed_(false),
	pause_button_(0),
	last_hiscore_button_(0),
	stick_(0),
	wrong_direction_image_(0),
	win_image_(0),
	check_icon_(0),
	lock_icon_(0),
	arrow_(0),
	arrow_billboard_(0),
	arrow_billboard_id_(0),
	arrow_total_power_(0),
	arrow_angle_(0),
	slow_system_counter_(0),
	hiscore_level_index_(-1),
	my_hiscore_index_(-1),
	hiscore_agent_(0) {
	collision_sound_manager_ = new UiCure::CollisionSoundManager(this, ui_manager);
	collision_sound_manager_->AddSound("explosion",	UiCure::CollisionSoundManager::SoundResourceInfo(0.8f, 0.4f, 0));
	collision_sound_manager_->AddSound("small_metal",	UiCure::CollisionSoundManager::SoundResourceInfo(0.2f, 0.4f, 0));
	collision_sound_manager_->AddSound("big_metal",	UiCure::CollisionSoundManager::SoundResourceInfo(1.5f, 0.4f, 0));
	collision_sound_manager_->AddSound("plastic",	UiCure::CollisionSoundManager::SoundResourceInfo(1.0f, 0.4f, 0));
	collision_sound_manager_->AddSound("rubber",	UiCure::CollisionSoundManager::SoundResourceInfo(0.5f, 0.3f, 0));
	collision_sound_manager_->AddSound("wood",	UiCure::CollisionSoundManager::SoundResourceInfo(0.5f, 0.5f, 0));
	collision_sound_manager_->AddSound("thump",	UiCure::CollisionSoundManager::SoundResourceInfo(5.0f, 0.5f, 1.0f));
	collision_sound_manager_->AddSound("gem",		UiCure::CollisionSoundManager::SoundResourceInfo(0.1f, 0.2f, 0.1f));
	collision_sound_manager_->PreLoadSound("explosion");

	SetConsoleManager(new DownwashConsoleManager(GetResourceManager(), this, ui_manager_, GetVariableScope(), render_area_));

	GetPhysicsManager()->SetSimulationParameters(0.0f, 0.03f, 0.2f);

	touchstick_timer_.ReduceTimeDiff(-5);

	v_set(GetVariableScope(), kRtvarGameStartLevel, "level_06");
	v_set(GetVariableScope(), kRtvarGameLevelcount, 14);
	v_set(GetVariableScope(), kRtvarGameChildishness, 1.0);
	v_set(GetVariableScope(), kRtvarGameAllowtoymode, false);
	v_set(GetVariableScope(), kRtvarGamePilotname, kDefaultPilotName);
	v_set(GetVariableScope(), kRtvarUiSoundMastervolume, 1.0);
	v_set(GetVariableScope(), kRtvarPhysicsRtrOffset, 0.0);
	v_set(GetVariableScope(), kRtvarUi3DEnablemassobjects, true);
}

DownwashManager::~DownwashManager() {
	Close();

	//delete autopilot_;	NOTE: owned by context manager.
	autopilot_ = 0;
	delete stick_;
	stick_ = 0;
	delete collision_sound_manager_;
	collision_sound_manager_ = 0;
	delete hemisphere_uv_transform_;
	hemisphere_uv_transform_ = 0;
	delete wrong_direction_image_;
	wrong_direction_image_ = 0;
	delete arrow_;
	arrow_ = 0;
	delete check_icon_;
	check_icon_ = 0;
	delete lock_icon_;
	lock_icon_ = 0;
	delete arrow_billboard_;
	arrow_billboard_ = 0;

}

void DownwashManager::Suspend(bool hard) {
	if (!menu_->GetDialog()) {
		pause_button_->SetVisible(false);
		OnPauseButton(0);
	}
}

void DownwashManager::LoadSettings() {
	v_set(GetVariableScope(), kRtvarGameSpawnpart, 1.0);

	Parent::LoadSettings();

	v_set(GetVariableScope(), kRtvarUi2DFont, "Verdana");
	v_set(GetVariableScope(), kRtvarUi2DFontflags, 0);
	v_set(GetVariableScope(), kRtvarUi3DFov, 30.0);

	UpdateCameraDistance();
	v_set(GetVariableScope(), kRtvarUi3DCamxoffset, 0.0);
	v_set(GetVariableScope(), kRtvarUi3DCamyoffset, 0.0);
	v_set(GetVariableScope(), kRtvarUi3DCamzoffset, 0.0);
	v_set(GetVariableScope(), kRtvarUi3DCamxangle, 0.0);

	v_set(GetVariableScope(), kRtvarUi3DEnableclear, false);
	v_set(GetVariableScope(), kRtvarCtrlSteerLeft3D, "Key.Left");
	v_set(GetVariableScope(), kRtvarCtrlSteerRight3D, "Key.Right");
	v_set(GetVariableScope(), kRtvarCtrlSteerUp3D, "Key.Up");
	v_set(GetVariableScope(), kRtvarCtrlSteerDown3D, "Key.Down");
	v_set(GetVariableScope(), kRtvarPhysicsNoclip, false);

	GetConsoleManager()->ExecuteCommand("bind-key F1 \"#Debug.Enable true; #Ui.3D.CamDistance 100.0\"");
	GetConsoleManager()->ExecuteCommand("bind-key F2 \"#Game.Childishness 1.0\"");
	GetConsoleManager()->ExecuteCommand("bind-key F3 \"#Game.Childishness 0.5\"");
	GetConsoleManager()->ExecuteCommand("bind-key F4 \"#Game.Childishness 0.0\"");
	GetConsoleManager()->ExecuteCommand("bind-key F6 prev-level");
	GetConsoleManager()->ExecuteCommand("bind-key F7 next-level");
	GetConsoleManager()->ExecuteCommand("bind-key F8 \"#Physics.NoClip true\"");
	GetConsoleManager()->ExecuteCommand("bind-key F9 \"#Physics.NoClip false\"");

#if defined(LEPRA_TOUCH) || defined(EMULATE_TOUCH)
	const str schtick_name = "Touchstick";
	v_override(GetVariableScope(), kRtvarCtrlSteerUp3D, schtick_name+".AxisY-");
	v_override(GetVariableScope(), kRtvarCtrlSteerDown3D, schtick_name+".AxisY+");
	v_override(GetVariableScope(), kRtvarCtrlSteerLeft3D, schtick_name+".AxisX-");
	v_override(GetVariableScope(), kRtvarCtrlSteerRight3D, schtick_name+".AxisX+");
#endif // touch device or emulated touch device
}

void DownwashManager::SaveSettings() {
#ifndef EMULATE_TOUCH
	GetConsoleManager()->ExecuteCommand("save-application-config-file "+GetApplicationCommandFilename());
#endif // Computer or touch device.
}

void DownwashManager::SetRenderArea(const PixelRect& render_area) {
	Parent::SetRenderArea(render_area);
	UpdateTouchstickPlacement();
}

bool DownwashManager::Open() {
	bool _ok = Parent::Open();
	if (_ok) {
		pause_button_ = ICONBTNA("btn_pause.png", L"");
		int x = render_area_.GetCenterX() - 32;
		int y = render_area_.bottom_ - 76;
		ui_manager_->GetDesktopWindow()->AddChild(pause_button_, x, y);
		pause_button_->SetVisible(true);
		pause_button_->SetOnClick(DownwashManager, OnPauseButton);
	}
	if (_ok) {
		last_hiscore_button_ = ICONBTNA("btn_hiscore.png", L"");
		int x = 12;
		int y = render_area_.bottom_ - 76;
		ui_manager_->GetDesktopWindow()->AddChild(last_hiscore_button_, x, y);
		last_hiscore_button_->SetVisible(false);
		last_hiscore_button_->SetOnClick(DownwashManager, OnLastHiscoreButton);
	}
	if (_ok) {
		menu_ = new life::Menu(ui_manager_, GetResourceManager());
		menu_->SetButtonTapSound("tap.wav", 1, 0.3f);
	}
	if (_ok) {
		wrong_direction_image_ = new UiCure::UserPainterKeepImageResource(ui_manager_, UiCure::PainterImageResource::kReleaseFreeBuffer);
		wrong_direction_image_->Load(GetResourceManager(), "direction.png",
			UiCure::UserPainterKeepImageResource::TypeLoadCallback(this, &DownwashManager::PainterImageLoadCallback));

		check_icon_ = new UiCure::UserPainterKeepImageResource(ui_manager_, UiCure::PainterImageResource::kReleaseFreeBuffer);
		check_icon_->Load(GetResourceManager(), "icon_check.png",
			UiCure::UserPainterKeepImageResource::TypeLoadCallback(this, &DownwashManager::PainterImageLoadCallback));

		lock_icon_ = new UiCure::UserPainterKeepImageResource(ui_manager_, UiCure::PainterImageResource::kReleaseFreeBuffer);
		lock_icon_->Load(GetResourceManager(), "icon_lock.png",
			UiCure::UserPainterKeepImageResource::TypeLoadCallback(this, &DownwashManager::PainterImageLoadCallback));

		arrow_ = new UiCure::UserRendererImageResource(ui_manager_, UiCure::ImageProcessSettings(Canvas::kResizeFast, false));
		arrow_->Load(GetResourceManager(), "arrow.png",
			UiCure::UserRendererImageResource::TypeLoadCallback(this, &DownwashManager::RendererTextureLoadCallback));
		arrow_billboard_ = new uitbc::BillboardGeometry(1/4.0f, 1);
		arrow_billboard_id_ = ui_manager_->GetRenderer()->AddGeometry(arrow_billboard_, uitbc::Renderer::kMatNull, uitbc::Renderer::kForceNoShadows);
	}
	if (_ok) {
		str start_level;
		v_get(start_level, =, GetVariableScope(), kRtvarGameStartLevel, "level_06");
		if (start_level == "level_06") {
			pause_button_->SetVisible(false);
			OnPauseButton(0);
		}
	}
	return _ok;
}

void DownwashManager::Close() {
	ScopeLock lock(GetTickLock());
	delete pause_button_;
	pause_button_ = 0;
	delete last_hiscore_button_;
	last_hiscore_button_ = 0;
	delete menu_;
	menu_ = 0;
	if (sunlight_) {
		delete sunlight_;
		sunlight_ = 0;
	}
	Parent::Close();
}

void DownwashManager::SetIsQuitting() {
	((DownwashConsoleManager*)GetConsoleManager())->GetUiConsole()->SetVisible(false);
	Parent::SetIsQuitting();
}

void DownwashManager::SetFade(float fade_amount) {
	(void)fade_amount;
}



bool DownwashManager::Render() {
	if (!hemisphere_ || !hemisphere_->IsLoaded() || !level_ || !level_->IsLoaded()) {
		if (!hemisphere_ || !hemisphere_->IsLoaded() || !old_level_ || !old_level_->IsLoaded()) {
			ui_manager_->GetRenderer()->Clear(uitbc::Renderer::kClearColorbuffer);
		}
		return true;
	}

	{
		double rtr_offset;
		v_get(rtr_offset, =, GetVariableScope(), kRtvarPhysicsRtrOffset, 0.0);
		const bool toy_mode = rtr_offset > 0.5;
		const double r = toy_mode? sin(toy_mode_color_timer_.QueryTimeDiff()*1.0) : 0.5;
		const double g = toy_mode? sin(toy_mode_color_timer_.QueryTimeDiff()*0.7) : 0.5;
		const double b = toy_mode? sin(toy_mode_color_timer_.QueryTimeDiff()*0.3) : 0.5;
		const bool enable_texturing = toy_mode? false : true;
		v_set(GetVariableScope(), kRtvarUi3DAmbientred, r);
		v_set(GetVariableScope(), kRtvarUi3DAmbientgreen, g);
		v_set(GetVariableScope(), kRtvarUi3DAmbientblue, b);
		v_set(GetVariableScope(), kRtvarUi3DEnabletexturing, enable_texturing);
	}

	hemisphere_->GetMesh(0)->SetAlwaysVisible(false);
	if (render_hemisphere_) {
		if (!hemisphere_->GetMesh(0)->GetUVAnimator()) {
			hemisphere_->GetMesh(0)->SetUVAnimator(hemisphere_uv_transform_);
			hemisphere_->GetMesh(0)->SetPreRenderCallback(tbc::GeometryBase::PreRenderCallback(this, &DownwashManager::DisableDepth));
			hemisphere_->GetMesh(0)->SetPostRenderCallback(tbc::GeometryBase::PostRenderCallback(this, &DownwashManager::EnableDepth));
		}
		vec3 _position = camera_transform_.GetPosition();
		_position.x = -_position.x;
		_position.y = -95 + _position.z * -0.09f;
		vec3 _angle = camera_transform_.GetOrientation()*vec3(0,250,0);
		_position.x -= _angle.x;
		_position.y -= _angle.z;
		hemisphere_uv_transform_->GetBones()[0].GetRelativeBoneTransformation(0).GetPosition() = _position * 0.003f;

		//xform t = hemisphere_->GetMesh(0)->GetTransformation();
		//t.orientation_ = quat(0.707106769f, 0.707106769f, 0, 0);
		//t.orientation_.RotateAroundOwnX((float)v_slowtryget(GetVariableScope(), "hemi_ang", 0.0));
		//hemisphere_->GetMesh(0)->SetTransformation(t);

		ui_manager_->GetRenderer()->RenderRelative(hemisphere_->GetMesh(0), 0);
	}

	bool _ok = Parent::Render();

	double rtr_offset;
	v_get(rtr_offset, =, GetVariableScope(), kRtvarPhysicsRtrOffset, 0.0);
	const bool is_toy_mode = (rtr_offset >= 0.5);
	if (!_ok || is_toy_mode) {
		return _ok;
	}

	const UiCure::CppContextObject* _object = (const UiCure::CppContextObject*)GetContext()->GetObject(avatar_id_);
	if (!_object || post_zoom_platform_frame_count_ < 10) {
		return true;
	}
	if (arrow_->GetLoadState() != cure::kResourceLoadComplete || _object->GetPhysics()->GetEngineCount() < 3) {
		return true;
	}
	const life::options::Steering& s = options_.GetSteeringControl();
#define S(dir) s.control_[life::options::Steering::kControl##dir]
	const float wanted_direction = S(Right3D) - S(Left3D);
	const float _power = S(Up3D) - S(Down3D);
	xform transform = GetMainRotorTransform(_object);
	float total_power = ::sqrt(wanted_direction*wanted_direction + _power*_power);
	if (!total_power) {
		return true;
	}
	total_power = Math::Lerp(0.4f, 1.0f, total_power);
	total_power *= Math::Lerp(0.7f, 1.0f, _power);
	arrow_total_power_ = Math::Lerp(arrow_total_power_, total_power, 0.3f);
	arrow_angle_ = Math::Lerp(arrow_angle_, std::atan2(wanted_direction, _power), 0.3f);
	float __size = arrow_total_power_*0.5f;
	float fo_v;
	v_get(fo_v, =(float), GetVariableScope(), kRtvarUi3DFov, 45.0);
	__size *= transform.GetPosition().GetDistance(camera_transform_.GetPosition()) * fo_v / 2400;
	vec3 _position = transform.GetPosition();
	_position.x += ::sin(arrow_angle_) * __size * 3.4f;
	_position.z += ::cos(arrow_angle_) * __size * 3.4f;
	_position += (camera_previous_position_-_position).GetNormalized()*4;

	uitbc::BillboardRenderInfoArray billboards;
	billboards.push_back(uitbc::BillboardRenderInfo(arrow_angle_, _position, __size, vec3(1, 1, 1), 1, 0));
	ui_manager_->GetRenderer()->RenderBillboards(arrow_billboard_, true, false, billboards);

	return true;
}

bool DownwashManager::Paint() {
	{
		int x = render_area_.GetCenterX() - 32;
		int y = render_area_.bottom_ - 76;
		pause_button_->SetPos(x, y);
		x = 12;
		last_hiscore_button_->SetPos(x, y);
		last_hiscore_button_->SetVisible(hiscore_just_uploaded_timer_.IsStarted());
		if (hiscore_just_uploaded_timer_.QuerySplitTime() > 10) {
			hiscore_just_uploaded_timer_.Stop();
		}
	}

	if (!Parent::Paint()) {
		return false;
	}

	if (stick_ && menu_->GetDialog() == 0) {
		DrawStick(stick_);
		stick_->ResetTap();
	}

	const cure::ContextObject* _avatar = GetContext()->GetObject(avatar_id_);
	if (_avatar) {
		ui_manager_->GetPainter()->SetAlphaValue(160);

		// Draw health bar first.
		ui_manager_->GetPainter()->SetLineWidth(2);
		const int w = ui_manager_->GetDisplayManager()->GetWidth();
		const int m = w / 2;
		const float width = (float)(((int)(w * 134.0f/480.0f)) & ~1);
		float __min = m-width/2;
		__min = (__min < 193)? 193 : __min;
		const float health = cure::Health::Get(_avatar);
		const uint8 r = (int8)Math::Clamp((int)((1-health)*1.9f*255), 0, 255);
		const uint8 g = (int8)Math::Clamp((int)((health-0.3f)*3*255), 0, 255);
		ui_manager_->GetPainter()->SetColor(Color(r, g, 0), 0);
		const float remaining = Math::Clamp(health*width, 0.0f, width);
		std::vector<vec2> coords;
		coords.push_back(vec2(__min, 16+0.4f));
		coords.push_back(vec2(__min, 24+0.6f));
		coords.push_back(vec2(__min+remaining, 24+0.6f));
		coords.push_back(vec2(__min+remaining, 16+0.4f));
		coords.push_back(coords[0]);
		ui_manager_->GetPainter()->DrawFan(coords, false);
		// Draw surrounding frame after.
		ui_manager_->GetPainter()->SetColor(Color(10, 30, 40), 0);
		const int padding = 4;
		coords.clear();
		coords.push_back(vec2(__min-padding, 16-padding+0.4f));
		coords.push_back(vec2(__min-padding, 24+padding+0.6f));
		coords.push_back(vec2(__min+width+padding, 24+padding+0.6f));
		coords.push_back(vec2(__min+width+padding, 16-padding+0.4f));
		coords.push_back(coords[0]);
		ui_manager_->GetPainter()->DrawFan(coords, false);
		ui_manager_->GetPainter()->SetLineWidth(1);

		// TRICKY: this needs to be run every loop, or time is not updated. Especially important
		//         when user is able to toggle between toy mode and normal mode.
		const double _time = fly_time_.QuerySplitTime();

		double rtr_offset;
		v_get(rtr_offset, =, GetVariableScope(), kRtvarPhysicsRtrOffset, 0.0);
		if (rtr_offset > 0.1) {
			uint8 r = uint8(sin(toy_mode_color_timer_.QueryTimeDiff()*3)*120+128);
			uint8 g = uint8(sin(toy_mode_color_timer_.QueryTimeDiff()*5)*120+128);
			uint8 b = uint8(sin(toy_mode_color_timer_.QueryTimeDiff()*7)*120+128);
			ui_manager_->GetPainter()->SetColor(Color(10, 10, 10, 128));
			ui_manager_->GetPainter()->PrintText(L"Toy mode", 11, 5);
			ui_manager_->GetPainter()->SetColor(Color(r, g, b, 255));
			ui_manager_->GetPainter()->PrintText(L"Toy mode", 10, 4);
		} else {
			const bool is_flying = fly_time_.IsStarted();
			const bool is_sloppy = (is_flying || !_time);
			PrintTime(L"", _time, is_sloppy, 100, 3, Color(192, 210, 220), Color(10, 10, 10, 128));

			if (GetControlMode() != 2) {
				double level_best_time = GetCurrentLevelBestTime(false);
				if (level_best_time > 0) {
					PrintTime(L"PR: ", level_best_time, is_sloppy, 100, 43, Color(10, 30, 40), Color(192, 192, 192, 128));
				}
				level_best_time = GetCurrentLevelBestTime(true);
				if (level_best_time > 0) {
					PrintTime(L"WR: ", level_best_time, is_sloppy, 100, 83, Color(210, 40, 40, 255), Color(40, 10, 10, 128));
				}
			}
		}

		ui_manager_->GetPainter()->SetAlphaValue(255);

		if (_avatar->GetPhysics()->GetEngineCount() >= 3 && wrong_direction_image_->GetLoadState() == cure::kResourceLoadComplete &&
			level_ && level_->IsLoaded()) {
			const double image_time = direction_image_timer_.QueryTimeDiff();
			if (!direction_image_timer_.IsStarted() && image_time > 2.0) {
				autopilot_->AttemptCloserPathDistance();
			}
			if (image_time >= 0.5) {
				if (direction_image_timer_.IsStarted()) {
					direction_image_timer_.PopTimeDiff();
					direction_image_timer_.Stop();
				} else {
					direction_image_timer_.Start();
				}
			}
			const float path_distance = autopilot_->GetClosestPathDistance();
			//const bool lForceShowDirection = (GetCurrentLevelNumber() == 0 && all_loaded_timer_.QueryTimeDiff() < 3.0);
			if (path_distance > 40.0f) {
				if (direction_image_timer_.IsStarted()) {
					const vec3 pos3d = camera_transform_.GetPosition();
					const vec2 pos(pos3d.x, pos3d.z);
					const vec3 goal3d = last_landing_trigger_position_;
					const vec2 goal(goal3d.x, goal3d.z);
					const float a = (goal-pos).GetAngle() - PIF/2;
					const float wanted_size = ui_manager_->GetCanvas()->GetWidth() * 0.1f;
					const float __size = wanted_size;
					/*const float __size = (float)wrong_direction_image_->GetRamData()->GetWidth();
					while (wanted_size >= __size*2) __size *= 2;
					while (wanted_size <= __size/2) __size /= 2;*/
					// Find out the screen coordinate of the chopper, so we can place our arrow around that.
					float fo_v;
					v_get(fo_v, =(float), GetVariableScope(), kRtvarUi3DFov, 45.0);
					fo_v /= 45.0f;
					const float _distance = ui_manager_->GetCanvas()->GetWidth() / 6.0f;
					const float x = ui_manager_->GetCanvas()->GetWidth() /2.0f - 2*_distance*::sin(a);
					const float y = ui_manager_->GetCanvas()->GetHeight()/2.0f - _distance*::cos(a);
					DrawImage(wrong_direction_image_->GetData(), x, y, __size, __size, a);
				}
			} else {
				direction_image_timer_.Start();
			}
		}
	}
	if (win_image_timer_.IsStarted()) {
		if (!win_image_) {
			win_image_ = new UiCure::UserPainterKeepImageResource(ui_manager_, UiCure::PainterImageResource::kReleaseFreeBuffer);
			const str name = strutil::Format("win%i.png", Random::GetRandomNumber()%9);
			//log_.Infof("Showing win image %s.", name.c_str());
			win_image_->LoadUnique(GetResourceManager(), name,
				UiCure::UserPainterKeepImageResource::TypeLoadCallback(this, &DownwashManager::PainterImageLoadCallback));
		}
		if (win_image_->GetLoadState() == cure::kResourceLoadInProgress) {
			win_image_timer_.PopTimeDiff();	// Reset timer as long as we're not loaded.
		} else if (win_image_->GetLoadState() == cure::kResourceLoadComplete) {
			const float sf = ::sin((float)win_image_timer_.QueryTimeDiff()*PIF/2.7f);
			const float f = std::min(1.0f, sf*1.3f);
			if (f < 0) {
				win_image_timer_.Stop();
				ui_manager_->GetDesktopWindow()->GetImageManager()->RemoveImage(win_image_->GetData());
				delete win_image_;
				win_image_ = 0;
			} else {
				float x = 12+64;
				float s = Math::Clamp(ui_manager_->GetCanvas()->GetWidth() / 4.0f, 128.0f, 256.0f);
				float y = f*s;
				y = render_area_.GetHeight()-y;
				x += s*0.5f;
				y += s*0.5f;
				DrawImage(win_image_->GetData(), x, y, s, s, 0);
			}
		} else {
			deb_assert(false);
			delete win_image_;
			win_image_ = 0;
		}
	}

	return true;
}

void DownwashManager::PrintTime(const wstr prefix, double time, bool is_sloppy, int x, int y, const Color c, const Color bg) {
	const int sec = (int)time;
	const wstr int_time_string = prefix + wstrutil::Format(L"%i", sec);
	const wstr time_string = prefix + wstrutil::Format(is_sloppy? L"%.1f s" : L"%.3f s", time);
	int w = ui_manager_->GetPainter()->GetStringWidth(int_time_string);
	x -= w;
	x = (x < 4)? 4 : x;
	ui_manager_->GetPainter()->SetColor(bg, 0);
	ui_manager_->GetPainter()->PrintText(time_string, x+1, y+1);
	ui_manager_->GetPainter()->SetColor(c, 0);
	ui_manager_->GetPainter()->PrintText(time_string, x, y);
}

void DownwashManager::DrawSyncDebugInfo() {
	Parent::DrawSyncDebugInfo();

	if (GetLevel() && GetLevel()->QueryPath()->GetPath(0)) {
		UiCure::DebugRenderer debug_renderer(GetVariableScope(), ui_manager_, GetContext(), 0, GetTickLock());
		for (int x = 0; x < 20; ++x) {
			cure::ContextPath::SplinePath* path = GetLevel()->QueryPath()->GetPath(x);
			if (!path) {
				break;
			}
			debug_renderer.RenderSpline(ui_manager_, path);
		}
	}
}



bool DownwashManager::IsObjectRelevant(const vec3& position, float distance) const {
	return (position.GetDistanceSquared(camera_transform_.GetPosition()) <= distance*distance);
}

cure::GameObjectId DownwashManager::GetAvatarInstanceId() const {
	return avatar_id_;
}



bool DownwashManager::SetAvatarEnginePower(unsigned aspect, float power) {
	deb_assert(aspect >= 0 && aspect < tbc::PhysicsEngine::kAspectCount);
	cure::ContextObject* _object = GetContext()->GetObject(avatar_id_);
	if (_object && !zoom_platform_) {
		return SetAvatarEnginePower(_object, aspect, power);
	}
	return SetAvatarEnginePower(_object, aspect, 0.0f);
}



void DownwashManager::Shoot(cure::ContextObject* canon, int ammo) {
	life::FastProjectile* projectile = 0;
	switch (ammo) {
		default: {
			projectile = new life::FastProjectile(GetResourceManager(), "bullet", ui_manager_, this);
		} break;
		case 1: {
			life::HomingProjectile* homing = new life::HomingProjectile(GetResourceManager(), "missile", ui_manager_, this);
			homing->SetTarget(avatar_id_);
			projectile = homing;
		} break;
	}
	AddContextObject(projectile, cure::kNetworkObjectLocalOnly, 0);
	projectile->SetOwnerInstanceId(canon->GetInstanceId());
	xform t(canon->GetOrientation(), canon->GetPosition());
	projectile->SetInitialTransform(t);
	projectile->StartLoading();

	uitbc::ParticleRenderer* particle_renderer = (uitbc::ParticleRenderer*)ui_manager_->GetRenderer()->GetDynamicRenderer("particle");
	vec3 v;
	life::ProjectileUtil::GetBarrel(projectile, t, v);
	particle_renderer->CreateFlare(vec3(0.9f, 0.7f, 0.5f), 0.3f, 7.5f, t.GetPosition(), v);
}

void DownwashManager::Detonate(cure::ContextObject* explosive, const tbc::ChunkyBoneGeometry* explosive_geometry, const vec3& position, const vec3& velocity, const vec3& normal, float strength) {
	collision_sound_manager_->OnCollision(strength, position, explosive_geometry, "explosion");

	uitbc::ParticleRenderer* particle_renderer = (uitbc::ParticleRenderer*)ui_manager_->GetRenderer()->GetDynamicRenderer("particle");
	const float keep_on_going_factor = 0.5f;	// How much of the velocity energy, [0;1], should be transferred to the explosion particles.
	vec3 u = velocity.ProjectOntoPlane(normal) * (1+keep_on_going_factor);
	u -= velocity;	// Mirror and inverse.
	u.Normalize();
	const int particles = Math::Lerp(8, 20, strength * 0.2f);
	vec3 start_fire_color(1.0f, 1.0f, 0.3f);
	vec3 fire_color(0.6f, 0.4f, 0.2f);
	vec3 start_smoke_color(0.4f, 0.4f, 0.4f);
	vec3 smoke_color(0.2f, 0.2f, 0.2f);
	vec3 shrapnel_color(0.3f, 0.3f, 0.3f);	// Default debris color is gray.
	if (explosive->GetClassId().find("barrel") != str::npos) {
		start_fire_color.Set(0.9f, 1.0f, 0.8f);
		fire_color.Set(0.3f, 0.7f, 0.2f);
		start_smoke_color.Set(0.3f, 0.35f, 0.3f);
		smoke_color.Set(0.2f, 0.4f, 0.2f);
		shrapnel_color.Set(0.5f, 0.5f, 0.1f);
	}
	particle_renderer->CreateExplosion(position, strength, u, 1, 1, start_fire_color, fire_color, start_smoke_color, smoke_color, shrapnel_color, particles, particles, particles/2, particles/3);


	// Slowmo check.
	const bool is_avatar = (explosive->GetInstanceId() == avatar_id_);
	bool normal_death = true;
	if (is_avatar && cure::Health::Get(explosive) < -5500) {
		if (Random::Uniform(0.0f, 1.0f) > 0.7f) {
			normal_death = false;
			slowmo_timer_.TryStart();
			GetContext()->DelayKillObject(explosive, 6.5);
		}
	}
	if (normal_death) {
		GetContext()->DelayKillObject(explosive, 3.0);
	}
	if (is_avatar) {
		((UiCure::Machine*)explosive)->GetBurnEmitter()->SetFreeFlow();
	}


	// Shove!
	ScopeLock lock(GetTickLock());
	tbc::PhysicsManager* physics_manager = GetPhysicsManager();
	cure::ContextManager::ContextObjectTable object_table = GetContext()->GetObjectTable();
	cure::ContextManager::ContextObjectTable::iterator x = object_table.begin();
	for (; x != object_table.end(); ++x) {
		cure::ContextObject* _object = x->second;
		if (!_object->IsLoaded()) {
			continue;
		}
		float _force = life::Explosion::CalculateForce(physics_manager, _object, position, strength) * 0.5f;
		if (explosive->GetClassId().find("missile") != str::npos) {
			_force *= 5;
		}
		if (_force > 0 && _object->GetNetworkObjectType() != cure::kNetworkObjectLocalOnly) {
			cure::FloatAttribute* health = cure::Health::GetAttribute(_object);
			if (health && !zoom_platform_) {
				const float value = health->GetValue() - _force*Random::Normal(0.51f, 0.05f, 0.3f, 0.5f);
				health->SetValue(value);
			}
			x->second->ForceSend();
		}
		life::Explosion::PushObject(physics_manager, _object, position, strength*0.1f, GetTimeManager()->GetNormalFrameTime());
	}
}

void DownwashManager::OnBulletHit(cure::ContextObject* bullet, cure::ContextObject* hit_object) {
	(void)hit_object;

	tbc::ChunkyPhysics* physics = bullet->GetPhysics();
	if (physics) {
		tbc::ChunkyBoneGeometry* geometry = physics->GetBoneGeometry(0);
		collision_sound_manager_->OnCollision(5.0f, bullet->GetPosition(), geometry, geometry->GetMaterial());
		cure::Health::Add(hit_object, -0.12f, false);
	}
}



bool DownwashManager::DidFinishLevel() {
	log_.Headlinef("Level %s done!", level_->GetClassId().c_str());
	cure::ContextObject* _avatar = GetContext()->GetObject(avatar_id_);
	if (_avatar && _avatar->GetPhysics()->GetEngineCount() >= 3) {
		const double _time = fly_time_.QuerySplitTime();
		const double level_best_time = GetCurrentLevelBestTime(false);
		const bool is_easy_mode = (GetControlMode() == 2);
		double rtr_offset;
		v_get(rtr_offset, =, GetVariableScope(), kRtvarPhysicsRtrOffset, 0.0);
		const bool is_toy_mode = (rtr_offset >= 0.5);
		if (_time > 0 && (_time < level_best_time || level_best_time <= 0) && !is_easy_mode && !is_toy_mode) {
			SetLevelBestTime(GetCurrentLevelNumber(), false, _time);

			str pilot_name;
			v_get(pilot_name, =, GetVariableScope(), kRtvarGamePilotname, kDefaultPilotName);
			const bool is_non_default_pilot_name =  (pilot_name != kDefaultPilotName);
			if (is_non_default_pilot_name) {
				hiscore_level_index_ = GetCurrentLevelNumber();
				my_hiscore_index_ = -1;
				hiscore_just_uploaded_timer_.Stop();
				CreateHiscoreAgent();
				const str level_name = strutil::Format("level_%i", GetCurrentLevelNumber());
				const int negative_time = (int)(_time*-1000);
				if (!hiscore_agent_->StartUploadingScore(kPlatform, level_name, kVehicleName, pilot_name, negative_time)) {
					delete hiscore_agent_;
					hiscore_agent_ = 0;
				}
			}
		}

		UiCure::UserSound2dResource* finish_sound = new UiCure::UserSound2dResource(ui_manager_, uilepra::SoundManager::kLoopNone);
		new UiCure::SoundReleaser(GetResourceManager(), ui_manager_, GetContext(), "finish.wav", finish_sound, 1.0f, 1.0f);
		zoom_platform_ = true;
		level_completed_ = true;
		win_image_timer_.Start();
		return true;
	}
	return false;
}

str DownwashManager::StepLevel(int count) {
	if (GetContext()->GetObject(avatar_id_)) {
		int level_number = GetCurrentLevelNumber();
		level_number += count;
		int level_count;
		v_get(level_count, =, GetVariableScope(), kRtvarGameLevelcount, 14);
		if (level_number >= level_count) {
			level_number = 0;

			double rtr_offset;
			v_get(rtr_offset, =, GetVariableScope(), kRtvarPhysicsRtrOffset, 0.0);
			rtr_offset += 1;
			v_set(GetVariableScope(), kRtvarPhysicsRtrOffset, rtr_offset);
			v_set(GetVariableScope(), kRtvarPhysicsRtr, 1.0+rtr_offset);
			v_set(GetVariableScope(), kRtvarGameAllowtoymode, true);
		}
		if (level_number < 0) {
			level_number = level_count-1;
		}
		level_number = ORDERED_LEVELNO[level_number];
		old_level_ = level_;
		level_completed_ = false;
		str new_level_name = strutil::Format("level_%.2i", level_number);
		level_ = (Level*)Parent::CreateContextObject(new_level_name, cure::kNetworkObjectLocallyControlled, 0);
		level_->StartLoading();
		v_set(GetVariableScope(), kRtvarGameStartLevel, new_level_name);
		return new_level_name;
	}
	return "";
}



Level* DownwashManager::GetLevel() const {
	if (level_ && level_->IsLoaded()) {
		return level_;
	}
	return 0;
}

int DownwashManager::GetCurrentLevelNumber() const {
	int level_number = 0;
	strutil::StringToInt(level_->GetClassId().substr(6), level_number);
	return REVERSED_LEVELNO[level_number];
}

double DownwashManager::GetCurrentLevelBestTime(bool world) const {
	const int _level_index = GetCurrentLevelNumber();
	const str record_format = world? kRtvarGameWorldrecordLevel "_%i" : kRtvarGamePersonalrecordLevel "_%i";
	const str level_time_var_name = strutil::Format(record_format.c_str(), _level_index);
	static HashedString fast_name(level_time_var_name);
	if (fast_name != level_time_var_name) {
		fast_name = level_time_var_name;
	}
	return GetVariableScope()->GetDefaultValue(cure::RuntimeVariableScope::kReadIgnore, fast_name, 0.0);
}

void DownwashManager::SetLevelBestTime(int level_index, bool world, double time) {
	const str record_format = world? kRtvarGameWorldrecordLevel "_%i" : kRtvarGamePersonalrecordLevel "_%i";
	const str level_time_var_name = strutil::Format(record_format.c_str(), level_index);
	GetVariableScope()->SetValue(cure::RuntimeVariable::kUsageSysOverride, level_time_var_name, time);
}

cure::ContextObject* DownwashManager::GetAvatar() const {
	cure::ContextObject* _avatar = GetContext()->GetObject(avatar_id_);
	return _avatar;
}



cure::RuntimeVariableScope* DownwashManager::GetVariableScope() const {
	return (Parent::GetVariableScope());
}



bool DownwashManager::InitializeUniverse() {
	// Create dummy explosion to ensure all geometries loaded and ready, to avoid LAAAG when first exploading.
	uitbc::ParticleRenderer* particle_renderer = (uitbc::ParticleRenderer*)ui_manager_->GetRenderer()->GetDynamicRenderer("particle");
	const vec3 v;
	particle_renderer->CreateExplosion(vec3(0,0,-2000), 1, v, 1, 1, v, v, v, v, v, 1, 1, 1, 1);

	double rtr_offset;
	v_get(rtr_offset, =, GetVariableScope(), kRtvarPhysicsRtrOffset, 0.0);
	v_set(GetVariableScope(), kRtvarPhysicsRtr, 1.0+rtr_offset);

	str start_level;
	v_get(start_level, =, GetVariableScope(), kRtvarGameStartLevel, "level_06");
	mass_object_array_.clear();
	level_ = (Level*)Parent::CreateContextObject(start_level, cure::kNetworkObjectLocallyControlled, 0);
	level_->StartLoading();
	tbc::BoneHierarchy* transform_bones = new tbc::BoneHierarchy;
	transform_bones->SetBoneCount(1);
	transform_bones->FinalizeInit(tbc::BoneHierarchy::kTransformNone);
	hemisphere_uv_transform_ = new tbc::BoneAnimator(transform_bones);
	hemisphere_ = (UiCure::CppContextObject*)Parent::CreateContextObject("hemisphere", cure::kNetworkObjectLocallyControlled, 0);
	hemisphere_->EnableRootShadow(false);
	hemisphere_->EnableMeshMove(false);
	hemisphere_->SetPhysicsTypeOverride(cure::kPhysicsOverrideBones);
	hemisphere_->SetInitialTransform(xform(quat(-1.69f, vec3(1,0,0)), vec3(0, 25, 0)));
	hemisphere_->StartLoading();
	sunlight_ = new Sunlight(ui_manager_);
	autopilot_ = new Autopilot(this);
	return true;
}

void DownwashManager::CreateChopper(const str& class_id) {
	hit_ground_frame_count_ = kStillFramesUntilCamPans;
	zoom_platform_ = false;

	cure::ContextObject* _avatar = Parent::CreateContextObject(class_id, cure::kNetworkObjectLocallyControlled, 0);
	cure::Spawner* spawner = GetAvatarSpawner(level_->GetInstanceId());
	if (spawner) {
		spawner->PlaceObject(_avatar, -1);
	}
	_avatar->QuerySetChildishness(1);
	avatar_id_ = _avatar->GetInstanceId();
	_avatar->StartLoading();
	autopilot_->Reset();
}

void DownwashManager::UpdateChopperColor(float lerp) {
	UiCure::CppContextObject* _avatar = (UiCure::CppContextObject*)GetContext()->GetObject(avatar_id_);
	if (!_avatar || !level_ || !level_->IsLoaded()) {
		return;
	}
	const float level_brightness = std::min(1.0f, level_->GetMesh(0)->GetBasicMaterialSettings().diffuse_.GetLength() * 2);
	uitbc::ChunkyClass* clazz = (uitbc::ChunkyClass*)_avatar->GetClass();
	const size_t mesh_count = clazz->GetMeshCount();
	for (size_t x = 0; x < mesh_count; ++x) {
		const vec3 d = (x == 0 && set_random_chopper_color_)? last_chopper_color_ : clazz->GetMaterial(x).diffuse_;
		_avatar->GetMesh(x)->GetBasicMaterialSettings().ambient_ = Math::Lerp(_avatar->GetMesh(x)->GetBasicMaterialSettings().ambient_, clazz->GetMaterial(x).ambient_ * level_brightness, lerp);
		_avatar->GetMesh(x)->GetBasicMaterialSettings().diffuse_ = Math::Lerp(_avatar->GetMesh(x)->GetBasicMaterialSettings().diffuse_, d * level_brightness, lerp);
	}
}

void DownwashManager::TickInput() {
	TickNetworkInput();
	TickUiInput();
	TickHiscore();
}



void DownwashManager::UpdateCameraDistance() {
	double cam_distance = 11.3 * ui_manager_->GetDisplayManager()->GetPhysicalScreenSize();
	cam_distance = (cam_distance+110)/2;	// Smooth towards a sensible cam distance.
	if (menu_ && menu_->GetDialog() != 0) {
		cam_distance *= 0.4f;
	}
	cam_distance = std::min(110.0, cam_distance);
	v_set(GetVariableScope(), kRtvarUi3DCamdistance, cam_distance);
}

void DownwashManager::UpdateTouchstickPlacement() {
	if (touchstick_timer_.QueryTimeDiff() < 2.0) {
		return;
	}
	touchstick_timer_.ClearTimeDiff();

#if defined(LEPRA_TOUCH) || defined(EMULATE_TOUCH)
	const float touch_side_scale = 1.28f;	// Inches.
	const float touch_scale = touch_side_scale / (float)ui_manager_->GetDisplayManager()->GetPhysicalScreenSize();
	if (!stick_) {
		int screen_pixel_width;
		v_get(screen_pixel_width, =, GetVariableScope(), kRtvarUiDisplayWidth, 1024);
		const int minimum_touch_radius = (int)(screen_pixel_width*touch_scale*0.17f);	// Touched area is a fraction of the required 32px/iPhone classic.
		stick_ = new Touchstick(ui_manager_->GetInputManager(), Touchstick::kModeRelativeCenter, PixelRect(0, 0, 10, 10),  0, minimum_touch_radius);
		stick_->SetUniqueIdentifier("Touchstick");
		stick_->SetValueScale(-1,+1, -1,-0.2f);
	}
	int finger_size = (int)(render_area_.GetWidth() * touch_scale);
	if (finger_size > render_area_.GetHeight()/3) {
		finger_size = render_area_.GetHeight()/3;
	}
	int joystick_size = finger_size*4/3;
	PixelRect right_stick_area(render_area_);
	right_stick_area.left_ = render_area_.GetWidth() - joystick_size;
	right_stick_area.top_ = right_stick_area.bottom_ - joystick_size;
	const int ow = right_stick_area.GetWidth();
	const int r = (int)(ow*0.27f);	// Knob radius.
	const int margin = (int)(r*0.2f);
	stick_->SetFingerRadius(r+margin);
	stick_->Move(right_stick_area, 0);
#endif // touch or emulated touch
}

int DownwashManager::GetControlMode() const {
	// Three modes: easy, medium, hard.
	float childishness;
	v_get(childishness, =(float), GetVariableScope(), kRtvarGameChildishness, 1.0);
	if (childishness > 0.95f) {
		return 2;	// Easy.
	} else if (childishness >= 0.1f) {
		return 1;	// Medium.
	}
	return 0;	// Hard.
}

void DownwashManager::UpdateControlMode() {
	cure::CppContextObject* _avatar = (cure::CppContextObject*)GetContext()->GetObject(avatar_id_);
	if (!_avatar || _avatar->GetPhysics()->GetEngineCount() < 3) {
		return;
	}
	const int helper_engine_index = _avatar->GetPhysics()->GetEngineCount() - 1;
	if (GetControlMode() == 2) {
		// Helper engine on + upright stabilization high.
		_avatar->GetPhysics()->GetEngine(helper_engine_index)->SetStrength(8000.0f);
		((tbc::ChunkyClass::Tag*)_avatar->GetClass()->GetTag("upright_stabilizer"))->float_value_list_[0] = 3;
	} else {
		// Engine off, low upright stabilization. Arcade mode compensates by having high autopilot and high durability.
		_avatar->GetPhysics()->GetEngine(helper_engine_index)->SetStrength(0.0f);
		((tbc::ChunkyClass::Tag*)_avatar->GetClass()->GetTag("upright_stabilizer"))->float_value_list_[0] = 0.3f;
	}
}

void DownwashManager::TickUiInput() {
	ui_manager_->GetInputManager()->SetCursorVisible(true);

	UpdateControlMode();

	const int physics_step_count = GetTimeManager()->GetAffordedPhysicsStepCount();
	if (physics_step_count > 0 && allow_movement_input_) {
		cure::ContextObject* _object = GetContext()->GetObject(avatar_id_);

		if (_object) {
			// Control steering.
			float childishness;
			v_get(childishness, =(float), GetVariableScope(), kRtvarGameChildishness, 1.0);
			if (hit_ground_frame_count_ >= -kStillFramesUntilCamPans) {
				childishness = 0;	// Don't help when not even started yet.
			}
			const vec3 auto_pilot3d = autopilot_->GetSteering();
			const vec2 auto_pilot(auto_pilot3d.x, auto_pilot3d.z);
			const vec3 auto_pilot_direction3d = autopilot_->GetClosestPathVector().GetNormalized();
			const vec2 auto_pilot_direction(auto_pilot_direction3d.x, auto_pilot_direction3d.z);
			const life::options::Steering& s = options_.GetSteeringControl();
#define S(dir) s.control_[life::options::Steering::kControl##dir]
			vec2 user_controls(S(Right3D) - S(Left3D), S(Up3D) - S(Down3D));
			vec2 user_direction = user_controls;
			const vec3 v = _object->GetVelocity();
			if (user_direction.GetLengthSquared() < 0.01f) {	// User is not controlling, AI is.
				if (v.GetLengthSquared() > 0.1f) {
					user_controls = auto_pilot * childishness;
					user_direction = user_controls;
				}
			} else if (user_direction.Dot(auto_pilot_direction) >= 0) {	// User wants to go in the same direction as AI, so AI helps.
				user_direction = Math::Lerp(user_direction, auto_pilot, childishness*0.35f);
			} else {
				// User wants to go in opposite direction of AI, so let him.
				// At the same time we can check if he's on to something new.
				static int cntHack = 0;
				if (++cntHack > 30) {
					autopilot_->AttemptCloserPathDistance();
				}
			}
			// Pull the brakes a little bit.
			const float current_flying_direction_x = v.x * 0.05f;
			user_direction.x = Math::Clamp(user_direction.x-current_flying_direction_x, -1.0f, +1.0f);
			//user_direction.x = (fabs(v.x) > 0.5f) ? user_direction.x : 0;
			// X follows helicopter yaw.
			float yaw, _;
			_object->GetOrientation().GetEulerAngles(yaw, _, _);
			const float power_fwd_rev = -::sin(yaw) * user_direction.x;
			const float power_left_right = ::cos(yaw) * user_direction.x;
			SetAvatarEnginePower(_object, 4, power_fwd_rev);
			SetAvatarEnginePower(_object, 5, power_left_right);
			SetAvatarEnginePower(_object, 7, user_direction.y);
			// Kids' push engine.
			if (autopilot_) {
				float f = std::min(1.0f, autopilot_->GetRotorSpeed(_object) / 14.0f);
				f /= 1+0.02f*v.GetLength();	// Reduce push at higher speeds or the chopper will spin out of control for some reson.
				SetAvatarEnginePower(_object,  9, user_controls.x*f);
				SetAvatarEnginePower(_object, 11, user_direction.y*f);
			}
			const life::options::FireControl& fc = options_.GetFireControl();
#define F(alt) fc.control_[life::options::FireControl::kFire##alt]
			if (F(0) > 0.5f) {
				//_avatar.Shoot();
			}
		}
	}
}

bool DownwashManager::SetAvatarEnginePower(cure::ContextObject* avatar, unsigned aspect, float power) {
	if (zoom_platform_) {
		avatar->SetEnginePower(aspect, 0);
		return false;
	}
	return avatar->SetEnginePower(aspect, power);
}

void DownwashManager::TickUiUpdate() {
	if (!is_hit_this_frame_ && !menu_->GetDialog()) {
		--hit_ground_frame_count_;
		if (hit_ground_frame_count_ > 0) {
			hit_ground_frame_count_ = 0;
		}
	}
	is_hit_this_frame_ = false;

	if (GetTimeManager()->GetAffordedPhysicsStepTime() > 1.1f/PHYSICS_FPS) {
		if (++slow_system_counter_ > 30) {
			v_set(GetVariableScope(), kRtvarUi3DEnablemassobjects, false);
		}
	} else {
		slow_system_counter_ = 0;
	}


	const cure::ContextObject* _avatar = GetContext()->GetObject(avatar_id_);
	if (hit_ground_frame_count_ <= -1 && level_ && level_->IsLoaded() && _avatar) {
		if (_avatar->GetPhysics()->GetEngineCount() < 3) {
			fly_time_.Stop();
		} else if (!fly_time_.IsStarted() && !level_completed_) {
			cure::Spawner* spawner = GetAvatarSpawner(level_->GetInstanceId());
			deb_assert(spawner);
			const float distance_to_home = GetContext()->GetObject(avatar_id_)->GetPosition().GetDistance(spawner->GetSpawnPoint().GetPosition());
			if (distance_to_home < 15) {
				fly_time_.TryStart();
			} else {
				fly_time_.ResumeFromLapTime();
			}
		}
	}

	((DownwashConsoleManager*)GetConsoleManager())->GetUiConsole()->Tick();
	collision_sound_manager_->Tick(camera_transform_.GetPosition());
}

bool DownwashManager::UpdateMassObjects(const vec3& position) {
	if (level_ && level_->IsLoaded() && mass_object_array_.empty()) {
		const Level::MassObjectList mass_objects = level_->GetMassObjects();
		for (Level::MassObjectList::const_iterator x = mass_objects.begin(); x != mass_objects.end(); ++x) {
			const Level::MassObjectInfo& info = *x;
			const tbc::PhysicsManager::BodyID terrain_body_id = level_->GetPhysics()->GetBoneGeometry(info.ground_body_index_)->GetBodyId();
			cure::GameObjectId mass_object_id = GetContext()->AllocateGameObjectId(cure::kNetworkObjectLocalOnly);
			mass_object_array_.push_back(mass_object_id);
			life::MassObject* mass_object = new life::MassObject(GetResourceManager(), info.class_id_, ui_manager_, terrain_body_id, info.count_, 120);
			mass_object->SetSeed((size_t)HashString(info.class_id_.c_str()));
			AddContextObject(mass_object, cure::kNetworkObjectLocalOnly, mass_object_id);
			mass_object->StartLoading();
			level_->AddChild(mass_object);
		}
	}

	ObjectArray::const_iterator x = mass_object_array_.begin();
	for (; x != mass_object_array_.end(); ++x) {
		life::MassObject* _object = (life::MassObject*)GetContext()->GetObject(*x, true);
		deb_assert(_object);
		_object->SetRootPosition(position);
	}

	return true;
}

void DownwashManager::SetLocalRender(bool render) {
	if (render) {
		bool mass;
		v_get(mass, =, GetVariableScope(), kRtvarUi3DEnablemassobjects, false);
		SetMassRender(mass);
	} else {
		SetMassRender(false);
	}
}

void DownwashManager::SetMassRender(bool render) {
	ObjectArray::const_iterator x = mass_object_array_.begin();
	for (; x != mass_object_array_.end(); ++x) {
		life::MassObject* _object = (life::MassObject*)GetContext()->GetObject(*x);
		if (_object) {
			_object->SetRender(render);
		}
	}
}



cure::ContextObject* DownwashManager::CreateContextObject(const str& class_id) const {
	cure::CppContextObject* _object;
	if (class_id == "missile") {
		_object = new life::HomingProjectile(GetResourceManager(), class_id, ui_manager_, (DownwashManager*)this);
	} else if (class_id == "grenade" || class_id == "rocket") {
		_object = new life::FastProjectile(GetResourceManager(), class_id, ui_manager_, (DownwashManager*)this);
	} else if (class_id == "bomb") {
		_object = new life::Projectile(GetResourceManager(), class_id, ui_manager_, (DownwashManager*)this);
	} else if (strutil::StartsWith(class_id, "barrel")) {
		life::Mine* mine = new life::Mine(GetResourceManager(), class_id, ui_manager_, (DownwashManager*)this);
		cure::Health::Set(mine, 0.3f);
		mine->EnableDeleteDetonation(false);
		mine->SetExplosiveStrength(5);
		new cure::FloatAttribute(mine, "DamageAbsorption", 0.2f);
		new cure::FloatAttribute(mine, "DamageReduction", 0.3f);
		_object = mine;
	} else if (class_id == "stone" || class_id == "cube") {
		_object = new CenteredMachine(GetResourceManager(), class_id, ui_manager_, (DownwashManager*)this);
		cure::Health::DeleteAttribute(_object);
	} else if (class_id == "beach_ball" || class_id == "dino_bubble" || class_id == "clown_bubble") {
		CenteredMachine* machine = new CenteredMachine(GetResourceManager(), class_id, ui_manager_, (DownwashManager*)this);
		new cure::FloatAttribute(machine, "DamageAbsorption", 1e-3f);
		machine->SetDeathFrameDelay(0);
		machine->SetDisappearAfterDeathDelay(0.2f);
		_object = machine;
	} else if (strutil::StartsWith(class_id, "level_")) {
		UiCure::GravelEmitter* gravel_particle_emitter = new UiCure::GravelEmitter(GetResourceManager(), ui_manager_, 0.5f, 1, 10, 2);
		Level* _level = new Level(GetResourceManager(), class_id, ui_manager_, gravel_particle_emitter);
		_level->EnableRootShadow(false);
		_object = _level;
	} else if (strutil::StartsWith(class_id, "helicopter_") || strutil::StartsWith(class_id, "monster") ||
		strutil::StartsWith(class_id, "fighter")) {
		CenteredMachine* machine = new CenteredMachine(GetResourceManager(), class_id, ui_manager_, (DownwashManager*)this);
		if (strutil::StartsWith(class_id, "helicopter_")) {
			new cure::FloatAttribute(machine, "DamageAbsorption", 1);
			machine->SetDeathFrameDelay(2);
		}
		machine->SetJetEngineEmitter(new UiCure::JetEngineEmitter(GetResourceManager(), ui_manager_));
		//machine->SetExhaustEmitter(new UiCure::ExhaustEmitter(GetResourceManager(), ui_manager_));
		machine->SetBurnEmitter(new UiCure::BurnEmitter(GetResourceManager(), ui_manager_));
		_object = machine;
	} else if (strutil::StartsWith(class_id, "forklift") || strutil::StartsWith(class_id, "corvette") ||
		strutil::StartsWith(class_id, "air_balloon") || strutil::StartsWith(class_id, "turret2")) {
		UiCure::Machine* machine = new BaseMachine(GetResourceManager(), class_id, ui_manager_, (DownwashManager*)this);
		if (strutil::StartsWith(class_id, "turret")) {
			new cure::FloatAttribute(machine, "DamageAbsorption", 1);
		}
		//machine->SetExhaustEmitter(new UiCure::ExhaustEmitter(GetResourceManager(), ui_manager_));
		machine->SetBurnEmitter(new UiCure::BurnEmitter(GetResourceManager(), ui_manager_));
		_object = machine;
	} else {
		//log_.Infof("Creating object of type %s.", class_id.c_str());
		UiCure::Machine* machine = new UiCure::Machine(GetResourceManager(), class_id, ui_manager_);
		//machine->SetExhaustEmitter(new UiCure::ExhaustEmitter(GetResourceManager(), ui_manager_));
		_object = machine;
	}
	_object->SetAllowNetworkLogic(true);
	return (_object);
}

cure::ContextObject* DownwashManager::CreateLogicHandler(const str& type) {
	if (type == "spawner" || type == "spawner_init") {
		return new life::Spawner(GetContext());
	} else if (type == "real_time_ratio") {
		return new LandingTrigger(GetContext());
	} else if (type == "eater") {
		return new Eater(GetContext());
	} else if (type == "context_path") {
		return level_->QueryPath();
	} else if (type == "trig_elevator") {
		cure::Elevator* elevator = new cure::Elevator(GetContext());
		elevator->SetStopDelay(0.5f);
		return elevator;
	} else if (strutil::StartsWith(type, "driver:")) {
		return new cure::Driver(GetContext());
	}
	return (0);
}

void DownwashManager::OnLoadCompleted(cure::ContextObject* object, bool ok) {
	if (ok) {
		if (object->GetInstanceId() == avatar_id_) {
			log_volatile(log_.Debug("Yeeha! Loaded avatar!"));
			if (set_random_chopper_color_) {
				vec3 color;
				do {
					color = RNDPOSVEC();
				} while (color.GetDistanceSquared(last_vehicle_color_) < 1);
				last_vehicle_color_ = color;
				last_chopper_color_ = color;
				((UiCure::CppContextObject*)object)->GetMesh(0)->GetBasicMaterialSettings().diffuse_ = color;
			}
			UpdateChopperColor(1.0f);
			EaseDown(object, 0);
		} else if (object == level_) {
			OnLevelLoadCompleted();
		} else if (strutil::StartsWith(object->GetClassId(), "monster")) {
			((life::ExplodingMachine*)object)->SetDeathFrameDelay(5);
			vec3 _direction(-1,0,0);
			new Automan(this, object->GetInstanceId(), _direction);
			vec3 color;
			do {
				color = RNDPOSVEC();
			} while (color.GetDistanceSquared(last_vehicle_color_) < 1);
			last_vehicle_color_ = color;
			((UiCure::CppContextObject*)object)->GetMesh(0)->GetBasicMaterialSettings().diffuse_ = color;
		} else if (strutil::StartsWith(object->GetClassId(), "forklift") ||
			strutil::StartsWith(object->GetClassId(), "corvette")) {
			str base_name = strutil::Split(object->GetClassId(), "_")[0];
			new AutoPathDriver(this, object->GetInstanceId(), base_name+"_path");
			vec3 color;
			do {
				color = RNDPOSVEC();
			} while (color.GetDistanceSquared(last_vehicle_color_) < 1);
			last_vehicle_color_ = color;
			((UiCure::CppContextObject*)object)->GetMesh(0)->GetBasicMaterialSettings().diffuse_ = color;
		} else if (object->GetClassId() == "turret") {
			new CanonDriver(this, object->GetInstanceId(), 0);
		} else if (object->GetClassId() == "turret2") {
			new CanonDriver(this, object->GetInstanceId(), 1);
		} else if (strutil::StartsWith(object->GetClassId(), "air_balloon")) {
			new AirBalloonPilot(this, object->GetInstanceId());
			if (last_vehicle_color_.y > 0.4f && last_vehicle_color_.x < 0.3f && last_vehicle_color_.z < 0.3f) {
				last_vehicle_color_ = vec3(0.6f, 0.2f, 0.2f);
				((UiCure::CppContextObject*)object)->GetMesh(2)->GetBasicMaterialSettings().diffuse_ = last_vehicle_color_;
			} else {
				last_vehicle_color_ = vec3(0, 1, 0);
			}
		} else if (strutil::StartsWith(object->GetClassId(), "fighter")) {
			//object->SetEnginePower(0, 1);
			object->SetEnginePower(1, 1);
			//object->SetEnginePower(2, 1);
		} else if (strutil::StartsWith(object->GetClassId(), "simulator")) {
			new SimulatorDriver(this, object->GetInstanceId());
		} else {
			log_volatile(log_.Tracef("Loaded object %s.", object->GetClassId().c_str()));
		}
		object->GetPhysics()->UpdateBonesObjectTransformation(0, kIdentityTransformationF);
		((UiCure::CppContextObject*)object)->UiMove();
	} else {
		log_.Errorf("Could not load object of type %s.", object->GetClassId().c_str());
		GetContext()->PostKillObject(object->GetInstanceId());
	}
}

void DownwashManager::OnLevelLoadCompleted() {
	/*// Kickstart all elevators.
	for (int x = 0; x < 20; ++x) {
		level_->SetEnginePower(x, 1);
	}*/

	cure::ContextObject* _avatar = GetContext()->GetObject(avatar_id_);
	if (_avatar && _avatar->IsLoaded() && _avatar->GetPhysics()->GetEngineCount() < 3) {
		// Avatar somehow crashed after landing last level... Well done, but not impossible in a dynamic world.
	} else if (!_avatar) {
		CreateChopper(kVehicleName);
		//v_set(GetVariableScope(), kRtvarPhysicsHalt, true);
	} else {
		cure::Health::Set(_avatar, 1);
		cure::Spawner* spawner = GetAvatarSpawner(level_->GetInstanceId());
		deb_assert(spawner);
		const vec3 landing_position = GetLandingTriggerPosition(old_level_);
		const vec3 heli_position = _avatar->GetPosition();
		const vec3 heli_delta = heli_position - landing_position;
		vec3 new_position = spawner->GetSpawnPoint().GetPosition() + heli_delta;
		const float cam_above_heli = camera_transform_.GetPosition().z - heli_position.z;
		const vec3 cam_delta = spawner->GetSpawnPoint().GetPosition() - landing_position;

		mass_object_array_.clear();
		GetContext()->DeleteObject(old_level_->GetInstanceId());
		old_level_ = 0;

		EaseDown(_avatar, &new_position);

		camera_transform_.GetPosition() += cam_delta;
		camera_transform_.GetPosition().z = _avatar->GetPosition().z + cam_above_heli;
		camera_previous_position_ = camera_transform_.GetPosition();
		UpdateCameraPosition(true);
	}
	if (old_level_) {
		mass_object_array_.clear();
		GetContext()->DeleteObject(old_level_->GetInstanceId());
		old_level_ = 0;
	}
	if (level_->GetBackgroundName().empty()) {
		render_hemisphere_ = false;
	} else {
		render_hemisphere_ = true;
		hemisphere_->ReplaceTexture(0, level_->GetBackgroundName());
	}

	for (int engine = 30; engine < 40; ++engine) {
		level_->SetEnginePower(engine, 1.0f);
	}

	UpdateHiscoreDialogTitle();
	GetLandingTriggerPosition(level_);	// Update shadow landing trigger position.
	autopilot_->Reset();
	zoom_platform_ = false;
}

void DownwashManager::OnCollision(const vec3& force, const vec3& torque, const vec3& position,
	cure::ContextObject* object1, cure::ContextObject* object2,
	tbc::PhysicsManager::BodyID body1_id, tbc::PhysicsManager::BodyID body2_id) {
	collision_sound_manager_->OnCollision(force, torque, position, object1, object2, body1_id, 5000, false);

	bool is_avatar = (object1->GetInstanceId() == avatar_id_);

	float childishness = 0.0f;

	if (!is_hit_this_frame_ && is_avatar) {
		// If it's a lever or something like that, this is not a landing!
		if (object2->GetPhysics()->GetPhysicsType() != tbc::ChunkyPhysics::kDynamic &&
			!object2->GetPhysics()->GetBoneGeometry(body2_id)->IsCollideWithSelf()) {
			is_hit_this_frame_ = true;
			if (hit_ground_frame_count_ < 0) {
				hit_ground_frame_count_ = 0;
			}
			++hit_ground_frame_count_;
		}
	}

	// Check if we're just stabilizing on starting pad.
	if (is_avatar && avatar_create_timer_.IsStarted()) {
		return;
	}

	// Check if we're colliding with a smooth landing pad.
	bool is_landing_pad = false;
	bool is_landing_on_elevator = false;
	float collision_impact_factor = 3;
	if (is_avatar && object2 == level_) {
		const tbc::ChunkyClass::Tag* tag = level_->GetClass()->GetTag("anything");
		std::vector<int>::const_iterator x = tag->body_index_list_.begin();
		for (; x != tag->body_index_list_.end(); ++x) {
			tbc::ChunkyBoneGeometry* bone = object2->GetPhysics()->GetBoneGeometry(*x);
			if (bone->GetBodyId() == body2_id) {
				is_landing_pad = true;
				collision_impact_factor = 1;
				fly_time_.Stop();
				if (bone->GetJointType() != tbc::ChunkyBoneGeometry::kJointExclude) {
					is_landing_on_elevator = true;
				} break;
			}
		}
	}

	const float orientation_factor = (object1->GetOrientation()*vec3(0,0,1)*vec3(0,0,1));

	// Don't do collisions if heli hasn't moved, such as in the case of standing on
	// an elevator.
	if (is_landing_on_elevator && is_avatar && is_landing_pad && hit_ground_frame_count_ >= kStillFramesUntilCamPans && orientation_factor > 0.95f) {
		return;
	}

	if (is_avatar) {
		v_get(childishness, =(float), GetVariableScope(), kRtvarGameChildishness, 1.0);
	}

	// Check if it's a rotor!
	bool is_rotor = false;
	const float damage_factor = object1->GetAttributeFloatValue("DamageAbsorption");
	if (damage_factor) {
		collision_impact_factor *= damage_factor;
		tbc::ChunkyBoneGeometry* geometry = object1->GetStructureGeometry(body1_id);
		if (geometry->GetJointType() == tbc::ChunkyBoneGeometry::kJointHinge &&
			geometry->GetGeometryType() == tbc::ChunkyBoneGeometry::kGeometryBox) {
			is_rotor = true;
			tbc::ChunkyBoneGeometry* hit_bone = object2->GetPhysics()->GetBoneGeometry(body2_id);
			collision_impact_factor *= Math::Lerp(1000.0f * hit_bone->GetImpactFactor(), 2.0f, childishness);
		} else if (is_avatar && object2->GetPhysics()->GetPhysicsType() == tbc::ChunkyPhysics::kDynamic) {
			const float damage_reduction = object2->GetAttributeFloatValue("DamageReduction");
			if (damage_reduction) {
				collision_impact_factor *= damage_reduction;
			}
		} else if (!is_avatar && object2->GetInstanceId() == avatar_id_) {
			collision_impact_factor *= 0.1f;
		}
	} else {
		collision_impact_factor *= 0.01f;
	}

	float _force = force.GetLength() * collision_impact_factor;
	if (childishness > 0.1f) {
		float up_factor = 1 + 0.5f*(object1->GetOrientation()*vec3(0,0,1)*vec3(0,0,1));
		up_factor *= up_factor;
		_force *= Math::Lerp(1.0f, 0.05f, childishness * up_factor);
		if (_force < 0) {
			_force = 0;
		}
	}
	// Take velocity into calculation if we're landing on a helipad (not using our rotors).
	if (is_landing_pad && !is_rotor) {
		_force *= force*object1->GetVelocity() * (collision_impact_factor / -15000);
	}
	_force *= 5 - 4*orientation_factor;	// Sideways orientation means chopper not aligned.
	if (_force > 15000) {
		_force /= 30000;
		_force *= 3 - 2*(force.GetNormalized()*vec3(0,0,1));	// Sideways force means non-vertical landing or landing on non-flat surface.
		if (cure::Health::Get(object1) > 0 && !zoom_platform_) {
			float force2 = _force*_force;
			force2 /= object1->GetMass();
			cure::Health::Add(object1, -force2, false);
		}
	}
}



vec3 DownwashManager::GetLandingTriggerPosition(cure::ContextObject* level) const {
	deb_assert(level);
	cure::ContextObject::Array::const_iterator x = level->GetChildArray().begin();
	for (; x != level->GetChildArray().end(); ++x) {
		LandingTrigger* landing_trigger = dynamic_cast<LandingTrigger*>(*x);
		if (landing_trigger) {
			const int trigger_count = level->GetPhysics()->GetTriggerCount();
			for (int x = 0; x < trigger_count; ++x) {
				const tbc::PhysicsTrigger* trigger = level->GetPhysics()->GetTrigger(x);
				if (level->GetTrigger(trigger->GetPhysicsTriggerId(0)) == landing_trigger) {
					last_landing_trigger_position_ = GetPhysicsManager()->GetBodyPosition(trigger->GetPhysicsTriggerId(0));
					return last_landing_trigger_position_;
				}
			}
		}
	}
	deb_assert(false);
	return vec3();
}

void DownwashManager::EaseDown(cure::ContextObject* object, const vec3* start_position) {
	all_loaded_timer_.Start();

	avatar_create_timer_.Start();
	fly_time_.Stop();
	fly_time_.PopTimeDiff();
	hit_ground_frame_count_ = kStillFramesUntilCamPans;
	is_hit_this_frame_ = true;

	cure::Spawner::EaseDown(GetPhysicsManager(), object, start_position);
}

xform DownwashManager::GetMainRotorTransform(const UiCure::CppContextObject* chopper) const {
	int phys_index;
	str mesh_name;
	xform transform;
	float mesh_scale;
	size_t mesh_count = ((uitbc::ChunkyClass*)chopper->GetClass())->GetMeshCount();
	for (size_t x = 0; x < mesh_count; ++x) {
		((uitbc::ChunkyClass*)chopper->GetClass())->GetMesh(x, phys_index, mesh_name, transform, mesh_scale);
		if (mesh_name.find("_rotor") != str::npos) {
			return chopper->GetMesh(x)->GetBaseTransformation();
		}
	}
	return transform;
}



void DownwashManager::OnPauseButton(uitbc::Button* button) {
	if (button) {
		menu_->OnTapSound(button);
		button->SetVisible(false);
	}

	uitbc::Dialog* d = menu_->CreateTbcDialog(life::Menu::ButtonAction(this, &DownwashManager::OnMenuAlternative), 0.8f, 0.8f);
	d->SetColor(kBgColor, OFF_BLACK, BLACK, BLACK);
	d->SetDirection(+1, false);

	uitbc::FixedLayouter layouter(d);

	str pilot_name;
	v_get(pilot_name, =, GetVariableScope(), kRtvarGamePilotname, kDefaultPilotName);
	const int difficulty_mode = GetControlMode();
	double master_volume;
	v_get(master_volume, =, GetVariableScope(), kRtvarUiSoundMastervolume, 1.0);
	bool allow_toy_mode;
	v_get(allow_toy_mode, =, GetVariableScope(), kRtvarGameAllowtoymode, false);
	double rtr_offset;
	v_get(rtr_offset, =, GetVariableScope(), kRtvarPhysicsRtrOffset, 0.0);

	uitbc::TextField* name_field = new uitbc::TextField(0, WHITE);
	name_field->SetName("pilot_name");
	name_field->SetText(wstrutil::Encode(pilot_name));
	layouter.AddWindow(name_field, 0, 5, 0, 1, 1);
	name_field->SetHorizontalMargin(name_field->GetPreferredHeight() / 3);
	//name_field->SetKeyboardFocus();

	layouter.SetContentXMargin(0);
	uitbc::RadioButton* easy_button = new uitbc::RadioButton(Color(20, 30, 20), L"Easy");
	easy_button->SetPressColor(Color(50, 210, 40));
	easy_button->SetRoundedRadiusMask(0x9);
	easy_button->SetPressed(difficulty_mode == 2);
	layouter.AddButton(easy_button, -2, 1, 5, 0, 1, 3, false);
	uitbc::RadioButton* medium_button = new uitbc::RadioButton(Color(30, 30, 20), L"Medium");
	medium_button->SetPressColor(Color(170, 165, 10));
	medium_button->SetRoundedRadiusMask(0);
	medium_button->SetPressed(difficulty_mode == 1);
	layouter.AddButton(medium_button, -3, 1, 5, 1, 1, 3, false);
	uitbc::RadioButton* hard_button = new uitbc::RadioButton(Color(30, 20, 20), L"Hard");
	hard_button->SetPressColor(Color(230, 40, 30));
	hard_button->SetRoundedRadiusMask(0x6);
	hard_button->SetPressed(difficulty_mode == 0);
	layouter.AddButton(hard_button, -4, 1, 5, 2, 1, 3, false);
	layouter.SetContentXMargin(layouter.GetContentYMargin());

	uitbc::CheckButton* toy_mode_button = new uitbc::CheckButton(Color(30, 70, 220), L"Toy mode");
	toy_mode_button->SetIcon(uitbc::Painter::kInvalidImageid, uitbc::Button::kIconRight);
	toy_mode_button->SetCheckedIcon(check_icon_->GetData());
	toy_mode_button->SetDisabledIcon(lock_icon_->GetData());
	toy_mode_button->Enable(allow_toy_mode);
	toy_mode_button->SetPressed(rtr_offset > 0.1);
	layouter.AddButton(toy_mode_button, -5, 2, 5, 0, 1, 2, false);
	if (!allow_toy_mode) {
		uitbc::TextArea* unlock_label = new uitbc::TextArea(BLACK);
		unlock_label->GetClientRectComponent()->SetIsHollow(true);
		unlock_label->SetFontColor(WHITE);
		unlock_label->AddText(L"Finish all levels to\nunlock toy mode");
		layouter.AddComponent(unlock_label, 2, 5, 1, 1, 2);
		unlock_label->SetHorizontalMargin(unlock_label->GetPreferredHeight() / 3);
	}

	uitbc::CheckButton* bedside_volume_button = new uitbc::CheckButton(Color(190, 50, 180), L"Bedside volume");
	bedside_volume_button->SetIcon(uitbc::Painter::kInvalidImageid, uitbc::Button::kIconRight);
	bedside_volume_button->SetCheckedIcon(check_icon_->GetData());
	bedside_volume_button->SetPressed(master_volume < 0.5);
	layouter.AddButton(bedside_volume_button, -6, 3, 5, 0, 1, 2, false);
	uitbc::Button* hiscore_button = new uitbc::Button(Color(90, 50, 10), L"High score");
	hiscore_button->SetIcon(uitbc::Painter::kInvalidImageid, uitbc::Button::kIconRight);
	layouter.AddButton(hiscore_button, -7, 3, 5, 1, 1, 2, true);

	uitbc::Button* restart_button = new uitbc::Button(Color(220, 110, 20), L"Restart from first level");
	restart_button->SetIcon(uitbc::Painter::kInvalidImageid, uitbc::Button::kIconRight);
	layouter.AddButton(restart_button, -8, 4, 5, 0, 1, 1, true);

	uitbc::Button* close_button = new uitbc::Button(Color(180, 60, 50), L"X");
	layouter.AddCornerButton(close_button, -9);

	v_set(GetVariableScope(), kRtvarPhysicsHalt, true);
}

void DownwashManager::OnLastHiscoreButton(uitbc::Button* button) {
	hiscore_just_uploaded_timer_.Stop();
	button->SetVisible(false);
	ShowHiscoreDialog(+1);
}

void DownwashManager::ShowHiscoreDialog(int direction) {
	pause_button_->SetVisible(false);
	hiscore_just_uploaded_timer_.Stop();
	menu_->DismissDialog();
	uitbc::Dialog* d = menu_->CreateTbcDialog(life::Menu::ButtonAction(this, &DownwashManager::OnMenuAlternative), 0.8f, 0.8f);
	d->SetColor(kBgColor, OFF_BLACK, BLACK, BLACK);
	d->SetName("hiscore_dialog");
	d->SetPreClickTarget(uitbc::Dialog::Action(this, &DownwashManager::OnPreHiscoreAction));
	d->SetDirection(direction, true);

	uitbc::FixedLayouter layouter(d);

	uitbc::Label* loading_label = new uitbc::Label(WHITE, L"Please wait while loading...");
	loading_label->SetIcon(uitbc::Painter::kInvalidImageid, uitbc::Label::kIconCenter);
	layouter.AddComponent(loading_label, 2, 5, 0, 1, 1);
	loading_label->SetAdaptive(false);
	d->SetQueryLabel(loading_label);

	uitbc::Button* close_button = new uitbc::Button(Color(180, 60, 50), L"X");
	layouter.AddCornerButton(close_button, -9);

	layouter.SetContentWidthPart(1);
	uitbc::Button* prev_level_button = ICONBTNA("btn_prev.png", L"");
	uitbc::Button* next_level_button = ICONBTNA("btn_next.png", L"");
	layouter.AddButton(prev_level_button, -100, 0, 1, 0, 1, 7, true);
	layouter.AddButton(next_level_button, -101, 0, 1, 6, 1, 7, true);
	prev_level_button->GetClientRectComponent()->SetIsHollow(true);
	prev_level_button->GetClientRectComponent()->SetBehaveSolid(true);
	next_level_button->GetClientRectComponent()->SetIsHollow(true);
	next_level_button->GetClientRectComponent()->SetBehaveSolid(true);

	CreateHiscoreAgent();
	hiscore_level_index_ = (hiscore_level_index_ >= 0)? hiscore_level_index_ : GetCurrentLevelNumber();
	const str level_name = strutil::Format("level_%i", hiscore_level_index_);
	const int offset = std::max(0, my_hiscore_index_-5);
	if (!hiscore_agent_->StartDownloadingList(kPlatform, level_name, kVehicleName, offset, 10)) {
		delete hiscore_agent_;
		hiscore_agent_ = 0;
	}
}

void DownwashManager::UpdateHiscoreDialog() {
	uitbc::Dialog* d = menu_->GetDialog();
	// If dialog still open: show it. Otherwise just fuck it.
	if (!d || d->GetName() != "hiscore_dialog") {
		return;
	}

	uitbc::FixedLayouter layouter(d);

	str pilot_name;
	v_get(pilot_name, =, GetVariableScope(), kRtvarGamePilotname, kDefaultPilotName);
	typedef cure::HiscoreAgent::Entry HiscoreEntry;
	typedef cure::HiscoreAgent::List HiscoreList;
	const HiscoreList& hiscore_list = hiscore_agent_->GetDownloadedList();
	str hiscore;
	const int base_place = hiscore_list.offset_;
	const int max_entry_count = 10;
	const int score_count = (int)hiscore_list.entry_list_.size();
	const wstr level_title = GetHiscoreLevelTitle();
	uitbc::Label* header = new uitbc::Label(WHITE, level_title);
	header->SetName("level_header");
	header->SetIcon(uitbc::Painter::kInvalidImageid, uitbc::Label::kIconCenter);
	layouter.AddComponent(header, 0, 2+max_entry_count, 0, 1, 1);
	header->SetAdaptive(false);
	for (int x = 0; x < score_count; ++x) {
		d->SetQueryLabel(0);
		const int place = x + 1 + base_place;
		const HiscoreEntry& entry = hiscore_list.entry_list_[x];
		const double _time = -entry.score_/1000.0;
		if (place == 1) {
			SetLevelBestTime(hiscore_level_index_, true, _time);
		}
		wstr time_str;
		wstrutil::DoubleToString(_time, 3, time_str);
		const Color color = (pilot_name == entry.name_)? YELLOW : WHITE;
		uitbc::Label* place_label = new uitbc::Label(color, wstrutil::Format(L"%i.", place));
		place_label->SetIcon(uitbc::Painter::kInvalidImageid, uitbc::Label::kIconLeft);
		uitbc::Label* name_label = new uitbc::Label(color, wstrutil::Encode(entry.name_));
		uitbc::Label* time_label = new uitbc::Label(color, time_str);
		time_label->SetIcon(uitbc::Painter::kInvalidImageid, uitbc::Label::kIconLeft);
		layouter.AddComponent(place_label, 2+x, 2+max_entry_count, 0, 1, 7);
		layouter.AddComponent(name_label,  2+x, 2+max_entry_count, 1, 4, 7);
		layouter.AddComponent(time_label,  2+x, 2+max_entry_count, 5, 2, 7);
		place_label->SetAdaptive(false);
		name_label->SetAdaptive(false);
		time_label->SetAdaptive(false);
	}
	if (!score_count) {
		d->UpdateQueryLabel(L"No high score entered. Yet.", WHITE);
	}
}

void DownwashManager::OnMenuAlternative(uitbc::Button* button) {
	// Always save pilot name.
	uitbc::TextField* pilot_name_field = (uitbc::TextField*)menu_->GetDialog()->GetChild("pilot_name");
	if (pilot_name_field) {
		v_set(GetVariableScope(), kRtvarGamePilotname, strutil::Encode(pilot_name_field->GetText()));
	}

	float pre_childishness;
	v_get(pre_childishness, =(float), GetVariableScope(), kRtvarGameChildishness, 1.0);
	if (button->GetTag() == -2) {
		v_set(GetVariableScope(), kRtvarGameChildishness, 1.0);
	} else if (button->GetTag() == -3) {
		v_set(GetVariableScope(), kRtvarGameChildishness, 0.5);
		if (pre_childishness > 0.75f) fly_time_.ReduceTimeDiff(-fly_time_.GetTimeDiff());	// Penalty for changing from easy mode.
	} else if (button->GetTag() == -4) {
		v_set(GetVariableScope(), kRtvarGameChildishness, 0.0);
		if (pre_childishness > 0.75f) fly_time_.ReduceTimeDiff(-fly_time_.GetTimeDiff());	// Penalty for changing from easy mode.
		if (pre_childishness > 0.25f) fly_time_.ReduceTimeDiff(-fly_time_.GetTimeDiff());	// Penalty for changing from medium mode.
	} else if (button->GetTag() == -5) {
		const bool toy_mode = (button->GetState() == uitbc::Button::kPressed);
		v_set(GetVariableScope(), kRtvarPhysicsRtr, toy_mode? 2.0 : 1.0);
		v_set(GetVariableScope(), kRtvarPhysicsRtrOffset, toy_mode? 1.0 : 0.0);
	} else if (button->GetTag() == -6) {
		double bedside_volume = (button->GetState() == uitbc::Button::kPressed)? 0.02 : 1.0;
		v_set(GetVariableScope(), kRtvarUiSoundMastervolume, bedside_volume);
		ui_manager_->GetSoundManager()->SetMasterVolume((float)bedside_volume);	// Set right away for button volume.
	} else if (button->GetTag() == -7) {
		hiscore_level_index_ = GetCurrentLevelNumber();
		my_hiscore_index_ = -1;
		ShowHiscoreDialog(+1);
	} else if (button->GetTag() == -8) {
		pause_button_->SetVisible(true);
		GetConsoleManager()->PushYieldCommand("set-level-index 0");
		menu_->DismissDialog();
		HiResTimer::StepCounterShadow();
		v_set(GetVariableScope(), kRtvarPhysicsHalt, false);
		set_random_chopper_color_ = false;
	} else if (button->GetTag() == -9) {
		pause_button_->SetVisible(true);
		HiResTimer::StepCounterShadow();
		hit_ground_frame_count_ = 2;
		v_set(GetVariableScope(), kRtvarPhysicsHalt, false);
	} else if (button->GetTag() == -100) {
		int level_count;
		v_get(level_count, =, GetVariableScope(), kRtvarGameLevelcount, 14);
		hiscore_level_index_ = (hiscore_level_index_-1 < 0)? level_count-1 : hiscore_level_index_-1;
		ShowHiscoreDialog(-1);
	} else if (button->GetTag() == -101) {
		int level_count;
		v_get(level_count, =, GetVariableScope(), kRtvarGameLevelcount, 14);
		hiscore_level_index_ = (hiscore_level_index_+1 >= level_count)? 0 : hiscore_level_index_+1;
		ShowHiscoreDialog(+1);
	}
}

void DownwashManager::OnPreHiscoreAction(uitbc::Button* button) {
	switch (button->GetTag()) {
		case -100: {
			menu_->GetDialog()->SetDirection(-1, false);
		} break;
		default: {
			menu_->GetDialog()->SetDirection(+1, false);
		} break;
	}
}


void DownwashManager::UpdateHiscoreDialogTitle() {
	uitbc::Dialog* d = menu_->GetDialog();
	if (d) {
		uitbc::Label* header = (uitbc::Label*)d->GetChild("level_header");
		if (header) {
			header->SetText(GetHiscoreLevelTitle());
		}
	}
}

wstr DownwashManager::GetHiscoreLevelTitle() const {
	wstr hiscore_level_info;
	if (GetCurrentLevelNumber() == hiscore_level_index_ || hiscore_level_index_ == -1) {
		hiscore_level_info = wstrutil::Format(L"Current level (%i) high score", hiscore_level_index_+1);
	} else if (GetCurrentLevelNumber()-1 == hiscore_level_index_) {
		hiscore_level_info = wstrutil::Format(L"Previous level (%i) high score", hiscore_level_index_+1);
	} else {
		hiscore_level_info = wstrutil::Format(L"Level %i high score", hiscore_level_index_+1);
	}
	return hiscore_level_info;
}

void DownwashManager::CreateHiscoreAgent() {
	delete hiscore_agent_;
	const str host = _O("7y=196h5+;/,9p.5&92r:/;*(,509p;/1", "gamehiscore.pixeldoctrine.com");
	hiscore_agent_ = new cure::HiscoreAgent(host, 80, "downwash");
	//hiscore_agent_ = new cure::HiscoreAgent("localhost", 8080, "downwash");
}

void DownwashManager::TickHiscore() {
	// Download any pending hiscore request.
	if (!hiscore_agent_) {
		return;
	}
	cure::ResourceLoadState load_state = hiscore_agent_->Poll();
	if (load_state != cure::kResourceLoadComplete) {
		if (load_state == cure::kResourceLoadError) {
			delete hiscore_agent_;
			hiscore_agent_ = 0;
			uitbc::Dialog* d = menu_->GetDialog();
			// If dialog still open: show it. Otherwise just fuck it.
			if (d && d->GetName() == "hiscore_dialog") {
				d->UpdateQueryLabel(L"Network problem.", RED);
			}
		}
		return;
	}

	switch (hiscore_agent_->GetAction()) {
		case cure::HiscoreAgent::kActionDownloadList: {
			my_hiscore_index_ = -1;
			UpdateHiscoreDialog();
			delete hiscore_agent_;
			hiscore_agent_ = 0;
		} break;
		case cure::HiscoreAgent::kActionUploadScore: {
			my_hiscore_index_ = hiscore_agent_->GetUploadedPlace();
			delete hiscore_agent_;
			hiscore_agent_ = 0;
			hiscore_just_uploaded_timer_.Start();
		} break;
	}
}



void DownwashManager::DrawStick(Touchstick* stick) {
	cure::ContextObject* _avatar = GetContext()->GetObject(avatar_id_);
	if (!stick || !_avatar) {
		return;
	}

	const PixelRect area = stick->GetArea();
	const int board_radius = area.GetWidth()/2;
	ui_manager_->GetPainter()->SetColor(Color(15, 10, 5));
	ui_manager_->GetPainter()->SetAlphaValue(192);
	ui_manager_->GetPainter()->DrawArc(area.GetCenterX(), area.GetCenterY(), board_radius*2, board_radius*2, 0, 360, true);
	ui_manager_->GetPainter()->SetAlphaValue(255);

	const int ow = area.GetWidth();
	const int r = (int)(ow*0.27f);	// Knob radius.
	const int margin = (int)(r*0.2f);
	float x, y;
	bool is_pressing = false;
	stick->GetValue(x, y, is_pressing);
	if (!is_pressing) {
		y = 0;
	}
	vec2 v(x, y);
	//v.Mul(2.0f * (ow - stick->GetFingerRadius()-margin*2) / ow);
	const float length = v.GetLength();
	if (length > 1) {
		v.Div(length);
	}
	x = v.x;
	y = v.y;
	x = 0.5f*x + 0.5f;
	y = 0.5f*y + 0.5f;
	const int w = area.GetWidth()  - r*2 - margin*2;
	const int h = area.GetHeight() - r*2 - margin*2;
	ui_manager_->GetPainter()->SetColor(is_pressing? Color(0xD0, 0x50, 0x40) : Color(0xC0, 0x30, 0x20));
	ui_manager_->GetPainter()->DrawArc(
		area.left_ + margin + (int)(w*x),
		area.top_  + margin + (int)(h*y),
		r*2, r*2, 0, 360, true);
}



void DownwashManager::ScriptPhysicsTick() {
	if (options_.IsEscape() && !menu_->GetDialog()) {
		OnPauseButton(pause_button_);
	}
	// Camera moves in a "moving average" kinda curve (halfs the distance in x seconds).
	const float physics_time = GetTimeManager()->GetAffordedPhysicsTotalTime();
	if (physics_time > 1e-5) {
		MoveCamera();
		UpdateCameraPosition(false);
	}

	if (all_loaded_timer_.IsStarted()) {
		if (all_loaded_timer_.QueryTimeDiff() > 5.0) {
			all_loaded_timer_.Stop();
			strutil::strvec resource_types;
			resource_types.push_back("RenderImg");
			resource_types.push_back("Geometry");
			resource_types.push_back("GeometryRef");
			resource_types.push_back("Physics");
			resource_types.push_back("PhysicsShared");
			resource_types.push_back("RamImg");
			resource_types.push_back("Sound3D");
			resource_types.push_back("Sound2D");
			GetResourceManager()->ForceFreeCache(resource_types);
			GetResourceManager()->ForceFreeCache(resource_types);	// Call again to release any dependent resources.
		}
	}

	if (slowmo_timer_.IsStarted()) {
		double rtr_offset;
		v_get(rtr_offset, =, GetVariableScope(), kRtvarPhysicsRtrOffset, 0.0);
		if (slowmo_timer_.QueryTimeDiff() < 3.5f) {
			v_set(GetVariableScope(), kRtvarPhysicsRtr, 0.3*(rtr_offset+1));
		} else {
			v_set(GetVariableScope(), kRtvarPhysicsRtr, 1.0+rtr_offset);
			slowmo_timer_.Stop();
		}
	}

	if (avatar_create_timer_.IsStarted()) {
		if (avatar_create_timer_.QueryTimeDiff() > 0.5) {
			avatar_create_timer_.Stop();
		}
	}
	cure::ContextObject* _avatar = GetContext()->GetObject(avatar_id_, true);
	if (lepra::GameTimer::GetRealTimeRatio() < 0.01f) {
		return;	// Wait until game runs again before resetting chopper.
	}
	if (avatar_create_timer_.IsStarted() || _avatar) {
		avatar_died_.Stop();
	} else if (avatar_id_) {
		avatar_died_.TryStart();
		if (avatar_died_.QueryTimeDiff() > 0.1f) {
			// If we're currently loading level, we'll load a chopper when level is complete.
			// Otherwise we just load one right away.
			if (old_level_) {
				avatar_id_ = 0;
			} else {
				CreateChopper(kVehicleName);
			}
		}
	}

	Parent::ScriptPhysicsTick();
}

void DownwashManager::HandleWorldBoundaries() {
	cure::ContextObject* _avatar = GetContext()->GetObject(avatar_id_);
	if (_avatar && !too_far_away_timer_.IsStarted() && _avatar->GetPosition().GetLength() > 300) {
		too_far_away_timer_.Start();
		life::HomingProjectile* rocket = (life::HomingProjectile*)Parent::CreateContextObject("missile", cure::kNetworkObjectLocalOnly);
		rocket->SetTarget(avatar_id_);
		vec3 fire_position(_avatar->GetPosition().x, 0, 0);
		fire_position.Normalize(200);
		fire_position += _avatar->GetPosition();
		fire_position.z += 50;
		rocket->SetInitialTransform(xform(quat(), fire_position));
		rocket->StartLoading();
	} else if (too_far_away_timer_.IsStarted() && too_far_away_timer_.QueryTimeDiff() > 25.0) {
		too_far_away_timer_.Stop();
	}

	std::vector<cure::GameObjectId> lost_object_array;
	typedef cure::ContextManager::ContextObjectTable ContextTable;
	const ContextTable& object_table = GetContext()->GetObjectTable();
	ContextTable::const_iterator x = object_table.begin();
	for (; x != object_table.end(); ++x) {
		cure::ContextObject* _object = x->second;
		if (_object->IsLoaded() && _object->GetPhysics()) {
			const vec3 _position = _object->GetPosition();
			if (!Math::IsInRange(_position.x, -1000.0f, +1000.0f) ||
				!Math::IsInRange(_position.y, -1000.0f, +1000.0f) ||
				!Math::IsInRange(_position.z, -1000.0f, +1000.0f)) {
				lost_object_array.push_back(_object->GetInstanceId());
			}
		}
	}
	if (!lost_object_array.empty()) {
		ScopeLock lock(GetTickLock());
		std::vector<cure::GameObjectId>::const_iterator y = lost_object_array.begin();
		for (; y != lost_object_array.end(); ++y) {
			DeleteContextObject(*y);
		}
	}
}

void DownwashManager::MoveCamera() {
	UpdateChopperColor(0.05f);
	UpdateCameraDistance();

	float half_cam_distance;
	v_get(half_cam_distance, =(float), GetVariableScope(), kRtvarUi3DCamdistance, 110.0);
	half_cam_distance /= 2;
	camera_previous_position_ = camera_transform_.GetPosition();
	cure::ContextObject* _avatar = GetContext()->GetObject(avatar_id_);
	vec3 avatar_position = helicopter_position_;
	float speed_x = 0;
	float ax;
	v_get(ax, =(float), GetVariableScope(), kRtvarUi3DCamxangle, 0.0);
	if (_avatar && _avatar->GetPhysics()->GetEngineCount() >= 3) {
		avatar_position = _avatar->GetPosition();
		float ox, oy, oz;
		v_get(ox, =(float), GetVariableScope(), kRtvarUi3DCamxoffset, 0.0);
		v_get(oy, =(float), GetVariableScope(), kRtvarUi3DCamyoffset, 0.0);
		v_get(oz, =(float), GetVariableScope(), kRtvarUi3DCamzoffset, 0.0);
		avatar_position.x += ox;
		avatar_position.y += oy;
		avatar_position.z += oz;
		helicopter_position_ = avatar_position;
		speed_x = _avatar->GetVelocity().x;
		if (cure::Health::Get(_avatar) <= 0) {
			set_random_chopper_color_ = true;
		}
	}
	xform target_transform(quat(), avatar_position + vec3(0, -2*half_cam_distance, 0));
	++post_zoom_platform_frame_count_;
	if (zoom_platform_) {
		cure::ContextObject* _level = old_level_? old_level_ : level_;
		target_transform.GetPosition() = GetLandingTriggerPosition(_level) + vec3(0, 0, 10);
		post_zoom_platform_frame_count_ = 0;
	} else if (hit_ground_frame_count_ >= -kStillFramesUntilCamPans) {
		target_transform.GetPosition().z += half_cam_distance;
	}

	vec3 cam_xz_pos(camera_transform_.GetPosition());
	cam_xz_pos.y = 0;
	if (cam_xz_pos.GetDistance(avatar_position) > 110.0) {
		camera_transform_ = target_transform;
		camera_speed_ = 0;
	} else {
		vec3 delta = Math::Lerp(camera_transform_.GetPosition(), target_transform.GetPosition(), 0.08f) - camera_transform_.GetPosition();
		camera_transform_.GetPosition().x += delta.x;
		//camera_transform_.GetPosition().z += delta.z;
		camera_transform_.GetPosition().z += Math::SmoothClamp(delta.z, -2.0f, +2.0f, 0.2f);
		camera_transform_.GetPosition().y += Math::SmoothClamp(delta.y, -1.3f, +2.0f, 0.2f);
	}

	// Angle.
	const float x = avatar_position.y - camera_transform_.GetPosition().y;
	const float y = avatar_position.z - camera_transform_.GetPosition().z;
	const float x_angle = ::atan2(y, x) + ax;
	target_transform.GetOrientation().RotateAroundOwnX(x_angle);
	if (post_zoom_platform_frame_count_ > 10) {
		camera_speed_ = Math::Lerp(camera_speed_, speed_x*0.4f, 0.05f);
		//camera_speed_ = Math::Clamp(camera_speed_, -3.0f, +4.0f);
		const float z = avatar_position.x + camera_speed_ - camera_transform_.GetPosition().x;
		const int smooth_steps = post_zoom_platform_frame_count_-10;
		const float smooth_factor = (smooth_steps >= 100)? 1.0f : smooth_steps/100.0f;
		const float z_angle = -::atan2(z, x) * smooth_factor;
		target_transform.GetOrientation().RotateAroundWorldZ(z_angle);
	}
	camera_transform_.GetOrientation().Slerp(camera_transform_.GetOrientation(), target_transform.GetOrientation(), 0.5f);
	/*float fo_v = Math::Lerp(30.0f, 60.0f, camera_speed_);
	fo_v = Math::SmoothClamp(fo_v, 30.0f, 60.0f, 0.4f);
	v_set(GetVariableScope(), kRtvarUi3DFov, fo_v);*/

	/*v_arithmetic(GetVariableScope(), "cam_ang", double, +, 0.01, 0.0, 3000.0);
	quat q;
	camera_transform_ = xform(quat(), vec3(0, -300, 50));
	camera_transform_.RotateAroundAnchor(vec3(), vec3(1,0,1), (float)v_slowtryget(GetVariableScope(), "cam_ang", 0.0));
	v_set(GetVariableScope(), kRtvarUi3DFov, 45.0);*/

	avatar_position.y -= 5.0f;	// Move closer to edge to avoid jitter.
	UpdateMassObjects(avatar_position);
}

void DownwashManager::UpdateCameraPosition(bool update_mic_position) {
	ui_manager_->SetCameraPosition(camera_transform_);
	if (update_mic_position) {
		cure::ContextObject* _avatar = GetContext()->GetObject(avatar_id_);
		if (_avatar && _avatar->IsLoaded()) {
			xform mic_pos(quat(), helicopter_position_);
			mic_pos.GetPosition().z -= 2;
			ui_manager_->SetMicrophonePosition(mic_pos, microphone_speed_);
		}
	}
}



void DownwashManager::DrawImage(uitbc::Painter::ImageID image_id, float cx, float cy, float w, float h, float angle) const {
	cx -= 0.5f;

	const float ca = ::cos(angle);
	const float sa = ::sin(angle);
	const float w2 = w*0.5f;
	const float h2 = h*0.5f;
	const float x = cx - w2*ca - h2*sa;
	const float y = cy - h2*ca + w2*sa;
	const vec2 c[] = { vec2(x, y), vec2(x+w*ca, y-w*sa), vec2(x+w*ca+h*sa, y+h*ca-w*sa), vec2(x+h*sa, y+h*ca) };
	const vec2 t[] = { vec2(0, 0), vec2(1, 0), vec2(1, 1), vec2(0, 1) };
#define V(z) std::vector<vec2>(z, z+LEPRA_ARRAY_COUNT(z))
	ui_manager_->GetPainter()->DrawImageFan(image_id, V(c), V(t));
}



void DownwashManager::PainterImageLoadCallback(UiCure::UserPainterKeepImageResource* resource) {
	if (resource->GetLoadState() == cure::kResourceLoadComplete) {
		ui_manager_->GetDesktopWindow()->GetImageManager()->AddLoadedImage(*resource->GetRamData(), resource->GetData(),
			uitbc::GUIImageManager::kCentered, uitbc::GUIImageManager::kAlphablend, 255);
	}
}

void DownwashManager::RendererTextureLoadCallback(UiCure::UserRendererImageResource* resource) {
	ui_manager_->GetRenderer()->TryAddGeometryTexture(arrow_billboard_id_, resource->GetData());
}



bool DownwashManager::DisableDepth() {
	ui_manager_->GetRenderer()->EnableAllLights(false);
	ui_manager_->GetRenderer()->SetDepthWriteEnabled(false);
	ui_manager_->GetRenderer()->SetDepthTestEnabled(false);
	return true;
}

void DownwashManager::EnableDepth() {
	ui_manager_->GetRenderer()->EnableAllLights(true);
	ui_manager_->GetRenderer()->SetDepthWriteEnabled(true);
	ui_manager_->GetRenderer()->SetDepthTestEnabled(true);
}



loginstance(kGame, DownwashManager);



}
