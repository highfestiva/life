
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Cure/Include/GameManager.h"



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



namespace GrenadeRun
{



using namespace Lepra;



struct FingerMovement
{
	int mStartX;
	int mStartY;
	int mLastX;
	int mLastY;
	bool mIsPress;

	inline FingerMovement(int x, int y):
		mStartX(x),
		mStartY(y),
		mLastX(x),
		mLastY(y),
		mIsPress(true)
	{
	}

	inline bool Update(int pLastX, int pLastY, int pNewX, int pNewY)
	{
		if (std::abs(mLastX-pLastX) < 10 && std::abs(mLastY-pLastY) < 10)
		{
			mLastX = pNewX;
			mLastY = pNewY;
			return true;
		}
		return false;
	}
};
typedef std::list<FingerMovement> FingerMoveList;



class Game: public Cure::GameTicker, public Cure::GameManager
{
	typedef GameManager Parent;
public:
	Game(UiCure::GameUiManager* pUiManager, Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager);
	virtual ~Game();
	virtual bool Initialize();
	virtual bool Tick();

	UiCure::CppContextObject* GetP1();
	UiCure::CppContextObject* GetP2();
	bool Shoot();
	void Blast(const Vector3DF& pForce, const Vector3DF& pTorque, const Vector3DF& pPosition, Cure::ContextObject* pObject1);

	bool Render();

	//void MoveTo(const FingerMovement& pMove);

private:
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
	virtual bool InitializeTerrain();

	Cure::ContextObject* CreateLogicHandler(const str&) const;

	UiCure::GameUiManager* mUiManager;
	UiCure::CollisionSoundManager* mCollisionSoundManager;
	UiCure::CppContextObject* mLevel;
	UiCure::CppContextObject* mVehicle;
	UiCure::CppContextObject* mLauncher;
};



}
