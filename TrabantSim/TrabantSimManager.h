
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Lepra/Include/Plane.h"
#include "../Life/LifeClient/GameClientSlaveManager.h"
#include "../Life/LifeClient/Menu.h"
#include "../UiCure/Include/UiResourceManager.h"
#include "../UiLepra/Include/UiTouchDrag.h"
#include "PhysGfxObject.h"
#include "TrabantSim.h"



namespace Lepra
{
class TcpListenerSocket;
}
namespace UiCure
{
class CollisionSoundManager;
class CppContextObject;
class SoundReleaser;
}
namespace UiTbc
{
class Button;
}


namespace TrabantSim
{



class Object;
class Light;



class TrabantSimManager: public Life::GameClientSlaveManager
{
	typedef Life::GameClientSlaveManager Parent;
public:
	struct CollisionInfo
	{
		int mObjectId;
		vec3 mForce;
		vec3 mPosition;
		int mOtherObjectId;
	};
	struct JoystickData
	{
		int mJoystickId;
		float x;
		float y;
	};
	typedef std::vector<CollisionInfo> CollisionList;
	typedef UiLepra::Touch::DragManager::DragList DragList;
	typedef std::vector<JoystickData> JoystickDataList;

	TrabantSimManager(Life::GameClientMasterTicker* pMaster, const Cure::TimeManager* pTime,
		Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager,
		UiCure::GameUiManager* pUiManager, int pSlaveIndex, const PixelRect& pRenderArea);
	virtual ~TrabantSimManager();

	void UserReset();
	int CreateObject(const MeshObject& pGfxObject, const PhysObjectArray& pPhysObjects, bool pIsStatic);
	void DeleteObject(int pObjectId);
	void DeleteAllObjects();
	bool IsLoaded(int pObjectId);
	void Expload(const vec3& pPos, const vec3& pVel);
	void PlaySound(const str& pSound, const vec3& pPos, const vec3& pVel);
	CollisionList PopCollisions();
	const DragList& GetTouchDrags() const;
	vec3 GetAccelerometer() const;
	int CreateJoystick(float x, float y);
	JoystickDataList GetJoystickData() const;
	float GetAspectRatio() const;
	int CreateEngine(int pObjectId, const str& pEngineType, const vec2& pMaxVelocity, const str& pEngineSound);
	int CreateJoint(int pObjectId, const str& pJointType, int pOtherObjectId, const vec3& pAxis);
	void Position(int pObjectId, bool pSet, vec3& pPosition);
	void Orientation(int pObjectId, bool pSet, quat& pOrientation);
	void Velocity(int pObjectId, bool pSet, vec3& pVelocity);
	void AngularVelocity(int pObjectId, bool pSet, vec3& pAngularVelocity);
	void Mass(int pObjectId, bool pSet, float& pMass);
	void ObjectColor(int pObjectId, bool pSet, vec3& pColor);
	void EngineForce(int pObjectId, int pEngineIndex, bool pSet, vec3& pForce);

	void AcceptLoop();
	void CommandLoop();
	bool IsControlled() const;

	virtual void SaveSettings();
	virtual bool Open();
	virtual void Close();
	virtual void SetIsQuitting();
	virtual void SetFade(float pFadeAmount);

	Cure::RuntimeVariableScope* GetVariableScope() const;

protected:
	virtual bool InitializeUniverse();
	virtual void TickInput();
	virtual void TickUiInput();
	virtual void TickUiUpdate();
	virtual void SetLocalRender(bool pRender);

	virtual Cure::ContextObject* CreateContextObject(const str& pClassId) const;
	virtual void OnLoadCompleted(Cure::ContextObject* pObject, bool pOk);
	void OnCollision(const vec3& pForce, const vec3& pTorque, const vec3& pPosition,
		Cure::ContextObject* pObject1, Cure::ContextObject* pObject2,
		Tbc::PhysicsManager::BodyID pBody1Id, Tbc::PhysicsManager::BodyID pBody2Id);

	void OnPauseButton(UiTbc::Button*);
	void OnMenuAlternative(UiTbc::Button* pButton);

	virtual void ScriptPhysicsTick();
	virtual void MoveCamera(float pFrameTime);
	virtual void UpdateCameraPosition(bool pUpdateMicPosition);
	virtual void UpdateUserMessage();

	void PrintText(const str& s, int x, int y) const;
	void DrawImage(UiTbc::Painter::ImageID pImageId, float cx, float cy, float w, float h, float pAngle) const;

	void PainterImageLoadCallback(UiCure::UserPainterKeepImageResource* pResource);

	UiCure::CollisionSoundManager* mCollisionSoundManager;
	std::set<Cure::GameObjectId> mObjects;
	Life::Menu* mMenu;
	Light* mLight;
	vec3 mCameraAngle;
	xform mCameraTransform;
	UiTbc::Button* mPauseButton;
	TcpListenerSocket* mListenerSocket;
	TcpSocket* mConnectSocket;
	MemberThread<TrabantSimManager>* mAcceptThread;
	MemberThread<TrabantSimManager>* mCommandThread;
	UiTbc::Dialog* mUserInfoDialog;
	UiTbc::Label* mUserInfoLabel;
	logclass();
};



}
