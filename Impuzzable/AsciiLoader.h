
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Cure/Include/ResourceManager.h"
#include "Impuzzable.h"
#include "PieceInfo.h"



namespace Impuzzable
{



class AsciiLoader
{
public:
	AsciiLoader(Cure::ResourceManager* pResourceManager);
	virtual ~AsciiLoader();

	virtual wstr Load(const str& pFilename) const;
	bool ConvertPieces(const wstr& pAscii, std::vector<PieceInfo>& pPieces) const;

private:
	Cure::ResourceManager* mResourceManager;
	logclass();
};



}
