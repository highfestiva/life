
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/imageloader.h"
#include "../include/lepraassert.h"
#include "../include/bmploader.h"
#include "../include/diskfile.h"
#include "../include/jpegloader.h"
#include "../include/path.h"
#include "../include/pngloader.h"
#include "../include/string.h"
#include "../include/tgaloader.h"
#include "../include/tiffloader.h"



namespace lepra {



ImageLoader::ImageLoader() {
}

ImageLoader::~ImageLoader() {
}



ImageLoader::FileType ImageLoader::GetFileTypeFromName(const str& filename) {
	str file_extension = Path::GetExtension(filename);
	strutil::ToLower(file_extension);
	if (file_extension == "bmp") {
		return kBmp;
	} else if (file_extension ==  "tga") {
		return kTga;
	} else if (file_extension == "tif" || file_extension == "tiff") {
		return kTif;
	} else if (file_extension == "jpg" || file_extension == "jpeg") {
		return kJpg;
	} else if (file_extension == "png") {
		return PNG;
	}
	return kUnknown;
}



bool ImageLoader::Load(const str& file_name, Canvas& canvas) {
	DiskFile file;
	if (!file.Open(file_name, DiskFile::kModeRead)) {
		return false;
	}
	return Load(GetFileTypeFromName(file_name), file, canvas);
}

bool ImageLoader::Save(const str& file_name, const Canvas& canvas) {
	DiskFile file;
	if (!file.Open(file_name, DiskFile::kModeWrite)) {
		return false;
	}
	return Save(GetFileTypeFromName(file_name), file, canvas);
}



bool ImageLoader::Load(FileType file_type, Reader& reader, Canvas& canvas) {
	switch (file_type) {
		case kBmp: {
			BmpLoader bmp_loader;
			return bmp_loader.Load(reader, canvas) == BmpLoader::kStatusSuccess;
		} break;
		case kTga: {
			TgaLoader tga_loader;
			return tga_loader.Load(reader, canvas) == TgaLoader::kStatusSuccess;
		} break;
		case kTif: {
			TiffLoader tiff_loader;
			return tiff_loader.Load(reader, canvas) == TiffLoader::kStatusSuccess;
		} break;
		case kJpg: {
			JpegLoader jpeg_loader;
			return jpeg_loader.Load(reader, canvas) == JpegLoader::kStatusSuccess;
		} break;
		case PNG: {
			PngLoader png_loader;
			return png_loader.Load(reader, canvas) == PngLoader::kStatusSuccess;
		} break;
		default:
		return false;
	}
}

bool ImageLoader::Save(FileType file_type, Writer& writer, const Canvas& canvas) {
	switch(file_type) {
		case kBmp: {
			BmpLoader bmp_loader;
			return bmp_loader.Save(writer, canvas) == BmpLoader::kStatusSuccess;
		} break;
		case kTga: {
			TgaLoader tga_loader;
			return tga_loader.Save(writer, canvas) == TgaLoader::kStatusSuccess;
		} break;
		case kTif: {
			TiffLoader tiff_loader;
			return tiff_loader.Save(writer, canvas) == TiffLoader::kStatusSuccess;
		} break;
		case kJpg: {
			JpegLoader jpeg_loader;
			return jpeg_loader.Save(writer, canvas) == JpegLoader::kStatusSuccess;
		} break;
		case PNG: {
			PngLoader png_loader;
			return png_loader.Save(writer, canvas) == PngLoader::kStatusSuccess;
		} break;
	}
	return false;
}



}
