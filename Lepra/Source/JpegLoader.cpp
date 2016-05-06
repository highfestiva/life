/*
	Class:  JpegLoader
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#include "pch.h"
#include "../../thirdparty/jpeg-6b/jinclude.h"
#include "../../thirdparty/jpeg-6b/jpeglib.h"
#include "../../thirdparty/jpeg-6b/jerror.h"

#define INT32 a_stupid_workaround_that_undefines_a_typedef
#include "../include/archivefile.h"
#include "../include/canvas.h"
#include "../include/diskfile.h"
#include "../include/metafile.h"
#include "../include/graphics2d.h"
#include "../include/jpegloader.h"
#include "../include/progresscallback.h"
#undef INT32

namespace lepra {

/*
	A class used to (locally, within this file) get access to the
	jpeg loader's private members.
*/

class JpegFriend {
public:
	static inline Reader* GetReader(JpegLoader* jpeg_loader) {
		return jpeg_loader->reader_;
	}
	static inline Writer* GetWriter(JpegLoader* jpeg_loader) {
		return jpeg_loader->writer_;
	}
};


/*
	C style source management functions.
*/
void InitSource(j_decompress_ptr c_info);
boolean FillInputBuffer(j_decompress_ptr c_info);
void SkipInputData(j_decompress_ptr c_info, long num_bytes);
void TerminateSource(j_decompress_ptr c_info);

/*
	C style destination management functions.
*/
void InitDestination(j_compress_ptr c_info);
boolean EmptyOutputBuffer(j_compress_ptr c_info);
void TerminateDestination(j_compress_ptr c_info);

/*
	C style source manager struct.
*/
struct SourceManager {
	jpeg_source_mgr source_manager_;
	uint8 io_buffer_[JpegLoader::kIoBufferSize];
	int io_buffer_size_;
	JpegLoader* jpeg_loader_;
};

/*
	C style destination manager struct.
*/
struct DestinationManager {
	jpeg_destination_mgr dest_manager_;
	uint8 io_buffer_[JpegLoader::kIoBufferSize];
	int io_buffer_size_;
	JpegLoader* jpeg_loader_;
};




/*
	Regular JpegLoader member functions.
*/

JpegLoader::JpegLoader() :
	reader_(0),
	writer_(0) {
}

JpegLoader::~JpegLoader() {
}

JpegLoader::Status JpegLoader::Load(const str& file_name, Canvas& canvas, ProgressCallback* progress) {
	Status status = kStatusSuccess;
	MetaFile file;

	if (file.Open(file_name, MetaFile::kReadOnly) == false) {
		status = kStatusOpenError;
	}

	if (status == kStatusSuccess) {
		reader_ = &file;
		status = Load(canvas, progress);
		file.Close();
	}

	return status;
}

JpegLoader::Status JpegLoader::Save(const str& file_name, const Canvas& canvas) {
	Status status = kStatusSuccess;
	DiskFile file;

	if (file.Open(file_name, DiskFile::kModeWrite) == false) {
		status = kStatusOpenError;
	}

	if (status == kStatusSuccess) {
		writer_ = &file;
		status = Save(canvas);
		file.Close();
	}

	return status;
}

JpegLoader::Status JpegLoader::Load(const str& archive_name, const str& file_name, Canvas& canvas, ProgressCallback* progress) {
	Status status = kStatusSuccess;
	ArchiveFile file(archive_name);

	if (file.Open(file_name, ArchiveFile::kReadOnly) == false) {
		status = kStatusOpenError;
	}

	if (status == kStatusSuccess) {
		reader_ = &file;
		status = Load(canvas, progress);
		file.Close();
	}

	return status;
}

JpegLoader::Status JpegLoader::Save(const str& archive_name, const str& file_name, const Canvas& canvas) {
	Status status = kStatusSuccess;
	ArchiveFile file(archive_name);

	if (file.Open(file_name, ArchiveFile::kWriteOnly) == false) {
		status = kStatusOpenError;
	}

	if (status == kStatusSuccess) {
		reader_ = &file;
		status = Save(canvas);
		file.Close();
	}

	return status;
}

JpegLoader::Status JpegLoader::Load(Reader& reader, Canvas& canvas, ProgressCallback* progress) {
	reader_ = &reader;
	return Load(canvas, progress);
}

JpegLoader::Status JpegLoader::Save(Writer& writer, const Canvas& canvas) {
	writer_ = &writer;
	return Save(canvas);
}

JpegLoader::Status JpegLoader::Load(Canvas& canvas, ProgressCallback* progress) {
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
		canvas.Reset(_c_info.output_width, _c_info.output_height, Canvas::kBitdepth8Bit);

		// Create grayscale palette.
		Color palette[256];
		for (int i = 0; i < 256; i++) {
			palette[i].Set(i, i, i, i);
		}

		canvas.SetPalette(palette);
	} else {
		canvas.Reset(_c_info.output_width, _c_info.output_height, Canvas::kBitdepth24Bit);
	}
	canvas.CreateBuffer();
	uint8* buffer = (uint8*)canvas.GetBuffer();
	int row_stride = canvas.GetPitch() * canvas.GetPixelByteSize();
	int scan_lines = canvas.GetHeight();
	if (progress) {
		progress->SetProgressMax(scan_lines-1);
	}

	const int read_rows_at_a_time = 16;
	JSAMPROW offset[read_rows_at_a_time];
	for (int x = 0; x < read_rows_at_a_time; ++x) {
		offset[x] = &buffer[x*row_stride];
	}

	if (progress) {
		progress->SetProgressPos(0);
	}
	for (int i = 0; i < scan_lines;) {
		const int rows = jpeg_read_scanlines(&_c_info, offset, read_rows_at_a_time);
		i += rows;
		if (progress) {
			progress->SetProgressPos(i);
		}
		for (int x = 0; x < read_rows_at_a_time; ++x) {
			offset[x] += row_stride*rows;
		}
	}

	jpeg_finish_decompress(&_c_info);
	jpeg_destroy_decompress(&_c_info);

	canvas.SwapRGBOrder();

	return kStatusSuccess;
}

JpegLoader::Status JpegLoader::Save(const Canvas& canvas) {
	jpeg_compress_struct _c_info;
	jpeg_error_mgr j_err;
	_c_info.err = jpeg_std_error(&j_err);
	jpeg_create_compress(&_c_info);

	InitDestinationManager(&_c_info);

	_c_info.image_width = canvas.GetWidth();
	_c_info.image_height = canvas.GetHeight();
	_c_info.input_components = 3;
	_c_info.in_color_space = JCS_RGB;

	// We must make sure that the image is in 24-bit kRgb mode.
	Canvas copy(canvas, true);
	if (copy.GetBitDepth() != Canvas::kBitdepth24Bit) {
		copy.ConvertBitDepth(Canvas::kBitdepth24Bit);
	}

	copy.SwapRGBOrder();

	jpeg_set_defaults(&_c_info);
	jpeg_start_compress(&_c_info, TRUE);

	uint8* buffer = (uint8*)copy.GetBuffer();
	int row_stride = copy.GetPitch() * copy.GetPixelByteSize();

	JSAMPROW offset[1];
	offset[0] = buffer;

	for (unsigned i = 0; i < copy.GetHeight(); i++) {
		jpeg_write_scanlines(&_c_info, offset, 1);
		offset[0] += row_stride;
	}

	jpeg_finish_compress(&_c_info);
	jpeg_destroy_compress(&_c_info);

	return kStatusSuccess;
}

void JpegLoader::InitSourceManager(j_decompress_ptr c_info) {
	if (c_info->src == NULL) {
		// First time for this JPEG object?
		c_info->src = (jpeg_source_mgr*)new SourceManager;
	}

	SourceManager* __src = (SourceManager*)c_info->src;
	__src->source_manager_.init_source       = InitSource;
	__src->source_manager_.fill_input_buffer = FillInputBuffer;
	__src->source_manager_.skip_input_data   = SkipInputData;
	__src->source_manager_.resync_to_restart = jpeg_resync_to_restart; /* use default method */
	__src->source_manager_.term_source       = TerminateSource;

	// Forces FillInputBuffer() on first read.
	__src->io_buffer_size_ = 0;
	__src->jpeg_loader_ = this;
	__src->source_manager_.bytes_in_buffer = 0;
	__src->source_manager_.next_input_byte = NULL;
}

void JpegLoader::InitDestinationManager(j_compress_ptr c_info) {
	if (c_info->dest == NULL) {
		// First time for this JPEG object?
		c_info->dest = (jpeg_destination_mgr*)new DestinationManager;
	}

	DestinationManager* __dest = (DestinationManager*)c_info->dest;

	__dest->io_buffer_size_ = 0;
	__dest->jpeg_loader_ = this;

	__dest->dest_manager_.init_destination    = InitDestination;
	__dest->dest_manager_.empty_output_buffer = EmptyOutputBuffer;
	__dest->dest_manager_.term_destination    = TerminateDestination;
}


/*
	And here are the C style functions.
*/



void InitSource(j_decompress_ptr c_info) {
	SourceManager* __src = (SourceManager*)c_info->src;
	__src->io_buffer_size_ = 0;
}

boolean FillInputBuffer(j_decompress_ptr c_info) {
	SourceManager* __src = (SourceManager*)c_info->src;

	Reader* _reader = JpegFriend::GetReader(__src->jpeg_loader_);

	int _num_bytes = (int)_reader->GetAvailable();
	if (_num_bytes > JpegLoader::kIoBufferSize) {
		_num_bytes = JpegLoader::kIoBufferSize;
	}

	IOError __err = _reader->ReadData(__src->io_buffer_, _num_bytes);

	if (__err != kIoOk) {
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

void SkipInputData(j_decompress_ptr c_info, long num_bytes) {
	SourceManager* __src = (SourceManager*)c_info->src;

	if (num_bytes > 0) {
		while (num_bytes > (long)__src->source_manager_.bytes_in_buffer) {
			num_bytes -= (long)__src->source_manager_.bytes_in_buffer;
			FillInputBuffer(c_info);
		}

		__src->source_manager_.next_input_byte += (size_t)num_bytes;
		__src->source_manager_.bytes_in_buffer -= (size_t)num_bytes;
	}
}

void TerminateSource(j_decompress_ptr c_info) {
	SourceManager* __src = (SourceManager*)c_info->src;
	__src->io_buffer_size_ = 0;
	__src->jpeg_loader_ = 0;
	delete __src;
}






void InitDestination(j_compress_ptr c_info) {
	DestinationManager* __dest = (DestinationManager*)c_info->dest;

	__dest->dest_manager_.next_output_byte = __dest->io_buffer_;
	__dest->dest_manager_.free_in_buffer = JpegLoader::kIoBufferSize;
}


boolean EmptyOutputBuffer(j_compress_ptr c_info) {
	DestinationManager* __dest = (DestinationManager*)c_info->dest;

	Writer* _writer = JpegFriend::GetWriter(__dest->jpeg_loader_);
	_writer->WriteData(__dest->io_buffer_, JpegLoader::kIoBufferSize);

	__dest->dest_manager_.next_output_byte = __dest->io_buffer_;
	__dest->dest_manager_.free_in_buffer = JpegLoader::kIoBufferSize;

	return TRUE;
}

void TerminateDestination(j_compress_ptr c_info) {
	DestinationManager* __dest = (DestinationManager*)c_info->dest;
	size_t data_count = JpegLoader::kIoBufferSize - __dest->dest_manager_.free_in_buffer;

	/* Write any data remaining in the buffer */
	if (data_count > 0) {
		JpegFriend::GetWriter(__dest->jpeg_loader_)->WriteData(__dest->io_buffer_, (unsigned)data_count);
	}

	__dest->io_buffer_size_ = 0;
	__dest->jpeg_loader_ = 0;
	delete __dest;
}

}
