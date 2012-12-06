
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "ConsoleManager.h"
#include "../Cure/Include/GameManager.h"
#include "../Cure/Include/RuntimeVariable.h"
#include "../Cure/Include/TimeManager.h"
#include "../Lepra/Include/CyclicArray.h"
#include "../Lepra/Include/LogListener.h"
#include "../Lepra/Include/Number.h"
#include "../Lepra/Include/SystemManager.h"



#define USER_SECTION_MARK	L"//!MARK!//"



namespace Life
{



// Must lie before ConsoleManager to compile.
const ConsoleManager::CommandPair ConsoleManager::mCommandIdList[] =
{
	// IO.
	{_T("alias"), COMMAND_ALIAS},
	{_T("echo"), COMMAND_ECHO},
	{_T("execute-file"), COMMAND_EXECUTE_FILE},
	{_T("execute-variable"), COMMAND_EXECUTE_VARIABLE},
	{_T("fork"), COMMAND_FORK},
	{_T("list-active-resources"), COMMAND_LIST_ACTIVE_RESOURCES},
	{_T("push"), COMMAND_PUSH},
	{_T("repeat"), COMMAND_REPEAT},
	{_T("save-system-config-file"), COMMAND_SAVE_SYSTEM_CONFIG_FILE},
	{_T("save-application-config-file"), COMMAND_SAVE_APPLICATION_CONFIG_FILE},
	{_T("set-default-config"), COMMAND_SET_DEFAULT_CONFIG},
	{_T("set-stdout-log-level"), COMMAND_SET_STDOUT_LOG_LEVEL},
	{_T("set-subsystem-log-level"), COMMAND_SET_SUBSYSTEM_LOG_LEVEL},
	{_T("sleep"), COMMAND_SLEEP},
	{_T("wait-loaded"), COMMAND_WAIT_LOADED},

	// Info/debug stuff.
	{_T("clear-performance-info"), COMMAND_CLEAR_PERFORMANCE_INFO},
	{_T("debug-break"), COMMAND_DEBUG_BREAK},
	{_T("dump-performance-info"), COMMAND_DUMP_PERFORMANCE_INFO},
	{_T("help"), COMMAND_HELP},
	{_T("system-info"), COMMAND_SHOW_SYSTEM_INFO},
	{_T("game-info"), COMMAND_SHOW_GAME_INFO},
};



ConsoleManager::ConsoleManager(Cure::ResourceManager* pResourceManager, Cure::GameManager* pGameManager,
	Cure::RuntimeVariableScope* pVariableScope, InteractiveConsoleLogListener* pConsoleLogger,
	ConsolePrompt* pConsolePrompt):
	Cure::ConsoleManager(pVariableScope, pConsoleLogger, pConsolePrompt),
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
	GetConsoleCommandManager()->SetComment(_T("//"));
	GetConsoleCommandManager()->AddCompleter(new Cure::RuntimeVariableCompleter(GetVariableScope(), _T("#")));
	GetConsoleCommandManager()->AddCompleter(new Cure::RuntimeVariableCompleter(Cure::GetSettings(), _T("#/")));
	GetConsoleCommandManager()->AddCompleter(new Cure::RuntimeVariableCompleter(Cure::GetSettings(), _T("#.")));

	ExecuteCommand(_T("alias trace-log-level \"set-stdout-log-level 0; set-subsystem-log-level 0\""));
	ExecuteCommand(_T("alias debug-log-level \"set-stdout-log-level 1; set-subsystem-log-level 1\""));
	ExecuteCommand(_T("alias performance-log-level \"set-stdout-log-level 2; set-subsystem-log-level 2\""));
	ExecuteCommand(_T("alias info-log-level \"set-stdout-log-level 3; set-subsystem-log-level 3\""));
	ExecuteCommand(_T("alias headline-log-level \"set-stdout-log-level 4; set-subsystem-log-level 4\""));
	ExecuteCommand(_T("alias warning-log-level \"set-stdout-log-level 5; set-subsystem-log-level 5\""));
	ExecuteCommand(_T("alias error-log-level \"set-stdout-log-level 6; set-subsystem-log-level 6\""));
	ExecuteCommand(_T("alias debug-focus \"set-subsystem-log-level 4; set-stdout-log-level 0; set-subsystem-log-level\""));
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



unsigned ConsoleManager::GetCommandCount() const
{
	return LEPRA_ARRAY_COUNT(mCommandIdList);
}

const ConsoleManager::CommandPair& ConsoleManager::GetCommand(unsigned pIndex) const
{
	return (mCommandIdList[pIndex]);
}

int ConsoleManager::TranslateCommand(const str& pCommand) const
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

int ConsoleManager::OnCommand(const str& pCommand, const strutil::strvec& pParameterVector)
{
	int lResult = 0;
	CommandCommon lCommand = (CommandCommon)TranslateCommand(pCommand);
	switch (lCommand)
	{
		case COMMAND_ALIAS:
		{
			//bool lUsage = false;
			if (pParameterVector.size() == 2)
			{
				mAliasMap.insert(AliasMap::value_type(pParameterVector[0], pParameterVector[1]));
				GetConsoleCommandManager()->AddCommand(pParameterVector[0]);
			}
			else if (pParameterVector.size() == 1)
			{
				mAliasMap.erase(pParameterVector[0]);
				GetConsoleCommandManager()->RemoveCommand(pParameterVector[0]);
			}
			else
			{
				mLog.AInfo("List of aliases:");
				for (AliasMap::iterator x = mAliasMap.begin(); x != mAliasMap.end(); ++x)
				{
					mLog.Infof(_T("  %s -> %s"), x->first.c_str(), x->second.c_str());
				}
			}
			/*if (lUsage)
			{
				mLog.Warningf(_T("usage: %s <alias_name> [<command>]"), pCommand.c_str());
				mLog.AWarning("Adds a new alias, or removes one if <command is left out.");
				lResult = 1;
			}*/
		}
		break;
		case COMMAND_ALIAS_VALUE:
		{
			AliasMap::iterator x = mAliasMap.find(pCommand);
			if (x != mAliasMap.end())
			{
				lResult = ExecuteCommand(x->second + _T(" ") + strutil::Join(pParameterVector, _T(" ")));
			}
			else
			{
				mLog.Warningf(_T("Alias %s is removed."), pCommand.c_str());
				lResult = 1;
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
					const str lCMessage = strutil::Join(pParameterVector, _T(" "), 1);
					str lMessage;
					if (strutil::CStringToString(lCMessage, lMessage))
					{
						mLog.Print((LogLevel)lLogLevel, lMessage+_T('\n'));
					}
					else
					{
						mLog.AWarning("Invalid message C-string.");
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
				mLog.Warningf(_T("usage: %s <log_level> <text to log>"), pCommand.c_str());
				mLog.AWarning("Prints to <text to log> on all loggers at <log_level> (integer).");
				lResult = 1;
			}
		}
		break;
		case COMMAND_DEBUG_BREAK:
		{
#ifdef LEPRA_DEBUG
			mLog.Warningf(_T("Debug break."));
#ifdef LEPRA_MSVC
			::DebugBreak();
#else // Other compilers
			assert(false);
#endif // MSVC / others
#else // Release
			log_atrace("debug break is not available in non-debug builds.");
#endif // Debug / Release
		}
		break;
		case COMMAND_HELP:
		{
			if (GetConsoleLogger())
			{
				GetConsoleLogger()->OnLogRawMessage(_T("These are the available commands:\n"));
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
				const tchar* lListenerNameArray[] = { _T("console"), _T("i-console"), _T("file"), };
				for (size_t x = 0; x < LEPRA_ARRAY_COUNT(lListenerNameArray); ++x)
				{
					LogListener* lLogger = LogType::GetLog(LogType::SUB_ROOT)->GetListener(lListenerNameArray[x]);
					if (lLogger)
					{
						lLogger->SetLevelThreashold((LogLevel)lLogLevel);
						LogLevel lNewLogLevel = lLogger->GetLevelThreashold();
						if (lNewLogLevel != lLogLevel)
						{
							mLog.Infof(_T("Listener '%s' log level clamped to %i."), lListenerNameArray[x], lNewLogLevel);
						}
					}
				}
			}
			else
			{
				mLog.Warningf(_T("usage: %s <log level>"), pCommand.c_str());
				mLog.Warningf(_T("where log level is 0=trace, 1=debug... %i=only fatal errors"), LEVEL_TYPE_COUNT-1);
				lResult = 1;
			}
		}
		break;
		case COMMAND_SET_SUBSYSTEM_LOG_LEVEL:
		{
			int lLogLevel = 0;
			if (pParameterVector.size() == 2 && strutil::StringToInt(pParameterVector[1], lLogLevel))
			{
				Log* lLog = LogType::GetLog(pParameterVector[0]);
				if (lLog)
				{
					lLog->SetLevelThreashold((LogLevel)lLogLevel);
					LogLevel lNewLogLevel = lLog->GetLevelThreashold();
					mLog.Infof(_T("Log level for subsystem '%s' set to %i."), pParameterVector[0].c_str(), lNewLogLevel);
				}
				else
				{
					mLog.Infof(_T("Unknown log \"%s\"."), pParameterVector[0].c_str());
				}
			}
			else if (pParameterVector.size() == 1 && strutil::StringToInt(pParameterVector[0], lLogLevel))
			{
				const std::vector<Log*> lLogArray = LogType::GetLogs();
				std::vector<Log*>::const_iterator x = lLogArray.begin();
				LogLevel lNewLogLevel = LEVEL_LOWEST_TYPE;
				for (; x != lLogArray.end(); ++x)
				{
					(*x)->SetLevelThreashold((LogLevel)lLogLevel);
					lNewLogLevel = (*x)->GetLevelThreashold();
				}
				mLog.Infof(_T("All logs levels' set to %i."), lNewLogLevel);
			}
			else
			{
				mLog.Warningf(_T("usage: %s [<subsystem>] <log level>"), pCommand.c_str());
				mLog.Warningf(_T("where subsystem is Root, Network, Game, UI..."));
				mLog.Warningf(_T("where log level is 0=trace, 1=debug... %i=only fatal errors"), LEVEL_TYPE_COUNT-1);
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
				mLog.AError("Can not dump performance info, since game manager not present in this context.");
			}
		}
		break;
		case COMMAND_CLEAR_PERFORMANCE_INFO:
		{
			if (mGameManager)
			{
				log_performance(_T("Clearing performance data."));
				mGameManager->ClearPerformanceData();
			}
			else
			{
				mLog.AError("Can not clear performance info, since game manager not present in this context.");
			}
		}
		break;
		case COMMAND_SHOW_SYSTEM_INFO:
		{
			mLog.Infof(_T("Login:            %s"), SystemManager::GetLoginName().c_str());
			mLog.Infof(_T("Full name:        %s"), SystemManager::QueryFullUserName().c_str());
			mLog.Infof(_T("OS:               %s"), SystemManager::GetOsName().c_str());
			mLog.Infof(_T("CPU:              %s at %sHz"), SystemManager::GetCpuName().c_str(), Number::ConvertToPostfixNumber((double)SystemManager::QueryCpuFrequency(), 1).c_str());
			mLog.Infof(_T("CPU count:        %u physical, %u cores, %u logical"), SystemManager::GetPhysicalCpuCount(), SystemManager::GetCoreCount(), SystemManager::GetLogicalCpuCount());
			mLog.Infof(_T("Physical RAM:     %sB"), Number::ConvertToPostfixNumber((double)SystemManager::GetAmountRam(), 1).c_str());
			mLog.Infof(_T("Available RAM:    %sB"), Number::ConvertToPostfixNumber((double)SystemManager::GetAvailRam(), 1).c_str());
			mLog.Infof(_T("Sleep resolution: %f s"), SystemManager::GetSleepResolution());
		}
		break;
		case COMMAND_SHOW_GAME_INFO:
		{
			int lTargetFps;
			CURE_RTVAR_GET(lTargetFps, =, GetVariableScope(), RTVAR_PHYSICS_FPS, 2);
			mLog.Infof(_T("Target frame rate:     %i"), lTargetFps);
			if (mGameManager)
			{
				mLog.Infof(_T("Current frame rate:    %g"), 1/mGameManager->GetTimeManager()->GetRealNormalFrameTime());
				mLog.Infof(_T("Absolute time:	      %g"), mGameManager->GetTimeManager()->GetAbsoluteTime());
				mLog.Infof(_T("Current physics frame: %i"), mGameManager->GetTimeManager()->GetCurrentPhysicsFrame());
			}
		}
		break;
		case COMMAND_FORK:
		{
			if (pParameterVector.size() >= 1)
			{
				if (!ForkExecuteCommand(strutil::Join(pParameterVector, _T(" "))))
				{
					mLog.AError("Could not start fork!");
				}
			}
			else
			{
				mLog.Warningf(_T("usage: %s <command> [<arg> ...]"), pCommand.c_str());
				mLog.AWarning("Creates a new thread and executes <command> asynchronously.");
				lResult = 1;
			}
		}
		break;
		case COMMAND_EXECUTE_VARIABLE:
		{
			if (pParameterVector.size() == 1)
			{
				str lValue = GetVariableScope()->GetDefaultValue(Cure::RuntimeVariableScope::READ_ONLY, pParameterVector[0]);
				if (ExecuteCommand(lValue) == 0)
				{
					mLog.Infof(_T("Variable %s executed OK."), pParameterVector[0].c_str());
				}
				else if (!Thread::GetCurrentThread()->GetStopRequest())
				{
					mLog.Errorf(_T("Variable %s was NOT sucessfully executed."), pParameterVector[0].c_str());
					lResult = 1;
				}
			}
			else
			{
				mLog.Warningf(_T("usage: %s <variable_name>"), pCommand.c_str());
				mLog.AWarning("Executes contents of <variable_name>.");
				lResult = 1;
			}
		}
		break;
		case COMMAND_EXECUTE_FILE:
		{
#ifndef LEPRA_TOUCH	// Only default parameters are good enough for touch devices.
			size_t lFilenameIndex = 0;
			bool lIgnoreIfMissing = false;
			if (pParameterVector[0] == _T("-i"))
			{
				lIgnoreIfMissing = true;
				++lFilenameIndex;
			}
			if (lFilenameIndex < pParameterVector.size())
			{
				DiskFile lFile;
				if (lFile.Open(pParameterVector[lFilenameIndex], DiskFile::MODE_TEXT_READ))
				{
					wstr lLine;
					int lLineNumber = 1;
					Thread* lSelf = Thread::GetCurrentThread();
					for (; lResult == 0 && lFile.ReadLine(lLine) == IO_OK && !lSelf->GetStopRequest(); ++lLineNumber)
					{
						const str lConvertedLine = strutil::Encode(lLine);
						lResult = ExecuteCommand(lConvertedLine);
					}
					if (lResult == 0)
					{
						//mLog.Infof(_T("File %s executed OK."), pParameterVector[0].c_str());
					}
					else
					{
						if (!lSelf->GetStopRequest())
						{
							mLog.Errorf(_T("File %s was NOT sucessfully executed; error in line %i."), pParameterVector[0].c_str(), lLineNumber);
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
					mLog.Errorf(_T("File %s did not open for reading."), pParameterVector[0].c_str());
					lResult = 2;
				}
			}
			else
			{
				mLog.Warningf(_T("usage: %s [-i] <file_name>"), pCommand.c_str());
				mLog.AWarning("Executes contents of <file_name>.");
				mLog.AWarning("  -i   Ignores non-existing shell file.");
				lResult = 1;
			}
#endif // Not for touch devices
		}
		break;
		case COMMAND_LIST_ACTIVE_RESOURCES:
		{
			typedef Cure::ResourceManager::NameTypeList NameTypeList;
			NameTypeList lNameTypeList = mResourceManager->QueryResourceNames();
			mLog.Infof(_T("Currently %u active resources:"), lNameTypeList.size());
			lNameTypeList.sort();
			for (NameTypeList::iterator x = lNameTypeList.begin(); x != lNameTypeList.end(); ++x)
			{
				mLog.Info(_T("  ")+x->first+_T(" [")+x->second+_T("]."));
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
				mLog.Warningf(_T("usage: %s <secs>"), pCommand.c_str());
				mLog.AWarning("Sleeps <secs> where secs is a decimal number, e.g. 0.001.");
				lResult = 1;
			}
		}
		break;
		case COMMAND_WAIT_LOADED:
		{
			mLog.AInfo("Waiting for resource pump to complete loading...");
			// Start out by waiting for the game manager, if we don't have one yet.
			for (int x = 0; !mGameManager && x < 200; ++x)
			{
				Thread::Sleep(0.1);
			}
			if (mResourceManager->WaitLoading())
			{
				mLog.AInfo("Everything loaded.");
			}
			else
			{
				mLog.AError("Load not completed in time!");
				lResult = 1;
			}
		}
		break;
		case COMMAND_PUSH:
		{
			if (pParameterVector.size() >= 1)
			{
				PushYieldCommand(strutil::Join(pParameterVector, _T(" ")));
			}
			else
			{
				mLog.Warningf(_T("usage: %s command [arg [arg ...]]"), pCommand.c_str());
				mLog.AWarning("Pushes a command to be executed next time the shell is idle.");
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
					const str lCommand = strutil::Join(pParameterVector, _T(" "), 1);
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
				mLog.Warningf(_T("usage: %s <count> command [arg [arg ...]]"), pCommand.c_str());
				mLog.AWarning("Repeats a command <count> times.");
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
				if (pParameterVector[0] == _T("-i"))
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
						wstr lUserConfig;
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
							if (lStoreResult)
							{
								mLog.Infof(_T("Successfully wrote script %s to disk."), lFilename.c_str());
							}
							else
							{
								mLog.Errorf(_T("Script %s was NOT sucessfully saved; error writing to disk."), lFilename.c_str());
								lResult = 2;
							}
						}
						else
						{
							mLog.Errorf(_T("Script %s was NOT sucessfully saved; error opening for writing."), lFilename.c_str());
							lResult = 2;
						}
					}
					else
					{
						mLog.Infof(_T("Script %s not written; already exists."), lFilename.c_str());
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
				mLog.Warningf(_T("usage: %s [<-i>] {skip_scope_count} <file_name>"), pCommand.c_str());
				mLog.AWarning("Saves settings to <file_name>.");
				mLog.AWarning("  {skip_scope_count}  How many variable scopes to skip before writing. Not valid for all cmds.");
				mLog.AWarning("  -i                  Skips overwriting in case of already existing file.");
				lResult = 1;
			}
#endif // Not for touch devices
		}
		break;
		default:
		{
			if (!pCommand.empty() && pCommand[0] == '#')
			{
				tchar lLevel = pCommand[1];
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
						mLog.Infof(_T("%s %s"), lVariable.c_str(), lValue.c_str());
					}
					else
					{
						mLog.Warningf(_T("Variable %s not defined."), lVariable.c_str());
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
					if (lOk)
					{
						str lValue = lScope->GetUntypedDefaultValue(RtScope::READ_ONLY, lVariable);
						lValue = strutil::StringToCString(lValue);
						mLog.Infof(_T("%s <- %s"), lVariable.c_str(), lValue.c_str());
					}
					else
					{
						lResult = 1;
					}
				}
				else
				{
					mLog.Warningf(_T("usage: #<variable_name> [<value>]"), pCommand.c_str());
					mLog.AWarning("Prints variable_name or sets it to <value>.");
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



bool ConsoleManager::SaveApplicationConfigFile(File* pFile, const wstr& pUserConfig)
{
	pFile->WriteString(wstr(L"// Generated application shell script section.\n"));
	std::list<str> lVariableList = GetVariableScope()->GetVariableNameList(Cure::RuntimeVariableScope::SEARCH_EXPORTABLE, 0, 0);
	bool lSaved = SaveConfigFile(pFile, str(_T(".")), lVariableList, pUserConfig);
	return (lSaved);
}

Cure::ResourceManager* ConsoleManager::GetResourceManager() const
{
	return mResourceManager;
}



wstr ConsoleManager::LoadUserConfig(File* pFile)
{
	wstr lUserConfig;
	wstr lLine;
	for (bool lInUserConfig = false; pFile->ReadLine(lLine) == IO_OK; )
	{
		if (lInUserConfig)
		{
			lUserConfig += lLine + L"\n";
		}
		else if (lLine.find(USER_SECTION_MARK) != str::npos)
		{
			lInUserConfig = true;
		}
	}
	return (lUserConfig);
}

bool ConsoleManager::SaveSystemConfigFile(int pScopeSkipCount, File* pFile, const wstr& pUserConfig)
{
	pFile->WriteString<wchar_t>(L"// Generated system shell script section.\n");
	std::list<str> lVariableList = GetVariableScope()->GetVariableNameList(Cure::RuntimeVariableScope::SEARCH_EXPORTABLE, pScopeSkipCount);
	return (SaveConfigFile(pFile, str(_T("/")), lVariableList, pUserConfig));
}

bool ConsoleManager::SaveConfigFile(File* pFile, const str& pPrefix, std::list<str>& pVariableList, const wstr& pUserConfig)
{
	pFile->WriteString<wchar_t>(L"// Only change variable values below - use user section for all else.\n");
	pFile->WriteString<wchar_t>(L"set-stdout-log-level 4\n");

	pVariableList.sort();
	str lLastGroup;
	str lGroupDelimitors;
	CURE_RTVAR_GET(lGroupDelimitors, =, GetVariableScope(), RTVAR_CONSOLE_CHARACTERDELIMITORS, _T(" "));
	std::list<str>::const_iterator x = pVariableList.begin();
	for (; x != pVariableList.end(); ++x)
	{
		const str& lVariable = *x;
		const str lGroup = strutil::Split(lVariable, lGroupDelimitors, 1)[0];
		if (lLastGroup != lGroup)
		{
			pFile->WriteString<wchar_t>(L"\n");
			lLastGroup = lGroup;
		}
		str lValue = GetVariableScope()->GetUntypedDefaultValue(Cure::RuntimeVariableScope::READ_ONLY, lVariable);
		lValue = strutil::StringToCString(lValue);
		if (GetVariableScope()->GetUntypedType(lValue) == Cure::RuntimeVariable::DATATYPE_STRING)
		{
			lValue = _T('"')+lValue+_T('"');
		}
		str lDefaultValue = GetVariableScope()->GetUntypedDefaultValue(Cure::RuntimeVariableScope::READ_DEFAULT, lVariable);
		lDefaultValue = strutil::StringToCString(lDefaultValue);
		if (GetVariableScope()->GetUntypedType(lDefaultValue) == Cure::RuntimeVariable::DATATYPE_STRING)
		{
			lDefaultValue = _T('"')+lDefaultValue+_T('"');
		}
		lDefaultValue = (lValue != lDefaultValue)? _T("\t// Default is ")+lDefaultValue+_T(".\n") : _T("\n");
		pFile->WriteString(wstrutil::Encode(_T("#")+pPrefix+lVariable+_T(" ")+lValue+lDefaultValue));
	}
	pFile->WriteString<wchar_t>(L"\nset-stdout-log-level 1\n");
	pFile->WriteString<wchar_t>(L"\n" USER_SECTION_MARK L" -- User config. Everything but variable values will be overwritten above this section!\n");
	pFile->WriteString(pUserConfig);
	return (true);	// TODO: check if all writes went well.
}



LOG_CLASS_DEFINE(CONSOLE, ConsoleManager);



}
