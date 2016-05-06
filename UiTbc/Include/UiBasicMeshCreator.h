/*
	Class:  BasicMeshCreator
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#pragma once

namespace uitbc {

class AnimatedGeometry;
class TriangleBasedGeometry;

class BasicMeshCreator {
public:
	static TriangleBasedGeometry* CreateFlatBox(float x_size, float y_size, float z_size,
						unsigned x_segments = 1,
						unsigned y_segments = 1,
						unsigned z_segments = 1);

	static TriangleBasedGeometry* CreateEllipsoid(float x_radius,
						      float y_radius,
						      float z_radius,
						      unsigned num_latitude_segments,
						      unsigned num_longitude_segments);
	static TriangleBasedGeometry* CreateCone(float base_radius,
						 float height,
						 unsigned num_segments);
	static TriangleBasedGeometry* CreateCylinder(float base_radius,
						     float top_radius,
						     float height,
						     unsigned num_segments);
	static TriangleBasedGeometry* CreateTorus(float radius,
						  float ring_radius_x,
						  float ring_radius_y,
						  unsigned num_segments,
						  unsigned num_ring_segments);

	static void CreateYBonedSkin(float start_y, float end_y, const TriangleBasedGeometry* geometry,
		AnimatedGeometry* skin, int bone_count, float bone_stiffness = 1.1);

	// TODO: Implement more. Capsule, tube
};

}
