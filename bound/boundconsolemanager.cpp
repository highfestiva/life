
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "boundconsolemanager.h"
#include "../cure/include/contextmanager.h"
#include "../lepra/include/cyclicarray.h"
#include "../lepra/include/path.h"
#include "../lepra/include/systemmanager.h"
#include "boundmanager.h"
#include "rtvar.h"



namespace Bound {



// Must lie before BoundConsoleManager to compile.
const BoundConsoleManager::CommandPair BoundConsoleManager::command_id_list_[] =
{
	{"step-level", kCommandStepLevel},
};



BoundConsoleManager::BoundConsoleManager(cure::ResourceManager* resource_manager, cure::GameManager* game_manager,
	UiCure::GameUiManager* ui_manager, cure::RuntimeVariableScope* variable_scope, const PixelRect& area):
	Parent(resource_manager, game_manager, ui_manager, variable_scope, area) {
	InitCommands();
	SetSecurityLevel(1);
}

BoundConsoleManager::~BoundConsoleManager() {
}

bool BoundConsoleManager::Start() {
#ifndef LEPRA_TOUCH
	return Parent::Start();
#else // touch
	return true;	// touch device don't need an interactive console.
#endif // Computer / touch
}



unsigned BoundConsoleManager::GetCommandCount() const {
	return Parent::GetCommandCount() + LEPRA_ARRAY_COUNT(command_id_list_);
}

const BoundConsoleManager::CommandPair& BoundConsoleManager::GetCommand(unsigned index) const {
	if (index < Parent::GetCommandCount()) {
		return (Parent::GetCommand(index));
	}
	return (command_id_list_[index-Parent::GetCommandCount()]);
}

int BoundConsoleManager::OnCommand(const HashedString& command, const strutil::strvec& parameter_vector) {
	int result = Parent::OnCommand(command, parameter_vector);
	if (result < 0) {
		result = 0;

		CommandClient _command = (CommandClient)TranslateCommand(command);
		switch ((int)_command) {
			case kCommandStepLevel: {
				int step = 0;
				if (parameter_vector.size() == 1 && strutil::StringToInt(parameter_vector[0], step)) {
					GetGameManager()->GetTickLock()->Acquire();
					((BoundManager*)GetGameManager())->StepLevel(step);
					GetGameManager()->GetTickLock()->Release();
					return 0;
				}
				return 1;
			} break;
			default: {
				result = -1;
			} break;
		}
	}
	return (result);
}



loginstance(kConsole, BoundConsoleManager);



}
