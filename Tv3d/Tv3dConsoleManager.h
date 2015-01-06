
// Author: Jonas Bystr�m
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Life/LifeClient/ClientConsoleManager.h"
#include "PhysGfxObject.h"
#include "Tv3d.h"



namespace Tv3d
{



class Tv3dConsoleManager: public Life::ClientConsoleManager
{
	typedef Life::ClientConsoleManager Parent;
public:
	Tv3dConsoleManager(Cure::ResourceManager* pResourceManager, Cure::GameManager* pGameManager,
		UiCure::GameUiManager* pUiManager, Cure::RuntimeVariableScope* pVariableScope, const PixelRect& pArea);
	virtual ~Tv3dConsoleManager();
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
		COMMAND_PREP_PHYS_MESH,
		COMMAND_PREP_GFX_MESH,
		COMMAND_SET_VERTICES,
		COMMAND_SET_INDICES,
		COMMAND_WAIT_UNTIL_LOADED,
		COMMAND_EXPLOAD,
		COMMAND_PLAY_SOUND,
		COMMAND_POP_COLLISIONS,
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
		COMMAND_WEIGHT,
		COMMAND_COLOR,
		COMMAND_ENGINE_FORCE,
	};

	virtual unsigned GetCommandCount() const;
	virtual const CommandPair& GetCommand(unsigned pIndex) const;
	virtual int OnCommand(const str& pCommand, const strutil::strvec& pParameterVector);

	std::vector<float> mVertices;
	std::vector<int> mIndices;
	MeshObject mGfxMesh;
	PhysObjectArray mPhysObjects;
	str mActiveResponse;
	static const CommandPair mCommandIdList[];
	logclass();
};



}
