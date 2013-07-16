
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

#pragma once

#include "../../Lepra/Include/Canvas.h"
#include "../../Lepra/Include/Graphics2D.h"
#include "../../Lepra/Include/HashSet.h"
#include "../../Lepra/Include/HashTable.h"
#include "../../Lepra/Include/IdManager.h"
#include "../../Lepra/Include/Lepra.h"
#include "../../Lepra/Include/Transformation.h"
#include "../../UiLepra/Include/UiOpenGLExtensions.h"
#include "UiRenderer.h"
#include "UiShadowVolume.h"
#include "UiTBC.h"
#include <list>



namespace UiTbc
{



class OpenGLRenderer: public Renderer
{
	typedef Renderer Parent;
public:

	class OGLGeometryData: public GeometryData
	{
	public:
		OGLGeometryData() :
			mVertexBufferID(0),
			mIndexBufferID(0),
			mNormalOffset(0),
			mColorOffset(0),
			mUVOffset(0),
			mTangentOffset(0),
			mBitangentOffset(0)
		{
		}

		virtual void CopyReferenceData(GeometryData* pGeom)
		{
			GeometryData::CopyReferenceData(pGeom);
			OGLGeometryData* lGeom = (OGLGeometryData*)pGeom;
			mVertexBufferID = lGeom->mVertexBufferID;
			mIndexBufferID  = lGeom->mIndexBufferID;
			mNormalOffset    = lGeom->mNormalOffset;
			mColorOffset     = lGeom->mColorOffset;
			mUVOffset        = lGeom->mUVOffset;
			mTangentOffset   = lGeom->mTangentOffset;
			mBitangentOffset = lGeom->mBitangentOffset;
		}

		// Access to the OpenGL buffer object data.
		int mVertexBufferID;
		int mIndexBufferID;
		size_t mNormalOffset;
		size_t mColorOffset;
		size_t mUVOffset;
		size_t mTangentOffset;   // Used with bump/normal mapping.
		size_t mBitangentOffset; // Used with bump/normal mapping.
	};

	OpenGLRenderer(Canvas* pScreen);
	virtual ~OpenGLRenderer();
	
	// Overloads from Renderer.
	void Clear(unsigned pClearFlags = CLEAR_COLORBUFFER | CLEAR_DEPTHBUFFER);
	void SetClearColor(const Color& pColor);

	virtual bool IsPixelShadersEnabled() const;

	void SetViewFrustum(float pFOVAngle, float pNear, float pFar);

	void SetShadowMode(Shadows pShadowMode, ShadowHint pHint);
	void SetDepthWriteEnabled(bool pEnabled);
	void SetDepthTestEnabled(bool pEnabled);

	void SetAmbientLight(float pRed, float pGreen, float pBlue);
	void AddAmbience(float pRed, float pGreen, float pBlue);

	virtual LightID AddDirectionalLight(LightHint pHint,
		const Vector3DF& pDir,
		const Vector3DF& pColor,
		float pShadowRange);

	virtual LightID AddPointLight(LightHint pHint,
		const Vector3DF& pPos,
		const Vector3DF& pColor,
		float pLightRadius,
		float pShadowRange);

	virtual LightID AddSpotLight(LightHint pHint,
		const Vector3DF& pPos,
		const Vector3DF& pDir,
		const Vector3DF& pColor,
		float pCutoffAngle,
		float pSpotExponent,
		float pLightRadius,
		float pShadowRange);

	virtual void RemoveLight(LightID pLightID);
	virtual void EnableAllLights(bool pEnable);

	void SetLightPosition(LightID pLightID, const Vector3DF& pPos);
	void SetLightDirection(LightID pLightID, const Vector3DF& pDir);

	virtual TextureID AddTexture(Texture* pTexture);

	void UpdateGeometry(GeometryID pGeometryID);
	bool ChangeMaterial(GeometryID pGeometryID, MaterialType pMaterialType);

	virtual unsigned RenderScene();
	virtual void RenderBillboards(TBC::GeometryBase* pGeometry, bool pRenderTexture, bool pAddativeBlending, const BillboardRenderInfoArray& pBillboards);
	virtual void RenderRelative(TBC::GeometryBase* pGeometry, const QuaternionF* pLightOrientation);

	// Only used by the OpenGL material classes.
	int GetEnvMapID();
	int GetNumTextureUnits() const;

	void SetGlobalMaterialReflectance(float pRed, float pGreen, float pBlue, float pSpecularity);

	bool PreRender(TBC::GeometryBase* pGeometry);
	void PostRender(TBC::GeometryBase* pGeometry);

	virtual void DrawLine(const Vector3DF& pPosition, const Vector3DF& pVector, const Color& pColor);

protected:
	virtual void DoSetClippingRect(const PixelRect& pRect);
	virtual void DoSetViewport(const PixelRect& pViewport);

	// Overloads from HardwareRenderer
	int ReleaseShadowMap(int pShadowMapID);

	TextureData* CreateTextureData(TextureID pTextureID);
	GeometryData* CreateGeometryData();

	void BindMap(int pMapType, TextureData* pTextureData, Texture* pTexture);
	void BindCubeMap(TextureData* pTextureData, Texture* pTexture);
	void ReleaseMap(TextureData* pTextureData);

	const Canvas* GetMap(int pMapType, int pMipMapLevel, Texture* pUserTexture);

	void BindGeometry(TBC::GeometryBase* pUserGeometry, GeometryID pID, MaterialType pMaterialType);
	bool BindShadowGeometry(UiTbc::ShadowVolume* pShadowGeometry, LightHint pLightHint);
	void ReleaseGeometry(TBC::GeometryBase* pUserGeometry, GeomReleaseOption pOption);

private:

	Material* CreateMaterial(MaterialType pMaterialType);

	void ProcessLights();
	void SetupGLLight(int pLightIndex, const LightData* pLight);

	// Shadow volumes are used with directional lights and point lights.
	void RenderShadowVolumes();

	// Shadow maps are used with spot lights. Requires OpenGL 1.4.
	// RenderShadowMaps() returns the number of spotlights currently in the
	// scene.
	int RenderShadowMaps();
	void RegenerateShadowMap(LightData* pLight);

	// Doing about the same thing as gluPerspective()...
	// gluPerspective() want's the vertical FOV-angle - between the top and
	// the bottom frustum planes (top and bottom screen edges), since a wide-
	// screen monitor should result in a wider view (thus, the height fov
	// remains constant).
	// Here's a function that does it the other way around.
	// pFOVAngle is the angle in degrees between the left and the right
	// frustum planes (left and right screen edges).
	void Perspective(float pFOVAngle, float pAspectRatio, float pNear, float pFar);

	void SetPixelFormat(int& pSize, GLenum& pPixelFormat, bool pCompress, const str& pErrorMessage);

	IdManager<int> mBufferIDManager;
	IdManager<int> mTMapIDManager;

	GLbitfield mGLClearMask;

	int mNumTextureUnits;	// Number of OpenGL texture units available.

	TransformationF mCamSpaceTransformation;	// Optimization to not have to run constructor.

	LOG_CLASS_DECLARE();
};



}
