
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "impuzzableconsolemanager.h"
#include "../cure/include/contextmanager.h"
#include "../lepra/include/cyclicarray.h"
#include "../lepra/include/path.h"
#include "../lepra/include/systemmanager.h"
#include "impuzzablemanager.h"
#include "rtvar.h"



namespace Impuzzable {



// Must lie before ImpuzzableConsoleManager to compile.
const ImpuzzableConsoleManager::CommandPair ImpuzzableConsoleManager::command_id_list_[] =
{
	{"step-level", kCommandStepLevel},
};



ImpuzzableConsoleManager::ImpuzzableConsoleManager(cure::ResourceManager* resource_manager, cure::GameManager* game_manager,
	UiCure::GameUiManager* ui_manager, cure::RuntimeVariableScope* variable_scope, const PixelRect& area):
	Parent(resource_manager, game_manager, ui_manager, variable_scope, area) {
	InitCommands();
	SetSecurityLevel(1);
}

ImpuzzableConsoleManager::~ImpuzzableConsoleManager() {
}

bool ImpuzzableConsoleManager::Start() {
#ifndef LEPRA_TOUCH
	return Parent::Start();
#else // touch
	return true;	// touch device don't need an interactive console.
#endif // Computer / touch
}



unsigned ImpuzzableConsoleManager::GetCommandCount() const {
	return Parent::GetCommandCount() + LEPRA_ARRAY_COUNT(command_id_list_);
}

const ImpuzzableConsoleManager::CommandPair& ImpuzzableConsoleManager::GetCommand(unsigned index) const {
	if (index < Parent::GetCommandCount()) {
		return (Parent::GetCommand(index));
	}
	return (command_id_list_[index-Parent::GetCommandCount()]);
}

int ImpuzzableConsoleManager::OnCommand(const HashedString& command, const strutil::strvec& parameter_vector) {
	int result = Parent::OnCommand(command, parameter_vector);
	if (result < 0) {
		result = 0;

		CommandClient _command = (CommandClient)TranslateCommand(command);
		switch ((int)_command) {
			case kCommandStepLevel: {
				int step = 0;
				if (parameter_vector.size() == 1 && strutil::StringToInt(parameter_vector[0], step)) {
					GetGameManager()->GetTickLock()->Acquire();
					((ImpuzzableManager*)GetGameManager())->StepLevel(step);
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



loginstance(kConsole, ImpuzzableConsoleManager);



}
