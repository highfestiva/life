
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



namespace lepra {



template<class _Base> ConsoleExecutor<_Base>::ConsoleExecutor(_Base* instance, CommandCallback command_listener, CommandErrorCallback command_error_listener):
	instance_(instance),
	command_listener_(command_listener),
	command_error_listener_(command_error_listener) {
}

template<class _Base> ConsoleExecutor<_Base>::~ConsoleExecutor() {
	instance_ = 0;
	command_listener_ = 0;
	command_error_listener_ = 0;
}



template<class _Base> int ConsoleExecutor<_Base>::Execute(const str& command, const strutil::strvec& parameter_list) {
	return ((instance_->*command_listener_)(command, parameter_list));
}

template<class _Base> void ConsoleExecutor<_Base>::OnExecutionError(const str& command,
	const strutil::strvec& parameter_list, int result) {
	(instance_->*(command_error_listener_))(command, parameter_list, result);
}



}
