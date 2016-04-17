
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
#include "../../UiLepra/Include/UiDisplayManager.h"



namespace UiCure
{
namespace PythonInterface
{



typedef boost::python::tuple tuple;



class Graphics
{
public:
	class Screen;
	class Viewport
	{
	public:
		Viewport();
		Viewport(const Viewport& pOriginal);
		virtual ~Viewport();
		const Viewport& operator=(const Viewport& pOriginal);
		tuple GetClippingRectangle();
		void SetClippingRectangle(int pX, int pY, int pWidth, int pHeight);
		void MoveCamera(const tuple& pPosition, const tuple& pOrientation);
		void Render();
		void PrintText(int pX, int pY, const char* pText);

	private:
		int mX;
		int mY;
		int mWidth;
		int mHeight;
	};
	class Screen: public Viewport
	{
	public:
		Screen();
		Screen(const Screen& pOriginal);
		Screen(UiLepra::DisplayManager::ContextType pRenderingContext, const tuple& pMode, bool pIsWindowed);
		virtual ~Screen();
		const Screen& operator=(const Screen& pOriginal);
		void UpdateScreen();
		Viewport CreateViewport(int pX, int pY, int pWidth, int pHeight);

	private:
		static int mReferenceCount;
	};
	static Screen CreateScreen(UiLepra::DisplayManager::ContextType pRenderingContext, const tuple& pMode, bool pIsWindowed);
	static tuple GetFullscreenResolutions();
};

class Sound
{
public:
	static void SetMicrophonePosition(float pX, float pY, float pZ);
	static void SetMicrophoneVelocity(float pVelocityX, float pVelocityY, float pVelocityZ);
	static void SetMicrophoneOrientation(float pTheta, float pPhi, float pGimbal);
};



}
}
#endif // 0
