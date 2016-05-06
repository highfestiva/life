/*
	Class:  TgaLoader
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

	NOTES:

	A class used to load/save tga-files into/from a Canvas.
*/

#pragma once

#include "string.h"



namespace lepra {



class File;
class Canvas;
class Color;
class Reader;
class Writer;



class TgaLoader {
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
		kStatusUnknownfilesizeError,
		kStatusReadstreamError,
	};

	Status Load(const str& file_name, Canvas& canvas);
	Status Save(const str& file_name, const Canvas& canvas);

	Status Load(const str& archive_name, const str& file_name, Canvas& canvas);
	Status Save(const str& archive_name, const str& file_name, const Canvas& canvas);

	Status Load(Reader& reader, Canvas& canvas);
	Status Save(Writer& writer, const Canvas& canvas);

private:

	enum TGAFormat {
		kOriginalTgaFormat = 0,
		kNewTgaFormat,
	};

	enum ImageType {
		kNoImageMap				= 0,
		kUncompressedColormapImage		= 1,
		kUncompressedTruecolorImage		= 2,
		kUncompressedBlackandwhiteImage	= 3,
		kRleColormappedImage			= 9,
		kRleTruecolorImage			= 10,
		kRleBlackandwhiteImage			= 11,
	};

	struct ColorMapSpecification {
		uint16 first_entry_index_;
		uint16 color_map_length_;
		uint8  color_map_entry_size_; // 15, 16, 24 or 32-bit.
	};

	struct ImageSpecification {
		uint16 x_origin_;
		uint16 y_origin_;
		uint16 image_width_;
		uint16 image_height_;
		uint8  pixel_depth_; // Number of bits per pixel. 8, 16, 24 or 32 are common.
		uint8  image_descriptor_; // Flags.
	};

	struct TGAFileHeader {
		uint8 id_length_;
		uint8 color_map_type_; // Boolean value, 1 or 0, true or false. ColorMap = Palette.
		uint8 image_type_;
		ColorMapSpecification color_map_spec_;
		ImageSpecification image_spec_;
	};

	// 26 byte file footer.
	struct FileFooter {
		unsigned extension_area_offset_;
		unsigned developer_directory_offset_;
		char signature_[18]; // "TRUEVISION-XFILE." with a terminating null-character.
	};

	Status Load(File& file, Canvas& canvas);

	Status LoadUncompressedColorMapImage(Canvas& canvas, TGAFileHeader& file_header, File& file, Color* palette);
	Status LoadUncompressedTrueColorImage(Canvas& canvas, TGAFileHeader& file_header, File& file);
	Status LoadUncompressedBlackAndWhiteImage(Canvas& canvas, TGAFileHeader& file_header, File& file);
	Status LoadRLEColorMapImage(Canvas& canvas, TGAFileHeader& file_header, File& file, Color* palette);
	Status LoadRLETrueColorImage(Canvas& canvas, TGAFileHeader& file_header, File& file);
	Status LoadRLEBlackAndWhiteImage(Canvas& canvas, TGAFileHeader& file_header, File& file);

	static const char* truevision_x_file_;
};



}
