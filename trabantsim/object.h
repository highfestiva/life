
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../uicure/include/uimachine.h"
#include "trabantsim.h"



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



namespace TrabantSim {



class Object: public UiCure::Machine {
	typedef UiCure::Machine Parent;
public:
	Object(cure::ResourceManager* resource_manager, const str& class_id, UiCure::GameUiManager* ui_manager);
	virtual ~Object();

	virtual const tbc::ChunkyClass* GetClass() const;
	void CreatePhysics(tbc::ChunkyPhysics* physics);
	uitbc::TriangleBasedGeometry* CreateGfxMesh(const std::vector<float>& vertices, const std::vector<int>& indices, const vec3& color, float alpha, bool is_smooth);
	void AddMeshInfo(const str& mesh_name, const str& shader, const str& texture, const vec3& color=vec3(1,1,1), float alpha=1, bool is_smooth=false);

	virtual void OnLoaded();

	quat initial_orientation_;
	quat initial_inverse_orientation_;
	tbc::ChunkyPhysics* generated_physics_;
	int physics_index_;
	bool same_as_previous_;
	vec3 scale_;

private:
	void OnLoadPhysics(cure::UserPhysicsReferenceResource* physics_resource);

	uitbc::ChunkyClass* clazz_;
};



}
