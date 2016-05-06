
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../lepra/include/vector2d.h"
#include "../../lepra/include/vector3d.h"
#include "../include/uitbc.h"



namespace tbc {
class GeometryBase;
}



namespace uitbc {

class UVMapper {
public:
	// plane_x and plane_y defines the plane from which the mapping coordinates will be calculated.
	// These vectors doesn't have to be orthogonal, but mustn't be parallel. To scale the coordinates,
	// just scale these vectors. The memory for the UV-set must be allocated before calling this function.
	static bool ApplyPlanarMapping(tbc::GeometryBase* geometry, unsigned int uv_set,
					const Vector2DD& uv_offset,
					const Vector3DD& plane_x,
					const Vector3DD& plane_y);

	// Some functions of convenience.
	inline static bool ApplyPlanarMappingX(tbc::GeometryBase* geometry, unsigned int uv_set, double scale, const Vector2DD& uv_offset);
	inline static bool ApplyPlanarMappingY(tbc::GeometryBase* geometry, unsigned int uv_set, double scale, const Vector2DD& uv_offset);
	inline static bool ApplyPlanarMappingZ(tbc::GeometryBase* geometry, unsigned int uv_set, double scale, const Vector2DD& uv_offset);

	// Will apply UV-mapping as seen from six different directions (the sides of a cube). The surface
	// normal will be used to determine which side of the cube to use for mapping. If two triangles
	// belonging to two different sides of the cube share the same vertices, the result will become
	// erroneous.
	inline static bool ApplyCubeMapping(tbc::GeometryBase* geometry, unsigned int uv_set,
					    float scale,
					    const Vector2DD& uv_offset);
	inline static bool ApplyCubeMapping(tbc::GeometryBase* geometry, unsigned int uv_set,
					    float scale,
					    const Vector2DD& uv_offset_left_right,
					    const Vector2DD& uv_offset_top_bottom,
					    const Vector2DD& uv_offset_front_back);
	static bool ApplyCubeMapping(tbc::GeometryBase* geometry, unsigned int uv_set,
				     float scale,
				     const Vector2DD& uv_offset_left,
				     const Vector2DD& uv_offset_right,
				     const Vector2DD& uv_offset_top,
				     const Vector2DD& uv_offset_bottom,
				     const Vector2DD& uv_offset_front,
				     const Vector2DD& uv_offset_back);

private:
};

bool UVMapper::ApplyPlanarMappingX(tbc::GeometryBase* geometry, unsigned int uv_set, double scale, const Vector2DD& uv_offset) {
	return ApplyPlanarMapping(geometry, uv_set, uv_offset, Vector3DD(0, 0, -1.0 / scale), Vector3DD(0, 1.0 / scale, 0));
}

bool UVMapper::ApplyPlanarMappingY(tbc::GeometryBase* geometry, unsigned int uv_set, double scale, const Vector2DD& uv_offset) {
	return ApplyPlanarMapping(geometry, uv_set, uv_offset, Vector3DD(1.0 / scale, 0, 0), Vector3DD(0, 0, 1.0 / scale));
}

bool UVMapper::ApplyPlanarMappingZ(tbc::GeometryBase* geometry, unsigned int uv_set, double scale, const Vector2DD& uv_offset) {
	return ApplyPlanarMapping(geometry, uv_set, uv_offset, Vector3DD(1.0 / scale, 0, 0), Vector3DD(0, 1.0 / scale, 0));
}

bool UVMapper::ApplyCubeMapping(tbc::GeometryBase* geometry, unsigned int uv_set,
				float scale,
				const Vector2DD& uv_offset) {
	return ApplyCubeMapping(geometry, uv_set, scale, uv_offset, uv_offset, uv_offset, uv_offset, uv_offset, uv_offset);
}

bool UVMapper::ApplyCubeMapping(tbc::GeometryBase* geometry, unsigned int uv_set,
				    float scale,
				    const Vector2DD& uv_offset_left_right,
				    const Vector2DD& uv_offset_top_bottom,
				    const Vector2DD& uv_offset_front_back) {
	return ApplyCubeMapping(geometry, uv_set, scale, uv_offset_left_right, uv_offset_left_right, uv_offset_top_bottom, uv_offset_top_bottom, uv_offset_front_back, uv_offset_front_back);
}



}
