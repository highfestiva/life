
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../UiCure/Include/UiCppContextObject.h"
#include "../UiTBC/Include/GUI/UiCustomButton.h"
#include "Push.h"



namespace Life
{
class ScreenPart;
}



namespace Push
{



class RoadSignButton: public UiCure::CppContextObject
{
	typedef UiCure::CppContextObject Parent;
public:
	enum Shape
	{
		SHAPE_BOX = 1,
		SHAPE_ROUND,
	};

	RoadSignButton(Life::ScreenPart* pScreenPart, Cure::ResourceManager* pResourceManager,
		UiCure::GameUiManager* pUiManager, const str& pName, const str& pClassResourceName,
		const str& pTextureResourceName, Shape pShape);
	virtual ~RoadSignButton();

	UiTbc::CustomButton& GetButton();
	void SetTrajectory(const Vector2DF& pEndPoint, float pEndDistance);
	void SetTrajectoryAngle(float pAngle);
	void SetOrientation(const QuaternionF& pOrientation);
	void SetIsMovingIn(bool pIsMovingIn);

	void MoveSign(const float pFrameTime);

protected:
	void OnTick();
	void RenderButton(UiTbc::CustomButton*);
	bool IsOverButton(UiTbc::CustomButton* pButton, int x, int y);
	virtual void LoadTextures();
	virtual void DispatchOnLoadMesh(UiCure::UserGeometryReferenceResource* pMeshResource);
	void OnLoadTexture(UiCure::UserRendererImageResource* pTextureResource);
	virtual str GetMeshInstanceId() const;

	Vector2DF Get2dProjectionPosition(const Vector3DF& p3dPosition, float& p2dRadius) const;
	Vector2DF GetAspectRatio(bool pInverse) const;

private:
	void SetInitialPosition(TBC::GeometryBase* pGeometry) const;
	float GetTargetAngle() const;

	Life::ScreenPart* mScreenPart;
	UiTbc::CustomButton mButton;
	Shape mShape;
	bool mActive;
	float mMeshRadius;
	float mAnglePart;
	float mAngleTime;
	float mAngle;
	bool mIsMovingIn;
	Vector3DF mCurrentOffset;
	float mTrajectoryEndDistance;
	Vector2DF mTrajectoryEndPoint;
	float mTrajectoryAngle;
	float mFov;
	QuaternionF mOriginalOrientation;
	QuaternionF mOrientation;
	bool mIsOriginalOrientationSet;

	LOG_CLASS_DECLARE();
};



}
