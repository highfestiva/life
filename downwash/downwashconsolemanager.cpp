
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "downwashconsolemanager.h"
#include "../cure/include/contextmanager.h"
#include "../lepra/include/cyclicarray.h"
#include "../lepra/include/path.h"
#include "../lepra/include/systemmanager.h"
#include "downwashmanager.h"
#include "rtvar.h"



namespace Downwash {



// Must lie before DownwashConsoleManager to compile.
const DownwashConsoleManager::CommandPair DownwashConsoleManager::command_id_list_[] =
{
	{"set-avatar", kCommandSetAvatar},
	{"prev-level", kCommandPrevLevel},
	{"next-level", kCommandNextLevel},
	{"set-level-index", kCommandSetLevelIndex},
	{"die", kCommandDie},
};



DownwashConsoleManager::DownwashConsoleManager(cure::ResourceManager* resource_manager, cure::GameManager* game_manager,
	UiCure::GameUiManager* ui_manager, cure::RuntimeVariableScope* variable_scope, const PixelRect& area):
	Parent(resource_manager, game_manager, ui_manager, variable_scope, area) {
	InitCommands();
	SetSecurityLevel(1);
}

DownwashConsoleManager::~DownwashConsoleManager() {
}

bool DownwashConsoleManager::Start() {
#ifndef LEPRA_TOUCH
	return Parent::Start();
#else // touch
	return true;	// touch device don't need an interactive console.
#endif // Computer / touch
}



unsigned DownwashConsoleManager::GetCommandCount() const {
	return Parent::GetCommandCount() + LEPRA_ARRAY_COUNT(command_id_list_);
}

const DownwashConsoleManager::CommandPair& DownwashConsoleManager::GetCommand(unsigned index) const {
	if (index < Parent::GetCommandCount()) {
		return (Parent::GetCommand(index));
	}
	return (command_id_list_[index-Parent::GetCommandCount()]);
}

int DownwashConsoleManager::OnCommand(const HashedString& command, const strutil::strvec& parameter_vector) {
	int result = Parent::OnCommand(command, parameter_vector);
	if (result < 0) {
		result = 0;

		CommandClient _command = (CommandClient)TranslateCommand(command);
		switch ((int)_command) {
			case kCommandSetAvatar: {
				if (parameter_vector.size() == 1) {
					//((DownwashManager*)GetGameManager())->SelectAvatar(parameter_vector[0]);
				} else {
					log_.Warningf("usage: %s <avatar>", command.c_str());
					result = 1;
				}
			} break;
			case kCommandPrevLevel: {
				GetGameManager()->GetTickLock()->Acquire();
				((DownwashManager*)GetGameManager())->StepLevel(-1);
				GetGameManager()->GetTickLock()->Release();
				ExecuteCommand("die");
				return 0;
			} break;
			case kCommandNextLevel: {
				GetGameManager()->GetTickLock()->Acquire();
				((DownwashManager*)GetGameManager())->StepLevel(+1);
				GetGameManager()->GetTickLock()->Release();
				ExecuteCommand("die");
				return 0;
			} break;
			case kCommandSetLevelIndex: {
				int target_level_index = -1;
				if (parameter_vector.size() == 1 && strutil::StringToInt(parameter_vector[0], target_level_index)) {
					GetGameManager()->GetTickLock()->Acquire();
					const int level_delta = target_level_index - ((DownwashManager*)GetGameManager())->GetCurrentLevelNumber();
					((DownwashManager*)GetGameManager())->StepLevel(level_delta);
					GetGameManager()->GetTickLock()->Release();
					ExecuteCommand("die");
					return 0;
				} else {
					log_.Warningf("usage: %s <index>", command.c_str());
					result = 1;
				}
			} break;
			case kCommandDie: {
				GetGameManager()->GetTickLock()->Acquire();
				const cure::GameObjectId avatar_id = ((DownwashManager*)GetGameManager())->GetAvatarInstanceId();
				GetGameManager()->GetContext()->PostKillObject(avatar_id);
				GetGameManager()->GetTickLock()->Release();
			} break;
			default: {
				result = -1;
			} break;
		}
	}
	return (result);
}



loginstance(kConsole, DownwashConsoleManager);



}
