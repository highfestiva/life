/*
	Lepra::File:   Gradients.cpp
	Class:  Gradients
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand
*/

#include "../Include/UiGradients.h"
#include "../Include/UiVertex.h"
#include "../../Lepra/Include/GammaLookup.h"

namespace UiTbc
{

Gradients::Gradients()
{
	mOneOverZXStep = 0.0f;
	mOneOverZYStep = 0.0f;
}

Gradients::Gradients(const Vertex3D* pV0,
					 const Vertex3D* pV1,
					 const Vertex3D* pV2)
{
	float lDummy1;
	float lDummy2;
	Gradients::Init(pV0, pV1, pV2, lDummy1, lDummy2);
}

void Gradients::Init(const Vertex3D* pVertex3D0,
					 const Vertex3D* pVertex3D1,
					 const Vertex3D* pVertex3D2,
					 float& pOneOverDX,
					 float& pOneOverDY)
{
	float lX0 = pVertex3D0->GetX();
	float lY0 = pVertex3D0->GetY();
	float lX1 = pVertex3D1->GetX();
	float lY1 = pVertex3D1->GetY();
	float lX2 = pVertex3D2->GetX();
	float lY2 = pVertex3D2->GetY();

	float lDY0 = lY0 - lY2;
	float lDY1 = lY1 - lY2;
	float lDX0 = lX0 - lX2;
	float lDX1 = lX1 - lX2;
	
	float lX1Y0 = lDX1 * lDY0;
	float lX0Y1 = lDX0 * lDY1;

	pOneOverDX = 1.0f / (lX1Y0 - lX0Y1);
	pOneOverDY = -pOneOverDX;

	mOneOverZXStep = pOneOverDX * (((pVertex3D1->GetOneOverZ() - pVertex3D2->GetOneOverZ()) * lDY0) -
										((pVertex3D0->GetOneOverZ() - pVertex3D2->GetOneOverZ()) * lDY1));
	mOneOverZYStep = pOneOverDY * (((pVertex3D1->GetOneOverZ() - pVertex3D2->GetOneOverZ()) * lDX0) -
										((pVertex3D0->GetOneOverZ() - pVertex3D2->GetOneOverZ()) * lDX1));
}

GradientsUV::GradientsUV(const Vertex3DUV* pV0,
						 const Vertex3DUV* pV1,
						 const Vertex3DUV* pV2)
{
	float lOneOverDX;
	float lOneOverDY;
	Gradients::Init(pV0, pV1, pV2, lOneOverDX, lOneOverDY);

	float lX0 = pV0->GetX();
	float lY0 = pV0->GetY();
	float lX1 = pV1->GetX();
	float lY1 = pV1->GetY();
	float lX2 = pV2->GetX();
	float lY2 = pV2->GetY();

	float lDY0 = lY0 - lY2;
	float lDY1 = lY1 - lY2;
	float lDX0 = lX0 - lX2;
	float lDX1 = lX1 - lX2;

	mUOverZXStep = lOneOverDX * ( (pV1->GetUOverZ() - pV2->GetUOverZ()) * lDY0 -
									  (pV0->GetUOverZ() - pV2->GetUOverZ()) * lDY1);
	mUOverZYStep = lOneOverDY * ( (pV1->GetUOverZ() - pV2->GetUOverZ()) * lDX0 -
									  (pV0->GetUOverZ() - pV2->GetUOverZ()) * lDX1);

	mVOverZXStep = lOneOverDX * ( (pV1->GetVOverZ() - pV2->GetVOverZ()) * lDY0 -
									  (pV0->GetVOverZ() - pV2->GetVOverZ()) * lDY1);
	mVOverZYStep = lOneOverDY * ( (pV1->GetVOverZ() - pV2->GetVOverZ()) * lDX0 -
									  (pV0->GetVOverZ() - pV2->GetVOverZ()) * lDX1);

	mDUDXModifier = GetModifier(pV0->GetUOverZ(),
								  mUOverZXStep,
								  mUOverZYStep,
								  pV0->GetOneOverZ());

	mDVDXModifier = GetModifier(pV0->GetVOverZ(),
								  mVOverZXStep,
								  mVOverZYStep,
								  pV0->GetOneOverZ());
}

GradientsRGB::GradientsRGB(const Vertex3DRGB* pV0, 
								 const Vertex3DRGB* pV1,
								 const Vertex3DRGB* pV2,
								 bool pGammaConvert)
{
	float lOneOverDX;
	float lOneOverDY;
	Gradients::Init(pV0, pV1, pV2, lOneOverDX, lOneOverDY);

	float lX0 = pV0->GetX();
	float lY0 = pV0->GetY();
	float lX1 = pV1->GetX();
	float lY1 = pV1->GetY();
	float lX2 = pV2->GetX();
	float lY2 = pV2->GetY();

	float lDY0 = lY0 - lY2;
	float lDY1 = lY1 - lY2;
	float lDX0 = lX0 - lX2;
	float lDX1 = lX1 - lX2;

	float lROverZ[3];
	float lGOverZ[3];
	float lBOverZ[3];

	if (pGammaConvert == true)
	{
		lROverZ[0] = Lepra::GammaLookup::GammaToLinearFloat((unsigned char)(pV0->GetR() * 255.0f)) * pV0->GetOneOverZ();
		lGOverZ[0] = Lepra::GammaLookup::GammaToLinearFloat((unsigned char)(pV0->GetG() * 255.0f)) * pV0->GetOneOverZ();
		lBOverZ[0] = Lepra::GammaLookup::GammaToLinearFloat((unsigned char)(pV0->GetB() * 255.0f)) * pV0->GetOneOverZ();

		lROverZ[1] = Lepra::GammaLookup::GammaToLinearFloat((unsigned char)(pV1->GetR() * 255.0f)) * pV1->GetOneOverZ();
		lGOverZ[1] = Lepra::GammaLookup::GammaToLinearFloat((unsigned char)(pV1->GetG() * 255.0f)) * pV1->GetOneOverZ();
		lBOverZ[1] = Lepra::GammaLookup::GammaToLinearFloat((unsigned char)(pV1->GetB() * 255.0f)) * pV1->GetOneOverZ();

		lROverZ[2] = Lepra::GammaLookup::GammaToLinearFloat((unsigned char)(pV2->GetR() * 255.0f)) * pV2->GetOneOverZ();
		lGOverZ[2] = Lepra::GammaLookup::GammaToLinearFloat((unsigned char)(pV2->GetG() * 255.0f)) * pV2->GetOneOverZ();
		lBOverZ[2] = Lepra::GammaLookup::GammaToLinearFloat((unsigned char)(pV2->GetB() * 255.0f)) * pV2->GetOneOverZ();
	}
	else
	{
		lROverZ[0] = pV0->GetROverZ();
		lGOverZ[0] = pV0->GetGOverZ();
		lBOverZ[0] = pV0->GetBOverZ();

		lROverZ[1] = pV1->GetROverZ();
		lGOverZ[1] = pV1->GetGOverZ();
		lBOverZ[1] = pV1->GetBOverZ();

		lROverZ[2] = pV2->GetROverZ();
		lGOverZ[2] = pV2->GetGOverZ();
		lBOverZ[2] = pV2->GetBOverZ();
	}

	mROverZXStep = lOneOverDX * ( (lROverZ[1] - lROverZ[2]) * lDY0 -
									  (lROverZ[0] - lROverZ[2]) * lDY1);
	mROverZYStep = lOneOverDY * ( (lROverZ[1] - lROverZ[2]) * lDX0 -
									  (lROverZ[0] - lROverZ[2]) * lDX1);

	mGOverZXStep = lOneOverDX * ( (lGOverZ[1] - lGOverZ[2]) * lDY0 -
									  (lGOverZ[0] - lGOverZ[2]) * lDY1);
	mGOverZYStep = lOneOverDY * ( (lGOverZ[1] - lGOverZ[2]) * lDX0 -
									  (lGOverZ[0] - lGOverZ[2]) * lDX1);

	mBOverZXStep = lOneOverDX * ( (lBOverZ[1] - lBOverZ[2]) * lDY0 -
									  (lBOverZ[0] - lBOverZ[2]) * lDY1);
	mBOverZYStep = lOneOverDY * ( (lBOverZ[1] - lBOverZ[2]) * lDX0 -
									  (lBOverZ[0] - lBOverZ[2]) * lDX1);

	mDRDXModifier = GetModifier(pV0->GetROverZ(),
								  mROverZXStep,
								  mROverZYStep,
								  pV0->GetOneOverZ());

	mDGDXModifier = GetModifier(pV0->GetGOverZ(),
								  mGOverZXStep,
								  mGOverZYStep,
								  pV0->GetOneOverZ());

	mDBDXModifier = GetModifier(pV0->GetBOverZ(),
								  mBOverZXStep,
								  mBOverZYStep,
								  pV0->GetOneOverZ());
}

GradientsUVRGB::GradientsUVRGB(const Vertex3DUVRGB* pV0,
									 const Vertex3DUVRGB* pV1,
									 const Vertex3DUVRGB* pV2,
									 bool pGammaConvert)
{
	float lOneOverDX;
	float lOneOverDY;
	Gradients::Init(pV0, pV1, pV2, lOneOverDX, lOneOverDY);

	float lX0 = pV0->GetX();
	float lY0 = pV0->GetY();
	float lX1 = pV1->GetX();
	float lY1 = pV1->GetY();
	float lX2 = pV2->GetX();
	float lY2 = pV2->GetY();

	float lDY0 = lY0 - lY2;
	float lDY1 = lY1 - lY2;
	float lDX0 = lX0 - lX2;
	float lDX1 = lX1 - lX2;

	mUOverZXStep = lOneOverDX * ( (pV1->GetUOverZ() - pV2->GetUOverZ()) * lDY0 -
									  (pV0->GetUOverZ() - pV2->GetUOverZ()) * lDY1);
	mUOverZYStep = lOneOverDY * ( (pV1->GetUOverZ() - pV2->GetUOverZ()) * lDX0 -
									  (pV0->GetUOverZ() - pV2->GetUOverZ()) * lDX1);

	mVOverZXStep = lOneOverDX * ( (pV1->GetVOverZ() - pV2->GetVOverZ()) * lDY0 -
									  (pV0->GetVOverZ() - pV2->GetVOverZ()) * lDY1);
	mVOverZYStep = lOneOverDY * ( (pV1->GetVOverZ() - pV2->GetVOverZ()) * lDX0 -
									  (pV0->GetVOverZ() - pV2->GetVOverZ()) * lDX1);

	float lROverZ[3];
	float lGOverZ[3];
	float lBOverZ[3];

	if (pGammaConvert == true)
	{
		lROverZ[0] = Lepra::GammaLookup::GammaToLinearFloat((unsigned char)(pV0->GetR() * 255.0f)) * pV0->GetOneOverZ();
		lGOverZ[0] = Lepra::GammaLookup::GammaToLinearFloat((unsigned char)(pV0->GetG() * 255.0f)) * pV0->GetOneOverZ();
		lBOverZ[0] = Lepra::GammaLookup::GammaToLinearFloat((unsigned char)(pV0->GetB() * 255.0f)) * pV0->GetOneOverZ();

		lROverZ[1] = Lepra::GammaLookup::GammaToLinearFloat((unsigned char)(pV1->GetR() * 255.0f)) * pV1->GetOneOverZ();
		lGOverZ[1] = Lepra::GammaLookup::GammaToLinearFloat((unsigned char)(pV1->GetG() * 255.0f)) * pV1->GetOneOverZ();
		lBOverZ[1] = Lepra::GammaLookup::GammaToLinearFloat((unsigned char)(pV1->GetB() * 255.0f)) * pV1->GetOneOverZ();

		lROverZ[2] = Lepra::GammaLookup::GammaToLinearFloat((unsigned char)(pV2->GetR() * 255.0f)) * pV2->GetOneOverZ();
		lGOverZ[2] = Lepra::GammaLookup::GammaToLinearFloat((unsigned char)(pV2->GetG() * 255.0f)) * pV2->GetOneOverZ();
		lBOverZ[2] = Lepra::GammaLookup::GammaToLinearFloat((unsigned char)(pV2->GetB() * 255.0f)) * pV2->GetOneOverZ();
	}
	else
	{
		lROverZ[0] = pV0->GetROverZ();
		lGOverZ[0] = pV0->GetGOverZ();
		lBOverZ[0] = pV0->GetBOverZ();

		lROverZ[1] = pV1->GetROverZ();
		lGOverZ[1] = pV1->GetGOverZ();
		lBOverZ[1] = pV1->GetBOverZ();

		lROverZ[2] = pV2->GetROverZ();
		lGOverZ[2] = pV2->GetGOverZ();
		lBOverZ[2] = pV2->GetBOverZ();
	}

	mROverZXStep = lOneOverDX * ( (lROverZ[1] - lROverZ[2]) * lDY0 -
									  (lROverZ[0] - lROverZ[2]) * lDY1);
	mROverZYStep = lOneOverDY * ( (lROverZ[1] - lROverZ[2]) * lDX0 -
									  (lROverZ[0] - lROverZ[2]) * lDX1);

	mGOverZXStep = lOneOverDX * ( (lGOverZ[1] - lGOverZ[2]) * lDY0 -
									  (lGOverZ[0] - lGOverZ[2]) * lDY1);
	mGOverZYStep = lOneOverDY * ( (lGOverZ[1] - lGOverZ[2]) * lDX0 -
									  (lGOverZ[0] - lGOverZ[2]) * lDX1);

	mBOverZXStep = lOneOverDX * ( (lBOverZ[1] - lBOverZ[2]) * lDY0 -
									  (lBOverZ[0] - lBOverZ[2]) * lDY1);
	mBOverZYStep = lOneOverDY * ( (lBOverZ[1] - lBOverZ[2]) * lDX0 -
									  (lBOverZ[0] - lBOverZ[2]) * lDX1);

	mDUDXModifier = GetModifier(pV0->GetUOverZ(),
								  mUOverZXStep,
								  mUOverZYStep,
								  pV0->GetOneOverZ());

	mDVDXModifier = GetModifier(pV0->GetVOverZ(),
								  mVOverZXStep,
								  mVOverZYStep,
								  pV0->GetOneOverZ());

	mDRDXModifier = GetModifier(pV0->GetROverZ(),
								  mROverZXStep,
								  mROverZYStep,
								  pV0->GetOneOverZ());

	mDGDXModifier = GetModifier(pV0->GetGOverZ(),
								  mGOverZXStep,
								  mGOverZYStep,
								  pV0->GetOneOverZ());

	mDBDXModifier = GetModifier(pV0->GetBOverZ(),
								  mBOverZXStep,
								  mBOverZYStep,
								  pV0->GetOneOverZ());
}

GradientsUVM::GradientsUVM(const Vertex3DUVM* pV0,
								 const Vertex3DUVM* pV1,
								 const Vertex3DUVM* pV2)
{
	float lOneOverDX;
	float lOneOverDY;
	Gradients::Init(pV0, pV1, pV2, lOneOverDX, lOneOverDY);

	float lX0 = pV0->GetX();
	float lY0 = pV0->GetY();
	float lX1 = pV1->GetX();
	float lY1 = pV1->GetY();
	float lX2 = pV2->GetX();
	float lY2 = pV2->GetY();

	float lDY0 = lY0 - lY2;
	float lDY1 = lY1 - lY2;
	float lDX0 = lX0 - lX2;
	float lDX1 = lX1 - lX2;

	mUOverZXStep = lOneOverDX * ( (pV1->GetUOverZ() - pV2->GetUOverZ()) * lDY0 -
									  (pV0->GetUOverZ() - pV2->GetUOverZ()) * lDY1);
	mUOverZYStep = lOneOverDY * ( (pV1->GetUOverZ() - pV2->GetUOverZ()) * lDX0 -
									  (pV0->GetUOverZ() - pV2->GetUOverZ()) * lDX1);

	mVOverZXStep = lOneOverDX * ( (pV1->GetVOverZ() - pV2->GetVOverZ()) * lDY0 -
									  (pV0->GetVOverZ() - pV2->GetVOverZ()) * lDY1);
	mVOverZYStep = lOneOverDY * ( (pV1->GetVOverZ() - pV2->GetVOverZ()) * lDX0 -
									  (pV0->GetVOverZ() - pV2->GetVOverZ()) * lDX1);

	mMOverZXStep = lOneOverDX * ( (pV1->GetMOverZ() - pV2->GetMOverZ()) * lDY0 -
									  (pV0->GetMOverZ() - pV2->GetMOverZ()) * lDY1);
	mMOverZYStep = lOneOverDY * ( (pV1->GetMOverZ() - pV2->GetMOverZ()) * lDX0 -
									  (pV0->GetMOverZ() - pV2->GetMOverZ()) * lDX1);

	mDUDXModifier = GetModifier(pV0->GetUOverZ(),
								  mUOverZXStep,
								  mUOverZYStep,
								  pV0->GetOneOverZ());

	mDVDXModifier = GetModifier(pV0->GetVOverZ(),
								  mVOverZXStep,
								  mVOverZYStep,
								  pV0->GetOneOverZ());

	mDMDXModifier = GetModifier(pV0->GetMOverZ(),
								  mMOverZXStep,
								  mMOverZYStep,
								  pV0->GetOneOverZ());
}

GradientsUVRGBM::GradientsUVRGBM(const Vertex3DUVRGBM* pV0,
									   const Vertex3DUVRGBM* pV1,
									   const Vertex3DUVRGBM* pV2,
									   bool pGammaConvert)
{
	float lOneOverDX;
	float lOneOverDY;
	Gradients::Init(pV0, pV1, pV2, lOneOverDX, lOneOverDY);

	float lX0 = pV0->GetX();
	float lY0 = pV0->GetY();
	float lX1 = pV1->GetX();
	float lY1 = pV1->GetY();
	float lX2 = pV2->GetX();
	float lY2 = pV2->GetY();

	float lDY0 = lY0 - lY2;
	float lDY1 = lY1 - lY2;
	float lDX0 = lX0 - lX2;
	float lDX1 = lX1 - lX2;

	mUOverZXStep = lOneOverDX * ( (pV1->GetUOverZ() - pV2->GetUOverZ()) * lDY0 -
									  (pV0->GetUOverZ() - pV2->GetUOverZ()) * lDY1);
	mUOverZYStep = lOneOverDY * ( (pV1->GetUOverZ() - pV2->GetUOverZ()) * lDX0 -
									  (pV0->GetUOverZ() - pV2->GetUOverZ()) * lDX1);

	mVOverZXStep = lOneOverDX * ( (pV1->GetVOverZ() - pV2->GetVOverZ()) * lDY0 -
									  (pV0->GetVOverZ() - pV2->GetVOverZ()) * lDY1);
	mVOverZYStep = lOneOverDY * ( (pV1->GetVOverZ() - pV2->GetVOverZ()) * lDX0 -
									  (pV0->GetVOverZ() - pV2->GetVOverZ()) * lDX1);

	float lROverZ[3];
	float lGOverZ[3];
	float lBOverZ[3];

	if (pGammaConvert == true)
	{
		lROverZ[0] = Lepra::GammaLookup::GammaToLinearFloat((unsigned char)(pV0->GetR() * 255.0f)) * pV0->GetOneOverZ();
		lGOverZ[0] = Lepra::GammaLookup::GammaToLinearFloat((unsigned char)(pV0->GetG() * 255.0f)) * pV0->GetOneOverZ();
		lBOverZ[0] = Lepra::GammaLookup::GammaToLinearFloat((unsigned char)(pV0->GetB() * 255.0f)) * pV0->GetOneOverZ();

		lROverZ[1] = Lepra::GammaLookup::GammaToLinearFloat((unsigned char)(pV1->GetR() * 255.0f)) * pV1->GetOneOverZ();
		lGOverZ[1] = Lepra::GammaLookup::GammaToLinearFloat((unsigned char)(pV1->GetG() * 255.0f)) * pV1->GetOneOverZ();
		lBOverZ[1] = Lepra::GammaLookup::GammaToLinearFloat((unsigned char)(pV1->GetB() * 255.0f)) * pV1->GetOneOverZ();

		lROverZ[2] = Lepra::GammaLookup::GammaToLinearFloat((unsigned char)(pV2->GetR() * 255.0f)) * pV2->GetOneOverZ();
		lGOverZ[2] = Lepra::GammaLookup::GammaToLinearFloat((unsigned char)(pV2->GetG() * 255.0f)) * pV2->GetOneOverZ();
		lBOverZ[2] = Lepra::GammaLookup::GammaToLinearFloat((unsigned char)(pV2->GetB() * 255.0f)) * pV2->GetOneOverZ();
	}
	else
	{
		lROverZ[0] = pV0->GetROverZ();
		lGOverZ[0] = pV0->GetGOverZ();
		lBOverZ[0] = pV0->GetBOverZ();

		lROverZ[1] = pV1->GetROverZ();
		lGOverZ[1] = pV1->GetGOverZ();
		lBOverZ[1] = pV1->GetBOverZ();

		lROverZ[2] = pV2->GetROverZ();
		lGOverZ[2] = pV2->GetGOverZ();
		lBOverZ[2] = pV2->GetBOverZ();
	}

	mROverZXStep = lOneOverDX * ( (lROverZ[1] - lROverZ[2]) * lDY0 -
									  (lROverZ[0] - lROverZ[2]) * lDY1);
	mROverZYStep = lOneOverDY * ( (lROverZ[1] - lROverZ[2]) * lDX0 -
									  (lROverZ[0] - lROverZ[2]) * lDX1);

	mGOverZXStep = lOneOverDX * ( (lGOverZ[1] - lGOverZ[2]) * lDY0 -
									  (lGOverZ[0] - lGOverZ[2]) * lDY1);
	mGOverZYStep = lOneOverDY * ( (lGOverZ[1] - lGOverZ[2]) * lDX0 -
									  (lGOverZ[0] - lGOverZ[2]) * lDX1);

	mBOverZXStep = lOneOverDX * ( (lBOverZ[1] - lBOverZ[2]) * lDY0 -
									  (lBOverZ[0] - lBOverZ[2]) * lDY1);
	mBOverZYStep = lOneOverDY * ( (lBOverZ[1] - lBOverZ[2]) * lDX0 -
									  (lBOverZ[0] - lBOverZ[2]) * lDX1);

	mMOverZXStep = lOneOverDX * ( (pV1->GetMOverZ() - pV2->GetMOverZ()) * lDY0 -
									  (pV0->GetMOverZ() - pV2->GetMOverZ()) * lDY1);
	mMOverZYStep = lOneOverDY * ( (pV1->GetMOverZ() - pV2->GetMOverZ()) * lDX0 -
									  (pV0->GetMOverZ() - pV2->GetMOverZ()) * lDX1);

	mDUDXModifier = GetModifier(pV0->GetUOverZ(),
								  mUOverZXStep,
								  mUOverZYStep,
								  pV0->GetOneOverZ());

	mDVDXModifier = GetModifier(pV0->GetVOverZ(),
								  mVOverZXStep,
								  mVOverZYStep,
								  pV0->GetOneOverZ());

	mDRDXModifier = GetModifier(pV0->GetROverZ(),
								  mROverZXStep,
								  mROverZYStep,
								  pV0->GetOneOverZ());

	mDGDXModifier = GetModifier(pV0->GetGOverZ(),
								  mGOverZXStep,
								  mGOverZYStep,
								  pV0->GetOneOverZ());

	mDBDXModifier = GetModifier(pV0->GetBOverZ(),
								  mBOverZXStep,
								  mBOverZYStep,
								  pV0->GetOneOverZ());

	mDMDXModifier = GetModifier(pV0->GetMOverZ(),
								  mMOverZXStep,
								  mMOverZYStep,
								  pV0->GetOneOverZ());
}



/*

	Linear Gradients. (For 2D graphics).

*/



Gradients2D::Gradients2D()
{
}

Gradients2DUV::Gradients2DUV(const Vertex2DUV* pV0,
							 const Vertex2DUV* pV1,
							 const Vertex2DUV* pV2)
{
	float lX0 = pV0->GetX();
	float lY0 = pV0->GetY();
	float lX1 = pV1->GetX();
	float lY1 = pV1->GetY();
	float lX2 = pV2->GetX();
	float lY2 = pV2->GetY();

	float lDY0 = lY0 - lY2;
	float lDY1 = lY1 - lY2;
	float lDX0 = lX0 - lX2;
	float lDX1 = lX1 - lX2;

	float lX1Y0 = lDX1 * lDY0;
	float lX0Y1 = lDX0 * lDY1;

	float lOneOverDX = 1.0f / (lX1Y0 - lX0Y1);
	float lOneOverDY = -lOneOverDX;

	mUXStep = lOneOverDX * ( (pV1->GetU() - pV2->GetU()) * lDY0 -
								 (pV0->GetU() - pV2->GetU()) * lDY1);
	mUYStep = lOneOverDY * ( (pV1->GetU() - pV2->GetU()) * lDX0 -
								 (pV0->GetU() - pV2->GetU()) * lDX1);

	mVXStep = lOneOverDX * ( (pV1->GetV() - pV2->GetV()) * lDY0 -
								 (pV0->GetV() - pV2->GetV()) * lDY1);
	mVYStep = lOneOverDY * ( (pV1->GetV() - pV2->GetV()) * lDX0 -
								 (pV0->GetV() - pV2->GetV()) * lDX1);

	mUXstep = Lepra::FixedPointMath::FloatToFixed16_16(mUXStep);
	mVXstep = Lepra::FixedPointMath::FloatToFixed16_16(mVXStep);

	mDUDXModifier = GetModifier(mUXStep, mUYStep);
	mDVDXModifier = GetModifier(mVXStep, mVYStep);
}

Gradients2DRGBA::Gradients2DRGBA(const Vertex2DRGBA* pV0,
								 const Vertex2DRGBA* pV1,
								 const Vertex2DRGBA* pV2)
{
	float lX0 = pV0->GetX();
	float lY0 = pV0->GetY();
	float lX1 = pV1->GetX();
	float lY1 = pV1->GetY();
	float lX2 = pV2->GetX();
	float lY2 = pV2->GetY();

	float lDY0 = lY0 - lY2;
	float lDY1 = lY1 - lY2;
	float lDX0 = lX0 - lX2;
	float lDX1 = lX1 - lX2;

	float lX1Y0 = lDX1 * lDY0;
	float lX0Y1 = lDX0 * lDY1;

	float lOneOverDX = 1.0f / (lX1Y0 - lX0Y1);
	float lOneOverDY = -lOneOverDX;

	mRXStep = lOneOverDX * ( (pV1->GetR() - pV2->GetR()) * lDY0 -
								 (pV0->GetR() - pV2->GetR()) * lDY1);
	mRYStep = lOneOverDY * ( (pV1->GetR() - pV2->GetR()) * lDX0 -
								 (pV0->GetR() - pV2->GetR()) * lDX1);

	mGXStep = lOneOverDX * ( (pV1->GetG() - pV2->GetG()) * lDY0 -
								 (pV0->GetG() - pV2->GetG()) * lDY1);
	mGYStep = lOneOverDY * ( (pV1->GetG() - pV2->GetG()) * lDX0 -
								 (pV0->GetG() - pV2->GetG()) * lDX1);

	mBXStep = lOneOverDX * ( (pV1->GetB() - pV2->GetB()) * lDY0 -
								 (pV0->GetB() - pV2->GetB()) * lDY1);
	mBYStep = lOneOverDY * ( (pV1->GetB() - pV2->GetB()) * lDX0 -
								 (pV0->GetB() - pV2->GetB()) * lDX1);

	mAXStep = lOneOverDX * ( (pV1->GetA() - pV2->GetA()) * lDY0 -
								 (pV0->GetA() - pV2->GetA()) * lDY1);
	mAYStep = lOneOverDY * ( (pV1->GetA() - pV2->GetA()) * lDX0 -
								 (pV0->GetA() - pV2->GetA()) * lDX1);

	mRXstep = Lepra::FixedPointMath::FloatToFixed16_16(mRXStep);
	mGXstep = Lepra::FixedPointMath::FloatToFixed16_16(mGXStep);
	mBXstep = Lepra::FixedPointMath::FloatToFixed16_16(mBXStep);
	mAXstep = Lepra::FixedPointMath::FloatToFixed16_16(mAXStep);

	mDRDXModifier = GetModifier(mRXStep, mRYStep);
	mDGDXModifier = GetModifier(mGXStep, mGYStep);
	mDBDXModifier = GetModifier(mBXStep, mBYStep);
	mDADXModifier = GetModifier(mAXStep, mAYStep);
}

} // End namespace.