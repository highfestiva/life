
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../../UiCure/Include/UiCppContextObject.h"
#include "../../UiTBC/Include/GUI/UiCustomButton.h"
#include "../Life.h"



namespace Life
{



class GameClientSlaveManager;



class RoadSignButton: public UiCure::CppContextObject
{
	typedef UiCure::CppContextObject Parent;
public:
	enum Shape
	{
		SHAPE_BOX = 1,
		SHAPE_ROUND,
	};

	RoadSignButton(GameClientSlaveManager* pClient, const str& pName, const str& pMeshResourceName, Shape pShape);
	virtual ~RoadSignButton();

	UiTbc::CustomButton& GetButton();
	void SetTrajectory(const Vector2DF& pEndPoint, float pEndDistance);
	void SetIsMovingIn(bool pIsMovingIn);

protected:
	void OnTick(float pFrameTime);
	void RenderButton(UiTbc::CustomButton* pButton);
	bool IsOverButton(UiTbc::CustomButton* pButton, int x, int y);
	void OnLoadMesh(UiCure::UserGeometryReferenceResource* pMeshResource);

	Vector2DF Get2dProjectionPosition(const Vector3DF& p3dPosition, float& p2dRadius) const;
	Vector2DF GetAspectRatio(bool pInverse) const;

private:
	float GetTargetAngle() const;

	GameClientSlaveManager* mClient;
	UiTbc::CustomButton mButton;
	UiCure::UserGeometryReferenceResource mSignMesh;
	Shape mShape;
	float mMeshRadius;
	float mAnglePart;
	float mAngleTime;
	float mAngle;
	bool mIsMovingIn;
	Vector3DF mCurrentOffset;
	float mTrajectoryEndDistance;
	Vector2DF mTrajectoryEndPoint;
	float mFov;

	LOG_CLASS_DECLARE();
};



}
