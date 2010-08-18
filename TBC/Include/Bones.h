
// Author: Alexander Hugestrand, Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include <list>
#include <vector>
#include "../../Lepra/Include/HashTable.h"
#include "../../Lepra/Include/String.h"
#include "../../Lepra/Include/Transformation.h"
#include "TBC.h"



namespace Lepra
{
class CubicSpline;
}



namespace TBC
{



class PhysicsManager;



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
	void AddChild(int pIndexValue);
	int GetChild(int pChildIndex);

	std::vector<int> mChildIndex;
};



// A full skeleton.
class BoneHierarchy
{
public:
	enum TransformOperation
	{
		TRANSFORM_NONE = 1,
		TRANSFORM_LOCAL2WORLD,
		TRANSFORM_WORLD2LOCAL,
	};

	BoneHierarchy();
	virtual ~BoneHierarchy();

	virtual void ClearAll(PhysicsManager* pPhysics);

	// Use these functions to setup the skeleton. Preferably in the given order.
	virtual void SetBoneCount(int pBoneCount);
	int GetBoneCount() const;
	//void SetRootBone(int pRootBoneIndex);
	int GetRootBone() const;
	void SetBoneChildCount(int pBoneIndex, int pChildCount);
	int GetBoneChildCount(int pBoneIndex) const;
	void SetChildIndex(int pParentBoneIndex, int pParentChildIndex, int pChildBoneIndex);
	void AddChild(int pParentBoneIndex, int pChildBoneIndex);
	int GetChildIndex(int pParentBoneIndex, int pParentChildIndex) const;
	void SetOriginalBoneTransformation(int pBoneIndex, const TransformationF& pTransformation, int pParentBoneIndex = -1);
	const TransformationF& GetOriginalBoneTransformation(int pBoneIndex) const;
	// Call this when the whole skeleton is complete.
	bool FinalizeInit(TransformOperation pTransformOperation);

	void Connect(BoneHierarchy* pParentBones, int pParentBoneIndex);

	// Returns the current local transform.
	const TransformationF& GetBoneTransformation(int pBoneIndex) const;
	TransformationF& GetBoneTransformation(int pBoneIndex);
	void SetBoneTransformation(int pBoneIndex, const TransformationF& pTransformation);
	// Returns the current transform relative to the root bone's origo,
	// which is in object space.
	const TransformationF& GetBoneObjectTransformation(int pBoneIndex) const;
	// Same as above, but relative to the original transform.
	const TransformationF& GetRelativeBoneTransformation(int pBoneIndex) const;

private:
	friend class BoneAnimator;

	void Transform(int pBoneIndex, TransformOperation pTransformOperation);
	void UpdateBonesObjectTransformation(int pBoneIndex, const TransformationF& pParentTransformation);

	int mBoneCount;
	int mRootBoneIndex;
	Bone* mBone;
	BoneHierarchy* mParent;
	int mParentBoneIndex;

	TransformationF* mOriginalBoneTransformation;
	TransformationF* mRelativeBoneTransformation;
	TransformationF* mCurrentBoneTransformation;
	TransformationF* mCurrentBoneObjectTransformation;
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
	void SetRootNodeName(const str& pRootNodeName);
	const str& GetRootNodeName() const;
	void SetKeyframeCount(int pKeyframeCount, bool pUseSplines);
	int GetKeyframeCount() const;
	bool GetUseSplines() const;
	void SetBoneCount(int pBoneCount);
	int GetBoneCount() const;
	void SetTimeTag(int pKeyframe, float pTimeTag);
	float GetTimeTag(int pKeyframe) const;
	void SetBoneTransformation(int pKeyframe, int pBoneIndex, const TransformationF& pTransformation);
	const TransformationF& GetBoneTransformation(int pKeyframe, int pBoneIndex) const;

	// pTransformation points to an array of transformations. One transformation per bone.
	void GetAnimationState(TransformationF* pTransformation, float pTime, Mode pMode);
	void GetAnimationStateInterpolated(TransformationF* pTransformation, float pTime, Mode pMode, float pFactor);

private:
	Mode mDefaultMode;
	str mRootNodeName;
	int mKeyframeCount;
	int mBoneCount;

	// mTransformation[KeyframeCount][BoneCount]
	// mSpline[BoneCount]
	TransformationF** mTransformation;
	CubicSpline* mSpline;
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

	void AddAnimation(const str& pName, BoneAnimation* pAnimation);

	void StartAnimation(const str& pAnimationName, float pBlendingTime, BoneAnimation::Mode pMode = BoneAnimation::MODE_PLAY_DEFAULT);

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

	typedef HashTable<str, BoneAnimation*, std::hash<str>, 16> AnimTable;
	typedef std::list<AnimData> AnimList;

	BoneHierarchy* mBones;
	AnimTable mAnimTable;

	AnimList mCurrentlyPlayingAnims;
};



}
