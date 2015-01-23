
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Life/LifeClient/ClientConsoleManager.h"
#include "PhysGfxObject.h"
#include "TrabantSim.h"



namespace TrabantSim
{



class TrabantSimConsoleManager: public Life::ClientConsoleManager
{
	typedef Life::ClientConsoleManager Parent;
public:
	TrabantSimConsoleManager(Cure::ResourceManager* pResourceManager, Cure::GameManager* pGameManager,
		UiCure::GameUiManager* pUiManager, Cure::RuntimeVariableScope* pVariableScope, const PixelRect& pArea);
	virtual ~TrabantSimConsoleManager();
	virtual bool Start();

	str GetActiveResponse() const;

protected:
	enum CommandClient
	{
		COMMAND_RESET = Parent::COMMAND_COUNT_LIFE_CLIENT,
		COMMAND_CREATE_OBJECT,
		COMMAND_DELETE_OBJECT,
		COMMAND_DELETE_ALL_OBJECTS,
		COMMAND_CLEAR_PHYS,
		COMMAND_PREP_PHYS_BOX,
		COMMAND_PREP_PHYS_SPHERE,
		COMMAND_PREP_PHYS_MESH,
		COMMAND_PREP_GFX_MESH,
		COMMAND_SET_VERTICES,
		COMMAND_SET_INDICES,
		COMMAND_WAIT_UNTIL_LOADED,
		COMMAND_EXPLODE,
		COMMAND_PLAY_SOUND,
		COMMAND_POP_COLLISIONS,
		COMMAND_GET_KEYS,
		COMMAND_GET_TOUCH_DRAGS,
		COMMAND_GET_ACCELEROMETER,
		COMMAND_CREATE_JOYSTICK,
		COMMAND_GET_JOYSTICK_DATA,
		COMMAND_GET_ASPECT_RATIO,
		COMMAND_CREATE_ENGINE,
		COMMAND_CREATE_JOINT,
		COMMAND_POSITION,
		COMMAND_ORIENTATION,
		COMMAND_VELOCITY,
		COMMAND_ANGULAR_VELOCITY,
		COMMAND_MASS,
		COMMAND_COLOR,
		COMMAND_ENGINE_FORCE,
		COMMAND_SET_TAG_FLOATS,
		COMMAND_SET_TAG_STRINGS,
		COMMAND_SET_TAG_PHYS,
		COMMAND_SET_TAG_ENGINE,
		COMMAND_SET_TAG_MESH,
		COMMAND_ADD_TAG,
	};

	virtual unsigned GetCommandCount() const;
	virtual const CommandPair& GetCommand(unsigned pIndex) const;
	virtual int OnCommand(const str& pCommand, const strutil::strvec& pParameterVector);

	std::vector<float> mVertices;
	std::vector<int> mIndices;
	MeshObject mGfxMesh;
	PhysObjectArray mPhysObjects;
	std::vector<float> mTagFloats;
	std::vector<str> mTagStrings;
	std::vector<int> mTagPhys;
	std::vector<int> mTagEngines;
	std::vector<int> mTagMeshes;
	str mActiveResponse;
	static const CommandPair mCommandIdList[];
	logclass();
};



}
