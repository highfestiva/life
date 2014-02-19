
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../Include/ConsoleManager.h"
#include "../../Lepra/Include/SystemManager.h"
#include "../Include/Cure.h"
#include "../Include/RuntimeVariable.h"



namespace Cure
{



ConsoleManager::ConsoleManager(RuntimeVariableScope* pVariableScope, InteractiveConsoleLogListener* pConsoleLogger,
	ConsolePrompt* pConsolePrompt):
	mVariableScope(pVariableScope),
	mConsoleLogger(pConsoleLogger),
	mConsolePrompt(pConsolePrompt),
	mConsoleCommandManager(0),
	mConsoleThread(0),
	mHistorySilentUntilNextExecute(false)
{
}

ConsoleManager::~ConsoleManager()
{
	Join();
	mConsoleLogger = 0;
	delete mConsolePrompt;
	mConsolePrompt = 0;
	mVariableScope = 0;
	delete mConsoleCommandManager;
	mConsoleCommandManager = 0;
};

void ConsoleManager::SetConsoleLogger(InteractiveConsoleLogListener* pLogger)
{
	mConsoleLogger = pLogger;
}

bool ConsoleManager::Start()
{
	if (!mConsoleThread)
	{
		mConsoleThread = new MemberThread<ConsoleManager>("ConsoleThread");
	}
	return (mConsoleThread->Start(this, &ConsoleManager::ConsoleThreadEntry));
}

void ConsoleManager::Join()
{
	if (mConsoleThread && mConsoleThread->IsRunning())
	{
		mConsoleThread->RequestStop();
	}

	{
		// Join forks.
		ScopeLock lLock(&mLock);
		ForkList::iterator x = mForkList.begin();
		for (; x != mForkList.end(); ++x)
		{
			(*x)->RequestStop();
		}
	}
	if (mConsolePrompt)
	{
		mConsolePrompt->ReleaseWaitCharThread();
	}
	if (Thread::GetCurrentThread() != mConsoleThread && mConsoleThread)
	{
		if (mConsoleThread->IsRunning())
		{
			mConsoleThread->Join(0.5);
			mConsoleThread->Kill();
			delete mConsoleThread;
			mConsoleThread = 0;
		}
	}

	// Wait for forks.
	for (int x = 0; x < 1000; ++x)
	{
		{
			ScopeLock lLock(&mLock);
			if (mForkList.empty())
			{
				break;
			}
		}
		Thread::Sleep(0.01f);
	}
	{
		// Kill forks.
		ScopeLock lLock(&mLock);
		ForkList::iterator x = mForkList.begin();
		for (; x != mForkList.end(); ++x)
		{
			(*x)->Kill();
		}
	}
}



void ConsoleManager::PushYieldCommand(const str& pCommand)
{
	ScopeLock lLock(&mLock);
	mYieldCommandList.push_back(pCommand);
}

int ConsoleManager::ExecuteCommand(const str& pCommand)
{
	return (mConsoleCommandManager->Execute(pCommand, false));
}

int ConsoleManager::ExecuteYieldCommand()
{
	str lYieldCommand;
	{
		ScopeLock lLock(&mLock);
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

ConsoleCommandManager* ConsoleManager::GetConsoleCommandManager() const
{
	return (mConsoleCommandManager);
}



LogDecorator& ConsoleManager::GetLog() const
{
	return (mLog);
}



void ConsoleManager::AddFork(Thread* pThread)
{
	ScopeLock lLock(&mLock);
	mForkList.push_back(pThread);
}

void ConsoleManager::RemoveFork(Thread* pThread)
{
	ScopeLock lLock(&mLock);
	mForkList.remove(pThread);
}



InteractiveConsoleLogListener* ConsoleManager::GetConsoleLogger() const
{
	return (mConsoleLogger);
}

ConsolePrompt* ConsoleManager::GetConsolePrompt() const
{
	return (mConsolePrompt);
}

RuntimeVariableScope* ConsoleManager::GetVariableScope() const
{
	return (mVariableScope);
}



bool ConsoleManager::ForkExecuteCommand(const str& pCommand)
{
	class ForkThread: public Thread
	{
	public:
		ForkThread(ConsoleManager* pConsole, const str& pCommand):
			Thread("ConsoleFork"),
			mConsole(pConsole),
			mCommand(pCommand)
		{
		}
	private:
		void Run()
		{
			log_volatile(mConsole->GetLog().Debug(_T("ForkThread: started.")));
			if (mConsole->ExecuteCommand(mCommand) != 0)
			{
				mConsole->GetLog().AError("ForkThread: execution resulted in an error.");
			}
			log_volatile(mConsole->GetLog().Debug(_T("ForkThread: ended.")));
			mConsole->RemoveFork(this);
		}
		ConsoleManager* mConsole;
		str mCommand;
		void operator=(const ForkThread&) {};
	};
	if (SystemManager::GetQuitRequest())
	{
		return false;
	}
	ForkThread* lExecutor = new ForkThread(this, pCommand);
	AddFork(lExecutor);
	lExecutor->RequestSelfDestruct();
	return (lExecutor->Start());
}



void ConsoleManager::InitCommands()
{
	mConsoleCommandManager = new ConsoleCommandManager();
	mConsoleCommandManager->AddExecutor(new ConsoleExecutor<ConsoleManager>(this, &ConsoleManager::OnCommandLocal, &ConsoleManager::OnCommandError));
	AddCommands();
}

std::list<str> ConsoleManager::GetCommandList() const
{
	str lDummy;
	std::list<str> lCommandList = mConsoleCommandManager->GetCommandCompletionList(_T(""), lDummy);
	return (lCommandList);
}

int ConsoleManager::TranslateCommand(const str& pCommand) const
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

void ConsoleManager::PrintCommandList(const std::list<str>& pCommandList)
{
	std::list<str>::const_iterator x = pCommandList.begin();
	int lSpacing;
	CURE_RTVAR_GET(lSpacing, =, mVariableScope, RTVAR_CONSOLE_COLUMNSPACING, 2);
	size_t lLongestCommand = 10;
	for (; x != pCommandList.end(); ++x)
	{
		
		const size_t lLength = x->length()+lSpacing;
		lLongestCommand = (lLength > lLongestCommand)? lLength : lLongestCommand;
	}
	size_t lIndent = 0;
	str lFormat = strutil::Format(_T("%%-%is"), lLongestCommand);
	for (x = pCommandList.begin(); x != pCommandList.end(); ++x)
	{
		str lCommand = strutil::Format(lFormat.c_str(), x->c_str());
		mConsoleLogger->OnLogRawMessage(lCommand);
		lIndent += lCommand.length();
		int lConsoleWidth;
		CURE_RTVAR_GET(lConsoleWidth, =, GetVariableScope(), RTVAR_CONSOLE_CHARACTERWIDTH, 80);
		if ((int)(lIndent+lLongestCommand) >= lConsoleWidth)
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
	const str lPrompt(_T(">"));
	str lInputText(_T(""));
	size_t lEditIndex = 0;
	while (!SystemManager::GetQuitRequest() && mConsoleThread && !mConsoleThread->GetStopRequest())
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

		str lWordDelimitors;
		int lKeyCompletion;
		int lKeyEnter;
		int lKeySilent;
		int lKeyBackspace;
		int lKeyDelete;
		int lKeyCtrlLeft;
		int lKeyCtrlRight;
		int lKeyHome;
		int lKeyEnd;
		int lKeyUp;
		int lKeyDown;
		int lKeyLeft;
		int lKeyRight;
		int lKeyEsc;
		int lKeyPgUp;
		int lKeyPgDn;
		CURE_RTVAR_GET(lWordDelimitors, =, mVariableScope, RTVAR_CONSOLE_CHARACTERDELIMITORS, _T(" "));
		CURE_RTVAR_GET(lKeyCompletion, =, mVariableScope, RTVAR_CONSOLE_KEY_COMPLETION, (int)'\t');
		CURE_RTVAR_GET(lKeyEnter, =, mVariableScope, RTVAR_CONSOLE_KEY_ENTER, (int)'\r');
		CURE_RTVAR_GET(lKeySilent, =, mVariableScope, RTVAR_CONSOLE_KEY_SILENT, (int)'\v');
		CURE_RTVAR_GET(lKeyBackspace, =, mVariableScope, RTVAR_CONSOLE_KEY_BACKSPACE, (int)'\b');
		CURE_RTVAR_GET(lKeyDelete, =, mVariableScope, RTVAR_CONSOLE_KEY_DELETE, ConsolePrompt::CON_KEY_DELETE);
		CURE_RTVAR_GET(lKeyCtrlLeft, =, mVariableScope, RTVAR_CONSOLE_KEY_CTRLLEFT, ConsolePrompt::CON_KEY_CTRL_LEFT);
		CURE_RTVAR_GET(lKeyCtrlRight, =, mVariableScope, RTVAR_CONSOLE_KEY_CTRLRIGHT, ConsolePrompt::CON_KEY_CTRL_RIGHT);
		CURE_RTVAR_GET(lKeyHome, =, mVariableScope, RTVAR_CONSOLE_KEY_HOME, ConsolePrompt::CON_KEY_HOME);
		CURE_RTVAR_GET(lKeyEnd, =, mVariableScope, RTVAR_CONSOLE_KEY_END, ConsolePrompt::CON_KEY_END);
		CURE_RTVAR_GET(lKeyUp, =, mVariableScope, RTVAR_CONSOLE_KEY_UP, ConsolePrompt::CON_KEY_UP);
		CURE_RTVAR_GET(lKeyDown, =, mVariableScope, RTVAR_CONSOLE_KEY_DOWN, ConsolePrompt::CON_KEY_DOWN);
		CURE_RTVAR_GET(lKeyLeft, =, mVariableScope, RTVAR_CONSOLE_KEY_LEFT, ConsolePrompt::CON_KEY_LEFT);
		CURE_RTVAR_GET(lKeyRight, =, mVariableScope, RTVAR_CONSOLE_KEY_RIGHT, ConsolePrompt::CON_KEY_RIGHT);
		CURE_RTVAR_GET(lKeyEsc, =, mVariableScope, RTVAR_CONSOLE_KEY_ESC, ConsolePrompt::CON_KEY_ESCAPE);
		CURE_RTVAR_GET(lKeyPgUp, =, mVariableScope, RTVAR_CONSOLE_KEY_PAGEUP, ConsolePrompt::CON_KEY_PAGE_UP);
		CURE_RTVAR_GET(lKeyPgDn, =, mVariableScope, RTVAR_CONSOLE_KEY_PAGEDOWN, ConsolePrompt::CON_KEY_PAGE_DOWN);

		if (c == lKeyCompletion)
		{
			str lBestCompletionString;
			std::list<str> lCompletions =
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
		else if (c == lKeyEnter)
		{
			mConsoleLogger->OnLogRawMessage(_T("\r")+lPrompt+lInputText+_T("\n"));
			mConsoleCommandManager->Execute(lInputText, !mHistorySilentUntilNextExecute);
			mHistorySilentUntilNextExecute = false;
			lInputText = _T("");
			lEditIndex = 0;
		}
		else if (c == lKeySilent)
		{
			mHistorySilentUntilNextExecute = true;
		}
		else if (c == lKeyBackspace)
		{
			if (lEditIndex > 0)
			{
				// Reset index in console history.
				mConsoleCommandManager->SetCurrentHistoryIndex((int)mConsoleCommandManager->GetHistoryCount());

				--lEditIndex;
				mConsolePrompt->Backspace(1);
				mConsolePrompt->EraseText(lInputText.length()-lEditIndex);
				str lNewInputText = lInputText.substr(0, lEditIndex) + lInputText.substr(lEditIndex+1);
				lInputText = lNewInputText;
			}
		}
		else if (c == lKeyDelete)
		{
			if (lEditIndex < lInputText.length())
			{
				// Reset index in console history.
				mConsoleCommandManager->SetCurrentHistoryIndex((int)mConsoleCommandManager->GetHistoryCount());

				mConsolePrompt->EraseText(lInputText.length()-lEditIndex);
				str lNewInputText = lInputText.substr(0, lEditIndex) + lInputText.substr(lEditIndex+1);
				lInputText = lNewInputText;
			}
		}
		else if (c == lKeyCtrlLeft)
		{
			lEditIndex = strutil::FindPreviousWord(lInputText, lWordDelimitors, lEditIndex);
		}
		else if (c == lKeyCtrlRight)
		{
			lEditIndex = strutil::FindNextWord(lInputText, lWordDelimitors, lEditIndex);
		}
		else if (c == lKeyHome)
		{
			lEditIndex = 0;
		}
		else if (c == lKeyEnd)
		{
			lEditIndex = lInputText.length();
		}
		else if (c == lKeyUp || c == lKeyDown)
		{
			// Erase current text.
			mConsolePrompt->Backspace(lEditIndex);
			mConsolePrompt->EraseText(lInputText.length());
			int lDesiredHistoryIndex = mConsoleCommandManager->GetCurrentHistoryIndex();
			if (c == lKeyUp)
			{
				// History->previous.
				--lDesiredHistoryIndex;
			}
			else if (c == lKeyDown)
			{
				// History->next.
				++lDesiredHistoryIndex;
			}
			mConsoleCommandManager->SetCurrentHistoryIndex(lDesiredHistoryIndex);
			lDesiredHistoryIndex = mConsoleCommandManager->GetCurrentHistoryIndex();
			lInputText = mConsoleCommandManager->GetHistory(lDesiredHistoryIndex);
			lEditIndex = lInputText.length();
		}
		else if (c == lKeyLeft)
		{
			if (lEditIndex > 0)
			{
				--lEditIndex;
			}
		}
		else if (c == lKeyRight)
		{
			if (lEditIndex < lInputText.length())
			{
				++lEditIndex;
			}
		}
		else if (c == lKeyEsc)
		{
			mConsolePrompt->Backspace(lEditIndex);
			mConsolePrompt->EraseText(lInputText.length());
			lInputText = _T("");
			lEditIndex = 0;
		}
		else if (c == lKeyPgUp)
		{
			mConsoleLogger->StepPage(-1);
		}
		else if (c == lKeyPgDn)
		{
			mConsoleLogger->StepPage(+1);
		}
		else if (c < 0)
		{
		}
		else
		{
			tchar lChars[2] = _T("?");
			lChars[0] = (tchar)c;
			lInputText.insert(lEditIndex, lChars);
			++lEditIndex;
		}
	}
}

int ConsoleManager::OnCommandLocal(const str& pCommand, const strutil::strvec& pParameterVector)
{
	return (OnCommand(pCommand, pParameterVector));
}

void ConsoleManager::OnCommandError(const str& pCommand, const strutil::strvec&, int pResult)
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
