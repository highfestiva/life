
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include <ctype.h>
#include "../include/filearchive.h"
#include "../include/progresscallback.h"
#include "../include/string.h"
#include "../include/stringutility.h"



namespace lepra {



FileArchive::FileArchive() :
	archive_file_name_(),
	temp_file_name_(),
	io_type_(kReadOnly),
	current_write_file_(0),
	file_handle_counter_(0),
	write_buffer_(0),
	write_buffer_size_(2 * 1024 * 1024),
	write_buffer_pos_(0),
	current_write_pos_(0),
	callback_(0) {
}

FileArchive::~FileArchive() {
	CloseArchive();
}

IOError FileArchive::OpenArchive(const str& archive_file_name, IOType io_type) {
	CloseArchive();

	archive_file_name_ = archive_file_name;

	str temp_file_name(archive_file_name + ".tmp");

	io_type_ = io_type;

	switch(io_type_) {
	case kReadOnly:

		if (archive_file_.Open(archive_file_name_, DiskFile::kModeRead, false, Endian::kTypeLittleEndian) == false) {
			return kIoFileNotFound;
		}

		return ReadHeader();

		break;
	case kWriteOnly:

		if (archive_file_.Open(temp_file_name, DiskFile::kModeWrite, false, Endian::kTypeLittleEndian) == false) {
			return kIoErrorWritingToStream;
		}

		write_buffer_ = new uint8[write_buffer_size_];
		write_buffer_pos_ = 0;
		current_write_pos_ = 0;

		break;
	case kWriteAppend:
		if (archive_file_.Open(archive_file_name_, DiskFile::kModeRead, false, Endian::kTypeLittleEndian) == false) {
			return kIoFileNotFound;
		}

		int64 _header_offset = 0;
		IOError error = ReadHeader(&_header_offset, true);

		archive_file_.Close();

		if (error != kIoOk) {
			return error;
		}

		if (archive_file_.Open(archive_file_name_, DiskFile::kModeWriteAppend, false, Endian::kTypeLittleEndian) == false) {
			return kIoErrorWritingToStream;
		}

		archive_file_.SeekSet(_header_offset);

		write_buffer_ = new uint8[write_buffer_size_];
		write_buffer_pos_ = 0;
		current_write_pos_ = _header_offset;
		break;
	};

	return kIoOk;
}

void FileArchive::CloseArchive() {
	if (io_type_ == kReadOnly) {
		if (archive_file_.IsOpen()) {
			archive_file_.Close();
		}
	} else if(io_type_ == kWriteOnly || io_type_ == kWriteAppend) {
		if (archive_file_.IsOpen()) {
			if (write_buffer_pos_ != 0) {
				archive_file_.WriteData(write_buffer_, write_buffer_pos_);
				write_buffer_pos_ = 0;
			}

			WriteHeader();

			archive_file_.Close();

			if (io_type_ == kWriteOnly) {
				// Replace the original file with the temp file.
				remove(archive_file_name_.c_str());
				rename(temp_file_name_.c_str(), archive_file_name_.c_str());
			}
		}
	}

	if (write_buffer_ != 0) {
		delete[] write_buffer_;
		write_buffer_ = 0;
		write_buffer_pos_ = 0;
	}

	archive_file_name_ = "";
	temp_file_name_ = "";

	open_file_table_.RemoveAll();

	FileNameTable::Iterator iter = file_name_table_.First();
	while (iter != file_name_table_.End()) {
/*		if (io_type_ == kWriteOnly || io_type_ == kWriteAppend) {
			// Delete filename, since it was allocated and inserted
			// in FileOpen().
			char* _file_name = (char*)iter.GetKey();
			delete[] _file_name;
		}
*/
		FileArchiveFile* file = *iter;
		file_name_table_.Remove(iter++);

		delete file;
	}

	file_name_list_.clear();
}

void FileArchive::CloseAndRemoveArchive() {
	archive_file_.Close();

	if (io_type_ == kReadOnly) {
#ifdef LEPRA_POSIX
		::remove(archive_file_name_.c_str()); // TODO: Find a unicode-version of this.
#else
		::_wremove(wstrutil::Encode(archive_file_name_).c_str());
#endif
	} else {
#ifdef LEPRA_POSIX
		::remove(temp_file_name_.c_str()); // TODO: Find a unicode-version of this.
#else
		::_wremove(wstrutil::Encode(temp_file_name_).c_str());
#endif
	}

	if (write_buffer_ != 0) {
		delete[] write_buffer_;
		write_buffer_ = 0;
		write_buffer_pos_ = 0;
	}

	archive_file_name_ = "";
	temp_file_name_ = "";

	open_file_table_.RemoveAll();

	FileNameTable::Iterator iter = file_name_table_.First();
	while (iter != file_name_table_.End()) {
		/* JB: using strings instead of char*.
		if (io_type_ == kWriteOnly || io_type_ == kWriteAppend) {
			// Delete filename, since it was allocated and inserted
			// in FileOpen().
			str& _file_name = iter.GetKey();
			delete[] _file_name;
		}*/

		FileArchiveFile* file = *iter;
		file_name_table_.Remove(iter++);
		delete (file);
	}

    file_name_list_.clear();
}

IOError FileArchive::ReadHeader(int64* header_offset, bool fill_file_name_list) {
	char data[8];

	int tail_size = 4 + sizeof(int64);
	archive_file_.SeekEnd(-tail_size);

	archive_file_.ReadData(data, tail_size);

	if (data[tail_size - 4] == 'B' &&
	   data[tail_size - 3] == 'U' &&
	   data[tail_size - 2] == 'N' &&
	   data[tail_size - 1] == 'T') {
		int64 _header_offset = ((int64*)data)[0];
		archive_file_.SeekSet(_header_offset);

		if (header_offset != 0) {
			*header_offset = _header_offset;
		}

		int file_count;
		archive_file_.ReadData(&file_count, sizeof(file_count));

		for (int i = 0; i < file_count; i++) {
			str _file_name;
			archive_file_.ReadLine(_file_name);
			strutil::ToLower(_file_name);

			FileArchiveFile* file = new FileArchiveFile(_file_name);

			archive_file_.ReadData(&file->size_, sizeof(file->size_));
			archive_file_.ReadData(&file->start_offset_, sizeof(file->start_offset_));

			file->current_pos_ = 0;

			file_name_table_.Insert(file->file_name_, file);

			if (fill_file_name_list == true) {
				file_name_list_.push_back(file);
			}
		}

		return kIoOk;
	}

	return kIoInvalidFormat;
}

IOError FileArchive::WriteHeader() {
	int64 _header_offset = archive_file_.Tell();

	int file_count = (int)file_name_list_.size();

	archive_file_.WriteData(&file_count, sizeof(file_count));

	FileList::iterator iter;

	for (iter = file_name_list_.begin();
		iter != file_name_list_.end();
		++iter) {
		FileArchiveFile* file = *iter;

		// Check for illegal '\n'
		if (file->file_name_.find('\n', 0) != str::npos) {
			return kIoInvalidFilename;
		}

		archive_file_.WriteString(file->file_name_);
		archive_file_.Write('\n');

		archive_file_.WriteData(&file->size_, sizeof(file->size_));
		archive_file_.WriteData(&file->start_offset_, sizeof(file->start_offset_));
	}

	archive_file_.WriteData(&_header_offset, sizeof(_header_offset));
	archive_file_.WriteString("BUNT");

	return kIoOk;
}

int FileArchive::GetFileCount() {
	return file_name_table_.GetCount();
}

str FileArchive::FileFindFirst() {
	str _file_name;

	if (io_type_ == kReadOnly) {
		FileNameTable::Iterator iter;
		iter = file_name_table_.First();

		if (iter != file_name_table_.End()) {
			current_find_iterator_ = iter;
			_file_name = (*iter)->file_name_;
		}
	}

	return _file_name;
}

str FileArchive::FileFindNext() {
	str _file_name;

	if (io_type_ == kReadOnly) {
		++current_find_iterator_;

		if (current_find_iterator_ != file_name_table_.End()) {
			_file_name = (*current_find_iterator_)->file_name_;
		}
	}

	return _file_name;
}

int FileArchive::FileOpen(const str& file_name) {
	if (file_name == "" ||
	   io_type_ == kInsertOnly ||
	   archive_file_.IsOpen() == false) {
		return 0;
	}

	str _file_name(file_name);
	strutil::ToLower(_file_name);

	// Find the file...
	FileNameTable::Iterator iter = file_name_table_.First();
	FileArchiveFile* file = 0;
	while (iter != file_name_table_.End()) {
		FileArchiveFile* temp_file = *iter;

		if (strutil::CompareIgnoreCase(_file_name, temp_file->file_name_) == 0) {
			file = temp_file;
			break;
		}

		++iter;
	}

	switch(io_type_) {
	case kReadOnly: {
			// TODO: Is it possible to optimize this?

			// File must exist.
			if (file != 0) {
				file->current_pos_ = 0;

				file_handle_counter_++;
				open_file_table_.Insert(file_handle_counter_, file);

				return file_handle_counter_;
			}
		} break;
	case kWriteOnly:
	case kWriteAppend: {
			// File mustn't exist.
			if (file == 0) {
				// Close the possibly opened file...
				if (current_write_file_ != 0) {
					current_write_file_->size_ = current_write_pos_ - current_write_file_->start_offset_;
					current_write_file_ = 0;
				}

				file = new FileArchiveFile(_file_name);
				file->start_offset_ = current_write_pos_;
				file->current_pos_ = 0;
				file->size_ = 0;

				current_write_file_ = file;

				// Inserting filename as a key... Deleting it in CloseArchive().
				file_name_table_.Insert(_file_name, file);
				file_name_list_.push_back(file);

				return 1;
			}
		} break;
	};

	return 0;
}

void FileArchive::FileClose(int file_handle) {
	if (io_type_ == kReadOnly) {
		open_file_table_.Remove(file_handle);
	} else if(io_type_ == kWriteOnly || io_type_ == kWriteAppend) {
		if (current_write_file_ != 0) {
			current_write_file_->size_ = current_write_pos_ - current_write_file_->start_offset_;
			current_write_file_ = 0;
		}
	}
}

bool FileArchive::FileExist(const str& file_name) {
	str _file_name(file_name);
	strutil::ToLower(_file_name);

	return file_name_table_.Find(_file_name) != file_name_table_.End();
}

IOError FileArchive::FileRead(void* dest, int _size, int file_handle) {
	if (archive_file_.IsOpen() == false) {
		return kIoPackageNotOpen;
	}

	if (io_type_ == kReadOnly) {
		FileArchiveFile* file = GetFile(file_handle);

		if (file != 0) {
			// Trying to read outside the file?
			if (file->current_pos_ + _size > file->size_) {
				return kIoErrorReadingFromStream;
			}

			archive_file_.SeekSet(file->start_offset_ + file->current_pos_);
			if (archive_file_.ReadData(dest, _size) == false) {
				return kIoErrorReadingFromStream;
			}

			file->current_pos_ += _size;
		} else {
			return kIoStreamNotOpen;
		}
	} else {
		return kIoErrorReadingFromStream;
	}

	return kIoOk;
}

IOError FileArchive::FileWrite(void* source, int _size) {
	if (archive_file_.IsOpen() == false) {
		return kIoPackageNotOpen;
	}

	if (io_type_ == kWriteOnly || io_type_ == kWriteAppend) {
		// While _size is larger than what's left in the write buffer,
		// or the write buffer itself.
		int write_size = _size;

		while (((int64)write_buffer_pos_ + write_size) > (int64)write_buffer_size_) {
			// Make sure the write buffer is filled to every byte...
			int rest = write_buffer_size_ - write_buffer_pos_;
			if (rest > 0) {
				::memcpy(write_buffer_ + write_buffer_pos_, (const char*)source + (_size - write_size), (size_t)rest);
				write_size -= rest;
			}

			// Write the whole buffer to disc.
			archive_file_.WriteData(write_buffer_, write_buffer_size_);
			write_buffer_pos_ = 0;

			// Update the file position...
			current_write_pos_ += rest;
		}

		// Write whatever is left from the previous check.
		::memcpy(write_buffer_ + write_buffer_pos_, (const char*)source + (_size - write_size), (size_t)write_size);
		write_buffer_pos_ += write_size;
		current_write_pos_ += write_size;
	} else {
		return kIoErrorWritingToStream;
	}

	return kIoOk;
}

int64 FileArchive::FileSize(int file_handle) {
	if (archive_file_.IsOpen() == false ||
	   io_type_ != kReadOnly) {
		return -1;
	}

	FileArchiveFile* file = GetFile(file_handle);
	if (file != 0) {
		return file->size_;
	}

	return 0;
}

FileArchive::FileArchiveFile* FileArchive::GetFile(int file_handle) {
	if (io_type_ == kReadOnly) {
		FileTable::Iterator iter = open_file_table_.Find(file_handle);

		if (iter != open_file_table_.End()) {
			return *iter;
		}
	}

	return 0;
}

void FileArchive::FileSeek(int64 offset, FileOrigin origin, int file_handle) {
	if (archive_file_.IsOpen() == false ||
	   io_type_ != kReadOnly) {
		return;
	}

	FileArchiveFile* file = GetFile(file_handle);
	if (file != 0) {
		switch(origin) {
		case kFseekSet:
			file->current_pos_ = offset;
			break;
		case kFseekCur:
			file->current_pos_ += offset;
			break;
		case kFseekEnd:
			file->current_pos_ = (file->size_ + offset) - 1;
			break;
		};

		if (file->current_pos_ < 0) {
			file->current_pos_ = 0;
		}

		if (file->current_pos_ > file->size_) {
			file->current_pos_ = file->size_;
		}
	}
}

IOError FileArchive::InsertArchive(const str& archive_file_name) {
/*
	if (callback_) {
		callback_->SetProgressMax(files)
	}
	for (file = 0; file < files; file++) {
		...
	...
*/
	const int chunk_size = 1024*1024; // 1 kMb

	if (io_type_ == kInsertOnly) {
		IOError error;

		error = OpenArchive(archive_file_name_, kReadOnly);
		if (error != kIoOk) {
			return error;
		}

		FileArchive insert_archive;
		error = insert_archive.OpenArchive(archive_file_name, kReadOnly);
		if (error != kIoOk) {
			return error;
		}

		// Writing is done using the temp file name...
		FileArchive out_archive;
		out_archive.OpenArchive(archive_file_name_, kWriteOnly);

		int progress = 0;
		if (callback_ != 0) {
			int files = insert_archive.GetFileCount() + GetFileCount();
			callback_->SetProgressPos(progress);
			callback_->SetProgressMax(files);
		}

		// Iterate through all files in "this"...
		FileNameTable::Iterator __iterator;
		for (__iterator = file_name_table_.First();
			__iterator != file_name_table_.End();
			++__iterator) {
			FileNameTable::Iterator find_iter =
				insert_archive.file_name_table_.Find((*__iterator)->file_name_);

			// For each file in "this" that doesn't exist in archive, write.
			if (find_iter == insert_archive.file_name_table_.End()) {
				// Open the source file.
				int _file_handle = FileOpen((*__iterator)->file_name_);
				int64 file_size = FileSize(_file_handle);

				// Open the file to write.
				out_archive.FileOpen((*__iterator)->file_name_);

				int64 num_chunks = file_size / (int64)chunk_size;
				int rest = (int)(file_size % (int64)chunk_size);

				uint8* data = new uint8[chunk_size];

				// Copy the file chunkwise.
				for (int64 i = 0; i < num_chunks; i++) {
					FileRead(data, chunk_size, _file_handle);
					out_archive.FileWrite(data, chunk_size);
				}

				if (rest != 0) {
					FileRead(data, rest, _file_handle);
					out_archive.FileWrite(data, rest);
				}

				out_archive.FileClose(0);

				delete[] data;

				FileClose(_file_handle);
			}

			if (callback_ != 0) {
				callback_->SetProgressPos(++progress);
			}
		}

		// Iterate through all files in archive...
		for (__iterator = insert_archive.file_name_table_.First();
			__iterator != insert_archive.file_name_table_.End();
			++__iterator) {
			// Open the source file.
			int _file_handle = insert_archive.FileOpen((*__iterator)->file_name_);
			int64 file_size = insert_archive.FileSize(_file_handle);

			// Open the file to write.
			out_archive.FileOpen((*__iterator)->file_name_);

			int64 num_chunks = file_size / (int64)chunk_size;
			int rest = (int)(file_size % (int64)chunk_size);

			uint8* data = new uint8[chunk_size];

			// Copy the file chunkwise.
			for (int64 i = 0; i < num_chunks; i++) {
				insert_archive.FileRead(data, chunk_size, _file_handle);
				out_archive.FileWrite(data, chunk_size);
			}

			if (rest != 0) {
				insert_archive.FileRead(data, rest, _file_handle);
				out_archive.FileWrite(data, rest);
			}

			out_archive.FileClose(0);

			delete[] data;

			insert_archive.FileClose(_file_handle);

			if (callback_ != 0) {
				callback_->SetProgressPos(++progress);
			}
		}

		insert_archive.CloseArchive();
		CloseArchive();

		out_archive.CloseArchive();

		return kIoOk;
	}

	return kIoInvalidMode;
}



void FileArchive::SetProgressCallback(ProgressCallback* callback) {
	callback_ = callback;
}

bool FileArchive::AddFile(const str& file_name, const str& dest_file_name,
						  int buffer_size, SizeUnit unit) {
	if (io_type_ != kWriteOnly && io_type_ != kWriteAppend) {
		return false;
	}

	DiskFile file;
	if (file.Open(file_name, DiskFile::kModeRead, false, Endian::kTypeLittleEndian) == true) {
		int _buffer_size = (int)buffer_size * (int)unit;
		int64 data_size   = file.GetSize();
		uint8* buffer = new uint8[(unsigned)_buffer_size];

		int64 num_chunks = data_size / _buffer_size;
		int rest      = (int)(data_size % (int64)_buffer_size);

		FileOpen(dest_file_name);

		for (int64 i = 0; i < num_chunks; i++) {
			file.ReadData(buffer, _buffer_size);
			FileWrite(buffer, _buffer_size);
		}

		if (rest != 0) {
			file.ReadData(buffer, rest);
			FileWrite(buffer, rest);
		}

		file.Close();
		FileClose(0);

		delete[] buffer;

		return true;
	} else {
		return false;
	}
}

bool FileArchive::ExtractFile(const str& file_name, const str& dest_file_name,
							  int buffer_size, SizeUnit unit) {
	if (io_type_ != kReadOnly) {
		return false;
	}

	int handle = FileOpen(file_name);

	if (handle != 0) {
		DiskFile file;
		if (file.Open(dest_file_name, DiskFile::kModeWrite, true, Endian::kTypeLittleEndian) == true) {
			int _buffer_size = (int)buffer_size * (int)unit;
			int64 data_size   = FileSize(handle);
			uint8* buffer = new uint8[(unsigned)_buffer_size];

			int64 num_chunks = data_size / _buffer_size;
			int rest      = (int)(data_size % (int64)_buffer_size);

			for (int64 i = 0; i < num_chunks; i++) {
				FileRead(buffer, _buffer_size, handle);
				file.WriteData(buffer, _buffer_size);
			}

			if (rest != 0) {
				FileRead(buffer, rest, handle);
				file.WriteData(buffer, rest);
			}

			FileClose(handle);
			file.Close();

			delete[] buffer;

			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}
}



}
