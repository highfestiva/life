
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

#include "pch.h"
#include "../Include/UiOpenGLRenderer.h"
#include "../../Lepra/Include/Canvas.h"
#include "../../Lepra/Include/Log.h"
#include "../../Lepra/Include/Math.h"
#include "../../Lepra/Include/Performance.h"
#include "../../Lepra/Include/Transformation.h"
#include "../../Tbc/Include/GeometryReference.h"
#include "../../UiLepra/Include/UiOpenGLExtensions.h"
#include "../Include/UiDynamicRenderer.h"
#include "../Include/UiOpenGLMaterials.h"
#include "../Include/UiTexture.h"



namespace UiTbc
{



#ifdef LEPRA_DEBUG
#define OGL_ASSERT()		{ GLenum lGlError = glGetError(); deb_assert(lGlError == GL_NO_ERROR); }
#define OGL_FAST_ASSERT()	OGL_ASSERT();
#else // !Debug
#define OGL_ASSERT()
#define OGL_FAST_ASSERT()
#endif // Debug / !Debug



Material* OpenGLRenderer::CreateMaterial(MaterialType pMaterialType)
{
	switch(pMaterialType)
	{
	case MAT_NULL:
		return new NullMaterial(this);
	case MAT_SINGLE_COLOR_SOLID:
		return new OpenGLMatSingleColorSolid(this, 0);
	case MAT_SINGLE_COLOR_SOLID_PXS:
		return new OpenGLMatSingleColorSolidPXS(this, GetMaterial(MAT_SINGLE_COLOR_SOLID));
	case MAT_VERTEX_COLOR_SOLID:
		return new OpenGLMatVertexColorSolid(this, GetMaterial(MAT_SINGLE_COLOR_SOLID));
	case MAT_SINGLE_TEXTURE_SOLID:
		return new OpenGLMatSingleTextureSolid(this, GetMaterial(MAT_SINGLE_COLOR_SOLID));
	case MAT_SINGLE_TEXTURE_HIGHLIGHT:
		return new OpenGLMatSingleTextureHighlight(this, GetMaterial(MAT_SINGLE_TEXTURE_SOLID));
	case MAT_SINGLE_TEXTURE_SOLID_PXS:
		return new OpenGLMatSingleTextureSolidPXS(this, GetMaterial(MAT_SINGLE_TEXTURE_SOLID));
	case MAT_SINGLE_COLOR_ENVMAP_SOLID:
		return new OpenGLMatSingleColorEnvMapSolid(this, GetMaterial(MAT_SINGLE_COLOR_SOLID));
	case MAT_SINGLE_TEXTURE_ENVMAP_SOLID:
		return new OpenGLMatSingleTextureEnvMapSolid(this, GetMaterial(MAT_SINGLE_TEXTURE_SOLID));
	case MAT_TEXTURE_AND_LIGHTMAP:
		return new OpenGLMatTextureAndLightmap(this, GetMaterial(MAT_SINGLE_TEXTURE_SOLID));
	case MAT_TEXTURE_AND_LIGHTMAP_PXS:
		return new OpenGLMatTextureAndLightmapPXS(this, GetMaterial(MAT_TEXTURE_AND_LIGHTMAP));
	case MAT_TEXTURE_AND_DIFFUSE_BUMPMAP_PXS:
		return new OpenGLMatTextureDiffuseBumpMapPXS(this, GetMaterial(MAT_SINGLE_TEXTURE_SOLID_PXS));
	case MAT_TEXTURE_SBMAP_PXS:
		return new OpenGLMatTextureSBMapPXS(this, GetMaterial(MAT_TEXTURE_AND_DIFFUSE_BUMPMAP_PXS));
	case MAT_SINGLE_COLOR_BLENDED:
		return new OpenGLMatSingleColorBlended(this, 0, false);
	case MAT_SINGLE_COLOR_OUTLINE_BLENDED:
		return new OpenGLMatSingleColorBlended(this, 0, true);
	case MAT_VERTEX_COLOR_BLENDED:
		return new OpenGLMatVertexColorBlended(this, GetMaterial(MAT_SINGLE_COLOR_BLENDED));
	case MAT_SINGLE_TEXTURE_BLENDED:
		return new OpenGLMatSingleTextureBlended(this, GetMaterial(MAT_SINGLE_COLOR_BLENDED));
	case MAT_SINGLE_TEXTURE_ALPHATESTED:
		return new OpenGLMatSingleTextureAlphaTested(this, GetMaterial(MAT_SINGLE_COLOR_BLENDED));
	case MAT_SINGLE_COLOR_ENVMAP_BLENDED:
		return new OpenGLMatSingleColorEnvMapBlended(this, GetMaterial(MAT_SINGLE_COLOR_BLENDED));
	case MAT_SINGLE_TEXTURE_ENVMAP_BLENDED:
		return new OpenGLMatSingleTextureEnvMapBlended(this, GetMaterial(MAT_SINGLE_TEXTURE_BLENDED));
	default:
		return 0;
	}
}



OpenGLRenderer::OpenGLRenderer(Canvas* pScreen) :
	Renderer(pScreen),
	mBufferIDManager(1, 1000000, 0),
	mTMapIDManager(10001, 1000000, INVALID_TEXTURE),	// 1-10000 is reserved by Painter.
	mGLClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
{
	OGL_ASSERT();

	InitRenderer();

	float r, g, b;
	GetAmbientLight(r, g, b);
	SetAmbientLight(r, g, b);

	glGetIntegerv(GL_MAX_TEXTURE_UNITS, &mNumTextureUnits);
	glGetError();	// If we have a weak card, let's do our best with funny looks.
	glDepthFunc(GL_LEQUAL);
	glClearStencil(128);
	OGL_ASSERT();
}



OpenGLRenderer::~OpenGLRenderer()
{
	CloseRenderer();
}

void OpenGLRenderer::Clear(unsigned pClearFlags)
{
	OGL_FAST_ASSERT();

	mGLClearMask = 0;

	if (CheckFlag(pClearFlags, CLEAR_COLORBUFFER))
	{
		mGLClearMask |= GL_COLOR_BUFFER_BIT;
	}
	if (CheckFlag(pClearFlags, CLEAR_DEPTHBUFFER))
	{
		mGLClearMask |= GL_DEPTH_BUFFER_BIT;
	}
	if (CheckFlag(pClearFlags, CLEAR_STENCILBUFFER))
	{
		mGLClearMask |= GL_STENCIL_BUFFER_BIT;
	}
#ifndef LEPRA_GL_ES
	if (CheckFlag(pClearFlags, CLEAR_ACCUMULATIONBUFFER))
	{
		mGLClearMask |= GL_ACCUM_BUFFER_BIT;
	}
#endif // !GLES

	if (GetShadowMode() != UiTbc::Renderer::NO_SHADOWS)
	{
		// Always clear the stencil buffer if shadows are activated.
		mGLClearMask |= GL_STENCIL_BUFFER_BIT;
	}

	glClear(mGLClearMask);

	OGL_ASSERT();
}

void OpenGLRenderer::SetClearColor(const Color& pColor)
{
	Parent::SetClearColor(pColor);
	::glClearColor(pColor.GetRf(), pColor.GetGf(), pColor.GetBf(), 1.0f);
	OGL_FAST_ASSERT();
}

bool OpenGLRenderer::IsPixelShadersEnabled() const
{
	return UiLepra::OpenGLExtensions::IsShaderAsmProgramsSupported() && Parent::IsPixelShadersEnabled();
}

void OpenGLRenderer::SetViewFrustum(float pFOVAngle, float pNear, float pFar)
{
	Parent::SetViewFrustum(pFOVAngle, pNear, pFar);
	Perspective(pFOVAngle, GetAspectRatio(), pNear, pFar);
	OGL_FAST_ASSERT();
}

void OpenGLRenderer::SetShadowMode(Shadows pShadowMode, ShadowHint pHint)
{
	Parent::SetShadowMode(pShadowMode, UiLepra::OpenGLExtensions::IsShadowMapsSupported()? pHint : SH_VOLUMES_ONLY);
}

void OpenGLRenderer::SetDepthWriteEnabled(bool pEnabled)
{
	if (pEnabled == true)
		glDepthMask(GL_TRUE);
	else
		glDepthMask(GL_FALSE);
	OGL_FAST_ASSERT();
}

void OpenGLRenderer::SetDepthTestEnabled(bool pEnabled)
{
	if (pEnabled == true)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
	OGL_FAST_ASSERT();
}

void OpenGLRenderer::SetLightsEnabled(bool pEnabled)
{
	Parent::SetLightsEnabled(pEnabled);
	if (pEnabled)
	{
		::glEnable(GL_LIGHTING);
	}
	else
	{
		::glDisable(GL_LIGHTING);
	}
}

void OpenGLRenderer::SetTexturingEnabled(bool pEnabled)
{
	Parent::SetTexturingEnabled(pEnabled);
	if (pEnabled)
	{
		::glEnable(GL_TEXTURE_2D);
		::glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	}
	else
	{
		::glDisable(GL_TEXTURE_2D);
		::glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
}

void OpenGLRenderer::SetLineWidth(float pPixels)
{
	::glLineWidth(pPixels);
}

void OpenGLRenderer::SetAmbientLight(float pRed, float pGreen, float pBlue)
{
	Parent::SetAmbientLight(pRed, pGreen, pBlue);

	float lAmbientLight[] = {pRed, pGreen, pBlue, 1.0f};
	::glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lAmbientLight);
	::glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, lAmbientLight);
#ifndef LEPRA_GL_ES
	if (IsPixelShadersEnabled())
	{
		GLfloat lLightAmbient[4] = {0,0,0,0};
		UiLepra::OpenGLExtensions::glProgramLocalParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB, 3, lLightAmbient);
	}
#endif // !OpenGL ES
	OGL_FAST_ASSERT();
}

void OpenGLRenderer::AddAmbience(float pRed, float pGreen, float pBlue)
{
	if (pRed != 0 || pGreen != 0 || pBlue != 0)
	{
		Parent::AddAmbience(pRed, pGreen, pBlue);
		float lAmbientLight[4];
		GetAmbientLight(lAmbientLight[0], lAmbientLight[1], lAmbientLight[2]);
		lAmbientLight[3] = 1.0f;
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lAmbientLight);
	}
	OGL_FAST_ASSERT();
}

void OpenGLRenderer::DoSetClippingRect(const PixelRect& pRect)
{
	::glEnable(GL_SCISSOR_TEST);
	::glScissor(pRect.mLeft, GetScreen()->GetActualHeight() - pRect.mBottom, pRect.GetWidth(), pRect.GetHeight());
	OGL_FAST_ASSERT();
}

void OpenGLRenderer::DoSetViewport(const PixelRect& pViewport)
{
	OGL_FAST_ASSERT();
	::glViewport(pViewport.mLeft, GetScreen()->GetActualHeight() - pViewport.mBottom, 
		pViewport.GetWidth(), pViewport.GetHeight());
	OGL_FAST_ASSERT();
}

int OpenGLRenderer::ReleaseShadowMap(int pShadowMapID)
{
	if (pShadowMapID != mTMapIDManager.GetInvalidId())
	{
		mTMapIDManager.RecycleId(pShadowMapID);
		GLuint lShadowMapID = (GLuint)pShadowMapID;
		glDeleteTextures(1, &lShadowMapID);
	}
	//OGL_FAST_ASSERT();
	return mTMapIDManager.GetInvalidId();
}




Renderer::LightID OpenGLRenderer::AddDirectionalLight(LightHint pHint,
		const vec3& pDir,
		const vec3& pColor,
		float pShadowRange)
{
	LightID lLightID = Parent::AddDirectionalLight(pHint, pDir, pColor, pShadowRange);
	SetupGLLight((int)lLightID, GetLightData((int)lLightID));
	OGL_FAST_ASSERT();
	return lLightID;
}

Renderer::LightID OpenGLRenderer::AddPointLight(LightHint pHint,
		const vec3& pPos,
		const vec3& pColor,
		float pLightRadius,
		float pShadowRange)
{
	LightID lLightID = Parent::AddPointLight(pHint, pPos, pColor, pLightRadius, pShadowRange);
	SetupGLLight((int)lLightID, GetLightData((int)lLightID));
	OGL_FAST_ASSERT();
	return lLightID;
}

Renderer::LightID OpenGLRenderer::AddSpotLight(LightHint pHint,
		const vec3& pPos,
		const vec3& pDir,
		const vec3& pColor,
		float pCutoffAngle,
		float pSpotExponent,
		float pLightRadius,
		float pShadowRange)
{
	LightID lLightID = Parent::AddSpotLight(pHint, 
		pPos,
		pDir,
		pColor,
		pCutoffAngle, 
		pSpotExponent, 
		pLightRadius, 
		pShadowRange);
	SetupGLLight((int)lLightID, GetLightData((int)lLightID));
	OGL_FAST_ASSERT();
	return lLightID;
}

void OpenGLRenderer::SetupGLLight(int pLightIndex, const LightData* pLight)
{
	GLenum lLight = GL_LIGHT0 + pLightIndex;
	glEnable(lLight);

	if (pLight->mType != LIGHT_SPOT)
	{
		float l180 = 180.0f;
		glLightf(lLight, GL_SPOT_CUTOFF, l180);

		float lDir[3] = {0, 0, -1};
		glLightfv(lLight, GL_SPOT_DIRECTION, lDir);
	}

	float lPos[4];
	if (pLight->mType == LIGHT_POINT ||
	   pLight->mType == LIGHT_SPOT)
	{
		vec3 lLightPos = GetCameraActualTransformation().InverseTransform(pLight->mPosition);
		lPos[0] = (float)lLightPos.x;
		lPos[1] = (float)lLightPos.y;
		lPos[2] = (float)lLightPos.z;
		lPos[3] = 1.0f;

		if (pLight->mType == LIGHT_SPOT)
		{
			vec3 lLightDir = GetCameraActualOrientationInverse() * pLight->mDirection;

			float lDir[3];
			lDir[0] = (float)lLightDir.x;
			lDir[1] = (float)lLightDir.y;
			lDir[2] = (float)lLightDir.z;

			glLightfv(lLight, GL_SPOT_DIRECTION, lDir);
			glLightfv(lLight, GL_SPOT_CUTOFF, &pLight->mCutoffAngle);
		}

		float lConst = 0.0f;
		float lLinear = 0.0f;
		float lQuad = 1.0f;
		glLightfv(lLight, GL_CONSTANT_ATTENUATION, &lConst);
		glLightfv(lLight, GL_LINEAR_ATTENUATION, &lLinear);
		glLightfv(lLight, GL_QUADRATIC_ATTENUATION, &lQuad);
	}
	else if(pLight->mType == LIGHT_DIRECTIONAL)
	{
		vec3 lLightDir = GetCameraActualOrientationInverse() * pLight->mDirection;
		lPos[0] = -(float)lLightDir.x;
		lPos[1] = -(float)lLightDir.y;
		lPos[2] = -(float)lLightDir.z;
		lPos[3] = 0.0f;

		float lConst = 1.0f;
		float lLinear = 0.0f;
		float lQuad = 0.0f;
		glLightfv(lLight, GL_CONSTANT_ATTENUATION, &lConst);
		glLightfv(lLight, GL_LINEAR_ATTENUATION, &lLinear);
		glLightfv(lLight, GL_QUADRATIC_ATTENUATION, &lQuad);
	}

	float lBlack[] = {0, 0, 0, 1.0f};

	glLightfv(lLight, GL_POSITION, lPos);
	glLightfv(lLight, GL_AMBIENT, lBlack);
	glLightfv(lLight, GL_DIFFUSE, pLight->mColor);
	glLightfv(lLight, GL_SPECULAR, lBlack);
	OGL_FAST_ASSERT();
}

void OpenGLRenderer::RemoveLight(LightID pLightID)
{
	Parent::RemoveLight(pLightID);
	GLenum lLight = (int)pLightID;
	glDisable(GL_LIGHT0+lLight);
	OGL_FAST_ASSERT();
}

void OpenGLRenderer::EnableAllLights(bool pEnable)
{
	for (LightDataMap::iterator x = mLightDataMap.begin(); x != mLightDataMap.end(); ++x)
	{
		LightData* lLight = x->second;
		lLight->mEnabled = pEnable;
		if (pEnable)
		{
			::glEnable(GL_LIGHT0 + x->first);
		}
		else
		{
			::glDisable(GL_LIGHT0 + x->first);
		}
	}
}



void OpenGLRenderer::SetLightPosition(LightID pLightID, const vec3& pPos)
{
	Parent::SetLightPosition(pLightID, pPos);

	int lLightIndex = (int)pLightID;
	if (lLightIndex == INVALID_LIGHT)
		return;

	LightData* lLightData = GetLightData(lLightIndex);
	if (lLightData->mType == LIGHT_POINT ||
	   lLightData->mType == LIGHT_SPOT)
	{
		vec3 lLightPos = GetCameraActualTransformation().InverseTransform(pPos);
		float lPos[4];
		lPos[0] = (float)lLightPos.x;
		lPos[1] = (float)lLightPos.y;
		lPos[2] = (float)lLightPos.z;
		lPos[3] = 1.0f;

		GLenum lLight = GL_LIGHT0 + lLightIndex;
		glLightfv(lLight, GL_POSITION, lPos);
	}
	OGL_FAST_ASSERT();
}



void OpenGLRenderer::SetLightDirection(LightID pLightID, const vec3& pDir)
{
	Parent::SetLightDirection(pLightID, pDir);

	int lLightIndex = (int)pLightID;
	if (lLightIndex == INVALID_LIGHT)
		return;

	LightData* lLightData = GetLightData(lLightIndex);
	if (lLightData->mType == LIGHT_DIRECTIONAL ||
	   lLightData->mType == LIGHT_SPOT)
	{
		GLenum lLight = GL_LIGHT0 + lLightIndex;

		if (lLightData->mType == LIGHT_DIRECTIONAL)
		{
			float lVector[4];
			lVector[0] = -pDir.x;
			lVector[1] = -pDir.y;
			lVector[2] = -pDir.z;
			lVector[3] = 0.0f;
			glLightfv(lLight, GL_POSITION, lVector);
		}
		else
		{
			float lVector[3];
			lVector[0] = pDir.x;
			lVector[1] = pDir.y;
			lVector[2] = pDir.z;
			glLightfv(lLight, GL_SPOT_DIRECTION, lVector);
		}
	}
	OGL_FAST_ASSERT();
}

Renderer::TextureID OpenGLRenderer::AddTexture(Texture* pTexture)
{
	pTexture->SwapRGBOrder();
	return (Parent::AddTexture(pTexture));
}

void OpenGLRenderer::SetGlobalMaterialReflectance(float pRed, float pGreen, float pBlue, float pSpecularity)
{
	// This function is the evidence of the total stupidity behind 
	// OpenGL and probably computer graphics in general.
	// To be able to have full control of a surface's diffuseness, 
	// we need to update the light's diffuseness. And therein lies
	// the stupidity - light with "diffuseness" doesn't make any sense
	// at all.

	// TRICKY: must add some to the colors, since black light means disabled light!
	pRed += 0.01f;
	pGreen += 0.01f;
	pBlue += 0.01f;

	for (LightDataMap::iterator x = mLightDataMap.begin(); x != mLightDataMap.end(); ++x)
	{
		LightData* lLightData = x->second;

		if (lLightData->mEnabled == true)
		{
			const float lRed   = lLightData->mColor[0] * pRed;
			const float lGreen = lLightData->mColor[1] * pGreen;
			const float lBlue  = lLightData->mColor[2] * pBlue;

			float lSpecular[] =
			{
				lRed   * pSpecularity,
				lGreen * pSpecularity,
				lBlue  * pSpecularity,
				1.0f
			};

			float lDiffuse[] =
			{
				lRed   * (1.0f - pSpecularity),
				lGreen * (1.0f - pSpecularity),
				lBlue  * (1.0f - pSpecularity),
				1.0f
			};

			float lAmbient[] = {0, 0, 0, 1.0f};

			GLenum lLight = GL_LIGHT0 + x->first;
			glLightfv(lLight, GL_DIFFUSE, lDiffuse);
			glLightfv(lLight, GL_SPECULAR, lSpecular);
			glLightfv(lLight, GL_AMBIENT, lAmbient);
		}
	}
	OGL_FAST_ASSERT();
}

Renderer::TextureData* OpenGLRenderer::CreateTextureData(TextureID pTextureID)
{
	return new Parent::TextureData(pTextureID, mTMapIDManager.GetInvalidId());
}

Renderer::GeometryData* OpenGLRenderer::CreateGeometryData()
{
	return new OGLGeometryData();
}

const Canvas* OpenGLRenderer::GetMap(int pMapType, int pMipMapLevel, Texture* pUserTexture)
{
	switch (pMapType)
	{
		case Texture::COLOR_MAP:
			return pUserTexture->GetColorMap(pMipMapLevel);
		case Texture::ALPHA_MAP:
			return pUserTexture->GetAlphaMap(pMipMapLevel);
		case Texture::NORMAL_MAP:
			return pUserTexture->GetNormalMap(pMipMapLevel);
		case Texture::SPECULAR_MAP:
			return pUserTexture->GetSpecularMap(pMipMapLevel);
		default:
			deb_assert(false);
			return 0;
	};
}

void OpenGLRenderer::BindMap(int pMapType, TextureData* pTextureData, Texture* pTexture)
{
	OGL_FAST_ASSERT();
	deb_assert(pMapType >= 0 && pMapType < Texture::NUM_MAPS);

	bool lCompress = UiLepra::OpenGLExtensions::IsCompressedTexturesSupported() &&
					GetCompressedTexturesEnabled();

	if (pTextureData->mTMapID[pMapType] == mTMapIDManager.GetInvalidId())
	{
		pTextureData->mTMapID[pMapType] = mTMapIDManager.GetFreeId();
		pTextureData->mTMipMapLevelCount[pMapType] = pTexture->GetNumMipMapLevels();
	}

	glBindTexture(GL_TEXTURE_2D, pTextureData->mTMapID[pMapType]);
	OGL_FAST_ASSERT();

	int lSize = GetMap(pMapType, 0, pTexture)->GetPixelByteSize();
	deb_assert(lSize == 1 || lSize == 3 || lSize == 4);
	
	GLenum lPixelFormat;
	SetPixelFormat(lSize, lPixelFormat, lCompress, 
		strutil::Format(_T("AddTexture() - the texture has an invalid pixel size of %i bytes!"), lSize));
	OGL_FAST_ASSERT();

	for (int i = 0; i < pTexture->GetNumMipMapLevels(); i++)
	{
		const Canvas* lMap = GetMap(pMapType, i, pTexture);
		glTexImage2D(GL_TEXTURE_2D, 
			     i,
			     lSize,
			     lMap->GetWidth(),
			     lMap->GetHeight(),
			     0,
			     lPixelFormat, // TODO: Verify that this is GL_LUMINANCE for specular maps.
			     GL_UNSIGNED_BYTE,
			     lMap->GetBuffer());
		OGL_FAST_ASSERT();
		if (!GetMipMappingEnabled())
		{
			break;
		}
	}
	OGL_FAST_ASSERT();
}

void OpenGLRenderer::BindCubeMap(TextureData* pTextureData, Texture* pTexture)
{
#ifndef LEPRA_GL_ES
	// Compress textures if possible.
	bool lCompress = UiLepra::OpenGLExtensions::IsCompressedTexturesSupported() &&
				GetCompressedTexturesEnabled();

	int lSize = pTexture->GetCubeMapPosX(0)->GetPixelByteSize();
	GLenum lPixelFormat;
	SetPixelFormat(lSize, lPixelFormat, lCompress, 
		strutil::Format(_T("AddTexture() - the cube map has an invalid pixel size of %i bytes!"), lSize));

	if (pTextureData->mTMapID[Texture::CUBE_MAP] == mTMapIDManager.GetInvalidId())
	{
		pTextureData->mTMapID[Texture::CUBE_MAP] = mTMapIDManager.GetFreeId();
		pTextureData->mTMipMapLevelCount[Texture::CUBE_MAP] = pTexture->GetNumMipMapLevels();
	}

	// Bind cube map.
	glBindTexture(GL_TEXTURE_CUBE_MAP, pTextureData->mTMapID[Texture::CUBE_MAP]);

	for (int i = 0; i < pTexture->GetNumMipMapLevels(); i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X,
			     i,
			     lSize,
			     pTexture->GetCubeMapPosX(i)->GetWidth(),
			     pTexture->GetCubeMapPosX(i)->GetHeight(),
			     0,
			     lPixelFormat,
			     GL_UNSIGNED_BYTE,
			     pTexture->GetCubeMapPosX(i)->GetBuffer());

		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
			     i,
			     lSize,
			     pTexture->GetCubeMapNegX(i)->GetWidth(),
			     pTexture->GetCubeMapNegX(i)->GetHeight(),
			     0,
			     lPixelFormat,
			     GL_UNSIGNED_BYTE,
			     pTexture->GetCubeMapNegX(i)->GetBuffer());

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
			     i,
			     lSize,
			     pTexture->GetCubeMapPosY(i)->GetWidth(),
			     pTexture->GetCubeMapPosY(i)->GetHeight(),
			     0,
			     lPixelFormat,
			     GL_UNSIGNED_BYTE,
			     pTexture->GetCubeMapPosY(i)->GetBuffer());

		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
			     i,
			     lSize,
			     pTexture->GetCubeMapNegY(i)->GetWidth(),
			     pTexture->GetCubeMapNegY(i)->GetHeight(),
			     0,
			     lPixelFormat,
			     GL_UNSIGNED_BYTE,
			     pTexture->GetCubeMapNegY(i)->GetBuffer());

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
			     i,
			     lSize,
			     pTexture->GetCubeMapPosZ(i)->GetWidth(),
			     pTexture->GetCubeMapPosZ(i)->GetHeight(),
			     0,
			     lPixelFormat,
			     GL_UNSIGNED_BYTE,
			     pTexture->GetCubeMapPosZ(i)->GetBuffer());

		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
			     i,
			     lSize,
			     pTexture->GetCubeMapNegZ(i)->GetWidth(),
			     pTexture->GetCubeMapNegZ(i)->GetHeight(),
			     0,
			     lPixelFormat,
			     GL_UNSIGNED_BYTE,
			     pTexture->GetCubeMapNegZ(i)->GetBuffer());
	}
	OGL_FAST_ASSERT();
#endif // !GLES
}

void OpenGLRenderer::ReleaseMap(TextureData* pTextureData)
{
	for (int i = 0; i < Texture::NUM_MAPS; i++)
	{
		if (pTextureData->mTMapID[i] != 0)
		{
			mTMapIDManager.RecycleId(pTextureData->mTMapID[i]);
			GLuint lTextureName = pTextureData->mTMapID[i];
			glDeleteTextures(1, &lTextureName);
			pTextureData->mTMapID[i] = 0;
			pTextureData->mTMipMapLevelCount[i] = 0;
		}
	}

	if (pTextureData == GetEnvTexture())
	{
		SetEnvironmentMap(INVALID_TEXTURE);
	}
	//OGL_FAST_ASSERT();
}

void OpenGLRenderer::BindGeometry(Tbc::GeometryBase* pGeometry,
				  GeometryID /*pID*/,
				  MaterialType pMaterialType)
{
	OGL_FAST_ASSERT();

	// A hard coded check to make life easier for the user.
	if (pMaterialType == MAT_TEXTURE_SBMAP_PXS ||
	    pMaterialType == MAT_TEXTURE_AND_DIFFUSE_BUMPMAP_PXS)
	{
		pGeometry->GenerateTangentAndBitangentData();
	}

	OGLGeometryData* lGeometryData = (OGLGeometryData*)pGeometry->GetRendererData();
	if (pGeometry->IsGeometryReference())
	{
		Tbc::GeometryBase* lParentGeometry = ((Tbc::GeometryReference*)pGeometry)->GetParentGeometry();
		GeometryData* lParentGeometryData = (GeometryData*)lParentGeometry->GetRendererData();
		lGeometryData->CopyReferenceData(lParentGeometryData);
	}
	else
	{
		if (UiLepra::OpenGLExtensions::IsBufferObjectsSupported() == true)
		{
			// Upload geometry to the GFX hardware.

			// First, get a free buffer ID and store it first in the VoidPtr array.
			lGeometryData->mVertexBufferID = mBufferIDManager.GetFreeId();
			//log_volatile(mLog.Tracef(_T("Alloc buffer ID %u (vertex)."), lGeometryData->mVertexBufferID));
			lGeometryData->mIndexBufferID  = mBufferIDManager.GetFreeId();
		
			// Calculate the size of the created buffer.
			int lVertexCount = pGeometry->GetMaxVertexCount();
			int lBufferSize = lVertexCount * sizeof(float) * 3;
			if (pGeometry->GetNormalData() != 0)
			{
				lBufferSize += lVertexCount * sizeof(float) * 3;
			}
			if (pGeometry->GetColorData() != 0)
			{
				int lSize = 4;
				if (pGeometry->GetColorFormat() == Tbc::GeometryBase::COLOR_RGB)
					lSize = 3;

				lBufferSize += lVertexCount * sizeof(unsigned char) * lSize;
			}
			if (pGeometry->GetUVSetCount() > 0)
			{
				lBufferSize += (lVertexCount * sizeof(float) * pGeometry->GetUVCountPerVertex()) * pGeometry->GetUVSetCount();
			}
			if (pGeometry->GetTangentData() != 0)
			{
				// Assume that we have bitangent data as well.
				lBufferSize += lVertexCount * sizeof(float) * 6;
			}

			// Set the most appropriate buffer object hint.
			GLenum lGLHint = GL_DYNAMIC_DRAW;
#ifndef LEPRA_GL_ES
			switch(pGeometry->GetGeometryVolatility())
			{
			case Tbc::GeometryBase::GEOM_STATIC:
			case Tbc::GeometryBase::GEOM_SEMI_STATIC:
				lGLHint = GL_STATIC_DRAW;
				break;
			case Tbc::GeometryBase::GEOM_DYNAMIC:
				lGLHint = GL_DYNAMIC_DRAW;
				break;
			case Tbc::GeometryBase::GEOM_VOLATILE:
				lGLHint = GL_STREAM_DRAW;
				break;
			}
#endif // !GLES
			// Bind and create the vertex buffer in GFX memory.
			//log_volatile(mLog.Tracef(_T("glBindBuffer %u (vertex)."), lGeometryData->mVertexBufferID));
			UiLepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, 
							      (GLuint)lGeometryData->mVertexBufferID);
			UiLepra::OpenGLExtensions::glBufferData(GL_ARRAY_BUFFER, 
							      lBufferSize,
							      NULL, lGLHint);

			size_t lOffset = 0;
			// Upload vertex data.
			UiLepra::OpenGLExtensions::glBufferSubData(GL_ARRAY_BUFFER, lOffset,
				lVertexCount * sizeof(float) * 3, (void*)pGeometry->GetVertexData());
			lOffset += lVertexCount * sizeof(float) * 3;

			// Upload normal data.
			lGeometryData->mNormalOffset = lOffset;
			if (pGeometry->GetNormalData() != 0)
			{
				UiLepra::OpenGLExtensions::glBufferSubData(GL_ARRAY_BUFFER, lOffset,
					lVertexCount * sizeof(float) * 3, (void*)pGeometry->GetNormalData());
				lOffset += lVertexCount * sizeof(float) * 3;
			}

			// Upload color data.
			lGeometryData->mColorOffset = lOffset;
			if (pGeometry->GetColorData() != 0)
			{
				int lSize = 4;
				if (pGeometry->GetColorFormat() == Tbc::GeometryBase::COLOR_RGB)
					lSize = 3;

				UiLepra::OpenGLExtensions::glBufferSubData(GL_ARRAY_BUFFER, 
									 lOffset,
									 lVertexCount * sizeof(unsigned char) * lSize,
									 (void*)pGeometry->GetColorData());
				lOffset += lVertexCount * sizeof(unsigned char) * lSize;
			}


			// Upload UV data.
			lGeometryData->mUVOffset = lOffset;
			if (pGeometry->GetUVSetCount() > 0)
			{
				const int lUVCountPerVertex = pGeometry->GetUVCountPerVertex();
				for (unsigned i = 0; i < pGeometry->GetUVSetCount(); i++)
				{
					UiLepra::OpenGLExtensions::glBufferSubData(GL_ARRAY_BUFFER, 
										 lOffset,
										 lVertexCount * sizeof(float) * lUVCountPerVertex,
										 (void*)pGeometry->GetUVData(i));
					lOffset += lVertexCount * sizeof(float) * lUVCountPerVertex;
				}
			}

			// Upload tangent data.
			lGeometryData->mTangentOffset = lOffset;
			if (pGeometry->GetTangentData() != 0)
			{
				UiLepra::OpenGLExtensions::glBufferSubData(GL_ARRAY_BUFFER, 
									 lOffset,
									 lVertexCount * sizeof(float) * 3,
									 (void*)pGeometry->GetTangentData());
				lOffset += lVertexCount * sizeof(float) * 3;
			}
			else
			{
				lGeometryData->mTangentOffset = 0;
			}

			// Upload bitangent data.
			lGeometryData->mBitangentOffset = lOffset;
			if (pGeometry->GetBitangentData() != 0)
			{
				UiLepra::OpenGLExtensions::glBufferSubData(GL_ARRAY_BUFFER, 
									 lOffset,
									 lVertexCount * sizeof(float) * 3,
									 (void*)pGeometry->GetBitangentData());
				lOffset += lVertexCount * sizeof(float) * 3;
			}
			else
			{
				lGeometryData->mBitangentOffset = 0;
			}

			// Bind and create the index buffer in GFX memory.
			UiLepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (GLuint)lGeometryData->mIndexBufferID);
			UiLepra::OpenGLExtensions::glBufferData(GL_ELEMENT_ARRAY_BUFFER,
							      pGeometry->GetMaxIndexCount() * sizeof(vtx_idx_t),
							      0, lGLHint);

			UiLepra::OpenGLExtensions::glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,
								 0,
								 pGeometry->GetIndexCount() * sizeof(vtx_idx_t),
								 (void*)pGeometry->GetIndexData());
		}
	}

	OGL_FAST_ASSERT();
}

bool OpenGLRenderer::BindShadowGeometry(UiTbc::ShadowVolume* pShadowVolume, LightHint pLightHint)
{
	bool lOK = false;

	OGLGeometryData* lShadowGeom = (OGLGeometryData*)pShadowVolume->GetRendererData();
	if (UiLepra::OpenGLExtensions::IsBufferObjectsSupported() == true)
	{
		// Upload geometry to the GFX hardware.

		// First, get a free buffer ID and store it first in the VoidPtr array.
		lShadowGeom->mVertexBufferID = mBufferIDManager.GetFreeId();
		//log_volatile(mLog.Tracef(_T("Alloc buffer ID %u (vertex)."), lShadowGeom->mVertexBufferID));
		lShadowGeom->mIndexBufferID  = mBufferIDManager.GetFreeId();
	
		// Calculate the size of the created buffer.
		int lVertexCount = pShadowVolume->GetMaxVertexCount();
		int lBufferSize = lVertexCount * sizeof(float) * 3;

		// Set the most appropriate buffer object hint.
		GLenum lGLHint = GL_DYNAMIC_DRAW;
#ifndef LEPRA_GL_ES
		switch(pShadowVolume->GetGeometryVolatility())
		{
		case Tbc::GeometryBase::GEOM_STATIC:
		case Tbc::GeometryBase::GEOM_SEMI_STATIC:
			if (pLightHint == LIGHT_MOVABLE)
				lGLHint = GL_STREAM_DRAW;
			else
				lGLHint = GL_STATIC_DRAW;
			break;
		case Tbc::GeometryBase::GEOM_DYNAMIC:
			if (pLightHint == LIGHT_MOVABLE)
				lGLHint = GL_STREAM_DRAW;
			else
				lGLHint = GL_DYNAMIC_DRAW;
			break;
		case Tbc::GeometryBase::GEOM_VOLATILE:
			lGLHint = GL_STREAM_DRAW;
			break;
		}
#endif // !GLES
		
		// Bind and create the vertex buffer in GFX memory.
		//log_volatile(mLog.Tracef(_T("glBindBuffer %u (vertex)."), lShadowGeom->mVertexBufferID));
		UiLepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, (GLuint)lShadowGeom->mVertexBufferID);
		UiLepra::OpenGLExtensions::glBufferData(GL_ARRAY_BUFFER, 
						      lBufferSize,
						      NULL, lGLHint);

		int lOffset = 0;
		// Upload vertex data.
		UiLepra::OpenGLExtensions::glBufferSubData(GL_ARRAY_BUFFER, 
							 lOffset, 
							 lVertexCount * sizeof(float) * 3,
							 (void*)pShadowVolume->GetVertexData());
		lOffset += lVertexCount * sizeof(float) * 3;

		// Bind and create the index buffer in GFX memory.
		UiLepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (GLuint)lShadowGeom->mIndexBufferID);
		UiLepra::OpenGLExtensions::glBufferData(GL_ELEMENT_ARRAY_BUFFER,
						      pShadowVolume->GetMaxIndexCount() * sizeof(vtx_idx_t),
						      pShadowVolume->GetIndexData(), lGLHint);

		lOK = true;
	}

	OGL_FAST_ASSERT();
	return lOK;
}

void OpenGLRenderer::UpdateGeometry(GeometryID pGeometryID, bool pForce)
{
	GeometryTable::Iterator lIter;
	lIter = GetGeometryTable().Find(pGeometryID);

	if (lIter != GetGeometryTable().End())
	{
		OGLGeometryData* lGeomData = (OGLGeometryData*)*lIter;
		Tbc::GeometryBase* lGeometry = lGeomData->mGeometry;

		if (pForce)
		{
			ReleaseGeometry(lGeometry, GRO_IGNORE_MATERIAL);
			BindGeometry(lGeometry, pGeometryID, GetMaterialType(pGeometryID));
			return;
		}
		// Force update of shadow volumes.

		if (UiLepra::OpenGLExtensions::IsBufferObjectsSupported() && !lGeometry->IsGeometryReference())
		{
			int lVertexCount = lGeometry->GetVertexCount();

			if (lGeometry->GetVertexDataChanged() ||
			   lGeometry->GetColorDataChanged() ||
			   lGeometry->GetUVDataChanged())
			{
				//log_volatile(mLog.Tracef(_T("glBindBuffer %u (vertex)."), lGeomData->mVertexBufferID));
				UiLepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, (GLuint)lGeomData->mVertexBufferID);
			}

			if (lGeometry->GetVertexDataChanged())
			{
				UiLepra::OpenGLExtensions::glBufferSubData(GL_ARRAY_BUFFER,
									 0,
									 lVertexCount * sizeof(float) * 3,
									 (void*)lGeometry->GetVertexData());

				if (lGeometry->GetNormalData())
				{
					UiLepra::OpenGLExtensions::glBufferSubData(GL_ARRAY_BUFFER,
										 lGeomData->mNormalOffset,
										 lVertexCount * sizeof(float) * 3,
										 (void*)lGeometry->GetNormalData());
				}

				// Only reset the flag if there are no shadows to update.
				// The flag will be reset when the shadows are updated.
				if (lGeomData->mShadow <= NO_SHADOWS)
				{
					lGeometry->SetVertexDataChanged(false);
				}
			}

			if (lGeometry->GetColorDataChanged() && lGeometry->GetColorData())
			{
				int lSize = 4;
				if (lGeometry->GetColorFormat() == Tbc::GeometryBase::COLOR_RGB)
					lSize = 3;

				UiLepra::OpenGLExtensions::glBufferSubData(GL_ARRAY_BUFFER, 
									 lGeomData->mColorOffset,
									 lVertexCount * sizeof(unsigned char) * lSize,
									 (void*)lGeometry->GetColorData());
			}

			if (lGeometry->GetUVDataChanged())
			{
				OGL_FAST_ASSERT();
				size_t lOffset = lGeomData->mUVOffset;
				int lUVCountPerVertex = lGeometry->GetUVCountPerVertex();
				for (unsigned i = 0; i < lGeometry->GetUVSetCount(); i++)
				{
					UiLepra::OpenGLExtensions::glBufferSubData(GL_ARRAY_BUFFER, lOffset,
						lVertexCount * sizeof(float) * lUVCountPerVertex, (void*)lGeometry->GetUVData(i));
					lOffset += lVertexCount * sizeof(float) * lUVCountPerVertex;
					OGL_FAST_ASSERT();
				}
			}

			if (lGeometry->GetTangentData() != 0 &&
			   (lGeometry->GetVertexDataChanged() || lGeometry->GetUVDataChanged()))
			{
				if (lGeomData->mTangentOffset > 0)
				{
					UiLepra::OpenGLExtensions::glBufferSubData(GL_ARRAY_BUFFER, 
										 lGeomData->mTangentOffset,
										 lVertexCount * sizeof(float) * 3,
										 (void*)lGeometry->GetTangentData());
				}

				if (lGeomData->mBitangentOffset > 0)
				{
					UiLepra::OpenGLExtensions::glBufferSubData(GL_ARRAY_BUFFER, 
										 lGeomData->mBitangentOffset,
										 lVertexCount * sizeof(float) * 3,
										 (void*)lGeometry->GetBitangentData());
				}
			}

			if (lGeometry->GetIndexDataChanged())
			{
				UiLepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (GLuint)lGeomData->mIndexBufferID);
				UiLepra::OpenGLExtensions::glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0,
					lGeometry->GetTriangleCount() * 3 * sizeof(vtx_idx_t),
					(void*)lGeometry->GetIndexData());
			}
		}
	}
	OGL_FAST_ASSERT();
}

void OpenGLRenderer::ReleaseGeometry(Tbc::GeometryBase* pUserGeometry, GeomReleaseOption pOption)
{
	OGLGeometryData* lGeometry = (OGLGeometryData*)pUserGeometry->GetRendererData();

	if (pUserGeometry->IsGeometryReference() == false && 
		UiLepra::OpenGLExtensions::IsBufferObjectsSupported() == true)
	{
		GLuint lVertexBufferID = (GLuint)lGeometry->mVertexBufferID;
		GLuint lIndexBufferID  = (GLuint)lGeometry->mIndexBufferID;

		//log_volatile(mLog.Tracef(_T("glDeleteBuffers %u (vertex)."), lVertexBufferID));
		UiLepra::OpenGLExtensions::glDeleteBuffers(1, &lVertexBufferID);
		//log_volatile(mLog.Tracef(_T("glDeleteBuffers %u (index)."), lIndexBufferID));
		UiLepra::OpenGLExtensions::glDeleteBuffers(1, &lIndexBufferID);

		mBufferIDManager.RecycleId(lGeometry->mVertexBufferID);
		mBufferIDManager.RecycleId(lGeometry->mIndexBufferID);
	}

	if (pOption == GRO_REMOVE_FROM_MATERIAL)
	{
		OpenGLMaterial* lMat = (OpenGLMaterial*)GetMaterial(lGeometry->mMaterialType);
		if (lMat->RemoveGeometry(pUserGeometry) == Material::NOT_REMOVED)
		{
			// Make sure the geometry is properly removed -- otherwise we have a leak!
			// Hugge: In case fallback materials are disabled, the material may not
			//        have added the geometry in the first place.
			deb_assert(false);
		}
	}

	for (LightDataMap::iterator x = mLightDataMap.begin(); x != mLightDataMap.end(); ++x)
	{
		LightData* lLight = x->second;
		lLight->mShadowMapGeometrySet.Remove(lGeometry);
	}

	//OGL_FAST_ASSERT();
}


bool OpenGLRenderer::ChangeMaterial(GeometryID pGeometryID, MaterialType pMaterialType)
{
	if ((int)pMaterialType < 0 || (int)pMaterialType >= MAT_COUNT)
	{
		mLog.Errorf(_T("ChangeMaterial() - Material %i is not a valid material ID!"), (int)pMaterialType);
		return (false);
	}
	OpenGLMaterial* lMat = (OpenGLMaterial*)GetMaterial(pMaterialType);
	if (lMat == 0)
	{
		mLog.Errorf(_T("ChangeMaterial() - Material %i is not implemented!"), (int)pMaterialType);
		return (false);
	}

	GeometryTable::Iterator lIter;
	lIter = GetGeometryTable().Find(pGeometryID);

	bool lOk = (lIter != GetGeometryTable().End());
	if (lOk)
	{
		OGLGeometryData* lGeometry = (OGLGeometryData*)*lIter;
		if (lGeometry->mMaterialType != pMaterialType)
		{
			OpenGLMaterial* lMat = (OpenGLMaterial*)GetMaterial(lGeometry->mMaterialType);
			lMat->RemoveGeometry(lGeometry->mGeometry);
			lGeometry->mMaterialType = pMaterialType;
			lMat = (OpenGLMaterial*)GetMaterial(lGeometry->mMaterialType);
			lOk = lMat->AddGeometry(lGeometry->mGeometry);
		}
	}
	OGL_FAST_ASSERT();
	return (lOk);
}



bool OpenGLRenderer::PreRender(Tbc::GeometryBase* pGeometry)
{
	if (pGeometry->IsTwoSided())
	{
		::glDisable(GL_CULL_FACE);
	}
	// Check if we can avoid double-rendering (if in unlit mode).
	if (pGeometry->IsRecvNoShadows())
	{
		const bool lLightsEnabled = GetLightsEnabled();
		if (GetShadowMode() != NO_SHADOWS && !lLightsEnabled)
		{
			return false;
		}
		else if (lLightsEnabled)
		{
			::glDisable(GL_STENCIL_TEST);
		}
	}

	const xform& t = pGeometry->GetTransformation();
	if (pGeometry->IsExcludeCulling() || CheckCamCulling(t.GetPosition(), pGeometry->GetBoundingRadius()))
	{
		mVisibleTriangleCount += pGeometry->GetTriangleCount();
		mCamSpaceTransformation.FastInverseTransform(mCameraActualTransformation, mCameraActualOrientationInverse, t);
		float lModelViewMatrix[16];
		mCamSpaceTransformation.GetAs4x4TransposeMatrix(pGeometry->GetScale(), lModelViewMatrix);
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(lModelViewMatrix);
		return true;
	}
	else
	{
		mCulledTriangleCount += pGeometry->GetTriangleCount();
	}
	return false;
}

void OpenGLRenderer::PostRender(Tbc::GeometryBase* pGeometry)
{
	pGeometry->SetTransformationChanged(false);
	if (pGeometry->IsRecvNoShadows())
	{
		if (GetLightsEnabled())
		{
			::glEnable(GL_STENCIL_TEST);
		}
	}
	if (pGeometry->IsTwoSided())
	{
		::glEnable(GL_CULL_FACE);
	}
}



void OpenGLRenderer::DrawLine(const vec3& pPosition, const vec3& pVector, const Color& pColor)
{
	glEnable(GL_DEPTH_TEST);
	xform lCamTransform = GetCameraActualTransformation().InverseTransform(gIdentityTransformationF);
	float lModelViewMatrix[16];
	lCamTransform.GetAs4x4TransposeMatrix(lModelViewMatrix);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(lModelViewMatrix);

	glColor4ub(pColor.mRed, pColor.mGreen, pColor.mBlue, 255);
	GLfloat v[] = {pPosition.x, pPosition.y, pPosition.z, pPosition.x+pVector.x, pPosition.y+pVector.y, pPosition.z+pVector.z};
	::glVertexPointer(3, GL_FLOAT, 0, v);
	::glDrawArrays(GL_LINES, 0, 2);

	OGL_FAST_ASSERT();
}



unsigned OpenGLRenderer::RenderScene()
{
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
		float lFOVAngle;
		float lNear;
		float lFar;
		GetViewFrustum(lFOVAngle, lNear, lFar);
		SetViewFrustum(lFOVAngle, lNear, lFar);

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
		if (GetFogFar() > 0)
		{
			::glEnable(GL_FOG);
			::glFogi(GL_FOG_MODE, (GetFogExponent()>0)? GL_EXP : GL_LINEAR);
			::glFogf(GL_FOG_DENSITY, GetFogDensity());
			::glFogf(GL_FOG_START, GetFogNear());
			::glFogf(GL_FOG_END, GetFogFar());
			const float lFogColor[4] = {mClearColor.GetRf(), mClearColor.GetGf(), mClearColor.GetBf(), mClearColor.GetAf()};
			::glFogfv(GL_FOG_COLOR, lFogColor);
		}
		else
		{
			::glDisable(GL_FOG);
		}
#ifndef LEPRA_GL_ES
		::glPolygonMode(GL_FRONT_AND_BACK, IsWireframeEnabled()? GL_LINE : GL_FILL);
		::glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);
#endif // !GLES

		Material::EnableDrawMaterial(true);

		if (!IsPixelShadersEnabled())
		{
			OpenGLMatPXS::CleanupShaderPrograms();
		}
	}

	float lAmbientRed, lAmbientGreen, lAmbientBlue;
	GetAmbientLight(lAmbientRed, lAmbientGreen, lAmbientBlue);

	if (GetShadowMode() != NO_SHADOWS && GetLightsEnabled())
	{
		UpdateShadowMaps();

		// Disable all lights (for shadow rendering).
		for (LightDataMap::iterator x = mLightDataMap.begin(); x != mLightDataMap.end(); ++x)
		{
			LightData* lLight = x->second;
			if (lLight->mShadowRange > 0 && lLight->mEnabled)
			{
				::glDisable(GL_LIGHT0 + x->first);
			}
		}
		Parent::SetLightsEnabled(false);

		if (IsOutlineRenderingEnabled())
		{
			SetAmbientLight(lAmbientRed*0.5f, lAmbientGreen*0.5f, lAmbientBlue*0.5f);
		}

		// Prepare the pixel shader materials.
		OpenGLMatPXS::PrepareLights(this);

		// Render the scene darkened.
		for (int i = 0; i <= (int)MAT_LAST_SOLID; i++)
		{
			if (GetMaterial((MaterialType)i) != 0)
			{
				Material::RenderAllGeometry(GetCurrentFrame(), GetMaterial((MaterialType)i));
				OGL_FAST_ASSERT();
			}
		}

		// Shadow stencil buffer operations.
		::glEnable(GL_STENCIL_TEST);
		::glStencilFunc(GL_ALWAYS, 128, 0xFF);
		::glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);

		// Render all shadow maps. In this step, we only render the shadows
		// using alpha testing to the stencil buffer (no output to the color buffer).
		if (GetNumSpotLights() > 0 && GetShadowHint() == SH_VOLUMES_AND_MAPS)
		{
			RenderShadowMaps();
		}

		// Render scene with stencil shadows.
		RenderShadowVolumes();

		// Go back to normal stencil buffer operations.
		::glDepthFunc(GL_LEQUAL);
		::glEnable(GL_STENCIL_TEST);
		::glStencilFunc(GL_GEQUAL, 128, 0xFF);
		::glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

		SetLightsEnabled(true);

		// Enable all lights again.
		for (LightDataMap::iterator x = mLightDataMap.begin(); x != mLightDataMap.end(); ++x)
		{
			LightData* lLight = x->second;
			if (lLight->mEnabled)
			{
				::glEnable(GL_LIGHT0 + x->first);
			}
		}
	}

	bool lSkipOutlined = false;
	if (IsOutlineRenderingEnabled() && !IsWireframeEnabled())
	{
		Material::EnableDrawMaterial(false);
		SetAmbientLight(1, 1, 1);
		const vec3 lColor(mOutlineFillColor.GetRf(), mOutlineFillColor.GetGf(), mOutlineFillColor.GetBf());
		Tbc::GeometryBase::BasicMaterialSettings lMaterial(vec3(1, 1, 1), lColor, vec3(), 1, 1, false);
		OpenGLMaterial::SetBasicMaterial(lMaterial, this);
		Material::RenderAllGeometry(GetCurrentFrame(), GetMaterial(MAT_SINGLE_COLOR_SOLID));
		Material::RenderAllGeometry(GetCurrentFrame(), GetMaterial(MAT_SINGLE_COLOR_SOLID_PXS), GetMaterial(MAT_SINGLE_COLOR_SOLID));
		Material::RenderAllGeometry(GetCurrentFrame(), GetMaterial(MAT_SINGLE_COLOR_ENVMAP_SOLID), GetMaterial(MAT_SINGLE_COLOR_SOLID));
		Material::RenderAllGeometry(GetCurrentFrame(), GetMaterial(MAT_SINGLE_COLOR_OUTLINE_BLENDED));
		::glCullFace(GL_FRONT);
#ifndef LEPRA_GL_ES
		::glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif // !GLES
		::glDepthFunc(GL_LEQUAL);
		::glDisable(GL_LIGHTING);
		Material::EnableDrawMaterial(true);
		SetAmbientLight(lAmbientRed, lAmbientGreen, lAmbientBlue);
		Material::RenderAllGeometry(GetCurrentFrame(), GetMaterial(MAT_SINGLE_COLOR_SOLID));
		Material::RenderAllGeometry(GetCurrentFrame(), GetMaterial(MAT_SINGLE_COLOR_SOLID_PXS), GetMaterial(MAT_SINGLE_COLOR_SOLID));
		Material::RenderAllGeometry(GetCurrentFrame(), GetMaterial(MAT_SINGLE_COLOR_ENVMAP_SOLID), GetMaterial(MAT_SINGLE_COLOR_SOLID));
		Material::RenderAllGeometry(GetCurrentFrame(), GetMaterial(MAT_SINGLE_COLOR_OUTLINE_BLENDED));
		::glCullFace(GL_BACK);
#ifndef LEPRA_GL_ES
		::glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif // !GLES
		//::glDepthFunc(GL_LESS);
		if (GetLightsEnabled())
		{
			::glEnable(GL_LIGHTING);
		}
		lSkipOutlined = true;
	}

	SetAmbientLight(lAmbientRed, lAmbientGreen, lAmbientBlue);

	{
		// Prepare the pixel shader materials.
		OpenGLMatPXS::PrepareLights(this);
	}
	{
		// This renders the scene.
		for (int i = 0; i < (int)MAT_COUNT; ++i)
		{
			if (lSkipOutlined && (i == MAT_SINGLE_COLOR_SOLID || i == MAT_SINGLE_COLOR_OUTLINE_BLENDED ||
				i == MAT_SINGLE_COLOR_ENVMAP_SOLID || i == MAT_SINGLE_COLOR_SOLID_PXS))
			{
				continue;
			}
			if (i == MAT_LAST_SOLID)
			{
				::glDisable(GL_STENCIL_TEST);
			}
			Material* lMaterial = GetMaterial((MaterialType)i);
			if (lMaterial != 0)
			{
				Material::RenderAllGeometry(GetCurrentFrame(), lMaterial);
			}
		}

		DynamicRendererMap::iterator x = mDynamicRendererMap.begin();
		for (; x != mDynamicRendererMap.end(); ++x)
		{
			x->second->Render();
		}
	}

	{
		for (LightDataMap::iterator x = mLightDataMap.begin(); x != mLightDataMap.end(); ++x)
		{
			LightData* lLight = x->second;
			lLight->mTransformationChanged = false;
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
		if (UiLepra::OpenGLExtensions::IsBufferObjectsSupported() == true)
		{
			UiLepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, 0);
			UiLepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
	}

	OGL_ASSERT();

	return (GetCurrentFrame());
}

void OpenGLRenderer::RenderBillboards(Tbc::GeometryBase* pGeometry, bool pRenderTexture, bool pAddativeBlending, const BillboardRenderInfoArray& pBillboards)
{
	if (pBillboards.size() == 0)
	{
		return;
	}

	OGL_FAST_ASSERT();

	Material* lMaterial;
	if (pRenderTexture)
	{
		lMaterial = GetMaterial(MAT_SINGLE_TEXTURE_BLENDED);
		OpenGLRenderer::OGLGeometryData* lGeometry = (OpenGLRenderer::OGLGeometryData*)pGeometry->GetRendererData();
		((OpenGLMatSingleTextureBlended*)lMaterial)->BindTexture(
			lGeometry->mTA->mMaps[0].mMapID[Texture::COLOR_MAP],
			lGeometry->mTA->mMaps[0].mMipMapLevelCount[Texture::COLOR_MAP]);
		::glMatrixMode(GL_TEXTURE);
		::glLoadIdentity();
		::glMatrixMode(GL_MODELVIEW);
	}
	else
	{
		lMaterial = GetMaterial(MAT_SINGLE_COLOR_BLENDED);
	}
	lMaterial->PreRender();
	if (pAddativeBlending)
	{
		::glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	}
	else
	{
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
	xform lCamSpace;
	//Tbc::GeometryBase::BasicMaterialSettings lMaterialSettings(vec3(), vec3(), vec3(), 0, 0, false);
	BillboardRenderInfoArray::const_iterator x = pBillboards.begin();
	for (; x != pBillboards.end(); ++x)
	{
		//lMaterialSettings.mDiffuse = x->mColor;
		//lMaterialSettings.mAlpha = x->mOpacity;
		//lMaterial->SetBasicMaterial(lMaterialSettings);
		glColor4f(x->mColor.x, x->mColor.y, x->mColor.z, x->mOpacity);

		quat lRot = mCameraTransformation.GetOrientation();
		lRot.RotateAroundOwnY(x->mAngle);
		lCamSpace.FastInverseTransform(mCameraActualTransformation, mCameraActualOrientationInverse, xform(lRot, x->mPosition));
		float lModelViewMatrix[16];
		lCamSpace.GetAs4x4TransposeMatrix(x->mScale, lModelViewMatrix);
		::glLoadMatrixf(lModelViewMatrix);

		lMaterial->RawRender(pGeometry, x->mUVIndex);
	}
	lMaterial->PostRender();
	::glDepthMask(GL_TRUE);
	//::glEnable(GL_NORMALIZE);
	::glDisable(GL_BLEND);
	::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	if (!GetTexturingEnabled())
	{
		::glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		::glDisable(GL_TEXTURE_2D);
	}

	OGL_FAST_ASSERT();
}

void OpenGLRenderer::RenderRelative(Tbc::GeometryBase* pGeometry, const quat* pLightOrientation)
{
	OGL_FAST_ASSERT();

	::glMatrixMode(GL_MODELVIEW);
	::glPushMatrix();
	float lModelViewMatrix[16];
	pGeometry->GetTransformation().GetAs4x4TransposeMatrix(pGeometry->GetScale(), lModelViewMatrix);
	::glLoadMatrixf(lModelViewMatrix);

	::glEnable(GL_DEPTH_TEST);

	const LightID lLightId = GetClosestLight(GetLightCount()-1);
	const LightData* lLightData = GetLightData(lLightId);
	if (pLightOrientation)
	{
		::glEnable(GL_LIGHTING);
		quat lOrientation = pGeometry->GetTransformation().GetOrientation().GetInverse();
		LightData lDataCopy = *lLightData;
		lOrientation *= *pLightOrientation;
		lDataCopy.mDirection = lOrientation * lLightData->mDirection;
		SetupGLLight(lLightId, &lDataCopy);
	}

	GeometryData* lGeometryData = (GeometryData*)pGeometry->GetRendererData();
	if (lGeometryData)
	{
		GetMaterial(lGeometryData->mMaterialType)->PreRender();
		GetMaterial(lGeometryData->mMaterialType)->RenderGeometry(pGeometry);
		GetMaterial(lGeometryData->mMaterialType)->PostRender();
		ResetAmbientLight(false);
	}

	if (pLightOrientation)
	{
		SetupGLLight(lLightId, lLightData);
	}

	PostRender(pGeometry);

	::glDisable(GL_LIGHTING);
	::glDisable(GL_DEPTH_TEST);

	::glMatrixMode(GL_MODELVIEW);
	::glPopMatrix();

	OGL_FAST_ASSERT();
}

int OpenGLRenderer::GetEnvMapID()
{
	if (GetEnvTexture() != 0)
	{
		if (GetEnvTexture()->mIsCubeMap == false)
		{
			return GetEnvTexture()->mTMapID[Texture::COLOR_MAP];
		}
		else
		{
			return GetEnvTexture()->mTMapID[Texture::CUBE_MAP];
		}
	}

	return mTMapIDManager.GetInvalidId();
}


int OpenGLRenderer::GetNumTextureUnits() const
{
	return mNumTextureUnits;
}

void OpenGLRenderer::ProcessLights()
{
	SetLightsEnabled(GetLightsEnabled());
	
	// Transform all light positions.
	for (LightDataMap::iterator x = mLightDataMap.begin(); x != mLightDataMap.end(); ++x)
	{
		LightData* lLight = x->second;

		if (lLight->mType == LIGHT_DIRECTIONAL)
		{
			vec3 lLightDir = GetCameraActualOrientationInverse() * lLight->mDirection;
			float lPos[4] =
			{
				(float)-lLightDir.x,
				(float)-lLightDir.y,
				(float)-lLightDir.z,
				0.0f
			};
			glLightfv(GL_LIGHT0 + x->first, GL_POSITION, lPos);
		}
		else if(lLight->mType == LIGHT_POINT)
		{
			vec3 lLightPos = GetCameraActualTransformation().InverseTransform(lLight->mPosition);
			float lPos[4] =
			{
				(float)lLightPos.x,
				(float)lLightPos.y,
				(float)lLightPos.z,
				1.0f
			};
			glLightfv(GL_LIGHT0 + x->first, GL_POSITION, lPos);
		}
		else if(lLight->mType == LIGHT_SPOT)
		{
			vec3 lLightPos = GetCameraActualTransformation().InverseTransform(lLight->mPosition);
			vec3 lLightDir = GetCameraActualOrientationInverse() * lLight->mDirection;
			float lPos[4] =
			{
				(float)lLightPos.x,
				(float)lLightPos.y,
				(float)lLightPos.z,
				1.0f
			};

			float lDir[3] =
			{
				(float)lLightDir.x,
				(float)lLightDir.y,
				(float)lLightDir.z
			};

			glLightfv(GL_LIGHT0 + x->first, GL_POSITION, lPos);
			glLightfv(GL_LIGHT0 + x->first, GL_SPOT_DIRECTION, lDir);

			if (GetShadowHint() == SH_VOLUMES_AND_MAPS)
			{
				if (lLight->mTransformationChanged == true)
				{
					if (lLight->mShadowRange > 0)
					{
						lLight->mShadowMapNeedUpdate = true;
					}
					lLight->mTransformationChanged = false;
				}

				if (lLight->mShadowMapNeedUpdate == true)
					RegenerateShadowMap(lLight);
			}
		}
	}
	OGL_FAST_ASSERT();
}

void OpenGLRenderer::RenderShadowVolumes()
{
	OGL_FAST_ASSERT();

	// Disable all fancy gfx.
#if !defined(LEPRA_GL_ES) && !defined(LEPRA_MAC)
	//glPushAttrib(GL_ENABLE_BIT | GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glDisable(GL_LOGIC_OP);
	glEnable(GL_POLYGON_OFFSET_EXT);
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
	glColorMask(0, 0, 0, 0);
	glDepthMask(GL_FALSE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);

	glDepthFunc(GL_LESS);
	glPolygonOffset(0.05f, 0);


	glMatrixMode(GL_MODELVIEW);

	// Offset shadow in light direction to avoid Z-fighting.
	vec3 lCamShadowOffset;
	for (LightDataMap::iterator x = mLightDataMap.begin(); x != mLightDataMap.end(); ++x)
	{
		LightData* lLight = x->second;
		if (lLight->mEnabled && lLight->mType == LIGHT_DIRECTIONAL)
		{
			lCamShadowOffset = lLight->mDirection*0.1f;
		}
	}

	// Render shadow volumes in two steps.
	for (int lStep = 0; lStep < 2; lStep++)
	{
		if (lStep == 0)
		{
			// Render back faces.
			glCullFace(GL_FRONT);
			glStencilFunc(GL_ALWAYS, 128, 0xFF);
			glStencilOp(GL_KEEP, GL_INCR, GL_KEEP);
		}
		else // lStep == 1
		{
			// Render front faces.
			glCullFace(GL_BACK);
			glStencilFunc(GL_ALWAYS, 128, 0xFF);
			glStencilOp(GL_KEEP, GL_DECR, GL_KEEP);
		}

		ShadowVolumeTable::Iterator lIter;
		for (lIter = GetShadowVolumeTable().First(); lIter != GetShadowVolumeTable().End(); ++lIter)
		{
			OGLGeometryData* lShadowGeometry = (OGLGeometryData*)*lIter;
			ShadowVolume* lShadowVolume = (ShadowVolume*)lShadowGeometry->mGeometry;

			if (lShadowVolume->GetParentGeometry()->GetAlwaysVisible() == true ||
			   lShadowVolume->GetParentGeometry()->GetLastFrameVisible() == GetCurrentFrame())
			{
				xform lShadowTransformation(lShadowVolume->GetTransformation());
				lShadowTransformation.GetPosition() += lCamShadowOffset;
				mCamSpaceTransformation.FastInverseTransform(mCameraActualTransformation, mCameraActualOrientationInverse, lShadowTransformation);
				float lModelViewMatrix[16];
				mCamSpaceTransformation.GetAs4x4TransposeMatrix(lShadowVolume->GetScale(), lModelViewMatrix);
				::glLoadMatrixf(lModelViewMatrix);

				if (UiLepra::OpenGLExtensions::IsBufferObjectsSupported() == true)
				{
					GLuint lVertexBufferID = (GLuint)lShadowGeometry->mVertexBufferID;
					GLuint lIndexBufferID  = (GLuint)lShadowGeometry->mIndexBufferID;

					//log_volatile(mLog.Tracef(_T("glBindBuffer %u (vertex)."), lShadowGeometry->mVertexBufferID));
					UiLepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, lVertexBufferID);
					glVertexPointer(3, GL_FLOAT, 0, 0);

					if (lShadowVolume->GetVertexDataChanged() == true)
					{
						// Upload new vertices.
						UiLepra::OpenGLExtensions::glBufferSubData(GL_ARRAY_BUFFER, 0,
							lShadowVolume->GetVertexCount() * sizeof(float) * 3, (void*)lShadowVolume->GetVertexData());
						lShadowVolume->SetVertexDataChanged(false);
					}

					UiLepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lIndexBufferID);
					
					if (lShadowVolume->GetIndexDataChanged() == true)
					{
						UiLepra::OpenGLExtensions::glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0,
							lShadowVolume->GetTriangleCount() * 3 * sizeof(vtx_idx_t), (void*)lShadowVolume->GetIndexData());
					}

					glDrawElements(GL_TRIANGLES, lShadowVolume->GetTriangleCount() * 3, LEPRA_GL_INDEX_TYPE, 0);
				}
				else
				{
					glVertexPointer(3, GL_FLOAT, 0, lShadowVolume->GetVertexData());
					glDrawElements(GL_TRIANGLES, lShadowVolume->GetTriangleCount() * 3, LEPRA_GL_INDEX_TYPE, lShadowVolume->GetIndexData());
				}
			}
		}
	}

	// Reset all settings.
#if !defined(LEPRA_GL_ES) && !defined(LEPRA_MAC)
	//glPopAttrib();
	glDisable(GL_POLYGON_OFFSET_EXT);
#endif // !GLES
	glShadeModel(GL_SMOOTH);
	glColorMask(1, 1, 1, 1);
	glDepthFunc(GL_LEQUAL);
	glPolygonOffset(0, 0);
	glDepthMask(GL_TRUE);

	OGL_FAST_ASSERT();
}

int OpenGLRenderer::RenderShadowMaps()
{
	int lCount = 0;
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


	static const GLfloat slSPlane[4] = {1.0f, 0.0f, 0.0f, 0.0f};
	static const GLfloat slTPlane[4] = {0.0f, 1.0f, 0.0f, 0.0f};
	static const GLfloat slRPlane[4] = {0.0f, 0.0f, 1.0f, 0.0f};
	static const GLfloat slQPlane[4] = {0.0f, 0.0f, 0.0f, 1.0f};

	glTexGenfv(GL_S, GL_EYE_PLANE, slSPlane);
	glTexGenfv(GL_T, GL_EYE_PLANE, slTPlane);
	glTexGenfv(GL_R, GL_EYE_PLANE, slRPlane);
	glTexGenfv(GL_Q, GL_EYE_PLANE, slQPlane);

	glEnable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glAlphaFunc(GL_LESS, 0.5f);

	for (LightDataMap::iterator x = mLightDataMap.begin(); x != mLightDataMap.end(); ++x)
	{
		LightData* lLight = x->second;

		if (lLight->mShadowMapID != 0)
		{
			glBindTexture(GL_TEXTURE_2D, lLight->mShadowMapID);

			// Prepare the texture projection matrix.
			static float slTextureMatrix[16];
			glMatrixMode(GL_TEXTURE);
			glLoadIdentity();
			glTranslatef(0.5f, 0.5f, 0.5f);
			glScalef(0.5f, 0.5f, 0.5f);
			glMultMatrixf(lLight->mLightProjectionMatrix);
			glGetFloatv(GL_TEXTURE_MATRIX, slTextureMatrix);

			LightData::GeometrySet::Iterator lGeoIter;
			for (lGeoIter = lLight->mShadowMapGeometrySet.First();
				lGeoIter != lLight->mShadowMapGeometrySet.End();
				++lGeoIter)
			{
				OGLGeometryData* lGeometry = (OGLGeometryData*)*lGeoIter;

				float lModelViewMatrix[16];
				GetCameraActualTransformation().InverseTransform(lGeometry->mGeometry->GetTransformation()).GetAs4x4TransposeMatrix(lModelViewMatrix);

				// Camera model view matrix.
				glMatrixMode(GL_MODELVIEW);
				glLoadMatrixf(lModelViewMatrix);

				float lLightModelViewMatrix[16];
				xform lLightTransformation(lLight->mOrientation, lLight->mPosition);
				lLightTransformation.mOrientation *= mCameraActualRotation;
				(lLightTransformation.InverseTransform(lGeometry->mGeometry->GetTransformation())).GetAs4x4TransposeMatrix(lLightModelViewMatrix);

				// Setup the texture projection matrix.
				glMatrixMode(GL_TEXTURE);
				glLoadMatrixf(slTextureMatrix);
				glMultMatrixf(lLightModelViewMatrix);

				// Need to call these here to update the model view transform.
				glTexGenfv(GL_S, GL_EYE_PLANE, slSPlane);
				glTexGenfv(GL_T, GL_EYE_PLANE, slTPlane);
				glTexGenfv(GL_R, GL_EYE_PLANE, slRPlane);
				glTexGenfv(GL_Q, GL_EYE_PLANE, slQPlane);

				if (UiLepra::OpenGLExtensions::IsBufferObjectsSupported() == true)
				{
					GLuint lVertexBufferID = (GLuint)lGeometry->mVertexBufferID;
					GLuint lIndexBufferID  = (GLuint)lGeometry->mIndexBufferID;

					//log_volatile(mLog.Tracef(_T("glBindBuffer %u (vertex)."), lGeometry->mVertexBufferID));
					UiLepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, lVertexBufferID);
					glVertexPointer(3, GL_FLOAT, 0, 0);

					UiLepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lIndexBufferID);
					
					glDrawElements(GL_TRIANGLES,
								lGeometry->mGeometry->GetTriangleCount() * 3,
								LEPRA_GL_INDEX_TYPE,
								0);
				}
				else
				{
					glVertexPointer(3, GL_FLOAT, 0, lGeometry->mGeometry->GetVertexData());
					glDrawElements(GL_TRIANGLES,
								lGeometry->mGeometry->GetTriangleCount() * 3,
								LEPRA_GL_INDEX_TYPE,
								lGeometry->mGeometry->GetIndexData());
				}
			}

			lCount++;
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

	return lCount;
}

void OpenGLRenderer::RegenerateShadowMap(LightData* pLight)
{
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
	glViewport(0, 0, pLight->mShadowMapRes, pLight->mShadowMapRes);

	// Set projection matrix.
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	Perspective(pLight->mCutoffAngle * 2.0f, 1.0f, pLight->mShadowMapNear, pLight->mShadowMapFar);

	glGetFloatv(GL_PROJECTION_MATRIX, pLight->mLightProjectionMatrix);

	glMatrixMode(GL_MODELVIEW);


	xform lLightTransformation(pLight->mOrientation, pLight->mPosition);
    
	// Clear depth buffer.
	glClear(GL_DEPTH_BUFFER_BIT);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);

	// Render "scene" with no fx, and only to the depth buffer. 
	// The scene in this case are all objects within the light's bounding radius.

	// Overcome imprecision.
	glEnable(GL_POLYGON_OFFSET_FILL);
//	glPolygonOffset(1.1f, 4.0f);
	glPolygonOffset(1.1f, 2.0f);

	LightData::GeometrySet::Iterator lIter;
	for (lIter = pLight->mShadowMapGeometrySet.First(); 
		lIter != pLight->mShadowMapGeometrySet.End(); 
		++lIter)
	{
		OGLGeometryData* lGeometry = (OGLGeometryData*)*lIter;

		if (lGeometry->mShadow >= CAST_SHADOWS)
		{
			float lModelViewMatrix[16];
			(lLightTransformation.InverseTransform(lGeometry->mGeometry->GetTransformation())).GetAs4x4TransposeMatrix(lModelViewMatrix);
			glLoadMatrixf(lModelViewMatrix);

			if (UiLepra::OpenGLExtensions::IsBufferObjectsSupported() == true)
			{
				GLuint lVertexBufferID = (GLuint)lGeometry->mVertexBufferID;
				GLuint lIndexBufferID  = (GLuint)lGeometry->mIndexBufferID;

				//log_volatile(mLog.Tracef(_T("glBindBuffer %u (vertex)."), lGeometry->mVertexBufferID));
				UiLepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, lVertexBufferID);
				glVertexPointer(3, GL_FLOAT, 0, 0);

				UiLepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lIndexBufferID);
				
				glDrawElements(GL_TRIANGLES,
							lGeometry->mGeometry->GetTriangleCount() * 3,
							LEPRA_GL_INDEX_TYPE,
							0);
			}
			else
			{
				glVertexPointer(3, GL_FLOAT, 0, lGeometry->mGeometry->GetVertexData());
				glDrawElements(GL_TRIANGLES,
							lGeometry->mGeometry->GetTriangleCount() * 3,
							LEPRA_GL_INDEX_TYPE,
							lGeometry->mGeometry->GetIndexData());
			}
		}
	}

	if (pLight->mShadowMapID == 0)
	{
		// Create a new depth texture.
		pLight->mShadowMapID = mTMapIDManager.GetFreeId();
	}

	// Bind the texture.
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, pLight->mShadowMapID);

#ifndef LEPRA_GL_ES
	// And finally read it from the back buffer.
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
					 0, 0, pLight->mShadowMapRes, pLight->mShadowMapRes, 0);
#endif // !GLES

	glDisable(GL_POLYGON_OFFSET_FILL);
	glDisable(GL_TEXTURE_2D);
	//glPopAttrib();
	glShadeModel(GL_SMOOTH);
	glColorMask(1, 1, 1, 1);
	if (GetLightsEnabled())
	{
		glEnable(GL_LIGHTING);
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	pLight->mShadowMapNeedUpdate = false;

	OGL_FAST_ASSERT();
}

void OpenGLRenderer::Perspective(float pFOVAngle, float pAspectRatio, float pNear, float pFar)
{
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

	float lDY = 1.0f / tan(Math::Deg2Rad(pFOVAngle) / 2.0f);
	float lDX = lDY / pAspectRatio;
	if (GetScreen()->GetOutputRotation()%180 != 0)
	{
		std::swap(lDX, lDY);
	}

	float lProjectionMatrix[16];

	lProjectionMatrix[0]  = lDX;
	lProjectionMatrix[1]  = 0;
	lProjectionMatrix[2]  = 0;
	lProjectionMatrix[3]  = 0;

	lProjectionMatrix[4]  = 0;
	lProjectionMatrix[5]  = lDY;
	lProjectionMatrix[6]  = 0;
	lProjectionMatrix[7]  = 0;

	lProjectionMatrix[8]  = 0;
	lProjectionMatrix[9]  = 0;
	lProjectionMatrix[10] = -(pFar + pNear) / (pFar - pNear);
	lProjectionMatrix[11] = -1;

	lProjectionMatrix[12] = 0;
	lProjectionMatrix[13] = 0;
	lProjectionMatrix[14] = (-2.0f * pFar * pNear) / (pFar - pNear);
	lProjectionMatrix[15] = 0;

	::glMatrixMode(GL_PROJECTION);
	::glLoadMatrixf(lProjectionMatrix);
	::glRotatef((float)GetScreen()->GetOutputRotation(), 0, 0, 1);

	::glMatrixMode(GL_MODELVIEW);

	OGL_FAST_ASSERT();
}

void OpenGLRenderer::SetPixelFormat(int& pSize, GLenum& pPixelFormat, bool pCompress, const str& pErrorMessage)
{
	switch(pSize)
	{
	case 4:	pPixelFormat	= GL_RGBA;	break;
	case 3:	pPixelFormat	= GL_RGB;	break;
	case 1:	pPixelFormat	= GL_LUMINANCE;	break;
	default:
		pPixelFormat	= GL_RGB;
		mLog.Info(pErrorMessage);
	break;
	}

#ifndef LEPRA_GL_ES
	if (pCompress == true)
	{
		pSize = (pSize == 4) ? GL_COMPRESSED_RGBA : GL_COMPRESSED_RGB;
	}
	else
#endif // !GLES
	{
		switch(pSize)
		{
			case 4:		pSize = GL_RGBA;	break;
			case 3:		pSize = GL_RGB;		break;
			case 1:		pSize = GL_ALPHA;	break;
			default:	pSize = GL_RGB;		break;
		}
	}
}



loginstance(UI_GFX_3D, OpenGLRenderer);



}
