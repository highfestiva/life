
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Life/LifeClient/GameClientSlaveManager.h"
#include "../Life/LifeClient/Menu.h"
#include "../UiCure/Include/UiResourceManager.h"
#include "Impuzzable.h"



namespace UiLepra
{
namespace Touch
{
struct Drag;
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


namespace Impuzzable
{



class Level;
class Piece;
class Sunlight;



class ImpuzzableManager: public Life::GameClientSlaveManager
{
	typedef Life::GameClientSlaveManager Parent;
public:
	typedef enum CutMode
	{
		CUT_NORMAL,
		CUT_ADD_WINDOW,
		CUT_WINDOW_ITSELF,
	};

	ImpuzzableManager(Life::GameClientMasterTicker* pMaster, const Cure::TimeManager* pTime,
		Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager,
		UiCure::GameUiManager* pUiManager, int pSlaveIndex, const PixelRect& pRenderArea);
	virtual ~ImpuzzableManager();
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

	void HandleDrag();
	Piece* PickPiece(UiLepra::Touch::Drag& pDrag, int pRadius);
	Piece* GetDraggedPiece(UiLepra::Touch::Drag& pDrag);
	void DragPiece(Piece* pPiece, const PixelCoord& pScreenPoint);
	vec3 To3dPoint(const PixelCoord& pCoord, float pDepth) const;

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

	void CreatePiece(int pIndex, const vec3* pPosition);
	virtual Cure::ContextObject* CreateContextObject(const str& pClassId) const;
	//virtual Cure::ContextObject* CreateLogicHandler(const str& pType);
	virtual void OnLoadCompleted(Cure::ContextObject* pObject, bool pOk);
	void OnCollision(const vec3& pForce, const vec3& pTorque, const vec3& pPosition,
		Cure::ContextObject* pObject1, Cure::ContextObject* pObject2,
		Tbc::PhysicsManager::BodyID pBody1Id, Tbc::PhysicsManager::BodyID pBody2Id);

	void ShowInstruction();
	void OnPauseButton(UiTbc::Button*);
	void OnMenuAlternative(UiTbc::Button* pButton);

	virtual void ScriptPhysicsTick();
	virtual void HandleWorldImpuzzablearies();
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
	std::vector<Piece*> mPieces;
	Life::Menu* mMenu;
	StopWatch mNextLevelTimer;
	Sunlight* mSunlight;
	float mCameraAngle;
	float mCameraRotateSpeed;
	xform mCameraTransform;
	float mPercentDone;
	bool mLevelCompleted;
	UiTbc::Button* mPauseButton;
	bool mIsCutting;
	bool mIsShaking;
	int mCutsLeft;
	int mShakesLeft;
	StopWatch mShakeTimer;
	CutMode mLastCutMode;
	HiResTimer mCutTimer;
	float mCutSoundPitch;
	int mQuickCutCount;
	float mLevelScore;
	UiCure::SoundReleaser* mShakeSound;
	logclass();
};



}
