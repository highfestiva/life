
// Author: Jonas Byström
// Copyright (c) 2002-2013, Pixel Doctrine



#include "../Include/UiParticleRenderer.h"
#include <algorithm>
#include "../../Lepra/Include/ImageLoader.h"
#include "../../Lepra/Include/Random.h"
#include "../Include/UiRenderer.h"

#define RNDVEC(q)	Vector3DF((float)Random::Uniform(-q, +q), (float)Random::Uniform(-q, +q), (float)Random::Uniform(-q, +q))



namespace UiTbc
{



Canvas gImage;
unsigned gTextureId = 0;



ParticleRenderer::ParticleRenderer(Renderer* pRenderer):
	Parent(pRenderer)
{
}

ParticleRenderer::~ParticleRenderer()
{
}

#include "../../UiLepra/Include/UiOpenGLExtensions.h"
void ParticleRenderer::Render()
{
	if (!gTextureId)
	{
		gTextureId = 10000;
		ImageLoader lLoader;
		if (lLoader.Load(_T("Data/explosion.png"), gImage))
		{
			glGenTextures(1, &gTextureId);
			glBindTexture(GL_TEXTURE_2D, gTextureId);
			glTexImage2D(GL_TEXTURE_2D, 
				     0,
				     GL_RGBA,
				     gImage.GetWidth(),
				     gImage.GetHeight(),
				     0,
				     GL_RGBA,
				     GL_UNSIGNED_BYTE,
				     gImage.GetBuffer());
		}
	}

	const TransformationF& lCam = mRenderer->GetCameraTransformation();
	const QuaternionF& lCamOrientationInverse = mRenderer->GetCameraOrientationInverse();

	SetDepth(mFires, lCam, lCamOrientationInverse);
	SetDepth(mSmokes, lCam, lCamOrientationInverse);
	SetDepth(mSparks, lCam, lCamOrientationInverse);
	SetDepth(mShrapnels, lCam, lCamOrientationInverse);
	std::sort(mFires.begin(), mFires.end(), CompareDepths);
	std::sort(mSmokes.begin(), mSmokes.end(), CompareDepths);
	std::sort(mSparks.begin(), mSparks.end(), CompareDepths);
	std::sort(mShrapnels.begin(), mShrapnels.end(), CompareDepths);

	TransformationF lCamSpace;
	lCamSpace.FastInverseTransform(lCam, lCamOrientationInverse, TransformationF(QuaternionF(), Vector3DF()));
	float lModelViewMatrix[16];
	lCamSpace.GetAs4x4TransposeMatrix(lModelViewMatrix);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(lModelViewMatrix);

	glDisable(GL_COLOR_LOGIC_OP);
	glDisable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glShadeModel(GL_SMOOTH);
	glDepthFunc(GL_LESS);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

	mRenderer->ResetAmbientLight(true);

	const float lAmbient[]  = { 0.4f, 0.4f, 0.4f, 1 };
	const float lDiffuse[]  = { 0.5f, 0.5f, 0.5f, 1 };
	const float lSpecular[] = { 0, 0, 0, 1 };

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, lAmbient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, lDiffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, lSpecular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.0f);
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_NORMALIZE);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindTexture(GL_TEXTURE_2D, gTextureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	const float q = 4;
	BillboardArray::iterator x;

	glEnable(GL_TEXTURE_2D);
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	x = mSmokes.begin();
	for (; x != mSmokes.end(); ++x)
	{
		const float s = q + x->mOpacityTime;
		Vector3DF lPos = x->mPosition;
		const float rgb = Math::Lerp(0.4f, 0.2f, x->mOpacityTime/PIF);
		glColor4f(rgb, rgb, rgb, x->mOpacity);
		const float lOffsetX = x->mTextureIndex * 0.25f;
		glBegin(GL_QUADS);
		glTexCoord2f(lOffsetX+0, 0);
		glVertex3f(lPos.x-s, lPos.y, lPos.z+s);
		glTexCoord2f(lOffsetX+0, 1);
		glVertex3f(lPos.x-s, lPos.y, lPos.z-s);
		glTexCoord2f(lOffsetX+0.25f, 1);
		glVertex3f(lPos.x+s, lPos.y, lPos.z-s);
		glTexCoord2f(lOffsetX+.25f, 0);
		glVertex3f(lPos.x+s, lPos.y, lPos.z+s);
		glEnd();
	}

	glDisable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	x = mShrapnels.begin();
	for (; x != mShrapnels.end(); ++x)
	{
		Vector3DF lPos = x->mPosition;
		glColor4f(0, 0, 0, x->mOpacity);
		glBegin(GL_QUADS);
		glVertex3f(lPos.x-.4f, lPos.y, lPos.z+.4f);
		glVertex3f(lPos.x-.4f, lPos.y, lPos.z-.4f);
		glVertex3f(lPos.x+.4f, lPos.y, lPos.z-.4f);
		glVertex3f(lPos.x+.4f, lPos.y, lPos.z+.4f);
		glEnd();
	}

	glDisable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glColor3f(0, 0, 0);
	x = mSparks.begin();
	for (; x != mSparks.end(); ++x)
	{
		Vector3DF lPos = x->mPosition;
		const float r  = Math::Lerp(1.0f, 0.6f, x->mOpacityTime/PIF);
		const float gb = Math::Lerp(1.0f, 0.3f, x->mOpacityTime/PIF);
		glColor4f(r, gb, gb, x->mOpacity);
		glBegin(GL_QUADS);
		glVertex3f(lPos.x-.1f, lPos.y, lPos.z+.1f);
		glVertex3f(lPos.x-.1f, lPos.y, lPos.z-.1f);
		glVertex3f(lPos.x+.1f, lPos.y, lPos.z-.1f);
		glVertex3f(lPos.x+.1f, lPos.y, lPos.z+.1f);
		glEnd();
	}

	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	x = mFires.begin();
	for (; x != mFires.end(); ++x)
	{
		const float s = (q + x->mOpacityTime) * 0.5f;
		Vector3DF lPos = x->mPosition;
		const float r = Math::Lerp(1.0f, 0.6f, x->mOpacityTime/PIF);
		const float g = Math::Lerp(1.0f, 0.4f, x->mOpacityTime/PIF);
		const float b = Math::Lerp(0.3f, 0.2f, x->mOpacityTime/PIF);
		glColor4f(r, g, b, x->mOpacity);
		const float lOffsetX = x->mTextureIndex * 0.25f;
		glBegin(GL_QUADS);
		glTexCoord2f(lOffsetX+0, 0);
		glVertex3f(lPos.x-s, lPos.y, lPos.z+s);
		glTexCoord2f(lOffsetX+0, 1);
		glVertex3f(lPos.x-s, lPos.y, lPos.z-s);
		glTexCoord2f(lOffsetX+0.25f, 1);
		glVertex3f(lPos.x+s, lPos.y, lPos.z-s);
		glTexCoord2f(lOffsetX+0.25f, 0);
		glVertex3f(lPos.x+s, lPos.y, lPos.z+s);
		glEnd();
	}

	glDisable(GL_TEXTURE_2D);
}

void ParticleRenderer::Tick(float pTime)
{
	StepBillboards(mFires, pTime, 10);
	StepBillboards(mSmokes, pTime, 5);
	StepBillboards(mSparks, pTime, -0.6f);
	StepBillboards(mShrapnels, pTime, -0.2f);
}

void ParticleRenderer::CreateExplosion(const Vector3DF& pPosition, float pVelocity, const Vector3DF& pDirection, const Vector3DF& pEllipsoidNorthPole, float pEllipsoidRatio, int pFires, int pSmokes, int pSparks, int pShrapnels)
{
	(void)pEllipsoidNorthPole;
	(void)pEllipsoidRatio;

	const float lRandomXYEndSpeed = 1.0f;
	CreateBillboards(pPosition, pVelocity*7, pDirection, Vector3DF(0, 0,  +9), lRandomXYEndSpeed, 3.5f, mFires, pFires);
	CreateBillboards(pPosition, pVelocity*8, pDirection, Vector3DF(0, 0,  +5), lRandomXYEndSpeed, 2, mSmokes, pSmokes);
	CreateBillboards(pPosition, pVelocity*6, pDirection, Vector3DF(0, 0,  -7), lRandomXYEndSpeed, 1, mSparks, pSparks);
	CreateBillboards(pPosition, pVelocity*4, pDirection, Vector3DF(0, 0, -10), lRandomXYEndSpeed, 1, mShrapnels, pShrapnels);
}

void ParticleRenderer::StepBillboards(BillboardArray& pBillboards, float pTime, float pFriction)
{
	BillboardArray::iterator x = pBillboards.begin();
	while (x != pBillboards.end())
	{
		if (pFriction > 0)
		{
			x->mVelocity = Math::Lerp(x->mVelocity, x->mTargetVelocity, pFriction*pTime);

			x->mAngularVelocity /= pFriction*pTime;
			x->mAngle += x->mAngularVelocity * pTime;
		}
		else
		{
			x->mVelocity *= 1 + pFriction*pTime;
			x->mVelocity += x->mTargetVelocity * pTime;
		}
		x->mPosition += x->mVelocity * pTime;

		x->mOpacityTime += pTime * x->mTimeFactor;
		x->mOpacity = ::sin(x->mOpacityTime) + 0.7f;
		if (x->mOpacity < 0)
		{
			x = pBillboards.erase(x);
		}
		else
		{
			++x;
		}
	}
}

void ParticleRenderer::SetDepth(BillboardArray& pBillboards, const TransformationF& pCam, const QuaternionF& pCamOrientationInverse)
{
	const Vector3DF& lCamPosition = pCam.GetPosition();
	const QuaternionF& lCamOrientation = pCam.GetOrientation();
	Vector3DF lDepth;
	BillboardArray::iterator x = pBillboards.begin();
	for (; x != pBillboards.end(); ++x)
	{
		lCamOrientation.FastInverseRotatedVector(pCamOrientationInverse, lDepth, x->mPosition - lCamPosition);
		x->mDepth = lDepth.y;
	}
}

void ParticleRenderer::CreateBillboards(const Vector3DF& pPosition, float pVelocity, const Vector3DF& pDirection, const Vector3DF& pTargetVelocity,
	float pEndTurbulence, float pTimeFactor, BillboardArray& pBillboards, int pCount)
{
	for (int x = 0; x < pCount; ++x)
	{
		pBillboards.push_back(Billboard());
		Billboard& lBillboard = pBillboards.back();
		lBillboard.mVelocity = RNDVEC(pVelocity) + pDirection*pVelocity * 1.2f;
		const Vector3DF lThisParticlesTargetVelocity = Math::Lerp(pTargetVelocity*0.8f, pTargetVelocity, (float)Random::Uniform(0, 2));
		lBillboard.mTargetVelocity = RNDVEC(pEndTurbulence) + lThisParticlesTargetVelocity;
		lBillboard.mPosition = pPosition + lBillboard.mVelocity * 0.05f + pDirection*0.5f;
		lBillboard.mTextureIndex = Random::GetRandomNumber() & 3;
		lBillboard.mDepth = 1000.0f;
		lBillboard.mAngle = (float)Random::Uniform(0, 2*PIF);
		lBillboard.mAngularVelocity = (float)Random::Uniform(-pVelocity, +pVelocity) * 0.1f;
		lBillboard.mOpacity = 0;
		lBillboard.mOpacityTime = (float)Random::Uniform(0, 0.5f);
		lBillboard.mTimeFactor = (float)Random::Uniform(pTimeFactor*0.7f, pTimeFactor*1.3f);
	}
}

bool ParticleRenderer::CompareDepths(const Billboard& b0, const Billboard& b1)
{
	return b0.mDepth < b1.mDepth;
}



}
