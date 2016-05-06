
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "grid.h"
#include "piece.h"
#include "../cure/include/contextmanager.h"
#include "../cure/include/gamemanager.h"
#include "../lepra/include/math.h"


#define TIME_UNTIL_TOUCHDOWN	0.05f
#define DRAG_POWER		50.0f
#define VELOCITY_DAMPING	10.0f
#define STOP_DISTANCE		0.1f



namespace Impuzzable {



Piece::Piece(cure::ResourceManager* resource_manager, const str& class_id, UiCure::GameUiManager* ui_manager):
	Parent(resource_manager, class_id, ui_manager),
	average_speed_(0),
	drag_depth_(5),
	is_dragging_(false) {
	EnableMeshMove(false);	// We only use translational positioning (orientation ignored).
}

Piece::~Piece() {
}

bool Piece::TryComplete() {
	if (Parent::TryComplete()) {
		GetManager()->EnableMicroTickCallback(this);
		return true;
	}
	return false;
}



void Piece::OnTick() {
	Parent::OnTick();
	if (!IsLoaded()) {
		return;
	}

	// Set only translation, ignore orientation.
	tbc::PhysicsManager* physics = manager_->GetGameManager()->GetPhysicsManager();
	const tbc::PhysicsManager::BodyID root_body = GetRootBodyId();
	manager_->GetGameManager()->GetPhysicsManager()->RestrictBody(root_body, 10, 0.1f);
	xform t;
	physics->GetBodyTransform(root_body, t);
	quat q;
	//q.RotateAroundOwnX(-PIF/2);
	q.RotateAroundVector(vec3(1,0,0), PIF/2);
	if (MathTraits<float>::IsNan(t.position_.z)) {
		t.position_ = previous_position_;
		physics->SetBodyTransform(root_body, t);
		physics->SetBodyVelocity(root_body, vec3());
	} else {
		previous_position_ = t.position_;
		physics->GetBodyVelocity(root_body, previous_velocity_);
	}
	xform physics_transform(t);
	for (size_t x = 0; x < mesh_resource_array_.size(); ++x) {
		UiCure::UserGeometryReferenceResource* resource = mesh_resource_array_[x];
		if (resource->GetLoadState() != cure::kResourceLoadComplete) {
			continue;
		}
		tbc::GeometryReference* gfx_geometry = (tbc::GeometryReference*)resource->GetRamData();
		physics_transform = physics_->GetOriginalBoneTransformation(resource->GetOffset().geometry_index_);
		if (x == 0) {
			physics_transform.position_ = t.position_;
		} else {
			physics_transform.position_ *= q;
			physics_transform.position_ += t.position_;
			physics_transform.orientation_ = q * physics_transform.orientation_;
		}
		gfx_geometry->SetTransformation(physics_transform);
	}

	/*vec3 velocity = GetVelocity();
	bool normalize = false;
	float speed = velocity.GetLength();
	average_speed_ = Math::Lerp(average_speed_, speed, 0.1f);
	if (speed > 5.1f) {
		speed = 4.9f;
		normalize = true;
	} else if (average_speed_ < 2.0f) {
		speed = 2.9f;
		average_speed_ = 2.9f;
		normalize = true;
	}
	if (normalize) {
		velocity.Normalize(speed);
		const tbc::ChunkyBoneGeometry* geometry = physics_->GetBoneGeometry(physics_->GetRootBone());
		manager_->GetGameManager()->GetPhysicsManager()->SetBodyVelocity(geometry->GetRootBodyId(), velocity);
	}*/
}

void Piece::OnMicroTick(float) {
	if (!IsLoaded()) {
		return;
	}
	const tbc::PhysicsManager::BodyID root_body = GetRootBodyId();
	tbc::PhysicsManager* physics = manager_->GetGameManager()->GetPhysicsManager();
	xform t;
	physics->GetBodyTransform(root_body, t);
	t.GetOrientation().SetIdentity();
	t.GetOrientation().RotateAroundOwnX(PIF/2);
	physics->SetBodyTransform(root_body, t);
	physics->SetBodyAngularVelocity(root_body, vec3());
	vec3 v;
	physics->GetBodyVelocity(root_body, v);
	const vec3 center = manager_->GetGameManager()->GetPhysicsManager()->GetBodyPosition(GetRootBodyId());
	const vec3 target = is_dragging_? move_target_ : grid::GetCenterPosition(center);
	const float time = is_dragging_? TIME_UNTIL_TOUCHDOWN : 0;
	const vec3 direction = target - (center + v * time);
	const float distance = direction.GetLength();
	if (is_dragging_) {
		if (distance > STOP_DISTANCE) {
			physics->AddForce(root_body, direction*(DRAG_POWER/distance));
		} else {
			v.Set(0,0,0);
			physics->SetBodyVelocity(root_body, v);
		}
	} else {	// Move to grid.
		physics->AddForce(root_body, direction*DRAG_POWER);
	}
	if (distance < STOP_DISTANCE*5 || !is_dragging_) {
		physics->AddForce(root_body, -v*VELOCITY_DAMPING);
	}
	/*else if (v.GetLengthSquared() > MAX_VELOCITY*MAX_VELOCITY) {
		physics->SetBodyVelocity(root_body, v.GetNormalized(MAX_VELOCITY));
	}*/
}



bool Piece::IsDragging() const {
	return is_dragging_;
}

float Piece::GetDragDepth() const {
	return drag_depth_;
}

void Piece::SetDragging(bool is_dragging, float depth) {
	is_dragging_ = is_dragging;
	drag_depth_ = depth;
}

tbc::PhysicsManager::BodyID Piece::GetRootBodyId() const {
	return physics_->GetBoneGeometry(0)->GetBodyId();
}

void Piece::GetBodyIds(std::vector<tbc::PhysicsManager::BodyID>& body_ids) const {
	const int c = physics_->GetBoneCount();
	for (int x = 0; x < c; ++x) {
		body_ids.push_back(physics_->GetBoneGeometry(x)->GetBodyId());
	}
}

void Piece::SetDragPosition(const vec3& position) {
	const vec3 center = manager_->GetGameManager()->GetPhysicsManager()->GetBodyPosition(GetRootBodyId());
	drag_offset_ = position - center;
}

vec3 Piece::GetDragPosition() const {
	const vec3 center = manager_->GetGameManager()->GetPhysicsManager()->GetBodyPosition(GetRootBodyId());
	return center+drag_offset_;
}

void Piece::SetMoveTarget(const vec3& position) {
	deb_assert(is_dragging_);
	move_target_ = position - drag_offset_;
}

vec3 Piece::GetMoveTarget() const {
	return move_target_ + drag_offset_;
}



}
