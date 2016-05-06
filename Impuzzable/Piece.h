
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../uicure/include/uimachine.h"
#include "impuzzable.h"



namespace Impuzzable {



class Piece: public UiCure::Machine {
	typedef UiCure::Machine Parent;
public:
	Piece(cure::ResourceManager* resource_manager, const str& class_id, UiCure::GameUiManager* ui_manager);
	virtual ~Piece();
	virtual bool TryComplete();

	virtual void OnTick();
	virtual void OnMicroTick(float);

	bool IsDragging() const;
	float GetDragDepth() const;
	void SetDragging(bool is_dragging, float depth);
	tbc::PhysicsManager::BodyID GetRootBodyId() const;
	void GetBodyIds(std::vector<tbc::PhysicsManager::BodyID>& body_ids) const;
	void SetDragPosition(const vec3& position);
	vec3 GetDragPosition() const;
	void SetMoveTarget(const vec3& position);
	vec3 GetMoveTarget() const;

private:
	vec3 drag_offset_;
	vec3 move_target_;
	vec3 previous_position_;
	vec3 previous_velocity_;
	float average_speed_;
	float drag_depth_;
	bool is_dragging_;
};



}
