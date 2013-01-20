
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "TBC.h"



namespace TBC
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

protected:
private:
	str mPhysicsBaseName;
	std::vector<Tag> mTagArray;

	LOG_CLASS_DECLARE();
};



}
