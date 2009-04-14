
// Author: Alexander Hugestrand
// Copyright (c) 2002-2008, Righteous Games



#include "../Include/Canvas.h"
#include "../Include/GammaLookup.h"
#include "../Include/Lepra.h"
#include "../Include/MetaFile.h"
#include "../Include/SystemManager.h"



namespace Lepra
{



void Init()
{
	LogType::Init();

	SystemManager::Init();

	GammaLookup::Initialize();
	Canvas::InitSamplingGrid();

	MetaFile::AddZipExtension(_T(".zip"));
}

void Shutdown()
{
	MetaFile::ClearExtensions();
	SystemManager::Shutdown();
}



}
