
// Author: Alexander Hugestrand
// Copyright (c) 2002-2006, Righteous Games
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

#include "Lepra.h"
#include "File.h"
#include "InputStream.h"
#include "OutputStream.h"

#ifdef LEPRA_POSIX
#include <sys/types.h>
#include <dirent.h>
#endif // LEPRA_POSIX



namespace Lepra
{



class FileArchive;



class DiskFile: public File, protected InputStream, protected OutputStream
{
public:
	// Not really an enum, but a bit field.
	enum OpenMode
	{
		MODE_TEXT		= 0x01,
		MODE_READ		= 0x02,
		MODE_TEXT_READ		= (MODE_READ|MODE_TEXT),
		MODE_WRITE		= 0x04,
		MODE_TEXT_WRITE		= (MODE_WRITE|MODE_TEXT),
		MODE_TEXT_READ_WRITE	= (MODE_WRITE|MODE_READ|MODE_TEXT),
		MODE_WRITE_APPEND	= 0x08,
		MODE_TEXT_WRITE_APPEND	= (MODE_WRITE_APPEND|MODE_TEXT),
	};

	class FindData
	{
	public:
		friend class DiskFile;

		FindData() :
#if defined LEPRA_WINDOWS
			mFindHandle(-1),
#elif defined LEPRA_POSIX
			mFileSpec(),
#endif
			mName(),
			mSubDir(false),
			mSize(0),
			mTime(-1)
		{
		}

		~FindData()
		{
#ifdef LEPRA_POSIX
			if (mDIR != 0)
			{
				::closedir(mDIR);
				mDIR = 0;
			}
#endif
		}

		inline String& GetName()
		{
			return mName;
		}

		inline const String& GetName() const
		{
			return mName;
		}

		inline uint64 GetSize()
		{
			return mSize;
		}

		inline int64 GetTimeTag()
		{
			return mTime;
		}

		inline bool IsSubDir()
		{
			return mSubDir;
		}

		inline void Clear()
		{
			mSize = 0;
			mName = _T("");
			mSubDir = false;
			mTime = -1;
		}
		
	private:
#if defined LEPRA_WINDOWS
		intptr_t mFindHandle;
#elif defined LEPRA_POSIX
		DIR* mDIR;	// Used with FindFirst() and FindNext().
		String mFileSpec;
#else
#error Lepra::DiskFile::FindData is not properly implemented on this platform!
#endif // LEPRA_POSIX

		String mName;
		bool   mSubDir;
		uint64 mSize;
		int64 mTime;
	};

	DiskFile();
	DiskFile(Reader* pReader);
	DiskFile(Writer* pWriter);
	DiskFile(Reader* pReader, Writer* pWriter);
	virtual ~DiskFile();

	// If pCreatePath is set to true, the directory structure given by
	// pFileName will be created if it doesn't already exist, and if
	// pMode is set to WRITE_ONLY or WRITE_APPEND.
	bool Open(const String& pFileName, OpenMode pMode, bool pCreatePath = false, Endian::EndianType pEndian = Endian::TYPE_LITTLE_ENDIAN);

	void Close();

	bool IsOpen() const;

	// Use this to change the endian in the middle of a file read/write.
	void SetEndian(Endian::EndianType pEndian);
	Endian::EndianType GetEndian();

	String GetFullName() const;	// Returns path+filename.
	String GetName() const;		// Returns filename only.
	String GetPath() const;		// Returns path.

	virtual int64 GetSize() const;

	// Overrided from Reader/Writer.
	IOError ReadData(void* pBuffer, size_t pSize);
	IOError WriteData(const void* pBuffer, size_t pSize);

	virtual void Flush();

	virtual int64 Tell() const;
	virtual int64 Seek(int64 pOffset, FileOrigin pFrom);

	// Static functions.

	static bool Exists(const String& pFileName);
	static bool PathExists(const String& pPathName);
	static bool Delete(const String& pFileName);
	static bool Rename(const String& pOldFileName, const String& pNewFileName);
	static bool CreateDir(const String& pPathName);
	static bool RemoveDir(const String& pPathName);

	static bool FindFirst(const String& pFileSpec, FindData& pFindData);
	static bool FindNext(FindData& pFindData);

	// Generates a unique random file name for the given path. The filename can then
	// be used to create a temp-file.
	static String GenerateUniqueFileName(const String& pPath);

	// Overrided from InputStream.
	int64 GetAvailable() const;
	virtual IOError ReadRaw(void* pBuffer, size_t pSize);
	IOError Skip(size_t pSize);

	// Overrided from OutputStream.
	virtual IOError WriteRaw(const void* pBuffer, size_t pSize);
private:
	void ExtractPathAndFileName(const String& pFileName);

	bool CreateSubDirs();

	static FILE* FileOpen(const String& pFileName, const String& pMode);

	//void CopyArchiveFiles(FileArchive& pSource, FileArchive& pDest, const String& pExceptThisFile);
	//bool CopyFileBetweenArchives(FileArchive& pSource, FileArchive& pDest, const String& pFileName);

	void operator=(const DiskFile&);

	Endian::EndianType mFileEndian;

	FILE* mFile;
	String mFileName;
	String mPath;

	int64 mFileSize;

	// Usually just NULL. Can be set by the user to redirect the IO
	// through another reader/writer.
	Writer* mWriter;
	Reader* mReader;

	OpenMode mMode;
};



}
