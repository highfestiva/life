
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../cure/include/cppcontextobject.h"
#include "../../thirdparty/FastDelegate/FastDelegate.h"
#include "uiresourcemanager.h"
#include "uicure.h"



namespace tbc {
class GeometryBase;
}
namespace cure {
class ResourceManager;
}
namespace uitbc {
class ChunkyClass;
}



namespace UiCure {



class CppContextObject: public cure::CppContextObject {
	typedef cure::CppContextObject Parent;
public:
	typedef fastdelegate::FastDelegate1<UserGeometryReferenceResource*, void> PostLoadMaterialDelegate;

	CppContextObject(cure::ResourceManager* resource_manager, const str& class_id, GameUiManager* ui_manager);
	virtual ~CppContextObject();

	void EnableUi(bool enable);
	void EnableRootShadow(bool enable);
	void SetUseDefaultTexture(bool use_default_texture);
	void EnablePixelShader(bool enable);
	void EnableMeshMove(bool enable);
	void EnableMeshSlide(bool enable);
	void SetPositionFinalized();

	virtual void StartLoading();

	void OnTick();
	virtual void UiMove();
	void ActivateLerp();
	void SetSinking(float sink_speed);
	void ShrinkMeshBigOrientationThreshold(float threshold);

	tbc::GeometryBase* GetMesh(int index) const;
	UserGeometryReferenceResource* GetMeshResource(int index) const;
	void AddMeshResource(tbc::GeometryBase* mesh, int casts_shadows);
	void AddMeshResourceRef(const str& mesh_name, int casts_shadows);
	void CenterMeshes();
	void LoadTexture(const str& name, Canvas::ResizeHint resize_hint=Canvas::kResizeFast);
	virtual void UpdateMaterial(int mesh_index);
	void SetPostLoadMaterialDelegate(const PostLoadMaterialDelegate& delegate);

	virtual void ReplaceTexture(int texture_index, const str& new_texture_name);

	GameUiManager* GetUiManager() const;
	virtual const tbc::ChunkyClass* GetClass() const;

protected:
	enum MeshSlideMode {
		kLerpStop,
		kLerpStart,
		kLerpRun,
	};

	void DoAddMeshResource(const str& mesh_name, tbc::GeometryBase* mesh, int casts_shadows);

	void OnLoadClass(UserClassResource* class_resource);
	virtual void LoadTextures();
	void OnLoadMesh(UserGeometryReferenceResource* mesh_resource);
	virtual void DispatchOnLoadMesh(UserGeometryReferenceResource* mesh_resource);
	void OnLoadTexture(UserRendererImageResource* texture_resource);
	virtual void TryAddTexture();
	virtual bool TryComplete();
	virtual str GetMeshInstanceId() const;

	typedef std::vector<UserGeometryReferenceResource*> MeshArray;
	typedef std::vector<UserRendererImageResource*> TextureArray;

	GameUiManager* ui_manager_;
	UserClassResource* ui_class_resource_;
	bool enable_ui_;
	bool allow_root_shadow_;
	bool use_default_texture_;
	bool enable_pixel_shader_;
	bool enable_mesh_move_;
	bool enable_mesh_slide_;
	float sink_speed_;
	float sink_offset_;
	MeshArray mesh_resource_array_;
	size_t mesh_load_count_;
	TextureArray texture_resource_array_;
	size_t texture_load_count_;
	xform lerp_offset_;
	MeshSlideMode lerp_mode_;
	PostLoadMaterialDelegate post_load_material_delegate_;

	logclass();
};



}
