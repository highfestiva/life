
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

#include "pch.h"
#include "../Include/DiskFile.h"
#include "../Include/Log.h"
#include "../Include/Path.h"
#include "../Include/ZipArchive.h"
#include <stdio.h>

namespace Lepra
{

ZipArchive::ZipArchive() :
	mArchiveName(""),
	mIOType(READ_ONLY),
	mUnzipFile(0),
	mZipFile(0),
	mFileSize(0)
{
}

ZipArchive::~ZipArchive()
{
	CloseArchive();
}

IOError ZipArchive::OpenArchive(const str& pArchiveFileName, IOType pIOType)
{
	CloseArchive();

	mArchiveName = pArchiveFileName;

	IOError lIOError = IO_FILE_NOT_FOUND;
	bool lCheckZipFile = false;

	mIOType = pIOType;
	switch(mIOType)
	{
		case READ_ONLY:
		{
			mUnzipFile = ::unzOpen64(pArchiveFileName.c_str());
			if (mUnzipFile != 0)
			{
				lIOError = IO_OK;
			}
		}
		break;
		case WRITE_ONLY:
		{
			mZipFile = ::zipOpen(pArchiveFileName.c_str(), APPEND_STATUS_CREATE);
			lCheckZipFile = true;
		}
		break;
		case WRITE_APPEND:
		{
			if (DiskFile::Exists(pArchiveFileName) == true)
				mZipFile = ::zipOpen(pArchiveFileName.c_str(), APPEND_STATUS_ADDINZIP);
			lCheckZipFile = true;
		}
		break;
		case INSERT_ONLY:
		{
		}
		break;
	}

	if (lCheckZipFile == true)
	{
		if (mZipFile != 0)
		{
			lIOError = IO_OK;
		}
		else
		{
			lIOError = IO_FILE_CREATION_FAILED;
		}
	}

	return lIOError;
}

void ZipArchive::CloseArchive()
{
	switch(mIOType)
	{
		case READ_ONLY:
		{
			if (mUnzipFile != 0)
			{
				::unzClose(mUnzipFile);
				mUnzipFile = 0;
			}
		}
		break;
		case WRITE_ONLY:
		case WRITE_APPEND:
		{
			if (mZipFile != 0)
			{
				::zipClose(mZipFile, "");
				mZipFile = 0;
			}
		}
		break;
		case INSERT_ONLY:
		{
		}
		break;
	}
}

void ZipArchive::CloseAndRemoveArchive()
{
	CloseArchive();
#ifdef LEPRA_MSVC
	::_wremove(wstrutil::Encode(mArchiveName).c_str());
#else
#ifdef LEPRA_POSIX
	::remove(mArchiveName.c_str());
#else
#error "ZipArchive::CloseAndRemoveArchive() is not implemented on this platform!"
#endif
#endif
}

int ZipArchive::GetFileCount()
{
	int lCount = 0;

	if (mIOType == READ_ONLY && mUnzipFile != 0)
	{
		unz_global_info64 lUnzGlobalInfo;
		::unzGetGlobalInfo64(mUnzipFile, &lUnzGlobalInfo);
		lCount = (int)lUnzGlobalInfo.number_entry;
	}

	return lCount;
}

IOError ZipArchive::InsertArchive(const str& /*pArchiveFileName*/)
{
	return IO_OK;
}

str ZipArchive::FileFindFirst()
{
	str lFileName;

	if (mIOType == READ_ONLY && mUnzipFile != 0 && ::unzGoToFirstFile(mUnzipFile) == UNZ_OK)
	{
		unz_file_info64 lFileInfo;
		char lCStrFileName[1024];
		::unzGetCurrentFileInfo64(mUnzipFile, &lFileInfo, lCStrFileName, 1024, 0, 0, 0, 0);
		lFileName = str(lCStrFileName);
	}

	return lFileName;
}

str ZipArchive::FileFindNext()
{
	str lFileName;

	if (mIOType == READ_ONLY && mUnzipFile != 0 && ::unzGoToNextFile(mUnzipFile) == UNZ_OK)
	{
		unz_file_info64 lFileInfo;
		char lCStrFileName[1024];
		::unzGetCurrentFileInfo64(mUnzipFile, &lFileInfo, lCStrFileName, 1024, 0, 0, 0, 0);
		lFileName = str(lCStrFileName);
	}

	return lFileName;
}

bool ZipArchive::FileOpen(const str& pFileName)
{
	bool lOK = false;

	switch(mIOType)
	{
		case READ_ONLY:
		{
			if (mUnzipFile != 0)
			{
				if (::unzLocateFile(mUnzipFile, pFileName.c_str(), 0) == UNZ_OK)
				{
					lOK = (::unzOpenCurrentFile(mUnzipFile) == UNZ_OK);
					if (lOK)
					{
						mFileSize = FileSize();
					}
				}
			}
		}
		break;
		case WRITE_ONLY:
		case WRITE_APPEND:
		{
			if (mZipFile != 0)
			{
				lOK = (::zipOpenNewFileInZip64(mZipFile, pFileName.c_str(), 0, 0, 0, 0, 0, 0, Z_DEFLATED, Z_DEFAULT_COMPRESSION, 0) == ZIP_OK);

				str lTempName;
				str lDirectory;
				lDirectory = Path::GetDirectory(mArchiveName);

				if (lOK)
				{
					lTempName = DiskFile::GenerateUniqueFileName(lDirectory);
					lOK = mOutFile.Open(lTempName, DiskFile::MODE_WRITE, false, Endian::TYPE_LITTLE_ENDIAN);

					if (lOK == false)
					{
						::zipCloseFileInZip(mZipFile);
					}
				}
			}
		}
		break;
		case INSERT_ONLY:
		{
		}
		break;
	}

	return lOK;
}

void ZipArchive::FileClose()
{
	switch(mIOType)
	{
		case READ_ONLY:
		{
			if (mUnzipFile != 0)
			{
				::unzCloseCurrentFile(mUnzipFile);
				mFileSize = 0;
			}
		}
		break;
		case WRITE_ONLY:
		case WRITE_APPEND:
		{
			if (mZipFile != 0)
			{
				if (mOutFile.IsOpen() == true)
				{
					str lTempFile = mOutFile.GetFullName();
					int64 lSize = mOutFile.GetSize();
					mOutFile.Close();
					if (mOutFile.Open(lTempFile, DiskFile::MODE_READ, false, Endian::TYPE_LITTLE_ENDIAN) == false)
					{
						mLog.Error("Failed to add file to archive.");
					}
					else
					{
						// Copy temp file to the zip archive.
						uint8 lBuf[1024];
						int64 lSteps = lSize / 1024;
						int64 lRest = lSize % 1024;
						int64 i;

						for (i = 0; i < lSteps; i++)
						{
							mOutFile.ReadData(lBuf, 1024);
							::zipWriteInFileInZip(mZipFile, lBuf, 1024);
						}

						if (lRest > 0)
						{
							mOutFile.ReadData(lBuf, (unsigned long)lRest);
							::zipWriteInFileInZip(mZipFile, lBuf, (unsigned long)lRest);
						}

						mOutFile.Close();
						DiskFile::Delete(lTempFile);
					}

					::zipCloseFileInZip(mZipFile);
				}
			}
		}
		break;
		case INSERT_ONLY:
		{
		}
		break;
	}
}

bool ZipArchive::FileExist(const str& pFileName)
{
	bool lExist = false;

	if (mIOType == READ_ONLY && mUnzipFile != 0)
	{
		lExist = (::unzLocateFile(mUnzipFile, pFileName.c_str(), 0) == UNZ_OK);
	}

	return lExist;
}

IOError ZipArchive::FileRead(void* pDest, int pSize)
{
	IOError lErr = IO_ERROR_READING_FROM_STREAM;

	if (mIOType == READ_ONLY)
	{
		if (mUnzipFile != 0)
		{
			int lSize = ::unzReadCurrentFile(mUnzipFile, pDest, pSize);
			if (lSize > 0)
			{
				lErr = IO_OK;
			}
			else if(lSize == 0)
			{
				lErr = IO_NO_DATA_AVAILABLE;
			}
		}
		else
		{
			lErr = IO_STREAM_NOT_OPEN;
		}
	}

	return lErr;
}

IOError ZipArchive::FileWrite(void* pSource, int pSize)
{
	IOError lErr = IO_ERROR_WRITING_TO_STREAM;

	if (mIOType == WRITE_ONLY || mIOType == WRITE_APPEND)
	{
		mOutFile.WriteData(pSource, pSize);
/*		if (::zipWriteInFileInZip(mZipFile, pSource, pSize) == ZIP_OK)
		{
			lErr = IO_OK;
		}
*/
	}

	return lErr;
}

int64 ZipArchive::FileSize()
{
	int64 lSize = 0;

	if (mIOType == READ_ONLY && mUnzipFile != 0)
	{
		unz_file_info64 lFileInfo;
		if (::unzGetCurrentFileInfo64(mUnzipFile, &lFileInfo, 0, 0, 0, 0, 0, 0) == UNZ_OK)
		{
			lSize = (int64)lFileInfo.uncompressed_size;
		}
	}
	else if(mIOType == WRITE_ONLY || mIOType == WRITE_APPEND)
	{
		lSize = mOutFile.GetSize();
	}

	return lSize;
}

void ZipArchive::FileSeek(int64 pOffset, FileOrigin pOrigin)
{
	if (mIOType == READ_ONLY && mUnzipFile != 0 && mFileSize != 0)
	{
		int64 lOffset = pOffset;
		switch(pOrigin)
		{
		case FSEEK_SET:
			lOffset = pOffset;
			break;
		case FSEEK_CUR:
			lOffset = ::unztell64(mUnzipFile) + pOffset;
			break;
		case FSEEK_END:
			lOffset = (uLong)(mFileSize + pOffset - 1);
			break;
		}

		::unzCloseCurrentFile(mUnzipFile);
		::unzOpenCurrentFile(mUnzipFile);

		// Now skip lOffset bytes of data.
		uint8 lBuf[1024];
		int64 lSteps = lOffset / 1024;
		int64 lRest  = lOffset % 1024;

		int64 i;
		for (i = 0; i < lSteps; i++)
		{
			::unzReadCurrentFile(mUnzipFile, lBuf, 1024);
		}

		if (lRest > 0)
		{
			::unzReadCurrentFile(mUnzipFile, lBuf, (uLong)lRest);
		}
	}
	else if(mIOType == WRITE_ONLY || mIOType == WRITE_APPEND)
	{
		switch(pOrigin)
		{
		case SEEK_SET:
			mOutFile.Seek(pOffset, File::FSEEK_SET);
			break;
		case SEEK_CUR:
			mOutFile.Seek(pOffset, File::FSEEK_CUR);
			break;
		case SEEK_END:
			mOutFile.Seek(pOffset, File::FSEEK_END);
			break;
		}
	}
}

bool ZipArchive::ExtractFile(const str& pFileName, 
			     const str& pDestFileName,
			     int pBufferSize, SizeUnit pUnit)
{
	if (mIOType != READ_ONLY)
	{
		return false;
	}

	bool lOK = FileOpen(pFileName);

	if (lOK == true)
	{
		DiskFile lFile;
		if (lFile.Open(pDestFileName, DiskFile::MODE_WRITE, false, Endian::TYPE_LITTLE_ENDIAN) == true)
		{
			int lBufferSize = (int)pBufferSize * (int)pUnit;
			int64 lDataSize = FileSize();
			uint8* lBuffer = new uint8[(unsigned)lBufferSize];

			int64 lNumChunks = lDataSize / lBufferSize;
			int lRest = (int)(lDataSize % (int64)lBufferSize);

			for (int64 i = 0; i < lNumChunks; i++)
			{
				FileRead(lBuffer, lBufferSize);
				lFile.WriteData(lBuffer, lBufferSize);
			}

			if (lRest != 0)
			{
				FileRead(lBuffer, lRest);
				lFile.WriteData(lBuffer, lRest);
			}

			FileClose();
			lFile.Close();

			delete[] lBuffer;

			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

loginstance(GENERAL_RESOURCES, ZipArchive);

}
