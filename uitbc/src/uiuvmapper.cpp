/*
	Class:  UVMapper
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#include "pch.h"
#include "../include/uiuvmapper.h"
#include "../../tbc/include/../../tbc/include/geometrybase.h"
#include <math.h>


namespace uitbc {

bool UVMapper::ApplyPlanarMapping(tbc::GeometryBase* geometry, unsigned int uv_set,
				  const Vector2DD& uv_offset,
				  const Vector3DD& plane_x,
				  const Vector3DD& plane_y) {
	const double epsilon = 1e-15;

	// Check if vectors are parallel.
	double dot = plane_x.Dot(plane_y);
	dot /= (plane_x.GetLength() * plane_y.GetLength());

	bool status_ok = (fabs(dot) < 1.0 - epsilon);
	float* vertex_data = geometry->GetVertexData();
	float* uv_data = geometry->GetUVData(uv_set);

	if (status_ok) {
		status_ok = (vertex_data != 0 && uv_data != 0);
	}

	if (status_ok) {
		unsigned int i;
		for (i = 0; i < geometry->GetVertexCount(); i++) {
			int v_index = i * 3;
			int uv_index = i * 2;
			Vector3DD v((double)vertex_data[v_index + 0],
					     (double)vertex_data[v_index + 1],
					     (double)vertex_data[v_index + 2]);

			uv_data[uv_index + 0] = (float)plane_x.Dot(v) + (float)uv_offset.x;
			uv_data[uv_index + 1] = (float)plane_y.Dot(v) + (float)uv_offset.y;
		}
	}

	return status_ok;
}


bool UVMapper::ApplyCubeMapping(tbc::GeometryBase* geometry, unsigned int uv_set,
				float scale,
				const Vector2DD& uv_offset_left,
				const Vector2DD& uv_offset_right,
				const Vector2DD& uv_offset_top,
				const Vector2DD& uv_offset_bottom,
				const Vector2DD& uv_offset_front,
				const Vector2DD& uv_offset_back) {
	geometry->GenerateSurfaceNormalData();

	float* vertex_data = geometry->GetVertexData();
	float* surface_normal = geometry->GetSurfaceNormalData();
	vtx_idx_t* index = geometry->GetIndexData();
	float* uv_data = geometry->GetUVData(uv_set);

	scale = 1.0f / scale;

	bool status_ok = (vertex_data != 0 && surface_normal != 0 && index != 0 && uv_data != 0);

	if (status_ok) {
		unsigned int i;
		for (i = 0; i < geometry->GetTriangleCount(); i++) {
			int tri_index = i * 3;
			int v1_index = index[tri_index + 0] * 3;
			int v2_index = index[tri_index + 1] * 3;
			int v3_index = index[tri_index + 2] * 3;
			int u_v1 = index[tri_index + 0] * 2;
			int u_v2 = index[tri_index + 1] * 2;
			int u_v3 = index[tri_index + 2] * 2;

			float x = surface_normal[tri_index + 0];
			float y = surface_normal[tri_index + 1];
			float z = surface_normal[tri_index + 2];

			float abs_x = abs(x);
			float abs_y = abs(y);
			float abs_z = abs(z);

			vec3 x_axis;
			vec3 y_axis;
			vec2 _uv_offset;

			if (abs_x > abs_y && abs_x > abs_z) {
				// Left or right.
				if (x > 0) {
					x_axis.Set(0, 1, 0);
					_uv_offset.Set((float)uv_offset_right.x, (float)uv_offset_right.y);
				} else {
					x_axis.Set(0, -1, 0);
					_uv_offset.Set((float)uv_offset_left.x, (float)uv_offset_left.y);
				}
				y_axis.Set(0, 0, 1.0f);
			} else if(abs_y > abs_x && abs_y > abs_z) {
				// Front or back.
				if (y < 0) {
					x_axis.Set(1, 0, 0);
					_uv_offset.Set((float)uv_offset_front.x, (float)uv_offset_front.y);
				} else {
					x_axis.Set(-1, 0, 0);
					_uv_offset.Set((float)uv_offset_back.x, (float)uv_offset_back.y);
				}
				y_axis.Set(0, 0, 1.0f);
			} else {
				// Top or bottom.
				if (z > 0) {
					y_axis.Set(0, 1, 0);
					_uv_offset.Set((float)uv_offset_top.x, (float)uv_offset_top.y);
				} else {
					y_axis.Set(0, -1, 0);
					_uv_offset.Set((float)uv_offset_bottom.x, (float)uv_offset_bottom.y);
				}
				x_axis.Set(1.0f, 0, 0);
			}

			vec3 v1(vertex_data[v1_index + 0],
			                     vertex_data[v1_index + 1],
			                     vertex_data[v1_index + 2]);
			vec3 v2(vertex_data[v2_index + 0],
			                     vertex_data[v2_index + 1],
			                     vertex_data[v2_index + 2]);
			vec3 v3(vertex_data[v3_index + 0],
			                     vertex_data[v3_index + 1],
			                     vertex_data[v3_index + 2]);

			uv_data[u_v1 + 0] = (float)x_axis.Dot(v1) * scale + _uv_offset.x;
			uv_data[u_v1 + 1] = (float)y_axis.Dot(-v1) * scale + _uv_offset.y;

			uv_data[u_v2 + 0] = (float)x_axis.Dot(v2) * scale + _uv_offset.x;
			uv_data[u_v2 + 1] = (float)y_axis.Dot(-v2) * scale + _uv_offset.y;

			uv_data[u_v3 + 0] = (float)x_axis.Dot(v3) * scale + _uv_offset.x;
			uv_data[u_v3 + 1] = (float)y_axis.Dot(-v3) * scale + _uv_offset.y;
		}
	}

	return status_ok;
}

}
