
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/DiskFile.h"
#include "../Include/FileArchive.h"
#include "../Include/Path.h"
#include "../Include/Random.h"
#include "../Include/String.h"

#include <string.h>
#include <errno.h>

#if defined LEPRA_WINDOWS
#include <direct.h>
#include <io.h>
#elif defined LEPRA_POSIX
#include <sys/stat.h>
#include <unistd.h>
#endif // Windows / Posix



namespace Lepra
{



DiskFile::DiskFile() :
	File(Endian::TYPE_BIG_ENDIAN, Endian::TYPE_BIG_ENDIAN, 0, 0),
	mFile(0),
	mFileName(""),
	mPath(""),
	mFileSize(0),
	mWriter(0),
	mReader(0),
	mMode(MODE_READ)
{
	Reader::SetInputStream(this);
	Writer::SetOutputStream(this);
}

DiskFile::DiskFile(Reader* pReader) :
	File(Endian::TYPE_BIG_ENDIAN, Endian::TYPE_BIG_ENDIAN, 0, 0),
	mFile(0),
	mFileName(""),
	mPath(""),
	mFileSize(0),
	mWriter(0),
	mReader(pReader),
	mMode(MODE_READ)
{
	Reader::SetInputStream(this);
	Writer::SetOutputStream(this);

	if (mReader != 0)
	{
		mReader->SetInputStream(this);
	}
}

DiskFile::DiskFile(Writer* pWriter) :
	File(Endian::TYPE_BIG_ENDIAN, Endian::TYPE_BIG_ENDIAN, 0, 0),
	mFile(0),
	mFileName(""),
	mPath(""),
	mFileSize(0),
	mWriter(pWriter),
	mReader(0),
	mMode(MODE_READ)
{
	Reader::SetInputStream(this);
	Writer::SetOutputStream(this);

	if (mWriter != 0)
	{
		mWriter->SetOutputStream(this);
	}
}

DiskFile::DiskFile(Reader* pReader, Writer* pWriter) :
	File(Endian::TYPE_BIG_ENDIAN, Endian::TYPE_BIG_ENDIAN, 0, 0),
	mFile(0),
	mFileName(""),
	mPath(""),
	mFileSize(0),
	mWriter(pWriter),
	mReader(pReader),
	mMode(MODE_READ)
{
	Reader::SetInputStream(this);
	Writer::SetOutputStream(this);

	if (mReader != 0)
	{
		mReader->SetInputStream(this);
	}

	if (mWriter != 0)
	{
		mWriter->SetOutputStream(this);
	}
}

DiskFile::~DiskFile()
{
	Close();
}

void DiskFile::ExtractPathAndFileName(const str& pFileName)
{
	Path::SplitPath(pFileName, mPath, mFileName);
	InputStream::SetName(mFileName);
	OutputStream::SetName(mFileName);
}

FILE* DiskFile::FileOpen(const str& pFileName, const str& pMode)
{
	FILE* lFile = 0;

#if _MSC_VER > 1310	// MS compiler version 13 1.0 = 2003 .NET.
#ifdef LEPRA_UNICODE
	if (::_wfopen_s(&lFile, pFileName.c_str(), pMode.c_str()) != 0)
#else // ANSI
	if (::fopen_s(&lFile, pFileName.c_str(), pMode.c_str()) != 0)
#endif // Unicode / ANSI.
	{
		lFile = 0;
	}
#else // _MSC_VER <= 1310
	lFile = fopen(pFileName.c_str(), pMode.c_str());
#endif // _MSC_VER > 1310 / _MSC_VER <= 1310

	return (lFile);
}

bool DiskFile::Open(const str& pFileName, OpenMode pMode, bool pCreatePath, Endian::EndianType pEndian)
{
	Close();

	SetEndian(pEndian);

	bool lOk = true;

	Parent::ClearMode(Parent::READ_MODE);
	Parent::ClearMode(Parent::WRITE_MODE);
	if (pMode&MODE_READ)
	{
		Parent::SetMode(Parent::READ_MODE);
	}
	if (pMode&(MODE_WRITE|MODE_WRITE_APPEND))
	{
		Parent::SetMode(Parent::WRITE_MODE);
	}
	if (!(pMode&(MODE_WRITE|MODE_WRITE_APPEND|MODE_READ)))
	{
		lOk = false;
	}

	if (lOk)
	{
		ExtractPathAndFileName(pFileName);
		if(pMode&(MODE_WRITE|MODE_WRITE_APPEND))
		{
			if (pCreatePath == true && CreateSubDirs() == false)
			{
				lOk = false;
			}
		}
	}

	str lModeString;
	if (lOk)
	{
		if ((pMode&(MODE_READ|MODE_WRITE|MODE_WRITE_APPEND)) == MODE_READ)
		{
			lModeString = "r";
		}
		else if ((pMode&(MODE_READ|MODE_WRITE|MODE_WRITE_APPEND)) == MODE_WRITE)
		{
			lModeString = "w";
		}
		else if ((pMode&(MODE_READ|MODE_WRITE_APPEND)) == MODE_WRITE_APPEND)
		{
			lModeString = "a";
		}
		else if ((pMode&(MODE_READ|MODE_WRITE|MODE_WRITE_APPEND)) == (MODE_READ|MODE_WRITE))
		{
			lModeString = "r+";
		}
		else if ((pMode&(MODE_READ|MODE_WRITE_APPEND)) == (MODE_READ|MODE_WRITE_APPEND))
		{
			lModeString = "a+";
		}
		else
		{
			lOk = false;
		}
	}
	if (lOk)
	{

		if (pMode&MODE_TEXT)
		{
			lModeString += "t";
		}
		else
		{
			lModeString += "b";
		}
		mFile = FileOpen(pFileName, lModeString);
	}

	if (lOk)
	{
		mMode = pMode;

		if (mFile != 0)
		{
			// Get the file size.
			long lPosition = ::ftell(mFile);
			::fseek(mFile, 0, SEEK_END);
			mFileSize = (int64)::ftell(mFile);
			if (!(pMode&MODE_WRITE_APPEND))
			{
				::fseek(mFile, lPosition, SEEK_SET);
			}
		}
		else
		{
			lOk = false;
		}
	}

	return lOk;
}

void DiskFile::Close()
{
	if (mFile != 0)
	{
		fclose(mFile);
		mFile = 0;
	}

	mFileName = "";
	mPath = "";
}

void DiskFile::SetEndian(Endian::EndianType pEndian)
{
	Parent::SetEndian(pEndian);
	if (mReader)
	{
		mReader->SetReaderEndian(pEndian);
	}
	if (mWriter)
	{
		mWriter->SetWriterEndian(pEndian);
	}
}

str DiskFile::GetFullName() const
{
	return mPath + "/" + mFileName;
}

str DiskFile::GetName() const
{
	return mFileName;
}

str DiskFile::GetPath() const
{
	return mPath;
}

IOError DiskFile::ReadRaw(void* pBuffer, size_t pSize)
{
	IOError lError = IO_OK;

	if (mFile == 0)
	{
		lError = IO_STREAM_NOT_OPEN;
	}

	if (lError == IO_OK && !(mMode&MODE_READ))
	{
		lError = IO_INVALID_OPERATION;
	}

	if (lError == IO_OK)
	{
		if (::fread(pBuffer, (size_t)pSize, 1, mFile) != 1)
		{
			lError = IO_ERROR_READING_FROM_STREAM;
		}
	}

	return lError;
}

IOError DiskFile::Skip(size_t pSize)
{
	return (Parent::Skip(pSize));
}

IOError DiskFile::WriteRaw(const void* pBuffer, size_t pSize)
{
	IOError lError = IO_OK;

	if (mFile == 0)
	{
		lError = IO_STREAM_NOT_OPEN;
	}

	if (lError == IO_OK && !(mMode&(MODE_WRITE|MODE_WRITE_APPEND)))
	{
		lError = IO_INVALID_OPERATION;
	}

	if (lError == IO_OK)
	{
		if (::fwrite(pBuffer, (size_t)pSize, 1, mFile) != 1)
		{
			lError = IO_ERROR_WRITING_TO_STREAM;
		}
	}

	return lError;
}

IOError DiskFile::ReadData(void* pBuffer, size_t pSize)
{
	IOError lError;
	if (mReader != 0)
	{
		lError = mReader->ReadData(pBuffer, pSize);
	}
	else
	{
		lError = Reader::ReadData(pBuffer, pSize);
	}

	return lError;
}

IOError DiskFile::WriteData(const void* pBuffer, size_t pSize)
{
	IOError lError;
	if (mWriter != 0)
	{
		lError = mWriter->WriteData(pBuffer, pSize);
	}
	else
	{
		lError = Writer::WriteData(pBuffer, pSize);
	}
	return lError;
}

int64 DiskFile::Tell() const
{
#ifdef LEPRA_MSVC
	return (int64)::_ftelli64(mFile);
#elif defined(LEPRA_MAC)
	return (int64)::ftell(mFile);
#else
	return (int64)::ftello64(mFile);
#endif
}

int64 DiskFile::Seek(int64 pOffset, FileOrigin pFrom)
{
	if (!IsOpen())
	{
		return (-1);
	}

	int lOrigin;
	
	switch (pFrom)
	{
		case FSEEK_CUR:
		{
			lOrigin = SEEK_CUR;
			break;
		}
		case FSEEK_END:
		{
			lOrigin = SEEK_END;
			break;
		}
		case FSEEK_SET:
		default:
		{
			lOrigin = SEEK_SET;
			break;
		}
	}

	int64 lPos = -1;

#if defined(LEPRA_MSVC)
	if (::_fseeki64(mFile, pOffset, lOrigin) == 0)
#elif defined(LEPRA_MAC)
	if (::fseek(mFile, pOffset, lOrigin) == 0)
#else
	if (::fseeko64(mFile, pOffset, lOrigin) == 0)
#endif
	{
		lPos = Tell();
	}

	return lPos;
}



//
// Static functions.
//

IOError DiskFile::Load(const str& pFilename, void** pData, int64& pDataSize)
{
	DiskFile lFile;
	bool lOk = false;
	for (int x = 0; !lOk && x < 3; ++x)
	{
		lOk = lFile.Open(pFilename, DiskFile::MODE_READ);
	}
	if (lOk)
	{
		pDataSize = lFile.GetSize();
		return lFile.AllocReadData(pData, (size_t)pDataSize);
	}
	return IO_FILE_NOT_FOUND;
}

bool DiskFile::Exists(const str& pFileName)
{
	FILE* lFile = FileOpen(pFileName, "rb");

	bool lOk = false;
	if (lFile != 0)
	{
		::fclose(lFile);
		lOk = true;
	}

	return lOk;
}

bool DiskFile::PathExists(const str& pPathName)
{
	char lCurrentDir[300];

#ifdef LEPRA_WINDOWS // Hugge/TRICKY: Should we check for Visual Studio instead?
	::_getcwd(lCurrentDir, 299);
	bool lSuccess = _chdir(pPathName.c_str()) == 0;
	::_chdir(lCurrentDir);
#else
	::getcwd(lCurrentDir, 299);
	bool lSuccess = ::chdir(pPathName.c_str()) == 0;
	::chdir(lCurrentDir);
#endif

	return lSuccess;
}

bool DiskFile::Delete(const str& pFileName)
{
	return (::remove(pFileName.c_str()) == 0);
}

bool DiskFile::Rename(const str& pOldFileName, const str& pNewFileName)
{
	return (::rename(pOldFileName.c_str(), pNewFileName.c_str()) == 0);
}

bool DiskFile::CreateDir(const str& pPathName)
{
#ifdef LEPRA_POSIX 
	return ::mkdir(pPathName.c_str(), 0775) != -1;
#else
	return ::_mkdir(pPathName.c_str()) != -1;
#endif
}

bool DiskFile::RemoveDir(const str& pPathName)
{
#ifdef LEPRA_WINDOWS // Hugge/TRICKY: Should we check for Visual Studio instead?
	return ::_rmdir(pPathName.c_str()) == 0;
#else
	return ::rmdir(pPathName.c_str()) == 0;
#endif
}

bool DiskFile::CreateSubDirs()
{
	str lDirectory = Path::GetDirectory(mPath);
	strutil::strvec lDirectoryArray = Path::SplitNodes(lDirectory);

	bool lOk = true;
	str lNewPath;
	for (size_t x = 0; x < lDirectoryArray.size(); ++x)
	{
		lNewPath += lDirectoryArray[x];
		if (CreateDir(lNewPath) != 0)
		{
			// errno is a global variable defined in errno.h
			if (errno == ENOENT)
			{
				lOk = false;
				break;
			}
		}
		lNewPath += "/";
	}

	return lOk;
}

bool DiskFile::FindFirst(const str& pFileSpec, FindData& pFindData)
{
	pFindData.Clear();
	bool lOk = true;

	pFindData.mFileSpec = pFileSpec;
#if defined LEPRA_WINDOWS
	_finddata_t lData;
	pFindData.mFindHandle = _findfirst(pFileSpec.c_str(), &lData);
	if (pFindData.mFindHandle == -1)
	{
		lOk = false;
	}
	if (lOk == true)
	{
		str lPath = Path::SplitPath(pFileSpec)[0];
		pFindData.mName = Path::JoinPath(lPath, str(lData.name));	// TODO: needs real Unicode findxxx().
		pFindData.mSize = lData.size;

		if ((lData.attrib & _A_SUBDIR) != 0)
		{
			pFindData.mSubDir = true;
		}

		pFindData.mTime  = lData.time_write;
	}
#elif defined LEPRA_POSIX
	pFindData.mGlobList.gl_offs = 0;
	::glob(pFileSpec.c_str(), GLOB_DOOFFS|GLOB_MARK, 0, &pFindData.mGlobList);
	if (pFindData.mGlobList.gl_pathc >= 1)
	{
		pFindData.mName = pFindData.mGlobList.gl_pathv[0];
		struct stat lFileInfo;
		::stat(pFindData.mGlobList.gl_pathv[1], &lFileInfo);	// TODO: error check.
		pFindData.mSize = lFileInfo.st_size;
		pFindData.mSubDir = (S_ISDIR(lFileInfo.st_mode) != 0);
		pFindData.mTime = lFileInfo.st_mtime;
		pFindData.mGlobIndex = 2;
	}
	else
	{
		lOk = false;
		pFindData.mGlobIndex = 0xffff;
	}
#else
#error DiskFile::FindFirst() not implemented on this platform!
#endif

	return lOk;
}

bool DiskFile::FindNext(FindData& pFindData)
{
	bool lOk = true;

#ifdef LEPRA_WINDOWS
	_finddata_t lData;
	if (_findnext(pFindData.mFindHandle, &lData) != 0)
	{
		lOk = false;
	}
	if (lOk == true)
	{
		str lPath = Path::SplitPath(pFindData.mFileSpec)[0];
		pFindData.mName = Path::JoinPath(lPath, str(lData.name));	// TODO: needs real Unicode findxxx()!
		pFindData.mSize = lData.size;
		pFindData.mSubDir = ((lData.attrib & _A_SUBDIR) != 0);
		pFindData.mTime  = lData.time_write;
	}
#elif defined LEPRA_POSIX
	lOk = false;
	if (pFindData.mGlobIndex < pFindData.mGlobList.gl_pathc)
	{
		lOk = true;
		pFindData.mName = pFindData.mGlobList.gl_pathv[pFindData.mGlobIndex];
		struct stat lFileInfo;
		::stat(pFindData.mGlobList.gl_pathv[pFindData.mGlobIndex], &lFileInfo);	// TODO: error check.
		pFindData.mSize = lFileInfo.st_size;
		pFindData.mSubDir = (S_ISDIR(lFileInfo.st_mode) != 0);
		pFindData.mTime = lFileInfo.st_mtime;
		++pFindData.mGlobIndex;
	}
	else if (pFindData.mGlobIndex != 0xffff)
	{
		::globfree(&pFindData.mGlobList);
		pFindData.mGlobIndex = 0xffff;
	}
#else
#error DiskFile::FindNext() not implemented on this platform!
#endif

	if (!lOk)
	{
		pFindData.Clear();
	}

	return lOk;
}

str DiskFile::GenerateUniqueFileName(const str& pPath)
{
	str lPath(pPath);
	lPath += '/';

	int64 lRandomNumber = (int64)Random::GetRandomNumber64();
	if (lRandomNumber < 0)
		lRandomNumber = -lRandomNumber;

	str lName = strutil::IntToString(lRandomNumber, 16);

	while (Exists(pPath + lName) == true)
	{
		lRandomNumber = (int64)Random::GetRandomNumber64();
		if (lRandomNumber < 0)
		{
			lRandomNumber = -lRandomNumber;
		}
		lName = strutil::IntToString(lRandomNumber, 16);
	}

	return pPath + lName;
}

void DiskFile::operator=(const DiskFile&)
{
}

bool DiskFile::IsOpen() const
{
	return (mFile != 0);
}

int64 DiskFile::GetSize() const
{
	return mFileSize;
}

int64 DiskFile::GetAvailable() const
{
	return (mFileSize - Tell());
}

void DiskFile::Flush()
{
	::fflush(mFile);
}



}
