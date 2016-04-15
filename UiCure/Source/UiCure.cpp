
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../Cure/Include/Cure.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../Include/UiCure.h"
#include "../Include/UiGameUiManager.h"
#include "../Include/UiRuntimeVariableName.h"



namespace UiCure
{



void Init()
{
	Cure::Init();
	UiCure::SetDefault(GetSettings());
}

void Shutdown()
{
	Cure::Shutdown();
}

void SetDefault(Cure::RuntimeVariableScope* pSettings)
{
	v_set(pSettings, RTVAR_CTRL_EMULATETOUCH, false);
	v_internal(pSettings, RTVAR_CTRL_ACCELEROMETER_X,  0.0);
	v_internal(pSettings, RTVAR_CTRL_ACCELEROMETER_Y, +1.0);
	v_internal(pSettings, RTVAR_CTRL_ACCELEROMETER_Z,  0.0);
	v_set(pSettings, RTVAR_DEBUG_3D_ENABLEAXES, false);
	v_set(pSettings, RTVAR_DEBUG_3D_ENABLEJOINTS, false);
	v_set(pSettings, RTVAR_DEBUG_3D_ENABLESHAPES, true);
	v_set(pSettings, RTVAR_UI_DISPLAY_WIDTH, 640);
	v_set(pSettings, RTVAR_UI_DISPLAY_HEIGHT, 480);
	v_set(pSettings, RTVAR_UI_DISPLAY_BITSPERPIXEL, 0);
	v_set(pSettings, RTVAR_UI_DISPLAY_FREQUENCY, 0);
	v_set(pSettings, RTVAR_UI_DISPLAY_FULLSCREEN, false);
	v_set(pSettings, RTVAR_UI_DISPLAY_ORIENTATION, "AllowUpsideDown");
	v_set(pSettings, RTVAR_UI_DISPLAY_ENABLEVSYNC, true);
	v_set(pSettings, RTVAR_UI_DISPLAY_RENDERENGINE, "OpenGL");
	v_set(pSettings, RTVAR_UI_3D_ENABLELIGHTS, true);
	v_set(pSettings, RTVAR_UI_3D_ENABLECLEAR, true);
	v_set(pSettings, RTVAR_UI_3D_ENABLEAUTOPERFORMANCE, true);
	v_set(pSettings, RTVAR_UI_3D_ENABLEMASSOBJECTS, true);
	v_set(pSettings, RTVAR_UI_3D_ENABLEMASSOBJECTFADING, true);
	v_set(pSettings, RTVAR_UI_3D_ENABLEPARTICLES, true);
	v_set(pSettings, RTVAR_UI_3D_EXHAUSTINTENSITY, 1.0);
	v_set(pSettings, RTVAR_UI_3D_OUTLINEMODE, false);
	v_set(pSettings, RTVAR_UI_3D_WIREFRAMEMODE, false);
	v_set(pSettings, RTVAR_UI_3D_PIXELSHADERS, true);
	v_set(pSettings, RTVAR_UI_3D_LINEWIDTH, 3.0);
	v_set(pSettings, RTVAR_UI_3D_CLEARRED, 0.75);
	v_set(pSettings, RTVAR_UI_3D_CLEARGREEN, 0.80);
	v_set(pSettings, RTVAR_UI_3D_CLEARBLUE, 0.85);
	v_set(pSettings, RTVAR_UI_3D_AMBIENTRED, 0.3);
	v_set(pSettings, RTVAR_UI_3D_AMBIENTGREEN, 0.3);
	v_set(pSettings, RTVAR_UI_3D_AMBIENTBLUE, 0.3);
	v_internal(pSettings, RTVAR_UI_3D_AMBIENTREDFACTOR, 1.0);
	v_internal(pSettings, RTVAR_UI_3D_AMBIENTGREENFACTOR, 1.0);
	v_internal(pSettings, RTVAR_UI_3D_AMBIENTBLUEFACTOR, 1.0);
	v_set(pSettings, RTVAR_UI_3D_ENABLETRILINEARFILTERING, false);
	v_set(pSettings, RTVAR_UI_3D_ENABLEBILINEARFILTERING, false);
	v_set(pSettings, RTVAR_UI_3D_ENABLEMIPMAPPING, true);
	v_set(pSettings, RTVAR_UI_3D_ENABLETEXTURING, true);
	v_set(pSettings, RTVAR_UI_3D_FOGNEAR, 0.0);
	v_set(pSettings, RTVAR_UI_3D_FOGFAR, 0.0);
	v_set(pSettings, RTVAR_UI_3D_FOGDENSITY, 1.0);
	v_set(pSettings, RTVAR_UI_3D_FOGEXPONENT, 0.0);
	v_set(pSettings, RTVAR_UI_3D_FOV, 45.0);
	v_set(pSettings, RTVAR_UI_3D_CLIPNEAR, 0.1);
	v_set(pSettings, RTVAR_UI_3D_CLIPFAR, 3000.0);
	v_set(pSettings, RTVAR_UI_3D_SHADOWS, "Volumes");
	v_set(pSettings, RTVAR_UI_3D_SHADOWDEVIATION, 0.0);
	v_set(pSettings, RTVAR_UI_2D_PAINTMODE, "Normal");
	v_set(pSettings, RTVAR_UI_2D_FONT, "Times New Roman");
	v_set(pSettings, RTVAR_UI_2D_FONTHEIGHT, 14.0);
	v_set(pSettings, RTVAR_UI_2D_FONTFLAGS, 0);
	v_set(pSettings, RTVAR_UI_SOUND_ENGINE, "OpenAL");
	v_set(pSettings, RTVAR_UI_SOUND_ROLLOFF, 0.2);
	v_set(pSettings, RTVAR_UI_SOUND_DOPPLER, 1.3);
	v_set(pSettings, RTVAR_UI_SOUND_MASTERVOLUME, 1.0);
	v_set(pSettings, RTVAR_UI_SOUND_MUSICVOLUME, 1.0);
}

Cure::RuntimeVariableScope* GetSettings()
{
	return (Cure::GetSettings());
}



}
