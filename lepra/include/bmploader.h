/*
	Class:  BmpLoader
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

	NOTES:

	This is an incomplete loader, and can only load uncompressed
	bmp-files using common pixel formats (8-, 15-, 16-, 24- and 32-bit).
*/

#pragma once

#include "lepratypes.h"
#include "string.h"



namespace lepra {



class Canvas;
class Reader;
class Writer;



class BmpLoader {
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

	enum Compression {
		kCompBiRgb		= 0,
		kCompBiRle8,
		kCompBiRle4,
		kCompBiBitfields,
	};

	Status Load(const str& file_name, Canvas& canvas);
	Status Save(const str& file_name, const Canvas& canvas);

	Status Load(const str& archive_name, const str& file_name, Canvas& canvas);
	Status Save(const str& archive_name, const str& file_name, const Canvas& canvas);

	Status Load(Reader& reader, Canvas& canvas);
	Status Save(Writer& writer, const Canvas& canvas);

private:

	class BitmapFileHeader {
	public:
		uint16	type_;
		unsigned	size_;
		uint16	reserved1_;
		uint16	reserved2_;
		unsigned	off_bits_;

		int GetSize();

		bool Load(Reader* reader);
		bool Save(Writer* writer);
	};

	class BitmapInfoHeader {
	public:
		int	size_;
		int	width_;
		int	height_;
		int16	planes_;
		int16	bit_count_;
		int	compression_;
		int	size_image_;
		int	x_pels_per_meter_;
		int	y_pels_per_meter_;
		int	clr_used_;
		int	clr_important_;

		int GetSize();

		bool Load(Reader* reader);
		bool Save(Writer* writer);
	};

};



}
