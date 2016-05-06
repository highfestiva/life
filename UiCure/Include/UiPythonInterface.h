
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



#pragma once

#if 0
#pragma warning(push, 3)	// MSVC: Move to warning level 3.
#pragma warning(disable: 4244)	// MSVC: Return: conversion from <x> to <y>, possible loss of data.
#pragma warning(disable: 4267)	// MSVC: Argument: conversion from <x> to <y>, possible loss of data.
#pragma warning(disable: 4541)	// MSVC: <runtime> used on polymorphic type 'Cure::PythonContextManager' with /GR-; unpredictable behavior may result
#include <boost/python.hpp>
#pragma warning(pop)
#include "../../uilepra/include/uidisplaymanager.h"



namespace UiCure {
namespace pythoninterface {



typedef boost::python::tuple tuple;



class Graphics {
public:
	class Screen;
	class Viewport {
	public:
		Viewport();
		Viewport(const Viewport& original);
		virtual ~Viewport();
		const Viewport& operator=(const Viewport& original);
		tuple GetClippingRectangle();
		void SetClippingRectangle(int x, int y, int width, int height);
		void MoveCamera(const tuple& position, const tuple& orientation);
		void Render();
		void PrintText(int x, int y, const char* text);

	private:
		int x_;
		int y_;
		int width_;
		int height_;
	};
	class Screen: public Viewport {
	public:
		Screen();
		Screen(const Screen& original);
		Screen(uilepra::DisplayManager::ContextType rendering_context, const tuple& mode, bool is_windowed);
		virtual ~Screen();
		const Screen& operator=(const Screen& original);
		void UpdateScreen();
		Viewport CreateViewport(int x, int y, int width, int height);

	private:
		static int reference_count_;
	};
	static Screen CreateScreen(uilepra::DisplayManager::ContextType rendering_context, const tuple& mode, bool is_windowed);
	static tuple GetFullscreenResolutions();
};

class Sound {
public:
	static void SetMicrophonePosition(float x, float y, float z);
	static void SetMicrophoneVelocity(float velocity_x, float velocity_y, float velocity_z);
	static void SetMicrophoneOrientation(float theta, float phi, float gimbal);
};



}
}
#endif // 0
