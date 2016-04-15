
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/ImageLoader.h"
#include "../Include/LepraAssert.h"
#include "../Include/BmpLoader.h"
#include "../Include/DiskFile.h"
#include "../Include/JpegLoader.h"
#include "../Include/Path.h"
#include "../Include/PngLoader.h"
#include "../Include/String.h"
#include "../Include/TgaLoader.h"
#include "../Include/TiffLoader.h"



namespace Lepra
{



ImageLoader::ImageLoader()
{
}

ImageLoader::~ImageLoader()
{
}



ImageLoader::FileType ImageLoader::GetFileTypeFromName(const str& pFilename)
{
	str lFileExtension = Path::GetExtension(pFilename);
	strutil::ToLower(lFileExtension);
	if (lFileExtension == "bmp")
	{
		return BMP;
	}
	else if (lFileExtension ==  "tga")
	{
		return TGA;
	}
	else if (lFileExtension == "tif" || lFileExtension == "tiff")
	{
		return TIF;
	}
	else if (lFileExtension == "jpg" || lFileExtension == "jpeg")
	{
		return JPG;
	}
	else if (lFileExtension == "png")
	{
		return PNG;
	}
	return UNKNOWN;
}



bool ImageLoader::Load(const str& pFileName, Canvas& pCanvas)
{
	DiskFile lFile;
	if (!lFile.Open(pFileName, DiskFile::MODE_READ))
	{
		return false;
	}
	return Load(GetFileTypeFromName(pFileName), lFile, pCanvas);
}

bool ImageLoader::Save(const str& pFileName, const Canvas& pCanvas)
{
	DiskFile lFile;
	if (!lFile.Open(pFileName, DiskFile::MODE_WRITE))
	{
		return false;
	}
	return Save(GetFileTypeFromName(pFileName), lFile, pCanvas);
}



bool ImageLoader::Load(FileType pFileType, Reader& pReader, Canvas& pCanvas)
{
	switch (pFileType)
	{
		case BMP:
		{
			BmpLoader lBmpLoader;
			return lBmpLoader.Load(pReader, pCanvas) == BmpLoader::STATUS_SUCCESS;
		}
		break;
		case TGA:
		{
			TgaLoader lTgaLoader;
			return lTgaLoader.Load(pReader, pCanvas) == TgaLoader::STATUS_SUCCESS;
		}
		break;
		case TIF:
		{
			TiffLoader lTiffLoader;
			return lTiffLoader.Load(pReader, pCanvas) == TiffLoader::STATUS_SUCCESS;
		}
		break;
		case JPG:
		{
			JpegLoader lJpegLoader;
			return lJpegLoader.Load(pReader, pCanvas) == JpegLoader::STATUS_SUCCESS;
		}
		break;
		case PNG:
		{
			PngLoader lPngLoader;
			return lPngLoader.Load(pReader, pCanvas) == PngLoader::STATUS_SUCCESS;
		}
		break;
		default:
		return false;
	}
}

bool ImageLoader::Save(FileType pFileType, Writer& pWriter, const Canvas& pCanvas)
{
	switch(pFileType)
	{
		case BMP:
		{
			BmpLoader lBmpLoader;
			return lBmpLoader.Save(pWriter, pCanvas) == BmpLoader::STATUS_SUCCESS;
		}
		break;
		case TGA:
		{
			TgaLoader lTgaLoader;
			return lTgaLoader.Save(pWriter, pCanvas) == TgaLoader::STATUS_SUCCESS;
		}
		break;
		case TIF:
		{
			TiffLoader lTiffLoader;
			return lTiffLoader.Save(pWriter, pCanvas) == TiffLoader::STATUS_SUCCESS;
		}
		break;
		case JPG:
		{
			JpegLoader lJpegLoader;
			return lJpegLoader.Save(pWriter, pCanvas) == JpegLoader::STATUS_SUCCESS;
		}
		break;
		case PNG:
		{
			PngLoader lPngLoader;
			return lPngLoader.Save(pWriter, pCanvas) == PngLoader::STATUS_SUCCESS;
		}
		break;
	}
	return false;
}



}
