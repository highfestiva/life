
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "firemanager.h"
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
//#include "../life/lifeclient/fastprojectile.h"
//#include "../life/lifeclient/homingprojectile.h"
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
#include "../uicure/include/uiexhaustemitter.h"
#include "../uicure/include/uigameuimanager.h"
#include "../uicure/include/uiiconbutton.h"
#include "../uicure/include/uijetengineemitter.h"
#include "../uicure/include/uigravelemitter.h"
#include "../uicure/include/uisoundreleaser.h"
#include "../uilepra/include/uitouchdrag.h"
//#include "../uilepra/include/uiopenglextensions.h"
#include "../uitbc/include/gui/uicheckbutton.h"
#include "../uitbc/include/gui/uidesktopwindow.h"
#include "../uitbc/include/gui/uifixedlayouter.h"
#include "../uitbc/include/gui/uiradiobutton.h"
#include "../uitbc/include/gui/uitextarea.h"
#include "../uitbc/include/gui/uitextfield.h"
#include "../uitbc/include/uibillboardgeometry.h"
#include "../uitbc/include/uiparticlerenderer.h"
#include "../uitbc/include/uirenderer.h"
#include "../uitbc/include/uitrianglebasedgeometry.h"
#include "autopathdriver.h"
#include "basemachine.h"
#include "fire.h"
#include "fireconsolemanager.h"
#include "fireticker.h"
#include "level.h"
#include "rtvar.h"
#include "eater.h"
#include "sunlight.h"
#include "version.h"



namespace Fire {



#define DRAG_FLAG	uilepra::touch::kDragUser
#define BG_COLOR Color(40, 40, 40, 160)
const float hp = 768/1024.0f;
const int kLevels[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
const float kRollOutTime = 1.5f;
const float kTargetInfoDelay = 0.1f;
struct VillainTypes {
	str name_;
	int count_;
};
#define PERSONS_INNOCENT_PART 0.40f
int g_personality_count = 0;
const VillainTypes kVillainTypes[] =
{
	{_O("JI9,,/0,5+*~29=:9,", "Terrorist leader"), 331},
	{_O("Jg9,,/-,5+*", "Terrorist"), 1502},
	{_O("KX)<(9.,+5(9", "Subversive"), 333},
	{_O("U+22)1050=*5", "Illuminati"), 9},
	{_O("Kp5*6", "Sith"), 31},
	{_O("N>)*50f", "Putin"), 1},
	{_O("Qo),:9k,9,", "Murderer"), 157},
	{_O("G:65+*W29<2/'9,", "Whistleblower"), 14},
	{_O("[%6915 ;=2~]25", "Chemical Ali"), 1},
	{_O("=32q]+w+=:", "al-Assad"), 1},
	{_O("Xb)75*h5(9", "Fugitive"), 61},
	{_O("Jm,9+.}=++9,", "Trespasser"), 59},
	{_O("J]9,,/s,5+*~'=00=<9B", "Terrorist wannabe"), 91},
	{_O("]-GOR", "AWOL"), 33},
	{_O("[5/.%,z576*~(5/2=*/{,", "Copyright violator"), 3},
	{_O("]:00/%*507~7)%", "Annoying guy"), 3},
};



FireManager::FireManager(life::GameClientMasterTicker* pMaster, const cure::TimeManager* time,
	cure::RuntimeVariableScope* variable_scope, cure::ResourceManager* resource_manager,
	UiCure::GameUiManager* ui_manager, int slave_index, const PixelRect& render_area):
	Parent(pMaster, time, variable_scope, resource_manager, ui_manager, slave_index, render_area),
	collision_sound_manager_(0),
	menu_(0),
	level_(0),
	stepped_level_(false),
	sunlight_(0),
	camera_transform_(quat(), vec3()),
	pause_button_(0),
	bomb_button_(0),
	//check_icon_(0),
	kills_(0),
	kill_limit_(0),
	level_total_kills_(0) {
	fire_delay_timer_.Start();

	collision_sound_manager_ = new UiCure::CollisionSoundManager(this, ui_manager);
	collision_sound_manager_->AddSound("explosion",	UiCure::CollisionSoundManager::SoundResourceInfo(0.8f, 0.4f, 0));
	collision_sound_manager_->AddSound("small_metal",	UiCure::CollisionSoundManager::SoundResourceInfo(0.2f, 0.4f, 0));
	collision_sound_manager_->AddSound("rubber",		UiCure::CollisionSoundManager::SoundResourceInfo(1.0f, 0.5f, 0));
	collision_sound_manager_->AddSound("wood",		UiCure::CollisionSoundManager::SoundResourceInfo(1.0f, 0.5f, 0));
	collision_sound_manager_->PreLoadSound("explosion");

	SetConsoleManager(new FireConsoleManager(GetResourceManager(), this, ui_manager_, GetVariableScope(), render_area_));

	GetPhysicsManager()->SetSimulationParameters(0.0f, 0.03f, 0.2f);

	tbc::GeometryBase::SetDefaultFlags(tbc::GeometryBase::kExcludeCulling);	// Save some math during rendering, as most objects are on stage in this game.

	v_set(GetVariableScope(), kRtvarGameExplosivestrength, 1.0);
	v_set(GetVariableScope(), kRtvarGameFirstrun, true);
	v_set(GetVariableScope(), kRtvarGameFiredelay, 1.0);
	v_set(GetVariableScope(), kRtvarGameStartlevel, "lvl00");
	v_set(GetVariableScope(), kRtvarGameVehicleremovedelay, 25.0);
}

FireManager::~FireManager() {
	Close();

	delete collision_sound_manager_;
	collision_sound_manager_ = 0;
}

void FireManager::Suspend(bool hard) {
	if (!menu_->GetDialog()) {
		pause_button_->SetVisible(false);
		OnPauseButton(0);
	}
}

void FireManager::LoadSettings() {
	v_set(GetVariableScope(), kRtvarGameSpawnpart, 1.0);
	v_set(GetVariableScope(), kRtvarUi2DFont, "Verdana");
	v_set(GetVariableScope(), kRtvarUi3DFov, 38.8);

	Parent::LoadSettings();
	v_slowget(GetVariableScope(), kRtvarUiSoundMastervolume, 1.0);

	v_set(GetVariableScope(), kRtvarPhysicsNoclip, false);
	v_set(GetVariableScope(), kRtvarCtrlEmulatetouch, true);

	GetConsoleManager()->ExecuteCommand("bind-key F2 prev-level");
	GetConsoleManager()->ExecuteCommand("bind-key F3 next-level");
}

void FireManager::SaveSettings() {
	GetConsoleManager()->ExecuteCommand("save-application-config-file "+GetApplicationCommandFilename());
}

void FireManager::SetRenderArea(const PixelRect& render_area) {
	Parent::SetRenderArea(render_area);
}

bool FireManager::Open() {
	bool _ok = Parent::Open();
	if (_ok) {
		ui_manager_->GetDisplayManager()->SetCaption("NSAgent");
	}
	if (_ok) {
		pause_button_ = ICONBTNA("btn_pause.png", L"");
		int x = 12;
		int y = 12;
		ui_manager_->GetDesktopWindow()->AddChild(pause_button_, x, y);
		pause_button_->SetVisible(true);
		pause_button_->SetOnClick(FireManager, OnPauseButton);
	}
	if (_ok) {
		bomb_button_ = ICONBTNA("btn_bomb.png", L"");
		int x = 12;
		int y = 12*2+64;
		ui_manager_->GetDesktopWindow()->AddChild(bomb_button_, x, y);
		bomb_button_->SetVisible(false);
		bomb_button_->SetOnClick(FireManager, OnBombButton);
	}
	if (_ok) {
		menu_ = new life::Menu(ui_manager_, GetResourceManager());
		menu_->SetButtonTapSound("tap.wav", 1, 0.3f);
	}
	return _ok;
}

void FireManager::Close() {
	ScopeLock lock(GetTickLock());
	delete bomb_button_;
	bomb_button_ = 0;
	delete pause_button_;
	pause_button_ = 0;
	delete menu_;
	menu_ = 0;
	delete sunlight_;
	sunlight_ = 0;

	Parent::Close();
}

void FireManager::SetIsQuitting() {
	((FireConsoleManager*)GetConsoleManager())->GetUiConsole()->SetVisible(false);
	Parent::SetIsQuitting();
}

void FireManager::SetFade(float fade_amount) {
	(void)fade_amount;
}



PixelRect FireManager::GetRenderableArea() const {
	PixelRect _render_area;
	const int w = (int)(render_area_.GetHeight()/hp);
	_render_area.Set(render_area_.GetCenterX()-w/2, render_area_.top_, render_area_.GetCenterX()+w/2, render_area_.bottom_);
	_render_area.left_ = std::max(_render_area.left_, render_area_.left_);
	_render_area.right_ = std::min(_render_area.right_, render_area_.right_);
	return _render_area;
}

bool FireManager::Render() {
	PixelRect _render_area = GetRenderableArea();
	// If we're 1024x768 (iPad), we don't need to clear.
	const bool need_size_clear = (_render_area.GetWidth() <= render_area_.GetWidth()-1);
	const bool need_level_clear = (!level_ || !level_->IsLoaded());
	v_set(GetVariableScope(), kRtvarUi3DEnableclear, (need_size_clear||need_level_clear));

	const PixelRect full_render_area = render_area_;
	render_area_ = _render_area;
	bool _ok = Parent::Render();
	render_area_ = full_render_area;

	return _ok;
}

static bool SortVillainsPredicate(const FireManager::VillainPair& a, const FireManager::VillainPair& b) {
	return a.second.scale_ < b.second.scale_;
}

bool FireManager::Paint() {
	if (!Parent::Paint()) {
		return false;
	}

	if (menu_->GetDialog() || !level_ || !level_->IsLoaded()) {
		return true;	// Don't draw our terrorist indicators any more when the user is looking at a dialog.
	}

	uitbc::Painter* painter = ui_manager_->GetPainter();
	wstr score = wstrutil::Format(L"Score: %i/%i", kills_, kill_limit_);
	painter->SetColor(WHITE);
	painter->PrintText(score, 100, 21);

	const int unit = std::max(8, render_area_.GetHeight()/50);
	ui_manager_->SetScaleFont(-unit*1.4f);
	const int r = 5;
	VillainArray sorted_villains;
	sorted_villains.assign(villain_map_.begin(), villain_map_.end());
	std::sort(sorted_villains.begin(), sorted_villains.end(), SortVillainsPredicate);
	VillainArray::iterator x = sorted_villains.begin();
	for (; x != sorted_villains.end(); ++x) {
		if (x->second.time_ < kTargetInfoDelay) {
			continue;	// If we've unrolled the indicator completely, paint no more.
		}
		const PixelCoord xy = x->second.xy;
		const float time_part = (x->second.time_-kTargetInfoDelay)/kRollOutTime;
		const float arc_end_part = 0.3f;
		const float diagonal_end_part = 0.48f;
		const int arc_end = (int)std::min(360.0f, time_part/arc_end_part*360.0f);
		Color color;
		if (x->second.dangerousness_ > 0.5f) {
			color = Color(RED, DARK_RED, (x->second.dangerousness_-0.5f)*2);
		} else {
			color = Color(YELLOW, RED, x->second.dangerousness_/0.5f);
		}
		const int info_bubble_radius = 5;
		if (time_part > arc_end_part) {
			const float line_part = std::min(1.0f, (time_part-arc_end_part)/(diagonal_end_part-arc_end_part));
			const int s1 = (int)(r/1.41421356f);	// Diagonal radius and sqrt(2).
			const int axis_length = (int)((unit*2-s1-info_bubble_radius)*line_part);
			painter->SetColor(Color(30, 30, 30, 110));
			painter->SetAlphaValue(110);
			painter->SetRenderMode(uitbc::Painter::kRmAlphablend);
			painter->DrawLine(xy.x+s1, xy.y-s1+1, xy.x+s1+axis_length, xy.y-s1-1-axis_length);
			painter->SetColor(color);
			painter->SetRenderMode(uitbc::Painter::kRmNormal);
			painter->DrawLine(xy.x+s1, xy.y-s1+1, xy.x+s1+axis_length, xy.y-s1-1-axis_length);
		}
		painter->SetColor(Color(30, 30, 30, 150));
		painter->SetAlphaValue(150);
		painter->SetRenderMode(uitbc::Painter::kRmAlphablend);
		painter->DrawArc(xy.x-r+1, xy.y-r+1, r*2, r*2, 45, 45-arc_end, true);
		painter->SetColor(color);
		painter->SetRenderMode(uitbc::Painter::kRmNormal);
		painter->DrawArc(xy.x-r, xy.y-r, r*2, r*2, 45, 45-arc_end, true);
		if (time_part > diagonal_end_part) {
			// Cut using rect, so text will appear smoothly.
			const int r  = info_bubble_radius;
			const int xl = xy.x+unit*2-r;
			const int yl = xy.y-unit*2+r;
			const int yt = yl-painter->GetFontHeight()-2*r;
			const wstr villain = wstrutil::Encode(x->second.villain_);
			const int wl = painter->GetFontManager()->GetStringWidth(villain)+2+2*r;
			if (time_part < 1) {
				const float text_part = std::min(1.0f, (time_part-diagonal_end_part)/(1.0f-diagonal_end_part));
				PixelRect rect(xl-1, yt-1, xl+1+(int)(wl*text_part), yl+2);
				painter->SetClippingRect(rect);
			}
			painter->SetAlphaValue(150);
			painter->SetRenderMode(uitbc::Painter::kRmAlphablend);
			painter->DrawRoundedRect(PixelRect(xl, yt, xl+wl, yl), r, 0x7, true);
			painter->SetColor(Color(30, 30, 30, 150));
			painter->SetAlphaValue(150);
			painter->PrintText(villain, xl+r+2, yt+r+1);
			painter->SetRenderMode(uitbc::Painter::kRmNormal);
			painter->SetColor(WHITE);
			painter->PrintText(villain, xl+r+1, yt+r);
			if (time_part < 1) {
				painter->SetClippingRect(render_area_);	// Restore for next loop.
			}
		}
	}

	ui_manager_->SetMasterFont();
	return true;
}

void FireManager::DrawSyncDebugInfo() {
	PixelRect _render_area;
	const int w = (int)(render_area_.GetHeight()/hp);
	_render_area.Set(render_area_.GetCenterX()-w/2, render_area_.top_, render_area_.GetCenterX()+w/2, render_area_.bottom_);
	_render_area.left_ = std::max(_render_area.left_, render_area_.left_);
	_render_area.right_ = std::min(_render_area.right_, render_area_.right_);
	const PixelRect full_render_area = render_area_;
	render_area_ = _render_area;

	Parent::DrawSyncDebugInfo();

	ScopeLock lock(GetTickLock());
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

	render_area_ = full_render_area;
}



bool FireManager::IsObjectRelevant(const vec3& position, float distance) const {
	return (position.GetDistanceSquared(camera_transform_.GetPosition()) <= distance*distance);
}



void FireManager::Shoot(cure::ContextObject* avatar, int weapon) {
	(void)avatar;
	(void)weapon;

	double fire_delay;
	v_get(fire_delay, =, GetVariableScope(), kRtvarGameFiredelay, 1.5);
	if (!level_->IsLoaded() || fire_delay_timer_.QueryTimeDiff() < fire_delay) {
		return;
	}
	fire_delay_timer_.Start();
	vec3 target_position;
	if (GetPhysicsManager()->QueryRayCollisionAgainst(camera_transform_.GetPosition(), shoot_direction_, 1000.0f, level_->GetPhysics()->GetBoneGeometry(0)->GetBodyId(), &target_position, 1) < 1) {
		// User aiming above ground. Find vehicle closest to that position, and adjust target range thereafter.
		float _distance = 350.0;
		float closest_ray_distance2 = 150.0f * 150.0f;
		cure::ContextManager::ContextObjectTable object_table = GetContext()->GetObjectTable();
		cure::ContextManager::ContextObjectTable::iterator x = object_table.begin();
		for (; x != object_table.end(); ++x) {
			cure::ContextObject* _object = x->second;
			tbc::ChunkyPhysics* physics = _object->ContextObject::GetPhysics();
			if (!_object->IsLoaded() || !physics) {
				continue;
			}
			const vec3 vehicle_position = _object->GetPosition();
			if (vehicle_position.y < 30.0f || cure::Health::Get(_object) <= 0) {
				continue;
			}
			const vec3 ray_relative_position = vehicle_position.ProjectOntoPlane(shoot_direction_);
			const float distance2 = ray_relative_position.GetLengthSquared();
			if (distance2 < closest_ray_distance2) {
				closest_ray_distance2 = distance2;
				_distance = vehicle_position * shoot_direction_;
			}
		}
		target_position = shoot_direction_ * _distance;
	} else {
		cure::ContextObject* _object = Parent::CreateContextObject("indicator", cure::kNetworkObjectLocalOnly);
		_object->SetInitialTransform(xform(kIdentityQuaternionF, target_position));
		_object->StartLoading();
		GetContext()->DelayKillObject(_object, 1.5f);
	}

	life::Projectile* projectile = new life::Projectile(GetResourceManager(), "rocket", ui_manager_, this);
	projectile->EnableRootShadow(true);
	AddContextObject(projectile, cure::kNetworkObjectLocalOnly, 0);
	projectile->SetJetEngineEmitter(new UiCure::JetEngineEmitter(GetResourceManager(), ui_manager_));
	projectile->SetExhaustEmitter(new UiCure::ExhaustEmitter(GetResourceManager(), ui_manager_));
	xform t(camera_transform_);
	t.GetPosition().x += 0.7f;
	t.GetPosition().y += 1.0f;
	t.GetPosition().z += 0.1f;
	t.GetOrientation().RotateAroundWorldX(-PIF/2);	// Tilt rocket.
	float acceleration = 150;
	float terminal_velocity = 300;
	float gravity_effect = 1.15f;
	float aim_above = 1.5f;
	float some_rocket_length = 9.0;
	float up_down_effect = -0.1f;
	/*v_tryget(acceleration, =(float), GetVariableScope(), "shot.acceleration", 150.0);
	v_tryget(terminal_velocity, =(float), GetVariableScope(), "shot.terminalvelocity", 300.0);
	v_tryget(gravity_effect, =(float), GetVariableScope(), "shot.gravityeffect", 1.15);
	v_tryget(aim_above, =(float), GetVariableScope(), "shot.aimabove", 1.5);
	v_tryget(some_rocket_length, =(float), GetVariableScope(), "shot.rocketlength", 9.0);
	v_tryget(up_down_effect, =(float), GetVariableScope(), "shot.updowneffect", -0.1);*/
	vec3 _distance = target_position - t.GetPosition();
	aim_above = Math::Lerp(0.0f, aim_above, std::min(100.0f, _distance.GetLength())/100.0f);
	_distance.z += aim_above;
	const vec3 shoot_direction_euler_angles = life::ProjectileUtil::CalculateInitialProjectileDirection(_distance, acceleration, terminal_velocity, GetPhysicsManager()->GetGravity()*gravity_effect, up_down_effect);
	t.GetOrientation().RotateAroundWorldX(shoot_direction_euler_angles.y);
	t.GetOrientation().RotateAroundWorldZ(shoot_direction_euler_angles.x);
	t.position_.x -= some_rocket_length*sin(shoot_direction_euler_angles.x);
	t.position_.z += some_rocket_length*sin(shoot_direction_euler_angles.y);
	projectile->SetInitialTransform(t);
	projectile->StartLoading();
}

void FireManager::Detonate(cure::ContextObject* explosive, const tbc::ChunkyBoneGeometry* explosive_geometry, const vec3& position, const vec3& velocity, const vec3& normal, float strength) {
	float volume_factor = 1;
	const bool is_rocket = (explosive->GetClassId() == "rocket");
	if (is_rocket) {
		float explosive_strength;
		v_get(explosive_strength, =(float), GetVariableScope(), kRtvarGameExplosivestrength, 1.0);
		strength *= explosive_strength;
		v_set(GetVariableScope(), kRtvarGameExplosivestrength, 1.0);	// Reset to normal strength.
		volume_factor *= (explosive_strength>1)? 4 : 1;
	}
	const float cubic_strength = 4*(::pow(strength+1, 1/3.0f) - 1);	// Reduce by 3D volume. Explosion spreads in all directions.
	if (!is_rocket && !menu_->GetDialog()) {
		BaseMachine* machine = dynamic_cast<BaseMachine*>(explosive);
		machine->DeleteEngineSounds();	// Stop makin em.
		kills_ += machine->villain_.empty()? -1 : +1;
		++level_total_kills_;
		// Logic for showing super bomb icon.
		const int show_bomb_limit = 10 + GetCurrentLevelNumber();
		if (level_total_kills_%show_bomb_limit == 0 && kills_ < kill_limit_) {
			if (!bomb_button_->IsVisible()) {
				bomb_button_->SetVisible(true);
				UiCure::UserSound2dResource* sound = new UiCure::UserSound2dResource(ui_manager_, uilepra::SoundManager::kLoopNone);
				new UiCure::SoundReleaser(GetResourceManager(), ui_manager_, GetContext(), "great.wav", sound, 1, 1);
			}
		}
	}

	collision_sound_manager_->OnCollision(strength*volume_factor, position, explosive_geometry, "explosion");

	uitbc::ParticleRenderer* particle_renderer = (uitbc::ParticleRenderer*)ui_manager_->GetRenderer()->GetDynamicRenderer("particle");
	const float keep_on_going_factor = 0.5f;	// How much of the velocity energy, [0;1], should be transferred to the explosion particles.
	vec3 u = velocity.ProjectOntoPlane(normal) * (1+keep_on_going_factor);
	u -= velocity;	// Mirror and inverse.
	u.Normalize();
	const int particles = Math::Lerp(6, 10, cubic_strength * 0.3f);
	vec3 start_fire_color(1.0f, 1.0f, 0.6f);
	vec3 fire_color(0.6f, 0.4f, 0.2f);
	vec3 start_smoke_color(0.4f, 0.4f, 0.4f);
	vec3 smoke_color(0.2f, 0.2f, 0.2f);
	vec3 shrapnel_color(0.3f, 0.3f, 0.3f);	// Default debris color is gray.
	vec3 sprites_position(position*0.98f-position.GetNormalized(2.0f));	// We just move it closer to make it less likely to be cut off by ground.
	particle_renderer->CreateExplosion(sprites_position, cubic_strength, u, 1, 1.5f, start_fire_color, fire_color, start_smoke_color, smoke_color, shrapnel_color, particles, particles, particles/2, particles/2);

	// Slowmo check.
	bool normal_death = true;
	if (!is_rocket && cure::Health::Get(explosive) < -5500) {
		if (Random::Uniform(0.0f, 1.0f) > 0.7f) {
			normal_death = false;
			slowmo_timer_.TryStart();
		}
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
		float _force = life::Explosion::CalculateForce(physics_manager, _object, position, strength);
		if (_force > 0 && _object->GetNetworkObjectType() != cure::kNetworkObjectLocalOnly) {
			cure::FloatAttribute* health = cure::Health::GetAttribute(_object);
			if (health) {
				const float value = health->GetValue() - _force*Random::Normal(1.01f, 0.1f, 0.7f, 1.0f);
				health->SetValue(value);
			}
			x->second->ForceSend();
			life::Explosion::PushObject(physics_manager, _object, position, strength, GetTimeManager()->GetNormalFrameTime());
		}
		BaseMachine* machine = dynamic_cast<BaseMachine*>(_object);
		if (machine && machine->GetPosition().GetDistanceSquared(position) < 150*150) {
			machine->AddPanic((_force > 0.1f)? 1.0f : 0.45f);
		}
	}
}

void FireManager::OnBulletHit(cure::ContextObject* bullet, cure::ContextObject* hit_object) {
	(void)bullet;
	(void)hit_object;
}

void FireManager::OnLetThroughTerrorist(BaseMachine* terrorist) {
	(void)terrorist;
	if (menu_->GetDialog()) {
		return;
	}

	--kills_;
	UiCure::UserSound2dResource* sound = new UiCure::UserSound2dResource(ui_manager_, uilepra::SoundManager::kLoopNone);
	new UiCure::SoundReleaser(GetResourceManager(), ui_manager_, GetContext(), "bad.wav", sound, 1, 1);
}



bool FireManager::DidFinishLevel() {
	log_.Headlinef("Level %s done!", level_->GetClassId().c_str());
	return true;
}

str FireManager::StepLevel(int count) {
	kills_ = 0;
	level_total_kills_ = 0;
	int _level_number = GetCurrentLevelNumber();
	_level_number += count;
	str new_level_name = StoreLevelIndex(_level_number);
	GetContext()->PostKillObject(level_->GetInstanceId());
	{
		ScopeLock lock(GetTickLock());
		kill_limit_ = 4+4*_level_number;
		level_ = (Level*)Parent::CreateContextObject(new_level_name, cure::kNetworkObjectLocalOnly, 0);
		level_->StartLoading();
	}
	stepped_level_ = true;
	return new_level_name;
}

str FireManager::StoreLevelIndex(int level_number) {
	const int level_count = LEPRA_ARRAY_COUNT(kLevels);
	if (level_number < 0) {
		level_number = level_count-1;
	} else if (level_number >= level_count) {
		level_number = 0;
	}
	str new_level_name = strutil::Format("lvl%2.2i", level_number);
	v_set(GetVariableScope(), kRtvarGameStartlevel, new_level_name);
	return new_level_name;
}


Level* FireManager::GetLevel() const {
	if (level_ && level_->IsLoaded()) {
		return level_;
	}
	return 0;
}

int FireManager::GetCurrentLevelNumber() const {
	int _level_number = 0;
	strutil::StringToInt(level_->GetClassId().substr(3), _level_number);
	return _level_number;
}



cure::RuntimeVariableScope* FireManager::GetVariableScope() const {
	return (Parent::GetVariableScope());
}



bool FireManager::InitializeUniverse() {
	ui_manager_->GetRenderer()->SetLineWidth(1);
	ui_manager_->UpdateSettings();

	// Create dummy explosion to ensure all geometries loaded and ready, to avoid LAAAG when first exploading.
	uitbc::ParticleRenderer* particle_renderer = (uitbc::ParticleRenderer*)ui_manager_->GetRenderer()->GetDynamicRenderer("particle");
	const vec3 v;
	particle_renderer->CreateExplosion(vec3(0,0,-2000), 1, v, 1, 1, v, v, v, v, v, 1, 1, 1, 1);

	str start_level;
	v_get(start_level, =, GetVariableScope(), kRtvarGameStartlevel, "lvl00");
	{
		ScopeLock lock(GetTickLock());
		int level_index = 0;
		strutil::StringToInt(start_level.substr(3), level_index);
		kill_limit_ = 4+4*level_index;
		level_ = (Level*)Parent::CreateContextObject(start_level, cure::kNetworkObjectLocalOnly, 0);
		level_->StartLoading();
	}
	sunlight_ = new Sunlight(ui_manager_);
	return true;
}

void FireManager::ScriptPhysicsTick() {
	const float physics_time = GetTimeManager()->GetAffordedPhysicsTotalTime();
	if (physics_time > 1e-5) {
		MoveCamera();
		UpdateCameraPosition(false);
		HandleShooting();
		HandleTargets(physics_time);
	}

	if (kills_ >= kill_limit_) {
		CreateNextLevelDialog();
	}

	if (stepped_level_ && !GetResourceManager()->IsLoading()) {
		stepped_level_ = false;
		all_loaded_timer_.TryStart();
	}
	if (all_loaded_timer_.QuerySplitTime() > 10.0) {
		all_loaded_timer_.Stop();
		all_loaded_timer_.ClearTimeDiff();
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

	if (slowmo_timer_.IsStarted()) {
		if (slowmo_timer_.QueryTimeDiff() < 3.5f) {
			v_set(GetVariableScope(), kRtvarPhysicsRtr, 0.3);
		} else {
			v_set(GetVariableScope(), kRtvarPhysicsRtr, 1.0);
			slowmo_timer_.Stop();
		}
	}

	Parent::ScriptPhysicsTick();
}

void FireManager::HandleWorldBoundaries() {
	std::vector<cure::GameObjectId> lost_object_array;
	typedef cure::ContextManager::ContextObjectTable ContextTable;
	const ContextTable& object_table = GetContext()->GetObjectTable();
	ContextTable::const_iterator x = object_table.begin();
	for (; x != object_table.end(); ++x) {
		cure::ContextObject* _object = x->second;
		if (_object->IsLoaded() && _object->GetPhysics()) {
			const vec3 _position = _object->GetPosition();
			if (!Math::IsInRange(_position.x, -250.0f, +250.0f) ||
				!Math::IsInRange(_position.y, -100.0f, +550.0f) ||
				!Math::IsInRange(_position.z, -250.0f, +250.0f)) {
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

void FireManager::MoveCamera() {
}

void FireManager::UpdateCameraPosition(bool update_mic_position) {
	ui_manager_->SetCameraPosition(camera_transform_);
	if (update_mic_position) {
		ui_manager_->SetMicrophonePosition(camera_transform_, vec3());
	}
}

void FireManager::HandleShooting() {
	if (ui_manager_->CanRender()) {
		float fov;
		v_get(fov, =(float), GetVariableScope(), kRtvarUi3DFov, 38.8);
		UpdateFrustum(fov);
	}

	typedef uilepra::touch::DragManager::DragList DragList;
	DragList drag_list = ui_manager_->GetDragManager()->GetDragList();
	for (DragList::iterator x = drag_list.begin(); x != drag_list.end(); ++x) {
		if (x->is_press_ && (x->flags_&DRAG_FLAG) == 0 && !menu_->GetDialog()) {
			x->flags_ |= DRAG_FLAG;
			shoot_direction_ = ui_manager_->GetRenderer()->ScreenCoordToVector(x->last_);
			Shoot(0, 0);
		}
	}
}

void FireManager::HandleTargets(float time) {
	VillainMap::iterator y = villain_map_.begin();
	for (; y != villain_map_.end(); ++y) {
		y->second.is_active_ = false;
	}

	PixelRect _render_area = GetRenderableArea();
	_render_area.left_ -= 30;	// Don't drop target info indicator just because the car wobbles slightly off left side of screen.
	cure::ContextManager::ContextObjectTable object_table = GetContext()->GetObjectTable();
	cure::ContextManager::ContextObjectTable::iterator x = object_table.begin();
	for (; x != object_table.end(); ++x) {
		cure::ContextObject* _object = x->second;
		if (_object->GetClassId().find("lvl") == 0 || _object->GetClassId() == "indicator") {
			continue;
		}
		BaseMachine* machine = dynamic_cast<BaseMachine*>(_object);
		if (!machine || machine->villain_.empty()) {
			continue;
		}
		const vec3 _position = machine->GetPosition();
		if (_position.y < 35 || _position.y > 350.0f) {
			continue;
		}
		const vec2 coord = ui_manager_->GetRenderer()->PositionToScreenCoord(_position, 0);
		const PixelCoord xy((int)coord.x, (int)coord.y);
		if (!_render_area.IsInside(xy.x, xy.y)) {
			continue;
		}
		const float scale = std::min(1.0f, 100.0f/_position.y);
		TargetInfo target_info(machine->villain_, xy, machine->dangerousness_, scale);
		VillainMap::iterator y = villain_map_.find(machine);
		if (y == villain_map_.end()) {
			villain_map_.insert(VillainMap::value_type(machine, target_info));
		} else {
			y->second.xy = xy;
			y->second.is_active_ = true;
			y->second.scale_ = scale;
			if (cure::Health::Get(machine) > 0) {
				y->second.time_ += time;
			} else {
				if (y->second.time_ > kTargetInfoDelay+kRollOutTime) {
					y->second.time_ = kTargetInfoDelay+kRollOutTime;
				}
				y->second.time_ -= time;
			}
		}
	}
	y = villain_map_.begin();
	while (y != villain_map_.end()) {
		if (!y->second.is_active_) {
			y = villain_map_.erase(y);
		} else {
			++y;
		}
	}

}



void FireManager::TickInput() {
	OnLevelLoadCompleted();
	TickNetworkInput();
	TickUiInput();
}

void FireManager::TickUiInput() {
	ui_manager_->GetInputManager()->SetCursorVisible(true);

	const int physics_step_count = GetTimeManager()->GetAffordedPhysicsStepCount();
	if (physics_step_count > 0 && allow_movement_input_) {
	}
}

void FireManager::TickUiUpdate() {
	((FireConsoleManager*)GetConsoleManager())->GetUiConsole()->Tick();
	collision_sound_manager_->Tick(camera_transform_.GetPosition());
}

void FireManager::SetLocalRender(bool render) {
	(void)render;
}



cure::ContextObject* FireManager::CreateContextObject(const str& class_id) const {
	cure::CppContextObject* _object;
	if (strutil::StartsWith(class_id, "lvl")) {
		UiCure::GravelEmitter* gravel_particle_emitter = new UiCure::GravelEmitter(GetResourceManager(), ui_manager_, 0.5f, 1, 10, 2);
		Level* level = new Level(GetResourceManager(), class_id, ui_manager_, gravel_particle_emitter);
		level->EnableRootShadow(false);
		_object = level;
	} else if (strutil::StartsWith(class_id, "indicator")) {
		_object = new UiCure::Machine(GetResourceManager(), class_id, ui_manager_);
	} else {
		BaseMachine* machine = new BaseMachine(GetResourceManager(), class_id, ui_manager_, (FireManager*)this);
		machine->level_speed_ = level_->GetLevelSpeed();
		//machine->SetExhaustEmitter(new UiCure::ExhaustEmitter(GetResourceManager(), ui_manager_));
		machine->SetBurnEmitter(new UiCure::BurnEmitter(GetResourceManager(), ui_manager_));
		//machine->GetBurnEmitter()->SetFreeFlow();
		machine->SetExplosiveStrength(0.6f);
		const int c = LEPRA_ARRAY_COUNT(kVillainTypes);
		if (g_personality_count <= 0) {
			g_personality_count = 0;
			for (int x = 0; x < c; ++x) {
				g_personality_count += kVillainTypes[x].count_;
			}
			g_personality_count = (int)(g_personality_count/(1-PERSONS_INNOCENT_PART));
		}
		int r = (int)Random::Uniform(0.0f, (float)g_personality_count+1);
		for (int x = 0; x < c; ++x) {
			r -= kVillainTypes[x].count_;
			if (r <= 0) {
				machine->villain_ = kVillainTypes[x].name_;
				machine->dangerousness_ = 1-(float)x/(c-1);
				break;
			}
		}
		_object = machine;
	}
	_object->SetAllowNetworkLogic(true);
	return (_object);
}

cure::ContextObject* FireManager::CreateLogicHandler(const str& type) {
	if (type == "spawner" || type == "spawner_init") {
		return new life::Spawner(GetContext());
	} else if (type == "eater") {
		return new Eater(GetContext());
	} else if (type == "context_path") {
		return level_->QueryPath();
	}
	return (0);
}

void FireManager::OnLoadCompleted(cure::ContextObject* object, bool ok) {
	if (ok) {
		if (object == level_) {
			OnLevelLoadCompleted();
		} else if (strutil::StartsWith(object->GetClassId(), "rocket")) {
			object->SetEnginePower(0, 1.0f);
			//object->SetEnginePower(2, 1.0f);
		} else if (strutil::StartsWith(object->GetClassId(), "indicator")) {
		} else {
			new cure::FloatAttribute(object, "float_childishness", 1);
			new AutoPathDriver(this, object->GetInstanceId(), "input");
			vec3 color = RNDPOSVEC();
			life::ExplodingMachine* machine = (life::ExplodingMachine*)object;
			machine->GetMesh(0)->GetBasicMaterialSettings().diffuse_ = color;
		}
		log_volatile(log_.Tracef("Loaded object %s.", object->GetClassId().c_str()));
		object->GetPhysics()->UpdateBonesObjectTransformation(0, kIdentityTransformationF);
		((UiCure::CppContextObject*)object)->UiMove();
	} else {
		log_.Errorf("Could not load object of type %s.", object->GetClassId().c_str());
		GetContext()->PostKillObject(object->GetInstanceId());
	}
}

void FireManager::OnLevelLoadCompleted() {
	if (!level_ || !level_->IsLoaded()) {
		return;
	}
	// Update texture UV coordinates according to FoV.
	uitbc::Renderer* renderer = ui_manager_->GetRenderer();
	float fov;
	v_get(fov, =(float), GetVariableScope(), kRtvarUi3DFov, 38.8);
	static float former_fo_v = 0;
	static unsigned former_level_id = 0;
	if (former_fo_v == fov && former_level_id == level_->GetInstanceId()) {
		return;
	}
	log_.Headlinef("Level %s loaded.", level_->GetClassId().c_str());
	bool first_run;
	v_get(first_run, =, GetVariableScope(), kRtvarGameFirstrun, false);
	if (first_run) {
		CreateNextLevelDialog();
	}
	renderer->ResetClippingRect();
	former_fo_v = fov;
	former_level_id = level_->GetInstanceId();
	const float wf = +1.0f / ui_manager_->GetDisplayManager()->GetWidth();
	const float hf = hp / ui_manager_->GetDisplayManager()->GetHeight();
	const size_t mesh_count = ((uitbc::ChunkyClass*)level_->GetClass())->GetMeshCount();
	for (size_t x = 0; x < mesh_count; ++x) {
		tbc::GeometryReference* mesh = (tbc::GeometryReference*)level_->GetMesh(x);
		xform transform = mesh->GetTransformation();
		if (mesh->GetUVSetCount()) {
			uitbc::TriangleBasedGeometry* parent = (uitbc::TriangleBasedGeometry*)mesh->GetParentGeometry();
			deb_assert(parent->GetUVCountPerVertex() == 4);
			const float* xyz = mesh->GetVertexData();
			const unsigned vertex_count = mesh->GetVertexCount();
			float* uvst = new float[vertex_count*4];
			for (unsigned z = 0; z < vertex_count; ++z) {
				vec3 __vector(&xyz[z*3]);
				__vector = transform.Transform(__vector);
				vec2 c = renderer->PositionToScreenCoord(__vector, 1/hp);
				const float x = c.x * wf;
				const float y = c.y * hf;
				uvst[z*4+0] = x*__vector.y;
				uvst[z*4+1] = y*__vector.y;
				uvst[z*4+2] = 0;
				uvst[z*4+3] = __vector.y;
			}
			parent->PopUVSet();
			parent->AddUVSet(uvst);
			parent->SetUVDataChanged(true);
			mesh->SetUVDataChanged(true);

			mesh->SetPreRenderCallback(tbc::GeometryBase::PreRenderCallback(this, &FireManager::DisableAmbient));
			mesh->SetPostRenderCallback(tbc::GeometryBase::PostRenderCallback(this, &FireManager::EnableAmbient));

			ui_manager_->GetRenderer()->UpdateGeometry(((UiCure::GeometryReferenceResource*)level_->GetMeshResource(x)->GetConstResource())->GetParent()->GetData(), false);

			delete[] uvst;
		}
	}
}

void FireManager::OnCollision(const vec3& force, const vec3& torque, const vec3& position,
	cure::ContextObject* object1, cure::ContextObject* object2,
	tbc::PhysicsManager::BodyID body1_id, tbc::PhysicsManager::BodyID body2_id) {
	(void)body2_id;
	collision_sound_manager_->OnCollision(force, torque, position, object1, object2, body1_id, 5000, false);

	BaseMachine* machine1 = dynamic_cast<BaseMachine*>(object1);
	BaseMachine* machine2 = dynamic_cast<BaseMachine*>(object2);
	if (machine1 && machine2) {
		const vec3 v = (machine2->GetPosition() - machine1->GetPosition());
		if (machine1->GetForwardDirection()*v > 0.1f) {
			machine2->AddPanic(1.5f);	// 1 drives into 2.
		}
		if (machine2->GetForwardDirection()*(-v) > 0.1f) {
			machine1->AddPanic(1.5f);	// 2 drives into 1.
		}
	}
}



void FireManager::OnBombButton(uitbc::Button* button) {
	button->SetVisible(false);
	v_set(GetVariableScope(), kRtvarGameExplosivestrength, 10.0);
}

void FireManager::OnPauseButton(uitbc::Button* button) {
	if (button) {
		menu_->OnTapSound(button);
		button->SetVisible(false);
	}

	uitbc::Dialog* d = menu_->CreateTbcDialog(life::Menu::ButtonAction(this, &FireManager::OnMenuAlternative), 0.5f, 0.5f);
	d->SetColor(BG_COLOR, OFF_BLACK, BLACK, BLACK);
	d->SetDirection(+1, false);

	uitbc::FixedLayouter layouter(d);
	layouter.SetContentYMargin(d->GetPreferredHeight()/10);

	uitbc::Button* restart_button = new uitbc::Button(Color(90, 10, 10), L"Reset game");
	layouter.AddButton(restart_button, -8, 0, 2, 0, 1, 1, true);

	uitbc::Button* restart_level_button = new uitbc::Button(Color(10, 90, 10), L"Restart level");
	layouter.AddButton(restart_level_button, -4, 1, 2, 0, 1, 1, true);

	uitbc::Button* close_button = new uitbc::Button(Color(180, 60, 50), L"X");
	layouter.AddCornerButton(close_button, -9);

	v_set(GetVariableScope(), kRtvarPhysicsHalt, true);
}

void FireManager::CreateNextLevelDialog() {
	if (menu_->GetDialog()) {
		return;
	}
	pause_button_->SetVisible(false);

	const int finished_level = GetCurrentLevelNumber();
	StoreLevelIndex(finished_level+1);

	uitbc::Dialog* d = menu_->CreateTbcDialog(life::Menu::ButtonAction(this, &FireManager::OnMenuAlternative), 0.8f, 0.4f);
	d->SetColor(BG_COLOR, OFF_BLACK, BLACK, BLACK);
	d->SetDirection(+1, false);

	uitbc::FixedLayouter layouter(d);
	layouter.SetContentWidthPart(0.85f);

	static const char* congratulations[] =
	{
		"Well done, agent!\n\nPrepare to protect other people in other parts of the world.",
		"Great Scott; you are good at this!\n\nRemember to relax between wet jobs.",
		"We sure are lucky to have you on our side.\n\nField work sure beats the office, huh?",
		"On behalf of all the people in the world:\n\nTHANK YOU!!!",
		"There might be a bug in our terrorist ID software.\nPlease don't worry about it while we remedy.\n\nYou should go on a mission now.",
		"They software guys say the bug might be fixed.\n\nGo kill!",
		"Everybody dies, but it's nice to see the bad ones go first.\n\nHead out agent!",
		"Rooting out vermin is your cup of tea.\n\nI'm glad that you are not in working in pesticides.",
		"You are the pride and joy of our agency.\n\nOh and btw: the President sends his gratitude!",
		"Your persistency must be admired.\n\nThe last guy quit after just a week!",
		"Your next assignment is... Haha! Just kiddin'!\nWho cares where you go when there are big guns at your disposal\nand a lot of bad people at the other end of the barrel?",
		"An awful lot of bad guys out there.\n\nGood work, agent!",
		"A little collateral is not a problem.\nI mean, it's like fishing: to exterminate the big\ncatch you've gotta kill innocent fish babies.",
		"There are almost no terrorists left in the world!\n\nHumanity is relying on you.",
		"You've done it, the world is cleansed!\n\nHowever, disturbing reports on the outskirts of\nyour home town tells me you should go there again.",
	};
	deb_assert(LEPRA_ARRAY_COUNT(congratulations) == LEPRA_ARRAY_COUNT(kLevels));
	deb_assert(finished_level < LEPRA_ARRAY_COUNT(congratulations));
	str congrats = congratulations[finished_level];
	bool first_run;
	v_get(first_run, =, GetVariableScope(), kRtvarGameFirstrun, false);
	if (first_run) {
		v_set(GetVariableScope(), kRtvarGameFirstrun, false);
		congrats = "Our patented EnemyVisionGoggles(r) indicates villains.\nAvoid collateral damage, when possible.\n\nGood luck agent!";
	} else {
		UiCure::UserSound2dResource* sound = new UiCure::UserSound2dResource(ui_manager_, uilepra::SoundManager::kLoopNone);
		new UiCure::SoundReleaser(GetResourceManager(), ui_manager_, GetContext(), "level_done.wav", sound, 1, 1);
	}
	uitbc::Label* label = new uitbc::Label(LIGHT_GRAY, wstrutil::Encode(congrats));
	label->SetFontId(ui_manager_->SetScaleFont(std::min(-14.0f, d->GetPreferredHeight()/-14.0f)));
	ui_manager_->SetMasterFont();
	//label->SetIcon(uitbc::Painter::kInvalidImageid, uitbc::TextComponent::kIconCenter);
	label->SetAdaptive(false);
	layouter.AddComponent(label, 0, 2, 0, 1, 1);

	if (first_run) {
		uitbc::Button* ok_button = new uitbc::Button(Color(10, 90, 10), L"OK");
		ok_button->SetFontId(label->GetFontId());
		layouter.AddButton(ok_button, -5, 3, 4, 4, 3, 7, true);
	} else {
		uitbc::Button* next_level_button = new uitbc::Button(Color(10, 90, 10), L"Next level");
		next_level_button->SetFontId(label->GetFontId());
		layouter.AddButton(next_level_button, -7, 3, 4, 4, 3, 7, true);

		uitbc::Button* restart_button = new uitbc::Button(Color(90, 10, 10), L"Restart from level 1");
		restart_button->SetFontId(label->GetFontId());
		layouter.AddButton(restart_button, -8, 3, 4, 0, 3, 7, true);
	}
}

void FireManager::OnMenuAlternative(uitbc::Button* button) {
	if (button->GetTag() == -5) {
		pause_button_->SetVisible(true);
	} else if (button->GetTag() == -6) {
		double bedside_volume = (button->GetState() == uitbc::Button::kPressed)? 0.02 : 1.0;
		v_set(GetVariableScope(), kRtvarUiSoundMastervolume, bedside_volume);
		ui_manager_->GetSoundManager()->SetMasterVolume((float)bedside_volume);	// Set right away for button volume.
	} else if (button->GetTag() == -4) {
		pause_button_->SetVisible(true);
		GetConsoleManager()->PushYieldCommand(strutil::Format("set-level-index %i", GetCurrentLevelNumber()));
		menu_->DismissDialog();
		HiResTimer::StepCounterShadow();
		v_set(GetVariableScope(), kRtvarPhysicsHalt, false);
	} else if (button->GetTag() == -7) {
		pause_button_->SetVisible(true);
		GetConsoleManager()->PushYieldCommand(strutil::Format("set-level-index %i", GetCurrentLevelNumber()+1));
		menu_->DismissDialog();
		HiResTimer::StepCounterShadow();
		v_set(GetVariableScope(), kRtvarPhysicsHalt, false);
	} else if (button->GetTag() == -8) {
		pause_button_->SetVisible(true);
		GetConsoleManager()->PushYieldCommand("set-level-index 0");
		menu_->DismissDialog();
		HiResTimer::StepCounterShadow();
		v_set(GetVariableScope(), kRtvarPhysicsHalt, false);
	} else if (button->GetTag() == -9) {
		pause_button_->SetVisible(true);
		HiResTimer::StepCounterShadow();
		v_set(GetVariableScope(), kRtvarPhysicsHalt, false);
	}
}



void FireManager::PainterImageLoadCallback(UiCure::UserPainterKeepImageResource* resource) {
	if (resource->GetLoadState() == cure::kResourceLoadComplete) {
		ui_manager_->GetDesktopWindow()->GetImageManager()->AddLoadedImage(*resource->GetRamData(), resource->GetData(),
			uitbc::GUIImageManager::kCentered, uitbc::GUIImageManager::kAlphablend, 255);
	}
}



bool FireManager::DisableAmbient() {
	uitbc::Renderer* renderer = ui_manager_->GetRenderer();
	store_lights_enabled_ = renderer->GetLightsEnabled();
	if (store_lights_enabled_) {
		renderer->GetAmbientLight(store_ambient_.x, store_ambient_.y, store_ambient_.z);
		renderer->SetAmbientLight(1.0f, 1.0f, 1.0f);
		renderer->EnableAllLights(false);
	}
	//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	return true;
}

void FireManager::EnableAmbient() {
	uitbc::Renderer* renderer = ui_manager_->GetRenderer();
	if (store_lights_enabled_) {
		renderer->EnableAllLights(true);
		renderer->SetAmbientLight(store_ambient_.x, store_ambient_.y, store_ambient_.z);
	}
	//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
}



loginstance(kGame, FireManager);



}
