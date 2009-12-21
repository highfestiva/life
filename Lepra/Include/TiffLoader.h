/*
	Class:  TiffLoader
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games

	NOTES:

	A class used to load/save tiff-files into/from a Canvas.
*/
#ifndef LEPRA_TIFFLOADER_H
#define LEPRA_TIFFLOADER_H

#include "LepraTypes.h"
#include "DiskFile.h"

namespace Lepra
{

class Canvas;
class Color;

class TiffLoader
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
		STATUS_PIXELFORMAT_ERROR,
		STATUS_RESOLUTION_ERROR,
		STATUS_COMPRESSION_ERROR,
		STATUS_UNKNOWNFILESIZE_ERROR,
		STATUS_READSTREAM_ERROR,
	};

	struct Header
	{
		int mWidth;
		int mHeight;
		int mSamplesPerPixel;
		int mBitsPerPixel;
	};

	Status Load(const str& pFileName, Canvas& pCanvas);
	Status Save(const str& pFileName, const Canvas& pCanvas);

	Status Load(const str& pArchiveName, const str& pFileName, Canvas& pCanvas);
	Status Save(const str& pArchiveName, const str& pFileName, const Canvas& pCanvas);

	Status Load(Reader& pReader, Canvas& pCanvas);
	Status Save(Writer& pWriter, const Canvas& pCanvas);

	Status ReadHeader(const str& pFileName, Header& pHeader);
	Status ReadHeader(const str& pArchiveName, const str& pFileName, Header& pHeader);


	/*
		Special piece by piece loading functions. 
		Good to use with very large files.
	*/

	// Will open the file, read the header, and return.
	// The user must call EndLoad() when finished.
	Status StartLoad(const str& pFileName, Header& pHeader);
	Status StartLoad(const str& pArchiveName, const str& pFileName, Header& pHeader);

	// Will load the given rectangle into pCanvas. 
	// The coordinates may be outside the image, in which 
	// case the "outside" will be filled with zeroes (usually black).
	Status LoadPiece(int pLeft, int pTop, int pRight, int pBottom, Canvas& pCanvas);

	void EndLoad();

	void EndPiecewiseLoad();

protected:
private:

	Status StartLoad(Header& pHeader);

	Status Load(File& pFile, Canvas& pCanvas);

	enum Tag
	{
		// Basic tags.
		TAG_NEWSUBFILETYPE			= 254,
		TAG_SUBFILETYPE				= 255,
		TAG_IMAGEWIDTH				= 256,
		TAG_IMAGELENGTH				= 257,
		TAG_BITSPERSAMPLE			= 258,
		TAG_COMPRESSION				= 259,
		TAG_PHOTOMETRICLNTERP		= 262,
		TAG_THRESHOLDING			= 263,
		TAG_CELLWIDTH				= 264,
		TAG_CELLLENGTH				= 265,
		TAG_FILLORDER				= 266,
		TAG_DOCUMENTNAME			= 269,
		TAG_IMAGEDESCRIPTION		= 270,
		TAG_MAKE					= 271,
		TAG_MODEL					= 272,
		TAG_STRIPOFFSETS			= 273,
		TAG_ORIENTATION				= 274,
		TAG_SAMPLESPERPIXEL			= 277,
		TAG_ROWSPERSTRIP			= 278,
		TAG_STRIPBYTECOUNTS			= 279,
		TAG_MINSAMPLEVALUE			= 280,
		TAG_MAXSAMPLEVALUE			= 281,
		TAG_XRESOLUTION				= 282,
		TAG_YRESOLUTION				= 283,
		TAG_PLANARCONFIGURATION		= 284,
		TAG_PAGENAME				= 285,
		TAG_XPOSITION				= 286,
		TAG_YPOSITION				= 287,
		TAG_FREEOFFSETS				= 288,
		TAG_FREEBYTECOUNTS			= 289,
		TAG_GRAYRESPONSEUNIT		= 290,
		TAG_GRAYRESPONSECURVE		= 291,
		TAG_GROUP3OPTIONS			= 292,
		TAG_GROUP4OPTIONS			= 293,
		TAG_RESOLUTIONUNIT			= 296,
		TAG_PAGENUMBER				= 297,
		TAG_COLORRESPONSECURVE		= 301,
		TAG_SOFTWARE				= 305,
		TAG_DATETIME				= 306,
		TAG_PREDICTOR				= 317,
		TAG_ARTIST					= 315,
		TAG_HOSTCOMPUTER			= 316,
		TAG_WHITEPOINT				= 318,
		TAG_PRIMARYCHROMATICITIES	= 319,
		TAG_COLORMAP				= 320,
		TAG_EXTRASAMPLES			= 338,
		TAG_COPYRIGHT				= 33432,
	};

	enum //Field Type
	{
		TYPE_BYTE = 1,	// 8 bits.
		TYPE_ASCII,		// null-terminated string.
		TYPE_SHORT,		// 2 bytes.
		TYPE_LONG,		// 4 bytes.
		TYPE_RATIONAL,	// A rational number, 2 LONG, first is numerator, second is denominator.
		TYPE_SBYTE,		// 8 bits, signed byte (twos-complement).
		TYPE_UNDEFINED,	// 8 bits of arbitrary data specified by the tag.
		TYPE_SSHORT,	// A 16-bit (2-byte) signed (twos-complement) integer.
		TYPE_SLONG,		// A 32-bit (4-byte) signed (twos-complement) integer.
		TYPE_SRATIONAL,	// 2 SLONGS.
		TYPE_FLOAT,		// Single precision (4-byte) IEEE format.
		TYPE_DOUBLE,	// Double precision (8-byte) IEEE format.
	};

	enum Orientation
	{
		ORIENTATION_TOPLEFT = 1,	// 0th row at top,    0th column at left.
		ORIENTATION_TOPRIGHT,		// 0th row at top,    0th column at right.
		ORIENTATION_BOTTOMRIGHT,	// 0th row at bottom, 0th column at right.
		ORIENTATION_BOTTOMLEFT,		// 0th row at bottom, 0th column at left.
		ORIENTATION_LEFTTOP,		// 0th row at left,   0th column at top.
		ORIENTATION_RIGHTTOP,		// 0th row at right,  0th column at top.
		ORIENTATION_RIGHTBOTTOM,	// 0th row at right,  0th column at bottom.
		ORIENTATION_LEFTBOTTOM,		// 0th row at left,   0th column at bottom.

		ORIENTATION_DEFAULT = ORIENTATION_TOPLEFT,
	};

	enum PhotometricInterpretation
	{
		PI_INVALID = -1,	// Not in the TIFF specification.

		PI_WHITEISZERO = 0,
		PI_BLACKISZERO,
		PI_RGB,
		PI_PALETTE,
		PI_TRANSPARENCYMASK,
	};

	enum PlanarConfig
	{
		PC_CHUNKY = 1,
		PC_PLANAR,
	};

	enum Compression
	{
		COMPRESSION_NONE     = 1,
		COMPRESSION_CCITT    = 2,
		COMPRESSION_CCITT_T4 = 3,
		COMPRESSION_CCITT_T6 = 4,
		COMPRESSION_LZW      = 5, // OBS! Patented in the US! You need a license in order to implement this.
		COMPRESSION_PACKBITS = 32733,

		COMPRESSION_DEFAULT = COMPRESSION_NONE,
	};

	enum ExtraSamples
	{
		EXTRA_UNSPECIFIED = 0,
		EXTRA_ASSOCIATEDALPHA,
		EXTRA_UNASSOCIATEDALPHA,
	};

	enum FillOrder
	{
		FILLORDER_HIGHBITSTOTHELEFT = 1,
		FILLORDER_HIGHBITSTOTHERIGHT,

		FILLORDER_DEFAULT = FILLORDER_HIGHBITSTOTHELEFT,
	};

	class DirectoryEntry
	{
	public:
		DirectoryEntry();
		DirectoryEntry(File& pFile);
		uint16 mTag;
		uint16 mType;
		unsigned mCount;
		unsigned mValueOffset;
	};

	// A class containing all data that we are interested in to be able
	// to load the image.
	class IFDInfo
	{
	public:

		IFDInfo();
		virtual ~IFDInfo();

		bool CheckPixelFormat();

		bool mReducedResVersion;
		bool mIsSinglePage;
		bool mIsAlphaMask;
		unsigned   mWidth;
		unsigned   mHeight;
		unsigned   mSamplesPerPixel;
		uint16* mSampleBits;
		unsigned   mBitsPerPixel;
		unsigned   mRowsPerStrip;
		unsigned*  mStripByteCounts;
		unsigned*  mStripOffsets;
		unsigned   mStripCount; // Num indices in mStripByteCounts.
		unsigned   mExtraSamples;
		ExtraSamples mExtra;
		Orientation mOrientation;
		Compression mCompression;
		FillOrder mFillOrder;
		PhotometricInterpretation mPI;
		PlanarConfig mPlanarConfig;
		Color* mPalette;
		uint16 mPaletteColorCount;

	private:
	};

	class ClipData
	{
	public:
		ClipData(int pLeft, int pTop, 
				 int pRight, int pBottom, 
				 int pDstWidth, int pDstHeight);

		int mDstXOffset;
		int mDstYOffset;

		int mSrcXOffset;
		int mSrcYOffset;

		int mSrcReadWidth;
		int mSrcReadHeight;

	};

	inline bool CheckBit(unsigned pSource, int pBit);

	Status ReadIFD(File& pFile, IFDInfo& pIFDInfo);
	Status ReadImage(File& pFile, IFDInfo& pIFDInfo, Canvas& pCanvas);
	Status ReadMonochromeImage(File& pFile, IFDInfo& pIFDInfo, Canvas& pCanvas, bool pInvert);
	Status ReadIndexedImage(File& pFile, IFDInfo& pIFDInfo, Canvas& pCanvas);
	Status ReadColorImage(File& pFile, IFDInfo& pIFDInfo, Canvas& pCanvas);

	Status ReadMonochromeImage(ClipData& pClipData, Canvas& pCanvas);
	Status ReadColorImage(ClipData& pClipData, Canvas& pCanvas);

	// Used when loading piece by piece.
	File* mLoadFile;
	IFDInfo mIFDInfo;
};

bool TiffLoader::CheckBit(unsigned pSource, int pBit)
{
	return (pSource & (1 << pBit)) != 0;
}

} // End namespace.

#endif // !LEPRA_TIFFLOADER_H
