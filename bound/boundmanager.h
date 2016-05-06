
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../lepra/include/plane.h"
#include "../life/lifeclient/gameclientslavemanager.h"
#include "../life/lifeclient/menu.h"
#include "../uicure/include/uiresourcemanager.h"
#include "bound.h"



namespace lepra {
class Plane;
}
namespace uilepra {
namespace touch {
class TouchstickInputDevice;
}
}
namespace UiCure {
class CollisionSoundManager;
class CppContextObject;
class SoundReleaser;
}
namespace uitbc {
class Button;
}


namespace Bound {



class Level;
class Sunlight;



class BoundManager: public life::GameClientSlaveManager {
	typedef life::GameClientSlaveManager Parent;
public:
	enum CutMode {
		kCutNormal,
		kCutAddWindow,
		kCutWindowItself,
	};

	BoundManager(life::GameClientMasterTicker* pMaster, const cure::TimeManager* time,
		cure::RuntimeVariableScope* variable_scope, cure::ResourceManager* resource_manager,
		UiCure::GameUiManager* ui_manager, int slave_index, const PixelRect& render_area);
	virtual ~BoundManager();
	virtual void Suspend(bool hard);
	virtual void LoadSettings();
	virtual void SaveSettings();
	virtual bool Open();
	virtual void Close();
	virtual void SetIsQuitting();
	virtual void SetFade(float fade_amount);

	virtual bool Render();
	void RenderBackground();
	virtual bool Paint();

	bool HandleCutting();
	Plane ScreenLineToPlane(const PixelCoord& coord, const	PixelCoord& end_point, Plane& cut_plane_delimiter);
	bool Cut(Plane cut_plane);
	bool DoCut(const uitbc::TriangleBasedGeometry* mesh, Plane cut_plane, CutMode cut_mode);
	void AddTriangle(const vec3& v0, const vec3& v1, const vec3& v2, const uint8* colors);
	void AddNGonPoints(std::vector<vec3>& n_gon, std::unordered_set<int>& n_gon_map, const vec3& p0, const vec3& p1);
	static void AddNGonPoint(std::vector<vec3>& n_gon, std::unordered_set<int>& n_gon_map, const vec3& p);
	void CreateNGon(std::vector<vec3>& n_gon);
	void LineUpNGonBorders(std::vector<vec3>& n_gon, bool sort);
	void SimplifyNGon(std::vector<vec3>& n_gon);
	void AddNGonTriangles(const Plane& cut_plane, const std::vector<vec3>& n_gon, const uint8* colors);
	int CheckIfPlaneSlicesBetweenBalls(const Plane& cut_plane);
	bool CheckBallsPlaneCollition(const Plane& cut_plane, const Plane* cut_plane_delimiter, vec3& collision_point);
	static bool AttachTouchToBorder(PixelCoord& point, int margin, int width, int height);

	bool SetAvatarEnginePower(unsigned aspect, float power);

	virtual bool DidFinishLevel();
	virtual int StepLevel(int count);

	cure::RuntimeVariableScope* GetVariableScope() const;

protected:
	typedef uilepra::touch::TouchstickInputDevice Touchstick;

	virtual bool InitializeUniverse();
	virtual void TickInput();
	virtual void TickUiInput();
	virtual void TickUiUpdate();
	virtual void SetLocalRender(bool render);

	void CreateBall(int index, const vec3* position);
	virtual cure::ContextObject* CreateContextObject(const str& class_id) const;
	//virtual cure::ContextObject* CreateLogicHandler(const str& type);
	virtual void OnLoadCompleted(cure::ContextObject* object, bool ok);
	void OnCollision(const vec3& force, const vec3& torque, const vec3& position,
		cure::ContextObject* object1, cure::ContextObject* object2,
		tbc::PhysicsManager::BodyID body1_id, tbc::PhysicsManager::BodyID body2_id);

	void ShowInstruction();
	void OnPauseButton(uitbc::Button*);
	void OnMenuAlternative(uitbc::Button* button);

	virtual void ScriptPhysicsTick();
	virtual void HandleWorldBoundaries();
	virtual void MoveCamera(float frame_time);
	virtual void UpdateCameraPosition(bool update_mic_position);

	void PrintText(const wstr& s, int x, int y) const;
	void DrawImage(uitbc::Painter::ImageID image_id, float cx, float cy, float w, float h, float angle) const;

	void PainterImageLoadCallback(UiCure::UserPainterKeepImageResource* resource);

	UiCure::CollisionSoundManager* collision_sound_manager_;
	Level* level_;
	std::vector<float> cut_vertices_;
	std::vector<float> cut_window_vertices_;
	std::vector<uint8> cut_colors_;
	bool force_cut_window_;
	std::vector<cure::GameObjectId> balls_;
	life::Menu* menu_;
	StopWatch next_level_timer_;
	Sunlight* sunlight_;
	float camera_angle_;
	float camera_rotate_speed_;
	xform camera_transform_;
	float percent_done_;
	bool level_completed_;
	uitbc::Button* pause_button_;
	bool is_cutting_;
	bool is_shaking_;
	int cuts_left_;
	int shakes_left_;
	StopWatch shake_timer_;
	Plane last_cut_plane_;
	CutMode last_cut_mode_;
	HiResTimer cut_timer_;
	float cut_sound_pitch_;
	int quick_cut_count_;
	float level_score_;
	UiCure::SoundReleaser* shake_sound_;
	logclass();
};



}
