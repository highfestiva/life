
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once



#include "../../lepra/include/transformation.h"
#include "../../uilepra/include/uidisplaymanager.h"
#include "../../uitbc/include/uifontmanager.h"
#include "uicure.h"



namespace lepra {
class Canvas;
}
namespace uilepra {
namespace touch {
class DragManager;
}
class InputManager;
class SoundManager;
}
namespace uitbc {
class DesktopWindow;
class Layout;
class Painter;
class Renderer;
}



namespace UiCure {



class GameUiManager: public uilepra::DisplayResizeObserver {
public:
	GameUiManager(cure::RuntimeVariableScope* variable_scope, uilepra::touch::DragManager* drag_manager);
	virtual ~GameUiManager();

	bool Open();
	bool OpenDraw();	// Only opens the renderer+painter.
	bool OpenRest();	// Opens rest; fonts, GUI, input, sound, etc.
	void Close();
	void DeleteDesktopWindow();

	bool CanRender() const;

	void InputTick();
	void EndInputTick();
	void BeginRender(const vec3& background_color);
	void Render(const PixelRect& area);
	void Paint(bool clear_depth_buffer);
	void PreparePaint(bool clear_depth_buffer);
	void EndRender(float frame_time);

	cure::RuntimeVariableScope* GetVariableScope() const;
	void SetVariableScope(cure::RuntimeVariableScope* variable_scope);
	uilepra::DisplayManager* GetDisplayManager() const;
	Canvas* GetCanvas() const;
	uitbc::Renderer* GetRenderer() const;
	uitbc::Painter* GetPainter() const;
	uitbc::FontManager* GetFontManager() const;
	uilepra::InputManager* GetInputManager() const;
	uilepra::touch::DragManager* GetDragManager() const;
	uitbc::DesktopWindow* GetDesktopWindow() const;
	uilepra::SoundManager* GetSoundManager() const;

	vec3 GetAccelerometer() const;
	void SetCameraPosition(const xform& transform);
	void SetMicrophonePosition(const xform& transform, const vec3& velocity);
	void SetViewport(int left, int top, int width, int height);
	void Clear(float red, float green, float blue, bool clear_depth = true);
	void ClearDepth();

	uitbc::FontManager::FontId SetScaleFont(float scale);
	void SetMasterFont();
	void PrintText(int x, int y, const wstr& text);

	PixelCoord GetMouseDisplayPosition() const;

	void AssertDesktopLayout(uitbc::Layout* layout, int layer);

	void UpdateSettings();

private:
	void OnResize(int width, int height);
	void OnMinimize();
	void OnMaximize(int width, int height);

	cure::RuntimeVariableScope* variable_scope_;
	uilepra::DisplayManager* display_;
	Canvas* canvas_;
	uitbc::Renderer* renderer_;
	uitbc::Painter* painter_;
	uitbc::FontManager* font_manager_;
	uitbc::DesktopWindow* desktop_window_;
	uilepra::InputManager* input_;
	uilepra::touch::DragManager* drag_manager_;
	uilepra::SoundManager* sound_;
	double sound_roll_off_shadow_;	// Optimization.
	double sound_doppler_shadow_;	// Optimization.
	uitbc::FontManager::FontId current_font_id_;

	logclass();
};



}
