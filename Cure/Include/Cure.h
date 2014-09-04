
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../Lepra/Include/LepraTypes.h"
#include "../../Lepra/Include/Log.h"



namespace Cure
{



using namespace Lepra;

class RuntimeVariableScope;

void Init();
void Shutdown();
void SetDefault(RuntimeVariableScope* pSettings);
RuntimeVariableScope* GetSettings();



// This is the ID of the game object. A GameObjectId identifies an entity,
// which may be composed of physics, network and context objects. The actual
// GameObjectId number of an object is, in most cases, created by the server.
// Exceptions are local objects that are local-only; such as a graphical
// effect or a local part of the world (on the client computer only).
typedef uint32 GameObjectId;



enum NetworkObjectType
{
	NETWORK_OBJECT_LOCAL_ONLY		= 0,	// Only on the local computer.
	NETWORK_OBJECT_LOCALLY_CONTROLLED	= 1,	// Present on local and remote computer, controlled locally.
	NETWORK_OBJECT_REMOTE_CONTROLLED	= 2,	// Present on local and remote computer, controlled remotely.
};



enum ResourceLoadState
{
	RESOURCE_UNLOADED		= 1,
	RESOURCE_LOAD_IN_PROGRESS	= 2,
	RESOURCE_LOAD_COMPLETE		= 3,
	RESOURCE_LOAD_ERROR		= 4,
};

enum PhysicsOverride
{
	PHYSICS_OVERRIDE_NORMAL = 1,
	PHYSICS_OVERRIDE_STATIC,
	PHYSICS_OVERRIDE_BONES,
};



#define CURE_STANDARD_FRAME_RATE	30



}
