
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../../UiCure/Include/UiCure.h"
#include "../../UiCure/Include/UiGameUiManager.h"
#include "../../UiLepra/Include/UiLepra.h"
#include "../../UiTbc/Include/UiTbc.h"
#include "../LifeApplication.h"
#include "GameClientMasterTicker.h"



namespace Life
{



class ClientApplication: public Application
{
public:
	typedef Application Parent;

	ClientApplication(const Lepra::StringUtility::StringVector& pArgumentList);
	virtual ~ClientApplication();

private:
	Lepra::String GetName() const;
	Cure::GameTicker* CreateGameTicker() const;

	UiCure::GameUiManager* mUiManager;
};



}



LEPRA_RUN_APPLICATION(Life::ClientApplication);



namespace Life
{



ClientApplication::ClientApplication(const Lepra::StringUtility::StringVector& pArgumentList):
	Application(pArgumentList),
	mUiManager(0)
{
	UiLepra::Init();
	UiTbc::Init();
	UiCure::Init();

	mUiManager = new UiCure::GameUiManager(UiCure::GetSettings());

	Init();
}

ClientApplication::~ClientApplication()
{
	Destroy();

	delete (mUiManager);
	mUiManager = 0;

	UiCure::Shutdown();
	UiTbc::Shutdown();
	UiLepra::Shutdown();
}

Lepra::String ClientApplication::GetName() const
{
	return (_T("Client"));
}

Cure::GameTicker* ClientApplication::CreateGameTicker() const
{
	GameClientMasterTicker* lMaster = new GameClientMasterTicker(mUiManager, mResourceManager);
	return (lMaster);
}



}
