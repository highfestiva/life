
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "Tv3dManager.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/TimeManager.h"
#include "../Lepra/Include/CyclicArray.h"
#include "../Lepra/Include/Socket.h"
#include "../Lepra/Include/SystemManager.h"
#include "../Lepra/Include/TimeLogger.h"
#include "../Lepra/Include/Unordered.h"
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
#include "../Lepra/Include/Random.h"
#include "../Life/LifeClient/UiConsole.h"
#include "Object.h"
#include "Tv3d.h"
#include "Tv3dConsoleManager.h"
#include "RtVar.h"
#include "Light.h"
#include "Version.h"

#define BG_COLOR		Color(25, 35, 45, 190)
#define BRIGHT_TEXT		Color(220, 215, 205)
#define GREEN_BUTTON		Color(20, 190, 15)


namespace Tv3d
{



Tv3dManager::Tv3dManager(Life::GameClientMasterTicker* pMaster, const Cure::TimeManager* pTime,
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
	mAcceptThread(new MemberThread<Tv3dManager>("ConnectionListener")),
	mCommandThread(0)
{
	mCollisionSoundManager = new UiCure::CollisionSoundManager(this, pUiManager);

	SetConsoleManager(new Tv3dConsoleManager(GetResourceManager(), this, mUiManager, GetVariableScope(), mRenderArea));

	GetPhysicsManager()->SetSimulationParameters(0.0f, 0.03f, 0.2f);

	SocketAddress lAddress;
	const strutil::strvec& args = SystemManager::GetArgumentVector();
	if (!lAddress.Resolve(args[args.size()-1]))
	{
		lAddress.Resolve(_T("localhost:2541"));
	}
	mListenerSocket = new TcpListenerSocket(lAddress, true);
	mAcceptThread->Start(this, &Tv3dManager::AcceptLoop);
	mLog.Headlinef(_T("Command server listening on %s."), lAddress.GetAsString().c_str());
}

Tv3dManager::~Tv3dManager()
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



void Tv3dManager::DeleteObjects()
{
	std::vector<Cure::GameObjectId>::iterator x;
	for (x = mObjects.begin(); x != mObjects.end(); ++x)
	{
		GetContext()->PostKillObject(*x);
	}
	mObjects.clear();
}

int Tv3dManager::CreateObject(const MeshObject& pGfxObject, const PhysObjectArray& pPhysObjects)
{
	(void)pPhysObjects;

	Object* lObject = (Object*)Parent::CreateContextObject(_T("object"), Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED, 0);
	lObject->SetPhysicsTypeOverride(Cure::PHYSICS_OVERRIDE_BONES);
	Tbc::ChunkyPhysics* lPhysics = new Tbc::ChunkyPhysics;
	lPhysics->SetBoneCount(1);
	Tbc::ChunkyBoneGeometry::BodyData lBoneData(0,0,0);
	Tbc::ChunkyBoneGeometry* lBone = new Tbc::ChunkyBoneSphere(lBoneData);
	lPhysics->AddBoneGeometry(xform(), lBone);
	lObject->CreatePhysics(lPhysics);
	float r,g,b;
	v_get(r, =(float), GetVariableScope(), RTVAR_UI_PENRED, 0.5);
	v_get(g, =(float), GetVariableScope(), RTVAR_UI_PENGREEN, 0.5);
	v_get(b, =(float), GetVariableScope(), RTVAR_UI_PENBLUE, 0.5);
	lObject->AddGfxMesh(pGfxObject.mVertices, pGfxObject.mIndices, vec3(r,g,b));
	mObjects.push_back(lObject->GetInstanceId());
	return 0;
}



void Tv3dManager::AcceptLoop()
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
			mCommandThread = new MemberThread<Tv3dManager>("CommandRecvThread");
			mCommandThread->Start(this, &Tv3dManager::CommandLoop);
		}
	}
}

void Tv3dManager::CommandLoop()
{
	char lData[128*1024];
	while (!mCommandThread->GetStopRequest())
	{
		int l = mConnectSocket->ReceiveDatagram(lData, sizeof(lData));
		if (l <= 0 || lData[l-1] != '\n')
		{
			break;
		}
		while (!mPauseButton->IsVisible())
		{
			Thread::Sleep(0.5);
			if (mCommandThread->GetStopRequest())
			{
				return;
			}
		}
		lData[l-1] = 0;	// Drop last linefeed.
		const str lCommand = astrutil::Encode(astr(lData));
		const int lResult = GetConsoleManager()->ExecuteCommand(lCommand);
		const astr lInfo = (lResult==0)? "ok\n" : "error\n";
		if (mConnectSocket->Send(lInfo.c_str(), lInfo.length()) != (int)lInfo.length())
		{
			break;
		}
	}
}



void Tv3dManager::SaveSettings()
{
	GetConsoleManager()->ExecuteCommand(_T("save-application-config-file ")+GetApplicationCommandFilename());
}

bool Tv3dManager::Open()
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
		mPauseButton->SetOnClick(Tv3dManager, OnPauseButton);
	}
	if (lOk)
	{
		mMenu = new Life::Menu(mUiManager, GetResourceManager());
	}
	return lOk;
}

void Tv3dManager::Close()
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

void Tv3dManager::SetIsQuitting()
{
	((Tv3dConsoleManager*)GetConsoleManager())->GetUiConsole()->SetVisible(false);
	Parent::SetIsQuitting();
}

void Tv3dManager::SetFade(float pFadeAmount)
{
	(void)pFadeAmount;
}



Cure::RuntimeVariableScope* Tv3dManager::GetVariableScope() const
{
	return (Parent::GetVariableScope());
}



bool Tv3dManager::InitializeUniverse()
{
	mLight = new Light(mUiManager);
	return true;
}

void Tv3dManager::TickInput()
{
	TickNetworkInput();
	TickUiInput();
}

void Tv3dManager::TickUiInput()
{
	mUiManager->GetInputManager()->SetCursorVisible(true);
}

void Tv3dManager::TickUiUpdate()
{
	((Tv3dConsoleManager*)GetConsoleManager())->GetUiConsole()->Tick();
}

void Tv3dManager::SetLocalRender(bool pRender)
{
	(void)pRender;
}



void Tv3dManager::CreateObject(int pIndex, const vec3* pPosition)
{
	vec3 lPosition;
	if (pPosition)
	{
		lPosition = *pPosition;
	}
	else
	{
		lPosition.x = pIndex%3*0.5f-0.5f;
		lPosition.y = pIndex/3%3*0.5f-0.5f;
		lPosition.z = -pIndex/9%3*0.5f-0.5f;
	}
	if ((int)mObjects.size() > pIndex)
	{
		Cure::ContextObject* lObject = GetContext()->GetObject(mObjects[pIndex]);
		if (lObject)
		{
			GetPhysicsManager()->SetBodyPosition(lObject->GetPhysics()->GetBoneGeometry(0)->GetBodyId(), lPosition);
		}
		return;
	}
	Cure::ContextObject* lObject = Parent::CreateContextObject(_T("soccerball"), Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED, 0);
	lObject->SetRootPosition(lPosition);
	lObject->SetRootVelocity(RNDVEC(1.0f));
	lObject->StartLoading();
	mObjects.push_back(lObject->GetInstanceId());
}

Cure::ContextObject* Tv3dManager::CreateContextObject(const str& pClassId) const
{
	UiCure::Machine* lObject = new Object(GetResourceManager(), pClassId, mUiManager);
	lObject->SetAllowNetworkLogic(true);
	return lObject;
}

void Tv3dManager::OnLoadCompleted(Cure::ContextObject* pObject, bool pOk)
{
	if (!pOk)
	{
		mLog.Errorf(_T("Could not load object of type %s."), pObject->GetClassId().c_str());
		GetContext()->PostKillObject(pObject->GetInstanceId());
	}
}

void Tv3dManager::OnCollision(const vec3& pForce, const vec3& pTorque, const vec3& pPosition,
	Cure::ContextObject* pObject1, Cure::ContextObject* pObject2,
	Tbc::PhysicsManager::BodyID pBody1Id, Tbc::PhysicsManager::BodyID pBody2Id)
{
	(void)pBody2Id;
	mCollisionSoundManager->OnCollision(pForce, pTorque, pPosition, pObject1, pObject2, pBody1Id, 5000, false);
}



void Tv3dManager::OnPauseButton(UiTbc::Button* pButton)
{
	if (pButton)
	{
		mMenu->OnTapSound(pButton);
	}
	mPauseButton->SetVisible(false);

	UiTbc::Dialog* d = mMenu->CreateTbcDialog(Life::Menu::ButtonAction(this, &Tv3dManager::OnMenuAlternative), 0.5f, 0.3f);
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

void Tv3dManager::OnMenuAlternative(UiTbc::Button*)
{
	mPauseButton->SetVisible(true);
	HiResTimer::StepCounterShadow();
	v_set(GetVariableScope(), RTVAR_PHYSICS_HALT, false);
}



void Tv3dManager::ScriptPhysicsTick()
{
	// Camera moves in a "moving average" kinda curve (halfs the distance in x seconds).
	const float lPhysicsTime = GetTimeManager()->GetAffordedPhysicsTotalTime();
	if (lPhysicsTime > 1e-5)
	{
		MoveCamera(lPhysicsTime);
		mLight->Tick(mCameraTransform.mOrientation);
		UpdateCameraPosition(false);
	}

	Parent::ScriptPhysicsTick();
	GetResourceManager()->ForceFreeCache();
}

void Tv3dManager::MoveCamera(float pFrameTime)
{
	float clx,cly,clz,cdist,cax,cay,caz,crx,cry,crz;
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
	xform t(quat(), lLookAt+vec3(0,-cdist,0));
	t.RotateAroundAnchor(lLookAt, vec3(0,1,0), cay);
	t.RotateAroundAnchor(lLookAt, vec3(1,0,0), cax);
	t.RotateAroundAnchor(lLookAt, vec3(0,0,1), caz);
	t.RotateAroundAnchor(lLookAt, vec3(0,1,0), mCameraAngle.y);
	t.RotateAroundAnchor(lLookAt, vec3(1,0,0), mCameraAngle.x);
	t.RotateAroundAnchor(lLookAt, vec3(0,0,1), mCameraAngle.z);
	mCameraAngle.x = fmod(mCameraAngle.x+crx*pFrameTime*2*PIF,2*PIF);
	mCameraAngle.y = fmod(mCameraAngle.y+cry*pFrameTime*2*PIF,2*PIF);
	mCameraAngle.z = fmod(mCameraAngle.z+crz*pFrameTime*2*PIF,2*PIF);
	mCameraTransform = t;
}

void Tv3dManager::UpdateCameraPosition(bool pUpdateMicPosition)
{
	mUiManager->SetCameraPosition(mCameraTransform);
	if (pUpdateMicPosition)
	{
		mUiManager->SetMicrophonePosition(mCameraTransform, vec3());
	}
}



void Tv3dManager::PrintText(const str& s, int x, int y) const
{
	Color lOldColor = mUiManager->GetPainter()->GetColor(0);
	mUiManager->GetPainter()->SetColor(DARK_BLUE, 0);
	mUiManager->GetPainter()->PrintText(s, x, y+1);
	mUiManager->GetPainter()->SetColor(lOldColor, 0);
	mUiManager->GetPainter()->PrintText(s, x, y);
}

void Tv3dManager::DrawImage(UiTbc::Painter::ImageID pImageId, float cx, float cy, float w, float h, float pAngle) const
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



void Tv3dManager::PainterImageLoadCallback(UiCure::UserPainterKeepImageResource* pResource)
{
	if (pResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		mUiManager->GetDesktopWindow()->GetImageManager()->AddLoadedImage(*pResource->GetRamData(), pResource->GetData(),
			UiTbc::GUIImageManager::CENTERED, UiTbc::GUIImageManager::ALPHABLEND, 255);
	}
}



loginstance(GAME, Tv3dManager);



}
