
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



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

	ArchiveFile(const String& pArchiveName);
	ArchiveFile(const String& pArchiveName, Reader* pReader);
	ArchiveFile(const String& pArchiveName, Writer* pWriter);
	ArchiveFile(const String& pArchiveName, Reader* pReader, Writer* pWriter);
	virtual ~ArchiveFile();

	void SetArchiveType(ArchiveType pType);

	bool Open(const String& pFileName, OpenMode pMode, Endian::EndianType pEndian = Endian::TYPE_LITTLE_ENDIAN);

	void Close();

	bool IsOpen() const;

	// Use this to change the endian in the middle of a file read/write.
	void SetEndian(Endian::EndianType pEndian);
	Endian::EndianType GetEndian();

	String GetFullName() const;	// Returns path+filename.
	String GetName() const;		// Returns filename only.
	String GetPath() const;		// Returns path.

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
	static bool ExtractFileFromArchive(const String& pArchiveName, const String& pFileName, bool pLocal);
	static bool ExtractFileFromArchive(const String& pArchiveName, const String& pFileName, const String& pExtractedFileName, bool pLocal);
	static bool InsertFileIntoArchive(const String& pArchiveName, const String& pFileName, bool pLocal);
	static bool InsertFileIntoArchive(const String& pArchiveName, const String& pFileName, const String& pInsertedFileName, bool pLocal);

	// Overrided from InputStream.
	int64 GetAvailable() const;
	IOError ReadRaw(void* pBuffer, size_t pSize);
	IOError Skip(size_t pSize);

	// Overrided from OutputStream.
	IOError WriteRaw(const void* pBuffer, size_t pSize);
	void Flush();
private:

	bool OpenForReading(const String& pFileName, OpenMode pMode);
	bool OpenZipForReading(const String& pFileName, OpenMode pMode);
	bool OpenForWriting(const String& pFileName, OpenMode pMode);
	bool OpenZipForWriting(const String& pFileName, OpenMode pMode);

	bool FlushDataBuffer();
	bool RefillDataBuffer();

	void ExtractPathAndFileName(const String& pFileName);

	void CopyArchiveFiles(FileArchive& pSource, FileArchive& pDest, const String& pExceptThisFile);
	void CopyZipArchiveFiles(ZipArchive& pSource, ZipArchive& pDest, const String& pExceptThisFile);

	bool CopyFileBetweenArchives(FileArchive& pSource, FileArchive& pDest, const String& pFileName);
	bool CopyFileBetweenZipArchives(ZipArchive& pSource, ZipArchive& pDest, const String& pFileName);

	void operator=(const ArchiveFile&);

	Endian::EndianType mFileEndian;

	String mArchiveFileName;
	bool mIsZipArchive;

	ZipArchive mZipArchive;
	FileArchive mArchive;
	int mFileHandle;

	String mFileName;
	String mPath;

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
