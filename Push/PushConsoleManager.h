
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../life/lifeclient/clientconsolemanager.h"
#include "push.h"



namespace Push {



class PushConsoleManager: public life::ClientConsoleManager {
	typedef life::ClientConsoleManager Parent;
public:
	PushConsoleManager(cure::ResourceManager* resource_manager, cure::GameManager* game_manager,
		UiCure::GameUiManager* ui_manager, cure::RuntimeVariableScope* variable_scope, const PixelRect& area);
	virtual ~PushConsoleManager();

protected:
	enum CommandClient {
		kCommandSetAvatar = Parent::kCommandCountLifeClient,
		kCommandSetAvatarEnginePower,
#if defined(LEPRA_DEBUG) && defined(LEPRA_WINDOWS)
		kCommandBuildData,
#endif // Debug & Windows
	};

	virtual unsigned GetCommandCount() const;
	virtual const CommandPair& GetCommand(unsigned index) const;
	virtual int OnCommand(const HashedString& command, const strutil::strvec& parameter_vector);

	static const CommandPair command_id_list_[];
	logclass();
};



}
