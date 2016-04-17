/*
	Class:  AnimatedGeometry
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#pragma once

#include "../../Lepra/Include/Vector3D.h"
#include "../../Tbc/Include/GeometryBase.h"
#include "UiTbc.h"



namespace Tbc
{
class BoneHierarchy;
}



namespace UiTbc
{

class AnimatedGeometry : public Tbc::GeometryBase
{
public:
	struct BoneWeightGroup
	{
		static const int MAX_BONES_PER_VERTEX = 4;	// TODO: move this constant out, make it more global.
		int mBoneCount;
		int mBoneIndexArray[MAX_BONES_PER_VERTEX];
		int mVectorIndexCount;		// Used both for vertices and normals.
		int* mVectorIndexArray;	// Used both for vertices and normals.
		float* mVectorWeightArray;	// Used both for vertices and normals.
	};

	AnimatedGeometry();
	virtual ~AnimatedGeometry();

	void SetGeometry(Tbc::GeometryBase* pGeometry);

	// Takes ownership of the allocated indices and weights.
	void AddBoneWeights(BoneWeightGroup& pWeights);
	void FreeAllBoneWeights();
	int GetBoneWeightGroupCount() const;
	const BoneWeightGroup& GetBoneWeightGroup(int pBoneWeightGroupIndex) const;

	void SetBoneHierarchy(Tbc::BoneHierarchy* pBones);	// TODO: replace this with user resource object?

	void UpdateAnimatedGeometry();

	GeometryVolatility GetGeometryVolatility() const;
	void SetGeometryVolatility(GeometryVolatility);

	unsigned int GetMaxVertexCount()  const;
	unsigned int GetMaxIndexCount() const;

	unsigned int GetVertexCount()  const;
	unsigned int GetIndexCount() const;
	unsigned int GetUVSetCount()    const;

	float*         GetVertexData() const;
	float*         GetUVData(unsigned int pUVSet) const;
	vtx_idx_t*     GetIndexData() const;
	uint8*         GetColorData() const;
	float*         GetNormalData() const;

	ColorFormat GetColorFormat() const;

	Tbc::GeometryBase* GetOriginalGeometry();

protected:
private:
	Tbc::GeometryBase* mOriginalGeometry;

	Tbc::BoneHierarchy* mBones;

	int mVertexCount;
	float* mVertexData;
	float* mNormalData;
	int mBoneWeightGroupCount;
	BoneWeightGroup* mBoneWeightGroupArray;
};



}
