
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../Cure/Include/GameManager.h"
#include "../Cure/Include/GameTicker.h"
#include "../UiTbc/Include/UiRenderer.h"



#define FPS	45



namespace Cure
{
class ContextObject;
class ResourceManager;
class RuntimeVariableScope;
}

namespace UiCure
{
class CollisionSoundManager;
class CppContextObject;
class GameUiManager;
}



namespace Bounce
{



using namespace Lepra;
class Ball;
class Racket;



struct FingerMovement
{
	int mStartX;
	int mStartY;
	int mLastX;
	int mLastY;
	int mDeltaX;
	int mDeltaY;
	int mMovedDistance;
	bool mIsPress;
	int mTag;
	HiResTimer mTimer;

	inline FingerMovement(int x, int y):
		mStartX(x),
		mStartY(y),
		mLastX(x),
		mLastY(y),
		mDeltaX(0),
		mDeltaY(0),
		mMovedDistance(0),
		mIsPress(true),
		mTag(0),
		mTimer(false)
	{
	}

	inline bool Update(int pLastX, int pLastY, int pNewX, int pNewY)
	{
		if (std::abs(mLastX-pLastX) < 44 && std::abs(mLastY-pLastY) < 44)
		{
			mDeltaX += std::abs(mLastX-pNewX);
			mDeltaY += std::abs(mLastY-pNewY);
			mLastX = pNewX;
			mLastY = pNewY;
			return true;
		}
		return false;
	}

	inline void UpdateDistance()
	{
		mMovedDistance = mDeltaX + mDeltaY;
	}
};
typedef std::list<FingerMovement> FingerMoveList;



class Game: public Cure::GameTicker, public Cure::GameManager
{
	typedef Cure::GameTicker GameTicker;
public:
	Game(UiCure::GameUiManager* pUiManager, Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager);
	virtual ~Game();
	UiCure::GameUiManager* GetUiManager() const;
	virtual bool Tick();

	void SetRacketForce(float pLiftFactor, const Vector3DF& pDown);
	bool MoveRacket();
	Racket* GetRacket() const;
	Ball* GetBall() const;
	void ResetScore();
	double GetScore() const;

	bool Render();
	bool Paint();

private:
	virtual void PollRoundTrip();	// Polls network for any incoming to yield lower latency.
	virtual float GetTickTimeReduction() const;	// Returns how much quicker the tick loop should be; can be negative.
	virtual float GetPowerSaveAmount() const;

	virtual void WillMicroTick(float pTimeDelta);
	virtual void DidPhysicsTick();
	virtual void OnTrigger(TBC::PhysicsManager::TriggerID pTrigger, int pTriggerListenerId, int pOtherBodyId, const Vector3DF& pNormal);
	virtual void OnForceApplied(int pObjectId, int pOtherObjectId, TBC::PhysicsManager::BodyID pBodyId, TBC::PhysicsManager::BodyID pOtherBodyId,
		const Vector3DF& pForce, const Vector3DF& pTorque, const Vector3DF& pPosition, const Vector3DF& pRelativeVelocity);

	virtual void OnLoadCompleted(Cure::ContextObject* pObject, bool pOk);
	virtual void OnCollision(const Vector3DF& pForce, const Vector3DF& pTorque, const Vector3DF& pPosition,
		Cure::ContextObject* pObject1, Cure::ContextObject* pObject2,
		TBC::PhysicsManager::BodyID pBody1Id, TBC::PhysicsManager::BodyID pBody2Id);
	virtual bool OnPhysicsSend(Cure::ContextObject* pObject);
	virtual bool OnAttributeSend(Cure::ContextObject* pObject);
	virtual bool IsServer();
	virtual void SendAttach(Cure::ContextObject* pObject1, unsigned pId1, Cure::ContextObject* pObject2, unsigned pId2);
	virtual void SendDetach(Cure::ContextObject* pObject1, Cure::ContextObject* pObject2);
	virtual void TickInput();
	virtual Cure::ContextObject* CreateContextObject(const str& pClassId) const;
	virtual bool Initialize();
	virtual bool InitializeUniverse();

	Cure::ContextObject* CreateLogicHandler(const str& pType);

	UiCure::GameUiManager* mUiManager;
	UiCure::CollisionSoundManager* mCollisionSoundManager;
	UiTbc::Renderer::LightID mLightId;
	Racket* mRacket;
	Ball* mBall;
	float mRacketLiftFactor;
	Vector3DF mRacketDownDirection;
	double mScore;

	LOG_CLASS_DECLARE();
};



}
