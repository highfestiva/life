
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "pushmanager.h"
#include <algorithm>
#include "../cure/include/contextmanager.h"
#include "../cure/include/floatattribute.h"
#include "../cure/include/intattribute.h"
#include "../cure/include/networkclient.h"
#include "../cure/include/timemanager.h"
#include "../lepra/include/random.h"
#include "../lepra/include/time.h"
#include "../life/lifeclient/clientoptions.h"
#include "../life/lifeclient/clientoptions.h"
#include "../life/lifeclient/level.h"
#include "../life/lifeclient/massobject.h"
#include "../life/lifeclient/uiconsole.h"
#include "../life/explosion.h"
#include "../life/projectileutil.h"
#include "../uicure/include/uicollisionsoundmanager.h"
#include "../uicure/include/uiexhaustemitter.h"
#include "../uicure/include/uigameuimanager.h"
#include "../uicure/include/uigravelemitter.h"
#include "../uicure/include/uiiconbutton.h"
#include "../uicure/include/uiprops.h"
#include "../uilepra/include/uitouchstick.h"
#include "../uitbc/include/gui/uidesktopwindow.h"
#include "../uitbc/include/gui/uifloatinglayout.h"
#include "../uitbc/include/uiparticlerenderer.h"
#include "pushconsolemanager.h"
#include "pushticker.h"
#include "roadsignbutton.h"
#include "rtvar.h"
#include "sunlight.h"
#include "version.h"

#define ICONBTN(i,n)			new UiCure::IconButton(ui_manager_, GetResourceManager(), i, n)
#define ICONBTNA(i,n)			ICONBTN(i, n)



namespace Push {



PushManager::PushManager(life::GameClientMasterTicker* pMaster, const cure::TimeManager* time,
	cure::RuntimeVariableScope* variable_scope, cure::ResourceManager* resource_manager,
	UiCure::GameUiManager* ui_manager, int slave_index, const PixelRect& render_area):
	Parent(pMaster, time, variable_scope, resource_manager, ui_manager, slave_index, render_area),
	collision_sound_manager_(0),
	avatar_id_(0),
	had_avatar_(false),
	update_camera_for_avatar_(false),
	cam_rotate_extra_(0),
	pick_vehicle_button_(0),
	avatar_invisible_count_(0),
	road_sign_index_(0),
	level_id_(0),
	level_(0),
	sun_(0),
	camera_position_(0, -200, 100),
	camera_follow_velocity_(0, 1, 0),
	camera_up_(0, 0, 1),
	camera_orientation_(PIF/2, acos(camera_position_.z/camera_position_.y), 0),
	camera_target_xy_distance_(20),
	camera_max_speed_(500),
	camera_mouse_angle_(0),
	camera_target_angle_(0),
	camera_target_angle_factor_(0),
	login_window_(0),
	stick_left_(0),
	stick_right_(0),
	engine_playback_time_(0) {
	collision_sound_manager_ = new UiCure::CollisionSoundManager(this, ui_manager);
	collision_sound_manager_->AddSound("explosion",	UiCure::CollisionSoundManager::SoundResourceInfo(0.8f, 0.4f, 0));
	collision_sound_manager_->AddSound("small_metal",	UiCure::CollisionSoundManager::SoundResourceInfo(0.2f, 0.4f, 0));
	collision_sound_manager_->AddSound("big_metal",	UiCure::CollisionSoundManager::SoundResourceInfo(1.5f, 0.4f, 0));
	collision_sound_manager_->AddSound("plastic",	UiCure::CollisionSoundManager::SoundResourceInfo(1.0f, 0.4f, 0));
	collision_sound_manager_->AddSound("rubber",	UiCure::CollisionSoundManager::SoundResourceInfo(1.0f, 0.5f, 0));
	collision_sound_manager_->AddSound("wood",	UiCure::CollisionSoundManager::SoundResourceInfo(1.0f, 0.5f, 0));

	::memset(engine_power_shadow_, 0, sizeof(engine_power_shadow_));

	camera_pivot_position_ = camera_position_ + GetCameraQuaternion() * vec3(0, camera_target_xy_distance_*3, 0);

	SetConsoleManager(new PushConsoleManager(GetResourceManager(), this, ui_manager_, GetVariableScope(), render_area_));
}

PushManager::~PushManager() {
	Close();

	delete stick_left_;
	stick_left_ = 0;
	delete stick_right_;
	stick_right_ = 0;
}

void PushManager::LoadSettings() {
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

void PushManager::SaveSettings() {
#ifndef EMULATE_TOUCH
	GetConsoleManager()->ExecuteCommand("save-application-config-file "+GetApplicationCommandFilename());
#endif // Computer or touch device.
}

void PushManager::SetRenderArea(const PixelRect& render_area) {
	Parent::SetRenderArea(render_area);
	if (login_window_) {
		login_window_->SetPos(render_area_.GetCenterX()-login_window_->GetSize().x/2,
			render_area_.GetCenterY()-login_window_->GetSize().y/2);
	}

	UpdateTouchstickPlacement();

	v_get(camera_target_xy_distance_, =(float), GetVariableScope(), kRtvarUi3DCamdistance, 20.0);
}

void PushManager::Close() {
	ScopeLock lock(GetTickLock());
	ClearRoadSigns();
	Parent::Close();
	CloseLoginGui();
}

void PushManager::SetIsQuitting() {
	CloseLoginGui();
	((PushConsoleManager*)GetConsoleManager())->GetUiConsole()->SetVisible(false);
	SetRoadSignsVisible(false);

	Parent::SetIsQuitting();
}

void PushManager::SetFade(float fade_amount) {
	camera_max_speed_ = 100000.0f;
	float base_distance;
	v_get(base_distance, =(float), GetVariableScope(), kRtvarUi3DCamdistance, 20.0);
	camera_target_xy_distance_ = base_distance + fade_amount*400.0f;
}



bool PushManager::Paint() {
	if (stick_left_) {
		DrawStick(stick_left_);
		DrawStick(stick_right_);
		stick_left_->ResetTap();
		stick_right_->ResetTap();
	}

#ifdef PUSH_DEMO
	const double _time = demo_time_.QueryTimeDiff();
	if ((slave_index_ >= 2 || (slave_index_ == 1 && _time > 10*60))
		&& !IsQuitting()) {
		const uitbc::FontManager::FontId old_font_id = SetFontHeight(36.0);
		str demo_text = strutil::Format(
			" This is a free demo.\n"
			" Buy the full version\n"
			"to loose this annoying\n"
			"  text for player %i.", slave_index_+1);
		if ((int)_time % 3*60 >= 3*60-2) {
			demo_text =
				"     ]0n7 b3 B1FF\n")
				"g!t pwn4ge & teh kekeke\n"
				"     !3UYZORZ n0vv\n";
		}
		const int text_width = ui_manager_->GetFontManager()->GetStringWidth(demo_text);
		const int text_height = ui_manager_->GetFontManager()->GetLineHeight()*4;
		const int offset_x = (int)(cos(_time*4.3)*15);
		const int offset_y = (int)(sin(_time*4.1)*15);
		ui_manager_->GetPainter()->SetColor(Color(255, (uint8)(50*sin(_time)+50), (uint8)(127*sin(_time*0.9)+127), 255), 0);
		ui_manager_->GetPainter()->SetColor(Color(0, 0, 0, 0), 1);
		ui_manager_->GetPainter()->PrintText(demo_text, render_area_.GetCenterX()-text_width/2+offset_x, render_area_.GetCenterY()-text_height/2+offset_y);
		ui_manager_->GetFontManager()->SetActiveFont(old_font_id);
	}
#endif // Demo
	return true;
}



void PushManager::RequestLogin(const str& server_address, const cure::LoginId& login_token) {
	ScopeLock lock(GetTickLock());
	CloseLoginGui();
	Parent::RequestLogin(server_address, login_token);
}

void PushManager::OnLoginSuccess() {
	ClearRoadSigns();
}



void PushManager::SelectAvatar(const cure::UserAccount::AvatarId& avatar_id) {
	DropAvatar();

	log_volatile(log_.Debugf("Clicked avatar %s.", avatar_id.c_str()));
	cure::Packet* packet = GetNetworkAgent()->GetPacketFactory()->Allocate();
	GetNetworkAgent()->SendStatusMessage(GetNetworkClient()->GetSocket(), 0, cure::kRemoteOk,
		cure::MessageStatus::kInfoAvatar, avatar_id, packet);
	GetNetworkAgent()->GetPacketFactory()->Release(packet);

	SetRoadSignsVisible(false);
}

void PushManager::AddLocalObjects(std::unordered_set<cure::GameObjectId>& local_object_set) {
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

bool PushManager::IsObjectRelevant(const vec3& position, float distance) const {
	return (position.GetDistanceSquared(camera_position_) <= distance*distance);
}

cure::GameObjectId PushManager::GetAvatarInstanceId() const {
	return avatar_id_;
}



bool PushManager::SetAvatarEnginePower(unsigned aspect, float power) {
	deb_assert(aspect >= 0 && aspect < tbc::PhysicsEngine::kAspectCount);
	cure::ContextObject* _object = GetContext()->GetObject(avatar_id_);
	if (_object) {
		return SetAvatarEnginePower(_object, aspect, power);
	}
	return false;
}



cure::RuntimeVariableScope* PushManager::GetVariableScope() const {
	return (Parent::GetVariableScope());
}



bool PushManager::Reset() {	// Run when disconnected. Removes all objects and displays login GUI.
	ScopeLock lock(GetTickLock());
	ClearRoadSigns();
	bool _ok = Parent::Reset();
	if (_ok) {
		CreateLoginView();
	}
	return (_ok);
}

void PushManager::CreateLoginView() {
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
                        str readable_password = "CarPassword";
                        const cure::MangledPassword password(readable_password);
			const cure::LoginId _login_token(user_name, password);
			RequestLogin(server_name, _login_token);
		} else {
			login_window_ = new LoginView(this, wstrutil::Encode(disconnect_reason_));
			ui_manager_->AssertDesktopLayout(new uitbc::FloatingLayout, 0);
			ui_manager_->GetDesktopWindow()->AddChild(login_window_, 0, 0, 0);
			login_window_->SetPos(render_area_.GetCenterX()-login_window_->GetSize().x/2,
				render_area_.GetCenterY()-login_window_->GetSize().y/2);
			login_window_->GetChild("User")->SetKeyboardFocus();
		}
	}
}

bool PushManager::InitializeUniverse() {
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

void PushManager::CloseLoginGui() {
	if (login_window_) {
		ScopeLock lock(GetTickLock());
		ui_manager_->GetDesktopWindow()->RemoveChild(login_window_, 0);
		delete (login_window_);
		login_window_ = 0;
	}
}

void PushManager::ClearRoadSigns() {
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

void PushManager::SetRoadSignsVisible(bool visible) {
	if (pick_vehicle_button_) {
		pick_vehicle_button_->SetIsMovingIn(!visible);
	}

	RoadSignMap::iterator x = road_sign_map_.begin();
	for (; x != road_sign_map_.end(); ++x) {
		x->second->SetIsMovingIn(visible);
	}

	ui_manager_->GetInputManager()->SetCursorVisible(visible);
}



void PushManager::TickInput() {
	TickNetworkInput();
	TickUiInput();
}



void PushManager::UpdateTouchstickPlacement() {
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

void PushManager::TickUiInput() {
	SteeringPlaybackMode playback_mode;
	v_tryget(playback_mode, =(SteeringPlaybackMode), GetVariableScope(), kRtvarSteeringPlaybackmode, kPlaybackNone);
	const int physics_step_count = GetTimeManager()->GetAffordedPhysicsStepCount();
	if (playback_mode != kPlaybackPlay && physics_step_count > 0 && allow_movement_input_) {
		cure::ContextObject* _object = GetContext()->GetObject(avatar_id_);

		if (_object) {
			float childishness;
			v_get(childishness, =(float), GetVariableScope(), kRtvarGameChildishness, 1.0);
			_object->QuerySetChildishness(childishness);

			// Control steering.
			const life::options::Steering& s = options_.GetSteeringControl();
#define S(dir) s.control_[life::options::Steering::kControl##dir]
			const float forward = S(Forward);
			const float back = S(Backward);
			const float brake_and_back = S(BrakeAndBack);
			const bool is_moving_forward = _object->GetForwardSpeed() > 3.0f;
			float power_fwd_rev = forward - std::max(back, is_moving_forward? 0.0f : brake_and_back);
			SetAvatarEnginePower(_object, 0, power_fwd_rev);
			float power_lr = S(Right)-S(Left);
			SetAvatarEnginePower(_object, 1, power_lr);
			float _power = S(Handbrake) - std::max(S(Brake), is_moving_forward? brake_and_back : 0.0f);
			if (!SetAvatarEnginePower(_object, 2, _power) &&
				brake_and_back > 0 && Math::IsEpsEqual(back, 0.0f, 0.01f)) {
				// Someone is apparently trying to stop/break, but no engine configured for breaking.
				// Just apply it as a reverse motion.
				power_fwd_rev = forward - brake_and_back;
				SetAvatarEnginePower(_object, 0, power_fwd_rev);
			}
			_power = S(Up)-S(Down);
			SetAvatarEnginePower(_object, 3, _power);
			_power = S(Forward3D) - S(Backward3D);
			SetAvatarEnginePower(_object, 4, _power);
			_power = S(Right3D) - S(Left3D);
			SetAvatarEnginePower(_object, 5, _power);
			// Engine aspect 6 is not currently in use (3D handbraking). Might come in useful some day though.
			_power = S(Up3D) - S(Down3D);
			SetAvatarEnginePower(_object, 7, _power);
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
#define C(dir) c.control_[life::options::CamControl::kCamdir##dir]
			float cam_power = C(Up)-C(Down);
			v_internal_arithmetic(GetVariableScope(), kRtvarUi3DCamheight, double, +, cam_power*scale, -5.0, 30.0);
			cam_rotate_extra_ = (C(Right)-C(Left)) * scale;
			cam_power = C(Backward)-C(Forward);
			v_internal_arithmetic(GetVariableScope(), kRtvarUi3DCamdistance, double, +, cam_power*scale, 3.0, 100.0);

			avatar_invisible_count_ = 0;
		} else if (++avatar_invisible_count_ > 60) {
			SetRoadSignsVisible(true);
			avatar_invisible_count_ = -100000;
		}
	}
}

bool PushManager::SetAvatarEnginePower(cure::ContextObject* avatar, unsigned aspect, float power) {
	bool set = avatar->SetEnginePower(aspect, power);

	SteeringPlaybackMode playback_mode;
	v_tryget(playback_mode, =(SteeringPlaybackMode), GetVariableScope(), kRtvarSteeringPlaybackmode, kPlaybackNone);
	if (playback_mode == kPlaybackRecord) {
		if (!Math::IsEpsEqual(engine_power_shadow_[aspect].power_, power)
			//|| !Math::IsEpsEqual(engine_power_shadow_[aspect].angle_, angle, 0.3f)
			) {
			engine_power_shadow_[aspect].power_ = power;
			if (!engine_playback_file_.IsOpen()) {
				engine_playback_file_.Open("data/steering.rec", DiskFile::kModeTextWrite);
				str comment = strutil::Format("// Recording %s at %s.\n", avatar->GetClassId().c_str(), Time().GetDateTimeAsString().c_str());
				engine_playback_file_.WriteString(comment);
				engine_playback_file_.WriteString(strutil::Encode("#" kRtvarSteeringPlaybackmode " 2\n"));
			}
			const float _time = GetTimeManager()->GetAbsoluteTime();
			if (_time != engine_playback_time_) {
				str command = strutil::Format("sleep %g\n", cure::TimeManager::GetAbsoluteTimeDiff(_time, engine_playback_time_));
				engine_playback_file_.WriteString(command);
				engine_playback_time_ = _time;
			}
			str command = strutil::Format("set-avatar-engine-power %u %g\n", aspect, power);
			engine_playback_file_.WriteString(command);
		}
	} else if (playback_mode == kPlaybackNone) {
		if (engine_playback_file_.IsOpen()) {
			if (engine_playback_file_.IsInMode(File::kWriteMode)) {
				engine_playback_file_.WriteString(strutil::Encode("#" kRtvarSteeringPlaybackmode " 0\n"));
			}
			engine_playback_file_.Close();
		}
		engine_playback_time_ = GetTimeManager()->GetAbsoluteTime();
		engine_power_shadow_[aspect].power_ = 0;
	}

	return set;
}

void PushManager::TickUiUpdate() {
	((PushConsoleManager*)GetConsoleManager())->GetUiConsole()->Tick();
	collision_sound_manager_->Tick(camera_position_);
}

bool PushManager::UpdateMassObjects(const vec3& position) {
	bool _ok = true;

	if (level_ && mass_object_array_.empty()) {
		const tbc::PhysicsManager::BodyID terrain_body_id = level_->GetPhysics()->GetBoneGeometry(0)->GetBodyId();
		if (_ok) {
			cure::GameObjectId mass_object_id = GetContext()->AllocateGameObjectId(cure::kNetworkObjectLocalOnly);
			mass_object_array_.push_back(mass_object_id);
			cure::ContextObject* flowers = new life::MassObject(GetResourceManager(), "flower", ui_manager_, terrain_body_id, 600, 170);
			AddContextObject(flowers, cure::kNetworkObjectLocalOnly, mass_object_id);
			flowers->StartLoading();
		}
		if (_ok) {
			cure::GameObjectId mass_object_id = GetContext()->AllocateGameObjectId(cure::kNetworkObjectLocalOnly);
			mass_object_array_.push_back(mass_object_id);
			cure::ContextObject* bushes = new life::MassObject(GetResourceManager(), "bush_01", ui_manager_, terrain_body_id, 150, 290);
			AddContextObject(bushes, cure::kNetworkObjectLocalOnly, mass_object_id);
			bushes->StartLoading();
		}
	}

	ObjectArray::const_iterator x = mass_object_array_.begin();
	for (; x != mass_object_array_.end(); ++x) {
		life::MassObject* _object = (life::MassObject*)GetContext()->GetObject(*x, true);
		deb_assert(_object);
		_object->SetRootPosition(position);
	}
	return _ok;
}

void PushManager::SetLocalRender(bool render) {
	if (render) {
		// Update light and sun according to this slave's camera.
		Sunlight* sunlight = ((PushTicker*)GetMaster())->GetSunlight();
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

void PushManager::SetMassRender(bool render) {
	ObjectArray::const_iterator x = mass_object_array_.begin();
	for (; x != mass_object_array_.end(); ++x) {
		life::MassObject* _object = (life::MassObject*)GetContext()->GetObject(*x);
		if (_object) {
			_object->SetRender(render);
		}
	}
}



void PushManager::ProcessNetworkInputMessage(cure::Message* message) {
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

void PushManager::ProcessNetworkStatusMessage(cure::MessageStatus* message) {
	switch (message->GetInfo()) {
		case cure::MessageStatus::kInfoAvatar: {
			str avatar_name;
			message->GetMessageString(avatar_name);
			cure::UserAccount::AvatarId _avatar_id = avatar_name;
			log_debug("Status: INFO_AVATAR...");
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
			_button->GetButton().SetOnClick(PushManager, OnAvatarSelect);
			road_sign_map_.insert(RoadSignMap::value_type(_button->GetInstanceId(), _button));
			_button->StartLoading();
		}
		return;
	}
	Parent::ProcessNetworkStatusMessage(message);
}

void PushManager::ProcessNumber(cure::MessageNumber::InfoType type, int32 integer, float32 _f) {
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
				life::Explosion::FallApart(GetPhysicsManager(), _object, true);
				_object->CenterMeshes();
				log_volatile(log_.Debugf("Object %i falling apart.", integer));
			}
		}
		return;
	}
	Parent::ProcessNumber(type, integer, _f);
}

cure::ContextObject* PushManager::CreateContextObject(const str& class_id) const {
	cure::CppContextObject* _object;
	if (class_id == "stone" || class_id == "cube") {
		_object = new UiCure::CppContextObject(GetResourceManager(), class_id, ui_manager_);
	} else if (strutil::StartsWith(class_id, "level_")) {
		UiCure::GravelEmitter* gravel_particle_emitter = new UiCure::GravelEmitter(GetResourceManager(), ui_manager_, 0.5f, 1, 10, 2);
		life::Level* level = new life::Level(GetResourceManager(), class_id, ui_manager_, gravel_particle_emitter);
		level->EnableRootShadow(false);
		level_ = level;
		_object = level;
	} else {
		UiCure::Machine* machine = new UiCure::Machine(GetResourceManager(), class_id, ui_manager_);
		machine->SetExhaustEmitter(new UiCure::ExhaustEmitter(GetResourceManager(), ui_manager_));
		_object = machine;
	}
	_object->SetAllowNetworkLogic(false);	// Only server gets to control logic.
	return (_object);
}

void PushManager::OnLoadCompleted(cure::ContextObject* object, bool ok) {
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

void PushManager::OnCollision(const vec3& force, const vec3& torque, const vec3& position,
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
				log_debug("Sending loan request to server.");
			}
		}
	}
}



void PushManager::CancelLogin() {
	CloseLoginGui();
	SetIsQuitting();
}

void PushManager::OnVehicleSelect(uitbc::Button* button) {
	(void)button;
	SetRoadSignsVisible(true);
}

void PushManager::OnAvatarSelect(uitbc::Button* button) {
	cure::UserAccount::AvatarId _avatar_id = button->GetName();
	SelectAvatar(_avatar_id);

	if (!pick_vehicle_button_) {
		pick_vehicle_button_ = new RoadSignButton(this, GetResourceManager(), ui_manager_, "PickVehicle",
			"road_sign_01", "road_sign_car.png", RoadSignButton::kShapeRound);
		GetContext()->AddLocalObject(pick_vehicle_button_);
		pick_vehicle_button_->SetTrajectory(vec2(0, 0.45f), 20);
		pick_vehicle_button_->SetTrajectoryAngle(-PIF/2);
		pick_vehicle_button_->GetButton().SetOnClick(PushManager, OnVehicleSelect);
		pick_vehicle_button_->StartLoading();
	}
	pick_vehicle_button_->SetIsMovingIn(true);
}

void PushManager::DropAvatar() {
	owned_object_list_.erase(avatar_id_);
	avatar_id_ = 0;
	had_avatar_ = false;
}



void PushManager::DrawStick(Touchstick* stick) {
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



void PushManager::ScriptPhysicsTick() {
	// Camera moves in a "moving average" kinda curve (halfs the distance in x seconds).
	const float physics_time = GetTimeManager()->GetAffordedPhysicsTotalTime();
	if (physics_time > 1e-5) {
		MoveCamera();
		UpdateCameraPosition(false);
	}

	Parent::ScriptPhysicsTick();
}

void PushManager::MoveCamera() {
	const float physics_time = GetTimeManager()->GetAffordedPhysicsTotalTime();
	camera_previous_position_ = camera_position_;
	cure::ContextObject* _object = GetContext()->GetObject(avatar_id_);
	vec3 avatar_position = camera_pivot_position_;
	float camera_pivot_speed = 0;
	if (_object) {
		// Target position is <cam> distance from the avatar along a straight line
		// (in the XY plane) to where the camera currently is.
		avatar_position = _object->GetPosition();
		camera_pivot_position_ = avatar_position;
		vec3 avatar_velocity = _object->GetVelocity();
		avatar_velocity.z *= 0.2f;	// Don't take very much action on the up/down speed, that has it's own algo.
		camera_pivot_velocity_ = Math::Lerp(camera_pivot_velocity_, avatar_velocity, 0.5f*physics_time/0.1f);
		camera_pivot_position_ += camera_pivot_velocity_ * 0.6f;	// Look to where the avatar will be in a while.
		camera_pivot_speed = camera_pivot_velocity_.GetLength();

		UpdateMassObjects(camera_pivot_position_);
	}
	const vec3 pivot_xy_position(camera_pivot_position_.x, camera_pivot_position_.y, camera_position_.z);
	vec3 target_camera_position(camera_position_-pivot_xy_position);
	const float current_camera_xy_distance = target_camera_position.GetLength();
	const float speed_dependant_camera_xy_distance = camera_target_xy_distance_ + camera_pivot_speed*0.6f;
	target_camera_position = pivot_xy_position + target_camera_position*(speed_dependant_camera_xy_distance/current_camera_xy_distance);
	float cam_height;
	v_get(cam_height, =(float), GetVariableScope(), kRtvarUi3DCamheight, 10.0);
	target_camera_position.z = camera_pivot_position_.z + cam_height;

	if (_object) {
		/* Almost tried out "stay behind velocity". Was too jerky, since velocity varies too much.
		vec3 velocity = _object->GetVelocity();
		camera_follow_velocity_ = velocity;
		float speed = velocity.GetLength();
		if (speed > 0.1f) {
			velocity.Normalize();
			camera_follow_velocity_ = Math::Lerp(camera_follow_velocity_, velocity, 0.1f).GetNormalized();
		}
		// Project previous "camera up" onto plane orthogonal to the velocity to get new "up".
		vec3 lCameraUp = camera_up_.ProjectOntoPlane(camera_follow_velocity_) + vec3(0, 0, 0.01f);
		if (lCameraUp.GetLengthSquared() > 0.1f) {
			camera_up_ = lCameraUp;
		}
		speed *= 0.05f;
		speed = (speed > 0.4f)? 0.4f : speed;
		camera_up_.Normalize();
		target_camera_position = Math::Lerp(target_camera_position, camera_pivot_position_ -
			camera_follow_velocity_ * camera_target_xy_distance_ +
			camera_up_ * camera_target_xy_distance_ * 0.3f, 0.0f);*/

		/*// Temporary: changed to "cam stay behind" mode.
		target_camera_position = _object->GetOrientation() *
			vec3(0, -camera_target_xy_distance_, camera_target_xy_distance_/4) +
			camera_pivot_position_;*/
	}

	target_camera_position.x = Math::Clamp(target_camera_position.x, -1000.0f, 1000.0f);
	target_camera_position.y = Math::Clamp(target_camera_position.y, -1000.0f, 1000.0f);
	target_camera_position.z = Math::Clamp(target_camera_position.z, -20.0f, 200.0f);

	// Now that we've settled where we should be, it's time to check where we actually can see our avatar.
	// TODO: currently only checks against terrain. Add a ray to world, that we can use for this kinda thing.
	if (level_ && level_->IsLoaded()) {
		const float camera_above_ground = 0.3f;
		target_camera_position.z -= camera_above_ground;
		const tbc::PhysicsManager::BodyID terrain_body_id = level_->GetPhysics()->GetBoneGeometry(0)->GetBodyId();
		vec3 collision_point;
		float step_size = (target_camera_position - avatar_position).GetLength() * 0.5f;
		for (int y = 0; y < 5; ++y) {
			int x;
			for (x = 0; x < 2; ++x) {
				const vec3 ray = target_camera_position - avatar_position;
				const bool is_collision = (GetPhysicsManager()->QueryRayCollisionAgainst(
					avatar_position, ray, ray.GetLength(), terrain_body_id, &collision_point, 1) > 0);
				if (is_collision) {
					target_camera_position.z += step_size;
				} else {
					if (x != 0) {
						target_camera_position.z -= step_size;
					} break;
				}
			}
			if (x == 0 && y == 0) {
				break;
			}
			step_size *= 1/3.0f;
			//target_camera_position.z += step_size;
		}
		target_camera_position.z += camera_above_ground;
	}

	const float half_distance_time = 0.1f;	// Time it takes to half the distance from where it is now to where it should be.
	float moving_average_part = 0.5f*physics_time/half_distance_time;
	if (moving_average_part > 0.8f) {
		moving_average_part = 0.8f;
	}
	//moving_average_part = 1;
	const vec3 new_position = Math::Lerp<vec3, float>(camera_position_,
		target_camera_position, moving_average_part);
	const vec3 direction = new_position-camera_position_;
	const float _distance = direction.GetLength();
	if (_distance > camera_max_speed_*physics_time) {
		camera_position_ += direction*(camera_max_speed_*physics_time/_distance);
	} else {
		camera_position_ = new_position;
	}
	if (new_position.z > camera_position_.z) {	// Dolly cam up pretty quick to avoid looking "through the ground."
		camera_position_.z = Math::Lerp(camera_position_.z, new_position.z, half_distance_time);
	}

	// "Roll" camera towards avatar.
	const float new_target_camera_xy_distance = camera_position_.GetDistance(pivot_xy_position);
	const float new_target_camera_distance = camera_position_.GetDistance(camera_pivot_position_);
	vec3 target_camera_orientation;
	target_camera_orientation.Set(::asin((camera_position_.x-pivot_xy_position.x)/new_target_camera_xy_distance) + PIF/2,
		::acos((camera_pivot_position_.z-camera_position_.z)/new_target_camera_distance), 0);
	if (pivot_xy_position.y-camera_position_.y < 0) {
		target_camera_orientation.x = -target_camera_orientation.x;
	}
	Math::RangeAngles(camera_orientation_.x, target_camera_orientation.x);
	float yaw_change = (target_camera_orientation.x-camera_orientation_.x)*3;
	yaw_change = Math::Clamp(yaw_change, -PIF*3/7, +PIF*3/7);
	target_camera_orientation.z = -yaw_change;
	Math::RangeAngles(camera_orientation_.x, target_camera_orientation.x);
	Math::RangeAngles(camera_orientation_.y, target_camera_orientation.y);
	Math::RangeAngles(camera_orientation_.z, target_camera_orientation.z);
	camera_orientation_ = Math::Lerp<vec3, float>(camera_orientation_, target_camera_orientation, moving_average_part);

	float rotation_factor;
	v_get(rotation_factor, =(float), GetVariableScope(), kRtvarUi3DCamrotate, 0.0);
	rotation_factor += cam_rotate_extra_;
	if (rotation_factor) {
		xform transform(GetCameraQuaternion(), camera_position_);
		transform.RotateAroundAnchor(camera_pivot_position_, vec3(0, 0, 1), rotation_factor * physics_time);
		camera_position_ = transform.GetPosition();
		float theta;
		float phi;
		float gimbal;
		transform.GetOrientation().GetEulerAngles(theta, phi, gimbal);
		camera_orientation_.x = theta+PIF/2;
		camera_orientation_.y = PIF/2-phi;
		camera_orientation_.z = gimbal;
	}
}

void PushManager::UpdateCameraPosition(bool update_mic_position) {
	xform camera_transform(GetCameraQuaternion(), camera_position_);
	ui_manager_->SetCameraPosition(camera_transform);
	if (update_mic_position) {
		const float frame_time = GetTimeManager()->GetNormalFrameTime();
		if (frame_time > 1e-4) {
			vec3 velocity = (camera_position_-camera_previous_position_) / frame_time;
			const float microphone_max_velocity = 100.0f;
			if (velocity.GetLength() > microphone_max_velocity) {
				velocity.Normalize(microphone_max_velocity);
			}
			const float lerp_time = Math::GetIterateLerpTime(0.9f, frame_time);
			microphone_speed_ = Math::Lerp(microphone_speed_, velocity, lerp_time);
			ui_manager_->SetMicrophonePosition(camera_transform, microphone_speed_);
		}
	}
}

quat PushManager::GetCameraQuaternion() const {
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



loginstance(kGame, PushManager);



}
