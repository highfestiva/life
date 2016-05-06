
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/uichunkyclass.h"
#include "../../lepra/include/lepraassert.h"



namespace uitbc {



ChunkyClass::Material::Material():
	ambient_(0,0,0),
	diffuse_(1,1,1),
	specular_(0.1f,0.1f,0.1f),
	shininess_(0),
	alpha_(1),
	smooth_(true) {
}



ChunkyClass::ChunkyClass() {
}

ChunkyClass::~ChunkyClass() {
}



void ChunkyClass::AddMesh(int phys_index, const str& mesh_base_name, const xform& transform, float scale) {
	mesh_array_.push_back(PhysMeshEntry(phys_index, mesh_base_name, transform, scale));
}

size_t ChunkyClass::GetMeshCount() const {
	return (mesh_array_.size());
}

void ChunkyClass::GetMesh(size_t index, int& phys_index, str& mesh_base_name, xform& transform, float& scale) const {
	deb_assert(index < mesh_array_.size());
	phys_index = mesh_array_[index].phys_index_;
	mesh_base_name = mesh_array_[index].mesh_base_name_;
	transform = mesh_array_[index].transform_;
	scale = mesh_array_[index].scale_;
}

void ChunkyClass::SetLastMeshMaterial(const Material& material) {
	deb_assert(!mesh_array_.empty());
	const size_t last_mesh = mesh_array_.size()-1;
	mesh_array_[last_mesh].material_ = material;
}

const ChunkyClass::Material& ChunkyClass::GetMaterial(size_t mesh_index) const {
	deb_assert(mesh_index < mesh_array_.size());
	return (mesh_array_[mesh_index].material_);
}



loginstance(kGameContext, ChunkyClass);



}
