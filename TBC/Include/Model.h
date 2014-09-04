
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

// NOTE:
// This class represents the skeleton of a 3D-model. It contain all
// bones, animations and collision volumes associated with the model.
//
// This information is enough on the server side, while the client
// needs graphical information as well (see GraphicalModel).



#ifndef MODEL_H
#define MODEL_H



#include "Bones.h"
#include <list>



namespace Tbc
{



class Model
{
public:
	Model();
	virtual ~Model();

	void RemoveAll();

	// Bones and animations all come with the animator.
	void AddAnimator(const str& pName, BoneAnimator* pBoneAnimator);
	BoneAnimator* GetAnimator(const str& pName);

	void StartAnimation(const str& pAnimName, float pBlendingTime, BoneAnimation::Mode pMode = BoneAnimation::MODE_PLAY_DEFAULT);

	// Updates animations.
	virtual void Update(double pDeltaTime);

	// Sets and gets the transformation of the entire model.
	void SetTransformation(const xform& pTransformation);
	const xform& GetTransformation() const;

protected:

	// TODO: Implement the following somewhere closer to the physical
	// elements of the Structure.
/*
	class PhysicsFrame
	{
	public:
		PhysicsFrame() {}
		PhysicsFrame(const xform& pTransformation,
			     const vec3& pVelocity,
			     const vec3& pRotationalVelocity,
			     const vec3& pAcceleration,
			     const vec3& pRotationalAcceleration,
			     int pFrame) :
			mTransformation(pTransformation),
			mVelocity(pVelocity),
			mRotationalVelocity(pRotationalVelocity),
			mAcceleration(pAcceleration),
			mRotationalAcceleration(pRotationalAcceleration),
			mFrame(pFrame)
		{	
		}

		xform mTransformation;

		vec3 mVelocity;
		vec3 mRotationalVelocity;

		vec3 mAcceleration;
		vec3 mRotationalAcceleration;

		int mFrame;
	};

	// Cyclic array of frames.
	PhysFrame* mPhysFrame;
	int mNumPhysFrames;
	int mFirstPhysFrame;
*/
	typedef HashTable<str, BoneAnimator*, std::hash<str> , 16> AnimatorTable;

	AnimatorTable mAnimatorTable;

	// This is the transformation of the entire model. Transform animations
	// are relative to this.
	xform mTransformation;
};



} // End namespace.



#endif // !MODEL_H
