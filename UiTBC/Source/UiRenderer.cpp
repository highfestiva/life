/*
	Class:  Renderer
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#include "pch.h"
#include "../Include/UiRenderer.h"
#include "../../Lepra/Include/HashUtil.h"
#include "../../Lepra/Include/Log.h"
#include "../../Lepra/Include/Math.h"
#include "../../Lepra/Include/Random.h"
#include "../../Lepra/Include/ResourceTracker.h"
#include "../../Tbc/Include/GeometryReference.h"
#include "../Include/UiDynamicRenderer.h"
#include "../Include/UiMaterial.h"



namespace UiTbc
{



Renderer::TextureAssociation::TextureAssociation(int pNumTextures):
	mNumTextures(pNumTextures),
	mTextureID(0),
	mMaps(0)
{
	LEPRA_ACQUIRE_RESOURCE(TextureAssociation);

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

Renderer::TextureAssociation::~TextureAssociation()
{
	delete[] mMaps;
	delete[] mTextureID;
	LEPRA_RELEASE_RESOURCE(TextureAssociation);
};



Renderer::GeometryData::GeometryData():
	mGeometryID(INVALID_GEOMETRY),
	mGeometry(0),
	mMaterialType(Renderer::MAT_SINGLE_COLOR_SOLID),
	mGeometryGroup(0),
	mTA(0),
	mShadow(NO_SHADOWS),
	mLastFrameShadowsUpdated(0)
{
	LEPRA_ACQUIRE_RESOURCE(GeometryData);
	for (int i = 0; i < Renderer::MAX_SHADOW_VOLUMES; i++)
	{
		mShadowVolume[i] = INVALID_GEOMETRY;
		mLightID[i] = INVALID_LIGHT;
	}
}

Renderer::GeometryData::~GeometryData()
{
	LEPRA_RELEASE_RESOURCE(GeometryData);
}

void Renderer::GeometryData::CopyReferenceData(GeometryData*)
{
}



Renderer::Renderer(Canvas* pScreen):
	mCurrentFrame(0),
	mVisibleTriangleCount(0),
	mCulledTriangleCount(0),
	mScreen(pScreen),
	mDX(1),
	mDY(1),
	mFOVAngle(90.0f),
	mNear(0.1f),
	mFar(10000.0f),
	mIsOutlineRenderEnabled(false),
	mIsWireframeEnabled(false),
	mIsPixelShadersEnabled(true),
	mViewport(0, 0, pScreen->GetWidth(), pScreen->GetHeight()),
	mGeometryIDManager(1, 1000000, INVALID_GEOMETRY),
	mTextureIDManager(1, 1000000, INVALID_TEXTURE),
	mLightIDManager(0, MAX_LIGHTS, INVALID_LIGHT),
	mEnvTexture(0),
	mNumSpotLights(0),
	mAmbientRed(0.2f),
	mAmbientGreen(0.2f),
	mAmbientBlue(0.2f),
	mOriginalAmbientRed(0.2f),
	mOriginalAmbientGreen(0.2f),
	mOriginalAmbientBlue(0.2f),
	mMipMapEnabled(false),
	mBilinearEnabled(false),
	mTrilinearEnabled(false),
	mCompressedTexturesEnabled(false),
	mLightsEnabled(false),
	mShadowMode(NO_SHADOWS),
	mShadowHint(Renderer::SH_VOLUMES_ONLY),
	mShadowUpdateIntensity(1.0f),
	mShadowUpdateFrameDelay(100),
	mClippingRect(0, 0, pScreen->GetWidth(), pScreen->GetHeight()),
	mShadowVolumesCreateMax(1),
	mShadowVolumeCreateCount(0),
	mFogNear(0),
	mFogFar(0),
	mFogDensity(1),
	mFogExponent(0)
{
	mCameraActualRotation.RotateAroundOwnX(PIF/2);	// Default for OpenGL.
}

Renderer::~Renderer()
{
}

void Renderer::ClearDebugInfo()
{
	mVisibleTriangleCount = 0;
	mCulledTriangleCount = 0;
}

void Renderer::InitRenderer()
{
	for (int i = 0; i < MAT_COUNT; i++)
	{
		mMaterial[i] = CreateMaterial((MaterialType)i);
	}
}

void Renderer::CloseRenderer()
{
	DynamicRendererMap::iterator x = mDynamicRendererMap.begin();
	for (; x != mDynamicRendererMap.end(); ++x)
	{
		DynamicRenderer* lRenderer = x->second;
		delete lRenderer;
	}
	mDynamicRendererMap.clear();

	ReleaseGeometries();
	ReleaseShadowVolumes();
	ReleaseTextureMaps();
	ReleaseShadowMaps();

	for (int i = 0; i < (int)MAT_COUNT; i++)
	{
		if (mMaterial[i] != 0)
		{
			delete mMaterial[i];
			mMaterial[i] = 0;
		}
	}
}

void Renderer::DeletingGeometry(Tbc::GeometryBase* pGeometry)
{
	GeometryData* lGeometryData = (GeometryData*)pGeometry->GetRendererData();
	if(lGeometryData != 0)
	{
		mLog.Warningf(_T("Geometry with GeometryID %i is deleted before removed from Renderer."), lGeometryData->mGeometryID);
		RemoveGeometry(lGeometryData->mGeometryID);
	}
}

const Canvas* Renderer::GetScreen() const
{
	return mScreen;
}



void Renderer::SetClearColor(const Color& pColor)
{
	mClearColor = pColor;
}



DynamicRenderer* Renderer::GetDynamicRenderer(str pName) const
{
	return HashUtil::FindMapObject(mDynamicRendererMap, pName);
}

void Renderer::AddDynamicRenderer(str pName, DynamicRenderer* pRenderer)
{
	if (!HashUtil::FindMapObject(mDynamicRendererMap, pName))
	{
		mDynamicRendererMap.insert(DynamicRendererMap::value_type(pName, pRenderer));
	}
	else
	{
		delete pRenderer;
	}
}



void Renderer::SetOutlineFillColor(const Color& pColor)
{
	mOutlineFillColor = pColor;
}

void Renderer::EnableOutlineRendering(bool pEnable)
{
	mIsOutlineRenderEnabled = pEnable;
}

bool Renderer::IsOutlineRenderingEnabled() const
{
	return (mIsOutlineRenderEnabled);
}

void Renderer::EnableWireframe(bool pEnable)
{
	mIsWireframeEnabled = pEnable;
}

bool Renderer::IsWireframeEnabled() const
{
	return (mIsWireframeEnabled);
}

void Renderer::EnablePixelShaders(bool pEnable)
{
	mIsPixelShadersEnabled = pEnable;
}

bool Renderer::IsPixelShadersEnabled() const
{
	return (mIsPixelShadersEnabled);
}

void Renderer::SetViewport(const PixelRect& pViewport)
{
	deb_assert(pViewport.mLeft <= pViewport.mRight && 
		pViewport.mTop <= pViewport.mBottom);
	mViewport = pViewport;
	RecalculateFrustumPlanes();

	if (mScreen->GetOutputRotation()%180 != 0)
	{
		PixelRect lRect(pViewport);
		std::swap(lRect.mLeft, lRect.mTop);
		std::swap(lRect.mRight, lRect.mBottom);
		DoSetViewport(lRect);
	}
	else
	{
		DoSetViewport(pViewport);
	}
}

const PixelRect& Renderer::GetViewport() const
{
	return mViewport;
}

void Renderer::SetViewFrustum(float pFOVAngle, float pNear, float pFar)
{
	if (pFOVAngle <= 0 || pFOVAngle >= 180 || pNear >= pFar)
	{
		return;
	}
	mFOVAngle = pFOVAngle;
	mNear = pNear;
	mFar = pFar;
	RecalculateFrustumPlanes();
}

void Renderer::GetViewFrustum(float& pFOVAngle, float& pNear, float& pFar) const
{
	pFOVAngle = mFOVAngle;
	pNear = mNear;
	pFar = mFar;
}

void Renderer::RecalculateFrustumPlanes()
{
	const float lY = Math::Deg2Rad(mFOVAngle*0.5f);
	const float lX = asin(sin(lY) * GetAspectRatio());

	mFrustumPlanes[0].Set(-(float)cos(lX), (float)sin(lX), 0); // Right plane.
	mFrustumPlanes[1].Set( (float)cos(lX), (float)sin(lX), 0); // Left plane.

	mFrustumPlanes[2].Set(0, (float)sin(lY), -(float)cos(lY)); // Bottom plane.
	mFrustumPlanes[3].Set(0, (float)sin(lY),  (float)cos(lY)); // Top plane.
}

void Renderer::SetClippingRect(const PixelRect& pRect)
{
	mClippingRect = pRect;
	if (mScreen->GetOutputRotation()%180 != 0)
	{
		PixelRect lRect(pRect);
		std::swap(lRect.mLeft, lRect.mTop);
		std::swap(lRect.mRight, lRect.mBottom);
		DoSetClippingRect(lRect);
	}
	else
	{
		DoSetClippingRect(pRect);
	}
}

void Renderer::ReduceClippingRect(const PixelRect& pRect)
{
	SetClippingRect(mClippingRect.GetOverlap(pRect));
}

void Renderer::ResetClippingRect()
{
	SetClippingRect(PixelRect(0, 0, mScreen->GetWidth(), mScreen->GetHeight()));
}

const PixelRect& Renderer::GetClippingRect() const
{
	return mClippingRect;
}

void Renderer::SetCameraTransformation(const xform& pTransformation)
{
	mCameraTransformation = pTransformation;
	mCameraActualTransformation = pTransformation;
	mCameraActualTransformation.mOrientation *= mCameraActualRotation;
	mCameraOrientationInverse = mCameraTransformation.mOrientation.GetInverse();
	mCameraActualOrientationInverse = mCameraActualTransformation.mOrientation.GetInverse();
}

const xform& Renderer::GetCameraTransformation()
{
	return mCameraTransformation;
}

const xform& Renderer::GetCameraActualTransformation()
{
	return mCameraActualTransformation;
}

const quat& Renderer::GetCameraOrientationInverse()
{
	return mCameraOrientationInverse;
}

const quat& Renderer::GetCameraActualOrientationInverse()
{
	return mCameraActualOrientationInverse;
}

void Renderer::SetLightsEnabled(bool pEnabled)
{
	mLightsEnabled = pEnabled;
}

bool Renderer::GetLightsEnabled()
{
	return mLightsEnabled;
}

void Renderer::SetMipMappingEnabled(bool pEnabled)
{
	mMipMapEnabled = pEnabled;
}

bool Renderer::GetMipMappingEnabled()
{
	return mMipMapEnabled;
}

void Renderer::SetBilinearFilteringEnabled(bool pEnabled)
{
	mBilinearEnabled = pEnabled;
}

bool Renderer::GetBilinearFilteringEnabled()
{
	return mBilinearEnabled;
}

void Renderer::SetTrilinearFilteringEnabled(bool pEnabled)
{
	mTrilinearEnabled = pEnabled;
}

bool Renderer::GetTrilinearFilteringEnabled()
{
	return mTrilinearEnabled;
}

void Renderer::SetTexturingEnabled(bool pEnabled)
{
	mTexturingEnabled = pEnabled;
}

bool Renderer::GetTexturingEnabled()
{
	return mTexturingEnabled;
}

void Renderer::SetCompressedTexturesEnabled(bool pEnabled)
{
	mCompressedTexturesEnabled = pEnabled;
}

bool Renderer::GetCompressedTexturesEnabled()
{
	return mCompressedTexturesEnabled;
}

void Renderer::SetShadowMode(Shadows pShadowMode, ShadowHint pHint)
{
	mShadowMode = pShadowMode;
	mShadowHint = pHint;
}

Renderer::Shadows Renderer::GetShadowMode()
{
	return 	mShadowMode;
}

void Renderer::SetShadowUpdateIntensity(float pUpdateIntensity)
{
	mShadowUpdateIntensity = pUpdateIntensity;
}

void Renderer::SetShadowUpdateFrameDelay(unsigned pFrameDelay)
{
	mShadowUpdateFrameDelay = pFrameDelay;
}

void Renderer::SetEnableDepthSorting(bool pEnabled)
{
	Material::SetEnableDepthSorting(pEnabled);
}

void Renderer::SetAmbientLight(float pRed, float pGreen, float pBlue)
{
	mOriginalAmbientRed   = mAmbientRed   = pRed;
	mOriginalAmbientGreen = mAmbientGreen = pGreen;
	mOriginalAmbientBlue  = mAmbientBlue  = pBlue;
}

void Renderer::GetAmbientLight(float& pRed, float& pGreen, float& pBlue)
{
	pRed   = mAmbientRed;
	pGreen = mAmbientGreen;
	pBlue  = mAmbientBlue;
}

void Renderer::AddAmbience(float pRed, float pGreen, float pBlue)
{
	mAmbientRed   += pRed;
	mAmbientGreen += pGreen;
	mAmbientBlue  += pBlue;
}

void Renderer::ResetAmbientLight(bool pPropagate)
{
	if (pPropagate)
	{
		SetAmbientLight(mOriginalAmbientRed, mOriginalAmbientGreen, mOriginalAmbientBlue);
	}
	else
	{
		Renderer::SetAmbientLight(mOriginalAmbientRed, mOriginalAmbientGreen, mOriginalAmbientBlue);
	}
}

Renderer::LightID Renderer::AllocLight()
{
	LightID lLightId = (LightID)mLightIDManager.GetFreeId();
	if (lLightId != INVALID_LIGHT &&
		!GetLightData(lLightId))
	{
		mSortedLights[GetLightCount()] = lLightId;	// Put new light at lowest priority.
		mLightDataMap.insert(LightDataMap::value_type(lLightId, new LightData));
	}
	return lLightId;
}

Renderer::LightID Renderer::AddDirectionalLight(LightHint pHint,
		const vec3& pDir,
		const vec3& pColor,
		float pShadowRange)
{
	LightID lLightId = AllocLight();
	if (lLightId == INVALID_LIGHT)
		return INVALID_LIGHT;

	LightData* lData = GetLightData(lLightId);
	deb_assert(lData);
	lData->mType			= LIGHT_DIRECTIONAL;
	lData->mHint			= pHint;
	lData->mPosition		= vec3();
	lData->mDirection		= pDir.GetNormalized();
	lData->mColor[0]		= pColor.x;
	lData->mColor[1]		= pColor.y;
	lData->mColor[2]		= pColor.z;
	lData->mRadius			= 0;
	lData->mShadowRange		= pShadowRange;
	lData->mCutoffAngle		= 89;
	lData->mSpotExponent		= 1;
	lData->mTransformationChanged	= true;
	lData->mEnabled			= true;

	return lLightId;
}

Renderer::LightID Renderer::AddPointLight(LightHint pHint,
		const vec3& pPos,
		const vec3& pColor,
		float pLightRadius,
		float pShadowRange)
{
	LightID lLightId = AllocLight();
	if (lLightId == INVALID_LIGHT)
		return INVALID_LIGHT;

	LightData* lData = GetLightData(lLightId);
	deb_assert(lData);
	lData->mType			= LIGHT_POINT;
	lData->mHint			= pHint;
	lData->mPosition		= pPos;
	lData->mDirection		= vec3();
	lData->mColor[0]		= pColor.x;
	lData->mColor[1]		= pColor.y;
	lData->mColor[2]		= pColor.z;
	lData->mRadius			= pLightRadius;
	lData->mShadowRange		= pShadowRange;
	lData->mCutoffAngle		= 89;
	lData->mSpotExponent		= 1;
	lData->mTransformationChanged	= true;
	lData->mEnabled			= true;

	return lLightId;
}

Renderer::LightID Renderer::AddSpotLight(LightHint pHint,
		const vec3& pPos,
		const vec3& pDir,
		const vec3& pColor,
		float pCutoffAngle,
		float pSpotExponent,
		float pLightRadius,
		float pShadowRange)
{
	LightID lLightId = AllocLight();
	if (lLightId == INVALID_LIGHT)
		return INVALID_LIGHT;

	// Clamp cutoff angle to a value between 0 and 89 degrees.
	// OpenGL accepts 90 degrees, but we'll set max to 89 in order to be able
	// to render shadow maps.
	pCutoffAngle = fabs(pCutoffAngle);
	pCutoffAngle = pCutoffAngle > 89.0f ? 89.0f : pCutoffAngle;

	LightData* lData = GetLightData(lLightId);
	deb_assert(lData);
	lData->mType			= LIGHT_SPOT;
	lData->mHint			= pHint;
	lData->mPosition		= pPos;
	lData->mDirection		= pDir.GetNormalized();
	lData->mColor[0]		= pColor.x;
	lData->mColor[1]		= pColor.y;
	lData->mColor[2]		= pColor.z;
	lData->mRadius			= pLightRadius;
	lData->mShadowRange		= pShadowRange;
	lData->mCutoffAngle		= pCutoffAngle;
	lData->mSpotExponent		= pSpotExponent;
	lData->mTransformationChanged	= true;
	lData->mEnabled		= true;

	const float lEpsilon = 1e-6f;

	// Generate an orientation for the light.
	vec3 lAxisX;
	vec3 lAxisY;
	vec3 lAxisZ;

	// If light direction is pointing up.
	if (lData->mDirection.y >= (1.0f - lEpsilon))
	{
		lAxisX = lData->mDirection / vec3(0, 0, 1);
		lAxisY = lAxisX / lData->mDirection;
		lAxisZ = lData->mDirection;
	}
	else
	{
		lAxisX = lData->mDirection / vec3(0, 1, 0);
		lAxisY = lAxisX / lData->mDirection;
		lAxisZ = lData->mDirection;
	}

	lAxisX.Normalize();
	lAxisY.Normalize();
	lAxisZ.Normalize();

	lData->mOrientation.SetAxisX(lAxisX);
	lData->mOrientation.SetAxisY(lAxisY);
	lData->mOrientation.SetAxisZ(lAxisZ);

	mNumSpotLights++;

	return lLightId;
}

void Renderer::RemoveLight(LightID pLightId)
{
	LightData* lData = GetLightData(pLightId);
	deb_assert(lData);

	if (lData->mType == Renderer::LIGHT_SPOT)
	{
		lData->mShadowMapGeometrySet.RemoveAll();
		if (lData->mShadowMapID != 0)
		{
			lData->mShadowMapID = ReleaseShadowMap(lData->mShadowMapID);
		}
		mNumSpotLights--;
	}

	mLightDataMap.erase(pLightId);
	delete lData;

	mLightIDManager.RecycleId(pLightId);
}

void Renderer::RemoveAllLights()
{
	while (!mLightDataMap.empty())
	{
		RemoveLight(mLightDataMap.begin()->first);
	}
}

int Renderer::GetMaxLights()
{
	return MAX_LIGHTS;
}

int Renderer::GetLightCount()
{
	return mLightDataMap.size();
}

void Renderer::SetShadowMapOptions(LightID pLightId,
				   unsigned char pLog2Res,
				   float pNearPlane,
				   float pFarPlane)
{
	LightData* lData = GetLightData(pLightId);
	deb_assert(lData);

	// Can't change settings when the shadow map has been generated.
	// TODO: Replace '0' with something else...
	if (lData->mShadowMapID == 0)
	{
		lData->mShadowMapRes = (1 << pLog2Res);
		lData->mShadowMapNear = pNearPlane;
		lData->mShadowMapFar  = pFarPlane;
	}
}

void Renderer::SetLightPosition(LightID pLightId, const vec3& pPos)
{
	LightData* lData = GetLightData(pLightId);
	deb_assert(lData);

	if (lData->mType == Renderer::LIGHT_POINT ||
	   lData->mType == Renderer::LIGHT_SPOT)
	{
		if (lData->mPosition != pPos)
		{
			lData->mTransformationChanged = true;
		}
		lData->mPosition = pPos;
	}
}

void Renderer::SetLightDirection(LightID pLightId, const vec3& pDir)
{
	LightData* lData = GetLightData(pLightId);
	deb_assert(lData);

	if (lData->mType == Renderer::LIGHT_DIRECTIONAL ||
	   lData->mType == Renderer::LIGHT_SPOT)
	{
		vec3 lPrevDir(lData->mDirection);
		lData->mDirection = pDir;
		lData->mDirection.Normalize();

		if (lPrevDir != lData->mDirection)
		{
			lData->mTransformationChanged = true;
		}

		if (lData->mType == Renderer::LIGHT_SPOT)
		{
			const float lEpsilon = 1e-6f;

			// Generate an orientation for the light.
			vec3 lAxisX;
			vec3 lAxisY;
			vec3 lAxisZ;

			// If light direction is pointing up.
			if (lData->mDirection.y >= (1.0f - lEpsilon))
			{
				lAxisX = lData->mDirection / vec3(1, 0, 0);
				lAxisY = lData->mDirection / lAxisX;
				lAxisZ = lData->mDirection;
			}
			else
			{
				lAxisX = vec3(0, 1, 0) / lData->mDirection;
				lAxisY = lData->mDirection / lAxisX;
				lAxisZ = lData->mDirection;
			}

			lAxisX.Normalize();
			lAxisY.Normalize();
			lAxisZ.Normalize();

			lData->mOrientation.SetAxisX(lAxisX);
			lData->mOrientation.SetAxisY(lAxisY);
			lData->mOrientation.SetAxisZ(lAxisZ);
		}
	}
}

void Renderer::SetLightColor(LightID pLightId, const vec3& pColor)
{
	LightData* lData = GetLightData(pLightId);
	deb_assert(lData);

	lData->mColor[0] = pColor.x;
	lData->mColor[1] = pColor.y;
	lData->mColor[2] = pColor.z;
}

vec3 Renderer::GetLightPosition(LightID pLightId) const
{
	const LightData* lData = GetLightData(pLightId);
	deb_assert(lData);
	if (lData)
	{
		return lData->mPosition;
	}
	return vec3();
}

vec3 Renderer::GetLightDirection(LightID pLightId) const
{
	const LightData* lData = GetLightData(pLightId);
	deb_assert(lData);
	if (lData)
	{
		return lData->mDirection;
	}
	return vec3();
}

vec3 Renderer::GetLightColor(LightID pLightId) const
{
	const LightData* lData = GetLightData(pLightId);
	deb_assert(lData);
	if (lData)
	{
		return vec3(lData->mColor[0], lData->mColor[1], lData->mColor[2]);
	}
	return vec3();
}

Renderer::LightType Renderer::GetLightType(LightID pLightId)
{
	const LightData* lData = GetLightData(pLightId);
	deb_assert(lData);
	if (lData)
	{
		return lData->mType;
	}
	return LIGHT_INVALID;
}

float Renderer::GetLightCutoffAngle(LightID pLightId)
{
	const LightData* lData = GetLightData(pLightId);
	deb_assert(lData);
	if (lData)
	{
		return lData->mCutoffAngle;
	}
	return 180;
}

float Renderer::GetLightSpotExponent(LightID pLightId)
{
	const LightData* lData = GetLightData(pLightId);
	deb_assert(lData);
	if (lData)
	{
		return lData->mSpotExponent;
	}
	return 180;
}

void Renderer::SortLights(const vec3& pReferencePosition)
{
	smRenderer = this;
	int i = 0;
	LightDataMap::iterator x = mLightDataMap.begin();
	for (; x != mLightDataMap.end(); ++i, ++x)
	{
		mSortedLights[i] = x->first;
	}
	smReferencePosition = pReferencePosition;
	::qsort(mSortedLights, i, sizeof(mSortedLights[0]), LightCompare);
}

int Renderer::LightCompare(const void* pLight1, const void* pLight2)
{
	const LightData* lLight1 = smRenderer->GetLightData(*(LightID*)pLight1);
	const LightData* lLight2 = smRenderer->GetLightData(*(LightID*)pLight2);

	const float lInfluence1 = GetLightInfluence(*lLight1);
	const float lInfluence2 = GetLightInfluence(*lLight2);
	if (lInfluence1 < lInfluence2)
		return +1;	// Sort in descending order of influence.
	else if (lInfluence1 > lInfluence2)
		return -1;	// Sort in descending order of influence.
	return 0;
}

float Renderer::GetLightInfluence(const LightData& pLightData)
{
	float lInfluence = pLightData.mColor[0] * pLightData.mColor[0] +
			    pLightData.mColor[1] * pLightData.mColor[1] +
			    pLightData.mColor[2] * pLightData.mColor[2];
	switch (pLightData.mType)
	{
		case LIGHT_SPOT:
		{
			// It's difficult to predict how much of the target object is within
			// the light cone of a spot light, so make it simple.. If the object
			// is behind the spot light, don't count it.
			if (pLightData.mDirection.Dot(smReferencePosition - pLightData.mPosition) < 0)
			{
				lInfluence = 0;
			}
		} // TRICKY: Fall through.
		case LIGHT_POINT:
		{
			lInfluence /= smReferencePosition.GetDistanceSquared(pLightData.mPosition);
		} // TRICKY: Fall through.
		case LIGHT_DIRECTIONAL:
		{
			lInfluence *= 1 + pLightData.mShadowRange;
			lInfluence *= pLightData.mEnabled? 1 : 0;
		}
		break;
		default: break;
	}
	return (lInfluence);
}

Renderer::LightID Renderer::GetClosestLight(int pIndex)
{
	return mSortedLights[pIndex];
}

Renderer::TextureID Renderer::AddTexture(Texture* pTexture)
{
	int lId = mTextureIDManager.GetFreeId();

	if (lId != INVALID_TEXTURE)
	{
		TextureData* lTexture = CreateTextureData((TextureID)lId);
		mTextureTable.Insert(lId, lTexture);

		lTexture->mIsCubeMap = pTexture->IsCubeMap();

		if (pTexture->IsCubeMap() == false)
		{
			lTexture->mWidth = pTexture->GetColorMap(0)->GetWidth();
			lTexture->mHeight = pTexture->GetColorMap(0)->GetHeight();

			BindMap(Texture::COLOR_MAP, lTexture, pTexture);
			if (pTexture->GetNormalMap(0) != 0)
			{
				BindMap(Texture::NORMAL_MAP, lTexture, pTexture);
			}

			if (pTexture->GetSpecularMap(0) != 0)
			{
				BindMap(Texture::SPECULAR_MAP, lTexture, pTexture);
			}
		}// End if(lTexture->IsCubeMap() == false)
		else
		{
			lTexture->mWidth = pTexture->GetCubeMapPosX(0)->GetWidth();
			lTexture->mHeight = pTexture->GetCubeMapPosX(0)->GetHeight();

			BindCubeMap(lTexture, pTexture);
		}
	}

	return (TextureID)lId;
}

void Renderer::UpdateTexture(TextureID pTextureID, Texture* pTexture)
{
	TextureTable::Iterator x = mTextureTable.Find(pTextureID);

	if (x == mTextureTable.End())
	{
		return;
	}

	TextureData* lTexture = *x;

	// Cube map or not, it has to remain the same type...
	deb_assert(lTexture->mIsCubeMap == pTexture->IsCubeMap());

	if (pTexture->IsCubeMap() == false)
	{
		BindMap(Texture::COLOR_MAP, lTexture, pTexture);

		if (pTexture->GetNormalMap(0) != 0)
		{
			BindMap(Texture::NORMAL_MAP, lTexture, pTexture);
		}

		if (pTexture->GetSpecularMap(0) != 0)
		{
			BindMap(Texture::SPECULAR_MAP, lTexture, pTexture);
		}
	}
	else
	{
		BindCubeMap(lTexture, pTexture);
	}
}

void Renderer::RemoveTexture(TextureID pTextureID)
{
	TextureTable::Iterator x = mTextureTable.Find(pTextureID);

	if (x == mTextureTable.End())
	{
		return;
	}

	TextureData* lTexture = *x;
	mTextureTable.Remove(x);
	ReleaseMap(lTexture);

	if (lTexture == mEnvTexture)
	{
		SetEnvironmentMap(INVALID_TEXTURE);
	}

	delete lTexture;

	mTextureIDManager.RecycleId(pTextureID);
}

void Renderer::SetEnvironmentMap(TextureID pTextureID)
{
	if (pTextureID == INVALID_TEXTURE)
	{
		mEnvTexture = 0;
		return;
	}

	TextureTable::Iterator x = mTextureTable.Find(pTextureID);
	if (x == mTextureTable.End())
	{
		return;
	}

	TextureData* lTexture = *x;
	mEnvTexture = lTexture;
}

bool Renderer::IsEnvMapCubeMap()
{
	if (mEnvTexture != 0)
		return mEnvTexture->mIsCubeMap;
	else
		return false;
}

Renderer::TextureData* Renderer::GetEnvTexture() const
{
	return mEnvTexture;
}

Renderer::GeometryID Renderer::AddGeometry(Tbc::GeometryBase* pGeometry, MaterialType pMaterialType, Shadows pShadows)
{
	if ((int)pMaterialType < 0 || (int)pMaterialType >= Renderer::MAT_COUNT)
	{
		mLog.Errorf(_T("AddGeometry() - Material %i is not a valid material ID!"), (int)pMaterialType);
		return (GeometryID)mGeometryIDManager.GetInvalidId();
	}

	int lID = mGeometryIDManager.GetFreeId();
	if (lID == mGeometryIDManager.GetInvalidId())
		return (GeometryID)lID;

	// Make sure there are vertex normals.
	pGeometry->GenerateVertexNormalData();

	GeometryData* lGeometryData = CreateGeometryData();
	lGeometryData->mGeometryID = (GeometryID)lID;
	lGeometryData->mGeometry = pGeometry;
	lGeometryData->mMaterialType = pMaterialType;
	lGeometryData->mShadow = pShadows;
	pGeometry->SetRendererData(lGeometryData);

	mGeometryTable.Insert(lID, lGeometryData);

	if (pGeometry->IsGeometryReference() == true)
	{
		Tbc::GeometryBase* lParentGeometry = ((Tbc::GeometryReference*)pGeometry)->GetParentGeometry();
		GeometryData* lParentGeometryData = (GeometryData*)lParentGeometry->GetRendererData();
		lGeometryData->CopyReferenceData(lParentGeometryData);
		lParentGeometryData->mReferenceSet.insert((GeometryID)lID);
	}
	else
	{
		BindGeometry(pGeometry, (GeometryID)lID, pMaterialType);
	}

	if (!mMaterial[pMaterialType]->AddGeometry(pGeometry))
	{
		RemoveGeometry((GeometryID)lID);
		lID = mGeometryIDManager.GetInvalidId();
	}

	pGeometry->AddListener(this);

	return (GeometryID)lID;
}

bool Renderer::TryAddGeometryTexture(GeometryID pGeometryId, TextureID pTexture)
{
	bool lOk = false;
	if (pGeometryId == INVALID_GEOMETRY)
	{
		return (false);
	}

	GeometryTable::Iterator lGeomIter;
	lGeomIter = mGeometryTable.Find(pGeometryId);
	deb_assert(lGeomIter != mGeometryTable.End());
	GeometryData* lGeometryData = *lGeomIter;

	/*JMB
	if (lGeometryData->mGeometry->IsGeometryReference() == true)
	{
		mLog.AError("Could not add texture to geometry reference!");
		deb_assert(false);
	}
	else*/
	{
		const int lTextureCount = Texture::NUM_MAPS;
		if (!lGeometryData->mTA)
		{
			lGeometryData->mTA = new Renderer::TextureAssociation(lTextureCount);
		}
		int x;
		lOk = true;
		for (x = 0; lOk && x < lTextureCount; ++x)
		{
			lOk = (lGeometryData->mTA->mTextureID[x] != pTexture);
			if (!lOk)
			{
				//deb_assert(lOk);
				log_atrace("Skipping add of texture to geometry a second time.");
			}
		}
		if (lOk)
		{
			lOk = false;
			for (x = 0; !lOk && x < lTextureCount; ++x)
			{
				if (lGeometryData->mTA->mTextureID[x] == INVALID_TEXTURE)
				{
					lGeometryData->mTA->mTextureID[x] = pTexture;
					TextureData* lTexture = *mTextureTable.Find(pTexture);
					lGeometryData->mTA->mMaps[x].mMapID[Texture::COLOR_MAP]    = lTexture->mTMapID[Texture::COLOR_MAP];
					lGeometryData->mTA->mMaps[x].mMapID[Texture::ALPHA_MAP]    = lTexture->mTMapID[Texture::ALPHA_MAP];
					lGeometryData->mTA->mMaps[x].mMapID[Texture::NORMAL_MAP]   = lTexture->mTMapID[Texture::NORMAL_MAP];
					lGeometryData->mTA->mMaps[x].mMapID[Texture::SPECULAR_MAP] = lTexture->mTMapID[Texture::SPECULAR_MAP];
					lGeometryData->mTA->mMaps[x].mMipMapLevelCount[Texture::COLOR_MAP]    = lTexture->mTMipMapLevelCount[Texture::COLOR_MAP];
					lGeometryData->mTA->mMaps[x].mMipMapLevelCount[Texture::ALPHA_MAP]    = lTexture->mTMipMapLevelCount[Texture::ALPHA_MAP];
					lGeometryData->mTA->mMaps[x].mMipMapLevelCount[Texture::NORMAL_MAP]   = lTexture->mTMipMapLevelCount[Texture::NORMAL_MAP];
					lGeometryData->mTA->mMaps[x].mMipMapLevelCount[Texture::SPECULAR_MAP] = lTexture->mTMipMapLevelCount[Texture::SPECULAR_MAP];
					lOk = true;
				}
			}
			if (!lOk)
			{
				mLog.AError("Could not add texture - no more slots available!");
				deb_assert(false);
			}
		}
		/*JMB
		if (lOk)
		{
			GeometryData::GeometryIDSet::iterator x;
			for (x = lGeometryData->mReferenceSet.begin(); x != lGeometryData->mReferenceSet.end(); ++x)
			{
				// Remove references recursively.
				GeometryTable::Iterator y = mGeometryTable.Find(*x);
				deb_assert(y != mGeometryTable.End());
				if (y != mGeometryTable.End())
				{
					GeometryData* lGeometryClone = *lGeomIter;
					lGeometryClone->CopyReferenceData(lGeometryData);
				}
			}
		}*/
	}
	return (lOk);
}

bool Renderer::DisconnectGeometryTexture(GeometryID pGeometryId, TextureID pTexture)
{
	bool lOk = false;
	if (pGeometryId == INVALID_GEOMETRY)
	{
		deb_assert(false);
		return false;
	}

	GeometryTable::Iterator lGeomIter;
	lGeomIter = mGeometryTable.Find(pGeometryId);
	deb_assert(lGeomIter != mGeometryTable.End());
	GeometryData* lGeometryData = *lGeomIter;

	const int lTextureCount = Texture::NUM_MAPS;
	if (!lGeometryData->mTA)
	{
		deb_assert(false);
		return false;
	}
	for (int x = 0; !lOk && x < lTextureCount; ++x)
	{
		if (lGeometryData->mTA->mTextureID[x] == pTexture)
		{
			lGeometryData->mTA->mTextureID[x] = INVALID_TEXTURE;
			for (int y = 0; y < Texture::NUM_MAPS; ++y)
			{
				lGeometryData->mTA->mMaps[x].mMapID[y] = INVALID_TEXTURE;
				lGeometryData->mTA->mMaps[x].mMipMapLevelCount[y] = 0;
			}
			return true;
		}
	}
	deb_assert(false);
	return false;
}

void Renderer::RemoveGeometry(GeometryID pGeometryID)
{
	if (pGeometryID == INVALID_GEOMETRY)
	{
		return;
	}

	GeometryTable::Iterator lGeomIter;
	lGeomIter = mGeometryTable.Find(pGeometryID);
	if (lGeomIter != mGeometryTable.End())
	{
		GeometryData* lGeometryData = *lGeomIter;
		lGeometryData->mGeometry->RemoveListener(this);

		RemoveShadowVolumes(lGeometryData);

		// Remove the geometry from all spot lights, in case it's added there.
		for (LightDataMap::iterator x = mLightDataMap.begin(); x != mLightDataMap.end(); ++x)
		{
			LightData* lLight = x->second;
			if (lLight->mType == Renderer::LIGHT_SPOT)
			{
				if (lLight->mShadowMapGeometrySet.Remove(lGeometryData) == true)
					lLight->mShadowMapNeedUpdate = true;
			}
		}

		mGeometryTable.Remove(lGeomIter);
		mGeometryIDManager.RecycleId((int)pGeometryID);

		if (lGeometryData->mGeometry->IsGeometryReference())
		{
			Tbc::GeometryBase* lParentGeometry = ((Tbc::GeometryReference*)lGeometryData->mGeometry)->GetParentGeometry();
			GeometryData* lParentGeometryData = (GeometryData*)lParentGeometry->GetRendererData();
			lParentGeometryData->mReferenceSet.erase(lGeometryData->mGeometryID);
		}

		delete (lGeometryData->mTA);
		lGeometryData->mTA = 0;

		ReleaseGeometry(lGeometryData->mGeometry, GRO_REMOVE_FROM_MATERIAL);

		while (!lGeometryData->mReferenceSet.empty())
		{
			// Remove references recursively.
			LEPRA_DEBUG_CODE(const size_t lReferenceCount = lGeometryData->mReferenceSet.size());
			RemoveGeometry(*lGeometryData->mReferenceSet.begin());
			LEPRA_DEBUG_CODE(deb_assert(lReferenceCount == lGeometryData->mReferenceSet.size()+1));
		}

		lGeometryData->mGeometry->SetRendererData(0);
		delete lGeometryData;
	}
}

void Renderer::RemoveAllGeometry()
{
	GeometryTable::Iterator x;
	int i;
	for (i = 0, x = mGeometryTable.First(); x != mGeometryTable.End(); ++i)
	{
		GeometryTable::Iterator y = x;
		++x;
		RemoveGeometry((GeometryID)y.GetKey());
	}
}

Renderer::MaterialType Renderer::GetMaterialType(GeometryID pGeometryID)
{
	deb_assert(pGeometryID != INVALID_GEOMETRY);
	MaterialType lMaterial = MAT_NULL;
	GeometryTable::Iterator x = mGeometryTable.Find(pGeometryID);
	deb_assert(x != mGeometryTable.End());
	if (x != mGeometryTable.End())
	{
		GeometryData* lGeometryData = *x;
		lMaterial = lGeometryData->mMaterialType;
	}
	return (lMaterial);
}

void Renderer::SetShadows(GeometryID pGeometryID, Renderer::Shadows pShadowMode)
{
	GeometryTable::Iterator x = mGeometryTable.Find(pGeometryID);
	deb_assert(x != mGeometryTable.End());
	if (x != mGeometryTable.End())
	{
		GeometryData* lGeometryData = *x;
		lGeometryData->mShadow = pShadowMode;
	}
}

Renderer::Shadows Renderer::GetShadows(GeometryID pGeometryID)
{
	deb_assert(pGeometryID != INVALID_GEOMETRY);
	Shadows lShadows = NO_SHADOWS;
	GeometryTable::Iterator x = mGeometryTable.Find(pGeometryID);
	deb_assert(x != mGeometryTable.End());
	if (x != mGeometryTable.End())
	{
		GeometryData* lGeometryData = *x;
		lShadows = lGeometryData->mShadow;
	}
	return (lShadows);
}

bool Renderer::PreRenderMaterial(MaterialType pMaterialType) 
{
	GetMaterial(pMaterialType)->PreRender();
	return true;
}

void Renderer::PostRenderMaterial(MaterialType pMaterialType)
{
	GetMaterial(pMaterialType)->PostRender();
}

void Renderer::UpdateShadowMaps()
{
	mShadowVolumeCreateCount = 0;	// Reset every frame.

	LightData* lLightData = GetLightData(GetClosestLight(0));
	if (!lLightData)
	{
		return;
	}
	unsigned lTrianglesCalculatedFor = 0;
	bool lDidStatic = false;
	for (int i = 0; i <= (int)MAT_LAST_SOLID; i++)
	{
		Tbc::GeometryBase* lGeometry = mMaterial[i]->GetFirstGeometry();
		while (lGeometry)
		{
			if (lGeometry->GetAlwaysVisible() || lGeometry->GetLastFrameVisible() == mCurrentFrame)
			{
				const Tbc::GeometryBase::GeometryVolatility lVolatility = lGeometry->GetGeometryVolatility();
				bool lUpdate = (lTrianglesCalculatedFor < 1000 || lVolatility >= Tbc::GeometryBase::GEOM_SEMI_STATIC || !lDidStatic);
				if (lUpdate)
				{
					lTrianglesCalculatedFor += UpdateShadowMaps(lGeometry, lLightData);
					if (lVolatility == Tbc::GeometryBase::GEOM_STATIC)
					{
						lDidStatic = true;
					}
				}
			}
			lGeometry = mMaterial[i]->GetNextGeometry();
		}
	}
}

unsigned Renderer::UpdateShadowMaps(Tbc::GeometryBase* pGeometry, LightData* pClosestLightData)
{
	GeometryData* lGeometry = (GeometryData*)pGeometry->GetRendererData();
	const float lLightShadowRange = pClosestLightData->mShadowRange * 4;
	const bool lDenyShadows =
		(lGeometry->mShadow == FORCE_NO_SHADOWS || mShadowMode <= NO_SHADOWS || 
		lGeometry->mMaterialType == MAT_NULL ||
		pGeometry->GetTransformation().GetPosition().GetDistanceSquared(mCameraTransformation.GetPosition()) >= lLightShadowRange*lLightShadowRange);
	const bool lForceShadows = (mShadowMode == FORCE_CAST_SHADOWS);
	const bool lEscapeShadows = (lGeometry->mShadow == NO_SHADOWS);
	if (lDenyShadows || (!lForceShadows && lEscapeShadows))
	{
		if (lGeometry->mShadowVolume[0])
		{
			RemoveShadowVolumes(lGeometry);
		}
		return 0;
	}

	if (lGeometry->mShadowVolume[0] == 0)
	{
		if (++mShadowVolumeCreateCount > mShadowVolumesCreateMax)
		{
			return 0;	// Better luck next frame.
		}
	}

	SortLights(pGeometry->GetTransformation().GetPosition());

	const int lLightCount = GetLightCount();
	if (mShadowHint == Renderer::SH_VOLUMES_AND_MAPS)
	{
		for (int i = 0; i < lLightCount; i++)
		{
			LightData* lLightData = (i==0)? pClosestLightData : GetLightData(GetClosestLight(i));
			if (lLightData->mShadowRange <= 0)
			{
				continue;
			}

			if (lLightData->mType == Renderer::LIGHT_SPOT)
			{
				if (pGeometry->GetTransformationChanged() == true)
					lLightData->mShadowMapNeedUpdate = true;

				float lDist = lLightData->mPosition.GetDistanceSquared(pGeometry->GetTransformation().GetPosition());
				float lMinDist = lLightData->mRadius + pGeometry->GetBoundingRadius();
				lMinDist *= lMinDist;

				if (lDist < lMinDist)
				{
					if (lLightData->mShadowMapGeometrySet.Insert(lGeometry) == true)
						lLightData->mShadowMapNeedUpdate = true;
				}
				else
				{
					if (lLightData->mShadowMapGeometrySet.Remove(lGeometry) == true)
						lLightData->mShadowMapNeedUpdate = true;
				}
			}
		}
	}

	bool lShadowsUpdated = false;

	// Update shadow volumes.

	// Iterate over all the closest light sources and update shadow volumes.
	unsigned lActiveLightCount = 0;
	const int lLoopMax = MAX_SHADOW_VOLUMES < GetLightCount() ? MAX_SHADOW_VOLUMES : lLightCount;
	int i;
	for (i = 0; i < lLoopMax; i++)
	{
		bool lProcessLight = false;
		LightData* lLightData = (i==0)? pClosestLightData : GetLightData(GetClosestLight(i));
		if (lLightData->mShadowRange <= 0)
		{
			continue;
		}

		if (lLightData->mEnabled == true)
		{
			if (lLightData->mType == Renderer::LIGHT_POINT ||
			   (lLightData->mType == Renderer::LIGHT_SPOT && mShadowHint == Renderer::SH_VOLUMES_ONLY))
			{
				float lDist = lLightData->mPosition.GetDistanceSquared(pGeometry->GetTransformation().GetPosition());
				float lMinDist = lLightData->mRadius + pGeometry->GetBoundingRadius();

				if (lDist < lMinDist*lMinDist)
				{
					lProcessLight = true;
				}
			}
			else if(lLightData->mType == Renderer::LIGHT_DIRECTIONAL)
			{
				lProcessLight = true;
			}
		}

		if (lProcessLight == true)
		{
			++lActiveLightCount;
			if (lGeometry->mShadowVolume[i] == 0)
			{
				// Light has been enabled. Create shadow volume.
				int lId = mGeometryIDManager.GetFreeId();

				if (lId != mGeometryIDManager.GetInvalidId())
				{
					ShadowVolume* lShadowVolume = new ShadowVolume(pGeometry);
					GeometryData* lShadowGeom = CreateGeometryData();
					lShadowGeom->mGeometry = lShadowVolume;
					lShadowVolume->SetRendererData(lShadowGeom);

					if (lLightData->mType == Renderer::LIGHT_DIRECTIONAL)
					{
						lShadowVolume->UpdateShadowVolume(lLightData->mDirection,
										    lLightData->mShadowRange, 
										    true);
					}
					else
					{
						lShadowVolume->UpdateShadowVolume(lLightData->mPosition,
										    lLightData->mShadowRange, 
										    false);
					}

					mShadowVolumeTable.Insert(lId, lShadowGeom);
					lGeometry->mShadowVolume[i] = (GeometryID)lId;

					lShadowsUpdated = BindShadowGeometry(lShadowVolume, lLightData->mHint);
				}

				lGeometry->mLightID[i] = GetClosestLight(i);
			}
			else
			{
				if ((pGeometry->GetTransformationChanged() || pGeometry->GetVertexDataChanged() ||
					lGeometry->mLightID[i] != GetClosestLight(i)) ||
					(lLightData->mTransformationChanged &&
					mCurrentFrame - lGeometry->mLastFrameShadowsUpdated >= mShadowUpdateFrameDelay))
				{
					ShadowVolumeTable::Iterator x = mShadowVolumeTable.Find(lGeometry->mShadowVolume[i]);
					if (x != mShadowVolumeTable.End())
					{
						GeometryData* lShadowGeom = *x;
						ShadowVolume* lShadowVolume = (ShadowVolume*)lShadowGeom->mGeometry;
						if (!pGeometry->GetBigOrientationChanged())
						{
							// Only update translation if orientation didn't change much.
							xform lTransform(pGeometry->GetLastBigOrientation(), pGeometry->GetTransformation().GetPosition());
							lShadowVolume->SetTransformation(lTransform);
						}
						else if (lLightData->mType == Renderer::LIGHT_DIRECTIONAL)
						{
							lShadowVolume->UpdateShadowVolume(lLightData->mDirection,
											    lLightData->mShadowRange, 
											    true);
						}
						else
						{
							lShadowVolume->UpdateShadowVolume(lLightData->mPosition,
											    lLightData->mShadowRange, 
											    false);
						}

						lShadowsUpdated = true;
						lGeometry->mLightID[i] = GetClosestLight(i);
					}
					else
					{
						deb_assert(false);
						lGeometry->mShadowVolume[i] = (GeometryID)0;
						lGeometry->mLightID[i] = INVALID_LIGHT;
					}
				}
			}
		}
		else if(lGeometry->mShadowVolume[i] != 0)
		{
			// Light is too far away or has been disabled. Remove shadow volume.
			RemoveShadowVolume(lGeometry->mShadowVolume[i]);
			lGeometry->mLightID[i] = INVALID_LIGHT;
		}
	}// End for(i < Renderer::MAX_SHADOW_VOLUMES)

	pGeometry->SetTransformationChanged(false);
	pGeometry->SetBigOrientationChanged(false);
	pGeometry->SetVertexDataChanged(false);

	if (lShadowsUpdated == true)
	{
		const int q = (mShadowUpdateFrameDelay > 3)? mShadowUpdateFrameDelay / 3 : 1;
		lGeometry->mLastFrameShadowsUpdated = mCurrentFrame + Random::GetRandomNumber() % q;
		return pGeometry->GetTriangleCount() * lActiveLightCount;
	}
	return 0;
}

void Renderer::Tick(float pTime)
{
	DynamicRendererMap::iterator x = mDynamicRendererMap.begin();
	for (; x != mDynamicRendererMap.end(); ++x)
	{
		DynamicRenderer* lRenderer = x->second;
		lRenderer->Tick(pTime);
	}
}

unsigned Renderer::GetCurrentFrame() const
{
	return mCurrentFrame;
}

bool Renderer::CheckFlag(unsigned pFlags, unsigned pFlag)
{
	return (pFlags & pFlag) != 0;
}

Material* Renderer::GetMaterial(MaterialType pMaterialType) const
{
	deb_assert(pMaterialType >= MAT_NULL && pMaterialType <= MAT_COUNT);
	return mMaterial[pMaterialType];
}

Renderer::GeometryTable& Renderer::GetGeometryTable()
{
	return mGeometryTable;
}

Renderer::ShadowVolumeTable& Renderer::GetShadowVolumeTable()
{
	return mShadowVolumeTable;
}

Renderer::LightData* Renderer::GetLightData(LightID pLightId) const
{
	return HashUtil::FindMapObject(mLightDataMap, pLightId);
}

int Renderer::GetNumSpotLights() const
{
	return mNumSpotLights;
}

void Renderer::StepCurrentFrame()
{
	++mCurrentFrame;
}

Renderer::ShadowHint Renderer::GetShadowHint() const
{
	return mShadowHint;
}

void Renderer::PrepareProjectionData()
{
	float32 lFOVAngle;
	float32 lNear;
	float32 lFar;
	GetViewFrustum(lFOVAngle, lNear, lFar);

	mDY = 1.0 / tan(Math::Deg2Rad(lFOVAngle) / 2.0);
	mDX = mDY * GetAspectRatio();
}

PixelRect Renderer::GetBoundingRect(const vec3* pVertex, int pNumVertices) const
{
	const Canvas* lScreen = GetScreen();

	if (pNumVertices <= 2)
	{
		mLog.AError("GetBoundingRect() - NumVertices < 3!");
		return PixelRect(0,0,0,0);
	}

	PixelRect lRect(0, 0, 0, 0);

	int lPrevIndex = pNumVertices - 1;
	vec3 lPrev(mCamTransform.InverseTransform(vec3(pVertex[lPrevIndex].x, pVertex[lPrevIndex].y, pVertex[lPrevIndex].z)));

	bool lLeftOK   = false;
	bool lRightOK  = false;
	bool lTopOK    = false;
	bool lBottomOK = false;
	double lRectLeft   = 0;
	double lRectRight  = 0;
	double lRectTop    = 0;
	double lRectBottom = 0;

	const double lScreenWidth  = (double)lScreen->GetWidth();
	const double lScreenHeight = (double)lScreen->GetHeight();

	for (int i = 0; i < pNumVertices; i++)
	{
		vec3 lCurrent(mCamTransform.InverseTransform(vec3(pVertex[i].x, pVertex[i].y, pVertex[i].z)));

		if ((lPrev.y <= 0 && lCurrent.y > 0) ||
		    (lPrev.y >  0 && lCurrent.y <= 0))
		{
			// Clip at z = 0.
			vec3 lDiff = lCurrent - lPrev;
			vec3 lClipPos = lPrev + lDiff * (-lPrev.y / lDiff.y);

			// Determine wether the clipped position is to the left or to the right.
			if (lClipPos.x <= 0)
			{
				// Bounding rect reaches the left edge of the screen.
				if (lLeftOK == false || lRectLeft > -0.5)
					lRectLeft = -0.5;
				lLeftOK = true;
			}
			else
			{
				if (lRightOK == false || lRectRight < lScreenWidth + 0.5)
					lRectRight = lScreenWidth + 0.5;
				lRightOK = true;
			}

			if (lClipPos.z >= 0)
			{
				// Bounding rect reaches the top edge of the screen.
				if (lTopOK == false || lRectTop > -0.5)
					lRectTop = -0.5;
				lTopOK = true;
			}
			else
			{
				if (lBottomOK == false || lRectBottom < lScreenHeight + 0.5)
					lRectBottom = lScreenHeight + 0.5;
				lBottomOK = true;
			}
		}

		if (lPrev.y > 0)
		{
			// Projection of previous point..
			double lRecipDepth = 1.0 / lPrev.y;
			double lPX = lPrev.x * mDX * lRecipDepth;
			double lPY = lPrev.z * mDY * lRecipDepth;
			lPX = ( lPX + 1.0) * 0.5 * (lScreenWidth  + 1.0) - 0.5;
			lPY = (-lPY + 1.0) * 0.5 * (lScreenHeight + 1.0) - 0.5;

			// Update bounding rect.
			if (lLeftOK == false || lPX < lRectLeft)
			{
				lRectLeft = lPX;
				lLeftOK = true;
			}

			if (lRightOK == false || lPX >= lRectRight)
			{
				lRectRight = lPX;
				lRightOK = true;
			}

			if (lTopOK == false || lPY < lRectTop)
			{
				lRectTop = lPY;
				lTopOK = true;
			}

			if (lBottomOK == false || lPY >= lRectBottom)
			{
				lRectBottom = lPY;
				lBottomOK = true;
			}
		}
		   
		lPrevIndex = i;
		lPrev = lCurrent;
	}

	// Finally, we need to clamp the bounding rect to the coordinates of the screen.
	lRect.mLeft   = (int)floor(Math::Clamp(lRectLeft,   -0.5, lScreenWidth  + 0.5) + 0.5);
	lRect.mRight  = (int)floor(Math::Clamp(lRectRight,  -0.5, lScreenWidth  + 0.5) + 0.5);
	lRect.mTop    = (int)floor(Math::Clamp(lRectTop,    -0.5, lScreenHeight + 0.5) + 0.5);
	lRect.mBottom = (int)floor(Math::Clamp(lRectBottom, -0.5, lScreenHeight + 0.5) + 0.5);

	return lRect;
}

bool Renderer::IsFacingFront(const vec3* pVertex, int pNumVertices)
{
	if (pNumVertices < 3)
		return false;

	// Use the cross product constructor to create the surface normal.
	vec3 lNormal(pVertex[1] - pVertex[0], pVertex[2] - pVertex[0]);
	vec3 lCamVector(pVertex[0] - mCamTransform.GetPosition());

	return lNormal.Dot(lCamVector) > 0;
}

vec3 Renderer::ScreenCoordToVector(const PixelCoord& pCoord) const
{
	const float w2 = mClippingRect.GetWidth() * 0.5f;
	const float h2 = mClippingRect.GetHeight() * 0.5f;
	float lFOV, lNear, lFar;
	GetViewFrustum(lFOV, lNear, lFar);
	lFOV = Math::Deg2Rad(lFOV);
	const float lAspect = w2/h2;
	const float tana = tan(lFOV*0.5f);
	float dx = tana * (pCoord.x/w2-1.0f) * lAspect;
	float dy = tana * (1.0f-pCoord.y/h2);
	vec3 lDirection(dx, 1, dy);
	lDirection.Normalize();
	lDirection = mCameraTransformation.GetOrientation() * lDirection;
	return lDirection;
}

vec2 Renderer::PositionToScreenCoord(const vec3& pPosition, float pAspectRatio) const
{
	vec3 lCamDirection(pPosition - mCameraTransformation.mPosition);
	vec3 lDirection;
	mCameraTransformation.mOrientation.FastInverseRotatedVector(mCameraOrientationInverse, lDirection, lCamDirection);

	// Normalize so Y-distance from camera is 1.
	const float eps = 1e-5f;	// Something small.
	if (std::abs(lDirection.y) < eps)
	{
		lDirection.y = eps;
	}
	lDirection.x /= lDirection.y;
	lDirection.z /= lDirection.y;

	const float w2 = mClippingRect.GetWidth() * 0.5f;
	const float h2 = mClippingRect.GetHeight() * 0.5f;
	float lFOV, lNear, lFar;
	GetViewFrustum(lFOV, lNear, lFar);
	lFOV = Math::Deg2Rad(lFOV);
	const float lInverseAspect = pAspectRatio? 1/pAspectRatio : h2/w2;
	const float lInverseTanA = 1/tan(lFOV*0.5f);

	vec2 lCoord;
	lCoord.x = ( lDirection.x*lInverseTanA*lInverseAspect+1.0f) * w2;
	lCoord.y = (-lDirection.z*lInverseTanA+1.0f) * h2;
	return lCoord;
}

float Renderer::GetAspectRatio() const
{
	return (float)mViewport.GetWidth() / (float)mViewport.GetHeight();
}

bool Renderer::CheckCulling(const xform& pTransform, double pBoundingRadius)
{
	const vec3& lPos = pTransform.GetPosition();

	bool lVisible = true;
	if (lVisible)
	{
		// Check if object is between near and far planes.
		lVisible = (lPos.y + pBoundingRadius) > mNear && (lPos.y - pBoundingRadius) < mFar;
	}

	// Loop over the remaining frustum planes.
	for (int i = 0; lVisible && i < 4; i++)
	{
		lVisible = (mFrustumPlanes[i].Dot(lPos) > -pBoundingRadius);
	}

	return lVisible;
}



void Renderer::ReleaseShadowVolumes()
{
	ShadowVolumeTable::Iterator lShadowIter;
	for (lShadowIter = mShadowVolumeTable.First(); lShadowIter != mShadowVolumeTable.End(); ++lShadowIter)
	{
		GeometryData* lShadowGeom = *lShadowIter;

		delete lShadowGeom->mGeometry;
		delete lShadowGeom;
	}
}

void Renderer::ReleaseGeometries()
{
	GeometryTable::Iterator lGeoIter;
	
	for (lGeoIter = mGeometryTable.First(); 
		lGeoIter != mGeometryTable.End(); 
		++lGeoIter)
	{
		GeometryData* lGeometry = *lGeoIter;
		delete lGeometry->mTA;
		lGeometry->mTA = 0;
		delete lGeometry;
	}
}

void Renderer::ReleaseTextureMaps()
{
	TextureTable::Iterator x = mTextureTable.First();
	while (x != mTextureTable.End())
	{
		TextureData* lTexture = *x;
		ReleaseMap(lTexture);
		TextureTable::Iterator y = x;
		++x;
		mTextureTable.Remove(y);
		delete lTexture;
	}
}

void Renderer::ReleaseShadowMaps()
{
	// Release shadow maps.
	LightDataMap::iterator x = mLightDataMap.begin();
	for (; x != mLightDataMap.end(); ++x)
	{
		LightData* lData = x->second;
		if (lData->mType == Renderer::LIGHT_SPOT)
		{
			lData->mShadowMapID = ReleaseShadowMap(lData->mShadowMapID);
		}
	}
}

void Renderer::RemoveShadowVolumes(GeometryData* pOwnerGeometry)
{
	for (int i = 0; i < MAX_SHADOW_VOLUMES; ++i)
	{
		if (pOwnerGeometry->mShadowVolume[i] != 0)
		{
			RemoveShadowVolume(pOwnerGeometry->mShadowVolume[i]);
		}
	}
}

void Renderer::RemoveShadowVolume(GeometryID& pShadowVolume)
{
	if (pShadowVolume != 0)
	{
		ShadowVolumeTable::Iterator x;
		x = mShadowVolumeTable.Find(pShadowVolume);
		
		if (x != mShadowVolumeTable.End())
		{
			GeometryData* lShadowGeom = *x;

			mShadowVolumeTable.Remove(x);

			ReleaseGeometry(lShadowGeom->mGeometry, GRO_IGNORE_MATERIAL);
			delete lShadowGeom->mGeometry;
			delete lShadowGeom;
		}

		mGeometryIDManager.RecycleId(pShadowVolume);
		pShadowVolume = (GeometryID)0;
	}
}

int Renderer::GetTriangleCount(bool pVisible)
{
	return pVisible? mVisibleTriangleCount : mCulledTriangleCount;
}

float Renderer::GetFogNear() const
{
	return mFogNear;
}

float Renderer::GetFogFar() const
{
	return mFogFar;
}

float Renderer::GetFogDensity() const
{
	return mFogDensity;
}

float Renderer::GetFogExponent() const
{
	return mFogExponent;
}

void Renderer::SetFog(float pNear, float pFar, float pDensity, float pExponent)
{
	mFogNear = pNear;
	mFogFar = pFar;
	mFogDensity = pDensity;
	mFogExponent = pExponent;
}

void Renderer::CalcCamCulling()
{
	for (int x = 0; x < 4; ++x)
	{
		mCamFrustumPlanes[x] = mCameraTransformation.GetOrientation() * mFrustumPlanes[x];
	}
}

bool Renderer::CheckCamCulling(const vec3& pPosition, float pBoundingRadius) const
{
	// We only check the frustum planes on the side, and totally ignore the near and far (they currently don't add much).
	vec3 lCamRelativePosition(pPosition);
	lCamRelativePosition.Sub(mCameraTransformation.mPosition);
	bool lVisible = true;
	for (int x = 0; lVisible && x < 4; ++x)
	{
		lVisible = (mCamFrustumPlanes[x].Dot(lCamRelativePosition) > -pBoundingRadius);
	}
	return lVisible;
}



Renderer* Renderer::smRenderer = 0;
vec3 Renderer::smReferencePosition;
loginstance(UI_GFX_3D, Renderer);



}
