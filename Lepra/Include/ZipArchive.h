/*
	Class:  ZipArchive
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games

	NOTES:

	This class is used to concatenate several files into one compressed
	zip-file.

	QUICK DOCUMENTATION (OBS! READ THIS BEFORE USING THIS CLASS!!!):

	1. Create a new instance of ZipArchive.

	2. Call OpenArchive() to start reading from or writing to a zip-file.
	   When done, call CloseArchive().

	3. FileFindFirst(), FileFindNext(), FileSize(), FileSetPos() and
	   FileRead() only works in READ_ONLY mode.

	4. FileOpen() only returns true if the file was opened successfully.
	   There can only be one opened file at a time.

	5. In WRITE_ONLY mode... When you are done writing, and opened another 
	   file to write to, you can't go back writing to the previous file again.

	6. InsertArchive() can only be used in INSERT_ONLY mode, and that's
	   the only function that can be used in that mode. It inserts a
	   ZipArchive package into "this" package. If there are two files
	   (one in each package) with the same name, the file in "this" package
	   will be overwritten.
*/

#ifndef LEPRA_ZIPARCHIVE_H
#define LEPRA_ZIPARCHIVE_H

#include "LepraTypes.h"
#include "IOError.h"
#include "DiskFile.h"
#include "String.h"
#include "HashTable.h"
#include "../../ThirdParty/minizip/unzip.h"
#include "../../ThirdParty/minizip/zip.h"

namespace Lepra
{

class ZipArchive
{
public:
	enum IOType
	{
		READ_ONLY = 0,
		WRITE_ONLY,
		WRITE_APPEND,
		INSERT_ONLY
	};

	enum FileOrigin
	{
		FSEEK_SET = 0,
		FSEEK_CUR,
		FSEEK_END,
	};

	enum SizeUnit
	{
		BYTES = 1,
		KB    = 1024,
		MB    = 1024 * KB,
	};

	ZipArchive();
	virtual ~ZipArchive();

	IOError OpenArchive(const String& pArchiveFileName, IOType pIOType);
	void CloseArchive();
	void CloseAndRemoveArchive();

	int GetFileCount();

	IOError InsertArchive(const String& pArchiveFileName);

	String FileFindFirst();
	String FileFindNext();

	bool FileOpen(const String& pFileName);
	void FileClose();

	bool FileExist(const String& pFileName);

	IOError FileRead(void* pDest, int pSize);
	IOError FileWrite(void* pSource, int pSize);

	int64 FileSize();
	void FileSeek(int64 pOffset, FileOrigin pOrigin);

	bool ExtractFile(const String& pFileName, 
			 const String& pDestFileName,
			 int pBufferSize, SizeUnit pUnit);

private:

	String mArchiveName;

	IOType mIOType;
	
	unzFile mUnzipFile;
	zipFile mZipFile;

	DiskFile mOutFile;

	int64 mFileSize;	// The uncompressed size of the current file.

	LOG_CLASS_DECLARE();
};

} // End namespace.

#endif
