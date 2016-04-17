/*
	Class:  JpegLoader
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

	NOTES:

	A class used to load/save jpeg-files into/from a Canvas.
*/

#pragma once

#include "LepraTypes.h"
#include "String.h"

struct jpeg_decompress_struct;
struct jpeg_compress_struct;

namespace Lepra
{

class Canvas;
class ProgressCallback;
class Reader;
class Writer;

class JpegLoader
{
public:

	friend class JpegFriend;

	enum Status
	{
		STATUS_OPEN_ERROR = 0, 
		STATUS_SUCCESS,
		STATUS_MEMORY_ERROR,
		STATUS_READ_HEADER_ERROR,
		STATUS_READ_INFO_ERROR,
		STATUS_READ_PALETTE_ERROR,
		STATUS_READ_PICTURE_ERROR,
		STATUS_PIXELFORMAT_ERROR,
		STATUS_RESOLUTION_ERROR,
		STATUS_COMPRESSION_ERROR,
	};

	enum
	{
		IO_BUFFER_SIZE = 4096,
	};

	JpegLoader();
	virtual ~JpegLoader();

	Status Load(const str& pFileName, Canvas& pCanvas, ProgressCallback* pProgress = 0);
	Status Save(const str& pFileName, const Canvas& pCanvas);

	Status Load(const str& pArchiveName, const str& pFileName, Canvas& pCanvas, ProgressCallback* pProgress = 0);
	Status Save(const str& pArchiveName, const str& pFileName, const Canvas& pCanvas);
	
	Status Load(Reader& pReader, Canvas& pCanvas, ProgressCallback* pProgress = 0);
	Status Save(Writer& pWriter, const Canvas& pCanvas);

protected:
private:

	Status Load(Canvas& pCanvas, ProgressCallback* pProgress = 0);
	Status Save(const Canvas& pCanvas);

	void InitSourceManager(jpeg_decompress_struct* pCInfo);
	void InitDestinationManager(jpeg_compress_struct* pCInfo);

	Reader* mReader;
	Writer* mWriter;
};

}
