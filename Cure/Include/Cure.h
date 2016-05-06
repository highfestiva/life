
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../lepra/include/lepratypes.h"
#include "../../lepra/include/log.h"



namespace cure {



using namespace lepra;

class RuntimeVariableScope;

void Init();
void Shutdown();
void SetDefault(RuntimeVariableScope* settings);
RuntimeVariableScope* GetSettings();



// This is the ID of the game object. A GameObjectId identifies an entity,
// which may be composed of physics, network and context objects. The actual
// GameObjectId number of an object is, in most cases, created by the server.
// Exceptions are local objects that are local-only; such as a graphical
// effect or a local part of the world (on the client computer only).
typedef uint32 GameObjectId;



enum NetworkObjectType {
	kNetworkObjectLocalOnly		= 0,	// Only on the local computer.
	kNetworkObjectLocallyControlled	= 1,	// Present on local and remote computer, controlled locally.
	kNetworkObjectRemoteControlled	= 2,	// Present on local and remote computer, controlled remotely.
};



enum ResourceLoadState {
	kResourceUnloaded		= 1,
	kResourceLoadInProgress	= 2,
	kResourceLoadComplete		= 3,
	kResourceLoadError		= 4,
};

enum PhysicsOverride {
	kPhysicsOverrideNormal = 1,
	kPhysicsOverrideStatic,
	kPhysicsOverrideDynamic,
	kPhysicsOverrideBones,
};



#define kCureStandardFrameRate	30



}
