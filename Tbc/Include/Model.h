
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

// NOTE:
// This class represents the skeleton of a 3D-model. It contain all
// bones, animations and collision volumes associated with the model.
//
// This information is enough on the server side, while the client
// needs graphical information as well (see GraphicalModel).



#pragma once

#include "bones.h"
#include <list>



namespace tbc {



class Model {
public:
	Model();
	virtual ~Model();

	void RemoveAll();

	// Bones and animations all come with the animator.
	void AddAnimator(const str& name, BoneAnimator* bone_animator);
	BoneAnimator* GetAnimator(const str& name);

	void StartAnimation(const str& anim_name, float blending_time, BoneAnimation::Mode mode = BoneAnimation::kModePlayDefault);

	// Updates animations.
	virtual void Update(double delta_time);

	// Sets and gets the transformation of the entire model.
	void SetTransformation(const xform& transformation);
	const xform& GetTransformation() const;

protected:

	// TODO: Implement the following somewhere closer to the physical
	// elements of the Structure.
/*
	class PhysicsFrame {
	public:
		PhysicsFrame() {}
		PhysicsFrame(const xform& transformation,
			     const vec3& velocity,
			     const vec3& pRotationalVelocity,
			     const vec3& acceleration,
			     const vec3& pRotationalAcceleration,
			     int pFrame) :
			transformation_(transformation),
			velocity_(velocity),
			mRotationalVelocity(pRotationalVelocity),
			acceleration_(acceleration),
			mRotationalAcceleration(pRotationalAcceleration),
			frame_(pFrame) {
		}

		xform transformation_;

		vec3 velocity_;
		vec3 mRotationalVelocity;

		vec3 acceleration_;
		vec3 mRotationalAcceleration;

		int frame_;
	};

	// Cyclic array of frames.
	PhysFrame* mPhysFrame;
	int mNumPhysFrames;
	int mFirstPhysFrame;
*/
	typedef HashTable<str, BoneAnimator*, std::hash<str> , 16> AnimatorTable;

	AnimatorTable animator_table_;

	// This is the transformation of the entire model. Transform animations
	// are relative to this.
	xform transformation_;
};



}
