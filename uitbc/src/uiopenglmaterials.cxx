
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../lepra/include/log.h"
#include "../../lepra/include/math.h"
#include "../../uilepra/include/uiopenglextensions.h"
#include "../../tbc/include/geometrybase.h"
#include "../include/uiopenglmaterials.h"
#include "../include/uiopenglrenderer.h"



#ifdef LEPRA_DEBUG
#define OGL_ASSERT()	{ GLenum gl_error = glGetError(); deb_assert(gl_error == GL_NO_ERROR); }
#else // !Debug
#define OGL_ASSERT()
#endif // Debug / !Debug



namespace uitbc {



OpenGLMaterial::OpenGLMaterial(OpenGLRenderer* renderer, Material::DepthSortHint sort_hint, Material* fall_back_material) :
	Material(renderer, sort_hint, fall_back_material) {
}

OpenGLMaterial::~OpenGLMaterial() {
}

Material::RemoveStatus OpenGLMaterial::RemoveGeometry(tbc::GeometryBase* geometry) {
	Material::RemoveStatus status = Parent::RemoveGeometry(geometry);
	if (status == Material::kNotRemoved && fall_back_material_) {
		status = fall_back_material_->RemoveGeometry(geometry);
		if (status == Material::kRemoved) {
			status = Material::kRemovedFromFallback;
		}
	}
	return status;
}

void OpenGLMaterial::EnableDisableTexturing() {
	GetRenderer()->SetTexturingEnabled(GetRenderer()->GetTexturingEnabled());
}

GLenum OpenGLMaterial::GetGLElementType(tbc::GeometryBase* geometry) {
	switch (geometry->GetPrimitiveType()) {
		case tbc::GeometryBase::kTriangles:
			if (enable_wireframe_) {
				return GL_LINES;
			}
			return (GL_TRIANGLES);
		case tbc::GeometryBase::kTriangleStrip:	return (GL_TRIANGLE_STRIP);
		case tbc::GeometryBase::kLines:		return (GL_LINES);
		case tbc::GeometryBase::kLineLoop:	return (GL_LINE_LOOP);
#ifndef LEPRA_GL_ES
		case tbc::GeometryBase::kQuads:		return (GL_QUADS);
#endif // !OpenGL ES
	}
	deb_assert(false);
	return (GL_TRIANGLES);
}

void OpenGLMaterial::SetBasicMaterial(const tbc::GeometryBase::BasicMaterialSettings& material) {
	SetBasicMaterial(material, GetRenderer());
}

void OpenGLMaterial::SetBasicMaterial(const tbc::GeometryBase::BasicMaterialSettings& material, Renderer* renderer) {
	current_material_ = material;

	const float ambient[]  = { material.ambient_.x,  material.ambient_.y,  material.ambient_.z,  material.alpha_ };
	const float diffuse[]  = { material.diffuse_.x,  material.diffuse_.y,  material.diffuse_.z,  material.alpha_ };
	const float specular[] = { material.specular_.x, material.specular_.y, material.specular_.z, material.alpha_ };

	::glColor4f(diffuse[0], diffuse[1], diffuse[2], diffuse[3]);
	::glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	::glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	::glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	::glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material.shininess_*0.5f);
	::glShadeModel(material.smooth_ ? GL_SMOOTH : GL_FLAT);
	//OGL_ASSERT();

	//renderer->SetGlobalMaterialReflectance(material.diffuse_.x, material.diffuse_.y, material.diffuse_.z, material.shininess_);

	if (material.ambient_.x || material.ambient_.y || material.ambient_.z) {
		renderer->AddAmbience(material.ambient_.x, material.ambient_.y, material.ambient_.z);
	}
}

void OpenGLMaterial::ResetBasicMaterial(const tbc::GeometryBase::BasicMaterialSettings& material) {
	if (material.ambient_.x || material.ambient_.y || material.ambient_.z) {
		GetRenderer()->ResetAmbientLight(true);
	}
}



void OpenGLMaterial::RenderAllBlendedGeometry(unsigned current_frame, const GeometryGroupList& geometry_group_list) {
	::glDepthMask(GL_FALSE);
	::glDisable(GL_CULL_FACE);
#ifndef LEPRA_GL_ES
	GLint old_fill[2];
	::glGetIntegerv(GL_POLYGON_MODE, old_fill);
	::glPolygonMode(GL_FRONT_AND_BACK, GetRenderer()->IsWireframeEnabled()? GL_LINE : GL_FILL);
#endif // !GLES
	Parent::RenderAllBlendedGeometry(current_frame, geometry_group_list);
	::glEnable(GL_CULL_FACE);
	::glDepthMask(GL_TRUE);
#ifndef LEPRA_GL_ES
	::glPolygonMode(GL_FRONT, old_fill[0]);
	::glPolygonMode(GL_BACK, old_fill[1]);
#endif // !GLES
}



void OpenGLMaterial::UpdateTextureMatrix(tbc::GeometryBase* geometry) {
	if (geometry->GetUVAnimator() != 0) {
		float uv_matrix[16];
		geometry->GetUVTransform().GetAs4x4TransposeMatrix(uv_matrix);
		glMatrixMode(GL_TEXTURE);
		glLoadMatrixf(uv_matrix);
	} else {
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
	}
	glMatrixMode(GL_MODELVIEW);
}



void OpenGLMatSingleColorSolid::RenderGeometry(tbc::GeometryBase* geometry) {
	SetBasicMaterial(geometry->GetBasicMaterialSettings());
	RawRender(geometry, 0);
	ResetBasicMaterial(geometry->GetBasicMaterialSettings());
}

void OpenGLMatSingleColorSolid::PreRender() {
	::glDisable(GL_ALPHA_TEST);
	::glDisable(GL_BLEND);
	//::glDisable(GL_LOGIC_OP);
	::glDisable(GL_TEXTURE_2D);

	::glEnableClientState(GL_VERTEX_ARRAY);
	::glEnableClientState(GL_NORMAL_ARRAY);
	::glDisableClientState(GL_COLOR_ARRAY);
	::glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	::glEnable(GL_COLOR_MATERIAL);
#ifndef LEPRA_GL_ES
	::glColorMaterial(GL_FRONT, GL_AMBIENT);
#endif // !GLES
}

void OpenGLMatSingleColorSolid::PostRender() {
	::glDisableClientState(GL_NORMAL_ARRAY);
	//::glDisableClientState(GL_VERTEX_ARRAY);
}

void OpenGLMatSingleColorSolid::RawRender(tbc::GeometryBase* geometry, int uv_set_index) {
	(void)uv_set_index;
	RenderBaseGeometry(geometry);
}

void OpenGLMatSingleColorSolid::RenderBaseGeometry(tbc::GeometryBase* geometry) {
	if (geometry->GetNormalData() == 0) {
		glDisableClientState(GL_NORMAL_ARRAY);
	} else {
		glEnableClientState(GL_NORMAL_ARRAY);
	}

	const int type_index = enable_wireframe_? 1 : 0;
	OpenGLRenderer::OGLGeometryData* _geometry = (OpenGLRenderer::OGLGeometryData*)geometry->GetRendererData();
	if (uilepra::OpenGLExtensions::IsBufferObjectsSupported() == true) {
		GLuint vertex_buffer_id = (GLuint)_geometry->vertex_buffer_id_;
		GLuint index_buffer_id  = (GLuint)_geometry->index_buffer_id_;

		uilepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
		glVertexPointer(3, GL_FLOAT, 0, (GLvoid*)_geometry->vertex_offset_);

		glNormalPointer(GL_FLOAT, 0, (GLvoid*)_geometry->normal_offset_);

		uilepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_id);
		glDrawElements(OpenGLMaterial::GetGLElementType(geometry),
			       _geometry->index_count_[type_index],
			       LEPRA_GL_INDEX_TYPE,
			       (GLvoid*)_geometry->index_offset_[type_index]);
	} else {
		glVertexPointer(3, GL_FLOAT, 0, geometry->GetVertexData());
		glNormalPointer(GL_FLOAT, 0, geometry->GetNormalData());
		glDrawElements(OpenGLMaterial::GetGLElementType(geometry),
			       _geometry->index_count_[type_index],
			       LEPRA_GL_INDEX_TYPE,
			       (char*)geometry->GetIndexData() + _geometry->index_offset_[type_index]);
	}
}



void OpenGLMatSingleColorBlended::RenderAllGeometry(unsigned current_frame, const GeometryGroupList& geometry_group_list) {
	if (outline_) {
		DoRenderAllGeometry(current_frame, geometry_group_list);
	} else {
		RenderAllBlendedGeometry(current_frame, geometry_group_list);
	}
}

void OpenGLMatSingleColorBlended::DoPreRender() {
	::glDisable(GL_ALPHA_TEST);
	::glEnable(GL_BLEND);
	::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	::glEnableClientState(GL_VERTEX_ARRAY);
	::glEnableClientState(GL_NORMAL_ARRAY);
	::glEnable(GL_COLOR_MATERIAL);
#ifndef LEPRA_GL_ES
	::glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
#endif // !GLES
}

void OpenGLMatSingleColorBlended::DoPostRender() {
	::glDisableClientState(GL_NORMAL_ARRAY);
	//::glDisableClientState(GL_VERTEX_ARRAY);
	::glDisable(GL_BLEND);
}

void OpenGLMatSingleColorBlended::PreRender() {
	DoPreRender();
}

void OpenGLMatSingleColorBlended::PostRender() {
	DoPostRender();
}



bool OpenGLMatVertexColorSolid::AddGeometry(tbc::GeometryBase* geometry) {
	if (!geometry->GetColorData()) {
		if (fall_back_material_) {
			log_.Warning("Material \"VertexColorSolid\", passing geometry to fallback material.");
			return fall_back_material_->AddGeometry(geometry);
		}
		return false;
	}

	return Parent::AddGeometry(geometry);
}

void OpenGLMatVertexColorSolid::PreRender() {
	Parent::PreRender();
	::glEnableClientState(GL_COLOR_ARRAY);
}

void OpenGLMatVertexColorSolid::RenderGeometry(tbc::GeometryBase* geometry) {
	SetBasicMaterial(geometry->GetBasicMaterialSettings());
	RawRender(geometry, 0);
	ResetBasicMaterial(geometry->GetBasicMaterialSettings());
}

void OpenGLMatVertexColorSolid::RawRender(tbc::GeometryBase* geometry, int uv_set_index) {
	(void)uv_set_index;
	OGL_ASSERT();
	const int type_index = enable_wireframe_? 1 : 0;
	OpenGLRenderer::OGLGeometryData* _geometry = (OpenGLRenderer::OGLGeometryData*)geometry->GetRendererData();
	if (uilepra::OpenGLExtensions::IsBufferObjectsSupported() == true) {
		GLuint vertex_buffer_id = (GLuint)_geometry->vertex_buffer_id_;
		GLuint index_buffer_id  = (GLuint)_geometry->index_buffer_id_;

		uilepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);

		glVertexPointer(3, GL_FLOAT, 0, (GLvoid*)_geometry->vertex_offset_);
		glNormalPointer(GL_FLOAT, 0, (GLvoid*)_geometry->normal_offset_);

		int size = 4;
		if (geometry->GetColorFormat() == tbc::GeometryBase::kColorRgb)
			size = 3;

		glColorPointer(size, GL_UNSIGNED_BYTE, 0, (GLvoid*)_geometry->color_offset_);

		uilepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_id);
		glDrawElements(OpenGLMaterial::GetGLElementType(geometry),
			       _geometry->index_count_[type_index],
			       LEPRA_GL_INDEX_TYPE,
			       (GLvoid*)_geometry->index_offset_[type_index]);
	} else {
		glVertexPointer(3, GL_FLOAT, 0, geometry->GetVertexData());
		glNormalPointer(GL_FLOAT, 0, geometry->GetNormalData());

		int size = 4;
		if (geometry->GetColorFormat() == tbc::GeometryBase::kColorRgb)
			size = 3;

		glColorPointer(size, GL_UNSIGNED_BYTE, 0, geometry->GetColorData());

		glDrawElements(OpenGLMaterial::GetGLElementType(geometry),
			       _geometry->index_count_[type_index],
			       LEPRA_GL_INDEX_TYPE,
			       (char*)geometry->GetIndexData() + _geometry->index_offset_[type_index]);
	}
	OGL_ASSERT();
}

void OpenGLMatVertexColorBlended::RenderAllGeometry(unsigned current_frame, const GeometryGroupList& geometry_group_list) {
	RenderAllBlendedGeometry(current_frame, geometry_group_list);
}

void OpenGLMatVertexColorBlended::PreRender() {
	Parent::PreRender();
	::glEnable(GL_BLEND);
}



bool OpenGLMatSingleTextureSolid::AddGeometry(tbc::GeometryBase* geometry) {
	if (geometry->GetUVSetCount() == 0) {
		if (fall_back_material_) {
			log_.Warning("Material \"SingleTextureSolid\", passing geometry to fallback material.");
			return fall_back_material_->AddGeometry(geometry);
		}
		return false;
	}
	return Parent::AddGeometry(geometry);
}

void OpenGLMatSingleTextureSolid::DoRawRender(tbc::GeometryBase* geometry, int uv_set_index) {
	if (!geometry->GetPreRenderCallback().empty()) {
		if (!geometry->GetPreRenderCallback()()) {
			return;
		}
	}

	int uv_count_per_vertex = geometry->GetUVCountPerVertex();
	const int type_index = enable_wireframe_? 1 : 0;
	OpenGLRenderer::OGLGeometryData* _geometry = (OpenGLRenderer::OGLGeometryData*)geometry->GetRendererData();
	if (uilepra::OpenGLExtensions::IsBufferObjectsSupported() == true) {
		GLuint vertex_buffer_id = (GLuint)_geometry->vertex_buffer_id_;
		GLuint index_buffer_id  = (GLuint)_geometry->index_buffer_id_;

		uilepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);

		glVertexPointer(3, GL_FLOAT, 0, (GLvoid*)_geometry->vertex_offset_);
		glNormalPointer(GL_FLOAT, 0, (GLvoid*)_geometry->normal_offset_);
		glTexCoordPointer(uv_count_per_vertex, GL_FLOAT, 0, (GLvoid*)(_geometry->uv_offset_ + sizeof(float)*uv_count_per_vertex*uv_set_index*geometry->GetMaxVertexCount()));

		uilepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_id);

		glDrawElements(OpenGLMaterial::GetGLElementType(geometry),
			       _geometry->index_count_[type_index],
			       LEPRA_GL_INDEX_TYPE,
			       (GLvoid*)_geometry->index_offset_[type_index]);
	} else {
		glVertexPointer(3, GL_FLOAT, 0, geometry->GetVertexData());
		glNormalPointer(GL_FLOAT, 0, geometry->GetNormalData());
		glTexCoordPointer(uv_count_per_vertex, GL_FLOAT, 0, geometry->GetUVData(uv_set_index));
		glDrawElements(OpenGLMaterial::GetGLElementType(geometry),
				_geometry->index_count_[type_index],
				LEPRA_GL_INDEX_TYPE,
				(char*)geometry->GetIndexData() + _geometry->index_offset_[type_index]);
	}
	OGL_ASSERT();

	if (!geometry->GetPostRenderCallback().empty()) {
		geometry->GetPostRenderCallback()();
	}
}

void OpenGLMatSingleTextureSolid::RenderGeometry(tbc::GeometryBase* geometry) {
	SetBasicMaterial(geometry->GetBasicMaterialSettings());

	OpenGLRenderer::OGLGeometryData* _geometry = (OpenGLRenderer::OGLGeometryData*)geometry->GetRendererData();
	BindTexture(_geometry->ta_->maps_[0].map_id_[Texture::kColorMap], _geometry->ta_->maps_[0].mip_map_level_count_[Texture::kColorMap]);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	OpenGLMaterial::UpdateTextureMatrix(_geometry->geometry_);

	RawRender(geometry, 0);

	ResetBasicMaterial(geometry->GetBasicMaterialSettings());
}

void OpenGLMatSingleTextureSolid::RawRender(tbc::GeometryBase* geometry, int uv_set_index) {
	DoRawRender(geometry, uv_set_index);
}

void OpenGLMatSingleTextureSolid::PreRender() {
	::glDisable(GL_ALPHA_TEST);
	::glDisable(GL_BLEND);
	//::glDisable(GL_LOGIC_OP);
	EnableDisableTexturing();

	::glEnableClientState(GL_VERTEX_ARRAY);
	::glEnableClientState(GL_NORMAL_ARRAY);
	::glDisableClientState(GL_COLOR_ARRAY);
	::glEnable(GL_COLOR_MATERIAL);
#ifndef LEPRA_GL_ES
	::glColorMaterial(GL_FRONT, GL_AMBIENT);
	::glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
#endif // !GLES
}

void OpenGLMatSingleTextureSolid::PostRender() {
	::glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	::glDisableClientState(GL_NORMAL_ARRAY);
	//::glDisableClientState(GL_VERTEX_ARRAY);
	::glDisable(GL_TEXTURE_2D);
}

void OpenGLMatSingleTextureSolid::BindTexture(int texture_id, int mip_map_level_count) {
	glBindTexture(GL_TEXTURE_2D, texture_id);
	OGL_ASSERT();

	GLint texture_param_min = GL_NEAREST;
	GLint texture_param_mag = GL_NEAREST;

	if(((OpenGLRenderer*)GetRenderer())->GetBilinearFilteringEnabled() == true) {
		if (((OpenGLRenderer*)GetRenderer())->GetMipMappingEnabled() == true)
			texture_param_min = GL_LINEAR_MIPMAP_NEAREST;
		else
			texture_param_min = GL_LINEAR;

		texture_param_mag = GL_LINEAR;
	} else if (mip_map_level_count <= 1 || !((OpenGLRenderer*)GetRenderer())->GetMipMappingEnabled()) {
		// Just use plain vanilla.
	} else if (((OpenGLRenderer*)GetRenderer())->GetTrilinearFilteringEnabled() == true) {
		// The trilinear setting overrides the other ones.
		texture_param_min = GL_LINEAR_MIPMAP_LINEAR;
		texture_param_mag = GL_LINEAR;
	} else if(((OpenGLRenderer*)GetRenderer())->GetMipMappingEnabled() == true) {
		texture_param_min = GL_NEAREST_MIPMAP_NEAREST;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture_param_min);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture_param_mag);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	OGL_ASSERT();
}



void OpenGLMatSingleTextureHighlight::PreRender() {
	Parent::PreRender();
	if (GetRenderer()->GetLightsEnabled()) {
		::glDisable(GL_LIGHTING);
	}
}

void OpenGLMatSingleTextureHighlight::PostRender() {
	if (GetRenderer()->GetLightsEnabled()) {
		::glEnable(GL_LIGHTING);
	}
	Parent::PostRender();
}



void OpenGLMatSingleTextureBlended::RenderAllGeometry(unsigned current_frame, const GeometryGroupList& geometry_group_list) {
	RenderAllBlendedGeometry(current_frame, geometry_group_list);
}

void OpenGLMatSingleTextureBlended::PreRender() {
	Parent::PreRender();
	::glEnable(GL_BLEND);
}

void OpenGLMatSingleTextureBlended::PostRender() {
	::glDisable(GL_BLEND);
	Parent::PostRender();
}



void OpenGLMatSingleTextureAlphaTested::PreRender() {
	Parent::PreRender();
	glEnable(GL_ALPHA_TEST);
}

void OpenGLMatSingleTextureAlphaTested::PostRender() {
	Parent::PostRender();
	glDisable(GL_ALPHA_TEST);
}

void OpenGLMatSingleTextureAlphaTested::RenderGeometry(tbc::GeometryBase* geometry) {
	const tbc::GeometryBase::BasicMaterialSettings& mat_settings =
		geometry->GetBasicMaterialSettings();
	glAlphaFunc(GL_GEQUAL, mat_settings.alpha_);

	Parent::RenderGeometry(geometry);
}



bool OpenGLMatSingleColorEnvMapSolid::AddGeometry(tbc::GeometryBase* geometry) {
	/*if (geometry->GetUVSetCount() == 0) {
		if (fall_back_material_) {
			log_.Warning("Material \"SingleColorEnvMapSolid\", passing geometry to fallback material.");
			return fall_back_material_->AddGeometry(geometry);
		}
		return false;
	}*/
	return OpenGLMatSingleColorSolid::AddGeometry(geometry);
}

void OpenGLMatSingleColorEnvMapSolid::DoRenderAllGeometry(unsigned current_frame, const GeometryGroupList& geometry_group_list) {
	if (!GetRenderer()->GetEnvTexture()) {
		fall_back_material_->DoRenderAllGeometry(current_frame, geometry_group_list);
		return;
	}

	// This material requires two rendering passes, since OpenGL doesn't
	// support blending the texture with the base color of the material.
	// So, first we render a "single color material" pass,
	// and then we blend in the environment map on top of that.
#ifndef LEPRA_GL_ES
	::glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
#endif // !GLES

	// Pass 1, single color.
	single_color_pass_ = true;
	::glDisable(GL_TEXTURE_2D);
	OpenGLMatSingleColorSolid::DoRenderAllGeometry(current_frame, geometry_group_list);
	::glEnable(GL_TEXTURE_2D);

	// Pass 2, Render the enviroment map.
	single_color_pass_ = false;
	//::glDepthFunc(GL_LEQUAL);
	//::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	::glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	BindTexture(GetRenderer()->GetEnvTexture()->t_map_id_[Texture::kColorMap], GetRenderer()->GetEnvTexture()->t_mip_map_level_count_[Texture::kColorMap]);

/*#ifndef LEPRA_GL_ES
	if (((OpenGLRenderer*)GetRenderer())->IsEnvMapCubeMap() == true) {
		// Use cube mapping.
		::glDisable(GL_TEXTURE_2D);
		::glEnable(GL_TEXTURE_CUBE_MAP);
		::glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
		::glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
		::glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
		::glEnable(GL_TEXTURE_GEN_S);
		::glEnable(GL_TEXTURE_GEN_T);
		::glEnable(GL_TEXTURE_GEN_R);

		int lEnvMapID = ((OpenGLRenderer*)GetRenderer())->GetEnvMapID();
		::glBindTexture(GL_TEXTURE_CUBE_MAP, lEnvMapID);

		::glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);
	} else {
		// Use sphere mapping.
		::glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
		::glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
		::glEnable(GL_TEXTURE_GEN_S);
		::glEnable(GL_TEXTURE_GEN_T);
	}
#endif // !GLES*/
	/*::glMatrixMode(GL_TEXTURE);
	GLfloat m[4][4] =
	{
		{ 1, 0, 0, 0 },
		{ 0, 1, 0, 0 },
		{ 0, 0, 1, 0 },
		{ 0, 0, 0, 1 },
	};
	::glLoadMatrixf((GLfloat*)m);
	::glMatrixMode(GL_MODELVIEW);*/

	((OpenGLRenderer*)GetRenderer())->AddAmbience(1.0f, 1.0f, 1.0f);

	Parent::DoRenderAllGeometry(current_frame, geometry_group_list);

	((OpenGLRenderer*)GetRenderer())->ResetAmbientLight(true);

/*#ifndef LEPRA_GL_ES
	if (((OpenGLRenderer*)GetRenderer())->IsEnvMapCubeMap() == true) {
		::glDisable(GL_TEXTURE_GEN_S);
		::glDisable(GL_TEXTURE_GEN_T);
		::glDisable(GL_TEXTURE_GEN_R);
		::glDisable(GL_TEXTURE_CUBE_MAP);
	} else {
		::glDisable(GL_TEXTURE_GEN_S);
		::glDisable(GL_TEXTURE_GEN_T);
	}
#endif // !GLES*/

	::glMatrixMode(GL_TEXTURE);
	::glLoadIdentity();
	::glMatrixMode(GL_MODELVIEW);
}

void OpenGLMatSingleColorEnvMapSolid::PreRender() {
	if (single_color_pass_) {
		OpenGLMatSingleColorSolid::PreRender();
	} else {
		Parent::PreRender();
		::glEnable(GL_BLEND);
		::glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		::glColor4f(1, 1, 1, 1);
		const float c[] = {1,1,1,1};
		::glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, c);
		::glEnable(GL_TEXTURE_2D);
		::glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		::glDepthFunc(GL_LEQUAL);
	}
}

void OpenGLMatSingleColorEnvMapSolid::PostRender() {
	if (single_color_pass_) {
		OpenGLMatSingleColorSolid::PostRender();
	} else {
		Parent::PostRender();
		::glDisable(GL_BLEND);
		::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		::glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		::glDepthFunc(GL_LESS);
	}
}

void OpenGLMatSingleColorEnvMapSolid::RenderGeometry(tbc::GeometryBase* geometry) {
	if (single_color_pass_) {
		SetBasicMaterial(geometry->GetBasicMaterialSettings());
		OpenGLMatSingleColorSolid::RawRender(geometry, 0);
		ResetBasicMaterial(geometry->GetBasicMaterialSettings());
	} else {
		::glMatrixMode(GL_TEXTURE);
		float lTextureMatrix[16];
		xform object_transform = geometry->GetTransformation();
		object_transform.RotateWorldX(PIF/2);
		(object_transform.Inverse() * GetRenderer()->GetCameraActualTransformation()).GetAs4x4OrientationMatrix(lTextureMatrix);
		lTextureMatrix[15] *= 3.0f;
		::glLoadMatrixf(lTextureMatrix);

		const int type_index = enable_wireframe_? 1 : 0;
		OpenGLRenderer::OGLGeometryData* _geometry = (OpenGLRenderer::OGLGeometryData*)geometry->GetRendererData();
		if (uilepra::OpenGLExtensions::IsBufferObjectsSupported() == true) {
			GLuint vertex_buffer_id = (GLuint)_geometry->vertex_buffer_id_;
			GLuint index_buffer_id  = (GLuint)_geometry->index_buffer_id_;

			uilepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);

			glVertexPointer(3, GL_FLOAT, 0, (GLvoid*)_geometry->vertex_offset_);
			glNormalPointer(GL_FLOAT, 0, (GLvoid*)_geometry->normal_offset_);
			glTexCoordPointer(3, GL_FLOAT, 0, (GLvoid*)_geometry->normal_offset_);	// Use vertex coordinates instead.

			uilepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_id);

			glDrawElements(OpenGLMaterial::GetGLElementType(geometry),
				       _geometry->index_count_[type_index],
				       LEPRA_GL_INDEX_TYPE,
				       (GLvoid*)_geometry->index_offset_[type_index]);
		} else {
			glVertexPointer(3, GL_FLOAT, 0, geometry->GetVertexData());
			glNormalPointer(GL_FLOAT, 0, geometry->GetNormalData());
			glTexCoordPointer(3, GL_FLOAT, 0, geometry->GetNormalData());	// Use vertex coordinates instead.
			glDrawElements(OpenGLMaterial::GetGLElementType(geometry),
				       _geometry->index_count_[type_index],
				       LEPRA_GL_INDEX_TYPE,
				       (char*)geometry->GetIndexData() + _geometry->index_offset_[type_index]);
		}
		OGL_ASSERT();
	}
}



void OpenGLMatSingleColorEnvMapBlended::RenderAllGeometry(unsigned current_frame, const GeometryGroupList& geometry_group_list) {
	RenderAllBlendedGeometry(current_frame, geometry_group_list);
}

void OpenGLMatSingleColorEnvMapBlended::PreRender() {
	if (single_color_pass_) {
		OpenGLMatSingleColorBlended::DoPreRender();
	} else {
		Parent::PreRender();
	}
}

void OpenGLMatSingleColorEnvMapBlended::PostRender() {
	if (single_color_pass_) {
		OpenGLMatSingleColorBlended::DoPostRender();
	} else {
		::glDisable(GL_BLEND);
	}
}



void OpenGLMatSingleTextureEnvMapSolid::DoRenderAllGeometry(unsigned current_frame, const GeometryGroupList& geometry_group_list) {
	if (!uilepra::OpenGLExtensions::IsMultiTextureSupported() || !GetRenderer()->GetEnvTexture()) {
		fall_back_material_->DoRenderAllGeometry(current_frame, geometry_group_list);
		return;
	}

	// This material requires two rendering passes, since OpenGL doesn't
	// support blending the two textures the right way - not even with
	// multi texture support. But we still need multi texture support though.
	// So, first we render a normal "single texture" pass,
	// and then we blend in the environment map on top of that.
#ifndef LEPRA_GL_ES
	::glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
#endif // !GLES
	glDisableClientState(GL_COLOR_ARRAY);

	// Pass 1, single texture.
	single_texture_pass_ = true;
	EnableDisableTexturing();
	Parent::DoRenderAllGeometry(current_frame, geometry_group_list);

	// Early bail out if no multitexturing support. TODO: fix in cards that don't support!
	if (!uilepra::OpenGLExtensions::IsMultiTextureSupported()) {
		return;
	}

	// Pass 2, Render the enviroment map.
	single_texture_pass_ = false;
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	uilepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE1);
	uilepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE1);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	BindTexture(GetRenderer()->GetEnvTexture()->t_map_id_[Texture::kColorMap], GetRenderer()->GetEnvTexture()->t_mip_map_level_count_[Texture::kColorMap]);

#ifndef LEPRA_GL_ES
	if (((OpenGLRenderer*)GetRenderer())->IsEnvMapCubeMap() == true) {
		// Use cube mapping.
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_TEXTURE_CUBE_MAP);
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
		glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		glEnable(GL_TEXTURE_GEN_R);

		int lEnvMapID = ((OpenGLRenderer*)GetRenderer())->GetEnvMapID();
		glBindTexture(GL_TEXTURE_CUBE_MAP, lEnvMapID);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);
	} else {
		// Use sphere mapping.
		glEnable(GL_TEXTURE_2D);
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
	}
#endif // !GLES

	glMatrixMode(GL_TEXTURE);
	float lTextureMatrix[16];
	GetRenderer()->GetCameraActualTransformation().GetAs4x4OrientationMatrix(lTextureMatrix);

	glLoadMatrixf(lTextureMatrix);
	glMatrixMode(GL_MODELVIEW);

	float ambient_red;
	float ambient_green;
	float ambient_blue;
	((OpenGLRenderer*)GetRenderer())->GetAmbientLight(ambient_red, ambient_green, ambient_blue);
	((OpenGLRenderer*)GetRenderer())->SetAmbientLight(1.0f, 1.0f, 1.0f);

	uilepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE0);
	uilepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE0);

	GLboolean lBlendEnabled = glIsEnabled(GL_BLEND);

	glEnable(GL_BLEND);

	//
	// Render the geometry.
	//
	Parent::DoRenderAllGeometry(current_frame, geometry_group_list);

	if (!lBlendEnabled)
		glDisable(GL_BLEND);

	((OpenGLRenderer*)GetRenderer())->SetAmbientLight(ambient_red, ambient_green, ambient_blue);

	uilepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE1);
	uilepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE1);
#ifndef LEPRA_GL_ES
	if (((OpenGLRenderer*)GetRenderer())->IsEnvMapCubeMap() == true) {
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_TEXTURE_GEN_R);
		glDisable(GL_TEXTURE_CUBE_MAP);
	} else {
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_TEXTURE_2D);
	}
#endif // !GLES

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);

	uilepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE0);
	uilepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void OpenGLMatSingleTextureEnvMapSolid::RenderGeometry(tbc::GeometryBase* geometry) {
	if (!uilepra::OpenGLExtensions::IsMultiTextureSupported()) {
		Parent::RenderGeometry(geometry);
		return;
	}

	SetBasicMaterial(geometry->GetBasicMaterialSettings());

	OpenGLRenderer::OGLGeometryData* _geometry = (OpenGLRenderer::OGLGeometryData*)geometry->GetRendererData();
	glBindTexture(GL_TEXTURE_2D, _geometry->ta_->maps_[0].map_id_[Texture::kColorMap]);
	OpenGLMaterial::UpdateTextureMatrix(_geometry->geometry_);

	RawRender(geometry, 0);

	ResetBasicMaterial(geometry->GetBasicMaterialSettings());
}

void OpenGLMatSingleTextureEnvMapSolid::RawRender(tbc::GeometryBase* geometry, int uv_set_index) {
	int uv_count_per_vertex = geometry->GetUVCountPerVertex();
	const int type_index = enable_wireframe_? 1 : 0;
	OpenGLRenderer::OGLGeometryData* _geometry = (OpenGLRenderer::OGLGeometryData*)geometry->GetRendererData();
	if (uilepra::OpenGLExtensions::IsBufferObjectsSupported() == true) {
		GLuint vertex_buffer_id = (GLuint)_geometry->vertex_buffer_id_;
		GLuint index_buffer_id  = (GLuint)_geometry->index_buffer_id_;

		uilepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);

		glVertexPointer(3, GL_FLOAT, 0, (GLvoid*)_geometry->vertex_offset_);
		glNormalPointer(GL_FLOAT, 0, (GLvoid*)_geometry->normal_offset_);
		glTexCoordPointer(uv_count_per_vertex, GL_FLOAT, 0, (GLvoid*)(_geometry->uv_offset_ + sizeof(float)*uv_count_per_vertex*uv_set_index*geometry->GetMaxVertexCount()));

		uilepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_id);
		glDrawElements(OpenGLMaterial::GetGLElementType(geometry),
			       _geometry->index_count_[type_index],
			       LEPRA_GL_INDEX_TYPE,
			       (GLvoid*)_geometry->index_offset_[type_index]);
	} else {
		glVertexPointer(3, GL_FLOAT, 0, geometry->GetVertexData());
		glNormalPointer(GL_FLOAT, 0, geometry->GetNormalData());
		glTexCoordPointer(uv_count_per_vertex, GL_FLOAT, 0, geometry->GetUVData(uv_set_index));
		glDrawElements(OpenGLMaterial::GetGLElementType(geometry),
			       _geometry->index_count_[type_index],
			       LEPRA_GL_INDEX_TYPE,
			       (char*)geometry->GetIndexData() + _geometry->index_offset_[type_index]);
	}
}

void OpenGLMatSingleTextureEnvMapBlended::RenderAllGeometry(unsigned current_frame, const GeometryGroupList& geometry_group_list) {
	RenderAllBlendedGeometry(current_frame, geometry_group_list);
}



void OpenGLMatTextureAndLightmap::DoRenderAllGeometry(unsigned current_frame, const GeometryGroupList& geometry_group_list) {
	// Early bail out if no multitexturing support. TODO: fix in cards that don't support!
	if (!uilepra::OpenGLExtensions::IsMultiTextureSupported()) {
		Parent::DoRenderAllGeometry(current_frame, geometry_group_list);
		return;
	}

	// The problem here is to render a surface with dynamic lights in the scene
	// combined with a light map. The formula we want is cout = c(L + lm), where
	// cout is the output color, c is the color map's color combined with the
	// surface color, L is the sum of all dynamic lights, and lm is the color of
	// the light map.
	//
	// The following solution is very easy to implement but doesn't give us the
	// dynamic lighting:
	//
	// 1. Modulate the color map with the surface color. (cout = c).
	// 2. Modulate the light map on top of the previous result. (cout = c * lm).
	//
	// If an area is pitch black, it will still be pitch black even if you add a
	// one billion megawatt search light to the scene.
	//
	// The correct way of doing this can only be done in two rendering passes:
	//
	// 1. Render the color map modulated with the light map (requires 2 texture
	//    units). Disable all lights and set ambience to (1, 1, 1). (cout = c * lm).
	// 2. Render the color map with dynamic lights and ADD it to the result from pass 1.
	//    (cout = c * lm + c * L = c(L + lm).

#ifndef LEPRA_GL_ES
	::glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
#endif // !GLES

	//
	// Pass 1.
	//

	first_pass_ = true;

	uilepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE0);
	uilepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	uilepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE1);
	uilepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	float ambient_red;
	float ambient_green;
	float ambient_blue;
	((OpenGLRenderer*)GetRenderer())->GetAmbientLight(ambient_red, ambient_green, ambient_blue);
	((OpenGLRenderer*)GetRenderer())->SetAmbientLight(1.0f, 1.0f, 1.0f);

	glDisable(GL_LIGHTING);

	Parent::DoRenderAllGeometry(current_frame, geometry_group_list);

	if (GetRenderer()->GetLightsEnabled())
		glEnable(GL_LIGHTING);
	((OpenGLRenderer*)GetRenderer())->SetAmbientLight(ambient_red, ambient_green, ambient_blue);

	uilepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE1);
	uilepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_2D);

	//
	// Pass 2.
	//
	first_pass_ = false;

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	Parent::DoRenderAllGeometry(current_frame, geometry_group_list);

	uilepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE1);
	uilepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glDisable(GL_BLEND);

	uilepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE0);
	uilepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE0);
}

void OpenGLMatTextureAndLightmap::RenderGeometry(tbc::GeometryBase* geometry) {
	if (!uilepra::OpenGLExtensions::IsMultiTextureSupported() ||
		geometry->GetUVSetCount() < 2) {
		Parent::RenderGeometry(geometry);
		return;
	}

	SetBasicMaterial(geometry->GetBasicMaterialSettings());

	RawRender(geometry, 0);

	ResetBasicMaterial(geometry->GetBasicMaterialSettings());
}

void OpenGLMatTextureAndLightmap::RawRender(tbc::GeometryBase* geometry, int uv_set_index) {
	OpenGLRenderer::OGLGeometryData* _geometry = (OpenGLRenderer::OGLGeometryData*)geometry->GetRendererData();

	// Setup the color map in texture unit 0.
	uilepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE0);
	uilepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE0);
	BindTexture(_geometry->ta_->maps_[0].map_id_[Texture::kColorMap], _geometry->ta_->maps_[0].mip_map_level_count_[Texture::kColorMap]);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	int uv_count_per_vertex = geometry->GetUVCountPerVertex();
	const int type_index = enable_wireframe_? 1 : 0;
	if (uilepra::OpenGLExtensions::IsBufferObjectsSupported() == true) {
		uilepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, (GLuint)_geometry->vertex_buffer_id_);
		glTexCoordPointer(uv_count_per_vertex, GL_FLOAT, 0, (GLvoid*)(_geometry->uv_offset_ + sizeof(float)*uv_count_per_vertex*uv_set_index*geometry->GetMaxVertexCount()));
	} else {
		glTexCoordPointer(uv_count_per_vertex, GL_FLOAT, 0, geometry->GetUVData(uv_set_index));
	}

	OpenGLMaterial::UpdateTextureMatrix(_geometry->geometry_);

	if (first_pass_ == true) {
		//
		// Render pass 1 - color map modulated with light map.
		//

		// Setup the light map in texture unit 1.
		uilepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE1);
		uilepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE1);
		BindTexture(_geometry->ta_->maps_[1].map_id_[Texture::kColorMap], _geometry->ta_->maps_[1].mip_map_level_count_[Texture::kColorMap]);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		uv_count_per_vertex = geometry->GetUVCountPerVertex();
		if (uilepra::OpenGLExtensions::IsBufferObjectsSupported() == true) {
			uilepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, (GLuint)_geometry->vertex_buffer_id_);
			glTexCoordPointer(uv_count_per_vertex, GL_FLOAT, 0, (GLvoid*)(_geometry->uv_offset_ + sizeof(float)*uv_count_per_vertex*(uv_set_index+1)*geometry->GetMaxVertexCount()));
		} else {
			glTexCoordPointer(uv_count_per_vertex, GL_FLOAT, 0, geometry->GetUVData(uv_set_index+1));
		}
	}

	if (uilepra::OpenGLExtensions::IsBufferObjectsSupported() == true) {
		GLuint index_buffer_id  = (GLuint)_geometry->index_buffer_id_;

		// Vertex buffer already bound.
		//OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, (GLuint)_geometry->vertex_buffer_id_);

		glVertexPointer(3, GL_FLOAT, 0, (GLvoid*)_geometry->vertex_offset_);
		glNormalPointer(GL_FLOAT, 0, (GLvoid*)_geometry->normal_offset_);

		uilepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_id);
		glDrawElements(OpenGLMaterial::GetGLElementType(geometry),
			       _geometry->index_count_[type_index],
			       LEPRA_GL_INDEX_TYPE,
			       (GLvoid*)_geometry->index_offset_[type_index]);
	} else {
		glVertexPointer(3, GL_FLOAT, 0, geometry->GetVertexData());
		glNormalPointer(GL_FLOAT, 0, geometry->GetNormalData());
		glDrawElements(OpenGLMaterial::GetGLElementType(geometry),
			       _geometry->index_count_[type_index],
			       LEPRA_GL_INDEX_TYPE,
			       (char*)geometry->GetIndexData() + _geometry->index_offset_[type_index]);
	}
}

//
// OpenGLMatPXS
//

int  OpenGLMatPXS::program_count_ = 0;
bool OpenGLMatPXS::fplut_initialized_ = false;
int  OpenGLMatPXS::fplut_[4][4][4];

float OpenGLMatPXS::light_pos_[kMaxShaderLights * 4];
float OpenGLMatPXS::light_dir_[kMaxShaderLights * 4];
float OpenGLMatPXS::light_col_[kMaxShaderLights * 4];
float OpenGLMatPXS::light_cut_[kMaxShaderLights];
float OpenGLMatPXS::light_exp_[kMaxShaderLights];

Renderer::LightType OpenGLMatPXS::light_type_[kMaxShaderLights];

int OpenGLMatPXS::num_dir_lights_ = 0;
int OpenGLMatPXS::num_pnt_lights_ = 0;
int OpenGLMatPXS::num_spt_lights_ = 0;
int OpenGLMatPXS::light_count_ = 0;


OpenGLMatPXS::OpenGLMatPXS(const str& vp, const str fp[kNumFp]):
	vpid_(0) {
#ifndef LEPRA_GL_ES
	::memset(fpid_, 0, sizeof(fpid_));

	if (fplut_initialized_ == false) {
		//
		// Precalculate the fragment program lookup table.
		//

		int p, s; // p for point lights, and s for spot lights.

		fplut_[0][0][0] = kFpNone;
		fplut_[0][0][1] = kFp1Spot;
		fplut_[0][0][2] = kFp2Spot;
		fplut_[0][0][3] = kFp3Spot;

		fplut_[0][1][0] = kFp1Point;
		fplut_[0][1][1] = kFp1Point1Spot;
		fplut_[0][1][2] = kFp1Point2Spot;
		fplut_[0][1][3] = kFp1Point2Spot;

		fplut_[0][2][0] = kFp2Point;
		fplut_[0][2][1] = kFp2Point1Spot;
		fplut_[0][2][2] = kFp2Point1Spot;
		fplut_[0][2][3] = kFp2Point1Spot;

		fplut_[0][3][0] = kFp3Point;
		fplut_[0][3][1] = kFp3Point;
		fplut_[0][3][2] = kFp3Point;
		fplut_[0][3][3] = kFp3Point;



		fplut_[1][0][0] = kFp1Dir;
		fplut_[1][0][1] = kFp1Dir1Spot;
		fplut_[1][0][2] = kFp1Dir2Spot;
		fplut_[1][0][3] = kFp1Dir2Spot;

		fplut_[1][1][0] = kFp1Dir1Point;
		fplut_[1][1][1] = kFp1Dir1Point1Spot;
		fplut_[1][1][2] = kFp1Dir1Point1Spot;
		fplut_[1][1][3] = kFp1Dir1Point1Spot;

		for (p = 2; p < 4; p++)
			for (s = 0; s < 4; s++)
				fplut_[1][p][s] = kFp1Dir2Point;



		fplut_[2][0][0] = kFp2Dir;
		fplut_[2][0][1] = kFp2Dir1Spot;
		fplut_[2][0][2] = kFp2Dir1Spot;
		fplut_[2][0][3] = kFp2Dir1Spot;

		for (p = 1; p < 4; p++)
			for (s = 0; s < 4; s++)
				fplut_[2][p][s] = kFp2Dir1Point;



		for (p = 0; p < 4; p++)
			for (s = 0; s < 4; s++)
				fplut_[3][p][s] = kFp3Dir;

		fplut_initialized_ = true;
	}

	if (uilepra::OpenGLExtensions::IsShaderAsmProgramsSupported()) {
		int i;

		//
		// Initialize the vertex shader.
		//

		glEnable(GL_VERTEX_PROGRAM_ARB);
		// Allocate a new ID for this vertex program.
		vpid_ = AllocProgramID();

		uilepra::OpenGLExtensions::glBindProgramARB(GL_VERTEX_PROGRAM_ARB, vpid_);

		GLint lErrorPos;
		uilepra::OpenGLExtensions::glProgramStringARB(GL_VERTEX_PROGRAM_ARB,
							    GL_PROGRAM_FORMAT_ASCII_ARB,
							    (GLsizei)vp.length(),
							    vp.c_str());
		glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB, &lErrorPos);
		if (lErrorPos != -1) {
			str gl_error((const char*)glGetString(GL_PROGRAM_ERROR_STRING_ARB));
			log_.Errorf("Error in vertex shader at pos %i!\r\n%s\r\n", lErrorPos, gl_error.c_str());
		}
		glDisable(GL_VERTEX_PROGRAM_ARB);

		//
		// Initialize the fragment shaders.
		//

		glEnable(GL_FRAGMENT_PROGRAM_ARB);
		for (i = 0; i < kNumFp; i++) {
			// Allocate a new ID for this fragment program.
			fpid_[i] = AllocProgramID();

			uilepra::OpenGLExtensions::glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, fpid_[i]);

			lErrorPos;
			uilepra::OpenGLExtensions::glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB,
								    GL_PROGRAM_FORMAT_ASCII_ARB,
								    (GLsizei)fp[i].length(),
								    fp[i].c_str());
			glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB, &lErrorPos);
			if (lErrorPos != -1) {
				str gl_error((const char*)glGetString(GL_PROGRAM_ERROR_STRING_ARB));
				log_.Errorf("Error in fragment shader %i at pos %i!\r\n%s\r\n", i, lErrorPos, gl_error.c_str());
				log_.Info("Setting fragment shader to fallback shader.");

				SetToFallbackFP(i);
			}
		}
		glDisable(GL_FRAGMENT_PROGRAM_ARB);
	}
#endif // !GLES
}

void OpenGLMatPXS::SetToFallbackFP(int fp_type) {
#ifndef LEPRA_GL_ES
	int& lDestFP = fpid_[fp_type];

	switch(fp_type) {
	case kFpNone:
		lDestFP = fpid_[kFpNone];	// There is no fallback on this one.
		break;
	case kFp1Point:
		lDestFP = fpid_[kFpNone];
		break;
	case kFp2Point:
		lDestFP = fpid_[kFp1Point];
		break;
	case kFp3Point:
		lDestFP = fpid_[kFp2Point];
		break;
	case kFp1Dir:
		lDestFP = fpid_[kFpNone];
		break;
	case kFp2Dir:
		lDestFP = fpid_[kFp1Dir];
		break;
	case kFp3Dir:
		lDestFP = fpid_[kFp2Dir];
		break;
	case kFp1Spot:
		lDestFP = fpid_[kFpNone];
		break;
	case kFp2Spot:
		lDestFP = fpid_[kFp1Spot];
		break;
	case kFp3Spot:
		lDestFP = fpid_[kFp2Spot];
		break;
	case kFp1Dir1Point:
		lDestFP = fpid_[kFp1Dir];
		break;
	case kFp1Dir2Point:
		lDestFP = fpid_[kFp1Dir1Point];
		break;
	case kFp2Dir1Point:
		lDestFP = fpid_[kFp2Dir];
		break;
	case kFp1Dir1Spot:
		lDestFP = fpid_[kFp1Dir];
		break;
	case kFp1Dir2Spot:
		lDestFP = fpid_[kFp1Dir1Spot];
		break;
	case kFp2Dir1Spot:
		lDestFP = fpid_[kFp2Dir];
		break;
	case kFp1Point1Spot:
		lDestFP = fpid_[kFp1Point];
		break;
	case kFp1Point2Spot:
		lDestFP = fpid_[kFp1Point1Spot];
		break;
	case kFp2Point1Spot:
		lDestFP = fpid_[kFp2Point];
		break;
	case kFp1Dir1Point1Spot:
		lDestFP = fpid_[kFp1Dir1Point];
		break;
	};
#endif // !GLES
}

void OpenGLMatPXS::PrepareLights(OpenGLRenderer* renderer) {
#ifndef LEPRA_GL_ES
	//
	// All pixel shaded materials support up to 3 simultaneous lights.
	// The fragment shaders need the lights sorted in the following order:
	// 1. Directional lights.
	// 2. Point lights.
	// 3. Spot lights.
	//
	enum {
		NUM_BUCKETS = 3
	};

	// One bucket per light type.
	float light_pos[NUM_BUCKETS * kMaxShaderLights * 4];
	float light_dir[NUM_BUCKETS * kMaxShaderLights * 4];
	float lLightCol[NUM_BUCKETS * kMaxShaderLights * 4];

	// Cutoff angle for each light. Assumes that kMaxShaderLights <= 4.
	float lLightCut[NUM_BUCKETS * kMaxShaderLights];
	float lLightExp[NUM_BUCKETS * kMaxShaderLights];

	::memset(light_pos, 0, sizeof(light_pos));
	::memset(light_dir, 0, sizeof(light_dir));
	::memset(lLightCol, 0, sizeof(lLightCol));
	::memset(lLightCut, 0, sizeof(lLightCut));
	::memset(lLightExp, 0, sizeof(lLightExp));

	num_dir_lights_ = 0;
	num_pnt_lights_ = 0;
	num_spt_lights_ = 0;
	int lTotalLightCount = 0;
	int i;

	// Sorting the lights with respect to the camera.
	renderer->SortLights(renderer->GetCameraTransformation().GetPosition());

	// Count the number of lights of each type, and get their position/direction and color.
	for (i = 0; i < renderer->GetLightCount() && lTotalLightCount < kMaxShaderLights; ++i) {
		const Renderer::LightID light_id = renderer->GetClosestLight(i);
		GLenum lGLLight = GL_LIGHT0 + (int)light_id;

		int light_index = 0;

		// Check if the light is enabled, if it's not, we are in the
		// "shadow rendering pass".
		if (::glIsEnabled(lGLLight) == GL_TRUE) {
			// Get light type.
			light_type_[lTotalLightCount] = renderer->GetLightType(light_id);
			int lBucket = (int)light_type_[lTotalLightCount];
			int index = 0;

			// Get light position and direction.
			switch(light_type_[lTotalLightCount]) {
			case Renderer::kLightDirectional: {
				index = lBucket * kMaxShaderLights * 4 + num_dir_lights_ * 4;
				light_index = num_dir_lights_;

				glGetLightfv(lGLLight, GL_POSITION, &light_dir[index]);

				num_dir_lights_++;
				break;
			}
			case Renderer::kLightPoint:
				index = lBucket * kMaxShaderLights * 4 + num_pnt_lights_ * 4;
				light_index = num_pnt_lights_;

				glGetLightfv(lGLLight, GL_POSITION, &light_pos[index]);

				num_pnt_lights_++;
				break;
			case Renderer::kLightSpot: {
				index = lBucket * kMaxShaderLights * 4 + num_spt_lights_ * 4;
				light_index = num_spt_lights_;

				glGetLightfv(lGLLight, GL_POSITION, &light_pos[index]);
				glGetLightfv(lGLLight, GL_SPOT_DIRECTION, &light_dir[index]);

				num_spt_lights_++;
				break;
			}
			default: break;
			}

			// Get light color.
			const vec3 lLightColor = renderer->GetLightColor(light_id);
			lLightCol[index + 0] = lLightColor.x;
			lLightCol[index + 1] = lLightColor.y;
			lLightCol[index + 2] = lLightColor.z;

			float lCutoffAngle = renderer->GetLightCutoffAngle(light_id);
			lLightCut[lBucket * kMaxShaderLights + light_index] = (float)cos(lCutoffAngle * PIF / 180.0f);

			lLightExp[lBucket * kMaxShaderLights + light_index] = renderer->GetLightSpotExponent(light_id);

			++lTotalLightCount;
		}
	}



	// Set light position, direction and color in sorted order.
	light_count_ = 0;

	for (i = 0; i < num_dir_lights_; i++) {
		int index = (int)Renderer::kLightDirectional * kMaxShaderLights * 4 + i * 4;
		memcpy(&light_pos_[light_count_ * 4], &light_pos[index], 4 * sizeof(float));
		memcpy(&light_dir_[light_count_ * 4], &light_dir[index], 4 * sizeof(float));
		memcpy(&light_col_[light_count_ * 4], &lLightCol[index], 4 * sizeof(float));

		index = (int)Renderer::kLightDirectional * kMaxShaderLights + i;
		light_cut_[light_count_] = lLightCut[index];
		light_exp_[light_count_] = lLightExp[index];

		light_count_++;
	}

	for (i = 0; i < num_pnt_lights_; i++) {
		int index = (int)Renderer::kLightPoint * kMaxShaderLights * 4 + i * 4;
		memcpy(&light_pos_[light_count_ * 4], &light_pos[index], 4 * sizeof(float));
		memcpy(&light_dir_[light_count_ * 4], &light_dir[index], 4 * sizeof(float));
		memcpy(&light_col_[light_count_ * 4], &lLightCol[index], 4 * sizeof(float));

		index = (int)Renderer::kLightPoint * kMaxShaderLights + i;
		light_cut_[light_count_] = lLightCut[index];
		light_exp_[light_count_] = lLightExp[index];

		light_count_++;
	}

	for (i = 0; i < num_spt_lights_; i++) {
		int index = (int)Renderer::kLightSpot * kMaxShaderLights * 4 + i * 4;
		memcpy(&light_pos_[light_count_ * 4], &light_pos[index], 4 * sizeof(float));
		memcpy(&light_dir_[light_count_ * 4], &light_dir[index], 4 * sizeof(float));
		memcpy(&light_col_[light_count_ * 4], &lLightCol[index], 4 * sizeof(float));

		index = (int)Renderer::kLightSpot * kMaxShaderLights + i;
		light_cut_[light_count_] = lLightCut[index];
		light_exp_[light_count_] = lLightExp[index];

		light_count_++;
	}
#endif // !GLES
}

void OpenGLMatPXS::CleanupShaderPrograms() {
#ifndef LEPRA_GL_ES
	glDisable(GL_VERTEX_PROGRAM_ARB);
	glDisable(GL_FRAGMENT_PROGRAM_ARB);
#endif // !GLES
}

void OpenGLMatPXS::SetAmbientLight(OpenGLRenderer* renderer, tbc::GeometryBase* geometry) {
#ifndef LEPRA_GL_ES
	float r, g, b;
	renderer->GetAmbientLight(r, g, b);

	const tbc::GeometryBase::BasicMaterialSettings& mat =
		geometry->GetBasicMaterialSettings();

	r += mat.ambient_.x;
	g += mat.ambient_.y;
	b += mat.ambient_.z;

	//if (r > 1.0f)
	//{
	//	r = 1.0f;
	//}
	//if (g > 1.0f)
	//{
	//	g = 1.0f;
	//}
	//if (b > 1.0f)
	//{
	//	b = 1.0f;
	//}

	// Set the ambient light.
	GLfloat light_ambient[4];
	light_ambient[0] = (GLfloat)r;
	light_ambient[1] = (GLfloat)g;
	light_ambient[2] = (GLfloat)b;
	light_ambient[3] = 0;
	uilepra::OpenGLExtensions::glProgramLocalParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB, 3, light_ambient);
#endif // !GLES
}

void OpenGLMatPXS::PrepareShaderPrograms(OpenGLRenderer* /*renderer*/) {
#ifndef LEPRA_GL_ES
	// Lookup which fragment shader to use for this combination of lights.
	int selected_fp = fplut_[num_dir_lights_][num_pnt_lights_][num_spt_lights_];

	glEnable(GL_VERTEX_PROGRAM_ARB);
	glEnable(GL_FRAGMENT_PROGRAM_ARB);

	uilepra::OpenGLExtensions::glBindProgramARB(GL_VERTEX_PROGRAM_ARB, vpid_);
	uilepra::OpenGLExtensions::glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, fpid_[selected_fp]);

	for (int i = 0; i < light_count_; i++) {
		uilepra::OpenGLExtensions::glProgramLocalParameter4fvARB(GL_VERTEX_PROGRAM_ARB, i,  &light_pos_[i * 4]);
		uilepra::OpenGLExtensions::glProgramLocalParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB, i + 4,  &light_pos_[i * 4]);
		uilepra::OpenGLExtensions::glProgramLocalParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB, i + 7,  &light_dir_[i * 4]);
		uilepra::OpenGLExtensions::glProgramLocalParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB, i + 10, &light_col_[i * 4]);
		// One extra copy to the vertex shader. Used in bump mapping for instance.
		uilepra::OpenGLExtensions::glProgramLocalParameter4fvARB(GL_VERTEX_PROGRAM_ARB, i + 3,  &light_dir_[i * 4]);
	}

	// Set the cutoff angle and spot exponent.
	uilepra::OpenGLExtensions::glProgramLocalParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB, 1, light_cut_);
	uilepra::OpenGLExtensions::glProgramLocalParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB, 2, light_exp_);
#endif // !GLES
}

//
// OpenGLMatSingleColorSolidPXS
//

OpenGLMatSingleColorSolidPXS::OpenGLMatSingleColorSolidPXS(OpenGLRenderer* renderer, Material* fall_back_material):
	OpenGLMatSingleColorSolid(renderer, fall_back_material),
	OpenGLMatPXS(vp_, fp_) {
}

OpenGLMatSingleColorSolidPXS::~OpenGLMatSingleColorSolidPXS() {
#ifndef LEPRA_GL_ES
	if (uilepra::OpenGLExtensions::IsShaderAsmProgramsSupported() == true) {
		uilepra::OpenGLExtensions::glDeleteProgramsARB(1, (const GLuint*)&vpid_);
		uilepra::OpenGLExtensions::glDeleteProgramsARB(kNumFp, (const GLuint*)fpid_);
	}
#endif // !GLES
}

void OpenGLMatSingleColorSolidPXS::DoRenderAllGeometry(unsigned current_frame, const GeometryGroupList& geometry_group_list) {
	if (!GetRenderer()->IsPixelShadersEnabled() || !uilepra::OpenGLExtensions::IsShaderAsmProgramsSupported()) {
		Parent::DoRenderAllGeometry(current_frame, geometry_group_list);
		return;
	}

	OpenGLMatPXS::PrepareShaderPrograms((OpenGLRenderer*)GetRenderer());
	Parent::DoRenderAllGeometry(current_frame, geometry_group_list);
	OpenGLMatPXS::CleanupShaderPrograms();
}

void OpenGLMatSingleColorSolidPXS::RenderGeometry(tbc::GeometryBase* geometry) {
	if (!GetRenderer()->IsPixelShadersEnabled() || !uilepra::OpenGLExtensions::IsShaderAsmProgramsSupported()) {
		Parent::RenderGeometry(geometry);
		return;
	}

	SetBasicMaterial(geometry->GetBasicMaterialSettings());
	OpenGLMatPXS::SetAmbientLight((OpenGLRenderer*)GetRenderer(), geometry);

#ifndef LEPRA_GL_ES
	float specular[4];
	const tbc::GeometryBase::BasicMaterialSettings& mat_settings = geometry->GetBasicMaterialSettings();
	specular[0] = mat_settings.shininess_;
	specular[1] = mat_settings.shininess_;
	specular[2] = mat_settings.shininess_;
	specular[3] = mat_settings.shininess_;
	uilepra::OpenGLExtensions::glProgramLocalParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB, 0, specular);
#endif // !GLES

	Parent::RawRender(geometry, 0);

	ResetBasicMaterial(geometry->GetBasicMaterialSettings());
}




//
// OpenGLMatSingleTextureSolidPXS
//

OpenGLMatSingleTextureSolidPXS::OpenGLMatSingleTextureSolidPXS(OpenGLRenderer* renderer,
							   Material* fall_back_material) :
	OpenGLMatSingleTextureSolid(renderer, fall_back_material),
	OpenGLMatPXS(vp_, fp_) {
}

OpenGLMatSingleTextureSolidPXS::~OpenGLMatSingleTextureSolidPXS() {
#ifndef LEPRA_GL_ES
	if (uilepra::OpenGLExtensions::IsShaderAsmProgramsSupported()) {
		uilepra::OpenGLExtensions::glDeleteProgramsARB(1, (const GLuint*)&vpid_);
		uilepra::OpenGLExtensions::glDeleteProgramsARB(kNumFp, (const GLuint*)fpid_);
	}
#endif // !GLES
}

void OpenGLMatSingleTextureSolidPXS::DoRenderAllGeometry(unsigned current_frame, const GeometryGroupList& geometry_group_list) {
	// Early bail out if no multitexturing support. TODO: fix in cards that don't support!
	if (!GetRenderer()->IsPixelShadersEnabled() || !uilepra::OpenGLExtensions::IsMultiTextureSupported()) {
		Parent::DoRenderAllGeometry(current_frame, geometry_group_list);
		return;
	}

	OpenGLMatPXS::PrepareShaderPrograms((OpenGLRenderer*)GetRenderer());
	Parent::DoRenderAllGeometry(current_frame, geometry_group_list);
	OpenGLMatPXS::CleanupShaderPrograms();
}

void OpenGLMatSingleTextureSolidPXS::RenderGeometry(tbc::GeometryBase* geometry) {
	if (!GetRenderer()->IsPixelShadersEnabled() || !uilepra::OpenGLExtensions::IsShaderAsmProgramsSupported()) {
		Parent::RenderGeometry(geometry);
		return;
	}

	OpenGLRenderer::OGLGeometryData* _geometry = (OpenGLRenderer::OGLGeometryData*)geometry->GetRendererData();
	SetBasicMaterial(geometry->GetBasicMaterialSettings());
	OpenGLMatPXS::SetAmbientLight((OpenGLRenderer*)GetRenderer(), geometry);

	uilepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE0);
	uilepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE0);
	//glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	//glEnable(GL_TEXTURE_2D);
#ifndef LEPRA_GL_ES
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
#endif // !GLES

	BindTexture(_geometry->ta_->maps_[0].map_id_[Texture::kColorMap], _geometry->ta_->maps_[0].mip_map_level_count_[Texture::kColorMap]);

	OpenGLMaterial::UpdateTextureMatrix(_geometry->geometry_);

#ifndef LEPRA_GL_ES
	float specular[4];
	const tbc::GeometryBase::BasicMaterialSettings& mat_settings = geometry->GetBasicMaterialSettings();
	specular[0] = mat_settings.shininess_;
	specular[1] = mat_settings.shininess_;
	specular[2] = mat_settings.shininess_;
	specular[3] = mat_settings.shininess_;
	uilepra::OpenGLExtensions::glProgramLocalParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB, 0, specular);
#endif // !GLES

	Parent::RawRender(geometry, 0);

	ResetBasicMaterial(geometry->GetBasicMaterialSettings());
}



//
// OpenGLMatTextureAndLightmapPXS
//

OpenGLMatTextureAndLightmapPXS::OpenGLMatTextureAndLightmapPXS(OpenGLRenderer* renderer,
							       Material* fall_back_material) :
	Parent(renderer, fall_back_material),
	OpenGLMatPXS(vp_, fp_) {
}

OpenGLMatTextureAndLightmapPXS::~OpenGLMatTextureAndLightmapPXS() {
#ifndef LEPRA_GL_ES
	if (uilepra::OpenGLExtensions::IsShaderAsmProgramsSupported()) {
		uilepra::OpenGLExtensions::glDeleteProgramsARB(1, (const GLuint*)&vpid_);
		uilepra::OpenGLExtensions::glDeleteProgramsARB(kNumFp, (const GLuint*)fpid_);
	}
#endif // !GLES
}

void OpenGLMatTextureAndLightmapPXS::DoRenderAllGeometry(unsigned current_frame, const GeometryGroupList& geometry_group_list) {
	// Early bail out if no multitexturing support. TODO: fix in cards that don't support!
	if (!GetRenderer()->IsPixelShadersEnabled() || !uilepra::OpenGLExtensions::IsMultiTextureSupported()) {
		Parent::DoRenderAllGeometry(current_frame, geometry_group_list);
		return;
	}

	uilepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE0);
	uilepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);

	uilepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE1);
	uilepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE1);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);

	OpenGLMatPXS::PrepareShaderPrograms((OpenGLRenderer*)GetRenderer());
	Parent::DoRenderAllGeometry(current_frame, geometry_group_list);
	OpenGLMatPXS::CleanupShaderPrograms();

	uilepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE1);
	uilepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE1);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_TEXTURE_2D);

	uilepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE0);
	uilepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE0);
}

void OpenGLMatTextureAndLightmapPXS::RenderGeometry(tbc::GeometryBase* geometry) {
	if (!GetRenderer()->IsPixelShadersEnabled() || !uilepra::OpenGLExtensions::IsShaderAsmProgramsSupported()) {
		Parent::RenderGeometry(geometry);
		return;
	}

	SetBasicMaterial(geometry->GetBasicMaterialSettings());
	OpenGLMatPXS::SetAmbientLight((OpenGLRenderer*)GetRenderer(), geometry);

	RawRender(geometry, 0);

	ResetBasicMaterial(geometry->GetBasicMaterialSettings());
}

void OpenGLMatTextureAndLightmapPXS::RawRender(tbc::GeometryBase* geometry, int uv_set_index) {
	uilepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE0);
	uilepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE0);
	OpenGLRenderer::OGLGeometryData* _geometry = (OpenGLRenderer::OGLGeometryData*)geometry->GetRendererData();
	BindTexture(_geometry->ta_->maps_[0].map_id_[Texture::kColorMap], _geometry->ta_->maps_[0].mip_map_level_count_[Texture::kColorMap]);

	const int uv_count_per_vertex = geometry->GetUVCountPerVertex();
	const int type_index = enable_wireframe_? 1 : 0;
	if (uilepra::OpenGLExtensions::IsBufferObjectsSupported() == true) {
		GLuint index_buffer_id  = (GLuint)_geometry->index_buffer_id_;
		uilepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, (GLuint)_geometry->vertex_buffer_id_);
		glTexCoordPointer(uv_count_per_vertex, GL_FLOAT, 0, (GLvoid*)(_geometry->uv_offset_ + sizeof(float)*uv_count_per_vertex*uv_set_index*geometry->GetMaxVertexCount()));

		uilepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_id);
	} else {
		glTexCoordPointer(uv_count_per_vertex, GL_FLOAT, 0, geometry->GetUVData(uv_set_index));
	}
	OpenGLMaterial::UpdateTextureMatrix(_geometry->geometry_);

	uilepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE1);
	uilepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE1);
	BindTexture(_geometry->ta_->maps_[1].map_id_[Texture::kColorMap], _geometry->ta_->maps_[1].mip_map_level_count_[Texture::kColorMap]);

	if (uilepra::OpenGLExtensions::IsBufferObjectsSupported() == true) {
		GLuint index_buffer_id  = (GLuint)_geometry->index_buffer_id_;
		uilepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, (GLuint)_geometry->vertex_buffer_id_);
		glTexCoordPointer(uv_count_per_vertex, GL_FLOAT, 0, (GLvoid*)(_geometry->uv_offset_ + sizeof(float)*uv_count_per_vertex*(uv_set_index+1)*geometry->GetMaxVertexCount()));

		uilepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_id);
	} else {
		glTexCoordPointer(uv_count_per_vertex, GL_FLOAT, 0, geometry->GetUVData(uv_set_index+1));
	}

#ifndef LEPRA_GL_ES
	float specular[4];
	const tbc::GeometryBase::BasicMaterialSettings& mat_settings = geometry->GetBasicMaterialSettings();
	specular[0] = mat_settings.shininess_;
	specular[1] = mat_settings.shininess_;
	specular[2] = mat_settings.shininess_;
	specular[3] = mat_settings.shininess_;
	uilepra::OpenGLExtensions::glProgramLocalParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB, 0, specular);
#endif // !GLES

	if (uilepra::OpenGLExtensions::IsBufferObjectsSupported() == true) {
		glVertexPointer(3, GL_FLOAT, 0, (GLvoid*)_geometry->vertex_offset_);
		glNormalPointer(GL_FLOAT, 0, (GLvoid*)_geometry->normal_offset_);

		glDrawElements(OpenGLMaterial::GetGLElementType(geometry),
			       _geometry->index_count_[type_index],
			       LEPRA_GL_INDEX_TYPE,
			       (GLvoid*)_geometry->index_offset_[type_index]);
	} else {
		glVertexPointer(3, GL_FLOAT, 0, geometry->GetVertexData());
		glNormalPointer(GL_FLOAT, 0, geometry->GetNormalData());
		glDrawElements(OpenGLMaterial::GetGLElementType(geometry),
			       _geometry->index_count_[type_index],
			       LEPRA_GL_INDEX_TYPE,
			       (char*)geometry->GetIndexData() + _geometry->index_offset_[type_index]);
	}
}



//
// OpenGLMatTextureSBMapPXS
//

OpenGLMatTextureSBMapPXS::OpenGLMatTextureSBMapPXS(OpenGLRenderer* renderer,
						   Material* fall_back_material,
						   const str vp,
						   const str* fp) :
	OpenGLMatSingleTextureSolid(renderer, fall_back_material),
	OpenGLMatPXS(vp, fp) {
}

OpenGLMatTextureSBMapPXS::OpenGLMatTextureSBMapPXS(OpenGLRenderer* renderer,
						   Material* fall_back_material) :
	OpenGLMatSingleTextureSolid(renderer, fall_back_material),
	OpenGLMatPXS(vp_, fp_) {
}

OpenGLMatTextureSBMapPXS::~OpenGLMatTextureSBMapPXS() {
#ifndef LEPRA_GL_ES
	if (uilepra::OpenGLExtensions::IsShaderAsmProgramsSupported()) {
		uilepra::OpenGLExtensions::glDeleteProgramsARB(1, (const GLuint*)&vpid_);
		uilepra::OpenGLExtensions::glDeleteProgramsARB(kNumFp, (const GLuint*)fpid_);
	}
#endif // !GLES
}

void OpenGLMatTextureSBMapPXS::DoRenderAllGeometry(unsigned current_frame, const GeometryGroupList& geometry_group_list) {
	// Early bail out if no multitexturing support. TODO: fix in cards that don't support!
	if (!GetRenderer()->IsPixelShadersEnabled() || !uilepra::OpenGLExtensions::IsMultiTextureSupported()) {
		return;
	}

	uilepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE0);
	uilepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);

	uilepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE1);
	uilepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE1);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);

	uilepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE2);
	uilepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE2);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);

	OpenGLMatPXS::PrepareShaderPrograms((OpenGLRenderer*)GetRenderer());
	Parent::DoRenderAllGeometry(current_frame, geometry_group_list);
	OpenGLMatPXS::CleanupShaderPrograms();

	uilepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE2);
	uilepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE2);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_TEXTURE_2D);

	uilepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE1);
	uilepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE1);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_TEXTURE_2D);

	uilepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE0);
	uilepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE0);
}

void OpenGLMatTextureSBMapPXS::RenderGeometry(tbc::GeometryBase* geometry) {
	if (!GetRenderer()->IsPixelShadersEnabled() || !uilepra::OpenGLExtensions::IsShaderAsmProgramsSupported()) {
		Parent::RenderGeometry(geometry);
		return;
	}

	SetBasicMaterial(geometry->GetBasicMaterialSettings());
	OpenGLMatPXS::SetAmbientLight((OpenGLRenderer*)GetRenderer(), geometry);

	RawRender(geometry, 0);

	ResetBasicMaterial(geometry->GetBasicMaterialSettings());
}

void OpenGLMatTextureSBMapPXS::RawRender(tbc::GeometryBase* geometry, int uv_set_index) {
	OpenGLRenderer::OGLGeometryData* _geometry = (OpenGLRenderer::OGLGeometryData*)geometry->GetRendererData();

	// Texture unit 0, handles color map and regular
	// texture (UV) coordinates.
	uilepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE0);
	uilepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE0);
	BindTexture(_geometry->ta_->maps_[0].map_id_[Texture::kColorMap], _geometry->ta_->maps_[0].mip_map_level_count_[Texture::kColorMap]);

	int uv_count_per_vertex = geometry->GetUVCountPerVertex();
	const int type_index = enable_wireframe_? 1 : 0;
	if (uilepra::OpenGLExtensions::IsBufferObjectsSupported() == true) {
		GLuint index_buffer_id  = (GLuint)_geometry->index_buffer_id_;
		uilepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, (GLuint)_geometry->vertex_buffer_id_);
		glTexCoordPointer(uv_count_per_vertex, GL_FLOAT, 0, (GLvoid*)(_geometry->uv_offset_ + sizeof(float)*uv_count_per_vertex*uv_set_index*geometry->GetMaxVertexCount()));

		uilepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_id);
	} else {
		glTexCoordPointer(uv_count_per_vertex, GL_FLOAT, 0, geometry->GetUVData(uv_set_index));
	}

	OpenGLMaterial::UpdateTextureMatrix(_geometry->geometry_);

	// Texture unit 1, handles specular map and tangents.
	uilepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE1);
	uilepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE1);
	BindTexture(_geometry->ta_->maps_[0].map_id_[Texture::kSpecularMap], _geometry->ta_->maps_[0].mip_map_level_count_[Texture::kSpecularMap]);

	if (uilepra::OpenGLExtensions::IsBufferObjectsSupported() == true) {
		GLuint index_buffer_id  = (GLuint)_geometry->index_buffer_id_;
		uilepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, (GLuint)_geometry->vertex_buffer_id_);
		glTexCoordPointer(3, GL_FLOAT, 0, (GLvoid*)_geometry->tangent_offset_);

		uilepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_id);
	} else {
		glTexCoordPointer(3, GL_FLOAT, 0, geometry->GetTangentData());
	}

	// Texture unit 2, handles bitangents and normal map.
	uilepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE2);
	uilepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE2);
	BindTexture(_geometry->ta_->maps_[0].map_id_[Texture::kNormalMap], _geometry->ta_->maps_[0].mip_map_level_count_[Texture::kNormalMap]);
	if (uilepra::OpenGLExtensions::IsBufferObjectsSupported() == true) {
		GLuint index_buffer_id  = (GLuint)_geometry->index_buffer_id_;
		uilepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, (GLuint)_geometry->vertex_buffer_id_);
		glTexCoordPointer(3, GL_FLOAT, 0, (GLvoid*)_geometry->bitangent_offset_);

		uilepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_id);
	} else {
		glTexCoordPointer(3, GL_FLOAT, 0, geometry->GetBitangentData());
	}

#ifndef LEPRA_GL_ES
	float specular[4];
	const tbc::GeometryBase::BasicMaterialSettings& mat_settings = geometry->GetBasicMaterialSettings();
	specular[0] = mat_settings.shininess_;
	specular[1] = mat_settings.shininess_;
	specular[2] = mat_settings.shininess_;
	specular[3] = mat_settings.shininess_;
	uilepra::OpenGLExtensions::glProgramLocalParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB, 0, specular);
#endif // !GLES

	if (uilepra::OpenGLExtensions::IsBufferObjectsSupported() == true) {
		glVertexPointer(3, GL_FLOAT, 0, (GLvoid*)_geometry->vertex_offset_);
		glNormalPointer(GL_FLOAT, 0, (GLvoid*)_geometry->normal_offset_);

		glDrawElements(OpenGLMaterial::GetGLElementType(geometry),
			       _geometry->index_count_[type_index],
			       LEPRA_GL_INDEX_TYPE,
			       (GLvoid*)_geometry->index_offset_[type_index]);
	} else {
		glVertexPointer(3, GL_FLOAT, 0, geometry->GetVertexData());
		glNormalPointer(GL_FLOAT, 0, geometry->GetNormalData());
		glDrawElements(OpenGLMaterial::GetGLElementType(geometry),
			       _geometry->index_count_[type_index],
			       LEPRA_GL_INDEX_TYPE,
			       (char*)geometry->GetIndexData() + _geometry->index_offset_[type_index]);
	}
}

OpenGLMatTextureDiffuseBumpMapPXS::OpenGLMatTextureDiffuseBumpMapPXS(OpenGLRenderer* renderer,
								     Material* fall_back_material) :
	OpenGLMatTextureSBMapPXS(renderer, fall_back_material, vp_, fp_) {
}

OpenGLMatTextureDiffuseBumpMapPXS::~OpenGLMatTextureDiffuseBumpMapPXS() {
}



loginstance(kUiGfx3D, OpenGLMatVertexColorSolid);
loginstance(kUiGfx3D, OpenGLMatSingleTextureSolid);
loginstance(kUiGfx3D, OpenGLMatSingleColorEnvMapSolid);
loginstance(kUiGfx3D, OpenGLMatSingleTextureEnvMapSolid);
loginstance(kUiGfx3D, OpenGLMatTextureAndLightmap);
loginstance(kUiGfx3D, OpenGLMatPXS);
loginstance(kUiGfx3D, OpenGLMatSingleColorSolidPXS);
loginstance(kUiGfx3D, OpenGLMatSingleTextureSolidPXS);
loginstance(kUiGfx3D, OpenGLMatTextureAndLightmapPXS);
loginstance(kUiGfx3D, OpenGLMatTextureSBMapPXS);



}
