/*
	Lepra::File:   TEXLoader.h
	Class:  TEXLoader
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games

	NOTES:

	Loads tex-files into a Lepra::Canvas.

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

#ifndef UITEXLOADER_H
#define UITEXLOADER_H

#include "../../Lepra/Include/DiskFile.h"
#include "../../Lepra/Include/Canvas.h"

struct jpeg_decompress_struct;
struct jpeg_compress_struct;

namespace UiTbc
{

class Texture;

class TEXLoader
{
public:
	friend class TEXFriend;
	
	enum Status
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

	Status Load(const Lepra::String& pFileName, Texture& pTexture, bool pMergeColorAndAlpha);
	Status Save(const Lepra::String& pFileName, const Texture& pTexture, bool pCompressed);

	Status Load(const Lepra::String& pArchiveName, const Lepra::String& pFileName, Texture& pTexture, bool pMergeColorAndAlpha);
	Status Save(const Lepra::String& pArchiveName, const Lepra::String& pFileName, const Texture& pTexture, bool pCompressed);

protected:
private:

	Status Load(Texture& pTexture, bool pMergeColorAndAlpha);
	Status Save(const Texture& pTexture, bool pCompressed);

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

		inline bool ReadHeader(Lepra::File* pFile)
		{
			if (pFile->ReadData(mTEXMagic, 4) != Lepra::IO_OK ||
			   pFile->Read(mVersion) != Lepra::IO_OK)
			{
				return false;
			}
			
			if (pFile->Read(mDataOffset) != Lepra::IO_OK ||
			   pFile->Read(mDimensionPowers) != Lepra::IO_OK ||
			   pFile->Read(mCompressionFlag) != Lepra::IO_OK ||
			   pFile->Read(mMapFlags) != Lepra::IO_OK)
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

	Status ReadJpeg(Lepra::Canvas& pCanvas);
	Status WriteJpeg(const Lepra::Canvas& pCanvas);

	void InitSourceManager(jpeg_decompress_struct* pCInfo);
	void InitDestinationManager(jpeg_compress_struct* pCInfo);

	Lepra::File* mLoadFile;
	Lepra::File* mSaveFile;
};

} // End namespace.

#endif
