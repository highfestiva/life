/*
	Class:  Renderer
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#include "pch.h"
#include "../include/uirenderer.h"
#include "../../lepra/include/hashutil.h"
#include "../../lepra/include/log.h"
#include "../../lepra/include/math.h"
#include "../../lepra/include/random.h"
#include "../../lepra/include/resourcetracker.h"
#include "../../tbc/include/geometryreference.h"
#include "../include/uidynamicrenderer.h"
#include "../include/uimaterial.h"



namespace uitbc {



Renderer::TextureAssociation::TextureAssociation(int num_textures):
	num_textures_(num_textures),
	texture_id_(0),
	maps_(0) {
	LEPRA_ACQUIRE_RESOURCE(TextureAssociation);

	texture_id_ = new TextureID[num_textures_];
	maps_ = new Maps[num_textures_];
	for (int i = 0; i < num_textures_; i++) {
		texture_id_[i] = INVALID_TEXTURE;
		maps_[i].map_id_[Texture::kColorMap]    = 0;
		maps_[i].map_id_[Texture::kAlphaMap]    = 0;
		maps_[i].map_id_[Texture::kNormalMap]   = 0;
		maps_[i].map_id_[Texture::kSpecularMap] = 0;
		maps_[i].map_id_[Texture::kCubeMap] = 0;
		maps_[i].mip_map_level_count_[Texture::kColorMap]    = 0;
		maps_[i].mip_map_level_count_[Texture::kAlphaMap]    = 0;
		maps_[i].mip_map_level_count_[Texture::kNormalMap]   = 0;
		maps_[i].mip_map_level_count_[Texture::kSpecularMap] = 0;
		maps_[i].mip_map_level_count_[Texture::kCubeMap] = 0;
	}
}

Renderer::TextureAssociation::~TextureAssociation() {
	delete[] maps_;
	delete[] texture_id_;
	LEPRA_RELEASE_RESOURCE(TextureAssociation);
};



Renderer::GeometryData::GeometryData():
	geometry_id_(INVALID_GEOMETRY),
	geometry_(0),
	material_type_(Renderer::kMatSingleColorSolid),
	geometry_group_(0),
	ta_(0),
	shadow_(kNoShadows),
	last_frame_shadows_updated_(0) {
	LEPRA_ACQUIRE_RESOURCE(GeometryData);
	for (int i = 0; i < Renderer::kMaxShadowVolumes; i++) {
		shadow_volume_[i] = INVALID_GEOMETRY;
		light_id_[i] = INVALID_LIGHT;
	}
}

Renderer::GeometryData::~GeometryData() {
	LEPRA_RELEASE_RESOURCE(GeometryData);
}

void Renderer::GeometryData::CopyReferenceData(GeometryData*) {
}



Renderer::Renderer(Canvas* screen):
	current_frame_(0),
	visible_triangle_count_(0),
	culled_triangle_count_(0),
	screen_(screen),
	dx_(1),
	dy_(1),
	fov_angle_(90.0f),
	near_(0.1f),
	far_(10000.0f),
	is_outline_render_enabled_(false),
	is_wireframe_enabled_(false),
	is_pixel_shaders_enabled_(true),
	viewport_(0, 0, screen->GetWidth(), screen->GetHeight()),
	geometry_id_manager_(1, 1000000, INVALID_GEOMETRY),
	texture_id_manager_(1, 1000000, INVALID_TEXTURE),
	light_id_manager_(0, kMaxLights, INVALID_LIGHT),
	env_texture_(0),
	num_spot_lights_(0),
	ambient_red_(0.2f),
	ambient_green_(0.2f),
	ambient_blue_(0.2f),
	original_ambient_red_(0.2f),
	original_ambient_green_(0.2f),
	original_ambient_blue_(0.2f),
	mip_map_enabled_(false),
	bilinear_enabled_(false),
	trilinear_enabled_(false),
	compressed_textures_enabled_(false),
	lights_enabled_(false),
	shadow_mode_(kNoShadows),
	shadow_hint_(Renderer::kShVolumesOnly),
	shadow_update_intensity_(1.0f),
	shadow_update_frame_delay_(100),
	clipping_rect_(0, 0, screen->GetWidth(), screen->GetHeight()),
	shadow_volumes_create_max_(1),
	shadow_volume_create_count_(0),
	fog_near_(0),
	fog_far_(0),
	fog_density_(1),
	fog_exponent_(0),
	line_width_(1) {
	camera_actual_rotation_.RotateAroundOwnX(PIF/2);	// Default for OpenGL.
}

Renderer::~Renderer() {
}

void Renderer::ClearDebugInfo() {
	visible_triangle_count_ = 0;
	culled_triangle_count_ = 0;
}

void Renderer::InitRenderer() {
	for (int i = 0; i < kMatCount; i++) {
		material_[i] = CreateMaterial((MaterialType)i);
	}
}

void Renderer::CloseRenderer() {
	DynamicRendererMap::iterator x = dynamic_renderer_map_.begin();
	for (; x != dynamic_renderer_map_.end(); ++x) {
		DynamicRenderer* _renderer = x->second;
		delete _renderer;
	}
	dynamic_renderer_map_.clear();

	ReleaseGeometries();
	ReleaseShadowVolumes();
	ReleaseTextureMaps();
	ReleaseShadowMaps();

	for (int i = 0; i < (int)kMatCount; i++) {
		if (material_[i] != 0) {
			delete material_[i];
			material_[i] = 0;
		}
	}
}

void Renderer::DeletingGeometry(tbc::GeometryBase* geometry) {
	GeometryData* geometry_data = (GeometryData*)geometry->GetRendererData();
	if(geometry_data != 0) {
		log_.Warningf("Geometry with GeometryID %i is deleted before removed from Renderer.", geometry_data->geometry_id_);
		RemoveGeometry(geometry_data->geometry_id_);
	}
}

const Canvas* Renderer::GetScreen() const {
	return screen_;
}



void Renderer::SetClearColor(const Color& color) {
	clear_color_ = color;
}

const Color& Renderer::GetClearColor() const {
	return clear_color_;
}



DynamicRenderer* Renderer::GetDynamicRenderer(str name) const {
	return HashUtil::FindMapObject(dynamic_renderer_map_, name);
}

void Renderer::AddDynamicRenderer(str name, DynamicRenderer* renderer) {
	if (!HashUtil::FindMapObject(dynamic_renderer_map_, name)) {
		dynamic_renderer_map_.insert(DynamicRendererMap::value_type(name, renderer));
	} else {
		delete renderer;
	}
}



void Renderer::SetOutlineFillColor(const Color& color) {
	outline_fill_color_ = color;
}

void Renderer::EnableOutlineRendering(bool enable) {
	is_outline_render_enabled_ = enable;
}

bool Renderer::IsOutlineRenderingEnabled() const {
	return (is_outline_render_enabled_);
}

void Renderer::EnableWireframe(bool enable) {
	is_wireframe_enabled_ = enable;
}

bool Renderer::IsWireframeEnabled() const {
	return (is_wireframe_enabled_);
}

void Renderer::EnablePixelShaders(bool enable) {
	is_pixel_shaders_enabled_ = enable;
}

bool Renderer::IsPixelShadersEnabled() const {
	return (is_pixel_shaders_enabled_);
}

void Renderer::SetViewport(const PixelRect& viewport) {
	deb_assert(viewport.left_ <= viewport.right_ &&
		viewport.top_ <= viewport.bottom_);
	viewport_ = viewport;
	RecalculateFrustumPlanes();

	if (screen_->GetOutputRotation()%180 != 0) {
		PixelRect _rect(viewport);
		std::swap(_rect.left_, _rect.top_);
		std::swap(_rect.right_, _rect.bottom_);
		DoSetViewport(_rect);
	} else {
		DoSetViewport(viewport);
	}
}

const PixelRect& Renderer::GetViewport() const {
	return viewport_;
}

void Renderer::SetViewFrustum(float fov_angle, float near, float far) {
	if (fov_angle <= 0 || fov_angle >= 180 || near >= far) {
		return;
	}
	fov_angle_ = fov_angle;
	near_ = near;
	far_ = far;
	RecalculateFrustumPlanes();
}

void Renderer::GetViewFrustum(float& fov_angle, float& near, float& far) const {
	fov_angle = fov_angle_;
	near = near_;
	far = far_;
}

void Renderer::RecalculateFrustumPlanes() {
	const float __y = Math::Deg2Rad(fov_angle_*0.5f);
	const float __x = asin(sin(__y) * GetAspectRatio());

	frustum_planes_[0].Set(-(float)cos(__x), (float)sin(__x), 0); // Right plane.
	frustum_planes_[1].Set( (float)cos(__x), (float)sin(__x), 0); // Left plane.

	frustum_planes_[2].Set(0, (float)sin(__y), -(float)cos(__y)); // Bottom plane.
	frustum_planes_[3].Set(0, (float)sin(__y),  (float)cos(__y)); // Top plane.
}

void Renderer::SetClippingRect(const PixelRect& rect) {
	clipping_rect_ = rect;
	if (screen_->GetOutputRotation()%180 != 0) {
		PixelRect _rect(rect);
		std::swap(_rect.left_, _rect.top_);
		std::swap(_rect.right_, _rect.bottom_);
		DoSetClippingRect(_rect);
	} else {
		DoSetClippingRect(rect);
	}
}

void Renderer::ReduceClippingRect(const PixelRect& rect) {
	SetClippingRect(clipping_rect_.GetOverlap(rect));
}

void Renderer::ResetClippingRect() {
	SetClippingRect(PixelRect(0, 0, screen_->GetWidth(), screen_->GetHeight()));
}

const PixelRect& Renderer::GetClippingRect() const {
	return clipping_rect_;
}

void Renderer::SetCameraTransformation(const xform& transformation) {
	camera_transformation_ = transformation;
	camera_actual_transformation_ = transformation;
	camera_actual_transformation_.orientation_ *= camera_actual_rotation_;
	camera_orientation_inverse_ = camera_transformation_.orientation_.GetInverse();
	camera_actual_orientation_inverse_ = camera_actual_transformation_.orientation_.GetInverse();
}

const xform& Renderer::GetCameraTransformation() {
	return camera_transformation_;
}

const xform& Renderer::GetCameraActualTransformation() {
	return camera_actual_transformation_;
}

const quat& Renderer::GetCameraOrientationInverse() {
	return camera_orientation_inverse_;
}

const quat& Renderer::GetCameraActualOrientationInverse() {
	return camera_actual_orientation_inverse_;
}

void Renderer::SetLightsEnabled(bool enabled) {
	lights_enabled_ = enabled;
}

bool Renderer::GetLightsEnabled() {
	return lights_enabled_;
}

void Renderer::SetMipMappingEnabled(bool enabled) {
	mip_map_enabled_ = enabled;
}

bool Renderer::GetMipMappingEnabled() {
	return mip_map_enabled_;
}

void Renderer::SetBilinearFilteringEnabled(bool enabled) {
	bilinear_enabled_ = enabled;
}

bool Renderer::GetBilinearFilteringEnabled() {
	return bilinear_enabled_;
}

void Renderer::SetTrilinearFilteringEnabled(bool enabled) {
	trilinear_enabled_ = enabled;
}

bool Renderer::GetTrilinearFilteringEnabled() {
	return trilinear_enabled_;
}

void Renderer::SetTexturingEnabled(bool enabled) {
	texturing_enabled_ = enabled;
}

bool Renderer::GetTexturingEnabled() {
	return texturing_enabled_;
}

void Renderer::SetCompressedTexturesEnabled(bool enabled) {
	compressed_textures_enabled_ = enabled;
}

bool Renderer::GetCompressedTexturesEnabled() {
	return compressed_textures_enabled_;
}

void Renderer::SetShadowMode(Shadows shadow_mode, ShadowHint hint) {
	shadow_mode_ = shadow_mode;
	shadow_hint_ = hint;
}

Renderer::Shadows Renderer::GetShadowMode() {
	return 	shadow_mode_;
}

void Renderer::SetShadowUpdateIntensity(float update_intensity) {
	shadow_update_intensity_ = update_intensity;
}

void Renderer::SetShadowUpdateFrameDelay(unsigned frame_delay) {
	shadow_update_frame_delay_ = frame_delay;
}

void Renderer::SetLineWidth(float pixels) {
	line_width_ = pixels;
}

void Renderer::SetEnableDepthSorting(bool enabled) {
	Material::SetEnableDepthSorting(enabled);
}

void Renderer::SetAmbientLight(float red, float green, float blue) {
	original_ambient_red_   = ambient_red_   = red;
	original_ambient_green_ = ambient_green_ = green;
	original_ambient_blue_  = ambient_blue_  = blue;
}

void Renderer::GetAmbientLight(float& red, float& green, float& blue) {
	red   = ambient_red_;
	green = ambient_green_;
	blue  = ambient_blue_;
}

void Renderer::AddAmbience(float red, float green, float blue) {
	ambient_red_   += red;
	ambient_green_ += green;
	ambient_blue_  += blue;
}

void Renderer::ResetAmbientLight(bool propagate) {
	if (propagate) {
		SetAmbientLight(original_ambient_red_, original_ambient_green_, original_ambient_blue_);
	} else {
		Renderer::SetAmbientLight(original_ambient_red_, original_ambient_green_, original_ambient_blue_);
	}
}

Renderer::LightID Renderer::AllocLight() {
	LightID _light_id = (LightID)light_id_manager_.GetFreeId();
	if (_light_id != INVALID_LIGHT &&
		!GetLightData(_light_id)) {
		sorted_lights_[GetLightCount()] = _light_id;	// Put new light at lowest priority.
		light_data_map_.insert(LightDataMap::value_type(_light_id, new LightData));
	}
	return _light_id;
}

Renderer::LightID Renderer::AddDirectionalLight(LightHint hint,
		const vec3& dir,
		const vec3& color,
		float shadow_range) {
	LightID _light_id = AllocLight();
	if (_light_id == INVALID_LIGHT)
		return INVALID_LIGHT;

	LightData* data = GetLightData(_light_id);
	deb_assert(data);
	data->type_			= kLightDirectional;
	data->hint_			= hint;
	data->position_		= vec3();
	data->direction_		= dir.GetNormalized();
	data->color_[0]		= color.x;
	data->color_[1]		= color.y;
	data->color_[2]		= color.z;
	data->radius_			= 0;
	data->shadow_range_		= shadow_range;
	data->cutoff_angle_		= 89;
	data->spot_exponent_		= 1;
	data->transformation_changed_	= true;
	data->enabled_			= true;

	return _light_id;
}

Renderer::LightID Renderer::AddPointLight(LightHint hint,
		const vec3& pos,
		const vec3& color,
		float light_radius,
		float shadow_range) {
	LightID _light_id = AllocLight();
	if (_light_id == INVALID_LIGHT)
		return INVALID_LIGHT;

	LightData* data = GetLightData(_light_id);
	deb_assert(data);
	data->type_			= kLightPoint;
	data->hint_			= hint;
	data->position_		= pos;
	data->direction_		= vec3();
	data->color_[0]		= color.x;
	data->color_[1]		= color.y;
	data->color_[2]		= color.z;
	data->radius_			= light_radius;
	data->shadow_range_		= shadow_range;
	data->cutoff_angle_		= 89;
	data->spot_exponent_		= 1;
	data->transformation_changed_	= true;
	data->enabled_			= true;

	return _light_id;
}

Renderer::LightID Renderer::AddSpotLight(LightHint hint,
		const vec3& pos,
		const vec3& dir,
		const vec3& color,
		float cutoff_angle,
		float spot_exponent,
		float light_radius,
		float shadow_range) {
	LightID _light_id = AllocLight();
	if (_light_id == INVALID_LIGHT)
		return INVALID_LIGHT;

	// Clamp cutoff angle to a value between 0 and 89 degrees.
	// OpenGL accepts 90 degrees, but we'll set max to 89 in order to be able
	// to render shadow maps.
	cutoff_angle = fabs(cutoff_angle);
	cutoff_angle = cutoff_angle > 89.0f ? 89.0f : cutoff_angle;

	LightData* data = GetLightData(_light_id);
	deb_assert(data);
	data->type_			= kLightSpot;
	data->hint_			= hint;
	data->position_		= pos;
	data->direction_		= dir.GetNormalized();
	data->color_[0]		= color.x;
	data->color_[1]		= color.y;
	data->color_[2]		= color.z;
	data->radius_			= light_radius;
	data->shadow_range_		= shadow_range;
	data->cutoff_angle_		= cutoff_angle;
	data->spot_exponent_		= spot_exponent;
	data->transformation_changed_	= true;
	data->enabled_		= true;

	const float epsilon = 1e-6f;

	// Generate an orientation for the light.
	vec3 axis_x;
	vec3 axis_y;
	vec3 axis_z;

	// If light direction is pointing up.
	if (data->direction_.y >= (1.0f - epsilon)) {
		axis_x = data->direction_ / vec3(0, 0, 1);
		axis_y = axis_x / data->direction_;
		axis_z = data->direction_;
	} else {
		axis_x = data->direction_ / vec3(0, 1, 0);
		axis_y = axis_x / data->direction_;
		axis_z = data->direction_;
	}

	axis_x.Normalize();
	axis_y.Normalize();
	axis_z.Normalize();

	data->orientation_.SetAxisX(axis_x);
	data->orientation_.SetAxisY(axis_y);
	data->orientation_.SetAxisZ(axis_z);

	num_spot_lights_++;

	return _light_id;
}

void Renderer::RemoveLight(LightID light_id) {
	LightData* data = GetLightData(light_id);
	deb_assert(data);

	if (data->type_ == Renderer::kLightSpot) {
		data->shadow_map_geometry_set_.RemoveAll();
		if (data->shadow_map_id_ != 0) {
			data->shadow_map_id_ = ReleaseShadowMap(data->shadow_map_id_);
		}
		num_spot_lights_--;
	}

	light_data_map_.erase(light_id);
	delete data;

	light_id_manager_.RecycleId(light_id);
}

void Renderer::RemoveAllLights() {
	while (!light_data_map_.empty()) {
		RemoveLight(light_data_map_.begin()->first);
	}
}

int Renderer::GetMaxLights() {
	return kMaxLights;
}

int Renderer::GetLightCount() {
	return light_data_map_.size();
}

void Renderer::SetShadowMapOptions(LightID light_id,
				   unsigned char log2_res,
				   float near_plane,
				   float far_plane) {
	LightData* data = GetLightData(light_id);
	deb_assert(data);

	// Can't change settings when the shadow map has been generated.
	// TODO: Replace '0' with something else...
	if (data->shadow_map_id_ == 0) {
		data->shadow_map_res_ = (1 << log2_res);
		data->shadow_map_near_ = near_plane;
		data->shadow_map_far_  = far_plane;
	}
}

void Renderer::SetLightPosition(LightID light_id, const vec3& pos) {
	LightData* data = GetLightData(light_id);
	deb_assert(data);

	if (data->type_ == Renderer::kLightPoint ||
	   data->type_ == Renderer::kLightSpot) {
		if (data->position_ != pos) {
			data->transformation_changed_ = true;
		}
		data->position_ = pos;
	}
}

void Renderer::SetLightDirection(LightID light_id, const vec3& dir) {
	LightData* data = GetLightData(light_id);
	deb_assert(data);

	if (data->type_ == Renderer::kLightDirectional ||
	   data->type_ == Renderer::kLightSpot) {
		vec3 prev_dir(data->direction_);
		data->direction_ = dir;
		data->direction_.Normalize();

		if (prev_dir != data->direction_) {
			data->transformation_changed_ = true;
		}

		if (data->type_ == Renderer::kLightSpot) {
			const float epsilon = 1e-6f;

			// Generate an orientation for the light.
			vec3 axis_x;
			vec3 axis_y;
			vec3 axis_z;

			// If light direction is pointing up.
			if (data->direction_.y >= (1.0f - epsilon)) {
				axis_x = data->direction_ / vec3(1, 0, 0);
				axis_y = data->direction_ / axis_x;
				axis_z = data->direction_;
			} else {
				axis_x = vec3(0, 1, 0) / data->direction_;
				axis_y = data->direction_ / axis_x;
				axis_z = data->direction_;
			}

			axis_x.Normalize();
			axis_y.Normalize();
			axis_z.Normalize();

			data->orientation_.SetAxisX(axis_x);
			data->orientation_.SetAxisY(axis_y);
			data->orientation_.SetAxisZ(axis_z);
		}
	}
}

void Renderer::SetLightColor(LightID light_id, const vec3& color) {
	LightData* data = GetLightData(light_id);
	deb_assert(data);

	data->color_[0] = color.x;
	data->color_[1] = color.y;
	data->color_[2] = color.z;
}

vec3 Renderer::GetLightPosition(LightID light_id) const {
	const LightData* data = GetLightData(light_id);
	deb_assert(data);
	if (data) {
		return data->position_;
	}
	return vec3();
}

vec3 Renderer::GetLightDirection(LightID light_id) const {
	const LightData* data = GetLightData(light_id);
	deb_assert(data);
	if (data) {
		return data->direction_;
	}
	return vec3();
}

vec3 Renderer::GetLightColor(LightID light_id) const {
	const LightData* data = GetLightData(light_id);
	deb_assert(data);
	if (data) {
		return vec3(data->color_[0], data->color_[1], data->color_[2]);
	}
	return vec3();
}

Renderer::LightType Renderer::GetLightType(LightID light_id) {
	const LightData* data = GetLightData(light_id);
	deb_assert(data);
	if (data) {
		return data->type_;
	}
	return kLightInvalid;
}

float Renderer::GetLightCutoffAngle(LightID light_id) {
	const LightData* data = GetLightData(light_id);
	deb_assert(data);
	if (data) {
		return data->cutoff_angle_;
	}
	return 180;
}

float Renderer::GetLightSpotExponent(LightID light_id) {
	const LightData* data = GetLightData(light_id);
	deb_assert(data);
	if (data) {
		return data->spot_exponent_;
	}
	return 180;
}

void Renderer::SortLights(const vec3& reference_position) {
	renderer_ = this;
	int i = 0;
	LightDataMap::iterator x = light_data_map_.begin();
	for (; x != light_data_map_.end(); ++i, ++x) {
		sorted_lights_[i] = x->first;
	}
	reference_position_ = reference_position;
	::qsort(sorted_lights_, i, sizeof(sorted_lights_[0]), LightCompare);
}

int Renderer::LightCompare(const void* light1, const void* light2) {
	const LightData* _light1 = renderer_->GetLightData(*(LightID*)light1);
	const LightData* _light2 = renderer_->GetLightData(*(LightID*)light2);

	const float influence1 = GetLightInfluence(*_light1);
	const float influence2 = GetLightInfluence(*_light2);
	if (influence1 < influence2)
		return +1;	// Sort in descending order of influence.
	else if (influence1 > influence2)
		return -1;	// Sort in descending order of influence.
	return 0;
}

float Renderer::GetLightInfluence(const LightData& light_data) {
	float influence = light_data.color_[0] * light_data.color_[0] +
			    light_data.color_[1] * light_data.color_[1] +
			    light_data.color_[2] * light_data.color_[2];
	switch (light_data.type_) {
		case kLightSpot: {
			// It's difficult to predict how much of the target object is within
			// the light cone of a spot light, so make it simple.. If the object
			// is behind the spot light, don't count it.
			if (light_data.direction_.Dot(reference_position_ - light_data.position_) < 0) {
				influence = 0;
			}
		} // TRICKY: Fall through.
		case kLightPoint: {
			influence /= reference_position_.GetDistanceSquared(light_data.position_);
		} // TRICKY: Fall through.
		case kLightDirectional: {
			influence *= 1 + light_data.shadow_range_;
			influence *= light_data.enabled_? 1 : 0;
		} break;
		default: break;
	}
	return (influence);
}

Renderer::LightID Renderer::GetClosestLight(int index) {
	return sorted_lights_[index];
}

Renderer::TextureID Renderer::AddTexture(Texture* texture) {
	int id = texture_id_manager_.GetFreeId();

	if (id != INVALID_TEXTURE) {
		TextureData* _texture = CreateTextureData((TextureID)id);
		texture_table_.Insert(id, _texture);

		_texture->is_cube_map_ = texture->IsCubeMap();

		if (texture->IsCubeMap() == false) {
			_texture->width_ = texture->GetColorMap(0)->GetWidth();
			_texture->height_ = texture->GetColorMap(0)->GetHeight();

			BindMap(Texture::kColorMap, _texture, texture);
			if (texture->GetNormalMap(0) != 0) {
				BindMap(Texture::kNormalMap, _texture, texture);
			}

			if (texture->GetSpecularMap(0) != 0) {
				BindMap(Texture::kSpecularMap, _texture, texture);
			}
		}// End if(lTexture->IsCubeMap() == false)
		else {
			_texture->width_ = texture->GetCubeMapPosX(0)->GetWidth();
			_texture->height_ = texture->GetCubeMapPosX(0)->GetHeight();

			BindCubeMap(_texture, texture);
		}
	}

	return (TextureID)id;
}

void Renderer::UpdateTexture(TextureID texture_id, Texture* texture) {
	TextureTable::Iterator x = texture_table_.Find(texture_id);

	if (x == texture_table_.End()) {
		return;
	}

	TextureData* _texture = *x;

	// Cube map or not, it has to remain the same type...
	deb_assert(_texture->is_cube_map_ == texture->IsCubeMap());

	if (texture->IsCubeMap() == false) {
		BindMap(Texture::kColorMap, _texture, texture);

		if (texture->GetNormalMap(0) != 0) {
			BindMap(Texture::kNormalMap, _texture, texture);
		}

		if (texture->GetSpecularMap(0) != 0) {
			BindMap(Texture::kSpecularMap, _texture, texture);
		}
	} else {
		BindCubeMap(_texture, texture);
	}
}

void Renderer::RemoveTexture(TextureID texture_id) {
	TextureTable::Iterator x = texture_table_.Find(texture_id);

	if (x == texture_table_.End()) {
		return;
	}

	TextureData* _texture = *x;
	texture_table_.Remove(x);
	ReleaseMap(_texture);

	if (_texture == env_texture_) {
		SetEnvironmentMap(INVALID_TEXTURE);
	}

	delete _texture;

	texture_id_manager_.RecycleId(texture_id);
}

void Renderer::SetEnvironmentMap(TextureID texture_id) {
	if (texture_id == INVALID_TEXTURE) {
		env_texture_ = 0;
		return;
	}

	TextureTable::Iterator x = texture_table_.Find(texture_id);
	if (x == texture_table_.End()) {
		return;
	}

	TextureData* _texture = *x;
	env_texture_ = _texture;
}

bool Renderer::IsEnvMapCubeMap() {
	if (env_texture_ != 0)
		return env_texture_->is_cube_map_;
	else
		return false;
}

Renderer::TextureData* Renderer::GetEnvTexture() const {
	return env_texture_;
}

Renderer::GeometryID Renderer::AddGeometry(tbc::GeometryBase* geometry, MaterialType material_type, Shadows shadows) {
	if ((int)material_type < 0 || (int)material_type >= Renderer::kMatCount) {
		log_.Errorf("AddGeometry - Material %i is not a valid material ID!", (int)material_type);
		return (GeometryID)geometry_id_manager_.GetInvalidId();
	}

	int id = geometry_id_manager_.GetFreeId();
	if (id == geometry_id_manager_.GetInvalidId())
		return (GeometryID)id;

	// Make sure there are vertex normals.
	geometry->GenerateVertexNormalData();

	GeometryData* geometry_data = CreateGeometryData();
	geometry_data->geometry_id_ = (GeometryID)id;
	geometry_data->geometry_ = geometry;
	geometry_data->material_type_ = material_type;
	geometry_data->shadow_ = shadows;
	geometry->SetRendererData(geometry_data);

	geometry_table_.Insert(id, geometry_data);

	if (geometry->IsGeometryReference() == true) {
		tbc::GeometryBase* parent_geometry = ((tbc::GeometryReference*)geometry)->GetParentGeometry();
		GeometryData* parent_geometry_data = (GeometryData*)parent_geometry->GetRendererData();
		geometry_data->CopyReferenceData(parent_geometry_data);
		parent_geometry_data->reference_set_.insert((GeometryID)id);
	} else {
		BindGeometry(geometry, (GeometryID)id, material_type);
	}

	if (!material_[material_type]->AddGeometry(geometry)) {
		RemoveGeometry((GeometryID)id);
		id = geometry_id_manager_.GetInvalidId();
	}

	geometry->AddListener(this);

	return (GeometryID)id;
}

bool Renderer::TryAddGeometryTexture(GeometryID geometry_id, TextureID texture) {
	bool ok = false;
	if (geometry_id == INVALID_GEOMETRY) {
		return (false);
	}

	GeometryTable::Iterator geom_iter;
	geom_iter = geometry_table_.Find(geometry_id);
	deb_assert(geom_iter != geometry_table_.End());
	GeometryData* geometry_data = *geom_iter;

	/*JMB
	if (geometry_data->geometry_->IsGeometryReference() == true) {
		log_.Error("Could not add texture to geometry reference!");
		deb_assert(false);
	} else*/
	{
		const int texture_count = Texture::kNumMaps;
		if (!geometry_data->ta_) {
			geometry_data->ta_ = new Renderer::TextureAssociation(texture_count);
		}
		int x;
		ok = true;
		for (x = 0; ok && x < texture_count; ++x) {
			ok = (geometry_data->ta_->texture_id_[x] != texture);
			if (!ok) {
				//deb_assert(ok);
				log_trace("Skipping add of texture to geometry a second time.");
			}
		}
		if (ok) {
			ok = false;
			for (x = 0; !ok && x < texture_count; ++x) {
				if (geometry_data->ta_->texture_id_[x] == INVALID_TEXTURE) {
					geometry_data->ta_->texture_id_[x] = texture;
					TextureData* _texture = *texture_table_.Find(texture);
					geometry_data->ta_->maps_[x].map_id_[Texture::kColorMap]    = _texture->t_map_id_[Texture::kColorMap];
					geometry_data->ta_->maps_[x].map_id_[Texture::kAlphaMap]    = _texture->t_map_id_[Texture::kAlphaMap];
					geometry_data->ta_->maps_[x].map_id_[Texture::kNormalMap]   = _texture->t_map_id_[Texture::kNormalMap];
					geometry_data->ta_->maps_[x].map_id_[Texture::kSpecularMap] = _texture->t_map_id_[Texture::kSpecularMap];
					geometry_data->ta_->maps_[x].mip_map_level_count_[Texture::kColorMap]    = _texture->t_mip_map_level_count_[Texture::kColorMap];
					geometry_data->ta_->maps_[x].mip_map_level_count_[Texture::kAlphaMap]    = _texture->t_mip_map_level_count_[Texture::kAlphaMap];
					geometry_data->ta_->maps_[x].mip_map_level_count_[Texture::kNormalMap]   = _texture->t_mip_map_level_count_[Texture::kNormalMap];
					geometry_data->ta_->maps_[x].mip_map_level_count_[Texture::kSpecularMap] = _texture->t_mip_map_level_count_[Texture::kSpecularMap];
					ok = true;
				}
			}
			if (!ok) {
				log_.Error("Could not add texture - no more slots available!");
				deb_assert(false);
			}
		}
		/*JMB
		if (ok) {
			GeometryData::GeometryIDSet::iterator x;
			for (x = geometry_data->reference_set_.begin(); x != geometry_data->reference_set_.end(); ++x) {
				// Remove references recursively.
				GeometryTable::Iterator y = geometry_table_.Find(*x);
				deb_assert(y != geometry_table_.End());
				if (y != geometry_table_.End()) {
					GeometryData* lGeometryClone = *geom_iter;
					lGeometryClone->CopyReferenceData(geometry_data);
				}
			}
		}*/
	}
	return (ok);
}

bool Renderer::DisconnectGeometryTexture(GeometryID geometry_id, TextureID texture) {
	bool ok = false;
	if (geometry_id == INVALID_GEOMETRY) {
		deb_assert(false);
		return false;
	}

	GeometryTable::Iterator geom_iter;
	geom_iter = geometry_table_.Find(geometry_id);
	deb_assert(geom_iter != geometry_table_.End());
	GeometryData* geometry_data = *geom_iter;

	const int texture_count = Texture::kNumMaps;
	if (!geometry_data->ta_) {
		deb_assert(false);
		return false;
	}
	for (int x = 0; !ok && x < texture_count; ++x) {
		if (geometry_data->ta_->texture_id_[x] == texture) {
			geometry_data->ta_->texture_id_[x] = INVALID_TEXTURE;
			for (int y = 0; y < Texture::kNumMaps; ++y) {
				geometry_data->ta_->maps_[x].map_id_[y] = INVALID_TEXTURE;
				geometry_data->ta_->maps_[x].mip_map_level_count_[y] = 0;
			}
			return true;
		}
	}
	deb_assert(false);
	return false;
}

void Renderer::RemoveGeometry(GeometryID geometry_id) {
	if (geometry_id == INVALID_GEOMETRY) {
		return;
	}

	GeometryTable::Iterator geom_iter;
	geom_iter = geometry_table_.Find(geometry_id);
	if (geom_iter != geometry_table_.End()) {
		GeometryData* geometry_data = *geom_iter;
		geometry_data->geometry_->RemoveListener(this);

		RemoveShadowVolumes(geometry_data);

		// Remove the geometry from all spot lights, in case it's added there.
		for (LightDataMap::iterator x = light_data_map_.begin(); x != light_data_map_.end(); ++x) {
			LightData* light = x->second;
			if (light->type_ == Renderer::kLightSpot) {
				if (light->shadow_map_geometry_set_.Remove(geometry_data) == true)
					light->shadow_map_need_update_ = true;
			}
		}

		geometry_table_.Remove(geom_iter);
		geometry_id_manager_.RecycleId((int)geometry_id);

		if (geometry_data->geometry_->IsGeometryReference()) {
			tbc::GeometryBase* parent_geometry = ((tbc::GeometryReference*)geometry_data->geometry_)->GetParentGeometry();
			GeometryData* parent_geometry_data = (GeometryData*)parent_geometry->GetRendererData();
			parent_geometry_data->reference_set_.erase(geometry_data->geometry_id_);
		}

		delete (geometry_data->ta_);
		geometry_data->ta_ = 0;

		ReleaseGeometry(geometry_data->geometry_, kGroRemoveFromMaterial);

		while (!geometry_data->reference_set_.empty()) {
			// Remove references recursively.
			LEPRA_DEBUG_CODE(const size_t reference_count = geometry_data->reference_set_.size());
			RemoveGeometry(*geometry_data->reference_set_.begin());
			LEPRA_DEBUG_CODE(deb_assert(reference_count == geometry_data->reference_set_.size()+1));
		}

		geometry_data->geometry_->SetRendererData(0);
		delete geometry_data;
	}
}

void Renderer::RemoveAllGeometry() {
	GeometryTable::Iterator x;
	int i;
	for (i = 0, x = geometry_table_.First(); x != geometry_table_.End(); ++i) {
		GeometryTable::Iterator y = x;
		++x;
		RemoveGeometry((GeometryID)y.GetKey());
	}
}

Renderer::MaterialType Renderer::GetMaterialType(GeometryID geometry_id) {
	deb_assert(geometry_id != INVALID_GEOMETRY);
	MaterialType material = kMatNull;
	GeometryTable::Iterator x = geometry_table_.Find(geometry_id);
	deb_assert(x != geometry_table_.End());
	if (x != geometry_table_.End()) {
		GeometryData* geometry_data = *x;
		material = geometry_data->material_type_;
	}
	return (material);
}

void Renderer::SetShadows(GeometryID geometry_id, Renderer::Shadows shadow_mode) {
	GeometryTable::Iterator x = geometry_table_.Find(geometry_id);
	deb_assert(x != geometry_table_.End());
	if (x != geometry_table_.End()) {
		GeometryData* geometry_data = *x;
		geometry_data->shadow_ = shadow_mode;
	}
}

Renderer::Shadows Renderer::GetShadows(GeometryID geometry_id) {
	deb_assert(geometry_id != INVALID_GEOMETRY);
	Shadows _shadows = kNoShadows;
	GeometryTable::Iterator x = geometry_table_.Find(geometry_id);
	deb_assert(x != geometry_table_.End());
	if (x != geometry_table_.End()) {
		GeometryData* geometry_data = *x;
		_shadows = geometry_data->shadow_;
	}
	return (_shadows);
}

bool Renderer::PreRenderMaterial(MaterialType material_type) {
	GetMaterial(material_type)->PreRender();
	return true;
}

void Renderer::PostRenderMaterial(MaterialType material_type) {
	GetMaterial(material_type)->PostRender();
}

void Renderer::UpdateShadowMaps() {
	shadow_volume_create_count_ = 0;	// Reset every frame.

	LightData* _light_data = GetLightData(GetClosestLight(0));
	if (!_light_data) {
		return;
	}
	unsigned triangles_calculated_for = 0;
	bool did_static = false;
	for (int i = 0; i <= (int)kMatLastSolid; i++) {
		tbc::GeometryBase* _geometry = material_[i]->GetFirstGeometry();
		while (_geometry) {
			if (_geometry->GetAlwaysVisible() || _geometry->GetLastFrameVisible() == current_frame_) {
				const tbc::GeometryBase::GeometryVolatility volatility = _geometry->GetGeometryVolatility();
				bool update = (triangles_calculated_for < 1000 || volatility >= tbc::GeometryBase::kGeomSemiStatic || !did_static);
				if (update) {
					triangles_calculated_for += UpdateShadowMaps(_geometry, _light_data);
					if (volatility == tbc::GeometryBase::kGeomStatic) {
						did_static = true;
					}
				}
			}
			_geometry = material_[i]->GetNextGeometry();
		}
	}
}

unsigned Renderer::UpdateShadowMaps(tbc::GeometryBase* geometry, LightData* closest_light_data) {
	GeometryData* _geometry = (GeometryData*)geometry->GetRendererData();
	const float light_shadow_range = closest_light_data->shadow_range_ * 4;
	const bool deny_shadows =
		(_geometry->shadow_ == kForceNoShadows || shadow_mode_ <= kNoShadows ||
		_geometry->material_type_ == kMatNull ||
		geometry->GetTransformation().GetPosition().GetDistanceSquared(camera_transformation_.GetPosition()) >= light_shadow_range*light_shadow_range);
	const bool force_shadows = (shadow_mode_ == kForceCastShadows);
	const bool escape_shadows = (_geometry->shadow_ == kNoShadows);
	if (deny_shadows || (!force_shadows && escape_shadows)) {
		if (_geometry->shadow_volume_[0]) {
			RemoveShadowVolumes(_geometry);
		}
		return 0;
	}

	if (_geometry->shadow_volume_[0] == 0) {
		if (++shadow_volume_create_count_ > shadow_volumes_create_max_) {
			return 0;	// Better luck next frame.
		}
	}

	SortLights(geometry->GetTransformation().GetPosition());

	const int light_count = GetLightCount();
	if (shadow_hint_ == Renderer::kShVolumesAndMaps) {
		for (int i = 0; i < light_count; i++) {
			LightData* _light_data = (i==0)? closest_light_data : GetLightData(GetClosestLight(i));
			if (_light_data->shadow_range_ <= 0) {
				continue;
			}

			if (_light_data->type_ == Renderer::kLightSpot) {
				if (geometry->GetTransformationChanged() == true)
					_light_data->shadow_map_need_update_ = true;

				float dist = _light_data->position_.GetDistanceSquared(geometry->GetTransformation().GetPosition());
				float min_dist = _light_data->radius_ + geometry->GetBoundingRadius();
				min_dist *= min_dist;

				if (dist < min_dist) {
					if (_light_data->shadow_map_geometry_set_.Insert(_geometry) == true)
						_light_data->shadow_map_need_update_ = true;
				} else {
					if (_light_data->shadow_map_geometry_set_.Remove(_geometry) == true)
						_light_data->shadow_map_need_update_ = true;
				}
			}
		}
	}

	bool shadows_updated = false;

	// Update shadow volumes.

	// Iterate over all the closest light sources and update shadow volumes.
	unsigned active_light_count = 0;
	int loop_max = kMaxShadowVolumes < GetLightCount() ? kMaxShadowVolumes : light_count;
	int i;
	for (i = 0; i < loop_max; i++) {
		bool process_light = false;
		LightData* _light_data = (i==0)? closest_light_data : GetLightData(GetClosestLight(i-1));
		if (i != 0 && closest_light_data == _light_data) {
			++loop_max;
			continue;
		}
		if (_light_data->shadow_range_ <= 0) {
			continue;
		}

		if (_light_data->enabled_ == true) {
			if (_light_data->type_ == Renderer::kLightPoint ||
			   (_light_data->type_ == Renderer::kLightSpot && shadow_hint_ == Renderer::kShVolumesOnly)) {
				float dist = _light_data->position_.GetDistanceSquared(geometry->GetTransformation().GetPosition());
				float min_dist = _light_data->radius_ + geometry->GetBoundingRadius();

				if (dist < min_dist*min_dist) {
					process_light = true;
				}
			} else if(_light_data->type_ == Renderer::kLightDirectional) {
				process_light = true;
			}
		}

		if (process_light == true) {
			++active_light_count;
			if (_geometry->shadow_volume_[i] == 0) {
				// Light has been enabled. Create shadow volume.
				int id = geometry_id_manager_.GetFreeId();

				if (id != geometry_id_manager_.GetInvalidId()) {
					ShadowVolume* _shadow_volume = new ShadowVolume(geometry);
					GeometryData* shadow_geom = CreateGeometryData();
					shadow_geom->geometry_ = _shadow_volume;
					_shadow_volume->SetRendererData(shadow_geom);

					if (_light_data->type_ == Renderer::kLightDirectional) {
						_shadow_volume->UpdateShadowVolume(_light_data->direction_,
										    _light_data->shadow_range_,
										    true);
					} else {
						_shadow_volume->UpdateShadowVolume(_light_data->position_,
										    _light_data->shadow_range_,
										    false);
					}

					shadow_volume_table_.Insert(id, shadow_geom);
					_geometry->shadow_volume_[i] = (GeometryID)id;

					shadows_updated = BindShadowGeometry(_shadow_volume, _light_data->hint_);
				}

				_geometry->light_id_[i] = GetClosestLight(i);
			} else {
				if ((geometry->GetTransformationChanged() || geometry->GetVertexDataChanged() ||
					_geometry->light_id_[i] != GetClosestLight(i)) ||
					(_light_data->transformation_changed_ &&
					current_frame_ - _geometry->last_frame_shadows_updated_ >= shadow_update_frame_delay_)) {
					ShadowVolumeTable::Iterator x = shadow_volume_table_.Find(_geometry->shadow_volume_[i]);
					if (x != shadow_volume_table_.End()) {
						GeometryData* shadow_geom = *x;
						ShadowVolume* _shadow_volume = (ShadowVolume*)shadow_geom->geometry_;
						if (!geometry->GetBigOrientationChanged()) {
							// Only update translation if orientation didn't change much.
							xform _transform(geometry->GetLastBigOrientation(), geometry->GetTransformation().GetPosition());
							_shadow_volume->SetTransformation(_transform);
						} else if (_light_data->type_ == Renderer::kLightDirectional) {
							_shadow_volume->UpdateShadowVolume(_light_data->direction_,
											    _light_data->shadow_range_,
											    true);
						} else {
							_shadow_volume->UpdateShadowVolume(_light_data->position_,
											    _light_data->shadow_range_,
											    false);
						}

						shadows_updated = true;
						_geometry->light_id_[i] = GetClosestLight(i);
					} else {
						deb_assert(false);
						_geometry->shadow_volume_[i] = (GeometryID)0;
						_geometry->light_id_[i] = INVALID_LIGHT;
					}
				}
			}
		} else if(_geometry->shadow_volume_[i] != 0) {
			// Light is too far away or has been disabled. Remove shadow volume.
			RemoveShadowVolume(_geometry->shadow_volume_[i]);
			_geometry->light_id_[i] = INVALID_LIGHT;
		}
	}// End for(i < Renderer::MAX_SHADOW_VOLUMES)

	geometry->SetTransformationChanged(false);
	geometry->SetBigOrientationChanged(false);
	geometry->SetVertexDataChanged(false);

	if (shadows_updated == true) {
		const int q = (shadow_update_frame_delay_ > 3)? shadow_update_frame_delay_ / 3 : 1;
		_geometry->last_frame_shadows_updated_ = current_frame_ + Random::GetRandomNumber() % q;
		return geometry->GetTriangleCount() * active_light_count;
	}
	return 0;
}

void Renderer::Tick(float time) {
	DynamicRendererMap::iterator x = dynamic_renderer_map_.begin();
	for (; x != dynamic_renderer_map_.end(); ++x) {
		DynamicRenderer* _renderer = x->second;
		_renderer->Tick(time);
	}
}

unsigned Renderer::GetCurrentFrame() const {
	return current_frame_;
}

bool Renderer::CheckFlag(unsigned flags, unsigned pFlag) {
	return (flags & pFlag) != 0;
}

Material* Renderer::GetMaterial(MaterialType material_type) const {
	deb_assert(material_type >= kMatNull && material_type <= kMatCount);
	return material_[material_type];
}

Renderer::GeometryTable& Renderer::GetGeometryTable() {
	return geometry_table_;
}

Renderer::ShadowVolumeTable& Renderer::GetShadowVolumeTable() {
	return shadow_volume_table_;
}

Renderer::LightData* Renderer::GetLightData(LightID light_id) const {
	return HashUtil::FindMapObject(light_data_map_, light_id);
}

int Renderer::GetNumSpotLights() const {
	return num_spot_lights_;
}

void Renderer::StepCurrentFrame() {
	++current_frame_;
}

Renderer::ShadowHint Renderer::GetShadowHint() const {
	return shadow_hint_;
}

void Renderer::PrepareProjectionData() {
	float32 _fov_angle;
	float32 _near;
	float32 _far;
	GetViewFrustum(_fov_angle, _near, _far);

	dy_ = 1.0 / tan(Math::Deg2Rad(_fov_angle) / 2.0);
	dx_ = dy_ * GetAspectRatio();
}

PixelRect Renderer::GetBoundingRect(const vec3* vertex, int num_vertices) const {
	const Canvas* _screen = GetScreen();

	if (num_vertices <= 2) {
		log_.Error("GetBoundingRect() - NumVertices < 3!");
		return PixelRect(0,0,0,0);
	}

	PixelRect _rect(0, 0, 0, 0);

	int prev_index = num_vertices - 1;
	vec3 prev(cam_transform_.InverseTransform(vec3(vertex[prev_index].x, vertex[prev_index].y, vertex[prev_index].z)));

	bool left_ok   = false;
	bool right_ok  = false;
	bool top_ok    = false;
	bool bottom_ok = false;
	double rect_left   = 0;
	double rect_right  = 0;
	double rect_top    = 0;
	double rect_bottom = 0;

	const double screen_width  = (double)_screen->GetWidth();
	const double screen_height = (double)_screen->GetHeight();

	for (int i = 0; i < num_vertices; i++) {
		vec3 current(cam_transform_.InverseTransform(vec3(vertex[i].x, vertex[i].y, vertex[i].z)));

		if ((prev.y <= 0 && current.y > 0) ||
		    (prev.y >  0 && current.y <= 0)) {
			// Clip at z = 0.
			vec3 diff = current - prev;
			vec3 clip_pos = prev + diff * (-prev.y / diff.y);

			// Determine wether the clipped position is to the left or to the right.
			if (clip_pos.x <= 0) {
				// Bounding rect reaches the left edge of the screen.
				if (left_ok == false || rect_left > -0.5)
					rect_left = -0.5;
				left_ok = true;
			} else {
				if (right_ok == false || rect_right < screen_width + 0.5)
					rect_right = screen_width + 0.5;
				right_ok = true;
			}

			if (clip_pos.z >= 0) {
				// Bounding rect reaches the top edge of the screen.
				if (top_ok == false || rect_top > -0.5)
					rect_top = -0.5;
				top_ok = true;
			} else {
				if (bottom_ok == false || rect_bottom < screen_height + 0.5)
					rect_bottom = screen_height + 0.5;
				bottom_ok = true;
			}
		}

		if (prev.y > 0) {
			// Projection of previous point..
			double recip_depth = 1.0 / prev.y;
			double px = prev.x * dx_ * recip_depth;
			double py = prev.z * dy_ * recip_depth;
			px = ( px + 1.0) * 0.5 * (screen_width  + 1.0) - 0.5;
			py = (-py + 1.0) * 0.5 * (screen_height + 1.0) - 0.5;

			// Update bounding rect.
			if (left_ok == false || px < rect_left) {
				rect_left = px;
				left_ok = true;
			}

			if (right_ok == false || px >= rect_right) {
				rect_right = px;
				right_ok = true;
			}

			if (top_ok == false || py < rect_top) {
				rect_top = py;
				top_ok = true;
			}

			if (bottom_ok == false || py >= rect_bottom) {
				rect_bottom = py;
				bottom_ok = true;
			}
		}

		prev_index = i;
		prev = current;
	}

	// Finally, we need to clamp the bounding rect to the coordinates of the screen.
	_rect.left_   = (int)floor(Math::Clamp(rect_left,   -0.5, screen_width  + 0.5) + 0.5);
	_rect.right_  = (int)floor(Math::Clamp(rect_right,  -0.5, screen_width  + 0.5) + 0.5);
	_rect.top_    = (int)floor(Math::Clamp(rect_top,    -0.5, screen_height + 0.5) + 0.5);
	_rect.bottom_ = (int)floor(Math::Clamp(rect_bottom, -0.5, screen_height + 0.5) + 0.5);

	return _rect;
}

bool Renderer::IsFacingFront(const vec3* vertex, int num_vertices) {
	if (num_vertices < 3)
		return false;

	// Use the cross product constructor to create the surface normal.
	vec3 normal(vertex[1] - vertex[0], vertex[2] - vertex[0]);
	vec3 cam_vector(vertex[0] - cam_transform_.GetPosition());

	return normal.Dot(cam_vector) > 0;
}

vec3 Renderer::ScreenCoordToVector(const PixelCoord& coord) const {
	const float w2 = clipping_rect_.GetWidth() * 0.5f;
	const float h2 = clipping_rect_.GetHeight() * 0.5f;
	float fov, _near, _far;
	GetViewFrustum(fov, _near, _far);
	fov = Math::Deg2Rad(fov);
	const float aspect = w2/h2;
	const float tana = tan(fov*0.5f);
	float dx = tana * (coord.x/w2-1.0f) * aspect;
	float dy = tana * (1.0f-coord.y/h2);
	vec3 direction(dx, 1, dy);
	direction.Normalize();
	direction = camera_transformation_.GetOrientation() * direction;
	return direction;
}

vec2 Renderer::PositionToScreenCoord(const vec3& position, float aspect_ratio) const {
	vec3 cam_direction(position - camera_transformation_.position_);
	vec3 direction;
	camera_transformation_.orientation_.FastInverseRotatedVector(camera_orientation_inverse_, direction, cam_direction);

	// Normalize so Y-distance from camera is 1.
	const float _eps = 1e-5f;	// Something small.
	if (std::abs(direction.y) < _eps) {
		direction.y = _eps;
	}
	direction.x /= direction.y;
	direction.z /= direction.y;

	const float w2 = clipping_rect_.GetWidth() * 0.5f;
	const float h2 = clipping_rect_.GetHeight() * 0.5f;
	float fov, _near, _far;
	GetViewFrustum(fov, _near, _far);
	fov = Math::Deg2Rad(fov);
	const float inverse_aspect = aspect_ratio? 1/aspect_ratio : h2/w2;
	const float inverse_tan_a = 1/tan(fov*0.5f);

	vec2 _coord;
	_coord.x = ( direction.x*inverse_tan_a*inverse_aspect+1.0f) * w2;
	_coord.y = (-direction.z*inverse_tan_a+1.0f) * h2;
	return _coord;
}

float Renderer::GetAspectRatio() const {
	return (float)viewport_.GetWidth() / (float)viewport_.GetHeight();
}

bool Renderer::CheckCulling(const xform& transform, double bounding_radius) {
	const vec3& _pos = transform.GetPosition();

	bool _visible = true;
	if (_visible) {
		// Check if object is between near and far planes.
		_visible = (_pos.y + bounding_radius) > near_ && (_pos.y - bounding_radius) < far_;
	}

	// Loop over the remaining frustum planes.
	for (int i = 0; _visible && i < 4; i++) {
		_visible = (frustum_planes_[i].Dot(_pos) > -bounding_radius);
	}

	return _visible;
}



void Renderer::ReleaseShadowVolumes() {
	ShadowVolumeTable::Iterator shadow_iter;
	for (shadow_iter = shadow_volume_table_.First(); shadow_iter != shadow_volume_table_.End(); ++shadow_iter) {
		GeometryData* shadow_geom = *shadow_iter;

		delete shadow_geom->geometry_;
		delete shadow_geom;
	}
}

void Renderer::ReleaseGeometries() {
	GeometryTable::Iterator geo_iter;

	for (geo_iter = geometry_table_.First();
		geo_iter != geometry_table_.End();
		++geo_iter) {
		GeometryData* _geometry = *geo_iter;
		delete _geometry->ta_;
		_geometry->ta_ = 0;
		delete _geometry;
	}
}

void Renderer::ReleaseTextureMaps() {
	TextureTable::Iterator x = texture_table_.First();
	while (x != texture_table_.End()) {
		TextureData* _texture = *x;
		ReleaseMap(_texture);
		TextureTable::Iterator y = x;
		++x;
		texture_table_.Remove(y);
		delete _texture;
	}
}

void Renderer::ReleaseShadowMaps() {
	// Release shadow maps.
	LightDataMap::iterator x = light_data_map_.begin();
	for (; x != light_data_map_.end(); ++x) {
		LightData* data = x->second;
		if (data->type_ == Renderer::kLightSpot) {
			data->shadow_map_id_ = ReleaseShadowMap(data->shadow_map_id_);
		}
	}
}

void Renderer::RemoveShadowVolumes(GeometryData* owner_geometry) {
	for (int i = 0; i < kMaxShadowVolumes; ++i) {
		if (owner_geometry->shadow_volume_[i] != 0) {
			RemoveShadowVolume(owner_geometry->shadow_volume_[i]);
		}
	}
}

void Renderer::RemoveShadowVolume(GeometryID& shadow_volume) {
	if (shadow_volume != 0) {
		ShadowVolumeTable::Iterator x;
		x = shadow_volume_table_.Find(shadow_volume);

		if (x != shadow_volume_table_.End()) {
			GeometryData* shadow_geom = *x;

			shadow_volume_table_.Remove(x);

			ReleaseGeometry(shadow_geom->geometry_, kGroIgnoreMaterial);
			delete shadow_geom->geometry_;
			delete shadow_geom;
		}

		geometry_id_manager_.RecycleId(shadow_volume);
		shadow_volume = (GeometryID)0;
	}
}

int Renderer::GetTriangleCount(bool visible) {
	return visible? visible_triangle_count_ : culled_triangle_count_;
}

float Renderer::GetFogNear() const {
	return fog_near_;
}

float Renderer::GetFogFar() const {
	return fog_far_;
}

float Renderer::GetFogDensity() const {
	return fog_density_;
}

float Renderer::GetFogExponent() const {
	return fog_exponent_;
}

void Renderer::SetFog(float near, float far, float density, float exponent) {
	fog_near_ = near;
	fog_far_ = far;
	fog_density_ = density;
	fog_exponent_ = exponent;
}

void Renderer::CalcCamCulling() {
	for (int x = 0; x < 4; ++x) {
		cam_frustum_planes_[x] = camera_transformation_.GetOrientation() * frustum_planes_[x];
	}
}

bool Renderer::CheckCamCulling(const vec3& position, float bounding_radius) const {
	// We only check the frustum planes on the side, and totally ignore the near and far (they currently don't add much).
	vec3 cam_relative_position(position);
	cam_relative_position.Sub(camera_transformation_.position_);
	bool _visible = true;
	for (int x = 0; _visible && x < 4; ++x) {
		_visible = (cam_frustum_planes_[x].Dot(cam_relative_position) > -bounding_radius);
	}
	return _visible;
}



Renderer* Renderer::renderer_ = 0;
vec3 Renderer::reference_position_;
loginstance(kUiGfx3D, Renderer);



}
