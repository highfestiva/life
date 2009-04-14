/*
	Class:  Vertex3D,
		Vertex3DUV,
		Vertex3DRGB,
		Vertex3DUVRGB,
		Vertex3DUVM,
		Vertex3DUVRGBM,
		Vertex2D,
		Vertex2DUV,
		Vertex2DRGBA
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand

	NOTES:

	These classes are used in Software3DPainter and represents
	a mesh's projected vertices.
*/

#ifndef VERTEX3D_H
#define VERTEX3D_H

#include "UiTBC.h"
#include "../../Lepra/Include/Graphics2D.h"
#include "../../Lepra/Include/FixedPointMath.h"



namespace UiTbc
{

//////////////////////////////////////////////////////////////////////////////
//
// Classes
//
//////////////////////////////////////////////////////////////////////////////



class Vertex3D
{
public:
	inline Vertex3D();
	inline Vertex3D(float pX, float pY, float pZ);
	inline Vertex3D(const Vertex3D& pV);

	inline virtual void Set(const Vertex3D& pV);
	inline virtual void operator= (const Vertex3D& pV);

	inline void SetX(float pX);
	inline void SetY(float pY);
	inline void SetZ(float pZ);

	inline Lepra::FixedPointMath::Fixed28_4 GetX28_4() const;
	inline Lepra::FixedPointMath::Fixed28_4 GetY28_4() const;
	inline float GetX() const;
	inline float GetY() const;
	inline float GetZ() const;

	inline float GetOneOverZ() const;

private:

	Lepra::FixedPointMath::Fixed28_4 mX;
	Lepra::FixedPointMath::Fixed28_4 mY;
	float z;

	float mOneOverZ;
};




class Vertex3DUV : public Vertex3D
{
public:
	inline Vertex3DUV();
	inline Vertex3DUV(float pX, float pY, float pZ, float pU, float pV);

	inline virtual void Set(const Vertex3DUV& pV);
	inline virtual void operator= (const Vertex3DUV& pV);

	inline void SetU(float pU);
	inline void SetV(float pV);

	inline float GetUOverZ() const;
	inline float GetVOverZ() const;
private:
	float u;
	float v;

	float mUOverZ;
	float mVOverZ;
};




class Vertex3DRGB : public Vertex3D
{
public:
	inline Vertex3DRGB();
	inline Vertex3DRGB(float pX, float pY, float pZ, const Lepra::Color& pColor);

	inline virtual void Set(const Vertex3DRGB& pV);
	inline virtual void operator= (const Vertex3DRGB& pV);

	inline void SetColor(const Lepra::Color& pColor);
	inline void SetR(float pR);
	inline void SetG(float pG);
	inline void SetB(float pB);

	inline float GetR() const;
	inline float GetG() const;
	inline float GetB() const;

	inline float GetROverZ() const;
	inline float GetGOverZ() const;
	inline float GetBOverZ() const;
private:
	float r;
	float g;
	float b;

	float mROverZ;
	float mGOverZ;
	float mBOverZ;
};




class Vertex3DUVRGB : public Vertex3D
{
public:
	inline Vertex3DUVRGB();
	inline Vertex3DUVRGB(float pX, float pY, float pZ, float pU, float pV, const Lepra::Color& pColor);

	inline virtual void Set(const Vertex3DUVRGB& pV);
	inline virtual void operator= (const Vertex3DUVRGB& pV);

	inline void SetColor(const Lepra::Color& pColor);
	inline void SetU(float pU);
	inline void SetV(float pV);
	inline void SetR(float pR);
	inline void SetG(float pG);
	inline void SetB(float pB);

	inline float GetR() const;
	inline float GetG() const;
	inline float GetB() const;

	inline float GetUOverZ() const;
	inline float GetVOverZ() const;
	inline float GetROverZ() const;
	inline float GetGOverZ() const;
	inline float GetBOverZ() const;
private:
	float u;
	float v;
	float r;
	float g;
	float b;

	float mUOverZ;
	float mVOverZ;
	float mROverZ;
	float mGOverZ;
	float mBOverZ;
};




class Vertex3DUVM : public Vertex3D
{
public:
	inline Vertex3DUVM();
	inline Vertex3DUVM(float pX, float pY, float pZ, float pU, float pV, float pM);

	inline virtual void Set(const Vertex3DUVM& pV);
	inline virtual void operator= (const Vertex3DUVM& pV);

	inline void SetU(float pU);
	inline void SetV(float pV);
	inline void SetM(float pM);

	inline float GetUOverZ() const;
	inline float GetVOverZ() const;
	inline float GetMOverZ() const;
private:
	float u;
	float v;
	float m;

	float mUOverZ;
	float mVOverZ;
	float mMOverZ;
};




class Vertex3DUVRGBM : public Vertex3D
{
public:
	inline Vertex3DUVRGBM();
	inline Vertex3DUVRGBM(float pX, float pY, float pZ, float pU, float pV, float pM, const Lepra::Color& pColor);

	inline virtual void Set(const Vertex3DUVRGBM& pV);
	inline virtual void operator= (const Vertex3DUVRGBM& pV);

	inline void SetColor(const Lepra::Color& pColor);
	inline void SetU(float pU);
	inline void SetV(float pV);
	inline void SetR(float pR);
	inline void SetG(float pG);
	inline void SetB(float pB);
	inline void SetM(float pM);

	inline float GetR() const;
	inline float GetG() const;
	inline float GetB() const;

	inline float GetUOverZ() const;
	inline float GetVOverZ() const;
	inline float GetROverZ() const;
	inline float GetGOverZ() const;
	inline float GetBOverZ() const;
	inline float GetMOverZ() const;
private:
	float u;
	float v;
	float r;
	float g;
	float b;
	float m;

	float mUOverZ;
	float mVOverZ;
	float mROverZ;
	float mGOverZ;
	float mBOverZ;
	float mMOverZ;
};

//////////////////////////////////////////////////////////////////////////////
//
// 2D - Classes
//
//////////////////////////////////////////////////////////////////////////////

class Vertex2D
{
public:
	inline Vertex2D();
	inline Vertex2D(float pX, float pY);
	inline Vertex2D(const Vertex2D& pV);

	inline virtual void Set(const Vertex2D& pV);
	inline virtual void operator= (const Vertex2D& pV);

	inline void SetX(float pX);
	inline void SetY(float pY);

	inline Lepra::FixedPointMath::Fixed28_4 GetX28_4() const;
	inline Lepra::FixedPointMath::Fixed28_4 GetY28_4() const;
	inline float GetX() const;
	inline float GetY() const;

private:

	Lepra::FixedPointMath::Fixed28_4 mX;
	Lepra::FixedPointMath::Fixed28_4 mY;
};

class Vertex2DUV : public Vertex2D
{
public:
	inline Vertex2DUV();
	inline Vertex2DUV(float pX, float pY, float pU, float pV);

	inline virtual void Set(const Vertex2DUV& pV);
	inline virtual void operator= (const Vertex2DUV& pV);

	inline void SetU(float pU);
	inline void SetV(float pV);

	inline float GetU() const;
	inline float GetV() const;
private:
	float u;
	float v;
};

class Vertex2DRGBA : public Vertex2D
{
public:
	inline Vertex2DRGBA();
	inline Vertex2DRGBA(float pX, float pY, float pR, float pG, float pB, float pA);

	inline virtual void Set(const Vertex2DRGBA& pV);
	inline virtual void operator= (const Vertex2DRGBA& pV);

	inline void SetR(float pR);
	inline void SetG(float pG);
	inline void SetB(float pB);
	inline void SetA(float pA);

	inline float GetR() const;
	inline float GetG() const;
	inline float GetB() const;
	inline float GetA() const;
private:
	float r;
	float g;
	float b;
	float a;
};

//////////////////////////////////////////////////////////////////////////////
//
// Inline functions
//
//////////////////////////////////////////////////////////////////////////////




Vertex3D::Vertex3D()
{
	mX = 0;
	mY = 0;
	z = 0;

	mOneOverZ = 0;
}

Vertex3D::Vertex3D(float pX, float pY, float pZ)
{
	SetX(pX);
	SetY(pY);
	SetZ(pZ);
}

Vertex3D::Vertex3D(const Vertex3D& pV)
{
	Set(pV);
}

void Vertex3D::Set(const Vertex3D& pV)
{
	mX = pV.mX;
	mY = pV.mY;
	z = pV.z;

	mOneOverZ = pV.mOneOverZ;
}

void Vertex3D::operator= (const Vertex3D& pV)
{
	Set(pV);
}

void Vertex3D::SetX(float pX)
{
	mX = Lepra::FixedPointMath::FloatToFixed28_4(pX);
}

void Vertex3D::SetY(float pY)
{
	mY = Lepra::FixedPointMath::FloatToFixed28_4(pY);
}

void Vertex3D::SetZ(float pZ)
{
	z = pZ;
	mOneOverZ = 1.0f / z;
}

Lepra::FixedPointMath::Fixed28_4 Vertex3D::GetX28_4() const
{
	return mX;
}

Lepra::FixedPointMath::Fixed28_4 Vertex3D::GetY28_4() const
{
	return mY;
}

float Vertex3D::GetX() const
{
	return (float)Lepra::FixedPointMath::Fixed28_4ToFloat(mX);
}

float Vertex3D::GetY() const
{
	return (float)Lepra::FixedPointMath::Fixed28_4ToFloat(mY);
}

float Vertex3D::GetZ() const
{
	return z;
}

float Vertex3D::GetOneOverZ() const
{
	return mOneOverZ;
}



Vertex3DUV::Vertex3DUV() :
	Vertex3D()
{
	u = 0;
	v = 0;
	
	mUOverZ = 0;
	mVOverZ = 0;
}

Vertex3DUV::Vertex3DUV(float pX, float pY, float pZ, float pU, float pV) :
	Vertex3D(pX, pY, pZ)
{
	SetU(pU);
	SetV(pV);
}

void Vertex3DUV::Set(const Vertex3DUV& pV)
{
	Vertex3D::Set(pV);

	u = pV.u;
	v = pV.v;

	mUOverZ = pV.mUOverZ;
	mVOverZ = pV.mVOverZ;
}

void Vertex3DUV::operator= (const Vertex3DUV& pV)
{
	Set(pV);
}

void Vertex3DUV::SetU(float pU)
{
	u = pU;
	mUOverZ = u * GetOneOverZ();
}

void Vertex3DUV::SetV(float pV)
{
	v = pV;
	mVOverZ = v * GetOneOverZ();
}

float Vertex3DUV::GetUOverZ() const
{
	return mUOverZ;
}

float Vertex3DUV::GetVOverZ() const
{
	return mVOverZ;
}




Vertex3DRGB::Vertex3DRGB() :
	Vertex3D()
{
	r = 0;
	g = 0;
	b = 0;
	
	mROverZ = 0;
	mGOverZ = 0;
	mBOverZ = 0;
}

Vertex3DRGB::Vertex3DRGB(float pX, float pY, float pZ, const Lepra::Color& pColor) :
	Vertex3D(pX, pY, pZ)
{
	SetColor(pColor);
}

void Vertex3DRGB::Set(const Vertex3DRGB& pV)
{
	Vertex3D::Set(pV);

	r = pV.r;
	g = pV.g;
	b = pV.b;

	mROverZ = pV.mROverZ;
	mGOverZ = pV.mGOverZ;
	mBOverZ = pV.mBOverZ;
}

void Vertex3DRGB::operator= (const Vertex3DRGB& pV)
{
	Set(pV);
}

void Vertex3DRGB::SetColor(const Lepra::Color& pColor)
{
	SetR((float)pColor.mRed   / 255.0f);
	SetG((float)pColor.mGreen / 255.0f);
	SetB((float)pColor.mBlue  / 255.0f);
}

void Vertex3DRGB::SetR(float pR)
{
	r = pR;
	mROverZ = r * GetOneOverZ();
}

void Vertex3DRGB::SetG(float pG)
{
	g = pG;
	mGOverZ = g * GetOneOverZ();
}

void Vertex3DRGB::SetB(float pB)
{
	b = pB;
	mBOverZ = b * GetOneOverZ();
}

float Vertex3DRGB::GetR() const
{
	return r;
}

float Vertex3DRGB::GetG() const
{
	return g;
}

float Vertex3DRGB::GetB() const
{
	return b;
}

float Vertex3DRGB::GetROverZ() const
{
	return mROverZ;
}

float Vertex3DRGB::GetGOverZ() const
{
	return mGOverZ;
}

float Vertex3DRGB::GetBOverZ() const
{
	return mBOverZ;
}




Vertex3DUVRGB::Vertex3DUVRGB() :
	Vertex3D()
{
	u = 0;
	v = 0;
	r = 0;
	g = 0;
	b = 0;
	
	mUOverZ = 0;
	mVOverZ = 0;
	mROverZ = 0;
	mGOverZ = 0;
	mBOverZ = 0;
}

Vertex3DUVRGB::Vertex3DUVRGB(float pX, float pY, float pZ, float pU, float pV, const Lepra::Color& pColor) :
	Vertex3D(pX, pY, pZ)
{
	SetU(pU);
	SetV(pV);
	SetColor(pColor);
}

void Vertex3DUVRGB::Set(const Vertex3DUVRGB& pV)
{
	Vertex3D::Set(pV);

	u = pV.u;
	v = pV.v;
	r = pV.r;
	g = pV.g;
	b = pV.b;

	mUOverZ = pV.mUOverZ;
	mVOverZ = pV.mVOverZ;
	mROverZ = pV.mROverZ;
	mGOverZ = pV.mGOverZ;
	mBOverZ = pV.mBOverZ;
}

void Vertex3DUVRGB::operator= (const Vertex3DUVRGB& pV)
{
	Set(pV);
}

void Vertex3DUVRGB::SetColor(const Lepra::Color& pColor)
{
	SetR((float)pColor.mRed   / 255.0f);
	SetG((float)pColor.mGreen / 255.0f);
	SetB((float)pColor.mBlue  / 255.0f);
}

void Vertex3DUVRGB::SetU(float pU)
{
	u = pU;
	mUOverZ = u * GetOneOverZ();
}

void Vertex3DUVRGB::SetV(float pV)
{
	v = pV;
	mVOverZ = v * GetOneOverZ();
}

void Vertex3DUVRGB::SetR(float pR)
{
	r = pR;
	mROverZ = r * GetOneOverZ();
}

void Vertex3DUVRGB::SetG(float pG)
{
	g = pG;
	mGOverZ = g * GetOneOverZ();
}

void Vertex3DUVRGB::SetB(float pB)
{
	b = pB;
	mBOverZ = b * GetOneOverZ();
}

float Vertex3DUVRGB::GetR() const
{
	return r;
}

float Vertex3DUVRGB::GetG() const
{
	return g;
}

float Vertex3DUVRGB::GetB() const
{
	return b;
}

float Vertex3DUVRGB::GetUOverZ() const
{
	return mUOverZ;
}

float Vertex3DUVRGB::GetVOverZ() const
{
	return mVOverZ;
}

float Vertex3DUVRGB::GetROverZ() const
{
	return mROverZ;
}

float Vertex3DUVRGB::GetGOverZ() const
{
	return mGOverZ;
}

float Vertex3DUVRGB::GetBOverZ() const
{
	return mBOverZ;
}




Vertex3DUVM::Vertex3DUVM() :
	Vertex3D()
{
	u = 0;
	v = 0;
	m = 0;
	
	mUOverZ = 0;
	mVOverZ = 0;
	mMOverZ = 0;
}

Vertex3DUVM::Vertex3DUVM(float pX, float pY, float pZ, float pU, float pV, float pM) :
	Vertex3D(pX, pY, pZ)
{
	SetU(pU);
	SetV(pV);
	SetM(pM);
}

void Vertex3DUVM::Set(const Vertex3DUVM& pV)
{
	Vertex3D::Set(pV);

	u = pV.u;
	v = pV.v;
	m = pV.m;

	mUOverZ = pV.mUOverZ;
	mVOverZ = pV.mVOverZ;
	mMOverZ = pV.mMOverZ;
}

void Vertex3DUVM::operator= (const Vertex3DUVM& pV)
{
	Set(pV);
}

void Vertex3DUVM::SetU(float pU)
{
	u = pU;
	mUOverZ = u * GetOneOverZ();
}

void Vertex3DUVM::SetV(float pV)
{
	v = pV;
	mVOverZ = v * GetOneOverZ();
}

void Vertex3DUVM::SetM(float pM)
{
	m = pM;
	mMOverZ = m * GetOneOverZ();
}

float Vertex3DUVM::GetUOverZ() const
{
	return mUOverZ;
}

float Vertex3DUVM::GetVOverZ() const
{
	return mVOverZ;
}

float Vertex3DUVM::GetMOverZ() const
{
	return mMOverZ;
}




Vertex3DUVRGBM::Vertex3DUVRGBM() :
	Vertex3D()
{
	u = 0;
	v = 0;
	r = 0;
	g = 0;
	b = 0;
	m = 0;
	
	mUOverZ = 0;
	mVOverZ = 0;
	mROverZ = 0;
	mGOverZ = 0;
	mBOverZ = 0;
	mMOverZ = 0;
}

Vertex3DUVRGBM::Vertex3DUVRGBM(float pX, float pY, float pZ, float pU, float pV, float pM, const Lepra::Color& pColor) :
	Vertex3D(pX, pY, pZ)
{
	SetU(pU);
	SetV(pV);
	SetM(pM);
	SetColor(pColor);
}

void Vertex3DUVRGBM::Set(const Vertex3DUVRGBM& pV)
{
	Vertex3D::Set(pV);

	u = pV.u;
	v = pV.v;
	r = pV.r;
	g = pV.g;
	b = pV.b;
	m = pV.m;

	mUOverZ = pV.mUOverZ;
	mVOverZ = pV.mVOverZ;
	mROverZ = pV.mROverZ;
	mGOverZ = pV.mGOverZ;
	mBOverZ = pV.mBOverZ;
	mMOverZ = pV.mMOverZ;
}

void Vertex3DUVRGBM::operator= (const Vertex3DUVRGBM& pV)
{
	Set(pV);
}

void Vertex3DUVRGBM::SetColor(const Lepra::Color& pColor)
{
	SetR((float)pColor.mRed   / 255.0f);
	SetG((float)pColor.mGreen / 255.0f);
	SetB((float)pColor.mBlue  / 255.0f);
}

void Vertex3DUVRGBM::SetU(float pU)
{
	u = pU;
	mUOverZ = u * GetOneOverZ();
}

void Vertex3DUVRGBM::SetV(float pV)
{
	v = pV;
	mVOverZ = v * GetOneOverZ();
}

void Vertex3DUVRGBM::SetR(float pR)
{
	r = pR;
	mROverZ = r * GetOneOverZ();
}

void Vertex3DUVRGBM::SetG(float pG)
{
	g = pG;
	mGOverZ = g * GetOneOverZ();
}

void Vertex3DUVRGBM::SetB(float pB)
{
	b = pB;
	mBOverZ = b * GetOneOverZ();
}

void Vertex3DUVRGBM::SetM(float pM)
{
	m = pM;
	mMOverZ = m * GetOneOverZ();
}

float Vertex3DUVRGBM::GetR() const
{
	return r;
}

float Vertex3DUVRGBM::GetG() const
{
	return g;
}

float Vertex3DUVRGBM::GetB() const
{
	return b;
}

float Vertex3DUVRGBM::GetUOverZ() const
{
	return mUOverZ;
}

float Vertex3DUVRGBM::GetVOverZ() const
{
	return mVOverZ;
}

float Vertex3DUVRGBM::GetROverZ() const
{
	return mROverZ;
}

float Vertex3DUVRGBM::GetGOverZ() const
{
	return mGOverZ;
}

float Vertex3DUVRGBM::GetBOverZ() const
{
	return mBOverZ;
}

float Vertex3DUVRGBM::GetMOverZ() const
{
	return mMOverZ;
}


//////////////////////////////////////////////////////////////////////////////
//
// Inline functions for 2D-classes.
//
//////////////////////////////////////////////////////////////////////////////

Vertex2D::Vertex2D() :
	mX(0),
	mY(0)
{
}

Vertex2D::Vertex2D(float pX, float pY)
{
	SetX(pX);
	SetY(pY);
}

Vertex2D::Vertex2D(const Vertex2D& pV)
{
	Set(pV);
}

void Vertex2D::Set(const Vertex2D& pV)
{
	mX = pV.mX;
	mY = pV.mY;
}

void Vertex2D::operator= (const Vertex2D& pV)
{
	Set(pV);
}

void Vertex2D::SetX(float pX)
{
	mX = Lepra::FixedPointMath::FloatToFixed28_4(pX);
}

void Vertex2D::SetY(float pY)
{
	mY = Lepra::FixedPointMath::FloatToFixed28_4(pY);
}

Lepra::FixedPointMath::Fixed28_4 Vertex2D::GetX28_4() const
{
	return mX;
}

Lepra::FixedPointMath::Fixed28_4 Vertex2D::GetY28_4() const
{
	return mY;
}

float Vertex2D::GetX() const
{
	return (float)Lepra::FixedPointMath::Fixed28_4ToFloat(mX);
}

float Vertex2D::GetY() const
{
	return (float)Lepra::FixedPointMath::Fixed28_4ToFloat(mY);
}




Vertex2DUV::Vertex2DUV() :
	Vertex2D(),
	u(0),
	v(0)
{
}

Vertex2DUV::Vertex2DUV(float pX, float pY, float pU, float pV) :
	Vertex2D(pX, pY),
	u(pU),
	v(pV)
{
}

void Vertex2DUV::Set(const Vertex2DUV& pV)
{
	Vertex2D::Set(pV);

	u = pV.u;
	v = pV.v;
}

void Vertex2DUV::operator= (const Vertex2DUV& pV)
{
	Set(pV);
}

void Vertex2DUV::SetU(float pU)
{
	u = pU;
}

void Vertex2DUV::SetV(float pV)
{
	v = pV;
}

float Vertex2DUV::GetU() const
{
	return u;
}

float Vertex2DUV::GetV() const
{
	return v;
}


Vertex2DRGBA::Vertex2DRGBA() :
	Vertex2D(),
	r(0),
	g(0),
	b(0),
	a(0)
{
}

Vertex2DRGBA::Vertex2DRGBA(float pX, float pY, float pR, float pG, float pB, float pA) :
	Vertex2D(pX, pY),
	r(pR),
	g(pG),
	b(pB),
	a(pA)
{
}

void Vertex2DRGBA::Set(const Vertex2DRGBA& pV)
{
	Vertex2D::Set(pV);
	r = pV.r;
	g = pV.g;
	b = pV.b;
	a = pV.a;
}

void Vertex2DRGBA::operator= (const Vertex2DRGBA& pV)
{
	Set(pV);
}

void Vertex2DRGBA::SetR(float pR)
{
	r = pR;
}

void Vertex2DRGBA::SetG(float pG)
{
	g = pG;
}

void Vertex2DRGBA::SetB(float pB)
{
	b = pB;
}

void Vertex2DRGBA::SetA(float pA)
{
	a = pA;
}

float Vertex2DRGBA::GetR() const
{
	return r;
}

float Vertex2DRGBA::GetG() const
{
	return g;
}

float Vertex2DRGBA::GetB() const
{
	return b;
}

float Vertex2DRGBA::GetA() const
{
	return a;
}

} // End namespace.

#endif