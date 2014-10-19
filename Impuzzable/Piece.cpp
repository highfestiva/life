
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "Grid.h"
#include "Piece.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/GameManager.h"
#include "../Lepra/Include/Math.h"


#define TIME_UNTIL_TOUCHDOWN	0.05f
#define DRAG_POWER		50.0f
#define VELOCITY_DAMPING	10.0f
#define STOP_DISTANCE		0.1f



namespace Impuzzable
{



Piece::Piece(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager):
	Parent(pResourceManager, pClassId, pUiManager),
	mAverageSpeed(0),
	mDragDepth(5),
	mIsDragging(false)
{
	EnableMeshMove(false);	// We only use translational positioning (orientation ignored).
}

Piece::~Piece()
{
}

bool Piece::TryComplete()
{
	if (Parent::TryComplete())
	{
		GetManager()->EnableMicroTickCallback(this);
		return true;
	}
	return false;
}



void Piece::OnTick()
{
	Parent::OnTick();
	if (!IsLoaded())
	{
		return;
	}

	// Set only translation, ignore orientation.
	Tbc::PhysicsManager* lPhysics = mManager->GetGameManager()->GetPhysicsManager();
	const Tbc::PhysicsManager::BodyID lRootBody = GetRootBodyId();
	mManager->GetGameManager()->GetPhysicsManager()->RestrictBody(lRootBody, 10, 0.1f);
	xform t;
	lPhysics->GetBodyTransform(lRootBody, t);
	quat q;
	//q.RotateAroundOwnX(-PIF/2);
	q.RotateAroundVector(vec3(1,0,0), PIF/2);
	if (MathTraits<float>::IsNan(t.mPosition.z))
	{
		t.mPosition = mPreviousPosition;
		lPhysics->SetBodyTransform(lRootBody, t);
		lPhysics->SetBodyVelocity(lRootBody, vec3());
	}
	else
	{
		mPreviousPosition = t.mPosition;
		lPhysics->GetBodyVelocity(lRootBody, mPreviousVelocity);
	}
	xform lPhysicsTransform(t);
	for (size_t x = 0; x < mMeshResourceArray.size(); ++x)
	{
		UiCure::UserGeometryReferenceResource* lResource = mMeshResourceArray[x];
		if (lResource->GetLoadState() != Cure::RESOURCE_LOAD_COMPLETE)
		{
			continue;
		}
		Tbc::GeometryReference* lGfxGeometry = (Tbc::GeometryReference*)lResource->GetRamData();
		lPhysicsTransform = mPhysics->GetOriginalBoneTransformation(lResource->GetOffset().mGeometryIndex);
		if (x == 0)
		{
			lPhysicsTransform.mPosition = t.mPosition;
		}
		else
		{
			lPhysicsTransform.mPosition *= q;
			lPhysicsTransform.mPosition += t.mPosition;
			lPhysicsTransform.mOrientation = q * lPhysicsTransform.mOrientation;
		}
		lGfxGeometry->SetTransformation(lPhysicsTransform);
	}

	/*vec3 lVelocity = GetVelocity();
	bool lNormalize = false;
	float lSpeed = lVelocity.GetLength();
	mAverageSpeed = Math::Lerp(mAverageSpeed, lSpeed, 0.1f);
	if (lSpeed > 5.1f)
	{
		lSpeed = 4.9f;
		lNormalize = true;
	}
	else if (mAverageSpeed < 2.0f)
	{
		lSpeed = 2.9f;
		mAverageSpeed = 2.9f;
		lNormalize = true;
	}
	if (lNormalize)
	{
		lVelocity.Normalize(lSpeed);
		const Tbc::ChunkyBoneGeometry* lGeometry = mPhysics->GetBoneGeometry(mPhysics->GetRootBone());
		mManager->GetGameManager()->GetPhysicsManager()->SetBodyVelocity(lGeometry->GetRootBodyId(), lVelocity);
	}*/
}

void Piece::OnMicroTick(float)
{
	if (!IsLoaded())
	{
		return;
	}
	const Tbc::PhysicsManager::BodyID lRootBody = GetRootBodyId();
	Tbc::PhysicsManager* lPhysics = mManager->GetGameManager()->GetPhysicsManager();
	xform t;
	lPhysics->GetBodyTransform(lRootBody, t);
	t.GetOrientation().SetIdentity();
	t.GetOrientation().RotateAroundOwnX(PIF/2);
	lPhysics->SetBodyTransform(lRootBody, t);
	lPhysics->SetBodyAngularVelocity(lRootBody, vec3());
	vec3 v;
	lPhysics->GetBodyVelocity(lRootBody, v);
	const vec3 lCenter = mManager->GetGameManager()->GetPhysicsManager()->GetBodyPosition(GetRootBodyId());
	const vec3 lTarget = mIsDragging? mMoveTarget : Grid::GetCenterPosition(lCenter);
	const float lTime = mIsDragging? TIME_UNTIL_TOUCHDOWN : 0;
	const vec3 lDirection = lTarget - (lCenter + v * lTime);
	const float lDistance = lDirection.GetLength();
	if (mIsDragging)
	{
		if (lDistance > STOP_DISTANCE)
		{
			lPhysics->AddForce(lRootBody, lDirection*(DRAG_POWER/lDistance));
		}
		else
		{
			v.Set(0,0,0);
			lPhysics->SetBodyVelocity(lRootBody, v);
		}
	}
	else	// Move to grid.
	{
		lPhysics->AddForce(lRootBody, lDirection*DRAG_POWER);
	}
	if (lDistance < STOP_DISTANCE*5 || !mIsDragging)
	{
		lPhysics->AddForce(lRootBody, -v*VELOCITY_DAMPING);
	}
	/*else if (v.GetLengthSquared() > MAX_VELOCITY*MAX_VELOCITY)
	{
		lPhysics->SetBodyVelocity(lRootBody, v.GetNormalized(MAX_VELOCITY));
	}*/
}



bool Piece::IsDragging() const
{
	return mIsDragging;
}

float Piece::GetDragDepth() const
{
	return mDragDepth;
}

void Piece::SetDragging(bool pIsDragging, float pDepth)
{
	mIsDragging = pIsDragging;
	mDragDepth = pDepth;
}

Tbc::PhysicsManager::BodyID Piece::GetRootBodyId() const
{
	return mPhysics->GetBoneGeometry(0)->GetBodyId();
}

void Piece::GetBodyIds(std::vector<Tbc::PhysicsManager::BodyID>& pBodyIds) const
{
	const int c = mPhysics->GetBoneCount();
	for (int x = 0; x < c; ++x)
	{
		pBodyIds.push_back(mPhysics->GetBoneGeometry(x)->GetBodyId());
	}
}

void Piece::SetDragPosition(const vec3& pPosition)
{
	const vec3 lCenter = mManager->GetGameManager()->GetPhysicsManager()->GetBodyPosition(GetRootBodyId());
	mDragOffset = pPosition - lCenter;
}

vec3 Piece::GetDragPosition() const
{
	const vec3 lCenter = mManager->GetGameManager()->GetPhysicsManager()->GetBodyPosition(GetRootBodyId());
	return lCenter+mDragOffset;
}

void Piece::SetMoveTarget(const vec3& pPosition)
{
	deb_assert(mIsDragging);
	mMoveTarget = pPosition - mDragOffset;
}

vec3 Piece::GetMoveTarget() const
{
	return mMoveTarget + mDragOffset;
}



}
