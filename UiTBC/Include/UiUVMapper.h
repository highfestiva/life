
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../Lepra/Include/Vector2D.h"
#include "../../Lepra/Include/Vector3D.h"
#include "../Include/UiTBC.h"



namespace TBC
{
class GeometryBase;
}



namespace UiTbc
{

class UVMapper
{
public:
	// pPlaneX and pPlaneY defines the plane from which the mapping coordinates will be calculated.
	// These vectors doesn't have to be orthogonal, but mustn't be parallel. To scale the coordinates,
	// just scale these vectors. The memory for the UV-set must be allocated before calling this function.
	static bool ApplyPlanarMapping(TBC::GeometryBase* pGeometry, unsigned int pUVSet, 
					const Vector2DD& pUVOffset, 
					const Vector3DD& pPlaneX, 
					const Vector3DD& pPlaneY);

	// Some functions of convenience.
	inline static bool ApplyPlanarMappingX(TBC::GeometryBase* pGeometry, unsigned int pUVSet, double pScale, const Vector2DD& pUVOffset);
	inline static bool ApplyPlanarMappingY(TBC::GeometryBase* pGeometry, unsigned int pUVSet, double pScale, const Vector2DD& pUVOffset);
	inline static bool ApplyPlanarMappingZ(TBC::GeometryBase* pGeometry, unsigned int pUVSet, double pScale, const Vector2DD& pUVOffset);

	// Will apply UV-mapping as seen from six different directions (the sides of a cube). The surface 
	// normal will be used to determine which side of the cube to use for mapping. If two triangles
	// belonging to two different sides of the cube share the same vertices, the result will become
	// erroneous.
	inline static bool ApplyCubeMapping(TBC::GeometryBase* pGeometry, unsigned int pUVSet, 
					    float pScale,
					    const Vector2DD& pUVOffset);
	inline static bool ApplyCubeMapping(TBC::GeometryBase* pGeometry, unsigned int pUVSet, 
					    float pScale,
					    const Vector2DD& pUVOffsetLeftRight,
					    const Vector2DD& pUVOffsetTopBottom,
					    const Vector2DD& pUVOffsetFrontBack);
	static bool ApplyCubeMapping(TBC::GeometryBase* pGeometry, unsigned int pUVSet, 
				     float pScale,
				     const Vector2DD& pUVOffsetLeft,
				     const Vector2DD& pUVOffsetRight,
				     const Vector2DD& pUVOffsetTop,
				     const Vector2DD& pUVOffsetBottom,
				     const Vector2DD& pUVOffsetFront,
				     const Vector2DD& pUVOffsetBack);

private:
};

bool UVMapper::ApplyPlanarMappingX(TBC::GeometryBase* pGeometry, unsigned int pUVSet, double pScale, const Vector2DD& pUVOffset)
{
	return ApplyPlanarMapping(pGeometry, pUVSet, pUVOffset, Vector3DD(0, 0, -1.0 / pScale), Vector3DD(0, 1.0 / pScale, 0));
}

bool UVMapper::ApplyPlanarMappingY(TBC::GeometryBase* pGeometry, unsigned int pUVSet, double pScale, const Vector2DD& pUVOffset)
{
	return ApplyPlanarMapping(pGeometry, pUVSet, pUVOffset, Vector3DD(1.0 / pScale, 0, 0), Vector3DD(0, 0, 1.0 / pScale));
}

bool UVMapper::ApplyPlanarMappingZ(TBC::GeometryBase* pGeometry, unsigned int pUVSet, double pScale, const Vector2DD& pUVOffset)
{
	return ApplyPlanarMapping(pGeometry, pUVSet, pUVOffset, Vector3DD(1.0 / pScale, 0, 0), Vector3DD(0, 1.0 / pScale, 0));
}

bool UVMapper::ApplyCubeMapping(TBC::GeometryBase* pGeometry, unsigned int pUVSet, 
				float pScale,
				const Vector2DD& pUVOffset)
{
	return ApplyCubeMapping(pGeometry, pUVSet, pScale, pUVOffset, pUVOffset, pUVOffset, pUVOffset, pUVOffset, pUVOffset);
}

bool UVMapper::ApplyCubeMapping(TBC::GeometryBase* pGeometry, unsigned int pUVSet, 
				    float pScale,
				    const Vector2DD& pUVOffsetLeftRight,
				    const Vector2DD& pUVOffsetTopBottom,
				    const Vector2DD& pUVOffsetFrontBack)
{
	return ApplyCubeMapping(pGeometry, pUVSet, pScale, pUVOffsetLeftRight, pUVOffsetLeftRight, pUVOffsetTopBottom, pUVOffsetTopBottom, pUVOffsetFrontBack, pUVOffsetFrontBack);
}



}
