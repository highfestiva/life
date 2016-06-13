
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "consolemanager.h"
#include "../cure/include/gamemanager.h"
#include "../cure/include/resourcemanager.h"
#include "../cure/include/runtimevariable.h"
#include "../cure/include/timemanager.h"
#include "../lepra/include/cyclicarray.h"
#include "../lepra/include/lepraos.h"
#include "../lepra/include/loglistener.h"
#include "../lepra/include/number.h"
#include "../lepra/include/resourcetracker.h"
#include "../lepra/include/systemmanager.h"
#include "../uitbc/include/gui/uiconsoleprompt.h"



#define kUserSectionMark	"//!MARK!//"



namespace {
bool is_invalid_char(char c) {
	return !isalnum(c) && c != '-' && c != '#' && c != '.';
}
}



namespace life {



// Must lie before ConsoleManager to compile.
const ConsoleManager::CommandPair ConsoleManager::command_id_list_[] =
{
	// IO.
	{"alias", kCommandAlias},
	{"bind-key", kCommandBindKey},
	{"echo", kCommandEcho},
	{"execute-file", kCommandExecuteFile},
	{"execute-variable", kCommandExecuteVariable},
	{"fork", kCommandFork},
	{"nop", kCommandNop},
	{"list-active-resources", kCommandListActiveResources},
	{"push", kCommandPush},
	{"repeat", kCommandRepeat},
	{"save-system-config-file", kCommandSaveSystemConfigFile},
	{"save-application-config-file", kCommandSaveApplicationConfigFile},
	{"set-default-config", kCommandSetDefaultConfig},
	{"set-stdout-log-level", kCommandSetStdoutLogLevel},
	{"set-subsystem-log-level", kCommandSetSubsystemLogLevel},
	{"sleep", kCommandSleep},
	{"wait-loaded", kCommandWaitLoaded},

	// Info/debug stuff.
	{"clear-performance-info", kCommandClearPerformanceInfo},
	{"debug-break", kCommandDebugBreak},
	{"dump-performance-info", kCommandDumpPerformanceInfo},
	{"help", kCommandHelp},
	{"shell-execute", kCommandShellExecute},
	{"system-info", kCommandShowSystemInfo},
	{"game-info", kCommandShowGameInfo},
	{"game-hang", kCommandHangGame},
	{"game-unhang", kCommandUnhangGame},
};



ConsoleManager::ConsoleManager(cure::ResourceManager* resource_manager, cure::GameManager* game_manager,
	cure::RuntimeVariableScope* variable_scope, InteractiveConsoleLogListener* console_logger,
	ConsolePrompt* console_prompt):
	cure::ConsoleManager(variable_scope, console_logger, console_prompt),
	security_level_(0),
	game_manager_(game_manager),
	resource_manager_(resource_manager),
	logger_(0) {
}

ConsoleManager::~ConsoleManager() {
}



void ConsoleManager::InitCommands() {
	Parent::InitCommands();
	GetConsoleCommandManager()->SetComment("//");
	GetConsoleCommandManager()->AddCompleter(new cure::RuntimeVariableCompleter(GetVariableScope(), "#"));
	GetConsoleCommandManager()->AddCompleter(new cure::RuntimeVariableCompleter(cure::GetSettings(), "#/"));
	GetConsoleCommandManager()->AddCompleter(new cure::RuntimeVariableCompleter(GetVariableScope(), "#."));

	ExecuteCommand("alias trace-log-level \"set-stdout-log-level 0; set-subsystem-log-level 0\"");
	ExecuteCommand("alias debug-log-level \"set-stdout-log-level 1; set-subsystem-log-level 1\"");
	ExecuteCommand("alias performance-log-level \"set-stdout-log-level 2; set-subsystem-log-level 2\"");
	ExecuteCommand("alias info-log-level \"set-stdout-log-level 3; set-subsystem-log-level 3\"");
	ExecuteCommand("alias headline-log-level \"set-stdout-log-level 4; set-subsystem-log-level 4\"");
	ExecuteCommand("alias warning-log-level \"set-stdout-log-level 5; set-subsystem-log-level 5\"");
	ExecuteCommand("alias error-log-level \"set-stdout-log-level 6; set-subsystem-log-level 6\"");
	ExecuteCommand("alias debug-focus \"set-subsystem-log-level 4; set-stdout-log-level 0; set-subsystem-log-level\"");
}



void ConsoleManager::SetSecurityLevel(int level) {
	security_level_ = level;
}

int ConsoleManager::GetSecurityLevel() const {
	return security_level_;
}



cure::GameManager* ConsoleManager::GetGameManager() const {
	return (game_manager_);
}

void ConsoleManager::SetGameManager(cure::GameManager* game_manager) {
	game_manager_ = game_manager;
}



void ConsoleManager::OnKey(const str& key_name) {
	KeyMap::iterator x = key_map_.find(key_name);
	if (x != key_map_.end()) {
		PushYieldCommand(x->second);
		// Make console thread run it.
		((uitbc::ConsolePrompt*)console_prompt_)->OnChar(' ');
		((uitbc::ConsolePrompt*)console_prompt_)->OnChar('\b');
	}
}



unsigned ConsoleManager::GetCommandCount() const {
	return LEPRA_ARRAY_COUNT(command_id_list_);
}

const ConsoleManager::CommandPair& ConsoleManager::GetCommand(unsigned index) const {
	return (command_id_list_[index]);
}

int ConsoleManager::TranslateCommand(const HashedString& command) const {
	int translation = Parent::TranslateCommand(command);
	if (translation == -1) {
		if (alias_map_.find(command) != alias_map_.end()) {
			translation = kCommandAliasValue;
		}
	}
	return (translation);
}

int ConsoleManager::OnCommand(const HashedString& command, const strutil::strvec& parameter_vector) {
	int result = 0;
	CommandCommon _command = (CommandCommon)TranslateCommand(command);
	switch (_command) {
		case kCommandAlias: {
			//bool usage = false;
			if (parameter_vector.size() >= 2) {
				GetConsoleCommandManager()->RemoveCommand(parameter_vector[0]);
				strutil::strvec args(parameter_vector);
				args.erase(args.begin());
				if (args.size() > 1) {
					for (size_t x = 0; x < args.size(); ++x) {
						if (args[x].find(" ") != str::npos) {
							args[x] = "\"" + args[x] + "\"";
						}
					}
				}
				alias_map_[parameter_vector[0]] = strutil::Join(args, " ");;
				GetConsoleCommandManager()->AddCommand(parameter_vector[0]);
			} else if (parameter_vector.size() == 1) {
				alias_map_.erase(parameter_vector[0]);
				GetConsoleCommandManager()->RemoveCommand(parameter_vector[0]);
			} else {
				log_.Info("List of aliases:");
				for (AliasMap::iterator x = alias_map_.begin(); x != alias_map_.end(); ++x) {
					log_.Infof("  %s -> %s", x->first.c_str(), x->second.c_str());
				}
			}
			/*if (usage) {
				log_.Warningf("usage: %s <alias_name> [<command>]", command.c_str());
				log_.Warning("Adds a new alias, or removes one if <command is left out.");
				result = 1;
			}*/
		} break;
		case kCommandAliasValue: {
			AliasMap::iterator x = alias_map_.find(command);
			if (x != alias_map_.end()) {
				result = ExecuteCommand(x->second + " " + strutil::Join(parameter_vector, " "));
			} else {
				log_.Warningf("Alias %s is removed.", command.c_str());
				result = 1;
			}
		} break;
		case kCommandBindKey: {
			if (parameter_vector.size() == 2) {
				key_map_[parameter_vector[0]] = parameter_vector[1];
			} else if (parameter_vector.size() == 1) {
				key_map_.erase(parameter_vector[0]);
			} else {
				log_.Info("List of keys:");
				for (KeyMap::iterator x = key_map_.begin(); x != key_map_.end(); ++x) {
					log_.Infof("  %s -> %s", x->first.c_str(), x->second.c_str());
				}
			}
		} break;
		case kCommandEcho: {
			bool usage = false;
			if (parameter_vector.size() >= 2) {
				int log_level = 0;
				if (strutil::StringToInt(parameter_vector[0], log_level)) {
					const str c_message = strutil::Join(parameter_vector, " ", 1);
					str message;
					if (strutil::CStringToString(c_message, message)) {
						log_.Print((LogLevel)log_level, message+'\n');
					} else {
						log_.Warning("Invalid message C-string.");
					}
				} else {
					usage = true;
				}
			} else {
				usage = true;
			}
			if (usage) {
				log_.Warningf("usage: %s <log_level> <text to log>", command.c_str());
				log_.Warning("Prints to <text to log> on all loggers at <log_level> (integer).");
				result = 1;
			}
		} break;
		case kCommandDebugBreak: {
#ifdef LEPRA_DEBUG
			log_.Warningf("Debug break.");
#ifdef LEPRA_MSVC
			::DebugBreak();
#else // Other compilers
			deb_assert(false);
#endif // MSVC / others
#else // Release
			log_trace("debug break is not available in non-debug builds.");
#endif // Debug / Release
		} break;
		case kCommandHelp: {
			if (GetConsoleLogger()) {
				GetConsoleLogger()->OnLogRawMessage("These are the available commands:\n");
				std::list<str> command_list = GetCommandList();
				PrintCommandList(command_list);
			}
		} break;
		case kCommandSetStdoutLogLevel: {
			int log_level = 0;
			if (parameter_vector.size() == 1 && strutil::StringToInt(parameter_vector[0], log_level)) {
				const char* listener_name_array[] = { "console", "i-console", "file", };
				for (size_t x = 0; x < LEPRA_ARRAY_COUNT(listener_name_array); ++x) {
					LogListener* logger = LogType::GetLogger(LogType::kRoot)->GetListener(listener_name_array[x]);
					if (logger) {
						logger->SetLevelThreashold((LogLevel)log_level);
						LogLevel new_log_level = logger->GetLevelThreashold();
						if (new_log_level != log_level) {
							log_.Infof("Listener '%s' log level clamped to %i.", listener_name_array[x], new_log_level);
						}
					}
				}
			} else {
				log_.Warningf("usage: %s <log level>", command.c_str());
				log_.Warningf("where log level is 0=trace, 1=debug... %i=only fatal errors", kLevelTypeCount-1);
				result = 1;
			}
		} break;
		case kCommandSetSubsystemLogLevel: {
			int log_level = 0;
			if (parameter_vector.size() == 2 && strutil::StringToInt(parameter_vector[1], log_level)) {
				Logger* log = LogType::GetLogger(parameter_vector[0]);
				if (log) {
					log->SetLevelThreashold((LogLevel)log_level);
					LogLevel new_log_level = log->GetLevelThreashold();
					log_.Infof("Logger level for subsystem '%s' set to %i.", parameter_vector[0].c_str(), new_log_level);
				} else {
					log_.Infof("Unknown log \"%s\".", parameter_vector[0].c_str());
				}
			} else if (parameter_vector.size() == 1 && strutil::StringToInt(parameter_vector[0], log_level)) {
				const std::vector<Logger*> log_array = LogType::GetLoggers();
				std::vector<Logger*>::const_iterator x = log_array.begin();
				LogLevel new_log_level = kLevelLowestType;
				for (; x != log_array.end(); ++x) {
					(*x)->SetLevelThreashold((LogLevel)log_level);
					new_log_level = (*x)->GetLevelThreashold();
				}
				log_.Infof("All logs levels' set to %i.", new_log_level);
			} else {
				log_.Warningf("usage: %s [<subsystem>] <log level>", command.c_str());
				log_.Warningf("where subsystem is Root, Network, Game, UI...");
				log_.Warningf("where log level is 0=trace, 1=debug... %i=only fatal errors", kLevelTypeCount-1);
				result = 1;
			}
		} break;
		case kCommandDumpPerformanceInfo: {
			if (game_manager_) {
				game_manager_->UpdateReportPerformance(true, 0);
			} else {
				log_.Error("Can not dump performance info, since game manager not present in this context.");
			}
		} break;
		case kCommandClearPerformanceInfo: {
			if (game_manager_) {
				log_performance("Clearing performance data.");
				game_manager_->ClearPerformanceData();
			} else {
				log_.Error("Can not clear performance info, since game manager not present in this context.");
			}
		} break;
		case kCommandShellExecute: {
			if (parameter_vector.size() == 1) {
				const int error_code = ::system(parameter_vector[0].c_str());
				if (error_code != 0) {
					log_.Errorf("Program '%s' returned error code %i.", parameter_vector[0].c_str(), error_code);
					result = 1;
				}
			} else {
				log_.Warningf("usage: %s <shell command line>", command.c_str());
				result = 1;
			}
		} break;
		case kCommandShowSystemInfo: {
			log_.Infof("Login:            %s", SystemManager::GetLoginName().c_str());
			log_.Infof("Full name:        %s", SystemManager::QueryFullUserName().c_str());
			log_.Infof("OS:               %s", SystemManager::GetOsName().c_str());
			log_.Infof("CPU:              %s at %sHz", SystemManager::GetCpuName().c_str(), Number::ConvertToPostfixNumber((double)SystemManager::QueryCpuFrequency(), 1).c_str());
			log_.Infof("CPU count:        %u physical, %u cores, %u logical", SystemManager::GetPhysicalCpuCount(), SystemManager::GetCoreCount(), SystemManager::GetLogicalCpuCount());
			log_.Infof("Physical RAM:     %sB", Number::ConvertToPostfixNumber((double)SystemManager::GetAmountRam(), 1).c_str());
			log_.Infof("Available RAM:    %sB", Number::ConvertToPostfixNumber((double)SystemManager::GetAvailRam(), 1).c_str());
			log_.Infof("Sleep resolution: %f s", SystemManager::GetSleepResolution());
		} break;
		case kCommandShowGameInfo: {
			int target_fps;
			v_get(target_fps, =, GetVariableScope(), kRtvarPhysicsFps, 2);
			log_.Infof("Target frame rate:     %i", target_fps);
			if (game_manager_) {
				log_.Infof("Current frame rate:    %g", 1/game_manager_->GetTimeManager()->GetRealNormalFrameTime());
				log_.Infof("Absolute time:	      %g", game_manager_->GetTimeManager()->GetAbsoluteTime());
				log_.Infof("Current physics frame: %i", game_manager_->GetTimeManager()->GetCurrentPhysicsFrame());
			}
		} break;
		case kCommandHangGame: {
			if (game_manager_) {
				game_manager_->GetTickLock()->Acquire();
			}
		} break;
		case kCommandUnhangGame: {
			if (game_manager_) {
				game_manager_->GetTickLock()->Release();
			}
		} break;
		case kCommandFork: {
			if (parameter_vector.size() >= 1) {
				if (!ForkExecuteCommand(strutil::Join(parameter_vector, " "))) {
					log_.Error("Could not start fork!");
				}
			} else {
				log_.Warningf("usage: %s <command> [<arg> ...]", command.c_str());
				log_.Warning("Creates a new thread and executes <command> asynchronously.");
				result = 1;
			}
		} break;
		case kCommandNop: {
			// Mhm...
		} break;
		case kCommandExecuteVariable: {
			if (parameter_vector.size() == 1) {
				str value = GetVariableScope()->GetDefaultValue(cure::RuntimeVariableScope::kReadOnly, parameter_vector[0]);
				if (ExecuteCommand(value) == 0) {
					log_.Infof("Variable %s executed OK.", parameter_vector[0].c_str());
				} else if (!Thread::GetCurrentThread()->GetStopRequest()) {
					log_.Errorf("Variable %s was NOT sucessfully executed.", parameter_vector[0].c_str());
					result = 1;
				}
			} else {
				log_.Warningf("usage: %s <variable_name>", command.c_str());
				log_.Warning("Executes contents of <variable_name>.");
				result = 1;
			}
		} break;
		case kCommandExecuteFile: {
#ifndef LEPRA_TOUCH	// Only default parameters are good enough for touch devices.
			size_t filename_index = 0;
			bool ignore_if_missing = false;
			if (parameter_vector[0] == "-i") {
				ignore_if_missing = true;
				++filename_index;
			}
			if (filename_index < parameter_vector.size()) {
				DiskFile _file;
				if (_file.Open(parameter_vector[filename_index], DiskFile::kModeTextRead)) {
					str line;
					int line_number = 1;
					Thread* self = Thread::GetCurrentThread();
					for (; result == 0 && _file.ReadLine(line) == kIoOk && !self->GetStopRequest(); ++line_number) {
						const str converted_line = line;
						result = ExecuteCommand(converted_line);
					}
					if (result == 0) {
						//log_.Infof("File %s executed OK.", parameter_vector[0].c_str());
					} else {
						if (!self->GetStopRequest()) {
							log_.Errorf("File %s was NOT sucessfully executed; error in line %i.", parameter_vector[0].c_str(), line_number);
							result += 100;
						} else {
							result = 0;	// Problem probably caused by termination. Ignore it.
						}
					}
				} else if (!ignore_if_missing) {
					log_.Errorf("File %s did not open for reading.", parameter_vector[0].c_str());
					result = 2;
				}
			} else {
				log_.Warningf("usage: %s [-i] <file_name>", command.c_str());
				log_.Warning("Executes contents of <file_name>.");
				log_.Warning("  -i   Ignores non-existing shell file.");
				result = 1;
			}
#endif // Not for touch devices
		} break;
		case kCommandListActiveResources: {
			typedef cure::ResourceManager::ResourceInfoList ResourceInfoList;
			ResourceInfoList name_type_list = resource_manager_->QueryResourceNames();
			log_.Infof("Currently %u active resources:", name_type_list.size());
			for (ResourceInfoList::iterator x = name_type_list.begin(); x != name_type_list.end(); ++x) {
				log_.Info(strutil::Format("  %s [%s] acquired %i times", x->name_.c_str(), x->type_.c_str(), x->reference_count_));
			}

			ResourceTracker::CounterMap resource_map = g_resource_tracker.GetAll();
			log_.Infof("Currently %u resource trackers:", resource_map.size());
			ResourceTracker::CounterMap::iterator y = resource_map.begin();
			for (; y != resource_map.end(); ++y) {
				log_.Info(strutil::Format("  %s acquired %i times", y->first.c_str(), y->second));
			}
		} break;
		case kCommandSleep: {
			double time = -1;
			if (parameter_vector.size() == 1 && strutil::StringToDouble(parameter_vector[0], time)) {
				while (time > 0.5f && !Thread::GetCurrentThread()->GetStopRequest()) {
					Thread::Sleep(0.5f);
					time -= 0.5f;
				}
				if (time > 0 && !Thread::GetCurrentThread()->GetStopRequest()) {
					Thread::Sleep(time);
				}
			} else {
				log_.Warningf("usage: %s <secs>", command.c_str());
				log_.Warning("Sleeps <secs> where secs is a decimal number, e.g. 0.001.");
				result = 1;
			}
		} break;
		case kCommandWaitLoaded: {
			log_.Info("Waiting for resource pump to complete loading...");
			// Start out by waiting for the game manager, if we don't have one yet.
			for (int x = 0; !game_manager_ && x < 200; ++x) {
				Thread::Sleep(0.1);
			}
			if (resource_manager_->WaitLoading()) {
				log_.Info("Everything loaded.");
			} else {
				log_.Error("Load not completed in time!");
				result = 1;
			}
		} break;
		case kCommandPush: {
			if (parameter_vector.size() >= 1) {
				PushYieldCommand(strutil::Join(parameter_vector, " "));
			} else {
				log_.Warningf("usage: %s command [arg [arg ...]]", command.c_str());
				log_.Warning("Pushes a command to be executed next time the shell is idle.");
				result = 1;
			}
		} break;
		case kCommandRepeat: {
			bool usage = false;
			if (parameter_vector.size() >= 2) {
				int repeat_count = 0;
				if (strutil::StringToInt(parameter_vector[0], repeat_count)) {
					const str _cmd = strutil::Join(parameter_vector, " ", 1);
					for (int x = 0; x < repeat_count; ++x) {
						ExecuteCommand(_cmd);
					}
				} else {
					usage = true;
				}
			} else {
				usage = true;
			}
			if (usage) {
				log_.Warningf("usage: %s <count> command [arg [arg ...]]", command.c_str());
				log_.Warning("Repeats a command <count> times.");
				result = 1;
			}
		} break;
		case kCommandSetDefaultConfig: {
			cure::Init();
		} break;
		case kCommandSaveSystemConfigFile:
		case kCommandSaveApplicationConfigFile: {
#ifndef LEPRA_TOUCH	// Only default parameters are good enough for touch devices.
			bool usage = false;
			if (parameter_vector.size() >= 1 && parameter_vector.size() <= 3) {
				size_t filename_index = 0;
				bool ignore_if_exists = false;
				if (parameter_vector[0] == "-i") {
					ignore_if_exists = true;
					++filename_index;
				}
				if (_command == kCommandSaveSystemConfigFile) {
					++filename_index;
				}
				int _scope_skip_count = 0;
				if (filename_index < parameter_vector.size() &&
					((_command == kCommandSaveSystemConfigFile &&
					strutil::StringToInt(parameter_vector[filename_index-1], _scope_skip_count))
					||
					(_command == kCommandSaveApplicationConfigFile &&
					filename_index < parameter_vector.size()))) {
					str filename = parameter_vector[filename_index];
					if (!ignore_if_exists || !DiskFile::Exists(filename)) {
						str _user_config;
						DiskFile disk_file;
						if (disk_file.Open(filename, DiskFile::kModeTextRead)) {
							_user_config = LoadUserConfig(&disk_file);
						}
						MemFile mem_file;
						bool store_result;
						if (_command == kCommandSaveSystemConfigFile) {
							store_result = SaveSystemConfigFile(_scope_skip_count, &mem_file, _user_config);
						} else {
							store_result = SaveApplicationConfigFile(&mem_file, _user_config);
						}
						disk_file.SeekSet(0);
						mem_file.SeekSet(0);
						if (disk_file.HasSameContent(mem_file, mem_file.GetSize())) {
							// Skip silently, there is no need to rewrite the file on disk.
						} else if (disk_file.Open(filename, DiskFile::kModeTextWrite)) {
							store_result = (disk_file.WriteData(mem_file.GetBuffer(), (size_t)mem_file.GetSize()) == kIoOk);
							//store_result = (disk_file.WriteData("HEJSAN!\n", (size_t)8) == kIoOk);
							if (store_result) {
								log_.Infof("Successfully wrote script %s to disk.", filename.c_str());
							} else {
								log_.Errorf("Script %s was NOT sucessfully saved; error writing to disk.", filename.c_str());
								result = 2;
							}
						} else {
							log_.Errorf("Script %s was NOT sucessfully saved; error opening for writing.", filename.c_str());
							result = 2;
						}
					} else {
						log_.Infof("Script %s not written; already exists.", filename.c_str());
					}
				} else {
					usage = true;
				}
			} else {
				usage = true;
			}
			if (usage) {
				log_.Warningf("usage: %s [<-i>] {skip_scope_count} <file_name>", command.c_str());
				log_.Warning("Saves settings to <file_name>.");
				log_.Warning("  {skip_scope_count}  How many variable scopes to skip before writing. Not valid for all cmds.");
				log_.Warning("  -i                  Skips overwriting in case of already existing file.");
				result = 1;
			}
#endif // Not for touch devices
		} break;
		default: {
			if (!command.empty() && command[0] == '#') {
				char _level = command[1];
				const int variable_name_index = (_level == '/' || _level == '.')? 2 : 1;
				typedef cure::RuntimeVariable RtVar;
				typedef cure::RuntimeVariableScope RtScope;
				RtScope* scope = (_level == '/')? cure::GetSettings() : GetVariableScope();
				const str variable = command.substr(variable_name_index);
				if (parameter_vector.size() == 0) {
					if (scope->IsDefined(variable)) {
						str value = scope->GetUntypedDefaultValue(RtScope::kReadOnly, variable);
						value = strutil::StringToCString(value);
						log_.Infof("%s %s", variable.c_str(), value.c_str());
					} else {
						log_.Warningf("Variable %s not defined.", variable.c_str());
						result = 1;
					}
				} else if (parameter_vector.size() == 1) {
					str value;
					strutil::CStringToString(parameter_vector[0], value);
					RtScope::SetMode mode = (_level == '.')? RtVar::kUsageUserOverride : RtVar::kUsageNormal;
					bool ok = true;
					cure::GameManager* manager = GetGameManager();
					if (manager) {
						ok = manager->ValidateVariable(security_level_, variable, value);
						manager->GetTickLock()->Acquire();
					}
					if (ok) {
						ok = scope->SetUntypedValue(mode, variable, value);
					}
					if (manager) {
						manager->GetTickLock()->Release();
					}
					if (!ok) {
						result = 1;
					}
				} else {
					log_.Warningf("usage: #<variable_name> [<value>]", command.c_str());
					log_.Warning("Prints variable_name or sets it to <value>.");
					result = 1;
				}
			} else {
				result = -1;
			}
		} break;
	}
	return (result);
}



bool ConsoleManager::SaveApplicationConfigFile(File* file, const str& user_config) {
	file->WriteString("// Generated application shell script section.\n");
	std::list<str> _variable_list = GetVariableScope()->GetVariableNameList(cure::RuntimeVariableScope::kSearchExportable, 0, 0);
	bool saved = SaveConfigFile(file, str("."), _variable_list, user_config);
	return (saved);
}

cure::ResourceManager* ConsoleManager::GetResourceManager() const {
	return resource_manager_;
}



str ConsoleManager::LoadUserConfig(File* file) {
	str _user_config;
	str line;
	for (bool in_user_config = false; file->ReadLine(line) == kIoOk; ) {
		if (in_user_config) {
			_user_config += line + "\n";
		} else if (line.find(kUserSectionMark) != str::npos) {
			in_user_config = true;
		}
	}
	return _user_config;
}

bool ConsoleManager::SaveSystemConfigFile(int scope_skip_count, File* file, const str& user_config) {
	file->WriteString("// Generated system shell script section.\n");
	std::list<str> _variable_list = GetVariableScope()->GetVariableNameList(cure::RuntimeVariableScope::kSearchExportable, scope_skip_count);
	return (SaveConfigFile(file, str("/"), _variable_list, user_config));
}

bool ConsoleManager::SaveConfigFile(File* file, const str& prefix, std::list<str>& variable_list, const str& user_config) {
	file->WriteString("// Only change variable values below - use user section for all else.\n");
	file->WriteString("set-stdout-log-level 4\n");

	variable_list.sort();
	str last_group;
	str group_delimitors;
	v_get(group_delimitors, =, GetVariableScope(), kRtvarConsoleCharacterdelimitors, " ");
	std::list<str>::const_iterator x = variable_list.begin();
	for (; x != variable_list.end(); ++x) {
		const str& variable = *x;
		const str group = strutil::Split(variable, group_delimitors, 1)[0];
		if (last_group != group) {
			file->WriteString("\n");
			last_group = group;
		}
		str value = GetVariableScope()->GetUntypedDefaultValue(cure::RuntimeVariableScope::kReadOnly, variable);
		value = strutil::StringToCString(value);
		if (GetVariableScope()->GetUntypedType(value) == cure::RuntimeVariable::kDatatypeString) {
			value = '"'+value+'"';
		}
		str default_value = GetVariableScope()->GetUntypedDefaultValue(cure::RuntimeVariableScope::kReadDefault, variable);
		default_value = strutil::StringToCString(default_value);
		if (GetVariableScope()->GetUntypedType(default_value) == cure::RuntimeVariable::kDatatypeString) {
			default_value = '"'+default_value+'"';
		}
		default_value = (value != default_value)? "\t// Default is "+default_value+".\n" : "\n";
		file->WriteString("#"+prefix+variable+" "+value+default_value);
	}

	file->WriteString("\n");
	for (AliasMap::iterator y = alias_map_.begin(); y != alias_map_.end(); ++y) {
		file->WriteString("alias " + GetQuoted(y->first) + " " + GetQuoted(y->second) + "\n");
	}

	file->WriteString("\n");
	for (KeyMap::iterator y = key_map_.begin(); y != key_map_.end(); ++y) {
		file->WriteString("bind-key " + GetQuoted(y->first) + " " + GetQuoted(y->second) + "\n");
	}

	file->WriteString("\nset-stdout-log-level 1\n");
	file->WriteString("\n" kUserSectionMark " -- User config. Everything but variable values will be overwritten above this section!\n");
	file->WriteString(user_config);
	return (true);	// TODO: check if all writes went well.
}

str ConsoleManager::GetQuoted(const str& s) {
	if (std::find_if(s.begin(), s.end(), is_invalid_char) == s.end()) {
		return s;
	}
	if (s.find("\"") != str::npos) {	// Partially quoted strings as is.
		return s;
	}
	return "\"" + s + "\"";
}



loginstance(kConsole, ConsoleManager);



}
