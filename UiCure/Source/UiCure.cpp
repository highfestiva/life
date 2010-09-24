
// Author: Jonas Bystr�m
// Copyright (c) 2002-2009, Righteous Games



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
	CURE_RTVAR_SET(pSettings, RTVAR_UI_DISPLAY_WIDTH, 640);
	CURE_RTVAR_SET(pSettings, RTVAR_UI_DISPLAY_HEIGHT, 480);
	CURE_RTVAR_SET(pSettings, RTVAR_UI_DISPLAY_BITSPERPIXEL, 0);
	CURE_RTVAR_SET(pSettings, RTVAR_UI_DISPLAY_FREQUENCY, 0);
	CURE_RTVAR_SET(pSettings, RTVAR_UI_DISPLAY_FULLSCREEN, false);
	CURE_RTVAR_SET(pSettings, RTVAR_UI_DISPLAY_ENABLEVSYNC, true);
	CURE_RTVAR_SET(pSettings, RTVAR_UI_3D_RENDERENGINE, _T("OpenGL"));
	CURE_RTVAR_SET(pSettings, RTVAR_UI_3D_ENABLELIGHTS, true);
	CURE_RTVAR_SET(pSettings, RTVAR_UI_3D_ENABLECLEAR, true);
	CURE_RTVAR_SET(pSettings, RTVAR_UI_3D_ENABLEMASSOBJECTS, true);
	CURE_RTVAR_SET(pSettings, RTVAR_UI_3D_OUTLINEMODE, true);
	CURE_RTVAR_SET(pSettings, RTVAR_UI_3D_WIREFRAMEMODE, false);
	CURE_RTVAR_SET(pSettings, RTVAR_UI_3D_PIXELSHADERS, true);
	CURE_RTVAR_SET(pSettings, RTVAR_UI_3D_CLEARRED, 0.75);
	CURE_RTVAR_SET(pSettings, RTVAR_UI_3D_CLEARGREEN, 0.80);
	CURE_RTVAR_SET(pSettings, RTVAR_UI_3D_CLEARBLUE, 0.85);
	CURE_RTVAR_SET(pSettings, RTVAR_UI_3D_AMBIENTRED, 0.1);
	CURE_RTVAR_SET(pSettings, RTVAR_UI_3D_AMBIENTGREEN, 0.1);
	CURE_RTVAR_SET(pSettings, RTVAR_UI_3D_AMBIENTBLUE, 0.1);
	CURE_RTVAR_SET(pSettings, RTVAR_UI_3D_ENABLETRILINEARFILTERING, false);
	CURE_RTVAR_SET(pSettings, RTVAR_UI_3D_ENABLEBILINEARFILTERING, false);
	CURE_RTVAR_SET(pSettings, RTVAR_UI_3D_ENABLEMIPMAPPING, true);
	CURE_RTVAR_SET(pSettings, RTVAR_UI_3D_FOV, 45.0);
	CURE_RTVAR_SET(pSettings, RTVAR_UI_3D_CLIPNEAR, 0.1);
	CURE_RTVAR_SET(pSettings, RTVAR_UI_3D_CLIPFAR, 2000.0);
	CURE_RTVAR_SET(pSettings, RTVAR_UI_3D_SHADOWS, _T("VolumesOnly"));
	CURE_RTVAR_SET(pSettings, RTVAR_UI_2D_PAINTMODE, _T("AlphaBlend"));
	CURE_RTVAR_SET(pSettings, RTVAR_UI_2D_FONT, _T("Times New Roman"));
	CURE_RTVAR_SET(pSettings, RTVAR_UI_2D_FONTHEIGHT, 14.0);
	CURE_RTVAR_SET(pSettings, RTVAR_UI_SOUND_ENGINE, _T("OpenAL"));
	CURE_RTVAR_SET(pSettings, RTVAR_UI_SOUND_ROLLOFF, 0.2);
	CURE_RTVAR_SET(pSettings, RTVAR_UI_SOUND_DOPPLER, 1.3);
	CURE_RTVAR_SET(pSettings, RTVAR_UI_SOUND_VOLUME, 1.0);
}

Cure::RuntimeVariableScope* GetSettings()
{
	return (Cure::GetSettings());
}



}
