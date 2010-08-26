
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "RoadSignButton.h"
#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Cure/Include/TimeManager.h"
#include "../../TBC/Include/ChunkyPhysics.h"
#include "../../UiCure/Include/UiGameUiManager.h"
#include "../../UiTBC/Include/GUI/UiDesktopWindow.h"
#include "../../UiTBC/Include/GUI/UiFloatingLayout.h"
#include "GameClientSlaveManager.h"



namespace Life
{



static const float MIN_TIME = 0.2f;
static const float MAX_TIME = 1.0f;



RoadSignButton::RoadSignButton(ScreenPart* pScreenPart, Cure::ResourceManager* pResourceManager,
	UiCure::GameUiManager* pUiManager, const str& pName, const str& pClassResourceName,
	const str& pTextureResourceName, Shape pShape):
	Parent(pResourceManager, pClassResourceName, pUiManager),
	mScreenPart(pScreenPart),
	mButton(pName),
	mShape(pShape),
	mActive(false),
	mMeshRadius(0),
	mAnglePart(0),
	mAngleTime(0),
	mAngle(0),
	mIsMovingIn(true),
	mTrajectoryEndDistance(0),
	mFov(90),
	mIsOriginalOrientationSet(false)
{
	mButton.SetPreferredSize(5, 5);
	mButton.SetMinSize(5, 5);
	mButton.SetPos(-50, -50);
	GetUiManager()->AssertDesktopLayout(new UiTbc::FloatingLayout, 0);
	GetUiManager()->GetDesktopWindow()->AddChild(&mButton, 0, 0, 0);

	mButton.SetOnRender(RoadSignButton, RenderButton);
	mButton.SetOnIsOver(RoadSignButton, IsOverButton);

	SetPhysicsTypeOverride(PHYSICS_OVERRIDE_BONES);
	UiCure::UserRendererImageResource* lTexture = new UiCure::UserRendererImageResource(mUiManager);
	mTextureResourceArray.push_back(lTexture);
	lTexture->Load(GetResourceManager(), pTextureResourceName,
		UiCure::UserRendererImageResource::TypeLoadCallback(this, &RoadSignButton::OnLoadTexture));
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
	if (!mMeshResourceArray.empty() && mMeshResourceArray[0] && mMeshResourceArray[0]->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		SetInitialPosition(mMeshResourceArray[0]->GetRamData());
	}

	mAnglePart = 1;
	SetIsMovingIn(true);
	mAngle = -PIF;
}

void RoadSignButton::SetOrientation(const QuaternionF& pOrientation)
{
	mOrientation = pOrientation;
}

void RoadSignButton::SetIsMovingIn(bool pIsMovingIn)
{
	if (pIsMovingIn)
	{
		mActive = true;
	}
	mIsMovingIn = pIsMovingIn;
	//const float lTime = mIsMovingIn? mTrajectoryEndPoint.GetLength() : 1 - mTrajectoryEndPoint.GetLength();
	mAngleTime = Math::Lerp(MIN_TIME, MAX_TIME, mTrajectoryEndPoint.GetLength());
}

void RoadSignButton::MoveSign(const float pFrameTime)
{
	if (!mActive)
	{
		return;
	}
	if (mPhysics == 0)
	{
		return;
	}
	if (!mIsOriginalOrientationSet)
	{
		mIsOriginalOrientationSet = true;
		mOriginalOrientation = mPhysics->GetBoneTransformation(mPhysics->GetRootBone()).GetOrientation();
		mOriginalOrientation *= mOrientation;
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

	UiCure::UserGeometryReferenceResource* lResource = mMeshResourceArray[0];
	if (lResource->GetLoadState() != Cure::RESOURCE_LOAD_COMPLETE)
	{
		return;
	}
	TBC::GeometryReference* lGfxGeometry = (TBC::GeometryReference*)lResource->GetRamData();
	const Vector3DF& lPosition = lGfxGeometry->GetTransformation().GetPosition();
	float lScreenRadius = 0;
	const Vector2DF lScreenPosition = Get2dProjectionPosition(lPosition, lScreenRadius);
	const float lTrajectoryAngle = (lScreenPosition.x <= GetUiManager()->GetDisplayManager()->GetWidth()/2)? PIF : 0;
	const float lAnchorX = 0.8f*mTrajectoryEndDistance*1/lRatio.y*cos(lTrajectoryAngle);
	const float lAnchorZ = 0.8f*mTrajectoryEndDistance*1/lRatio.x*sin(lTrajectoryAngle);
	const Vector3DF lAnchor = Vector3DF(lAnchorX, 2*mTrajectoryEndDistance, lAnchorZ) - lEndPosition;
	const Vector3DF lAxis(-lAnchorZ, 0, lAnchorX);

	// Set button size and position according to 3D-geometry location on screen.
	if (mShape == SHAPE_BOX)
	{
		lScreenRadius = lScreenRadius / 1.35f;
	}
	mButton.SetPos((int)(lScreenPosition.x-lScreenRadius), (int)(lScreenPosition.y-lScreenRadius));
	mButton.SetSize((int)(lScreenRadius*2), (int)(lScreenRadius*2));
	mButton.SetPreferredSize((int)(lScreenRadius*2), (int)(lScreenRadius*2));

	lTransform.RotateAroundAnchor(lAnchor, lAxis, mAngle);
	lTransform.GetOrientation() *= mOriginalOrientation;

	// Button hoover yields hot road sign.
	Vector3DF lTargetOffset;
	if (mButton.GetState() == UiTbc::Button::RELEASED_HOOVER ||
		mButton.GetState() == UiTbc::Button::PRESSING)
	{
		lTargetOffset = lEndPosition*0.4f;
		mCurrentOffset = Math::Lerp(mCurrentOffset, lTargetOffset, Math::GetIterateLerpTime(1.2f, pFrameTime));
	}
	else
	{
		mCurrentOffset = Math::Lerp(mCurrentOffset, lTargetOffset, Math::GetIterateLerpTime(0.8f, pFrameTime));
	}
	lTransform.GetPosition() -= mCurrentOffset;

	for (size_t x = 0; x < mMeshResourceArray.size(); ++x)
	{
		UiCure::UserGeometryReferenceResource* lResource = mMeshResourceArray[x];
		if (lResource->GetLoadState() != Cure::RESOURCE_LOAD_COMPLETE)
		{
			continue;
		}

		TBC::GeometryBase* lGfxGeometry = lResource->GetRamData();
		lGfxGeometry->SetTransformation(lTransform);
	}
}

void RoadSignButton::OnPhysicsTick()
{
	if (!GetManager())
	{
		return;
	}

	const float lFrameTime = std::min(0.1f, GetManager()->GetGameManager()->GetTimeManager()->GetRealNormalFrameTime());
	MoveSign(lFrameTime);
}



void RoadSignButton::RenderButton(UiTbc::CustomButton*)
{
	//pButton->Button::Repaint(GetUiManager()->GetPainter());

	if (mAnglePart >= 1 && ::fabs(mAngle-GetTargetAngle()) < 0.1f)
	{
		mActive = false;
		return;
	}

	// Render all meshes that constitute the road sign.
	for (size_t x = 0; x < mMeshResourceArray.size(); ++x)
	{
		UiCure::UserGeometryReferenceResource* lResource = mMeshResourceArray[x];
		if (lResource->GetLoadState() != Cure::RESOURCE_LOAD_COMPLETE)
		{
			continue;
		}

		TBC::GeometryBase* lGfxGeometry = lResource->GetRamData();

		if (x == 0)
		{
			const PixelRect lTopViewport = mScreenPart->GetRenderArea();
			GetUiManager()->GetRenderer()->ResetClippingRect();
			GetUiManager()->GetRenderer()->SetClippingRect(lTopViewport);
			GetUiManager()->GetRenderer()->SetViewport(lTopViewport);
			mFov = mScreenPart->UpdateFrustum(45.0f);
		}

		QuaternionF lLightOrientation;
		lLightOrientation.RotateAroundOwnX(PIF/8);
		GetUiManager()->GetRenderer()->RenderRelative(lGfxGeometry, &lLightOrientation);
	}
	GetUiManager()->PreparePaint();	// Back to painting again afterwards.
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

void RoadSignButton::LoadTextures()
{
	// We don't want any more textures than we already started loading.
}

void RoadSignButton::DispatchOnLoadMesh(UiCure::UserGeometryReferenceResource* pMeshResource)
{
	Parent::DispatchOnLoadMesh(pMeshResource);

	if (pMeshResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		TBC::GeometryBase* lGeometry = pMeshResource->GetRamData();
		lGeometry->SetAlwaysVisible(false);
		SetInitialPosition(lGeometry);

		if (pMeshResource != mMeshResourceArray[0])
		{
			return;
		}
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
}

void RoadSignButton::OnLoadTexture(UiCure::UserRendererImageResource* pTextureResource)
{
	Parent::OnLoadTexture(pTextureResource);
}

str RoadSignButton::GetMeshInstanceId() const
{
	const int lSlaveIndex = GetManager()? ((GameClientSlaveManager*)GetManager()->GetGameManager())->GetSlaveIndex() : -1;
	const int lSlaveIndexOffset = lSlaveIndex * 100000;
	return (strutil::IntToString(GetInstanceId() + lSlaveIndexOffset, 10));
}



Vector2DF RoadSignButton::Get2dProjectionPosition(const Vector3DF& p3dPosition, float& p2dRadius) const
{
	const PixelRect lTopViewport = mScreenPart->GetRenderArea();
	const float lAspectRatio = lTopViewport.GetWidth() / (float)lTopViewport.GetHeight();
	const float lStrangeCorrectionConstant = 1.011f;
	const float lDistance = ::pow(p3dPosition.y, lStrangeCorrectionConstant);
	const float lProjectionLengthInverse = 1 / (lStrangeCorrectionConstant * asin(mFov * PIF / 180 / 2));
	p2dRadius = mMeshRadius/lDistance * lProjectionLengthInverse * 0.5f * lTopViewport.GetWidth();
	const float lScreenX = +p3dPosition.x/lDistance * lProjectionLengthInverse * 0.5f * lTopViewport.GetWidth() + lTopViewport.GetCenterX();
	const float lScreenY = -p3dPosition.z/lDistance * lProjectionLengthInverse * lAspectRatio * 0.5f * lTopViewport.GetHeight() + lTopViewport.GetCenterY();
	return (Vector2DF(lScreenX, lScreenY));
}

Vector2DF RoadSignButton::GetAspectRatio(bool pInverse) const
{
	const PixelRect lTopViewport = mScreenPart->GetRenderArea();
	const float lAspectRatio = lTopViewport.GetWidth() / (float)lTopViewport.GetHeight();
	const float x = (pInverse^(lAspectRatio < 1))? lAspectRatio : 1;
	const float y = (pInverse^(lAspectRatio < 1))? 1 : 1/lAspectRatio;
	return (Vector2DF(x, y));
}



void RoadSignButton::SetInitialPosition(TBC::GeometryBase* pGeometry) const
{
	TransformationF lTransformation;
	if (mTrajectoryEndPoint.x > 0)
	{
		lTransformation.SetPosition(Vector3DF(mTrajectoryEndDistance*3, 0, mTrajectoryEndDistance));
	}
	else
	{
		lTransformation.SetPosition(Vector3DF(-mTrajectoryEndDistance*3, 0, mTrajectoryEndDistance));
	}
	pGeometry->SetTransformation(lTransformation);
}

float RoadSignButton::GetTargetAngle() const
{
	const float lHingeAngleFactor = 0.7f;	// How much the sign should "hinge" *(1.0 is half a revolution).
	return (-PIF*lHingeAngleFactor*mAnglePart);
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, RoadSignButton);



}
