
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Lepra/Include/Plane.h"
#include "../Life/LifeClient/GameClientSlaveManager.h"
#include "../Life/LifeClient/Menu.h"
#include "../UiCure/Include/UiResourceManager.h"
#include "Bound.h"



namespace Lepra
{
class Plane;
}
namespace UiLepra
{
namespace Touch
{
class TouchstickInputDevice;
}
}
namespace UiCure
{
class CollisionSoundManager;
class CppContextObject;
class SoundReleaser;
}
namespace UiTbc
{
class Button;
}


namespace Bound
{



class Level;
class Sunlight;



class BoundManager: public Life::GameClientSlaveManager
{
	typedef Life::GameClientSlaveManager Parent;
public:
	typedef enum CutMode
	{
		CUT_NORMAL,
		CUT_ADD_WINDOW,
		CUT_WINDOW_ITSELF,
	};

	BoundManager(Life::GameClientMasterTicker* pMaster, const Cure::TimeManager* pTime,
		Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager,
		UiCure::GameUiManager* pUiManager, int pSlaveIndex, const PixelRect& pRenderArea);
	virtual ~BoundManager();
	virtual void Suspend();
	virtual void LoadSettings();
	virtual void SaveSettings();
	virtual bool Open();
	virtual void Close();
	virtual void SetIsQuitting();
	virtual void SetFade(float pFadeAmount);

	virtual bool Render();
	void RenderBackground();
	virtual bool Paint();

	bool HandleCutting();
	Plane ScreenLineToPlane(const PixelCoord& pCoord, const	PixelCoord& pEndPoint, Plane& pCutPlaneDelimiter);
	bool Cut(Plane pCutPlane);
	bool DoCut(const UiTbc::TriangleBasedGeometry* pMesh, Plane pCutPlane, CutMode pCutMode);
	void AddTriangle(const Vector3DF& v0, const Vector3DF& v1, const Vector3DF& v2, const uint8* pColors);
	void AddNGonPoints(std::vector<Vector3DF>& pNGon, std::unordered_set<int>& pNGonMap, const Vector3DF& p0, const Vector3DF& p1);
	static void AddNGonPoint(std::vector<Vector3DF>& pNGon, std::unordered_set<int>& pNGonMap, const Vector3DF& p);
	void CreateNGon(std::vector<Vector3DF>& pNGon);
	void LineUpNGonBorders(std::vector<Vector3DF>& pNGon, bool pSort);
	void SimplifyNGon(std::vector<Vector3DF>& pNGon);
	void AddNGonTriangles(const Plane& pCutPlane, const std::vector<Vector3DF>& pNGon, const uint8* pColors);
	int CheckIfPlaneSlicesBetweenBalls(const Plane& pCutPlane);
	bool CheckBallsPlaneCollition(const Plane& pCutPlane, const Plane* pCutPlaneDelimiter, Vector3DF& pCollisionPoint);
	static bool AttachTouchToBorder(PixelCoord& pPoint, int pMargin, int pWidth, int pHeight);

	bool SetAvatarEnginePower(unsigned pAspect, float pPower);

	virtual bool DidFinishLevel();
	virtual int StepLevel(int pCount);

	Cure::RuntimeVariableScope* GetVariableScope() const;

protected:
	typedef UiLepra::Touch::TouchstickInputDevice Touchstick;

	virtual bool InitializeUniverse();
	virtual void TickInput();
	virtual void TickUiInput();
	virtual void TickUiUpdate();
	virtual void SetLocalRender(bool pRender);

	void CreateBall(int pIndex, const Vector3DF* pPosition);
	virtual Cure::ContextObject* CreateContextObject(const str& pClassId) const;
	//virtual Cure::ContextObject* CreateLogicHandler(const str& pType);
	virtual void OnLoadCompleted(Cure::ContextObject* pObject, bool pOk);
	void OnCollision(const Vector3DF& pForce, const Vector3DF& pTorque, const Vector3DF& pPosition,
		Cure::ContextObject* pObject1, Cure::ContextObject* pObject2,
		TBC::PhysicsManager::BodyID pBody1Id, TBC::PhysicsManager::BodyID pBody2Id);

	void ShowInstruction();
	void OnPauseButton(UiTbc::Button*);
	void OnMenuAlternative(UiTbc::Button* pButton);

	virtual void ScriptPhysicsTick();
	virtual void HandleWorldBoundaries();
	virtual void MoveCamera(float pFrameTime);
	virtual void UpdateCameraPosition(bool pUpdateMicPosition);

	void PrintText(const str& s, int x, int y) const;
	void DrawImage(UiTbc::Painter::ImageID pImageId, float cx, float cy, float w, float h, float pAngle) const;

	void PainterImageLoadCallback(UiCure::UserPainterKeepImageResource* pResource);

	UiCure::CollisionSoundManager* mCollisionSoundManager;
	Level* mLevel;
	std::vector<float> mCutVertices;
	std::vector<float> mCutWindowVertices;
	std::vector<uint8> mCutColors;
	bool mForceCutWindow;
	std::vector<Cure::GameObjectId> mBalls;
	Life::Menu* mMenu;
	StopWatch mNextLevelTimer;
	Sunlight* mSunlight;
	float mCameraAngle;
	float mCameraRotateSpeed;
	TransformationF mCameraTransform;
	float mPercentDone;
	bool mLevelCompleted;
	UiTbc::Button* mPauseButton;
	bool mIsCutting;
	bool mIsShaking;
	int mCutsLeft;
	int mShakesLeft;
	StopWatch mShakeTimer;
	Plane mLastCutPlane;
	CutMode mLastCutMode;
	HiResTimer mCutTimer;
	float mCutSoundPitch;
	int mQuickCutCount;
	float mLevelScore;
	UiCure::SoundReleaser* mShakeSound;
	LOG_CLASS_DECLARE();
};



}
