
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

// This class is a high level interface to the sublevel rendering API (which can be OpenGL, Direct3D or some other API).
// The renderer uses a right handed coordinate system:
//    X is right.
//    Y is forward.
//    Z is up.

#pragma once

#include "../../tbc/include/geometrybase.h"
#include "../../lepra/include/unordered.h"
#include "../../lepra/include/canvas.h"
#include "../../lepra/include/graphics2d.h"
#include "../../lepra/include/idmanager.h"
#include "../../lepra/include/hashset.h"
#include "../../lepra/include/hashtable.h"
#include "../../lepra/include/rotationmatrix.h"
#include "../../lepra/include/transformation.h"
#include "../../lepra/include/vector2d.h"
#include "uitbc.h"
#include "uibillboardrenderinfo.h"
#include "uitexture.h"
#include "uishadowvolume.h"
#include "uiportalmanager.h"



namespace tbc {
class GeometryReference;
}



namespace uitbc {



class DynamicRenderer;
class GeometryGroup;
class Material;



class Renderer : public tbc::GeometryBase::Listener {
public:

	// All supported materials. TODO: Add more.
	// The enumeration order is very important, since
	// the materials are rendered in this order.
	enum MaterialType {
		kMatNull = 0,				// The NULL material doesn't render anything.
		kMatSingleColorSolid,
		kMatSingleColorSolidPxs,		// Single color using pixel shaders.
		kMatVertexColorSolid,
		kMatSingleTextureSolid,
		kMatSingleTextureHighlight,		// Single texture without lighting/specular.
		kMatSingleTextureSolidPxs,		// Single texture using pixel shaders.
		kMatSingleColorEnvmapSolid,
		kMatSingleTextureEnvmapSolid,	// Environtment map + single texture.

		kMatTextureAndLightmap,		// Requires two rendering passes and a multitexture unit.
		kMatTextureAndLightmapPxs,		// Requires pixel shaders.

		kMatTextureAndDiffuseBumpmapPxs,	// Texture + normal map. Faster.
		kMatTextureSbmapPxs,			// Texture + specular map + normal map.

		kMatLastSolid = kMatTextureSbmapPxs,

		kMatSingleTextureAlphatested,

		kMatSingleColorBlended,
		kMatSingleColorOutlineBlended,
		kMatVertexColorBlended,
		kMatSingleTextureBlended,
		kMatSingleColorEnvmapBlended,
		kMatSingleTextureEnvmapBlended,

		kMatCount,
	};

	typedef std::unordered_map<str, DynamicRenderer*> DynamicRendererMap;

	enum LightType {
		kLightInvalid = -1,
		kLightDirectional = 0,
		kLightPoint,
		kLightSpot,
	};

	enum LightHint {
		kLightStatic = 0,
		kLightMovable,
	};

	enum Shadows {
		kForceNoShadows = 0,
		kNoShadows,
		kCastShadows,
		kForceCastShadows,
	};

	enum ShadowHint {
		kShVolumesOnly = 0,	// Use shadow volumes for all light sources.
		kShVolumesAndMaps,	// Use shadow volumes for directional and point lights,
					// shadow maps for spot lights.
	};

	enum {
		kMaxShadowVolumes = 4,	// Max shadow volumes per geometry instance.
		kMaxLights = 8,
	};

	enum { // Clear flags.
		kClearColorbuffer        = (1 << 0),
		kClearDepthbuffer        = (1 << 1),
		kClearStencilbuffer      = (1 << 2),
		kClearAccumulationbuffer = (1 << 3),
	};

	typedef int TextureID;
	typedef int GeometryID;
	typedef int LightID;
	static const TextureID INVALID_TEXTURE = 0;
	static const GeometryID INVALID_GEOMETRY = 0;
	static const LightID INVALID_LIGHT = -1;

	class TextureAssociation {
	public:
		struct Maps {
			int map_id_[Texture::kNumMaps];
			int mip_map_level_count_[Texture::kNumMaps];
		};

		TextureAssociation(int num_textures);
		~TextureAssociation();

		int num_textures_;
		TextureID* texture_id_; // One ID per texture.
		Maps* maps_; // A bunch of different maps (buffers) per texture.
	};

	class GeometryData {
	public:
		GeometryData();
		virtual ~GeometryData();
		virtual void CopyReferenceData(GeometryData*);

		typedef std::unordered_set<GeometryID, LEPRA_ENUM_HASHER> GeometryIDSet;

		// List of geometries that refer to this geometry.
		GeometryIDSet reference_set_;

		// The ID of this geometry.
		GeometryID geometry_id_;

		// The actual geometry.
		tbc::GeometryBase* geometry_;

		// The associated material.
		MaterialType material_type_;
		GeometryGroup* geometry_group_;

		// The associated texture.
		TextureAssociation* ta_;

		// Shadow volumes and associated light sources.
		GeometryID shadow_volume_[kMaxShadowVolumes];
		int light_id_[kMaxShadowVolumes];

		// Tells wether the geometry is subject to shadow casting.
		Shadows shadow_;

		unsigned last_frame_shadows_updated_;
	};

	class TextureData {
	public:
		TextureData(TextureID texture_id, int invalid_id) :
			width_(0),
			height_(0),
			is_cube_map_(0),
			texture_id_(texture_id) {
			for (int i = 0; i < Texture::kNumMaps; i++) {
				t_map_id_[i] = invalid_id;
				t_mip_map_level_count_[i] = 0;
			}
		}

		// Width and height of texture (highest resolution).
		int width_;
		int height_;

		// Set to true if this texture is actually six textures
		// that defines a cube map.
		bool is_cube_map_;

		// IDs to identify the actual texture. For OpenGL this is
		// simply the associated "texture names". For Direct3D this
		// is the index into an array of texture handles.
		int t_map_id_[Texture::kNumMaps];

		int t_mip_map_level_count_[Texture::kNumMaps];

		TextureID texture_id_;
	};

	class LightData {
	public:

		LightData() :
			type_(Renderer::kLightDirectional),
			enabled_(false),
			transformation_changed_(false),
			radius_(0.0f),
			shadow_range_(0.0f),
			cutoff_angle_(180.0f),
			spot_exponent_(1),
			shadow_map_id_(0),
			shadow_map_res_(0),
			shadow_map_near_(0),
			shadow_map_far_(0),
			shadow_map_need_update_(true) {
			color_[0] = 1.0f;
			color_[1] = 1.0f;
			color_[2] = 1.0f;
			color_[3] = 1.0f;
		}

		typedef HashSet<GeometryData*, LEPRA_VOIDP_HASHER> GeometrySet;

		vec3 position_;
		vec3 direction_;
		LightType type_;
		LightHint hint_;
		bool enabled_;
		bool transformation_changed_;

		float color_[4];
		float radius_;
		float shadow_range_;

		// Spot light & shadow map stuff.
		float cutoff_angle_;
		float spot_exponent_;
		int shadow_map_id_;
		int shadow_map_res_;
		float shadow_map_near_;
		float shadow_map_far_;
		bool shadow_map_need_update_;
		GeometrySet shadow_map_geometry_set_;

		// Used to be GLfloat in OpenGLRenderer...
		float light_projection_matrix_[16];

		RotationMatrixF orientation_;
	};

	Renderer(Canvas* screen);
	virtual ~Renderer();

	void ClearDebugInfo();

	// Implementing the GeometryBase::Listener interface.
	void DeletingGeometry(tbc::GeometryBase* geometry);

	const Canvas* GetScreen() const;

	virtual void Clear(unsigned clear_flags = kClearColorbuffer | kClearDepthbuffer) = 0;
	virtual void SetClearColor(const Color& color);
	const Color& GetClearColor() const;

	DynamicRenderer* GetDynamicRenderer(str name) const;
	void AddDynamicRenderer(str name, DynamicRenderer* renderer);

	virtual void SetOutlineFillColor(const Color& color);
	void EnableOutlineRendering(bool enable);
	bool IsOutlineRenderingEnabled() const;
	void EnableWireframe(bool enable);
	bool IsWireframeEnabled() const;
	void EnablePixelShaders(bool enable);
	virtual bool IsPixelShadersEnabled() const;
	void EnableSolidShadows(bool enable);
	bool IsSolidShadowsEnabled() const;

	// A viewport is a portion of the screen where the graphics will be rendered.
	// The center of the viewport is also the center of perspective. The viewport may
	// be partially outside the actual screen. Note that bottom > top.
	virtual void SetViewport(const PixelRect& viewport);
	const PixelRect& GetViewport() const;

	// A view frustum defines the Field Of View (FOV). The fov_angle is given
	// in degrees and defines the angle between the left and the right edge of the
	// viewport.
	virtual void SetViewFrustum(float fov_angle, float near, float far);
	void GetViewFrustum(float& fov_angle, float& near, float& far) const;

	// The clipping rectangle defines a smaller portion of the viewport where graphics
	// should be rendered. A clipping rect doesn't affect the perspective center.
	// Everything outside the clipping rect will (obviously) be clipped. Note that
	// bottom > top.
	//
	// When REDUCING the clipping rect, the new rectangle will be restricted to the area
	// within the one previously set. This means that setting the clipping rect one time
	// after another will reduce the size of the clipping area each time. Call
	// ResetClippingRect() to set the clipping rect back to cover the entire screen.
	void SetClippingRect(const PixelRect& rect);
	void ReduceClippingRect(const PixelRect& rect);
	void ResetClippingRect();
	const PixelRect& GetClippingRect() const;

	virtual void SetCameraTransformation(const xform& transformation);
	const xform& GetCameraTransformation();
	const xform& GetCameraActualTransformation();
	const quat& GetCameraOrientationInverse();
	const quat& GetCameraActualOrientationInverse();

	virtual void SetLightsEnabled(bool enabled);
	bool GetLightsEnabled();

	virtual void SetMipMappingEnabled(bool enabled);
	bool GetMipMappingEnabled();

	virtual void SetBilinearFilteringEnabled(bool enabled);
	bool GetBilinearFilteringEnabled();

	// Will also activate mip mapping and bilinear.
	virtual void SetTrilinearFilteringEnabled(bool enabled);
	bool GetTrilinearFilteringEnabled();

	virtual void SetTexturingEnabled(bool enabled);
	bool GetTexturingEnabled();

	// Will only affect textures added AFTER calling one of these.
	virtual void SetCompressedTexturesEnabled(bool enabled);
	bool GetCompressedTexturesEnabled();

	virtual void SetShadowMode(Shadows mode, ShadowHint hint);
	Shadows GetShadowMode();
	void SetShadowUpdateIntensity(float update_intensity);
	void SetShadowUpdateFrameDelay(unsigned frame_delay);

	virtual void SetDepthWriteEnabled(bool enabled) = 0;
	virtual void SetDepthTestEnabled(bool enabled) = 0;
	virtual void SetLineWidth(float pixels);

	// These simply forwards the call to Material::SetDepthSortEnabled().
	void SetEnableDepthSorting(bool enabled);

	virtual void SetAmbientLight(float red, float green, float blue);
	void GetAmbientLight(float& red, float& green, float& blue);
	// Adds ambient light. Used to implement self illumination in standard materials.
	virtual void AddAmbience(float red, float green, float blue);
	// Sets the ambient light back to the value set in the last call to SetAmbientLight().
	void ResetAmbientLight(bool propagate);

	// Adds a light to the scene.
	// light_radius tells how close an object must be before it starts casting any
	// shadows. shadow_range tells how far to extrude the shadow volumes relative
	// to the light source.
	virtual LightID AddDirectionalLight(LightHint hint, // Helps the renderer to optimize shadow rendering.
		const vec3& dir,
		const vec3& color,
		float shadow_range);

	virtual LightID AddPointLight(LightHint hint, // Helps the renderer to optimize shadow rendering.
		const vec3& pos,
		const vec3& color,
		float light_radius,
		float shadow_range);

	virtual LightID AddSpotLight(LightHint hint, // Helps the renderer to optimize shadow rendering.
		const vec3& pos,
		const vec3& dir,
		const vec3& color,
		float cutoff_angle,
		float spot_exponent,
		float light_radius,
		float shadow_range);

	virtual void RemoveLight(LightID light_id);
	virtual void EnableAllLights(bool enable) = 0;
	void RemoveAllLights();
	int GetMaxLights();
	int GetLightCount();

	// Set the shadow map options for one specific spot light.
	virtual void SetShadowMapOptions(LightID light_id,
					 unsigned char log2_res,
					 float near_plane,
					 float far_plane);
	void SetShadowRange(LightID light_id, float shadow_range);

	virtual void SetLightPosition(LightID light_id, const vec3& pos);
	virtual void SetLightDirection(LightID light_id, const vec3& dir);
	virtual void SetLightColor(LightID light_id, const vec3& color);
	vec3 GetLightPosition(LightID light_id) const;
	vec3 GetLightDirection(LightID light_id) const;
	vec3 GetLightColor(LightID light_id) const;

	LightType GetLightType(LightID light_id);
	float GetLightCutoffAngle(LightID light_id);
	float GetLightSpotExponent(LightID light_id);

	// First sort the lights by "influence", then retrieve them one by one
	// by calling GetClosestLight().
	void SortLights(const vec3& reference_position);
	LightID GetClosestLight(int index);

	virtual TextureID AddTexture(Texture* texture);
	virtual void UpdateTexture(TextureID texture_id, Texture* texture);
	virtual void RemoveTexture(TextureID texture_id);

	virtual void SetEnvironmentMap(TextureID texture_id);

	TextureData* GetEnvTexture() const;
	bool IsEnvMapCubeMap(); // Returns true if environment map is a cube map.

	// Adds/updates/removes geometry to/in/from the renderer.
	virtual GeometryID AddGeometry(tbc::GeometryBase* geometry, MaterialType material_type, Shadows shadows);
	virtual bool TryAddGeometryTexture(GeometryID geometry_id, TextureID texture);	// Only adds a texture ID if it hasn't been added already.
	virtual bool DisconnectGeometryTexture(GeometryID geometry_id, TextureID texture);
	virtual void UpdateGeometry(GeometryID geometry_id, bool force) = 0;
	virtual void RemoveGeometry(GeometryID geometry_id);
	virtual void RemoveAllGeometry();
	MaterialType GetMaterialType(GeometryID geometry_id);
	void SetShadows(GeometryID geometry_id, Renderer::Shadows shadow_mode);
	Shadows GetShadows(GeometryID geometry_id);
	virtual bool ChangeMaterial(GeometryID geometry_id, MaterialType material_type) = 0;

	// These are used by the material classes in the context specific renderer.
	// Don't use them elsewhere.
	virtual bool PreRender(tbc::GeometryBase* geometry) = 0;
	virtual void PostRender(tbc::GeometryBase* geometry) = 0;
	virtual bool PreRenderMaterial(MaterialType material_type);
	virtual void PostRenderMaterial(MaterialType material_type);

	void UpdateShadowMaps();
	unsigned UpdateShadowMaps(tbc::GeometryBase* geometry, LightData* closest_light_data);	// Returns the number of triangles calculated for.

	virtual void Tick(float time);
	virtual unsigned RenderScene() = 0;
	virtual void RenderBillboards(tbc::GeometryBase* geometry, bool render_texture, bool addative_blending, const BillboardRenderInfoArray& billboards) = 0;

	// Used for rendering stuff that are NOT in the world, such as
	// 3D-objects in the GUI. The position of the geometry is considered
	// relative to the camera.
	virtual void RenderRelative(tbc::GeometryBase* geometry, const quat* light_orientation) = 0;

	unsigned GetCurrentFrame() const;

	//
	// Special functions implemented to support portal rendering.
	//

	// Prepares projection calculations.
	void PrepareProjectionData();

	// Given a polygon (vertex) in world space, this function returns the
	// projected bounding rectangle in screen coordinates.
	PixelRect GetBoundingRect(const vec3* vertex, int num_vertices) const;

	// Returns true if the front face is turned towards the camera.
	bool IsFacingFront(const vec3* vertex, int num_vertices);
	vec3 ScreenCoordToVector(const PixelCoord& coord) const;
	vec2 PositionToScreenCoord(const vec3& position, float aspect_ratio) const;

	// Returns the triangle count in the scene.
	// Parameter controls counting visible OR culled triangles.
	int GetTriangleCount(bool visible);

	virtual void DrawLine(const vec3& position, const vec3& vector, const Color& color) = 0;

	float GetFogNear() const;
	float GetFogFar() const;
	float GetFogDensity() const;
	float GetFogExponent() const;
	virtual void SetFog(float near, float far, float exponent, float density);

	void CalcCamCulling();
	bool CheckCamCulling(const vec3& position, float bounding_radius) const;

protected:
	enum GeomReleaseOption {
		kGroIgnoreMaterial = 0,
		kGroRemoveFromMaterial
	};

	typedef HashTable<int, GeometryData*> GeometryTable;
	typedef HashTable<int, GeometryData*> ShadowVolumeTable;
	typedef HashTable<int, TextureData*> TextureTable;

	// Call these from the child class' constructor and destructor, respectively.
	void InitRenderer();
	void CloseRenderer();

	virtual void DoSetClippingRect(const PixelRect& rect) = 0;
	virtual void DoSetViewport(const PixelRect& viewport) = 0;

	// Some functions commonly used by the child classes.
	Material* GetMaterial(MaterialType material_type) const;
	GeometryTable& GetGeometryTable();
	ShadowVolumeTable& GetShadowVolumeTable();
	LightData* GetLightData(LightID light_id) const;
	int GetNumSpotLights() const;
	void StepCurrentFrame();
	ShadowHint GetShadowHint() const;

	virtual Material* CreateMaterial(Renderer::MaterialType material_type) = 0;
	virtual GeometryData* CreateGeometryData() = 0;
	virtual TextureData* CreateTextureData(TextureID texture_id) = 0;

	static bool CheckFlag(unsigned flags, unsigned pFlag);
	float GetAspectRatio() const;

	bool CheckCulling(const xform& transform, double bounding_radius);

	virtual void BindMap(int map_type,
			     TextureData* texture_data,
			     Texture* texture) = 0;
	virtual void BindCubeMap(TextureData* texture_data,
				 Texture* texture) = 0;
	virtual void ReleaseMap(TextureData* texture_data) = 0;
	virtual int ReleaseShadowMap(int shadow_map_id) = 0;

	virtual void BindGeometry(tbc::GeometryBase* geometry,
				  GeometryID id,
				  MaterialType material_type) = 0;

	virtual bool BindShadowGeometry(uitbc::ShadowVolume* shadow_geometry, LightHint light_hint) = 0;
	virtual void ReleaseGeometry(tbc::GeometryBase* user_geometry, GeomReleaseOption option) = 0;

	LightID AllocLight();

	void ReleaseShadowVolumes();
	void ReleaseGeometries();
	void ReleaseTextureMaps();
	void ReleaseShadowMaps();
	void RemoveShadowVolumes(GeometryData* owner_geometry);
	void RemoveShadowVolume(GeometryID& shadow_volume);

	void RecalculateFrustumPlanes();

	static int LightCompare(const void* light1, const void* light2);
	static float GetLightInfluence(const LightData& light_data);

	Material* material_[kMatCount];
	DynamicRendererMap dynamic_renderer_map_;
	unsigned current_frame_;

	unsigned visible_triangle_count_;
	unsigned culled_triangle_count_;

	Canvas* screen_;

	Color clear_color_;
	Color outline_fill_color_;

	// Used for portal rendering.
	double dx_;
	double dy_;
	xform cam_transform_;

	// View frustum.
	float fov_angle_;
	float near_;
	float far_;
	vec3 frustum_planes_[4];
	vec3 cam_frustum_planes_[4];

	bool is_outline_render_enabled_;
	bool is_wireframe_enabled_;
	bool is_pixel_shaders_enabled_;
	bool is_solid_shadows_enabled_;

	// Viewport.
	PixelRect viewport_;

	IdManager<int> geometry_id_manager_;
	IdManager<int> texture_id_manager_;
	IdManager<int> light_id_manager_;
	GeometryTable geometry_table_;
	TextureTable texture_table_;
	ShadowVolumeTable shadow_volume_table_;

	TextureData* env_texture_;

	// Lights.
	typedef std::unordered_map<LightID, LightData*> LightDataMap;
	LightDataMap light_data_map_;
	LightID sorted_lights_[kMaxLights];
	int num_spot_lights_;

	float ambient_red_;
	float ambient_green_;
	float ambient_blue_;
	float original_ambient_red_;
	float original_ambient_green_;
	float original_ambient_blue_;

	bool mip_map_enabled_;
	bool bilinear_enabled_;
	bool trilinear_enabled_;
	bool texturing_enabled_;
	bool compressed_textures_enabled_;
	bool lights_enabled_;

	Shadows shadow_mode_;
	ShadowHint shadow_hint_;
	float shadow_update_intensity_;
	unsigned shadow_update_frame_delay_;

	PixelRect clipping_rect_;
	xform camera_transformation_;
	xform camera_actual_transformation_;
	quat camera_orientation_inverse_;
	quat camera_actual_orientation_inverse_;
	quat camera_actual_rotation_;

	int shadow_volumes_create_max_;
	int shadow_volume_create_count_;

	float fog_near_;
	float fog_far_;
	float fog_density_;
	float fog_exponent_;

	float line_width_;

	// Since distance sorting lights is implemented using ::qsort(),
	// we need to store the following variables "globally".
	static Renderer* renderer_;
	static vec3 reference_position_;

	logclass();
};



}
