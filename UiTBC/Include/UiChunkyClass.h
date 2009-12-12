
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "UiTbc.h"
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
		Lepra::Vector3DF mAmbient;
		Lepra::Vector3DF mDiffuse;
		Lepra::Vector3DF mSpecular;
		float mShininess;
		float mAlpha;
		std::vector<Lepra::UnicodeString> mTextureList;
		Lepra::UnicodeString mShaderName;

		Material();
	};

	ChunkyClass();
	virtual ~ChunkyClass();

	void AddMesh(int pPhysIndex, const Lepra::String& pMeshBaseName, const Lepra::TransformationF& pTransform);
	size_t GetMeshCount() const;
	void GetMesh(size_t pIndex, int& pPhysIndex, Lepra::String& pMeshBaseName, Lepra::TransformationF& pTransform) const;
	void SetLastMeshMaterial(const Material& pMaterial);
	const Material& GetMaterial(size_t pMeshIndex) const;

private:
	struct PhysMeshEntry
	{
		PhysMeshEntry(int pPhysIndex, const Lepra::String& pMeshBaseName,
			const Lepra::TransformationF& pTransform):
			mPhysIndex(pPhysIndex),
			mMeshBaseName(pMeshBaseName),
			mTransform(pTransform)
		{
		}
		int mPhysIndex;
		Lepra::String mMeshBaseName;
		Lepra::TransformationF mTransform;
		Material mMaterial;
	};
	std::vector<PhysMeshEntry> mMeshArray;

	LOG_CLASS_DECLARE();
};



}
