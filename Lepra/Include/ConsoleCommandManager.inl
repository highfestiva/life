
// Author: Jonas Byström
// Copyright (c) 2002-2006, Righteous Games



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



template<class _Base> int ConsoleExecutor<_Base>::OnExecute(const String& pCommand, const StringUtility::StringVector& pParameterList)
{
	return ((mInstance->*mCommandListener)(pCommand, pParameterList));
}

template<class _Base> void ConsoleExecutor<_Base>::OnExecutionError(const String& pCommand,
	const StringUtility::StringVector& pParameterList, int pResult)
{
	(mInstance->*(mCommandErrorListener))(pCommand, pParameterList, pResult);
}



}
