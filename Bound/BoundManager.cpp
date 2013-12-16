
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

#define BG_COLOR		Color(110, 110, 110, 160)
#define CAM_DISTANCE		7.0f
#define BALL_RADIUS		0.111f
#define DRAG_FLAG_INVALID	1
#define DRAG_FLAG_STARTED	2
#define CLOSE_NORMAL		5e-5
#define SAME_NORMAL		(1-CLOSE_NORMAL)
#define NGON_INDEX(i)		(((int)i<0)? cnt-1 : (i>=(int)cnt)? 0 : i)


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

	bool ok = Parent::Render();
	if (mLevel)
	{
		mUiManager->GetPainter()->SetLineWidth(1);
		mLevel->RenderOutline();
	}
	return ok;
}

bool BoundManager::Paint()
{
	mUiManager->GetPainter()->SetLineWidth(3);
	if (!Parent::Paint())
	{
		return false;
	}
	if (mLevel)
	{
		mPercentDone = 100-mLevel->GetVolumePercent()*100;
		const str lText = strutil::Format(_T("%.1f%% cut out"), mPercentDone);
		mUiManager->GetPainter()->SetColor(Color(12, 15, 30));
		mUiManager->GetPainter()->PrintText(lText, 100, 10);
	}
	HandleCutting();
	return true;
}

void BoundManager::HandleCutting()
{
	mIsCutting = false;
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
			if (lDragLength < 20)
			{
				lIsVeryNewDrag = true;
			}
			else if (lDragLength > 40)
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

		if (lCutPlane.n.GetLengthSquared() > 0 && !lIsVeryNewDrag && CheckBallsPlaneCollition(lCutPlane, lDragStarted? 0 : &lCutPlaneDelimiter))
		{
			// Invalidate swipe.
			x->mFlags |= 1;
		}
		else if (lDragStarted && !x->mIsPress)
		{
			if (Cut(lCutPlane))
			{
				const int lSide = CheckIfPlaneSlicesBetweenBalls(lCutPlane);
				if (lSide > 0)
				{
					lCutPlane = -lCutPlane;
				}
				float lCutX = mCameraTransform.GetOrientation().GetInverseRotatedVector(lCutPlane.n).x;
				mCameraRotateSpeed = (lCutX < 0)? +6.0f : -6.0f;
			}
		}
	}
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
	TimeLogger lTimeLogger(&mLog, _T("CheckIfPlaneSlicesBetweenBalls + prep"));
	const int lSide = CheckIfPlaneSlicesBetweenBalls(pCutPlane);
	if (lSide == 0)	// 0 == Both sides.
	{
		ExplodeBalls();
		return false;
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
	std::unordered_set<int> pNGonMap;
	bool lDidCut = false;
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
			// All vertices on staying side of mesh. No cut, only copy.
			mCutVertices.insert(mCutVertices.end(), &v[x*9], &v[x*9+9]);
			mCutColors.insert(mCutColors.end(), &c[x*12], &c[x*12+12]);
		}
		else if (d0 <= 0 && d1 <= 0 && d2 <= 0)
		{
			// The whole triangle got cut off - way to go! No cut, only discard.
			lDidCut = true;
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
				AddTriangle(p0, p1, p3, &c[x*12]);
				AddTriangle(p0, p3, p4, &c[x*12]);
				AddNGonPoints(lNGon, pNGonMap, p3, p4);
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
				AddNGonPoints(lNGon, pNGonMap, p3, p4);
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
				AddNGonPoints(lNGon, pNGonMap, p3, p4);
			}
			else if (d0 > 0)
			{
				// Triangle cut.
				const float t3 = -d0 / (pCutPlane.n*d01);
				const float t4 = -d0 / (pCutPlane.n*d20);
				Vector3DF p3 = p0+t3*d01;
				Vector3DF p4 = p0+t4*d20;
				AddTriangle(p0, p3, p4, &c[x*12]);
				AddNGonPoints(lNGon, pNGonMap, p3, p4);
			}
			else if (d1 > 0)
			{
				// Triangle cut.
				const float t3 = -d1 / (pCutPlane.n*d12);
				const float t4 = -d1 / (pCutPlane.n*d01);
				Vector3DF p3 = p1+t3*d12;
				Vector3DF p4 = p1+t4*d01;
				AddTriangle(p1, p3, p4, &c[x*12]);
				AddNGonPoints(lNGon, pNGonMap, p3, p4);
			}
			else
			{
				// Triangle cut.
				const float t3 = -d2 / (pCutPlane.n*d20);
				const float t4 = -d2 / (pCutPlane.n*d12);
				Vector3DF p3 = p2+t3*d20;
				Vector3DF p4 = p2+t4*d12;
				AddTriangle(p2, p3, p4, &c[x*12]);
				AddNGonPoints(lNGon, pNGonMap, p3, p4);
			}
		}
	}
	lTimeLogger.Transfer(_T("CreateNGon()"));
	CreateNGon(lNGon);
	if (lNGon.size() < 3 || !lDidCut)
	{
		mCutVertices.clear();
		mCutColors.clear();
		return false;
	}
	// Generate random colors and add.
	std::vector<uint8> lNGonColors;
	const size_t nvc = (lNGon.size()-2)*3;
	lNGonColors.resize(nvc*4);
	Vector3DF lNewColor(RNDPOSVEC());
	for (size_t x = 0; x < nvc; ++x)
	{
		lNGonColors[x*4+0] = (uint8)(lNewColor.x*255);
		lNGonColors[x*4+1] = (uint8)(lNewColor.y*255);
		lNGonColors[x*4+2] = (uint8)(lNewColor.z*255);
		lNGonColors[x*4+3] = 255;
	}
	lTimeLogger.Transfer(_T("AddNGonTriangles()"));
	AddNGonTriangles(pCutPlane, lNGon, &lNGonColors[0]);
	return true;
}

void BoundManager::AddTriangle(const Vector3DF& v0, const Vector3DF& v1, const Vector3DF& v2, const uint8* pColors)
{
	mCutVertices.push_back(v0.x); mCutVertices.push_back(v0.y); mCutVertices.push_back(v0.z);
	mCutVertices.push_back(v1.x); mCutVertices.push_back(v1.y); mCutVertices.push_back(v1.z);
	mCutVertices.push_back(v2.x); mCutVertices.push_back(v2.y); mCutVertices.push_back(v2.z);
	mCutColors.insert(mCutColors.end(), pColors, pColors+12);
}

void BoundManager::AddNGonPoints(std::vector<Vector3DF>& pNGon, std::unordered_set<int>& pNGonMap, const Vector3DF& p0, const Vector3DF& p1)
{
	bool lAddNGon;
	CURE_RTVAR_TRYGET(lAddNGon, =, GetVariableScope(), "AddNGon", true);
	if (!lAddNGon)
	{
		return;
	}
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

bool BoundManager::CheckBallsPlaneCollition(const Plane& pCutPlane, const Plane* pCutPlaneDelimiter)
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
				return true;
			}
			// Check if the ball is on the "wrong" side of the touch endpoint of the cutting plane.
			lDistance = pCutPlaneDelimiter->GetDistance(p);
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
	StepLevel(1);
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
	int lLevelNumber;
	CURE_RTVAR_GET(lLevelNumber, =, GetVariableScope(), RTVAR_GAME_LEVEL, 0);
	lLevelNumber += pCount;
	mLevel->GenerateLevel(GetPhysicsManager(), lLevelNumber);
	int lBallCount = lLevelNumber + 2;
	for (int x = 0; x < lBallCount; ++x)
	{
		CreateBall(x);
	}
	/*while (mBalls.size() > lBallCount)
	{
		mBalls.
	}*/
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
	lParticleRenderer->CreateExplosion(Vector3DF(0,0,-2000), 1, v, 1, v, v, v, v, v, 1, 1, 1, 1);

	mLevel = (Level*)Parent::CreateContextObject(_T("level"), Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED, 0);
	int lLevelIndex;
	CURE_RTVAR_GET(lLevelIndex, =, GetVariableScope(), RTVAR_GAME_LEVEL, 0);
	mLevel->GenerateLevel(GetPhysicsManager(), lLevelIndex);
	for (int x = 0; x < 2; ++x)
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
	if ((int)mBalls.size() > pIndex)
	{
		return;
	}
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
			TimeLogger lTimeLogger(&mLog, _T("mLevel->SetTriangles"));
			mLevel->SetTriangles(GetPhysicsManager(), mCutVertices, mCutColors);
			mCutVertices.clear();
			mCutColors.clear();
		}
		if (mPercentDone >= 70)
		{
			DidFinishLevel();
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

	mCameraAngle += 0.3f*mCameraRotateSpeed*pFrameTime;
	mCameraRotateSpeed = Math::Lerp(mCameraRotateSpeed, (mCameraRotateSpeed < 0)? -1.0f : 1.0f, 0.1f);
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
