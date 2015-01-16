
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
#include "../UiLepra/Include/UiTouchstick.h"
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



TrabantSimManager::JoystickData::JoystickData(int pJoystickId, float px, float py):
	mJoystickId(pJoystickId),
	x(px),
	y(py)
{
}

TrabantSimManager::TouchstickInfo::TouchstickInfo(Touchstick* pStick, float px, float py, int pOrientation, bool pIsSloppy):
	mStick(pStick),
	x(px),
	y(py),
	mOrientation(pOrientation),
	mIsSloppy(pIsSloppy)
{
}

TrabantSimManager::EngineTarget::EngineTarget(int pInstanceId, float pStrength):
	mInstanceId(pInstanceId),
	mStrength(pStrength)
{
}



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
		if (strutil::StartsWith(lName, _T("Ui.3D.Clear")) || strutil::StartsWith(lName, _T("Ui.3D.Clip")) || strutil::StartsWith(lName, _T("Ui.Pen")))
		{
			continue;
		}
		lScope->ResetDefaultValue(lName);
	}
	mCameraAngle.Set(0,0,0);

	GetResourceManager()->ForceFreeCache();
	GetResourceManager()->ForceFreeCache();
	GetResourceManager()->ForceFreeCache();

	ScopeLock lGameLock(GetTickLock());

	mCollisionList.clear();

	TouchstickList::iterator y = mTouchstickList.begin();
	for (; y != mTouchstickList.end(); ++y)
	{
		delete y->mStick;
	}
	mTouchstickList.clear();
}

int TrabantSimManager::CreateObject(const MeshObject& pGfxObject, const PhysObjectArray& pPhysObjects, ObjectMaterial pMaterial, bool pIsStatic)
{
	ScopeLock lPhysLock(GetMaster()->GetPhysicsLock());
	ScopeLock lGameLock(GetTickLock());

	quat pq;
	Object* lObject = (Object*)Parent::CreateContextObject(_T("object"), Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED, 0);
	Tbc::ChunkyPhysics* lPhysics = new Tbc::ChunkyPhysics(Tbc::BoneHierarchy::TRANSFORM_LOCAL2WORLD, pIsStatic? Tbc::ChunkyPhysics::STATIC : Tbc::ChunkyPhysics::DYNAMIC);
	lPhysics->SetGuideMode(Tbc::ChunkyPhysics::GUIDE_ALWAYS);
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
			Tbc::ChunkyBoneGeometry::BodyData lBoneData(y==0? 15.0f:1.0f, lFriction, lBounce, lParent, Tbc::ChunkyBoneGeometry::JOINT_EXCLUDE, Tbc::ChunkyBoneGeometry::CONNECTEE_3DOF);
			PlacedObject* lPhysObject = *x;
			xform t = xform(lPhysObject->mOrientation, lPhysObject->mPos);
			if (lParentPhysObject)
			{
				t.mPosition = pq.GetInverse() * t.mPosition;
				t.mOrientation = pq.GetInverse() * t.mOrientation;
			}
			else
			{
				pq = t.mOrientation;
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
	lObject->SetRootOrientation(pq);
	lObject->CreatePhysics(lPhysics);
	float r,g,b;
	v_get(r, =(float), GetVariableScope(), RTVAR_UI_PENRED, 0.5);
	v_get(g, =(float), GetVariableScope(), RTVAR_UI_PENGREEN, 0.5);
	v_get(b, =(float), GetVariableScope(), RTVAR_UI_PENBLUE, 0.5);
	const bool lIsSmooth = (pMaterial == MaterialSmooth);
	lObject->AddGfxMesh(pGfxObject.mVertices, pGfxObject.mIndices, vec3(r,g,b), lIsSmooth, pIsStatic? -1 : 1);
	lObject->GetMeshResource(0)->mOffset.mOffset.mOrientation = pGfxObject.mOrientation;
	lObject->mInitialOrientation = pq;
	lObject->mInitialInverseOrientation = pq.GetInverse();
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

void TrabantSimManager::PopCollisions(CollisionList& pCollisionList)
{
	ScopeLock lGameLock(GetTickLock());
	pCollisionList.splice(pCollisionList.end(), mCollisionList);
}

void TrabantSimManager::GetTouchDrags(DragList& pDragList) const
{
	ScopeLock lGameLock(GetTickLock());
	pDragList = mUiManager->GetDragManager()->GetDragList();
}

vec3 TrabantSimManager::GetAccelerometer() const
{
	return mUiManager->GetAccelerometer();
}

int TrabantSimManager::CreateJoystick(float x, float y, bool pIsSloppy)
{
	ScopeLock lGameLock(GetTickLock());
	const float lTouchSideScale = 1.28f;	// Inches.
	const float lTouchScale = lTouchSideScale / (float)mUiManager->GetDisplayManager()->GetPhysicalScreenSize();
	const int lScreenPixelWidth = mUiManager->GetDisplayManager()->GetWidth();
	const int lMinimumTouchRadius = (int)(lScreenPixelWidth*lTouchScale*0.17f);	// 30 pixels in iPhone classic.
	PixelRect lRect(0,0,10,10);
	Touchstick* lStick = new Touchstick(mUiManager->GetInputManager(), UiLepra::Touch::TouchstickInputDevice::MODE_RELATIVE_CENTER, lRect, 0, lMinimumTouchRadius);
	const str lName = strutil::Format(_T("Touchstick%i"), mTouchstickList.size());
	lStick->SetUniqueIdentifier(lName);
	mTouchstickList.push_back(TouchstickInfo(lStick, x, y, -90, pIsSloppy));
	mTouchstickTimer.ReduceTimeDiff(-10);
	return mTouchstickList.size()-1;
}

TrabantSimManager::JoystickDataList TrabantSimManager::GetJoystickData() const
{
	ScopeLock lGameLock(GetTickLock());
	JoystickDataList lList;
	int lId = 0;
	TouchstickList::const_iterator x = mTouchstickList.begin();
	for (; x != mTouchstickList.end(); ++x, ++lId)
	{
		float jx = 0;
		float jy = 0;
		bool lIsPressing = false;
		x->mStick->GetValue(jx, jy, lIsPressing);
		if (lIsPressing)
		{
			lList.push_back(JoystickData(lId, jx, -jy));
		}
		if (!x->mIsSloppy)
		{
			x->mStick->ResetTap();
		}
	}
	return lList;
}

float TrabantSimManager::GetAspectRatio() const
{
	return mUiManager->GetDisplayManager()->GetWidth()/(float)mUiManager->GetDisplayManager()->GetHeight();
}

int TrabantSimManager::CreateEngine(int pObjectId, const str& pEngineType, const vec2& pMaxVelocity, float pStrength, float pFriction, const EngineTargetList& pEngineTargets)
{
	ScopeLock lPhysLock(GetMaster()->GetPhysicsLock());
	ScopeLock lGameLock(GetTickLock());
	Object* lObject = (Object*)GetContext()->GetObject(pObjectId);
	if (!lObject)
	{
		return -1;
	}

	bool lIsAttachment = false;
	Tbc::PhysicsEngine::EngineType lEngineType;
	vec2 lMaxVelocity(pMaxVelocity);
	float lStrength = pStrength * lObject->GetMass();
	float lFriction = pFriction*2;
	if (pEngineType == _T("roll_turn"))
	{
		lEngineType = Tbc::PhysicsEngine::ENGINE_HINGE2_TURN;
		lMaxVelocity.x = (!lMaxVelocity.x)? 2.0f : lMaxVelocity.x;
		lMaxVelocity.y = (!lMaxVelocity.y)? 2.0f : lMaxVelocity.y;
		lIsAttachment = true;
	}
	else if (pEngineType == _T("roll"))
	{
		lEngineType = Tbc::PhysicsEngine::ENGINE_HINGE_ROLL;
		lMaxVelocity.x = (!lMaxVelocity.x)? 100.0f : lMaxVelocity.x;
		lMaxVelocity.y = (!lMaxVelocity.y)? -20.0f : lMaxVelocity.y;
		lStrength *= 10;
		lIsAttachment = true;
	}
	else if (pEngineType == _T("push_abs"))
	{
		lEngineType = Tbc::PhysicsEngine::ENGINE_PUSH_ABSOLUTE;
		lMaxVelocity.x = (!lMaxVelocity.x)? 100.0f : lMaxVelocity.x;
	}
	else if (pEngineType == _T("push_rel"))
	{
		lEngineType = Tbc::PhysicsEngine::ENGINE_PUSH_RELATIVE;
		lMaxVelocity.x = (!lMaxVelocity.x)? 100.0f : lMaxVelocity.x;
	}
	else if (pEngineType == _T("push_turn_abs"))
	{
		lEngineType = Tbc::PhysicsEngine::ENGINE_PUSH_TURN_ABSOLUTE;
		lMaxVelocity.x = (!lMaxVelocity.x)? 0.1f : lMaxVelocity.x;
	}
	else if (pEngineType == _T("push_turn_rel"))
	{
		lEngineType = Tbc::PhysicsEngine::ENGINE_PUSH_TURN_RELATIVE;
		lMaxVelocity.x = (!lMaxVelocity.x)? 0.1f : lMaxVelocity.x;
	}
	else if (pEngineType == _T("gyro"))
	{
		lEngineType = Tbc::PhysicsEngine::ENGINE_HINGE_GYRO;
		lMaxVelocity.x = (!lMaxVelocity.x)? 150.0f : lMaxVelocity.x;
		lMaxVelocity.y = (!lMaxVelocity.y)? 40.0f : lMaxVelocity.y;
		lStrength *= 3;
		lFriction = lFriction? lFriction : 0.01f;
		lIsAttachment = true;
	}
	else if (pEngineType == _T("rotor"))
	{
		lEngineType = Tbc::PhysicsEngine::ENGINE_ROTOR;
		lStrength *= 0.4f;
		lFriction = lFriction? lFriction : 0.01f;
		lIsAttachment = true;
	}
	else if (pEngineType == _T("tilt"))
	{
		lEngineType = Tbc::PhysicsEngine::ENGINE_ROTOR_TILT;
		lStrength /= 15;
		lIsAttachment = true;
	}
	else
	{
		return -1;
	}

	EngineTargetList lTargets(pEngineTargets);
	if (lTargets.empty())
	{
		Object::Array lObjects = lObject->GetAttachedObjects();
		Cure::ContextObject* lLastAttachedObject = lObjects.empty()? 0 : lObjects.back();
		Cure::ContextObject* lEngineObject = lIsAttachment? lLastAttachedObject : lObject;
		if (!lEngineObject)
		{
			mLog.Warningf(_T("No object attached to create a %s engine to."), pEngineType.c_str());
			return -1;
		}
		lTargets.push_back(EngineTarget(lEngineObject->GetInstanceId(),1));
	}
	Tbc::PhysicsEngine* lEngine = new Tbc::PhysicsEngine(lEngineType, lStrength, lMaxVelocity.x, lMaxVelocity.y, lFriction, lObject->GetPhysics()->GetEngineCount()*4);
	EngineTargetList::iterator x = lTargets.begin();
	for (; x != lTargets.end(); ++x)
	{
		Cure::ContextObject* lEngineObject = GetContext()->GetObject(x->mInstanceId);
		if (!lEngineObject)
		{
			mLog.Warningf(_T("No object %i attached to create a %s engine to."), x->mInstanceId, pEngineType.c_str());
			delete lEngine;
			return -1;
		}
		Tbc::ChunkyBoneGeometry* lGeometry = lEngineObject->GetPhysics()->GetBoneGeometry(0);
		lEngine->AddControlledGeometry(lGeometry, x->mStrength);
	}
	lObject->GetPhysics()->AddEngine(lEngine);
	if (lIsAttachment)
	{
		EngineTargetList::iterator x = lTargets.begin();
		for (; x != lTargets.end(); ++x)
		{
			Cure::ContextObject* lEngineObject = GetContext()->GetObject(x->mInstanceId);
			lObject->AddAttachedObjectEngine(lEngineObject, lEngine);
		}
	}
	GetContext()->EnableMicroTickCallback(lObject);
	return lObject->GetPhysics()->GetEngineCount()-1;
}

int TrabantSimManager::CreateJoint(int pObjectId, const str& pJointType, int pOtherObjectId, const vec3& pAxis, const vec2& pStop, const vec2& pSpringSettings)
{
	ScopeLock lPhysLock(GetMaster()->GetPhysicsLock());
	ScopeLock lGameLock(GetTickLock());
	Object* lObject = (Object*)GetContext()->GetObject(pObjectId);
	Object* lObject2 = (Object*)GetContext()->GetObject(pOtherObjectId);
	if (!lObject || !lObject2)
	{
		return -1;
	}

	Tbc::ChunkyBoneGeometry::ConnectorType lType;
	float lLoStop = pStop.x;
	float lHiStop = pStop.y;
	float lSpringConstant = pSpringSettings.x;
	float lSpringDamping = pSpringSettings.y;
	if (pJointType == _T("hinge"))
	{
		lType = Tbc::ChunkyBoneGeometry::CONNECTOR_HINGE;
		if (lLoStop == 0 && lHiStop == 0)
		{
			lLoStop = -100.0f;
			lHiStop = +100.0f;
		}
	}
	else if (pJointType == _T("suspend_hinge"))
	{
		lType = Tbc::ChunkyBoneGeometry::CONNECTOR_SUSPEND_HINGE;
		lSpringConstant = (lSpringConstant<=0)? 22 : lSpringConstant;
		lSpringDamping = (lSpringDamping<=0)? 0.8f : lSpringDamping;
		lSpringConstant *= lObject->GetMass() * 100;
	}
	else if (pJointType == _T("turn_hinge"))
	{
		lType = Tbc::ChunkyBoneGeometry::CONNECTOR_HINGE2;
		if (lLoStop == 0 && lHiStop == 0)
		{
			lLoStop = -0.5f;
			lHiStop = +0.5f;
		}
		lSpringConstant = (lSpringConstant<=0)? 22 : lSpringConstant;
		lSpringDamping = (lSpringDamping<=0)? 0.8f : lSpringDamping;
		lSpringConstant *= lObject->GetMass() * 100;
	}
	else if (pJointType == _T("ball"))
	{
		lType = Tbc::ChunkyBoneGeometry::CONNECTOR_3DOF;
	}
	else if (pJointType == _T("slider"))
	{
		lType = Tbc::ChunkyBoneGeometry::CONNECTOR_SLIDER;
	}
	else if (pJointType == _T("universal"))
	{
		lType = Tbc::ChunkyBoneGeometry::CONNECTOR_UNIVERSAL;
	}
	else
	{
		return -1;
	}

	lObject->GetPhysics()->GetBoneGeometry(0)->ClearConnectorTypes();
	lObject->GetPhysics()->GetBoneGeometry(0)->AddConnectorType(lType);
	Tbc::ChunkyBoneGeometry::BodyDataBase& lBodyData = lObject2->GetPhysics()->GetBoneGeometry(0)->GetBodyData();
	lBodyData.mParameter[Tbc::ChunkyBoneGeometry::PARAM_EULER_THETA] = pAxis.GetPolarCoordAngleZ();
	lBodyData.mParameter[Tbc::ChunkyBoneGeometry::PARAM_EULER_PHI] = pAxis.GetAngle(vec3(0,0,1));
	lBodyData.mParameter[Tbc::ChunkyBoneGeometry::PARAM_LOW_STOP] = lLoStop;
	lBodyData.mParameter[Tbc::ChunkyBoneGeometry::PARAM_HIGH_STOP] = lHiStop;
	lBodyData.mParameter[Tbc::ChunkyBoneGeometry::PARAM_SPRING_CONSTANT] = lSpringConstant;
	lBodyData.mParameter[Tbc::ChunkyBoneGeometry::PARAM_SPRING_DAMPING] = lSpringDamping;
	lBodyData.mParameter[Tbc::ChunkyBoneGeometry::PARAM_OFFSET_X] = 0;
	lBodyData.mParameter[Tbc::ChunkyBoneGeometry::PARAM_OFFSET_Y] = 0;
	lBodyData.mParameter[Tbc::ChunkyBoneGeometry::PARAM_OFFSET_Z] = 0;
	lObject->AttachToObjectByBodyIndices(0, lObject2, 0);
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
		pOrientation = pOrientation * lObject->mInitialOrientation;
		vec3 lPosition = lObject->GetPosition();
		GetPhysicsManager()->SetBodyTransform(lObject->GetPhysics()->GetBoneGeometry(0)->GetBodyId(), xform(pOrientation, lPosition));
	}
	else
	{
		xform t;
		GetPhysicsManager()->GetBodyTransform(lObject->GetPhysics()->GetBoneGeometry(0)->GetBodyId(), t);
		pOrientation = t.mOrientation * lObject->mInitialInverseOrientation;
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
		lObject->QueryMass();
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
	if (pEngineIndex >= lObject->GetPhysics()->GetEngineCount())
	{
		mLog.Warningf(_T("Object %i does not have an engine with index %i."), pObjectId, pEngineIndex);
		return;
	}
	if (pSet)
	{
		switch (lObject->GetPhysics()->GetEngine(pEngineIndex)->GetEngineType())
		{
			case Tbc::PhysicsEngine::ENGINE_PUSH_ABSOLUTE:
			case Tbc::PhysicsEngine::ENGINE_PUSH_RELATIVE:
				lObject->SetEnginePower(pEngineIndex*4+0, pForce.y);
				lObject->SetEnginePower(pEngineIndex*4+1, pForce.x);
				lObject->SetEnginePower(pEngineIndex*4+3, pForce.z);
				break;
			case Tbc::PhysicsEngine::ENGINE_PUSH_TURN_ABSOLUTE:
			case Tbc::PhysicsEngine::ENGINE_PUSH_TURN_RELATIVE:
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

void TrabantSimManager::AddTag(int pObjectId, const str& pTagType, const FloatList& pFloats, const StringList& pStrings, const IntList& pPhys, const IntList& pEngines, const IntList& pMeshes)
{
	ScopeLock lPhysLock(GetMaster()->GetPhysicsLock());
	ScopeLock lGameLock(GetTickLock());
	Object* lObject = (Object*)GetContext()->GetObject(pObjectId);
	if (!lObject)
	{
		return;
	}
	const Tbc::ChunkyPhysics* lPhysics = lObject->GetPhysics();
	for (IntList::const_iterator x = pPhys.begin(); x != pPhys.end(); ++x)
	{
		if (*x >= lPhysics->GetBoneCount())
		{
			mLog.Warningf(_T("Object %i does not have a body with index %i."), pObjectId, *x);
			return;
		}
	}
	for (IntList::const_iterator x = pEngines.begin(); x != pEngines.end(); ++x)
	{
		if (*x >= lPhysics->GetEngineCount())
		{
			mLog.Warningf(_T("Object %i does not have an engine with index %i."), pObjectId, *x);
			return;
		}
	}
	for (IntList::const_iterator x = pMeshes.begin(); x != pMeshes.end(); ++x)
	{
		if (*x >= (int)((UiTbc::ChunkyClass*)lObject->GetClass())->GetMeshCount())
		{
			mLog.Warningf(_T("Object %i does not have a mesh with index %i."), pObjectId, *x);
			return;
		}
	}
	Tbc::ChunkyClass::Tag lTag;
	lTag.mTagName = pTagType;
	lTag.mFloatValueList = pFloats;
	lTag.mStringValueList = pStrings;
	lTag.mBodyIndexList = pPhys;
	lTag.mEngineIndexList = pEngines;
	lTag.mMeshIndexList = pMeshes;
	((Tbc::ChunkyClass*)lObject->GetClass())->AddTag(lTag);
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
	v_set(GetVariableScope(), RTVAR_UI_SOUND_MASTERVOLUME, 1.0);
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
	v_set(GetVariableScope(), RTVAR_UI_SOUND_MASTERVOLUME, 0.0);
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

void TrabantSimManager::SetRenderArea(const PixelRect& pRenderArea)
{
	Parent::SetRenderArea(pRenderArea);
	UpdateTouchstickPlacement();
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



bool TrabantSimManager::Paint()
{
	if (!Parent::Paint())
	{
		return false;
	}

	float r, g, b;
	v_get(r, =1-(float), UiCure::GetSettings(), RTVAR_UI_3D_CLEARRED, 1.0);
	v_get(g, =1-(float), UiCure::GetSettings(), RTVAR_UI_3D_CLEARGREEN, 1.0);
	v_get(b, =1-(float), UiCure::GetSettings(), RTVAR_UI_3D_CLEARBLUE, 1.0);
	Color lColor = Color::CreateColor(r,g,b,1);
	mUiManager->GetPainter()->SetColor(lColor, 0);
	TouchstickList::iterator x = mTouchstickList.begin();
	for (; x != mTouchstickList.end(); ++x)
	{
		DrawStick(x->mStick, x->mIsSloppy);
	}
	return true;
}

void TrabantSimManager::DrawStick(Touchstick* pStick, bool pIsSloppy)
{
	PixelRect lArea = pStick->GetArea();
	const int ow = lArea.GetWidth();
	const int lMargin = pStick->GetFingerRadius() / 8;
	const int r = pStick->GetFingerRadius() - lMargin;
	lArea.Shrink(lMargin*2);
	mUiManager->GetPainter()->DrawArc(lArea.mLeft, lArea.mTop, lArea.GetWidth(), lArea.GetHeight(), 0, 360, false);
	float x;
	float y;
	bool lIsPressing;
	pStick->GetValue(x, y, lIsPressing);
	if (lIsPressing || pIsSloppy)
	{
		vec2 v(x, y);
		v.Mul((ow+lMargin*2) / (float)ow);
		const float lLength = v.GetLength();
		if (lLength > 1)
		{
			v.Div(lLength);
		}
		x = v.x;
		y = v.y;
		x = 0.5f*x + 0.5f;
		y = 0.5f*y + 0.5f;
		const int w = lArea.GetWidth()  - r*2;
		const int h = lArea.GetHeight() - r*2;
		mUiManager->GetPainter()->DrawArc(
			lArea.mLeft + (int)(w*x),
			lArea.mTop  + (int)(h*y),
			r*2, r*2, 0, 360, true);
	}
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

void TrabantSimManager::UpdateTouchstickPlacement()
{
	if (mTouchstickTimer.QueryTimeDiff() < 3.0)
	{
		return;
	}
	mTouchstickTimer.ClearTimeDiff();

	const float lTouchSideScale = 1.28f;	// Inches.
	const float lTouchScale = lTouchSideScale / (float)mUiManager->GetDisplayManager()->GetPhysicalScreenSize();
	const int lSide = std::max((int)(mRenderArea.GetHeight() * lTouchScale), 80);
	TouchstickList::iterator x = mTouchstickList.begin();
	for (; x != mTouchstickList.end(); ++x)
	{
		PixelRect lStickArea(mRenderArea);
		lStickArea.mTop = lStickArea.mBottom - lSide;
		lStickArea.mRight = lStickArea.mLeft + lStickArea.GetHeight();
		const int ox = (int)(x->x * (mRenderArea.GetWidth()-lSide));
		const int oy = -(int)(x->y * (mRenderArea.GetHeight()-lSide));
		lStickArea.Offset(ox,oy);
		x->mStick->Move(lStickArea, x->mOrientation);
	}
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

	ScopeLock lGameLock(GetTickLock());
	CollisionInfo ci;
	ci.mObjectId = pObject1->GetInstanceId();
	ci.mForce = pForce;
	ci.mPosition = pPosition;
	ci.mOtherObjectId = pObject2->GetInstanceId();
	mCollisionList.push_back(ci);
	if (mCollisionList.size() > 300)
	{
		mCollisionList.pop_front();
	}
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
	v_get(clx, =(float), GetVariableScope(), RTVAR_UI_3D_CAMLOOKATX, 0.0);
	v_get(cly, =(float), GetVariableScope(), RTVAR_UI_3D_CAMLOOKATY, 0.0);
	v_get(clz, =(float), GetVariableScope(), RTVAR_UI_3D_CAMLOOKATZ, 0.0);
	v_get(cdist, =(float), GetVariableScope(), RTVAR_UI_3D_CAMDISTANCE, 3.0);
	v_get(cax, =(float), GetVariableScope(), RTVAR_UI_3D_CAMANGLEX, 0.0);
	v_get(cay, =(float), GetVariableScope(), RTVAR_UI_3D_CAMANGLEY, 0.0);
	v_get(caz, =(float), GetVariableScope(), RTVAR_UI_3D_CAMANGLEZ, 0.0);
	v_get(crx, =(float), GetVariableScope(), RTVAR_UI_3D_CAMROTATEX, 0.0);
	v_get(cry, =(float), GetVariableScope(), RTVAR_UI_3D_CAMROTATEY, 0.0);
	v_get(crz, =(float), GetVariableScope(), RTVAR_UI_3D_CAMROTATEZ, 0.0);
	vec3 lLookAt(clx,cly,clz);
	Object* lObject = 0;
	if (ctgt)
	{
		lObject = (Object*)GetContext()->GetObject(ctgt);
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
		xform pt;
		GetPhysicsManager()->GetBodyTransform(lObject->GetPhysics()->GetBoneGeometry(0)->GetBodyId(), pt);
		quat q = pt.mOrientation * lObject->mInitialInverseOrientation;
		t.mOrientation = q * t.mOrientation;
		t.mPosition = q*(t.mPosition-lLookAt) + lLookAt;
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
