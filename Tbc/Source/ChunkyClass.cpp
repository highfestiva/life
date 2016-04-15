
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/ChunkyClass.h"
#include "../../Lepra/Include/LepraAssert.h"
#include "../../Lepra/Include/Endian.h"
#include "../../Lepra/Include/HashUtil.h"
#include "../../Lepra/Include/Packer.h"



namespace Tbc
{



ChunkyClass::ChunkyClass()
{
}

ChunkyClass::~ChunkyClass()
{
}



const str& ChunkyClass::GetPhysicsBaseName() const
{
	return (mPhysicsBaseName);
}

str& ChunkyClass::GetPhysicsBaseName()
{
	return (mPhysicsBaseName);
}



bool ChunkyClass::UnpackTag(uint8* pBuffer, unsigned pSize)
{
	bool lOk = true;
	int lIndex = 0;
	Tag lTag;
	lTag.mTagName = "<unknown>";
	if (lOk)
	{
		int lStrSize = PackerUnicodeString::Unpack(lTag.mTagName, &pBuffer[lIndex], pSize-lIndex);
		lStrSize = (lStrSize+3)&(~3);
		lOk = (lIndex+lStrSize < (int)pSize);
		deb_assert(lOk);
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
		const int32 lStringValueCount = Endian::BigToHost(*(int32*)&pBuffer[lIndex]);
		lIndex += sizeof(lStringValueCount);
		for (int x = 0; lOk && x < lStringValueCount; ++x)
		{
			str lValue;
			int lStrSize = PackerUnicodeString::Unpack(lValue, &pBuffer[lIndex], pSize-lIndex);
			lStrSize = (lStrSize+3)&(~3);
			lOk = (lIndex+lStrSize < (int)pSize);
			deb_assert(lOk);
			if (!lOk)
			{
				mLog.Errorf("String index %i had wrong length (%i).", x, lStrSize);
			}
			lIndex += lStrSize;
			lTag.mStringValueList.push_back(lValue);
		}
	}
	if (lOk)
	{
		const int32 lBodyIndexCount = Endian::BigToHost(*(int32*)&pBuffer[lIndex]);
		lIndex += sizeof(lBodyIndexCount);
		for (int x = 0; x < lBodyIndexCount; ++x)
		{
			const int32 lBodyIndex = Endian::BigToHost(*(int32*)&pBuffer[lIndex]);
			deb_assert(lBodyIndex >= 0 && lBodyIndex < 200);
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
			deb_assert(lEngineIndex >= 0 && lEngineIndex < 20);
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
			//deb_assert(lMeshIndex >= 0 && lMeshIndex < (int32)GetMeshCount());
			lIndex += sizeof(lMeshIndex);
			lTag.mMeshIndexList.push_back(lMeshIndex);
		}
	}
	lOk = (lIndex == (int)pSize);
	deb_assert(lOk);
	if (lOk)
	{
		AddTag(lTag);
	}
	else
	{
		mLog.Errorf("File error: could not unpack class tag of type %s.", lTag.mTagName.c_str());
	}
	return (lOk);
}

void ChunkyClass::AddTag(const Tag& pTag)
{
	mTagArray.push_back(pTag);
}

void ChunkyClass::RemoveTag(size_t pTagIndex)
{
	if (pTagIndex >= GetTagCount())
	{
		deb_assert(false);
		return;
	}
	mTagArray.erase(mTagArray.begin()+pTagIndex);
}

size_t ChunkyClass::GetTagCount() const
{
	return (mTagArray.size());
}

const ChunkyClass::Tag& ChunkyClass::GetTag(size_t pTagIndex) const
{
	deb_assert(pTagIndex < GetTagCount());
	return (mTagArray[pTagIndex]);
}

const ChunkyClass::Tag* ChunkyClass::GetTag(const str& pTagName) const
{
	for (size_t x = 0, n = GetTagCount(); x < n; ++x)
	{
		const ChunkyClass::Tag& lTag = mTagArray[x];
		if (lTag.mTagName == pTagName)
		{
			return &lTag;
		}
	}
	return 0;
}



void ChunkyClass::AddPhysRoot(int pPhysIndex)
{
	mPhysRootSet.insert(pPhysIndex);
}

bool ChunkyClass::IsPhysRoot(int pPhysIndex) const
{
	return (HashUtil::FindSetObjectDefault(mPhysRootSet, pPhysIndex, -1) == pPhysIndex);
}



loginstance(GAME_CONTEXT, ChunkyClass);



}
