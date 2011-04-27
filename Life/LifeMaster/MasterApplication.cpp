
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "../../TBC/Include/TBC.h"
#include "../LifeApplication.h"
#include "MasterServer.h"



namespace Life
{



class MasterApplication: public Application
{
public:
	typedef Application Parent;

	MasterApplication(const strutil::strvec& pArgumentList);
	virtual ~MasterApplication();
	virtual void Init();

private:
	virtual str GetName() const;
	virtual str GetVersion() const;
	virtual Cure::GameTicker* CreateGameTicker() const;
};



}



LEPRA_RUN_APPLICATION(Life::MasterApplication, Lepra::Main);



namespace Life
{



MasterApplication::MasterApplication(const strutil::strvec& pArgumentList):
	Parent(pArgumentList)
{
}

MasterApplication::~MasterApplication()
{
	Destroy();

	Cure::Shutdown();
	TBC::Shutdown();
	Lepra::Shutdown();
};

void MasterApplication::Init()
{
	Lepra::Init();
	TBC::Init();
	Cure::Init();

	Init();
}

str MasterApplication::GetName() const
{
	return _T(MASTER_APPLICATION_NAME);
}

str MasterApplication::GetVersion() const
{
	return _T(MASTER_SERVER_VERSION);
}

Cure::GameTicker* MasterApplication::CreateGameTicker() const
{
	return new MasterServer;
}



}
