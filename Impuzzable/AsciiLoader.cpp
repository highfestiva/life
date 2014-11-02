
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "AsciiLoader.h"



namespace Impuzzable
{



AsciiLoader::AsciiLoader(Cure::ResourceManager* pResourceManager):
	mResourceManager(pResourceManager)
{
}

AsciiLoader::~AsciiLoader()
{
}

wstr AsciiLoader::Load(const str& pFilename) const
{
	File* f = mResourceManager->QueryFile(pFilename);
	if (!f)
	{
		mLog.Errorf(_T("No such file %s present"), pFilename.c_str());
		return wstr();
	}
	wstr s;
	wstrutil::strvec lLines;
	while (f->ReadLine(s) == IO_OK)
	{
		lLines.push_back(s);
	}
	return wstrutil::Join(lLines, L"\n");
}



bool AsciiLoader::ConvertPieces(const wstr& pAscii, std::vector<PieceInfo>& pPieces) const
{
	(void)pAscii;
	(void)pPieces;
	return true;
}



loginstance(GAME_CONTEXT_CPP, AsciiLoader);



}
