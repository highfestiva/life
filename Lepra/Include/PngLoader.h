/*
	Class:  PngLoader
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games
*/

#ifndef PNGLOADER_H
#define PNGLOADER_H

#include "LepraTypes.h"
#include "DiskFile.h"
#include "../../ThirdParty/lpng1212/png.h"

namespace Lepra
{

class Canvas;

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

	Status Load(const String& pFileName, Canvas& pCanvas);
	Status Save(const String& pFileName, const Canvas& pCanvas);

	Status Load(const String& pArchiveName, const String& pFileName, Canvas& pCanvas);
	Status Save(const String& pArchiveName, const String& pFileName, const Canvas& pCanvas);

	Status Load(Reader& pReader, Canvas& pCanvas);
	Status Save(Writer& pWriter, const Canvas& pCanvas);

private:

	bool CheckIfPNG();
	static void ReadDataCallback(png_structp pPNG, png_bytep pData, png_size_t pLength);
	static void WriteDataCallback(png_structp pPNG, png_bytep pData, png_size_t pLength);
	static void FlushCallback(png_structp pPNG);

	Reader* mReader;
	Writer* mWriter;
};


} // End namespace.

#endif

