
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "massobject.h"
#include "../../cure/include/contextmanager.h"
#include "../../cure/include/gamemanager.h"
#include "../../lepra/include/cyclicarray.h"
#include "../../lepra/include/random.h"
#include "../../uitbc/include/uigeometrybatch.h"
#include "../../uicure/include/uigameuimanager.h"
#include "rtvar.h"



namespace life {



MassObject::MassObject(cure::ResourceManager* resource_manager, const str& class_resource_name,
	UiCure::GameUiManager* ui_manager, tbc::PhysicsManager::BodyID terrain_body_id, size_t instance_count,
	float side_length):
	Parent(resource_manager, class_resource_name, ui_manager),
	terrain_body_id_(terrain_body_id),
	square_instance_count_(instance_count/kSquareCount),
	visible_add_term_(0.1f),
	square_side_length_((int)(side_length/kSquareSide)),
	middle_square_x_(0x80000000),
	middle_square_y_(0x80000000),
	seed_(0) {
	deb_assert(square_instance_count_ > 0);
	::memset(square_array_, 0, sizeof(square_array_));
	fully_visible_distance_ = square_side_length_ * (kSquareMidToCorner - 1.0f);
	visible_distance_factor_ = (1+visible_add_term_)/square_side_length_;

	SetPhysicsTypeOverride(cure::kPhysicsOverrideBones);
}

MassObject::~MassObject() {
	for (int x = 0; x < kSquareCount; ++x) {
		delete square_array_[x];
	}
}



void MassObject::SetSeed(unsigned seed) {
	seed_ = seed;
}

void MassObject::SetRender(bool render) {
	const vec3& center_position = GetPosition();
	for (int y = 0; y < kSquareSide; ++y) {
		for (int x = 0; x < kSquareSide; ++x) {
			const size_t offset = y*kSquareSide+x;
			if (square_array_[offset]) {
				vec3 square_position;
				GridToPosition(x, y, square_position);
				const float distance = square_position.GetDistance(center_position);
				const float _alpha = (1.0f + visible_add_term_) - Math::Lerp(0.0f, 1.0f, (distance-fully_visible_distance_)*visible_distance_factor_);
				square_array_[offset]->SetRender(render, _alpha);
			}
		}
	}
}

void MassObject::UiMove() {
	if (!GetUiManager()->CanRender()) {	// Can only create/remove graphical mass objects if the renderer is online.
		return;
	}

	int x = middle_square_x_;
	int y = middle_square_y_;

	PositionToGrid(GetPosition(), x, y);
	if (x != middle_square_x_ || y != middle_square_y_) {
		MoveToSquare(x, y);
	}
	for (size_t v = 0; v < kSquareSide; ++v) {
		for (size_t u = 0; u < kSquareSide; ++u) {
			const size_t offset = v*kSquareSide+u;
			if (!square_array_[offset]) {
				CreateSquare(u, v);
				return;	// Optimization: only create a single square every loop to avoid heavy burdon on single loop.
			}
		}
	}
}



void MassObject::OnLoaded() {
	Parent::OnLoaded();

	for (MeshArray::const_iterator y = mesh_resource_array_.begin(); y != mesh_resource_array_.end(); ++y) {
		tbc::GeometryReference* mesh = (tbc::GeometryReference*)(*y)->GetRamData();
		if (mesh) {
			mesh->SetAlwaysVisible(false);
		}
	}
}

void MassObject::PositionToGrid(const vec3& position, int& _x, int& _y) const {
	// 2's complement...
	_x = (position.x < 0)? (int)(position.x/square_side_length_)-1 : (int)(position.x/square_side_length_);
	_y = (position.y < 0)? (int)(position.y/square_side_length_)-1 : (int)(position.y/square_side_length_);
}

void MassObject::GridToPosition(int _x, int _y, vec3& position) const {
	// 2's complement...
	position.x = (float)((_x-kSquareMidToCorner+middle_square_x_) * square_side_length_);
	position.y = (float)((_y-kSquareMidToCorner+middle_square_y_) * square_side_length_);
}

void MassObject::MoveToSquare(int _x, int _y) {
	const int dy = _y - middle_square_y_;
	const int dx = _x - middle_square_x_;

	// Move all existing once that are in reach.
	Square* square_array[kSquareCount];
	::memcpy(square_array, square_array_, sizeof(square_array));
	for (int v = 0; v < kSquareSide; ++v) {
		const size_t y = v + dy;
		for (int u = 0; u < kSquareSide; ++u) {
			const size_t x = u + dx;
			Square* square = 0;
			if (x < kSquareSide && y < kSquareSide) {
				const size_t read_offset = y*kSquareSide+x;
				square = square_array[read_offset];
				square_array[read_offset] = 0;
			}
			const size_t write_offset = v*kSquareSide+u;
			square_array_[write_offset] = square;
		}
	}
	// Destroy all that are no longer in use.
	for (int x = 0; x < kSquareCount; ++x) {
		delete square_array[x];
	}

	middle_square_x_ = _x;
	middle_square_y_ = _y;
}

void MassObject::CreateSquare(size_t _x, size_t _y) {
	deb_assert(_x < kSquareSide && _y < kSquareSide);
	deb_assert(!square_array_[_y*kSquareSide+_x]);

	uint32 _seed = seed_ + (uint32)((_y<<16)+_x);
	std::vector<xform> _displacement_array;
	for (size_t x = 0; x < square_instance_count_; ++x) {
		quat rotation;
		rotation.RotateAroundOwnY(Random::Uniform(_seed, 0.0f, PIF*2));
		rotation.RotateAroundOwnX(Random::Uniform(_seed, 0.0f, PIF/8));
		rotation.RotateAroundOwnZ(Random::Uniform(_seed, 0.0f, PIF/8));
		vec3 _position;
		GridToPosition(_x, _y, _position);
		_position.x += Random::Uniform(_seed, 0.3f, (float)square_side_length_);
		_position.y += Random::Uniform(_seed, 0.3f, (float)square_side_length_);
		if (GetObjectPlacement(_position)) {
			std::swap(_position.y, _position.z);	// TRICKY: transform from RG coords to Maya coords.
			_position.z = -_position.z;	// TRICKY: transform from RG coords to Maya coords.
			//log_.Infof("Placing mass object at (%g; %g; %g)", _position.x, _position.y, _position.z);
			_displacement_array.push_back(xform(rotation, _position));
		}
	}
	square_array_[_y*kSquareSide+_x] = new Square(_seed, mesh_resource_array_, _displacement_array, GetUiManager()->GetRenderer());
}

bool MassObject::GetObjectPlacement(vec3& position) const {
	const float ray_length = 500;
	position.z += ray_length * 0.5f;
	quat orientation;
	orientation.RotateAroundOwnX(PIF);
	xform transform(orientation, position);
	vec3 collision_position[3];
	const int collisions = GetManager()->GetGameManager()->GetPhysicsManager()->QueryRayCollisionAgainst(
		transform, ray_length, terrain_body_id_, collision_position, 3);
	if (collisions >= 1) {
		unsigned index = 0;
		if (collisions > 1) {
			unsigned _seed = ((int)position.y << 10) + (int)position.x;
			index = Random::GetRandomNumber(_seed) % collisions;
		}
		position.z = collision_position[index].z;
		return true;
	}
	return false;
}



MassObject::Square::Square(uint32 seed, const MeshArray& resource_array,
	const std::vector<xform>& displacement_array, uitbc::Renderer* renderer):
	renderer_(renderer) {
	if (displacement_array.empty()) {
		return;	// No mesh batches if not inside terrain.
	}

	quat rotation;
	rotation.RotateAroundWorldX(PIF*0.5f);
	for (MeshArray::const_iterator y = resource_array.begin(); y != resource_array.end(); ++y) {
		tbc::GeometryReference* mesh = (tbc::GeometryReference*)(*y)->GetRamData();

		uitbc::GeometryBatch* batch = new uitbc::GeometryBatch(mesh);
		batch->SetBasicMaterialSettings(mesh->GetBasicMaterialSettings());
		batch->SetTransformation(xform(rotation, vec3(0, 0, 0)));
		batch->SetInstances(&displacement_array[0], mesh->GetTransformation().GetPosition(),
			displacement_array.size(), seed, 0.8f, 1.3f, 0.6f, 2.0f, 0.7f, 1.2f);
		typedef uitbc::Renderer R;
		R::GeometryID geometry_id = renderer_->AddGeometry(batch, R::kMatSingleColorSolidPxs, R::kForceNoShadows);
		mass_mesh_array_.push_back(MassMeshPair(batch, geometry_id));
	}
}

MassObject::Square::~Square() {
	for (MassMeshArray::iterator x = mass_mesh_array_.begin(); x != mass_mesh_array_.end(); ++x) {
		renderer_->RemoveGeometry(x->second);
		delete x->first;
	}
	//mass_mesh_array_.clear();
	//renderer_ = 0;
}

void MassObject::Square::SetRender(bool render, float alpha) {
	alpha = Math::Clamp(alpha, 0.0f, 1.0f);
	bool mass_object_fading;
	v_get(mass_object_fading, =, UiCure::GetSettings(), kRtvarUi3DEnablemassobjectfading, true);
	if (!mass_object_fading) {
		alpha = 1;
	}
	for (MassMeshArray::iterator x = mass_mesh_array_.begin(); x != mass_mesh_array_.end(); ++x) {
		x->first->SetAlwaysVisible(render);
		x->first->GetBasicMaterialSettings().alpha_ = alpha;
		if (alpha <= 0.001f) {
			x->first->SetAlwaysVisible(false);
		} else if (alpha >= 0.99f) {
			renderer_->ChangeMaterial(x->second, uitbc::Renderer::kMatSingleColorSolidPxs);
		} else {
			renderer_->ChangeMaterial(x->second, uitbc::Renderer::kMatSingleColorOutlineBlended);
		}
	}
}



loginstance(kGameContextCpp, MassObject);



}
