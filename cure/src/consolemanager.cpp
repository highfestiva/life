
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/consolemanager.h"
#include "../../lepra/include/loglistener.h"
#include "../../lepra/include/systemmanager.h"
#include "../include/cure.h"
#include "../include/runtimevariable.h"



namespace cure {



ConsoleManager::ConsoleManager(RuntimeVariableScope* variable_scope, InteractiveConsoleLogListener* console_logger,
	ConsolePrompt* console_prompt):
	variable_scope_(variable_scope),
	console_logger_(console_logger),
	console_prompt_(console_prompt),
	console_command_manager_(0),
	console_thread_(0),
	history_silent_until_next_execute_(false) {
}

ConsoleManager::~ConsoleManager() {
	Join();
	console_logger_ = 0;
	delete console_prompt_;
	console_prompt_ = 0;
	variable_scope_ = 0;
	delete console_command_manager_;
	console_command_manager_ = 0;
};

void ConsoleManager::SetConsoleLogger(InteractiveConsoleLogListener* logger) {
	console_logger_ = logger;
}

bool ConsoleManager::Start() {
	if (!console_thread_) {
		console_thread_ = new MemberThread<ConsoleManager>("ConsoleThread");
	}
	return (console_thread_->Start(this, &ConsoleManager::ConsoleThreadEntry));
}

void ConsoleManager::Join() {
	if (console_thread_ && console_thread_->IsRunning()) {
		console_thread_->RequestStop();
	}

	{
		// Join forks.
		ScopeLock lock(&lock_);
		ForkList::iterator x = fork_list_.begin();
		for (; x != fork_list_.end(); ++x) {
			(*x)->RequestStop();
		}
	}
	if (console_prompt_) {
		console_prompt_->ReleaseWaitCharThread();
	}
	if (Thread::GetCurrentThread() != console_thread_ && console_thread_) {
		if (console_thread_->IsRunning()) {
			console_thread_->Join(0.5);
			console_thread_->Kill();
			delete console_thread_;
			console_thread_ = 0;
		}
	}

	// Wait for forks.
	for (int x = 0; x < 1000; ++x) {
		{
			ScopeLock lock(&lock_);
			if (fork_list_.empty()) {
				break;
			}
		}
		Thread::Sleep(0.01f);
	}
	{
		// Kill forks.
		ScopeLock lock(&lock_);
		ForkList::iterator x = fork_list_.begin();
		for (; x != fork_list_.end(); ++x) {
			(*x)->Kill();
		}
	}
}



void ConsoleManager::PushYieldCommand(const str& command) {
	ScopeLock lock(&lock_);
	yield_command_list_.push_back(command);
}

int ConsoleManager::ExecuteCommand(const str& command) {
	return (console_command_manager_->Execute(command, false));
}

int ConsoleManager::ExecuteYieldCommand() {
	str yield_command;
	{
		ScopeLock lock(&lock_);
		if (!yield_command_list_.empty()) {
			yield_command = yield_command_list_.front();
			yield_command_list_.pop_front();
		}
	}
	if (!yield_command.empty()) {
		return (console_command_manager_->Execute(yield_command, false));
	}
	return (-1);
}

ConsoleCommandManager* ConsoleManager::GetConsoleCommandManager() const {
	return (console_command_manager_);
}



LogDecorator& ConsoleManager::GetLogger() const {
	return (log_);
}



void ConsoleManager::AddFork(Thread* thread) {
	ScopeLock lock(&lock_);
	fork_list_.push_back(thread);
}

void ConsoleManager::RemoveFork(Thread* thread) {
	ScopeLock lock(&lock_);
	fork_list_.remove(thread);
}



InteractiveConsoleLogListener* ConsoleManager::GetConsoleLogger() const {
	return (console_logger_);
}

ConsolePrompt* ConsoleManager::GetConsolePrompt() const {
	return (console_prompt_);
}

RuntimeVariableScope* ConsoleManager::GetVariableScope() const {
	return (variable_scope_);
}



bool ConsoleManager::ForkExecuteCommand(const str& command) {
	class ForkThread: public Thread {
	public:
		ForkThread(ConsoleManager* console, const str& command):
			Thread("ConsoleFork"),
			console_(console),
			command_(command) {
		}
	private:
		void Run() {
			log_volatile(console_->GetLogger().Debug("ForkThread: started."));
			if (console_->ExecuteCommand(command_) != 0) {
				console_->GetLogger().Error("ForkThread: execution resulted in an error.");
			}
			log_volatile(console_->GetLogger().Debug("ForkThread: ended."));
			console_->RemoveFork(this);
		}
		ConsoleManager* console_;
		str command_;
		void operator=(const ForkThread&) {};
	};
	if (SystemManager::GetQuitRequest()) {
		return false;
	}
	ForkThread* executor = new ForkThread(this, command);
	AddFork(executor);
	executor->RequestSelfDestruct();
	return (executor->Start());
}



void ConsoleManager::InitCommands() {
	console_command_manager_ = new ConsoleCommandManager();
	console_command_manager_->AddExecutor(new ConsoleExecutor<ConsoleManager>(this, &ConsoleManager::OnCommandLocal, &ConsoleManager::OnCommandError));
	AddCommands();
}

std::list<str> ConsoleManager::GetCommandList() const {
	str dummy;
	std::list<str> _command_list = console_command_manager_->GetCommandCompletionList("", dummy);
	return (_command_list);
}

int ConsoleManager::TranslateCommand(const HashedString& command) const {
	CommandLookupMap::const_iterator x = command_lookup_.find(command);
	if (x != command_lookup_.end()) {
		return x->second;
	}
	return -1;
}

void ConsoleManager::PrintCommandList(const std::list<str>& command_list) {
	std::list<str>::const_iterator x = command_list.begin();
	int spacing;
	v_get(spacing, =, variable_scope_, kRtvarConsoleColumnspacing, 2);
	size_t longest_command = 10;
	for (; x != command_list.end(); ++x) {

		const size_t __length = x->length()+spacing;
		longest_command = (__length > longest_command)? __length : longest_command;
	}
	size_t indent = 0;
	str format = strutil::Format("%%-%is", longest_command);
	for (x = command_list.begin(); x != command_list.end(); ++x) {
		str _command = strutil::Format(format.c_str(), x->c_str());
		console_logger_->OnLogRawMessage(_command);
		indent += _command.length();
		int console_width;
		v_get(console_width, =, GetVariableScope(), kRtvarConsoleCharacterwidth, 80);
		if ((int)(indent+longest_command) >= console_width) {
			console_logger_->OnLogRawMessage("\n");
			indent = 0;
		}
	}
	if (!command_list.empty() && indent != 0) {
		console_logger_->OnLogRawMessage("\n");
	}
}



void ConsoleManager::AddCommands() {
	for (unsigned x = 0; x < GetCommandCount(); ++x) {
		const CommandPair& _command = GetCommand(x);
		HashedString name(_command.command_name_);
		console_command_manager_->AddCommand(name);
		command_lookup_.insert(CommandLookupMap::value_type(name, _command.command_id_));
	}
}

void ConsoleManager::ConsoleThreadEntry() {
	// Main console IO loop.
	const str prompt(">");
	wstr input_text;
	size_t edit_index = 0;
	while (!SystemManager::GetQuitRequest() && console_thread_ && !console_thread_->GetStopRequest()) {
		// Execute any pending yield command.
		if (input_text.empty()) {
			if (ExecuteYieldCommand() >= 0) {
				continue;
			}
		}

		console_prompt_->PrintPrompt(prompt, strutil::Encode(input_text), edit_index);

		console_logger_->SetAutoPrompt(prompt + strutil::Encode(input_text));
		int c = console_prompt_->WaitChar();
		console_logger_->SetAutoPrompt("");

		str word_delimitors_utf8;
		int key_completion;
		int key_enter;
		int key_silent;
		int key_backspace;
		int key_delete;
		int key_ctrl_left;
		int key_ctrl_right;
		int key_home;
		int key_end;
		int key_up;
		int key_down;
		int key_left;
		int key_right;
		int key_esc;
		int key_pg_up;
		int key_pg_dn;
		v_get(word_delimitors_utf8, =, variable_scope_, kRtvarConsoleCharacterdelimitors, " ");
		v_get(key_completion, =, variable_scope_, kRtvarConsoleKeyCompletion, (int)'\t');
		v_get(key_enter, =, variable_scope_, kRtvarConsoleKeyEnter, (int)'\r');
		v_get(key_silent, =, variable_scope_, kRtvarConsoleKeySilent, (int)'\v');
		v_get(key_backspace, =, variable_scope_, kRtvarConsoleKeyBackspace, (int)'\b');
		v_get(key_delete, =, variable_scope_, kRtvarConsoleKeyDelete, ConsolePrompt::kConKeyDelete);
		v_get(key_ctrl_left, =, variable_scope_, kRtvarConsoleKeyCtrlleft, ConsolePrompt::kConKeyCtrlLeft);
		v_get(key_ctrl_right, =, variable_scope_, kRtvarConsoleKeyCtrlright, ConsolePrompt::kConKeyCtrlRight);
		v_get(key_home, =, variable_scope_, kRtvarConsoleKeyHome, ConsolePrompt::kConKeyHome);
		v_get(key_end, =, variable_scope_, kRtvarConsoleKeyEnd, ConsolePrompt::kConKeyEnd);
		v_get(key_up, =, variable_scope_, kRtvarConsoleKeyUp, ConsolePrompt::kConKeyUp);
		v_get(key_down, =, variable_scope_, kRtvarConsoleKeyDown, ConsolePrompt::kConKeyDown);
		v_get(key_left, =, variable_scope_, kRtvarConsoleKeyLeft, ConsolePrompt::kConKeyLeft);
		v_get(key_right, =, variable_scope_, kRtvarConsoleKeyRight, ConsolePrompt::kConKeyRight);
		v_get(key_esc, =, variable_scope_, kRtvarConsoleKeyEsc, ConsolePrompt::kConKeyEscape);
		v_get(key_pg_up, =, variable_scope_, kRtvarConsoleKeyPageup, ConsolePrompt::kConKeyPageUp);
		v_get(key_pg_dn, =, variable_scope_, kRtvarConsoleKeyPagedown, ConsolePrompt::kConKeyPageDown);
		const wstr word_delimitors = wstrutil::Encode(word_delimitors_utf8);

		if (c == key_completion) {
			str best_completion_string;
			std::list<str> completions =
				console_command_manager_->GetCommandCompletionList(strutil::Encode(input_text), best_completion_string);
			// Print command completion list.
			if (completions.size() > 1) {
				console_logger_->OnLogRawMessage("\n");
				PrintCommandList(completions);
			}
			input_text = wstrutil::Encode(best_completion_string);
			edit_index = input_text.length();
			if (completions.size() == 1) {
				input_text += ' ';
				++edit_index;
			}
		} else if (c == key_enter) {
			console_logger_->OnLogRawMessage("\r" + prompt + strutil::Encode(input_text) + "\n");
			console_command_manager_->Execute(strutil::Encode(input_text), !history_silent_until_next_execute_);
			history_silent_until_next_execute_ = false;
			input_text = L"";
			edit_index = 0;
		} else if (c == key_silent) {
			history_silent_until_next_execute_ = true;
		} else if (c == key_backspace) {
			if (edit_index > 0) {
				// Reset index in console history.
				console_command_manager_->SetCurrentHistoryIndex((int)console_command_manager_->GetHistoryCount());

				--edit_index;
				console_prompt_->Backspace(1);
				console_prompt_->EraseText(input_text.length()-edit_index);
				wstr new_input_text = input_text.substr(0, edit_index) + input_text.substr(edit_index+1);
				input_text = new_input_text;
			}
		} else if (c == key_delete) {
			if (edit_index < input_text.length()) {
				// Reset index in console history.
				console_command_manager_->SetCurrentHistoryIndex((int)console_command_manager_->GetHistoryCount());

				console_prompt_->EraseText(input_text.length()-edit_index);
				wstr new_input_text = input_text.substr(0, edit_index) + input_text.substr(edit_index+1);
				input_text = new_input_text;
			}
		} else if (c == key_ctrl_left) {
			edit_index = wstrutil::FindPreviousWord(input_text, word_delimitors, edit_index);
		} else if (c == key_ctrl_right) {
			edit_index = wstrutil::FindNextWord(input_text, word_delimitors, edit_index);
		} else if (c == key_home) {
			edit_index = 0;
		} else if (c == key_end) {
			edit_index = input_text.length();
		} else if (c == key_up || c == key_down) {
			// Erase current text.
			console_prompt_->Backspace(edit_index);
			console_prompt_->EraseText(input_text.length());
			int desired_history_index = console_command_manager_->GetCurrentHistoryIndex();
			if (c == key_up) {
				// History->previous.
				--desired_history_index;
			} else if (c == key_down) {
				// History->next.
				++desired_history_index;
			}
			console_command_manager_->SetCurrentHistoryIndex(desired_history_index);
			desired_history_index = console_command_manager_->GetCurrentHistoryIndex();
			input_text = wstrutil::Encode(console_command_manager_->GetHistory(desired_history_index));
			edit_index = input_text.length();
		} else if (c == key_left) {
			if (edit_index > 0) {
				--edit_index;
			}
		} else if (c == key_right) {
			if (edit_index < input_text.length()) {
				++edit_index;
			}
		} else if (c == key_esc) {
			console_prompt_->Backspace(edit_index);
			console_prompt_->EraseText(input_text.length());
			input_text = L"";
			edit_index = 0;
		} else if (c == key_pg_up) {
			console_logger_->StepPage(-1);
		} else if (c == key_pg_dn) {
			console_logger_->StepPage(+1);
		} else if (c < 0) {
		} else {
			const wstr __c(1, (wchar_t)c);
			input_text.insert(edit_index, __c);
			++edit_index;
		}
	}
}

int ConsoleManager::OnCommandLocal(const str& command, const strutil::strvec& parameter_vector) {
	return (OnCommand(command, parameter_vector));
}

void ConsoleManager::OnCommandError(const str& command, const strutil::strvec&, int result) {
	if (result < 0) {
		log_.Warningf("Unknown command: \"%s\". Type 'help' for more info.", command.c_str());
	} else {
		log_.Warningf("Command error: \"%s\" returned error code %i.", command.c_str(), result);
	}
}



loginstance(kConsole, ConsoleManager);



}
