
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../../Cure/Include/RuntimeVariable.h"
#include "../LifeApplication.h"
#include "GameServerManager.h"



namespace Life
{



class ServerApplication: public Application
{
public:
	typedef Application Parent;

	ServerApplication(const Lepra::StringUtility::StringVector& pArgumentList);
	virtual ~ServerApplication();
	Lepra::String GetName() const;
	Cure::GameTicker* CreateGameTicker() const;

private:
	Lepra::LogListener* CreateConsoleLogListener() const;
};



}



LEPRA_RUN_APPLICATION(Life::ServerApplication);



namespace Life
{



ServerApplication::ServerApplication(const Lepra::StringUtility::StringVector& pArgumentList):
	Application(pArgumentList)
{
	Lepra::Init();
	TBC::Init();
	Cure::Init();

	Init();
}

ServerApplication::~ServerApplication()
{
	Destroy();

	Cure::Shutdown();
	TBC::Shutdown();
	Lepra::Shutdown();
};

Lepra::String ServerApplication::GetName() const
{
	return (_T("Server"));
}

Cure::GameTicker* ServerApplication::CreateGameTicker() const
{
	Cure::RuntimeVariableScope* lVariables = new Cure::RuntimeVariableScope(Cure::GetSettings());
	return (new GameServerManager(lVariables, mResourceManager,
		(Lepra::InteractiveStdioConsoleLogListener*)mConsoleLogger));
}

Lepra::LogListener* ServerApplication::CreateConsoleLogListener() const
{
#ifdef LEPRA_CONSOLE
	return (new Lepra::InteractiveStdioConsoleLogListener());
#else // !LEPRA_CONSOLE
	return (Parent::CreateConsoleLogListener());
#endif // LEPRA_CONSOLE / !LEPRA_CONSOLE
}



}
