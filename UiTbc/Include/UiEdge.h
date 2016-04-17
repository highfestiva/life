/*
	Class:  Edge
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand

	NOTES:

	Class Edge and other Edge-classes. 
	Used to store info about a polygon edge during rasterization.

	Edge is responsible of clipping against the left and right
	edges of the screen. This is done by returning clipped coordinates
	when calling a Get() function.

	Clipping against top is accomplished by calling Step(count). The bottom
	edge is the easiest to clip against, and doesn't need any special functionality.
	The triangle renderer is responsible of this.

	I made it like this to avoid the problems that occur if all ScanLine-
	functions would do the clipping instead.
*/

#pragma once

#include "UiTbc.h"
#include "UiGradients.h"

namespace UiTbc
{

class Vertex3D;
class Vertex3DUV;
class Vertex3DRGB;
class Vertex3DUVRGB;
class Vertex3DUVM;
class Vertex3DUVRGBM;

class Vertex2D;
class Vertex2DUV;
class Vertex2DRGBA;

class Edge
{
public:

	virtual void Init(const Vertex3D* pV0, const Vertex3D* pV1);
	void Init(const Vertex2D* pV0, const Vertex2D* pV1);

	inline static void SetClipLeftRight(int pClipLeft, int pClipRight);

	virtual inline bool Step();
	virtual inline int Step(unsigned pCount);

	virtual inline void Reset();

	inline int GetX();
	inline int GetY();
	inline int GetHeight();

protected:

	int mX;				// DDA info for x.
	int mXStep;			// ..
	int mNumerator;		// ..
	int mDenominator;		// ..
	int mErrorTerm;		// ..

	int mY;				// Current y
	int mHeight;			// Vertical count...

	static int smClipLeft;
	static int smClipRight;

	int mStartX;
	int mStartY;
	int mStartHeight;
	int mStartErrorTerm;
	float mXPrestep;
	float mYPrestep;
};

void Edge::SetClipLeftRight(int pClipLeft, int pClipRight)
{
	smClipLeft = pClipLeft;
	smClipRight = pClipRight;
}

int Edge::GetX()
{
	if (mX < smClipLeft)
	{
		return smClipLeft;
	}
	else if(mX > smClipRight)
	{
		return smClipRight;
	}
	else
	{
		return mX;
	}
}

int Edge::GetY()
{
	return mY;
}

int Edge::GetHeight()
{
	return mHeight;
}

bool Edge::Step()
{
	bool lStepExtra = false;

	mX += mXStep;
	mY++;
	mHeight--;

	mErrorTerm += mNumerator;

	if ((unsigned)mErrorTerm >= (unsigned)mDenominator)
	{
		lStepExtra = true;

		mX++;
	
		mErrorTerm -= mDenominator;
	}

	return lStepExtra;
}

int Edge::Step(unsigned pCount)
{
	mX += mXStep * pCount;
	mY += pCount;
	mHeight -= pCount;

	mErrorTerm += mNumerator * pCount;

	if ((unsigned)mErrorTerm >= (unsigned)mDenominator &&
		mDenominator > 0)
	{
		int lCount = (unsigned)mErrorTerm / (unsigned)mDenominator;
		mX += lCount;
	
		mErrorTerm -= ((unsigned)mDenominator * lCount);

		return lCount;
	}

	return 0;
}

void Edge::Reset()
{
	mX = mStartX;
	mY = mStartY;
	mHeight = mStartHeight;
	mErrorTerm = mStartErrorTerm;
}















class EdgeZ : public Edge
{
public:

	virtual void Init(const Vertex3D* pV0, 
					  const Vertex3D* pV1,
					  const Gradients* pGradients);

	virtual inline bool Step();
	virtual inline int Step(unsigned pCount);

	virtual void Reset(const Gradients* pGradients);

	inline float GetOneOverZ();

private:

	float mOneOverZ;		// 1 / z
	float mOneOverZStep;
	float mOneOverZStepExtra;

	const Gradients* mGradients;

	float mStartOneOverZ;		// 1 / z
};

bool EdgeZ::Step()
{
	bool lStepExtra = Edge::Step();

	mOneOverZ += mOneOverZStep;

	if (lStepExtra == true)
	{
		mOneOverZ += mOneOverZStepExtra;
	}

	return lStepExtra;
}

int EdgeZ::Step(unsigned pCount)
{
	int lExtraSteps = Edge::Step(pCount);

	mOneOverZ += mOneOverZStep * (float)pCount;

	if (lExtraSteps > 0)
	{
		mOneOverZ += mOneOverZStepExtra * (float)lExtraSteps;
	}

	return lExtraSteps;
}

float EdgeZ::GetOneOverZ()
{
	if (mX < smClipLeft)
	{
		return mOneOverZ + mGradients->GetOneOverZXStep() * (smClipLeft - mX);
	}
	else if(mX > smClipRight)
	{
		return mOneOverZ - mGradients->GetOneOverZXStep() * (mX - smClipRight);
	}
	else
	{
		return mOneOverZ;
	}
}














class EdgeUV : public Edge
{
public:

	virtual void Init(const Vertex3DUV* pV0, 
					  const Vertex3DUV* pV1,
					  const GradientsUV* pGradients);

	virtual inline bool Step();
	virtual inline int Step(unsigned pCount);

	virtual void Reset(const GradientsUV* pGradients);

	inline float GetOneOverZ();
	inline float GetUOverZ();
	inline float GetVOverZ();

private:
	float mOneOverZ;		// 1 / z
	float mOneOverZStep;
	float mOneOverZStepExtra;

	float mUOverZ;		// u / z
	float mUOverZStep;
	float mUOverZStepExtra;

	float mVOverZ;		// v / z
	float mVOverZStep;
	float mVOverZStepExtra;

	const GradientsUV* mGradients;

	float mStartOneOverZ;
	float mStartUOverZ;
	float mStartVOverZ;
};

bool EdgeUV::Step()
{
	bool lStepExtra = Edge::Step();

	mOneOverZ += mOneOverZStep;
	mUOverZ   += mUOverZStep;
	mVOverZ   += mVOverZStep;

	if (lStepExtra == true)
	{
		mOneOverZ += mOneOverZStepExtra;
		mUOverZ   += mUOverZStepExtra;
		mVOverZ   += mVOverZStepExtra;
	}

	return lStepExtra;
}

int EdgeUV::Step(unsigned pCount)
{
	int lExtraSteps = Edge::Step(pCount);
	float lCount = (float)pCount;

	mOneOverZ += mOneOverZStep * lCount;
	mUOverZ   += mUOverZStep * lCount;
	mVOverZ   += mVOverZStep * lCount;

	if (lExtraSteps > 0)
	{
		float lExtraStepsFloat = (float)lExtraSteps;
		mOneOverZ += mOneOverZStepExtra * lExtraStepsFloat;
		mUOverZ   += mUOverZStepExtra * lExtraStepsFloat;
		mVOverZ   += mVOverZStepExtra * lExtraStepsFloat;
	}

	return lExtraSteps;
}

float EdgeUV::GetOneOverZ()
{
	if (mX < smClipLeft)
	{
		return mOneOverZ + mGradients->GetOneOverZXStep() * (smClipLeft - mX);
	}
	else if(mX > smClipRight)
	{
		return mOneOverZ - mGradients->GetOneOverZXStep() * (mX - smClipRight);
	}
	else
	{
		return mOneOverZ;
	}
}

float EdgeUV::GetUOverZ()
{
	if (mX < smClipLeft)
	{
		return mUOverZ + mGradients->GetUOverZXStep() * (smClipLeft - mX);
	}
	else if(mX > smClipRight)
	{
		return mUOverZ - mGradients->GetUOverZXStep() * (mX - smClipRight);
	}
	else
	{
		return mUOverZ;
	}
}

float EdgeUV::GetVOverZ()
{
	if (mX < smClipLeft)
	{
		return mVOverZ + mGradients->GetVOverZXStep() * (smClipLeft - mX);
	}
	else if(mX > smClipRight)
	{
		return mVOverZ - mGradients->GetVOverZXStep() * (mX - smClipRight);
	}
	else
	{
		return mVOverZ;
	}
}
















class EdgeRGB : public Edge
{
public:

	virtual void Init(const Vertex3DRGB* pV0, 
					  const Vertex3DRGB* pV1,
					  const GradientsRGB* pGradients,
					  bool pGammaConvert);

	virtual inline bool Step();
	virtual inline int Step(unsigned pCount);

	virtual void Reset(const GradientsRGB* pGradients);

	inline float GetOneOverZ();
	inline float GetROverZ();
	inline float GetGOverZ();
	inline float GetBOverZ();

private:

	float mOneOverZ;		// 1 / z
	float mOneOverZStep;
	float mOneOverZStepExtra;

	float mROverZ;		// r / z
	float mROverZStep;
	float mROverZStepExtra;

	float mGOverZ;	// g / z
	float mGOverZStep;
	float mGOverZStepExtra;

	float mBOverZ;		// b / z
	float mBOverZStep;
	float mBOverZStepExtra;

	const GradientsRGB* mGradients;

	float mStartOneOverZ;
	float mStartROverZ;
	float mStartGOverZ;
	float mStartBOverZ;
};

bool EdgeRGB::Step()
{
	bool lStepExtra = Edge::Step();

	mOneOverZ += mOneOverZStep;
	mROverZ += mROverZStep;
	mGOverZ += mGOverZStep;
	mBOverZ += mBOverZStep;

	if (lStepExtra == true)
	{
		mOneOverZ += mOneOverZStepExtra;
		mROverZ += mROverZStepExtra;
		mGOverZ += mGOverZStepExtra;
		mBOverZ += mBOverZStepExtra;
	}

	return lStepExtra;
}

int EdgeRGB::Step(unsigned pCount)
{
	int lExtraSteps = Edge::Step(pCount);
	float lCount = (float)pCount;

	mOneOverZ += mOneOverZStep * lCount;
	mROverZ += mROverZStep * lCount;
	mGOverZ += mGOverZStep * lCount;
	mBOverZ += mBOverZStep * lCount;

	if (lExtraSteps > 0)
	{
		float lExtraStepsFloat = (float)lExtraSteps;
		mOneOverZ += mOneOverZStepExtra * lExtraStepsFloat;
		mROverZ += mROverZStepExtra * lExtraStepsFloat;
		mGOverZ += mGOverZStepExtra * lExtraStepsFloat;
		mBOverZ += mBOverZStepExtra * lExtraStepsFloat;
	}

	return lExtraSteps;
}

float EdgeRGB::GetOneOverZ()
{
	if (mX < smClipLeft)
	{
		return mOneOverZ + mGradients->GetOneOverZXStep() * (smClipLeft - mX);
	}
	else if(mX > smClipRight)
	{
		return mOneOverZ - mGradients->GetOneOverZXStep() * (mX - smClipRight);
	}
	else
	{
		return mOneOverZ;
	}
}

float EdgeRGB::GetROverZ()
{
	if (mX < smClipLeft)
	{
		return mROverZ + mGradients->GetROverZXStep() * (smClipLeft - mX);
	}
	else if(mX > smClipRight)
	{
		return mROverZ - mGradients->GetROverZXStep() * (mX - smClipRight);
	}
	else
	{
		return mROverZ;
	}
}

float EdgeRGB::GetGOverZ()
{
	if (mX < smClipLeft)
	{
		return mGOverZ + mGradients->GetGOverZXStep() * (smClipLeft - mX);
	}
	else if(mX > smClipRight)
	{
		return mGOverZ - mGradients->GetGOverZXStep() * (mX - smClipRight);
	}
	else
	{
		return mGOverZ;
	}
}

float EdgeRGB::GetBOverZ()
{
	if (mX < smClipLeft)
	{
		return mBOverZ + mGradients->GetBOverZXStep() * (smClipLeft - mX);
	}
	else if(mX > smClipRight)
	{
		return mBOverZ - mGradients->GetBOverZXStep() * (mX - smClipRight);
	}
	else
	{
		return mBOverZ;
	}
}
















class EdgeUVRGB : public Edge
{
public:

	virtual void Init(const Vertex3DUVRGB* pV0, 
					  const Vertex3DUVRGB* pV1,
					  const GradientsUVRGB* pGradients,
					  bool pGammaConvert);

	virtual inline bool Step();
	virtual inline int Step(unsigned pCount);

	virtual void Reset(const GradientsUVRGB* pGradients);

	inline float GetOneOverZ();
	inline float GetUOverZ();
	inline float GetVOverZ();
	inline float GetROverZ();
	inline float GetGOverZ();
	inline float GetBOverZ();

private:

	float mOneOverZ;		// 1 / z
	float mOneOverZStep;
	float mOneOverZStepExtra;

	float mUOverZ;		// u / z
	float mUOverZStep;
	float mUOverZStepExtra;

	float mVOverZ;		// v / z
	float mVOverZStep;
	float mVOverZStepExtra;

	float mROverZ;		// r / z
	float mROverZStep;
	float mROverZStepExtra;

	float mGOverZ;	// g / z
	float mGOverZStep;
	float mGOverZStepExtra;

	float mBOverZ;		// b / z
	float mBOverZStep;
	float mBOverZStepExtra;

	const GradientsUVRGB* mGradients;

	float mStartOneOverZ;
	float mStartUOverZ;
	float mStartVOverZ;
	float mStartROverZ;
	float mStartGOverZ;
	float mStartBOverZ;
};

bool EdgeUVRGB::Step()
{
	bool lStepExtra = Edge::Step();

	mOneOverZ += mOneOverZStep;
	mUOverZ += mUOverZStep;
	mVOverZ += mVOverZStep;
	mROverZ += mROverZStep;
	mGOverZ += mGOverZStep;
	mBOverZ += mBOverZStep;

	if (lStepExtra == true)
	{
		mOneOverZ += mOneOverZStepExtra;
		mUOverZ += mUOverZStepExtra;
		mVOverZ += mVOverZStepExtra;
		mROverZ += mROverZStepExtra;
		mGOverZ += mGOverZStepExtra;
		mBOverZ += mBOverZStepExtra;
	}

	return lStepExtra;
}

int EdgeUVRGB::Step(unsigned pCount)
{
	int lExtraSteps = Edge::Step(pCount);
	float lCount = (float)pCount;

	mOneOverZ += mOneOverZStep * lCount;
	mUOverZ += mUOverZStep * lCount;
	mVOverZ += mVOverZStep * lCount;
	mROverZ += mROverZStep * lCount;
	mGOverZ += mGOverZStep * lCount;
	mBOverZ += mBOverZStep * lCount;

	if (lExtraSteps > 0)
	{
		float lExtraStepsFloat = (float)lExtraSteps;
		mOneOverZ += mOneOverZStepExtra * lExtraStepsFloat;
		mUOverZ += mUOverZStepExtra * lExtraStepsFloat;
		mVOverZ += mVOverZStepExtra * lExtraStepsFloat;
		mROverZ += mROverZStepExtra * lExtraStepsFloat;
		mGOverZ += mGOverZStepExtra * lExtraStepsFloat;
		mBOverZ += mBOverZStepExtra * lExtraStepsFloat;
	}

	return lExtraSteps;
}

float EdgeUVRGB::GetOneOverZ()
{
	if (mX < smClipLeft)
	{
		return mOneOverZ + mGradients->GetOneOverZXStep() * (smClipLeft - mX);
	}
	else if(mX > smClipRight)
	{
		return mOneOverZ - mGradients->GetOneOverZXStep() * (mX - smClipRight);
	}
	else
	{
		return mOneOverZ;
	}
}

float EdgeUVRGB::GetUOverZ()
{
	if (mX < smClipLeft)
	{
		return mUOverZ + mGradients->GetUOverZXStep() * (smClipLeft - mX);
	}
	else if(mX > smClipRight)
	{
		return mUOverZ - mGradients->GetUOverZXStep() * (mX - smClipRight);
	}
	else
	{
		return mUOverZ;
	}
}

float EdgeUVRGB::GetVOverZ()
{
	if (mX < smClipLeft)
	{
		return mVOverZ + mGradients->GetVOverZXStep() * (smClipLeft - mX);
	}
	else if(mX > smClipRight)
	{
		return mVOverZ - mGradients->GetVOverZXStep() * (mX - smClipRight);
	}
	else
	{
		return mVOverZ;
	}
}

float EdgeUVRGB::GetROverZ()
{
	if (mX < smClipLeft)
	{
		return mROverZ + mGradients->GetROverZXStep() * (smClipLeft - mX);
	}
	else if(mX > smClipRight)
	{
		return mROverZ - mGradients->GetROverZXStep() * (mX - smClipRight);
	}
	else
	{
		return mROverZ;
	}
}

float EdgeUVRGB::GetGOverZ()
{
	if (mX < smClipLeft)
	{
		return mGOverZ + mGradients->GetGOverZXStep() * (smClipLeft - mX);
	}
	else if(mX > smClipRight)
	{
		return mGOverZ - mGradients->GetGOverZXStep() * (mX - smClipRight);
	}
	else
	{
		return mGOverZ;
	}
}

float EdgeUVRGB::GetBOverZ()
{
	if (mX < smClipLeft)
	{
		return mBOverZ + mGradients->GetBOverZXStep() * (smClipLeft - mX);
	}
	else if(mX > smClipRight)
	{
		return mBOverZ - mGradients->GetBOverZXStep() * (mX - smClipRight);
	}
	else
	{
		return mBOverZ;
	}
}

















class EdgeUVM : public Edge
{
public:

	virtual void Init(const Vertex3DUVM* pV0, 
					  const Vertex3DUVM* pV1,
					  const GradientsUVM* pGradients);

	virtual inline bool Step();
	virtual inline int Step(unsigned pCount);

	virtual void Reset(const GradientsUVM* pGradients);

	inline float GetOneOverZ();
	inline float GetUOverZ();
	inline float GetVOverZ();
	inline float GetMOverZ();

private:

	float mOneOverZ;		// 1 / z
	float mOneOverZStep;
	float mOneOverZStepExtra;

	float mUOverZ;		// u / z
	float mUOverZStep;
	float mUOverZStepExtra;

	float mVOverZ;		// v / z
	float mVOverZStep;
	float mVOverZStepExtra;

	float mMOverZ;		// m / z
	float mMOverZStep;
	float mMOverZStepExtra;

	const GradientsUVM* mGradients;

	float mStartOneOverZ;
	float mStartUOverZ;
	float mStartVOverZ;
	float mStartMOverZ;
};

bool EdgeUVM::Step()
{
	bool lStepExtra = Edge::Step();

	mOneOverZ += mOneOverZStep;
	mUOverZ   += mUOverZStep;
	mVOverZ   += mVOverZStep;
	mMOverZ   += mMOverZStep;

	if (lStepExtra == true)
	{
		mOneOverZ += mOneOverZStepExtra;
		mUOverZ   += mUOverZStepExtra;
		mVOverZ   += mVOverZStepExtra;
		mMOverZ   += mMOverZStepExtra;
	}

	return lStepExtra;
}

int EdgeUVM::Step(unsigned pCount)
{
	int lExtraSteps = Edge::Step(pCount);
	float lCount = (float)pCount;

	mOneOverZ += mOneOverZStep * lCount;
	mUOverZ += mUOverZStep * lCount;
	mVOverZ += mVOverZStep * lCount;
	mMOverZ += mMOverZStep * lCount;

	if (lExtraSteps > 0)
	{
		float lExtraStepsFloat = (float)lExtraSteps;
		mOneOverZ += mOneOverZStepExtra * lExtraStepsFloat;
		mUOverZ += mUOverZStepExtra * lExtraStepsFloat;
		mVOverZ += mVOverZStepExtra * lExtraStepsFloat;
		mMOverZ += mMOverZStepExtra * lExtraStepsFloat;
	}

	return lExtraSteps;
}

float EdgeUVM::GetOneOverZ()
{
	if (mX < smClipLeft)
	{
		return mOneOverZ + mGradients->GetOneOverZXStep() * (smClipLeft - mX);
	}
	else if(mX > smClipRight)
	{
		return mOneOverZ - mGradients->GetOneOverZXStep() * (mX - smClipRight);
	}
	else
	{
		return mOneOverZ;
	}
}

float EdgeUVM::GetUOverZ()
{
	if (mX < smClipLeft)
	{
		return mUOverZ + mGradients->GetUOverZXStep() * (smClipLeft - mX);
	}
	else if(mX > smClipRight)
	{
		return mUOverZ - mGradients->GetUOverZXStep() * (mX - smClipRight);
	}
	else
	{
		return mUOverZ;
	}
}

float EdgeUVM::GetVOverZ()
{
	if (mX < smClipLeft)
	{
		return mVOverZ + mGradients->GetVOverZXStep() * (smClipLeft - mX);
	}
	else if(mX > smClipRight)
	{
		return mVOverZ - mGradients->GetVOverZXStep() * (mX - smClipRight);
	}
	else
	{
		return mVOverZ;
	}
}

float EdgeUVM::GetMOverZ()
{
	if (mX < smClipLeft)
	{
		return mMOverZ + mGradients->GetMOverZXStep() * (smClipLeft - mX);
	}
	else if(mX > smClipRight)
	{
		return mMOverZ - mGradients->GetMOverZXStep() * (mX - smClipRight);
	}
	else
	{
		return mMOverZ;
	}
}
















class EdgeUVRGBM : public Edge
{
public:

	virtual void Init(const Vertex3DUVRGBM* pV0, 
					  const Vertex3DUVRGBM* pV1,
					  const GradientsUVRGBM* pGradients,
					  bool pGammaConvert);

	virtual inline bool Step();
	virtual inline int Step(unsigned pCount);

	virtual void Reset(const GradientsUVRGBM* pGradients);

	inline float GetOneOverZ();
	inline float GetUOverZ();
	inline float GetVOverZ();
	inline float GetROverZ();
	inline float GetGOverZ();
	inline float GetBOverZ();
	inline float GetMOverZ();

private:

	float mOneOverZ;		// 1 / z
	float mOneOverZStep;
	float mOneOverZStepExtra;

	float mUOverZ;		// u / z
	float mUOverZStep;
	float mUOverZStepExtra;

	float mVOverZ;		// v / z
	float mVOverZStep;
	float mVOverZStepExtra;

	float mROverZ;		// r / z
	float mROverZStep;
	float mROverZStepExtra;

	float mGOverZ;	// g / z
	float mGOverZStep;
	float mGOverZStepExtra;

	float mBOverZ;		// b / z
	float mBOverZStep;
	float mBOverZStepExtra;

	float mMOverZ;		// m / z
	float mMOverZStep;
	float mMOverZStepExtra;

	const GradientsUVRGBM* mGradients;

	float mStartOneOverZ;
	float mStartUOverZ;
	float mStartVOverZ;
	float mStartROverZ;
	float mStartGOverZ;
	float mStartBOverZ;
	float mStartMOverZ;
};

bool EdgeUVRGBM::Step()
{
	bool lStepExtra = Edge::Step();

	mOneOverZ += mOneOverZStep;
	mUOverZ += mUOverZStep;
	mVOverZ += mVOverZStep;
	mROverZ += mROverZStep;
	mGOverZ += mGOverZStep;
	mBOverZ += mBOverZStep;
	mMOverZ += mMOverZStep;

	if (lStepExtra == true)
	{
		mOneOverZ += mOneOverZStepExtra;
		mUOverZ += mUOverZStepExtra;
		mVOverZ += mVOverZStepExtra;
		mROverZ += mROverZStepExtra;
		mGOverZ += mGOverZStepExtra;
		mBOverZ += mBOverZStepExtra;
		mMOverZ += mMOverZStepExtra;
	}

	return lStepExtra;
}

int EdgeUVRGBM::Step(unsigned pCount)
{
	int lExtraSteps = Edge::Step(pCount);
	float lCount = (float)pCount;

	mOneOverZ += mOneOverZStep * lCount;
	mUOverZ += mUOverZStep * lCount;
	mVOverZ += mVOverZStep * lCount;
	mROverZ += mROverZStep * lCount;
	mGOverZ += mGOverZStep * lCount;
	mBOverZ += mBOverZStep * lCount;
	mMOverZ += mMOverZStep * lCount;

	if (lExtraSteps > 0)
	{
		float lExtraStepsFloat = (float)lExtraSteps;
		mOneOverZ += mOneOverZStepExtra * lExtraStepsFloat;
		mUOverZ += mUOverZStepExtra * lExtraStepsFloat;
		mVOverZ += mVOverZStepExtra * lExtraStepsFloat;
		mROverZ += mROverZStepExtra * lExtraStepsFloat;
		mGOverZ += mGOverZStepExtra * lExtraStepsFloat;
		mBOverZ += mBOverZStepExtra * lExtraStepsFloat;
		mMOverZ += mMOverZStepExtra * lExtraStepsFloat;
	}

	return lExtraSteps;
}

float EdgeUVRGBM::GetOneOverZ()
{
	if (mX < smClipLeft)
	{
		return mOneOverZ + mGradients->GetOneOverZXStep() * (smClipLeft - mX);
	}
	else if(mX > smClipRight)
	{
		return mOneOverZ - mGradients->GetOneOverZXStep() * (mX - smClipRight);
	}
	else
	{
		return mOneOverZ;
	}
}

float EdgeUVRGBM::GetUOverZ()
{
	if (mX < smClipLeft)
	{
		return mUOverZ + mGradients->GetUOverZXStep() * (smClipLeft - mX);
	}
	else if(mX > smClipRight)
	{
		return mUOverZ - mGradients->GetUOverZXStep() * (mX - smClipRight);
	}
	else
	{
		return mUOverZ;
	}
}

float EdgeUVRGBM::GetVOverZ()
{
	if (mX < smClipLeft)
	{
		return mVOverZ + mGradients->GetVOverZXStep() * (smClipLeft - mX);
	}
	else if(mX > smClipRight)
	{
		return mVOverZ - mGradients->GetVOverZXStep() * (mX - smClipRight);
	}
	else
	{
		return mVOverZ;
	}
}

float EdgeUVRGBM::GetROverZ()
{
	if (mX < smClipLeft)
	{
		return mROverZ + mGradients->GetROverZXStep() * (smClipLeft - mX);
	}
	else if(mX > smClipRight)
	{
		return mROverZ - mGradients->GetROverZXStep() * (mX - smClipRight);
	}
	else
	{
		return mROverZ;
	}
}

float EdgeUVRGBM::GetGOverZ()
{
	if (mX < smClipLeft)
	{
		return mGOverZ + mGradients->GetGOverZXStep() * (smClipLeft - mX);
	}
	else if(mX > smClipRight)
	{
		return mGOverZ - mGradients->GetGOverZXStep() * (mX - smClipRight);
	}
	else
	{
		return mGOverZ;
	}
}

float EdgeUVRGBM::GetBOverZ()
{
	if (mX < smClipLeft)
	{
		return mBOverZ + mGradients->GetBOverZXStep() * (smClipLeft - mX);
	}
	else if(mX > smClipRight)
	{
		return mBOverZ - mGradients->GetBOverZXStep() * (mX - smClipRight);
	}
	else
	{
		return mBOverZ;
	}
}

float EdgeUVRGBM::GetMOverZ()
{
	if (mX < smClipLeft)
	{
		return mMOverZ + mGradients->GetMOverZXStep() * (smClipLeft - mX);
	}
	else if(mX > smClipRight)
	{
		return mMOverZ - mGradients->GetMOverZXStep() * (mX - smClipRight);
	}
	else
	{
		return mMOverZ;
	}
}


















class Edge2DUV : public Edge
{
public:

	virtual void Init(const Vertex2DUV* pV0, 
					  const Vertex2DUV* pV1,
					  const Gradients2DUV* pGradients);

	virtual inline bool Step();
	virtual inline int Step(unsigned pCount);

	virtual void Reset(const Gradients2DUV* pGradients);

	inline float GetU();
	inline float GetV();

private:
	float mU;
	float mUStep;
	float mUStepExtra;

	float mV;
	float mVStep;
	float mVStepExtra;

	const Gradients2DUV* mGradients;

	float mStartU;
	float mStartV;
};

bool Edge2DUV::Step()
{
	bool lStepExtra = Edge::Step();

	mU += mUStep;
	mV += mVStep;

	if (lStepExtra == true)
	{
		mU += mUStepExtra;
		mV += mVStepExtra;
	}

	return lStepExtra;
}

int Edge2DUV::Step(unsigned pCount)
{
	int lExtraSteps = Edge::Step(pCount);
	float lCount = (float)pCount;

	mU += mUStep * lCount;
	mV += mVStep * lCount;

	if (lExtraSteps > 0)
	{
		float lExtraStepsFloat = (float)lExtraSteps;
		mU += mUStepExtra * lExtraStepsFloat;
		mV += mVStepExtra * lExtraStepsFloat;
	}

	return lExtraSteps;
}

float Edge2DUV::GetU()
{
	if (mX < smClipLeft)
	{
		return mU + mGradients->GetUXStep() * (smClipLeft - mX);
	}
	else if(mX > smClipRight)
	{
		return mU - mGradients->GetUXStep() * (mX - smClipRight);
	}
	else
	{
		return mU;
	}
}

float Edge2DUV::GetV()
{
	if (mX < smClipLeft)
	{
		return mV + mGradients->GetVXStep() * (smClipLeft - mX);
	}
	else if(mX > smClipRight)
	{
		return mV - mGradients->GetVXStep() * (mX - smClipRight);
	}
	else
	{
		return mV;
	}
}











class Edge2DRGBA : public Edge
{
public:

	virtual void Init(const Vertex2DRGBA* pV0, 
					  const Vertex2DRGBA* pV1,
					  const Gradients2DRGBA* pGradients);

	virtual inline bool Step();
	virtual inline int Step(unsigned pCount);

	virtual void Reset(const Gradients2DRGBA* pGradients);

	inline float GetR();
	inline float GetG();
	inline float GetB();
	inline float GetA();

private:
	float mR;
	float mRStep;
	float mRStepExtra;

	float mG;
	float mGStep;
	float mGStepExtra;

	float mB;
	float mBStep;
	float mBStepExtra;

	float mA;
	float mAStep;
	float mAStepExtra;

	const Gradients2DRGBA* mGradients;

	float mStartR;
	float mStartG;
	float mStartB;
	float mStartA;
};

bool Edge2DRGBA::Step()
{
	bool lStepExtra = Edge::Step();

	mR += mRStep;
	mG += mGStep;
	mB += mBStep;
	mA += mAStep;

	if (lStepExtra == true)
	{
		mR += mRStepExtra;
		mG += mGStepExtra;
		mB += mBStepExtra;
		mA += mAStepExtra;
	}

	return lStepExtra;
}

int Edge2DRGBA::Step(unsigned pCount)
{
	int lExtraSteps = Edge::Step(pCount);
	float lCount = (float)pCount;

	mR += mRStep * lCount;
	mG += mGStep * lCount;
	mB += mBStep * lCount;
	mA += mAStep * lCount;

	if (lExtraSteps > 0)
	{
		float lExtraStepsFloat = (float)lExtraSteps;
		mR += mRStepExtra * lExtraStepsFloat;
		mG += mGStepExtra * lExtraStepsFloat;
		mB += mBStepExtra * lExtraStepsFloat;
		mA += mAStepExtra * lExtraStepsFloat;
	}

	return lExtraSteps;
}

float Edge2DRGBA::GetR()
{
	if (mX < smClipLeft)
	{
		return mR + mGradients->GetRXStep() * (smClipLeft - mX);
	}
	else if(mX > smClipRight)
	{
		return mR - mGradients->GetRXStep() * (mX - smClipRight);
	}
	else
	{
		return mR;
	}
}

float Edge2DRGBA::GetG()
{
	if (mX < smClipLeft)
	{
		return mG + mGradients->GetGXStep() * (smClipLeft - mX);
	}
	else if(mX > smClipRight)
	{
		return mG - mGradients->GetGXStep() * (mX - smClipRight);
	}
	else
	{
		return mG;
	}
}

float Edge2DRGBA::GetB()
{
	if (mX < smClipLeft)
	{
		return mB + mGradients->GetBXStep() * (smClipLeft - mX);
	}
	else if(mX > smClipRight)
	{
		return mB - mGradients->GetBXStep() * (mX - smClipRight);
	}
	else
	{
		return mB;
	}
}

float Edge2DRGBA::GetA()
{
	if (mX < smClipLeft)
	{
		return mA + mGradients->GetAXStep() * (smClipLeft - mX);
	}
	else if(mX > smClipRight)
	{
		return mA - mGradients->GetAXStep() * (mX - smClipRight);
	}
	else
	{
		return mA;
	}
}

}
