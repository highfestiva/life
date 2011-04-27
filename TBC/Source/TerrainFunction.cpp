
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include <assert.h>
#include "../../Lepra/Include/CubicSpline.h"
#include "../../Lepra/Include/Math.h"
#include "../Include/TerrainFunction.h"



namespace TBC
{

TerrainFunction::TerrainFunction(float pAmplitude, const Vector2DF& pPosition, float pInnerRadius, float pOuterRadius):
	mAmplitude(pAmplitude),
	mPosition(pPosition),
	mInnerRadius(pInnerRadius),
	mOuterRadius(pOuterRadius)
{
	assert(mInnerRadius <= mOuterRadius);	// Outer radius should be AT LEAST as big as the inner ditto.
}

TerrainFunction::~TerrainFunction()
{
}

void TerrainFunction::AddFunction(TerrainPatch& pPatch) const
{
	// Read all frequently used data from pGrid.
	const Vector2DF& lWorldSouthWest(pPatch.GetSouthWest());
	const Vector2DF& lWorldNorthEast(pPatch.GetNorthEast());
	const int lVertexCountX = pPatch.GetVertexRes();
	const int lVertexCountY = pPatch.GetVertexRes();
	const float lGridWidth = pPatch.GetPatchSize();
	const float lGridHeight = pPatch.GetPatchSize();

	float a;

	// Calculate start X vertex index.
	int lVertexMinimumX;
	a = mPosition.x-mOuterRadius;
	if (a <= lWorldSouthWest.x)
	{
		lVertexMinimumX = 0;
	}
	else
	{
		lVertexMinimumX = (int)::floor((a-lWorldSouthWest.x) * lVertexCountX / lGridWidth);
	}

	// Calculate end X vertex index.
	int lVertexMaximumX;
	a = mPosition.x+mOuterRadius;
	if (a >= lWorldNorthEast.x)
	{
		lVertexMaximumX = lVertexCountX;
	}
	else
	{
		lVertexMaximumX = (int)::ceil((a-lWorldSouthWest.x) * lVertexCountX / lGridWidth);
	}

	// Calculate start Y vertex index.
	int lVertexMinimumY;
	a = mPosition.y-mOuterRadius;
	if (a <= lWorldSouthWest.y)
	{
		lVertexMinimumY = 0;
	}
	else
	{
		lVertexMinimumY = (int)::floor((a-lWorldSouthWest.y) * lVertexCountY / lGridHeight);
	}

	// Calculate end Y vertex index.
	int lVertexMaximumY;
	a = mPosition.y+mOuterRadius;
	if (a >= lWorldNorthEast.y)
	{
		lVertexMaximumY = lVertexCountY;
	}
	else
	{
		lVertexMaximumY = (int)::ceil((a-lWorldSouthWest.y) * lVertexCountY / lGridHeight);
	}

	pPatch.IterateOverPatch(*this, lVertexMinimumX, lVertexMaximumX, lVertexMinimumY, lVertexMaximumY);
}

void TerrainFunction::ModifyVertex(const Vector2DF& pWorldFlatPos, Vector3DF& pVertex) const
{
	const float lDistance = pWorldFlatPos.GetDistance(mPosition);
	if (lDistance < mOuterRadius)
	{
		// We made it!
		float lScale;
		if (lDistance <= mInnerRadius)
		{
			// This point is inside, or on, the inner radius.
			lScale = 1;
		}
		else
		{
			// We linearly down-scale our method parameter. This does not necessarily mean
			// that the parameter is used in a linear fasion.
			lScale = 1-(lDistance-mInnerRadius)/(mOuterRadius-mInnerRadius);
		}
		const Vector2DF lRelativeNormalizedPos = (pWorldFlatPos - mPosition) / mOuterRadius;
		AddPoint(lRelativeNormalizedPos.x, lRelativeNormalizedPos.y, lScale, lDistance, pVertex);
	}
}

float TerrainFunction::GetAmplitude() const
{
	return (mAmplitude);
}

const Vector2DF& TerrainFunction::GetPosition() const
{
	return (mPosition);
}

float TerrainFunction::GetInnerRadius() const
{
	return (mInnerRadius);
}

float TerrainFunction::GetOuterRadius() const
{
	return (mOuterRadius);
}


TerrainFunctionGroup::TerrainFunctionGroup(TerrainFunction** pTFArray, int pCount, 
					   DataPolicy pArrayPolicy,
					   SubDataPolicy pTFPolicy) :
	mTFArray(pTFArray),
	mCount(pCount),
	mArrayPolicy(pArrayPolicy),
	mTFPolicy(pTFPolicy)
{
	if (mArrayPolicy == FULL_COPY)
	{
		mTFArray = new TerrainFunction*[mCount];
		::memcpy(mTFArray, pTFArray, mCount * sizeof(TerrainFunction*));
	}
}

TerrainFunctionGroup::~TerrainFunctionGroup()
{
	if (mTFPolicy == TAKE_SUBDATA_OWNERSHIP)
	{
		for (int i = 0; i < mCount; i++)
		{
			delete mTFArray[i];
		}
	}

	if (mArrayPolicy != COPY_REFERENCE)
	{
		delete[] mTFArray;
	}
}

void TerrainFunctionGroup::AddFunctions(TerrainPatch& pPatch) const
{
	// No optimizations like the ones in TerrainFunction are possible.
	pPatch.IterateOverPatch(*this, 0, pPatch.GetVertexRes(), 0, pPatch.GetVertexRes());
}

void TerrainFunctionGroup::ModifyVertex(const Vector2DF& pWorldFlatPos, Vector3DF& pVertex) const
{
	for (int i = 0; i < mCount; i++)
	{
		mTFArray[i]->ModifyVertex(pWorldFlatPos, pVertex);
	}
}

TerrainConeFunction::TerrainConeFunction(float pAmplitude, const Vector2DF& pPosition, float pInnerRadius, float pOuterRadius):
	TerrainFunction(pAmplitude, pPosition, pInnerRadius, pOuterRadius)
{
	// Calculate the "profile length", which is the slope plus the inner radius.
	//      ____|____
	//   pl/ ir |    \
	// ___/     |     \___
	//    or
	mRadiusDifferance = mOuterRadius-mInnerRadius;
	mSlopeLength = ::sqrt(mRadiusDifferance*mRadiusDifferance + pAmplitude*pAmplitude);
	mProfileLength = mSlopeLength + mInnerRadius;
	mPlateauPart = mInnerRadius/mProfileLength;
}

void TerrainConeFunction::AddPoint(float pRelativeNormalizedX, float pRelativeNormalizedY,
	float /*pScale*/, float pAbsoluteXyDistance, Vector3DF& pPoint) const
{
	// Does not simply crunch the terrain grid vertically, but has a smarter
	// function that distributes the grid vertices evenly around the cone profile.

	//pScale;

	// Calculate a new distance, where the slope is weighed in correctly.
	const float lNormalizedXyDistance = pAbsoluteXyDistance/mOuterRadius;
	float lNewAbsoluteXyDistance;
	if (lNormalizedXyDistance < mPlateauPart)
	{
		// We're inside the inner radius = simple scale.
		lNewAbsoluteXyDistance = lNormalizedXyDistance/mPlateauPart*mInnerRadius;
	}
	else
	{
		// We're between the inner radius and the outer radius.
		const float lSlopePart = 1-mPlateauPart;
		lNewAbsoluteXyDistance = (lNormalizedXyDistance-mPlateauPart)/lSlopePart*mRadiusDifferance + mInnerRadius;
	}

	// Move X and Y out.
	if(pAbsoluteXyDistance != 0.0f)
	{
		const float lXyScale = mOuterRadius/pAbsoluteXyDistance;
		float dx = pRelativeNormalizedX*lXyScale*lNewAbsoluteXyDistance;
		float dy = pRelativeNormalizedY*lXyScale*lNewAbsoluteXyDistance;
		pPoint.x = mPosition.x+dx;
		pPoint.y = mPosition.y+dy;
	}

	// Calculate new amplitude from the new distance.
	float lNewAmplitude;
	if (lNewAbsoluteXyDistance <= mInnerRadius)
	{
		lNewAmplitude = mAmplitude;
	}
	else
	{
		lNewAmplitude = (mOuterRadius-lNewAbsoluteXyDistance)/(mOuterRadius-mInnerRadius)*mAmplitude;
	}
	pPoint.z += lNewAmplitude;
}



TerrainHemisphereFunction::TerrainHemisphereFunction(float pAmplitude, const Vector2DF& pPosition, float pInnerRadius, float pOuterRadius):
	TerrainFunction(pAmplitude, pPosition, pInnerRadius, pOuterRadius)
{
	//     ___
	//   /     \
	// _|       |_
}

void TerrainHemisphereFunction::AddPoint(float pRelativeNormalizedX, float pRelativeNormalizedY,
	float pScale, float pAbsoluteXyDistance, Vector3DF& pPoint) const
{
	// Does not simply crunch the terrain grid vertically, but has a smarter
	// function that distributes the grid vertices evenly around the hemisphere.

	// Calculate dx, dy and dz (the radius is 1).
	float dx = pRelativeNormalizedX;
	float dy = pRelativeNormalizedY;
	float dz = ::sqrt(1 - (dx*dx+dy*dy));
	// Angle between XY-plane and the point (dx, dy, dz).
	const float lAlpha = ::asin(dz/1);	// Divided by normalized radius 1.
	// This is the actual function. The new angle strives with a power of two towards the steepest
	const float lBeta = 2/PIF*lAlpha*lAlpha;
	if (pAbsoluteXyDistance != 0)
	{
		// Calculate new distance from center in XY-plane.
		const float lNewAbsoluteXyDistance = mOuterRadius*::cos(lBeta);
		const float lNewLinearXyDistanceScale = lNewAbsoluteXyDistance/pAbsoluteXyDistance;
		// Elongate dx and dy, since the angle in the XY-plane hasn't changed.
		dx *= lNewLinearXyDistanceScale*mOuterRadius;
		dy *= lNewLinearXyDistanceScale*mOuterRadius;
		pPoint.x = Math::Lerp(pPoint.x, mPosition.x+dx, pScale);
		pPoint.y = Math::Lerp(pPoint.y, mPosition.y+dy, pScale);
	}
	// Recalculate dz with the new vertical angle from our vector (dx, dy).
	// We don't scale it up by mOuterRadius (to make it a perfect hemisphere),
	// but instead by mAmplitude so that its size can be user controlled.
	dz = mAmplitude*::sin(lBeta);

	pPoint.z += dz*pScale;
}



TerrainDuneFunction::TerrainDuneFunction(float pWidthProportion, float pCurvature, float pAmplitude,
	const Vector2DF& pPosition, float pInnerRadius, float pOuterRadius):
	TerrainFunction(pAmplitude, pPosition, pInnerRadius, pOuterRadius),
	mProfileSpline(0),
	mWidthProportion(pWidthProportion)
{
	Vector2DF* lCoordinates = new Vector2DF[10];
	float* lTimes = new float[11];
	lTimes[0] = -1.0f;
	lCoordinates[0].x = -1;
	lCoordinates[0].y = 0;
	lTimes[1] = -0.5f;
	lCoordinates[1].x = -0.1f;
	lCoordinates[1].y = 0;
	lTimes[2] = -0.35f;
	lCoordinates[2].x = 0.4f*pCurvature;
	lCoordinates[2].y = -0.25f;
	lTimes[3] = -0.2f;
	lCoordinates[3].x = -0.05f*pCurvature;
	lCoordinates[3].y = 0.41f+pCurvature*0.4f;
	lTimes[4] = -0.1f;
	lCoordinates[4].x = -0.18f*pCurvature;
	lCoordinates[4].y = 0.58f-pCurvature*0.4f;
	lTimes[5] = 0.0f;
	lCoordinates[5].x = -0.3f*pCurvature;
	lCoordinates[5].y = 0.75f;
	lTimes[6] = 0.1f;
	lCoordinates[6].x = 0.0f;
	lCoordinates[6].y = 1.0f;
	lTimes[7] = 0.2f;
	lCoordinates[7].x = 0.2f;
	lCoordinates[7].y = 0.3f;
	lTimes[8] = 0.6f;
	lCoordinates[8].x = 0.6f;
	lCoordinates[8].y = 0.0f;
	lTimes[9] = 0.9f;
	lCoordinates[9].x = 1.0f;
	lCoordinates[9].y = 0.0f;
	lTimes[10] = 1.0f;
	mProfileSpline = new CubicDeCasteljauSpline<Vector2DF, float>(lCoordinates,
		lTimes, 10, CubicDeCasteljauSpline<Vector2DF, float>::TYPE_BSPLINE,
		TAKE_OWNERSHIP);
	mProfileSpline->StartInterpolation(-1);
}

TerrainDuneFunction::~TerrainDuneFunction()
{
	delete (mProfileSpline);
	mProfileSpline = 0;
}

void TerrainDuneFunction::AddPoint(float pRelativeNormalizedX, float pRelativeNormalizedY, float pScale, float, Vector3DF& pPoint) const
{
	mProfileSpline->GotoAbsoluteTime(pRelativeNormalizedX);
	Vector2DF d;
	// Create a small bulge on the edges.
	const float t = 3*pRelativeNormalizedX;
	const float bz = ::exp(-t*t/mWidthProportion)*0.5f;

	float lYScale = ::fabs(pRelativeNormalizedY)*mOuterRadius;
	if (lYScale > mInnerRadius)
	{
		lYScale = 1-(lYScale-mInnerRadius)/(mOuterRadius-mInnerRadius);
	}
	else
	{
		lYScale = 1;
	}

	d = mProfileSpline->GetValue();
	if (lYScale < 0.5f)
	{
		d.x = Math::Lerp(pRelativeNormalizedX, d.x, lYScale*2);
		d.y = bz*lYScale*2;
	}
	else
	{
		d.y = Math::Lerp(bz, d.y, (lYScale-0.5f)*2);
	}

	pPoint.x = Math::Lerp(pPoint.x, mPosition.x+d.x*mOuterRadius, pScale);
	pPoint.z += d.y*mAmplitude*pScale;
}



TerrainDecorator::TerrainDecorator(TerrainFunction* pFunction):
	mFunction(pFunction)
{
}

TerrainDecorator::~TerrainDecorator()
{
	mFunction = 0;
}



TerrainAmplitudeFunction::TerrainAmplitudeFunction(const float* pAmplitudeVector, unsigned pAmplitudeVectorLength, TerrainFunction* pFunction):
	TerrainFunction(1.0, pFunction->GetPosition(), pFunction->GetInnerRadius(), pFunction->GetOuterRadius()),
	TerrainDecorator(pFunction),
	mAmplitudeSpline(0)
{
	assert(pAmplitudeVectorLength >= 2);

	const float lTimeStep = 2.0f/(pAmplitudeVectorLength+1-1);
	float lTime = -1;
	float* lTimes = new float[pAmplitudeVectorLength+1];
	for (unsigned x = 0; x < pAmplitudeVectorLength+1; ++x)
	{
		lTimes[x] = lTime;
		lTime += lTimeStep;
	}
	mAmplitudeSpline = new CubicDeCasteljauSpline<float, float, float>((float*)pAmplitudeVector,
		lTimes, pAmplitudeVectorLength, CubicDeCasteljauSpline<float, float, float>::TYPE_CATMULLROM,
		FULL_COPY);
	delete[] (lTimes);
	mAmplitudeSpline->StartInterpolation(-1);
}

TerrainAmplitudeFunction::~TerrainAmplitudeFunction()
{
	delete (mAmplitudeSpline);
	mAmplitudeSpline = 0;
}

void TerrainAmplitudeFunction::AddPoint(float pRelativeNormalizedX, float pRelativeNormalizedY, float pScale,
	float pAbsoluteXyDistance, Vector3DF& pPoint) const
{
	const float oz = pPoint.z;
	mFunction->AddPoint(pRelativeNormalizedX, pRelativeNormalizedY, pScale, pAbsoluteXyDistance, pPoint);
	mAmplitudeSpline->GotoAbsoluteTime(pRelativeNormalizedY);
	const float lAmplitude = mAmplitudeSpline->GetValue();
	const float dz = pPoint.z-oz;
	pPoint.z = oz+dz*lAmplitude;
}



TerrainWidthFunction::TerrainWidthFunction(float pWidthFactor, TerrainFunction* pFunction):
	TerrainFunction(1.0, pFunction->GetPosition(), pFunction->GetInnerRadius(), pFunction->GetOuterRadius()),
	TerrainDecorator(pFunction),
	mWidthFactor(pWidthFactor)
{
}

void TerrainWidthFunction::AddPoint(float pRelativeNormalizedX, float pRelativeNormalizedY, float pScale,
	float pAbsoluteXyDistance, Vector3DF& pPoint) const
{
	const float rx = pRelativeNormalizedX/mWidthFactor;
	pAbsoluteXyDistance = ::sqrt(rx*rx+pRelativeNormalizedY*pRelativeNormalizedY)*mOuterRadius;
	if (pAbsoluteXyDistance >= mOuterRadius)
	{
		return;	// TRICKY: optimization by quick return.
	}
	else if (pAbsoluteXyDistance <= mInnerRadius)
	{
		pScale = 1;
	}
	else
	{
		pScale = 1-(pAbsoluteXyDistance-mInnerRadius)/(mOuterRadius-mInnerRadius);
	}
	float mx = pPoint.x;
	pPoint.x = mPosition.x+(pPoint.x-mPosition.x)/mWidthFactor;
	mx -= pPoint.x;
	mFunction->AddPoint(rx, pRelativeNormalizedY, pScale, pAbsoluteXyDistance, pPoint);
	pPoint.x += mx;
	//pPoint.x = (pPoint.x-lOriginalPoint.x)*mWidthFactor*pScale;
	//pPoint.x = Math::Lerp(pPoint.x, (pPoint.x-mPosition.x)*mWidthFactor + mPosition.x, pScale);
}


TerrainPushFunction::TerrainPushFunction(const float* pPushVector, unsigned pPushVectorLength, TerrainFunction* pFunction):
	TerrainFunction(1.0, pFunction->GetPosition(), pFunction->GetInnerRadius(), pFunction->GetOuterRadius()),
	TerrainDecorator(pFunction),
	mPushSpline(0)
{
	assert(pPushVectorLength >= 2);

	const float lTimeStep = 2.0f/(pPushVectorLength+1-1);
	float lTime = -1;
	float* lTimes = new float[pPushVectorLength+1];
	for (unsigned x = 0; x < pPushVectorLength+1; ++x)
	{
		lTimes[x] = lTime;
		lTime += lTimeStep;
	}
	mPushSpline = new CubicDeCasteljauSpline<float, float, float>((float*)pPushVector,
		lTimes, pPushVectorLength, CubicDeCasteljauSpline<float, float, float>::TYPE_CATMULLROM,
		FULL_COPY);
	delete[] (lTimes);
	mPushSpline->StartInterpolation(-1);
}

TerrainPushFunction::~TerrainPushFunction()
{
	delete (mPushSpline);
	mPushSpline = 0;
}

void TerrainPushFunction::AddPoint(float pRelativeNormalizedX, float pRelativeNormalizedY, float pScale,
	float pAbsoluteXyDistance, Vector3DF& pPoint) const
{
	mFunction->AddPoint(pRelativeNormalizedX, pRelativeNormalizedY, pScale, pAbsoluteXyDistance, pPoint);
	mPushSpline->GotoAbsoluteTime(pRelativeNormalizedY);
	const float lPush = mPushSpline->GetValue();
	pPoint.x += lPush*pScale;
}



TerrainRotateFunction::TerrainRotateFunction(float pAngle, TerrainFunction* pFunction):
	TerrainFunction(1.0, pFunction->GetPosition(), pFunction->GetInnerRadius(), pFunction->GetOuterRadius()),
	TerrainDecorator(pFunction),
	mAngle(pAngle)
{
}

void TerrainRotateFunction::AddPoint(float pRelativeNormalizedX, float pRelativeNormalizedY, float pScale,
	float pAbsoluteXyDistance, Vector3DF& pPoint) const
{
	// TODO: optimize by using matrices for rotation.
	const float lNewRelativeNormalizedX = ::cos(-mAngle)*pRelativeNormalizedX - ::sin(-mAngle)*pRelativeNormalizedY;
	const float lNewRelativeNormalizedY = ::sin(-mAngle)*pRelativeNormalizedX + ::cos(-mAngle)*pRelativeNormalizedY;
	Vector3DF lRotatePoint(lNewRelativeNormalizedX, lNewRelativeNormalizedY, pPoint.z/mOuterRadius);
	lRotatePoint *= mOuterRadius;
	lRotatePoint.x += mPosition.x;
	lRotatePoint.y += mPosition.y;
	mFunction->AddPoint(lNewRelativeNormalizedX, lNewRelativeNormalizedY, pScale, pAbsoluteXyDistance, lRotatePoint);
	lRotatePoint.x -= mPosition.x;
	lRotatePoint.y -= mPosition.y;
	pPoint.x = ::cos(mAngle)*lRotatePoint.x - ::sin(mAngle)*lRotatePoint.y + mPosition.x;
	pPoint.y = ::sin(mAngle)*lRotatePoint.x + ::cos(mAngle)*lRotatePoint.y + mPosition.y;
	pPoint.z = lRotatePoint.z;
}



}
