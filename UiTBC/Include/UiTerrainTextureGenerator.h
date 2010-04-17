/*
	Class:  TerrainTextureGenerator
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games
*/

#ifndef UITERRAINTEXTUREGENERATOR_H
#define UITERRAINTEXTUREGENERATOR_H

//#include "UiTerrainPatch.h"	// TRICKY: this must be before UiTbc include due to a bug in MSVC8.
#include "../../Lepra/Include/Canvas.h"
#include "../../Lepra/Include/Vector3D.h"
#include "UiTBC.h"
#include <list>

namespace UiTbc
{

class TerrainTextureGenerator
{
public:

	TerrainTextureGenerator();
	virtual ~TerrainTextureGenerator();

	// Adds a new range on top of the previous one. pMaxLevel must be higher than
	// the previous range's max level. Returns false if that's not the case.
	// The number of images, and the numbers of elements in the two arrays pMinNormalY
	// and pMaxNormalY, must be the same as the value of pNumImages.
	// pMinNormalY and pMaxNormalY specifies the range within which the corresponding
	// image should be rendered, and FadeRange specifies the range within which to
	// fade between two images. Setting this to zero will result in very sharp edges
	// between two textures, while a higher value will result in smoother transitions.
	bool AddNewRange(float pMaxLevel,          // Maximum height for this range
					 float pFadeRange,         // Height dependent fading.
					 Canvas* pImages, 
					 float* pMinNormalY,       // Defines the angle for each image.
					 float* pMaxNormalY,       // ...
					 float* pAngularFadeRange, // Angular fading between images.
					 int pNumImages);

	// Renders the final texture to match pPatch. The canvas must be initialized with
	// the desired resolution of the texture.
//	void RenderTexture(const TerrainPatch& pPatch, Canvas& pCanvas);

private:

	class Range
	{
	public:
		Range(float pMaxLevel,
			  float pFadeRange,
			  Canvas* pImages, 
			  float* pMinNormalY, 
			  float* pMaxNormalY, 
			  float* pAngularFadeRange,
			  int pNumImages);
		Range(float pMinLevel,
			  float pMaxLevel,
			  float pFadeRange,
			  Canvas* pImages, 
			  float* pMinNormalY, 
			  float* pMaxNormalY, 
			  float* pAngularFadeRange,
			  int pNumImages);
		~Range();

		float mMinLevel;
		float mMaxLevel;
		float mFadeRange;
		float* mMinNormalY;
		float* mMaxNormalY;
		float* mAngularFadeRange;
		Canvas* mImages;
		int mNumImages;
	};

	typedef std::list<Range*> RangeList;

	void GetAngleDependentRGB(Range* pRange, const Vector3DF& pNormal, 
		float pU, float pV, float& r, float& g, float& b);
	void GetHeightDependentRGB(const Vector3DF& pPos, const Vector3DF& pNormal,
		float pU, float pV, float& r, float& g, float& b);

	RangeList mRangeList;
	RangeList::iterator mCurrentRangeIter;
};

} // End namespace.

#endif