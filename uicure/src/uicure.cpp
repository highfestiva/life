
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../cure/include/cure.h"
#include "../../cure/include/runtimevariable.h"
#include "../include/uicure.h"
#include "../include/uigameuimanager.h"
#include "../include/uiruntimevariablename.h"



namespace UiCure {



void Init() {
	cure::Init();
	UiCure::SetDefault(GetSettings());
}

void Shutdown() {
	cure::Shutdown();
}

void SetDefault(cure::RuntimeVariableScope* settings) {
	v_set(settings, kRtvarCtrlEmulatetouch, false);
	v_internal(settings, kRtvarCtrlAccelerometerX,  0.0);
	v_internal(settings, kRtvarCtrlAccelerometerY, +1.0);
	v_internal(settings, kRtvarCtrlAccelerometerZ,  0.0);
	v_set(settings, kRtvarCtrlDisableWinMgr, true);
	v_set(settings, kRtvarDebug3DEnableaxes, false);
	v_set(settings, kRtvarDebug3DEnablejoints, false);
	v_set(settings, kRtvarDebug3DEnableshapes, true);
	v_set(settings, kRtvarUiDisplayWidth, 640);
	v_set(settings, kRtvarUiDisplayHeight, 480);
	v_set(settings, kRtvarUiDisplayBitsperpixel, 0);
	v_set(settings, kRtvarUiDisplayFrequency, 0);
	v_set(settings, kRtvarUiDisplayFullscreen, false);
	v_set(settings, kRtvarUiDisplayOrientation, "AllowUpsideDown");
	v_set(settings, kRtvarUiDisplayEnablevsync, true);
	v_set(settings, kRtvarUiDisplayRenderengine, "OpenGL");
	v_set(settings, kRtvarUi3DEnablelights, true);
	v_set(settings, kRtvarUi3DEnableclear, true);
	v_set(settings, kRtvarUi3DEnableautoperformance, true);
	v_set(settings, kRtvarUi3DEnablemassobjects, true);
	v_set(settings, kRtvarUi3DEnablemassobjectfading, true);
	v_set(settings, kRtvarUi3DEnableparticles, true);
	v_set(settings, kRtvarUi3DExhaustintensity, 1.0);
	v_set(settings, kRtvarUi3DOutlinemode, false);
	v_set(settings, kRtvarUi3DWireframemode, false);
	v_set(settings, kRtvarUi3DPixelshaders, true);
	v_set(settings, kRtvarUi3DLinewidth, 3.0);
	v_set(settings, kRtvarUi3DClearred, 0.75);
	v_set(settings, kRtvarUi3DCleargreen, 0.80);
	v_set(settings, kRtvarUi3DClearblue, 0.85);
	v_set(settings, kRtvarUi3DAmbientred, 0.3);
	v_set(settings, kRtvarUi3DAmbientgreen, 0.3);
	v_set(settings, kRtvarUi3DAmbientblue, 0.3);
	v_internal(settings, kRtvarUi3DAmbientredfactor, 1.0);
	v_internal(settings, kRtvarUi3DAmbientgreenfactor, 1.0);
	v_internal(settings, kRtvarUi3DAmbientbluefactor, 1.0);
	v_set(settings, kRtvarUi3DEnabletrilinearfiltering, false);
	v_set(settings, kRtvarUi3DEnablebilinearfiltering, false);
	v_set(settings, kRtvarUi3DEnablemipmapping, true);
	v_set(settings, kRtvarUi3DEnabletexturing, true);
	v_set(settings, kRtvarUi3DFognear, 0.0);
	v_set(settings, kRtvarUi3DFogfar, 0.0);
	v_set(settings, kRtvarUi3DFogdensity, 1.0);
	v_set(settings, kRtvarUi3DFogexponent, 0.0);
	v_set(settings, kRtvarUi3DFov, 45.0);
	v_set(settings, kRtvarUi3DClipnear, 0.1);
	v_set(settings, kRtvarUi3DClipfar, 3000.0);
	v_set(settings, kRtvarUi3DShadows, "Volumes");
	v_set(settings, kRtvarUi3DShadowdeviation, 1e-5);
	v_set(settings, kRtvarUi2DPaintmode, "Normal");
	v_set(settings, kRtvarUi2DFont, "Times New Roman");
	v_set(settings, kRtvarUi2DFontheight, 14.0);
	v_set(settings, kRtvarUi2DFontflags, 0);
	v_set(settings, kRtvarUiSoundEngine, "OpenAL");
	v_set(settings, kRtvarUiSoundRolloff, 0.2);
	v_set(settings, kRtvarUiSoundDoppler, 1.3);
	v_set(settings, kRtvarUiSoundMastervolume, 1.0);
	v_set(settings, kRtvarUiSoundMusicvolume, 1.0);
}

cure::RuntimeVariableScope* GetSettings() {
	return (cure::GetSettings());
}



}
