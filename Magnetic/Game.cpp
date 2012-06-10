
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games

#include "Game.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/ContextPath.h"
#include "../Cure/Include/FloatAttribute.h"
#include "../Cure/Include/RuntimeVariable.h"
#include "../Cure/Include/TimeManager.h"
#include "../Lepra/Include/Random.h"
#include "../TBC/Include/PhysicsEngine.h"
#include "../UiCure/Include/UiCollisionSoundManager.h"
#include "../UiCure/Include/UiGameUiManager.h"
#include "../UiCure/Include/UiProps.h"
#include "../UiCure/Include/UiRuntimeVariableName.h"
#include "../UiCure/Include/UiSound.h"
#include "Ball.h"
#include "Racket.h"



#define GRENADE_RELAUNCH_DELAY	2.7f
#define CUTIE_START		Vector3DF(-57.67f, -28.33f, 2.33f)



namespace Magnetic
{



Game::Game(UiCure::GameUiManager* pUiManager, Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager):
	Cure::GameTicker(),
	Cure::GameManager(Cure::GameTicker::GetTimeManager(), pVariableScope, pResourceManager, 20, 3, 0),
	mUiManager(pUiManager),
	mCollisionSoundManager(0),
	mLightId(UiTbc::Renderer::INVALID_LIGHT),
	mRacket(0),
	mBall(0),
	mRacketLiftFactor(0),
	mScore(0)
{
	mCollisionSoundManager = new UiCure::CollisionSoundManager(this, pUiManager);
	mCollisionSoundManager->SetScale(0.05f, 0.1f, 0.1f);
	mCollisionSoundManager->AddSound(_T("hit"), UiCure::CollisionSoundManager::SoundResourceInfo(0.9f, 0.1f, 0.5f));

	Initialize();
}

Game::~Game()
{
	mUiManager->GetRenderer()->RemoveLight(mLightId);

	delete mCollisionSoundManager;
	mCollisionSoundManager = 0;
	mUiManager = 0;
	SetVariableScope(0);	// Not owned by us.
}

UiCure::GameUiManager* Game::GetUiManager() const
{
	return mUiManager;
}

bool Game::Tick()
{
	if (!mRacket || !mRacket->IsLoaded() ||
		!mBall || !mBall->IsLoaded())
	{
		return true;
	}

	GameTicker::GetTimeManager()->Tick();

	Vector3DF lPosition;
	Vector3DF lVelocity;
	mCollisionSoundManager->Tick(lPosition);
	mUiManager->SetMicrophonePosition(TransformationF(gIdentityQuaternionF, lPosition), lVelocity);

	return true;
}



void Game::SetRacketForce(float pLiftFactor, const Vector3DF& pDown)
{
	mRacketLiftFactor = (pDown.z > 0)? -pLiftFactor : pLiftFactor;
	mRacketDownDirection = pDown;
}

bool Game::MoveRacket()
{
	if (GetRacket() && GetRacket()->IsLoaded() &&
		GetBall() && GetBall()->IsLoaded())
	{
		TransformationF lRacketTransform;
		GetPhysicsManager()->GetBodyTransform(
			GetRacket()->GetPhysics()->GetBoneGeometry(0)->GetBodyId(),
			lRacketTransform);
		Vector3DF lRacketLinearVelocity;
		GetPhysicsManager()->GetBodyVelocity(
			GetRacket()->GetPhysics()->GetBoneGeometry(0)->GetBodyId(),
			lRacketLinearVelocity);
		Vector3DF lRacketAngularVelocity;
		GetPhysicsManager()->GetBodyAngularVelocity(
			GetRacket()->GetPhysics()->GetBoneGeometry(0)->GetBodyId(),
			lRacketAngularVelocity);

		// Calculate where ball will be as it passes z = racket z.
		Vector3DF lBallPosition =
			GetPhysicsManager()->GetBodyPosition(GetBall()->GetPhysics()->GetBoneGeometry(0)->GetBodyId());
		Vector3DF lBallVelocity;
		GetPhysicsManager()->GetBodyVelocity(
			GetBall()->GetPhysics()->GetBoneGeometry(0)->GetBodyId(),
			lBallVelocity);
		if (lBallPosition.z < -2)
		{
			lBallPosition.Set(0, 0, 0.4f);
			GetPhysicsManager()->SetBodyTransform(GetBall()->GetPhysics()->GetBoneGeometry(0)->GetBodyId(), TransformationF(QuaternionF(), lBallPosition));
			lBallVelocity.Set(0, 0, 2.0f);
			GetPhysicsManager()->SetBodyVelocity(GetBall()->GetPhysics()->GetBoneGeometry(0)->GetBodyId(), lBallVelocity);
			GetPhysicsManager()->SetBodyAngularVelocity(GetBall()->GetPhysics()->GetBoneGeometry(0)->GetBodyId(), Vector3DF());
			lRacketTransform.SetIdentity();
			GetPhysicsManager()->SetBodyTransform(GetRacket()->GetPhysics()->GetBoneGeometry(0)->GetBodyId(), lRacketTransform);
			lRacketLinearVelocity.Set(0, 0, 0);
			GetPhysicsManager()->SetBodyVelocity(GetRacket()->GetPhysics()->GetBoneGeometry(0)->GetBodyId(), lRacketLinearVelocity);
			lRacketAngularVelocity.Set(0, 0, 0);
			GetPhysicsManager()->SetBodyAngularVelocity(GetRacket()->GetPhysics()->GetBoneGeometry(0)->GetBodyId(), lRacketAngularVelocity);
			return false;
		}
		Vector3DF lHome;
		const float h = lBallPosition.z - lRacketTransform.GetPosition().z;
		if (h > -0.5f)
		{
			lHome.Set(lBallPosition.x*0.8f, lBallPosition.y*0.8f, 0);
		}
		const float vup = lBallVelocity.z;
		const float a = +9.82f / 2;
		const float b = -vup;
		const float c = +h;
		const float b2 = b*b;
		const float _4ac = 4*a*c;
		if (b2 < _4ac || _4ac < 0)
		{
			// Does not compute.
		}
		else
		{
			const float t = (-b + sqrt(b2 - _4ac)) / (2*a);
			if (t > 0)
			{
				lHome.x += lBallVelocity.x * t;
				lHome.y += lBallVelocity.y * t;
			}
		}
		// Set linear force.
		const Vector3DF lDirectionHome = lHome - lRacketTransform.GetPosition();
		float f = lDirectionHome.GetLength();
		f *= 50;
		f *= f;
		Vector3DF lForce = lDirectionHome * f;
		lForce -= lRacketLinearVelocity * 10;
		float lUserForceFactor = ::fabs(mRacketLiftFactor) * 1.7f;
		lUserForceFactor = std::min(1.0f, lUserForceFactor);
		const float lRacketAcceleration = 250.0f * mRacketLiftFactor;
		const float zForce = Math::Lerp(lForce.z, lRacketAcceleration, lUserForceFactor);
		lForce.z = zForce;
		f = lForce.GetLength();
		if (f > 100)
		{
			lForce *= 100 / f;
		}
		//mLog.Infof(_T("force = (%f, %f, %f)"), lForce.x, lForce.y, lForce.z);
		GetPhysicsManager()->AddForce(
			GetRacket()->GetPhysics()->GetBoneGeometry(0)->GetBodyId(),
			lForce);

		// Set torque. Note that racket is "flat" along the XY-plane.
		//const float lTiltAngleFactor = 1.2f;
		//const float dx = lDirectionHome.x * lTiltAngleFactor;
		//const float dy = lDirectionHome.y * lTiltAngleFactor;
		//const float dx = -lRacketTransform.GetPosition().x * lTiltAngleFactor;
		//const float dy = -lRacketTransform.GetPosition().y * lTiltAngleFactor;
		mRacketDownDirection.Normalize();
		const Vector3DF lHomeTorque = Vector3DF(::acos(mRacketDownDirection.y), ::acos(mRacketDownDirection.x), 0);
		Vector3DF lRacketTorque = lRacketTransform.GetOrientation() * Vector3DF(0,0,1);
		lRacketTorque = Vector3DF(::acos(lRacketTorque.y), ::acos(lRacketTorque.x), 0);
		Vector3DF lAngleHome = lHomeTorque - lRacketTorque;
		lAngleHome.y = -lAngleHome.y;
		lAngleHome.z = 0;
		f = Math::Clamp(-lBallVelocity.z, 0.0f, 4.0f) / 4.0f;
		f = Math::Lerp(0.8f, 0.3f, f);
		f = lAngleHome.GetLength() * f;
		f *= f;
		f = 1;
		Vector3DF lTorque = lAngleHome * f;
		lTorque -= lRacketAngularVelocity * 0.2f;
		//mLog.Infof(_T("torque = (%f, %f, %f)"), lTorque.x, lTorque.y, lTorque.z);
		GetPhysicsManager()->AddTorque(
			GetRacket()->GetPhysics()->GetBoneGeometry(0)->GetBodyId(),
			lTorque);
	}
	return true;
}

Racket* Game::GetRacket() const
{
	return mRacket;
}

Ball* Game::GetBall() const
{
	return mBall;
}

void Game::ResetScore()
{
	mScore = 0;
}

double Game::GetScore() const
{
	return mScore;
}



bool Game::Render()
{
	QuaternionF lOrientation;
	lOrientation.RotateAroundOwnX(-PIF/5);
	mUiManager->SetCameraPosition(TransformationF(lOrientation, Vector3DF(0, -0.4f, 0.6f)));
	const PixelRect lFullRect(0, 0, mUiManager->GetCanvas()->GetWidth(), mUiManager->GetCanvas()->GetHeight());
	mUiManager->Render(lFullRect);
	return true;
}

bool Game::Paint()
{
	return true;
}



void Game::PollRoundTrip()
{
}

float Game::GetTickTimeReduction() const
{
        return 0;
}

float Game::GetPowerSaveAmount() const
{
	bool lIsMinimized = !mUiManager->GetDisplayManager()->IsVisible();
	return (lIsMinimized? 1.0f : 0);
}



void Game::OnLoadCompleted(Cure::ContextObject* pObject, bool pOk)
{
	(void)pObject;
	(void)pOk;
}

void Game::OnCollision(const Vector3DF& pForce, const Vector3DF& pTorque, const Vector3DF& pPosition,
	Cure::ContextObject* pObject1, Cure::ContextObject* pObject2,
	TBC::PhysicsManager::BodyID pBody1Id, TBC::PhysicsManager::BodyID pBody2Id)
{
	(void)pBody2Id;
	mCollisionSoundManager->OnCollision(pForce, pTorque, pPosition, pObject1, pObject2, pBody1Id, 2000, false);

	const float lForce = pForce.GetLength();
	if (pObject1 == mBall && lForce > 1.0f)
	{
		mScore += ::sqrt(lForce) * 20;
	}
}

bool Game::OnPhysicsSend(Cure::ContextObject* pObject)
{
	(void)pObject;
	return true;
}

bool Game::OnAttributeSend(Cure::ContextObject* pObject)
{
	(void)pObject;
	return true;
}

bool Game::IsServer()
{
	return true;
}

void Game::SendAttach(Cure::ContextObject* pObject1, unsigned pId1, Cure::ContextObject* pObject2, unsigned pId2)
{
	(void)pObject1;
	(void)pId1;
	(void)pObject2;
	(void)pId2;
}

void Game::SendDetach(Cure::ContextObject* pObject1, Cure::ContextObject* pObject2)
{
	(void)pObject1;
	(void)pObject2;
}

void Game::TickInput()
{
}

Cure::ContextObject* Game::CreateContextObject(const str& pClassId) const
{
	return new UiCure::Machine(GetResourceManager(), pClassId, mUiManager);
}

bool Game::Initialize()
{
	bool lOk = true;
	if (lOk)
	{
		lOk = InitializeTerrain();
	}
	if (lOk)
	{
		const bool lPixelShadersEnabled = mUiManager->GetRenderer()->IsPixelShadersEnabled();
		mLightId = mUiManager->GetRenderer()->AddDirectionalLight(
			UiTbc::Renderer::LIGHT_MOVABLE, Vector3DF(-1, 0.5f, -1.5),
			Color::Color(255, 255, 255), lPixelShadersEnabled? 1.0f : 1.5f, 300);
		mUiManager->GetRenderer()->EnableAllLights(true);
	}
	return lOk;
}

bool Game::InitializeTerrain()
{
	bool lOk = true;
	if (lOk)
	{
		delete mRacket;
		mRacket = new Racket(GetResourceManager(), _T("racket"), mUiManager);
		AddContextObject(mRacket, Cure::NETWORK_OBJECT_LOCAL_ONLY, 0);
		lOk = (mRacket != 0);
		assert(lOk);
		if (lOk)
		{
			mRacket->SetInitialTransform(TransformationF(QuaternionF(), Vector3DF(0, 0, 0)));
			mRacket->DisableRootShadow();
			mRacket->StartLoading();
		}
	}
	if (lOk)
	{
		delete mBall;
		mBall = new Ball(GetResourceManager(), _T("ball"), mUiManager);
		AddContextObject(mBall, Cure::NETWORK_OBJECT_LOCAL_ONLY, 0);
		lOk = (mBall != 0);
		assert(lOk);
		if (lOk)
		{
			mBall->SetInitialTransform(TransformationF(QuaternionF(), Vector3DF(0, 0, 0.4f)));
			mBall->DisableRootShadow();
			mBall->StartLoading();
		}
	}
	return lOk;
}



Cure::ContextObject* Game::CreateLogicHandler(const str& pType)
{
	(void)pType;
	return 0;
}



LOG_CLASS_DEFINE(GAME, Game);



}
