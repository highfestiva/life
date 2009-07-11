
// Author: Alexander Hugestrand, Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../../Lepra/Include/HashTable.h"
#include "../../Lepra/Include/String.h"
#include "../../Lepra/Include/Transformation.h"
#include "TBC.h"
#include <list>



namespace Lepra
{
class CubicSpline;
}



namespace TBC
{



class PhysicsEngine;



// One bone...
class Bone
{
public:
	friend class BoneHierarchy;

private:
	Bone();
	Bone(int pChildCount);
	~Bone();

	void SetChildCount(int pChildCount);
	int GetChildCount();
	void SetChild(int pChildIndex, int pIndexValue);
	int GetChild(int pChildIndex);

	int mChildCount;
	int* mChildIndex;
};



// A full skeleton.
class BoneHierarchy
{
public:
	friend class BoneAnimator;

	BoneHierarchy();
	virtual ~BoneHierarchy();

	virtual void ClearAll(PhysicsEngine* pPhysics);

	// Use these functions to setup the skeleton. Preferably in the given order.
	virtual void SetBoneCount(int pBoneCount);
	int GetBoneCount() const;
	//void SetRootBone(int pRootBoneIndex);
	int GetRootBone() const;
	void SetBoneChildCount(int pBoneIndex, int pChildCount);
	int GetBoneChildCount(int pBoneIndex) const;
	void SetChildIndex(int pParentBoneIndex, int pParentChildIndex, int pChildBoneIndex);
	int GetChildIndex(int pParentBoneIndex, int pParentChildIndex) const;
	void SetOriginalBoneTransformation(int pBoneIndex, const Lepra::TransformationF& pTransformation);
	const Lepra::TransformationF& GetOriginalBoneTransformation(int pBoneIndex) const;
	// Call this when the whole skeleton is complete.
	bool FinalizeInit();

	void Connect(BoneHierarchy* pParentBones, int pParentBoneIndex);

	// Returns the current local transform.
	const Lepra::TransformationF& GetBoneTransformation(int pBoneIndex) const;
	// Returns the current transform relative to the root bone's origo,
	// which is in object space.
	const Lepra::TransformationF& GetBoneObjectTransformation(int pBoneIndex) const;
	// Same as above, but relative to the original transform.
	const Lepra::TransformationF& GetRelativeBoneTransformation(int pBoneIndex) const;

private:
	void UpdateBonesObjectTransformation(int pBoneIndex, const Lepra::TransformationF& pParentTransformation);

	int mBoneCount;
	int mRootBoneIndex;
	Bone* mBone;
	BoneHierarchy* mParent;
	int mParentBoneIndex;

	Lepra::TransformationF* mOriginalBoneTransformation;
	Lepra::TransformationF* mRelativeBoneTransformation;
	Lepra::TransformationF* mCurrentBoneTransformation;
	Lepra::TransformationF* mCurrentBoneObjectTransformation;
};



// The animation itself. "Played" by the class BoneAnimator.
class BoneAnimation
{
public:
	enum Mode
	{
		MODE_PLAY_DEFAULT = 1,	// Only used by animator to not override the default animation behavior.
		MODE_PLAY_ONCE,
		MODE_PLAY_LOOP,
	};

	BoneAnimation();
	virtual ~BoneAnimation();

	// Functions used to setup the animation.
	void SetDefaultMode(Mode pDefaultMode);
	Mode GetDefaultMode() const;
	void SetRootNodeName(const Lepra::String& pRootNodeName);
	const Lepra::String& GetRootNodeName() const;
	void SetKeyframeCount(int pKeyframeCount, bool pUseSplines);
	int GetKeyframeCount() const;
	bool GetUseSplines() const;
	void SetBoneCount(int pBoneCount);
	int GetBoneCount() const;
	void SetTimeTag(int pKeyframe, float pTimeTag);
	float GetTimeTag(int pKeyframe) const;
	void SetBoneTransformation(int pKeyframe, int pBoneIndex, const Lepra::TransformationF& pTransformation);
	const Lepra::TransformationF& GetBoneTransformation(int pKeyframe, int pBoneIndex) const;

	// pTransformation points to an array of transformations. One transformation per bone.
	void GetAnimationState(Lepra::TransformationF* pTransformation, float pTime, Mode pMode);
	void GetAnimationStateInterpolated(Lepra::TransformationF* pTransformation, float pTime, Mode pMode, float pFactor);

private:
	Mode mDefaultMode;
	Lepra::String mRootNodeName;
	int mKeyframeCount;
	int mBoneCount;

	// mTransformation[KeyframeCount][BoneCount]
	// mSpline[BoneCount]
	Lepra::TransformationF** mTransformation;
	Lepra::CubicSpline* mSpline;
	bool mUseSplines;

	// Time tag for each key frame. The first one is always 0.
	float* mTimeTag;
	float mExtraTimeTag;
};



// "Plays" BonesAnimations.
class BoneAnimator
{
public:
	BoneAnimator(BoneHierarchy* pBones);
	~BoneAnimator();

	void AddAnimation(const Lepra::String& pName, BoneAnimation* pAnimation);

	void StartAnimation(const Lepra::String& pAnimationName, float pBlendingTime, BoneAnimation::Mode pMode = BoneAnimation::MODE_PLAY_DEFAULT);

	// Steps all animations.
	void Step(float pStepTime);

	BoneHierarchy* GetBones();

private:

	struct AnimData
	{
		BoneAnimation* mAnimation;
		float mCurrentTime;
		float mBlendingTime;
		BoneAnimation::Mode mMode;
	};

	typedef Lepra::HashTable<Lepra::String, BoneAnimation*, std::hash<Lepra::String>, 16> AnimTable;
	typedef std::list<AnimData> AnimList;

	BoneHierarchy* mBones;
	AnimTable mAnimTable;

	AnimList mCurrentlyPlayingAnims;
};



}
