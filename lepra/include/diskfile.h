
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine
//
// NOTES:
//
// A file reader/writer. This class is a real monster! :)
//
// It is a monster because of its inheritance... But this makes your life
// much easier if you use it in the right way. Here are the two major features:
//
// 1. Automatic conversion between big/little endian byte ordering. Default
//    is little endian (Intel standard).
// 2. Implement your own reader/writer and save your files with your own
//    encryption scheme.



#pragma once



#include <stdio.h>

#include "lepra.h"
#include "file.h"
#include "inputstream.h"
#include "outputstream.h"

#ifdef LEPRA_POSIX
#include <sys/types.h>
#include <dirent.h>
#include <glob.h>
#endif // LEPRA_POSIX



namespace lepra {



class FileArchive;



class DiskFile: public File, protected InputStream, protected OutputStream {
	typedef File Parent;
public:
	// Not really an enum, but a bit field.
	enum OpenMode {
		kModeText		= 0x01,
		kModeRead		= 0x02,
		kModeTextRead		= (kModeRead|kModeText),
		kModeWrite		= 0x04,
		kModeTextWrite		= (kModeWrite|kModeText),
		kModeTextReadWrite	= (kModeWrite|kModeRead|kModeText),
		kModeWriteAppend	= 0x08,
		kModeTextWriteAppend	= (kModeWriteAppend|kModeText),
	};

	class FindData {
	public:
		friend class DiskFile;

		FindData() :
#if defined LEPRA_WINDOWS
			find_handle_(-1),
#elif defined LEPRA_POSIX
			glob_list_(),
			glob_index_(0xffff),
#endif
			file_spec_(),
			name_(),
			sub_dir_(false),
			size_(0),
			time_(-1) {
		}

		~FindData() {
		}

		inline str& GetName() {
			return name_;
		}

		inline const str& GetName() const {
			return name_;
		}

		inline uint64 GetSize() {
			return size_;
		}

		inline int64 GetTimeTag() {
			return time_;
		}

		inline bool IsSubDir() {
			return sub_dir_;
		}

		inline void Clear() {
			file_spec_.clear();
			name_.clear();
			size_ = 0;
			sub_dir_ = false;
			time_ = -1;
		}

	private:
#if defined LEPRA_WINDOWS
		intptr_t find_handle_;
#elif defined LEPRA_POSIX
		glob_t glob_list_;
		size_t glob_index_;
#else
#error DiskFile::FindData is not properly implemented on this platform!
#endif // LEPRA_POSIX

		str file_spec_;
		str name_;
		bool sub_dir_;
		uint64 size_;
		int64 time_;
	};

	DiskFile();
	DiskFile(Reader* reader);
	DiskFile(Writer* writer);
	DiskFile(Reader* reader, Writer* writer);
	virtual ~DiskFile();

	// If create_path is set to true, the directory structure given by
	// file_name will be created if it doesn't already exist, and if
	// mode is set to kWriteOnly or kWriteAppend.
	bool Open(const str& file_name, OpenMode mode, bool create_path = false, Endian::EndianType endian = Endian::kTypeBigEndian);

	void Close();

	bool IsOpen() const;

	// Use this to change the endian in the middle of a file read/write.
	void SetEndian(Endian::EndianType endian);

	str GetFullName() const;	// Returns path+filename.
	str GetName() const;		// Returns filename only.
	str GetPath() const;		// Returns path.

	virtual int64 GetSize() const;

	// Overrided from Reader/Writer.
	IOError ReadData(void* buffer, size_t size);
	IOError WriteData(const void* buffer, size_t size);

	virtual void Flush();

	virtual int64 Tell() const;
	virtual int64 Seek(int64 offset, FileOrigin from);

	// Static functions.

	static IOError Load(const str& filename, void** data, int64& data_size);	// Retries a few times to avoid anti-virus glitches.

	static bool Exists(const str& file_name);
	static bool PathExists(const str& path_name);
	static bool Delete(const str& file_name);
	static bool Rename(const str& old_file_name, const str& new_file_name);
	static bool CreateDir(const str& path_name);
	static bool RemoveDir(const str& path_name);

	static bool FindFirst(const str& file_spec, FindData& find_data);
	static bool FindNext(FindData& find_data);

	// Generates a unique random file name for the given path. The filename can then
	// be used to create a temp-file.
	static str GenerateUniqueFileName(const str& path);

	// Overrided from InputStream.
	int64 GetAvailable() const;
	virtual IOError ReadRaw(void* buffer, size_t size);
	IOError Skip(size_t size);

	// Overrided from OutputStream.
	virtual IOError WriteRaw(const void* buffer, size_t size);
private:
	void ExtractPathAndFileName(const str& file_name);

	bool CreateSubDirs();

	static FILE* FileOpen(const str& file_name, const str& mode);

	//void CopyArchiveFiles(FileArchive& source, FileArchive& dest, const str& except_this_file);
	//bool CopyFileBetweenArchives(FileArchive& source, FileArchive& dest, const str& file_name);

	void operator=(const DiskFile&);

	FILE* file_;
	str file_name_;
	str path_;

	int64 file_size_;

	// Usually just NULL. Can be set by the user to redirect the IO
	// through another reader/writer.
	Writer* writer_;
	Reader* reader_;

	OpenMode mode_;
};



}
