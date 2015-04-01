
// Author: Jonas Bystr�m
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/UiParticleRenderer.h"
#include "../../Lepra/Include/Random.h"
#include "../Include/UiBillboardGeometry.h"

#define RNDCOL(col, lo, hi)		vec3(Random::Uniform(col.x*lo, col.x*hi), Random::Uniform(col.y*lo, col.y*hi), Random::Uniform(col.z*lo, col.z*hi))
#define OPACITY_FADE_IN_TIME_OFFSET	(PIF*0.25f + 0.03f)
#define PARTICLE_TIME			(PIF+OPACITY_FADE_IN_TIME_OFFSET*2)



namespace UiTbc
{



ParticleRenderer::ParticleRenderer(Renderer* pRenderer, int pMaxLightCount):
	Parent(pRenderer),
	mGravity(0,0,-9.8f),
	mMaxLightCount(pMaxLightCount),
	mGasTextureCount(1),
	mTotalTextureCount(1),
	mBillboardGas(0),
	mBillboardShrapnel(0),
	mBillboardSpark(0),
	mBillboardGlow(0)
{
}

ParticleRenderer::~ParticleRenderer()
{
	delete mBillboardGas;
	mBillboardGas = 0;
	delete mBillboardShrapnel;
	mBillboardShrapnel = 0;
	delete mBillboardSpark;
	mBillboardSpark = 0;
	delete mBillboardGlow;
	mBillboardGlow = 0;
}



void ParticleRenderer::SetGravity(vec3 pGravity)
{
	mGravity = pGravity;
}

void ParticleRenderer::SetData(int pGasTextureCount, int pTotalTextureCount, BillboardGeometry* pGas, BillboardGeometry* pShrapnel, BillboardGeometry* pSpark, BillboardGeometry* pGlow)
{
	mGasTextureCount = pGasTextureCount;
	mTotalTextureCount = pTotalTextureCount;
	deb_assert(mTotalTextureCount >= mGasTextureCount);
	deb_assert(mTotalTextureCount > 0);
	mBillboardGas = pGas;
	mBillboardShrapnel = pShrapnel;
	mBillboardSpark = pSpark;
	mBillboardGlow = pGlow;
}

void ParticleRenderer::Render()
{
	if (!mBillboardGas)
	{
		return;
	}

	const float q = 1;
	BillboardArray::iterator x;
	BillboardRenderInfoArray lBillboards;
	x = mSmokes.begin();
	for (; x != mSmokes.end(); ++x)
	{
		const float s = (q + x->mOpacityTime) * x->mSizeFactor;
		const float r = Math::Lerp(x->mStartColor.x, x->mColor.x, x->mOpacityTime/PARTICLE_TIME);
		const float g = Math::Lerp(x->mStartColor.y, x->mColor.y, x->mOpacityTime/PARTICLE_TIME);
		const float b = Math::Lerp(x->mStartColor.z, x->mColor.z, x->mOpacityTime/PARTICLE_TIME);
		lBillboards.push_back(BillboardRenderInfo(x->mAngle, x->mPosition, s, vec3(r, g, b), x->mOpacity, x->mTextureIndex));
	}
	mRenderer->RenderBillboards(mBillboardGas, true, false, lBillboards);

	lBillboards.clear();
	x = mShrapnels.begin();
	for (; x != mShrapnels.end(); ++x)
	{
		const float s = x->mSizeFactor;
		lBillboards.push_back(BillboardRenderInfo(x->mAngle, x->mPosition, s, x->mColor, x->mOpacity, x->mTextureIndex));
	}
	mRenderer->RenderBillboards(mBillboardShrapnel, false, false, lBillboards);

	lBillboards.clear();
	const quat& lCamOrientationInverse = mRenderer->GetCameraOrientationInverse();
	const vec3 lCameraXZPlane(0,1,0);	// In cam space, that is.
	x = mSparks.begin();
	for (; x != mSparks.end(); ++x)
	{
		const vec3 lAngleVector = (lCamOrientationInverse * x->mVelocity).ProjectOntoPlane(lCameraXZPlane);
		const float lAngle = PIF/2 - lAngleVector.GetPolarCoordAngleY();
		const float s = x->mSizeFactor;
		const float r  = Math::Lerp(1.0f, 0.6f, x->mOpacityTime/PARTICLE_TIME);
		const float gb = Math::Lerp(1.0f, 0.3f, x->mOpacityTime/PARTICLE_TIME);
		lBillboards.push_back(BillboardRenderInfo(lAngle, x->mPosition, s, vec3(r, gb, gb), x->mOpacity, x->mTextureIndex));
	}
	mRenderer->RenderBillboards(mBillboardSpark, false, true, lBillboards);

	lBillboards.clear();
	x = mFires.begin();
	for (; x != mFires.end(); ++x)
	{
		const float s = (q + x->mOpacityTime) * x->mSizeFactor;
		const float r = Math::Lerp(x->mStartColor.x, x->mColor.x, x->mOpacityTime/PARTICLE_TIME);
		const float g = Math::Lerp(x->mStartColor.y, x->mColor.y, x->mOpacityTime/PARTICLE_TIME);
		const float b = Math::Lerp(x->mStartColor.z, x->mColor.z, x->mOpacityTime/PARTICLE_TIME);
		lBillboards.push_back(BillboardRenderInfo(x->mAngle, x->mPosition, s, vec3(r, g, b), x->mOpacity, x->mTextureIndex));
	}
	mRenderer->RenderBillboards(mBillboardGas, true, true, lBillboards);

	lBillboards.clear();
	x = mTempFires.begin();
	for (; x != mTempFires.end(); ++x)
	{
		lBillboards.push_back(BillboardRenderInfo(x->mAngle, x->mPosition, x->mSizeFactor, x->mColor, x->mOpacity, x->mTextureIndex));
	}
	mRenderer->RenderBillboards(mBillboardGlow, true, true, lBillboards);

	// Update lights.
	LightArray::iterator y = mLights.begin();
	for (; y < mLights.end(); ++y)
	{
		if (y->mRenderLightId != Renderer::INVALID_LIGHT)
		{
			mRenderer->SetLightPosition(y->mRenderLightId, y->mPosition);
			mRenderer->SetLightColor(y->mRenderLightId, vec3(0.6f, 0.4f, 0.2f) * y->mStrength * 30.0f);
			deb_assert(y->mStrength >= 0 && y->mStrength < 1000);
		}
	}
}

void ParticleRenderer::Tick(float pTime)
{
	StepLights(pTime, 6);

	StepBillboards(mFires, pTime, 6);
	StepBillboards(mSmokes, pTime, 5);
	StepBillboards(mSparks, pTime, -0.4f);
	StepBillboards(mShrapnels, pTime, -0.2f);
	mTempFires.clear();
}

void ParticleRenderer::CreateFlare(const vec3& pColor, float pStrength, float pTimeFactor, const vec3& pPosition, const vec3& pVelocity)
{
	CreateTempLight(pColor, pStrength, pPosition, pVelocity, pVelocity, pTimeFactor);
}

void ParticleRenderer::CreateExplosion(const vec3& pPosition, float pStrength, const vec3& pVelocity, float pFalloff, float pTime, const vec3& pStartFireColor, const vec3& pFireColor,
	const vec3& pStartSmokeColor, const vec3& pSmokeColor, const vec3& pSharpnelColor, int pFires, int pSmokes, int pSparks, int pShrapnels)
{
	const float lRandomXYEndSpeed = 1.0f;
	const float lStrength2 = (pStrength>1)? ::sqrt(pStrength) : pStrength*pStrength;
	const float lParticleSize = lStrength2*0.5f;
	const float lSpeed = pVelocity.GetLength() * 0.01f;
	CreateBillboards(pPosition,  7*lStrength2+ 1*lSpeed,      pVelocity, Math::Lerp(pVelocity,mGravity*-0.2f,pFalloff), lRandomXYEndSpeed, 5.3f/pTime, lParticleSize,      pStartFireColor, pFireColor, mFires, pFires);
	CreateBillboards(pPosition,  8*lStrength2+ 1*lSpeed,      pVelocity, Math::Lerp(pVelocity,mGravity*+0.2f,pFalloff), lRandomXYEndSpeed,    3/pTime, lParticleSize*2,    pStartSmokeColor, pSmokeColor, mSmokes, pSmokes);
	CreateBillboards(pPosition, 20*lStrength2+20*lSpeed, 1.2f*pVelocity, Math::Lerp(pVelocity,mGravity*+0.8f,pFalloff), lRandomXYEndSpeed, 4.5f/pTime, lParticleSize*0.4f, vec3(), vec3(), mSparks, pSparks);
	CreateBillboards(pPosition,  9*lStrength2+10*lSpeed, 1.1f*pVelocity, Math::Lerp(pVelocity,mGravity*+1.1f,pFalloff), lRandomXYEndSpeed, 0.7f/pTime, lParticleSize*0.5f, pSharpnelColor, pSharpnelColor, mShrapnels, pShrapnels);

	const float lMinSparkVelocity2 = lStrength2*100;
	const vec3 lCamPlane = mRenderer->GetCameraTransformation().GetOrientation() * vec3(0,1,0);
	BillboardArray::reverse_iterator x = mSparks.rbegin();
	for (int y = 0; y < pSparks; ++y, ++x)
	{
		x->mVelocity = x->mVelocity.ProjectOntoPlane(lCamPlane);
		float lSpeed2 = x->mVelocity.GetLengthSquared();
		if (lSpeed2 < lMinSparkVelocity2)
		{
			x->mVelocity.Mul(::sqrt(lMinSparkVelocity2/lSpeed2));
		}
	}

	if (pFires > 0)
	{
		const Billboard& lFire = mFires.back();
		CreateTempLight(pFireColor, pStrength, lFire.mPosition, lFire.mVelocity, lFire.mTargetVelocity, lFire.mTimeFactor);
	}
}

void ParticleRenderer::CreatePebble(float pTime, float pScale, float pAngularVelocity, const vec3& pColor, const vec3& pPosition, const vec3& pVelocity)
{
	const float lTimeFactor = PARTICLE_TIME*0.5f/pTime;	// Split in two, as we're only using latter half of sine curve (don't fade into existance).
	const float lRandomXYEndSpeed = 1.0f;

	CreateBillboards(pPosition, 0, vec3(), vec3(0,0,-10), lRandomXYEndSpeed, lTimeFactor, pScale*0.1f, vec3(), pColor, mShrapnels, 1);
	Billboard& lPebbleBillboard = mShrapnels.back();
	lPebbleBillboard.mVelocity = pVelocity;
	lPebbleBillboard.mAngularVelocity = Random::Uniform(-pAngularVelocity, +pAngularVelocity);
	lPebbleBillboard.mOpacity = 1;
	lPebbleBillboard.mOpacityTime += PIF/2;	// Move to "fully opaque" time in sine curve.
}

void ParticleRenderer::CreateFume(float pTime, float pScale, float pAngularVelocity, float pOpacity, const vec3& pPosition, const vec3& pVelocity)
{
	const float lTimeFactor = PARTICLE_TIME/pTime;
	const float lRandomXYEndSpeed = 0.5f;

	CreateBillboards(pPosition, 0, vec3(), vec3(0,0,2), lRandomXYEndSpeed, lTimeFactor, pScale*0.1f, vec3(0.4f,0.4f,0.4f), vec3(), mSmokes, 1);
	Billboard& lPebbleBillboard = mSmokes.back();
	lPebbleBillboard.mVelocity = pVelocity;
	lPebbleBillboard.mAngularVelocity = Random::Uniform(-pAngularVelocity, +pAngularVelocity);
	lPebbleBillboard.mOpacityFactor = pOpacity;
	//lPebbleBillboard.mOpacityTime += OPACITY_FADE_IN_TIME_OFFSET * 0.5f;	// Move forward some in time to ensure that we're very visible when we come out of the pipe.
}

void ParticleRenderer::CreateGlow(float pTime, float pScale, const vec3& pStartColor, const vec3& pColor, float pOpacity, const vec3& pPosition, const vec3& pVelocity)
{
	const float lTimeFactor = PARTICLE_TIME/pTime;
	CreateBillboards(pPosition, 0, vec3(), vec3(), 0, lTimeFactor, pScale, pStartColor, pColor, mFires, 1);
	Billboard& lGlowBillboard = mFires.back();
	lGlowBillboard.mOpacityFactor = pOpacity;
	lGlowBillboard.mVelocity = pVelocity;
	lGlowBillboard.mTargetVelocity = pVelocity;
}

void ParticleRenderer::RenderFireBillboard(float pAngle, float pSize, const vec3& pColor, float pOpacity, const vec3& pPosition)
{
	mTempFires.push_back(Billboard());
	Billboard& lBillboard = mTempFires.back();
	lBillboard.mPosition = pPosition;
	lBillboard.mColor = pColor;
	lBillboard.mTextureIndex = mTotalTextureCount-1;
	lBillboard.mDepth = 1000.0f;
	lBillboard.mSizeFactor = pSize;
	lBillboard.mAngle = pAngle;
	lBillboard.mOpacity = pOpacity;
}



void ParticleRenderer::CreateTempLight(const vec3& pColor, float pStrength, const vec3& pPosition, const vec3& pVelocity, const vec3& pTargetVelocity, float pTimeFactor)
{
	if (mLights.size() < mMaxLightCount)
	{
		mLights.push_back(Light(pColor, pStrength, pPosition, pVelocity, pTargetVelocity, pTimeFactor));
		//mLog.Infof(_T("Creating new light with strength %f"), pStrength);
	}
	else
	{
		int lDarkestLightIndex = -1;
		float lDarkestLightStrength = 1e15f;
		int i = 0;
		LightArray::iterator x = mLights.begin();
		for (; x < mLights.end(); ++x, ++i)
		{
			if (x->mStrength < lDarkestLightStrength)
			{
				lDarkestLightIndex = i;
				lDarkestLightStrength = x->mStrength;
			}
		}
		if (pStrength >= lDarkestLightStrength && lDarkestLightIndex >= 0)
		{
			//mLog.Infof(_T("Overtaking light with render ID %i (had strength %f, got strength %f)"), mLights[lDarkestLightIndex].mRenderLightId, mLights[lDarkestLightIndex].mStrength, pStrength);
			// TRICKY: don't overwrite! We must not leak the previosly allocated hardware light!
			mLights[lDarkestLightIndex].mColor = pColor;
			mLights[lDarkestLightIndex].mStrength = pStrength;
			mLights[lDarkestLightIndex].mPosition = pPosition;
			mLights[lDarkestLightIndex].mVelocity = pVelocity;
			mLights[lDarkestLightIndex].mTargetVelocity = pTargetVelocity;
			mLights[lDarkestLightIndex].mTimeFactor = pTimeFactor;
		}
	}
}

void ParticleRenderer::StepLights(float pTime, float pFriction)
{
	(void)pFriction;

	LightArray::iterator x = mLights.begin();
	while (x != mLights.end())
	{
		x->mVelocity = Math::Lerp(x->mVelocity, x->mTargetVelocity, pFriction*pTime);
		x->mPosition += x->mVelocity * pTime;

		x->mStrength -= pTime * x->mTimeFactor * 2 / PIF;
		if (x->mStrength <= 0.1f)
		{
			if (x->mRenderLightId != Renderer::INVALID_LIGHT)
			{
				//mLog.Infof(_T("Dropping light with render ID %i."), x->mRenderLightId);
				mRenderer->RemoveLight(x->mRenderLightId);
			}
			x = mLights.erase(x);
		}
		else
		{
			if (x->mRenderLightId == Renderer::INVALID_LIGHT)
			{
				x->mRenderLightId = mRenderer->AddPointLight(Renderer::LIGHT_MOVABLE, x->mPosition, x->mColor*x->mStrength*10, x->mStrength*10, 0);
				//mLog.Infof(_T("Creating render ID %i for light with strength %f"), x->mRenderLightId, x->mStrength);
			}
			++x;
		}
	}
}

void ParticleRenderer::CreateBillboards(const vec3& pPosition, float pStrength, const vec3& pVelocity, const vec3& pTargetVelocity,
	float pEndTurbulence, float pTimeFactor, float pSizeFactor, const vec3& pStartColor, const vec3& pColor, BillboardArray& pBillboards, int pCount)
{
	for (int x = 0; x < pCount; ++x)
	{
		pBillboards.push_back(Billboard());
		Billboard& lBillboard = pBillboards.back();
		lBillboard.mVelocity = pVelocity + RNDVEC(pStrength);
		const vec3 lThisParticlesTargetVelocity = Math::Lerp(pTargetVelocity*0.8f, pTargetVelocity, Random::Uniform(0.0f, 2.0f));
		lBillboard.mTargetVelocity = RNDVEC(pEndTurbulence) + lThisParticlesTargetVelocity;
		lBillboard.mPosition = pPosition + lBillboard.mVelocity*0.1f;
		lBillboard.mStartColor = RNDCOL(pStartColor, 0.9f, 1.1f);
		lBillboard.mColor = RNDCOL(pColor, 0.7f, 1.3f);
		lBillboard.mTextureIndex = Random::GetRandomNumber() % mGasTextureCount;
		lBillboard.mDepth = 1000.0f;
		lBillboard.mSizeFactor = Random::Uniform(pSizeFactor*0.7f, pSizeFactor*1.4f);
		lBillboard.mAngle = Random::Uniform(0.0f, 2*PIF);
		lBillboard.mAngularVelocity = Random::Uniform(-5.0f, +5.0f);
		lBillboard.mOpacity = 0;
		lBillboard.mOpacityFactor = 1;
		lBillboard.mOpacityTime = Random::Uniform(0.0f, 0.3f);
		deb_assert(pTimeFactor > 0);
		lBillboard.mTimeFactor = Random::Uniform(pTimeFactor*0.7f, pTimeFactor*1.3f);
	}
}

void ParticleRenderer::StepBillboards(BillboardArray& pBillboards, float pTime, float pFriction)
{
	BillboardArray::iterator x = pBillboards.begin();
	while (x != pBillboards.end())
	{
		if (pFriction > 0)
		{
			x->mVelocity = Math::Lerp(x->mVelocity, x->mTargetVelocity, pFriction*pTime);
			x->mAngularVelocity = Math::Lerp(x->mAngularVelocity, x->mAngularVelocity * 0.7f, pFriction*pTime);
		}
		else
		{
			x->mVelocity *= 1 + pFriction*pTime;
			x->mVelocity += x->mTargetVelocity * pTime;
		}
		x->mPosition += x->mVelocity * pTime;
		x->mAngle += x->mAngularVelocity * pTime;

		x->mOpacityTime += pTime * x->mTimeFactor;
		x->mOpacity = (::sin(x->mOpacityTime) + 0.7f) * x->mOpacityFactor;
		if (x->mOpacity <= 0.0f || x->mOpacityTime > PARTICLE_TIME)
		{
			//deb_assert(x->mOpacityTime > PIF);	// Verify that the particle was visible at all, or the algo's wrong.
			x = pBillboards.erase(x);
		}
		else
		{
			++x;
		}
	}
}



loginstance(UI_GFX_3D, ParticleRenderer);



}