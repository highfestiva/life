
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "gameclientmasterticker.h"
#include "../../cure/include/networkclient.h"
#include "../../cure/include/networkfreeagent.h"
#include "../../cure/include/resourcemanager.h"
#include "../../cure/include/runtimevariable.h"
#include "../../cure/include/timemanager.h"
#include "../../lepra/include/network.h"
#include "../../lepra/include/number.h"
#include "../../lepra/include/performance.h"
#include "../../lepra/include/systemmanager.h"
#include "../../uicure/include/uigameuimanager.h"
#include "../../uicure/include/uiresourcemanager.h"
#include "../../uicure/include/uiruntimevariablename.h"
#include "../../uitbc/include/gui/uiconsoleloglistener.h"
#include "../../uitbc/include/gui/uiconsoleprompt.h"
#include "../lifeserver/masterserverconnection.h"
#include "../consolemanager.h"
#include "../lifeapplication.h"
#include "../systemutil.h"
#include "clientconsolemanager.h"
#include "gameclientslavemanager.h"
#include "rtvar.h"
#include "uiconsole.h"
#include "uigameservermanager.h"
#include "uiracescore.h"



namespace life {



#define kSetPlayerCount	"set-player-count"



GameClientMasterTicker::GameClientMasterTicker(UiCure::GameUiManager* ui_manager, cure::ResourceManager* resource_manager, float physics_radius, int physics_levels, float physics_sensitivity):
	Parent(physics_radius, physics_levels, physics_sensitivity),
	ui_manager_(ui_manager),
	resource_manager_(resource_manager),
	server_(0),
	master_connection_(0),
	free_network_agent_(new cure::NetworkFreeAgent),
	restart_ui_(false),
	initialized_(false),
	active_width_(0),
	active_height_(0),
	active_slave_count_(0),
	slave_top_split_(1),
	slave_bottom_split_(1),
	slave_v_split_(1),
	slave_fade_(0),
	performance_adjustment_ticks_(0),
	debug_font_id_(uitbc::FontManager::kInvalidFontid) {
	slave_array_.resize(4, 0);
	slave_array_[0] = 0;
	slave_array_[1] = 0;
	slave_array_[2] = 0;
	slave_array_[3] = 0;

	uilepra::DisplayManager::EnableScreensaver(false);

	cure::ContextObjectAttribute::SetCreator(cure::ContextObjectAttribute::Factory(
		this, &GameClientMasterTicker::CreateObjectAttribute));

	v_set(UiCure::GetSettings(), kRtvarCtrlUiConspeed, 2.7);
	v_set(UiCure::GetSettings(), kRtvarDebugPerformanceCount, true);
	v_set(UiCure::GetSettings(), kRtvarDebugPerformanceGraph, false);
	v_set(UiCure::GetSettings(), kRtvarDebugPerformanceNames, true);
	v_set(UiCure::GetSettings(), kRtvarDebugPerformanceYoffset, 10);
	v_set(UiCure::GetSettings(), kRtvarDebug3DDrawlocalserver, true);

	console_ = new ConsoleManager(resource_manager_, 0, UiCure::GetSettings(), 0, 0);
	console_->InitCommands();
	console_->GetConsoleCommandManager()->AddExecutor(
		new ConsoleExecutor<GameClientMasterTicker>(
			this, &GameClientMasterTicker::OnCommandLocal, &GameClientMasterTicker::OnCommandError));
	console_->GetConsoleCommandManager()->AddCommand(kSetPlayerCount);

	console_->ExecuteCommand("execute-file -i Default.lsh");
	console_->ExecuteCommand("execute-file -i " + Application::GetIoFile("ClientBase", "lsh"));
}

GameClientMasterTicker::~GameClientMasterTicker() {
	uilepra::DisplayManager::EnableScreensaver(true);

	{
		StashCalibration();

		console_->ExecuteCommand("save-system-config-file 0 " + Application::GetIoFile("ClientBase", "lsh"));
		delete (console_);
		console_ = 0;
	}

	if (ui_manager_->GetInputManager()) {
		ui_manager_->GetInputManager()->ReleaseAll();
		ui_manager_->GetInputManager()->RemoveKeyCodeInputObserver(this);
	}

	SlaveArray::iterator x;
	for (x = slave_array_.begin(); x != slave_array_.end(); ++x) {
		GameClientSlaveManager* _slave = *x;
		delete (_slave);
	}
	slave_array_.clear();

	DeleteServer();

	resource_manager_ = 0;
	ui_manager_ = 0;

	SetMasterServerConnection(0);
	delete free_network_agent_;
	free_network_agent_ = 0;
}

void GameClientMasterTicker::AddBackedRtvar(const str& rtvar_name) {
	rtvars_.push_back(rtvar_name);
}

void GameClientMasterTicker::Resume(bool hard) {
	Parent::Resume(hard);

	ScopeLock lock(&lock_);
	SlaveArray::iterator x;
	for (x = slave_array_.begin(); x != slave_array_.end(); ++x) {
		GameClientSlaveManager* _slave = *x;
		if (_slave) {
			_slave->Resume(hard);
		}
	}

	cure::RuntimeVariableScope* _scope = UiCure::GetSettings();
	_scope = slave_array_[0]? slave_array_[0]->GetVariableScope() : _scope;
	LoadRtvars(_scope);
}

void GameClientMasterTicker::Suspend(bool hard) {
	Parent::Suspend(hard);

	ScopeLock lock(&lock_);
	SlaveArray::iterator x;
	for (x = slave_array_.begin(); x != slave_array_.end(); ++x) {
		GameClientSlaveManager* _slave = *x;
		if (_slave) {
			_slave->Suspend(hard);
		}
	}

	cure::RuntimeVariableScope* _scope = UiCure::GetSettings();
	_scope = slave_array_[0]? slave_array_[0]->GetVariableScope() : _scope;
	SaveRtvars(_scope);
}

void GameClientMasterTicker::LoadRtvars(cure::RuntimeVariableScope* scope) {
	strutil::strvec::iterator x = rtvars_.begin();
	for (; x != rtvars_.end(); ++x) {
		SystemUtil::LoadRtvar(scope, *x);
	}
}

void GameClientMasterTicker::SaveRtvars(cure::RuntimeVariableScope* scope) {
	strutil::strvec::iterator x = rtvars_.begin();
	for (; x != rtvars_.end(); ++x) {
		SystemUtil::SaveRtvar(scope, *x);
	}
}

MasterServerConnection* GameClientMasterTicker::GetMasterServerConnection() const {
	return master_connection_;
}

void GameClientMasterTicker::SetMasterServerConnection(MasterServerConnection* connection) {
	delete master_connection_;
	master_connection_ = connection;
}



void GameClientMasterTicker::PrepareQuit() {
	if (ui_manager_->GetSoundManager()) {
		// Keep quiet so there won't be a lot of noise when destroying explosives, and so forth.
		ui_manager_->GetSoundManager()->SetMasterVolume(0);
	}
}



bool GameClientMasterTicker::Tick() {
	LEPRA_MEASURE_SCOPE(MasterTicker);

	GetTimeManager()->Tick();

	ui_manager_->InputTick();

	bool ok = true;

	ScopeLock lock(&lock_);

	SlaveArray::iterator x;

	if (master_connection_) {
		LEPRA_MEASURE_SCOPE(MasterServerConnectionTick);
		// If we're not running server: use free network interface for obtaining server list.
		if (!server_) {
			free_network_agent_->Tick();
			float connect_timeout;
			v_get(connect_timeout, =(float), UiCure::GetSettings(), kRtvarNetworkConnectTimeout, 3.0);
			master_connection_->SetSocketInfo(free_network_agent_, connect_timeout);
		}
		bool allow_online;
		v_get(allow_online, =, UiCure::GetSettings(), kRtvarNetworkEnableonlinemaster, false);
		if (allow_online) {
			master_connection_->Tick();
		}
	}

	{
		LEPRA_MEASURE_SCOPE(BeginRenderAndInput);

		float r, g, b;
		v_get(r, =(float), UiCure::GetSettings(), kRtvarUi3DClearred, 0.75);
		v_get(g, =(float), UiCure::GetSettings(), kRtvarUi3DCleargreen, 0.80);
		v_get(b, =(float), UiCure::GetSettings(), kRtvarUi3DClearblue, 0.85);
		vec3 _color(r, g, b);
		BeginRender(_color);
	}

	if (server_) {
		LEPRA_MEASURE_SCOPE(ServerBeginTick);
		server_->BeginTick();
	}

	{
		LEPRA_MEASURE_SCOPE(BeginTickSlaves);

		// Kickstart physics so no slaves have to wait too long for completion.
		int _slave_index = 0;
		for (x = slave_array_.begin(); ok && x != slave_array_.end(); ++x) {
			GameClientSlaveManager* _slave = *x;
			if (_slave) {
				ui_manager_->GetSoundManager()->SetCurrentListener(_slave_index, active_slave_count_);
				ok = _slave->BeginTick();
				++_slave_index;
			}
		}
	}

	StartPhysicsTick();

	{
		MeasureLoad();
	}

	{
		//LEPRA_MEASURE_SCOPE(RenderSlaves);

		ui_manager_->GetRenderer()->ClearDebugInfo();

		// Start rendering machine directly afterwards.
		int _slave_index = 0;
		for (x = slave_array_.begin(); ok && x != slave_array_.end(); ++x) {
			GameClientSlaveManager* _slave = *x;
			if (_slave) {
				ui_manager_->GetSoundManager()->SetCurrentListener(_slave_index, active_slave_count_);
				++_slave_index;
				if (ui_manager_->CanRender()) {
					ok = _slave->Render();
				}
			}
		}
	}

	bool lights_enabled = true;
	{
		LEPRA_MEASURE_SCOPE(Paint);
		if (ui_manager_->CanRender()) {
			lights_enabled = ui_manager_->GetRenderer()->GetLightsEnabled();
			ui_manager_->GetRenderer()->SetLightsEnabled(false);
			ui_manager_->Paint(true);
			for (x = slave_array_.begin(); ok && x != slave_array_.end(); ++x) {
				GameClientSlaveManager* _slave = *x;
				if (_slave) {
					ok = _slave->Paint();
				}
			}
		}
	}

	{
		LEPRA_MEASURE_SCOPE(DrawGraph);
		if (ui_manager_->CanRender()) {
			if (debug_font_id_) {
				ui_manager_->GetPainter()->GetFontManager()->SetActiveFont(debug_font_id_);
			}
			DrawDebugData();
			DrawPerformanceLineGraph2d();
			ui_manager_->GetRenderer()->SetLightsEnabled(lights_enabled);
		}
	}

	if (server_) {
		server_->PreEndTick();
	}

	{
		for (x = slave_array_.begin(); ok && x != slave_array_.end(); ++x) {
			GameClientSlaveManager* _slave = *x;
			if (_slave) {
				_slave->PreEndTick();
			}
		}
	}

	PreWaitPhysicsTick();

	WaitPhysicsTick();

	if (server_) {
		server_->EndTick();
	}

	{
		LEPRA_MEASURE_SCOPE(SlavesEndTick);
		for (x = slave_array_.begin(); ok && x != slave_array_.end(); ++x) {
			GameClientSlaveManager* _slave = *x;
			if (_slave) {
				ok = _slave->EndTick();
			}
		}
	}

	{
		LEPRA_MEASURE_SCOPE(ResourceTick);
		// This must be synchronous. The reason is that it may add objects to "script" or "physics" enginges,
		// as well as upload data to the GPU and so forth; parallelization here will certainly cause threading
		// errors.
		lock.Release();
		GetPhysicsLock()->Acquire();	// Fetch the physics lock first.
		lock.Acquire();
		for (x = slave_array_.begin(); x != slave_array_.end(); ++x) {
			GameClientSlaveManager* _slave = *x;
			if (_slave) {
				_slave->GetTickLock()->Acquire();
			}
		}
		resource_manager_->Tick();
		GetPhysicsLock()->Release();
		// OK done, now we can release all slave locks.
		for (x = slave_array_.begin(); x != slave_array_.end(); ++x) {
			GameClientSlaveManager* _slave = *x;
			if (_slave) {
				_slave->GetTickLock()->Release();
			}
		}
	}

	{
		LEPRA_MEASURE_SCOPE(UiEndRender);
		ui_manager_->EndRender(GetTimeManager()->GetNormalGameFrameTime());
	}

	{
		//LEPRA_MEASURE_SCOPE(UpdateSlaveLayout);
		if (active_width_ != ui_manager_->GetDisplayManager()->GetWidth() ||
			active_height_ != ui_manager_->GetDisplayManager()->GetHeight()) {
			active_width_ = ui_manager_->GetDisplayManager()->GetWidth();
			active_height_ = ui_manager_->GetDisplayManager()->GetHeight();
		}
		UpdateSlaveLayout();
	}

	Repair();

	ui_manager_->EndInputTick();

	if (restart_ui_) {
		if (!Reinitialize()) {
			log_.Fatal("Failure to re-initialize UI! Going down now!");
			SystemManager::AddQuitRequest(+1);
		}
		restart_ui_ = false;
	} else {
		//LEPRA_MEASURE_SCOPE(RunYieldCommand);
		console_->ExecuteYieldCommand();
	}

	return (ok);
}

void GameClientMasterTicker::PollRoundTrip() {
	// 1. Client push on network.
	// 2. Server pull on network.
	// 3. Server push on network.
	// 4. Client pull on network.
	SlaveArray::iterator x;
	for (x = slave_array_.begin(); x != slave_array_.end(); ++x) {
		GameClientSlaveManager* _slave = *x;
		if (_slave) {
			ScopeLock lock(_slave->GetTickLock());
			_slave->TickNetworkOutput();
		}
	}
	if (server_) {
		ScopeLock lock(server_->GetTickLock());
		server_->GameServerManager::TickInput();
		server_->TickNetworkOutput();
	}
	for (x = slave_array_.begin(); x != slave_array_.end(); ++x) {
		GameClientSlaveManager* _slave = *x;
		if (_slave) {
			ScopeLock lock(_slave->GetTickLock());
			_slave->TickNetworkInput();
		}
	}
}

void GameClientMasterTicker::PreWaitPhysicsTick() {
	++performance_adjustment_ticks_;
	if (performance_adjustment_ticks_ & 0x3F) {
		return;
	}
	const int adjustment_index = (performance_adjustment_ticks_ >> 7);

	bool enable_auto_performance;
	v_get(enable_auto_performance, =, UiCure::GetSettings(), kRtvarUi3DEnableautoperformance, true);
	if (!enable_auto_performance) {
		return;
	}


	double performance_load;
	v_tryget(performance_load, =, UiCure::GetSettings(), kRtvarDebugPerformanceLoad, 0.95);
	switch (adjustment_index) {
		default: {
			performance_adjustment_ticks_ = 0;
		} break;
		case 1: {
			bool enable_mass_objects;
			v_get(enable_mass_objects, =, UiCure::GetSettings(), kRtvarUi3DEnablemassobjects, true);
			bool enable_mass_object_fading;
			v_get(enable_mass_object_fading, =, UiCure::GetSettings(), kRtvarUi3DEnablemassobjectfading, true);
			if (performance_load > 1) {
				v_internal(UiCure::GetSettings(), kRtvarUi3DEnablemassobjectfading, false);
			} else if (performance_load < 0.6 && enable_mass_objects) {
				v_internal(UiCure::GetSettings(), kRtvarUi3DEnablemassobjectfading, true);
			}
		} break;
		case 2: {
			double exhaust_intensity;
			v_get(exhaust_intensity, =, UiCure::GetSettings(), kRtvarUi3DExhaustintensity, 1.0);
			if (performance_load > 0.9) {
				exhaust_intensity = Math::Lerp(exhaust_intensity, 1.9-performance_load, 0.1);
			} else if (performance_load < 0.8) {
				exhaust_intensity = Math::Lerp(exhaust_intensity, 1.0, 0.3);
			}
			v_internal(UiCure::GetSettings(), kRtvarUi3DExhaustintensity, exhaust_intensity);
		} break;
		case 3: {
			bool enable_mass_objects;
			v_get(enable_mass_objects, =, UiCure::GetSettings(), kRtvarUi3DEnablemassobjects, true);
			if (performance_load > 1) {
				v_internal(UiCure::GetSettings(), kRtvarUi3DEnablemassobjects, false);
			} else if (performance_load < 0.2) {
				v_internal(UiCure::GetSettings(), kRtvarUi3DEnablemassobjects, true);
			}
		} break;
	}
}



bool GameClientMasterTicker::StartResetUi() {
	restart_ui_ = true;
	return (true);
}

bool GameClientMasterTicker::WaitResetUi() {
	for (int x = 0; restart_ui_ && x < 50; ++x) {
		Thread::Sleep(0.1);
	}
	return (!restart_ui_);
}

bool GameClientMasterTicker::IsFirstSlave(const GameClientSlaveManager* slave) const {
	if (slave == slave_array_[0]) {	// Optimization.
		return true;
	}

	SlaveArray::const_iterator x = slave_array_.begin();
	for (; x != slave_array_.end(); ++x) {
		GameClientSlaveManager* _slave = *x;
		if (_slave == slave) {
			return true;
		}
		if (_slave) {
			return false;
		}
	}
	deb_assert(false);
	return false;
}

void GameClientMasterTicker::GetSlaveInfo(const GameClientSlaveManager* slave, int& index, int& count) const {
	int _index = 0;
	count = active_slave_count_;
	SlaveArray::const_iterator x = slave_array_.begin();
	for (; x != slave_array_.end(); ++x) {
		GameClientSlaveManager* _slave = *x;
		if (_slave == slave) {
			index = _index;
			return;
		}
		if (_slave) {
			++_index;
		}
	}
	index = 0;
}

GameClientSlaveManager* GameClientMasterTicker::GetSlave(int index) const {
	if ((int)slave_array_.size() > index) {
		return slave_array_[index];
	}
	deb_assert(false);
	return 0;
}

bool GameClientMasterTicker::IsLocalObject(cure::GameObjectId instance_id) const {
	return (local_object_set_.find(instance_id) != local_object_set_.end());
}



PixelRect GameClientMasterTicker::GetRenderArea() const {
	return (PixelRect(0, 0, ui_manager_->GetDisplayManager()->GetWidth()-1,
		ui_manager_->GetDisplayManager()->GetHeight()-1));
}

float GameClientMasterTicker::UpdateFrustum(float fov) {
	float clip_near;
	float clip_far;
	v_get(clip_near, =(float), UiCure::GetSettings(), kRtvarUi3DClipnear, 0.1);
	v_get(clip_far, =(float), UiCure::GetSettings(), kRtvarUi3DClipfar, 3000.0);
	ui_manager_->GetRenderer()->SetViewFrustum(fov, clip_near, clip_far);
	return fov;
}



void GameClientMasterTicker::PreLogin(const str& server_address) {
	const bool is_local_server = Network::IsLocalAddress(server_address);
	if (is_local_server && !server_) {
		cure::RuntimeVariableScope* variable_scope = new cure::RuntimeVariableScope(UiCure::GetSettings());
		UiGameServerManager* server = new UiGameServerManager(GetTimeManager(), variable_scope, resource_manager_, ui_manager_, PixelRect(0, 0, 100, 100));
		server->SetTicker(this);
		OnServerCreated(server);
		server->StartConsole(new uitbc::ConsoleLogListener, new uitbc::ConsolePrompt);
		if (!server->Initialize(master_connection_, server_address)) {
			delete server;
			server = 0;
		}
		if (server) {
			ScopeLock lock(&lock_);
			if (!server_) {
				server_ = server;
			} else {
				delete server;
			}
		}
	} else if (!is_local_server && server_) {
		DeleteServer();
	}
}

bool GameClientMasterTicker::IsLocalServer() const {
	return server_ != 0;
}

UiGameServerManager* GameClientMasterTicker::GetLocalServer() const {
	return server_;
}

void GameClientMasterTicker::OnExit() {
	log_.Headline("Number of players not picked, quitting.");
	SystemManager::AddQuitRequest(+1);
	CloseMainMenu();
}

void GameClientMasterTicker::OnSetPlayerCount(int player_count) {
	CloseMainMenu();

	for (int x = 0; x < player_count; ++x) {
		CreateSlave();
	}

	if (!server_ && master_connection_) {
		// If client descided we won't run local server, then we have no need for
		// a master connection any more. Was only used to download server list anyway.
		master_connection_->GraceClose(0.1, false);
	}
}

void GameClientMasterTicker::DownloadServerList() {
	if (master_connection_) {
		master_connection_->RequestServerList("");
	}
}



bool GameClientMasterTicker::CreateSlave(SlaveFactoryMethod create) {
	const PixelRect render_area(0, 0, ui_manager_->GetDisplayManager()->GetWidth(), ui_manager_->GetDisplayManager()->GetHeight());
	ScopeLock lock(&lock_);
	bool ok = (active_slave_count_ < 4);
	if (ok) {
		int free_slave_index = 0;
		for (; free_slave_index < 4; ++free_slave_index) {
			if (!slave_array_[free_slave_index]) {
				break;
			}
		}
		deb_assert(free_slave_index < 4);
		cure::RuntimeVariableScope* variables = new cure::RuntimeVariableScope(UiCure::GetSettings());
		GameClientSlaveManager* _slave = create(this, GetTimeManager(), variables, resource_manager_,
			ui_manager_, free_slave_index, render_area);
		AddSlave(_slave);
		if (initialized_) {
			ok = OpenSlave(_slave);
		}
	} else {
		log_.Error("Could not create another split screen player - not supported.");
	}
	return (ok);
}

cure::ContextObjectAttribute* GameClientMasterTicker::CreateObjectAttribute(cure::ContextObject* object, const str& attribute_name) {
	ScreenPart* screen_part = this;
	SlaveArray::iterator x;
	for (x = slave_array_.begin(); x != slave_array_.end(); ++x) {
		GameClientSlaveManager* _slave = *x;
		if (_slave) {
			if (_slave->IsOwned(object->GetInstanceId())) {
				screen_part = _slave;
				break;
			}
		}
	}
	if (strutil::StartsWith(attribute_name, "race_timer_")) {
		return new UiRaceScore(object, attribute_name, screen_part, ui_manager_, attribute_name);
	}
	return life::CreateObjectAttribute(object, attribute_name);
}

void GameClientMasterTicker::AddSlave(GameClientSlaveManager* slave) {
	{
		ScopeLock lock(&lock_);
		slave->LoadSettings();
/*#ifdef LEPRA_DEBUG
		int error_code;
		if ((error_code = slave->GetConsoleManager()->ExecuteCommand("rebuild-data")) != 0) {
			log_.Error("Unable to rebuild data in debug mode!");
			SystemManager::ExitProcess(error_code);
		}
#endif // Debug.*/
		LoadRtvars(slave->GetVariableScope());
		slave->RefreshOptions();
		deb_assert(slave_array_[slave->GetSlaveIndex()] == 0);
		slave_array_[slave->GetSlaveIndex()] = slave;
		slave->GetContext()->SetLocalRange(slave->GetSlaveIndex()+1, slave_array_.size()+1);	// Local server is first one, so skip that part in the rance.
		++active_slave_count_;
	}
}

void GameClientMasterTicker::DeleteSlave(GameClientSlaveManager* slave, bool allow_main_menu) {
	if (!slave) {
		return;
	}

	ScopeLock lock(&lock_);
	deb_assert(slave_array_[slave->GetSlaveIndex()] == slave);
	slave->SaveSettings();
	slave_array_[slave->GetSlaveIndex()] = 0;
	delete (slave);
	if (console_ && console_->GetGameManager() == slave) {
		console_->SetGameManager(0);
	}
	if (--active_slave_count_ == 0) {
		//resource_manager_->ForceFreeCache();
		if (allow_main_menu) {
			OnSlavesKilled();
		}
	}
}

void GameClientMasterTicker::DeleteServer() {
	if (server_) {
		GameServerManager* server = server_;
		server_ = 0;
		delete server;
	}
}


bool GameClientMasterTicker::Initialize() {
	bool ok = Reinitialize();
	if (ok) {
		if (!ApplyCalibration()) {
			log_.Error("An error ocurred when applying calibration.");
		}

		OnSlavesKilled();
	}
	return (ok);
}

bool GameClientMasterTicker::Reinitialize() {
	initialized_ = false;

	// Close all gfx/snd stuff.
	if (ui_manager_->GetInputManager()) {
		ui_manager_->GetInputManager()->RemoveKeyCodeInputObserver(this);
	}
	SlaveArray::iterator x;
	for (x = slave_array_.begin(); x != slave_array_.end(); ++x) {
		GameClientSlaveManager* _slave = *x;
		if (_slave) {
			_slave->Close();
		}
	}
	DeleteServer();
	console_->SetGameManager(0);
	resource_manager_->StopClear();
	ui_manager_->Close();
	SystemManager::AddQuitRequest(-1);

	// (Re)open.
	LoadRtvars(UiCure::GetSettings());
	bool ok = resource_manager_->InitDefault();
	if (ok) {
		ok = OpenUiManager();
	}
	if (ok) {
		ui_manager_->GetInputManager()->AddKeyCodeInputObserver(this);
	}
	if (ok) {
		/*ui_manager_->GetInputManager()->ActivateAll();
		ui_manager_->GetInputManager()->PollEvents();
		const uilepra::InputManager::DeviceList& lDeviceList = ui_manager_->GetInputManager()->GetDeviceList();
		uilepra::InputManager::DeviceList::const_iterator y = lDeviceList.begin();
		for (; y != lDeviceList.end(); ++y) {
			uilepra::InputDevice* device = (*y);
			log_.Infof("Found input device %s with %u digital and %u analogue input elements.",
				device->GetIdentifier().c_str(), device->GetNumDigitalElements(),
				device->GetNumAnalogueElements());
			if (device->GetNumElements() < 30) {
				log_.Info("Elements are:");
				for (unsigned e = 0; e < device->GetNumElements(); ++e) {
					uilepra::InputElement* _element = device->GetElement(e);
					str interpretation;
					switch (_element->GetInterpretation()) {
						case uilepra::InputElement::kAbsoluteAxis:	interpretation += "AbsoluteAxis";	break;
						case uilepra::InputElement::kRelativeAxis:	interpretation += "RelativeAxis";	break;
						default:					interpretation += "Button";	break;
					}
					log_.Infof("  - '%s' of type '%s' with value '%f'",
						_element->GetIdentifier().c_str(),
						interpretation.c_str(),
						_element->GetValue());
				}
			}
			device->AddFunctor(new MasterInputFunctor(this));
		}*/
		ui_manager_->GetInputManager()->AddFunctor(new MasterInputFunctor(this, &GameClientMasterTicker::OnInput));

		SlaveArray::iterator y;
		for (y = slave_array_.begin(); ok && y != slave_array_.end(); ++y) {
			GameClientSlaveManager* _slave = *y;
			if (_slave) {
				ok = OpenSlave(_slave);
			}
		}
	}
	initialized_ = ok;
	if (!ok) {
		log_.Error("Could not initialize game!");
	}
	return (ok);
}

bool GameClientMasterTicker::OpenSlave(GameClientSlaveManager* slave) {
	bool ok = slave->Open();
	if (ok && console_->GetGameManager() == 0) {
		console_->SetGameManager(slave);
	}
	// Pick a good font for the debug rendering from the slave console.
	debug_font_id_ = ((ClientConsoleManager*)slave->GetConsoleManager())->GetUiConsole()->GetFontId();
	return ok;
}

bool GameClientMasterTicker::OpenUiManager() {
	return ui_manager_->Open();
}

void GameClientMasterTicker::Repair() {
	// OpenAL/iOS bug: after some time playback stops working. We solve this by locking down the whole system,
	// re-initialize the sound system, loading all the sound resources again, then unlocking.
	if (ui_manager_->GetSoundManager()->IsIrreparableErrorState()) {
		typedef cure::ResourceManager::ResourceList ResourceList;
		ResourceList resource_list = resource_manager_->HookAllResourcesOfType("Sound");
		for (ResourceList::iterator x = resource_list.begin(); x != resource_list.end(); ++x) {
			UiCure::SoundResource* sound = (UiCure::SoundResource*)*x;
			sound->Release();
		}
		ui_manager_->GetSoundManager()->Close();
		ui_manager_->GetSoundManager()->Open();
		for (ResourceList::iterator x = resource_list.begin(); x != resource_list.end(); ++x) {
			UiCure::SoundResource* sound = (UiCure::SoundResource*)*x;
			sound->Load();
		}
		resource_manager_->UnhookResources(resource_list);
	}
}

void GameClientMasterTicker::UpdateSlaveLayout() {
	if (!initialized_) {
		return;
	}

	int averaged_slaves = 1;
	float fps;
	v_get(fps, =(float), UiCure::GetSettings(), kRtvarPhysicsFps, 30);
	float frame_time = 1/fps;
	for (int x = 0; x < 4; ++x) {
		if (slave_array_[x]) {
			frame_time += GetTimeManager()->GetNormalFrameTime();
			++averaged_slaves;
		}
	}
	frame_time /= averaged_slaves;
	const float layout_speed = Math::GetIterateLerpTime(0.7f, frame_time);

	GameClientSlaveManager* last_slave = 0;
	if (active_slave_count_ == 1) {
		for (int x = 0; x < 4; ++x) {
			if (slave_array_[x] && slave_array_[x]->IsQuitting()) {
				last_slave = slave_array_[x];
				break;
			}
		}
	}
	if (!last_slave) {
		slave_top_split_ = Math::Lerp(slave_top_split_, (GetSlaveAnimationTarget(0)-GetSlaveAnimationTarget(1)) * 0.5f + 0.5f, layout_speed);
		slave_bottom_split_ = Math::Lerp(slave_bottom_split_, (GetSlaveAnimationTarget(2)-GetSlaveAnimationTarget(3)) * 0.5f + 0.5f, layout_speed);
		slave_v_split_ = Math::Lerp(slave_v_split_, GetSlavesVerticalAnimationTarget(), layout_speed);
		slave_fade_ = 0;
		SlideSlaveLayout();
	} else {
		slave_fade_ = Math::Lerp(slave_fade_, 1.0f, layout_speed);
		if (slave_fade_ < 1 - 1e-5f) {
			last_slave->SetFade(slave_fade_);
		} else {
			DeleteSlave(last_slave, true);
		}
	}
}

void GameClientMasterTicker::SlideSlaveLayout() {
	const PixelRect render_area(0, 0, ui_manager_->GetDisplayManager()->GetWidth(), ui_manager_->GetDisplayManager()->GetHeight());
	if (server_) {
		server_->SetRenderArea(render_area);
	}
	const float render_areas[][4] =
	{
		{ 0, 0, slave_top_split_, slave_v_split_, },
		{ slave_top_split_, 0, 1, slave_v_split_, },
		{ 0, slave_v_split_, slave_bottom_split_, 1, },
		{ slave_bottom_split_, slave_v_split_, 1, 1, },
	};
	for (int x = 0; x < 4; ++x) {
		if (slave_array_[x] == 0) {
			continue;
		}

		PixelRect part_render_area(
			(int)(render_area.GetWidth()  * render_areas[x][0]),
			(int)(render_area.GetHeight() * render_areas[x][1]),
			(int)(render_area.GetWidth()  * render_areas[x][2]),
			(int)(render_area.GetHeight() * render_areas[x][3])
			);
		const int REQUIRED_PIXELS = 4;
		if (part_render_area.left_ <= REQUIRED_PIXELS) {	// Hatch onto edge?
			part_render_area.left_ = 0;
		} else {	// Do we want a border to the left?
			part_render_area.left_ += (int)(render_areas[x][0] * 4);
		}
		if (part_render_area.GetWidth() >= render_area.GetWidth()-REQUIRED_PIXELS) {		// Hatch onto edge?
			part_render_area.right_ = render_area.right_;
		}
		if (part_render_area.top_ <= REQUIRED_PIXELS) {	// Hatch onto edge?
			part_render_area.top_ = 0;
		} else {	// Do we want a border to the right?
			part_render_area.top_ += (int)(render_areas[x][1] * 4);
		}
		if (part_render_area.GetHeight() >= render_area.GetHeight()-REQUIRED_PIXELS) {	// Hatch onto edge?
			part_render_area.bottom_ = render_area.bottom_;
		}

		if (part_render_area.GetWidth() < REQUIRED_PIXELS || part_render_area.GetHeight() < REQUIRED_PIXELS) {
			if (slave_array_[x]->IsQuitting()) {
				DeleteSlave(slave_array_[x], true);
			}
		} else {
			slave_array_[x]->SetRenderArea(part_render_area);
		}
	}
}

int GameClientMasterTicker::GetSlaveAnimationTarget(int slave_index) const {
	deb_assert(slave_index >= 0 && slave_index < 4);
	GameClientSlaveManager* _slave = slave_array_[slave_index];
	return (_slave && !_slave->IsQuitting())? 1 : 0;
}

float GameClientMasterTicker::GetSlavesVerticalAnimationTarget() const {
	const int top = GetSlaveAnimationTarget(0) + GetSlaveAnimationTarget(1);
	const int bottom = GetSlaveAnimationTarget(2) + GetSlaveAnimationTarget(3);
	if (top == 0 && bottom == 0) {
		return (1.0f);
	}

	float top_scale = 0;
	if (top == 1) {
		top_scale = 0.6f;
	} else if (top == 2) {
		top_scale = 1.0f;
	}
	float bottom_scale = 0;
	if (bottom == 1) {
		bottom_scale = 0.6f;
	} else if (bottom == 2) {
		bottom_scale = 1.0f;
	}
	return (top_scale / (top_scale+bottom_scale));
}

void GameClientMasterTicker::MeasureLoad() {
	const ScopePerformanceData* main_loop = ScopePerformanceData::GetRoots()[0];
	const ScopePerformanceData* app_sleep = main_loop->GetChild("AppSleep");
	if (app_sleep) {
		int target_frame_rate;
		v_get(target_frame_rate, =, cure::GetSettings(), kRtvarPhysicsFps, 2);
		const double current_performance_load = (main_loop->GetSlidingAverage()-app_sleep->GetSlidingAverage()) * target_frame_rate;
		double average_performance_load;
		v_tryget(average_performance_load, =, UiCure::GetSettings(), kRtvarDebugPerformanceLoad, 0.95);
		if (current_performance_load < 20) {
			average_performance_load = Math::Lerp(average_performance_load, current_performance_load, 0.05);
		}
		v_internal(UiCure::GetSettings(), kRtvarDebugPerformanceLoad, average_performance_load);
	}
}

void GameClientMasterTicker::Profile() {
	bool debug_graph;
	v_get(debug_graph, =, UiCure::GetSettings(), kRtvarDebugPerformanceGraph, false);
	if (!debug_graph) {
		return;
	}

	const int height = 100;
	typedef ScopePerformanceData::NodeArray ScopeArray;
	typedef std::pair<ScopePerformanceData*, int> ScopeLevel;
	typedef std::vector<ScopeLevel> ScopeLevelArray;
	ScopeArray roots = ScopePerformanceData::GetRoots();
	ScopeLevelArray stacked_nodes;
	stacked_nodes.reserve(100);	// Optimization.
	str name;
	name.reserve(200);	// Optimization;
	for (size_t root_index = 0; root_index < roots.size(); ++root_index) {
		if (performance_graph_list_.size() <= root_index) {
			performance_graph_list_.push_back(UiCure::LineGraph2d(ui_manager_->GetPainter()));
		}
		performance_graph_list_[root_index].TickLine(height);

		const double total_percent_factor = 100/roots[root_index]->GetSlidingAverage();
		ScopeLevel current_node(roots[root_index], 0);
		const double root_start = current_node.first->GetTimeOfLastMeasure();
		for (;;) {
			ScopePerformanceData* node = current_node.first;
			const ScopeArray& children = node->GetChildren();
			for (size_t y = 0; y < children.size(); ++y) {
				stacked_nodes.push_back(ScopeLevel(children[y], current_node.second+1));
			}

			// Just multiply by number of times hit. Not perfect. Good enough but best?
			const double total_estimated_duration = node->GetLast() * node->GetHitCount();
			const double previous_estimated_duration = node->GetLast() * (node->GetHitCount() - 1);
			double start = node->GetTimeOfLastMeasure() - root_start - previous_estimated_duration;
			start = (start < 0)? 0 : start;
			name  = str(current_node.second, ' ');
			name += node->GetName();
			name += " (";
			name += strutil::DoubleToString(node->GetRangeFactor()*100, 1);
			name += " % fluctuation, ";
			name += strutil::DoubleToString(node->GetSlidingAverage()*node->GetHitCount()*total_percent_factor, 1);
			name += " % total time)";
			performance_graph_list_[root_index].AddSegment(name, start, start + total_estimated_duration);
			node->ResetHitCount();

			if (!stacked_nodes.empty()) {
				current_node = stacked_nodes.back();
				stacked_nodes.pop_back();
			} else {
				break;
			}
		}
	}
}

void GameClientMasterTicker::PhysicsTick() {
	Parent::PhysicsTick();

	for (SlaveArray::iterator x = slave_array_.begin(); x != slave_array_.end(); ++x) {
		GameClientSlaveManager* _slave = *x;
		if (_slave) {
			_slave->TickNetworkOutputGhosts();
		}
	}
}

void GameClientMasterTicker::WillMicroTick(float time_delta) {
	for (SlaveArray::iterator x = slave_array_.begin(); x != slave_array_.end(); ++x) {
		GameClientSlaveManager* _slave = *x;
		if (_slave) {
			_slave->MicroTick(time_delta);
		}
	}
	if (server_) {
		server_->MicroTick(time_delta);
	}
}

void GameClientMasterTicker::DidPhysicsTick() {
	for (SlaveArray::iterator x = slave_array_.begin(); x != slave_array_.end(); ++x) {
		GameClientSlaveManager* _slave = *x;
		if (_slave) {
			_slave->PostPhysicsTick();
		}
	}
	if (server_) {
		server_->PostPhysicsTick();
	}
}

void GameClientMasterTicker::BeginRender(vec3& color) {
	ui_manager_->BeginRender(color);

	local_object_set_.clear();
	SlaveArray::iterator x;
	for (x = slave_array_.begin(); x != slave_array_.end(); ++x) {
		GameClientSlaveManager* _slave = *x;
		if (_slave) {
			_slave->AddLocalObjects(local_object_set_);
		}
	}
}

void GameClientMasterTicker::DrawDebugData() const {
	bool debugging;
	v_get(debugging, =, UiCure::GetSettings(), kRtvarDebugEnable, false);
	if (!debugging) {
		return;
	}

	const ScopePerformanceData* main_loop = ScopePerformanceData::GetRoots()[0];
	wstr info = wstrutil::Format(L"FPS %.1f", 1/main_loop->GetSlidingAverage());
	double load;
	v_tryget(load, =, UiCure::GetSettings(), kRtvarDebugPerformanceLoad, -1.0);
	info += wstrutil::Format(L"\nUsedPerf %2.f %%", 100 * load + 0.5f);
	int w = 110;
	int h = 37;
	bool show_performance_counters;
	v_get(show_performance_counters, =, UiCure::GetSettings(), kRtvarDebugPerformanceCount, false);
	if (show_performance_counters) {
		w = 130;
		double up_bandwidth = 0;
		double down_bandwidth = 0;
		for (int x = 0; x < 4; ++x) {
			if (slave_array_[x]) {
				cure::GameManager::BandwidthData up;
				cure::GameManager::BandwidthData down;
				slave_array_[x]->GetBandwidthData(up, down);
				up_bandwidth += up.GetLast();
				down_bandwidth += down.GetLast();
			}
		}

		info += wstrutil::Format(L"\nvTRI %i\ncTRI %i\nUpload %sB/s\nDownload %sB/s",
			ui_manager_->GetRenderer()->GetTriangleCount(true),
			ui_manager_->GetRenderer()->GetTriangleCount(false),
			wstrutil::Encode(Number::ConvertToPostfixNumber(up_bandwidth, 1)).c_str(),
			wstrutil::Encode(Number::ConvertToPostfixNumber(down_bandwidth, 1)).c_str());
		h += 17*4;
	}
	ui_manager_->GetPainter()->SetColor(Color(0, 0, 0));
	const int right = ui_manager_->GetDisplayManager()->GetWidth();
	ui_manager_->GetPainter()->FillRect(right-w, 3, right-5, h);
	ui_manager_->GetPainter()->SetColor(Color(200, 200, 0));
	ui_manager_->GetPainter()->PrintText(info, right-w+5, 5);
}

void GameClientMasterTicker::DrawPerformanceLineGraph2d() const {
	bool debug;
	bool debug_graph;
	v_get(debug, =, UiCure::GetSettings(), kRtvarDebugEnable, false);
	v_get(debug_graph, =, UiCure::GetSettings(), kRtvarDebugPerformanceGraph, false);
	if (!debug || !debug_graph) {
		return;
	}

	// Draw all scope nodes as line segments in one hunky graph.
	typedef ScopePerformanceData::NodeArray ScopeArray;

	float longest_root_time = 1e-15f;
	ScopeArray roots = ScopePerformanceData::GetRoots();
	for (size_t root_index = 0; root_index < roots.size(); ++root_index) {
		const float root_delta = (float)roots[root_index]->GetMaximum();
		if (root_delta > longest_root_time) {
			longest_root_time = root_delta;
		}
	}

	const int margin = 10;
	const float scale = (ui_manager_->GetDisplayManager()->GetWidth() - margin*2)/longest_root_time;
	int y_offset;
	v_get(y_offset, =, UiCure::GetSettings(), kRtvarDebugPerformanceYoffset, 0);
	int __y = margin + y_offset;
	for (size_t root_index = 0; root_index < roots.size(); ++root_index) {
		if (performance_graph_list_.size() <= root_index) {
			return;
		}

		performance_graph_list_[root_index].Render(margin, scale, __y);

		bool debug_names;
		v_get(debug_names, =, UiCure::GetSettings(), kRtvarDebugPerformanceNames, false);
		if (debug_names) {
			performance_graph_list_[root_index].RenderNames(margin, __y);
		}
	}

}



float GameClientMasterTicker::GetTickTimeReduction() const {
	return GetTimeManager()->GetTickLoopTimeReduction();
}

float GameClientMasterTicker::GetPowerSaveAmount() const {
	bool is_minimized = !ui_manager_->GetDisplayManager()->IsVisible();
	return (is_minimized? 0.2f : 0);
}



void GameClientMasterTicker::OnTrigger(tbc::PhysicsManager::BodyID trigger, int trigger_listener_id, int other_object_id, tbc::PhysicsManager::BodyID body_id, const vec3& position, const vec3& normal) {
	for (SlaveArray::iterator x = slave_array_.begin(); x != slave_array_.end(); ++x) {
		GameClientSlaveManager* _slave = *x;
		if (_slave) {
			_slave->OnTrigger(trigger, trigger_listener_id, other_object_id, body_id, position, normal);
		}
	}
	if (server_) {
		server_->OnTrigger(trigger, trigger_listener_id, other_object_id, body_id, position, normal);
	}
}

void GameClientMasterTicker::OnForceApplied(int object_id, int other_object_id, tbc::PhysicsManager::BodyID body_id, tbc::PhysicsManager::BodyID other_body_id,
		const vec3& force, const vec3& torque, const vec3& position, const vec3& relative_velocity) {
	for (SlaveArray::iterator x = slave_array_.begin(); x != slave_array_.end(); ++x) {
		GameClientSlaveManager* _slave = *x;
		if (_slave) {
			_slave->OnForceApplied(object_id, other_object_id, body_id, other_body_id, force, torque, position, relative_velocity);
		}
	}
	if (server_) {
		server_->OnForceApplied(object_id, other_object_id, body_id, other_body_id, force, torque, position, relative_velocity);
	}
}



int GameClientMasterTicker::OnCommandLocal(const str& command, const strutil::strvec& parameter_vector) {
	if (command == kSetPlayerCount) {
		int _player_count = 0;
		if (parameter_vector.size() == 1 && strutil::StringToInt(parameter_vector[0], _player_count)) {
			if (_player_count >= 1 && _player_count <= 4) {
				OnSetPlayerCount(_player_count);
				return (0);
			} else {
				log_.Error("player count must lie between 1 and 4");
			}
		} else {
			log_.Warningf("usage: %s <no. of players>", command.c_str());
		}
		return (1);
	}
	return (-1);
}

void GameClientMasterTicker::OnCommandError(const str&, const strutil::strvec&, int) {
}



bool GameClientMasterTicker::OnKeyDown(uilepra::InputManager::KeyCode key_code) {
	bool consumed = false;

	if (server_) {
		consumed = server_->OnKeyDown(key_code);
	}

	SlaveArray::iterator x = slave_array_.begin();
	for (; !consumed && x != slave_array_.end(); ++x) {
		GameClientSlaveManager* _slave = *x;
		if (_slave) {
			consumed = _slave->OnKeyDown(key_code);
		}
	}
	return (consumed);
}

bool GameClientMasterTicker::OnKeyUp(uilepra::InputManager::KeyCode key_code) {
	bool consumed = false;
	if (server_) {
		consumed = server_->OnKeyUp(key_code);
	}
	SlaveArray::iterator x = slave_array_.begin();
	for (; !consumed && x != slave_array_.end(); ++x) {
		GameClientSlaveManager* _slave = *x;
		if (_slave) {
			consumed = _slave->OnKeyUp(key_code);
		}
	}
	return (consumed);
}

void GameClientMasterTicker::OnInput(uilepra::InputElement* element) {
	SlaveArray::iterator x = slave_array_.begin();
	for (; x != slave_array_.end(); ++x) {
		GameClientSlaveManager* _slave = *x;
		if (_slave) {
			_slave->OnInput(element);
		}
	}
}



bool GameClientMasterTicker::ApplyCalibration() {
	bool ok = true;
	const uilepra::InputManager::DeviceList& devices = ui_manager_->GetInputManager()->GetDeviceList();
	uilepra::InputManager::DeviceList::const_iterator x = devices.begin();
	for (; x != devices.end(); ++x) {
		uilepra::InputDevice* device = *x;
		str device_id = strutil::ReplaceAll(device->GetIdentifier(), ' ', '_');
		device_id = strutil::ReplaceAll(device_id, '.', '_');
		uilepra::InputDevice::CalibrationData calibration;

		const std::list<str> variable_names = UiCure::GetSettings()->GetVariableNameList(cure::RuntimeVariableScope::kSearchAll);
		std::list<str>::const_iterator y = variable_names.begin();
		for (; y != variable_names.end(); ++y) {
			const str& var_name = *y;
			const strutil::strvec var_names = strutil::Split(var_name, ".", 2);
			if (var_names.size() != 3) {
				continue;
			}
			if (var_names[0] == "Calibration" && var_names[1] == device_id) {
				str value = UiCure::GetSettings()->GetUntypedDefaultValue(cure::RuntimeVariableScope::kReadOnly, var_name);
				calibration.push_back(uilepra::InputDevice::CalibrationElement(var_names[2], value));
			}
		}
		ok &= device->SetCalibration(calibration);
	}
	return (ok);
}

void GameClientMasterTicker::StashCalibration() {
	if (!ui_manager_->GetInputManager()) {
		return;
	}

	const uilepra::InputManager::DeviceList& devices = ui_manager_->GetInputManager()->GetDeviceList();
	uilepra::InputManager::DeviceList::const_iterator x = devices.begin();
	for (; x != devices.end(); ++x) {
		uilepra::InputDevice* device = *x;
		str device_id = strutil::ReplaceAll(device->GetIdentifier(), ' ', '_');
		device_id = strutil::ReplaceAll(device_id, '.', '_');
		const uilepra::InputDevice::CalibrationData calibration = device->GetCalibration();

		uilepra::InputDevice::CalibrationData::const_iterator y = calibration.begin();
		for (; y != calibration.end(); ++y) {
			const uilepra::InputDevice::CalibrationElement& _element = *y;
			UiCure::GetSettings()->SetValue(cure::RuntimeVariable::kUsageNormal,
				"Calibration."+device_id+"."+_element.first, _element.second);
		}
	}
}



loginstance(kGame, GameClientMasterTicker);



}
