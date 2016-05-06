
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#ifndef CURE_TEST_WITHOUT_UI
#include "../../lepra/include/lepraassert.h"
#include "../../lepra/include/archivefile.h"
#include "../../lepra/include/cubicdecasteljauspline.h"
#include "../../lepra/include/hirestimer.h"
#include "../../lepra/include/ioerror.h"
#include "../../lepra/include/log.h"
#include "../../lepra/include/math.h"
#include "../../lepra/include/number.h"
#include "../../lepra/include/performance.h"
#include "../../lepra/include/random.h"
#include "../../lepra/include/string.h"
#include "../../lepra/include/systemmanager.h"
#include "../../lepra/include/pngloader.h"
#include "../../lepra/include/thread.h"
#include "../../lepra/include/timer.h"
#include "../../lepra/include/ziparchive.h"
#include "../../tbc/include/chunkyphysics.h"
#include "../../tbc/include/geometryreference.h"
#include "../../tbc/include/terrainfunction.h"
#include "../../tbc/include/terrainpatch.h"
#include "../../uilepra/include/uicore.h"
#include "../../uilepra/include/uidisplaymanager.h"
#include "../../uilepra/include/uiinput.h"
#include "../include/gui/uidesktopwindow.h"
#include "../include/gui/uiguiimagemanager.h"
#include "../include/gui/uibutton.h"
#include "../include/gui/uicaption.h"
#include "../include/gui/uiradiobutton.h"
#include "../include/gui/uicheckbutton.h"
#include "../include/gui/uilistcontrol.h"
#include "../include/gui/uifilenamefield.h"
#include "../include/gui/uiprogressbar.h"
#include "../include/gui/uiscrollbar.h"
#include "../include/gui/uiwindow.h"
#include "../include/gui/uifloatinglayout.h"
#include "../include/gui/uigridlayout.h"
#include "../include/gui/uitreenode.h"
#include "../include/uibasicmeshcreator.h"
#include "../include/uichunkyclass.h"
#include "../include/uichunkyloader.h"
#include "../include/uifontmanager.h"
#include "../include/uigeometrybatch.h"
#include "../include/uigraphicalmodel.h"
#include "../include/uiopenglpainter.h"
#include "../include/uiopenglrenderer.h"
#include "../include/uitexloader.h"
#include "../include/uitrianglebasedgeometry.h"
#include "../include/uiuvmapper.h"
#include "rotationalagreementtest.h"



using namespace lepra;
void ReportTestResult(const lepra::LogDecorator& log, const str& test_name, const str& context, bool result);
bool ResetAndClearFrame();
bool CloseRenderer();



//#define TEST_D3D_RENDERER		// Uncomment to drop software renderer test.
#define kObjectDistance		30

#define kTexturemap 0
#define kLightmap 1
#define kEnvmap 2
#define kSpecmap 3
#define kNormalmap 4
#define kMultimap 5


uilepra::DisplayManager* g_display = 0;
lepra::Canvas* g_screen = 0;
uitbc::Renderer* g_renderer = 0;
uitbc::Painter* g_painter = 0;
uitbc::FontManager* g_font_manager = 0;

uilepra::InputManager* g_input = 0;
int g_texture_map_count = 0;
int g_texture_index = 0;
uitbc::Renderer::TextureID g_texture_id[10];
uitbc::Painter::ImageID g_image_id[10];
int g_x = 0;
int g_y = 0;
double g_total_fps = 0;


class UiTbcTest {
};
lepra::LogDecorator gUiTbcLog(lepra::LogType::GetLogger(lepra::LogType::kTest), typeid(UiTbcTest));

class GUITestWindow: public uitbc::Window {
public:
	GUITestWindow() :
		Window(uitbc::Window::kBorderLinearshading | uitbc::Window::kBorderResizable,
			10, lepra::LIGHT_GREEN, new uitbc::GridLayout(5, 1)) {
		Init();
		SetPreferredSize(200, 300);

		uitbc::Caption* caption = new uitbc::Caption(lepra::BLUE, lepra::DARK_BLUE, lepra::BLUE, lepra::DARK_BLUE,
			lepra::LIGHT_GRAY, lepra::GRAY, lepra::LIGHT_GRAY, lepra::GRAY, 20);
		caption->SetText(L"My Test Window", lepra::WHITE, lepra::BLACK, lepra::OFF_BLACK, lepra::BLACK);
		SetCaption(caption);

		uitbc::Button* close_button = new uitbc::Button(uitbc::BorderComponent::kZigzag, 2, lepra::RED, L"x");
		close_button->SetText(L"x", lepra::WHITE, lepra::BLACK);
		close_button->SetPreferredSize(16, 16);
		close_button->SetMinSize(16, 16);

		close_button->SetOnClick(GUITestWindow, OnClose);

		caption->SetRightButton(close_button);

/*
		uitbc::ListControl* list_control = new uitbc::ListControl(uitbc::ListControl::kBorderLinearshading | uitbc::ListControl::kBorderSunken, 3, lepra::LIGHT_GRAY);
		list_control->SetStyle(uitbc::ListControl::kMultiSelect);
		uitbc::TreeNode::UseFont(lepra::BLACK, lepra::LIGHT_GRAY, lepra::BLACK, lepra::LIGHT_BLUE);
		uitbc::TreeNode* top_node = new uitbc::TreeNode("TopNode", "TopNode");

		int i;
		for (i = 0; i < 10; i++) {
			uitbc::TreeNode* lTreeNode;

			if (i == 5) {
				lTreeNode = new uitbc::TreeNode(lepra::strutil::Format("SubDirectory", i), "TreeNode");

				for (int j = 0; j < 5; j++) {
					uitbc::TreeNode* lChildNode = new uitbc::TreeNode(lepra::strutil::Format("List Item %i", i), "TreeNode");
					lTreeNode->AddChildNode(lChildNode);
				}
			} else {
				lTreeNode = new uitbc::TreeNode(lepra::strutil::Format("List Item %i", lepra::Random::GetRandomNumber() % 1024), "TreeNode");
			}

			top_node->AddChildNode(lTreeNode);
			//list_control->AddChild(lTreeNode);
		}

		list_control->AddChild(top_node);

		AddChild(list_control);
*/
		uitbc::FileNameField* text_field = new uitbc::FileNameField(GetClientRectComponent(), uitbc::Window::kBorderSunken | uitbc::Window::kBorderLinearshading,
			3, lepra::WHITE);
		text_field->SetPreferredSize(0, 24);
		text_field->SetFontColor(lepra::OFF_BLACK);
		text_field->SetText(L"Hullo!");
		AddChild(text_field);

		uitbc::Label* label = new uitbc::Label(lepra::OFF_BLACK, L"A Row List:");
		label->SetPreferredSize(0, 24, false);
		AddChild(label);

		uitbc::ListControl* list_control = new uitbc::ListControl(uitbc::ListControl::kBorderLinearshading | uitbc::ListControl::kBorderSunken, 3, lepra::LIGHT_GRAY, uitbc::ListLayout::kRow);
		list_control->SetPreferredHeight(30);
		for (int i = 0; i < 20; i++) {
			uitbc::Label* list_item = new uitbc::Label(lepra::OFF_BLACK, lepra::wstrutil::Format(L"Apa %i", i));
			list_item->SetPreferredWidth(12 * 6);
			list_control->AddChild(list_item);
		}
		AddChild(list_control);

		uitbc::Button* button = new uitbc::Button(uitbc::BorderComponent::kZigzag, 3, lepra::GRAY, L"NewWindow");
		button->SetText(L"New Window", lepra::OFF_BLACK, lepra::BLACK);
		button->SetPreferredSize(0, 40);
		button->SetMinSize(20, 20);
		button->SetOnClick(GUITestWindow, OnNewWindow);
		AddChild(button);
	}

	void OnClose(uitbc::Button*) {
		uitbc::DesktopWindow* desktop_window = (uitbc::DesktopWindow*)GetParentOfType(uitbc::Component::kDesktopwindow);
		desktop_window->PostDeleteComponent(this, 0);
	}

	void OnNewWindow(uitbc::Button*) {
		uitbc::DesktopWindow* desktop_window = (uitbc::DesktopWindow*)GetParentOfType(uitbc::Component::kDesktopwindow);
		desktop_window->AddChild(new GUITestWindow);
	}
};


class SceneTest {
public:
	SceneTest(const lepra::LogDecorator& log,
		const str& scene_name,
		float cam_rot_speed = 1.0f, float cam_move_speed = 8.0f):
		test_ok_(true),
		log_(log),
		scene_name_(scene_name),
		cam_rot_speed_(cam_rot_speed),
		cam_move_speed_(cam_move_speed) {
		ResetAndClearFrame();

		desktop_window_ = new uitbc::DesktopWindow(g_input, g_painter, new uitbc::FloatingLayout(), 0, 0);
		desktop_window_->SetPreferredSize(g_screen->GetWidth(), g_screen->GetHeight());
		desktop_window_->AddChild(new GUITestWindow);
		//desktop_window_->AddChild(new uitbc::ASEFileConverter(desktop_window_));
	}
	virtual ~SceneTest() {
		delete desktop_window_;
		g_renderer->RemoveAllGeometry();
		g_renderer->RemoveAllLights();
	}
	bool Run(double time);
	virtual void UpdateScene(double total_time, double delta_time) = 0;

protected:
	static void RenderGDITestImage(const wchar_t* text);
	static void PrintFps(double fps);

	bool test_ok_;
	const lepra::LogDecorator& log_;
	str context_;
	wstr extra_info_;

private:
	void operator=(const SceneTest&) {
		deb_assert(false);
	}
	str scene_name_;
	float cam_rot_speed_;
	float cam_move_speed_;
	uitbc::DesktopWindow* desktop_window_;
};

bool SceneTest::Run(double time) {
	g_input->ActivateAll();

	if (test_ok_) {
		lepra::Timer total_timer;
		lepra::HiResTimer delta_timer;
		lepra::Timer fps_timer;

		context_ = "clear";

		if (test_ok_ == true) {
			context_ = "running";
		}

		while (test_ok_ && total_timer.GetTimeDiff() < time && g_input->ReadKey(uilepra::InputManager::kInKbdEsc) == false) {
			double _delta_time = delta_timer.GetTimeDiff();
			delta_timer.ClearTimeDiff();

			uilepra::Core::ProcessMessages();
			if (lepra::SystemManager::GetQuitRequest()) {
				CloseRenderer();
				lepra::SystemManager::ExitProcess(0);
			}

			g_input->PollEvents();
			g_screen->SetBuffer(0);

			lepra::xform cam = g_renderer->GetCameraTransformation();
			if (g_input->ReadKey(uilepra::InputManager::kInKbdUp))
				cam.RotatePitch(cam_rot_speed_ * (float)_delta_time);
			if (g_input->ReadKey(uilepra::InputManager::kInKbdDown))
				cam.RotatePitch(-cam_rot_speed_ * (float)_delta_time);
			if (g_input->ReadKey(uilepra::InputManager::kInKbdLeft))
				cam.RotateYaw(cam_rot_speed_ * (float)_delta_time);
			if (g_input->ReadKey(uilepra::InputManager::kInKbdRight))
				cam.RotateYaw(-cam_rot_speed_ * (float)_delta_time);
			if (g_input->ReadKey(uilepra::InputManager::kInKbdQ))
				cam.RotateRoll(-cam_rot_speed_ * (float)_delta_time);
			if (g_input->ReadKey(uilepra::InputManager::kInKbdE))
				cam.RotateRoll(cam_rot_speed_ * (float)_delta_time);

			if (g_input->ReadKey(uilepra::InputManager::kInKbdW))
				cam.MoveForward(cam_move_speed_ * (float)_delta_time);
			if (g_input->ReadKey(uilepra::InputManager::kInKbdS))
				cam.MoveBackward(cam_move_speed_ * (float)_delta_time);
			if (g_input->ReadKey(uilepra::InputManager::kInKbdA))
				cam.MoveLeft(cam_move_speed_ * (float)_delta_time);
			if (g_input->ReadKey(uilepra::InputManager::kInKbdD))
				cam.MoveRight(cam_move_speed_ * (float)_delta_time);
			if (g_input->ReadKey(uilepra::InputManager::kInKbdR))
				cam.MoveUp(cam_move_speed_ * (float)_delta_time);
			if (g_input->ReadKey(uilepra::InputManager::kInKbdF))
				cam.MoveDown(cam_move_speed_ * (float)_delta_time);

			g_renderer->SetCameraTransformation(cam);

			{
				LEPRA_MEASURE_SCOPE(RenderScene);
				//g_renderer->ResetClippingRect();
				g_renderer->RenderScene();
				g_painter->ResetClippingRect();
				g_painter->PrePaint(true);

				g_painter->SetColor(lepra::WHITE);
				const lepra::vec3& pos = cam.GetPosition();
				g_painter->PrintText(lepra::wstrutil::Format(L"(%.2f, %.2f, %.2f)", pos.x, pos.y, pos.z)+extra_info_, 10, 10);

				desktop_window_->Repaint();
			}

			UpdateScene(total_timer.GetTimeDiff(), _delta_time);

			{
				LEPRA_MEASURE_SCOPE(UpdateScreen);
				g_display->UpdateScreen();
			}

			{
				LEPRA_MEASURE_SCOPE(ClearScreen);
				g_renderer->Clear(uitbc::Renderer::kClearColorbuffer |
						  uitbc::Renderer::kClearDepthbuffer |
						  uitbc::Renderer::kClearStencilbuffer);
			}

			//RenderGDITestImage();
			if (_delta_time != 0) {
				PrintFps(1/_delta_time);
			}

			total_timer.UpdateTimer();
			delta_timer.UpdateTimer();
			lepra::Thread::YieldCpu();
		}
	}

	g_input->ClearFunctors();

	ReportTestResult(log_, scene_name_, context_, test_ok_);
	return (test_ok_);
}

void SceneTest::RenderGDITestImage(const wchar_t* text = 0) {
	LEPRA_MEASURE_SCOPE(TextOutput);

	static int x = 0;
	static int dir = 1;

	if (!text) {
		text = L"This text is printed using GDI!";
	}
	g_painter->PrintText(text, x, 20);

	if (x >= 100 || x < 0) {
		dir = -dir;
	}

	x += dir;
}

void SceneTest::PrintFps(double fps) {
	static int frame_count = 0;
	static double _fps = 0;
	if (++frame_count > _fps/2) {
		frame_count = 0;
		_fps = fps;
	}
	wstr _text = lepra::wstrutil::Format(L"%f FPS", _fps);
	RenderGDITestImage(_text.c_str());
}



class BumpMapSceneTest: public SceneTest {
public:
	// All funtions are defined where TestScene() used to be.
	BumpMapSceneTest(const lepra::LogDecorator& log);
	virtual ~BumpMapSceneTest();

	void UpdateScene(double total_time, double delta_time);
private:

	void InitTerrain();

	tbc::TerrainPatch* t_patch_[17];
	uitbc::TriangleBasedGeometry* sphere_;
	uitbc::TriangleBasedGeometry* torus_;
	uitbc::TriangleBasedGeometry* cone_;

	tbc::BoneAnimation* animation_;
	tbc::BoneAnimator* animator_;
	uitbc::DefaultStaticGeometryHandler* torus_handler_;
	uitbc::GraphicalModel model_;

	lepra::vec3 light_pos_;
	lepra::vec3 light_dir_;
	lepra::vec3 light_color_;

	uitbc::Renderer::LightID light_id_;
};

class TerrainFunctionTest: public SceneTest {
public:
	TerrainFunctionTest(const lepra::LogDecorator& log);
	virtual ~TerrainFunctionTest();

	void UpdateScene(double total_time, double delta_time);

private:
	str triangle_count_info_;
	logclass();
};
loginstance(kTest, TerrainFunctionTest);



class GeometryReferenceTest: public SceneTest {
public:
	GeometryReferenceTest(const lepra::LogDecorator& log);
	virtual ~GeometryReferenceTest();

	void UpdateScene(double total_time, double delta_time);

private:
	uitbc::TriangleBasedGeometry* sphere_;
	tbc::GeometryReference* sphere_reference_;

	logclass();
};
loginstance(kTest, GeometryReferenceTest);



bool CloseRenderer() {
	delete (g_input);
	g_input = 0;
	delete (g_renderer);
	g_renderer = 0;
	delete (g_painter);
	g_painter = 0;
	delete (g_font_manager);
	g_font_manager = 0;
	delete (g_screen);
	g_screen = 0;
	delete (g_display);
	g_display = 0;
	return (true);
}

bool OpenRenderer(const lepra::LogDecorator& log, uilepra::DisplayManager::ContextType context) {
	bool ok = true;
	str _context;

	if (ok) {
		_context = "create display manager";
		lepra::SystemManager::AddQuitRequest(-1);
		g_display = uilepra::DisplayManager::CreateDisplayManager(context);
		ok = (g_display != 0);
	}
	uilepra::DisplayMode display_mode;
	if (ok) {
		_context = "find display mode";
		ok = g_display->FindDisplayMode(display_mode, 640, 480, 32) || g_display->FindDisplayMode(display_mode, 1920, 1080);
	}
	if (ok) {
		_context = "open screen";
		//ok = g_display->OpenScreen(display_mode, uilepra::DisplayManager::kFullscreen, uilepra::DisplayManager::kOrientationAllowAny);
		ok = g_display->OpenScreen(display_mode, uilepra::DisplayManager::kWindowed, uilepra::DisplayManager::kOrientationAllowAny);
	}
	if (ok) {
		_context = "open input";
		g_input = uilepra::InputManager::CreateInputManager(g_display);
	}

	g_screen = 0;
	g_renderer = 0;
	g_painter = 0;
	g_font_manager = 0;
	if (ok) {
		_context = "create renderer";
		g_screen = new lepra::Canvas(g_display->GetWidth(), g_display->GetHeight(), lepra::Canvas::IntToBitDepth(g_display->GetBitDepth()));
		g_screen->SetBuffer(0);
		if (context == uilepra::DisplayManager::kOpenglContext) {
			g_renderer = new uitbc::OpenGLRenderer(g_screen);
			g_painter = new uitbc::OpenGLPainter;
		} else {
			deb_assert(false);
		}
		g_renderer->SetMipMappingEnabled(true);

		g_font_manager = uitbc::FontManager::Create(g_display);
		uitbc::FontManager::FontId font_id = g_font_manager->AddFont("Arial", 16, 0);
		g_font_manager->SetActiveFont(font_id);
		g_painter->SetFontManager(g_font_manager);

		g_painter->SetDestCanvas(g_screen);
		g_painter->SetRenderMode(uitbc::Painter::kRmAlphablend);
	}

#ifdef LEPRA_WINDOWS
	if (ok) {
		_context = "disable vsync";
		ok = g_display->SetVSyncEnabled(false);
	}
#endif // Windows

	uitbc::Texture texture_map;
	if (ok) {
		_context = "load texturemap (TEX)";
		uitbc::TEXLoader tex_loader;
		ok = (tex_loader.Load("texturemap.tex", texture_map, true) == uitbc::TEXLoader::kStatusSuccess);
		if (!ok) {
			_context = "load texturemap";
			lepra::Canvas canvas;
			lepra::PngLoader loader;
			ok = (loader.Load("data/nuclear.png", canvas) == lepra::PngLoader::kStatusSuccess);

			texture_map.Set(canvas);
			if (ok) {
				uitbc::TEXLoader tex_loader;
				tex_loader.Save("texturemap.tex", texture_map, false);
			}
			deb_assert(ok);
		}
	}

	uitbc::Texture light_map;
	if (ok) {
		_context = "load lightmap (TEX)";
		uitbc::TEXLoader tex_loader;
		ok = (tex_loader.Load("lightmap.tex", light_map, false) == uitbc::TEXLoader::kStatusSuccess);
		if (!ok) {
			_context = "load lightmap";
			lepra::Canvas canvas;
			lepra::PngLoader loader;
			ok = (loader.Load("data/env.png", canvas) == lepra::PngLoader::kStatusSuccess);

			light_map.Set(canvas);

			if (ok) {
				uitbc::TEXLoader tex_loader;
				tex_loader.Save("lightmap.tex", light_map, false);
			}
			deb_assert(ok);
		}
	}

	uitbc::Texture env_map;
	if (ok) {
		_context = "load envmap (TEX)";
		uitbc::TEXLoader tex_loader;
		ok = (tex_loader.Load("envmap.tex", env_map, false) == uitbc::TEXLoader::kStatusSuccess);
		if (!ok) {
			_context = "load envmap";
			lepra::Canvas canvas;
			lepra::PngLoader loader;
			ok = (loader.Load("data/env.png", canvas) == lepra::PngLoader::kStatusSuccess);

			env_map.Set(canvas);

			if (ok) {
				uitbc::TEXLoader tex_loader;
				tex_loader.Save("envmap.tex", env_map, false);
			}
			deb_assert(ok);
		}
	}

	uitbc::Texture spec_map;
	if (ok) {
		_context = "load specmap (TEX)";
		uitbc::TEXLoader tex_loader;
		ok = (tex_loader.Load("specmap.tex", spec_map, false) == uitbc::TEXLoader::kStatusSuccess);
		if (!ok) {
			_context = "load specmap";
			lepra::Canvas canvas;
			lepra::PngLoader loader;
			ok = (loader.Load("data/env.png", canvas) == lepra::PngLoader::kStatusSuccess);

			spec_map.Set(canvas);

			if (ok) {
				uitbc::TEXLoader tex_loader;
				tex_loader.Save("specmap.tex", spec_map, false);
			}
			deb_assert(ok);
		}
	}

	uitbc::Texture normal_map;
	if (ok) {
		_context = "load normalmap (TEX)";
		uitbc::TEXLoader tex_loader;
		ok = (tex_loader.Load("normalmap.tex", normal_map, false) == uitbc::TEXLoader::kStatusSuccess);
		if (!ok) {
			_context = "load normalmap";
			lepra::Canvas canvas;
			lepra::PngLoader loader;
			ok = (loader.Load("data/env.png", canvas) == lepra::PngLoader::kStatusSuccess);

			normal_map.Set(canvas);

			if (ok) {
				uitbc::TEXLoader tex_loader;
				tex_loader.Save("normalmap.tex", normal_map, false);
			}
			deb_assert(ok);
		}
	}

	uitbc::Texture multi_map;
	if (ok) {
		_context = "load multimap (TEX)";
		uitbc::TEXLoader tex_loader;
		ok = tex_loader.Load("multiMap.tex", multi_map, false) == uitbc::TEXLoader::kStatusSuccess;
		if (!ok) {
			multi_map.Set(*texture_map.GetColorMap(0), Canvas::kResizeFast, 0, 0, normal_map.GetColorMap(0), spec_map.GetColorMap(0));
			uitbc::TEXLoader tex_loader;
			tex_loader.Save("multiMap.tex", multi_map, false);
			ok = true;
		}
		deb_assert(ok);
	}

	if (ok) {
		_context = "add texturemap";
		if (context == uilepra::DisplayManager::kOpenglContext)
			texture_map.SwapRGBOrder();

		g_texture_id[kTexturemap] = g_renderer->AddTexture(&texture_map);
		ok = (g_texture_id[kTexturemap] != 0);
		deb_assert(ok);
	}
	if (ok) {
		_context = "add texturemap to painter";
		g_image_id[kTexturemap] = g_painter->AddImage(texture_map.GetColorMap(0), texture_map.GetAlphaMap(0));
		ok = (g_image_id[kTexturemap] != 0);
		deb_assert(ok);
	}
	if (ok) {
		_context = "add lightmap";
		if (context == uilepra::DisplayManager::kOpenglContext)
			light_map.SwapRGBOrder();

		g_texture_id[kLightmap] = g_renderer->AddTexture(&light_map);
		ok = (g_texture_id[kLightmap] != 0);
		deb_assert(ok);
	}
	if (ok) {
		_context = "add envmap";
		if (context == uilepra::DisplayManager::kOpenglContext)
			env_map.SwapRGBOrder();

		g_texture_id[kEnvmap] = g_renderer->AddTexture(&env_map);
		ok = (g_texture_id[kEnvmap] != 0);
		if (ok) {
			g_renderer->SetEnvironmentMap(g_texture_id[kEnvmap]);
		}
		deb_assert(ok);
	}
	if (ok) {
		_context = "add specmap";
		if (context == uilepra::DisplayManager::kOpenglContext)
			spec_map.SwapRGBOrder();

		g_texture_id[kSpecmap] = g_renderer->AddTexture(&spec_map);
		ok = (g_texture_id[kSpecmap] != 0);
		deb_assert(ok);
	}
	if (ok) {
		_context = "add normalmap";
		if (context == uilepra::DisplayManager::kOpenglContext)
			normal_map.SwapRGBOrder();

		g_texture_id[kNormalmap] = g_renderer->AddTexture(&normal_map);
		ok = (g_texture_id[kNormalmap] != 0);
		deb_assert(ok);
	}
	if (ok) {
		_context = "add multimap";
		if (context == uilepra::DisplayManager::kOpenglContext)
			multi_map.SwapRGBOrder();

		g_texture_id[kMultimap] = g_renderer->AddTexture(&multi_map);
		ok = (g_texture_id[kMultimap] != 0);
		deb_assert(ok);
	}

	if (ok) {
		_context = "initialize renderer";
		g_renderer->SetLightsEnabled(false);
		g_renderer->SetAmbientLight(0.5, 0.5, 0.5);
		g_renderer->SetTrilinearFilteringEnabled(true);
	}
	if (!ok) {
		CloseRenderer();
	}


	str renderer_type = "OpenGL";
	if (context != uilepra::DisplayManager::kOpenglContext) {
		renderer_type = "D3D";
	}
	ReportTestResult(log, renderer_type+"Renderer", _context, ok);

	return (ok);
}

bool ClearBackground() {
	LEPRA_MEASURE_SCOPE(ClearScreen);
	g_renderer->SetViewport(lepra::PixelRect(0, 0, g_screen->GetWidth(), g_screen->GetHeight()));
	g_renderer->SetViewFrustum(90, 0.1f, 10000);
	g_renderer->SetClippingRect(lepra::PixelRect(0, 0, g_screen->GetWidth(), g_screen->GetHeight()));
	g_renderer->SetClearColor(lepra::Color(0, 0, 0, 255));
	g_renderer->Clear();
	return (true);
}

bool ClearSubframe(int x_box = 0, int y_box = 0, int splits_x = 1, int splits_y = 1, int frame_fatso = 4, bool clear_color_only = true) {
	LEPRA_MEASURE_SCOPE(ClearFrame);

	const int frame_x_size = g_screen->GetWidth()/splits_x - splits_x + 1;
	const int frame_y_size = g_screen->GetHeight()/splits_y - splits_y + 1;
	const int xo = frame_x_size*x_box;
	const int yo = frame_y_size*y_box;
	const int frame_thickness = frame_x_size/frame_fatso;
	g_x = xo+frame_thickness;
	g_y = yo+frame_thickness;
	int right  = xo + frame_x_size-frame_thickness-1;
	int bottom = yo + frame_y_size-frame_thickness-1;

	g_renderer->SetViewport(lepra::PixelRect(g_x, g_y, right, bottom));
	g_renderer->SetViewFrustum(90, 1, 1000);
	g_renderer->SetClippingRect(lepra::PixelRect(g_x, g_y, right, bottom));


	g_renderer->SetClearColor(lepra::Color(
		(lepra::uint8)(128-128*x_box/splits_x),
		(lepra::uint8)(128*y_box/splits_y),
		(lepra::uint8)(128*x_box/splits_x),
		255));

	// Set camera transformation according to to placement.
/*	lepra::RotationMatrixF rotation;
	rotation.MakeIdentity();
//	rotation.RotateAroundOwnX(1.57);
	lepra::vec3 transformation(0, 0, 0);
	transformation.y -= (1.0+x_box*2.0-splits_x)/splits_x*kObjectDistance;
	transformation.z -= (1.0+y_box*2.0-splits_y)/splits_y*0.8*kObjectDistance;
	lepra::xform lCameraTrans(rotation, transformation);
	g_renderer->SetCameraTransformation(lCameraTrans);
*/
	unsigned clear_flags = uitbc::Renderer::kClearColorbuffer;
	if (!clear_color_only) {
		clear_flags |= uitbc::Renderer::kClearDepthbuffer;
	}
	g_renderer->Clear(clear_flags);
	return (true);
}

bool ResetAndClearFrame() {
	bool cleared = ClearBackground();
	return (cleared);
}

void PrintInfo(const wstr& info) {
	g_painter->ResetClippingRect();
	g_painter->SetColor(lepra::Color(255, 255, 255, 255), 0);
	g_painter->SetColor(lepra::Color(0, 0, 0, 0), 1);
	g_painter->PrintText(info, g_x+2, g_y+2);
}

bool InitializeGeometry(tbc::GeometryBase* geometry) {
	lepra::RotationMatrixF rotation;
	rotation.MakeIdentity();
	lepra::xform object_trans(rotation, lepra::vec3(0, kObjectDistance, 0));
	geometry->SetTransformation(object_trans);
	tbc::GeometryBase::BasicMaterialSettings _material(
		lepra::vec3(0, 0, 0),
		lepra::vec3(0.8f, 0.9f, 0.7f),
		lepra::vec3(0.1f, 0.1f, 0.1f),
		0.8f, 1.0f, true);
	geometry->SetBasicMaterialSettings(_material);
	return (true);
}

bool QuickRender(tbc::GeometryBase* geometry, const uitbc::Renderer::MaterialType material, bool update_screen,
		 const lepra::xform* transform = 0, double duration = 0.5, const char* /*text*/ = 0,
		 bool clear_screen = true, float rotation_speed = 0.1f, const lepra::vec3 rotation_axis = lepra::vec3(1, 1, 1)) {
	bool ok = true;

	uitbc::Renderer::GeometryID graphic_id = uitbc::Renderer::INVALID_GEOMETRY;
	if (ok) {
		graphic_id = g_renderer->AddGeometry(geometry, material, uitbc::Renderer::kNoShadows);
		ok = (graphic_id != uitbc::Renderer::INVALID_GEOMETRY);
		deb_assert(ok);
		for (int x = 0; ok && x < g_texture_map_count; ++x) {
			ok = g_renderer->TryAddGeometryTexture(graphic_id, g_texture_id[x]);
			deb_assert(ok);
		}
	}
	if (ok) {
		lepra::Timer total_timer;
		lepra::HiResTimer frame_timer;

		int i = 0;

		//
		// Test Quaternion and RotationMatrix. They should give equal results.
		//

		lepra::quat q_rot;
		lepra::RotationMatrixF m_rot;
		q_rot.RotateAroundWorldX(1.7f);
		m_rot.RotateAroundWorldX(1.7f);

		q_rot.RotateAroundWorldY(1.23f);
		m_rot.RotateAroundWorldY(1.23f);

		q_rot.RotateAroundWorldZ(1.14f);
		m_rot.RotateAroundWorldZ(1.14f);

		q_rot.RotateAroundOwnX(1.7f);
		m_rot.RotateAroundOwnX(1.7f);

		q_rot.RotateAroundOwnY(1.23f);
		m_rot.RotateAroundOwnY(1.23f);

		q_rot.RotateAroundOwnZ(1.14f);
		m_rot.RotateAroundOwnZ(1.14f);

		q_rot.RotateAroundVector(lepra::vec3(1, 0, 0), 0.2f);
		m_rot.RotateAroundVector(lepra::vec3(1, 0, 0), 0.2f);

		do {
			if (clear_screen) {
				LEPRA_MEASURE_SCOPE(ClearScreen);
				g_renderer->Clear();
			}

			// Change between quaternions and matrices every second frame.
			// If everything works the user will not see the difference.
			lepra::xform object_movement;
			if (i & 2) {
				lepra::quat rotation;
				rotation.RotateAroundVector(rotation_axis, (float)frame_timer.GetTimeDiff()*rotation_speed*lepra::PIF*2/1.5f);

				// Test conversion to and from RotationMatrix.
				lepra::RotationMatrixF mtx(rotation.GetAsRotationMatrix());
				rotation.Set(mtx);

				object_movement.SetOrientation(rotation * q_rot);
				object_movement.SetPosition((rotation * q_rot) * lepra::vec3(0, 5, 0) + lepra::vec3(0, kObjectDistance, 0));
			}
			//TODO:
			//	This is a bug. When switching rotation algo every other frame (uncomment this and the if-statement above),
			//	the whole mesh starts flickering in the terrain test case.*/
			else {
				lepra::RotationMatrixF rotation;
				rotation.RotateAroundVector(rotation_axis, (float)frame_timer.GetTimeDiff()*rotation_speed*lepra::PIF*2/1.5f);
				object_movement.SetOrientation(rotation * m_rot);
				object_movement.SetPosition((rotation * m_rot) * lepra::vec3(0, 5, 0) + lepra::vec3(0, kObjectDistance, 0));
			}
			if (transform) {
				object_movement = object_movement*(*transform);
			}
			geometry->SetTransformation(object_movement);
			geometry->SetLastFrameVisible(g_renderer->GetCurrentFrame());
			{
				LEPRA_MEASURE_SCOPE(RenderScene);
				g_renderer->RenderScene();
			}
			i++;

			if (update_screen) {
				//RenderGDITestImage(text);

				LEPRA_MEASURE_SCOPE(UpdateScreen);
				g_display->UpdateScreen();
				uilepra::Core::ProcessMessages();
				if (lepra::SystemManager::GetQuitRequest()) {
					CloseRenderer();
					lepra::SystemManager::ExitProcess(0);
				}
			}

			total_timer.UpdateTimer();
			frame_timer.UpdateTimer();
		} while (update_screen && total_timer.GetTimeDiff() < duration);
	}
	if (graphic_id != uitbc::Renderer::INVALID_GEOMETRY) {
		g_renderer->RemoveGeometry(graphic_id);
	}
	return ok;
}

bool AddRandomVertexColor(uitbc::TriangleBasedGeometry* geometry) {
	bool ok = true;
	if (ok) {
		lepra::uint8* vertex_color = new lepra::uint8[geometry->GetVertexCount()*4];
		for (unsigned y = 0; y < geometry->GetVertexCount()*4; y += 4) {
			vertex_color[y+0] = (lepra::uint8)lepra::Random::GetRandomNumber();
			vertex_color[y+1] = (lepra::uint8)lepra::Random::GetRandomNumber();
			vertex_color[y+2] = (lepra::uint8)lepra::Random::GetRandomNumber();
			vertex_color[y+3] = (lepra::uint8)lepra::Random::GetRandomNumber();
		}
		geometry->SetColorData(vertex_color, tbc::GeometryBase::kColorRgba);
		delete[] (vertex_color);
	}
	return (ok);
}

bool AddMappingCoords(uitbc::TriangleBasedGeometry* geometry) {
	bool ok = true;
	geometry->SplitVertices();
	geometry->AddEmptyUVSet();
	lepra::Vector2DD uv_offset(0.5, 0.5);
	uitbc::UVMapper::ApplyCubeMapping(geometry, 0, geometry->GetBoundingRadius(), uv_offset);
	geometry->DupUVSet(0);

	/*if (ok) {
		unsigned lCorner = 0;
		unsigned vertex_count = geometry->GetVertexCount();
		float* uv_coords = new float[vertex_count*2];
		for (unsigned z = 0; z < vertex_count; ++z) {
			switch (lCorner) {
				case 0: {
					uv_coords[z*2+0] = 0;
					uv_coords[z*2+1] = 0;
				} break;
				case 1: {
					uv_coords[z*2+0] = 1;
					uv_coords[z*2+1] = 0;
				} break;
				case 2: {
					uv_coords[z*2+0] = 1;
					uv_coords[z*2+1] = 1;
				} break;
				case 3: {
					uv_coords[z*2+0] = 0;
					uv_coords[z*2+1] = 1;
				} break;
			}
			lCorner = (lCorner+1)&3;
		}
		geometry->AddUVSet(uv_coords);	// Add for texturemap.
		geometry->AddUVSet(uv_coords);	// Add for lightmap.
		delete[] (uv_coords);
	}*/

	return (ok);
}

bool AddUVAnimation(tbc::GeometryBase* geometry) {
	// TODO: Fix the memory leaks.

	tbc::BoneHierarchy* bones = new tbc::BoneHierarchy;
	bones->SetBoneCount(1);
	bones->FinalizeInit(tbc::BoneHierarchy::kTransformNone);

	tbc::BoneAnimation* animation = new tbc::BoneAnimation;
	animation->SetKeyframeCount(4, true);
	animation->SetBoneCount(1);
	animation->SetTimeTag(0, 0);
	animation->SetTimeTag(1, 0.2f);
	animation->SetTimeTag(2, 0.4f);
	animation->SetTimeTag(3, 0.6f);
	animation->SetTimeTag(4, 0.8f);
	animation->SetBoneTransformation(0, 0,
		lepra::xform(lepra::g3x3IdentityMatrixF,
				       lepra::vec3(0.5f, 0.5f, 0)));
	animation->SetBoneTransformation(1, 0,
		lepra::xform(lepra::g3x3IdentityMatrixF,
				       lepra::vec3(0.5f, -0.5f, 0)));
	animation->SetBoneTransformation(2, 0,
		lepra::xform(lepra::g3x3IdentityMatrixF,
				       lepra::vec3(-0.5f, -0.5f, 0)));
	animation->SetBoneTransformation(3, 0,
		lepra::xform(lepra::g3x3IdentityMatrixF,
				       lepra::vec3(-0.5, 0.5, 0)));

	tbc::BoneAnimator* animator = new tbc::BoneAnimator(bones);
	str anim_name("UVAnimation");
	animator->AddAnimation(anim_name, animation);
	animator->StartAnimation(anim_name, 0, tbc::BoneAnimation::kModePlayLoop);

	geometry->SetUVAnimator(animator);

	return true;
}

bool TestSkinningSaveLoad(const lepra::LogDecorator& log, double show_time) {
	// Performs the following steps:
	//  1. Create+save mesh 1.
	//  2. Load mesh 1.
	//  3. Create+save mesh 2.
	//  4. Load mesh 2.
	//  5. Create+save skin 1.
	//  6. Load skin 1.
	//  7. Create+save skin 2.
	//  8. Load skin 2.
	//  9. Create+save animation.
	// 10. Load animation.
	// 11. Create+save structure.
	// 12. Load structure.
	// 13. Set two different materials on the two meshes.
	// 14. Render+animate the bloody thing!

	str _context;
	bool test_ok = true;
	const str file_name("chain");

	const float cuboid_length = 40.0f;
	uitbc::TriangleBasedGeometry _geometry[2];
	for (int mesh_index = 0; mesh_index < 2; ++mesh_index) {
		const str this_mesh_name = file_name+lepra::strutil::Format("%i.mesh", mesh_index);
		if (test_ok) {
			_context = "save chain mesh";
			//uitbc::TriangleBasedGeometry* _geometry = uitbc::BasicMeshCreator::CreateFlatBox(cuboid_length/4.0f, cuboid_length/2.0f, cuboid_length/4.0f, 1, 2, 1);
			uitbc::TriangleBasedGeometry* _geometry = uitbc::BasicMeshCreator::CreateTorus(cuboid_length/4.0f, cuboid_length/10.0f, cuboid_length/10.0f, 32, 24);
			//uitbc::TriangleBasedGeometry* _geometry = uitbc::BasicMeshCreator::CreateCylinder(cuboid_length/10.0f, cuboid_length/10.0f, cuboid_length/2.0f, 32);
			lepra::RotationMatrixF rotation;
			rotation.RotateAroundVector(lepra::vec3(1, 0, 0), -lepra::PIF/2.0f);
			_geometry->Rotate(rotation);
			float displacement = (mesh_index*2-1)*cuboid_length/4.0f;
			_geometry->Translate(lepra::vec3(0, displacement, 0));
			lepra::DiskFile file;
			test_ok = file.Open(this_mesh_name, lepra::DiskFile::kModeWrite);
			deb_assert(test_ok);
			if (test_ok) {
				test_ok = InitializeGeometry(_geometry);
				deb_assert(test_ok);
			}
			if (test_ok) {
				AddRandomVertexColor(_geometry);
			}
			if (test_ok) {
				uitbc::ChunkyMeshLoader mesh_loader(&file, false);
				test_ok = mesh_loader.Save(_geometry, true);
				deb_assert(test_ok);
			}
			delete (_geometry);
		}
		if (test_ok) {
			_context = "load chain mesh";
			lepra::DiskFile file;
			test_ok = file.Open(this_mesh_name, lepra::DiskFile::kModeRead);
			deb_assert(test_ok);
			if (test_ok) {
				uitbc::ChunkyMeshLoader mesh_loader(&file, false);
				int casts_shadows;
				test_ok = mesh_loader.Load(&_geometry[mesh_index], casts_shadows);
				deb_assert(test_ok && casts_shadows == 1);
			}
		}
		lepra::DiskFile::Delete(this_mesh_name);
	}

	uitbc::AnimatedGeometry skin[2];
	for (int skin_index = 0; skin_index < 2; ++skin_index) {
		const str this_skin_name = file_name+lepra::strutil::Format("%i.skin", skin_index);
		if (test_ok) {
			_context = "save chain skin";
			uitbc::BasicMeshCreator::CreateYBonedSkin(-cuboid_length/2.0f, +cuboid_length/2.0f, &_geometry[skin_index], &skin[skin_index], 2);
			lepra::DiskFile file;
			test_ok = file.Open(this_skin_name, lepra::DiskFile::kModeWrite);
			deb_assert(test_ok);
			if (test_ok) {
				uitbc::ChunkySkinLoader skin_loader(&file, false);
				test_ok = skin_loader.Save(&skin[skin_index]);
				deb_assert(test_ok);
			}
		}
		if (test_ok) {
			_context = "load chain skin";
			lepra::DiskFile file;
			test_ok = file.Open(this_skin_name, lepra::DiskFile::kModeRead);
			deb_assert(test_ok);
			if (test_ok) {
				uitbc::ChunkySkinLoader skin_loader(&file, false);
				test_ok = skin_loader.Load(&skin[skin_index]);
				deb_assert(test_ok);
			}
		}
		lepra::DiskFile::Delete(this_skin_name);
	}

	const str animation_name(file_name+".animation");
	if (test_ok) {
		_context = "save chain animation";
		tbc::BoneAnimation animation;
		animation.SetDefaultMode(tbc::BoneAnimation::kModePlayLoop);
		animation.SetRootNodeName("any_friggen_node");
		animation.SetKeyframeCount(2, true);
		animation.SetBoneCount(2);
		animation.SetTimeTag(0, 0);
		animation.SetTimeTag(1, 0.5f);
		animation.SetTimeTag(2, 1.0f);
		lepra::xform transform0 = lepra::kIdentityTransformationF;
		animation.SetBoneTransformation(0, 0, transform0);
		lepra::xform transform1 = lepra::kIdentityTransformationF;
		transform1.RotatePitch(-0.8f);
		animation.SetBoneTransformation(0, 1, transform1);
		animation.SetBoneTransformation(1, 0, transform0);
		transform1 = lepra::kIdentityTransformationF;
		transform1.RotatePitch(+0.8f);
		animation.SetBoneTransformation(1, 1, transform1);
		lepra::DiskFile file;
		test_ok = file.Open(animation_name, lepra::DiskFile::kModeWrite);
		deb_assert(test_ok);
		if (test_ok) {
			tbc::ChunkyAnimationLoader animation_loader(&file, false);
			test_ok = animation_loader.Save(&animation);
			deb_assert(test_ok);
		}
	}
	tbc::BoneAnimation animation;
	if (test_ok) {
		_context = "load chain animation";
		lepra::DiskFile file;
		test_ok = file.Open(animation_name, lepra::DiskFile::kModeRead);
		deb_assert(test_ok);
		if (test_ok) {
			tbc::ChunkyAnimationLoader animation_loader(&file, false);
			test_ok = animation_loader.Load(&animation);
			deb_assert(test_ok);
		}
	}
	lepra::DiskFile::Delete(animation_name);

	const str structure_name(file_name+".structure");
	if (test_ok) {
		_context = "save chain structure";
		tbc::ChunkyPhysics structure(tbc::BoneHierarchy::kTransformNone, tbc::ChunkyPhysics::kDynamic);
		structure.SetBoneCount(2);
		structure.SetBoneChildCount(0, 1);
		structure.SetChildIndex(0, 0, 1);
		lepra::xform _transform = lepra::kIdentityTransformationF;
		structure.SetOriginalBoneTransformation(0, _transform);
		structure.SetOriginalBoneTransformation(1, _transform);
		structure.SetPhysicsType(tbc::ChunkyPhysics::kDynamic);
		structure.BoneHierarchy::FinalizeInit(tbc::BoneHierarchy::kTransformNone);
		test_ok = (structure.GetBoneChildCount(structure.GetRootBone()) == 1);
		deb_assert(test_ok);
		lepra::DiskFile file;
		test_ok = file.Open(structure_name, lepra::DiskFile::kModeWrite);
		deb_assert(test_ok);
		if (test_ok) {
			tbc::ChunkyPhysicsLoader structure_loader(&file, false);
			test_ok = structure_loader.Save(&structure);
			deb_assert(test_ok);
		}
	}
	tbc::ChunkyPhysics structure(tbc::BoneHierarchy::kTransformNone, tbc::ChunkyPhysics::kDynamic);
	if (test_ok) {
		_context = "load chain structure";
		lepra::DiskFile file;
		test_ok = file.Open(structure_name, lepra::DiskFile::kModeRead);
		deb_assert(test_ok);
		if (test_ok) {
			tbc::ChunkyPhysicsLoader structure_loader(&file, false);
			test_ok = structure_loader.Load(&structure);
			deb_assert(test_ok);
		}
		if (test_ok) {
			test_ok = structure.BoneHierarchy::FinalizeInit(tbc::BoneHierarchy::kTransformNone);
			deb_assert(test_ok);
		}
		if (test_ok) {
			test_ok = (structure.GetBoneChildCount(structure.GetRootBone()) == 1);
			deb_assert(test_ok);
		}
	}
	lepra::DiskFile::Delete(structure_name);

	if (test_ok) {
		_context = "clear screen";
		test_ok = ResetAndClearFrame();
		deb_assert(test_ok);
	}
	tbc::BoneAnimator animator(&structure);
	if (test_ok) {
		skin[0].SetBoneHierarchy(&structure);
		skin[1].SetBoneHierarchy(&structure);
		skin[0].SetGeometry(&_geometry[0]);
		skin[1].SetGeometry(&_geometry[1]);
		animator.AddAnimation(animation_name, &animation);
		animator.StartAnimation(animation_name, 0, tbc::BoneAnimation::kModePlayDefault);
	}
	lepra::Timer total_timer;
	lepra::HiResTimer frame_timer;
	lepra::xform object_transform;
	while (test_ok && total_timer.GetTimeDiff() < show_time) {
		_context = "render";
		if (test_ok) {
			test_ok = QuickRender(&skin[0], uitbc::Renderer::kMatSingleColorSolid, false, &object_transform, -1.0, 0, true);
			deb_assert(test_ok);
		}
		if (test_ok) {
			test_ok = QuickRender(&skin[1], uitbc::Renderer::kMatVertexColorSolid, true, &object_transform, -1.0, "Loaded from disk!", false);
			deb_assert(test_ok);
		}
		if (test_ok) {
			lepra::quat rotation;
			rotation.RotateAroundVector(lepra::vec3(0.0f, 0.0f, 1), (float)total_timer.GetTimeDiff()*lepra::PIF*2/6.7f);
			object_transform.SetOrientation(rotation);

			skin[0].UpdateAnimatedGeometry();
			skin[1].UpdateAnimatedGeometry();
			animator.Step((float)frame_timer.PopTimeDiff());
			total_timer.UpdateTimer();
		}
	}

	ReportTestResult(log, "SkinningLoadSave", _context, test_ok);
	return (test_ok);
}

bool TestMeshImport(const lepra::LogDecorator& log, double show_time) {
	str _context;
	bool test_ok = true;

	uitbc::TriangleBasedGeometry _geometry;
	if (test_ok) {
		_context = "load imported mesh";
		const str mesh_name = "data/tractor_01_rear_wheel0.mesh";
		lepra::DiskFile file;
		test_ok = file.Open(mesh_name, lepra::DiskFile::kModeRead);
		deb_assert(test_ok);
		if (test_ok) {
			uitbc::ChunkyMeshLoader mesh_loader(&file, false);
			int casts_shadows;
			test_ok = mesh_loader.Load(&_geometry, casts_shadows);
			deb_assert(test_ok && casts_shadows == 1);
		}
		if (test_ok) {
			test_ok = (_geometry.GetVertexCount() == 442 && _geometry.GetIndexCount() == 2640);
			deb_assert(test_ok);
		}
		if (test_ok) {
			float* vertices = _geometry.GetVertexData();
			unsigned vc = _geometry.GetVertexCount()*3;
			for (unsigned x = 0; x < vc; ++x) {
				vertices[x] *= 7;
			}
			AddRandomVertexColor(&_geometry);
		}
	}

	if (test_ok) {
		_context = "clear screen";
		test_ok = ResetAndClearFrame();
	}
	lepra::Timer total_timer;
	lepra::xform object_transform;
	while (test_ok && total_timer.GetTimeDiff() < show_time) {
		_context = "render";
		if (test_ok) {
			test_ok = QuickRender(&_geometry, uitbc::Renderer::kMatVertexColorSolid, true, &object_transform, -1.0, 0, true);
		}
		if (test_ok) {
			lepra::quat rotation;
			rotation.RotateAroundVector(lepra::vec3(0.1f, -0.1f, 1), (float)total_timer.GetTimeDiff()*lepra::PIF*2/6.7f);
			object_transform.SetOrientation(rotation);
			total_timer.UpdateTimer();
		}
	}

	ReportTestResult(log, "MeshImport", _context, test_ok);
	return (test_ok);
}

bool TestLoadClass(const lepra::LogDecorator& log) {
	str _context;
	bool test_ok = true;

	lepra::DiskFile file;
	if (test_ok) {
		_context = "open file";
		const str class_name = "data/tractor_01.class";
		test_ok = file.Open(class_name, lepra::DiskFile::kModeRead);
		deb_assert(test_ok);
	}
	uitbc::ChunkyClass clazz;
	if (test_ok) {
		_context = "load class";
		uitbc::ChunkyClassLoader loader(&file, false);
		test_ok = loader.Load(&clazz);
		deb_assert(test_ok);
	}

	ReportTestResult(log, "LoadClass", _context, test_ok);
	return (test_ok);
}

TerrainFunctionTest::TerrainFunctionTest(const lepra::LogDecorator& log) :
	SceneTest(log, "TerrainFunctionTest") {
	const float patch_left = 5;
	const float patch_size = 20;
	const lepra::vec2 volcano_position(patch_size-patch_left, 2*patch_left);
	const lepra::vec2 dune_position(-patch_left, 2*patch_left);

	tbc::TerrainPatch::SetDimensions(6, patch_size);

	int triangle_count = 0;
	const int patch_count_side = 10;
	const int patch_count = patch_count_side*patch_count_side;
	tbc::TerrainPatch* patch[patch_count];
	for (int y = 0; y < patch_count_side; ++y) {
		for (int x = 0; x < patch_count_side; ++x) {
			const int half_side_count = patch_count_side/2;
			patch[y*patch_count_side+x] = new tbc::TerrainPatch(lepra::Vector2D<int>(x-half_side_count, y-half_side_count), 0, 0, 0, 0, 8, 8, 0);
			triangle_count += patch[y*patch_count_side+x]->GetTriangleCount();
		}
	}
	triangle_count_info_ = " - " + lepra::Number::ConvertToPostfixNumber(triangle_count, 1) + " triangles in scene.";
	log_volatile(log_.Debug(triangle_count_info_));
	if (test_ok_) {
		context_ = "cone function";
		const float cone_amplitude = 4.0f;
		tbc::TerrainConeFunction cone_function(cone_amplitude, volcano_position, (patch_left+1)*2, patch_left*3);
		tbc::TerrainWidthFunction width_function(0.5f, &cone_function);
		const float cone_push_vector[] = {0.0f, -2.0f, -1.5f, -3.0f, -0.0f};
		tbc::TerrainPushFunction push_function(cone_push_vector, 5, &width_function);
		for (int x = 0; x < patch_count; ++x) {
			push_function.AddFunction(*patch[x]);
		}
		//test_ok_ = (grid[2*vertex_count_x*((vertex_count_y+1)/4)+(vertex_count_x-1)*2].z >= cone_amplitude*0.8);
		deb_assert(test_ok_);
	}
	if (test_ok_) {
		context_ = "hemisphere functions";
		const float hemisphere_amplitude = 4.0f;
		tbc::TerrainHemisphereFunction hemisphere_function(-hemisphere_amplitude, volcano_position, patch_left-1, patch_left-1);
		for (int x = 0; x < patch_count; ++x) {
			hemisphere_function.AddFunction(*patch[x]);
		}
		tbc::TerrainHemisphereFunction hemisphere_function2(hemisphere_amplitude, volcano_position, patch_left/2, patch_left/2);
		for (int x = 0; x < patch_count; ++x) {
			hemisphere_function2.AddFunction(*patch[x]);
		}
		//test_ok_ = (grid[2*vertex_count_x*((vertex_count_y+1)/4)+(vertex_count_x-1)*2].z >= cone_amplitude*0.8);
		deb_assert(test_ok_);
	}
	if (test_ok_) {
		context_ = "dune function";
		const float dune_amplitude = 4.0f;
		tbc::TerrainDuneFunction dune_function(0.1f, 1.0f, dune_amplitude, dune_position, patch_left*1.5f, patch_left*3);
		const float dune_amplitude_vector[] = {1.0f, 2.0f, 1.0f, 1.5f, 1.0f, 1.0f};
		tbc::TerrainAmplitudeFunction amplitude_function(dune_amplitude_vector, 6, &dune_function);
		const float dune_push_vector[] = {0.0f, 1.5f, 2.5f, -1.0f, -2.0f, 0.0f};
		tbc::TerrainPushFunction push_function(dune_push_vector, 6, &amplitude_function);
		tbc::TerrainRotateFunction rotate_function(-lepra::PIF*3/4, &push_function);
		for (int x = 0; x < patch_count; ++x) {
			rotate_function.AddFunction(*patch[x]);
		}
		//test_ok_ = (grid[2*vertex_count_x*((vertex_count_y+1)/4)+(vertex_count_x-1)*2].z >= cone_amplitude*0.9);
		deb_assert(test_ok_);
	}
	if (test_ok_) {
		context_ = "zero effect validation";
		//test_ok_ = (grid[0].z == 0 && grid[1].z == 0);
		deb_assert(test_ok_);
	}
	if (test_ok_) {
		context_ = "initializing patch geometry";
		for (int x = 0; test_ok_ && x < patch_count; ++x) {
			test_ok_ = InitializeGeometry(patch[x]);
			deb_assert(test_ok_);
		}
	}
	if (test_ok_) {
		context_ = "rendering terrain";
		lepra::xform _transform;
		_transform.RotatePitch(-0.4f);
		_transform.MoveUp(3.0f);
		_transform.MoveBackward(20.0f);

		for (int x = 0; x < patch_count; ++x) {
			patch[x]->SetAlwaysVisible(true);
			uitbc::Renderer::GeometryID _geometry = g_renderer->AddGeometry(patch[x], uitbc::Renderer::kMatSingleTextureSolid, uitbc::Renderer::kNoShadows);
			g_renderer->TryAddGeometryTexture(_geometry, g_texture_id[kTexturemap]);
		}
		g_renderer->SetCameraTransformation(_transform);
	}
}

TerrainFunctionTest::~TerrainFunctionTest() {
}

void TerrainFunctionTest::UpdateScene(double, double) {
	extra_info_ = wstrutil::Encode(triangle_count_info_);
}

bool TestRayPicker(const lepra::LogDecorator& log) {
	g_total_fps = 0;

	str _context = "clear";
	bool test_ok = ResetAndClearFrame();
	g_renderer->SetCameraTransformation(xform());

	if (test_ok) {
		str _context = "pick and inverse";
		PixelCoord xs(640, 480/2);
		PixelCoord ys(640/2, 480);
		PixelCoord zs(0, 0);
		vec3 xp = g_renderer->ScreenCoordToVector(xs)*100;
		vec3 yp = g_renderer->ScreenCoordToVector(ys)*100;
		vec3 zp = g_renderer->ScreenCoordToVector(zs)*100;
		vec2 xb = g_renderer->PositionToScreenCoord(xp, 0);
		vec2 yb = g_renderer->PositionToScreenCoord(yp, 0);
		vec2 zb = g_renderer->PositionToScreenCoord(zp, 0);
		test_ok = ((int)xb.x == xs.x && (int)xb.y == xs.y &&
			   (int)yb.x == ys.x && (int)yb.y == ys.y &&
			   (int)zb.x == zs.x && (int)yb.y == ys.y);
		deb_assert(test_ok);
	}

	ReportTestResult(log, "TestRayPicker", _context, test_ok);
	return test_ok;
}

bool TestMaterials(const lepra::LogDecorator& log, double show_time) {
	g_total_fps = 0;

	str _context = "clear";
	bool test_ok = ResetAndClearFrame();

	//uitbc::TriangleBasedGeometry* _geometry = uitbc::BasicMeshCreator::CreateCone(15, 40, 24);
	uitbc::TriangleBasedGeometry* _geometry = uitbc::BasicMeshCreator::CreateFlatBox(10, 10, 10, 1, 1, 4);
	//uitbc::TriangleBasedGeometry* _geometry = uitbc::BasicMeshCreator::CreateEllipsoid(10, 11, 12, 10, 10);
	if (test_ok) {
		_context = "initialize geometry";
		test_ok = InitializeGeometry(_geometry);
	}
	//uitbc::Renderer::GeometryID geometry_id = uitbc::Renderer::INVALID_GEOMETRY;
	if (test_ok) {
		_context = "add vertex color data";
		test_ok = AddRandomVertexColor(_geometry);
	}
	if (test_ok) {
		_context = "add texture coords";
		test_ok = AddMappingCoords(_geometry);
	}
	/*if (test_ok) {
		_context = "add geometry";
		geometry_id = g_renderer->AddGeometry(_geometry, uitbc::Renderer::kMatSingleTextureSolid, g_texture_id, 1, uitbc::Renderer::kNoShadows);
		test_ok = (geometry_id != uitbc::Renderer::INVALID_GEOMETRY);
	}*/

	int frame_count = 0;
	lepra::HiResTimer total_timer;
	bool first_time = true;
	lepra::HiResTimer frame_timer;
	lepra::xform object_transform;
	do {
		test_ok = !lepra::SystemManager::GetQuitRequest();

		if (test_ok) {
			test_ok = ClearBackground();
			deb_assert(test_ok);
		}

		// ----------------------- Draw all frames and geometry within. -----------------------
		uitbc::Renderer::LightID lights[2];
		lights[0] = uitbc::Renderer::INVALID_LIGHT;
		lights[1] = uitbc::Renderer::INVALID_LIGHT;
		for (int y = 0; y < 4; y += 2) {
			wstr mode_info;
			// Set lighting according to what row we're rendering.
			if (y == 0) {
				mode_info = L"Ambient";
				g_renderer->SetLightsEnabled(false);
				g_renderer->SetAmbientLight(0.5, 0.5, 0.5);
			} else {
				mode_info = L"2 point lights";
				if (test_ok) {
					_context = "add point light 0";
					g_renderer->SetAmbientLight(0.1f, 0.1f, 0.1f);
					g_renderer->SetLightsEnabled(true);
					lights[0] = g_renderer->AddPointLight(uitbc::Renderer::kLightStatic, vec3(30, kObjectDistance - 60, -30), vec3(50, 100, 50), kObjectDistance, kObjectDistance*10);
					test_ok = (lights[0] != uitbc::Renderer::INVALID_LIGHT);
					deb_assert(test_ok);
				}
				if (test_ok) {
					_context = "add point light 1";
					lights[1] = g_renderer->AddPointLight(uitbc::Renderer::kLightStatic, vec3(-30, kObjectDistance - 60, 30), vec3(50, 50, 100), kObjectDistance, kObjectDistance*10);
					test_ok = (lights[1] != uitbc::Renderer::INVALID_LIGHT);
					deb_assert(test_ok);
				}
				if (test_ok) {
					_context = "add # lights";
					test_ok = (g_renderer->GetLightCount() == 2);
					deb_assert(test_ok);
				}
			}

			// Render the different scenes.
			if (test_ok) {
				_context = "single solid";
				ClearSubframe(0, 0+y, 5, 4, 30, false);
				test_ok = QuickRender(_geometry, uitbc::Renderer::kMatSingleColorSolid, false, &object_transform);
				deb_assert(test_ok);
				PrintInfo(L"Flat\nSolid\n" + mode_info);
			}
			if (test_ok) {
				_context = "vertex solid";
				ClearSubframe(1, 0+y, 5, 4, 30, false);
				// TODO: make this render correctly with vertex colors.
				test_ok = QuickRender(_geometry, uitbc::Renderer::kMatVertexColorSolid, false, &object_transform);
				deb_assert(test_ok);
				PrintInfo(L"Vertex color\nSolid\n" + mode_info);
			}
			if (test_ok) {
				_context = "single texture blended";
				ClearSubframe(2, 0+y, 5, 4, 30, false);
				g_texture_map_count = 1;
				test_ok = QuickRender(_geometry, uitbc::Renderer::kMatSingleTextureBlended, false, &object_transform);
				deb_assert(test_ok);
				g_texture_map_count = 0;
				PrintInfo(L"Tex\nTransparent\n" + mode_info);
			}
			if (test_ok) {
				_context = "texture and lightmap";
				ClearSubframe(3, 0+y, 5, 4, 30, false);
				g_texture_map_count = 2;
				test_ok = QuickRender(_geometry, uitbc::Renderer::kMatTextureAndLightmap, false, &object_transform);
				deb_assert(test_ok);
				g_texture_map_count = 0;
				PrintInfo(L"Tex&lightmap\nSolid\n" + mode_info);
			}
			if (test_ok) {
				_context = "single envmap solid";
				_geometry->GenerateVertexNormalData();
				ClearSubframe(4, 0+y, 5, 4, 30, false);
				g_texture_map_count = 1;
				g_texture_index = kEnvmap;
				test_ok = QuickRender(_geometry, uitbc::Renderer::kMatSingleColorEnvmapSolid, false, &object_transform);
				deb_assert(test_ok);
				g_texture_map_count = 0;
				g_texture_index = kTexturemap;
				PrintInfo(L"Envmap\nSolid\n" + mode_info);
			}
			if (test_ok) {
				_context = "single solid PXS";
				ClearSubframe(0, 1+y, 5, 4, 30, false);
				test_ok = QuickRender(_geometry, uitbc::Renderer::kMatSingleColorSolidPxs, false, &object_transform);
				deb_assert(test_ok);
				PrintInfo(L"Flat PXS\nSolid\n" + mode_info);
			}
			if (test_ok) {
				_context = "single texture solid PXS";
				ClearSubframe(2, 1+y, 5, 4, 30, false);
				g_texture_map_count = 1;
				test_ok = QuickRender(_geometry, uitbc::Renderer::kMatSingleTextureSolidPxs, false, &object_transform);
				deb_assert(test_ok);
				g_texture_map_count = 0;
				PrintInfo(L"Texture PXS\nSolid\n" + mode_info);
			}
			if (test_ok) {
				_context = "texture and lightmap PXS";
				ClearSubframe(3, 1+y, 5, 4, 30, false);
				g_texture_map_count = 2;
				test_ok = QuickRender(_geometry, uitbc::Renderer::kMatTextureAndLightmapPxs, false, &object_transform);
				deb_assert(test_ok);
				g_texture_map_count = 0;
				PrintInfo(L"Tex&light PXS\nSolid\n" + mode_info);
			}
			if (test_ok) {
				_context = "texture sbmap PXS";
				ClearSubframe(4, 1+y, 5, 4, 30, false);
				g_texture_map_count = 1;
				g_texture_index = kMultimap;
				test_ok = QuickRender(_geometry, uitbc::Renderer::kMatTextureSbmapPxs, false, &object_transform);
				deb_assert(test_ok);
				g_texture_map_count = 0;
				g_texture_index = kTexturemap;
				PrintInfo(L"Tex&sbmap PXS\nSolid\n" + mode_info);
			}

			// Remove lights.
			if (lights[0] != uitbc::Renderer::INVALID_LIGHT) {
				g_renderer->RemoveLight(lights[0]);
				g_renderer->RemoveLight(lights[1]);
				lights[0] = uitbc::Renderer::INVALID_LIGHT;
				lights[1] = uitbc::Renderer::INVALID_LIGHT;
				if (test_ok) {
					_context = "remove # lights";
					test_ok = (g_renderer->GetLightCount() == 0);
					deb_assert(test_ok);
				}
			}
		}

		// ----------------------- Update geometry orientation. -----------------------
		if (test_ok) {
			LEPRA_MEASURE_SCOPE(UpdateScreen);

			frame_timer.UpdateTimer();
			g_display->UpdateScreen();
			uilepra::Core::ProcessMessages();
			lepra::RotationMatrixF rotation;
			rotation.RotateAroundVector(lepra::vec3(1, -1, 1), (float)(frame_timer.GetTimeDiff()*1.79));
			object_transform.SetOrientation(rotation);
		}

		if (first_time) {
			first_time = false;
			total_timer.PopTimeDiff();
		} else {
			total_timer.UpdateTimer();
			++frame_count;
		}
	} while (test_ok && total_timer.GetTimeDiff() < show_time);

	//g_renderer->RemoveGeometry(geometry_id);

	delete (_geometry);

	// Put renderer back in
	g_renderer->SetLightsEnabled(false);
	g_renderer->SetAmbientLight(0.5, 0.5, 0.5);

	// Assign kFps meter for next test.
	g_total_fps = frame_count/total_timer.GetTimeDiff();

	ReportTestResult(log, "TestMaterials", _context, test_ok);
	return (test_ok);
}

bool TestFps(const lepra::LogDecorator& log, double average_fps) {
	str _context = "too low";
	bool test_ok = (g_total_fps > average_fps/8.0);
	deb_assert(test_ok);
	if (test_ok) {
		_context = "too high";
		test_ok = (g_total_fps < average_fps*8.0);
		deb_assert(test_ok);
	}
	ReportTestResult(log, lepra::strutil::Format("FPS (%.1f)", g_total_fps), _context, test_ok);
	return (test_ok);
}

bool TestGenerate(const lepra::LogDecorator& log, double show_time) {
	str _context;
	bool test_ok = true;

	float _rotation_speed = lepra::PIF * 0.25f;

	if (test_ok) {
		_context = "box";
		uitbc::TriangleBasedGeometry* _geometry = uitbc::BasicMeshCreator::CreateFlatBox(20, 10, 5, 1, 1, 1);
		test_ok = InitializeGeometry(_geometry);
		deb_assert(test_ok);
		if (test_ok) {
			AddRandomVertexColor(_geometry);
			ClearSubframe();
			test_ok = QuickRender(_geometry, uitbc::Renderer::kMatVertexColorSolid, true, 0, show_time/5, 0, true, _rotation_speed);
			deb_assert(test_ok);
		}
		delete (_geometry);
	}

	if (test_ok) {
		_context = "cone";
		uitbc::TriangleBasedGeometry* _geometry = uitbc::BasicMeshCreator::CreateCone(10, 20, 11);
		test_ok = InitializeGeometry(_geometry);
		deb_assert(test_ok);
		if (test_ok) {
			AddRandomVertexColor(_geometry);
			ClearSubframe();
			test_ok = QuickRender(_geometry, uitbc::Renderer::kMatVertexColorSolid, true, 0, show_time/5, 0, true, _rotation_speed);
			deb_assert(test_ok);
		}
		delete (_geometry);
	}

	if (test_ok) {
		_context = "cylinder";
		uitbc::TriangleBasedGeometry* _geometry = uitbc::BasicMeshCreator::CreateCylinder(10, 15, 25, 5);
		test_ok = InitializeGeometry(_geometry);
		deb_assert(test_ok);
		if (test_ok) {
			AddRandomVertexColor(_geometry);
			ClearSubframe();
			test_ok = QuickRender(_geometry, uitbc::Renderer::kMatVertexColorSolid, true, 0, show_time/5, 0, true, _rotation_speed);
			deb_assert(test_ok);
		}
		delete (_geometry);
	}

	if (test_ok) {
		_context = "ellipsoid";
		uitbc::TriangleBasedGeometry* _geometry = uitbc::BasicMeshCreator::CreateEllipsoid(10, 15, 20, 8, 8);
		test_ok = InitializeGeometry(_geometry);
		deb_assert(test_ok);
		if (test_ok) {
			AddRandomVertexColor(_geometry);
			ClearSubframe();
			test_ok = QuickRender(_geometry, uitbc::Renderer::kMatVertexColorSolid, true, 0, show_time/5, 0, true, _rotation_speed);
			deb_assert(test_ok);
		}
		delete (_geometry);
	}

	if (test_ok) {
		_context = "torus";
		uitbc::TriangleBasedGeometry* _geometry = uitbc::BasicMeshCreator::CreateTorus(15, 4, 8, 14, 10);
		test_ok = InitializeGeometry(_geometry);
		deb_assert(test_ok);
		if (test_ok) {
			AddRandomVertexColor(_geometry);
			ClearSubframe();
			test_ok = QuickRender(_geometry, uitbc::Renderer::kMatVertexColorSolid, true, 0, show_time/5, 0, true, _rotation_speed);
			deb_assert(test_ok);
		}
		delete (_geometry);
	}

	ReportTestResult(log, "GeomGeneration", _context, test_ok);
	return (test_ok);
}


GeometryReferenceTest::GeometryReferenceTest(const lepra::LogDecorator& log) :
	SceneTest(log, "GeometryReferenceTest") {
	g_renderer->SetClearColor(lepra::YELLOW);

	// Default material...
	tbc::GeometryBase::BasicMaterialSettings _material(
		lepra::vec3(0, 0, 0),
		lepra::vec3(1.0f, 1.0f, 1.0f),
		lepra::vec3(0.1f, 0.1f, 0.1f),
		0.8f, 1.0f, true);

	sphere_ = uitbc::BasicMeshCreator::CreateEllipsoid(1, 1, 1, 16, 8);
	_material.ambient_.x = 1.0f;
	_material.ambient_.y = 1.0f;
	_material.ambient_.z = 1.0f;
	sphere_->SetBasicMaterialSettings(_material);

	sphere_->SetAlwaysVisible(false);
	uitbc::Renderer::GeometryID graphic_id = g_renderer->AddGeometry(sphere_, uitbc::Renderer::kMatSingleColorSolid, uitbc::Renderer::kCastShadows);
	g_renderer->TryAddGeometryTexture(graphic_id, g_texture_id[kTexturemap]);
	lepra::xform transf;
	transf.SetPosition(lepra::vec3(1000, 0, 1000));
	sphere_->SetTransformation(transf);

	sphere_reference_ = new tbc::GeometryReference(sphere_);
	sphere_reference_->SetAlwaysVisible(true);
	graphic_id = g_renderer->AddGeometry(sphere_reference_, uitbc::Renderer::kMatSingleColorSolid, uitbc::Renderer::kCastShadows);
	transf.SetIdentity();
	sphere_reference_->SetTransformation(transf);

	transf.SetIdentity();
	transf.RotatePitch(-lepra::PIF / 8.0f);
	transf.SetPosition(lepra::vec3(0, -2, 4));
	g_renderer->SetCameraTransformation(transf);
}

GeometryReferenceTest::~GeometryReferenceTest() {
	delete (sphere_);
	delete (sphere_reference_);
}

void GeometryReferenceTest::UpdateScene(double total_time, double) {
	lepra::xform transf;
	transf.SetPosition(lepra::vec3(0, 0, (float)::sin(total_time)*3));
	sphere_reference_->SetTransformation(transf);
}



BumpMapSceneTest::BumpMapSceneTest(const lepra::LogDecorator& log) :
	SceneTest(log, "BumpMapScene") {
	g_renderer->SetClearColor(lepra::RED);

	// Default material...
	tbc::GeometryBase::BasicMaterialSettings _material(
		lepra::vec3(0, 0, 0),
		lepra::vec3(1.0f, 1.0f, 1.0f),
		lepra::vec3(0.1f, 0.1f, 0.1f),
		0.8f, 1.0f, true);

	InitTerrain();

	sphere_ = uitbc::BasicMeshCreator::CreateEllipsoid(1, 1, 1, 16, 8);
	//torus_  = uitbc::BasicMeshCreator::CreateTorus(0.4f, 0.06f, 0.12f, 16, 12);
	torus_  = uitbc::BasicMeshCreator::CreateFlatBox(1, 1, 1);
	cone_   = uitbc::BasicMeshCreator::CreateCone(1, 5, 8);

	torus_->SplitVertices();

	// Setup sphere.
	_material.ambient_ = lepra::vec3(1,1,1);
	sphere_->SetBasicMaterialSettings(_material);
	_material.ambient_ = lepra::vec3(0,0,0);
	//AddRandomVertexColor(sphere_);
	AddMappingCoords(sphere_);
	sphere_->SetAlwaysVisible(true);
	uitbc::Renderer::GeometryID graphic_id = g_renderer->AddGeometry(sphere_, uitbc::Renderer::kMatSingleColorSolid, uitbc::Renderer::kNoShadows);
	g_renderer->TryAddGeometryTexture(graphic_id, g_texture_id[kTexturemap]);

	// Setup cone forest.
	_material.diffuse_ = lepra::vec3(0.2f,1,0.2f);
	cone_->SetBasicMaterialSettings(_material);
	uitbc::GeometryBatch* cone_batch = new uitbc::GeometryBatch(cone_);
	lepra::xform positions[100];
	for (int i = 0; i < 100; i++) {
		positions[i].GetPosition().x = lepra::Random::Uniform(0.0f, 50.0f);
		positions[i].GetPosition().y = lepra::Random::Uniform(0.0f, 50.0f);
		positions[i].GetPosition().z = 0;
	}

	lepra::xform transf;
	transf.SetPosition(lepra::vec3(0, 0, -1));

	cone_batch->SetInstances(positions, vec3(), 100, 0, 0.8f, 1.2f, 0.8f, 1.2f, 0.8f, 1.2f);
	cone_batch->SetAlwaysVisible(true);
	cone_batch->SetTransformation(transf);
	cone_batch->CalculateBoundingRadius();
	uitbc::Renderer::GeometryID graphic_id2 = g_renderer->AddGeometry(cone_batch, uitbc::Renderer::kMatSingleColorSolid, uitbc::Renderer::kNoShadows);
	g_renderer->TryAddGeometryTexture(graphic_id2, g_texture_id[kTexturemap]);

	// Setup torus using the Model-class.
	_material.diffuse_ = lepra::vec3(1,1,1);
	torus_->SetBasicMaterialSettings(_material);
	AddUVAnimation(torus_);
	AddRandomVertexColor(torus_);
	int uv_set = torus_->AddEmptyUVSet();
	uitbc::UVMapper::ApplyCubeMapping(torus_, uv_set, 1, lepra::Vector2DD(0.5, 0.5));

	transf.SetIdentity();
	transf.SetPosition(lepra::vec3(-0.5, 2, 0));
	transf.RotatePitch(-lepra::PIF / 3.0f);
	transf.RotateWorldZ(lepra::PIF / 4.0f);
	transf.RotateWorldX(lepra::PIF / 4.0f);
	model_.SetTransformation(transf);

	// Setup a transform animation bone.
	tbc::BoneHierarchy* transform_bones = new tbc::BoneHierarchy;
	transform_bones->SetBoneCount(1);
	transform_bones->FinalizeInit(tbc::BoneHierarchy::kTransformNone);

	animation_ = new tbc::BoneAnimation;
	animation_->SetKeyframeCount(8, true);
	animation_->SetBoneCount(1);
	animation_->SetTimeTag(0, 0);
	animation_->SetTimeTag(1, 0.5f);
	animation_->SetTimeTag(2, 1.0f);
	animation_->SetTimeTag(3, 1.5f);
	animation_->SetTimeTag(4, 2.0f);
	animation_->SetTimeTag(5, 2.5f);
	animation_->SetTimeTag(6, 3.0f);
	animation_->SetTimeTag(7, 3.5f);
	animation_->SetTimeTag(8, 4.0f);
	animation_->SetBoneTransformation(0, 0,
		lepra::xform(lepra::g3x3IdentityMatrixF,
				       lepra::vec3(-1, -1, -1)));
	animation_->SetBoneTransformation(1, 0,
		lepra::xform(lepra::g3x3IdentityMatrixF,
				       lepra::vec3(-1, -1, 1)));
	animation_->SetBoneTransformation(2, 0,
		lepra::xform(lepra::g3x3IdentityMatrixF,
				       lepra::vec3(-1, 1, 1)));
	animation_->SetBoneTransformation(3, 0,
		lepra::xform(lepra::g3x3IdentityMatrixF,
				       lepra::vec3(-1, 1, -1)));
	animation_->SetBoneTransformation(4, 0,
		lepra::xform(lepra::g3x3IdentityMatrixF,
				       lepra::vec3(1, 1, -1)));
	animation_->SetBoneTransformation(5, 0,
		lepra::xform(lepra::g3x3IdentityMatrixF,
				       lepra::vec3(1, 1, 1)));
	animation_->SetBoneTransformation(6, 0,
		lepra::xform(lepra::g3x3IdentityMatrixF,
				       lepra::vec3(1, -1, 1)));
	animation_->SetBoneTransformation(7, 0,
		lepra::xform(lepra::g3x3IdentityMatrixF,
				       lepra::vec3(1, -1, -1)));

	animator_ = new tbc::BoneAnimator(transform_bones);
	animator_->AddAnimation("TransformAnimation", animation_);

	model_.AddAnimator("TransformAnimator", animator_);

	torus_handler_ = new uitbc::DefaultStaticGeometryHandler(torus_, 1, &g_texture_id[kMultimap], 1, uitbc::Renderer::kMatTextureAndDiffuseBumpmapPxs, uitbc::Renderer::kCastShadows, g_renderer);
	model_.AddGeometry("Torus", torus_handler_, "TransformAnimator");
	model_.SetAlwaysVisible(true);

	model_.StartAnimation("TransformAnimation", 0, tbc::BoneAnimation::kModePlayLoop);



	lepra::quat q1;
	lepra::quat q2;
	lepra::quat q3;
	lepra::quat q4;

	q1.RotateAroundWorldZ(lepra::PIF / 5.33f);
	q1.RotateAroundWorldX(lepra::PIF / 1.234f);
	q2.RotateAroundWorldY(lepra::PIF / 5.33f);
	q2.RotateAroundWorldX(-lepra::PIF / 1.234f);

	// Setup lights.

	transf.SetIdentity();
	transf.RotatePitch(-lepra::PIF / 8.0f);
	transf.SetPosition(lepra::vec3(0, -0.5, 1));
	g_renderer->SetCameraTransformation(transf);

	g_renderer->SetLightsEnabled(true);
	g_renderer->SetShadowMode(uitbc::Renderer::kCastShadows, uitbc::Renderer::kShVolumesOnly);
	g_renderer->SetAmbientLight(0.4f, 0.4f, 0.4f);
	g_renderer->SetViewFrustum(90, 0.01f, 300.0f);

	light_pos_.Set(-8, 10, 4);
	light_dir_.Set(1, -1, -1);
	light_color_.Set(1, 1, 1);

	transf.SetIdentity();
	transf.SetPosition(light_pos_);
	sphere_->SetTransformation(transf);

	//light_id_ = g_renderer->AddSpotLight(uitbc::Renderer::kLightMovable, light_pos, light_dir, lLightColor, 30, 32, 100, 500);
	//light_id_ = g_renderer->AddDirectionalLight(uitbc::Renderer::kLightMovable, light_dir, lLightColor, 500);
	light_id_ = g_renderer->AddPointLight(uitbc::Renderer::kLightMovable, light_pos_, vec3(2.0f, 2.0f, 2.0f), 1000, 500);
}

BumpMapSceneTest::~BumpMapSceneTest() {
	delete torus_handler_;
	delete animator_;
	delete animation_;
}

void BumpMapSceneTest::InitTerrain() {
	tbc::GeometryBase::BasicMaterialSettings _material(
		lepra::vec3(0, 0, 0),
		lepra::vec3(1.0f, 1.0f, 1.0f),
		lepra::vec3(0.1f, 0.1f, 0.1f),
		0.7f, 1.0f, true);

	tbc::TerrainFunction* tf[3];
	tf[0] = new tbc::TerrainConeFunction(15.0f, lepra::vec2(-50, -50), 20.0f, 30.0f);
	tf[1] = new tbc::TerrainHemisphereFunction(-10.0f, lepra::vec2(-50, -40), 10.0f, 15.0f);
	tf[2] = new tbc::TerrainHemisphereFunction(10.0f, lepra::vec2(100, 100), 40.0f, 50.0f);
	//tf[1] = new tbc::TerrainDuneFunction(2, 1, 10, lepra::vec2(100, 100), 20.0f, 30.0f);
	tbc::TerrainFunctionGroup tf_group(tf, 3, lepra::kCopyReference, lepra::kTakeSubdataOwnership);

	tbc::TerrainPatch::SetDimensions(4, 100);

	lepra::xform transf;
	transf.SetPosition(lepra::vec3(0, 0, -1));

	t_patch_[0] = new tbc::TerrainPatch(lepra::Vector2D<int>(0, 0));
	tf_group.AddFunctions(*t_patch_[0]);
	t_patch_[0]->GenerateVertexNormalData();
	t_patch_[0]->SetBasicMaterialSettings(_material);
	t_patch_[0]->SetAlwaysVisible(true);
	t_patch_[0]->SetTransformation(transf);

	const int levels = 2;
	int prev_xy = 0;
	int i;
	for (i = 0; i < levels; i++) {
		int patch_index = i * 8 + 1;
		int size_multiplier = lepra::Math::Pow(3, i);
		for (int y = -1; y <= 1; y++) {
			for (int x = -1; x <= 1; x++) {
				int edge_flags = 0;
				if (i > 0) {
					if (y == 0) {
						if (x == -1) {
							edge_flags |= tbc::TerrainPatch::kEastEdge;
						} else if (x == 1) {
							edge_flags |= tbc::TerrainPatch::kWestEdge;
						}
					} else if (x == 0) {
						if (y == -1) {
							edge_flags |= tbc::TerrainPatch::kNorthEdge;
						} else if (y == 1) {
							edge_flags |= tbc::TerrainPatch::kSouthEdge;
						}
					}
				}

				if (y != 0 || x != 0) {
					t_patch_[patch_index] = new tbc::TerrainPatch(lepra::Vector2D<int>(x * size_multiplier + prev_xy, y * size_multiplier + prev_xy), 0, i, edge_flags);
					tf_group.AddFunctions(*t_patch_[patch_index]);
					t_patch_[patch_index]->GenerateVertexNormalData();
					t_patch_[patch_index]->SetBasicMaterialSettings(_material);
					t_patch_[patch_index]->SetAlwaysVisible(true);
					t_patch_[patch_index]->SetTransformation(transf);

					if ((edge_flags & tbc::TerrainPatch::kSouthEdge) != 0) {
						int south_index = -1;
						if (y == -1) {
							if (i == 0)
								south_index = 0;
							else
								south_index = (i - 1) * 8 + 1 + (x + 1);
						} else if (y == 0) {
							south_index = i * 8 + 1 + 6 + (x + 1);
						}

						if (south_index != -1) {
							t_patch_[patch_index]->ShareVerticesWithSouthNeighbour(*t_patch_[south_index]);
							t_patch_[patch_index]->MergeNormalsWithSouthNeighbour(*t_patch_[south_index]);
						}
					}
					if ((edge_flags & tbc::TerrainPatch::kNorthEdge) != 0) {
					}
					if ((edge_flags & tbc::TerrainPatch::kWestEdge) != 0) {
					}
					if ((edge_flags & tbc::TerrainPatch::kEastEdge) != 0) {
					}


					patch_index++;
				}
			}
		}

		prev_xy += -size_multiplier;
	}

/*
	t_patch_[0]->ShareVerticesWithNorthNeighbour(*t_patch_[3]);
	t_patch_[1]->ShareVerticesWithNorthNeighbour(*t_patch_[4]);
	t_patch_[2]->ShareVerticesWithNorthNeighbour(*t_patch_[5]);

	t_patch_[3]->ShareVerticesWithNorthNeighbour(*t_patch_[6]);
	t_patch_[4]->ShareVerticesWithNorthNeighbour(*t_patch_[7]);
	t_patch_[5]->ShareVerticesWithNorthNeighbour(*t_patch_[8]);
*/

	uitbc::Renderer::TextureID texture[2];
	texture[0] = g_texture_id[kMultimap];
	texture[1] = g_texture_id[kLightmap];

	for (i = 0; i < 17; i++) {
//		uitbc::Renderer::MaterialType _material = uitbc::Renderer::kMatSingleTextureSolid;
		//uitbc::Renderer::MaterialType _material = uitbc::Renderer::kMatTextureAndDiffuseBumpmapPxs;
//		g_renderer->AddGeometry(t_patch_[i], _material, texture, 2, uitbc::Renderer::kNoShadows);
	}
}

void BumpMapSceneTest::UpdateScene(double total_time, double delta_time) {
	lepra::vec3 temp_pos(light_pos_ + lepra::vec3(10.0f * (float)sin(total_time * 2.0), 0, 0));
	g_renderer->SetLightPosition(light_id_, temp_pos);
	lepra::xform transf;
	transf.SetPosition(temp_pos);
	sphere_->SetTransformation(transf);


	transf = model_.GetTransformation();
	const float RPS = 0.125 * 0.25;
	lepra::quat q;
	q.RotateAroundWorldZ(RPS * (float)total_time * 2.0f * lepra::PIF);
	transf.SetOrientation(q);
	model_.SetTransformation(transf);
	model_.Update(delta_time * 0.1);
}

bool TestGUI(const lepra::LogDecorator& /*log*/, double show_time) {
	g_painter->ResetClippingRect();
	g_painter->SetRenderMode(uitbc::Painter::kRmNormal);
	g_painter->PrePaint(true);

	uitbc::DesktopWindow* desktop_window = new uitbc::DesktopWindow(g_input, g_painter, lepra::DARK_GREEN, new uitbc::FloatingLayout(), 0, 0);
	desktop_window->SetPreferredSize(g_screen->GetWidth(), g_screen->GetHeight());
	desktop_window->AddChild(new GUITestWindow);
	//desktop_window->AddChild(new uitbc::ASEFileConverter(desktop_window));

	g_input->ActivateAll();

	lepra::Timer total_timer;

	bool test_ok = true;

//	((uitbc::SoftwarePainter*)g_painter)->SetIncrementalAlpha(true);

	do {
		uilepra::Core::ProcessMessages();
		if (lepra::SystemManager::GetQuitRequest()) {
			CloseRenderer();
			lepra::SystemManager::ExitProcess(0);
		}

		g_input->PollEvents();

		g_screen->SetBuffer(0);
		//g_painter->SetColor(lepra::OFF_BLACK);
		g_painter->FillRect(0, 0, g_screen->GetWidth(), g_screen->GetHeight());
		//g_font_manager->SetColor(lepra::OFF_BLACK);
		desktop_window->Repaint();
		{
			LEPRA_MEASURE_SCOPE(UpdateScreen);
			g_display->UpdateScreen();
		}

		total_timer.UpdateTimer();
	} while (test_ok && total_timer.GetTimeDiff() < show_time);

	delete desktop_window;

	return test_ok;
}

bool TestCubicSpline(const lepra::LogDecorator&, double show_time) {
	lepra::vec2 coordinates[4];
	coordinates[0].Set(0, 0);
	coordinates[1].Set(10, 500);
	coordinates[2].Set(400, 550);
	coordinates[3].Set(400, 0);

	float time_tags[10] = {0, 1, 2, 3, 4};
	lepra::CubicDeCasteljauSpline<lepra::vec2, float> spline(coordinates, time_tags, 4, lepra::CubicDeCasteljauSpline<lepra::vec2, float>::kTypeBezier);

	g_painter->ResetClippingRect();
	g_painter->SetRenderMode(uitbc::Painter::kRmNormal);

	g_painter->SetColor(lepra::BLUE);
	g_painter->FillRect(0, 0, g_painter->GetCanvas()->GetWidth(), g_painter->GetCanvas()->GetHeight());

	float step = 4.0f / (float)g_painter->GetCanvas()->GetWidth();
	spline.StartInterpolation(0);
	for (unsigned int i = 0; i < g_painter->GetCanvas()->GetWidth(); i++) {
		lepra::vec2 value = spline.GetValue();
		float x = value.x;
		float y = value.y;

		g_painter->SetColor(lepra::WHITE);
		g_painter->DrawPixel((int)x, (int)y);

		spline.StepInterpolation(step);
		g_display->UpdateScreen();
	}

	lepra::Timer timer;
	while (timer.GetTimeDiff() < show_time && g_input->ReadKey(uilepra::InputManager::kInKbdEsc) == false) {
		timer.UpdateTimer();
		if (lepra::SystemManager::GetQuitRequest()) {
			CloseRenderer();
			lepra::SystemManager::ExitProcess(0);
		}
		lepra::Thread::Sleep(0.01f);
	}

	return true;
}

void TestPainter() {
	g_painter->ResetClippingRect();
	uitbc::Painter::DisplayListID disp_id = g_painter->NewDisplayList();

	g_painter->BeginDisplayList(disp_id);

		g_painter->SetRenderMode(uitbc::Painter::kRmNormal);
		g_painter->SetColor(lepra::BLUE);
		g_painter->FillRect(0, 0, g_screen->GetWidth(), g_screen->GetHeight());

		g_painter->SetColor(lepra::WHITE);
		g_painter->DrawLine(0, 0, 0, g_screen->GetHeight());
		g_painter->DrawLine(g_screen->GetWidth() - 1, 0, g_screen->GetWidth() - 1, g_screen->GetHeight());
		g_painter->DrawLine(0, 0, g_screen->GetWidth(), 0);
		g_painter->DrawLine(0, g_screen->GetHeight() - 1, g_screen->GetWidth(), g_screen->GetHeight() - 1);

		g_painter->SetColor(lepra::GRAY);
		g_painter->FillRect(100, 100, 200, 300);

		g_painter->SetColor(lepra::PINK, 0);
		g_painter->SetColor(lepra::RED, 1);
		g_painter->Draw3DRect(100, 100, 200, 300, 1, false);

	g_painter->EndDisplayList();

	while(!lepra::SystemManager::GetQuitRequest()) {
		uilepra::Core::ProcessMessages();
		g_input->PollEvents();

		g_painter->RenderDisplayList(disp_id);

		g_display->UpdateScreen();
	}

	g_painter->DeleteDisplayList(disp_id);
}

bool TestUiTbc() {
	//RunRotationalAgreementTest();
	/*OpenRenderer(gUiTbcLog, uilepra::DisplayManager::kOpenglContext);
	TestPainter();
	CloseRenderer();*/


	bool test_ok = true;
#ifdef TEST_D3D_RENDERER
	for (unsigned y = 0; y < 2; ++y)
#else // TEST_D3D_RENDERER
	for (unsigned y = 0; y < 1; ++y)
#endif // TEST_D3D_RENDERER/!TEST_D3D_RENDERER
	{
		LEPRA_MEASURE_SCOPE(Graphics);
		if (test_ok) {
			LEPRA_MEASURE_SCOPE(OpenRenderer);
			if (y == 0) {
				test_ok = OpenRenderer(gUiTbcLog, uilepra::DisplayManager::kOpenglContext);
			} else {
				deb_assert(false);
			}
			deb_assert(test_ok);
		}
		/*//TestOpenGLPainter();
		if (test_ok) {
			LEPRA_MEASURE_SCOPE(GenerateTest);
			test_ok = TestGenerate(gUiTbcLog, 2.0);
		}*/
		if (test_ok) {
			test_ok = TestRayPicker(gUiTbcLog);
		}
		/*if (test_ok) {
			LEPRA_MEASURE_SCOPE(MaterialTest);
			test_ok = TestMaterials(gUiTbcLog, 20.0);
		}
		if (test_ok) {
			test_ok = TestFps(gUiTbcLog, 150.0);
		}
		if (test_ok) {
			test_ok = TestSkinningSaveLoad(gUiTbcLog, 2.0);
		}
		if (test_ok) {
			test_ok = TestMeshImport(gUiTbcLog, 4.0);
		}
		if (test_ok) {
			test_ok = TestLoadClass(gUiTbcLog);
		}
		if (test_ok) {
			test_ok = TestCubicSpline(gUiTbcLog, 1.0);
		}
		if (test_ok) {
			GeometryReferenceTest test(gUiTbcLog);
			test_ok = test.Run(10.0f);
		}
		if (test_ok && y == 0) {
			TerrainFunctionTest test(gUiTbcLog);
			test_ok = test.Run(3.0f);
		}
		if (test_ok && y == 0) {
			LEPRA_MEASURE_SCOPE(CompleteScene);
			BumpMapSceneTest test(gUiTbcLog);
			test_ok = test.Run(3.0f);
		}
		if (test_ok) {
			LEPRA_MEASURE_SCOPE(GuiTest);
			test_ok = TestGUI(gUiTbcLog, 3.0);
		}

		{
			LEPRA_MEASURE_SCOPE(CloseRenderer);
			test_ok = CloseRenderer();
		}

		if (test_ok) {
			uitbc::TriangleBasedGeometry mesh;
			bool lCastShadows = false;
			DiskFile file;
			test_ok = file.Open("data/road_sign_01_sign.mesh", DiskFile::kModeRead);
			deb_assert(test_ok);
			if (test_ok) {
				uitbc::ChunkyMeshLoader loader(&file, false);
				test_ok = loader.Load(&mesh, lCastShadows);
				deb_assert(test_ok);
				if (test_ok) {
					test_ok = (mesh.GetUVSetCount() == 1);
					deb_assert(test_ok);
				}
			}
		}*/
	}
	return (test_ok);
}

#endif //!CURE_TEST_WITHOUT_UI
