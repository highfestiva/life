/*
	File:   TerrainTextureGenerator.cpp
	Class:  TerrainTextureGenerator
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#include "pch.h"
#include "../Include/UiTerrainTextureGenerator.h"

namespace UiTbc
{

TerrainTextureGenerator::TerrainTextureGenerator()
{
}

TerrainTextureGenerator::~TerrainTextureGenerator()
{
	RangeList::iterator lIter;
	for (lIter = mRangeList.begin(); lIter != mRangeList.end(); ++lIter)
	{
		delete (*lIter);
	}
}

bool TerrainTextureGenerator::AddNewRange(float pMaxLevel, float pFadeRange, Canvas* pImages,
	float* pMinNormalY, float* pMaxNormalY, float* pAngularFadeRange, int pNumImages)
{
	if (mRangeList.empty() == true)
	{
		Range* lRange = new Range(pMaxLevel, pFadeRange, pImages, pMinNormalY, pMaxNormalY, pAngularFadeRange, pNumImages);
		mRangeList.push_back(lRange);
		return true;
	}
	else
	{
		Range* lLastRange = *(--mRangeList.end());
		if (lLastRange->mMaxLevel >= pMaxLevel)
		{
			return false;
		}

		Range* lRange = new Range(lLastRange->mMaxLevel, pMaxLevel, pFadeRange, pImages, pMinNormalY, pMaxNormalY, pAngularFadeRange, pNumImages);
		mRangeList.push_back(lRange);
		return true;
	}
}

/*void TerrainTextureGenerator::RenderTexture(const TerrainPatch& pPatch, Canvas& pCanvas)
{
	pCanvas.CreateBuffer();
	pCanvas.Clear();

	float lOneOverWidth = 1.0f / ((float)pCanvas.GetWidth() - 1);
	float lOneOverHeight = 1.0f / ((float)pCanvas.GetHeight() - 1);

	vec3* lPos = 0;
	vec3* lNormal = 0;

	for (unsigned y = 0; y < pCanvas.GetHeight(); y++)
	{
		float lV = (float)y * lOneOverHeight;

		for (unsigned x = 0; x < pCanvas.GetWidth(); x++)
		{
			float lU = (float)x * lOneOverWidth;

			vec3 lPos;
			vec3 lNormal;

			pPatch.GetPosAndNormal(lU, lV, lPos, lNormal);

			float r = 0;
			float g = 0;
			float b = 0;
			GetHeightDependentRGB(lPos, lNormal, lU, lV, r, g, b);

			Color lColor((uint8)floor(r * 255.0f), (uint8)floor(g * 255.0f), (uint8)floor(b * 255.0f));
			pCanvas.SetPixelColor(x, y, lColor);
		}
	}

	if (lPos != 0)
	{
		delete[] lPos;
	}
	if (lNormal != 0)
	{
		delete[] lNormal;
	}
}*/

void TerrainTextureGenerator::GetHeightDependentRGB(const vec3& pPos,
													const vec3& pNormal,
													float pU, float pV,
													float& r, float& g, float& b)
{
	RangeList::iterator lIter;
	for (lIter = mRangeList.begin(); lIter != mRangeList.end(); ++lIter)
	{
		Range* lRange = *lIter;
		float lMinMin = lRange->mMinLevel - lRange->mFadeRange;
		float lMinMax = lRange->mMinLevel + lRange->mFadeRange;
		float lMaxMin = lRange->mMaxLevel - lRange->mFadeRange;
		float lMaxMax = lRange->mMaxLevel + lRange->mFadeRange;
		
		if (pPos.y < lRange->mMinLevel && lIter != mRangeList.begin())
		{
			RangeList::iterator lPrev(lIter);
			--lPrev;
			Range* lTemp = *lPrev;
			lMinMin = lRange->mMinLevel - lTemp->mFadeRange;
			lMinMax = lRange->mMinLevel + lTemp->mFadeRange;
		}
		if (pPos.y > lRange->mMaxLevel && lIter != mRangeList.end())
		{
			RangeList::iterator lNext(lIter);
			++lNext;
			Range* lTemp = *lNext;
			lMaxMin = lRange->mMaxLevel - lTemp->mFadeRange;
			lMaxMax = lRange->mMaxLevel + lTemp->mFadeRange;
		}

		float lMinDiff = (pPos.y - lMinMin) / (lMinMax - lMinMin);
		float lMaxDiff = 1.0f - (pPos.y - lMaxMin) / (lMaxMax - lMaxMin);

		float lRed   = 0;
		float lGreen = 0;
		float lBlue  = 0;
		GetAngleDependentRGB(lRange, pNormal, pU, pV, lRed, lGreen, lBlue);

		if (lMaxDiff >= 0 && lMaxDiff <= 1)
		{
			if (lIter == mRangeList.begin())
			{
				r = lRed;
				g = lGreen;
				b = lBlue;
			}
			else
			{
				float t = lMaxDiff;
				r = t * lRed   + (1.0f - t) * r;
				g = t * lGreen + (1.0f - t) * g;
				b = t * lBlue  + (1.0f - t) * b;
			}
		}
		else if(lMinDiff >= 0 && lMinDiff <= 1)
		{
			if (lIter == mRangeList.begin())
			{
				r = lRed;
				g = lGreen;
				b = lBlue;
			}
			else
			{
				float t = lMinDiff;
				r = t * lRed   + (1.0f - t) * r;
				g = t * lGreen + (1.0f - t) * g;
				b = t * lBlue  + (1.0f - t) * b;
			}
		}
		else if(pPos.y >= lRange->mMinLevel && pPos.y < lRange->mMaxLevel)
		{
			r = lRed;
			g = lGreen;
			b = lBlue;
		}
	}
}

void TerrainTextureGenerator::GetAngleDependentRGB(Range* pRange, 
												   const vec3& pNormal, 
												   float pU, float pV,
												   float& r, float& g, float& b)
{
	for (int i = 0; i < pRange->mNumImages; i++)
	{
		Color lColor;
		pRange->mImages[i].GetPixelColor((unsigned)(pU * (float)(pRange->mImages[i].GetWidth() - 1)), 
											 (unsigned)(pV * (float)(pRange->mImages[i].GetHeight() - 1)),
											 lColor);
		float lMinMin = pRange->mMinNormalY[i] - pRange->mAngularFadeRange[i];
		float lMinMax = pRange->mMinNormalY[i] + pRange->mAngularFadeRange[i];
		float lMaxMin = pRange->mMaxNormalY[i] - pRange->mAngularFadeRange[i];
		float lMaxMax = pRange->mMaxNormalY[i] + pRange->mAngularFadeRange[i];

		float lMinDiff = (pNormal.y - lMinMin) / (lMinMax - lMinMin);
		float lMaxDiff = 1.0f - (pNormal.y - lMaxMin) / (lMaxMax - lMaxMin);
		if (lMaxDiff >= 0 && lMaxDiff <= 1)
		{
			if (i == 0)
			{
				r = (float)lColor.mRed   / 255.0f;
				g = (float)lColor.mGreen / 255.0f;
				b = (float)lColor.mBlue  / 255.0f;
			}
			else
			{
				float t = lMaxDiff;
				r = t * (float)lColor.mRed   / 255.0f + (1.0f - t) * r;
				g = t * (float)lColor.mGreen / 255.0f + (1.0f - t) * g;
				b = t * (float)lColor.mBlue  / 255.0f + (1.0f - t) * b;
			}
		}
		else if(lMinDiff >= 0 && lMinDiff <= 1)
		{
			if (i == 0)
			{
				r = (float)lColor.mRed   / 255.0f;
				g = (float)lColor.mGreen / 255.0f;
				b = (float)lColor.mBlue  / 255.0f;
			}
			else
			{
				float t = lMinDiff;
				r = t * (float)lColor.mRed   / 255.0f + (1.0f - t) * r;
				g = t * (float)lColor.mGreen / 255.0f + (1.0f - t) * g;
				b = t * (float)lColor.mBlue  / 255.0f + (1.0f - t) * b;
			}
		}
		else if(pNormal.y >= pRange->mMinNormalY[i] && pNormal.y < pRange->mMaxNormalY[i])
		{
			r = (float)lColor.mRed   / 255.0f;
			g = (float)lColor.mGreen / 255.0f;
			b = (float)lColor.mBlue  / 255.0f;
		}
	}
}

TerrainTextureGenerator::Range::Range(float pMaxLevel,
									  float pFadeRange,
									  Canvas* pImages, 
									  float* pMinNormalY, 
									  float* pMaxNormalY, 
									  float* pAngularFadeRange,
									  int pNumImages)
{
	// The first range begins at negative infinity.
	unsigned lBits = 0xFF800000;
	mMinLevel = *(float*)&lBits;
	mMaxLevel = pMaxLevel;
	mFadeRange = pFadeRange;
	mNumImages = pNumImages;

	mImages = new Canvas[mNumImages];
	mMinNormalY = new float[mNumImages];
	mMaxNormalY = new float[mNumImages];
	mAngularFadeRange = new float[mNumImages];

	for (int i = 0; i < mNumImages; i++)
	{
		mImages[i].Copy(pImages[i]);
		mMinNormalY[i] = pMinNormalY[i];
		mMaxNormalY[i] = pMaxNormalY[i];
		mAngularFadeRange[i] = pAngularFadeRange[i];
	}
}

TerrainTextureGenerator::Range::Range(float pMinLevel,
									  float pMaxLevel,
									  float pFadeRange,
									  Canvas* pImages, 
									  float* pMinNormalY, 
									  float* pMaxNormalY, 
									  float* pAngularFadeRange,
									  int pNumImages)
{
	mMinLevel = pMinLevel;
	mMaxLevel = pMaxLevel;
	mFadeRange = pFadeRange;
	mNumImages = pNumImages;

	mImages = new Canvas[mNumImages];
	mMinNormalY = new float[mNumImages];
	mMaxNormalY = new float[mNumImages];
	mAngularFadeRange = new float[mNumImages];

	for (int i = 0; i < mNumImages; i++)
	{
		mImages[i].Copy(pImages[i]);
		mMinNormalY[i] = pMinNormalY[i];
		mMaxNormalY[i] = pMaxNormalY[i];
		mAngularFadeRange[i] = pAngularFadeRange[i];
	}
}

TerrainTextureGenerator::Range::~Range()
{
	delete[] mImages;
	delete[] mMinNormalY;
	delete[] mMaxNormalY;
	delete[] mAngularFadeRange;
}


}

