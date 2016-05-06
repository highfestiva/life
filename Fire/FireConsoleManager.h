
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../life/lifeclient/clientconsolemanager.h"
#include "fire.h"



namespace Fire {



class FireConsoleManager: public life::ClientConsoleManager {
	typedef life::ClientConsoleManager Parent;
public:
	FireConsoleManager(cure::ResourceManager* resource_manager, cure::GameManager* game_manager,
		UiCure::GameUiManager* ui_manager, cure::RuntimeVariableScope* variable_scope, const PixelRect& area);
	virtual ~FireConsoleManager();
	virtual bool Start();

protected:
	enum CommandClient {
		kCommandPrevLevel = Parent::kCommandCountLifeClient,
		kCommandNextLevel,
		kCommandSetLevelIndex,
	};

	virtual unsigned GetCommandCount() const;
	virtual const CommandPair& GetCommand(unsigned index) const;
	virtual int OnCommand(const HashedString& command, const strutil::strvec& parameter_vector);

	static const CommandPair command_id_list_[];
	logclass();
};



}
