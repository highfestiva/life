
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#define INT32 a_workaround_that_undefines_INT32_typedef
#define LEPRA_INCLUDE_NO_OS
#include "../Include/UiTEXLoader.h"
#include "../../ThirdParty/jpeg-6b/jinclude.h"
#include "../../ThirdParty/jpeg-6b/jpeglib.h"
#include "../../ThirdParty/jpeg-6b/jerror.h"
#include "../../Lepra/Include/ArchiveFile.h"
#include "../../Lepra/Include/Canvas.h"
#include "../../Lepra/Include/DiskFile.h"
#include "../../Lepra/Include/MetaFile.h"
#include "../../Lepra/Include/Graphics2D.h"
#include "../Include/UiTexture.h"
#include "../Include/UiTBC.h"
#undef LEPRA_INCLUDE_NO_OS
#undef INT32



namespace UiTbc
{



enum
{
	IO_BUFFER_SIZE = 4096,
};


/*
	A class used to (locally, within this file) get access to the 
	tex loader's private members.
*/

class TEXFriend
{
public:
	static inline File* GetLoadFile(TEXLoader* pTEXLoader)
	{
		return pTEXLoader->mLoadFile;
	}
	static inline File* GetSaveFile(TEXLoader* pTEXLoader)
	{
		return pTEXLoader->mSaveFile;
	}
};


/*
	C style jpeg source management functions.
*/
void TEXInitSource(j_decompress_ptr pCInfo);
boolean TEXFillInputBuffer(j_decompress_ptr pCInfo);
void TEXSkipInputData(j_decompress_ptr pCInfo, long pNumBytes);
void TEXTerminateSource(j_decompress_ptr pCInfo);

/*
	C style jpeg destination management functions.
*/
void TEXInitDestination(j_compress_ptr pCInfo);
boolean TEXEmptyOutputBuffer(j_compress_ptr pCInfo);
void TEXTerminateDestination(j_compress_ptr pCInfo);

/*
	C style jpeg source manager struct.
*/
struct SourceManager
{
	jpeg_source_mgr mSourceManager;
	unsigned char mIOBuffer[IO_BUFFER_SIZE];
	int mIOBufferSize;
	TEXLoader* mTEXLoader;
};

/*
	C style jpeg destination manager struct.
*/
struct DestinationManager
{
	jpeg_destination_mgr mDestManager;
	unsigned char mIOBuffer[IO_BUFFER_SIZE];
	int mIOBufferSize;
	TEXLoader* mTEXLoader;
};

void TEXLoader::InitSourceManager(jpeg_decompress_struct* pCInfo)
{
	if (pCInfo->src == NULL) 
	{	
		// First time for this JPEG object?
		pCInfo->src = (jpeg_source_mgr*)new SourceManager;
	}

	SourceManager* lSrc = (SourceManager*)pCInfo->src;
	lSrc->mSourceManager.init_source       = TEXInitSource;
	lSrc->mSourceManager.fill_input_buffer = TEXFillInputBuffer;
	lSrc->mSourceManager.skip_input_data   = TEXSkipInputData;
	lSrc->mSourceManager.resync_to_restart = jpeg_resync_to_restart; /* use default method */
	lSrc->mSourceManager.term_source       = TEXTerminateSource;

	// Forces FillInputBuffer() on first read.
	lSrc->mIOBufferSize = 0;
	lSrc->mTEXLoader = this;
	lSrc->mSourceManager.bytes_in_buffer = 0; 
	lSrc->mSourceManager.next_input_byte = NULL;
}

void TEXLoader::InitDestinationManager(jpeg_compress_struct* pCInfo)
{
	if (pCInfo->dest == NULL)
	{	
		// First time for this JPEG object?
		pCInfo->dest = (jpeg_destination_mgr*)new DestinationManager;
	}

	DestinationManager* lDest = (DestinationManager*)pCInfo->dest;

	lDest->mIOBufferSize = 0;
	lDest->mTEXLoader = this;

	lDest->mDestManager.init_destination    = TEXInitDestination;
	lDest->mDestManager.empty_output_buffer = TEXEmptyOutputBuffer;
	lDest->mDestManager.term_destination    = TEXTerminateDestination;
}

/*
	And here are the C style functions.
*/



void TEXInitSource(j_decompress_ptr pCInfo)
{
	SourceManager* lSrc = (SourceManager*)pCInfo->src;
	lSrc->mIOBufferSize = 0;
}

boolean TEXFillInputBuffer(j_decompress_ptr pCInfo)
{
	SourceManager* lSrc = (SourceManager*)pCInfo->src;

	File* lFile = TEXFriend::GetLoadFile(lSrc->mTEXLoader);

	uint64 lPrevPos = lFile->Tell();
	/* TODO: use assigned variable!
	IOError lErr = */ lFile->ReadData(lSrc->mIOBuffer, IO_BUFFER_SIZE);
	uint64 lNewPos = lFile->Tell();
	int lNumBytes = (int)(lNewPos - lPrevPos);

	if (lNumBytes == 0)
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

void TEXSkipInputData(j_decompress_ptr pCInfo, long pNumBytes)
{
	SourceManager* lSrc = (SourceManager*)pCInfo->src;

	if (pNumBytes > 0)
	{
		while (pNumBytes > (long)lSrc->mSourceManager.bytes_in_buffer) 
		{
			pNumBytes -= (long)lSrc->mSourceManager.bytes_in_buffer;
			TEXFillInputBuffer(pCInfo);
		}

		lSrc->mSourceManager.next_input_byte += (size_t)pNumBytes;
		lSrc->mSourceManager.bytes_in_buffer -= (size_t)pNumBytes;
	}
}

void TEXTerminateSource(j_decompress_ptr pCInfo)
{
	SourceManager* lSrc = (SourceManager*)pCInfo->src;
	lSrc->mIOBufferSize = 0;
	lSrc->mTEXLoader = 0;
	delete lSrc;
}






void TEXInitDestination(j_compress_ptr pCInfo)
{
	DestinationManager* lDest = (DestinationManager*)pCInfo->dest;

	lDest->mDestManager.next_output_byte = lDest->mIOBuffer;
	lDest->mDestManager.free_in_buffer = IO_BUFFER_SIZE;
}


boolean TEXEmptyOutputBuffer(j_compress_ptr pCInfo)
{
	DestinationManager* lDest = (DestinationManager*)pCInfo->dest;

	TEXFriend::GetSaveFile(lDest->mTEXLoader)->WriteData(lDest->mIOBuffer, IO_BUFFER_SIZE);

	lDest->mDestManager.next_output_byte = lDest->mIOBuffer;
	lDest->mDestManager.free_in_buffer = IO_BUFFER_SIZE;

	return TRUE;
}

void TEXTerminateDestination(j_compress_ptr pCInfo)
{
	DestinationManager* lDest = (DestinationManager*)pCInfo->dest;
	size_t lDataCount = IO_BUFFER_SIZE - lDest->mDestManager.free_in_buffer;

	/* Write any data remaining in the buffer */
	if (lDataCount > 0) 
	{
		TEXFriend::GetSaveFile(lDest->mTEXLoader)->WriteData(lDest->mIOBuffer, (unsigned)lDataCount);
	}

	lDest->mIOBufferSize = 0;
	lDest->mTEXLoader = 0;
	delete lDest;
}

TEXLoader::IoStatus TEXLoader::ReadJpeg(Canvas& pCanvas)
{
	unsigned lSize;
	mLoadFile->Read(lSize);

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
		// Create grayscale palette.
		Color lPalette[256];
		for (int i = 0; i < 256; i++)
		{
			lPalette[i].Set(i, i, i, i);
		}
		pCanvas.SetPalette(lPalette);
	}

	uint8* lBuffer = (uint8*)pCanvas.GetBuffer();
	int lRowStride = pCanvas.GetPitch() * pCanvas.GetPixelByteSize();
	int lScanLines = pCanvas.GetHeight();

	JSAMPROW lOffset[1];
	lOffset[0] = lBuffer;

	for (int i = 0; i < lScanLines; i++)
	{
		jpeg_read_scanlines(&lCInfo, lOffset, 1);
		lOffset[0] += lRowStride;
	}

	jpeg_finish_decompress(&lCInfo);
	jpeg_destroy_decompress(&lCInfo);

	mLoadFile->SeekCur(lSize);

	return STATUS_SUCCESS;
}

TEXLoader::IoStatus TEXLoader::WriteJpeg(const Canvas& pCanvas)
{
	int lSizeOffset = (int)mSaveFile->Tell();
	unsigned lSize = 0;
	mSaveFile->Write(lSize);	// We will update this data after writing the jpeg image.
	int lStartOffset = (int)mSaveFile->Tell();

	jpeg_compress_struct lCInfo;
	jpeg_error_mgr lJErr;
	lCInfo.err = jpeg_std_error(&lJErr);
	jpeg_create_compress(&lCInfo);

	InitDestinationManager(&lCInfo);

	lCInfo.image_width = pCanvas.GetWidth();
	lCInfo.image_height = pCanvas.GetHeight();

	if (pCanvas.GetBitDepth() == Canvas::BITDEPTH_8_BIT)
	{
		lCInfo.input_components = 1;
		lCInfo.in_color_space = JCS_GRAYSCALE;
	}
	else
	{
		lCInfo.input_components = 3;
		lCInfo.in_color_space = JCS_RGB;
	}

	// If the image is in 8-bit mode, it is considered to be a grayscale image.
	// Otherwise, we must make sure that the image is in 24-bit RGB mode.
	Canvas lCopy(pCanvas, true);
	if (lCopy.GetBitDepth() != Canvas::BITDEPTH_24_BIT &&
	   lCopy.GetBitDepth() != Canvas::BITDEPTH_8_BIT)
	{
		lCopy.ConvertBitDepth(Canvas::BITDEPTH_24_BIT);
	}

	jpeg_set_defaults(&lCInfo);
	jpeg_start_compress(&lCInfo, TRUE);

	uint8* lBuffer = (uint8*)lCopy.GetBuffer();
	int lRowStride = lCopy.GetPitch() * lCopy.GetPixelByteSize();
	int lScanLines = lCopy.GetHeight();

	JSAMPROW lOffset[1];
	lOffset[0] = lBuffer;

	for (int i = 0; i < lScanLines; i++)
	{
		jpeg_write_scanlines(&lCInfo, lOffset, 1);
		lOffset[0] += lRowStride;
	}

	jpeg_finish_compress(&lCInfo);
	jpeg_destroy_compress(&lCInfo);

	// Calculate and write the size of this jpeg image.
	const int lEndOffset = (int)mSaveFile->Tell();
	lSize = (unsigned)(lEndOffset - lStartOffset);
	mSaveFile->SeekSet(lSizeOffset);
	mSaveFile->Write(lSize);

	// Go back to where we were...
	mSaveFile->SeekEnd(0);

	return STATUS_SUCCESS;
}

TEXLoader::IoStatus TEXLoader::Load(const str& pFileName, Texture& pTexture, bool pMergeColorAndAlpha)
{
	IoStatus lIoStatus = STATUS_SUCCESS;
	MetaFile lFile;
	mLoadFile = &lFile;

	if (lFile.Open(pFileName, MetaFile::READ_ONLY) == false)
	{
		lIoStatus = STATUS_OPEN_ERROR;
	}

	if (lIoStatus == STATUS_SUCCESS)
	{
		lIoStatus = Load(pTexture, pMergeColorAndAlpha);
	}

	return lIoStatus;
}

TEXLoader::IoStatus TEXLoader::Save(const str& pFileName, const Texture& pTexture, bool pCompressed)
{
	IoStatus lIoStatus = STATUS_SUCCESS;
	DiskFile lFile;
	mSaveFile = &lFile;

	if (lFile.Open(pFileName, DiskFile::MODE_WRITE) == false)
	{
		lIoStatus = STATUS_OPEN_ERROR;
	}

	if (lIoStatus == STATUS_SUCCESS)
	{
		lIoStatus = Save(pTexture, pCompressed);
	}

	return lIoStatus;
}

TEXLoader::IoStatus TEXLoader::Load(const str& pArchiveName, const str& pFileName, Texture& pTexture, bool pMergeColorAndAlpha)
{
	IoStatus lIoStatus = STATUS_SUCCESS;
	ArchiveFile lFile(pArchiveName);
	mLoadFile = &lFile;

	if (lFile.Open(pFileName, ArchiveFile::READ_ONLY) == false)
	{
		lIoStatus = STATUS_OPEN_ERROR;
	}

	if (lIoStatus == STATUS_SUCCESS)
	{
		lIoStatus = Load(pTexture, pMergeColorAndAlpha);
	}

	return lIoStatus;
}

TEXLoader::IoStatus TEXLoader::Save(const str& pArchiveName, const str& pFileName, const Texture& pTexture, bool pCompressed)
{
	IoStatus lIoStatus = STATUS_SUCCESS;
	ArchiveFile lFile(pArchiveName);
	mSaveFile = &lFile;

	if (lFile.Open(pFileName, ArchiveFile::WRITE_ONLY) == false)
	{
		lIoStatus = STATUS_OPEN_ERROR;
	}

	if (lIoStatus == STATUS_SUCCESS)
	{
		lIoStatus = Save(pTexture, pCompressed);
	}

	return lIoStatus;
}

TEXLoader::IoStatus TEXLoader::Load(Texture& pTexture, bool pMergeColorAndAlpha)
{
	FileHeader lFileHeader;
	if (lFileHeader.ReadHeader(mLoadFile) == false)
	{
		mLoadFile->Close();
		return STATUS_READ_HEADER_ERROR;
	}

	pTexture.ClearAll();

	if (lFileHeader.CheckMapFlag(CUBE_MAP) == false)
	{
		int i;
		int lNumLevels = lFileHeader.mNumMipMapLevels;

		// Create all buffers.
		pTexture.Prepare(true, 
				  lFileHeader.CheckMapFlag(ALPHA_MAP),
				  lFileHeader.CheckMapFlag(NORMAL_MAP),
				  lFileHeader.CheckMapFlag(SPECULAR_MAP),
				  false,
				  lFileHeader.mWidth,
				  lFileHeader.mHeight);

		// Jump to the color data.
		mLoadFile->SeekSet(lFileHeader.mDataOffset);

		// Load color map.
		for (i = 0; i < lNumLevels; i++)
		{
			// The last two mip map levels are stored raw.
			if (i < (lNumLevels - 2) && lFileHeader.mCompressionFlag == 1)
				ReadJpeg(*pTexture._GetColorMap(i));
			else
				mLoadFile->ReadData(pTexture._GetColorMap(i)->GetBuffer(), pTexture._GetColorMap(i)->GetBufferByteSize());
		}

		// Load alpha map.
		if (lFileHeader.CheckMapFlag(ALPHA_MAP) == true)
		{
			for (i = 0; i < lNumLevels; i++)
			{
				// The last two mip map levels are stored raw.
				if (i < (lNumLevels - 2) && lFileHeader.mCompressionFlag == 1)
					ReadJpeg(*pTexture._GetAlphaMap(i));
				else
					mLoadFile->ReadData(pTexture._GetAlphaMap(i)->GetBuffer(), pTexture._GetAlphaMap(i)->GetBufferByteSize());
			}

			if (pMergeColorAndAlpha == true)
			{
				for (i = 0; i < lNumLevels; i++)
				{
					pTexture._GetColorMap(i)->ConvertTo32BitWithAlpha(*pTexture._GetAlphaMap(i));
				}
			}
		}

		// Load normal map.
		if (lFileHeader.CheckMapFlag(NORMAL_MAP) == true)
		{
			for (i = 0; i < lNumLevels; i++)
			{
				// The last two mip map levels are stored raw.
				if (i < (lNumLevels - 2) && lFileHeader.mCompressionFlag == 1)
					ReadJpeg(*pTexture._GetNormalMap(i));
				else
					mLoadFile->ReadData(pTexture._GetNormalMap(i)->GetBuffer(), pTexture._GetNormalMap(i)->GetBufferByteSize());
			}
		}

		// Load specular map.
		if (lFileHeader.CheckMapFlag(SPECULAR_MAP) == true)
		{
			for (i = 0; i < lNumLevels; i++)
			{
				// The last two mip map levels are stored raw.
				if (i < (lNumLevels - 2) && lFileHeader.mCompressionFlag == 1)
					ReadJpeg(*pTexture._GetSpecularMap(i));
				else
					mLoadFile->ReadData(pTexture._GetSpecularMap(i)->GetBuffer(), pTexture._GetSpecularMap(i)->GetBufferByteSize());
			}
		}
	}
	else
	{
		int i;
		int lNumLevels = lFileHeader.mNumMipMapLevels;

		// Create all buffers.
		pTexture.Prepare(false, 
						  false,
						  false,
						  false,
						  true,
						  lFileHeader.mWidth,
						  lFileHeader.mHeight);

		// Jump to the color data.
		mLoadFile->SeekSet(lFileHeader.mDataOffset);

		// Load cube map.
		for (i = 0; i < lNumLevels; i++)
		{
			// The last two mip map levels are stored raw.
			if (i < (lNumLevels - 2) && lFileHeader.mCompressionFlag == 1)
				ReadJpeg(*pTexture._GetCubeMapPosX(i));
			else
				mLoadFile->ReadData(pTexture._GetCubeMapPosX(i)->GetBuffer(), pTexture._GetCubeMapPosX(i)->GetBufferByteSize());
		}

		for (i = 0; i < lNumLevels; i++)
		{
			// The last two mip map levels are stored raw.
			if (i < (lNumLevels - 2) && lFileHeader.mCompressionFlag == 1)
				ReadJpeg(*pTexture._GetCubeMapNegX(i));
			else
				mLoadFile->ReadData(pTexture._GetCubeMapNegX(i)->GetBuffer(), pTexture._GetCubeMapNegX(i)->GetBufferByteSize());
		}

		for (i = 0; i < lNumLevels; i++)
		{
			// The last two mip map levels are stored raw.
			if (i < (lNumLevels - 2) && lFileHeader.mCompressionFlag == 1)
				ReadJpeg(*pTexture._GetCubeMapPosY(i));
			else
				mLoadFile->ReadData(pTexture._GetCubeMapPosY(i)->GetBuffer(), pTexture._GetCubeMapPosY(i)->GetBufferByteSize());
		}

		for (i = 0; i < lNumLevels; i++)
		{
			// The last two mip map levels are stored raw.
			if (i < (lNumLevels - 2) && lFileHeader.mCompressionFlag == 1)
				ReadJpeg(*pTexture._GetCubeMapNegY(i));
			else
				mLoadFile->ReadData(pTexture._GetCubeMapNegY(i)->GetBuffer(), pTexture._GetCubeMapNegY(i)->GetBufferByteSize());
		}

		for (i = 0; i < lNumLevels; i++)
		{
			// The last two mip map levels are stored raw.
			if (i < (lNumLevels - 2) && lFileHeader.mCompressionFlag == 1)
				ReadJpeg(*pTexture._GetCubeMapPosZ(i));
			else
				mLoadFile->ReadData(pTexture._GetCubeMapPosZ(i)->GetBuffer(), pTexture._GetCubeMapPosZ(i)->GetBufferByteSize());
		}

		for (i = 0; i < lNumLevels; i++)
		{
			// The last two mip map levels are stored raw.
			if (i < (lNumLevels - 2) && lFileHeader.mCompressionFlag == 1)
				ReadJpeg(*pTexture._GetCubeMapNegZ(i));
			else
				mLoadFile->ReadData(pTexture._GetCubeMapNegZ(i)->GetBuffer(), pTexture._GetCubeMapNegZ(i)->GetBufferByteSize());
		}
	}

	mLoadFile->Close();

	return STATUS_SUCCESS;
}

TEXLoader::IoStatus TEXLoader::Save(const Texture& pTexture, bool pCompressed)
{
	if (pTexture.GetColorMap(0) == 0)
	{
		return STATUS_MISSING_COLORMAP_ERROR;
	}

	// Calculate log2 of width and height.
	int lWidth = 0;
	int lHeight = 0;
	int lLog2Width = 0;
	int lLog2Height = 0;

	if (pTexture.IsCubeMap() == false)
	{
		lWidth  = pTexture.GetColorMap(0)->GetWidth();
		lHeight = pTexture.GetColorMap(0)->GetHeight();
	}
	else
	{
		lWidth  = pTexture.GetCubeMapPosX(0)->GetWidth();
		lHeight = pTexture.GetCubeMapPosX(0)->GetHeight();
	}

	while ((1 << lLog2Width) < lWidth)
	{
		lLog2Width++;
	}
	while ((1 << lLog2Height) < lHeight)
	{
		lLog2Height++;
	}

	// Prepare file header.
	FileHeader lFileHeader;
	lFileHeader.mTEXMagic[0] = 'T';
	lFileHeader.mTEXMagic[1] = 'T';
	lFileHeader.mTEXMagic[2] = 'E';
	lFileHeader.mTEXMagic[3] = 'X';

	lFileHeader.mVersion = 1;
	lFileHeader.mDataOffset = 16; // Size of file header.
	lFileHeader.mDimensionPowers = (uint8)((lLog2Height << 4) | lLog2Width);

	lFileHeader.mCompressionFlag = pCompressed == true ? 1 : 0;
	lFileHeader.mMapFlags = 0;

	if (pTexture.IsCubeMap() == false)
	{
		if (pTexture.GetAlphaMap(0) != 0 || 
		   pTexture.GetColorMap(0)->GetBitDepth() == Canvas::BITDEPTH_32_BIT)
			lFileHeader.mMapFlags |= ALPHA_MAP;
		if (pTexture.GetNormalMap(0) != 0)
			lFileHeader.mMapFlags |= NORMAL_MAP;
		if (pTexture.GetSpecularMap(0) != 0)
			lFileHeader.mMapFlags |= SPECULAR_MAP;
	}
	else
	{
		lFileHeader.mMapFlags = CUBE_MAP;
	}

	// Write the file header.
	mSaveFile->WriteData(lFileHeader.mTEXMagic, 4);
	mSaveFile->Write(lFileHeader.mVersion);
	mSaveFile->Write(lFileHeader.mDataOffset);
	mSaveFile->Write(lFileHeader.mDimensionPowers);
	mSaveFile->Write(lFileHeader.mCompressionFlag);
	mSaveFile->Write(lFileHeader.mMapFlags);

	int lNumLevels = pTexture.GetNumMipMapLevels();

	if (pTexture.IsCubeMap() == false)
	{
		int i;
		for (i = 0; i < lNumLevels; i++)
		{
			if (pTexture.GetColorMap(i)->GetBitDepth() != Canvas::BITDEPTH_24_BIT)
			{
				Canvas lTemp(*pTexture.GetColorMap(i), true);
				lTemp.ConvertBitDepth(Canvas::BITDEPTH_24_BIT);

				if (i < (lNumLevels - 2) && pCompressed == true)
					WriteJpeg(lTemp);
				else
					mSaveFile->WriteData(lTemp.GetBuffer(), lTemp.GetBufferByteSize());
			}
			else
			{
				if (i < (lNumLevels - 2) && pCompressed == true)
					WriteJpeg(*pTexture.GetColorMap(i));
				else
					mSaveFile->WriteData(pTexture.GetColorMap(i)->GetBuffer(), pTexture.GetColorMap(i)->GetBufferByteSize());
			}
		}

		if (pTexture.GetAlphaMap(0) != 0)
		{
			for (int i = 0; i < pTexture.GetNumMipMapLevels(); i++)
			{
				if (i < (lNumLevels - 2) && pCompressed == true)
					WriteJpeg(*pTexture.GetAlphaMap(i));
				else
					mSaveFile->WriteData(pTexture.GetAlphaMap(i)->GetBuffer(), pTexture.GetAlphaMap(i)->GetBufferByteSize());
			}
		}
		else if(pTexture.GetColorMap(0)->GetBitDepth() == Canvas::BITDEPTH_32_BIT)
		{
			for (i = 0; i < pTexture.GetNumMipMapLevels(); i++)
			{
				Canvas lAlphaMap;
				pTexture.GetColorMap(i)->GetAlphaChannel(lAlphaMap);

				if (i < (lNumLevels - 2) && pCompressed == true)
					WriteJpeg(lAlphaMap);
				else
					mSaveFile->WriteData(lAlphaMap.GetBuffer(), lAlphaMap.GetBufferByteSize());
			}
		}

		if (pTexture.GetNormalMap(0) != 0)
		{
			for (i = 0; i < pTexture.GetNumMipMapLevels(); i++)
			{
				if (i < (lNumLevels - 2) && pCompressed == true)
					WriteJpeg(*pTexture.GetNormalMap(i));
				else
					mSaveFile->WriteData(pTexture.GetNormalMap(i)->GetBuffer(), pTexture.GetNormalMap(i)->GetBufferByteSize());
			}
		}

		if (pTexture.GetSpecularMap(0) != 0)
		{
			for (i = 0; i < pTexture.GetNumMipMapLevels(); i++)
			{
				if (i < (lNumLevels - 2) && pCompressed == true)
					WriteJpeg(*pTexture.GetSpecularMap(i));
				else
					mSaveFile->WriteData(pTexture.GetSpecularMap(i)->GetBuffer(), pTexture.GetSpecularMap(i)->GetBufferByteSize());
			}
		}
	}
	else
	{
		int i;
		for (i = 0; i < pTexture.GetNumMipMapLevels(); i++)
		{
			if (i < (lNumLevels - 2) && pCompressed == true)
				WriteJpeg(*pTexture.GetCubeMapPosX(i));
			else
				mSaveFile->WriteData(pTexture.GetCubeMapPosX(i)->GetBuffer(), pTexture.GetCubeMapPosX(i)->GetBufferByteSize());
		}

		for (i = 0; i < pTexture.GetNumMipMapLevels(); i++)
		{
			if (i < (lNumLevels - 2) && pCompressed == true)
				WriteJpeg(*pTexture.GetCubeMapNegX(i));
			else
				mSaveFile->WriteData(pTexture.GetCubeMapNegX(i)->GetBuffer(), pTexture.GetCubeMapNegX(i)->GetBufferByteSize());
		}

		for (i = 0; i < pTexture.GetNumMipMapLevels(); i++)
		{
			if (i < (lNumLevels - 2) && pCompressed == true)
				WriteJpeg(*pTexture.GetCubeMapPosY(i));
			else
				mSaveFile->WriteData(pTexture.GetCubeMapPosY(i)->GetBuffer(), pTexture.GetCubeMapPosY(i)->GetBufferByteSize());
		}

		for (i = 0; i < pTexture.GetNumMipMapLevels(); i++)
		{
			if (i < (lNumLevels - 2) && pCompressed == true)
				WriteJpeg(*pTexture.GetCubeMapNegY(i));
			else
				mSaveFile->WriteData(pTexture.GetCubeMapNegY(i)->GetBuffer(), pTexture.GetCubeMapNegY(i)->GetBufferByteSize());
		}

		for (i = 0; i < pTexture.GetNumMipMapLevels(); i++)
		{
			if (i < (lNumLevels - 2) && pCompressed == true)
				WriteJpeg(*pTexture.GetCubeMapPosZ(i));
			else
				mSaveFile->WriteData(pTexture.GetCubeMapPosZ(i)->GetBuffer(), pTexture.GetCubeMapPosZ(i)->GetBufferByteSize());
		}

		for (i = 0; i < pTexture.GetNumMipMapLevels(); i++)
		{
			if (i < (lNumLevels - 2) && pCompressed == true)
				WriteJpeg(*pTexture.GetCubeMapNegZ(i));
			else
				mSaveFile->WriteData(pTexture.GetCubeMapNegZ(i)->GetBuffer(), pTexture.GetCubeMapNegZ(i)->GetBufferByteSize());
		}
	}

	mSaveFile->Close();

	return STATUS_SUCCESS;
}



}
