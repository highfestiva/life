
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../uicure/include/uimachine.h"
#include "bound.h"



namespace lepra {
class Color;
class Plane;
}
namespace tbc {
class PhysicsManager;
}
namespace uitbc {
class TriangleBasedGeometry;
}



namespace Bound {



class Level: public UiCure::Machine {
	typedef UiCure::Machine Parent;
public:
	Level(cure::ResourceManager* resource_manager, const str& class_id, UiCure::GameUiManager* ui_manager);
	virtual ~Level();

	void GenerateLevel(tbc::PhysicsManager* physics_manager, bool vary_shapes, int level);
	const uitbc::TriangleBasedGeometry* GetMesh() const;
	const uitbc::TriangleBasedGeometry* GetWindowMesh() const;
	float GetVolumePart() const;
	void SetTriangles(tbc::PhysicsManager* physics_manager, const std::vector<float>& vertices, const std::vector<uint8>& colors);
	void SetWindowTriangles(const std::vector<float>& vertices);
	void AddCutPlane(tbc::PhysicsManager* physics_manager, const Plane& window_plane, const std::vector<float>& vertices, const Color& color);
	void RenderOutline();

private:
	uitbc::TriangleBasedGeometry* CreateMesh(bool vary_shapes, int level, float x, float y, float z);
	static void SetVertices(uitbc::TriangleBasedGeometry* gfx_mesh, const float* v, size_t vc, const uint8* color_data);
	static void FlipTriangles(uitbc::TriangleBasedGeometry* mesh);
	static void GenerateVertexColors(uitbc::TriangleBasedGeometry* mesh);
	void CreatePhysicsMesh(tbc::PhysicsManager* physics_manager);
	void AddPhysicsWindowBox(tbc::PhysicsManager* physics_manager, const Plane& plane);
	void DeleteWindowBoxes(tbc::PhysicsManager* physics_manager);
	float CalculateVolume() const;

	uitbc::TriangleBasedGeometry* gfx_mesh_;
	uitbc::TriangleBasedGeometry* gfx_window_mesh_;
	uitbc::Renderer::GeometryID gfx_mesh_id_;
	uitbc::Renderer::GeometryID gfx_window_mesh_id_;
	tbc::PhysicsManager::BodyID phys_mesh_body_id_;
	std::vector<tbc::PhysicsManager::BodyID> phys_window_box_ids_;
	uint32* body_index_data_;
	vec3 size_;
	int level_;
	float original_volume_;
	float volume_;
};



}
