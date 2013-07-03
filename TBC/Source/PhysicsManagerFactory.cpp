
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../Include/PhysicsManagerFactory.h"
#include "../Include/PhysicsManagerODE.h"



namespace TBC
{



PhysicsManager* PhysicsManagerFactory::Create(Engine pEngine, float pRadius, int pLevels, float pSensitivity)
{
	PhysicsManager* lEngine = 0;
	switch (pEngine)
	{
		case ENGINE_ODE:	lEngine = new PhysicsManagerODE(pRadius, pLevels, pSensitivity);	break;
		default:		mLog.Error(_T("Physics type not supported."));				break;
	}
	return (lEngine);
}



LOG_CLASS_DEFINE(PHYSICS, PhysicsManagerFactory);



}
