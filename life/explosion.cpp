
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "explosion.h"
#include "../cure/include/contextmanager.h"
#include "../cure/include/cppcontextobject.h"
#include "../cure/include/gamemanager.h"
#include "../tbc/include/chunkybonegeometry.h"
#include "../tbc/include/chunkyphysics.h"



namespace life {



float Explosion::CalculateForce(tbc::PhysicsManager* physics_manager, const cure::ContextObject* object, const vec3& position, float strength) {
	return Force(physics_manager, object, position, strength, 0);
}

float Explosion::PushObject(tbc::PhysicsManager* physics_manager, const cure::ContextObject* object, const vec3& position, float strength, float time_factor) {
	return Force(physics_manager, object, position, strength, time_factor);
}

void Explosion::FallApart(tbc::PhysicsManager* physics_manager, cure::CppContextObject* object, bool include_fixed) {
	tbc::ChunkyPhysics* physics = object->GetPhysics();
	const int bone_count = physics->GetBoneCount();
	for (int x = 0; x < bone_count; ++x) {
		tbc::ChunkyBoneGeometry* geometry = physics->GetBoneGeometry(x);
		if (geometry->GetBodyId() == tbc::INVALID_BODY) {
			continue;
		}
		if (!include_fixed && geometry->GetBoneType() != tbc::ChunkyBoneGeometry::kBoneBody) {
			continue;
		}
		if (geometry->GetJointType() == tbc::ChunkyBoneGeometry::kJointExclude) {
			if (geometry->GetParent() && geometry->IsDetachable()) {
				physics_manager->DetachToDynamic(geometry->GetBodyId(), geometry->GetMass());
			}
			/*else {
				physics_manager->AddMass(geometry->GetBodyId(), geometry->GetParent()->GetBodyId());
			}*/
		} else if (geometry->GetJointId() != tbc::INVALID_JOINT) {
			physics_manager->DeleteJoint(geometry->GetJointId());
			geometry->ResetJointId();
		}
		// This is so that the different parts of the now broken object can collide with each other.
		//geometry->SetFlag(AWAITING_DISCONNECTION_FROM_OTHER_SELF_PART...);
		physics_manager->EnableCollideWithSelf(geometry->GetBodyId(), true);
	}

	physics->ClearEngines();

	object->QuerySetChildishness(0);
}

void Explosion::Freeze(tbc::PhysicsManager* physics_manager, const cure::ContextObject* object) {
	// Make physics static.
	tbc::ChunkyPhysics* physics = object->ContextObject::GetPhysics();
	const int bone_count = physics->GetBoneCount();
	for (int x = 0; x < bone_count; ++x) {
		tbc::ChunkyBoneGeometry* geometry = physics->GetBoneGeometry(x);
		if (geometry->GetBoneType() != tbc::ChunkyBoneGeometry::kBoneBody) {
			continue;
		}
		physics_manager->EnableCollideWithSelf(geometry->GetBodyId(), false);
		physics_manager->MakeStatic(geometry->GetBodyId());
	}
}



float Explosion::Force(tbc::PhysicsManager* physics_manager, const cure::ContextObject* object, const vec3& position, float strength, float time_factor) {
	tbc::ChunkyPhysics* physics = object->ContextObject::GetPhysics();
	if (!physics) {
		return 0;
	}
	const float explosive_dig = 0.75f;	// How much below the collision point the explosion "digs" things up. Adds angular velocity to objects.
	const float force_factor = time_factor? 4.8f/time_factor : 0.0f;
	// Dynamics only get hit in the main body, while statics gets all their dynamic sub-bodies hit.
	const vec3 epicenter = position + vec3(0, 0, -explosive_dig);
	const bool is_dynamic = (physics->GetPhysicsType() == tbc::ChunkyPhysics::kDynamic);
	const int bone_start = is_dynamic? 0 : 1;
	const int bone_count = physics->GetBoneCount();
	float force = 0;
	int y;
	for (y = bone_start; y < bone_count; ++y) {
		const tbc::ChunkyBoneGeometry* geometry = physics->GetBoneGeometry(y);
		if (geometry->GetBoneType() != tbc::ChunkyBoneGeometry::kBoneBody
			|| physics_manager->IsStaticBody(geometry->GetBodyId())) {
			continue;
		}
		vec3 body_center = physics_manager->GetBodyPosition(geometry->GetBodyId());
		vec3 f = body_center - epicenter;
		f.z += explosive_dig*2;	// Multiply by two, to end up above center, so we'll cause angular rotation.
		float d = f.GetLength();
		if (d > 80*strength) {
			continue;
		}
		d = 1/d;
		f *= d;
		d *= 8;	// Just so we have at least some cubic strength. This affects the blast radius. Don't change!
		d = d*d*d;
		d = std::min(1.0f, d);
		d *= strength;
		//mLog.Infof("Explosion for %s with strength %f at (%f;%f;%f."), pObject->GetClassId().c_str(), d, pPosition.x, pPosition.y, pPosition.z);
		force += d;

		if (force_factor) {
			const float ff = force_factor * geometry->GetMass() * d;
			f *= ff;
			physics_manager->AddForceAtPos(geometry->GetBodyId(), f, epicenter);
		}
	}
	force /= (bone_count-bone_start);
	return force;
}



loginstance(kGame, Explosion);



}
