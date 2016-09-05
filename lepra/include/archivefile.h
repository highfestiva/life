
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include <stdio.h>

#include "lepra.h"
#include "file.h"
#include "inputstream.h"
#include "outputstream.h"
#include "../include/filearchive.h"
#include "../include/ziparchive.h"



namespace lepra {



class FileArchive;



class ArchiveFile : public File, protected InputStream, protected OutputStream {
	typedef File Parent;
public:
	enum OpenMode {
		kReadOnly = 0,
		kWriteOnly,
		kWriteAppend,
	};

	enum ArchiveType {
		kUncompressed = 0,
		kZip,
	};

	ArchiveFile(const str& archive_name);
	ArchiveFile(const str& archive_name, Reader* reader);
	ArchiveFile(const str& archive_name, Writer* writer);
	ArchiveFile(const str& archive_name, Reader* reader, Writer* writer);
	virtual ~ArchiveFile();

	void SetArchiveType(ArchiveType type);

	bool Open(const str& file_name, OpenMode mode, Endian::EndianType endian = Endian::kTypeBigEndian);

	void Close();

	bool IsOpen() const;

	// Use this to change the endian in the middle of a file read/write.
	void SetEndian(Endian::EndianType endian);

	str GetFullName() const;	// Returns path+filename.
	str GetName() const;		// Returns filename only.
	str GetPath() const;		// Returns path.

	int64 GetSize() const;

	// Overrided from Reader/Writer.
	IOError ReadData(void* buffer, size_t size);
	IOError WriteData(const void* buffer, size_t size);

	int64 Tell() const;
	int64 Seek(int64 offset, FileOrigin from);

	// OBS! The following functions will overwrite any existing file with the
	// name file_name. If local is set to true, the file will be written
	// directly in the "current" directory or archive, as a local file.
	// With other words, the path is not taken into account.
	static bool ExtractFileFromArchive(const str& archive_name, const str& file_name, bool local);
	static bool ExtractFileFromArchive(const str& archive_name, const str& file_name, const str& extracted_file_name, bool local);
	static bool InsertFileIntoArchive(const str& archive_name, const str& file_name, bool local);
	static bool InsertFileIntoArchive(const str& archive_name, const str& file_name, const str& inserted_file_name, bool local);

	// Overrided from InputStream.
	int64 GetAvailable() const;
	virtual IOError ReadRaw(void* buffer, size_t size);
	IOError Skip(size_t size);

	// Overrided from OutputStream.
	virtual IOError WriteRaw(const void* buffer, size_t size);
	void Flush();
private:

	bool OpenForReading(const str& file_name, OpenMode mode);
	bool OpenZipForReading(const str& file_name, OpenMode mode);
	bool OpenForWriting(const str& file_name, OpenMode mode);
	bool OpenZipForWriting(const str& file_name, OpenMode mode);

	bool FlushDataBuffer();
	bool RefillDataBuffer();

	void ExtractPathAndFileName(const str& file_name);

	void CopyArchiveFiles(FileArchive& source, FileArchive& dest, const str& except_this_file);
	void CopyZipArchiveFiles(ZipArchive& source, ZipArchive& dest, const str& except_this_file);

	bool CopyFileBetweenArchives(FileArchive& source, FileArchive& dest, const str& file_name);
	bool CopyFileBetweenZipArchives(ZipArchive& source, ZipArchive& dest, const str& file_name);

	void operator=(const ArchiveFile&);

	str archive_file_name_;
	bool is_zip_archive_;

	ZipArchive zip_archive_;
	FileArchive archive_;
	int file_handle_;

	str file_name_;
	str path_;

	int64 file_size_;

	uint8* data_buffer_;
	const int data_buffer_size_;	// Size of the data buffer.
	int data_size_;				// Size of data in data buffer.
	int current_db_pos_;			// Current position in data buffer.
	int64 current_pos_;

	// Usually just NULL. Can be set by the user to redirect the IO
	// through another reader/writer.
	Writer* writer_;
	Reader* reader_;

	OpenMode mode_;
};



}
