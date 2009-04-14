/*
	Lepra::File:   Edge.cpp
	Class:  Edge
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand
*/

#include "../../Lepra/Include/Lepra.h"
#include "../Include/UiEdge.h"
#include "../Include/UiVertex.h"
#include "../Include/UiGradients.h"
#include "../../Lepra/Include/GammaLookup.h"
#include "../../Lepra/Include/FixedPointMath.h"

#include <math.h>

namespace UiTbc
{

int Edge::smClipLeft = 0;
int Edge::smClipRight = 0;

void Edge::Init(const Vertex3D* pV0, const Vertex3D* pV1)
{
/*	if (pV1->GetY28_4() < pV0->GetY28_4())
	{
		const Vertex3D* lT = pV0;
		pV0 = pV1;
		pV1 = lT;
	}
*/
	mY = Lepra::FixedPointMath::Ceil28_4(pV0->GetY28_4());
	mHeight = Lepra::FixedPointMath::Ceil28_4(pV1->GetY28_4()) - mY;

	if (mHeight > 0)
	{
		int lDX = pV1->GetX28_4() - pV0->GetX28_4();
		int lDY = pV1->GetY28_4() - pV0->GetY28_4();

		Lepra::FixedPointMath::Fixed28_4 lT1 = Lepra::FixedPointMath::Fixed28_4Mul(lDX, (mY << 4) - pV0->GetY28_4());
		Lepra::FixedPointMath::Fixed28_4 lT2 = Lepra::FixedPointMath::Fixed28_4Mul(lDY, pV0->GetX28_4() + 16);

		int lInitialNumerator = lT1 + lT2 - 1;
/*		int lInitialNumerator = lDX * ((mY << 4) - pV0->GetY28_4()) +
								  lDY * (pV0->GetX28_4() + 16) - 1;
*/
		Lepra::FixedPointMath::FloorDivMod(lInitialNumerator, lDY, mX, mErrorTerm);
		Lepra::FixedPointMath::FloorDivMod(lDX, lDY, mXStep, mNumerator);
		
		mDenominator = lDY;
	}

	mStartX = mX;
	mStartY = mY;
	mStartHeight = mHeight;
	mStartErrorTerm = mErrorTerm;
}

void Edge::Init(const Vertex2D* pV0, const Vertex2D* pV1)
{
	if (pV1->GetY28_4() < pV0->GetY28_4())
	{
		const Vertex2D* lT = pV0;
		pV0 = pV1;
		pV1 = lT;
	}

	mY = Lepra::FixedPointMath::Ceil28_4(pV0->GetY28_4());
	mHeight = Lepra::FixedPointMath::Ceil28_4(pV1->GetY28_4()) - mY;

	if (mHeight > 0)
	{
		int lDX = pV1->GetX28_4() - pV0->GetX28_4();
		int lDY = pV1->GetY28_4() - pV0->GetY28_4();

		Lepra::FixedPointMath::Fixed28_4 lT1 = Lepra::FixedPointMath::Fixed28_4Mul(lDX, (mY << 4) - pV0->GetY28_4());
		Lepra::FixedPointMath::Fixed28_4 lT2 = Lepra::FixedPointMath::Fixed28_4Mul(lDY, pV0->GetX28_4() + 16);

		int lInitialNumerator = lT1 + lT2 - 1;

		Lepra::FixedPointMath::FloorDivMod(lInitialNumerator, lDY, mX, mErrorTerm);
		Lepra::FixedPointMath::FloorDivMod(lDX, lDY, mXStep, mNumerator);
		
		mDenominator = lDY;
	}

	mStartX = mX;
	mStartY = mY;
	mStartHeight = mHeight;
	mStartErrorTerm = mErrorTerm;
}

void EdgeZ::Init(const Vertex3D* pV0, 
					const Vertex3D* pV1,
					const Gradients* pGradients)
{
	if (pV1->GetY28_4() < pV0->GetY28_4())
	{
		const Vertex3D* lT = pV0;
		pV0 = pV1;
		pV1 = lT;
	}

	Edge::Init(pV0, pV1);

	mXPrestep = (float)Lepra::FixedPointMath::Fixed28_4ToFloat(mX * 16 - pV0->GetX28_4());
	mYPrestep = (float)Lepra::FixedPointMath::Fixed28_4ToFloat(mY * 16 - pV0->GetY28_4());

	mStartOneOverZ = pV0->GetOneOverZ();

	Reset(pGradients);
}

void EdgeZ::Reset(const Gradients* pGradients)
{
	Edge::Reset();

	mOneOverZ = mStartOneOverZ +
				  mXPrestep * pGradients->GetOneOverZXStep() +
				  mYPrestep * pGradients->GetOneOverZYStep();
	mOneOverZStep = (float)mXStep * pGradients->GetOneOverZXStep() + pGradients->GetOneOverZYStep();
	mOneOverZStepExtra = pGradients->GetOneOverZXStep();

	mGradients = pGradients;
}



void EdgeUV::Init(const Vertex3DUV* pV0, 
					 const Vertex3DUV* pV1,
					 const GradientsUV* pGradients)
{
	if (pV1->GetY28_4() < pV0->GetY28_4())
	{
		const Vertex3DUV* lT = pV0;
		pV0 = pV1;
		pV1 = lT;
	}

	Edge::Init(pV0, pV1);

	mXPrestep = (float)Lepra::FixedPointMath::Fixed28_4ToFloat(mX * 16 - pV0->GetX28_4());
	mYPrestep = (float)Lepra::FixedPointMath::Fixed28_4ToFloat(mY * 16 - pV0->GetY28_4());

	mStartOneOverZ = pV0->GetOneOverZ();
	mStartUOverZ = pV0->GetUOverZ();
	mStartVOverZ = pV0->GetVOverZ();

	Reset(pGradients);
}

void EdgeUV::Reset(const GradientsUV* pGradients)
{
	Edge::Reset();

	float lXStep = (float)mXStep;

	mOneOverZ = mStartOneOverZ +
				  mXPrestep * pGradients->GetOneOverZXStep() +
				  mYPrestep * pGradients->GetOneOverZYStep();
	mOneOverZStep = lXStep * pGradients->GetOneOverZXStep() + pGradients->GetOneOverZYStep();
	mOneOverZStepExtra = pGradients->GetOneOverZXStep();

	mUOverZ = mStartUOverZ +
				mXPrestep * pGradients->GetUOverZXStep() +
				mYPrestep * pGradients->GetUOverZYStep();
	mUOverZStep = lXStep * pGradients->GetUOverZXStep() + pGradients->GetUOverZYStep();
	mUOverZStepExtra = pGradients->GetUOverZXStep();

	mVOverZ = mStartVOverZ +
				mXPrestep * pGradients->GetVOverZXStep() +
				mYPrestep * pGradients->GetVOverZYStep();
	mVOverZStep = lXStep * pGradients->GetVOverZXStep() + pGradients->GetVOverZYStep();
	mVOverZStepExtra = pGradients->GetVOverZXStep();

	mGradients = pGradients;
}



void EdgeRGB::Init(const Vertex3DRGB* pV0, 
					  const Vertex3DRGB* pV1,
					  const GradientsRGB* pGradients,
					  bool pGammaConvert)
{
	if (pV1->GetY28_4() < pV0->GetY28_4())
	{
		const Vertex3DRGB* lT = pV0;
		pV0 = pV1;
		pV1 = lT;
	}

	Edge::Init(pV0, pV1);

	mXPrestep = (float)Lepra::FixedPointMath::Fixed28_4ToFloat((mX << 4) - pV0->GetX28_4());
	mYPrestep = (float)Lepra::FixedPointMath::Fixed28_4ToFloat((mY << 4) - pV0->GetY28_4());

	mStartOneOverZ = pV0->GetOneOverZ();
	if (pGammaConvert == true)
	{
		mStartROverZ = Lepra::GammaLookup::GammaToLinearFloat((unsigned char)(pV0->GetR() * 255.0f)) * pV0->GetOneOverZ();
		mStartGOverZ = Lepra::GammaLookup::GammaToLinearFloat((unsigned char)(pV0->GetG() * 255.0f)) * pV0->GetOneOverZ();
		mStartBOverZ = Lepra::GammaLookup::GammaToLinearFloat((unsigned char)(pV0->GetB() * 255.0f)) * pV0->GetOneOverZ();
	}
	else
	{
		mStartROverZ = pV0->GetROverZ();
		mStartGOverZ = pV0->GetGOverZ();
		mStartBOverZ = pV0->GetBOverZ();
	}

	Reset(pGradients);
}

void EdgeRGB::Reset(const GradientsRGB* pGradients)
{
	Edge::Reset();

	float lXStep = (float)mXStep;

	mOneOverZ = mStartOneOverZ +
				  mXPrestep * pGradients->GetOneOverZXStep() +
				  mYPrestep * pGradients->GetOneOverZYStep();
	mOneOverZStep = lXStep * pGradients->GetOneOverZXStep() + pGradients->GetOneOverZYStep();
	mOneOverZStepExtra = pGradients->GetOneOverZXStep();

	mROverZ = mStartROverZ +
				mXPrestep * pGradients->GetROverZXStep() +
				mYPrestep * pGradients->GetROverZYStep();
	mROverZStep = lXStep * pGradients->GetROverZXStep() + pGradients->GetROverZYStep();
	mROverZStepExtra = pGradients->GetROverZXStep();

	mGOverZ = mStartGOverZ +
				mXPrestep * pGradients->GetGOverZXStep() +
				mYPrestep * pGradients->GetGOverZYStep();
	mGOverZStep = lXStep * pGradients->GetGOverZXStep() + pGradients->GetGOverZYStep();
	mGOverZStepExtra = pGradients->GetGOverZXStep();

	mBOverZ = mStartBOverZ +
				mXPrestep * pGradients->GetBOverZXStep() +
				mYPrestep * pGradients->GetBOverZYStep();
	mBOverZStep = lXStep * pGradients->GetBOverZXStep() + pGradients->GetBOverZYStep();
	mBOverZStepExtra = pGradients->GetBOverZXStep();

	mGradients = pGradients;
}



void EdgeUVRGB::Init(const Vertex3DUVRGB* pV0, 
						const Vertex3DUVRGB* pV1,
						const GradientsUVRGB* pGradients,
						bool pGammaConvert)
{
	if (pV1->GetY28_4() < pV0->GetY28_4())
	{
		const Vertex3DUVRGB* lT = pV0;
		pV0 = pV1;
		pV1 = lT;
	}

	Edge::Init(pV0, pV1);

	mXPrestep = (float)Lepra::FixedPointMath::Fixed28_4ToFloat((mX << 4) - pV0->GetX28_4());
	mYPrestep = (float)Lepra::FixedPointMath::Fixed28_4ToFloat((mY << 4) - pV0->GetY28_4());

	mStartOneOverZ = pV0->GetOneOverZ();
	mStartUOverZ = pV0->GetUOverZ();
	mStartVOverZ = pV0->GetVOverZ();

	if (pGammaConvert == true)
	{
		mStartROverZ = Lepra::GammaLookup::GammaToLinearFloat((unsigned char)(pV0->GetR() * 255.0f)) * pV0->GetOneOverZ();
		mStartGOverZ = Lepra::GammaLookup::GammaToLinearFloat((unsigned char)(pV0->GetG() * 255.0f)) * pV0->GetOneOverZ();
		mStartBOverZ = Lepra::GammaLookup::GammaToLinearFloat((unsigned char)(pV0->GetB() * 255.0f)) * pV0->GetOneOverZ();
	}
	else
	{
		mStartROverZ = pV0->GetROverZ();
		mStartGOverZ = pV0->GetGOverZ();
		mStartBOverZ = pV0->GetBOverZ();
	}

	Reset(pGradients);
}

void EdgeUVRGB::Reset(const GradientsUVRGB* pGradients)
{
	Edge::Reset();

	float lXStep = (float)mXStep;

	mOneOverZ = mStartOneOverZ +
				  mXPrestep * pGradients->GetOneOverZXStep() +
				  mYPrestep * pGradients->GetOneOverZYStep();
	mOneOverZStep = lXStep * pGradients->GetOneOverZXStep() + pGradients->GetOneOverZYStep();
	mOneOverZStepExtra = pGradients->GetOneOverZXStep();

	mUOverZ = mStartUOverZ +
				mXPrestep * pGradients->GetUOverZXStep() +
				mYPrestep * pGradients->GetUOverZYStep();
	mUOverZStep = lXStep * pGradients->GetUOverZXStep() + pGradients->GetUOverZYStep();
	mUOverZStepExtra = pGradients->GetUOverZXStep();

	mVOverZ = mStartVOverZ +
				mXPrestep * pGradients->GetVOverZXStep() +
				mYPrestep * pGradients->GetVOverZYStep();
	mVOverZStep = lXStep * pGradients->GetVOverZXStep() + pGradients->GetVOverZYStep();
	mVOverZStepExtra = pGradients->GetVOverZXStep();

	mROverZ = mStartROverZ +
				mXPrestep * pGradients->GetROverZXStep() +
				mYPrestep * pGradients->GetROverZYStep();
	mROverZStep = lXStep * pGradients->GetROverZXStep() + pGradients->GetROverZYStep();
	mROverZStepExtra = pGradients->GetROverZXStep();

	mGOverZ = mStartGOverZ +
				mXPrestep * pGradients->GetGOverZXStep() +
				mYPrestep * pGradients->GetGOverZYStep();
	mGOverZStep = lXStep * pGradients->GetGOverZXStep() + pGradients->GetGOverZYStep();
	mGOverZStepExtra = pGradients->GetGOverZXStep();

	mBOverZ = mStartBOverZ +
				mXPrestep * pGradients->GetBOverZXStep() +
				mYPrestep * pGradients->GetBOverZYStep();
	mBOverZStep = lXStep * pGradients->GetBOverZXStep() + pGradients->GetBOverZYStep();
	mBOverZStepExtra = pGradients->GetBOverZXStep();

	mGradients = pGradients;
}




void EdgeUVM::Init(const Vertex3DUVM* pV0, 
					  const Vertex3DUVM* pV1,
					  const GradientsUVM* pGradients)
{
	if (pV1->GetY28_4() < pV0->GetY28_4())
	{
		const Vertex3DUVM* lT = pV0;
		pV0 = pV1;
		pV1 = lT;
	}

	Edge::Init(pV0, pV1);

	mXPrestep = (float)Lepra::FixedPointMath::Fixed28_4ToFloat(mX * 16 - pV0->GetX28_4());
	mYPrestep = (float)Lepra::FixedPointMath::Fixed28_4ToFloat(mY * 16 - pV0->GetY28_4());

	mStartOneOverZ = pV0->GetOneOverZ();
	mStartUOverZ = pV0->GetUOverZ();
	mStartVOverZ = pV0->GetVOverZ();
	mStartMOverZ = pV0->GetMOverZ();

	Reset(pGradients);
}

void EdgeUVM::Reset(const GradientsUVM* pGradients)
{
	Edge::Reset();

	float lXStep = (float)mXStep;

	mOneOverZ = mStartOneOverZ +
				  mXPrestep * pGradients->GetOneOverZXStep() +
				  mYPrestep * pGradients->GetOneOverZYStep();
	mOneOverZStep = lXStep * pGradients->GetOneOverZXStep() + pGradients->GetOneOverZYStep();
	mOneOverZStepExtra = pGradients->GetOneOverZXStep();

	mUOverZ = mStartUOverZ +
				mXPrestep * pGradients->GetUOverZXStep() +
				mYPrestep * pGradients->GetUOverZYStep();
	mUOverZStep = lXStep * pGradients->GetUOverZXStep() + pGradients->GetUOverZYStep();
	mUOverZStepExtra = pGradients->GetUOverZXStep();

	mVOverZ = mStartVOverZ +
				mXPrestep * pGradients->GetVOverZXStep() +
				mYPrestep * pGradients->GetVOverZYStep();
	mVOverZStep = lXStep * pGradients->GetVOverZXStep() + pGradients->GetVOverZYStep();
	mVOverZStepExtra = pGradients->GetVOverZXStep();

	mMOverZ = mStartMOverZ +
				mXPrestep * pGradients->GetMOverZXStep() +
				mYPrestep * pGradients->GetMOverZYStep();
	mMOverZStep = lXStep * pGradients->GetMOverZXStep() + pGradients->GetMOverZYStep();
	mMOverZStepExtra = pGradients->GetMOverZXStep();

	mGradients = pGradients;
}


void EdgeUVRGBM::Init(const Vertex3DUVRGBM* pV0, 
						 const Vertex3DUVRGBM* pV1,
						 const GradientsUVRGBM* pGradients,
						 bool pGammaConvert)
{
	if (pV1->GetY28_4() < pV0->GetY28_4())
	{
		const Vertex3DUVRGBM* lT = pV0;
		pV0 = pV1;
		pV1 = lT;
	}

	Edge::Init(pV0, pV1);

	mXPrestep = (float)Lepra::FixedPointMath::Fixed28_4ToFloat((mX << 4) - pV0->GetX28_4());
	mYPrestep = (float)Lepra::FixedPointMath::Fixed28_4ToFloat((mY << 4) - pV0->GetY28_4());

	mStartOneOverZ = pV0->GetOneOverZ();
	mStartUOverZ = pV0->GetUOverZ();
	mStartVOverZ = pV0->GetVOverZ();
	mStartMOverZ = pV0->GetMOverZ();

	if (pGammaConvert == true)
	{
		mStartROverZ = Lepra::GammaLookup::GammaToLinearFloat((unsigned char)(pV0->GetR() * 255.0f)) * pV0->GetOneOverZ();
		mStartGOverZ = Lepra::GammaLookup::GammaToLinearFloat((unsigned char)(pV0->GetG() * 255.0f)) * pV0->GetOneOverZ();
		mStartBOverZ = Lepra::GammaLookup::GammaToLinearFloat((unsigned char)(pV0->GetB() * 255.0f)) * pV0->GetOneOverZ();
	}
	else
	{
		mStartROverZ = pV0->GetROverZ();
		mStartGOverZ = pV0->GetGOverZ();
		mStartBOverZ = pV0->GetBOverZ();
	}

	Reset(pGradients);
}





void EdgeUVRGBM::Reset(const GradientsUVRGBM* pGradients)
{
	Edge::Reset();

	float lXStep = (float)mXStep;

	mOneOverZ = mStartOneOverZ +
				  mXPrestep * pGradients->GetOneOverZXStep() +
				  mYPrestep * pGradients->GetOneOverZYStep();
	mOneOverZStep = lXStep * pGradients->GetOneOverZXStep() + pGradients->GetOneOverZYStep();
	mOneOverZStepExtra = pGradients->GetOneOverZXStep();

	mUOverZ = mStartUOverZ +
				mXPrestep * pGradients->GetUOverZXStep() +
				mYPrestep * pGradients->GetUOverZYStep();
	mUOverZStep = lXStep * pGradients->GetUOverZXStep() + pGradients->GetUOverZYStep();
	mUOverZStepExtra = pGradients->GetUOverZXStep();

	mVOverZ = mStartVOverZ +
				mXPrestep * pGradients->GetVOverZXStep() +
				mYPrestep * pGradients->GetVOverZYStep();
	mVOverZStep = lXStep * pGradients->GetVOverZXStep() + pGradients->GetVOverZYStep();
	mVOverZStepExtra = pGradients->GetVOverZXStep();

	mROverZ = mStartROverZ +
				mXPrestep * pGradients->GetROverZXStep() +
				mYPrestep * pGradients->GetROverZYStep();
	mROverZStep = lXStep * pGradients->GetROverZXStep() + pGradients->GetROverZYStep();
	mROverZStepExtra = pGradients->GetROverZXStep();

	mGOverZ = mStartGOverZ +
				mXPrestep * pGradients->GetGOverZXStep() +
				mYPrestep * pGradients->GetGOverZYStep();
	mGOverZStep = lXStep * pGradients->GetGOverZXStep() + pGradients->GetGOverZYStep();
	mGOverZStepExtra = pGradients->GetGOverZXStep();

	mBOverZ = mStartBOverZ +
				mXPrestep * pGradients->GetBOverZXStep() +
				mYPrestep * pGradients->GetBOverZYStep();
	mBOverZStep = lXStep * pGradients->GetBOverZXStep() + pGradients->GetBOverZYStep();
	mBOverZStepExtra = pGradients->GetBOverZXStep();

	mMOverZ = mStartMOverZ +
				mXPrestep * pGradients->GetMOverZXStep() +
				mYPrestep * pGradients->GetMOverZYStep();
	mMOverZStep = lXStep * pGradients->GetMOverZXStep() + pGradients->GetMOverZYStep();
	mMOverZStepExtra = pGradients->GetMOverZXStep();

	mGradients = pGradients;
}

















void Edge2DUV::Init(const Vertex2DUV* pV0, 
					   const Vertex2DUV* pV1,
					   const Gradients2DUV* pGradients)
{
	if (pV1->GetY28_4() < pV0->GetY28_4())
	{
		const Vertex2DUV* lT = pV0;
		pV0 = pV1;
		pV1 = lT;
	}

	Edge::Init(pV0, pV1);

	mXPrestep = (float)Lepra::FixedPointMath::Fixed28_4ToFloat(mX * 16 - pV0->GetX28_4());
	mYPrestep = (float)Lepra::FixedPointMath::Fixed28_4ToFloat(mY * 16 - pV0->GetY28_4());

	mStartU = pV0->GetU();
	mStartV = pV0->GetV();

	Reset(pGradients);
}

void Edge2DUV::Reset(const Gradients2DUV* pGradients)
{
	Edge::Reset();

	float lXStep = (float)mXStep;

	mU = mStartU + mXPrestep * pGradients->GetUXStep() +
					   mYPrestep * pGradients->GetUYStep();
	mUStep = lXStep * pGradients->GetUXStep() + pGradients->GetUYStep();
	mUStepExtra = pGradients->GetUXStep();

	mV = mStartV + mXPrestep * pGradients->GetVXStep() +
					   mYPrestep * pGradients->GetVYStep();
	mVStep = lXStep * pGradients->GetVXStep() + pGradients->GetVYStep();
	mVStepExtra = pGradients->GetVXStep();

	mGradients = pGradients;
}









void Edge2DRGBA::Init(const Vertex2DRGBA* pV0, 
					  const Vertex2DRGBA* pV1,
					  const Gradients2DRGBA* pGradients)
{
	if (pV1->GetY28_4() < pV0->GetY28_4())
	{
		const Vertex2DRGBA* lT = pV0;
		pV0 = pV1;
		pV1 = lT;
	}

	Edge::Init(pV0, pV1);

	mXPrestep = (float)Lepra::FixedPointMath::Fixed28_4ToFloat(mX * 16 - pV0->GetX28_4());
	mYPrestep = (float)Lepra::FixedPointMath::Fixed28_4ToFloat(mY * 16 - pV0->GetY28_4());

	mStartR = pV0->GetR();
	mStartG = pV0->GetG();
	mStartB = pV0->GetB();
	mStartA = pV0->GetA();

	Reset(pGradients);
}

void Edge2DRGBA::Reset(const Gradients2DRGBA* pGradients)
{
	Edge::Reset();

	float lXStep = (float)mXStep;

	mR = mStartR + mXPrestep * pGradients->GetRXStep() +
					   mYPrestep * pGradients->GetRYStep();
	mRStep = lXStep * pGradients->GetRXStep() + pGradients->GetRYStep();
	mRStepExtra = pGradients->GetRXStep();


	mG = mStartG + mXPrestep * pGradients->GetGXStep() +
					   mYPrestep * pGradients->GetGYStep();
	mGStep = lXStep * pGradients->GetGXStep() + pGradients->GetGYStep();
	mGStepExtra = pGradients->GetGXStep();


	mB = mStartB + mXPrestep * pGradients->GetBXStep() +
					   mYPrestep * pGradients->GetBYStep();
	mBStep = lXStep * pGradients->GetBXStep() + pGradients->GetBYStep();
	mBStepExtra = pGradients->GetBXStep();


	mA = mStartA + mXPrestep * pGradients->GetAXStep() +
					   mYPrestep * pGradients->GetAYStep();
	mAStep = lXStep * pGradients->GetAXStep() + pGradients->GetAYStep();
	mAStepExtra = pGradients->GetAXStep();


	mGradients = pGradients;
}

} // End namespace.
