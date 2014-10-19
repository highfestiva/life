
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include <algorithm>
#include "ImpuzzableManager.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/TimeManager.h"
#include "../Lepra/Include/CyclicArray.h"
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
#include "Piece.h"
#include "Impuzzable.h"
#include "ImpuzzableConsoleManager.h"
#include "Level.h"
#include "RtVar.h"
#include "Sunlight.h"
#include "Version.h"

#define BG_COLOR		Color(5, 10, 15, 230)
#define CAM_DISTANCE		7.0f
#define PIECE_RADIUS		0.15f
#define DRAG_FLAG_INVALID	1
#define DRAG_FLAG_STARTED	2
#define CLOSE_NORMAL		5e-5
#define SAME_NORMAL		(1-CLOSE_NORMAL)
#define NGON_INDEX(i)		(((int)i<0)? cnt-1 : (i>=(int)cnt)? 0 : i)
#define LEVEL_DONE()		(mPercentDone >= 85)
#define BRIGHT_TEXT		Color(220, 215, 205)
#define DIM_TEXT		Color(200, 190, 180)
#define DIM_RED_TEXT		Color(240, 80, 80)
#define DIM_RED			Color(180, 60, 50)
#define GREEN_BUTTON		Color(20, 190, 15)
#define ORANGE_BUTTON		Color(220, 110, 20)
#define RED_BUTTON		Color(210, 40, 30)
#define BLACK_BUTTON		DARK_GRAY


namespace Impuzzable
{



struct PieceDistanceAscending
{
	vec3 mCamPos;
	PieceDistanceAscending(vec3 pCamPos): mCamPos(pCamPos) {}
	bool operator() (const Piece* a, const Piece* b) { return mCamPos.GetDistance(a->GetPosition()) < mCamPos.GetDistance(b->GetPosition()); }
};

void Impuzzable__ShowAd();
void Impuzzable__Buy();



ImpuzzableManager::ImpuzzableManager(Life::GameClientMasterTicker* pMaster, const Cure::TimeManager* pTime,
	Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager,
	UiCure::GameUiManager* pUiManager, int pSlaveIndex, const PixelRect& pRenderArea):
	Parent(pMaster, pTime, pVariableScope, pResourceManager, pUiManager, pSlaveIndex, pRenderArea),
	mCollisionSoundManager(0),
	mLevel(0),
	mForceCutWindow(false),
	mMenu(0),
	mSunlight(0),
	mCameraAngle(0),
	mCameraRotateSpeed(1.0f),
	mCameraTransform(quat(), vec3(0, -CAM_DISTANCE, 0)),
	mLevelCompleted(false),
	mPauseButton(0),
	mIsCutting(false),
	mIsShaking(false),
	mCutsLeft(1),
	mShakesLeft(1),
	mLastCutMode(CUT_NORMAL),
	mCutSoundPitch(1),
	mQuickCutCount(0),
	mLevelScore(0),
	mShakeSound(0)
{
	mCollisionSoundManager = new UiCure::CollisionSoundManager(this, pUiManager);
	mCollisionSoundManager->AddSound(_T("explosion"),	UiCure::CollisionSoundManager::SoundResourceInfo(0.8f, 0.4f, 0));
	mCollisionSoundManager->AddSound(_T("rubber"),		UiCure::CollisionSoundManager::SoundResourceInfo(1.0f, 0.5f, 0));
	mCollisionSoundManager->PreLoadSound(_T("explosion"));

	SetConsoleManager(new ImpuzzableConsoleManager(GetResourceManager(), this, mUiManager, GetVariableScope(), mRenderArea));

	GetPhysicsManager()->SetSimulationParameters(0, 0.01f, 0.2f);

	v_set(GetVariableScope(), RTVAR_GAME_FIRSTTIME, true);
	v_set(GetVariableScope(), RTVAR_GAME_LEVEL, 0);
	v_set(GetVariableScope(), RTVAR_GAME_LEVELSHAPEALTERNATE, false);
	v_set(GetVariableScope(), RTVAR_GAME_RUNADS, true);
	v_set(GetVariableScope(), RTVAR_GAME_SCORE, 0.0);
	v_set(GetVariableScope(), RTVAR_UI_SOUND_MASTERVOLUME, 1.0);
	/*
#define RNDMZEL \
	for (int x = 0; x < 20; ++x) \
	{ \
		size_t a = Random::GetRandomNumber() % lNGon.size(); \
		size_t b = Random::GetRandomNumber() % lNGon.size(); \
		if (a != b) \
			std::swap(lNGon[1], lNGon[b]); \
	}
	std::vector<vec3> lNGon;
#define CLRL \
	lNGon.clear(); \
	lNGon.push_back(vec3(-2.1314f, 0, -0.333f));	\
	for (int x = 0; x < 10; ++x)	\
		lNGon.push_back(lNGon[lNGon.size()-1] + vec3(1,0,0.14f)*Random::Uniform(0.001f, 0.1f));	\
	for (int x = 0; x < 10; ++x)	\
		lNGon.push_back(lNGon[lNGon.size()-1] + vec3(0.14f,0,-1)*Random::Uniform(0.001f, 0.1f));	\
	for (int x = 0; x < 10; ++x)	\
		lNGon.push_back(lNGon[lNGon.size()-1] + vec3(-0.34f,0,-1)*Random::Uniform(0.001f, 0.1f));	\
	for (int x = 0; x < 10; ++x)	\
		lNGon.push_back(lNGon[lNGon.size()-1] + vec3(-1,0,-0.14f)*Random::Uniform(0.001f, 0.1f));
	for (int y = 0; y < 100; ++y)
	{
		CLRL;
		RNDMZEL;
		std::vector<vec3> lCopy(lNGon);
		CreateNGon(lNGon);
		mLog.Infof(_T("----------"));
		for (int x = 0; x < (int)lNGon.size(); ++x)
		{
			mLog.Infof(_T("%f;%f;"), lNGon[x].x, lNGon[x].z);
		}
		deb_assert(lNGon.size() == 5);
	}*/
}

ImpuzzableManager::~ImpuzzableManager()
{
	Close();

	delete mCollisionSoundManager;
	mCollisionSoundManager = 0;
}

void ImpuzzableManager::Suspend()
{
	if (!mMenu->GetDialog())
	{
		mPauseButton->SetVisible(false);
		OnPauseButton(0);
	}
}

void ImpuzzableManager::LoadSettings()
{
	v_set(GetVariableScope(), RTVAR_GAME_SPAWNPART, 1.0);

	Parent::LoadSettings();

	v_set(GetVariableScope(), RTVAR_UI_2D_FONT, _T("Verdana"));
	v_set(GetVariableScope(), RTVAR_UI_2D_FONTFLAGS, 0);
	v_set(GetVariableScope(), RTVAR_UI_3D_FOV, 20.0);
	v_set(GetVariableScope(), RTVAR_PHYSICS_MICROSTEPS, 3);
	v_set(GetVariableScope(), RTVAR_PHYSICS_NOCLIP, false);

	GetConsoleManager()->ExecuteCommand(_T("bind-key F2 prev-level"));
	GetConsoleManager()->ExecuteCommand(_T("bind-key F3 next-level"));
}

void ImpuzzableManager::SaveSettings()
{
	GetConsoleManager()->ExecuteCommand(_T("save-application-config-file ")+GetApplicationCommandFilename());
}

bool ImpuzzableManager::Open()
{
	bool lOk = Parent::Open();
	if (lOk)
	{
		mPauseButton = ICONBTNA("btn_pause.png", "");
		int x = mRenderArea.mLeft + 12;
		int y = mRenderArea.mBottom - 12 - 32;
		mUiManager->GetDesktopWindow()->AddChild(mPauseButton, x, y);
		mPauseButton->SetVisible(true);
		mPauseButton->SetOnClick(ImpuzzableManager, OnPauseButton);
	}
	if (lOk)
	{
		mMenu = new Life::Menu(mUiManager, GetResourceManager());
		mMenu->SetButtonTapSound(_T("tap.wav"), 0.2f, 0.05f);
	}
	return lOk;
}

void ImpuzzableManager::Close()
{
	ScopeLock lLock(GetTickLock());
	delete mPauseButton;
	mPauseButton = 0;
	delete mMenu;
	mMenu = 0;
	if (mSunlight)
	{
		delete mSunlight;
		mSunlight = 0;
	}
	Parent::Close();
}

void ImpuzzableManager::SetIsQuitting()
{
	((ImpuzzableConsoleManager*)GetConsoleManager())->GetUiConsole()->SetVisible(false);
	Parent::SetIsQuitting();
}

void ImpuzzableManager::SetFade(float pFadeAmount)
{
	(void)pFadeAmount;
}



bool ImpuzzableManager::Render()
{
	if (!mIsShaking && mShakeTimer.QuerySplitTime() < 4.0)
	{
		mSunlight->Tick(mCameraTransform.GetOrientation());
	}

	RenderBackground();

	bool ok = Parent::Render();
	if (mLevel)
	{
		mUiManager->GetPainter()->SetLineWidth(1);
		mLevel->RenderOutline();
	}
	return ok;
}

void ImpuzzableManager::RenderBackground()
{
	if (!mUiManager->CanRender())
	{
		return;
	}

	mUiManager->GetPainter()->PrePaint(false);
	mUiManager->GetPainter()->ResetClippingRect();
	mUiManager->GetRenderer()->SetDepthWriteEnabled(false);
	mUiManager->GetRenderer()->SetDepthTestEnabled(false);
	mUiManager->GetRenderer()->SetLightsEnabled(false);
	mUiManager->GetRenderer()->SetTexturingEnabled(false);
	vec3 lTopColor(84,217,245);
	vec3 lBottomColor(12,19,87);
	static float lAngle = 0;
	vec3 lDeviceAcceleration(mUiManager->GetAccelerometer());
	float lAcceleration = lDeviceAcceleration.GetLength();
	float lScreenAngle = 0;
	if (lAcceleration > 0.7f && std::abs(lDeviceAcceleration.y) < 0.7f*lAcceleration)
	{
		vec2 lScreenDirection(lDeviceAcceleration.x, lDeviceAcceleration.z);
		lScreenDirection.Normalize();
		lScreenAngle = -lScreenDirection.GetAngle() - PIF/2;
	}
	Math::RangeAngles(lAngle, lScreenAngle);
	lAngle = Math::Lerp(lAngle, lScreenAngle, 0.1f);
	float x = mUiManager->GetCanvas()->GetWidth() / 2.0f;
	float y = mUiManager->GetCanvas()->GetHeight() / 2.0f;
	float l = ::sqrt(x*x + y*y);
	x /= l;
	y /= l;
	vec2 tl(-x,+y);
	vec2 tr(+x,+y);
	vec2 bl(-x,-y);
	vec2 br(+x,-y);
	tl.RotateAround(vec2(), lAngle);
	tr.RotateAround(vec2(), lAngle);
	bl.RotateAround(vec2(), lAngle);
	br.RotateAround(vec2(), lAngle);
	vec3 tlc = Math::Lerp(lBottomColor, lTopColor, tl.y*0.5f+0.5f)/255;
	vec3 trc = Math::Lerp(lBottomColor, lTopColor, tr.y*0.5f+0.5f)/255;
	vec3 blc = Math::Lerp(lBottomColor, lTopColor, bl.y*0.5f+0.5f)/255;
	vec3 brc = Math::Lerp(lBottomColor, lTopColor, br.y*0.5f+0.5f)/255;
	mUiManager->GetPainter()->SetColor(Color::CreateColor(tlc.x, tlc.y, tlc.z, 1), 0);
	mUiManager->GetPainter()->SetColor(Color::CreateColor(trc.x, trc.y, trc.z, 1), 1);
	mUiManager->GetPainter()->SetColor(Color::CreateColor(brc.x, brc.y, brc.z, 1), 2);
	mUiManager->GetPainter()->SetColor(Color::CreateColor(blc.x, blc.y, blc.z, 1), 3);
	mUiManager->GetPainter()->FillShadedRect(0, 0, mUiManager->GetCanvas()->GetWidth(), mUiManager->GetCanvas()->GetHeight());
	mUiManager->GetRenderer()->SetDepthWriteEnabled(true);
	mUiManager->GetRenderer()->SetDepthTestEnabled(true);
	mUiManager->GetRenderer()->SetLightsEnabled(true);
	mUiManager->GetRenderer()->Renderer::SetTexturingEnabled(true);
}

bool ImpuzzableManager::Paint()
{
	mUiManager->GetPainter()->SetLineWidth(3);
	if (!Parent::Paint())
	{
		return false;
	}
	std::vector<Piece*>::iterator x;
	for (x = mPieces.begin(); x != mPieces.end(); ++x)
	{
		Piece* lPiece = *x;
		if (!lPiece->IsDragging())
		{
			continue;
		}
		const vec3 p = lPiece->GetMoveTarget();
		const vec2 c = mUiManager->GetRenderer()->PositionToScreenCoord(p, 0);
		mUiManager->GetPainter()->DrawArc((int)c.x, (int)c.y, 10, 10, 0, 360, true);
	}
	return true;
}

void ImpuzzableManager::HandleDrag()
{
	if (mMenu->GetDialog())
	{
		return;
	}
	const int w = mUiManager->GetCanvas()->GetWidth();
	const float lTouchSideScale = 1.28f;	// Inches.
	const float lTouchScale = lTouchSideScale / (float)mUiManager->GetDisplayManager()->GetPhysicalScreenSize();
	const float lResolutionMargin = w / 50.0f;
	const int m = (int)Math::Lerp(lTouchScale * w * 0.25f, lResolutionMargin, 0.7f);
	const int r = m-2;
	typedef UiLepra::Touch::DragManager::DragList DragList;
	DragList& lDragList = mUiManager->GetDragManager()->GetDragList();
	for (DragList::iterator x = lDragList.begin(); x != lDragList.end(); ++x)
	{
		Piece* lPiece = 0;
		if (x->mIsNew)
		{
			lPiece = PickPiece(*x, r);
		}
		else
		{
			lPiece = GetDraggedPiece(*x);
		}
		if (!lPiece)
		{
			continue;
		}
		if (!x->mIsPress)
		{
			x->mFlags = 0;
			lPiece->SetDragging(false, CAM_DISTANCE);
		}
		else
		{
			DragPiece(lPiece, x->mLast);
		}
	}
}

Piece* ImpuzzableManager::PickPiece(UiLepra::Touch::Drag& pDrag, int pRadius)
{
	const vec3 v = mUiManager->GetRenderer()->ScreenCoordToVector(pDrag.mStart);
	Tbc::PhysicsManager* lPhysics = GetPhysicsManager();
	std::vector<Piece*>::iterator x;
	vec3 lHits[4];
	std::vector<Piece*> lTappedPieces;
	typedef std::vector<Tbc::PhysicsManager::BodyID> BodyArray;
	BodyArray lBodies;
	for (x = mPieces.begin(); x != mPieces.end(); ++x)
	{
		Piece* lPiece = *x;
		if (lPiece->IsDragging())
		{
			continue;
		}
		int lHitCount = 0;
		lBodies.clear();
		lPiece->GetBodyIds(lBodies);
		for (BodyArray::iterator y = lBodies.begin(); y != lBodies.end(); ++y)
		{
			lHitCount = lPhysics->QueryRayCollisionAgainst(mCameraTransform.mPosition, v, CAM_DISTANCE*2, *y, lHits, LEPRA_ARRAY_COUNT(lHits));
			if (lHitCount)
			{
				break;
			}
		}
		if (lHitCount)
		{
			lPiece->SetDragPosition(lHits[0]);
			lTappedPieces.push_back(lPiece);
		}
	}
	if (lTappedPieces.empty())
	{
		// TODO:
		// Find piece by distance to closest vertex, if not too far.
		pRadius;
	}
	if (lTappedPieces.empty())
	{
		return 0;
	}
	std::sort(lTappedPieces.begin(), lTappedPieces.end(), PieceDistanceAscending(mCameraTransform.mPosition));
	Piece* lPiece = lTappedPieces[0];
	pDrag.mFlags = lPiece->GetInstanceId();
	const float lDepth = mCameraTransform.mPosition.GetDistance(lPiece->GetDragPosition());
	lPiece->SetDragging(true, lDepth);
	return lPiece;
}

Piece* ImpuzzableManager::GetDraggedPiece(UiLepra::Touch::Drag& pDrag)
{
	if (!pDrag.mFlags)
	{
		return 0;
	}
	Piece* lPiece = (Piece*)GetContext()->GetObject(pDrag.mFlags);
	if (!lPiece)
	{
		pDrag.mFlags = 0;
	}
	return lPiece;
}

void ImpuzzableManager::DragPiece(Piece* pPiece, const PixelCoord& pScreenPoint)
{
	const vec3 d = pPiece->GetDragPosition();
	const vec3 lTarget = To3dPoint(pScreenPoint, pPiece->GetDragDepth());
	pPiece->SetMoveTarget(lTarget);
}

vec3 ImpuzzableManager::To3dPoint(const PixelCoord& pCoord, float pDepth) const
{
	const vec3 v = mUiManager->GetRenderer()->ScreenCoordToVector(pCoord);
	return v * pDepth + mCameraTransform.mPosition;
}



bool ImpuzzableManager::DidFinishLevel()
{
	int lLevel;
	v_get(lLevel, =, GetVariableScope(), RTVAR_GAME_LEVEL, 0);
	mLog.Headlinef(_T("Level %i done!"), lLevel);
	OnPauseButton(0);
	UiCure::UserSound2dResource* lFinishSound = new UiCure::UserSound2dResource(mUiManager, UiLepra::SoundManager::LOOP_NONE);
	new UiCure::SoundReleaser(GetResourceManager(), mUiManager, GetContext(), _T("finish.wav"), lFinishSound, 0.5f, Random::Uniform(0.98f, 1.02f));

	bool lRunAds;
	v_get(lRunAds, =, GetVariableScope(), RTVAR_GAME_RUNADS, true);
	if (lRunAds)
	{
		Impuzzable__ShowAd();
	}

	return true;
}

int ImpuzzableManager::StepLevel(int pCount)
{
	mCutsLeft = 25;
	mShakesLeft = 2;
	mPercentDone = 0;
	float lPreviousScore;
	v_get(lPreviousScore, =(float), GetVariableScope(), RTVAR_GAME_SCORE, 0.0);
	if (pCount > 0)
	{
		lPreviousScore += mLevelScore;
	}
	else if (pCount < 0)
	{
		lPreviousScore = 0;
	}
	mLevelScore = 0;
	mQuickCutCount = 0;

	int lLevelNumber;
	v_get(lLevelNumber, =, GetVariableScope(), RTVAR_GAME_LEVEL, 0);
	lLevelNumber = std::max(0, lLevelNumber+pCount);
	bool lVaryShapes;
	v_get(lVaryShapes, =, GetVariableScope(), RTVAR_GAME_LEVELSHAPEALTERNATE, false);
	mLevel->GenerateLevel(GetPhysicsManager(), lVaryShapes, lLevelNumber);
	int lPieceCount = lLevelNumber + 2;
	for (int x = 0; x < lPieceCount; ++x)
	{
		CreatePiece(x, 0);
	}
	while ((int)mPieces.size() > lPieceCount)
	{
		Piece* lPiece = mPieces.back();
		GetContext()->PostKillObject(lPiece->GetInstanceId());
		mPieces.pop_back();
	}
	v_set(GetVariableScope(), RTVAR_GAME_LEVEL, lLevelNumber);
	v_set(GetVariableScope(), RTVAR_GAME_SCORE, (double)lPreviousScore);
	return lLevelNumber;
}



Cure::RuntimeVariableScope* ImpuzzableManager::GetVariableScope() const
{
	return (Parent::GetVariableScope());
}



bool ImpuzzableManager::InitializeUniverse()
{
	// Create dummy explosion to ensure all geometries loaded and ready, to avoid LAAAG when first exploading.
	UiTbc::ParticleRenderer* lParticleRenderer = (UiTbc::ParticleRenderer*)mUiManager->GetRenderer()->GetDynamicRenderer(_T("particle"));
	const vec3 v;
	lParticleRenderer->CreateExplosion(vec3(0,0,-2000), 1, v, 1, 1, v, v, v, v, v, 1, 1, 1, 1);

	mCutsLeft = 25;
	mShakesLeft = 2;
	mLevel = (Level*)Parent::CreateContextObject(_T("level"), Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED, 0);
	StepLevel(0);
	mSunlight = new Sunlight(mUiManager);
	return true;
}

void ImpuzzableManager::TickInput()
{
	TickNetworkInput();
	TickUiInput();
}

void ImpuzzableManager::TickUiInput()
{
	mUiManager->GetInputManager()->SetCursorVisible(true);

	const int lPhysicsStepCount = GetTimeManager()->GetAffordedPhysicsStepCount();
	if (lPhysicsStepCount > 0 && mAllowMovementInput)
	{
	}
}

void ImpuzzableManager::TickUiUpdate()
{
	((ImpuzzableConsoleManager*)GetConsoleManager())->GetUiConsole()->Tick();
}

void ImpuzzableManager::SetLocalRender(bool pRender)
{
	(void)pRender;
}



void ImpuzzableManager::CreatePiece(int pIndex, const vec3* pPosition)
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
	if ((int)mPieces.size() > pIndex)
	{
		GetPhysicsManager()->SetBodyPosition(mPieces[pIndex]->GetRootBodyId(), lPosition);
		return;
	}
	Piece* lPiece = (Piece*)Parent::CreateContextObject(_T("testblock"), Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED, 0);
	lPiece->SetRootPosition(lPosition);
	lPiece->SetRootVelocity(RNDVEC(1.0f));
	lPiece->StartLoading();
	mPieces.push_back(lPiece);
}

Cure::ContextObject* ImpuzzableManager::CreateContextObject(const str& pClassId) const
{
	UiCure::Machine* lObject = 0;
	if (pClassId == _T("level"))
	{
		lObject = new Level(GetResourceManager(), pClassId, mUiManager);
	}
	else
	{
		lObject = new Piece(GetResourceManager(), pClassId, mUiManager);
	}
	lObject->SetAllowNetworkLogic(true);
	return (lObject);
}

/*Cure::ContextObject* ImpuzzableManager::CreateLogicHandler(const str& pType)
{
}*/

void ImpuzzableManager::OnLoadCompleted(Cure::ContextObject* pObject, bool pOk)
{
	if (pOk)
	{
		/*if (pObject->GetClassId() == _T("testblock")))
		{
		}*/
	}
	else
	{
		mLog.Errorf(_T("Could not load object of type %s."), pObject->GetClassId().c_str());
		GetContext()->PostKillObject(pObject->GetInstanceId());
	}
}

void ImpuzzableManager::OnCollision(const vec3& pForce, const vec3& pTorque, const vec3& pPosition,
	Cure::ContextObject* pObject1, Cure::ContextObject* pObject2,
	Tbc::PhysicsManager::BodyID pBody1Id, Tbc::PhysicsManager::BodyID pBody2Id)
{
	(void)pBody2Id;
	if (pForce.GetLengthSquared() < 400)	// Optimization.
	{
		return;
	}
	mCollisionSoundManager->OnCollision(pForce, pTorque, pPosition, pObject1, pObject2, pBody1Id, 5000, false);
}



void ImpuzzableManager::ShowInstruction()
{
	mPauseButton->SetVisible(false);

	UiTbc::Dialog* d = mMenu->CreateTbcDialog(Life::Menu::ButtonAction(this, &ImpuzzableManager::OnMenuAlternative), 0.8f, 0.6f);
	d->SetColor(BG_COLOR, OFF_BLACK, BLACK, BLACK);
	d->SetDirection(+1, false);
	UiTbc::FixedLayouter lLayouter(d);

	UiTbc::Label* lLabel1 = new UiTbc::Label(BRIGHT_TEXT, _T("Swipe to cut the box. Avoid hitting the"));
	lLayouter.AddComponent(lLabel1, 0, 6, 0, 1, 1);
	UiTbc::Label* lLabel2 = new UiTbc::Label(BRIGHT_TEXT, _T("pieces. Cut away 85% to complete level."));
	lLayouter.AddComponent(lLabel2, 1, 6, 0, 1, 1);

	UiTbc::Button* lResetLevelButton = new UiTbc::Button(GREEN_BUTTON, _T("OK"));
	lLayouter.AddButton(lResetLevelButton, -9, 2, 3, 0, 1, 1, true);

	v_set(GetVariableScope(), RTVAR_PHYSICS_HALT, true);
}

void ImpuzzableManager::OnPauseButton(UiTbc::Button* pButton)
{
	if (pButton)
	{
		mMenu->OnTapSound(pButton);
	}
	mPauseButton->SetVisible(false);

	bool lRunAds;
	v_get(lRunAds, =, GetVariableScope(), RTVAR_GAME_RUNADS, true);
	bool lDidBuy = !lRunAds;
	UiTbc::Dialog* d = mMenu->CreateTbcDialog(Life::Menu::ButtonAction(this, &ImpuzzableManager::OnMenuAlternative), 0.6f, lDidBuy? 0.6f : 0.7f);
	d->SetColor(BG_COLOR, OFF_BLACK, BLACK, BLACK);
	d->SetDirection(+1, false);
	UiTbc::FixedLayouter lLayouter(d);
	int lRow = 0;
	const int lRowCount = lDidBuy? 3 : 4;

	bool lIsPaused = false;
	if (LEVEL_DONE())
	{
		UiTbc::Label* lLabel = new UiTbc::Label(BRIGHT_TEXT, _T("Level completed (85%)"));
		lLabel->SetFontId(mUiManager->SetScaleFont(1.2f));
		mUiManager->SetMasterFont();
		lLabel->SetIcon(UiTbc::Painter::INVALID_IMAGEID, UiTbc::TextComponent::ICON_CENTER);
		lLabel->SetAdaptive(false);
		lLayouter.AddComponent(lLabel, lRow++, lRowCount, 0, 1, 1);

		UiTbc::Button* lNextLevelButton = new UiTbc::Button(GREEN_BUTTON, _T("Next level"));
		lLayouter.AddButton(lNextLevelButton, -1, lRow++, lRowCount, 0, 1, 1, true);
	}
	else
	{
		UiTbc::Label* lLabel;
		if (mCutsLeft > 0)
		{
			lLabel = new UiTbc::Label(BRIGHT_TEXT, _T("Paused"));
			lIsPaused = true;
		}
		else
		{
			lLabel = new UiTbc::Label(RED_BUTTON, _T("Out of cuts!"));
		}
		lLabel->SetFontId(mUiManager->SetScaleFont(1.2f));
		mUiManager->SetMasterFont();
		lLabel->SetIcon(UiTbc::Painter::INVALID_IMAGEID, UiTbc::TextComponent::ICON_CENTER);
		lLabel->SetAdaptive(false);
		lLayouter.AddComponent(lLabel, lRow++, lRowCount, 0, 1, 1);
	}

	UiTbc::Button* lResetLevelButton = new UiTbc::Button(ORANGE_BUTTON, _T("Reset level"));
	lLayouter.AddButton(lResetLevelButton, -3, lRow++, lRowCount, 0, 1, 1, true);

	if (lRow < 3)
	{
		UiTbc::Button* lRestartFrom1stLevelButton = new UiTbc::Button(RED_BUTTON, _T("Reset game"));
		lLayouter.AddButton(lRestartFrom1stLevelButton, -4, lRow++, lRowCount, 0, 1, 1, true);
	}

	if (!lDidBuy)
	{
		UiTbc::Button* lBuyButton = new UiTbc::Button(BLACK_BUTTON, _T("Buy full"));
		lBuyButton->SetFontColor(DIM_TEXT);
		lLayouter.AddButton(lBuyButton, -5, lRow++, lRowCount, 0, 1, 1, true);
	}

	if (lIsPaused)
	{
		UiTbc::Button* lCloseButton = new UiTbc::Button(DIM_RED, _T("X"));
		lLayouter.AddCornerButton(lCloseButton, -9);
	}

	v_set(GetVariableScope(), RTVAR_PHYSICS_HALT, true);
}

void ImpuzzableManager::OnMenuAlternative(UiTbc::Button* pButton)
{
	int lLevel;
	v_get(lLevel, =, GetVariableScope(), RTVAR_GAME_LEVEL, 0);
	switch (pButton->GetTag())
	{
		case -1:	GetConsoleManager()->PushYieldCommand(_T("step-level +1"));				break;
		case -2:	GetConsoleManager()->PushYieldCommand(_T("step-level -1"));				break;
		case -3:	GetConsoleManager()->PushYieldCommand(_T("step-level 0"));				break;
		case -4:	GetConsoleManager()->PushYieldCommand(strutil::Format(_T("step-level %i"), -lLevel));	break;
		case -5:	Impuzzable__Buy();										break;
	}
	mPauseButton->SetVisible(true);
	HiResTimer::StepCounterShadow();
	v_set(GetVariableScope(), RTVAR_PHYSICS_HALT, false);
}



void ImpuzzableManager::ScriptPhysicsTick()
{
	// Camera moves in a "moving average" kinda curve (halfs the distance in x seconds).
	const float lPhysicsTime = GetTimeManager()->GetAffordedPhysicsTotalTime();
	if (lPhysicsTime > 1e-5)
	{
		bool lIsFirstTime;
		v_get(lIsFirstTime, =, GetVariableScope(), RTVAR_GAME_FIRSTTIME, true);
		if (lIsFirstTime)
		{
			ShowInstruction();
			v_set(GetVariableScope(), RTVAR_GAME_FIRSTTIME, false);
		}
		HandleDrag();
		MoveCamera(lPhysicsTime);
		UpdateCameraPosition(false);
	}

	if (mNextLevelTimer.IsStarted())
	{
		if (mNextLevelTimer.QueryTimeDiff() > 5.0)
		{
			mNextLevelTimer.Stop();
			strutil::strvec lResourceTypes;
			lResourceTypes.push_back(_T("RenderImg"));
			lResourceTypes.push_back(_T("Geometry"));
			lResourceTypes.push_back(_T("GeometryRef"));
			lResourceTypes.push_back(_T("Physics"));
			lResourceTypes.push_back(_T("PhysicsShared"));
			lResourceTypes.push_back(_T("RamImg"));
			lResourceTypes.push_back(_T("Sound3D"));
			lResourceTypes.push_back(_T("Sound2D"));
			GetResourceManager()->ForceFreeCache(lResourceTypes);
			GetResourceManager()->ForceFreeCache(lResourceTypes);	// Call again to release any dependent resources.
		}
	}

	Parent::ScriptPhysicsTick();
}

void ImpuzzableManager::HandleWorldImpuzzablearies()
{
	/*std::vector<Piece*> lLostObjectArray;
	typedef Cure::ContextManager::ContextObjectTable ContextTable;
	const ContextTable& lObjectTable = GetContext()->GetObjectTable();
	ContextTable::const_iterator x = lObjectTable.begin();
	for (; x != lObjectTable.end(); ++x)
	{
		Cure::ContextObject* lObject = x->second;
		if (lObject->IsLoaded() && lObject->GetPhysics())
		{
			const vec3 lPosition = lObject->GetPosition();
			if (!Math::IsInRange(lPosition.x, -10.0f, +10.0f) ||
				!Math::IsInRange(lPosition.y, -10.0f, +10.0f) ||
				!Math::IsInRange(lPosition.z, -10.0f, +10.0f))
			{
				lLostObjectArray.push_back(lObject->GetInstanceId());
			}
		}
	}
	if (!lLostObjectArray.empty())
	{
		ScopeLock lLock(GetTickLock());
		std::vector<Piece*>::const_iterator y = lLostObjectArray.begin();
		for (; y != lLostObjectArray.end(); ++y)
		{
			DeleteContextObject(*y);
			std::vector<Piece*>::iterator x;
			for (x = mPieces.begin(); x != mPieces.end(); ++x)
			{
				if (*x == *y)
				{
					mPieces.erase(x);
					break;
				}
			}
		}
	}*/
}

void ImpuzzableManager::MoveCamera(float pFrameTime)
{
	if (mIsCutting || mIsShaking)
	{
		return;
	}

	int lLevel;
	v_get(lLevel, =, GetVariableScope(), RTVAR_GAME_LEVEL, 0);
	float lBaseSpeed = 0.15f + lLevel * 0.01f;
	mCameraAngle += lBaseSpeed*mCameraRotateSpeed*pFrameTime;
	mCameraRotateSpeed = Math::Lerp(mCameraRotateSpeed, (mCameraRotateSpeed < 0)? -1.0f : 1.0f, 0.07f);
	if (mCameraAngle > 2*PIF)
	{
		mCameraAngle -= 2*PIF;
	}
	quat q(0, vec3(0,1,0));
	vec3 p(0,-CAM_DISTANCE,0);
	mCameraTransform = xform(q, p);
	mCameraTransform.RotateAroundAnchor(vec3(), vec3(0,0,1), mCameraAngle);
	mCameraTransform.RotatePitch(-sin(mCameraAngle*2)*0.3f);
	mCameraTransform.MoveUp(sin(mCameraAngle*2)*1.5f);
}

void ImpuzzableManager::UpdateCameraPosition(bool pUpdateMicPosition)
{
	mUiManager->SetCameraPosition(mCameraTransform);
	if (pUpdateMicPosition)
	{
		mUiManager->SetMicrophonePosition(mCameraTransform, vec3());
	}
}



void ImpuzzableManager::PrintText(const str& s, int x, int y) const
{
	Color lOldColor = mUiManager->GetPainter()->GetColor(0);
	mUiManager->GetPainter()->SetColor(DARK_BLUE, 0);
	mUiManager->GetPainter()->PrintText(s, x, y+1);
	mUiManager->GetPainter()->SetColor(lOldColor, 0);
	mUiManager->GetPainter()->PrintText(s, x, y);
}

void ImpuzzableManager::DrawImage(UiTbc::Painter::ImageID pImageId, float cx, float cy, float w, float h, float pAngle) const
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



void ImpuzzableManager::PainterImageLoadCallback(UiCure::UserPainterKeepImageResource* pResource)
{
	if (pResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		mUiManager->GetDesktopWindow()->GetImageManager()->AddLoadedImage(*pResource->GetRamData(), pResource->GetData(),
			UiTbc::GUIImageManager::CENTERED, UiTbc::GUIImageManager::ALPHABLEND, 255);
	}
}



loginstance(GAME, ImpuzzableManager);



}
