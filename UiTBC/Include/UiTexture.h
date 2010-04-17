/*
	File:   Texture.h
	Class:  Texture
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games
*/

#ifndef UITEXTURE_H
#define UITEXTURE_H

#include "../../Lepra/Include/Canvas.h"

namespace UiTbc
{

class Texture
{
public:

	friend class TEXLoader;

	enum // Map. 
	{
		COLOR_MAP = 0,
		ALPHA_MAP,
		NORMAL_MAP,
		SPECULAR_MAP,
		CUBE_MAP,

		NUM_MAPS
	};

	// The constructor makes a copy of the data, and is not responsible
	// for deleting the canvas.
	Texture();
	Texture(const Canvas& pColorMap); // Will generate mipmaps.
	Texture(Texture* pTexture);
	virtual ~Texture();

	void ClearAll();

	void ConvertBitDepth(Canvas::BitDepth pBitDepth);

	// Normal texture set. If both specular and normal maps are used, they will be merged
	// such that the specular value will be stored in the alpha-channel of the normal map.
	void Set(const Canvas& pColorMap,		// Required.
		 const Canvas* pAlphaMap    = 0,	// Optional, will be embedded in the color map.
		 const Canvas* pNormalMap   = 0,	// Optional.
		 const Canvas* pSpecularMap = 0,	// Optional.
		 bool pMergeColorWithAlpha = true);

	// Set cube map.
	void Set(const Canvas& pCubeMapPosX,			// Required.
		 const Canvas& pCubeMapNegX,			// Required.
		 const Canvas& pCubeMapPosY,			// Required.
		 const Canvas& pCubeMapNegY,			// Required.
		 const Canvas& pCubeMapPosZ,			// Required.
		 const Canvas& pCubeMapNegZ);			// Required.

	void Copy(Texture* pTexture);

	// Level 0 is the largest one...
	inline int GetNumMipMapLevels() const;

	inline bool IsCubeMap() const;

	// Color map contains alpha channel.
	const Canvas* GetColorMap   (unsigned int pMipMapLevel) const;
	const Canvas* GetAlphaMap   (unsigned int pMipMapLevel) const;
	const Canvas* GetNormalMap  (unsigned int pMipMapLevel) const;
	const Canvas* GetSpecularMap(unsigned int pMipMapLevel) const;

	const Canvas* GetCubeMapPosX (unsigned int pMipMapLevel) const;
	const Canvas* GetCubeMapNegX (unsigned int pMipMapLevel) const;
	const Canvas* GetCubeMapPosY (unsigned int pMipMapLevel) const;
	const Canvas* GetCubeMapNegY (unsigned int pMipMapLevel) const;
	const Canvas* GetCubeMapPosZ (unsigned int pMipMapLevel) const;
	const Canvas* GetCubeMapNegZ (unsigned int pMipMapLevel) const;

	inline unsigned int GetWidth(int pMipMapLevel = 0) const;
	inline unsigned int GetHeight(int pMipMapLevel = 0) const;
	unsigned int GetPixelByteSize() const;	// Returns the pixel size in bytes.

	// Generate a normal map from a grayscale bump map. The scale parameter is multiplied
	// with the bumpmap, which can control the steepness of the edges in the bumpmap.
	static void GenerateNormalMap(Canvas& pNormalMap, const Canvas& pBumpMap, double pScale = 1.0);

	void SwapRGBOrder();
private:

	// Functions used by TEXLoader.
	void Prepare(bool pColorMap,
		     bool pAlphaMap,
		     bool pNormalMap,
		     bool pSpecularMap,
		     bool pCubeMap,
		     unsigned int pWidth,
		     unsigned int pHeight);

	Canvas* _GetColorMap   (unsigned int pMipMapLevel);
	Canvas* _GetAlphaMap   (unsigned int pMipMapLevel);
	Canvas* _GetNormalMap  (unsigned int pMipMapLevel);
	Canvas* _GetSpecularMap(unsigned int pMipMapLevel);

	Canvas* _GetCubeMapPosX (unsigned int pMipMapLevel);
	Canvas* _GetCubeMapNegX (unsigned int pMipMapLevel);
	Canvas* _GetCubeMapPosY (unsigned int pMipMapLevel);
	Canvas* _GetCubeMapNegY (unsigned int pMipMapLevel);
	Canvas* _GetCubeMapPosZ (unsigned int pMipMapLevel);
	Canvas* _GetCubeMapNegZ (unsigned int pMipMapLevel);



	int mNumMipMapLevels;
	bool mIsCubeMap;

	Canvas* mColorMap;
	Canvas* mAlphaMap;
	Canvas* mNormalMap;
	Canvas* mSpecularMap;

	Canvas* mCubeMapPosX;
	Canvas* mCubeMapNegX;
	Canvas* mCubeMapPosY;
	Canvas* mCubeMapNegY;
	Canvas* mCubeMapPosZ;
	Canvas* mCubeMapNegZ;
};

int Texture::GetNumMipMapLevels() const
{
	return mNumMipMapLevels;
}

bool Texture::IsCubeMap() const
{
	return mIsCubeMap;
}

unsigned int Texture::GetWidth(int pMipMapLevel) const
{
	if (pMipMapLevel >= 0 && pMipMapLevel < mNumMipMapLevels)
		return mColorMap[pMipMapLevel].GetWidth();
	else
		return 0;
}

unsigned int Texture::GetHeight(int pMipMapLevel) const
{
	if (pMipMapLevel >= 0 && pMipMapLevel < mNumMipMapLevels)
		return mColorMap[pMipMapLevel].GetHeight();
	else
		return 0;
}


} // End namespace.

#endif