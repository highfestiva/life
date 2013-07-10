
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

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

	void SetData(int pGasTextureCount, int pTotalTextureCount, BillboardGeometry* pGas, BillboardGeometry* pShrapnel, BillboardGeometry* pSpark, BillboardGeometry* pGlow);

	virtual void Render();
	virtual void Tick(float pTime);

	void CreateFlare(const Vector3DF& pColor, float pStrength, float pTimeFactor, const Vector3DF& pPosition, const Vector3DF& pVelocity);
	void CreateExplosion(const Vector3DF& pPosition, float pStrength, const Vector3DF& pDirection, float pFalloff, const Vector3DF& pStartFireColor, const Vector3DF& pFireColor,
		const Vector3DF& pStartSmokeColor, const Vector3DF& pSmokeColor, const Vector3DF& pSharpnelColor, int pFires, int pSmokes, int pSparks, int pShrapnels);
	void CreatePebble(float pTime, float pScale, float pAngularVelocity, const Vector3DF& pColor, const Vector3DF& pPosition, const Vector3DF& pVelocity);
	void CreateFume(float pTime, float pScale, float pAngularVelocity, float pOpacity, const Vector3DF& pPosition, const Vector3DF& pVelocity);
	void CreateGlow(float pTime, float pScale, const Vector3DF& pStartColor, const Vector3DF& pColor, float pOpacity, const Vector3DF& pPosition, const Vector3DF& pVelocity);
	void RenderFireBillboard(float pAngle, float pSize, const Vector3DF& pColor, float pOpacity, const Vector3DF& pPosition);

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
		Vector3DF mStartColor;
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
		float pEndTurbulence, float pTimeFactor, float pSizeFactor, const Vector3DF& pStartColor, const Vector3DF& pColor,
		BillboardArray& pBillboards, int pCount);
	static void StepBillboards(BillboardArray& pBillboards, float pTime, float pFriction);

	LightArray mLights;
	size_t mMaxLightCount;
	size_t mGasTextureCount;
	size_t mTotalTextureCount;
	BillboardGeometry* mBillboardGas;
	BillboardGeometry* mBillboardShrapnel;
	BillboardGeometry* mBillboardSpark;
	BillboardGeometry* mBillboardGlow;

	BillboardArray mFires;
	BillboardArray mSmokes;
	BillboardArray mSparks;
	BillboardArray mShrapnels;
	BillboardArray mTempFires;

	LOG_CLASS_DECLARE();
};



}
