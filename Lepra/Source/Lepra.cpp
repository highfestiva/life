
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/Lepra.h"
#include "../Include/Canvas.h"
#include "../Include/GammaLookup.h"
#include "../Include/HiResTimer.h"
#include "../Include/MetaFile.h"
#include "../Include/Random.h"
#include "../Include/SystemManager.h"



namespace Lepra
{



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
