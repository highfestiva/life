
// Author: Jonas Bystr�m
// Copyright (c) 2002-2008, Righteous Games



#include "../Include/ConsoleCommandManager.h"
#include "../Include/SystemManager.h"
#ifdef LEPRA_WINDOWS
#include <conio.h>
#elif defined(LEPRA_POSIX)
#include <termios.h>
#endif // Windows / POSIX



namespace Lepra
{



#if defined(LEPRA_POSIX)
int Getc(float pTimeout)
{
	const int fn = fileno(stdin);
	fd_set lCharSet;
	FD_ZERO(&lCharSet);
	FD_SET(fn, &lCharSet);
	timeval* lTime = 0;
	timeval lCharTimeout;
	if (pTimeout >= 0)
	{
		lTime = &lCharTimeout;
		lCharTimeout.tv_sec = (int)pTimeout;
		pTimeout -= lCharTimeout.tv_sec;
		lCharTimeout.tv_usec = (int)(pTimeout*1000*1000);
	}
	int lResult = -1;
	if (::select(fn+1, &lCharSet, 0, 0, lTime) > 0)
	{
		unsigned char c;
		if (::read(fn, &c, 1) == 1)
		{
			lResult = c;
		}
	}
	return (lResult);
}
#endif // Posix.



ConsoleCommandManager::ConsoleCommandManager():
	mCurrentHistoryIndex(0)
{
	AddCompleter(this);
}

ConsoleCommandManager::~ConsoleCommandManager()
{
	RemoveCompleter(this);
	CommandCompleterSet::iterator x = mCommandCompleterList.begin();
	for (; x != mCommandCompleterList.end(); ++x)
	{
		CommandCompleter* lCompleter = *x;
		delete (lCompleter);
	}
	mCommandCompleterList.clear();
}



void ConsoleCommandManager::SetComment(const String& pComment)
{
	mComment = pComment;
}

bool ConsoleCommandManager::AddCommand(const String& pCommand)
{
	return mCommandSet.insert(pCommand).second;
}

int ConsoleCommandManager::Execute(const String& pCommand, bool pAppendToHistory)
{
	int lExecutionResult = 0;
	String lCommandDelimitors(_T(" \t\v\r\n"));

	String lInCommand(pCommand);
	if (!mComment.empty())
	{
		// Drop comments (ignore if inside string).
		lInCommand = lInCommand.substr(0, lInCommand.find(mComment, 0));
	}

	StringUtility::StringVector lCommandList = StringUtility::BlockSplit(lInCommand, _T(";"), true);

	if (lCommandList.size() == 0)
	{
		// We will execute nothing.
	}
	else if (pAppendToHistory)
	{
		AppendHistory(pCommand);
	}

	for (size_t lCommandIndex = 0; lExecutionResult == 0 && lCommandIndex < lCommandList.size(); ++lCommandIndex)
	{
		const String& lTempCommand = lCommandList[lCommandIndex];
		String lCommand = StringUtility::StripLeft(lTempCommand, lCommandDelimitors);
		StringUtility::StringVector lCommandTokenList = StringUtility::BlockSplit(lCommand, lCommandDelimitors, 1, false);
		if (lCommandTokenList.size() > 0)
		{
			lCommand = lCommandTokenList[0];
			StringUtility::StringVector lParameterTokenList;
			if (lCommandTokenList.size() > 1)
			{
				String lParameters = lCommandTokenList[1];
				lParameters = StringUtility::StripLeft(lParameters, lCommandDelimitors);
				lParameterTokenList = StringUtility::BlockSplit(lParameters, lCommandDelimitors, false);
			}

			lExecutionResult = OnExecute(lCommand, lParameterTokenList);
			if (lExecutionResult != 0)
			{
				OnExecutionError(lCommand, lParameterTokenList, lExecutionResult);
			}
		}
	}
	return (lExecutionResult);
}

void ConsoleCommandManager::AddCompleter(CommandCompleter* pCompleter)
{
	mCommandCompleterList.insert(pCompleter);
}

void ConsoleCommandManager::RemoveCompleter(CommandCompleter* pCompleter)
{
	mCommandCompleterList.erase(pCompleter);
}

ConsoleCommandManager::CommandList ConsoleCommandManager::GetCommandCompletionList(
	const String& pPartialCommand, String& pCompleted) const
{
	// Pick partial command.
	const String lCommandDelimitors(_T(" \t\v\r\n"));
	const String lPartialCommand = StringUtility::StripLeft(pPartialCommand, lCommandDelimitors);
	pCompleted = lPartialCommand;

	// Complete partial command.
	CommandList lCompletionList;
	CommandCompleterSet::const_iterator x = mCommandCompleterList.begin();
	for (; x != mCommandCompleterList.end(); ++x)
	{
		CommandList lTempCompletions = (*x)->CompleteCommand(lPartialCommand);
		lCompletionList.splice(lCompletionList.end(), lTempCompletions);
	}
	lCompletionList.sort();

	// Find lest common denominator (=best completion).
	size_t lCompletionLetterCount = 1000;
	CommandList::const_iterator y = lCompletionList.begin();
	for (; y != lCompletionList.end(); ++y)
	{
		const String& lCompletion = *y;
		// Trim completed string, depending on completion matching.
		if (lCompletion.length() < lCompletionLetterCount)
		{
			lCompletionLetterCount = lCompletion.length();
		}
		size_t y;
		for (y = lPartialCommand.length(); y < lCompletionLetterCount; ++y)
		{
			if (y >= pCompleted.length())
			{
				// Add more text (more completed).
				pCompleted += lCompletion[y];
			}
			else if (lCompletion[y] != pCompleted[y])
			{
				// Cut completion, since this command is shorter.
				lCompletionLetterCount = y;
			}
		}
		pCompleted.resize(lCompletionLetterCount);
	}
	return (lCompletionList);
}

std::list<String> ConsoleCommandManager::CompleteCommand(const String& pPartialCommand) const
{
	std::list<String> lCompletionList;
	CommandSet::const_iterator x = mCommandSet.begin();
	for (; x != mCommandSet.end(); ++x)
	{
		const String& lCompletion = *x;
		if (lCompletion.substr(0, pPartialCommand.length()) == pPartialCommand)
		{
			// Add to list.
			lCompletionList.push_back(lCompletion);
		}
	}
	return (lCompletionList);
}

unsigned ConsoleCommandManager::GetHistoryCount() const
{
	return ((unsigned)mHistoryVector.size());
}

void ConsoleCommandManager::SetCurrentHistoryIndex(int pIndex)
{
	if (pIndex < 0)
	{
		pIndex = 0;
	}
	else if (pIndex > (int)GetHistoryCount())
	{
		pIndex = GetHistoryCount();
	}
	mCurrentHistoryIndex = pIndex;
}

int ConsoleCommandManager::GetCurrentHistoryIndex() const
{
	return (mCurrentHistoryIndex);
}

String ConsoleCommandManager::GetHistory(int pIndex) const
{
	String lHistory;
	if (pIndex >= 0 && pIndex < (int)mHistoryVector.size())
	{
		lHistory = mHistoryVector[pIndex];
	}
	return (lHistory);
}

void ConsoleCommandManager::AppendHistory(const String& pCommand)
{
	// Only append if this isn't exactly the same as the last command.
	if (mHistoryVector.size() == 0 || (pCommand != mHistoryVector.back()))
	{
		mHistoryVector.push_back(pCommand);
	}
	mCurrentHistoryIndex = GetHistoryCount();
}



ConsolePrompt::ConsolePrompt()
{
}

ConsolePrompt::~ConsolePrompt()
{
}



StdioConsolePrompt::StdioConsolePrompt()
{
}

StdioConsolePrompt::~StdioConsolePrompt()
{
}

void StdioConsolePrompt::SetFocus(bool)
{
	// Nothing to do, always focused.
}

int StdioConsolePrompt::WaitChar()
{
	if (SystemManager::GetQuitRequest())
	{
		return (-1);	// TRICKY: RAII simplifies platform-dependant code.
	}

#ifdef LEPRA_WINDOWS
	int c = _getch_nolock();
	if (c == 27)
	{
		c = CON_KEY_ESCAPE;
	}
	else if (c == 224)
	{
		c = ::_getch_nolock();
		switch (c)
		{
			case 72:	c = CON_KEY_UP;		break;
			case 80:	c = CON_KEY_DOWN;	break;
			case 75:	c = CON_KEY_LEFT;	break;
			case 77:	c = CON_KEY_RIGHT;	break;
			case 73:	c = CON_KEY_PAGE_DOWN;	break;
			case 81:	c = CON_KEY_PAGE_UP;	break;
			case 71:	c = CON_KEY_HOME;	break;
			case 79:	c = CON_KEY_END;	break;
			case 82:	c = CON_KEY_INSERT;	break;
			case 83:	c = CON_KEY_DELETE;	break;
			case 133:	c = CON_KEY_F11;	break;
			case 134:	c = CON_KEY_F12;	break;
			case 115:	c = CON_KEY_CTRL_LEFT;	break;
			case 116:	c = CON_KEY_CTRL_RIGHT;	break;
			case 141:	c = CON_KEY_CTRL_UP;	break;
			case 145:	c = CON_KEY_CTRL_DOWN;	break;
			case 119:	c = CON_KEY_CTRL_HOME;	break;
			case 117:	c = CON_KEY_CTRL_END;	break;
		}
	}
	else if (c == 0)
	{
		c = ::_getch_nolock();
		switch (c)
		{
			case 59:	c = CON_KEY_F1;		break;
			case 60:	c = CON_KEY_F2;		break;
			case 61:	c = CON_KEY_F3;		break;
			case 62:	c = CON_KEY_F4;		break;
			case 63:	c = CON_KEY_F5;		break;
			case 64:	c = CON_KEY_F6;		break;
			case 65:	c = CON_KEY_F7;		break;
			case 66:	c = CON_KEY_F8;		break;
			case 67:	c = CON_KEY_F9;		break;
			case 68:	c = CON_KEY_F10;	break;
		}
	}
#elif defined(LEPRA_POSIX)
	::fflush(stdout);
	int c = Getc(-1.0f);
	if (c == 27)
	{
		c = CON_KEY_ESCAPE;
		int c2 = Getc(0.5f);
		if (c2 == '[')
		{
			c = '?';
			UnicodeString lSequence;
			for (int x = 0; x < 30 && c2 > 0 && c2 != '~'; ++x)
			{
				c2 = Getc(0.5f);
				lSequence.push_back(c2);
				if (lSequence == L"A")		{ c = CON_KEY_UP;	break; }
				else if (lSequence == L"B")	{ c = CON_KEY_DOWN;	break; }
				else if (lSequence == L"C")	{ c = CON_KEY_RIGHT;	break; }
				else if (lSequence == L"D")	{ c = CON_KEY_LEFT;	break; }
				else if (lSequence == L"1~")	c = CON_KEY_HOME;
				else if (lSequence == L"2~")	c = CON_KEY_INSERT;
				else if (lSequence == L"3~")	c = CON_KEY_DELETE;
				else if (lSequence == L"4~")	c = CON_KEY_END;
				else if (lSequence == L"5~")	c = CON_KEY_PAGE_UP;
				else if (lSequence == L"6~")	c = CON_KEY_PAGE_DOWN;
				else if (lSequence == L"11~")	c = CON_KEY_F1;
				else if (lSequence == L"12~")	c = CON_KEY_F2;
				else if (lSequence == L"13~")	c = CON_KEY_F3;
				else if (lSequence == L"14~")	c = CON_KEY_F4;
				else if (lSequence == L"15~")	c = CON_KEY_F5;
				else if (lSequence == L"17~")	c = CON_KEY_F6;
				else if (lSequence == L"18~")	c = CON_KEY_F7;
				else if (lSequence == L"19~")	c = CON_KEY_F8;
				else if (lSequence == L"20~")	c = CON_KEY_F9;
				else if (lSequence == L"21~")	c = CON_KEY_F10;
				else if (lSequence == L"23~")	c = CON_KEY_F11;
				else if (lSequence == L"24~")	c = CON_KEY_F12;
			}
		}
	}
	else if (c == '\n')
	{
		c = '\r';
	}
	else if (c == 127)
	{
		c = '\b';
	}
#else
#error "Other console support not implemented..."
#endif // LEPRA_WINDOWS
	return (c);
}

void StdioConsolePrompt::ReleaseWaitCharThread()
{
#ifdef LEPRA_WINDOWS
	::_ungetch_nolock(-100);
#elif defined(LEPRA_POSIX)
	::write(fileno(stdin), "Q", 1);
#else
#error "other curses support not implemented..."
#endif // LEPRA_WINDOWS
}

void StdioConsolePrompt::Backspace(size_t pCount)
{
	for (size_t x = 0; x < pCount; ++x)
	{
		::printf("\b");
	}
}

void StdioConsolePrompt::EraseText(size_t pCount)
{
	for (size_t x = 0; x < pCount; ++x)
	{
		::printf(" ");
	}
	Backspace(pCount);
}

void StdioConsolePrompt::PrintPrompt(const Lepra::String& pPrompt, const Lepra::String& pInputText, size_t pEditIndex)
{
	::printf("\r%s%s", Lepra::AnsiStringUtility::ToOwnCode(pPrompt).c_str(), Lepra::AnsiStringUtility::ToOwnCode(pInputText).c_str());
	// Back up to edit index.
	for (size_t x = pInputText.length(); x > pEditIndex; --x)
	{
		::printf("\b");
	}
	::fflush(stdout);
}


}
