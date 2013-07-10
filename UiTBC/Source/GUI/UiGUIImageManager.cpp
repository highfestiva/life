
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



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

Painter::ImageID GUIImageManager::AddImage(const Canvas& pImage, ImageStyle pStyle, BlendFunc pBlendFunc, uint8 pAlphaValue)
{
	Canvas lImage(pImage, true);
	if (mSwapRGB == true)
	{
		lImage.SwapRGBOrder();
	}
	Painter::ImageID lID = mPainter->AddImage(&lImage, 0);
	AddLoadedImage(lImage, lID, pStyle, pBlendFunc, pAlphaValue);
	return lID;
}

void GUIImageManager::AddLoadedImage(const Canvas& pImage, Painter::ImageID pImageId, ImageStyle pStyle, BlendFunc pBlendFunc, uint8 pAlphaValue)
{
	deb_assert(!HasImage(pImageId));
	mImageTable.Insert(pImageId, new Image(pImageId, pImage, pStyle, pBlendFunc, pAlphaValue));
}

bool GUIImageManager::RemoveImage(Painter::ImageID pImageId)
{
	const bool lDropped = DropImage(pImageId);
	if (lDropped)
	{
		mPainter->RemoveImage(pImageId);
	}
	return lDropped;
}

bool GUIImageManager::DropImage(Painter::ImageID pImageId)
{
	ImageTable::Iterator lIter = mImageTable.Find(pImageId);
	if (lIter != mImageTable.End())
	{
		Image* lImage = *lIter;
		mImageTable.Remove(lIter);
		delete lImage;
		return true;
	}
	deb_assert(false);
	return false;
}

bool GUIImageManager::HasImage(Painter::ImageID pImageId) const
{
	return (mImageTable.Find(pImageId) != mImageTable.End());
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

Painter::ImageID GUIImageManager::GetImageID(const str& pImageName)
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
		deb_assert(lIter != mImageTable.End());
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

void GUIImageManager::DrawImage(Painter::ImageID pImageID, const PixelRect& pRect)
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

			PixelRect lRect(pRect);
			lRect.Offset(lImage->mXOffset, lImage->mYOffset);
			PixelCoord lPos(lRect.mLeft, lRect.mTop);

			switch(lImage->mStyle)
			{
			case TILED:
				mPainter->DrawImage(pImageID, lPos, PixelRect(0, 0, lRect.GetWidth(), lRect.GetHeight()));
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

PixelCoord GUIImageManager::GetImageSize(Painter::ImageID pImageID)
{
	PixelCoord lSize(0, 0);

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

bool GUIImageManager::IsOverImage(Painter::ImageID pImageID, int pScreenX, int pScreenY, const PixelRect& pScreenRect)
{
	if (pImageID != Painter::INVALID_IMAGEID)
	{
		ImageTable::Iterator lIter = mImageTable.Find(pImageID);
		if (lIter != mImageTable.End())
		{
			Image* lImage = *lIter;
			Canvas* lCanvas = &lImage->mCanvas;

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
				PixelCoord lTopLeft(pScreenRect.mLeft + (pScreenRect.GetWidth()  - lCanvas->GetWidth()) / 2,
									  pScreenRect.mTop  + (pScreenRect.GetHeight() - lCanvas->GetHeight()) / 2);
				PixelRect lImageRect(lTopLeft, lTopLeft + PixelCoord(lCanvas->GetWidth(), lCanvas->GetHeight()));
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
			
			Color lColor;
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
