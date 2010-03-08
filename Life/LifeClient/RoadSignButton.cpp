
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "RoadSignButton.h"
#include "../../Cure/Include/ContextManager.h"
#include "../../UiCure/Include/UiGameUiManager.h"
#include "../../UiTBC/Include/GUI/UiDesktopWindow.h"
#include "GameClientSlaveManager.h"



namespace Life
{



RoadSignButton::RoadSignButton(GameClientSlaveManager* pClient, const str& pName, const str& pMeshResourceName, Shape pShape):
	Parent(_T("RoadSignButton"), pClient->GetUiManager()),
	mClient(pClient),
	mButton(pName),
	mSignMesh(pClient->GetUiManager()),
	mShape(pShape),
	mMeshRadius(0),
	mAnglePart(0),
	mAngleTime(0),
	mAngle(0),
	mIsMovingIn(true)
{
	mButton.SetPreferredSize(5, 5);
	mButton.SetMinSize(5, 5);
	mButton.SetPos(-50, -50);
	GetUiManager()->GetDesktopWindow()->AddChild(&mButton);

	mSignMesh.Load(mClient->GetResourceManager(), pMeshResourceName,
		UiCure::UserGeometryReferenceResource::TypeLoadCallback(this, &RoadSignButton::OnLoadMesh));

	mButton.SetOnRender(RoadSignButton, RenderButton);
	mButton.SetOnIsOver(RoadSignButton, IsOverButton);
}

RoadSignButton::~RoadSignButton()
{
	GetUiManager()->GetDesktopWindow()->RemoveChild(&mButton, 0);
}



UiTbc::CustomButton& RoadSignButton::GetButton()
{
	return (mButton);
}

void RoadSignButton::SetTrajectory(float pAngle, const PixelCoord& pEndPoint, float pEndDistance, float pTime)
{
	const float lRatioX = 1.05f;
	const float lRatioY = 1.45f;
	const PixelRect& lTopViewport = mClient->GetRenderArea();
	float x = (pEndPoint.x-lTopViewport.GetCenterX()) * pEndDistance / (lRatioX * lTopViewport.GetWidth());
	float z = (pEndPoint.y-lTopViewport.GetCenterY()) * pEndDistance / (lRatioY * lTopViewport.GetHeight());
	mEndPosition = Vector3DF(x, pEndDistance, z);

	float lAnchorX = 0.9f*pEndDistance*cos(pAngle);
	float lAnchorZ = 0.9f*pEndDistance*sin(pAngle);
	mAnchor = Vector3DF(lAnchorX, 2*pEndDistance, lAnchorZ) - mEndPosition;
	mAxis = Vector3DF(-lAnchorZ, 0, lAnchorX);

	mAnglePart = 1;
	mAngleTime = pTime;
	mAngle = -PIF;

	GetManager()->EnableTickCallback(this);
}

void RoadSignButton::SetIsMovingIn(bool pIsMovingIn, float pTime)
{
	mIsMovingIn = pIsMovingIn;
	mAngleTime = pTime;
}



void RoadSignButton::OnTick(float pFrameTime)
{
	Parent::OnTick(pFrameTime);
	if (mSignMesh.GetLoadState() != Cure::RESOURCE_LOAD_COMPLETE)
	{
		return;
	}

	// Rotate "inward" until end pos reached, or outward until invisible.
	TransformationF lTransform;
	lTransform.SetPosition(mEndPosition);
	if (mIsMovingIn && mAnglePart > 0)
	{
		mAnglePart -= pFrameTime/mAngleTime;
	}
	else if (!mIsMovingIn && mAnglePart < 1)
	{
		mAnglePart += pFrameTime/mAngleTime;
	}
	mAngle = Math::Lerp(mAngle, GetTargetAngle(), 0.07f);
	lTransform.RotateAroundAnchor(mAnchor, mAxis, mAngle);
	// Button hoover yields hot road sign.
	Vector3DF lTarget;
	if (mButton.GetState() == UiTbc::Button::RELEASED_HOOVER ||
		mButton.GetState() == UiTbc::Button::PRESSING)
	{
		lTarget = mEndPosition*0.4f;
		mCurrentPosition = Math::Lerp(mCurrentPosition, lTarget, 0.2f);
	}
	else
	{
		mCurrentPosition = Math::Lerp(mCurrentPosition, lTarget, 0.02f);
	}
	lTransform.GetPosition() -= mCurrentPosition;

	mSignMesh.GetRamData()->SetTransformation(lTransform);
}

void RoadSignButton::RenderButton(UiTbc::CustomButton* pButton)
{
	if (mSignMesh.GetLoadState() != Cure::RESOURCE_LOAD_COMPLETE ||
		(mAnglePart >= 1 && ::fabs(mAngle-GetTargetAngle()) < 0.1f))
	{
		return;
	}

	TBC::GeometryBase* lGfxGeometry = mSignMesh.GetRamData();

	// Set button size and position according to 3D-geometry location on screen.
	const Vector3DF& lPosition = lGfxGeometry->GetTransformation().GetPosition();
	const float lRatioX = 1.05f;
	const float lRatioY = 1.45f;
	const PixelRect& lTopViewport = mClient->GetRenderArea();
	const int lScreenX = (int)(lPosition.x/lPosition.y * lRatioX * lTopViewport.GetWidth() + lTopViewport.GetCenterX());
	const int lScreenY = (int)(-lPosition.z/lPosition.y * lRatioY * lTopViewport.GetHeight() + lTopViewport.GetCenterY());
	const int lScreenRadius = (int)(mMeshRadius/lPosition.y * lRatioX * lTopViewport.GetWidth());
	pButton->SetPos(lScreenX-lScreenRadius, lScreenY-lScreenRadius);
	pButton->SetPreferredSize(lScreenRadius*2, lScreenRadius*2);

	GetUiManager()->GetRenderer()->RenderRelative(mSignMesh.GetRamData());
}

bool RoadSignButton::IsOverButton(UiTbc::CustomButton* pButton, int x, int y)
{
	if (mShape == SHAPE_ROUND)
	{
		PixelCoord lMiddle(pButton->GetPos() + pButton->GetSize()/2);
		const int lRadius = pButton->GetSize().x/2;
		return (lMiddle.GetDistance(PixelCoord(x, y)) <= lRadius);
	}
	return (pButton->Button::IsOver(x, y));
}

void RoadSignButton::OnLoadMesh(UiCure::UserGeometryReferenceResource* pMeshResource)
{
	if (pMeshResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		TBC::GeometryBase* lGeometry = pMeshResource->GetRamData();
		lGeometry->SetAlwaysVisible(false);
		TBC::GeometryBase::BasicMaterialSettings lMaterial(Vector3DF(1,0,0), Vector3DF(0,1,0),
			Vector3DF(0,0,1), 0.5, 1, true);
		lGeometry->SetBasicMaterialSettings(lMaterial);
		UiTbc::Renderer::MaterialType lMaterialType = UiTbc::Renderer::MAT_SINGLE_COLOR_SOLID;
		GetUiManager()->GetRenderer()->ChangeMaterial(pMeshResource->GetData(), lMaterialType);

		mMeshRadius = 0;
		Vector3DF lFar;
		const float* vtx = lGeometry->GetVertexData();
		const unsigned c = lGeometry->GetVertexCount();
		for (unsigned x = 0; x < c; ++x)
		{
			Vector3DF lVertex(vtx[x*3+0], vtx[x*3+1], vtx[x*3+2]);
			if (lVertex.GetLength() > mMeshRadius)
			{
				lFar = lVertex;
				mMeshRadius = lVertex.GetLength();
			}
		}
	}
	else
	{
		mLog.AError("Could not load road sign mesh! Shit.");
		assert(false);
	}
}



float RoadSignButton::GetTargetAngle() const
{
	const float lAngleFactor = 0.7f;
	return (-PIF*lAngleFactor*mAnglePart);
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, RoadSignButton);



}
