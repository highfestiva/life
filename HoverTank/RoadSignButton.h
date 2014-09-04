
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../UiCure/Include/UiCppContextObject.h"
#include "../UiTbc/Include/GUI/UiCustomButton.h"
#include "HoverTank.h"



namespace Life
{
class ScreenPart;
}



namespace HoverTank
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
	void SetTrajectory(const vec2& pEndPoint, float pEndDistance);
	void SetTrajectoryAngle(float pAngle);
	void SetOrientation(const quat& pOrientation);
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

	vec2 Get2dProjectionPosition(const vec3& p3dPosition, float& p2dRadius) const;
	vec2 GetAspectRatio(bool pInverse) const;

private:
	void SetInitialPosition(Tbc::GeometryBase* pGeometry) const;
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
	vec3 mCurrentOffset;
	float mTrajectoryEndDistance;
	vec2 mTrajectoryEndPoint;
	float mTrajectoryAngle;
	float mFov;
	quat mOriginalOrientation;
	quat mOrientation;
	bool mIsOriginalOrientationSet;

	logclass();
};



}
