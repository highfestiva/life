
// Author: Jonas Byström
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
		bool mSmooth;
		std::vector<str> mTextureList;
		str mShaderName;

		Material();
	};

	ChunkyClass();
	virtual ~ChunkyClass();

	void AddMesh(int pPhysIndex, const str& pMeshBaseName, const TransformationF& pTransform);
	size_t GetMeshCount() const;
	void GetMesh(size_t pIndex, int& pPhysIndex, str& pMeshBaseName, TransformationF& pTransform) const;
	void SetLastMeshMaterial(const Material& pMaterial);
	const Material& GetMaterial(size_t pMeshIndex) const;

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
	typedef std::vector<PhysMeshEntry> PhysMeshArray;
	PhysMeshArray mMeshArray;

	LOG_CLASS_DECLARE();
};



}
