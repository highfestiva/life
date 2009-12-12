/*
	Class:  MetaFile
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games

	NOTES:

	This class abstracts the usage of files and archives (such as .zip files).
	Example:

	Given a path, let's say "C:\My\Data\Directory\File.Extension", the default
	procedure is as follows:

	1. Check if the file "C:\My\Data\Directory\File.Extension" exists. If it
	   does, open it. Otherwise, go on to step 2.
	2. Check if the file "C:\My\Data\Directory.zip" exists. If it does, open
	   it and check if it contains the file "File.Extension". If so, open the
	   file. Otherwise, go on to step 3.
	3. Check if the file "C:\My\Data.zip" exists. If it does, open it and check
	   if it contains the file "Directory\File.Extension"...

	And on it goes...

	The ".zip"-extension can be replaced by other extensions. Extensions for
	uncompressed archives may be used as well. When searching for archives
	the zip-file extensions will be tested first (and thus prioritized).
*/



#pragma once



#include "Lepra.h"
#include "File.h"
#include "InputStream.h"
#include "OutputStream.h"
#include "DiskFile.h"
#include "ArchiveFile.h"
#include "String.h"
#include <list>



namespace Lepra
{



class MetaFile: public File, protected InputStream, protected OutputStream
{
	typedef File Parent;
public:
	enum OpenMode
	{
		READ_ONLY = 0,
		WRITE_ONLY,
		WRITE_APPEND,
	};

	MetaFile();
	MetaFile(Reader* pReader);
	MetaFile(Writer* pWriter);
	MetaFile(Reader* pReader, Writer* pWriter);
	virtual ~MetaFile();

	bool Open(const String& pFileName, OpenMode pMode, bool pCreatePath = false, Endian::EndianType pEndian = Endian::TYPE_BIG_ENDIAN);
	void Close();

	void SetEndian(Endian::EndianType pEndian);
	Endian::EndianType GetEndian();

	int64 GetSize() const;

	int64 Tell() const;
	int64 Seek(int64 pOffset, FileOrigin pFrom);

	String GetFullName() const;	// Returns path+filename.
	String GetName() const;		// Returns filename only.
	String GetPath() const;		// Returns path.

	// Overrided from Reader/Writer.
	IOError ReadData(void* pBuffer, size_t pSize);
	IOError WriteData(const void* pBuffer, size_t pSize);

	static void AddZipExtension(const String& pExtension);
	static void AddUncompressedExtension(const String& pExtension);
	static void ClearExtensions();

//protected:
	// Overrided from InputStream.
	int64 GetAvailable() const;
	virtual IOError ReadRaw(void* pBuffer, size_t pSize);
	IOError Skip(size_t pSize);

	// Overrided from OutputStream.
	virtual IOError WriteRaw(const void* pBuffer, size_t pSize);
	void Flush();

private:

	void AllocDiskFile();
	void AllocArchiveFile(const String& pArchiveName);
	DiskFile::OpenMode ToDiskFileMode(OpenMode pMode);
	ArchiveFile::OpenMode ToArchiveMode(OpenMode pMode);

	static std::list<String>* smZipExtensions;
	static std::list<String>* smArchiveExtensions;

	static bool IsZipFile(const String& pExtension);
	static bool IsUncompressedArchive(const String& pExtension);

	// Splits the filename into an archive name and a filename at
	// the '/' pointed to by pSplitIndex, counting from right to
	// left. Examples:
	//
	// pFilename = "C:/My/Current/Directory/Filename.file"
	//
	// pSplitIndex = 0 returns pLeft = "C:/My/Current/Directory/Filename.file" and pRight = "".
	// pSplitIndex = 1 returns pLeft = "C:/My/Current/Directory" and pFile = "Filename.file".
	static bool SplitPath(const String& pFilename, String& pLeft, String& pRight, size_t pSplitIndex);

	static bool FindValidArchiveName(const String& pArchivePrefix, String& pFullArchiveName);

	DiskFile* mDiskFile;
	ArchiveFile* mArchiveFile;

	Reader* mReader;
	Writer* mWriter;

	Endian::EndianType mEndian;
};



}
