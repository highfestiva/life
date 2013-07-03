
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include <stdio.h>

#include "Lepra.h"
#include "File.h"
#include "InputStream.h"
#include "OutputStream.h"
#include "../Include/FileArchive.h"
#include "../Include/ZipArchive.h"



namespace Lepra
{



class FileArchive;



class ArchiveFile : public File, protected InputStream, protected OutputStream
{
	typedef File Parent;
public:
	enum OpenMode
	{
		READ_ONLY = 0,
		WRITE_ONLY,
		WRITE_APPEND,
	};

	enum ArchiveType
	{
		UNCOMPRESSED = 0,
		ZIP,
	};

	ArchiveFile(const str& pArchiveName);
	ArchiveFile(const str& pArchiveName, Reader* pReader);
	ArchiveFile(const str& pArchiveName, Writer* pWriter);
	ArchiveFile(const str& pArchiveName, Reader* pReader, Writer* pWriter);
	virtual ~ArchiveFile();

	void SetArchiveType(ArchiveType pType);

	bool Open(const str& pFileName, OpenMode pMode, Endian::EndianType pEndian = Endian::TYPE_BIG_ENDIAN);

	void Close();

	bool IsOpen() const;

	// Use this to change the endian in the middle of a file read/write.
	void SetEndian(Endian::EndianType pEndian);

	str GetFullName() const;	// Returns path+filename.
	str GetName() const;		// Returns filename only.
	str GetPath() const;		// Returns path.

	int64 GetSize() const;

	// Overrided from Reader/Writer.
	IOError ReadData(void* pBuffer, size_t pSize);
	IOError WriteData(const void* pBuffer, size_t pSize);

	int64 Tell() const;
	int64 Seek(int64 pOffset, FileOrigin pFrom);

	// OBS! The following functions will overwrite any existing file with the 
	// name pFileName. If pLocal is set to true, the file will be written
	// directly in the "current" directory or archive, as a local file. 
	// With other words, the path is not taken into account.
	static bool ExtractFileFromArchive(const str& pArchiveName, const str& pFileName, bool pLocal);
	static bool ExtractFileFromArchive(const str& pArchiveName, const str& pFileName, const str& pExtractedFileName, bool pLocal);
	static bool InsertFileIntoArchive(const str& pArchiveName, const str& pFileName, bool pLocal);
	static bool InsertFileIntoArchive(const str& pArchiveName, const str& pFileName, const str& pInsertedFileName, bool pLocal);

	// Overrided from InputStream.
	int64 GetAvailable() const;
	virtual IOError ReadRaw(void* pBuffer, size_t pSize);
	IOError Skip(size_t pSize);

	// Overrided from OutputStream.
	virtual IOError WriteRaw(const void* pBuffer, size_t pSize);
	void Flush();
private:

	bool OpenForReading(const str& pFileName, OpenMode pMode);
	bool OpenZipForReading(const str& pFileName, OpenMode pMode);
	bool OpenForWriting(const str& pFileName, OpenMode pMode);
	bool OpenZipForWriting(const str& pFileName, OpenMode pMode);

	bool FlushDataBuffer();
	bool RefillDataBuffer();

	void ExtractPathAndFileName(const str& pFileName);

	void CopyArchiveFiles(FileArchive& pSource, FileArchive& pDest, const str& pExceptThisFile);
	void CopyZipArchiveFiles(ZipArchive& pSource, ZipArchive& pDest, const str& pExceptThisFile);

	bool CopyFileBetweenArchives(FileArchive& pSource, FileArchive& pDest, const str& pFileName);
	bool CopyFileBetweenZipArchives(ZipArchive& pSource, ZipArchive& pDest, const str& pFileName);

	void operator=(const ArchiveFile&);

	Endian::EndianType mFileEndian;

	str mArchiveFileName;
	bool mIsZipArchive;

	ZipArchive mZipArchive;
	FileArchive mArchive;
	int mFileHandle;

	str mFileName;
	str mPath;

	int64 mFileSize;

	uint8* mDataBuffer;
	const int mDataBufferSize;	// Size of the data buffer.
	int mDataSize;				// Size of data in data buffer.
	int mCurrentDBPos;			// Current position in data buffer.
	int64 mCurrentPos;

	// Usually just NULL. Can be set by the user to redirect the IO
	// through another reader/writer.
	Writer* mWriter;
	Reader* mReader;

	OpenMode mMode;
};



}
