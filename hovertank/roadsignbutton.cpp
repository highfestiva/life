
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../uicure/include/uiresourcemanager.h"
#include "roadsignbutton.h"
#include "../cure/include/contextmanager.h"
#include "../cure/include/runtimevariable.h"
#include "../cure/include/timemanager.h"
#include "../tbc/include/chunkyphysics.h"
#include "../uicure/include/uigameuimanager.h"
#include "../uitbc/include/gui/uidesktopwindow.h"
#include "../uitbc/include/gui/uifloatinglayout.h"
#include "hovertankmanager.h"



namespace HoverTank {



static const float MIN_TIME = 0.2f;
static const float MAX_TIME = 1.0f;



RoadSignButton::RoadSignButton(life::ScreenPart* screen_part, cure::ResourceManager* resource_manager,
	UiCure::GameUiManager* ui_manager, const str& name, const str& class_resource_name,
	const str& texture_resource_name, Shape shape):
	Parent(resource_manager, class_resource_name, ui_manager),
	screen_part_(screen_part),
	button_(name),
	shape_(shape),
	active_(false),
	mesh_radius_(0),
	angle_part_(0),
	angle_time_(0),
	angle_(0),
	is_moving_in_(true),
	trajectory_end_distance_(0),
	trajectory_angle_(-4*PIF),
	fov_(90),
	is_original_orientation_set_(false) {
	button_.SetPreferredSize(5, 5);
	button_.SetMinSize(5, 5);
	button_.SetPos(-50, -50);
	GetUiManager()->AssertDesktopLayout(new uitbc::FloatingLayout, 0);
	GetUiManager()->GetDesktopWindow()->AddChild(&button_, 0, 0, 0);

	button_.SetOnRender(RoadSignButton, RenderButton);
	button_.SetOnIsOver(RoadSignButton, IsOverButton);

	SetPhysicsTypeOverride(cure::kPhysicsOverrideBones);
	SetUseDefaultTexture(true);
	UiCure::UserRendererImageResource* texture = new UiCure::UserRendererImageResource(ui_manager_, UiCure::ImageProcessSettings(Canvas::kResizeFast, false));
	texture_resource_array_.push_back(texture);
	texture->Load(GetResourceManager(), texture_resource_name,
		UiCure::UserRendererImageResource::TypeLoadCallback(this, &RoadSignButton::OnLoadTexture));
}

RoadSignButton::~RoadSignButton() {
	GetUiManager()->GetDesktopWindow()->RemoveChild(&button_, 0);
}



uitbc::CustomButton& RoadSignButton::GetButton() {
	return (button_);
}

void RoadSignButton::SetTrajectory(const vec2& end_point, float end_distance) {
	trajectory_end_distance_ = end_distance;
	trajectory_end_point_ = end_point;
	if (!mesh_resource_array_.empty() && mesh_resource_array_[0] && mesh_resource_array_[0]->GetLoadState() == cure::kResourceLoadComplete) {
		SetInitialPosition(mesh_resource_array_[0]->GetRamData());
	}

	angle_part_ = 1;
	SetIsMovingIn(true);
	angle_ = -PIF;
}

void RoadSignButton::SetTrajectoryAngle(float angle) {
	trajectory_angle_ = angle;
}

void RoadSignButton::SetOrientation(const quat& orientation) {
	orientation_ = orientation;
}

void RoadSignButton::SetIsMovingIn(bool is_moving_in) {
	if (is_moving_in) {
		active_ = true;
	}
	is_moving_in_ = is_moving_in;
	//const float time = is_moving_in_? trajectory_end_point_.GetLength() : 1 - trajectory_end_point_.GetLength();
	angle_time_ = Math::Lerp(MIN_TIME, MAX_TIME, trajectory_end_point_.GetLength());
}

void RoadSignButton::MoveSign(const float frame_time) {
	button_.SetVisible(active_);
	if (!active_) {
		return;
	}
	if (physics_ == 0) {
		return;
	}
	if (!is_original_orientation_set_) {
		is_original_orientation_set_ = true;
		original_orientation_ = physics_->GetBoneTransformation(physics_->GetRootBone()).GetOrientation();
		original_orientation_ *= orientation_;
	}

	const vec2 ratio = GetAspectRatio(false);
	const float x = trajectory_end_point_.x * pow(ratio.x, 0.7f) * 0.9f * trajectory_end_distance_;
	const float z = trajectory_end_point_.y * pow(ratio.y, 0.7f) * 0.9f * trajectory_end_distance_;
	const vec3 end_position(x, trajectory_end_distance_, z);

	// Rotate "inward" until end pos reached, or outward until invisible.
	xform transform;
	transform.SetPosition(end_position);
	if (is_moving_in_ && angle_part_ > 0) {
		angle_part_ -= frame_time/angle_time_;
	} else if (!is_moving_in_ && angle_part_ < 1) {
		angle_part_ += frame_time/angle_time_;
	}
	angle_part_ = Math::Clamp(angle_part_, 0.0f, 1.0f);
	angle_ = Math::Lerp(angle_, GetTargetAngle(), 0.2f);

	UiCure::UserGeometryReferenceResource* resource = mesh_resource_array_[0];
	if (resource->GetLoadState() != cure::kResourceLoadComplete) {
		return;
	}
	tbc::GeometryReference* gfx_geometry = (tbc::GeometryReference*)resource->GetRamData();
	const vec3& position = gfx_geometry->GetTransformation().GetPosition();
	float screen_radius = 0;
	const vec2 screen_position = Get2dProjectionPosition(position, screen_radius);
	float trajectory_angle = trajectory_angle_;
	if (trajectory_angle < -2*PIF) {
		trajectory_angle = (screen_position.x <= GetUiManager()->GetDisplayManager()->GetWidth()/2)? PIF : 0;
	}
	const float anchor_x = 0.8f*trajectory_end_distance_*1/ratio.y*cos(trajectory_angle);
	const float anchor_z = 0.8f*trajectory_end_distance_*1/ratio.x*-sin(trajectory_angle);
	const vec3 anchor = vec3(anchor_x, 2*trajectory_end_distance_, anchor_z) - end_position;
	const vec3 axis(-anchor_z, 0, anchor_x);

	// Set button size and position according to 3D-geometry location on screen.
	if (shape_ == kShapeBox) {
		screen_radius = screen_radius / 1.35f;
	}
	button_.SetPos((int)(screen_position.x-screen_radius), (int)(screen_position.y-screen_radius));
	button_.SetSize((int)(screen_radius*2), (int)(screen_radius*2));
	button_.SetPreferredSize((int)(screen_radius*2), (int)(screen_radius*2));

	transform.RotateAroundAnchor(anchor, axis, angle_);
	transform.GetOrientation() *= original_orientation_;

	// Button hoover yields hot road sign.
	vec3 target_offset;
	if (button_.GetState() == uitbc::Button::kReleasedHoover ||
		button_.GetState() == uitbc::Button::kPressing) {
		target_offset = end_position*0.4f;
		current_offset_ = Math::Lerp(current_offset_, target_offset, Math::GetIterateLerpTime(1.2f, frame_time));
	} else {
		current_offset_ = Math::Lerp(current_offset_, target_offset, Math::GetIterateLerpTime(0.8f, frame_time));
	}
	transform.GetPosition() -= current_offset_;

	for (size_t x = 0; x < mesh_resource_array_.size(); ++x) {
		UiCure::UserGeometryReferenceResource* resource = mesh_resource_array_[x];
		if (resource->GetLoadState() != cure::kResourceLoadComplete) {
			continue;
		}

		tbc::GeometryBase* gfx_geometry = resource->GetRamData();
		gfx_geometry->SetTransformation(transform);
	}
}

void RoadSignButton::OnTick() {
	if (!GetManager()) {
		return;
	}

	const float _frame_time = std::min(0.1f, GetManager()->GetGameManager()->GetTimeManager()->GetRealNormalFrameTime());
	MoveSign(_frame_time);
}



void RoadSignButton::RenderButton(uitbc::CustomButton*) {
	//button->Button::Repaint(GetUiManager()->GetPainter());

	if (angle_part_ >= 1 && ::fabs(angle_-GetTargetAngle()) < 0.1f) {
		active_ = false;
		return;
	}

	// Render all meshes that constitute the road sign.
	for (size_t x = 0; x < mesh_resource_array_.size(); ++x) {
		UiCure::UserGeometryReferenceResource* resource = mesh_resource_array_[x];
		if (resource->GetLoadState() != cure::kResourceLoadComplete) {
			continue;
		}

		tbc::GeometryBase* gfx_geometry = resource->GetRamData();

		if (x == 0) {
			const PixelRect top_viewport = screen_part_->GetRenderArea();
			GetUiManager()->GetRenderer()->ResetClippingRect();
			GetUiManager()->GetRenderer()->SetClippingRect(top_viewport);
			GetUiManager()->GetRenderer()->SetViewport(top_viewport);
			fov_ = screen_part_->UpdateFrustum(45.0f);
		}

		quat light_orientation;
		light_orientation.RotateAroundOwnX(PIF/8);
		GetUiManager()->GetRenderer()->RenderRelative(gfx_geometry, &light_orientation);
	}
	GetUiManager()->PreparePaint(false);	// Back to painting again afterwards.
}

bool RoadSignButton::IsOverButton(uitbc::CustomButton* button, int x, int y) {
	if (shape_ == kShapeRound) {
		PixelCoord middle(button->GetPos() + button->GetSize()/2);
		const int radius = button->GetSize().x/2;
		return (middle.GetDistance(PixelCoord(x, y)) <= radius);
	}
	return (button->Button::IsOver(x, y));
}

void RoadSignButton::LoadTextures() {
	// We don't want any more textures than we already started loading.
}

void RoadSignButton::DispatchOnLoadMesh(UiCure::UserGeometryReferenceResource* mesh_resource) {
	Parent::DispatchOnLoadMesh(mesh_resource);

	if (mesh_resource->GetLoadState() == cure::kResourceLoadComplete) {
		tbc::GeometryBase* _geometry = mesh_resource->GetRamData();
		_geometry->SetAlwaysVisible(false);
		SetInitialPosition(_geometry);

		if (mesh_resource != mesh_resource_array_[0]) {
			return;
		}
		vec3 far;
		const float* vtx = _geometry->GetVertexData();
		const unsigned c = _geometry->GetVertexCount();
		for (unsigned x = 0; x < c; ++x) {
			vec3 vertex(vtx[x*3+0], vtx[x*3+1], vtx[x*3+2]);
			if (vertex.GetLength() > mesh_radius_) {
				far = vertex;
				mesh_radius_ = vertex.GetLength();
			}
		}
	}
}

void RoadSignButton::OnLoadTexture(UiCure::UserRendererImageResource* texture_resource) {
	Parent::OnLoadTexture(texture_resource);
}

str RoadSignButton::GetMeshInstanceId() const {
	const int slave_index = GetManager()? ((HoverTankManager*)GetManager()->GetGameManager())->GetSlaveIndex() : -1;
	const int slave_index_offset = slave_index * 100000;
	return (strutil::IntToString(GetInstanceId() + slave_index_offset, 10));
}



vec2 RoadSignButton::Get2dProjectionPosition(const vec3& p3dPosition, float& p2dRadius) const {
	const PixelRect top_viewport = screen_part_->GetRenderArea();
	const float aspect_ratio = top_viewport.GetWidth() / (float)top_viewport.GetHeight();
	const float strange_correction_constant = 1.011f;
	const float distance = ::pow(p3dPosition.y, strange_correction_constant);
	const float projection_length_inverse = 1 / (strange_correction_constant * asin(fov_ * PIF / 180 / 2));
	p2dRadius = mesh_radius_/distance * projection_length_inverse * 0.5f * top_viewport.GetWidth();
	const float screen_x = +p3dPosition.x/distance * projection_length_inverse * 0.5f * top_viewport.GetWidth() + top_viewport.GetCenterX();
	const float screen_y = -p3dPosition.z/distance * projection_length_inverse * aspect_ratio * 0.5f * top_viewport.GetHeight() + top_viewport.GetCenterY();
	return (vec2(screen_x, screen_y));
}

vec2 RoadSignButton::GetAspectRatio(bool inverse) const {
	const PixelRect top_viewport = screen_part_->GetRenderArea();
	const float aspect_ratio = top_viewport.GetWidth() / (float)top_viewport.GetHeight();
	const float x = (inverse^(aspect_ratio < 1))? aspect_ratio : 1;
	const float y = (inverse^(aspect_ratio < 1))? 1 : 1/aspect_ratio;
	return (vec2(x, y));
}



void RoadSignButton::SetInitialPosition(tbc::GeometryBase* geometry) const {
	xform transformation;
	if (trajectory_angle_ < -2*PIF) {
		if (trajectory_end_point_.x > 0) {
			transformation.SetPosition(vec3(trajectory_end_distance_*3, 0, trajectory_end_distance_));
		} else {
			transformation.SetPosition(vec3(-trajectory_end_distance_*3, 0, trajectory_end_distance_));
		}
	} else {
		transformation.SetPosition(vec3(-trajectory_end_distance_*3*cos(trajectory_angle_), -trajectory_end_distance_*3*sin(trajectory_angle_), trajectory_end_distance_));
	}
	geometry->SetTransformation(transformation);
}

float RoadSignButton::GetTargetAngle() const {
	const float hinge_angle_factor = 0.7f;	// How much the sign should "hinge" *(1.0 is half a revolution).
	return (-PIF*hinge_angle_factor*angle_part_);
}



loginstance(kGameContextCpp, RoadSignButton);



}
