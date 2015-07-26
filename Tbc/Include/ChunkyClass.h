
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include <set>
#include "Tbc.h"



namespace Tbc
{



class ChunkyPhysics;



class ChunkyClass
{
public:
	struct Tag
	{
		str mTagName;
		std::vector<float> mFloatValueList;
		std::vector<str> mStringValueList;
		std::vector<int> mBodyIndexList;
		std::vector<int> mEngineIndexList;
		std::vector<int> mMeshIndexList;
	};

	ChunkyClass();
	virtual ~ChunkyClass();

	const str& GetPhysicsBaseName() const;
	str& GetPhysicsBaseName();

	bool UnpackTag(uint8* pBuffer, unsigned pSize);
	void AddTag(const Tag& pTag);
	void RemoveTag(size_t pTagIndex);
	size_t GetTagCount() const;
	const Tag& GetTag(size_t pTagIndex) const;
	const Tag* GetTag(const str& pTagName) const;

	void AddPhysRoot(int pPhysIndex);
	bool IsPhysRoot(int pPhysIndex) const;

protected:
private:
	str mPhysicsBaseName;
	std::vector<Tag> mTagArray;
	std::set<int> mPhysRootSet;

	logclass();
};



}
