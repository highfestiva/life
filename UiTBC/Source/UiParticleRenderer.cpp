
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../Include/UiParticleRenderer.h"
#include "../../Lepra/Include/Random.h"
#include "../Include/UiBillboardGeometry.h"

#define RNDCOL(col, lo, hi)		Vector3DF(Random::Uniform(col.x*lo, col.x*hi), Random::Uniform(col.y*lo, col.y*hi), Random::Uniform(col.z*lo, col.z*hi))
#define OPACITY_FADE_IN_TIME_OFFSET	(PIF*0.25f + 0.03f)
#define PARTICLE_TIME			(PIF+OPACITY_FADE_IN_TIME_OFFSET*2)



namespace UiTbc
{



ParticleRenderer::ParticleRenderer(Renderer* pRenderer, int pMaxLightCount):
	Parent(pRenderer),
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
		lBillboards.push_back(BillboardRenderInfo(x->mAngle, x->mPosition, s, Vector3DF(r, g, b), x->mOpacity, x->mTextureIndex));
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
	const QuaternionF& lCamOrientationInverse = mRenderer->GetCameraOrientationInverse();
	const Vector3DF lCameraXZPlane(0,1,0);	// In cam space, that is.
	x = mSparks.begin();
	for (; x != mSparks.end(); ++x)
	{
		const Vector3DF lAngleVector = (lCamOrientationInverse * x->mVelocity).ProjectOntoPlane(lCameraXZPlane);
		const float lAngle = PIF/2 - lAngleVector.GetPolarCoordAngleY();
		const float s = x->mSizeFactor;
		const float r  = Math::Lerp(1.0f, 0.6f, x->mOpacityTime/PARTICLE_TIME);
		const float gb = Math::Lerp(1.0f, 0.3f, x->mOpacityTime/PARTICLE_TIME);
		lBillboards.push_back(BillboardRenderInfo(lAngle, x->mPosition, s, Vector3DF(r, gb, gb), x->mOpacity, x->mTextureIndex));
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
		lBillboards.push_back(BillboardRenderInfo(x->mAngle, x->mPosition, s, Vector3DF(r, g, b), x->mOpacity, x->mTextureIndex));
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
			mRenderer->SetLightColor(y->mRenderLightId, Vector3DF(0.6f, 0.4f, 0.2f) * y->mStrength * 30.0f);
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

void ParticleRenderer::CreateFlare(const Vector3DF& pColor, float pStrength, float pTimeFactor, const Vector3DF& pPosition, const Vector3DF& pVelocity)
{
	CreateTempLight(pColor, pStrength, pPosition, pVelocity, pVelocity, pTimeFactor);
}

void ParticleRenderer::CreateExplosion(const Vector3DF& pPosition, float pStrength, const Vector3DF& pDirection, float pFalloff, const Vector3DF& pStartFireColor, const Vector3DF& pFireColor,
	const Vector3DF& pStartSmokeColor, const Vector3DF& pSmokeColor, const Vector3DF& pSharpnelColor, int pFires, int pSmokes, int pSparks, int pShrapnels)
{
	const float lRandomXYEndSpeed = 1.0f;
	const float lSparkSize = (pStrength > 1)? ::sqrt(pStrength)*0.25f : pStrength*0.15f;
	CreateBillboards(pPosition, pStrength* 7, pDirection, Vector3DF(0, 0,  +9)*pFalloff, lRandomXYEndSpeed, 5.3f, pStrength*0.4f, pStartFireColor, pFireColor, mFires, pFires);
	CreateBillboards(pPosition, pStrength* 8, pDirection, Vector3DF(0, 0,  +5)*pFalloff, lRandomXYEndSpeed,    3, pStrength*0.8f, pStartSmokeColor, pSmokeColor, mSmokes, pSmokes);
	CreateBillboards(pPosition, pStrength*13, pDirection, Vector3DF(0, 0,  -8)*pFalloff, lRandomXYEndSpeed, 4.5f, lSparkSize, Vector3DF(), Vector3DF(), mSparks, pSparks);
	CreateBillboards(pPosition, pStrength* 5, pDirection, Vector3DF(0, 0, -10)*pFalloff, lRandomXYEndSpeed, 1.5f, ::sqrtf(pStrength)*0.20f, pSharpnelColor, pSharpnelColor, mShrapnels, pShrapnels);

	const float lMinSparkVelocity2 = pStrength*9*1.2f*pStrength*8*1.2f;
	BillboardArray::reverse_iterator x = mSparks.rbegin();
	for (int y = 0; y < pSparks; ++y, ++x)
	{
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

void ParticleRenderer::CreatePebble(float pTime, float pScale, float pAngularVelocity, const Vector3DF& pColor, const Vector3DF& pPosition, const Vector3DF& pVelocity)
{
	const float lTimeFactor = PARTICLE_TIME*0.5f/pTime;	// Split in two, as we're only using latter half of sine curve (don't fade into existance).
	const float lRandomXYEndSpeed = 1.0f;

	CreateBillboards(pPosition, 0, Vector3DF(), Vector3DF(0, 0, -10), lRandomXYEndSpeed, lTimeFactor, pScale*0.1f, Vector3DF(), pColor, mShrapnels, 1);
	Billboard& lPebbleBillboard = mShrapnels.back();
	lPebbleBillboard.mVelocity = pVelocity;
	lPebbleBillboard.mAngularVelocity = Random::Uniform(-pAngularVelocity, +pAngularVelocity);
	lPebbleBillboard.mOpacity = 1;
	lPebbleBillboard.mOpacityTime += PIF/2;	// Move to "fully opaque" time in sine curve.
}

void ParticleRenderer::CreateFume(float pTime, float pScale, float pAngularVelocity, float pOpacity, const Vector3DF& pPosition, const Vector3DF& pVelocity)
{
	const float lTimeFactor = PARTICLE_TIME/pTime;
	const float lRandomXYEndSpeed = 0.5f;

	CreateBillboards(pPosition, 0, Vector3DF(), Vector3DF(0,0,2), lRandomXYEndSpeed, lTimeFactor, pScale*0.1f, Vector3DF(0.4f,0.4f,0.4f), Vector3DF(), mSmokes, 1);
	Billboard& lPebbleBillboard = mSmokes.back();
	lPebbleBillboard.mVelocity = pVelocity;
	lPebbleBillboard.mAngularVelocity = Random::Uniform(-pAngularVelocity, +pAngularVelocity);
	lPebbleBillboard.mOpacityFactor = pOpacity;
	//lPebbleBillboard.mOpacityTime += OPACITY_FADE_IN_TIME_OFFSET * 0.5f;	// Move forward some in time to ensure that we're very visible when we come out of the pipe.
}

void ParticleRenderer::CreateGlow(float pTime, float pScale, const Vector3DF& pStartColor, const Vector3DF& pColor, float pOpacity, const Vector3DF& pPosition, const Vector3DF& pVelocity)
{
	const float lTimeFactor = PARTICLE_TIME/pTime;
	CreateBillboards(pPosition, 0, Vector3DF(), Vector3DF(), 0, lTimeFactor, pScale, pStartColor, pColor, mFires, 1);
	Billboard& lGlowBillboard = mFires.back();
	lGlowBillboard.mOpacityFactor = pOpacity;
	lGlowBillboard.mVelocity = pVelocity;
	lGlowBillboard.mTargetVelocity = pVelocity;
}

void ParticleRenderer::RenderFireBillboard(float pAngle, float pSize, const Vector3DF& pColor, float pOpacity, const Vector3DF& pPosition)
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



void ParticleRenderer::CreateTempLight(const Vector3DF& pColor, float pStrength, const Vector3DF& pPosition, const Vector3DF& pVelocity, const Vector3DF& pTargetVelocity, float pTimeFactor)
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

void ParticleRenderer::CreateBillboards(const Vector3DF& pPosition, float pStrength, const Vector3DF& pDirection, const Vector3DF& pTargetVelocity,
	float pEndTurbulence, float pTimeFactor, float pSizeFactor, const Vector3DF& pStartColor, const Vector3DF& pColor, BillboardArray& pBillboards, int pCount)
{
	for (int x = 0; x < pCount; ++x)
	{
		pBillboards.push_back(Billboard());
		Billboard& lBillboard = pBillboards.back();
		lBillboard.mVelocity = RNDVEC(pStrength) + pDirection*pStrength * 1.2f;
		const Vector3DF lThisParticlesTargetVelocity = Math::Lerp(pTargetVelocity*0.8f, pTargetVelocity, Random::Uniform(0.0f, 2.0f));
		lBillboard.mTargetVelocity = RNDVEC(pEndTurbulence) + lThisParticlesTargetVelocity;
		lBillboard.mPosition = pPosition + lBillboard.mVelocity * 0.05f + pDirection*0.5f;
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
			deb_assert(x->mOpacityTime > PIF);	// Verify that the particle was visible at all, or the algo's wrong.
			x = pBillboards.erase(x);
		}
		else
		{
			++x;
		}
	}
}



LOG_CLASS_DEFINE(UI_GFX_3D, ParticleRenderer);



}
