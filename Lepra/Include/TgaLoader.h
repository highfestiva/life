/*
	Class:  TgaLoader
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games

	NOTES:

	A class used to load/save tga-files into/from a Canvas.
*/

#pragma once

#include "String.h"



namespace Lepra
{



class File;
class Canvas;
class Color;
class Reader;
class Writer;



class TgaLoader
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
		STATUS_UNKNOWNFILESIZE_ERROR,
		STATUS_READSTREAM_ERROR,
	};

	Status Load(const str& pFileName, Canvas& pCanvas);
	Status Save(const str& pFileName, const Canvas& pCanvas);

	Status Load(const str& pArchiveName, const str& pFileName, Canvas& pCanvas);
	Status Save(const str& pArchiveName, const str& pFileName, const Canvas& pCanvas);

	Status Load(Reader& pReader, Canvas& pCanvas);
	Status Save(Writer& pWriter, const Canvas& pCanvas);

private:

	enum TGAFormat
	{
		ORIGINAL_TGA_FORMAT = 0,
		NEW_TGA_FORMAT,
	};

	enum ImageType
	{
		NO_IMAGE_MAP				= 0,
		UNCOMPRESSED_COLORMAP_IMAGE		= 1,
		UNCOMPRESSED_TRUECOLOR_IMAGE		= 2,
		UNCOMPRESSED_BLACKANDWHITE_IMAGE	= 3,
		RLE_COLORMAPPED_IMAGE			= 9,
		RLE_TRUECOLOR_IMAGE			= 10,
		RLE_BLACKANDWHITE_IMAGE			= 11,
	};

	struct ColorMapSpecification
	{
		uint16 mFirstEntryIndex;
		uint16 mColorMapLength;
		uint8  mColorMapEntrySize; // 15, 16, 24 or 32-bit.
	};

	struct ImageSpecification
	{
		uint16 mXOrigin;
		uint16 mYOrigin;
		uint16 mImageWidth;
		uint16 mImageHeight;
		uint8  mPixelDepth; // Number of bits per pixel. 8, 16, 24 or 32 are common.
		uint8  mImageDescriptor; // Flags.
	};

	struct TGAFileHeader
	{
		uint8 mIDLength;
		uint8 mColorMapType; // Boolean value, 1 or 0, true or false. ColorMap = Palette.
		uint8 mImageType;
		ColorMapSpecification mColorMapSpec;
		ImageSpecification mImageSpec;
	};

	// 26 byte file footer.
	struct FileFooter
	{
		unsigned mExtensionAreaOffset;
		unsigned mDeveloperDirectoryOffset;
		char mSignature[18]; // "TRUEVISION-XFILE." with a terminating null-character.
	};

	Status Load(File& pFile, Canvas& pCanvas);

	Status LoadUncompressedColorMapImage(Canvas& pCanvas, TGAFileHeader& pFileHeader, File& pFile, Color* pPalette);
	Status LoadUncompressedTrueColorImage(Canvas& pCanvas, TGAFileHeader& pFileHeader, File& pFile);
	Status LoadUncompressedBlackAndWhiteImage(Canvas& pCanvas, TGAFileHeader& pFileHeader, File& pFile);
	Status LoadRLEColorMapImage(Canvas& pCanvas, TGAFileHeader& pFileHeader, File& pFile, Color* pPalette);
	Status LoadRLETrueColorImage(Canvas& pCanvas, TGAFileHeader& pFileHeader, File& pFile);
	Status LoadRLEBlackAndWhiteImage(Canvas& pCanvas, TGAFileHeader& pFileHeader, File& pFile);

	static const char* smTruevisionXFile;
};



}
