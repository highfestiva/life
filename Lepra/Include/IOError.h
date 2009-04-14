/*
	Class:  -
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games

	NOTES:

	An enumeration shared by File and FileArchive.
*/

#ifndef IOERROR_H
#define IOERROR_H

namespace Lepra
{

enum IOError
{
	IO_OK = 0,
	IO_FILE_NOT_OPEN,
	IO_FILE_ALREADY_OPEN,
	IO_FILE_NOT_FOUND,
	IO_FILE_CREATION_FAILED,
	IO_STREAM_NOT_OPEN,
	IO_ERROR_WRITING_TO_STREAM,
	IO_ERROR_READING_FROM_STREAM,
	IO_INVALID_FORMAT,
	IO_INVALID_FILENAME,
	IO_INVALID_MODE,
	IO_PERMISSION_DENIED,
	IO_INVALID_OPERATION,
	IO_CANCEL,
	IO_PACKAGE_NOT_OPEN,
	IO_BUFFER_OVERFLOW,
	IO_BUFFER_UNDERFLOW,
	IO_NO_DATA_AVAILABLE,

	NUM_IO_ERRORS,
};

const char* GetIOErrorString(IOError pErr);

} // End namespace.

#endif
