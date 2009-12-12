
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games

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



namespace TBC
{



class Model
{
public:
	Model();
	virtual ~Model();

	void RemoveAll();

	// Bones and animations all come with the animator.
	void AddAnimator(const Lepra::String& pName, BoneAnimator* pBoneAnimator);
	BoneAnimator* GetAnimator(const Lepra::String& pName);

	void StartAnimation(const Lepra::String& pAnimName, float pBlendingTime, BoneAnimation::Mode pMode = BoneAnimation::MODE_PLAY_DEFAULT);

	// Updates animations.
	virtual void Update(double pDeltaTime);

	// Sets and gets the transformation of the entire model.
	void SetTransformation(const Lepra::TransformationF& pTransformation);
	const Lepra::TransformationF& GetTransformation() const;

protected:

	// TODO: Implement the following somewhere closer to the physical
	// elements of the Structure.
/*
	class PhysicsFrame
	{
	public:
		PhysicsFrame() {}
		PhysicsFrame(const Lepra::TransformationF& pTransformation,
			     const Lepra::Vector3DF& pVelocity,
			     const Lepra::Vector3DF& pRotationalVelocity,
			     const Lepra::Vector3DF& pAcceleration,
			     const Lepra::Vector3DF& pRotationalAcceleration,
			     int pFrame) :
			mTransformation(pTransformation),
			mVelocity(pVelocity),
			mRotationalVelocity(pRotationalVelocity),
			mAcceleration(pAcceleration),
			mRotationalAcceleration(pRotationalAcceleration),
			mFrame(pFrame)
		{	
		}

		Lepra::TransformationF mTransformation;

		Lepra::Vector3DF mVelocity;
		Lepra::Vector3DF mRotationalVelocity;

		Lepra::Vector3DF mAcceleration;
		Lepra::Vector3DF mRotationalAcceleration;

		int mFrame;
	};

	// Cyclic array of frames.
	PhysFrame* mPhysFrame;
	int mNumPhysFrames;
	int mFirstPhysFrame;
*/
	typedef Lepra::HashTable<Lepra::String, BoneAnimator*, std::hash<Lepra::String> , 16> AnimatorTable;

	AnimatorTable mAnimatorTable;

	// This is the transformation of the entire model. Transform animations
	// are relative to this.
	Lepra::TransformationF mTransformation;
};



} // End namespace.



#endif // !MODEL_H
