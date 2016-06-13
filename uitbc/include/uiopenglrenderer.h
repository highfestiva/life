
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

#pragma once

#include "../../lepra/include/canvas.h"
#include "../../lepra/include/cyclicarray.h"
#include "../../lepra/include/graphics2d.h"
#include "../../lepra/include/hashset.h"
#include "../../lepra/include/hashtable.h"
#include "../../lepra/include/idmanager.h"
#include "../../lepra/include/lepra.h"
#include "../../lepra/include/transformation.h"
#include "../../uilepra/include/uiopenglextensions.h"
#include "uirenderer.h"
#include "uishadowvolume.h"
#include "uitbc.h"
#include <list>



namespace uitbc {



class OpenGLRenderer: public Renderer {
	typedef Renderer Parent;
public:

	class OGLGeometryData: public GeometryData {
	public:
		OGLGeometryData() {
			LEPRA_INSTANCE_CLEAR(&vertex_buffer_id_, (char*)(&bitangent_offset_)+sizeof(size_t));
		}

		virtual void CopyReferenceData(GeometryData* geom) {
			GeometryData::CopyReferenceData(geom);
			OGLGeometryData* _geom = (OGLGeometryData*)geom;
			LEPRA_INSTANCE_COPY(&vertex_buffer_id_, (char*)(&bitangent_offset_)+sizeof(size_t), &_geom->vertex_buffer_id_);
		}

		// Access to the OpenGL buffer object data.
		int vertex_buffer_id_;
		int index_buffer_id_;
		size_t vertex_offset_;
		size_t index_offset_[2];
		size_t index_count_[2];
		size_t normal_offset_;
		size_t color_offset_;
		size_t uv_offset_;
		size_t tangent_offset_;   // Used with bump/normal mapping.
		size_t bitangent_offset_; // Used with bump/normal mapping.
	};

	OpenGLRenderer(Canvas* screen);
	virtual ~OpenGLRenderer();

	// Overloads from Renderer.
	void Clear(unsigned clear_flags = kClearColorbuffer | kClearDepthbuffer);
	void SetClearColor(const Color& color);

	virtual bool IsPixelShadersEnabled() const;

	void SetViewFrustum(float fov_angle, float cam_near, float cam_far);

	void SetShadowMode(Shadows shadow_mode, ShadowHint hint);
	void SetDepthWriteEnabled(bool enabled);
	void SetDepthTestEnabled(bool enabled);
	void SetLightsEnabled(bool enabled);
	void SetTexturingEnabled(bool enabled);
	virtual void SetLineWidth(float pixels);

	void SetAmbientLight(float red, float green, float blue);
	void AddAmbience(float red, float green, float blue);

	virtual LightID AddDirectionalLight(LightHint hint,
		const vec3& dir,
		const vec3& color,
		float shadow_range);

	virtual LightID AddPointLight(LightHint hint,
		const vec3& pos,
		const vec3& color,
		float light_radius,
		float shadow_range);

	virtual LightID AddSpotLight(LightHint hint,
		const vec3& pos,
		const vec3& dir,
		const vec3& color,
		float cutoff_angle,
		float spot_exponent,
		float light_radius,
		float shadow_range);

	virtual void RemoveLight(LightID light_id);
	virtual void EnableAllLights(bool enable);

	void SetLightPosition(LightID light_id, const vec3& pos);
	void SetLightDirection(LightID light_id, const vec3& dir);

	virtual TextureID AddTexture(Texture* texture);

	void UpdateGeometry(GeometryID geometry_id, bool force);
	bool ChangeMaterial(GeometryID geometry_id, MaterialType material_type);

	virtual unsigned RenderScene();
	virtual void RenderBillboards(tbc::GeometryBase* geometry, bool render_texture, bool addative_blending, const BillboardRenderInfoArray& billboards);
	virtual void RenderRelative(tbc::GeometryBase* geometry, const quat* light_orientation);

	// Only used by the OpenGL material classes.
	int GetEnvMapID();
	int GetNumTextureUnits() const;

	void SetGlobalMaterialReflectance(float red, float green, float blue, float specularity);

	bool PreRender(tbc::GeometryBase* geometry);
	void PostRender(tbc::GeometryBase* geometry);

	virtual void DrawLine(const vec3& position, const vec3& vector, const Color& color);

protected:
	virtual void DoSetClippingRect(const PixelRect& rect);
	virtual void DoSetViewport(const PixelRect& viewport);

	// Overloads from HardwareRenderer
	int ReleaseShadowMap(int shadow_map_id);

	TextureData* CreateTextureData(TextureID texture_id);
	GeometryData* CreateGeometryData();

	void BindMap(int map_type, TextureData* texture_data, Texture* texture);
	void BindCubeMap(TextureData* texture_data, Texture* texture);
	void ReleaseMap(TextureData* texture_data);

	const Canvas* GetMap(int map_type, int mip_map_level, Texture* user_texture);

	void BindGeometry(tbc::GeometryBase* user_geometry, GeometryID id, MaterialType material_type);
	bool BindShadowGeometry(uitbc::ShadowVolume* shadow_geometry, LightHint light_hint);
	void ReleaseGeometry(tbc::GeometryBase* user_geometry, GeomReleaseOption option);
	void AppendWireframeLines(tbc::GeometryBase* geometry);

private:

	Material* CreateMaterial(MaterialType material_type);

	void ProcessLights();
	void SetupGLLight(int light_index, const LightData* light);

	// Shadow volumes are used with directional lights and point lights.
	void RenderShadowVolumes();

	// Shadow maps are used with spot lights. Requires OpenGL 1.4.
	// RenderShadowMaps() returns the number of spotlights currently in the
	// scene.
	int RenderShadowMaps();
	void RegenerateShadowMap(LightData* light);

	// Doing about the same thing as gluPerspective()...
	// gluPerspective() want's the vertical FOV-angle - between the top and
	// the bottom frustum planes (top and bottom screen edges), since a wide-
	// screen monitor should result in a wider view (thus, the height fov
	// remains constant).
	// Here's a function that does it the other way around.
	// fov_angle is the angle in degrees between the left and the right
	// frustum planes (left and right screen edges).
	void Perspective(float fov_angle, float aspect_ratio, float cam_near, float cam_far);

	void SetPixelFormat(int& size, GLenum& pixel_format, bool compress, const str& error_message);

	IdManager<int> buffer_id_manager_;
	IdManager<int> t_map_id_manager_;

	GLbitfield gl_clear_mask_;

	int num_texture_units_;	// Number of OpenGL texture units available.

	xform cam_space_transformation_;	// Optimization to not have to run constructor.

	logclass();
};



}
