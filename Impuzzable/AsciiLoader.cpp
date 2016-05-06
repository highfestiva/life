
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "asciiloader.h"



namespace Impuzzable {



AsciiLoader::AsciiLoader(cure::ResourceManager* resource_manager):
	resource_manager_(resource_manager) {
}

AsciiLoader::~AsciiLoader() {
}

wstr AsciiLoader::Load(const str& filename) const {
	File* f = resource_manager_->QueryFile(filename);
	if (!f) {
		log_.Errorf("No such file %s present", filename.c_str());
		return wstr();
	}
	wstr s;
	wstrutil::strvec lines;
	while (f->ReadLine(s) == kIoOk) {
		lines.push_back(s);
	}
	return wstrutil::Join(lines, L"\n");
}



bool AsciiLoader::ConvertPieces(const wstr& ascii, std::vector<PieceInfo>& pieces) const {
	(void)ascii;
	(void)pieces;
	return true;
}



loginstance(kGameContextCpp, AsciiLoader);



}
