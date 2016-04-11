/*
	Class:  PngLoader
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#include "pch.h"
#include "../Include/PngLoader.h"
#include "../Include/Graphics2D.h"
#include "../Include/Canvas.h"
#include "../Include/MetaFile.h"
#include "../Include/ArchiveFile.h"

#ifdef LEPRA_MSVC
// Disable warning about functions that include catch which may not support C++ semantics. PNG loader
// requires setjmp function, which has this downside.
#pragma warning(disable: 4611)
#endif // LEPRA_MSVC

// The png_jmpbuf() macro, used in error handling, became available in
// libpng version 1.0.6.  If you want to be able to run your code with older
// versions of libpng, you must define the macro yourself (but only if it
// is not already defined by libpng!).
#ifndef png_jmpbuf
#define png_jmpbuf(png_ptr) ((png_ptr)->jmpbuf)
#endif



namespace Lepra
{



PngLoader::Status PngLoader::Load(const str& pFileName, Canvas& pCanvas)
{
	Status lStatus = STATUS_SUCCESS;
	MetaFile lFile;

	if (lFile.Open(pFileName, MetaFile::READ_ONLY) == false)
	{
		lStatus = STATUS_OPEN_ERROR;
	}

	if (lStatus == STATUS_SUCCESS)
	{
		lStatus = Load(lFile, pCanvas);
		lFile.Close();
	}

	return lStatus;
}

PngLoader::Status PngLoader::Save(const str& pFileName, const Canvas& pCanvas)
{
	Status lStatus = STATUS_SUCCESS;
	DiskFile lFile;

	if (lFile.Open(pFileName, DiskFile::MODE_WRITE) == false)
	{
		lStatus = STATUS_OPEN_ERROR;
	}

	if (lStatus == STATUS_SUCCESS)
	{
		lStatus = Save(lFile, pCanvas);
		lFile.Close();
	}

	return lStatus;
}

PngLoader::Status PngLoader::Load(const str& pArchiveName, const str& pFileName, Canvas& pCanvas)
{
	Status lStatus = STATUS_SUCCESS;
	ArchiveFile lFile(pArchiveName);

	if (lFile.Open(pFileName, ArchiveFile::READ_ONLY) == false)
	{
		lStatus = STATUS_OPEN_ERROR;
	}

	if (lStatus == STATUS_SUCCESS)
	{
		lStatus = Load(lFile, pCanvas);
		lFile.Close();
	}

	return lStatus;
}

PngLoader::Status PngLoader::Save(const str& pArchiveName, const str& pFileName, const Canvas& pCanvas)
{
	Status lStatus = STATUS_SUCCESS;
	ArchiveFile lFile(pArchiveName);

	if (lFile.Open(pFileName, ArchiveFile::WRITE_ONLY) == false)
	{
		lStatus = STATUS_OPEN_ERROR;
	}

	if (lStatus == STATUS_SUCCESS)
	{
		lStatus = Save(lFile, pCanvas);
		lFile.Close();
	}

	return lStatus;
}

PngLoader::Status PngLoader::Load(Reader& pReader, Canvas& pCanvas)
{
	mReader = &pReader;

	if (CheckIfPNG() == false)
	{
		mLog.AWarning("PNG header error!");
		return STATUS_READ_HEADER_ERROR;
	}

	png_structp lPNG = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
	if (lPNG == 0)
	{
		mLog.AError("PNG reader runs out of memory!");
		return STATUS_MEMORY_ERROR;
	}

	png_infop lInfo = png_create_info_struct(lPNG);
	if (lInfo == 0)
	{
		mLog.AError("PNG reader runs out of memory!");
		png_destroy_read_struct(&lPNG, png_infopp_NULL, png_infopp_NULL);
		return STATUS_MEMORY_ERROR;
	}

	if (setjmp(png_jmpbuf(lPNG)))
	{
		mLog.AError("PNG reader runs out of memory!");
		png_destroy_read_struct(&lPNG, &lInfo, png_infopp_NULL);
		return STATUS_MEMORY_ERROR;
	}

	png_set_read_fn(lPNG, (void*)this, ReadDataCallback);
	png_set_sig_bytes(lPNG, 8);
	png_read_png(lPNG, lInfo, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING, 0);

	const int lPitch = pCanvas.GetPitch();
	const int lPixelByteSize = pCanvas.GetPixelByteSize();
	const int lWidth = pCanvas.GetWidth();
	const int lHeight = pCanvas.GetHeight();
	int i;
	switch(png_get_color_type(lPNG, lInfo))
	{
	case PNG_COLOR_TYPE_GRAY:
		{
			pCanvas.Reset(png_get_image_width(lPNG, lInfo), png_get_image_height(lPNG, lInfo), Canvas::BITDEPTH_8_BIT);
			pCanvas.CreateBuffer();
			uint8* lBuffer = (uint8*)pCanvas.GetBuffer();

			Color lPalette[256];
			for (i = 0; i < 256; i++)
			{
				lPalette[i].mRed   = (uint8)i;
				lPalette[i].mGreen = (uint8)i;
				lPalette[i].mBlue  = (uint8)i;
				lPalette[i].mAlpha = 255;
			}

			pCanvas.SetPalette(lPalette);

			uint8** lRow = png_get_rows(lPNG, lInfo);
			for (i = 0; i < (int)lHeight; i++)
			{
				memcpy(&lBuffer[i * lPitch * lPixelByteSize], lRow[i], lWidth * lPixelByteSize);
			}
		}
		break;
	case PNG_COLOR_TYPE_GRAY_ALPHA:
		{
			pCanvas.Reset(png_get_image_width(lPNG, lInfo), png_get_image_height(lPNG, lInfo), Canvas::BITDEPTH_32_BIT);
			pCanvas.CreateBuffer();
			uint8* lBuffer = (uint8*)pCanvas.GetBuffer();

			Color lPalette[256];
			for (i = 0; i < 256; i++)
			{
				lPalette[i].mRed   = (uint8)i;
				lPalette[i].mGreen = (uint8)i;
				lPalette[i].mBlue  = (uint8)i;
				lPalette[i].mAlpha = 255;
			}

			pCanvas.SetPalette(lPalette);

			uint8** lRow = png_get_rows(lPNG, lInfo);
			for (int y = 0; y < (int)lHeight; y++)
			{
				uint8* lDstRow = &lBuffer[y * lPitch * lPixelByteSize];
				for (int x = 0; x < (int)lWidth; x++)
				{
					int lIndex = x * lPixelByteSize;
					lDstRow[lIndex + 0] = lRow[y][x * 2 + 0];
					lDstRow[lIndex + 1] = lRow[y][x * 2 + 0];
					lDstRow[lIndex + 2] = lRow[y][x * 2 + 0];
					lDstRow[lIndex + 3] = lRow[y][x * 2 + 1];
				}
			}
		}
		break;
	case PNG_COLOR_TYPE_PALETTE:
		{
			pCanvas.Reset(png_get_image_width(lPNG, lInfo), png_get_image_height(lPNG, lInfo), Canvas::BITDEPTH_8_BIT);
			pCanvas.CreateBuffer();
			uint8* lBuffer = (uint8*)pCanvas.GetBuffer();

			int i;
			int lNumEntries;
			png_color* lPngPalette;
			png_get_PLTE(lPNG, lInfo, &lPngPalette, &lNumEntries);
			Color lPalette[256];
			for (i = 0; i < lNumEntries; i++)
			{
				lPalette[i].mRed   = lPngPalette[i].red;
				lPalette[i].mGreen = lPngPalette[i].green;
				lPalette[i].mBlue  = lPngPalette[i].blue;
				lPalette[i].mAlpha = 255;
			}

			png_bytep lTrans;
			png_color_16p lTransValues;
			png_get_tRNS(lPNG, lInfo, &lTrans, &lNumEntries, &lTransValues);
			for (i = 0; i < lNumEntries; i++)
			{
				// Is lTransValues[i].index == lTrans[i]?
				lPalette[lTransValues[i].index].mAlpha = 0;
			}

			pCanvas.SetPalette(lPalette);

			uint8** lRow = png_get_rows(lPNG, lInfo);
			for (int i = 0; i < (int)lHeight; i++)
			{
				memcpy(&lBuffer[i * lPitch * lPixelByteSize], lRow[i], lWidth * lPixelByteSize);
			}
		}
		break;
	case PNG_COLOR_TYPE_RGB:
		{
			pCanvas.Reset(png_get_image_width(lPNG, lInfo), png_get_image_height(lPNG, lInfo), Canvas::BITDEPTH_24_BIT);
			pCanvas.CreateBuffer();
			uint8* lBuffer = (uint8*)pCanvas.GetBuffer();
			uint8** lRow = png_get_rows(lPNG, lInfo);
			for (i = 0; i < (int)lHeight; i++)
			{
				memcpy(&lBuffer[i * lPitch * lPixelByteSize], lRow[i], lWidth * lPixelByteSize);
			}
		}
		break;
	case PNG_COLOR_TYPE_RGB_ALPHA:
		{
			pCanvas.Reset(png_get_image_width(lPNG, lInfo), png_get_image_height(lPNG, lInfo), Canvas::BITDEPTH_32_BIT);
			pCanvas.CreateBuffer();
			uint8* lBuffer = (uint8*)pCanvas.GetBuffer();
			uint8** lRow = png_get_rows(lPNG, lInfo);
			for (i = 0; i < (int)lHeight; i++)
			{
				memcpy(&lBuffer[i * lPitch * lPixelByteSize], lRow[i], lWidth * lPixelByteSize);
			}
		}
		break;
	default:
		mLog.AError("PNG is of unknown type!");
		png_destroy_read_struct(&lPNG, &lInfo, 0);
		return STATUS_READ_INFO_ERROR;
	}

	png_destroy_read_struct(&lPNG, &lInfo, 0);

	pCanvas.SwapRGBOrder();

	return STATUS_SUCCESS;
}

PngLoader::Status PngLoader::Save(Writer& pWriter, const Canvas& pCanvas)
{
	mWriter = &pWriter;

	png_structp lPNG = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);

	if (lPNG == 0)
	{
		return STATUS_MEMORY_ERROR;
	}

	png_infop lInfo = png_create_info_struct(lPNG);

	if (lInfo == 0)
	{
		png_destroy_write_struct(&lPNG, 0);
		return STATUS_MEMORY_ERROR;
	}

	if (setjmp(png_jmpbuf(lPNG)))
	{
		png_destroy_write_struct(&lPNG, &lInfo);
		return STATUS_MEMORY_ERROR;
	}

	png_set_write_fn(lPNG, (void*)this, WriteDataCallback, FlushCallback);

	// Copy the image and convert it to an appropriate bit depth.
	Canvas lImage(pCanvas, true);
	int lColorType = 0;
	int lBitDepth = 0;
	switch(lImage.GetBitDepth())
	{
	case Canvas::BITDEPTH_8_BIT:
		lColorType = PNG_COLOR_TYPE_PALETTE;
		lBitDepth = 8;
		break;
	case Canvas::BITDEPTH_15_BIT:
	case Canvas::BITDEPTH_16_BIT:
		lImage.ConvertBitDepth(Canvas::BITDEPTH_24_BIT);
	case Canvas::BITDEPTH_24_BIT:
		lColorType = PNG_COLOR_TYPE_RGB;
		lBitDepth = 8;
		break;
	case Canvas::BITDEPTH_32_BIT:
		lColorType = PNG_COLOR_TYPE_RGBA;
		lBitDepth = 8;
		break;
	case Canvas::BITDEPTH_32_BIT_PER_CHANNEL:
		lImage.ConvertBitDepth(Canvas::BITDEPTH_16_BIT_PER_CHANNEL);
	case Canvas::BITDEPTH_16_BIT_PER_CHANNEL:
		lColorType = PNG_COLOR_TYPE_RGB;
		lBitDepth = 16;
		break;
	}

	// Set image header.
	png_set_IHDR(lPNG, lInfo, 
				 lImage.GetWidth(), lImage.GetHeight(), 
				 lBitDepth,
				 lColorType,
				 PNG_INTERLACE_NONE,
				 PNG_COMPRESSION_TYPE_BASE, 
				 PNG_FILTER_TYPE_BASE);

	// Set palette if needed.
	png_colorp lPalette = 0;
	if (lImage.GetBitDepth() == Canvas::BITDEPTH_8_BIT)
	{
		lPalette = (png_colorp)png_malloc(lPNG, PNG_MAX_PALETTE_LENGTH * png_sizeof(png_color));

		const Color* lSrcPlt = lImage.GetPalette();
		int i;
		for (i = 0; i < 256 && i < PNG_MAX_PALETTE_LENGTH; i++)
		{
		   lPalette[i].red   = lSrcPlt[i].mRed;
		   lPalette[i].green = lSrcPlt[i].mGreen;
		   lPalette[i].blue  = lSrcPlt[i].mBlue;
		}

		png_set_PLTE(lPNG, lInfo, lPalette, PNG_MAX_PALETTE_LENGTH);
	}

	png_write_info(lPNG, lInfo);

	if (Endian::GetSystemEndian() == Endian::TYPE_LITTLE_ENDIAN)
	{
		// The byte order must be big endian.
		png_set_swap(lPNG);
	}

	int i;
	png_bytep* lRow = new png_bytep[lImage.GetHeight()];
	for (i = 0; i < (int)lImage.GetHeight(); i++)
	{
		lRow[i] = &((png_bytep)lImage.GetBuffer())[i * lImage.GetPitch() * lImage.GetPixelByteSize()];
	}

	png_write_image(lPNG, lRow);
	png_write_end(lPNG, lInfo);

	delete[] lRow;

	if (lPalette != 0)
	{
		png_free(lPNG, lPalette);
	}

	png_destroy_write_struct(&lPNG, &lInfo);

	return STATUS_SUCCESS;
}

bool PngLoader::CheckIfPNG()
{
	uint8 lBuffer[8];
	if (mReader->ReadData(lBuffer, 8) != IO_OK)
	{
		return false;
	}

	return (png_sig_cmp(lBuffer, (png_size_t)0, 8) == 0);
}

void PngLoader::ReadDataCallback(png_structp pPNG, png_bytep pData, png_size_t pLength)
{
    PngLoader* lThis = (PngLoader*)png_get_io_ptr(pPNG);
	lThis->mReader->ReadData(pData, (unsigned)pLength);
}

void PngLoader::WriteDataCallback(png_structp pPNG, png_bytep pData, png_size_t pLength)
{
    PngLoader* lThis = (PngLoader*)png_get_io_ptr(pPNG);
	lThis->mWriter->WriteData(pData, (unsigned)pLength);
}

void PngLoader::FlushCallback(png_structp /*pPNG*/)
{
}



loginstance(UI_GFX_2D, PngLoader);



} // End namespace.
