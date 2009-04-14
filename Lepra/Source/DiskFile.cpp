/*
	Class:  DiskFile
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games
*/

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

#include <unistd.h>
#include <sys/stat.h>

#endif

namespace Lepra
{

DiskFile::DiskFile() :
	File(Endian::TYPE_LITTLE_ENDIAN, Endian::TYPE_LITTLE_ENDIAN, 0, 0),
	mFileEndian(Endian::TYPE_LITTLE_ENDIAN),
	mFile(0),
	mFileName(_T("")),
	mPath(_T("")),
	mFileSize(0),
	mWriter(0),
	mReader(0),
	mMode(MODE_READ_ONLY)
{
	Reader::SetInputStream(this);
	Writer::SetOutputStream(this);
}

DiskFile::DiskFile(Reader* pReader) :
	File(Endian::TYPE_LITTLE_ENDIAN, Endian::TYPE_LITTLE_ENDIAN, 0, 0),
	mFileEndian(Endian::TYPE_LITTLE_ENDIAN),
	mFile(0),
	mFileName(_T("")),
	mPath(_T("")),
	mFileSize(0),
	mWriter(0),
	mReader(pReader),
	mMode(MODE_READ_ONLY)
{
	Reader::SetInputStream(this);
	Writer::SetOutputStream(this);

	if (mReader != 0)
	{
		mReader->SetInputStream(this);
	}
}

DiskFile::DiskFile(Writer* pWriter) :
	File(Endian::TYPE_LITTLE_ENDIAN, Endian::TYPE_LITTLE_ENDIAN, 0, 0),
	mFileEndian(Endian::TYPE_LITTLE_ENDIAN),
	mFile(0),
	mFileName(_T("")),
	mPath(_T("")),
	mFileSize(0),
	mWriter(pWriter),
	mReader(0),
	mMode(MODE_READ_ONLY)
{
	Reader::SetInputStream(this);
	Writer::SetOutputStream(this);

	if (mWriter != 0)
	{
		mWriter->SetOutputStream(this);
	}
}

DiskFile::DiskFile(Reader* pReader, Writer* pWriter) :
	File(Endian::TYPE_LITTLE_ENDIAN, Endian::TYPE_LITTLE_ENDIAN, 0, 0),
	mFileEndian(Endian::TYPE_LITTLE_ENDIAN),
	mFile(0),
	mFileName(_T("")),
	mPath(_T("")),
	mFileSize(0),
	mWriter(pWriter),
	mReader(pReader),
	mMode(MODE_READ_ONLY)
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
	if (::_wfopen_s(&lFile, UnicodeStringUtility::ToOwnCode(pFileName).c_str(), UnicodeStringUtility::ToOwnCode(pMode+_T(", ccs=UTF-16LE")).c_str()) != 0)
#else
	if (::fopen_s(&lFile, AnsiStringUtility::ToOwnCode(pFileName).c_str(), AnsiStringUtility::ToOwnCode(pMode).c_str()) != 0)
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

	mFileEndian = pEndian;

	bool lOk = true;

	if (pMode&MODE_READ_ONLY)
	{
		File::SetMode(File::READ_MODE);
		File::ClearMode(File::WRITE_MODE);
	}
	else if(pMode&(MODE_WRITE_ONLY|MODE_WRITE_APPEND))
	{
		File::SetMode(File::WRITE_MODE);
		File::ClearMode(File::READ_MODE);
	}
	else
	{
		lOk = false;
	}

	if (lOk)
	{
		ExtractPathAndFileName(pFileName);

		if (pMode&MODE_READ_ONLY)
		{
			if (pMode&MODE_TEXT)
			{
				mFile = FileOpen(pFileName, _T("rt"));
			}
			else
			{
				mFile = FileOpen(pFileName, _T("rb"));
			}
		}
		else if (pMode&MODE_WRITE_ONLY)
		{
			if (pCreatePath == true && CreateSubDirs() == false)
			{
				lOk = false;
			}
			else
			{
				if (pMode&MODE_TEXT)
				{
					mFile = FileOpen(pFileName, _T("wt"));
				}
				else
				{
					mFile = FileOpen(pFileName, _T("wb"));
				}
			}
		}
		else if (pMode&MODE_WRITE_APPEND)
		{
			if (pCreatePath == true && CreateSubDirs() == false)
			{
				lOk = false;
			}
			else
			{
				if (pMode&MODE_TEXT)
				{
					mFile = FileOpen(pFileName, _T("a+t"));
				}
				else
				{
					mFile = FileOpen(pFileName, _T("a+b"));
				}
			}
		}
	}

	if (lOk == true)
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
	File::SetEndian(pEndian);
	mFileEndian = pEndian;

	if (mReader != 0)
	{
		mReader->SetReaderEndian(mFileEndian);
	}

	if (mWriter != 0)
	{
		mWriter->SetWriterEndian(mFileEndian);
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

IOError DiskFile::ReadRaw(void* pBuffer, unsigned pSize)
{
	IOError lError = IO_OK;

	if (mFile == 0)
	{
		lError = IO_STREAM_NOT_OPEN;
	}

	if (lError == IO_OK && !(mMode&MODE_READ_ONLY))
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

IOError DiskFile::Skip(unsigned pSize)
{
	return (File::Skip(pSize));
}

IOError DiskFile::WriteRaw(const void* pBuffer, unsigned pSize)
{
	IOError lError = IO_OK;

	if (mFile == 0)
	{
		lError = IO_STREAM_NOT_OPEN;
	}

	if (lError == IO_OK && !(mMode&(MODE_WRITE_ONLY|MODE_WRITE_APPEND)))
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

IOError DiskFile::ReadData(void* pBuffer, unsigned pSize)
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

IOError DiskFile::WriteData(const void* pBuffer, unsigned pSize)
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
	bool lSuccess = ::chdir(AnsiStringUtility::ToOwnCode(pPathName)) == 0;
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

	String lPath;
	File::ExtractPathAndFileName(pFileSpec, pFindData.mFileSpec, lPath);

	if (pFindData.mDIR != 0)
	{
		::closedir(pFindData.mDIR);
		pFindData.mDIR = 0;
	}

	pFindData.mDIR = ::opendir(AnsiStringUtility::ToOwnCode(lPath));

	if (pFindData.mDIR == 0)
	{
		lOk = false;
	}

	dirent* lDirEntry = 0;

	if (lOk == true)
	{
		lDirEntry = readdir(pFindData.mDIR);
		lOk = (lDirEntry != 0);
	}

	if (lOk == true)
	{
		pFindData.mName = AnsiStringUtility::ToCurrentCode(AnsiString(lDirEntry->d_name));	// TODO: needs real Unicode findxxx().

		DIR* lDIR = ::opendir(lDirEntry->d_name);
		if (lDIR != 0)
		{
			::closedir(lDIR);
			pFindData.mSubDir = true;
		}
	}

	// Check if this file matches the file specification.
	if (File::CompareFileName(pFindData.mName, pFindData.mFileSpec) == false)
	{
		lOk = FindNext(pFindData);
	}
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
	if (pFindData.mDIR == 0)
	{
		lOk = false;
	}

	dirent* lDirEntry = 0;

	do
	{
		if (lOk == true)
		{
			lDirEntry = readdir(pFindData.mDIR);
			lOk = (lDirEntry != 0);
		}
	
		if (lOk == true)
		{
			pFindData.mName = AnsiStringUtility::ToCurrentCode(AnsiString(lDirEntry->d_name));	// TODO: needs real Unicode findxxx().
	
			DIR* lDIR = ::opendir(lDirEntry->d_name);
			if (lDIR != 0)
			{
				::closedir(lDIR);
				pFindData.mSubDir = true;
			}
		}
	}while(lOk == true && File::CompareFileName(pFindData.mName, pFindData.mFileSpec) == false);
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

Endian::EndianType DiskFile::GetEndian()
{
	return mFileEndian;
}

} // End namespace.
