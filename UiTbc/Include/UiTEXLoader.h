/*
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

	NOTES:

	Loads tex-files into a Canvas.

	TEX (Texture) is partially my own texture file format,
	specially designed to use with games. It doesn't only store
	a color map and eventually an alpha-channel as other file
	formats, but specular maps, normal maps, prefabricated
	mip maps etc as well.

	All image dimensions are a power of 2, and nothing else.

	The TEX file format is a plain and simple raw data file format,
	with no mubo jumbo information stored all over the file, as in
	standardized file formats like TIFF or kTga.

	TEX-standard is that the data is stored either as raw uncompressed
	24- or 8-bit (color or monochrome) data, or compressed using
	jpeg-compression.

	TEX uses Intel little endian byte ordering.
*/



#pragma once

#include "../../lepra/include/diskfile.h"
#include "../../lepra/include/canvas.h"
#include "../include/uitbc.h"



struct jpeg_decompress_struct;
struct jpeg_compress_struct;



namespace uitbc {



class Texture;



class TEXLoader {
public:
	friend class TEXFriend;

	enum IoStatus {
		kStatusOpenError = 0,
		kStatusSuccess,
		kStatusMissingColormapError,
		kStatusMemoryError,
		kStatusReadHeaderError,
		kStatusReadInfoError,
		kStatusReadPaletteError,
		kStatusReadPictureError,
		kStatusResolutionError,
		kStatusCompressionError,
	};

	IoStatus Load(const str& file_name, Texture& texture, bool merge_color_and_alpha);
	IoStatus Save(const str& file_name, const Texture& texture, bool compressed);

	IoStatus Load(const str& archive_name, const str& file_name, Texture& texture, bool merge_color_and_alpha);
	IoStatus Save(const str& archive_name, const str& file_name, const Texture& texture, bool compressed);

protected:
private:

	IoStatus Load(Texture& texture, bool merge_color_and_alpha);
	IoStatus Save(const Texture& texture, bool compressed);

	enum { // Map flags
		kAlphaMap    = (1 << 0), // 8-bit alpha channel.
		kNormalMap   = (1 << 1), // 24-bit XYZ normal map, 32-bit XYZS if combined with specular map.
		kSpecularMap = (1 << 2), // 8-bit specular map. Merged with normal map.
		kCubeMap     = (1 << 3), // 6 24-bit color maps ONLY. Not possible to combine with the
					 // previous flags. The color maps are stored in the
					 // following order: PosX, NegX, PosY, NegY, PosZ and NegZ.
	};

	class FileHeader {
	public:
		char tex_magic_[4];
		unsigned short version_;
		unsigned char dimension_powers_;   // Image height = 1 << "upper 4 bits"
						     // Image width  = 1 << "lower 4 bits"
		unsigned char compression_flag_;   // 1 if using jpeg compression, 0 otherwise.
		unsigned map_flags_;

		// Offset into the file where the color data can be found.
		unsigned data_offset_;

		// Data members not stored in the file...
		int width_;
		int height_;
		int num_mip_map_levels_;

		inline bool ReadHeader(File* file) {
			if (file->ReadData(tex_magic_, 4) != kIoOk ||
			   file->Read(version_) != kIoOk) {
				return false;
			}

			if (file->Read(data_offset_) != kIoOk ||
			   file->Read(dimension_powers_) != kIoOk ||
			   file->Read(compression_flag_) != kIoOk ||
			   file->Read(map_flags_) != kIoOk) {
				return false;
			}

			if (tex_magic_[0] != 'T' ||
			   tex_magic_[1] != 'T' ||
			   tex_magic_[2] != 'E' ||
			   tex_magic_[3] != 'X') {
				return false;
			}

			if (compression_flag_ != 0 &&
			   compression_flag_ != 1) {
				return false;
			}

			// Get the texture dimensions.

			int log2_width  = (dimension_powers_ & 0x0F);
			int log2_height = ((dimension_powers_ >> 4) & 0x0F);
			width_  = (1 << log2_width);
			height_ = (1 << log2_height);

			num_mip_map_levels_ = log2_width;
			if (log2_height > log2_width) {
				num_mip_map_levels_ = log2_height;
			}
			num_mip_map_levels_++;

			return true;
		}

		inline bool CheckMapFlag(unsigned long pFlag) {
			return ((map_flags_ & pFlag) != 0);
		}
	};

	IoStatus ReadJpeg(Canvas& canvas);
	IoStatus WriteJpeg(const Canvas& canvas);

	void InitSourceManager(jpeg_decompress_struct* c_info);
	void InitDestinationManager(jpeg_compress_struct* c_info);

	File* load_file_;
	File* save_file_;
};



}
