
// Author: Jonas Bystr�m
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "UiTBC.h"
#include "../../Lepra/Include/Graphics2D.h"
#include "../../Lepra/Include/Transformation.h"
#include "../../TBC/Include/ChunkyClass.h"



namespace UiTbc
{



class ChunkyClass: public TBC::ChunkyClass
{
public:
	struct Material
	{
		Vector3DF mAmbient;
		Vector3DF mDiffuse;
		Vector3DF mSpecular;
		float mShininess;
		float mAlpha;
		std::vector<str> mTextureList;
		str mShaderName;

		Material();
	};

	struct Tag
	{
		str mTagName;
		std::vector<float> mFloatValueList;
		std::vector<int> mBodyIndexList;
		std::vector<int> mEngineIndexList;
		std::vector<int> mMeshIndexList;
	};

	ChunkyClass();
	virtual ~ChunkyClass();

	void AddMesh(int pPhysIndex, const str& pMeshBaseName, const TransformationF& pTransform);
	size_t GetMeshCount() const;
	void GetMesh(size_t pIndex, int& pPhysIndex, str& pMeshBaseName, TransformationF& pTransform) const;
	void SetLastMeshMaterial(const Material& pMaterial);
	const Material& GetMaterial(size_t pMeshIndex) const;

	bool UnpackTag(uint8* pBuffer, unsigned pSize);
	void AddTag(const Tag& pTag);
	size_t GetTagCount() const;
	const Tag& GetTag(size_t pTagIndex) const;

private:
	struct PhysMeshEntry
	{
		PhysMeshEntry(int pPhysIndex, const str& pMeshBaseName,
			const TransformationF& pTransform):
			mPhysIndex(pPhysIndex),
			mMeshBaseName(pMeshBaseName),
			mTransform(pTransform)
		{
		}
		int mPhysIndex;
		str mMeshBaseName;
		TransformationF mTransform;
		Material mMaterial;
	};
	std::vector<PhysMeshEntry> mMeshArray;
	std::vector<Tag> mTagArray;

	LOG_CLASS_DECLARE();
};



}
