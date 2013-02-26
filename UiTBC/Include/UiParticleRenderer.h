
// Author: Jonas Byström
// Copyright (c) 2002-2013, Pixel Doctrine

#pragma once

#include "UiDynamicRenderer.h"
#include <list>
#include "../../Lepra/Include/Transformation.h"
#include "UiRenderer.h"



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

	void CreateFlare(float pStrength, float pTimeFactor, const Vector3DF& pPosition, const Vector3DF& pVelocity);
	void CreateExplosion(const Vector3DF& pPosition, float pStrength, const Vector3DF& pDirection, float pFalloff, const Vector3DF& pEllipsoidNorthPole, float pEllipsoidRatio, int pFires, int pSmokes, int pSparks, int pShrapnels);

protected:
	struct Light
	{
		Light(float pStrength, const Vector3DF& pPosition, const Vector3DF& pVelocity, const Vector3DF& pTargetVelocity, float pTimeFactor):
			mPosition(pPosition),
			mVelocity(pVelocity),
			mTargetVelocity(pTargetVelocity),
			mStrength(pStrength),
			mTimeFactor(pTimeFactor),
			mRenderLightId(UiTbc::Renderer::INVALID_LIGHT)
		{
		}
		Vector3DF mPosition;
		Vector3DF mVelocity;
		Vector3DF mTargetVelocity;
		float mStrength;
		float mTimeFactor;
		UiTbc::Renderer::LightID mRenderLightId;
	};
	typedef std::vector<Light> LightArray;

	struct Billboard
	{
		Vector3DF mPosition;
		Vector3DF mVelocity;
		Vector3DF mTargetVelocity;
		int mTextureIndex;
		float mSizeFactor;
		float mDepth;
		float mAngle;
		float mAngularVelocity;
		float mOpacity;
		float mOpacityTime;
		float mTimeFactor;
	};
	typedef std::vector<Billboard> BillboardArray;

	void CreateTempLight(float pStrength, const Vector3DF& pPosition, const Vector3DF& pVelocity, const Vector3DF& pTargetVelocity, float pTimeFactor);
	void StepLights(float pTime, float pFriction);

	static void CreateBillboards(const Vector3DF& pPosition, float pStrength, const Vector3DF& pDirection, const Vector3DF& pTargetVelocity,
		float pEndTurbulence, float pTimeFactor, float pSizeFactor, BillboardArray& pBillboards, int pCount);
	static void StepBillboards(BillboardArray& pBillboards, float pTime, float pFriction);

	LightArray mLights;
	size_t mMaxLightCount;

	BillboardArray mFires;
	BillboardArray mSmokes;
	BillboardArray mSparks;
	BillboardArray mShrapnels;

	LOG_CLASS_DECLARE();
};



}
