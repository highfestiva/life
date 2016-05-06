
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

#include "pch.h"
#include "../include/diskfile.h"
#include "../include/log.h"
#include "../include/path.h"
#include "../include/ziparchive.h"
#include <stdio.h>

namespace lepra {

ZipArchive::ZipArchive() :
	archive_name_(""),
	io_type_(kReadOnly),
	unzip_file_(0),
	zip_file_(0),
	file_size_(0) {
}

ZipArchive::~ZipArchive() {
	CloseArchive();
}

IOError ZipArchive::OpenArchive(const str& archive_file_name, IOType io_type) {
	CloseArchive();

	archive_name_ = archive_file_name;

	IOError io_error = kIoFileNotFound;
	bool check_zip_file = false;

	io_type_ = io_type;
	switch(io_type_) {
		case kReadOnly: {
			unzip_file_ = ::unzOpen64(archive_file_name.c_str());
			if (unzip_file_ != 0) {
				io_error = kIoOk;
			}
		} break;
		case kWriteOnly: {
			zip_file_ = ::zipOpen(archive_file_name.c_str(), APPEND_STATUS_CREATE);
			check_zip_file = true;
		} break;
		case kWriteAppend: {
			if (DiskFile::Exists(archive_file_name) == true)
				zip_file_ = ::zipOpen(archive_file_name.c_str(), APPEND_STATUS_ADDINZIP);
			check_zip_file = true;
		} break;
		case kInsertOnly: {
		} break;
	}

	if (check_zip_file == true) {
		if (zip_file_ != 0) {
			io_error = kIoOk;
		} else {
			io_error = kIoFileCreationFailed;
		}
	}

	return io_error;
}

void ZipArchive::CloseArchive() {
	switch(io_type_) {
		case kReadOnly: {
			if (unzip_file_ != 0) {
				::unzClose(unzip_file_);
				unzip_file_ = 0;
			}
		} break;
		case kWriteOnly:
		case kWriteAppend: {
			if (zip_file_ != 0) {
				::zipClose(zip_file_, "");
				zip_file_ = 0;
			}
		} break;
		case kInsertOnly: {
		} break;
	}
}

void ZipArchive::CloseAndRemoveArchive() {
	CloseArchive();
#ifdef LEPRA_MSVC
	::_wremove(wstrutil::Encode(archive_name_).c_str());
#else
#ifdef LEPRA_POSIX
	::remove(archive_name_.c_str());
#else
#error "ZipArchive::CloseAndRemoveArchive() is not implemented on this platform!"
#endif
#endif
}

int ZipArchive::GetFileCount() {
	int count = 0;

	if (io_type_ == kReadOnly && unzip_file_ != 0) {
		unz_global_info64 unz_global_info;
		::unzGetGlobalInfo64(unzip_file_, &unz_global_info);
		count = (int)unz_global_info.number_entry;
	}

	return count;
}

IOError ZipArchive::InsertArchive(const str& /*archive_file_name*/) {
	return kIoOk;
}

str ZipArchive::FileFindFirst() {
	str _file_name;

	if (io_type_ == kReadOnly && unzip_file_ != 0 && ::unzGoToFirstFile(unzip_file_) == UNZ_OK) {
		unz_file_info64 file_info;
		char c_str_file_name[1024];
		::unzGetCurrentFileInfo64(unzip_file_, &file_info, c_str_file_name, 1024, 0, 0, 0, 0);
		_file_name = str(c_str_file_name);
	}

	return _file_name;
}

str ZipArchive::FileFindNext() {
	str _file_name;

	if (io_type_ == kReadOnly && unzip_file_ != 0 && ::unzGoToNextFile(unzip_file_) == UNZ_OK) {
		unz_file_info64 file_info;
		char c_str_file_name[1024];
		::unzGetCurrentFileInfo64(unzip_file_, &file_info, c_str_file_name, 1024, 0, 0, 0, 0);
		_file_name = str(c_str_file_name);
	}

	return _file_name;
}

bool ZipArchive::FileOpen(const str& file_name) {
	bool ok = false;

	switch(io_type_) {
		case kReadOnly: {
			if (unzip_file_ != 0) {
				if (::unzLocateFile(unzip_file_, file_name.c_str(), 0) == UNZ_OK) {
					ok = (::unzOpenCurrentFile(unzip_file_) == UNZ_OK);
					if (ok) {
						file_size_ = FileSize();
					}
				}
			}
		} break;
		case kWriteOnly:
		case kWriteAppend: {
			if (zip_file_ != 0) {
				ok = (::zipOpenNewFileInZip64(zip_file_, file_name.c_str(), 0, 0, 0, 0, 0, 0, Z_DEFLATED, Z_DEFAULT_COMPRESSION, 0) == ZIP_OK);

				str temp_name;
				str directory;
				directory = Path::GetDirectory(archive_name_);

				if (ok) {
					temp_name = DiskFile::GenerateUniqueFileName(directory);
					ok = out_file_.Open(temp_name, DiskFile::kModeWrite, false, Endian::kTypeLittleEndian);

					if (ok == false) {
						::zipCloseFileInZip(zip_file_);
					}
				}
			}
		} break;
		case kInsertOnly: {
		} break;
	}

	return ok;
}

void ZipArchive::FileClose() {
	switch(io_type_) {
		case kReadOnly: {
			if (unzip_file_ != 0) {
				::unzCloseCurrentFile(unzip_file_);
				file_size_ = 0;
			}
		} break;
		case kWriteOnly:
		case kWriteAppend: {
			if (zip_file_ != 0) {
				if (out_file_.IsOpen() == true) {
					str temp_file = out_file_.GetFullName();
					int64 _size = out_file_.GetSize();
					out_file_.Close();
					if (out_file_.Open(temp_file, DiskFile::kModeRead, false, Endian::kTypeLittleEndian) == false) {
						log_.Error("Failed to add file to archive.");
					} else {
						// Copy temp file to the zip archive.
						uint8 buf[1024];
						int64 steps = _size / 1024;
						int64 rest = _size % 1024;
						int64 i;

						for (i = 0; i < steps; i++) {
							out_file_.ReadData(buf, 1024);
							::zipWriteInFileInZip(zip_file_, buf, 1024);
						}

						if (rest > 0) {
							out_file_.ReadData(buf, (unsigned long)rest);
							::zipWriteInFileInZip(zip_file_, buf, (unsigned long)rest);
						}

						out_file_.Close();
						DiskFile::Delete(temp_file);
					}

					::zipCloseFileInZip(zip_file_);
				}
			}
		} break;
		case kInsertOnly: {
		} break;
	}
}

bool ZipArchive::FileExist(const str& file_name) {
	bool exist = false;

	if (io_type_ == kReadOnly && unzip_file_ != 0) {
		exist = (::unzLocateFile(unzip_file_, file_name.c_str(), 0) == UNZ_OK);
	}

	return exist;
}

IOError ZipArchive::FileRead(void* dest, int size) {
	IOError err = kIoErrorReadingFromStream;

	if (io_type_ == kReadOnly) {
		if (unzip_file_ != 0) {
			int _size = ::unzReadCurrentFile(unzip_file_, dest, size);
			if (_size > 0) {
				err = kIoOk;
			} else if(_size == 0) {
				err = kIoNoDataAvailable;
			}
		} else {
			err = kIoStreamNotOpen;
		}
	}

	return err;
}

IOError ZipArchive::FileWrite(void* source, int size) {
	IOError err = kIoErrorWritingToStream;

	if (io_type_ == kWriteOnly || io_type_ == kWriteAppend) {
		out_file_.WriteData(source, size);
/*		if (::zipWriteInFileInZip(zip_file_, source, size) == ZIP_OK) {
			err = kIoOk;
		}
*/
	}

	return err;
}

int64 ZipArchive::FileSize() {
	int64 _size = 0;

	if (io_type_ == kReadOnly && unzip_file_ != 0) {
		unz_file_info64 file_info;
		if (::unzGetCurrentFileInfo64(unzip_file_, &file_info, 0, 0, 0, 0, 0, 0) == UNZ_OK) {
			_size = (int64)file_info.uncompressed_size;
		}
	} else if(io_type_ == kWriteOnly || io_type_ == kWriteAppend) {
		_size = out_file_.GetSize();
	}

	return _size;
}

void ZipArchive::FileSeek(int64 offset, FileOrigin origin) {
	if (io_type_ == kReadOnly && unzip_file_ != 0 && file_size_ != 0) {
		int64 _offset = offset;
		switch(origin) {
		case kFseekSet:
			_offset = offset;
			break;
		case kFseekCur:
			_offset = ::unztell64(unzip_file_) + offset;
			break;
		case kFseekEnd:
			_offset = (uLong)(file_size_ + offset - 1);
			break;
		}

		::unzCloseCurrentFile(unzip_file_);
		::unzOpenCurrentFile(unzip_file_);

		// Now skip _offset bytes of data.
		uint8 buf[1024];
		int64 steps = _offset / 1024;
		int64 rest  = _offset % 1024;

		int64 i;
		for (i = 0; i < steps; i++) {
			::unzReadCurrentFile(unzip_file_, buf, 1024);
		}

		if (rest > 0) {
			::unzReadCurrentFile(unzip_file_, buf, (uLong)rest);
		}
	} else if(io_type_ == kWriteOnly || io_type_ == kWriteAppend) {
		switch(origin) {
		case SEEK_SET:
			out_file_.Seek(offset, File::kFseekSet);
			break;
		case SEEK_CUR:
			out_file_.Seek(offset, File::kFseekCur);
			break;
		case SEEK_END:
			out_file_.Seek(offset, File::kFseekEnd);
			break;
		}
	}
}

bool ZipArchive::ExtractFile(const str& file_name,
			     const str& dest_file_name,
			     int buffer_size, SizeUnit unit) {
	if (io_type_ != kReadOnly) {
		return false;
	}

	bool ok = FileOpen(file_name);

	if (ok == true) {
		DiskFile file;
		if (file.Open(dest_file_name, DiskFile::kModeWrite, false, Endian::kTypeLittleEndian) == true) {
			int _buffer_size = (int)buffer_size * (int)unit;
			int64 data_size = FileSize();
			uint8* buffer = new uint8[(unsigned)_buffer_size];

			int64 num_chunks = data_size / _buffer_size;
			int rest = (int)(data_size % (int64)_buffer_size);

			for (int64 i = 0; i < num_chunks; i++) {
				FileRead(buffer, _buffer_size);
				file.WriteData(buffer, _buffer_size);
			}

			if (rest != 0) {
				FileRead(buffer, rest);
				file.WriteData(buffer, rest);
			}

			FileClose();
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

loginstance(kGeneralResources, ZipArchive);

}
