/*
	Class:  SoftwareRenderer
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games
*/

#ifndef UISOFTWARERENDERER_H
#define UISOFTWARERENDERER_H

#include "../../Lepra/Include/HashTable.h"
#include "../../Lepra/Include/Vector3D.h"
#include "../../Lepra/Include/RotationMatrix.h"
#include "../../Lepra/Include/Canvas.h"
#include "UiMaterial.h"
#include "UiRenderer.h"
#include "UiSoftware3DPainter.h"
#include "UiTBC.h"

namespace UiTbc
{

class SoftwareRenderer : public Renderer
{
public:
	
	SoftwareRenderer(Lepra::Canvas* pScreen, float pMipMapFactor = 1.0f / 256.0f);
	virtual ~SoftwareRenderer();

	// Overloads from Renderer.

	void Clear(unsigned int pClearFlags = CLEAR_COLORBUFFER | CLEAR_DEPTHBUFFER);

	void SetClearColor(const Lepra::Color& pColor);

	void SetViewport(const Lepra::PixelRect& pViewport);
	void SetViewFrustum(float pFOVAngle, float pNearZ, float pFarZ);

	void SetClippingRect(const Lepra::PixelRect& pRect);
	void ResetClippingRect();

	void SetCameraTransformation(const Lepra::TransformationF& pTransformation);

	void SetLightsEnabled(bool pEnabled);
	void SetMipMappingEnabled(bool pEnabled);
	void SetBilinearFilteringEnabled(bool pEnabled);
	void SetTrilinearFilteringEnabled(bool pEnabled);
	void SetDepthWriteEnabled(bool pEnabled);
	void SetDepthTestEnabled(bool pEnabled);

	void SetAmbientLight(float pRed, float pGreen, float pBlue);
	void AddAmbience(float pRed, float pGreen, float pBlue);

	LightID AddDirectionalLight(LightHint pHint,
				    float pDirX, float pDirY, float pDirZ,
				    float pRed, float pGreen, float pBlue,
				    float pShadowRange);

	LightID AddPointLight(LightHint pHint,
				  float pPosX, float pPosY, float pPosZ,
				  float pRed, float pGreen, float pBlue,
				  float pLightRadius,
				  float pShadowRange);

	LightID AddSpotLight(LightHint pHint,
				 float pPosX, float pPosY, float pPosZ,
				 float pDirX, float pDirY, float pDirZ,
				 float pRed, float pGreen, float pBlue,
				 float pCutoffAngle,
				 float pSpotExponent,
				 float pLightRadius,
				 float pShadowRange);

	void RemoveLight(LightID pLightID);
	void RemoveAllLights();
	//int GetMaxLights();
	inline int GetNumLights();

	void SetLightPosition(LightID pLightID, float pX, float pY, float pZ);
	void SetLightDirection(LightID pLightID, float pX, float pY, float pZ);

	void UpdateGeometry(GeometryID pGeometryID);
	bool ChangeMaterial(GeometryID pGeometryID, MaterialType pMaterialType);

	// Overloaded from MaterialListener.
	bool PreRender(TBC::GeometryBase* pGeometry);
	void PostRender(TBC::GeometryBase* pGeometry);

	virtual void DrawLine(const Lepra::Vector3DF& pPosition, const Lepra::Vector3DF& pVector, const Lepra::Color& pColor);

protected:
	GeometryData* CreateGeometryData();
	TextureData* CreateTextureData(TextureID pTextureID);

	void BindMap(int pMapType, 
		     TextureData* pTextureData,
		     Texture* pTexture);
	void BindCubeMap(TextureData* pTextureData,
			 Texture* pTexture);
	void ReleaseMap(TextureData* pTextureData);
	int ReleaseShadowMap(int pShadowMapID);

	void BindGeometry(TBC::GeometryBase* pGeometry,
			  GeometryID pID,
			  MaterialType pMaterialType);

	bool BindShadowGeometry(UiTbc::ShadowVolume* pShadowGeometry, LightHint pLightHint);
	void ReleaseGeometry(TBC::GeometryBase* pUserGeometry, GeomReleaseOption pOption);

private:

	typedef Lepra::HashTable<Renderer::TextureID, UiTbc::Texture*, std::hash<int> > TextureTable;

	Material* CreateMaterial(Renderer::MaterialType pMaterialType);

	Software3DPainter m3DPainter;
	TextureTable mTextureTable;
};

int SoftwareRenderer::GetNumLights()
{
	return m3DPainter.GetNumLights();
}

} // End namespace.

#endif
