
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/uitexloader.h"
#include "../../thirdparty/jpeg-6b/jinclude.h"
#include "../../thirdparty/jpeg-6b/jpeglib.h"
#include "../../thirdparty/jpeg-6b/jerror.h"
#include "../../lepra/include/archivefile.h"
#include "../../lepra/include/canvas.h"
#include "../../lepra/include/diskfile.h"
#include "../../lepra/include/metafile.h"
#include "../../lepra/include/graphics2d.h"
#include "../include/uitexture.h"
#include "../include/uitbc.h"



namespace uitbc {



enum {
	kIoBufferSize = 4096,
};


/*
	A class used to (locally, within this file) get access to the
	tex loader's private members.
*/

class TEXFriend {
public:
	static inline File* GetLoadFile(TEXLoader* tex_loader) {
		return tex_loader->load_file_;
	}
	static inline File* GetSaveFile(TEXLoader* tex_loader) {
		return tex_loader->save_file_;
	}
};


/*
	C style jpeg source management functions.
*/
void TEXInitSource(j_decompress_ptr c_info);
boolean TEXFillInputBuffer(j_decompress_ptr c_info);
void TEXSkipInputData(j_decompress_ptr c_info, long num_bytes);
void TEXTerminateSource(j_decompress_ptr c_info);

/*
	C style jpeg destination management functions.
*/
void TEXInitDestination(j_compress_ptr c_info);
boolean TEXEmptyOutputBuffer(j_compress_ptr c_info);
void TEXTerminateDestination(j_compress_ptr c_info);

/*
	C style jpeg source manager struct.
*/
struct SourceManager {
	jpeg_source_mgr source_manager_;
	unsigned char io_buffer_[kIoBufferSize];
	int io_buffer_size_;
	TEXLoader* tex_loader_;
};

/*
	C style jpeg destination manager struct.
*/
struct DestinationManager {
	jpeg_destination_mgr dest_manager_;
	unsigned char io_buffer_[kIoBufferSize];
	int io_buffer_size_;
	TEXLoader* tex_loader_;
};

void TEXLoader::InitSourceManager(jpeg_decompress_struct* c_info) {
	if (c_info->src == NULL) {
		// First time for this JPEG object?
		c_info->src = (jpeg_source_mgr*)new SourceManager;
	}

	SourceManager* __src = (SourceManager*)c_info->src;
	__src->source_manager_.init_source       = TEXInitSource;
	__src->source_manager_.fill_input_buffer = TEXFillInputBuffer;
	__src->source_manager_.skip_input_data   = TEXSkipInputData;
	__src->source_manager_.resync_to_restart = jpeg_resync_to_restart; /* use default method */
	__src->source_manager_.term_source       = TEXTerminateSource;

	// Forces FillInputBuffer() on first read.
	__src->io_buffer_size_ = 0;
	__src->tex_loader_ = this;
	__src->source_manager_.bytes_in_buffer = 0;
	__src->source_manager_.next_input_byte = NULL;
}

void TEXLoader::InitDestinationManager(jpeg_compress_struct* c_info) {
	if (c_info->dest == NULL) {
		// First time for this JPEG object?
		c_info->dest = (jpeg_destination_mgr*)new DestinationManager;
	}

	DestinationManager* __dest = (DestinationManager*)c_info->dest;

	__dest->io_buffer_size_ = 0;
	__dest->tex_loader_ = this;

	__dest->dest_manager_.init_destination    = TEXInitDestination;
	__dest->dest_manager_.empty_output_buffer = TEXEmptyOutputBuffer;
	__dest->dest_manager_.term_destination    = TEXTerminateDestination;
}

/*
	And here are the C style functions.
*/



void TEXInitSource(j_decompress_ptr c_info) {
	SourceManager* __src = (SourceManager*)c_info->src;
	__src->io_buffer_size_ = 0;
}

boolean TEXFillInputBuffer(j_decompress_ptr c_info) {
	SourceManager* __src = (SourceManager*)c_info->src;

	File* file = TEXFriend::GetLoadFile(__src->tex_loader_);

	uint64 prev_pos = file->Tell();
	/* TODO: use assigned variable!
	IOError __err = */ file->ReadData(__src->io_buffer_, kIoBufferSize);
	uint64 new_pos = file->Tell();
	int _num_bytes = (int)(new_pos - prev_pos);

	if (_num_bytes == 0) {
		WARNMS(c_info, JWRN_JPEG_EOF);

		/* Insert a fake EOI marker */
		__src->io_buffer_[0] = (JOCTET) 0xFF;
		__src->io_buffer_[1] = (JOCTET) JPEG_EOI;

		__src->source_manager_.next_input_byte = __src->io_buffer_;
		__src->source_manager_.bytes_in_buffer = 2;
	} else {
		__src->io_buffer_size_ = _num_bytes;
		__src->source_manager_.next_input_byte = __src->io_buffer_;
		__src->source_manager_.bytes_in_buffer = __src->io_buffer_size_;
	}

	return TRUE;
}

void TEXSkipInputData(j_decompress_ptr c_info, long num_bytes) {
	SourceManager* __src = (SourceManager*)c_info->src;

	if (num_bytes > 0) {
		while (num_bytes > (long)__src->source_manager_.bytes_in_buffer) {
			num_bytes -= (long)__src->source_manager_.bytes_in_buffer;
			TEXFillInputBuffer(c_info);
		}

		__src->source_manager_.next_input_byte += (size_t)num_bytes;
		__src->source_manager_.bytes_in_buffer -= (size_t)num_bytes;
	}
}

void TEXTerminateSource(j_decompress_ptr c_info) {
	SourceManager* __src = (SourceManager*)c_info->src;
	__src->io_buffer_size_ = 0;
	__src->tex_loader_ = 0;
	delete __src;
}






void TEXInitDestination(j_compress_ptr c_info) {
	DestinationManager* __dest = (DestinationManager*)c_info->dest;

	__dest->dest_manager_.next_output_byte = __dest->io_buffer_;
	__dest->dest_manager_.free_in_buffer = kIoBufferSize;
}


boolean TEXEmptyOutputBuffer(j_compress_ptr c_info) {
	DestinationManager* __dest = (DestinationManager*)c_info->dest;

	TEXFriend::GetSaveFile(__dest->tex_loader_)->WriteData(__dest->io_buffer_, kIoBufferSize);

	__dest->dest_manager_.next_output_byte = __dest->io_buffer_;
	__dest->dest_manager_.free_in_buffer = kIoBufferSize;

	return TRUE;
}

void TEXTerminateDestination(j_compress_ptr c_info) {
	DestinationManager* __dest = (DestinationManager*)c_info->dest;
	size_t data_count = kIoBufferSize - __dest->dest_manager_.free_in_buffer;

	/* Write any data remaining in the buffer */
	if (data_count > 0) {
		TEXFriend::GetSaveFile(__dest->tex_loader_)->WriteData(__dest->io_buffer_, (unsigned)data_count);
	}

	__dest->io_buffer_size_ = 0;
	__dest->tex_loader_ = 0;
	delete __dest;
}

TEXLoader::IoStatus TEXLoader::ReadJpeg(Canvas& canvas) {
	unsigned size;
	load_file_->Read(size);

	jpeg_decompress_struct _c_info;
	jpeg_error_mgr j_err;

	_c_info.err = jpeg_std_error(&j_err);
	jpeg_create_decompress(&_c_info);

	InitSourceManager(&_c_info);

	if (jpeg_read_header(&_c_info, TRUE) != JPEG_HEADER_OK) {
		jpeg_destroy_decompress(&_c_info);
		return kStatusReadHeaderError;
	}

	if (jpeg_start_decompress(&_c_info) != TRUE) {
		jpeg_destroy_decompress(&_c_info);
		return kStatusReadPictureError;
	}

	if (_c_info.output_components == 1) {
		// Create grayscale palette.
		Color palette[256];
		for (int i = 0; i < 256; i++) {
			palette[i].Set(i, i, i, i);
		}
		canvas.SetPalette(palette);
	}

	uint8* buffer = (uint8*)canvas.GetBuffer();
	int row_stride = canvas.GetPitch() * canvas.GetPixelByteSize();
	int scan_lines = canvas.GetHeight();

	JSAMPROW offset[1];
	offset[0] = buffer;

	for (int i = 0; i < scan_lines; i++) {
		jpeg_read_scanlines(&_c_info, offset, 1);
		offset[0] += row_stride;
	}

	jpeg_finish_decompress(&_c_info);
	jpeg_destroy_decompress(&_c_info);

	load_file_->SeekCur(size);

	return kStatusSuccess;
}

TEXLoader::IoStatus TEXLoader::WriteJpeg(const Canvas& canvas) {
	int size_offset = (int)save_file_->Tell();
	unsigned size = 0;
	save_file_->Write(size);	// We will update this data after writing the jpeg image.
	int start_offset = (int)save_file_->Tell();

	jpeg_compress_struct _c_info;
	jpeg_error_mgr j_err;
	_c_info.err = jpeg_std_error(&j_err);
	jpeg_create_compress(&_c_info);

	InitDestinationManager(&_c_info);

	_c_info.image_width = canvas.GetWidth();
	_c_info.image_height = canvas.GetHeight();

	if (canvas.GetBitDepth() == Canvas::kBitdepth8Bit) {
		_c_info.input_components = 1;
		_c_info.in_color_space = JCS_GRAYSCALE;
	} else {
		_c_info.input_components = 3;
		_c_info.in_color_space = JCS_RGB;
	}

	// If the image is in 8-bit mode, it is considered to be a grayscale image.
	// Otherwise, we must make sure that the image is in 24-bit kRgb mode.
	Canvas copy(canvas, true);
	if (copy.GetBitDepth() != Canvas::kBitdepth24Bit &&
	   copy.GetBitDepth() != Canvas::kBitdepth8Bit) {
		copy.ConvertBitDepth(Canvas::kBitdepth24Bit);
	}

	jpeg_set_defaults(&_c_info);
	jpeg_start_compress(&_c_info, TRUE);

	uint8* buffer = (uint8*)copy.GetBuffer();
	int row_stride = copy.GetPitch() * copy.GetPixelByteSize();
	int scan_lines = copy.GetHeight();

	JSAMPROW offset[1];
	offset[0] = buffer;

	for (int i = 0; i < scan_lines; i++) {
		jpeg_write_scanlines(&_c_info, offset, 1);
		offset[0] += row_stride;
	}

	jpeg_finish_compress(&_c_info);
	jpeg_destroy_compress(&_c_info);

	// Calculate and write the size of this jpeg image.
	const int end_offset = (int)save_file_->Tell();
	size = (unsigned)(end_offset - start_offset);
	save_file_->SeekSet(size_offset);
	save_file_->Write(size);

	// Go back to where we were...
	save_file_->SeekEnd(0);

	return kStatusSuccess;
}

TEXLoader::IoStatus TEXLoader::Load(const str& file_name, Texture& texture, bool merge_color_and_alpha) {
	IoStatus io_status = kStatusSuccess;
	MetaFile file;
	load_file_ = &file;

	if (file.Open(file_name, MetaFile::kReadOnly) == false) {
		io_status = kStatusOpenError;
	}

	if (io_status == kStatusSuccess) {
		io_status = Load(texture, merge_color_and_alpha);
	}

	return io_status;
}

TEXLoader::IoStatus TEXLoader::Save(const str& file_name, const Texture& texture, bool compressed) {
	IoStatus io_status = kStatusSuccess;
	DiskFile file;
	save_file_ = &file;

	if (file.Open(file_name, DiskFile::kModeWrite) == false) {
		io_status = kStatusOpenError;
	}

	if (io_status == kStatusSuccess) {
		io_status = Save(texture, compressed);
	}

	return io_status;
}

TEXLoader::IoStatus TEXLoader::Load(const str& archive_name, const str& file_name, Texture& texture, bool merge_color_and_alpha) {
	IoStatus io_status = kStatusSuccess;
	ArchiveFile file(archive_name);
	load_file_ = &file;

	if (file.Open(file_name, ArchiveFile::kReadOnly) == false) {
		io_status = kStatusOpenError;
	}

	if (io_status == kStatusSuccess) {
		io_status = Load(texture, merge_color_and_alpha);
	}

	return io_status;
}

TEXLoader::IoStatus TEXLoader::Save(const str& archive_name, const str& file_name, const Texture& texture, bool compressed) {
	IoStatus io_status = kStatusSuccess;
	ArchiveFile file(archive_name);
	save_file_ = &file;

	if (file.Open(file_name, ArchiveFile::kWriteOnly) == false) {
		io_status = kStatusOpenError;
	}

	if (io_status == kStatusSuccess) {
		io_status = Save(texture, compressed);
	}

	return io_status;
}

TEXLoader::IoStatus TEXLoader::Load(Texture& texture, bool merge_color_and_alpha) {
	FileHeader file_header;
	if (file_header.ReadHeader(load_file_) == false) {
		load_file_->Close();
		return kStatusReadHeaderError;
	}

	texture.ClearAll();

	if (file_header.CheckMapFlag(kCubeMap) == false) {
		int i;
		int num_levels = file_header.num_mip_map_levels_;

		// Create all buffers.
		texture.Prepare(true,
				  file_header.CheckMapFlag(kAlphaMap),
				  file_header.CheckMapFlag(kNormalMap),
				  file_header.CheckMapFlag(kSpecularMap),
				  false,
				  file_header.width_,
				  file_header.height_);

		// Jump to the color data.
		load_file_->SeekSet(file_header.data_offset_);

		// Load color map.
		for (i = 0; i < num_levels; i++) {
			// The last two mip map levels are stored raw.
			if (i < (num_levels - 2) && file_header.compression_flag_ == 1)
				ReadJpeg(*texture._GetColorMap(i));
			else
				load_file_->ReadData(texture._GetColorMap(i)->GetBuffer(), texture._GetColorMap(i)->GetBufferByteSize());
		}

		// Load alpha map.
		if (file_header.CheckMapFlag(kAlphaMap) == true) {
			for (i = 0; i < num_levels; i++) {
				// The last two mip map levels are stored raw.
				if (i < (num_levels - 2) && file_header.compression_flag_ == 1)
					ReadJpeg(*texture._GetAlphaMap(i));
				else
					load_file_->ReadData(texture._GetAlphaMap(i)->GetBuffer(), texture._GetAlphaMap(i)->GetBufferByteSize());
			}

			if (merge_color_and_alpha == true) {
				for (i = 0; i < num_levels; i++) {
					texture._GetColorMap(i)->ConvertTo32BitWithAlpha(*texture._GetAlphaMap(i));
				}
			}
		}

		// Load normal map.
		if (file_header.CheckMapFlag(kNormalMap) == true) {
			for (i = 0; i < num_levels; i++) {
				// The last two mip map levels are stored raw.
				if (i < (num_levels - 2) && file_header.compression_flag_ == 1)
					ReadJpeg(*texture._GetNormalMap(i));
				else
					load_file_->ReadData(texture._GetNormalMap(i)->GetBuffer(), texture._GetNormalMap(i)->GetBufferByteSize());
			}
		}

		// Load specular map.
		if (file_header.CheckMapFlag(kSpecularMap) == true) {
			for (i = 0; i < num_levels; i++) {
				// The last two mip map levels are stored raw.
				if (i < (num_levels - 2) && file_header.compression_flag_ == 1)
					ReadJpeg(*texture._GetSpecularMap(i));
				else
					load_file_->ReadData(texture._GetSpecularMap(i)->GetBuffer(), texture._GetSpecularMap(i)->GetBufferByteSize());
			}
		}
	} else {
		int i;
		int num_levels = file_header.num_mip_map_levels_;

		// Create all buffers.
		texture.Prepare(false,
						  false,
						  false,
						  false,
						  true,
						  file_header.width_,
						  file_header.height_);

		// Jump to the color data.
		load_file_->SeekSet(file_header.data_offset_);

		// Load cube map.
		for (i = 0; i < num_levels; i++) {
			// The last two mip map levels are stored raw.
			if (i < (num_levels - 2) && file_header.compression_flag_ == 1)
				ReadJpeg(*texture._GetCubeMapPosX(i));
			else
				load_file_->ReadData(texture._GetCubeMapPosX(i)->GetBuffer(), texture._GetCubeMapPosX(i)->GetBufferByteSize());
		}

		for (i = 0; i < num_levels; i++) {
			// The last two mip map levels are stored raw.
			if (i < (num_levels - 2) && file_header.compression_flag_ == 1)
				ReadJpeg(*texture._GetCubeMapNegX(i));
			else
				load_file_->ReadData(texture._GetCubeMapNegX(i)->GetBuffer(), texture._GetCubeMapNegX(i)->GetBufferByteSize());
		}

		for (i = 0; i < num_levels; i++) {
			// The last two mip map levels are stored raw.
			if (i < (num_levels - 2) && file_header.compression_flag_ == 1)
				ReadJpeg(*texture._GetCubeMapPosY(i));
			else
				load_file_->ReadData(texture._GetCubeMapPosY(i)->GetBuffer(), texture._GetCubeMapPosY(i)->GetBufferByteSize());
		}

		for (i = 0; i < num_levels; i++) {
			// The last two mip map levels are stored raw.
			if (i < (num_levels - 2) && file_header.compression_flag_ == 1)
				ReadJpeg(*texture._GetCubeMapNegY(i));
			else
				load_file_->ReadData(texture._GetCubeMapNegY(i)->GetBuffer(), texture._GetCubeMapNegY(i)->GetBufferByteSize());
		}

		for (i = 0; i < num_levels; i++) {
			// The last two mip map levels are stored raw.
			if (i < (num_levels - 2) && file_header.compression_flag_ == 1)
				ReadJpeg(*texture._GetCubeMapPosZ(i));
			else
				load_file_->ReadData(texture._GetCubeMapPosZ(i)->GetBuffer(), texture._GetCubeMapPosZ(i)->GetBufferByteSize());
		}

		for (i = 0; i < num_levels; i++) {
			// The last two mip map levels are stored raw.
			if (i < (num_levels - 2) && file_header.compression_flag_ == 1)
				ReadJpeg(*texture._GetCubeMapNegZ(i));
			else
				load_file_->ReadData(texture._GetCubeMapNegZ(i)->GetBuffer(), texture._GetCubeMapNegZ(i)->GetBufferByteSize());
		}
	}

	load_file_->Close();

	return kStatusSuccess;
}

TEXLoader::IoStatus TEXLoader::Save(const Texture& texture, bool compressed) {
	if (texture.GetColorMap(0) == 0) {
		return kStatusMissingColormapError;
	}

	// Calculate log2 of width and height.
	int width = 0;
	int height = 0;
	int log2_width = 0;
	int log2_height = 0;

	if (texture.IsCubeMap() == false) {
		width  = texture.GetColorMap(0)->GetWidth();
		height = texture.GetColorMap(0)->GetHeight();
	} else {
		width  = texture.GetCubeMapPosX(0)->GetWidth();
		height = texture.GetCubeMapPosX(0)->GetHeight();
	}

	while ((1 << log2_width) < width) {
		log2_width++;
	}
	while ((1 << log2_height) < height) {
		log2_height++;
	}

	// Prepare file header.
	FileHeader file_header;
	file_header.tex_magic_[0] = 'T';
	file_header.tex_magic_[1] = 'T';
	file_header.tex_magic_[2] = 'E';
	file_header.tex_magic_[3] = 'X';

	file_header.version_ = 1;
	file_header.data_offset_ = 16; // Size of file header.
	file_header.dimension_powers_ = (uint8)((log2_height << 4) | log2_width);

	file_header.compression_flag_ = compressed == true ? 1 : 0;
	file_header.map_flags_ = 0;

	if (texture.IsCubeMap() == false) {
		if (texture.GetAlphaMap(0) != 0 ||
		   texture.GetColorMap(0)->GetBitDepth() == Canvas::kBitdepth32Bit)
			file_header.map_flags_ |= kAlphaMap;
		if (texture.GetNormalMap(0) != 0)
			file_header.map_flags_ |= kNormalMap;
		if (texture.GetSpecularMap(0) != 0)
			file_header.map_flags_ |= kSpecularMap;
	} else {
		file_header.map_flags_ = kCubeMap;
	}

	// Write the file header.
	save_file_->WriteData(file_header.tex_magic_, 4);
	save_file_->Write(file_header.version_);
	save_file_->Write(file_header.data_offset_);
	save_file_->Write(file_header.dimension_powers_);
	save_file_->Write(file_header.compression_flag_);
	save_file_->Write(file_header.map_flags_);

	int num_levels = texture.GetNumMipMapLevels();

	if (texture.IsCubeMap() == false) {
		int i;
		for (i = 0; i < num_levels; i++) {
			if (texture.GetColorMap(i)->GetBitDepth() != Canvas::kBitdepth24Bit) {
				Canvas temp(*texture.GetColorMap(i), true);
				temp.ConvertBitDepth(Canvas::kBitdepth24Bit);

				if (i < (num_levels - 2) && compressed == true)
					WriteJpeg(temp);
				else
					save_file_->WriteData(temp.GetBuffer(), temp.GetBufferByteSize());
			} else {
				if (i < (num_levels - 2) && compressed == true)
					WriteJpeg(*texture.GetColorMap(i));
				else
					save_file_->WriteData(texture.GetColorMap(i)->GetBuffer(), texture.GetColorMap(i)->GetBufferByteSize());
			}
		}

		if (texture.GetAlphaMap(0) != 0) {
			for (int i = 0; i < texture.GetNumMipMapLevels(); i++) {
				if (i < (num_levels - 2) && compressed == true)
					WriteJpeg(*texture.GetAlphaMap(i));
				else
					save_file_->WriteData(texture.GetAlphaMap(i)->GetBuffer(), texture.GetAlphaMap(i)->GetBufferByteSize());
			}
		} else if(texture.GetColorMap(0)->GetBitDepth() == Canvas::kBitdepth32Bit) {
			for (i = 0; i < texture.GetNumMipMapLevels(); i++) {
				Canvas alpha_map;
				texture.GetColorMap(i)->GetAlphaChannel(alpha_map);

				if (i < (num_levels - 2) && compressed == true)
					WriteJpeg(alpha_map);
				else
					save_file_->WriteData(alpha_map.GetBuffer(), alpha_map.GetBufferByteSize());
			}
		}

		if (texture.GetNormalMap(0) != 0) {
			for (i = 0; i < texture.GetNumMipMapLevels(); i++) {
				if (i < (num_levels - 2) && compressed == true)
					WriteJpeg(*texture.GetNormalMap(i));
				else
					save_file_->WriteData(texture.GetNormalMap(i)->GetBuffer(), texture.GetNormalMap(i)->GetBufferByteSize());
			}
		}

		if (texture.GetSpecularMap(0) != 0) {
			for (i = 0; i < texture.GetNumMipMapLevels(); i++) {
				if (i < (num_levels - 2) && compressed == true)
					WriteJpeg(*texture.GetSpecularMap(i));
				else
					save_file_->WriteData(texture.GetSpecularMap(i)->GetBuffer(), texture.GetSpecularMap(i)->GetBufferByteSize());
			}
		}
	} else {
		int i;
		for (i = 0; i < texture.GetNumMipMapLevels(); i++) {
			if (i < (num_levels - 2) && compressed == true)
				WriteJpeg(*texture.GetCubeMapPosX(i));
			else
				save_file_->WriteData(texture.GetCubeMapPosX(i)->GetBuffer(), texture.GetCubeMapPosX(i)->GetBufferByteSize());
		}

		for (i = 0; i < texture.GetNumMipMapLevels(); i++) {
			if (i < (num_levels - 2) && compressed == true)
				WriteJpeg(*texture.GetCubeMapNegX(i));
			else
				save_file_->WriteData(texture.GetCubeMapNegX(i)->GetBuffer(), texture.GetCubeMapNegX(i)->GetBufferByteSize());
		}

		for (i = 0; i < texture.GetNumMipMapLevels(); i++) {
			if (i < (num_levels - 2) && compressed == true)
				WriteJpeg(*texture.GetCubeMapPosY(i));
			else
				save_file_->WriteData(texture.GetCubeMapPosY(i)->GetBuffer(), texture.GetCubeMapPosY(i)->GetBufferByteSize());
		}

		for (i = 0; i < texture.GetNumMipMapLevels(); i++) {
			if (i < (num_levels - 2) && compressed == true)
				WriteJpeg(*texture.GetCubeMapNegY(i));
			else
				save_file_->WriteData(texture.GetCubeMapNegY(i)->GetBuffer(), texture.GetCubeMapNegY(i)->GetBufferByteSize());
		}

		for (i = 0; i < texture.GetNumMipMapLevels(); i++) {
			if (i < (num_levels - 2) && compressed == true)
				WriteJpeg(*texture.GetCubeMapPosZ(i));
			else
				save_file_->WriteData(texture.GetCubeMapPosZ(i)->GetBuffer(), texture.GetCubeMapPosZ(i)->GetBufferByteSize());
		}

		for (i = 0; i < texture.GetNumMipMapLevels(); i++) {
			if (i < (num_levels - 2) && compressed == true)
				WriteJpeg(*texture.GetCubeMapNegZ(i));
			else
				save_file_->WriteData(texture.GetCubeMapNegZ(i)->GetBuffer(), texture.GetCubeMapNegZ(i)->GetBufferByteSize());
		}
	}

	save_file_->Close();

	return kStatusSuccess;
}



}
