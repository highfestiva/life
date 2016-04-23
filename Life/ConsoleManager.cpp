
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "ConsoleManager.h"
#include <algorithm>
#include "../Cure/Include/GameManager.h"
#include "../Cure/Include/ResourceManager.h"
#include "../Cure/Include/RuntimeVariable.h"
#include "../Cure/Include/TimeManager.h"
#include "../Lepra/Include/CyclicArray.h"
#include "../Lepra/Include/LepraOS.h"
#include "../Lepra/Include/LogListener.h"
#include "../Lepra/Include/Number.h"
#include "../Lepra/Include/ResourceTracker.h"
#include "../Lepra/Include/SystemManager.h"
#include "../UiTbc/Include/GUI/UiConsolePrompt.h"



#define USER_SECTION_MARK	"//!MARK!//"



namespace
{
bool is_invalid_char(char c)
{
	return !isalnum(c) && c != '-' && c != '#' && c != '.';
}
}



namespace Life
{



// Must lie before ConsoleManager to compile.
const ConsoleManager::CommandPair ConsoleManager::mCommandIdList[] =
{
	// IO.
	{"alias", COMMAND_ALIAS},
	{"bind-key", COMMAND_BIND_KEY},
	{"echo", COMMAND_ECHO},
	{"execute-file", COMMAND_EXECUTE_FILE},
	{"execute-variable", COMMAND_EXECUTE_VARIABLE},
	{"fork", COMMAND_FORK},
	{"nop", COMMAND_NOP},
	{"list-active-resources", COMMAND_LIST_ACTIVE_RESOURCES},
	{"push", COMMAND_PUSH},
	{"repeat", COMMAND_REPEAT},
	{"save-system-config-file", COMMAND_SAVE_SYSTEM_CONFIG_FILE},
	{"save-application-config-file", COMMAND_SAVE_APPLICATION_CONFIG_FILE},
	{"set-default-config", COMMAND_SET_DEFAULT_CONFIG},
	{"set-stdout-log-level", COMMAND_SET_STDOUT_LOG_LEVEL},
	{"set-subsystem-log-level", COMMAND_SET_SUBSYSTEM_LOG_LEVEL},
	{"sleep", COMMAND_SLEEP},
	{"wait-loaded", COMMAND_WAIT_LOADED},

	// Info/debug stuff.
	{"clear-performance-info", COMMAND_CLEAR_PERFORMANCE_INFO},
	{"debug-break", COMMAND_DEBUG_BREAK},
	{"dump-performance-info", COMMAND_DUMP_PERFORMANCE_INFO},
	{"help", COMMAND_HELP},
	{"shell-execute", COMMAND_SHELL_EXECUTE},
	{"system-info", COMMAND_SHOW_SYSTEM_INFO},
	{"game-info", COMMAND_SHOW_GAME_INFO},
	{"game-hang", COMMAND_HANG_GAME},
	{"game-unhang", COMMAND_UNHANG_GAME},
};



ConsoleManager::ConsoleManager(Cure::ResourceManager* pResourceManager, Cure::GameManager* pGameManager,
	Cure::RuntimeVariableScope* pVariableScope, InteractiveConsoleLogListener* pConsoleLogger,
	ConsolePrompt* pConsolePrompt):
	Cure::ConsoleManager(pVariableScope, pConsoleLogger, pConsolePrompt),
	mSecurityLevel(0),
	mGameManager(pGameManager),
	mResourceManager(pResourceManager),
	mLogger(0)
{
}

ConsoleManager::~ConsoleManager()
{
}



void ConsoleManager::InitCommands()
{
	Parent::InitCommands();
	GetConsoleCommandManager()->SetComment("//");
	GetConsoleCommandManager()->AddCompleter(new Cure::RuntimeVariableCompleter(GetVariableScope(), "#"));
	GetConsoleCommandManager()->AddCompleter(new Cure::RuntimeVariableCompleter(Cure::GetSettings(), "#/"));
	GetConsoleCommandManager()->AddCompleter(new Cure::RuntimeVariableCompleter(GetVariableScope(), "#."));

	ExecuteCommand("alias trace-log-level \"set-stdout-log-level 0; set-subsystem-log-level 0\"");
	ExecuteCommand("alias debug-log-level \"set-stdout-log-level 1; set-subsystem-log-level 1\"");
	ExecuteCommand("alias performance-log-level \"set-stdout-log-level 2; set-subsystem-log-level 2\"");
	ExecuteCommand("alias info-log-level \"set-stdout-log-level 3; set-subsystem-log-level 3\"");
	ExecuteCommand("alias headline-log-level \"set-stdout-log-level 4; set-subsystem-log-level 4\"");
	ExecuteCommand("alias warning-log-level \"set-stdout-log-level 5; set-subsystem-log-level 5\"");
	ExecuteCommand("alias error-log-level \"set-stdout-log-level 6; set-subsystem-log-level 6\"");
	ExecuteCommand("alias debug-focus \"set-subsystem-log-level 4; set-stdout-log-level 0; set-subsystem-log-level\"");
}



void ConsoleManager::SetSecurityLevel(int pLevel)
{
	mSecurityLevel = pLevel;
}

int ConsoleManager::GetSecurityLevel() const
{
	return mSecurityLevel;
}



Cure::GameManager* ConsoleManager::GetGameManager() const
{
	return (mGameManager);
}

void ConsoleManager::SetGameManager(Cure::GameManager* pGameManager)
{
	mGameManager = pGameManager;
}



void ConsoleManager::OnKey(const str& pKeyName)
{
	KeyMap::iterator x = mKeyMap.find(pKeyName);
	if (x != mKeyMap.end())
	{
		PushYieldCommand(x->second);
		// Make console thread run it.
		((UiTbc::ConsolePrompt*)mConsolePrompt)->OnChar(' ');
		((UiTbc::ConsolePrompt*)mConsolePrompt)->OnChar('\b');
	}
}



unsigned ConsoleManager::GetCommandCount() const
{
	return LEPRA_ARRAY_COUNT(mCommandIdList);
}

const ConsoleManager::CommandPair& ConsoleManager::GetCommand(unsigned pIndex) const
{
	return (mCommandIdList[pIndex]);
}

int ConsoleManager::TranslateCommand(const HashedString& pCommand) const
{
	int lTranslation = Parent::TranslateCommand(pCommand);
	if (lTranslation == -1)
	{
		if (mAliasMap.find(pCommand) != mAliasMap.end())
		{
			lTranslation = COMMAND_ALIAS_VALUE;
		}
	}
	return (lTranslation);
}

int ConsoleManager::OnCommand(const HashedString& pCommand, const strutil::strvec& pParameterVector)
{
	int lResult = 0;
	CommandCommon lCommand = (CommandCommon)TranslateCommand(pCommand);
	switch (lCommand)
	{
		case COMMAND_ALIAS:
		{
			//bool lUsage = false;
			if (pParameterVector.size() >= 2)
			{
				GetConsoleCommandManager()->RemoveCommand(pParameterVector[0]);
				strutil::strvec lArgs(pParameterVector);
				lArgs.erase(lArgs.begin());
				if (lArgs.size() > 1)
				{
					for (size_t x = 0; x < lArgs.size(); ++x)
					{
						if (lArgs[x].find(" ") != str::npos)
						{
							lArgs[x] = "\"" + lArgs[x] + "\"";
						}
					}
				}
				mAliasMap[pParameterVector[0]] = strutil::Join(lArgs, " ");;
				GetConsoleCommandManager()->AddCommand(pParameterVector[0]);
			}
			else if (pParameterVector.size() == 1)
			{
				mAliasMap.erase(pParameterVector[0]);
				GetConsoleCommandManager()->RemoveCommand(pParameterVector[0]);
			}
			else
			{
				mLog.Info("List of aliases:");
				for (AliasMap::iterator x = mAliasMap.begin(); x != mAliasMap.end(); ++x)
				{
					mLog.Infof("  %s -> %s", x->first.c_str(), x->second.c_str());
				}
			}
			/*if (lUsage)
			{
				mLog.Warningf("usage: %s <alias_name> [<command>]", pCommand.c_str());
				mLog.Warning("Adds a new alias, or removes one if <command is left out.");
				lResult = 1;
			}*/
		}
		break;
		case COMMAND_ALIAS_VALUE:
		{
			AliasMap::iterator x = mAliasMap.find(pCommand);
			if (x != mAliasMap.end())
			{
				lResult = ExecuteCommand(x->second + " " + strutil::Join(pParameterVector, " "));
			}
			else
			{
				mLog.Warningf("Alias %s is removed.", pCommand.c_str());
				lResult = 1;
			}
		}
		break;
		case COMMAND_BIND_KEY:
		{
			if (pParameterVector.size() == 2)
			{
				mKeyMap[pParameterVector[0]] = pParameterVector[1];
			}
			else if (pParameterVector.size() == 1)
			{
				mKeyMap.erase(pParameterVector[0]);
			}
			else
			{
				mLog.Info("List of keys:");
				for (KeyMap::iterator x = mKeyMap.begin(); x != mKeyMap.end(); ++x)
				{
					mLog.Infof("  %s -> %s", x->first.c_str(), x->second.c_str());
				}
			}
		}
		break;
		case COMMAND_ECHO:
		{
			bool lUsage = false;
			if (pParameterVector.size() >= 2)
			{
				int lLogLevel = 0;
				if (strutil::StringToInt(pParameterVector[0], lLogLevel))
				{
					const str lCMessage = strutil::Join(pParameterVector, " ", 1);
					str lMessage;
					if (strutil::CStringToString(lCMessage, lMessage))
					{
						mLog.Print((LogLevel)lLogLevel, lMessage+'\n');
					}
					else
					{
						mLog.Warning("Invalid message C-string.");
					}
				}
				else
				{
					lUsage = true;
				}
			}
			else
			{
				lUsage = true;
			}
			if (lUsage)
			{
				mLog.Warningf("usage: %s <log_level> <text to log>", pCommand.c_str());
				mLog.Warning("Prints to <text to log> on all loggers at <log_level> (integer).");
				lResult = 1;
			}
		}
		break;
		case COMMAND_DEBUG_BREAK:
		{
#ifdef LEPRA_DEBUG
			mLog.Warningf("Debug break.");
#ifdef LEPRA_MSVC
			::DebugBreak();
#else // Other compilers
			deb_assert(false);
#endif // MSVC / others
#else // Release
			log_trace("debug break is not available in non-debug builds.");
#endif // Debug / Release
		}
		break;
		case COMMAND_HELP:
		{
			if (GetConsoleLogger())
			{
				GetConsoleLogger()->OnLogRawMessage("These are the available commands:\n");
				std::list<str> lCommandList = GetCommandList();
				PrintCommandList(lCommandList);
			}
		}
		break;
		case COMMAND_SET_STDOUT_LOG_LEVEL:
		{
			int lLogLevel = 0;
			if (pParameterVector.size() == 1 && strutil::StringToInt(pParameterVector[0], lLogLevel))
			{
				const char* lListenerNameArray[] = { "console", "i-console", "file", };
				for (size_t x = 0; x < LEPRA_ARRAY_COUNT(lListenerNameArray); ++x)
				{
					LogListener* lLogger = LogType::GetLogger(LogType::ROOT)->GetListener(lListenerNameArray[x]);
					if (lLogger)
					{
						lLogger->SetLevelThreashold((LogLevel)lLogLevel);
						LogLevel lNewLogLevel = lLogger->GetLevelThreashold();
						if (lNewLogLevel != lLogLevel)
						{
							mLog.Infof("Listener '%s' log level clamped to %i.", lListenerNameArray[x], lNewLogLevel);
						}
					}
				}
			}
			else
			{
				mLog.Warningf("usage: %s <log level>", pCommand.c_str());
				mLog.Warningf("where log level is 0=trace, 1=debug... %i=only fatal errors", LEVEL_TYPE_COUNT-1);
				lResult = 1;
			}
		}
		break;
		case COMMAND_SET_SUBSYSTEM_LOG_LEVEL:
		{
			int lLogLevel = 0;
			if (pParameterVector.size() == 2 && strutil::StringToInt(pParameterVector[1], lLogLevel))
			{
				Logger* lLog = LogType::GetLogger(pParameterVector[0]);
				if (lLog)
				{
					lLog->SetLevelThreashold((LogLevel)lLogLevel);
					LogLevel lNewLogLevel = lLog->GetLevelThreashold();
					mLog.Infof("Logger level for subsystem '%s' set to %i.", pParameterVector[0].c_str(), lNewLogLevel);
				}
				else
				{
					mLog.Infof("Unknown log \"%s\".", pParameterVector[0].c_str());
				}
			}
			else if (pParameterVector.size() == 1 && strutil::StringToInt(pParameterVector[0], lLogLevel))
			{
				const std::vector<Logger*> lLogArray = LogType::GetLoggers();
				std::vector<Logger*>::const_iterator x = lLogArray.begin();
				LogLevel lNewLogLevel = LEVEL_LOWEST_TYPE;
				for (; x != lLogArray.end(); ++x)
				{
					(*x)->SetLevelThreashold((LogLevel)lLogLevel);
					lNewLogLevel = (*x)->GetLevelThreashold();
				}
				mLog.Infof("All logs levels' set to %i.", lNewLogLevel);
			}
			else
			{
				mLog.Warningf("usage: %s [<subsystem>] <log level>", pCommand.c_str());
				mLog.Warningf("where subsystem is Root, Network, Game, UI...");
				mLog.Warningf("where log level is 0=trace, 1=debug... %i=only fatal errors", LEVEL_TYPE_COUNT-1);
				lResult = 1;
			}
		}
		break;
		case COMMAND_DUMP_PERFORMANCE_INFO:
		{
			if (mGameManager)
			{
				mGameManager->UpdateReportPerformance(true, 0);
			}
			else
			{
				mLog.Error("Can not dump performance info, since game manager not present in this context.");
			}
		}
		break;
		case COMMAND_CLEAR_PERFORMANCE_INFO:
		{
			if (mGameManager)
			{
				log_performance("Clearing performance data.");
				mGameManager->ClearPerformanceData();
			}
			else
			{
				mLog.Error("Can not clear performance info, since game manager not present in this context.");
			}
		}
		break;
		case COMMAND_SHELL_EXECUTE:
		{
			if (pParameterVector.size() == 1)
			{
				const int lErrorCode = ::system(pParameterVector[0].c_str());
				if (lErrorCode != 0)
				{
					mLog.Errorf("Program '%s' returned error code %i.", pParameterVector[0].c_str(), lErrorCode);
					lResult = 1;
				}
			}
			else
			{
				mLog.Warningf("usage: %s <shell command line>", pCommand.c_str());
				lResult = 1;
			}
		}
		break;
		case COMMAND_SHOW_SYSTEM_INFO:
		{
			mLog.Infof("Login:            %s", SystemManager::GetLoginName().c_str());
			mLog.Infof("Full name:        %s", SystemManager::QueryFullUserName().c_str());
			mLog.Infof("OS:               %s", SystemManager::GetOsName().c_str());
			mLog.Infof("CPU:              %s at %sHz", SystemManager::GetCpuName().c_str(), Number::ConvertToPostfixNumber((double)SystemManager::QueryCpuFrequency(), 1).c_str());
			mLog.Infof("CPU count:        %u physical, %u cores, %u logical", SystemManager::GetPhysicalCpuCount(), SystemManager::GetCoreCount(), SystemManager::GetLogicalCpuCount());
			mLog.Infof("Physical RAM:     %sB", Number::ConvertToPostfixNumber((double)SystemManager::GetAmountRam(), 1).c_str());
			mLog.Infof("Available RAM:    %sB", Number::ConvertToPostfixNumber((double)SystemManager::GetAvailRam(), 1).c_str());
			mLog.Infof("Sleep resolution: %f s", SystemManager::GetSleepResolution());
		}
		break;
		case COMMAND_SHOW_GAME_INFO:
		{
			int lTargetFps;
			v_get(lTargetFps, =, GetVariableScope(), RTVAR_PHYSICS_FPS, 2);
			mLog.Infof("Target frame rate:     %i", lTargetFps);
			if (mGameManager)
			{
				mLog.Infof("Current frame rate:    %g", 1/mGameManager->GetTimeManager()->GetRealNormalFrameTime());
				mLog.Infof("Absolute time:	      %g", mGameManager->GetTimeManager()->GetAbsoluteTime());
				mLog.Infof("Current physics frame: %i", mGameManager->GetTimeManager()->GetCurrentPhysicsFrame());
			}
		}
		break;
		case COMMAND_HANG_GAME:
		{
			if (mGameManager)
			{
				mGameManager->GetTickLock()->Acquire();
			}
		}
		break;
		case COMMAND_UNHANG_GAME:
		{
			if (mGameManager)
			{
				mGameManager->GetTickLock()->Release();
			}
		}
		break;
		case COMMAND_FORK:
		{
			if (pParameterVector.size() >= 1)
			{
				if (!ForkExecuteCommand(strutil::Join(pParameterVector, " ")))
				{
					mLog.Error("Could not start fork!");
				}
			}
			else
			{
				mLog.Warningf("usage: %s <command> [<arg> ...]", pCommand.c_str());
				mLog.Warning("Creates a new thread and executes <command> asynchronously.");
				lResult = 1;
			}
		}
		break;
		case COMMAND_NOP:
		{
			// Mhm...
		}
		break;
		case COMMAND_EXECUTE_VARIABLE:
		{
			if (pParameterVector.size() == 1)
			{
				str lValue = GetVariableScope()->GetDefaultValue(Cure::RuntimeVariableScope::READ_ONLY, pParameterVector[0]);
				if (ExecuteCommand(lValue) == 0)
				{
					mLog.Infof("Variable %s executed OK.", pParameterVector[0].c_str());
				}
				else if (!Thread::GetCurrentThread()->GetStopRequest())
				{
					mLog.Errorf("Variable %s was NOT sucessfully executed.", pParameterVector[0].c_str());
					lResult = 1;
				}
			}
			else
			{
				mLog.Warningf("usage: %s <variable_name>", pCommand.c_str());
				mLog.Warning("Executes contents of <variable_name>.");
				lResult = 1;
			}
		}
		break;
		case COMMAND_EXECUTE_FILE:
		{
#ifndef LEPRA_TOUCH	// Only default parameters are good enough for touch devices.
			size_t lFilenameIndex = 0;
			bool lIgnoreIfMissing = false;
			if (pParameterVector[0] == "-i")
			{
				lIgnoreIfMissing = true;
				++lFilenameIndex;
			}
			if (lFilenameIndex < pParameterVector.size())
			{
				DiskFile lFile;
				if (lFile.Open(pParameterVector[lFilenameIndex], DiskFile::MODE_TEXT_READ))
				{
					str lLine;
					int lLineNumber = 1;
					Thread* lSelf = Thread::GetCurrentThread();
					for (; lResult == 0 && lFile.ReadLine(lLine) == IO_OK && !lSelf->GetStopRequest(); ++lLineNumber)
					{
						const str lConvertedLine = lLine;
						lResult = ExecuteCommand(lConvertedLine);
					}
					if (lResult == 0)
					{
						//mLog.Infof("File %s executed OK.", pParameterVector[0].c_str());
					}
					else
					{
						if (!lSelf->GetStopRequest())
						{
							mLog.Errorf("File %s was NOT sucessfully executed; error in line %i.", pParameterVector[0].c_str(), lLineNumber);
							lResult += 100;
						}
						else
						{
							lResult = 0;	// Problem probably caused by termination. Ignore it.
						}
					}
				}
				else if (!lIgnoreIfMissing)
				{
					mLog.Errorf("File %s did not open for reading.", pParameterVector[0].c_str());
					lResult = 2;
				}
			}
			else
			{
				mLog.Warningf("usage: %s [-i] <file_name>", pCommand.c_str());
				mLog.Warning("Executes contents of <file_name>.");
				mLog.Warning("  -i   Ignores non-existing shell file.");
				lResult = 1;
			}
#endif // Not for touch devices
		}
		break;
		case COMMAND_LIST_ACTIVE_RESOURCES:
		{
			typedef Cure::ResourceManager::ResourceInfoList ResourceInfoList;
			ResourceInfoList lNameTypeList = mResourceManager->QueryResourceNames();
			mLog.Infof("Currently %u active resources:", lNameTypeList.size());
			for (ResourceInfoList::iterator x = lNameTypeList.begin(); x != lNameTypeList.end(); ++x)
			{
				mLog.Info(strutil::Format("  %s [%s] acquired %i times", x->mName.c_str(), x->mType.c_str(), x->mReferenceCount));
			}

			ResourceTracker::CounterMap lResourceMap = gResourceTracker.GetAll();
			mLog.Infof("Currently %u resource trackers:", lResourceMap.size());
			ResourceTracker::CounterMap::iterator y = lResourceMap.begin();
			for (; y != lResourceMap.end(); ++y)
			{
				mLog.Info(strutil::Format("  %s acquired %i times", y->first.c_str(), y->second));
			}
		}
		break;
		case COMMAND_SLEEP:
		{
			double lTime = -1;
			if (pParameterVector.size() == 1 && strutil::StringToDouble(pParameterVector[0], lTime))
			{
				while (lTime > 0.5f && !Thread::GetCurrentThread()->GetStopRequest())
				{
					Thread::Sleep(0.5f);
					lTime -= 0.5f;
				}
				if (lTime > 0 && !Thread::GetCurrentThread()->GetStopRequest())
				{
					Thread::Sleep(lTime);
				}
			}
			else
			{
				mLog.Warningf("usage: %s <secs>", pCommand.c_str());
				mLog.Warning("Sleeps <secs> where secs is a decimal number, e.g. 0.001.");
				lResult = 1;
			}
		}
		break;
		case COMMAND_WAIT_LOADED:
		{
			mLog.Info("Waiting for resource pump to complete loading...");
			// Start out by waiting for the game manager, if we don't have one yet.
			for (int x = 0; !mGameManager && x < 200; ++x)
			{
				Thread::Sleep(0.1);
			}
			if (mResourceManager->WaitLoading())
			{
				mLog.Info("Everything loaded.");
			}
			else
			{
				mLog.Error("Load not completed in time!");
				lResult = 1;
			}
		}
		break;
		case COMMAND_PUSH:
		{
			if (pParameterVector.size() >= 1)
			{
				PushYieldCommand(strutil::Join(pParameterVector, " "));
			}
			else
			{
				mLog.Warningf("usage: %s command [arg [arg ...]]", pCommand.c_str());
				mLog.Warning("Pushes a command to be executed next time the shell is idle.");
				lResult = 1;
			}
		}
		break;
		case COMMAND_REPEAT:
		{
			bool lUsage = false;
			if (pParameterVector.size() >= 2)
			{
				int lRepeatCount = 0;
				if (strutil::StringToInt(pParameterVector[0], lRepeatCount))
				{
					const str lCommand = strutil::Join(pParameterVector, " ", 1);
					for (int x = 0; x < lRepeatCount; ++x)
					{
						ExecuteCommand(lCommand);
					}
				}
				else
				{
					lUsage = true;
				}
			}
			else
			{
				lUsage = true;
			}
			if (lUsage)
			{
				mLog.Warningf("usage: %s <count> command [arg [arg ...]]", pCommand.c_str());
				mLog.Warning("Repeats a command <count> times.");
				lResult = 1;
			}
		}
		break;
		case COMMAND_SET_DEFAULT_CONFIG:
		{
			Cure::Init();
		}
		break;
		case COMMAND_SAVE_SYSTEM_CONFIG_FILE:
		case COMMAND_SAVE_APPLICATION_CONFIG_FILE:
		{
#ifndef LEPRA_TOUCH	// Only default parameters are good enough for touch devices.
			bool lUsage = false;
			if (pParameterVector.size() >= 1 && pParameterVector.size() <= 3)
			{
				size_t lFilenameIndex = 0;
				bool lIgnoreIfExists = false;
				if (pParameterVector[0] == "-i")
				{
					lIgnoreIfExists = true;
					++lFilenameIndex;
				}
				if (lCommand == COMMAND_SAVE_SYSTEM_CONFIG_FILE)
				{
					++lFilenameIndex;
				}
				int lScopeSkipCount = 0;
				if (lFilenameIndex < pParameterVector.size() &&
					((lCommand == COMMAND_SAVE_SYSTEM_CONFIG_FILE &&
					strutil::StringToInt(pParameterVector[lFilenameIndex-1], lScopeSkipCount))
					||
					(lCommand == COMMAND_SAVE_APPLICATION_CONFIG_FILE &&
					lFilenameIndex < pParameterVector.size())))
				{
					str lFilename = pParameterVector[lFilenameIndex];
					if (!lIgnoreIfExists || !DiskFile::Exists(lFilename))
					{
						str lUserConfig;
						DiskFile lDiskFile;
						if (lDiskFile.Open(lFilename, DiskFile::MODE_TEXT_READ))
						{
							lUserConfig = LoadUserConfig(&lDiskFile);
						}
						MemFile lMemFile;
						bool lStoreResult;
						if (lCommand == COMMAND_SAVE_SYSTEM_CONFIG_FILE)
						{
							lStoreResult = SaveSystemConfigFile(lScopeSkipCount, &lMemFile, lUserConfig);
						}
						else
						{
							lStoreResult = SaveApplicationConfigFile(&lMemFile, lUserConfig);
						}
						lDiskFile.SeekSet(0);
						lMemFile.SeekSet(0);
						if (lDiskFile.HasSameContent(lMemFile, lMemFile.GetSize()))
						{
							// Skip silently, there is no need to rewrite the file on disk.
						}
						else if (lDiskFile.Open(lFilename, DiskFile::MODE_TEXT_WRITE))
						{
							lStoreResult = (lDiskFile.WriteData(lMemFile.GetBuffer(), (size_t)lMemFile.GetSize()) == IO_OK);
							//lStoreResult = (lDiskFile.WriteData("HEJSAN!\n", (size_t)8) == IO_OK);
							if (lStoreResult)
							{
								mLog.Infof("Successfully wrote script %s to disk.", lFilename.c_str());
							}
							else
							{
								mLog.Errorf("Script %s was NOT sucessfully saved; error writing to disk.", lFilename.c_str());
								lResult = 2;
							}
						}
						else
						{
							mLog.Errorf("Script %s was NOT sucessfully saved; error opening for writing.", lFilename.c_str());
							lResult = 2;
						}
					}
					else
					{
						mLog.Infof("Script %s not written; already exists.", lFilename.c_str());
					}
				}
				else
				{
					lUsage = true;
				}
			}
			else
			{
				lUsage = true;
			}
			if (lUsage)
			{
				mLog.Warningf("usage: %s [<-i>] {skip_scope_count} <file_name>", pCommand.c_str());
				mLog.Warning("Saves settings to <file_name>.");
				mLog.Warning("  {skip_scope_count}  How many variable scopes to skip before writing. Not valid for all cmds.");
				mLog.Warning("  -i                  Skips overwriting in case of already existing file.");
				lResult = 1;
			}
#endif // Not for touch devices
		}
		break;
		default:
		{
			if (!pCommand.empty() && pCommand[0] == '#')
			{
				char lLevel = pCommand[1];
				const int lVariableNameIndex = (lLevel == '/' || lLevel == '.')? 2 : 1;
				typedef Cure::RuntimeVariable RtVar;
				typedef Cure::RuntimeVariableScope RtScope;
				RtScope* lScope = (lLevel == '/')? Cure::GetSettings() : GetVariableScope();
				const str lVariable = pCommand.substr(lVariableNameIndex);
				if (pParameterVector.size() == 0)
				{
					if (lScope->IsDefined(lVariable))
					{
						str lValue = lScope->GetUntypedDefaultValue(RtScope::READ_ONLY, lVariable);
						lValue = strutil::StringToCString(lValue);
						mLog.Infof("%s %s", lVariable.c_str(), lValue.c_str());
					}
					else
					{
						mLog.Warningf("Variable %s not defined.", lVariable.c_str());
						lResult = 1;
					}
				}
				else if (pParameterVector.size() == 1)
				{
					str lValue;
					strutil::CStringToString(pParameterVector[0], lValue);
					RtScope::SetMode lMode = (lLevel == '.')? RtVar::USAGE_USER_OVERRIDE : RtVar::USAGE_NORMAL;
					bool lOk = true;
					Cure::GameManager* lManager = GetGameManager();
					if (lManager)
					{
						lOk = lManager->ValidateVariable(mSecurityLevel, lVariable, lValue);
						lManager->GetTickLock()->Acquire();
					}
					if (lOk)
					{
						lOk = lScope->SetUntypedValue(lMode, lVariable, lValue);
					}
					if (lManager)
					{
						lManager->GetTickLock()->Release();
					}
					if (!lOk)
					{
						lResult = 1;
					}
				}
				else
				{
					mLog.Warningf("usage: #<variable_name> [<value>]", pCommand.c_str());
					mLog.Warning("Prints variable_name or sets it to <value>.");
					lResult = 1;
				}
			}
			else
			{
				lResult = -1;
			}
		}
		break;
	}
	return (lResult);
}



bool ConsoleManager::SaveApplicationConfigFile(File* pFile, const str& pUserConfig)
{
	pFile->WriteString("// Generated application shell script section.\n");
	std::list<str> lVariableList = GetVariableScope()->GetVariableNameList(Cure::RuntimeVariableScope::SEARCH_EXPORTABLE, 0, 0);
	bool lSaved = SaveConfigFile(pFile, str("."), lVariableList, pUserConfig);
	return (lSaved);
}

Cure::ResourceManager* ConsoleManager::GetResourceManager() const
{
	return mResourceManager;
}



str ConsoleManager::LoadUserConfig(File* pFile)
{
	str lUserConfig;
	str lLine;
	for (bool lInUserConfig = false; pFile->ReadLine(lLine) == IO_OK; )
	{
		if (lInUserConfig)
		{
			lUserConfig += lLine + "\n";
		}
		else if (lLine.find(USER_SECTION_MARK) != str::npos)
		{
			lInUserConfig = true;
		}
	}
	return lUserConfig;
}

bool ConsoleManager::SaveSystemConfigFile(int pScopeSkipCount, File* pFile, const str& pUserConfig)
{
	pFile->WriteString("// Generated system shell script section.\n");
	std::list<str> lVariableList = GetVariableScope()->GetVariableNameList(Cure::RuntimeVariableScope::SEARCH_EXPORTABLE, pScopeSkipCount);
	return (SaveConfigFile(pFile, str("/"), lVariableList, pUserConfig));
}

bool ConsoleManager::SaveConfigFile(File* pFile, const str& pPrefix, std::list<str>& pVariableList, const str& pUserConfig)
{
	pFile->WriteString("// Only change variable values below - use user section for all else.\n");
	pFile->WriteString("set-stdout-log-level 4\n");

	pVariableList.sort();
	str lLastGroup;
	str lGroupDelimitors;
	v_get(lGroupDelimitors, =, GetVariableScope(), RTVAR_CONSOLE_CHARACTERDELIMITORS, " ");
	std::list<str>::const_iterator x = pVariableList.begin();
	for (; x != pVariableList.end(); ++x)
	{
		const str& lVariable = *x;
		const str lGroup = strutil::Split(lVariable, lGroupDelimitors, 1)[0];
		if (lLastGroup != lGroup)
		{
			pFile->WriteString("\n");
			lLastGroup = lGroup;
		}
		str lValue = GetVariableScope()->GetUntypedDefaultValue(Cure::RuntimeVariableScope::READ_ONLY, lVariable);
		lValue = strutil::StringToCString(lValue);
		if (GetVariableScope()->GetUntypedType(lValue) == Cure::RuntimeVariable::DATATYPE_STRING)
		{
			lValue = '"'+lValue+'"';
		}
		str lDefaultValue = GetVariableScope()->GetUntypedDefaultValue(Cure::RuntimeVariableScope::READ_DEFAULT, lVariable);
		lDefaultValue = strutil::StringToCString(lDefaultValue);
		if (GetVariableScope()->GetUntypedType(lDefaultValue) == Cure::RuntimeVariable::DATATYPE_STRING)
		{
			lDefaultValue = '"'+lDefaultValue+'"';
		}
		lDefaultValue = (lValue != lDefaultValue)? "\t// Default is "+lDefaultValue+".\n" : "\n";
		pFile->WriteString("#"+pPrefix+lVariable+" "+lValue+lDefaultValue);
	}

	pFile->WriteString("\n");
	for (AliasMap::iterator x = mAliasMap.begin(); x != mAliasMap.end(); ++x)
	{
		pFile->WriteString("alias " + GetQuoted(x->first) + " " + GetQuoted(x->second) + "\n");
	}

	pFile->WriteString("\n");
	for (KeyMap::iterator x = mKeyMap.begin(); x != mKeyMap.end(); ++x)
	{
		pFile->WriteString("bind-key " + GetQuoted(x->first) + " " + GetQuoted(x->second) + "\n");
	}

	pFile->WriteString("\nset-stdout-log-level 1\n");
	pFile->WriteString("\n" USER_SECTION_MARK " -- User config. Everything but variable values will be overwritten above this section!\n");
	pFile->WriteString(pUserConfig);
	return (true);	// TODO: check if all writes went well.
}

str ConsoleManager::GetQuoted(const str& s)
{
	if (std::find_if(s.begin(), s.end(), is_invalid_char) == s.end())
	{
		return s;
	}
	if (s.find("\"") != str::npos)	// Partially quoted strings as is.
	{
		return s;
	}
	return "\"" + s + "\"";
}



loginstance(CONSOLE, ConsoleManager);



}
