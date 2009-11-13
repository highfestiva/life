
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games



#include "../../Lepra/Include/SystemManager.h"
#include "../Include/ConsoleManager.h"
#include "../Include/Cure.h"
#include "../Include/RuntimeVariable.h"



namespace Cure
{



ConsoleManager::ConsoleManager(RuntimeVariableScope* pVariableScope, Lepra::InteractiveConsoleLogListener* pConsoleLogger,
	Lepra::ConsolePrompt* pConsolePrompt):
	mVariableScope(pVariableScope),
	mConsoleLogger(pConsoleLogger),
	mConsolePrompt(pConsolePrompt),
	mConsoleCommandManager(0),
	mConsoleThread(_T("ConsoleThread"))
{
}

ConsoleManager::~ConsoleManager()
{
	Join();
	mConsoleLogger = 0;
	delete (mConsolePrompt);
	mConsolePrompt = 0;
	mVariableScope = 0;
};

void ConsoleManager::SetConsoleLogger(Lepra::InteractiveConsoleLogListener* pLogger)
{
	mConsoleLogger = pLogger;
}

bool ConsoleManager::Start()
{
	return (mConsoleThread.Start(this, &ConsoleManager::ConsoleThreadEntry));
}

void ConsoleManager::Join()
{
	mConsoleThread.RequestStop();
	if (mConsolePrompt)
	{
		mConsolePrompt->ReleaseWaitCharThread();
	}
	if (Lepra::Thread::GetCurrentThread() != &mConsoleThread)
	{
		mConsoleThread.Join(0.5);
		mConsoleThread.Kill();
	}
}



void ConsoleManager::PushYieldCommand(const Lepra::String& pCommand)
{
	Lepra::ScopeLock lLock(&mLock);
	mYieldCommandList.push_back(pCommand);
}

int ConsoleManager::ExecuteCommand(const Lepra::String& pCommand)
{
	return (mConsoleCommandManager->Execute(pCommand, false));
}

int ConsoleManager::ExecuteYieldCommand()
{
	Lepra::String lYieldCommand;
	{
		Lepra::ScopeLock lLock(&mLock);
		if (!mYieldCommandList.empty())
		{
			lYieldCommand = mYieldCommandList.front();
			mYieldCommandList.pop_front();
		}
	}
	if (!lYieldCommand.empty())
	{
		return (mConsoleCommandManager->Execute(lYieldCommand, false));
	}
	return (-1);
}

Lepra::ConsoleCommandManager* ConsoleManager::GetConsoleCommandManager() const
{
	return (mConsoleCommandManager);
}



bool ConsoleManager::ForkExecuteCommand(const Lepra::String& pCommand)
{
	class ForkThread: public Lepra::Thread
	{
	public:
		ForkThread(ConsoleManager* pConsole, const Lepra::String& pCommand):
			Lepra::Thread(_T("ConsoleFork")),
			mConsole(pConsole),
			mCommand(pCommand)
		{
		}
	private:
		void Run()
		{
			if (mConsole->GetConsoleCommandManager()->Execute(mCommand, false) != 0)
			{
				Lepra::LogType::GetLog(Lepra::LogType::SUB_CONSOLE)->Print(_T("ForkThread"), _T("Fork execution resulted in an error."), Lepra::Log::LEVEL_ERROR);
			}
		}
		ConsoleManager* mConsole;
		Lepra::String mCommand;
		void operator=(const ForkThread&) {};
	};
	ForkThread* lExecutor = new ForkThread(this, pCommand);
	lExecutor->RequestSelfDestruct();
	return (lExecutor->Start());
}



void ConsoleManager::Init()
{
	mConsoleCommandManager = new Lepra::ConsoleCommandManager();
	mConsoleCommandManager->AddExecutor(new Lepra::ConsoleExecutor<ConsoleManager>(this, &ConsoleManager::OnCommandLocal, &ConsoleManager::OnCommandError));
	AddCommands();
}

std::list<Lepra::String> ConsoleManager::GetCommandList() const
{
	Lepra::String lDummy;
	std::list<Lepra::String> lCommandList = mConsoleCommandManager->GetCommandCompletionList(_T(""), lDummy);
	return (lCommandList);
}

int ConsoleManager::TranslateCommand(const Lepra::String& pCommand) const
{
	int lCommand = -1;
	for (unsigned x = 0; x < GetCommandCount(); ++x)
	{
		if (pCommand == GetCommand(x).mCommandName)
		{
			lCommand = GetCommand(x).mCommandId;
			break;
		}
	}
	return (lCommand);
}

void ConsoleManager::PrintCommandList(const std::list<Lepra::String>& pCommandList)
{
	std::list<Lepra::String>::const_iterator x = pCommandList.begin();
	const int lSpacing = CURE_RTVAR_GET(mVariableScope, RTVAR_CONSOLE_COLUMNSPACING, 2);
	size_t lLongestCommand = 10;
	for (; x != pCommandList.end(); ++x)
	{
		
		const size_t lLength = x->length()+lSpacing;
		lLongestCommand = (lLength > lLongestCommand)? lLength : lLongestCommand;
	}
	size_t lIndent = 0;
	Lepra::String lFormat = Lepra::StringUtility::Format(_T("%%-%is"), lLongestCommand);
	for (x = pCommandList.begin(); x != pCommandList.end(); ++x)
	{
		Lepra::String lCommand = Lepra::StringUtility::Format(lFormat.c_str(), x->c_str());
		mConsoleLogger->OnLogRawMessage(lCommand);
		lIndent += lCommand.length();
		const size_t lConsoleWidth = CURE_RTVAR_GET(GetVariableScope(), RTVAR_CONSOLE_CHARACTERWIDTH, 80);
		if (lIndent+lLongestCommand >= lConsoleWidth)
		{
			mConsoleLogger->OnLogRawMessage(_T("\n"));
			lIndent = 0;
		}
	}
	if (!pCommandList.empty() && lIndent != 0)
	{
		mConsoleLogger->OnLogRawMessage(_T("\n"));
	}
}

Lepra::InteractiveConsoleLogListener* ConsoleManager::GetConsoleLogger() const
{
	return (mConsoleLogger);
}

Lepra::ConsolePrompt* ConsoleManager::GetConsolePrompt() const
{
	return (mConsolePrompt);
}



RuntimeVariableScope* ConsoleManager::GetVariableScope() const
{
	return (mVariableScope);
}



void ConsoleManager::AddCommands()
{
	for (unsigned x = 0; x < GetCommandCount(); ++x)
	{
		mConsoleCommandManager->AddCommand(GetCommand(x).mCommandName);
	}
}

void ConsoleManager::ConsoleThreadEntry()
{
	// Main console IO loop.
	const Lepra::String lPrompt(_T(">"));
	Lepra::String lInputText(_T(""));
	size_t lEditIndex = 0;
	while (!Lepra::SystemManager::GetQuitRequest() && !mConsoleThread.GetStopRequest())
	{
		// Execute any pending yield command.
		if (lInputText.empty())
		{
			if (ExecuteYieldCommand() >= 0)
			{
				continue;
			}
		}

		mConsolePrompt->PrintPrompt(lPrompt, lInputText, lEditIndex);

		mConsoleLogger->SetAutoPrompt(lPrompt+lInputText);
		int c = mConsolePrompt->WaitChar();
		mConsoleLogger->SetAutoPrompt(_T(""));

		const Lepra::String lWordDelimitors(CURE_RTVAR_GET(GetVariableScope(), RTVAR_CONSOLE_CHARACTERDELIMITORS, _T(" ")));

		if (c == CURE_RTVAR_GET(GetVariableScope(), RTVAR_CONSOLE_KEY_COMPLETION, (int)'\t'))
		{
			Lepra::String lBestCompletionString;
			std::list<Lepra::String> lCompletions =
				mConsoleCommandManager->GetCommandCompletionList(lInputText, lBestCompletionString);
			// Print command completion list.
			if (lCompletions.size() > 1)
			{
				mConsoleLogger->OnLogRawMessage(_T("\n"));
				PrintCommandList(lCompletions);
			}
			lInputText = lBestCompletionString;
			lEditIndex = lInputText.length();
			if (lCompletions.size() == 1)
			{
				lInputText += _T(' ');
				++lEditIndex;
			}
		}
		else if (c == CURE_RTVAR_GET(GetVariableScope(), RTVAR_CONSOLE_KEY_ENTER, (int)'\r'))
		{
			mConsoleLogger->OnLogRawMessage(_T("\r")+lPrompt+lInputText+_T("\n"));
			mConsoleCommandManager->Execute(lInputText, true);
			lInputText = _T("");
			lEditIndex = 0;
		}
		else if (c == CURE_RTVAR_GET(GetVariableScope(), RTVAR_CONSOLE_KEY_BACKSPACE, (int)'\b'))
		{
			if (lEditIndex > 0)
			{
				// Reset index in console history.
				mConsoleCommandManager->SetCurrentHistoryIndex((int)mConsoleCommandManager->GetHistoryCount());

				--lEditIndex;
				mConsolePrompt->Backspace(1);
				mConsolePrompt->EraseText(lInputText.length()-lEditIndex);
				Lepra::String lNewInputText = lInputText.substr(0, lEditIndex) + lInputText.substr(lEditIndex+1);
				lInputText = lNewInputText;
			}
		}
		else if (c == CURE_RTVAR_GET(GetVariableScope(), RTVAR_CONSOLE_KEY_DELETE, Lepra::ConsolePrompt::CON_KEY_DELETE))
		{
			if (lEditIndex < lInputText.length())
			{
				// Reset index in console history.
				mConsoleCommandManager->SetCurrentHistoryIndex((int)mConsoleCommandManager->GetHistoryCount());

				mConsolePrompt->EraseText(lInputText.length()-lEditIndex);
				Lepra::String lNewInputText = lInputText.substr(0, lEditIndex) + lInputText.substr(lEditIndex+1);
				lInputText = lNewInputText;
			}
		}
		else if (c == CURE_RTVAR_GET(GetVariableScope(), RTVAR_CONSOLE_KEY_CTRLLEFT, Lepra::ConsolePrompt::CON_KEY_CTRL_LEFT))
		{
			lEditIndex = Lepra::StringUtility::FindPreviousWord(lInputText, lWordDelimitors, lEditIndex);
		}
		else if (c == CURE_RTVAR_GET(GetVariableScope(), RTVAR_CONSOLE_KEY_CTRLRIGHT, Lepra::ConsolePrompt::CON_KEY_CTRL_RIGHT))
		{
			lEditIndex = Lepra::StringUtility::FindNextWord(lInputText, lWordDelimitors, lEditIndex);
		}
		else if (c == CURE_RTVAR_GET(GetVariableScope(), RTVAR_CONSOLE_KEY_HOME, Lepra::ConsolePrompt::CON_KEY_HOME))
		{
			lEditIndex = 0;
		}
		else if (c == CURE_RTVAR_GET(GetVariableScope(), RTVAR_CONSOLE_KEY_END, Lepra::ConsolePrompt::CON_KEY_END))
		{
			lEditIndex = lInputText.length();
		}
		else if (c == CURE_RTVAR_GET(GetVariableScope(), RTVAR_CONSOLE_KEY_UP, Lepra::ConsolePrompt::CON_KEY_UP) ||
			c == CURE_RTVAR_GET(GetVariableScope(), RTVAR_CONSOLE_KEY_DOWN, Lepra::ConsolePrompt::CON_KEY_DOWN))
		{
			// Erase current text.
			mConsolePrompt->Backspace(lEditIndex);
			mConsolePrompt->EraseText(lInputText.length());
			int lDesiredHistoryIndex = mConsoleCommandManager->GetCurrentHistoryIndex();
			if (c == Lepra::ConsolePrompt::CON_KEY_UP)
			{
				// History->previous.
				--lDesiredHistoryIndex;
			}
			else if (c == Lepra::ConsolePrompt::CON_KEY_DOWN)
			{
				// History->next.
				++lDesiredHistoryIndex;
			}
			mConsoleCommandManager->SetCurrentHistoryIndex(lDesiredHistoryIndex);
			lDesiredHistoryIndex = mConsoleCommandManager->GetCurrentHistoryIndex();
			lInputText = mConsoleCommandManager->GetHistory(lDesiredHistoryIndex);
			lEditIndex = lInputText.length();
		}
		else if (c == CURE_RTVAR_GET(GetVariableScope(), RTVAR_CONSOLE_KEY_LEFT, Lepra::ConsolePrompt::CON_KEY_LEFT))
		{
			if (lEditIndex > 0)
			{
				--lEditIndex;
			}
		}
		else if (c == CURE_RTVAR_GET(GetVariableScope(), RTVAR_CONSOLE_KEY_RIGHT, Lepra::ConsolePrompt::CON_KEY_RIGHT))
		{
			if (lEditIndex < lInputText.length())
			{
				++lEditIndex;
			}
		}
		else if (c == CURE_RTVAR_GET(GetVariableScope(), RTVAR_CONSOLE_KEY_ESC, Lepra::ConsolePrompt::CON_KEY_ESCAPE))
		{
			mConsolePrompt->Backspace(lEditIndex);
			mConsolePrompt->EraseText(lInputText.length());
			lInputText = _T("");
			lEditIndex = 0;
		}
		else if (c == CURE_RTVAR_GET(GetVariableScope(), RTVAR_CONSOLE_KEY_PAGEUP, Lepra::ConsolePrompt::CON_KEY_PAGE_UP))
		{
			mConsoleLogger->StepPage(-1);
		}
		else if (c == CURE_RTVAR_GET(GetVariableScope(), RTVAR_CONSOLE_KEY_PAGEDOWN, Lepra::ConsolePrompt::CON_KEY_PAGE_DOWN))
		{
			mConsoleLogger->StepPage(+1);
		}
		else if (c < 0)
		{
		}
		else
		{
			Lepra::tchar lChars[2] = _T("?");
			lChars[0] = (Lepra::tchar)c;
			lInputText.insert(lEditIndex, lChars);
			++lEditIndex;
		}
	}
}

int ConsoleManager::OnCommandLocal(const Lepra::String& pCommand, const Lepra::StringUtility::StringVector& pParameterVector)
{
	return (OnCommand(pCommand, pParameterVector));
}

void ConsoleManager::OnCommandError(const Lepra::String& pCommand, const Lepra::StringUtility::StringVector&, int pResult)
{
	if (pResult < 0)
	{
		mLog.Warningf(_T("Unknown command: \"%s\". Type 'help' for more info."), pCommand.c_str());
	}
	else
	{
		mLog.Warningf(_T("Command error: \"%s\" returned error code %i."), pCommand.c_str(), pResult);
	}
}



LOG_CLASS_DEFINE(CONSOLE, ConsoleManager);



}
