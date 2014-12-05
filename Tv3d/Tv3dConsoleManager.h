
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Life/LifeClient/ClientConsoleManager.h"
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

protected:
	enum CommandClient
	{
		COMMAND_CREATE_OBJECT = Parent::COMMAND_COUNT_LIFE_CLIENT,
		COMMAND_DELETE_OBJECTS,
		COMMAND_CLEAR_PHYS,
		COMMAND_PREP_PHYS_BOX,
		COMMAND_PREP_PHYS_MESH,
		COMMAND_PREP_GFX_MESH,
		COMMAND_SET_VERTICES,
		COMMAND_SET_INDICES,
	};

	virtual unsigned GetCommandCount() const;
	virtual const CommandPair& GetCommand(unsigned pIndex) const;
	virtual int OnCommand(const str& pCommand, const strutil::strvec& pParameterVector);

	std::vector<float> mVertices;
	std::vector<int> mIndices;
	static const CommandPair mCommandIdList[];
	logclass();
};



}
