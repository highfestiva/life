/*
	Class:  GraphicalModel
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#include "pch.h"
#include "../include/uigraphicalmodel.h"
#include "../include/uitrianglebasedgeometry.h"
#include "../include/uianimatedgeometry.h"
#include "../include/uiprogressivetrianglegeometry.h"

namespace uitbc {

GraphicalModel::GraphicalModel() :
	level_of_detail_(1) {
}

GraphicalModel::~GraphicalModel() {
}

void GraphicalModel::AddGeometry(const str& name, GeometryHandler* geometry, const str& transform_animator) {
	geometry->SetTransformAnimator(Model::GetAnimator(transform_animator));
	geometry->UpdateGeometry((float)level_of_detail_);
	geometry_table_.Insert(name, geometry);
}

tbc::GeometryBase* GraphicalModel::GetGeometry(const str& name) {
	tbc::GeometryBase* _geometry = 0;
	GeometryTable::Iterator iter = geometry_table_.Find(name);
	if (iter != geometry_table_.End()) {
		_geometry = (*iter)->GetGeometry();
	}

	return _geometry;
}

void GraphicalModel::Update(double delta_time) {
	Model::Update(delta_time);

	GeometryTable::Iterator iter;
	for (iter = geometry_table_.First(); iter != geometry_table_.End(); ++iter) {
		GeometryHandler* geometry_handler = *iter;
		geometry_handler->UpdateGeometry((float)level_of_detail_);

		tbc::GeometryBase* _geometry = geometry_handler->GetGeometry();
		tbc::BoneAnimator* animator = geometry_handler->GetTransformAnimator();

		// Set transformation.
		xform transform;
		if (animator != 0) {
			transform = transformation_.Transform(animator->GetBones()->GetRelativeBoneTransformation(0));
		} else {
			transform = transformation_;
		}

		_geometry->SetTransformation(transform);

		if(_geometry->GetUVAnimator() != 0) {
			_geometry->GetUVAnimator()->Step((float)delta_time);
		}
	}
}

void GraphicalModel::SetDetailLevel(double level_of_detail) {
	level_of_detail_ = level_of_detail;
}

void GraphicalModel::SetLastFrameVisible(unsigned int last_frame_visible) {
	GeometryTable::Iterator iter;
	for (iter = geometry_table_.First(); iter != geometry_table_.End(); ++iter) {
		(*iter)->GetGeometry()->SetLastFrameVisible(last_frame_visible);
	}
}

unsigned int GraphicalModel::GetLastFrameVisible() const {
	unsigned int _last_frame_visible = 0;

	if (geometry_table_.IsEmpty() == false) {
		_last_frame_visible = (*geometry_table_.First())->GetGeometry()->GetLastFrameVisible();
	}
	return _last_frame_visible;
}

void GraphicalModel::SetAlwaysVisible(bool always_visible) {
	GeometryTable::Iterator iter;
	for (iter = geometry_table_.First(); iter != geometry_table_.End(); ++iter) {
		(*iter)->GetGeometry()->SetAlwaysVisible(always_visible);
	}
}

bool GraphicalModel::GetAlwaysVisible() {
	bool _always_visible = false;

	if (geometry_table_.IsEmpty() == false) {
		_always_visible = (*geometry_table_.First())->GetGeometry()->GetAlwaysVisible();
	}
	return _always_visible;
}








DefaultStaticGeometryHandler::DefaultStaticGeometryHandler(TriangleBasedGeometry* geometry,
							   int num_lod_levels,
							   Renderer::TextureID* texture_id,
							   int num_textures,
							   Renderer::MaterialType material,
							   Renderer::Shadows shadows,
							   Renderer* renderer) :
	geometry_(geometry),
	num_lod_levels_(num_lod_levels),
	texture_id_(texture_id),
	num_textures_(num_textures),
	material_(material),
	shadows_(shadows),
	current_lod_level_(-1),
	geom_id_(Renderer::INVALID_GEOMETRY),
	renderer_(renderer) {
}

void DefaultStaticGeometryHandler::UpdateGeometry(float lod_level) {
	int new_lod_index = (int)std::min(num_lod_levels_ - 1, (int)floor(lod_level * (num_lod_levels_ - 1) + 0.5));

	if (new_lod_index != current_lod_level_) {
		current_lod_level_ = new_lod_index;
		renderer_->RemoveGeometry(geom_id_);
		geom_id_ = renderer_->AddGeometry(&geometry_[new_lod_index],
						   material_,
						   shadows_);
		if (geom_id_ != Renderer::INVALID_GEOMETRY) {
			for (int x = 0; x < num_textures_; ++x) {
				renderer_->TryAddGeometryTexture(geom_id_, texture_id_[x]);
			}
		}
	}
}

tbc::GeometryBase* DefaultStaticGeometryHandler::GetGeometry() {
	tbc::GeometryBase* _geometry = 0;
	if (current_lod_level_ >= 0 && current_lod_level_ < num_lod_levels_) {
		_geometry = &geometry_[current_lod_level_];
	}
	return _geometry;
}




DefaultProgressiveGeometryHandler::DefaultProgressiveGeometryHandler(ProgressiveTriangleGeometry* geometry,
								     Renderer::MaterialType material,
								     Renderer::TextureID* texture_id,
								     int num_textures,
								     Renderer::Shadows shadows,
								     Renderer* renderer) :
	geometry_(geometry),
	renderer_(renderer) {
	geom_id_ = renderer_->AddGeometry(geometry_, material, shadows);
	if (geom_id_ != Renderer::INVALID_GEOMETRY) {
		for (int x = 0; x < num_textures; ++x) {
			renderer_->TryAddGeometryTexture(geom_id_, texture_id[x]);
		}
	}
}

void DefaultProgressiveGeometryHandler::UpdateGeometry(float lod_level) {
	geometry_->SetDetailLevel(lod_level);
}

tbc::GeometryBase* DefaultProgressiveGeometryHandler::GetGeometry() {
	return geometry_;
}




DefaultAnimatedStaticGeometryHandler::DefaultAnimatedStaticGeometryHandler(AnimatedGeometry* geometry,
									   int num_lod_levels,
									   Renderer::TextureID* texture_id,
									   int num_textures,
									   Renderer::MaterialType material,
									   Renderer::Shadows shadows,
									   Renderer* renderer) :
	geometry_(geometry),
	num_lod_levels_(num_lod_levels),
	texture_id_(texture_id),
	num_textures_(num_textures),
	material_(material),
	shadows_(shadows),
	current_lod_level_(-1),
	geom_id_(Renderer::INVALID_GEOMETRY),
	renderer_(renderer) {
}

void DefaultAnimatedStaticGeometryHandler::UpdateGeometry(float lod_level) {
	int new_lod_index = (int)std::min(num_lod_levels_ - 1, (int)floor(lod_level * (num_lod_levels_ - 1) + 0.5));

	if (new_lod_index != current_lod_level_) {
		current_lod_level_ = new_lod_index;
		renderer_->RemoveGeometry(geom_id_);
		geom_id_ = renderer_->AddGeometry(&geometry_[new_lod_index],
						   material_,
						   shadows_);
		if (geom_id_ != Renderer::INVALID_GEOMETRY) {
			for (int x = 0; x < num_textures_; ++x) {
				renderer_->TryAddGeometryTexture(geom_id_, texture_id_[x]);
			}
		}
	}

	geometry_[current_lod_level_].UpdateAnimatedGeometry();
}

tbc::GeometryBase* DefaultAnimatedStaticGeometryHandler::GetGeometry() {
	tbc::GeometryBase* _geometry = 0;
	if (current_lod_level_ >= 0 && current_lod_level_ < num_lod_levels_) {
		_geometry = &geometry_[current_lod_level_];
	}
	return _geometry;
}




DefaultAnimatedProgressiveGeometryHandler::DefaultAnimatedProgressiveGeometryHandler(AnimatedGeometry* geometry,
										     Renderer::TextureID* texture_id,
										     int num_textures,
										     Renderer::MaterialType material,
										     Renderer::Shadows shadows,
										     Renderer* renderer) :
	geometry_(geometry),
	texture_id_(texture_id),
	num_textures_(num_textures),
	material_(material),
	shadows_(shadows),
	geom_id_(Renderer::INVALID_GEOMETRY),
	renderer_(renderer) {
}

void DefaultAnimatedProgressiveGeometryHandler::UpdateGeometry(float lod_level) {
	// TRICKY: This typecast is the reason why the user needs to be extra careful
	//         using this class.
	((ProgressiveTriangleGeometry*)geometry_->GetOriginalGeometry())->SetDetailLevel(lod_level);
	geometry_->UpdateAnimatedGeometry();
}

tbc::GeometryBase* DefaultAnimatedProgressiveGeometryHandler::GetGeometry() {
	return geometry_;
}

}
