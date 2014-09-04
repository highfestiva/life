/*
	Class:  PngLoader
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#pragma once

#include "Log.h"
#include "../../ThirdParty/lpng1212/png.h"



namespace Lepra
{



class Canvas;
class File;
class Reader;
class Writer;



class PngLoader
{
public:

	enum Status
	{
		STATUS_OPEN_ERROR = 0, 
		STATUS_SUCCESS,
		STATUS_MEMORY_ERROR,
		STATUS_READ_HEADER_ERROR,
		STATUS_READ_INFO_ERROR,
		STATUS_READ_PALETTE_ERROR,
		STATUS_READ_PICTURE_ERROR,
		STATUS_RESOLUTION_ERROR,
		STATUS_COMPRESSION_ERROR,
	};

	Status Load(const str& pFileName, Canvas& pCanvas);
	Status Save(const str& pFileName, const Canvas& pCanvas);

	Status Load(const str& pArchiveName, const str& pFileName, Canvas& pCanvas);
	Status Save(const str& pArchiveName, const str& pFileName, const Canvas& pCanvas);

	Status Load(Reader& pReader, Canvas& pCanvas);
	Status Save(Writer& pWriter, const Canvas& pCanvas);

private:
	bool CheckIfPNG();
	static void ReadDataCallback(png_structp pPNG, png_bytep pData, png_size_t pLength);
	static void WriteDataCallback(png_structp pPNG, png_bytep pData, png_size_t pLength);
	static void FlushCallback(png_structp pPNG);

	Reader* mReader;
	Writer* mWriter;
	logclass();
};



}
