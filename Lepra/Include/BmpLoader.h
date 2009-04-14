/*
	Class:  BmpLoader
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games

	NOTES:

	This is an incomplete loader, and can only load uncompressed
	bmp-files using common pixel formats (8-, 15-, 16-, 24- and 32-bit).
*/

#ifndef LEPRA_BMPLOADER_H
#define LEPRA_BMPLOADER_H

#include "LepraTypes.h"
#include "String.h"

namespace Lepra
{
class DiskFile;
class Canvas;
class Reader;
class Writer;

class BmpLoader
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

	enum Compression
	{
		COMP_BI_RGB		= 0,
		COMP_BI_RLE8,
		COMP_BI_RLE4,
		COMP_BI_BITFIELDS,
	};

	Status Load(const String& pFileName, Canvas& pCanvas);
	Status Save(const String& pFileName, const Canvas& pCanvas);

	Status Load(const String& pArchiveName, const String& pFileName, Canvas& pCanvas);
	Status Save(const String& pArchiveName, const String& pFileName, const Canvas& pCanvas);

	Status Load(Reader& pReader, Canvas& pCanvas);
	Status Save(Writer& pWriter, const Canvas& pCanvas);

private:

	class BitmapFileHeader 
	{
	public:
		uint16	mType;
		unsigned	mSize;
		uint16	mReserved1;
		uint16	mReserved2;
		unsigned	mOffBits;

		int GetSize();

		bool Load(Reader* pReader);
		bool Save(Writer* pWriter);
	};

	class BitmapInfoHeader
	{
	public:
		int	mSize;
		int	mWidth;
		int	mHeight;
		int16	mPlanes;
		int16	mBitCount;
		int	mCompression;
		int	mSizeImage;
		int	mXPelsPerMeter;
		int	mYPelsPerMeter;
		int	mClrUsed;
		int	mClrImportant;

		int GetSize();

		bool Load(Reader* pReader);
		bool Save(Writer* pWriter);
	};

};

} // End namespace.

#endif // !LEPRA_BMPLOADER_H
