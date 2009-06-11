/*
	Class:  TiffLoader
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games
*/

#include "../Include/LepraTypes.h"
#include "../Include/TiffLoader.h"
#include "../Include/Graphics2D.h"
#include "../Include/Canvas.h"
#include "../Include/DiskFile.h"
#include "../Include/MetaFile.h"
#include "../Include/MemFile.h"
#include "../Include/ArchiveFile.h"

#include <memory.h>

namespace Lepra
{

TiffLoader::Status TiffLoader::Load(const String& pFileName, Canvas& pCanvas)
{
	MetaFile lFile;
	Status lStatus = STATUS_SUCCESS;

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

TiffLoader::Status TiffLoader::Save(const String& pFileName, const Canvas& pCanvas)
{
	DiskFile lFile;
	Status lStatus = STATUS_SUCCESS;

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

TiffLoader::Status TiffLoader::Load(const String& pArchiveName, const String& pFileName, Canvas& pCanvas)
{
	ArchiveFile lFile(pArchiveName);
	Status lStatus = STATUS_SUCCESS;

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

TiffLoader::Status TiffLoader::Save(const String& pArchiveName, const String& pFileName,
									const Canvas& pCanvas)
{
	ArchiveFile lFile(pArchiveName);
	Status lStatus = STATUS_SUCCESS;

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

TiffLoader::Status TiffLoader::Load(Reader& pReader, Canvas& pCanvas)
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

TiffLoader::Status TiffLoader::Save(Writer& pWriter, const Canvas& pCanvas)
{
	/*
		Write Image File Header (8 bytes)
	*/

	uint16 lVersion = 42;
	unsigned lIFDOffset = 8; // IFD = Image File Directory
	pWriter.WriteData("II", 2);
	pWriter.Write(lVersion);
	pWriter.Write(lIFDOffset);

	bool lStoreColorMap = pCanvas.GetBitDepth() == Canvas::BITDEPTH_8_BIT;
	
	// IFH = 8 bytes.
	// IFD = 6 + NumDEs * 12 bytes.
	int lOffset = lStoreColorMap ? 158 : 146;

	// Setup DEs to write.
	DirectoryEntry lDE[12];

	lDE[0].mTag = TAG_NEWSUBFILETYPE;
	lDE[0].mType = TYPE_LONG;
	lDE[0].mCount = 1;
	lDE[0].mValueOffset = 0;

	lDE[1].mTag = TAG_IMAGEWIDTH;
	lDE[1].mType = pCanvas.GetWidth() <= 32767? (uint16)TYPE_SHORT : (uint16)TYPE_LONG;
	lDE[1].mCount = 1;
	lDE[1].mValueOffset = (unsigned)pCanvas.GetWidth();

	lDE[2].mTag = TAG_IMAGELENGTH;
	lDE[2].mType = pCanvas.GetHeight() <= 32767? (uint16)TYPE_SHORT : (uint16)TYPE_LONG;
	lDE[2].mCount = 1;
	lDE[2].mValueOffset = (unsigned)pCanvas.GetHeight();

	lDE[3].mTag = TAG_BITSPERSAMPLE;
	lDE[3].mType = TYPE_SHORT;
	lDE[3].mCount = lStoreColorMap ? 1 : 3;
	lDE[3].mValueOffset = lDE[3].mCount == 1 ? 8 : lOffset + 0;

	lDE[4].mTag = TAG_COMPRESSION;
	lDE[4].mType = TYPE_SHORT;
	lDE[4].mCount = 1;
	lDE[4].mValueOffset = COMPRESSION_NONE;

	lDE[5].mTag = TAG_PHOTOMETRICLNTERP;
	lDE[5].mType = TYPE_SHORT;
	lDE[5].mCount = 1;
	lDE[5].mValueOffset = lStoreColorMap ? PI_PALETTE : PI_RGB;

	lDE[6].mTag = TAG_STRIPOFFSETS;
	lDE[6].mType = TYPE_SHORT;
	lDE[6].mCount = 1;
	lDE[6].mValueOffset = lStoreColorMap ? lOffset + sizeof(int16) * 768 : lOffset + sizeof(int16) * 3;

	lDE[7].mTag = TAG_SAMPLESPERPIXEL;
	lDE[7].mType = TYPE_SHORT;
	lDE[7].mCount = 1;
	lDE[7].mValueOffset = lStoreColorMap ? 1 : 3;

	lDE[8].mTag = TAG_ROWSPERSTRIP;
	lDE[8].mType = pCanvas.GetHeight() <= 32767? (uint16)TYPE_SHORT : (uint16)TYPE_LONG;
	lDE[8].mCount = 1;
	lDE[8].mValueOffset = pCanvas.GetHeight();

	int lStripSize = pCanvas.GetPixelByteSize() * pCanvas.GetHeight() * pCanvas.GetWidth();
	lDE[9].mTag = TAG_STRIPBYTECOUNTS;
	lDE[9].mType = lStripSize <= 32767? (uint16)TYPE_SHORT : (uint16)TYPE_LONG;
	lDE[9].mCount = 1;
	lDE[9].mValueOffset = lStripSize;

	lDE[10].mTag = TAG_PLANARCONFIGURATION;
	lDE[10].mType = TYPE_SHORT;
	lDE[10].mCount = 1;
	lDE[10].mValueOffset = PC_CHUNKY;

	lDE[11].mTag = TAG_COLORMAP;
	lDE[11].mType = TYPE_SHORT;
	lDE[11].mCount = 768;
	lDE[11].mValueOffset = lOffset;

	// Write DEs.
	uint16 lNumDEs = lStoreColorMap ? 12 : 11;
	pWriter.Write(lNumDEs);
	for (uint16 i = 0; i < lNumDEs; i++)
	{
		pWriter.Write(lDE[i].mTag);
		pWriter.Write(lDE[i].mType);
		pWriter.Write(lDE[i].mCount);
		pWriter.Write(lDE[i].mValueOffset);
	}

	// The end of this (and the only) Image File Directory.
	unsigned lNextIFDOffset = 0;
	pWriter.Write(lNextIFDOffset);

	// Now write all data pointed at by the DEs.
	if (lDE[3].mCount == 3)
	{
		uint16 lEight = 8;
		pWriter.Write(lEight);
		pWriter.Write(lEight);
		pWriter.Write(lEight);
	}

	// Write strip offset and strip.
	if (lStoreColorMap == true)
	{
		int i;
		for (i = 0; i < 256; i++)
		{
			uint16 lRed = (uint16)pCanvas.GetPalette()[i].mRed;
			lRed <<= 8;
			pWriter.Write(lRed);
		}
		for (i = 0; i < 256; i++)
		{
			uint16 lGreen = (uint16)pCanvas.GetPalette()[i].mGreen;
			lGreen <<= 8;
			pWriter.Write(lGreen);
		}
		for (i = 0; i < 256; i++)
		{
			uint16 lBlue = (uint16)pCanvas.GetPalette()[i].mBlue;
			lBlue <<= 8;
			pWriter.Write(lBlue);
		}
	}

	// Write strip (the actual image data).

	if (lStoreColorMap == false)
	{
		// Need to swap RGB.
		Canvas lCopy(pCanvas, true);
		lCopy.ConvertBitDepth(Canvas::BITDEPTH_24_BIT);
		lCopy.SwapRGBOrder();
		unsigned lRowSize = lCopy.GetPixelByteSize() * lCopy.GetWidth();
		unsigned lPitch = lCopy.GetPixelByteSize() * lCopy.GetPitch();
		uint8* lSrc = (uint8*)lCopy.GetBuffer();

		Color lColor;
		for (unsigned y = 0; y < lCopy.GetHeight(); y++)
		{
			pWriter.WriteData(lSrc, lRowSize);
			lSrc += lPitch;
		}
	}
	else
	{
		// No need to swap anything...
		unsigned lRowSize = pCanvas.GetPixelByteSize() * pCanvas.GetWidth();
		unsigned lPitch = pCanvas.GetPixelByteSize() * pCanvas.GetPitch();
		uint8* lSrc = (uint8*)pCanvas.GetBuffer();
		for (unsigned y = 0; y < pCanvas.GetHeight(); y++)
		{
			pWriter.WriteData(lSrc, lRowSize);
			lSrc += lPitch;
		}
	}

	return STATUS_SUCCESS;
}

TiffLoader::Status TiffLoader::Load(File& pFile, Canvas& pCanvas)
{
	/*
		Read Image File Header (8 bytes)
	*/

	uint8 lBuffer[2];

	// First two bytes tells us which byte order to use. M = Motorola, I = Intel.
	pFile.ReadData(lBuffer, 2);
	if (lBuffer[0] == 'M' &&
	   lBuffer[1] == 'M')
	{
		pFile.SetReaderEndian(Endian::TYPE_BIG_ENDIAN);
	}
	else if(lBuffer[0] == 'I' &&
		    lBuffer[1] == 'I')
	{
		pFile.SetReaderEndian(Endian::TYPE_LITTLE_ENDIAN);
	}
	else
	{
		return STATUS_READ_HEADER_ERROR;
	}

	// The next two bytes should always have the decimal value 42.
	uint16 lVersion;
	pFile.Read(lVersion);
	if (lVersion != 42)
	{
		return STATUS_READ_HEADER_ERROR;
	}

	// And finally read the Image File Directory Offset.
	unsigned lIFDOffset; // IFD = Image File Directory
	pFile.Read(lIFDOffset);
	pFile.SeekSet(lIFDOffset);

	// Load the first IFD, ignore the others.
	// (One IFD = one image, which means that one tiff file
	// can contain several images)

	Status lStatus = STATUS_SUCCESS;

	bool lDone = false;
	while (lDone == false)
	{
		IFDInfo lIFDInfo;
		lStatus = ReadIFD(pFile, lIFDInfo);

		bool lPixelFormatOK = lIFDInfo.CheckPixelFormat();
		bool lNoCompression = lIFDInfo.mCompression == COMPRESSION_NONE;

		if (lStatus != STATUS_SUCCESS)
		{
			lDone = true;
		}
		else if(lPixelFormatOK == true && lNoCompression == true)
		{
			if (lIFDInfo.mIsAlphaMask == false && 
			   lIFDInfo.mReducedResVersion == false)
			{
				ReadImage(pFile, lIFDInfo, pCanvas);
				lDone = true;
			}
		}
		else
		{
			// Check if we can continue and load next IFD.
			if (pFile.Tell() == 0)
			{
				// File position has been set to zero, which means
				// that we have reached the end of the file.
				lDone = true;

				if (lPixelFormatOK == false)
					lStatus = STATUS_PIXELFORMAT_ERROR;
				else
					lStatus = STATUS_COMPRESSION_ERROR;
			}
		}
	}

	return lStatus;
}

TiffLoader::Status TiffLoader::ReadHeader(const String& pFileName, Header& pHeader)
{
	Status lReturn = StartLoad(pFileName, pHeader);
	EndLoad();
	return lReturn;
}

TiffLoader::Status TiffLoader::ReadHeader(const String& pArchiveName, const String& pFileName, Header& pHeader)
{
	Status lReturn = StartLoad(pArchiveName, pFileName, pHeader);
	EndLoad();
	return lReturn;
}

TiffLoader::Status TiffLoader::StartLoad(const String& pArchiveName, const String& pFileName, Header& pHeader)
{
	Status lStatus = STATUS_SUCCESS;

	ArchiveFile* lFile = new ArchiveFile(pArchiveName);
	mLoadFile = lFile;

	if (lFile->Open(pFileName, ArchiveFile::READ_ONLY) == false)
	{
		lStatus = STATUS_OPEN_ERROR;
	}

	if (lStatus == STATUS_SUCCESS)
	{
		lStatus = StartLoad(pHeader);
	}

	return lStatus;
}

TiffLoader::Status TiffLoader::StartLoad(const String& pFileName, Header& pHeader)
{
	Status lStatus = STATUS_SUCCESS;

	DiskFile* lFile = new DiskFile;
	mLoadFile = lFile;

	if (lFile->Open(pFileName, DiskFile::MODE_READ) == false)
	{
		return STATUS_OPEN_ERROR;
	}

	if (lStatus == STATUS_SUCCESS)
	{
		lStatus = StartLoad(pHeader);
	}

	return lStatus;
}

TiffLoader::Status TiffLoader::StartLoad(Header& pHeader)
{
	//
	//	Read Image File Header (8 bytes)
	//

	uint8 lBuffer[2];

	// First two bytes tells us which byte order to use. M = Motorola, I = Intel.
	mLoadFile->ReadData(lBuffer, 2);
	if (lBuffer[0] == 'M' &&
	   lBuffer[1] == 'M')
	{
		mLoadFile->SetEndian(Endian::TYPE_BIG_ENDIAN);
	}
	else if(lBuffer[0] == 'I' &&
		    lBuffer[1] == 'I')
	{
		mLoadFile->SetEndian(Endian::TYPE_LITTLE_ENDIAN);
	}
	else
	{
		mLoadFile->Close();
		return STATUS_READ_HEADER_ERROR;
	}

	// The next two bytes should always have the decimal value 42.
	uint16 lVersion;
	mLoadFile->Read(lVersion);
	if (lVersion != 42)
	{
		mLoadFile->Close();
		return STATUS_READ_HEADER_ERROR;
	}

	// And finally read the Image File Directory Offset.
	unsigned lIFDOffset; // IFD = Image File Directory
	mLoadFile->Read(lIFDOffset);
	mLoadFile->SeekSet(lIFDOffset);

	// Load the first IFD, ignore the others.
	// (One IFD = one image, which means that one tiff file
	// can contain several images)

	Status lStatus = STATUS_SUCCESS;

	bool lDone = false;
	while (lDone == false)
	{
		lStatus = ReadIFD(*mLoadFile, mIFDInfo);

		bool lPixelFormatOK = mIFDInfo.CheckPixelFormat();
		bool lNoCompression = mIFDInfo.mCompression == COMPRESSION_NONE;

		if (lStatus != STATUS_SUCCESS)
		{
			lDone = true;
		}
		else if(lPixelFormatOK == true && lNoCompression == true)
		{
			if (mIFDInfo.mIsAlphaMask == false && 
			   mIFDInfo.mReducedResVersion == false)
			{
				pHeader.mWidth  = (int)mIFDInfo.mWidth;
				pHeader.mHeight = (int)mIFDInfo.mHeight;
				pHeader.mSamplesPerPixel = (int)mIFDInfo.mSamplesPerPixel;
				pHeader.mBitsPerPixel = (int)mIFDInfo.mBitsPerPixel;

				lDone = true;
			}
		}
		else
		{
			// Check if we can continue and load next IFD.
			if (mLoadFile->Tell() == 0)
			{
				// File position has been set to zero, which means
				// that we have reached the end of the file.
				lDone = true;

				if (lPixelFormatOK == false)
					lStatus = STATUS_PIXELFORMAT_ERROR;
				else
					lStatus = STATUS_COMPRESSION_ERROR;
			}
		}
	}

	if (lStatus != STATUS_SUCCESS)
	{
		mLoadFile->Close();
	}

	return lStatus;
}

TiffLoader::Status TiffLoader::LoadPiece(int pLeft, int pTop, int pRight, int pBottom, Canvas& pCanvas)
{
	ClipData lClipData(pLeft, pTop, pRight, pBottom, (int)mIFDInfo.mWidth, (int)mIFDInfo.mHeight);
	pCanvas.Reset(abs(pRight - pLeft), abs(pBottom - pTop), Canvas::IntToBitDepth(mIFDInfo.mBitsPerPixel));
	pCanvas.CreateBuffer();
	memset(pCanvas.GetBuffer(), 0, pCanvas.GetHeight() * pCanvas.GetPitch() * pCanvas.GetPixelByteSize());

	if (mIFDInfo.mPI == PI_WHITEISZERO)
	{
		// Generate grayscale palette.
		Color lPalette[256];
		for (int i = 0; i < 256; i++)
		{
			uint8 lCol = (uint8)i;
			lPalette[i].Set(lCol, lCol, lCol, lCol); 
		}

		pCanvas.SetPalette(lPalette);
	}
	else if(mIFDInfo.mPI == PI_BLACKISZERO)
	{
		// Generate inverted grayscale palette.
		Color lPalette[256];
		for (int i = 0; i < 256; i++)
		{
			uint8 lCol = 255 - (uint8)i;
			lPalette[i].Set(lCol, lCol, lCol, lCol); 
		}

		pCanvas.SetPalette(lPalette);
	}

	switch(mIFDInfo.mPI)
	{
	case PI_WHITEISZERO:
	case PI_BLACKISZERO:
		if (lClipData.mSrcReadWidth == 0 ||
			lClipData.mSrcReadHeight == 0)
			return STATUS_SUCCESS;

		return ReadMonochromeImage(lClipData, pCanvas);
	case PI_RGB:
		if (lClipData.mSrcReadWidth == 0 ||
			lClipData.mSrcReadHeight == 0)
			return STATUS_SUCCESS;

		return ReadColorImage(lClipData, pCanvas);
	case PI_PALETTE:
		if (lClipData.mSrcReadWidth == 0 ||
			lClipData.mSrcReadHeight == 0)
			return STATUS_SUCCESS;
	
		pCanvas.SetPalette(mIFDInfo.mPalette);
		
		// Monochrome images has the same pixel format as indexed ones...
		return ReadMonochromeImage(lClipData, pCanvas);
	default:
		return STATUS_READ_PICTURE_ERROR;
	}
}

void TiffLoader::EndLoad()
{
	mLoadFile->Close();
}

TiffLoader::Status TiffLoader::ReadIFD(File& pFile, IFDInfo& pIFDInfo)
{
	uint16 lEntryCount;
	pFile.Read(lEntryCount);

	uint16 i;
	for (i = 0; i < lEntryCount; i++)
	{
		// Read directory entry.
		DirectoryEntry lDE(pFile);

		Status lCurrentStatus = STATUS_SUCCESS;

		switch((Tag)lDE.mTag)
		{
		case TAG_BITSPERSAMPLE:
			{
				if (lDE.mType != TYPE_SHORT)
				{
					lCurrentStatus = STATUS_READ_PICTURE_ERROR;
				}
				else
				{
					pIFDInfo.mSamplesPerPixel = lDE.mCount;
					pIFDInfo.mSampleBits = new uint16[pIFDInfo.mSamplesPerPixel];
					pIFDInfo.mBitsPerPixel = 0;

					if (pIFDInfo.mSamplesPerPixel <= 2)
					{
						pIFDInfo.mSampleBits[0] = (uint16)(lDE.mValueOffset & 0xFFFF);
						pIFDInfo.mBitsPerPixel += pIFDInfo.mSampleBits[0];

						if (pIFDInfo.mSamplesPerPixel == 2)
						{
							pIFDInfo.mSampleBits[1] = (uint16)((lDE.mValueOffset >> 16) & 0xFFFF);
							pIFDInfo.mBitsPerPixel += pIFDInfo.mSampleBits[1];
						}
					}
					else
					{
						int64 lPrevPos = pFile.Tell();
						pFile.SeekSet(lDE.mValueOffset);

						for (unsigned i = 0; i < pIFDInfo.mSamplesPerPixel; i++)
						{
							pFile.Read(pIFDInfo.mSampleBits[i]);
							pIFDInfo.mBitsPerPixel += (unsigned)pIFDInfo.mSampleBits[i];
						}

						pFile.SeekSet(lPrevPos);
					}
				}
			}
			break;
		case TAG_COLORMAP:
			{
				pIFDInfo.mPaletteColorCount = (uint16)lDE.mCount / 3;
				pIFDInfo.mPalette = new Color[pIFDInfo.mPaletteColorCount];

				int64 lPrevPos = pFile.Tell();
				pFile.SeekSet(lDE.mValueOffset);

				uint16 i;
				for (i = 0; i < pIFDInfo.mPaletteColorCount; i++)
				{
					uint16 lRed;
					pFile.Read(lRed);
					pIFDInfo.mPalette[i].mRed  = (uint8)(lRed >> 8);
				}

				for (i = 0; i < pIFDInfo.mPaletteColorCount; i++)
				{
					uint16 lGreen;
					pFile.Read(lGreen);
					pIFDInfo.mPalette[i].mGreen  = (uint8)(lGreen >> 8);
				}

				for (i = 0; i < pIFDInfo.mPaletteColorCount; i++)
				{
					uint16 lBlue;
					pFile.Read(lBlue);
					pIFDInfo.mPalette[i].mBlue  = (uint8)(lBlue >> 8);
				}

				pFile.SeekSet(lPrevPos);
			}
			break;
		case TAG_EXTRASAMPLES:
			pIFDInfo.mExtraSamples = lDE.mCount;
			pIFDInfo.mExtra = (ExtraSamples)lDE.mValueOffset;
			break;
		case TAG_COMPRESSION:
			pIFDInfo.mCompression = (Compression)lDE.mValueOffset;
			break;
		case TAG_IMAGELENGTH:
			{
				if (lDE.mType == TYPE_SHORT ||
				   lDE.mType == TYPE_LONG)
				{
					pIFDInfo.mHeight = lDE.mValueOffset;
				}
				else
				{
					// Will initiate a return from this function.
					lCurrentStatus = STATUS_RESOLUTION_ERROR;
				}
			}
			break;
		case TAG_IMAGEWIDTH:
				if (lDE.mType == TYPE_SHORT ||
				   lDE.mType == TYPE_LONG)
				{
					pIFDInfo.mWidth = lDE.mValueOffset;
				}
				else
				{
					// Will initiate a return from this function.
					lCurrentStatus = STATUS_RESOLUTION_ERROR;
				}
			break;
		case TAG_NEWSUBFILETYPE:
			{
				pIFDInfo.mReducedResVersion = CheckBit(lDE.mValueOffset, 0);
				pIFDInfo.mIsSinglePage = CheckBit(lDE.mValueOffset, 1);
				pIFDInfo.mIsAlphaMask = CheckBit(lDE.mValueOffset, 2);
			}
			break;
		case TAG_PHOTOMETRICLNTERP:
			pIFDInfo.mPI = (PhotometricInterpretation)lDE.mValueOffset;
			break;
		case TAG_PLANARCONFIGURATION:
			pIFDInfo.mPlanarConfig = (PlanarConfig)lDE.mValueOffset;

			// Only support chunky data.
			if (pIFDInfo.mPlanarConfig != PC_CHUNKY)
			{
				// Will initiate a return from this function.
				lCurrentStatus = STATUS_READ_PICTURE_ERROR;
			}
			break;
		case TAG_PREDICTOR:
			break;
		case TAG_ROWSPERSTRIP:
			pIFDInfo.mRowsPerStrip = lDE.mValueOffset;
			break;
		case TAG_SAMPLESPERPIXEL:
			pIFDInfo.mSamplesPerPixel = lDE.mValueOffset;
			break;
		case TAG_STRIPBYTECOUNTS:
			pIFDInfo.mStripCount = lDE.mCount;
			pIFDInfo.mStripByteCounts = new unsigned[pIFDInfo.mStripCount];

			if (pIFDInfo.mStripCount == 1)
			{
				pIFDInfo.mStripByteCounts[0] = lDE.mValueOffset;
			}
			else
			{
				unsigned i;
				int64 lPrevPos = pFile.Tell();
				pFile.SeekSet(lDE.mValueOffset);

				if (lDE.mType == TYPE_SHORT)
				{
					uint16 lTemp;
					
					for (i = 0; i < pIFDInfo.mStripCount; i++)
					{
						pFile.Read(lTemp);
						pIFDInfo.mStripByteCounts[i] = (unsigned)lTemp;
					}
				}
				else if(lDE.mType == TYPE_LONG)
				{
					for (i = 0; i < pIFDInfo.mStripCount; i++)
					{
						pFile.Read(pIFDInfo.mStripByteCounts[i]);
					}
				}
				else
				{
					lCurrentStatus = STATUS_READ_INFO_ERROR;
				}

				pFile.SeekSet(lPrevPos);
			}
			break;
		case TAG_STRIPOFFSETS:
			if (pIFDInfo.mStripCount == 0)
			{
				pIFDInfo.mStripCount = lDE.mCount;
			}
			else if(pIFDInfo.mStripCount != lDE.mCount)
			{
				lCurrentStatus = STATUS_READ_INFO_ERROR;
			}

			pIFDInfo.mStripOffsets = new unsigned[pIFDInfo.mStripCount];

			if (pIFDInfo.mStripCount == 1)
			{
				pIFDInfo.mStripOffsets[0] = lDE.mValueOffset;
			}
			else
			{
				unsigned i;
				int64 lPrevPos = pFile.Tell();
				pFile.SeekSet(lDE.mValueOffset);

				if (lDE.mType == TYPE_SHORT)
				{
					uint16 lTemp;
					
					for (i = 0; i < pIFDInfo.mStripCount; i++)
					{
						pFile.Read(lTemp);
						pIFDInfo.mStripOffsets[i] = (unsigned)lTemp;
					}
				}
				else if(lDE.mType == TYPE_LONG)
				{
					for (i = 0; i < pIFDInfo.mStripCount; i++)
					{
						pFile.Read(pIFDInfo.mStripOffsets[i]);
					}
				}
				else
				{
					lCurrentStatus = STATUS_READ_INFO_ERROR;
				}

				pFile.SeekSet(lPrevPos);
			}

			break;
		case TAG_FILLORDER:
			pIFDInfo.mFillOrder = (FillOrder)lDE.mValueOffset;
			break;
		case TAG_ORIENTATION:
			pIFDInfo.mOrientation = (Orientation)lDE.mValueOffset;
			break;

		// Ignored tags.
		case TAG_MAKE:
		case TAG_COPYRIGHT:
		case TAG_ARTIST:
		case TAG_DATETIME:
		case TAG_FREEBYTECOUNTS:
		case TAG_FREEOFFSETS:
		case TAG_GRAYRESPONSECURVE:
		case TAG_GRAYRESPONSEUNIT:
		case TAG_COLORRESPONSECURVE:
		case TAG_HOSTCOMPUTER:
		case TAG_IMAGEDESCRIPTION:
		case TAG_MODEL:
		case TAG_MAXSAMPLEVALUE:
		case TAG_MINSAMPLEVALUE:
		case TAG_RESOLUTIONUNIT:
		case TAG_SOFTWARE:
		case TAG_THRESHOLDING:
		case TAG_XRESOLUTION:
		case TAG_YRESOLUTION:
		case TAG_SUBFILETYPE:
		case TAG_DOCUMENTNAME:
		case TAG_PAGENAME:
		case TAG_PAGENUMBER:
		case TAG_CELLLENGTH:
		case TAG_CELLWIDTH:
		case TAG_WHITEPOINT:
		case TAG_XPOSITION:
		case TAG_YPOSITION:
		case TAG_GROUP3OPTIONS:
		case TAG_GROUP4OPTIONS:
		case TAG_PRIMARYCHROMATICITIES:
		default:
			break;
		}

		if (lCurrentStatus != STATUS_SUCCESS)
		{
			return lCurrentStatus;
		}
	}

	// Jump to next IFD.
	unsigned lIFDOffset;
	pFile.Read(lIFDOffset);
	pFile.SeekSet(lIFDOffset);

	return STATUS_SUCCESS;
}

TiffLoader::Status TiffLoader::ReadImage(File& pFile, 
										 IFDInfo& pIFDInfo, 
										 Canvas& pCanvas)
{
	switch(pIFDInfo.mPI)
	{
	case PI_WHITEISZERO:
		return ReadMonochromeImage(pFile, pIFDInfo, pCanvas, true);
	case PI_BLACKISZERO:
		return ReadMonochromeImage(pFile, pIFDInfo, pCanvas, false);
	case PI_RGB:
		return ReadColorImage(pFile, pIFDInfo, pCanvas);
	case PI_PALETTE:
		return ReadIndexedImage(pFile, pIFDInfo, pCanvas);
	default:
		return STATUS_READ_PICTURE_ERROR;
	}
}

TiffLoader::Status TiffLoader::ReadMonochromeImage(File& pFile, 
												   IFDInfo& pIFDInfo, 
												   Canvas& pCanvas, 
												   bool pInvert)
{
	if (pIFDInfo.mSamplesPerPixel != 1)
	{
		return STATUS_READ_PICTURE_ERROR;
	}

	int64 lPrevPos = pFile.Tell();
	int lRowLength = 0; // Row length in bytes.

	pCanvas.Reset(pIFDInfo.mWidth, pIFDInfo.mHeight, Canvas::BITDEPTH_8_BIT);
	pCanvas.CreateBuffer();
	lRowLength = pIFDInfo.mWidth;

	uint8* lDest = (uint8*)pCanvas.GetBuffer();

	unsigned i;
	for (i = 0; i < pIFDInfo.mStripCount; i++)
	{
		pFile.SeekSet(pIFDInfo.mStripOffsets[i]);
		for (unsigned y = 0; y < pIFDInfo.mRowsPerStrip; y++)
		{
			// Read one row.
			pFile.ReadData(lDest, lRowLength);
			lDest += lRowLength;
		}
	}

	pFile.SeekSet(lPrevPos);

	// Generate grayscale palette.
	Color lPalette[256];
	for (i = 0; i < 256; i++)
	{
		uint8 lCol = (uint8)i;

		if (pInvert == true)
		{
			lCol = 255 - lCol;
		}

		lPalette[i].Set(lCol, lCol, lCol, lCol); 
	}

	pCanvas.SetPalette(lPalette);

	// Finally rotate or flip image into correct orientation.
	switch(pIFDInfo.mOrientation)
	{
	case ORIENTATION_TOPLEFT:
		// Do nothing.
		break;
	case ORIENTATION_TOPRIGHT:
		pCanvas.FlipHorizontal();
		break;
	case ORIENTATION_BOTTOMRIGHT:
		pCanvas.FlipHorizontal();
		pCanvas.FlipVertical();
		break;
	case ORIENTATION_BOTTOMLEFT:
		pCanvas.FlipVertical();
		break;
	case ORIENTATION_LEFTTOP:
		pCanvas.FlipVertical();
		pCanvas.Rotate90DegClockWise(1);
		break;
	case ORIENTATION_RIGHTTOP:
		pCanvas.Rotate90DegClockWise(3);
		break;
	case ORIENTATION_RIGHTBOTTOM:
		pCanvas.FlipHorizontal();
		pCanvas.Rotate90DegClockWise(3);
		break;
	case ORIENTATION_LEFTBOTTOM:
		pCanvas.Rotate90DegClockWise(1);
		break;
	default:
		break;
	}

	return STATUS_SUCCESS;
}

TiffLoader::Status TiffLoader::ReadIndexedImage(File& pFile, 
												IFDInfo& pIFDInfo, 
												Canvas& pCanvas)
{
	if (pIFDInfo.mSamplesPerPixel != 1)
	{
		return STATUS_READ_PICTURE_ERROR;
	}

	int64 lPrevPos = pFile.Tell();
	int lRowLength = 0; // Row length in bytes.

	pCanvas.Reset(pIFDInfo.mWidth, pIFDInfo.mHeight, Canvas::BITDEPTH_8_BIT);
	pCanvas.CreateBuffer();
	lRowLength = pIFDInfo.mWidth;

	uint8* lDest = (uint8*)pCanvas.GetBuffer();

	for (unsigned i = 0; i < pIFDInfo.mStripCount; i++)
	{
		pFile.SeekSet(pIFDInfo.mStripOffsets[i]);
		for (unsigned y = 0; y < pIFDInfo.mRowsPerStrip; y++)
		{
			// Read one row.
			pFile.ReadData(lDest, lRowLength);
			lDest += lRowLength;
		}
	}

	pFile.SeekSet(lPrevPos);

	pCanvas.SetPalette(pIFDInfo.mPalette);

	// Finally rotate or flip image into correct orientation.
	switch(pIFDInfo.mOrientation)
	{
	case ORIENTATION_TOPLEFT:
		// Do nothing.
		break;
	case ORIENTATION_TOPRIGHT:
		pCanvas.FlipHorizontal();
		break;
	case ORIENTATION_BOTTOMRIGHT:
		pCanvas.FlipHorizontal();
		pCanvas.FlipVertical();
		break;
	case ORIENTATION_BOTTOMLEFT:
		pCanvas.FlipVertical();
		break;
	case ORIENTATION_LEFTTOP:
		pCanvas.FlipVertical();
		pCanvas.Rotate90DegClockWise(1);
		break;
	case ORIENTATION_RIGHTTOP:
		pCanvas.Rotate90DegClockWise(3);
		break;
	case ORIENTATION_RIGHTBOTTOM:
		pCanvas.FlipHorizontal();
		pCanvas.Rotate90DegClockWise(3);
		break;
	case ORIENTATION_LEFTBOTTOM:
		pCanvas.Rotate90DegClockWise(1);
		break;
	default:
		break;
	}

	return STATUS_SUCCESS;
}

TiffLoader::Status TiffLoader::ReadColorImage(File& pFile, 
											  IFDInfo& pIFDInfo, 
											  Canvas& pCanvas)
{
	if (pIFDInfo.mSamplesPerPixel < 3)
	{
		return STATUS_READ_PICTURE_ERROR;
	}

	int64 lPrevPos = pFile.Tell();

	int lRowLength = 0; // Row length in bytes.
	pCanvas.Reset(pIFDInfo.mWidth, pIFDInfo.mHeight, Canvas::IntToBitDepth(pIFDInfo.mBitsPerPixel));
	pCanvas.CreateBuffer();
	lRowLength = pIFDInfo.mWidth * pCanvas.GetPixelByteSize();

	uint8* lDest = (uint8*)pCanvas.GetBuffer();

	for (unsigned i = 0; i < pIFDInfo.mStripCount; i++)
	{
		pFile.SeekSet(pIFDInfo.mStripOffsets[i]);
		for (unsigned y = 0; y < pIFDInfo.mRowsPerStrip; y++)
		{
			// Read one row.
			pFile.ReadData(lDest, lRowLength);
			lDest += lRowLength;
		}
	}

	pFile.SeekSet(lPrevPos);

	// Finally rotate or flip image into correct orientation.
	switch(pIFDInfo.mOrientation)
	{
	case ORIENTATION_TOPLEFT:
		// Do nothing.
		break;
	case ORIENTATION_TOPRIGHT:
		pCanvas.FlipHorizontal();
		break;
	case ORIENTATION_BOTTOMRIGHT:
		pCanvas.FlipHorizontal();
		pCanvas.FlipVertical();
		break;
	case ORIENTATION_BOTTOMLEFT:
		pCanvas.FlipVertical();
		break;
	case ORIENTATION_LEFTTOP:
		pCanvas.FlipVertical();
		pCanvas.Rotate90DegClockWise(1);
		break;
	case ORIENTATION_RIGHTTOP:
		pCanvas.Rotate90DegClockWise(3);
		break;
	case ORIENTATION_RIGHTBOTTOM:
		pCanvas.FlipHorizontal();
		pCanvas.Rotate90DegClockWise(3);
		break;
	case ORIENTATION_LEFTBOTTOM:
		pCanvas.Rotate90DegClockWise(1);
		break;
	default:
		break;
	}

	pCanvas.SwapRGBOrder();

	return STATUS_SUCCESS;
}

TiffLoader::Status TiffLoader::ReadMonochromeImage(ClipData& pClipData,
												   Canvas& pCanvas)
{
	if (mIFDInfo.mSamplesPerPixel != 1)
	{
		return STATUS_READ_PICTURE_ERROR;
	}

	int64 lPrevPos = mLoadFile->Tell();

	uint8* lDest = (uint8*)pCanvas.GetBuffer() + 
								pClipData.mDstYOffset * pCanvas.GetPitch() + 
								pClipData.mDstXOffset;

	int lStartStrip = pClipData.mSrcYOffset / mIFDInfo.mRowsPerStrip;
	int lStartLineWithinStrip = pClipData.mSrcYOffset % mIFDInfo.mRowsPerStrip;
	int lStartOffsetWithinStrip = lStartLineWithinStrip * mIFDInfo.mWidth + pClipData.mSrcXOffset;

	int lLinesRead = 0;

	unsigned i;
	for (i = lStartStrip; i < mIFDInfo.mStripCount && lLinesRead < pClipData.mSrcReadHeight; i++)
	{
		int lFileOffset = mIFDInfo.mStripOffsets[i] + lStartOffsetWithinStrip;
		
		lStartLineWithinStrip = 0;
		lStartOffsetWithinStrip = pClipData.mSrcXOffset;

		for (unsigned y = 0; y < mIFDInfo.mRowsPerStrip && lLinesRead < pClipData.mSrcReadHeight; y++)
		{
			mLoadFile->SeekSet(lFileOffset);

			// Read one row.
			if (mLoadFile->ReadData(lDest, pClipData.mSrcReadWidth) == 0)
			{
				return STATUS_READ_PICTURE_ERROR;
			}

			lDest += pCanvas.GetPitch();
			lFileOffset += mIFDInfo.mWidth;
			lLinesRead++;
		}
	}

	mLoadFile->SeekSet(lPrevPos);

	return STATUS_SUCCESS;
}


TiffLoader::Status TiffLoader::ReadColorImage(ClipData& pClipData, 
											  Canvas& pCanvas)
{
	if (mIFDInfo.mSamplesPerPixel != 3)
	{
		return STATUS_READ_PICTURE_ERROR;
	}

	int64 lPrevPos = mLoadFile->Tell();

	uint8* lDest = (uint8*)pCanvas.GetBuffer() + 
								(pClipData.mDstYOffset * pCanvas.GetPitch() + 
								pClipData.mDstXOffset) * 3;

	int lStartStrip = pClipData.mSrcYOffset / mIFDInfo.mRowsPerStrip;
	int lStartLineWithinStrip = pClipData.mSrcYOffset % mIFDInfo.mRowsPerStrip;
	int lStartOffsetWithinStrip = (lStartLineWithinStrip * mIFDInfo.mWidth + pClipData.mSrcXOffset) * 3;

	int lLinesRead = 0;

	unsigned i;
	for (i = lStartStrip; i < mIFDInfo.mStripCount && lLinesRead < pClipData.mSrcReadHeight; i++)
	{
		int lFileOffset = mIFDInfo.mStripOffsets[i] + lStartOffsetWithinStrip;
		
		lStartLineWithinStrip = 0;
		lStartOffsetWithinStrip = pClipData.mSrcXOffset * 3;

		for (unsigned y = 0; y < mIFDInfo.mRowsPerStrip && lLinesRead < pClipData.mSrcReadHeight; y++)
		{
			mLoadFile->SeekSet(lFileOffset);

			// Read one row.
			if (mLoadFile->ReadData(lDest, pClipData.mSrcReadWidth * 3) == 0)
			{
				return STATUS_READ_PICTURE_ERROR;
			}

			lDest += pCanvas.GetPitch() * 3;
			lFileOffset += mIFDInfo.mWidth * 3;
			lLinesRead++;
		}
	}

	mLoadFile->SeekSet(lPrevPos);
	pCanvas.SwapRGBOrder();

	return STATUS_SUCCESS;
}




TiffLoader::DirectoryEntry::DirectoryEntry() :
	mTag(0),
	mType(0),
	mCount(0),
	mValueOffset(0)
{
}

TiffLoader::DirectoryEntry::DirectoryEntry(File& pFile)
{
	pFile.Read(mTag);
	pFile.Read(mType);
	pFile.Read(mCount);
	pFile.Read(mValueOffset);

	if (pFile.GetReaderEndian() == Endian::TYPE_BIG_ENDIAN)
	{
		// Check if the data fits in ithe mValueOffset..
		int lTypeSize = 8; // Too high to fit...
		switch(mType)
		{
		case TYPE_BYTE:
		case TYPE_SBYTE:
		case TYPE_UNDEFINED:
			lTypeSize = 1;
			break;
		case TYPE_SHORT:
		case TYPE_SSHORT:
			lTypeSize = 2;
			break;
		case TYPE_LONG:
		case TYPE_SLONG:
		case TYPE_FLOAT:
			lTypeSize = 4;
			break;
		}

		if (mCount != 0 && lTypeSize * mCount <= 4)
		{
			// The data fits... Adjust mValueOffset to contain the right value.
			if (lTypeSize == 3)
			{
				mValueOffset >>= 8;
			}
			else if(lTypeSize == 2)
			{
				mValueOffset >>= 16;
			}
			else if(lTypeSize == 1)
			{
				mValueOffset >>= 24;
			}
		}
	}
}

TiffLoader::IFDInfo::IFDInfo() :
		mReducedResVersion(false),
		mIsSinglePage(false),
		mIsAlphaMask(false),
		mWidth(0),
		mHeight(0),
		mSamplesPerPixel(1),
		mSampleBits(0),
		mRowsPerStrip(0xFFFFFFFF),
		mStripByteCounts(0),
		mStripOffsets(0),
		mStripCount(0),
		mExtraSamples(0),
		mExtra(EXTRA_UNASSOCIATEDALPHA),
		mOrientation(ORIENTATION_DEFAULT),
		mCompression(COMPRESSION_DEFAULT),
		mFillOrder(FILLORDER_DEFAULT),
		mPI(PI_INVALID),
		mPlanarConfig(PC_CHUNKY),
		mPalette(0),
		mPaletteColorCount(0)
{
}

TiffLoader::IFDInfo::~IFDInfo()
{
	if (mPalette != 0)
	{
		delete[] mPalette;
	}

	if (mStripByteCounts != 0)
	{
		delete[] mStripByteCounts;
	}

	if (mSampleBits != 0)
	{
		delete[] mSampleBits;
	}
}


bool TiffLoader::IFDInfo::CheckPixelFormat()
{
	// Reject all weird pixel formats.
	// All "special" one-of-a-kind 
	// only-used-by-one-dude-in-the-whole-wide-world
	// pixel formats can go to hell.

	switch(mBitsPerPixel)
	{
	case 8:
		if (mSamplesPerPixel != 1)
			return false;
		break;
	case 15:
		if (mSamplesPerPixel != 3)
			return false;

		if (mSampleBits[0] != 5 ||
		mSampleBits[1] != 5 ||
		mSampleBits[2] != 5)
		{
			return false;
		}
		break;
	case 16:
		if (mSamplesPerPixel != 3)
			return false;

		if (mSampleBits[0] != 5 ||
		mSampleBits[1] != 6 ||
		mSampleBits[2] != 5)
		{
			return false;
		}
		break;
	case 24:
		if (mSamplesPerPixel != 3)
			return false;

		if (mSampleBits[0] != 8 ||
		mSampleBits[1] != 8 ||
		mSampleBits[2] != 8)
		{
			return false;
		}
		break;
	case 32:
		if (mSamplesPerPixel != 4)
			return false;

		if (mSampleBits[0] != 8 ||
		mSampleBits[1] != 8 ||
		mSampleBits[2] != 8 ||
		mSampleBits[3] != 8)
		{
			return false;
		}
		break;
	default:
		return false;
	}

	// If this is an indexed image, the palette must contain 256 colors.
	if (mPI == PI_PALETTE && mPaletteColorCount != 256)
		return false;

	// Check extras.
	if (mExtraSamples != 0 && mExtraSamples != 1)
		return false;

	if (mExtraSamples == 1 && mExtra != EXTRA_UNASSOCIATEDALPHA)
		return false;

	return true;
}



TiffLoader::ClipData::ClipData(int pLeft, int pTop, 
								  int pRight, int pBottom, 
								  int pDstWidth, int pDstHeight)
{
	if (pLeft > pRight)
	{
		int lTemp = pLeft;
		pLeft = pRight;
		pRight = lTemp;
	}

	if (pTop > pBottom)
	{
		int lTemp = pTop;
		pTop = pBottom;
		pBottom = lTemp;
	}

	if (pLeft > pDstWidth ||
	   pTop  > pDstHeight ||
	   pRight < 0 || pBottom < 0)
	{
		// No need to read anything.
		mSrcReadWidth = 0;
		mSrcReadHeight = 0; 
		return;
	}

	mDstXOffset = 0;
	mDstYOffset = 0;

	mSrcXOffset = pLeft;
	mSrcYOffset = pTop;

	if (mSrcXOffset < 0)
	{
		mDstXOffset = -mSrcXOffset;
		mSrcXOffset = 0;
	}

	if (mSrcYOffset < 0)
	{
		mDstYOffset = -mSrcYOffset;
		mSrcYOffset = 0;
	}

	mSrcReadWidth = pRight - mSrcXOffset;
	mSrcReadHeight = pBottom - mSrcYOffset;

	if ((mSrcXOffset + mSrcReadWidth) > pDstWidth)
	{
		mSrcReadWidth = pDstWidth - mSrcXOffset;
	}

	if ((mSrcYOffset + mSrcReadHeight) > pDstHeight)
	{
		mSrcReadHeight = pDstHeight - mSrcYOffset;
	}
}

} // End namespace.
