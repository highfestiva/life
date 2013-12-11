
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "BoundManager.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/TimeManager.h"
#include "../Lepra/Include/Plane.h"
#include "../UiCure/Include/UiCollisionSoundManager.h"
#include "../UiCure/Include/UiIconButton.h"
#include "../UiCure/Include/UiMachine.h"
#include "../UiLepra/Include/UiTouchDrag.h"
#include "../UiTBC/Include/GUI/UiDesktopWindow.h"
#include "../UiTBC/Include/GUI/UiFixedLayouter.h"
#include "../UiTBC/Include/UiMaterial.h"
#include "../UiTBC/Include/UiParticleRenderer.h"
#include "../UiTBC/Include/UiTriangleBasedGeometry.h"
#include "../Lepra/Include/Random.h"
#include "../Life/LifeClient/UiConsole.h"
#include "Ball.h"
#include "BoundConsoleManager.h"
#include "Level.h"
#include "RtVar.h"
#include "Sunlight.h"
#include "Version.h"

#define BG_COLOR	Color(110, 110, 110, 160)
#define CAM_DISTANCE	6.5f
#define BALL_RADIUS	0.111f



namespace Bound
{



BoundManager::BoundManager(Life::GameClientMasterTicker* pMaster, const Cure::TimeManager* pTime,
	Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager,
	UiCure::GameUiManager* pUiManager, int pSlaveIndex, const PixelRect& pRenderArea):
	Parent(pMaster, pTime, pVariableScope, pResourceManager, pUiManager, pSlaveIndex, pRenderArea),
	mCollisionSoundManager(0),
	mLevel(0),
	mMenu(0),
	mSunlight(0),
	mCameraAngle(0),
	mCameraRotateSpeed(1.0f),
	mCameraTransform(QuaternionF(), Vector3DF(0, -CAM_DISTANCE, 0)),
	mLevelCompleted(false),
	mPauseButton(0),
	mIsCutting(false)
{
	mCollisionSoundManager = new UiCure::CollisionSoundManager(this, pUiManager);
	mCollisionSoundManager->AddSound(_T("explosion"),	UiCure::CollisionSoundManager::SoundResourceInfo(0.8f, 0.4f, 0));
	mCollisionSoundManager->AddSound(_T("rubber"),		UiCure::CollisionSoundManager::SoundResourceInfo(1.0f, 0.5f, 0));
	mCollisionSoundManager->PreLoadSound(_T("explosion"));

	SetConsoleManager(new BoundConsoleManager(GetResourceManager(), this, mUiManager, GetVariableScope(), mRenderArea));

	GetPhysicsManager()->SetSimulationParameters(0.0f, 0.03f, 0.2f);

	CURE_RTVAR_SET(GetVariableScope(), RTVAR_GAME_LEVEL, 0);
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_UI_SOUND_MASTERVOLUME, 1.0);
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_PHYSICS_RTR_OFFSET, 0.0);
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
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_UI_3D_FOV, 60.0);
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_PHYSICS_NOCLIP, false);

	GetConsoleManager()->ExecuteCommand(_T("bind-key F5 prev-level"));
	GetConsoleManager()->ExecuteCommand(_T("bind-key F6 next-level"));
}

void BoundManager::SaveSettings()
{
}

bool BoundManager::Open()
{
	bool lOk = Parent::Open();
	if (lOk)
	{
		mPauseButton = ICONBTNA("btn_pause.png", "");
		int x = mRenderArea.mLeft + 12;
		int y = mRenderArea.mTop + 12;
		mUiManager->GetDesktopWindow()->AddChild(mPauseButton, x, y);
		mPauseButton->SetVisible(true);
		mPauseButton->SetOnClick(BoundManager, OnPauseButton);
	}
	if (lOk)
	{
		mMenu = new Life::Menu(mUiManager, GetResourceManager());
		mMenu->SetButtonTapSound(_T("tap.wav"), 0.3f);
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
	mSunlight->Tick(mCameraTransform.GetOrientation());

	mUiManager->GetPainter()->PrePaint(false);
	mUiManager->GetPainter()->ResetClippingRect();
	mUiManager->GetRenderer()->SetDepthWriteEnabled(false);
	mUiManager->GetRenderer()->SetDepthTestEnabled(false);
	mUiManager->GetRenderer()->SetLightsEnabled(false);
	mUiManager->GetRenderer()->SetTexturingEnabled(false);
	mUiManager->GetPainter()->SetColor(Color(45,68,129), 0);
	mUiManager->GetPainter()->SetColor(Color(49,83,163), 1);
	mUiManager->GetPainter()->SetColor(Color(15,25,43), 2);
	mUiManager->GetPainter()->SetColor(Color(15,39,57), 3);
	mUiManager->GetPainter()->FillShadedRect(0, 0, mUiManager->GetCanvas()->GetWidth(), mUiManager->GetCanvas()->GetHeight());
	mUiManager->GetRenderer()->SetDepthWriteEnabled(true);
	mUiManager->GetRenderer()->SetDepthTestEnabled(true);
	mUiManager->GetRenderer()->SetLightsEnabled(true);
	mUiManager->GetRenderer()->Renderer::SetTexturingEnabled(true);

	return Parent::Render();
}

bool BoundManager::Paint()
{
	if (!Parent::Paint())
	{
		return false;
	}

	// Draw frame.
	const int w = mUiManager->GetCanvas()->GetWidth();
	const int h = mUiManager->GetCanvas()->GetHeight();
	const float lTouchSideScale = 1.28f;	// Inches.
	const float lTouchScale = lTouchSideScale / (float)mUiManager->GetDisplayManager()->GetPhysicalScreenSize();
	const int m = (int)(lTouchScale * w * 0.25f);
	mUiManager->GetPainter()->SetColor(Color(140,30,20), 0);
	mUiManager->GetPainter()->DrawLine(m*3, m, w-m*3, m);
	mUiManager->GetPainter()->DrawLine(m, m*3, m, h-m*3);
	mUiManager->GetPainter()->DrawLine(m*3, h-m, w-m*3, h-m);
	mUiManager->GetPainter()->DrawLine(w-m, m*3, w-m, h-m*3);

	HandleCutting(m, w, h);
	return true;
}

void BoundManager::HandleCutting(int m, int w, int h)
{
	mIsCutting = false;
	const int r = m-2;
	const int d = r*2;
	typedef UiLepra::Touch::DragManager::DragList DragList;
	DragList& lDragList = mUiManager->GetDragManager()->GetDragList();
	for (DragList::iterator x = lDragList.begin(); x != lDragList.end(); ++x)
	{
		PixelCoord lFrom = x->mStart;
		if (!AttachTouchToBorder(lFrom, m, w, h))
		{
			continue;
		}
		mIsCutting = true;
		mUiManager->GetPainter()->SetColor(Color(140,30,20), 0);
		mUiManager->GetPainter()->DrawArc(lFrom.x-r, lFrom.y-r, d, d, 0, 360, false);

		PixelCoord lTo = x->mLast;
		bool lDidFindTargetBorder = AttachTouchToBorder(lTo, m, w, h);
		mUiManager->GetPainter()->SetColor(Color(30,140,20), 0);
		mUiManager->GetPainter()->DrawArc(lTo.x-r, lTo.y-r, d, d, 0, 360, false);

		lDidFindTargetBorder &= (std::abs(lFrom.x-lTo.x) >= d*4 || std::abs(lFrom.y-lTo.y) >= d*4);
		if (!lDidFindTargetBorder)
		{
			mUiManager->GetPainter()->SetColor(Color(140,30,20), 0);
		}
		mUiManager->GetPainter()->DrawLine(lFrom.x, lFrom.y, lTo.x, lTo.y);

		// The plane goes through the camera and the projected midpoint of the line.
		PixelCoord lScreenMid((lFrom.x+lTo.x)/2, (lFrom.y+lTo.y)/2);
		Plane lCutPlaneDelimiter;
		const Plane lCutPlane = ScreenLineToPlane(lScreenMid, lTo, lCutPlaneDelimiter);

		if (lCutPlane.n.GetLengthSquared() > 0 && CheckBallsPlaneCollition(lCutPlane, lCutPlaneDelimiter))
		{
			// Set to middle of screen = invalidate swipe.
			x->mStart.x = w/2;
			x->mStart.y = h/2;
		}
		else if (lDidFindTargetBorder && !x->mIsPress)
		{
			Cut(lCutPlane);
			mCameraRotateSpeed = (lScreenMid.x < (int)w/2)? +1.0f : -1.0f;
		}
	}
}

Plane BoundManager::ScreenLineToPlane(PixelCoord& pCoord, PixelCoord& pEndPoint, Plane& pCutPlaneDelimiter)
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

void BoundManager::Cut(Plane pCutPlane)
{
	/*{
		// Debug render plane.
		Vector3DF lDir = pPosition - mCameraTransform.GetPosition();
		Vector3DF lTangent = lDir.Cross(pNormal);
		lTangent.Normalize(3.0f);
		Vector3DF lBitangent = lDir.GetNormalized(3.0f);
		Vector3DF v[4];
		v[0] = pPosition + -lBitangent + -lTangent;
		v[1] = pPosition +  lBitangent + -lTangent;
		v[2] = pPosition +  lBitangent +  lTangent;
		v[3] = pPosition + -lBitangent +  lTangent;
		uint32 lIndices[] = { 0, 1, 3, 2 };
		UiTbc::TriangleBasedGeometry* lCutPlane = new UiTbc::TriangleBasedGeometry(v, 0, 0, 0, UiTbc::TriangleBasedGeometry::COLOR_RGBA, lIndices,
			4, 4, TBC::GeometryBase::TRIANGLE_STRIP, TBC::GeometryBase::GEOM_STATIC);
		lCutPlane->SetAlwaysVisible(true);
		mUiManager->GetRenderer()->AddGeometry(lCutPlane, UiTbc::Renderer::MAT_SINGLE_COLOR_BLENDED, UiTbc::Renderer::FORCE_NO_SHADOWS);
	}*/

	const int lSide = CheckIfPlaneSlicesBetweenBalls(pCutPlane);
	if (lSide == 0)	// 0 == Both sides.
	{
		ExplodeBalls();
		return;
	}
	if (lSide < 0)
	{
		pCutPlane = -pCutPlane;
	}
	// Plane normal now "points" toward vertices that says. Those on the other side gets cut off. The new triangles will use this normal.
	const QuaternionF q = mCameraTransform.GetOrientation();
	const int tc = mLevel->GetMesh()->GetVertexCount() / 3;
	const float* v = mLevel->GetMesh()->GetVertexData();
	const uint8* c = mLevel->GetMesh()->GetColorData();
	mCutVertices.reserve(tc*2*3*3);
	mCutColors.reserve(tc*2*3*4);
	mCutVertices.clear();
	mCutColors.clear();
	std::vector<Vector3DF> lNGon;
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
			// All vertices on staying side of mesh. No cut, only copy.
			mCutVertices.insert(mCutVertices.end(), &v[x*9], &v[x*9+9]);
			mCutColors.insert(mCutColors.end(), &c[x*12], &c[x*12+12]);
		}
		else if (d0 <= 0 && d1 <= 0 && d2 <= 0)
		{
			// The whole triangle got cut off - way to go! No cut, only discard.
		}
		else
		{
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
				AddTriangle(p0, p1, p3, &c[x*12]);
				AddTriangle(p0, p3, p4, &c[x*12]);
			}
			else if (d1 > 0 && d2 > 0)
			{
				// Quad cut.
				const float t3 = -d2 / (pCutPlane.n*d20);
				const float t4 = -d1 / (pCutPlane.n*d01);
				Vector3DF p3 = p2+t3*d20;
				Vector3DF p4 = p1+t4*d01;
				AddTriangle(p1, p2, p3, &c[x*12]);
				AddTriangle(p1, p3, p4, &c[x*12]);
			}
			else if (d0 > 0 && d2 > 0)
			{
				// Quad cut.
				const float t3 = -d0 / (pCutPlane.n*d01);
				const float t4 = -d2 / (pCutPlane.n*d12);
				Vector3DF p3 = p0+t3*d01;
				Vector3DF p4 = p2+t4*d12;
				AddTriangle(p2, p0, p3, &c[x*12]);
				AddTriangle(p2, p3, p4, &c[x*12]);
			}
			else if (d0 > 0)
			{
				// Triangle cut.
				const float t3 = -d0 / (pCutPlane.n*d01);
				const float t4 = -d0 / (pCutPlane.n*d20);
				Vector3DF p3 = p0+t3*d01;
				Vector3DF p4 = p0+t4*d20;
				AddTriangle(p0, p3, p4, &c[x*12]);
			}
			else if (d1 > 0)
			{
				// Triangle cut.
				const float t3 = -d1 / (pCutPlane.n*d12);
				const float t4 = -d1 / (pCutPlane.n*d01);
				Vector3DF p3 = p1+t3*d12;
				Vector3DF p4 = p1+t4*d01;
				AddTriangle(p1, p3, p4, &c[x*12]);
			}
			else
			{
				// Triangle cut.
				const float t3 = -d2 / (pCutPlane.n*d20);
				const float t4 = -d2 / (pCutPlane.n*d12);
				Vector3DF p3 = p2+t3*d20;
				Vector3DF p4 = p2+t4*d12;
				AddTriangle(p2, p3, p4, &c[x*12]);
			}

			// Add points to N-gon. Note that there will always be a pair of identical twin vertices per
			// each edge (second vertex rocketed into cyberspace).
			lNGon.push_back(p0);
		}
	}
}

void BoundManager::AddTriangle(const Vector3DF& v0, const Vector3DF& v1, const Vector3DF& v2, const uint8* pColors)
{
	mCutVertices.push_back(v0.x); mCutVertices.push_back(v0.y); mCutVertices.push_back(v0.z);
	mCutVertices.push_back(v1.x); mCutVertices.push_back(v1.y); mCutVertices.push_back(v1.z);
	mCutVertices.push_back(v2.x); mCutVertices.push_back(v2.y); mCutVertices.push_back(v2.z);
	mCutColors.insert(mCutColors.end(), pColors, pColors+12);
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

bool BoundManager::CheckBallsPlaneCollition(const Plane& pCutPlane, const Plane& pCutPlaneDelimiter)
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
			// Check if the ball is on the "wrong" side of the touch endpoint of the cutting plane.
			lDistance = pCutPlaneDelimiter.GetDistance(p);
			if (lDistance >= -BALL_RADIUS)
			{
				return true;
			}
		}
	}
	return false;
}

void BoundManager::ExplodeBalls()
{
}

bool BoundManager::AttachTouchToBorder(PixelCoord& pPoint, int pMargin, int w, int h)
{
	int dt = std::abs(pPoint.y - pMargin);
	int dl = std::abs(pPoint.x - pMargin);
	int db = std::abs(pPoint.y - (h-pMargin));
	int dr = std::abs(pPoint.x - (w-pMargin));
	if (dt < dl && dt < dr && dt < db)
	{
		if (dt < pMargin*2)
		{
			pPoint.y = pMargin;
			return true;
		}
	}
	else if (db < dl && db < dr)
	{
		if (db < pMargin*2)
		{
			pPoint.y = h-pMargin;
			return true;
		}
	}
	else if (dl < dr)
	{
		if (dl < pMargin*2)
		{
			pPoint.x = pMargin;
			return true;
		}
	}
	else
	{
		if (dr < pMargin*2)
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
	/*Cure::ContextObject* lAvatar = GetContext()->GetObject(mAvatarId);
	if (lAvatar && lAvatar->GetPhysics()->GetEngineCount() >= 3)
	{
		const double lTime = mFlyTime.QuerySplitTime();
		const double lLevelBestTime = GetCurrentLevelBestTime(false);
		const bool lIsEasyMode = (GetControlMode() == 2);
		double lRtrOffset;
		CURE_RTVAR_GET(lRtrOffset, =, GetVariableScope(), RTVAR_PHYSICS_RTR_OFFSET, 0.0);
		const bool lIsToyMode = (lRtrOffset >= 0.5);
		if (lTime > 0 && (lTime < lLevelBestTime || lLevelBestTime <= 0) && !lIsEasyMode && !lIsToyMode)
		{
			SetLevelBestTime(GetCurrentLevelNumber(), false, lTime);

			str lPilotName;
			CURE_RTVAR_GET(lPilotName, =, GetVariableScope(), RTVAR_GAME_PILOTNAME, gDefaultPilotName);
			const bool lIsNonDefaultPilotName =  (lPilotName != gDefaultPilotName);
			if (lIsNonDefaultPilotName)
			{
				mHiscoreLevelIndex = GetCurrentLevelNumber();
				mMyHiscoreIndex = -1;
				mHiscoreJustUploadedTimer.Stop();
				CreateHiscoreAgent();
				const str lLevelName = strutil::Format(_T("level_%i"), GetCurrentLevelNumber());
				const int lNegativeTime = (int)(lTime*-1000);
				if (!mHiscoreAgent->StartUploadingScore(gPlatform, lLevelName, gVehicleName, lPilotName, lNegativeTime))
				{
					delete mHiscoreAgent;
					mHiscoreAgent = 0;
				}
			}
		}

		UiCure::UserSound2dResource* lFinishSound = new UiCure::UserSound2dResource(mUiManager, UiLepra::SoundManager::LOOP_NONE);
		new UiCure::SoundReleaser(GetResourceManager(), mUiManager, GetContext(), _T("finish.wav"), lFinishSound, 1.0f, 1.0f);
		mZoomPlatform = true;
		mLevelCompleted = true;
		mWinImageTimer.Start();
		return true;
	}*/
	return true;
}

int BoundManager::StepLevel(int pCount)
{
	/*if (GetContext()->GetObject(mAvatarId))
	{
		int lLevelNumber = GetCurrentLevelNumber();
		lLevelNumber += pCount;
		int lLevelCount;
		CURE_RTVAR_GET(lLevelCount, =, GetVariableScope(), RTVAR_GAME_LEVELCOUNT, 0);
		if (lLevelNumber >= lLevelCount)
		{
			lLevelNumber = 0;

			double lRtrOffset;
			CURE_RTVAR_GET(lRtrOffset, =, GetVariableScope(), RTVAR_PHYSICS_RTR_OFFSET, 0.0);
			lRtrOffset += 1;
			CURE_RTVAR_SET(GetVariableScope(), RTVAR_PHYSICS_RTR_OFFSET, lRtrOffset);
			CURE_RTVAR_SET(GetVariableScope(), RTVAR_PHYSICS_RTR, 1.0+lRtrOffset);
			CURE_RTVAR_SET(GetVariableScope(), RTVAR_GAME_ALLOWTOYMODE, true);
		}
		if (lLevelNumber < 0)
		{
			lLevelNumber = lLevelCount-1;
		}
		lLevelNumber = ORDERED_LEVELNO[lLevelNumber];
		mOldLevel = mLevel;
		mLevelCompleted = false;
		str lNewLevelName = strutil::Format(_T("level_%.2i"), lLevelNumber);
		mLevel = (Level*)Parent::CreateContextObject(lNewLevelName, Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED, 0);
		mLevel->StartLoading();
		CURE_RTVAR_SET(GetVariableScope(), RTVAR_GAME_LEVEL, lLevelNumber);
		return lNewLevelName;
	}
	return _T("");*/
	(void)pCount;
	return 0;
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
	lParticleRenderer->CreateExplosion(Vector3DF(0,0,-2000), 1, v, 1, v, v, v, v, v, 1, 1, 1, 1);

	mLevel = (Level*)Parent::CreateContextObject(_T("level"), Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED, 0);
	int lLevelIndex;
	CURE_RTVAR_GET(lLevelIndex, =, GetVariableScope(), RTVAR_GAME_LEVEL, 0);
	mLevel->GenerateLevel(GetPhysicsManager(), lLevelIndex);
	for (int x = 0; x < 4; ++x)
	{
		CreateBall(x);
	}
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



void BoundManager::CreateBall(int pIndex)
{
	Cure::ContextObject* lBall = Parent::CreateContextObject(_T("soccerball"), Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED, 0);
	int x = pIndex%3;
	int y = pIndex/3%3;
	int z = pIndex/9%3;
	lBall->SetRootPosition(Vector3DF(-0.5f+0.5f*x, -0.5f+0.5f*y, -0.5f+0.5f*z));
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



void BoundManager::OnPauseButton(UiTbc::Button* pButton)
{
	if (pButton)
	{
		mMenu->OnTapSound(pButton);
		pButton->SetVisible(false);
	}

	UiTbc::Dialog* d = mMenu->CreateTbcDialog(Life::Menu::ButtonAction(this, &BoundManager::OnMenuAlternative), 0.8f, 0.8f);
	d->SetColor(BG_COLOR, OFF_BLACK, BLACK, BLACK);
	d->SetDirection(+1, false);
	UiTbc::FixedLayouter lLayouter(d);

	UiTbc::Button* lPreviousLevelButton = new UiTbc::Button(Color(20, 110, 220), _T("Previous level"));
	lLayouter.AddButton(lPreviousLevelButton, -1, 0, 2, 0, 1, 1, true);

	UiTbc::Button* lRestartButton = new UiTbc::Button(Color(220, 110, 20), _T("1st level"));
	lLayouter.AddButton(lRestartButton, -2, 1, 2, 0, 1, 1, true);

	UiTbc::Button* lCloseButton = new UiTbc::Button(Color(180, 60, 50), _T("X"));
	lLayouter.AddCornerButton(lCloseButton, -9);

	CURE_RTVAR_SET(GetVariableScope(), RTVAR_PHYSICS_HALT, true);
}

void BoundManager::OnMenuAlternative(UiTbc::Button* pButton)
{
	if (pButton->GetTag() == -2)
	{
		GetConsoleManager()->PushYieldCommand(_T("set-level-index 0"));	// Something, something...
	}
	else if (pButton->GetTag() == -2)
	{
		GetConsoleManager()->PushYieldCommand(_T("set-level-index 0"));
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
		if (!mCutVertices.empty())
		{
			mLevel->SetTriangles(GetPhysicsManager(), mCutVertices, mCutColors);
			mCutVertices.clear();
			mCutColors.clear();
		}
		std::vector<Cure::GameObjectId>::iterator x;
		for (x = mBalls.begin(); x != mBalls.end(); ++x)
		{
			Cure::ContextObject* lBall = GetContext()->GetObject(*x);
			if (!lBall)
			{
				continue;
			}
			Vector3DF p = lBall->GetPosition();
			lBall->SetInitialTransform(TransformationF(QuaternionF(), p));
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
			if (!Math::IsInRange(lPosition.x, -1000.0f, +1000.0f) ||
				!Math::IsInRange(lPosition.y, -1000.0f, +1000.0f) ||
				!Math::IsInRange(lPosition.z, -1000.0f, +1000.0f))
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
		}
	}
}

void BoundManager::MoveCamera(float pFrameTime)
{
	if (mIsCutting)
	{
		return;
	}

	mCameraAngle += 0.1f*mCameraRotateSpeed*pFrameTime;
	if (mCameraAngle > 2*PIF)
	{
		mCameraAngle -= 2*PIF;
	}
	QuaternionF q(0, Vector3DF(0,1,0));
	Vector3DF p(0,-CAM_DISTANCE,0);
	mCameraTransform = TransformationF(q, p);
	mCameraTransform.RotateAroundAnchor(Vector3DF(), Vector3DF(0,0,1), mCameraAngle);
	mCameraTransform.RotatePitch(-sin(mCameraAngle)*0.2f);
	mCameraTransform.MoveUp(sin(mCameraAngle)*1.0f);
}

void BoundManager::UpdateCameraPosition(bool pUpdateMicPosition)
{
	mUiManager->SetCameraPosition(mCameraTransform);
	if (pUpdateMicPosition)
	{
		mUiManager->SetMicrophonePosition(mCameraTransform, Vector3DF());
	}
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
