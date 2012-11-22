/*
	File:   Texture.cpp
	Class:  Texture
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games
*/

#include "../Include/UiTBC.h"
#include "../Include/UiTexture.h"
#include "../../Lepra/Include/Canvas.h"
#include <math.h>

namespace UiTbc
{

Texture::Texture():
	mNumMipMapLevels(0),
	mIsCubeMap(false),
	mColorMap(0),
	mAlphaMap(0),
	mNormalMap(0),
	mSpecularMap(0),
	mCubeMapPosX(0),
	mCubeMapNegX(0),
	mCubeMapPosY(0),
	mCubeMapNegY(0),
	mCubeMapPosZ(0),
	mCubeMapNegZ(0)
{
}

Texture::Texture(const Canvas& pColorMap, Canvas::ResizeHint pResizeHint, int pGenerateMipMapLevels):
	mNumMipMapLevels(0),
	mIsCubeMap(false),
	mColorMap(0),
	mAlphaMap(0),
	mNormalMap(0),
	mSpecularMap(0),
	mCubeMapPosX(0),
	mCubeMapNegX(0),
	mCubeMapPosY(0),
	mCubeMapNegY(0),
	mCubeMapPosZ(0),
	mCubeMapNegZ(0)
{
	Set(pColorMap, pResizeHint, pGenerateMipMapLevels);
}

Texture::Texture(Texture* pTexture):
	mNumMipMapLevels(0),
	mIsCubeMap(false),
	mColorMap(0),
	mAlphaMap(0),
	mNormalMap(0),
	mSpecularMap(0),
	mCubeMapPosX(0),
	mCubeMapNegX(0),
	mCubeMapPosY(0),
	mCubeMapNegY(0),
	mCubeMapPosZ(0),
	mCubeMapNegZ(0)
{
	Copy(pTexture);
}

Texture::~Texture()
{
	ClearAll();
}

void Texture::ClearAll()
{
	if (mColorMap != 0)
	{
		delete[] mColorMap;
		mColorMap = 0;
	}

	if (mAlphaMap != 0)
	{
		delete[] mAlphaMap;
		mAlphaMap = 0;
	}

	if (mNormalMap != 0)
	{
		delete[] mNormalMap;
		mNormalMap = 0;
	}

	if (mSpecularMap != 0)
	{
		delete[] mSpecularMap;
		mSpecularMap = 0;
	}

	if (mCubeMapPosX != 0)
	{
		delete[] mCubeMapPosX;
		mCubeMapPosX = 0;
	}

	if (mCubeMapNegX != 0)
	{
		delete[] mCubeMapNegX;
		mCubeMapNegX = 0;
	}

	if (mCubeMapPosY != 0)
	{
		delete[] mCubeMapPosY;
		mCubeMapPosY = 0;
	}

	if (mCubeMapNegY != 0)
	{
		delete[] mCubeMapNegY;
		mCubeMapNegY = 0;
	}

	if (mCubeMapPosZ != 0)
	{
		delete[] mCubeMapPosZ;
		mCubeMapPosZ = 0;
	}

	if (mCubeMapNegZ != 0)
	{
		delete[] mCubeMapNegZ;
		mCubeMapNegZ = 0;
	}

	mNumMipMapLevels = 0;
	mIsCubeMap = false;
}

void Texture::Copy(Texture* pTexture)
{
	ClearAll();

	mNumMipMapLevels = pTexture->mNumMipMapLevels;
	mIsCubeMap       = pTexture->mIsCubeMap;

	if (pTexture->mColorMap != 0)
	{
		mColorMap = new Canvas[mNumMipMapLevels];
		for (int i = 0; i < mNumMipMapLevels; i++)
		{
			mColorMap[i].Copy(pTexture->mColorMap[i]);
		}
	}

	if (pTexture->mAlphaMap != 0)
	{
		mAlphaMap = new Canvas[mNumMipMapLevels];
		for (int i = 0; i < mNumMipMapLevels; i++)
		{
			mAlphaMap[i].Copy(pTexture->mAlphaMap[i]);
		}
	}

	if (pTexture->mNormalMap != 0)
	{
		mNormalMap = new Canvas[mNumMipMapLevels];
		for (int i = 0; i < mNumMipMapLevels; i++)
		{
			mNormalMap[i].Copy(pTexture->mNormalMap[i]);
		}
	}

	if (pTexture->mSpecularMap != 0)
	{
		mSpecularMap = new Canvas[mNumMipMapLevels];
		for (int i = 0; i < mNumMipMapLevels; i++)
		{
			mSpecularMap[i].Copy(pTexture->mSpecularMap[i]);
		}
	}

	if (pTexture->mCubeMapPosX != 0)
	{
		mCubeMapPosX = new Canvas[mNumMipMapLevels];
		for (int i = 0; i < mNumMipMapLevels; i++)
		{
			mCubeMapPosX[i].Copy(pTexture->mCubeMapPosX[i]);
		}
	}

	if (pTexture->mCubeMapNegX != 0)
	{
		mCubeMapNegX = new Canvas[mNumMipMapLevels];
		for (int i = 0; i < mNumMipMapLevels; i++)
		{
			mCubeMapNegX[i].Copy(pTexture->mCubeMapNegX[i]);
		}
	}

	if (pTexture->mCubeMapPosY != 0)
	{
		mCubeMapPosY = new Canvas[mNumMipMapLevels];
		for (int i = 0; i < mNumMipMapLevels; i++)
		{
			mCubeMapPosY[i].Copy(pTexture->mCubeMapPosY[i]);
		}
	}

	if (pTexture->mCubeMapNegY != 0)
	{
		mCubeMapNegY = new Canvas[mNumMipMapLevels];
		for (int i = 0; i < mNumMipMapLevels; i++)
		{
			mCubeMapNegY[i].Copy(pTexture->mCubeMapNegY[i]);
		}
	}

	if (pTexture->mCubeMapPosZ != 0)
	{
		mCubeMapPosZ = new Canvas[mNumMipMapLevels];
		for (int i = 0; i < mNumMipMapLevels; i++)
		{
			mCubeMapPosZ[i].Copy(pTexture->mCubeMapPosZ[i]);
		}
	}

	if (pTexture->mCubeMapNegZ != 0)
	{
		mCubeMapNegZ = new Canvas[mNumMipMapLevels];
		for (int i = 0; i < mNumMipMapLevels; i++)
		{
			mCubeMapNegZ[i].Copy(pTexture->mCubeMapNegZ[i]);
		}
	}
}

void Texture::Set(const Canvas& pColorMap, Canvas::ResizeHint pResizeHint, int pNumLevels, const Canvas* pAlphaMap, const Canvas* pNormalMap,
	const Canvas* pSpecularMap, bool pMergeColorWithAlpha)
{
	// The texture's dimensions must be a power of two.
	// Check if they are, otherwise set them to the nearest lower
	// power of two.
	int lWidth = pColorMap.GetWidth();
	int lHeight = pColorMap.GetHeight();
	int i;
	int lNumWidthBits = 0;
	int lNumHeightBits = 0;
	int lNearestPow2Width = 1;
	int lNearestPow2Height = 1;
	for (i = 0; i < (int)sizeof(int) * 8; i++)
	{
		if (lNearestPow2Width * 2 > lWidth)
			break;
		lNearestPow2Width <<= 1;
		lNumWidthBits++;
	}
	for (i = 0; i < (int)sizeof(int) * 8; i++)
	{
		if (lNearestPow2Height * 2 > lHeight)
			break;
		lNearestPow2Height <<= 1;
		lNumHeightBits++;
	}

	if (pNumLevels <= 0)
	{
		// Calculate how many levels there will be.
		pNumLevels = lNumWidthBits > lNumHeightBits ? lNumWidthBits : lNumHeightBits;
		pNumLevels++;
	}

	ClearAll();

	// Create all textures.
	mColorMap = new Canvas[pNumLevels];
	if (pAlphaMap != 0 && pMergeColorWithAlpha == false)
		pAlphaMap = new Canvas[pNumLevels];
	if (pNormalMap != 0)
		mNormalMap = new Canvas[pNumLevels];
	if (pSpecularMap != 0)
		mSpecularMap = new Canvas[pNumLevels];

	// And finally, resize them to their respective size.
	for (int lLevel = 0; lLevel < pNumLevels; lLevel++)
	{
		int lWidth  = (lNearestPow2Width  >> lLevel);
		int lHeight = (lNearestPow2Height >> lLevel);

		// If the texture isn't square, either width or height
		// will become 0 at the smallest levels, which we can't
		// allow.
		if (lWidth == 0)
			lWidth = 1;
		if (lHeight == 0)
			lHeight = 1;

		mColorMap[lLevel].Copy(pColorMap);

		if (pMergeColorWithAlpha == true)
		{
			if (pAlphaMap != 0)
			{
				Canvas lAlphaMap(*pAlphaMap);
				lAlphaMap.Resize(mColorMap[lLevel].GetWidth(), mColorMap[lLevel].GetHeight(), pResizeHint);
				mColorMap[lLevel].ConvertTo32BitWithAlpha(lAlphaMap);
			}
			else
				mColorMap[lLevel].ConvertBitDepth(Canvas::BITDEPTH_32_BIT);
		}
		else if(pAlphaMap != 0)
		{
			// Alpha map is stored separately and color map keeps its bitrate.
			mAlphaMap[lLevel].Copy(*pAlphaMap);
			mAlphaMap[lLevel].ConvertToGrayscale();
			mAlphaMap[lLevel].Resize(lWidth, lHeight, pResizeHint);
		}

		mColorMap[lLevel].Resize(lWidth, lHeight, pResizeHint);

		if (pNormalMap != 0)
		{
			mNormalMap[lLevel].Copy(*pNormalMap);
			mNormalMap[lLevel].ConvertBitDepth(Canvas::BITDEPTH_24_BIT);
			mNormalMap[lLevel].Resize(lWidth, lHeight, pResizeHint);
		}
		if (pSpecularMap != 0)
		{
			mSpecularMap[lLevel].Copy(*pSpecularMap);
			mSpecularMap[lLevel].ConvertToGrayscale();
			mSpecularMap[lLevel].Resize(lWidth, lHeight, pResizeHint);
		}
	}

	mNumMipMapLevels = pNumLevels;
	mIsCubeMap = false;
}

void Texture::Set(const Canvas& pCubeMapPosX,
		  const Canvas& pCubeMapNegX,
		  const Canvas& pCubeMapPosY,
		  const Canvas& pCubeMapNegY,
		  const Canvas& pCubeMapPosZ,
		  const Canvas& pCubeMapNegZ)
{
	ClearAll();

	// The texture's dimensions must be a power of two.
	// Check if they are, otherwise set them to the nearest lower
	// power of two.
	int lWidth = pCubeMapPosX.GetWidth();
	int lHeight = pCubeMapPosX.GetHeight();
	int i;
	int lNumWidthBits = 0;
	int lNumHeightBits = 0;
	int lNearestPow2Width = 1;
	int lNearestPow2Height = 1;
	for (i = 0; i < (int)sizeof(int) * 8; i++)
	{
		if (lNearestPow2Width * 2 > lWidth)
			break;
		lNearestPow2Width <<= 1;
		lNumWidthBits++;
	}
	for (i = 0; i < (int)sizeof(int) * 8; i++)
	{
		if (lNearestPow2Height * 2 > lHeight)
			break;
		lNearestPow2Height <<= 1;
		lNumHeightBits++;
	}

	// First calculate how many levels there will be.
	unsigned int lNumLevels = lNumWidthBits > lNumHeightBits ? lNumWidthBits : lNumHeightBits;
	lNumLevels++;

	// Now create all textures.
	mCubeMapPosX = new Canvas[lNumLevels];
	mCubeMapNegX = new Canvas[lNumLevels];
	mCubeMapPosY = new Canvas[lNumLevels];
	mCubeMapNegY = new Canvas[lNumLevels];
	mCubeMapPosZ = new Canvas[lNumLevels];
	mCubeMapNegZ = new Canvas[lNumLevels];

	// And finally, resize them to their respective size.
	for (unsigned int lLevel = 0; lLevel < lNumLevels; lLevel++)
	{
		int lWidth  = (lNearestPow2Width  >> lLevel);
		int lHeight = (lNearestPow2Height >> lLevel);

		// If the texture isn't square, either width or height
		// will become 0 at the smallest levels, which we can't
		// allow.
		if (lWidth == 0)
			lWidth = 1;
		if (lHeight == 0)
			lHeight = 1;

		mCubeMapPosX[lLevel].Copy(pCubeMapPosX);
		mCubeMapNegX[lLevel].Copy(pCubeMapNegX);
		mCubeMapPosY[lLevel].Copy(pCubeMapPosY);
		mCubeMapNegY[lLevel].Copy(pCubeMapNegY);
		mCubeMapPosZ[lLevel].Copy(pCubeMapPosZ);
		mCubeMapNegZ[lLevel].Copy(pCubeMapNegZ);

		mCubeMapPosX[lLevel].Resize(lWidth, lHeight, Canvas::RESIZE_NICEST);
		mCubeMapNegX[lLevel].Resize(lWidth, lHeight, Canvas::RESIZE_NICEST);
		mCubeMapPosY[lLevel].Resize(lWidth, lHeight, Canvas::RESIZE_NICEST);
		mCubeMapNegY[lLevel].Resize(lWidth, lHeight, Canvas::RESIZE_NICEST);
		mCubeMapPosZ[lLevel].Resize(lWidth, lHeight, Canvas::RESIZE_NICEST);
		mCubeMapNegZ[lLevel].Resize(lWidth, lHeight, Canvas::RESIZE_NICEST);
	}

	mNumMipMapLevels = lNumLevels;
	mIsCubeMap = true;
}

void Texture::Prepare(bool pColorMap,
		      bool pAlphaMap,
		      bool pNormalMap,
		      bool pSpecularMap,
		      bool pCubeMap,
		      unsigned int pWidth,
		      unsigned int pHeight)
{
	ClearAll();

	// The texture's dimensions must be a power of two.
	// Check if they are, otherwise set them to the nearest lower
	// power of two.
	unsigned int i;
	unsigned int lNumWidthBits = 0;
	unsigned int lNumHeightBits = 0;
	unsigned int lNearestPow2Width = 1;
	unsigned int lNearestPow2Height = 1;
	for (i = 0; i < sizeof(int) * 8; i++)
	{
		if (lNearestPow2Width * 2 > pWidth)
			break;
		lNearestPow2Width <<= 1;
		lNumWidthBits++;
	}
	for (i = 0; i < sizeof(int) * 8; i++)
	{
		if (lNearestPow2Height * 2 > pHeight)
			break;
		lNearestPow2Height <<= 1;
		lNumHeightBits++;
	}

	// First calculate how many levels there will be.
	unsigned int lNumLevels = lNumWidthBits > lNumHeightBits ? lNumWidthBits : lNumHeightBits;
	lNumLevels++;

	mNumMipMapLevels = lNumLevels;
	mIsCubeMap = pCubeMap;

	if (pCubeMap == false)
	{
		if (pColorMap == true)
			mColorMap = new Canvas[lNumLevels];
		if (pAlphaMap == true)
			mAlphaMap = new Canvas[lNumLevels];
		if (pNormalMap == true)
			mNormalMap = new Canvas[lNumLevels];
		if (pSpecularMap == true)
			mSpecularMap = new Canvas[lNumLevels];

		for (unsigned int lLevel = 0; lLevel < lNumLevels; lLevel++)
		{
			unsigned int lWidth  = (lNearestPow2Width  >> lLevel);
			unsigned int lHeight = (lNearestPow2Height >> lLevel);

			// If the texture isn't square, either width or height
			// will become 0 at the smallest levels, which we can't
			// allow.
			if (lWidth == 0)
				lWidth = 1;
			if (lHeight == 0)
				lHeight = 1;

			if (pColorMap == true)
			{
				mColorMap[lLevel].Reset(lWidth, lHeight, Canvas::BITDEPTH_24_BIT);
				mColorMap[lLevel].CreateBuffer();
			}

			if (pAlphaMap == true)
			{
				mAlphaMap[lLevel].Reset(lWidth, lHeight, Canvas::BITDEPTH_8_BIT);
				mAlphaMap[lLevel].CreateBuffer();
				mAlphaMap[lLevel].SetGrayscalePalette();
			}

			if (pNormalMap == true)
			{
				mNormalMap[lLevel].Reset(lWidth, lHeight, Canvas::BITDEPTH_24_BIT);
				mNormalMap[lLevel].CreateBuffer();
			}

			if (pSpecularMap == true)
			{
				mSpecularMap[lLevel].Reset(lWidth, lHeight, Canvas::BITDEPTH_8_BIT);
				mSpecularMap[lLevel].CreateBuffer();
				mSpecularMap[lLevel].SetGrayscalePalette();
			}
		}
	}
	else
	{
		mCubeMapPosX = new Canvas[lNumLevels];
		mCubeMapNegX = new Canvas[lNumLevels];
		mCubeMapPosY = new Canvas[lNumLevels];
		mCubeMapNegY = new Canvas[lNumLevels];
		mCubeMapPosZ = new Canvas[lNumLevels];
		mCubeMapNegZ = new Canvas[lNumLevels];

		for (unsigned int lLevel = 0; lLevel < lNumLevels; lLevel++)
		{
			unsigned int lWidth  = (lNearestPow2Width  >> lLevel);
			unsigned int lHeight = (lNearestPow2Height >> lLevel);

			// If the texture isn't square, either width or height
			// will become 0 at the smallest levels, which we can't
			// allow.
			if (lWidth == 0)
				lWidth = 1;
			if (lHeight == 0)
				lHeight = 1;

			mCubeMapPosX[lLevel].Reset(lWidth, lHeight, Canvas::BITDEPTH_24_BIT);
			mCubeMapNegX[lLevel].Reset(lWidth, lHeight, Canvas::BITDEPTH_24_BIT);
			mCubeMapPosY[lLevel].Reset(lWidth, lHeight, Canvas::BITDEPTH_24_BIT);
			mCubeMapNegY[lLevel].Reset(lWidth, lHeight, Canvas::BITDEPTH_24_BIT);
			mCubeMapPosZ[lLevel].Reset(lWidth, lHeight, Canvas::BITDEPTH_24_BIT);
			mCubeMapNegZ[lLevel].Reset(lWidth, lHeight, Canvas::BITDEPTH_24_BIT);

			mCubeMapPosX[lLevel].CreateBuffer();
			mCubeMapNegX[lLevel].CreateBuffer();
			mCubeMapPosY[lLevel].CreateBuffer();
			mCubeMapNegY[lLevel].CreateBuffer();
			mCubeMapPosZ[lLevel].CreateBuffer();
			mCubeMapNegZ[lLevel].CreateBuffer();
		}
	}
}

const Canvas* Texture::GetColorMap(unsigned int pMipMapLevel) const
{
	if (mColorMap != 0 && pMipMapLevel < (unsigned int)mNumMipMapLevels)
		return &mColorMap[pMipMapLevel];

	return 0;
}

const Canvas* Texture::GetAlphaMap(unsigned int pMipMapLevel) const
{
	if (mAlphaMap != 0 && pMipMapLevel < (unsigned int)mNumMipMapLevels)
		return &mAlphaMap[pMipMapLevel];

	return 0;
}

const Canvas* Texture::GetNormalMap(unsigned int pMipMapLevel) const
{
	if (mNormalMap != 0 && pMipMapLevel < (unsigned int)mNumMipMapLevels)
		return &mNormalMap[pMipMapLevel];

	return 0;
}

const Canvas* Texture::GetSpecularMap(unsigned int pMipMapLevel) const
{
	if (mSpecularMap != 0 && pMipMapLevel < (unsigned int)mNumMipMapLevels)
		return &mSpecularMap[pMipMapLevel];

	return 0;
}

const Canvas* Texture::GetCubeMapPosX(unsigned int pMipMapLevel) const
{
	if (mCubeMapPosX != 0 && pMipMapLevel < (unsigned int)mNumMipMapLevels)
		return &mCubeMapPosX[pMipMapLevel];

	return 0;
}

const Canvas* Texture::GetCubeMapNegX(unsigned int pMipMapLevel) const
{
	if (mCubeMapNegX != 0 && pMipMapLevel < (unsigned int)mNumMipMapLevels)
		return &mCubeMapNegX[pMipMapLevel];

	return 0;
}

const Canvas* Texture::GetCubeMapPosY(unsigned int pMipMapLevel) const
{
	if (mCubeMapPosY != 0 && pMipMapLevel < (unsigned int)mNumMipMapLevels)
		return &mCubeMapPosY[pMipMapLevel];

	return 0;
}

const Canvas* Texture::GetCubeMapNegY(unsigned int pMipMapLevel) const
{
	if (mCubeMapNegY != 0 && pMipMapLevel < (unsigned int)mNumMipMapLevels)
		return &mCubeMapNegY[pMipMapLevel];

	return 0;
}

const Canvas* Texture::GetCubeMapPosZ(unsigned int pMipMapLevel) const
{
	if (mCubeMapPosZ != 0 && pMipMapLevel < (unsigned int)mNumMipMapLevels)
		return &mCubeMapPosZ[pMipMapLevel];

	return 0;
}

const Canvas* Texture::GetCubeMapNegZ(unsigned int pMipMapLevel) const
{
	if (mCubeMapNegZ != 0 && pMipMapLevel < (unsigned int)mNumMipMapLevels)
		return &mCubeMapNegZ[pMipMapLevel];

	return 0;
}

Canvas* Texture::_GetColorMap(unsigned int pMipMapLevel)
{
	if (mColorMap != 0 && pMipMapLevel < (unsigned int)mNumMipMapLevels)
		return &mColorMap[pMipMapLevel];

	return 0;
}

Canvas* Texture::_GetAlphaMap(unsigned int pMipMapLevel)
{
	if (mAlphaMap != 0 && pMipMapLevel < (unsigned int)mNumMipMapLevels)
		return &mAlphaMap[pMipMapLevel];

	return 0;
}

Canvas* Texture::_GetNormalMap(unsigned int pMipMapLevel)
{
	if (mNormalMap != 0 && pMipMapLevel < (unsigned int)mNumMipMapLevels)
		return &mNormalMap[pMipMapLevel];

	return 0;
}

Canvas* Texture::_GetSpecularMap(unsigned int pMipMapLevel)
{
	if (mSpecularMap != 0 && pMipMapLevel < (unsigned int)mNumMipMapLevels)
		return &mSpecularMap[pMipMapLevel];

	return 0;
}

Canvas* Texture::_GetCubeMapPosX(unsigned int pMipMapLevel)
{
	if (mCubeMapPosX != 0 && pMipMapLevel < (unsigned int)mNumMipMapLevels)
		return &mCubeMapPosX[pMipMapLevel];

	return 0;
}

Canvas* Texture::_GetCubeMapNegX(unsigned int pMipMapLevel)
{
	if (mCubeMapNegX != 0 && pMipMapLevel < (unsigned int)mNumMipMapLevels)
		return &mCubeMapNegX[pMipMapLevel];

	return 0;
}

Canvas* Texture::_GetCubeMapPosY(unsigned int pMipMapLevel)
{
	if (mCubeMapPosY != 0 && pMipMapLevel < (unsigned int)mNumMipMapLevels)
		return &mCubeMapPosY[pMipMapLevel];

	return 0;
}

Canvas* Texture::_GetCubeMapNegY(unsigned int pMipMapLevel)
{
	if (mCubeMapNegY != 0 && pMipMapLevel < (unsigned int)mNumMipMapLevels)
		return &mCubeMapNegY[pMipMapLevel];

	return 0;
}

Canvas* Texture::_GetCubeMapPosZ(unsigned int pMipMapLevel)
{
	if (mCubeMapPosZ != 0 && pMipMapLevel < (unsigned int)mNumMipMapLevels)
		return &mCubeMapPosZ[pMipMapLevel];

	return 0;
}

Canvas* Texture::_GetCubeMapNegZ(unsigned int pMipMapLevel)
{
	if (mCubeMapNegZ != 0 && pMipMapLevel < (unsigned int)mNumMipMapLevels)
		return &mCubeMapNegZ[pMipMapLevel];

	return 0;
}

unsigned int Texture::GetPixelByteSize() const
{
	if (mColorMap != 0)
		return mColorMap[0].GetPixelByteSize();
	else
		return mCubeMapPosX[0].GetPixelByteSize();
}

void Texture::SwapRGBOrder()
{
	int i;

	if (mColorMap != 0)
	{
		for (i = 0; i < GetNumMipMapLevels(); i++)
		{
			mColorMap[i].SwapRGBOrder();
			if (mSpecularMap != 0)
				mSpecularMap[i].SwapRGBOrder();
			if (mNormalMap != 0)
				mNormalMap[i].SwapRGBOrder();
		}
	}
	else
	{
		for (i = 0; i < GetNumMipMapLevels(); i++)
		{
			mCubeMapPosX[i].SwapRGBOrder();
			mCubeMapNegX[i].SwapRGBOrder();
			mCubeMapPosY[i].SwapRGBOrder();
			mCubeMapNegY[i].SwapRGBOrder();
			mCubeMapPosZ[i].SwapRGBOrder();
			mCubeMapNegZ[i].SwapRGBOrder();
		}
	}
}

void Texture::ConvertBitDepth(Canvas::BitDepth pBitDepth)
{
	if (mColorMap != 0)
	{
		for (int i = 0; i < mNumMipMapLevels; i++)
		{
			mColorMap[i].ConvertBitDepth(pBitDepth);
		}
	}
}

void Texture::GenerateNormalMap(Canvas& pNormalMap, const Canvas& pBumpMap, double pScale)
{
	Canvas lBumpMap(pBumpMap, true);
	lBumpMap.ConvertToGrayscale();

	pNormalMap.Reset(lBumpMap.GetWidth(), lBumpMap.GetHeight(), Canvas::BITDEPTH_24_BIT);
	pNormalMap.CreateBuffer();

	for (unsigned y = 0; y < lBumpMap.GetHeight(); y++)
	{
		for (unsigned x = 0; x < lBumpMap.GetWidth(); x++)
		{
			double lNorthValue = 0;
			double lSouthValue = 0;
			double lEastValue  = 0;
			double lWestValue  = 0;

			if (y > 0)
			{
				Color lColor;
				lBumpMap.GetPixelColor(x, y - 1, lColor);
				lNorthValue = (double)lColor.mRed * pScale;
			}
			if (y < lBumpMap.GetHeight() - 1)
			{
				Color lColor;
				lBumpMap.GetPixelColor(x, y + 1, lColor);
				lSouthValue = (double)lColor.mRed * pScale;
			}
			if (x > 0)
			{
				Color lColor;
				lBumpMap.GetPixelColor(x - 1, y, lColor);
				lWestValue = (double)lColor.mRed * pScale;
			}
			if (x < lBumpMap.GetWidth() - 1)
			{
				Color lColor;
				lBumpMap.GetPixelColor(x + 1, y, lColor);
				lEastValue = (double)lColor.mRed * pScale;
			}

			double lNX = sin(atan(-(lEastValue - lWestValue) / 255.0));
			double lNY = sin(atan((lSouthValue - lNorthValue) / 255.0));
			double lNZ = 1.0 - (lNX * lNX + lNY * lNY);

			Color lColor((uint8)(lNX * 127.0) + 128, (uint8)(lNY * 127.0) + 128, (uint8)(lNZ * 127.0) + 128);
			pNormalMap.SetPixelColor(x, y, lColor);
		}
	}
}

} // End namespace.
