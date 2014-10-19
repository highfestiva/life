
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../UiCure/Include/UiMachine.h"
#include "Impuzzable.h"



namespace Impuzzable
{



class Piece: public UiCure::Machine
{
	typedef UiCure::Machine Parent;
public:
	Piece(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager);
	virtual ~Piece();
	virtual bool TryComplete();

	virtual void OnTick();
	virtual void OnMicroTick(float);

	bool IsDragging() const;
	float GetDragDepth() const;
	void SetDragging(bool pIsDragging, float pDepth);
	Tbc::PhysicsManager::BodyID GetRootBodyId() const;
	void GetBodyIds(std::vector<Tbc::PhysicsManager::BodyID>& pBodyIds) const;
	void SetDragPosition(const vec3& pPosition);
	vec3 GetDragPosition() const;
	void SetMoveTarget(const vec3& pPosition);
	vec3 GetMoveTarget() const;

private:
	vec3 mDragOffset;
	vec3 mMoveTarget;
	vec3 mPreviousPosition;
	vec3 mPreviousVelocity;
	float mAverageSpeed;
	float mDragDepth;
	bool mIsDragging;
};



}
