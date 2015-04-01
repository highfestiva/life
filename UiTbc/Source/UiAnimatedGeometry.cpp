/*
	Class:  AnimatedGeometry
	Author: Jonas Bystr�m
	Copyright (c) Pixel Doctrine
*/

#include "pch.h"
#include "../Include/UiAnimatedGeometry.h"
#include "../../Tbc/Include/Bones.h"
#include "../../Lepra/Include/LepraAssert.h"

namespace UiTbc
{

AnimatedGeometry::AnimatedGeometry() :
	mOriginalGeometry(0),
	mVertexCount(0),
	mVertexData(0),
	mNormalData(0),
	mBoneWeightGroupCount(0),
	mBoneWeightGroupArray(0)
{
}

AnimatedGeometry::~AnimatedGeometry()
{
	mVertexCount = 0;
	delete[] mVertexData;
	mVertexData = 0;
	delete[] mNormalData;
	mNormalData = 0;

	FreeAllBoneWeights();
}



void AnimatedGeometry::SetGeometry(Tbc::GeometryBase* pGeometry)
{
	mOriginalGeometry = pGeometry;
	SetPrimitiveType(mOriginalGeometry->GetPrimitiveType());

	mVertexCount = mOriginalGeometry->GetMaxVertexCount();
	mVertexData = new float[mVertexCount * 3];
	mNormalData = new float[mVertexCount * 3];

	mOriginalGeometry->GenerateVertexNormalData();

	const float* lOriginalVData = mOriginalGeometry->GetVertexData();
	const float* lOriginalNData = mOriginalGeometry->GetNormalData();
	for (int i = 0; i < mVertexCount; i++)
	{
		int lIndex = i * 3;
		mVertexData[lIndex + 0] = lOriginalVData[lIndex + 0];
		mVertexData[lIndex + 1] = lOriginalVData[lIndex + 1];
		mVertexData[lIndex + 2] = lOriginalVData[lIndex + 2];

		mNormalData[lIndex + 0] = lOriginalNData[lIndex + 0];
		mNormalData[lIndex + 1] = lOriginalNData[lIndex + 1];
		mNormalData[lIndex + 2] = lOriginalNData[lIndex + 2];
	}
}



void AnimatedGeometry::AddBoneWeights(BoneWeightGroup& pWeights)
{
	int lNewCount = mBoneWeightGroupCount + 1;

	BoneWeightGroup* lOldWeightGroupArray = mBoneWeightGroupArray;
	mBoneWeightGroupArray = new BoneWeightGroup[lNewCount];
	::memcpy(mBoneWeightGroupArray, lOldWeightGroupArray, sizeof(BoneWeightGroup)*mBoneWeightGroupCount);
	delete [] (lOldWeightGroupArray);

	mBoneWeightGroupArray[mBoneWeightGroupCount] = pWeights;
	mBoneWeightGroupCount = lNewCount;
}

void AnimatedGeometry::FreeAllBoneWeights()
{
	for (int x = 0; x < mBoneWeightGroupCount; ++x)
	{
		delete[] (mBoneWeightGroupArray[x].mVectorIndexArray);
		mBoneWeightGroupArray[x].mVectorIndexArray = 0;
		delete[] (mBoneWeightGroupArray[x].mVectorWeightArray);
		mBoneWeightGroupArray[x].mVectorWeightArray = 0;
	}
	delete[] (mBoneWeightGroupArray);
	mBoneWeightGroupCount = 0;
}

int AnimatedGeometry::GetBoneWeightGroupCount() const
{
	return (mBoneWeightGroupCount);
}

const AnimatedGeometry::BoneWeightGroup& AnimatedGeometry::GetBoneWeightGroup(int pBoneWeightGroupIndex) const
{
	return (mBoneWeightGroupArray[pBoneWeightGroupIndex]);
}



void AnimatedGeometry::SetBoneHierarchy(Tbc::BoneHierarchy* pBones)
{
	mBones = pBones;
}



void AnimatedGeometry::UpdateAnimatedGeometry()
{
	// TODO: this whole thing should usually be run on the GPU or any available vector processors.
	// Alex: ...but to do so while maintaining the goal of scalability, the following must be
	//       done...
	//
	//       Having two levels of materials (standard pipeline and pixel shaded materials), a
	//       third layer needs to be added - skinned materials. These materials only differ
	//       from the "standard" pixel shaded materials by their vertex shaders.
	//
	//       BUT!!! The code below can't be translated into a vertex shader!!! Why? Because
	//       there are no loops in shaders! (Not old version shaders, at least). We need to hard
	//       code things like how many bones there are per vertex. And the outer loop needs to
	//       be an outer loop in the renderer as well. This means one of three things:
	//
	//       1. We need to render the skinned material using several rendering passes (one
	//          pass per group), and between the passes we need to store the intermediate
	//          results somewhere, which means we need to use framebuffers. This CAN be faster
	//          than doing it on the CPU, but I wonder...
	//       2. OR we simply disallow more than one BoneWeightGroup.
	//       3. OR we write some weird translation routine that picks as many bones from
	//          different groups as the vertex shaders supports.
	//
	//       Is it worth the trouble? We might just stick with the code below and exploit the 
	//       fact that all future CPU:s have at least two cores.

	const float* lOriginalVData = mOriginalGeometry->GetVertexData();
	const float* lOriginalNData = mOriginalGeometry->GetNormalData();

	for (int x = 0; x < mBoneWeightGroupCount; ++x)
	{
		const BoneWeightGroup& lGroup = mBoneWeightGroupArray[x];
		deb_assert(lGroup.mBoneCount <= mBones->GetBoneCount());

		unsigned lVectorWeightIndex = 0;
		for (int y = 0; y < lGroup.mVectorIndexCount; ++y)
		{
			const unsigned lVectorIndex = lGroup.mVectorIndexArray[y] * 3;
			deb_assert(lVectorIndex+2 < mOriginalGeometry->GetVertexCount()*3);

			const vec3 v(lOriginalVData[lVectorIndex+0], lOriginalVData[lVectorIndex+1], lOriginalVData[lVectorIndex+2]);
			const vec3 n(lOriginalNData[lVectorIndex+0], lOriginalNData[lVectorIndex+1], lOriginalNData[lVectorIndex+2]);
			vec3 lWeightedVertex;
			vec3 lWeightedNormal;
			for (int z = 0; z < lGroup.mBoneCount; ++z, ++lVectorWeightIndex)
			{
				const int lBoneIndex = lGroup.mBoneIndexArray[z];
				const xform& lTransform = mBones->GetRelativeBoneTransformation(lBoneIndex);
				const float lWeight = lGroup.mVectorWeightArray[lVectorWeightIndex];
				lWeightedVertex += lTransform.Transform(v)*lWeight;
				lWeightedNormal += lTransform.Transform(n)*lWeight;
			}
			mVertexData[lVectorIndex+0] = lWeightedVertex.x;
			mVertexData[lVectorIndex+1] = lWeightedVertex.y;
			mVertexData[lVectorIndex+2] = lWeightedVertex.z;
			mNormalData[lVectorIndex+0] = lWeightedNormal.x;
			mNormalData[lVectorIndex+1] = lWeightedNormal.y;
			mNormalData[lVectorIndex+2] = lWeightedNormal.z;
		}
	}

	GeometryBase::SetVertexDataChanged(true);
}

Tbc::GeometryBase::GeometryVolatility AnimatedGeometry::GetGeometryVolatility() const
{
	return Tbc::GeometryBase::GEOM_VOLATILE;
}

void AnimatedGeometry::SetGeometryVolatility(GeometryVolatility)
{
}

unsigned int AnimatedGeometry::GetMaxVertexCount() const
{
	return mOriginalGeometry->GetMaxVertexCount();
}

unsigned int AnimatedGeometry::GetMaxIndexCount() const
{
	return mOriginalGeometry->GetMaxIndexCount();
}

unsigned int AnimatedGeometry::GetVertexCount() const
{
	return mOriginalGeometry->GetVertexCount();
}

unsigned int AnimatedGeometry::GetIndexCount() const
{
	return mOriginalGeometry->GetIndexCount();
}

unsigned int AnimatedGeometry::GetUVSetCount() const
{
	return mOriginalGeometry->GetUVSetCount();
}

float* AnimatedGeometry::GetVertexData() const
{
	return mVertexData;
}

float* AnimatedGeometry::GetUVData(unsigned int pUVSet) const
{
	return mOriginalGeometry->GetUVData(pUVSet);
}

vtx_idx_t* AnimatedGeometry::GetIndexData() const
{
	return mOriginalGeometry->GetIndexData();
}

uint8* AnimatedGeometry::GetColorData() const
{
	return mOriginalGeometry->GetColorData();
}

float* AnimatedGeometry::GetNormalData() const
{
	return mNormalData;
}

Tbc::GeometryBase::ColorFormat AnimatedGeometry::GetColorFormat() const
{
	return mOriginalGeometry->GetColorFormat();
}

Tbc::GeometryBase* AnimatedGeometry::GetOriginalGeometry()
{
	return mOriginalGeometry;
}



} // End namespace.