
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/ArchiveFile.h"
#include "../Include/Path.h"
#include "../Include/String.h"

#include <string.h>



namespace Lepra
{



ArchiveFile::ArchiveFile(const str& pArchiveName):
	File(Endian::TYPE_BIG_ENDIAN, Endian::TYPE_BIG_ENDIAN, 0, 0),
	mArchiveFileName(pArchiveName),
	mIsZipArchive(false),
	mFileHandle(0),
	mFileName(_T("")),
	mPath(_T("")),
	mFileSize(0),
	mDataBuffer(0),
	mDataBufferSize(1024 * 1024), // 1 Mb
	mDataSize(0),
	mCurrentDBPos(0),
	mCurrentPos(0),
	mWriter(0),
	mReader(0),
	mMode(READ_ONLY)
{
	Reader::SetInputStream(this);
	Writer::SetOutputStream(this);

	mDataBuffer = new uint8[(int)mDataBufferSize];

	mIsZipArchive = (strutil::CompareIgnoreCase(strutil::Right(mArchiveFileName, 4), _T(".zip")) == 0);
}

ArchiveFile::ArchiveFile(const str& pArchiveName, Reader* pReader):
	File(Endian::TYPE_BIG_ENDIAN, Endian::TYPE_BIG_ENDIAN, 0, 0),
	mArchiveFileName(pArchiveName),
	mIsZipArchive(false),
	mFileHandle(0),
	mFileName(_T("")),
	mPath(_T("")),
	mFileSize(0),
	mDataBuffer(0),
	mDataBufferSize(1024 * 1024), // 1 Mb
	mDataSize(0),
	mCurrentDBPos(0),
	mCurrentPos(0),
	mWriter(0),
	mReader(pReader),
	mMode(READ_ONLY)
{
	Reader::SetInputStream(this);
	Writer::SetOutputStream(this);

	if (mReader != 0)
	{
		mReader->SetInputStream(this);
	}

	mDataBuffer = new uint8[(int)mDataBufferSize];

	mIsZipArchive = (strutil::CompareIgnoreCase(strutil::Right(mArchiveFileName, 4), _T(".zip")) == 0);
}

ArchiveFile::ArchiveFile(const str& pArchiveName, Writer* pWriter):
	File(Endian::TYPE_BIG_ENDIAN, Endian::TYPE_BIG_ENDIAN, 0, 0),
	mArchiveFileName(pArchiveName),
	mIsZipArchive(false),
	mFileHandle(0),
	mFileName(_T("")),
	mPath(_T("")),
	mFileSize(0),
	mDataBuffer(0),
	mDataBufferSize(1024 * 1024), // 1 Mb
	mDataSize(0),
	mCurrentDBPos(0),
	mCurrentPos(0),
	mWriter(pWriter),
	mReader(0),
	mMode(READ_ONLY)
{
	Reader::SetInputStream(this);
	Writer::SetOutputStream(this);

	if (mWriter != 0)
	{
		mWriter->SetOutputStream(this);
	}

	mDataBuffer = new uint8[(int)mDataBufferSize];

	mIsZipArchive = (strutil::CompareIgnoreCase(strutil::Right(mArchiveFileName, 4), _T(".zip")) == 0);
}

ArchiveFile::ArchiveFile(const str& pArchiveName, Reader* pReader, Writer* pWriter):
	File(Endian::TYPE_BIG_ENDIAN, Endian::TYPE_BIG_ENDIAN, 0, 0),
	mArchiveFileName(pArchiveName),
	mIsZipArchive(false),
	mFileHandle(0),
	mFileName(_T("")),
	mPath(_T("")),
	mFileSize(0),
	mDataBuffer(0),
	mDataBufferSize(1024 * 1024), // 1 Mb
	mDataSize(0),
	mCurrentDBPos(0),
	mCurrentPos(0),
	mWriter(pWriter),
	mReader(pReader),
	mMode(READ_ONLY)
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

	mDataBuffer = new uint8[(int)mDataBufferSize];

	mIsZipArchive = (strutil::CompareIgnoreCase(strutil::Right(mArchiveFileName, 4), _T(".zip")) == 0);
}

ArchiveFile::~ArchiveFile()
{
	Close();

	delete[] mDataBuffer;
}

void ArchiveFile::ExtractPathAndFileName(const str& pFileName)
{
	Path::SplitPath(pFileName, mPath, mFileName);
	InputStream::SetName(mFileName);
	OutputStream::SetName(mFileName);
}

bool ArchiveFile::Open(const str& pFileName, OpenMode pMode, Endian::EndianType pEndian)
{
	Close();

	SetEndian(pEndian);

	bool lOK = true;

	if (lOK == true)
	{
		switch(pMode)
		{
		case READ_ONLY:
			Parent::SetMode(Parent::READ_MODE);
			Parent::ClearMode(Parent::WRITE_MODE);
			break;
		case WRITE_ONLY:
		case WRITE_APPEND:
			Parent::SetMode(Parent::WRITE_MODE);
			Parent::ClearMode(Parent::READ_MODE);
			break;
		default:
			lOK = false;
		}
	}

	if (lOK == true)
	{
		ExtractPathAndFileName(pFileName);

		switch(pMode)
		{
		case READ_ONLY:
			if (mIsZipArchive == true)
			{
				lOK = OpenZipForReading(pFileName, pMode);
			}
			else
			{
				lOK = OpenForReading(pFileName, pMode);
			}
			break;
		case WRITE_ONLY:
		case WRITE_APPEND:
			if (mIsZipArchive == true)
			{
				lOK = OpenZipForWriting(pFileName, pMode);
			}
			else
			{
				lOK = OpenForWriting(pFileName, pMode);
			}
			break;
		};
	}

	return lOK;
}

bool ArchiveFile::OpenForReading(const str& pFileName, OpenMode pMode)
{
	bool lOK = (mArchive.OpenArchive(mArchiveFileName, FileArchive::READ_ONLY) == IO_OK);

	if (lOK == true)
	{
		mFileHandle = mArchive.FileOpen(pFileName);
		lOK = (mFileHandle != 0);
	}

	if (lOK == true)
	{
		// Get the file size.
		mFileSize = mArchive.FileSize(mFileHandle);

		mMode = pMode;
		mCurrentPos = 0;
		mCurrentDBPos = 0;
	}

	return lOK;
}

bool ArchiveFile::OpenZipForReading(const str& pFileName, OpenMode pMode)
{
	bool lOK = (mZipArchive.OpenArchive(mArchiveFileName, ZipArchive::READ_ONLY) == IO_OK);

	if (lOK == true)
	{
		lOK = mZipArchive.FileOpen(pFileName);
	}

	if (lOK == true)
	{
		// Get the file size.
		mFileSize = mZipArchive.FileSize();

		mMode = pMode;
		mCurrentPos = 0;
		mCurrentDBPos = 0;

		// Treat the file handle as a flag telling wether this file is open.
		mFileHandle = 1;
	}

	return lOK;
}

bool ArchiveFile::OpenForWriting(const str& pFileName, OpenMode pMode)
{
	bool lOK = true;

	// First check if the file already exist in the archive... If it does,
	// we have to remove it from the archive. In append mode, we have to
	// extract the file first, and then add it to the archive again.

	FileArchive lOriginal;
	lOK = (lOriginal.OpenArchive(mArchiveFileName, FileArchive::READ_ONLY) == IO_OK);

	str lTempFile;

	if (lOK == true)
	{
		if (lOriginal.FileExist(pFileName) == true)
		{
			// The file exists in the archive, thus we have to remove or extract it.
			// First generate a temporary file name for the target archive.
			str lTempName = DiskFile::GenerateUniqueFileName(mPath);
			lOK = (mArchive.OpenArchive(lTempName, FileArchive::WRITE_ONLY) == IO_OK);

			if (lOK == true)
			{
				// Copy all files from the original to the target archive (mArchive),
				// except pFileName.
				CopyArchiveFiles(lOriginal, mArchive, pFileName);
				
				if (pMode == WRITE_APPEND)
				{
					// Now create a temp file and extract the file.
					lTempFile = DiskFile::GenerateUniqueFileName(mPath);
					lOK = lOriginal.ExtractFile(pFileName, lTempFile, 1);
				}
		
				lOriginal.CloseAndRemoveArchive();
				mArchive.CloseArchive();
				DiskFile::Rename(lTempName, mArchiveFileName);
			}
			else
			{
				lOriginal.CloseArchive();
			}
		}
		else
		{
			lOriginal.CloseArchive();
		}
	}

	// At this point we know that the archive doesn't contain the file pFileName.
	if (lOK == true)
	{
		lOK = (mArchive.OpenArchive(mArchiveFileName, FileArchive::WRITE_APPEND) == IO_OK);
	}

	if (lOK == true)
	{
		mFileHandle = mArchive.FileOpen(pFileName);

		if (pMode == WRITE_APPEND)
		{
			// We have to take care about the temp file...
			// Insert it into the archive.
			DiskFile lFile;
			if (lFile.Open(lTempFile, DiskFile::MODE_READ, false, Endian::TYPE_LITTLE_ENDIAN) == true)
			{
				uint8 lBuf[1024];
				uint64 lSteps = lFile.GetSize() / 1024;
				uint64 lRest  = lFile.GetSize() % 1024;
				uint64 i;

				for (i = 0; i < lSteps; i++)
				{
					lFile.ReadData(lBuf, 1024);
					mArchive.FileWrite(lBuf, 1024);
				}

				if (lRest > 0)
				{
					lFile.ReadData(lBuf, (size_t)lRest);
					mArchive.FileWrite(lBuf, (int)lRest);
				}

				lFile.Close();
				DiskFile::Delete(lTempFile);
			}
		}
	}

	if (lOK == true)
	{
		mMode = pMode;
		mCurrentPos = 0;
		mCurrentDBPos = 0;
	}

	return lOK;
}

bool ArchiveFile::OpenZipForWriting(const str& pFileName, OpenMode pMode)
{
	bool lOK = true;

	// First check if the file already exist in the archive... If it does,
	// we have to remove it from the archive. In append mode, we have to
	// extract the file first, and then add it to the archive again.

	ZipArchive lOriginal;
	lOK = (lOriginal.OpenArchive(mArchiveFileName, ZipArchive::READ_ONLY) == IO_OK);

	str lTempFile;

	if (lOK == true)
	{
		if (lOriginal.FileExist(pFileName) == true)
		{
			// The file exists in the archive, thus we have to remove or extract it.
			// First generate a temporary file name for the target archive.
			str lTempName = DiskFile::GenerateUniqueFileName(mPath);
			lOK = (mZipArchive.OpenArchive(lTempName, ZipArchive::WRITE_ONLY) == IO_OK);

			if (lOK == true)
			{
				// Copy all files from the original to the target archive (mArchive),
				// except pFileName.
				CopyZipArchiveFiles(lOriginal, mZipArchive, pFileName);
				
				if (pMode == WRITE_APPEND)
				{
					// Now create a temp file and extract the file.
					lTempFile = DiskFile::GenerateUniqueFileName(mPath);
					lOK = lOriginal.ExtractFile(pFileName, lTempFile, 1, ZipArchive::MB);
				}
		
				lOriginal.CloseAndRemoveArchive();
				mZipArchive.CloseArchive();
				//Parent::ExtractPathAndFileName(lTempName, 
				DiskFile::Rename(lTempName, mArchiveFileName);
			}
			else
			{
				lOriginal.CloseArchive();
			}
		}
		else
		{
			lOriginal.CloseArchive();
		}
	}

	// At this point we know that the archive doesn't contain the file pFileName.
	if (lOK == true)
	{
		lOK = (mZipArchive.OpenArchive(mArchiveFileName, ZipArchive::WRITE_APPEND) == IO_OK);
	}

	if (lOK == true)
	{
		lOK = mZipArchive.FileOpen(pFileName);
	}

	mFileHandle = 0;

	if (lOK == true)
	{
		mFileHandle = 1;
		mMode = pMode;
		mCurrentPos = 0;
		mCurrentDBPos = 0;

		if (pMode == WRITE_APPEND)
		{
			// We have to take care about the temp file...
			// Insert it into the archive.
			DiskFile lFile;
			if (lFile.Open(lTempFile, DiskFile::MODE_READ, false, Endian::TYPE_LITTLE_ENDIAN) == true)
			{
				mCurrentPos = lFile.GetSize();

				uint8 lBuf[1024];
				uint64 lSteps = lFile.GetSize() / 1024;
				uint64 lRest  = lFile.GetSize() % 1024;
				uint64 i;

				for (i = 0; i < lSteps; i++)
				{
					lFile.ReadData(lBuf, 1024);
					mZipArchive.FileWrite(lBuf, 1024);
				}

				if (lRest > 0)
				{
					lFile.ReadData(lBuf, (size_t)lRest);
					mZipArchive.FileWrite(lBuf, (int)lRest);
				}

				lFile.Close();
				DiskFile::Delete(lTempFile);
			}
		}
	}

	return lOK;
}

void ArchiveFile::Close()
{
	if (IsOpen() == true)
	{
		FlushDataBuffer();

		if (mIsZipArchive == true)
		{
			mZipArchive.FileClose();
			mZipArchive.CloseArchive();
		}
		else
		{
			mArchive.FileClose(mFileHandle);
			mArchive.CloseArchive();
		}

		mFileHandle = 0;
	}

	mFileName = _T("");
	mPath = _T("");

	mCurrentDBPos = 0;
	mDataSize = 0;
}

void ArchiveFile::CopyArchiveFiles(FileArchive& pSource, FileArchive& pDest, const str& pExceptThisFile)
{
	str lFileName = pSource.FileFindFirst();
	
	while (lFileName != _T(""))
	{
		if (strutil::CompareIgnoreCase(pExceptThisFile, lFileName) != 0)
		{
			CopyFileBetweenArchives(pSource, pDest, lFileName);
		}

		lFileName = pSource.FileFindNext();
	}
}

void ArchiveFile::CopyZipArchiveFiles(ZipArchive& pSource, ZipArchive& pDest, const str& pExceptThisFile)
{
	str lFileName = pSource.FileFindFirst();
	
	while (lFileName != _T(""))
	{
		if (strutil::CompareIgnoreCase(pExceptThisFile, lFileName) != 0)
		{
			CopyFileBetweenZipArchives(pSource, pDest, lFileName);
		}

		lFileName = pSource.FileFindNext();
	}
}

bool ArchiveFile::CopyFileBetweenArchives(FileArchive& pSource, FileArchive& pDest, const str& pFileName)
{
	bool lOK = true;

	// Open source file.
	int lFileHandle = pSource.FileOpen(pFileName);
	int64 lFileSize = pSource.FileSize(lFileHandle);

	lOK = lFileHandle != 0 && pDest.FileOpen(pFileName) != 0;

	// Open dest file.
	if (lOK == true)
	{
		const int lChunkSize = 1024*1024; // 1 MB
		uint8* lData = new uint8[lChunkSize];

		int64 lNumChunks = lFileSize / (int64)lChunkSize;
		int lRest = (int)(lFileSize % (int64)lChunkSize);

		for (int64 i = 0; i < lNumChunks; i++)
		{
			pSource.FileRead(lData, lChunkSize, lFileHandle);
			pDest.FileWrite(lData, lChunkSize);
		}

		if (lRest != 0)
		{
			pSource.FileRead(lData, lRest, lFileHandle);
			pDest.FileWrite(lData, lRest);
		}

		pDest.FileClose(0);

		delete[] lData;
	}

	if (lFileHandle != 0)
	{
		pSource.FileClose(lFileHandle);
	}

	return lOK;
}

bool ArchiveFile::CopyFileBetweenZipArchives(ZipArchive& pSource, ZipArchive& pDest, const str& pFileName)
{
	bool lOK = true;

	// Open source file.
	bool lSourceOpened = pSource.FileOpen(pFileName);

	// Open dest file.
	lOK = (lSourceOpened && pDest.FileOpen(pFileName));

	int64 lFileSize = pSource.FileSize();

	if (lOK == true)
	{
		const int lChunkSize = 1024*1024; // 1 MB
		uint8* lData = new uint8[lChunkSize];

		int64 lNumChunks = lFileSize / (int64)lChunkSize;
		int lRest = (int)(lFileSize % (int64)lChunkSize);

		for (int64 i = 0; i < lNumChunks; i++)
		{
			pSource.FileRead(lData, lChunkSize);
			pDest.FileWrite(lData, lChunkSize);
		}

		if (lRest != 0)
		{
			pSource.FileRead(lData, lRest);
			pDest.FileWrite(lData, lRest);
		}

		pDest.FileClose();

		delete[] lData;
	}

	if (lSourceOpened == true)
	{
		pSource.FileClose();
	}

	return lOK;
}


void ArchiveFile::SetEndian(Endian::EndianType pEndian)
{
	Parent::SetEndian(pEndian);
	if (mReader != 0)
	{
		mReader->SetReaderEndian(pEndian);
	}
	if (mWriter != 0)
	{
		mWriter->SetWriterEndian(pEndian);
	}
}

str ArchiveFile::GetFullName() const
{
	return mPath + _T("/") + mFileName;
}

str ArchiveFile::GetName() const
{
	return mFileName;
}

str ArchiveFile::GetPath() const
{
	return mPath;
}

bool ArchiveFile::FlushDataBuffer()
{
	bool lOK = IsOpen();

	if (lOK == true)
	{
		if ((mMode == WRITE_ONLY || mMode == WRITE_APPEND) && mCurrentDBPos > 0)
		{
			int lWriteSize = mCurrentDBPos;
			mCurrentDBPos = 0;
			mDataSize = 0;

			if (mIsZipArchive == true)
			{
				lOK = (mZipArchive.FileWrite(mDataBuffer, lWriteSize) == IO_OK);
			}
			else
			{
				lOK = (mArchive.FileWrite(mDataBuffer, lWriteSize) == IO_OK);
			}
		}
	}

	return lOK;
}

bool ArchiveFile::RefillDataBuffer()
{
	bool lOK = IsOpen();

	if (lOK == true)
	{
		int64 lPos = mCurrentPos;
		int64 lReadSize = mFileSize - lPos;

		if (lReadSize > (int64)mDataBufferSize)
		{
			lReadSize = mDataBufferSize;
		}

		mCurrentDBPos = 0;
		mDataSize = (int)lReadSize;

		if (mIsZipArchive == true)
		{
			lOK = (mZipArchive.FileRead(mDataBuffer, (int)lReadSize) == IO_OK);
		}
		else
		{
			lOK = (mArchive.FileRead(mDataBuffer, (int)lReadSize, mFileHandle) == IO_OK);
		}
	}

	return lOK;
}

IOError ArchiveFile::ReadRaw(void* pBuffer, size_t pSize)
{
	IOError lError = IO_OK;

	if (IsOpen() == false)
	{
		lError = IO_STREAM_NOT_OPEN;
	}

	uint8* lBuffer = (uint8*)pBuffer;
	int lReadSize = (int)pSize;
	int lIndex = 0;

	if (lError == IO_OK)
	{
		while ((mCurrentDBPos + lReadSize) > mDataSize)
		{
			// Read what's left in the buffer.
			int lSize = mDataSize - mCurrentDBPos;
			if (lSize > 0)
			{
				memcpy(lBuffer + lIndex, mDataBuffer + mCurrentDBPos, (size_t)lSize);

				lReadSize -= lSize;
				lIndex += lSize;
				mCurrentPos += lSize;
			}

			if (RefillDataBuffer() == false)
			{
				if (lReadSize != 0)
					lError = IO_ERROR_READING_FROM_STREAM;
				break;
			}
		}
	}

	if (lError == IO_OK && lReadSize > 0)
	{
		memcpy(lBuffer + lIndex, mDataBuffer + mCurrentDBPos, (size_t)lReadSize);
		mCurrentDBPos += lReadSize;
		mCurrentPos += lReadSize;
	}

	return lError;
}

IOError ArchiveFile::Skip(size_t pSize)
{
	return (Parent::Skip(pSize));
}

IOError ArchiveFile::WriteRaw(const void* pBuffer, size_t pSize)
{
	IOError lError = IO_OK;

	if (IsOpen() == false)
	{
		lError = IO_STREAM_NOT_OPEN;
	}

	if (lError == IO_OK && mMode != WRITE_ONLY && mMode != WRITE_APPEND)
	{
		lError = IO_INVALID_OPERATION;
	}

	uint8* lBuffer = (uint8*)pBuffer;
	int lWriteSize = (int)pSize;
	int lIndex = 0;

	if (lError == IO_OK)
	{
		while ((mCurrentDBPos + lWriteSize) > mDataBufferSize)
		{
			// Fill the buffer.
			int lSize = mDataBufferSize - mCurrentDBPos;
			if (lSize > 0)
			{
				memcpy(mDataBuffer + mCurrentDBPos, lBuffer + lIndex, (size_t)lSize);

				lWriteSize -= lSize;
				lIndex += lSize;
				mCurrentPos += lSize;
				mCurrentDBPos += lSize;
			}

			if (FlushDataBuffer() == false)
			{
				lError = IO_ERROR_WRITING_TO_STREAM;
				break;
			}
		}
	}

	if (lError == IO_OK && lWriteSize > 0)
	{
		memcpy(mDataBuffer + mCurrentDBPos, lBuffer + lIndex, (size_t)lWriteSize);
		mCurrentDBPos += lWriteSize;
		mCurrentPos += lWriteSize;
	}

	return lError;
}

IOError ArchiveFile::ReadData(void* pBuffer, size_t pSize)
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

IOError ArchiveFile::WriteData(const void* pBuffer, size_t pSize)
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

int64 ArchiveFile::Tell() const
{
	int64 lPos = mCurrentPos;

	if (IsOpen() == false)
	{
		lPos = -1;
	}

	return lPos;
}

int64 ArchiveFile::Seek(int64 pOffset, FileOrigin pFrom)
{
	bool lOK = IsOpen();

	if (lOK == true && (mMode == WRITE_ONLY || mMode == WRITE_APPEND))
	{
		FlushDataBuffer();
	}

	if (lOK == true && mMode != READ_ONLY)
	{
		lOK = false;
	}

	if (lOK == true)
	{
		// DataBuffer start position (in "file coordinates").
		int64 lDBStart = (mCurrentPos - mCurrentDBPos);

		switch(pFrom)
		{
		case FSEEK_SET:
			mCurrentPos = pOffset;
			break;
		case FSEEK_CUR:
			mCurrentPos += pOffset;
			break;
		case FSEEK_END:
			mCurrentPos = (mFileSize + pOffset);
			break;
		};

		mCurrentDBPos = (int)(mCurrentPos - lDBStart);

		if (mMode == READ_ONLY && (mCurrentDBPos < 0 || mCurrentDBPos >= mDataSize))
		{
			if (mIsZipArchive == true)
			{
				mZipArchive.FileSeek(mCurrentPos, ZipArchive::FSEEK_SET);
			}
			else
			{
				mArchive.FileSeek(mCurrentPos, FileArchive::FSEEK_SET, mFileHandle);
			}

			if (mMode == READ_ONLY)
			{
				RefillDataBuffer();
			}
		}
		else if((mMode == WRITE_ONLY || mMode == WRITE_APPEND) &&
				mCurrentDBPos != 0)
		{
			if (mIsZipArchive == true)
			{
				mZipArchive.FileSeek(mCurrentPos, ZipArchive::FSEEK_SET);
			}
			else
			{
				mArchive.FileSeek(mCurrentPos, FileArchive::FSEEK_SET, mFileHandle);
			}
			mCurrentDBPos = 0;
		}
	}

	int64 lPos = -1;
	if (lOK == true)
	{
		lPos = Tell();
	}

	return lPos;
}



//
// Static functions.
//

bool ArchiveFile::ExtractFileFromArchive(const str& pArchiveName, const str& pFileName, bool pLocal)
{
	return ExtractFileFromArchive(pArchiveName, pFileName, pFileName, pLocal);
}

bool ArchiveFile::ExtractFileFromArchive(const str& pArchiveName, const str& pFileName, const str& pExtractedFileName, bool pLocal)
{
	bool lRet = false;

	ArchiveFile lArchivedFile(pArchiveName);
	if (lArchivedFile.Open(pFileName, ArchiveFile::READ_ONLY, Endian::TYPE_LITTLE_ENDIAN) == true)
	{
		DiskFile lDiskFile;
		bool lOpened = false;

		if (pLocal == false)
		{
			lOpened = lDiskFile.Open(pExtractedFileName, DiskFile::MODE_WRITE, true, Endian::TYPE_LITTLE_ENDIAN);
		}
		else
		{
			str lFileName = Path::GetCompositeFilename(pExtractedFileName);
			lOpened = lDiskFile.Open(lFileName, DiskFile::MODE_WRITE, false, Endian::TYPE_LITTLE_ENDIAN);
		}

		if (lOpened == true)
		{
			int64 lFileSize = lArchivedFile.GetSize();

			const int lChunkSize = 1024 * 1024;
			int64 lNumChunks = lFileSize / (int64)lChunkSize;
			int lRest = (int)(lFileSize % (int64)lChunkSize);

			uint8* lData = new uint8[lChunkSize];

			for (int64 i = 0; i < lNumChunks; i++)
			{
				lArchivedFile.ReadData(lData, lChunkSize);
				lDiskFile.WriteData(lData, lChunkSize);
			}

			lArchivedFile.ReadData(lData, lRest);
			lDiskFile.WriteData(lData, lRest);

			delete[]lData;
			lDiskFile.Close();

			lRet = true;
		}

		lArchivedFile.Close();
	}

	return lRet;
}

bool ArchiveFile::InsertFileIntoArchive(const str& pArchiveName, const str& pFileName, bool pLocal)
{
	return InsertFileIntoArchive(pArchiveName, pFileName, pFileName, pLocal);
}

bool ArchiveFile::InsertFileIntoArchive(const str& pArchiveName, const str& pFileName, const str& pInsertedFileName, bool pLocal)
{
	bool lRet = false;

	DiskFile lDiskFile;

	if (lDiskFile.Open(pFileName, DiskFile::MODE_READ, false, Endian::TYPE_LITTLE_ENDIAN) == true)
	{
		ArchiveFile lArchiveFile(pArchiveName);
		bool lOpened = false;

		if (pLocal == false)
		{
			lOpened = lArchiveFile.Open(pInsertedFileName, ArchiveFile::WRITE_ONLY, Endian::TYPE_LITTLE_ENDIAN);
		}
		else
		{
			str lFileName = Path::GetCompositeFilename(pInsertedFileName);
			lOpened = lArchiveFile.Open(lFileName, ArchiveFile::WRITE_ONLY, Endian::TYPE_LITTLE_ENDIAN);
		}

		if (lOpened == true)
		{
			int64 lFileSize = lDiskFile.GetSize();

			const int lChunkSize = 1024 * 1024;
			int64 lNumChunks = lFileSize / (int64)lChunkSize;
			int lRest = (int)(lFileSize % (int64)lChunkSize);

			uint8* lData = new uint8[lChunkSize];

			for (int64 i = 0; i < lNumChunks; i++)
			{
				lDiskFile.ReadData(lData, lChunkSize);
				lArchiveFile.WriteData(lData, lChunkSize);
			}

			lDiskFile.ReadData(lData, lRest);
			lArchiveFile.WriteData(lData, lRest);

			delete[]lData;
			lArchiveFile.Close();

			lRet = true;
		}

		lDiskFile.Close();
	}

	return lRet;
}


void ArchiveFile::operator=(const ArchiveFile&)
{
}



void ArchiveFile::SetArchiveType(ArchiveType pType)
{
	mIsZipArchive = (pType == ZIP);
}

bool ArchiveFile::IsOpen() const
{
	return (mFileHandle != 0);
}

int64 ArchiveFile::GetSize() const
{
	return mFileSize;
}

int64 ArchiveFile::GetAvailable() const
{
	return (mFileSize - mCurrentPos);
}

void ArchiveFile::Flush()
{
	FlushDataBuffer();
}



}
