
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../uicure/include/uicppcontextobject.h"
#include "../uitbc/include/gui/uicustombutton.h"
#include "hovertank.h"



namespace life {
class ScreenPart;
}



namespace HoverTank {



class RoadSignButton: public UiCure::CppContextObject {
	typedef UiCure::CppContextObject Parent;
public:
	enum Shape {
		kShapeBox = 1,
		kShapeRound,
	};

	RoadSignButton(life::ScreenPart* screen_part, cure::ResourceManager* resource_manager,
		UiCure::GameUiManager* ui_manager, const str& name, const str& class_resource_name,
		const str& texture_resource_name, Shape shape);
	virtual ~RoadSignButton();

	uitbc::CustomButton& GetButton();
	void SetTrajectory(const vec2& end_point, float end_distance);
	void SetTrajectoryAngle(float angle);
	void SetOrientation(const quat& orientation);
	void SetIsMovingIn(bool is_moving_in);

	void MoveSign(const float frame_time);

protected:
	void OnTick();
	void RenderButton(uitbc::CustomButton*);
	bool IsOverButton(uitbc::CustomButton* button, int x, int y);
	virtual void LoadTextures();
	virtual void DispatchOnLoadMesh(UiCure::UserGeometryReferenceResource* mesh_resource);
	void OnLoadTexture(UiCure::UserRendererImageResource* texture_resource);
	virtual str GetMeshInstanceId() const;

	vec2 Get2dProjectionPosition(const vec3& p3dPosition, float& p2dRadius) const;
	vec2 GetAspectRatio(bool inverse) const;

private:
	void SetInitialPosition(tbc::GeometryBase* geometry) const;
	float GetTargetAngle() const;

	life::ScreenPart* screen_part_;
	uitbc::CustomButton button_;
	Shape shape_;
	bool active_;
	float mesh_radius_;
	float angle_part_;
	float angle_time_;
	float angle_;
	bool is_moving_in_;
	vec3 current_offset_;
	float trajectory_end_distance_;
	vec2 trajectory_end_point_;
	float trajectory_angle_;
	float fov_;
	quat original_orientation_;
	quat orientation_;
	bool is_original_orientation_set_;

	logclass();
};



}
