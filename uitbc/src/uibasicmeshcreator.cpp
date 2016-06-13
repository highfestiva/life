
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/uibasicmeshcreator.h"
#include "../../lepra/include/lepraassert.h"
#include <algorithm>
#include <list>
#include "../../lepra/include/math.h"
#include "../../lepra/include/vector3d.h"
#include "../include/uianimatedgeometry.h"
#include "../include/uitbc.h"
#include "../include/uitrianglebasedgeometry.h"



namespace uitbc {



TriangleBasedGeometry* BasicMeshCreator::CreateFlatBox(float x_size, float y_size, float z_size,
						   unsigned x_segments,
						   unsigned y_segments,
						   unsigned z_segments) {
	int vertex_count = (x_segments + 1) * (y_segments + 1) * 2 +  // Top & bottom plane.
			    (x_segments + 1) * (z_segments + 1) * 2 +  // Front and back plane.
			    (y_segments + 1) * (z_segments + 1) * 2;   // Left and right plane.

	int num_triangles = x_segments * y_segments * 4 +
			     x_segments * z_segments * 4 +
			     y_segments * z_segments * 4;

	vec3* __v = new vec3[vertex_count];
	vec3* __n = new vec3[vertex_count];
	uint32* indices = new uint32[num_triangles * 3];

	::memset(indices, 0, num_triangles * 3 * sizeof(uint32));

	float x_step = x_size / (float)x_segments;
	float y_step = y_size / (float)y_segments;
	float z_step = z_size / (float)z_segments;

	float half_x_size = x_size * 0.5f;
	float half_y_size = y_size * 0.5f;
	float half_z_size = z_size * 0.5f;

	float __x, __y, __z;
	unsigned x, y, z;

	int v_index = 0;
	int t_index = 0;

	// TOP AND BOTTOM PLANES.
	int v_index_offset = (x_segments + 1) * (y_segments  + 1);
	__y = -half_y_size;
	for (y = 0; y < y_segments + 1; y++) {
		__x = -half_x_size;
		for (x = 0; x < x_segments + 1; x++) {
			// Top plane vertex and normal.
			__v[v_index].Set(__x, __y, half_z_size);
			__n[v_index].Set(0, 0, 1);

			// Bottom plane vertex and normal.
			__v[v_index + v_index_offset].Set(__x, __y, -half_z_size);
			__n[v_index + v_index_offset].Set(0, 0, -1);

			v_index++;
			__x += x_step;
		}
		__y += y_step;
	}
	int bottom_plane_index_offset = v_index;
	v_index += v_index_offset;

	// FRONT AND BACK PLANES.
	int front_plane_index_offset = v_index;
	v_index_offset = (x_segments + 1) * (z_segments + 1);
	__z = -half_z_size;
	for (z = 0; z < z_segments + 1; z++) {
		__x = -half_x_size;
		for (x = 0; x < x_segments + 1; x++) {
			// Front plane vertex and normal.
			__v[v_index].Set(__x, -half_y_size, __z);
			__n[v_index].Set(0, -1, 0);

			// Back plane vertex and normal.
			__v[v_index + v_index_offset].Set(__x, half_y_size, __z);
			__n[v_index + v_index_offset].Set(0, 1, 0);

			v_index++;
			__x += x_step;
		}
		__z += z_step;
	}
	int back_plane_index_offset = v_index;
	v_index += v_index_offset;

	// LEFT AND RIGHT PLANES.
	int left_plane_index_offset = v_index;
	v_index_offset = (y_segments + 1) * (z_segments + 1);
	__z = -half_z_size;
	for (z = 0; z < z_segments + 1; z++) {
		__y = -half_y_size;
		for (y = 0; y < y_segments + 1; y++) {
			// Left plane vertex and normal.
			__v[v_index].Set(-half_x_size, __y, __z);
			__n[v_index].Set(-1, 0, 0);

			// Right plane vertex and normal.
			__v[v_index + v_index_offset].Set(half_x_size, __y, __z);
			__n[v_index + v_index_offset].Set(1, 0, 0);

			v_index++;
			__y += y_step;
		}
		__z += z_step;
	}
	int right_plane_index_offset = v_index;
	v_index += v_index_offset;

	// Setup top plane triangles.
	for (y = 0; y < y_segments; y++) {
		for (x = 0; x < x_segments; x++) {
			// First triangle.
			indices[t_index++] = (y + 0) * (x_segments + 1) + x + 0;
			indices[t_index++] = (y + 0) * (x_segments + 1) + x + 1;
			indices[t_index++] = (y + 1) * (x_segments + 1) + x + 1;

			// Second triangle.
			indices[t_index++] = (y + 1) * (x_segments + 1) + x + 0;
			indices[t_index++] = (y + 0) * (x_segments + 1) + x + 0;
			indices[t_index++] = (y + 1) * (x_segments + 1) + x + 1;

			deb_assert(t_index <= num_triangles * 3);
		}
	}

	// Setup bottom plane triangles.
	for (z = 0; z < y_segments; z++) {
		for (x = 0; x < x_segments; x++) {
			// First triangle.
			indices[t_index++] = (z + 0) * (x_segments + 1) + x + 1 + bottom_plane_index_offset;
			indices[t_index++] = (z + 0) * (x_segments + 1) + x + 0 + bottom_plane_index_offset;
			indices[t_index++] = (z + 1) * (x_segments + 1) + x + 1 + bottom_plane_index_offset;

			// Second triangle.
			indices[t_index++] = (z + 0) * (x_segments + 1) + x + 0 + bottom_plane_index_offset;
			indices[t_index++] = (z + 1) * (x_segments + 1) + x + 0 + bottom_plane_index_offset;
			indices[t_index++] = (z + 1) * (x_segments + 1) + x + 1 + bottom_plane_index_offset;

			deb_assert(t_index <= num_triangles * 3);
		}
	}

	// Setup front plane triangles.
	for (z = 0; z < z_segments; z++) {
		for (x = 0; x < x_segments; x++) {
			// First triangle.
			indices[t_index++] = (z + 0) * (x_segments + 1) + x + 0 + front_plane_index_offset;
			indices[t_index++] = (z + 0) * (x_segments + 1) + x + 1 + front_plane_index_offset;
			indices[t_index++] = (z + 1) * (x_segments + 1) + x + 1 + front_plane_index_offset;

			// Second triangle.
			indices[t_index++] = (z + 1) * (x_segments + 1) + x + 0 + front_plane_index_offset;
			indices[t_index++] = (z + 0) * (x_segments + 1) + x + 0 + front_plane_index_offset;
			indices[t_index++] = (z + 1) * (x_segments + 1) + x + 1 + front_plane_index_offset;

			deb_assert(t_index <= num_triangles * 3);
		}
	}

	// Setup back plane triangles.
	for (z = 0; z < z_segments; z++) {
		for (x = 0; x < x_segments; x++) {
			// First triangle.
			indices[t_index++] = (z + 0) * (x_segments + 1) + x + 1 + back_plane_index_offset;
			indices[t_index++] = (z + 0) * (x_segments + 1) + x + 0 + back_plane_index_offset;
			indices[t_index++] = (z + 1) * (x_segments + 1) + x + 1 + back_plane_index_offset;

			// Second triangle.
			indices[t_index++] = (z + 0) * (x_segments + 1) + x + 0 + back_plane_index_offset;
			indices[t_index++] = (z + 1) * (x_segments + 1) + x + 0 + back_plane_index_offset;
			indices[t_index++] = (z + 1) * (x_segments + 1) + x + 1 + back_plane_index_offset;

			deb_assert(t_index <= num_triangles * 3);
		}
	}

	// Setup left plane triangles.
	for (z = 0; z < z_segments; z++) {
		for (y = 0; y < y_segments; y++) {
			// First triangle.
			indices[t_index++] = (z + 0) * (y_segments + 1) + y + 1 + left_plane_index_offset;
			indices[t_index++] = (z + 0) * (y_segments + 1) + y + 0 + left_plane_index_offset;
			indices[t_index++] = (z + 1) * (y_segments + 1) + y + 1 + left_plane_index_offset;

			// Second triangle.
			indices[t_index++] = (z + 0) * (y_segments + 1) + y + 0 + left_plane_index_offset;
			indices[t_index++] = (z + 1) * (y_segments + 1) + y + 0 + left_plane_index_offset;
			indices[t_index++] = (z + 1) * (y_segments + 1) + y + 1 + left_plane_index_offset;

			deb_assert(t_index <= num_triangles * 3);
		}
	}

	// Setup right plane triangles.
	for (z = 0; z < z_segments; z++) {
		for (y = 0; y < y_segments; y++) {
			// First triangle.
			indices[t_index++] = (z + 0) * (y_segments + 1) + y + 0 + right_plane_index_offset;
			indices[t_index++] = (z + 0) * (y_segments + 1) + y + 1 + right_plane_index_offset;
			indices[t_index++] = (z + 1) * (y_segments + 1) + y + 1 + right_plane_index_offset;

			// Second triangle.
			indices[t_index++] = (z + 1) * (y_segments + 1) + y + 0 + right_plane_index_offset;
			indices[t_index++] = (z + 0) * (y_segments + 1) + y + 0 + right_plane_index_offset;
			indices[t_index++] = (z + 1) * (y_segments + 1) + y + 1 + right_plane_index_offset;

			deb_assert(t_index <= num_triangles * 3);
		}
	}

	TriangleBasedGeometry* _geometry;
	_geometry = new TriangleBasedGeometry(__v, __n, 0, 0,
					    TriangleBasedGeometry::kColorRgb,
					    indices, vertex_count, num_triangles * 3,
					    tbc::GeometryBase::kTriangles,
					    tbc::GeometryBase::kGeomStatic);

	delete[] __v;
	delete[] __n;
	delete[] indices;

	return _geometry;
}

TriangleBasedGeometry* BasicMeshCreator::CreateEllipsoid(float x_radius,
							 float y_radius,
							 float z_radius,
							 unsigned num_latitude_segments,
							 unsigned num_longitude_segments) {
	int vertex_count = num_longitude_segments * (num_latitude_segments - 1) + 2;
	int num_triangles = num_longitude_segments * (num_latitude_segments - 2) * 2 + num_longitude_segments * 2;

	Vector3D<float>* __v = new Vector3D<float>[vertex_count];
	Vector3D<float>* __n = new Vector3D<float>[vertex_count];
	uint32* __i = new uint32[num_triangles * 3];

	const float lat_angle_step = PIF / (float)num_latitude_segments;
	const float long_angle_step = PIF * 2.0f / (float)num_longitude_segments;

	// Setup top and bottom vertex.
	__v[0].Set(0, y_radius, 0);
	__v[1].Set(0, -y_radius, 0);
	__n[0].Set(0, 1.0f, 0);
	__n[1].Set(0, -1.0f, 0);

	// Setup the rest of the vertices.
	int index = 2;
	float lat_angle = lat_angle_step;
	unsigned lat_segment_count;
	for (lat_segment_count = 1; lat_segment_count < num_latitude_segments; lat_segment_count++) {
		float circle_radius = (float)sin(lat_angle);
		float __y = (float)cos(lat_angle) * y_radius;
		float norm_y = (float)cos(lat_angle) / y_radius;

		float long_angle = 0.0f;
		unsigned long_segment_count;
		for (long_segment_count = 0; long_segment_count < num_longitude_segments; long_segment_count++) {
			float cos_a = (float)cos(long_angle) * circle_radius;
			float sin_a = (float)sin(long_angle) * circle_radius;

			__v[index].x = cos_a * x_radius;
			__v[index].y = __y;
			__v[index].z = sin_a * z_radius;

			__n[index].x = cos_a / x_radius;
			__n[index].y = norm_y;
			__n[index].z = sin_a / z_radius;
			__n[index].Normalize();

			long_angle += long_angle_step;
			index++;
		}

		lat_angle += lat_angle_step;
	}

	// Setup "middle" triangles.
	index = 0;
	for (lat_segment_count = 1; lat_segment_count < num_latitude_segments - 1; lat_segment_count++) {
		unsigned lat_offset0 = (lat_segment_count - 1) * num_longitude_segments;
		unsigned lat_offset1 = lat_offset0 + num_longitude_segments;

		unsigned long_segment_count;
		for (long_segment_count = 0; long_segment_count < num_longitude_segments; long_segment_count++) {
			__i[index + 0] = 2 + lat_offset0 + long_segment_count;
			__i[index + 1] = 2 + lat_offset0 + (long_segment_count + 1) % num_longitude_segments;
			__i[index + 2] = 2 + lat_offset1 + (long_segment_count + 1) % num_longitude_segments;
			__i[index + 3] = 2 + lat_offset0 + long_segment_count;
			__i[index + 4] = 2 + lat_offset1 + (long_segment_count + 1) % num_longitude_segments;
			__i[index + 5] = 2 + lat_offset1 + long_segment_count;

			index += 6;
		}
	}

	// Setup "top" triangles.
	unsigned long_segment_count;
	for (long_segment_count = 0; long_segment_count < num_longitude_segments; long_segment_count++) {
		__i[index + 0] = 0;
		__i[index + 1] = 2 + (long_segment_count + 1) % num_longitude_segments;
		__i[index + 2] = 2 + long_segment_count;

		index += 3;
	}

	// Setup "bottom" triangles.
	unsigned offset = 2 + (num_latitude_segments - 2) * num_longitude_segments;
	for (long_segment_count = 0; long_segment_count < num_longitude_segments; long_segment_count++) {
		__i[index + 0] = 1;
		__i[index + 1] = offset + long_segment_count;
		__i[index + 2] = offset + (long_segment_count + 1) % num_longitude_segments;

		index += 3;
	}

	TriangleBasedGeometry* ellipsoid =
		new TriangleBasedGeometry(__v, __n, 0, 0,
					  TriangleBasedGeometry::kColorRgb,
					  __i, vertex_count, num_triangles * 3,
					  tbc::GeometryBase::kTriangles,
					  tbc::GeometryBase::kGeomStatic);
	delete[] __v;
	delete[] __n;
	delete[] __i;

	return ellipsoid;
}

TriangleBasedGeometry* BasicMeshCreator::CreateCone(float base_radius,
						    float height,
						    unsigned num_segments) {
	int vertex_count = num_segments + 1 + 1;
	int num_triangles = num_segments * 2;

	Vector3D<float>* __v = new Vector3D<float>[vertex_count];
	Vector3D<float>* __n = new Vector3D<float>[vertex_count];
	uint32* __i = new uint32[num_triangles * 3];

	// Set the top vertex.
	__v[0].Set(0, 0, height);
	__n[0].Set(0, 0, 1.0f);

	const float angle_step = (2.0f * PIF) / (float)num_segments;
	float angle = 0.0f;

	int index = 0;
	unsigned i;
	for (i = 0; i < num_segments; i++, index += 3) {
		float cos_a = (float)cos(angle);
		float sin_a = -(float)sin(angle);

		// Setup vertex
		__v[i + 1].x = cos_a * base_radius;
		__v[i + 1].y = sin_a * base_radius;
		__v[i + 1].z = 0.0f;

		// Setup normal
		__n[i + 1].x = cos_a;
		if (base_radius > height) {
			__n[i + 1].z = ((float)cos(atan(height / base_radius)) - 1.0f) * 0.5f;
		} else {
			__n[i + 1].z = ((float)sin(atan(base_radius / height)) - 1.0f) * 0.5f;
		}

		__n[i + 1].y = sin_a;
		__n[i + 1].Normalize();

		// Setup triangle index.
		__i[index + 0] = i + 1;
		__i[index + 1] = 0;
		__i[index + 2] = ((i + 1) % num_segments) + 1;

		angle += angle_step;
	}
	// Bottom vertex.
	const int bottom_index = i+1;
	__v[bottom_index].Set(0, 0, 0);
	__n[bottom_index].Set(0, 0, -1.0f);

	// Setup the bottom triangles...
	for (i = 0; i < num_segments; i++, index += 3) {
		__i[index + 0] = (i+1)%num_segments + 1;
		__i[index + 1] = bottom_index;
		__i[index + 2] = i+1;
	}

	TriangleBasedGeometry* cone =
		new TriangleBasedGeometry(__v, __n, 0, 0,
					  TriangleBasedGeometry::kColorRgb,
					  __i, vertex_count, num_triangles * 3,
					  tbc::GeometryBase::kTriangles,
					  tbc::GeometryBase::kGeomStatic);

	delete[] __v;
	delete[] __n;
	delete[] __i;

	return cone;
}

TriangleBasedGeometry* BasicMeshCreator::CreateCylinder(float base_radius,
							float top_radius,
							float height,
							unsigned num_segments) {
	int vertex_count = num_segments * 2 + 2;
	int num_triangles = num_segments * 2 + num_segments * 2;

	Vector3D<float>* __v = new Vector3D<float>[vertex_count];
	Vector3D<float>* __n = new Vector3D<float>[vertex_count];
	uint32* __i = new uint32[num_triangles * 3];

	const float angle_step = (2.0f * PIF) / (float)num_segments;
	float angle = 0.0f;

	float radius_diff = base_radius - top_radius;
	float norm_y_add;

	if (fabs(radius_diff) > height) {
		if (radius_diff > 0.0f) {
			norm_y_add = (float)cos(atan(height / radius_diff));
		} else {
			norm_y_add = -(float)cos(atan(height / radius_diff));
		}
	} else {
		norm_y_add = (float)sin(atan(radius_diff / height));
	}

	// Setup cylindrical vertices.
	unsigned i;
	for (i = 0; i < num_segments; i++) {
		float cos_a = (float)cos(angle);
		float sin_a = (float)sin(angle);

		__v[i].x = cos_a * top_radius;
		__v[i].y = height * 0.5f;
		__v[i].z = sin_a * top_radius;

		__n[i].x = cos_a;
		__n[i].y = 1.0f + norm_y_add;
		__n[i].z = sin_a;
		__n[i].Normalize();

		__v[num_segments + i].x = cos_a * base_radius;
		__v[num_segments + i].y = -height * 0.5f;
		__v[num_segments + i].z = sin_a * base_radius;

		__n[num_segments + i].x = cos_a;
		__n[num_segments + i].y = -(1.0f - norm_y_add);
		__n[num_segments + i].z = sin_a;
		__n[num_segments + i].Normalize();

		angle += angle_step;
	}
	// Set the top and bottom vertices.
	const int top_vertex = num_segments*2;
	const int bottom_vertex = top_vertex+1;
	__v[top_vertex].Set(0, height*0.5f, 0);
	__n[top_vertex].Set(0, 1.0f, 0);
	__v[bottom_vertex].Set(0, -height*0.5f, 0);
	__n[bottom_vertex].Set(0, -1.0f, 0);

	// Setup triangles.
	int index = 0;
	for (i = 0; i < num_segments; i++) {
		__i[index + 0] = i;
		__i[index + 1] = (i + 1) % num_segments;
		__i[index + 2] = (i + 1) % num_segments + num_segments;
		__i[index + 3] = i;
		__i[index + 4] = (i + 1) % num_segments + num_segments;
		__i[index + 5] = i + num_segments;

		index += 6;
	}

	// Setup top triangles.
	for (i = 0; i < num_segments; i++) {
		__i[index + 0] = top_vertex;
		__i[index + 1] = (i+1) % num_segments;
		__i[index + 2] = i;
		index += 3;
	}

	// Setup bottom triangles.
	for (i = 0; i < num_segments; i++) {
		__i[index + 0] = bottom_vertex;
		__i[index + 1] = num_segments + i;
		__i[index + 2] = num_segments + (i+1) % num_segments;
		index += 3;
	}

	TriangleBasedGeometry* cylinder =
		new TriangleBasedGeometry(__v, __n, 0, 0,
					  TriangleBasedGeometry::kColorRgb,
					  __i, vertex_count, num_triangles * 3,
					  tbc::GeometryBase::kTriangles,
					  tbc::GeometryBase::kGeomStatic);

	delete[] __v;
	delete[] __n;
	delete[] __i;

	return cylinder;
}

TriangleBasedGeometry* BasicMeshCreator::CreateTorus(float radius,
						     float ring_radius_x,
						     float ring_radius_y,
						     unsigned num_segments,
						     unsigned num_ring_segments) {
	int vertex_count = num_segments * num_ring_segments;
	int num_triangles = num_segments * num_ring_segments * 2;

	Vector3D<float>* __v = new Vector3D<float>[vertex_count];
	Vector3D<float>* __n = new Vector3D<float>[vertex_count];
	uint32* __i = new uint32[num_triangles * 3];

	const float angle_step = 2.0f * PIF / (float)num_segments;
	const float ring_angle_step = 2.0f * PIF / (float)num_ring_segments;

	float angle = 0.0f;

	int index = 0;
	unsigned i;
	for (i = 0; i < num_segments; i++) {
		float cos_a = (float)cos(angle);
		float sin_a = (float)sin(angle);

		float ring_angle = 0.0f;
		for (unsigned j = 0; j < num_ring_segments; j++) {
			float cos_ring_a = (float)cos(ring_angle);
			float sin_ring_a = (float)sin(ring_angle);

			float __x = cos_ring_a * ring_radius_x;

			__v[index].x = cos_a * (__x + radius);
			__v[index].y = sin_ring_a * ring_radius_y;
			__v[index].z = sin_a * (__x + radius);

			__n[index].x = cos_ring_a * cos_a;
			__n[index].y = sin_ring_a;
			__n[index].z = cos_ring_a * sin_a;
			//__n[index].Normalize();

			ring_angle += ring_angle_step;
			index++;
		}

		angle += angle_step;
	}

	index = 0;
	for (i = 0; i < num_segments; i++) {
		unsigned offset0 = i * num_ring_segments;
		unsigned offset1 = ((i + 1) % num_segments) * num_ring_segments;

		for (unsigned j = 0; j < num_ring_segments; j++) {
			__i[index + 0] = offset0 + j;
			__i[index + 1] = offset0 + (j + 1) % num_ring_segments;
			__i[index + 2] = offset1 + (j + 1) % num_ring_segments;
			__i[index + 3] = offset0 + j;
			__i[index + 4] = offset1 + (j + 1) % num_ring_segments;
			__i[index + 5] = offset1 + j;

			index += 6;
		}
	}

	TriangleBasedGeometry* torus =
		new TriangleBasedGeometry(__v, __n, 0, 0,
					  TriangleBasedGeometry::kColorRgb,
					  __i, vertex_count, num_triangles * 3,
					  tbc::GeometryBase::kTriangles,
					  tbc::GeometryBase::kGeomStatic);

	delete[] __v;
	delete[] __n;
	delete[] __i;

	return torus;
}

void BasicMeshCreator::CreateYBonedSkin(float start_y, float end_y, const TriangleBasedGeometry* geometry,
	AnimatedGeometry* skin, int bone_count, float bone_stiffness) {
	// Collect the Y minimas and maximas.
	std::list<int> minima_list;
	std::list<int> maxima_list;
	unsigned v;
	for (v = 0; v < geometry->GetVertexCount(); ++v) {
		float y = geometry->GetVertexData()[v*3+1];
		if (y <= start_y+eps_f) {
			minima_list.push_back(v);
		} else if (y >= end_y-eps_f) {
			maxima_list.push_back(v);
		}
	}

	uitbc::AnimatedGeometry::BoneWeightGroup weight_group;

	if (!minima_list.empty()) {
		// Create weights for proximal end cap of skinned mesh.
		weight_group.bone_count_ = 1;
		weight_group.bone_index_array_[0] = 0;
		weight_group.vector_index_count_ = (int)minima_list.size();
		weight_group.vector_index_array_ = new int[weight_group.vector_index_count_];
		weight_group.vector_weight_array_ = new float[weight_group.vector_index_count_*weight_group.bone_count_];
		std::list<int>::iterator i = minima_list.begin();
		for (v = 0; i != minima_list.end(); ++i, ++v) {
			weight_group.vector_index_array_[v] = *i;
			weight_group.vector_weight_array_[v] = 1;
		}
		skin->AddBoneWeights(weight_group);
	}

	// The middle part. Segment the geometry into (bone count - 1) groups; i.e. two bones per vertex.
	const int segments = bone_count-1;
	for (int y = 0; y < segments; ++y) {
		const float part_min = Math::Lerp(start_y, end_y, (float)y/segments);
		const float part_max = Math::Lerp(start_y, end_y, (float)(y+1)/segments);
		std::list<int> segment_list;
		for (v = 0; v < geometry->GetVertexCount(); ++v) {
			float yp = geometry->GetVertexData()[v*3+1];
			if (yp >= part_min && yp < part_max) {
				// Make sure it's not part of the caps.
				if (std::find(minima_list.begin(), minima_list.end(), (int)v) == minima_list.end() &&
					std::find(maxima_list.begin(), maxima_list.end(), (int)v) == maxima_list.end()) {
					segment_list.push_back(v);
				}
			}
		}
		if (!segment_list.empty()) {
			weight_group.bone_count_ = 2;
			weight_group.bone_index_array_[0] = y;
			weight_group.bone_index_array_[1] = y+1;
			weight_group.vector_index_count_ = (int)segment_list.size();
			weight_group.vector_index_array_ = new int[weight_group.vector_index_count_];
			weight_group.vector_weight_array_ = new float[weight_group.vector_index_count_*weight_group.bone_count_];
			std::list<int>::iterator i = segment_list.begin();
			for (v = 0; i != segment_list.end(); ++i, ++v) {
				int idx = *i;
				weight_group.vector_index_array_[v] = idx;
				float yp = geometry->GetVertexData()[idx*3+1];
				// Scale down to [0, 1].
				yp = (yp-part_min)/(part_max-part_min);
				// Scale up to [0, 2];
				yp *= 2;
				// Move by the power of n towards the closest part.
				const float n = bone_stiffness;
				yp = ::pow(yp, n);
				// Scale down to [0, 1] again.
				yp /= (float)::pow(2, n);
				weight_group.vector_weight_array_[v*2+0] = 1-yp;
				weight_group.vector_weight_array_[v*2+1] = yp;
			}
			skin->AddBoneWeights(weight_group);
		}
	}

	if (!maxima_list.empty()) {
		// Create weights for distal end cap of skinned mesh.
		weight_group.bone_count_ = 1;
		weight_group.bone_index_array_[0] = bone_count-1;
		weight_group.vector_index_count_ = (int)maxima_list.size();
		weight_group.vector_index_array_ = new int[weight_group.vector_index_count_];
		weight_group.vector_weight_array_ = new float[weight_group.vector_index_count_*weight_group.bone_count_];
		std::list<int>::iterator i = maxima_list.begin();
		for (v = 0; i != maxima_list.end(); ++i, ++v) {
			weight_group.vector_index_array_[v] = *i;
			weight_group.vector_weight_array_[v] = 1;
		}
		skin->AddBoneWeights(weight_group);
	}
}



}
