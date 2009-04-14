
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#include "../../Lepra/Include/Log.h"
#include "../../Lepra/Include/Math.h"
#include "../../UiLepra/Include/UiOpenGLExtensions.h"
#include "../../TBC/Include/GeometryBase.h"
#include "../Include/UiOpenGLMaterials.h"
#include "../Include/UiOpenGLRenderer.h"



namespace UiTbc
{

/*
	OpenGLMaterial
*/

GLenum OpenGLMaterial::smPrimitiveLUT[2] = {GL_TRIANGLES, GL_TRIANGLE_STRIP};

Material::RemoveStatus OpenGLMaterial::RemoveGeometry(TBC::GeometryBase* pGeometry)
{
	Material::RemoveStatus lStatus = Material::RemoveGeometry(pGeometry);
	if (lStatus == Material::NOT_REMOVED && mFallBackMaterial)
	{
		lStatus = mFallBackMaterial->RemoveGeometry(pGeometry);

		if (lStatus == Material::REMOVED)
		{
			lStatus = Material::REMOVED_FROM_FALLBACK;
		}
	}
	return (lStatus);
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

/*
	OpenGLMatSingleColorSolid
*/

void OpenGLMatSingleColorSolid::RenderAllGeometry(unsigned int pCurrentFrame)
{
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_LOGIC_OP);
	glDisable(GL_TEXTURE_2D);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_COLOR_MATERIAL);

	glColorMaterial(GL_FRONT, GL_AMBIENT);

	Material::RenderAllGeometry(pCurrentFrame);
}

void OpenGLMatSingleColorSolid::RenderGeometry(TBC::GeometryBase* pGeometry)
{
	PrepareBasicMaterialSettings(pGeometry);

	OpenGLRenderer::OGLGeometryData* lGeometry = (OpenGLRenderer::OGLGeometryData*)pGeometry->GetRendererData();

	if (pGeometry->GetNormalData() == 0)
	{
		glDisableClientState(GL_NORMAL_ARRAY);
	}

	if (UiLepra::OpenGLExtensions::BufferObjectsSupported() == true)
	{
		GLuint lVertexBufferID = (GLuint)lGeometry->mVertexBufferID;
		GLuint lIndexBufferID  = (GLuint)lGeometry->mIndexBufferID;

		UiLepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, lVertexBufferID);
		glVertexPointer(3, GL_FLOAT, 0, 0);

		glNormalPointer(GL_FLOAT, 0, (GLvoid*)lGeometry->mNormalOffset);

		UiLepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lIndexBufferID);
		glDrawElements(OpenGLMaterial::GetGLElementType(pGeometry), 
			       pGeometry->GetIndexCount(),
			       GL_UNSIGNED_INT, 
			       0);
	}
	else
	{
		glVertexPointer(3, GL_FLOAT, 0, pGeometry->GetVertexData());
		glNormalPointer(GL_FLOAT, 0, pGeometry->GetNormalData());
		glDrawElements(OpenGLMaterial::GetGLElementType(pGeometry),
			       pGeometry->GetIndexCount(),
			       GL_UNSIGNED_INT,
			       pGeometry->GetIndexData());
	}

	if (pGeometry->GetNormalData() == 0)
	{
		glEnableClientState(GL_NORMAL_ARRAY);
	}

	((OpenGLRenderer*)GetRenderer())->ResetAmbientLight();
}

void OpenGLMatSingleColorSolid::PrepareBasicMaterialSettings(TBC::GeometryBase* pGeometry)
{
	TBC::GeometryBase::BasicMaterialSettings lMatSettings;
	pGeometry->GetBasicMaterialSettings(lMatSettings);

	glColor4f(lMatSettings.mRed,
		  lMatSettings.mGreen,
		  lMatSettings.mBlue,
		  lMatSettings.mAlpha);

	float lDiffuse = 1.0f - lMatSettings.mSpecular;
	float lDiffuseColor[] = 
	{
		lMatSettings.mRed   * lDiffuse,
		lMatSettings.mGreen * lDiffuse,
		lMatSettings.mBlue  * lDiffuse,
		1.0f
	};
	float lSpecularColor[] = 
	{
		lMatSettings.mRed   * lMatSettings.mSpecular,
		lMatSettings.mGreen * lMatSettings.mSpecular,
		lMatSettings.mBlue  * lMatSettings.mSpecular,
		1.0f
	};
	static float lWhite[] = {1.0f, 1.0f, 1.0f, 1.0f};

	glMaterialfv(GL_FRONT, GL_AMBIENT, lWhite);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, lDiffuseColor);
	glMaterialfv(GL_FRONT, GL_SPECULAR, lSpecularColor);
	//glMateriali(GL_FRONT, GL_SHININESS, (int)floor(lMatSettings.mSpecular * 128.0f));
	glMateriali(GL_FRONT, GL_SHININESS, 128);

	glShadeModel(lMatSettings.mSmooth ? GL_SMOOTH : GL_FLAT);

	((OpenGLRenderer*)GetRenderer())->SetGlobalMaterialReflectance(lMatSettings.mRed,
						  lMatSettings.mGreen,
						  lMatSettings.mBlue,
						  lMatSettings.mSpecular);

	((OpenGLRenderer*)GetRenderer())->AddAmbience(lMatSettings.mSelfIllumination,
				 lMatSettings.mSelfIllumination,
				 lMatSettings.mSelfIllumination);
}

/*
	OpenGLMatSingleColorBlended
*/

void OpenGLMatSingleColorBlended::RenderAllGeometry(unsigned int pCurrentFrame)
{
	glDisable(GL_ALPHA_TEST);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	Material::RenderAllGeometry(pCurrentFrame);
}


/*
	OpenGLMatVertexColorSolid
*/

bool OpenGLMatVertexColorSolid::AddGeometry(TBC::GeometryBase* pGeometry)
{
	if (pGeometry->GetColorData() != 0)
	{
		return OpenGLMaterial::AddGeometry(pGeometry);
	}
	else if(mFallBackMaterial != 0 && ((OpenGLRenderer*)GetRenderer())->GetFallbackMaterialEnabled())
	{
		mLog.AWarning("Material \"VertexColorSolid\", passing geometry to fallback material.");
		return mFallBackMaterial->AddGeometry(pGeometry);
	}

	return false;
}

void OpenGLMatVertexColorSolid::RenderAllGeometry(unsigned int pCurrentFrame)
{
	// This is the only state we need to activate, since the other ones
	// were activated by OpenGLMatSingleColorSolid.
	glEnableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	Material::RenderAllGeometry(pCurrentFrame);

	glDisableClientState(GL_COLOR_ARRAY);
}

void OpenGLMatVertexColorSolid::RenderGeometry(TBC::GeometryBase* pGeometry)
{
	PrepareBasicMaterialSettings(pGeometry);

	OpenGLRenderer::OGLGeometryData* lGeometry = (OpenGLRenderer::OGLGeometryData*)pGeometry->GetRendererData();

	if (UiLepra::OpenGLExtensions::BufferObjectsSupported() == true)
	{
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
			       GL_UNSIGNED_INT, 
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
			       GL_UNSIGNED_INT,
			       pGeometry->GetIndexData());
	}

	((OpenGLRenderer*)GetRenderer())->ResetAmbientLight();
}



/*
	OpenGLMatVertexColorBlended
*/

void OpenGLMatVertexColorBlended::RenderAllGeometry(unsigned int pCurrentFrame)
{
	// This is the only state we need to activate, since the other ones
	// were activated by previous materials.
	glEnableClientState(GL_COLOR_ARRAY);

	Material::RenderAllGeometry(pCurrentFrame);

	glDisableClientState(GL_COLOR_ARRAY);
}




/*
	OpenGLMatSingleTextureSolid
*/


bool OpenGLMatSingleTextureSolid::AddGeometry(TBC::GeometryBase* pGeometry)
{
	bool lAdded = false;
	if (/*lGeometry->mTA == 0 ||
	   lGeometry->mTA->mNumTextures == 0 ||*/
	   pGeometry->GetUVSetCount() == 0)
	{
		if (mFallBackMaterial != 0 && ((OpenGLRenderer*)GetRenderer())->GetFallbackMaterialEnabled())
		{
			mLog.AWarning("Material \"SingleTextureSolid\", passing geometry to fallback material.");
			lAdded = mFallBackMaterial->AddGeometry(pGeometry);
		}
	}
	else
	{
		lAdded = OpenGLMatSingleColorSolid::AddGeometry(pGeometry);
	}

	return (lAdded);
}

void OpenGLMatSingleTextureSolid::RenderAllGeometry(unsigned int pCurrentFrame)
{
	glEnable(GL_TEXTURE_2D);
	glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	//glTexEnvf(GL_TEXTURE_FILTER_CONTROL, GL_TEXTURE_LOD_BIAS, 0.0f);

	OpenGLMaterial::RenderAllGeometry(pCurrentFrame);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_TEXTURE_2D);

//	glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_COLOR_SINGLE);
}

void OpenGLMatSingleTextureSolid::RenderGeometry(TBC::GeometryBase* pGeometry)
{
	PrepareBasicMaterialSettings(pGeometry);

	OpenGLRenderer::OGLGeometryData* lGeometry = (OpenGLRenderer::OGLGeometryData*)pGeometry->GetRendererData();

	BindTexture(lGeometry->mTA->mMaps[0].mMapID[Texture::COLOR_MAP]);
	
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	OpenGLMaterial::UpdateTextureMatrix(lGeometry->mGeometry);

	if (UiLepra::OpenGLExtensions::BufferObjectsSupported() == true)
	{
		GLuint lVertexBufferID = (GLuint)lGeometry->mVertexBufferID;
		GLuint lIndexBufferID  = (GLuint)lGeometry->mIndexBufferID;

		UiLepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, lVertexBufferID);

		glVertexPointer(3, GL_FLOAT, 0, 0);
		glNormalPointer(GL_FLOAT, 0, (GLvoid*)lGeometry->mNormalOffset);
		glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)lGeometry->mUVOffset);

		UiLepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lIndexBufferID);

		glDrawElements(OpenGLMaterial::GetGLElementType(pGeometry), pGeometry->GetIndexCount(), GL_UNSIGNED_INT, 0);
	}
	else
	{
		glVertexPointer(3, GL_FLOAT, 0, pGeometry->GetVertexData());
		glNormalPointer(GL_FLOAT, 0, pGeometry->GetNormalData());
		glTexCoordPointer(2, GL_FLOAT, 0, pGeometry->GetUVData(0));
		glDrawElements(OpenGLMaterial::GetGLElementType(pGeometry), pGeometry->GetIndexCount(), GL_UNSIGNED_INT, pGeometry->GetIndexData());
	}

	((OpenGLRenderer*)GetRenderer())->ResetAmbientLight();
}

void OpenGLMatSingleTextureSolid::BindTexture(int pTextureID)
{
	glBindTexture(GL_TEXTURE_2D, pTextureID);

	mTextureParamMin = GL_NEAREST;
	mTextureParamMag = GL_NEAREST;
	
	if (((OpenGLRenderer*)GetRenderer())->GetTrilinearFilteringEnabled() == true)
	{
		// The trilinear setting overrides the other ones.
		mTextureParamMin = GL_LINEAR_MIPMAP_LINEAR;
		mTextureParamMag = GL_LINEAR;
	}
	else if(((OpenGLRenderer*)GetRenderer())->GetBilinearFilteringEnabled() == true)
	{
		if (((OpenGLRenderer*)GetRenderer())->GetMipMappingEnabled() == true)
			mTextureParamMin = GL_LINEAR_MIPMAP_NEAREST;
		else
			mTextureParamMin = GL_LINEAR;

		mTextureParamMag = GL_LINEAR;
	}
	else if(((OpenGLRenderer*)GetRenderer())->GetMipMappingEnabled() == true)
	{
		mTextureParamMin = GL_NEAREST_MIPMAP_NEAREST;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mTextureParamMin);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mTextureParamMag);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

/*
	OpenGLMatSingleTextureBlended
*/

void OpenGLMatSingleTextureBlended::RenderAllGeometry(unsigned int pCurrentFrame)
{
	OpenGLMatSingleTextureSolid::RenderAllGeometry(pCurrentFrame);
//	glDisable(GL_BLEND);
}

/*
	OpenGLMatSingleTextureAlphaTested
*/

void OpenGLMatSingleTextureAlphaTested::RenderAllGeometry(unsigned int pCurrentFrame)
{
	glEnable(GL_ALPHA_TEST);
	OpenGLMatSingleTextureSolid::RenderAllGeometry(pCurrentFrame);
	glDisable(GL_ALPHA_TEST);
}

void OpenGLMatSingleTextureAlphaTested::RenderGeometry(TBC::GeometryBase* pGeometry)
{
	TBC::GeometryBase::BasicMaterialSettings lMatSettings;
	pGeometry->GetBasicMaterialSettings(lMatSettings);
	glAlphaFunc(GL_GEQUAL, lMatSettings.mAlpha);

	OpenGLMatSingleTextureSolid::RenderGeometry(pGeometry);
}

/*
	OpenGLMatSingleColorEnvMapSolid
*/

bool OpenGLMatSingleColorEnvMapSolid::AddGeometry(TBC::GeometryBase* pGeometry)
{
	if (pGeometry->GetUVSetCount() == 0)
	{
		if (mFallBackMaterial != 0 && ((OpenGLRenderer*)GetRenderer())->GetFallbackMaterialEnabled())
		{
			mLog.AWarning("Material \"SingleColorEnvMapSolid\", passing geometry to fallback material.");
			return mFallBackMaterial->AddGeometry(pGeometry);
		}
		else
		{
			return false;
		}
	}

	return OpenGLMaterial::AddGeometry(pGeometry);
}

void OpenGLMatSingleColorEnvMapSolid::RenderAllGeometry(unsigned int pCurrentFrame)
{
	// This material requires two rendering passes, since OpenGL doesn't
	// support blending the texture with the base color of the material.
	// So, first we render a "single color material" pass,
	// and then we blend in the environment map on top of that.
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Pass 1, single color.
	mSingleColorPass = true;
	glDisable(GL_TEXTURE_2D);
	OpenGLMaterial::RenderAllGeometry(pCurrentFrame);
	glEnable(GL_TEXTURE_2D);

	// Pass 2, Render the enviroment map.
	mSingleColorPass = false;
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	int lEnvMapID = ((OpenGLRenderer*)GetRenderer())->GetEnvMapID();

	mTextureParamMin = GL_NEAREST;
	mTextureParamMag = GL_NEAREST;
	
	if (((OpenGLRenderer*)GetRenderer())->GetTrilinearFilteringEnabled() == true)
	{
		// The trilinear setting overrides the other ones.
		mTextureParamMin = GL_LINEAR_MIPMAP_LINEAR;
		mTextureParamMag = GL_LINEAR;
	}
	else if(((OpenGLRenderer*)GetRenderer())->GetBilinearFilteringEnabled() == true)
	{
		if (((OpenGLRenderer*)GetRenderer())->GetMipMappingEnabled() == true)
			mTextureParamMin = GL_LINEAR_MIPMAP_NEAREST;
		else
			mTextureParamMin = GL_LINEAR;

		mTextureParamMag = GL_LINEAR;
	}
	else if(((OpenGLRenderer*)GetRenderer())->GetMipMappingEnabled() == true)
	{
		mTextureParamMin = GL_NEAREST_MIPMAP_NEAREST;
	}

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

		glBindTexture(GL_TEXTURE_CUBE_MAP, lEnvMapID);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, mTextureParamMin);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, mTextureParamMag);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);
	}
	else
	{
		// Use sphere mapping.
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);

		glBindTexture(GL_TEXTURE_2D, lEnvMapID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mTextureParamMin);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mTextureParamMag);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

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

	GLboolean lBlendEnabled = glIsEnabled(GL_BLEND);

	glEnable(GL_BLEND);
	OpenGLMaterial::RenderAllGeometry(pCurrentFrame);

	if (!lBlendEnabled)
		glDisable(GL_BLEND);

	((OpenGLRenderer*)GetRenderer())->SetAmbientLight(lAmbientRed, lAmbientGreen, lAmbientBlue);

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
	}

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
}

void OpenGLMatSingleColorEnvMapSolid::PrepareBasicMaterialSettings(TBC::GeometryBase* pGeometry)
{
	if (mSingleColorPass == true)
	{
		OpenGLMatSingleColorSolid::PrepareBasicMaterialSettings(pGeometry);
	}
	else
	{
		// Prepare basic material settings using 100% specularity.
		TBC::GeometryBase::BasicMaterialSettings lMatSettings;
		pGeometry->GetBasicMaterialSettings(lMatSettings);

		glColor4f(lMatSettings.mRed,
			  lMatSettings.mGreen,
			  lMatSettings.mBlue,
			  lMatSettings.mSpecular);

		float lDiffuse[] = { 0, 0, 0, 1.0f };
		float lSpecular[] =
		{
			lMatSettings.mRed,
			lMatSettings.mGreen,
			lMatSettings.mBlue,
			1.0f
		};
		static float lWhite[] = {1.0f, 1.0f, 1.0f, 1.0f};

		glMaterialfv(GL_FRONT, GL_AMBIENT, lWhite);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, lDiffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, lSpecular);
		glMateriali(GL_FRONT, GL_SHININESS, 128);

		glShadeModel(lMatSettings.mSmooth ? GL_SMOOTH : GL_FLAT);

		((OpenGLRenderer*)GetRenderer())->SetGlobalMaterialReflectance(lMatSettings.mRed,
							  lMatSettings.mGreen,
							  lMatSettings.mBlue,
							  lMatSettings.mSpecular);
	}
}

/*
	OpenGLMatSingleEnvMapBlended
*/
/*
void OpenGLMatSingleColorEnvMapBlended::RenderAllGeometry(unsigned int pCurrentFrame)
{
	glEnable(GL_BLEND);
	OpenGLMatSingleColorEnvMapSolid::RenderAllGeometry(pCurrentFrame);
	glDisable(GL_BLEND);
}
*/
void OpenGLMatSingleColorEnvMapBlended::PrepareBasicMaterialSettings(TBC::GeometryBase* pGeometry)
{
	if (mSingleColorPass == true)
	{
		OpenGLMatSingleColorSolid::PrepareBasicMaterialSettings(pGeometry);
	}
	else
	{
		// Prepare basic material settings using 100% specularity.
		TBC::GeometryBase::BasicMaterialSettings lMatSettings;
		pGeometry->GetBasicMaterialSettings(lMatSettings);

		glColor4f(lMatSettings.mRed,
			  lMatSettings.mGreen,
			  lMatSettings.mBlue,
			  lMatSettings.mSpecular * lMatSettings.mAlpha);

		float lDiffuse[] = { 0, 0, 0, 1.0f };
		float lSpecular[] =
		{
			lMatSettings.mRed,
			lMatSettings.mGreen,
			lMatSettings.mBlue,
			1.0f
		};
		static float lWhite[] = {1.0f, 1.0f, 1.0f, 1.0f};

		glMaterialfv(GL_FRONT, GL_AMBIENT, lWhite);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, lDiffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, lSpecular);
		glMateriali(GL_FRONT, GL_SHININESS, 128);

		((OpenGLRenderer*)GetRenderer())->SetGlobalMaterialReflectance(lMatSettings.mRed,
							  lMatSettings.mGreen,
							  lMatSettings.mBlue,
							  lMatSettings.mSpecular);
	}
}


/*
	OpenGLMatSingleTextureEnvMapSolid
*/

bool OpenGLMatSingleTextureEnvMapSolid::AddGeometry(TBC::GeometryBase* pGeometry)
{
	if (/*lGeometry->mTA == 0 ||
	   lGeometry->mTA->mNumTextures == 0 ||*/
	   pGeometry->GetUVSetCount() == 0 ||
	   UiLepra::OpenGLExtensions::MultiTextureSupported() == false)
	{
		if (mFallBackMaterial != 0 && ((OpenGLRenderer*)GetRenderer())->GetFallbackMaterialEnabled())
		{
			mLog.AWarning("Material \"SingleTextureEnvMapSolid\", passing geometry to fallback material.");
			return mFallBackMaterial->AddGeometry(pGeometry);
		}
		else
		{
			return false;
		}
	}

	return OpenGLMatSingleTextureSolid::AddGeometry(pGeometry);
}

void OpenGLMatSingleTextureEnvMapSolid::RenderAllGeometry(unsigned int pCurrentFrame)
{
	// This material requires two rendering passes, since OpenGL doesn't
	// support blending the two textures the right way - not even with
	// multi texture support. But we still need multi texture support though.
	// So, first we render a normal "single texture" pass,
	// and then we blend in the environment map on top of that.
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	// Pass 1, single texture.
	mSingleTexturePass = true;
	glEnable(GL_TEXTURE_2D);
	OpenGLMatSingleTextureSolid::RenderAllGeometry(pCurrentFrame);

	// Pass 2, Render the enviroment map.
	mSingleTexturePass = false;

	UiLepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE1);
	UiLepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE1);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	int lEnvMapID = ((OpenGLRenderer*)GetRenderer())->GetEnvMapID();

	mTextureParamMin = GL_NEAREST;
	mTextureParamMag = GL_NEAREST;
	
	if (((OpenGLRenderer*)GetRenderer())->GetTrilinearFilteringEnabled() == true)
	{
		// The trilinear setting overrides the other ones.
		mTextureParamMin = GL_LINEAR_MIPMAP_LINEAR;
		mTextureParamMag = GL_LINEAR;
	}
	else if(((OpenGLRenderer*)GetRenderer())->GetBilinearFilteringEnabled() == true)
	{
		if (((OpenGLRenderer*)GetRenderer())->GetMipMappingEnabled() == true)
			mTextureParamMin = GL_LINEAR_MIPMAP_NEAREST;
		else
			mTextureParamMin = GL_LINEAR;

		mTextureParamMag = GL_LINEAR;
	}
	else if(((OpenGLRenderer*)GetRenderer())->GetMipMappingEnabled() == true)
	{
		mTextureParamMin = GL_NEAREST_MIPMAP_NEAREST;
	}

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

		glBindTexture(GL_TEXTURE_CUBE_MAP, lEnvMapID);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, mTextureParamMin);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, mTextureParamMag);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
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

		glBindTexture(GL_TEXTURE_2D, lEnvMapID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mTextureParamMin);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mTextureParamMag);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

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
	OpenGLMatSingleTextureSolid::RenderAllGeometry(pCurrentFrame);

	if (!lBlendEnabled)
		glDisable(GL_BLEND);

	((OpenGLRenderer*)GetRenderer())->SetAmbientLight(lAmbientRed, lAmbientGreen, lAmbientBlue);

	UiLepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE1);
	UiLepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE1);
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

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);

	UiLepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE0);
	UiLepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void OpenGLMatSingleTextureEnvMapSolid::RenderGeometry(TBC::GeometryBase* pGeometry)
{
	PrepareBasicMaterialSettings(pGeometry);

	OpenGLRenderer::OGLGeometryData* lGeometry = (OpenGLRenderer::OGLGeometryData*)pGeometry->GetRendererData();

	glBindTexture(GL_TEXTURE_2D, lGeometry->mTA->mMaps[0].mMapID[Texture::COLOR_MAP]);
	OpenGLMaterial::UpdateTextureMatrix(lGeometry->mGeometry);

	if (UiLepra::OpenGLExtensions::BufferObjectsSupported() == true)
	{
		GLuint lVertexBufferID = (GLuint)lGeometry->mVertexBufferID;
		GLuint lIndexBufferID  = (GLuint)lGeometry->mIndexBufferID;

		UiLepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, lVertexBufferID);

		glVertexPointer(3, GL_FLOAT, 0, 0);
		glNormalPointer(GL_FLOAT, 0, (GLvoid*)lGeometry->mNormalOffset);
		glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)lGeometry->mUVOffset);

		UiLepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lIndexBufferID);
		glDrawElements(OpenGLMaterial::GetGLElementType(pGeometry), 
			       pGeometry->GetIndexCount(),
			       GL_UNSIGNED_INT, 
			       0);
	}
	else
	{
		glVertexPointer(3, GL_FLOAT, 0, pGeometry->GetVertexData());
		glNormalPointer(GL_FLOAT, 0, pGeometry->GetNormalData());
		glTexCoordPointer(2, GL_FLOAT, 0, pGeometry->GetUVData(0));
		glDrawElements(OpenGLMaterial::GetGLElementType(pGeometry),
			       pGeometry->GetIndexCount(),
			       GL_UNSIGNED_INT,
			       pGeometry->GetIndexData());
	}
	((OpenGLRenderer*)GetRenderer())->ResetAmbientLight();
}

void OpenGLMatSingleTextureEnvMapSolid::PrepareBasicMaterialSettings(TBC::GeometryBase* pGeometry)
{
	if (mSingleTexturePass == true)
	{
		OpenGLMatSingleColorSolid::PrepareBasicMaterialSettings(pGeometry);
	}
	else
	{
		// Prepare basic material settings using 100% specularity.
		TBC::GeometryBase::BasicMaterialSettings lMatSettings;
		pGeometry->GetBasicMaterialSettings(lMatSettings);

		glColor4f(lMatSettings.mRed,
			  lMatSettings.mGreen,
			  lMatSettings.mBlue,
			  lMatSettings.mSpecular);

		float lDiffuse[] = { 0, 0, 0, 1.0f };
		float lSpecular[] =
		{
			lMatSettings.mRed,
			lMatSettings.mGreen,
			lMatSettings.mBlue,
			1.0f
		};
		static float lWhite[] = {1.0f, 1.0f, 1.0f, 1.0f};

		glMaterialfv(GL_FRONT, GL_AMBIENT, lWhite);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, lDiffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, lSpecular);
		glMateriali(GL_FRONT, GL_SHININESS, 128);

		((OpenGLRenderer*)GetRenderer())->SetGlobalMaterialReflectance(lMatSettings.mRed,
							  lMatSettings.mGreen,
							  lMatSettings.mBlue,
							  lMatSettings.mSpecular);
	}
}


/*
	OpenGLMatSingleTextureEnvMapBlended
*/
/*
void OpenGLMatSingleTextureEnvMapBlended::RenderAllGeometry(unsigned int pCurrentFrame)
{
	glEnable(GL_BLEND);
	OpenGLMatSingleTextureEnvMapSolid::RenderAllGeometry(pCurrentFrame);
	glDisable(GL_BLEND);
}
*/
void OpenGLMatSingleTextureEnvMapBlended::PrepareBasicMaterialSettings(TBC::GeometryBase* pGeometry)
{
	if (mSingleTexturePass == true)
	{
		OpenGLMatSingleColorSolid::PrepareBasicMaterialSettings(pGeometry);
	}
	else
	{
		// Prepare basic material settings using 100% specularity.
		TBC::GeometryBase::BasicMaterialSettings lMatSettings;
		pGeometry->GetBasicMaterialSettings(lMatSettings);

		glColor4f(lMatSettings.mRed,
			  lMatSettings.mGreen,
			  lMatSettings.mBlue,
			  lMatSettings.mSpecular * lMatSettings.mAlpha);

		float lDiffuse[] = { 0, 0, 0, 1.0f };
		float lSpecular[] =
		{
			lMatSettings.mRed,
			lMatSettings.mGreen,
			lMatSettings.mBlue,
			1.0f
		};
		static float lWhite[] = {1.0f, 1.0f, 1.0f, 1.0f};

		glMaterialfv(GL_FRONT, GL_AMBIENT, lWhite);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, lDiffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, lSpecular);
		glMateriali(GL_FRONT, GL_SHININESS, 128);

		((OpenGLRenderer*)GetRenderer())->SetGlobalMaterialReflectance(lMatSettings.mRed,
							  lMatSettings.mGreen,
							  lMatSettings.mBlue,
							  lMatSettings.mSpecular);
	}
}


/*
	OpenGLMatTextureAndLightmap
*/
bool OpenGLMatTextureAndLightmap::AddGeometry(TBC::GeometryBase* pGeometry)
{
	// We require at least 2 textures.
	if (/*lGeometry->mTA == 0 ||
	   lGeometry->mTA->mNumTextures < 2 ||*/
	   pGeometry->GetUVSetCount() < 2 ||
	   UiLepra::OpenGLExtensions::MultiTextureSupported() == false)
	{
		if (mFallBackMaterial != 0 && ((OpenGLRenderer*)GetRenderer())->GetFallbackMaterialEnabled())
		{
			mLog.AWarning("Material \"TextureAndLightmap\", passing geometry to fallback material.");
			return mFallBackMaterial->AddGeometry(pGeometry);
		}
		else
		{
			return false;
		}
	}

	return OpenGLMatSingleTextureSolid::AddGeometry(pGeometry);
}

void OpenGLMatTextureAndLightmap::RenderAllGeometry(unsigned int pCurrentFrame)
{
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

	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

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

	OpenGLMaterial::RenderAllGeometry(pCurrentFrame);

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

	OpenGLMaterial::RenderAllGeometry(pCurrentFrame);

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
	OpenGLRenderer::OGLGeometryData* lGeometry = (OpenGLRenderer::OGLGeometryData*)pGeometry->GetRendererData();
	PrepareBasicMaterialSettings(pGeometry);

	// Setup the color map in texture unit 0.
	UiLepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE0);
	UiLepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE0);
	BindTexture(lGeometry->mTA->mMaps[0].mMapID[Texture::COLOR_MAP]);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	if (UiLepra::OpenGLExtensions::BufferObjectsSupported() == true)
	{
		UiLepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, (GLuint)lGeometry->mVertexBufferID);
		glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)lGeometry->mUVOffset);
	}
	else
	{
		glTexCoordPointer(2, GL_FLOAT, 0, pGeometry->GetUVData(0));
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
		BindTexture(lGeometry->mTA->mMaps[1].mMapID[Texture::COLOR_MAP]);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		if (UiLepra::OpenGLExtensions::BufferObjectsSupported() == true)
		{
			UiLepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, (GLuint)lGeometry->mVertexBufferID);
			glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)(lGeometry->mUVOffset + pGeometry->GetMaxVertexCount() * sizeof(float) * 2));
		}
		else
		{
			glTexCoordPointer(2, GL_FLOAT, 0, pGeometry->GetUVData(1));
		}
	}

	if (UiLepra::OpenGLExtensions::BufferObjectsSupported() == true)
	{
		GLuint lIndexBufferID  = (GLuint)lGeometry->mIndexBufferID;
		
		// Vertex buffer already bound.
		//OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, (GLuint)lGeometry->mVertexBufferID);

		glVertexPointer(3, GL_FLOAT, 0, 0);
		glNormalPointer(GL_FLOAT, 0, (GLvoid*)lGeometry->mNormalOffset);

		UiLepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lIndexBufferID);
		glDrawElements(OpenGLMaterial::GetGLElementType(pGeometry), 
			       pGeometry->GetIndexCount(),
			       GL_UNSIGNED_INT, 
			       0);
	}
	else
	{
		glVertexPointer(3, GL_FLOAT, 0, pGeometry->GetVertexData());
		glNormalPointer(GL_FLOAT, 0, pGeometry->GetNormalData());
		glDrawElements(OpenGLMaterial::GetGLElementType(pGeometry),
			       pGeometry->GetIndexCount(),
			       GL_UNSIGNED_INT,
			       pGeometry->GetIndexData());
	}
	((OpenGLRenderer*)GetRenderer())->ResetAmbientLight();
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


OpenGLMatPXS::OpenGLMatPXS(char* pVP, char* pFP[NUM_FP]):
	mVPID(0)
{
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

	if (UiLepra::OpenGLExtensions::ShaderProgramsSupported() == true)
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
							    (GLsizei)::strlen(pVP),
							    pVP);
		glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB, &lErrorPos);
		if (lErrorPos != -1)
		{
			Lepra::AnsiString lGlError((const char*)glGetString(GL_PROGRAM_ERROR_STRING_ARB));
			mLog.Errorf(_T("Error in vertex shader at pos %i!\r\n%s\r\n"), lErrorPos, Lepra::AnsiStringUtility::ToCurrentCode(lGlError).c_str());
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
								    (GLsizei)::strlen(pFP[i]),
								    pFP[i]);
			glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB, &lErrorPos);
			if (lErrorPos != -1)
			{
				Lepra::AnsiString lGlError((const char*)glGetString(GL_PROGRAM_ERROR_STRING_ARB));
				mLog.Errorf(_T("Error in fragment shader %i at pos %i!\r\n%s\r\n"), i, lErrorPos, Lepra::AnsiStringUtility::ToCurrentCode(lGlError).c_str());
				mLog.AInfo("Setting fragment shader to fallback shader.");

				SetToFallbackFP(i);
			}
		}
		glDisable(GL_FRAGMENT_PROGRAM_ARB);
	}
}

void OpenGLMatPXS::SetToFallbackFP(int pFPType)
{
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
}

void OpenGLMatPXS::PrepareLights(OpenGLRenderer* pRenderer)
{
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

	smNumDirLights = 0;
	smNumPntLights = 0;
	smNumSptLights = 0;
	int lTotalLightCount = 0;
	int i;

	// Sorting the lights with respect to the camera.
	pRenderer->SortLights(pRenderer->GetCameraTransformation().GetPosition());

	// Count the number of lights of each type, and get their position/direction and color.
	for (i = 0; 
		i < pRenderer->GetLightCount() && 
		lTotalLightCount < MAX_SHADER_LIGHTS; 
		i++)
	{
		Renderer::LightID lLightID = pRenderer->GetClosestLight(i);
		GLenum lGLLight = GL_LIGHT0 + (int)lLightID;

		int lLightIndex = 0;

		// Check if the light is enabled, if it's not, we are in the 
		// "shadow rendering pass".
		if (glIsEnabled(lGLLight) == GL_TRUE)
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
			}

			// Get light color.
			pRenderer->GetLightColor(lLightID, 
						   lLightCol[lIndex + 0], 
						   lLightCol[lIndex + 1], 
						   lLightCol[lIndex + 2]);

			float lCutoffAngle = pRenderer->GetLightCutoffAngle(lLightID);
			lLightCut[lBucket * MAX_SHADER_LIGHTS + lLightIndex] = (float)cos(lCutoffAngle * Lepra::PIF / 180.0f);

			lLightExp[lBucket * MAX_SHADER_LIGHTS + lLightIndex] = pRenderer->GetLightSpotExponent(lLightID);

			lTotalLightCount++;
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
}

void OpenGLMatPXS::PrepareShaderPrograms(OpenGLRenderer* /*pRenderer*/)
{
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
}

void OpenGLMatPXS::CleanupShaderPrograms()
{
	glDisable(GL_VERTEX_PROGRAM_ARB);
	glDisable(GL_FRAGMENT_PROGRAM_ARB);
}

void OpenGLMatPXS::SetAmbientLight(OpenGLRenderer* pRenderer, TBC::GeometryBase* pGeometry)
{
	float r, g, b;
	pRenderer->GetAmbientLight(r, g, b);

	TBC::GeometryBase::BasicMaterialSettings lMat;
	pGeometry->GetBasicMaterialSettings(lMat);

	r += lMat.mSelfIllumination;
	g += lMat.mSelfIllumination;
	b += lMat.mSelfIllumination;

	if (r > 1.0f) 
	{
		r = 1.0f;
	}
	if (g > 1.0f) 
	{
		g = 1.0f;
	}
	if (b > 1.0f) 
	{
		b = 1.0f;
	}

	// Set the ambient light.
	GLfloat lLightAmbient[4];
	lLightAmbient[0] = (GLfloat)r;
	lLightAmbient[1] = (GLfloat)g;
	lLightAmbient[2] = (GLfloat)b;
	lLightAmbient[3] = 0;
	UiLepra::OpenGLExtensions::glProgramLocalParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB, 3, lLightAmbient);
}

//
// OpenGLMatSingleColorSolidPXS
//

OpenGLMatSingleColorSolidPXS::OpenGLMatSingleColorSolidPXS(OpenGLRenderer* pRenderer,
	Material* pFallBackMaterial):
	OpenGLMatSingleColorSolid(pRenderer, pFallBackMaterial),
	OpenGLMatPXS(smVP, smFP)
{
}

OpenGLMatSingleColorSolidPXS::~OpenGLMatSingleColorSolidPXS()
{
	if (UiLepra::OpenGLExtensions::ShaderProgramsSupported() == true)
	{
		UiLepra::OpenGLExtensions::glDeleteProgramsARB(1, (const GLuint*)&mVPID);
		UiLepra::OpenGLExtensions::glDeleteProgramsARB(NUM_FP, (const GLuint*)mFPID);
	}
}

bool OpenGLMatSingleColorSolidPXS::AddGeometry(TBC::GeometryBase* pGeometry)
{
	if (UiLepra::OpenGLExtensions::ShaderProgramsSupported() == false)
	{
		if (mFallBackMaterial != 0 && ((OpenGLRenderer*)GetRenderer())->GetFallbackMaterialEnabled())
		{
			mLog.AWarning("Passing geometry to fallback material.");
			return mFallBackMaterial->AddGeometry(pGeometry);
		}
		else
		{
			return false;
		}
	}

	return OpenGLMatSingleColorSolid::AddGeometry(pGeometry);
}

void OpenGLMatSingleColorSolidPXS::RenderAllGeometry(unsigned int pCurrentFrame)
{
	if (Material::IsEmpty() == true)
		return;

	OpenGLMatPXS::PrepareShaderPrograms((OpenGLRenderer*)GetRenderer());
	OpenGLMatSingleColorSolid::RenderAllGeometry(pCurrentFrame);
	OpenGLMatPXS::CleanupShaderPrograms();
}

void OpenGLMatSingleColorSolidPXS::RenderGeometry(TBC::GeometryBase* pGeometry)
{
	OpenGLRenderer::OGLGeometryData* lGeometry = (OpenGLRenderer::OGLGeometryData*)pGeometry->GetRendererData();
	PrepareBasicMaterialSettings(pGeometry);
	OpenGLMatPXS::SetAmbientLight((OpenGLRenderer*)GetRenderer(), pGeometry);

	float lSpecular[4];
	TBC::GeometryBase::BasicMaterialSettings lMatSettings;
	pGeometry->GetBasicMaterialSettings(lMatSettings);
	lSpecular[0] = lMatSettings.mSpecular;
	lSpecular[1] = lMatSettings.mSpecular;
	lSpecular[2] = lMatSettings.mSpecular;
	lSpecular[3] = lMatSettings.mSpecular;
	UiLepra::OpenGLExtensions::glProgramLocalParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB, 0, lSpecular);

	if (UiLepra::OpenGLExtensions::BufferObjectsSupported() == true)
	{
		GLuint lIndexBufferID  = (GLuint)lGeometry->mIndexBufferID;
		
		UiLepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, (GLuint)lGeometry->mVertexBufferID);

		glVertexPointer(3, GL_FLOAT, 0, 0);
		glNormalPointer(GL_FLOAT, 0, (GLvoid*)lGeometry->mNormalOffset);

		UiLepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lIndexBufferID);
		glDrawElements(OpenGLMaterial::GetGLElementType(pGeometry), 
			       pGeometry->GetIndexCount(),
			       GL_UNSIGNED_INT, 
			       0);
	}
	else
	{
		glVertexPointer(3, GL_FLOAT, 0, pGeometry->GetVertexData());
		glNormalPointer(GL_FLOAT, 0, pGeometry->GetNormalData());
		glDrawElements(OpenGLMaterial::GetGLElementType(pGeometry),
			       pGeometry->GetIndexCount(),
			       GL_UNSIGNED_INT,
			       pGeometry->GetIndexData());
	}
	((OpenGLRenderer*)GetRenderer())->ResetAmbientLight();
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
	if (UiLepra::OpenGLExtensions::ShaderProgramsSupported() == true)
	{
		UiLepra::OpenGLExtensions::glDeleteProgramsARB(1, (const GLuint*)&mVPID);
		UiLepra::OpenGLExtensions::glDeleteProgramsARB(NUM_FP, (const GLuint*)mFPID);
	}
}

bool OpenGLMatSingleTextureSolidPXS::AddGeometry(TBC::GeometryBase* pGeometry)
{
	if (UiLepra::OpenGLExtensions::ShaderProgramsSupported() == false ||
	   /*lGeometry->mTA == 0 ||
	   lGeometry->mTA->mNumTextures == 0 ||*/
	   pGeometry->GetUVSetCount() == 0)
	{
		if (mFallBackMaterial != 0 && ((OpenGLRenderer*)GetRenderer())->GetFallbackMaterialEnabled())
		{
			mLog.AWarning("Material \"SingleTextureSolidPXS\", passing geometry to fallback material.");
			return mFallBackMaterial->AddGeometry(pGeometry);
		}
		else
		{
			return false;
		}
	}

	return OpenGLMatSingleTextureSolid::AddGeometry(pGeometry);
}

void OpenGLMatSingleTextureSolidPXS::RenderAllGeometry(unsigned int pCurrentFrame)
{
	if (Material::IsEmpty() == true)
		return;

	OpenGLMatPXS::PrepareShaderPrograms((OpenGLRenderer*)GetRenderer());
	OpenGLMatSingleTextureSolid::RenderAllGeometry(pCurrentFrame);
	OpenGLMatPXS::CleanupShaderPrograms();
}

void OpenGLMatSingleTextureSolidPXS::RenderGeometry(TBC::GeometryBase* pGeometry)
{
	OpenGLRenderer::OGLGeometryData* lGeometry = (OpenGLRenderer::OGLGeometryData*)pGeometry->GetRendererData();
	PrepareBasicMaterialSettings(pGeometry);
	OpenGLMatPXS::SetAmbientLight((OpenGLRenderer*)GetRenderer(), pGeometry);

	UiLepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE0);
	UiLepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);

	BindTexture(lGeometry->mTA->mMaps[0].mMapID[Texture::COLOR_MAP]);

	OpenGLMaterial::UpdateTextureMatrix(lGeometry->mGeometry);
	
	float lSpecular[4];
	TBC::GeometryBase::BasicMaterialSettings lMatSettings;
	pGeometry->GetBasicMaterialSettings(lMatSettings);
	lSpecular[0] = lMatSettings.mSpecular;
	lSpecular[1] = lMatSettings.mSpecular;
	lSpecular[2] = lMatSettings.mSpecular;
	lSpecular[3] = lMatSettings.mSpecular;
	UiLepra::OpenGLExtensions::glProgramLocalParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB, 0, lSpecular);

	if (UiLepra::OpenGLExtensions::BufferObjectsSupported() == true)
	{
		GLuint lIndexBufferID  = (GLuint)lGeometry->mIndexBufferID;
		
		UiLepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, (GLuint)lGeometry->mVertexBufferID);

		glVertexPointer(3, GL_FLOAT, 0, 0);
		glNormalPointer(GL_FLOAT, 0, (GLvoid*)lGeometry->mNormalOffset);
		glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)lGeometry->mUVOffset);

		UiLepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lIndexBufferID);
		glDrawElements(OpenGLMaterial::GetGLElementType(pGeometry), 
			       pGeometry->GetIndexCount(),
			       GL_UNSIGNED_INT, 
			       0);
	}
	else
	{
		glVertexPointer(3, GL_FLOAT, 0, pGeometry->GetVertexData());
		glNormalPointer(GL_FLOAT, 0, pGeometry->GetNormalData());
		glTexCoordPointer(2, GL_FLOAT, 0, pGeometry->GetUVData(0));
		glDrawElements(OpenGLMaterial::GetGLElementType(pGeometry),
			       pGeometry->GetIndexCount(),
			       GL_UNSIGNED_INT,
			       pGeometry->GetIndexData());
	}
	((OpenGLRenderer*)GetRenderer())->ResetAmbientLight();
}



//
// OpenGLMatTextureAndLightmapPXS
//

OpenGLMatTextureAndLightmapPXS::OpenGLMatTextureAndLightmapPXS(OpenGLRenderer* pRenderer,
							       Material* pFallBackMaterial) :
	OpenGLMatSingleTextureSolid(pRenderer, pFallBackMaterial),
	OpenGLMatPXS(smVP, smFP)
{
}

OpenGLMatTextureAndLightmapPXS::~OpenGLMatTextureAndLightmapPXS()
{
	if (UiLepra::OpenGLExtensions::ShaderProgramsSupported() == true)
	{
		UiLepra::OpenGLExtensions::glDeleteProgramsARB(1, (const GLuint*)&mVPID);
		UiLepra::OpenGLExtensions::glDeleteProgramsARB(NUM_FP, (const GLuint*)mFPID);
	}
}

bool OpenGLMatTextureAndLightmapPXS::AddGeometry(TBC::GeometryBase* pGeometry)
{
	if (UiLepra::OpenGLExtensions::ShaderProgramsSupported() == false ||
	   /*lGeometry->mTA == 0 ||
	   lGeometry->mTA->mNumTextures < 2 ||*/
	   pGeometry->GetUVSetCount() < 2)
	{
		if (mFallBackMaterial != 0 && ((OpenGLRenderer*)GetRenderer())->GetFallbackMaterialEnabled())
		{
			mLog.AWarning("Material \"TextureAndLightmapPXS\", passing geometry to fallback material.");
			return mFallBackMaterial->AddGeometry(pGeometry);
		}
		else
		{
			return false;
		}
	}

	return OpenGLMatSingleTextureSolid::AddGeometry(pGeometry);
}

void OpenGLMatTextureAndLightmapPXS::RenderAllGeometry(unsigned int pCurrentFrame)
{
	if (Material::IsEmpty() == true)
		return;

	UiLepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE0);
	UiLepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);

	UiLepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE1);
	UiLepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE1);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);

	OpenGLMatPXS::PrepareShaderPrograms((OpenGLRenderer*)GetRenderer());
	OpenGLMatSingleTextureSolid::RenderAllGeometry(pCurrentFrame);
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
	OpenGLRenderer::OGLGeometryData* lGeometry = (OpenGLRenderer::OGLGeometryData*)pGeometry->GetRendererData();
	PrepareBasicMaterialSettings(pGeometry);
	OpenGLMatPXS::SetAmbientLight((OpenGLRenderer*)GetRenderer(), pGeometry);

	UiLepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE0);
	UiLepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE0);
	BindTexture(lGeometry->mTA->mMaps[0].mMapID[Texture::COLOR_MAP]);

	if (UiLepra::OpenGLExtensions::BufferObjectsSupported() == true)
	{
		GLuint lIndexBufferID  = (GLuint)lGeometry->mIndexBufferID;
		UiLepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, (GLuint)lGeometry->mVertexBufferID);
		glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)lGeometry->mUVOffset);

		UiLepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lIndexBufferID);
	}
	else
	{
		glTexCoordPointer(2, GL_FLOAT, 0, pGeometry->GetUVData(0));
	}
	OpenGLMaterial::UpdateTextureMatrix(lGeometry->mGeometry);

	UiLepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE1);
	UiLepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE1);
	BindTexture(lGeometry->mTA->mMaps[1].mMapID[Texture::COLOR_MAP]);

	if (UiLepra::OpenGLExtensions::BufferObjectsSupported() == true)
	{
		GLuint lIndexBufferID  = (GLuint)lGeometry->mIndexBufferID;
		UiLepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, (GLuint)lGeometry->mVertexBufferID);
		glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)(lGeometry->mUVOffset + pGeometry->GetMaxVertexCount() * sizeof(float) * 2));

		UiLepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lIndexBufferID);
	}
	else
	{
		glTexCoordPointer(2, GL_FLOAT, 0, pGeometry->GetUVData(1));
	}

	float lSpecular[4];
	TBC::GeometryBase::BasicMaterialSettings lMatSettings;
	pGeometry->GetBasicMaterialSettings(lMatSettings);
	lSpecular[0] = lMatSettings.mSpecular;
	lSpecular[1] = lMatSettings.mSpecular;
	lSpecular[2] = lMatSettings.mSpecular;
	lSpecular[3] = lMatSettings.mSpecular;
	UiLepra::OpenGLExtensions::glProgramLocalParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB, 0, lSpecular);

	if (UiLepra::OpenGLExtensions::BufferObjectsSupported() == true)
	{
		glVertexPointer(3, GL_FLOAT, 0, 0);
		glNormalPointer(GL_FLOAT, 0, (GLvoid*)lGeometry->mNormalOffset);

		glDrawElements(OpenGLMaterial::GetGLElementType(pGeometry), 
			       pGeometry->GetIndexCount(),
			       GL_UNSIGNED_INT, 
			       0);
	}
	else
	{
		glVertexPointer(3, GL_FLOAT, 0, pGeometry->GetVertexData());
		glNormalPointer(GL_FLOAT, 0, pGeometry->GetNormalData());
		glDrawElements(OpenGLMaterial::GetGLElementType(pGeometry),
			       pGeometry->GetIndexCount(),
			       GL_UNSIGNED_INT,
			       pGeometry->GetIndexData());
	}
	((OpenGLRenderer*)GetRenderer())->ResetAmbientLight();
}




//
// OpenGLMatTextureSBMapPXS
//

OpenGLMatTextureSBMapPXS::OpenGLMatTextureSBMapPXS(OpenGLRenderer* pRenderer,
						   Material* pFallBackMaterial,
						   char* pVP,
						   char** pFP) :
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
	if (UiLepra::OpenGLExtensions::ShaderProgramsSupported() == true)
	{
		UiLepra::OpenGLExtensions::glDeleteProgramsARB(1, (const GLuint*)&mVPID);
		UiLepra::OpenGLExtensions::glDeleteProgramsARB(NUM_FP, (const GLuint*)mFPID);
	}
}

bool OpenGLMatTextureSBMapPXS::AddGeometry(TBC::GeometryBase* pGeometry)
{
	if (UiLepra::OpenGLExtensions::ShaderProgramsSupported() == false ||
	   /*lGeometry->mTA == 0 ||
	   lGeometry->mTA->mNumTextures == 0 ||
	   lGeometry->mTA->mMaps[0].mMapID[Texture::SPECULAR_MAP] == Renderer::INVALID_TEXTURE ||
	   lGeometry->mTA->mMaps[0].mMapID[Texture::NORMAL_MAP]   == Renderer::INVALID_TEXTURE ||*/
	   pGeometry->GetUVSetCount() == 0)
	{
		if (mFallBackMaterial != 0 && ((OpenGLRenderer*)GetRenderer())->GetFallbackMaterialEnabled())
		{
			mLog.AWarning("Material \"TextureSBMapPXS\", passing geometry to fallback material.");
			return mFallBackMaterial->AddGeometry(pGeometry);
		}
		else
		{
			return false;
		}
	}

	return OpenGLMatSingleTextureSolid::AddGeometry(pGeometry);
}

void OpenGLMatTextureSBMapPXS::RenderAllGeometry(unsigned int pCurrentFrame)
{
	if (Material::IsEmpty() == true)
		return;

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
	OpenGLMatSingleTextureSolid::RenderAllGeometry(pCurrentFrame);
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
	OpenGLRenderer::OGLGeometryData* lGeometry = (OpenGLRenderer::OGLGeometryData*)pGeometry->GetRendererData();
	PrepareBasicMaterialSettings(pGeometry);
	OpenGLMatPXS::SetAmbientLight((OpenGLRenderer*)GetRenderer(), pGeometry);

	// Texture unit 0, handles color map and regular 
	// texture (UV) coordinates.
	UiLepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE0);
	UiLepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE0);
	BindTexture(lGeometry->mTA->mMaps[0].mMapID[Texture::COLOR_MAP]);

	if (UiLepra::OpenGLExtensions::BufferObjectsSupported() == true)
	{
		GLuint lIndexBufferID  = (GLuint)lGeometry->mIndexBufferID;
		UiLepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, (GLuint)lGeometry->mVertexBufferID);
		glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)lGeometry->mUVOffset);

		UiLepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lIndexBufferID);
	}
	else
	{
		glTexCoordPointer(2, GL_FLOAT, 0, pGeometry->GetUVData(0));
	}

	OpenGLMaterial::UpdateTextureMatrix(lGeometry->mGeometry);

	// Texture unit 1, handles specular map and tangents.
	UiLepra::OpenGLExtensions::glActiveTexture(GL_TEXTURE1);
	UiLepra::OpenGLExtensions::glClientActiveTexture(GL_TEXTURE1);
	BindTexture(lGeometry->mTA->mMaps[0].mMapID[Texture::SPECULAR_MAP]);

	if (UiLepra::OpenGLExtensions::BufferObjectsSupported() == true)
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
	BindTexture(lGeometry->mTA->mMaps[0].mMapID[Texture::NORMAL_MAP]);
	if (UiLepra::OpenGLExtensions::BufferObjectsSupported() == true)
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

	float lSpecular[4];
	TBC::GeometryBase::BasicMaterialSettings lMatSettings;
	pGeometry->GetBasicMaterialSettings(lMatSettings);
	lSpecular[0] = lMatSettings.mSpecular;
	lSpecular[1] = lMatSettings.mSpecular;
	lSpecular[2] = lMatSettings.mSpecular;
	lSpecular[3] = lMatSettings.mSpecular;
	UiLepra::OpenGLExtensions::glProgramLocalParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB, 0, lSpecular);

	if (UiLepra::OpenGLExtensions::BufferObjectsSupported() == true)
	{
		glVertexPointer(3, GL_FLOAT, 0, 0);
		glNormalPointer(GL_FLOAT, 0, (GLvoid*)lGeometry->mNormalOffset);

		glDrawElements(OpenGLMaterial::GetGLElementType(pGeometry), 
			       pGeometry->GetIndexCount(),
			       GL_UNSIGNED_INT, 
			       0);
	}
	else
	{
		glVertexPointer(3, GL_FLOAT, 0, pGeometry->GetVertexData());
		glNormalPointer(GL_FLOAT, 0, pGeometry->GetNormalData());
		glDrawElements(OpenGLMaterial::GetGLElementType(pGeometry),
			       pGeometry->GetIndexCount(),
			       GL_UNSIGNED_INT,
			       pGeometry->GetIndexData());
	}
	((OpenGLRenderer*)GetRenderer())->ResetAmbientLight();
}


OpenGLMatTextureDiffuseBumpMapPXS::OpenGLMatTextureDiffuseBumpMapPXS(OpenGLRenderer* pRenderer,
								     Material* pFallBackMaterial) :
	OpenGLMatTextureSBMapPXS(pRenderer, pFallBackMaterial, smVP, smFP)
{
}

OpenGLMatTextureDiffuseBumpMapPXS::~OpenGLMatTextureDiffuseBumpMapPXS()
{
	// This is already done in the superclass.
/*	if (UiLepra::OpenGLExtensions::ShaderProgramsSupported() == true)
	{
		UiLepra::OpenGLExtensions::glDeleteProgramsARB(1, (const GLuint*)&mVPID);
		UiLepra::OpenGLExtensions::glDeleteProgramsARB(NUM_FP, (const GLuint*)mFPID);
	}
*/
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



} // End namespace.
