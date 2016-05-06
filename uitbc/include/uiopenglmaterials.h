
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once



#include "../../lepra/include/lepra.h"
#include "uimaterial.h"
#include "uiopenglrenderer.h"
#include <list>



namespace uitbc {



class OpenGLRenderer;



class OpenGLMaterial: public Material {
	typedef Material Parent;
public:
	OpenGLMaterial(OpenGLRenderer* renderer, Material::DepthSortHint sort_hint, Material* fall_back_material);
	virtual ~OpenGLMaterial();

	virtual Material::RemoveStatus RemoveGeometry(tbc::GeometryBase* geometry);

	static GLenum GetGLElementType(tbc::GeometryBase* geometry);

	void EnableDisableTexturing();
	void SetBasicMaterial(const tbc::GeometryBase::BasicMaterialSettings& material);
	static void SetBasicMaterial(const tbc::GeometryBase::BasicMaterialSettings& material, Renderer* renderer);
	void ResetBasicMaterial(const tbc::GeometryBase::BasicMaterialSettings& material);

	virtual void RenderAllBlendedGeometry(unsigned current_frame, const GeometryGroupList& geometry_group_list);

protected:
	void UpdateTextureMatrix(tbc::GeometryBase* geometry);
};



class OpenGLMatSingleColorSolid: public OpenGLMaterial {
	typedef OpenGLMaterial Parent;
public:
	inline OpenGLMatSingleColorSolid(OpenGLRenderer* renderer, Material* fall_back_material):
		OpenGLMaterial(renderer, Material::kDepthsortF2B, fall_back_material) {
	}
	inline OpenGLMatSingleColorSolid(OpenGLRenderer* renderer,
					 Material::DepthSortHint sort_hint,
					 Material* fall_back_material) :
		OpenGLMaterial(renderer, sort_hint, fall_back_material) {
	}

	inline virtual ~OpenGLMatSingleColorSolid(){}

	virtual void PreRender();
	virtual void PostRender();

protected:
	virtual void RenderGeometry(tbc::GeometryBase* geometry);
	virtual void RawRender(tbc::GeometryBase* geometry, int uv_set_index);
	virtual void RenderBaseGeometry(tbc::GeometryBase* geometry);
};



class OpenGLMatSingleColorBlended: public OpenGLMatSingleColorSolid {
	typedef OpenGLMatSingleColorSolid Parent;
public:
	inline OpenGLMatSingleColorBlended(OpenGLRenderer* renderer, Material* fall_back_material, bool outline):
		OpenGLMatSingleColorSolid(renderer, Material::kDepthsortB2F, fall_back_material),
		outline_(outline) {
	}

	inline virtual ~OpenGLMatSingleColorBlended(){}

	virtual void RenderAllGeometry(unsigned current_frame, const GeometryGroupList& geometry_group_list);
	static void DoPreRender();
	static void DoPostRender();
	virtual void PreRender();
	virtual void PostRender();

protected:
	bool outline_;
};



class OpenGLMatVertexColorSolid: public OpenGLMatSingleColorSolid {
	typedef OpenGLMatSingleColorSolid Parent;
public:
	inline OpenGLMatVertexColorSolid(OpenGLRenderer* renderer,
					 Material* fall_back_material) :
		OpenGLMatSingleColorSolid(renderer, Material::kDepthsortF2B, fall_back_material) {
	}
	inline OpenGLMatVertexColorSolid(OpenGLRenderer* renderer,
					 Material::DepthSortHint sort_hint,
					 Material* fall_back_material) :
		OpenGLMatSingleColorSolid(renderer, sort_hint, fall_back_material) {
	}

	inline virtual ~OpenGLMatVertexColorSolid(){}

	virtual bool AddGeometry(tbc::GeometryBase* geometry);

protected:
	virtual void PreRender();
	virtual void RenderGeometry(tbc::GeometryBase* geometry);
	virtual void RawRender(tbc::GeometryBase* geometry, int uv_set_index);

private:
	logclass();
};



class OpenGLMatVertexColorBlended: public OpenGLMatVertexColorSolid {
	typedef OpenGLMatVertexColorSolid Parent;
public:
	inline OpenGLMatVertexColorBlended(OpenGLRenderer* renderer,
					   Material* fall_back_material) :
		OpenGLMatVertexColorSolid(renderer, Material::kDepthsortB2F, fall_back_material) {
	}

	inline virtual ~OpenGLMatVertexColorBlended(){}

	virtual void RenderAllGeometry(unsigned current_frame, const GeometryGroupList& geometry_group_list);

protected:
	virtual void PreRender();
};



class OpenGLMatSingleTextureSolid: public OpenGLMatSingleColorSolid {
	typedef OpenGLMatSingleColorSolid Parent;
public:
	inline OpenGLMatSingleTextureSolid(OpenGLRenderer* renderer,
					   Material* fall_back_material) :
		OpenGLMatSingleColorSolid(renderer, Material::kDepthsortF2B, fall_back_material) {
	}
	inline OpenGLMatSingleTextureSolid(OpenGLRenderer* renderer,
					   Material::DepthSortHint sort_hint,
					   Material* fall_back_material) :
		OpenGLMatSingleColorSolid(renderer, sort_hint, fall_back_material) {
	}

	inline virtual ~OpenGLMatSingleTextureSolid(){}

	virtual bool AddGeometry(tbc::GeometryBase* geometry);
	virtual void PreRender();
	virtual void PostRender();
	void BindTexture(int texture_id, int mip_map_level_count);

	static void DoRawRender(tbc::GeometryBase* geometry, int uv_set_index);

protected:
	virtual void RenderGeometry(tbc::GeometryBase* geometry);
	virtual void RawRender(tbc::GeometryBase* geometry, int uv_set_index);

private:
	logclass();
};



class OpenGLMatSingleTextureHighlight: public OpenGLMatSingleTextureSolid {
	typedef OpenGLMatSingleTextureSolid Parent;
public:
	inline OpenGLMatSingleTextureHighlight(OpenGLRenderer* renderer,
					   Material* fall_back_material) :
		Parent(renderer, Material::kDepthsortF2B, fall_back_material) {
	}
	virtual void PreRender();
	virtual void PostRender();

private:
	bool is_lighted_;
};



class OpenGLMatSingleTextureBlended: public OpenGLMatSingleTextureSolid {
	typedef OpenGLMatSingleTextureSolid Parent;
public:
	inline OpenGLMatSingleTextureBlended(OpenGLRenderer* renderer,
					     Material* fall_back_material) :
		OpenGLMatSingleTextureSolid(renderer, Material::kDepthsortB2F, fall_back_material) {
	}
	inline OpenGLMatSingleTextureBlended(OpenGLRenderer* renderer,
					     Material::DepthSortHint sort_hint,
					     Material* fall_back_material) :
		OpenGLMatSingleTextureSolid(renderer, sort_hint, fall_back_material) {
	}

	inline virtual ~OpenGLMatSingleTextureBlended(){}

	virtual void RenderAllGeometry(unsigned current_frame, const GeometryGroupList& geometry_group_list);

protected:
	virtual void PreRender();
	virtual void PostRender();
};



class OpenGLMatSingleTextureAlphaTested: public OpenGLMatSingleTextureSolid {
	typedef OpenGLMatSingleTextureSolid Parent;
public:
	inline OpenGLMatSingleTextureAlphaTested(OpenGLRenderer* renderer,
						 Material* fall_back_material) :
		OpenGLMatSingleTextureSolid(renderer, Material::kDepthsortF2B, fall_back_material) {
	}

	inline virtual ~OpenGLMatSingleTextureAlphaTested(){}

protected:
	virtual void PreRender();
	virtual void PostRender();
	virtual void RenderGeometry(tbc::GeometryBase* geometry);
};



class OpenGLMatSingleColorEnvMapSolid: public OpenGLMatSingleTextureSolid {
	typedef OpenGLMatSingleTextureSolid Parent;
public:
	inline OpenGLMatSingleColorEnvMapSolid(OpenGLRenderer* renderer,
					       Material* fall_back_material) :
		Parent(renderer, Material::kDepthsortF2B, fall_back_material) {
	}
	inline OpenGLMatSingleColorEnvMapSolid(OpenGLRenderer* renderer,
					       Material::DepthSortHint sort_hint,
					       Material* fall_back_material) :
		Parent(renderer, sort_hint, fall_back_material) {
	}

	inline virtual ~OpenGLMatSingleColorEnvMapSolid(){}

	virtual bool AddGeometry(tbc::GeometryBase* geometry);
	virtual void PreRender();
	virtual void PostRender();

protected:
	virtual void DoRenderAllGeometry(unsigned current_frame, const GeometryGroupList& geometry_group_list);
	virtual void RenderGeometry(tbc::GeometryBase* geometry);

	bool single_color_pass_;

private:
	logclass();
};



class OpenGLMatSingleColorEnvMapBlended: public OpenGLMatSingleColorEnvMapSolid {
	typedef OpenGLMatSingleColorEnvMapSolid Parent;
public:
	inline OpenGLMatSingleColorEnvMapBlended(OpenGLRenderer* renderer,
						 Material* fall_back_material) :
		OpenGLMatSingleColorEnvMapSolid(renderer, Material::kDepthsortB2F, fall_back_material) {
	}

	inline virtual ~OpenGLMatSingleColorEnvMapBlended(){}

	virtual void RenderAllGeometry(unsigned current_frame, const GeometryGroupList& geometry_group_list);
	virtual void PreRender();
	virtual void PostRender();
};



class OpenGLMatSingleTextureEnvMapSolid: public OpenGLMatSingleTextureSolid {
	typedef OpenGLMatSingleTextureSolid Parent;
public:
	inline OpenGLMatSingleTextureEnvMapSolid(OpenGLRenderer* renderer,
						 Material* fall_back_material) :
		OpenGLMatSingleTextureSolid(renderer, Material::kDepthsortF2B, fall_back_material) {
	}
	inline OpenGLMatSingleTextureEnvMapSolid(OpenGLRenderer* renderer,
						 Material::DepthSortHint sort_hint,
						 Material* fall_back_material) :
		OpenGLMatSingleTextureSolid(renderer, sort_hint, fall_back_material) {
	}

	inline virtual ~OpenGLMatSingleTextureEnvMapSolid(){}

protected:
	virtual void DoRenderAllGeometry(unsigned current_frame, const GeometryGroupList& geometry_group_list);
	virtual void RenderGeometry(tbc::GeometryBase* geometry);
	virtual void RawRender(tbc::GeometryBase* geometry, int uv_set_index);

	bool single_texture_pass_;

private:
	logclass();
};



class OpenGLMatSingleTextureEnvMapBlended: public OpenGLMatSingleTextureEnvMapSolid {
	typedef OpenGLMatSingleTextureEnvMapSolid Parent;
public:
	inline OpenGLMatSingleTextureEnvMapBlended(OpenGLRenderer* renderer,
						   Material* fall_back_material) :
		OpenGLMatSingleTextureEnvMapSolid(renderer, Material::kDepthsortB2F, fall_back_material) {
	}

	inline virtual ~OpenGLMatSingleTextureEnvMapBlended(){}

	virtual void RenderAllGeometry(unsigned current_frame, const GeometryGroupList& geometry_group_list);
};



class OpenGLMatTextureAndLightmap: public OpenGLMatSingleTextureSolid {
	typedef OpenGLMatSingleTextureSolid Parent;
public:
	inline OpenGLMatTextureAndLightmap(OpenGLRenderer* renderer,
					   Material* fall_back_material) :
		OpenGLMatSingleTextureSolid(renderer, Material::kDepthsortF2B, fall_back_material) {
	}

	inline virtual ~OpenGLMatTextureAndLightmap(){}

protected:
	virtual void DoRenderAllGeometry(unsigned current_frame, const GeometryGroupList& geometry_group_list);
	virtual void RenderGeometry(tbc::GeometryBase* geometry);
	virtual void RawRender(tbc::GeometryBase* geometry, int uv_set_index);

	bool first_pass_;

private:
	logclass();
};



//
// Base class for materials using pixel shaders. As long as all shaders
// have the same interface (parameters at specific indices) this class
// provides some useful base functionality.
//

class OpenGLMatPXS {
public:
	static void PrepareLights(OpenGLRenderer* renderer);
	static void CleanupShaderPrograms();
	static void SetAmbientLight(OpenGLRenderer* renderer, tbc::GeometryBase* geometry);

protected:

	enum {
		kFpNone   = 0,

		kFp1Point,
		kFp2Point,
		kFp3Point,

		kFp1Dir,
		kFp2Dir,
		kFp3Dir,

		kFp1Spot,
		kFp2Spot,
		kFp3Spot,

		kFp1Dir1Point,
		kFp1Dir2Point,
		kFp2Dir1Point,

		kFp1Dir1Spot,
		kFp1Dir2Spot,
		kFp2Dir1Spot,

		kFp1Point1Spot,
		kFp1Point2Spot,
		kFp2Point1Spot,

		kFp1Dir1Point1Spot,

		kNumFp
	};

	enum {
		// This is the maximum number of lights supported by the shaders.
		// This is a hard coded value which can't be changed, because 3
		// lights is the maximum number which can fit in one fragment
		// program without exceeding the maximum number of instructions.
		kMaxShaderLights = 3
	};

	OpenGLMatPXS(const str& vp, const str fp[kNumFp]);

	void PrepareShaderPrograms(OpenGLRenderer* renderer);

	static inline int AllocProgramID() {
		return ++program_count_;
	}

	void SetToFallbackFP(int fp_type);

	int vpid_;
	int fpid_[kNumFp];

	// Vertex- and fragment-program counter. Used to allocate unique IDs for
	// each program.
	static int program_count_;

	// Fragment program lookup table.
	static bool fplut_initialized_;
	static int fplut_[4][4][4];

	// Light information.
	static float light_pos_[kMaxShaderLights * 4];
	static float light_dir_[kMaxShaderLights * 4];
	static float light_col_[kMaxShaderLights * 4];
	static float light_cut_[kMaxShaderLights];     // Cutoff = cos(CutOffAngle)
	static float light_exp_[kMaxShaderLights];     // Spot exponent.
	static Renderer::LightType light_type_[kMaxShaderLights];

	static int num_dir_lights_;
	static int num_pnt_lights_;
	static int num_spt_lights_;
	static int light_count_;

private:
	logclass();
};



class OpenGLMatSingleColorSolidPXS: public OpenGLMatSingleColorSolid, public OpenGLMatPXS {
	typedef OpenGLMatSingleColorSolid Parent;
public:
	OpenGLMatSingleColorSolidPXS(OpenGLRenderer* renderer,
				     Material* fall_back_material);

	virtual ~OpenGLMatSingleColorSolidPXS();

protected:
	virtual void DoRenderAllGeometry(unsigned current_frame, const GeometryGroupList& geometry_group_list);
	virtual void RenderGeometry(tbc::GeometryBase* geometry);

	static const str vp_;
	static const str fp_[kNumFp];

private:
	logclass();
};



class OpenGLMatSingleTextureSolidPXS: public OpenGLMatSingleTextureSolid, public OpenGLMatPXS {
	typedef OpenGLMatSingleTextureSolid Parent;
public:
	OpenGLMatSingleTextureSolidPXS(OpenGLRenderer* renderer,
				       Material* fall_back_material);

	virtual ~OpenGLMatSingleTextureSolidPXS();

protected:
	virtual void DoRenderAllGeometry(unsigned current_frame, const GeometryGroupList& geometry_group_list);
	virtual void RenderGeometry(tbc::GeometryBase* geometry);

	static const str vp_;
	static const str fp_[kNumFp];

private:
	logclass();
};



class OpenGLMatTextureAndLightmapPXS: public OpenGLMatTextureAndLightmap, public OpenGLMatPXS {
	typedef OpenGLMatTextureAndLightmap Parent;
public:
	OpenGLMatTextureAndLightmapPXS(OpenGLRenderer* renderer,
				       Material* fall_back_material);

	virtual ~OpenGLMatTextureAndLightmapPXS();

protected:
	virtual void DoRenderAllGeometry(unsigned current_frame, const GeometryGroupList& geometry_group_list);
	virtual void RenderGeometry(tbc::GeometryBase* geometry);
	virtual void RawRender(tbc::GeometryBase* geometry, int uv_set_index);

	static const str vp_;
	static const str fp_[kNumFp];

private:
	logclass();
};



class OpenGLMatTextureSBMapPXS: public OpenGLMatSingleTextureSolid, public OpenGLMatPXS {
	typedef OpenGLMatSingleTextureSolid Parent;
public:
	OpenGLMatTextureSBMapPXS(OpenGLRenderer* renderer,
				 Material* fall_back_material);
	OpenGLMatTextureSBMapPXS(OpenGLRenderer* renderer,
				 Material* fall_back_material,
				 const str vp,
				 const str* fp);

	virtual ~OpenGLMatTextureSBMapPXS();

protected:
	virtual void DoRenderAllGeometry(unsigned current_frame, const GeometryGroupList& geometry_group_list);
	virtual void RenderGeometry(tbc::GeometryBase* geometry);
	virtual void RawRender(tbc::GeometryBase* geometry, int uv_set_index);

	static const str vp_;
	static const str fp_[kNumFp];

private:
	logclass();
};



class OpenGLMatTextureDiffuseBumpMapPXS: public OpenGLMatTextureSBMapPXS {
	typedef OpenGLMatTextureSBMapPXS Parent;
public:
	OpenGLMatTextureDiffuseBumpMapPXS(OpenGLRenderer* renderer,
					  Material* fall_back_material);

	virtual ~OpenGLMatTextureDiffuseBumpMapPXS();

protected:

	static const str vp_;
	static const str fp_[kNumFp];
};



}
