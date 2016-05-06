
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/uidebugrenderer.h"
#include "../../cure/include/contextmanager.h"
#include "../../cure/include/contextobject.h"
#include "../../cure/include/gamemanager.h"
#include "../../cure/include/runtimevariable.h"
#include "../../tbc/include/chunkybonegeometry.h"
#include "../../tbc/include/chunkyphysics.h"
#include "../../uitbc/include/uirenderer.h"
#include "../include/uigameuimanager.h"
#include "../include/uiruntimevariablename.h"



namespace UiCure {



DebugRenderer::DebugRenderer(const cure::RuntimeVariableScope* variable_scope, GameUiManager* ui_manager, const cure::ContextManager* context, const cure::ContextManager* remote_context, Lock* tick_lock):
	ui_manager_(ui_manager),
	context_(context),
	remote_context_(remote_context),
	tick_lock_(tick_lock) {
}

DebugRenderer::~DebugRenderer() {
}



void DebugRenderer::Render(const GameUiManager* ui_manager, const PixelRect& render_area) {
	bool debug_axes;
	bool debug_joints;
	bool debug_shapes;
	v_get(debug_axes, =, GetSettings(), kRtvarDebug3DEnableaxes, false);
	v_get(debug_joints, =, GetSettings(), kRtvarDebug3DEnablejoints, false);
	v_get(debug_shapes, =, GetSettings(), kRtvarDebug3DEnableshapes, false);
	if (debug_axes || debug_joints || debug_shapes) {
		ScopeLock lock(tick_lock_);
		ui_manager->GetRenderer()->ResetClippingRect();
		ui_manager->GetRenderer()->SetClippingRect(render_area);
		ui_manager->GetRenderer()->SetViewport(render_area);

		const cure::ContextManager::ContextObjectTable& object_table = context_->GetObjectTable();
		cure::ContextManager::ContextObjectTable::const_iterator x = object_table.begin();
		for (; x != object_table.end(); ++x) {
			cure::ContextObject* _object;
			if (remote_context_) {
				if (context_->IsLocalGameObjectId(x->first)) {
					continue;
				}
				_object = remote_context_->GetObject(x->first);
				if (!_object) {
					_object = x->second;
				}
			} else {
				_object = x->second;
			}
			if (debug_axes) {
				DebugDrawPrimitive(_object, kDebugAxes);
			}
			if (debug_joints) {
				DebugDrawPrimitive(_object, kDebugJoints);
			}
			if (debug_shapes) {
				DebugDrawPrimitive(_object, kDebugShapes);
			}
		}
	}
}

void DebugRenderer::DebugDrawPrimitive(cure::ContextObject* object, DebugPrimitive primitive) {
	if (!object->GetPhysics()) {
		return;
	}

	cure::ContextManager* context_manager = object->GetManager();
	xform physics_transform;
	const int bone_count = object->GetPhysics()->GetBoneCount();
	for (int x = 0; x < bone_count; ++x) {
		const tbc::ChunkyBoneGeometry* geometry = object->GetPhysics()->GetBoneGeometry(x);
		tbc::PhysicsManager::BodyID body_id = geometry->GetBodyId();
		if (body_id != tbc::INVALID_BODY) {
			context_manager->GetGameManager()->GetPhysicsManager()->GetBodyTransform(body_id, physics_transform);
		} else if (geometry->GetBoneType() == tbc::ChunkyBoneGeometry::kBonePosition) {
			body_id = object->GetPhysics()->GetBoneGeometry(0)->GetBodyId();
			context_manager->GetGameManager()->GetPhysicsManager()->GetBodyTransform(body_id, physics_transform);
			physics_transform.GetPosition() += physics_transform.GetOrientation() * object->GetPhysics()->GetOriginalBoneTransformation(x).GetPosition();
		} else {
			continue;
		}
		vec3 pos = physics_transform.GetPosition();
		switch (primitive) {
			case kDebugAxes: {
				const float length = 2;
				const vec3& axis_x = physics_transform.GetOrientation().GetAxisX();
				ui_manager_->GetRenderer()->DrawLine(pos, axis_x*length, RED);
				const vec3& axis_y = physics_transform.GetOrientation().GetAxisY();
				ui_manager_->GetRenderer()->DrawLine(pos, axis_y*length, GREEN);
				const vec3& axis_z = physics_transform.GetOrientation().GetAxisZ();
				ui_manager_->GetRenderer()->DrawLine(pos, axis_z*length, BLUE);
			} break;
			case kDebugJoints: {
				const tbc::PhysicsManager::JointID joint = geometry->GetJointId();
				if (joint != tbc::INVALID_JOINT) {
					vec3 anchor;
					if (geometry->GetJointType() == tbc::ChunkyBoneGeometry::kJointSlider) {
						// Ignore, no anchor to be had.
					} else if (context_manager->GetGameManager()->GetPhysicsManager()->GetAnchorPos(joint, anchor)) {
						const float length = 1;
						vec3 axis;
						if (geometry->GetJointType() == tbc::ChunkyBoneGeometry::kJointBall) {
							// Ball joints don't have axes.
							ui_manager_->GetRenderer()->DrawLine(anchor, vec3(0,0,3), BLACK);
							break;
						} else if (context_manager->GetGameManager()->GetPhysicsManager()->GetAxis1(joint, axis)) {
							ui_manager_->GetRenderer()->DrawLine(anchor, axis*length, DARK_GRAY);
						} else {
							deb_assert(false);
						}
						if (context_manager->GetGameManager()->GetPhysicsManager()->GetAxis2(joint, axis)) {
							ui_manager_->GetRenderer()->DrawLine(anchor, axis*length, BLACK);
						}
					} else {
						deb_assert(false);
					}
				}
			} break;
			case kDebugShapes: {
				const vec3 size = geometry->GetShapeSize() / 2;
				const quat& rot = physics_transform.GetOrientation();
				vec3 vertex[8];
				for (int x = 0; x < 8; ++x) {
					vertex[x] = pos - rot *
						vec3(size.x*((x&4)? 1 : -1),
							size.y*((x&1)? 1 : -1),
							size.z*((x&2)? 1 : -1));
				}
				ui_manager_->GetRenderer()->DrawLine(vertex[0], vertex[1]-vertex[0], YELLOW);
				ui_manager_->GetRenderer()->DrawLine(vertex[1], vertex[3]-vertex[1], YELLOW);
				ui_manager_->GetRenderer()->DrawLine(vertex[3], vertex[2]-vertex[3], YELLOW);
				ui_manager_->GetRenderer()->DrawLine(vertex[2], vertex[0]-vertex[2], YELLOW);
				ui_manager_->GetRenderer()->DrawLine(vertex[4], vertex[5]-vertex[4], MAGENTA);
				ui_manager_->GetRenderer()->DrawLine(vertex[5], vertex[7]-vertex[5], MAGENTA);
				ui_manager_->GetRenderer()->DrawLine(vertex[7], vertex[6]-vertex[7], MAGENTA);
				ui_manager_->GetRenderer()->DrawLine(vertex[6], vertex[4]-vertex[6], MAGENTA);
				ui_manager_->GetRenderer()->DrawLine(vertex[0], vertex[4]-vertex[0], CYAN);
				ui_manager_->GetRenderer()->DrawLine(vertex[1], vertex[5]-vertex[1], CYAN);
				ui_manager_->GetRenderer()->DrawLine(vertex[2], vertex[6]-vertex[2], ORANGE);
				ui_manager_->GetRenderer()->DrawLine(vertex[3], vertex[7]-vertex[3], ORANGE);
			} break;
			default: {
				deb_assert(false);
			} break;
		}
	}
}

void DebugRenderer::RenderSpline(const GameUiManager* ui_manager, Spline* spline) {
	bool debug_joints;
	v_get(debug_joints, =, GetSettings(), kRtvarDebug3DEnablejoints, false);
	if (!debug_joints) {
		return;
	}
	const float t = spline->GetCurrentInterpolationTime();
	for (float x = 0; x < 1; x += 0.01f) {
		spline->GotoAbsoluteTime(x);
		ui_manager->GetRenderer()->DrawLine(spline->GetValue(), spline->GetSlope() * 2, WHITE);
	}
	spline->GotoAbsoluteTime(t);
}


}
