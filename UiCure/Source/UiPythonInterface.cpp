
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



#include "pch.h"
#if 0
#include <stdexcept>
#include "../../cure/include/pythoncontextmanager.h"
#include "../../cure/include/pythoncontextobject.h"
#include "../../lepra/include/PerformanceScope.h"
#include "../include/UiContextManager.h"
#include "../include/UiGraphicsManager.h"
#include "../include/UiGraphicsObject.h"
#include "../include/uipythoninterface.h"
#include "../include/uisoundmanager.h"
//#include "../include/uiterrainmanager.h"



BOOST_PYTHON_MODULE(uicure) {
	{
		boost::python::scope GfxScope = boost::python::class_<UiCure::pythoninterface::Graphics>("Graphics")
			.def("createScreen", &UiCure::pythoninterface::Graphics::CreateScreen)
			.staticmethod("createScreen")
			.def("getFullscreenResolutions", &UiCure::pythoninterface::Graphics::GetFullscreenResolutions)
			.staticmethod("getFullscreenResolutions")
		;
		boost::python::enum<uilepra::DisplayManager::ContextType>("renderType")
			.value("OPENGL", uilepra::DisplayManager::kOpenglContext)
			.value("DIRECT3D", uilepra::DisplayManager::kDirectxContext)
			.value("SOFTWARE", uilepra::DisplayManager::SOFTWARE_CONTEXT)
		;
		boost::python::class_<UiCure::pythoninterface::Graphics::Screen>("Screen")
			.def("getClippingRectangle", &UiCure::pythoninterface::Graphics::Screen::GetClippingRectangle)
			.def("setClippingRectangle", &UiCure::pythoninterface::Graphics::Screen::SetClippingRectangle)
			.def("moveCamera", &UiCure::pythoninterface::Graphics::Screen::MoveCamera)
			.def("render", &UiCure::pythoninterface::Graphics::Screen::Render)
			.def("createViewport", &UiCure::pythoninterface::Graphics::Screen::CreateViewport)
			.def("printText", &UiCure::pythoninterface::Graphics::Screen::PrintText)
		;
		boost::python::class_<UiCure::pythoninterface::Graphics::Viewport>("Viewport")
			.def("getClippingRectangle", &UiCure::pythoninterface::Graphics::Viewport::GetClippingRectangle)
			.def("setClippingRectangle", &UiCure::pythoninterface::Graphics::Viewport::SetClippingRectangle)
			.def("moveCamera", &UiCure::pythoninterface::Graphics::Viewport::MoveCamera)
			.def("render", &UiCure::pythoninterface::Graphics::Viewport::Render)
			.def("printText", &UiCure::pythoninterface::Graphics::Viewport::PrintText)
		;
	}

	boost::python::class_<UiCure::pythoninterface::Sound>("Sound")
		.def("setMicrophonePosition", &UiCure::pythoninterface::Sound::SetMicrophonePosition)
		.staticmethod("setMicrophonePosition")
		.def("setMicrophoneVelocity", &UiCure::pythoninterface::Sound::SetMicrophoneVelocity)
		.staticmethod("setMicrophoneVelocity")
		.def("setMicrophoneOrientation", &UiCure::pythoninterface::Sound::SetMicrophoneOrientation)
		.staticmethod("setMicrophoneOrientation")
	;
}



namespace UiCure {
namespace pythoninterface {



Graphics::Viewport::Viewport():
	x_(0),
	y_(0),
	width_(320),
	height_(180) {
}

Graphics::Viewport::Viewport(const Viewport& original) {
	*this = original;
}

Graphics::Viewport::~Viewport() {
}

const Graphics::Viewport& Graphics::Viewport::operator=(const Graphics::Viewport& original) {
	x_ = original.x_;
	y_ = original.y_;
	width_ = original.width_;
	height_ = original.height_;
	return (*this);
}

tuple Graphics::Viewport::GetClippingRectangle() {
	return (boost::python::make_tuple(x_, y_, width_, height_));
}

void Graphics::Viewport::SetClippingRectangle(int _x, int y, int width, int height) {
	x_ = _x;
	y_ = y;
	width_ = width;
	height_ = height;
}

void Graphics::Viewport::MoveCamera(const tuple& position, const tuple& orientation) {
	float __x = boost::python::extract<float>(position[0]);
	float _y = boost::python::extract<float>(position[1]);
	float _z = boost::python::extract<float>(position[2]);
	ContextManager::Get()->GetGraphicsManager()->SetCameraPosition(__x, _y, _z);
	float _theta = boost::python::extract<float>(orientation[0]);
	float _phi = boost::python::extract<float>(orientation[1]);
	float _gimbal = boost::python::extract<float>(orientation[2]);
	ContextManager::Get()->GetGraphicsManager()->SetCameraOrientation(_theta, _phi, _gimbal);
}

void Graphics::Viewport::Render() {
	ContextManager::Get()->GetGraphicsManager()->SetViewport(x_, y_, width_, height_);
	ContextManager::Get()->GetGraphicsManager()->RenderScene();
}

void Graphics::Viewport::PrintText(int _x, int y, const char* text) {
	lepra::AnsiString s(text);
	ContextManager::Get()->GetGraphicsManager()->PrintText(x_+_x, y_+y, s.ToCurrentCode());
}

Graphics::Screen::Screen():
	Viewport() {
	++reference_count_;
}

Graphics::Screen::Screen(const Screen& original):
	Viewport() {
	++reference_count_;
	*this = original;
}

Graphics::Screen::Screen(uilepra::DisplayManager::ContextType rendering_context, const tuple& mode, bool is_windowed):
	Viewport() {
	++reference_count_;

	int _width = boost::python::extract<int>(mode[0]);
	int _height = boost::python::extract<int>(mode[1]);
	int bpp = boost::python::extract<int>(mode[2]);
	int frequency = boost::python::extract<int>(mode[3]);
	_height = _height > 0? _height : _height = _width*3/4;
	SetClippingRectangle(0, 0, _width, _height);

	ContextManager::Get()->GetGraphicsManager()->Open(rendering_context, _width, _height, bpp, frequency, is_windowed);
}

Graphics::Screen::~Screen() {
	if (--reference_count_ <= 0) {
		ContextManager::Get()->GetGraphicsManager()->Close();
	}
}

const Graphics::Screen& Graphics::Screen::operator=(const Graphics::Screen& original) {
	Viewport::operator=(original);
	return (*this);
}

void Graphics::Screen::UpdateScreen() {
	ContextManager::Get()->GetGraphicsManager()->UpdateScreen();
}

Graphics::Viewport Graphics::Screen::CreateViewport(int _x, int y, int width, int height) {
	Viewport viewport;
	viewport.SetClippingRectangle(_x, y, width, height);
	return (viewport);
}

int Graphics::Screen::reference_count_ = 0;

Graphics::Screen Graphics::CreateScreen(uilepra::DisplayManager::ContextType rendering_context, const tuple& mode, bool is_windowed) {
	return (Screen(rendering_context, mode, is_windowed));
}

tuple Graphics::GetFullscreenResolutions() {
	tuple t;
	uilepra::DisplayManager* display = uilepra::DisplayManager::CreateDisplayManager(uilepra::DisplayManager::kOpenglContext);
	for (int x = 0; x < display->GetNumDisplayModes(); ++x) {
		uilepra::DisplayMode display_mode;
		display->GetDisplayMode(display_mode, x);
		t += boost::python::make_tuple(boost::python::make_tuple(display_mode.width_, display_mode.height_, display_mode.bit_depth_, display_mode.refresh_rate_));
	}
	delete (display);
	return (t);
}



void Sound::SetMicrophonePosition(float _x, float y, float z) {
	ContextManager::Get()->GetSoundManager()->SetMicrophonePosition(lepra::Vector3DD(_x, y, z));
}

void Sound::SetMicrophoneVelocity(float velocity_x, float velocity_y, float velocity_z) {
	ContextManager::Get()->GetSoundManager()->SetMicrophoneVelocity(lepra::Vector3DD(velocity_x, velocity_y, velocity_z));
}

void Sound::SetMicrophoneOrientation(float theta, float phi, float gimbal) {
	ContextManager::Get()->GetSoundManager()->SetMicrophoneOrientation(lepra::Vector3DD(theta, phi, gimbal));
}



}
}
#endif // 0
