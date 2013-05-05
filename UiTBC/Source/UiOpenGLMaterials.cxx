
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#include "../../Lepra/Include/Log.h"
#include "../../Lepra/Include/Math.h"
#include "../../UiLepra/Include/UiOpenGLExtensions.h"
#include "../../TBC/Include/GeometryBase.h"
#include "../Include/UiOpenGLMaterials.h"
#include "../Include/UiOpenGLRenderer.h"



#ifdef LEPRA_DEBUG
#define OGL_ASSERT()	{ GLenum lGlError = glGetError(); assert(lGlError == GL_NO_ERROR); }
#else // !Debug
#define OGL_ASSERT()
#endif // Debug / !Debug



namespace UiTbc
{



OpenGLMaterial::OpenGLMaterial(OpenGLRenderer* pRenderer, Material::DepthSortHint pSortHint, Material* pFallBackMaterial) :
	Material(pRenderer, pSortHint, pFallBackMaterial)
{
}

OpenGLMaterial::~OpenGLMaterial()
{
}

Material::RemoveStatus OpenGLMaterial::RemoveGeometry(TBC::GeometryBase* pGeometry)
{
	Material::RemoveStatus lStatus = Parent::RemoveGeometry(pGeometry);
	if (lStatus == Material::NOT_REMOVED && mFallBackMaterial)
	{
		lStatus = mFallBackMaterial->RemoveGeometry(pGeometry);
		if (lStatus == Material::REMOVED)
		{
			lStatus = Material::REMOVED_FROM_FALLBACK;
		}
	}
	return lStatus;
}

GLenum OpenGLMaterial::GetGLElementType(TBC::GeometryBase* pGeometry)
{
	switch (pGeometry->GetPrimitiveType())
	{
		case TBC::GeometryBase::TRIANGLES:	return (GL_TRIANGLES);
		case TBC::GeometryBase::TRIANGLE_STRIP:	return (GL_TRIANGLE_STRIP);
		case TBC::GeometryBase::LINES:		return (GL_LINES);
		case TBC::GeometryBase::LINE_LOOP:	return (GL_LINE_LOOP);
	}
	assert(false);
	return (GL_TRIANGLES);
}

void OpenGLMaterial::SetBasicMaterial(const TBC::GeometryBase::BasicMaterialSettings& pMaterial)
{
	SetBasicMaterial(pMaterial, GetRenderer());
}

void OpenGLMaterial::SetBasicMaterial(const TBC::GeometryBase::BasicMaterialSettings& pMaterial, Renderer* pRenderer)
{
	mCurrentMaterial = pMaterial;

	const float lAmbient[]  = { pMaterial.mAmbient.x,  pMaterial.mAmbient.y,  pMaterial.mAmbient.z,  pMaterial.mAlpha };
	const float lDiffuse[]  = { pMaterial.mDiffuse.x,  pMaterial.mDiffuse.y,  pMaterial.mDiffuse.z,  pMaterial.mAlpha };
	const float lSpecular[] = { pMaterial.mSpecular.x, pMaterial.mSpecular.y, pMaterial.mSpecular.z, pMaterial.mAlpha };

	::glColor4f(lDiffuse[0], lDiffuse[1], lDiffuse[2], lDiffuse[3]);
	::glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, lAmbient);
	::glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, lDiffuse);
	::glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, lSpecular);
	::glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, pMaterial.mShininess*0.5f);
	::glShadeModel(pMaterial.mSmooth ? GL_SMOOTH : GL_FLAT);
	OGL_ASSERT();

	//pRenderer->SetGlobalMaterialReflectance(pMaterial.mDiffuse.x, pMaterial.mDiffuse.y, pMaterial.mDiffuse.z, pMaterial.mShininess);

	if (pMaterial.mAmbient.x || pMaterial.mAmbient.y || pMaterial.mAmbient.z)
	{
		pRenderer->AddAmbience(pMaterial.mAmbient.x, pMaterial.mAmbient.y, pMaterial.mAmbient.z);
	}
}

void OpenGLMaterial::ResetBasicMaterial(const TBC::GeometryBase::BasicMaterialSettings& pMaterial)
{
	if (pMaterial.mAmbient.x || pMaterial.mAmbient.y || pMaterial.mAmbient.z)
	{
		GetRenderer()->ResetAmbientLight(true);
	}
}



void OpenGLMaterial::RenderAllBlendedGeometry(unsigned pCurrentFrame, const GeometryGroupList& pGeometryGroupList)
{
	::glDepthMask(GL_FALSE);
	::glDisable(GL_CULL_FACE);
#ifndef LEPRA_GL_E
	GLint lOldFill[2];
	::glGetIntegerv(GL_POLYGON_MODE, lOldFill);
	::glPolygonMode(GL_FRONT_AND_BACK, GetRenderer()->IsWireframeEnabled()? GL_LINE : GL_FILL);
#endif // !GLES
	Parent::RenderAllBlendedGeometry(pCurrentFrame, pGeometryGroupList);
	::glEnable(GL_CULL_FACE);
	::glDepthMask(GL_TRUE);
#ifndef LEPRA_GL_ES
	::glPolygonMode(GL_FRONT, lOldFill[0]);
	::glPolygonMode(GL_BACK, lOldFill[1]);
#endif // !GLES
}



void OpenGLMaterial::UpdateTextureMatrix(TBC::GeometryBase* pGeometry)
{
	if (pGeometry->GetUVAnimator() != 0)
	{
		float lUVMatrix[16];
		pGeometry->GetUVTransform().GetAs4x4TransposeMatrix(lUVMatrix);
		glMatrixMode(GL_TEXTURE);
		glLoadMatrixf(lUVMatrix);
	}
	else
	{
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
	}
	glMatrixMode(GL_MODELVIEW);
}



void OpenGLMatSingleColorSolid::RenderGeometry(TBC::GeometryBase* pGeometry)
{
	SetBasicMaterial(pGeometry->GetBasicMaterialSettings());
	RawRender(pGeometry, 0);
	ResetBasicMaterial(pGeometry->GetBasicMaterialSettings());
}

void OpenGLMatSingleColorSolid::PreRender()
{
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

void OpenGLMatSingleColorSolid::PostRender()
{
	::glDisableClientState(GL_NORMAL_ARRAY);
	//::glDisableClientState(GL_VERTEX_ARRAY);
}

void OpenGLMatSingleColorSolid::RawRender(TBC::GeometryBase* pGeometry, int pUVSetIndex)
{
	(void)pUVSetIndex;
	RenderBaseGeometry(pGeometry);
}

void OpenGLMatSingleColorSolid::RenderBaseGeometry(TBC::GeometryBase* pGeometry)
{
	OpenGLRenderer::OGLGeometryData* lGeometry = (OpenGLRenderer::OGLGeometryData*)pGeometry->GetRendererData();

	if (pGeometry->GetNormalData() == 0)
	{
		glDisableClientState(GL_NORMAL_ARRAY);
	}
	else
	{
		glEnableClientState(GL_NORMAL_ARRAY);
	}

	if (UiLepra::OpenGLExtensions::IsBufferObjectsSupported() == true)
	{
		GLuint lVertexBufferID = (GLuint)lGeometry->mVertexBufferID;
		GLuint lIndexBufferID  = (GLuint)lGeometry->mIndexBufferID;

		UiLepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, lVertexBufferID);
		glVertexPointer(3, GL_FLOAT, 0, 0);

		glNormalPointer(GL_FLOAT, 0, (GLvoid*)lGeometry->mNormalOffset);

		UiLepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lIndexBufferID);
		glDrawElements(OpenGLMaterial::GetGLElementType(pGeometry),
			       pGeometry->GetIndexCount(),
			       LEPRA_GL_INDEX_TYPE,
			       0);
	}
	else
	{
		glVertexPointer(3, GL_FLOAT, 0, pGeometry->GetVertexData());
		glNormalPointer(GL_FLOAT, 0, pGeometry->GetNormalData());
		glDrawElements(OpenGLMaterial::GetGLElementType(pGeometry),
			       pGeometry->GetIndexCount(),
			       LEPRA_GL_INDEX_TYPE,
			       pGeometry->GetIndexData());
	}
}



void OpenGLMatSingleColorBlended::RenderAllGeometry(unsigned pCurrentFrame, const GeometryGroupList& pGeometryGroupList)
{
	if (mOutline)
	{
		DoRenderAllGeometry(pCurrentFrame, pGeometryGroupList);
	}
	else
	{
		RenderAllBlendedGeometry(pCurrentFrame, pGeometryGroupList);
	}
}

void OpenGLMatSingleColorBlended::DoPreRender()
{
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

void OpenGLMatSingleColorBlended::DoPostRender()
{
	::glDisableClientState(GL_NORMAL_ARRAY);
	//::glDisableClientState(GL_VERTEX_ARRAY);
	::glDisable(GL_BLEND);
}

void OpenGLMatSingleColorBlended::PreRender()
{
	DoPreRender();
}

void OpenGLMatSingleColorBlended::PostRender()
{
	DoPostRender();
}



bool OpenGLMatVertexColorSolid::AddGeometry(TBC::GeometryBase* pGeometry)
{
	if (!pGeometry->GetColorData())
	{
		if (mFallBackMaterial)
		{
			mLog.AWarning("Material \"VertexColorSolid\", passing geometry to fallback material.");
			return mFallBackMaterial->AddGeometry(pGeometry);
		}
		return false;
	}

	return Parent::AddGeometry(pGeometry);
}

void OpenGLMatVertexColorSolid::DoRenderAllGeometry(unsigned pCurrentFrame, const GeometryGroupList& pGeometryGroupList)
{
	// This is the only state we need to activate, since the other ones
	// were activated by OpenGLMatSingleColorSolid.
	::glEnableClientState(GL_COLOR_ARRAY);
	Parent::DoRenderAllGeometry(pCurrentFrame, pGeometryGroupList);
	::glDisableClientState(GL_COLOR_ARRAY);
}

void OpenGLMatVertexColorSolid::RenderGeometry(TBC::GeometryBase* pGeometry)
{
	SetBasicMaterial(pGeometry->GetBasicMaterialSettings());
	RawRender(pGeometry, 0);
	ResetBasicMaterial(pGeometry->GetBasicMaterialSettings());
}

void OpenGLMatVertexColorSolid::RawRender(TBC::GeometryBase* pGeometry, int pUVSetIndex)
{
	(void)pUVSetIndex;
	if (UiLepra::OpenGLExtensions::IsBufferObjectsSupported() == true)
	{
		OpenGLRenderer::OGLGeometryData* lGeometry = (OpenGLRenderer::OGLGeometryData*)pGeometry->GetRendererData();

		GLuint lVertexBufferID = (GLuint)lGeometry->mVertexBufferID;
		GLuint lIndexBufferID  = (GLuint)lGeometry->mIndexBufferID;

		UiLepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, lVertexBufferID);

		glVertexPointer(3, GL_FLOAT, 0, 0);
		glNormalPointer(GL_FLOAT, 0, (GLvoid*)lGeometry->mNormalOffset);

		int lSize = 4;
		if (pGeometry->GetColorFormat() == TBC::GeometryBase::COLOR_RGB)
			lSize = 3;

		glColorPointer(lSize, GL_UNSIGNED_BYTE, 0, (GLvoid*)lGeometry->mColorOffset);

		UiLepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lIndexBufferID);
		glDrawElements(OpenGLMaterial::GetGLElementType(pGeometry), 
			       pGeometry->GetIndexCount(),
			       LEPRA_GL_INDEX_TYPE,
			       0);
	}
	else
	{
		glVertexPointer(3, GL_FLOAT, 0, pGeometry->GetVertexData());
		glNormalPointer(GL_FLOAT, 0, pGeometry->GetNormalData());

		int lSize = 4;
		if (pGeometry->GetColorFormat() == TBC::GeometryBase::COLOR_RGB)
			lSize = 3;

		glColorPointer(lSize, GL_UNSIGNED_BYTE, 0, pGeometry->GetColorData());

		glDrawElements(OpenGLMaterial::GetGLElementType(pGeometry),
			       pGeometry->GetIndexCount(),
			       LEPRA_GL_INDEX_TYPE,
			       pGeometry->GetIndexData());
	}
}

void OpenGLMatVertexColorBlended::RenderAllGeometry(unsigned pCurrentFrame, const GeometryGroupList& pGeometryGroupList)
{
	RenderAllBlendedGeometry(pCurrentFrame, pGeometryGroupList);
}

void OpenGLMatVertexColorBlended::DoRenderAllGeometry(unsigned pCurrentFrame, const GeometryGroupList& pGeometryGroupList)
{
	// This is the only state we need to activate, since the other ones
	// were activated by previous materials.
	glEnableClientState(GL_COLOR_ARRAY);
	Parent::DoRenderAllGeometry(pCurrentFrame, pGeometryGroupList);
	glDisableClientState(GL_COLOR_ARRAY);
}



bool OpenGLMatSingleTextureSolid::AddGeometry(TBC::GeometryBase* pGeometry)
{
	if (pGeometry->GetUVSetCount() == 0)
	{
		if (mFallBackMaterial)
		{
			mLog.AWarning("Material \"SingleTextureSolid\", passing geometry to fallback material.");
			return mFallBackMaterial->AddGeometry(pGeometry);
		}
		return false;
	}
	return Parent::AddGeometry(pGeometry);
}

void OpenGLMatSingleTextureSolid::DoRawRender(TBC::GeometryBase* pGeometry, int pUVSetIndex)
{
	if (UiLepra::OpenGLExtensions::IsBufferObjectsSupported() == true)
	{
		OpenGLRenderer::OGLGeometryData* lGeometryData = (OpenGLRenderer::OGLGeometryData*)pGeometry->GetRendererData();

		GLuint lVertexBufferID = (GLuint)lGeometryData->mVertexBufferID;
		GLuint lIndexBufferID  = (GLuint)lGeometryData->mIndexBufferID;

		UiLepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, lVertexBufferID);

		glVertexPointer(3, GL_FLOAT, 0, 0);
		glNormalPointer(GL_FLOAT, 0, (GLvoid*)lGeometryData->mNormalOffset);
		glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)(lGeometryData->mUVOffset + sizeof(float)*2*pUVSetIndex*pGeometry->GetMaxVertexCount()));

		UiLepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lIndexBufferID);

		glDrawElements(OpenGLMaterial::GetGLElementType(pGeometry), pGeometry->GetIndexCount(), LEPRA_GL_INDEX_TYPE, 0);
	}
	else
	{
		glVertexPointer(3, GL_FLOAT, 0, pGeometry->GetVertexData());
		glNormalPointer(GL_FLOAT, 0, pGeometry->GetNormalData());
		glTexCoordPointer(2, GL_FLOAT, 0, pGeometry->GetUVData(pUVSetIndex));
		glDrawElements(OpenGLMaterial::GetGLElementType(pGeometry), pGeometry->GetIndexCount(), LEPRA_GL_INDEX_TYPE, pGeometry->GetIndexData());
	}
	OGL_ASSERT();
}

void OpenGLMatSingleTextureSolid::RenderGeometry(TBC::GeometryBase* pGeometry)
{
	SetBasicMaterial(pGeometry->GetBasicMaterialSettings());

	OpenGLRenderer::OGLGeometryData* lGeometry = (OpenGLRenderer::OGLGeometryData*)pGeometry->GetRendererData();
	BindTexture(lGeometry->mTA->mMaps[0].mMapID[Texture::COLOR_MAP], lGeometry->mTA->mMaps[0].mMipMapLevelCount[Texture::COLOR_MAP]);
	
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	OpenGLMaterial::UpdateTextureMatrix(lGeometry->mGeometry);

	RawRender(pGeometry, 0);

	ResetBasicMaterial(pGeometry->GetBasicMaterialSettings());
}

void OpenGLMatSingleTextureSolid::RawRender(TBC::GeometryBase* pGeometry, int pUVSetIndex)
{
	DoRawRender(pGeometry, pUVSetIndex);
}

void OpenGLMatSingleTextureSolid::PreRender()
{
	::glDisable(GL_ALPHA_TEST);
	::glDisable(GL_BLEND);
	//::glDisable(GL_LOGIC_OP);
	::glEnable(GL_TEXTURE_2D);

	::glEnableClientState(GL_VERTEX_ARRAY);
	::glEnableClientState(GL_NORMAL_ARRAY);
	::glDisableClientState(GL_COLOR_ARRAY);
	::glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	::glEnable(GL_COLOR_MATERIAL);
#ifndef LEPRA_GL_ES
	::glColorMaterial(GL_FRONT, GL_AMBIENT);
	::glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
#endif // !GLES
}

void OpenGLMatSingleTextureSolid::PostRender()
{
	::glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	::glDisableClientState(GL_NORMAL_ARRAY);
	//::glDisableClientState(GL_VERTEX_ARRAY);
	::glDisable(GL_TEXTURE_2D);
}

void OpenGLMatSingleTextureSolid::BindTexture(int pTextureID, int pMipMapLevelCount)
{
	glBindTexture(GL_TEXTURE_2D, pTextureID);
	OGL_ASSERT();

	GLint lTextureParamMin = GL_NEAREST;
	GLint lTextureParamMag = GL_NEAREST;

	if (pMipMapLevelCount <= 1 || !((OpenGLRenderer*)GetRenderer())->GetMipMappingEnabled())
	{
		// Just use plain vanilla.
	}
	else if (((OpenGLRenderer*)GetRenderer())->GetTrilinearFilteringEnabled() == true)
	{
		// The trilinear setting overrides the other ones.
		lTextureParamMin = GL_LINEAR_MIPMAP_LINEAR;
		lTextureParamMag = GL_LINEAR;
	}
	else if(((OpenGLRenderer*)GetRenderer())->GetBilinearFilteringEnabled() == true)
	{
		if (((OpenGLRenderer*)GetRenderer())->GetMipMappingEnabled() == true)
			lTextureParamMin = GL_LINEAR_MIPMAP_NEAREST;
		else
			lTextureParamMin = GL_LINEAR;

		lTextureParamMag = GL_LINEAR;
	}
	else if(((OpenGLRenderer*)GetRenderer())->GetMipMappingEnabled() == true)
	{
		lTextureParamMin = GL_NEAREST_MIPMAP_NEAREST;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, lTextureParamMin);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, lTextureParamMag);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	OGL_ASSERT();
}



void OpenGLMatSingleTextureHighlight::PreRender()
{
	Parent::PreRender();
	if (GetRenderer()->GetLightsEnabled())
	{
		::glDisable(GL_LIGHTING);
	}
}

void OpenGLMatSingleTextureHighlight::PostRender()
{
	if (GetRenderer()->GetLightsEnabled())
	{
		::glEnable(GL_LIGHTING);
	}
	Parent::PostRender();
}



void OpenGLMatSingleTextureBlended::RenderAllGeometry(unsigned pCurrentFrame, const GeometryGroupList& pGeometryGroupList)
{
	RenderAllBlendedGeometry(pCurrentFrame, pGeometryGroupList);
}

void OpenGLMatSingleTextureBlended::DoRenderAllGeometry(unsigned pCurrentFrame, const GeometryGroupList& pGeometryGroupList)
{
	Parent::DoRenderAllGeometry(pCurrentFrame, pGeometryGroupList);
}

void OpenGLMatSingleTextureBlended::PreRender()
{
	Parent::PreRender();
	::glEnable(GL_BLEND);
}

void OpenGLMatSingleTextureBlended::PostRender()
{
	::glDisable(GL_BLEND);
	Parent::PostRender();
}



void OpenGLMatSingleTextureAlphaTested::DoRenderAllGeometry(unsigned pCurrentFrame, const GeometryGroupList& pGeometryGroupList)
{
	glEnable(GL_ALPHA_TEST);
	Parent::DoRenderAllGeometry(pCurrentFrame, pGeometryGroupList);
	glDisable(GL_ALPHA_TEST);
}

void OpenGLMatSingleTextureAlphaTested::RenderGeometry(TBC::GeometryBase* pGeometry)
{
	const TBC::GeometryBase::BasicMaterialSettings& lMatSettings =
		pGeometry->GetBasicMaterialSettings();
	glAlphaFunc(GL_GEQUAL, lMatSettings.mAlpha);

	Parent::RenderGeometry(pGeometry);
}



bool OpenGLMatSingleColorEnvMapSolid::AddGeometry(TBC::GeometryBase* pGeometry)
{
	/*if (pGeometry->GetUVSetCount() == 0)
	{
		if (mFallBackMaterial)
		{
			mLog.AWarning("Material \"SingleColorEnvMapSolid\", passing geometry to fallback material.");
			return mFallBackMaterial->AddGeometry(pGeometry);
		}
		return false;
	}*/
	return OpenGLMatSingleColorSolid::AddGeometry(pGeometry);
}

void OpenGLMatSingleColorEnvMapSolid::DoRenderAllGeometry(unsigned pCurrentFrame, const GeometryGroupList& pGeometryGroupList)
{
	if (!GetRenderer()->GetEnvTexture())
	{
		mFallBackMaterial->DoRenderAllGeometry(pCurrentFrame, pGeometryGroupList);
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
	mSingleColorPass = true;
	::glDisable(GL_TEXTURE_2D);
	OpenGLMatSingleColorSolid::DoRenderAllGeometry(pCurrentFrame, pGeometryGroupList);
	::glEnable(GL_TEXTURE_2D);

	// Pass 2, Render the enviroment map.
	mSingleColorPass = false;
	//::glDepthFunc(GL_LEQUAL);
	//::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	::glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	BindTexture(GetRenderer()->GetEnvTexture()->mTMapID[Texture::COLOR_MAP], GetRenderer()->GetEnvTexture()->mTMipMapLevelCount[Texture::COLOR_MAP]);

/*#ifndef LEPRA_GL_ES
	if (((OpenGLRenderer*)GetRenderer())->IsEnvMapCubeMap() == true)
	{
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
	}
	else
	{
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

	Parent::DoRenderAllGeometry(pCurrentFrame, pGeometryGroupList);

	((OpenGLRenderer*)GetRenderer())->ResetAmbientLight(true);

/*#ifndef LEPRA_GL_ES
	if (((OpenGLRenderer*)GetRenderer())->IsEnvMapCubeMap() == true)
	{
		::glDisable(GL_TEXTURE_GEN_S);
		::glDisable(GL_TEXTURE_GEN_T);
		::glDisable(GL_TEXTURE_GEN_R);
		::glDisable(GL_TEXTURE_CUBE_MAP);
	}
	else
	{
		::glDisable(GL_TEXTURE_GEN_S);
		::glDisable(GL_TEXTURE_GEN_T);
	}
#endif // !GLES*/

	::glMatrixMode(GL_TEXTURE);
	::glLoadIdentity();
	::glMatrixMode(GL_MODELVIEW);
}

void OpenGLMatSingleColorEnvMapSolid::PreRender()
{
	if (mSingleColorPass)
	{
		OpenGLMatSingleColorSolid::PreRender();
	}
	else
	{
		Parent::PreRender();
		::glEnable(GL_BLEND);
		::glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		::glColor4f(1, 1, 1, 1);
		const float c[] = {1,1,1,1};
		::glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, c);
		::glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		::glDepthFunc(GL_LEQUAL);
	}
}

void OpenGLMatSingleColorEnvMapSolid::PostRender()
{
	if (mSingleColorPass)
	{
		OpenGLMatSingleColorSolid::PostRender();
	}
	else
	{
		Parent::PostRender();
		::glDisable(GL_BLEND);
		::glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		::glDepthFunc(GL_LESS);
	}
}

void OpenGLMatSingleColorEnvMapSolid::RenderGeometry(TBC::GeometryBase* pGeometry)
{
	if (mSingleColorPass)
	{
		SetBasicMaterial(pGeometry->GetBasicMaterialSettings());
		OpenGLMatSingleColorSolid::RawRender(pGeometry, 0);
		ResetBasicMaterial(pGeometry->GetBasicMaterialSettings());
	}
	else
	{
		::glMatrixMode(GL_TEXTURE);
		float lTextureMatrix[16];
		TransformationF lObjectTransform = pGeometry->GetTransformation();
		lObjectTransform.RotateWorldX(PIF/2);
		(lObjectTransform.Inverse() * ((OpenGLRenderer*)GetRenderer())->GetCameraTransformation()).GetAs4x4OrientationMatrix(lTextureMatrix);
		lTextureMatrix[15] *= 3.0f;
		::glLoadMatrixf(lTextureMatrix);

		if (UiLepra::OpenGLExtensions::IsBufferObjectsSupported() == true)
		{
			OpenGLRenderer::OGLGeometryData* lGeometryData = (OpenGLRenderer::OGLGeometryData*)pGeometry->GetRendererData();

			GLuint lVertexBufferID = (GLuint)lGeometryData->mVertexBufferID;
			GLuint lIndexBufferID  = (GLuint)lGeometryData->mIndexBufferID;

			UiLepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, lVertexBufferID);

			glVertexPointer(3, GL_FLOAT, 0, 0);
			glNormalPointer(GL_FLOAT, 0, (GLvoid*)lGeometryData->mNormalOffset);
			glTexCoordPointer(3, GL_FLOAT, 0, (GLvoid*)lGeometryData->mNormalOffset);	// Use vertex coordinates instead.

			UiLepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lIndexBufferID);

			glDrawElements(OpenGLMaterial::GetGLElementType(pGeometry), pGeometry->GetIndexCount(), LEPRA_GL_INDEX_TYPE, 0);
		}
		else
		{
			glVertexPointer(3, GL_FLOAT, 0, pGeometry->GetVertexData());
			glNormalPointer(GL_FLOAT, 0, pGeometry->GetNormalData());
			glTexCoordPointer(3, GL_FLOAT, 0, pGeometry->GetNormalData());	// Use vertex coordinates instead.
			glDrawElements(OpenGLMaterial::GetGLElementType(pGeometry), pGeometry->GetIndexCount(), LEPRA_GL_INDEX_TYPE, pGeometry->GetIndexData());
		}
		OGL_ASSERT();
	}
}



void OpenGLMatSingleColorEnvMapBlended::RenderAllGeometry(unsigned pCurrentFrame, const GeometryGroupList& pGeometryGroupList)
{
	RenderAllBlendedGeometry(pCurrentFrame, pGeometryGroupList);
}

void OpenGLMatSingleColorEnvMapBlended::PreRender()
{
	if (mSingleColorPass)
	{
		OpenGLMatSingleColorBlended::DoPreRender();
	}
	else
	{
		Parent::PreRender();
	}
}

void OpenGLMatSingleColorEnvMapBlended::PostRender()
{
	if (mSingleColorPass)
	{
		OpenGLMatSingleColorBlended::DoPostRender();
	}
	else
	{
		::glDisable(GL_BLEND);
	}
}



void OpenGLMatSingleTextureEnvMapSolid::DoRenderAllGeometry(unsigned pCurrentFrame, const GeometryGroupList& pGeometryGroupList)
{
	if (!UiLepra::OpenGLExtensions::IsMultiTextureSupported() || !GetRenderer()->GetEnvTexture())
	{
		mFallBackMaterial->DoRenderAllGeometry(pCurrentFrame, pGeometryGroupList);
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
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	// Pass 1, single texture.
	mSingleTexturePass = true;
	glEnable(GL_TEXTURE_2D);
	Parent::DoRenderAllGeometry(pCurrentFrame, pGeometryGroupList);

	// Early bail out if no multitexturing support. TODO: fix in cards that don't support!
	if (!UiLepra::OpenGLExtensions::IsMultiTextureSupported())
	{
		return;
	}

	// Pass 2, Render the enviroment map.
	mSingleTexturePass = false;

	UiLepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE1);
	UiLepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE1);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	BindTexture(GetRenderer()->GetEnvTexture()->mTMapID[Texture::COLOR_MAP], GetRenderer()->GetEnvTexture()->mTMipMapLevelCount[Texture::COLOR_MAP]);

#ifndef LEPRA_GL_ES
	if (((OpenGLRenderer*)GetRenderer())->IsEnvMapCubeMap() == true)
	{
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
	}
	else
	{
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
	((OpenGLRenderer*)GetRenderer())->GetCameraTransformation().GetAs4x4OrientationMatrix(lTextureMatrix);

	glLoadMatrixf(lTextureMatrix);
	glMatrixMode(GL_MODELVIEW);

	float lAmbientRed;
	float lAmbientGreen;
	float lAmbientBlue;
	((OpenGLRenderer*)GetRenderer())->GetAmbientLight(lAmbientRed, lAmbientGreen, lAmbientBlue);
	((OpenGLRenderer*)GetRenderer())->SetAmbientLight(1.0f, 1.0f, 1.0f);

	UiLepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE0);
	UiLepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE0);

	GLboolean lBlendEnabled = glIsEnabled(GL_BLEND);

	glEnable(GL_BLEND);

	//
	// Render the geometry.
	//
	Parent::DoRenderAllGeometry(pCurrentFrame, pGeometryGroupList);

	if (!lBlendEnabled)
		glDisable(GL_BLEND);

	((OpenGLRenderer*)GetRenderer())->SetAmbientLight(lAmbientRed, lAmbientGreen, lAmbientBlue);

	UiLepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE1);
	UiLepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE1);
#ifndef LEPRA_GL_ES
	if (((OpenGLRenderer*)GetRenderer())->IsEnvMapCubeMap() == true)
	{
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_TEXTURE_GEN_R);
		glDisable(GL_TEXTURE_CUBE_MAP);
	}
	else
	{
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_TEXTURE_2D);
	}
#endif // !GLES

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);

	UiLepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE0);
	UiLepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void OpenGLMatSingleTextureEnvMapSolid::RenderGeometry(TBC::GeometryBase* pGeometry)
{
	if (!UiLepra::OpenGLExtensions::IsMultiTextureSupported())
	{
		Parent::RenderGeometry(pGeometry);
		return;
	}

	SetBasicMaterial(pGeometry->GetBasicMaterialSettings());

	OpenGLRenderer::OGLGeometryData* lGeometry = (OpenGLRenderer::OGLGeometryData*)pGeometry->GetRendererData();
	glBindTexture(GL_TEXTURE_2D, lGeometry->mTA->mMaps[0].mMapID[Texture::COLOR_MAP]);
	OpenGLMaterial::UpdateTextureMatrix(lGeometry->mGeometry);

	RawRender(pGeometry, 0);

	ResetBasicMaterial(pGeometry->GetBasicMaterialSettings());
}

void OpenGLMatSingleTextureEnvMapSolid::RawRender(TBC::GeometryBase* pGeometry, int pUVSetIndex)
{
	if (UiLepra::OpenGLExtensions::IsBufferObjectsSupported() == true)
	{
		OpenGLRenderer::OGLGeometryData* lGeometry = (OpenGLRenderer::OGLGeometryData*)pGeometry->GetRendererData();

		GLuint lVertexBufferID = (GLuint)lGeometry->mVertexBufferID;
		GLuint lIndexBufferID  = (GLuint)lGeometry->mIndexBufferID;

		UiLepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, lVertexBufferID);

		glVertexPointer(3, GL_FLOAT, 0, 0);
		glNormalPointer(GL_FLOAT, 0, (GLvoid*)lGeometry->mNormalOffset);
		glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)(lGeometry->mUVOffset + sizeof(float)*2*pUVSetIndex*pGeometry->GetMaxVertexCount()));

		UiLepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lIndexBufferID);
		glDrawElements(OpenGLMaterial::GetGLElementType(pGeometry), 
			       pGeometry->GetIndexCount(),
			       LEPRA_GL_INDEX_TYPE,
			       0);
	}
	else
	{
		glVertexPointer(3, GL_FLOAT, 0, pGeometry->GetVertexData());
		glNormalPointer(GL_FLOAT, 0, pGeometry->GetNormalData());
		glTexCoordPointer(2, GL_FLOAT, 0, pGeometry->GetUVData(pUVSetIndex));
		glDrawElements(OpenGLMaterial::GetGLElementType(pGeometry),
			       pGeometry->GetIndexCount(),
			       LEPRA_GL_INDEX_TYPE,
			       pGeometry->GetIndexData());
	}
}

void OpenGLMatSingleTextureEnvMapBlended::RenderAllGeometry(unsigned pCurrentFrame, const GeometryGroupList& pGeometryGroupList)
{
	RenderAllBlendedGeometry(pCurrentFrame, pGeometryGroupList);
}



void OpenGLMatTextureAndLightmap::DoRenderAllGeometry(unsigned pCurrentFrame, const GeometryGroupList& pGeometryGroupList)
{
	// Early bail out if no multitexturing support. TODO: fix in cards that don't support!
	if (!UiLepra::OpenGLExtensions::IsMultiTextureSupported())
	{
		Parent::DoRenderAllGeometry(pCurrentFrame, pGeometryGroupList);
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

	mFirstPass = true;

	UiLepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE0);
	UiLepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	UiLepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE1);
	UiLepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	float lAmbientRed;
	float lAmbientGreen;
	float lAmbientBlue;
	((OpenGLRenderer*)GetRenderer())->GetAmbientLight(lAmbientRed, lAmbientGreen, lAmbientBlue);
	((OpenGLRenderer*)GetRenderer())->SetAmbientLight(1.0f, 1.0f, 1.0f);

	bool lLightsEnabled = glIsEnabled(GL_LIGHTING) != 0;
	glDisable(GL_LIGHTING);

	Parent::DoRenderAllGeometry(pCurrentFrame, pGeometryGroupList);

	if (lLightsEnabled == true)
		glEnable(GL_LIGHTING);
	((OpenGLRenderer*)GetRenderer())->SetAmbientLight(lAmbientRed, lAmbientGreen, lAmbientBlue);

	UiLepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE1);
	UiLepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_2D);

	//
	// Pass 2.
	//
	mFirstPass = false;

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	Parent::DoRenderAllGeometry(pCurrentFrame, pGeometryGroupList);

	UiLepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE1);
	UiLepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glDisable(GL_BLEND);

	UiLepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE0);
	UiLepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE0);
}

void OpenGLMatTextureAndLightmap::RenderGeometry(TBC::GeometryBase* pGeometry)
{
	if (!UiLepra::OpenGLExtensions::IsMultiTextureSupported() ||
		pGeometry->GetUVSetCount() < 2)
	{
		Parent::RenderGeometry(pGeometry);
		return;
	}

	SetBasicMaterial(pGeometry->GetBasicMaterialSettings());

	RawRender(pGeometry, 0);

	ResetBasicMaterial(pGeometry->GetBasicMaterialSettings());
}

void OpenGLMatTextureAndLightmap::RawRender(TBC::GeometryBase* pGeometry, int pUVSetIndex)
{
	OpenGLRenderer::OGLGeometryData* lGeometry = (OpenGLRenderer::OGLGeometryData*)pGeometry->GetRendererData();

	// Setup the color map in texture unit 0.
	UiLepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE0);
	UiLepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE0);
	BindTexture(lGeometry->mTA->mMaps[0].mMapID[Texture::COLOR_MAP], lGeometry->mTA->mMaps[0].mMipMapLevelCount[Texture::COLOR_MAP]);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	if (UiLepra::OpenGLExtensions::IsBufferObjectsSupported() == true)
	{
		UiLepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, (GLuint)lGeometry->mVertexBufferID);
		glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)(lGeometry->mUVOffset + sizeof(float)*2*pUVSetIndex*pGeometry->GetMaxVertexCount()));
	}
	else
	{
		glTexCoordPointer(2, GL_FLOAT, 0, pGeometry->GetUVData(pUVSetIndex));
	}

	OpenGLMaterial::UpdateTextureMatrix(lGeometry->mGeometry);

	if (mFirstPass == true)
	{
		//
		// Render pass 1 - color map modulated with light map.
		//

		// Setup the light map in texture unit 1.
		UiLepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE1);
		UiLepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE1);
		BindTexture(lGeometry->mTA->mMaps[1].mMapID[Texture::COLOR_MAP], lGeometry->mTA->mMaps[1].mMipMapLevelCount[Texture::COLOR_MAP]);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		if (UiLepra::OpenGLExtensions::IsBufferObjectsSupported() == true)
		{
			UiLepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, (GLuint)lGeometry->mVertexBufferID);
			glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)(lGeometry->mUVOffset + sizeof(float)*2*(pUVSetIndex+1)*pGeometry->GetMaxVertexCount()));
		}
		else
		{
			glTexCoordPointer(2, GL_FLOAT, 0, pGeometry->GetUVData(pUVSetIndex+1));
		}
	}

	if (UiLepra::OpenGLExtensions::IsBufferObjectsSupported() == true)
	{
		GLuint lIndexBufferID  = (GLuint)lGeometry->mIndexBufferID;
		
		// Vertex buffer already bound.
		//OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, (GLuint)lGeometry->mVertexBufferID);

		glVertexPointer(3, GL_FLOAT, 0, 0);
		glNormalPointer(GL_FLOAT, 0, (GLvoid*)lGeometry->mNormalOffset);

		UiLepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lIndexBufferID);
		glDrawElements(OpenGLMaterial::GetGLElementType(pGeometry), 
			       pGeometry->GetIndexCount(),
			       LEPRA_GL_INDEX_TYPE,
			       0);
	}
	else
	{
		glVertexPointer(3, GL_FLOAT, 0, pGeometry->GetVertexData());
		glNormalPointer(GL_FLOAT, 0, pGeometry->GetNormalData());
		glDrawElements(OpenGLMaterial::GetGLElementType(pGeometry),
			       pGeometry->GetIndexCount(),
			       LEPRA_GL_INDEX_TYPE,
			       pGeometry->GetIndexData());
	}
}

//
// OpenGLMatPXS
//

int  OpenGLMatPXS::smProgramCount = 0;
bool OpenGLMatPXS::smFPLUTInitialized = false;
int  OpenGLMatPXS::smFPLUT[4][4][4];

float OpenGLMatPXS::smLightPos[MAX_SHADER_LIGHTS * 4];
float OpenGLMatPXS::smLightDir[MAX_SHADER_LIGHTS * 4];
float OpenGLMatPXS::smLightCol[MAX_SHADER_LIGHTS * 4];
float OpenGLMatPXS::smLightCut[MAX_SHADER_LIGHTS];
float OpenGLMatPXS::smLightExp[MAX_SHADER_LIGHTS];

Renderer::LightType OpenGLMatPXS::smLightType[MAX_SHADER_LIGHTS];

int OpenGLMatPXS::smNumDirLights = 0;
int OpenGLMatPXS::smNumPntLights = 0;
int OpenGLMatPXS::smNumSptLights = 0;
int OpenGLMatPXS::smLightCount = 0;


OpenGLMatPXS::OpenGLMatPXS(const astr& pVP, const astr pFP[NUM_FP]):
	mVPID(0)
{
#ifndef LEPRA_GL_ES
	::memset(mFPID, 0, sizeof(mFPID));

	if (smFPLUTInitialized == false)
	{
		//
		// Precalculate the fragment program lookup table.
		//

		int p, s; // p for point lights, and s for spot lights.

		smFPLUT[0][0][0] = FP_NONE;
		smFPLUT[0][0][1] = FP_1SPOT;
		smFPLUT[0][0][2] = FP_2SPOT;
		smFPLUT[0][0][3] = FP_3SPOT;

		smFPLUT[0][1][0] = FP_1POINT;
		smFPLUT[0][1][1] = FP_1POINT1SPOT;
		smFPLUT[0][1][2] = FP_1POINT2SPOT;
		smFPLUT[0][1][3] = FP_1POINT2SPOT;

		smFPLUT[0][2][0] = FP_2POINT;
		smFPLUT[0][2][1] = FP_2POINT1SPOT;
		smFPLUT[0][2][2] = FP_2POINT1SPOT;
		smFPLUT[0][2][3] = FP_2POINT1SPOT;

		smFPLUT[0][3][0] = FP_3POINT;
		smFPLUT[0][3][1] = FP_3POINT;
		smFPLUT[0][3][2] = FP_3POINT;
		smFPLUT[0][3][3] = FP_3POINT;



		smFPLUT[1][0][0] = FP_1DIR;
		smFPLUT[1][0][1] = FP_1DIR1SPOT;
		smFPLUT[1][0][2] = FP_1DIR2SPOT;
		smFPLUT[1][0][3] = FP_1DIR2SPOT;

		smFPLUT[1][1][0] = FP_1DIR1POINT;
		smFPLUT[1][1][1] = FP_1DIR1POINT1SPOT;
		smFPLUT[1][1][2] = FP_1DIR1POINT1SPOT;
		smFPLUT[1][1][3] = FP_1DIR1POINT1SPOT;

		for (p = 2; p < 4; p++)
			for (s = 0; s < 4; s++)
				smFPLUT[1][p][s] = FP_1DIR2POINT;



		smFPLUT[2][0][0] = FP_2DIR;
		smFPLUT[2][0][1] = FP_2DIR1SPOT;
		smFPLUT[2][0][2] = FP_2DIR1SPOT;
		smFPLUT[2][0][3] = FP_2DIR1SPOT;

		for (p = 1; p < 4; p++)
			for (s = 0; s < 4; s++)
				smFPLUT[2][p][s] = FP_2DIR1POINT;



		for (p = 0; p < 4; p++)
			for (s = 0; s < 4; s++)
				smFPLUT[3][p][s] = FP_3DIR;

		smFPLUTInitialized = true;
	}

	if (UiLepra::OpenGLExtensions::IsShaderAsmProgramsSupported())
	{
		int i;

		//
		// Initialize the vertex shader.
		//

		glEnable(GL_VERTEX_PROGRAM_ARB);
		// Allocate a new ID for this vertex program.
		mVPID = AllocProgramID();

		UiLepra::OpenGLExtensions::glBindProgramARB(GL_VERTEX_PROGRAM_ARB, mVPID);

		GLint lErrorPos;
		UiLepra::OpenGLExtensions::glProgramStringARB(GL_VERTEX_PROGRAM_ARB, 
							    GL_PROGRAM_FORMAT_ASCII_ARB,
							    (GLsizei)pVP.length(),
							    pVP.c_str());
		glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB, &lErrorPos);
		if (lErrorPos != -1)
		{
			astr lGlError((const char*)glGetString(GL_PROGRAM_ERROR_STRING_ARB));
			mLog.Errorf(_T("Error in vertex shader at pos %i!\r\n%s\r\n"), lErrorPos, strutil::Encode(lGlError).c_str());
		}
		glDisable(GL_VERTEX_PROGRAM_ARB);

		//
		// Initialize the fragment shaders.
		//

		glEnable(GL_FRAGMENT_PROGRAM_ARB);
		for (i = 0; i < NUM_FP; i++)
		{
			// Allocate a new ID for this fragment program.
			mFPID[i] = AllocProgramID();

			UiLepra::OpenGLExtensions::glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, mFPID[i]);

			GLint lErrorPos;
			UiLepra::OpenGLExtensions::glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB, 
								    GL_PROGRAM_FORMAT_ASCII_ARB,
								    (GLsizei)pFP[i].length(),
								    pFP[i].c_str());
			glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB, &lErrorPos);
			if (lErrorPos != -1)
			{
				astr lGlError((const char*)glGetString(GL_PROGRAM_ERROR_STRING_ARB));
				mLog.Errorf(_T("Error in fragment shader %i at pos %i!\r\n%s\r\n"), i, lErrorPos, strutil::Encode(lGlError).c_str());
				mLog.AInfo("Setting fragment shader to fallback shader.");

				SetToFallbackFP(i);
			}
		}
		glDisable(GL_FRAGMENT_PROGRAM_ARB);
	}
#endif // !GLES
}

void OpenGLMatPXS::SetToFallbackFP(int pFPType)
{
#ifndef LEPRA_GL_ES
	int& lDestFP = mFPID[pFPType];

	switch(pFPType)
	{
	case FP_NONE:
		lDestFP = mFPID[FP_NONE];	// There is no fallback on this one.
		break;
	case FP_1POINT:
		lDestFP = mFPID[FP_NONE];
		break;
	case FP_2POINT:
		lDestFP = mFPID[FP_1POINT];
		break;
	case FP_3POINT:
		lDestFP = mFPID[FP_2POINT];
		break;
	case FP_1DIR:
		lDestFP = mFPID[FP_NONE];
		break;
	case FP_2DIR:
		lDestFP = mFPID[FP_1DIR];
		break;
	case FP_3DIR:
		lDestFP = mFPID[FP_2DIR];
		break;
	case FP_1SPOT:
		lDestFP = mFPID[FP_NONE];
		break;
	case FP_2SPOT:
		lDestFP = mFPID[FP_1SPOT];
		break;
	case FP_3SPOT:
		lDestFP = mFPID[FP_2SPOT];
		break;
	case FP_1DIR1POINT:
		lDestFP = mFPID[FP_1DIR];
		break;
	case FP_1DIR2POINT:
		lDestFP = mFPID[FP_1DIR1POINT];
		break;
	case FP_2DIR1POINT:
		lDestFP = mFPID[FP_2DIR];
		break;
	case FP_1DIR1SPOT:
		lDestFP = mFPID[FP_1DIR];
		break;
	case FP_1DIR2SPOT:
		lDestFP = mFPID[FP_1DIR1SPOT];
		break;
	case FP_2DIR1SPOT:
		lDestFP = mFPID[FP_2DIR];
		break;
	case FP_1POINT1SPOT:
		lDestFP = mFPID[FP_1POINT];
		break;
	case FP_1POINT2SPOT:
		lDestFP = mFPID[FP_1POINT1SPOT];
		break;
	case FP_2POINT1SPOT:
		lDestFP = mFPID[FP_2POINT];
		break;
	case FP_1DIR1POINT1SPOT:
		lDestFP = mFPID[FP_1DIR1POINT];
		break;
	};
#endif // !GLES
}

void OpenGLMatPXS::PrepareLights(OpenGLRenderer* pRenderer)
{
#ifndef LEPRA_GL_ES
	//
	// All pixel shaded materials support up to 3 simultaneous lights. 
	// The fragment shaders need the lights sorted in the following order:
	// 1. Directional lights.
	// 2. Point lights.
	// 3. Spot lights.
	//
	enum
	{
		NUM_BUCKETS = 3
	};

	// One bucket per light type.
	float lLightPos[NUM_BUCKETS * MAX_SHADER_LIGHTS * 4];
	float lLightDir[NUM_BUCKETS * MAX_SHADER_LIGHTS * 4];
	float lLightCol[NUM_BUCKETS * MAX_SHADER_LIGHTS * 4];

	// Cutoff angle for each light. Assumes that MAX_SHADER_LIGHTS <= 4.
	float lLightCut[NUM_BUCKETS * MAX_SHADER_LIGHTS];
	float lLightExp[NUM_BUCKETS * MAX_SHADER_LIGHTS];

	::memset(lLightPos, 0, sizeof(lLightPos));
	::memset(lLightDir, 0, sizeof(lLightDir));
	::memset(lLightCol, 0, sizeof(lLightCol));
	::memset(lLightCut, 0, sizeof(lLightCut));
	::memset(lLightExp, 0, sizeof(lLightExp));

	smNumDirLights = 0;
	smNumPntLights = 0;
	smNumSptLights = 0;
	int lTotalLightCount = 0;
	int i;

	// Sorting the lights with respect to the camera.
	pRenderer->SortLights(pRenderer->GetCameraTransformation().GetPosition());

	// Count the number of lights of each type, and get their position/direction and color.
	for (i = 0; i < pRenderer->GetLightCount() && lTotalLightCount < MAX_SHADER_LIGHTS; ++i)
	{
		const Renderer::LightID lLightID = pRenderer->GetClosestLight(i);
		GLenum lGLLight = GL_LIGHT0 + (int)lLightID;

		int lLightIndex = 0;

		// Check if the light is enabled, if it's not, we are in the 
		// "shadow rendering pass".
		if (::glIsEnabled(lGLLight) == GL_TRUE)
		{
			// Get light type.
			smLightType[lTotalLightCount] = pRenderer->GetLightType(lLightID);
			int lBucket = (int)smLightType[lTotalLightCount];
			int lIndex = 0;

			// Get light position and direction.
			switch(smLightType[lTotalLightCount])
			{
			case Renderer::LIGHT_DIRECTIONAL:
			{
				lIndex = lBucket * MAX_SHADER_LIGHTS * 4 + smNumDirLights * 4;
				lLightIndex = smNumDirLights;

				glGetLightfv(lGLLight, GL_POSITION, &lLightDir[lIndex]);

				smNumDirLights++;
				break;
			}
			case Renderer::LIGHT_POINT:
				lIndex = lBucket * MAX_SHADER_LIGHTS * 4 + smNumPntLights * 4;
				lLightIndex = smNumPntLights;

				glGetLightfv(lGLLight, GL_POSITION, &lLightPos[lIndex]);

				smNumPntLights++;
				break;
			case Renderer::LIGHT_SPOT:
			{
				lIndex = lBucket * MAX_SHADER_LIGHTS * 4 + smNumSptLights * 4;
				lLightIndex = smNumSptLights;

				glGetLightfv(lGLLight, GL_POSITION, &lLightPos[lIndex]);
				glGetLightfv(lGLLight, GL_SPOT_DIRECTION, &lLightDir[lIndex]);

				smNumSptLights++;
				break;
			}
			default: break;
			}

			// Get light color.
			const Vector3DF lLightColor = pRenderer->GetLightColor(lLightID);
			lLightCol[lIndex + 0] = lLightColor.x;
			lLightCol[lIndex + 1] = lLightColor.y;
			lLightCol[lIndex + 2] = lLightColor.z;

			float lCutoffAngle = pRenderer->GetLightCutoffAngle(lLightID);
			lLightCut[lBucket * MAX_SHADER_LIGHTS + lLightIndex] = (float)cos(lCutoffAngle * PIF / 180.0f);

			lLightExp[lBucket * MAX_SHADER_LIGHTS + lLightIndex] = pRenderer->GetLightSpotExponent(lLightID);

			++lTotalLightCount;
		}
	}



	// Set light position, direction and color in sorted order.
	smLightCount = 0;

	for (i = 0; i < smNumDirLights; i++)
	{
		int lIndex = (int)Renderer::LIGHT_DIRECTIONAL * MAX_SHADER_LIGHTS * 4 + i * 4;
		memcpy(&smLightPos[smLightCount * 4], &lLightPos[lIndex], 4 * sizeof(float));
		memcpy(&smLightDir[smLightCount * 4], &lLightDir[lIndex], 4 * sizeof(float));
		memcpy(&smLightCol[smLightCount * 4], &lLightCol[lIndex], 4 * sizeof(float));

		lIndex = (int)Renderer::LIGHT_DIRECTIONAL * MAX_SHADER_LIGHTS + i;
		smLightCut[smLightCount] = lLightCut[lIndex];
		smLightExp[smLightCount] = lLightExp[lIndex];

		smLightCount++;
	}

	for (i = 0; i < smNumPntLights; i++)
	{
		int lIndex = (int)Renderer::LIGHT_POINT * MAX_SHADER_LIGHTS * 4 + i * 4;
		memcpy(&smLightPos[smLightCount * 4], &lLightPos[lIndex], 4 * sizeof(float));
		memcpy(&smLightDir[smLightCount * 4], &lLightDir[lIndex], 4 * sizeof(float));
		memcpy(&smLightCol[smLightCount * 4], &lLightCol[lIndex], 4 * sizeof(float));

		lIndex = (int)Renderer::LIGHT_POINT * MAX_SHADER_LIGHTS + i;
		smLightCut[smLightCount] = lLightCut[lIndex];
		smLightExp[smLightCount] = lLightExp[lIndex];

		smLightCount++;
	}

	for (i = 0; i < smNumSptLights; i++)
	{
		int lIndex = (int)Renderer::LIGHT_SPOT * MAX_SHADER_LIGHTS * 4 + i * 4;
		memcpy(&smLightPos[smLightCount * 4], &lLightPos[lIndex], 4 * sizeof(float));
		memcpy(&smLightDir[smLightCount * 4], &lLightDir[lIndex], 4 * sizeof(float));
		memcpy(&smLightCol[smLightCount * 4], &lLightCol[lIndex], 4 * sizeof(float));

		lIndex = (int)Renderer::LIGHT_SPOT * MAX_SHADER_LIGHTS + i;
		smLightCut[smLightCount] = lLightCut[lIndex];
		smLightExp[smLightCount] = lLightExp[lIndex];

		smLightCount++;
	}
#endif // !GLES
}

void OpenGLMatPXS::PrepareShaderPrograms(OpenGLRenderer* /*pRenderer*/)
{
#ifndef LEPRA_GL_ES
	// Lookup which fragment shader to use for this combination of lights.
	int lSelectedFP = smFPLUT[smNumDirLights][smNumPntLights][smNumSptLights];

	glEnable(GL_VERTEX_PROGRAM_ARB);
	glEnable(GL_FRAGMENT_PROGRAM_ARB);

	UiLepra::OpenGLExtensions::glBindProgramARB(GL_VERTEX_PROGRAM_ARB, mVPID);
	UiLepra::OpenGLExtensions::glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, mFPID[lSelectedFP]);

	for (int i = 0; i < smLightCount; i++)
	{
		UiLepra::OpenGLExtensions::glProgramLocalParameter4fvARB(GL_VERTEX_PROGRAM_ARB, i,  &smLightPos[i * 4]);
		UiLepra::OpenGLExtensions::glProgramLocalParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB, i + 4,  &smLightPos[i * 4]);
		UiLepra::OpenGLExtensions::glProgramLocalParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB, i + 7,  &smLightDir[i * 4]);
		UiLepra::OpenGLExtensions::glProgramLocalParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB, i + 10, &smLightCol[i * 4]);
		// One extra copy to the vertex shader. Used in bump mapping for instance.
		UiLepra::OpenGLExtensions::glProgramLocalParameter4fvARB(GL_VERTEX_PROGRAM_ARB, i + 3,  &smLightDir[i * 4]);
	}

	// Set the cutoff angle and spot exponent.
	UiLepra::OpenGLExtensions::glProgramLocalParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB, 1, smLightCut);
	UiLepra::OpenGLExtensions::glProgramLocalParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB, 2, smLightExp);
#endif // !GLES
}

void OpenGLMatPXS::CleanupShaderPrograms()
{
#ifndef LEPRA_GL_ES
	glDisable(GL_VERTEX_PROGRAM_ARB);
	glDisable(GL_FRAGMENT_PROGRAM_ARB);
#endif // !GLES
}

void OpenGLMatPXS::SetAmbientLight(OpenGLRenderer* pRenderer, TBC::GeometryBase* pGeometry)
{
#ifndef LEPRA_GL_ES
	float r, g, b;
	pRenderer->GetAmbientLight(r, g, b);

	const TBC::GeometryBase::BasicMaterialSettings& lMat =
		pGeometry->GetBasicMaterialSettings();

	r += lMat.mAmbient.x;
	g += lMat.mAmbient.y;
	b += lMat.mAmbient.z;

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
	GLfloat lLightAmbient[4];
	lLightAmbient[0] = (GLfloat)r;
	lLightAmbient[1] = (GLfloat)g;
	lLightAmbient[2] = (GLfloat)b;
	lLightAmbient[3] = 0;
	UiLepra::OpenGLExtensions::glProgramLocalParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB, 3, lLightAmbient);
#endif // !GLES
}

//
// OpenGLMatSingleColorSolidPXS
//

OpenGLMatSingleColorSolidPXS::OpenGLMatSingleColorSolidPXS(OpenGLRenderer* pRenderer, Material* pFallBackMaterial):
	OpenGLMatSingleColorSolid(pRenderer, pFallBackMaterial),
	OpenGLMatPXS(smVP, smFP)
{
}

OpenGLMatSingleColorSolidPXS::~OpenGLMatSingleColorSolidPXS()
{
#ifndef LEPRA_GL_ES
	if (UiLepra::OpenGLExtensions::IsShaderAsmProgramsSupported() == true)
	{
		UiLepra::OpenGLExtensions::glDeleteProgramsARB(1, (const GLuint*)&mVPID);
		UiLepra::OpenGLExtensions::glDeleteProgramsARB(NUM_FP, (const GLuint*)mFPID);
	}
#endif // !GLES
}

void OpenGLMatSingleColorSolidPXS::DoRenderAllGeometry(unsigned pCurrentFrame, const GeometryGroupList& pGeometryGroupList)
{
	if (!GetRenderer()->IsPixelShadersEnabled() || !UiLepra::OpenGLExtensions::IsShaderAsmProgramsSupported())
	{
		Parent::DoRenderAllGeometry(pCurrentFrame, pGeometryGroupList);
		return;
	}

	OpenGLMatPXS::PrepareShaderPrograms((OpenGLRenderer*)GetRenderer());
	Parent::DoRenderAllGeometry(pCurrentFrame, pGeometryGroupList);
	OpenGLMatPXS::CleanupShaderPrograms();
}

void OpenGLMatSingleColorSolidPXS::RenderGeometry(TBC::GeometryBase* pGeometry)
{
	if (!GetRenderer()->IsPixelShadersEnabled() || !UiLepra::OpenGLExtensions::IsShaderAsmProgramsSupported())
	{
		Parent::RenderGeometry(pGeometry);
		return;
	}

	SetBasicMaterial(pGeometry->GetBasicMaterialSettings());
	OpenGLMatPXS::SetAmbientLight((OpenGLRenderer*)GetRenderer(), pGeometry);

#ifndef LEPRA_GL_ES
	float lSpecular[4];
	const TBC::GeometryBase::BasicMaterialSettings& lMatSettings = pGeometry->GetBasicMaterialSettings();
	lSpecular[0] = lMatSettings.mShininess;
	lSpecular[1] = lMatSettings.mShininess;
	lSpecular[2] = lMatSettings.mShininess;
	lSpecular[3] = lMatSettings.mShininess;
	UiLepra::OpenGLExtensions::glProgramLocalParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB, 0, lSpecular);
#endif // !GLES

	Parent::RawRender(pGeometry, 0);

	ResetBasicMaterial(pGeometry->GetBasicMaterialSettings());
}




//
// OpenGLMatSingleTextureSolidPXS
//

OpenGLMatSingleTextureSolidPXS::OpenGLMatSingleTextureSolidPXS(OpenGLRenderer* pRenderer,
							   Material* pFallBackMaterial) :
	OpenGLMatSingleTextureSolid(pRenderer, pFallBackMaterial),
	OpenGLMatPXS(smVP, smFP)
{
}

OpenGLMatSingleTextureSolidPXS::~OpenGLMatSingleTextureSolidPXS()
{
#ifndef LEPRA_GL_ES
	if (UiLepra::OpenGLExtensions::IsShaderAsmProgramsSupported())
	{
		UiLepra::OpenGLExtensions::glDeleteProgramsARB(1, (const GLuint*)&mVPID);
		UiLepra::OpenGLExtensions::glDeleteProgramsARB(NUM_FP, (const GLuint*)mFPID);
	}
#endif // !GLES
}

void OpenGLMatSingleTextureSolidPXS::DoRenderAllGeometry(unsigned pCurrentFrame, const GeometryGroupList& pGeometryGroupList)
{
	// Early bail out if no multitexturing support. TODO: fix in cards that don't support!
	if (!UiLepra::OpenGLExtensions::IsMultiTextureSupported())
	{
		Parent::DoRenderAllGeometry(pCurrentFrame, pGeometryGroupList);
		return;
	}

	OpenGLMatPXS::PrepareShaderPrograms((OpenGLRenderer*)GetRenderer());
	Parent::DoRenderAllGeometry(pCurrentFrame, pGeometryGroupList);
	OpenGLMatPXS::CleanupShaderPrograms();
}

void OpenGLMatSingleTextureSolidPXS::RenderGeometry(TBC::GeometryBase* pGeometry)
{
	if (!GetRenderer()->IsPixelShadersEnabled() || !UiLepra::OpenGLExtensions::IsShaderAsmProgramsSupported())
	{
		Parent::RenderGeometry(pGeometry);
		return;
	}

	OpenGLRenderer::OGLGeometryData* lGeometry = (OpenGLRenderer::OGLGeometryData*)pGeometry->GetRendererData();
	SetBasicMaterial(pGeometry->GetBasicMaterialSettings());
	OpenGLMatPXS::SetAmbientLight((OpenGLRenderer*)GetRenderer(), pGeometry);

	UiLepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE0);
	UiLepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE0);
	//glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	//glEnable(GL_TEXTURE_2D);
#ifndef LEPRA_GL_ES
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
#endif // !GLES

	BindTexture(lGeometry->mTA->mMaps[0].mMapID[Texture::COLOR_MAP], lGeometry->mTA->mMaps[0].mMipMapLevelCount[Texture::COLOR_MAP]);

	OpenGLMaterial::UpdateTextureMatrix(lGeometry->mGeometry);
	
#ifndef LEPRA_GL_ES
	float lSpecular[4];
	const TBC::GeometryBase::BasicMaterialSettings& lMatSettings = pGeometry->GetBasicMaterialSettings();
	lSpecular[0] = lMatSettings.mShininess;
	lSpecular[1] = lMatSettings.mShininess;
	lSpecular[2] = lMatSettings.mShininess;
	lSpecular[3] = lMatSettings.mShininess;
	UiLepra::OpenGLExtensions::glProgramLocalParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB, 0, lSpecular);
#endif // !GLES

	Parent::RawRender(pGeometry, 0);

	ResetBasicMaterial(pGeometry->GetBasicMaterialSettings());
}



//
// OpenGLMatTextureAndLightmapPXS
//

OpenGLMatTextureAndLightmapPXS::OpenGLMatTextureAndLightmapPXS(OpenGLRenderer* pRenderer,
							       Material* pFallBackMaterial) :
	Parent(pRenderer, pFallBackMaterial),
	OpenGLMatPXS(smVP, smFP)
{
}

OpenGLMatTextureAndLightmapPXS::~OpenGLMatTextureAndLightmapPXS()
{
#ifndef LEPRA_GL_ES
	if (UiLepra::OpenGLExtensions::IsShaderAsmProgramsSupported())
	{
		UiLepra::OpenGLExtensions::glDeleteProgramsARB(1, (const GLuint*)&mVPID);
		UiLepra::OpenGLExtensions::glDeleteProgramsARB(NUM_FP, (const GLuint*)mFPID);
	}
#endif // !GLES
}

void OpenGLMatTextureAndLightmapPXS::DoRenderAllGeometry(unsigned pCurrentFrame, const GeometryGroupList& pGeometryGroupList)
{
	// Early bail out if no multitexturing support. TODO: fix in cards that don't support!
	if (!UiLepra::OpenGLExtensions::IsMultiTextureSupported())
	{
		Parent::DoRenderAllGeometry(pCurrentFrame, pGeometryGroupList);
		return;
	}

	UiLepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE0);
	UiLepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);

	UiLepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE1);
	UiLepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE1);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);

	OpenGLMatPXS::PrepareShaderPrograms((OpenGLRenderer*)GetRenderer());
	Parent::DoRenderAllGeometry(pCurrentFrame, pGeometryGroupList);
	OpenGLMatPXS::CleanupShaderPrograms();

	UiLepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE1);
	UiLepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE1);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_TEXTURE_2D);

	UiLepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE0);
	UiLepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE0);
}

void OpenGLMatTextureAndLightmapPXS::RenderGeometry(TBC::GeometryBase* pGeometry)
{
	if (!GetRenderer()->IsPixelShadersEnabled() || !UiLepra::OpenGLExtensions::IsShaderAsmProgramsSupported())
	{
		Parent::RenderGeometry(pGeometry);
		return;
	}

	SetBasicMaterial(pGeometry->GetBasicMaterialSettings());
	OpenGLMatPXS::SetAmbientLight((OpenGLRenderer*)GetRenderer(), pGeometry);

	RawRender(pGeometry, 0);

	ResetBasicMaterial(pGeometry->GetBasicMaterialSettings());
}

void OpenGLMatTextureAndLightmapPXS::RawRender(TBC::GeometryBase* pGeometry, int pUVSetIndex)
{
	UiLepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE0);
	UiLepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE0);
	OpenGLRenderer::OGLGeometryData* lGeometry = (OpenGLRenderer::OGLGeometryData*)pGeometry->GetRendererData();
	BindTexture(lGeometry->mTA->mMaps[0].mMapID[Texture::COLOR_MAP], lGeometry->mTA->mMaps[0].mMipMapLevelCount[Texture::COLOR_MAP]);

	if (UiLepra::OpenGLExtensions::IsBufferObjectsSupported() == true)
	{
		GLuint lIndexBufferID  = (GLuint)lGeometry->mIndexBufferID;
		UiLepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, (GLuint)lGeometry->mVertexBufferID);
		glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)(lGeometry->mUVOffset + sizeof(float)*2*pUVSetIndex*pGeometry->GetMaxVertexCount()));

		UiLepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lIndexBufferID);
	}
	else
	{
		glTexCoordPointer(2, GL_FLOAT, 0, pGeometry->GetUVData(pUVSetIndex));
	}
	OpenGLMaterial::UpdateTextureMatrix(lGeometry->mGeometry);

	UiLepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE1);
	UiLepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE1);
	BindTexture(lGeometry->mTA->mMaps[1].mMapID[Texture::COLOR_MAP], lGeometry->mTA->mMaps[1].mMipMapLevelCount[Texture::COLOR_MAP]);

	if (UiLepra::OpenGLExtensions::IsBufferObjectsSupported() == true)
	{
		GLuint lIndexBufferID  = (GLuint)lGeometry->mIndexBufferID;
		UiLepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, (GLuint)lGeometry->mVertexBufferID);
		glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)(lGeometry->mUVOffset + sizeof(float)*2*(pUVSetIndex+1)*pGeometry->GetMaxVertexCount()));

		UiLepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lIndexBufferID);
	}
	else
	{
		glTexCoordPointer(2, GL_FLOAT, 0, pGeometry->GetUVData(pUVSetIndex+1));
	}

#ifndef LEPRA_GL_ES
	float lSpecular[4];
	const TBC::GeometryBase::BasicMaterialSettings& lMatSettings = pGeometry->GetBasicMaterialSettings();
	lSpecular[0] = lMatSettings.mShininess;
	lSpecular[1] = lMatSettings.mShininess;
	lSpecular[2] = lMatSettings.mShininess;
	lSpecular[3] = lMatSettings.mShininess;
	UiLepra::OpenGLExtensions::glProgramLocalParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB, 0, lSpecular);
#endif // !GLES

	if (UiLepra::OpenGLExtensions::IsBufferObjectsSupported() == true)
	{
		glVertexPointer(3, GL_FLOAT, 0, 0);
		glNormalPointer(GL_FLOAT, 0, (GLvoid*)lGeometry->mNormalOffset);

		glDrawElements(OpenGLMaterial::GetGLElementType(pGeometry), 
			       pGeometry->GetIndexCount(),
			       LEPRA_GL_INDEX_TYPE,
			       0);
	}
	else
	{
		glVertexPointer(3, GL_FLOAT, 0, pGeometry->GetVertexData());
		glNormalPointer(GL_FLOAT, 0, pGeometry->GetNormalData());
		glDrawElements(OpenGLMaterial::GetGLElementType(pGeometry),
			       pGeometry->GetIndexCount(),
			       LEPRA_GL_INDEX_TYPE,
			       pGeometry->GetIndexData());
	}
}



//
// OpenGLMatTextureSBMapPXS
//

OpenGLMatTextureSBMapPXS::OpenGLMatTextureSBMapPXS(OpenGLRenderer* pRenderer,
						   Material* pFallBackMaterial,
						   const astr pVP,
						   const astr* pFP) :
	OpenGLMatSingleTextureSolid(pRenderer, pFallBackMaterial),
	OpenGLMatPXS(pVP, pFP)
{
}

OpenGLMatTextureSBMapPXS::OpenGLMatTextureSBMapPXS(OpenGLRenderer* pRenderer,
						   Material* pFallBackMaterial) :
	OpenGLMatSingleTextureSolid(pRenderer, pFallBackMaterial),
	OpenGLMatPXS(smVP, smFP)
{
}

OpenGLMatTextureSBMapPXS::~OpenGLMatTextureSBMapPXS()
{
#ifndef LEPRA_GL_ES
	if (UiLepra::OpenGLExtensions::IsShaderAsmProgramsSupported())
	{
		UiLepra::OpenGLExtensions::glDeleteProgramsARB(1, (const GLuint*)&mVPID);
		UiLepra::OpenGLExtensions::glDeleteProgramsARB(NUM_FP, (const GLuint*)mFPID);
	}
#endif // !GLES
}

void OpenGLMatTextureSBMapPXS::DoRenderAllGeometry(unsigned pCurrentFrame, const GeometryGroupList& pGeometryGroupList)
{
	// Early bail out if no multitexturing support. TODO: fix in cards that don't support!
	if (!UiLepra::OpenGLExtensions::IsMultiTextureSupported())
	{
		return;
	}

	UiLepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE0);
	UiLepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);

	UiLepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE1);
	UiLepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE1);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);

	UiLepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE2);
	UiLepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE2);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);

	OpenGLMatPXS::PrepareShaderPrograms((OpenGLRenderer*)GetRenderer());
	Parent::DoRenderAllGeometry(pCurrentFrame, pGeometryGroupList);
	OpenGLMatPXS::CleanupShaderPrograms();

	UiLepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE2);
	UiLepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE2);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_TEXTURE_2D);

	UiLepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE1);
	UiLepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE1);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_TEXTURE_2D);

	UiLepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE0);
	UiLepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE0);
}

void OpenGLMatTextureSBMapPXS::RenderGeometry(TBC::GeometryBase* pGeometry)
{
	if (!GetRenderer()->IsPixelShadersEnabled() || !UiLepra::OpenGLExtensions::IsShaderAsmProgramsSupported())
	{
		Parent::RenderGeometry(pGeometry);
		return;
	}

	SetBasicMaterial(pGeometry->GetBasicMaterialSettings());
	OpenGLMatPXS::SetAmbientLight((OpenGLRenderer*)GetRenderer(), pGeometry);

	RawRender(pGeometry, 0);

	ResetBasicMaterial(pGeometry->GetBasicMaterialSettings());
}

void OpenGLMatTextureSBMapPXS::RawRender(TBC::GeometryBase* pGeometry, int pUVSetIndex)
{
	OpenGLRenderer::OGLGeometryData* lGeometry = (OpenGLRenderer::OGLGeometryData*)pGeometry->GetRendererData();

	// Texture unit 0, handles color map and regular 
	// texture (UV) coordinates.
	UiLepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE0);
	UiLepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE0);
	BindTexture(lGeometry->mTA->mMaps[0].mMapID[Texture::COLOR_MAP], lGeometry->mTA->mMaps[0].mMipMapLevelCount[Texture::COLOR_MAP]);

	if (UiLepra::OpenGLExtensions::IsBufferObjectsSupported() == true)
	{
		GLuint lIndexBufferID  = (GLuint)lGeometry->mIndexBufferID;
		UiLepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, (GLuint)lGeometry->mVertexBufferID);
		glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)(lGeometry->mUVOffset + sizeof(float)*2*pUVSetIndex*pGeometry->GetMaxVertexCount()));

		UiLepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lIndexBufferID);
	}
	else
	{
		glTexCoordPointer(2, GL_FLOAT, 0, pGeometry->GetUVData(pUVSetIndex));
	}

	OpenGLMaterial::UpdateTextureMatrix(lGeometry->mGeometry);

	// Texture unit 1, handles specular map and tangents.
	UiLepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE1);
	UiLepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE1);
	BindTexture(lGeometry->mTA->mMaps[0].mMapID[Texture::SPECULAR_MAP], lGeometry->mTA->mMaps[0].mMipMapLevelCount[Texture::SPECULAR_MAP]);

	if (UiLepra::OpenGLExtensions::IsBufferObjectsSupported() == true)
	{
		GLuint lIndexBufferID  = (GLuint)lGeometry->mIndexBufferID;
		UiLepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, (GLuint)lGeometry->mVertexBufferID);
		glTexCoordPointer(3, GL_FLOAT, 0, (GLvoid*)lGeometry->mTangentOffset);

		UiLepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lIndexBufferID);
	}
	else
	{
		glTexCoordPointer(3, GL_FLOAT, 0, pGeometry->GetTangentData());
	}

	// Texture unit 2, handles bitangents and normal map.
	UiLepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE2);
	UiLepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE2);
	BindTexture(lGeometry->mTA->mMaps[0].mMapID[Texture::NORMAL_MAP], lGeometry->mTA->mMaps[0].mMipMapLevelCount[Texture::NORMAL_MAP]);
	if (UiLepra::OpenGLExtensions::IsBufferObjectsSupported() == true)
	{
		GLuint lIndexBufferID  = (GLuint)lGeometry->mIndexBufferID;
		UiLepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, (GLuint)lGeometry->mVertexBufferID);
		glTexCoordPointer(3, GL_FLOAT, 0, (GLvoid*)lGeometry->mBitangentOffset);

		UiLepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lIndexBufferID);
	}
	else
	{
		glTexCoordPointer(3, GL_FLOAT, 0, pGeometry->GetBitangentData());
	}

#ifndef LEPRA_GL_ES
	float lSpecular[4];
	const TBC::GeometryBase::BasicMaterialSettings& lMatSettings = pGeometry->GetBasicMaterialSettings();
	lSpecular[0] = lMatSettings.mShininess;
	lSpecular[1] = lMatSettings.mShininess;
	lSpecular[2] = lMatSettings.mShininess;
	lSpecular[3] = lMatSettings.mShininess;
	UiLepra::OpenGLExtensions::glProgramLocalParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB, 0, lSpecular);
#endif // !GLES

	if (UiLepra::OpenGLExtensions::IsBufferObjectsSupported() == true)
	{
		glVertexPointer(3, GL_FLOAT, 0, 0);
		glNormalPointer(GL_FLOAT, 0, (GLvoid*)lGeometry->mNormalOffset);

		glDrawElements(OpenGLMaterial::GetGLElementType(pGeometry), 
			       pGeometry->GetIndexCount(),
			       LEPRA_GL_INDEX_TYPE,
			       0);
	}
	else
	{
		glVertexPointer(3, GL_FLOAT, 0, pGeometry->GetVertexData());
		glNormalPointer(GL_FLOAT, 0, pGeometry->GetNormalData());
		glDrawElements(OpenGLMaterial::GetGLElementType(pGeometry),
			       pGeometry->GetIndexCount(),
			       LEPRA_GL_INDEX_TYPE,
			       pGeometry->GetIndexData());
	}
}

OpenGLMatTextureDiffuseBumpMapPXS::OpenGLMatTextureDiffuseBumpMapPXS(OpenGLRenderer* pRenderer,
								     Material* pFallBackMaterial) :
	OpenGLMatTextureSBMapPXS(pRenderer, pFallBackMaterial, smVP, smFP)
{
}

OpenGLMatTextureDiffuseBumpMapPXS::~OpenGLMatTextureDiffuseBumpMapPXS()
{
}



LOG_CLASS_DEFINE(UI_GFX_3D, OpenGLMatVertexColorSolid);
LOG_CLASS_DEFINE(UI_GFX_3D, OpenGLMatSingleTextureSolid);
LOG_CLASS_DEFINE(UI_GFX_3D, OpenGLMatSingleColorEnvMapSolid);
LOG_CLASS_DEFINE(UI_GFX_3D, OpenGLMatSingleTextureEnvMapSolid);
LOG_CLASS_DEFINE(UI_GFX_3D, OpenGLMatTextureAndLightmap);
LOG_CLASS_DEFINE(UI_GFX_3D, OpenGLMatPXS);
LOG_CLASS_DEFINE(UI_GFX_3D, OpenGLMatSingleColorSolidPXS);
LOG_CLASS_DEFINE(UI_GFX_3D, OpenGLMatSingleTextureSolidPXS);
LOG_CLASS_DEFINE(UI_GFX_3D, OpenGLMatTextureAndLightmapPXS);
LOG_CLASS_DEFINE(UI_GFX_3D, OpenGLMatTextureSBMapPXS);



}
