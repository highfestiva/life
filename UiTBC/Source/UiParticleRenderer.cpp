
// Author: Jonas Byström
// Copyright (c) 2002-2013, Pixel Doctrine



#include "../Include/UiParticleRenderer.h"
#include <algorithm>
#include "../../Lepra/Include/ImageLoader.h"
#include "../../Lepra/Include/Random.h"
#include "../Include/UiRenderer.h"



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

	SetCamSpaceDepth(mFires, lCam, lCamOrientationInverse);
	SetCamSpaceDepth(mSmokes, lCam, lCamOrientationInverse);
	SetCamSpaceDepth(mSparks, lCam, lCamOrientationInverse);
	SetCamSpaceDepth(mShrapnels, lCam, lCamOrientationInverse);
	std::sort(mFires.begin(), mFires.end(), CompareDepths);
	std::sort(mSmokes.begin(), mSmokes.end(), CompareDepths);
	std::sort(mSparks.begin(), mSparks.end(), CompareDepths);
	std::sort(mShrapnels.begin(), mShrapnels.end(), CompareDepths);

	glMatrixMode(GL_MODELVIEW);
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

	const float q = 1;
	BillboardArray::iterator x;

	glEnable(GL_TEXTURE_2D);
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	x = mSmokes.begin();
	for (; x != mSmokes.end(); ++x)
	{
		TransformationF lCamSpace;
		QuaternionF lRot;
		lRot *= lCam.GetOrientation();
		lRot.RotateAroundOwnY(x->mAngle);
		lCamSpace.FastInverseTransform(lCam, lCamOrientationInverse, TransformationF(lRot, x->mPosition));
		float lModelViewMatrix[16];
		lCamSpace.GetAs4x4TransposeMatrix(lModelViewMatrix);
		glLoadMatrixf(lModelViewMatrix);

		const float s = (q + x->mOpacityTime) * x->mSizeFactor;
		const float rgb = Math::Lerp(0.4f, 0.2f, x->mOpacityTime/PIF);
		glColor4f(rgb, rgb, rgb, x->mOpacity);
		const float lOffsetX = x->mTextureIndex * 0.25f;
		glBegin(GL_QUADS);
		glTexCoord2f(lOffsetX+0, 0);
		glVertex3f(-s, 0, +s);
		glTexCoord2f(lOffsetX+0, 1);
		glVertex3f(-s, 0, -s);
		glTexCoord2f(lOffsetX+0.25f, 1);
		glVertex3f(+s, 0, -s);
		glTexCoord2f(lOffsetX+.25f, 0);
		glVertex3f(+s, 0, +s);
		glEnd();
	}

	glDisable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	x = mShrapnels.begin();
	for (; x != mShrapnels.end(); ++x)
	{
		TransformationF lCamSpace;
		QuaternionF lRot;
		lRot *= lCam.GetOrientation();
		lRot.RotateAroundOwnY(x->mAngle);
		lCamSpace.FastInverseTransform(lCam, lCamOrientationInverse, TransformationF(lRot, x->mPosition));
		float lModelViewMatrix[16];
		lCamSpace.GetAs4x4TransposeMatrix(lModelViewMatrix);
		glLoadMatrixf(lModelViewMatrix);

		const float s = x->mSizeFactor;
		glColor4f(0.3f, 0.3f, 0.3f, x->mOpacity);
		glBegin(GL_QUADS);
		glVertex3f(-s, 0, +s*2.0f);
		glVertex3f(-s, 0, -s*2.0f);
		glVertex3f(+s, 0, -s*1.2f);
		glVertex3f(+s, 0, +s*1.8f);
		glEnd();
	}

	glDisable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glColor3f(0, 0, 0);
	x = mSparks.begin();
	for (; x != mSparks.end(); ++x)
	{
		TransformationF lCamSpace;
		QuaternionF lRot;
		lRot *= lCam.GetOrientation();
		// Spark always points in same direction as its velocity. Thus we transform velocity into
		// camera space and project onto camera's XZ plane.
		const Vector3DF lCameraXZPlane(0,1,0);	// In cam space that is.
		const Vector3DF lAngleVector = (lCamOrientationInverse * x->mVelocity).ProjectOntoPlane(lCameraXZPlane);
		const float lAngle = lAngleVector.GetPolarCoordAngleY();
		lRot.RotateAroundOwnY(PIF/2 - lAngle);
		lCamSpace.FastInverseTransform(lCam, lCamOrientationInverse, TransformationF(lRot, x->mPosition));
		float lModelViewMatrix[16];
		lCamSpace.GetAs4x4TransposeMatrix(lModelViewMatrix);
		glLoadMatrixf(lModelViewMatrix);

		const float s = x->mSizeFactor;
		const float r  = Math::Lerp(1.0f, 0.6f, x->mOpacityTime/PIF);
		const float gb = Math::Lerp(1.0f, 0.3f, x->mOpacityTime/PIF);
		glColor4f(r, gb, gb, x->mOpacity);
		glBegin(GL_QUADS);
		glVertex3f(0, 0, +s*0.3f);
		glVertex3f(-s*0.4f, 0, 0);
		glVertex3f(0, 0, -s*4.5f);
		glVertex3f(+s*0.4f, 0, 0);
		glEnd();
	}

	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	x = mFires.begin();
	for (; x != mFires.end(); ++x)
	{
		TransformationF lCamSpace;
		QuaternionF lRot;
		lRot *= lCam.GetOrientation();
		lRot.RotateAroundOwnY(x->mAngle);
		lCamSpace.FastInverseTransform(lCam, lCamOrientationInverse, TransformationF(lRot, x->mPosition));
		float lModelViewMatrix[16];
		lCamSpace.GetAs4x4TransposeMatrix(lModelViewMatrix);
		glLoadMatrixf(lModelViewMatrix);

		const float s = (q + x->mOpacityTime) * x->mSizeFactor;
		const float r = Math::Lerp(1.0f, 0.6f, x->mOpacityTime/PIF);
		const float g = Math::Lerp(1.0f, 0.4f, x->mOpacityTime/PIF);
		const float b = Math::Lerp(0.3f, 0.2f, x->mOpacityTime/PIF);
		glColor4f(r, g, b, x->mOpacity);
		const float lOffsetX = x->mTextureIndex * 0.25f;
		glBegin(GL_QUADS);
		glTexCoord2f(lOffsetX+0, 0);
		glVertex3f(-s, 0, +s);
		glTexCoord2f(lOffsetX+0, 1);
		glVertex3f(-s, 0, -s);
		glTexCoord2f(lOffsetX+0.25f, 1);
		glVertex3f(+s, 0, -s);
		glTexCoord2f(lOffsetX+0.25f, 0);
		glVertex3f(+s, 0, +s);
		glEnd();
	}

	glDisable(GL_TEXTURE_2D);
}

void ParticleRenderer::Tick(float pTime)
{
	StepBillboards(mFires, pTime, 6);
	StepBillboards(mSmokes, pTime, 5);
	StepBillboards(mSparks, pTime, -0.4f);
	StepBillboards(mShrapnels, pTime, -0.2f);
}

void ParticleRenderer::CreateExplosion(const Vector3DF& pPosition, float pStrength, const Vector3DF& pDirection, const Vector3DF& pEllipsoidNorthPole, float pEllipsoidRatio, int pFires, int pSmokes, int pSparks, int pShrapnels)
{
	(void)pEllipsoidNorthPole;
	(void)pEllipsoidRatio;

	const float lRandomXYEndSpeed = 1.0f;
	CreateBillboards(pPosition, pStrength* 7, pDirection, Vector3DF(0, 0,  +9), lRandomXYEndSpeed, 3.5f, pStrength*0.4f, mFires, pFires);
	CreateBillboards(pPosition, pStrength* 8, pDirection, Vector3DF(0, 0,  +5), lRandomXYEndSpeed,    2, pStrength*0.8f, mSmokes, pSmokes);
	CreateBillboards(pPosition, pStrength*10, pDirection, Vector3DF(0, 0,  -8), lRandomXYEndSpeed,    3, ::sqrtf(pStrength)*0.25f, mSparks, pSparks);
	CreateBillboards(pPosition, pStrength* 5, pDirection, Vector3DF(0, 0, -10), lRandomXYEndSpeed,    1, ::sqrtf(pStrength)*0.10f, mShrapnels, pShrapnels);
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
		lBillboard.mTextureIndex = Random::GetRandomNumber() & 3;
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

void ParticleRenderer::SetCamSpaceDepth(BillboardArray& pBillboards, const TransformationF& pCam, const QuaternionF& pCamOrientationInverse)
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

bool ParticleRenderer::CompareDepths(const Billboard& b0, const Billboard& b1)
{
	return b0.mDepth < b1.mDepth;
}



}
