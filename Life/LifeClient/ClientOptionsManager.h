
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "OptionsManager.h"
#include "ClientOptions.h"



namespace Life
{
namespace Options
{



struct ClientOptionsManager: OptionsManager
{
	ClientOptionsManager(Cure::RuntimeVariableScope* pVariableScope, int pPriority);

	const Vehicle& GetControl() const;

private:
	virtual bool SetDefault(int pPriority);
	virtual void DoRefreshConfiguration();

	Vehicle mControl;

	void operator=(const ClientOptionsManager&);

	LOG_CLASS_DECLARE();
};



}
}
