
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../lepra/include/hirestimer.h"
#include "../../uitbc/include/uichunkyclass.h"
#include "cppcontextobject.h"
#include "cure.h"



namespace cure {



class Driver: public CppContextObject {
	typedef CppContextObject Parent;
public:
	Driver(ContextManager* manager);
	virtual ~Driver();

protected:
	virtual void SetTagIndex(int index);
	virtual void OnTick();

private:
	str type_;
	uitbc::ChunkyClass::Tag tag_;
	HiResTimer time_;

	logclass();
};



}
