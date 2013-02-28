
// Author: Jonas Byström
// Copyright (c) 2002-2013, Pixel Doctrine



#include "../Include/UiParticleRenderer.h"
#include "../../Lepra/Include/Random.h"
#include "../Include/UiBillboardGeometry.h"



namespace UiTbc
{



ParticleRenderer::ParticleRenderer(Renderer* pRenderer, int pMaxLightCount):
	Parent(pRenderer),
	mMaxLightCount(pMaxLightCount),
	mTextureCount(1),
	mBillboardGas(0),
	mBillboardShrapnel(0),
	mBillboardSpark(0)
{
}

ParticleRenderer::~ParticleRenderer()
{
}

void ParticleRenderer::SetData(int pTextureCount, BillboardGeometry* pGas, BillboardGeometry* pShrapnel, BillboardGeometry* pSpark)
{
	mTextureCount = pTextureCount;
	mBillboardGas = pGas;
	mBillboardShrapnel = pShrapnel;
	mBillboardSpark = pSpark;
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
		const float rgb = Math::Lerp(0.4f, 0.2f, x->mOpacityTime/PIF);
		lBillboards.push_back(BillboardRenderInfo(x->mAngle, x->mPosition, s, Vector3DF(rgb, rgb, rgb), x->mOpacity, x->mTextureIndex));
	}
	mRenderer->RenderBillboards(mBillboardGas, true, false, lBillboards);

	lBillboards.clear();
	x = mShrapnels.begin();
	for (; x != mShrapnels.end(); ++x)
	{
		const float s = x->mSizeFactor;
		const float rgb = 0.3f;
		lBillboards.push_back(BillboardRenderInfo(x->mAngle, x->mPosition, s, Vector3DF(rgb, rgb, rgb), x->mOpacity, x->mTextureIndex));
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
		const float r  = Math::Lerp(1.0f, 0.6f, x->mOpacityTime/PIF);
		const float gb = Math::Lerp(1.0f, 0.3f, x->mOpacityTime/PIF);
		lBillboards.push_back(BillboardRenderInfo(lAngle, x->mPosition, s, Vector3DF(r, gb, gb), x->mOpacity, x->mTextureIndex));
	}
	mRenderer->RenderBillboards(mBillboardSpark, false, true, lBillboards);

	lBillboards.clear();
	x = mFires.begin();
	for (; x != mFires.end(); ++x)
	{
		const float s = (q + x->mOpacityTime) * x->mSizeFactor;
		const float r = Math::Lerp(1.0f, 0.6f, x->mOpacityTime/PIF);
		const float g = Math::Lerp(1.0f, 0.4f, x->mOpacityTime/PIF);
		const float b = Math::Lerp(0.3f, 0.2f, x->mOpacityTime/PIF);
		lBillboards.push_back(BillboardRenderInfo(x->mAngle, x->mPosition, s, Vector3DF(r, g, b), x->mOpacity, x->mTextureIndex));
	}
	mRenderer->RenderBillboards(mBillboardGas, true, true, lBillboards);

	// Update lights.
	LightArray::iterator y = mLights.begin();
	for (; y < mLights.end(); ++y)
	{
		if (y->mRenderLightId != Renderer::INVALID_LIGHT)
		{
			mRenderer->SetLightPosition(y->mRenderLightId, y->mPosition);
			mRenderer->SetLightColor(y->mRenderLightId, Vector3DF(0.6f, 0.4f, 0.2f) * y->mStrength * 30.0f);
			assert(y->mStrength >= 0 && y->mStrength < 10);
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
}

void ParticleRenderer::CreateFlare(float pStrength, float pTimeFactor, const Vector3DF& pPosition, const Vector3DF& pVelocity)
{
	CreateTempLight(pStrength, pPosition, pVelocity, pVelocity, pTimeFactor);
}

void ParticleRenderer::CreateExplosion(const Vector3DF& pPosition, float pStrength, const Vector3DF& pDirection, float pFalloff, const Vector3DF& pEllipsoidNorthPole, float pEllipsoidRatio, int pFires, int pSmokes, int pSparks, int pShrapnels)
{
	(void)pEllipsoidNorthPole;
	(void)pEllipsoidRatio;

	const float lRandomXYEndSpeed = 1.0f;
	CreateBillboards(pPosition, pStrength* 7, pDirection, Vector3DF(0, 0,  +9)*pFalloff, lRandomXYEndSpeed, 3.5f, pStrength*0.4f, mFires, pFires);
	CreateBillboards(pPosition, pStrength* 8, pDirection, Vector3DF(0, 0,  +5)*pFalloff, lRandomXYEndSpeed,    2, pStrength*0.8f, mSmokes, pSmokes);
	CreateBillboards(pPosition, pStrength*10, pDirection, Vector3DF(0, 0,  -8)*pFalloff, lRandomXYEndSpeed,    3, ::sqrtf(pStrength)*0.25f, mSparks, pSparks);
	CreateBillboards(pPosition, pStrength* 5, pDirection, Vector3DF(0, 0, -10)*pFalloff, lRandomXYEndSpeed,    1, ::sqrtf(pStrength)*0.10f, mShrapnels, pShrapnels);

	if (pFires > 0)
	{
		const Billboard& lFire = mFires.back();
		CreateTempLight(pStrength, lFire.mPosition, lFire.mVelocity, lFire.mTargetVelocity, lFire.mTimeFactor);
	}
}



void ParticleRenderer::CreateTempLight(float pStrength, const Vector3DF& pPosition, const Vector3DF& pVelocity, const Vector3DF& pTargetVelocity, float pTimeFactor)
{
	if (mLights.size() < mMaxLightCount)
	{
		mLights.push_back(Light(pStrength, pPosition, pVelocity, pTargetVelocity, pTimeFactor));
		mLog.Infof(_T("Creating new light with strength %f"), pStrength);
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
			mLog.Infof(_T("Overtaking light with render ID %i (had strength %f, got strength %f)"), mLights[lDarkestLightIndex].mRenderLightId, mLights[lDarkestLightIndex].mStrength, pStrength);
			// TRICKY: don't overwrite! We must not leak the previosly allocated hardware light!
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
				mLog.Infof(_T("Dropping light with render ID %i."), x->mRenderLightId);
				mRenderer->RemoveLight(x->mRenderLightId);
			}
			x = mLights.erase(x);
		}
		else
		{
			if (x->mRenderLightId == Renderer::INVALID_LIGHT)
			{
				x->mRenderLightId = mRenderer->AddPointLight(Renderer::LIGHT_MOVABLE, x->mPosition, Vector3DF(1,1,1)*x->mStrength*10, x->mStrength*10, 0);
				mLog.Infof(_T("Creating render ID %i for light with strength %f"), x->mRenderLightId, x->mStrength);
			}
			++x;
		}
	}
}

void ParticleRenderer::CreateBillboards(const Vector3DF& pPosition, float pStrength, const Vector3DF& pDirection, const Vector3DF& pTargetVelocity,
	float pEndTurbulence, float pTimeFactor, float pSizeFactor, BillboardArray& pBillboards, int pCount)
{
	for (int x = 0; x < pCount; ++x)
	{
		pBillboards.push_back(Billboard());
		Billboard& lBillboard = pBillboards.back();
		lBillboard.mVelocity = RNDVEC(pStrength) + pDirection*pStrength * 1.2f;
		const Vector3DF lThisParticlesTargetVelocity = Math::Lerp(pTargetVelocity*0.8f, pTargetVelocity, Random::Uniform(0.0f, 2.0f));
		lBillboard.mTargetVelocity = RNDVEC(pEndTurbulence) + lThisParticlesTargetVelocity;
		lBillboard.mPosition = pPosition + lBillboard.mVelocity * 0.05f + pDirection*0.5f;
		lBillboard.mTextureIndex = Random::GetRandomNumber() % mTextureCount;
		lBillboard.mDepth = 1000.0f;
		lBillboard.mSizeFactor = Random::Uniform(pSizeFactor*0.7f, pSizeFactor*1.4f);
		lBillboard.mAngle = Random::Uniform(0.0f, 2*PIF);
		lBillboard.mAngularVelocity = Random::Uniform(-5.0f, +5.0f);
		lBillboard.mOpacity = 0;
		lBillboard.mOpacityTime = Random::Uniform(0.01f, 0.3f);
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
		x->mOpacity = ::sin(x->mOpacityTime) + 0.7f;
		if (x->mOpacity <= 0.1f)
		{
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
