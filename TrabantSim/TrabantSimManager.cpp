
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "TrabantSimManager.h"
#include <algorithm>
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/TimeManager.h"
#include "../Lepra/Include/CyclicArray.h"
#include "../Lepra/Include/Random.h"
#include "../Lepra/Include/Socket.h"
#include "../Lepra/Include/SystemManager.h"
#include "../Lepra/Include/TimeLogger.h"
#include "../Lepra/Include/Unordered.h"
#include "../Life/LifeClient/GameClientMasterTicker.h"
#include "../Life/LifeClient/UiConsole.h"
#include "../Tbc/Include/PhysicsEngine.h"
#include "../UiCure/Include/UiCollisionSoundManager.h"
#include "../UiCure/Include/UiGameUiManager.h"
#include "../UiCure/Include/UiIconButton.h"
#include "../UiCure/Include/UiMachine.h"
#include "../UiCure/Include/UiSoundReleaser.h"
#include "../UiLepra/Include/UiTouchDrag.h"
#include "../UiTbc/Include/GUI/UiDesktopWindow.h"
#include "../UiTbc/Include/GUI/UiFixedLayouter.h"
#include "../UiTbc/Include/UiMaterial.h"
#include "../UiTbc/Include/UiParticleRenderer.h"
#include "../UiTbc/Include/UiTriangleBasedGeometry.h"
#include "Object.h"
#include "TrabantSim.h"
#include "TrabantSimConsoleManager.h"
#include "RtVar.h"
#include "Light.h"
#include "Version.h"

#define BG_COLOR		Color(25, 35, 45, 190)
#define BRIGHT_TEXT		Color(220, 215, 205)
#define GREEN_BUTTON		Color(20, 190, 15)


namespace TrabantSim
{



TrabantSimManager::TrabantSimManager(Life::GameClientMasterTicker* pMaster, const Cure::TimeManager* pTime,
	Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager,
	UiCure::GameUiManager* pUiManager, int pSlaveIndex, const PixelRect& pRenderArea):
	Parent(pMaster, pTime, pVariableScope, pResourceManager, pUiManager, pSlaveIndex, pRenderArea),
	mCollisionSoundManager(0),
	mMenu(0),
	mLight(0),
	mCameraTransform(quat(), vec3(0, -3, 0)),
	mPauseButton(0),
	mListenerSocket(0),
	mConnectSocket(0),
	mAcceptThread(new MemberThread<TrabantSimManager>("ConnectionListener")),
	mCommandThread(0),
	mUserInfoDialog(0),
	mUserInfoLabel(0)
{
	mCollisionSoundManager = new UiCure::CollisionSoundManager(this, pUiManager);

	SetConsoleManager(new TrabantSimConsoleManager(GetResourceManager(), this, mUiManager, GetVariableScope(), mRenderArea));

	GetPhysicsManager()->SetSimulationParameters(0.0f, 0.03f, 0.2f);

	SocketAddress lAddress;
	const strutil::strvec& args = SystemManager::GetArgumentVector();
	if (!lAddress.Resolve(args[args.size()-1]))
	{
		lAddress.Resolve(_T("localhost:2541"));
	}
	mListenerSocket = new TcpListenerSocket(lAddress, true);
	if (mListenerSocket->IsOpen())
	{
		mAcceptThread->Start(this, &TrabantSimManager::AcceptLoop);
		mLog.Headlinef(_T("Command server listening on %s."), lAddress.GetAsString().c_str());
	}
	else
	{
		mLog.Headlinef(_T("Could not open server on %s. Shutting down."), lAddress.GetAsString().c_str());
		SystemManager::AddQuitRequest(1);
	}
}

TrabantSimManager::~TrabantSimManager()
{
	Close();

	delete mCollisionSoundManager;
	mCollisionSoundManager = 0;

	if (mConnectSocket)	// Must be destroyed before the server socket.
	{
		if (mCommandThread) mCommandThread->RequestStop();
		mConnectSocket->Disconnect();
		delete mCommandThread;
		mCommandThread = 0;
		delete mConnectSocket;
		mConnectSocket = 0;
	}

	mAcceptThread->RequestStop();
	TcpSocket(0).Connect(mListenerSocket->GetLocalAddress());
	delete mAcceptThread;
	mAcceptThread = 0;
	delete mListenerSocket;
	mListenerSocket = 0;
}



void TrabantSimManager::UserReset()
{
	Cure::RuntimeVariableScope* lScope = GetVariableScope();
	const std::list<str> lVariableList = lScope->GetVariableNameList(Cure::RuntimeVariableScope::SEARCH_EXPORTABLE);
	std::list<str>::const_iterator x = lVariableList.begin();
	for (; x != lVariableList.end(); ++x)
	{
		const str lName = *x;
		if (strutil::StartsWith(lName, _T("Ui.3D.Clear")) || strutil::StartsWith(lName, _T("Ui.3D.FOV")) || strutil::StartsWith(lName, _T("Ui.3D.Clip"))
			 || strutil::StartsWith(lName, _T("Ui.3D.Cam")) || strutil::StartsWith(lName, _T("Ui.Pen")))
		{
			if (!strutil::StartsWith(lName, _T("Ui.3D.CamTarget")))
			{
				continue;
			}
		}
		lScope->ResetDefaultValue(lName);
	}
	mCameraAngle.Set(0,0,0);

	GetResourceManager()->ForceFreeCache();
	GetResourceManager()->ForceFreeCache();
	GetResourceManager()->ForceFreeCache();

	ScopeLock lGameLock(GetTickLock());
	// TODO: Kill all joysticks and collisions.
}

int TrabantSimManager::CreateObject(const MeshObject& pGfxObject, const PhysObjectArray& pPhysObjects, bool pIsStatic)
{
	ScopeLock lPhysLock(GetMaster()->GetPhysicsLock());
	ScopeLock lGameLock(GetTickLock());

	xform pt;
	quat q, pq;
	q.RotateAroundWorldX(PIF/-2);
	Object* lObject = (Object*)Parent::CreateContextObject(_T("object"), Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED, 0);
	Tbc::ChunkyPhysics* lPhysics = new Tbc::ChunkyPhysics(Tbc::BoneHierarchy::TRANSFORM_LOCAL2WORLD, pIsStatic? Tbc::ChunkyPhysics::STATIC : Tbc::ChunkyPhysics::DYNAMIC);
	if (pPhysObjects.empty())
	{
		lObject->SetPhysicsTypeOverride(Cure::PHYSICS_OVERRIDE_BONES);
		lPhysics->SetBoneCount(1);
		Tbc::ChunkyBoneGeometry::BodyData lBoneData(0,0,0);
		Tbc::ChunkyBoneGeometry* lBone = new Tbc::ChunkyBoneSphere(lBoneData);
		lPhysics->AddBoneGeometry(xform(quat(1,0,0,0),vec3(0,0,0)), lBone);
	}
	else
	{
		float lFriction, lBounce;
		v_get(lFriction, =(float), GetVariableScope(), RTVAR_PHYSICS_FRICTION, 0.5);
		v_get(lBounce, =(float), GetVariableScope(), RTVAR_PHYSICS_BOUNCE, 0.2);

		lPhysics->SetBoneCount(pPhysObjects.size());
		PhysObjectArray::const_iterator x = pPhysObjects.begin();
		int y = 0;
		Tbc::ChunkyBoneGeometry* lParent = 0;
		PlacedObject* lParentPhysObject = 0;
		for (; x != pPhysObjects.end(); ++x, ++y)
		{
			Tbc::ChunkyBoneGeometry::BodyData lBoneData(y==0? 15.0f:1.0f, lFriction, lBounce, lParent);
			PlacedObject* lPhysObject = *x;
			xform t = xform(lPhysObject->mOrientation, lPhysObject->mPos);
			if (lParentPhysObject)
			{
				//t.mOrientation = t.mOrientation * pq.GetInverse();
				//t.mOrientation = t.mOrientation.GetInverse();
				//t.mPosition -= lParentPhysObject->mPos;
				//t.RotateAroundAnchor(lPhysObject->pos, vec3(1,0,0), PIF/2);
				//t = pt.InverseTransform(t);
				t.mPosition = pq.GetInverse() * t.mPosition;
				t.mOrientation = pq.GetInverse() * t.mOrientation;
			}
			else
			{
				pt = t;
				pt.mPosition.Set(0,0,0);
				pq = pt.mOrientation;
				//t.RotateAroundAnchor(lPhysObject->pos, vec3(1,0,0), PIF/2);
				//t.mOrientation = q.GetInverse() * t.mOrientation.GetInverse() * q;
			}
			BoxObject* lBox = dynamic_cast<BoxObject*>(lPhysObject);
			SphereObject* lSphere = dynamic_cast<SphereObject*>(lPhysObject);
			MeshObject* lMesh = dynamic_cast<MeshObject*>(lPhysObject);
			if (lBox)
			{
				Tbc::ChunkyBoneBox* lBone = new Tbc::ChunkyBoneBox(lBoneData);
				lBone->mSize = lBox->mSize;
				lPhysics->AddBoneGeometry(t, lBone, lParent);
			}
			else if (lSphere)
			{
				Tbc::ChunkyBoneSphere* lBone = new Tbc::ChunkyBoneSphere(lBoneData);
				lBone->mRadius = lSphere->mRadius;
				lPhysics->AddBoneGeometry(t, lBone, lParent);
			}
			else if (lMesh)
			{
				Tbc::ChunkyBoneMesh* lBone = new Tbc::ChunkyBoneMesh(lBoneData);
				lBone->mVertexCount = lMesh->mVertices.size()/3;
				lBone->mTriangleCount = lMesh->mIndices.size()/3;
				lBone->mVertices = new float[lBone->mVertexCount*3];
				lBone->mIndices = new uint32[lBone->mTriangleCount*3];
				for (unsigned x = 0; x < lBone->mVertexCount; ++x)
				{
					lBone->mVertices[x*3+0] = lMesh->mVertices[x*3+0];
					lBone->mVertices[x*3+1] = lMesh->mVertices[x*3+1];
					lBone->mVertices[x*3+2] = lMesh->mVertices[x*3+2];
				}
				for (unsigned x = 0; x < lBone->mTriangleCount; ++x)
				{
					lBone->mIndices[x*3+0] = lMesh->mIndices[x*3+0];
					lBone->mIndices[x*3+1] = lMesh->mIndices[x*3+1];
					lBone->mIndices[x*3+2] = lMesh->mIndices[x*3+2];
				}
				lPhysics->AddBoneGeometry(t, lBone, lParent);
			}
			if (y == 0)
			{
				lParent = lPhysics->GetBoneGeometry(0);
				lParentPhysObject = lPhysObject;
			}
		}
	}
	lObject->SetRootOrientation(pt.mOrientation);
	lObject->CreatePhysics(lPhysics);
	float r,g,b;
	v_get(r, =(float), GetVariableScope(), RTVAR_UI_PENRED, 0.5);
	v_get(g, =(float), GetVariableScope(), RTVAR_UI_PENGREEN, 0.5);
	v_get(b, =(float), GetVariableScope(), RTVAR_UI_PENBLUE, 0.5);
	lObject->AddGfxMesh(pGfxObject.mVertices, pGfxObject.mIndices, vec3(r,g,b));
	//q = q.GetInverse() * pGfxObject.mOrientation.GetInverse() * q;
	q.Set(1,0,0,0);
	//q.RotateAroundWorldX(PIF);
	lObject->GetMeshResource(0)->mOffset.mOffset.mOrientation = pGfxObject.mOrientation;
	mObjects.insert(lObject->GetInstanceId());
	return lObject->GetInstanceId();
}

void TrabantSimManager::DeleteObject(int pObjectId)
{
	ScopeLock lGameLock(GetTickLock());
	GetContext()->PostKillObject(pObjectId);
	std::set<Cure::GameObjectId>::iterator x = mObjects.find(pObjectId);
	if (x != mObjects.end())
	{
		mObjects.erase(x);
	}
	GetResourceManager()->ForceFreeCache();
}

void TrabantSimManager::DeleteAllObjects()
{
	ScopeLock lGameLock(GetTickLock());
	std::set<Cure::GameObjectId>::iterator x;
	for (x = mObjects.begin(); x != mObjects.end(); ++x)
	{
		GetContext()->PostKillObject(*x);
	}
	mObjects.clear();
	GetResourceManager()->ForceFreeCache();
}

bool TrabantSimManager::IsLoaded(int pObjectId)
{
	ScopeLock lGameLock(GetTickLock());
	return !!GetContext()->GetObject(pObjectId);
}

void TrabantSimManager::Expload(const vec3& pPos, const vec3& pVel)
{
	ScopeLock lGameLock(GetTickLock());
	(void)pPos; (void)pVel;
}

void TrabantSimManager::PlaySound(const str& pSound, const vec3& pPos, const vec3& pVel)
{
	ScopeLock lGameLock(GetTickLock());
	(void)pSound; (void)pPos; (void)pVel;
}

TrabantSimManager::CollisionList TrabantSimManager::PopCollisions()
{
	ScopeLock lGameLock(GetTickLock());
	CollisionList lList;
	return lList;
}

const TrabantSimManager::DragList& TrabantSimManager::GetTouchDrags() const
{
	ScopeLock lGameLock(GetTickLock());
	return mUiManager->GetDragManager()->GetDragList();
}

vec3 TrabantSimManager::GetAccelerometer() const
{
	return mUiManager->GetAccelerometer();
}

int TrabantSimManager::CreateJoystick(float x, float y)
{
	ScopeLock lGameLock(GetTickLock());
	(void)x; (void)y;
	return 0;
}

TrabantSimManager::JoystickDataList TrabantSimManager::GetJoystickData() const
{
	ScopeLock lGameLock(GetTickLock());
	JoystickDataList lList;
	return lList;
}

float TrabantSimManager::GetAspectRatio() const
{
	return mUiManager->GetDisplayManager()->GetWidth()/(float)mUiManager->GetDisplayManager()->GetHeight();
}

int TrabantSimManager::CreateEngine(int pObjectId, const str& pEngineType, const vec2& pMaxVelocity, const str& pEngineSound)
{
	ScopeLock lPhysLock(GetMaster()->GetPhysicsLock());
	ScopeLock lGameLock(GetTickLock());
	Object* lObject = (Object*)GetContext()->GetObject(pObjectId);
	if (!lObject)
	{
		return -1;
	}

	Tbc::PhysicsEngine::EngineType lEngineType;
	vec2 lMaxVelocity(pMaxVelocity);
	float lStrength = 15;
	float lFriction = 0.5f;
	if (pEngineType == _T("roll_turn"))
	{
		lEngineType = Tbc::PhysicsEngine::ENGINE_HINGE2_TURN;
	}
	else if (pEngineType == _T("roll"))
	{
		lEngineType = Tbc::PhysicsEngine::ENGINE_HINGE_ROLL;
	}
	else if (pEngineType == _T("push_abs"))
	{
		lEngineType = Tbc::PhysicsEngine::ENGINE_PUSH_ABSOLUTE;
		lMaxVelocity.x = (!lMaxVelocity.x)? 100.0f : lMaxVelocity.x;
		lFriction = 0.9f;
	}
	else if (pEngineType == _T("push_turn_abs"))
	{
		lEngineType = Tbc::PhysicsEngine::ENGINE_PUSH_TURN_ABSOLUTE;
		lMaxVelocity.x = (!lMaxVelocity.x)? 0.1f : lMaxVelocity.x;
		lFriction = 0.9f;
	}
	else if (pEngineType == _T("gyro"))
	{
		lEngineType = Tbc::PhysicsEngine::ENGINE_HINGE_GYRO;
	}
	else if (pEngineType == _T("rotor"))
	{
		lEngineType = Tbc::PhysicsEngine::ENGINE_ROTOR;
	}
	else if (pEngineType == _T("tilt"))
	{
		lEngineType = Tbc::PhysicsEngine::ENGINE_ROTOR_TILT;
	}
	else
	{
		return -1;
	}
	Tbc::ChunkyBoneGeometry* lGeometry = lObject->GetPhysics()->GetBoneGeometry(0);
	Tbc::PhysicsEngine* lEngine = new Tbc::PhysicsEngine(lEngineType, lStrength, lMaxVelocity.x, lMaxVelocity.y, lFriction, lObject->GetPhysics()->GetEngineCount()*4);
	lEngine->AddControlledGeometry(lGeometry, 1);
	lObject->GetPhysics()->AddEngine(lEngine);
	GetContext()->EnableMicroTickCallback(lObject);
	if (!pEngineSound.empty())
	{
		//lObject->AddTag(something something engine sound something something);
	}
	return lObject->GetPhysics()->GetEngineCount()-1;
}

int TrabantSimManager::CreateJoint(int pObjectId, const str& pJointType, int pOtherObjectId, const vec3& pAxis)
{
	ScopeLock lPhysLock(GetMaster()->GetPhysicsLock());
	ScopeLock lGameLock(GetTickLock());
	(void)pObjectId; (void)pJointType; (void)pOtherObjectId; (void)pAxis;
	return 0;
}

void TrabantSimManager::Position(int pObjectId, bool pSet, vec3& pPosition)
{
	ScopeLock lPhysLock(GetMaster()->GetPhysicsLock());
	ScopeLock lGameLock(GetTickLock());
	Object* lObject = (Object*)GetContext()->GetObject(pObjectId);
	if (!lObject)
	{
		return;
	}
	if (pSet)
	{
		GetPhysicsManager()->SetBodyPosition(lObject->GetPhysics()->GetBoneGeometry(0)->GetBodyId(), pPosition);
	}
	else
	{
		pPosition = lObject->GetPosition();
	}
}

void TrabantSimManager::Orientation(int pObjectId, bool pSet, quat& pOrientation)
{
	ScopeLock lPhysLock(GetMaster()->GetPhysicsLock());
	ScopeLock lGameLock(GetTickLock());
	Object* lObject = (Object*)GetContext()->GetObject(pObjectId);
	if (!lObject)
	{
		return;
	}
	if (pSet)
	{
		pOrientation.RotateAroundOwnX(PIF/-2);
		vec3 lPosition = lObject->GetPosition();
		GetPhysicsManager()->SetBodyTransform(lObject->GetPhysics()->GetBoneGeometry(0)->GetBodyId(), xform(pOrientation, lPosition));
	}
	else
	{
		xform t;
		GetPhysicsManager()->GetBodyTransform(lObject->GetPhysics()->GetBoneGeometry(0)->GetBodyId(), t);
		pOrientation = t.mOrientation;
		pOrientation.RotateAroundOwnX(PIF/2);
	}
}

void TrabantSimManager::Velocity(int pObjectId, bool pSet, vec3& pVelocity)
{
	ScopeLock lPhysLock(GetMaster()->GetPhysicsLock());
	ScopeLock lGameLock(GetTickLock());
	Object* lObject = (Object*)GetContext()->GetObject(pObjectId);
	if (!lObject)
	{
		return;
	}
	if (pSet)
	{
		GetPhysicsManager()->SetBodyVelocity(lObject->GetPhysics()->GetBoneGeometry(0)->GetBodyId(), pVelocity);
	}
	else
	{
		pVelocity = lObject->GetVelocity();
	}
}

void TrabantSimManager::AngularVelocity(int pObjectId, bool pSet, vec3& pAngularVelocity)
{
	ScopeLock lPhysLock(GetMaster()->GetPhysicsLock());
	ScopeLock lGameLock(GetTickLock());
	Object* lObject = (Object*)GetContext()->GetObject(pObjectId);
	if (!lObject)
	{
		return;
	}
	if (pSet)
	{
		GetPhysicsManager()->SetBodyAngularVelocity(lObject->GetPhysics()->GetBoneGeometry(0)->GetBodyId(), pAngularVelocity);
	}
	else
	{
		pAngularVelocity = lObject->GetAngularVelocity();
	}
}

void TrabantSimManager::Mass(int pObjectId, bool pSet, float& pMass)
{
	ScopeLock lPhysLock(GetMaster()->GetPhysicsLock());
	ScopeLock lGameLock(GetTickLock());
	Object* lObject = (Object*)GetContext()->GetObject(pObjectId);
	if (!lObject)
	{
		return;
	}
	if (pSet)
	{
		GetPhysicsManager()->SetBodyMass(lObject->GetPhysics()->GetBoneGeometry(0)->GetBodyId(), pMass);
	}
	else
	{
		pMass = GetPhysicsManager()->GetBodyMass(lObject->GetPhysics()->GetBoneGeometry(0)->GetBodyId());
	}
}

void TrabantSimManager::ObjectColor(int pObjectId, bool pSet, vec3& pColor)
{
	ScopeLock lGameLock(GetTickLock());
	Object* lObject = (Object*)GetContext()->GetObject(pObjectId, true);
	if (!lObject)
	{
		return;
	}
	if (pSet)
	{
		lObject->GetMesh(0)->GetBasicMaterialSettings().mDiffuse = pColor;
	}
	else
	{
		pColor = lObject->GetMesh(0)->GetBasicMaterialSettings().mDiffuse;
	}
}

void TrabantSimManager::EngineForce(int pObjectId, int pEngineIndex, bool pSet, vec3& pForce)
{
	ScopeLock lPhysLock(GetMaster()->GetPhysicsLock());
	ScopeLock lGameLock(GetTickLock());
	Object* lObject = (Object*)GetContext()->GetObject(pObjectId);
	if (!lObject)
	{
		return;
	}
	if (pSet)
	{
		switch (lObject->GetPhysics()->GetEngine(pEngineIndex)->GetEngineType())
		{
			case Tbc::PhysicsEngine::ENGINE_PUSH_ABSOLUTE:
				lObject->SetEnginePower(pEngineIndex*4+0, pForce.y);
				lObject->SetEnginePower(pEngineIndex*4+1, pForce.x);
				lObject->SetEnginePower(pEngineIndex*4+3, pForce.z);
				break;
			case Tbc::PhysicsEngine::ENGINE_PUSH_TURN_ABSOLUTE:
				lObject->SetEnginePower(pEngineIndex*4+0, pForce.z);
				lObject->SetEnginePower(pEngineIndex*4+1, pForce.x);
				lObject->SetEnginePower(pEngineIndex*4+3, pForce.y);
				break;
			default:
				lObject->SetEnginePower(pEngineIndex*4+0, pForce.x);
				lObject->SetEnginePower(pEngineIndex*4+1, pForce.y);
				lObject->SetEnginePower(pEngineIndex*4+3, pForce.z);
				break;
		}
	}
	else
	{
		// That's fine, wouldn't you say?
	}
}



void TrabantSimManager::AcceptLoop()
{
	for (;;)
	{
		TcpSocket* s = mListenerSocket->Accept();
		if (mAcceptThread->GetStopRequest())
		{
			return;
		}
		// Kill current socket.
		if (mConnectSocket)
		{
			if (mCommandThread) mCommandThread->RequestStop();
			mConnectSocket->Disconnect();
			delete mCommandThread;
			mCommandThread = 0;
			delete mConnectSocket;
			mConnectSocket = 0;
		}
		if (s)
		{
			mConnectSocket = s;
			mCommandThread = new MemberThread<TrabantSimManager>("CommandRecvThread");
			mCommandThread->Start(this, &TrabantSimManager::CommandLoop);
		}
	}
}

void TrabantSimManager::CommandLoop()
{
	char lData[128*1024];
	while (!mCommandThread->GetStopRequest())
	{
		int l = mConnectSocket->ReceiveDatagram(lData, sizeof(lData));
		if (l <= 0 || lData[l-1] != '\n')
		{
			break;
		}
		while (!mPauseButton || !mPauseButton->IsVisible())
		{
			Thread::Sleep(0.5);
			if (mCommandThread->GetStopRequest())
			{
				return;
			}
		}
		lData[l-1] = 0;	// Drop last linefeed.
		const str lCommand = astrutil::Encode(astr(lData));
		/*astr lResponse
		if (strutil::StartsWith(lCommand, _T("set-vertices") || strutil::StartsWith(lCommand, _T("set-indices"))
		{
			GetConsoleManager()->PushYieldCommand(lCommand)
		}
		else
		{*/
		//HiResTimer t(false);
		GetConsoleManager()->ExecuteCommand(lCommand);
		//mLog.Infof(_T("%s took %f s."), lCommand.substr(0, 10).c_str(), t.QueryTimeDiff());
		const astr lResponse = astrutil::Encode(((TrabantSimConsoleManager*)GetConsoleManager())->GetActiveResponse());
		if (mConnectSocket->Send(lResponse.c_str(), lResponse.length()) != (int)lResponse.length())
		{
			break;
		}
	}
	mLog.Info(_T("Terminating command thread."));
}

bool TrabantSimManager::IsControlled() const
{
	return mCommandThread && mCommandThread->IsRunning();
}



void TrabantSimManager::SaveSettings()
{
	GetConsoleManager()->ExecuteCommand(_T("save-application-config-file ")+GetApplicationCommandFilename());
}

bool TrabantSimManager::Open()
{
	bool lOk = Parent::Open();
	if (lOk)
	{
		mPauseButton = new UiTbc::Button(GREEN_BUTTON, "Pause");
		int x = mRenderArea.mLeft + 2;
		int y = mRenderArea.mTop + 2;
		mUiManager->GetDesktopWindow()->AddChild(mPauseButton, x, y);
		mPauseButton->SetPreferredSize(70,30);
		mPauseButton->SetRoundedRadius(4);
		mPauseButton->SetVisible(true);
		mPauseButton->SetOnClick(TrabantSimManager, OnPauseButton);
	}
	if (lOk)
	{
		mMenu = new Life::Menu(mUiManager, GetResourceManager());
	}
	return lOk;
}

void TrabantSimManager::Close()
{
	ScopeLock lLock(GetTickLock());
	delete mPauseButton;
	mPauseButton = 0;
	delete mMenu;
	mMenu = 0;
	if (mLight)
	{
		delete mLight;
		mLight = 0;
	}
	Parent::Close();
}

void TrabantSimManager::SetIsQuitting()
{
	((TrabantSimConsoleManager*)GetConsoleManager())->GetUiConsole()->SetVisible(false);
	Parent::SetIsQuitting();
}

void TrabantSimManager::SetFade(float pFadeAmount)
{
	(void)pFadeAmount;
}



Cure::RuntimeVariableScope* TrabantSimManager::GetVariableScope() const
{
	return (Parent::GetVariableScope());
}



bool TrabantSimManager::InitializeUniverse()
{
	mLight = new Light(mUiManager);
	return true;
}

void TrabantSimManager::TickInput()
{
	TickNetworkInput();
	TickUiInput();
}

void TrabantSimManager::TickUiInput()
{
	mUiManager->GetInputManager()->SetCursorVisible(true);
}

void TrabantSimManager::TickUiUpdate()
{
	((TrabantSimConsoleManager*)GetConsoleManager())->GetUiConsole()->Tick();
}

void TrabantSimManager::SetLocalRender(bool pRender)
{
	(void)pRender;
}



Cure::ContextObject* TrabantSimManager::CreateContextObject(const str& pClassId) const
{
	UiCure::Machine* lObject = new Object(GetResourceManager(), pClassId, mUiManager);
	lObject->SetAllowNetworkLogic(true);
	return lObject;
}

void TrabantSimManager::OnLoadCompleted(Cure::ContextObject* pObject, bool pOk)
{
	if (!pOk)
	{
		mLog.Errorf(_T("Could not load object of type %s."), pObject->GetClassId().c_str());
		GetContext()->PostKillObject(pObject->GetInstanceId());
	}
}

void TrabantSimManager::OnCollision(const vec3& pForce, const vec3& pTorque, const vec3& pPosition,
	Cure::ContextObject* pObject1, Cure::ContextObject* pObject2,
	Tbc::PhysicsManager::BodyID pBody1Id, Tbc::PhysicsManager::BodyID pBody2Id)
{
	(void)pBody2Id;
	mCollisionSoundManager->OnCollision(pForce, pTorque, pPosition, pObject1, pObject2, pBody1Id, 5000, false);
}



void TrabantSimManager::OnPauseButton(UiTbc::Button* pButton)
{
	if (pButton)
	{
		mMenu->OnTapSound(pButton);
	}
	mPauseButton->SetVisible(false);

	UiTbc::Dialog* d = mMenu->CreateTbcDialog(Life::Menu::ButtonAction(this, &TrabantSimManager::OnMenuAlternative), 0.5f, 0.3f);
	d->SetColor(BG_COLOR, OFF_BLACK, BLACK, BLACK);
	d->SetDirection(+1, false);
	UiTbc::FixedLayouter lLayouter(d);

	UiTbc::Label* lLabel = new UiTbc::Label(BRIGHT_TEXT, _T("Paused"));
	lLabel->SetFontId(mUiManager->SetScaleFont(1.2f));
	mUiManager->SetMasterFont();
	lLabel->SetIcon(UiTbc::Painter::INVALID_IMAGEID, UiTbc::TextComponent::ICON_CENTER);
	lLabel->SetAdaptive(false);
	lLayouter.AddComponent(lLabel, 0, 2, 0, 1, 1);

	UiTbc::Button* lUnpauseButton = new UiTbc::Button(GREEN_BUTTON, _T(">"));
	lLayouter.AddButton(lUnpauseButton, -1000, 1, 2, 0, 1, 1, true);

	v_set(GetVariableScope(), RTVAR_PHYSICS_HALT, true);
}

void TrabantSimManager::OnMenuAlternative(UiTbc::Button*)
{
	mPauseButton->SetVisible(true);
	HiResTimer::StepCounterShadow();
	v_set(GetVariableScope(), RTVAR_PHYSICS_HALT, false);
}



void TrabantSimManager::ScriptPhysicsTick()
{
	// Camera moves in a "moving average" kinda curve (halfs the distance in x seconds).
	const float lPhysicsTime = GetTimeManager()->GetAffordedPhysicsTotalTime();
	if (lPhysicsTime > 1e-5)
	{
		float gx,gy,gz;
		v_get(gx, =(float), GetVariableScope(), RTVAR_PHYSICS_GRAVITYX, 0.0);
		v_get(gy, =(float), GetVariableScope(), RTVAR_PHYSICS_GRAVITYY, 0.0);
		v_get(gz, =(float), GetVariableScope(), RTVAR_PHYSICS_GRAVITYZ, -9.8);
		GetPhysicsManager()->SetGravity(vec3(gx,gy,gz));
		MoveCamera(lPhysicsTime);
		mLight->Tick(mCameraTransform.mOrientation);
		UpdateCameraPosition(false);

		UpdateUserMessage();
	}

	Parent::ScriptPhysicsTick();
	GetResourceManager()->ForceFreeCache();
}

void TrabantSimManager::MoveCamera(float pFrameTime)
{
	int ctgt = 0;
	float clx,cly,clz,cdist,cax,cay,caz,crx,cry,crz;
	bool car;
	v_get(ctgt, =, GetVariableScope(), RTVAR_UI_3D_CAMTARGETOBJECT, 0);
	v_get(car, =, GetVariableScope(), RTVAR_UI_3D_CAMANGLERELATIVE, false);
	v_get(clx, =(float), GetVariableScope(), RTVAR_UI_3D_CAMLOOKX, 0.0);
	v_get(cly, =(float), GetVariableScope(), RTVAR_UI_3D_CAMLOOKY, 0.0);
	v_get(clz, =(float), GetVariableScope(), RTVAR_UI_3D_CAMLOOKZ, 0.0);
	v_get(cdist, =(float), GetVariableScope(), RTVAR_UI_3D_CAMDISTANCE, 3.0);
	v_get(cax, =(float), GetVariableScope(), RTVAR_UI_3D_CAMANGLEX, 0.0);
	v_get(cay, =(float), GetVariableScope(), RTVAR_UI_3D_CAMANGLEY, 0.0);
	v_get(caz, =(float), GetVariableScope(), RTVAR_UI_3D_CAMANGLEZ, 0.0);
	v_get(crx, =(float), GetVariableScope(), RTVAR_UI_3D_CAMROTATEX, 0.0);
	v_get(cry, =(float), GetVariableScope(), RTVAR_UI_3D_CAMROTATEY, 0.0);
	v_get(crz, =(float), GetVariableScope(), RTVAR_UI_3D_CAMROTATEZ, 0.0);
	quat q;
	vec3 lLookAt(clx,cly,clz);
	Cure::ContextObject* lObject = 0;
	if (ctgt)
	{
		lObject = GetContext()->GetObject(ctgt);
		if (lObject)
		{
			lLookAt = lObject->GetPosition();
		}
	}
	xform t(quat(), lLookAt+vec3(0,-cdist,0));
	t.RotateAroundAnchor(lLookAt, vec3(0,1,0), cay);
	t.RotateAroundAnchor(lLookAt, vec3(1,0,0), cax);
	t.RotateAroundAnchor(lLookAt, vec3(0,0,1), caz);
	if (lObject && car)
	{
		vec3 a;
		lObject->GetOrientation().GetEulerAngles(a);
		t.RotateAroundAnchor(lLookAt, vec3(0,0,1), a.x);
		t.RotateAroundAnchor(lLookAt, vec3(1,0,0), a.y-PIF/2);
		t.RotateAroundAnchor(lLookAt, vec3(0,1,0), a.z);
	}
	t.RotateAroundAnchor(lLookAt, vec3(0,1,0), mCameraAngle.y);
	t.RotateAroundAnchor(lLookAt, vec3(1,0,0), mCameraAngle.x);
	t.RotateAroundAnchor(lLookAt, vec3(0,0,1), mCameraAngle.z);
	mCameraAngle.x = fmod(mCameraAngle.x+crx*pFrameTime*2*PIF,2*PIF);
	mCameraAngle.y = fmod(mCameraAngle.y+cry*pFrameTime*2*PIF,2*PIF);
	mCameraAngle.z = fmod(mCameraAngle.z+crz*pFrameTime*2*PIF,2*PIF);
	mCameraTransform = t;
}

void TrabantSimManager::UpdateCameraPosition(bool pUpdateMicPosition)
{
	mUiManager->SetCameraPosition(mCameraTransform);
	if (pUpdateMicPosition)
	{
		mUiManager->SetMicrophonePosition(mCameraTransform, vec3());
	}
}

void TrabantSimManager::UpdateUserMessage()
{
	str lUserMessage;
	v_get(lUserMessage, =, GetVariableScope(), RTVAR_GAME_USERMESSAGE, _T(" "));
	if (strutil::Strip(lUserMessage, _T(" \t\r\n")).empty())
	{
		if (mUserInfoDialog)
		{
			mMenu->DismissDialog();
			mUserInfoDialog = 0;
			mUserInfoLabel = 0;
		}
	}
	else
	{
		if (mUserInfoDialog)
		{
			if (mUserInfoLabel->GetText() != lUserMessage)
			{
				mUserInfoLabel->SetText(lUserMessage);
			}
		}
		else
		{
			UiTbc::Dialog* d = mMenu->CreateTbcDialog(Life::Menu::ButtonAction(this, &TrabantSimManager::OnMenuAlternative), 0.8f, 0.5f);
			if (!d)
			{
				return;
			}
			d->SetColor(BG_COLOR, OFF_BLACK, BLACK, BLACK);
			d->SetDirection(+1, false);
			UiTbc::FixedLayouter lLayouter(d);

			UiTbc::Label* lLabel = new UiTbc::Label(BRIGHT_TEXT, lUserMessage);
			lLabel->SetIcon(UiTbc::Painter::INVALID_IMAGEID, UiTbc::TextComponent::ICON_CENTER);
			lLabel->SetAdaptive(false);
			lLayouter.AddComponent(lLabel, 0, 1, 0, 1, 1);
			mUserInfoDialog = d;
			mUserInfoLabel = lLabel;
		}
	}
}



void TrabantSimManager::PrintText(const str& s, int x, int y) const
{
	Color lOldColor = mUiManager->GetPainter()->GetColor(0);
	mUiManager->GetPainter()->SetColor(DARK_BLUE, 0);
	mUiManager->GetPainter()->PrintText(s, x, y+1);
	mUiManager->GetPainter()->SetColor(lOldColor, 0);
	mUiManager->GetPainter()->PrintText(s, x, y);
}

void TrabantSimManager::DrawImage(UiTbc::Painter::ImageID pImageId, float cx, float cy, float w, float h, float pAngle) const
{
	cx -= 0.5f;

	const float ca = ::cos(pAngle);
	const float sa = ::sin(pAngle);
	const float w2 = w*0.5f;
	const float h2 = h*0.5f;
	const float x = cx - w2*ca - h2*sa;
	const float y = cy - h2*ca + w2*sa;
	const vec2 c[] = { vec2(x, y), vec2(x+w*ca, y-w*sa), vec2(x+w*ca+h*sa, y+h*ca-w*sa), vec2(x+h*sa, y+h*ca) };
	const vec2 t[] = { vec2(0, 0), vec2(1, 0), vec2(1, 1), vec2(0, 1) };
#define V(z) std::vector<vec2>(z, z+LEPRA_ARRAY_COUNT(z))
	mUiManager->GetPainter()->DrawImageFan(pImageId, V(c), V(t));
}



void TrabantSimManager::PainterImageLoadCallback(UiCure::UserPainterKeepImageResource* pResource)
{
	if (pResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		mUiManager->GetDesktopWindow()->GetImageManager()->AddLoadedImage(*pResource->GetRamData(), pResource->GetData(),
			UiTbc::GUIImageManager::CENTERED, UiTbc::GUIImageManager::ALPHABLEND, 255);
	}
}



loginstance(GAME, TrabantSimManager);



}
