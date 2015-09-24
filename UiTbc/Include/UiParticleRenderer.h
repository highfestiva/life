
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

#pragma once

#include "UiDynamicRenderer.h"
#include "../../Lepra/Include/Thread.h"
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

	void SetGravity(vec3 pGravity);
	void SetData(int pGasTextureCount, int pTotalTextureCount, BillboardGeometry* pGas, BillboardGeometry* pShrapnel, BillboardGeometry* pSpark, BillboardGeometry* pGlow);

	virtual void Render();
	virtual void Tick(float pTime);

	void CreateFlare(const vec3& pColor, float pStrength, float pTimeFactor, const vec3& pPosition, const vec3& pVelocity);
	void CreateExplosion(const vec3& pPosition, float pStrength, const vec3& pVelocity, float pFalloff, float pTime, const vec3& pStartFireColor, const vec3& pFireColor,
		const vec3& pStartSmokeColor, const vec3& pSmokeColor, const vec3& pSharpnelColor, int pFires, int pSmokes, int pSparks, int pShrapnels);
	void CreatePebble(float pTime, float pScale, float pAngularVelocity, const vec3& pColor, const vec3& pPosition, const vec3& pVelocity);
	void CreateFume(float pTime, float pScale, float pAngularVelocity, float pOpacity, const vec3& pPosition, const vec3& pVelocity);
	void CreateGlow(float pTime, float pScale, const vec3& pStartColor, const vec3& pColor, float pOpacity, const vec3& pPosition, const vec3& pVelocity);
	void RenderFireBillboard(float pAngle, float pSize, const vec3& pColor, float pOpacity, const vec3& pPosition);

protected:
	struct Light
	{
		Light(const vec3& pColor, float pStrength, const vec3& pPosition, const vec3& pVelocity, const vec3& pTargetVelocity, float pTimeFactor):
			mColor(pColor),
			mPosition(pPosition),
			mVelocity(pVelocity),
			mTargetVelocity(pTargetVelocity),
			mStrength(pStrength),
			mTimeFactor(pTimeFactor),
			mRenderLightId(Renderer::INVALID_LIGHT)
		{
		}
		vec3 mColor;
		vec3 mPosition;
		vec3 mVelocity;
		vec3 mTargetVelocity;
		float mStrength;
		float mTimeFactor;
		Renderer::LightID mRenderLightId;
	};
	typedef std::vector<Light> LightArray;

	struct Billboard
	{
		vec3 mPosition;
		vec3 mVelocity;
		vec3 mTargetVelocity;
		vec3 mStartColor;
		vec3 mColor;
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

	void CreateTempLight(const vec3& pColor, float pStrength, const vec3& pPosition, const vec3& pVelocity, const vec3& pTargetVelocity, float pTimeFactor);
	void StepLights(float pTime, float pFriction);

	void CreateBillboards(const vec3& pPosition, float pStrength, const vec3& pVelocity, const vec3& pTargetVelocity,
		float pEndTurbulence, float pTimeFactor, float pSizeFactor, const vec3& pStartColor, const vec3& pColor,
		BillboardArray& pBillboards, int pCount);
	static void StepBillboards(BillboardArray& pBillboards, float pTime, float pFriction);

	Lock* mLock;
	vec3 mGravity;
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

	logclass();
};



}
