
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "TrabantSimManager.h"
#include <algorithm>
#include <iterator>
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



void FoldSimulator();



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
	mIsMouseControlled(false),
	mSetFocus(false),
	mSetCursorVisible(false),
	mSetCursorInvisible(false),
	mMenu(0),
	mLight(0),
	mCameraTransform(quat(), vec3(0, -3, 0)),
	mPauseButton(0),
	mBackButton(0),
	mIsPaused(false),
	mIsControlled(false),
	mWasControlled(false),
	mIsControlTimeout(false),
	mCommandSocket(0),
	mCommandThread(0),
	mUserInfoDialog(0),
	mUserInfoLabel(0)
{
	mCollisionSoundManager = new UiCure::CollisionSoundManager(this, pUiManager);
	mCollisionSoundManager->SetScale(1, 0.5f, 0.08f, 0.2f);
	mCollisionSoundManager->AddSound(_T("explosion"),	UiCure::CollisionSoundManager::SoundResourceInfo(0.8f, 0.4f, 0));
	mCollisionSoundManager->AddSound(_T("rubber"),		UiCure::CollisionSoundManager::SoundResourceInfo(1.0f, 0.3f, 0));

	SetConsoleManager(new TrabantSimConsoleManager(GetResourceManager(), this, mUiManager, GetVariableScope(), mRenderArea));

	GetPhysicsManager()->SetSimulationParameters(0.0f, 0.03f, 0.2f);

	SocketAddress lAddress;
	const strutil::strvec& args = SystemManager::GetArgumentVector();
	if (args.size() <= 1 || !lAddress.Resolve(args[args.size()-1]))
	{
		lAddress.Resolve(_T("0.0.0.0:2541"));
	}
	mLocalAddress = lAddress;
	mLastRemoteAddress = lAddress;
	Resume();
}

TrabantSimManager::~TrabantSimManager()
{
	CloseConnection();

	Close();

	delete mCollisionSoundManager;
	mCollisionSoundManager = 0;
}



void TrabantSimManager::Suspend()
{
	mIsControlTimeout = false;
	v_set(GetVariableScope(), RTVAR_GAME_USERMESSAGE, _T(" "));

	if (mCommandSocket && mCommandSocket->IsOpen())
	{
		if (mLastRemoteAddress != mLocalAddress)
		{
			mCommandSocket->SendTo((const unsigned char*)"disconnect\n", 11, mLastRemoteAddress);
		}
	}
}

void TrabantSimManager::Resume()
{
	mIsControlTimeout = false;
	v_set(GetVariableScope(), RTVAR_GAME_USERMESSAGE, _T(" "));

	if (mCommandSocket)
	{
		return;
	}

	mCommandSocket = new UdpSocket(mLocalAddress, true);
	if (mCommandSocket->IsOpen())
	{
		mCommandThread = new MemberThread<TrabantSimManager>("CommandRecvThread");
		mCommandThread->Start(this, &TrabantSimManager::CommandLoop);
		mLog.Headlinef(_T("Command server listening on %s."), mLocalAddress.GetAsString().c_str());
	}
	else
	{
		mLog.Headlinef(_T("Could not open server on %s. Shutting down."), mLocalAddress.GetAsString().c_str());
		SystemManager::AddQuitRequest(1);
	}
}



void TrabantSimManager::UserReset()
{
	mSetFocus = true;
	mSetCursorVisible = true;
	mIsMouseControlled = false;

	GetConsoleManager()->ExecuteCommand(_T("bind-key F5 \"\""));

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
	mCameraVelocity.Set(0,0,0);

	GetResourceManager()->ForceFreeCache();
	GetResourceManager()->ForceFreeCache();
	GetResourceManager()->ForceFreeCache();

	ScopeLock lGameLock(GetTickLock());

	mCollisionList.clear();
	mDragList.clear();
	mUiManager->GetDragManager()->ClearDrags(mUiManager->GetInputManager());

	TouchstickList::iterator y = mTouchstickList.begin();
	for (; y != mTouchstickList.end(); ++y)
	{
		delete y->mStick;
	}
	mTouchstickList.clear();
}

int TrabantSimManager::CreateObject(const quat& pOrientation, const vec3& pPosition, const MeshObject& pGfxObject, const PhysObjectArray& pPhysObjects,
					ObjectMaterial pMaterial, bool pIsStatic)
{
	ScopeLock lPhysLock(GetMaster()->GetPhysicsLock());
	ScopeLock lGameLock(GetTickLock());

	quat pq, rootq;
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

		lPhysics->SetBoneCount((int)pPhysObjects.size());
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
				t.mPosition = rootq.GetInverse() * t.mPosition;
				t.mOrientation = rootq.GetInverse() * t.mOrientation;
			}
			else
			{
				rootq = t.mOrientation;
				t.mOrientation = pOrientation * t.mOrientation;
				t.mPosition += pPosition;
				pq = t.mOrientation;
			}
			BoxObject* lBox = dynamic_cast<BoxObject*>(lPhysObject);
			SphereObject* lSphere = dynamic_cast<SphereObject*>(lPhysObject);
			CapsuleObject* lCapsule = dynamic_cast<CapsuleObject*>(lPhysObject);
			MeshObject* lMesh = dynamic_cast<MeshObject*>(lPhysObject);
			if (lBox)
			{
				Tbc::ChunkyBoneBox* lBone = new Tbc::ChunkyBoneBox(lBoneData);
				lBone->SetMaterial(_T("rubber"));
				lBone->mSize = lBox->mSize;
				lPhysics->AddBoneGeometry(t, lBone, lParent);
			}
			else if (lSphere)
			{
				Tbc::ChunkyBoneSphere* lBone = new Tbc::ChunkyBoneSphere(lBoneData);
				lBone->SetMaterial(_T("rubber"));
				lBone->mRadius = lSphere->mRadius;
				lPhysics->AddBoneGeometry(t, lBone, lParent);
			}
			else if (lCapsule)
			{
				Tbc::ChunkyBoneCapsule* lBone = new Tbc::ChunkyBoneCapsule(lBoneData);
				lBone->SetMaterial(_T("rubber"));
				lBone->mRadius = lCapsule->mRadius;
				lBone->mLength = lCapsule->mLength;
				lPhysics->AddBoneGeometry(t, lBone, lParent);
			}
			else if (lMesh)
			{
				if (lMesh->mIndices.size() <= 3)
				{
					mLog.Warningf(_T("Need two triangles or more (%i indices is too few) to create physics mesh."), lMesh->mIndices.size());
					delete lPhysics;
					return -1;
				}
				Tbc::ChunkyBoneMesh* lBone = new Tbc::ChunkyBoneMesh(lBoneData);
				lBone->SetMaterial(_T("rubber"));
				lBone->mVertexCount = (int)lMesh->mVertices.size()/3;
				lBone->mTriangleCount = (int)lMesh->mIndices.size()/3;
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
	float r,g,b,a;
	if (pMaterial == MaterialChecker)
	{
		r = g = b = a = 1;
	}
	else
	{
		v_get(r, =(float), GetVariableScope(), RTVAR_UI_PENRED, 0.5);
		v_get(g, =(float), GetVariableScope(), RTVAR_UI_PENGREEN, 0.5);
		v_get(b, =(float), GetVariableScope(), RTVAR_UI_PENBLUE, 0.5);
		v_get(a, =(float), GetVariableScope(), RTVAR_UI_PENALPHA, 1.0);
	}
	vec3 lColor(r,g,b);
	const bool lIsSmooth = (pMaterial == MaterialSmooth);
	UiTbc::TriangleBasedGeometry* lMesh = lObject->CreateGfxMesh(pGfxObject.mVertices, pGfxObject.mIndices, lColor, a, lIsSmooth);
	if (!lMesh)
	{
		delete lObject;
		return -1;
	}
	if (pMaterial == MaterialChecker)
	{
		const float lScale = (fabs(pGfxObject.mVertices[pGfxObject.mVertices.size()-3]-pGfxObject.mVertices[0]) >= 60)? 20.0f : 2.0f;
		AddCheckerTexturing(lMesh, lScale);
		lObject->LoadTexture(_T("checker.png"));
	}
	lObject->AddMeshResource(lMesh, pIsStatic? -1 : 1);
	lObject->AddMeshInfo(lObject->GetMeshResource(0)->GetName(), _T("texture"), _T("checker.png"), lColor, a);
	lObject->GetMeshResource(0)->mOffset.mOffset.mOrientation = pGfxObject.mOrientation;
	lObject->mInitialOrientation = pq;
	lObject->mInitialInverseOrientation = pq.GetInverse();
	mObjects.insert(lObject->GetInstanceId());
	return lObject->GetInstanceId();
}

void TrabantSimManager::CreateClones(IntList& pCreatedObjectIds, int pOriginalId, const XformList& pPlacements, ObjectMaterial pMaterial, bool pIsStatic)
{
	str lMeshName;
	str lPhysName;
	float r,g,b,a;
	if (pMaterial == MaterialChecker)
	{
		r = g = b = a = 1;
	}
	else
	{
		v_get(r, =(float), GetVariableScope(), RTVAR_UI_PENRED, 0.5);
		v_get(g, =(float), GetVariableScope(), RTVAR_UI_PENGREEN, 0.5);
		v_get(b, =(float), GetVariableScope(), RTVAR_UI_PENBLUE, 0.5);
		v_get(a, =(float), GetVariableScope(), RTVAR_UI_PENALPHA, 1.0);
	}
	const vec3 lColor(r,g,b);
	const bool lIsSmooth = (pMaterial == MaterialSmooth);
	quat lOriginalRoot;
	quat lOriginalOffsetOrientation;

	LEPRA_MEASURE_SCOPE(CreateClonesLock);
	ScopeLock lGameLock(GetTickLock());
	Object* lOriginal = (Object*)GetContext()->GetObject(pOriginalId);
	if (!lOriginal)
	{
		return;
	}
	lMeshName = lOriginal->GetMeshResource(0)->GetName();
	lMeshName.resize(lMeshName.find(_T(".mesh")));
	lPhysName = lOriginal->GetPhysicsResource()->GetName();
	lPhysName.resize(lPhysName.find(_T(".phys")));
	lOriginalRoot = lOriginal->GetPhysics()->GetOriginalBoneTransformation(0).mOrientation;
	lOriginalOffsetOrientation = lOriginal->GetMeshResource(0)->mOffset.mOffset.mOrientation;

	// Tricky loop to ensure we don't hold any of the mutexes very long.
	while (!GetMaster()->GetPhysicsLock()->TryAcquire())
	{
		lGameLock.Release();
		Thread::YieldCpu();
		lGameLock.Acquire();
	}
	{

		for (XformList::const_iterator x = pPlacements.begin(); x != pPlacements.end(); ++x)
		{
			Object* lObject = (Object*)Parent::CreateContextObject(_T("object"), Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED, 0);
			{
				const quat pq = x->mOrientation * lOriginalRoot;
				lObject->SetRootOrientation(pq);
				lObject->SetRootPosition(x->mPosition);
				{
					LEPRA_MEASURE_SCOPE(CreateClonesPhys);
					lObject->CreatePhysicsRef(lPhysName);
				}
				if (pMaterial == MaterialChecker)
				{
					LEPRA_MEASURE_SCOPE(CreateClonesTexture);
					lObject->LoadTexture(_T("checker.png"));
				}
				lObject->AddMeshInfo(lMeshName, _T("texture"), _T("checker.png"), lColor, a, lIsSmooth);
				{
					LEPRA_MEASURE_SCOPE(CreateClonesMesh);
					lObject->AddMeshResourceRef(lMeshName, pIsStatic? -1 : 1);
				}
				lObject->GetMeshResource(0)->mOffset.mOffset.mOrientation = lOriginalOffsetOrientation;
				lObject->mInitialOrientation = pq;
				lObject->mInitialInverseOrientation = pq.GetInverse();
				mObjects.insert(lObject->GetInstanceId());
				pCreatedObjectIds.push_back(lObject->GetInstanceId());
			}
		}
	}
	GetMaster()->GetPhysicsLock()->Release();
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
	GetContext()->SetPostKillTimeout(1);
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
	int lStableCount = 0;
	size_t lLastCount = 0;
	for (int x = 0;; ++x)
	{
		Thread::Sleep(0.1);
		ScopeLock lGameLock(GetTickLock());
		const size_t lCount = GetContext()->GetObjectTable().size();
		if (lCount == lLastCount)
		{
			++lStableCount;
		}
		else
		{
			lStableCount = 0;
		}
		lLastCount = lCount;
		if (lCount == 0 || lStableCount >= 5)
		{
			v_set(GetVariableScope(), RTVAR_GAME_USERMESSAGE, _T(" "));
			if (lCount)
			{
				mLog.Warningf(_T("Unable to delete all objects, %i remaining."), lCount);
			}
			break;
		}
		if (x > 5)
		{
			v_set(GetVariableScope(), RTVAR_GAME_USERMESSAGE, _T("Cleaning up..."));
		}
	}
	GetContext()->SetPostKillTimeout(0.01);
}

void TrabantSimManager::PickObjects(const vec3& pPosition, const vec3& pDirection, const vec2& pRange, IntList& pPickedObjectIds, Vec3List& pPickedPositions)
{
	ScopeLock lPhysLock(GetMaster()->GetPhysicsLock());
	ScopeLock lGameLock(GetTickLock());
	vec3 lDirection = pDirection.GetNormalized();
	vec3 lPosition = pPosition + lDirection*pRange.x;
	int lHitObjects[16];
	vec3 lHitPositions[16];
	const int lHits = GetPhysicsManager()->QueryRayPick(lPosition, lDirection, pRange.y-pRange.x, lHitObjects, lHitPositions, LEPRA_ARRAY_COUNT(lHitObjects));
	std::copy(&lHitObjects[0], &lHitObjects[lHits], std::back_inserter(pPickedObjectIds));
	std::copy(&lHitPositions[0], &lHitPositions[lHits], std::back_inserter(pPickedPositions));
}

bool TrabantSimManager::IsLoaded(int pObjectId)
{
	ScopeLock lGameLock(GetTickLock());
	return !!GetContext()->GetObject(pObjectId);
}

void TrabantSimManager::Explode(const vec3& pPos, const vec3& pVel, float pStrength)
{
	ScopeLock lGameLock(GetTickLock());

	mCollisionSoundManager->OnCollision(pStrength, pPos, 0, _T("explosion"));

	const float lKeepOnGoingFactor = 1.0f;	// How much of the velocity energy, [0;1], should be transferred to the explosion particles.
	const int lParticles = std::max(10, Math::Lerp(8, 20, pStrength * 0.2f));
	const int lFires    = lParticles;
	const int lSmokes   = lParticles;
	const int lSparks   = lParticles/2;
	const int lShrapnel = lParticles/3;
	vec3 lStartFireColor(1.0f, 1.0f, 0.3f);
	vec3 lFireColor(0.6f, 0.4f, 0.2f);
	vec3 lStartSmokeColor(0.4f, 0.4f, 0.4f);
	vec3 lSmokeColor(0.2f, 0.2f, 0.2f);
	vec3 lShrapnelColor(0.3f, 0.3f, 0.3f);	// Default debris color is gray.
	UiTbc::ParticleRenderer* lParticleRenderer = (UiTbc::ParticleRenderer*)mUiManager->GetRenderer()->GetDynamicRenderer(_T("particle"));
	lParticleRenderer->CreateExplosion(pPos, pStrength, pVel*lKeepOnGoingFactor, 1, 1, lStartFireColor, lFireColor, lStartSmokeColor, lSmokeColor, lShrapnelColor, lFires, lSmokes, lSparks, lShrapnel);
}

void TrabantSimManager::PlaySound(const str& pSound, const vec3& pPos, const vec3& pVel, float pVolume)
{
	ScopeLock lGameLock(GetTickLock());
	UiCure::UserSound3dResource* lSoundResource = new UiCure::UserSound3dResource(mUiManager, UiLepra::SoundManager::LOOP_NONE);
	new UiCure::SoundReleaser(GetResourceManager(), mUiManager, GetContext(), pSound, lSoundResource, pPos, pVel, pVolume, 1);
}

void TrabantSimManager::PopCollisions(CollisionList& pCollisionList)
{
	ScopeLock lGameLock(GetTickLock());
	pCollisionList.splice(pCollisionList.end(), mCollisionList);
}

void TrabantSimManager::GetKeys(strutil::strvec& pKeys)
{
	ScopeLock lGameLock(GetTickLock());
	for (KeyMap::iterator x = mKeyMap.begin(); x != mKeyMap.end();)
	{
		pKeys.push_back(UiLepra::InputManager::GetKeyName(x->first));
		if (x->second)
		{
			KeyMap::iterator y = x;
			++y;
			mKeyMap.erase(x);
			x = y;
		}
		else
		{
			++x;
		}
	}
}

void TrabantSimManager::GetTouchDrags(DragList& pDragList)
{
	ScopeLock lGameLock(GetTickLock());
	pDragList = mDragList;

	for (DragEraseList::iterator x = mDragEraseList.begin(); x != mDragEraseList.end(); ++x)
	{
		for (DragList::iterator y = mDragList.begin(); y != mDragList.end();)
		{
			if (y->mStart.x == x->x && y->mStart.y == x->y)
			{
				y = mDragList.erase(y);
			}
			else
			{
				++y;
			}
		}
	}
	mDragEraseList.clear();
}

vec3 TrabantSimManager::GetAccelerometer() const
{
	return mUiManager->GetAccelerometer();
}

vec3 TrabantSimManager::GetMouseMove()
{
#ifdef LEPRA_TOUCH
	return vec3();
#else // Computer
	if (!mIsMouseControlled)
	{
		mIsMouseControlled = true;
		mSetCursorInvisible = true;
	}
	vec3 m(mMouseMove);
	mMouseMove.Set(0,0,0);
	if (((TrabantSimConsoleManager*)GetConsoleManager())->GetUiConsole()->IsVisible())
	{
		m.Set(0,0,0);
	}
	return m;
#endif // Touch device / computer
}

int TrabantSimManager::CreateJoystick(float x, float y, bool pIsSloppy)
{
	ScopeLock lGameLock(GetTickLock());
	const float lTouchSideScale = 1.28f;	// Inches.
	const float lTouchScale = lTouchSideScale / (float)mUiManager->GetDisplayManager()->GetPhysicalScreenSize();
	const int lScreenPixelWidth = mUiManager->GetDisplayManager()->GetWidth();
	const int lMinimumTouchRadius = (int)(lScreenPixelWidth*lTouchScale*0.17f);	// 30 pixels in iPhone classic.
	PixelRect lRect(0,0,10,10);
	Touchstick::InputMode lMode = pIsSloppy? Touchstick::MODE_RELATIVE_CENTER_NOSPRING : Touchstick::MODE_RELATIVE_CENTER;
	Touchstick* lStick = new Touchstick(mUiManager->GetInputManager(), lMode, lRect, 0, lMinimumTouchRadius);
	const str lName = strutil::Format(_T("Touchstick%i"), mTouchstickList.size());
	lStick->SetUniqueIdentifier(lName);
	mTouchstickList.push_back(TouchstickInfo(lStick, x, y, -90, pIsSloppy));
	mTouchstickTimer.ReduceTimeDiff(-10);
	return (int)mTouchstickList.size()-1;
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
		lIsAttachment = true;
	}
	else if (pEngineType == _T("roll"))
	{
		lEngineType = Tbc::PhysicsEngine::ENGINE_HINGE_ROLL;
		lIsAttachment = true;
	}
	else if (pEngineType == _T("walk_abs"))
	{
		lEngineType = Tbc::PhysicsEngine::ENGINE_WALK;
	}
	else if (pEngineType == _T("push_abs"))
	{
		lEngineType = Tbc::PhysicsEngine::ENGINE_PUSH_ABSOLUTE;
	}
	else if (pEngineType == _T("push_rel"))
	{
		lEngineType = Tbc::PhysicsEngine::ENGINE_PUSH_RELATIVE;
	}
	else if (pEngineType == _T("push_turn_abs"))
	{
		lEngineType = Tbc::PhysicsEngine::ENGINE_PUSH_TURN_ABSOLUTE;
	}
	else if (pEngineType == _T("push_turn_rel"))
	{
		lEngineType = Tbc::PhysicsEngine::ENGINE_PUSH_TURN_RELATIVE;
	}
	else if (pEngineType == _T("gyro"))
	{
		lEngineType = Tbc::PhysicsEngine::ENGINE_HINGE_GYRO;
		lIsAttachment = true;
	}
	else if (pEngineType == _T("rotor"))
	{
		lEngineType = Tbc::PhysicsEngine::ENGINE_ROTOR;
		lIsAttachment = true;
	}
	else if (pEngineType == _T("tilt"))
	{
		lEngineType = Tbc::PhysicsEngine::ENGINE_ROTOR_TILT;
		lIsAttachment = true;
	}
	else if (pEngineType == _T("slider"))
	{
		lEngineType = Tbc::PhysicsEngine::ENGINE_SLIDER_FORCE;
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
	for (EngineTargetList::iterator x = lTargets.begin(); x != lTargets.end(); ++x)
	{
		Cure::ContextObject* lEngineObject = GetContext()->GetObject(x->mInstanceId);
		Tbc::ChunkyBoneGeometry* lGeometry = lEngineObject->GetPhysics()->GetBoneGeometry(0);
		lEngine->AddControlledGeometry(lGeometry, x->mStrength);
	}
	lObject->GetPhysics()->AddEngine(lEngine);
	if (lIsAttachment)
	{
		for (EngineTargetList::iterator x = lTargets.begin(); x != lTargets.end(); ++x)
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
		if (lLoStop == 0 && lHiStop == 0)
		{
			lLoStop = -1.0f;
			lHiStop = +1.0f;
		}
	}
	else if (pJointType == _T("universal"))
	{
		lType = Tbc::ChunkyBoneGeometry::CONNECTOR_UNIVERSAL;
	}
	else if (pJointType == _T("fixed"))
	{
		lType = Tbc::ChunkyBoneGeometry::CONNECTOR_FIXED;
		GetPhysicsManager()->MakeStatic(lObject2->GetPhysics()->GetBoneGeometry(0)->GetBodyId());
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
	const float lTotalMass = lObject->GetMass() + lObject2->GetMass();
	lObject->SetMass(lTotalMass);	// Avoid collision sounds.
	lObject2->SetMass(lTotalMass);
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
		if (lObject->GetPhysics()->GetPhysicsType() == Tbc::ChunkyPhysics::STATIC)
		{
			lObject->UiMove();
		}
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
		if (pOrientation.GetNorm() < 0.5)
		{
			return;
		}
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

void TrabantSimManager::ObjectColor(int pObjectId, bool pSet, vec3& pColor, float pAlpha)
{
	ScopeLock lGameLock(GetTickLock());
	Object* lObject = (Object*)GetContext()->GetObject(pObjectId);
	if (!lObject)
	{
		return;
	}
	if (pSet)
	{
		lObject->GetMesh(0)->GetBasicMaterialSettings().mDiffuse = pColor;
		lObject->GetMesh(0)->GetBasicMaterialSettings().mAlpha = pAlpha;
		lObject->GetMesh(0)->SetAlwaysVisible(!!pAlpha);
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
	if (pEngineIndex < 0 || pEngineIndex >= lObject->GetPhysics()->GetEngineCount())
	{
		mLog.Warningf(_T("Object %i does not have an engine with index %i."), pObjectId, pEngineIndex);
		return;
	}
	if (pSet)
	{
		switch (lObject->GetPhysics()->GetEngine(pEngineIndex)->GetEngineType())
		{
			case Tbc::PhysicsEngine::ENGINE_WALK:
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



void TrabantSimManager::CommandLoop()
{
	mIsControlled = false;
	mIsControlTimeout = false;
	{
		ScopeLock lPhysLock(GetMaster()->GetPhysicsLock());
		ScopeLock lGameLock(GetTickLock());
		GetPhysicsManager()->InitCurrentThread();
	}
	uint8 lData[128*1024];
	while (!mCommandThread->GetStopRequest())
	{
		const int l = mCommandSocket->ReceiveFrom(lData, sizeof(lData), mLastRemoteAddress, 2);
		if (l <= 0 || lData[l-1] != '\n' || ::memcmp("disconnect\n", lData, 11) == 0)
		{
			if (l == 0)
			{
				mIsControlTimeout = true;
			}
			else
			{
				mIsControlled = false;
			}
			for (int x = 0; x < 10; ++x)
			{
				if (mCommandSocket->ReceiveFrom(lData, sizeof(lData), mLastRemoteAddress, 0.01) == 0)
				{
					break;
				}
			}
			continue;
		}
		mIsControlled = true;
		mIsControlTimeout = false;
		while (mIsPaused)
		{
			Thread::Sleep(0.5);
			if (mCommandThread->GetStopRequest())
			{
				break;
			}
		}
		if (mCommandThread->GetStopRequest())
		{
			break;
		}
		lData[l-1] = 0;	// Drop last linefeed.
		const str lCommand = strutil::Encode(astr((char*)lData));
		if (!GetConsoleManager())
		{
			break;
		}
		GetConsoleManager()->ExecuteCommand(lCommand);
		const astr lResponse = astrutil::Encode(((TrabantSimConsoleManager*)GetConsoleManager())->GetActiveResponse());
		if (mCommandSocket->SendTo((const uint8*)lResponse.c_str(), (int)lResponse.length(), mLastRemoteAddress) != (int)lResponse.length())
		{
			mIsControlled = false;
		}
	}
	mLog.Info(_T("Terminating command thread."));
	mIsControlled = false;
	deb_assert(mCommandThread);
	deb_assert(mCommandSocket);
}

bool TrabantSimManager::IsControlled()
{
	if (!mCommandThread || !mCommandThread->IsRunning())
	{
		return false;
	}

	bool lAllowPowerDown;
	v_get(lAllowPowerDown, =, GetVariableScope(), RTVAR_GAME_ALLOWPOWERDOWN, true);
	const bool lIsPowerDown = (lAllowPowerDown && mIsControlTimeout);
	const bool lIsControlled = (!lIsPowerDown && mIsControlled);
	if (lIsControlled != mWasControlled)
	{
		if (lIsControlled)
		{
			v_set(GetVariableScope(), RTVAR_GAME_USERMESSAGE, _T(" "));
		}
		else
		{
			v_set(GetVariableScope(), RTVAR_GAME_USERMESSAGE, _T("Controlling application went silent."));
		}
		mWasControlled = lIsControlled;
	}
	if (!lIsControlled)
	{
		mSetCursorVisible = true;
		mIsMouseControlled = false;
	}
	return lIsControlled;
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
#ifndef LEPRA_TOUCH
		mPauseButton = new UiTbc::Button(GREEN_BUTTON, _T("Pause"));
		mPauseButton->SetOnClick(TrabantSimManager, OnPauseButton);
		UiTbc::Button* lButton = mPauseButton;
#else
		mBackButton = new UiTbc::Button(GREEN_BUTTON, _T("Back"));
		mBackButton->SetOnClick(TrabantSimManager, OnBackButton);
		UiTbc::Button* lButton = mBackButton;
#endif
		int x = mRenderArea.mLeft + 2;
		int y = mRenderArea.mTop + 2;
		mUiManager->GetDesktopWindow()->AddChild(lButton, x, y);
		double lFontHeight;
		v_get(lFontHeight, =, UiCure::GetSettings(), RTVAR_UI_2D_FONTHEIGHT, 30.0);
		lButton->SetPreferredSize(lFontHeight*7/3,lFontHeight);
		lButton->SetRoundedRadius(4);
		lButton->SetVisible(true);
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
	delete mBackButton;
	mBackButton = 0;
	delete mMenu;
	mMenu = 0;
	if (mLight)
	{
		delete mLight;
		mLight = 0;
	}
	Parent::Close();
}

void TrabantSimManager::CloseConnection()
{
	if (mCommandSocket)
	{
		mCommandSocket->Shutdown(SocketBase::SHUTDOWN_RECV);
		mCommandSocket->SendTo((uint8*)"disconnect\n", 11, mLastRemoteAddress);
		mCommandSocket->Shutdown(SocketBase::SHUTDOWN_BOTH);
		if (mCommandThread) mCommandThread->RequestStop();
		UdpSocket(SocketAddress(), false).SendTo((uint8*)"?", 1, mCommandSocket->GetLocalAddress());
		delete mCommandThread;
		mCommandThread = 0;
		Thread::Sleep(0.1);	// Wait for shutdown.
		delete mCommandSocket;
		mCommandSocket = 0;
	}
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
	if (mSetFocus)
	{
		mSetFocus = false;
		mUiManager->GetDisplayManager()->SetFocus(true);
	}
	if (mSetCursorVisible)
	{
		mSetCursorVisible = false;
		mUiManager->GetInputManager()->SetCursorVisible(true);
		if (mPauseButton)
		{
			mPauseButton->SetVisible(true);
		}
	}
	if (mSetCursorInvisible)
	{
		mSetCursorInvisible = false;
		mUiManager->GetInputManager()->SetCursorVisible(false);
		if (mPauseButton)
		{
			mPauseButton->SetVisible(false);
		}
	}
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
}

void TrabantSimManager::TickUiUpdate()
{
	((TrabantSimConsoleManager*)GetConsoleManager())->GetUiConsole()->Tick();
	mCollisionSoundManager->Tick(mCameraTransform.GetPosition());
}

void TrabantSimManager::SetLocalRender(bool pRender)
{
	(void)pRender;
}



void TrabantSimManager::AddCheckerTexturing(UiTbc::TriangleBasedGeometry* pMesh, float pScale)
{
	const float lScale = 1/pScale;
	const float lOff = lScale*0.5f;
	const float* lVertices = pMesh->GetVertexData();
	const vtx_idx_t* lTriangles = pMesh->GetIndexData();
	const unsigned tc = pMesh->GetTriangleCount();
	std::vector<float> lUVs;
	lUVs.resize(pMesh->GetVertexCount()*2);
	for (unsigned t = 0; t < tc; ++t)
	{
		int v0,v1,v2;
		v0 = lTriangles[t*3+0];
		v1 = lTriangles[t*3+1];
		v2 = lTriangles[t*3+2];
		vec3 u0(lVertices[v0*3+0], lVertices[v0*3+1], lVertices[v0*3+2]);
		vec3 u1(lVertices[v1*3+0], lVertices[v1*3+1], lVertices[v1*3+2]);
		vec3 u2(lVertices[v2*3+0], lVertices[v2*3+1], lVertices[v2*3+2]);
		vec3 n = (u1-u0).Cross(u2-u0);
		n.x = fabs(n.x); n.y = fabs(n.y); n.z = fabs(n.z);
		u0 *= lScale; u1 *= lScale; u2 *= lScale;
		if (n.x > n.y && n.x > n.z)
		{
			lUVs[v0*2+0] = u0.y+lOff;
			lUVs[v0*2+1] = u0.z+lOff;
			lUVs[v1*2+0] = u1.y+lOff;
			lUVs[v1*2+1] = u1.z+lOff;
			lUVs[v2*2+0] = u2.y+lOff;
			lUVs[v2*2+1] = u2.z+lOff;
		}
		else if (n.y > n.z)
		{
			lUVs[v0*2+0] = u0.x+lOff;
			lUVs[v0*2+1] = u0.z+lOff;
			lUVs[v1*2+0] = u1.x+lOff;
			lUVs[v1*2+1] = u1.z+lOff;
			lUVs[v2*2+0] = u2.x+lOff;
			lUVs[v2*2+1] = u2.z+lOff;
		}
		else
		{
			lUVs[v0*2+0] = u0.x+lOff;
			lUVs[v0*2+1] = u0.y+lOff;
			lUVs[v1*2+0] = u1.x+lOff;
			lUVs[v1*2+1] = u1.y+lOff;
			lUVs[v2*2+0] = u2.x+lOff;
			lUVs[v2*2+1] = u2.y+lOff;
		}
	}
	pMesh->AddUVSet(lUVs.data());
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

	if (pObject1->GetPhysics()->GetPhysicsType() != Tbc::ChunkyPhysics::DYNAMIC)
	{
		return;
	}

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
	mIsPaused = true;

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

void TrabantSimManager::OnBackButton(UiTbc::Button* pButton)
{
	if (pButton)
	{
		mMenu->OnTapSound(pButton);
	}
	FoldSimulator();
}

void TrabantSimManager::OnMenuAlternative(UiTbc::Button*)
{
	mPauseButton->SetVisible(true);
	mIsPaused = false;
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
		UiTbc::ParticleRenderer* lParticleRenderer = (UiTbc::ParticleRenderer*)mUiManager->GetRenderer()->GetDynamicRenderer(_T("particle"));
		lParticleRenderer->SetGravity(vec3(gx,gy,gz));
		MoveCamera(lPhysicsTime);
		mLight->Tick(mCameraTransform.mOrientation);
		UpdateCameraPosition(true);

		UpdateUserMessage();
	}

	v_set(GetVariableScope(), RTVAR_UI_SOUND_MASTERVOLUME, IsControlled()? 1.0 : 0.0);

	{
		ScopeLock lGameLock(GetTickLock());
		const UiDragList lDrags = mUiManager->GetDragManager()->GetDragList();
		for (UiDragList::const_iterator x = lDrags.begin(); x != lDrags.end(); ++x)
		{
			bool lFound = false;
			for (DragList::iterator y = mDragList.begin(); y != mDragList.end(); ++y)
			{
				if (y->mStart.x == x->mStart.x && y->mStart.y == x->mStart.y)
				{
					lFound = true;
					const float tf = 1 / (float)y->mTimer.PopTimeDiff();
					if (tf > 1e-3)
					{
						y->mVelocity = Math::Lerp(y->mVelocity, vec2((x->mLast.x - y->mLast.x)*tf, (x->mLast.y - y->mLast.y)*tf), 0.5f);
					}
					y->mLast = x->mLast;
					y->mIsPress = x->mIsPress;
				}
			}
			if (!lFound)
			{
				Drag d;
				d.mStart = x->mStart;
				d.mLast = x->mLast;
				d.mIsPress = x->mIsPress;
				d.mButtonMask = x->mButtonMask;
				mDragList.push_back(d);
			}
			if (!x->mIsPress)
			{
				mDragEraseList.push_back(x->mStart);
			}
		}
	}

	Parent::ScriptPhysicsTick();
	GetResourceManager()->ForceFreeCache();
}

void TrabantSimManager::MoveCamera(float pFrameTime)
{
	int ctgt = 0;
	float clx,cly,clz,cdist,cax,cay,caz,crx,cry,crz,smooth;
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
	v_get(smooth, =(float), GetVariableScope(), RTVAR_UI_3D_CAMSMOOTH, 0.0);
	vec3 lLookAt(clx,cly,clz);
	Object* lObject = 0;
	if (ctgt)
	{
		lObject = (Object*)GetContext()->GetObject(ctgt);
		if (lObject)
		{
			lLookAt = lObject->GetPosition() + lObject->GetOrientation()*lLookAt;
			mCameraVelocity = lObject->GetVelocity();
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
	mCameraTransform.mPosition = t.mPosition;
	mCameraTransform.mOrientation.Slerp(t.mOrientation, mCameraTransform.mOrientation, smooth);
}

void TrabantSimManager::UpdateCameraPosition(bool pUpdateMicPosition)
{
	mUiManager->SetCameraPosition(mCameraTransform);
	if (pUpdateMicPosition)
	{
		mUiManager->SetMicrophonePosition(mCameraTransform, mCameraVelocity);
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



bool TrabantSimManager::OnKeyDown(UiLepra::InputManager::KeyCode pKeyCode)
{
	if (!Parent::OnKeyDown(pKeyCode))
	{
		ScopeLock lGameLock(GetTickLock());
		mKeyMap.insert(KeyMap::value_type(pKeyCode,false));
		return false;
	}
	return true;
}

bool TrabantSimManager::OnKeyUp(UiLepra::InputManager::KeyCode pKeyCode)
{
	if (pKeyCode == UiLepra::InputManager::IN_KBD_ESC && mIsMouseControlled)
	{
		if (mPauseButton)
		{
			if (mPauseButton->IsVisible())
			{
				mSetCursorInvisible = true;
			}
			else
			{
				mSetCursorVisible = true;
			}
		}
	}

	{
		ScopeLock lGameLock(GetTickLock());
		mKeyMap[pKeyCode] = true;
	}
	return Parent::OnKeyUp(pKeyCode);
}

void TrabantSimManager::OnInput(UiLepra::InputElement* pElement)
{
	if (pElement->GetInterpretation() == UiLepra::InputElement::RELATIVE_AXIS)
	{
		UiLepra::InputDevice* lMouse = mUiManager->GetInputManager()->GetMouse();
		if (pElement == lMouse->GetAxis(0))
		{
			mMouseMove.x += pElement->GetValue();
		}
		else if (pElement == lMouse->GetAxis(1))
		{
			mMouseMove.y += pElement->GetValue();
		}
		else
		{
			mMouseMove.z += pElement->GetValue();
		}
	}
	return Parent::OnInput(pElement);
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
