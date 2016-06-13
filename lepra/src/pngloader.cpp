/*
	Class:  PngLoader
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#include "pch.h"
#include "../include/pngloader.h"
#include "../include/graphics2d.h"
#include "../include/canvas.h"
#include "../include/metafile.h"
#include "../include/archivefile.h"

#ifdef LEPRA_MSVC
// Disable warning about functions that include catch which may not support C++ semantics. PNG loader
// requires setjmp function, which has this downside.
#pragma warning(disable: 4611)
#endif // LEPRA_MSVC

// The png_jmpbuf() macro, used in error handling, became available in
// libpng version 1.0.6.  If you want to be able to run your code with older
// versions of libpng, you must define the macro yourself (but only if it
// is not already defined by libpng!).
#ifndef png_jmpbuf
#define png_jmpbuf(png_ptr) ((png_ptr)->jmpbuf)
#endif



namespace lepra {



PngLoader::Status PngLoader::Load(const str& file_name, Canvas& canvas) {
	Status status = kStatusSuccess;
	MetaFile file;

	if (file.Open(file_name, MetaFile::kReadOnly) == false) {
		status = kStatusOpenError;
	}

	if (status == kStatusSuccess) {
		status = Load(file, canvas);
		file.Close();
	}

	return status;
}

PngLoader::Status PngLoader::Save(const str& file_name, const Canvas& canvas) {
	Status status = kStatusSuccess;
	DiskFile file;

	if (file.Open(file_name, DiskFile::kModeWrite) == false) {
		status = kStatusOpenError;
	}

	if (status == kStatusSuccess) {
		status = Save(file, canvas);
		file.Close();
	}

	return status;
}

PngLoader::Status PngLoader::Load(const str& archive_name, const str& file_name, Canvas& canvas) {
	Status status = kStatusSuccess;
	ArchiveFile file(archive_name);

	if (file.Open(file_name, ArchiveFile::kReadOnly) == false) {
		status = kStatusOpenError;
	}

	if (status == kStatusSuccess) {
		status = Load(file, canvas);
		file.Close();
	}

	return status;
}

PngLoader::Status PngLoader::Save(const str& archive_name, const str& file_name, const Canvas& canvas) {
	Status status = kStatusSuccess;
	ArchiveFile file(archive_name);

	if (file.Open(file_name, ArchiveFile::kWriteOnly) == false) {
		status = kStatusOpenError;
	}

	if (status == kStatusSuccess) {
		status = Save(file, canvas);
		file.Close();
	}

	return status;
}

PngLoader::Status PngLoader::Load(Reader& reader, Canvas& canvas) {
	reader_ = &reader;

	if (CheckIfPNG() == false) {
		log_.Warning("PNG header error!");
		return kStatusReadHeaderError;
	}

	png_structp _png = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
	if (_png == 0) {
		log_.Error("PNG reader runs out of memory!");
		return kStatusMemoryError;
	}

	png_infop info = png_create_info_struct(_png);
	if (info == 0) {
		log_.Error("PNG reader runs out of memory!");
		png_destroy_read_struct(&_png, png_infopp_NULL, png_infopp_NULL);
		return kStatusMemoryError;
	}

	if (setjmp(png_jmpbuf(_png))) {
		log_.Error("PNG reader runs out of memory!");
		png_destroy_read_struct(&_png, &info, png_infopp_NULL);
		return kStatusMemoryError;
	}

	png_set_read_fn(_png, (void*)this, ReadDataCallback);
	png_set_sig_bytes(_png, 8);
	png_read_png(_png, info, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING, 0);

	const int width = png_get_image_width(_png, info);
	const int height = png_get_image_height(_png, info);
	int i;
	switch(png_get_color_type(_png, info)) {
	case PNG_COLOR_TYPE_GRAY: {
			canvas.Reset(width, height, Canvas::kBitdepth8Bit);
			canvas.CreateBuffer();
			uint8* buffer = (uint8*)canvas.GetBuffer();
			const int pixel_byte_size = canvas.GetPixelByteSize();
			const int pitch = canvas.GetPitch();

			Color palette[256];
			for (i = 0; i < 256; i++) {
				palette[i].red_   = (uint8)i;
				palette[i].green_ = (uint8)i;
				palette[i].blue_  = (uint8)i;
				palette[i].alpha_ = 255;
			}

			canvas.SetPalette(palette);

			uint8** row = png_get_rows(_png, info);
			for (i = 0; i < (int)height; i++) {
				memcpy(&buffer[i * pitch * pixel_byte_size], row[i], width * pixel_byte_size);
			}
		} break;
	case PNG_COLOR_TYPE_GRAY_ALPHA: {
			canvas.Reset(width, height, Canvas::kBitdepth32Bit);
			canvas.CreateBuffer();
			uint8* buffer = (uint8*)canvas.GetBuffer();
			const int pixel_byte_size = canvas.GetPixelByteSize();
			const int pitch = canvas.GetPitch();

			Color palette[256];
			for (i = 0; i < 256; i++) {
				palette[i].red_   = (uint8)i;
				palette[i].green_ = (uint8)i;
				palette[i].blue_  = (uint8)i;
				palette[i].alpha_ = 255;
			}

			canvas.SetPalette(palette);

			uint8** row = png_get_rows(_png, info);
			for (int y = 0; y < (int)height; y++) {
				uint8* dst_row = &buffer[y * pitch * pixel_byte_size];
				for (int x = 0; x < (int)width; x++) {
					int __index = x * pixel_byte_size;
					dst_row[__index + 0] = row[y][x * 2 + 0];
					dst_row[__index + 1] = row[y][x * 2 + 0];
					dst_row[__index + 2] = row[y][x * 2 + 0];
					dst_row[__index + 3] = row[y][x * 2 + 1];
				}
			}
		} break;
	case PNG_COLOR_TYPE_PALETTE: {
			canvas.Reset(width, height, Canvas::kBitdepth8Bit);
			canvas.CreateBuffer();
			uint8* buffer = (uint8*)canvas.GetBuffer();
			const int pixel_byte_size = canvas.GetPixelByteSize();
			const int pitch = canvas.GetPitch();

			int num_entries;
			png_color* png_palette;
			png_get_PLTE(_png, info, &png_palette, &num_entries);
			Color palette[256];
			for (i = 0; i < num_entries; i++) {
				palette[i].red_   = png_palette[i].red;
				palette[i].green_ = png_palette[i].green;
				palette[i].blue_  = png_palette[i].blue;
				palette[i].alpha_ = 255;
			}

			png_bytep trans;
			png_color_16p trans_values;
			png_get_tRNS(_png, info, &trans, &num_entries, &trans_values);
			for (i = 0; i < num_entries; i++) {
				// Is trans_values[i].index == trans[i]?
				palette[trans_values[i].index].alpha_ = 0;
			}

			canvas.SetPalette(palette);

			uint8** row = png_get_rows(_png, info);
			for (i = 0; i < (int)height; i++) {
				memcpy(&buffer[i * pitch * pixel_byte_size], row[i], width * pixel_byte_size);
			}
		} break;
	case PNG_COLOR_TYPE_RGB: {
			canvas.Reset(width, height, Canvas::kBitdepth24Bit);
			canvas.CreateBuffer();
			uint8* buffer = (uint8*)canvas.GetBuffer();
			const int pixel_byte_size = canvas.GetPixelByteSize();
			const int pitch = canvas.GetPitch();

			uint8** row = png_get_rows(_png, info);
			for (i = 0; i < (int)height; i++) {
				memcpy(&buffer[i * pitch * pixel_byte_size], row[i], width * pixel_byte_size);
			}
		} break;
	case PNG_COLOR_TYPE_RGB_ALPHA: {
			canvas.Reset(width, height, Canvas::kBitdepth32Bit);
			canvas.CreateBuffer();
			uint8* buffer = (uint8*)canvas.GetBuffer();
			const int pixel_byte_size = canvas.GetPixelByteSize();
			const int pitch = canvas.GetPitch();

			uint8** row = png_get_rows(_png, info);
			for (i = 0; i < (int)height; i++) {
				memcpy(&buffer[i * pitch * pixel_byte_size], row[i], width * pixel_byte_size);
			}
		} break;
	default:
		log_.Error("PNG is of unknown type!");
		png_destroy_read_struct(&_png, &info, 0);
		return kStatusReadInfoError;
	}

	png_destroy_read_struct(&_png, &info, 0);

	canvas.SwapRGBOrder();

	return kStatusSuccess;
}

PngLoader::Status PngLoader::Save(Writer& writer, const Canvas& canvas) {
	writer_ = &writer;

	png_structp _png = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);

	if (_png == 0) {
		return kStatusMemoryError;
	}

	png_infop info = png_create_info_struct(_png);

	if (info == 0) {
		png_destroy_write_struct(&_png, 0);
		return kStatusMemoryError;
	}

	if (setjmp(png_jmpbuf(_png))) {
		png_destroy_write_struct(&_png, &info);
		return kStatusMemoryError;
	}

	png_set_write_fn(_png, (void*)this, WriteDataCallback, FlushCallback);

	// Copy the image and convert it to an appropriate bit depth.
	Canvas image(canvas, true);
	int color_type = 0;
	int bit_depth = 0;
	switch(image.GetBitDepth()) {
	case Canvas::kBitdepth8Bit:
		color_type = PNG_COLOR_TYPE_PALETTE;
		bit_depth = 8;
		break;
	case Canvas::kBitdepth15Bit:
	case Canvas::kBitdepth16Bit:
		image.ConvertBitDepth(Canvas::kBitdepth24Bit);
	case Canvas::kBitdepth24Bit:
		color_type = PNG_COLOR_TYPE_RGB;
		bit_depth = 8;
		break;
	case Canvas::kBitdepth32Bit:
		color_type = PNG_COLOR_TYPE_RGBA;
		bit_depth = 8;
		break;
	case Canvas::kBitdepth32BitPerChannel:
		image.ConvertBitDepth(Canvas::kBitdepth16BitPerChannel);
	case Canvas::kBitdepth16BitPerChannel:
		color_type = PNG_COLOR_TYPE_RGB;
		bit_depth = 16;
		break;
	}

	// Set image header.
	png_set_IHDR(_png, info,
				 image.GetWidth(), image.GetHeight(),
				 bit_depth,
				 color_type,
				 PNG_INTERLACE_NONE,
				 PNG_COMPRESSION_TYPE_BASE,
				 PNG_FILTER_TYPE_BASE);

	// Set palette if needed.
	png_colorp palette = 0;
	if (image.GetBitDepth() == Canvas::kBitdepth8Bit) {
		palette = (png_colorp)png_malloc(_png, PNG_MAX_PALETTE_LENGTH * png_sizeof(png_color));

		const Color* src_plt = image.GetPalette();
		int i;
		for (i = 0; i < 256 && i < PNG_MAX_PALETTE_LENGTH; i++) {
		   palette[i].red   = src_plt[i].red_;
		   palette[i].green = src_plt[i].green_;
		   palette[i].blue  = src_plt[i].blue_;
		}

		png_set_PLTE(_png, info, palette, PNG_MAX_PALETTE_LENGTH);
	}

	png_write_info(_png, info);

	if (Endian::GetSystemEndian() == Endian::kTypeLittleEndian) {
		// The byte order must be big endian.
		png_set_swap(_png);
	}

	int i;
	png_bytep* row = new png_bytep[image.GetHeight()];
	for (i = 0; i < (int)image.GetHeight(); i++) {
		row[i] = &((png_bytep)image.GetBuffer())[i * image.GetPitch() * image.GetPixelByteSize()];
	}

	png_write_image(_png, row);
	png_write_end(_png, info);

	delete[] row;

	if (palette != 0) {
		png_free(_png, palette);
	}

	png_destroy_write_struct(&_png, &info);

	return kStatusSuccess;
}

bool PngLoader::CheckIfPNG() {
	uint8 buffer[8];
	if (reader_->ReadData(buffer, 8) != kIoOk) {
		return false;
	}

	return (png_sig_cmp(buffer, (png_size_t)0, 8) == 0);
}

void PngLoader::ReadDataCallback(png_structp png, png_bytep data, png_size_t length) {
    PngLoader* value = (PngLoader*)png_get_io_ptr(png);
	value->reader_->ReadData(data, (unsigned)length);
}

void PngLoader::WriteDataCallback(png_structp png, png_bytep data, png_size_t length) {
    PngLoader* value = (PngLoader*)png_get_io_ptr(png);
	value->writer_->WriteData(data, (unsigned)length);
}

void PngLoader::FlushCallback(png_structp /*png*/) {
}



loginstance(kUiGfx2D, PngLoader);



}
