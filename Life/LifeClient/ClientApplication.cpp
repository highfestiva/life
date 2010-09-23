
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../../Cure/Include/RuntimeVariable.h"
#include "../../UiCure/Include/UiCure.h"
#include "../../UiCure/Include/UiGameUiManager.h"
#include "../../UiLepra/Include/UiLepra.h"
#include "../../UiTBC/Include/UiTBC.h"
#include "../LifeApplication.h"
#include "GameClientMasterTicker.h"
#include "RtVar.h"



namespace Life
{



class ClientApplication: public Application
{
public:
	typedef Application Parent;

	ClientApplication(const strutil::strvec& pArgumentList);
	virtual ~ClientApplication();

private:
	str GetName() const;
	Cure::GameTicker* CreateGameTicker() const;

	UiCure::GameUiManager* mUiManager;
};



}



LEPRA_RUN_APPLICATION(Life::ClientApplication);



namespace Life
{



ClientApplication::ClientApplication(const strutil::strvec& pArgumentList):
	Application(pArgumentList),
	mUiManager(0)
{
	UiLepra::Init();
	UiTbc::Init();
	UiCure::Init();

	// This sets the default settings for client-specific rtvars. Note that these should not be removed,
	// since that causes the client to start without defaults.
	CURE_RTVAR_SET(UiCure::GetSettings(), RTVAR_GAME_ISCHILD, true);
	CURE_RTVAR_SET(UiCure::GetSettings(), RTVAR_GAME_ENABLESTARTLOGO, true);
	CURE_RTVAR_SET(UiCure::GetSettings(), RTVAR_GAME_TIMEOFDAYFACTOR, 1.0);
	CURE_RTVAR_SET(UiCure::GetSettings(), RTVAR_NETWORK_ENABLEOPENSERVER, false);
	CURE_RTVAR_SET(UiCure::GetSettings(), RTVAR_UI_3D_CAMDISTANCE, 20.0);
	CURE_RTVAR_SET(UiCure::GetSettings(), RTVAR_UI_3D_CAMHEIGHT, 10.0);
	CURE_RTVAR_SET(UiCure::GetSettings(), RTVAR_UI_3D_CAMROTATE, 0.0);

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

str ClientApplication::GetName() const
{
	return (_T("Client"));
}

Cure::GameTicker* ClientApplication::CreateGameTicker() const
{
	GameClientMasterTicker* lMaster = new GameClientMasterTicker(mUiManager, mResourceManager);
	return (lMaster);
}



}
