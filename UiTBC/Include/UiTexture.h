/*
	Lepra::File:   Texture.h
	Class:  Texture
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand
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
	Texture(const Lepra::Canvas& pColorMap); // Will generate mipmaps.
	Texture(Texture* pTexture);
	virtual ~Texture();

	void ClearAll();

	void ConvertBitDepth(Lepra::Canvas::BitDepth pBitDepth);

	// Normal texture set. If both specular and normal maps are used, they will be merged
	// such that the specular value will be stored in the alpha-channel of the normal map.
	void Set(const Lepra::Canvas& pColorMap,		// Required.
		 const Lepra::Canvas* pAlphaMap    = 0,	// Optional, will be embedded in the color map.
		 const Lepra::Canvas* pNormalMap   = 0,	// Optional.
		 const Lepra::Canvas* pSpecularMap = 0,	// Optional.
		 bool pMergeColorWithAlpha = true);

	// Set cube map.
	void Set(const Lepra::Canvas& pCubeMapPosX,			// Required.
		 const Lepra::Canvas& pCubeMapNegX,			// Required.
		 const Lepra::Canvas& pCubeMapPosY,			// Required.
		 const Lepra::Canvas& pCubeMapNegY,			// Required.
		 const Lepra::Canvas& pCubeMapPosZ,			// Required.
		 const Lepra::Canvas& pCubeMapNegZ);			// Required.

	void Copy(Texture* pTexture);

	// Level 0 is the largest one...
	inline int GetNumMipMapLevels() const;

	inline bool IsCubeMap() const;

	// Lepra::Color map contains alpha channel.
	const Lepra::Canvas* GetColorMap   (unsigned int pMipMapLevel) const;
	const Lepra::Canvas* GetAlphaMap   (unsigned int pMipMapLevel) const;
	const Lepra::Canvas* GetNormalMap  (unsigned int pMipMapLevel) const;
	const Lepra::Canvas* GetSpecularMap(unsigned int pMipMapLevel) const;

	const Lepra::Canvas* GetCubeMapPosX (unsigned int pMipMapLevel) const;
	const Lepra::Canvas* GetCubeMapNegX (unsigned int pMipMapLevel) const;
	const Lepra::Canvas* GetCubeMapPosY (unsigned int pMipMapLevel) const;
	const Lepra::Canvas* GetCubeMapNegY (unsigned int pMipMapLevel) const;
	const Lepra::Canvas* GetCubeMapPosZ (unsigned int pMipMapLevel) const;
	const Lepra::Canvas* GetCubeMapNegZ (unsigned int pMipMapLevel) const;

	inline unsigned int GetWidth(int pMipMapLevel = 0) const;
	inline unsigned int GetHeight(int pMipMapLevel = 0) const;
	unsigned int GetPixelByteSize() const;	// Returns the pixel size in bytes.

	// Generate a normal map from a grayscale bump map. The scale parameter is multiplied
	// with the bumpmap, which can control the steepness of the edges in the bumpmap.
	static void GenerateNormalMap(Lepra::Canvas& pNormalMap, const Lepra::Canvas& pBumpMap, double pScale = 1.0);

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

	Lepra::Canvas* _GetColorMap   (unsigned int pMipMapLevel);
	Lepra::Canvas* _GetAlphaMap   (unsigned int pMipMapLevel);
	Lepra::Canvas* _GetNormalMap  (unsigned int pMipMapLevel);
	Lepra::Canvas* _GetSpecularMap(unsigned int pMipMapLevel);

	Lepra::Canvas* _GetCubeMapPosX (unsigned int pMipMapLevel);
	Lepra::Canvas* _GetCubeMapNegX (unsigned int pMipMapLevel);
	Lepra::Canvas* _GetCubeMapPosY (unsigned int pMipMapLevel);
	Lepra::Canvas* _GetCubeMapNegY (unsigned int pMipMapLevel);
	Lepra::Canvas* _GetCubeMapPosZ (unsigned int pMipMapLevel);
	Lepra::Canvas* _GetCubeMapNegZ (unsigned int pMipMapLevel);



	int mNumMipMapLevels;
	bool mIsCubeMap;

	Lepra::Canvas* mColorMap;
	Lepra::Canvas* mAlphaMap;
	Lepra::Canvas* mNormalMap;
	Lepra::Canvas* mSpecularMap;

	Lepra::Canvas* mCubeMapPosX;
	Lepra::Canvas* mCubeMapNegX;
	Lepra::Canvas* mCubeMapPosY;
	Lepra::Canvas* mCubeMapNegY;
	Lepra::Canvas* mCubeMapPosZ;
	Lepra::Canvas* mCubeMapNegZ;
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