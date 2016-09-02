
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "object.h"
#include "../cure/include/contextmanager.h"
#include "../uitbc/include/uitrianglebasedgeometry.h"



namespace TrabantSim {



Object::Object(cure::ResourceManager* resource_manager, const str& class_id, UiCure::GameUiManager* ui_manager):
	Parent(resource_manager, class_id, ui_manager),
	generated_physics_(0),
	clazz_(new uitbc::ChunkyClass),
	gfx_mesh_(0),
	gfx_mesh_id_(0) {
}

Object::~Object() {
	generated_physics_ = 0;	// Will auto-delete when physics goes missing.
	delete clazz_;
}



const tbc::ChunkyClass* Object::GetClass() const {
	return clazz_;
}

uitbc::TriangleBasedGeometry* Object::CreateGfxMesh(const std::vector<float>& vertices, const std::vector<int>& indices, const vec3& color, float alpha, bool is_smooth) {
	if (vertices.empty() || indices.empty()) {
		return 0;
	}
	uitbc::TriangleBasedGeometry* gfx_mesh = new uitbc::TriangleBasedGeometry(&vertices[0], 0, 0, 0, uitbc::TriangleBasedGeometry::kColorRgb, (const uint32*)&indices[0],
			vertices.size()/3, indices.size(), tbc::GeometryBase::kTriangles, tbc::GeometryBase::kGeomStatic);
	if (!is_smooth) {
		gfx_mesh->SplitVertices();
		gfx_mesh->ClearVertexNormalData();
		gfx_mesh->ClearSurfaceNormalData();
	}
	gfx_mesh->GetBasicMaterialSettings().diffuse_	= color;
	gfx_mesh->GetBasicMaterialSettings().specular_	= vec3();
	gfx_mesh->GetBasicMaterialSettings().shininess_	= !is_smooth;
	gfx_mesh->GetBasicMaterialSettings().smooth_	= is_smooth;
	gfx_mesh->GetBasicMaterialSettings().alpha_	= alpha;
	gfx_mesh->SetGeometryVolatility(tbc::GeometryBase::kGeomSemiStatic);
	return gfx_mesh;
}

void Object::AddMeshInfo(const str& mesh_name, const str& shader, const str& texture, const vec3& color, float alpha, bool is_smooth) {
	clazz_->AddMesh(0, mesh_name, xform(), 1);
	uitbc::ChunkyClass::Material material;
	material.diffuse_ = color;
	material.shininess_ = 1;
	material.alpha_ = alpha;
	material.smooth_ = is_smooth;
	material.resize_hint_ = Canvas::kResizeFast;
	material.texture_list_.push_back(texture);
	material.shader_name_ = shader;
	clazz_->SetLastMeshMaterial(material);
}



void Object::OnLoaded() {
	Parent::OnLoaded();
	const uitbc::ChunkyClass::Material& material = clazz_->GetMaterial(0);
	GetMesh(0)->GetBasicMaterialSettings().diffuse_ = material.diffuse_;
	if (GetPhysics()->GetPhysicsType() == tbc::ChunkyPhysics::kStatic) {
		GetManager()->DisableTickCallback(this);
	}
}



}
