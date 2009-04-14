
// Author: Alexander Hugestrand
// Copyright (c) 2002-2008, Righteous Games

#include "../Include/BmpLoader.h"
#include "../Include/ImageLoader.h"
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

bool ImageLoader::Load(const String& pFileName, Canvas& pCanvas)
{
	bool lOk = false;
	String lFileExtension = Path::GetExtension(pFileName);
	if (!lFileExtension.empty())
	{
		if (StringUtility::CompareIgnoreCase(lFileExtension, _T("bmp")) == 0)
		{
			BmpLoader lBmpLoader;
			lOk = (lBmpLoader.Load(pFileName, pCanvas) == BmpLoader::STATUS_SUCCESS);
		}
		else if(StringUtility::CompareIgnoreCase(lFileExtension, _T("tga")) == 0)
		{
			TgaLoader lTgaLoader;
			lOk = (lTgaLoader.Load(pFileName, pCanvas) == TgaLoader::STATUS_SUCCESS);
		}
		else if(StringUtility::CompareIgnoreCase(lFileExtension, _T("tif")) == 0 || StringUtility::CompareIgnoreCase(lFileExtension, _T("tiff")) == 0)
		{
			TiffLoader lTiffLoader;
			lOk = (lTiffLoader.Load(pFileName, pCanvas) == TiffLoader::STATUS_SUCCESS);
		}
		else if(StringUtility::CompareIgnoreCase(lFileExtension, _T("jpg")) == 0 || StringUtility::CompareIgnoreCase(lFileExtension, _T("jpeg")) == 0)
		{
			JpegLoader lJpegLoader;
			lOk = (lJpegLoader.Load(pFileName, pCanvas) == JpegLoader::STATUS_SUCCESS);
		}
		else if(StringUtility::CompareIgnoreCase(lFileExtension, _T("png")) == 0 || StringUtility::CompareIgnoreCase(lFileExtension, _T("pngf")) == 0)
		{
			PngLoader lPngLoader;
			lOk = (lPngLoader.Load(pFileName, pCanvas) == PngLoader::STATUS_SUCCESS);
		}
		else
		{
			lOk = false;
		}
	}
	return (lOk);
}

bool ImageLoader::Save(const String& pFileName, const Canvas& pCanvas)
{
	String lFileExtension = Path::GetExtension(pFileName);

	if (StringUtility::CompareIgnoreCase(lFileExtension, _T("bmp")) == 0)
	{
		BmpLoader lBmpLoader;
		return lBmpLoader.Save(pFileName, pCanvas) == BmpLoader::STATUS_SUCCESS;
	}
	else if(StringUtility::CompareIgnoreCase(lFileExtension, _T("tga")) == 0)
	{
		TgaLoader lTgaLoader;
		return lTgaLoader.Save(pFileName, pCanvas) == TgaLoader::STATUS_SUCCESS;
	}
	else if(StringUtility::CompareIgnoreCase(lFileExtension, _T("tif")) == 0 || StringUtility::CompareIgnoreCase(lFileExtension, _T("tiff")) == 0)
	{
		TiffLoader lTiffLoader;
		return lTiffLoader.Save(pFileName, pCanvas) == TiffLoader::STATUS_SUCCESS;
	}
	else if(StringUtility::CompareIgnoreCase(lFileExtension, _T("jpg")) == 0 || StringUtility::CompareIgnoreCase(lFileExtension, _T("jpeg")) == 0)
	{
		JpegLoader lJpegLoader;
		return lJpegLoader.Save(pFileName, pCanvas) == JpegLoader::STATUS_SUCCESS;
	}
	else if(StringUtility::CompareIgnoreCase(lFileExtension, _T("png")) == 0 || StringUtility::CompareIgnoreCase(lFileExtension, _T("pngf")) == 0)
	{
		PngLoader lPngLoader;
		return lPngLoader.Save(pFileName, pCanvas) == PngLoader::STATUS_SUCCESS;
	}
	else
	{
		return false;
	}
}

bool ImageLoader::Load(const String& pArchiveName, const String& pFileName, Canvas& pCanvas)
{
	String lFileExtension = Path::GetExtension(pFileName);

	if (StringUtility::CompareIgnoreCase(lFileExtension, _T("bmp")) == 0)
	{
		BmpLoader lBmpLoader;
		return lBmpLoader.Load(pArchiveName, pFileName, pCanvas) == BmpLoader::STATUS_SUCCESS;
	}
	else if(StringUtility::CompareIgnoreCase(lFileExtension, _T("tga")) == 0)
	{
		TgaLoader lTgaLoader;
		return lTgaLoader.Load(pArchiveName, pFileName, pCanvas) == TgaLoader::STATUS_SUCCESS;
	}
	else if(StringUtility::CompareIgnoreCase(lFileExtension, _T("tif")) == 0 || StringUtility::CompareIgnoreCase(lFileExtension, _T("tiff")) == 0)
	{
		TiffLoader lTiffLoader;
		return lTiffLoader.Load(pArchiveName, pFileName, pCanvas) == TiffLoader::STATUS_SUCCESS;
	}
	else if(StringUtility::CompareIgnoreCase(lFileExtension, _T("jpg")) == 0 || StringUtility::CompareIgnoreCase(lFileExtension, _T("jpeg")) == 0)
	{
		JpegLoader lJpegLoader;
		return lJpegLoader.Load(pArchiveName, pFileName, pCanvas) == JpegLoader::STATUS_SUCCESS;
	}
	else if(StringUtility::CompareIgnoreCase(lFileExtension, _T("png")) == 0 || StringUtility::CompareIgnoreCase(lFileExtension, _T("pngf")) == 0)
	{
		PngLoader lPngLoader;
		return lPngLoader.Load(pArchiveName, pFileName, pCanvas) == PngLoader::STATUS_SUCCESS;
	}
	else
	{
		return false;
	}
}

bool ImageLoader::Save(const String& pArchiveName, const String& pFileName, const Canvas& pCanvas)
{
	String lFileExtension = Path::GetExtension(pFileName);

	if (StringUtility::CompareIgnoreCase(lFileExtension, _T("bmp")) == 0)
	{
		BmpLoader lBmpLoader;
		return lBmpLoader.Save(pArchiveName, pFileName, pCanvas) == BmpLoader::STATUS_SUCCESS;
	}
	else if(StringUtility::CompareIgnoreCase(lFileExtension, _T("tga")) == 0)
	{
		TgaLoader lTgaLoader;
		return lTgaLoader.Save(pArchiveName, pFileName, pCanvas) == TgaLoader::STATUS_SUCCESS;
	}
	else if(StringUtility::CompareIgnoreCase(lFileExtension, _T("tif")) == 0 || StringUtility::CompareIgnoreCase(lFileExtension, _T("tiff")) == 0)
	{
		TiffLoader lTiffLoader;
		return lTiffLoader.Save(pArchiveName, pFileName, pCanvas) == TiffLoader::STATUS_SUCCESS;
	}
	else if(StringUtility::CompareIgnoreCase(lFileExtension, _T("jpg")) == 0 || StringUtility::CompareIgnoreCase(lFileExtension, _T("jpeg")) == 0)
	{
		JpegLoader lJpegLoader;
		return lJpegLoader.Save(pArchiveName, pFileName, pCanvas) == JpegLoader::STATUS_SUCCESS;
	}
	else if(StringUtility::CompareIgnoreCase(lFileExtension, _T("png")) == 0 || StringUtility::CompareIgnoreCase(lFileExtension, _T("pngf")) == 0)
	{
		PngLoader lPngLoader;
		return lPngLoader.Save(pArchiveName, pFileName, pCanvas) == PngLoader::STATUS_SUCCESS;
	}
	else
	{
		return false;
	}
}

bool ImageLoader::Load(FileType pFileType, Reader& pReader, Canvas& pCanvas)
{
	switch(pFileType)
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
		default:
		return false;
	}
}

} // End namespace.
