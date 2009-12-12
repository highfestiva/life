/*
	Class:  UVMapper
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games
*/

#ifndef UIUVMAPPER_H
#define UIUVMAPPER_H



#include "../../Lepra/Include/Vector2D.h"
#include "../../Lepra/Include/Vector3D.h"



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
					const Lepra::Vector2DD& pUVOffset, 
					const Lepra::Vector3DD& pPlaneX, 
					const Lepra::Vector3DD& pPlaneY);

	// Some functions of convenience.
	inline static bool ApplyPlanarMappingX(TBC::GeometryBase* pGeometry, unsigned int pUVSet, double pScale, const Lepra::Vector2DD& pUVOffset);
	inline static bool ApplyPlanarMappingY(TBC::GeometryBase* pGeometry, unsigned int pUVSet, double pScale, const Lepra::Vector2DD& pUVOffset);
	inline static bool ApplyPlanarMappingZ(TBC::GeometryBase* pGeometry, unsigned int pUVSet, double pScale, const Lepra::Vector2DD& pUVOffset);

	// Will apply UV-mapping as seen from six different directions (the sides of a cube). The surface 
	// normal will be used to determine which side of the cube to use for mapping. If two triangles
	// belonging to two different sides of the cube share the same vertices, the result will become
	// erroneous.
	inline static bool ApplyCubeMapping(TBC::GeometryBase* pGeometry, unsigned int pUVSet, 
					    float pScale,
					    const Lepra::Vector2DD& pUVOffset);
	inline static bool ApplyCubeMapping(TBC::GeometryBase* pGeometry, unsigned int pUVSet, 
					    float pScale,
					    const Lepra::Vector2DD& pUVOffsetLeftRight,
					    const Lepra::Vector2DD& pUVOffsetTopBottom,
					    const Lepra::Vector2DD& pUVOffsetFrontBack);
	static bool ApplyCubeMapping(TBC::GeometryBase* pGeometry, unsigned int pUVSet, 
				     float pScale,
				     const Lepra::Vector2DD& pUVOffsetLeft,
				     const Lepra::Vector2DD& pUVOffsetRight,
				     const Lepra::Vector2DD& pUVOffsetTop,
				     const Lepra::Vector2DD& pUVOffsetBottom,
				     const Lepra::Vector2DD& pUVOffsetFront,
				     const Lepra::Vector2DD& pUVOffsetBack);

private:
};

bool UVMapper::ApplyPlanarMappingX(TBC::GeometryBase* pGeometry, unsigned int pUVSet, double pScale, const Lepra::Vector2DD& pUVOffset)
{
	return ApplyPlanarMapping(pGeometry, pUVSet, pUVOffset, Lepra::Vector3DD(0, 0, -1.0 / pScale), Lepra::Vector3DD(0, 1.0 / pScale, 0));
}

bool UVMapper::ApplyPlanarMappingY(TBC::GeometryBase* pGeometry, unsigned int pUVSet, double pScale, const Lepra::Vector2DD& pUVOffset)
{
	return ApplyPlanarMapping(pGeometry, pUVSet, pUVOffset, Lepra::Vector3DD(1.0 / pScale, 0, 0), Lepra::Vector3DD(0, 0, 1.0 / pScale));
}

bool UVMapper::ApplyPlanarMappingZ(TBC::GeometryBase* pGeometry, unsigned int pUVSet, double pScale, const Lepra::Vector2DD& pUVOffset)
{
	return ApplyPlanarMapping(pGeometry, pUVSet, pUVOffset, Lepra::Vector3DD(1.0 / pScale, 0, 0), Lepra::Vector3DD(0, 1.0 / pScale, 0));
}

bool UVMapper::ApplyCubeMapping(TBC::GeometryBase* pGeometry, unsigned int pUVSet, 
				float pScale,
				const Lepra::Vector2DD& pUVOffset)
{
	return ApplyCubeMapping(pGeometry, pUVSet, pScale, pUVOffset, pUVOffset, pUVOffset, pUVOffset, pUVOffset, pUVOffset);
}

bool UVMapper::ApplyCubeMapping(TBC::GeometryBase* pGeometry, unsigned int pUVSet, 
				    float pScale,
				    const Lepra::Vector2DD& pUVOffsetLeftRight,
				    const Lepra::Vector2DD& pUVOffsetTopBottom,
				    const Lepra::Vector2DD& pUVOffsetFrontBack)
{
	return ApplyCubeMapping(pGeometry, pUVSet, pScale, pUVOffsetLeftRight, pUVOffsetLeftRight, pUVOffsetTopBottom, pUVOffsetTopBottom, pUVOffsetFrontBack, pUVOffsetFrontBack);
}



} // End namespace.



#endif
