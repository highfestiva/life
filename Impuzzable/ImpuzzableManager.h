
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../life/lifeclient/gameclientslavemanager.h"
#include "../life/lifeclient/menu.h"
#include "../uicure/include/uiresourcemanager.h"
#include "impuzzable.h"



namespace uilepra {
namespace touch {
struct Drag;
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


namespace Impuzzable {



class Level;
class Piece;
class Sunlight;



class ImpuzzableManager: public life::GameClientSlaveManager {
	typedef life::GameClientSlaveManager Parent;
public:
	typedef enum CutMode {
		kCutNormal,
		kCutAddWindow,
		kCutWindowItself,
	};

	ImpuzzableManager(life::GameClientMasterTicker* pMaster, const cure::TimeManager* time,
		cure::RuntimeVariableScope* variable_scope, cure::ResourceManager* resource_manager,
		UiCure::GameUiManager* ui_manager, int slave_index, const PixelRect& render_area);
	virtual ~ImpuzzableManager();
	virtual void Suspend();
	virtual void LoadSettings();
	virtual void SaveSettings();
	virtual bool Open();
	virtual void Close();
	virtual void SetIsQuitting();
	virtual void SetFade(float fade_amount);

	virtual bool Render();
	void RenderBackground();
	virtual bool Paint();

	void HandleDrag();
	Piece* PickPiece(uilepra::touch::Drag& drag, int radius);
	Piece* GetDraggedPiece(uilepra::touch::Drag& drag);
	void DragPiece(Piece* piece, const PixelCoord& screen_point);
	vec3 To3dPoint(const PixelCoord& coord, float depth) const;

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

	void CreatePiece(int index, const vec3* position);
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
	virtual void HandleWorldImpuzzablearies();
	virtual void MoveCamera(float frame_time);
	virtual void UpdateCameraPosition(bool update_mic_position);

	void PrintText(const str& s, int x, int y) const;
	void DrawImage(uitbc::Painter::ImageID image_id, float cx, float cy, float w, float h, float angle) const;

	void PainterImageLoadCallback(UiCure::UserPainterKeepImageResource* resource);

	UiCure::CollisionSoundManager* collision_sound_manager_;
	Level* level_;
	std::vector<float> cut_vertices_;
	std::vector<float> cut_window_vertices_;
	std::vector<uint8> cut_colors_;
	bool force_cut_window_;
	std::vector<Piece*> pieces_;
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
	CutMode last_cut_mode_;
	HiResTimer cut_timer_;
	float cut_sound_pitch_;
	int quick_cut_count_;
	float level_score_;
	UiCure::SoundReleaser* shake_sound_;
	logclass();
};



}
