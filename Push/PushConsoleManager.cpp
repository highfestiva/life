
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "pushconsolemanager.h"
#include "../cure/include/contextmanager.h"
#include "../cure/include/resourcemanager.h"
#include "../lepra/include/cyclicarray.h"
#include "../lepra/include/lepraos.h"
#include "../lepra/include/path.h"
#include "../lepra/include/systemmanager.h"
#include "pushmanager.h"
#include "rtvar.h"



namespace Push {



// Must lie before PushConsoleManager to compile.
const PushConsoleManager::CommandPair PushConsoleManager::command_id_list_[] =
{
	{"set-avatar", kCommandSetAvatar},
	{"set-avatar-engine-power", kCommandSetAvatarEnginePower},
#if defined(LEPRA_DEBUG) && defined(LEPRA_WINDOWS)
	{"builddata", kCommandBuildData},
#endif // Debug & Windows
};



PushConsoleManager::PushConsoleManager(cure::ResourceManager* resource_manager, cure::GameManager* game_manager,
	UiCure::GameUiManager* ui_manager, cure::RuntimeVariableScope* variable_scope, const PixelRect& area):
	Parent(resource_manager, game_manager, ui_manager, variable_scope, area) {
	InitCommands();
}

PushConsoleManager::~PushConsoleManager() {
}



unsigned PushConsoleManager::GetCommandCount() const {
	return Parent::GetCommandCount() + LEPRA_ARRAY_COUNT(command_id_list_);
}

const PushConsoleManager::CommandPair& PushConsoleManager::GetCommand(unsigned index) const {
	if (index < Parent::GetCommandCount()) {
		return (Parent::GetCommand(index));
	}
	return (command_id_list_[index-Parent::GetCommandCount()]);
}

int PushConsoleManager::OnCommand(const HashedString& command, const strutil::strvec& parameter_vector) {
	int result = Parent::OnCommand(command, parameter_vector);
	if (result < 0) {
		result = 0;

		CommandClient _command = (CommandClient)TranslateCommand(command);
		switch ((int)_command) {
			case kCommandSetAvatar: {
				if (parameter_vector.size() == 1) {
					((PushManager*)GetGameManager())->SelectAvatar(parameter_vector[0]);
				} else {
					log_.Warningf("usage: %s <avatar>", command.c_str());
					result = 1;
				}
			} break;
			case kCommandSetAvatarEnginePower: {
				if (parameter_vector.size() == 2) {
					log_debug("Setting avatar engine power.");
					int aspect = 0;
					strutil::StringToInt(parameter_vector[0], aspect);
					double power;
					strutil::StringToDouble(parameter_vector[1], power);
					if (!((PushManager*)GetGameManager())->SetAvatarEnginePower(aspect, (float)power)) {
						log_.Error("Could not set avatar engine power!");
						result = 1;
					}
				} else {
					log_.Warningf("usage: %s <aspect> <power>", command.c_str());
					result = 1;
				}
			} break;
#if defined(LEPRA_DEBUG) && defined(LEPRA_WINDOWS)
			case kCommandBuildData: {
				const cure::GameObjectId avatar_id = ((PushManager*)GetGameManager())->GetAvatarInstanceId();
				if (!avatar_id) {
					break;
				}
				const str avatar_type = GetGameManager()->GetContext()->GetObject(avatar_id)->GetClassId();
				((PushManager*)GetGameManager())->Logout();
				Thread::Sleep(0.5);
				GetResourceManager()->ForceFreeCache();
				const str current_dir = SystemManager::GetCurrentDirectory();
				::SetCurrentDirectoryA(astrutil::Encode(Path::GetParentDirectory(current_dir)).c_str());
				::system("c:/Program/Python31/python.exe -B Tools/build/rgo.py builddata");
				::SetCurrentDirectoryA(astrutil::Encode(current_dir).c_str());
				const str user_name = v_slowget(GetVariableScope(), kRtvarLoginUsername, EmptyString);
				const str server = v_slowget(GetVariableScope(), kRtvarNetworkServeraddress, EmptyString);
				wstr pw(L"CarPassword");
				const cure::LoginId login_id(wstrutil::Encode(user_name), cure::MangledPassword(pw));
				((PushManager*)GetGameManager())->RequestLogin(server, login_id);
				((PushManager*)GetGameManager())->ToggleConsole();
				ExecuteCommand("wait-login");
				((PushManager*)GetGameManager())->SelectAvatar(avatar_type);
			} break;
#endif // Debug & Windows
			default: {
				result = -1;
			} break;
		}
	}
	return (result);
}



loginstance(kConsole, PushConsoleManager);



}
