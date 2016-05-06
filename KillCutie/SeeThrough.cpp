
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "seethrough.h"
#include "../cure/include/cppcontextobject.h"
#include "../cure/include/contextmanager.h"
#include "../lepra/include/collisiondetector3d.h"
#include "../tbc/include/physicstrigger.h"
#include "../uicure/include/uicppcontextobject.h"



namespace grenaderun {



SeeThrough::SeeThrough(cure::ContextManager* manager, const Game* game):
	Parent(manager->GetGameManager()->GetResourceManager(), "SeeThrough", game->GetUiManager()),
	game_(game),
	tag_(0),
	opacity_(1) {
	manager->AddLocalObject(this);
	GetManager()->EnableTickCallback(this);
}

SeeThrough::~SeeThrough() {
}



void SeeThrough::SetTagIndex(int index) {
	cure::CppContextObject* parent = (cure::CppContextObject*)parent_;
	const Tag* tag = &parent->GetClass()->GetTag(index);
	if (tag->body_index_list_.size() == 1 && tag->engine_index_list_.size() == 0 &&
		tag->float_value_list_.size() == 8 && tag->mesh_index_list_.size() == 1 &&
		tag->string_value_list_.size() == 0) {
		tag_ = tag;
	} else {
		log_.Error("Badly configured see-through tag!");
		deb_assert(false);
	}
}

void SeeThrough::OnTick() {
	deb_assert(tag_);
	if (!tag_) {
		return;
	}
	const cure::ContextObject* target = game_->GetP1();
	if (!target || !target->IsLoaded()) {
		return;
	}

	enum FloatValues {
		kMaxTransparency = 0,
		kMaxOpacity,
		kXo,
		kYo,
		kZo,
		kXs,
		kYs,
		kZs,
	};
	UiCure::CppContextObject* parent = (UiCure::CppContextObject*)parent_;
	tbc::ChunkyPhysics* physics = parent_->GetPhysics();
	const int bone_index = tag_->body_index_list_[0];
	const vec3 bone_position = physics->GetBoneTransformation(bone_index).GetPosition();
	const vec3 target_position = target->GetPosition();
	const vec3 half_size(tag_->float_value_list_[kXs]/2, tag_->float_value_list_[kYs]/2, tag_->float_value_list_[kZs]/2);
	AABB<float> aabb;
	aabb.SetPosition(bone_position + vec3(tag_->float_value_list_[kXo], tag_->float_value_list_[kYo], tag_->float_value_list_[kZo]));
	aabb.SetSize(half_size);
	if (CollisionDetector3D<float>::IsAABBEnclosingPoint(aabb, target_position)) {
		opacity_ = Math::Lerp(opacity_, tag_->float_value_list_[kMaxTransparency], 0.05f);
	} else {
		opacity_ = Math::Lerp(opacity_, tag_->float_value_list_[kMaxOpacity], 0.05f);
		if (opacity_ >= tag_->float_value_list_[kMaxOpacity]*0.95f) {
			opacity_ = tag_->float_value_list_[kMaxOpacity];
		}
	}

	UiCure::UserGeometryReferenceResource* mesh = parent->GetMeshResource(tag_->mesh_index_list_[0]);
	if (mesh && mesh->GetLoadState() == cure::kResourceLoadComplete) {
		const float alpha = mesh->GetRamData()->GetBasicMaterialSettings().alpha_;
		mesh->GetRamData()->GetBasicMaterialSettings().alpha_ = opacity_;
		if ((alpha == 1 || opacity_ == 1) && alpha != opacity_) {
			parent->EnablePixelShader(false);
			parent->UpdateMaterial(tag_->mesh_index_list_[0]);
		}
	}
}



loginstance(kGameContextCpp, SeeThrough);




}
