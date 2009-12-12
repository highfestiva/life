
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#include "../../Include/GUI/UiGUIImageManager.h"
#include "../../../Lepra/Include/Log.h"



namespace UiTbc
{



GUIImageManager::GUIImageManager():
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

Painter::ImageID GUIImageManager::AddImage(const Lepra::Canvas& pImage, ImageStyle pStyle, BlendFunc pBlendFunc, Lepra::uint8 pAlphaValue)
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



Painter* GUIImageManager::GetPainter() const
{
	return mPainter;
}

void GUIImageManager::SwapRGB()
{
	mSwapRGB = !mSwapRGB;
}



}
