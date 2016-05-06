
// Author: Jonas Byström, Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../lepra/include/cubicspline.h"
#include "../include/bones.h"
#include "../../lepra/include/lepraassert.h"


namespace tbc {



Bone::Bone() {
}

Bone::Bone(int child_count) {
	SetChildCount(child_count);
}

Bone::~Bone() {
	child_index_.clear();
}


void Bone::SetChildCount(int child_count) {
	child_index_.resize(child_count);
}

int Bone::GetChildCount() {
	return ((int)child_index_.size());
}

void Bone::SetChild(int child_index, int index_value) {
	deb_assert(child_index >= 0 && child_index < GetChildCount());
	child_index_[child_index] = index_value;
}

void Bone::AddChild(int index_value) {
	child_index_.push_back(index_value);
}

int Bone::GetChild(int child_index) {
	deb_assert(child_index >= 0 && child_index < GetChildCount());
	return (child_index_[child_index]);
}



BoneHierarchy::BoneHierarchy() :
	bone_count_(0),
	root_bone_index_(-1),
	bone_(0),
	parent_(0),
	parent_bone_index_(0),
	original_bone_transformation_(0),
	relative_bone_transformation_(0),
	current_bone_transformation_(0),
	current_bone_object_transformation_(0) {
}

BoneHierarchy::BoneHierarchy(const BoneHierarchy& original):
	bone_count_(0),
	root_bone_index_(original.root_bone_index_),
	parent_(0),
	parent_bone_index_(0) {
	SetBoneCount(original.bone_count_);
	for (int x = 0; x < bone_count_; ++x) {
		bone_[x] = original.bone_[x];
	}
	::memcpy(original_bone_transformation_, original.original_bone_transformation_, sizeof(xform)*bone_count_);
	::memcpy(relative_bone_transformation_, original.relative_bone_transformation_, sizeof(xform)*bone_count_);
	::memcpy(current_bone_transformation_, original.current_bone_transformation_, sizeof(xform)*bone_count_);
	::memcpy(current_bone_object_transformation_, original.current_bone_object_transformation_, sizeof(xform)*bone_count_);
}

BoneHierarchy::~BoneHierarchy() {
	ClearAll(0);
}

void BoneHierarchy::ClearAll(PhysicsManager*) {
	delete[] (bone_);
	bone_ = 0;
	delete[] (original_bone_transformation_);
	original_bone_transformation_ = 0;
	delete[] (relative_bone_transformation_);
	relative_bone_transformation_ = 0;
	delete[] (current_bone_transformation_);
	current_bone_transformation_ = 0;
	delete[] (current_bone_object_transformation_);
	current_bone_object_transformation_ = 0;

	parent_ = 0;
	parent_bone_index_ = 0;
}

void BoneHierarchy::SetBoneCount(int bone_count) {
	deb_assert(bone_count_ == 0);

	bone_count_ = bone_count;

	bone_ = new Bone[bone_count_];
	original_bone_transformation_       = new xform[bone_count_];
	relative_bone_transformation_       = new xform[bone_count_];
	current_bone_transformation_        = new xform[bone_count_];
	current_bone_object_transformation_  = new xform[bone_count_];
}

int BoneHierarchy::GetBoneCount() const {
	return (bone_count_);
}

/*void BoneHierarchy::SetRootBone(int pRootBoneIndex) {
	deb_assert(pRootBoneIndex < bone_count_);
	root_bone_index_ = pRootBoneIndex;
}*/

int BoneHierarchy::GetRootBone() const {
	deb_assert(root_bone_index_ == 0);
	return (root_bone_index_);
}

void BoneHierarchy::SetBoneChildCount(int bone_index, int child_count) {
	deb_assert(bone_index >= 0 && bone_index < bone_count_);
	bone_[bone_index].SetChildCount(child_count);
}

int BoneHierarchy::GetBoneChildCount(int bone_index) const {
	deb_assert(bone_index >= 0 && bone_index < bone_count_);
	return (bone_[bone_index].GetChildCount());
}

void BoneHierarchy::SetChildIndex(int parent_bone_index, int parent_child_index, int child_bone_index) {
	deb_assert(parent_bone_index >= 0 && parent_bone_index < bone_count_);
	deb_assert(child_bone_index >= 0 && child_bone_index < bone_count_);
	bone_[parent_bone_index].SetChild(parent_child_index, child_bone_index);
}

void BoneHierarchy::AddChild(int parent_bone_index, int child_bone_index) {
	deb_assert(parent_bone_index >= 0 && parent_bone_index < bone_count_);
	deb_assert(child_bone_index >= 0 && child_bone_index < bone_count_);
	bone_[parent_bone_index].AddChild(child_bone_index);
}

int BoneHierarchy::GetChildIndex(int parent_bone_index, int parent_child_index) const {
	deb_assert(parent_bone_index < bone_count_);
	return (bone_[parent_bone_index].GetChild(parent_child_index));
}

void BoneHierarchy::SetOriginalBoneTransformation(int bone_index, const xform& transformation, int parent_bone_index) {
	deb_assert(bone_index >= 0 && bone_index < bone_count_);
	original_bone_transformation_[bone_index] = transformation;
	if (parent_bone_index >= 0) {
		AddChild(parent_bone_index, bone_index);
	}
}

const xform& BoneHierarchy::GetOriginalBoneTransformation(int bone_index) const {
	deb_assert(bone_index >= 0 && bone_index < bone_count_);
	return (original_bone_transformation_[bone_index]);
}

bool BoneHierarchy::FinalizeInit(TransformOperation transform_operation) {
	deb_assert(bone_count_ > 0);
	root_bone_index_ = 0;
	for (int i = 0; i < bone_count_; i++) {
		current_bone_transformation_[i] = original_bone_transformation_[i];
	}
	Transform(root_bone_index_, transform_operation);
	return (true);
}



void BoneHierarchy::Connect(BoneHierarchy* parent_bones, int parent_bone_index) {
	parent_ = parent_bones;
	parent_bone_index_ = parent_bone_index;
}



const xform& BoneHierarchy::GetBoneTransformation(int bone_index) const {
	deb_assert(bone_index >= 0 && bone_index < bone_count_);
	return current_bone_transformation_[bone_index];
}

xform& BoneHierarchy::GetBoneTransformation(int bone_index) {
	deb_assert(bone_index >= 0 && bone_index < bone_count_);
	return current_bone_transformation_[bone_index];
}

void BoneHierarchy::SetBoneTransformation(int bone_index, const xform& transformation) {
	deb_assert(bone_index >= 0 && bone_index < bone_count_);
	current_bone_transformation_[bone_index] = transformation;
}

const xform& BoneHierarchy::GetBoneObjectTransformation(int bone_index) const {
	deb_assert(bone_index >= 0 && bone_index < bone_count_);
	return current_bone_object_transformation_[bone_index];
}

const xform& BoneHierarchy::GetRelativeBoneTransformation(int bone_index) const {
	deb_assert(bone_index >= 0 && bone_index < bone_count_);
	return relative_bone_transformation_[bone_index];
}

xform& BoneHierarchy::GetRelativeBoneTransformation(int bone_index) {
	deb_assert(bone_index >= 0 && bone_index < bone_count_);
	return relative_bone_transformation_[bone_index];
}


void BoneHierarchy::UpdateBonesObjectTransformation(int bone_index, const xform& parent_transformation) {
	deb_assert(bone_index >= 0 && bone_index < bone_count_);

	current_bone_object_transformation_[bone_index] = parent_transformation * current_bone_transformation_[bone_index];
	xform org_obj_transf(parent_transformation * original_bone_transformation_[bone_index]);

	relative_bone_transformation_[bone_index] = parent_transformation * (org_obj_transf / current_bone_object_transformation_[bone_index]);

	// Update children recursively.
	for (int i = 0; i < bone_[bone_index].GetChildCount(); i++) {
		UpdateBonesObjectTransformation(bone_[bone_index].GetChild(i), current_bone_object_transformation_[bone_index]);
	}
}



void BoneHierarchy::Transform(int bone_index, TransformOperation transform_operation) {
	deb_assert(bone_index >= 0 && bone_index < bone_count_);
	for (int i = 0; i < bone_[bone_index].GetChildCount(); i++) {
		const int _child_index = bone_[bone_index].GetChild(i);
		deb_assert(_child_index >= 0 && _child_index < bone_count_);
		deb_assert(_child_index != bone_index);

		if (transform_operation == kTransformLocal2World) {
			const xform& this_transform = current_bone_transformation_[bone_index];
			xform& child_transform = current_bone_transformation_[_child_index];
			child_transform = this_transform.Transform(child_transform);
		}

		Transform(_child_index, transform_operation);

		if (transform_operation == kTransformWorld2Local) {
			const xform& this_transform = current_bone_transformation_[bone_index];
			xform& child_transform = current_bone_transformation_[_child_index];
			child_transform = this_transform.InverseTransform(child_transform);
		}
	}
}



BoneAnimation::BoneAnimation():
	default_mode_(kModePlayOnce),
	keyframe_count_(0),
	bone_count_(0),
	transformation_(0),
	spline_(0),
	use_splines_(false),
	time_tag_(0),
	extra_time_tag_(0) {
}

BoneAnimation::~BoneAnimation() {
	if (transformation_ != 0) {
		for (int i = 0; i < keyframe_count_; i++) {
			delete[] transformation_[i];
		}
		delete[] transformation_;
		transformation_ = 0;
	}

	if (spline_ != 0) {
		delete[] spline_;
		spline_ = 0;
	}

	if (time_tag_ != 0) {
		delete[] time_tag_;
		time_tag_ = 0;
	}
}

void BoneAnimation::SetDefaultMode(Mode default_mode) {
	default_mode_ = default_mode;
}

BoneAnimation::Mode BoneAnimation::GetDefaultMode() const {
	return (default_mode_);
}

void BoneAnimation::SetRootNodeName(const str& root_node_name) {
	root_node_name_ = root_node_name;
}

const str& BoneAnimation::GetRootNodeName() const {
	return (root_node_name_);
}

void BoneAnimation::SetKeyframeCount(int keyframe_count, bool use_splines) {
	if (transformation_ != 0) {
		for (int i = 0; i < keyframe_count_; i++) {
			if (transformation_[i] != 0)
				delete[] transformation_[i];
		}
		delete[] transformation_;
		transformation_ = 0;
	}

	if (spline_ != 0) {
		delete[] spline_;
		spline_ = 0;
	}

	if (time_tag_ != 0) {
		delete[] time_tag_;
	}

	use_splines_ = use_splines;

	keyframe_count_ = keyframe_count;

	transformation_ = new xform*[keyframe_count_];
	time_tag_ = new float[keyframe_count_];

	for (int i = 0; i < keyframe_count_; i++) {
		transformation_[i] = 0;
		time_tag_[i] = 0.0f;
	}

	bone_count_ = 0;
}

int BoneAnimation::GetKeyframeCount() const {
	return (keyframe_count_);
}

bool BoneAnimation::GetUseSplines() const {
	return (use_splines_);
}

void BoneAnimation::SetBoneCount(int bone_count) {
	int i;
	for (i = 0; i < keyframe_count_; i++) {
		if (transformation_[i] != 0) {
			delete[] transformation_[i];
			transformation_[i] = 0;
		}

		bone_count_ = bone_count;
		transformation_[i] = new xform[bone_count_];
	}

	if (spline_ != 0) {
		delete[] spline_;
		spline_ = 0;
	}

	if (use_splines_ == true) {
		spline_ = new CubicSpline[bone_count_];
		for (i = 0; i < bone_count_; i++) {
			spline_[i].Init(keyframe_count_, 3);
		}
	}
}

int BoneAnimation::GetBoneCount() const {
	return (bone_count_);
}

void BoneAnimation::SetTimeTag(int keyframe, float time_tag) {
	if (keyframe < keyframe_count_) {
		time_tag_[keyframe] = time_tag;
	} else {
		extra_time_tag_ = time_tag;
	}
}

float BoneAnimation::GetTimeTag(int keyframe) const {
	float _time_tag;
	if (keyframe < keyframe_count_) {
		_time_tag = time_tag_[keyframe];
	} else {
		_time_tag = extra_time_tag_;
	}
	return (_time_tag);
}

void BoneAnimation::SetBoneTransformation(int keyframe, int bone_index, const xform& transformation) {
	transformation_[keyframe][bone_index] = transformation;

	if (use_splines_ == true) {
		const vec3& position = transformation.GetPosition();
		float value[3];
		value[0] = position.x;
		value[1] = position.y;
		value[2] = position.z;
		spline_[bone_index].SetPointValues(keyframe, time_tag_[keyframe], value);
	}
}

const xform& BoneAnimation::GetBoneTransformation(int keyframe, int bone_index) const {
	return (transformation_[keyframe][bone_index]);
}

void BoneAnimation::GetAnimationState(xform* transformation, float time, Mode mode) {
	GetAnimationStateInterpolated(transformation, time, mode, 1);
}

void BoneAnimation::GetAnimationStateInterpolated(xform* transformation, float time, Mode mode, float factor) {
	if (factor <= 0) {
		// Nothing to do.
		return;
	}

	if (mode == kModePlayDefault) {
		mode = default_mode_;
	}

	// Step to current frame.
	int current_keyframe = 0;
	if (mode == kModePlayLoop) {
		while (current_keyframe < (keyframe_count_ - 1) &&
			  time_tag_[current_keyframe + 1] < time) {
			current_keyframe++;
		}
	} else {
		while (current_keyframe < (keyframe_count_ - 2) &&
			  time_tag_[current_keyframe + 1] < time) {
			current_keyframe++;
		}
	}

	// Find time tags.
	float current_time_tag = time_tag_[current_keyframe];
	float next_time_tag = time_tag_[current_keyframe + 1];
	float last_time_tag = time_tag_[keyframe_count_ - 1];
	if (mode == kModePlayLoop) {
		if (current_keyframe == keyframe_count_ - 1)
			next_time_tag = extra_time_tag_;
		last_time_tag = extra_time_tag_;
	}

	// Check time constraints.
	if (time > last_time_tag) {
		if(mode == kModePlayLoop) {
			float rest_time = time - last_time_tag * floor(time / last_time_tag);
			GetAnimationStateInterpolated(transformation, rest_time, mode, factor);
			return;
		} else {
			// We can't step further than the last keyframe.
			time = last_time_tag;
		}
	}

	// Init splines.
	if (use_splines_ == true) {
		if (mode == kModePlayLoop) {
			for (int i = 0; i < bone_count_; i++) {
				spline_[i].StartLoopedInterpolation(0.0f, extra_time_tag_);
				spline_[i].StepInterpolation(time);
			}
		} else {
			for (int i = 0; i < bone_count_; i++) {
				spline_[i].StartInterpolation(0.0f);
				spline_[i].StepInterpolation(time);
			}
		}
	}

	int next_keyframe = (current_keyframe + 1) % keyframe_count_;
	xform* current_transformation = transformation_[current_keyframe];
	xform* next_transformation = transformation_[next_keyframe];
	float frame_factor = (time - current_time_tag) / (next_time_tag - current_time_tag);

	if (use_splines_ == false) {
		// Linear interpolation (no splines).
		for (int i = 0; i < bone_count_; i++) {
			xform final_transform;
			final_transform.Interpolate(current_transformation[i],
						     next_transformation[i],
						     frame_factor);

			if (factor < 1) {
				transformation[i].Interpolate(transformation[i], final_transform, factor);
			} else {
				transformation[i] = final_transform;
			}
		}
	} else {
		// Splines.
		int next_next_keyframe = (next_keyframe + 1) % keyframe_count_;
		int prev_keyframe = (current_keyframe + keyframe_count_ - 1) % keyframe_count_;

		xform* next_next_transformation = transformation_[next_next_keyframe];
		xform* prev_transformation = transformation_[prev_keyframe];

		for (int i = 0; i < bone_count_; i++) {
			vec3 position(spline_[i].GetValue(0),
						spline_[i].GetValue(1),
						spline_[i].GetValue(2));

			xform final_transform;
			final_transform.SetPosition(position);



			float t = spline_[i].GetCurrentSegmentTime();

			// We have 4 frames, numbered from 1 to 4, where the current frame
			// is number 2. Using these, we are doing a Catmull-Rom spline.
			// TODO: Test this with uneven time tags.
			quat q_temp12;
			quat q_temp23;
			quat q_temp34;

			q_temp12.Slerp(prev_transformation[i].GetOrientation(),
					current_transformation[i].GetOrientation(), t + 1);
			q_temp23.Slerp(current_transformation[i].GetOrientation(),
					next_transformation[i].GetOrientation(), t);
			q_temp34.Slerp(next_transformation[i].GetOrientation(),
					next_next_transformation[i].GetOrientation(), t - 1);

			quat q_temp123;
			quat q_temp234;

			q_temp123.Slerp(q_temp12, q_temp23, (t + 1) / 2.0f);
			q_temp234.Slerp(q_temp23, q_temp34, t / 2.0f);

			// And finally...
			quat rot;
			rot.Slerp(q_temp123, q_temp234, t);

			final_transform.SetOrientation(rot);

			if (factor < 1) {
				transformation[i].Interpolate(transformation[i], final_transform, factor);
			} else {
				transformation[i] = final_transform;
			}
		}
	}
}






BoneAnimator::BoneAnimator(BoneHierarchy* bones) :
	bones_(bones) {
}

BoneAnimator::~BoneAnimator() {
	delete bones_;
	bones_ = 0;
}



void BoneAnimator::AddAnimation(const str& name, BoneAnimation* animation) {
	anim_table_.Insert(name, animation);
}



void BoneAnimator::StartAnimation(const str& animation_name, float blending_time, BoneAnimation::Mode mode) {
	AnimTable::Iterator iter = anim_table_.Find(animation_name);

	if (iter != anim_table_.End()) {
		BoneAnimation* _animation = *iter;

		AnimData anim_data;
		anim_data.animation_ = _animation;
		anim_data.current_time_ = 0;
		anim_data.blending_time_ = blending_time;
		anim_data.mode_ = mode;

		currently_playing_anims_.push_back(anim_data);
	}
}



void BoneAnimator::Step(float step_time) {
	if (currently_playing_anims_.empty()) {
		return;
	}

	AnimList::iterator next_iter(currently_playing_anims_.end());
	AnimList::iterator iter(--currently_playing_anims_.end());

	// Increase time counters and check wether the animations are currently playing.
	for (; iter != currently_playing_anims_.end(); --iter) {
		(*iter).current_time_ += step_time;

		if (next_iter != currently_playing_anims_.end() &&
		   (*iter).current_time_ >= (*next_iter).blending_time_) {
			// Delete all animations previous to this.
			while (iter != currently_playing_anims_.end()) {
				AnimList::iterator y = iter;
				--iter;
				currently_playing_anims_.erase(y);
			}
		}

		next_iter = iter;
	}

	// The following requires access to the private members of 'BoneHierarchy'.
	if (currently_playing_anims_.size() == 1) {
		// Only one animation is running. Just update the bones.

		AnimData& anim_data = currently_playing_anims_.front();
		anim_data.animation_->GetAnimationState(bones_->current_bone_transformation_, anim_data.current_time_, anim_data.mode_);

		if (bones_->parent_ == 0) {
			bones_->UpdateBonesObjectTransformation(bones_->root_bone_index_, kIdentityTransformationF);
		} else {
			bones_->UpdateBonesObjectTransformation(bones_->root_bone_index_, bones_->parent_->GetBoneObjectTransformation(bones_->parent_bone_index_));
		}
	} else {
		// Several animations should be blended. Start by just copying the first animation.

		iter = currently_playing_anims_.begin();
		AnimData& first  = *iter;
		first.animation_->GetAnimationState(bones_->current_bone_transformation_, first.current_time_, first.mode_);

		// Step to next animation.
		++iter;
		for (; iter != currently_playing_anims_.end(); ++iter) {
			AnimData& second = *iter;

			// The blending time can't be zero due to the check that is performed first in this function.
			float _factor = second.current_time_ / second.blending_time_;
			if (_factor > 1)
				_factor = 1;

			second.animation_->GetAnimationStateInterpolated(bones_->current_bone_transformation_, second.current_time_, second.mode_, _factor);

			first = second;
		}
	}
}



BoneHierarchy* BoneAnimator::GetBones() {
	return (bones_);
}



}
