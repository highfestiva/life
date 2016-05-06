/*
	Class:  PngLoader
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#pragma once

#include "log.h"
#include "../../thirdparty/lpng1212/png.h"



namespace lepra {



class Canvas;
class File;
class Reader;
class Writer;



class PngLoader {
public:

	enum Status {
		kStatusOpenError = 0,
		kStatusSuccess,
		kStatusMemoryError,
		kStatusReadHeaderError,
		kStatusReadInfoError,
		kStatusReadPaletteError,
		kStatusReadPictureError,
		kStatusResolutionError,
		kStatusCompressionError,
	};

	Status Load(const str& file_name, Canvas& canvas);
	Status Save(const str& file_name, const Canvas& canvas);

	Status Load(const str& archive_name, const str& file_name, Canvas& canvas);
	Status Save(const str& archive_name, const str& file_name, const Canvas& canvas);

	Status Load(Reader& reader, Canvas& canvas);
	Status Save(Writer& writer, const Canvas& canvas);

private:
	bool CheckIfPNG();
	static void ReadDataCallback(png_structp png, png_bytep data, png_size_t length);
	static void WriteDataCallback(png_structp png, png_bytep data, png_size_t length);
	static void FlushCallback(png_structp png);

	Reader* reader_;
	Writer* writer_;
	logclass();
};



}
