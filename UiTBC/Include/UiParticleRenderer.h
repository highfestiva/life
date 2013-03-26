
// Author: Jonas Bystr�m
// Copyright (c) 2002-2013, Pixel Doctrine

#pragma once

#include "UiDynamicRenderer.h"
#include "UiRenderer.h"



namespace UiTbc
{



class BillboardGeometry;



class ParticleRenderer: public DynamicRenderer
{
	typedef DynamicRenderer Parent;
public:
	ParticleRenderer(Renderer* pRenderer, int pMaxLightCount);
	virtual ~ParticleRenderer();

	void SetData(int pTextureCount, BillboardGeometry* pGas, BillboardGeometry* pShrapnel, BillboardGeometry* pSpark);

	virtual void Render();
	virtual void Tick(float pTime);

	void CreateFlare(const Vector3DF& pColor, float pStrength, float pTimeFactor, const Vector3DF& pPosition, const Vector3DF& pVelocity);
	void CreateExplosion(const Vector3DF& pPosition, float pStrength, const Vector3DF& pDirection, float pFalloff, const Vector3DF& pFireColor,
		const Vector3DF& pSmokeColor, const Vector3DF& pSharpnelColor, int pFires, int pSmokes, int pSparks, int pShrapnels);
	void CreatePebble(float pTime, float pScale, float pAngularVelocity, const Vector3DF& pColor, const Vector3DF& pPosition, const Vector3DF& pVelocity);
	void CreateFume(float pTime, float pScale, float pAngularVelocity, float pOpacity, const Vector3DF& pPosition, const Vector3DF& pVelocity);

protected:
	struct Light
	{
		Light(const Vector3DF& pColor, float pStrength, const Vector3DF& pPosition, const Vector3DF& pVelocity, const Vector3DF& pTargetVelocity, float pTimeFactor):
			mColor(pColor),
			mPosition(pPosition),
			mVelocity(pVelocity),
			mTargetVelocity(pTargetVelocity),
			mStrength(pStrength),
			mTimeFactor(pTimeFactor),
			mRenderLightId(Renderer::INVALID_LIGHT)
		{
		}
		Vector3DF mColor;
		Vector3DF mPosition;
		Vector3DF mVelocity;
		Vector3DF mTargetVelocity;
		float mStrength;
		float mTimeFactor;
		Renderer::LightID mRenderLightId;
	};
	typedef std::vector<Light> LightArray;

	struct Billboard
	{
		Vector3DF mPosition;
		Vector3DF mVelocity;
		Vector3DF mTargetVelocity;
		Vector3DF mColor;
		int mTextureIndex;
		float mSizeFactor;
		float mDepth;
		float mAngle;
		float mAngularVelocity;
		float mOpacity;
		float mOpacityFactor;
		float mOpacityTime;
		float mTimeFactor;
	};
	typedef std::vector<Billboard> BillboardArray;

	void CreateTempLight(const Vector3DF& pColor, float pStrength, const Vector3DF& pPosition, const Vector3DF& pVelocity, const Vector3DF& pTargetVelocity, float pTimeFactor);
	void StepLights(float pTime, float pFriction);

	void CreateBillboards(const Vector3DF& pPosition, float pStrength, const Vector3DF& pDirection, const Vector3DF& pTargetVelocity,
		float pEndTurbulence, float pTimeFactor, float pSizeFactor, const Vector3DF& pColor, BillboardArray& pBillboards, int pCount);
	static void StepBillboards(BillboardArray& pBillboards, float pTime, float pFriction);

	LightArray mLights;
	size_t mMaxLightCount;
	size_t mTextureCount;
	BillboardGeometry* mBillboardGas;
	BillboardGeometry* mBillboardShrapnel;
	BillboardGeometry* mBillboardSpark;

	BillboardArray mFires;
	BillboardArray mSmokes;
	BillboardArray mSparks;
	BillboardArray mShrapnels;

	LOG_CLASS_DECLARE();
};



}
