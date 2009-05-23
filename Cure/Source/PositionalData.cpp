
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games



#include <assert.h>
#include "../Include/Packer.h"
#include "../Include/PositionalData.h"



namespace Cure
{



#define CHECK_SIZE(size)		\
	if (pSize < (int)(size))	\
	{				\
		assert(false);		\
		return (-1);		\
	}

#define CHECK_TYPE(type)			\
	if (type != (Type)pData[lSize++])	\
	{					\
		assert(false);			\
		return (-1);			\
	}



PositionalData::PositionalData():
	mScale(1)
{
}

PositionalData::~PositionalData()
{
}

float PositionalData::GetScaledDifference(const PositionalData* pReference) const
{
	return (mScale*GetBiasedDifference(pReference));
}

void PositionalData::SetScale(float pScale)
{
	mScale = pScale;
}



int PositionalData6::GetStaticPackSize()
{
	return (1+sizeof(Lepra::TransformationF::BaseType)*7 + sizeof(Lepra::Vector3DF::BaseType)*3*4);
}

int PositionalData6::GetPackSize() const
{
	return (GetStaticPackSize());
}

int PositionalData6::Pack(Lepra::uint8* pData) const
{
	int lSize = 0;
	pData[lSize++] = (Lepra::uint8)TYPE_POSITION_6;
	lSize += Cure::PackerTransformation::Pack(&pData[lSize], mTransformation);
	lSize += Cure::PackerVector::Pack(&pData[lSize], mVelocity);
	lSize += Cure::PackerVector::Pack(&pData[lSize], mAcceleration);
	lSize += Cure::PackerVector::Pack(&pData[lSize], mAngularVelocity);
	lSize += Cure::PackerVector::Pack(&pData[lSize], mAngularAcceleration);
	return (lSize);
}

int PositionalData6::Unpack(const Lepra::uint8* pData, int pSize)
{
	CHECK_SIZE(1+sizeof(float)*7+sizeof(float)*3*4);
	int lSize = 0;
	CHECK_TYPE(TYPE_POSITION_6);
	lSize += Cure::PackerTransformation::Unpack(mTransformation, &pData[lSize], sizeof(float)*7);
	lSize += Cure::PackerVector::Unpack(mVelocity, &pData[lSize], sizeof(float)*3);
	lSize += Cure::PackerVector::Unpack(mAcceleration, &pData[lSize], sizeof(float)*3);
	lSize += Cure::PackerVector::Unpack(mAngularVelocity, &pData[lSize], sizeof(float)*3);
	lSize += Cure::PackerVector::Unpack(mAngularAcceleration, &pData[lSize], sizeof(float)*3);
	return (lSize);
}

float PositionalData6::GetBiasedDifference(const PositionalData* pReference) const
{
	const PositionalData6& lReference = (const PositionalData6&)*pReference;
	float lWeightedDifferenceSum =
		(GetDifference(mTransformation.GetOrientation(), lReference.mTransformation.GetOrientation())*1.0f +
		GetDifference(mTransformation.GetPosition(), lReference.mTransformation.GetPosition())*1.0f +
		GetDifference(mVelocity, lReference.mVelocity)*1.0f +
		GetDifference(mAcceleration, lReference.mAcceleration)*3.0f +	// Linear acceleration is by far the most important one when doing some simple "CrocketBalls".
		GetDifference(mAngularVelocity, lReference.mAngularVelocity)*1.0f +
		GetDifference(mAngularAcceleration, lReference.mAngularAcceleration)*1.0f);
	return (lWeightedDifferenceSum);
}

float PositionalData6::GetDifference(const Lepra::QuaternionF& pQ1, const Lepra::QuaternionF& pQ2)
{
	float lWeightedDifferenceSum =
		(::fabs(pQ1.GetA()-pQ2.GetA()) +
		::fabs(pQ1.GetB()-pQ2.GetB()) +
		::fabs(pQ1.GetC()-pQ2.GetC()) +
		::fabs(pQ1.GetD()-pQ2.GetD()));
	return (lWeightedDifferenceSum);
}

float PositionalData6::GetDifference(const Lepra::Vector3DF& pV1, const Lepra::Vector3DF& pV2)
{
	float lWeightedDifferenceSum =
		(::fabs(pV1.x-pV2.x) +
		::fabs(pV1.y-pV2.y) +
		::fabs(pV1.z-pV2.z));
	return (lWeightedDifferenceSum);
}

PositionalData::Type PositionalData6::GetType() const
{
	return (TYPE_POSITION_6);
}

void PositionalData6::CopyData(const PositionalData* pData)
{
	assert(pData->GetType() == GetType());
	const PositionalData6& lCopy = *(PositionalData6*)pData;
	*this = lCopy;
}

PositionalData* PositionalData6::Clone()
{
	return (new PositionalData6(*this));
}

LOG_CLASS_DEFINE(NETWORK, PositionalData6);



int PositionalData3::GetPackSize() const
{
	return (1+sizeof(float)*3*3);
}

int PositionalData3::Pack(Lepra::uint8* pData) const
{
	int lSize = 0;
	pData[lSize++] = (Lepra::uint8)TYPE_POSITION_3;
	lSize += Cure::PackerReal::Pack(&pData[lSize], mTransformation[0]);
	lSize += Cure::PackerReal::Pack(&pData[lSize], mTransformation[1]);
	lSize += Cure::PackerReal::Pack(&pData[lSize], mTransformation[2]);
	lSize += Cure::PackerReal::Pack(&pData[lSize], mVelocity[0]);
	lSize += Cure::PackerReal::Pack(&pData[lSize], mVelocity[1]);
	lSize += Cure::PackerReal::Pack(&pData[lSize], mVelocity[2]);
	lSize += Cure::PackerReal::Pack(&pData[lSize], mAcceleration[0]);
	lSize += Cure::PackerReal::Pack(&pData[lSize], mAcceleration[1]);
	lSize += Cure::PackerReal::Pack(&pData[lSize], mAcceleration[2]);
	return (lSize);
}

int PositionalData3::Unpack(const Lepra::uint8* pData, int pSize)
{
	CHECK_SIZE(1+sizeof(mTransformation[0])*3*3);
	int lSize = 0;
	CHECK_TYPE(TYPE_POSITION_3);
	lSize += Cure::PackerReal::Unpack(mTransformation[0], &pData[lSize], sizeof(float));
	lSize += Cure::PackerReal::Unpack(mTransformation[1], &pData[lSize], sizeof(float));
	lSize += Cure::PackerReal::Unpack(mTransformation[2], &pData[lSize], sizeof(float));
	lSize += Cure::PackerReal::Unpack(mVelocity[0], &pData[lSize], sizeof(float));
	lSize += Cure::PackerReal::Unpack(mVelocity[1], &pData[lSize], sizeof(float));
	lSize += Cure::PackerReal::Unpack(mVelocity[2], &pData[lSize], sizeof(float));
	lSize += Cure::PackerReal::Unpack(mAcceleration[0], &pData[lSize], sizeof(float));
	lSize += Cure::PackerReal::Unpack(mAcceleration[1], &pData[lSize], sizeof(float));
	lSize += Cure::PackerReal::Unpack(mAcceleration[2], &pData[lSize], sizeof(float));
	return (lSize);
}

float PositionalData3::GetBiasedDifference(const PositionalData* pReference) const
{
	const PositionalData3& lReference = (const PositionalData3&)*pReference;
	float lWeightedDifferenceSum =
		::fabs(mTransformation[0]-lReference.mTransformation[0]) +
		::fabs(mTransformation[1]-lReference.mTransformation[1]) +
		::fabs(mTransformation[2]-lReference.mTransformation[2]) +
		::fabs(mVelocity[0]-lReference.mVelocity[0])*3.0f +		// Linear acceleration seems by far the most important one.
		::fabs(mVelocity[1]-lReference.mVelocity[1]) +
		::fabs(mVelocity[2]-lReference.mVelocity[2]) +
		::fabs(mAcceleration[0]-lReference.mAcceleration[0]) +
		::fabs(mAcceleration[1]-lReference.mAcceleration[1]) +
		::fabs(mAcceleration[2]-lReference.mAcceleration[2]);
	return (lWeightedDifferenceSum);
}

PositionalData::Type PositionalData3::GetType() const
{
	return (TYPE_POSITION_3);
}

void PositionalData3::CopyData(const PositionalData* pData)
{
	assert(pData->GetType() == GetType());
	const PositionalData3& lCopy = *(PositionalData3*)pData;
	*this = lCopy;
}

PositionalData* PositionalData3::Clone()
{
	return (new PositionalData3(*this));
}



int PositionalData2::GetPackSize() const
{
	return (1+sizeof(float)*2*3);
}

int PositionalData2::Pack(Lepra::uint8* pData) const
{
	int lSize = 0;
	pData[lSize++] = (Lepra::uint8)TYPE_POSITION_2;
	lSize += Cure::PackerReal::Pack(&pData[lSize], mTransformation[0]);
	lSize += Cure::PackerReal::Pack(&pData[lSize], mTransformation[1]);
	lSize += Cure::PackerReal::Pack(&pData[lSize], mVelocity[0]);
	lSize += Cure::PackerReal::Pack(&pData[lSize], mVelocity[1]);
	lSize += Cure::PackerReal::Pack(&pData[lSize], mAcceleration[0]);
	lSize += Cure::PackerReal::Pack(&pData[lSize], mAcceleration[1]);
	return (lSize);
}

int PositionalData2::Unpack(const Lepra::uint8* pData, int pSize)
{
	CHECK_SIZE(1+sizeof(mTransformation[0])*2*3);
	int lSize = 0;
	CHECK_TYPE(TYPE_POSITION_2);
	lSize += Cure::PackerReal::Unpack(mTransformation[0], &pData[lSize], sizeof(float));
	lSize += Cure::PackerReal::Unpack(mTransformation[1], &pData[lSize], sizeof(float));
	lSize += Cure::PackerReal::Unpack(mVelocity[0], &pData[lSize], sizeof(float));
	lSize += Cure::PackerReal::Unpack(mVelocity[1], &pData[lSize], sizeof(float));
	lSize += Cure::PackerReal::Unpack(mAcceleration[0], &pData[lSize], sizeof(float));
	lSize += Cure::PackerReal::Unpack(mAcceleration[1], &pData[lSize], sizeof(float));
	return (lSize);
}

float PositionalData2::GetBiasedDifference(const PositionalData* pReference) const
{
	const PositionalData2& lReference = (const PositionalData2&)*pReference;
	float lWeightedDifferenceSum =
		::fabs(mTransformation[0]-lReference.mTransformation[0]) +
		::fabs(mTransformation[1]-lReference.mTransformation[1]) +
		::fabs(mVelocity[0]-lReference.mVelocity[0])*3.0f +		// Linear acceleration seems by far the most important one.
		::fabs(mVelocity[1]-lReference.mVelocity[1]) +
		::fabs(mAcceleration[0]-lReference.mAcceleration[0]) +
		::fabs(mAcceleration[1]-lReference.mAcceleration[1]);
	return (lWeightedDifferenceSum);
}

PositionalData::Type PositionalData2::GetType() const
{
	return (TYPE_POSITION_2);
}

void PositionalData2::CopyData(const PositionalData* pData)
{
	assert(pData->GetType() == GetType());
	const PositionalData2& lCopy = *(PositionalData2*)pData;
	*this = lCopy;
}

PositionalData* PositionalData2::Clone()
{
	return (new PositionalData2(*this));
}



int PositionalData1::GetPackSize() const
{
	return (1+sizeof(float)*1*3);
}

int PositionalData1::Pack(Lepra::uint8* pData) const
{
	int lSize = 0;
	pData[lSize++] = (Lepra::uint8)TYPE_POSITION_1;
	lSize += Cure::PackerReal::Pack(&pData[lSize], mTransformation);
	lSize += Cure::PackerReal::Pack(&pData[lSize], mVelocity);
	lSize += Cure::PackerReal::Pack(&pData[lSize], mAcceleration);
	return (lSize);
}

int PositionalData1::Unpack(const Lepra::uint8* pData, int pSize)
{
	CHECK_SIZE(1+sizeof(mTransformation)*1*3);
	int lSize = 0;
	CHECK_TYPE(TYPE_POSITION_1);
	lSize += Cure::PackerReal::Unpack(mTransformation, &pData[lSize], sizeof(float));
	lSize += Cure::PackerReal::Unpack(mVelocity, &pData[lSize], sizeof(float));
	lSize += Cure::PackerReal::Unpack(mAcceleration, &pData[lSize], sizeof(float));
	return (lSize);
}

float PositionalData1::GetBiasedDifference(const PositionalData* pReference) const
{
	const PositionalData1& lReference = (const PositionalData1&)*pReference;
	float lWeightedDifferenceSum =
		::fabs(mTransformation-lReference.mTransformation) +
		::fabs(mVelocity-lReference.mVelocity)*3.0f +		// Linear acceleration seems by far the most important one.
		::fabs(mAcceleration-lReference.mAcceleration);
	return (lWeightedDifferenceSum);
}

PositionalData::Type PositionalData1::GetType() const
{
	return (TYPE_POSITION_1);
}

void PositionalData1::CopyData(const PositionalData* pData)
{
	assert(pData->GetType() == GetType());
	const PositionalData1& lCopy = *(PositionalData1*)pData;
	*this = lCopy;
}

PositionalData* PositionalData1::Clone()
{
	return (new PositionalData1(*this));
}



ObjectPositionalData::ObjectPositionalData()
{
}

ObjectPositionalData::~ObjectPositionalData()
{
	Clear();
}

void ObjectPositionalData::Clear()
{
	BodyPositionArray::iterator x = mBodyPositionArray.begin();
	for (; x != mBodyPositionArray.end(); ++x)
	{
		PositionalData* lPosition = *x;
		delete (lPosition);
	}
	mBodyPositionArray.clear();
}

int ObjectPositionalData::GetPackSize() const
{
	int lSize = 1+sizeof(Lepra::int16)+mPosition.GetPackSize();
	BodyPositionArray::const_iterator x = mBodyPositionArray.begin();
	for (; x != mBodyPositionArray.end(); ++x)
	{
		lSize += (*x)->GetPackSize();
	}
	return (lSize);
}

int ObjectPositionalData::Pack(Lepra::uint8* pData) const
{
	int lSize = 0;
	pData[lSize++] = (Lepra::uint8)TYPE_OBJECT;
	lSize += PackerInt16::Pack(&pData[lSize], GetPackSize()-1-sizeof(Lepra::int16));
	lSize += mPosition.Pack(&pData[lSize]);
	BodyPositionArray::const_iterator x = mBodyPositionArray.begin();
	for (; x != mBodyPositionArray.end(); ++x)
	{
		lSize += (*x)->Pack(&pData[lSize]);
	}
	return (lSize);
}

int ObjectPositionalData::Unpack(const Lepra::uint8* pData, int pSize)
{
	CHECK_SIZE((int)(1+sizeof(Lepra::int16)+PositionalData6::GetStaticPackSize()));
	int lSize = 0;
	CHECK_TYPE(TYPE_OBJECT);
	assert(mBodyPositionArray.size() == 0);
	int lDataSize;
	int lSubSize = PackerInt16::Unpack(lDataSize, &pData[lSize], pSize-lSize);
	if (lSubSize <= 0)
	{
		assert(false);
		return (-1);
	}
	lSize += lSubSize;
	if (lDataSize < PositionalData6::GetStaticPackSize() || lDataSize > pSize-lSize)
	{
		assert(false);
		return (-1);
	}
	pSize = lDataSize+lSize;
	lSubSize = mPosition.Unpack(&pData[lSize], pSize-lSize);
	if (lSubSize <= 0)
	{
		assert(false);
		return (-1);
	}
	lSize += lSubSize;
	while (lSize < pSize)
	{
		PositionalData* lPosition = 0;
		switch (pData[lSize])
		{
			case TYPE_POSITION_6:	lPosition = new PositionalData6;	break;
			case TYPE_POSITION_3:	lPosition = new PositionalData3;	break;
			case TYPE_POSITION_2:	lPosition = new PositionalData2;	break;
			case TYPE_POSITION_1:	lPosition = new PositionalData1;	break;
			case TYPE_REAL_4:	lPosition = new RealData4;		break;
			case TYPE_REAL_1_BOOL:	lPosition = new RealData1Bool;		break;
			case TYPE_REAL_1:	lPosition = new RealData1;		break;
		}
		if (!lPosition)
		{
			assert(false);
			lSize = -1;
			break;
		}
		mBodyPositionArray.push_back(lPosition);
		lSubSize = lPosition->Unpack(&pData[lSize], pSize-lSize);
		if (lSubSize > 0)
		{
			lSize += lSubSize;
		}
		else
		{
			lSize = -1;
			break;
		}
	}
	return (lSize);
}

float ObjectPositionalData::GetBiasedDifference(const PositionalData* pReference) const
{
	assert(GetType() == pReference->GetType());
	const ObjectPositionalData& lReference = (const ObjectPositionalData&)*pReference;
	assert(IsSameStructure(lReference));
	float lDiff = 0;
	lDiff += mPosition.GetBiasedDifference(&lReference.mPosition);
	BodyPositionArray::const_iterator x = mBodyPositionArray.begin();
	BodyPositionArray::const_iterator y = lReference.mBodyPositionArray.begin();
	for (; x != mBodyPositionArray.end(); ++x, ++y)
	{
		assert(y != lReference.mBodyPositionArray.end());
		lDiff += (*x)->GetScaledDifference(*y);
	}
	assert((x == mBodyPositionArray.end()) == (y == lReference.mBodyPositionArray.end()));
	return (lDiff);
}

PositionalData* ObjectPositionalData::GetAt(size_t pIndex) const
{
	if (pIndex >= mBodyPositionArray.size())
	{
		return (0);
	}
	return (mBodyPositionArray[pIndex]);
}

void ObjectPositionalData::SetAt(size_t pIndex, PositionalData* pData)
{
	if (mBodyPositionArray.size() >= pIndex)
	{
		if (mBodyPositionArray.size() == pIndex)
		{
			mBodyPositionArray.push_back(pData);
		}
		else
		{
			delete (mBodyPositionArray[pIndex]);
			mBodyPositionArray[pIndex] = pData;
		}
	}
	else
	{
		mLog.AError("Trying to set positional data out of range!");
		assert(false);
	}
}

void ObjectPositionalData::Trunkate(size_t pSize)
{
	for (size_t x = pSize; x < mBodyPositionArray.size(); ++x)
	{
		delete (mBodyPositionArray[x]);
	}
	mBodyPositionArray.resize(pSize);
}

PositionalData::Type ObjectPositionalData::GetType() const
{
	return (TYPE_OBJECT);
}

void ObjectPositionalData::CopyData(const PositionalData* pData)
{
	assert(pData->GetType() == GetType());
	const ObjectPositionalData& lCopy = *(ObjectPositionalData*)pData;
	if (IsSameStructure(lCopy))
	{
		mPosition = lCopy.mPosition;
		BodyPositionArray::iterator x = mBodyPositionArray.begin();
		BodyPositionArray::iterator y = ((ObjectPositionalData*)pData)->mBodyPositionArray.begin();
		for (; x != mBodyPositionArray.end(); ++x, ++y)
		{
			PositionalData* lPosition = *x;
			lPosition->CopyData(*y);
		}
	}
	else
	{
		Clear();
		mPosition = lCopy.mPosition;
		BodyPositionArray::const_iterator x = lCopy.mBodyPositionArray.begin();
		for (; x != lCopy.mBodyPositionArray.end(); ++x)
		{
			mBodyPositionArray.push_back((*x)->Clone());
		}
	}
}

PositionalData* ObjectPositionalData::Clone()
{
	ObjectPositionalData* lData = new ObjectPositionalData;
	BodyPositionArray::const_iterator x = mBodyPositionArray.begin();
	for (; x != mBodyPositionArray.end(); ++x)
	{
		lData->mBodyPositionArray.push_back((*x)->Clone());
	}
	return (lData);
}

bool ObjectPositionalData::IsSameStructure(const ObjectPositionalData& pCopy) const
{
	if (mBodyPositionArray.size() != pCopy.mBodyPositionArray.size())
	{
		return (false);
	}
	BodyPositionArray::const_iterator x = mBodyPositionArray.begin();
	BodyPositionArray::const_iterator y = pCopy.mBodyPositionArray.begin();
	for (; x != mBodyPositionArray.end(); ++x, ++y)
	{
		if ((*x)->GetType() != (*y)->GetType())
		{
			return (false);
		}
	}
	return (true);
}

ObjectPositionalData::ObjectPositionalData(const ObjectPositionalData&)
{
	assert(false);
}

void ObjectPositionalData::operator=(const ObjectPositionalData&)
{
	assert(false);
}

LOG_CLASS_DEFINE(NETWORK, ObjectPositionalData);



int RealData4::GetPackSize() const
{
	return (1+sizeof(mValue[0])*4);
}

int RealData4::Pack(Lepra::uint8* pData) const
{
	int lSize = 0;
	pData[lSize++] = (Lepra::uint8)TYPE_REAL_4;
	lSize += Cure::PackerReal::Pack(&pData[lSize], mValue[0]);
	lSize += Cure::PackerReal::Pack(&pData[lSize], mValue[1]);
	lSize += Cure::PackerReal::Pack(&pData[lSize], mValue[2]);
	lSize += Cure::PackerReal::Pack(&pData[lSize], mValue[3]);
	return (lSize);
}

int RealData4::Unpack(const Lepra::uint8* pData, int pSize)
{
	CHECK_SIZE(1+sizeof(mValue[0])*4);
	int lSize = 0;
	CHECK_TYPE(TYPE_REAL_4);
	lSize += Cure::PackerReal::Unpack(mValue[0], &pData[lSize], sizeof(float));
	lSize += Cure::PackerReal::Unpack(mValue[1], &pData[lSize], sizeof(float));
	lSize += Cure::PackerReal::Unpack(mValue[2], &pData[lSize], sizeof(float));
	lSize += Cure::PackerReal::Unpack(mValue[3], &pData[lSize], sizeof(float));
	return (lSize);
}

float RealData4::GetBiasedDifference(const PositionalData* pReference) const
{
	const RealData4& lReference = (const RealData4&)*pReference;
	float lWeightedDifferenceSum =
		::fabs(mValue[0]-lReference.mValue[0])*1000 +
		::fabs(mValue[1]-lReference.mValue[1])*1000 +
		::fabs(mValue[2]-lReference.mValue[2])*1000 +
		::fabs(mValue[3]-lReference.mValue[3])*1000;
	return (lWeightedDifferenceSum);
}

PositionalData::Type RealData4::GetType() const
{
	return (TYPE_REAL_4);
}

void RealData4::CopyData(const PositionalData* pData)
{
	assert(pData->GetType() == GetType());
	const RealData4& lCopy = *(RealData4*)pData;
	*this = lCopy;
}

PositionalData* RealData4::Clone()
{
	return (new RealData4(*this));
}



int RealData1Bool::GetPackSize() const
{
	return (1+sizeof(mValue)+1);
}

int RealData1Bool::Pack(Lepra::uint8* pData) const
{
	int lSize = 0;
	pData[lSize++] = (Lepra::uint8)TYPE_REAL_1_BOOL;
	lSize += Cure::PackerReal::Pack(&pData[lSize], mValue);
	pData[lSize++] = (Lepra::uint8)mBool;
	return (lSize);
}

int RealData1Bool::Unpack(const Lepra::uint8* pData, int pSize)
{
	CHECK_SIZE(1+sizeof(mValue)+1);
	int lSize = 0;
	CHECK_TYPE(TYPE_REAL_1_BOOL);
	lSize += Cure::PackerReal::Unpack(mValue, &pData[lSize], sizeof(float));
	mBool = (pData[lSize++] != 0);
	return (lSize);
}

float RealData1Bool::GetBiasedDifference(const PositionalData* pReference) const
{
	const RealData1Bool& lReference = (const RealData1Bool&)*pReference;
	float lWeightedDifferenceSum =
		::fabs(mValue-lReference.mValue)*1000 +
		::abs((int)mBool-(int)mBool)*1000;
	return (lWeightedDifferenceSum);
}

PositionalData::Type RealData1Bool::GetType() const
{
	return (TYPE_REAL_1_BOOL);
}

void RealData1Bool::CopyData(const PositionalData* pData)
{
	assert(pData->GetType() == GetType());
	const RealData1Bool& lCopy = *(RealData1Bool*)pData;
	*this = lCopy;
}

PositionalData* RealData1Bool::Clone()
{
	return (new RealData1Bool(*this));
}



int RealData1::GetPackSize() const
{
	return (1+sizeof(mValue));
}

int RealData1::Pack(Lepra::uint8* pData) const
{
	int lSize = 0;
	pData[lSize++] = (Lepra::uint8)TYPE_REAL_1;
	lSize += Cure::PackerReal::Pack(&pData[lSize], mValue);
	return (lSize);
}

int RealData1::Unpack(const Lepra::uint8* pData, int pSize)
{
	CHECK_SIZE(1+sizeof(mValue));
	int lSize = 0;
	CHECK_TYPE(TYPE_REAL_1);
	lSize += Cure::PackerReal::Unpack(mValue, &pData[lSize], sizeof(float));
	return (lSize);
}

float RealData1::GetBiasedDifference(const PositionalData* pReference) const
{
	const RealData1& lReference = (const RealData1&)*pReference;
	float lWeightedDifferenceSum =
		::fabs(mValue-lReference.mValue)*1000;
	return (lWeightedDifferenceSum);
}

PositionalData::Type RealData1::GetType() const
{
	return (TYPE_REAL_1);
}

void RealData1::CopyData(const PositionalData* pData)
{
	assert(pData->GetType() == GetType());
	const RealData1& lCopy = *(RealData1*)pData;
	*this = lCopy;
}

PositionalData* RealData1::Clone()
{
	return (new RealData1(*this));
}



}
