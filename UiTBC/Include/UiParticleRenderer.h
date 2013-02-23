
// Author: Jonas Byström
// Copyright (c) 2002-2013, Pixel Doctrine

#pragma once

#include "UiDynamicRenderer.h"
#include <list>
#include "../../Lepra/Include/Transformation.h"
#include "UiTbc.h"



namespace UiTbc
{



class ParticleRenderer: public DynamicRenderer
{
	typedef DynamicRenderer Parent;
public:
	ParticleRenderer(Renderer* pRenderer);
	virtual ~ParticleRenderer();

	virtual void Render();
	virtual void Tick(float pTime);

	void CreateExplosion(const Vector3DF& pPosition, float pVelocity, const Vector3DF& pDirection, const Vector3DF& pEllipsoidNorthPole, float pEllipsoidRatio, int pFires, int pSmokes, int pSparks, int pShrapnels);

protected:
	struct Billboard
	{
		Vector3DF mPosition;
		Vector3DF mVelocity;
		Vector3DF mTargetVelocity;
		int mTextureIndex;
		float mDepth;
		float mAngle;
		float mAngularVelocity;
		float mOpacity;
		float mOpacityTime;
		float mTimeFactor;
	};
	typedef std::vector<Billboard> BillboardArray;

	static void StepBillboards(BillboardArray& pBillboards, float pTime, float pFriction);
	static void SetDepth(BillboardArray& pBillboards, const TransformationF& pCam, const QuaternionF& pCamOrientationInverse);
	static void CreateBillboards(const Vector3DF& pPosition, float pVelocity, const Vector3DF& pDirection, const Vector3DF& pTargetVelocity,
		float pEndTurbulence, float pTimeFactor, BillboardArray& pBillboards, int pCount);
	static bool CompareDepths(const Billboard& b0, const Billboard& b1);

	BillboardArray mFires;
	BillboardArray mSmokes;
	BillboardArray mSparks;
	BillboardArray mShrapnels;
};



}
