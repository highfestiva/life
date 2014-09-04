/*
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

	NOTES:

	Loads tex-files into a Canvas.

	TEX (Texture) is partially my own texture file format,
	specially designed to use with games. It doesn't only store 
	a color map and eventually an alpha-channel as other file 
	formats, but specular maps, normal maps, prefabricated 
	mip maps etc as well.

	All image dimensions are a power of 2, and nothing else.

	The TEX file format is a plain and simple raw data file format,
	with no mubo jumbo information stored all over the file, as in 
	standardized file formats like TIFF or TGA.

	TEX-standard is that the data is stored either as raw uncompressed
	24- or 8-bit (color or monochrome) data, or compressed using
	jpeg-compression.

	TEX uses Intel little endian byte ordering.
*/



#pragma once

#include "../../Lepra/Include/DiskFile.h"
#include "../../Lepra/Include/Canvas.h"
#include "../Include/UiTbc.h"



struct jpeg_decompress_struct;
struct jpeg_compress_struct;



namespace UiTbc
{



class Texture;



class TEXLoader
{
public:
	friend class TEXFriend;
	
	enum IoStatus
	{
		STATUS_OPEN_ERROR = 0, 
		STATUS_SUCCESS,
		STATUS_MISSING_COLORMAP_ERROR,
		STATUS_MEMORY_ERROR,
		STATUS_READ_HEADER_ERROR,
		STATUS_READ_INFO_ERROR,
		STATUS_READ_PALETTE_ERROR,
		STATUS_READ_PICTURE_ERROR,
		STATUS_RESOLUTION_ERROR,
		STATUS_COMPRESSION_ERROR,
	};

	IoStatus Load(const str& pFileName, Texture& pTexture, bool pMergeColorAndAlpha);
	IoStatus Save(const str& pFileName, const Texture& pTexture, bool pCompressed);

	IoStatus Load(const str& pArchiveName, const str& pFileName, Texture& pTexture, bool pMergeColorAndAlpha);
	IoStatus Save(const str& pArchiveName, const str& pFileName, const Texture& pTexture, bool pCompressed);

protected:
private:

	IoStatus Load(Texture& pTexture, bool pMergeColorAndAlpha);
	IoStatus Save(const Texture& pTexture, bool pCompressed);

	enum // Map flags
	{
		ALPHA_MAP    = (1 << 0), // 8-bit alpha channel.
		NORMAL_MAP   = (1 << 1), // 24-bit XYZ normal map, 32-bit XYZS if combined with specular map.
		SPECULAR_MAP = (1 << 2), // 8-bit specular map. Merged with normal map.
		CUBE_MAP     = (1 << 3), // 6 24-bit color maps ONLY. Not possible to combine with the 
					 // previous flags. The color maps are stored in the
					 // following order: PosX, NegX, PosY, NegY, PosZ and NegZ.
	};

	class FileHeader
	{
	public:
		char mTEXMagic[4];
		unsigned short mVersion;
		unsigned char mDimensionPowers;   // Image height = 1 << "upper 4 bits"
						     // Image width  = 1 << "lower 4 bits"
		unsigned char mCompressionFlag;   // 1 if using jpeg compression, 0 otherwise.
		unsigned mMapFlags;
		
		// Offset into the file where the color data can be found.
		unsigned mDataOffset;

		// Data members not stored in the file...
		int mWidth;
		int mHeight;
		int mNumMipMapLevels;

		inline bool ReadHeader(File* pFile)
		{
			if (pFile->ReadData(mTEXMagic, 4) != IO_OK ||
			   pFile->Read(mVersion) != IO_OK)
			{
				return false;
			}
			
			if (pFile->Read(mDataOffset) != IO_OK ||
			   pFile->Read(mDimensionPowers) != IO_OK ||
			   pFile->Read(mCompressionFlag) != IO_OK ||
			   pFile->Read(mMapFlags) != IO_OK)
			{
				return false;
			}

			if (mTEXMagic[0] != 'T' ||
			   mTEXMagic[1] != 'T' ||
			   mTEXMagic[2] != 'E' ||
			   mTEXMagic[3] != 'X')
			{
				return false;
			}

			if (mCompressionFlag != 0 &&
			   mCompressionFlag != 1)
			{
				return false;
			}

			// Get the texture dimensions.

			int lLog2Width  = (mDimensionPowers & 0x0F);
			int lLog2Height = ((mDimensionPowers >> 4) & 0x0F);
			mWidth  = (1 << lLog2Width);
			mHeight = (1 << lLog2Height);

			mNumMipMapLevels = lLog2Width;
			if (lLog2Height > lLog2Width)
			{
				mNumMipMapLevels = lLog2Height;
			}
			mNumMipMapLevels++;

			return true;
		}

		inline bool CheckMapFlag(unsigned long pFlag)
		{
			return ((mMapFlags & pFlag) != 0);
		}
	};

	IoStatus ReadJpeg(Canvas& pCanvas);
	IoStatus WriteJpeg(const Canvas& pCanvas);

	void InitSourceManager(jpeg_decompress_struct* pCInfo);
	void InitDestinationManager(jpeg_compress_struct* pCInfo);

	File* mLoadFile;
	File* mSaveFile;
};



}
