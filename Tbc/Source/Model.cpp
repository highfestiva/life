
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/Model.h"
#include <math.h>



namespace Tbc
{



Model::Model()
{
}

Model::~Model()
{
}

void Model::RemoveAll()
{
	AnimatorTable::Iterator x = mAnimatorTable.First();
	for (; x != mAnimatorTable.End(); ++x)
	{
		BoneAnimator* lAnimator = *x;
		delete (lAnimator);
	}
	mAnimatorTable.RemoveAll();
}

void Model::AddAnimator(const str& pName, BoneAnimator* pBonesAnimator)
{
	mAnimatorTable.Insert(pName, pBonesAnimator);
}

BoneAnimator* Model::GetAnimator(const str& pName)
{
	BoneAnimator* lAnimator = 0;
	AnimatorTable::Iterator x = mAnimatorTable.Find(pName);
	if (x != mAnimatorTable.End())
	{
		lAnimator = *x;
	}

	return lAnimator;
}

void Model::StartAnimation(const str& pAnimName, float pBlendingTime, BoneAnimation::Mode pMode)
{
	AnimatorTable::Iterator x;
	for (x = mAnimatorTable.First(); x != mAnimatorTable.End(); ++x)
	{
		BoneAnimator* lAnimator = *x;
		lAnimator->StartAnimation(pAnimName, pBlendingTime, pMode);
	}
}

void Model::Update(double pDeltaTime)
{
	AnimatorTable::Iterator lAIter;
	for (lAIter = mAnimatorTable.First(); lAIter != mAnimatorTable.End(); ++lAIter)
	{
		BoneAnimator* lAnimator = *lAIter;
		lAnimator->Step((float)pDeltaTime);
	}
}



void Model::SetTransformation(const xform& pTransformation)
{
	mTransformation = pTransformation;
}

const xform& Model::GetTransformation() const
{
	return (mTransformation);
}



}
