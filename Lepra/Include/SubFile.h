
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

// A subfile takes an OPEN file and allows you to work an a PART of that file.
// This class does not allow any operations outside the file area given by
// CURRENT_FILE_POS and (CURRENT_FILE_POS+SUB_FILE_SIZE).



#pragma once

#include "../include/file.h"
#include "../include/inputstream.h"
#include "../include/outputstream.h"



namespace lepra {



class SubFile: public File, protected InputStream, protected OutputStream {
public:
	SubFile(File* master_file, int64 file_size);
	virtual ~SubFile();

	void Flush();
	void Close();	// Dummy.
	int64 GetAvailable() const;
	IOError Skip(size_t length);
	IOError ReadRaw(void* data, size_t length);
	IOError WriteRaw(const void* data, size_t length);

	int64 GetSize() const;
	int64 Tell() const;
	int64 Seek(int64 offset, FileOrigin from);

private:
	File* master_file_;
	int64 file_start_;
	int64 file_size_;
};



}
