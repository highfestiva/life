
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../life/lifeclient/clientconsolemanager.h"
#include "downwash.h"



namespace Downwash {



class DownwashConsoleManager: public life::ClientConsoleManager {
	typedef life::ClientConsoleManager Parent;
public:
	DownwashConsoleManager(cure::ResourceManager* resource_manager, cure::GameManager* game_manager,
		UiCure::GameUiManager* ui_manager, cure::RuntimeVariableScope* variable_scope, const PixelRect& area);
	virtual ~DownwashConsoleManager();
	virtual bool Start();

protected:
	enum CommandClient {
		kCommandSetAvatar = Parent::kCommandCountLifeClient,
		kCommandPrevLevel,
		kCommandNextLevel,
		kCommandSetLevelIndex,
		kCommandDie,
	};

	virtual unsigned GetCommandCount() const;
	virtual const CommandPair& GetCommand(unsigned index) const;
	virtual int OnCommand(const HashedString& command, const strutil::strvec& parameter_vector);

	static const CommandPair command_id_list_[];
	logclass();
};



}
