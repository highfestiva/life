/*
	Class:  AnimatedGeometry
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games
*/

#ifndef UIANIMATEDGEOMETRY_H
#define UIANIMATEDGEOMETRY_H



#include "../../Lepra/Include/Vector3D.h"
#include "../../TBC/Include/GeometryBase.h"
#include "UiTBC.h"



namespace TBC
{
class BoneHierarchy;
}



namespace UiTbc
{

class AnimatedGeometry : public TBC::GeometryBase
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

	void SetGeometry(TBC::GeometryBase* pGeometry);

	// Takes ownership of the allocated indices and weights.
	void AddBoneWeights(BoneWeightGroup& pWeights);
	void FreeAllBoneWeights();
	int GetBoneWeightGroupCount() const;
	const BoneWeightGroup& GetBoneWeightGroup(int pBoneWeightGroupIndex) const;

	void SetBoneHierarchy(TBC::BoneHierarchy* pBones);	// TODO: replace this with user resource object?

	void UpdateAnimatedGeometry();

	PrimitiveType GetPrimitiveType() const;

	GeometryVolatility GetGeometryVolatility() const;
	void SetGeometryVolatility(GeometryVolatility);

	unsigned int GetMaxVertexCount()  const;
	unsigned int GetMaxIndexCount() const;

	unsigned int GetVertexCount()  const;
	unsigned int GetIndexCount() const;
	unsigned int GetUVSetCount()    const;

	float*         GetVertexData() const;
	float*         GetUVData(unsigned int pUVSet) const;
	Lepra::uint32* GetIndexData() const;
	Lepra::uint8*  GetColorData() const;
	float*         GetNormalData() const;

	ColorFormat GetColorFormat() const;

	TBC::GeometryBase* GetOriginalGeometry();

protected:
private:
	TBC::GeometryBase* mOriginalGeometry;

	TBC::BoneHierarchy* mBones;

	int mVertexCount;
	float* mVertexData;
	float* mNormalData;
	int mBoneWeightGroupCount;
	BoneWeightGroup* mBoneWeightGroupArray;
};



} // End namespace.

#endif
