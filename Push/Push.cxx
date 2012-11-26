
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Cure/Include/RuntimeVariable.h"
#include "../Lepra/Include/SystemManager.h"
#include "../UiCure/Include/UiCure.h"
#include "../UiCure/Include/UiGameUiManager.h"
#include "../UiLepra/Include/UiCore.h"
#include "../UiTBC/Include/UiTBC.h"
#include "../Life/LifeServer/MasterServerConnection.h"
#include "../Life/LifeApplication.h"
#include "PushMaster/MasterServerPort.h"
#include "Push.h"
#include "PushTicker.h"
#include "RtVar.h"
#include "Version.h"



namespace Push
{



class Push: public Life::Application
{
	typedef Life::Application Parent;
public:
	Push(const strutil::strvec& pArgumentList);
	virtual ~Push();
	virtual void Init();

private:
	str GetName() const;
	str GetVersion() const;
	Cure::ApplicationTicker* CreateTicker() const;

	UiCure::GameUiManager* mUiManager;

	LOG_CLASS_DECLARE();
};



}



LEPRA_RUN_APPLICATION(Push::Push, UiLepra::UiMain);



namespace Push
{



Push::Push(const strutil::strvec& pArgumentList):
	Parent(_T(PUSH_APPLICATION_NAME), pArgumentList),
	mUiManager(0)
{
}

Push::~Push()
{
	Destroy();

	delete (mUiManager);
	mUiManager = 0;

	UiCure::Shutdown();
	UiTbc::Shutdown();
	UiLepra::Shutdown();
}

void Push::Init()
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

str Push::GetName() const
{
	return _T("Push");
}

str Push::GetVersion() const
{
	return _T(PLATFORM_VERSION);
}

Cure::ApplicationTicker* Push::CreateTicker() const
{
	PushTicker* lTicker = new PushTicker(mUiManager, mResourceManager, 2000, 7, 1);
	lTicker->SetMasterServerConnection(new Life::MasterServerConnection(_T(MASTER_SERVER_ADDRESS) _T(":") _T(MASTER_SERVER_PORT)));
	return lTicker;
}



LOG_CLASS_DEFINE(GAME, Push);



}
