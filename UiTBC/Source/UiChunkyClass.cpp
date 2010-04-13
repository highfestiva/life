
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/UiChunkyClass.h"
#include <assert.h>
#include "../../Lepra/Include/Endian.h"
#include "../../Lepra/Include/Packer.h"
#include "../../TBC/Include/ChunkyPhysics.h"



namespace UiTbc
{



ChunkyClass::Material::Material():
	mAmbient(0,0,0),
	mDiffuse(1,1,1),
	mSpecular(0.1f,0.1f,0.1f),
	mShininess(0),
	mAlpha(1)
{
}



ChunkyClass::ChunkyClass()
{
}

ChunkyClass::~ChunkyClass()
{
}



void ChunkyClass::AddMesh(int pPhysIndex, const str& pMeshBaseName, const TransformationF& pTransform)
{
	mMeshArray.push_back(PhysMeshEntry(pPhysIndex, pMeshBaseName, pTransform));
}

size_t ChunkyClass::GetMeshCount() const
{
	return (mMeshArray.size());
}

void ChunkyClass::GetMesh(size_t pIndex, int& pPhysIndex, str& pMeshBaseName,
	TransformationF& pTransform) const
{
	assert(pIndex < mMeshArray.size());
	pPhysIndex = mMeshArray[pIndex].mPhysIndex;
	pMeshBaseName = mMeshArray[pIndex].mMeshBaseName;
	pTransform = mMeshArray[pIndex].mTransform;
}

void ChunkyClass::SetLastMeshMaterial(const Material& pMaterial)
{
	assert(!mMeshArray.empty());
	const size_t lLastMesh = mMeshArray.size()-1;
	mMeshArray[lLastMesh].mMaterial = pMaterial;
}

const ChunkyClass::Material& ChunkyClass::GetMaterial(size_t pMeshIndex) const
{
	assert(pMeshIndex < mMeshArray.size());
	return (mMeshArray[pMeshIndex].mMaterial);
}



bool ChunkyClass::UnpackTag(uint8* pBuffer, unsigned pSize)
{
	bool lOk = true;
	int lIndex = 0;
	Tag lTag;
	lTag.mTagName = _T("<unknown>");
	if (lOk)
	{
		int lStrSize = PackerUnicodeString::Unpack(lTag.mTagName, &pBuffer[lIndex], pSize-lIndex);
		lStrSize = (lStrSize+3)&(~3);
		lOk = (lIndex+lStrSize < (int)pSize);
		assert(lOk);
		lIndex += lStrSize;
	}
	if (lOk)
	{
		const int32 lFloatValueCount = Endian::BigToHost(*(int32*)&pBuffer[lIndex]);
		lIndex += sizeof(lFloatValueCount);
		for (int x = 0; x < lFloatValueCount; ++x)
		{
			const float lValue = Endian::BigToHostF(*(uint32*)&pBuffer[lIndex]);
			lIndex += sizeof(lValue);
			lTag.mFloatValueList.push_back(lValue);
		}
	}
	if (lOk)
	{
		const int32 lBodyIndexCount = Endian::BigToHost(*(int32*)&pBuffer[lIndex]);
		lIndex += sizeof(lBodyIndexCount);
		for (int x = 0; x < lBodyIndexCount; ++x)
		{
			const int32 lBodyIndex = Endian::BigToHost(*(int32*)&pBuffer[lIndex]);
			assert(lBodyIndex >= 0 && lBodyIndex < 200);
			lIndex += sizeof(lBodyIndex);
			lTag.mBodyIndexList.push_back(lBodyIndex);
		}
	}
	if (lOk)
	{
		const int32 lEngineIndexCount = Endian::BigToHost(*(int32*)&pBuffer[lIndex]);
		lIndex += sizeof(lEngineIndexCount);
		for (int x = 0; x < lEngineIndexCount; ++x)
		{
			const int32 lEngineIndex = Endian::BigToHost(*(int32*)&pBuffer[lIndex]);
			assert(lEngineIndex >= 0 && lEngineIndex < 20);
			lIndex += sizeof(lEngineIndex);
			lTag.mEngineIndexList.push_back(lEngineIndex);
		}
	}
	if (lOk)
	{
		const int32 lMeshIndexCount = Endian::BigToHost(*(int32*)&pBuffer[lIndex]);
		lIndex += sizeof(lMeshIndexCount);
		for (int x = 0; x < lMeshIndexCount; ++x)
		{
			const int32 lMeshIndex = Endian::BigToHost(*(int32*)&pBuffer[lIndex]);
			assert(lMeshIndex >= 0 && lMeshIndex < (int32)GetMeshCount());
			lIndex += sizeof(lMeshIndex);
			lTag.mMeshIndexList.push_back(lMeshIndex);
		}
	}
	lOk = (lIndex == (int)pSize);
	assert(lOk);
	if (lOk)
	{
		AddTag(lTag);
	}
	else
	{
		mLog.Errorf(_T("File error: could not unpack class tag of type %s."), lTag.mTagName.c_str());
	}
	return (lOk);
}

void ChunkyClass::AddTag(const Tag& pTag)
{
	mTagArray.push_back(pTag);
}

size_t ChunkyClass::GetTagCount() const
{
	return (mTagArray.size());
}

const ChunkyClass::Tag& ChunkyClass::GetTag(size_t pTagIndex) const
{
	assert(pTagIndex < GetTagCount());
	return (mTagArray[pTagIndex]);
}



LOG_CLASS_DEFINE(GAME_CONTEXT, ChunkyClass);



}
