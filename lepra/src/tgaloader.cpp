
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/tgaloader.h"
#include "../include/graphics2d.h"
#include "../include/canvas.h"
#include "../include/diskfile.h"
#include "../include/metafile.h"
#include "../include/memfile.h"
#include "../include/archivefile.h"



namespace lepra {



TgaLoader::Status TgaLoader::Load(const str& file_name, Canvas& canvas) {
	Status status = kStatusSuccess;
	MetaFile _file;
	if (_file.Open(file_name, MetaFile::kReadOnly, false, Endian::kTypeLittleEndian) == false) {
		status = kStatusOpenError;
	}

	if (status == kStatusSuccess) {
		status = Load(_file, canvas);
		_file.Close();
	}
	return status;
}

TgaLoader::Status TgaLoader::Save(const str& file_name, const Canvas& canvas) {
	Status status = kStatusSuccess;
	DiskFile _file;
	if (_file.Open(file_name, DiskFile::kModeWrite, false, Endian::kTypeLittleEndian) == false) {
		status = kStatusOpenError;
	}

	if (status == kStatusSuccess) {
		status = Save(_file, canvas);
		_file.Close();
	}
	return status;
}

TgaLoader::Status TgaLoader::Load(const str& archive_name, const str& file_name, Canvas& canvas) {
	Status status = kStatusSuccess;
	ArchiveFile _file(archive_name);
	if (_file.Open(file_name, ArchiveFile::kReadOnly, Endian::kTypeLittleEndian) == false) {
		status = kStatusOpenError;
	}

	if (status == kStatusSuccess) {
		status = Load(_file, canvas);
		_file.Close();
	}
	return status;
}

TgaLoader::Status TgaLoader::Save(const str& archive_name, const str& file_name, const Canvas& canvas) {
	Status status = kStatusSuccess;
	ArchiveFile _file(archive_name);
	if (_file.Open(file_name, ArchiveFile::kWriteOnly, Endian::kTypeLittleEndian) == false) {
		status = kStatusOpenError;
	}

	if (status == kStatusSuccess) {
		status = Save(_file, canvas);
		_file.Close();
	}
	return status;
}

TgaLoader::Status TgaLoader::Load(Reader& reader, Canvas& canvas) {
	int64 file_size = reader.GetAvailable();
	if (file_size <= 0) {
		return kStatusUnknownfilesizeError;
	}

	enum {
		kBufferSize = 512,
	};

	//
	// Read the entire file into memory.
	//
	MemFile mem_file;

	uint8* buffer[kBufferSize];
	unsigned num_chunks = (unsigned)file_size / kBufferSize;
	unsigned rest = (unsigned)file_size % kBufferSize;
	unsigned i;

	for (i = 0; i < num_chunks; i++) {
		if (reader.ReadData(buffer, kBufferSize) != kIoOk)
			return kStatusReadstreamError;
		mem_file.WriteData(buffer, kBufferSize);
	}

	if (rest > 0) {
		if (reader.ReadData(buffer, rest) != kIoOk)
			return kStatusReadstreamError;
		mem_file.WriteData(buffer, rest);
	}

	// Now actually load the file.
	return Load(mem_file, canvas);
}

TgaLoader::Status TgaLoader::Load(File& file, Canvas& canvas) {
	// Determine the file format by reading the file footer.
	FileFooter footer;
	file.SeekEnd(-26);
	file.Read(footer.extension_area_offset_);
	file.Read(footer.developer_directory_offset_);
	file.ReadData(footer.signature_, 18);
	file.SeekSet(0);

	TGAFormat format = kOriginalTgaFormat;
	if (strcmp(footer.signature_, truevision_x_file_) == 0) {
		format = kNewTgaFormat;
	}

	// Read the file header.
	TGAFileHeader _file_header;
	file.Read(_file_header.id_length_);
	file.Read(_file_header.color_map_type_);
	file.Read(_file_header.image_type_);
	file.Read(_file_header.color_map_spec_.first_entry_index_);
	file.Read(_file_header.color_map_spec_.color_map_length_);
	file.Read(_file_header.color_map_spec_.color_map_entry_size_);
	file.Read(_file_header.image_spec_.x_origin_);
	file.Read(_file_header.image_spec_.y_origin_);
	file.Read(_file_header.image_spec_.image_width_);
	file.Read(_file_header.image_spec_.image_height_);
	file.Read(_file_header.image_spec_.pixel_depth_);
	file.Read(_file_header.image_spec_.image_descriptor_);

	if (_file_header.id_length_ != 0) {
		// Skip image ID. This is just identification info about the image.
		file.SeekCur((int64)_file_header.id_length_);
	}

	// Read the palette, if any.
	Color* _palette = 0;
	if (_file_header.color_map_type_ != 0) {
		// Don't allow less than 256 entries in the palette.
		int palette_entries = _file_header.color_map_spec_.color_map_length_ >= 256 ? _file_header.color_map_spec_.color_map_length_ : 256;
		_palette = new Color[palette_entries];

		switch(_file_header.color_map_spec_.color_map_entry_size_) {
		case 15:
		case 16: {	// If 16-bit, it's still stored as 15-bit, according to the manual...
				for (int i = _file_header.color_map_spec_.first_entry_index_;
					i < _file_header.color_map_spec_.color_map_length_;
					i++) {
					uint16 color;
					file.Read(color);

					_palette[i].red_   = (uint8)(((color >> 10) & 31) << 3);
					_palette[i].green_ = (uint8)(((color >> 5) & 31) << 3);
					_palette[i].blue_  = (uint8)((color & 31) << 3);
				}
			} break;
		case 24: {
				for (int i = _file_header.color_map_spec_.first_entry_index_;
					i < _file_header.color_map_spec_.color_map_length_;
					i++) {
					file.Read(_palette[i].blue_);
					file.Read(_palette[i].green_);
					file.Read(_palette[i].red_);
				}
			} break;
		case 32: {
				for (int i = _file_header.color_map_spec_.first_entry_index_;
					i < _file_header.color_map_spec_.color_map_length_;
					i++) {
					file.Read(_palette[i].blue_);
					file.Read(_palette[i].green_);
					file.Read(_palette[i].red_);
					file.Read(_palette[i].alpha_);
				}
			} break;
		default:
			delete[] _palette;
			return kStatusReadPaletteError;
			break;
		};
	}

	// And now it's time for the image data itself.

	Status return_value = kStatusReadPictureError;

	switch(_file_header.image_type_) {
	case kUncompressedColormapImage:
		return_value = LoadUncompressedColorMapImage(canvas, _file_header, file, _palette);
		break;
	case kUncompressedTruecolorImage:
		return_value = LoadUncompressedTrueColorImage(canvas, _file_header, file);
		break;
	case kUncompressedBlackandwhiteImage:
		return_value = LoadUncompressedBlackAndWhiteImage(canvas, _file_header, file);
		break;
	case kRleColormappedImage:
		return_value = LoadRLEColorMapImage(canvas, _file_header, file, _palette);
		break;
	case kRleTruecolorImage:
		return_value = LoadRLETrueColorImage(canvas, _file_header, file);
		break;
	case kRleBlackandwhiteImage:
		return_value = LoadRLEBlackAndWhiteImage(canvas, _file_header, file);
		break;
	};

	if (_palette != 0) {
		delete[] _palette;
	}

	return return_value;
}

TgaLoader::Status TgaLoader::Save(Writer& file, const Canvas& canvas) {
	// Write the file header.
	TGAFileHeader _file_header;
	_file_header.id_length_ = 0;
	_file_header.color_map_type_ = (canvas.GetBitDepth() == Canvas::kBitdepth8Bit) ? 1 : 0;
	_file_header.image_type_ = (uint8)(canvas.GetBitDepth() == Canvas::kBitdepth8Bit ? kUncompressedColormapImage : kUncompressedTruecolorImage);
	_file_header.color_map_spec_.first_entry_index_ = 0;
	_file_header.color_map_spec_.color_map_length_ = (canvas.GetBitDepth() == Canvas::kBitdepth8Bit) ? 256 : 0;
	_file_header.color_map_spec_.color_map_entry_size_ = (canvas.GetBitDepth() == Canvas::kBitdepth8Bit) ? 24 : 0;
	_file_header.image_spec_.x_origin_ = 0;
	_file_header.image_spec_.y_origin_ = 0;
	_file_header.image_spec_.image_width_ = (uint16)canvas.GetWidth();
	_file_header.image_spec_.image_height_ = (uint16)canvas.GetHeight();
	_file_header.image_spec_.pixel_depth_ = (uint8)Canvas::BitDepthToInt(canvas.GetBitDepth());
	_file_header.image_spec_.image_descriptor_ = 0;

	if (_file_header.image_spec_.pixel_depth_ == 15) {
		_file_header.image_spec_.pixel_depth_ = 16;
	}

	file.Write(_file_header.id_length_);
	file.Write(_file_header.color_map_type_);
	file.Write(_file_header.image_type_);
	file.Write(_file_header.color_map_spec_.first_entry_index_);
	file.Write(_file_header.color_map_spec_.color_map_length_);
	file.Write(_file_header.color_map_spec_.color_map_entry_size_);
	file.Write(_file_header.image_spec_.x_origin_);
	file.Write(_file_header.image_spec_.y_origin_);
	file.Write(_file_header.image_spec_.image_width_);
	file.Write(_file_header.image_spec_.image_height_);
	file.Write(_file_header.image_spec_.pixel_depth_);
	file.Write(_file_header.image_spec_.image_descriptor_);

	// Write the palette.
	if (canvas.GetBitDepth() == Canvas::kBitdepth8Bit) {
		for (int i = 0; i < 256; i++) {
			file.Write(canvas.GetPalette()[i].blue_);
			file.Write(canvas.GetPalette()[i].green_);
			file.Write(canvas.GetPalette()[i].red_);
		}
	}

	if (canvas.GetBitDepth() == Canvas::kBitdepth16Bit) {
		// We have to convert it to 15 bit (well, at least that's what Adobe Photoshop expects).
		Canvas _canvas(canvas, true);
		_canvas.ConvertBitDepth(Canvas::kBitdepth15Bit);

		// Write the image data.
		for (int y = canvas.GetHeight() - 1; y >= 0; y--) {
			int y_offset = y * _canvas.GetPitch() * _canvas.GetPixelByteSize();
			file.WriteData(((uint8*)_canvas.GetBuffer()) + y_offset, _canvas.GetWidth() * _canvas.GetPixelByteSize());
		}
	} else {
		// Write the image data.
		for (int y = canvas.GetHeight() - 1; y >= 0; y--) {
			int y_offset = y * canvas.GetPitch() * canvas.GetPixelByteSize();
			file.WriteData(((uint8*)canvas.GetBuffer()) + y_offset, canvas.GetWidth() * canvas.GetPixelByteSize());
		}
	}

	FileFooter footer;
	footer.extension_area_offset_ = 0;
	footer.developer_directory_offset_ = 0;

	file.Write(footer.extension_area_offset_);
	file.Write(footer.developer_directory_offset_);
	file.WriteData(truevision_x_file_, (unsigned)::strlen(truevision_x_file_) + 1);

	return kStatusSuccess;
}

TgaLoader::Status TgaLoader::LoadUncompressedColorMapImage(Canvas& canvas, TGAFileHeader& file_header, File& file, Color* palette) {
	if (palette == 0) {
		return kStatusReadPaletteError;
	}

	bool left_to_right = (file_header.image_spec_.image_descriptor_ & 16) == 0;
	bool top_to_bottom = (file_header.image_spec_.image_descriptor_ & 32) != 0;

	if (file_header.color_map_spec_.color_map_length_ <= 256) {
		canvas.Reset(file_header.image_spec_.image_width_,
					   file_header.image_spec_.image_height_,
					   Canvas::IntToBitDepth(file_header.image_spec_.pixel_depth_));
		canvas.SetPalette(palette);
	} else {
		// Reset image to same pixel depth as the palette, if there are more than 256 colors.
		canvas.Reset(file_header.image_spec_.image_width_,
					   file_header.image_spec_.image_height_,
					   Canvas::IntToBitDepth(file_header.color_map_spec_.color_map_entry_size_));
	}

	canvas.CreateBuffer();

	switch(file_header.image_spec_.pixel_depth_) {
	case 8: {
			uint8* data = (uint8*)canvas.GetBuffer();

			int y;
			for (y = 0; y < file_header.image_spec_.image_height_; y++) {
				int y_offset = 0;
				if (top_to_bottom == true) {
					y_offset = y * file_header.image_spec_.image_width_;
				} else {
					y_offset = (file_header.image_spec_.image_height_ - (y + 1)) * file_header.image_spec_.image_width_;
				}

				if (left_to_right == true) {
					file.ReadData(data + y_offset, file_header.image_spec_.image_width_);
				} else {
					for (int x = file_header.image_spec_.image_width_ - 1; x >= 0; x--) {
						file.Read(data[y_offset + x]);
					}
				}
			}
		} break;
	case 15:
	case 16: {
			switch(canvas.GetBitDepth()) {
			case Canvas::kBitdepth15Bit:
			case Canvas::kBitdepth16Bit:
			case Canvas::kBitdepth24Bit:
			case Canvas::kBitdepth32Bit:
				// OK...
				break;
			default:
				return kStatusReadPaletteError;
			};

			uint16* data = new uint16[file_header.image_spec_.image_width_ * file_header.image_spec_.image_height_];

			int y;
			for (y = 0; y < file_header.image_spec_.image_height_; y++) {
				int y_offset = 0;
				if (top_to_bottom == true) {
					y_offset = y * file_header.image_spec_.image_width_;
				} else {
					y_offset = (file_header.image_spec_.image_height_ - (y + 1)) * file_header.image_spec_.image_width_;
				}

				if (left_to_right == true) {
					for (int x = 0; x < file_header.image_spec_.image_width_; x++) {
						file.Read(data[y_offset + x]);
					}
				} else {
					for (int x = file_header.image_spec_.image_width_ - 1; x >= 0; x--) {
						file.Read(data[y_offset + x]);
					}
				}
			}

			// Convert this image to a "true color image".
			int image_size = file_header.image_spec_.image_width_ * file_header.image_spec_.image_height_;
			for (int i = 0; i < image_size; i++) {
				uint16 index = data[i];
				unsigned r = (unsigned)palette[index].red_;
				unsigned g = (unsigned)palette[index].green_;
				unsigned b = (unsigned)palette[index].blue_;

				// Write one pixel.
				switch(canvas.GetBitDepth()) {
				case Canvas::kBitdepth15Bit: {
						uint16 color = (uint16)(((r >> 3) << 10) | ((g >> 3) << 5) | (b >> 3));
						((uint16*)canvas.GetBuffer())[i] = color;
					} break;
				case Canvas::kBitdepth16Bit: {
						uint16 color = (uint16)(((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3));
						((uint16*)canvas.GetBuffer())[i] = color;
					} break;
				case Canvas::kBitdepth24Bit: {
						((uint8*)canvas.GetBuffer())[i * 3 + 0] = (uint8)b;
						((uint8*)canvas.GetBuffer())[i * 3 + 1] = (uint8)g;
						((uint8*)canvas.GetBuffer())[i * 3 + 2] = (uint8)r;
					} break;
				case Canvas::kBitdepth32Bit:
						((uint8*)canvas.GetBuffer())[i * 4 + 0] = (uint8)b;
						((uint8*)canvas.GetBuffer())[i * 4 + 1] = (uint8)g;
						((uint8*)canvas.GetBuffer())[i * 4 + 2] = (uint8)r;
						((uint8*)canvas.GetBuffer())[i * 4 + 3] = 0;
					break;
				}
			}

			delete[] data;
		} break;
	default:
		return kStatusReadPictureError;
	};

	return kStatusSuccess;
}

TgaLoader::Status TgaLoader::LoadUncompressedTrueColorImage(Canvas& canvas, TGAFileHeader& file_header, File& file) {
	bool left_to_right = (file_header.image_spec_.image_descriptor_ & 16) == 0;
	bool top_to_bottom = (file_header.image_spec_.image_descriptor_ & 32) != 0;

	int pixel_depth = file_header.image_spec_.pixel_depth_;
	if (pixel_depth == 16) {
		// Change this to 15 bit, since that's what Adobe Photoshop _means_ with "16".
		pixel_depth = 15;
	}

	canvas.Reset(file_header.image_spec_.image_width_,
				   file_header.image_spec_.image_height_,
				   Canvas::IntToBitDepth(pixel_depth));
	canvas.CreateBuffer();

	switch(file_header.image_spec_.pixel_depth_) {
	case 8:
	case 24:
	case 32: {
			uint8* data = (uint8*)canvas.GetBuffer();

			int y;
			for (y = 0; y < file_header.image_spec_.image_height_; y++) {
				int y_offset = 0;
				if (top_to_bottom == true) {
					y_offset = y * file_header.image_spec_.image_width_ * canvas.GetPixelByteSize();
				} else {
					y_offset = (file_header.image_spec_.image_height_ - (y + 1)) * file_header.image_spec_.image_width_ * canvas.GetPixelByteSize();
				}

				if (left_to_right == true) {
					file.ReadData(data + y_offset, file_header.image_spec_.image_width_ * canvas.GetPixelByteSize());
				} else {
					for (int x = file_header.image_spec_.image_width_ - 1; x >= 0; x--) {
						file.ReadData(data + y_offset + x * canvas.GetPixelByteSize(), canvas.GetPixelByteSize());
					}
				}
			}
		} break;
	case 15:
	case 16:
			uint16* data = (uint16*)canvas.GetBuffer();

			int y;
			for (y = 0; y < file_header.image_spec_.image_height_; y++) {
				int y_offset = 0;
				if (top_to_bottom == true) {
					y_offset = y * file_header.image_spec_.image_width_;
				} else {
					y_offset = (file_header.image_spec_.image_height_ - (y + 1)) * file_header.image_spec_.image_width_;
				}

				if (left_to_right == true) {
					for (int x = 0; x < file_header.image_spec_.image_width_; x++) {
						file.Read(data[y_offset + x]);
					}
				} else {
					for (int x = file_header.image_spec_.image_width_ - 1; x >= 0; x--) {
						file.Read(data[y_offset + x]);
					}
				}
			} break;
	};

	return kStatusSuccess;
}

TgaLoader::Status TgaLoader::LoadUncompressedBlackAndWhiteImage(Canvas& canvas, TGAFileHeader& file_header, File& file) {
	if (file_header.image_spec_.pixel_depth_ != 8) {
		return kStatusReadPictureError;
	}

	canvas.Reset(file_header.image_spec_.image_width_,
				   file_header.image_spec_.image_height_,
				   Canvas::kBitdepth8Bit);

	canvas.CreateBuffer();

	Color _palette[256];
	for (int i = 0; i < 256; i++) {
		_palette[i].red_   = (uint8)i;
		_palette[i].green_ = (uint8)i;
		_palette[i].blue_  = (uint8)i;
		_palette[i].alpha_ = (uint8)i;
	}
	canvas.SetPalette(_palette);

	bool left_to_right = (file_header.image_spec_.image_descriptor_ & 16) == 0;
	bool top_to_bottom = (file_header.image_spec_.image_descriptor_ & 32) != 0;

	uint8* data = (uint8*)canvas.GetBuffer();

	int y;
	for (y = 0; y < file_header.image_spec_.image_height_; y++) {
		int y_offset = 0;
		if (top_to_bottom == true) {
			y_offset = y * file_header.image_spec_.image_width_;
		} else {
			y_offset = (file_header.image_spec_.image_height_ - (y + 1)) * file_header.image_spec_.image_width_;
		}

		if (left_to_right == true) {
			file.ReadData(data + y_offset, file_header.image_spec_.image_width_);
		} else {
			for (int x = file_header.image_spec_.image_width_ - 1; x >= 0; x--) {
				file.ReadData(data + y_offset + x, 1);
			}
		}
	}

	return kStatusSuccess;
}

TgaLoader::Status TgaLoader::LoadRLEColorMapImage(Canvas& canvas, TGAFileHeader& file_header, File& file, Color* palette) {
	if (palette == 0) {
		return kStatusReadPaletteError;
	}

	bool left_to_right = (file_header.image_spec_.image_descriptor_ & 16) == 0;
	bool top_to_bottom = (file_header.image_spec_.image_descriptor_ & 32) != 0;

	if (file_header.color_map_spec_.color_map_length_ <= 256) {
		canvas.Reset(file_header.image_spec_.image_width_,
					   file_header.image_spec_.image_height_,
					   Canvas::IntToBitDepth(file_header.image_spec_.pixel_depth_));
		canvas.SetPalette(palette);
	} else {
		// Reset image to same pixel depth as the palette, if there are more than 256 colors.
		canvas.Reset(file_header.image_spec_.image_width_,
					   file_header.image_spec_.image_height_,
					   Canvas::IntToBitDepth(file_header.color_map_spec_.color_map_entry_size_));
	}

	canvas.CreateBuffer();

	switch(file_header.image_spec_.pixel_depth_) {
	case 8: {
			uint8* data = (uint8*)canvas.GetBuffer();

			int y;
			for (y = 0; y < file_header.image_spec_.image_height_; y++) {
				int y_offset = 0;
				if (top_to_bottom == true) {
					y_offset = y * file_header.image_spec_.image_width_;
				} else {
					y_offset = (file_header.image_spec_.image_height_ - (y + 1)) * file_header.image_spec_.image_width_;
				}

				int x_add = (left_to_right == true) ? 1 : -1;
				for (int x = (left_to_right == true) ? 0 : (file_header.image_spec_.image_width_ - 1);
					(left_to_right == true) ? (x < file_header.image_spec_.image_width_) : x >= 0;) {
					uint8 repetition_count;
					file.Read(repetition_count);

					bool rle_packet = (repetition_count & 0x80) != 0;
					repetition_count &= 0x7F;
					repetition_count++;

					if (rle_packet == true) {
						// RLE packet.
						uint8 pixel_value;
						file.Read(pixel_value);

						for (int count = 0; count < (int)repetition_count; count++) {
							data[y_offset + x] = pixel_value;
							x += x_add;
						}
					} else {
						// Raw packet.
						for (int count = 0; count < (int)repetition_count; count++) {
							file.Read(data[y_offset + x]);
							x += x_add;
						}
					}
				}
			}
		} break;
	case 15:
	case 16: {
			switch(canvas.GetBitDepth()) {
			case Canvas::kBitdepth15Bit:
			case Canvas::kBitdepth16Bit:
			case Canvas::kBitdepth24Bit:
			case Canvas::kBitdepth32Bit:
				// OK...
				break;
			default:
				return kStatusReadPaletteError;
			};

			uint16* data = new uint16[file_header.image_spec_.image_width_ * file_header.image_spec_.image_height_];

			int y;
			for (y = 0; y < file_header.image_spec_.image_height_; y++) {
				int y_offset = 0;
				if (top_to_bottom == true) {
					y_offset = y * file_header.image_spec_.image_width_;
				} else {
					y_offset = (file_header.image_spec_.image_height_ - (y + 1)) * file_header.image_spec_.image_width_;
				}

				int x_add = (left_to_right == true) ? 1 : -1;
				for (int x = (left_to_right == true) ? 0 : (file_header.image_spec_.image_width_ - 1);
					(left_to_right == true) ? (x < file_header.image_spec_.image_width_) : x >= 0;) {
					uint8 repetition_count;
					file.Read(repetition_count);

					bool rle_packet = (repetition_count & 0x80) != 0;
					repetition_count &= 0x7F;
					repetition_count++;

					if (rle_packet == true) {
						// RLE packet.
						uint16 pixel_value;
						file.Read(pixel_value);

						for (int count = 0; count < (int)repetition_count; count++) {
							data[y_offset + x] = pixel_value;
							x += x_add;
						}
					} else {
						// Raw packet.
						for (int count = 0; count < (int)repetition_count; count++) {
							file.Read(data[y_offset + x]);
							x += x_add;
						}
					}
				}
			}

			// Convert this image to a "true color image".
			int image_size = file_header.image_spec_.image_width_ * file_header.image_spec_.image_height_;
			for (int i = 0; i < image_size; i++) {
				uint16 index = data[i];
				unsigned r = (unsigned)palette[index].red_;
				unsigned g = (unsigned)palette[index].green_;
				unsigned b = (unsigned)palette[index].blue_;

				// Write one pixel.
				switch(canvas.GetBitDepth()) {
				case Canvas::kBitdepth15Bit: {
						uint16 color = (uint16)(((r >> 3) << 10) | ((g >> 3) << 5) | (b >> 3));
						((uint16*)canvas.GetBuffer())[i] = color;
					} break;
				case Canvas::kBitdepth16Bit: {
						uint16 color = (uint16)(((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3));
						((uint16*)canvas.GetBuffer())[i] = color;
					} break;
				case Canvas::kBitdepth24Bit: {
						((uint8*)canvas.GetBuffer())[i * 3 + 0] = (uint8)b;
						((uint8*)canvas.GetBuffer())[i * 3 + 1] = (uint8)g;
						((uint8*)canvas.GetBuffer())[i * 3 + 2] = (uint8)r;
					} break;
				case Canvas::kBitdepth32Bit:
						((uint8*)canvas.GetBuffer())[i * 4 + 0] = (uint8)b;
						((uint8*)canvas.GetBuffer())[i * 4 + 1] = (uint8)g;
						((uint8*)canvas.GetBuffer())[i * 4 + 2] = (uint8)r;
						((uint8*)canvas.GetBuffer())[i * 4 + 3] = 0;
					break;
				}
			}

			delete[] data;
		} break;
	default:
		return kStatusReadPictureError;
	};

	return kStatusSuccess;
}

TgaLoader::Status TgaLoader::LoadRLETrueColorImage(Canvas& canvas, TGAFileHeader& file_header, File& file) {
	bool left_to_right = (file_header.image_spec_.image_descriptor_ & 16) == 0;
	bool top_to_bottom = (file_header.image_spec_.image_descriptor_ & 32) != 0;

	int pixel_depth = file_header.image_spec_.pixel_depth_;
	if (pixel_depth == 16) {
		// Change this to 15 bit, since that's what Adobe Photoshop _means_ with "16".
		pixel_depth = 15;
	}

	canvas.Reset(file_header.image_spec_.image_width_,
				   file_header.image_spec_.image_height_,
				   Canvas::IntToBitDepth(pixel_depth));
	canvas.CreateBuffer();

	switch(file_header.image_spec_.pixel_depth_) {
	case 8: {
			uint8* data = (uint8*)canvas.GetBuffer();

			int y;
			for (y = 0; y < file_header.image_spec_.image_height_; y++) {
				int y_offset = 0;
				if (top_to_bottom == true) {
					y_offset = y * file_header.image_spec_.image_width_;
				} else {
					y_offset = (file_header.image_spec_.image_height_ - (y + 1)) * file_header.image_spec_.image_width_;
				}

				int x_add = (left_to_right == true) ? 1 : -1;
				for (int x = (left_to_right == true) ? 0 : (file_header.image_spec_.image_width_ - 1);
					(left_to_right == true) ? (x < file_header.image_spec_.image_width_) : x >= 0;) {
					uint8 repetition_count;
					file.Read(repetition_count);

					bool rle_packet = (repetition_count & 0x80) != 0;
					repetition_count &= 0x7F;
					repetition_count++;

					if (rle_packet == true) {
						// RLE packet.
						uint8 pixel_value;
						file.Read(pixel_value);

						for (int count = 0; count < (int)repetition_count; count++) {
							data[y_offset + x] = pixel_value;
							x += x_add;
						}
					} else {
						// Raw packet.
						for (int count = 0; count < (int)repetition_count; count++) {
							if (file.Read(data[y_offset + x]) != kIoOk) {
								return (kStatusReadstreamError);	// TRICKY: RAII!
							}
							x += x_add;
						}
					}
				}
			}
		} break;
	case 15:
	case 16: {
			uint16* data = (uint16*)canvas.GetBuffer();

			int y;
			for (y = 0; y < file_header.image_spec_.image_height_; y++) {
				int y_offset = 0;
				if (top_to_bottom == true) {
					y_offset = y * file_header.image_spec_.image_width_;
				} else {
					y_offset = (file_header.image_spec_.image_height_ - (y + 1)) * file_header.image_spec_.image_width_;
				}

				int x_add = (left_to_right == true) ? 1 : -1;
				for (int x = (left_to_right == true) ? 0 : (file_header.image_spec_.image_width_ - 1);
					(left_to_right == true) ? (x < file_header.image_spec_.image_width_) : x >= 0;) {
					uint8 repetition_count;
					file.Read(repetition_count);

					bool rle_packet = (repetition_count & 0x80) != 0;
					repetition_count &= 0x7F;
					repetition_count++;

					if (rle_packet == true) {
						// RLE packet.
						uint16 pixel_value;
						file.Read(pixel_value);

						for (int count = 0; count < (int)repetition_count; count++) {
							data[y_offset + x] = pixel_value;
							x += x_add;
						}
					} else {
						// Raw packet.
						for (int count = 0; count < (int)repetition_count; count++) {
							if (file.Read(data[y_offset + x]) != kIoOk) {
								return (kStatusReadstreamError);	// TRICKY: RAII!
							}
							x += x_add;
						}
					}
				}
			}
		} break;
	case 24: {
			uint8* data = (uint8*)canvas.GetBuffer();

			int y;
			for (y = 0; y < file_header.image_spec_.image_height_; y++) {
				int y_offset = 0;
				if (top_to_bottom == true) {
					y_offset = y * file_header.image_spec_.image_width_ * 3;
				} else {
					y_offset = (file_header.image_spec_.image_height_ - (y + 1)) * file_header.image_spec_.image_width_ * 3;
				}

				int x_add = (left_to_right == true) ? 1 : -1;
				for (int x = (left_to_right == true) ? 0 : (file_header.image_spec_.image_width_ - 1);
					(left_to_right == true) ? (x < file_header.image_spec_.image_width_) : x >= 0;) {
					uint8 repetition_count;
					file.Read(repetition_count);

					bool rle_packet = (repetition_count & 0x80) != 0;
					repetition_count &= 0x7F;
					repetition_count++;

					if (rle_packet == true) {
						// RLE packet.
						uint8 pixel_value[3];
						file.ReadData(&pixel_value, 3);

						for (int count = 0; count < (int)repetition_count; count++) {
							data[y_offset + x * 3 + 0] = pixel_value[0];
							data[y_offset + x * 3 + 1] = pixel_value[1];
							data[y_offset + x * 3 + 2] = pixel_value[2];

							x += x_add;
						}
					} else {
						// Raw packet.
						for (int count = 0; count < (int)repetition_count; count++) {
							if (file.ReadData(&data[y_offset + x * 3], 3) != kIoOk) {
								return (kStatusReadstreamError);	// TRICKY: RAII!
							}
							x += x_add;
						}
					}
				}
			}
		} break;
	case 32: {
			uint8* data = (uint8*)canvas.GetBuffer();

			int y;
			for (y = 0; y < file_header.image_spec_.image_height_; y++) {
				int y_offset = 0;
				if (top_to_bottom == true) {
					y_offset = y * file_header.image_spec_.image_width_ * 4;
				} else {
					y_offset = (file_header.image_spec_.image_height_ - (y + 1)) * file_header.image_spec_.image_width_ * 4;
				}

				int x_add = (left_to_right == true) ? 1 : -1;
				for (int x = (left_to_right == true) ? 0 : (file_header.image_spec_.image_width_ - 1);
					(left_to_right == true) ? (x < file_header.image_spec_.image_width_) : x >= 0;) {
					uint8 repetition_count;
					file.Read(repetition_count);

					bool rle_packet = (repetition_count & 0x80) != 0;
					repetition_count &= 0x7F;
					repetition_count++;

					if (rle_packet == true) {
						// RLE packet.
						uint8 bgra[4];
						file.ReadData(&bgra, 4);

						for (int count = 0; count < (int)repetition_count; count++) {
							// Jonte: RLE stores color as kBgr.
							data[y_offset + x * 4 + 0] = bgra[2];
							data[y_offset + x * 4 + 1] = bgra[1];
							data[y_offset + x * 4 + 2] = bgra[0];
							data[y_offset + x * 4 + 3] = bgra[3];

							x += x_add;
						}
					} else {
						// Raw packet.
						for (int count = 0; count < (int)repetition_count; count++) {
							uint8 bgra[4];
							if (file.ReadData(bgra, sizeof(bgra)) != kIoOk) {
								return (kStatusReadstreamError);	// TRICKY: RAII!
							}
							data[y_offset + x * 4 + 0] = bgra[2];
							data[y_offset + x * 4 + 1] = bgra[1];
							data[y_offset + x * 4 + 2] = bgra[0];
							data[y_offset + x * 4 + 3] = bgra[3];
							x += x_add;
						}
					}
				}
			}
		} break;
	};

	return kStatusSuccess;
}

TgaLoader::Status TgaLoader::LoadRLEBlackAndWhiteImage(Canvas& canvas, TGAFileHeader& file_header, File& file) {
	if (file_header.image_spec_.pixel_depth_ != 8) {
		return kStatusReadPictureError;
	}

	canvas.Reset(file_header.image_spec_.image_width_,
				   file_header.image_spec_.image_height_,
				   Canvas::kBitdepth8Bit);
	canvas.CreateBuffer();

	Color _palette[256];
	for (int i = 0; i < 256; i++) {
		_palette[i].red_   = (uint8)i;
		_palette[i].green_ = (uint8)i;
		_palette[i].blue_  = (uint8)i;
		_palette[i].alpha_ = (uint8)i;
	}
	canvas.SetPalette(_palette);

	bool left_to_right = (file_header.image_spec_.image_descriptor_ & 16) == 0;
	bool top_to_bottom = (file_header.image_spec_.image_descriptor_ & 32) != 0;

	uint8* data = (uint8*)canvas.GetBuffer();

	int y;
	for (y = 0; y < file_header.image_spec_.image_height_; y++) {
		int y_offset = 0;
		if (top_to_bottom == true) {
			y_offset = y * file_header.image_spec_.image_width_;
		} else {
			y_offset = (file_header.image_spec_.image_height_ - (y + 1)) * file_header.image_spec_.image_width_;
		}

		int x_add = (left_to_right == true) ? 1 : -1;
		for (int x = (left_to_right == true) ? 0 : (file_header.image_spec_.image_width_ - 1);
			(left_to_right == true) ? (x < file_header.image_spec_.image_width_) : x >= 0;) {
			uint8 repetition_count;
			file.Read(repetition_count);

			bool rle_packet = (repetition_count & 0x80) != 0;
			repetition_count &= 0x7F;
			repetition_count++;

			if (rle_packet == true) {
				// RLE packet.
				uint8 pixel_value;
				file.Read(pixel_value);

				for (int count = 0; count < (int)repetition_count; count++) {
					data[y_offset + x] = pixel_value;
					x += x_add;
				}
			} else {
				// Raw packet.
				for (int count = 0; count < (int)repetition_count; count++) {
					file.Read(data[y_offset + x]);
					x += x_add;
				}
			}
		}
	}

	return kStatusSuccess;
}



const char* TgaLoader::truevision_x_file_ = "TRUEVISION-XFILE.";



}
