
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "UiTbc.h"
#include "../../Lepra/Include/Canvas.h"
#include "../../Lepra/Include/Transformation.h"
#include "../../Tbc/Include/ChunkyClass.h"



namespace UiTbc
{



class ChunkyClass: public Tbc::ChunkyClass
{
public:
	struct Material
	{
		vec3 mAmbient;
		vec3 mDiffuse;
		vec3 mSpecular;
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

	void AddMesh(int pPhysIndex, const str& pMeshBaseName, const xform& pTransform, float pScale);
	size_t GetMeshCount() const;
	void GetMesh(size_t pIndex, int& pPhysIndex, str& pMeshBaseName, xform& pTransform, float& pScale) const;
	void SetLastMeshMaterial(const Material& pMaterial);
	const Material& GetMaterial(size_t pMeshIndex) const;

private:
	struct PhysMeshEntry
	{
		PhysMeshEntry(int pPhysIndex, const str& pMeshBaseName,
			const xform& pTransform, float pScale):
			mPhysIndex(pPhysIndex),
			mMeshBaseName(pMeshBaseName),
			mTransform(pTransform),
			mScale(pScale)
		{
		}
		int mPhysIndex;
		str mMeshBaseName;
		xform mTransform;
		float mScale;
		Material mMaterial;
	};
	typedef std::vector<PhysMeshEntry> PhysMeshArray;
	PhysMeshArray mMeshArray;

	logclass();
};



}
