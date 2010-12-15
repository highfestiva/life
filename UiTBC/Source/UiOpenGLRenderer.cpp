
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games

#include "../Include/UiOpenGLRenderer.h"
#include "../../Lepra/Include/Canvas.h"
#include "../../Lepra/Include/Log.h"
#include "../../Lepra/Include/Math.h"
#include "../../Lepra/Include/Performance.h"
#include "../../Lepra/Include/Transformation.h"
#include "../../TBC/Include/GeometryBase.h"
#include "../../UiLepra/Include/UiOpenGLExtensions.h"
#include "../Include/UiOpenGLMaterials.h"
#include "../Include/UiTexture.h"



namespace UiTbc
{



#ifdef LEPRA_DEBUG
#define OGL_ASSERT()	{ GLenum lGlError = glGetError(); assert(lGlError == GL_NO_ERROR); }
#else // !Debug
#define OGL_ASSERT()
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
	case MAT_SINGLE_TEXTURE_SOLID_PXS:
		return new OpenGLMatSingleTextureSolidPXS(this, GetMaterial(MAT_SINGLE_TEXTURE_SOLID));
	case MAT_SINGLE_COLOR_ENVMAP_SOLID:
		return new OpenGLMatSingleColorEnvMapSolid(this, 0);
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
	InitRenderer();

	float r, g, b;
	GetAmbientLight(r, g, b);
	SetAmbientLight(r, g, b);

	glGetIntegerv(GL_MAX_TEXTURE_UNITS, &mNumTextureUnits);
	glDepthFunc(GL_LEQUAL);
	glClearStencil(128);
}



OpenGLRenderer::~OpenGLRenderer()
{
	CloseRenderer();
}

void OpenGLRenderer::Clear(unsigned pClearFlags)
{
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
	if (CheckFlag(pClearFlags, CLEAR_ACCUMULATIONBUFFER))
	{
		mGLClearMask |= GL_ACCUM_BUFFER_BIT;
	}

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
	::glClearColor(pColor.GetRf(), pColor.GetGf(), pColor.GetBf(), 1.0f);
	OGL_ASSERT();
}

bool OpenGLRenderer::IsPixelShadersEnabled() const
{
	return UiLepra::OpenGLExtensions::IsShaderAsmProgramsSupported() && Parent::IsPixelShadersEnabled();
}

void OpenGLRenderer::SetViewport(const PixelRect& pViewport)
{
	OGL_ASSERT();
	Parent::SetViewport(pViewport);
	glViewport(pViewport.mLeft, GetScreen()->GetHeight() - pViewport.mBottom, 
		pViewport.GetWidth(), pViewport.GetHeight());
	OGL_ASSERT();
}

void OpenGLRenderer::SetViewFrustum(float pFOVAngle, float pNear, float pFar)
{
	Parent::SetViewFrustum(pFOVAngle, pNear, pFar);
	glMatrixMode(GL_PROJECTION);
	Perspective(pFOVAngle, GetAspectRatio(), pNear, pFar);

	glMatrixMode(GL_MODELVIEW);
	OGL_ASSERT();
}

void OpenGLRenderer::SetClippingRect(const PixelRect& pRect)
{
	Parent::SetClippingRect(pRect);
	glScissor(pRect.mLeft, 
		  GetScreen()->GetHeight() - pRect.mBottom, 
		  pRect.GetWidth(), pRect.GetHeight());
	OGL_ASSERT();
}

void OpenGLRenderer::ResetClippingRect()
{
	Parent::ResetClippingRect();
	const PixelRect& lClippingRect = GetClippingRect();
	glScissor(lClippingRect.mLeft, 
		  GetScreen()->GetHeight() - lClippingRect.mBottom, 
		  lClippingRect.GetWidth(), lClippingRect.GetHeight());
	OGL_ASSERT();
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
	OGL_ASSERT();
}

void OpenGLRenderer::SetDepthTestEnabled(bool pEnabled)
{
	if (pEnabled == true)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
	OGL_ASSERT();
}

void OpenGLRenderer::SetAmbientLight(float pRed, float pGreen, float pBlue)
{
	Parent::SetAmbientLight(pRed, pGreen, pBlue);

	float lAmbientLight[] = {pRed, pGreen, pBlue, 1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lAmbientLight);
	OGL_ASSERT();
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
	OGL_ASSERT();
}

int OpenGLRenderer::ReleaseShadowMap(int pShadowMapID)
{
	if (pShadowMapID != mTMapIDManager.GetInvalidId())
	{
		mTMapIDManager.RecycleId(pShadowMapID);
		GLuint lShadowMapID = (GLuint)pShadowMapID;
		glDeleteTextures(1, &lShadowMapID);
	}
	//OGL_ASSERT();
	return mTMapIDManager.GetInvalidId();
}




Renderer::LightID OpenGLRenderer::AddDirectionalLight(LightHint pHint,
						      float pDirX, float pDirY, float pDirZ,
						      float pRed, float pGreen, float pBlue,
						      float pShadowRange)
{
	LightID lLightID = Parent::AddDirectionalLight(pHint, pDirX, pDirY, pDirZ, pRed, pGreen, pBlue, pShadowRange);
	SetupGLLight((int)lLightID, GetLightData((int)lLightID));
	OGL_ASSERT();
	return lLightID;
}

Renderer::LightID OpenGLRenderer::AddPointLight(LightHint pHint,
						float pPosX, float pPosY, float pPosZ,
						float pRed, float pGreen, float pBlue,
						float pLightRadius,
						float pShadowRange)
{
	LightID lLightID = Parent::AddPointLight(pHint, pPosX, pPosY, pPosZ, pRed, pGreen, pBlue, pLightRadius, pShadowRange);
	SetupGLLight((int)lLightID, GetLightData((int)lLightID));
	OGL_ASSERT();
	return lLightID;
}

Renderer::LightID OpenGLRenderer::AddSpotLight(LightHint pHint,
					       float pPosX, float pPosY, float pPosZ,
					       float pDirX, float pDirY, float pDirZ,
					       float pRed, float pGreen, float pBlue,
					       float pCutoffAngle,
					       float pSpotExponent,
					       float pLightRadius,
					       float pShadowRange)
{
	LightID lLightID = Parent::AddSpotLight(pHint, 
		pPosX, pPosY, pPosZ, 
		pDirX, pDirY, pDirZ, 
		pRed, pGreen, pBlue, 
		pCutoffAngle, 
		pSpotExponent, 
		pLightRadius, 
		pShadowRange);
	SetupGLLight((int)lLightID, GetLightData((int)lLightID));
	OGL_ASSERT();
	return lLightID;
}

void OpenGLRenderer::SetupGLLight(int pLightIndex, const LightData& pLight)
{
	GLenum lLight = GL_LIGHT0 + pLightIndex;
	glEnable(lLight);

	if (pLight.mType != LIGHT_SPOT)
	{
		int l180 = 180;
		glLighti(lLight, GL_SPOT_CUTOFF, l180);

		float lDir[3] = {0, 0, -1};
		glLightfv(lLight, GL_SPOT_DIRECTION, lDir);
	}

	float lPos[4];
	if (pLight.mType == LIGHT_POINT ||
	   pLight.mType == LIGHT_SPOT)
	{
		Vector3DF lLightPos = GetCameraTransformation().InverseTransform(pLight.mPosition);
		lPos[0] = (float)lLightPos.x;
		lPos[1] = (float)lLightPos.y;
		lPos[2] = (float)lLightPos.z;
		lPos[3] = 1.0f;

		if (pLight.mType == LIGHT_SPOT)
		{
			Vector3DF lLightDir = GetCameraTransformation().GetOrientation().GetInverseRotatedVector(pLight.mDirection);

			float lDir[3];
			lDir[0] = (float)lLightDir.x;
			lDir[1] = (float)lLightDir.y;
			lDir[2] = (float)lLightDir.z;

			glLightfv(lLight, GL_SPOT_DIRECTION, lDir);
			glLightfv(lLight, GL_SPOT_CUTOFF, &pLight.mCutoffAngle);
		}

		float lConst = 0.0f;
		float lLinear = 0.0f;
		float lQuad = 1.0f;
		glLightfv(lLight, GL_CONSTANT_ATTENUATION, &lConst);
		glLightfv(lLight, GL_LINEAR_ATTENUATION, &lLinear);
		glLightfv(lLight, GL_QUADRATIC_ATTENUATION, &lQuad);
	}
	else if(pLight.mType == LIGHT_DIRECTIONAL)
	{
		lPos[0] = -(float)pLight.mDirection.x;
		lPos[1] = -(float)pLight.mDirection.y;
		lPos[2] = -(float)pLight.mDirection.z;
		lPos[3] = 0.0f;

		float lConst = 1.0f;
		float lLinear = 0.0f;
		float lQuad = 0.0f;
		glLightfv(lLight, GL_CONSTANT_ATTENUATION, &lConst);
		glLightfv(lLight, GL_LINEAR_ATTENUATION, &lLinear);
		glLightfv(lLight, GL_QUADRATIC_ATTENUATION, &lQuad);
	}

	float lAmbient[] = {0, 0, 0, 1.0f};

	glLightfv(lLight, GL_POSITION, lPos);
	glLightfv(lLight, GL_AMBIENT, lAmbient);
	glLightfv(lLight, GL_DIFFUSE, pLight.mColor);
	glLightfv(lLight, GL_SPECULAR, pLight.mColor);
	OGL_ASSERT();
}

void OpenGLRenderer::RemoveLight(LightID pLightID)
{
	Parent::RemoveLight(pLightID);
	GLenum lLight = (int)pLightID;
	glDisable(GL_LIGHT0+lLight);
	OGL_ASSERT();
}



void OpenGLRenderer::SetLightPosition(LightID pLightID, float pX, float pY, float pZ)
{
	Parent::SetLightPosition(pLightID, pX, pY, pZ);
	int lLightIndex = (int)pLightID;

	if (lLightIndex == INVALID_LIGHT)
		return;

	LightData& lLightData = GetLightData(lLightIndex);
	if (lLightData.mType == LIGHT_POINT ||
	   lLightData.mType == LIGHT_SPOT)
	{
		float lVector[4];
		lVector[0] = pX;
		lVector[1] = pY;
		lVector[2] = pZ;
		lVector[3] = 1.0f;

		GLenum lLight = GL_LIGHT0 + lLightIndex;
		glLightfv(lLight, GL_POSITION, lVector);
	}
	OGL_ASSERT();
}



void OpenGLRenderer::SetLightDirection(LightID pLightID, float pX, float pY, float pZ)
{
	Parent::SetLightDirection(pLightID, pX, pY, pZ);

	int lLightIndex = (int)pLightID;

	if (lLightIndex == INVALID_LIGHT)
		return;

	LightData& lLightData = GetLightData(lLightIndex);
	if (lLightData.mType == LIGHT_DIRECTIONAL ||
	   lLightData.mType == LIGHT_SPOT)
	{
		GLenum lLight = GL_LIGHT0 + lLightIndex;

		if (lLightData.mType == LIGHT_DIRECTIONAL)
		{
			float lVector[4];
			lVector[0] = -pX;
			lVector[1] = -pY;
			lVector[2] = -pZ;
			lVector[3] = 0.0f;
			glLightfv(lLight, GL_POSITION, lVector);
		}
		else
		{
			float lVector[3];
			lVector[0] = pX;
			lVector[1] = pY;
			lVector[2] = pZ;
			glLightfv(lLight, GL_SPOT_DIRECTION, lVector);
		}
	}
	OGL_ASSERT();
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

	for (int i = 0; i < GetLightCount(); i++)
	{
		int lLightIndex = GetLightIndex(i);
		LightData& lLightData = GetLightData(lLightIndex);

		if (lLightData.mEnabled == true)
		{
			const float lRed   = lLightData.mColor[0] * pRed;
			const float lGreen = lLightData.mColor[1] * pGreen;
			const float lBlue  = lLightData.mColor[2] * pBlue;

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

			GLenum lLight = GL_LIGHT0 + lLightIndex;
			glLightfv(lLight, GL_DIFFUSE, lDiffuse);
			glLightfv(lLight, GL_SPECULAR, lSpecular);
			glLightfv(lLight, GL_AMBIENT, lAmbient);
		}
	}
	OGL_ASSERT();
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
			assert(false);
			return 0;
	};
}

void OpenGLRenderer::BindMap(int pMapType, TextureData* pTextureData, Texture* pTexture)
{
	assert(pMapType >= 0 && pMapType < Texture::NUM_MAPS);

	bool lCompress = UiLepra::OpenGLExtensions::IsCompressedTexturesSupported() &&
					GetCompressedTexturesEnabled();

	if (pTextureData->mTMapID[pMapType] == mTMapIDManager.GetInvalidId())
	{
		pTextureData->mTMapID[pMapType] = mTMapIDManager.GetFreeId();
	}

	glBindTexture(GL_TEXTURE_2D, pTextureData->mTMapID[pMapType]);

	int lSize = GetMap(pMapType, 0, pTexture)->GetPixelByteSize();
	assert(lSize == 1 || lSize == 3 || lSize == 4);
	
	GLenum lPixelFormat;
	SetPixelFormat(lSize, lPixelFormat, lCompress, 
		strutil::Format(_T("AddTexture() - the texture has an invalid pixel size of %i bytes!"), lSize));

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
	}
	OGL_ASSERT();
}

void OpenGLRenderer::BindCubeMap(TextureData* pTextureData, Texture* pTexture)
{
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
	OGL_ASSERT();
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
		}
	}

	if (pTextureData == GetEnvTexture())
	{
		SetEnvironmentMap(INVALID_TEXTURE);
	}
	//OGL_ASSERT();
}

void OpenGLRenderer::BindGeometry(TBC::GeometryBase* pGeometry,
				  GeometryID /*pID*/,
				  MaterialType pMaterialType)
{
	OGL_ASSERT();

	// A hard coded check to make life easier for the user.
	if (pMaterialType == MAT_TEXTURE_SBMAP_PXS ||
	    pMaterialType == MAT_TEXTURE_AND_DIFFUSE_BUMPMAP_PXS)
	{
		pGeometry->GenerateTangentAndBitangentData();
	}

	if (pGeometry->IsGeometryReference() == false)
	{
		if (UiLepra::OpenGLExtensions::IsBufferObjectsSupported() == true)
		{
			OGLGeometryData* lGeometryData = (OGLGeometryData*)pGeometry->GetRendererData();

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
				if (pGeometry->GetColorFormat() == TBC::GeometryBase::COLOR_RGB)
					lSize = 3;

				lBufferSize += lVertexCount * sizeof(unsigned char) * lSize;
			}
			if (pGeometry->GetUVSetCount() > 0)
			{
				lBufferSize += (lVertexCount * sizeof(float) * 2) * pGeometry->GetUVSetCount();
			}
			if (pGeometry->GetTangentData() != 0)
			{
				// Assume that we have bitangent data as well.
				lBufferSize += lVertexCount * sizeof(float) * 6;
			}

			// Set the most appropriate buffer object hint.
			GLenum lGLHint = GL_DYNAMIC_DRAW;
			switch(pGeometry->GetGeometryVolatility())
			{
			case TBC::GeometryBase::GEOM_STATIC:
			case TBC::GeometryBase::GEOM_SEMI_STATIC:
				lGLHint = GL_STATIC_DRAW;
				break;
			case TBC::GeometryBase::GEOM_DYNAMIC:
				lGLHint = GL_DYNAMIC_DRAW;
				break;
			case TBC::GeometryBase::GEOM_VOLATILE:
				lGLHint = GL_STREAM_DRAW;
				break;
			}
			
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
				if (pGeometry->GetColorFormat() == TBC::GeometryBase::COLOR_RGB)
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
				for (unsigned i = 0; i < pGeometry->GetUVSetCount(); i++)
				{
					UiLepra::OpenGLExtensions::glBufferSubData(GL_ARRAY_BUFFER, 
										 lOffset,
										 lVertexCount * sizeof(float) * 2,
										 (void*)pGeometry->GetUVData(i));
					lOffset += lVertexCount * sizeof(float) * 2;
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
							      pGeometry->GetMaxIndexCount() * sizeof(unsigned int),
							      0, lGLHint);

			UiLepra::OpenGLExtensions::glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,
								 0,
								 pGeometry->GetIndexCount() * sizeof(unsigned int),
								 (void*)pGeometry->GetIndexData());
		}
	}

	OGL_ASSERT();
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
		switch(pShadowVolume->GetGeometryVolatility())
		{
		case TBC::GeometryBase::GEOM_STATIC:
		case TBC::GeometryBase::GEOM_SEMI_STATIC:
			if (pLightHint == LIGHT_MOVABLE)
				lGLHint = GL_STREAM_DRAW;
			else
				lGLHint = GL_STATIC_DRAW;
			break;
		case TBC::GeometryBase::GEOM_DYNAMIC:
			if (pLightHint == LIGHT_MOVABLE)
				lGLHint = GL_STREAM_DRAW;
			else
				lGLHint = GL_DYNAMIC_DRAW;
			break;
		case TBC::GeometryBase::GEOM_VOLATILE:
			lGLHint = GL_STREAM_DRAW;
			break;
		}
		
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
						      pShadowVolume->GetMaxIndexCount() * sizeof(unsigned int),
						      pShadowVolume->GetIndexData(), lGLHint);

		lOK = true;
	}

	OGL_ASSERT();
	return lOK;
}

void OpenGLRenderer::UpdateGeometry(GeometryID pGeometryID)
{
	GeometryTable::Iterator lIter;
	lIter = GetGeometryTable().Find(pGeometryID);

	if (lIter != GetGeometryTable().End())
	{
		OGLGeometryData* lGeomData = (OGLGeometryData*)*lIter;
		TBC::GeometryBase* lGeometry = lGeomData->mGeometry;
		
		// Force update of shadow volumes.

		if (UiLepra::OpenGLExtensions::IsBufferObjectsSupported() == true && lGeometry->IsGeometryReference() == false)
		{
			int lVertexCount = lGeometry->GetVertexCount();

			if (lGeometry->GetVertexDataChanged() == true ||
			   lGeometry->GetColorDataChanged() == true ||
			   lGeometry->GetUVDataChanged() == true)
			{
				//log_volatile(mLog.Tracef(_T("glBindBuffer %u (vertex)."), lGeomData->mVertexBufferID));
				UiLepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, (GLuint)lGeomData->mVertexBufferID);
			}

			if (lGeometry->GetVertexDataChanged() == true)
			{
				UiLepra::OpenGLExtensions::glBufferSubData(GL_ARRAY_BUFFER,
									 0,
									 lVertexCount * sizeof(float) * 3,
									 (void*)lGeometry->GetVertexData());

				UiLepra::OpenGLExtensions::glBufferSubData(GL_ARRAY_BUFFER,
									 lGeomData->mNormalOffset,
									 lVertexCount * sizeof(float) * 3,
									 (void*)lGeometry->GetNormalData());

				// Only reset the flag if there are no shadows to update.
				// The flag will be reset when the shadows are updated.
				if (lGeomData->mShadow <= NO_SHADOWS)
				{
					lGeometry->SetVertexDataChanged(false);
				}
			}

			if (lGeometry->GetColorDataChanged() == true)
			{
				int lSize = 4;
				if (lGeometry->GetColorFormat() == TBC::GeometryBase::COLOR_RGB)
					lSize = 3;

				UiLepra::OpenGLExtensions::glBufferSubData(GL_ARRAY_BUFFER, 
									 lGeomData->mColorOffset,
									 lVertexCount * sizeof(unsigned char) * lSize,
									 (void*)lGeometry->GetColorData());
			}

			if (lGeometry->GetUVDataChanged() == true)
			{
				size_t lOffset = lGeomData->mUVOffset;
				for (unsigned i = 0; i < lGeometry->GetUVSetCount(); i++)
				{
					UiLepra::OpenGLExtensions::glBufferSubData(GL_ARRAY_BUFFER, lOffset,
						lVertexCount * sizeof(float) * 2, (void*)lGeometry->GetUVData(i));
					lOffset += lVertexCount * sizeof(float) * 2;
				}
			}

			if (lGeometry->GetTangentData() != 0 &&
			   (lGeometry->GetVertexDataChanged() == true ||
			    lGeometry->GetUVDataChanged() == true))
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

			if (lGeometry->GetIndexDataChanged() == true)
			{
				UiLepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (GLuint)lGeomData->mIndexBufferID);
				UiLepra::OpenGLExtensions::glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0,
					lGeometry->GetTriangleCount() * 3 * sizeof(unsigned int),
					(void*)lGeometry->GetIndexData());
			}
		}
	}
	OGL_ASSERT();
}

void OpenGLRenderer::ReleaseGeometry(TBC::GeometryBase* pUserGeometry, GeomReleaseOption pOption)
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
			assert(false);
		}
	}
	//OGL_ASSERT();
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
	OGL_ASSERT();
	return (lOk);
}



bool OpenGLRenderer::PreRender(TBC::GeometryBase* pGeometry)
{
	if (pGeometry->IsSimpleObject() || CheckCamCulling(pGeometry->GetTransformation().GetPosition(), pGeometry->GetBoundingRadius()))
	{
		mVisibleTriangleCount += pGeometry->GetTriangleCount();
		// Transform the geometry.
		/*TransformationF t = GetCameraTransformation();
		t.GetOrientation().Div(t.GetOrientation().GetMagnitude());
		SetCameraTransformation(t);*/
		//mCamSpaceTransformation.FastInverseTransform(mCameraTransformation, mCameraOrientationInverse, pGeometry->GetTransformation());
		mCamSpaceTransformation = mCameraTransformation.InverseTransform(pGeometry->GetTransformation());
		/*TransformationF lCamSpaceTransformation = GetCameraTransformation().InverseTransform(pGeometry->GetTransformation());
		assert(mCamSpaceTransformation.GetPosition().GetDistanceSquared(lCamSpaceTransformation.GetPosition()) < 1e-8);
		assert((mCamSpaceTransformation.GetOrientation() - lCamSpaceTransformation.GetOrientation()).GetNorm() < 1e-8);*/
		float lModelViewMatrix[16];
		mCamSpaceTransformation.GetAs4x4TransposeMatrix(lModelViewMatrix);
		const float lScale = pGeometry->GetScale();
		if (lScale != 1)
		{
			lModelViewMatrix[0]  *= lScale;
			lModelViewMatrix[1]  *= lScale;
			lModelViewMatrix[2]  *= lScale;
			lModelViewMatrix[4]  *= lScale;
			lModelViewMatrix[5]  *= lScale;
			lModelViewMatrix[6]  *= lScale;
			lModelViewMatrix[8]  *= lScale;
			lModelViewMatrix[9]  *= lScale;
			lModelViewMatrix[10] *= lScale;
		}
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(lModelViewMatrix);
		OGL_ASSERT();
		return true;
	}
	else
	{
		mCulledTriangleCount += pGeometry->GetTriangleCount();
	}
	return false;
}

void OpenGLRenderer::PostRender(TBC::GeometryBase* pGeometry)
{
	pGeometry->SetTransformationChanged(false);
}



void OpenGLRenderer::DrawLine(const Vector3DF& pPosition, const Vector3DF& pVector, const Color& pColor)
{
	glEnable(GL_DEPTH_TEST);
	TransformationF lCamTransform = GetCameraTransformation().InverseTransform(TransformationF());
	float lModelViewMatrix[16];
	lCamTransform.GetAs4x4TransposeMatrix(lModelViewMatrix);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(lModelViewMatrix);

	glColor3ub(pColor.mRed, pColor.mGreen, pColor.mBlue);
	glBegin(GL_LINES);
	glVertex3f(pPosition.x, pPosition.y, pPosition.z);
	glVertex3f(pPosition.x+pVector.x, pPosition.y+pVector.y, pPosition.z+pVector.z);
	glEnd();

	OGL_ASSERT();
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

		::glPushAttrib(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT | GL_VIEWPORT_BIT);

		::glDisable(GL_COLOR_LOGIC_OP);
		::glDisable(GL_ALPHA_TEST);
		::glDisable(GL_BLEND);
		::glEnable(GL_CULL_FACE);
		::glEnable(GL_DEPTH_TEST);
		::glDepthMask(GL_TRUE);
		::glFrontFace(GL_CCW);
		::glShadeModel(GL_SMOOTH);
		::glPolygonMode(GL_FRONT_AND_BACK, IsWireframeEnabled()? GL_LINE : GL_FILL);
		::glDepthFunc(GL_LESS);
		::glCullFace(GL_BACK);
		::glLineWidth(3.0f);
		::glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		::glEnable(GL_LINE_SMOOTH);

		::glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);

		Material::EnableDrawMaterial(true);
	}

	float lAmbientRed, lAmbientGreen, lAmbientBlue;
	GetAmbientLight(lAmbientRed, lAmbientGreen, lAmbientBlue);

	if (GetShadowMode() != NO_SHADOWS && GetLightsEnabled())
	{
		UpdateShadowMaps();

		// Disable all lights (for shadow rendering).
		for (int i = 0; i < GetLightCount(); ++i)
		{
			::glDisable(GL_LIGHT0 + GetLightIndex(i));
		}

		if (IsOutlineRenderingEnabled())
		{
			SetAmbientLight(-10, -10, -10);
		}

		// Prepare the pixel shader materials.
		OpenGLMatPXS::PrepareLights(this);

		// Render the scene darkened.
		for (int i = 0; i <= (int)MAT_LAST_SOLID; i++)
		{
			if (GetMaterial((MaterialType)i) != 0)
			{
				Material::RenderAllGeometry(GetCurrentFrame(), GetMaterial((MaterialType)i));
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
		::glEnable(GL_STENCIL_TEST);
		::glStencilFunc(GL_GEQUAL, 128, 0xFF);
		::glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

		// Enable all lights again.
		for (int i = 0; i < GetLightCount(); ++i)
		{
			::glEnable(GL_LIGHT0 + GetLightIndex(i));
		}
	}

	bool lSkipOutlined = false;
	if (IsOutlineRenderingEnabled() && !IsWireframeEnabled())
	{
		Material::EnableDrawMaterial(false);
		SetAmbientLight(1, 1, 1);
		const Vector3DF lColor(mOutlineFillColor.GetRf(), mOutlineFillColor.GetGf(), mOutlineFillColor.GetBf());
		TBC::GeometryBase::BasicMaterialSettings lMaterial(Vector3DF(1, 1, 1), lColor, Vector3DF(), 1, 1, false);
		OpenGLMaterial::SetBasicMaterial(lMaterial, this);
		Material::RenderAllGeometry(GetCurrentFrame(), GetMaterial(MAT_SINGLE_COLOR_SOLID));
		Material::RenderAllGeometry(GetCurrentFrame(), GetMaterial(MAT_SINGLE_COLOR_SOLID_PXS), GetMaterial(MAT_SINGLE_COLOR_SOLID));
		Material::RenderAllGeometry(GetCurrentFrame(), GetMaterial(MAT_SINGLE_COLOR_OUTLINE_BLENDED));
		::glCullFace(GL_FRONT);
		::glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		::glDepthFunc(GL_LEQUAL);
		::glDisable(GL_LIGHTING);
		Material::EnableDrawMaterial(true);
		SetAmbientLight(lAmbientRed, lAmbientGreen, lAmbientBlue);
		Material::RenderAllGeometry(GetCurrentFrame(), GetMaterial(MAT_SINGLE_COLOR_SOLID));
		Material::RenderAllGeometry(GetCurrentFrame(), GetMaterial(MAT_SINGLE_COLOR_SOLID_PXS), GetMaterial(MAT_SINGLE_COLOR_SOLID));
		Material::RenderAllGeometry(GetCurrentFrame(), GetMaterial(MAT_SINGLE_COLOR_OUTLINE_BLENDED));
		::glCullFace(GL_BACK);
		::glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		//::glDepthFunc(GL_LESS);
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
				i == MAT_SINGLE_COLOR_SOLID_PXS))
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
	}

	{
		for (int i = 0; i < GetLightCount(); i++)
		{
			LightData& lLight = GetLightData(GetLightIndex(i));
			lLight.mTransformationChanged = false;
		}

		StepCurrentFrame();

		::glPopAttrib();
	}

	OGL_ASSERT();

	return (GetCurrentFrame());
}

void OpenGLRenderer::RenderRelative(TBC::GeometryBase* pGeometry, const QuaternionF* pLightOrientation)
{
	OGL_ASSERT();

	::glMatrixMode(GL_MODELVIEW);
	::glPushMatrix();
	float lModelViewMatrix[16];
	pGeometry->GetTransformation().GetAs4x4TransposeMatrix(lModelViewMatrix);
	::glLoadMatrixf(lModelViewMatrix);

	::glEnable(GL_DEPTH_TEST);

	const int lLightIndex = GetLightIndex(0);
	const LightData& lLightData = GetLightData(lLightIndex);
	if (pLightOrientation)
	{
		::glEnable(GL_LIGHTING);
		QuaternionF lOrientation = pGeometry->GetTransformation().GetOrientation().GetInverse();
		LightData lDataCopy = lLightData;
		lOrientation *= *pLightOrientation;
		lDataCopy.mDirection = lOrientation * lLightData.mDirection;
		SetupGLLight(lLightIndex, lDataCopy);
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
		SetupGLLight(lLightIndex, lLightData);
	}

	PostRender(pGeometry);

	::glDisable(GL_LIGHTING);
	::glDisable(GL_DEPTH_TEST);

	::glMatrixMode(GL_MODELVIEW);
	::glPopMatrix();

	OGL_ASSERT();
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
	int i;

	if (GetLightsEnabled() == true)
	{
		glEnable(GL_LIGHTING);
	}
	else
	{
		glDisable(GL_LIGHTING);
	}
	
	// Transform all light positions.
	for (i = 0; i < GetLightCount(); i++)
	{
		LightData& lLight = GetLightData(GetLightIndex(i));

		if (lLight.mType == LIGHT_DIRECTIONAL)
		{
			Vector3DF lLightDir = GetCameraTransformation().GetOrientation().GetInverseRotatedVector(lLight.mDirection);
			float lPos[4] =
			{
				(float)-lLightDir.x,
				(float)-lLightDir.y,
				(float)-lLightDir.z,
				0.0f
			};

			glLightfv(GL_LIGHT0 + GetLightIndex(i), GL_POSITION, lPos);
		}
		else if(lLight.mType == LIGHT_POINT)
		{
			Vector3DF lLightPos = GetCameraTransformation().InverseTransform(lLight.mPosition);
			float lPos[4] =
			{
				(float)lLightPos.x,
				(float)lLightPos.y,
				(float)lLightPos.z,
				1.0f
			};

			glLightfv(GL_LIGHT0 + GetLightIndex(i), GL_POSITION, lPos);
		}
		else if(lLight.mType == LIGHT_SPOT)
		{
			Vector3DF lLightPos = GetCameraTransformation().InverseTransform(lLight.mPosition);
			Vector3DF lLightDir = GetCameraTransformation().GetOrientation().GetInverseRotatedVector(lLight.mDirection);
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

			glLightfv(GL_LIGHT0 + GetLightIndex(i), GL_POSITION, lPos);
			glLightfv(GL_LIGHT0 + GetLightIndex(i), GL_SPOT_DIRECTION, lDir);

			if (GetShadowHint() == SH_VOLUMES_AND_MAPS)
			{
				/*
					Update shadow map.
				*/
				if (lLight.mTransformationChanged == true)
				{
					lLight.mShadowMapNeedUpdate = true;
					lLight.mTransformationChanged = false;
				}

				if (lLight.mShadowMapNeedUpdate == true)
					RegenerateShadowMap(&lLight);
			}
		}
	}
	OGL_ASSERT();
}

void OpenGLRenderer::RenderShadowVolumes()
{
	OGL_ASSERT();

	// Disable all fancy gfx.
	glPushAttrib(GL_ENABLE_BIT | GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_NORMALIZE);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_LOGIC_OP);
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
//	glEnable(GL_POLYGON_OFFSET_EXT);
//	glPolygonOffset(0.5f, 0);


	glMatrixMode(GL_MODELVIEW);

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
				float lModelViewMatrix[16];
				(GetCameraTransformation().InverseTransform(lShadowVolume->GetTransformation())).GetAs4x4TransposeMatrix(lModelViewMatrix);
				const float lScale = lShadowVolume->GetScale();
				if (lScale != 1)
				{
					lModelViewMatrix[0]  *= lScale;
					lModelViewMatrix[1]  *= lScale;
					lModelViewMatrix[2]  *= lScale;
					lModelViewMatrix[4]  *= lScale;
					lModelViewMatrix[5]  *= lScale;
					lModelViewMatrix[6]  *= lScale;
					lModelViewMatrix[8]  *= lScale;
					lModelViewMatrix[9]  *= lScale;
					lModelViewMatrix[10] *= lScale;
				}
				glLoadMatrixf(lModelViewMatrix);

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
						UiLepra::OpenGLExtensions::glBufferSubData(GL_ARRAY_BUFFER,
															0,
															lShadowVolume->GetVertexCount() * sizeof(float) * 3,
															(void*)lShadowVolume->GetVertexData());
						lShadowVolume->SetVertexDataChanged(false);
					}

					UiLepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lIndexBufferID);
					
					if (lShadowVolume->GetIndexDataChanged() == true)
					{
						UiLepra::OpenGLExtensions::glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,
															0,
															lShadowVolume->GetTriangleCount() * 3 * sizeof(unsigned int),
															(void*)lShadowVolume->GetIndexData());
					}

					glDrawElements(GL_TRIANGLES,
								lShadowVolume->GetTriangleCount() * 3,
								GL_UNSIGNED_INT,
								0);
				}
				else
				{
					glVertexPointer(3, GL_FLOAT, 0, lShadowVolume->GetVertexData());
					glDrawElements(GL_TRIANGLES,
								lShadowVolume->GetTriangleCount() * 3,
								GL_UNSIGNED_INT,
								lShadowVolume->GetIndexData());
				}
			}
		}
	}	

	// Reset all settings.
	glPopAttrib();
	glShadeModel(GL_SMOOTH);
	glColorMask(1, 1, 1, 1);
	glDepthFunc(GL_LEQUAL);
//	glPolygonOffset(0, 0);
//	glDisable(GL_POLYGON_OFFSET_EXT);

	OGL_ASSERT();
}

int OpenGLRenderer::RenderShadowMaps()
{
	OGL_ASSERT();

	glPushAttrib(GL_ENABLE_BIT | GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT | GL_VIEWPORT_BIT);
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_NORMALIZE);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_LOGIC_OP);
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

	int i;
	int lCount = 0;
	for (i = 0; i < GetLightCount(); i++)
	{
		LightData& lLight = GetLightData(GetLightIndex(i));

		if (lLight.mType == LIGHT_SPOT && lLight.mShadowMapID != 0)
		{
			glBindTexture(GL_TEXTURE_2D, lLight.mShadowMapID);

			// Prepare the texture projection matrix.
			static float slTextureMatrix[16];
			glMatrixMode(GL_TEXTURE);
			glLoadIdentity();
			glTranslatef(0.5f, 0.5f, 0.5f);
			glScalef(0.5f, 0.5f, 0.5f);
			glMultMatrixf(lLight.mLightProjectionMatrix);
			glGetFloatv(GL_TEXTURE_MATRIX, slTextureMatrix);

			LightData::GeometrySet::Iterator lGeoIter;
			for (lGeoIter = lLight.mShadowMapGeometrySet.First();
				lGeoIter != lLight.mShadowMapGeometrySet.End();
				++lGeoIter)
			{
				OGLGeometryData* lGeometry = (OGLGeometryData*)*lGeoIter;

				float lModelViewMatrix[16];
				(GetCameraTransformation().InverseTransform(lGeometry->mGeometry->GetTransformation())).GetAs4x4TransposeMatrix(lModelViewMatrix);

				float lLightModelViewMatrix[16];
				TransformationF lLightTransformation(lLight.mOrientation, lLight.mPosition);
				(lLightTransformation.InverseTransform(lGeometry->mGeometry->GetTransformation())).GetAs4x4TransposeMatrix(lLightModelViewMatrix);

				// Camera model view matrix.
				glMatrixMode(GL_MODELVIEW);
				glLoadMatrixf(lModelViewMatrix);

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
								GL_UNSIGNED_INT,
								0);
				}
				else
				{
					glVertexPointer(3, GL_FLOAT, 0, lGeometry->mGeometry->GetVertexData());
					glDrawElements(GL_TRIANGLES,
								lGeometry->mGeometry->GetTriangleCount() * 3,
								GL_UNSIGNED_INT,
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

	glPopAttrib();
	glColorMask(1, 1, 1, 1);

	glDisable(GL_TEXTURE_2D);

	OGL_ASSERT();

	return lCount;
}

void OpenGLRenderer::RegenerateShadowMap(LightData* pLight)
{
	glPushAttrib(GL_ENABLE_BIT | GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT | GL_VIEWPORT_BIT);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_NORMALIZE);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_LOGIC_OP);
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


	TransformationF lLightTransformation(pLight->mOrientation, pLight->mPosition);
    
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
							GL_UNSIGNED_INT,
							0);
			}
			else
			{
				glVertexPointer(3, GL_FLOAT, 0, lGeometry->mGeometry->GetVertexData());
				glDrawElements(GL_TRIANGLES,
							lGeometry->mGeometry->GetTriangleCount() * 3,
							GL_UNSIGNED_INT,
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

	// And finally read it from the back buffer.
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
					 0, 0, pLight->mShadowMapRes, pLight->mShadowMapRes, 0);

	glDisable(GL_POLYGON_OFFSET_FILL);
	glPopAttrib();
	glShadeModel(GL_SMOOTH);
	glColorMask(1, 1, 1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	pLight->mShadowMapNeedUpdate = false;

	OGL_ASSERT();
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

	const float lDX = 1.0f / tan(Math::Deg2Rad(pFOVAngle) / 2.0f);
	const float lDY = lDX * pAspectRatio;

	float lProjectionMatrix[16];

	lProjectionMatrix[0]  = lDX;
	lProjectionMatrix[1]  = 0;
	lProjectionMatrix[2]  = 0;
	lProjectionMatrix[3]  = 0;

	lProjectionMatrix[4]  = 0;
	lProjectionMatrix[5]  = 0;
	lProjectionMatrix[6]  = (pFar + pNear) / (pFar - pNear);
	lProjectionMatrix[7]  = 1;

	lProjectionMatrix[8]  = 0;
	lProjectionMatrix[9]  = lDY;
	lProjectionMatrix[10] = 0;
	lProjectionMatrix[11] = 0;

	lProjectionMatrix[12] = 0;
	lProjectionMatrix[13] = 0;
	lProjectionMatrix[14] = -(2.0f * pFar * pNear) / (pFar - pNear);
	lProjectionMatrix[15] = 0;

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(lProjectionMatrix);

	glMatrixMode(GL_MODELVIEW);

	OGL_ASSERT();
}

void OpenGLRenderer::SetPixelFormat(int& pSize, GLenum& pPixelFormat, bool pCompress, const str& pErrorMessage)
{
	switch(pSize)
	{
	case 4:
		pPixelFormat = GL_RGBA;
		break;
	case 3: 
		pPixelFormat = GL_RGB;
		break;
	case 1:
		pPixelFormat = GL_LUMINANCE;
		break;
	default:
		pPixelFormat = GL_RGB;
		mLog.Info(pErrorMessage);
	}

	if (pCompress == true)
	{
		pSize = (pSize == 4) ? GL_COMPRESSED_RGBA : GL_COMPRESSED_RGB;
	}
}



LOG_CLASS_DEFINE(UI_GFX_3D, OpenGLRenderer);



}
