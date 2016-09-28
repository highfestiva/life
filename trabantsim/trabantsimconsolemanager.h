
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../life/lifeclient/clientconsolemanager.h"
#include "physgfxobject.h"
#include "trabantsim.h"



namespace TrabantSim {



class TrabantSimConsoleManager: public life::ClientConsoleManager {
	typedef life::ClientConsoleManager Parent;
public:
	TrabantSimConsoleManager(cure::ResourceManager* resource_manager, cure::GameManager* game_manager,
		UiCure::GameUiManager* ui_manager, cure::RuntimeVariableScope* variable_scope, const PixelRect& area);
	virtual ~TrabantSimConsoleManager();
	virtual bool Start();

	const str GetActiveResponse() const;

protected:
	enum CommandClient {
		kCommandReset = Parent::kCommandCountLifeClient,
		kCommandGetPlatformName,
		kCommandCreateObject,
		kCommandCreateClones,
		kCommandDeleteObject,
		kCommandDeleteAllObjects,
		kCommandPickObjects,
		kCommandClearPhys,
		kCommandPrepPhysBox,
		kCommandPrepPhysSphere,
		kCommandPrepPhysCapsule,
		kCommandPrepPhysMesh,
		kCommandPrepGfxMesh,
		kCommandSetVertices,
		kCommandAddVertices,
		kCommandSetIndices,
		kCommandAddIndices,
		kCommandAreLoaded,
		kCommandWaitUntilLoaded,
		kCommandExplode,
		kCommandPlaySound,
		kCommandPopCollisions,
		kCommandGetKeys,
		kCommandGetTouchDrags,
		kCommandGetAccelerometer,
		kCommandGetMousemove,
		kCommandCreateJoystick,
		kCommandGetJoystickData,
		kCommandGetAspectRatio,
		kCommandCreateEngine,
		kCommandDeleteEngine,
		kCommandCreateJoint,
		kCommandPosition,
		kCommandPositionObjects,
		kCommandOrientation,
		kCommandVelocity,
		kCommandAngularVelocity,
		kCommandForce,
		kCommandTorque,
		kCommandMass,
		kCommandScale,
		kCommandColor,
		kCommandShadow,
		kCommandEngineForce,
		kCommandSetTagFloats,
		kCommandSetTagStrings,
		kCommandSetTagPhys,
		kCommandSetTagEngine,
		kCommandSetTagMesh,
		kCommandAddTag,
	};

	virtual unsigned GetCommandCount() const;
	virtual const CommandPair& GetCommand(unsigned index) const;
	virtual int OnCommand(const HashedString& command, const strutil::strvec& parameter_vector);

	std::vector<float> vertices_;
	std::vector<int> indices_;
	MeshObject gfx_mesh_;
	PhysObjectArray phys_objects_;
	std::vector<float> tag_floats_;
	std::vector<str> tag_strings_;
	std::vector<int> tag_phys_;
	std::vector<int> tag_engines_;
	std::vector<int> tag_meshes_;
	static const CommandPair command_id_list_[];
	logclass();
};



}
