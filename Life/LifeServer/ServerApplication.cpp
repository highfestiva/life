
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../../Cure/Include/RuntimeVariable.h"
#include "../LifeApplication.h"
#include "GameServerTicker.h"
#include "RtVar.h"



namespace Life
{



class ServerApplication: public Application
{
public:
	typedef Application Parent;

	ServerApplication(const strutil::strvec& pArgumentList);
	virtual ~ServerApplication();
	str GetName() const;
	Cure::GameTicker* CreateGameTicker() const;

private:
	LogListener* CreateConsoleLogListener() const;
};



}



LEPRA_RUN_APPLICATION(Life::ServerApplication);



namespace Life
{



ServerApplication::ServerApplication(const strutil::strvec& pArgumentList):
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

str ServerApplication::GetName() const
{
	return (_T("Server"));
}

Cure::GameTicker* ServerApplication::CreateGameTicker() const
{
	return (new GameServerTicker(mResourceManager, (InteractiveStdioConsoleLogListener*)mConsoleLogger));
}

LogListener* ServerApplication::CreateConsoleLogListener() const
{
	return (new InteractiveStdioConsoleLogListener());
}



}
