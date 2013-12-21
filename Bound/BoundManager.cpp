
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "BoundManager.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/TimeManager.h"
#include "../Lepra/Include/Plane.h"
#include "../Lepra/Include/TimeLogger.h"
#include "../Lepra/Include/Unordered.h"
#include "../UiCure/Include/UiCollisionSoundManager.h"
#include "../UiCure/Include/UiIconButton.h"
#include "../UiCure/Include/UiMachine.h"
#include "../UiCure/Include/UiSoundReleaser.h"
#include "../UiLepra/Include/UiTouchDrag.h"
#include "../UiTBC/Include/GUI/UiDesktopWindow.h"
#include "../UiTBC/Include/GUI/UiFixedLayouter.h"
#include "../UiTBC/Include/UiMaterial.h"
#include "../UiTBC/Include/UiParticleRenderer.h"
#include "../UiTBC/Include/UiTriangleBasedGeometry.h"
#include "../Lepra/Include/Random.h"
#include "../Life/LifeClient/UiConsole.h"
#include "Ball.h"
#include "Bound.h"
#include "BoundConsoleManager.h"
#include "Level.h"
#include "RtVar.h"
#include "Sunlight.h"
#include "Version.h"

#define BG_COLOR		Color(5, 10, 15, 230)
#define CAM_DISTANCE		7.0f
#define BALL_RADIUS		0.15f
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


namespace Bound
{



void Bound__ShowAd();
void Bound__Buy();



BoundManager::BoundManager(Life::GameClientMasterTicker* pMaster, const Cure::TimeManager* pTime,
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
	mCameraTransform(QuaternionF(), Vector3DF(0, -CAM_DISTANCE, 0)),
	mLevelCompleted(false),
	mPauseButton(0),
	mIsCutting(false),
	mIsShaking(false),
	mCutsLeft(1),
	mShakesLeft(1),
	mLastCutMode(CUT_NORMAL),
	mCutSoundPitch(1),
	mQuickCutCount(0),
	mPreviousScore(0),
	mLevelScore(0),
	mShakeSound(0)
{
	mCollisionSoundManager = new UiCure::CollisionSoundManager(this, pUiManager);
	mCollisionSoundManager->AddSound(_T("explosion"),	UiCure::CollisionSoundManager::SoundResourceInfo(0.8f, 0.4f, 0));
	mCollisionSoundManager->AddSound(_T("rubber"),		UiCure::CollisionSoundManager::SoundResourceInfo(1.0f, 0.5f, 0));
	mCollisionSoundManager->PreLoadSound(_T("explosion"));

	SetConsoleManager(new BoundConsoleManager(GetResourceManager(), this, mUiManager, GetVariableScope(), mRenderArea));

	GetPhysicsManager()->SetSimulationParameters(0.0f, 0.03f, 0.2f);

	CURE_RTVAR_SET(GetVariableScope(), RTVAR_GAME_FIRSTTIME, true);
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_GAME_LEVEL, 0);
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_GAME_LEVELSHAPEALTERNATE, true);
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_GAME_RUNADS, true);
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_UI_SOUND_MASTERVOLUME, 1.0);
	/*
#define RNDMZEL \
	for (int x = 0; x < 20; ++x) \
	{ \
		size_t a = Random::GetRandomNumber() % lNGon.size(); \
		size_t b = Random::GetRandomNumber() % lNGon.size(); \
		if (a != b) \
			std::swap(lNGon[1], lNGon[b]); \
	}
	std::vector<Vector3DF> lNGon;
#define CLRL \
	lNGon.clear(); \
	lNGon.push_back(Vector3DF(-2.1314f, 0, -0.333f));	\
	for (int x = 0; x < 10; ++x)	\
		lNGon.push_back(lNGon[lNGon.size()-1] + Vector3DF(1,0,0.14f)*Random::Uniform(0.001f, 0.1f));	\
	for (int x = 0; x < 10; ++x)	\
		lNGon.push_back(lNGon[lNGon.size()-1] + Vector3DF(0.14f,0,-1)*Random::Uniform(0.001f, 0.1f));	\
	for (int x = 0; x < 10; ++x)	\
		lNGon.push_back(lNGon[lNGon.size()-1] + Vector3DF(-0.34f,0,-1)*Random::Uniform(0.001f, 0.1f));	\
	for (int x = 0; x < 10; ++x)	\
		lNGon.push_back(lNGon[lNGon.size()-1] + Vector3DF(-1,0,-0.14f)*Random::Uniform(0.001f, 0.1f));
	for (int y = 0; y < 100; ++y)
	{
		CLRL;
		RNDMZEL;
		std::vector<Vector3DF> lCopy(lNGon);
		CreateNGon(lNGon);
		mLog.Infof(_T("----------"));
		for (int x = 0; x < (int)lNGon.size(); ++x)
		{
			mLog.Infof(_T("%f;%f;"), lNGon[x].x, lNGon[x].z);
		}
		deb_assert(lNGon.size() == 5);
	}*/
}

BoundManager::~BoundManager()
{
	Close();

	delete mCollisionSoundManager;
	mCollisionSoundManager = 0;
}

void BoundManager::Suspend()
{
	if (!mMenu->GetDialog())
	{
		mPauseButton->SetVisible(false);
		OnPauseButton(0);
	}
}

void BoundManager::LoadSettings()
{
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_GAME_SPAWNPART, 1.0);

	Parent::LoadSettings();

	CURE_RTVAR_SET(GetVariableScope(), RTVAR_UI_2D_FONT, _T("Verdana"));
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_UI_2D_FONTFLAGS, 0);
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_UI_3D_FOV, 52.0);
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_PHYSICS_MICROSTEPS, 3);
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_PHYSICS_NOCLIP, false);

	GetConsoleManager()->ExecuteCommand(_T("bind-key F5 prev-level"));
	GetConsoleManager()->ExecuteCommand(_T("bind-key F6 next-level"));
}

void BoundManager::SaveSettings()
{
	GetConsoleManager()->ExecuteCommand(_T("save-application-config-file ")+GetApplicationCommandFilename());
}

bool BoundManager::Open()
{
	bool lOk = Parent::Open();
	if (lOk)
	{
		mPauseButton = ICONBTNA("btn_pause.png", "");
		int x = mRenderArea.mLeft + 12;
		int y = mRenderArea.mBottom - 12 - 32;
		mUiManager->GetDesktopWindow()->AddChild(mPauseButton, x, y);
		mPauseButton->SetVisible(true);
		mPauseButton->SetOnClick(BoundManager, OnPauseButton);
	}
	if (lOk)
	{
		mMenu = new Life::Menu(mUiManager, GetResourceManager());
		mMenu->SetButtonTapSound(_T("tap.wav"), 0.2f, 0.05f);
	}
	return lOk;
}

void BoundManager::Close()
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

void BoundManager::SetIsQuitting()
{
	((BoundConsoleManager*)GetConsoleManager())->GetUiConsole()->SetVisible(false);
	Parent::SetIsQuitting();
}

void BoundManager::SetFade(float pFadeAmount)
{
	(void)pFadeAmount;
}



bool BoundManager::Render()
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

void BoundManager::RenderBackground()
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
	Vector3DF lTopColor(84,217,245);
	Vector3DF lBottomColor(12,19,87);
	static float lAngle = 0;
	Vector3DF lDeviceAcceleration(-mUiManager->GetAccelerometer());
	float lAcceleration = lDeviceAcceleration.GetLength();
	float lScreenAngle = 0;
	if (lAcceleration > 0.7f && std::abs(lDeviceAcceleration.y) < 0.7f*lAcceleration)
	{
		Vector2DF lScreenDirection(lDeviceAcceleration.x, lDeviceAcceleration.z);
		lScreenDirection.Normalize();
		lScreenAngle = -lScreenDirection.GetAngle() + PIF/2;
	}
	Math::RangeAngles(lAngle, lScreenAngle);
	lAngle = Math::Lerp(lAngle, lScreenAngle, 0.1f);
	float x = mUiManager->GetCanvas()->GetWidth() / 2.0f;
	float y = mUiManager->GetCanvas()->GetHeight() / 2.0f;
	float l = ::sqrt(x*x + y*y);
	x /= l;
	y /= l;
	Vector2DF tl(-x,+y);
	Vector2DF tr(+x,+y);
	Vector2DF bl(-x,-y);
	Vector2DF br(+x,-y);
	tl.RotateAround(Vector2DF(), lAngle);
	tr.RotateAround(Vector2DF(), lAngle);
	bl.RotateAround(Vector2DF(), lAngle);
	br.RotateAround(Vector2DF(), lAngle);
	Vector3DF tlc = Math::Lerp(lBottomColor, lTopColor, tl.y*0.5f+0.5f)/255;
	Vector3DF trc = Math::Lerp(lBottomColor, lTopColor, tr.y*0.5f+0.5f)/255;
	Vector3DF blc = Math::Lerp(lBottomColor, lTopColor, bl.y*0.5f+0.5f)/255;
	Vector3DF brc = Math::Lerp(lBottomColor, lTopColor, br.y*0.5f+0.5f)/255;
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

bool BoundManager::Paint()
{
	mUiManager->GetPainter()->SetLineWidth(3);
	if (!Parent::Paint())
	{
		return false;
	}
	bool lIsUsingACut = HandleCutting();
	if (mLevel)
	{
		mUiManager->GetPainter()->SetColor(BRIGHT_TEXT);

		const str lCompletionText = strutil::Format(_T("Reduced: %.1f%%"), mPercentDone);
		int cw = mUiManager->GetPainter()->GetStringWidth(lCompletionText);
		PrintText(lCompletionText, (mUiManager->GetCanvas()->GetWidth()-cw)/2, 7);

		mUiManager->SetScaleFont(0.9f);
		const str lScoreText = strutil::Format(_T("Score: %.0f"), mLevelScore+mPreviousScore);
		PrintText(lScoreText, 25, 9);

		int lLevel;
		CURE_RTVAR_GET(lLevel, =, GetVariableScope(), RTVAR_GAME_LEVEL, 0);
		const str lLevelText = strutil::Format(_T("Level: %i"), lLevel+1);
		int lw = mUiManager->GetPainter()->GetStringWidth(lLevelText);
		PrintText(lLevelText, mUiManager->GetCanvas()->GetWidth()-lw-25, 9);

		mUiManager->GetPainter()->SetColor(lIsUsingACut? DIM_RED_TEXT : DIM_TEXT);
		mUiManager->SetScaleFont(0.7f);
		const str lLinesText = strutil::Format(_T("Cuts: %i"), mCutsLeft);
		PrintText(lLinesText, mUiManager->GetCanvas()->GetWidth()-lw-24, 31);

		Vector3DF v = RNDPOSVEC()*255;
		Color lBlinkCol;
		lBlinkCol.Set(v.x, v.y, v.z, 1.0f);
		bool lIsShowingShake = (mIsShaking && mShakeTimer.QuerySplitTime() < 2.5);
		mUiManager->GetPainter()->SetColor(lIsShowingShake? lBlinkCol : DIM_TEXT);
		const str lShakesText = strutil::Format(_T("Shakes: %i"), mShakesLeft);
		PrintText(lShakesText, mUiManager->GetCanvas()->GetWidth()-lw-24, 48);
		mUiManager->SetMasterFont();
	}
	return true;
}

bool BoundManager::HandleCutting()
{
	mIsCutting = false;
	if (mMenu->GetDialog())
	{
		return false;
	}
	bool lAnyDragStarted = false;
	const int w = mUiManager->GetCanvas()->GetWidth();
	const int h = mUiManager->GetCanvas()->GetHeight();
	const float lTouchSideScale = 1.28f;	// Inches.
	const float lTouchScale = lTouchSideScale / (float)mUiManager->GetDisplayManager()->GetPhysicalScreenSize();
	const float lResolutionMargin = w / 50.0f;
	const int m = (int)Math::Lerp(lTouchScale * w * 0.25f, lResolutionMargin, 0.7f);
	const int r = m-2;
	const int d = r*2;
	typedef UiLepra::Touch::DragManager::DragList DragList;
	DragList& lDragList = mUiManager->GetDragManager()->GetDragList();
	for (DragList::iterator x = lDragList.begin(); x != lDragList.end(); ++x)
	{
		if (x->mFlags&DRAG_FLAG_INVALID)
		{
			continue;
		}
		mIsCutting = true;

		Vector2DF lFrom((float)x->mStart.x, (float)x->mStart.y);
		Vector2DF lTo((float)x->mLast.x, (float)x->mLast.y);
		Vector2DF lVector = lTo-lFrom;
		bool lIsVeryNewDrag = false;
		if (!(x->mFlags&DRAG_FLAG_STARTED))
		{
			const float lDragLength = lVector.GetLength();
			if (lDragLength < w*0.042f)
			{
				lIsVeryNewDrag = true;
			}
			else if (lDragLength > w*0.083f)
			{
				x->mFlags |= DRAG_FLAG_STARTED;
			}
		}
		const bool lDragStarted = ((x->mFlags&DRAG_FLAG_STARTED) == DRAG_FLAG_STARTED);
		mUiManager->GetPainter()->SetColor(lDragStarted? Color(30,140,20) : Color(140,30,20), 0);
		mUiManager->GetPainter()->DrawArc((int)lFrom.x-r, (int)lFrom.y-r, d, d, 0, 360, false);
		mUiManager->GetPainter()->DrawArc((int)lTo.x-r, (int)lTo.y-r, d, d, 0, 360, false);
		const float lCutLineLength = (float)(w+h);
		lVector.Normalize(lCutLineLength);
		Vector2DF lStart = lIsVeryNewDrag? lFrom : lFrom - lVector;
		if (!lDragStarted)
		{
			mUiManager->GetPainter()->DrawLine((int)lStart.x, (int)lStart.y, (int)lTo.x, (int)lTo.y);
		}
		else
		{
			Vector2DF lEnd = lTo + lVector;
			mUiManager->GetPainter()->DrawLine((int)lStart.x, (int)lStart.y, (int)lEnd.x, (int)lEnd.y);
		}

		// The plane goes through the camera and the projected midpoint of the line.
		PixelCoord lScreenMid((int)(lFrom.x+lTo.x)/2, (int)(lFrom.y+lTo.y)/2);
		Plane lCutPlaneDelimiter;
		Plane lCutPlane = ScreenLineToPlane(lScreenMid, PixelCoord((int)lTo.x, (int)lTo.y), lCutPlaneDelimiter);
		Vector3DF lCollisionPoint;

		if (lCutPlane.n.GetLengthSquared() > 0 && !lIsVeryNewDrag && CheckBallsPlaneCollition(lCutPlane, lDragStarted? 0 : &lCutPlaneDelimiter, lCollisionPoint))
		{
			// Invalidate swipe.
			--mCutsLeft;
			x->mFlags |= 1;

			// Explosions! YES!
			const int lParticles = Random::Uniform(5, 11);
			Vector3DF lStartFireColor(1.0f, 1.0f, 0.3f);
			Vector3DF lFireColor(0.6f, 0.4f, 0.2f);
			Vector3DF lStartSmokeColor(0.4f, 0.4f, 0.4f);
			Vector3DF lSmokeColor(0.2f, 0.2f, 0.2f);
			Vector3DF lShrapnelColor(0.3f, 0.3f, 0.3f);	// Default debris color is gray.
			UiTbc::ParticleRenderer* lParticleRenderer = (UiTbc::ParticleRenderer*)mUiManager->GetRenderer()->GetDynamicRenderer(_T("particle"));
			lParticleRenderer->CreateExplosion(lCollisionPoint, 0.05f, Vector3DF(), 0.15f, 0.25f, lStartFireColor, lFireColor, lStartSmokeColor, lSmokeColor, lShrapnelColor, lParticles, lParticles, lParticles/2, lParticles/3);
			UiCure::UserSound2dResource* lBreakSound = new UiCure::UserSound2dResource(mUiManager, UiLepra::SoundManager::LOOP_NONE);
			new UiCure::SoundReleaser(GetResourceManager(), mUiManager, GetContext(), _T("broken_window.wav"), lBreakSound, 0.5f, Random::Uniform(0.9f, 1.2f));
			mCutTimer.ReduceTimeDiff(-2.0);
			mLevelScore -= 200;
		}
		else if (lDragStarted && !x->mIsPress && mCutsLeft > 0)
		{
			--mCutsLeft;
			if (Cut(lCutPlane))
			{
				const int lSide = CheckIfPlaneSlicesBetweenBalls(lCutPlane);
				if (lSide > 0)
				{
					lCutPlane = -lCutPlane;
				}
				else
				{
					mLevelScore += 500;
				}
				float lCutX = mCameraTransform.GetOrientation().GetInverseRotatedVector(lCutPlane.n).x;
				if (mCutTimer.PopTimeDiff() < 1.4)
				{
					mCutSoundPitch += 0.3f;
					++mQuickCutCount;
				}
				else
				{
					mCutSoundPitch = Random::Uniform(0.9f, 1.1f);
					mQuickCutCount = 0;
				}
				mCameraRotateSpeed = (lCutX < 0)? +3+mQuickCutCount*0.5f : -3-mQuickCutCount*0.5f;
				UiCure::UserSound2dResource* lCutSound = new UiCure::UserSound2dResource(mUiManager, UiLepra::SoundManager::LOOP_NONE);
				new UiCure::SoundReleaser(GetResourceManager(), mUiManager, GetContext(), _T("cut.wav"), lCutSound, 0.5f, mCutSoundPitch);
			}
		}
		lAnyDragStarted |= lDragStarted;
	}
	return lAnyDragStarted;
}

Plane BoundManager::ScreenLineToPlane(const PixelCoord& pCoord, const PixelCoord& pEndPoint, Plane& pCutPlaneDelimiter)
{
	const PixelCoord lScreenNormal(pCoord.y-pEndPoint.y, pCoord.x-pEndPoint.x);
	const Vector3DF lDirectionToPlaneCenter = mUiManager->GetRenderer()->ScreenCoordToVector(pCoord);
	Vector3DF lBadNormal((float)lScreenNormal.x, 0, (float)lScreenNormal.y);
	lBadNormal = mCameraTransform.GetOrientation() * lBadNormal;
	const Plane lCutPlane(mCameraTransform.GetPosition(), lDirectionToPlaneCenter, lBadNormal);

	const Vector3DF lDirectionToEndPoint = mUiManager->GetRenderer()->ScreenCoordToVector(pEndPoint);
	pCutPlaneDelimiter = Plane(mCameraTransform.GetPosition(), lDirectionToEndPoint, lCutPlane.n.Cross(lDirectionToEndPoint));
	if (pCutPlaneDelimiter.n*lDirectionToPlaneCenter < 0)
	{
		pCutPlaneDelimiter = -pCutPlaneDelimiter;
	}

	return lCutPlane;
}

bool BoundManager::Cut(Plane pCutPlane)
{
	const int lSide = CheckIfPlaneSlicesBetweenBalls(pCutPlane);
	if (lSide == 0)	// Balls on both sides.
	{
		return DoCut(mLevel->GetMesh(), pCutPlane, CUT_ADD_WINDOW);
	}
	if (lSide == -1)
	{
		pCutPlane = -pCutPlane;
	}
	DoCut(mLevel->GetWindowMesh(), pCutPlane, CUT_WINDOW_ITSELF);
	return DoCut(mLevel->GetMesh(), pCutPlane, CUT_NORMAL);
}

bool BoundManager::DoCut(const UiTbc::TriangleBasedGeometry* pMesh, Plane pCutPlane, CutMode pCutMode)
{
	if (!pMesh)
	{
		return false;
	}

	mLastCutMode = pCutMode;
	// Plane normal now "points" toward vertices that stays. Those on the other side gets cut off. The new triangles will use this normal.
	TimeLogger lTimeLogger(&mLog, _T("CheckIfPlaneSlicesBetweenBalls + prep"));
	const QuaternionF q = mCameraTransform.GetOrientation();
	const int tc = pMesh->GetVertexCount() / 3;
	const float* v = pMesh->GetVertexData();
	const uint8* c = pMesh->GetColorData();
	if (mLastCutMode == CUT_NORMAL)
	{
		mCutVertices.reserve(tc*2*3*3);
		mCutColors.reserve(tc*2*3*4);
		mCutVertices.clear();
		mCutColors.clear();
	}
	else
	{
		mCutWindowVertices.reserve(tc*2*3*3);
		mCutWindowVertices.clear();
	}
	std::vector<Vector3DF> lNGon;
	std::unordered_set<int> pNGonMap;
	bool lDidCut = false;
	const bool lCreateWindow = (pCutMode == CUT_ADD_WINDOW);
	lTimeLogger.Transfer(_T("CutLoop"));
	for (int x = 0; x < tc; ++x)
	{
		Vector3DF p0(v[x*9+0], v[x*9+1], v[x*9+2]);
		Vector3DF p1(v[x*9+3], v[x*9+4], v[x*9+5]);
		Vector3DF p2(v[x*9+6], v[x*9+7], v[x*9+8]);
		const float d0 = pCutPlane.GetDistance(p0);
		const float d1 = pCutPlane.GetDistance(p1);
		const float d2 = pCutPlane.GetDistance(p2);
		if (d0 >= 0 && d1 >= 0 && d2 >= 0)
		{
			if (!lCreateWindow)
			{
				// All vertices on staying side of mesh. No cut, only copy.
				if (pCutMode == CUT_NORMAL)
				{
					mCutVertices.insert(mCutVertices.end(), &v[x*9], &v[x*9+9]);
				}
				else
				{
					mCutWindowVertices.insert(mCutWindowVertices.end(), &v[x*9], &v[x*9+9]);
				}
				if (c)
				{
					mCutColors.insert(mCutColors.end(), &c[x*12], &c[x*12+12]);
				}
			}
		}
		else if (d0 <= 0 && d1 <= 0 && d2 <= 0)
		{
			if (!lCreateWindow)
			{
				// The whole triangle got cut off - way to go! No cut, only discard.
				lDidCut = true;
			}
		}
		else
		{
			lDidCut = true;

			// Go ahead and cut. Ends up with either a triangle (single point on the positive side), or
			// a quad (two points on the positive side). Quad is cut along pseudo-shortest diagonal.
			Vector3DF d01 = p1-p0;
			Vector3DF d12 = p2-p1;
			Vector3DF d20 = p0-p2;
			if (d0 > 0 && d1 > 0)
			{
				// Quad cut.
				const float t3 = -d1 / (pCutPlane.n*d12);
				const float t4 = -d0 / (pCutPlane.n*d20);
				Vector3DF p3 = p1+t3*d12;
				Vector3DF p4 = p0+t4*d20;
				if (!lCreateWindow)
				{
					AddTriangle(p0, p1, p3, &c[x*12]);
					AddTriangle(p0, p3, p4, &c[x*12]);
				}
				AddNGonPoints(lNGon, pNGonMap, p3, p4);
			}
			else if (d1 > 0 && d2 > 0)
			{
				// Quad cut.
				const float t3 = -d2 / (pCutPlane.n*d20);
				const float t4 = -d1 / (pCutPlane.n*d01);
				Vector3DF p3 = p2+t3*d20;
				Vector3DF p4 = p1+t4*d01;
				if (!lCreateWindow)
				{
					AddTriangle(p1, p2, p3, &c[x*12]);
					AddTriangle(p1, p3, p4, &c[x*12]);
				}
				AddNGonPoints(lNGon, pNGonMap, p3, p4);
			}
			else if (d0 > 0 && d2 > 0)
			{
				// Quad cut.
				const float t3 = -d0 / (pCutPlane.n*d01);
				const float t4 = -d2 / (pCutPlane.n*d12);
				Vector3DF p3 = p0+t3*d01;
				Vector3DF p4 = p2+t4*d12;
				if (!lCreateWindow)
				{
					AddTriangle(p2, p0, p3, &c[x*12]);
					AddTriangle(p2, p3, p4, &c[x*12]);
				}
				AddNGonPoints(lNGon, pNGonMap, p3, p4);
			}
			else if (d0 > 0)
			{
				// Triangle cut.
				const float t3 = -d0 / (pCutPlane.n*d01);
				const float t4 = -d0 / (pCutPlane.n*d20);
				Vector3DF p3 = p0+t3*d01;
				Vector3DF p4 = p0+t4*d20;
				if (!lCreateWindow)
				{
					AddTriangle(p0, p3, p4, &c[x*12]);
				}
				AddNGonPoints(lNGon, pNGonMap, p3, p4);
			}
			else if (d1 > 0)
			{
				// Triangle cut.
				const float t3 = -d1 / (pCutPlane.n*d12);
				const float t4 = -d1 / (pCutPlane.n*d01);
				Vector3DF p3 = p1+t3*d12;
				Vector3DF p4 = p1+t4*d01;
				if (!lCreateWindow)
				{
					AddTriangle(p1, p3, p4, &c[x*12]);
				}
				AddNGonPoints(lNGon, pNGonMap, p3, p4);
			}
			else
			{
				// Triangle cut.
				const float t3 = -d2 / (pCutPlane.n*d20);
				const float t4 = -d2 / (pCutPlane.n*d12);
				Vector3DF p3 = p2+t3*d20;
				Vector3DF p4 = p2+t4*d12;
				if (!lCreateWindow)
				{
					AddTriangle(p2, p3, p4, &c[x*12]);
				}
				AddNGonPoints(lNGon, pNGonMap, p3, p4);
			}
		}
	}
	if (mLastCutMode == CUT_WINDOW_ITSELF)
	{
		if (lDidCut && mLastCutMode == CUT_WINDOW_ITSELF && mCutWindowVertices.empty())
		{
			mForceCutWindow = true;
		}
		return true;
	}
	lTimeLogger.Transfer(_T("CreateNGon()"));
	CreateNGon(lNGon);
	if (lNGon.size() < 3 || !lDidCut)
	{
		mCutVertices.clear();
		mCutWindowVertices.clear();
		mCutColors.clear();
		return false;
	}
	// Generate random colors and add.
	lTimeLogger.Transfer(_T("AddNGonTriangles()"));
	std::vector<uint8> lNGonColors;
	const size_t nvc = (lNGon.size()-2)*3;
	lNGonColors.resize(nvc*4);
	static const Color lColors[] = {ORANGE, RED, BLUE, GREEN, WHITE, CYAN, BLACK, ORANGE, MAGENTA, YELLOW, Color(255, 98, 128), Color(98, 60, 0)};
	static int lColorIndex = 0;
	const Color lNewColor = lColors[lColorIndex++];
	lColorIndex %= LEPRA_ARRAY_COUNT(lColors);
	for (size_t x = 0; x < nvc; ++x)
	{
		lNGonColors[x*4+0] = lNewColor.mRed;
		lNGonColors[x*4+1] = lNewColor.mGreen;
		lNGonColors[x*4+2] = lNewColor.mBlue;
		lNGonColors[x*4+3] = 255;
	}
	AddNGonTriangles(pCutPlane, lNGon, &lNGonColors[0]);
	return true;
}

void BoundManager::AddTriangle(const Vector3DF& v0, const Vector3DF& v1, const Vector3DF& v2, const uint8* pColors)
{
	if (mLastCutMode == CUT_NORMAL)
	{
		mCutVertices.push_back(v0.x); mCutVertices.push_back(v0.y); mCutVertices.push_back(v0.z);
		mCutVertices.push_back(v1.x); mCutVertices.push_back(v1.y); mCutVertices.push_back(v1.z);
		mCutVertices.push_back(v2.x); mCutVertices.push_back(v2.y); mCutVertices.push_back(v2.z);
		mCutColors.insert(mCutColors.end(), pColors, pColors+12);
	}
	else
	{
		mCutWindowVertices.push_back(v0.x); mCutWindowVertices.push_back(v0.y); mCutWindowVertices.push_back(v0.z);
		mCutWindowVertices.push_back(v1.x); mCutWindowVertices.push_back(v1.y); mCutWindowVertices.push_back(v1.z);
		mCutWindowVertices.push_back(v2.x); mCutWindowVertices.push_back(v2.y); mCutWindowVertices.push_back(v2.z);
	}
}

void BoundManager::AddNGonPoints(std::vector<Vector3DF>& pNGon, std::unordered_set<int>& pNGonMap, const Vector3DF& p0, const Vector3DF& p1)
{
	AddNGonPoint(pNGon, pNGonMap, p0);
	AddNGonPoint(pNGon, pNGonMap, p1);
}

void BoundManager::AddNGonPoint(std::vector<Vector3DF>& pNGon, std::unordered_set<int>& pNGonMap, const Vector3DF& p)
{
	const int lPositionHash = (int)(p.x*1051 + p.y*1117 + p.z*1187);
	if (pNGonMap.find(lPositionHash) == pNGonMap.end())
	{
		pNGonMap.insert(lPositionHash);
		pNGon.push_back(p);
	}
	/*(void)pNGonMap;
	std::vector<Vector3DF>::iterator x;
	for (x = pNGon.begin(); x != pNGon.end(); ++x)
	{
		if (x->GetDistanceSquared(p) < 1e-6)
		{
			return;
		}
	}
	pNGon.push_back(p);*/
}

void BoundManager::CreateNGon(std::vector<Vector3DF>& pNGon)
{
	//LineUpNGonSides(pNGon);
	//SortNGonSides(pNGon);
	// Iteratively producing better N-gon each loop.
	/*LineUpNGonBorders(pNGon, false);
	LineUpNGonBorders(pNGon, true);
	SimplifyNGon(pNGon);*/
	size_t lPreSize;
	size_t lSize;
	do
	{
		lPreSize = pNGon.size();
		LineUpNGonBorders(pNGon, false);
		LineUpNGonBorders(pNGon, true);
		SimplifyNGon(pNGon);
		lSize = pNGon.size();
	}
	while (lSize < lPreSize);
}

void BoundManager::LineUpNGonBorders(std::vector<Vector3DF>& pNGon, bool pSort)
{
	// Create triangles from N-gon.
	if (pNGon.size() <= 3)
	{
		return;
	}
	// Since the N-gon is convex, we only need to find the smallect angle for each vertex, and
	// those adjoining vertices will be the neighbours. It will automatically appear in order.
	// Rendering will determine if it's normal is aligned or perpendicular to the cutting plane.
	// 1. Start with any two vertices.
	Vector3DF p0 = pNGon[0];
	Vector3DF p1 = pNGon[1];
	Vector3DF t0 = (p1-p0).GetNormalized();
	// 2. Find the tangent with the smallect angle (biggest dot product) to this tangent.
	float lBiggestDot = -2;
	size_t lAdjoiningVertex = 2;
	size_t cnt = pNGon.size();
	Vector3DF lBoundaryVector;
	for (size_t idx = 2; idx < cnt; ++idx)
	{
		Vector3DF p2 = pNGon[idx];
		Vector3DF t1 = (p2-p1).GetNormalized();
		float lDot = t0*t1;
		if (lDot > lBiggestDot)
		{
			lBiggestDot = lDot;
			lAdjoiningVertex = idx;
			lBoundaryVector = t1;
		}
	}
	// 3. This found vertex and tangent contitutes our second outer point and our first found outer boundary.
	//    Place the remaining vertices in a list that we are going to search in.
	p0 = p1;
	p1 = pNGon[lAdjoiningVertex];
	t0 = lBoundaryVector;
	std::list<Vector3DF> lRemainingVertices;
	for (size_t idx = 0; idx < cnt; ++idx)
	{
		if (idx != 1 && idx != lAdjoiningVertex)
		{
			lRemainingVertices.push_back(pNGon[idx]);
		}
	}
	// 4. Clear the N-gon array, and append points on the "angle order" they appear, while dropping them in the search list.
	pNGon.clear();
	pNGon.push_back(p0);
	pNGon.push_back(p1);
	while (!lRemainingVertices.empty())
	{
		float lBestDistance = 1e5f;
		float lThisDistance = 1e5f;
		float lBiggestDot = -2;
		std::list<Vector3DF>::iterator lAdjoiningVertex = lRemainingVertices.begin();
		std::list<Vector3DF>::iterator x;
		for (x = lRemainingVertices.begin(); x != lRemainingVertices.end();)
		{
			Vector3DF p2 = *x;
			Vector3DF t1 = p2-p1;
			lThisDistance = t1.GetLength();
			t1.Div(lThisDistance);
			float lDot = pSort? t0*t1 : std::abs(t0*t1);
			/*if (lDot < -SAME_NORMAL)
			{
				if (p1.GetDistance(p0) >= p1.GetDistance(p2))
				{
					// Midpoint on same line (i.e. in reverse direction). Throw it away.
					std::list<Vector3DF>::iterator y = x;
					++y;
					lRemainingVertices.erase(x);
					x = y;
					continue;
				}
				// This vertex will come back as we move around one lap; if it's doubly redundant it will be handled then.
				++x;
				continue;
			}*/
			if (lDot >= lBiggestDot-CLOSE_NORMAL)
			{
				if (lDot >= SAME_NORMAL)
				{
					if (lThisDistance < lBestDistance)	// Closer vertex on same border?
					{
						lBestDistance = lThisDistance;
						lBiggestDot = lDot;
						lAdjoiningVertex = x;
					}
				}
				else	// Best vertex on upcoming border.
				{
					lBiggestDot = lDot;
					lAdjoiningVertex = x;
				}
				/*if (lDot > SAME_NORMAL)
				{
					// Uh-oh! We found two points on the same line; and the second one is better than this one (we can be certain as the N-gon is convex).
					p1 = p0;
					pNGon.pop_back();
					break;
				}*/
			}
			++x;
		}
		if (lBiggestDot > -1)
		{
			p0 = p1;
			p1 = *lAdjoiningVertex;
			t0 = (p1-p0).GetNormalized();
			pNGon.push_back(p1);
			lRemainingVertices.erase(lAdjoiningVertex);
		}
		else
		{
			break;
		}
	}
}

void BoundManager::SimplifyNGon(std::vector<Vector3DF>& pNGon)
{
	// Eliminate redundant vertices. N-gon must be sorted when entering.
	int cnt = (int)pNGon.size();
	for (int x = 0; x < cnt;)
	{
		size_t xn = NGON_INDEX(x-1);
		size_t xp = NGON_INDEX(x+1);
		Vector3DF p0 = pNGon[xn];
		Vector3DF p1 = pNGon[x];
		Vector3DF p2 = pNGon[xp];
		Vector3DF t0 = p1-p0;
		float l0 = t0.GetLength();
		t0.Div(l0);
		Vector3DF t1 = (p2-p1).GetNormalized();
		const float lDot = t0*t1;
		if (lDot > SAME_NORMAL)
		{
			// Kill middle vertex.
			pNGon.erase(pNGon.begin() + x);
			--cnt;
			continue;
		}
		if (lDot < -SAME_NORMAL)
		{
			// Kill middle vertex.
			float l1 = p1.GetDistance(p2);
			if (l0 < l1)
			{
				pNGon[xn] = p2;
			}
			pNGon.erase(pNGon.begin() + xp);
			--cnt;
			continue;
		}
		++x;
	}
}

void BoundManager::AddNGonTriangles(const Plane& pCutPlane, const std::vector<Vector3DF>& pNGon, const uint8* pColors)
{
	// Since the N-gon is convex, we hold on to the first point and step the other two around.
	Vector3DF p0 = pNGon[0];
	Vector3DF p1 = pNGon[1];
	Vector3DF p2 = pNGon[2];
	//const Vector3DF lTriangleNormal = (p1-p0).Cross(p2-p1);
	//const bool lNoFlip = (pCutPlane.n*lTriangleNormal < 0);
	const size_t cnt = pNGon.size();
	for (size_t idx = 2; idx < cnt; ++idx)
	{
		p2 = pNGon[idx];
		const Vector3DF lTriangleNormal = (p1-p0).Cross(p2-p1);
		const bool lNoFlip = (pCutPlane.n*lTriangleNormal < 0);
		if (lNoFlip)
		{
			AddTriangle(p0, p1, p2, pColors);
		}
		else
		{
			AddTriangle(p0, p2, p1, pColors);
		}
		p1 = p2;
		pColors += 4*3;
	}
	mLastCutPlane = pCutPlane;
}

int BoundManager::CheckIfPlaneSlicesBetweenBalls(const Plane& pCutPlane)
{
	int lSide = 0;
	std::vector<Cure::GameObjectId>::iterator x;
	for (x = mBalls.begin(); x != mBalls.end(); ++x)
	{
		Cure::ContextObject* lBall = GetContext()->GetObject(*x);
		if (!lBall)
		{
			continue;
		}
		Vector3DF p = lBall->GetRootPosition();
		const float d = pCutPlane.GetDistance(p);
		int s = (d < 0)? -1 : +1;
		if (lSide == 0)
		{
			lSide = s;
		}
		else if ((s<0) != (lSide<0))
		{
			return 0;
		}
	}
	return lSide;
}

bool BoundManager::CheckBallsPlaneCollition(const Plane& pCutPlane, const Plane* pCutPlaneDelimiter, Vector3DF& pCollisionPoint)
{
	std::vector<Cure::GameObjectId>::iterator x;
	for (x = mBalls.begin(); x != mBalls.end(); ++x)
	{
		Cure::ContextObject* lBall = GetContext()->GetObject(*x);
		if (!lBall)
		{
			continue;
		}
		Vector3DF p = lBall->GetRootPosition();
		float lDistance = pCutPlane.GetAbsDistance(p);
		if (lDistance < BALL_RADIUS)
		{
			if (!pCutPlaneDelimiter)
			{
				pCollisionPoint = p - pCutPlane.n*pCutPlane.GetDistance(p);
				return true;
			}
			// Check if the ball is on the "wrong" side of the touch endpoint of the cutting plane.
			lDistance = pCutPlaneDelimiter->GetDistance(p);
			if (lDistance >= -BALL_RADIUS)
			{
				pCollisionPoint = p - pCutPlane.n*pCutPlane.GetDistance(p);
				return true;
			}
		}
	}
	return false;
}

bool BoundManager::AttachTouchToBorder(PixelCoord& pPoint, int pMargin, int w, int h)
{
	int dt = std::abs(pPoint.y - pMargin);
	int dl = std::abs(pPoint.x - pMargin);
	int db = std::abs(pPoint.y - (h-pMargin));
	int dr = std::abs(pPoint.x - (w-pMargin));
	if (dt < dl && dt < dr && dt < db)
	{
		if (dt < pMargin*8)
		{
			pPoint.y = pMargin;
			return true;
		}
	}
	else if (db < dl && db < dr)
	{
		if (db < pMargin*8)
		{
			pPoint.y = h-pMargin;
			return true;
		}
	}
	else if (dl < dr)
	{
		if (dl < pMargin*8)
		{
			pPoint.x = pMargin;
			return true;
		}
	}
	else
	{
		if (dr < pMargin*8)
		{
			pPoint.x = w-pMargin;
			return true;
		}
	}
	return false;
}



bool BoundManager::DidFinishLevel()
{
	int lLevel;
	CURE_RTVAR_GET(lLevel, =, GetVariableScope(), RTVAR_GAME_LEVEL, 0);
	mLog.Headlinef(_T("Level %i done!"), lLevel);
	OnPauseButton(0);
	UiCure::UserSound2dResource* lFinishSound = new UiCure::UserSound2dResource(mUiManager, UiLepra::SoundManager::LOOP_NONE);
	new UiCure::SoundReleaser(GetResourceManager(), mUiManager, GetContext(), _T("finish.wav"), lFinishSound, 0.5f, Random::Uniform(0.98f, 1.02f));

	bool lRunAds;
	CURE_RTVAR_GET(lRunAds, =, GetVariableScope(), RTVAR_GAME_RUNADS, true);
	if (lRunAds)
	{
		Bound__ShowAd();
	}

	return true;
}

int BoundManager::StepLevel(int pCount)
{
	mCutsLeft = 25;
	mShakesLeft = 2;
	mPercentDone = 0;
	if (pCount > 0)
	{
		mPreviousScore += mLevelScore;
	}
	else if (pCount < 0)
	{
		mPreviousScore = 0;
	}
	mLevelScore = 0;
	mQuickCutCount = 0;

	int lLevelNumber;
	CURE_RTVAR_GET(lLevelNumber, =, GetVariableScope(), RTVAR_GAME_LEVEL, 0);
	lLevelNumber = std::max(0, lLevelNumber+pCount);
	bool lVaryShapes;
	CURE_RTVAR_GET(lVaryShapes, =, GetVariableScope(), RTVAR_GAME_LEVELSHAPEALTERNATE, false);
	mLevel->GenerateLevel(GetPhysicsManager(), lVaryShapes, lLevelNumber);
	int lBallCount = lLevelNumber + 2;
	for (int x = 0; x < lBallCount; ++x)
	{
		CreateBall(x, 0);
	}
	while ((int)mBalls.size() > lBallCount)
	{
		Cure::ContextObject* lBall = GetContext()->GetObject(mBalls.back());
		if (!lBall)
		{
			break;
		}
		mBalls.pop_back();
		DeleteContextObjectDelay(lBall, 0.1f);
	}
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_GAME_LEVEL, lLevelNumber);
	return lLevelNumber;
}



Cure::RuntimeVariableScope* BoundManager::GetVariableScope() const
{
	return (Parent::GetVariableScope());
}



bool BoundManager::InitializeUniverse()
{
	// Create dummy explosion to ensure all geometries loaded and ready, to avoid LAAAG when first exploading.
	UiTbc::ParticleRenderer* lParticleRenderer = (UiTbc::ParticleRenderer*)mUiManager->GetRenderer()->GetDynamicRenderer(_T("particle"));
	const Vector3DF v;
	lParticleRenderer->CreateExplosion(Vector3DF(0,0,-2000), 1, v, 1, 1, v, v, v, v, v, 1, 1, 1, 1);

	mCutsLeft = 25;
	mShakesLeft = 2;
	mLevel = (Level*)Parent::CreateContextObject(_T("level"), Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED, 0);
	StepLevel(0);
	mSunlight = new Sunlight(mUiManager);
	return true;
}

void BoundManager::TickInput()
{
	TickNetworkInput();
	TickUiInput();
}

void BoundManager::TickUiInput()
{
	mUiManager->GetInputManager()->SetCursorVisible(true);

	const int lPhysicsStepCount = GetTimeManager()->GetAffordedPhysicsStepCount();
	if (lPhysicsStepCount > 0 && mAllowMovementInput)
	{
	}
}

void BoundManager::TickUiUpdate()
{
	((BoundConsoleManager*)GetConsoleManager())->GetUiConsole()->Tick();
}

void BoundManager::SetLocalRender(bool pRender)
{
	(void)pRender;
}



void BoundManager::CreateBall(int pIndex, const Vector3DF* pPosition)
{
	Vector3DF lPosition;
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
	if ((int)mBalls.size() > pIndex)
	{
		Cure::ContextObject* lBall = GetContext()->GetObject(mBalls[pIndex]);
		if (lBall)
		{
			GetPhysicsManager()->SetBodyPosition(lBall->GetPhysics()->GetBoneGeometry(0)->GetBodyId(), lPosition);
		}
		return;
	}
	Cure::ContextObject* lBall = Parent::CreateContextObject(_T("soccerball"), Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED, 0);
	lBall->SetRootPosition(lPosition);
	lBall->SetRootVelocity(RNDVEC(1.0f));
	lBall->StartLoading();
	mBalls.push_back(lBall->GetInstanceId());
}

Cure::ContextObject* BoundManager::CreateContextObject(const str& pClassId) const
{
	UiCure::Machine* lObject = 0;
	if (pClassId == _T("level"))
	{
		lObject = new Level(GetResourceManager(), pClassId, mUiManager);
	}
	else
	{
		lObject = new Ball(GetResourceManager(), pClassId, mUiManager);
	}
	lObject->SetAllowNetworkLogic(true);
	return (lObject);
}

/*Cure::ContextObject* BoundManager::CreateLogicHandler(const str& pType)
{
}*/

void BoundManager::OnLoadCompleted(Cure::ContextObject* pObject, bool pOk)
{
	if (pOk)
	{
		/*if (pObject->GetClassId() == _T("soccerball")))
		{
		}*/
	}
	else
	{
		mLog.Errorf(_T("Could not load object of type %s."), pObject->GetClassId().c_str());
		GetContext()->PostKillObject(pObject->GetInstanceId());
	}
}

void BoundManager::OnCollision(const Vector3DF& pForce, const Vector3DF& pTorque, const Vector3DF& pPosition,
	Cure::ContextObject* pObject1, Cure::ContextObject* pObject2,
	TBC::PhysicsManager::BodyID pBody1Id, TBC::PhysicsManager::BodyID pBody2Id)
{
	(void)pBody2Id;
	mCollisionSoundManager->OnCollision(pForce, pTorque, pPosition, pObject1, pObject2, pBody1Id, 5000, false);
}



void BoundManager::ShowInstruction()
{
	mPauseButton->SetVisible(false);

	UiTbc::Dialog* d = mMenu->CreateTbcDialog(Life::Menu::ButtonAction(this, &BoundManager::OnMenuAlternative), 0.8f, 0.6f);
	d->SetColor(BG_COLOR, OFF_BLACK, BLACK, BLACK);
	d->SetDirection(+1, false);
	UiTbc::FixedLayouter lLayouter(d);

	UiTbc::Label* lLabel1 = new UiTbc::Label(BRIGHT_TEXT, _T("Swipe to cut the box. Avoid hitting the"));
	lLayouter.AddComponent(lLabel1, 0, 6, 0, 1, 1);
	UiTbc::Label* lLabel2 = new UiTbc::Label(BRIGHT_TEXT, _T("balls. Cut away 85% to complete level."));
	lLayouter.AddComponent(lLabel2, 1, 6, 0, 1, 1);

	UiTbc::Button* lResetLevelButton = new UiTbc::Button(GREEN_BUTTON, _T("OK"));
	lLayouter.AddButton(lResetLevelButton, -9, 2, 3, 0, 1, 1, true);

	CURE_RTVAR_SET(GetVariableScope(), RTVAR_PHYSICS_HALT, true);
}

void BoundManager::OnPauseButton(UiTbc::Button* pButton)
{
	if (pButton)
	{
		mMenu->OnTapSound(pButton);
	}
	mPauseButton->SetVisible(false);

	bool lRunAds;
	CURE_RTVAR_GET(lRunAds, =, GetVariableScope(), RTVAR_GAME_RUNADS, true);
	bool lDidBuy = !lRunAds;
	UiTbc::Dialog* d = mMenu->CreateTbcDialog(Life::Menu::ButtonAction(this, &BoundManager::OnMenuAlternative), 0.6f, lDidBuy? 0.6f : 0.7f);
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

	CURE_RTVAR_SET(GetVariableScope(), RTVAR_PHYSICS_HALT, true);
}

void BoundManager::OnMenuAlternative(UiTbc::Button* pButton)
{
	int lLevel;
	CURE_RTVAR_GET(lLevel, =, GetVariableScope(), RTVAR_GAME_LEVEL, 0);
	switch (pButton->GetTag())
	{
		case -1:	GetConsoleManager()->PushYieldCommand(_T("step-level +1"));				break;
		case -2:	GetConsoleManager()->PushYieldCommand(_T("step-level -1"));				break;
		case -3:	GetConsoleManager()->PushYieldCommand(_T("step-level 0"));				break;
		case -4:	GetConsoleManager()->PushYieldCommand(strutil::Format(_T("step-level %i"), -lLevel));	break;
		case -5:	Bound__Buy();										break;
	}
	mPauseButton->SetVisible(true);
	HiResTimer::StepCounterShadow();
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_PHYSICS_HALT, false);
}



void BoundManager::ScriptPhysicsTick()
{
	// Camera moves in a "moving average" kinda curve (halfs the distance in x seconds).
	const float lPhysicsTime = GetTimeManager()->GetAffordedPhysicsTotalTime();
	if (lPhysicsTime > 1e-5)
	{
		bool lIsFirstTime;
		CURE_RTVAR_GET(lIsFirstTime, =, GetVariableScope(), RTVAR_GAME_FIRSTTIME, true);
		if (lIsFirstTime)
		{
			ShowInstruction();
			CURE_RTVAR_SET(GetVariableScope(), RTVAR_GAME_FIRSTTIME, false);
		}
		if (!mCutVertices.empty() || !mCutWindowVertices.empty() || mForceCutWindow)
		{
			TimeLogger lTimeLogger(&mLog, _T("mLevel->SetTriangles"));
			if (mLastCutMode == CUT_ADD_WINDOW)
			{
				mLevel->AddCutPlane(GetPhysicsManager(), mLastCutPlane, mCutWindowVertices, Color(90, 100, 210, 180));
			}
			else
			{
				float lPreVolume = mLevel->GetVolumePart();
				if (!mCutVertices.empty())
				{
					mLevel->SetTriangles(GetPhysicsManager(), mCutVertices, mCutColors);
				}
				if (!mCutWindowVertices.empty() || mForceCutWindow)
				{
					mLevel->SetWindowTriangles(mCutWindowVertices);
				}
				float lPostVolume = mLevel->GetVolumePart();
				float lAmount = std::max(0.0f, lPreVolume-lPostVolume);
				mLevelScore += 1000 * lAmount * (1 + mQuickCutCount*0.8f);
			}
			mCutVertices.clear();
			mCutWindowVertices.clear();
			mCutColors.clear();
			mForceCutWindow = false;
		}
		mPercentDone = 100-mLevel->GetVolumePart()*100;
		if (LEVEL_DONE() && !mMenu->GetDialog())
		{
			DidFinishLevel();
		}
		else if (mCutsLeft <= 0 && !mMenu->GetDialog())
		{
			OnPauseButton(0);
		}
		Vector3DF lAcceleration = mUiManager->GetAccelerometer();
		float lAccelerationLength = lAcceleration.GetLength();
		mIsShaking = (lAccelerationLength > 1.6f);
		bool lIsReallyShaking = false;
		double lShakeTime = mShakeTimer.QuerySplitTime();
		if (lShakeTime > 6.0)
		{
			mIsShaking = false;
			mShakeTimer.Stop();
			mShakeTimer.PopTimeDiff();
		}
		else if (lShakeTime > 2.5)
		{
			// Intermission.
			mIsShaking = true;
			delete mShakeSound;
			mShakeSound = 0;
		}
		else if (mIsShaking && mShakesLeft > 0)
		{
			if (lAccelerationLength > 3)
			{
				lAcceleration.Normalize(3);
			}
			lAcceleration = mCameraTransform.GetOrientation() * lAcceleration;
			if (!mShakeTimer.IsStarted())
			{
				mShakeTimer.Start();
				delete mShakeSound;
				UiCure::UserSound2dResource* lBreakSound = new UiCure::UserSound2dResource(mUiManager, UiLepra::SoundManager::LOOP_FORWARD);
				mShakeSound = new UiCure::SoundReleaser(GetResourceManager(), mUiManager, GetContext(), _T("shake.wav"), lBreakSound, 0.5f, 1.0);
				--mShakesLeft;
			}
			lIsReallyShaking = true;
		}
		else if (mShakeTimer.IsStarted())
		{
			mIsShaking = true;
			if (lAccelerationLength > 1.2f)
			{
				// Even though the user isn't having high Gs right now, we still follow the user acceleration a bit.
				lAcceleration = mCameraTransform.GetOrientation() * lAcceleration;
				lIsReallyShaking = true;
			}
		}
		Vector3DF lCenter;
		std::vector<Cure::GameObjectId>::iterator x;
		for (x = mBalls.begin(); x != mBalls.end(); ++x)
		{
			Cure::ContextObject* lBall = GetContext()->GetObject(*x);
			if (!lBall)
			{
				continue;
			}
			Vector3DF p = lBall->GetPosition();
			lCenter += p;
			lBall->SetInitialTransform(TransformationF(QuaternionF(), p));

			if (lIsReallyShaking)
			{
				Vector3DF v = lBall->GetVelocity();
				v += lAcceleration*3.0f;
				GetPhysicsManager()->SetBodyVelocity(lBall->GetPhysics()->GetBoneGeometry(0)->GetBodyId(), v);
			}
		}
		int lLevel;
		CURE_RTVAR_GET(lLevel, =, GetVariableScope(), RTVAR_GAME_LEVEL, 0);
		if ((int)mBalls.size() < lLevel+1)
		{
			lCenter /= (float)mBalls.size();
			CreateBall(mBalls.size(), &lCenter);
		}
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

void BoundManager::HandleWorldBoundaries()
{
	std::vector<Cure::GameObjectId> lLostObjectArray;
	typedef Cure::ContextManager::ContextObjectTable ContextTable;
	const ContextTable& lObjectTable = GetContext()->GetObjectTable();
	ContextTable::const_iterator x = lObjectTable.begin();
	for (; x != lObjectTable.end(); ++x)
	{
		Cure::ContextObject* lObject = x->second;
		if (lObject->IsLoaded() && lObject->GetPhysics())
		{
			const Vector3DF lPosition = lObject->GetPosition();
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
		std::vector<Cure::GameObjectId>::const_iterator y = lLostObjectArray.begin();
		for (; y != lLostObjectArray.end(); ++y)
		{
			DeleteContextObject(*y);
			std::vector<Cure::GameObjectId>::iterator x;
			for (x = mBalls.begin(); x != mBalls.end(); ++x)
			{
				if (*x == *y)
				{
					mBalls.erase(x);
					break;
				}
			}
		}
	}
}

void BoundManager::MoveCamera(float pFrameTime)
{
	if (mIsCutting || mIsShaking)
	{
		return;
	}

	int lLevel;
	CURE_RTVAR_GET(lLevel, =, GetVariableScope(), RTVAR_GAME_LEVEL, 0);
	float lBaseSpeed = 0.15f + lLevel * 0.01f;
	mCameraAngle += lBaseSpeed*mCameraRotateSpeed*pFrameTime;
	mCameraRotateSpeed = Math::Lerp(mCameraRotateSpeed, (mCameraRotateSpeed < 0)? -1.0f : 1.0f, 0.07f);
	if (mCameraAngle > 2*PIF)
	{
		mCameraAngle -= 2*PIF;
	}
	QuaternionF q(0, Vector3DF(0,1,0));
	Vector3DF p(0,-CAM_DISTANCE,0);
	mCameraTransform = TransformationF(q, p);
	mCameraTransform.RotateAroundAnchor(Vector3DF(), Vector3DF(0,0,1), mCameraAngle);
	mCameraTransform.RotatePitch(-sin(mCameraAngle*2)*0.3f);
	mCameraTransform.MoveUp(sin(mCameraAngle*2)*1.5f);
}

void BoundManager::UpdateCameraPosition(bool pUpdateMicPosition)
{
	mUiManager->SetCameraPosition(mCameraTransform);
	if (pUpdateMicPosition)
	{
		mUiManager->SetMicrophonePosition(mCameraTransform, Vector3DF());
	}
}



void BoundManager::PrintText(const str& s, int x, int y) const
{
	Color lOldColor = mUiManager->GetPainter()->GetColor(0);
	mUiManager->GetPainter()->SetColor(DARK_BLUE, 0);
	mUiManager->GetPainter()->PrintText(s, x, y+1);
	mUiManager->GetPainter()->SetColor(lOldColor, 0);
	mUiManager->GetPainter()->PrintText(s, x, y);
}

void BoundManager::DrawImage(UiTbc::Painter::ImageID pImageId, float cx, float cy, float w, float h, float pAngle) const
{
	cx -= 0.5f;

	const float ca = ::cos(pAngle);
	const float sa = ::sin(pAngle);
	const float w2 = w*0.5f;
	const float h2 = h*0.5f;
	const float x = cx - w2*ca - h2*sa;
	const float y = cy - h2*ca + w2*sa;
	const Vector2DF c[] = { Vector2DF(x, y), Vector2DF(x+w*ca, y-w*sa), Vector2DF(x+w*ca+h*sa, y+h*ca-w*sa), Vector2DF(x+h*sa, y+h*ca) };
	const Vector2DF t[] = { Vector2DF(0, 0), Vector2DF(1, 0), Vector2DF(1, 1), Vector2DF(0, 1) };
#define V(z) std::vector<Vector2DF>(z, z+LEPRA_ARRAY_COUNT(z))
	mUiManager->GetPainter()->DrawImageFan(pImageId, V(c), V(t));
}



void BoundManager::PainterImageLoadCallback(UiCure::UserPainterKeepImageResource* pResource)
{
	if (pResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		mUiManager->GetDesktopWindow()->GetImageManager()->AddLoadedImage(*pResource->GetRamData(), pResource->GetData(),
			UiTbc::GUIImageManager::CENTERED, UiTbc::GUIImageManager::ALPHABLEND, 255);
	}
}



LOG_CLASS_DEFINE(GAME, BoundManager);



}
