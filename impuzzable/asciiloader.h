
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../cure/include/resourcemanager.h"
#include "impuzzable.h"
#include "pieceinfo.h"



namespace Impuzzable {



class AsciiLoader {
public:
	AsciiLoader(cure::ResourceManager* resource_manager);
	virtual ~AsciiLoader();

	virtual wstr Load(const str& filename) const;
	bool ConvertPieces(const wstr& ascii, std::vector<PieceInfo>& pieces) const;

private:
	cure::ResourceManager* resource_manager_;
	logclass();
};



}
