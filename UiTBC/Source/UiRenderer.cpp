/*
	Class:  Renderer
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games
*/

#include "../Include/UiRenderer.h"
#include "../Include/UiMaterial.h"
#include "../../TBC/Include/GeometryReference.h"
#include "../../Lepra/Include/Log.h"
#include "../../Lepra/Include/Math.h"

namespace UiTbc
{

Renderer::Renderer(Canvas* pScreen) :
	mTriangleCount(0),
	mScreen(pScreen),
	mDX(1),
	mDY(1),
	mFOVAngle(90.0f),
	mNear(0.1f),
	mFar(10000.0f),
	mViewport(0, 0, pScreen->GetWidth(), pScreen->GetHeight()),
	mGeometryIDManager(1, 1000000, INVALID_GEOMETRY),
	mTextureIDManager(1, 1000000, INVALID_TEXTURE),
	mLightIDManager(0, MAX_LIGHTS, INVALID_LIGHT),
	mEnvTexture(0),
	mLightCount(0),
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
	mShadowsEnabled(false),
	mLightsEnabled(false),
	mFallbackMaterialEnabled(false),
	mShadowHint(Renderer::SH_VOLUMES_ONLY),
	mShadowUpdateFrameDelay(0),
	mClippingRect(0, 0, pScreen->GetWidth(), pScreen->GetHeight())
{
	mCurrentFrame = 0;
}

Renderer::~Renderer()
{
}

void Renderer::InitRenderer()
{
	for (int i = 0; i < NUM_MATERIALTYPES; i++)
	{
		mMaterial[i] = CreateMaterial((MaterialType)i);
	}
}

void Renderer::CloseRenderer()
{
	ReleaseGeometries();
	ReleaseShadowVolumes();
	ReleaseTextureMaps();
	ReleaseShadowMaps();

	for (int i = 0; i < (int)NUM_MATERIALTYPES; i++)
	{
		if (mMaterial[i] != 0)
		{
			delete mMaterial[i];
		}
	}
}

void Renderer::DeletingGeometry(TBC::GeometryBase* pGeometry)
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

void Renderer::SetViewport(const PixelRect& pViewport)
{
	assert(pViewport.mLeft <= pViewport.mRight && 
		pViewport.mTop <= pViewport.mBottom);
	mViewport = pViewport;
	RecalculateFrustumPlanes();
}

const PixelRect& Renderer::GetViewport() const
{
	return mViewport;
}

void Renderer::SetViewFrustum(float pFOVAngle, float pNear, float pFar)
{
	assert(pFOVAngle > 0 && pFOVAngle < 180 && pNear > 0 && pNear < pFar);
	mFOVAngle = pFOVAngle;
	mNear = pNear;
	mFar = pFar;
	RecalculateFrustumPlanes();
}

void Renderer::GetViewFrustum(float& pFOVAngle, float& pNear, float& pFar)
{
	pFOVAngle = mFOVAngle;
	pNear = mNear;
	pFar = mFar;
}

void Renderer::RecalculateFrustumPlanes()
{
	float lAngle = Math::Deg2Rad(90.0f - mFOVAngle * 0.5f);
	mFrustumPlanes[0].Set(-(float)sin(lAngle), (float)cos(lAngle), 0); // Right plane.
	mFrustumPlanes[1].Set( (float)sin(lAngle), (float)cos(lAngle), 0); // Left plane.

	lAngle = (float)atan(tan(lAngle) / GetAspectRatio());
	mFrustumPlanes[2].Set(0, (float)sin(lAngle), -(float)cos(lAngle)); // Top plane.
	mFrustumPlanes[3].Set(0, (float)sin(lAngle),  (float)cos(lAngle)); // Bottom plane.
}

float Renderer::CalcFOVAngle(float pReferenceAngle, float pAspectRatio)
{
	float lRatio = pAspectRatio * 3.0f / 4.0f;
	return Math::Rad2Deg(atan(tan(Math::Deg2Rad(pReferenceAngle * 0.5f)) * lRatio) * 2.0f);
}

void Renderer::SetClippingRect(const PixelRect& pRect)
{
	mClippingRect = pRect;
}

void Renderer::ReduceClippingRect(const PixelRect& pRect)
{
	Renderer::SetClippingRect(mClippingRect.GetOverlap(pRect));
}

void Renderer::ResetClippingRect()
{
	SetClippingRect(PixelRect(0, 0, mScreen->GetWidth(), mScreen->GetHeight()));
}

const PixelRect& Renderer::GetClippingRect() const
{
	return mClippingRect;
}

void Renderer::SetCameraTransformation(const TransformationF& pTransformation)
{
	mCameraTransformation = pTransformation;
}

const TransformationF& Renderer::GetCameraTransformation()
{
	return mCameraTransformation;
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

void Renderer::SetCompressedTexturesEnabled(bool pEnabled)
{
	mCompressedTexturesEnabled = pEnabled;
}

bool Renderer::GetCompressedTexturesEnabled()
{
	return mCompressedTexturesEnabled;
}

void Renderer::SetShadowsEnabled(bool pEnabled, ShadowHint pHint)
{
	mShadowsEnabled = pEnabled;
	mShadowHint = pHint;
}

bool Renderer::GetShadowsEnabled()
{
	return mShadowsEnabled;
}

void Renderer::SetShadowUpdateFrameDelay(unsigned int pFrameDelay)
{
	mShadowUpdateFrameDelay = pFrameDelay;
}

void Renderer::SetFallbackMaterialEnabled(bool pEnabled)
{
	mFallbackMaterialEnabled = pEnabled;
}

bool Renderer::GetFallbackMaterialEnabled()
{
	return mFallbackMaterialEnabled;
}

void Renderer::SetDepthSortingEnabled(bool pEnabled)
{
	Material::SetDepthSortingEnabled(pEnabled);
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

void Renderer::ResetAmbientLight()
{
	SetAmbientLight(mOriginalAmbientRed, mOriginalAmbientGreen, mOriginalAmbientBlue);
}

int Renderer::AllocLight()
{
	int lLightIndex = mLightIDManager.GetFreeId();

	if (lLightIndex != INVALID_LIGHT &&
		mLightData[lLightIndex].mEnabled == false)
	{
		mLightIndex[mLightCount] = lLightIndex;
		mLightData[lLightIndex].mIndex = mLightCount;
		mLightCount++;
	}
	return lLightIndex;
}

Renderer::LightID Renderer::AddDirectionalLight(LightHint pHint,
						float pDirX, float pDirY, float pDirZ,
						float pRed, float pGreen, float pBlue,
						float pShadowRange)
{
	int lLightIndex = AllocLight();
	if (lLightIndex == INVALID_LIGHT)
		return INVALID_LIGHT;

	mLightData[lLightIndex].mType = LIGHT_DIRECTIONAL;
	mLightData[lLightIndex].mHint = pHint;
	mLightData[lLightIndex].mPosition.Set(0, 0, 0);
	mLightData[lLightIndex].mDirection.Set(pDirX, pDirY, pDirZ);
	mLightData[lLightIndex].mColor[0]            = pRed;
	mLightData[lLightIndex].mColor[1]            = pGreen;
	mLightData[lLightIndex].mColor[2]            = pBlue;
	mLightData[lLightIndex].mRadius                = 0;
	mLightData[lLightIndex].mShadowRange           = pShadowRange;
	mLightData[lLightIndex].mCutoffAngle           = 89;
	mLightData[lLightIndex].mSpotExponent          = 1;
	mLightData[lLightIndex].mTransformationChanged = true;
	mLightData[lLightIndex].mEnabled               = true;

	mLightData[lLightIndex].mDirection.Normalize();

	return (LightID)lLightIndex;
}

Renderer::LightID Renderer::AddPointLight(LightHint pHint,
					  float pPosX, float pPosY, float pPosZ,
					  float pRed, float pGreen, float pBlue,
					  float pLightRadius,
					  float pShadowRange)
{
	int lLightIndex = AllocLight();
	if (lLightIndex == INVALID_LIGHT)
		return INVALID_LIGHT;

	// Scale the light intensity (makes this function accept lower rgb values).
	const float lLightIntensityFactor = 100.0f;
	pRed   *= lLightIntensityFactor;
	pGreen *= lLightIntensityFactor;
	pBlue  *= lLightIntensityFactor;

	mLightData[lLightIndex].mType = LIGHT_POINT;
	mLightData[lLightIndex].mHint = pHint;
	mLightData[lLightIndex].mPosition.Set(pPosX, pPosY, pPosZ);
	mLightData[lLightIndex].mDirection.Set(0, 0, 0);
	mLightData[lLightIndex].mColor[0]            = pRed;
	mLightData[lLightIndex].mColor[1]            = pGreen;
	mLightData[lLightIndex].mColor[2]            = pBlue;
	mLightData[lLightIndex].mRadius                = pLightRadius;
	mLightData[lLightIndex].mShadowRange           = pShadowRange;
	mLightData[lLightIndex].mCutoffAngle           = 89;
	mLightData[lLightIndex].mSpotExponent          = 1;
	mLightData[lLightIndex].mTransformationChanged = true;
	mLightData[lLightIndex].mEnabled               = true;

	return (LightID)lLightIndex;
}

Renderer::LightID Renderer::AddSpotLight(LightHint pHint,
					 float pPosX, float pPosY, float pPosZ,
					 float pDirX, float pDirY, float pDirZ,
					 float pRed, float pGreen, float pBlue,
					 float pCutoffAngle,
					 float pSpotExponent,
					 float pLightRadius,
					 float pShadowRange)
{
	int lLightIndex = AllocLight();
	if (lLightIndex == INVALID_LIGHT)
		return INVALID_LIGHT;

	// Clamp cutoff angle to a value between 0 and 89 degrees.
	// OpenGL accepts 90 degrees, but we'll set max to 89 in order to be able
	// to render shadow maps.
	pCutoffAngle = fabs(pCutoffAngle);
	pCutoffAngle = pCutoffAngle > 89.0f ? 89.0f : pCutoffAngle;

	// Scale the light intensity (makes this function accept lower rgb values).
	const float lLightIntensityFactor = 100.0f;
	pRed   *= lLightIntensityFactor;
	pGreen *= lLightIntensityFactor;
	pBlue  *= lLightIntensityFactor;

	mLightData[lLightIndex].mType = LIGHT_SPOT;
	mLightData[lLightIndex].mHint = pHint;
	mLightData[lLightIndex].mPosition.Set(pPosX, pPosY, pPosZ);
	mLightData[lLightIndex].mDirection.Set(pDirX, pDirY, pDirZ);
	mLightData[lLightIndex].mColor[0]            = pRed;
	mLightData[lLightIndex].mColor[1]            = pGreen;
	mLightData[lLightIndex].mColor[2]            = pBlue;
	mLightData[lLightIndex].mRadius                = pLightRadius;
	mLightData[lLightIndex].mShadowRange           = pShadowRange;
	mLightData[lLightIndex].mCutoffAngle           = pCutoffAngle;
	mLightData[lLightIndex].mSpotExponent          = pSpotExponent;
	mLightData[lLightIndex].mTransformationChanged = true;
	mLightData[lLightIndex].mEnabled               = true;

	mLightData[lLightIndex].mDirection.Normalize();

	const float lEpsilon = 1e-6f;

	// Generate an orientation for the light.
	Vector3DF lAxisX;
	Vector3DF lAxisY;
	Vector3DF lAxisZ;

	// If light direction is pointing up.
	if (mLightData[lLightIndex].mDirection.y >= (1.0f - lEpsilon))
	{
		lAxisX = mLightData[lLightIndex].mDirection / Vector3DF(0, 0, 1);
		lAxisY = lAxisX / mLightData[lLightIndex].mDirection;
		lAxisZ = mLightData[lLightIndex].mDirection;
	}
	else
	{
		lAxisX = mLightData[lLightIndex].mDirection / Vector3DF(0, 1, 0);
		lAxisY = lAxisX / mLightData[lLightIndex].mDirection;
		lAxisZ = mLightData[lLightIndex].mDirection;
	}

	lAxisX.Normalize();
	lAxisY.Normalize();
	lAxisZ.Normalize();

	mLightData[lLightIndex].mOrientation.SetAxisX(lAxisX);
	mLightData[lLightIndex].mOrientation.SetAxisY(lAxisY);
	mLightData[lLightIndex].mOrientation.SetAxisZ(lAxisZ);

	mNumSpotLights++;

	return (LightID)lLightIndex;
}

void Renderer::RemoveLight(LightID pLightID)
{
	int lLightIndex = (int)pLightID;

	if (lLightIndex == INVALID_LIGHT)
		return;

	mLightData[lLightIndex].mEnabled = false;

	if (mLightData[lLightIndex].mType == Renderer::LIGHT_SPOT)
	{
		mLightData[lLightIndex].mShadowMapGeometrySet.RemoveAll();

		if (mLightData[lLightIndex].mShadowMapID != 0)
		{
			mLightData[lLightIndex].mShadowMapID = ReleaseShadowMap(mLightData[lLightIndex].mShadowMapID);
		}

		mNumSpotLights--;
	}

	// The light is being removed, "defrag" the indices lookup..
	for (int i = mLightData[lLightIndex].mIndex; i < (mLightCount - 1); i++)
	{
		mLightData[mLightIndex[i]].mIndex--;
		mLightIndex[i] = mLightIndex[i + 1];
	}
	mLightCount--;

	mLightIDManager.RecycleId(lLightIndex);
}

void Renderer::RemoveAllLights()
{
	int lLightID[MAX_LIGHTS];
	int lLightCount = mLightCount;
	::memcpy(lLightID, mLightIndex, mLightCount * sizeof(int));
	for (int i = 0; i < lLightCount; ++i)
	{
		RemoveLight((LightID)lLightID[i]);
	}
}

int Renderer::GetMaxLights()
{
	return MAX_LIGHTS;
}

int Renderer::GetLightCount()
{
	return mLightCount;
}

void Renderer::SetShadowMapOptions(LightID pLightID,
				   unsigned char pLog2Res,
				   float pNearPlane,
				   float pFarPlane)
{
	int lLightIndex = (int)pLightID;
	if (lLightIndex == INVALID_LIGHT)
		return;

	// Can't change settings when the shadow map has been generated.
	// TODO: Replace '0' with something else...
	if (mLightData[lLightIndex].mShadowMapID == 0)
	{
		mLightData[lLightIndex].mShadowMapRes = (1 << pLog2Res);
		mLightData[lLightIndex].mShadowMapNear = pNearPlane;
		mLightData[lLightIndex].mShadowMapFar  = pFarPlane;
	}
}

void Renderer::SetLightPosition(LightID pLightID, float pX, float pY, float pZ)
{
	int lLightIndex = (int)pLightID;

	if (lLightIndex == INVALID_LIGHT)
		return;

	if (mLightData[lLightIndex].mType == Renderer::LIGHT_POINT ||
	   mLightData[lLightIndex].mType == Renderer::LIGHT_SPOT)
	{
		if (mLightData[lLightIndex].mPosition != Vector3DF(pX, pY, pZ))
		{
			mLightData[lLightIndex].mTransformationChanged = true;
		}

		mLightData[lLightIndex].mPosition.Set(pX, pY, pZ);
	}
}

void Renderer::SetLightDirection(LightID pLightID, float pX, float pY, float pZ)
{
	int lLightIndex = (int)pLightID;

	if (lLightIndex == INVALID_LIGHT)
		return;

	if (mLightData[lLightIndex].mType == Renderer::LIGHT_DIRECTIONAL ||
	   mLightData[lLightIndex].mType == Renderer::LIGHT_SPOT)
	{
		Vector3DF lPrevDir(mLightData[lLightIndex].mDirection);
		mLightData[lLightIndex].mDirection.Set(pX, pY, pZ);
		mLightData[lLightIndex].mDirection.Normalize();

		if (lPrevDir != mLightData[lLightIndex].mDirection)
		{
			mLightData[lLightIndex].mTransformationChanged = true;
		}

		if (mLightData[lLightIndex].mType == Renderer::LIGHT_SPOT)
		{
			const float lEpsilon = 1e-6f;

			// Generate an orientation for the light.
			Vector3DF lAxisX;
			Vector3DF lAxisY;
			Vector3DF lAxisZ;

			// If light direction is pointing up.
			if (mLightData[lLightIndex].mDirection.y >= (1.0f - lEpsilon))
			{
				lAxisX = mLightData[lLightIndex].mDirection / Vector3DF(1, 0, 0);
				lAxisY = mLightData[lLightIndex].mDirection / lAxisX;
				lAxisZ = mLightData[lLightIndex].mDirection;
			}
			else
			{
				lAxisX = Vector3DF(0, 1, 0) / mLightData[lLightIndex].mDirection;
				lAxisY = mLightData[lLightIndex].mDirection / lAxisX;
				lAxisZ = mLightData[lLightIndex].mDirection;
			}

			lAxisX.Normalize();
			lAxisY.Normalize();
			lAxisZ.Normalize();

			mLightData[lLightIndex].mOrientation.SetAxisX(lAxisX);
			mLightData[lLightIndex].mOrientation.SetAxisY(lAxisY);
			mLightData[lLightIndex].mOrientation.SetAxisZ(lAxisZ);
		}
	}
}

void Renderer::GetLightPosition(LightID pLightID, float& pX, float& pY, float& pZ)
{
	int lLightIndex = (int)pLightID;

	pX = 0;
	pY = 0;
	pZ = 0;

	if (lLightIndex != INVALID_LIGHT)
	{
		pX = (float)mLightData[lLightIndex].mPosition.x;
		pY = (float)mLightData[lLightIndex].mPosition.y;
		pZ = (float)mLightData[lLightIndex].mPosition.z;
	}
}

void Renderer::GetLightDirection(LightID pLightID, float& pX, float& pY, float& pZ)
{
	int lLightIndex = (int)pLightID;

	pX = 0;
	pY = 0;
	pZ = 0;

	if (lLightIndex != INVALID_LIGHT)
	{
		pX = (float)mLightData[lLightIndex].mDirection.x;
		pY = (float)mLightData[lLightIndex].mDirection.y;
		pZ = (float)mLightData[lLightIndex].mDirection.z;
	}
}

void Renderer::GetLightColor(LightID pLightID, float& pR, float& pG, float& pB)
{
	int lLightIndex = (int)pLightID;

	pR = 0;
	pG = 0;
	pB = 0;

	if (lLightIndex != INVALID_LIGHT && mLightData[lLightIndex].mEnabled == true)
	{
		pR = mLightData[lLightIndex].mColor[0];
		pG = mLightData[lLightIndex].mColor[1];
		pB = mLightData[lLightIndex].mColor[2];
	}
}

Renderer::LightType Renderer::GetLightType(LightID pLightID)
{
	int lLightIndex = (int)pLightID;

	if (lLightIndex == INVALID_LIGHT)
		return LIGHT_INVALID;

	return mLightData[lLightIndex].mType;
}

float Renderer::GetLightCutoffAngle(LightID pLightID)
{
	int lLightIndex = (int)pLightID;

	if (lLightIndex == INVALID_LIGHT)
		return 180;

	return mLightData[lLightIndex].mCutoffAngle;
}

float Renderer::GetLightSpotExponent(LightID pLightID)
{
	int lLightIndex = (int)pLightID;

	if (lLightIndex == INVALID_LIGHT)
		return 180;

	return mLightData[lLightIndex].mSpotExponent;
}

void Renderer::SortLights(const Vector3DF& pReferencePosition)
{
	smRenderer = this;
	smReferencePosition = pReferencePosition;
	::qsort(mLightIndex, mLightCount, sizeof(int), LightCompare);
}

int Renderer::LightCompare(const void* pLight1, const void* pLight2)
{
	const LightData& lLight1 = smRenderer->GetLightData(*(int*)pLight1);
	const LightData& lLight2 = smRenderer->GetLightData(*(int*)pLight2);

	float lInfluence1 = GetLightInfluence(lLight1);
	float lInfluence2 = GetLightInfluence(lLight2);

	if (lInfluence1 < lInfluence2)
		return -1;
	else if (lInfluence1 > lInfluence2)
		return 1;
	else
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
		}
		break;
		default: break;
	}
	return (lInfluence);
}

Renderer::LightID Renderer::GetClosestLight(int pIndex)
{
	return (LightID)GetLightIndex(pIndex);
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
	assert(lTexture->mIsCubeMap == pTexture->IsCubeMap());

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

Renderer::GeometryID Renderer::AddGeometry(TBC::GeometryBase* pGeometry, MaterialType pMaterialType, Shadows pShadows)
{
	if ((int)pMaterialType < 0 || (int)pMaterialType >= Renderer::NUM_MATERIALTYPES)
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
		TBC::GeometryBase* lParentGeometry = ((TBC::GeometryReference*)pGeometry)->GetParentGeometry();
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
	assert(lGeomIter != mGeometryTable.End());
	GeometryData* lGeometryData = *lGeomIter;

	/*JMB
	if (lGeometryData->mGeometry->IsGeometryReference() == true)
	{
		mLog.AError("Could not add texture to geometry reference!");
		assert(false);
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
					lOk = true;
				}
			}
			if (!lOk)
			{
				mLog.AError("Could not add texture - no more slots available!");
				assert(false);
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
				assert(y != mGeometryTable.End());
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

void Renderer::RemoveGeometry(GeometryID pGeometryID)
{
	if (pGeometryID == INVALID_GEOMETRY)
	{
		return;
	}

	GeometryTable::Iterator lGeomIter;
	lGeomIter = mGeometryTable.Find(pGeometryID);
	assert(lGeomIter != mGeometryTable.End());
	if (lGeomIter != mGeometryTable.End())
	{
		GeometryData* lGeometryData = *lGeomIter;
		lGeometryData->mGeometry->RemoveListener(this);

		int i;
		for (i = 0; i < MAX_SHADOW_VOLUMES; i++)
		{
			if (lGeometryData->mShadowVolume[i] != 0)
			{
				RemoveShadowVolume(lGeometryData->mShadowVolume[i]);
			}
		}

		// Remove the geometry from all spot lights, in case it's added there.
		for (i = 0; i < mLightCount; i++)
		{
			LightData& lLight = mLightData[mLightIndex[i]];
			if (lLight.mType == Renderer::LIGHT_SPOT)
			{
				if (lLight.mShadowMapGeometrySet.Remove(lGeometryData) == true)
					lLight.mShadowMapNeedUpdate = true;
			}
		}

		mGeometryTable.Remove(lGeomIter);
		mGeometryIDManager.RecycleId((int)pGeometryID);

		if (lGeometryData->mGeometry->IsGeometryReference() == false)
		{
			delete (lGeometryData->mTA);
			lGeometryData->mTA = 0;
		}
		else
		{
			TBC::GeometryBase* lParentGeometry = ((TBC::GeometryReference*)lGeometryData->mGeometry)->GetParentGeometry();
			GeometryData* lParentGeometryData = (GeometryData*)lParentGeometry->GetRendererData();
			lParentGeometryData->mReferenceSet.erase(lGeometryData->mGeometryID);
		}
		ReleaseGeometry(lGeometryData->mGeometry, GRO_REMOVE_FROM_MATERIAL);

		GeometryData::GeometryIDSet::iterator x;
		for (x = lGeometryData->mReferenceSet.begin(); x != lGeometryData->mReferenceSet.end(); ++x)
		{
			// Remove references recursively.
			RemoveGeometry(*x);
		}

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
	assert(pGeometryID != INVALID_GEOMETRY);
	MaterialType lMaterial = MAT_NULL;
	GeometryTable::Iterator x = mGeometryTable.Find(pGeometryID);
	assert(x != mGeometryTable.End());
	if (x != mGeometryTable.End())
	{
		GeometryData* lGeometryData = *x;
		lMaterial = lGeometryData->mMaterialType;
	}
	return (lMaterial);
}

Renderer::Shadows Renderer::GetShadows(GeometryID pGeometryID)
{
	assert(pGeometryID != INVALID_GEOMETRY);
	Shadows lShadows = NO_SHADOWS;
	GeometryTable::Iterator x = mGeometryTable.Find(pGeometryID);
	assert(x != mGeometryTable.End());
	if (x != mGeometryTable.End())
	{
		GeometryData* lGeometryData = *x;
		lShadows = lGeometryData->mShadow;
	}
	return (lShadows);
}

void Renderer::UpdateShadowMaps()
{
	for (int i = 0; i < (int)NUM_MATERIALTYPES; i++)
	{
		TBC::GeometryBase* lGeometry = mMaterial[i]->GetFirstGeometry();
		while(lGeometry != 0)
		{
			UpdateShadowMaps(lGeometry);
			lGeometry = mMaterial[i]->GetNextGeometry();
		}
	}
}

void Renderer::UpdateShadowMaps(TBC::GeometryBase* pGeometry)
{
	GeometryData* lGeometry = (GeometryData*)pGeometry->GetRendererData();
	SortLights(lGeometry->mGeometry->GetTransformation().GetPosition());

	/*
		Make sure shadow maps are up to date.
	*/
	if (mShadowHint == Renderer::SH_VOLUMES_AND_MAPS)
	{
		for (int i = 0; i < mLightCount; i++)
		{
			LightData& lLight = mLightData[mLightIndex[i]];

			if (lLight.mType == Renderer::LIGHT_SPOT)
			{
				if (pGeometry->GetTransformationChanged() == true)
					lLight.mShadowMapNeedUpdate = true;

				float lDist = lLight.mPosition.GetDistanceSquared(pGeometry->GetTransformation().GetPosition());
				float lMinDist = lLight.mRadius + pGeometry->GetBoundingRadius();
				lMinDist *= lMinDist;

				if (lDist < lMinDist)
				{
					if (lLight.mShadowMapGeometrySet.Insert(lGeometry) == true)
						lLight.mShadowMapNeedUpdate = true;
				}
				else
				{
					if (lLight.mShadowMapGeometrySet.Remove(lGeometry) == true)
						lLight.mShadowMapNeedUpdate = true;
				}
			}
		}
	}

	bool lShadowsUpdated = false;

	// Update shadow volumes.
	if (mShadowsEnabled == true && 
	   lGeometry->mShadow == Renderer::CAST_SHADOWS)
	{
		bool lGeomTransformationChanged = pGeometry->GetTransformationChanged();

		// Iterate over all the closest light sources and update shadow volumes.
		int lLoopMax = MAX_SHADOW_VOLUMES < mLightCount ? MAX_SHADOW_VOLUMES : mLightCount;
		int i;
		for (i = 0; i < lLoopMax; i++)
		{
			bool lProcessLight = false;
			LightData& lLight = mLightData[mLightIndex[i]];

			if (lLight.mEnabled == true)
			{
				if (lLight.mType == Renderer::LIGHT_POINT ||
				   (lLight.mType == Renderer::LIGHT_SPOT && mShadowHint == Renderer::SH_VOLUMES_ONLY))
				{
					float lDist = lLight.mPosition.GetDistanceSquared(pGeometry->GetTransformation().GetPosition());
					float lMinDist = lLight.mRadius + pGeometry->GetBoundingRadius();
					lMinDist *= lMinDist;

					if (lDist < lMinDist)
					{
						lProcessLight = true;
					}
				}
				else if(lLight.mType == Renderer::LIGHT_DIRECTIONAL)
				{
					lProcessLight = true;
				}
			}

			if (lProcessLight == true)
			{
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

						if (lLight.mType == Renderer::LIGHT_DIRECTIONAL)
						{
							lShadowVolume->UpdateShadowVolume(lLight.mDirection,
											    lLight.mShadowRange, 
											    true);
						}
						else
						{
							lShadowVolume->UpdateShadowVolume(lLight.mPosition,
											    lLight.mShadowRange, 
											    false);
						}

						mShadowVolumeTable.Insert(lId, lShadowGeom);
						lGeometry->mShadowVolume[i] = (GeometryID)lId;

						lShadowsUpdated = BindShadowGeometry(lShadowVolume, lLight.mHint);
					}

					lGeometry->mLightID[i] = mLightIndex[i];
				}
				else if(pGeometry->GetAlwaysVisible() == true ||
					pGeometry->GetLastFrameVisible() == GetCurrentFrame())
				{
					if (	(lLight.mTransformationChanged    == true || 
						 lGeomTransformationChanged        == true ||
	 					 pGeometry->GetVertexDataChanged() == true ||
						 lGeometry->mLightID[i]         != mLightIndex[i]) &&
						(mCurrentFrame - lGeometry->mLastFrameShadowsUpdated) >= mShadowUpdateFrameDelay)
					{
						ShadowVolumeTable::Iterator x = mShadowVolumeTable.Find(lGeometry->mShadowVolume[i]);
						
						if (x != mShadowVolumeTable.End())
						{
							GeometryData* lShadowGeom = *x;
							ShadowVolume* lShadowVolume = (ShadowVolume*)lShadowGeom->mGeometry;
							if (lLight.mType == Renderer::LIGHT_DIRECTIONAL)
							{
								lShadowVolume->UpdateShadowVolume(lLight.mDirection,
												    lLight.mShadowRange, 
												    true);
							}
							else
							{
								lShadowVolume->UpdateShadowVolume(lLight.mPosition,
												    lLight.mShadowRange, 
												    false);
							}

							lShadowsUpdated = true;
							lGeometry->mLightID[i] = mLightIndex[i];
						}
						else
						{
							lGeometry->mShadowVolume[i] = (GeometryID)0;
							lGeometry->mLightID[i] = -1;
						}
					}
				}
			}
			else if(lGeometry->mShadowVolume[i] != 0)
			{
				// Light is too far away or has been disabled. Remove shadow volume.
				RemoveShadowVolume(lGeometry->mShadowVolume[i]);
				lGeometry->mLightID[i] = -1;
			}
		}// End for(i < Renderer::MAX_SHADOW_VOLUMES)

		pGeometry->SetTransformationChanged(false);
		pGeometry->SetVertexDataChanged(false);
	}

	if (lShadowsUpdated == true)
	{
		lGeometry->mLastFrameShadowsUpdated = mCurrentFrame;
	}
}

unsigned int Renderer::RenderScene()
{
	// Prepare projection data in order to be able to call CheckCulling().
	PrepareProjectionData();

	for (int i = 0; i < (int)NUM_MATERIALTYPES; i++)
	{
		mMaterial[i]->RenderAllGeometry(mCurrentFrame);
	}

	mCurrentFrame++;

	return mCurrentFrame;
}

unsigned int Renderer::GetCurrentFrame() const
{
	return mCurrentFrame;
}

bool Renderer::CheckFlag(unsigned int pFlags, unsigned int pFlag)
{
	return (pFlags & pFlag) != 0;
}

Renderer::LightID Renderer::AddDirectionalLight(LightHint pHint, const Vector3DF& pDir, const Color& pColor, float pLightIntensity, float pShadowRange)
{
	return AddDirectionalLight(pHint, (float)pDir.x, (float)pDir.y, (float)pDir.z, pLightIntensity * (float)pColor.mRed / 255.0f, pLightIntensity * (float)pColor.mGreen / 255.0f, pLightIntensity * (float)pColor.mBlue  / 255.0f, pShadowRange);
}

Renderer::LightID Renderer::AddPointLight(LightHint pHint, const Vector3DF& pPos, const Color& pColor, float pLightIntensity, float pLightRadius, float pShadowRange)
{
	return AddPointLight(pHint, (float)pPos.x, (float)pPos.y, (float)pPos.z, pLightIntensity * (float)pColor.mRed   / 255.0f, pLightIntensity * (float)pColor.mGreen / 255.0f, pLightIntensity * (float)pColor.mBlue  / 255.0f, pLightRadius, pShadowRange);
}

Renderer::LightID Renderer::AddSpotLight(LightHint pHint, const Vector3DF& pPos, const Vector3DF& pDir, const Color& pColor, float pLightIntensity, float pCutoffAngle, float pSpotExponent, float pLightRadius, float pShadowRange)
{
	return AddSpotLight(pHint, (float)pPos.x, (float)pPos.y, (float)pPos.z, (float)pDir.x, (float)pDir.y, (float)pDir.z, pLightIntensity * (float)pColor.mRed   / 255.0f, pLightIntensity * (float)pColor.mGreen / 255.0f, pLightIntensity * (float)pColor.mBlue  / 255.0f, pCutoffAngle, pSpotExponent, pLightRadius, pShadowRange);
}

Material* Renderer::GetMaterial(MaterialType pMaterialType) const
{
	assert(pMaterialType >= MAT_NULL && pMaterialType <= NUM_MATERIALTYPES);
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

Renderer::TextureData* Renderer::GetEnvTexture() const
{
	return mEnvTexture;
}

Renderer::LightData& Renderer::GetLightData(int pLightIndex)
{
	return mLightData[pLightIndex];
}

int Renderer::GetLightIndex(int pIndex) const
{
	return mLightIndex[pIndex];
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

	const Canvas* lScreen = GetScreen();
	float64 lAspect = (float64)lScreen->GetWidth() / (float64)lScreen->GetHeight();

	// Convert to radians.
	float64 lFOVAngleRad = (double)lFOVAngle * PI / 180.0;

	mDX = (1.0 / tan(lFOVAngleRad / 2.0));
	mDY = mDX * lAspect;
}

PixelRect Renderer::GetBoundingRect(const Vector3DF* pVertex, int pNumVertices) const
{
	const Canvas* lScreen = GetScreen();

	if (pNumVertices <= 2)
	{
		mLog.AError("GetBoundingRect() - NumVertices < 3!");
		return PixelRect(0,0,0,0);
	}

	PixelRect lRect(0, 0, 0, 0);

	int lPrevIndex = pNumVertices - 1;
	Vector3DF lPrev(mCamTransform.InverseTransform(Vector3DF(pVertex[lPrevIndex].x, pVertex[lPrevIndex].y, pVertex[lPrevIndex].z)));

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
		Vector3DF lCurrent(mCamTransform.InverseTransform(Vector3DF(pVertex[i].x, pVertex[i].y, pVertex[i].z)));

		if ((lPrev.y <= 0 && lCurrent.y > 0) ||
		    (lPrev.y >  0 && lCurrent.y <= 0))
		{
			// Clip at z = 0.
			Vector3DF lDiff = lCurrent - lPrev;
			Vector3DF lClipPos = lPrev + lDiff * (-lPrev.y / lDiff.y);

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

bool Renderer::IsFacingFront(const Vector3DF* pVertex, int pNumVertices)
{
	if (pNumVertices < 3)
		return false;

	// Use the cross product constructor to create the surface normal.
	Vector3DF lNormal(pVertex[1] - pVertex[0], pVertex[2] - pVertex[0]);
	Vector3DF lCamVector(pVertex[0] - mCamTransform.GetPosition());

	return lNormal.Dot(lCamVector) > 0;
}

float Renderer::GetAspectRatio() const
{
	return (float)mViewport.GetWidth() / (float)mViewport.GetHeight();
}

bool Renderer::CheckCulling(const TransformationF& pTransform, double pBoundingRadius)
{
	const Vector3DF& lPos = pTransform.GetPosition();

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

		if (lGeometry->mGeometry->IsGeometryReference() == false)
		{
			delete lGeometry->mTA;
			lGeometry->mTA = 0;
		}

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
	for (int i = 0; i < mLightCount; i++)
	{
		int lIndex = mLightIndex[i];

		if (mLightData[lIndex].mType == Renderer::LIGHT_SPOT)
		{
			mLightData[lIndex].mShadowMapID = ReleaseShadowMap(mLightData[lIndex].mShadowMapID);
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
		}

		mGeometryIDManager.RecycleId(pShadowVolume);
		pShadowVolume = (GeometryID)0;
	}
}

int Renderer::QueryTriangleCount(bool pVisibleOnly)
{
	int lTriangleCount = 0;
	for (int i = 0; i < (int)NUM_MATERIALTYPES; ++i)
	{
		TBC::GeometryBase* lGeometry = mMaterial[i]->GetFirstVisibleGeometry();
		while(lGeometry != 0)
		{
			// PreRender performs the frustum culling.
			if(!pVisibleOnly || PreRender(lGeometry) == true)
			{
				lTriangleCount += lGeometry->GetTriangleCount();
			}
			lGeometry = mMaterial[i]->GetNextVisibleGeometry();
		}
	}
	return lTriangleCount;
}

Renderer* Renderer::smRenderer = 0;
Vector3DF Renderer::smReferencePosition;
LOG_CLASS_DEFINE(UI_GFX_3D, Renderer);

} // End namespace.
