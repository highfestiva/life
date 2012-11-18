
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../Cure/Include/GameManager.h"
#include "../UiTbc/Include/UiRenderer.h"



#ifdef LEPRA_TOUCH
#define LEPRA_TOUCH_LOOKANDFEEL
#endif // iOS
#define LEPRA_TOUCH_LOOKANDFEEL
#define FPS			20
#define SCALE_FACTOR		1.0f
#define HEALTH_ROUND_FACTOR	0.8f



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



namespace TireFire
{



using namespace Lepra;
class Goal;
class Vehicle;
class Level;
class VehicleAi;



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
public:
	enum FlybyMode
	{
		FLYBY_INACTIVE = 1,
		FLYBY_INTRODUCTION,
		FLYBY_INTRODUCTION_FINISHING_UP,
		FLYBY_PAUSE,
	};

	Game(UiCure::GameUiManager* pUiManager, Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager);
	virtual ~Game();
	UiCure::GameUiManager* GetUiManager() const;
	const str& GetLevelName() const;
	bool SetLevelName(const str& pLevel);
	bool RestartLevel();
	TransformationF GetVehicleStart() const;
	virtual bool Tick();
	void TickFlyby();

	str GetVehicleName() const;
	void SetVehicleName(const str& pVehicle);
	Level* GetLevel() const;
	Vehicle* GetVehicle() const;
	Goal* GetGoal() const;
	void GetVehicleMotion(Vector3DF& pPosition, Vector3DF pVelocity) const;
	void SetThrottle(float pThrottle);
	FlybyMode GetFlybyMode() const;
	void SetFlybyMode(FlybyMode pFlybyMode);
	void ResetScore();
	void AddScore(double pScore);
	double GetScore() const;
	void EnableScoreCounting(bool pEnable);
	bool IsScoreCountingEnabled() const;
	void Detonate(const Vector3DF& pForce, const Vector3DF& pTorque, const Vector3DF& pPosition,
		Cure::ContextObject* pExplosive, Cure::ContextObject* pTarget, TBC::PhysicsManager::BodyID pExplosiveBodyId, TBC::PhysicsManager::BodyID pTargetBodyId);
	void OnCapture();	// CTF
	int GetHeartBalance() const;	// 0 = Heart is 2 love, -1 = P1 leads with 1 point, +2 P2 leads with 2 points...
	void SetHeartBalance(int pBalance);
	void FlipRenderSides();
	bool IsFlipRenderSide() const;
	void NextRound();
	int GetRoundIndex() const;
	void EndSlowmo();

	bool Render();
	bool Paint();

	//void MoveTo(const FingerMovement& pMove);

private:
	bool FlybyRender();

	virtual void PollRoundTrip();	// Polls network for any incoming to yield lower latency.
	virtual float GetTickTimeReduction() const;	// Returns how much quicker the tick loop should be; can be negative.
	virtual float GetPowerSaveAmount() const;

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
	virtual bool InitializeTerrain();

	Cure::ContextObject* CreateLogicHandler(const str& pType);

	UiCure::GameUiManager* mUiManager;
	UiCure::CollisionSoundManager* mCollisionSoundManager;
	UiTbc::Renderer::LightID mLightId;
	Level* mLevel;
	str mLevelName;
	FlybyMode mFlybyMode;
	double mFlyByTime;
	Vehicle* mVehicle;
	Vector3DF mVehicleCamPos;

	mutable Goal* mGoal;
	VehicleAi* mVehicleAi;
	int mFlipRenderSide;
	float mFlipRenderSideFactor;
	double mScore;
	bool mScoreCountingEnabled;
	StopWatch mSlowmoTimer;
};



}
