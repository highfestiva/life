
// Author: Jonas Bystr�m
// Copyright (c) Pixel Doctrine



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

	virtual Material::RemoveStatus RemoveGeometry(Tbc::GeometryBase* pGeometry);

	static GLenum GetGLElementType(Tbc::GeometryBase* pGeometry);

	void EnableDisableTexturing();
	void SetBasicMaterial(const Tbc::GeometryBase::BasicMaterialSettings& pMaterial);
	static void SetBasicMaterial(const Tbc::GeometryBase::BasicMaterialSettings& pMaterial, Renderer* pRenderer);
	void ResetBasicMaterial(const Tbc::GeometryBase::BasicMaterialSettings& pMaterial);

	virtual void RenderAllBlendedGeometry(unsigned pCurrentFrame, const GeometryGroupList& pGeometryGroupList);

protected:
	void UpdateTextureMatrix(Tbc::GeometryBase* pGeometry);
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
	virtual void RenderGeometry(Tbc::GeometryBase* pGeometry);
	virtual void RawRender(Tbc::GeometryBase* pGeometry, int pUVSetIndex);
	virtual void RenderBaseGeometry(Tbc::GeometryBase* pGeometry);
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
	static void DoPreRender();
	static void DoPostRender();
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

	virtual bool AddGeometry(Tbc::GeometryBase* pGeometry);

protected:
	virtual void PreRender();
	virtual void RenderGeometry(Tbc::GeometryBase* pGeometry);
	virtual void RawRender(Tbc::GeometryBase* pGeometry, int pUVSetIndex);

private:
	logclass();
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
	virtual void PreRender();
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

	virtual bool AddGeometry(Tbc::GeometryBase* pGeometry);
	virtual void PreRender();
	virtual void PostRender();
	void BindTexture(int pTextureID, int pMipMapLevelCount);

	static void DoRawRender(Tbc::GeometryBase* pGeometry, int pUVSetIndex);

protected:
	virtual void RenderGeometry(Tbc::GeometryBase* pGeometry);
	virtual void RawRender(Tbc::GeometryBase* pGeometry, int pUVSetIndex);

private:
	logclass();
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
	virtual void PreRender();
	virtual void PostRender();
	virtual void RenderGeometry(Tbc::GeometryBase* pGeometry);
};



class OpenGLMatSingleColorEnvMapSolid: public OpenGLMatSingleTextureSolid
{
	typedef OpenGLMatSingleTextureSolid Parent;
public:
	inline OpenGLMatSingleColorEnvMapSolid(OpenGLRenderer* pRenderer,
					       Material* pFallBackMaterial) :
		Parent(pRenderer, Material::DEPTHSORT_F2B, pFallBackMaterial)
	{
	}
	inline OpenGLMatSingleColorEnvMapSolid(OpenGLRenderer* pRenderer,
					       Material::DepthSortHint pSortHint,
					       Material* pFallBackMaterial) :
		Parent(pRenderer, pSortHint, pFallBackMaterial)
	{
	}

	inline virtual ~OpenGLMatSingleColorEnvMapSolid(){}

	virtual bool AddGeometry(Tbc::GeometryBase* pGeometry);
	virtual void PreRender();
	virtual void PostRender();

protected:
	virtual void DoRenderAllGeometry(unsigned pCurrentFrame, const GeometryGroupList& pGeometryGroupList);
	virtual void RenderGeometry(Tbc::GeometryBase* pGeometry);

	bool mSingleColorPass;

private:
	logclass();
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
	virtual void PreRender();
	virtual void PostRender();
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
	virtual void RenderGeometry(Tbc::GeometryBase* pGeometry);
	virtual void RawRender(Tbc::GeometryBase* pGeometry, int pUVSetIndex);

	bool mSingleTexturePass;

private:
	logclass();
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
	virtual void RenderGeometry(Tbc::GeometryBase* pGeometry);
	virtual void RawRender(Tbc::GeometryBase* pGeometry, int pUVSetIndex);

	bool mFirstPass;

private:
	logclass();
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
	static void CleanupShaderPrograms();
	static void SetAmbientLight(OpenGLRenderer* pRenderer, Tbc::GeometryBase* pGeometry);

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
	logclass();
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
	virtual void RenderGeometry(Tbc::GeometryBase* pGeometry);

	static const astr smVP;
	static const astr smFP[NUM_FP];

private:
	logclass();
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
	virtual void RenderGeometry(Tbc::GeometryBase* pGeometry);

	static const astr smVP;
	static const astr smFP[NUM_FP];

private:
	logclass();
};



class OpenGLMatTextureAndLightmapPXS: public OpenGLMatTextureAndLightmap, public OpenGLMatPXS
{
	typedef OpenGLMatTextureAndLightmap Parent;
public:
	OpenGLMatTextureAndLightmapPXS(OpenGLRenderer* pRenderer,
				       Material* pFallBackMaterial);

	virtual ~OpenGLMatTextureAndLightmapPXS();

protected:
	virtual void DoRenderAllGeometry(unsigned pCurrentFrame, const GeometryGroupList& pGeometryGroupList);
	virtual void RenderGeometry(Tbc::GeometryBase* pGeometry);
	virtual void RawRender(Tbc::GeometryBase* pGeometry, int pUVSetIndex);

	static const astr smVP;
	static const astr smFP[NUM_FP];

private:
	logclass();
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
	virtual void RenderGeometry(Tbc::GeometryBase* pGeometry);
	virtual void RawRender(Tbc::GeometryBase* pGeometry, int pUVSetIndex);

	static const astr smVP;
	static const astr smFP[NUM_FP];

private:
	logclass();
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