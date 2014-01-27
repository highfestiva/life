
// Author: Jonas Bystr�m
// Copyright (c) Pixel Doctrine



#pragma once

#include "UiTBC.h"
#include "../../Lepra/Include/Canvas.h"
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
		Canvas::ResizeHint mResizeHint;
		std::vector<str> mTextureList;
		str mShaderName;

		Material();
	};

	ChunkyClass();
	virtual ~ChunkyClass();

	void AddMesh(int pPhysIndex, const str& pMeshBaseName, const TransformationF& pTransform, float pScale);
	size_t GetMeshCount() const;
	void GetMesh(size_t pIndex, int& pPhysIndex, str& pMeshBaseName, TransformationF& pTransform, float& pScale) const;
	void SetLastMeshMaterial(const Material& pMaterial);
	const Material& GetMaterial(size_t pMeshIndex) const;

private:
	struct PhysMeshEntry
	{
		PhysMeshEntry(int pPhysIndex, const str& pMeshBaseName,
			const TransformationF& pTransform, float pScale):
			mPhysIndex(pPhysIndex),
			mMeshBaseName(pMeshBaseName),
			mTransform(pTransform),
			mScale(pScale)
		{
		}
		int mPhysIndex;
		str mMeshBaseName;
		TransformationF mTransform;
		float mScale;
		Material mMaterial;
	};
	typedef std::vector<PhysMeshEntry> PhysMeshArray;
	PhysMeshArray mMeshArray;

	LOG_CLASS_DECLARE();
};



}
