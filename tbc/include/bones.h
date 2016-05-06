
// Author: Jonas Byström, Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include <list>
#include <vector>
#include "../../lepra/include/hashtable.h"
#include "../../lepra/include/string.h"
#include "../../lepra/include/transformation.h"
#include "tbc.h"



namespace lepra {
class CubicSpline;
}



namespace tbc {



class PhysicsManager;



// One bone...
class Bone {
public:
	friend class BoneHierarchy;

private:
	Bone();
	Bone(int child_count);
	~Bone();

	void SetChildCount(int child_count);
	int GetChildCount();
	void SetChild(int child_index, int index_value);
	void AddChild(int index_value);
	int GetChild(int child_index);

	std::vector<int> child_index_;
};



// A full skeleton.
class BoneHierarchy {
public:
	enum TransformOperation {
		kTransformNone = 1,
		kTransformLocal2World,
		kTransformWorld2Local,
	};

	BoneHierarchy();
	BoneHierarchy(const BoneHierarchy& original);
	virtual ~BoneHierarchy();

	virtual void ClearAll(PhysicsManager* physics);

	// Use these functions to setup the skeleton. Preferably in the given order.
	virtual void SetBoneCount(int bone_count);
	int GetBoneCount() const;
	//void SetRootBone(int pRootBoneIndex);
	int GetRootBone() const;
	void SetBoneChildCount(int bone_index, int child_count);
	int GetBoneChildCount(int bone_index) const;
	void SetChildIndex(int parent_bone_index, int parent_child_index, int child_bone_index);
	void AddChild(int parent_bone_index, int child_bone_index);
	int GetChildIndex(int parent_bone_index, int parent_child_index) const;
	void SetOriginalBoneTransformation(int bone_index, const xform& transformation, int parent_bone_index = -1);
	const xform& GetOriginalBoneTransformation(int bone_index) const;
	// Call this when the whole skeleton is complete.
	bool FinalizeInit(TransformOperation transform_operation);

	void Connect(BoneHierarchy* parent_bones, int parent_bone_index);

	// Returns the current local transform.
	const xform& GetBoneTransformation(int bone_index) const;
	xform& GetBoneTransformation(int bone_index);
	void SetBoneTransformation(int bone_index, const xform& transformation);
	// Returns the current transform relative to the root bone's origo,
	// which is in object space.
	const xform& GetBoneObjectTransformation(int bone_index) const;
	// Same as above, but relative to the original transform.
	const xform& GetRelativeBoneTransformation(int bone_index) const;
	xform& GetRelativeBoneTransformation(int bone_index);

	void UpdateBonesObjectTransformation(int bone_index, const xform& parent_transformation);

private:
	friend class BoneAnimator;

	void Transform(int bone_index, TransformOperation transform_operation);

	int bone_count_;
	int root_bone_index_;
	Bone* bone_;
	BoneHierarchy* parent_;
	int parent_bone_index_;

	xform* original_bone_transformation_;
	xform* relative_bone_transformation_;
	xform* current_bone_transformation_;
	xform* current_bone_object_transformation_;
};



// The animation itself. "Played" by the class BoneAnimator.
class BoneAnimation {
public:
	enum Mode {
		kModePlayDefault = 1,	// Only used by animator to not override the default animation behavior.
		kModePlayOnce,
		kModePlayLoop,
	};

	BoneAnimation();
	virtual ~BoneAnimation();

	// Functions used to setup the animation.
	void SetDefaultMode(Mode default_mode);
	Mode GetDefaultMode() const;
	void SetRootNodeName(const str& root_node_name);
	const str& GetRootNodeName() const;
	void SetKeyframeCount(int keyframe_count, bool use_splines);
	int GetKeyframeCount() const;
	bool GetUseSplines() const;
	void SetBoneCount(int bone_count);
	int GetBoneCount() const;
	void SetTimeTag(int keyframe, float time_tag);
	float GetTimeTag(int keyframe) const;
	void SetBoneTransformation(int keyframe, int bone_index, const xform& transformation);
	const xform& GetBoneTransformation(int keyframe, int bone_index) const;

	// transformation points to an array of transformations. One transformation per bone.
	void GetAnimationState(xform* transformation, float time, Mode mode);
	void GetAnimationStateInterpolated(xform* transformation, float time, Mode mode, float factor);

private:
	Mode default_mode_;
	str root_node_name_;
	int keyframe_count_;
	int bone_count_;

	// transformation_[KeyframeCount][BoneCount]
	// spline_[BoneCount]
	xform** transformation_;
	CubicSpline* spline_;
	bool use_splines_;

	// Time tag for each key frame. The first one is always 0.
	float* time_tag_;
	float extra_time_tag_;
};



// "Plays" BonesAnimations.
class BoneAnimator {
public:
	BoneAnimator(BoneHierarchy* bones);
	~BoneAnimator();

	void AddAnimation(const str& name, BoneAnimation* animation);

	void StartAnimation(const str& animation_name, float blending_time, BoneAnimation::Mode mode = BoneAnimation::kModePlayDefault);

	// Steps all animations.
	void Step(float step_time);

	BoneHierarchy* GetBones();

private:

	struct AnimData {
		BoneAnimation* animation_;
		float current_time_;
		float blending_time_;
		BoneAnimation::Mode mode_;
	};

	typedef HashTable<str, BoneAnimation*, std::hash<str>, 16> AnimTable;
	typedef std::list<AnimData> AnimList;

	BoneHierarchy* bones_;
	AnimTable anim_table_;

	AnimList currently_playing_anims_;
};



}
