/*
	Class:  FileArchive
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games

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
	   FileRead() only works in READ_ONLY mode.

	4. FileOpen() only returns a filehandle in READ_ONLY mode. Otherwise
	   it just returns 1 if successful.

	5. In WRITE_ONLY mode, you can only write to one file at a time, and
	   that's the last opened file (no file handles are used). When you
	   are done writing, and opened another file to write to, you can't go
	   back writing to the previous file again. Just send 0 as the
	   filehandle when calling FileClose().

	6. InsertArchive() can only be used in INSERT_ONLY mode, and that's
	   the only function that can be used in that mode. It inserts a
	   FileArchive package into "this" package. If there are two files
	   (one in each package) with the same name, the file in "this" package
	   will be overwritten.
*/

#ifndef LEPRA_FILEARCHIVE_H
#define LEPRA_FILEARCHIVE_H

#include "LepraTypes.h"
#include "HashTable.h"
#include "DiskFile.h"
#include "IOError.h"
#include <list>

namespace Lepra
{

class ProgressCallback;

class FileArchive
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

	FileArchive();
	virtual ~FileArchive();

	IOError OpenArchive(const str& pArchiveFileName, IOType pIOType);
	void CloseArchive();
	void CloseAndRemoveArchive();

	int GetFileCount();

	IOError InsertArchive(const str& pArchiveFileName);

	str FileFindFirst();
	str FileFindNext();

	int FileOpen(const str& pFileName);
	void FileClose(int pFileHandle);

	bool FileExist(const str& pFileName);

	IOError FileRead(void* pDest, int pSize, int pFileHandle);
	IOError FileWrite(void* pSource, int pSize);

	int64 FileSize(int pFileHandle);
	void FileSeek(int64 pOffset, FileOrigin pOrigin, int pFileHandle);

	void SetProgressCallback(ProgressCallback* pCallback);

	bool AddFile(const str& pFileName, const str& pDestFileName, 
		     int pCacheSize = 1, SizeUnit pUnit = MB);
	bool ExtractFile(const str& pFileName, const str& pDestFileName, 
			 int pCacheSize = 1, SizeUnit pUnit = MB);

private:

	class FileArchiveFile
	{
	public:
		FileArchiveFile(const str& pFileName) :
			mFileName(pFileName)
		{
		}

		~FileArchiveFile()
		{
		}

		str mFileName;
		int64 mStartOffset;
		int64 mCurrentPos;
		int64 mSize;
	};


	typedef HashTable<int, FileArchiveFile*> FileTable;
	typedef HashTable<str, FileArchiveFile*> FileNameTable;
	typedef std::list<FileArchiveFile*> FileList;

	IOError ReadHeader(int64* pHeaderOffset = 0, bool pFillFileNameList = false);
	IOError WriteHeader();

	FileArchiveFile* GetFile(int pFileHandle);

	FileTable mOpenFileTable;
	FileNameTable mFileNameTable;
	FileList mFileNameList;  // Only used in WRITE_ONLY mode.

	str mArchiveFileName;
	str mTempFileName;

	DiskFile mArchiveFile;
	IOType mIOType;

	int mCurrentFindIndex;
	FileNameTable::Iterator mCurrentFindIterator;

	FileArchiveFile* mCurrentWriteFile;

	int mFileHandleCounter;

	uint8* mWriteBuffer;
	const int mWriteBufferSize;
	int mWriteBufferPos;
	int64 mCurrentWritePos;

	ProgressCallback* mCallback;
};

} // End namespace.

#endif
