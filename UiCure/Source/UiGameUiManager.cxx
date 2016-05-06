
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../cure/include/gamemanager.h"
#include "../../cure/include/runtimevariable.h"
#include "../../lepra/include/math.h"
#include "../../lepra/include/lepratarget.h"
#include "../../lepra/include/thread.h"
#include "../../tbc/include/physicsmanager.h"
#include "../../uilepra/include/uicore.h"
#include "../../uilepra/include/uitouchdrag.h"
#include "../../uilepra/include/uiinput.h"
#include "../../uilepra/include/uisoundmanager.h"
#include "../../uitbc/include/gui/uidesktopwindow.h"
#include "../../uitbc/include/gui/uifloatinglayout.h"
#include "../../uitbc/include/uiopenglpainter.h"
#include "../../uitbc/include/uiopenglrenderer.h"
#include "../include/uigameuimanager.h"
#include "../include/uiruntimevariablename.h"



namespace UiCure {



GameUiManager::GameUiManager(cure::RuntimeVariableScope* variable_scope, uilepra::touch::DragManager* drag_manager):
	variable_scope_(variable_scope),
	display_(0),
	canvas_(0),
	renderer_(0),
	painter_(0),
	font_manager_(0),
	desktop_window_(0),
	input_(0),
	drag_manager_(drag_manager),
	sound_(0),
	sound_roll_off_shadow_(0),
	sound_doppler_shadow_(0),
	current_font_id_(uitbc::FontManager::kInvalidFontid) {
}

GameUiManager::~GameUiManager() {
	Close();
	variable_scope_ = 0;
	drag_manager_ = 0;
}



bool GameUiManager::Open() {
	bool ok = OpenDraw();
	if (ok) {
		ok = OpenRest();
	}
	return ok;
}

bool GameUiManager::OpenDraw() {
	str render_type_string;
	int display_width;
	int display_height;
	int display_bpp;
	int display_frequency;
	bool display_full_screen;
	v_get(render_type_string, =, variable_scope_, kRtvarUiDisplayRenderengine, "OpenGL");
	v_get(display_width, =, variable_scope_, kRtvarUiDisplayWidth, 640);
	v_get(display_height, =, variable_scope_, kRtvarUiDisplayHeight, 480);
	v_get(display_bpp, =, variable_scope_, kRtvarUiDisplayBitsperpixel, 0);
	v_get(display_frequency, =, variable_scope_, kRtvarUiDisplayFrequency, 0);
	v_get(display_full_screen, =, variable_scope_, kRtvarUiDisplayFullscreen, false);

	uilepra::DisplayManager::ContextType rendering_context = uilepra::DisplayManager::kOpenglContext;
	if (render_type_string == "OpenGL") {
		rendering_context = uilepra::DisplayManager::kOpenglContext;
	} else if (render_type_string == "DirectX") {
		rendering_context = uilepra::DisplayManager::kDirectxContext;
	}

	// Initialize kUi based on settings parameters.
	bool ok = true;
	display_ = uilepra::DisplayManager::CreateDisplayManager(rendering_context);
	uilepra::DisplayMode display_mode;
	display_mode.width_ = display_width;
	display_mode.height_ = display_height;
	display_mode.refresh_rate_ = 0;
	display_mode.bit_depth_ = 0;
#ifndef LEPRA_TOUCH
	if (display_bpp > 0 && display_frequency > 0) {
		ok = display_->FindDisplayMode(display_mode, display_width, display_height, display_bpp, display_frequency);
	} else if (display_bpp > 0) {
		ok = display_->FindDisplayMode(display_mode, display_width, display_height, display_bpp);
	} else {
		ok = display_->FindDisplayMode(display_mode, display_width, display_height);
	}
	if (!ok) {
		if (display_full_screen) {
			log_.Errorf("Unsupported resolution %ux%u.", display_width, display_height);
		} else {
			log_.Infof("Note that resolution %ux%u only supported in windowed mode.", display_width, display_height);
			ok = true;	// Go ahead - running in a window is OK.
			display_mode.width_ = display_width;
			display_mode.height_ = display_height;
			display_mode.bit_depth_ = display_bpp;
			display_mode.refresh_rate_ = display_frequency;
		}
	}
#endif // touch device.
	if (ok) {
		if (display_full_screen) {
			ok = display_->OpenScreen(display_mode, uilepra::DisplayManager::kFullscreen, uilepra::DisplayManager::kOrientationAllowUpsideDown);
		} else {
			ok = display_->OpenScreen(display_mode, uilepra::DisplayManager::kWindowed, uilepra::DisplayManager::kOrientationAllowUpsideDown);
		}
	}
	if (ok) {
		display_->AddResizeObserver(this);
		const double physical_screen_size = display_->GetPhysicalScreenSize();
		v_override(variable_scope_, kRtvarUiDisplayPhysicalsize, physical_screen_size);

		canvas_ = new Canvas(display_mode.width_, display_mode.height_, Canvas::IntToBitDepth(display_mode.bit_depth_));
	}
	if (ok) {
		if (rendering_context == uilepra::DisplayManager::kOpenglContext) {
			renderer_ = new uitbc::OpenGLRenderer(canvas_);
			painter_ = new uitbc::OpenGLPainter;
		}
		/*else if (context == uilepra::DisplayManager::kDirectxContext) {
			renderer_ = new tbc::Direct3DRenderer(canvas_);
			painter_ = new tbc::DirectXPainter;
		}*/
	}
	if (ok) {
		painter_->SetDestCanvas(canvas_);
	}
	uilepra::Core::ProcessMessages();
	return ok;
}

bool GameUiManager::OpenRest() {
	str sound_type_string;
	v_get(sound_type_string, =, variable_scope_, kRtvarUiSoundEngine, "OpenAL");
	uilepra::SoundManager::ContextType sound_context = uilepra::SoundManager::kContextOpenal;
	if (sound_type_string == "OpenAL") {
		sound_context = uilepra::SoundManager::kContextOpenal;
	} else if (sound_type_string == "FMOD") {
		sound_context = uilepra::SoundManager::kContextFmod;
	}

	bool ok = true;
	if (ok) {
		font_manager_ = uitbc::FontManager::Create(display_);
		painter_->SetFontManager(font_manager_);

		str font;
		v_get(font, =, variable_scope_, kRtvarUi2DFont, "Times New Roman");
		double font_height;
		v_get(font_height, =, variable_scope_, kRtvarUi2DFontheight, 14.0);
		int font_flags;
		v_get(font_flags, =, variable_scope_, kRtvarUi2DFontflags, 0);
		uitbc::FontManager::FontId font_id = font_manager_->QueryAddFont(font, font_height, font_flags);
		const char* font_names[] =
		{
			"Times New Roman",
			"Arial",
			"Verdana",
			"Helvetica",
			"Courier New",
			"Segoe UI",
			"Open Sans",
			"Liberation Sans",
			"Liberation Serif",
			"Nimbus",
			"Cantarell",
			"Bookman",
			"Gothic",
			"Sans",
			"Serif",
			"",
			0
		};
		for (int x = 0; font_names[x] && font_id == uitbc::FontManager::kInvalidFontid; ++x) {
			font = font_names[x];
			font_id = font_manager_->QueryAddFont(font, font_height, font_flags);
		}
		if (font_id != uitbc::FontManager::kInvalidFontid) {
			v_override(variable_scope_, kRtvarUi2DFont, font);
		}
		current_font_id_ = font_id;
	}
	if (ok) {
		input_ = uilepra::InputManager::CreateInputManager(display_);
		input_->ActivateAll();
	}
	if (ok) {
		desktop_window_ = new uitbc::DesktopWindow(input_, painter_, new uitbc::FloatingLayout(), 0, 0);
		desktop_window_->SetIsHollow(true);
		desktop_window_->SetPreferredSize(canvas_->GetWidth(), canvas_->GetHeight());
	}
	if (ok) {
		sound_ = uilepra::SoundManager::CreateSoundManager(sound_context);
	}
	if (ok) {
		// Cut sound some slack. Sometimes OpenAL crashes during startup.
		for (int x = 0; x < 3; ++x) {
			uilepra::Core::ProcessMessages();
			Thread::Sleep(0.001f);
		}
		UpdateSettings();
	}

	uilepra::Core::ProcessMessages();
	return ok;
}

void GameUiManager::Close() {
	if (display_) {
		display_->RemoveResizeObserver(this);
	}

	// Poll system to let go of old windows.
	uilepra::Core::ProcessMessages();
	Thread::Sleep(0.001);
	uilepra::Core::ProcessMessages();

	delete (sound_);
	sound_ = 0;

	delete (desktop_window_);
	desktop_window_ = 0;

	delete (input_);
	input_ = 0;

	delete (font_manager_);
	font_manager_ = 0;
	delete (painter_);
	painter_ = 0;
	delete (renderer_);
	renderer_ = 0;
	delete (canvas_);
	canvas_ = 0;

	delete (display_);
	display_ = 0;

	// Poll system to let go of old windows.
	uilepra::Core::ProcessMessages();
	Thread::Sleep(0.001);
	uilepra::Core::ProcessMessages();
}

void GameUiManager::DeleteDesktopWindow() {
	delete (desktop_window_);
	desktop_window_ = 0;
}



bool GameUiManager::CanRender() const {
	return display_->IsVisible();
}



void GameUiManager::InputTick() {
	if (CanRender()) {
		input_->PreProcessEvents();
	}
	uilepra::Core::ProcessMessages();
	if (CanRender()) {
		input_->PollEvents();
	}

	if (drag_manager_) {
		const float drag_length_in_inches = 0.5f;
		const int drag_pixels = (int)(GetCanvas()->GetWidth() * drag_length_in_inches / GetDisplayManager()->GetPhysicalScreenSize());
		drag_manager_->SetMaxDragDistance(drag_pixels);
#if defined(LEPRA_TOUCH)
		drag_manager_->UpdateMouseByDrag(GetInputManager());
#else // Not a touch device.
		bool emulate_touch;
		v_get(emulate_touch, =, variable_scope_, kRtvarCtrlEmulatetouch, false);
		if (emulate_touch) {
			drag_manager_->UpdateDragByMouse(GetInputManager());
		}
#endif // touch device / Not a touch device.
		drag_manager_->UpdateTouchsticks(GetInputManager());
		// Check if the actual tap/click has been consumed by any button or such.
		if (desktop_window_ && desktop_window_->GetMouseButtonFlags()&uitbc::DesktopWindow::kConsumedMouseButton) {
			drag_manager_->SetDraggingUi(true);
		}
	}
}

void GameUiManager::EndInputTick() {
	if (drag_manager_) {
		drag_manager_->DropReleasedDrags();
	}
}

void GameUiManager::BeginRender(const vec3& background_color) {
	if (CanRender()) {
		renderer_->ResetClippingRect();
		bool enable_clear;
		v_get(enable_clear, =, variable_scope_, kRtvarUi3DEnableclear, true);
		if (enable_clear) {
			Clear(background_color.x, background_color.y, background_color.z);
		} else {
			ClearDepth();
		}

		float master_volume;
		v_get(master_volume, =(float), variable_scope_, kRtvarUiSoundMastervolume, 1.0);
		sound_->SetMasterVolume(master_volume);
		float music_volume;
		v_get(music_volume, =(float), variable_scope_, kRtvarUiSoundMusicvolume, 1.0);
		sound_->SetMusicVolume(music_volume);
	} else {
		sound_->SetMasterVolume(0);
	}
}

void GameUiManager::Render(const PixelRect& area) {
	if (CanRender()) {
		if (area.GetWidth() > 0 && area.GetHeight() > 0) {
			renderer_->SetClippingRect(area);
			renderer_->SetViewport(area);
			renderer_->RenderScene();
		}
	}
}

void GameUiManager::Paint(bool clear_depth_buffer) {
	if (CanRender()) {
		canvas_->SetBuffer(0);
		painter_->SetDestCanvas(canvas_);
		float r, g, b;
		renderer_->GetAmbientLight(r, g, b);
		renderer_->SetAmbientLight(0.1f, 0.1f, 0.1f);
		PreparePaint(clear_depth_buffer);
		desktop_window_->Repaint(painter_);
		renderer_->SetAmbientLight(r, g, b);
	}
}

void GameUiManager::PreparePaint(bool clear_depth_buffer) {
	painter_->ResetClippingRect();
	painter_->PrePaint(clear_depth_buffer);
}

void GameUiManager::EndRender(float frame_time) {
	if (CanRender()) {
		UpdateSettings();
		display_->UpdateScreen();

		renderer_->Tick(frame_time);
	}
}



cure::RuntimeVariableScope* GameUiManager::GetVariableScope() const {
	return variable_scope_;
}

void GameUiManager::SetVariableScope(cure::RuntimeVariableScope* variable_scope) {
	variable_scope_ = variable_scope;
}

uilepra::DisplayManager* GameUiManager::GetDisplayManager() const {
	return display_;
}

Canvas* GameUiManager::GetCanvas() const {
	return canvas_;
}

uitbc::Renderer* GameUiManager::GetRenderer() const {
	return renderer_;
}

uitbc::Painter* GameUiManager::GetPainter() const {
	return painter_;
}

uitbc::FontManager* GameUiManager::GetFontManager() const {
	return font_manager_;
}

uilepra::InputManager* GameUiManager::GetInputManager() const {
	return input_;
}

uilepra::touch::DragManager* GameUiManager::GetDragManager() const {
	return drag_manager_;
}

uitbc::DesktopWindow* GameUiManager::GetDesktopWindow() const {
	return desktop_window_;
}

uilepra::SoundManager* GameUiManager::GetSoundManager() const {
	return sound_;
}



vec3 GameUiManager::GetAccelerometer() const {
	float x, y, z;
	v_get(x, =(float), GetVariableScope(), kRtvarCtrlAccelerometerX,  0.0);
	v_get(y, =(float), GetVariableScope(), kRtvarCtrlAccelerometerY,  0.0);
	v_get(z, =(float), GetVariableScope(), kRtvarCtrlAccelerometerZ, -1.0);
	return vec3(x,y,z);
}

void GameUiManager::SetCameraPosition(const xform& transform) {
	renderer_->SetCameraTransformation(transform);
}

void GameUiManager::SetMicrophonePosition(const xform& transform, const vec3& velocity) {
	vec3 up = transform.GetOrientation() * vec3(0,0,1);
	vec3 forward = transform.GetOrientation() * vec3(0,1,0);
	sound_->SetListenerPosition(transform.GetPosition(), velocity, up, forward);
}

void GameUiManager::SetViewport(int left, int top, int display_width, int display_height) {
	if(display_width < 0 || display_height < 0) {
		return;
	}
	renderer_->SetViewport(PixelRect(left, top, left+display_width, top+display_height));
	renderer_->SetClippingRect(PixelRect(left, top, left+display_width, top+display_height));
}

void GameUiManager::Clear(float red, float green, float blue, bool clear_depth) {
	//display_->Activate();

	Color color;
	color.Set(red, green, blue, 1.0f);
	renderer_->SetClearColor(color);
	unsigned clear_flags = uitbc::Renderer::kClearColorbuffer;
	if (clear_depth) {
		clear_flags |= uitbc::Renderer::kClearDepthbuffer;
	}
	renderer_->Clear(clear_flags);
}

void GameUiManager::ClearDepth() {
	renderer_->Clear(uitbc::Renderer::kClearDepthbuffer);
}



uitbc::FontManager::FontId GameUiManager::SetScaleFont(float scale) {
	SetMasterFont();
	str font;
	v_get(font, =, variable_scope_, kRtvarUi2DFont, "Times New Roman");
	double font_height;
	if (scale > 0) {
		v_get(font_height, =, variable_scope_, kRtvarUi2DFontheight, 14.0);
		font_height *= scale;
	} else {
		font_height = -scale;
	}
	int font_flags;
	v_get(font_flags, =, variable_scope_, kRtvarUi2DFontflags, 0);
	return font_manager_->QueryAddFont(font, font_height, font_flags);
}

void GameUiManager::SetMasterFont() {
	if (current_font_id_ != uitbc::FontManager::kInvalidFontid) {
		font_manager_->SetActiveFont(current_font_id_);
	} else {
		current_font_id_ = font_manager_->GetActiveFontId();
	}
}

void GameUiManager::PrintText(int _x, int _y, const wstr& text) {
	//painter_->ResetClippingRect();
	painter_->SetColor(Color(255, 255, 255, 255), 0);
	painter_->SetColor(Color(0, 0, 0, 0), 1);
	painter_->PrintText(text, _x, _y);
}



PixelCoord GameUiManager::GetMouseDisplayPosition() const {
	return (PixelCoord((int)((GetInputManager()->GetCursorX() + 1) * 0.5 * GetDisplayManager()->GetWidth()),
		(int)((GetInputManager()->GetCursorY() + 1) * 0.5 * GetDisplayManager()->GetHeight())));
}



void GameUiManager::AssertDesktopLayout(uitbc::Layout* layout, int layer) {
	if (GetDesktopWindow()->GetLayout(layer)->GetType() != layout->GetType()) {
		GetDesktopWindow()->ReplaceLayer(layer, layout);
	} else {
		delete (layout);
	}
}



void GameUiManager::UpdateSettings() {
	// Display.
	bool enable_v_sync;
	v_get(enable_v_sync, =, variable_scope_, kRtvarUiDisplayEnablevsync, true);
	display_->SetVSyncEnabled(enable_v_sync);

	str display_orientation;
	v_get(display_orientation, =, variable_scope_, kRtvarUiDisplayOrientation, "AllowUpsideDown");
	uilepra::DisplayManager::Orientation orientation = uilepra::DisplayManager::kOrientationAllowUpsideDown;
	if (display_orientation == "AllowAny") {
		orientation = uilepra::DisplayManager::kOrientationAllowAny;
	} else if (display_orientation == "AllowUpsideDown") {
		orientation = uilepra::DisplayManager::kOrientationAllowUpsideDown;
	} else if (display_orientation == "AllowAnyInternal") {
		orientation = uilepra::DisplayManager::kOrientationAllowAnyInternal;
	} else if (display_orientation == "AllowUpsideDownInternal") {
		orientation = uilepra::DisplayManager::kOrientationAllowUpsideDownInternal;
	} else if (display_orientation == "Fixed") {
		orientation = uilepra::DisplayManager::kOrientationFixed;
	}
	display_->SetOrientation(orientation);

	// ----------------------------------------

	// 3D rendering settings.
	bool enable_lights;
	double ambient_red;
	double ambient_green;
	double ambient_blue;
	bool enable_trilinear_filtering;
	bool enable_bilinear_filtering;
	bool enable_mip_mapping;
	bool enable_texturing;
	bool enable_pixel_shaders;
	double fog_near;
	double fog_far;
	double fog_density;
	double fog_exponent;
	double fov;
	double clip_near;
	double clip_far;
	str shadows_string;
	double shadow_deviation;
	v_get(enable_lights, =, variable_scope_, kRtvarUi3DEnablelights, true);
	v_get(ambient_red, =, variable_scope_, kRtvarUi3DAmbientred, 0.1);
	v_get(ambient_green, =, variable_scope_, kRtvarUi3DAmbientgreen, 0.1);
	v_get(ambient_blue, =, variable_scope_, kRtvarUi3DAmbientblue, 0.1);
	v_tryget(ambient_red, *=, variable_scope_, kRtvarUi3DAmbientredfactor, 1.0);
	v_tryget(ambient_green, *=, variable_scope_, kRtvarUi3DAmbientgreenfactor, 1.0);
	v_tryget(ambient_blue, *=, variable_scope_, kRtvarUi3DAmbientbluefactor, 1.0);
	/*if (!renderer_->IsPixelShadersEnabled()) {
		// Without pixel shader highlights the scene becomes darker. At least on my computer...
		ambient_red *= 1.2;
		ambient_green *= 1.2;
		ambient_blue *= 1.2;
	}*/

	v_get(enable_trilinear_filtering, =, variable_scope_, kRtvarUi3DEnabletrilinearfiltering, false);
	v_get(enable_bilinear_filtering, =, variable_scope_, kRtvarUi3DEnablebilinearfiltering, false);
	v_get(enable_mip_mapping, =, variable_scope_, kRtvarUi3DEnablemipmapping, true);
	v_get(enable_texturing, =, variable_scope_, kRtvarUi3DEnabletexturing, false);
	v_get(enable_pixel_shaders, =, variable_scope_, kRtvarUi3DPixelshaders, true);
	v_get(fog_near, =, variable_scope_, kRtvarUi3DFognear, 0.0);
	v_get(fog_far, =, variable_scope_, kRtvarUi3DFogfar, 0.0);
	v_get(fog_density, =, variable_scope_, kRtvarUi3DFogdensity, 1.0);
	v_get(fog_exponent, =, variable_scope_, kRtvarUi3DFogexponent, 0.0);
	v_get(fov, =, variable_scope_, kRtvarUi3DFov, 45.0);
	v_get(clip_near, =, variable_scope_, kRtvarUi3DClipnear, 0.1);
	v_get(clip_far, =, variable_scope_, kRtvarUi3DClipfar, 3000.0);
	v_get(shadows_string, =, variable_scope_, kRtvarUi3DShadows, "Volumes");
	v_get(shadow_deviation, =, variable_scope_, kRtvarUi3DShadowdeviation, 1e-5);

	renderer_->SetLightsEnabled(enable_lights);
	renderer_->SetAmbientLight((float)ambient_red, (float)ambient_green, (float)ambient_blue);
	renderer_->SetTrilinearFilteringEnabled(enable_trilinear_filtering);
	renderer_->SetBilinearFilteringEnabled(enable_bilinear_filtering);
	renderer_->SetMipMappingEnabled(enable_mip_mapping);
	renderer_->SetTexturingEnabled(enable_texturing);
	renderer_->EnablePixelShaders(enable_pixel_shaders);
	renderer_->SetFog((float)fog_near, (float)fog_far, (float)fog_density, (float)fog_exponent);
	renderer_->SetViewFrustum((float)fov, (float)clip_near, (float)clip_far);

	uitbc::Renderer::Shadows shadow_mode = uitbc::Renderer::kNoShadows;
	uitbc::Renderer::ShadowHint shadow_type = uitbc::Renderer::kShVolumesOnly;
	bool force_shadows_on_all = false;
	if (strutil::StartsWith(shadows_string, "Force:")) {
		shadows_string = shadows_string.substr(6);
		force_shadows_on_all = true;
	}
	if (shadows_string == "Volumes") {
		shadow_mode = uitbc::Renderer::kCastShadows;
		shadow_type = uitbc::Renderer::kShVolumesOnly;
	} else if (shadows_string == "VolumesAndMaps") {
		shadow_mode = uitbc::Renderer::kCastShadows;
		shadow_type = uitbc::Renderer::kShVolumesAndMaps;
	}
	if (force_shadows_on_all) {
		shadow_mode = uitbc::Renderer::kForceCastShadows;
	}
	renderer_->SetShadowMode(shadow_mode, shadow_type);
	renderer_->SetShadowUpdateFrameDelay(60);
	tbc::GeometryBase::SetDefaultBigOrientationThreshold((float)shadow_deviation);

	// ----------------------------------------
	// 2D rendering settings.
	str paint_mode_string;
	v_get(paint_mode_string, =, variable_scope_, kRtvarUi2DPaintmode, "AlphaBlend");
	uitbc::Painter::RenderMode painter_render_mode = uitbc::Painter::kRmAlphablend;
	if (paint_mode_string == "Add") {
		painter_render_mode = uitbc::Painter::kRmAdd;
	} else if (paint_mode_string == "AlphaBlend") {
		painter_render_mode = uitbc::Painter::kRmAlphablend;
	} else if (paint_mode_string == "AlphaTest") {
		painter_render_mode = uitbc::Painter::kRmAlphatest;
	} else if (paint_mode_string == "Normal") {
		painter_render_mode = uitbc::Painter::kRmNormal;
	} else if (paint_mode_string == "Xor") {
		painter_render_mode = uitbc::Painter::kRmXor;
	}
	painter_->SetRenderMode(painter_render_mode);

	const bool smooth_fonts = true;
	painter_->SetFontSmoothness(smooth_fonts);

	if (font_manager_) {
		str font;
		v_get(font, =, variable_scope_, kRtvarUi2DFont, "Times New Roman");
		double font_height;
		v_get(font_height, =, variable_scope_, kRtvarUi2DFontheight, 14.0);
		int font_flags;
		v_get(font_flags, =, variable_scope_, kRtvarUi2DFontflags, 0);
		current_font_id_ = font_manager_->QueryAddFont(font, font_height, font_flags);
	}

	// --------------
	// Sound settings
	if (sound_) {
		double sound_roll_off;
		v_get(sound_roll_off, =, variable_scope_, kRtvarUiSoundRolloff, 0.2);
		if (sound_roll_off != sound_roll_off_shadow_) {
			sound_roll_off_shadow_ = sound_roll_off;
			sound_->SetRollOffFactor((float)sound_roll_off);
		}
		double sound_doppler;
		v_get(sound_doppler, =, variable_scope_, kRtvarUiSoundDoppler, 1.3);
		if (sound_doppler != sound_doppler_shadow_) {
			sound_doppler_shadow_ = sound_doppler;
			sound_->SetDopplerFactor((float)sound_doppler);
		}
	}
}


void GameUiManager::OnResize(int width, int height) {
	if (canvas_) {
		canvas_->Reset(width, height, canvas_->GetBitDepth());
		if (desktop_window_) {
			desktop_window_->SetPreferredSize(canvas_->GetWidth(), canvas_->GetHeight());
			desktop_window_->SetSize(canvas_->GetWidth(), canvas_->GetHeight());
		}
	}
	if (input_) {
		input_->Refresh();
	}
}

void GameUiManager::OnMinimize() {
}

void GameUiManager::OnMaximize(int width, int height) {
	OnResize(width, height);
}



loginstance(kGame, GameUiManager);



}
