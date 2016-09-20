
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

#include "pch.h"
#include "../include/uiopenglrenderer.h"
#include "../../lepra/include/canvas.h"
#include "../../lepra/include/log.h"
#include "../../lepra/include/math.h"
#include "../../lepra/include/performance.h"
#include "../../lepra/include/transformation.h"
#include "../../tbc/include/geometryreference.h"
#include "../../uilepra/include/uiopenglextensions.h"
#include "../include/uidynamicrenderer.h"
#include "../include/uiopenglmaterials.h"
#include "../include/uitexture.h"



namespace uitbc {



#ifdef LEPRA_DEBUG
#define OGL_ASSERT()		{ GLenum gl_error = glGetError(); deb_assert(gl_error == GL_NO_ERROR); }
#define OGL_FAST_ASSERT()	OGL_ASSERT();
#else // !Debug
#define OGL_ASSERT()
#define OGL_FAST_ASSERT()
#endif // Debug / !Debug



static void EnableGlLights(OpenGLRenderer* renderer, std::unordered_map<Renderer::LightID,Renderer::LightData*>& light_map, bool enable) {
		// Disable all lights (for shadow rendering).
		for (std::unordered_map<Renderer::LightID,Renderer::LightData*>::iterator x = light_map.begin(); x != light_map.end(); ++x) {
			if (x->second->enabled_) {
				enable? glEnable(GL_LIGHT0 + x->first) : glDisable(GL_LIGHT0 + x->first);
			}
		}
		enable? renderer->SetLightsEnabled(enable) : renderer->Renderer::SetLightsEnabled(enable);
}



Material* OpenGLRenderer::CreateMaterial(MaterialType material_type) {
	switch(material_type) {
	case kMatNull:
		return new NullMaterial(this);
	case kMatSingleColorSolid:
		return new OpenGLMatSingleColorSolid(this, 0);
	case kMatSingleColorSolidPxs:
		return new OpenGLMatSingleColorSolidPXS(this, GetMaterial(kMatSingleColorSolid));
	case kMatVertexColorSolid:
		return new OpenGLMatVertexColorSolid(this, GetMaterial(kMatSingleColorSolid));
	case kMatSingleTextureSolid:
		return new OpenGLMatSingleTextureSolid(this, GetMaterial(kMatSingleColorSolid));
	case kMatSingleTextureHighlight:
		return new OpenGLMatSingleTextureHighlight(this, GetMaterial(kMatSingleTextureSolid));
	case kMatSingleTextureSolidPxs:
		return new OpenGLMatSingleTextureSolidPXS(this, GetMaterial(kMatSingleTextureSolid));
	case kMatSingleColorEnvmapSolid:
		return new OpenGLMatSingleColorEnvMapSolid(this, GetMaterial(kMatSingleColorSolid));
	case kMatSingleTextureEnvmapSolid:
		return new OpenGLMatSingleTextureEnvMapSolid(this, GetMaterial(kMatSingleTextureSolid));
	case kMatTextureAndLightmap:
		return new OpenGLMatTextureAndLightmap(this, GetMaterial(kMatSingleTextureSolid));
	case kMatTextureAndLightmapPxs:
		return new OpenGLMatTextureAndLightmapPXS(this, GetMaterial(kMatTextureAndLightmap));
	case kMatTextureAndDiffuseBumpmapPxs:
		return new OpenGLMatTextureDiffuseBumpMapPXS(this, GetMaterial(kMatSingleTextureSolidPxs));
	case kMatTextureSbmapPxs:
		return new OpenGLMatTextureSBMapPXS(this, GetMaterial(kMatTextureAndDiffuseBumpmapPxs));
	case kMatSingleColorBlended:
		return new OpenGLMatSingleColorBlended(this, 0, false);
	case kMatSingleColorOutlineBlended:
		return new OpenGLMatSingleColorBlended(this, 0, true);
	case kMatVertexColorBlended:
		return new OpenGLMatVertexColorBlended(this, GetMaterial(kMatSingleColorBlended));
	case kMatSingleTextureBlended:
		return new OpenGLMatSingleTextureBlended(this, GetMaterial(kMatSingleColorBlended));
	case kMatSingleTextureAlphatested:
		return new OpenGLMatSingleTextureAlphaTested(this, GetMaterial(kMatSingleColorBlended));
	case kMatSingleColorEnvmapBlended:
		return new OpenGLMatSingleColorEnvMapBlended(this, GetMaterial(kMatSingleColorBlended));
	case kMatSingleTextureEnvmapBlended:
		return new OpenGLMatSingleTextureEnvMapBlended(this, GetMaterial(kMatSingleTextureBlended));
	default:
		return 0;
	}
}



OpenGLRenderer::OpenGLRenderer(Canvas* screen) :
	Renderer(screen),
	buffer_id_manager_(1, 1000000, 0),
	t_map_id_manager_(10001, 1000000, INVALID_TEXTURE),	// 1-10000 is reserved by Painter.
	gl_clear_mask_(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) {
	OGL_ASSERT();

	InitRenderer();

	float r, g, b;
	GetAmbientLight(r, g, b);
	SetAmbientLight(r, g, b);

	glGetIntegerv(GL_MAX_TEXTURE_UNITS, &num_texture_units_);
	glGetError();	// If we have a weak card, let's do our best with funny looks.
	glDepthFunc(GL_LEQUAL);
	glClearStencil(128);
	OGL_ASSERT();
}



OpenGLRenderer::~OpenGLRenderer() {
	CloseRenderer();
}

void OpenGLRenderer::Clear(unsigned clear_flags) {
	OGL_FAST_ASSERT();

	gl_clear_mask_ = 0;

	if (CheckFlag(clear_flags, kClearColorbuffer)) {
		gl_clear_mask_ |= GL_COLOR_BUFFER_BIT;
	}
	if (CheckFlag(clear_flags, kClearDepthbuffer)) {
		gl_clear_mask_ |= GL_DEPTH_BUFFER_BIT;
	}
	if (CheckFlag(clear_flags, kClearStencilbuffer)) {
		gl_clear_mask_ |= GL_STENCIL_BUFFER_BIT;
	}
#ifndef LEPRA_GL_ES
	if (CheckFlag(clear_flags, kClearAccumulationbuffer)) {
		gl_clear_mask_ |= GL_ACCUM_BUFFER_BIT;
	}
#endif // !GLES

	if (GetShadowMode() != uitbc::Renderer::kNoShadows) {
		// Always clear the stencil buffer if shadows are activated.
		gl_clear_mask_ |= GL_STENCIL_BUFFER_BIT;
	}

	glClear(gl_clear_mask_);

	OGL_ASSERT();
}

void OpenGLRenderer::SetClearColor(const Color& color) {
	Parent::SetClearColor(color);
	::glClearColor(color.GetRf(), color.GetGf(), color.GetBf(), 1.0f);
	OGL_FAST_ASSERT();
}

bool OpenGLRenderer::IsPixelShadersEnabled() const {
	return uilepra::OpenGLExtensions::IsShaderAsmProgramsSupported() && Parent::IsPixelShadersEnabled();
}

void OpenGLRenderer::SetViewFrustum(float fov_angle, float cam_near, float cam_far) {
	Parent::SetViewFrustum(fov_angle, cam_near, cam_far);
	Perspective(fov_angle, GetAspectRatio(), cam_near, cam_far);
	OGL_FAST_ASSERT();
}

void OpenGLRenderer::SetShadowMode(Shadows shadow_mode, ShadowHint hint) {
	Parent::SetShadowMode(shadow_mode, uilepra::OpenGLExtensions::IsShadowMapsSupported()? hint : kShVolumesOnly);
}

void OpenGLRenderer::SetDepthWriteEnabled(bool enabled) {
	if (enabled == true)
		glDepthMask(GL_TRUE);
	else
		glDepthMask(GL_FALSE);
	OGL_FAST_ASSERT();
}

void OpenGLRenderer::SetDepthTestEnabled(bool enabled) {
	if (enabled == true)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
	OGL_FAST_ASSERT();
}

void OpenGLRenderer::SetLightsEnabled(bool enabled) {
	Parent::SetLightsEnabled(enabled);
	if (enabled) {
		::glEnable(GL_LIGHTING);
	} else {
		::glDisable(GL_LIGHTING);
	}
}

void OpenGLRenderer::SetTexturingEnabled(bool enabled) {
	Parent::SetTexturingEnabled(enabled);
	if (enabled) {
		::glEnable(GL_TEXTURE_2D);
		::glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	} else {
		::glDisable(GL_TEXTURE_2D);
		::glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
}

void OpenGLRenderer::SetLineWidth(float pixels) {
	Parent::SetLineWidth(pixels);
	::glLineWidth(pixels);
}

void OpenGLRenderer::SetAmbientLight(float red, float green, float blue) {
	Parent::SetAmbientLight(red, green, blue);

	float ambient_light[] = {red, green, blue, 1.0f};
	::glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient_light);
	::glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient_light);
#ifndef LEPRA_GL_ES
	if (IsPixelShadersEnabled()) {
		GLfloat light_ambient[4] = {0,0,0,0};
		uilepra::OpenGLExtensions::glProgramLocalParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB, 3, light_ambient);
	}
#endif // !OpenGL ES
	OGL_FAST_ASSERT();
}

void OpenGLRenderer::AddAmbience(float red, float green, float blue) {
	if (red != 0 || green != 0 || blue != 0) {
		Parent::AddAmbience(red, green, blue);
		float ambient_light[4];
		GetAmbientLight(ambient_light[0], ambient_light[1], ambient_light[2]);
		ambient_light[3] = 1.0f;
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient_light);
	}
	OGL_FAST_ASSERT();
}

void OpenGLRenderer::DoSetClippingRect(const PixelRect& rect) {
	//::glGetError();
	if (rect.GetWidth() == (int)GetScreen()->GetActualWidth() && rect.GetHeight() == (int)GetScreen()->GetActualHeight()) {
		::glDisable(GL_SCISSOR_TEST);
	} else {
		::glEnable(GL_SCISSOR_TEST);
		::glScissor(GetScreen()->GetActualWidth() - rect.right_, GetScreen()->GetActualHeight() - rect.bottom_, rect.GetWidth(), rect.GetHeight());
	}
	OGL_FAST_ASSERT();
}

void OpenGLRenderer::DoSetViewport(const PixelRect& viewport) {
	OGL_FAST_ASSERT();
	::glViewport(GetScreen()->GetActualWidth() - viewport.right_, GetScreen()->GetActualHeight() - viewport.bottom_, viewport.GetWidth(), viewport.GetHeight());
	OGL_FAST_ASSERT();
}

int OpenGLRenderer::ReleaseShadowMap(int shadow_map_id) {
	if (shadow_map_id != t_map_id_manager_.GetInvalidId()) {
		t_map_id_manager_.RecycleId(shadow_map_id);
		GLuint _shadow_map_id = (GLuint)shadow_map_id;
		glDeleteTextures(1, &_shadow_map_id);
	}
	//OGL_FAST_ASSERT();
	return t_map_id_manager_.GetInvalidId();
}




Renderer::LightID OpenGLRenderer::AddDirectionalLight(LightHint hint,
		const vec3& dir,
		const vec3& color,
		float shadow_range) {
	LightID _light_id = Parent::AddDirectionalLight(hint, dir, color, shadow_range);
	SetupGLLight((int)_light_id, GetLightData((int)_light_id));
	OGL_FAST_ASSERT();
	return _light_id;
}

Renderer::LightID OpenGLRenderer::AddPointLight(LightHint hint,
		const vec3& pos,
		const vec3& color,
		float light_radius,
		float shadow_range) {
	LightID _light_id = Parent::AddPointLight(hint, pos, color, light_radius, shadow_range);
	SetupGLLight((int)_light_id, GetLightData((int)_light_id));
	OGL_FAST_ASSERT();
	return _light_id;
}

Renderer::LightID OpenGLRenderer::AddSpotLight(LightHint hint,
		const vec3& pos,
		const vec3& dir,
		const vec3& color,
		float cutoff_angle,
		float spot_exponent,
		float light_radius,
		float shadow_range) {
	LightID _light_id = Parent::AddSpotLight(hint,
		pos,
		dir,
		color,
		cutoff_angle,
		spot_exponent,
		light_radius,
		shadow_range);
	SetupGLLight((int)_light_id, GetLightData((int)_light_id));
	OGL_FAST_ASSERT();
	return _light_id;
}

void OpenGLRenderer::SetupGLLight(int light_index, const LightData* light) {
	GLenum _light = GL_LIGHT0 + light_index;
	glEnable(_light);

	if (light->type_ != kLightSpot) {
		float l180 = 180.0f;
		glLightf(_light, GL_SPOT_CUTOFF, l180);

		float _dir[3] = {0, 0, -1};
		glLightfv(_light, GL_SPOT_DIRECTION, _dir);
	}

	float _pos[4];
	if (light->type_ == kLightPoint ||
	   light->type_ == kLightSpot) {
		vec3 light_pos = GetCameraActualTransformation().InverseTransform(light->position_);
		_pos[0] = (float)light_pos.x;
		_pos[1] = (float)light_pos.y;
		_pos[2] = (float)light_pos.z;
		_pos[3] = 1.0f;

		if (light->type_ == kLightSpot) {
			vec3 light_dir = GetCameraActualOrientationInverse() * light->direction_;

			float _dir[3];
			_dir[0] = (float)light_dir.x;
			_dir[1] = (float)light_dir.y;
			_dir[2] = (float)light_dir.z;

			glLightfv(_light, GL_SPOT_DIRECTION, _dir);
			glLightfv(_light, GL_SPOT_CUTOFF, &light->cutoff_angle_);
		}

		float constant = 0.0f;
		float linear = 0.0f;
		float quad = 1.0f;
		glLightfv(_light, GL_CONSTANT_ATTENUATION, &constant);
		glLightfv(_light, GL_LINEAR_ATTENUATION, &linear);
		glLightfv(_light, GL_QUADRATIC_ATTENUATION, &quad);
	} else if(light->type_ == kLightDirectional) {
		vec3 light_dir = GetCameraActualOrientationInverse() * light->direction_;
		_pos[0] = -(float)light_dir.x;
		_pos[1] = -(float)light_dir.y;
		_pos[2] = -(float)light_dir.z;
		_pos[3] = 0.0f;

		float constant = 1.0f;
		float linear = 0.0f;
		float quad = 0.0f;
		glLightfv(_light, GL_CONSTANT_ATTENUATION, &constant);
		glLightfv(_light, GL_LINEAR_ATTENUATION, &linear);
		glLightfv(_light, GL_QUADRATIC_ATTENUATION, &quad);
	}

	float black[] = {0, 0, 0, 1.0f};

	glLightfv(_light, GL_POSITION, _pos);
	glLightfv(_light, GL_AMBIENT, black);
	glLightfv(_light, GL_DIFFUSE, light->color_);
	glLightfv(_light, GL_SPECULAR, black);
	OGL_FAST_ASSERT();
}

void OpenGLRenderer::RemoveLight(LightID light_id) {
	Parent::RemoveLight(light_id);
	GLenum _light = (int)light_id;
	glDisable(GL_LIGHT0+_light);
	OGL_FAST_ASSERT();
}

void OpenGLRenderer::EnableAllLights(bool enable) {
	for (LightDataMap::iterator x = light_data_map_.begin(); x != light_data_map_.end(); ++x) {
		LightData* _light = x->second;
		_light->enabled_ = enable;
		if (enable) {
			::glEnable(GL_LIGHT0 + x->first);
		} else {
			::glDisable(GL_LIGHT0 + x->first);
		}
	}
}



void OpenGLRenderer::SetLightPosition(LightID light_id, const vec3& pos) {
	Parent::SetLightPosition(light_id, pos);

	int _light_index = (int)light_id;
	if (_light_index == INVALID_LIGHT)
		return;

	LightData* light_data = GetLightData(_light_index);
	if (light_data->type_ == kLightPoint ||
	   light_data->type_ == kLightSpot) {
		vec3 light_pos = GetCameraActualTransformation().InverseTransform(pos);
		float _pos[4];
		_pos[0] = (float)light_pos.x;
		_pos[1] = (float)light_pos.y;
		_pos[2] = (float)light_pos.z;
		_pos[3] = 1.0f;

		GLenum _light = GL_LIGHT0 + _light_index;
		glLightfv(_light, GL_POSITION, _pos);
	}
	OGL_FAST_ASSERT();
}



void OpenGLRenderer::SetLightDirection(LightID light_id, const vec3& dir) {
	Parent::SetLightDirection(light_id, dir);

	int _light_index = (int)light_id;
	if (_light_index == INVALID_LIGHT)
		return;

	LightData* light_data = GetLightData(_light_index);
	if (light_data->type_ == kLightDirectional ||
	   light_data->type_ == kLightSpot) {
		GLenum _light = GL_LIGHT0 + _light_index;

		if (light_data->type_ == kLightDirectional) {
			float _vector[4];
			_vector[0] = -dir.x;
			_vector[1] = -dir.y;
			_vector[2] = -dir.z;
			_vector[3] = 0.0f;
			glLightfv(_light, GL_POSITION, _vector);
		} else {
			float _vector[3];
			_vector[0] = dir.x;
			_vector[1] = dir.y;
			_vector[2] = dir.z;
			glLightfv(_light, GL_SPOT_DIRECTION, _vector);
		}
	}
	OGL_FAST_ASSERT();
}

Renderer::TextureID OpenGLRenderer::AddTexture(Texture* texture) {
	texture->SwapRGBOrder();
	return (Parent::AddTexture(texture));
}

void OpenGLRenderer::SetGlobalMaterialReflectance(float red, float green, float blue, float specularity) {
	// This function is the evidence of the total stupidity behind
	// OpenGL and probably computer graphics in general.
	// To be able to have full control of a surface's diffuseness,
	// we need to update the light's diffuseness. And therein lies
	// the stupidity - light with "diffuseness" doesn't make any sense
	// at all.

	// TRICKY: must add some to the colors, since black light means disabled light!
	red += 0.01f;
	green += 0.01f;
	blue += 0.01f;

	for (LightDataMap::iterator x = light_data_map_.begin(); x != light_data_map_.end(); ++x) {
		LightData* light_data = x->second;

		if (light_data->enabled_ == true) {
			const float _red   = light_data->color_[0] * red;
			const float _green = light_data->color_[1] * green;
			const float _blue  = light_data->color_[2] * blue;

			float specular[] =
			{
				_red   * specularity,
				_green * specularity,
				_blue  * specularity,
				1.0f
			};

			float diffuse[] =
			{
				_red   * (1.0f - specularity),
				_green * (1.0f - specularity),
				_blue  * (1.0f - specularity),
				1.0f
			};

			float ambient[] = {0, 0, 0, 1.0f};

			GLenum _light = GL_LIGHT0 + x->first;
			glLightfv(_light, GL_DIFFUSE, diffuse);
			glLightfv(_light, GL_SPECULAR, specular);
			glLightfv(_light, GL_AMBIENT, ambient);
		}
	}
	OGL_FAST_ASSERT();
}

Renderer::TextureData* OpenGLRenderer::CreateTextureData(TextureID texture_id) {
	return new Parent::TextureData(texture_id, t_map_id_manager_.GetInvalidId());
}

Renderer::GeometryData* OpenGLRenderer::CreateGeometryData() {
	return new OGLGeometryData();
}

const Canvas* OpenGLRenderer::GetMap(int map_type, int mip_map_level, Texture* user_texture) {
	switch (map_type) {
		case Texture::kColorMap:
			return user_texture->GetColorMap(mip_map_level);
		case Texture::kAlphaMap:
			return user_texture->GetAlphaMap(mip_map_level);
		case Texture::kNormalMap:
			return user_texture->GetNormalMap(mip_map_level);
		case Texture::kSpecularMap:
			return user_texture->GetSpecularMap(mip_map_level);
		default:
			deb_assert(false);
			return 0;
	};
}

void OpenGLRenderer::BindMap(int map_type, TextureData* texture_data, Texture* texture) {
	OGL_FAST_ASSERT();
	deb_assert(map_type >= 0 && map_type < Texture::kNumMaps);

	bool _compress = uilepra::OpenGLExtensions::IsCompressedTexturesSupported() &&
					GetCompressedTexturesEnabled();

	if (texture_data->t_map_id_[map_type] == t_map_id_manager_.GetInvalidId()) {
		texture_data->t_map_id_[map_type] = t_map_id_manager_.GetFreeId();
		texture_data->t_mip_map_level_count_[map_type] = texture->GetNumMipMapLevels();
	}

	glBindTexture(GL_TEXTURE_2D, texture_data->t_map_id_[map_type]);
	OGL_FAST_ASSERT();

	int __size = GetMap(map_type, 0, texture)->GetPixelByteSize();
	deb_assert(__size == 1 || __size == 3 || __size == 4);

	GLenum _pixel_format;
	SetPixelFormat(__size, _pixel_format, _compress,
		strutil::Format("AddTexture - the texture has an invalid pixel size of %i bytes!", __size));
	OGL_FAST_ASSERT();

	for (int i = 0; i < texture->GetNumMipMapLevels(); i++) {
		const Canvas* map = GetMap(map_type, i, texture);
		glTexImage2D(GL_TEXTURE_2D,
			     i,
			     __size,
			     map->GetWidth(),
			     map->GetHeight(),
			     0,
			     _pixel_format, // TODO: Verify that this is GL_LUMINANCE for specular maps.
			     GL_UNSIGNED_BYTE,
			     map->GetBuffer());
		OGL_FAST_ASSERT();
		if (!GetMipMappingEnabled()) {
			break;
		}
	}
	OGL_FAST_ASSERT();
}

void OpenGLRenderer::BindCubeMap(TextureData* texture_data, Texture* texture) {
#ifndef LEPRA_GL_ES
	// Compress textures if possible.
	bool _compress = uilepra::OpenGLExtensions::IsCompressedTexturesSupported() &&
				GetCompressedTexturesEnabled();

	int __size = texture->GetCubeMapPosX(0)->GetPixelByteSize();
	GLenum _pixel_format;
	SetPixelFormat(__size, _pixel_format, _compress,
		strutil::Format("AddTexture - the cube map has an invalid pixel size of %i bytes!", __size));

	if (texture_data->t_map_id_[Texture::kCubeMap] == t_map_id_manager_.GetInvalidId()) {
		texture_data->t_map_id_[Texture::kCubeMap] = t_map_id_manager_.GetFreeId();
		texture_data->t_mip_map_level_count_[Texture::kCubeMap] = texture->GetNumMipMapLevels();
	}

	// Bind cube map.
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_data->t_map_id_[Texture::kCubeMap]);

	for (int i = 0; i < texture->GetNumMipMapLevels(); i++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X,
			     i,
			     __size,
			     texture->GetCubeMapPosX(i)->GetWidth(),
			     texture->GetCubeMapPosX(i)->GetHeight(),
			     0,
			     _pixel_format,
			     GL_UNSIGNED_BYTE,
			     texture->GetCubeMapPosX(i)->GetBuffer());

		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
			     i,
			     __size,
			     texture->GetCubeMapNegX(i)->GetWidth(),
			     texture->GetCubeMapNegX(i)->GetHeight(),
			     0,
			     _pixel_format,
			     GL_UNSIGNED_BYTE,
			     texture->GetCubeMapNegX(i)->GetBuffer());

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
			     i,
			     __size,
			     texture->GetCubeMapPosY(i)->GetWidth(),
			     texture->GetCubeMapPosY(i)->GetHeight(),
			     0,
			     _pixel_format,
			     GL_UNSIGNED_BYTE,
			     texture->GetCubeMapPosY(i)->GetBuffer());

		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
			     i,
			     __size,
			     texture->GetCubeMapNegY(i)->GetWidth(),
			     texture->GetCubeMapNegY(i)->GetHeight(),
			     0,
			     _pixel_format,
			     GL_UNSIGNED_BYTE,
			     texture->GetCubeMapNegY(i)->GetBuffer());

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
			     i,
			     __size,
			     texture->GetCubeMapPosZ(i)->GetWidth(),
			     texture->GetCubeMapPosZ(i)->GetHeight(),
			     0,
			     _pixel_format,
			     GL_UNSIGNED_BYTE,
			     texture->GetCubeMapPosZ(i)->GetBuffer());

		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
			     i,
			     __size,
			     texture->GetCubeMapNegZ(i)->GetWidth(),
			     texture->GetCubeMapNegZ(i)->GetHeight(),
			     0,
			     _pixel_format,
			     GL_UNSIGNED_BYTE,
			     texture->GetCubeMapNegZ(i)->GetBuffer());
	}
	OGL_FAST_ASSERT();
#endif // !GLES
}

void OpenGLRenderer::ReleaseMap(TextureData* texture_data) {
	for (int i = 0; i < Texture::kNumMaps; i++) {
		if (texture_data->t_map_id_[i] != 0) {
			t_map_id_manager_.RecycleId(texture_data->t_map_id_[i]);
			GLuint texture_name = texture_data->t_map_id_[i];
			glDeleteTextures(1, &texture_name);
			texture_data->t_map_id_[i] = 0;
			texture_data->t_mip_map_level_count_[i] = 0;
		}
	}

	if (texture_data == GetEnvTexture()) {
		SetEnvironmentMap(INVALID_TEXTURE);
	}
	//OGL_FAST_ASSERT();
}

void OpenGLRenderer::BindGeometry(tbc::GeometryBase* geometry,
				  GeometryID /*id*/,
				  MaterialType material_type) {
	OGL_FAST_ASSERT();

	// A hard coded check to make life easier for the user.
	if (material_type == kMatTextureSbmapPxs ||
	    material_type == kMatTextureAndDiffuseBumpmapPxs) {
		geometry->GenerateTangentAndBitangentData();
	}

	OGLGeometryData* geometry_data = (OGLGeometryData*)geometry->GetRendererData();
	if (geometry->IsGeometryReference()) {
		tbc::GeometryBase* parent_geometry = ((tbc::GeometryReference*)geometry)->GetParentGeometry();
		GeometryData* parent_geometry_data = (GeometryData*)parent_geometry->GetRendererData();
		geometry_data->CopyReferenceData(parent_geometry_data);
	} else {
		AppendWireframeLines(geometry);
		geometry_data->index_count_[0] = geometry->GetIndexCount();
		geometry_data->index_count_[1] = geometry->GetMaxIndexCount() - geometry->GetIndexCount();
		geometry_data->index_offset_[1] = geometry->GetIndexCount()*sizeof(vtx_idx_t);

		if (uilepra::OpenGLExtensions::IsBufferObjectsSupported() == true) {
			// Upload geometry to the GFX hardware.

			// First, get a free buffer ID and store it first in the VoidPtr array.
			geometry_data->vertex_buffer_id_ = buffer_id_manager_.GetFreeId();
			//log_volatile(mLog.Tracef("Alloc buffer ID %u (vertex."), lGeometryData->mVertexBufferID));
			geometry_data->index_buffer_id_  = buffer_id_manager_.GetFreeId();

			// Calculate the size of the created buffer.
			int vertex_count = geometry->GetMaxVertexCount();
			int buffer_size = vertex_count * sizeof(float) * 3;
			if (geometry->GetNormalData() != 0) {
				buffer_size += vertex_count * sizeof(float) * 3;
			}
			if (geometry->GetColorData() != 0) {
				int __size = 4;
				if (geometry->GetColorFormat() == tbc::GeometryBase::kColorRgb)
					__size = 3;

				buffer_size += vertex_count * sizeof(unsigned char) * __size;
			}
			if (geometry->GetUVSetCount() > 0) {
				buffer_size += (vertex_count * sizeof(float) * geometry->GetUVCountPerVertex()) * geometry->GetUVSetCount();
			}
			if (geometry->GetTangentData() != 0) {
				// Assume that we have bitangent data as well.
				buffer_size += vertex_count * sizeof(float) * 6;
			}

			// Set the most appropriate buffer object hint.
			GLenum gl_hint = GL_DYNAMIC_DRAW;
#ifndef LEPRA_GL_ES
			switch(geometry->GetGeometryVolatility()) {
			case tbc::GeometryBase::kGeomStatic:
			case tbc::GeometryBase::kGeomSemiStatic:
				gl_hint = GL_STATIC_DRAW;
				break;
			case tbc::GeometryBase::kGeomDynamic:
				gl_hint = GL_DYNAMIC_DRAW;
				break;
			case tbc::GeometryBase::kGeomVolatile:
				gl_hint = GL_STREAM_DRAW;
				break;
			}
#endif // !GLES
			// Bind and create the vertex buffer in GFX memory.
			//log_volatile(mLog.Tracef("glBindBuffer %u (vertex."), lGeometryData->mVertexBufferID));
			uilepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER,
							      (GLuint)geometry_data->vertex_buffer_id_);
			uilepra::OpenGLExtensions::glBufferData(GL_ARRAY_BUFFER,
							      buffer_size,
							      NULL, gl_hint);

			size_t offset = 0;
			// Upload vertex data.
			uilepra::OpenGLExtensions::glBufferSubData(GL_ARRAY_BUFFER, offset,
				vertex_count * sizeof(float) * 3, (void*)geometry->GetVertexData());
			offset += vertex_count * sizeof(float) * 3;

			// Upload normal data.
			geometry_data->normal_offset_ = offset;
			if (geometry->GetNormalData() != 0) {
				uilepra::OpenGLExtensions::glBufferSubData(GL_ARRAY_BUFFER, offset,
						vertex_count * sizeof(float) * 3, (void*)geometry->GetNormalData());
				offset += vertex_count * sizeof(float) * 3;
			}

			// Upload color data.
			geometry_data->color_offset_ = offset;
			if (geometry->GetColorData() != 0) {
				int __size = 4;
				if (geometry->GetColorFormat() == tbc::GeometryBase::kColorRgb)
					__size = 3;

				uilepra::OpenGLExtensions::glBufferSubData(GL_ARRAY_BUFFER,
									 offset,
									 vertex_count * sizeof(unsigned char) * __size,
									 (void*)geometry->GetColorData());
				offset += vertex_count * sizeof(unsigned char) * __size;
			}


			// Upload UV data.
			geometry_data->uv_offset_ = offset;
			if (geometry->GetUVSetCount() > 0) {
				const int uv_count_per_vertex = geometry->GetUVCountPerVertex();
				for (unsigned i = 0; i < geometry->GetUVSetCount(); i++) {
					uilepra::OpenGLExtensions::glBufferSubData(GL_ARRAY_BUFFER,
										 offset,
										 vertex_count * sizeof(float) * uv_count_per_vertex,
										 (void*)geometry->GetUVData(i));
					offset += vertex_count * sizeof(float) * uv_count_per_vertex;
				}
			}

			// Upload tangent data.
			geometry_data->tangent_offset_ = offset;
			if (geometry->GetTangentData() != 0) {
				uilepra::OpenGLExtensions::glBufferSubData(GL_ARRAY_BUFFER,
									 offset,
									 vertex_count * sizeof(float) * 3,
									 (void*)geometry->GetTangentData());
				offset += vertex_count * sizeof(float) * 3;
			} else {
				geometry_data->tangent_offset_ = 0;
			}

			// Upload bitangent data.
			geometry_data->bitangent_offset_ = offset;
			if (geometry->GetBitangentData() != 0) {
				uilepra::OpenGLExtensions::glBufferSubData(GL_ARRAY_BUFFER,
									 offset,
									 vertex_count * sizeof(float) * 3,
									 (void*)geometry->GetBitangentData());
				offset += vertex_count * sizeof(float) * 3;
			} else {
				geometry_data->bitangent_offset_ = 0;
			}

			// Bind and create the index buffer in GFX memory.
			uilepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (GLuint)geometry_data->index_buffer_id_);
			uilepra::OpenGLExtensions::glBufferData(GL_ELEMENT_ARRAY_BUFFER,
							      geometry->GetMaxIndexCount() * sizeof(vtx_idx_t),
							      0, gl_hint);

			uilepra::OpenGLExtensions::glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,
								 0,
								 geometry->GetMaxIndexCount() * sizeof(vtx_idx_t),
								 (void*)geometry->GetIndexData());
		}
	}

	OGL_FAST_ASSERT();
}

bool OpenGLRenderer::BindShadowGeometry(uitbc::ShadowVolume* shadow_volume, LightHint light_hint) {
	bool ok = false;

	OGLGeometryData* shadow_geom = (OGLGeometryData*)shadow_volume->GetRendererData();
	if (uilepra::OpenGLExtensions::IsBufferObjectsSupported() == true) {
		// Upload geometry to the GFX hardware.

		// First, get a free buffer ID and store it first in the VoidPtr array.
		shadow_geom->vertex_buffer_id_ = buffer_id_manager_.GetFreeId();
		//log_volatile(mLog.Tracef("Alloc buffer ID %u (vertex."), lShadowGeom->mVertexBufferID));
		shadow_geom->index_buffer_id_  = buffer_id_manager_.GetFreeId();

		// Calculate the size of the created buffer.
		int vertex_count = shadow_volume->GetMaxVertexCount();
		int buffer_size = vertex_count * sizeof(float) * 3;

		// Set the most appropriate buffer object hint.
		GLenum gl_hint = GL_DYNAMIC_DRAW;
#ifndef LEPRA_GL_ES
		switch(shadow_volume->GetGeometryVolatility()) {
		case tbc::GeometryBase::kGeomStatic:
		case tbc::GeometryBase::kGeomSemiStatic:
			if (light_hint == kLightMovable)
				gl_hint = GL_STREAM_DRAW;
			else
				gl_hint = GL_STATIC_DRAW;
			break;
		case tbc::GeometryBase::kGeomDynamic:
			if (light_hint == kLightMovable)
				gl_hint = GL_STREAM_DRAW;
			else
				gl_hint = GL_DYNAMIC_DRAW;
			break;
		case tbc::GeometryBase::kGeomVolatile:
			gl_hint = GL_STREAM_DRAW;
			break;
		}
#endif // !GLES

		// Bind and create the vertex buffer in GFX memory.
		//log_volatile(mLog.Tracef("glBindBuffer %u (vertex."), lShadowGeom->mVertexBufferID));
		uilepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, (GLuint)shadow_geom->vertex_buffer_id_);
		uilepra::OpenGLExtensions::glBufferData(GL_ARRAY_BUFFER,
						      buffer_size,
						      NULL, gl_hint);

		int offset = 0;
		// Upload vertex data.
		uilepra::OpenGLExtensions::glBufferSubData(GL_ARRAY_BUFFER,
							 offset,
							 vertex_count * sizeof(float) * 3,
							 (void*)shadow_volume->GetVertexData());
		offset += vertex_count * sizeof(float) * 3;

		// Bind and create the index buffer in GFX memory.
		uilepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (GLuint)shadow_geom->index_buffer_id_);
		uilepra::OpenGLExtensions::glBufferData(GL_ELEMENT_ARRAY_BUFFER,
						      shadow_volume->GetMaxIndexCount() * sizeof(vtx_idx_t),
						      shadow_volume->GetIndexData(), gl_hint);

		ok = true;
	}

	OGL_FAST_ASSERT();
	return ok;
}

void OpenGLRenderer::UpdateGeometry(GeometryID geometry_id, bool force) {
	GeometryTable::Iterator iter;
	iter = GetGeometryTable().Find(geometry_id);

	if (iter != GetGeometryTable().End()) {
		OGLGeometryData* geom_data = (OGLGeometryData*)*iter;
		tbc::GeometryBase* _geometry = geom_data->geometry_;

		if (force) {
			ReleaseGeometry(_geometry, kGroIgnoreMaterial);
			BindGeometry(_geometry, geometry_id, GetMaterialType(geometry_id));
			return;
		}
		// Force update of shadow volumes.

		if (uilepra::OpenGLExtensions::IsBufferObjectsSupported() && !_geometry->IsGeometryReference()) {
			int vertex_count = _geometry->GetVertexCount();

			if (_geometry->GetVertexDataChanged() ||
			   _geometry->GetColorDataChanged() ||
			   _geometry->GetUVDataChanged()) {
				//log_volatile(mLog.Tracef("glBindBuffer %u (vertex."), lGeomData->mVertexBufferID));
				uilepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, (GLuint)geom_data->vertex_buffer_id_);
			}

			if (_geometry->GetVertexDataChanged()) {
				uilepra::OpenGLExtensions::glBufferSubData(GL_ARRAY_BUFFER,
									 geom_data->vertex_offset_,
									 vertex_count * sizeof(float) * 3,
									 (void*)_geometry->GetVertexData());

				if (_geometry->GetNormalData()) {
					uilepra::OpenGLExtensions::glBufferSubData(GL_ARRAY_BUFFER,
										 geom_data->normal_offset_,
										 vertex_count * sizeof(float) * 3,
										 (void*)_geometry->GetNormalData());
				}

				// Only reset the flag if there are no shadows to update.
				// The flag will be reset when the shadows are updated.
				if (geom_data->shadow_ <= kNoShadows) {
					_geometry->SetVertexDataChanged(false);
				}
			}

			if (_geometry->GetColorDataChanged() && _geometry->GetColorData()) {
				int __size = 4;
				if (_geometry->GetColorFormat() == tbc::GeometryBase::kColorRgb)
					__size = 3;

				uilepra::OpenGLExtensions::glBufferSubData(GL_ARRAY_BUFFER,
									 geom_data->color_offset_,
									 vertex_count * sizeof(unsigned char) * __size,
									 (void*)_geometry->GetColorData());
			}

			if (_geometry->GetUVDataChanged()) {
				OGL_FAST_ASSERT();
				size_t offset = geom_data->uv_offset_;
				int uv_count_per_vertex = _geometry->GetUVCountPerVertex();
				for (unsigned i = 0; i < _geometry->GetUVSetCount(); i++) {
					uilepra::OpenGLExtensions::glBufferSubData(GL_ARRAY_BUFFER, offset,
						vertex_count * sizeof(float) * uv_count_per_vertex, (void*)_geometry->GetUVData(i));
					offset += vertex_count * sizeof(float) * uv_count_per_vertex;
					OGL_FAST_ASSERT();
				}
			}

			if (_geometry->GetTangentData() != 0 &&
			   (_geometry->GetVertexDataChanged() || _geometry->GetUVDataChanged())) {
				if (geom_data->tangent_offset_ > 0) {
					uilepra::OpenGLExtensions::glBufferSubData(GL_ARRAY_BUFFER,
										 geom_data->tangent_offset_,
										 vertex_count * sizeof(float) * 3,
										 (void*)_geometry->GetTangentData());
				}

				if (geom_data->bitangent_offset_ > 0) {
					uilepra::OpenGLExtensions::glBufferSubData(GL_ARRAY_BUFFER,
										 geom_data->bitangent_offset_,
										 vertex_count * sizeof(float) * 3,
										 (void*)_geometry->GetBitangentData());
				}
			}

			if (_geometry->GetIndexDataChanged()) {
				uilepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (GLuint)geom_data->index_buffer_id_);
				uilepra::OpenGLExtensions::glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0,
					_geometry->GetTriangleCount() * 3 * sizeof(vtx_idx_t),
					(void*)_geometry->GetIndexData());
			}
		}
	}
	OGL_FAST_ASSERT();
}

void OpenGLRenderer::ReleaseGeometry(tbc::GeometryBase* user_geometry, GeomReleaseOption option) {
	OGLGeometryData* _geometry = (OGLGeometryData*)user_geometry->GetRendererData();

	if (user_geometry->IsGeometryReference() == false &&
		uilepra::OpenGLExtensions::IsBufferObjectsSupported() == true) {
		GLuint vertex_buffer_id = (GLuint)_geometry->vertex_buffer_id_;
		GLuint index_buffer_id  = (GLuint)_geometry->index_buffer_id_;

		//log_volatile(mLog.Tracef("glDeleteBuffers %u (vertex."), lVertexBufferID));
		uilepra::OpenGLExtensions::glDeleteBuffers(1, &vertex_buffer_id);
		//log_volatile(mLog.Tracef("glDeleteBuffers %u (index."), lIndexBufferID));
		uilepra::OpenGLExtensions::glDeleteBuffers(1, &index_buffer_id);

		buffer_id_manager_.RecycleId(_geometry->vertex_buffer_id_);
		buffer_id_manager_.RecycleId(_geometry->index_buffer_id_);
	}

	if (option == kGroRemoveFromMaterial) {
		OpenGLMaterial* mat = (OpenGLMaterial*)GetMaterial(_geometry->material_type_);
		if (mat->RemoveGeometry(user_geometry) == Material::kNotRemoved) {
			// Make sure the geometry is properly removed -- otherwise we have a leak!
			// In case fallback materials are disabled, the material may not have added
			// the geometry in the first place.
			deb_assert(false);
		}
	}

	for (LightDataMap::iterator x = light_data_map_.begin(); x != light_data_map_.end(); ++x) {
		LightData* _light = x->second;
		_light->shadow_map_geometry_set_.Remove(_geometry);
	}

	//OGL_FAST_ASSERT();
}

void OpenGLRenderer::AppendWireframeLines(tbc::GeometryBase* geometry) {
	if (geometry->GetPrimitiveType() != tbc::GeometryBase::kTriangles || geometry->CheckFlag(tbc::GeometryBase::kContainsWireframe)) {
		return;
	}
	geometry->SetFlag(tbc::GeometryBase::kContainsWireframe);
	if (!geometry->GetEdgeData()) {
		geometry->GenerateEdgeData();
	}
	const int line_count = geometry->GetEdgeCount();
	const int base_index = geometry->GetIndexCount();
	const int total_index_count = base_index+line_count*2;
	vtx_idx_t* index_data = new vtx_idx_t[total_index_count];
	::memcpy(index_data, geometry->GetIndexData(), geometry->GetIndexCount()*sizeof(vtx_idx_t));
	const tbc::GeometryBase::Edge* edges = geometry->GetEdgeData();
	for (int i = 0; i < line_count; ++i) {
		index_data[i*2+0+base_index] = edges[i].vertex_[0];
		index_data[i*2+1+base_index] = edges[i].vertex_[1];
	}
	geometry->SetIndexData(index_data, geometry->GetIndexCount(), total_index_count);
}


bool OpenGLRenderer::ChangeMaterial(GeometryID geometry_id, MaterialType material_type) {
	if ((int)material_type < 0 || (int)material_type >= kMatCount) {
		log_.Errorf("ChangeMaterial - Material %i is not a valid material ID!", (int)material_type);
		return (false);
	}
	OpenGLMaterial* mat = (OpenGLMaterial*)GetMaterial(material_type);
	if (mat == 0) {
		log_.Errorf("ChangeMaterial - Material %i is not implemented!", (int)material_type);
		return (false);
	}

	GeometryTable::Iterator iter;
	iter = GetGeometryTable().Find(geometry_id);

	bool ok = (iter != GetGeometryTable().End());
	if (ok) {
		OGLGeometryData* _geometry = (OGLGeometryData*)*iter;
		if (_geometry->material_type_ != material_type) {
			mat = (OpenGLMaterial*)GetMaterial(_geometry->material_type_);
			mat->RemoveGeometry(_geometry->geometry_);
			_geometry->material_type_ = material_type;
			mat = (OpenGLMaterial*)GetMaterial(_geometry->material_type_);
			ok = mat->AddGeometry(_geometry->geometry_);
		}
	}
	OGL_FAST_ASSERT();
	return (ok);
}



bool OpenGLRenderer::PreRender(tbc::GeometryBase* geometry) {
	if (geometry->IsTwoSided()) {
		::glDisable(GL_CULL_FACE);
	}
	// Check if we can avoid double-rendering (if in unlit mode).
	if (geometry->IsRecvNoShadows()) {
		const bool lights_enabled = GetLightsEnabled();
		if (GetShadowMode() != kNoShadows && !lights_enabled) {
			return false;
		} else if (lights_enabled) {
			::glDisable(GL_STENCIL_TEST);
		}
	}

	const xform& t = geometry->GetTransformation();
	if (geometry->IsExcludeCulling() || CheckCamCulling(t.GetPosition(), geometry->GetBoundingRadius())) {
		visible_triangle_count_ += geometry->GetTriangleCount();
		cam_space_transformation_.FastInverseTransform(camera_actual_transformation_, camera_actual_orientation_inverse_, t);
		float model_view_matrix[16];
		cam_space_transformation_.GetAs4x4TransposeMatrix(geometry->GetScale(), model_view_matrix);
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(model_view_matrix);
		return true;
	} else {
		culled_triangle_count_ += geometry->GetTriangleCount();
	}
	return false;
}

void OpenGLRenderer::PostRender(tbc::GeometryBase* geometry) {
	geometry->SetTransformationChanged(false);
	if (geometry->IsRecvNoShadows()) {
		if (GetLightsEnabled()) {
			::glEnable(GL_STENCIL_TEST);
		}
	}
	if (geometry->IsTwoSided()) {
		::glEnable(GL_CULL_FACE);
	}
}



void OpenGLRenderer::DrawLine(const vec3& position, const vec3& vector, const Color& color) {
	glEnable(GL_DEPTH_TEST);
	xform cam_transform = GetCameraActualTransformation().InverseTransform(kIdentityTransformationF);
	float model_view_matrix[16];
	cam_transform.GetAs4x4TransposeMatrix(model_view_matrix);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(model_view_matrix);

	glColor4ub(color.red_, color.green_, color.blue_, 255);
	GLfloat v[] = {position.x, position.y, position.z, position.x+vector.x, position.y+vector.y, position.z+vector.z};
	::glVertexPointer(3, GL_FLOAT, 0, v);
	::glDrawArrays(GL_LINES, 0, 2);

	OGL_FAST_ASSERT();
}



unsigned OpenGLRenderer::RenderScene() {
	LEPRA_MEASURE_SCOPE(RenderScene);

	OGL_ASSERT();

	{
		// Prepare projection data in order to be able to call CheckCulling().
		PrepareProjectionData();

		::glMatrixMode(GL_MODELVIEW);
		::glLoadIdentity();

		// Transform lights (among other things); must also run when lighting is disabled.
		ProcessLights();

		//Reset viewport and frustum every frame. This may look silly,
		//but the Get-functions will return the values stored in Renderer,
		//while the Set-functions are virtual and will call the OpenGLRenderer-ditto.
		SetViewport(GetViewport());
		float _fov_angle;
		float _near;
		float _far;
		GetViewFrustum(_fov_angle, _near, _far);
		SetViewFrustum(_fov_angle, _near, _far);

		CalcCamCulling();

		::glDisable(GL_COLOR_LOGIC_OP);
		::glDisable(GL_ALPHA_TEST);
		::glDisable(GL_BLEND);
		::glEnable(GL_CULL_FACE);
		::glEnable(GL_DEPTH_TEST);
		::glDepthMask(GL_TRUE);
		::glFrontFace(GL_CCW);
		::glShadeModel(GL_SMOOTH);
		::glDepthFunc(GL_LESS);
		::glCullFace(GL_BACK);
		::glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		::glEnable(GL_LINE_SMOOTH);
		if (GetFogFar() > 0) {
			::glEnable(GL_FOG);
			::glFogf(GL_FOG_MODE, (GetFogExponent()>0)? (float)GL_EXP : (float)GL_LINEAR);
			::glFogf(GL_FOG_DENSITY, GetFogDensity());
			::glFogf(GL_FOG_START, GetFogNear());
			::glFogf(GL_FOG_END, GetFogFar());
			const float fog_color[4] = {clear_color_.GetRf(), clear_color_.GetGf(), clear_color_.GetBf(), clear_color_.GetAf()};
			::glFogfv(GL_FOG_COLOR, fog_color);
		} else {
			::glDisable(GL_FOG);
		}
		Material::EnableDrawMaterial(true);
		Material::EnableWireframe(IsWireframeEnabled());

		if (!IsPixelShadersEnabled()) {
			OpenGLMatPXS::CleanupShaderPrograms();
		}
	}

	float ambient_red, ambient_green, ambient_blue;
	GetAmbientLight(ambient_red, ambient_green, ambient_blue);

	if (GetShadowMode() != kNoShadows && GetLightsEnabled()) {
		UpdateShadowMaps();

		EnableGlLights(this, light_data_map_, false);

		if (IsOutlineRenderingEnabled()) {
			SetAmbientLight(ambient_red*0.5f, ambient_green*0.5f, ambient_blue*0.5f);
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(-1, -2000);
		}

		// Prepare the pixel shader materials.
		OpenGLMatPXS::PrepareLights(this);

		// Render the scene darkened.
		for (int i = 0; i <= (int)kMatLastSolid; i++) {
			Material* material = GetMaterial((MaterialType)i);
			if (material == 0) {
				continue;
			}
			Material::RenderAllGeometry(GetCurrentFrame(), material);
			OGL_FAST_ASSERT();
		}

		// Render the outline before shadow volumes, to avoid obfuscation.
		if (IsOutlineRenderingEnabled()) {
			EnableGlLights(this, light_data_map_, true);
			glPolygonOffset(0, 0);
			glDisable(GL_POLYGON_OFFSET_FILL);
			SetAmbientLight(ambient_red, ambient_green, ambient_blue);
			Material::EnableWireframe(true);
			Material::RenderAllGeometry(GetCurrentFrame(), GetMaterial(kMatSingleColorSolid));
			Material::RenderAllGeometry(GetCurrentFrame(), GetMaterial(kMatSingleColorSolidPxs), GetMaterial(kMatSingleColorSolid));
			Material::RenderAllGeometry(GetCurrentFrame(), GetMaterial(kMatSingleColorEnvmapSolid), GetMaterial(kMatSingleColorSolid));
			Material::RenderAllGeometry(GetCurrentFrame(), GetMaterial(kMatSingleColorOutlineBlended));
			Material::EnableWireframe(false);
			EnableGlLights(this, light_data_map_, false);
		}

		// Shadow stencil buffer operations.
		//::glEnable(GL_STENCIL_TEST);
		//::glStencilFunc(GL_ALWAYS, 128, 0xFF);
		//::glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);

		// Render all shadow maps. In this step, we only render the shadows
		// using alpha testing to the stencil buffer (no output to the color buffer).
		if (GetNumSpotLights() > 0 && GetShadowHint() == kShVolumesAndMaps) {
			RenderShadowMaps();
		}

		// Render scene with stencil shadows.
		RenderShadowVolumes();

		// Go back to normal stencil buffer operations.
		::glDepthFunc(GL_LEQUAL);
		::glEnable(GL_STENCIL_TEST);
		::glStencilFunc(GL_GEQUAL, 128, 0xFF);
		::glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

		EnableGlLights(this, light_data_map_, true);
	}

	bool skip_outlined = false;
	if (IsOutlineRenderingEnabled() && !IsWireframeEnabled()) {
		Material::EnableDrawMaterial(false);
		SetAmbientLight(1, 1, 1);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(-1, -2600);
		const vec3 _color(outline_fill_color_.GetRf(), outline_fill_color_.GetGf(), outline_fill_color_.GetBf());
		tbc::GeometryBase::BasicMaterialSettings material(vec3(1, 1, 1), _color, vec3(), 1, 1, false);
		OpenGLMaterial::SetBasicMaterial(material, this);
		Material::RenderAllGeometry(GetCurrentFrame(), GetMaterial(kMatSingleColorSolid));
		Material::RenderAllGeometry(GetCurrentFrame(), GetMaterial(kMatSingleColorSolidPxs), GetMaterial(kMatSingleColorSolid));
		Material::RenderAllGeometry(GetCurrentFrame(), GetMaterial(kMatSingleColorEnvmapSolid), GetMaterial(kMatSingleColorSolid));
		Material::RenderAllGeometry(GetCurrentFrame(), GetMaterial(kMatSingleColorOutlineBlended));
		Material::EnableDrawMaterial(true);

		skip_outlined = true;
	}

	SetAmbientLight(ambient_red, ambient_green, ambient_blue);

	{
		// Prepare the pixel shader materials.
		OpenGLMatPXS::PrepareLights(this);
	}
	{
		// This renders the scene.
		for (int i = 0; i < (int)kMatCount; ++i) {
			if (skip_outlined && (i == kMatSingleColorSolid || i == kMatSingleColorOutlineBlended ||
				i == kMatSingleColorEnvmapSolid || i == kMatSingleColorSolidPxs)) {
				continue;
			}
			if (i == kMatLastSolid) {
				::glDisable(GL_STENCIL_TEST);
			}
			Material* material = GetMaterial((MaterialType)i);
			if (material != 0) {
				Material::RenderAllGeometry(GetCurrentFrame(), material);
			}
		}

		DynamicRendererMap::iterator x = dynamic_renderer_map_.begin();
		for (; x != dynamic_renderer_map_.end(); ++x) {
			x->second->Render();
		}
	}

	{
		for (LightDataMap::iterator x = light_data_map_.begin(); x != light_data_map_.end(); ++x) {
			LightData* _light = x->second;
			_light->transformation_changed_ = false;
		}

		StepCurrentFrame();
	}

	{
		::glDisable(GL_BLEND);
		::glDisable(GL_TEXTURE_2D);
		::glDisable(GL_LIGHTING);
		::glDisable(GL_COLOR_MATERIAL);
		::glDisable(GL_NORMALIZE);
		::glDisable(GL_FOG);
		::glDisableClientState(GL_NORMAL_ARRAY);
		//::glDisableClientState(GL_INDEX_ARRAY);
		::glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		if (uilepra::OpenGLExtensions::IsBufferObjectsSupported() == true) {
			uilepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, 0);
			uilepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
		glPolygonOffset(0, 0);
		glDisable(GL_POLYGON_OFFSET_FILL);
	}

	OGL_ASSERT();

	return (GetCurrentFrame());
}

void OpenGLRenderer::RenderBillboards(tbc::GeometryBase* geometry, bool render_texture, bool addative_blending, const BillboardRenderInfoArray& billboards) {
	if (billboards.size() == 0) {
		return;
	}

	OGL_FAST_ASSERT();

	Material* material;
	if (render_texture) {
		material = GetMaterial(kMatSingleTextureBlended);
		OpenGLRenderer::OGLGeometryData* _geometry = (OpenGLRenderer::OGLGeometryData*)geometry->GetRendererData();
		((OpenGLMatSingleTextureBlended*)material)->BindTexture(
			_geometry->ta_->maps_[0].map_id_[Texture::kColorMap],
			_geometry->ta_->maps_[0].mip_map_level_count_[Texture::kColorMap]);
		::glMatrixMode(GL_TEXTURE);
		::glLoadIdentity();
		::glMatrixMode(GL_MODELVIEW);
	} else {
		material = GetMaterial(kMatSingleColorBlended);
	}
	material->PreRender();
	if (addative_blending) {
		::glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	} else {
		::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	::glEnable(GL_DEPTH_TEST);
	::glDepthMask(GL_FALSE);
	::glDisable(GL_CULL_FACE);
	::glDisableClientState(GL_NORMAL_ARRAY);
	::glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	::glDisable(GL_LIGHTING);
	::glDisable(GL_COLOR_MATERIAL);
	::glDisable(GL_NORMALIZE);
	::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	xform cam_space;
	//tbc::GeometryBase::BasicMaterialSettings lMaterialSettings(vec3(), vec3(), vec3(), 0, 0, false);
	BillboardRenderInfoArray::const_iterator x = billboards.begin();
	for (; x != billboards.end(); ++x) {
		//lMaterialSettings.diffuse_ = x->color_;
		//lMaterialSettings.alpha_ = x->opacity_;
		//material->SetBasicMaterial(lMaterialSettings);
		glColor4f(x->color_.x, x->color_.y, x->color_.z, x->opacity_);

		quat rot = camera_transformation_.GetOrientation();
		rot.RotateAroundOwnY(x->angle_);
		cam_space.FastInverseTransform(camera_actual_transformation_, camera_actual_orientation_inverse_, xform(rot, x->position_));
		float model_view_matrix[16];
		cam_space.GetAs4x4TransposeMatrix(x->scale_, model_view_matrix);
		::glLoadMatrixf(model_view_matrix);

		material->RawRender(geometry, x->uv_index_);
	}
	material->PostRender();
	::glDepthMask(GL_TRUE);
	//::glEnable(GL_NORMALIZE);
	::glDisable(GL_BLEND);
	::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	if (!GetTexturingEnabled()) {
		::glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		::glDisable(GL_TEXTURE_2D);
	}

	OGL_FAST_ASSERT();
}

void OpenGLRenderer::RenderRelative(tbc::GeometryBase* geometry, const quat* light_orientation) {
	OGL_FAST_ASSERT();

	::glMatrixMode(GL_MODELVIEW);
	::glPushMatrix();
	float model_view_matrix[16];
	geometry->GetTransformation().GetAs4x4TransposeMatrix(geometry->GetScale(), model_view_matrix);
	::glLoadMatrixf(model_view_matrix);

	::glEnable(GL_DEPTH_TEST);

	const LightID light_id = GetClosestLight(GetLightCount()-1);
	const LightData* light_data = GetLightData(light_id);
	if (light_orientation) {
		::glEnable(GL_LIGHTING);
		quat orientation = geometry->GetTransformation().GetOrientation().GetInverse();
		LightData data_copy = *light_data;
		orientation *= *light_orientation;
		data_copy.direction_ = orientation * light_data->direction_;
		SetupGLLight(light_id, &data_copy);
	}

	GeometryData* geometry_data = (GeometryData*)geometry->GetRendererData();
	if (geometry_data) {
		if (geometry->IsTwoSided()) {
			::glDisable(GL_CULL_FACE);
		}
		GetMaterial(geometry_data->material_type_)->PreRender();
		GetMaterial(geometry_data->material_type_)->RenderGeometry(geometry);
		GetMaterial(geometry_data->material_type_)->PostRender();
		if (geometry->IsTwoSided()) {
			::glEnable(GL_CULL_FACE);
		}
		ResetAmbientLight(false);
	}

	if (light_orientation) {
		SetupGLLight(light_id, light_data);
	}

	PostRender(geometry);

	::glDisable(GL_LIGHTING);
	::glDisable(GL_DEPTH_TEST);

	::glMatrixMode(GL_MODELVIEW);
	::glPopMatrix();

	OGL_FAST_ASSERT();
}

int OpenGLRenderer::GetEnvMapID() {
	if (GetEnvTexture() != 0) {
		if (GetEnvTexture()->is_cube_map_ == false) {
			return GetEnvTexture()->t_map_id_[Texture::kColorMap];
		} else {
			return GetEnvTexture()->t_map_id_[Texture::kCubeMap];
		}
	}

	return t_map_id_manager_.GetInvalidId();
}


int OpenGLRenderer::GetNumTextureUnits() const {
	return num_texture_units_;
}

void OpenGLRenderer::ProcessLights() {
	SetLightsEnabled(GetLightsEnabled());

	// Transform all light positions.
	for (LightDataMap::iterator x = light_data_map_.begin(); x != light_data_map_.end(); ++x) {
		LightData* _light = x->second;

		if (_light->type_ == kLightDirectional) {
			vec3 light_dir = GetCameraActualOrientationInverse() * _light->direction_;
			float _pos[4] =
			{
				(float)-light_dir.x,
				(float)-light_dir.y,
				(float)-light_dir.z,
				0.0f
			};
			glLightfv(GL_LIGHT0 + x->first, GL_POSITION, _pos);
		} else if(_light->type_ == kLightPoint) {
			vec3 light_pos = GetCameraActualTransformation().InverseTransform(_light->position_);
			float _pos[4] =
			{
				(float)light_pos.x,
				(float)light_pos.y,
				(float)light_pos.z,
				1.0f
			};
			glLightfv(GL_LIGHT0 + x->first, GL_POSITION, _pos);
		} else if(_light->type_ == kLightSpot) {
			vec3 light_pos = GetCameraActualTransformation().InverseTransform(_light->position_);
			vec3 light_dir = GetCameraActualOrientationInverse() * _light->direction_;
			float _pos[4] =
			{
				(float)light_pos.x,
				(float)light_pos.y,
				(float)light_pos.z,
				1.0f
			};

			float _dir[3] =
			{
				(float)light_dir.x,
				(float)light_dir.y,
				(float)light_dir.z
			};

			glLightfv(GL_LIGHT0 + x->first, GL_POSITION, _pos);
			glLightfv(GL_LIGHT0 + x->first, GL_SPOT_DIRECTION, _dir);

			if (GetShadowHint() == kShVolumesAndMaps) {
				if (_light->transformation_changed_ == true) {
					if (_light->shadow_range_ > 0) {
						_light->shadow_map_need_update_ = true;
					}
					_light->transformation_changed_ = false;
				}

				if (_light->shadow_map_need_update_ == true)
					RegenerateShadowMap(_light);
			}
		}
	}
	OGL_FAST_ASSERT();
}

void OpenGLRenderer::RenderShadowVolumes() {
	OGL_FAST_ASSERT();

	// Disable all fancy gfx.
#if !defined(LEPRA_GL_ES) && !defined(LEPRA_MAC)
	//glPushAttrib(GL_ENABLE_BIT | GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glDisable(GL_LOGIC_OP);
	//glEnable(GL_POLYGON_OFFSET_EXT);
#endif // !GLES
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_NORMALIZE);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glShadeModel(GL_FLAT);
	if (!is_solid_shadows_enabled_) {
		glStencilFunc(GL_ALWAYS, 128, 0xFF);
		glColorMask(0, 0, 0, 0);
		glDepthMask(GL_FALSE);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_STENCIL_TEST);
	} else {
		glDisable(GL_CULL_FACE);
	}

	glDepthFunc(GL_LESS);
	//glEnable(GL_POLYGON_OFFSET_FILL);
	//glPolygonOffset(1.5f, 700);

	glMatrixMode(GL_MODELVIEW);

	// Offset shadow in light direction to avoid Z-fighting.
	vec3 cam_shadow_offset;
	for (LightDataMap::iterator x = light_data_map_.begin(); x != light_data_map_.end(); ++x) {
		LightData* _light = x->second;
		if (_light->enabled_ && _light->type_ == kLightDirectional) {
			cam_shadow_offset = _light->direction_*0.1f;
		}
	}

	// Render shadow volumes in two steps.
	const int steps = is_solid_shadows_enabled_ ? 1 : 2;
	for (int step = 0; step < steps; step++) {
		if (is_solid_shadows_enabled_) {
			// Nothing.
		} else if (step == 0) {
			// Render back faces.
			glCullFace(GL_FRONT);
			glStencilOp(GL_KEEP, GL_INCR, GL_KEEP);
		} else { // step == 1
			// Render front faces.
			glCullFace(GL_BACK);
			glStencilOp(GL_KEEP, GL_DECR, GL_KEEP);
		}

		ShadowVolumeTable::Iterator iter;
		for (iter = GetShadowVolumeTable().First(); iter != GetShadowVolumeTable().End(); ++iter) {
			OGLGeometryData* shadow_geometry = (OGLGeometryData*)*iter;
			ShadowVolume* _shadow_volume = (ShadowVolume*)shadow_geometry->geometry_;

			if (_shadow_volume->GetParentGeometry()->GetAlwaysVisible() == true ||
			   _shadow_volume->GetParentGeometry()->GetLastFrameVisible() == GetCurrentFrame()) {
				xform shadow_transformation(_shadow_volume->GetTransformation());
				shadow_transformation.GetPosition() += cam_shadow_offset;
				cam_space_transformation_.FastInverseTransform(camera_actual_transformation_, camera_actual_orientation_inverse_, shadow_transformation);
				float model_view_matrix[16];
				cam_space_transformation_.GetAs4x4TransposeMatrix(_shadow_volume->GetScale(), model_view_matrix);
				::glLoadMatrixf(model_view_matrix);

				if (uilepra::OpenGLExtensions::IsBufferObjectsSupported() == true) {
					GLuint vertex_buffer_id = (GLuint)shadow_geometry->vertex_buffer_id_;
					GLuint index_buffer_id  = (GLuint)shadow_geometry->index_buffer_id_;

					//log_volatile(mLog.Tracef("glBindBuffer %u (vertex."), lShadowGeometry->mVertexBufferID));
					uilepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
					glVertexPointer(3, GL_FLOAT, 0, 0);

					if (_shadow_volume->GetVertexDataChanged() == true) {
						// Upload new vertices.
						uilepra::OpenGLExtensions::glBufferSubData(GL_ARRAY_BUFFER, 0,
							_shadow_volume->GetVertexCount() * sizeof(float) * 3, (void*)_shadow_volume->GetVertexData());
						_shadow_volume->SetVertexDataChanged(false);
					}

					uilepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_id);

					if (_shadow_volume->GetIndexDataChanged() == true) {
						uilepra::OpenGLExtensions::glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0,
							_shadow_volume->GetTriangleCount() * 3 * sizeof(vtx_idx_t), (void*)_shadow_volume->GetIndexData());
					}

					glDrawElements(GL_TRIANGLES, _shadow_volume->GetTriangleCount() * 3, LEPRA_GL_INDEX_TYPE, 0);
				} else {
					glVertexPointer(3, GL_FLOAT, 0, _shadow_volume->GetVertexData());
					glDrawElements(GL_TRIANGLES, _shadow_volume->GetTriangleCount() * 3, LEPRA_GL_INDEX_TYPE, _shadow_volume->GetIndexData());
				}
			}
		}
	}

	// Reset all settings.
	glShadeModel(GL_SMOOTH);
	glColorMask(1, 1, 1, 1);
	glDepthFunc(GL_LEQUAL);
	//glPolygonOffset(0, 0);
	//glDisable(GL_POLYGON_OFFSET_FILL);
	glDepthMask(GL_TRUE);

	//OGL_FAST_ASSERT();
	glGetError();	// TRICKY: for some reason when recording with Fraps we get an error here.
}

int OpenGLRenderer::RenderShadowMaps() {
	int count = 0;
#ifndef LEPRA_GL_ES

	OGL_FAST_ASSERT();

	// TODO: use flat maps instead of cube maps for directional/spot shadow maps.

	//glPushAttrib(GL_ENABLE_BIT | GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT | GL_VIEWPORT_BIT);
	//glDisable(GL_LOGIC_OP);
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_NORMALIZE);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glShadeModel(GL_FLAT);

	glEnable(GL_TEXTURE_2D);
	glColorMask(0, 0, 0, 1);

	glEnableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);
	glEnable(GL_TEXTURE_GEN_Q);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);


	static const GLfloat s_plane_[4] = {1.0f, 0.0f, 0.0f, 0.0f};
	static const GLfloat t_plane_[4] = {0.0f, 1.0f, 0.0f, 0.0f};
	static const GLfloat r_plane_[4] = {0.0f, 0.0f, 1.0f, 0.0f};
	static const GLfloat q_plane_[4] = {0.0f, 0.0f, 0.0f, 1.0f};

	glTexGenfv(GL_S, GL_EYE_PLANE, s_plane_);
	glTexGenfv(GL_T, GL_EYE_PLANE, t_plane_);
	glTexGenfv(GL_R, GL_EYE_PLANE, r_plane_);
	glTexGenfv(GL_Q, GL_EYE_PLANE, q_plane_);

	glEnable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glAlphaFunc(GL_LESS, 0.5f);

	for (LightDataMap::iterator x = light_data_map_.begin(); x != light_data_map_.end(); ++x) {
		LightData* _light = x->second;

		if (_light->shadow_map_id_ != 0) {
			glBindTexture(GL_TEXTURE_2D, _light->shadow_map_id_);

			// Prepare the texture projection matrix.
			static float texture_matrix_[16];
			glMatrixMode(GL_TEXTURE);
			glLoadIdentity();
			glTranslatef(0.5f, 0.5f, 0.5f);
			glScalef(0.5f, 0.5f, 0.5f);
			glMultMatrixf(_light->light_projection_matrix_);
			glGetFloatv(GL_TEXTURE_MATRIX, texture_matrix_);

			LightData::GeometrySet::Iterator geo_iter;
			for (geo_iter = _light->shadow_map_geometry_set_.First();
				geo_iter != _light->shadow_map_geometry_set_.End();
				++geo_iter) {
				OGLGeometryData* _geometry = (OGLGeometryData*)*geo_iter;

				float model_view_matrix[16];
				GetCameraActualTransformation().InverseTransform(_geometry->geometry_->GetTransformation()).GetAs4x4TransposeMatrix(model_view_matrix);

				// Camera model view matrix.
				glMatrixMode(GL_MODELVIEW);
				glLoadMatrixf(model_view_matrix);

				float light_model_view_matrix[16];
				xform light_transformation(_light->orientation_, _light->position_);
				light_transformation.orientation_ *= camera_actual_rotation_;
				(light_transformation.InverseTransform(_geometry->geometry_->GetTransformation())).GetAs4x4TransposeMatrix(light_model_view_matrix);

				// Setup the texture projection matrix.
				glMatrixMode(GL_TEXTURE);
				glLoadMatrixf(texture_matrix_);
				glMultMatrixf(light_model_view_matrix);

				// Need to call these here to update the model view transform.
				glTexGenfv(GL_S, GL_EYE_PLANE, s_plane_);
				glTexGenfv(GL_T, GL_EYE_PLANE, t_plane_);
				glTexGenfv(GL_R, GL_EYE_PLANE, r_plane_);
				glTexGenfv(GL_Q, GL_EYE_PLANE, q_plane_);

				if (uilepra::OpenGLExtensions::IsBufferObjectsSupported() == true) {
					GLuint vertex_buffer_id = (GLuint)_geometry->vertex_buffer_id_;
					GLuint index_buffer_id  = (GLuint)_geometry->index_buffer_id_;

					//log_volatile(mLog.Tracef("glBindBuffer %u (vertex."), lGeometry->mVertexBufferID));
					uilepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
					glVertexPointer(3, GL_FLOAT, 0, 0);

					uilepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_id);

					glDrawElements(GL_TRIANGLES,
								_geometry->geometry_->GetTriangleCount() * 3,
								LEPRA_GL_INDEX_TYPE,
								0);
				} else {
					glVertexPointer(3, GL_FLOAT, 0, _geometry->geometry_->GetVertexData());
					glDrawElements(GL_TRIANGLES,
								_geometry->geometry_->GetTriangleCount() * 3,
								LEPRA_GL_INDEX_TYPE,
								_geometry->geometry_->GetIndexData());
				}
			}

			count++;
		}
	}

	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_GEN_Q);
	glDisable(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);

	//glPopAttrib();
	glColorMask(1, 1, 1, 1);

	glDisable(GL_TEXTURE_2D);

	OGL_FAST_ASSERT();
#endif // !GLES

	return count;
}

void OpenGLRenderer::RegenerateShadowMap(LightData* light) {
	//glPushAttrib(GL_ENABLE_BIT | GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT | GL_VIEWPORT_BIT);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_NORMALIZE);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	//glDisable(GL_LOGIC_OP);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glShadeModel(GL_FLAT);
	glColorMask(0, 0, 0, 0);

	// Set viewport.
	glViewport(0, 0, light->shadow_map_res_, light->shadow_map_res_);

	// Set projection matrix.
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	Perspective(light->cutoff_angle_ * 2.0f, 1.0f, light->shadow_map_near_, light->shadow_map_far_);

	glGetFloatv(GL_PROJECTION_MATRIX, light->light_projection_matrix_);

	glMatrixMode(GL_MODELVIEW);


	xform light_transformation(light->orientation_, light->position_);

	// Clear depth buffer.
	glClear(GL_DEPTH_BUFFER_BIT);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);

	// Render "scene" with no fx, and only to the depth buffer.
	// The scene in this case are all objects within the light's bounding radius.

	// Overcome imprecision.
	//glEnable(GL_POLYGON_OFFSET_FILL);
	//glPolygonOffset(1.5f, 700);

	LightData::GeometrySet::Iterator iter;
	for (iter = light->shadow_map_geometry_set_.First();
		iter != light->shadow_map_geometry_set_.End();
		++iter) {
		OGLGeometryData* _geometry = (OGLGeometryData*)*iter;

		if (_geometry->shadow_ >= kCastShadows) {
			float model_view_matrix[16];
			(light_transformation.InverseTransform(_geometry->geometry_->GetTransformation())).GetAs4x4TransposeMatrix(model_view_matrix);
			glLoadMatrixf(model_view_matrix);

			if (uilepra::OpenGLExtensions::IsBufferObjectsSupported() == true) {
				GLuint vertex_buffer_id = (GLuint)_geometry->vertex_buffer_id_;
				GLuint index_buffer_id  = (GLuint)_geometry->index_buffer_id_;

				//log_volatile(mLog.Tracef("glBindBuffer %u (vertex."), lGeometry->mVertexBufferID));
				uilepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
				glVertexPointer(3, GL_FLOAT, 0, 0);

				uilepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_id);

				glDrawElements(GL_TRIANGLES,
							_geometry->geometry_->GetTriangleCount() * 3,
							LEPRA_GL_INDEX_TYPE,
							0);
			} else {
				glVertexPointer(3, GL_FLOAT, 0, _geometry->geometry_->GetVertexData());
				glDrawElements(GL_TRIANGLES,
							_geometry->geometry_->GetTriangleCount() * 3,
							LEPRA_GL_INDEX_TYPE,
							_geometry->geometry_->GetIndexData());
			}
		}
	}

	if (light->shadow_map_id_ == 0) {
		// Create a new depth texture.
		light->shadow_map_id_ = t_map_id_manager_.GetFreeId();
	}

	// Bind the texture.
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, light->shadow_map_id_);

#ifndef LEPRA_GL_ES
	// And finally read it from the back buffer.
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
					 0, 0, light->shadow_map_res_, light->shadow_map_res_, 0);
#endif // !GLES

	//glDisable(GL_POLYGON_OFFSET_FILL);
	//glPolygonOffset(0, 0);
	glDisable(GL_TEXTURE_2D);
	//glPopAttrib();
	glShadeModel(GL_SMOOTH);
	glColorMask(1, 1, 1, 1);
	if (GetLightsEnabled()) {
		glEnable(GL_LIGHTING);
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	light->shadow_map_need_update_ = false;

	OGL_FAST_ASSERT();
}

void OpenGLRenderer::Perspective(float fov_angle, float aspect_ratio, float cam_near, float cam_far) {
	/*
	    Looking at the FOV from above.

	    \             /  <- Field of view.
	     \           /
	   -1 l----x----r 1  <- Projection plane (width = 2). X marks the center,
	       \   |   /        l = left at -1, and r = right at 1.
	        \  |D /   <- D = Distance from eye position to projection plane.
	     	 \ | /
	     	  \|/
	     	   *  <- Eye position.

	   Consider the triangle *-x-r.

	   The angle between the line D, and the FOV's right side is halv the
	   angle of the FOV, which gives us FOVAngle / 2.0f.

	   Now notice that OpenGL consideres the screen to be a square area with
	   the coordinates (-1, 1) at left and right screen edges, and (-1, 1)
	   at the bottom and top screen edges as well.

	   Thus, the distance between x and r is 1.0, which is half the width of
	   the projection plane. From no on, "1.0f" represents this distance.

	   Recalling what we have learned in school about trigonometry, we know
	   that 1.0f / D = tan(FOVAngle / 2.0f).

	   This implies D = 1.0f / tan(FOVAngle / 2.0f).

	   But since this is seen from above, and because of the fact that the
	   screen area isn't square, this D can only be used for the X-coordinate
	   projection. The corresponding value for the Y-coordinates is simply
	   D * "aspect ratio".
	*/

	float dy = 1.0f / tan(Math::Deg2Rad(fov_angle) / 2.0f);
	float dx = dy / aspect_ratio;
	if (GetScreen()->GetOutputRotation()%180 != 0) {
		std::swap(dx, dy);
	}

	float projection_matrix[16];

	projection_matrix[0]  = dx;
	projection_matrix[1]  = 0;
	projection_matrix[2]  = 0;
	projection_matrix[3]  = 0;

	projection_matrix[4]  = 0;
	projection_matrix[5]  = dy;
	projection_matrix[6]  = 0;
	projection_matrix[7]  = 0;

	projection_matrix[8]  = 0;
	projection_matrix[9]  = 0;
	projection_matrix[10] = -(cam_far + cam_near) / (cam_far - cam_near);
	projection_matrix[11] = -1;

	projection_matrix[12] = 0;
	projection_matrix[13] = 0;
	projection_matrix[14] = (-2.0f * cam_far * cam_near) / (cam_far - cam_near);
	projection_matrix[15] = 0;

	::glMatrixMode(GL_PROJECTION);
	::glLoadMatrixf(projection_matrix);
	::glRotatef((float)GetScreen()->GetOutputRotation(), 0, 0, 1);

	::glMatrixMode(GL_MODELVIEW);

	OGL_FAST_ASSERT();
}

void OpenGLRenderer::SetPixelFormat(int& _size, GLenum& pixel_format, bool compress, const str& error_message) {
	switch(_size) {
	case 4:	pixel_format	= GL_RGBA;	break;
	case 3:	pixel_format	= GL_RGB;	break;
	case 1:	pixel_format	= GL_LUMINANCE;	break;
	default:
		pixel_format	= GL_RGB;
		log_.Info(error_message);
	break;
	}

#ifndef LEPRA_GL_ES
	if (compress == true) {
		_size = (_size == 4) ? GL_COMPRESSED_RGBA : GL_COMPRESSED_RGB;
	} else
#endif // !GLES
	{
		switch(_size) {
			case 4:		_size = GL_RGBA;	break;
			case 3:		_size = GL_RGB;		break;
			case 1:		_size = GL_ALPHA;	break;
			default:	_size = GL_RGB;		break;
		}
	}
}



loginstance(kUiGfx3D, OpenGLRenderer);



}
