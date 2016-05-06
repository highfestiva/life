
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../lepra/include/aabr.h"
#include "../../lepra/include/idmanager.h"
#include "../../lepra/include/hashtable.h"
#include "../../lepra/include/vector3d.h"
#include "cure.h"
#include "resourcemanager.h"



namespace tbc {
class TerrainPatch;
}



namespace cure {



class TerrainFunctionManager;
class TerrainPatchManager;



// Supervisor terrain manager. It, and its sidekicks patch and function manager,
// are built around some general principles:
// 1. Terrain manager handles the interface (including thread safety).
// 2. The patch manager handles the actual patches. The patch instances passed
//    around are resources from the resource system. (In this way we get a better
//    global resource cache than if we'd keep one locally in the patch manager.)
// 3. The function manager owns its resources, rather than using those from the
//    resource manager. The reason for this is that functions are loaded directly
//    when required by a patch, since it in turn is loaded by the resource system.
//    Therefore, latency when loading a function is not a problem. We do not need
//    a resource cache of terrain functions either, because the patches in the
//    managers cache provide already generated terrain data.
class TerrainManager {
public:
	typedef int CameraId;
	typedef AABR<float> PatchArea;

	TerrainManager(ResourceManager* resource_manager);
	virtual ~TerrainManager();
	void Clear();

	static tbc::TerrainPatch* CreatePatch(const PatchArea& area, float lod);
	static void DeletePatch(tbc::TerrainPatch* patch);

	CameraId AddCamera(const vec3& position, float visible_radius);
	void RemoveCamera(CameraId camera_id);
	void MoveCamera(CameraId camera_id, const vec3& position);

	void GetLoadCount(int& load_patch_ok_count, int& load_patch_error_count);

private:
	void UpdatePatchTree();
	void TerrainPatchLoadCallback(UserPhysicalTerrainResource* loaded_resource);

	Lock terrain_lock_;

	ResourceManager* resource_manager_;

	int load_patch_ok_count_;
	int load_patch_error_count_;

	TerrainPatchManager* patch_manager_;

	typedef IdManager<CameraId> CameraIdManager;
	CameraIdManager camera_id_manager_;

	struct CameraInfo {
		inline CameraInfo(const vec3& position, float visible_radius):
			position_(position),
			visible_radius_(visible_radius) {
		}
		vec3 position_;
		float visible_radius_;
	};
	typedef HashTable<CameraId, CameraInfo> CameraTable;
	CameraTable camera_table_;

	logclass();
};



}
