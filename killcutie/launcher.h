
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../uicure/include/uiprops.h"
#include "game.h"



namespace grenaderun {



class Launcher: public UiCure::Props {
	typedef UiCure::Props Parent;
public:
	Launcher(Game* game);
	virtual ~Launcher();

	void SetBarrelAngle(float yaw, float pitch);

	void GetAngles(const cure::ContextObject* target, float& pitch, float& guide_pitch,
		float& yaw, float& guide_yaw) const;
	void GetAngles(const vec3& target_position, const vec3& target_velocity,
		float& pitch, float& guide_pitch, float& yaw, float& guide_yaw) const;

	void CreateEngines();

private:
	void GetBallisticData(const vec3& position1, const vec3& position2,
		float pitch, float& guide_pitch, float& guide_yaw, float &time) const;
	void DispatchOnLoadMesh(UiCure::UserGeometryReferenceResource* mesh_resource);

	Game* game_;
};



}
