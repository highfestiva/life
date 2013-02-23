
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#pragma once



#include "../../Lepra/Include/Lepra.h"
#include "UiMaterial.h"
#include "UiOpenGLRenderer.h"
#include <list>



namespace UiTbc
{



class OpenGLRenderer;



class OpenGLMaterial: public Material
{
	typedef Material Parent;
public:
	OpenGLMaterial(OpenGLRenderer* pRenderer, Material::DepthSortHint pSortHint, Material* pFallBackMaterial);
	virtual ~OpenGLMaterial();

	virtual Material::RemoveStatus RemoveGeometry(TBC::GeometryBase* pGeometry);

	GLenum GetGLElementType(TBC::GeometryBase* pGeometry);

	void SetBasicMaterial(const TBC::GeometryBase::BasicMaterialSettings& pMaterial);
	static void SetBasicMaterial(const TBC::GeometryBase::BasicMaterialSettings& pMaterial, Renderer* pRenderer);
	void ResetBasicMaterial(const TBC::GeometryBase::BasicMaterialSettings& pMaterial);

	virtual void RenderAllBlendedGeometry(unsigned pCurrentFrame, const GeometryGroupList& pGeometryGroupList);

protected:
	void UpdateTextureMatrix(TBC::GeometryBase* pGeometry);

	Material* mFallBackMaterial;	// If geometry doesn't contain all data needed.
};



class OpenGLMatSingleColorSolid: public OpenGLMaterial
{
	typedef OpenGLMaterial Parent;
public:
	inline OpenGLMatSingleColorSolid(OpenGLRenderer* pRenderer, Material* pFallBackMaterial):
		OpenGLMaterial(pRenderer, Material::DEPTHSORT_F2B, pFallBackMaterial)
	{
	}
	inline OpenGLMatSingleColorSolid(OpenGLRenderer* pRenderer,
					 Material::DepthSortHint pSortHint,
					 Material* pFallBackMaterial) :
		OpenGLMaterial(pRenderer, pSortHint, pFallBackMaterial)
	{
	}

	inline virtual ~OpenGLMatSingleColorSolid(){}

	virtual void PreRender();
	virtual void PostRender();

protected:
	virtual void RenderGeometry(TBC::GeometryBase* pGeometry);
	virtual void RenderBaseGeometry(TBC::GeometryBase* pGeometry);
};



class OpenGLMatSingleColorBlended: public OpenGLMatSingleColorSolid
{
	typedef OpenGLMatSingleColorSolid Parent;
public:
	inline OpenGLMatSingleColorBlended(OpenGLRenderer* pRenderer, Material* pFallBackMaterial, bool pOutline):
		OpenGLMatSingleColorSolid(pRenderer, Material::DEPTHSORT_B2F, pFallBackMaterial),
		mOutline(pOutline)
	{
	}

	inline virtual ~OpenGLMatSingleColorBlended(){}

	virtual void RenderAllGeometry(unsigned pCurrentFrame, const GeometryGroupList& pGeometryGroupList);
	virtual void PreRender();
	virtual void PostRender();

protected:
	bool mOutline;
};



class OpenGLMatVertexColorSolid: public OpenGLMatSingleColorSolid
{
	typedef OpenGLMatSingleColorSolid Parent;
public:
	inline OpenGLMatVertexColorSolid(OpenGLRenderer* pRenderer,
					 Material* pFallBackMaterial) :
		OpenGLMatSingleColorSolid(pRenderer, Material::DEPTHSORT_F2B, pFallBackMaterial)
	{
	}
	inline OpenGLMatVertexColorSolid(OpenGLRenderer* pRenderer,
					 Material::DepthSortHint pSortHint,
					 Material* pFallBackMaterial) :
		OpenGLMatSingleColorSolid(pRenderer, pSortHint, pFallBackMaterial)
	{
	}

	inline virtual ~OpenGLMatVertexColorSolid(){}

	virtual bool AddGeometry(TBC::GeometryBase* pGeometry);

protected:
	virtual void DoRenderAllGeometry(unsigned pCurrentFrame, const GeometryGroupList& pGeometryGroupList);
	virtual void RenderGeometry(TBC::GeometryBase* pGeometry);

private:
	LOG_CLASS_DECLARE();
};



class OpenGLMatVertexColorBlended: public OpenGLMatVertexColorSolid
{
	typedef OpenGLMatVertexColorSolid Parent;
public:
	inline OpenGLMatVertexColorBlended(OpenGLRenderer* pRenderer,
					   Material* pFallBackMaterial) :
		OpenGLMatVertexColorSolid(pRenderer, Material::DEPTHSORT_B2F, pFallBackMaterial)
	{
	}

	inline virtual ~OpenGLMatVertexColorBlended(){}

	virtual void RenderAllGeometry(unsigned pCurrentFrame, const GeometryGroupList& pGeometryGroupList);

protected:
	virtual void DoRenderAllGeometry(unsigned pCurrentFrame, const GeometryGroupList& pGeometryGroupList);
};



class OpenGLMatSingleTextureSolid: public OpenGLMatSingleColorSolid
{
	typedef OpenGLMatSingleColorSolid Parent;
public:
	inline OpenGLMatSingleTextureSolid(OpenGLRenderer* pRenderer,
					   Material* pFallBackMaterial) :
		OpenGLMatSingleColorSolid(pRenderer, Material::DEPTHSORT_F2B, pFallBackMaterial)
	{
	}
	inline OpenGLMatSingleTextureSolid(OpenGLRenderer* pRenderer,
					   Material::DepthSortHint pSortHint,
					   Material* pFallBackMaterial) :
		OpenGLMatSingleColorSolid(pRenderer, pSortHint, pFallBackMaterial)
	{
	}

	inline virtual ~OpenGLMatSingleTextureSolid(){}

	virtual bool AddGeometry(TBC::GeometryBase* pGeometry);
	virtual void PreRender();
	virtual void PostRender();

protected:
	
	virtual void RenderGeometry(TBC::GeometryBase* pGeometry);
	void BindTexture(int pTextureID, int pMipMapLevelCount);

	GLint mTextureParamMin;
	GLint mTextureParamMag;

private:
	LOG_CLASS_DECLARE();
};



class OpenGLMatSingleTextureHighlight: public OpenGLMatSingleTextureSolid
{
	typedef OpenGLMatSingleTextureSolid Parent;
public:
	inline OpenGLMatSingleTextureHighlight(OpenGLRenderer* pRenderer,
					   Material* pFallBackMaterial) :
		Parent(pRenderer, Material::DEPTHSORT_F2B, pFallBackMaterial)
	{
	}
	virtual void PreRender();
	virtual void PostRender();

private:
	bool mIsLighted;
};



class OpenGLMatSingleTextureBlended: public OpenGLMatSingleTextureSolid
{
	typedef OpenGLMatSingleTextureSolid Parent;
public:
	inline OpenGLMatSingleTextureBlended(OpenGLRenderer* pRenderer,
					     Material* pFallBackMaterial) :
		OpenGLMatSingleTextureSolid(pRenderer, Material::DEPTHSORT_B2F, pFallBackMaterial)
	{
	}
	inline OpenGLMatSingleTextureBlended(OpenGLRenderer* pRenderer,
					     Material::DepthSortHint pSortHint,
					     Material* pFallBackMaterial) :
		OpenGLMatSingleTextureSolid(pRenderer, pSortHint, pFallBackMaterial)
	{
	}

	inline virtual ~OpenGLMatSingleTextureBlended(){}

	virtual void RenderAllGeometry(unsigned pCurrentFrame, const GeometryGroupList& pGeometryGroupList);

protected:
	virtual void DoRenderAllGeometry(unsigned pCurrentFrame, const GeometryGroupList& pGeometryGroupList);
	virtual void PreRender();
	virtual void PostRender();
};



class OpenGLMatSingleTextureAlphaTested: public OpenGLMatSingleTextureSolid
{
	typedef OpenGLMatSingleTextureSolid Parent;
public:
	inline OpenGLMatSingleTextureAlphaTested(OpenGLRenderer* pRenderer,
						 Material* pFallBackMaterial) :
		OpenGLMatSingleTextureSolid(pRenderer, Material::DEPTHSORT_F2B, pFallBackMaterial)
	{
	}

	inline virtual ~OpenGLMatSingleTextureAlphaTested(){}

protected:
	virtual void DoRenderAllGeometry(unsigned pCurrentFrame, const GeometryGroupList& pGeometryGroupList);
	virtual void RenderGeometry(TBC::GeometryBase* pGeometry);
};



class OpenGLMatSingleColorEnvMapSolid: public OpenGLMatSingleColorSolid
{
	typedef OpenGLMatSingleColorSolid Parent;
public:
	inline OpenGLMatSingleColorEnvMapSolid(OpenGLRenderer* pRenderer,
					       Material* pFallBackMaterial) :
		OpenGLMatSingleColorSolid(pRenderer, Material::DEPTHSORT_F2B, pFallBackMaterial)
	{
	}
	inline OpenGLMatSingleColorEnvMapSolid(OpenGLRenderer* pRenderer,
					       Material::DepthSortHint pSortHint,
					       Material* pFallBackMaterial) :
		OpenGLMatSingleColorSolid(pRenderer, pSortHint, pFallBackMaterial)
	{
	}

	inline virtual ~OpenGLMatSingleColorEnvMapSolid(){}

	virtual bool AddGeometry(TBC::GeometryBase* pGeometry);

protected:
	virtual void DoRenderAllGeometry(unsigned pCurrentFrame, const GeometryGroupList& pGeometryGroupList);

	bool mSingleColorPass;
	GLint mTextureParamMin;
	GLint mTextureParamMag;

private:
	LOG_CLASS_DECLARE();
};



class OpenGLMatSingleColorEnvMapBlended: public OpenGLMatSingleColorEnvMapSolid
{
	typedef OpenGLMatSingleColorEnvMapSolid Parent;
public:
	inline OpenGLMatSingleColorEnvMapBlended(OpenGLRenderer* pRenderer,
						 Material* pFallBackMaterial) :
		OpenGLMatSingleColorEnvMapSolid(pRenderer, Material::DEPTHSORT_B2F, pFallBackMaterial)
	{
	}

	inline virtual ~OpenGLMatSingleColorEnvMapBlended(){}

	virtual void RenderAllGeometry(unsigned pCurrentFrame, const GeometryGroupList& pGeometryGroupList);
};



class OpenGLMatSingleTextureEnvMapSolid: public OpenGLMatSingleTextureSolid
{
	typedef OpenGLMatSingleTextureSolid Parent;
public:
	inline OpenGLMatSingleTextureEnvMapSolid(OpenGLRenderer* pRenderer,
						 Material* pFallBackMaterial) :
		OpenGLMatSingleTextureSolid(pRenderer, Material::DEPTHSORT_F2B, pFallBackMaterial)
	{
	}
	inline OpenGLMatSingleTextureEnvMapSolid(OpenGLRenderer* pRenderer,
						 Material::DepthSortHint pSortHint,
						 Material* pFallBackMaterial) :
		OpenGLMatSingleTextureSolid(pRenderer, pSortHint, pFallBackMaterial)
	{
	}

	inline virtual ~OpenGLMatSingleTextureEnvMapSolid(){}

protected:
	virtual void DoRenderAllGeometry(unsigned pCurrentFrame, const GeometryGroupList& pGeometryGroupList);
	virtual void RenderGeometry(TBC::GeometryBase* pGeometry);

	bool mSingleTexturePass;

private:
	LOG_CLASS_DECLARE();
};



class OpenGLMatSingleTextureEnvMapBlended: public OpenGLMatSingleTextureEnvMapSolid
{
	typedef OpenGLMatSingleTextureEnvMapSolid Parent;
public:
	inline OpenGLMatSingleTextureEnvMapBlended(OpenGLRenderer* pRenderer,
						   Material* pFallBackMaterial) :
		OpenGLMatSingleTextureEnvMapSolid(pRenderer, Material::DEPTHSORT_B2F, pFallBackMaterial)
	{
	}

	inline virtual ~OpenGLMatSingleTextureEnvMapBlended(){}

	virtual void RenderAllGeometry(unsigned pCurrentFrame, const GeometryGroupList& pGeometryGroupList);
};



class OpenGLMatTextureAndLightmap: public OpenGLMatSingleTextureSolid
{
	typedef OpenGLMatSingleTextureSolid Parent;
public:
	inline OpenGLMatTextureAndLightmap(OpenGLRenderer* pRenderer,
					   Material* pFallBackMaterial) :
		OpenGLMatSingleTextureSolid(pRenderer, Material::DEPTHSORT_F2B, pFallBackMaterial)
	{
	}

	inline virtual ~OpenGLMatTextureAndLightmap(){}

protected:
	virtual void DoRenderAllGeometry(unsigned pCurrentFrame, const GeometryGroupList& pGeometryGroupList);
	virtual void RenderGeometry(TBC::GeometryBase* pGeometry);

	bool mFirstPass;

private:
	LOG_CLASS_DECLARE();
};



//
// Base class for materials using pixel shaders. As long as all shaders
// have the same interface (parameters at specific indices) this class
// provides some useful base functionality.
//

class OpenGLMatPXS
{
public:
	static void PrepareLights(OpenGLRenderer* pRenderer);

protected:

	enum
	{
		FP_NONE   = 0,

		FP_1POINT,
		FP_2POINT,
		FP_3POINT,

		FP_1DIR,
		FP_2DIR,
		FP_3DIR,

		FP_1SPOT,
		FP_2SPOT,
		FP_3SPOT,

		FP_1DIR1POINT,
		FP_1DIR2POINT,
		FP_2DIR1POINT,

		FP_1DIR1SPOT,
		FP_1DIR2SPOT,
		FP_2DIR1SPOT,

		FP_1POINT1SPOT,
		FP_1POINT2SPOT,
		FP_2POINT1SPOT,

		FP_1DIR1POINT1SPOT,

		NUM_FP
	};

	enum
	{
		// This is the maximum number of lights supported by the shaders.
		// This is a hard coded value which can't be changed, because 3
		// lights is the maximum number which can fit in one fragment
		// program without exceeding the maximum number of instructions.
		MAX_SHADER_LIGHTS = 3
	};

	OpenGLMatPXS(const astr& pVP, const astr pFP[NUM_FP]);

	void PrepareShaderPrograms(OpenGLRenderer* pRenderer);
	void CleanupShaderPrograms();
	void SetAmbientLight(OpenGLRenderer* pRenderer, TBC::GeometryBase* pGeometry);

	static inline int AllocProgramID()
	{
		return ++smProgramCount;
	}

	void SetToFallbackFP(int pFPType);

	int mVPID;
	int mFPID[NUM_FP];

	// Vertex- and fragment-program counter. Used to allocate unique IDs for 
	// each program.
	static int smProgramCount;

	// Fragment program lookup table.
	static bool smFPLUTInitialized;
	static int smFPLUT[4][4][4];

	// Light information.
	static float smLightPos[MAX_SHADER_LIGHTS * 4];
	static float smLightDir[MAX_SHADER_LIGHTS * 4];
	static float smLightCol[MAX_SHADER_LIGHTS * 4];
	static float smLightCut[MAX_SHADER_LIGHTS];     // Cutoff = cos(CutOffAngle)
	static float smLightExp[MAX_SHADER_LIGHTS];     // Spot exponent.
	static Renderer::LightType smLightType[MAX_SHADER_LIGHTS];

	static int smNumDirLights;
	static int smNumPntLights;
	static int smNumSptLights;
	static int smLightCount;

private:
	LOG_CLASS_DECLARE();
};



class OpenGLMatSingleColorSolidPXS: public OpenGLMatSingleColorSolid, public OpenGLMatPXS
{
	typedef OpenGLMatSingleColorSolid Parent;
public:
	OpenGLMatSingleColorSolidPXS(OpenGLRenderer* pRenderer,
				     Material* pFallBackMaterial);

	virtual ~OpenGLMatSingleColorSolidPXS();

protected:
	virtual void DoRenderAllGeometry(unsigned pCurrentFrame, const GeometryGroupList& pGeometryGroupList);
	virtual void RenderGeometry(TBC::GeometryBase* pGeometry);

	static const astr smVP;
	static const astr smFP[NUM_FP];

private:
	LOG_CLASS_DECLARE();
};



class OpenGLMatSingleTextureSolidPXS: public OpenGLMatSingleTextureSolid, public OpenGLMatPXS
{
	typedef OpenGLMatSingleTextureSolid Parent;
public:
	OpenGLMatSingleTextureSolidPXS(OpenGLRenderer* pRenderer,
				       Material* pFallBackMaterial);

	virtual ~OpenGLMatSingleTextureSolidPXS();

protected:
	virtual void DoRenderAllGeometry(unsigned pCurrentFrame, const GeometryGroupList& pGeometryGroupList);
	virtual void RenderGeometry(TBC::GeometryBase* pGeometry);

	static const astr smVP;
	static const astr smFP[NUM_FP];

private:
	LOG_CLASS_DECLARE();
};



class OpenGLMatTextureAndLightmapPXS: public OpenGLMatSingleTextureSolid, public OpenGLMatPXS
{
	typedef OpenGLMatSingleTextureSolid Parent;
public:
	OpenGLMatTextureAndLightmapPXS(OpenGLRenderer* pRenderer,
				       Material* pFallBackMaterial);

	virtual ~OpenGLMatTextureAndLightmapPXS();

protected:
	virtual void DoRenderAllGeometry(unsigned pCurrentFrame, const GeometryGroupList& pGeometryGroupList);
	virtual void RenderGeometry(TBC::GeometryBase* pGeometry);

	static const astr smVP;
	static const astr smFP[NUM_FP];

private:
	LOG_CLASS_DECLARE();
};



class OpenGLMatTextureSBMapPXS: public OpenGLMatSingleTextureSolid, public OpenGLMatPXS
{
	typedef OpenGLMatSingleTextureSolid Parent;
public:
	OpenGLMatTextureSBMapPXS(OpenGLRenderer* pRenderer,
				 Material* pFallBackMaterial);
	OpenGLMatTextureSBMapPXS(OpenGLRenderer* pRenderer,
				 Material* pFallBackMaterial,
				 const astr pVP,
				 const astr* pFP);

	virtual ~OpenGLMatTextureSBMapPXS();

protected:
	virtual void DoRenderAllGeometry(unsigned pCurrentFrame, const GeometryGroupList& pGeometryGroupList);
	virtual void RenderGeometry(TBC::GeometryBase* pGeometry);

	static const astr smVP;
	static const astr smFP[NUM_FP];

private:
	LOG_CLASS_DECLARE();
};



class OpenGLMatTextureDiffuseBumpMapPXS: public OpenGLMatTextureSBMapPXS
{
	typedef OpenGLMatTextureSBMapPXS Parent;
public:
	OpenGLMatTextureDiffuseBumpMapPXS(OpenGLRenderer* pRenderer,
					  Material* pFallBackMaterial);

	virtual ~OpenGLMatTextureDiffuseBumpMapPXS();

protected:

	static const astr smVP;
	static const astr smFP[NUM_FP];
};



}
