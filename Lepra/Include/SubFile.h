
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

// A subfile takes an OPEN file and allows you to work an a PART of that file.
// This class does not allow any operations outside the file area given by
// CURRENT_FILE_POS and (CURRENT_FILE_POS+SUB_FILE_SIZE).



#pragma once

#include "../Include/File.h"
#include "../Include/InputStream.h"
#include "../Include/OutputStream.h"



namespace Lepra
{



class SubFile: public File, protected InputStream, protected OutputStream
{
public:
	SubFile(File* pMasterFile, int64 pFileSize);
	virtual ~SubFile();

	void Flush();
	void Close();	// Dummy.
	int64 GetAvailable() const;
	IOError Skip(size_t pLength);
	IOError ReadRaw(void* pData, size_t pLength);
	IOError WriteRaw(const void* pData, size_t pLength);

	int64 GetSize() const;
	int64 Tell() const;
	int64 Seek(int64 pOffset, FileOrigin pFrom);

private:
	File* mMasterFile;
	int64 mFileStart;
	int64 mFileSize;
};



}
