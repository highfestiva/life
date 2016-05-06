
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "boundmanager.h"
#include "../cure/include/contextmanager.h"
#include "../cure/include/timemanager.h"
#include "../lepra/include/cyclicarray.h"
#include "../lepra/include/plane.h"
#include "../lepra/include/timelogger.h"
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
#include "ball.h"
#include "bound.h"
#include "boundconsolemanager.h"
#include "level.h"
#include "rtvar.h"
#include "sunlight.h"
#include "version.h"

#define BG_COLOR		Color(5, 10, 15, 230)
#define CAM_DISTANCE		7.0f
#define BALL_RADIUS		0.15f
#define DRAG_FLAG_INVALID	uilepra::touch::kDragUser
#define DRAG_FLAG_STARTED	(uilepra::touch::kDragUser<<1)
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


namespace Bound {



void Bound__ShowAd();
void Bound__Buy();



BoundManager::BoundManager(life::GameClientMasterTicker* pMaster, const cure::TimeManager* time,
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

	SetConsoleManager(new BoundConsoleManager(GetResourceManager(), this, ui_manager_, GetVariableScope(), render_area_));

	GetPhysicsManager()->SetSimulationParameters(0.0f, 0.03f, 0.2f);

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
		size_t a = Random::GetRandomNumber() % _n_gon.size(); \
		size_t b = Random::GetRandomNumber() % _n_gon.size(); \
		if (a != b) \
			std::swap(_n_gon[1], _n_gon[b]); \
	}
	std::vector<vec3> _n_gon;
#define CLRL \
	_n_gon.clear(); \
	_n_gon.push_back(vec3(-2.1314f, 0, -0.333f));	\
	for (int x = 0; x < 10; ++x)	\
		_n_gon.push_back(_n_gon[_n_gon.size()-1] + vec3(1,0,0.14f)*Random::Uniform(0.001f, 0.1f));	\
	for (int x = 0; x < 10; ++x)	\
		_n_gon.push_back(_n_gon[_n_gon.size()-1] + vec3(0.14f,0,-1)*Random::Uniform(0.001f, 0.1f));	\
	for (int x = 0; x < 10; ++x)	\
		_n_gon.push_back(_n_gon[_n_gon.size()-1] + vec3(-0.34f,0,-1)*Random::Uniform(0.001f, 0.1f));	\
	for (int x = 0; x < 10; ++x)	\
		_n_gon.push_back(_n_gon[_n_gon.size()-1] + vec3(-1,0,-0.14f)*Random::Uniform(0.001f, 0.1f));
	for (int y = 0; y < 100; ++y) {
		CLRL;
		RNDMZEL;
		std::vector<vec3> copy(_n_gon);
		CreateNGon(_n_gon);
		log_.Infof("----------");
		for (int x = 0; x < (int)_n_gon.size(); ++x) {
			log_.Infof("%f;%f;", _n_gon[x].x, _n_gon[x].z);
		}
		deb_assert(_n_gon.size() == 5);
	}*/
}

BoundManager::~BoundManager() {
	Close();

	delete collision_sound_manager_;
	collision_sound_manager_ = 0;
}

void BoundManager::Suspend(bool hard) {
	if (!menu_->GetDialog()) {
		pause_button_->SetVisible(false);
		OnPauseButton(0);
	}
}

void BoundManager::LoadSettings() {
	v_set(GetVariableScope(), kRtvarGameSpawnpart, 1.0);

	Parent::LoadSettings();

	v_set(GetVariableScope(), kRtvarUi2DFont, "Verdana");
	v_set(GetVariableScope(), kRtvarUi2DFontflags, 0);
	v_set(GetVariableScope(), kRtvarUi3DFov, 52.0);
	v_set(GetVariableScope(), kRtvarPhysicsMicrosteps, 3);
	v_set(GetVariableScope(), kRtvarPhysicsNoclip, false);

	GetConsoleManager()->ExecuteCommand("bind-key F2 prev-level");
	GetConsoleManager()->ExecuteCommand("bind-key F3 next-level");
}

void BoundManager::SaveSettings() {
	GetConsoleManager()->ExecuteCommand("save-application-config-file "+GetApplicationCommandFilename());
}

bool BoundManager::Open() {
	bool __ok = Parent::Open();
	if (__ok) {
		pause_button_ = ICONBTNA("btn_pause.png", L"");
		int x = render_area_.left_ + 12;
		int y = render_area_.bottom_ - 12 - 32;
		ui_manager_->GetDesktopWindow()->AddChild(pause_button_, x, y);
		pause_button_->SetVisible(true);
		pause_button_->SetOnClick(BoundManager, OnPauseButton);
	}
	if (__ok) {
		menu_ = new life::Menu(ui_manager_, GetResourceManager());
		menu_->SetButtonTapSound("tap.wav", 0.2f, 0.05f);
	}
	return __ok;
}

void BoundManager::Close() {
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

void BoundManager::SetIsQuitting() {
	((BoundConsoleManager*)GetConsoleManager())->GetUiConsole()->SetVisible(false);
	Parent::SetIsQuitting();
}

void BoundManager::SetFade(float fade_amount) {
	(void)fade_amount;
}



bool BoundManager::Render() {
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

void BoundManager::RenderBackground() {
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

bool BoundManager::Paint() {
	ui_manager_->GetPainter()->SetLineWidth(3);
	if (!Parent::Paint()) {
		return false;
	}
	bool is_using_a_cut = HandleCutting();
	if (level_) {
		ui_manager_->GetPainter()->SetColor(BRIGHT_TEXT);

		const wstr completion_text = wstrutil::Format(L"Reduced volume: %.1f%%", percent_done_);
		int cw = ui_manager_->GetPainter()->GetStringWidth(completion_text);
		PrintText(completion_text, (ui_manager_->GetCanvas()->GetWidth()-cw)/2, 7);

		ui_manager_->SetScaleFont(0.9f);
		float previous_score;
		v_get(previous_score, =(float), GetVariableScope(), kRtvarGameScore, 0.0);
		const wstr score_text = wstrutil::Format(L"Score: %.0f", level_score_+previous_score);
		PrintText(score_text, 25, 9);

		int level;
		v_get(level, =, GetVariableScope(), kRtvarGameLevel, 0);
		const wstr level_text = wstrutil::Format(L"Level: %i", level+1);
		int lw = ui_manager_->GetPainter()->GetStringWidth(level_text);
		PrintText(level_text, ui_manager_->GetCanvas()->GetWidth()-lw-25, 9);

		ui_manager_->GetPainter()->SetColor(is_using_a_cut? DIM_RED_TEXT : DIM_TEXT);
		ui_manager_->SetScaleFont(0.7f);
		const wstr lines_text = wstrutil::Format(L"Cuts: %i", cuts_left_);
		PrintText(lines_text, ui_manager_->GetCanvas()->GetWidth()-lw-24, 31);

#ifdef LEPRA_TOUCH
		vec3 v = RNDPOSVEC()*255;
		Color blink_col;
		blink_col.Set(v.x, v.y, v.z, 1.0f);
		bool is_showing_shake = (is_shaking_ && shake_timer_.QuerySplitTime() < 2.5);
		ui_manager_->GetPainter()->SetColor(is_showing_shake? blink_col : DIM_TEXT);
		const wstr shakes_text = wstrutil::Format(L"Shakes: %i", shakes_left_);
		PrintText(shakes_text, ui_manager_->GetCanvas()->GetWidth()-lw-24, 48);
#endif // touch device.
		ui_manager_->SetMasterFont();
	}
	return true;
}

bool BoundManager::HandleCutting() {
	is_cutting_ = false;
	if (menu_->GetDialog()) {
		return false;
	}
	bool any_drag_started = false;
	const int w = ui_manager_->GetCanvas()->GetWidth();
	const int h = ui_manager_->GetCanvas()->GetHeight();
	const float touch_side_scale = 1.28f;	// Inches.
	const float touch_scale = touch_side_scale / (float)ui_manager_->GetDisplayManager()->GetPhysicalScreenSize();
	const float resolution_margin = w / 50.0f;
	const int m = (int)Math::Lerp(touch_scale * w * 0.25f, resolution_margin, 0.7f);
	const int r = m-2;
	const int d = r*2;
	typedef uilepra::touch::DragManager::DragList DragList;
	DragList drag_list = ui_manager_->GetDragManager()->GetDragList();
	for (DragList::iterator x = drag_list.begin(); x != drag_list.end(); ++x) {
		if (x->flags_&DRAG_FLAG_INVALID) {
			continue;
		}
		is_cutting_ = true;

		vec2 from((float)x->start_.x, (float)x->start_.y);
		vec2 to((float)x->last_.x, (float)x->last_.y);
		vec2 __vector = to-from;
		bool is_very_new_drag = false;
		if (!(x->flags_&DRAG_FLAG_STARTED)) {
			const float drag_length = __vector.GetLength();
			if (drag_length < w*0.042f) {
				is_very_new_drag = true;
			} else if (drag_length > w*0.083f) {
				x->flags_ |= DRAG_FLAG_STARTED;
			}
		}
		const bool drag_started = ((x->flags_&DRAG_FLAG_STARTED) == DRAG_FLAG_STARTED);
		ui_manager_->GetPainter()->SetColor(drag_started? Color(30,140,20) : Color(140,30,20), 0);
		ui_manager_->GetPainter()->DrawArc((int)from.x-r, (int)from.y-r, d, d, 0, 360, false);
		ui_manager_->GetPainter()->DrawArc((int)to.x-r, (int)to.y-r, d, d, 0, 360, false);
		const float cut_line_length = (float)(w+h);
		__vector.Normalize(cut_line_length);
		vec2 start = is_very_new_drag? from : from - __vector;
		if (!drag_started) {
			ui_manager_->GetPainter()->DrawLine((int)start.x, (int)start.y, (int)to.x, (int)to.y);
		} else {
			vec2 __end = to + __vector;
			ui_manager_->GetPainter()->DrawLine((int)start.x, (int)start.y, (int)__end.x, (int)__end.y);
		}

		// The plane goes through the camera and the projected midpoint of the line.
		PixelCoord screen_mid((int)(from.x+to.x)/2, (int)(from.y+to.y)/2);
		Plane _cut_plane_delimiter;
		Plane _cut_plane = ScreenLineToPlane(screen_mid, PixelCoord((int)to.x, (int)to.y), _cut_plane_delimiter);
		vec3 _collision_point;

		if (_cut_plane.n.GetLengthSquared() > 0 && !is_very_new_drag && CheckBallsPlaneCollition(_cut_plane, drag_started? 0 : &_cut_plane_delimiter, _collision_point)) {
			// Invalidate swipe.
			--cuts_left_;
			x->flags_ |= DRAG_FLAG_INVALID;

			// Explosions! YES!
			const int particles = Random::Uniform(5, 11);
			vec3 start_fire_color(1.0f, 1.0f, 0.3f);
			vec3 fire_color(0.6f, 0.4f, 0.2f);
			vec3 start_smoke_color(0.4f, 0.4f, 0.4f);
			vec3 smoke_color(0.2f, 0.2f, 0.2f);
			vec3 shrapnel_color(0.3f, 0.3f, 0.3f);	// Default debris color is gray.
			uitbc::ParticleRenderer* particle_renderer = (uitbc::ParticleRenderer*)ui_manager_->GetRenderer()->GetDynamicRenderer("particle");
			particle_renderer->CreateExplosion(_collision_point, 0.05f, vec3(), 0.15f, 0.25f, start_fire_color, fire_color, start_smoke_color, smoke_color, shrapnel_color, particles, particles, particles/2, particles/3);
			UiCure::UserSound2dResource* break_sound = new UiCure::UserSound2dResource(ui_manager_, uilepra::SoundManager::kLoopNone);
			new UiCure::SoundReleaser(GetResourceManager(), ui_manager_, GetContext(), "broken_window.wav", break_sound, 0.5f, Random::Uniform(0.9f, 1.2f));
			cut_timer_.ReduceTimeDiff(-2.0);
			level_score_ -= 200;
		} else if (drag_started && !x->is_press_ && cuts_left_ > 0) {
			--cuts_left_;
			if (Cut(_cut_plane)) {
				const int side = CheckIfPlaneSlicesBetweenBalls(_cut_plane);
				if (side > 0) {
					_cut_plane = -_cut_plane;
				} else {
					level_score_ += 500;
				}
				float cut_x = camera_transform_.GetOrientation().GetInverseRotatedVector(_cut_plane.n).x;
				if (cut_timer_.PopTimeDiff() < 1.4) {
					cut_sound_pitch_ += 0.3f;
					++quick_cut_count_;
				} else {
					cut_sound_pitch_ = Random::Uniform(0.9f, 1.1f);
					quick_cut_count_ = 0;
				}
				camera_rotate_speed_ = (cut_x < 0)? +3+quick_cut_count_*0.5f : -3-quick_cut_count_*0.5f;
				UiCure::UserSound2dResource* cut_sound = new UiCure::UserSound2dResource(ui_manager_, uilepra::SoundManager::kLoopNone);
				new UiCure::SoundReleaser(GetResourceManager(), ui_manager_, GetContext(), "cut.wav", cut_sound, 0.5f, cut_sound_pitch_);
			}
		}
		any_drag_started |= drag_started;
	}
	ui_manager_->GetDragManager()->SetDragList(drag_list);
	return any_drag_started;
}

Plane BoundManager::ScreenLineToPlane(const PixelCoord& coord, const PixelCoord& end_point, Plane& cut_plane_delimiter) {
	const PixelCoord screen_normal(coord.y-end_point.y, coord.x-end_point.x);
	const vec3 direction_to_plane_center = ui_manager_->GetRenderer()->ScreenCoordToVector(coord);
	vec3 bad_normal((float)screen_normal.x, 0, (float)screen_normal.y);
	bad_normal = camera_transform_.GetOrientation() * bad_normal;
	const Plane _cut_plane(camera_transform_.GetPosition(), direction_to_plane_center, bad_normal);

	const vec3 direction_to_end_point = ui_manager_->GetRenderer()->ScreenCoordToVector(end_point);
	cut_plane_delimiter = Plane(camera_transform_.GetPosition(), direction_to_end_point, _cut_plane.n.Cross(direction_to_end_point));
	if (cut_plane_delimiter.n*direction_to_plane_center < 0) {
		cut_plane_delimiter = -cut_plane_delimiter;
	}

	return _cut_plane;
}

bool BoundManager::Cut(Plane cut_plane) {
	const int side = CheckIfPlaneSlicesBetweenBalls(cut_plane);
	if (side == 0) {	// Balls on both sides.
		return DoCut(level_->GetMesh(), cut_plane, kCutAddWindow);
	}
	if (side == -1) {
		cut_plane = -cut_plane;
	}
	DoCut(level_->GetWindowMesh(), cut_plane, kCutWindowItself);
	return DoCut(level_->GetMesh(), cut_plane, kCutNormal);
}

bool BoundManager::DoCut(const uitbc::TriangleBasedGeometry* mesh, Plane cut_plane, CutMode cut_mode) {
	if (!mesh) {
		return false;
	}

	last_cut_mode_ = cut_mode;
	// Plane normal now "points" toward vertices that stays. Those on the other side gets cut off. The new triangles will use this normal.
	TimeLogger time_logger(&log_, "CheckIfPlaneSlicesBetweenBalls + prep");
	const quat q = camera_transform_.GetOrientation();
	const int tc = mesh->GetVertexCount() / 3;
	const float* v = mesh->GetVertexData();
	const uint8* c = mesh->GetColorData();
	if (last_cut_mode_ == kCutNormal) {
		cut_vertices_.reserve(tc*2*3*3);
		cut_colors_.reserve(tc*2*3*4);
		cut_vertices_.clear();
		cut_colors_.clear();
	} else {
		cut_window_vertices_.reserve(tc*2*3*3);
		cut_window_vertices_.clear();
	}
	std::vector<vec3> _n_gon;
	std::unordered_set<int> n_gon_map;
	bool did_cut = false;
	const bool create_window = (cut_mode == kCutAddWindow);
	time_logger.Transfer("CutLoop");
	for (int x = 0; x < tc; ++x) {
		vec3 p0(v[x*9+0], v[x*9+1], v[x*9+2]);
		vec3 p1(v[x*9+3], v[x*9+4], v[x*9+5]);
		vec3 p2(v[x*9+6], v[x*9+7], v[x*9+8]);
		const float d0 = cut_plane.GetDistance(p0);
		const float d1 = cut_plane.GetDistance(p1);
		const float d2 = cut_plane.GetDistance(p2);
		if (d0 >= 0 && d1 >= 0 && d2 >= 0) {
			if (!create_window) {
				// All vertices on staying side of mesh. No cut, only copy.
				if (cut_mode == kCutNormal) {
					cut_vertices_.insert(cut_vertices_.end(), &v[x*9], &v[x*9+9]);
				} else {
					cut_window_vertices_.insert(cut_window_vertices_.end(), &v[x*9], &v[x*9+9]);
				}
				if (c) {
					cut_colors_.insert(cut_colors_.end(), &c[x*12], &c[x*12+12]);
				}
			}
		} else if (d0 <= 0 && d1 <= 0 && d2 <= 0) {
			if (!create_window) {
				// The whole triangle got cut off - way to go! No cut, only discard.
				did_cut = true;
			}
		} else {
			did_cut = true;

			// Go ahead and cut. Ends up with either a triangle (single point on the positive side), or
			// a quad (two points on the positive side). Quad is cut along pseudo-shortest diagonal.
			vec3 d01 = p1-p0;
			vec3 d12 = p2-p1;
			vec3 d20 = p0-p2;
			if (d0 > 0 && d1 > 0) {
				// Quad cut.
				const float t3 = -d1 / (cut_plane.n*d12);
				const float t4 = -d0 / (cut_plane.n*d20);
				vec3 p3 = p1+t3*d12;
				vec3 p4 = p0+t4*d20;
				if (!create_window) {
					AddTriangle(p0, p1, p3, &c[x*12]);
					AddTriangle(p0, p3, p4, &c[x*12]);
				}
				AddNGonPoints(_n_gon, n_gon_map, p3, p4);
			} else if (d1 > 0 && d2 > 0) {
				// Quad cut.
				const float t3 = -d2 / (cut_plane.n*d20);
				const float t4 = -d1 / (cut_plane.n*d01);
				vec3 p3 = p2+t3*d20;
				vec3 p4 = p1+t4*d01;
				if (!create_window) {
					AddTriangle(p1, p2, p3, &c[x*12]);
					AddTriangle(p1, p3, p4, &c[x*12]);
				}
				AddNGonPoints(_n_gon, n_gon_map, p3, p4);
			} else if (d0 > 0 && d2 > 0) {
				// Quad cut.
				const float t3 = -d0 / (cut_plane.n*d01);
				const float t4 = -d2 / (cut_plane.n*d12);
				vec3 p3 = p0+t3*d01;
				vec3 p4 = p2+t4*d12;
				if (!create_window) {
					AddTriangle(p2, p0, p3, &c[x*12]);
					AddTriangle(p2, p3, p4, &c[x*12]);
				}
				AddNGonPoints(_n_gon, n_gon_map, p3, p4);
			} else if (d0 > 0) {
				// Triangle cut.
				const float t3 = -d0 / (cut_plane.n*d01);
				const float t4 = -d0 / (cut_plane.n*d20);
				vec3 p3 = p0+t3*d01;
				vec3 p4 = p0+t4*d20;
				if (!create_window) {
					AddTriangle(p0, p3, p4, &c[x*12]);
				}
				AddNGonPoints(_n_gon, n_gon_map, p3, p4);
			} else if (d1 > 0) {
				// Triangle cut.
				const float t3 = -d1 / (cut_plane.n*d12);
				const float t4 = -d1 / (cut_plane.n*d01);
				vec3 p3 = p1+t3*d12;
				vec3 p4 = p1+t4*d01;
				if (!create_window) {
					AddTriangle(p1, p3, p4, &c[x*12]);
				}
				AddNGonPoints(_n_gon, n_gon_map, p3, p4);
			} else {
				// Triangle cut.
				const float t3 = -d2 / (cut_plane.n*d20);
				const float t4 = -d2 / (cut_plane.n*d12);
				vec3 p3 = p2+t3*d20;
				vec3 p4 = p2+t4*d12;
				if (!create_window) {
					AddTriangle(p2, p3, p4, &c[x*12]);
				}
				AddNGonPoints(_n_gon, n_gon_map, p3, p4);
			}
		}
	}
	if (last_cut_mode_ == kCutWindowItself) {
		if (did_cut && last_cut_mode_ == kCutWindowItself && cut_window_vertices_.empty()) {
			force_cut_window_ = true;
		}
		return true;
	}
	time_logger.Transfer("CreateNGon");
	CreateNGon(_n_gon);
	if (_n_gon.size() < 3 || !did_cut) {
		cut_vertices_.clear();
		cut_window_vertices_.clear();
		cut_colors_.clear();
		return false;
	}
	// Generate random colors and add.
	time_logger.Transfer("AddNGonTriangles");
	std::vector<uint8> n_gon_colors;
	const size_t nvc = (_n_gon.size()-2)*3;
	n_gon_colors.resize(nvc*4);
	static const Color _colors[] = {ORANGE, RED, BLUE, GREEN, WHITE, CYAN, BLACK, ORANGE, MAGENTA, YELLOW, Color(255, 98, 128), Color(98, 60, 0)};
	static int color_index = 0;
	const Color new_color = _colors[color_index++];
	color_index %= LEPRA_ARRAY_COUNT(_colors);
	for (size_t x = 0; x < nvc; ++x) {
		n_gon_colors[x*4+0] = new_color.red_;
		n_gon_colors[x*4+1] = new_color.green_;
		n_gon_colors[x*4+2] = new_color.blue_;
		n_gon_colors[x*4+3] = 255;
	}
	AddNGonTriangles(cut_plane, _n_gon, &n_gon_colors[0]);
	return true;
}

void BoundManager::AddTriangle(const vec3& v0, const vec3& v1, const vec3& v2, const uint8* colors) {
	if (last_cut_mode_ == kCutNormal) {
		cut_vertices_.push_back(v0.x); cut_vertices_.push_back(v0.y); cut_vertices_.push_back(v0.z);
		cut_vertices_.push_back(v1.x); cut_vertices_.push_back(v1.y); cut_vertices_.push_back(v1.z);
		cut_vertices_.push_back(v2.x); cut_vertices_.push_back(v2.y); cut_vertices_.push_back(v2.z);
		cut_colors_.insert(cut_colors_.end(), colors, colors+12);
	} else {
		cut_window_vertices_.push_back(v0.x); cut_window_vertices_.push_back(v0.y); cut_window_vertices_.push_back(v0.z);
		cut_window_vertices_.push_back(v1.x); cut_window_vertices_.push_back(v1.y); cut_window_vertices_.push_back(v1.z);
		cut_window_vertices_.push_back(v2.x); cut_window_vertices_.push_back(v2.y); cut_window_vertices_.push_back(v2.z);
	}
}

void BoundManager::AddNGonPoints(std::vector<vec3>& n_gon, std::unordered_set<int>& n_gon_map, const vec3& p0, const vec3& p1) {
	AddNGonPoint(n_gon, n_gon_map, p0);
	AddNGonPoint(n_gon, n_gon_map, p1);
}

void BoundManager::AddNGonPoint(std::vector<vec3>& n_gon, std::unordered_set<int>& n_gon_map, const vec3& p) {
	const int position_hash = (int)(p.x*1051 + p.y*1117 + p.z*1187);
	if (n_gon_map.find(position_hash) == n_gon_map.end()) {
		n_gon_map.insert(position_hash);
		n_gon.push_back(p);
	}
	/*(void)n_gon_map;
	std::vector<vec3>::iterator x;
	for (x = n_gon.begin(); x != n_gon.end(); ++x) {
		if (x->GetDistanceSquared(p) < 1e-6) {
			return;
		}
	}
	n_gon.push_back(p);*/
}

void BoundManager::CreateNGon(std::vector<vec3>& n_gon) {
	//LineUpNGonSides(n_gon);
	//SortNGonSides(n_gon);
	// Iteratively producing better N-gon each loop.
	/*LineUpNGonBorders(n_gon, false);
	LineUpNGonBorders(n_gon, true);
	SimplifyNGon(n_gon);*/
	size_t pre_size;
	size_t __size;
	do {
		pre_size = n_gon.size();
		LineUpNGonBorders(n_gon, false);
		LineUpNGonBorders(n_gon, true);
		SimplifyNGon(n_gon);
		__size = n_gon.size();
	} while (__size < pre_size);
}

void BoundManager::LineUpNGonBorders(std::vector<vec3>& n_gon, bool sort) {
	// Create triangles from N-gon.
	if (n_gon.size() <= 3) {
		return;
	}
	// Since the N-gon is convex, we only need to find the smallect angle for each vertex, and
	// those adjoining vertices will be the neighbours. It will automatically appear in order.
	// Rendering will determine if it's normal is aligned or perpendicular to the cutting plane.
	// 1. Start with any two vertices.
	vec3 p0 = n_gon[0];
	vec3 p1 = n_gon[1];
	vec3 t0 = (p1-p0).GetNormalized();
	// 2. Find the tangent with the smallect angle (biggest dot product) to this tangent.
	float biggest_dot = -2;
	size_t adjoining_vertex = 2;
	size_t cnt = n_gon.size();
	vec3 boundary_vector;
	for (size_t idx = 2; idx < cnt; ++idx) {
		vec3 p2 = n_gon[idx];
		vec3 t1 = (p2-p1).GetNormalized();
		float dot = t0*t1;
		if (dot > biggest_dot) {
			biggest_dot = dot;
			adjoining_vertex = idx;
			boundary_vector = t1;
		}
	}
	// 3. This found vertex and tangent contitutes our second outer point and our first found outer boundary.
	//    Place the remaining vertices in a list that we are going to search in.
	p0 = p1;
	p1 = n_gon[adjoining_vertex];
	t0 = boundary_vector;
	std::list<vec3> remaining_vertices;
	for (size_t idx = 0; idx < cnt; ++idx) {
		if (idx != 1 && idx != adjoining_vertex) {
			remaining_vertices.push_back(n_gon[idx]);
		}
	}
	// 4. Clear the N-gon array, and append points on the "angle order" they appear, while dropping them in the search list.
	n_gon.clear();
	n_gon.push_back(p0);
	n_gon.push_back(p1);
	while (!remaining_vertices.empty()) {
		float best_distance = 1e5f;
		float this_distance = 1e5f;
		float biggest_dot = -2;
		std::list<vec3>::iterator adjoining_vertex = remaining_vertices.begin();
		std::list<vec3>::iterator x;
		for (x = remaining_vertices.begin(); x != remaining_vertices.end();) {
			vec3 p2 = *x;
			vec3 t1 = p2-p1;
			this_distance = t1.GetLength();
			t1.Div(this_distance);
			float dot = sort? t0*t1 : std::abs(t0*t1);
			/*if (dot < -SAME_NORMAL) {
				if (p1.GetDistance(p0) >= p1.GetDistance(p2)) {
					// Midpoint on same line (i.e. in reverse direction). Throw it away.
					std::list<vec3>::iterator y = x;
					++y;
					remaining_vertices.erase(x);
					x = y;
					continue;
				}
				// This vertex will come back as we move around one lap; if it's doubly redundant it will be handled then.
				++x;
				continue;
			}*/
			if (dot >= biggest_dot-CLOSE_NORMAL) {
				if (dot >= SAME_NORMAL) {
					if (this_distance < best_distance) {	// Closer vertex on same border?
						best_distance = this_distance;
						biggest_dot = dot;
						adjoining_vertex = x;
					}
				} else {	// Best vertex on upcoming border.
					biggest_dot = dot;
					adjoining_vertex = x;
				}
				/*if (dot > SAME_NORMAL) {
					// Uh-oh! We found two points on the same line; and the second one is better than this one (we can be certain as the N-gon is convex).
					p1 = p0;
					n_gon.pop_back();
					break;
				}*/
			}
			++x;
		}
		if (biggest_dot > -1) {
			p0 = p1;
			p1 = *adjoining_vertex;
			t0 = (p1-p0).GetNormalized();
			n_gon.push_back(p1);
			remaining_vertices.erase(adjoining_vertex);
		} else {
			break;
		}
	}
}

void BoundManager::SimplifyNGon(std::vector<vec3>& n_gon) {
	// Eliminate redundant vertices. N-gon must be sorted when entering.
	int cnt = (int)n_gon.size();
	for (int x = 0; x < cnt;) {
		size_t xn = NGON_INDEX(x-1);
		size_t xp = NGON_INDEX(x+1);
		vec3 p0 = n_gon[xn];
		vec3 p1 = n_gon[x];
		vec3 p2 = n_gon[xp];
		vec3 t0 = p1-p0;
		float l0 = t0.GetLength();
		t0.Div(l0);
		vec3 t1 = (p2-p1).GetNormalized();
		const float dot = t0*t1;
		if (dot > SAME_NORMAL) {
			// Kill middle vertex.
			n_gon.erase(n_gon.begin() + x);
			--cnt;
			continue;
		}
		if (dot < -SAME_NORMAL) {
			// Kill middle vertex.
			float l1 = p1.GetDistance(p2);
			if (l0 < l1) {
				n_gon[xn] = p2;
			}
			n_gon.erase(n_gon.begin() + xp);
			--cnt;
			continue;
		}
		++x;
	}
}

void BoundManager::AddNGonTriangles(const Plane& cut_plane, const std::vector<vec3>& n_gon, const uint8* colors) {
	// Since the N-gon is convex, we hold on to the first point and step the other two around.
	vec3 p0 = n_gon[0];
	vec3 p1 = n_gon[1];
	vec3 p2 = n_gon[2];
	//const vec3 triangle_normal = (p1-p0).Cross(p2-p1);
	//const bool no_flip = (cut_plane.n*triangle_normal < 0);
	const size_t cnt = n_gon.size();
	for (size_t idx = 2; idx < cnt; ++idx) {
		p2 = n_gon[idx];
		const vec3 triangle_normal = (p1-p0).Cross(p2-p1);
		const bool no_flip = (cut_plane.n*triangle_normal < 0);
		if (no_flip) {
			AddTriangle(p0, p1, p2, colors);
		} else {
			AddTriangle(p0, p2, p1, colors);
		}
		p1 = p2;
		colors += 4*3;
	}
	last_cut_plane_ = cut_plane;
}

int BoundManager::CheckIfPlaneSlicesBetweenBalls(const Plane& cut_plane) {
	int side = 0;
	std::vector<cure::GameObjectId>::iterator x;
	for (x = balls_.begin(); x != balls_.end(); ++x) {
		cure::ContextObject* ball = GetContext()->GetObject(*x);
		if (!ball) {
			continue;
		}
		vec3 p = ball->GetRootPosition();
		const float d = cut_plane.GetDistance(p);
		int s = (d < 0)? -1 : +1;
		if (side == 0) {
			side = s;
		} else if ((s<0) != (side<0)) {
			return 0;
		}
	}
	return side;
}

bool BoundManager::CheckBallsPlaneCollition(const Plane& cut_plane, const Plane* cut_plane_delimiter, vec3& collision_point) {
	std::vector<cure::GameObjectId>::iterator x;
	for (x = balls_.begin(); x != balls_.end(); ++x) {
		cure::ContextObject* ball = GetContext()->GetObject(*x);
		if (!ball) {
			continue;
		}
		vec3 p = ball->GetRootPosition();
		float distance = cut_plane.GetAbsDistance(p);
		if (distance < BALL_RADIUS) {
			if (!cut_plane_delimiter) {
				collision_point = p - cut_plane.n*cut_plane.GetDistance(p);
				return true;
			}
			// Check if the ball is on the "wrong" side of the touch endpoint of the cutting plane.
			distance = cut_plane_delimiter->GetDistance(p);
			if (distance >= -BALL_RADIUS) {
				collision_point = p - cut_plane.n*cut_plane.GetDistance(p);
				return true;
			}
		}
	}
	return false;
}

bool BoundManager::AttachTouchToBorder(PixelCoord& point, int margin, int w, int h) {
	int dt = std::abs(point.y - margin);
	int dl = std::abs(point.x - margin);
	int db = std::abs(point.y - (h-margin));
	int dr = std::abs(point.x - (w-margin));
	if (dt < dl && dt < dr && dt < db) {
		if (dt < margin*8) {
			point.y = margin;
			return true;
		}
	} else if (db < dl && db < dr) {
		if (db < margin*8) {
			point.y = h-margin;
			return true;
		}
	} else if (dl < dr) {
		if (dl < margin*8) {
			point.x = margin;
			return true;
		}
	} else {
		if (dr < margin*8) {
			point.x = w-margin;
			return true;
		}
	}
	return false;
}



bool BoundManager::DidFinishLevel() {
	int level;
	v_get(level, =, GetVariableScope(), kRtvarGameLevel, 0);
	log_.Headlinef("Level %i done!", level);
	OnPauseButton(0);
	UiCure::UserSound2dResource* finish_sound = new UiCure::UserSound2dResource(ui_manager_, uilepra::SoundManager::kLoopNone);
	new UiCure::SoundReleaser(GetResourceManager(), ui_manager_, GetContext(), "finish.wav", finish_sound, 0.5f, Random::Uniform(0.98f, 1.02f));

	bool run_ads;
	v_get(run_ads, =, GetVariableScope(), kRtvarGameRunads, true);
	if (run_ads) {
		Bound__ShowAd();
	}

	return true;
}

int BoundManager::StepLevel(int count) {
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
	int ball_count = level_number + 2;
	for (int x = 0; x < ball_count; ++x) {
		CreateBall(x, 0);
	}
	while ((int)balls_.size() > ball_count) {
		cure::ContextObject* ball = GetContext()->GetObject(balls_.back());
		if (!ball) {
			break;
		}
		balls_.pop_back();
		GetContext()->DelayKillObject(ball, 0.1f);
	}
	v_set(GetVariableScope(), kRtvarGameLevel, level_number);
	v_set(GetVariableScope(), kRtvarGameScore, (double)previous_score);
	return level_number;
}



cure::RuntimeVariableScope* BoundManager::GetVariableScope() const {
	return (Parent::GetVariableScope());
}



bool BoundManager::InitializeUniverse() {
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

void BoundManager::TickInput() {
	TickNetworkInput();
	TickUiInput();
}

void BoundManager::TickUiInput() {
	ui_manager_->GetInputManager()->SetCursorVisible(true);

	const int physics_step_count = GetTimeManager()->GetAffordedPhysicsStepCount();
	if (physics_step_count > 0 && allow_movement_input_) {
	}
}

void BoundManager::TickUiUpdate() {
	((BoundConsoleManager*)GetConsoleManager())->GetUiConsole()->Tick();
}

void BoundManager::SetLocalRender(bool render) {
	(void)render;
}



void BoundManager::CreateBall(int index, const vec3* position) {
	vec3 _position;
	if (position) {
		_position = *position;
	} else {
		_position.x = index%3*0.5f-0.5f;
		_position.y = index/3%3*0.5f-0.5f;
		_position.z = -index/9%3*0.5f-0.5f;
	}
	if ((int)balls_.size() > index) {
		cure::ContextObject* ball = GetContext()->GetObject(balls_[index]);
		if (ball) {
			GetPhysicsManager()->SetBodyPosition(ball->GetPhysics()->GetBoneGeometry(0)->GetBodyId(), _position);
		}
		return;
	}
	cure::ContextObject* ball = Parent::CreateContextObject("soccerball", cure::kNetworkObjectLocallyControlled, 0);
	ball->SetRootPosition(_position);
	ball->SetRootVelocity(RNDVEC(1.0f));
	ball->StartLoading();
	balls_.push_back(ball->GetInstanceId());
}

cure::ContextObject* BoundManager::CreateContextObject(const str& class_id) const {
	UiCure::Machine* _object = 0;
	if (class_id == "level") {
		_object = new Level(GetResourceManager(), class_id, ui_manager_);
	} else {
		_object = new Ball(GetResourceManager(), class_id, ui_manager_);
	}
	_object->SetAllowNetworkLogic(true);
	return (_object);
}

/*cure::ContextObject* BoundManager::CreateLogicHandler(const str& type) {
}*/

void BoundManager::OnLoadCompleted(cure::ContextObject* object, bool _ok) {
	if (_ok) {
		/*if (object->GetClassId() == "soccerball")) {
		}*/
	} else {
		log_.Errorf("Could not load object of type %s.", object->GetClassId().c_str());
		GetContext()->PostKillObject(object->GetInstanceId());
	}
}

void BoundManager::OnCollision(const vec3& force, const vec3& torque, const vec3& position,
	cure::ContextObject* object1, cure::ContextObject* object2,
	tbc::PhysicsManager::BodyID body1_id, tbc::PhysicsManager::BodyID body2_id) {
	(void)body2_id;
	collision_sound_manager_->OnCollision(force, torque, position, object1, object2, body1_id, 5000, false);
}



void BoundManager::ShowInstruction() {
	pause_button_->SetVisible(false);

	uitbc::Dialog* d = menu_->CreateTbcDialog(life::Menu::ButtonAction(this, &BoundManager::OnMenuAlternative), 0.8f, 0.6f);
	d->SetColor(BG_COLOR, OFF_BLACK, BLACK, BLACK);
	d->SetDirection(+1, false);
	uitbc::FixedLayouter layouter(d);

	uitbc::Label* label1 = new uitbc::Label(BRIGHT_TEXT, L"Swipe to cut the box. Avoid hitting the");
	layouter.AddComponent(label1, 0, 6, 0, 1, 1);
	uitbc::Label* label2 = new uitbc::Label(BRIGHT_TEXT, L"balls. Cut away 85% to complete level.");
	layouter.AddComponent(label2, 1, 6, 0, 1, 1);

	uitbc::Button* reset_level_button = new uitbc::Button(GREEN_BUTTON, L"OK");
	layouter.AddButton(reset_level_button, -9, 2, 3, 0, 1, 1, true);

	v_set(GetVariableScope(), kRtvarPhysicsHalt, true);
}

void BoundManager::OnPauseButton(uitbc::Button* button) {
	if (button) {
		menu_->OnTapSound(button);
	}
	pause_button_->SetVisible(false);

	bool run_ads;
	v_get(run_ads, =, GetVariableScope(), kRtvarGameRunads, true);
	bool did_buy = !run_ads;
	uitbc::Dialog* d = menu_->CreateTbcDialog(life::Menu::ButtonAction(this, &BoundManager::OnMenuAlternative), 0.6f, did_buy? 0.6f : 0.7f);
	d->SetColor(BG_COLOR, OFF_BLACK, BLACK, BLACK);
	d->SetDirection(+1, false);
	uitbc::FixedLayouter layouter(d);
	int row = 0;
	const int row_count = did_buy? 3 : 4;

	bool is_paused = false;
	if (LEVEL_DONE()) {
		uitbc::Label* label = new uitbc::Label(BRIGHT_TEXT, L"Level completed (85%)");
		label->SetFontId(ui_manager_->SetScaleFont(1.2f));
		ui_manager_->SetMasterFont();
		label->SetIcon(uitbc::Painter::kInvalidImageid, uitbc::TextComponent::kIconCenter);
		label->SetAdaptive(false);
		layouter.AddComponent(label, row++, row_count, 0, 1, 1);

		uitbc::Button* next_level_button = new uitbc::Button(GREEN_BUTTON, L"Next level");
		layouter.AddButton(next_level_button, -1, row++, row_count, 0, 1, 1, true);
	} else {
		uitbc::Label* label;
		if (cuts_left_ > 0) {
			label = new uitbc::Label(BRIGHT_TEXT, L"Paused");
			is_paused = true;
		} else {
			label = new uitbc::Label(RED_BUTTON, L"Out of cuts!");
		}
		label->SetFontId(ui_manager_->SetScaleFont(1.2f));
		ui_manager_->SetMasterFont();
		label->SetIcon(uitbc::Painter::kInvalidImageid, uitbc::TextComponent::kIconCenter);
		label->SetAdaptive(false);
		layouter.AddComponent(label, row++, row_count, 0, 1, 1);
	}

	uitbc::Button* reset_level_button = new uitbc::Button(ORANGE_BUTTON, L"Reset level");
	layouter.AddButton(reset_level_button, -3, row++, row_count, 0, 1, 1, true);

	if (row < 3) {
		uitbc::Button* restart_from1st_level_button = new uitbc::Button(RED_BUTTON, L"Reset game");
		layouter.AddButton(restart_from1st_level_button, -4, row++, row_count, 0, 1, 1, true);
	}

#ifdef LEPRA_TOUCH
	if (!did_buy) {
		uitbc::Button* buy_button = new uitbc::Button(BLACK_BUTTON, L"Buy full");
		buy_button->SetFontColor(DIM_TEXT);
		layouter.AddButton(buy_button, -5, row++, row_count, 0, 1, 1, true);
	}
#endif // touch device.

	if (is_paused) {
		uitbc::Button* close_button = new uitbc::Button(DIM_RED, L"X");
		layouter.AddCornerButton(close_button, -9);
	}

	v_set(GetVariableScope(), kRtvarPhysicsHalt, true);
}

void BoundManager::OnMenuAlternative(uitbc::Button* button) {
	int level;
	v_get(level, =, GetVariableScope(), kRtvarGameLevel, 0);
	switch (button->GetTag()) {
		case -1:	GetConsoleManager()->PushYieldCommand("step-level +1");				break;
		case -2:	GetConsoleManager()->PushYieldCommand("step-level -1");				break;
		case -3:	GetConsoleManager()->PushYieldCommand("step-level 0");				break;
		case -4:	GetConsoleManager()->PushYieldCommand(strutil::Format("step-level %i", -level));	break;
		case -5:	Bound__Buy();										break;
	}
	pause_button_->SetVisible(true);
	HiResTimer::StepCounterShadow();
	v_set(GetVariableScope(), kRtvarPhysicsHalt, false);
}



void BoundManager::ScriptPhysicsTick() {
	// Camera moves in a "moving average" kinda curve (halfs the distance in x seconds).
	const float physics_time = GetTimeManager()->GetAffordedPhysicsTotalTime();
	if (physics_time > 1e-5) {
		bool is_first_time;
		v_get(is_first_time, =, GetVariableScope(), kRtvarGameFirsttime, true);
		if (is_first_time) {
			ShowInstruction();
			v_set(GetVariableScope(), kRtvarGameFirsttime, false);
		}
		if (!cut_vertices_.empty() || !cut_window_vertices_.empty() || force_cut_window_) {
			TimeLogger time_logger(&log_, "mLevel->SetTriangles");
			if (last_cut_mode_ == kCutAddWindow) {
				level_->AddCutPlane(GetPhysicsManager(), last_cut_plane_, cut_window_vertices_, Color(90, 100, 210, 180));
			} else {
				float pre_volume = level_->GetVolumePart();
				if (!cut_vertices_.empty()) {
					level_->SetTriangles(GetPhysicsManager(), cut_vertices_, cut_colors_);
				}
				if (!cut_window_vertices_.empty() || force_cut_window_) {
					level_->SetWindowTriangles(cut_window_vertices_);
				}
				float post_volume = level_->GetVolumePart();
				float amount = std::max(0.0f, pre_volume-post_volume);
				level_score_ += 1000 * amount * (1 + quick_cut_count_*0.8f);
			}
			cut_vertices_.clear();
			cut_window_vertices_.clear();
			cut_colors_.clear();
			force_cut_window_ = false;
		}
		percent_done_ = 100-level_->GetVolumePart()*100;
		if (LEVEL_DONE() && !menu_->GetDialog()) {
			DidFinishLevel();
		} else if (cuts_left_ <= 0 && !menu_->GetDialog()) {
			OnPauseButton(0);
		}
		vec3 acceleration = ui_manager_->GetAccelerometer();
		acceleration.x = -acceleration.x;
		acceleration.z = -acceleration.z;
		float acceleration_length = acceleration.GetLength();
		is_shaking_ = (acceleration_length > 1.6f);
		bool is_really_shaking = false;
		double shake_time = shake_timer_.QuerySplitTime();
		if (shake_time > 6.0) {
			is_shaking_ = false;
			shake_timer_.Stop();
			shake_timer_.PopTimeDiff();
		} else if (shake_time > 2.5) {
			// Intermission.
			is_shaking_ = true;
			delete shake_sound_;
			shake_sound_ = 0;
		} else if (is_shaking_ && shakes_left_ > 0) {
			if (acceleration_length > 3) {
				acceleration.Normalize(3);
			}
			acceleration = camera_transform_.GetOrientation() * acceleration;
			if (!shake_timer_.IsStarted()) {
				shake_timer_.Start();
				delete shake_sound_;
				UiCure::UserSound2dResource* break_sound = new UiCure::UserSound2dResource(ui_manager_, uilepra::SoundManager::kLoopForward);
				shake_sound_ = new UiCure::SoundReleaser(GetResourceManager(), ui_manager_, GetContext(), "shake.wav", break_sound, 0.5f, 1.0);
				level_score_ -= 1000;
				--shakes_left_;
			}
			is_really_shaking = true;
		} else if (shake_timer_.IsStarted()) {
			is_shaking_ = true;
			if (acceleration_length > 1.2f) {
				// Even though the user isn't having high Gs right now, we still follow the user acceleration a bit.
				acceleration = camera_transform_.GetOrientation() * acceleration;
				is_really_shaking = true;
			}
		}
		vec3 center;
		std::vector<cure::GameObjectId>::iterator x;
		for (x = balls_.begin(); x != balls_.end(); ++x) {
			cure::ContextObject* ball = GetContext()->GetObject(*x);
			if (!ball) {
				continue;
			}
			vec3 p = ball->GetPosition();
			center += p;
			ball->SetInitialTransform(xform(quat(), p));

			if (is_really_shaking) {
				vec3 v = ball->GetVelocity();
				v += acceleration*3.0f;
				GetPhysicsManager()->SetBodyVelocity(ball->GetPhysics()->GetBoneGeometry(0)->GetBodyId(), v);
			}
		}
		int level;
		v_get(level, =, GetVariableScope(), kRtvarGameLevel, 0);
		if ((int)balls_.size() < level+1) {
			center /= (float)balls_.size();
			CreateBall(balls_.size(), &center);
		}
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

void BoundManager::HandleWorldBoundaries() {
	std::vector<cure::GameObjectId> lost_object_array;
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
		std::vector<cure::GameObjectId>::const_iterator y = lost_object_array.begin();
		for (; y != lost_object_array.end(); ++y) {
			DeleteContextObject(*y);
			std::vector<cure::GameObjectId>::iterator x;
			for (x = balls_.begin(); x != balls_.end(); ++x) {
				if (*x == *y) {
					balls_.erase(x);
					break;
				}
			}
		}
	}
}

void BoundManager::MoveCamera(float frame_time) {
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

void BoundManager::UpdateCameraPosition(bool update_mic_position) {
	ui_manager_->SetCameraPosition(camera_transform_);
	if (update_mic_position) {
		ui_manager_->SetMicrophonePosition(camera_transform_, vec3());
	}
}



void BoundManager::PrintText(const wstr& s, int x, int y) const {
	Color old_color = ui_manager_->GetPainter()->GetColor(0);
	ui_manager_->GetPainter()->SetColor(DARK_BLUE, 0);
	ui_manager_->GetPainter()->PrintText(s, x, y+1);
	ui_manager_->GetPainter()->SetColor(old_color, 0);
	ui_manager_->GetPainter()->PrintText(s, x, y);
}

void BoundManager::DrawImage(uitbc::Painter::ImageID image_id, float cx, float cy, float w, float h, float angle) const {
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



void BoundManager::PainterImageLoadCallback(UiCure::UserPainterKeepImageResource* resource) {
	if (resource->GetLoadState() == cure::kResourceLoadComplete) {
		ui_manager_->GetDesktopWindow()->GetImageManager()->AddLoadedImage(*resource->GetRamData(), resource->GetData(),
			uitbc::GUIImageManager::kCentered, uitbc::GUIImageManager::kAlphablend, 255);
	}
}



loginstance(kGame, BoundManager);



}
