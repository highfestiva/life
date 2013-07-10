
// Author: Jonas Bystr�m
// Copyright (c) Pixel Doctrine



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



const QuaternionF gIdentityQuaternionF;
const QuaternionD gIdentityQuaternionD;
const TransformationF gIdentityTransformationF;
const TransformationD gIdentityTransformationD;



void Init()
{
	LogType::Init();

	HiResTimer::InitFrequency();

	SystemManager::Init();

	GammaLookup::Initialize();
	Canvas::InitSamplingGrid();

	MetaFile::AddZipExtension(_T(".zip"));

	Random::SetSeed((uint32)HiResTimer::GetSystemCounter());
}

void Shutdown()
{
	MetaFile::ClearExtensions();
	SystemManager::Shutdown();
}



}
