// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/uimaterial.h"
#include "../../tbc/include/geometrybase.h"
#include "../../lepra/include/listutil.h"
#include "../../lepra/include/math.h"



namespace uitbc {



GeometryGroup::GeometryGroup(Material* material, int alloc_size) :
	parent_material_(material),
	geom_array_(new Pair[alloc_size]),
	geometry_count_(0),
	array_length_(alloc_size),
	mean_depth_(0),
	group_texture_id_(Renderer::INVALID_TEXTURE) {
}

GeometryGroup::~GeometryGroup() {
	delete[] geom_array_;
	geom_array_ = 0;
}

void GeometryGroup::AddGeometry(tbc::GeometryBase* geometry) {
	if (geometry_count_ >= array_length_) {
		// Realloc... Increase memory usage by a constant
		// to avoid excessive use of memory.
		array_length_ += 8;
		Pair* new_array = new Pair[array_length_];
		::memcpy(new_array, geom_array_, geometry_count_ * sizeof(Pair));
		delete[] geom_array_;
		geom_array_ = new_array;
	}

	Renderer::GeometryData* geom_data = (Renderer::GeometryData*)geometry->GetRendererData();
	deb_assert(geom_data != 0);
	geom_data->geometry_group_ = this;
	geom_array_[geometry_count_++].geometry_ = geometry;
}

bool GeometryGroup::RemoveGeometry(tbc::GeometryBase* geometry) {
	int i;

	// Search for the geometry...
	for (i = 0; i < geometry_count_ && geom_array_[i].geometry_ != geometry; i++) {}

	bool removed = false;
	if (i < geometry_count_) {
		// The geometry has been found.
		removed = true;
		geometry_count_--;
		for (int j = i; j < geometry_count_; j++) {
			geom_array_[j] = geom_array_[j + 1];
		}
	}
	return removed;
}

int GeometryGroup::CalculateDepths(bool f2_b) {
	mean_depth_ = 0.0f;

	const xform& cam = parent_material_->GetRenderer()->GetCameraTransformation();
	const quat& cam_orientation = cam.GetOrientation();
	const quat& cam_orientation_inverse = parent_material_->GetRenderer()->GetCameraOrientationInverse();
	const vec3& cam_position = cam.GetPosition();
	int inversion_count = 0;

	// The first depth goes outside the loop...
	vec3 temp;
	cam_orientation.FastInverseRotatedVector(cam_orientation_inverse, temp, geom_array_[0].geometry_->GetTransformation().GetPosition() - cam_position);
	geom_array_[0].depth_ = temp.y;
	mean_depth_ += geom_array_[0].depth_;

	int i;
	for (i = 1; i < geometry_count_; i++) {
		cam_orientation.FastInverseRotatedVector(cam_orientation_inverse, temp, geom_array_[i].geometry_->GetTransformation().GetPosition() - cam_position);
		geom_array_[i].depth_ = temp.y;
		mean_depth_ += geom_array_[i].depth_;

		if(f2_b) {
			if (geom_array_[i - 1].depth_ > geom_array_[i].depth_)
				++inversion_count;
		} else {
			if (geom_array_[i - 1].depth_ < geom_array_[i].depth_)
				++inversion_count;
		}
	}

	mean_depth_ /= (float)geometry_count_;

	return inversion_count;
}

void GeometryGroup::F2BSortGroup() {
	int inversion_count = CalculateDepths(true);

	if (inversion_count == 0) {
		// Already sorted.
		return;
	}

	if (geometry_count_ < 4 || inversion_count < Math::Log2(geometry_count_)) {
		// Bubble sorting is faster for almost sorted lists.
		BubbleSort(F2BCompare);
	} else {
		::qsort(geom_array_, geometry_count_, sizeof(Pair), F2BCompare);
	}
}

void GeometryGroup::B2FSortGroup() {
	int inversion_count = CalculateDepths(false);

	if (inversion_count == 0) {
		// Already sorted.
		return;
	}

	if (geometry_count_ < 4 || inversion_count < Math::Log2(geometry_count_)) {
		// Bubble sorting is faster for almost sorted lists.
		BubbleSort(B2FCompare);
	} else {
		::qsort(geom_array_, geometry_count_, sizeof(Pair), B2FCompare);
	}
}

void GeometryGroup::BubbleSort(int (*Cmp)(const void* geom1, const void* geom2)) {
	bool done = false;
	while (done == false) {
		done = true;
		for (int i = 1; i < geometry_count_; i++) {
			if (Cmp(&geom_array_[i - 1], &geom_array_[i]) > 0) {
				Pair tmp = geom_array_[i - 1];
				geom_array_[i - 1] = geom_array_[i];
				geom_array_[i] = tmp;
				done = false;
			}
		}
	}
}

int GeometryGroup::F2BCompare(const void* pair1, const void* pair2) {
	if (((Pair*)pair1)->depth_ < ((Pair*)pair2)->depth_) {
		return -1;
	} else if (((Pair*)pair1)->depth_ > ((Pair*)pair2)->depth_) {
		return 1;
	} else {
		return 0;
	}
}

int GeometryGroup::B2FCompare(const void* pair1, const void* pair2) {
	if (((Pair*)pair1)->depth_ > ((Pair*)pair2)->depth_) {
		return -1;
	} else if (((Pair*)pair1)->depth_ < ((Pair*)pair2)->depth_) {
		return 1;
	} else {
		return 0;
	}
}



Material::Material(Renderer* renderer, DepthSortHint sort_hint, Material* fall_back_material):
	renderer_(renderer),
	sort_hint_(sort_hint),
	fall_back_material_(fall_back_material) {
}

Material::~Material() {
	RemoveAllGeometry();
}



void Material::EnableWireframe(bool enabled) {
	enable_wireframe_ = enabled;
}

void Material::SetEnableDepthSorting(bool enabled) {
	enable_depth_sort_ = enabled;
}

void Material::EnableDrawMaterial(bool enabled) {
	enable_draw_material_ = enabled;
}



Renderer* Material::GetRenderer() {
	return renderer_;
}

bool Material::AddGeometry(tbc::GeometryBase* geometry) {
	if (geometry == 0)
		return false;


	GeometryGroupList::iterator iter;
	for (iter = geometry_group_list_.begin(); iter != geometry_group_list_.end(); ++iter) {
		if ((*iter)->GetGroupTextureID() == GetGroupTextureID(geometry)) {
			break;
		}
	}

	GeometryGroup* group = 0;
	if (iter != geometry_group_list_.end()) {
		group = *iter;
	} else {
		group = new GeometryGroup(this);
		geometry_group_list_.push_back(group);
	}

	group->AddGeometry(geometry);

	return true;
}

Material::RemoveStatus Material::RemoveGeometry(tbc::GeometryBase* geometry) {
	Renderer::GeometryData* geom_data = (Renderer::GeometryData*)geometry->GetRendererData();
	RemoveStatus status = (geom_data->geometry_group_->RemoveGeometry(geometry) ? kRemoved : kNotRemoved);

	if (geom_data->geometry_group_->GetGeometryCount() == 0) {
		Material* _material = this;
		while (_material && !ListUtil::Contains(_material->geometry_group_list_, geom_data->geometry_group_)) {
			_material = _material->fall_back_material_;
		}
		deb_assert(_material);
		if (_material) {
			_material->geometry_group_list_.remove(geom_data->geometry_group_);
			delete geom_data->geometry_group_;
			geom_data->geometry_group_ = 0;
		}
	}

	return status;
}

void Material::RemoveAllGeometry() {
	GeometryGroupList::iterator iter;
	while(geometry_group_list_.empty() == false) {
		GeometryGroup* group = geometry_group_list_.front();
		geometry_group_list_.pop_front();
		delete group;
	}
}

void Material::PreRender() {
}

void Material::PostRender() {
}

void Material::RenderAllGeometry(unsigned current_frame, Material* geometry_container, Material* renderer) {
	const GeometryGroupList& geometries = geometry_container->GetGeometryGroupList();
	if (geometries.empty()) {
		return;
	}
	if (!renderer) {
		renderer = geometry_container;
	}
	renderer->RenderAllGeometry(current_frame, geometries);
}

tbc::GeometryBase* Material::GetFirstGeometry() {
	group_iter_ = geometry_group_list_.begin();
	index_ = 0;
	tbc::GeometryBase* _geometry = 0;
	if(group_iter_ != geometry_group_list_.end()) {
		_geometry = (*group_iter_)->GetGeometry(index_);
	}
	return _geometry;
}

tbc::GeometryBase* Material::GetNextGeometry() {
	tbc::GeometryBase* _geometry = 0;
	if (group_iter_ != geometry_group_list_.end()) {
		++index_;
		if(index_ >= (*group_iter_)->GetGeometryCount()) {
			index_ = 0;
			++group_iter_;
		}
	}
	if(group_iter_ != geometry_group_list_.end()) {
		_geometry = (*group_iter_)->GetGeometry(index_);
	}
	return _geometry;
}

void Material::RenderAllGeometry(unsigned current_frame, const GeometryGroupList& geometry_group_list) {
	if (enable_draw_material_) {
		DoRenderAllGeometry(current_frame, geometry_group_list);
	} else {
		Material::DoRenderAllGeometry(current_frame, geometry_group_list);
	}
}

void Material::RenderAllBlendedGeometry(unsigned current_frame, const GeometryGroupList& geometry_group_list) {
	tbc::GeometryBase::BasicMaterialSettings previous_material = current_material_;
	bool old_enable_draw_material = enable_draw_material_;
	enable_draw_material_ = true;
	enable_depth_sort_ = true;

	DoRenderAllGeometry(current_frame, geometry_group_list);

	SetBasicMaterial(previous_material);
	GetRenderer()->ResetAmbientLight(true);
	enable_draw_material_ = old_enable_draw_material;
	enable_depth_sort_ = false;
}

void Material::DoRenderAllGeometry(unsigned current_frame, const GeometryGroupList& geometry_group_list) {
	PreRender();

	GeometryGroupList::const_iterator iter;
	for (iter = geometry_group_list.begin(); iter != geometry_group_list.end(); ++iter) {
		GeometryGroup* group = *iter;

		if (enable_depth_sort_ == true) {
			if (sort_hint_ == kDepthsortF2B) {
				group->F2BSortGroup();
			} else if (sort_hint_ == kDepthsortB2F) {
				group->B2FSortGroup();
			}
		}

		const int geometry_count = group->GetGeometryCount();
		for (int i = 0; i < geometry_count; i++) {
			tbc::GeometryBase* _geometry = group->GetGeometry(i);
			if (_geometry->GetAlwaysVisible() || _geometry->GetLastFrameVisible() == current_frame) {
				if (renderer_->PreRender(_geometry)) {
					if (enable_draw_material_) {
						RenderGeometry(_geometry);
					} else {
						RenderBaseGeometry(_geometry);
					}
				}
				renderer_->PostRender(_geometry);
			}
		}
	}

	PostRender();
}

Renderer::TextureID Material::GetGroupTextureID(tbc::GeometryBase* geometry) const {
	Renderer::TextureID texture_id = Renderer::INVALID_TEXTURE;

	Renderer::GeometryData* geom_data = (Renderer::GeometryData*)geometry->GetRendererData();
	if (geom_data->ta_ && geom_data->ta_->num_textures_ > 0) {
		// Select the first texture for grouping.
		texture_id = geom_data->ta_->texture_id_[0];
	}

	return texture_id;
}

const Material::GeometryGroupList& Material::GetGeometryGroupList() const {
	return geometry_group_list_;
}



tbc::GeometryBase::BasicMaterialSettings Material::current_material_;
bool Material::enable_wireframe_ = false;
bool Material::enable_depth_sort_ = false;
bool Material::enable_draw_material_ = true;



}
