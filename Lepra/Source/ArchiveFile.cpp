
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/archivefile.h"
#include "../include/path.h"
#include "../include/string.h"

#include <string.h>



namespace lepra {



ArchiveFile::ArchiveFile(const str& archive_name):
	File(Endian::kTypeBigEndian, Endian::kTypeBigEndian, 0, 0),
	archive_file_name_(archive_name),
	is_zip_archive_(false),
	file_handle_(0),
	file_name_(""),
	path_(""),
	file_size_(0),
	data_buffer_(0),
	data_buffer_size_(1024 * 1024), // 1 Mb
	data_size_(0),
	current_db_pos_(0),
	current_pos_(0),
	writer_(0),
	reader_(0),
	mode_(kReadOnly) {
	Reader::SetInputStream(this);
	Writer::SetOutputStream(this);

	data_buffer_ = new uint8[(int)data_buffer_size_];

	is_zip_archive_ = (strutil::CompareIgnoreCase(strutil::Right(archive_file_name_, 4), ".zip") == 0);
}

ArchiveFile::ArchiveFile(const str& archive_name, Reader* reader):
	File(Endian::kTypeBigEndian, Endian::kTypeBigEndian, 0, 0),
	archive_file_name_(archive_name),
	is_zip_archive_(false),
	file_handle_(0),
	file_name_(""),
	path_(""),
	file_size_(0),
	data_buffer_(0),
	data_buffer_size_(1024 * 1024), // 1 Mb
	data_size_(0),
	current_db_pos_(0),
	current_pos_(0),
	writer_(0),
	reader_(reader),
	mode_(kReadOnly) {
	Reader::SetInputStream(this);
	Writer::SetOutputStream(this);

	if (reader_ != 0) {
		reader_->SetInputStream(this);
	}

	data_buffer_ = new uint8[(int)data_buffer_size_];

	is_zip_archive_ = (strutil::CompareIgnoreCase(strutil::Right(archive_file_name_, 4), ".zip") == 0);
}

ArchiveFile::ArchiveFile(const str& archive_name, Writer* writer):
	File(Endian::kTypeBigEndian, Endian::kTypeBigEndian, 0, 0),
	archive_file_name_(archive_name),
	is_zip_archive_(false),
	file_handle_(0),
	file_name_(""),
	path_(""),
	file_size_(0),
	data_buffer_(0),
	data_buffer_size_(1024 * 1024), // 1 Mb
	data_size_(0),
	current_db_pos_(0),
	current_pos_(0),
	writer_(writer),
	reader_(0),
	mode_(kReadOnly) {
	Reader::SetInputStream(this);
	Writer::SetOutputStream(this);

	if (writer_ != 0) {
		writer_->SetOutputStream(this);
	}

	data_buffer_ = new uint8[(int)data_buffer_size_];

	is_zip_archive_ = (strutil::CompareIgnoreCase(strutil::Right(archive_file_name_, 4), ".zip") == 0);
}

ArchiveFile::ArchiveFile(const str& archive_name, Reader* reader, Writer* writer):
	File(Endian::kTypeBigEndian, Endian::kTypeBigEndian, 0, 0),
	archive_file_name_(archive_name),
	is_zip_archive_(false),
	file_handle_(0),
	file_name_(""),
	path_(""),
	file_size_(0),
	data_buffer_(0),
	data_buffer_size_(1024 * 1024), // 1 Mb
	data_size_(0),
	current_db_pos_(0),
	current_pos_(0),
	writer_(writer),
	reader_(reader),
	mode_(kReadOnly) {
	Reader::SetInputStream(this);
	Writer::SetOutputStream(this);

	if (reader_ != 0) {
		reader_->SetInputStream(this);
	}

	if (writer_ != 0) {
		writer_->SetOutputStream(this);
	}

	data_buffer_ = new uint8[(int)data_buffer_size_];

	is_zip_archive_ = (strutil::CompareIgnoreCase(strutil::Right(archive_file_name_, 4), ".zip") == 0);
}

ArchiveFile::~ArchiveFile() {
	Close();

	delete[] data_buffer_;
}

void ArchiveFile::ExtractPathAndFileName(const str& file_name) {
	Path::SplitPath(file_name, path_, file_name_);
	InputStream::SetName(file_name_);
	OutputStream::SetName(file_name_);
}

bool ArchiveFile::Open(const str& file_name, OpenMode mode, Endian::EndianType endian) {
	Close();

	SetEndian(endian);

	bool ok = true;

	if (ok == true) {
		switch(mode) {
		case kReadOnly:
			Parent::SetMode(Parent::kReadMode);
			Parent::ClearMode(Parent::kWriteMode);
			break;
		case kWriteOnly:
		case kWriteAppend:
			Parent::SetMode(Parent::kWriteMode);
			Parent::ClearMode(Parent::kReadMode);
			break;
		default:
			ok = false;
		}
	}

	if (ok == true) {
		ExtractPathAndFileName(file_name);

		switch(mode) {
		case kReadOnly:
			if (is_zip_archive_ == true) {
				ok = OpenZipForReading(file_name, mode);
			} else {
				ok = OpenForReading(file_name, mode);
			} break;
		case kWriteOnly:
		case kWriteAppend:
			if (is_zip_archive_ == true) {
				ok = OpenZipForWriting(file_name, mode);
			} else {
				ok = OpenForWriting(file_name, mode);
			} break;
		};
	}

	return ok;
}

bool ArchiveFile::OpenForReading(const str& file_name, OpenMode mode) {
	bool ok = (archive_.OpenArchive(archive_file_name_, FileArchive::kReadOnly) == kIoOk);

	if (ok == true) {
		file_handle_ = archive_.FileOpen(file_name);
		ok = (file_handle_ != 0);
	}

	if (ok == true) {
		// Get the file size.
		file_size_ = archive_.FileSize(file_handle_);

		mode_ = mode;
		current_pos_ = 0;
		current_db_pos_ = 0;
	}

	return ok;
}

bool ArchiveFile::OpenZipForReading(const str& file_name, OpenMode mode) {
	bool ok = (zip_archive_.OpenArchive(archive_file_name_, ZipArchive::kReadOnly) == kIoOk);

	if (ok == true) {
		ok = zip_archive_.FileOpen(file_name);
	}

	if (ok == true) {
		// Get the file size.
		file_size_ = zip_archive_.FileSize();

		mode_ = mode;
		current_pos_ = 0;
		current_db_pos_ = 0;

		// Treat the file handle as a flag telling wether this file is open.
		file_handle_ = 1;
	}

	return ok;
}

bool ArchiveFile::OpenForWriting(const str& file_name, OpenMode mode) {
	bool ok = true;

	// First check if the file already exist in the archive... If it does,
	// we have to remove it from the archive. In append mode, we have to
	// extract the file first, and then add it to the archive again.

	FileArchive original;
	ok = (original.OpenArchive(archive_file_name_, FileArchive::kReadOnly) == kIoOk);

	str temp_file;

	if (ok == true) {
		if (original.FileExist(file_name) == true) {
			// The file exists in the archive, thus we have to remove or extract it.
			// First generate a temporary file name for the target archive.
			str temp_name = DiskFile::GenerateUniqueFileName(path_);
			ok = (archive_.OpenArchive(temp_name, FileArchive::kWriteOnly) == kIoOk);

			if (ok == true) {
				// Copy all files from the original to the target archive (archive_),
				// except file_name.
				CopyArchiveFiles(original, archive_, file_name);

				if (mode == kWriteAppend) {
					// Now create a temp file and extract the file.
					temp_file = DiskFile::GenerateUniqueFileName(path_);
					ok = original.ExtractFile(file_name, temp_file, 1);
				}

				original.CloseAndRemoveArchive();
				archive_.CloseArchive();
				DiskFile::Rename(temp_name, archive_file_name_);
			} else {
				original.CloseArchive();
			}
		} else {
			original.CloseArchive();
		}
	}

	// At this point we know that the archive doesn't contain the file file_name.
	if (ok == true) {
		ok = (archive_.OpenArchive(archive_file_name_, FileArchive::kWriteAppend) == kIoOk);
	}

	if (ok == true) {
		file_handle_ = archive_.FileOpen(file_name);

		if (mode == kWriteAppend) {
			// We have to take care about the temp file...
			// Insert it into the archive.
			DiskFile file;
			if (file.Open(temp_file, DiskFile::kModeRead, false, Endian::kTypeLittleEndian) == true) {
				uint8 buf[1024];
				uint64 steps = file.GetSize() / 1024;
				uint64 rest  = file.GetSize() % 1024;
				uint64 i;

				for (i = 0; i < steps; i++) {
					file.ReadData(buf, 1024);
					archive_.FileWrite(buf, 1024);
				}

				if (rest > 0) {
					file.ReadData(buf, (size_t)rest);
					archive_.FileWrite(buf, (int)rest);
				}

				file.Close();
				DiskFile::Delete(temp_file);
			}
		}
	}

	if (ok == true) {
		mode_ = mode;
		current_pos_ = 0;
		current_db_pos_ = 0;
	}

	return ok;
}

bool ArchiveFile::OpenZipForWriting(const str& file_name, OpenMode mode) {
	bool ok = true;

	// First check if the file already exist in the archive... If it does,
	// we have to remove it from the archive. In append mode, we have to
	// extract the file first, and then add it to the archive again.

	ZipArchive original;
	ok = (original.OpenArchive(archive_file_name_, ZipArchive::kReadOnly) == kIoOk);

	str temp_file;

	if (ok == true) {
		if (original.FileExist(file_name) == true) {
			// The file exists in the archive, thus we have to remove or extract it.
			// First generate a temporary file name for the target archive.
			str temp_name = DiskFile::GenerateUniqueFileName(path_);
			ok = (zip_archive_.OpenArchive(temp_name, ZipArchive::kWriteOnly) == kIoOk);

			if (ok == true) {
				// Copy all files from the original to the target archive (archive_),
				// except file_name.
				CopyZipArchiveFiles(original, zip_archive_, file_name);

				if (mode == kWriteAppend) {
					// Now create a temp file and extract the file.
					temp_file = DiskFile::GenerateUniqueFileName(path_);
					ok = original.ExtractFile(file_name, temp_file, 1, ZipArchive::kMb);
				}

				original.CloseAndRemoveArchive();
				zip_archive_.CloseArchive();
				//Parent::ExtractPathAndFileName(lTempName,
				DiskFile::Rename(temp_name, archive_file_name_);
			} else {
				original.CloseArchive();
			}
		} else {
			original.CloseArchive();
		}
	}

	// At this point we know that the archive doesn't contain the file file_name.
	if (ok == true) {
		ok = (zip_archive_.OpenArchive(archive_file_name_, ZipArchive::kWriteAppend) == kIoOk);
	}

	if (ok == true) {
		ok = zip_archive_.FileOpen(file_name);
	}

	file_handle_ = 0;

	if (ok == true) {
		file_handle_ = 1;
		mode_ = mode;
		current_pos_ = 0;
		current_db_pos_ = 0;

		if (mode == kWriteAppend) {
			// We have to take care about the temp file...
			// Insert it into the archive.
			DiskFile file;
			if (file.Open(temp_file, DiskFile::kModeRead, false, Endian::kTypeLittleEndian) == true) {
				current_pos_ = file.GetSize();

				uint8 buf[1024];
				uint64 steps = file.GetSize() / 1024;
				uint64 rest  = file.GetSize() % 1024;
				uint64 i;

				for (i = 0; i < steps; i++) {
					file.ReadData(buf, 1024);
					zip_archive_.FileWrite(buf, 1024);
				}

				if (rest > 0) {
					file.ReadData(buf, (size_t)rest);
					zip_archive_.FileWrite(buf, (int)rest);
				}

				file.Close();
				DiskFile::Delete(temp_file);
			}
		}
	}

	return ok;
}

void ArchiveFile::Close() {
	if (IsOpen() == true) {
		FlushDataBuffer();

		if (is_zip_archive_ == true) {
			zip_archive_.FileClose();
			zip_archive_.CloseArchive();
		} else {
			archive_.FileClose(file_handle_);
			archive_.CloseArchive();
		}

		file_handle_ = 0;
	}

	file_name_ = "";
	path_ = "";

	current_db_pos_ = 0;
	data_size_ = 0;
}

void ArchiveFile::CopyArchiveFiles(FileArchive& source, FileArchive& dest, const str& except_this_file) {
	str _file_name = source.FileFindFirst();

	while (_file_name != "") {
		if (strutil::CompareIgnoreCase(except_this_file, _file_name) != 0) {
			CopyFileBetweenArchives(source, dest, _file_name);
		}

		_file_name = source.FileFindNext();
	}
}

void ArchiveFile::CopyZipArchiveFiles(ZipArchive& source, ZipArchive& dest, const str& except_this_file) {
	str _file_name = source.FileFindFirst();

	while (_file_name != "") {
		if (strutil::CompareIgnoreCase(except_this_file, _file_name) != 0) {
			CopyFileBetweenZipArchives(source, dest, _file_name);
		}

		_file_name = source.FileFindNext();
	}
}

bool ArchiveFile::CopyFileBetweenArchives(FileArchive& source, FileArchive& dest, const str& file_name) {
	bool ok = true;

	// Open source file.
	int file_handle = source.FileOpen(file_name);
	int64 file_size = source.FileSize(file_handle);

	ok = file_handle != 0 && dest.FileOpen(file_name) != 0;

	// Open dest file.
	if (ok == true) {
		const int chunk_size = 1024*1024; // 1 kMb
		uint8* data = new uint8[chunk_size];

		int64 num_chunks = file_size / (int64)chunk_size;
		int rest = (int)(file_size % (int64)chunk_size);

		for (int64 i = 0; i < num_chunks; i++) {
			source.FileRead(data, chunk_size, file_handle);
			dest.FileWrite(data, chunk_size);
		}

		if (rest != 0) {
			source.FileRead(data, rest, file_handle);
			dest.FileWrite(data, rest);
		}

		dest.FileClose(0);

		delete[] data;
	}

	if (file_handle != 0) {
		source.FileClose(file_handle);
	}

	return ok;
}

bool ArchiveFile::CopyFileBetweenZipArchives(ZipArchive& source, ZipArchive& dest, const str& file_name) {
	bool ok = true;

	// Open source file.
	bool source_opened = source.FileOpen(file_name);

	// Open dest file.
	ok = (source_opened && dest.FileOpen(file_name));

	int64 file_size = source.FileSize();

	if (ok == true) {
		const int chunk_size = 1024*1024; // 1 kMb
		uint8* data = new uint8[chunk_size];

		int64 num_chunks = file_size / (int64)chunk_size;
		int rest = (int)(file_size % (int64)chunk_size);

		for (int64 i = 0; i < num_chunks; i++) {
			source.FileRead(data, chunk_size);
			dest.FileWrite(data, chunk_size);
		}

		if (rest != 0) {
			source.FileRead(data, rest);
			dest.FileWrite(data, rest);
		}

		dest.FileClose();

		delete[] data;
	}

	if (source_opened == true) {
		source.FileClose();
	}

	return ok;
}


void ArchiveFile::SetEndian(Endian::EndianType endian) {
	Parent::SetEndian(endian);
	if (reader_ != 0) {
		reader_->SetReaderEndian(endian);
	}
	if (writer_ != 0) {
		writer_->SetWriterEndian(endian);
	}
}

str ArchiveFile::GetFullName() const {
	return path_ + "/" + file_name_;
}

str ArchiveFile::GetName() const {
	return file_name_;
}

str ArchiveFile::GetPath() const {
	return path_;
}

bool ArchiveFile::FlushDataBuffer() {
	bool ok = IsOpen();

	if (ok == true) {
		if ((mode_ == kWriteOnly || mode_ == kWriteAppend) && current_db_pos_ > 0) {
			int write_size = current_db_pos_;
			current_db_pos_ = 0;
			data_size_ = 0;

			if (is_zip_archive_ == true) {
				ok = (zip_archive_.FileWrite(data_buffer_, write_size) == kIoOk);
			} else {
				ok = (archive_.FileWrite(data_buffer_, write_size) == kIoOk);
			}
		}
	}

	return ok;
}

bool ArchiveFile::RefillDataBuffer() {
	bool ok = IsOpen();

	if (ok == true) {
		int64 pos = current_pos_;
		int64 read_size = file_size_ - pos;

		if (read_size > (int64)data_buffer_size_) {
			read_size = data_buffer_size_;
		}

		current_db_pos_ = 0;
		data_size_ = (int)read_size;

		if (is_zip_archive_ == true) {
			ok = (zip_archive_.FileRead(data_buffer_, (int)read_size) == kIoOk);
		} else {
			ok = (archive_.FileRead(data_buffer_, (int)read_size, file_handle_) == kIoOk);
		}
	}

	return ok;
}

IOError ArchiveFile::ReadRaw(void* buffer, size_t size) {
	IOError error = kIoOk;

	if (IsOpen() == false) {
		error = kIoStreamNotOpen;
	}

	uint8* _buffer = (uint8*)buffer;
	int read_size = (int)size;
	int index = 0;

	if (error == kIoOk) {
		while ((current_db_pos_ + read_size) > data_size_) {
			// Read what's left in the buffer.
			int _size = data_size_ - current_db_pos_;
			if (_size > 0) {
				memcpy(_buffer + index, data_buffer_ + current_db_pos_, (size_t)_size);

				read_size -= _size;
				index += _size;
				current_pos_ += _size;
			}

			if (RefillDataBuffer() == false) {
				if (read_size != 0)
					error = kIoErrorReadingFromStream;
				break;
			}
		}
	}

	if (error == kIoOk && read_size > 0) {
		memcpy(_buffer + index, data_buffer_ + current_db_pos_, (size_t)read_size);
		current_db_pos_ += read_size;
		current_pos_ += read_size;
	}

	return error;
}

IOError ArchiveFile::Skip(size_t size) {
	return (Parent::Skip(size));
}

IOError ArchiveFile::WriteRaw(const void* buffer, size_t size) {
	IOError error = kIoOk;

	if (IsOpen() == false) {
		error = kIoStreamNotOpen;
	}

	if (error == kIoOk && mode_ != kWriteOnly && mode_ != kWriteAppend) {
		error = kIoInvalidOperation;
	}

	uint8* _buffer = (uint8*)buffer;
	int write_size = (int)size;
	int index = 0;

	if (error == kIoOk) {
		while ((current_db_pos_ + write_size) > data_buffer_size_) {
			// Fill the buffer.
			int _size = data_buffer_size_ - current_db_pos_;
			if (_size > 0) {
				memcpy(data_buffer_ + current_db_pos_, _buffer + index, (size_t)_size);

				write_size -= _size;
				index += _size;
				current_pos_ += _size;
				current_db_pos_ += _size;
			}

			if (FlushDataBuffer() == false) {
				error = kIoErrorWritingToStream;
				break;
			}
		}
	}

	if (error == kIoOk && write_size > 0) {
		memcpy(data_buffer_ + current_db_pos_, _buffer + index, (size_t)write_size);
		current_db_pos_ += write_size;
		current_pos_ += write_size;
	}

	return error;
}

IOError ArchiveFile::ReadData(void* buffer, size_t size) {
	IOError error;
	if (reader_ != 0) {
		error = reader_->ReadData(buffer, size);
	} else {
		error = Reader::ReadData(buffer, size);
	}

	return error;
}

IOError ArchiveFile::WriteData(const void* buffer, size_t size) {
	IOError error;
	if (writer_ != 0) {
		error = writer_->WriteData(buffer, size);
	} else {
		error = Writer::WriteData(buffer, size);
	}
	return error;
}

int64 ArchiveFile::Tell() const {
	int64 pos = current_pos_;

	if (IsOpen() == false) {
		pos = -1;
	}

	return pos;
}

int64 ArchiveFile::Seek(int64 offset, FileOrigin from) {
	bool ok = IsOpen();

	if (ok == true && (mode_ == kWriteOnly || mode_ == kWriteAppend)) {
		FlushDataBuffer();
	}

	if (ok == true && mode_ != kReadOnly) {
		ok = false;
	}

	if (ok == true) {
		// DataBuffer start position (in "file coordinates").
		int64 db_start = (current_pos_ - current_db_pos_);

		switch(from) {
		case kFseekSet:
			current_pos_ = offset;
			break;
		case kFseekCur:
			current_pos_ += offset;
			break;
		case kFseekEnd:
			current_pos_ = (file_size_ + offset);
			break;
		};

		current_db_pos_ = (int)(current_pos_ - db_start);

		if (mode_ == kReadOnly && (current_db_pos_ < 0 || current_db_pos_ >= data_size_)) {
			if (is_zip_archive_ == true) {
				zip_archive_.FileSeek(current_pos_, ZipArchive::kFseekSet);
			} else {
				archive_.FileSeek(current_pos_, FileArchive::kFseekSet, file_handle_);
			}

			if (mode_ == kReadOnly) {
				RefillDataBuffer();
			}
		} else if((mode_ == kWriteOnly || mode_ == kWriteAppend) &&
				current_db_pos_ != 0) {
			if (is_zip_archive_ == true) {
				zip_archive_.FileSeek(current_pos_, ZipArchive::kFseekSet);
			} else {
				archive_.FileSeek(current_pos_, FileArchive::kFseekSet, file_handle_);
			}
			current_db_pos_ = 0;
		}
	}

	int64 pos = -1;
	if (ok == true) {
		pos = Tell();
	}

	return pos;
}



//
// Static functions.
//

bool ArchiveFile::ExtractFileFromArchive(const str& archive_name, const str& file_name, bool local) {
	return ExtractFileFromArchive(archive_name, file_name, file_name, local);
}

bool ArchiveFile::ExtractFileFromArchive(const str& archive_name, const str& file_name, const str& extracted_file_name, bool local) {
	bool ret = false;

	ArchiveFile archived_file(archive_name);
	if (archived_file.Open(file_name, ArchiveFile::kReadOnly, Endian::kTypeLittleEndian) == true) {
		DiskFile disk_file;
		bool opened = false;

		if (local == false) {
			opened = disk_file.Open(extracted_file_name, DiskFile::kModeWrite, true, Endian::kTypeLittleEndian);
		} else {
			str _file_name = Path::GetCompositeFilename(extracted_file_name);
			opened = disk_file.Open(_file_name, DiskFile::kModeWrite, false, Endian::kTypeLittleEndian);
		}

		if (opened == true) {
			int64 file_size = archived_file.GetSize();

			const int chunk_size = 1024 * 1024;
			int64 num_chunks = file_size / (int64)chunk_size;
			int rest = (int)(file_size % (int64)chunk_size);

			uint8* data = new uint8[chunk_size];

			for (int64 i = 0; i < num_chunks; i++) {
				archived_file.ReadData(data, chunk_size);
				disk_file.WriteData(data, chunk_size);
			}

			archived_file.ReadData(data, rest);
			disk_file.WriteData(data, rest);

			delete[]data;
			disk_file.Close();

			ret = true;
		}

		archived_file.Close();
	}

	return ret;
}

bool ArchiveFile::InsertFileIntoArchive(const str& archive_name, const str& file_name, bool local) {
	return InsertFileIntoArchive(archive_name, file_name, file_name, local);
}

bool ArchiveFile::InsertFileIntoArchive(const str& archive_name, const str& file_name, const str& inserted_file_name, bool local) {
	bool ret = false;

	DiskFile disk_file;

	if (disk_file.Open(file_name, DiskFile::kModeRead, false, Endian::kTypeLittleEndian) == true) {
		ArchiveFile archive_file(archive_name);
		bool opened = false;

		if (local == false) {
			opened = archive_file.Open(inserted_file_name, ArchiveFile::kWriteOnly, Endian::kTypeLittleEndian);
		} else {
			str _file_name = Path::GetCompositeFilename(inserted_file_name);
			opened = archive_file.Open(_file_name, ArchiveFile::kWriteOnly, Endian::kTypeLittleEndian);
		}

		if (opened == true) {
			int64 file_size = disk_file.GetSize();

			const int chunk_size = 1024 * 1024;
			int64 num_chunks = file_size / (int64)chunk_size;
			int rest = (int)(file_size % (int64)chunk_size);

			uint8* data = new uint8[chunk_size];

			for (int64 i = 0; i < num_chunks; i++) {
				disk_file.ReadData(data, chunk_size);
				archive_file.WriteData(data, chunk_size);
			}

			disk_file.ReadData(data, rest);
			archive_file.WriteData(data, rest);

			delete[]data;
			archive_file.Close();

			ret = true;
		}

		disk_file.Close();
	}

	return ret;
}


void ArchiveFile::operator=(const ArchiveFile&) {
}



void ArchiveFile::SetArchiveType(ArchiveType type) {
	is_zip_archive_ = (type == kZip);
}

bool ArchiveFile::IsOpen() const {
	return (file_handle_ != 0);
}

int64 ArchiveFile::GetSize() const {
	return file_size_;
}

int64 ArchiveFile::GetAvailable() const {
	return (file_size_ - current_pos_);
}

void ArchiveFile::Flush() {
	FlushDataBuffer();
}



}
