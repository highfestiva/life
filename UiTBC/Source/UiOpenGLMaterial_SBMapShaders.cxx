/*
	File:   OpenGLMaterials_SBMapShaders.cpp
	Class:  -
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games
*/

#include "../Include/UiOpenGLMaterials.h"

namespace UiTbc
{

const astr OpenGLMatTextureSBMapPXS::smVP =
	"!!ARBvp1.0\n\
	 OPTION ARB_position_invariant;\n\
	 \n\
	 ATTRIB iPos       = vertex.position;          # input position\n\
	 ATTRIB iPriCol    = vertex.color.primary;     # input primary color\n\
	 ATTRIB iNormal    = vertex.normal;            # input normal\n\
	 ATTRIB iTCoord0   = vertex.texcoord[0];       # input texture coordinates\n\
	 ATTRIB iTangent   = vertex.texcoord[1];       # input tangent vectors\n\
	 ATTRIB iBitangent = vertex.texcoord[2];       # input bitangent vectors\n\
	 \n\
	 #OUTPUT oPos           = result.position;       # output position\n\
	 OUTPUT oPriCol        = result.color.primary;  # output primary color\n\
	 OUTPUT oFragPos       = result.texcoord[0];\n\
	 OUTPUT oTCoord0       = result.texcoord[1];\n\
	 OUTPUT oLightVec0     = result.texcoord[2];\n\
	 OUTPUT oLightVec1     = result.texcoord[3];\n\
	 OUTPUT oLightVec2     = result.texcoord[4];\n\
	 OUTPUT oLightDir0     = result.texcoord[5];\n\
	 OUTPUT oLightDir1     = result.texcoord[6];\n\
	 OUTPUT oLightDir2     = result.texcoord[7];\n\
	 \n\
	 PARAM mvp[4] = { state.matrix.mvp };       # model-view * projection matrix\n\
	 PARAM mv[4]  = { state.matrix.modelview }; # model-view matrix\n\
	 PARAM mtx[4] = { state.matrix.texture };   # texture matrix\n\
	 # inverse transpose of model-view matrix:\n\
	 PARAM mvIT[4] = { state.matrix.modelview.invtrans };\n\
	 PARAM iLightPos0  = program.local[0];\n\
	 PARAM iLightPos1  = program.local[1];\n\
	 PARAM iLightPos2  = program.local[2];\n\
	 PARAM iLightDir0  = program.local[3];\n\
	 PARAM iLightDir1  = program.local[4];\n\
	 PARAM iLightDir2  = program.local[5];\n\
	 PARAM constants = {0.5, 0.5, 0, 0};\n\
	 \n\
	 TEMP V, L0, L1, L2, N, T, B;          # temporary registers\n\
	 \n\
	 MOV T, iTCoord0;\n\
	 DP4 oTCoord0.x, iTCoord0, mtx[0];\n\
	 DP4 oTCoord0.y, iTCoord0, mtx[1];\n\
	 DP4 oTCoord0.z, iTCoord0, mtx[2];\n\
	 DP4 oTCoord0.w, iTCoord0, mtx[3];\n\
	 #DP4 oPos.x, iPos, mvp[0];             # transform input pos by TBC::ModelViewProjection\n\
	 #DP4 oPos.y, iPos, mvp[1];\n\
	 #DP4 oPos.z, iPos, mvp[2];\n\
	 #DP4 oPos.w, iPos, mvp[3];\n\
	 \n\
	 DP4 V.x, iPos, mv[0];                 # transform input pos by TBC::ModelView\n\
	 DP4 V.y, iPos, mv[1];\n\
	 DP4 V.z, iPos, mv[2];\n\
	 DP4 V.w, iPos, mv[3];\n\
	 \n\
	 DP3 N.x, iNormal, mvIT[0];            # transform normal to eye space\n\
	 DP3 N.y, iNormal, mvIT[1];\n\
	 DP3 N.z, iNormal, mvIT[2];\n\
	 \n"
	"DP3 T.x, iTangent, mvIT[0];           # transform normal to eye space\n\
	 DP3 T.y, iTangent, mvIT[1];\n\
	 DP3 T.z, iTangent, mvIT[2];\n\
	 \n\
	 DP3 B.x, iBitangent, mvIT[0];         # transform normal to eye space\n\
	 DP3 B.y, iBitangent, mvIT[1];\n\
	 DP3 B.z, iBitangent, mvIT[2];\n\
	 \n\
	 # Transform light positions to tangent space.\n\
	 SUB L0, iLightPos0, V;\n\
	 DP3 oLightVec0.x, L0, T;\n\
	 DP3 oLightVec0.y, L0, B;\n\
	 DP3 oLightVec0.z, L0, N;\n\
	 \n\
	 SUB L1, iLightPos1, V;\n\
	 DP3 oLightVec1.x, L1, T;\n\
	 DP3 oLightVec1.y, L1, B;\n\
	 DP3 oLightVec1.z, L1, N;\n\
	 \n\
	 SUB L2, iLightPos2, V;\n\
	 DP3 oLightVec2.x, L2, T;\n\
	 DP3 oLightVec2.y, L2, B;\n\
	 DP3 oLightVec2.z, L2, N;\n\
	 \n\
	 DP3 oLightDir0.x, iLightDir0, T;\n\
	 DP3 oLightDir0.y, iLightDir0, B;\n\
	 DP3 oLightDir0.z, iLightDir0, N;\n\
	 \n\
	 DP3 oLightDir1.x, iLightDir1, T;\n\
	 DP3 oLightDir1.y, iLightDir1, B;\n\
	 DP3 oLightDir1.z, iLightDir1, N;\n\
	 \n\
	 DP3 oLightDir2.x, iLightDir2, T;\n\
	 DP3 oLightDir2.y, iLightDir2, B;\n\
	 DP3 oLightDir2.z, iLightDir2, N;\n\
	 \n\
	 DP3 oFragPos.x, V, T;\n\
	 DP3 oFragPos.y, V, B;\n\
	 DP3 oFragPos.z, V, N;\n\
	 \n\
	 MOV oPriCol, iPriCol;\n\
	 END";

// Same header for all shaders.
#define FP_HEADER "\
!!ARBfp1.0\n\
ATTRIB iPrimCol       = fragment.color.primary;\n\
ATTRIB iFragPos       = fragment.texcoord[0];\n\
ATTRIB iTCoord0       = fragment.texcoord[1];\n\
ATTRIB iLightVec0     = fragment.texcoord[2];\n\
ATTRIB iLightVec1     = fragment.texcoord[3];\n\
ATTRIB iLightVec2     = fragment.texcoord[4];\n\
ATTRIB iLightDir0     = fragment.texcoord[5];\n\
ATTRIB iLightDir1     = fragment.texcoord[6];\n\
ATTRIB iLightDir2     = fragment.texcoord[7];\n\
\n\
OUTPUT outCol = result.color;\n\
\n\
PARAM iSpecular  = program.local[0];   # specularity factor.\n\
PARAM cutoff     = program.local[1];   # cosine of cutoff angle.\n\
PARAM spotexp    = program.local[2];   # cosine of cutoff angle.\n\
PARAM ambientCol = program.local[3];   # ambient color.\n\
PARAM iLightCol0  = program.local[10];  # light color.\n\
PARAM iLightCol1  = program.local[11];  # light color.\n\
PARAM iLightCol2  = program.local[12];  # light color.\n\
PARAM constants  = {0, 6, 2, 128};\n\
PARAM constants2 = {-1, 1, 0.5, 0};\n\
\n\
TEMP N, L, H, V, Temp, toLIT, fromLIT, iBitang;\n\
TEMP DiffuseCol;\n\
TEMP SpecularCol;\n\
TEMP fSpot;\n\
TEMP fSpotNorm;\n\
TEMP primCol;\n\
TEMP specular;\n\
\n\
TEX primCol, iTCoord0, texture[0], 2D;\n\
MUL primCol, primCol, iPrimCol;\n\
\n\
TEX Temp, iTCoord0, texture[1], 2D;       # read specularity from specular map.\n\
MUL specular, Temp, iSpecular;\n\
\n\
TEX N, iTCoord0, texture[2], 2D;     # read normal map.\n\
MAD N, N, constants.z, constants2.x;\n\
\n\
MOV DiffuseCol, constants.x;\n\
MOV SpecularCol, constants.x;\n\
MOV toLIT.w, constants.w;\n\
\n\
DP3 V.w, iFragPos, iFragPos;           # normalize view vector\n\
RSQ V.w, V.w;\n\
MUL V, iFragPos, V.w;\n\
\n" // Line 55.

#define FP_CALC_LIGHT_VEC(lv) "\
DP3 Temp.w, "#lv", "#lv";\n\
RCP L.w, Temp.w;\n\
RSQ Temp.w, Temp.w;\n\
MUL L.xyz, "#lv", Temp.w;\n\
\n"

#define FP_CALC_FSPOT(ldir, lcol, exp) "\
DP3 fSpotNorm, "#ldir", -L;\n\
POW fSpot, fSpotNorm.x, spotexp."#exp";\n\
SUB fSpotNorm, fSpotNorm, cutoff."#exp";\n\
MAD Temp, constants.y, fSpotNorm, fSpot;   # These 3 rows are used to create\n\
MAX Temp, constants.x, fSpot;         # ...\n\
CMP fSpot, fSpotNorm, Temp, fSpot;\n\
MUL fSpot, fSpot, "#lcol";        # Light color.\n\
MUL fSpot, fSpot, L.w;         # Attenuation.\n\
\n"

#define FP_CALC_POINT_LIGHT(lcol, lightVec) "\
SUB H, L, V;                           # half-angle vector\n\
DP3 H.w, H, H;                         # normalize it\n\
RSQ H.w, H.w;\n\
MUL H, H, H.w;\n\
\n\
DP3 toLIT.x, N, L;                     # diffuse dot product\n\
DP3 toLIT.y, N, H;                     # specular dot product\n\
LIT fromLIT, toLIT;\n\
MUL fromLIT, fromLIT, L.w;\n\
MAD DiffuseCol,  "#lcol", fromLIT.y, DiffuseCol;\n\
MAD SpecularCol, "#lcol", fromLIT.z, SpecularCol;\n\
\n"


#define FP_CALC_DIR_LIGHT(ldir, lcol) "\
SUB H, "#ldir", V;                           # half-angle vector\n\
DP3 H.w, H, H;                         # normalize it\n\
RSQ H.w, H.w;\n\
MUL H, H, H.w;\n\
\n\
DP3 toLIT.x, N, "#ldir";               # diffuse dot product\n\
DP3 toLIT.y, N, H;                     # specular dot product\n\
LIT fromLIT, toLIT;\n\
MAD DiffuseCol, "#lcol", fromLIT.y, DiffuseCol;\n\
MAD SpecularCol, "#lcol", fromLIT.z, SpecularCol;\n\
\n"

#define FP_CALC_SPOT_LIGHT "\
SUB H, L, V;                           # half-angle vector\n\
DP3 H.w, H, H;                         # normalize it\n\
RSQ H.w, H.w;\n\
MUL H, H, H.w;\n\
\n\
DP3 toLIT.x, N, L;                     # diffuse dot product\n\
DP3 toLIT.y, N, H;                     # specular dot product\n\
LIT fromLIT, toLIT;\n\
MAD DiffuseCol,  fSpot, fromLIT.y, DiffuseCol;\n\
MAD SpecularCol, fSpot, fromLIT.z, SpecularCol;\n\
\n"

#define FP_END "\
LRP Temp, specular, SpecularCol, DiffuseCol;\n\
MUL N, primCol, ambientCol;\n\
MAD outCol.rgb, Temp, primCol, N;\n\
MOV outCol.a, iPrimCol.a;              # preserve alpha\n\
END"


const astr OpenGLMatTextureSBMapPXS::smFP[NUM_FP] =
{
	// Ambient light only.
	"!!ARBfp1.0\n\
	 ATTRIB iPrimCol   = fragment.color.primary;\n\
	 ATTRIB iTCoord    = fragment.texcoord[1];\n\
	 \n\
	 OUTPUT outCol = result.color;\n\
	 \n\
	 PARAM specular   = program.local[0];   # specularity factor.\n\
	 PARAM ambientCol = program.local[3];   # ambient color.\n\
	 \n\
	 TEMP  primCol, diffuse, T;\n\
	 \n\
	 TEX primCol, iTCoord, texture[0], 2D;\n\
	 MUL primCol, primCol, iPrimCol;\n\
	 MUL outCol.rgb, primCol, ambientCol;\n\
	 MOV outCol.a, iPrimCol.a;\n\
	 END",

	// 1 Point light.
	FP_HEADER
	FP_CALC_LIGHT_VEC(iLightVec0)
	FP_CALC_POINT_LIGHT(iLightCol0, iLightVec0)
	FP_END,

	// 2 Point lights.
	FP_HEADER
	FP_CALC_LIGHT_VEC(iLightVec0)
	FP_CALC_POINT_LIGHT(iLightCol0, iLightVec0)
	FP_CALC_LIGHT_VEC(iLightVec1)
	FP_CALC_POINT_LIGHT(iLightCol1, iLightVec1)
	FP_END,

	// 3 Point lights.
	FP_HEADER
	FP_CALC_LIGHT_VEC(iLightVec0)
	FP_CALC_POINT_LIGHT(iLightCol0, iLightVec0)
	FP_CALC_LIGHT_VEC(iLightVec1)
	FP_CALC_POINT_LIGHT(iLightCol1, iLightVec1)
	FP_CALC_LIGHT_VEC(iLightVec2)
	FP_CALC_POINT_LIGHT(iLightCol2, iLightVec2)
	FP_END,

	// 1 Directional light.
	FP_HEADER
	FP_CALC_DIR_LIGHT(iLightDir0, iLightCol0)
	FP_END,

	// 2 Directional lights.
	FP_HEADER
	FP_CALC_DIR_LIGHT(iLightDir0, iLightCol0)
	FP_CALC_DIR_LIGHT(iLightDir1, iLightCol1)
	FP_END,

	// 3 Directional lights.
	FP_HEADER
	FP_CALC_DIR_LIGHT(iLightDir0, iLightCol0)
	FP_CALC_DIR_LIGHT(iLightDir1, iLightCol1)
	FP_CALC_DIR_LIGHT(iLightDir2, iLightCol2)
	FP_END,

	// 1 Spot light.
	FP_HEADER
	FP_CALC_LIGHT_VEC(iLightVec0)
	FP_CALC_FSPOT(iLightDir0, iLightCol0, x)
	FP_CALC_SPOT_LIGHT
	FP_END,

	// 2 Spot lights.
	FP_HEADER
	FP_CALC_LIGHT_VEC(iLightVec0)
	FP_CALC_FSPOT(iLightDir0, iLightCol0, x)
	FP_CALC_SPOT_LIGHT
	FP_CALC_LIGHT_VEC(iLightVec1)
	FP_CALC_FSPOT(iLightDir1, iLightCol1, y)
	FP_CALC_SPOT_LIGHT
	FP_END,

	// 3 Spot lights.
	FP_HEADER
	FP_CALC_LIGHT_VEC(iLightVec0)
	FP_CALC_FSPOT(iLightDir0, iLightCol0, x)
	FP_CALC_SPOT_LIGHT
	FP_CALC_LIGHT_VEC(iLightVec1)
	FP_CALC_FSPOT(iLightDir1, iLightCol1, y)
	FP_CALC_SPOT_LIGHT
	FP_CALC_LIGHT_VEC(iLightVec2)
	FP_CALC_FSPOT(iLightDir2, iLightCol2, z)
	FP_CALC_SPOT_LIGHT
	FP_END,

	// 1 Directional light, 1 Point light.
	FP_HEADER
	FP_CALC_DIR_LIGHT(iLightDir0, iLightCol0)
	FP_CALC_LIGHT_VEC(iLightVec1)
	FP_CALC_POINT_LIGHT(iLightCol1, iLightVec1)
	FP_END,

	// 1 Directional light, 2 Point lights.
	FP_HEADER
	FP_CALC_DIR_LIGHT(iLightDir0, iLightCol0)
	FP_CALC_LIGHT_VEC(iLightVec1)
	FP_CALC_POINT_LIGHT(iLightCol1, iLightVec1)
	FP_CALC_LIGHT_VEC(iLightVec2)
	FP_CALC_POINT_LIGHT(iLightCol2, iLightVec2)
	FP_END,

	// 2 Directional lights, 1 Point light.
	FP_HEADER
	FP_CALC_DIR_LIGHT(iLightDir0, iLightCol0)
	FP_CALC_DIR_LIGHT(iLightDir1, iLightCol1)
	FP_CALC_LIGHT_VEC(iLightVec2)
	FP_CALC_POINT_LIGHT(iLightCol2, iLightVec2)
	FP_END,

	// 1 Directional light, 1 Spot light.
	FP_HEADER
	FP_CALC_DIR_LIGHT(iLightDir0, iLightCol0)
	FP_CALC_LIGHT_VEC(iLightVec1)
	FP_CALC_FSPOT(iLightDir1, iLightCol1, y)
	FP_CALC_SPOT_LIGHT
	FP_END,

	// 1 Directional light, 2 Spot lights.
	FP_HEADER
	FP_CALC_DIR_LIGHT(iLightDir0, iLightCol0)
	FP_CALC_LIGHT_VEC(iLightVec1)
	FP_CALC_FSPOT(iLightDir1, iLightCol1, y)
	FP_CALC_SPOT_LIGHT
	FP_CALC_LIGHT_VEC(iLightVec2)
	FP_CALC_FSPOT(iLightDir2, iLightCol2, z)
	FP_CALC_SPOT_LIGHT
	FP_END,

	// 2 Directional lights, 1 Spot light.
	FP_HEADER
	FP_CALC_DIR_LIGHT(iLightDir0, iLightCol0)
	FP_CALC_DIR_LIGHT(iLightDir1, iLightCol1)
	FP_CALC_LIGHT_VEC(iLightVec2)
	FP_CALC_FSPOT(iLightDir2, iLightCol2, z)
	FP_CALC_SPOT_LIGHT
	FP_END,

	// 1 Point light, 1 Spot light.
	FP_HEADER
	FP_CALC_LIGHT_VEC(iLightVec0)
	FP_CALC_POINT_LIGHT(iLightCol0, iLightVec0)
	FP_CALC_LIGHT_VEC(iLightVec1)
	FP_CALC_FSPOT(iLightDir1, iLightCol1, y)
	FP_CALC_SPOT_LIGHT
	FP_END,

	// 1 Point light, 2 Spot lights.
	FP_HEADER
	FP_CALC_LIGHT_VEC(iLightVec0)
	FP_CALC_POINT_LIGHT(iLightCol0, iLightVec0)
	FP_CALC_LIGHT_VEC(iLightVec1)
	FP_CALC_FSPOT(iLightDir1, iLightCol1, y)
	FP_CALC_SPOT_LIGHT
	FP_CALC_LIGHT_VEC(iLightVec2)
	FP_CALC_FSPOT(iLightDir2, iLightCol2, z)
	FP_CALC_SPOT_LIGHT
	FP_END,

	// 2 Point lights, 1 Spot light.
	FP_HEADER
	FP_CALC_LIGHT_VEC(iLightVec0)
	FP_CALC_POINT_LIGHT(iLightCol0, iLightVec0)
	FP_CALC_LIGHT_VEC(iLightVec1)
	FP_CALC_POINT_LIGHT(iLightCol1, iLightVec1)
	FP_CALC_LIGHT_VEC(iLightVec2)
	FP_CALC_FSPOT(iLightDir2, iLightCol2, z)
	FP_CALC_SPOT_LIGHT
	FP_END,

	// 1 Directional light, 1 Point light, 1 Spot light.
	FP_HEADER
	FP_CALC_DIR_LIGHT(iLightDir0, iLightCol0)
	FP_CALC_LIGHT_VEC(iLightVec1)
	FP_CALC_POINT_LIGHT(iLightCol1, iLightVec1)
	FP_CALC_LIGHT_VEC(iLightVec2)
	FP_CALC_FSPOT(iLightDir2, iLightCol2, z)
	FP_CALC_SPOT_LIGHT
	FP_END
};

#undef FP_HEADER
#undef FP_CALC_LIGHT_VEC
#undef FP_CALC_FSPOT
#undef FP_CALC_POINT_LIGHT
#undef FP_CALC_DIR_LIGHT
#undef FP_CALC_SPOT_LIGHT
#undef FP_END

} // End namespace.
