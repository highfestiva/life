/*
	Class:  MetaFile
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

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



#include "lepra.h"
#include "file.h"
#include "inputstream.h"
#include "outputstream.h"
#include "diskfile.h"
#include "archivefile.h"
#include "string.h"
#include <list>



namespace lepra {



class MetaFile: public File, protected InputStream, protected OutputStream {
	typedef File Parent;
public:
	enum OpenMode {
		kReadOnly = 0,
		kWriteOnly,
		kWriteAppend,
	};

	MetaFile();
	MetaFile(Reader* reader);
	MetaFile(Writer* writer);
	MetaFile(Reader* reader, Writer* writer);
	virtual ~MetaFile();

	bool Open(const str& file_name, OpenMode mode, bool create_path = false, Endian::EndianType endian = Endian::kTypeBigEndian);
	void Close();

	void SetEndian(Endian::EndianType endian);
	Endian::EndianType GetEndian();

	int64 GetSize() const;

	int64 Tell() const;
	int64 Seek(int64 offset, FileOrigin from);

	str GetFullName() const;	// Returns path+filename.
	str GetName() const;		// Returns filename only.
	str GetPath() const;		// Returns path.

	// Overrided from Reader/Writer.
	IOError ReadData(void* buffer, size_t size);
	IOError WriteData(const void* buffer, size_t size);

	static void AddZipExtension(const str& extension);
	static void AddUncompressedExtension(const str& extension);
	static void ClearExtensions();

//protected:
	// Overrided from InputStream.
	int64 GetAvailable() const;
	virtual IOError ReadRaw(void* buffer, size_t size);
	IOError Skip(size_t size);

	// Overrided from OutputStream.
	virtual IOError WriteRaw(const void* buffer, size_t size);
	void Flush();

private:

	void AllocDiskFile();
	void AllocArchiveFile(const str& archive_name);
	DiskFile::OpenMode ToDiskFileMode(OpenMode mode);
	ArchiveFile::OpenMode ToArchiveMode(OpenMode mode);

	static std::list<str>* zip_extensions_;
	static std::list<str>* archive_extensions_;

	static bool IsZipFile(const str& extension);
	static bool IsUncompressedArchive(const str& extension);

	// Splits the filename into an archive name and a filename at
	// the '/' pointed to by split_index, counting from right to
	// left. Examples:
	//
	// filename = "C:/My/Current/Directory/Filename.file"
	//
	// split_index = 0 returns left = "C:/My/Current/Directory/Filename.file" and right = "".
	// split_index = 1 returns left = "C:/My/Current/Directory" and file = "Filename.file".
	static bool SplitPath(const str& filename, str& left, str& right, size_t split_index);

	static bool FindValidArchiveName(const str& archive_prefix, str& full_archive_name);

	DiskFile* disk_file_;
	ArchiveFile* archive_file_;

	Reader* reader_;
	Writer* writer_;

	Endian::EndianType endian_;
};



}
