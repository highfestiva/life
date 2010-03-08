
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../../Lepra/Include/Graphics2D.h"
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
	void SetTrajectory(float pAngle, const PixelCoord& pEndPoint, float pEndDistance, float pTime);
	void SetIsMovingIn(bool pIsMovingIn, float pTime);

protected:
	void OnTick(float pFrameTime);
	void RenderButton(UiTbc::CustomButton* pButton);
	bool IsOverButton(UiTbc::CustomButton* pButton, int x, int y);
	void OnLoadMesh(UiCure::UserGeometryReferenceResource* pMeshResource);

private:
	float GetTargetAngle() const;

	GameClientSlaveManager* mClient;
	UiTbc::CustomButton mButton;
	UiCure::UserGeometryReferenceResource mSignMesh;
	Shape mShape;
	bool mIsMeshMoved;
	float mMeshRadius;
	float mAnglePart;
	float mAngleTime;
	float mAngle;
	bool mIsMovingIn;
	Vector3DF mEndPosition;
	Vector3DF mCurrentPosition;
	Vector3DF mAnchor;
	Vector3DF mAxis;

	LOG_CLASS_DECLARE();
};



}
