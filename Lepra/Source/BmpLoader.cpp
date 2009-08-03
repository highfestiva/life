/*
	Class:  BmpLoader
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games
*/

#include <math.h>
#include "../Include/BmpLoader.h"
#include "../Include/Canvas.h"
#include "../Include/DiskFile.h"
#include "../Include/MetaFile.h"
#include "../Include/ArchiveFile.h"

namespace Lepra
{

BmpLoader::Status BmpLoader::Load(const String& pFileName, Canvas& pCanvas)
{
	Status lStatus = STATUS_SUCCESS;
	MetaFile lFile;
	
	if (lFile.Open(pFileName, MetaFile::READ_ONLY, false, Endian::TYPE_LITTLE_ENDIAN) == false)
	{
		lStatus = STATUS_OPEN_ERROR;
	}

	if (lStatus == STATUS_SUCCESS)
	{
		Load(lFile, pCanvas);
		lFile.Close();
	}

	return lStatus;
}

BmpLoader::Status BmpLoader::Save(const String& pFileName, const Canvas& pCanvas)
{
	Status lStatus = STATUS_SUCCESS;
	DiskFile lFile;
	
	if (lFile.Open(pFileName, DiskFile::MODE_WRITE, false, Endian::TYPE_LITTLE_ENDIAN) == false)
	{
		lStatus = STATUS_OPEN_ERROR;
	}

	if (lStatus == STATUS_SUCCESS)
	{
		Save(lFile, pCanvas);
		lFile.Close();
	}

	return lStatus;
}

BmpLoader::Status BmpLoader::Load(const String& pArchiveName, const String& pFileName, Canvas& pCanvas)
{
	Status lStatus = STATUS_SUCCESS;
	ArchiveFile lFile(pArchiveName);
	
	if (lFile.Open(pFileName, ArchiveFile::READ_ONLY, Endian::TYPE_LITTLE_ENDIAN) == false)
	{
		lStatus = STATUS_OPEN_ERROR;
	}

	if (lStatus == STATUS_SUCCESS)
	{
		Load(lFile, pCanvas);
		lFile.Close();
	}

	return lStatus;
}

BmpLoader::Status BmpLoader::Save(const String& pArchiveName, const String& pFileName, const Canvas& pCanvas)
{
	Status lStatus = STATUS_SUCCESS;
	ArchiveFile lFile(pArchiveName);
	
	if (lFile.Open(pFileName, ArchiveFile::WRITE_ONLY, Endian::TYPE_LITTLE_ENDIAN) == false)
	{
		lStatus = STATUS_OPEN_ERROR;
	}

	if (lStatus == STATUS_SUCCESS)
	{
		Save(lFile, pCanvas);
		lFile.Close();
	}

	return lStatus;
}

BmpLoader::Status BmpLoader::Load(Reader& pReader, Canvas& pCanvas)
{
	BitmapFileHeader lFileHeader;
	BitmapInfoHeader lInfoHeader;

	// Load the file header data:
	if (lFileHeader.Load(&pReader) == false)
	{
		return STATUS_READ_HEADER_ERROR;
	}

	// Load the information header data:
	if (lInfoHeader.Load(&pReader) == false)
	{
		return STATUS_READ_INFO_ERROR;
	}

	int lColorCount = (int) (1 << lInfoHeader.mBitCount);
	int lBitDepth   = lInfoHeader.mBitCount;

	if (lBitDepth <= 8)
	{
		Color lPalette[256];

		for (int i = 0; i < lColorCount; i++)
		{
			if (pReader.ReadData(&lPalette[i], sizeof(Color)) != IO_OK)
			{
				return STATUS_READ_PALETTE_ERROR;
			}
		}

		pCanvas.SetPalette(lPalette);
	}

	int lWidth	= lInfoHeader.mWidth;
	int lHeight	= lInfoHeader.mHeight;

	pCanvas.Reset(abs(lWidth), abs(lHeight), Canvas::IntToBitDepth(lBitDepth));
	pCanvas.CreateBuffer();

	// The ScanWidth is a multiple by 4.
	int lScanWidth = ((lWidth * (lBitDepth >> 3)) + 3) & (~3);
	int lScanPadding	= lScanWidth - (lWidth * (lBitDepth >> 3));

	pReader.Skip(lFileHeader.mOffBits - (int)pReader.GetReadCount());
	//lFile.SeekSet(lFileHeader.mOffBits);

	uint8* lImage = (uint8*)pCanvas.GetBuffer();

	// Load the bitmap:
	for (int y = 0; y < abs(lHeight); y++)
	{
		unsigned lYOffset;

		// Load a scan-line:
		switch(lInfoHeader.mCompression)
		{
			case COMP_BI_RGB:

				// Uncompressed image:
				if (lHeight < 0)
				{
					lYOffset = y * lWidth * (lBitDepth >> 3);
				}
				else
				{
					//YPos = ((-mHeight - Y) * (mWidth * (mBitDepth >> 3))) - (mWidth * (mBitDepth >> 3));
					lYOffset = (lHeight - (y + 1)) * lWidth * (lBitDepth >> 3);
				}

				if (pReader.ReadData(&lImage[lYOffset], lWidth * (lBitDepth >> 3)) != IO_OK)
				{
					return STATUS_READ_PICTURE_ERROR;
				}
				break;
			case COMP_BI_RLE8:
			case COMP_BI_RLE4:
			case COMP_BI_BITFIELDS:
			default:
				return STATUS_COMPRESSION_ERROR;
		}
		// Skip the padding possibly located at the end of each
		// scan-line:
		pReader.Skip(lScanPadding);
	}

	return STATUS_SUCCESS;
}

BmpLoader::Status BmpLoader::Save(Writer& pWriter, const Canvas& pCanvas)
{
	BitmapFileHeader lFileHeader;
	BitmapInfoHeader lInfoHeader;

	lFileHeader.mType = (((int16)'M') << 8) + (int16)'B';
	lFileHeader.mSize = lFileHeader.GetSize() + lInfoHeader.GetSize() +
						   pCanvas.GetWidth() * pCanvas.GetHeight() * pCanvas.GetPixelByteSize();
	lFileHeader.mReserved1 = 0;
	lFileHeader.mReserved2 = 0;
	lFileHeader.mOffBits = lFileHeader.GetSize() + lInfoHeader.GetSize();

	if (pCanvas.GetBitDepth() == Canvas::BITDEPTH_8_BIT)
	{
		lFileHeader.mSize += sizeof(Color) * 256;
		lFileHeader.mOffBits += sizeof(Color) * 256;
	}

	unsigned lBitDepth = Canvas::BitDepthToInt(pCanvas.GetBitDepth());

	lInfoHeader.mSize			= lInfoHeader.GetSize();
	lInfoHeader.mWidth			= pCanvas.GetWidth();
	lInfoHeader.mHeight			= pCanvas.GetHeight();
	lInfoHeader.mPlanes			= 1;
	lInfoHeader.mBitCount		= (int16)lBitDepth;
	lInfoHeader.mCompression		= COMP_BI_RGB;
	lInfoHeader.mSizeImage		= pCanvas.GetWidth() * pCanvas.GetHeight() * pCanvas.GetPixelByteSize();
	lInfoHeader.mXPelsPerMeter	= 2834;	// Kind'a Photoshop standard, I think.
	lInfoHeader.mYPelsPerMeter	= 2834;
	lInfoHeader.mClrUsed			= 0;
	lInfoHeader.mClrImportant	= 0;

	if (lBitDepth <= 8)
	{
		lInfoHeader.mClrUsed			= (1 << lBitDepth);
		lInfoHeader.mClrImportant	= (1 << lBitDepth);
	}

	lFileHeader.Save(&pWriter);
	lInfoHeader.Save(&pWriter);

	int y;

	if (lBitDepth <= 8)
	{
		for (y = 0; y < (1 << lBitDepth); y++)
		{
			pWriter.WriteData(&pCanvas.GetPalette()[y], sizeof(Color));
		}
	}

	int lScanWidth	= ((pCanvas.GetWidth() * pCanvas.GetPixelByteSize()) + 3) & (~3);
	int lScanPadding	= lScanWidth - (pCanvas.GetWidth() * pCanvas.GetPixelByteSize());
	uint8 lPadBytes[4];

	for (y = pCanvas.GetHeight() - 1; y >= 0; y--)
	{
		pWriter.WriteData((uint8*)pCanvas.GetBuffer() + y * pCanvas.GetPitch() * pCanvas.GetPixelByteSize(),
						 pCanvas.GetWidth() * pCanvas.GetPixelByteSize());
		// The ScanWidth must be a multiple by 4.
		pWriter.WriteData(lPadBytes, lScanPadding);
	}

	return STATUS_SUCCESS;
}


/////////////////////////////////////////////////////
//                                                 //
//             Class BitmapFileHeader           //
//                                                 //
/////////////////////////////////////////////////////

int BmpLoader::BitmapFileHeader::GetSize()
{
	int lSize = sizeof(mType) +
					sizeof(mSize) +
					sizeof(mReserved1) +
					sizeof(mReserved2) +
					sizeof(mOffBits);

	return lSize;
}


bool BmpLoader::BitmapFileHeader::Load(Reader* pReader)
{
	// Load each member one by one... This is done to avoid errors that
	// can occur due to strange compiler settings.
	if (pReader->Read(mType) != IO_OK)
	{
		return false;
	}

	if (pReader->Read(mSize) != IO_OK)
	{
		return false;
	}

	if (pReader->Read(mReserved1) != IO_OK)
	{
		return false;
	}

	if (pReader->Read(mReserved2) != IO_OK)
	{
		return false;
	}

	if (pReader->Read(mOffBits) != IO_OK)
	{
		return false;
	}

	return true;
}




bool BmpLoader::BitmapFileHeader::Save(Writer* pWriter)
{
	// Save each member one by one... This is done to avoid errors that
	// can occur due to strange compiler settings.
	if (pWriter->Write(mType) != IO_OK)
	{
		return false;
	}

	if (pWriter->Write(mSize) != IO_OK)
	{
		return false;
	}

	if (pWriter->Write(mReserved1) != IO_OK)
	{
		return false;
	}

	if (pWriter->Write(mReserved2) != IO_OK)
	{
		return false;
	}

	if (pWriter->Write(mOffBits) != IO_OK)
	{
		return false;
	}

	return true;
}



/////////////////////////////////////////////////////
//                                                 //
//             Class BitmapInfoHeader           //
//                                                 //
/////////////////////////////////////////////////////



int BmpLoader::BitmapInfoHeader::GetSize()
{
	int lSize = sizeof(mSize) +
					sizeof(mWidth) +
					sizeof(mHeight) +
					sizeof(mPlanes) +
					sizeof(mBitCount) +
					sizeof(mCompression) +
					sizeof(mSizeImage) +
					sizeof(mXPelsPerMeter) +
					sizeof(mYPelsPerMeter) +
					sizeof(mClrUsed) +
					sizeof(mClrImportant);
	return lSize;
}


bool BmpLoader::BitmapInfoHeader::Load(Reader* pReader)
{
	// Load each member one by one... This is done to avoid errors that
	// can occur due to strange compiler settings.

	if (pReader->Read(mSize) != IO_OK)
	{
		return false;
	}

	if (pReader->Read(mWidth) != IO_OK)
	{
		return false;
	}

	if (pReader->Read(mHeight) != IO_OK)
	{
		return false;
	}

	if (pReader->Read(mPlanes) != IO_OK)
	{
		return false;
	}

	if (pReader->Read(mBitCount) != IO_OK)
	{
		return false;
	}

	if (pReader->Read(mCompression) != IO_OK)
	{
		return false;
	}

	if (pReader->Read(mSizeImage) != IO_OK)
	{
		return false;
	}

	if (pReader->Read(mXPelsPerMeter) != IO_OK)
	{
		return false;
	}

	if (pReader->Read(mYPelsPerMeter) != IO_OK)
	{
		return false;
	}

	if (pReader->Read(mClrUsed) != IO_OK)
	{
		return false;
	}

	if (pReader->Read(mClrImportant) != IO_OK)
	{
		return false;
	}

	return true;
}




bool BmpLoader::BitmapInfoHeader::Save(Writer* pWriter)
{
	// Save each member one by one... This is done to avoid errors that
	// can occur due to strange compiler settings.

	if (pWriter->Write(mSize) != IO_OK)
	{
		return false;
	}

	if (pWriter->Write(mWidth) != IO_OK)
	{
		return false;
	}

	if (pWriter->Write(mHeight) != IO_OK)
	{
		return false;
	}

	if (pWriter->Write(mPlanes) != IO_OK)
	{
		return false;
	}

	if (pWriter->Write(mBitCount) != IO_OK)
	{
		return false;
	}

	if (pWriter->Write(mCompression) != IO_OK)
	{
		return false;
	}

	if (pWriter->Write(mSizeImage) != IO_OK)
	{
		return false;
	}

	if (pWriter->Write(mXPelsPerMeter) != IO_OK)
	{
		return false;
	}

	if (pWriter->Write(mYPelsPerMeter) != IO_OK)
	{
		return false;
	}

	if (pWriter->Write(mClrUsed) != IO_OK)
	{
		return false;
	}

	if (pWriter->Write(mClrImportant) != IO_OK)
	{
		return false;
	}

	return true;
}


} // End namespace.
