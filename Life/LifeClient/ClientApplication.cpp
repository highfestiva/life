
// Author: Jonas Bystr�m
// Copyright (c) 2002-2009, Righteous Games



#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Lepra/Include/SystemManager.h"
#include "../../UiCure/Include/UiCure.h"
#include "../../UiCure/Include/UiGameUiManager.h"
#include "../../UiLepra/Include/UiCore.h"
#include "../../UiTBC/Include/UiTBC.h"
#include "../LifeMaster/MasterServerPort.h"
#include "../LifeServer/MasterServerConnection.h"
#include "../LifeApplication.h"
#include "GameClientMasterTicker.h"
#include "RtVar.h"
#include "../LifeServer/Version.h"



namespace Life
{



class ClientApplication: public Application
{
public:
	typedef Application Parent;

	ClientApplication(const strutil::strvec& pArgumentList);
	virtual ~ClientApplication();
	virtual void Init();

private:
	str GetName() const;
	str GetVersion() const;
	Cure::ApplicationTicker* CreateTicker() const;

	UiCure::GameUiManager* mUiManager;

	LOG_CLASS_DECLARE();
};



}



LEPRA_RUN_APPLICATION(Life::ClientApplication, UiLepra::UiMain);



namespace Life
{



ClientApplication::ClientApplication(const strutil::strvec& pArgumentList):
	Application(pArgumentList),
	mUiManager(0)
{
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

void ClientApplication::Init()
{
	UiLepra::Init();
	UiTbc::Init();
	UiCure::Init();

	// This sets the default settings for client-specific rtvars. Note that these should not be removed,
	// since that causes the client to start without defaults.
	CURE_RTVAR_SET(UiCure::GetSettings(), RTVAR_GAME_CHILDISHNESS, 1.0);
	CURE_RTVAR_SET(UiCure::GetSettings(), RTVAR_GAME_ENABLESTARTLOGO, true);
	CURE_RTVAR_SET(UiCure::GetSettings(), RTVAR_GAME_TIMEOFDAYFACTOR, 1.0);
	CURE_RTVAR_SET(UiCure::GetSettings(), RTVAR_NETWORK_ENABLEOPENSERVER, false);
	CURE_RTVAR_INTERNAL(UiCure::GetSettings(), RTVAR_UI_3D_CAMDISTANCE, 20.0);
	CURE_RTVAR_INTERNAL(UiCure::GetSettings(), RTVAR_UI_3D_CAMHEIGHT, 10.0);
	CURE_RTVAR_INTERNAL(UiCure::GetSettings(), RTVAR_UI_3D_CAMROTATE, 0.0);

	mUiManager = new UiCure::GameUiManager(UiCure::GetSettings());

	Parent::Init();
}

str ClientApplication::GetName() const
{
	return _T("Client");
}

str ClientApplication::GetVersion() const
{
	return _T(PLATFORM_VERSION);
}

Cure::ApplicationTicker* ClientApplication::CreateTicker() const
{
	GameClientMasterTicker* lTicker = new GameClientMasterTicker(mUiManager, mResourceManager, 2000, 7, 1);
	lTicker->SetMasterServerConnection(new MasterServerConnection(_T(MASTER_SERVER_ADDRESS) _T(":") _T(MASTER_SERVER_PORT)));
	return lTicker;
}



LOG_CLASS_DEFINE(GAME, ClientApplication);



}
