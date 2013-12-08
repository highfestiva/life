
// Author: Jonas Bystr�m
// Copyright (c) Pixel Doctrine



#pragma once

//#include "../Lepra/Include/GameTimer.h"
#include "../Life/LifeClient/GameClientSlaveManager.h"
#include "../Life/LifeClient/Menu.h"
#include "../UiCure/Include/UiResourceManager.h"
#include "Bound.h"
//#include "Version.h"



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
}
namespace UiTbc
{
class Button;
}


namespace Bound
{



class Sunlight;



class BoundManager: public Life::GameClientSlaveManager
{
	typedef Life::GameClientSlaveManager Parent;
public:
	BoundManager(Life::GameClientMasterTicker* pMaster, const Cure::TimeManager* pTime,
		Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager,
		UiCure::GameUiManager* pUiManager, int pSlaveIndex, const PixelRect& pRenderArea);
	virtual ~BoundManager();
	virtual void Suspend();
	virtual void LoadSettings();
	virtual void SaveSettings();
	virtual bool Open();
	virtual void Close();
	virtual void SetIsQuitting();
	virtual void SetFade(float pFadeAmount);

	virtual bool Render();
	virtual bool Paint();
	static bool AttachToBorder(PixelCoord& pPoint, int pMargin, int pWidth, int pHeight);

	bool SetAvatarEnginePower(unsigned pAspect, float pPower);

	virtual bool DidFinishLevel();
	virtual int StepLevel(int pCount);

	Cure::RuntimeVariableScope* GetVariableScope() const;

protected:
	typedef UiLepra::Touch::TouchstickInputDevice Touchstick;

	virtual bool InitializeUniverse();
	virtual void TickInput();
	virtual void TickUiInput();
	virtual void TickUiUpdate();
	virtual void SetLocalRender(bool pRender);

	void CreateBall(int pIndex);
	virtual Cure::ContextObject* CreateContextObject(const str& pClassId) const;
	//virtual Cure::ContextObject* CreateLogicHandler(const str& pType);
	virtual void OnLoadCompleted(Cure::ContextObject* pObject, bool pOk);
	void OnCollision(const Vector3DF& pForce, const Vector3DF& pTorque, const Vector3DF& pPosition,
		Cure::ContextObject* pObject1, Cure::ContextObject* pObject2,
		TBC::PhysicsManager::BodyID pBody1Id, TBC::PhysicsManager::BodyID pBody2Id);

	void OnPauseButton(UiTbc::Button*);
	void OnMenuAlternative(UiTbc::Button* pButton);

	virtual void ScriptPhysicsTick();
	virtual void HandleWorldBoundaries();
	virtual void MoveCamera(float pFrameTime);
	virtual void UpdateCameraPosition(bool pUpdateMicPosition);

	void DrawImage(UiTbc::Painter::ImageID pImageId, float cx, float cy, float w, float h, float pAngle) const;

	void PainterImageLoadCallback(UiCure::UserPainterKeepImageResource* pResource);

	UiCure::CollisionSoundManager* mCollisionSoundManager;
	std::vector<Cure::GameObjectId> mBalls;
	Life::Menu* mMenu;
	StopWatch mNextLevelTimer;
	Sunlight* mSunlight;
	float mCameraAngle;
	TransformationF mCameraTransform;
	bool mLevelCompleted;
	UiTbc::Button* mPauseButton;
    bool mIsCutting;
	LOG_CLASS_DECLARE();
};



}
