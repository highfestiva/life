/*
	Class:  TiffLoader
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#include "pch.h"
#include "../include/lepratypes.h"
#include "../include/tiffloader.h"
#include "../include/graphics2d.h"
#include "../include/canvas.h"
#include "../include/diskfile.h"
#include "../include/metafile.h"
#include "../include/memfile.h"
#include "../include/archivefile.h"

#include <memory.h>

namespace lepra {

TiffLoader::Status TiffLoader::Load(const str& file_name, Canvas& canvas) {
	MetaFile _file;
	Status status = kStatusSuccess;

	if (_file.Open(file_name, MetaFile::kReadOnly) == false) {
		status = kStatusOpenError;
	}

	if (status == kStatusSuccess) {
		status = Load(_file, canvas);
		_file.Close();
	}

	return status;
}

TiffLoader::Status TiffLoader::Save(const str& file_name, const Canvas& canvas) {
	DiskFile _file;
	Status status = kStatusSuccess;

	if (_file.Open(file_name, DiskFile::kModeWrite) == false) {
		status = kStatusOpenError;
	}

	if (status == kStatusSuccess) {
		status = Save(_file, canvas);
		_file.Close();
	}

	return status;
}

TiffLoader::Status TiffLoader::Load(const str& archive_name, const str& file_name, Canvas& canvas) {
	ArchiveFile _file(archive_name);
	Status status = kStatusSuccess;

	if (_file.Open(file_name, ArchiveFile::kReadOnly) == false) {
		status = kStatusOpenError;
	}

	if (status == kStatusSuccess) {
		status = Load(_file, canvas);
		_file.Close();
	}

	return status;
}

TiffLoader::Status TiffLoader::Save(const str& archive_name, const str& file_name,
									const Canvas& canvas) {
	ArchiveFile _file(archive_name);
	Status status = kStatusSuccess;

	if (_file.Open(file_name, ArchiveFile::kWriteOnly) == false) {
		status = kStatusOpenError;
	}

	if (status == kStatusSuccess) {
		status = Save(_file, canvas);
		_file.Close();
	}

	return status;
}

TiffLoader::Status TiffLoader::Load(Reader& reader, Canvas& canvas) {
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

TiffLoader::Status TiffLoader::Save(Writer& writer, const Canvas& canvas) {
	/*
		Write Image File Header (8 bytes)
	*/

	const bool is_little_endian = (writer.GetWriterEndian() == Endian::kTypeLittleEndian);
	uint16 version = 42;
	unsigned ifd_offset = 8; // IFD = Image File Directory.
	writer.WriteData(is_little_endian? "II" : "MM", 2);
	writer.Write(version);
	writer.Write(ifd_offset);

	bool store_color_map = canvas.GetBitDepth() == Canvas::kBitdepth8Bit;

	// IFH = 8 bytes.
	// IFD = 6 + NumDEs * 12 bytes.
	int offset = store_color_map ? 158 : 146;

	// Setup DEs to write.
	DirectoryEntry de[12];

	de[0].tag_ = kTagNewsubfiletype;
	de[0].type_ = kTypeLong;
	de[0].count_ = 1;
	de[0].value_offset_ = 0;

	de[1].tag_ = kTagImagewidth;
	de[1].type_ = canvas.GetWidth() <= 32767? (uint16)kTypeShort : (uint16)kTypeLong;
	de[1].count_ = 1;
	de[1].value_offset_ = (unsigned)canvas.GetWidth();

	de[2].tag_ = kTagImagelength;
	de[2].type_ = canvas.GetHeight() <= 32767? (uint16)kTypeShort : (uint16)kTypeLong;
	de[2].count_ = 1;
	de[2].value_offset_ = (unsigned)canvas.GetHeight();

	de[3].tag_ = kTagBitspersample;
	de[3].type_ = kTypeShort;
	de[3].count_ = store_color_map ? 1 : 3;
	de[3].value_offset_ = de[3].count_ == 1 ? 8 : offset + 0;

	de[4].tag_ = kTagCompression;
	de[4].type_ = kTypeShort;
	de[4].count_ = 1;
	de[4].value_offset_ = kCompressionNone;

	de[5].tag_ = kTagPhotometriclnterp;
	de[5].type_ = kTypeShort;
	de[5].count_ = 1;
	de[5].value_offset_ = store_color_map ? kPiPalette : kPiRgb;

	de[6].tag_ = kTagStripoffsets;
	de[6].type_ = kTypeShort;
	de[6].count_ = 1;
	de[6].value_offset_ = store_color_map ? offset + sizeof(int16) * 768 : offset + sizeof(int16) * 3;

	de[7].tag_ = kTagSamplesperpixel;
	de[7].type_ = kTypeShort;
	de[7].count_ = 1;
	de[7].value_offset_ = store_color_map ? 1 : 3;

	de[8].tag_ = kTagRowsperstrip;
	de[8].type_ = canvas.GetHeight() <= 32767? (uint16)kTypeShort : (uint16)kTypeLong;
	de[8].count_ = 1;
	de[8].value_offset_ = canvas.GetHeight();

	int strip_size = canvas.GetPixelByteSize() * canvas.GetHeight() * canvas.GetWidth();
	de[9].tag_ = kTagStripbytecounts;
	de[9].type_ = strip_size <= 32767? (uint16)kTypeShort : (uint16)kTypeLong;
	de[9].count_ = 1;
	de[9].value_offset_ = strip_size;

	de[10].tag_ = kTagPlanarconfiguration;
	de[10].type_ = kTypeShort;
	de[10].count_ = 1;
	de[10].value_offset_ = kPcChunky;

	de[11].tag_ = kTagColormap;
	de[11].type_ = kTypeShort;
	de[11].count_ = 768;
	de[11].value_offset_ = offset;

	// Write DEs.
	uint16 num_d_es = store_color_map ? 12 : 11;
	writer.Write(num_d_es);
	for (uint16 i = 0; i < num_d_es; i++) {
		writer.Write(de[i].tag_);
		writer.Write(de[i].type_);
		writer.Write(de[i].count_);
		writer.Write(de[i].value_offset_);
	}

	// The end of this (and the only) Image File Directory.
	unsigned next_ifd_offset = 0;
	writer.Write(next_ifd_offset);

	// Now write all data pointed at by the DEs.
	if (de[3].count_ == 3) {
		uint16 eight = 8;
		writer.Write(eight);
		writer.Write(eight);
		writer.Write(eight);
	}

	// Write strip offset and strip.
	if (store_color_map == true) {
		int i;
		for (i = 0; i < 256; i++) {
			uint16 red = (uint16)canvas.GetPalette()[i].red_;
			red <<= 8;
			writer.Write(red);
		}
		for (i = 0; i < 256; i++) {
			uint16 green = (uint16)canvas.GetPalette()[i].green_;
			green <<= 8;
			writer.Write(green);
		}
		for (i = 0; i < 256; i++) {
			uint16 blue = (uint16)canvas.GetPalette()[i].blue_;
			blue <<= 8;
			writer.Write(blue);
		}
	}

	// Write strip (the actual image data).

	if (store_color_map == false) {
		// Need to swap kRgb.
		Canvas copy(canvas, true);
		copy.ConvertBitDepth(Canvas::kBitdepth24Bit);
		copy.SwapRGBOrder();
		unsigned row_size = copy.GetPixelByteSize() * copy.GetWidth();
		unsigned pitch = copy.GetPixelByteSize() * copy.GetPitch();
		uint8* src = (uint8*)copy.GetBuffer();

		Color color;
		for (unsigned y = 0; y < copy.GetHeight(); y++) {
			writer.WriteData(src, row_size);
			src += pitch;
		}
	} else {
		// No need to swap anything...
		unsigned row_size = canvas.GetPixelByteSize() * canvas.GetWidth();
		unsigned pitch = canvas.GetPixelByteSize() * canvas.GetPitch();
		uint8* src = (uint8*)canvas.GetBuffer();
		for (unsigned y = 0; y < canvas.GetHeight(); y++) {
			writer.WriteData(src, row_size);
			src += pitch;
		}
	}

	return kStatusSuccess;
}

TiffLoader::Status TiffLoader::Load(File& file, Canvas& canvas) {
	/*
		Read Image File Header (8 bytes)
	*/

	uint8 buffer[2];

	// First two bytes tells us which byte order to use. M = Motorola, I = Intel.
	file.ReadData(buffer, 2);
	if (buffer[0] == 'M' &&
	   buffer[1] == 'M') {
		file.SetReaderEndian(Endian::kTypeBigEndian);
	} else if(buffer[0] == 'I' &&
		    buffer[1] == 'I') {
		file.SetReaderEndian(Endian::kTypeLittleEndian);
	} else {
		return kStatusReadHeaderError;
	}

	// The next two bytes should always have the decimal value 42.
	uint16 version;
	file.Read(version);
	if (version != 42) {
		return kStatusReadHeaderError;
	}

	// And finally read the Image File Directory Offset.
	unsigned ifd_offset; // IFD = Image File Directory
	file.Read(ifd_offset);
	file.SeekSet(ifd_offset);

	// Load the first IFD, ignore the others.
	// (One IFD = one image, which means that one tiff file
	// can contain several images)

	Status status = kStatusSuccess;

	bool done = false;
	while (done == false) {
		IFDInfo _ifd_info;
		status = ReadIFD(file, _ifd_info);

		bool pixel_format_ok = _ifd_info.CheckPixelFormat();
		bool no_compression = _ifd_info.compression_ == kCompressionNone;

		if (status != kStatusSuccess) {
			done = true;
		} else if(pixel_format_ok == true && no_compression == true) {
			if (_ifd_info.is_alpha_mask_ == false &&
			   _ifd_info.reduced_res_version_ == false) {
				ReadImage(file, _ifd_info, canvas);
				done = true;
			}
		} else {
			// Check if we can continue and load next IFD.
			if (file.Tell() == 0) {
				// File position has been set to zero, which means
				// that we have reached the end of the file.
				done = true;

				if (pixel_format_ok == false)
					status = kStatusPixelformatError;
				else
					status = kStatusCompressionError;
			}
		}
	}

	return status;
}

TiffLoader::Status TiffLoader::ReadHeader(const str& file_name, Header& header) {
	Status __return = StartLoad(file_name, header);
	EndLoad();
	return __return;
}

TiffLoader::Status TiffLoader::ReadHeader(const str& archive_name, const str& file_name, Header& header) {
	Status __return = StartLoad(archive_name, file_name, header);
	EndLoad();
	return __return;
}

TiffLoader::Status TiffLoader::StartLoad(const str& archive_name, const str& file_name, Header& header) {
	Status status = kStatusSuccess;

	ArchiveFile* _file = new ArchiveFile(archive_name);
	load_file_ = _file;

	if (_file->Open(file_name, ArchiveFile::kReadOnly) == false) {
		status = kStatusOpenError;
	}

	if (status == kStatusSuccess) {
		status = StartLoad(header);
	}

	return status;
}

TiffLoader::Status TiffLoader::StartLoad(const str& file_name, Header& header) {
	Status status = kStatusSuccess;

	DiskFile* _file = new DiskFile;
	load_file_ = _file;

	if (_file->Open(file_name, DiskFile::kModeRead) == false) {
		return kStatusOpenError;
	}

	if (status == kStatusSuccess) {
		status = StartLoad(header);
	}

	return status;
}

TiffLoader::Status TiffLoader::StartLoad(Header& header) {
	//
	//	Read Image File Header (8 bytes)
	//

	uint8 buffer[2];

	// First two bytes tells us which byte order to use. M = Motorola, I = Intel.
	load_file_->ReadData(buffer, 2);
	if (buffer[0] == 'M' &&
	   buffer[1] == 'M') {
		load_file_->SetEndian(Endian::kTypeBigEndian);
	} else if(buffer[0] == 'I' &&
		    buffer[1] == 'I') {
		load_file_->SetEndian(Endian::kTypeLittleEndian);
	} else {
		load_file_->Close();
		return kStatusReadHeaderError;
	}

	// The next two bytes should always have the decimal value 42.
	uint16 version;
	load_file_->Read(version);
	if (version != 42) {
		load_file_->Close();
		return kStatusReadHeaderError;
	}

	// And finally read the Image File Directory Offset.
	unsigned ifd_offset; // IFD = Image File Directory
	load_file_->Read(ifd_offset);
	load_file_->SeekSet(ifd_offset);

	// Load the first IFD, ignore the others.
	// (One IFD = one image, which means that one tiff file
	// can contain several images)

	Status status = kStatusSuccess;

	bool done = false;
	while (done == false) {
		status = ReadIFD(*load_file_, ifd_info_);

		bool pixel_format_ok = ifd_info_.CheckPixelFormat();
		bool no_compression = ifd_info_.compression_ == kCompressionNone;

		if (status != kStatusSuccess) {
			done = true;
		} else if(pixel_format_ok == true && no_compression == true) {
			if (ifd_info_.is_alpha_mask_ == false &&
			   ifd_info_.reduced_res_version_ == false) {
				header.width_  = (int)ifd_info_.width_;
				header.height_ = (int)ifd_info_.height_;
				header.samples_per_pixel_ = (int)ifd_info_.samples_per_pixel_;
				header.bits_per_pixel_ = (int)ifd_info_.bits_per_pixel_;

				done = true;
			}
		} else {
			// Check if we can continue and load next IFD.
			if (load_file_->Tell() == 0) {
				// File position has been set to zero, which means
				// that we have reached the end of the file.
				done = true;

				if (pixel_format_ok == false)
					status = kStatusPixelformatError;
				else
					status = kStatusCompressionError;
			}
		}
	}

	if (status != kStatusSuccess) {
		load_file_->Close();
	}

	return status;
}

TiffLoader::Status TiffLoader::LoadPiece(int left, int top, int right, int bottom, Canvas& canvas) {
	ClipData _clip_data(left, top, right, bottom, (int)ifd_info_.width_, (int)ifd_info_.height_);
	canvas.Reset(abs(right - left), abs(bottom - top), Canvas::IntToBitDepth(ifd_info_.bits_per_pixel_));
	canvas.CreateBuffer();
	memset(canvas.GetBuffer(), 0, canvas.GetHeight() * canvas.GetPitch() * canvas.GetPixelByteSize());

	if (ifd_info_.pi_ == kPiWhiteiszero) {
		// Generate grayscale palette.
		Color palette[256];
		for (int i = 0; i < 256; i++) {
			uint8 col = (uint8)i;
			palette[i].Set(col, col, col, col);
		}

		canvas.SetPalette(palette);
	} else if(ifd_info_.pi_ == kPiBlackiszero) {
		// Generate inverted grayscale palette.
		Color palette[256];
		for (int i = 0; i < 256; i++) {
			uint8 col = 255 - (uint8)i;
			palette[i].Set(col, col, col, col);
		}

		canvas.SetPalette(palette);
	}

	switch(ifd_info_.pi_) {
	case kPiWhiteiszero:
	case kPiBlackiszero:
		if (_clip_data.src_read_width_ == 0 ||
			_clip_data.src_read_height_ == 0)
			return kStatusSuccess;

		return ReadMonochromeImage(_clip_data, canvas);
	case kPiRgb:
		if (_clip_data.src_read_width_ == 0 ||
			_clip_data.src_read_height_ == 0)
			return kStatusSuccess;

		return ReadColorImage(_clip_data, canvas);
	case kPiPalette:
		if (_clip_data.src_read_width_ == 0 ||
			_clip_data.src_read_height_ == 0)
			return kStatusSuccess;

		canvas.SetPalette(ifd_info_.palette_);

		// Monochrome images has the same pixel format as indexed ones...
		return ReadMonochromeImage(_clip_data, canvas);
	default:
		return kStatusReadPictureError;
	}
}

void TiffLoader::EndLoad() {
	load_file_->Close();
}

TiffLoader::Status TiffLoader::ReadIFD(File& file, IFDInfo& ifd_info) {
	uint16 entry_count;
	file.Read(entry_count);

	uint16 i;
	for (i = 0; i < entry_count; i++) {
		// Read directory entry.
		DirectoryEntry de(file);

		Status current_status = kStatusSuccess;

		switch((Tag)de.tag_) {
		case kTagBitspersample: {
				if (de.type_ != kTypeShort) {
					current_status = kStatusReadPictureError;
				} else {
					ifd_info.samples_per_pixel_ = de.count_;
					ifd_info.sample_bits_ = new uint16[ifd_info.samples_per_pixel_];
					ifd_info.bits_per_pixel_ = 0;

					if (ifd_info.samples_per_pixel_ <= 2) {
						ifd_info.sample_bits_[0] = (uint16)(de.value_offset_ & 0xFFFF);
						ifd_info.bits_per_pixel_ += ifd_info.sample_bits_[0];

						if (ifd_info.samples_per_pixel_ == 2) {
							ifd_info.sample_bits_[1] = (uint16)((de.value_offset_ >> 16) & 0xFFFF);
							ifd_info.bits_per_pixel_ += ifd_info.sample_bits_[1];
						}
					} else {
						int64 prev_pos = file.Tell();
						file.SeekSet(de.value_offset_);

						for (unsigned i = 0; i < ifd_info.samples_per_pixel_; i++) {
							file.Read(ifd_info.sample_bits_[i]);
							ifd_info.bits_per_pixel_ += (unsigned)ifd_info.sample_bits_[i];
						}

						file.SeekSet(prev_pos);
					}
				}
			} break;
		case kTagColormap: {
				ifd_info.palette_color_count_ = (uint16)de.count_ / 3;
				ifd_info.palette_ = new Color[ifd_info.palette_color_count_];

				int64 prev_pos = file.Tell();
				file.SeekSet(de.value_offset_);

				uint16 i;
				for (i = 0; i < ifd_info.palette_color_count_; i++) {
					uint16 red;
					file.Read(red);
					ifd_info.palette_[i].red_  = (uint8)(red >> 8);
				}

				for (i = 0; i < ifd_info.palette_color_count_; i++) {
					uint16 green;
					file.Read(green);
					ifd_info.palette_[i].green_  = (uint8)(green >> 8);
				}

				for (i = 0; i < ifd_info.palette_color_count_; i++) {
					uint16 blue;
					file.Read(blue);
					ifd_info.palette_[i].blue_  = (uint8)(blue >> 8);
				}

				file.SeekSet(prev_pos);
			} break;
		case kTagExtrasamples:
			ifd_info.extra_samples_ = de.count_;
			ifd_info.extra_ = (ExtraSamples)de.value_offset_;
			break;
		case kTagCompression:
			ifd_info.compression_ = (Compression)de.value_offset_;
			break;
		case kTagImagelength: {
				if (de.type_ == kTypeShort ||
				   de.type_ == kTypeLong) {
					ifd_info.height_ = de.value_offset_;
				} else {
					// Will initiate a return from this function.
					current_status = kStatusResolutionError;
				}
			} break;
		case kTagImagewidth:
				if (de.type_ == kTypeShort ||
				   de.type_ == kTypeLong) {
					ifd_info.width_ = de.value_offset_;
				} else {
					// Will initiate a return from this function.
					current_status = kStatusResolutionError;
				} break;
		case kTagNewsubfiletype: {
				ifd_info.reduced_res_version_ = CheckBit(de.value_offset_, 0);
				ifd_info.is_single_page_ = CheckBit(de.value_offset_, 1);
				ifd_info.is_alpha_mask_ = CheckBit(de.value_offset_, 2);
			} break;
		case kTagPhotometriclnterp:
			ifd_info.pi_ = (PhotometricInterpretation)de.value_offset_;
			break;
		case kTagPlanarconfiguration:
			ifd_info.planar_config_ = (PlanarConfig)de.value_offset_;

			// Only support chunky data.
			if (ifd_info.planar_config_ != kPcChunky) {
				// Will initiate a return from this function.
				current_status = kStatusReadPictureError;
			} break;
		case kTagPredictor:
			break;
		case kTagRowsperstrip:
			ifd_info.rows_per_strip_ = de.value_offset_;
			break;
		case kTagSamplesperpixel:
			ifd_info.samples_per_pixel_ = de.value_offset_;
			break;
		case kTagStripbytecounts:
			ifd_info.strip_count_ = de.count_;
			ifd_info.strip_byte_counts_ = new unsigned[ifd_info.strip_count_];

			if (ifd_info.strip_count_ == 1) {
				ifd_info.strip_byte_counts_[0] = de.value_offset_;
			} else {
				unsigned i;
				int64 prev_pos = file.Tell();
				file.SeekSet(de.value_offset_);

				if (de.type_ == kTypeShort) {
					uint16 temp;

					for (i = 0; i < ifd_info.strip_count_; i++) {
						file.Read(temp);
						ifd_info.strip_byte_counts_[i] = (unsigned)temp;
					}
				} else if(de.type_ == kTypeLong) {
					for (i = 0; i < ifd_info.strip_count_; i++) {
						file.Read(ifd_info.strip_byte_counts_[i]);
					}
				} else {
					current_status = kStatusReadInfoError;
				}

				file.SeekSet(prev_pos);
			} break;
		case kTagStripoffsets:
			if (ifd_info.strip_count_ == 0) {
				ifd_info.strip_count_ = de.count_;
			} else if(ifd_info.strip_count_ != de.count_) {
				current_status = kStatusReadInfoError;
			}

			ifd_info.strip_offsets_ = new unsigned[ifd_info.strip_count_];

			if (ifd_info.strip_count_ == 1) {
				ifd_info.strip_offsets_[0] = de.value_offset_;
			} else {
				unsigned i;
				int64 prev_pos = file.Tell();
				file.SeekSet(de.value_offset_);

				if (de.type_ == kTypeShort) {
					uint16 temp;

					for (i = 0; i < ifd_info.strip_count_; i++) {
						file.Read(temp);
						ifd_info.strip_offsets_[i] = (unsigned)temp;
					}
				} else if(de.type_ == kTypeLong) {
					for (i = 0; i < ifd_info.strip_count_; i++) {
						file.Read(ifd_info.strip_offsets_[i]);
					}
				} else {
					current_status = kStatusReadInfoError;
				}

				file.SeekSet(prev_pos);
			}

			break;
		case kTagFillorder:
			ifd_info.fill_order_ = (FillOrder)de.value_offset_;
			break;
		case kTagOrientation:
			ifd_info.orientation_ = (Orientation)de.value_offset_;
			break;

		// Ignored tags.
		case kTagMake:
		case kTagCopyright:
		case kTagArtist:
		case kTagDatetime:
		case kTagFreebytecounts:
		case kTagFreeoffsets:
		case kTagGrayresponsecurve:
		case kTagGrayresponseunit:
		case kTagColorresponsecurve:
		case kTagHostcomputer:
		case kTagImagedescription:
		case kTagModel:
		case kTagMaxsamplevalue:
		case kTagMinsamplevalue:
		case kTagResolutionunit:
		case kTagSoftware:
		case kTagThresholding:
		case kTagXresolution:
		case kTagYresolution:
		case kTagSubfiletype:
		case kTagDocumentname:
		case kTagPagename:
		case kTagPagenumber:
		case kTagCelllength:
		case kTagCellwidth:
		case kTagWhitepoint:
		case kTagXposition:
		case kTagYposition:
		case kTagGroup3Options:
		case kTagGroup4Options:
		case kTagPrimarychromaticities:
		default:
			break;
		}

		if (current_status != kStatusSuccess) {
			return current_status;
		}
	}

	// Jump to next IFD.
	unsigned ifd_offset;
	file.Read(ifd_offset);
	file.SeekSet(ifd_offset);

	return kStatusSuccess;
}

TiffLoader::Status TiffLoader::ReadImage(File& file,
										 IFDInfo& ifd_info,
										 Canvas& canvas) {
	switch(ifd_info.pi_) {
	case kPiWhiteiszero:
		return ReadMonochromeImage(file, ifd_info, canvas, true);
	case kPiBlackiszero:
		return ReadMonochromeImage(file, ifd_info, canvas, false);
	case kPiRgb:
		return ReadColorImage(file, ifd_info, canvas);
	case kPiPalette:
		return ReadIndexedImage(file, ifd_info, canvas);
	default:
		return kStatusReadPictureError;
	}
}

TiffLoader::Status TiffLoader::ReadMonochromeImage(File& file,
												   IFDInfo& ifd_info,
												   Canvas& canvas,
												   bool invert) {
	if (ifd_info.samples_per_pixel_ != 1) {
		return kStatusReadPictureError;
	}

	int64 prev_pos = file.Tell();
	int row_length = 0; // Row length in bytes.

	canvas.Reset(ifd_info.width_, ifd_info.height_, Canvas::kBitdepth8Bit);
	canvas.CreateBuffer();
	row_length = ifd_info.width_;

	uint8* dest = (uint8*)canvas.GetBuffer();

	unsigned i;
	for (i = 0; i < ifd_info.strip_count_; i++) {
		file.SeekSet(ifd_info.strip_offsets_[i]);
		for (unsigned y = 0; y < ifd_info.rows_per_strip_; y++) {
			// Read one row.
			file.ReadData(dest, row_length);
			dest += row_length;
		}
	}

	file.SeekSet(prev_pos);

	// Generate grayscale palette.
	Color palette[256];
	for (i = 0; i < 256; i++) {
		uint8 col = (uint8)i;

		if (invert == true) {
			col = 255 - col;
		}

		palette[i].Set(col, col, col, col);
	}

	canvas.SetPalette(palette);

	// Finally rotate or flip image into correct orientation.
	switch(ifd_info.orientation_) {
	case kOrientationTopleft:
		// Do nothing.
		break;
	case kOrientationTopright:
		canvas.FlipHorizontal();
		break;
	case kOrientationBottomright:
		canvas.FlipHorizontal();
		canvas.FlipVertical();
		break;
	case kOrientationBottomleft:
		canvas.FlipVertical();
		break;
	case kOrientationLefttop:
		canvas.FlipVertical();
		canvas.Rotate90DegClockWise(1);
		break;
	case kOrientationRighttop:
		canvas.Rotate90DegClockWise(3);
		break;
	case kOrientationRightbottom:
		canvas.FlipHorizontal();
		canvas.Rotate90DegClockWise(3);
		break;
	case kOrientationLeftbottom:
		canvas.Rotate90DegClockWise(1);
		break;
	default:
		break;
	}

	return kStatusSuccess;
}

TiffLoader::Status TiffLoader::ReadIndexedImage(File& file,
												IFDInfo& ifd_info,
												Canvas& canvas) {
	if (ifd_info.samples_per_pixel_ != 1) {
		return kStatusReadPictureError;
	}

	int64 prev_pos = file.Tell();
	int row_length = 0; // Row length in bytes.

	canvas.Reset(ifd_info.width_, ifd_info.height_, Canvas::kBitdepth8Bit);
	canvas.CreateBuffer();
	row_length = ifd_info.width_;

	uint8* dest = (uint8*)canvas.GetBuffer();

	for (unsigned i = 0; i < ifd_info.strip_count_; i++) {
		file.SeekSet(ifd_info.strip_offsets_[i]);
		for (unsigned y = 0; y < ifd_info.rows_per_strip_; y++) {
			// Read one row.
			file.ReadData(dest, row_length);
			dest += row_length;
		}
	}

	file.SeekSet(prev_pos);

	canvas.SetPalette(ifd_info.palette_);

	// Finally rotate or flip image into correct orientation.
	switch(ifd_info.orientation_) {
	case kOrientationTopleft:
		// Do nothing.
		break;
	case kOrientationTopright:
		canvas.FlipHorizontal();
		break;
	case kOrientationBottomright:
		canvas.FlipHorizontal();
		canvas.FlipVertical();
		break;
	case kOrientationBottomleft:
		canvas.FlipVertical();
		break;
	case kOrientationLefttop:
		canvas.FlipVertical();
		canvas.Rotate90DegClockWise(1);
		break;
	case kOrientationRighttop:
		canvas.Rotate90DegClockWise(3);
		break;
	case kOrientationRightbottom:
		canvas.FlipHorizontal();
		canvas.Rotate90DegClockWise(3);
		break;
	case kOrientationLeftbottom:
		canvas.Rotate90DegClockWise(1);
		break;
	default:
		break;
	}

	return kStatusSuccess;
}

TiffLoader::Status TiffLoader::ReadColorImage(File& file,
											  IFDInfo& ifd_info,
											  Canvas& canvas) {
	if (ifd_info.samples_per_pixel_ < 3) {
		return kStatusReadPictureError;
	}

	int64 prev_pos = file.Tell();

	int row_length = 0; // Row length in bytes.
	canvas.Reset(ifd_info.width_, ifd_info.height_, Canvas::IntToBitDepth(ifd_info.bits_per_pixel_));
	canvas.CreateBuffer();
	row_length = ifd_info.width_ * canvas.GetPixelByteSize();

	uint8* dest = (uint8*)canvas.GetBuffer();

	for (unsigned i = 0; i < ifd_info.strip_count_; i++) {
		file.SeekSet(ifd_info.strip_offsets_[i]);
		for (unsigned y = 0; y < ifd_info.rows_per_strip_; y++) {
			// Read one row.
			file.ReadData(dest, row_length);
			dest += row_length;
		}
	}

	file.SeekSet(prev_pos);

	// Finally rotate or flip image into correct orientation.
	switch(ifd_info.orientation_) {
	case kOrientationTopleft:
		// Do nothing.
		break;
	case kOrientationTopright:
		canvas.FlipHorizontal();
		break;
	case kOrientationBottomright:
		canvas.FlipHorizontal();
		canvas.FlipVertical();
		break;
	case kOrientationBottomleft:
		canvas.FlipVertical();
		break;
	case kOrientationLefttop:
		canvas.FlipVertical();
		canvas.Rotate90DegClockWise(1);
		break;
	case kOrientationRighttop:
		canvas.Rotate90DegClockWise(3);
		break;
	case kOrientationRightbottom:
		canvas.FlipHorizontal();
		canvas.Rotate90DegClockWise(3);
		break;
	case kOrientationLeftbottom:
		canvas.Rotate90DegClockWise(1);
		break;
	default:
		break;
	}

	canvas.SwapRGBOrder();

	return kStatusSuccess;
}

TiffLoader::Status TiffLoader::ReadMonochromeImage(ClipData& clip_data,
												   Canvas& canvas) {
	if (ifd_info_.samples_per_pixel_ != 1) {
		return kStatusReadPictureError;
	}

	int64 prev_pos = load_file_->Tell();

	uint8* dest = (uint8*)canvas.GetBuffer() +
								clip_data.dst_y_offset_ * canvas.GetPitch() +
								clip_data.dst_x_offset_;

	int start_strip = clip_data.src_y_offset_ / ifd_info_.rows_per_strip_;
	int start_line_within_strip = clip_data.src_y_offset_ % ifd_info_.rows_per_strip_;
	int start_offset_within_strip = start_line_within_strip * ifd_info_.width_ + clip_data.src_x_offset_;

	int lines_read = 0;

	unsigned i;
	for (i = start_strip; i < ifd_info_.strip_count_ && lines_read < clip_data.src_read_height_; i++) {
		int file_offset = ifd_info_.strip_offsets_[i] + start_offset_within_strip;

		start_line_within_strip = 0;
		start_offset_within_strip = clip_data.src_x_offset_;

		for (unsigned y = 0; y < ifd_info_.rows_per_strip_ && lines_read < clip_data.src_read_height_; y++) {
			load_file_->SeekSet(file_offset);

			// Read one row.
			if (load_file_->ReadData(dest, clip_data.src_read_width_) == 0) {
				return kStatusReadPictureError;
			}

			dest += canvas.GetPitch();
			file_offset += ifd_info_.width_;
			lines_read++;
		}
	}

	load_file_->SeekSet(prev_pos);

	return kStatusSuccess;
}


TiffLoader::Status TiffLoader::ReadColorImage(ClipData& clip_data,
											  Canvas& canvas) {
	if (ifd_info_.samples_per_pixel_ != 3) {
		return kStatusReadPictureError;
	}

	int64 prev_pos = load_file_->Tell();

	uint8* dest = (uint8*)canvas.GetBuffer() +
								(clip_data.dst_y_offset_ * canvas.GetPitch() +
								clip_data.dst_x_offset_) * 3;

	int start_strip = clip_data.src_y_offset_ / ifd_info_.rows_per_strip_;
	int start_line_within_strip = clip_data.src_y_offset_ % ifd_info_.rows_per_strip_;
	int start_offset_within_strip = (start_line_within_strip * ifd_info_.width_ + clip_data.src_x_offset_) * 3;

	int lines_read = 0;

	unsigned i;
	for (i = start_strip; i < ifd_info_.strip_count_ && lines_read < clip_data.src_read_height_; i++) {
		int file_offset = ifd_info_.strip_offsets_[i] + start_offset_within_strip;

		start_line_within_strip = 0;
		start_offset_within_strip = clip_data.src_x_offset_ * 3;

		for (unsigned y = 0; y < ifd_info_.rows_per_strip_ && lines_read < clip_data.src_read_height_; y++) {
			load_file_->SeekSet(file_offset);

			// Read one row.
			if (load_file_->ReadData(dest, clip_data.src_read_width_ * 3) == 0) {
				return kStatusReadPictureError;
			}

			dest += canvas.GetPitch() * 3;
			file_offset += ifd_info_.width_ * 3;
			lines_read++;
		}
	}

	load_file_->SeekSet(prev_pos);
	canvas.SwapRGBOrder();

	return kStatusSuccess;
}




TiffLoader::DirectoryEntry::DirectoryEntry() :
	tag_(0),
	type_(0),
	count_(0),
	value_offset_(0) {
}

TiffLoader::DirectoryEntry::DirectoryEntry(File& file) {
	file.Read(tag_);
	file.Read(type_);
	file.Read(count_);
	file.Read(value_offset_);

	if (file.GetReaderEndian() == Endian::kTypeBigEndian) {
		// Check if the data fits in ithe value_offset_..
		int type_size = 8; // Too high to fit...
		switch(type_) {
		case kTypeByte:
		case kTypeSbyte:
		case kTypeUndefined:
			type_size = 1;
			break;
		case kTypeShort:
		case kTypeSshort:
			type_size = 2;
			break;
		case kTypeLong:
		case kTypeSlong:
		case kTypeFloat:
			type_size = 4;
			break;
		}

		if (count_ != 0 && type_size * count_ <= 4) {
			// The data fits... Adjust value_offset_ to contain the right value.
			if (type_size == 3) {
				value_offset_ >>= 8;
			} else if(type_size == 2) {
				value_offset_ >>= 16;
			} else if(type_size == 1) {
				value_offset_ >>= 24;
			}
		}
	}
}

TiffLoader::IFDInfo::IFDInfo() :
		reduced_res_version_(false),
		is_single_page_(false),
		is_alpha_mask_(false),
		width_(0),
		height_(0),
		samples_per_pixel_(1),
		sample_bits_(0),
		rows_per_strip_(0xFFFFFFFF),
		strip_byte_counts_(0),
		strip_offsets_(0),
		strip_count_(0),
		extra_samples_(0),
		extra_(kExtraUnassociatedalpha),
		orientation_(kOrientationDefault),
		compression_(kCompressionDefault),
		fill_order_(kFillorderDefault),
		pi_(kPiInvalid),
		planar_config_(kPcChunky),
		palette_(0),
		palette_color_count_(0) {
}

TiffLoader::IFDInfo::~IFDInfo() {
	if (palette_ != 0) {
		delete[] palette_;
	}

	if (strip_byte_counts_ != 0) {
		delete[] strip_byte_counts_;
	}

	if (sample_bits_ != 0) {
		delete[] sample_bits_;
	}
}


bool TiffLoader::IFDInfo::CheckPixelFormat() {
	// Reject all weird pixel formats.
	// All "special" one-of-a-kind
	// only-used-by-one-dude-in-the-whole-wide-world
	// pixel formats can go to hell.

	switch(bits_per_pixel_) {
	case 8:
		if (samples_per_pixel_ != 1)
			return false;
		break;
	case 15:
		if (samples_per_pixel_ != 3)
			return false;

		if (sample_bits_[0] != 5 ||
		sample_bits_[1] != 5 ||
		sample_bits_[2] != 5) {
			return false;
		} break;
	case 16:
		if (samples_per_pixel_ != 3)
			return false;

		if (sample_bits_[0] != 5 ||
		sample_bits_[1] != 6 ||
		sample_bits_[2] != 5) {
			return false;
		} break;
	case 24:
		if (samples_per_pixel_ != 3)
			return false;

		if (sample_bits_[0] != 8 ||
		sample_bits_[1] != 8 ||
		sample_bits_[2] != 8) {
			return false;
		} break;
	case 32:
		if (samples_per_pixel_ != 4)
			return false;

		if (sample_bits_[0] != 8 ||
		sample_bits_[1] != 8 ||
		sample_bits_[2] != 8 ||
		sample_bits_[3] != 8) {
			return false;
		} break;
	default:
		return false;
	}

	// If this is an indexed image, the palette must contain 256 colors.
	if (pi_ == kPiPalette && palette_color_count_ != 256)
		return false;

	// Check extras.
	if (extra_samples_ != 0 && extra_samples_ != 1)
		return false;

	if (extra_samples_ == 1 && extra_ != kExtraUnassociatedalpha)
		return false;

	return true;
}



TiffLoader::ClipData::ClipData(int left, int top,
								  int right, int bottom,
								  int dst_width, int dst_height) {
	if (left > right) {
		int temp = left;
		left = right;
		right = temp;
	}

	if (top > bottom) {
		int temp = top;
		top = bottom;
		bottom = temp;
	}

	if (left > dst_width ||
	   top  > dst_height ||
	   right < 0 || bottom < 0) {
		// No need to read anything.
		src_read_width_ = 0;
		src_read_height_ = 0;
		return;
	}

	dst_x_offset_ = 0;
	dst_y_offset_ = 0;

	src_x_offset_ = left;
	src_y_offset_ = top;

	if (src_x_offset_ < 0) {
		dst_x_offset_ = -src_x_offset_;
		src_x_offset_ = 0;
	}

	if (src_y_offset_ < 0) {
		dst_y_offset_ = -src_y_offset_;
		src_y_offset_ = 0;
	}

	src_read_width_ = right - src_x_offset_;
	src_read_height_ = bottom - src_y_offset_;

	if ((src_x_offset_ + src_read_width_) > dst_width) {
		src_read_width_ = dst_width - src_x_offset_;
	}

	if ((src_y_offset_ + src_read_height_) > dst_height) {
		src_read_height_ = dst_height - src_y_offset_;
	}
}

}
