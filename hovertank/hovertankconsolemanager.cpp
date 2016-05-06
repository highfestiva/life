
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "hovertankconsolemanager.h"
#include "../cure/include/contextmanager.h"
#include "../lepra/include/cyclicarray.h"
#include "../lepra/include/path.h"
#include "../lepra/include/systemmanager.h"
#include "hovertankmanager.h"
#include "rtvar.h"



namespace HoverTank {



// Must lie before HoverTankConsoleManager to compile.
const HoverTankConsoleManager::CommandPair HoverTankConsoleManager::command_id_list_[] =
{
	{"set-avatar", kCommandSetAvatar},
};



HoverTankConsoleManager::HoverTankConsoleManager(cure::ResourceManager* resource_manager, cure::GameManager* game_manager,
	UiCure::GameUiManager* ui_manager, cure::RuntimeVariableScope* variable_scope, const PixelRect& area):
	Parent(resource_manager, game_manager, ui_manager, variable_scope, area) {
	InitCommands();
}

HoverTankConsoleManager::~HoverTankConsoleManager() {
}



bool HoverTankConsoleManager::Start() {
#ifndef LEPRA_TOUCH
	return Parent::Start();
#else // touch
	return true;	// touch device don't need an interactive console.
#endif // Computer / touch
}



unsigned HoverTankConsoleManager::GetCommandCount() const {
	return Parent::GetCommandCount() + LEPRA_ARRAY_COUNT(command_id_list_);
}

const HoverTankConsoleManager::CommandPair& HoverTankConsoleManager::GetCommand(unsigned index) const {
	if (index < Parent::GetCommandCount()) {
		return (Parent::GetCommand(index));
	}
	return (command_id_list_[index-Parent::GetCommandCount()]);
}

int HoverTankConsoleManager::OnCommand(const HashedString& command, const strutil::strvec& parameter_vector) {
	int result = Parent::OnCommand(command, parameter_vector);
	if (result < 0) {
		result = 0;

		CommandClient _command = (CommandClient)TranslateCommand(command);
		switch ((int)_command) {
			case kCommandSetAvatar: {
				if (parameter_vector.size() == 1) {
					((HoverTankManager*)GetGameManager())->SelectAvatar(parameter_vector[0]);
				} else {
					log_.Warningf("usage: %s <avatar>", command.c_str());
					result = 1;
				}
			} break;
			default: {
				result = -1;
			} break;
		}
	}
	return (result);
}



loginstance(kConsole, HoverTankConsoleManager);



}
