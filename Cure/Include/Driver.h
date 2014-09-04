
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../Lepra/Include/HiResTimer.h"
#include "../../UiTbc/Include/UiChunkyClass.h"
#include "CppContextObject.h"
#include "Cure.h"



namespace Cure
{



class Driver: public CppContextObject
{
	typedef CppContextObject Parent;
public:
	Driver(ContextManager* pManager);
	virtual ~Driver();

protected:
	virtual void SetTagIndex(int pIndex);
	virtual void OnTick();

private:
	str mType;
	UiTbc::ChunkyClass::Tag mTag;
	HiResTimer mTime;

	logclass();
};



}
