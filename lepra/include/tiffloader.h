/*
	Class:  TiffLoader
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

	NOTES:

	A class used to load/save tiff-files into/from a Canvas.
*/

#pragma once

#include "string.h"



namespace lepra {



class Canvas;
class Color;
class File;
class Reader;
class Writer;



class TiffLoader {
public:

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
		kStatusUnknownfilesizeError,
		kStatusReadstreamError,
	};

	struct Header {
		int width_;
		int height_;
		int samples_per_pixel_;
		int bits_per_pixel_;
	};

	Status Load(const str& file_name, Canvas& canvas);
	Status Save(const str& file_name, const Canvas& canvas);

	Status Load(const str& archive_name, const str& file_name, Canvas& canvas);
	Status Save(const str& archive_name, const str& file_name, const Canvas& canvas);

	Status Load(Reader& reader, Canvas& canvas);
	Status Save(Writer& writer, const Canvas& canvas);

	Status ReadHeader(const str& file_name, Header& header);
	Status ReadHeader(const str& archive_name, const str& file_name, Header& header);


	/*
		Special piece by piece loading functions.
		Good to use with very large files.
	*/

	// Will open the file, read the header, and return.
	// The user must call EndLoad() when finished.
	Status StartLoad(const str& file_name, Header& header);
	Status StartLoad(const str& archive_name, const str& file_name, Header& header);

	// Will load the given rectangle into canvas.
	// The coordinates may be outside the image, in which
	// case the "outside" will be filled with zeroes (usually black).
	Status LoadPiece(int left, int top, int right, int bottom, Canvas& canvas);

	void EndLoad();

	void EndPiecewiseLoad();

protected:
private:

	Status StartLoad(Header& header);

	Status Load(File& file, Canvas& canvas);

	enum Tag {
		// Basic tags.
		kTagNewsubfiletype			= 254,
		kTagSubfiletype				= 255,
		kTagImagewidth				= 256,
		kTagImagelength				= 257,
		kTagBitspersample			= 258,
		kTagCompression				= 259,
		kTagPhotometriclnterp		= 262,
		kTagThresholding			= 263,
		kTagCellwidth				= 264,
		kTagCelllength				= 265,
		kTagFillorder				= 266,
		kTagDocumentname			= 269,
		kTagImagedescription		= 270,
		kTagMake					= 271,
		kTagModel					= 272,
		kTagStripoffsets			= 273,
		kTagOrientation				= 274,
		kTagSamplesperpixel			= 277,
		kTagRowsperstrip			= 278,
		kTagStripbytecounts			= 279,
		kTagMinsamplevalue			= 280,
		kTagMaxsamplevalue			= 281,
		kTagXresolution				= 282,
		kTagYresolution				= 283,
		kTagPlanarconfiguration		= 284,
		kTagPagename				= 285,
		kTagXposition				= 286,
		kTagYposition				= 287,
		kTagFreeoffsets				= 288,
		kTagFreebytecounts			= 289,
		kTagGrayresponseunit		= 290,
		kTagGrayresponsecurve		= 291,
		kTagGroup3Options			= 292,
		kTagGroup4Options			= 293,
		kTagResolutionunit			= 296,
		kTagPagenumber				= 297,
		kTagColorresponsecurve		= 301,
		kTagSoftware				= 305,
		kTagDatetime				= 306,
		kTagPredictor				= 317,
		kTagArtist					= 315,
		kTagHostcomputer			= 316,
		kTagWhitepoint				= 318,
		kTagPrimarychromaticities	= 319,
		kTagColormap				= 320,
		kTagExtrasamples			= 338,
		kTagCopyright				= 33432,
	};

	enum { //Field Type
		kTypeByte = 1,	// 8 bits.
		kTypeAscii,		// null-terminated string.
		kTypeShort,		// 2 bytes.
		kTypeLong,		// 4 bytes.
		kTypeRational,	// A rational number, 2 LONG, first is numerator, second is denominator.
		kTypeSbyte,		// 8 bits, signed byte (twos-complement).
		kTypeUndefined,	// 8 bits of arbitrary data specified by the tag.
		kTypeSshort,	// A 16-bit (2-byte) signed (twos-complement) integer.
		kTypeSlong,		// A 32-bit (4-byte) signed (twos-complement) integer.
		kTypeSrational,	// 2 SLONGS.
		kTypeFloat,		// Single precision (4-byte) IEEE format.
		kTypeDouble,	// Double precision (8-byte) IEEE format.
	};

	enum Orientation {
		kOrientationTopleft = 1,	// 0th row at top,    0th column at left.
		kOrientationTopright,		// 0th row at top,    0th column at right.
		kOrientationBottomright,	// 0th row at bottom, 0th column at right.
		kOrientationBottomleft,		// 0th row at bottom, 0th column at left.
		kOrientationLefttop,		// 0th row at left,   0th column at top.
		kOrientationRighttop,		// 0th row at right,  0th column at top.
		kOrientationRightbottom,	// 0th row at right,  0th column at bottom.
		kOrientationLeftbottom,		// 0th row at left,   0th column at bottom.

		kOrientationDefault = kOrientationTopleft,
	};

	enum PhotometricInterpretation {
		kPiInvalid = -1,	// Not in the TIFF specification.

		kPiWhiteiszero = 0,
		kPiBlackiszero,
		kPiRgb,
		kPiPalette,
		kPiTransparencymask,
	};

	enum PlanarConfig {
		kPcChunky = 1,
		kPcPlanar,
	};

	enum Compression {
		kCompressionNone     = 1,
		kCompressionCcitt    = 2,
		kCompressionCcittT4 = 3,
		kCompressionCcittT6 = 4,
		kCompressionLzw      = 5, // OBS! Patented in the US! You need a license in order to implement this.
		kCompressionPackbits = 32733,

		kCompressionDefault = kCompressionNone,
	};

	enum ExtraSamples {
		kExtraUnspecified = 0,
		kExtraAssociatedalpha,
		kExtraUnassociatedalpha,
	};

	enum FillOrder {
		kFillorderHighbitstotheleft = 1,
		kFillorderHighbitstotheright,

		kFillorderDefault = kFillorderHighbitstotheleft,
	};

	class DirectoryEntry {
	public:
		DirectoryEntry();
		DirectoryEntry(File& file);
		uint16 tag_;
		uint16 type_;
		unsigned count_;
		unsigned value_offset_;
	};

	// A class containing all data that we are interested in to be able
	// to load the image.
	class IFDInfo {
	public:

		IFDInfo();
		virtual ~IFDInfo();

		bool CheckPixelFormat();

		bool reduced_res_version_;
		bool is_single_page_;
		bool is_alpha_mask_;
		unsigned   width_;
		unsigned   height_;
		unsigned   samples_per_pixel_;
		uint16* sample_bits_;
		unsigned   bits_per_pixel_;
		unsigned   rows_per_strip_;
		unsigned*  strip_byte_counts_;
		unsigned*  strip_offsets_;
		unsigned   strip_count_; // Num indices in strip_byte_counts_.
		unsigned   extra_samples_;
		ExtraSamples extra_;
		Orientation orientation_;
		Compression compression_;
		FillOrder fill_order_;
		PhotometricInterpretation pi_;
		PlanarConfig planar_config_;
		Color* palette_;
		uint16 palette_color_count_;

	private:
	};

	class ClipData {
	public:
		ClipData(int left, int top,
				 int right, int bottom,
				 int dst_width, int dst_height);

		int dst_x_offset_;
		int dst_y_offset_;

		int src_x_offset_;
		int src_y_offset_;

		int src_read_width_;
		int src_read_height_;

	};

	inline bool CheckBit(unsigned source, int bit);

	Status ReadIFD(File& file, IFDInfo& ifd_info);
	Status ReadImage(File& file, IFDInfo& ifd_info, Canvas& canvas);
	Status ReadMonochromeImage(File& file, IFDInfo& ifd_info, Canvas& canvas, bool invert);
	Status ReadIndexedImage(File& file, IFDInfo& ifd_info, Canvas& canvas);
	Status ReadColorImage(File& file, IFDInfo& ifd_info, Canvas& canvas);

	Status ReadMonochromeImage(ClipData& clip_data, Canvas& canvas);
	Status ReadColorImage(ClipData& clip_data, Canvas& canvas);

	// Used when loading piece by piece.
	File* load_file_;
	IFDInfo ifd_info_;
};

bool TiffLoader::CheckBit(unsigned source, int bit) {
	return (source & (1 << bit)) != 0;
}



}
