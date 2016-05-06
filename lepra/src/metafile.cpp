
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

#include "pch.h"
#include "../include/metafile.h"
#include <algorithm>
#include "../include/path.h"

namespace lepra {

MetaFile::MetaFile() :
	disk_file_(0),
	archive_file_(0),
	reader_(0),
	writer_(0),
	endian_(Endian::kTypeBigEndian) {
}

MetaFile::MetaFile(Reader* reader) :
	disk_file_(0),
	archive_file_(0),
	reader_(reader),
	writer_(0),
	endian_(Endian::kTypeBigEndian) {
}

MetaFile::MetaFile(Writer* writer) :
	disk_file_(0),
	archive_file_(0),
	reader_(0),
	writer_(writer),
	endian_(Endian::kTypeBigEndian) {
}

MetaFile::MetaFile(Reader* reader, Writer* writer) :
	disk_file_(0),
	archive_file_(0),
	reader_(reader),
	writer_(writer),
	endian_(Endian::kTypeBigEndian) {
}

MetaFile::~MetaFile() {
	Close();
}

bool MetaFile::Open(const str& file_name, OpenMode mode, bool create_path, Endian::EndianType endian) {
	Close();
	SetEndian(endian);

	bool ok = false;

	size_t _split_index = 0;
	str path;
	str file;
	bool do_continue = true;

	// Find a valid combination of archive and file...
	while (do_continue) {
		do_continue = SplitPath(file_name, path, file, _split_index);

		if (_split_index == 0) {
			ok = DiskFile::Exists(path);
			if (ok) {
				AllocDiskFile();
				ok = disk_file_->Open(path, ToDiskFileMode(mode), create_path, endian);
				if (ok) {
					do_continue = false;
				} else {
					Close();
				}
			}
		} else {
			str _archive_name;
			ok = FindValidArchiveName(path, _archive_name);

			if (ok) {
				AllocArchiveFile(_archive_name);
				ok = archive_file_->Open(file, ToArchiveMode(mode), endian);
			}

			if (ok) {
				do_continue = false;
			} else {
				Close();
			}
		}

		_split_index++;
	}

	ok = (disk_file_ != 0 || archive_file_ != 0);

	return ok;
}

void MetaFile::Close() {
	if (disk_file_ != 0) {
		disk_file_->Close();
		delete disk_file_;
		disk_file_ = 0;
	} else if (archive_file_ != 0) {
		archive_file_->Close();
		delete archive_file_;
		archive_file_ = 0;
	}
}

void MetaFile::SetEndian(Endian::EndianType endian) {
	endian_ = endian;
	Parent::SetEndian(endian);
	if (disk_file_ != 0) {
		disk_file_->SetEndian(endian_);
	} else if (archive_file_ != 0) {
		archive_file_->SetEndian(endian_);
	}
}

Endian::EndianType MetaFile::GetEndian() {
	return endian_;
}

int64 MetaFile::GetSize() const {
	int64 _size = 0;

	if (disk_file_ != 0) {
		_size = disk_file_->GetSize();
	} else if (archive_file_ != 0) {
		_size = archive_file_->GetSize();
	}

	return _size;
}

int64 MetaFile::Tell() const {
	int64 pos = 0;

	if (disk_file_ != 0) {
		pos = disk_file_->Tell();
	} else if (archive_file_ != 0) {
		pos = archive_file_->Tell();
	}

	return pos;
}

int64 MetaFile::Seek(int64 offset, FileOrigin from) {
	int64 _offset = 0;

	if (disk_file_ != 0) {
		_offset = disk_file_->Seek(offset, from);
	} else if (archive_file_ != 0) {
		_offset = archive_file_->Seek(offset, from);
	}

	return _offset;
}

str MetaFile::GetFullName() const {
	if (disk_file_ != 0) {
		return disk_file_->GetFullName();
	} else if (archive_file_ != 0) {
		return archive_file_->GetFullName();
	}

	return "";
}

str MetaFile::GetName() const {
	if (disk_file_ != 0) {
		return disk_file_->GetName();
	} else if (archive_file_ != 0) {
		return archive_file_->GetName();
	}

	return "";
}

str MetaFile::GetPath() const {
	if (disk_file_ != 0) {
		return disk_file_->GetPath();
	} else if (archive_file_ != 0) {
		return archive_file_->GetPath();
	}

	return "";
}

IOError MetaFile::ReadData(void* buffer, size_t size) {
	IOError error = kIoFileNotOpen;

	if (disk_file_ != 0) {
		error = disk_file_->ReadData(buffer, size);
	} else if (archive_file_ != 0) {
		error = archive_file_->ReadData(buffer, size);
	}

	return error;
}

IOError MetaFile::WriteData(const void* buffer, size_t size) {
	IOError error = kIoFileNotOpen;

	if (disk_file_ != 0) {
		error = disk_file_->WriteData(buffer, size);
	} else if (archive_file_ != 0) {
		error = archive_file_->WriteData(buffer, size);
	}

	return error;
}


int64 MetaFile::GetAvailable() const {
	int64 available = 0;

	if (disk_file_ != 0) {
		available = disk_file_->GetAvailable();
	} else if (archive_file_ != 0) {
		available = archive_file_->GetAvailable();
	}

	return available;
}

IOError MetaFile::ReadRaw(void* buffer, size_t size) {
	IOError error = kIoFileNotOpen;

	if (disk_file_ != 0) {
		error = disk_file_->ReadRaw(buffer, size);
	} else if (archive_file_ != 0) {
		error = archive_file_->ReadRaw(buffer, size);
	}

	return error;
}

IOError MetaFile::Skip(size_t size) {
	return (Parent::Skip(size));
}

IOError MetaFile::WriteRaw(const void* buffer, size_t size) {
	IOError error = kIoFileNotOpen;

	if (disk_file_ != 0) {
		error = disk_file_->WriteRaw(buffer, size);
	} else if (archive_file_ != 0) {
		error = archive_file_->WriteRaw(buffer, size);
	}

	return error;
}

void MetaFile::Flush() {
	if (disk_file_ != 0) {
		disk_file_->Flush();
	} else if (archive_file_ != 0) {
		archive_file_->Flush();
	}
}

void MetaFile::AllocDiskFile() {
	if(reader_ != 0 && writer_ != 0) {
		disk_file_ = new DiskFile(reader_, writer_);
	} else if(reader_ != 0) {
		disk_file_ = new DiskFile(reader_);
	} else if(writer_ != 0) {
		disk_file_ = new DiskFile(writer_);
	} else {
		disk_file_ = new DiskFile();
	}

	disk_file_->SetEndian(endian_);
}

void MetaFile::AllocArchiveFile(const str& archive_name) {
	if(reader_ != 0 && writer_ != 0) {
		archive_file_ = new ArchiveFile(archive_name, reader_, writer_);
	} else if(reader_ != 0) {
		archive_file_ = new ArchiveFile(archive_name, reader_);
	} else if(writer_ != 0) {
		archive_file_ = new ArchiveFile(archive_name, writer_);
	} else {
		archive_file_ = new ArchiveFile(archive_name);
	}


	if (IsZipFile(Path::GetExtension(archive_name))) {
		archive_file_->SetArchiveType(ArchiveFile::kZip);
	} else {
		archive_file_->SetArchiveType(ArchiveFile::kUncompressed);
	}

	archive_file_->SetEndian(endian_);
}

DiskFile::OpenMode MetaFile::ToDiskFileMode(OpenMode mode) {
	DiskFile::OpenMode _mode = DiskFile::kModeRead;
	switch (mode) {
		case kReadOnly: {
			_mode = DiskFile::kModeRead;
			break;
		}
		case kWriteOnly: {
			_mode = DiskFile::kModeWrite;
			break;
		}
		case kWriteAppend: {
			_mode = DiskFile::kModeWriteAppend;
			break;
		}
	}

	return _mode;
}

ArchiveFile::OpenMode MetaFile::ToArchiveMode(OpenMode mode) {
	ArchiveFile::OpenMode _mode = ArchiveFile::kReadOnly;
	switch (mode) {
		case kReadOnly: {
			_mode = ArchiveFile::kReadOnly;
			break;
		}
		case kWriteOnly: {
			_mode = ArchiveFile::kWriteOnly;
			break;
		}
		case kWriteAppend: {
			_mode = ArchiveFile::kWriteAppend;
			break;
		}
	}

	return _mode;
}


bool MetaFile::IsZipFile(const str& extension) {
	bool ok = false;

	if (zip_extensions_ != 0) {
		ok = (std::find(zip_extensions_->begin(), zip_extensions_->end(), extension) != zip_extensions_->end());
	}
	return ok;
}

bool MetaFile::IsUncompressedArchive(const str& extension) {
	bool ok = false;

	if (archive_extensions_ != 0) {
		ok = (std::find(archive_extensions_->begin(), archive_extensions_->end(), extension) != archive_extensions_->end());
	}

	return ok;
}

void MetaFile::AddZipExtension(const str& extension) {
	if (zip_extensions_ == 0) {
		zip_extensions_ = new std::list<str>();
	}

	zip_extensions_->push_back(extension);
	zip_extensions_->sort();
	zip_extensions_->unique();
}

void MetaFile::AddUncompressedExtension(const str& extension) {
	if (archive_extensions_ == 0) {
		archive_extensions_ = new std::list<str>();
	}

	archive_extensions_->push_back(extension);
	archive_extensions_->sort();
	archive_extensions_->unique();
}

void MetaFile::ClearExtensions() {
	if (zip_extensions_) {
		zip_extensions_->clear();
		delete zip_extensions_;
		zip_extensions_ = 0;
	}

	if (archive_extensions_) {
		archive_extensions_->clear();
		delete archive_extensions_;
		archive_extensions_ = 0;
	}
}

std::list<str>* MetaFile::zip_extensions_;
std::list<str>* MetaFile::archive_extensions_;

bool MetaFile::SplitPath(const str& filename, str& left, str& right, size_t split_index) {
	bool ok = true;
	size_t _split_index = filename.length();
	size_t i;

	for (i = 0; i < split_index; i++) {
		int index1 = (int)filename.rfind((char)'/', _split_index-1);
		int index2 = (int)filename.rfind((char)'\\', _split_index-1);

		if (index1 > index2) {
			_split_index = index1;
		} else if (index2 > index1) {
			_split_index = index2;
		} else if (index1 == -1 && index2 == -1) {
			_split_index = 0;
			ok = false;
			break;
		}
	}

	left = filename.substr(0, _split_index);
	right = filename.substr(_split_index);

	if (!right.empty() && (right[0] == '/' || right[0] == '\\')) {
		right.erase(0, 1);
	}

	return ok;
}

bool MetaFile::FindValidArchiveName(const str& archive_prefix, str& full_archive_name) {
	std::list<str>::iterator iter;

	bool ok = false;

	if (zip_extensions_ != 0) {
		for (iter = zip_extensions_->begin(); iter != zip_extensions_->end(); ++iter) {
			str _file_name(archive_prefix + (*iter));
			if (DiskFile::Exists(_file_name) == true) {
				full_archive_name = _file_name;
				ok = true;
				break;
			}
		}
	}

	if (ok == false && archive_extensions_ != 0) {
		for (iter = archive_extensions_->begin(); iter != archive_extensions_->end(); ++iter) {
			str _file_name(archive_prefix + (*iter));
			if (DiskFile::Exists(_file_name) == true) {
				full_archive_name = _file_name;
				ok = true;
				break;
			}
		}
	}

	return ok;
}

}
