
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../cure/include/contextmanager.h"
#include "../../cure/include/runtimevariable.h"
#include "../../cure/include/runtimevariablename.h"
#include "../../cure/include/timemanager.h"
#include "../../lepra/include/cyclicarray.h"
#include "../../lepra/include/lepraos.h"
#include "../../lepra/include/number.h"
#include "../../lepra/include/path.h"
#include "../../lepra/include/systemmanager.h"
#include "../../uicure/include/uigameuimanager.h"
#include "../../uicure/include/uiresourcemanager.h"
#include "../../uitbc/include/gui/uiconsoleloglistener.h"
#include "../../uitbc/include/gui/uiconsoleprompt.h"
#include "../../uitbc/include/gui/uidesktopwindow.h"
#include "../../uitbc/include/gui/uifilenamefield.h"
#include "../../uitbc/include/gui/uitextarea.h"
#include "../lifeapplication.h"
#include "gameclientslavemanager.h"
#include "gameclientmasterticker.h"
#include "clientconsolemanager.h"
#include "rtvar.h"
#include "uiconsole.h"



namespace life {



// Must lie before ClientConsoleManager to compile.
const ClientConsoleManager::CommandPair ClientConsoleManager::command_id_list_[] =
{
	{"quit", kCommandQuit},
	{"bye", kCommandBye},
	{"zombie", kCommandZombie},
	{"echo-msgbox", kCommandEchoMsgbox},
	{"start-login", kCommandStartLogin},
	{"wait-login", kCommandWaitLogin},
	{"logout", kCommandLogout},
	{"start-reset-ui", kCommandStartResetUi},
	{"wait-reset-ui", kCommandWaitResetUi},
	{"add-player", kCommandAddPlayer},
	{"set-mesh-visible", kCommandSetMeshVisible},
};



ClientConsoleManager::ClientConsoleManager(cure::ResourceManager* resource_manager, cure::GameManager* game_manager,
	UiCure::GameUiManager* ui_manager, cure::RuntimeVariableScope* variable_scope, const PixelRect& area):
	ConsoleManager(resource_manager, game_manager, variable_scope, new uitbc::ConsoleLogListener,
		new uitbc::ConsolePrompt),
	was_cursor_visible_(true) {
	ui_console_ = new UiConsole(this, ui_manager, area);
}

ClientConsoleManager::~ClientConsoleManager() {
	delete GetConsoleLogger();
	SetConsoleLogger(0);
	delete (ui_console_);
	ui_console_ = 0;
}



bool ClientConsoleManager::Start() {
	ui_console_->Open();
	return Parent::Start();
}

void ClientConsoleManager::Join() {
	ui_console_->Close();
	Parent::Join();
}



bool ClientConsoleManager::ToggleVisible() {
	const bool console_active = ui_console_->ToggleVisible();
	if (console_active) {
		was_cursor_visible_ = ui_console_->GetUiManager()->GetInputManager()->IsCursorVisible();
		ui_console_->GetUiManager()->GetInputManager()->SetCursorVisible(console_active);
	} else {
		ui_console_->GetUiManager()->GetInputManager()->SetCursorVisible(was_cursor_visible_);
	}
	return console_active;
}

UiConsole* ClientConsoleManager::GetUiConsole() const {
	return (ui_console_);
}



int ClientConsoleManager::FilterExecuteCommand(const str& command_line) {
	const str command_delimitors(" \t\v\r\n");
	const strutil::strvec command_list = strutil::BlockSplit(command_line, ";", true, true);
	const int allowed_count = 5;
	const str allowed_list[allowed_count] =
	{
		str("#" kRtvarPhysicsFps),
		str("#" kRtvarPhysicsRtr),
		str("#" kRtvarPhysicsHalt),
		str("#Ui.3D."),
		str("echo "),
	};
	int result = 0;
	for (size_t command_index = 0; result == 0 && command_index < command_list.size(); ++command_index) {
		result = -1;
		const str& temp_command = command_list[command_index];
		const str _command = strutil::StripLeft(temp_command, command_delimitors);
		for (int x = 0; x < allowed_count; ++x) {
			if (strutil::StartsWith(_command, allowed_list[x])) {
				result = ExecuteCommand(_command);
				break;
			}
		}
	}
	return result;
}



bool ClientConsoleManager::SaveApplicationConfigFile(File* file, const str& user_config) {
	bool ok = Parent::SaveApplicationConfigFile(file, user_config);
	if (ok && user_config.empty()) {
		file->WriteString("//push \"start-login server:port username password\"\n");
		ok = true;	// TODO: check if all writes went well.
	}
	return (ok);
}



unsigned ClientConsoleManager::GetCommandCount() const {
	return Parent::GetCommandCount() + LEPRA_ARRAY_COUNT(command_id_list_);
}

const ClientConsoleManager::CommandPair& ClientConsoleManager::GetCommand(unsigned index) const {
	if (index < Parent::GetCommandCount()) {
		return (Parent::GetCommand(index));
	}
	return (command_id_list_[index-Parent::GetCommandCount()]);
}

int ClientConsoleManager::OnCommand(const HashedString& command, const strutil::strvec& parameter_vector) {
	int result = Parent::OnCommand(command, parameter_vector);
	if (result < 0) {
		result = 0;

		CommandClient _command = (CommandClient)TranslateCommand(command);
		switch ((int)_command) {
			case kCommandSetDefaultConfig: {
				UiCure::SetDefault(GetVariableScope());
			} break;
			case kCommandQuit: {
				if (!parameter_vector.empty() && parameter_vector[0] == "!") {
					log_.Warning("Hard process termination due to user command!");
					SystemManager::ExitProcess(0);
				} else {
					log_.Info("Terminating due to user command.");
					SystemManager::AddQuitRequest(+1);
				}
			} break;
			case kCommandBye: {
				((GameClientSlaveManager*)GetGameManager())->SetIsQuitting();
			} break;
			case kCommandZombie: {
				Application::GetApplication()->SetZombieTick(Application::ZombieTick(this, &ClientConsoleManager::HeadlessTick));
				while (Application::GetApplication()->GetTicker()) {
					Thread::Sleep(0.5f);
				}
				for (size_t x = 0; x < parameter_vector.size(); ++x) {
					PushYieldCommand(parameter_vector[x]);
				}
				MemberThread<cure::ConsoleManager>* console_thread = console_thread_;
				console_thread_ = 0;
				console_thread->RequestSelfDestruct();
				console_thread->RequestStop();
			} break;
			case kCommandEchoMsgbox: {
				if (parameter_vector.size() == 2) {
					ui_console_->GetUiManager()->GetDisplayManager()->ShowMessageBox(parameter_vector[1], parameter_vector[0]);
				} else {
					log_.Warningf("usage: %s <title> <msg>", command.c_str());
					result = 1;
				}
			} break;
			case kCommandStartLogin: {
				((GameClientSlaveManager*)GetGameManager())->Logout();

				if (parameter_vector.size() == 3) {
					str username = parameter_vector[1];
					str readable_password = parameter_vector[2];
					//parameter_vector[2] = "        ";
					// Convert into login format.
					cure::MangledPassword password(readable_password);
					readable_password.clear();	// Clear out password traces in string.
					cure::LoginId login_token(username, password);
					((GameClientSlaveManager*)GetGameManager())->RequestLogin(parameter_vector[0], login_token);
				} else {
					log_.Warningf("usage: %s <server> <username> <password>", command.c_str());
					result = 1;
				}
			} break;
			case kCommandWaitLogin: {
				log_.Info("Waiting for login to finish...");
				while (((GameClientSlaveManager*)GetGameManager())->IsLoggingIn()) {
					Thread::Sleep(0.01);
				}
			} break;
			case kCommandLogout: {
				log_.Info("Logging off due to user command.");
				((GameClientSlaveManager*)GetGameManager())->Logout();
			} break;
			case kCommandStartResetUi: {
				log_.Info("Running UI restart...");
				if (((GameClientSlaveManager*)GetGameManager())->GetMaster()->StartResetUi()) {
					log_.Info("UI is restarting.");
				} else {
					log_.Error("Could not run UI restart!");
					result = 1;
				}
			} break;
			case kCommandWaitResetUi: {
				log_.Info("Waiting for UI to be restarted...");
				if (((GameClientSlaveManager*)GetGameManager())->GetMaster()->WaitResetUi()) {
					log_.Info("UI is up and running.");
				} else {
					log_.Error("UI restarted was not completed in time!");
					result = 1;
				}
			} break;
			case kCommandAddPlayer: {
				log_.Info("Adding another player.");
				if (((GameClientSlaveManager*)GetGameManager())->GetMaster()->CreateSlave()) {
					log_.Info("Another player added.");
				} else {
					log_.Error("Could not add another player!");
					result = 1;
				}
			} break;
			case kCommandSetMeshVisible: {
				bool visible = false;
				if (parameter_vector.size() == 2 && strutil::StringToBool(parameter_vector[1], visible)) {
					int affected_mesh_count = 0;
					typedef cure::ResourceManager::ResourceList ResourceList;
					ResourceList resource_list = resource_manager_->HookAllResourcesOfType("GeometryRef");
					for (ResourceList::iterator x = resource_list.begin(); x != resource_list.end(); ++x) {
						UiCure::GeometryReferenceResource* mesh_ref_resource = (UiCure::GeometryReferenceResource*)*x;
						if (mesh_ref_resource->GetName().find(parameter_vector[0]) != str::npos) {
							tbc::GeometryBase* mesh = mesh_ref_resource->GetRamData();
							if (mesh) {
								mesh->SetAlwaysVisible(visible);
								++affected_mesh_count;
							}
						}
					}
					resource_manager_->UnhookResources(resource_list);
					log_.Infof("%i meshes affected.", affected_mesh_count);
				} else {
					log_.Warningf("usage: %s <mesh> <bool>", command.c_str());
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



void ClientConsoleManager::HeadlessTick() {
	// Check What state our application zombie is in.
	if (ui_console_) {
		// Pre-destroy.
		delete ui_console_;
		ui_console_ = 0;
		GetGameManager()->SetConsoleManager(0);	// Snip. Now we're free floating.
		SetGameManager(0);
	} else if (!Application::GetApplication()->GetTicker()) {
		// Post-destroy, pre-init.
#ifdef LEPRA_WINDOWS
		::MessageBox(NULL, "Ready?", "Waiting...", MB_OK);
#endif // Windows
		while (ExecuteYieldCommand() >= 0)
			;
	} else {
		// Post-init.
		SetGameManager(((life::GameClientMasterTicker*)Application::GetApplication()->GetTicker())->GetSlave(0));
		life::Application::GetApplication()->SetZombieTick(life::Application::ZombieTick());
		delete this;	// Nice...
	}
}



loginstance(kConsole, ClientConsoleManager);



}
