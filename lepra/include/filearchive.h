/*
	Class:  FileArchive
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

	NOTES:

	This class is used to concatenate several files into one large file.
	No compression is performed though. If you need a compressed archive,
	please use ZipArchive instead. Use this class if read/write performance
	is an issue.

	QUICK DOCUMENTATION (OBS! READ THIS BEFORE USING THIS CLASS!!!):

	1. Create a new instance of FileArchive, and give the constructor
	   the name of the concat-file to work with.

	2. Call OpenArchive() to start reading from or writing to the file.
	   When done, call CloseArchive().

	3. FileFindFirst(), FileFindNext(), FileSize(), FileSetPos() and
	   FileRead() only works in kReadOnly mode.

	4. FileOpen() only returns a filehandle in kReadOnly mode. Otherwise
	   it just returns 1 if successful.

	5. In kWriteOnly mode, you can only write to one file at a time, and
	   that's the last opened file (no file handles are used). When you
	   are done writing, and opened another file to write to, you can't go
	   back writing to the previous file again. Just send 0 as the
	   filehandle when calling FileClose().

	6. InsertArchive() can only be used in kInsertOnly mode, and that's
	   the only function that can be used in that mode. It inserts a
	   FileArchive package into "this" package. If there are two files
	   (one in each package) with the same name, the file in "this" package
	   will be overwritten.
*/

#pragma once

#include "lepratypes.h"
#include "hashtable.h"
#include "diskfile.h"
#include "ioerror.h"
#include <list>

namespace lepra {

class ProgressCallback;

class FileArchive {
public:

	enum IOType {
		kReadOnly = 0,
		kWriteOnly,
		kWriteAppend,
		kInsertOnly
	};

	enum FileOrigin {
		kFseekSet = 0,
		kFseekCur,
		kFseekEnd,
	};

	enum SizeUnit {
		kBytes = 1,
		kKb    = 1024,
		kMb    = 1024 * kKb,
	};

	FileArchive();
	virtual ~FileArchive();

	IOError OpenArchive(const str& archive_file_name, IOType io_type);
	void CloseArchive();
	void CloseAndRemoveArchive();

	int GetFileCount();

	IOError InsertArchive(const str& archive_file_name);

	str FileFindFirst();
	str FileFindNext();

	int FileOpen(const str& file_name);
	void FileClose(int file_handle);

	bool FileExist(const str& file_name);

	IOError FileRead(void* dest, int size, int file_handle);
	IOError FileWrite(void* source, int size);

	int64 FileSize(int file_handle);
	void FileSeek(int64 offset, FileOrigin origin, int file_handle);

	void SetProgressCallback(ProgressCallback* callback);

	bool AddFile(const str& file_name, const str& dest_file_name,
		     int cache_size = 1, SizeUnit unit = kMb);
	bool ExtractFile(const str& file_name, const str& dest_file_name,
			 int cache_size = 1, SizeUnit unit = kMb);

private:

	class FileArchiveFile {
	public:
		FileArchiveFile(const str& file_name) :
			file_name_(file_name) {
		}

		~FileArchiveFile() {
		}

		str file_name_;
		int64 start_offset_;
		int64 current_pos_;
		int64 size_;
	};


	typedef HashTable<int, FileArchiveFile*> FileTable;
	typedef HashTable<str, FileArchiveFile*> FileNameTable;
	typedef std::list<FileArchiveFile*> FileList;

	IOError ReadHeader(int64* header_offset = 0, bool fill_file_name_list = false);
	IOError WriteHeader();

	FileArchiveFile* GetFile(int file_handle);

	FileTable open_file_table_;
	FileNameTable file_name_table_;
	FileList file_name_list_;  // Only used in kWriteOnly mode.

	str archive_file_name_;
	str temp_file_name_;

	DiskFile archive_file_;
	IOType io_type_;

	FileNameTable::Iterator current_find_iterator_;

	FileArchiveFile* current_write_file_;

	int file_handle_counter_;

	uint8* write_buffer_;
	const int write_buffer_size_;
	int write_buffer_pos_;
	int64 current_write_pos_;

	ProgressCallback* callback_;
};

}
