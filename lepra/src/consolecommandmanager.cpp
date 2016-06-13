
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/consolecommandmanager.h"
#include "../include/lepraos.h"
#include "../include/systemmanager.h"
#ifdef LEPRA_WINDOWS
#include <conio.h>
#elif defined(LEPRA_POSIX)
#include <termios.h>
#endif // Windows / POSIX



namespace lepra {



#if defined(LEPRA_POSIX)
int Getc(float timeout) {
	const int fn = fileno(stdin);
	fd_set char_set;
	FD_ZERO(&char_set);
	FD_SET(fn, &char_set);
	timeval* time = 0;
	timeval char_timeout;
	if (timeout >= 0) {
		time = &char_timeout;
		char_timeout.tv_sec = (int)timeout;
		timeout -= char_timeout.tv_sec;
		char_timeout.tv_usec = (int)(timeout*1000*1000);
	}
	int result = -1;
	if (::select(fn+1, &char_set, 0, 0, time) > 0) {
		unsigned char c;
		if (::read(fn, &c, 1) == 1) {
			result = c;
		}
	}
	return (result);
}
#endif // Posix.



CommandCompleter::CommandCompleter() {
}

CommandCompleter::~CommandCompleter() {
}



ConsoleCommandExecutor::ConsoleCommandExecutor() {
}

ConsoleCommandExecutor::~ConsoleCommandExecutor() {
}



ConsoleCommandManager::ConsoleCommandManager():
	current_history_index_(0) {
	AddCompleter(this);
}

ConsoleCommandManager::~ConsoleCommandManager() {
	CommandExecutorSet::iterator e = command_executor_set_.begin();
	for (; e != command_executor_set_.end(); ++e) {
		ConsoleCommandExecutor* executor = *e;
		delete (executor);
	}
	command_executor_set_.clear();

	RemoveCompleter(this);
	CommandCompleterSet::iterator x = command_completer_list_.begin();
	for (; x != command_completer_list_.end(); ++x) {
		CommandCompleter* _completer = *x;
		delete (_completer);
	}
	command_completer_list_.clear();
}



void ConsoleCommandManager::AddExecutor(ConsoleCommandExecutor* executor) {
	command_executor_set_.insert(executor);
}

void ConsoleCommandManager::DeleteExecutor(ConsoleCommandExecutor* executor) {
	command_executor_set_.erase(executor);
	delete (executor);
}



void ConsoleCommandManager::SetComment(const str& comment) {
	comment_ = comment;
}

bool ConsoleCommandManager::AddCommand(const str& command) {
	return command_set_.insert(command).second;
}

bool ConsoleCommandManager::RemoveCommand(const str& command) {
	return (command_set_.erase(command) != 0);
}

int ConsoleCommandManager::Execute(const str& command, bool append_to_history) {
	int execution_result = 0;
	str command_delimitors(" \t\v\r\n");

	str in_command(command);
	if (!comment_.empty()) {
		// Drop comments (ignore if inside string).
		in_command = in_command.substr(0, in_command.find(comment_, 0));
	}

	strutil::strvec command_list;
	command_list.reserve(10);
	strutil::FastBlockSplit(command_list, in_command, ";", true, true);

	if (command_list.size() == 0) {
		// We will execute nothing.
	} else if (append_to_history) {
		AppendHistory(command);
	}

	strutil::strvec command_token_list;
	command_token_list.reserve(2);
	strutil::strvec parameter_token_list;
	parameter_token_list.reserve(128);
	const size_t __size = command_list.size();
	for (size_t command_index = 0; execution_result == 0 && command_index < __size; ++command_index) {
		const str& temp_command = command_list[command_index];
		str _command = strutil::StripLeft(temp_command, command_delimitors);
		command_token_list.clear();
		strutil::FastBlockSplit(command_token_list, _command, command_delimitors, false, true, 1);
		if (command_token_list.size() > 0) {
			_command = command_token_list[0];
			parameter_token_list.clear();
			if (command_token_list.size() > 1) {
				str parameters = command_token_list[1];
				parameters = strutil::StripLeft(parameters, command_delimitors);
				strutil::FastBlockSplit(parameter_token_list, parameters, command_delimitors, false, true);
			}

			bool executed_ok = false;
			execution_result = -2;
			CommandExecutorSet::iterator x = command_executor_set_.begin();
			for (; x != command_executor_set_.end() && execution_result < 0; ++x) {
				execution_result = (*x)->Execute(_command, parameter_token_list);
				if (execution_result == 0) {
					executed_ok = true;
				}
			}
			if (!executed_ok) {
				x = command_executor_set_.begin();
				for (; x != command_executor_set_.end() && execution_result < 0; ++x) {
					(*x)->OnExecutionError(_command, parameter_token_list, execution_result);
				}
			}
		}
	}
	return (execution_result);
}

void ConsoleCommandManager::AddCompleter(CommandCompleter* completer) {
	command_completer_list_.insert(completer);
}

void ConsoleCommandManager::RemoveCompleter(CommandCompleter* completer) {
	command_completer_list_.erase(completer);
}

ConsoleCommandManager::CommandList ConsoleCommandManager::GetCommandCompletionList(
	const str& partial_command, str& completed) const {
	// Pick partial command.
	const str command_delimitors(" \t\v\r\n");
	const str _partial_command = strutil::StripLeft(partial_command, command_delimitors);
	completed = _partial_command;

	// Complete partial command.
	CommandList completion_list;
	CommandCompleterSet::const_iterator x = command_completer_list_.begin();
	for (; x != command_completer_list_.end(); ++x) {
		CommandList temp_completions = (*x)->CompleteCommand(_partial_command);
		completion_list.splice(completion_list.end(), temp_completions);
	}
	completion_list.sort();

	// Find lest common denominator (=best completion).
	size_t completion_letter_count = 1000;
	CommandList::const_iterator y = completion_list.begin();
	for (; y != completion_list.end(); ++y) {
		const str& completion = *y;
		// Trim completed string, depending on completion matching.
		if (completion.length() < completion_letter_count) {
			completion_letter_count = completion.length();
		}
		size_t z;
		for (z = _partial_command.length(); z < completion_letter_count; ++z) {
			if (z >= completed.length()) {
				// Add more text (more completed).
				completed += completion[z];
			} else if (completion[z] != completed[z]) {
				// Cut completion, since this command is shorter.
				completion_letter_count = z;
			}
		}
		completed.resize(completion_letter_count);
	}
	return (completion_list);
}

std::list<str> ConsoleCommandManager::CompleteCommand(const str& partial_command) const {
	std::list<str> completion_list;
	CommandSet::const_iterator x = command_set_.begin();
	for (; x != command_set_.end(); ++x) {
		const str& completion = *x;
		if (strutil::StartsWith(completion, partial_command)) {
			// Add to list.
			completion_list.push_back(completion);
		}
	}
	return (completion_list);
}

unsigned ConsoleCommandManager::GetHistoryCount() const {
	return ((unsigned)history_vector_.size());
}

void ConsoleCommandManager::SetCurrentHistoryIndex(int index) {
	if (index < 0) {
		index = 0;
	} else if (index > (int)GetHistoryCount()) {
		index = GetHistoryCount();
	}
	current_history_index_ = index;
}

int ConsoleCommandManager::GetCurrentHistoryIndex() const {
	return (current_history_index_);
}

str ConsoleCommandManager::GetHistory(int index) const {
	str history;
	if (index >= 0 && index < (int)history_vector_.size()) {
		history = history_vector_[index];
	}
	return (history);
}

void ConsoleCommandManager::AppendHistory(const str& command) {
	// Only append if this isn't exactly the same as the last command.
	if (history_vector_.size() == 0 || (command != history_vector_.back())) {
		history_vector_.push_back(command);
	}
	current_history_index_ = GetHistoryCount();
}



ConsolePrompt::ConsolePrompt() {
}

ConsolePrompt::~ConsolePrompt() {
}



StdioConsolePrompt::StdioConsolePrompt() {
}

StdioConsolePrompt::~StdioConsolePrompt() {
}

void StdioConsolePrompt::SetFocus(bool) {
	// Nothing to do, always focused.
}

int StdioConsolePrompt::WaitChar() {
	if (SystemManager::GetQuitRequest()) {
		return (-1);	// TRICKY: RAII simplifies platform-dependant code.
	}

#ifdef LEPRA_WINDOWS
	int c = _getch_nolock();
	if (c == 27) {
		c = kConKeyEscape;
	} else if (c == 224) {
		c = ::_getch_nolock();
		switch (c) {
			case 72:	c = kConKeyUp;		break;
			case 80:	c = kConKeyDown;	break;
			case 75:	c = kConKeyLeft;	break;
			case 77:	c = kConKeyRight;	break;
			case 73:	c = kConKeyPageDown;	break;
			case 81:	c = kConKeyPageUp;	break;
			case 71:	c = kConKeyHome;	break;
			case 79:	c = kConKeyEnd;	break;
			case 82:	c = kConKeyInsert;	break;
			case 83:	c = kConKeyDelete;	break;
			case 133:	c = kConKeyF11;	break;
			case 134:	c = kConKeyF12;	break;
			case 115:	c = kConKeyCtrlLeft;	break;
			case 116:	c = kConKeyCtrlRight;	break;
			case 141:	c = kConKeyCtrlUp;	break;
			case 145:	c = kConKeyCtrlDown;	break;
			case 119:	c = kConKeyCtrlHome;	break;
			case 117:	c = kConKeyCtrlEnd;	break;
		}
	} else if (c == 0) {
		c = ::_getch_nolock();
		switch (c) {
			case 59:	c = kConKeyF1;		break;
			case 60:	c = kConKeyF2;		break;
			case 61:	c = kConKeyF3;		break;
			case 62:	c = kConKeyF4;		break;
			case 63:	c = kConKeyF5;		break;
			case 64:	c = kConKeyF6;		break;
			case 65:	c = kConKeyF7;		break;
			case 66:	c = kConKeyF8;		break;
			case 67:	c = kConKeyF9;		break;
			case 68:	c = kConKeyF10;	break;
		}
	}
#elif defined(LEPRA_POSIX)
	::fflush(stdout);
	int c = Getc(-1.0f);
	if (c == 27) {
		c = kConKeyEscape;
		int c2 = Getc(0.5f);
		if (c2 == '[') {
			c = '?';
			wstr sequence;
			for (int x = 0; x < 30 && c2 > 0 && c2 != '~'; ++x) {
				c2 = Getc(0.5f);
				sequence.push_back(c2);
				if (sequence == L"A")		{ c = kConKeyUp;	break; } else if (sequence == L"B")	{ c = kConKeyDown;	break; } else if (sequence == L"C")	{ c = kConKeyRight;	break; } else if (sequence == L"D")	{ c = kConKeyLeft;	break; } else if (sequence == L"1~")	c = kConKeyHome;
				else if (sequence == L"2~")	c = kConKeyInsert;
				else if (sequence == L"3~")	c = kConKeyDelete;
				else if (sequence == L"4~")	c = kConKeyEnd;
				else if (sequence == L"5~")	c = kConKeyPageUp;
				else if (sequence == L"6~")	c = kConKeyPageDown;
				else if (sequence == L"11~")	c = kConKeyF1;
				else if (sequence == L"12~")	c = kConKeyF2;
				else if (sequence == L"13~")	c = kConKeyF3;
				else if (sequence == L"14~")	c = kConKeyF4;
				else if (sequence == L"15~")	c = kConKeyF5;
				else if (sequence == L"17~")	c = kConKeyF6;
				else if (sequence == L"18~")	c = kConKeyF7;
				else if (sequence == L"19~")	c = kConKeyF8;
				else if (sequence == L"20~")	c = kConKeyF9;
				else if (sequence == L"21~")	c = kConKeyF10;
				else if (sequence == L"23~")	c = kConKeyF11;
				else if (sequence == L"24~")	c = kConKeyF12;
			}
		}
	} else if (c == '\n') {
		c = '\r';
	} else if (c == 127) {
		c = '\b';
	}
#else
#error "Other console support not implemented..."
#endif // LEPRA_WINDOWS
	return (c);
}

void StdioConsolePrompt::ReleaseWaitCharThread() {
#ifdef LEPRA_WINDOWS
	::_ungetch_nolock(-100);
#elif defined(LEPRA_POSIX)
	::write(fileno(stdin), "Q", 1);
#else
#error "other curses support not implemented..."
#endif // LEPRA_WINDOWS
}

void StdioConsolePrompt::Backspace(size_t count) {
	for (size_t x = 0; x < count; ++x) {
		::printf("\b");
	}
}

void StdioConsolePrompt::EraseText(size_t count) {
	for (size_t x = 0; x < count; ++x) {
		::printf(" ");
	}
	Backspace(count);
}

void StdioConsolePrompt::PrintPrompt(const str& prompt, const str& input_text, size_t edit_index) {
	::printf("\r%s%s", prompt.c_str(), input_text.c_str());
	// Back up to edit index.
	for (size_t x = input_text.length(); x > edit_index; --x) {
		::printf("\b");
	}
	::fflush(stdout);
}


}
