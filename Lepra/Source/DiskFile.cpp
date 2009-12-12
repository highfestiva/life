
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



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
#include <glob.h>
#include <unistd.h>
#endif // Windows / Posix



namespace Lepra
{



DiskFile::DiskFile() :
	File(Endian::TYPE_BIG_ENDIAN, Endian::TYPE_BIG_ENDIAN, 0, 0),
	mFile(0),
	mFileName(_T("")),
	mPath(_T("")),
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
	mFileName(_T("")),
	mPath(_T("")),
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
	mFileName(_T("")),
	mPath(_T("")),
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
	mFileName(_T("")),
	mPath(_T("")),
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

void DiskFile::ExtractPathAndFileName(const String& pFileName)
{
	Path::SplitPath(pFileName, mPath, mFileName);
	InputStream::SetName(mFileName);
	OutputStream::SetName(mFileName);
}

FILE* DiskFile::FileOpen(const String& pFileName, const String& pMode)
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
	lFile = fopen(AnsiStringUtility::ToOwnCode(pFileName).c_str(), AnsiStringUtility::ToOwnCode(pMode).c_str());
#endif // _MSC_VER > 1310 / _MSC_VER <= 1310

	return (lFile);
}

bool DiskFile::Open(const String& pFileName, OpenMode pMode, bool pCreatePath, Endian::EndianType pEndian)
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

	Lepra::String lModeString;
	if (lOk)
	{
		if ((pMode&(MODE_READ|MODE_WRITE|MODE_WRITE_APPEND)) == MODE_READ)
		{
			lModeString = _T("r");
		}
		else if ((pMode&(MODE_READ|MODE_WRITE|MODE_WRITE_APPEND)) == MODE_WRITE)
		{
			lModeString = _T("w");
		}
		else if ((pMode&(MODE_READ|MODE_WRITE_APPEND)) == MODE_WRITE_APPEND)
		{
			lModeString = _T("a");
		}
		else if ((pMode&(MODE_READ|MODE_WRITE|MODE_WRITE_APPEND)) == (MODE_READ|MODE_WRITE))
		{
			lModeString = _T("r+");
		}
		else if ((pMode&(MODE_READ|MODE_WRITE_APPEND)) == (MODE_READ|MODE_WRITE_APPEND))
		{
			lModeString = _T("a+");
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
			lModeString += _T("t");
		}
		else
		{
			lModeString += _T("b");
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

	mFileName = _T("");
	mPath = _T("");
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

String DiskFile::GetFullName() const
{
	return mPath + _T("/") + mFileName;
}

String DiskFile::GetName() const
{
	return mFileName;
}

String DiskFile::GetPath() const
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

#ifdef LEPRA_MSVC
	if (::_fseeki64(mFile, pOffset, lOrigin) == 0)
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

bool DiskFile::Exists(const String& pFileName)
{
	FILE* lFile = FileOpen(pFileName, _T("rb"));

	bool lOk = false;
	if (lFile != 0)
	{
		::fclose(lFile);
		lOk = true;
	}

	return lOk;
}

bool DiskFile::PathExists(const String& pPathName)
{
	char lCurrentDir[300];

#ifdef LEPRA_WINDOWS // Hugge/TRICKY: Should we check for Visual Studio instead?
	::_getcwd(lCurrentDir, 299);
	bool lSuccess = _chdir(AnsiStringUtility::ToOwnCode(pPathName).c_str()) == 0;
	::_chdir(lCurrentDir);
#else
	::getcwd(lCurrentDir, 299);
	bool lSuccess = ::chdir(AnsiStringUtility::ToOwnCode(pPathName).c_str()) == 0;
	::chdir(lCurrentDir);
#endif

	return lSuccess;
}

bool DiskFile::Delete(const String& pFileName)
{
	return (::remove(AnsiStringUtility::ToOwnCode(pFileName).c_str()) == 0);
}

bool DiskFile::Rename(const String& pOldFileName, const String& pNewFileName)
{
	return (::rename(AnsiStringUtility::ToOwnCode(pOldFileName).c_str(), AnsiStringUtility::ToOwnCode(pNewFileName).c_str()) == 0);
}

bool DiskFile::CreateDir(const String& pPathName)
{
#ifdef LEPRA_POSIX 
	return ::mkdir(AnsiStringUtility::ToOwnCode(pPathName).c_str(), 0775) != -1;
#else
	return ::_mkdir(AnsiStringUtility::ToOwnCode(pPathName).c_str()) != -1;
#endif
}

bool DiskFile::RemoveDir(const String& pPathName)
{
#ifdef LEPRA_WINDOWS // Hugge/TRICKY: Should we check for Visual Studio instead?
	return ::_rmdir(AnsiStringUtility::ToOwnCode(pPathName).c_str()) == 0;
#else
	return ::rmdir(AnsiStringUtility::ToOwnCode(pPathName).c_str()) == 0;
#endif
}

bool DiskFile::CreateSubDirs()
{
	String lDirectory = Path::GetDirectory(mPath);
	StringUtility::StringVector lDirectoryArray = Path::SplitNodes(lDirectory);

	bool lOk = true;
	String lNewPath;
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
		lNewPath += _T("/");
	}

	return lOk;
}

bool DiskFile::FindFirst(const String& pFileSpec, FindData& pFindData)
{
	pFindData.Clear();
	bool lOk = true;

#if defined LEPRA_WINDOWS
	_finddata_t lData;
	pFindData.mFindHandle = _findfirst(AnsiStringUtility::ToOwnCode(pFileSpec).c_str(), &lData);

	if (pFindData.mFindHandle == -1)
	{
		lOk = false;
	}

	if (lOk == true)
	{
		pFindData.mName = AnsiStringUtility::ToCurrentCode(AnsiString(lData.name));	// TODO: needs real Unicode findxxx().
		pFindData.mSize = lData.size;

		if ((lData.attrib & _A_SUBDIR) != 0)
		{
			pFindData.mSubDir = true;
		}

		pFindData.mTime  = lData.time_write;
	}
#elif defined LEPRA_POSIX
	glob_t lGlobList;
	lGlobList.gl_offs = 1;
	::glob(AnsiStringUtility::ToOwnCode(pFileSpec).c_str(), GLOB_DOOFFS|GLOB_MARK, 0, &lGlobList);
	if (lGlobList.gl_pathc >= 1)
	{
		pFindData.mFileSpec = pFileSpec;
		pFindData.mName = AnsiStringUtility::ToCurrentCode(lGlobList.gl_pathv[0]);
		struct stat lFileInfo;
		::stat(lGlobList.gl_pathv[0], &lFileInfo);	// TODO: error check.
		pFindData.mSize = lFileInfo.st_size;
		pFindData.mSubDir = (S_ISDIR(lFileInfo.st_mode) != 0);
		pFindData.mTime = lFileInfo.st_mtime;
	}
	else
	{
		lOk = false;
	}
	::globfree(&lGlobList);
#else
#error DiskFile::FindFirst() not implemented on this platform!
#endif

	return lOk;
}

bool DiskFile::FindNext(FindData& pFindData)
{
	pFindData.Clear();
	bool lOk = true;

#ifdef LEPRA_WINDOWS
	_finddata_t lData;
	if (_findnext(pFindData.mFindHandle, &lData) != 0)
	{
		lOk = false;
	}
	if (lOk == true)
	{
		pFindData.mName = AnsiStringUtility::ToCurrentCode(AnsiString(lData.name));	// TODO: needs real Unicode findxxx()!
		pFindData.mSize = lData.size;

		if ((lData.attrib & _A_SUBDIR) != 0)
		{
			pFindData.mSubDir = true;
		}

		pFindData.mTime  = lData.time_write;
	}
#elif defined LEPRA_POSIX
	lOk = false;
	glob_t lGlobList;
	lGlobList.gl_offs = 1000;
	::glob(AnsiStringUtility::ToOwnCode(pFindData.mFileSpec).c_str(), GLOB_DOOFFS|GLOB_MARK, 0, &lGlobList);
	if (lGlobList.gl_pathc >= 1)
	{
		for (size_t x = 0; x < lGlobList.gl_pathc; ++x)
		{
			if (AnsiStringUtility::ToCurrentCode(lGlobList.gl_pathv[x]) == pFindData.mName)
			{
				++x;
			  	if (x < lGlobList.gl_pathc)
				{
					lOk = true;
					pFindData.mName = AnsiStringUtility::ToCurrentCode(lGlobList.gl_pathv[0]);
					struct stat lFileInfo;
					::stat(lGlobList.gl_pathv[0], &lFileInfo);	// TODO: error check.
					pFindData.mSize = lFileInfo.st_size;
					pFindData.mSubDir = (S_ISDIR(lFileInfo.st_mode) != 0);
					pFindData.mTime = lFileInfo.st_mtime;
					break;
				}
			}
		}
	}
	::globfree(&lGlobList);
#else
#error DiskFile::FindFirst() not implemented on this platform!
#endif

	return lOk;
}

String DiskFile::GenerateUniqueFileName(const String& pPath)
{
	String lPath(pPath);
	lPath += _T('/');

	int64 lRandomNumber = (int64)Random::GetRandomNumber64();
	if (lRandomNumber < 0)
		lRandomNumber = -lRandomNumber;

	String lName = StringUtility::IntToString(lRandomNumber, 16);

	while (Exists(pPath + lName) == true)
	{
		lRandomNumber = (int64)Random::GetRandomNumber64();
		if (lRandomNumber < 0)
		{
			lRandomNumber = -lRandomNumber;
		}
		lName = StringUtility::IntToString(lRandomNumber, 16);
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
