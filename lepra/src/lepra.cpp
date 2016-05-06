
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/lepra.h"
#include "../include/canvas.h"
#include "../include/gammalookup.h"
#include "../include/hirestimer.h"
#include "../include/metafile.h"
#include "../include/quaternion.h"
#include "../include/random.h"
#include "../include/systemmanager.h"
#include "../include/transformation.h"



namespace lepra {



const quat kIdentityQuaternionF;
const QuaternionD kIdentityQuaternionD;
const xform kIdentityTransformationF;
const TransformationD kIdentityTransformationD;



void Init() {
	LogType::Init();

	HiResTimer::InitFrequency();
	HiResTimer::StepCounterShadow();

	SystemManager::Init();

	GammaLookup::Initialize();
	Canvas::InitSamplingGrid();

	MetaFile::AddZipExtension(".zip");

	Random::SetSeed((uint32)HiResTimer::GetSystemCounter());
}

void Shutdown() {
	MetaFile::ClearExtensions();
	SystemManager::Shutdown();
}



}
