
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games

// The base class TerrainFunction should be sub-classed; this is where the actual implementation goes.
// All terrain functions inherit a behavior where the range of the function is restricted by two radii.
// Within the InnerRadius, the function is unrestricted. In the area between the InnerRadius and the
// OuterRadius, the function must scale (typically linear) down to 0. Outside OuterRadius, the function
// must be 0.

// The method AddPoint() adds the value of the function to a given 3D point within a grid.

// The TerrainFunction base class calls the virtual method AddPoint() in the sub-class for each point
// it affects in the grid. The x and y parameters for AddPoint() have been scaled down to range [0,1],
// where the extents of the function intersect the axis-aligned square with side OuterRaduis*2.



#ifndef TERRAINFUNCTION_H
#define TERRAINFUNCTION_H



#include "../../Lepra/Include/CubicDeCasteljauSpline.h"
#include "../../Lepra/Include/Vector2D.h"
#include "../../Lepra/Include/Vector3D.h"
#include "TerrainPatch.h"
#include "TBC.h"



namespace TBC
{

// The underlying function does generally not generate a hightfield, but instead a plain mesh.
// The whole terrain system is built around meshes, not hightfields.
class TerrainFunction : public TerrainPatch::Modifier
{
public:
	TerrainFunction(float pAmplitude, const Lepra::Vector2DF& pPosition, float pInnerRadius, float pOuterRadius);
	virtual ~TerrainFunction();

	void AddFunction(TerrainPatch& pPatch) const;

	float GetAmplitude() const;
	const Lepra::Vector2DF& GetPosition() const;
	float GetInnerRadius() const;
	float GetOuterRadius() const;

	// From TerrainPatch::Modifier
	void ModifyVertex(const Lepra::Vector2DF& pWorldFlatPos, Lepra::Vector3DF& pVertex) const;

	// Parameters pRelativeNormalizedX, pRelativeNormalizedY and pScale have been scaled to inclusive range [0,1].
	virtual void AddPoint(float pRelativeNormalizedX, float pRelativeNormalizedY, float pScale, float pAbsoluteXyDistance, Lepra::Vector3DF& pPoint) const = 0;

protected:
	float mAmplitude;
	Lepra::Vector2DF mPosition;
	float mInnerRadius;
	float mOuterRadius;
};

class TerrainFunctionGroup : public TerrainPatch::Modifier
{
public:
	// TODO: Add a linked list version?
	TerrainFunctionGroup(TerrainFunction** pTFArray, int pCount, 
		Lepra::DataPolicy pArrayPolicy = Lepra::FULL_COPY,
		Lepra::SubDataPolicy pTFPolicy = Lepra::KEEP_REFERENCE);
	~TerrainFunctionGroup();

	// Adds all functions at once.
	void AddFunctions(TerrainPatch& pPatch) const;

	void ModifyVertex(const Lepra::Vector2DF& pWorldFlatPos, Lepra::Vector3DF& pVertex) const;

private:
	TerrainFunction** mTFArray;
	int mCount;
	Lepra::DataPolicy mArrayPolicy;
	Lepra::SubDataPolicy mTFPolicy;
};

// Generates a cone without hang over (hehe), but using strech.
class TerrainConeFunction: public TerrainFunction
{
public:
	TerrainConeFunction(float pAmplitude, const Lepra::Vector2DF& pPosition, float pInnerRadius, float pOuterRadius);

private:
	void AddPoint(float pRelativeNormalizedX, float pRelativeNormalizedY, float pScale, float pAbsoluteXyDistance, Lepra::Vector3DF& pPoint) const;

	float mRadiusDifferance;
	float mSlopeLength;
	float mProfileLength;
	float mPlateauPart;
};

// Generates a hemisphere without hang over, but using strech.
class TerrainHemisphereFunction: public TerrainFunction
{
public:
	TerrainHemisphereFunction(float pAmplitude, const Lepra::Vector2DF& pPosition, float pInnerRadius, float pOuterRadius);

private:
	void AddPoint(float pRelativeNormalizedX, float pRelativeNormalizedY, float pScale, float pAbsoluteXyDistance, Lepra::Vector3DF& pPoint) const;
};

// Generates a dune (from -X to +X) which is really a spline profile (in the XZ-plane).
class TerrainDuneFunction: public TerrainFunction
{
public:
	TerrainDuneFunction(float pWidthProportion, float pCurvature, float pAmplitude, const Lepra::Vector2DF& pPosition, float pInnerRadius, float pOuterRadius);
	virtual ~TerrainDuneFunction();

private:
	void AddPoint(float pRelativeNormalizedX, float pRelativeNormalizedY, float pScale, float pAbsoluteXyDistance, Lepra::Vector3DF& pPoint) const;

	Lepra::CubicDeCasteljauSpline<Lepra::Vector2DF, float>* mProfileSpline;
	float mWidthProportion;
};

// Decorator design pattern. Used for inheritance.
class TerrainDecorator
{
public:
	TerrainDecorator(TerrainFunction* pFunction);
	virtual ~TerrainDecorator();

protected:
	TerrainFunction* mFunction;
};

// Decorator design pattern. Multiplies with the underlying function (from -Y to +Y) with a one-dimensional spline (in the YZ-plane).
class TerrainAmplitudeFunction: public TerrainFunction, protected TerrainDecorator
{
public:
	TerrainAmplitudeFunction(const float* pAmplitudeVector, unsigned pAmplitudeVectorLength, TerrainFunction* pFunction);
	virtual ~TerrainAmplitudeFunction();

private:
	void AddPoint(float pRelativeNormalizedX, float pRelativeNormalizedY, float pScale, float pAbsoluteXyDistance, Lepra::Vector3DF& pPoint) const;

	Lepra::CubicDeCasteljauSpline<float, float, float>* mAmplitudeSpline;
};

// Decorator design pattern. Compresses/streches the decorated function along the X-axis.
class TerrainWidthFunction: public TerrainFunction, protected TerrainDecorator
{
public:
	TerrainWidthFunction(float pWidthFactor, TerrainFunction* pFunction);

private:
	void AddPoint(float pRelativeNormalizedX, float pRelativeNormalizedY, float pScale, float pAbsoluteXyDistance, Lepra::Vector3DF& pPoint) const;

	float mWidthFactor;
};

// Decorator design pattern. Multiplies with the underlying function (from -Y to +Y) with a one-dimensional spline (in the YZ-plane).
class TerrainPushFunction: public TerrainFunction, protected TerrainDecorator
{
public:
	TerrainPushFunction(const float* pPushVector, unsigned pPushVectorLength, TerrainFunction* pFunction);
	virtual ~TerrainPushFunction();

private:
	void AddPoint(float pRelativeNormalizedX, float pRelativeNormalizedY, float pScale, float pAbsoluteXyDistance, Lepra::Vector3DF& pPoint) const;

	Lepra::CubicDeCasteljauSpline<float, float, float>* mPushSpline;
};

// Decorator design pattern. Transforms the underlying function (in the XY-plane) along a two-dimensional spline (in the XY-plane).
class TerrainRotateFunction: public TerrainFunction, protected TerrainDecorator
{
public:
	TerrainRotateFunction(float pAngle, TerrainFunction* pFunction);

private:
	void AddPoint(float pRelativeNormalizedX, float pRelativeNormalizedY, float pScale, float pAbsoluteXyDistance, Lepra::Vector3DF& pPoint) const;

	float mAngle;
};



}



#endif // !TERRAINFUNCTION_H
