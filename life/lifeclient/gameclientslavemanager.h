
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../cure/include/gamemanager.h"
#include "../../cure/include/packet.h"
#include "../../cure/include/positionaldata.h"
#include "../../cure/include/useraccount.h"
#include "../../lepra/include/alarm.h"
#include "../../lepra/include/graphics2d.h"
#include "clientoptionsmanager.h"
#include "inputobserver.h"
#include "screenpart.h"



namespace cure {
class NetworkClient;
class ResourceManager;
}
namespace UiCure {
class GameUiManager;
}



namespace life {



class GameClientMasterTicker;
class MasterServerConnection;



class GameClientSlaveManager: public cure::GameManager, public InputObserver, public ScreenPart {
public:
	typedef cure::GameManager Parent;
	GameClientSlaveManager(GameClientMasterTicker* pMaster, const cure::TimeManager* time,
		cure::RuntimeVariableScope* variable_scope, cure::ResourceManager* resource_manager,
		UiCure::GameUiManager* ui_manager, int slave_index, const PixelRect& render_area);
	virtual ~GameClientSlaveManager();
	void SetMasterServerConnection(MasterServerConnection* connection);
	virtual void Resume(bool hard);
	virtual void Suspend(bool hard);
	virtual void LoadSettings();
	virtual void SaveSettings() = 0;
	virtual void RefreshOptions();
	virtual void SetRenderArea(const PixelRect& render_area);
	virtual bool Open();
	virtual void Close();
	bool IsQuitting() const;
	virtual void SetIsQuitting();
	virtual void SetFade(float fade_amount) = 0;

	GameClientMasterTicker* GetMaster() const;

	virtual bool Render();
	virtual bool Paint();
	virtual bool IsPrimaryManager() const;
	virtual void PreEndTick();
	virtual bool EndTick();
	void TickNetworkInput();
	bool TickNetworkOutput();
	void TickNetworkOutputGhosts();

	void ToggleConsole();

	virtual void RequestLogin(const str& server_address, const cure::LoginId& login_token);
	virtual void OnLoginSuccess();
	void Logout();
	bool IsLoggingIn() const;
	bool IsUiMoveForbidden(cure::GameObjectId object_id) const;
	void AddLocalObjects(std::unordered_set<cure::GameObjectId>& local_object_set);
	virtual bool IsOwned(cure::GameObjectId object_id) const;

	virtual bool OnKeyDown(uilepra::InputManager::KeyCode key_code);
	virtual bool OnKeyUp(uilepra::InputManager::KeyCode key_code);
	virtual void OnInput(uilepra::InputElement* element);
	virtual void HandleUnusedRelativeAxis();

	int GetSlaveIndex() const;

	virtual PixelRect GetRenderArea() const;
	virtual float UpdateFrustum(float fov);

protected:
	typedef std::vector<cure::GameObjectId> ObjectArray;
	typedef std::unordered_set<uilepra::InputElement*, LEPRA_VOIDP_HASHER> InputElementSet;
	enum SteeringPlaybackMode {
		kPlaybackNone = 0,
		kPlaybackRecord,
		kPlaybackPlay,
	};

	str GetApplicationCommandFilename() const;

	virtual bool Reset();
	virtual bool InitializeUniverse() = 0;

	virtual void TickUiUpdate() = 0;
	virtual void SetLocalRender(bool render) = 0;

	virtual void ProcessNetworkInputMessage(cure::Message* message);
	virtual void ProcessNetworkStatusMessage(cure::MessageStatus* message);
	virtual void ProcessNumber(cure::MessageNumber::InfoType type, int32 integer, float32 f);
	cure::ContextObject* CreateObject(cure::GameObjectId instance_id, const str& class_id, cure::NetworkObjectType network_type,
		xform* transform = 0);
	void SetMovement(cure::GameObjectId instance_id, int32 frame_index, cure::ObjectPositionalData& data, float delta_threshold);
	virtual bool OnPhysicsSend(cure::ContextObject* object);
	virtual bool OnAttributeSend(cure::ContextObject* object);
	bool IsServer();
	void SendAttach(cure::ContextObject*, unsigned, cure::ContextObject*, unsigned);
	void SendDetach(cure::ContextObject*, cure::ContextObject*);
	void OnIdOwnershipExpired(int, cure::ContextObject* object, void*);
	void AttachObjects(cure::GameObjectId object1_id, unsigned body1_index, cure::GameObjectId object2_id, unsigned body2_index);
	void DetachObjects(cure::GameObjectId object1_id, cure::GameObjectId object2_id);

	cure::NetworkClient* GetNetworkClient() const;

	virtual void UpdateCameraPosition(bool update_mic_position) = 0;

	virtual void DrawAsyncDebugInfo();
	void DrawDebugStaple(int index, int height, const Color& color);
	virtual void DrawSyncDebugInfo();

	typedef std::unordered_map<cure::GameObjectId, int> ObjectFrameIndexMap;
	typedef std::unordered_set<cure::GameObjectId> ObjectIdSet;

	UiCure::GameUiManager* ui_manager_;
	const int slave_index_;
	PixelRect render_area_;

	MasterServerConnection* master_server_connection_;

	ObjectFrameIndexMap object_frame_index_map_;

	bool is_reset_;
	bool is_reset_complete_;
	bool quit_;

	ObjectIdSet owned_object_list_;
	uint64 last_sent_byte_count_;
	Timer last_send_time_;
	int ping_attempt_count_;
	Timer last_unsafe_receive_time_;
	Alarm collision_expire_alarm_;
	Alarm input_expire_alarm_;
	Alarm send_expire_alarm_;

	InputElementSet relative_axis_;
	InputElementSet unused_relative_axis_;

	bool allow_movement_input_;
	options::ClientOptionsManager options_;
	str connect_user_name_;
	str connect_server_address_;
	str disconnect_reason_;

	logclass();
};



}
