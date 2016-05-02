
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/Lepra.h"
#include "../Include/Canvas.h"
#include "../Include/GammaLookup.h"
#include "../Include/HiResTimer.h"
#include "../Include/MetaFile.h"
#include "../Include/Quaternion.h"
#include "../Include/Random.h"
#include "../Include/SystemManager.h"
#include "../Include/Transformation.h"



namespace Lepra
{



const quat gIdentityQuaternionF;
const QuaternionD gIdentityQuaternionD;
const xform gIdentityTransformationF;
const TransformationD gIdentityTransformationD;



void Init()
{
	LogType::Init();

	HiResTimer::InitFrequency();
	HiResTimer::StepCounterShadow();

	SystemManager::Init();

	GammaLookup::Initialize();
	Canvas::InitSamplingGrid();

	MetaFile::AddZipExtension(".zip");

	Random::SetSeed((uint32)HiResTimer::GetSystemCounter());
}

void Shutdown()
{
	MetaFile::ClearExtensions();
	SystemManager::Shutdown();
}



}
