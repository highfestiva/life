
// Author: Jonas Bystr�m
// Copyright (c) 2002-2010, Righteous Games



#include "RoadSignButton.h"
#include "../../Cure/Include/ContextManager.h"
#include "../../UiCure/Include/UiGameUiManager.h"
#include "../../UiTBC/Include/GUI/UiDesktopWindow.h"
#include "GameClientSlaveManager.h"



namespace Life
{



static const float MIN_TIME = 0.2f;
static const float MAX_TIME = 0.9f;



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
	mIsMovingIn(true),
	mTrajectoryEndDistance(0),
	mFov(90)
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

void RoadSignButton::SetTrajectory(const Vector2DF& pEndPoint, float pEndDistance)
{
	mTrajectoryEndDistance = pEndDistance;
	mTrajectoryEndPoint = pEndPoint;

	mAnglePart = 1;
	SetIsMovingIn(true);
	mAngle = -PIF;

	GetManager()->EnableTickCallback(this);
}

void RoadSignButton::SetIsMovingIn(bool pIsMovingIn)
{
	mIsMovingIn = pIsMovingIn;
	//const float lTime = mIsMovingIn? mTrajectoryEndPoint.GetLength() : 1 - mTrajectoryEndPoint.GetLength();
	mAngleTime = Math::Lerp(MIN_TIME, MAX_TIME, mTrajectoryEndPoint.GetLength());
}



void RoadSignButton::OnTick(float pFrameTime)
{
	Parent::OnTick(pFrameTime);
	if (mSignMesh.GetLoadState() != Cure::RESOURCE_LOAD_COMPLETE)
	{
		return;
	}

	const Vector2DF lRatio = GetAspectRatio(false);
	const float x = mTrajectoryEndPoint.x * pow(lRatio.x, 0.7f) * 0.9f * mTrajectoryEndDistance;
	const float z = mTrajectoryEndPoint.y * pow(lRatio.y, 0.7f) * 0.9f * mTrajectoryEndDistance;
	const Vector3DF lEndPosition(x, mTrajectoryEndDistance, z);

	// Rotate "inward" until end pos reached, or outward until invisible.
	TransformationF lTransform;
	lTransform.SetPosition(lEndPosition);
	if (mIsMovingIn && mAnglePart > 0)
	{
		mAnglePart -= pFrameTime/mAngleTime;
	}
	else if (!mIsMovingIn && mAnglePart < 1)
	{
		mAnglePart += pFrameTime/mAngleTime;
	}
	mAnglePart = Math::Clamp(mAnglePart, 0.0f, 1.0f);
	mAngle = Math::Lerp(mAngle, GetTargetAngle(), 0.2f);

	const TBC::GeometryBase* lGfxGeometry = mSignMesh.GetRamData();
	float lScreenRadius = 0;
	const Vector2DF lScreenPosition = Get2dProjectionPosition(lGfxGeometry->GetTransformation().GetPosition(), lScreenRadius);
	const float lTrajectoryAngle = (lScreenPosition.x <= mClient->GetUiManager()->GetDisplayManager()->GetWidth()/2)? PIF : 0;
	const float lAnchorX = 0.8f*mTrajectoryEndDistance*1/lRatio.y*cos(lTrajectoryAngle);
	const float lAnchorZ = 0.8f*mTrajectoryEndDistance*1/lRatio.x*sin(lTrajectoryAngle);
	const Vector3DF lAnchor = Vector3DF(lAnchorX, 2*mTrajectoryEndDistance, lAnchorZ) - lEndPosition;
	const Vector3DF lAxis(-lAnchorZ, 0, lAnchorX);

	lTransform.RotateAroundAnchor(lAnchor, lAxis, mAngle);

	// Button hoover yields hot road sign.
	Vector3DF lTarget;
	if (mButton.GetState() == UiTbc::Button::RELEASED_HOOVER ||
		mButton.GetState() == UiTbc::Button::PRESSING)
	{
		lTarget = lEndPosition*0.4f;
		mCurrentOffset = Math::Lerp(mCurrentOffset, lTarget, 0.2f);
	}
	else
	{
		mCurrentOffset = Math::Lerp(mCurrentOffset, lTarget, 0.02f);
	}
	lTransform.GetPosition() -= mCurrentOffset;

	mSignMesh.GetRamData()->SetTransformation(lTransform);
}

void RoadSignButton::RenderButton(UiTbc::CustomButton* pButton)
{
	if (mSignMesh.GetLoadState() != Cure::RESOURCE_LOAD_COMPLETE ||
		(mAnglePart >= 1 && ::fabs(mAngle-GetTargetAngle()) < 0.1f))
	{
		return;
	}

	const TBC::GeometryBase* lGfxGeometry = mSignMesh.GetRamData();

	// Set button size and position according to 3D-geometry location on screen.
	float lScreenRadius = 0;
	const Vector2DF lScreenPosition = Get2dProjectionPosition(lGfxGeometry->GetTransformation().GetPosition(), lScreenRadius);
	pButton->SetPos((int)(lScreenPosition.x-lScreenRadius), (int)(lScreenPosition.y-lScreenRadius));
	pButton->SetPreferredSize((int)(lScreenRadius*2), (int)(lScreenRadius*2));
	//pButton->Button::Repaint(mClient->GetUiManager()->GetPainter());

	const PixelRect& lTopViewport = mClient->GetRenderArea();
	GetUiManager()->GetRenderer()->ResetClippingRect();
	GetUiManager()->GetRenderer()->SetClippingRect(lTopViewport);
	GetUiManager()->GetRenderer()->SetViewport(lTopViewport);
	mFov = mClient->UpdateFrustum();
	GetUiManager()->GetRenderer()->RenderRelative(mSignMesh.GetRamData());
	GetUiManager()->PreparePaint();
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

		const float lXFactor = (mTrajectoryEndPoint.x <= 0)? -2.0f : 2.0f;
		lGeometry->SetTransformation(TransformationF(QuaternionF(),
			Vector3DF(mTrajectoryEndDistance*lXFactor, mTrajectoryEndDistance, 0)));

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



Vector2DF RoadSignButton::Get2dProjectionPosition(const Vector3DF& p3dPosition, float& p2dRadius) const
{
	const PixelRect& lTopViewport = mClient->GetRenderArea();
	const float lAspectRatio = lTopViewport.GetWidth() / (float)lTopViewport.GetHeight();
	const float lDistance = p3dPosition.y + 0.3f;
	const float lProjectionLengthInverse = 1 / asin(mFov * PIF / 180 / 2);
	p2dRadius = mMeshRadius/lDistance * lProjectionLengthInverse * 0.5f * lTopViewport.GetWidth();
	const float lScreenX = +p3dPosition.x/lDistance * lProjectionLengthInverse * 0.5f * lTopViewport.GetWidth() + lTopViewport.GetCenterX();
	const float lScreenY = -p3dPosition.z/lDistance * lProjectionLengthInverse * lAspectRatio * 0.5f * lTopViewport.GetHeight() + lTopViewport.GetCenterY();
	return (Vector2DF(lScreenX, lScreenY));
}

Vector2DF RoadSignButton::GetAspectRatio(bool pInverse) const
{
	const PixelRect& lTopViewport = mClient->GetRenderArea();
	const float lAspectRatio = lTopViewport.GetWidth() / (float)lTopViewport.GetHeight();
	const float x = (pInverse^(lAspectRatio < 1))? lAspectRatio : 1;
	const float y = (pInverse^(lAspectRatio < 1))? 1 : 1/lAspectRatio;
	return (Vector2DF(x, y));
}



float RoadSignButton::GetTargetAngle() const
{
	const float lHingeAngleFactor = 0.7f;	// How much the sign should "hinge" *(1.0 is half a revolution).
	return (-PIF*lHingeAngleFactor*mAnglePart);
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, RoadSignButton);



}
