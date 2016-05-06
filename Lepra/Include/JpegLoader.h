/*
	Class:  JpegLoader
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

	NOTES:

	A class used to load/save jpeg-files into/from a Canvas.
*/

#pragma once

#include "lepratypes.h"
#include "string.h"

struct jpeg_decompress_struct;
struct jpeg_compress_struct;

namespace lepra {

class Canvas;
class ProgressCallback;
class Reader;
class Writer;

class JpegLoader {
public:

	friend class JpegFriend;

	enum Status {
		kStatusOpenError = 0,
		kStatusSuccess,
		kStatusMemoryError,
		kStatusReadHeaderError,
		kStatusReadInfoError,
		kStatusReadPaletteError,
		kStatusReadPictureError,
		kStatusPixelformatError,
		kStatusResolutionError,
		kStatusCompressionError,
	};

	enum {
		kIoBufferSize = 4096,
	};

	JpegLoader();
	virtual ~JpegLoader();

	Status Load(const str& file_name, Canvas& canvas, ProgressCallback* progress = 0);
	Status Save(const str& file_name, const Canvas& canvas);

	Status Load(const str& archive_name, const str& file_name, Canvas& canvas, ProgressCallback* progress = 0);
	Status Save(const str& archive_name, const str& file_name, const Canvas& canvas);

	Status Load(Reader& reader, Canvas& canvas, ProgressCallback* progress = 0);
	Status Save(Writer& writer, const Canvas& canvas);

protected:
private:

	Status Load(Canvas& canvas, ProgressCallback* progress = 0);
	Status Save(const Canvas& canvas);

	void InitSourceManager(jpeg_decompress_struct* c_info);
	void InitDestinationManager(jpeg_compress_struct* c_info);

	Reader* reader_;
	Writer* writer_;
};

}
