
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/PhysicsManagerFactory.h"
#include "../Include/PhysicsManagerODE.h"



namespace TBC
{



PhysicsManager* PhysicsManagerFactory::Create(Engine pEngine)
{
	PhysicsManager* lEngine = 0;
	switch (pEngine)
	{
		case ENGINE_ODE:	lEngine = new PhysicsManagerODE();		break;
		default:		mLog.Error(_T("Physics type not supported."));	break;
	}
	return (lEngine);
}



LOG_CLASS_DEFINE(PHYSICS, PhysicsManagerFactory);



}
