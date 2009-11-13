
// Author: Alexander Hugestrand
// Copyright (c) 2002-2008, Righteous Games



#include "../Include/TgaLoader.h"
#include "../Include/Graphics2D.h"
#include "../Include/Canvas.h"
#include "../Include/DiskFile.h"
#include "../Include/MetaFile.h"
#include "../Include/MemFile.h"
#include "../Include/ArchiveFile.h"



namespace Lepra
{



TgaLoader::Status TgaLoader::Load(const String& pFileName, Canvas& pCanvas)
{
	Status lStatus = STATUS_SUCCESS;
	MetaFile lFile;
	if (lFile.Open(pFileName, MetaFile::READ_ONLY, false, Endian::TYPE_LITTLE_ENDIAN) == false)
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

TgaLoader::Status TgaLoader::Save(const String& pFileName, const Canvas& pCanvas)
{
	Status lStatus = STATUS_SUCCESS;
	DiskFile lFile;
	if (lFile.Open(pFileName, DiskFile::MODE_WRITE, false, Endian::TYPE_LITTLE_ENDIAN) == false)
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

TgaLoader::Status TgaLoader::Load(const String& pArchiveName, const String& pFileName, Canvas& pCanvas)
{
	Status lStatus = STATUS_SUCCESS;
	ArchiveFile lFile(pArchiveName);
	if (lFile.Open(pFileName, ArchiveFile::READ_ONLY, Endian::TYPE_LITTLE_ENDIAN) == false)
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

TgaLoader::Status TgaLoader::Save(const String& pArchiveName, const String& pFileName, const Canvas& pCanvas)
{
	Status lStatus = STATUS_SUCCESS;
	ArchiveFile lFile(pArchiveName);
	if (lFile.Open(pFileName, ArchiveFile::WRITE_ONLY, Endian::TYPE_LITTLE_ENDIAN) == false)
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

TgaLoader::Status TgaLoader::Load(Reader& pReader, Canvas& pCanvas)
{
	int64 lFileSize = pReader.GetAvailable();
	if (lFileSize <= 0)
	{
		return STATUS_UNKNOWNFILESIZE_ERROR;
	}

	enum
	{
		BUFFER_SIZE = 512,
	};

	//
	// Read the entire file into memory.
	//
	MemFile lMemFile;

	uint8* lBuffer[BUFFER_SIZE];
	unsigned lNumChunks = (unsigned)lFileSize / BUFFER_SIZE;
	unsigned lRest = (unsigned)lFileSize % BUFFER_SIZE;
	unsigned i;

	for (i = 0; i < lNumChunks; i++)
	{
		if (pReader.ReadData(lBuffer, BUFFER_SIZE) != IO_OK)
			return STATUS_READSTREAM_ERROR;
		lMemFile.WriteData(lBuffer, BUFFER_SIZE);
	}

	if (lRest > 0)
	{
		if (pReader.ReadData(lBuffer, lRest) != IO_OK)
			return STATUS_READSTREAM_ERROR;
		lMemFile.WriteData(lBuffer, lRest);
	}

	// Now actually load the file.
	return Load(lMemFile, pCanvas);
}

TgaLoader::Status TgaLoader::Load(File& pFile, Canvas& pCanvas)
{
	// Determine the file format by reading the file footer.
	FileFooter lFooter;
	pFile.SeekEnd(-26);
	pFile.Read(lFooter.mExtensionAreaOffset);
	pFile.Read(lFooter.mDeveloperDirectoryOffset);
	pFile.ReadData(lFooter.mSignature, 18);
	pFile.SeekSet(0);

	TGAFormat lFormat = ORIGINAL_TGA_FORMAT;
	if (strcmp(lFooter.mSignature, smTruevisionXFile) == 0)
	{
		lFormat = NEW_TGA_FORMAT;
	}

	// Read the file header.
	TGAFileHeader lFileHeader;
	pFile.Read(lFileHeader.mIDLength);
	pFile.Read(lFileHeader.mColorMapType);
	pFile.Read(lFileHeader.mImageType);
	pFile.Read(lFileHeader.mColorMapSpec.mFirstEntryIndex);
	pFile.Read(lFileHeader.mColorMapSpec.mColorMapLength);
	pFile.Read(lFileHeader.mColorMapSpec.mColorMapEntrySize);
	pFile.Read(lFileHeader.mImageSpec.mXOrigin);
	pFile.Read(lFileHeader.mImageSpec.mYOrigin);
	pFile.Read(lFileHeader.mImageSpec.mImageWidth);
	pFile.Read(lFileHeader.mImageSpec.mImageHeight);
	pFile.Read(lFileHeader.mImageSpec.mPixelDepth);
	pFile.Read(lFileHeader.mImageSpec.mImageDescriptor);

	if (lFileHeader.mIDLength != 0)
	{
		// Skip image ID. This is just identification info about the image.
		pFile.SeekCur((int64)lFileHeader.mIDLength);
	}

	// Read the palette, if any.
	Color* lPalette = 0;
	if (lFileHeader.mColorMapType != 0)
	{
		// Don't allow less than 256 entries in the palette.
		int lPaletteEntries = lFileHeader.mColorMapSpec.mColorMapLength >= 256 ? lFileHeader.mColorMapSpec.mColorMapLength : 256;
		lPalette = new Color[lPaletteEntries];

		switch(lFileHeader.mColorMapSpec.mColorMapEntrySize)
		{
		case 15:
		case 16:	// If 16-bit, it's still stored as 15-bit, according to the manual...
			{
				for (int i = lFileHeader.mColorMapSpec.mFirstEntryIndex;
					i < lFileHeader.mColorMapSpec.mColorMapLength;
					i++)
				{
					uint16 lColor;
					pFile.Read(lColor);

					lPalette[i].mRed   = (uint8)(((lColor >> 10) & 31) << 3);
					lPalette[i].mGreen = (uint8)(((lColor >> 5) & 31) << 3);
					lPalette[i].mBlue  = (uint8)((lColor & 31) << 3);
				}
			}
			break;
		case 24:
			{
				for (int i = lFileHeader.mColorMapSpec.mFirstEntryIndex;
					i < lFileHeader.mColorMapSpec.mColorMapLength;
					i++)
				{
					pFile.Read(lPalette[i].mBlue);
					pFile.Read(lPalette[i].mGreen);
					pFile.Read(lPalette[i].mRed);
				}
			}
			break;
		case 32:
			{
				for (int i = lFileHeader.mColorMapSpec.mFirstEntryIndex;
					i < lFileHeader.mColorMapSpec.mColorMapLength;
					i++)
				{
					pFile.Read(lPalette[i].mBlue);
					pFile.Read(lPalette[i].mGreen);
					pFile.Read(lPalette[i].mRed);
					pFile.Read(lPalette[i].mAlpha);
				}
			}
			break;
		default:
			delete[] lPalette;
			return STATUS_READ_PALETTE_ERROR;
			break;
		};
	}

	// And now it's time for the image data itself.

	Status lReturnValue = STATUS_READ_PICTURE_ERROR;

	switch(lFileHeader.mImageType)
	{
	case UNCOMPRESSED_COLORMAP_IMAGE:
		lReturnValue = LoadUncompressedColorMapImage(pCanvas, lFileHeader, pFile, lPalette);
		break;
	case UNCOMPRESSED_TRUECOLOR_IMAGE:
		lReturnValue = LoadUncompressedTrueColorImage(pCanvas, lFileHeader, pFile);
		break;
	case UNCOMPRESSED_BLACKANDWHITE_IMAGE:
		lReturnValue = LoadUncompressedBlackAndWhiteImage(pCanvas, lFileHeader, pFile);
		break;
	case RLE_COLORMAPPED_IMAGE:
		lReturnValue = LoadRLEColorMapImage(pCanvas, lFileHeader, pFile, lPalette);
		break;
	case RLE_TRUECOLOR_IMAGE:
		lReturnValue = LoadRLETrueColorImage(pCanvas, lFileHeader, pFile);
		break;
	case RLE_BLACKANDWHITE_IMAGE:
		lReturnValue = LoadRLEBlackAndWhiteImage(pCanvas, lFileHeader, pFile);
		break;
	};

	if (lPalette != 0)
	{
		delete[] lPalette;
	}

	return lReturnValue;
}

TgaLoader::Status TgaLoader::Save(Writer& pFile, const Canvas& pCanvas)
{
	// Write the file header.
	TGAFileHeader lFileHeader;
	lFileHeader.mIDLength = 0;
	lFileHeader.mColorMapType = (pCanvas.GetBitDepth() == Canvas::BITDEPTH_8_BIT) ? 1 : 0;
	lFileHeader.mImageType = (uint8)(pCanvas.GetBitDepth() == Canvas::BITDEPTH_8_BIT ? UNCOMPRESSED_COLORMAP_IMAGE : UNCOMPRESSED_TRUECOLOR_IMAGE);
	lFileHeader.mColorMapSpec.mFirstEntryIndex = 0;
	lFileHeader.mColorMapSpec.mColorMapLength = (pCanvas.GetBitDepth() == Canvas::BITDEPTH_8_BIT) ? 256 : 0;
	lFileHeader.mColorMapSpec.mColorMapEntrySize = (pCanvas.GetBitDepth() == Canvas::BITDEPTH_8_BIT) ? 24 : 0;
	lFileHeader.mImageSpec.mXOrigin = 0;
	lFileHeader.mImageSpec.mYOrigin = 0;
	lFileHeader.mImageSpec.mImageWidth = (uint16)pCanvas.GetWidth();
	lFileHeader.mImageSpec.mImageHeight = (uint16)pCanvas.GetHeight();
	lFileHeader.mImageSpec.mPixelDepth = (uint8)Canvas::BitDepthToInt(pCanvas.GetBitDepth());
	lFileHeader.mImageSpec.mImageDescriptor = 0;

	if (lFileHeader.mImageSpec.mPixelDepth == 15)
	{
		lFileHeader.mImageSpec.mPixelDepth = 16;
	}

	pFile.Write(lFileHeader.mIDLength);
	pFile.Write(lFileHeader.mColorMapType);
	pFile.Write(lFileHeader.mImageType);
	pFile.Write(lFileHeader.mColorMapSpec.mFirstEntryIndex);
	pFile.Write(lFileHeader.mColorMapSpec.mColorMapLength);
	pFile.Write(lFileHeader.mColorMapSpec.mColorMapEntrySize);
	pFile.Write(lFileHeader.mImageSpec.mXOrigin);
	pFile.Write(lFileHeader.mImageSpec.mYOrigin);
	pFile.Write(lFileHeader.mImageSpec.mImageWidth);
	pFile.Write(lFileHeader.mImageSpec.mImageHeight);
	pFile.Write(lFileHeader.mImageSpec.mPixelDepth);
	pFile.Write(lFileHeader.mImageSpec.mImageDescriptor);

	// Write the palette.
	if (pCanvas.GetBitDepth() == Canvas::BITDEPTH_8_BIT)
	{
		for (int i = 0; i < 256; i++)
		{
			pFile.Write(pCanvas.GetPalette()[i].mBlue);
			pFile.Write(pCanvas.GetPalette()[i].mGreen);
			pFile.Write(pCanvas.GetPalette()[i].mRed);
		}
	}

	if (pCanvas.GetBitDepth() == Canvas::BITDEPTH_16_BIT)
	{
		// We have to convert it to 15 bit (well, at least that's what Adobe Photoshop expects).
		Canvas lCanvas(pCanvas, true);
		lCanvas.ConvertBitDepth(Canvas::BITDEPTH_15_BIT);

		// Write the image data.
		for (int y = pCanvas.GetHeight() - 1; y >= 0; y--)
		{
			int lYOffset = y * lCanvas.GetPitch() * lCanvas.GetPixelByteSize();
			pFile.WriteData(((uint8*)lCanvas.GetBuffer()) + lYOffset, lCanvas.GetWidth() * lCanvas.GetPixelByteSize());
		}
	}
	else
	{
		// Write the image data.
		for (int y = pCanvas.GetHeight() - 1; y >= 0; y--)
		{
			int lYOffset = y * pCanvas.GetPitch() * pCanvas.GetPixelByteSize();
			pFile.WriteData(((uint8*)pCanvas.GetBuffer()) + lYOffset, pCanvas.GetWidth() * pCanvas.GetPixelByteSize());
		}
	}

	FileFooter lFooter;
	lFooter.mExtensionAreaOffset = 0;
	lFooter.mDeveloperDirectoryOffset = 0;

	pFile.Write(lFooter.mExtensionAreaOffset);
	pFile.Write(lFooter.mDeveloperDirectoryOffset);
	pFile.WriteData(smTruevisionXFile, (unsigned)::strlen(smTruevisionXFile) + 1);

	return STATUS_SUCCESS;
}

TgaLoader::Status TgaLoader::LoadUncompressedColorMapImage(Canvas& pCanvas, TGAFileHeader& pFileHeader, File& pFile, Color* pPalette)
{
	if (pPalette == 0)
	{
		return STATUS_READ_PALETTE_ERROR;
	}

	bool lLeftToRight = (pFileHeader.mImageSpec.mImageDescriptor & 16) == 0;
	bool lTopToBottom = (pFileHeader.mImageSpec.mImageDescriptor & 32) != 0;

	if (pFileHeader.mColorMapSpec.mColorMapLength <= 256)
	{
		pCanvas.Reset(pFileHeader.mImageSpec.mImageWidth,
					   pFileHeader.mImageSpec.mImageHeight,
					   Canvas::IntToBitDepth(pFileHeader.mImageSpec.mPixelDepth));
		pCanvas.SetPalette(pPalette);
	}
	else
	{
		// Reset image to same pixel depth as the palette, if there are more than 256 colors.
		pCanvas.Reset(pFileHeader.mImageSpec.mImageWidth,
					   pFileHeader.mImageSpec.mImageHeight,
					   Canvas::IntToBitDepth(pFileHeader.mColorMapSpec.mColorMapEntrySize));
	}

	pCanvas.CreateBuffer();

	switch(pFileHeader.mImageSpec.mPixelDepth)
	{
	case 8:
		{
			uint8* lData = (uint8*)pCanvas.GetBuffer();

			int y;
			for (y = 0; y < pFileHeader.mImageSpec.mImageHeight; y++)
			{
				int lYOffset = 0;
				if (lTopToBottom == true)
				{
					lYOffset = y * pFileHeader.mImageSpec.mImageWidth;
				}
				else
				{
					lYOffset = (pFileHeader.mImageSpec.mImageHeight - (y + 1)) * pFileHeader.mImageSpec.mImageWidth;
				}

				if (lLeftToRight == true)
				{
					pFile.ReadData(lData + lYOffset, pFileHeader.mImageSpec.mImageWidth);
				}
				else
				{
					for (int x = pFileHeader.mImageSpec.mImageWidth - 1; x >= 0; x--)
					{
						pFile.Read(lData[lYOffset + x]);
					}
				}
			}
		}
		break;
	case 15:
	case 16:
		{
			switch(pCanvas.GetBitDepth())
			{
			case Canvas::BITDEPTH_15_BIT:
			case Canvas::BITDEPTH_16_BIT:
			case Canvas::BITDEPTH_24_BIT:
			case Canvas::BITDEPTH_32_BIT:
				// OK...
				break;
			default:
				return STATUS_READ_PALETTE_ERROR;
			};
		
			uint16* lData = new uint16[pFileHeader.mImageSpec.mImageWidth * pFileHeader.mImageSpec.mImageHeight];

			int y;
			for (y = 0; y < pFileHeader.mImageSpec.mImageHeight; y++)
			{
				int lYOffset = 0;
				if (lTopToBottom == true)
				{
					lYOffset = y * pFileHeader.mImageSpec.mImageWidth;
				}
				else
				{
					lYOffset = (pFileHeader.mImageSpec.mImageHeight - (y + 1)) * pFileHeader.mImageSpec.mImageWidth;
				}

				if (lLeftToRight == true)
				{
					for (int x = 0; x < pFileHeader.mImageSpec.mImageWidth; x++)
					{
						pFile.Read(lData[lYOffset + x]);
					}
				}
				else
				{
					for (int x = pFileHeader.mImageSpec.mImageWidth - 1; x >= 0; x--)
					{
						pFile.Read(lData[lYOffset + x]);
					}
				}
			}

			// Convert this image to a "true color image".
			int lImageSize = pFileHeader.mImageSpec.mImageWidth * pFileHeader.mImageSpec.mImageHeight;
			for (int i = 0; i < lImageSize; i++)
			{
				uint16 lIndex = lData[i];
				unsigned r = (unsigned)pPalette[lIndex].mRed;
				unsigned g = (unsigned)pPalette[lIndex].mGreen;
				unsigned b = (unsigned)pPalette[lIndex].mBlue;

				// Write one pixel.
				switch(pCanvas.GetBitDepth())
				{
				case Canvas::BITDEPTH_15_BIT:
					{
						uint16 lColor = (uint16)(((r >> 3) << 10) | ((g >> 3) << 5) | (b >> 3));
						((uint16*)pCanvas.GetBuffer())[i] = lColor;
					}
					break;
				case Canvas::BITDEPTH_16_BIT:
					{
						uint16 lColor = (uint16)(((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3));
						((uint16*)pCanvas.GetBuffer())[i] = lColor;
					}
					break;
				case Canvas::BITDEPTH_24_BIT:
					{
						((uint8*)pCanvas.GetBuffer())[i * 3 + 0] = (uint8)b;
						((uint8*)pCanvas.GetBuffer())[i * 3 + 1] = (uint8)g;
						((uint8*)pCanvas.GetBuffer())[i * 3 + 2] = (uint8)r;
					}
					break;
				case Canvas::BITDEPTH_32_BIT:
						((uint8*)pCanvas.GetBuffer())[i * 4 + 0] = (uint8)b;
						((uint8*)pCanvas.GetBuffer())[i * 4 + 1] = (uint8)g;
						((uint8*)pCanvas.GetBuffer())[i * 4 + 2] = (uint8)r;
						((uint8*)pCanvas.GetBuffer())[i * 4 + 3] = 0;
					break;
				}
			}

			delete[] lData;
		}
		break;
	default:
		return STATUS_READ_PICTURE_ERROR;
	};

	return STATUS_SUCCESS;
}

TgaLoader::Status TgaLoader::LoadUncompressedTrueColorImage(Canvas& pCanvas, TGAFileHeader& pFileHeader, File& pFile)
{
	bool lLeftToRight = (pFileHeader.mImageSpec.mImageDescriptor & 16) == 0;
	bool lTopToBottom = (pFileHeader.mImageSpec.mImageDescriptor & 32) != 0;

	int lPixelDepth = pFileHeader.mImageSpec.mPixelDepth;
	if (lPixelDepth == 16)
	{
		// Change this to 15 bit, since that's what Adobe Photoshop _means_ with "16".
		lPixelDepth = 15;
	}

	pCanvas.Reset(pFileHeader.mImageSpec.mImageWidth,
				   pFileHeader.mImageSpec.mImageHeight,
				   Canvas::IntToBitDepth(lPixelDepth));
	pCanvas.CreateBuffer();

	switch(pFileHeader.mImageSpec.mPixelDepth)
	{
	case 8:
	case 24:
	case 32:
		{
			uint8* lData = (uint8*)pCanvas.GetBuffer();

			int y;
			for (y = 0; y < pFileHeader.mImageSpec.mImageHeight; y++)
			{
				int lYOffset = 0;
				if (lTopToBottom == true)
				{
					lYOffset = y * pFileHeader.mImageSpec.mImageWidth * pCanvas.GetPixelByteSize();
				}
				else
				{
					lYOffset = (pFileHeader.mImageSpec.mImageHeight - (y + 1)) * pFileHeader.mImageSpec.mImageWidth * pCanvas.GetPixelByteSize();
				}

				if (lLeftToRight == true)
				{
					pFile.ReadData(lData + lYOffset, pFileHeader.mImageSpec.mImageWidth * pCanvas.GetPixelByteSize());
				}
				else
				{
					for (int x = pFileHeader.mImageSpec.mImageWidth - 1; x >= 0; x--)
					{
						pFile.ReadData(lData + lYOffset + x * pCanvas.GetPixelByteSize(), pCanvas.GetPixelByteSize());
					}
				}
			}
		}
		break;
	case 15:
	case 16:
			uint16* lData = (uint16*)pCanvas.GetBuffer();

			int y;
			for (y = 0; y < pFileHeader.mImageSpec.mImageHeight; y++)
			{
				int lYOffset = 0;
				if (lTopToBottom == true)
				{
					lYOffset = y * pFileHeader.mImageSpec.mImageWidth;
				}
				else
				{
					lYOffset = (pFileHeader.mImageSpec.mImageHeight - (y + 1)) * pFileHeader.mImageSpec.mImageWidth;
				}

				if (lLeftToRight == true)
				{
					for (int x = 0; x < pFileHeader.mImageSpec.mImageWidth; x++)
					{
						pFile.Read(lData[lYOffset + x]);
					}
				}
				else
				{
					for (int x = pFileHeader.mImageSpec.mImageWidth - 1; x >= 0; x--)
					{
						pFile.Read(lData[lYOffset + x]);
					}
				}
			}
		break;
	};

	return STATUS_SUCCESS;
}

TgaLoader::Status TgaLoader::LoadUncompressedBlackAndWhiteImage(Canvas& pCanvas, TGAFileHeader& pFileHeader, File& pFile)
{
	if (pFileHeader.mImageSpec.mPixelDepth != 8)
	{
		return STATUS_READ_PICTURE_ERROR;
	}

	pCanvas.Reset(pFileHeader.mImageSpec.mImageWidth,
				   pFileHeader.mImageSpec.mImageHeight,
				   Canvas::BITDEPTH_8_BIT);

	pCanvas.CreateBuffer();

	Color lPalette[256];
	for (int i = 0; i < 256; i++)
	{
		lPalette[i].mRed   = (uint8)i;
		lPalette[i].mGreen = (uint8)i;
		lPalette[i].mBlue  = (uint8)i;
		lPalette[i].mAlpha = (uint8)i;
	}
	pCanvas.SetPalette(lPalette);

	bool lLeftToRight = (pFileHeader.mImageSpec.mImageDescriptor & 16) == 0;
	bool lTopToBottom = (pFileHeader.mImageSpec.mImageDescriptor & 32) != 0;

	uint8* lData = (uint8*)pCanvas.GetBuffer();

	int y;
	for (y = 0; y < pFileHeader.mImageSpec.mImageHeight; y++)
	{
		int lYOffset = 0;
		if (lTopToBottom == true)
		{
			lYOffset = y * pFileHeader.mImageSpec.mImageWidth;
		}
		else
		{
			lYOffset = (pFileHeader.mImageSpec.mImageHeight - (y + 1)) * pFileHeader.mImageSpec.mImageWidth;
		}

		if (lLeftToRight == true)
		{
			pFile.ReadData(lData + lYOffset, pFileHeader.mImageSpec.mImageWidth);
		}
		else
		{
			for (int x = pFileHeader.mImageSpec.mImageWidth - 1; x >= 0; x--)
			{
				pFile.ReadData(lData + lYOffset + x, 1);
			}
		}
	}

	return STATUS_SUCCESS;
}

TgaLoader::Status TgaLoader::LoadRLEColorMapImage(Canvas& pCanvas, TGAFileHeader& pFileHeader, File& pFile, Color* pPalette)
{
	if (pPalette == 0)
	{
		return STATUS_READ_PALETTE_ERROR;
	}

	bool lLeftToRight = (pFileHeader.mImageSpec.mImageDescriptor & 16) == 0;
	bool lTopToBottom = (pFileHeader.mImageSpec.mImageDescriptor & 32) != 0;

	if (pFileHeader.mColorMapSpec.mColorMapLength <= 256)
	{
		pCanvas.Reset(pFileHeader.mImageSpec.mImageWidth,
					   pFileHeader.mImageSpec.mImageHeight,
					   Canvas::IntToBitDepth(pFileHeader.mImageSpec.mPixelDepth));
		pCanvas.SetPalette(pPalette);
	}
	else
	{
		// Reset image to same pixel depth as the palette, if there are more than 256 colors.
		pCanvas.Reset(pFileHeader.mImageSpec.mImageWidth,
					   pFileHeader.mImageSpec.mImageHeight,
					   Canvas::IntToBitDepth(pFileHeader.mColorMapSpec.mColorMapEntrySize));
	}

	pCanvas.CreateBuffer();

	switch(pFileHeader.mImageSpec.mPixelDepth)
	{
	case 8:
		{
			uint8* lData = (uint8*)pCanvas.GetBuffer();

			int y;
			for (y = 0; y < pFileHeader.mImageSpec.mImageHeight; y++)
			{
				int lYOffset = 0;
				if (lTopToBottom == true)
				{
					lYOffset = y * pFileHeader.mImageSpec.mImageWidth;
				}
				else
				{
					lYOffset = (pFileHeader.mImageSpec.mImageHeight - (y + 1)) * pFileHeader.mImageSpec.mImageWidth;
				}

				int lXAdd = (lLeftToRight == true) ? 1 : -1;
				for (int x = (lLeftToRight == true) ? 0 : (pFileHeader.mImageSpec.mImageWidth - 1);
					(lLeftToRight == true) ? (x < pFileHeader.mImageSpec.mImageWidth) : x >= 0;)
				{
					uint8 lRepetitionCount;
					pFile.Read(lRepetitionCount);

					bool lRLEPacket = (lRepetitionCount & 0x80) != 0;
					lRepetitionCount &= 0x7F;
					lRepetitionCount++;

					if (lRLEPacket == true)
					{
						// RLE packet.
						uint8 lPixelValue;
						pFile.Read(lPixelValue);

						for (int lCount = 0; lCount < (int)lRepetitionCount; lCount++)
						{
							lData[lYOffset + x] = lPixelValue;
							x += lXAdd;
						}
					}
					else
					{
						// Raw packet.
						for (int lCount = 0; lCount < (int)lRepetitionCount; lCount++)
						{
							pFile.Read(lData[lYOffset + x]);
							x += lXAdd;
						}
					}
				}
			}
		}
		break;
	case 15:
	case 16:
		{
			switch(pCanvas.GetBitDepth())
			{
			case Canvas::BITDEPTH_15_BIT:
			case Canvas::BITDEPTH_16_BIT:
			case Canvas::BITDEPTH_24_BIT:
			case Canvas::BITDEPTH_32_BIT:
				// OK...
				break;
			default:
				return STATUS_READ_PALETTE_ERROR;
			};
		
			uint16* lData = new uint16[pFileHeader.mImageSpec.mImageWidth * pFileHeader.mImageSpec.mImageHeight];

			int y;
			for (y = 0; y < pFileHeader.mImageSpec.mImageHeight; y++)
			{
				int lYOffset = 0;
				if (lTopToBottom == true)
				{
					lYOffset = y * pFileHeader.mImageSpec.mImageWidth;
				}
				else
				{
					lYOffset = (pFileHeader.mImageSpec.mImageHeight - (y + 1)) * pFileHeader.mImageSpec.mImageWidth;
				}

				int lXAdd = (lLeftToRight == true) ? 1 : -1;
				for (int x = (lLeftToRight == true) ? 0 : (pFileHeader.mImageSpec.mImageWidth - 1);
					(lLeftToRight == true) ? (x < pFileHeader.mImageSpec.mImageWidth) : x >= 0;)
				{
					uint8 lRepetitionCount;
					pFile.Read(lRepetitionCount);

					bool lRLEPacket = (lRepetitionCount & 0x80) != 0;
					lRepetitionCount &= 0x7F;
					lRepetitionCount++;

					if (lRLEPacket == true)
					{
						// RLE packet.
						uint16 lPixelValue;
						pFile.Read(lPixelValue);

						for (int lCount = 0; lCount < (int)lRepetitionCount; lCount++)
						{
							lData[lYOffset + x] = lPixelValue;
							x += lXAdd;
						}
					}
					else
					{
						// Raw packet.
						for (int lCount = 0; lCount < (int)lRepetitionCount; lCount++)
						{
							pFile.Read(lData[lYOffset + x]);
							x += lXAdd;
						}
					}
				}
			}

			// Convert this image to a "true color image".
			int lImageSize = pFileHeader.mImageSpec.mImageWidth * pFileHeader.mImageSpec.mImageHeight;
			for (int i = 0; i < lImageSize; i++)
			{
				uint16 lIndex = lData[i];
				unsigned r = (unsigned)pPalette[lIndex].mRed;
				unsigned g = (unsigned)pPalette[lIndex].mGreen;
				unsigned b = (unsigned)pPalette[lIndex].mBlue;

				// Write one pixel.
				switch(pCanvas.GetBitDepth())
				{
				case Canvas::BITDEPTH_15_BIT:
					{
						uint16 lColor = (uint16)(((r >> 3) << 10) | ((g >> 3) << 5) | (b >> 3));
						((uint16*)pCanvas.GetBuffer())[i] = lColor;
					}
					break;
				case Canvas::BITDEPTH_16_BIT:
					{
						uint16 lColor = (uint16)(((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3));
						((uint16*)pCanvas.GetBuffer())[i] = lColor;
					}
					break;
				case Canvas::BITDEPTH_24_BIT:
					{
						((uint8*)pCanvas.GetBuffer())[i * 3 + 0] = (uint8)b;
						((uint8*)pCanvas.GetBuffer())[i * 3 + 1] = (uint8)g;
						((uint8*)pCanvas.GetBuffer())[i * 3 + 2] = (uint8)r;
					}
					break;
				case Canvas::BITDEPTH_32_BIT:
						((uint8*)pCanvas.GetBuffer())[i * 4 + 0] = (uint8)b;
						((uint8*)pCanvas.GetBuffer())[i * 4 + 1] = (uint8)g;
						((uint8*)pCanvas.GetBuffer())[i * 4 + 2] = (uint8)r;
						((uint8*)pCanvas.GetBuffer())[i * 4 + 3] = 0;
					break;
				}
			}

			delete[] lData;
		}
		break;
	default:
		return STATUS_READ_PICTURE_ERROR;
	};

	return STATUS_SUCCESS;
}

TgaLoader::Status TgaLoader::LoadRLETrueColorImage(Canvas& pCanvas, TGAFileHeader& pFileHeader, File& pFile)
{
	bool lLeftToRight = (pFileHeader.mImageSpec.mImageDescriptor & 16) == 0;
	bool lTopToBottom = (pFileHeader.mImageSpec.mImageDescriptor & 32) != 0;

	int lPixelDepth = pFileHeader.mImageSpec.mPixelDepth;
	if (lPixelDepth == 16)
	{
		// Change this to 15 bit, since that's what Adobe Photoshop _means_ with "16".
		lPixelDepth = 15;
	}

	pCanvas.Reset(pFileHeader.mImageSpec.mImageWidth,
				   pFileHeader.mImageSpec.mImageHeight,
				   Canvas::IntToBitDepth(lPixelDepth));
	pCanvas.CreateBuffer();

	switch(pFileHeader.mImageSpec.mPixelDepth)
	{
	case 8:
		{
			uint8* lData = (uint8*)pCanvas.GetBuffer();

			int y;
			for (y = 0; y < pFileHeader.mImageSpec.mImageHeight; y++)
			{
				int lYOffset = 0;
				if (lTopToBottom == true)
				{
					lYOffset = y * pFileHeader.mImageSpec.mImageWidth;
				}
				else
				{
					lYOffset = (pFileHeader.mImageSpec.mImageHeight - (y + 1)) * pFileHeader.mImageSpec.mImageWidth;
				}

				int lXAdd = (lLeftToRight == true) ? 1 : -1;
				for (int x = (lLeftToRight == true) ? 0 : (pFileHeader.mImageSpec.mImageWidth - 1);
					(lLeftToRight == true) ? (x < pFileHeader.mImageSpec.mImageWidth) : x >= 0;)
				{
					uint8 lRepetitionCount;
					pFile.Read(lRepetitionCount);

					bool lRLEPacket = (lRepetitionCount & 0x80) != 0;
					lRepetitionCount &= 0x7F;
					lRepetitionCount++;

					if (lRLEPacket == true)
					{
						// RLE packet.
						uint8 lPixelValue;
						pFile.Read(lPixelValue);

						for (int lCount = 0; lCount < (int)lRepetitionCount; lCount++)
						{
							lData[lYOffset + x] = lPixelValue;
							x += lXAdd;
						}
					}
					else
					{
						// Raw packet.
						for (int lCount = 0; lCount < (int)lRepetitionCount; lCount++)
						{
							if (pFile.Read(lData[lYOffset + x]) != IO_OK)
							{
								return (STATUS_READSTREAM_ERROR);	// TRICKY: RAII!
							}
							x += lXAdd;
						}
					}
				}
			}
		}
		break;
	case 15:
	case 16:
		{
			uint16* lData = (uint16*)pCanvas.GetBuffer();

			int y;
			for (y = 0; y < pFileHeader.mImageSpec.mImageHeight; y++)
			{
				int lYOffset = 0;
				if (lTopToBottom == true)
				{
					lYOffset = y * pFileHeader.mImageSpec.mImageWidth;
				}
				else
				{
					lYOffset = (pFileHeader.mImageSpec.mImageHeight - (y + 1)) * pFileHeader.mImageSpec.mImageWidth;
				}

				int lXAdd = (lLeftToRight == true) ? 1 : -1;
				for (int x = (lLeftToRight == true) ? 0 : (pFileHeader.mImageSpec.mImageWidth - 1);
					(lLeftToRight == true) ? (x < pFileHeader.mImageSpec.mImageWidth) : x >= 0;)
				{
					uint8 lRepetitionCount;
					pFile.Read(lRepetitionCount);

					bool lRLEPacket = (lRepetitionCount & 0x80) != 0;
					lRepetitionCount &= 0x7F;
					lRepetitionCount++;

					if (lRLEPacket == true)
					{
						// RLE packet.
						uint16 lPixelValue;
						pFile.Read(lPixelValue);

						for (int lCount = 0; lCount < (int)lRepetitionCount; lCount++)
						{
							lData[lYOffset + x] = lPixelValue;
							x += lXAdd;
						}
					}
					else
					{
						// Raw packet.
						for (int lCount = 0; lCount < (int)lRepetitionCount; lCount++)
						{
							if (pFile.Read(lData[lYOffset + x]) != IO_OK)
							{
								return (STATUS_READSTREAM_ERROR);	// TRICKY: RAII!
							}
							x += lXAdd;
						}
					}
				}
			}
		}
		break;
	case 24:
		{
			uint8* lData = (uint8*)pCanvas.GetBuffer();

			int y;
			for (y = 0; y < pFileHeader.mImageSpec.mImageHeight; y++)
			{
				int lYOffset = 0;
				if (lTopToBottom == true)
				{
					lYOffset = y * pFileHeader.mImageSpec.mImageWidth * 3;
				}
				else
				{
					lYOffset = (pFileHeader.mImageSpec.mImageHeight - (y + 1)) * pFileHeader.mImageSpec.mImageWidth * 3;
				}

				int lXAdd = (lLeftToRight == true) ? 1 : -1;
				for (int x = (lLeftToRight == true) ? 0 : (pFileHeader.mImageSpec.mImageWidth - 1);
					(lLeftToRight == true) ? (x < pFileHeader.mImageSpec.mImageWidth) : x >= 0;)
				{
					uint8 lRepetitionCount;
					pFile.Read(lRepetitionCount);

					bool lRLEPacket = (lRepetitionCount & 0x80) != 0;
					lRepetitionCount &= 0x7F;
					lRepetitionCount++;

					if (lRLEPacket == true)
					{
						// RLE packet.
						uint8 lPixelValue[3];
						pFile.ReadData(&lPixelValue, 3);

						for (int lCount = 0; lCount < (int)lRepetitionCount; lCount++)
						{
							lData[lYOffset + x * 3 + 0] = lPixelValue[0];
							lData[lYOffset + x * 3 + 1] = lPixelValue[1];
							lData[lYOffset + x * 3 + 2] = lPixelValue[2];

							x += lXAdd;
						}
					}
					else
					{
						// Raw packet.
						for (int lCount = 0; lCount < (int)lRepetitionCount; lCount++)
						{
							if (pFile.ReadData(&lData[lYOffset + x * 3], 3) != IO_OK)
							{
								return (STATUS_READSTREAM_ERROR);	// TRICKY: RAII!
							}
							x += lXAdd;
						}
					}
				}
			}
		}
		break;
	case 32:
		{
			uint8* lData = (uint8*)pCanvas.GetBuffer();

			int y;
			for (y = 0; y < pFileHeader.mImageSpec.mImageHeight; y++)
			{
				int lYOffset = 0;
				if (lTopToBottom == true)
				{
					lYOffset = y * pFileHeader.mImageSpec.mImageWidth * 4;
				}
				else
				{
					lYOffset = (pFileHeader.mImageSpec.mImageHeight - (y + 1)) * pFileHeader.mImageSpec.mImageWidth * 4;
				}

				int lXAdd = (lLeftToRight == true) ? 1 : -1;
				for (int x = (lLeftToRight == true) ? 0 : (pFileHeader.mImageSpec.mImageWidth - 1);
					(lLeftToRight == true) ? (x < pFileHeader.mImageSpec.mImageWidth) : x >= 0;)
				{
					uint8 lRepetitionCount;
					pFile.Read(lRepetitionCount);

					bool lRLEPacket = (lRepetitionCount & 0x80) != 0;
					lRepetitionCount &= 0x7F;
					lRepetitionCount++;

					if (lRLEPacket == true)
					{
						// RLE packet.
						uint8 lBGRA[4];
						pFile.ReadData(&lBGRA, 4);

						for (int lCount = 0; lCount < (int)lRepetitionCount; lCount++)
						{
							// Jonte: RLE stores color as BGR.
							lData[lYOffset + x * 4 + 0] = lBGRA[2];
							lData[lYOffset + x * 4 + 1] = lBGRA[1];
							lData[lYOffset + x * 4 + 2] = lBGRA[0];
							lData[lYOffset + x * 4 + 3] = lBGRA[3];

							x += lXAdd;
						}
					}
					else
					{
						// Raw packet.
						for (int lCount = 0; lCount < (int)lRepetitionCount; lCount++)
						{
							uint8 lBGRA[4];
							if (pFile.ReadData(lBGRA, sizeof(lBGRA)) != IO_OK)
							{
								return (STATUS_READSTREAM_ERROR);	// TRICKY: RAII!
							}
							lData[lYOffset + x * 4 + 0] = lBGRA[2];
							lData[lYOffset + x * 4 + 1] = lBGRA[1];
							lData[lYOffset + x * 4 + 2] = lBGRA[0];
							lData[lYOffset + x * 4 + 3] = lBGRA[3];
							x += lXAdd;
						}
					}
				}
			}
		}
		break;
	};

	return STATUS_SUCCESS;
}

TgaLoader::Status TgaLoader::LoadRLEBlackAndWhiteImage(Canvas& pCanvas, TGAFileHeader& pFileHeader, File& pFile)
{
	if (pFileHeader.mImageSpec.mPixelDepth != 8)
	{
		return STATUS_READ_PICTURE_ERROR;
	}

	pCanvas.Reset(pFileHeader.mImageSpec.mImageWidth,
				   pFileHeader.mImageSpec.mImageHeight,
				   Canvas::BITDEPTH_8_BIT);
	pCanvas.CreateBuffer();

	Color lPalette[256];
	for (int i = 0; i < 256; i++)
	{
		lPalette[i].mRed   = (uint8)i;
		lPalette[i].mGreen = (uint8)i;
		lPalette[i].mBlue  = (uint8)i;
		lPalette[i].mAlpha = (uint8)i;
	}
	pCanvas.SetPalette(lPalette);

	bool lLeftToRight = (pFileHeader.mImageSpec.mImageDescriptor & 16) == 0;
	bool lTopToBottom = (pFileHeader.mImageSpec.mImageDescriptor & 32) != 0;

	uint8* lData = (uint8*)pCanvas.GetBuffer();

	int y;
	for (y = 0; y < pFileHeader.mImageSpec.mImageHeight; y++)
	{
		int lYOffset = 0;
		if (lTopToBottom == true)
		{
			lYOffset = y * pFileHeader.mImageSpec.mImageWidth;
		}
		else
		{
			lYOffset = (pFileHeader.mImageSpec.mImageHeight - (y + 1)) * pFileHeader.mImageSpec.mImageWidth;
		}

		int lXAdd = (lLeftToRight == true) ? 1 : -1;
		for (int x = (lLeftToRight == true) ? 0 : (pFileHeader.mImageSpec.mImageWidth - 1);
			(lLeftToRight == true) ? (x < pFileHeader.mImageSpec.mImageWidth) : x >= 0;)
		{
			uint8 lRepetitionCount;
			pFile.Read(lRepetitionCount);

			bool lRLEPacket = (lRepetitionCount & 0x80) != 0;
			lRepetitionCount &= 0x7F;
			lRepetitionCount++;

			if (lRLEPacket == true)
			{
				// RLE packet.
				uint8 lPixelValue;
				pFile.Read(lPixelValue);

				for (int lCount = 0; lCount < (int)lRepetitionCount; lCount++)
				{
					lData[lYOffset + x] = lPixelValue;
					x += lXAdd;
				}
			}
			else
			{
				// Raw packet.
				for (int lCount = 0; lCount < (int)lRepetitionCount; lCount++)
				{
					pFile.Read(lData[lYOffset + x]);
					x += lXAdd;
				}
			}
		}
	}

	return STATUS_SUCCESS;
}



const char* TgaLoader::smTruevisionXFile = "TRUEVISION-XFILE.";



}
