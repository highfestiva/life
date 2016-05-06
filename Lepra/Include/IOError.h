/*
	Class:  -
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

	NOTES:

	An enumeration shared by File and FileArchive.
*/

#pragma once

namespace lepra {

enum IOError {
	kIoOk = 0,
	kIoFileNotOpen,
	kIoFileAlreadyOpen,
	kIoFileNotFound,
	kIoFileCreationFailed,
	kIoStreamNotOpen,
	kIoErrorWritingToStream,
	kIoErrorReadingFromStream,
	kIoInvalidFormat,
	kIoInvalidFilename,
	kIoInvalidMode,
	kIoPermissionDenied,
	kIoInvalidOperation,
	kIoCancel,
	kIoPackageNotOpen,
	kIoBufferOverflow,
	kIoBufferUnderflow,
	kIoNoDataAvailable,

	kNumIoErrors,
};

const char* GetIOErrorString(IOError err);

}
