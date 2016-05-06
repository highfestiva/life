
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include <algorithm>
#include "impuzzablemanager.h"
#include "../cure/include/contextmanager.h"
#include "../cure/include/timemanager.h"
#include "../lepra/include/cyclicarray.h"
#include "../lepra/include/unordered.h"
#include "../uicure/include/uicollisionsoundmanager.h"
#include "../uicure/include/uigameuimanager.h"
#include "../uicure/include/uiiconbutton.h"
#include "../uicure/include/uimachine.h"
#include "../uicure/include/uisoundreleaser.h"
#include "../uilepra/include/uitouchdrag.h"
#include "../uitbc/include/gui/uidesktopwindow.h"
#include "../uitbc/include/gui/uifixedlayouter.h"
#include "../uitbc/include/uimaterial.h"
#include "../uitbc/include/uiparticlerenderer.h"
#include "../uitbc/include/uitrianglebasedgeometry.h"
#include "../lepra/include/random.h"
#include "../life/lifeclient/uiconsole.h"
#include "piece.h"
#include "impuzzable.h"
#include "impuzzableconsolemanager.h"
#include "level.h"
#include "rtvar.h"
#include "sunlight.h"
#include "version.h"

#define BG_COLOR		Color(5, 10, 15, 230)
#define CAM_DISTANCE		7.0f
#define PIECE_RADIUS		0.15f
#define DRAG_FLAG_INVALID	1
#define DRAG_FLAG_STARTED	2
#define CLOSE_NORMAL		5e-5
#define SAME_NORMAL		(1-CLOSE_NORMAL)
#define NGON_INDEX(i)		(((int)i<0)? cnt-1 : (i>=(int)cnt)? 0 : i)
#define LEVEL_DONE()		(percent_done_ >= 85)
#define BRIGHT_TEXT		Color(220, 215, 205)
#define DIM_TEXT		Color(200, 190, 180)
#define DIM_RED_TEXT		Color(240, 80, 80)
#define DIM_RED			Color(180, 60, 50)
#define GREEN_BUTTON		Color(20, 190, 15)
#define ORANGE_BUTTON		Color(220, 110, 20)
#define RED_BUTTON		Color(210, 40, 30)
#define BLACK_BUTTON		DARK_GRAY


namespace Impuzzable {



struct PieceDistanceAscending {
	vec3 cam_pos_;
	PieceDistanceAscending(vec3 cam_pos): cam_pos_(cam_pos) {}
	bool operator() (const Piece* a, const Piece* b) { return cam_pos_.GetDistance(a->GetPosition()) < cam_pos_.GetDistance(b->GetPosition()); }
};

void Impuzzable__ShowAd();
void Impuzzable__Buy();



ImpuzzableManager::ImpuzzableManager(life::GameClientMasterTicker* pMaster, const cure::TimeManager* time,
	cure::RuntimeVariableScope* variable_scope, cure::ResourceManager* resource_manager,
	UiCure::GameUiManager* ui_manager, int slave_index, const PixelRect& render_area):
	Parent(pMaster, time, variable_scope, resource_manager, ui_manager, slave_index, render_area),
	collision_sound_manager_(0),
	level_(0),
	force_cut_window_(false),
	menu_(0),
	sunlight_(0),
	camera_angle_(0),
	camera_rotate_speed_(1.0f),
	camera_transform_(quat(), vec3(0, -CAM_DISTANCE, 0)),
	level_completed_(false),
	pause_button_(0),
	is_cutting_(false),
	is_shaking_(false),
	cuts_left_(1),
	shakes_left_(1),
	last_cut_mode_(kCutNormal),
	cut_sound_pitch_(1),
	quick_cut_count_(0),
	level_score_(0),
	shake_sound_(0) {
	collision_sound_manager_ = new UiCure::CollisionSoundManager(this, ui_manager);
	collision_sound_manager_->AddSound("explosion",	UiCure::CollisionSoundManager::SoundResourceInfo(0.8f, 0.4f, 0));
	collision_sound_manager_->AddSound("rubber",		UiCure::CollisionSoundManager::SoundResourceInfo(1.0f, 0.5f, 0));
	collision_sound_manager_->PreLoadSound("explosion");

	SetConsoleManager(new ImpuzzableConsoleManager(GetResourceManager(), this, ui_manager_, GetVariableScope(), render_area_));

	GetPhysicsManager()->SetSimulationParameters(0, 0.01f, 0.2f);

	v_set(GetVariableScope(), kRtvarGameFirsttime, true);
	v_set(GetVariableScope(), kRtvarGameLevel, 0);
	v_set(GetVariableScope(), kRtvarGameLevelshapealternate, false);
	v_set(GetVariableScope(), kRtvarGameRunads, true);
	v_set(GetVariableScope(), kRtvarGameScore, 0.0);
	v_set(GetVariableScope(), kRtvarUiSoundMastervolume, 1.0);
	/*
#define RNDMZEL \
	for (int x = 0; x < 20; ++x) \
	{ \
		size_t a = Random::GetRandomNumber() % n_gon.size(); \
		size_t b = Random::GetRandomNumber() % n_gon.size(); \
		if (a != b) \
			std::swap(n_gon[1], n_gon[b]); \
	}
	std::vector<vec3> n_gon;
#define CLRL \
	n_gon.clear(); \
	n_gon.push_back(vec3(-2.1314f, 0, -0.333f));	\
	for (int x = 0; x < 10; ++x)	\
		n_gon.push_back(n_gon[n_gon.size()-1] + vec3(1,0,0.14f)*Random::Uniform(0.001f, 0.1f));	\
	for (int x = 0; x < 10; ++x)	\
		n_gon.push_back(n_gon[n_gon.size()-1] + vec3(0.14f,0,-1)*Random::Uniform(0.001f, 0.1f));	\
	for (int x = 0; x < 10; ++x)	\
		n_gon.push_back(n_gon[n_gon.size()-1] + vec3(-0.34f,0,-1)*Random::Uniform(0.001f, 0.1f));	\
	for (int x = 0; x < 10; ++x)	\
		n_gon.push_back(n_gon[n_gon.size()-1] + vec3(-1,0,-0.14f)*Random::Uniform(0.001f, 0.1f));
	for (int y = 0; y < 100; ++y) {
		CLRL;
		RNDMZEL;
		std::vector<vec3> copy(n_gon);
		CreateNGon(n_gon);
		log_.Infof("----------");
		for (int x = 0; x < (int)n_gon.size(); ++x) {
			log_.Infof("%f;%f;", n_gon[x].x, n_gon[x].z);
		}
		deb_assert(n_gon.size() == 5);
	}*/
}

ImpuzzableManager::~ImpuzzableManager() {
	Close();

	delete collision_sound_manager_;
	collision_sound_manager_ = 0;
}

void ImpuzzableManager::Suspend() {
	if (!menu_->GetDialog()) {
		pause_button_->SetVisible(false);
		OnPauseButton(0);
	}
}

void ImpuzzableManager::LoadSettings() {
	v_set(GetVariableScope(), kRtvarGameSpawnpart, 1.0);

	Parent::LoadSettings();

	v_set(GetVariableScope(), kRtvarUi2DFont, "Verdana");
	v_set(GetVariableScope(), kRtvarUi2DFontflags, 0);
	v_set(GetVariableScope(), kRtvarUi3DFov, 20.0);
	v_set(GetVariableScope(), kRtvarPhysicsMicrosteps, 3);
	v_set(GetVariableScope(), kRtvarPhysicsNoclip, false);

	GetConsoleManager()->ExecuteCommand("bind-key F2 prev-level");
	GetConsoleManager()->ExecuteCommand("bind-key F3 next-level");
}

void ImpuzzableManager::SaveSettings() {
	GetConsoleManager()->ExecuteCommand("save-application-config-file "+GetApplicationCommandFilename());
}

bool ImpuzzableManager::Open() {
	bool __ok = Parent::Open();
	if (__ok) {
		pause_button_ = ICONBTNA("btn_pause.png", "");
		int x = render_area_.left_ + 12;
		int y = render_area_.bottom_ - 12 - 32;
		ui_manager_->GetDesktopWindow()->AddChild(pause_button_, x, y);
		pause_button_->SetVisible(true);
		pause_button_->SetOnClick(ImpuzzableManager, OnPauseButton);
	}
	if (__ok) {
		menu_ = new life::Menu(ui_manager_, GetResourceManager());
		menu_->SetButtonTapSound("tap.wav", 0.2f, 0.05f);
	}
	return __ok;
}

void ImpuzzableManager::Close() {
	ScopeLock lock(GetTickLock());
	delete pause_button_;
	pause_button_ = 0;
	delete menu_;
	menu_ = 0;
	if (sunlight_) {
		delete sunlight_;
		sunlight_ = 0;
	}
	Parent::Close();
}

void ImpuzzableManager::SetIsQuitting() {
	((ImpuzzableConsoleManager*)GetConsoleManager())->GetUiConsole()->SetVisible(false);
	Parent::SetIsQuitting();
}

void ImpuzzableManager::SetFade(float fade_amount) {
	(void)fade_amount;
}



bool ImpuzzableManager::Render() {
	if (!is_shaking_ && shake_timer_.QuerySplitTime() < 4.0) {
		sunlight_->Tick(camera_transform_.GetOrientation());
	}

	RenderBackground();

	bool ok = Parent::Render();
	if (level_) {
		ui_manager_->GetPainter()->SetLineWidth(1);
		level_->RenderOutline();
	}
	return ok;
}

void ImpuzzableManager::RenderBackground() {
	if (!ui_manager_->CanRender()) {
		return;
	}

	ui_manager_->GetPainter()->PrePaint(false);
	ui_manager_->GetPainter()->ResetClippingRect();
	ui_manager_->GetRenderer()->SetDepthWriteEnabled(false);
	ui_manager_->GetRenderer()->SetDepthTestEnabled(false);
	ui_manager_->GetRenderer()->SetLightsEnabled(false);
	ui_manager_->GetRenderer()->SetTexturingEnabled(false);
	vec3 top_color(84,217,245);
	vec3 bottom_color(12,19,87);
	static float _angle = 0;
	vec3 device_acceleration(ui_manager_->GetAccelerometer());
	float acceleration = device_acceleration.GetLength();
	float screen_angle = 0;
	if (acceleration > 0.7f && std::abs(device_acceleration.y) < 0.7f*acceleration) {
		vec2 screen_direction(device_acceleration.x, device_acceleration.z);
		screen_direction.Normalize();
		screen_angle = -screen_direction.GetAngle() - PIF/2;
	}
	Math::RangeAngles(_angle, screen_angle);
	_angle = Math::Lerp(_angle, screen_angle, 0.1f);
	float x = ui_manager_->GetCanvas()->GetWidth() / 2.0f;
	float y = ui_manager_->GetCanvas()->GetHeight() / 2.0f;
	float l = ::sqrt(x*x + y*y);
	x /= l;
	y /= l;
	vec2 tl(-x,+y);
	vec2 tr(+x,+y);
	vec2 bl(-x,-y);
	vec2 br(+x,-y);
	tl.RotateAround(vec2(), _angle);
	tr.RotateAround(vec2(), _angle);
	bl.RotateAround(vec2(), _angle);
	br.RotateAround(vec2(), _angle);
	vec3 tlc = Math::Lerp(bottom_color, top_color, tl.y*0.5f+0.5f)/255;
	vec3 trc = Math::Lerp(bottom_color, top_color, tr.y*0.5f+0.5f)/255;
	vec3 blc = Math::Lerp(bottom_color, top_color, bl.y*0.5f+0.5f)/255;
	vec3 brc = Math::Lerp(bottom_color, top_color, br.y*0.5f+0.5f)/255;
	ui_manager_->GetPainter()->SetColor(Color::CreateColor(tlc.x, tlc.y, tlc.z, 1), 0);
	ui_manager_->GetPainter()->SetColor(Color::CreateColor(trc.x, trc.y, trc.z, 1), 1);
	ui_manager_->GetPainter()->SetColor(Color::CreateColor(brc.x, brc.y, brc.z, 1), 2);
	ui_manager_->GetPainter()->SetColor(Color::CreateColor(blc.x, blc.y, blc.z, 1), 3);
	ui_manager_->GetPainter()->FillShadedRect(0, 0, ui_manager_->GetCanvas()->GetWidth(), ui_manager_->GetCanvas()->GetHeight());
	ui_manager_->GetRenderer()->SetDepthWriteEnabled(true);
	ui_manager_->GetRenderer()->SetDepthTestEnabled(true);
	ui_manager_->GetRenderer()->SetLightsEnabled(true);
	ui_manager_->GetRenderer()->Renderer::SetTexturingEnabled(true);
}

bool ImpuzzableManager::Paint() {
	ui_manager_->GetPainter()->SetLineWidth(3);
	if (!Parent::Paint()) {
		return false;
	}
	std::vector<Piece*>::iterator x;
	for (x = pieces_.begin(); x != pieces_.end(); ++x) {
		Piece* _piece = *x;
		if (!_piece->IsDragging()) {
			continue;
		}
		const vec3 p = _piece->GetMoveTarget();
		const vec2 c = ui_manager_->GetRenderer()->PositionToScreenCoord(p, 0);
		ui_manager_->GetPainter()->DrawArc((int)c.x, (int)c.y, 10, 10, 0, 360, true);
	}
	return true;
}

void ImpuzzableManager::HandleDrag() {
	if (menu_->GetDialog()) {
		return;
	}
	const int w = ui_manager_->GetCanvas()->GetWidth();
	const float touch_side_scale = 1.28f;	// Inches.
	const float touch_scale = touch_side_scale / (float)ui_manager_->GetDisplayManager()->GetPhysicalScreenSize();
	const float resolution_margin = w / 50.0f;
	const int m = (int)Math::Lerp(touch_scale * w * 0.25f, resolution_margin, 0.7f);
	const int r = m-2;
	typedef uilepra::touch::DragManager::DragList DragList;
	DragList drag_list = ui_manager_->GetDragManager()->GetDragList();
	for (DragList::iterator x = drag_list.begin(); x != drag_list.end(); ++x) {
		Piece* _piece = 0;
		if (x->is_new_) {
			_piece = PickPiece(*x, r);
		} else {
			_piece = GetDraggedPiece(*x);
		}
		if (!_piece) {
			continue;
		}
		if (!x->is_press_) {
			x->extra_ = 0;
			_piece->SetDragging(false, CAM_DISTANCE);
		} else {
			DragPiece(_piece, x->last_);
		}
	}
}

Piece* ImpuzzableManager::PickPiece(uilepra::touch::Drag& drag, int radius) {
	const vec3 v = ui_manager_->GetRenderer()->ScreenCoordToVector(drag.start_);
	tbc::PhysicsManager* physics = GetPhysicsManager();
	std::vector<Piece*>::iterator x;
	vec3 hits[4];
	std::vector<Piece*> tapped_pieces;
	typedef std::vector<tbc::PhysicsManager::BodyID> BodyArray;
	BodyArray bodies;
	for (x = pieces_.begin(); x != pieces_.end(); ++x) {
		Piece* _piece = *x;
		if (_piece->IsDragging()) {
			continue;
		}
		int hit_count = 0;
		bodies.clear();
		_piece->GetBodyIds(bodies);
		for (BodyArray::iterator y = bodies.begin(); y != bodies.end(); ++y) {
			hit_count = physics->QueryRayCollisionAgainst(camera_transform_.position_, v, CAM_DISTANCE*2, *y, hits, LEPRA_ARRAY_COUNT(hits));
			if (hit_count) {
				break;
			}
		}
		if (hit_count) {
			_piece->SetDragPosition(hits[0]);
			tapped_pieces.push_back(_piece);
		}
	}
	if (tapped_pieces.empty()) {
		// TODO:
		// Find piece by distance to closest vertex, if not too far.
		radius;
	}
	if (tapped_pieces.empty()) {
		return 0;
	}
	std::sort(tapped_pieces.begin(), tapped_pieces.end(), PieceDistanceAscending(camera_transform_.position_));
	Piece* _piece = tapped_pieces[0];
	drag.extra_ = _piece->GetInstanceId();
	const float _depth = camera_transform_.position_.GetDistance(_piece->GetDragPosition());
	_piece->SetDragging(true, _depth);
	return _piece;
}

Piece* ImpuzzableManager::GetDraggedPiece(uilepra::touch::Drag& drag) {
	if (!drag.extra_) {
		return 0;
	}
	Piece* _piece = (Piece*)GetContext()->GetObject(drag.extra_);
	if (!_piece) {
		drag.extra_ = 0;
	}
	return _piece;
}

void ImpuzzableManager::DragPiece(Piece* piece, const PixelCoord& screen_point) {
	const vec3 d = piece->GetDragPosition();
	const vec3 target = To3dPoint(screen_point, piece->GetDragDepth());
	piece->SetMoveTarget(target);
}

vec3 ImpuzzableManager::To3dPoint(const PixelCoord& coord, float depth) const {
	const vec3 v = ui_manager_->GetRenderer()->ScreenCoordToVector(coord);
	return v * depth + camera_transform_.position_;
}



bool ImpuzzableManager::DidFinishLevel() {
	int level;
	v_get(level, =, GetVariableScope(), kRtvarGameLevel, 0);
	log_.Headlinef("Level %i done!", level);
	OnPauseButton(0);
	UiCure::UserSound2dResource* finish_sound = new UiCure::UserSound2dResource(ui_manager_, uilepra::SoundManager::kLoopNone);
	new UiCure::SoundReleaser(GetResourceManager(), ui_manager_, GetContext(), "finish.wav", finish_sound, 0.5f, Random::Uniform(0.98f, 1.02f));

	bool run_ads;
	v_get(run_ads, =, GetVariableScope(), kRtvarGameRunads, true);
	if (run_ads) {
		Impuzzable__ShowAd();
	}

	return true;
}

int ImpuzzableManager::StepLevel(int count) {
	cuts_left_ = 25;
	shakes_left_ = 2;
	percent_done_ = 0;
	float previous_score;
	v_get(previous_score, =(float), GetVariableScope(), kRtvarGameScore, 0.0);
	if (count > 0) {
		previous_score += level_score_;
	} else if (count < 0) {
		previous_score = 0;
	}
	level_score_ = 0;
	quick_cut_count_ = 0;

	int level_number;
	v_get(level_number, =, GetVariableScope(), kRtvarGameLevel, 0);
	level_number = std::max(0, level_number+count);
	bool vary_shapes;
	v_get(vary_shapes, =, GetVariableScope(), kRtvarGameLevelshapealternate, false);
	level_->GenerateLevel(GetPhysicsManager(), vary_shapes, level_number);
	int piece_count = level_number + 2;
	for (int x = 0; x < piece_count; ++x) {
		CreatePiece(x, 0);
	}
	while ((int)pieces_.size() > piece_count) {
		Piece* _piece = pieces_.back();
		GetContext()->PostKillObject(_piece->GetInstanceId());
		pieces_.pop_back();
	}
	v_set(GetVariableScope(), kRtvarGameLevel, level_number);
	v_set(GetVariableScope(), kRtvarGameScore, (double)previous_score);
	return level_number;
}



cure::RuntimeVariableScope* ImpuzzableManager::GetVariableScope() const {
	return (Parent::GetVariableScope());
}



bool ImpuzzableManager::InitializeUniverse() {
	// Create dummy explosion to ensure all geometries loaded and ready, to avoid LAAAG when first exploading.
	uitbc::ParticleRenderer* particle_renderer = (uitbc::ParticleRenderer*)ui_manager_->GetRenderer()->GetDynamicRenderer("particle");
	const vec3 v;
	particle_renderer->CreateExplosion(vec3(0,0,-2000), 1, v, 1, 1, v, v, v, v, v, 1, 1, 1, 1);

	cuts_left_ = 25;
	shakes_left_ = 2;
	level_ = (Level*)Parent::CreateContextObject("level", cure::kNetworkObjectLocallyControlled, 0);
	StepLevel(0);
	sunlight_ = new Sunlight(ui_manager_);
	return true;
}

void ImpuzzableManager::TickInput() {
	TickNetworkInput();
	TickUiInput();
}

void ImpuzzableManager::TickUiInput() {
	ui_manager_->GetInputManager()->SetCursorVisible(true);

	const int physics_step_count = GetTimeManager()->GetAffordedPhysicsStepCount();
	if (physics_step_count > 0 && allow_movement_input_) {
	}
}

void ImpuzzableManager::TickUiUpdate() {
	((ImpuzzableConsoleManager*)GetConsoleManager())->GetUiConsole()->Tick();
}

void ImpuzzableManager::SetLocalRender(bool render) {
	(void)render;
}



void ImpuzzableManager::CreatePiece(int index, const vec3* position) {
	vec3 _position;
	if (position) {
		_position = *position;
	} else {
		_position.x = index%3*0.5f-0.5f;
		_position.y = index/3%3*0.5f-0.5f;
		_position.z = -index/9%3*0.5f-0.5f;
	}
	if ((int)pieces_.size() > index) {
		GetPhysicsManager()->SetBodyPosition(pieces_[index]->GetRootBodyId(), _position);
		return;
	}
	Piece* _piece = (Piece*)Parent::CreateContextObject("testblock", cure::kNetworkObjectLocallyControlled, 0);
	_piece->SetRootPosition(_position);
	_piece->SetRootVelocity(RNDVEC(1.0f));
	_piece->StartLoading();
	pieces_.push_back(_piece);
}

cure::ContextObject* ImpuzzableManager::CreateContextObject(const str& class_id) const {
	UiCure::Machine* _object = 0;
	if (class_id == "level") {
		_object = new Level(GetResourceManager(), class_id, ui_manager_);
	} else {
		_object = new Piece(GetResourceManager(), class_id, ui_manager_);
	}
	_object->SetAllowNetworkLogic(true);
	return (_object);
}

/*cure::ContextObject* ImpuzzableManager::CreateLogicHandler(const str& type) {
}*/

void ImpuzzableManager::OnLoadCompleted(cure::ContextObject* object, bool _ok) {
	if (_ok) {
		/*if (object->GetClassId() == "testblock")) {
		}*/
	} else {
		log_.Errorf("Could not load object of type %s.", object->GetClassId().c_str());
		GetContext()->PostKillObject(object->GetInstanceId());
	}
}

void ImpuzzableManager::OnCollision(const vec3& force, const vec3& torque, const vec3& position,
	cure::ContextObject* object1, cure::ContextObject* object2,
	tbc::PhysicsManager::BodyID body1_id, tbc::PhysicsManager::BodyID body2_id) {
	(void)body2_id;
	if (force.GetLengthSquared() < 400) {	// Optimization.
		return;
	}
	collision_sound_manager_->OnCollision(force, torque, position, object1, object2, body1_id, 5000, false);
}



void ImpuzzableManager::ShowInstruction() {
	pause_button_->SetVisible(false);

	uitbc::Dialog* d = menu_->CreateTbcDialog(life::Menu::ButtonAction(this, &ImpuzzableManager::OnMenuAlternative), 0.8f, 0.6f);
	d->SetColor(BG_COLOR, OFF_BLACK, BLACK, BLACK);
	d->SetDirection(+1, false);
	uitbc::FixedLayouter layouter(d);

	uitbc::Label* label1 = new uitbc::Label(BRIGHT_TEXT, "Swipe to cut the box. Avoid hitting the");
	layouter.AddComponent(label1, 0, 6, 0, 1, 1);
	uitbc::Label* label2 = new uitbc::Label(BRIGHT_TEXT, "pieces. Cut away 85% to complete level.");
	layouter.AddComponent(label2, 1, 6, 0, 1, 1);

	uitbc::Button* reset_level_button = new uitbc::Button(GREEN_BUTTON, "OK");
	layouter.AddButton(reset_level_button, -9, 2, 3, 0, 1, 1, true);

	v_set(GetVariableScope(), kRtvarPhysicsHalt, true);
}

void ImpuzzableManager::OnPauseButton(uitbc::Button* button) {
	if (button) {
		menu_->OnTapSound(button);
	}
	pause_button_->SetVisible(false);

	bool run_ads;
	v_get(run_ads, =, GetVariableScope(), kRtvarGameRunads, true);
	bool did_buy = !run_ads;
	uitbc::Dialog* d = menu_->CreateTbcDialog(life::Menu::ButtonAction(this, &ImpuzzableManager::OnMenuAlternative), 0.6f, did_buy? 0.6f : 0.7f);
	d->SetColor(BG_COLOR, OFF_BLACK, BLACK, BLACK);
	d->SetDirection(+1, false);
	uitbc::FixedLayouter layouter(d);
	int row = 0;
	const int row_count = did_buy? 3 : 4;

	bool is_paused = false;
	if (LEVEL_DONE()) {
		uitbc::Label* label = new uitbc::Label(BRIGHT_TEXT, "Level completed (85%"));
		label->SetFontId(ui_manager_->SetScaleFont(1.2f));
		ui_manager_->SetMasterFont();
		label->SetIcon(uitbc::Painter::kInvalidImageid, uitbc::TextComponent::kIconCenter);
		label->SetAdaptive(false);
		layouter.AddComponent(label, row++, row_count, 0, 1, 1);

		uitbc::Button* next_level_button = new uitbc::Button(GREEN_BUTTON, "Next level");
		layouter.AddButton(next_level_button, -1, row++, row_count, 0, 1, 1, true);
	} else {
		uitbc::Label* label;
		if (cuts_left_ > 0) {
			label = new uitbc::Label(BRIGHT_TEXT, "Paused");
			is_paused = true;
		} else {
			label = new uitbc::Label(RED_BUTTON, "Out of cuts!");
		}
		label->SetFontId(ui_manager_->SetScaleFont(1.2f));
		ui_manager_->SetMasterFont();
		label->SetIcon(uitbc::Painter::kInvalidImageid, uitbc::TextComponent::kIconCenter);
		label->SetAdaptive(false);
		layouter.AddComponent(label, row++, row_count, 0, 1, 1);
	}

	uitbc::Button* reset_level_button = new uitbc::Button(ORANGE_BUTTON, "Reset level");
	layouter.AddButton(reset_level_button, -3, row++, row_count, 0, 1, 1, true);

	if (row < 3) {
		uitbc::Button* restart_from1st_level_button = new uitbc::Button(RED_BUTTON, "Reset game");
		layouter.AddButton(restart_from1st_level_button, -4, row++, row_count, 0, 1, 1, true);
	}

	if (!did_buy) {
		uitbc::Button* buy_button = new uitbc::Button(BLACK_BUTTON, "Buy full");
		buy_button->SetFontColor(DIM_TEXT);
		layouter.AddButton(buy_button, -5, row++, row_count, 0, 1, 1, true);
	}

	if (is_paused) {
		uitbc::Button* close_button = new uitbc::Button(DIM_RED, "X");
		layouter.AddCornerButton(close_button, -9);
	}

	v_set(GetVariableScope(), kRtvarPhysicsHalt, true);
}

void ImpuzzableManager::OnMenuAlternative(uitbc::Button* button) {
	int level;
	v_get(level, =, GetVariableScope(), kRtvarGameLevel, 0);
	switch (button->GetTag()) {
		case -1:	GetConsoleManager()->PushYieldCommand("step-level +1");				break;
		case -2:	GetConsoleManager()->PushYieldCommand("step-level -1");				break;
		case -3:	GetConsoleManager()->PushYieldCommand("step-level 0");				break;
		case -4:	GetConsoleManager()->PushYieldCommand(strutil::Format("step-level %i", -level));	break;
		case -5:	Impuzzable__Buy();										break;
	}
	pause_button_->SetVisible(true);
	HiResTimer::StepCounterShadow();
	v_set(GetVariableScope(), kRtvarPhysicsHalt, false);
}



void ImpuzzableManager::ScriptPhysicsTick() {
	// Camera moves in a "moving average" kinda curve (halfs the distance in x seconds).
	const float physics_time = GetTimeManager()->GetAffordedPhysicsTotalTime();
	if (physics_time > 1e-5) {
		bool is_first_time;
		v_get(is_first_time, =, GetVariableScope(), kRtvarGameFirsttime, true);
		if (is_first_time) {
			ShowInstruction();
			v_set(GetVariableScope(), kRtvarGameFirsttime, false);
		}
		HandleDrag();
		MoveCamera(physics_time);
		UpdateCameraPosition(false);
	}

	if (next_level_timer_.IsStarted()) {
		if (next_level_timer_.QueryTimeDiff() > 5.0) {
			next_level_timer_.Stop();
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

	Parent::ScriptPhysicsTick();
}

void ImpuzzableManager::HandleWorldImpuzzablearies() {
	/*std::vector<Piece*> lost_object_array;
	typedef cure::ContextManager::ContextObjectTable ContextTable;
	const ContextTable& object_table = GetContext()->GetObjectTable();
	ContextTable::const_iterator x = object_table.begin();
	for (; x != object_table.end(); ++x) {
		cure::ContextObject* _object = x->second;
		if (_object->IsLoaded() && _object->GetPhysics()) {
			const vec3 _position = _object->GetPosition();
			if (!Math::IsInRange(_position.x, -10.0f, +10.0f) ||
				!Math::IsInRange(_position.y, -10.0f, +10.0f) ||
				!Math::IsInRange(_position.z, -10.0f, +10.0f)) {
				lost_object_array.push_back(_object->GetInstanceId());
			}
		}
	}
	if (!lost_object_array.empty()) {
		ScopeLock lock(GetTickLock());
		std::vector<Piece*>::const_iterator y = lost_object_array.begin();
		for (; y != lost_object_array.end(); ++y) {
			DeleteContextObject(*y);
			std::vector<Piece*>::iterator x;
			for (x = pieces_.begin(); x != pieces_.end(); ++x) {
				if (*x == *y) {
					pieces_.erase(x);
					break;
				}
			}
		}
	}*/
}

void ImpuzzableManager::MoveCamera(float frame_time) {
	if (is_cutting_ || is_shaking_) {
		return;
	}

	int level;
	v_get(level, =, GetVariableScope(), kRtvarGameLevel, 0);
	float base_speed = 0.15f + level * 0.01f;
	camera_angle_ += base_speed*camera_rotate_speed_*frame_time;
	camera_rotate_speed_ = Math::Lerp(camera_rotate_speed_, (camera_rotate_speed_ < 0)? -1.0f : 1.0f, 0.07f);
	if (camera_angle_ > 2*PIF) {
		camera_angle_ -= 2*PIF;
	}
	quat q(0, vec3(0,1,0));
	vec3 p(0,-CAM_DISTANCE,0);
	camera_transform_ = xform(q, p);
	camera_transform_.RotateAroundAnchor(vec3(), vec3(0,0,1), camera_angle_);
	camera_transform_.RotatePitch(-sin(camera_angle_*2)*0.3f);
	camera_transform_.MoveUp(sin(camera_angle_*2)*1.5f);
}

void ImpuzzableManager::UpdateCameraPosition(bool update_mic_position) {
	ui_manager_->SetCameraPosition(camera_transform_);
	if (update_mic_position) {
		ui_manager_->SetMicrophonePosition(camera_transform_, vec3());
	}
}



void ImpuzzableManager::PrintText(const str& s, int x, int y) const {
	Color old_color = ui_manager_->GetPainter()->GetColor(0);
	ui_manager_->GetPainter()->SetColor(DARK_BLUE, 0);
	ui_manager_->GetPainter()->PrintText(s, x, y+1);
	ui_manager_->GetPainter()->SetColor(old_color, 0);
	ui_manager_->GetPainter()->PrintText(s, x, y);
}

void ImpuzzableManager::DrawImage(uitbc::Painter::ImageID image_id, float cx, float cy, float w, float h, float angle) const {
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



void ImpuzzableManager::PainterImageLoadCallback(UiCure::UserPainterKeepImageResource* resource) {
	if (resource->GetLoadState() == cure::kResourceLoadComplete) {
		ui_manager_->GetDesktopWindow()->GetImageManager()->AddLoadedImage(*resource->GetRamData(), resource->GetData(),
			uitbc::GUIImageManager::kCentered, uitbc::GUIImageManager::kAlphablend, 255);
	}
}



loginstance(kGame, ImpuzzableManager);



}
