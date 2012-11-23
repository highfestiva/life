 
// Author: Alexander Hugestrand, Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../../Lepra/Include/CubicSpline.h"
#include "../Include/Bones.h"
#include <assert.h>


namespace TBC
{



Bone::Bone()
{
}

Bone::Bone(int pChildCount)
{
	SetChildCount(pChildCount);
}

Bone::~Bone()
{
	mChildIndex.clear();
}


void Bone::SetChildCount(int pChildCount)
{
	mChildIndex.resize(pChildCount);
}

int Bone::GetChildCount()
{
	return ((int)mChildIndex.size());
}

void Bone::SetChild(int pChildIndex, int pIndexValue)
{
	assert(pChildIndex >= 0 && pChildIndex < GetChildCount());
	mChildIndex[pChildIndex] = pIndexValue;
}

void Bone::AddChild(int pIndexValue)
{
	mChildIndex.push_back(pIndexValue);
}

int Bone::GetChild(int pChildIndex)
{
	assert(pChildIndex >= 0 && pChildIndex < GetChildCount());
	return (mChildIndex[pChildIndex]);
}



BoneHierarchy::BoneHierarchy() :
	mBoneCount(0),
	mRootBoneIndex(-1),
	mBone(0),
	mParent(0),
	mParentBoneIndex(0),
	mOriginalBoneTransformation(0),
	mRelativeBoneTransformation(0),
	mCurrentBoneTransformation(0),
	mCurrentBoneObjectTransformation(0)
{
}

BoneHierarchy::BoneHierarchy(const BoneHierarchy& pOriginal):
	mBoneCount(0),
	mRootBoneIndex(pOriginal.mRootBoneIndex),
	mParent(0),
	mParentBoneIndex(0)
{
	SetBoneCount(pOriginal.mBoneCount);
	for (int x = 0; x < mBoneCount; ++x)
	{
		mBone[x] = pOriginal.mBone[x];
	}
	::memcpy(mOriginalBoneTransformation, pOriginal.mOriginalBoneTransformation, sizeof(TransformationF)*mBoneCount);
	::memcpy(mRelativeBoneTransformation, pOriginal.mRelativeBoneTransformation, sizeof(TransformationF)*mBoneCount);
	::memcpy(mCurrentBoneTransformation, pOriginal.mCurrentBoneTransformation, sizeof(TransformationF)*mBoneCount);
	::memcpy(mCurrentBoneObjectTransformation, pOriginal.mCurrentBoneObjectTransformation, sizeof(TransformationF)*mBoneCount);
}

BoneHierarchy::~BoneHierarchy()
{
	ClearAll(0);
}

void BoneHierarchy::ClearAll(PhysicsManager*)
{
	delete[] (mBone);
	mBone = 0;
	delete[] (mOriginalBoneTransformation);
	mOriginalBoneTransformation = 0;
	delete[] (mRelativeBoneTransformation);
	mRelativeBoneTransformation = 0;
	delete[] (mCurrentBoneTransformation);
	mCurrentBoneTransformation = 0;
	delete[] (mCurrentBoneObjectTransformation);
	mCurrentBoneObjectTransformation = 0;

	mParent = 0;
	mParentBoneIndex = 0;
}

void BoneHierarchy::SetBoneCount(int pBoneCount)
{
	assert(mBoneCount == 0);

	mBoneCount = pBoneCount;

	mBone = new Bone[mBoneCount];
	mOriginalBoneTransformation       = new TransformationF[mBoneCount];
	mRelativeBoneTransformation       = new TransformationF[mBoneCount];
	mCurrentBoneTransformation        = new TransformationF[mBoneCount];
	mCurrentBoneObjectTransformation  = new TransformationF[mBoneCount];
}

int BoneHierarchy::GetBoneCount() const
{
	return (mBoneCount);
}

/*void BoneHierarchy::SetRootBone(int pRootBoneIndex)
{
	assert(pRootBoneIndex < mBoneCount);
	mRootBoneIndex = pRootBoneIndex;
}*/

int BoneHierarchy::GetRootBone() const
{
	assert(mRootBoneIndex == 0);
	return (mRootBoneIndex);
}

void BoneHierarchy::SetBoneChildCount(int pBoneIndex, int pChildCount)
{
	assert(pBoneIndex >= 0 && pBoneIndex < mBoneCount);
	mBone[pBoneIndex].SetChildCount(pChildCount);
}

int BoneHierarchy::GetBoneChildCount(int pBoneIndex) const
{
	assert(pBoneIndex >= 0 && pBoneIndex < mBoneCount);
	return (mBone[pBoneIndex].GetChildCount());
}

void BoneHierarchy::SetChildIndex(int pParentBoneIndex, int pParentChildIndex, int pChildBoneIndex)
{
	assert(pParentBoneIndex >= 0 && pParentBoneIndex < mBoneCount);
	assert(pChildBoneIndex >= 0 && pChildBoneIndex < mBoneCount);
	mBone[pParentBoneIndex].SetChild(pParentChildIndex, pChildBoneIndex);
}

void BoneHierarchy::AddChild(int pParentBoneIndex, int pChildBoneIndex)
{
	assert(pParentBoneIndex >= 0 && pParentBoneIndex < mBoneCount);
	assert(pChildBoneIndex >= 0 && pChildBoneIndex < mBoneCount);
	mBone[pParentBoneIndex].AddChild(pChildBoneIndex);
}

int BoneHierarchy::GetChildIndex(int pParentBoneIndex, int pParentChildIndex) const
{
	assert(pParentBoneIndex < mBoneCount);
	return (mBone[pParentBoneIndex].GetChild(pParentChildIndex));
}

void BoneHierarchy::SetOriginalBoneTransformation(int pBoneIndex, const TransformationF& pTransformation, int pParentBoneIndex)
{
	assert(pBoneIndex >= 0 && pBoneIndex < mBoneCount);
	mOriginalBoneTransformation[pBoneIndex] = pTransformation;
	if (pParentBoneIndex >= 0)
	{
		AddChild(pParentBoneIndex, pBoneIndex);
	}
}

const TransformationF& BoneHierarchy::GetOriginalBoneTransformation(int pBoneIndex) const
{
	assert(pBoneIndex >= 0 && pBoneIndex < mBoneCount);
	return (mOriginalBoneTransformation[pBoneIndex]);
}

bool BoneHierarchy::FinalizeInit(TransformOperation pTransformOperation)
{
	assert(mBoneCount > 0);
	mRootBoneIndex = 0;
	for (int i = 0; i < mBoneCount; i++)
	{
		mCurrentBoneTransformation[i] = mOriginalBoneTransformation[i];
	}
	Transform(mRootBoneIndex, pTransformOperation);
	return (true);
}



void BoneHierarchy::Connect(BoneHierarchy* pParentBones, int pParentBoneIndex)
{
	mParent = pParentBones;
	mParentBoneIndex = pParentBoneIndex;
}



const TransformationF& BoneHierarchy::GetBoneTransformation(int pBoneIndex) const
{
	assert(pBoneIndex >= 0 && pBoneIndex < mBoneCount);
	return mCurrentBoneTransformation[pBoneIndex];
}

TransformationF& BoneHierarchy::GetBoneTransformation(int pBoneIndex)
{
	assert(pBoneIndex >= 0 && pBoneIndex < mBoneCount);
	return mCurrentBoneTransformation[pBoneIndex];
}

void BoneHierarchy::SetBoneTransformation(int pBoneIndex, const TransformationF& pTransformation)
{
	assert(pBoneIndex >= 0 && pBoneIndex < mBoneCount);
	mCurrentBoneTransformation[pBoneIndex] = pTransformation;
}

const TransformationF& BoneHierarchy::GetBoneObjectTransformation(int pBoneIndex) const
{
	assert(pBoneIndex >= 0 && pBoneIndex < mBoneCount);
	return mCurrentBoneObjectTransformation[pBoneIndex];
}

const TransformationF& BoneHierarchy::GetRelativeBoneTransformation(int pBoneIndex) const
{
	assert(pBoneIndex >= 0 && pBoneIndex < mBoneCount);
	return mRelativeBoneTransformation[pBoneIndex];
}



void BoneHierarchy::Transform(int pBoneIndex, TransformOperation pTransformOperation)
{
	assert(pBoneIndex >= 0 && pBoneIndex < mBoneCount);
	for (int i = 0; i < mBone[pBoneIndex].GetChildCount(); i++)
	{
		const int lChildIndex = mBone[pBoneIndex].GetChild(i);
		assert(lChildIndex >= 0 && lChildIndex < mBoneCount);
		assert(lChildIndex != pBoneIndex);

		if (pTransformOperation == TRANSFORM_LOCAL2WORLD)
		{
			const TransformationF& lThisTransform = mCurrentBoneTransformation[pBoneIndex];
			TransformationF& lChildTransform = mCurrentBoneTransformation[lChildIndex];
			lChildTransform = lThisTransform*lChildTransform;
		}

		Transform(lChildIndex, pTransformOperation);

		if (pTransformOperation == TRANSFORM_WORLD2LOCAL)
		{
			const TransformationF& lThisTransform = mCurrentBoneTransformation[pBoneIndex];
			TransformationF& lChildTransform = mCurrentBoneTransformation[lChildIndex];
			lChildTransform = lThisTransform.InverseTransform(lChildTransform);
		}
	}
}

void BoneHierarchy::UpdateBonesObjectTransformation(int pBoneIndex, const TransformationF& pParentTransformation)
{
	assert(pBoneIndex >= 0 && pBoneIndex < mBoneCount);

	mCurrentBoneObjectTransformation[pBoneIndex] = pParentTransformation * mCurrentBoneTransformation[pBoneIndex];
	TransformationF lOrgObjTransf(pParentTransformation * mOriginalBoneTransformation[pBoneIndex]);

	mRelativeBoneTransformation[pBoneIndex] = pParentTransformation * (lOrgObjTransf / mCurrentBoneObjectTransformation[pBoneIndex]);

	// Update children recursively.
	for (int i = 0; i < mBone[pBoneIndex].GetChildCount(); i++)
	{
		UpdateBonesObjectTransformation(mBone[pBoneIndex].GetChild(i), mCurrentBoneObjectTransformation[pBoneIndex]);
	}
}



BoneAnimation::BoneAnimation():
	mDefaultMode(MODE_PLAY_ONCE),
	mKeyframeCount(0),
	mBoneCount(0),
	mTransformation(0),
	mSpline(0),
	mUseSplines(false),
	mTimeTag(0),
	mExtraTimeTag(0)
{
}

BoneAnimation::~BoneAnimation()
{
	if (mTransformation != 0)
	{
		for (int i = 0; i < mKeyframeCount; i++)
		{
			delete[] mTransformation[i];
		}
		delete[] mTransformation;
		mTransformation = 0;
	}

	if (mSpline != 0)
	{
		delete[] mSpline;
		mSpline = 0;
	}

	if (mTimeTag != 0)
	{
		delete[] mTimeTag;
		mTimeTag = 0;
	}
}

void BoneAnimation::SetDefaultMode(Mode pDefaultMode)
{
	mDefaultMode = pDefaultMode;
}

BoneAnimation::Mode BoneAnimation::GetDefaultMode() const
{
	return (mDefaultMode);
}

void BoneAnimation::SetRootNodeName(const str& pRootNodeName)
{
	mRootNodeName = pRootNodeName;
}

const str& BoneAnimation::GetRootNodeName() const
{
	return (mRootNodeName);
}

void BoneAnimation::SetKeyframeCount(int pKeyframeCount, bool pUseSplines)
{
	if (mTransformation != 0)
	{
		for (int i = 0; i < mKeyframeCount; i++)
		{
			if (mTransformation[i] != 0)
				delete[] mTransformation[i];
		}
		delete[] mTransformation;
		mTransformation = 0;
	}

	if (mSpline != 0)
	{
		delete[] mSpline;
		mSpline = 0;
	}

	if (mTimeTag != 0)
	{
		delete[] mTimeTag;
	}

	mUseSplines = pUseSplines;

	mKeyframeCount = pKeyframeCount;

	mTransformation = new TransformationF*[mKeyframeCount];
	mTimeTag = new float[mKeyframeCount];

	for (int i = 0; i < mKeyframeCount; i++)
	{
		mTransformation[i] = 0;
		mTimeTag[i] = 0.0f;
	}

	mBoneCount = 0;
}

int BoneAnimation::GetKeyframeCount() const
{
	return (mKeyframeCount);
}

bool BoneAnimation::GetUseSplines() const
{
	return (mUseSplines);
}

void BoneAnimation::SetBoneCount(int pBoneCount)
{
	int i;
	for (i = 0; i < mKeyframeCount; i++)
	{
		if (mTransformation[i] != 0)
		{	
			delete[] mTransformation[i];
			mTransformation[i] = 0;
		}

		mBoneCount = pBoneCount;
		mTransformation[i] = new TransformationF[mBoneCount];
	}

	if (mSpline != 0)
	{
		delete[] mSpline;
		mSpline = 0;
	}

	if (mUseSplines == true)
	{
		mSpline = new CubicSpline[mBoneCount];
		for (i = 0; i < mBoneCount; i++)
		{
			mSpline[i].Init(mKeyframeCount, 3);
		}
	}
}

int BoneAnimation::GetBoneCount() const
{
	return (mBoneCount);
}

void BoneAnimation::SetTimeTag(int pKeyframe, float pTimeTag)
{
	if (pKeyframe < mKeyframeCount)
	{
		mTimeTag[pKeyframe] = pTimeTag;
	}
	else
	{
		mExtraTimeTag = pTimeTag;
	}
}

float BoneAnimation::GetTimeTag(int pKeyframe) const
{
	float lTimeTag;
	if (pKeyframe < mKeyframeCount)
	{
		lTimeTag = mTimeTag[pKeyframe];
	}
	else
	{
		lTimeTag = mExtraTimeTag;
	}
	return (lTimeTag);
}

void BoneAnimation::SetBoneTransformation(int pKeyframe, int pBoneIndex, const TransformationF& pTransformation)
{
	mTransformation[pKeyframe][pBoneIndex] = pTransformation;

	if (mUseSplines == true)
	{
		const Vector3DF& lPosition = pTransformation.GetPosition();
		float lValue[3];
		lValue[0] = lPosition.x;
		lValue[1] = lPosition.y;
		lValue[2] = lPosition.z;
		mSpline[pBoneIndex].SetPointValues(pKeyframe, mTimeTag[pKeyframe], lValue);
	}
}

const TransformationF& BoneAnimation::GetBoneTransformation(int pKeyframe, int pBoneIndex) const
{
	return (mTransformation[pKeyframe][pBoneIndex]);
}

void BoneAnimation::GetAnimationState(TransformationF* pTransformation, float pTime, Mode pMode)
{
	GetAnimationStateInterpolated(pTransformation, pTime, pMode, 1);
}

void BoneAnimation::GetAnimationStateInterpolated(TransformationF* pTransformation, float pTime, Mode pMode, float pFactor)
{
	if (pFactor <= 0)
	{
		// Nothing to do.
		return;
	}

	if (pMode == MODE_PLAY_DEFAULT)
	{
		pMode = mDefaultMode;
	}

	// Step to current frame.
	int lCurrentKeyframe = 0;
	if (pMode == MODE_PLAY_LOOP)
	{
		while (lCurrentKeyframe < (mKeyframeCount - 1) &&
			  mTimeTag[lCurrentKeyframe + 1] < pTime)
		{
			lCurrentKeyframe++;
		}
	}
	else
	{
		while (lCurrentKeyframe < (mKeyframeCount - 2) &&
			  mTimeTag[lCurrentKeyframe + 1] < pTime)
		{
			lCurrentKeyframe++;
		}
	}

	// Find time tags.
	float lCurrentTimeTag = mTimeTag[lCurrentKeyframe];
	float lNextTimeTag = mTimeTag[lCurrentKeyframe + 1];
	float lLastTimeTag = mTimeTag[mKeyframeCount - 1];
	if (pMode == MODE_PLAY_LOOP)
	{
		if (lCurrentKeyframe == mKeyframeCount - 1)
			lNextTimeTag = mExtraTimeTag;
		lLastTimeTag = mExtraTimeTag;
	}

	// Check time constraints.
	if (pTime > lLastTimeTag)
	{
		if(pMode == MODE_PLAY_LOOP)
		{
			float lRestTime = pTime - lLastTimeTag * floor(pTime / lLastTimeTag);
			GetAnimationStateInterpolated(pTransformation, lRestTime, pMode, pFactor);
			return;
		}
		else
		{
			// We can't step further than the last keyframe.
			pTime = lLastTimeTag;
		}
	}

	// Init splines.
	if (mUseSplines == true)
	{
		if (pMode == MODE_PLAY_LOOP)
		{
			for (int i = 0; i < mBoneCount; i++)
			{
				mSpline[i].StartLoopedInterpolation(0.0f, mExtraTimeTag);
				mSpline[i].StepInterpolation(pTime);
			}
		}
		else
		{
			for (int i = 0; i < mBoneCount; i++)
			{
				mSpline[i].StartInterpolation(0.0f);
				mSpline[i].StepInterpolation(pTime);
			}
		}
	}

	int lNextKeyframe = (lCurrentKeyframe + 1) % mKeyframeCount;
	TransformationF* lCurrentTransformation = mTransformation[lCurrentKeyframe];
	TransformationF* lNextTransformation = mTransformation[lNextKeyframe];
	float lFrameFactor = (pTime - lCurrentTimeTag) / (lNextTimeTag - lCurrentTimeTag);

	if (mUseSplines == false)
	{
		// Linear interpolation (no splines).
		for (int i = 0; i < mBoneCount; i++)
		{
			TransformationF lFinalTransform;
			lFinalTransform.Interpolate(lCurrentTransformation[i],
						     lNextTransformation[i],
						     lFrameFactor);

			if (pFactor < 1)
			{
				pTransformation[i].Interpolate(pTransformation[i], lFinalTransform, pFactor);
			}
			else
			{
				pTransformation[i] = lFinalTransform;
			}
		}
	}
	else
	{
		// Splines.
		int lNextNextKeyframe = (lNextKeyframe + 1) % mKeyframeCount;
		int lPrevKeyframe = (lCurrentKeyframe + mKeyframeCount - 1) % mKeyframeCount;

		TransformationF* lNextNextTransformation = mTransformation[lNextNextKeyframe];
		TransformationF* lPrevTransformation = mTransformation[lPrevKeyframe];

		for (int i = 0; i < mBoneCount; i++)
		{
			Vector3DF lPosition(mSpline[i].GetValue(0),
						mSpline[i].GetValue(1),
						mSpline[i].GetValue(2));

			TransformationF lFinalTransform;
			lFinalTransform.SetPosition(lPosition);



			float t = mSpline[i].GetCurrentSegmentTime();

			// We have 4 frames, numbered from 1 to 4, where the current frame 
			// is number 2. Using these, we are doing a Catmull-Rom spline.
			// TODO: Test this with uneven time tags.
			QuaternionF lQTemp12;
			QuaternionF lQTemp23;
			QuaternionF lQTemp34;

			lQTemp12.Slerp(lPrevTransformation[i].GetOrientation(), 
					lCurrentTransformation[i].GetOrientation(), t + 1);
			lQTemp23.Slerp(lCurrentTransformation[i].GetOrientation(), 
					lNextTransformation[i].GetOrientation(), t);
			lQTemp34.Slerp(lNextTransformation[i].GetOrientation(), 
					lNextNextTransformation[i].GetOrientation(), t - 1);

			QuaternionF lQTemp123;
			QuaternionF lQTemp234;

			lQTemp123.Slerp(lQTemp12, lQTemp23, (t + 1) / 2.0f);
			lQTemp234.Slerp(lQTemp23, lQTemp34, t / 2.0f);

			// And finally...
			QuaternionF lRot;
			lRot.Slerp(lQTemp123, lQTemp234, t);

			lFinalTransform.SetOrientation(lRot);

			if (pFactor < 1)
			{
				pTransformation[i].Interpolate(pTransformation[i], lFinalTransform, pFactor);
			}
			else
			{
				pTransformation[i] = lFinalTransform;
			}
		}
	}
}






BoneAnimator::BoneAnimator(BoneHierarchy* pBones) :
	mBones(pBones)
{
}

BoneAnimator::~BoneAnimator()
{
}



void BoneAnimator::AddAnimation(const str& pName, BoneAnimation* pAnimation)
{
	mAnimTable.Insert(pName, pAnimation);
}



void BoneAnimator::StartAnimation(const str& pAnimationName, float pBlendingTime, BoneAnimation::Mode pMode)
{
	AnimTable::Iterator lIter = mAnimTable.Find(pAnimationName);

	if (lIter != mAnimTable.End())
	{
		BoneAnimation* lAnimation = *lIter;

		AnimData lAnimData;
		lAnimData.mAnimation = lAnimation;
		lAnimData.mCurrentTime = 0;
		lAnimData.mBlendingTime = pBlendingTime;
		lAnimData.mMode = pMode;

		mCurrentlyPlayingAnims.push_back(lAnimData);
	}
}



void BoneAnimator::Step(float pStepTime)
{
	if (mCurrentlyPlayingAnims.empty())
	{
		return;
	}

	AnimList::iterator lNextIter(mCurrentlyPlayingAnims.end());
	AnimList::iterator lIter(--mCurrentlyPlayingAnims.end());

	// Increase time counters and check wether the animations are currently playing.
	for (; lIter != mCurrentlyPlayingAnims.end(); --lIter)
	{
		(*lIter).mCurrentTime += pStepTime;

		if (lNextIter != mCurrentlyPlayingAnims.end() &&
		   (*lIter).mCurrentTime >= (*lNextIter).mBlendingTime)
		{
			// Delete all animations previous to this.
			while (lIter != mCurrentlyPlayingAnims.end())
			{
				AnimList::iterator y = lIter;
				--lIter;
				mCurrentlyPlayingAnims.erase(y);
			}
		}

		lNextIter = lIter;
	}

	// The following requires access to the private members of 'BoneHierarchy'.
	if (mCurrentlyPlayingAnims.size() == 1)
	{
		// Only one animation is running. Just update the bones.

		AnimData& lAnimData = mCurrentlyPlayingAnims.front();
		lAnimData.mAnimation->GetAnimationState(mBones->mCurrentBoneTransformation, lAnimData.mCurrentTime, lAnimData.mMode);

		if (mBones->mParent == 0)
		{
			mBones->UpdateBonesObjectTransformation(mBones->mRootBoneIndex, gIdentityTransformationF);
		}
		else
		{
			mBones->UpdateBonesObjectTransformation(mBones->mRootBoneIndex, mBones->mParent->GetBoneObjectTransformation(mBones->mParentBoneIndex));
		}
	}
	else
	{
		// Several animations should be blended. Start by just copying the first animation.

		lIter = mCurrentlyPlayingAnims.begin();
		AnimData& lFirst  = *lIter;
		lFirst.mAnimation->GetAnimationState(mBones->mCurrentBoneTransformation, lFirst.mCurrentTime, lFirst.mMode);

		// Step to next animation.
		++lIter;
		for (; lIter != mCurrentlyPlayingAnims.end(); ++lIter)
		{
			AnimData& lSecond = *lIter;

			// The blending time can't be zero due to the check that is performed first in this function.
			float lFactor = lSecond.mCurrentTime / lSecond.mBlendingTime;
			if (lFactor > 1)
				lFactor = 1;

			lSecond.mAnimation->GetAnimationStateInterpolated(mBones->mCurrentBoneTransformation, lSecond.mCurrentTime, lSecond.mMode, lFactor);

			lFirst = lSecond;
		}
	}
}



BoneHierarchy* BoneAnimator::GetBones()
{
	return (mBones);
}



} // End namespace.
