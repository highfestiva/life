/*
	Class:  Renderer
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games

	NOTES:

	This class is a high level interface to the sublevel rendering API 
	(which can be OpenGL, Direct3D or some other API). 
	
	Renderer implements all	common functionality to avoid code duplication.
	This makes it a little bit harder to read and follow, but it makes
	it so much easier to implement new APIs. 

	The renderer is using a right handed coordinate system where...
	X is right.
	Y is forward.
	Z is up.
*/

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
#include "UiTBC.h"
#include "UiTexture.h"
#include "UiShadowVolume.h"
#include "UiPortalManager.h"



namespace TBC
{
class GeometryReference;
}



namespace UiTbc
{



class Material;
class GeometryGroup;



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
		MAT_SINGLE_TEXTURE_SOLID_PXS,		// Single texture using pixel shaders.
		MAT_SINGLE_COLOR_ENVMAP_SOLID,
		MAT_SINGLE_TEXTURE_ENVMAP_SOLID,	// Environtment map + single texture.

		MAT_TEXTURE_AND_LIGHTMAP,		// Requires two rendering passes and a multitexture unit.
		MAT_TEXTURE_AND_LIGHTMAP_PXS,		// Requires pixel shaders.

		MAT_TEXTURE_AND_DIFFUSE_BUMPMAP_PXS,	// Texture + normal map. Faster.
		MAT_TEXTURE_SBMAP_PXS,			// Texture + specular map + normal map.

		MAT_SINGLE_TEXTURE_ALPHATESTED,

		MAT_SINGLE_COLOR_BLENDED,
		MAT_VERTEX_COLOR_BLENDED,
		MAT_SINGLE_TEXTURE_BLENDED,
		MAT_SINGLE_COLOR_ENVMAP_BLENDED,
		MAT_SINGLE_TEXTURE_ENVMAP_BLENDED,

		NUM_MATERIALTYPES,
	};

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
		NO_SHADOWS = 0,
		CAST_SHADOWS,
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

	enum TextureID
	{
		INVALID_TEXTURE = 0,
	};
	enum GeometryID
	{
		INVALID_GEOMETRY = 0,
	};
	enum LightID
	{
		INVALID_LIGHT = -1,
	};

	class TextureAssociation
	{
	public:
		struct Maps
		{
			int mMapID[Texture::NUM_MAPS];
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

		TextureID mTextureID;
	};

	class LightData
	{
	public:

		LightData() :
			mType(Renderer::LIGHT_DIRECTIONAL),
			mEnabled(false),
			mTransformationChanged(false),
			mIndex(0),
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

		int mIndex;

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

	void EnableOutlineRendering(bool pEnable);
	bool IsOutlineRenderingEnabled() const;
	void EnableWireframe(bool pEnable);
	bool IsWireframeEnabled() const;

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
	virtual void SetClippingRect(const PixelRect& pRect);
	void ReduceClippingRect(const PixelRect& pRect);
	virtual void ResetClippingRect();
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

	virtual void SetShadowsEnabled(bool pEnabled, ShadowHint pHint);
	bool GetShadowsEnabled();
	void SetShadowUpdateFrameDelay(unsigned pFrameDelay);

	virtual void SetFallbackMaterialEnabled(bool pEnabled);
	bool GetFallbackMaterialEnabled();

	virtual void SetDepthWriteEnabled(bool pEnabled) = 0;
	virtual void SetDepthTestEnabled(bool pEnabled) = 0;

	// These simply forwards the call to Material::SetDepthSortEnabled().
	void SetEnableDepthSorting(bool pEnabled);

	virtual void SetAmbientLight(float pRed, float pGreen, float pBlue);
	void GetAmbientLight(float& pRed, float& pGreen, float& pBlue);
	// Adds ambient light. Used to implement self illumination in standard materials.
	virtual void AddAmbience(float pRed, float pGreen, float pBlue);
	// Sets the ambient light back to the value set in the last call to SetAmbientLight().
	void ResetAmbientLight();

	// Adds a light to the scene. 
	// pLightRadius tells how close an object must be before it starts casting any
	// shadows. pShadowRange tells how far to extrude the shadow volumes relative
	// to the light source.
	virtual LightID AddDirectionalLight(LightHint pHint, // Helps the renderer to optimize shadow rendering.
				    float pDirX, float pDirY, float pDirZ,
				    float pRed, float pGreen, float pBlue,
				    float pShadowRange);

	virtual LightID AddPointLight(LightHint pHint, // Helps the renderer to optimize shadow rendering.
			      float pPosX, float pPosY, float pPosZ,
			      float pRed, float pGreen, float pBlue,
			      float pLightRadius,
			      float pShadowRange);

	virtual LightID AddSpotLight(LightHint pHint, // Helps the renderer to optimize shadow rendering.
			     float pPosX, float pPosY, float pPosZ,
			     float pDirX, float pDirY, float pDirZ,
			     float pRed, float pGreen, float pBlue,
			     float pCutoffAngle,
			     float pSpotExponent,
			     float pLightRadius,
			     float pShadowRange);

	LightID AddDirectionalLight(LightHint pHint, const Vector3DF& pDir, const Color& pColor, float pLightIntensity, float pShadowRange);
	LightID AddPointLight(LightHint pHint, const Vector3DF& pPos, const Color& pColor, float pLightIntensity, float pLightRadius, float pShadowRange);
	LightID AddSpotLight(LightHint pHint, const Vector3DF& pPos, const Vector3DF& pDir, const Color& pColor, float pLightIntensity, float pCutoffAngle, float pSpotExponent, float pLightRadius, float pShadowRange);

	virtual void RemoveLight(LightID pLightID);
	void RemoveAllLights();
	int GetMaxLights();
	int GetLightCount();

	// Set the shadow map options for one specific spot light.
	virtual void SetShadowMapOptions(LightID pLightID,
					 unsigned char pLog2Res, 
					 float pNearPlane,
					 float pFarPlane);

	virtual void SetLightPosition(LightID pLightID, float pX, float pY, float pZ);
	virtual void SetLightDirection(LightID pLightID, float pX, float pY, float pZ);
	void GetLightPosition(LightID pLightID, float& pX, float& pY, float& pZ);
	void GetLightDirection(LightID pLightID, float& pX, float& pY, float& pZ);
	void GetLightColor(LightID pLightID, float& pR, float& pG, float& pB);

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

	bool IsEnvMapCubeMap(); // Returns true if environment map is a cube map.



	// Adds/updates/removes geometry to/in/from the renderer.
	virtual GeometryID AddGeometry(TBC::GeometryBase* pGeometry, MaterialType pMaterialType, Shadows pShadows);
	virtual bool TryAddGeometryTexture(GeometryID pGeometryId, TextureID pTexture);	// Only adds a texture ID if it hasn't been added already.
	virtual void UpdateGeometry(GeometryID pGeometryID) = 0;
	virtual void RemoveGeometry(GeometryID pGeometryID);
	virtual void RemoveAllGeometry();
	MaterialType GetMaterialType(GeometryID pGeometryID);
	Shadows GetShadows(GeometryID pGeometryID);
	virtual bool ChangeMaterial(GeometryID pGeometryID, MaterialType pMaterialType) = 0;

	// These are used by the material classes in the context specific renderer.
	// Don't use them elsewhere.
	virtual bool PreRender(TBC::GeometryBase* pGeometry) = 0;
	virtual void PostRender(TBC::GeometryBase* pGeometry) = 0;

	void UpdateShadowMaps();
	void UpdateShadowMaps(TBC::GeometryBase* pGeometry);

	virtual unsigned RenderScene() = 0;

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
	bool CheckCamCulling(const Vector3DF& pPosition, float pBoundingRadius);

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

	// Some functions commonly used by the child classes.
	Material* GetMaterial(MaterialType pMaterialType) const;
	GeometryTable& GetGeometryTable();
	ShadowVolumeTable& GetShadowVolumeTable();
	TextureData* GetEnvTexture() const;
	LightData& GetLightData(int pLightIndex);
	int GetNumSpotLights() const;
	void StepCurrentFrame();
	ShadowHint GetShadowHint() const;

	// 0 <= pIndex < GetLightCount().
	// Returns the actual index of light number "pIndex".
	int GetLightIndex(int pIndex) const;


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

protected:
	int AllocLight();

	void ReleaseShadowVolumes();
	void ReleaseGeometries();
	void ReleaseTextureMaps();
	void ReleaseShadowMaps();
	void RemoveShadowVolume(GeometryID& pShadowVolume);

	void RecalculateFrustumPlanes();

	static int LightCompare(const void* pLight1, const void* pLight2);
	static float GetLightInfluence(const LightData& pLightData);

	Material* mMaterial[NUM_MATERIALTYPES];
	unsigned mCurrentFrame;

	unsigned mVisibleTriangleCount;
	unsigned mCulledTriangleCount;

	Canvas* mScreen;

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
	LightData mLightData[MAX_LIGHTS];
	int mLightIndex[MAX_LIGHTS];
	int mLightCount;
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
	bool mShadowsEnabled;
	bool mLightsEnabled;
	bool mFallbackMaterialEnabled;

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
