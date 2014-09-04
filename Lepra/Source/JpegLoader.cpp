/*
	Class:  JpegLoader
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#include "pch.h"
#include "../../ThirdParty/jpeg-6b/jinclude.h"
#include "../../ThirdParty/jpeg-6b/jpeglib.h"
#include "../../ThirdParty/jpeg-6b/jerror.h"

#define INT32 a_stupid_workaround_that_undefines_a_typedef
#include "../Include/ArchiveFile.h"
#include "../Include/Canvas.h"
#include "../Include/DiskFile.h"
#include "../Include/MetaFile.h"
#include "../Include/Graphics2D.h"
#include "../Include/JpegLoader.h"
#include "../Include/ProgressCallback.h"
#undef INT32

namespace Lepra
{

/*
	A class used to (locally, within this file) get access to the 
	jpeg loader's private members.
*/

class JpegFriend
{
public:
	static inline Reader* GetReader(JpegLoader* pJpegLoader)
	{
		return pJpegLoader->mReader;
	}
	static inline Writer* GetWriter(JpegLoader* pJpegLoader)
	{
		return pJpegLoader->mWriter;
	}
};


/*
	C style source management functions.
*/
void InitSource(j_decompress_ptr pCInfo);
boolean FillInputBuffer(j_decompress_ptr pCInfo);
void SkipInputData(j_decompress_ptr pCInfo, long pNumBytes);
void TerminateSource(j_decompress_ptr pCInfo);

/*
	C style destination management functions.
*/
void InitDestination(j_compress_ptr pCInfo);
boolean EmptyOutputBuffer(j_compress_ptr pCInfo);
void TerminateDestination(j_compress_ptr pCInfo);

/*
	C style source manager struct.
*/
struct SourceManager
{
	jpeg_source_mgr mSourceManager;
	uint8 mIOBuffer[JpegLoader::IO_BUFFER_SIZE];
	int mIOBufferSize;
	JpegLoader* mJpegLoader;
};

/*
	C style destination manager struct.
*/
struct DestinationManager
{
	jpeg_destination_mgr mDestManager;
	uint8 mIOBuffer[JpegLoader::IO_BUFFER_SIZE];
	int mIOBufferSize;
	JpegLoader* mJpegLoader;
};




/*
	Regular JpegLoader member functions.
*/

JpegLoader::JpegLoader() :
	mReader(0),
	mWriter(0)
{
}

JpegLoader::~JpegLoader()
{
}

JpegLoader::Status JpegLoader::Load(const str& pFileName, Canvas& pCanvas, ProgressCallback* pProgress)
{
	Status lStatus = STATUS_SUCCESS;
	MetaFile lFile;

	if (lFile.Open(pFileName, MetaFile::READ_ONLY) == false)
	{
		lStatus = STATUS_OPEN_ERROR;
	}

	if (lStatus == STATUS_SUCCESS)
	{
		mReader = &lFile;
		lStatus = Load(pCanvas, pProgress);
		lFile.Close();
	}

	return lStatus;
}

JpegLoader::Status JpegLoader::Save(const str& pFileName, const Canvas& pCanvas)
{
	Status lStatus = STATUS_SUCCESS;
	DiskFile lFile;

	if (lFile.Open(pFileName, DiskFile::MODE_WRITE) == false)
	{
		lStatus = STATUS_OPEN_ERROR;
	}

	if (lStatus == STATUS_SUCCESS)
	{
		mWriter = &lFile;
		lStatus = Save(pCanvas);
		lFile.Close();
	}

	return lStatus;
}

JpegLoader::Status JpegLoader::Load(const str& pArchiveName, const str& pFileName, Canvas& pCanvas, ProgressCallback* pProgress)
{
	Status lStatus = STATUS_SUCCESS;
	ArchiveFile lFile(pArchiveName);

	if (lFile.Open(pFileName, ArchiveFile::READ_ONLY) == false)
	{
		lStatus = STATUS_OPEN_ERROR;
	}

	if (lStatus == STATUS_SUCCESS)
	{
		mReader = &lFile;
		lStatus = Load(pCanvas, pProgress);
		lFile.Close();
	}

	return lStatus;
}

JpegLoader::Status JpegLoader::Save(const str& pArchiveName, const str& pFileName, const Canvas& pCanvas)
{
	Status lStatus = STATUS_SUCCESS;
	ArchiveFile lFile(pArchiveName);

	if (lFile.Open(pFileName, ArchiveFile::WRITE_ONLY) == false)
	{
		lStatus = STATUS_OPEN_ERROR;
	}

	if (lStatus == STATUS_SUCCESS)
	{
		mReader = &lFile;
		lStatus = Save(pCanvas);
		lFile.Close();
	}

	return lStatus;
}

JpegLoader::Status JpegLoader::Load(Reader& pReader, Canvas& pCanvas, ProgressCallback* pProgress)
{
	mReader = &pReader;
	return Load(pCanvas, pProgress);
}

JpegLoader::Status JpegLoader::Save(Writer& pWriter, const Canvas& pCanvas)
{
	mWriter = &pWriter;
	return Save(pCanvas);
}

JpegLoader::Status JpegLoader::Load(Canvas& pCanvas, ProgressCallback* pProgress)
{
	jpeg_decompress_struct lCInfo;
	jpeg_error_mgr lJErr;

	lCInfo.err = jpeg_std_error(&lJErr);
	jpeg_create_decompress(&lCInfo);

	InitSourceManager(&lCInfo);

	if (jpeg_read_header(&lCInfo, TRUE) != JPEG_HEADER_OK)
	{
		jpeg_destroy_decompress(&lCInfo);
		return STATUS_READ_HEADER_ERROR;
	}

	if (jpeg_start_decompress(&lCInfo) != TRUE)
	{
		jpeg_destroy_decompress(&lCInfo);
		return STATUS_READ_PICTURE_ERROR;
	}

	if (lCInfo.output_components == 1)
	{
		pCanvas.Reset(lCInfo.output_width, lCInfo.output_height, Canvas::BITDEPTH_8_BIT);
		
		// Create grayscale palette.
		Color lPalette[256];
		for (int i = 0; i < 256; i++)
		{
			lPalette[i].Set(i, i, i, i);
		}

		pCanvas.SetPalette(lPalette);
	}
	else
	{
		pCanvas.Reset(lCInfo.output_width, lCInfo.output_height, Canvas::BITDEPTH_24_BIT);
	}
	pCanvas.CreateBuffer();
	uint8* lBuffer = (uint8*)pCanvas.GetBuffer();
	int lRowStride = pCanvas.GetPitch() * pCanvas.GetPixelByteSize();
	int lScanLines = pCanvas.GetHeight();
	if (pProgress)
	{
		pProgress->SetProgressMax(lScanLines-1);
	}

	const int lReadRowsAtATime = 16;
	JSAMPROW lOffset[lReadRowsAtATime];
	for (int x = 0; x < lReadRowsAtATime; ++x)
	{
		lOffset[x] = &lBuffer[x*lRowStride];
	}

	if (pProgress)
	{
		pProgress->SetProgressPos(0);
	}
	for (int i = 0; i < lScanLines;)
	{
		const int lRows = jpeg_read_scanlines(&lCInfo, lOffset, lReadRowsAtATime);
		i += lRows;
		if (pProgress)
		{
			pProgress->SetProgressPos(i);
		}
		for (int x = 0; x < lReadRowsAtATime; ++x)
		{
			lOffset[x] += lRowStride*lRows;
		}
	}

	jpeg_finish_decompress(&lCInfo);
	jpeg_destroy_decompress(&lCInfo);

	pCanvas.SwapRGBOrder();

	return STATUS_SUCCESS;
}

JpegLoader::Status JpegLoader::Save(const Canvas& pCanvas)
{
	jpeg_compress_struct lCInfo;
	jpeg_error_mgr lJErr;
	lCInfo.err = jpeg_std_error(&lJErr);
	jpeg_create_compress(&lCInfo);

	InitDestinationManager(&lCInfo);

	lCInfo.image_width = pCanvas.GetWidth();
	lCInfo.image_height = pCanvas.GetHeight();
	lCInfo.input_components = 3;
	lCInfo.in_color_space = JCS_RGB;

	// We must make sure that the image is in 24-bit RGB mode.
	Canvas lCopy(pCanvas, true);
	if (lCopy.GetBitDepth() != Canvas::BITDEPTH_24_BIT)
	{
		lCopy.ConvertBitDepth(Canvas::BITDEPTH_24_BIT);
	}

	lCopy.SwapRGBOrder();

	jpeg_set_defaults(&lCInfo);
	jpeg_start_compress(&lCInfo, TRUE);

	uint8* lBuffer = (uint8*)lCopy.GetBuffer();
	int lRowStride = lCopy.GetPitch() * lCopy.GetPixelByteSize();

	JSAMPROW lOffset[1];
	lOffset[0] = lBuffer;

	for (unsigned i = 0; i < lCopy.GetHeight(); i++)
	{
		jpeg_write_scanlines(&lCInfo, lOffset, 1);
		lOffset[0] += lRowStride;
	}

	jpeg_finish_compress(&lCInfo);
	jpeg_destroy_compress(&lCInfo);

	return STATUS_SUCCESS;
}

void JpegLoader::InitSourceManager(j_decompress_ptr pCInfo)
{
	if (pCInfo->src == NULL) 
	{	
		// First time for this JPEG object?
		pCInfo->src = (jpeg_source_mgr*)new SourceManager;
	}

	SourceManager* lSrc = (SourceManager*)pCInfo->src;
	lSrc->mSourceManager.init_source       = InitSource;
	lSrc->mSourceManager.fill_input_buffer = FillInputBuffer;
	lSrc->mSourceManager.skip_input_data   = SkipInputData;
	lSrc->mSourceManager.resync_to_restart = jpeg_resync_to_restart; /* use default method */
	lSrc->mSourceManager.term_source       = TerminateSource;

	// Forces FillInputBuffer() on first read.
	lSrc->mIOBufferSize = 0;
	lSrc->mJpegLoader = this;
	lSrc->mSourceManager.bytes_in_buffer = 0; 
	lSrc->mSourceManager.next_input_byte = NULL;
}

void JpegLoader::InitDestinationManager(j_compress_ptr pCInfo)
{
	if (pCInfo->dest == NULL)
	{	
		// First time for this JPEG object?
		pCInfo->dest = (jpeg_destination_mgr*)new DestinationManager;
	}

	DestinationManager* lDest = (DestinationManager*)pCInfo->dest;

	lDest->mIOBufferSize = 0;
	lDest->mJpegLoader = this;

	lDest->mDestManager.init_destination    = InitDestination;
	lDest->mDestManager.empty_output_buffer = EmptyOutputBuffer;
	lDest->mDestManager.term_destination    = TerminateDestination;
}


/*
	And here are the C style functions.
*/



void InitSource(j_decompress_ptr pCInfo)
{
	SourceManager* lSrc = (SourceManager*)pCInfo->src;
	lSrc->mIOBufferSize = 0;
}

boolean FillInputBuffer(j_decompress_ptr pCInfo)
{
	SourceManager* lSrc = (SourceManager*)pCInfo->src;

	Reader* lReader = JpegFriend::GetReader(lSrc->mJpegLoader);

	int lNumBytes = (int)lReader->GetAvailable();
	if (lNumBytes > JpegLoader::IO_BUFFER_SIZE)
	{
		lNumBytes = JpegLoader::IO_BUFFER_SIZE;
	}

	IOError lErr = lReader->ReadData(lSrc->mIOBuffer, lNumBytes);

	if (lErr != IO_OK)
	{
		WARNMS(pCInfo, JWRN_JPEG_EOF);

		/* Insert a fake EOI marker */
		lSrc->mIOBuffer[0] = (JOCTET) 0xFF;
		lSrc->mIOBuffer[1] = (JOCTET) JPEG_EOI;

		lSrc->mSourceManager.next_input_byte = lSrc->mIOBuffer;
		lSrc->mSourceManager.bytes_in_buffer = 2;
	}
	else
	{
		lSrc->mIOBufferSize = lNumBytes;
		lSrc->mSourceManager.next_input_byte = lSrc->mIOBuffer;
		lSrc->mSourceManager.bytes_in_buffer = lSrc->mIOBufferSize;
	}

	return TRUE;
}

void SkipInputData(j_decompress_ptr pCInfo, long pNumBytes)
{
	SourceManager* lSrc = (SourceManager*)pCInfo->src;

	if (pNumBytes > 0)
	{
		while (pNumBytes > (long)lSrc->mSourceManager.bytes_in_buffer) 
		{
			pNumBytes -= (long)lSrc->mSourceManager.bytes_in_buffer;
			FillInputBuffer(pCInfo);
		}

		lSrc->mSourceManager.next_input_byte += (size_t)pNumBytes;
		lSrc->mSourceManager.bytes_in_buffer -= (size_t)pNumBytes;
	}
}

void TerminateSource(j_decompress_ptr pCInfo)
{
	SourceManager* lSrc = (SourceManager*)pCInfo->src;
	lSrc->mIOBufferSize = 0;
	lSrc->mJpegLoader = 0;
	delete lSrc;
}






void InitDestination(j_compress_ptr pCInfo)
{
	DestinationManager* lDest = (DestinationManager*)pCInfo->dest;

	lDest->mDestManager.next_output_byte = lDest->mIOBuffer;
	lDest->mDestManager.free_in_buffer = JpegLoader::IO_BUFFER_SIZE;
}


boolean EmptyOutputBuffer(j_compress_ptr pCInfo)
{
	DestinationManager* lDest = (DestinationManager*)pCInfo->dest;

	Writer* lWriter = JpegFriend::GetWriter(lDest->mJpegLoader);
	lWriter->WriteData(lDest->mIOBuffer, JpegLoader::IO_BUFFER_SIZE);

	lDest->mDestManager.next_output_byte = lDest->mIOBuffer;
	lDest->mDestManager.free_in_buffer = JpegLoader::IO_BUFFER_SIZE;

	return TRUE;
}

void TerminateDestination(j_compress_ptr pCInfo)
{
	DestinationManager* lDest = (DestinationManager*)pCInfo->dest;
	size_t lDataCount = JpegLoader::IO_BUFFER_SIZE - lDest->mDestManager.free_in_buffer;

	/* Write any data remaining in the buffer */
	if (lDataCount > 0) 
	{
		JpegFriend::GetWriter(lDest->mJpegLoader)->WriteData(lDest->mIOBuffer, (unsigned)lDataCount);
	}

	lDest->mIOBufferSize = 0;
	lDest->mJpegLoader = 0;
	delete lDest;
}

} // End namespace.
