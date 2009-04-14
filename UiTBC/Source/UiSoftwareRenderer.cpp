/*
	Lepra::File:   SoftwareRenderer.cpp
	Class:  SoftwareRenderer
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games
*/

#include "../../TBC/Include/Bones.h"
#include "../../TBC/Include/../../TBC/Include/GeometryBase.h"
#include "../Include/UiSoftwareMaterials.h"
#include "../Include/UiSoftwareRenderer.h"
#include "../Include/UiTexture.h"

namespace UiTbc
{

Material* SoftwareRenderer::CreateMaterial(Renderer::MaterialType pMaterialType)
{
	switch(pMaterialType)
	{
	case MAT_NULL:
		return new NullMaterial(this);
	case MAT_VERTEX_COLOR_SOLID:
	case MAT_VERTEX_COLOR_BLENDED:
		return new SoftMatVertexColorSolid(this, &m3DPainter, GetMaterial(MAT_SINGLE_COLOR_SOLID));
	case MAT_SINGLE_TEXTURE_SOLID:
	case MAT_SINGLE_TEXTURE_SOLID_PXS:
	case MAT_TEXTURE_AND_LIGHTMAP:
	case MAT_TEXTURE_AND_LIGHTMAP_PXS:
	case MAT_TEXTURE_AND_DIFFUSE_BUMPMAP_PXS:
	case MAT_TEXTURE_SBMAP_PXS:
	case MAT_SINGLE_TEXTURE_BLENDED:
	case MAT_SINGLE_TEXTURE_ALPHATESTED:
		return new SoftMatSingleTextureSolid(this, &m3DPainter, GetMaterial(MAT_SINGLE_COLOR_SOLID));
	case MAT_SINGLE_COLOR_SOLID:
	case MAT_SINGLE_COLOR_SOLID_PXS:
	case MAT_SINGLE_COLOR_BLENDED:
	default:
		return new SoftMatSingleColorSolid(this, &m3DPainter, 0);
	}
}


SoftwareRenderer::SoftwareRenderer(Lepra::Canvas* pScreen, float pMipMapFactor) :
	Renderer(pScreen)
{
	Renderer::InitRenderer();

	Lepra::RotationMatrixF lRotMtx(1, 0, 0,
					0, 0, 1,
					0, 1, 0);

	m3DPainter.SetDefaultTransform(lRotMtx, Lepra::Vector3DF(0,0,0));
	m3DPainter.SetFrontFace(Software3DPainter::FACE_CCW);

	m3DPainter.SetDestCanvas(pScreen);
	m3DPainter.SetRenderingHint(Software3DPainter::RENDER_FAST);
	m3DPainter.SetZTestEnabled(true);
	m3DPainter.SetZWriteEnabled(true);
	m3DPainter.SetVertexColorsEnabled(true);
	m3DPainter.SetTextureMappingEnabled(true);
	m3DPainter.SetMipMappingEnabled(true);
	m3DPainter.SetLightModel(Software3DPainter::LIGHT_GOURAUD);
	m3DPainter.SetMipMapReferenceFactor(pMipMapFactor);
}

SoftwareRenderer::~SoftwareRenderer()
{
	Renderer::CloseRenderer();
}

void SoftwareRenderer::Clear(unsigned int pClearFlags)
{
	if (Renderer::CheckFlag(pClearFlags, CLEAR_COLORBUFFER) == true)
		m3DPainter.ClearColorBuffer();

	if (Renderer::CheckFlag(pClearFlags, CLEAR_DEPTHBUFFER) == true)
		m3DPainter.ClearZBuffer();
}

void SoftwareRenderer::SetClearColor(const Lepra::Color& pColor)
{
	m3DPainter.SetClearColor(pColor);
}

void SoftwareRenderer::SetViewport(const Lepra::PixelRect& pViewport)
{
	Renderer::SetViewport(pViewport);
	m3DPainter.SetViewport(pViewport.mLeft, pViewport.mTop, pViewport.mRight, pViewport.mBottom);
}

void SoftwareRenderer::SetViewFrustum(float pFOVAngle, float pNearZ, float pFarZ)
{
	Renderer::SetViewFrustum(pFOVAngle, pNearZ, pFarZ);
	m3DPainter.SetViewFrustum(pFOVAngle, pNearZ, pFarZ);
}

void SoftwareRenderer::SetClippingRect(const Lepra::PixelRect& pRect)
{
	Renderer::SetClippingRect(pRect);
	m3DPainter.SetClippingRect(pRect.mLeft, pRect.mTop, pRect.mRight, pRect.mBottom);
}

void SoftwareRenderer::ResetClippingRect()
{
	m3DPainter.ResetClippingRect();
	Renderer::ResetClippingRect();
}

void SoftwareRenderer::SetCameraTransformation(const Lepra::TransformationF& pTransformation)
{
	Renderer::SetCameraTransformation(pTransformation);
	m3DPainter.SetCameraTransform(pTransformation);
}

void SoftwareRenderer::SetLightsEnabled(bool pEnabled)
{
	Renderer::SetLightsEnabled(pEnabled);
	m3DPainter.SetLightsEnabled(pEnabled);
}

void SoftwareRenderer::SetMipMappingEnabled(bool pEnabled)
{
	Renderer::SetMipMappingEnabled(pEnabled);
	m3DPainter.SetMipMappingEnabled(pEnabled);
}

void SoftwareRenderer::SetBilinearFilteringEnabled(bool pEnabled)
{
	Renderer::SetBilinearFilteringEnabled(pEnabled);
	m3DPainter.SetBilinearFilteringEnabled(pEnabled);
}

void SoftwareRenderer::SetTrilinearFilteringEnabled(bool pEnabled)
{
	Renderer::SetTrilinearFilteringEnabled(pEnabled);
	if (pEnabled == true)
	{
		m3DPainter.SetMipMappingEnabled(pEnabled);
		m3DPainter.SetBilinearFilteringEnabled(pEnabled);
	}
	m3DPainter.SetTrilinearFilteringEnabled(pEnabled);
}


void SoftwareRenderer::SetDepthWriteEnabled(bool pEnabled)
{
	m3DPainter.SetZWriteEnabled(pEnabled);
}

void SoftwareRenderer::SetDepthTestEnabled(bool pEnabled)
{
	m3DPainter.SetZTestEnabled(pEnabled);
}

void SoftwareRenderer::SetAmbientLight(float pRed, float pGreen, float pBlue)
{
	Renderer::SetAmbientLight(pRed, pGreen, pBlue);
	m3DPainter.SetAmbientLight(pRed, pGreen, pBlue);
}

void SoftwareRenderer::AddAmbience(float pRed, float pGreen, float pBlue)
{
	Renderer::AddAmbience(pRed, pGreen, pBlue);
	Renderer::GetAmbientLight(pRed, pGreen, pBlue);
	m3DPainter.SetAmbientLight(pRed, pGreen, pBlue);
}

Renderer::LightID SoftwareRenderer::AddDirectionalLight(LightHint pHint, float pDirX, float pDirY,
	float pDirZ, float pRed, float pGreen, float pBlue, float pShadowRange)
{
	Renderer::LightID lLightID = Renderer::AddDirectionalLight(pHint, pDirX, pDirY, pDirZ, pRed, pGreen, pBlue, pShadowRange);

	m3DPainter.SetLight((int)lLightID, Software3DPainter::LIGHT_DIRECTIONAL, 
		pDirX, pDirY, pDirZ, pRed, pGreen, pBlue);

	return lLightID;
}

Renderer::LightID SoftwareRenderer::AddPointLight(LightHint pHint, float pPosX, float pPosY, float pPosZ,
	float pRed, float pGreen, float pBlue, float pLightRadius, float pShadowRange)
{
	Renderer::LightID lLightID = Renderer::AddPointLight(pHint, pPosX, pPosY, pPosZ, pRed, pGreen, pBlue, pLightRadius, pShadowRange);

	m3DPainter.SetLight((int)lLightID, Software3DPainter::LIGHT_POINT, 
		pPosX, pPosY, pPosZ, pRed, pGreen, pBlue);

	return lLightID;
}

Renderer::LightID SoftwareRenderer::AddSpotLight(LightHint pHint, float pPosX, float pPosY, float pPosZ,
	float pDirX, float pDirY, float pDirZ, float pRed, float pGreen, float pBlue,
	float pCutoffAngle, float pSpotExponent, float pLightRadius, float pShadowRange)
{
	Renderer::LightID lLightID = Renderer::AddSpotLight(pHint, pPosX, pPosY, pPosZ, pDirX, pDirY, pDirZ, pRed, pGreen, pBlue, pCutoffAngle, pSpotExponent, pLightRadius, pShadowRange);

	m3DPainter.SetLight((int)lLightID, Software3DPainter::LIGHT_POINT, 
		pPosX, pPosY, pPosZ, pRed, pGreen, pBlue);

	return lLightID;
}

void SoftwareRenderer::RemoveLight(LightID pLightID)
{
	Renderer::RemoveLight(pLightID);
	m3DPainter.SetLight((int)pLightID, Software3DPainter::LIGHT_NONE, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
}

/*int SoftwareRenderer::GetMaxLights()
{
	return m3DPainter.GetMaxLights();
}
*/
void SoftwareRenderer::SetLightPosition(LightID pLightID, float pX, float pY, float pZ)
{
	Renderer::SetLightPosition(pLightID, pX, pY, pZ);
	m3DPainter.SetLightPosition((int)pLightID, pX, pY, pZ);
}

void SoftwareRenderer::SetLightDirection(LightID pLightID, float pX, float pY, float pZ)
{
	Renderer::SetLightDirection(pLightID, pX, pY, pZ);
	m3DPainter.SetLightDirection((int)pLightID, pX, pY, pZ);
}

void SoftwareRenderer::UpdateGeometry(GeometryID /*pGeometryID*/)
{
	// No need to do anything...
}

bool SoftwareRenderer::ChangeMaterial(GeometryID pGeometryID, MaterialType pMaterialType)
{
	assert((int)pMaterialType >= 0 && (int)pMaterialType < Renderer::NUM_MATERIALTYPES);
	GeometryTable::Iterator lIter(GetGeometryTable().Find(pGeometryID));

	bool lOk = false;
	if (lIter != GetGeometryTable().End())
	{
		GeometryData* lGeometryData = *lIter;
		GetMaterial(lGeometryData->mMaterialType)->RemoveGeometry(lGeometryData->mGeometry);
		lGeometryData->mMaterialType = pMaterialType;
		lOk = GetMaterial(pMaterialType)->AddGeometry(lGeometryData->mGeometry);
	}
	return (lOk);
}

bool SoftwareRenderer::PreRender(TBC::GeometryBase* pGeometry)
{
	m3DPainter.SetModelTransform(pGeometry->GetTransformation());

	GeometryData* lGeometryData = (GeometryData*)pGeometry->GetRendererData();

	TextureTable::Iterator lIter = lGeometryData->mTA? mTextureTable.Find(lGeometryData->mTA->mTextureID[0]) : mTextureTable.End();

	TBC::GeometryBase::BasicMaterialSettings lMatSettings;
	pGeometry->GetBasicMaterialSettings(lMatSettings);

	float lRed   = lMatSettings.mRed;
	float lGreen = lMatSettings.mGreen;
	float lBlue  = lMatSettings.mBlue;

	if (lIter != mTextureTable.End())
	{
		m3DPainter.SetTexture((*lIter));
	}

	m3DPainter.SetDefaultColor(lRed, lGreen, lBlue);
	m3DPainter.SetTextureTransform(pGeometry->GetUVTransform());

	return Renderer::CheckCulling(m3DPainter.GetCameraTransform().InverseTransform(pGeometry->GetTransformation()), pGeometry->GetBoundingRadius());
}

void SoftwareRenderer::PostRender(TBC::GeometryBase* /*pGeometry*/)
{
	m3DPainter.SetTexture(0);
}

void SoftwareRenderer::DrawLine(const Lepra::Vector3DF&, const Lepra::Vector3DF&, const Lepra::Color&)
{
}

Renderer::GeometryData* SoftwareRenderer::CreateGeometryData()
{
	return new Renderer::GeometryData();
}

Renderer::TextureData* SoftwareRenderer::CreateTextureData(TextureID pTextureID)
{
	return new Renderer::TextureData(pTextureID, Renderer::INVALID_TEXTURE);
}

void SoftwareRenderer::BindMap(int pMapType, 
			       TextureData* pTextureData,
			       Texture* pTexture)
{
	if (pMapType == Texture::COLOR_MAP)
	{
		TextureTable::Iterator lIter = mTextureTable.Find(pTextureData->mTextureID);
		Texture* lTexture = 0;

		if (lIter == mTextureTable.End())
		{
			lTexture = new Texture(pTexture);
			mTextureTable.Insert(pTextureData->mTextureID, lTexture);
		}
		else
		{
			lTexture = *lIter;
			lTexture->Copy(pTexture);
		}

		if (GetScreen() != 0 && lTexture->GetColorMap(0)->GetBitDepth() != GetScreen()->GetBitDepth())
		{
			lTexture->ConvertBitDepth(GetScreen()->GetBitDepth());
		}
	}
}

void SoftwareRenderer::BindCubeMap(TextureData* /*pTextureData*/,
				   Texture* /*pTexture*/)
{
	// Not supported.
}

void SoftwareRenderer::ReleaseMap(TextureData* pTextureData)
{
	TextureTable::Iterator lIter = mTextureTable.Find(pTextureData->mTextureID);
	if (lIter != mTextureTable.End())
	{
		delete *lIter;
		mTextureTable.Remove(lIter);
	}
}

int SoftwareRenderer::ReleaseShadowMap(int /*pShadowMapID*/)
{
	// Not supported.
	return INVALID_TEXTURE;
}

void SoftwareRenderer::BindGeometry(TBC::GeometryBase* /*pGeometry*/,
				    GeometryID /*pID*/,
				    MaterialType /*pMaterialType*/)
{
	// Do nothing.
}

bool SoftwareRenderer::BindShadowGeometry(UiTbc::ShadowVolume* /*pShadowGeometry*/, LightHint /*pLightHint*/)
{
	// Do nothing.
	return true;
}

void SoftwareRenderer::ReleaseGeometry(TBC::GeometryBase* /*pUserGeometry*/, GeomReleaseOption /*pOption*/)
{
	// Do nothing.
}

}
