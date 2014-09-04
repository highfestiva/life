
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



#include "pch.h"
#if 0
#include <stdexcept>
#include "../../Cure/Include/PythonContextManager.h"
#include "../../Cure/Include/PythonContextObject.h"
#include "../../Lepra/Include/PerformanceScope.h"
#include "../Include/UiContextManager.h"
#include "../Include/UiGraphicsManager.h"
#include "../Include/UiGraphicsObject.h"
#include "../Include/UiPythonInterface.h"
#include "../Include/UiSoundManager.h"
//#include "../Include/UiTerrainManager.h"



BOOST_PYTHON_MODULE(uicure)
{
	{
		boost::python::scope GfxScope = boost::python::class_<UiCure::PythonInterface::Graphics>("Graphics")
			.def("createScreen", &UiCure::PythonInterface::Graphics::CreateScreen)
			.staticmethod("createScreen")
			.def("getFullscreenResolutions", &UiCure::PythonInterface::Graphics::GetFullscreenResolutions)
			.staticmethod("getFullscreenResolutions")
		;
		boost::python::enum<UiLepra::DisplayManager::ContextType>("renderType")
			.value("OPENGL", UiLepra::DisplayManager::OPENGL_CONTEXT)
			.value("DIRECT3D", UiLepra::DisplayManager::DIRECTX_CONTEXT)
			.value("SOFTWARE", UiLepra::DisplayManager::SOFTWARE_CONTEXT)
		;
		boost::python::class_<UiCure::PythonInterface::Graphics::Screen>("Screen")
			.def("getClippingRectangle", &UiCure::PythonInterface::Graphics::Screen::GetClippingRectangle)
			.def("setClippingRectangle", &UiCure::PythonInterface::Graphics::Screen::SetClippingRectangle)
			.def("moveCamera", &UiCure::PythonInterface::Graphics::Screen::MoveCamera)
			.def("render", &UiCure::PythonInterface::Graphics::Screen::Render)
			.def("createViewport", &UiCure::PythonInterface::Graphics::Screen::CreateViewport)
			.def("printText", &UiCure::PythonInterface::Graphics::Screen::PrintText)
		;
		boost::python::class_<UiCure::PythonInterface::Graphics::Viewport>("Viewport")
			.def("getClippingRectangle", &UiCure::PythonInterface::Graphics::Viewport::GetClippingRectangle)
			.def("setClippingRectangle", &UiCure::PythonInterface::Graphics::Viewport::SetClippingRectangle)
			.def("moveCamera", &UiCure::PythonInterface::Graphics::Viewport::MoveCamera)
			.def("render", &UiCure::PythonInterface::Graphics::Viewport::Render)
			.def("printText", &UiCure::PythonInterface::Graphics::Viewport::PrintText)
		;
	}

	boost::python::class_<UiCure::PythonInterface::Sound>("Sound")
		.def("setMicrophonePosition", &UiCure::PythonInterface::Sound::SetMicrophonePosition)
		.staticmethod("setMicrophonePosition")
		.def("setMicrophoneVelocity", &UiCure::PythonInterface::Sound::SetMicrophoneVelocity)
		.staticmethod("setMicrophoneVelocity")
		.def("setMicrophoneOrientation", &UiCure::PythonInterface::Sound::SetMicrophoneOrientation)
		.staticmethod("setMicrophoneOrientation")
	;
}



namespace UiCure
{
namespace PythonInterface
{



Graphics::Viewport::Viewport():
	mX(0),
	mY(0),
	mWidth(320),
	mHeight(180)
{
}

Graphics::Viewport::Viewport(const Viewport& pOriginal)
{
	*this = pOriginal;
}

Graphics::Viewport::~Viewport()
{
}

const Graphics::Viewport& Graphics::Viewport::operator=(const Graphics::Viewport& pOriginal)
{
	mX = pOriginal.mX;
	mY = pOriginal.mY;
	mWidth = pOriginal.mWidth;
	mHeight = pOriginal.mHeight;
	return (*this);
}

tuple Graphics::Viewport::GetClippingRectangle()
{
	return (boost::python::make_tuple(mX, mY, mWidth, mHeight));
}

void Graphics::Viewport::SetClippingRectangle(int pX, int pY, int pWidth, int pHeight)
{
	mX = pX;
	mY = pY;
	mWidth = pWidth;
	mHeight = pHeight;
}

void Graphics::Viewport::MoveCamera(const tuple& pPosition, const tuple& pOrientation)
{
	float lX = boost::python::extract<float>(pPosition[0]);
	float lY = boost::python::extract<float>(pPosition[1]);
	float lZ = boost::python::extract<float>(pPosition[2]);
	ContextManager::Get()->GetGraphicsManager()->SetCameraPosition(lX, lY, lZ);
	float lTheta = boost::python::extract<float>(pOrientation[0]);
	float lPhi = boost::python::extract<float>(pOrientation[1]);
	float lGimbal = boost::python::extract<float>(pOrientation[2]);
	ContextManager::Get()->GetGraphicsManager()->SetCameraOrientation(lTheta, lPhi, lGimbal);
}

void Graphics::Viewport::Render()
{
	ContextManager::Get()->GetGraphicsManager()->SetViewport(mX, mY, mWidth, mHeight);
	ContextManager::Get()->GetGraphicsManager()->RenderScene();
}

void Graphics::Viewport::PrintText(int pX, int pY, const char* pText)
{
	Lepra::AnsiString lString(pText);
	ContextManager::Get()->GetGraphicsManager()->PrintText(mX+pX, mY+pY, lString.ToCurrentCode());
}

Graphics::Screen::Screen():
	Viewport()
{
	++mReferenceCount;
}

Graphics::Screen::Screen(const Screen& pOriginal):
	Viewport()
{
	++mReferenceCount;
	*this = pOriginal;
}

Graphics::Screen::Screen(UiLepra::DisplayManager::ContextType pRenderingContext, const tuple& pMode, bool pIsWindowed):
	Viewport()
{
	++mReferenceCount;

	int lWidth = boost::python::extract<int>(pMode[0]);
	int lHeight = boost::python::extract<int>(pMode[1]);
	int lBpp = boost::python::extract<int>(pMode[2]);
	int lFrequency = boost::python::extract<int>(pMode[3]);
	lHeight = lHeight > 0? lHeight : lHeight = lWidth*3/4;
	SetClippingRectangle(0, 0, lWidth, lHeight);

	ContextManager::Get()->GetGraphicsManager()->Open(pRenderingContext, lWidth, lHeight, lBpp, lFrequency, pIsWindowed);
}

Graphics::Screen::~Screen()
{
	if (--mReferenceCount <= 0)
	{
		ContextManager::Get()->GetGraphicsManager()->Close();
	}
}

const Graphics::Screen& Graphics::Screen::operator=(const Graphics::Screen& pOriginal)
{
	Viewport::operator=(pOriginal);
	return (*this);
}

void Graphics::Screen::UpdateScreen()
{
	ContextManager::Get()->GetGraphicsManager()->UpdateScreen();
}

Graphics::Viewport Graphics::Screen::CreateViewport(int pX, int pY, int pWidth, int pHeight)
{
	Viewport lViewport;
	lViewport.SetClippingRectangle(pX, pY, pWidth, pHeight);
	return (lViewport);
}

int Graphics::Screen::mReferenceCount = 0;

Graphics::Screen Graphics::CreateScreen(UiLepra::DisplayManager::ContextType pRenderingContext, const tuple& pMode, bool pIsWindowed)
{
	return (Screen(pRenderingContext, pMode, pIsWindowed));
}

tuple Graphics::GetFullscreenResolutions()
{
	tuple t;
	UiLepra::DisplayManager* lDisplay = UiLepra::DisplayManager::CreateDisplayManager(UiLepra::DisplayManager::OPENGL_CONTEXT);
	for (int x = 0; x < lDisplay->GetNumDisplayModes(); ++x)
	{
		UiLepra::DisplayMode lDisplayMode;
		lDisplay->GetDisplayMode(lDisplayMode, x);
		t += boost::python::make_tuple(boost::python::make_tuple(lDisplayMode.mWidth, lDisplayMode.mHeight, lDisplayMode.mBitDepth, lDisplayMode.mRefreshRate));
	}
	delete (lDisplay);
	return (t);
}



void Sound::SetMicrophonePosition(float pX, float pY, float pZ)
{
	ContextManager::Get()->GetSoundManager()->SetMicrophonePosition(Lepra::Vector3DD(pX, pY, pZ));
}

void Sound::SetMicrophoneVelocity(float pVelocityX, float pVelocityY, float pVelocityZ)
{
	ContextManager::Get()->GetSoundManager()->SetMicrophoneVelocity(Lepra::Vector3DD(pVelocityX, pVelocityY, pVelocityZ));
}

void Sound::SetMicrophoneOrientation(float pTheta, float pPhi, float pGimbal)
{
	ContextManager::Get()->GetSoundManager()->SetMicrophoneOrientation(Lepra::Vector3DD(pTheta, pPhi, pGimbal));
}



}
}
#endif // 0
