/*
	Class:  GUIImageManager
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand
*/

#include "../../Include/GUI/UiGUIImageManager.h"
#include "../../../Lepra/Include/ParamLoader.h"
#include "../../../Lepra/Include/Log.h"
//#include "../../../Lepra/Include/ImageLoader.h"

namespace UiTbc
{

GUIImageManager::GUIImageManager() :
	mPainter(0),
	mSwapRGB(false)
{
}

GUIImageManager::~GUIImageManager()
{
	ClearImageTable();
}

void GUIImageManager::ClearImageTable()
{
	ImageTable::Iterator lIter;
	for (lIter = mImageTable.First(); lIter != mImageTable.End(); ++lIter)
	{
		Image* lImage = *lIter;

		if (mPainter != 0)
		{
			mPainter->RemoveImage(lImage->mID);
		}

		delete lImage;
	}

	mImageTable.RemoveAll();
}

void GUIImageManager::SetPainter(Painter* pPainter)
{
	if (mPainter != pPainter)
	{
		ClearImageTable();
	}

	mPainter = pPainter;
}


/*void GUIImageManager::LoadImages(const Lepra::tchar* pImageDefinitionFile, const Lepra::tchar* pArchive)
{
	Lepra::ParamLoader lParLoader;
	Lepra::Params lParams;

	// Load the image definition file.
	if (pArchive != 0)
	{
		lParLoader.Load(pArchive, pImageDefinitionFile, lParams);
	}
	else
	{
		lParLoader.Load(pImageDefinitionFile, lParams);
	}

	LoadStruct(&lParams, TILED, NO_BLEND, 255, Lepra::String(""));
}

void GUIImageManager::LoadStruct(Lepra::Params* pStruct, ImageStyle pStyle, BlendFunc pBlendFunc, int pAlpha, Lepra::String pArchive)
{
	// Constants...
	static const Lepra::String NULLSTRING(_T(""));
	static const Lepra::String ARCHIVE("archive");
	static const Lepra::String CFILE("cfile");
	static const Lepra::String AFILE("afile");
	static const Lepra::String STYLE("style");
	static const Lepra::String BLEND("blend");
	static const Lepra::String ALPHA("alpha");

	Lepra::String lArchive(pArchive);
	if (pStruct->ConditionalFind(ARCHIVE, Lepra::Params::VT_STRING) > 0)
	{
		lArchive = pStruct->GetStringValue(ARCHIVE);
	}

	Lepra::ImageLoader lImageLoader;
	Lepra::Canvas lImage;
	Lepra::Canvas lImageAlpha;	/// Alpha channel image.

	// Read the style...
	if (pStruct->ConditionalFind(STYLE, Lepra::Params::VT_STRING) > 0)
	{
		Lepra::String lStyleStr(pStruct->GetStringValue(STYLE));
		if (lStyleStr.CompareIgnoreCase("centered") == 0)
		{
			pStyle = CENTERED;
		}
		else if(lStyleStr.CompareIgnoreCase("stretched") == 0)
		{
			pStyle = STRETCHED;
		}
		else
		{
			pStyle = TILED;
		}
	}

	// Read the blend function.
	if (pStruct->ConditionalFind(BLEND, Lepra::Params::VT_STRING) > 0)
	{
		Lepra::String lBlendStr(pStruct->GetStringValue(BLEND));
		if (lBlendStr.CompareIgnoreCase("test") == 0)
		{
			pBlendFunc = ALPHATEST;
		}
		else if(lBlendStr.CompareIgnoreCase("blend") == 0)
		{
			pBlendFunc = ALPHABLEND;
		}
		else
		{
			pBlendFunc = NO_BLEND;
		}
	}

	// Read the alpha value.
	if (pStruct->ConditionalFind(ALPHA, Lepra::Params::VT_INT) > 0)
	{
		pAlpha = pStruct->GetIntValue(ALPHA);
		if (pAlpha < 0)
		{
			pAlpha = 0;
		}
		else if(pAlpha > 255)
		{
			pAlpha = 255;
		}
	}

	// This is a valid image definition as long as there is a filename.
	if (pStruct->ConditionalFind(CFILE, Lepra::Params::VT_STRING) > 0)
	{
		Lepra::String lFileName(pStruct->GetStringValue(CFILE));

		bool lLoaded = false;
		if (pArchive == NULLSTRING)
		{
			lLoaded = lImageLoader.Load(lFileName, lImage);
		}
		else
		{
			lLoaded = lImageLoader.Load(lArchive, lFileName, lImage);
		}

		if (lLoaded == true)
		{
			if (mSwapRGB == true)
			{
				lImage.SwapRGBOrder();
			}

			// Now check if we should merge this image with an alphachannel.
			if (pStruct->ConditionalFind(AFILE, Lepra::Params::VT_STRING) > 0)
			{
				Lepra::String lAlphaFileName(pStruct->GetStringValue(AFILE));

				lLoaded = false;
				if (pArchive == NULLSTRING)
				{
					lLoaded = lImageLoader.Load(lAlphaFileName, lImageAlpha);
				}
				else
				{
					lLoaded = lImageLoader.Load(lArchive, lAlphaFileName, lImageAlpha);
				}

				// Load the alpha image.
				if (lLoaded == true)
				{
					// Check if it is an 8-bit image.
					if (lImageAlpha.GetBitDepth() == Lepra::Canvas::BITDEPTH_8_BIT)
					{
						lImage.ConvertTo32BitWithAlpha(lImageAlpha);
					}
					else
					{
						Lepra::String lMessage = Lepra::StringUtility::Format(_T("Alpha image \"%s\" wasn't an 8-bit grayscale image!"), lAlphaFileName);
						log_error(_T("GUI"), lMessage);
					}
				}
				else
				{
					Lepra::String lMessage = Lepra::StringUtility::Format(_T("Couldn't load alpha image \"%s\""), lAlphaFileName);
					log_error(_T("GUI"), lMessage);
				}
			}

			Painter::ImageID lID = mPainter->AddImage(&lImage, 0);
			mIDTable.Insert(pStruct->GetName(), lID);
			mImageTable.Insert(lID, new Image(lID, lImage, pStyle, pBlendFunc, (Lepra::uint8)pAlpha));
		}
		else
		{
			Lepra::String lMessage = Lepra::StringUtility::Format(_T("Couldn't load image \"%s\""), lFileName);
			log_error(_T("GUI"), lMessage);
		}
	}

	Lepra::String lVarName;
	Lepra::Params::VariableType lVarType;
	int lCount;

	// Now load eventual substructs.
	bool lContinue = pStruct->GetFirstVariable(lVarName, lVarType, lCount);
	while (lContinue == true)
	{
		if (lVarType == Lepra::Params::VT_STRUCT)
		{
			LoadStruct(pStruct->GetStruct(lVarName), pStyle, pBlendFunc, pAlpha, lArchive);
		}
		lContinue = pStruct->GetNextVariable(lVarName, lVarType, lCount);
	}
}*/

Painter::ImageID GUIImageManager::AddImage(const Lepra::Canvas& pImage,
					   ImageStyle pStyle,
					   BlendFunc pBlendFunc,
					   Lepra::uint8 pAlphaValue)
{
	Lepra::Canvas lImage(pImage, true);

	if (mSwapRGB == true)
	{
		lImage.SwapRGBOrder();
	}

	Painter::ImageID lID = mPainter->AddImage(&lImage, 0);
	mImageTable.Insert(lID, new Image(lID, lImage, pStyle, pBlendFunc, pAlphaValue));
	return lID;
}

void GUIImageManager::SetImageOffset(Painter::ImageID pImageID, int pXOffset, int pYOffset)
{
	ImageTable::Iterator lIter = mImageTable.Find(pImageID);
	if (lIter != mImageTable.End())
	{
		Image* lImage = *lIter;
		lImage->mXOffset = pXOffset;
		lImage->mYOffset = pYOffset;
	}
}

Painter::ImageID GUIImageManager::GetImageID(const Lepra::String& pImageName)
{
	IDTable::Iterator lIter(mIDTable.Find(pImageName));
	if (lIter == mIDTable.End())
	{
		return Painter::INVALID_IMAGEID;
	}

	return *lIter;
}

void GUIImageManager::DrawImage(Painter::ImageID pImageID, int x, int y)
{
	if (pImageID != Painter::INVALID_IMAGEID)
	{
		ImageTable::Iterator lIter = mImageTable.Find(pImageID);
		if (lIter != mImageTable.End())
		{
			Image* lImage = *lIter;

			if (lImage->mBlendFunc == ALPHATEST)
			{
				mPainter->SetRenderMode(Painter::RM_ALPHATEST);
				mPainter->SetAlphaValue(lImage->mAlphaValue);
			}
			else if(lImage->mBlendFunc == ALPHABLEND)
			{
				mPainter->SetRenderMode(Painter::RM_ALPHABLEND);
				mPainter->SetAlphaValue(255);
			}
			else
			{
				mPainter->SetRenderMode(Painter::RM_NORMAL);
			}

			mPainter->DrawImage(pImageID, x + lImage->mXOffset, y + lImage->mYOffset);
		}
	}
}

void GUIImageManager::DrawImage(Painter::ImageID pImageID, const Lepra::PixelRect& pRect)
{
	if (pImageID != Painter::INVALID_IMAGEID)
	{
		ImageTable::Iterator lIter = mImageTable.Find(pImageID);
		if (lIter != mImageTable.End())
		{
			Image* lImage = *lIter;

			if (lImage->mBlendFunc == ALPHATEST)
			{
				mPainter->SetRenderMode(Painter::RM_ALPHATEST);
				mPainter->SetAlphaValue(lImage->mAlphaValue);
			}
			else if(lImage->mBlendFunc == ALPHABLEND)
			{
				mPainter->SetRenderMode(Painter::RM_ALPHABLEND);
				mPainter->SetAlphaValue(255);
			}
			else
			{
				mPainter->SetRenderMode(Painter::RM_NORMAL);
			}

			Lepra::PixelRect lRect(pRect);
			lRect.Offset(lImage->mXOffset, lImage->mYOffset);
			Lepra::PixelCoords lPos(lRect.mLeft, lRect.mTop);

			switch(lImage->mStyle)
			{
			case TILED:
				mPainter->DrawImage(pImageID, lPos, Lepra::PixelRect(0, 0, lRect.GetWidth(), lRect.GetHeight()));
				break;
			case CENTERED:
				mPainter->DrawImage(pImageID, 
					(int)lPos.x + ((int)lRect.GetWidth()  - (int)lImage->mCanvas.GetWidth())  / 2,
					(int)lPos.y + ((int)lRect.GetHeight() - (int)lImage->mCanvas.GetHeight()) / 2);
				break;
			case STRETCHED:
				mPainter->DrawImage(pImageID, lRect);
				break;
			default:
				break;
			}
		}
	}
}

Lepra::PixelCoords GUIImageManager::GetImageSize(Painter::ImageID pImageID)
{
	Lepra::PixelCoords lSize(0, 0);

	if (pImageID != Painter::INVALID_IMAGEID)
	{
		ImageTable::Iterator lIter = mImageTable.Find(pImageID);
		if (lIter != mImageTable.End())
		{
			Image* lImage = *lIter;
			lSize.x = lImage->mCanvas.GetWidth();
			lSize.y = lImage->mCanvas.GetHeight();
		}
	}

	return lSize;
}

bool GUIImageManager::IsOverImage(Painter::ImageID pImageID, int pScreenX, int pScreenY, const Lepra::PixelRect& pScreenRect)
{
	if (pImageID != Painter::INVALID_IMAGEID)
	{
		ImageTable::Iterator lIter = mImageTable.Find(pImageID);
		if (lIter != mImageTable.End())
		{
			Image* lImage = *lIter;
			Lepra::Canvas* lCanvas = &lImage->mCanvas;

			if (lImage->mBlendFunc == NO_BLEND)
			{
				return pScreenRect.IsInside(pScreenX, pScreenY);
			}

			// Calculate the image's pixel coordinates.
			int x = pScreenX - pScreenRect.mLeft;
			int y = pScreenY - pScreenRect.mTop;

			switch(lImage->mStyle)
			{
			case TILED:
				x %= lCanvas->GetWidth();
				y %= lCanvas->GetHeight();
				break;
			case CENTERED:
			{
				Lepra::PixelCoords lTopLeft(pScreenRect.mLeft + (pScreenRect.GetWidth()  - lCanvas->GetWidth()) / 2,
									  pScreenRect.mTop  + (pScreenRect.GetHeight() - lCanvas->GetHeight()) / 2);
				Lepra::PixelRect lImageRect(lTopLeft, lTopLeft + Lepra::PixelCoords(lCanvas->GetWidth(), lCanvas->GetHeight()));
				if (lImageRect.IsInside(pScreenX, pScreenY) == false)
				{
					return false;
				}

				x = pScreenX - lTopLeft.x;
				y = pScreenY - lTopLeft.y;
			}
				break;
			case STRETCHED:
			{
				x = (x * lCanvas->GetWidth())  / pScreenRect.GetWidth();
				y = (y * lCanvas->GetHeight()) / pScreenRect.GetHeight();
			}
				break;
			default:
				return false;
			}
			
			Lepra::Color lColor;
			lCanvas->GetPixelColor(x, y, lColor);

			return (lColor.mAlpha >= lImage->mAlphaValue);
		}
	}

	return false;
}

} // End namespace.
