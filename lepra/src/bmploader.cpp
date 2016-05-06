/*
	Class:  BmpLoader
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#include "pch.h"
#include <math.h>
#include "../include/bmploader.h"
#include "../include/canvas.h"
#include "../include/diskfile.h"
#include "../include/metafile.h"
#include "../include/archivefile.h"

namespace lepra {

BmpLoader::Status BmpLoader::Load(const str& file_name, Canvas& canvas) {
	Status status = kStatusSuccess;
	MetaFile file;

	if (file.Open(file_name, MetaFile::kReadOnly, false, Endian::kTypeLittleEndian) == false) {
		status = kStatusOpenError;
	}

	if (status == kStatusSuccess) {
		Load(file, canvas);
		file.Close();
	}

	return status;
}

BmpLoader::Status BmpLoader::Save(const str& file_name, const Canvas& canvas) {
	Status status = kStatusSuccess;
	DiskFile file;

	if (file.Open(file_name, DiskFile::kModeWrite, false, Endian::kTypeLittleEndian) == false) {
		status = kStatusOpenError;
	}

	if (status == kStatusSuccess) {
		Save(file, canvas);
		file.Close();
	}

	return status;
}

BmpLoader::Status BmpLoader::Load(const str& archive_name, const str& file_name, Canvas& canvas) {
	Status status = kStatusSuccess;
	ArchiveFile file(archive_name);

	if (file.Open(file_name, ArchiveFile::kReadOnly, Endian::kTypeLittleEndian) == false) {
		status = kStatusOpenError;
	}

	if (status == kStatusSuccess) {
		Load(file, canvas);
		file.Close();
	}

	return status;
}

BmpLoader::Status BmpLoader::Save(const str& archive_name, const str& file_name, const Canvas& canvas) {
	Status status = kStatusSuccess;
	ArchiveFile file(archive_name);

	if (file.Open(file_name, ArchiveFile::kWriteOnly, Endian::kTypeLittleEndian) == false) {
		status = kStatusOpenError;
	}

	if (status == kStatusSuccess) {
		Save(file, canvas);
		file.Close();
	}

	return status;
}

BmpLoader::Status BmpLoader::Load(Reader& reader, Canvas& canvas) {
	BitmapFileHeader file_header;
	BitmapInfoHeader info_header;

	// Load the file header data:
	if (file_header.Load(&reader) == false) {
		return kStatusReadHeaderError;
	}

	// Load the information header data:
	if (info_header.Load(&reader) == false) {
		return kStatusReadInfoError;
	}

	int color_count = (int) (1 << info_header.bit_count_);
	int bit_depth   = info_header.bit_count_;

	if (bit_depth <= 8) {
		Color palette[256];

		for (int i = 0; i < color_count; i++) {
			if (reader.ReadData(&palette[i], sizeof(Color)) != kIoOk) {
				return kStatusReadPaletteError;
			}
		}

		canvas.SetPalette(palette);
	}

	int width	= info_header.width_;
	int height	= info_header.height_;

	canvas.Reset(abs(width), abs(height), Canvas::IntToBitDepth(bit_depth));
	canvas.CreateBuffer();

	// The ScanWidth is a multiple by 4.
	int scan_width = ((width * (bit_depth >> 3)) + 3) & (~3);
	int scan_padding	= scan_width - (width * (bit_depth >> 3));

	reader.Skip(file_header.off_bits_ - (int)reader.GetReadCount());
	//file.SeekSet(file_header.off_bits_);

	uint8* image = (uint8*)canvas.GetBuffer();

	// Load the bitmap:
	for (int y = 0; y < abs(height); y++) {
		unsigned y_offset;

		// Load a scan-line:
		switch(info_header.compression_) {
			case kCompBiRgb:

				// Uncompressed image:
				if (height < 0) {
					y_offset = y * width * (bit_depth >> 3);
				} else {
					//YPos = ((-height_ - Y) * (width_ * (bit_depth_ >> 3))) - (width_ * (bit_depth_ >> 3));
					y_offset = (height - (y + 1)) * width * (bit_depth >> 3);
				}

				if (reader.ReadData(&image[y_offset], width * (bit_depth >> 3)) != kIoOk) {
					return kStatusReadPictureError;
				} break;
			case kCompBiRle8:
			case kCompBiRle4:
			case kCompBiBitfields:
			default:
				return kStatusCompressionError;
		}
		// Skip the padding possibly located at the end of each
		// scan-line:
		reader.Skip(scan_padding);
	}

	return kStatusSuccess;
}

BmpLoader::Status BmpLoader::Save(Writer& writer, const Canvas& canvas) {
	BitmapFileHeader file_header;
	BitmapInfoHeader info_header;

	file_header.type_ = (((int16)'M') << 8) + (int16)'B';
	file_header.size_ = file_header.GetSize() + info_header.GetSize() +
						   canvas.GetWidth() * canvas.GetHeight() * canvas.GetPixelByteSize();
	file_header.reserved1_ = 0;
	file_header.reserved2_ = 0;
	file_header.off_bits_ = file_header.GetSize() + info_header.GetSize();

	if (canvas.GetBitDepth() == Canvas::kBitdepth8Bit) {
		file_header.size_ += sizeof(Color) * 256;
		file_header.off_bits_ += sizeof(Color) * 256;
	}

	unsigned bit_depth = Canvas::BitDepthToInt(canvas.GetBitDepth());

	info_header.size_			= info_header.GetSize();
	info_header.width_			= canvas.GetWidth();
	info_header.height_			= canvas.GetHeight();
	info_header.planes_			= 1;
	info_header.bit_count_		= (int16)bit_depth;
	info_header.compression_		= kCompBiRgb;
	info_header.size_image_		= canvas.GetWidth() * canvas.GetHeight() * canvas.GetPixelByteSize();
	info_header.x_pels_per_meter_	= 2834;	// Kind'a Photoshop standard, I think.
	info_header.y_pels_per_meter_	= 2834;
	info_header.clr_used_			= 0;
	info_header.clr_important_	= 0;

	if (bit_depth <= 8) {
		info_header.clr_used_			= (1 << bit_depth);
		info_header.clr_important_	= (1 << bit_depth);
	}

	file_header.Save(&writer);
	info_header.Save(&writer);

	int y;

	if (bit_depth <= 8) {
		for (y = 0; y < (1 << bit_depth); y++) {
			writer.WriteData(&canvas.GetPalette()[y], sizeof(Color));
		}
	}

	int scan_width	= ((canvas.GetWidth() * canvas.GetPixelByteSize()) + 3) & (~3);
	int scan_padding	= scan_width - (canvas.GetWidth() * canvas.GetPixelByteSize());
	uint8 pad_bytes[4];

	for (y = canvas.GetHeight() - 1; y >= 0; y--) {
		writer.WriteData((uint8*)canvas.GetBuffer() + y * canvas.GetPitch() * canvas.GetPixelByteSize(),
						 canvas.GetWidth() * canvas.GetPixelByteSize());
		// The ScanWidth must be a multiple by 4.
		writer.WriteData(pad_bytes, scan_padding);
	}

	return kStatusSuccess;
}


/////////////////////////////////////////////////////
//                                                 //
//             Class BitmapFileHeader           //
//                                                 //
/////////////////////////////////////////////////////

int BmpLoader::BitmapFileHeader::GetSize() {
	int size = sizeof(type_) +
					sizeof(size_) +
					sizeof(reserved1_) +
					sizeof(reserved2_) +
					sizeof(off_bits_);

	return size;
}


bool BmpLoader::BitmapFileHeader::Load(Reader* reader) {
	// Load each member one by one... This is done to avoid errors that
	// can occur due to strange compiler settings.
	if (reader->Read(type_) != kIoOk) {
		return false;
	}

	if (reader->Read(size_) != kIoOk) {
		return false;
	}

	if (reader->Read(reserved1_) != kIoOk) {
		return false;
	}

	if (reader->Read(reserved2_) != kIoOk) {
		return false;
	}

	if (reader->Read(off_bits_) != kIoOk) {
		return false;
	}

	return true;
}




bool BmpLoader::BitmapFileHeader::Save(Writer* writer) {
	// Save each member one by one... This is done to avoid errors that
	// can occur due to strange compiler settings.
	if (writer->Write(type_) != kIoOk) {
		return false;
	}

	if (writer->Write(size_) != kIoOk) {
		return false;
	}

	if (writer->Write(reserved1_) != kIoOk) {
		return false;
	}

	if (writer->Write(reserved2_) != kIoOk) {
		return false;
	}

	if (writer->Write(off_bits_) != kIoOk) {
		return false;
	}

	return true;
}



/////////////////////////////////////////////////////
//                                                 //
//             Class BitmapInfoHeader           //
//                                                 //
/////////////////////////////////////////////////////



int BmpLoader::BitmapInfoHeader::GetSize() {
	int size = sizeof(size_) +
					sizeof(width_) +
					sizeof(height_) +
					sizeof(planes_) +
					sizeof(bit_count_) +
					sizeof(compression_) +
					sizeof(size_image_) +
					sizeof(x_pels_per_meter_) +
					sizeof(y_pels_per_meter_) +
					sizeof(clr_used_) +
					sizeof(clr_important_);
	return size;
}


bool BmpLoader::BitmapInfoHeader::Load(Reader* reader) {
	// Load each member one by one... This is done to avoid errors that
	// can occur due to strange compiler settings.

	if (reader->Read(size_) != kIoOk) {
		return false;
	}

	if (reader->Read(width_) != kIoOk) {
		return false;
	}

	if (reader->Read(height_) != kIoOk) {
		return false;
	}

	if (reader->Read(planes_) != kIoOk) {
		return false;
	}

	if (reader->Read(bit_count_) != kIoOk) {
		return false;
	}

	if (reader->Read(compression_) != kIoOk) {
		return false;
	}

	if (reader->Read(size_image_) != kIoOk) {
		return false;
	}

	if (reader->Read(x_pels_per_meter_) != kIoOk) {
		return false;
	}

	if (reader->Read(y_pels_per_meter_) != kIoOk) {
		return false;
	}

	if (reader->Read(clr_used_) != kIoOk) {
		return false;
	}

	if (reader->Read(clr_important_) != kIoOk) {
		return false;
	}

	return true;
}




bool BmpLoader::BitmapInfoHeader::Save(Writer* writer) {
	// Save each member one by one... This is done to avoid errors that
	// can occur due to strange compiler settings.

	if (writer->Write(size_) != kIoOk) {
		return false;
	}

	if (writer->Write(width_) != kIoOk) {
		return false;
	}

	if (writer->Write(height_) != kIoOk) {
		return false;
	}

	if (writer->Write(planes_) != kIoOk) {
		return false;
	}

	if (writer->Write(bit_count_) != kIoOk) {
		return false;
	}

	if (writer->Write(compression_) != kIoOk) {
		return false;
	}

	if (writer->Write(size_image_) != kIoOk) {
		return false;
	}

	if (writer->Write(x_pels_per_meter_) != kIoOk) {
		return false;
	}

	if (writer->Write(y_pels_per_meter_) != kIoOk) {
		return false;
	}

	if (writer->Write(clr_used_) != kIoOk) {
		return false;
	}

	if (writer->Write(clr_important_) != kIoOk) {
		return false;
	}

	return true;
}


}
