
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



namespace Lepra
{



template<class _Base> ConsoleExecutor<_Base>::ConsoleExecutor(_Base* pInstance, CommandCallback pCommandListener, CommandErrorCallback pCommandErrorListener):
	mInstance(pInstance),
	mCommandListener(pCommandListener),
	mCommandErrorListener(pCommandErrorListener)
{
}

template<class _Base> ConsoleExecutor<_Base>::~ConsoleExecutor()
{
	mInstance = 0;
	mCommandListener = 0;
	mCommandErrorListener = 0;
}



template<class _Base> int ConsoleExecutor<_Base>::Execute(const str& pCommand, const strutil::strvec& pParameterList)
{
	return ((mInstance->*mCommandListener)(pCommand, pParameterList));
}

template<class _Base> void ConsoleExecutor<_Base>::OnExecutionError(const str& pCommand,
	const strutil::strvec& pParameterList, int pResult)
{
	(mInstance->*(mCommandErrorListener))(pCommand, pParameterList, pResult);
}



}
