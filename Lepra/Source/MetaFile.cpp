
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games

#include "../Include/MetaFile.h"
#include "../Include/Path.h"

namespace Lepra
{

MetaFile::MetaFile() :
	mDiskFile(0),
	mArchiveFile(0),
	mReader(0),
	mWriter(0),
	mEndian(Endian::TYPE_BIG_ENDIAN)
{
}

MetaFile::MetaFile(Reader* pReader) :
	mDiskFile(0),
	mArchiveFile(0),
	mReader(pReader),
	mWriter(0),
	mEndian(Endian::TYPE_BIG_ENDIAN)
{
}

MetaFile::MetaFile(Writer* pWriter) :
	mDiskFile(0),
	mArchiveFile(0),
	mReader(0),
	mWriter(pWriter),
	mEndian(Endian::TYPE_BIG_ENDIAN)
{
}

MetaFile::MetaFile(Reader* pReader, Writer* pWriter) :
	mDiskFile(0),
	mArchiveFile(0),
	mReader(pReader),
	mWriter(pWriter),
	mEndian(Endian::TYPE_BIG_ENDIAN)
{
}

MetaFile::~MetaFile()
{
	Close();
}

bool MetaFile::Open(const String& pFileName, OpenMode pMode, bool pCreatePath, Endian::EndianType pEndian)
{
	Close();
	SetEndian(pEndian);

	bool lOk = false;

	size_t lSplitIndex = 0;
	String lPath;
	String lFile;
	bool lContinue = true;

	// Find a valid combination of archive and file...
	while (lContinue)
	{
		lContinue = SplitPath(pFileName, lPath, lFile, lSplitIndex);

		if (lSplitIndex == 0)
		{
			lOk = DiskFile::Exists(lPath);
			if (lOk)
			{
				AllocDiskFile();
				lOk = mDiskFile->Open(lPath, ToDiskFileMode(pMode), pCreatePath, pEndian);
				if (lOk)
				{
					lContinue = false;
				}
				else
				{
					Close();
				}
			}
		}
		else
		{
			String lArchiveName;
			lOk = FindValidArchiveName(lPath, lArchiveName);

			if (lOk)
			{
				AllocArchiveFile(lArchiveName);
				lOk = mArchiveFile->Open(lFile, ToArchiveMode(pMode), pEndian);
			}

			if (lOk)
			{
				lContinue = false;
			}
			else
			{
				Close();
			}
		}

		lSplitIndex++;
	}

	lOk = (mDiskFile != 0 || mArchiveFile != 0);

	return lOk;
}

void MetaFile::Close()
{
	if (mDiskFile != 0)
	{
		mDiskFile->Close();
		delete mDiskFile;
		mDiskFile = 0;
	}
	else if (mArchiveFile != 0)
	{
		mArchiveFile->Close();
		delete mArchiveFile;
		mArchiveFile = 0;
	}
}

void MetaFile::SetEndian(Endian::EndianType pEndian)
{
	mEndian = pEndian;
	Parent::SetEndian(pEndian);
	if (mDiskFile != 0)
	{
		mDiskFile->SetEndian(mEndian);
	}
	else if (mArchiveFile != 0)
	{
		mArchiveFile->SetEndian(mEndian);
	}
}

Endian::EndianType MetaFile::GetEndian()
{
	return mEndian;
}

int64 MetaFile::GetSize() const
{
	int64 lSize = 0;

	if (mDiskFile != 0)
	{
		lSize = mDiskFile->GetSize();
	}
	else if (mArchiveFile != 0)
	{
		lSize = mArchiveFile->GetSize();
	}

	return lSize;
}

int64 MetaFile::Tell() const
{
	int64 lPos = 0;

	if (mDiskFile != 0)
	{
		lPos = mDiskFile->Tell();
	}
	else if (mArchiveFile != 0)
	{
		lPos = mArchiveFile->Tell();
	}

	return lPos;
}

int64 MetaFile::Seek(int64 pOffset, FileOrigin pFrom)
{
	int64 lOffset = 0;

	if (mDiskFile != 0)
	{
		lOffset = mDiskFile->Seek(pOffset, pFrom);
	}
	else if (mArchiveFile != 0)
	{
		lOffset = mArchiveFile->Seek(pOffset, pFrom);
	}

	return lOffset;
}

String MetaFile::GetFullName() const
{
	if (mDiskFile != 0)
	{
		return mDiskFile->GetFullName();
	}
	else if (mArchiveFile != 0)
	{
		return mArchiveFile->GetFullName();
	}

	return _T("");
}

String MetaFile::GetName() const
{
	if (mDiskFile != 0)
	{
		return mDiskFile->GetName();
	}
	else if (mArchiveFile != 0)
	{
		return mArchiveFile->GetName();
	}

	return _T("");
}

String MetaFile::GetPath() const
{
	if (mDiskFile != 0)
	{
		return mDiskFile->GetPath();
	}
	else if (mArchiveFile != 0)
	{
		return mArchiveFile->GetPath();
	}

	return _T("");
}

IOError MetaFile::ReadData(void* pBuffer, size_t pSize)
{
	IOError lError = IO_FILE_NOT_OPEN;

	if (mDiskFile != 0)
	{
		lError = mDiskFile->ReadData(pBuffer, pSize);
	}
	else if (mArchiveFile != 0)
	{
		lError = mArchiveFile->ReadData(pBuffer, pSize);
	}

	return lError;
}

IOError MetaFile::WriteData(const void* pBuffer, size_t pSize)
{
	IOError lError = IO_FILE_NOT_OPEN;

	if (mDiskFile != 0)
	{
		lError = mDiskFile->WriteData(pBuffer, pSize);
	}
	else if (mArchiveFile != 0)
	{
		lError = mArchiveFile->WriteData(pBuffer, pSize);
	}

	return lError;
}


int64 MetaFile::GetAvailable() const
{
	int64 lAvailable = 0;

	if (mDiskFile != 0)
	{
		lAvailable = mDiskFile->GetAvailable();
	}
	else if (mArchiveFile != 0)
	{
		lAvailable = mArchiveFile->GetAvailable();
	}

	return lAvailable;
}

IOError MetaFile::ReadRaw(void* pBuffer, size_t pSize)
{
	IOError lError = IO_FILE_NOT_OPEN;

	if (mDiskFile != 0)
	{
		lError = mDiskFile->ReadRaw(pBuffer, pSize);
	}
	else if (mArchiveFile != 0)
	{
		lError = mArchiveFile->ReadRaw(pBuffer, pSize);
	}

	return lError;
}

IOError MetaFile::Skip(size_t pSize)
{
	return (Parent::Skip(pSize));
}

IOError MetaFile::WriteRaw(const void* pBuffer, size_t pSize)
{
	IOError lError = IO_FILE_NOT_OPEN;

	if (mDiskFile != 0)
	{
		lError = mDiskFile->WriteRaw(pBuffer, pSize);
	}
	else if (mArchiveFile != 0)
	{
		lError = mArchiveFile->WriteRaw(pBuffer, pSize);
	}

	return lError;
}

void MetaFile::Flush()
{
	if (mDiskFile != 0)
	{
		mDiskFile->Flush();
	}
	else if (mArchiveFile != 0)
	{
		mArchiveFile->Flush();
	}
}

void MetaFile::AllocDiskFile()
{
	if(mReader != 0 && mWriter != 0)
	{
		mDiskFile = new DiskFile(mReader, mWriter);
	}
	else if(mReader != 0)
	{
		mDiskFile = new DiskFile(mReader);
	}
	else if(mWriter != 0)
	{
		mDiskFile = new DiskFile(mWriter);
	}
	else
	{
		mDiskFile = new DiskFile();
	}

	mDiskFile->SetEndian(mEndian);
}

void MetaFile::AllocArchiveFile(const String& pArchiveName)
{
	if(mReader != 0 && mWriter != 0)
	{
		mArchiveFile = new ArchiveFile(pArchiveName, mReader, mWriter);
	}
	else if(mReader != 0)
	{
		mArchiveFile = new ArchiveFile(pArchiveName, mReader);
	}
	else if(mWriter != 0)
	{
		mArchiveFile = new ArchiveFile(pArchiveName, mWriter);
	}
	else
	{
		mArchiveFile = new ArchiveFile(pArchiveName);
	}

	
	if (IsZipFile(Path::GetExtension(pArchiveName)))
	{
		mArchiveFile->SetArchiveType(ArchiveFile::ZIP);
	}
	else
	{
		mArchiveFile->SetArchiveType(ArchiveFile::UNCOMPRESSED);
	}

	mArchiveFile->SetEndian(mEndian);
}

DiskFile::OpenMode MetaFile::ToDiskFileMode(OpenMode pMode)
{
	DiskFile::OpenMode lMode = DiskFile::MODE_READ;
	switch (pMode)
	{
		case READ_ONLY:
		{
			lMode = DiskFile::MODE_READ;
			break;
		}
		case WRITE_ONLY:
		{
			lMode = DiskFile::MODE_WRITE;
			break;
		}
		case WRITE_APPEND:
		{
			lMode = DiskFile::MODE_WRITE_APPEND;
			break;
		}
	}

	return lMode;
}

ArchiveFile::OpenMode MetaFile::ToArchiveMode(OpenMode pMode)
{
	ArchiveFile::OpenMode lMode = ArchiveFile::READ_ONLY;
	switch (pMode)
	{
		case READ_ONLY:
		{
			lMode = ArchiveFile::READ_ONLY;
			break;
		}
		case WRITE_ONLY:
		{
			lMode = ArchiveFile::WRITE_ONLY;
			break;
		}
		case WRITE_APPEND:
		{
			lMode = ArchiveFile::WRITE_APPEND;
			break;
		}
	}

	return lMode;
}


bool MetaFile::IsZipFile(const String& pExtension)
{
	bool lOk = false;

	if (smZipExtensions != 0)
	{
		lOk = (std::find(smZipExtensions->begin(), smZipExtensions->end(), pExtension) != smZipExtensions->end());
	}
	return lOk;
}

bool MetaFile::IsUncompressedArchive(const String& pExtension)
{
	bool lOk = false;

	if (smArchiveExtensions != 0)
	{
		lOk = (std::find(smArchiveExtensions->begin(), smArchiveExtensions->end(), pExtension) != smArchiveExtensions->end());
	}

	return lOk;
}

void MetaFile::AddZipExtension(const String& pExtension)
{
	if (smZipExtensions == 0)
	{
		smZipExtensions = new std::list<String>();
	}

	smZipExtensions->push_back(pExtension);
	smZipExtensions->unique();
}

void MetaFile::AddUncompressedExtension(const String& pExtension)
{
	if (smArchiveExtensions == 0)
	{
		smArchiveExtensions = new std::list<String>();
	}

	smArchiveExtensions->push_back(pExtension);
	smArchiveExtensions->unique();
}

void MetaFile::ClearExtensions()
{
	if (smZipExtensions)
	{
		smZipExtensions->clear();
		delete smZipExtensions;
		smZipExtensions = 0;
	}

	if (smArchiveExtensions)
	{
		smArchiveExtensions->clear();
		delete smArchiveExtensions;
		smArchiveExtensions = 0;
	}
}

std::list<String>* MetaFile::smZipExtensions;
std::list<String>* MetaFile::smArchiveExtensions;

bool MetaFile::SplitPath(const String& pFilename, String& pLeft, String& pRight, size_t pSplitIndex)
{
	bool lOk = true;
	size_t lSplitIndex = pFilename.length();
	size_t i;

	for (i = 0; i < pSplitIndex; i++)
	{
		int lIndex1 = (int)pFilename.rfind((tchar)'/', lSplitIndex-1);
		int lIndex2 = (int)pFilename.rfind((tchar)'\\', lSplitIndex-1);

		if (lIndex1 > lIndex2)
		{
			lSplitIndex = lIndex1;
		}
		else if (lIndex2 > lIndex1)
		{
			lSplitIndex = lIndex2;
		}
		else if (lIndex1 == -1 && lIndex2 == -1)
		{
			lSplitIndex = 0;
			lOk = false;
			break;
		}
	}

	pLeft = pFilename.substr(0, lSplitIndex);
	pRight = pFilename.substr(lSplitIndex);

	if (pRight[0] == '/' || pRight[0] == '\\')
	{
		pRight.erase(0, 1);
	}

	return lOk;
}

bool MetaFile::FindValidArchiveName(const String& pArchivePrefix, String& pFullArchiveName)
{
	std::list<String>::iterator lIter;

	bool lOk = false;

	if (smZipExtensions != 0)
	{
		for (lIter = smZipExtensions->begin(); lIter != smZipExtensions->end(); ++lIter)
		{
			String lFileName(pArchivePrefix + (*lIter));
			if (DiskFile::Exists(lFileName) == true)
			{
				pFullArchiveName = lFileName;
				lOk = true;
				break;
			}
		}
	}

	if (lOk == false && smArchiveExtensions != 0)
	{
		for (lIter = smArchiveExtensions->begin(); lIter != smArchiveExtensions->end(); ++lIter)
		{
			String lFileName(pArchivePrefix + (*lIter));
			if (DiskFile::Exists(lFileName) == true)
			{
				pFullArchiveName = lFileName;
				lOk = true;
				break;
			}
		}
	}

	return lOk;
}

} // End namespace.
