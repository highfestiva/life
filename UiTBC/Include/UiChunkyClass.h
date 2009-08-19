
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



#pragma once

#include "UiTbc.h"
#include "../../Lepra/Include/Transformation.h"
#include "../../TBC/Include/ChunkyClass.h"



namespace UiTbc
{



class ChunkyClass: public TBC::ChunkyClass
{
public:
	ChunkyClass();
	virtual ~ChunkyClass();

	void AddMesh(int pPhysIndex, const Lepra::String& pMeshBaseName, const Lepra::TransformationF& pTransform);
	size_t GetMeshCount() const;
	void GetMesh(size_t pIndex, int& pPhysIndex, Lepra::String& pMeshBaseName, Lepra::TransformationF& pTransform);

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
	};
	std::vector<PhysMeshEntry> mMeshArray;

	LOG_CLASS_DECLARE();
};



}
