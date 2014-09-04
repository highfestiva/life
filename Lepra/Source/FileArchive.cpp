
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include <ctype.h>
#include "../Include/FileArchive.h"
#include "../Include/ProgressCallback.h"
#include "../Include/String.h"
#include "../Include/StringUtility.h"



namespace Lepra
{



FileArchive::FileArchive() :
	mArchiveFileName(),
	mTempFileName(),
	mIOType(READ_ONLY),
	mCurrentFindIndex(0),
	mCurrentWriteFile(0),
	mFileHandleCounter(0),
	mWriteBuffer(0),
	mWriteBufferSize(2 * 1024 * 1024),
	mWriteBufferPos(0),
	mCurrentWritePos(0),
	mCallback(0)
{
}

FileArchive::~FileArchive()
{
	CloseArchive();
}

IOError FileArchive::OpenArchive(const str& pArchiveFileName, IOType pIOType)
{
	CloseArchive();

	mArchiveFileName = pArchiveFileName;

	str lTempFileName(pArchiveFileName + _T(".tmp"));

	mIOType = pIOType;

	switch(mIOType)
	{
	case READ_ONLY:

		if (mArchiveFile.Open(mArchiveFileName, DiskFile::MODE_READ, false, Endian::TYPE_LITTLE_ENDIAN) == false)
		{
			return IO_FILE_NOT_FOUND;
		}

		return ReadHeader();

		break;
	case WRITE_ONLY:

		if (mArchiveFile.Open(lTempFileName, DiskFile::MODE_WRITE, false, Endian::TYPE_LITTLE_ENDIAN) == false)
		{
			return IO_ERROR_WRITING_TO_STREAM;
		}

		mWriteBuffer = new uint8[mWriteBufferSize];
		mWriteBufferPos = 0;
		mCurrentWritePos = 0;

		break;
	case WRITE_APPEND:
		if (mArchiveFile.Open(mArchiveFileName, DiskFile::MODE_READ, false, Endian::TYPE_LITTLE_ENDIAN) == false)
		{
			return IO_FILE_NOT_FOUND;
		}

		int64 lHeaderOffset = 0;
		IOError lError = ReadHeader(&lHeaderOffset, true);

		mArchiveFile.Close();

		if (lError != IO_OK)
		{
			return lError;
		}

		if (mArchiveFile.Open(mArchiveFileName, DiskFile::MODE_WRITE_APPEND, false, Endian::TYPE_LITTLE_ENDIAN) == false)
		{
			return IO_ERROR_WRITING_TO_STREAM;
		}

		mArchiveFile.SeekSet(lHeaderOffset);

		mWriteBuffer = new uint8[mWriteBufferSize];
		mWriteBufferPos = 0;
		mCurrentWritePos = lHeaderOffset;
		break;
	};

	return IO_OK;
}

void FileArchive::CloseArchive()
{
	if (mIOType == READ_ONLY)
	{
		if (mArchiveFile.IsOpen())
		{
			mArchiveFile.Close();
		}
	}
	else if(mIOType == WRITE_ONLY || mIOType == WRITE_APPEND)
	{
		if (mArchiveFile.IsOpen())
		{
			if (mWriteBufferPos != 0)
			{
				mArchiveFile.WriteData(mWriteBuffer, mWriteBufferPos);
				mWriteBufferPos = 0;
			}

			WriteHeader();

			mArchiveFile.Close();

			if (mIOType == WRITE_ONLY)
			{
				// Replace the original file with the temp file.
				remove(astrutil::Encode(mArchiveFileName).c_str());
				rename(astrutil::Encode(mTempFileName).c_str(), astrutil::Encode(mArchiveFileName).c_str());
			}
		}
	}

	if (mWriteBuffer != 0)
	{
		delete[] mWriteBuffer;
		mWriteBuffer = 0;
		mWriteBufferPos = 0;
	}

	mArchiveFileName = _T("");
	mTempFileName = _T("");

	mOpenFileTable.RemoveAll();

	FileNameTable::Iterator lIter = mFileNameTable.First();
	while (lIter != mFileNameTable.End())
	{
/*		if (mIOType == WRITE_ONLY || mIOType == WRITE_APPEND)
		{
			// Delete filename, since it was allocated and inserted 
			// in FileOpen().
			char* lFileName = (char*)lIter.GetKey();
			delete[] lFileName;
		}
*/
		FileArchiveFile* lFile = *lIter;
		mFileNameTable.Remove(lIter++);

		delete lFile;
	}

	mFileNameList.clear();
}

void FileArchive::CloseAndRemoveArchive()
{
	mArchiveFile.Close();

	if (mIOType == READ_ONLY)
	{
#ifdef LEPRA_POSIX
		::remove(astrutil::Encode(mArchiveFileName).c_str()); // TODO: Find a unicode-version of this.
#else
		::_wremove(wstrutil::Encode(mArchiveFileName).c_str());
#endif
	}
	else
	{
#ifdef LEPRA_POSIX
		::remove(astrutil::Encode(mTempFileName).c_str()); // TODO: Find a unicode-version of this.
#else
		::_wremove(wstrutil::Encode(mTempFileName).c_str());
#endif
	}

	if (mWriteBuffer != 0)
	{
		delete[] mWriteBuffer;
		mWriteBuffer = 0;
		mWriteBufferPos = 0;
	}

	mArchiveFileName = _T("");
	mTempFileName = _T("");

	mOpenFileTable.RemoveAll();

	FileNameTable::Iterator lIter = mFileNameTable.First();
	while (lIter != mFileNameTable.End())
	{
		/* JB: using strings instead of tchar*.
		if (mIOType == WRITE_ONLY || mIOType == WRITE_APPEND)
		{
			// Delete filename, since it was allocated and inserted 
			// in FileOpen().
			str& lFileName = lIter.GetKey();
			delete[] lFileName;
		}*/

		FileArchiveFile* lFile = *lIter;
		mFileNameTable.Remove(lIter++);
		delete (lFile);
	}

    mFileNameList.clear();
}

IOError FileArchive::ReadHeader(int64* pHeaderOffset, bool pFillFileNameList)
{
	char lData[8];

	int lTailSize = 4 + sizeof(int64);
	mArchiveFile.SeekEnd(-lTailSize);

	mArchiveFile.ReadData(lData, lTailSize);

	if (lData[lTailSize - 4] == 'B' &&
	   lData[lTailSize - 3] == 'U' &&
	   lData[lTailSize - 2] == 'N' &&
	   lData[lTailSize - 1] == 'T')
	{
		int64 lHeaderOffset = ((int64*)lData)[0];
		mArchiveFile.SeekSet(lHeaderOffset);

		if (pHeaderOffset != 0)
		{
			*pHeaderOffset = lHeaderOffset;
		}

		int lFileCount;
		mArchiveFile.ReadData(&lFileCount, sizeof(lFileCount));

		for (int i = 0; i < lFileCount; i++)
		{
			str lFileName;
			mArchiveFile.ReadLine(lFileName);
			strutil::ToLower(lFileName);

			FileArchiveFile* lFile = new FileArchiveFile(lFileName);

			mArchiveFile.ReadData(&lFile->mSize, sizeof(lFile->mSize));
			mArchiveFile.ReadData(&lFile->mStartOffset, sizeof(lFile->mStartOffset));

			lFile->mCurrentPos = 0;

			mFileNameTable.Insert(lFile->mFileName, lFile);

			if (pFillFileNameList == true)
			{
				mFileNameList.push_back(lFile);
			}
		}

		return IO_OK;
	}

	return IO_INVALID_FORMAT;
}

IOError FileArchive::WriteHeader()
{
	int64 lHeaderOffset = mArchiveFile.Tell();

	int lFileCount = (int)mFileNameList.size();

	mArchiveFile.WriteData(&lFileCount, sizeof(lFileCount));

	FileList::iterator lIter;

	for (lIter = mFileNameList.begin();
		lIter != mFileNameList.end();
		++lIter)
	{
		FileArchiveFile* lFile = *lIter;

		// Check for illegal '\n'
		if (lFile->mFileName.find('\n', 0) >= 0)
		{
			return IO_INVALID_FILENAME;
		}

		mArchiveFile.WriteString(lFile->mFileName);
		mArchiveFile.Write(_T('\n'));

		mArchiveFile.WriteData(&lFile->mSize, sizeof(lFile->mSize));
		mArchiveFile.WriteData(&lFile->mStartOffset, sizeof(lFile->mStartOffset));
	}

	mArchiveFile.WriteData(&lHeaderOffset, sizeof(lHeaderOffset));
	mArchiveFile.WriteString<tchar>(_T("BUNT"));

	return IO_OK;
}

int FileArchive::GetFileCount()
{
	return mFileNameTable.GetCount();
}

str FileArchive::FileFindFirst()
{
	str lFileName;

	if (mIOType == READ_ONLY)
	{
		FileNameTable::Iterator lIter;
		lIter = mFileNameTable.First();

		if (lIter != mFileNameTable.End())
		{
			mCurrentFindIterator = lIter;
			lFileName = (*lIter)->mFileName;
		}
	}

	return lFileName;
}

str FileArchive::FileFindNext()
{
	str lFileName;

	if (mIOType == READ_ONLY)
	{
		++mCurrentFindIterator;

		if (mCurrentFindIterator != mFileNameTable.End())
		{
			lFileName = (*mCurrentFindIterator)->mFileName;
		}
	}

	return lFileName;
}

int FileArchive::FileOpen(const str& pFileName)
{
	if (pFileName == _T("") ||
	   mIOType == INSERT_ONLY ||
	   mArchiveFile.IsOpen() == false)
	{
		return 0;
	}

	str lFileName(pFileName);
	strutil::ToLower(lFileName);

	// Find the file...
	FileNameTable::Iterator lIter = mFileNameTable.First();
	FileArchiveFile* lFile = 0;
	while (lIter != mFileNameTable.End())
	{
		FileArchiveFile* lTempFile = *lIter;

		if (strutil::CompareIgnoreCase(lFileName, lTempFile->mFileName) == 0)
		{
			lFile = lTempFile;
			break;
		}

		++lIter;
	}

	switch(mIOType)
	{
	case READ_ONLY:
		{
			// TODO: Is it possible to optimize this?
    
			// File must exist.
			if (lFile != 0)
			{
				lFile->mCurrentPos = 0;

				mFileHandleCounter++;
				mOpenFileTable.Insert(mFileHandleCounter, lFile);

				return mFileHandleCounter;
			}
		}
		break;
	case WRITE_ONLY:
	case WRITE_APPEND:
		{
			// File mustn't exist.
			if (lFile == 0)
			{
				// Close the possibly opened file...
				if (mCurrentWriteFile != 0)
				{
					mCurrentWriteFile->mSize = mCurrentWritePos - mCurrentWriteFile->mStartOffset;
					mCurrentWriteFile = 0;
				}

				lFile = new FileArchiveFile(lFileName);
				lFile->mStartOffset = mCurrentWritePos;
				lFile->mCurrentPos = 0;
				lFile->mSize = 0;

				mCurrentWriteFile = lFile;

				// Inserting filename as a key... Deleting it in CloseArchive().
				mFileNameTable.Insert(lFileName, lFile);
				mFileNameList.push_back(lFile);

				return 1;
			}
		}
		break;
	};

	return 0;
}

void FileArchive::FileClose(int pFileHandle)
{
	if (mIOType == READ_ONLY)
	{
		mOpenFileTable.Remove(pFileHandle);
	}
	else if(mIOType == WRITE_ONLY || mIOType == WRITE_APPEND)
	{
		if (mCurrentWriteFile != 0)
		{
			mCurrentWriteFile->mSize = mCurrentWritePos - mCurrentWriteFile->mStartOffset;
			mCurrentWriteFile = 0;
		}
	}
}

bool FileArchive::FileExist(const str& pFileName)
{
	str lFileName(pFileName);
	strutil::ToLower(lFileName);

	return mFileNameTable.Find(lFileName) != mFileNameTable.End();
}

IOError FileArchive::FileRead(void* pDest, int pSize, int pFileHandle)
{
	if (mArchiveFile.IsOpen() == false)
	{
		return IO_PACKAGE_NOT_OPEN;
	}

	if (mIOType == READ_ONLY)
	{
		FileArchiveFile* lFile = GetFile(pFileHandle);

		if (lFile != 0)
		{
			// Trying to read outside the file?
			if (lFile->mCurrentPos + pSize > lFile->mSize)
			{
				return IO_ERROR_READING_FROM_STREAM;
			}

			mArchiveFile.SeekSet(lFile->mStartOffset + lFile->mCurrentPos);
			if (mArchiveFile.ReadData(pDest, pSize) == false)
			{
				return IO_ERROR_READING_FROM_STREAM;
			}

			lFile->mCurrentPos += pSize;
		}
		else
		{
			return IO_STREAM_NOT_OPEN;
		}
	}
	else
	{
		return IO_ERROR_READING_FROM_STREAM;
	}

	return IO_OK;
}

IOError FileArchive::FileWrite(void* pSource, int pSize)
{
	if (mArchiveFile.IsOpen() == false)
	{
		return IO_PACKAGE_NOT_OPEN;
	}

	if (mIOType == WRITE_ONLY || mIOType == WRITE_APPEND)
	{
		// While pSize is larger than what's left in the write buffer,
		// or the write buffer itself.
		int lWriteSize = pSize;

		while (((int64)mWriteBufferPos + lWriteSize) > (int64)mWriteBufferSize)
		{
			// Make sure the write buffer is filled to every byte...
			int lRest = mWriteBufferSize - mWriteBufferPos;
			if (lRest > 0)
			{
				::memcpy(mWriteBuffer + mWriteBufferPos, (const char*)pSource + (pSize - lWriteSize), (size_t)lRest);
				lWriteSize -= lRest;
			}

			// Write the whole buffer to disc.
			mArchiveFile.WriteData(mWriteBuffer, mWriteBufferSize);
			mWriteBufferPos = 0;
			
			// Update the file position...
			mCurrentWritePos += lRest;
		}

		// Write whatever is left from the previous check.
		::memcpy(mWriteBuffer + mWriteBufferPos, (const char*)pSource + (pSize - lWriteSize), (size_t)lWriteSize);
		mWriteBufferPos += lWriteSize;
		mCurrentWritePos += lWriteSize;
	}
	else
	{
		return IO_ERROR_WRITING_TO_STREAM;
	}

	return IO_OK;
}

int64 FileArchive::FileSize(int pFileHandle)
{
	if (mArchiveFile.IsOpen() == false ||
	   mIOType != READ_ONLY)
	{
		return -1;
	}

	FileArchiveFile* lFile = GetFile(pFileHandle);
	if (lFile != 0)
	{
		return lFile->mSize;
	}

	return 0;
}

FileArchive::FileArchiveFile* FileArchive::GetFile(int pFileHandle)
{
	if (mIOType == READ_ONLY)
	{
		FileTable::Iterator lIter = mOpenFileTable.Find(pFileHandle);

		if (lIter != mOpenFileTable.End())
		{
			return *lIter;
		}
	}

	return 0;
}

void FileArchive::FileSeek(int64 pOffset, FileOrigin pOrigin, int pFileHandle)
{
	if (mArchiveFile.IsOpen() == false ||
	   mIOType != READ_ONLY)
	{
		return;
	}

	FileArchiveFile* lFile = GetFile(pFileHandle);
	if (lFile != 0)
	{
		switch(pOrigin)
		{
		case FSEEK_SET:
			lFile->mCurrentPos = pOffset;
			break;
		case FSEEK_CUR:
			lFile->mCurrentPos += pOffset;
			break;
		case FSEEK_END:
			lFile->mCurrentPos = (lFile->mSize + pOffset) - 1;
			break;
		};

		if (lFile->mCurrentPos < 0)
		{
			lFile->mCurrentPos = 0;
		}

		if (lFile->mCurrentPos > lFile->mSize)
		{
			lFile->mCurrentPos = lFile->mSize;
		}
	}
}

IOError FileArchive::InsertArchive(const str& pArchiveFileName)
{
/*
	if (mCallback)
	{
		mCallback->SetProgressMax(files)
	}
	for (file = 0; file < files; file++)
	{
		...
	...
*/
	const int lChunkSize = 1024*1024; // 1 MB

	if (mIOType == INSERT_ONLY)
	{
		IOError lError;

		lError = OpenArchive(mArchiveFileName, READ_ONLY);
		if (lError != IO_OK)
		{
			return lError;
		}

		FileArchive lInsertArchive;
		lError = lInsertArchive.OpenArchive(pArchiveFileName, READ_ONLY);
		if (lError != IO_OK)
		{
			return lError;
		}

		// Writing is done using the temp file name...
		FileArchive lOutArchive;
		lOutArchive.OpenArchive(mArchiveFileName, WRITE_ONLY);

		int lProgress = 0;
		if (mCallback != 0)
		{
			int lFiles = lInsertArchive.GetFileCount() + GetFileCount();
			mCallback->SetProgressPos(lProgress);
			mCallback->SetProgressMax(lFiles);
		}

		// Iterate through all files in "this"...
		FileNameTable::Iterator lIterator;
		for (lIterator = mFileNameTable.First();
			lIterator != mFileNameTable.End();
			++lIterator)
		{
			FileNameTable::Iterator lFindIter =
				lInsertArchive.mFileNameTable.Find((*lIterator)->mFileName);

			// For each file in "this" that doesn't exist in pArchive, write.
			if (lFindIter == lInsertArchive.mFileNameTable.End())
			{
				// Open the source file.
				int lFileHandle = FileOpen((*lIterator)->mFileName);
				int64 lFileSize = FileSize(lFileHandle);

				// Open the file to write.
				lOutArchive.FileOpen((*lIterator)->mFileName);

				int64 lNumChunks = lFileSize / (int64)lChunkSize;
				int lRest = (int)(lFileSize % (int64)lChunkSize);

				uint8* lData = new uint8[lChunkSize];

				// Copy the file chunkwise.
				for (int64 i = 0; i < lNumChunks; i++)
				{
					FileRead(lData, lChunkSize, lFileHandle);
					lOutArchive.FileWrite(lData, lChunkSize);
				}

				if (lRest != 0)
				{
					FileRead(lData, lRest, lFileHandle);
					lOutArchive.FileWrite(lData, lRest);
				}

				lOutArchive.FileClose(0);

				delete[] lData;

				FileClose(lFileHandle);
			}

			if (mCallback != 0)
			{
				mCallback->SetProgressPos(++lProgress);
			}
		}

		// Iterate through all files in pArchive...
		for (lIterator = lInsertArchive.mFileNameTable.First();
			lIterator != lInsertArchive.mFileNameTable.End();
			++lIterator)
		{
			// Open the source file.
			int lFileHandle = lInsertArchive.FileOpen((*lIterator)->mFileName);
			int64 lFileSize = lInsertArchive.FileSize(lFileHandle);

			// Open the file to write.
			lOutArchive.FileOpen((*lIterator)->mFileName);

			int64 lNumChunks = lFileSize / (int64)lChunkSize;
			int lRest = (int)(lFileSize % (int64)lChunkSize);

			uint8* lData = new uint8[lChunkSize];

			// Copy the file chunkwise.
			for (int64 i = 0; i < lNumChunks; i++)
			{
				lInsertArchive.FileRead(lData, lChunkSize, lFileHandle);
				lOutArchive.FileWrite(lData, lChunkSize);
			}

			if (lRest != 0)
			{
				lInsertArchive.FileRead(lData, lRest, lFileHandle);
				lOutArchive.FileWrite(lData, lRest);
			}

			lOutArchive.FileClose(0);

			delete[] lData;

			lInsertArchive.FileClose(lFileHandle);

			if (mCallback != 0)
			{
				mCallback->SetProgressPos(++lProgress);
			}
		}

		lInsertArchive.CloseArchive();
		CloseArchive();

		lOutArchive.CloseArchive();

		return IO_OK;
	}

	return IO_INVALID_MODE;
}



void FileArchive::SetProgressCallback(ProgressCallback* pCallback)
{
	mCallback = pCallback;
}

bool FileArchive::AddFile(const str& pFileName, const str& pDestFileName, 
						  int pBufferSize, SizeUnit pUnit)
{
	if (mIOType != WRITE_ONLY && mIOType != WRITE_APPEND)
	{
		return false;
	}

	DiskFile lFile;
	if (lFile.Open(pFileName, DiskFile::MODE_READ, false, Endian::TYPE_LITTLE_ENDIAN) == true)
	{
		int lBufferSize = (int)pBufferSize * (int)pUnit;
		int64 lDataSize   = lFile.GetSize();
		uint8* lBuffer = new uint8[(unsigned)lBufferSize];

		int64 lNumChunks = lDataSize / lBufferSize;
		int lRest      = (int)(lDataSize % (int64)lBufferSize);

		FileOpen(pDestFileName);

		for (int64 i = 0; i < lNumChunks; i++)
		{
			lFile.ReadData(lBuffer, lBufferSize);
			FileWrite(lBuffer, lBufferSize);
		}

		if (lRest != 0)
		{
			lFile.ReadData(lBuffer, lRest);
			FileWrite(lBuffer, lRest);
		}

		lFile.Close();
		FileClose(0);

		delete[] lBuffer;

		return true;
	}
	else
	{
		return false;
	}
}

bool FileArchive::ExtractFile(const str& pFileName, const str& pDestFileName,
							  int pBufferSize, SizeUnit pUnit)
{
	if (mIOType != READ_ONLY)
	{
		return false;
	}

	int lHandle = FileOpen(pFileName);

	if (lHandle != 0)
	{
		DiskFile lFile;
		if (lFile.Open(pDestFileName, DiskFile::MODE_WRITE, true, Endian::TYPE_LITTLE_ENDIAN) == true)
		{
			int lBufferSize = (int)pBufferSize * (int)pUnit;
			int64 lDataSize   = FileSize(lHandle);
			uint8* lBuffer = new uint8[(unsigned)lBufferSize];

			int64 lNumChunks = lDataSize / lBufferSize;
			int lRest      = (int)(lDataSize % (int64)lBufferSize);

			for (int64 i = 0; i < lNumChunks; i++)
			{
				FileRead(lBuffer, lBufferSize, lHandle);
				lFile.WriteData(lBuffer, lBufferSize);
			}

			if (lRest != 0)
			{
				FileRead(lBuffer, lRest, lHandle);
				lFile.WriteData(lBuffer, lRest);
			}

			FileClose(lHandle);
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



}
