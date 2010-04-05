
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



//
//	OpenGLMaterial
//

class OpenGLMaterial: public Material
{
	typedef Material Parent;
public:
	OpenGLMaterial(OpenGLRenderer* pRenderer, Material::DepthSortHint pSortHint, Material* pFallBackMaterial);
	virtual ~OpenGLMaterial();

	virtual Material::RemoveStatus RemoveGeometry(TBC::GeometryBase* pGeometry);

	GLenum GetGLElementType(TBC::GeometryBase* pGeometry);

	virtual void SetBasicMaterial(const TBC::GeometryBase::BasicMaterialSettings& pMaterial);
	static void SetBasicMaterial(const TBC::GeometryBase::BasicMaterialSettings& pMaterial, OpenGLRenderer* pRenderer, bool pStore);

	virtual void RenderAllBlendedGeometry(unsigned pCurrentFrame);

protected:
	void UpdateTextureMatrix(TBC::GeometryBase* pGeometry);

	Material* mFallBackMaterial;	// If geometry doesn't contain all data needed.
};



class OpenGLMatSingleColorSolid: public OpenGLMaterial
{
public:
	inline OpenGLMatSingleColorSolid(OpenGLRenderer* pRenderer,
					 Material* pFallBackMaterial) :
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

	virtual void DoRenderAllGeometry(unsigned int pCurrentFrame);

protected:
	virtual void RenderGeometry(TBC::GeometryBase* pGeometry);
	virtual void RenderBaseGeometry(TBC::GeometryBase* pGeometry);
	virtual void PrepareBasicMaterialSettings(TBC::GeometryBase* pGeometry);
};



//
//	OpenGLMatSingleColorBlended
//

class OpenGLMatSingleColorBlended: public OpenGLMatSingleColorSolid
{
	typedef OpenGLMatSingleColorSolid Parent;
public:
	inline OpenGLMatSingleColorBlended(OpenGLRenderer* pRenderer,
					   Material* pFallBackMaterial) :
		OpenGLMatSingleColorSolid(pRenderer, Material::DEPTHSORT_B2F, pFallBackMaterial)
	{
	}

	inline virtual ~OpenGLMatSingleColorBlended(){}

	virtual void RenderAllGeometry(unsigned int pCurrentFrame);
	virtual void DoRenderAllGeometry(unsigned int pCurrentFrame);

protected:
};



//
//	OpenGLMatVertexColorSolid
//

class OpenGLMatVertexColorSolid: public OpenGLMatSingleColorSolid
{
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
	virtual void DoRenderAllGeometry(unsigned int pCurrentFrame);

protected:
	virtual void RenderGeometry(TBC::GeometryBase* pGeometry);

private:
	LOG_CLASS_DECLARE();
};


//
//	OpenGLMatVertexColorBlended
//

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

	virtual void RenderAllGeometry(unsigned int pCurrentFrame);
	virtual void DoRenderAllGeometry(unsigned int pCurrentFrame);
protected:
};



//
//	OpenGLMatSingleTextureSolid
//

class OpenGLMatSingleTextureSolid: public OpenGLMatSingleColorSolid
{
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
	virtual void DoRenderAllGeometry(unsigned int pCurrentFrame);

protected:
	
	virtual void RenderGeometry(TBC::GeometryBase* pGeometry);
	void BindTexture(int pTextureID);

	GLint mTextureParamMin;
	GLint mTextureParamMag;

private:
	LOG_CLASS_DECLARE();
};

//
//	OpenGLMatSingleTextureBlended
//

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

	virtual void RenderAllGeometry(unsigned int pCurrentFrame);
	virtual void DoRenderAllGeometry(unsigned int pCurrentFrame);

protected:
};


//
//	OpenGLMatSingleTextureAlphaTested
//

class OpenGLMatSingleTextureAlphaTested: public OpenGLMatSingleTextureSolid
{
public:
	inline OpenGLMatSingleTextureAlphaTested(OpenGLRenderer* pRenderer,
						 Material* pFallBackMaterial) :
		OpenGLMatSingleTextureSolid(pRenderer, Material::DEPTHSORT_F2B, pFallBackMaterial)
	{
	}

	inline virtual ~OpenGLMatSingleTextureAlphaTested(){}

	virtual void DoRenderAllGeometry(unsigned int pCurrentFrame);

protected:
	virtual void RenderGeometry(TBC::GeometryBase* pGeometry);
};


//
//	OpenGLMatSingleEnvMapSolid
//

class OpenGLMatSingleColorEnvMapSolid: public OpenGLMatSingleColorSolid
{
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
	virtual void DoRenderAllGeometry(unsigned int pCurrentFrame);

protected:
	virtual void PrepareBasicMaterialSettings(TBC::GeometryBase* pGeometry);

	bool mSingleColorPass;
	GLint mTextureParamMin;
	GLint mTextureParamMag;

private:
	LOG_CLASS_DECLARE();
};

//
//	OpenGLMatSingleEnvMapBlended
//

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

	virtual void RenderAllGeometry(unsigned int pCurrentFrame);
//	virtual void DoRenderAllGeometry(unsigned int pCurrentFrame);

protected:
	virtual void PrepareBasicMaterialSettings(TBC::GeometryBase* pGeometry);
};

//
//	OpenGLMatSingleTextureEnvMapSolid
//
class OpenGLMatSingleTextureEnvMapSolid: public OpenGLMatSingleTextureSolid
{
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

	virtual bool AddGeometry(TBC::GeometryBase* pGeometry);
	virtual void DoRenderAllGeometry(unsigned int pCurrentFrame);

protected:
	virtual void RenderGeometry(TBC::GeometryBase* pGeometry);
	virtual void PrepareBasicMaterialSettings(TBC::GeometryBase* pGeometry);

	bool mSingleTexturePass;

private:
	LOG_CLASS_DECLARE();
};


//
//	OpenGLMatSingleTextureEnvMapBlended
//
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

	virtual void RenderAllGeometry(unsigned int pCurrentFrame);
//	virtual void DoRenderAllGeometry(unsigned int pCurrentFrame);

protected:
	virtual void PrepareBasicMaterialSettings(TBC::GeometryBase* pGeometry);
};


//
// OpenGLMatTextureAndLightmap
//
class OpenGLMatTextureAndLightmap: public OpenGLMatSingleTextureSolid
{
public:
	inline OpenGLMatTextureAndLightmap(OpenGLRenderer* pRenderer,
					   Material* pFallBackMaterial) :
		OpenGLMatSingleTextureSolid(pRenderer, Material::DEPTHSORT_F2B, pFallBackMaterial)
	{
	}

	inline virtual ~OpenGLMatTextureAndLightmap(){}

	virtual bool AddGeometry(TBC::GeometryBase* pGeometry);
	virtual void DoRenderAllGeometry(unsigned int pCurrentFrame);

protected:
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

	OpenGLMatPXS(const char* pVP, const char* pFP[NUM_FP]);

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

//
// OpenGLMatSingleColorSolidPXS
//
class OpenGLMatSingleColorSolidPXS: public OpenGLMatSingleColorSolid, public OpenGLMatPXS
{
public:
	OpenGLMatSingleColorSolidPXS(OpenGLRenderer* pRenderer,
				     Material* pFallBackMaterial);

	virtual ~OpenGLMatSingleColorSolidPXS();

	virtual bool AddGeometry(TBC::GeometryBase* pGeometry);
	virtual void DoRenderAllGeometry(unsigned int pCurrentFrame);

protected:
	virtual void RenderGeometry(TBC::GeometryBase* pGeometry);

	static const char* smVP;
	static const char* smFP[NUM_FP];

private:
	LOG_CLASS_DECLARE();
};


//
// OpenGLMatSingleTextureSolidPXS
//
class OpenGLMatSingleTextureSolidPXS: public OpenGLMatSingleTextureSolid, public OpenGLMatPXS
{
public:
	OpenGLMatSingleTextureSolidPXS(OpenGLRenderer* pRenderer,
				       Material* pFallBackMaterial);

	virtual ~OpenGLMatSingleTextureSolidPXS();

	virtual bool AddGeometry(TBC::GeometryBase* pGeometry);
	virtual void DoRenderAllGeometry(unsigned int pCurrentFrame);

protected:
	virtual void RenderGeometry(TBC::GeometryBase* pGeometry);

	static const char* smVP;
	static const char* smFP[NUM_FP];

private:
	LOG_CLASS_DECLARE();
};



//
// OpenGLMatTextureAndLightmapPXS
//
class OpenGLMatTextureAndLightmapPXS: public OpenGLMatSingleTextureSolid, public OpenGLMatPXS
{
public:
	OpenGLMatTextureAndLightmapPXS(OpenGLRenderer* pRenderer,
				       Material* pFallBackMaterial);

	virtual ~OpenGLMatTextureAndLightmapPXS();

	virtual bool AddGeometry(TBC::GeometryBase* pGeometry);
	virtual void DoRenderAllGeometry(unsigned int pCurrentFrame);

protected:
	virtual void RenderGeometry(TBC::GeometryBase* pGeometry);

	static const char* smVP;
	static const char* smFP[NUM_FP];

private:
	LOG_CLASS_DECLARE();
};



//
// OpenGLMatTextureSBMapPXS
//
class OpenGLMatTextureSBMapPXS: public OpenGLMatSingleTextureSolid, public OpenGLMatPXS
{
public:
	OpenGLMatTextureSBMapPXS(OpenGLRenderer* pRenderer,
				 Material* pFallBackMaterial);
	OpenGLMatTextureSBMapPXS(OpenGLRenderer* pRenderer,
				 Material* pFallBackMaterial,
				 const char* pVP,
				 const char** pFP);

	virtual ~OpenGLMatTextureSBMapPXS();

	virtual bool AddGeometry(TBC::GeometryBase* pGeometry);
	virtual void DoRenderAllGeometry(unsigned int pCurrentFrame);

protected:
	virtual void RenderGeometry(TBC::GeometryBase* pGeometry);

	static const char* smVP;
	static const char* smFP[NUM_FP];

private:
	LOG_CLASS_DECLARE();
};

class OpenGLMatTextureDiffuseBumpMapPXS: public OpenGLMatTextureSBMapPXS
{
public:
	OpenGLMatTextureDiffuseBumpMapPXS(OpenGLRenderer* pRenderer,
					  Material* pFallBackMaterial);

	virtual ~OpenGLMatTextureDiffuseBumpMapPXS();

protected:

	static const char* smVP;
	static const char* smFP[NUM_FP];
};



}
