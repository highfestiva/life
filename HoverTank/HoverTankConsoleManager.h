
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../life/lifeclient/clientconsolemanager.h"
#include "hovertank.h"



namespace HoverTank {



class HoverTankConsoleManager: public life::ClientConsoleManager {
	typedef life::ClientConsoleManager Parent;
public:
	HoverTankConsoleManager(cure::ResourceManager* resource_manager, cure::GameManager* game_manager,
		UiCure::GameUiManager* ui_manager, cure::RuntimeVariableScope* variable_scope, const PixelRect& area);
	virtual ~HoverTankConsoleManager();
	bool Start();

protected:
	enum CommandClient {
		kCommandSetAvatar = Parent::kCommandCountLifeClient,
	};

	virtual unsigned GetCommandCount() const;
	virtual const CommandPair& GetCommand(unsigned index) const;
	virtual int OnCommand(const HashedString& command, const strutil::strvec& parameter_vector);

	static const CommandPair command_id_list_[];
	logclass();
};



}
