
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/model.h"
#include <math.h>



namespace tbc {



Model::Model() {
}

Model::~Model() {
}

void Model::RemoveAll() {
	AnimatorTable::Iterator x = animator_table_.First();
	for (; x != animator_table_.End(); ++x) {
		BoneAnimator* animator = *x;
		delete (animator);
	}
	animator_table_.RemoveAll();
}

void Model::AddAnimator(const str& name, BoneAnimator* bones_animator) {
	animator_table_.Insert(name, bones_animator);
}

BoneAnimator* Model::GetAnimator(const str& name) {
	BoneAnimator* animator = 0;
	AnimatorTable::Iterator x = animator_table_.Find(name);
	if (x != animator_table_.End()) {
		animator = *x;
	}

	return animator;
}

void Model::StartAnimation(const str& anim_name, float blending_time, BoneAnimation::Mode mode) {
	AnimatorTable::Iterator x;
	for (x = animator_table_.First(); x != animator_table_.End(); ++x) {
		BoneAnimator* animator = *x;
		animator->StartAnimation(anim_name, blending_time, mode);
	}
}

void Model::Update(double delta_time) {
	AnimatorTable::Iterator a_iter;
	for (a_iter = animator_table_.First(); a_iter != animator_table_.End(); ++a_iter) {
		BoneAnimator* animator = *a_iter;
		animator->Step((float)delta_time);
	}
}



void Model::SetTransformation(const xform& transformation) {
	transformation_ = transformation;
}

const xform& Model::GetTransformation() const {
	return (transformation_);
}



}
