
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "hovertankmanager.h"
#include <algorithm>
#include "../cure/include/contextmanager.h"
#include "../cure/include/health.h"
#include "../cure/include/floatattribute.h"
#include "../cure/include/intattribute.h"
#include "../cure/include/networkclient.h"
#include "../cure/include/timemanager.h"
#include "../lepra/include/random.h"
#include "../lepra/include/time.h"
#include "../life/lifeclient/clientoptions.h"
#include "../life/lifeclient/clientoptions.h"
#include "../life/lifeclient/explodingmachine.h"
#include "../life/lifeclient/fastprojectile.h"
#include "../life/lifeclient/level.h"
#include "../life/lifeclient/massobject.h"
#include "../life/lifeclient/mine.h"
#include "../life/lifeclient/projectile.h"
#include "../life/lifeclient/uiconsole.h"
#include "../life/explosion.h"
#include "../life/projectileutil.h"
#include "../uicure/include/uicollisionsoundmanager.h"
#include "../uicure/include/uiexhaustemitter.h"
#include "../uicure/include/uijetengineemitter.h"
#include "../uicure/include/uigameuimanager.h"
#include "../uicure/include/uigravelemitter.h"
#include "../uicure/include/uiiconbutton.h"
#include "../uicure/include/uiprops.h"
#include "../uilepra/include/uitouchstick.h"
#include "../uitbc/include/gui/uidesktopwindow.h"
#include "../uitbc/include/gui/uifloatinglayout.h"
#include "../uitbc/include/uiparticlerenderer.h"
#include "hovertankconsolemanager.h"
#include "hovertankticker.h"
#include "roadsignbutton.h"
#include "rtvar.h"
#include "sunlight.h"
#include "version.h"

#define ICONBTN(i,n)			new UiCure::IconButton(ui_manager_, GetResourceManager(), i, n)
#define ICONBTNA(i,n)			ICONBTN(i, n)



namespace HoverTank {



namespace {

struct Score {
	str name_;
	int kills_;
	int deaths_;
	int ping_;
};

struct DeathsAscendingOrder {
	bool operator() (const Score& a, const Score& b) { return a.deaths_ < b.deaths_; }
}
g_deaths_ascending_order;

struct KillsDecendingOrder {
	bool operator() (const Score& a, const Score& b) { return a.kills_ > b.kills_; }
}
g_kills_decending_order;

}



HoverTankManager::HoverTankManager(life::GameClientMasterTicker* pMaster, const cure::TimeManager* time,
	cure::RuntimeVariableScope* variable_scope, cure::ResourceManager* resource_manager,
	UiCure::GameUiManager* ui_manager, int slave_index, const PixelRect& render_area):
	Parent(pMaster, time, variable_scope, resource_manager, ui_manager, slave_index, render_area),
	collision_sound_manager_(0),
	avatar_id_(0),
	had_avatar_(false),
	update_camera_for_avatar_(false),
	cam_rotate_extra_(0),
	active_weapon_(0),
	pick_vehicle_button_(0),
	avatar_invisible_count_(0),
	road_sign_index_(0),
	level_id_(0),
	level_(0),
	sun_(0),
	score_info_id_(0),
	camera_position_(0, -200, 100),
	camera_up_(0, 0, 1),
	camera_orientation_(PIF/2, acos(camera_position_.z/camera_position_.y), 0),
	camera_target_xy_distance_(20),
	camera_max_speed_(500),
	camera_mouse_angle_(0),
	camera_target_angle_(0),
	camera_target_angle_factor_(0),
	login_window_(0),
#if defined(LEPRA_TOUCH) || defined(EMULATE_TOUCH)
	fire_button_(0),
#endif // touch or emulated touch.
	stick_left_(0),
	stick_right_(0),
	engine_playback_time_(0) {
	collision_sound_manager_ = new UiCure::CollisionSoundManager(this, ui_manager);
	collision_sound_manager_->AddSound("explosion",	UiCure::CollisionSoundManager::SoundResourceInfo(0.8f, 0.4f, 0));
	collision_sound_manager_->AddSound("small_metal",	UiCure::CollisionSoundManager::SoundResourceInfo(0.2f, 0.4f, 0));
	collision_sound_manager_->AddSound("big_metal",	UiCure::CollisionSoundManager::SoundResourceInfo(1.5f, 0.4f, 0));
	collision_sound_manager_->AddSound("plastic",		UiCure::CollisionSoundManager::SoundResourceInfo(1.0f, 0.4f, 0));
	collision_sound_manager_->AddSound("rubber",		UiCure::CollisionSoundManager::SoundResourceInfo(1.0f, 0.5f, 0));
	collision_sound_manager_->AddSound("wood",		UiCure::CollisionSoundManager::SoundResourceInfo(1.0f, 0.5f, 0));

	::memset(engine_power_shadow_, 0, sizeof(engine_power_shadow_));

	camera_pivot_position_ = camera_position_ + GetCameraQuaternion() * vec3(0, camera_target_xy_distance_*3, 0);

	SetConsoleManager(new HoverTankConsoleManager(GetResourceManager(), this, ui_manager_, GetVariableScope(), render_area_));
}

HoverTankManager::~HoverTankManager() {
	Close();

	delete stick_left_;
	stick_left_ = 0;
	delete stick_right_;
	stick_right_ = 0;
}

void HoverTankManager::LoadSettings() {
	v_internal(GetVariableScope(), kRtvarGameDrawscore, false);

	Parent::LoadSettings();

	v_internal(GetVariableScope(), kRtvarUi3DCamdistance, 20.0);
	v_internal(GetVariableScope(), kRtvarUi3DCamheight, 10.0);
	v_internal(GetVariableScope(), kRtvarUi3DCamrotate, 0.0);
	v_internal(GetVariableScope(), kRtvarSteeringPlaybackmode, kPlaybackNone);

#if defined(LEPRA_TOUCH) || defined(EMULATE_TOUCH)
	const str left_name  = strutil::Format("TouchstickLeft%i", slave_index_);
	const str right_name = strutil::Format("TouchstickRight%i", slave_index_);
	v_override(GetVariableScope(), kRtvarCtrlSteerFwd, left_name+".AxisY-");
	v_override(GetVariableScope(), kRtvarCtrlSteerBrkback, left_name+".AxisY+");
	v_override(GetVariableScope(), kRtvarCtrlSteerLeft, left_name+".AxisX-");
	v_override(GetVariableScope(), kRtvarCtrlSteerRight, left_name+".AxisX+");
	v_override(GetVariableScope(), kRtvarCtrlSteerFwd3D, right_name+".AxisY-");
	v_override(GetVariableScope(), kRtvarCtrlSteerBack3D, right_name+".AxisY+");
	v_override(GetVariableScope(), kRtvarCtrlSteerLeft3D, right_name+".AxisX-");
	v_override(GetVariableScope(), kRtvarCtrlSteerRight3D, right_name+".AxisX+");
	v_override(GetVariableScope(), kRtvarCtrlSteerUp3D, left_name+".AxisY-");
	v_override(GetVariableScope(), kRtvarCtrlSteerDown3D, left_name+".AxisY+");
	v_override(GetVariableScope(), kRtvarCtrlSteerUp, right_name+".AxisY-");
	v_override(GetVariableScope(), kRtvarCtrlSteerDown, right_name+".AxisY+");
#endif // touch device or emulated touch device

#ifdef LEPRA_TOUCH
	// TODO: remove hard-coding!
	//v_set(GetVariableScope(), kRtvarNetworkServeraddress, "pixeldoctrine.dyndns.org:16650");
	//v_internal(UiCure::GetSettings(), kRtvarLoginIsserverselected, true);
	v_set(GetVariableScope(), kRtvarNetworkServeraddress, "localhost:16650");
#endif // touch device
}

void HoverTankManager::SaveSettings() {
#ifndef EMULATE_TOUCH
	GetConsoleManager()->ExecuteCommand("save-application-config-file "+GetApplicationCommandFilename());
#endif // Computer or touch device.
}

void HoverTankManager::SetRenderArea(const PixelRect& render_area) {
	Parent::SetRenderArea(render_area);
	if (login_window_) {
		login_window_->SetPos(render_area_.GetCenterX()-login_window_->GetSize().x/2,
			render_area_.GetCenterY()-login_window_->GetSize().y/2);
	}

	UpdateTouchstickPlacement();

	v_get(camera_target_xy_distance_, =(float), GetVariableScope(), kRtvarUi3DCamdistance, 20.0);
}

bool HoverTankManager::Open() {
	bool _ok = Parent::Open();
#if defined(LEPRA_TOUCH) || defined(EMULATE_TOUCH)
	if (_ok) {
		fire_button_ = ICONBTNA("grenade.png", "");
		int x = render_area_.GetCenterX() - 32;
		int y = render_area_.bottom_ - 76;
		ui_manager_->GetDesktopWindow()->AddChild(fire_button_, x, y);
		fire_button_->SetVisible(true);
		fire_button_->SetOnClick(HoverTankManager, OnFireButton);
	}
#endif // touch or emulated touch.
	return _ok;
}

void HoverTankManager::Close() {
	ScopeLock lock(GetTickLock());
	ClearRoadSigns();
#if defined(LEPRA_TOUCH) || defined(EMULATE_TOUCH)
	delete fire_button_;
	fire_button_ = 0;
#endif // touch or emulated touch.
	Parent::Close();
	CloseLoginGui();
}

void HoverTankManager::SetIsQuitting() {
	CloseLoginGui();
	((HoverTankConsoleManager*)GetConsoleManager())->GetUiConsole()->SetVisible(false);
	SetRoadSignsVisible(false);

	Parent::SetIsQuitting();
}

void HoverTankManager::SetFade(float fade_amount) {
	camera_max_speed_ = 100000.0f;
	float base_distance;
	v_get(base_distance, =(float), GetVariableScope(), kRtvarUi3DCamdistance, 20.0);
	camera_target_xy_distance_ = base_distance + fade_amount*400.0f;
}



bool HoverTankManager::Paint() {
	if (stick_left_) {
		DrawStick(stick_left_);
		DrawStick(stick_right_);
		stick_left_->ResetTap();
		stick_right_->ResetTap();
	}

	const cure::ContextObject* _object = GetContext()->GetObject(avatar_id_);
	if (_object) {
		cure::FloatAttribute* health = cure::Health::GetAttribute(_object);
		const str info = health? strutil::DoubleToString(health->GetValue()*100, 0) : "";
		ui_manager_->GetPainter()->SetColor(Color(255, 0, 0, 255), 0);
		ui_manager_->GetPainter()->SetColor(Color(0, 0, 0, 0), 1);
		ui_manager_->GetPainter()->PrintText(info, render_area_.left_ + 10, 10);
	}

	bool draw_score;
	v_get(draw_score, =, GetVariableScope(), kRtvarGameDrawscore, false);
	if (draw_score) {
		DrawScore();
	}
	return true;
}



void HoverTankManager::RequestLogin(const str& server_address, const cure::LoginId& login_token) {
	ScopeLock lock(GetTickLock());
	CloseLoginGui();
	Parent::RequestLogin(server_address, login_token);
}

void HoverTankManager::OnLoginSuccess() {
	ClearRoadSigns();
}



void HoverTankManager::SelectAvatar(const cure::UserAccount::AvatarId& avatar_id) {
	DropAvatar();

	log_volatile(log_.Debugf("Clicked avatar %s.", avatar_id.c_str()));
	cure::Packet* packet = GetNetworkAgent()->GetPacketFactory()->Allocate();
	GetNetworkAgent()->SendStatusMessage(GetNetworkClient()->GetSocket(), 0, cure::kRemoteOk,
		cure::MessageStatus::kInfoAvatar, wstrutil::Encode(avatar_id), packet);
	GetNetworkAgent()->GetPacketFactory()->Release(packet);

	SetRoadSignsVisible(false);
}

void HoverTankManager::AddLocalObjects(std::unordered_set<cure::GameObjectId>& local_object_set) {
	if (avatar_id_) {
		cure::ContextObject* _avatar = GetContext()->GetObject(avatar_id_);
		if (had_avatar_ && !_avatar) {
			DropAvatar();
		} else if (_avatar) {
			had_avatar_ = true;
		}
	}

	Parent::AddLocalObjects(local_object_set);
}

bool HoverTankManager::IsObjectRelevant(const vec3& position, float distance) const {
	return (position.GetDistanceSquared(camera_position_) <= distance*distance);
}

cure::GameObjectId HoverTankManager::GetAvatarInstanceId() const {
	return avatar_id_;
}



bool HoverTankManager::SetAvatarEnginePower(unsigned aspect, float power) {
	deb_assert(aspect >= 0 && aspect < tbc::PhysicsEngine::kAspectCount);
	cure::ContextObject* _object = GetContext()->GetObject(avatar_id_);
	if (_object) {
		return SetAvatarEnginePower(_object, aspect, power);
	}
	return false;
}



void HoverTankManager::Detonate(cure::ContextObject* explosive, const tbc::ChunkyBoneGeometry* explosive_geometry, const vec3& position, const vec3& velocity, const vec3& normal, float strength) {
	(void)explosive;

	collision_sound_manager_->OnCollision(5.0f * strength, position, explosive_geometry, "explosion");

	uitbc::ParticleRenderer* particle_renderer = (uitbc::ParticleRenderer*)ui_manager_->GetRenderer()->GetDynamicRenderer("particle");
	//mLog.Infof("Hit object normal is (%.1f; %.1f; %.1f"), pNormal.x, pNormal.y, pNormal.z);
	const float keep_on_going_factor = 0.5f;	// How much of the velocity energy, [0;1], should be transferred to the explosion particles.
	vec3 u = velocity.ProjectOntoPlane(normal) * (1+keep_on_going_factor);
	u -= velocity;	// Mirror and inverse.
	u.Normalize();
	const int particles = Math::Lerp(4, 10, strength * 0.2f);
	vec3 start_fire_color(1.0f, 1.0f, 0.3f);
	vec3 fire_color(0.6f, 0.4f, 0.2f);
	vec3 start_smoke_color(0.4f, 0.4f, 0.4f);
	vec3 smoke_color(0.2f, 0.2f, 0.2f);
	vec3 shrapnel_color(0.3f, 0.3f, 0.3f);	// Default debris color is gray.
	if (dynamic_cast<life::Mine*>(explosive)) {
		start_fire_color.Set(0.9f, 1.0f, 0.8f);
		fire_color.Set(0.3f, 0.7f, 0.2f);
		start_smoke_color.Set(0.3f, 0.35f, 0.3f);
		smoke_color.Set(0.2f, 0.4f, 0.2f);
		shrapnel_color.Set(0.5f, 0.5f, 0.1f);
	}
	particle_renderer->CreateExplosion(position, strength * 1.5f, u, 1, 1, start_fire_color, fire_color, start_smoke_color, smoke_color, shrapnel_color, particles*2, particles*2, particles, particles/2);

	/*if (!GetMaster()->IsLocalServer()) {	// If local server, it will already have given us a hover_tank.
		const cure::ContextObject* _object = GetContext()->GetObject(avatar_id_);
		if (_object) {
			Explosion::HoverTankObject(GetPhysicsManager(), _object, position, 1.0f);
		}
	}*/
}

void HoverTankManager::OnBulletHit(cure::ContextObject* bullet, cure::ContextObject* hit_object) {
	(void)hit_object;

	tbc::ChunkyPhysics* physics = bullet->GetPhysics();
	if (physics) {
		tbc::ChunkyBoneGeometry* geometry = physics->GetBoneGeometry(0);
		collision_sound_manager_->OnCollision(5.0f, bullet->GetPosition(), geometry, geometry->GetMaterial());
	}
}

cure::RuntimeVariableScope* HoverTankManager::GetVariableScope() const {
	return (Parent::GetVariableScope());
}



bool HoverTankManager::Reset() {	// Run when disconnected. Removes all objects and displays login GUI.
	ScopeLock lock(GetTickLock());
	score_info_id_ = 0;
	ClearRoadSigns();
	bool _ok = Parent::Reset();
	if (_ok) {
		CreateLoginView();
	}
	return (_ok);
}

void HoverTankManager::CreateLoginView() {
	if (!login_window_ && !GetNetworkClient()->IsActive()) {
		// If first attempt (i.e. no connection problems) just skip interactivity.
		if (disconnect_reason_.empty()) {
			str server_name;
			v_tryget(server_name, =, cure::GetSettings(), kRtvarNetworkServeraddress, "localhost:16650");
			if (strutil::StartsWith(server_name, "0.0.0.0")) {
				server_name = server_name.substr(7);
			}
			const str default_user_name = strutil::Format("User%u", slave_index_);
			str user_name;
			v_tryget(user_name, =, GetVariableScope(), kRtvarLoginUsername, default_user_name);
                        wstr readable_password = L"CarPassword";
                        const cure::MangledPassword password(readable_password);
			const cure::LoginId _login_token(wstrutil::Encode(user_name), password);
			RequestLogin(server_name, _login_token);
		} else {
			login_window_ = new LoginView(this, disconnect_reason_);
			ui_manager_->AssertDesktopLayout(new uitbc::FloatingLayout, 0);
			ui_manager_->GetDesktopWindow()->AddChild(login_window_, 0, 0, 0);
			login_window_->SetPos(render_area_.GetCenterX()-login_window_->GetSize().x/2,
				render_area_.GetCenterY()-login_window_->GetSize().y/2);
			login_window_->GetChild("User", 0)->SetKeyboardFocus();
		}
	}
}

bool HoverTankManager::InitializeUniverse() {
	sun_ = 0;
	cloud_array_.clear();

	bool _ok = true;
	if (_ok) {
		sun_ = new UiCure::Props(GetResourceManager(), "sun", ui_manager_);
		AddContextObject(sun_, cure::kNetworkObjectLocalOnly, 0);
		_ok = (sun_ != 0);
		deb_assert(_ok);
		if (_ok) {
			sun_->StartLoading();
		}
	}
	const int prime_cloud_count = 11;	// TRICKY: must be prime or clouds start moving in sync.
	for (int x = 0; _ok && x < prime_cloud_count; ++x) {
		cure::ContextObject* cloud = new UiCure::Props(GetResourceManager(), "cloud_01", ui_manager_);
		AddContextObject(cloud, cure::kNetworkObjectLocalOnly, 0);
		cloud->StartLoading();
		cloud_array_.push_back(cloud);
	}
	mass_object_array_.clear();
	return (_ok);
}

void HoverTankManager::CloseLoginGui() {
	if (login_window_) {
		ScopeLock lock(GetTickLock());
		ui_manager_->GetDesktopWindow()->RemoveChild(login_window_, 0);
		delete (login_window_);
		login_window_ = 0;
	}
}

void HoverTankManager::ClearRoadSigns() {
	ScopeLock lock(GetTickLock());

	if (pick_vehicle_button_) {
		GetContext()->DeleteObject(pick_vehicle_button_->GetInstanceId());
		pick_vehicle_button_ = 0;
	}

	road_sign_index_ = 0;
	RoadSignMap::iterator x = road_sign_map_.begin();
	for (; x != road_sign_map_.end(); ++x) {
		GetContext()->DeleteObject(x->second->GetInstanceId());
	}
	road_sign_map_.clear();
}

void HoverTankManager::SetRoadSignsVisible(bool visible) {
	if (pick_vehicle_button_) {
		pick_vehicle_button_->SetIsMovingIn(!visible);
	}

	RoadSignMap::iterator x = road_sign_map_.begin();
	for (; x != road_sign_map_.end(); ++x) {
		x->second->SetIsMovingIn(visible);
	}

	ui_manager_->GetInputManager()->SetCursorVisible(visible);
}



void HoverTankManager::TickInput() {
	TickNetworkInput();
	TickUiInput();
}



void HoverTankManager::UpdateTouchstickPlacement() {
	if (touchstick_timer_.QueryTimeDiff() < 3.0) {
		return;
	}
	touchstick_timer_.ClearTimeDiff();

#if defined(LEPRA_TOUCH) || defined(EMULATE_TOUCH)
	const float touch_side_scale = 1.28f;	// Inches.
	const float touch_scale = touch_side_scale / (float)ui_manager_->GetDisplayManager()->GetPhysicalScreenSize();
	if (!stick_left_) {
		int screen_pixel_width;
		v_get(screen_pixel_width, =, GetVariableScope(), kRtvarUiDisplayWidth, 1024);
		const int minimum_touch_radius = (int)(screen_pixel_width*touch_scale*0.17f);	// 30 pixels in iPhone classic.
		stick_left_  = new Touchstick(ui_manager_->GetInputManager(), Touchstick::kModeRelativeCenter, PixelRect(0, 0, 10, 10),  0, minimum_touch_radius);
		const str left_name = strutil::Format("TouchstickLeft%i", slave_index_);
		stick_left_->SetUniqueIdentifier(left_name);
		stick_right_ = new Touchstick(ui_manager_->GetInputManager(), Touchstick::kModeRelativeCenter, PixelRect(0, 0, 10, 10), 0, minimum_touch_radius);
		const str right_name = strutil::Format("TouchstickRight%i", slave_index_);
		stick_right_->SetUniqueIdentifier(right_name);
	}
	int index = 0;
	int count = 0;
	GetMaster()->GetSlaveInfo(this, index, count);
	if (count == 2) {
		PixelRect left_stick_area(render_area_);
		PixelRect right_stick_area(render_area_);
		left_stick_area.bottom_ = render_area_.GetHeight() * touch_scale;
		right_stick_area.top_ = render_area_.GetHeight() * (1-touch_scale);
		left_stick_area.right_ = left_stick_area.left_ + left_stick_area.GetHeight();
		right_stick_area.right_ = left_stick_area.right_;

		if (index == 0) {
			stick_left_->Move(left_stick_area, -90);
			stick_right_->Move(right_stick_area, -90);
		} else {
			left_stick_area.left_ += render_area_.GetWidth() - left_stick_area.GetWidth();
			right_stick_area.left_ = left_stick_area.left_;
			left_stick_area.right_ = left_stick_area.left_ + left_stick_area.GetHeight();
			right_stick_area.right_ = left_stick_area.right_;
			std::swap(left_stick_area, right_stick_area);
			stick_left_->Move(left_stick_area, +90);
			stick_right_->Move(right_stick_area, +90);
		}
	} else {
		PixelRect left_stick_area(render_area_);
		PixelRect right_stick_area(render_area_);
		left_stick_area.right_ = render_area_.GetWidth() * touch_scale;
		right_stick_area.left_ = render_area_.GetWidth() * (1-touch_scale);
		left_stick_area.top_ = left_stick_area.bottom_ - (left_stick_area.right_ - left_stick_area.left_);
		right_stick_area.top_ = left_stick_area.top_;
		stick_left_->Move(left_stick_area, 0);
		stick_right_->Move(right_stick_area, 0);
	}
#endif // touch or emulated touch
}

void HoverTankManager::TickUiInput() {
	SteeringPlaybackMode playback_mode;
	v_tryget(playback_mode, =(SteeringPlaybackMode), GetVariableScope(), kRtvarSteeringPlaybackmode, kPlaybackNone);
	const int physics_step_count = GetTimeManager()->GetAffordedPhysicsStepCount();
	if (playback_mode != kPlaybackPlay && physics_step_count > 0 && allow_movement_input_) {
		cure::ContextObject* _object = GetContext()->GetObject(avatar_id_);

		// Show billboard.
		v_internal(GetVariableScope(), kRtvarGameDrawscore, !_object || options_.GetShowScore());

		if (_object) {
			float childishness;
			v_get(childishness, =(float), GetVariableScope(), kRtvarGameChildishness, 1.0);
			_object->QuerySetChildishness(childishness);

			// Control steering.
			const life::options::Steering& s = options_.GetSteeringControl();
#define S(dir) s.control_[life::options::Steering::CONTROL_##dir]
			vec2 left_power(S(RIGHT)-S(LEFT), S(FORWARD)-S(BRAKEANDBACK));
			vec2 right_power(S(RIGHT3D)-S(LEFT3D), S(FORWARD3D)-S(BACKWARD3D));
			if (left_power*right_power > 0.2f) {
				// Pointing somewhat in the same direction, so let's assume that's what the user is trying to accomplish.
				left_power = (left_power+right_power) * 0.5f;
				const float angle = left_power.GetAngle();
				if (angle >= -PIF/4 && angle <= PIF/4) {
					left_power.Set(+1, 0);
				} else if (angle >= -3*PIF/4 && angle <= -PIF/4) {
					left_power.Set(0, -1);
				} else if (angle <= 3*PIF/4 && angle >= PIF/4) {
					left_power.Set(0, +1);
				} else {
					left_power.Set(-1, 0);
				}
				right_power = left_power;
			}
			/*else if (std::abs(left_power.x) > std::abs(left_power.y)*2 && right_power.GetLengthSquared() < 0.05f) {
				// Left stick moving left/right. Simulate left or right rotation instead of the logically expected one-sided strafe.
				left_power.y = left_power.x;
				right_power.y = -left_power.x;
				left_power.x = 0;
			} else if (std::abs(right_power.x) > std::abs(right_power.y)*2 && left_power.GetLengthSquared() < 0.05f) {
				// Right stick moving left/right. Simulate left or right rotation instead of the logically expected one-sided strafe.
				right_power.y = -right_power.x;
				left_power.y = right_power.x;
				right_power.x = 0;
			}*/
			//const float steering_power = std::abs(lLeftPowerFwdRev);
			//lRightPowerLR *= Math::Lerp(0.8f, 2.0f, steering_power);
			deb_assert(left_power.y  >= -3 && left_power.y  <= +3);
			deb_assert(right_power.y >= -3 && right_power.y <= +3);
			deb_assert(left_power.x  >= -3 &&  left_power.x <= +3);
			deb_assert(right_power.x >= -3 && right_power.y <= +3);

			// Mouse controls yaw angle.
			const float angle_delta = S(YAW_ANGLE) * 0.05f;
			const cure::ContextObject* _avatar = GetContext()->GetObject(avatar_id_);
			const bool is_updating_yaw = !!angle_delta;
			const bool is_timed_yaw_update = (camera_mouse_angle_timer_.QueryTimeDiff() < 1.5f);
			if (_avatar && update_camera_for_avatar_) {
				update_camera_for_avatar_ = false;
				float current_angle = 0;
				float _;
				_avatar->GetOrientation().GetEulerAngles(current_angle, _, _);
				//current_angle = +PIF/2 - current_angle;
				//const vec3 fwd = _avatar->GetForwardDirection();
				//log_.Infof("Setting cam from avatar fwd vec %f, %f, %f", fwd.x, fwd.y, fwd.z);
				//const quat r = _avatar->GetOrientation();
				//log_.Infof("Setting cam from Q=%f, %f, %f, %f", r.a, r.b, r.c, r.d);
				camera_mouse_angle_ = current_angle;
				camera_target_angle_ = current_angle;
				camera_orientation_.x = current_angle + PIF/2;
			}
			/*if (_avatar) {
				static int pc = 0;
				if (++pc > 20) {
					pc = 0;
					//const vec3 fwd = _avatar->GetForwardDirection();
					//log_.Infof("Avatar fwd vec %f, %f, %f", fwd.x, fwd.y, fwd.z);
					//const quat q = _avatar->GetPhysics()->GetOriginalBoneTransformation(0).GetOrientation();
					//log_.Infof("Avatar original Q=%f, %f, %f, %f", q.a, q.b, q.c, q.d);
					const quat r = _avatar->GetOrientation();
					log_.Infof("Avatar Q=%f, %f, %f, %f", r.a, r.b, r.c, r.d);
				}
			}*/
			if (_avatar && (is_updating_yaw || is_timed_yaw_update)) {
				float current_angle = 0;
				float _;
				_avatar->GetOrientation().GetEulerAngles(current_angle, _, _);
				const bool is_first_yaw_update = !is_timed_yaw_update;
				if (is_first_yaw_update) {
					camera_mouse_angle_ = current_angle;
				}
				if (is_updating_yaw) {
					camera_mouse_angle_timer_.ClearTimeDiff();
				}
				camera_mouse_angle_ -= angle_delta;
				Math::RangeAngles(current_angle, camera_mouse_angle_);
				camera_target_angle_ = camera_mouse_angle_;
				camera_target_angle_factor_ = 1;
				float angle_diff = current_angle - camera_mouse_angle_;
				const vec3 rotation_velocity = _avatar->GetAngularVelocity();
				float rotation_friction = -rotation_velocity.z * 0.2f;
				if ((angle_delta < 0) == (angle_diff > 0)) {
					rotation_friction = 0;
				}
				/*static int pc = 0;
				if (++pc > 5) {
					pc = 0;
					log_.Infof("angle=%f, delta=%f, current=%f, diff=%f, friction=%f", camera_mouse_angle_, angle_delta, current_angle, current_angle - camera_mouse_angle_, rotation_friction);
				}*/
				const float _strength = Math::Lerp(1.0f, 2.0f, left_power.GetLength());
				angle_diff *= 4;
				angle_diff -= rotation_friction * _strength;
				right_power.x += angle_diff;
			} else {
				camera_target_angle_factor_ *= 0.5f;
			}

			SetAvatarEnginePower(_object, 0,  left_power.y);
			SetAvatarEnginePower(_object, 1,  left_power.x);
			SetAvatarEnginePower(_object, 4, right_power.y);
			SetAvatarEnginePower(_object, 5, right_power.x);
			SetAvatarEnginePower(_object, 8, left_power.GetDistance(right_power)*0.5f);
			const float steering_change = last_steering_-s;
			if (steering_change > 0.5f) {
				input_expire_alarm_.Set();
			} else if (!Math::IsEpsEqual(steering_change, 0.0f, 0.01f)) {
				input_expire_alarm_.Push(0.1f);
			}
			last_steering_ = s;

			// Control camera.
			const float scale = 50.0f * GetTimeManager()->GetAffordedPhysicsTotalTime();
			const life::options::CamControl& c = options_.GetCamControl();
#define C(dir) c.control_[life::options::CamControl::CAMDIR_##dir]
			float cam_power = C(UP)-C(DOWN);
			v_internal_arithmetic(GetVariableScope(), kRtvarUi3DCamheight, double, +, cam_power*scale, -5.0, 30.0);
			cam_rotate_extra_ = (C(RIGHT)-C(LEFT)) * scale;
			cam_power = C(BACKWARD)-C(FORWARD);
			v_internal_arithmetic(GetVariableScope(), kRtvarUi3DCamdistance, double, +, cam_power*scale, 3.0, 100.0);

			// Control fire.
			const life::options::FireControl& f = options_.GetFireControl();
#define F(alt) f.control_[life::options::FireControl::FIRE##alt]
			if (F(0) > 0.5f) {
				AvatarShoot();
			}

			avatar_invisible_count_ = 0;
		} else if (++avatar_invisible_count_ > 60) {
			SetRoadSignsVisible(true);
			avatar_invisible_count_ = -100000;
		}
	}
}

bool HoverTankManager::SetAvatarEnginePower(cure::ContextObject* avatar, unsigned aspect, float power) {
	bool set = avatar->SetEnginePower(aspect, power);

	SteeringPlaybackMode playback_mode;
	v_tryget(playback_mode, =(SteeringPlaybackMode), GetVariableScope(), kRtvarSteeringPlaybackmode, kPlaybackNone);
	if (playback_mode == kPlaybackRecord) {
		if (!Math::IsEpsEqual(engine_power_shadow_[aspect].power_, power)
			//|| !Math::IsEpsEqual(engine_power_shadow_[aspect].angle_, angle, 0.3f)
			) {
			engine_power_shadow_[aspect].power_ = power;
			if (!engine_playback_file_.IsOpen()) {
				engine_playback_file_.Open("Data/Steering.rec", DiskFile::kModeTextWrite);
				wstr comment = wstrutil::Format(L"// Recording %s at %s.\n", avatar->GetClassId().c_str(), Time().GetDateTimeAsString().c_str());
				engine_playback_file_.WriteString(comment);
				engine_playback_file_.WriteString(wstrutil::Encode("#" kRtvarSteeringPlaybackmode " 2\n"));
			}
			const float _time = GetTimeManager()->GetAbsoluteTime();
			if (_time != engine_playback_time_) {
				wstr command = wstrutil::Format(L"sleep %g\n", cure::TimeManager::GetAbsoluteTimeDiff(_time, engine_playback_time_));
				engine_playback_file_.WriteString(command);
				engine_playback_time_ = _time;
			}
			wstr command = wstrutil::Format(L"set-avatar-engine-power %u %g\n", aspect, power);
			engine_playback_file_.WriteString(command);
		}
	} else if (playback_mode == kPlaybackNone) {
		if (engine_playback_file_.IsOpen()) {
			if (engine_playback_file_.IsInMode(File::kWriteMode)) {
				engine_playback_file_.WriteString(wstrutil::Encode("#" kRtvarSteeringPlaybackmode " 0\n"));
			}
			engine_playback_file_.Close();
		}
		engine_playback_time_ = GetTimeManager()->GetAbsoluteTime();
		engine_power_shadow_[aspect].power_ = 0;
	}

	return set;
}

void HoverTankManager::TickUiUpdate() {
	((HoverTankConsoleManager*)GetConsoleManager())->GetUiConsole()->Tick();
	collision_sound_manager_->Tick(camera_position_);
}

bool HoverTankManager::UpdateMassObjects(const vec3& position) {
	bool _ok = true;
	ObjectArray::const_iterator x = mass_object_array_.begin();
	for (; x != mass_object_array_.end(); ++x) {
		life::MassObject* _object = (life::MassObject*)GetContext()->GetObject(*x, true);
		deb_assert(_object);
		_object->SetRootPosition(position);
	}
	return _ok;
}

void HoverTankManager::SetLocalRender(bool render) {
	if (render) {
		// Update light and sun according to this slave's camera.
		Sunlight* sunlight = ((HoverTankTicker*)GetMaster())->GetSunlight();
		const float sun_distance = 1700;
		sun_->SetRootPosition(camera_position_ + sun_distance * sunlight->GetDirection());

		const float cloud_distance = 600;
		size_t x = 0;
		for (; x < cloud_array_.size(); ++x) {
			cure::ContextObject* cloud = cloud_array_[x];
			float tod = sunlight->GetTimeOfDay();
			tod += x / (float)cloud_array_.size();
			tod *= 2 * PIF;
			const float x = sin(tod*2) * cloud_distance;
			const float y = cos(tod) * cloud_distance;
			const float z = cos(tod*3) * cloud_distance * 0.2f + cloud_distance * 0.4f;
			cloud->SetRootPosition(vec3(x, y, z));
		}

		bool mass;
		v_get(mass, =, GetVariableScope(), kRtvarUi3DEnablemassobjects, false);
		SetMassRender(mass);
	} else {
		SetMassRender(false);
	}
}

void HoverTankManager::SetMassRender(bool render) {
	ObjectArray::const_iterator x = mass_object_array_.begin();
	for (; x != mass_object_array_.end(); ++x) {
		life::MassObject* _object = (life::MassObject*)GetContext()->GetObject(*x);
		if (_object) {
			_object->SetRender(render);
		}
	}
}



void HoverTankManager::ProcessNetworkInputMessage(cure::Message* message) {
	Parent::ProcessNetworkInputMessage(message);

	cure::MessageType _type = message->GetType();
	switch (_type) {
		case cure::kMessageTypeDeleteObject: {
			cure::MessageDeleteObject* message_delete_object = (cure::MessageDeleteObject*)message;
			cure::GameObjectId id = message_delete_object->GetObjectId();
			if (id == avatar_id_) {
				DropAvatar();
			}
			owned_object_list_.erase(id);
		} break;
	}
}

void HoverTankManager::ProcessNetworkStatusMessage(cure::MessageStatus* message) {
	switch (message->GetInfo()) {
		case cure::MessageStatus::kInfoAvatar: {
			wstr avatar_name;
			message->GetMessageString(avatar_name);
			cure::UserAccount::AvatarId _avatar_id = avatar_name;
			log_adebug("Status: INFO_AVATAR...");
			str texture_id = strutil::Format("road_sign_%s.png", _avatar_id.c_str());
			if (!GetResourceManager()->QueryFileExists(texture_id)) {
				texture_id = "road_sign_car.png";
			}
			RoadSignButton* _button = new RoadSignButton(this, GetResourceManager(),
				ui_manager_, _avatar_id, "road_sign_01", texture_id, RoadSignButton::kShapeRound);
			GetContext()->AddLocalObject(_button);
			const int SIGN_COUNT_X = 5;
			const int SIGN_COUNT_Y = 5;
			const float delta_x = 1 / (float)SIGN_COUNT_X;
			const float delta_y = 1 / (float)SIGN_COUNT_Y;
			const float x = (road_sign_index_ % SIGN_COUNT_X) * delta_x - 0.5f + 0.5f*delta_x;
			const float y = (road_sign_index_ / SIGN_COUNT_X) * delta_y - 0.5f + 0.5f*delta_y;
			++road_sign_index_;
			_button->SetTrajectory(vec2(x, y), 8);
			_button->GetButton().SetOnClick(HoverTankManager, OnAvatarSelect);
			road_sign_map_.insert(RoadSignMap::value_type(_button->GetInstanceId(), _button));
			_button->StartLoading();
		}
		return;
	}
	Parent::ProcessNetworkStatusMessage(message);
}

void HoverTankManager::ProcessNumber(cure::MessageNumber::InfoType type, int32 integer, float32 _f) {
	switch (type) {
		case cure::MessageNumber::kInfoAvatar: {
			avatar_id_ = integer;
			owned_object_list_.insert(avatar_id_);
			update_camera_for_avatar_ = true;
			camera_mouse_angle_timer_.ReduceTimeDiff(-10.0f);
			camera_target_angle_factor_ = 0;
			log_volatile(log_.Debugf("Got control over avatar with ID %i.", integer));
		}
		return;
		case cure::MessageNumber::kInfoFallApart: {
			const cure::GameObjectId instance_id = integer;
			UiCure::CppContextObject* _object = (UiCure::CppContextObject*)GetContext()->GetObject(instance_id);
			if (_object) {
				life::Explosion::FallApart(GetPhysicsManager(), _object);
				_object->CenterMeshes();
				log_volatile(log_.Debugf("Object %i falling apart.", integer));
			}
		}
		return;
		case cure::MessageNumber::kInfoTool0: {
			const cure::GameObjectId _avatar_id = integer;
			UiCure::CppContextObject* _avatar = (UiCure::CppContextObject*)GetContext()->GetObject(_avatar_id);
			if (_avatar) {
				Shoot(_avatar, (int)_f);
			}
		}
		return;
	}
	Parent::ProcessNumber(type, integer, _f);
}

cure::ContextObject* HoverTankManager::CreateContextObject(const str& class_id) const {
	cure::CppContextObject* _object;
	if (class_id == "grenade" || class_id == "rocket") {
		_object = new life::FastProjectile(GetResourceManager(), class_id, ui_manager_, (HoverTankManager*)this);
	} else if (class_id == "bomb") {
		_object = new life::Projectile(GetResourceManager(), class_id, ui_manager_, (HoverTankManager*)this);
	} else if (strutil::StartsWith(class_id, "mine")) {
		_object = new life::Mine(GetResourceManager(), class_id, ui_manager_, (HoverTankManager*)this);
	} else if (class_id == "stone" || class_id == "cube") {
		_object = new UiCure::CppContextObject(GetResourceManager(), class_id, ui_manager_);
	} else if (strutil::StartsWith(class_id, "level_")) {
		UiCure::GravelEmitter* gravel_particle_emitter = new UiCure::GravelEmitter(GetResourceManager(), ui_manager_, 0.5f, 1, 10, 2);
		life::Level* level = new life::Level(GetResourceManager(), class_id, ui_manager_, gravel_particle_emitter);
		level->EnableRootShadow(false);
		_object = level;
	} else if (class_id == "score_info") {
		_object = new UiCure::CppContextObject(GetResourceManager(), "score_info", ui_manager_);
		_object->SetLoadResult(true);
	} else if (strutil::StartsWith(class_id, "hover_tank") ||
		strutil::StartsWith(class_id, "deltawing")) {
		UiCure::Machine* machine = new life::ExplodingMachine(GetResourceManager(), class_id, ui_manager_, (HoverTankManager*)this);
		machine->SetJetEngineEmitter(new UiCure::JetEngineEmitter(GetResourceManager(), ui_manager_));
		_object = machine;
	} else {
		UiCure::Machine* machine = new UiCure::Machine(GetResourceManager(), class_id, ui_manager_);
		machine->SetExhaustEmitter(new UiCure::ExhaustEmitter(GetResourceManager(), ui_manager_));
		_object = machine;
	}
	_object->SetAllowNetworkLogic(false);	// Only server gets to control logic.
	return (_object);
}

void HoverTankManager::OnLoadCompleted(cure::ContextObject* object, bool ok) {
	if (ok) {
		if (object->GetInstanceId() == avatar_id_) {
			log_volatile(log_.Debug("Yeeha! Loaded avatar!"));
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

void HoverTankManager::OnCollision(const vec3& force, const vec3& torque, const vec3& position,
	cure::ContextObject* object1, cure::ContextObject* object2,
	tbc::PhysicsManager::BodyID body1_id, tbc::PhysicsManager::BodyID) {
	collision_sound_manager_->OnCollision(force, torque, position, object1, object2, body1_id, 200, false);

	const bool object1_dynamic = (object1->GetPhysics()->GetPhysicsType() == tbc::ChunkyPhysics::kDynamic);
	const bool object2_dynamic = (object2->GetPhysics()->GetPhysicsType() == tbc::ChunkyPhysics::kDynamic);
	if (!object1_dynamic || !object2_dynamic) {
		return;
	}

	if (IsOwned(object1->GetInstanceId())) {
		if (object1->GetImpact(GetPhysicsManager()->GetGravity(), force, torque) >= 2.0f) {
			object1->QueryResendTime(0, false);
		}
		collision_expire_alarm_.SetIfNotSet();
	} else if (object2->GetInstanceId() == avatar_id_ &&
		object1->GetNetworkObjectType() == cure::kNetworkObjectRemoteControlled) {
		if (!GetMaster()->IsLocalObject(object1->GetInstanceId()) &&
			object1->GetImpact(GetPhysicsManager()->GetGravity(), force, torque) >= 0.5f) {
			if (object1->QueryResendTime(1.0, false)) {
				GetNetworkAgent()->SendNumberMessage(false, GetNetworkClient()->GetSocket(),
					cure::MessageNumber::kInfoRequestLoan, object1->GetInstanceId(), 0, 0);
				log_adebug("Sending loan request to server.");
			}
		}
	}
}



void HoverTankManager::OnFireButton(uitbc::Button*) {
	AvatarShoot();
}

void HoverTankManager::AvatarShoot() {
	if (fire_timeout_.QueryTimeDiff() < 0.15f) {
		return;

	}

	cure::ContextObject* _avatar = GetContext()->GetObject(avatar_id_);
	if (!_avatar) {
		return;
	}

	fire_timeout_.ClearTimeDiff();
	GetNetworkClient()->SendNumberMessage(false, GetNetworkClient()->GetSocket(),
		cure::MessageNumber::kInfoTool0, 0, (float)active_weapon_);

	if (active_weapon_ == 0) {
		Shoot(_avatar, active_weapon_);
	}

	++active_weapon_;
	active_weapon_ %= 3;
}

void HoverTankManager::Shoot(cure::ContextObject* avatar, int weapon) {
	str ammo;
	switch (weapon) {
		case 0:	ammo = "bullet";	break;
		default: deb_assert(false); return;
	}
	life::FastProjectile* projectile = new life::FastProjectile(GetResourceManager(), ammo, ui_manager_, this);
	AddContextObject(projectile, cure::kNetworkObjectLocalOnly, 0);
	projectile->SetOwnerInstanceId(avatar->GetInstanceId());
	xform t(avatar->GetOrientation(), avatar->GetPosition());
	projectile->SetInitialTransform(t);
	projectile->StartLoading();

	if (weapon >= 0) {
		uitbc::ParticleRenderer* particle_renderer = (uitbc::ParticleRenderer*)ui_manager_->GetRenderer()->GetDynamicRenderer("particle");
		xform t;
		vec3 v;
		life::ProjectileUtil::GetBarrel(projectile, t, v);
		particle_renderer->CreateFlare(vec3(0.9f, 0.7f, 0.5f), 0.3f, 7.5f, t.GetPosition(), v);
	}
}



void HoverTankManager::CancelLogin() {
	CloseLoginGui();
	SetIsQuitting();
}

void HoverTankManager::OnVehicleSelect(uitbc::Button* button) {
	(void)button;
	SetRoadSignsVisible(true);
}

void HoverTankManager::OnAvatarSelect(uitbc::Button* button) {
	cure::UserAccount::AvatarId _avatar_id = button->GetName();
	SelectAvatar(_avatar_id);

	if (!pick_vehicle_button_) {
		pick_vehicle_button_ = new RoadSignButton(this, GetResourceManager(), ui_manager_, "PickVehicle",
			"road_sign_01", "road_sign_car.png", RoadSignButton::kShapeRound);
		GetContext()->AddLocalObject(pick_vehicle_button_);
		pick_vehicle_button_->SetTrajectory(vec2(0, 0.45f), 20);
		pick_vehicle_button_->SetTrajectoryAngle(-PIF/2);
		pick_vehicle_button_->GetButton().SetOnClick(HoverTankManager, OnVehicleSelect);
		pick_vehicle_button_->StartLoading();
	}
	pick_vehicle_button_->SetIsMovingIn(true);
}

void HoverTankManager::DropAvatar() {
	owned_object_list_.erase(avatar_id_);
	avatar_id_ = 0;
	had_avatar_ = false;
}



void HoverTankManager::DrawStick(Touchstick* stick) {
	cure::ContextObject* _avatar = GetContext()->GetObject(avatar_id_);
	if (!stick || !_avatar) {
		return;
	}

	PixelRect area = stick->GetArea();
	const int ow = area.GetWidth();
	const int margin = stick->GetFingerRadius() / 8;
	const int r = stick->GetFingerRadius() - margin;
	area.Shrink(margin*2);
	ui_manager_->GetPainter()->DrawArc(area.left_, area.top_, area.GetWidth(), area.GetHeight(), 0, 360, false);
	float x;
	float y;
	bool is_pressing;
	stick->GetValue(x, y, is_pressing);
	if (is_pressing) {
		vec2 v(x, y);
		v.Mul((ow+margin*2) / (float)ow);
		const float length = v.GetLength();
		if (length > 1) {
			v.Div(length);
		}
		x = v.x;
		y = v.y;
		x = 0.5f*x + 0.5f;
		y = 0.5f*y + 0.5f;
		const int w = area.GetWidth()  - r*2;
		const int h = area.GetHeight() - r*2;
		ui_manager_->GetPainter()->DrawArc(
			area.left_ + (int)(w*x),
			area.top_  + (int)(h*y),
			r*2, r*2, 0, 360, true);
	}
}

void HoverTankManager::DrawScore() {
	typedef cure::ContextObject::AttributeArray AttributeArray;
	if (!score_info_id_) {
		const cure::ContextManager::ContextObjectTable& object_table = GetContext()->GetObjectTable();
		cure::ContextManager::ContextObjectTable::const_iterator x = object_table.begin();
		for (; x != object_table.end(); ++x) {
			cure::ContextObject* _object = x->second;
			const AttributeArray& attribute_array = _object->GetAttributes();
			AttributeArray::const_iterator y = attribute_array.begin();
			for (; y != attribute_array.end(); ++y) {
				cure::ContextObjectAttribute* attribute = *y;
				if (strutil::StartsWith(attribute->GetName(), "int_kills:")) {
					score_info_id_ = _object->GetInstanceId();
					return;	// Better luck next time.
				}
			}
		}
	}

	cure::ContextObject* score_info = GetContext()->GetObject(score_info_id_);
	if (!score_info) {
		score_info_id_ = 0;
		return;
	}

	typedef std::unordered_map<str, Score*> ScoreMap;
	typedef std::vector<Score> ScoreArray;
	ScoreMap score_map;
	ScoreArray score_array;
	const AttributeArray& attribute_array = score_info->GetAttributes();
	AttributeArray::const_iterator y = attribute_array.begin();
	for (; y != attribute_array.end(); ++y) {
		cure::ContextObjectAttribute* attribute = *y;
		str name;
		int value = 0;
		int mode = 0;
		if (strutil::StartsWith(attribute->GetName(), "int_kills:")) {
			name = attribute->GetName().substr(10);
			value = ((cure::IntAttribute*)attribute)->GetValue();
			mode = 0;
		} else if (strutil::StartsWith(attribute->GetName(), "int_deaths:")) {
			name = attribute->GetName().substr(11);
			value = ((cure::IntAttribute*)attribute)->GetValue();
			mode = 1;
		} else if (strutil::StartsWith(attribute->GetName(), "int_ping:")) {
			name = attribute->GetName().substr(9);
			value = ((cure::IntAttribute*)attribute)->GetValue();
			mode = 2;
		}
		if (!name.empty()) {
			ScoreMap::iterator x = score_map.find(name);
			if (x == score_map.end()) {
				Score s;
				s.name_ = name;
				s.kills_ = 0;
				s.deaths_ = 0;
				s.ping_ = 0;
				score_array.push_back(s);
				x = score_map.insert(ScoreMap::value_type(name, &score_array.back())).first;
			}
			switch (mode) {
				case 0:	x->second->kills_	= value;	break;
				case 1:	x->second->deaths_	= value;	break;
				case 2:	x->second->ping_	= value;	break;
			}
		}
	}
	std::sort(score_array.begin(), score_array.end(), g_deaths_ascending_order);
	std::sort(score_array.begin(), score_array.end(), g_kills_decending_order);
	ui_manager_->GetPainter()->SetTabSize(140);
	str score = "Name\tKills\tDeaths\tPing";
	ScoreArray::iterator x = score_array.begin();
	for (; x != score_array.end(); ++x) {
		score += strutil::Format("\n%s\t%i\t%i\t%i", x->name_.c_str(), x->kills_, x->deaths_, x->ping_);
	}
	ui_manager_->GetPainter()->SetColor(Color(1, 1, 1, 190), 0);
	ui_manager_->GetPainter()->SetColor(Color(0, 0, 0, 0), 1);
	ui_manager_->GetPainter()->SetAlphaValue(140);
	int height = ui_manager_->GetFontManager()->GetStringHeight(score);
	ui_manager_->GetPainter()->FillRect(render_area_.left_ + 10, 30, render_area_.left_ + 450, 30+height+20);
	ui_manager_->GetPainter()->SetColor(Color(140, 140, 140, 255), 0);
	ui_manager_->GetPainter()->SetAlphaValue(255);
	ui_manager_->GetPainter()->PrintText(score, render_area_.left_ + 20, 40);
}



void HoverTankManager::ScriptPhysicsTick() {
	// Camera moves in a "moving average" kinda curve (halfs the distance in x seconds).
	const float physics_time = GetTimeManager()->GetAffordedPhysicsTotalTime();
	if (physics_time > 1e-5) {
		MoveCamera();
		UpdateCameraPosition(false);
	}

	Parent::ScriptPhysicsTick();
}

void HoverTankManager::MoveCamera() {
	cure::ContextObject* _object = GetContext()->GetObject(avatar_id_);
	if (_object) {
		camera_pivot_position_ = _object->GetPosition();
		UpdateMassObjects(camera_pivot_position_);

		if (_object->GetAngularVelocity().GetLengthSquared() > 30.0f) {
			vec3 target = camera_pivot_position_  - GetCameraQuaternion() * vec3(0, camera_target_xy_distance_, 0);
			camera_position_ = Math::Lerp(camera_position_, target, 0.2f);
			return;
		}

		const vec3 forward3d = _object->GetForwardDirection();
		//const vec3 lRight3d = forward3d.Cross(vec3(0, 0, 1));
		vec3 backward2d = -forward3d.ProjectOntoPlane(vec3(0, 0, 1));
		//vec3 backward2d = lRight3d.ProjectOntoPlane(vec3(0, 0, 1));
		backward2d.Normalize(camera_target_xy_distance_);
		float cam_height;
		v_get(cam_height, =(float), GetVariableScope(), kRtvarUi3DCamheight, 10.0);
		backward2d.z = cam_height;
		{
			float rotation_factor;
			v_get(rotation_factor, =(float), GetVariableScope(), kRtvarUi3DCamrotate, 0.0);
			rotation_factor += cam_rotate_extra_ * 0.06f;
			camera_pivot_velocity_.x += rotation_factor;
			backward2d = quat(camera_pivot_velocity_.x, vec3(0,0,1)) * backward2d;
		}
		camera_previous_position_ = camera_position_;
		camera_position_ = Math::Lerp(camera_position_, camera_pivot_position_ + backward2d, 0.4f);
	}

	vec3 pivot_xy_position = camera_pivot_position_;
	pivot_xy_position.z = camera_position_.z;
	const float new_target_camera_xy_distance = camera_position_.GetDistance(pivot_xy_position);
	vec3 target_camera_orientation(::asin((camera_position_.x-pivot_xy_position.x)/new_target_camera_xy_distance) + PIF/2, 4*PIF/7, 0);
	if (pivot_xy_position.y-camera_position_.y < 0) {
		target_camera_orientation.x = -target_camera_orientation.x;
	}
	{
		float camera_target_angle = camera_target_angle_ + PIF/2;
		Math::RangeAngles(target_camera_orientation.x, camera_target_angle_);
		target_camera_orientation.x = Math::Lerp(target_camera_orientation.x, camera_target_angle, camera_target_angle_factor_);
		if (camera_target_angle_factor_ < 0.1f) {
			camera_target_angle_ = target_camera_orientation.x - PIF/2;
		}
	}
	Math::RangeAngles(camera_orientation_.x, target_camera_orientation.x);
	Math::RangeAngles(camera_orientation_.y, target_camera_orientation.y);
	float yaw_change = (target_camera_orientation.x-camera_orientation_.x) * 0.5f;
	yaw_change = Math::Clamp(yaw_change, -PIF*3/7, +PIF*3/7);
	target_camera_orientation.z = -yaw_change;
	Math::RangeAngles(camera_orientation_.z, target_camera_orientation.z);
	camera_orientation_ = Math::Lerp<vec3, float>(camera_orientation_, target_camera_orientation, 0.4f);
}

void HoverTankManager::UpdateCameraPosition(bool update_mic_position) {
	xform camera_transform(GetCameraQuaternion(), camera_position_);
	ui_manager_->SetCameraPosition(camera_transform);
	if (update_mic_position) {
		const float frame_time = GetTimeManager()->GetNormalFrameTime();
		if (frame_time > 1e-4) {
			vec3 _velocity = (camera_position_-camera_previous_position_) / frame_time;
			const float microphone_max_velocity = 100.0f;
			if (_velocity.GetLength() > microphone_max_velocity) {
				_velocity.Normalize(microphone_max_velocity);
			}
			const float lerp_time = Math::GetIterateLerpTime(0.9f, frame_time);
			microphone_speed_ = Math::Lerp(microphone_speed_, _velocity, lerp_time);
			ui_manager_->SetMicrophonePosition(camera_transform, microphone_speed_);
		}
	}
}

quat HoverTankManager::GetCameraQuaternion() const {
	const float theta = camera_orientation_.x;
	const float phi = camera_orientation_.y;
	const float gimbal = camera_orientation_.z;
	quat orientation;
	orientation.SetEulerAngles(theta-PIF/2, PIF/2-phi, gimbal);

#if defined(LEPRA_TOUCH) || defined(EMULATE_TOUCH)
	int index = 0;
	int count = 0;
	GetMaster()->GetSlaveInfo(this, index, count);
	if (count == 2) {
		if (index == 0) {
			orientation.RotateAroundOwnY(-PIF/2);
		} else {
			orientation.RotateAroundOwnY(+PIF/2);
		}
	}
#endif // touch or emulated touch.

	return (orientation);
}



loginstance(kGame, HoverTankManager);



}
