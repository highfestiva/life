/*
	Class:  Gradients
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand

	NOTES:

	Gradients are all those values that you want to interpolate
	over a triangle (in a software renderer), like uv-coordinates 
	and rgb-values.	OBS! Don't mix this up with edges!
*/

#ifndef GRADIENTSTEXTURIZED_H
#define GRADIENTSTEXTURIZED_H

#include "UiTBC.h"
#include "../../Lepra/Include/FixedPointMath.h"

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

class Gradients
{
public:

	Gradients();
	Gradients(const Vertex3D* pV0,
		  const Vertex3D* pV1,
		  const Vertex3D* pV2);

	inline float GetOneOverZXStep() const;
	inline float GetOneOverZYStep() const;

	inline Lepra::FixedPointMath::Fixed16_16 GetModifier(float pValueOverZ, float pValueOverZXStep,
		float pValueOverZYStep, float pOneOverZ);

protected:
	void Init(const Vertex3D* pVertex0,
		  const Vertex3D* pVertex1,
		  const Vertex3D* pVertex2,
		  float& pOneOverDX,
		  float& pOneOverDY);

private:
	float mOneOverZXStep;
	float mOneOverZYStep;
};

float Gradients::GetOneOverZXStep() const
{
	return mOneOverZXStep;
}

float Gradients::GetOneOverZYStep() const
{
	return mOneOverZYStep;
}

Lepra::FixedPointMath::Fixed16_16 Gradients::GetModifier(float pValueOverZ, 
							 float pValueOverZXStep, 
							 float pValueOverZYStep,
							 float pOneOverZ)
{
	//
	// Set up rounding modifiers.
	// Read Chris Heckers articles about texture mapping for 
	// the detailed description. 
	// http://www.d6.com/users/checker/misctech.htm
	//

	const Lepra::FixedPointMath::Fixed16_16 lHalf = 0x8000;
	const Lepra::FixedPointMath::Fixed16_16 lPosModifier = lHalf;
	const Lepra::FixedPointMath::Fixed16_16 lNegModifier = lHalf - 1;

	float lDUDXIndicator = pValueOverZXStep * pOneOverZ - 
				  pValueOverZ * mOneOverZXStep;

	if (lDUDXIndicator > 0.0f)
	{
		return lPosModifier;
	}
	else if(lDUDXIndicator < 0.0f)
	{
		return lNegModifier;
	}
	else
	{
		float lDUDYIndicator = pValueOverZYStep * pOneOverZ -
					  pValueOverZ * mOneOverZYStep;

		if (lDUDYIndicator >= 0.0f)
		{
			return lPosModifier;
		}
		else
		{
			return lNegModifier;
		}
	}
}

class GradientsRGB : public Gradients
{
public:
	
	GradientsRGB(const Vertex3DRGB* pV0, 
		     const Vertex3DRGB* pV1,
		     const Vertex3DRGB* pV2,
		     bool pGammaConvert);

	inline float GetROverZXStep() const;
	inline float GetROverZYStep() const;
	inline float GetGOverZXStep() const;
	inline float GetGOverZYStep() const;
	inline float GetBOverZXStep() const;
	inline float GetBOverZYStep() const;
	
	inline Lepra::FixedPointMath::Fixed16_16 GetDRDXModifier() const;
	inline Lepra::FixedPointMath::Fixed16_16 GetDGDXModifier() const;
	inline Lepra::FixedPointMath::Fixed16_16 GetDBDXModifier() const;

private:

	float mROverZXStep;
	float mROverZYStep;
	float mGOverZXStep;
	float mGOverZYStep;
	float mBOverZXStep;
	float mBOverZYStep;

	Lepra::FixedPointMath::Fixed16_16 mDRDXModifier;
	Lepra::FixedPointMath::Fixed16_16 mDGDXModifier;
	Lepra::FixedPointMath::Fixed16_16 mDBDXModifier;
};

float GradientsRGB::GetROverZXStep() const
{
	return mROverZXStep;
}

float GradientsRGB::GetROverZYStep() const
{
	return mROverZYStep;
}

float GradientsRGB::GetGOverZXStep() const
{
	return mGOverZXStep;
}

float GradientsRGB::GetGOverZYStep() const
{
	return mGOverZYStep;
}

float GradientsRGB::GetBOverZXStep() const
{
	return mBOverZXStep;
}

float GradientsRGB::GetBOverZYStep() const
{
	return mBOverZYStep;
}

Lepra::FixedPointMath::Fixed16_16 GradientsRGB::GetDRDXModifier() const 
{
	return mDRDXModifier;
}

Lepra::FixedPointMath::Fixed16_16 GradientsRGB::GetDGDXModifier() const 
{
	return mDGDXModifier;
}

Lepra::FixedPointMath::Fixed16_16 GradientsRGB::GetDBDXModifier() const 
{
	return mDBDXModifier;
}

class GradientsUV : public Gradients
{
public:
	
	GradientsUV(const Vertex3DUV* pV0, 
		    const Vertex3DUV* pV1,
		    const Vertex3DUV* pV2);

	inline float GetUOverZXStep() const;
	inline float GetUOverZYStep() const;
	inline float GetVOverZXStep() const;
	inline float GetVOverZYStep() const;

	inline Lepra::FixedPointMath::Fixed16_16 GetDUDXModifier() const;
	inline Lepra::FixedPointMath::Fixed16_16 GetDVDXModifier() const;

private:

	float mUOverZXStep;
	float mUOverZYStep;
	float mVOverZXStep;
	float mVOverZYStep;

	Lepra::FixedPointMath::Fixed16_16 mDUDXModifier;
	Lepra::FixedPointMath::Fixed16_16 mDVDXModifier;
};

float GradientsUV::GetUOverZXStep() const
{
	return mUOverZXStep;
}

float GradientsUV::GetUOverZYStep() const
{
	return mUOverZYStep;
}

float GradientsUV::GetVOverZXStep() const
{
	return mVOverZXStep;
}

float GradientsUV::GetVOverZYStep() const
{
	return mVOverZYStep;
}

Lepra::FixedPointMath::Fixed16_16 GradientsUV::GetDUDXModifier() const 
{
	return mDUDXModifier;
}

Lepra::FixedPointMath::Fixed16_16 GradientsUV::GetDVDXModifier() const 
{
	return mDVDXModifier;
}

class GradientsUVRGB : public Gradients
{
public:
	
	GradientsUVRGB(const Vertex3DUVRGB* pV0, 
		       const Vertex3DUVRGB* pV1,
		       const Vertex3DUVRGB* pV2,
		       bool pGammaConvert);

	inline float GetUOverZXStep() const;
	inline float GetUOverZYStep() const;
	inline float GetVOverZXStep() const;
	inline float GetVOverZYStep() const;
	inline float GetROverZXStep() const;
	inline float GetROverZYStep() const;
	inline float GetGOverZXStep() const;
	inline float GetGOverZYStep() const;
	inline float GetBOverZXStep() const;
	inline float GetBOverZYStep() const;

	inline Lepra::FixedPointMath::Fixed16_16 GetDUDXModifier() const;
	inline Lepra::FixedPointMath::Fixed16_16 GetDVDXModifier() const;
	inline Lepra::FixedPointMath::Fixed16_16 GetDRDXModifier() const;
	inline Lepra::FixedPointMath::Fixed16_16 GetDGDXModifier() const;
	inline Lepra::FixedPointMath::Fixed16_16 GetDBDXModifier() const;

private:

	float mUOverZXStep;
	float mUOverZYStep;
	float mVOverZXStep;
	float mVOverZYStep;
	float mROverZXStep;
	float mROverZYStep;
	float mGOverZXStep;
	float mGOverZYStep;
	float mBOverZXStep;
	float mBOverZYStep;

	Lepra::FixedPointMath::Fixed16_16 mDUDXModifier;
	Lepra::FixedPointMath::Fixed16_16 mDVDXModifier;
	Lepra::FixedPointMath::Fixed16_16 mDRDXModifier;
	Lepra::FixedPointMath::Fixed16_16 mDGDXModifier;
	Lepra::FixedPointMath::Fixed16_16 mDBDXModifier;
};

float GradientsUVRGB::GetUOverZXStep() const
{
	return mUOverZXStep;
}

float GradientsUVRGB::GetUOverZYStep() const
{
	return mUOverZYStep;
}

float GradientsUVRGB::GetVOverZXStep() const
{
	return mVOverZXStep;
}

float GradientsUVRGB::GetVOverZYStep() const
{
	return mVOverZYStep;
}

float GradientsUVRGB::GetROverZXStep() const
{
	return mROverZXStep;
}

float GradientsUVRGB::GetROverZYStep() const
{
	return mROverZYStep;
}

float GradientsUVRGB::GetGOverZXStep() const
{
	return mGOverZXStep;
}

float GradientsUVRGB::GetGOverZYStep() const
{
	return mGOverZYStep;
}

float GradientsUVRGB::GetBOverZXStep() const
{
	return mBOverZXStep;
}

float GradientsUVRGB::GetBOverZYStep() const
{
	return mBOverZYStep;
}

Lepra::FixedPointMath::Fixed16_16 GradientsUVRGB::GetDUDXModifier() const 
{
	return mDUDXModifier;
}

Lepra::FixedPointMath::Fixed16_16 GradientsUVRGB::GetDVDXModifier() const 
{
	return mDVDXModifier;
}

Lepra::FixedPointMath::Fixed16_16 GradientsUVRGB::GetDRDXModifier() const 
{
	return mDRDXModifier;
}

Lepra::FixedPointMath::Fixed16_16 GradientsUVRGB::GetDGDXModifier() const 
{
	return mDGDXModifier;
}

Lepra::FixedPointMath::Fixed16_16 GradientsUVRGB::GetDBDXModifier() const 
{
	return mDBDXModifier;
}

class GradientsUVM : public Gradients
{
public:
	
	GradientsUVM(const Vertex3DUVM* pV0,
		     const Vertex3DUVM* pV1,
		     const Vertex3DUVM* pV2);

	inline float GetUOverZXStep() const;
	inline float GetUOverZYStep() const;
	inline float GetVOverZXStep() const;
	inline float GetVOverZYStep() const;
	inline float GetMOverZXStep() const;
	inline float GetMOverZYStep() const;

	inline Lepra::FixedPointMath::Fixed16_16 GetDUDXModifier() const;
	inline Lepra::FixedPointMath::Fixed16_16 GetDVDXModifier() const;
	inline Lepra::FixedPointMath::Fixed16_16 GetDMDXModifier() const;

private:

	float mUOverZXStep;
	float mUOverZYStep;
	float mVOverZXStep;
	float mVOverZYStep;
	float mMOverZXStep;
	float mMOverZYStep;

	Lepra::FixedPointMath::Fixed16_16 mDUDXModifier;
	Lepra::FixedPointMath::Fixed16_16 mDVDXModifier;
	Lepra::FixedPointMath::Fixed16_16 mDMDXModifier;
};

float GradientsUVM::GetUOverZXStep() const
{
	return mUOverZXStep;
}

float GradientsUVM::GetUOverZYStep() const
{
	return mUOverZYStep;
}

float GradientsUVM::GetVOverZXStep() const
{
	return mVOverZXStep;
}

float GradientsUVM::GetVOverZYStep() const
{
	return mVOverZYStep;
}

float GradientsUVM::GetMOverZXStep() const
{
	return mMOverZXStep;
}

float GradientsUVM::GetMOverZYStep() const
{
	return mMOverZYStep;
}

Lepra::FixedPointMath::Fixed16_16 GradientsUVM::GetDUDXModifier() const 
{
	return mDUDXModifier;
}

Lepra::FixedPointMath::Fixed16_16 GradientsUVM::GetDVDXModifier() const 
{
	return mDVDXModifier;
}

Lepra::FixedPointMath::Fixed16_16 GradientsUVM::GetDMDXModifier() const 
{
	return mDMDXModifier;
}

class GradientsUVRGBM : public Gradients
{
public:
	
	GradientsUVRGBM(const Vertex3DUVRGBM* pV0, 
			const Vertex3DUVRGBM* pV1,
			const Vertex3DUVRGBM* pV2,
			bool pGammaConvert);

	inline float GetUOverZXStep() const;
	inline float GetUOverZYStep() const;
	inline float GetVOverZXStep() const;
	inline float GetVOverZYStep() const;
	inline float GetROverZXStep() const;
	inline float GetROverZYStep() const;
	inline float GetGOverZXStep() const;
	inline float GetGOverZYStep() const;
	inline float GetBOverZXStep() const;
	inline float GetBOverZYStep() const;
	inline float GetMOverZXStep() const;
	inline float GetMOverZYStep() const;

	inline Lepra::FixedPointMath::Fixed16_16 GetDUDXModifier() const;
	inline Lepra::FixedPointMath::Fixed16_16 GetDVDXModifier() const;
	inline Lepra::FixedPointMath::Fixed16_16 GetDRDXModifier() const;
	inline Lepra::FixedPointMath::Fixed16_16 GetDGDXModifier() const;
	inline Lepra::FixedPointMath::Fixed16_16 GetDBDXModifier() const;
	inline Lepra::FixedPointMath::Fixed16_16 GetDMDXModifier() const;

private:

	float mUOverZXStep;
	float mUOverZYStep;
	float mVOverZXStep;
	float mVOverZYStep;
	float mROverZXStep;
	float mROverZYStep;
	float mGOverZXStep;
	float mGOverZYStep;
	float mBOverZXStep;
	float mBOverZYStep;
	float mMOverZXStep;
	float mMOverZYStep;

	Lepra::FixedPointMath::Fixed16_16 mDUDXModifier;
	Lepra::FixedPointMath::Fixed16_16 mDVDXModifier;
	Lepra::FixedPointMath::Fixed16_16 mDRDXModifier;
	Lepra::FixedPointMath::Fixed16_16 mDGDXModifier;
	Lepra::FixedPointMath::Fixed16_16 mDBDXModifier;
	Lepra::FixedPointMath::Fixed16_16 mDMDXModifier;
};


float GradientsUVRGBM::GetUOverZXStep() const
{
	return mUOverZXStep;
}

float GradientsUVRGBM::GetUOverZYStep() const
{
	return mUOverZYStep;
}

float GradientsUVRGBM::GetVOverZXStep() const
{
	return mVOverZXStep;
}

float GradientsUVRGBM::GetVOverZYStep() const
{
	return mVOverZYStep;
}

float GradientsUVRGBM::GetROverZXStep() const
{
	return mROverZXStep;
}

float GradientsUVRGBM::GetROverZYStep() const
{
	return mROverZYStep;
}

float GradientsUVRGBM::GetGOverZXStep() const
{
	return mGOverZXStep;
}

float GradientsUVRGBM::GetGOverZYStep() const
{
	return mGOverZYStep;
}

float GradientsUVRGBM::GetBOverZXStep() const
{
	return mBOverZXStep;
}

float GradientsUVRGBM::GetBOverZYStep() const
{
	return mBOverZYStep;
}

float GradientsUVRGBM::GetMOverZXStep() const
{
	return mMOverZXStep;
}

float GradientsUVRGBM::GetMOverZYStep() const
{
	return mMOverZYStep;
}

Lepra::FixedPointMath::Fixed16_16 GradientsUVRGBM::GetDUDXModifier() const 
{
	return mDUDXModifier;
}

Lepra::FixedPointMath::Fixed16_16 GradientsUVRGBM::GetDVDXModifier() const 
{
	return mDVDXModifier;
}

Lepra::FixedPointMath::Fixed16_16 GradientsUVRGBM::GetDRDXModifier() const 
{
	return mDRDXModifier;
}

Lepra::FixedPointMath::Fixed16_16 GradientsUVRGBM::GetDGDXModifier() const 
{
	return mDGDXModifier;
}

Lepra::FixedPointMath::Fixed16_16 GradientsUVRGBM::GetDBDXModifier() const 
{
	return mDBDXModifier;
}

Lepra::FixedPointMath::Fixed16_16 GradientsUVRGBM::GetDMDXModifier() const 
{
	return mDMDXModifier;
}

/*

	Linear Gradients. (For 2D graphics).

*/

class Gradients2D
{
public:

	Gradients2D();

	inline Lepra::FixedPointMath::Fixed16_16 GetModifier(float pValueXStep, 
							     float pValueYStep);

protected:

private:
};

Lepra::FixedPointMath::Fixed16_16 Gradients2D::GetModifier(float pValueXStep, 
							   float pValueYStep)
{
	//
	// Set up rounding modifiers.
	// Read Chris Heckers articles about texture mapping for 
	// the detailed description. 
	// http://www.d6.com/users/checker/misctech.htm
	//

	const Lepra::FixedPointMath::Fixed16_16 lHalf = 0x8000;
	const Lepra::FixedPointMath::Fixed16_16 lPosModifier = lHalf;
	const Lepra::FixedPointMath::Fixed16_16 lNegModifier = lHalf - 1;

	float lDUDXIndicator = pValueXStep;

	if (lDUDXIndicator > 0.0f)
	{
		return lPosModifier;
	}
	else if(lDUDXIndicator < 0.0f)
	{
		return lNegModifier;
	}
	else
	{
		float lDUDYIndicator = pValueYStep;

		if (lDUDYIndicator >= 0.0f)
		{
			return lPosModifier;
		}
		else
		{
			return lNegModifier;
		}
	}
}







class Gradients2DUV : public Gradients2D
{
public:
	
	Gradients2DUV(const Vertex2DUV* pV0, 
		      const Vertex2DUV* pV1,
		      const Vertex2DUV* pV2);

	inline float GetUXStep() const;
	inline float GetUYStep() const;
	inline float GetVXStep() const;
	inline float GetVYStep() const;

	inline Lepra::FixedPointMath::Fixed16_16 GetUXStep16_16() const;
	inline Lepra::FixedPointMath::Fixed16_16 GetVXStep16_16() const;

	inline Lepra::FixedPointMath::Fixed16_16 GetDUDXModifier() const;
	inline Lepra::FixedPointMath::Fixed16_16 GetDVDXModifier() const;

private:

	float mUXStep;
	float mUYStep;
	float mVXStep;
	float mVYStep;

	Lepra::FixedPointMath::Fixed16_16 mUXstep;
	Lepra::FixedPointMath::Fixed16_16 mVXstep;

	Lepra::FixedPointMath::Fixed16_16 mDUDXModifier;
	Lepra::FixedPointMath::Fixed16_16 mDVDXModifier;
};

float Gradients2DUV::GetUXStep() const
{
	return mUXStep;
}

float Gradients2DUV::GetUYStep() const
{
	return mUYStep;
}

float Gradients2DUV::GetVXStep() const
{
	return mVXStep;
}

float Gradients2DUV::GetVYStep() const
{
	return mVYStep;
}

Lepra::FixedPointMath::Fixed16_16 Gradients2DUV::GetUXStep16_16() const
{
	return mUXstep;
}

Lepra::FixedPointMath::Fixed16_16 Gradients2DUV::GetVXStep16_16() const
{
	return mVXstep;
}

Lepra::FixedPointMath::Fixed16_16 Gradients2DUV::GetDUDXModifier() const 
{
	return mDUDXModifier;
}

Lepra::FixedPointMath::Fixed16_16 Gradients2DUV::GetDVDXModifier() const 
{
	return mDVDXModifier;
}














class Gradients2DRGBA : public Gradients2D
{
public:
	
	Gradients2DRGBA(const Vertex2DRGBA* pV0, 
			const Vertex2DRGBA* pV1,
			const Vertex2DRGBA* pV2);

	inline float GetRXStep() const;
	inline float GetRYStep() const;
	inline float GetGXStep() const;
	inline float GetGYStep() const;
	inline float GetBXStep() const;
	inline float GetBYStep() const;
	inline float GetAXStep() const;
	inline float GetAYStep() const;

	inline Lepra::FixedPointMath::Fixed16_16 GetRXStep16_16() const;
	inline Lepra::FixedPointMath::Fixed16_16 GetGXStep16_16() const;
	inline Lepra::FixedPointMath::Fixed16_16 GetBXStep16_16() const;
	inline Lepra::FixedPointMath::Fixed16_16 GetAXStep16_16() const;

	inline Lepra::FixedPointMath::Fixed16_16 GetDRDXModifier() const;
	inline Lepra::FixedPointMath::Fixed16_16 GetDGDXModifier() const;
	inline Lepra::FixedPointMath::Fixed16_16 GetDBDXModifier() const;
	inline Lepra::FixedPointMath::Fixed16_16 GetDADXModifier() const;

private:

	float mRXStep;
	float mRYStep;
	float mGXStep;
	float mGYStep;
	float mBXStep;
	float mBYStep;
	float mAXStep;
	float mAYStep;

	Lepra::FixedPointMath::Fixed16_16 mRXstep;
	Lepra::FixedPointMath::Fixed16_16 mGXstep;
	Lepra::FixedPointMath::Fixed16_16 mBXstep;
	Lepra::FixedPointMath::Fixed16_16 mAXstep;

	Lepra::FixedPointMath::Fixed16_16 mDRDXModifier;
	Lepra::FixedPointMath::Fixed16_16 mDGDXModifier;
	Lepra::FixedPointMath::Fixed16_16 mDBDXModifier;
	Lepra::FixedPointMath::Fixed16_16 mDADXModifier;
};

float Gradients2DRGBA::GetRXStep() const
{
	return mRXStep;
}

float Gradients2DRGBA::GetRYStep() const
{
	return mRYStep;
}

float Gradients2DRGBA::GetGXStep() const
{
	return mGXStep;
}

float Gradients2DRGBA::GetGYStep() const
{
	return mGYStep;
}

float Gradients2DRGBA::GetBXStep() const
{
	return mBXStep;
}

float Gradients2DRGBA::GetBYStep() const
{
	return mBYStep;
}

float Gradients2DRGBA::GetAXStep() const
{
	return mAXStep;
}

float Gradients2DRGBA::GetAYStep() const
{
	return mAYStep;
}

Lepra::FixedPointMath::Fixed16_16 Gradients2DRGBA::GetRXStep16_16() const
{
	return mRXstep;
}

Lepra::FixedPointMath::Fixed16_16 Gradients2DRGBA::GetGXStep16_16() const
{
	return mGXstep;
}

Lepra::FixedPointMath::Fixed16_16 Gradients2DRGBA::GetBXStep16_16() const
{
	return mBXstep;
}

Lepra::FixedPointMath::Fixed16_16 Gradients2DRGBA::GetAXStep16_16() const
{
	return mAXstep;
}

Lepra::FixedPointMath::Fixed16_16 Gradients2DRGBA::GetDRDXModifier() const 
{
	return mDRDXModifier;
}

Lepra::FixedPointMath::Fixed16_16 Gradients2DRGBA::GetDGDXModifier() const 
{
	return mDGDXModifier;
}

Lepra::FixedPointMath::Fixed16_16 Gradients2DRGBA::GetDBDXModifier() const 
{
	return mDBDXModifier;
}

Lepra::FixedPointMath::Fixed16_16 Gradients2DRGBA::GetDADXModifier() const 
{
	return mDADXModifier;
}

} // End namespace.


#endif