
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/diskfile.h"
#include "../include/filearchive.h"
#include "../include/path.h"
#include "../include/random.h"
#include "../include/string.h"

#include <string.h>
#include <errno.h>

#if defined LEPRA_WINDOWS
#include <direct.h>
#include <io.h>
#elif defined LEPRA_POSIX
#include <sys/stat.h>
#include <unistd.h>
#endif // Windows / Posix



namespace lepra {



DiskFile::DiskFile() :
	File(Endian::kTypeBigEndian, Endian::kTypeBigEndian, 0, 0),
	file_(0),
	file_name_(""),
	path_(""),
	file_size_(0),
	writer_(0),
	reader_(0),
	mode_(kModeRead) {
	Reader::SetInputStream(this);
	Writer::SetOutputStream(this);
}

DiskFile::DiskFile(Reader* reader) :
	File(Endian::kTypeBigEndian, Endian::kTypeBigEndian, 0, 0),
	file_(0),
	file_name_(""),
	path_(""),
	file_size_(0),
	writer_(0),
	reader_(reader),
	mode_(kModeRead) {
	Reader::SetInputStream(this);
	Writer::SetOutputStream(this);

	if (reader_ != 0) {
		reader_->SetInputStream(this);
	}
}

DiskFile::DiskFile(Writer* writer) :
	File(Endian::kTypeBigEndian, Endian::kTypeBigEndian, 0, 0),
	file_(0),
	file_name_(""),
	path_(""),
	file_size_(0),
	writer_(writer),
	reader_(0),
	mode_(kModeRead) {
	Reader::SetInputStream(this);
	Writer::SetOutputStream(this);

	if (writer_ != 0) {
		writer_->SetOutputStream(this);
	}
}

DiskFile::DiskFile(Reader* reader, Writer* writer) :
	File(Endian::kTypeBigEndian, Endian::kTypeBigEndian, 0, 0),
	file_(0),
	file_name_(""),
	path_(""),
	file_size_(0),
	writer_(writer),
	reader_(reader),
	mode_(kModeRead) {
	Reader::SetInputStream(this);
	Writer::SetOutputStream(this);

	if (reader_ != 0) {
		reader_->SetInputStream(this);
	}

	if (writer_ != 0) {
		writer_->SetOutputStream(this);
	}
}

DiskFile::~DiskFile() {
	Close();
}

void DiskFile::ExtractPathAndFileName(const str& file_name) {
	Path::SplitPath(file_name, path_, file_name_);
	InputStream::SetName(file_name_);
	OutputStream::SetName(file_name_);
}

FILE* DiskFile::FileOpen(const str& file_name, const str& mode) {
	FILE* file = 0;

#if _MSC_VER > 1310	// MS compiler version 13 1.0 = 2003 .NET.
#ifdef LEPRA_UNICODE
	if (::_wfopen_s(&file, file_name.c_str(), mode.c_str()) != 0)
#else // ANSI
	if (::fopen_s(&file, file_name.c_str(), mode.c_str()) != 0)
#endif // Unicode / ANSI.
	{
		file = 0;
	}
#else // _MSC_VER <= 1310
	file = fopen(file_name.c_str(), mode.c_str());
#endif // _MSC_VER > 1310 / _MSC_VER <= 1310

	return (file);
}

bool DiskFile::Open(const str& file_name, OpenMode mode, bool create_path, Endian::EndianType endian) {
	Close();

	SetEndian(endian);

	bool ok = true;

	Parent::ClearMode(Parent::kReadMode);
	Parent::ClearMode(Parent::kWriteMode);
	if (mode&kModeRead) {
		Parent::SetMode(Parent::kReadMode);
	}
	if (mode&(kModeWrite|kModeWriteAppend)) {
		Parent::SetMode(Parent::kWriteMode);
	}
	if (!(mode&(kModeWrite|kModeWriteAppend|kModeRead))) {
		ok = false;
	}

	if (ok) {
		ExtractPathAndFileName(file_name);
		if(mode&(kModeWrite|kModeWriteAppend)) {
			if (create_path == true && CreateSubDirs() == false) {
				ok = false;
			}
		}
	}

	str mode_string;
	if (ok) {
		if ((mode&(kModeRead|kModeWrite|kModeWriteAppend)) == kModeRead) {
			mode_string = "r";
		} else if ((mode&(kModeRead|kModeWrite|kModeWriteAppend)) == kModeWrite) {
			mode_string = "w";
		} else if ((mode&(kModeRead|kModeWriteAppend)) == kModeWriteAppend) {
			mode_string = "a";
		} else if ((mode&(kModeRead|kModeWrite|kModeWriteAppend)) == (kModeRead|kModeWrite)) {
			mode_string = "r+";
		} else if ((mode&(kModeRead|kModeWriteAppend)) == (kModeRead|kModeWriteAppend)) {
			mode_string = "a+";
		} else {
			ok = false;
		}
	}
	if (ok) {

		if (mode&kModeText) {
			mode_string += "t";
		} else {
			mode_string += "b";
		}
		file_ = FileOpen(file_name, mode_string);
	}

	if (ok) {
		mode_ = mode;

		if (file_ != 0) {
			// Get the file size.
			long position = ::ftell(file_);
			::fseek(file_, 0, SEEK_END);
			file_size_ = (int64)::ftell(file_);
			if (!(mode&kModeWriteAppend)) {
				::fseek(file_, position, SEEK_SET);
			}
		} else {
			ok = false;
		}
	}

	return ok;
}

void DiskFile::Close() {
	if (file_ != 0) {
		fclose(file_);
		file_ = 0;
	}

	file_name_ = "";
	path_ = "";
}

void DiskFile::SetEndian(Endian::EndianType endian) {
	Parent::SetEndian(endian);
	if (reader_) {
		reader_->SetReaderEndian(endian);
	}
	if (writer_) {
		writer_->SetWriterEndian(endian);
	}
}

str DiskFile::GetFullName() const {
	return path_ + "/" + file_name_;
}

str DiskFile::GetName() const {
	return file_name_;
}

str DiskFile::GetPath() const {
	return path_;
}

IOError DiskFile::ReadRaw(void* buffer, size_t _size) {
	IOError error = kIoOk;

	if (file_ == 0) {
		error = kIoStreamNotOpen;
	}

	if (error == kIoOk && !(mode_&kModeRead)) {
		error = kIoInvalidOperation;
	}

	if (error == kIoOk) {
		if (::fread(buffer, (size_t)_size, 1, file_) != 1) {
			error = kIoErrorReadingFromStream;
		}
	}

	return error;
}

IOError DiskFile::Skip(size_t _size) {
	return (Parent::Skip(_size));
}

IOError DiskFile::WriteRaw(const void* buffer, size_t _size) {
	IOError error = kIoOk;

	if (file_ == 0) {
		error = kIoStreamNotOpen;
	}

	if (error == kIoOk && !(mode_&(kModeWrite|kModeWriteAppend))) {
		error = kIoInvalidOperation;
	}

	if (error == kIoOk) {
		if (::fwrite(buffer, (size_t)_size, 1, file_) != 1) {
			error = kIoErrorWritingToStream;
		}
	}

	return error;
}

IOError DiskFile::ReadData(void* buffer, size_t _size) {
	IOError error;
	if (reader_ != 0) {
		error = reader_->ReadData(buffer, _size);
	} else {
		error = Reader::ReadData(buffer, _size);
	}

	return error;
}

IOError DiskFile::WriteData(const void* buffer, size_t _size) {
	IOError error;
	if (writer_ != 0) {
		error = writer_->WriteData(buffer, _size);
	} else {
		error = Writer::WriteData(buffer, _size);
	}
	return error;
}

int64 DiskFile::Tell() const {
#ifdef LEPRA_MSVC
	return (int64)::_ftelli64(file_);
#elif defined(LEPRA_MAC)
	return (int64)::ftell(file_);
#else
	return (int64)::ftello64(file_);
#endif
}

int64 DiskFile::Seek(int64 offset, FileOrigin from) {
	if (!IsOpen()) {
		return (-1);
	}

	int origin;

	switch (from) {
		case kFseekCur: {
			origin = SEEK_CUR;
			break;
		}
		case kFseekEnd: {
			origin = SEEK_END;
			break;
		}
		case kFseekSet:
		default: {
			origin = SEEK_SET;
			break;
		}
	}

	int64 pos = -1;

#if defined(LEPRA_MSVC)
	if (::_fseeki64(file_, offset, origin) == 0)
#elif defined(LEPRA_MAC)
	if (::fseek(file_, offset, origin) == 0)
#else
	if (::fseeko64(file_, offset, origin) == 0)
#endif
	{
		pos = Tell();
	}

	return pos;
}



//
// Static functions.
//

IOError DiskFile::Load(const str& filename, void** data, int64& data_size) {
	DiskFile file;
	bool ok = false;
	for (int x = 0; !ok && x < 3; ++x) {
		ok = file.Open(filename, DiskFile::kModeRead);
	}
	if (ok) {
		data_size = file.GetSize();
		return file.AllocReadData(data, (size_t)data_size);
	}
	return kIoFileNotFound;
}

bool DiskFile::Exists(const str& file_name) {
	FILE* file = FileOpen(file_name, "rb");

	bool ok = false;
	if (file != 0) {
		::fclose(file);
		ok = true;
	}

	return ok;
}

bool DiskFile::PathExists(const str& path_name) {
	char current_dir[300];

#ifdef LEPRA_WINDOWS // Hugge/TRICKY: Should we check for Visual Studio instead?
	::_getcwd(current_dir, 299);
	bool success = _chdir(path_name.c_str()) == 0;
	::_chdir(current_dir);
#else
	::getcwd(current_dir, 299);
	bool success = ::chdir(path_name.c_str()) == 0;
	::chdir(current_dir);
#endif

	return success;
}

bool DiskFile::Delete(const str& file_name) {
	return (::remove(file_name.c_str()) == 0);
}

bool DiskFile::Rename(const str& old_file_name, const str& new_file_name) {
	return (::rename(old_file_name.c_str(), new_file_name.c_str()) == 0);
}

bool DiskFile::CreateDir(const str& path_name) {
#ifdef LEPRA_POSIX
	return ::mkdir(path_name.c_str(), 0775) != -1;
#else
	return ::_mkdir(path_name.c_str()) != -1;
#endif
}

bool DiskFile::RemoveDir(const str& path_name) {
#ifdef LEPRA_WINDOWS // Hugge/TRICKY: Should we check for Visual Studio instead?
	return ::_rmdir(path_name.c_str()) == 0;
#else
	return ::rmdir(path_name.c_str()) == 0;
#endif
}

bool DiskFile::CreateSubDirs() {
	str directory = Path::GetDirectory(path_);
	strutil::strvec directory_array = Path::SplitNodes(directory);

	bool ok = true;
	str new_path;
	for (size_t x = 0; x < directory_array.size(); ++x) {
		new_path += directory_array[x];
		if (CreateDir(new_path) != 0) {
			// errno is a global variable defined in errno.h
			if (errno == ENOENT) {
				ok = false;
				break;
			}
		}
		new_path += "/";
	}

	return ok;
}

bool DiskFile::FindFirst(const str& file_spec, FindData& find_data) {
	find_data.Clear();
	bool ok = true;

	find_data.file_spec_ = file_spec;
#if defined LEPRA_WINDOWS
	_finddata_t _data;
	find_data.find_handle_ = _findfirst(file_spec.c_str(), &_data);
	if (find_data.find_handle_ == -1) {
		ok = false;
	}
	if (ok == true) {
		str _path = Path::SplitPath(file_spec)[0];
		find_data.name_ = Path::JoinPath(_path, str(_data.name));	// TODO: needs real Unicode findxxx().
		find_data.size_ = _data.size;

		if ((_data.attrib & _A_SUBDIR) != 0) {
			find_data.sub_dir_ = true;
		}

		find_data.time_  = _data.time_write;
	}
#elif defined LEPRA_POSIX
	find_data.glob_list_.gl_offs = 0;
	::glob(file_spec.c_str(), GLOB_DOOFFS|GLOB_MARK, 0, &find_data.glob_list_);
	if (find_data.glob_list_.gl_pathc >= 1) {
		find_data.name_ = find_data.glob_list_.gl_pathv[0];
		struct stat file_info;
		::stat(find_data.glob_list_.gl_pathv[1], &file_info);	// TODO: error check.
		find_data.size_ = file_info.st_size;
		find_data.sub_dir_ = (S_ISDIR(file_info.st_mode) != 0);
		find_data.time_ = file_info.st_mtime;
		find_data.glob_index_ = 2;
	} else {
		ok = false;
		find_data.glob_index_ = 0xffff;
	}
#else
#error DiskFile::FindFirst() not implemented on this platform!
#endif

	return ok;
}

bool DiskFile::FindNext(FindData& find_data) {
	bool ok = true;

#ifdef LEPRA_WINDOWS
	_finddata_t _data;
	if (_findnext(find_data.find_handle_, &_data) != 0) {
		ok = false;
	}
	if (ok == true) {
		str _path = Path::SplitPath(find_data.file_spec_)[0];
		find_data.name_ = Path::JoinPath(_path, str(_data.name));	// TODO: needs real Unicode findxxx()!
		find_data.size_ = _data.size;
		find_data.sub_dir_ = ((_data.attrib & _A_SUBDIR) != 0);
		find_data.time_  = _data.time_write;
	}
#elif defined LEPRA_POSIX
	ok = false;
	if (find_data.glob_index_ < find_data.glob_list_.gl_pathc) {
		ok = true;
		find_data.name_ = find_data.glob_list_.gl_pathv[find_data.glob_index_];
		struct stat file_info;
		::stat(find_data.glob_list_.gl_pathv[find_data.glob_index_], &file_info);	// TODO: error check.
		find_data.size_ = file_info.st_size;
		find_data.sub_dir_ = (S_ISDIR(file_info.st_mode) != 0);
		find_data.time_ = file_info.st_mtime;
		++find_data.glob_index_;
	} else if (find_data.glob_index_ != 0xffff) {
		::globfree(&find_data.glob_list_);
		find_data.glob_index_ = 0xffff;
	}
#else
#error DiskFile::FindNext() not implemented on this platform!
#endif

	if (!ok) {
		find_data.Clear();
	}

	return ok;
}

str DiskFile::GenerateUniqueFileName(const str& path) {
	str _path(path);
	_path += '/';

	int64 random_number = (int64)Random::GetRandomNumber64();
	if (random_number < 0)
		random_number = -random_number;

	str __name = strutil::IntToString(random_number, 16);

	while (Exists(path + __name) == true) {
		random_number = (int64)Random::GetRandomNumber64();
		if (random_number < 0) {
			random_number = -random_number;
		}
		__name = strutil::IntToString(random_number, 16);
	}

	return path + __name;
}

void DiskFile::operator=(const DiskFile&) {
}

bool DiskFile::IsOpen() const {
	return (file_ != 0);
}

int64 DiskFile::GetSize() const {
	return file_size_;
}

int64 DiskFile::GetAvailable() const {
	return (file_size_ - Tell());
}

void DiskFile::Flush() {
	::fflush(file_);
}



}
