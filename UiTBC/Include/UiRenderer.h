
// Author: Jonas Bystr�m
// Copyright (c) 2002-2013, Pixel Doctrine

// This class is a high level interface to the sublevel rendering API (which can be OpenGL, Direct3D or some other API). 
// The renderer uses a right handed coordinate system:
//    X is right.
//    Y is forward.
//    Z is up.

#pragma once

#include "../../TBC/Include/GeometryBase.h"
#include <hash_set>
#include "../../Lepra/Include/Canvas.h"
#include "../../Lepra/Include/Graphics2D.h"
#include "../../Lepra/Include/IdManager.h"
#include "../../Lepra/Include/HashSet.h"
#include "../../Lepra/Include/HashTable.h"
#include "../../Lepra/Include/RotationMatrix.h"
#include "../../Lepra/Include/Transformation.h"
#include "../../Lepra/Include/Vector2D.h"
#include "UiTBC.h"
#include "UiBillboardRenderInfo.h"
#include "UiTexture.h"
#include "UiShadowVolume.h"
#include "UiPortalManager.h"



namespace TBC
{
class GeometryReference;
}



namespace UiTbc
{



class DynamicRenderer;
class GeometryGroup;
class Material;



class Renderer : public TBC::GeometryBase::Listener
{
public:

	// All supported materials. TODO: Add more.
	// The enumeration order is very important, since
	// the materials are rendered in this order.
	enum MaterialType
	{
		MAT_NULL = 0,				// The NULL material doesn't render anything.
		MAT_SINGLE_COLOR_SOLID,
		MAT_SINGLE_COLOR_SOLID_PXS,		// Single color using pixel shaders.
		MAT_VERTEX_COLOR_SOLID,
		MAT_SINGLE_TEXTURE_SOLID,
		MAT_SINGLE_TEXTURE_HIGHLIGHT,		// Single texture without lighting/specular.
		MAT_SINGLE_TEXTURE_SOLID_PXS,		// Single texture using pixel shaders.
		MAT_SINGLE_COLOR_ENVMAP_SOLID,
		MAT_SINGLE_TEXTURE_ENVMAP_SOLID,	// Environtment map + single texture.

		MAT_TEXTURE_AND_LIGHTMAP,		// Requires two rendering passes and a multitexture unit.
		MAT_TEXTURE_AND_LIGHTMAP_PXS,		// Requires pixel shaders.

		MAT_TEXTURE_AND_DIFFUSE_BUMPMAP_PXS,	// Texture + normal map. Faster.
		MAT_TEXTURE_SBMAP_PXS,			// Texture + specular map + normal map.

		MAT_LAST_SOLID = MAT_TEXTURE_SBMAP_PXS,

		MAT_SINGLE_TEXTURE_ALPHATESTED,

		MAT_SINGLE_COLOR_BLENDED,
		MAT_SINGLE_COLOR_OUTLINE_BLENDED,
		MAT_VERTEX_COLOR_BLENDED,
		MAT_SINGLE_TEXTURE_BLENDED,
		MAT_SINGLE_COLOR_ENVMAP_BLENDED,
		MAT_SINGLE_TEXTURE_ENVMAP_BLENDED,

		MAT_COUNT,
	};

	typedef std::hash_map<str, DynamicRenderer*> DynamicRendererMap;

	enum LightType
	{
		LIGHT_INVALID = -1,
		LIGHT_DIRECTIONAL = 0,
		LIGHT_POINT,
		LIGHT_SPOT,
	};

	enum LightHint
	{
		LIGHT_STATIC = 0,
		LIGHT_MOVABLE,
	};

	enum Shadows
	{
		FORCE_NO_SHADOWS = 0,
		NO_SHADOWS,
		CAST_SHADOWS,
		FORCE_CAST_SHADOWS,
	};

	enum ShadowHint
	{
		SH_VOLUMES_ONLY = 0,	// Use shadow volumes for all light sources.
		SH_VOLUMES_AND_MAPS,	// Use shadow volumes for directional and point lights,
					// shadow maps for spot lights.
	};

	enum
	{
		MAX_SHADOW_VOLUMES = 4,	// Max shadow volumes per geometry instance.
		MAX_LIGHTS = 8,
	};

	enum // Clear flags.
	{
		CLEAR_COLORBUFFER        = (1 << 0),
		CLEAR_DEPTHBUFFER        = (1 << 1),
		CLEAR_STENCILBUFFER      = (1 << 2),
		CLEAR_ACCUMULATIONBUFFER = (1 << 3),
	};

	typedef int TextureID;
	typedef int GeometryID;
	typedef int LightID;
	static const TextureID INVALID_TEXTURE = 0;
	static const GeometryID INVALID_GEOMETRY = 0;
	static const LightID INVALID_LIGHT = -1;

	class TextureAssociation
	{
	public:
		struct Maps
		{
			int mMapID[Texture::NUM_MAPS];
			int mMipMapLevelCount[Texture::NUM_MAPS];
		};

		TextureAssociation(int pNumTextures) :
			mNumTextures(pNumTextures),
			mTextureID(0),
			mMaps(0)
		{
			mTextureID = new TextureID[mNumTextures];
			mMaps = new Maps[mNumTextures];
			for (int i = 0; i < mNumTextures; i++)
			{
				mTextureID[i] = INVALID_TEXTURE;
				mMaps[i].mMapID[Texture::COLOR_MAP]    = 0;
				mMaps[i].mMapID[Texture::ALPHA_MAP]    = 0;
				mMaps[i].mMapID[Texture::NORMAL_MAP]   = 0;
				mMaps[i].mMapID[Texture::SPECULAR_MAP] = 0;
				mMaps[i].mMapID[Texture::CUBE_MAP] = 0;
				mMaps[i].mMipMapLevelCount[Texture::COLOR_MAP]    = 0;
				mMaps[i].mMipMapLevelCount[Texture::ALPHA_MAP]    = 0;
				mMaps[i].mMipMapLevelCount[Texture::NORMAL_MAP]   = 0;
				mMaps[i].mMipMapLevelCount[Texture::SPECULAR_MAP] = 0;
				mMaps[i].mMipMapLevelCount[Texture::CUBE_MAP] = 0;
			}
		}

		~TextureAssociation()
		{
			delete[] mMaps;
			delete[] mTextureID;
		};

		int mNumTextures;
		TextureID* mTextureID; // One ID per texture.
		Maps* mMaps; // A bunch of different maps (buffers) per texture.
	};

	class GeometryData
	{
	public:
		GeometryData() :
			mGeometryID(INVALID_GEOMETRY),
			mGeometry(0),
			mMaterialType(Renderer::MAT_SINGLE_COLOR_SOLID),
			mGeometryGroup(0),
			mTA(0),
			mShadow(NO_SHADOWS),
			mLastFrameShadowsUpdated(0)
		{
			for (int i = 0; i < Renderer::MAX_SHADOW_VOLUMES; i++)
			{
				mShadowVolume[i] = INVALID_GEOMETRY;
				mLightID[i] = INVALID_LIGHT;
			}
		}

		virtual void CopyReferenceData(GeometryData*)
		{
			// JMB: mTA = pGeom->mTA;
		}

		typedef std::hash_set<GeometryID, LEPRA_ENUM_HASHER> GeometryIDSet;

		// List of geometries that refer to this geometry.
		GeometryIDSet mReferenceSet;

		// The ID of this geometry.
		GeometryID mGeometryID;

		// The actual geometry.
		TBC::GeometryBase* mGeometry;

		// The associated material.
		MaterialType mMaterialType;
		GeometryGroup* mGeometryGroup;

		// The associated texture.
		TextureAssociation* mTA;

		// Shadow volumes and associated light sources.
		GeometryID mShadowVolume[MAX_SHADOW_VOLUMES];
		int mLightID[MAX_SHADOW_VOLUMES];

		// Tells wether the geometry is subject to shadow casting.
		Shadows mShadow;

		unsigned mLastFrameShadowsUpdated;
	};

	class TextureData
	{
	public:
		TextureData(TextureID pTextureID, int pInvalidID) :
			mWidth(0),
			mHeight(0),
			mIsCubeMap(0),
			mTextureID(pTextureID)
		{
			for (int i = 0; i < Texture::NUM_MAPS; i++)
			{
				mTMapID[i] = pInvalidID;
				mTMipMapLevelCount[i] = 0;
			}
		}

		// Width and height of texture (highest resolution).
		int mWidth;
		int mHeight;

		// Set to true if this texture is actually six textures
		// that defines a cube map.
		bool mIsCubeMap;

		// IDs to identify the actual texture. For OpenGL this is
		// simply the associated "texture names". For Direct3D this
		// is the index into an array of texture handles.
		int mTMapID[Texture::NUM_MAPS];

		int mTMipMapLevelCount[Texture::NUM_MAPS];

		TextureID mTextureID;
	};

	class LightData
	{
	public:

		LightData() :
			mType(Renderer::LIGHT_DIRECTIONAL),
			mEnabled(false),
			mTransformationChanged(false),
			mRadius(0.0f),
			mShadowRange(0.0f),
			mCutoffAngle(180.0f),
			mSpotExponent(1),
			mShadowMapID(0),
			mShadowMapRes(0),
			mShadowMapNear(0),
			mShadowMapFar(0),
			mShadowMapNeedUpdate(true)
		{
			mColor[0] = 1.0f;
			mColor[1] = 1.0f;
			mColor[2] = 1.0f;
			mColor[3] = 1.0f;
		}

		typedef HashSet<GeometryData*, LEPRA_VOIDP_HASHER> GeometrySet;

		Vector3DF mPosition;
		Vector3DF mDirection;
		LightType mType;
		LightHint mHint;
		bool mEnabled;
		bool mTransformationChanged;

		float mColor[4];
		float mRadius;
		float mShadowRange;

		// Spot light & shadow map stuff.
		float mCutoffAngle;
		float mSpotExponent;
		int mShadowMapID;
		int mShadowMapRes;
		float mShadowMapNear;
		float mShadowMapFar;
		bool mShadowMapNeedUpdate;
		GeometrySet mShadowMapGeometrySet;

		// Used to be GLfloat in OpenGLRenderer...
		float mLightProjectionMatrix[16];

		RotationMatrixF mOrientation;
	};

	Renderer(Canvas* pScreen);
	virtual ~Renderer();

	void ClearDebugInfo();

	// Implementing the GeometryBase::Listener interface.
	void DeletingGeometry(TBC::GeometryBase* pGeometry);

	const Canvas* GetScreen() const;

	virtual void Clear(unsigned pClearFlags = CLEAR_COLORBUFFER | CLEAR_DEPTHBUFFER) = 0;
	virtual void SetClearColor(const Color& pColor) = 0;

	DynamicRenderer* GetDynamicRenderer(str pName) const;
	void AddDynamicRenderer(str pName, DynamicRenderer* pRenderer);

	virtual void SetOutlineFillColor(const Color& pColor);
	void EnableOutlineRendering(bool pEnable);
	bool IsOutlineRenderingEnabled() const;
	void EnableWireframe(bool pEnable);
	bool IsWireframeEnabled() const;
	void EnablePixelShaders(bool pEnable);
	virtual bool IsPixelShadersEnabled() const;

	// A viewport is a portion of the screen where the graphics will be rendered.
	// The center of the viewport is also the center of perspective. The viewport may
	// be partially outside the actual screen. Note that pBottom > pTop.
	virtual void SetViewport(const PixelRect& pViewport);
	const PixelRect& GetViewport() const;

	// A view frustum defines the Field Of View (FOV). The pFOVAngle is given 
	// in degrees and defines the angle between the left and the right edge of the 
	// viewport.
	virtual void SetViewFrustum(float pFOVAngle, float pNear, float pFar);
	void GetViewFrustum(float& pFOVAngle, float& pNear, float& pFar);

	// Given the FOV angle you want on a 4:3 monitor (the reference angle),
	// and the actual aspect ratio of your screen (which may be 16:9 or 
	// something else), this function returns the FOV angle you should use
	// on your display.
	float CalcFOVAngle(float pReferenceAngle, float pAspectRatio);

	// The clipping rectangle defines a smaller portion of the viewport where graphics 
	// should be rendered. A clipping rect doesn't affect the perspective center. 
	// Everything outside the clipping rect will (obviously) be clipped. Note that 
	// pBottom > pTop.
	// 
	// When REDUCING the clipping rect, the new rectangle will be restricted to the area
	// within the one previously set. This means that setting the clipping rect one time 
	// after another will reduce the size of the clipping area each time. Call 
	// ResetClippingRect() to set the clipping rect back to cover the entire screen.
	void SetClippingRect(const PixelRect& pRect);
	void ReduceClippingRect(const PixelRect& pRect);
	void ResetClippingRect();
	const PixelRect& GetClippingRect() const;

	virtual void SetCameraTransformation(const TransformationF& pTransformation);
	const TransformationF& GetCameraTransformation();
	const QuaternionF& GetCameraOrientationInverse();

	virtual void SetLightsEnabled(bool pEnabled);
	bool GetLightsEnabled();

	virtual void SetMipMappingEnabled(bool pEnabled);
	bool GetMipMappingEnabled();

	virtual void SetBilinearFilteringEnabled(bool pEnabled);
	bool GetBilinearFilteringEnabled();

	// Will also activate mip mapping and bilinear.
	virtual void SetTrilinearFilteringEnabled(bool pEnabled);
	bool GetTrilinearFilteringEnabled();

	// Will only affect textures added AFTER calling one of these.
	virtual void SetCompressedTexturesEnabled(bool pEnabled);
	bool GetCompressedTexturesEnabled();

	virtual void SetShadowMode(Shadows pMode, ShadowHint pHint);
	Shadows GetShadowMode();
	void SetShadowUpdateFrameDelay(unsigned pFrameDelay);

	virtual void SetDepthWriteEnabled(bool pEnabled) = 0;
	virtual void SetDepthTestEnabled(bool pEnabled) = 0;

	// These simply forwards the call to Material::SetDepthSortEnabled().
	void SetEnableDepthSorting(bool pEnabled);

	virtual void SetAmbientLight(float pRed, float pGreen, float pBlue);
	void GetAmbientLight(float& pRed, float& pGreen, float& pBlue);
	// Adds ambient light. Used to implement self illumination in standard materials.
	virtual void AddAmbience(float pRed, float pGreen, float pBlue);
	// Sets the ambient light back to the value set in the last call to SetAmbientLight().
	void ResetAmbientLight(bool pPropagate);

	// Adds a light to the scene. 
	// pLightRadius tells how close an object must be before it starts casting any
	// shadows. pShadowRange tells how far to extrude the shadow volumes relative
	// to the light source.
	virtual LightID AddDirectionalLight(LightHint pHint, // Helps the renderer to optimize shadow rendering.
		const Vector3DF& pDir,
		const Vector3DF& pColor,
		float pShadowRange);

	virtual LightID AddPointLight(LightHint pHint, // Helps the renderer to optimize shadow rendering.
		const Vector3DF& pPos,
		const Vector3DF& pColor,
		float pLightRadius,
		float pShadowRange);

	virtual LightID AddSpotLight(LightHint pHint, // Helps the renderer to optimize shadow rendering.
		const Vector3DF& pPos,
		const Vector3DF& pDir,
		const Vector3DF& pColor,
		float pCutoffAngle,
		float pSpotExponent,
		float pLightRadius,
		float pShadowRange);

	virtual void RemoveLight(LightID pLightID);
	virtual void EnableAllLights(bool pEnable) = 0;
	void RemoveAllLights();
	int GetMaxLights();
	int GetLightCount();

	// Set the shadow map options for one specific spot light.
	virtual void SetShadowMapOptions(LightID pLightID,
					 unsigned char pLog2Res, 
					 float pNearPlane,
					 float pFarPlane);

	virtual void SetLightPosition(LightID pLightID, const Vector3DF& pPos);
	virtual void SetLightDirection(LightID pLightID, const Vector3DF& pDir);
	virtual void SetLightColor(LightID pLightID, const Vector3DF& pColor);
	Vector3DF GetLightPosition(LightID pLightID) const;
	Vector3DF GetLightDirection(LightID pLightID) const;
	Vector3DF GetLightColor(LightID pLightID) const;

	LightType GetLightType(LightID pLightID);
	float GetLightCutoffAngle(LightID pLightID);
	float GetLightSpotExponent(LightID pLightID);

	// First sort the lights by "influence", then retrieve them one by one
	// by calling GetClosestLight().
	void SortLights(const Vector3DF& pReferencePosition);
	LightID GetClosestLight(int pIndex);

	virtual TextureID AddTexture(Texture* pTexture);
	virtual void UpdateTexture(TextureID pTextureID, Texture* pTexture);
	virtual void RemoveTexture(TextureID pTextureID);

	virtual void SetEnvironmentMap(TextureID pTextureID);

	TextureData* GetEnvTexture() const;
	bool IsEnvMapCubeMap(); // Returns true if environment map is a cube map.

	// Adds/updates/removes geometry to/in/from the renderer.
	virtual GeometryID AddGeometry(TBC::GeometryBase* pGeometry, MaterialType pMaterialType, Shadows pShadows);
	virtual bool TryAddGeometryTexture(GeometryID pGeometryId, TextureID pTexture);	// Only adds a texture ID if it hasn't been added already.
	virtual void UpdateGeometry(GeometryID pGeometryID) = 0;
	virtual void RemoveGeometry(GeometryID pGeometryID);
	virtual void RemoveAllGeometry();
	MaterialType GetMaterialType(GeometryID pGeometryID);
	void SetShadows(GeometryID pGeometryID, Renderer::Shadows pShadowMode);
	Shadows GetShadows(GeometryID pGeometryID);
	virtual bool ChangeMaterial(GeometryID pGeometryID, MaterialType pMaterialType) = 0;

	// These are used by the material classes in the context specific renderer.
	// Don't use them elsewhere.
	virtual bool PreRender(TBC::GeometryBase* pGeometry) = 0;
	virtual void PostRender(TBC::GeometryBase* pGeometry) = 0;

	void UpdateShadowMaps();
	unsigned UpdateShadowMaps(TBC::GeometryBase* pGeometry);	// Returns the number of triangles calculated for.

	virtual void Tick(float pTime);
	virtual unsigned RenderScene() = 0;
	virtual void RenderBillboards(TBC::GeometryBase* pGeometry, bool pRenderTexture, bool pAddativeBlending, const BillboardRenderInfoArray& pBillboards) = 0;

	// Used for rendering stuff that are NOT in the world, such as
	// 3D-objects in the GUI. The position of the geometry is considered
	// relative to the camera.
	virtual void RenderRelative(TBC::GeometryBase* pGeometry, const QuaternionF* pLightOrientation) = 0;

	unsigned GetCurrentFrame() const;

	//
	// Special functions implemented to support portal rendering.
	//

	// Prepares projection calculations.
	void PrepareProjectionData();

	// Given a polygon (pVertex) in world space, this function returns the
	// projected bounding rectangle in screen coordinates.
	PixelRect GetBoundingRect(const Vector3DF* pVertex, int pNumVertices) const;

	// Returns true if the front face is turned towards the camera.
	bool IsFacingFront(const Vector3DF* pVertex, int pNumVertices);

	// Returns the triangle count in the scene. 
	// Parameter controls counting visible OR culled triangles.
	int GetTriangleCount(bool pVisible);

	virtual void DrawLine(const Vector3DF& pPosition, const Vector3DF& pVector, const Color& pColor) = 0;

	void CalcCamCulling();
	bool CheckCamCulling(const Vector3DF& pPosition, float pBoundingRadius) const;

protected:
	enum GeomReleaseOption
	{
		GRO_IGNORE_MATERIAL = 0,
		GRO_REMOVE_FROM_MATERIAL
	};

	typedef HashTable<int, GeometryData*> GeometryTable;
	typedef HashTable<int, GeometryData*> ShadowVolumeTable;
	typedef HashTable<int, TextureData*> TextureTable;

	// Call these from the child class' constructor and destructor, respectively.
	void InitRenderer();
	void CloseRenderer();

	virtual void DoSetClippingRect(const PixelRect& pRect) = 0;
	virtual void DoSetViewport(const PixelRect& pViewport) = 0;

	// Some functions commonly used by the child classes.
	Material* GetMaterial(MaterialType pMaterialType) const;
	GeometryTable& GetGeometryTable();
	ShadowVolumeTable& GetShadowVolumeTable();
	LightData* GetLightData(LightID pLightId) const;
	int GetNumSpotLights() const;
	void StepCurrentFrame();
	ShadowHint GetShadowHint() const;

	virtual Material* CreateMaterial(Renderer::MaterialType pMaterialType) = 0;
	virtual GeometryData* CreateGeometryData() = 0;
	virtual TextureData* CreateTextureData(TextureID pTextureID) = 0;

	static bool CheckFlag(unsigned pFlags, unsigned pFlag);
	float GetAspectRatio() const;

	bool CheckCulling(const TransformationF& pTransform, double pBoundingRadius);

	virtual void BindMap(int pMapType, 
			     TextureData* pTextureData,
			     Texture* pTexture) = 0;
	virtual void BindCubeMap(TextureData* pTextureData,
				 Texture* pTexture) = 0;
	virtual void ReleaseMap(TextureData* pTextureData) = 0;
	virtual int ReleaseShadowMap(int pShadowMapID) = 0;

	virtual void BindGeometry(TBC::GeometryBase* pGeometry,
				  GeometryID pID,
				  MaterialType pMaterialType) = 0;

	virtual bool BindShadowGeometry(UiTbc::ShadowVolume* pShadowGeometry, LightHint pLightHint) = 0;
	virtual void ReleaseGeometry(TBC::GeometryBase* pUserGeometry, GeomReleaseOption pOption) = 0;

	LightID AllocLight();

	void ReleaseShadowVolumes();
	void ReleaseGeometries();
	void ReleaseTextureMaps();
	void ReleaseShadowMaps();
	void RemoveShadowVolumes(GeometryData* pOwnerGeometry);
	void RemoveShadowVolume(GeometryID& pShadowVolume);

	void RecalculateFrustumPlanes();

	static int LightCompare(const void* pLight1, const void* pLight2);
	static float GetLightInfluence(const LightData& pLightData);

	Material* mMaterial[MAT_COUNT];
	DynamicRendererMap mDynamicRendererMap;
	unsigned mCurrentFrame;

	unsigned mVisibleTriangleCount;
	unsigned mCulledTriangleCount;

	Canvas* mScreen;

	Color mOutlineFillColor;

	// Used for portal rendering.
	double mDX;
	double mDY;
	TransformationF mCamTransform;

	// View frustum.
	float mFOVAngle;
	float mNear;
	float mFar;
	Vector3DF mFrustumPlanes[4];
	Vector3DF mCamFrustumPlanes[4];

	bool mIsOutlineRenderEnabled;
	bool mIsWireframeEnabled;
	bool mIsPixelShadersEnabled;

	// Viewport.
	PixelRect mViewport;

	IdManager<int> mGeometryIDManager;
	IdManager<int> mTextureIDManager;
	IdManager<int> mLightIDManager;
	GeometryTable mGeometryTable;
	TextureTable mTextureTable;
	ShadowVolumeTable mShadowVolumeTable;

	TextureData* mEnvTexture;

	// Lights.
	typedef std::hash_map<LightID, LightData*> LightDataMap;
	LightDataMap mLightDataMap;
	LightID mSortedLights[MAX_LIGHTS];
	int mNumSpotLights;

	float mAmbientRed;
	float mAmbientGreen;
	float mAmbientBlue;
	float mOriginalAmbientRed;
	float mOriginalAmbientGreen;
	float mOriginalAmbientBlue;

	bool mMipMapEnabled;
	bool mBilinearEnabled;
	bool mTrilinearEnabled;
	bool mCompressedTexturesEnabled;
	bool mLightsEnabled;

	Shadows mShadowMode;
	ShadowHint mShadowHint;
	unsigned mShadowUpdateFrameDelay;

	PixelRect mClippingRect;
	TransformationF mCameraTransformation;
	QuaternionF mCameraOrientationInverse;

	// Since distance sorting lights is implemented using ::qsort(),
	// we need to store the following variables "globally".
	static Renderer* smRenderer;
	static Vector3DF smReferencePosition;

	LOG_CLASS_DECLARE();
};



}
