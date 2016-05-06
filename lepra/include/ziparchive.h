/*
	Class:  ZipArchive
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

	NOTES:

	This class is used to concatenate several files into one compressed
	zip-file.

	QUICK DOCUMENTATION (OBS! READ THIS BEFORE USING THIS CLASS!!!):

	1. Create a new instance of ZipArchive.

	2. Call OpenArchive() to start reading from or writing to a zip-file.
	   When done, call CloseArchive().

	3. FileFindFirst(), FileFindNext(), FileSize(), FileSetPos() and
	   FileRead() only works in kReadOnly mode.

	4. FileOpen() only returns true if the file was opened successfully.
	   There can only be one opened file at a time.

	5. In kWriteOnly mode... When you are done writing, and opened another
	   file to write to, you can't go back writing to the previous file again.

	6. InsertArchive() can only be used in kInsertOnly mode, and that's
	   the only function that can be used in that mode. It inserts a
	   ZipArchive package into "this" package. If there are two files
	   (one in each package) with the same name, the file in "this" package
	   will be overwritten.
*/



#pragma once

#include "lepratypes.h"
#include "ioerror.h"
#include "diskfile.h"
#include "string.h"
#include "hashtable.h"
#include "../../thirdparty/minizip/unzip.h"
#include "../../thirdparty/minizip/zip.h"



namespace lepra {



class ZipArchive {
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

	ZipArchive();
	virtual ~ZipArchive();

	IOError OpenArchive(const str& archive_file_name, IOType io_type);
	void CloseArchive();
	void CloseAndRemoveArchive();

	int GetFileCount();

	IOError InsertArchive(const str& archive_file_name);

	str FileFindFirst();
	str FileFindNext();

	bool FileOpen(const str& file_name);
	void FileClose();

	bool FileExist(const str& file_name);

	IOError FileRead(void* dest, int size);
	IOError FileWrite(void* source, int size);

	int64 FileSize();
	void FileSeek(int64 offset, FileOrigin origin);

	bool ExtractFile(const str& file_name,
			 const str& dest_file_name,
			 int buffer_size, SizeUnit unit);

private:

	str archive_name_;

	IOType io_type_;

	unzFile unzip_file_;
	zipFile zip_file_;

	DiskFile out_file_;

	int64 file_size_;	// The uncompressed size of the current file.

	logclass();
};



}
