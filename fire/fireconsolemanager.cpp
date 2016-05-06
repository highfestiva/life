
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "fireconsolemanager.h"
#include "../cure/include/contextmanager.h"
#include "../lepra/include/cyclicarray.h"
#include "../lepra/include/path.h"
#include "../lepra/include/systemmanager.h"
#include "firemanager.h"
#include "rtvar.h"



namespace Fire {



// Must lie before FireConsoleManager to compile.
const FireConsoleManager::CommandPair FireConsoleManager::command_id_list_[] =
{
	{"prev-level", kCommandPrevLevel},
	{"next-level", kCommandNextLevel},
	{"set-level-index", kCommandSetLevelIndex},
};



FireConsoleManager::FireConsoleManager(cure::ResourceManager* resource_manager, cure::GameManager* game_manager,
	UiCure::GameUiManager* ui_manager, cure::RuntimeVariableScope* variable_scope, const PixelRect& area):
	Parent(resource_manager, game_manager, ui_manager, variable_scope, area) {
	InitCommands();
	SetSecurityLevel(1);
}

FireConsoleManager::~FireConsoleManager() {
}

bool FireConsoleManager::Start() {
#ifndef LEPRA_TOUCH
	return Parent::Start();
#else // touch
	return true;	// touch device don't need an interactive console.
#endif // Computer / touch
}



unsigned FireConsoleManager::GetCommandCount() const {
	return Parent::GetCommandCount() + LEPRA_ARRAY_COUNT(command_id_list_);
}

const FireConsoleManager::CommandPair& FireConsoleManager::GetCommand(unsigned index) const {
	if (index < Parent::GetCommandCount()) {
		return (Parent::GetCommand(index));
	}
	return (command_id_list_[index-Parent::GetCommandCount()]);
}

int FireConsoleManager::OnCommand(const HashedString& command, const strutil::strvec& parameter_vector) {
	int result = Parent::OnCommand(command, parameter_vector);
	if (result < 0) {
		result = 0;

		CommandClient _command = (CommandClient)TranslateCommand(command);
		switch ((int)_command) {
			case kCommandPrevLevel: {
				GetGameManager()->GetTickLock()->Acquire();
				((FireManager*)GetGameManager())->StepLevel(-1);
				GetGameManager()->GetTickLock()->Release();
				return 0;
			} break;
			case kCommandNextLevel: {
				GetGameManager()->GetTickLock()->Acquire();
				((FireManager*)GetGameManager())->StepLevel(+1);
				GetGameManager()->GetTickLock()->Release();
				return 0;
			} break;
			case kCommandSetLevelIndex: {
				int target_level_index = -1;
				if (parameter_vector.size() == 1 && strutil::StringToInt(parameter_vector[0], target_level_index)) {
					GetGameManager()->GetTickLock()->Acquire();
					const int level_delta = target_level_index - ((FireManager*)GetGameManager())->GetCurrentLevelNumber();
					((FireManager*)GetGameManager())->StepLevel(level_delta);
					GetGameManager()->GetTickLock()->Release();
					return 0;
				} else {
					log_.Warningf("usage: %s <index>", command.c_str());
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



loginstance(kConsole, FireConsoleManager);



}
