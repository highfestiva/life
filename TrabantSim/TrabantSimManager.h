
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Life/LifeClient/GameClientSlaveManager.h"
#include <map>
#include "../Lepra/Include/SocketAddress.h"
#include "../Life/LifeClient/Menu.h"
#include "../UiCure/Include/UiResourceManager.h"
#include "../UiLepra/Include/UiTouchDrag.h"
#include "PhysGfxObject.h"
#include "TrabantSim.h"



namespace Lepra
{
class UdpSocket;
}
namespace UiLepra
{
namespace Touch
{
class TouchstickInputDevice;
}
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



class FileServer;
class Light;
class Object;



class TrabantSimManager: public Life::GameClientSlaveManager
{
	typedef Life::GameClientSlaveManager Parent;
public:
	typedef UiLepra::Touch::TouchstickInputDevice Touchstick;
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
		JoystickData(int pJoystickId, float px, float py);
	};
	struct TouchstickInfo
	{
		Touchstick* mStick;
		float x;
		float y;
		int mOrientation;
		bool mIsSloppy;
		TouchstickInfo(Touchstick* pStick, float px, float py, int pOrientation, bool pIsSloppy);
	};
	struct EngineTarget
	{
		int mInstanceId;
		float mStrength;
		EngineTarget(int pInstanceId, float pStrength);
	};
	struct Drag
	{
		PixelCoord mStart;
		PixelCoord mLast;
		vec2 mVelocity;
		HiResTimer mTimer;
		bool mIsPress;
		int mButtonMask;
	};
	typedef std::list<CollisionInfo> CollisionList;
	typedef std::vector<Drag> DragList;
	typedef std::vector<PixelCoord> DragEraseList;
	typedef UiLepra::Touch::DragManager::DragList UiDragList;
	typedef std::vector<JoystickData> JoystickDataList;
	typedef std::vector<TouchstickInfo> TouchstickList;
	typedef std::vector<EngineTarget> EngineTargetList;
	typedef std::vector<float> FloatList;
	typedef std::vector<int> IntList;
	typedef std::vector<str> StringList;
	typedef std::vector<vec3> Vec3List;
	typedef std::vector<xform> XformList;

	TrabantSimManager(Life::GameClientMasterTicker* pMaster, const Cure::TimeManager* pTime,
		Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager,
		UiCure::GameUiManager* pUiManager, int pSlaveIndex, const PixelRect& pRenderArea);
	virtual ~TrabantSimManager();

	virtual void Resume(bool pHard);
	virtual void Suspend(bool pHard);

	void UserReset();
	int CreateObject(const quat& pOrientation, const vec3& pPosition, const MeshObject& pGfxObject, const PhysObjectArray& pPhysObjects, ObjectMaterial pMaterial, bool pIsStatic);
	void CreateClones(IntList& pCreatedObjectIds, int pOriginalId, const XformList& pPlacements, ObjectMaterial pMaterial, bool pIsStatic);
	void DeleteObject(int pObjectId);
	void DeleteAllObjects();
	void PickObjects(const vec3& pPosition, const vec3& pDirection, const vec2& pRange, IntList& pPickedObjectIds, Vec3List& pPickedPositions);
	bool IsLoaded(int pObjectId);
	void Explode(const vec3& pPos, const vec3& pVel, float pStrength);
	void PlaySound(const str& pSound, const vec3& pPos, const vec3& pVel, float pVolume);
	void PopCollisions(CollisionList& pCollisionList);
	void GetKeys(strutil::strvec& pKeys);
	void GetTouchDrags(DragList& pDragList);
	vec3 GetAccelerometer() const;
	vec3 GetMouseMove();
	int CreateJoystick(float x, float y, bool pIsSloppy);
	JoystickDataList GetJoystickData() const;
	float GetAspectRatio() const;
	int CreateEngine(int pObjectId, const str& pEngineType, const vec2& pMaxVelocity, float pStrength, float pFriction, const EngineTargetList& pEngineTargets);
	int CreateJoint(int pObjectId, const str& pJointType, int pOtherObjectId, const vec3& pAxis, const vec2& pStop, const vec2& pSpringSettings);
	void Position(int pObjectId, bool pSet, vec3& pPosition);
	void Orientation(int pObjectId, bool pSet, quat& pOrientation);
	void Velocity(int pObjectId, bool pSet, vec3& pVelocity);
	void AngularVelocity(int pObjectId, bool pSet, vec3& pAngularVelocity);
	void Mass(int pObjectId, bool pSet, float& pMass);
	void ObjectColor(int pObjectId, bool pSet, vec3& pColor, float pAlpha);
	void EngineForce(int pObjectId, int pEngineIndex, bool pSet, vec3& pForce);
	void AddTag(int pObjectId, const str& pTagType, const FloatList& pFloats, const StringList& pStrings, const IntList& pPhys, const IntList& pEngines, const IntList& pMeshes);

	void CommandLoop();
	bool IsControlled();

	virtual void SaveSettings();
	virtual void SetRenderArea(const PixelRect& pRenderArea);
	virtual bool Open();
	virtual void Close();
	void CloseConnection();
	virtual void SetIsQuitting();
	virtual void SetFade(float pFadeAmount);

	virtual bool Paint();
	void DrawStick(Touchstick* pStick, bool pIsSloppy);

	Cure::RuntimeVariableScope* GetVariableScope() const;

protected:
	typedef std::map<UiLepra::InputManager::KeyCode,bool> KeyMap;

	virtual bool InitializeUniverse();
	virtual void TickInput();
	void UpdateTouchstickPlacement();
	virtual void TickUiInput();
	virtual void TickUiUpdate();
	virtual void SetLocalRender(bool pRender);

	void AddCheckerTexturing(UiTbc::TriangleBasedGeometry* pMesh, float pScale);

	virtual Cure::ContextObject* CreateContextObject(const str& pClassId) const;
	virtual void OnLoadCompleted(Cure::ContextObject* pObject, bool pOk);
	void OnCollision(const vec3& pForce, const vec3& pTorque, const vec3& pPosition,
		Cure::ContextObject* pObject1, Cure::ContextObject* pObject2,
		Tbc::PhysicsManager::BodyID pBody1Id, Tbc::PhysicsManager::BodyID pBody2Id);

	void OnPauseButton(UiTbc::Button*);
	void OnBackButton(UiTbc::Button*);
	void OnMenuAlternative(UiTbc::Button* pButton);

	virtual void ScriptPhysicsTick();
	virtual void MoveCamera(float pFrameTime);
	virtual void UpdateCameraPosition(bool pUpdateMicPosition);
	virtual void UpdateUserMessage();

	void PrintText(const str& s, int x, int y) const;
	void DrawImage(UiTbc::Painter::ImageID pImageId, float cx, float cy, float w, float h, float pAngle) const;

	virtual bool OnKeyDown(UiLepra::InputManager::KeyCode pKeyCode);
	virtual bool OnKeyUp(UiLepra::InputManager::KeyCode pKeyCode);
	virtual void OnInput(UiLepra::InputElement* pElement);
	void PainterImageLoadCallback(UiCure::UserPainterKeepImageResource* pResource);

	UiCure::CollisionSoundManager* mCollisionSoundManager;
	std::set<Cure::GameObjectId> mObjects;
	CollisionList mCollisionList;
	KeyMap mKeyMap;
	DragList mDragList;
	DragEraseList mDragEraseList;
	bool mIsMouseControlled;
	bool mSetFocus;
	bool mSetCursorVisible;
	bool mSetCursorInvisible;
	vec3 mMouseMove;
	HiResTimer mTouchstickTimer;
	TouchstickList mTouchstickList;
	Life::Menu* mMenu;
	Light* mLight;
	vec3 mCameraAngle;
	xform mCameraTransform;
	vec3 mCameraVelocity;
	UiTbc::Button* mPauseButton;
	UiTbc::Button* mBackButton;
	bool mIsPaused;
	bool mIsControlled;
	bool mWasControlled;
	bool mIsControlTimeout;
	SocketAddress mLocalAddress;
	SocketAddress mLastRemoteAddress;
	UdpSocket* mCommandSocket;
	MemberThread<TrabantSimManager>* mCommandThread;
	FileServer* mFileServer;
	UiTbc::Dialog* mUserInfoDialog;
	UiTbc::Label* mUserInfoLabel;
	logclass();
};



}
