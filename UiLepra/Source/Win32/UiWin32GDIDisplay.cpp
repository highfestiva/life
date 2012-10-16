
// Author: Jonas Byström
// Copyright (c) 2012, Pixel Doctrine



#include "../../Include/Win32/UiWin32GDIDisplay.h"



namespace UiLepra
{



Win32GDIDisplay::Win32GDIDisplay()
{
}

Win32GDIDisplay::~Win32GDIDisplay()
{
}

Win32GDIDisplay::ContextType Win32GDIDisplay::GetContextType()
{
	return OPENGL_CONTEXT;
}

bool Win32GDIDisplay::Activate()
{
	return true;
}

bool Win32GDIDisplay::UpdateScreen()
{
	return true;
}

bool Win32GDIDisplay::IsVSyncEnabled() const
{
	return false;
}

bool Win32GDIDisplay::SetVSyncEnabled(bool)
{
	return false;
}

void Win32GDIDisplay::OnResize(int, int)
{
}

void Win32GDIDisplay::OnMinimize()
{
}

void Win32GDIDisplay::OnMaximize(int, int)
{
}

bool Win32GDIDisplay::InitScreen()
{
	return true;
}

void Win32GDIDisplay::SetFocus(bool)
{
}



}
