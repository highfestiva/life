/*
	File:   OpenGLMaterials_SingleColorShaders.cpp
	Class:  -
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#include "pch.h"
#include "../Include/UiOpenGLMaterials.h"

namespace UiTbc
{

const astr OpenGLMatSingleColorSolidPXS::smVP =
	"!!ARBvp1.0\n\
	 OPTION ARB_position_invariant;\n\
	 # Diffuse vp.\n\
	 # Setup interpolants for diffuse lighting.\n\
	 \n\
	 ATTRIB iPos    = vertex.position;          # input position\n\
	 ATTRIB iPriCol = vertex.color.primary;     # input primary color\n\
	 ATTRIB iNormal = vertex.normal;            # input normal\n\
	 \n\
	 #OUTPUT oPos       = result.position;       # output position\n\
	 OUTPUT oPriCol    = result.color.primary;  # output primary color\n\
	 OUTPUT oFragPos   = result.texcoord[0];\n\
	 OUTPUT oNormal    = result.texcoord[1];\n\
	 \n\
	 PARAM mvp[4] = { state.matrix.mvp };       # model-view * projection matrix\n\
	 PARAM mv[4]  = { state.matrix.modelview }; # model-view matrix\n\
	 # inverse transpose of model-view matrix:\n\
	 PARAM mvIT[4] = { state.matrix.modelview.invtrans };\n\
	 \n\
	 TEMP V;                                    # temporary registers\n\
	 \n\
	 #DP4 oPos.x, iPos, mvp[0];                  # transform input pos by MVP\n\
	 #DP4 oPos.y, iPos, mvp[1];\n\
	 #DP4 oPos.z, iPos, mvp[2];\n\
	 #DP4 oPos.w, iPos, mvp[3];\n\
	 \n\
	 DP4 V.x, iPos, mv[0];                      # transform input pos by MV\n\
	 DP4 V.y, iPos, mv[1];\n\
	 DP4 V.z, iPos, mv[2];\n\
	 DP4 V.w, iPos, mv[3];\n\
	 \n\
	 MOV oFragPos, V;                           # output position for use in the fragment shader.\n\
	 \n\
	 DP3 oNormal.x, iNormal, mvIT[0];           # transform normal to eye space\n\
	 DP3 oNormal.y, iNormal, mvIT[1];\n\
	 DP3 oNormal.z, iNormal, mvIT[2];\n\
	 \n\
	 MOV oPriCol, iPriCol;\n\
	 END";

// Same header for all shaders.
#define FP_HEADER "\
!!ARBfp1.0\n\
ATTRIB iPrimCol   = fragment.color.primary;\n\
ATTRIB iSecCol    = fragment.color.secondary;\n\
ATTRIB iFragPos   = fragment.texcoord[0];# The fragment´s world position in eye space.\n\
ATTRIB iNormal    = fragment.texcoord[1];# The surface normal.\n\
\n\
OUTPUT outCol = result.color;\n\
\n\
PARAM specular   = program.local[0];   # specularity factor.\n\
PARAM cutoff     = program.local[1];   # cosine of cutoff angle.\n\
PARAM spotexp    = program.local[2];   # cosine of cutoff angle.\n\
PARAM ambientCol = program.local[3];   # ambient color.\n\
PARAM light0Pos  = program.local[4];   # light pos.\n\
PARAM light1Pos  = program.local[5];   # light pos.\n\
PARAM light2Pos  = program.local[6];   # light pos.\n\
PARAM light0Dir  = program.local[7];   # light dir.\n\
PARAM light1Dir  = program.local[8];   # light dir.\n\
PARAM light2Dir  = program.local[9];   # light dir.\n\
PARAM light0Col  = program.local[10];  # light color.\n\
PARAM light1Col  = program.local[11];  # light color.\n\
PARAM light2Col  = program.local[12];  # light color.\n\
PARAM constants  = {0, 6, 0, 128};\n\
\n\
TEMP N, L, H, V, Dist, Temp, toLIT, fromLIT;\n\
TEMP DiffuseCol;\n\
TEMP SpecularCol;\n\
TEMP fSpot;\n\
TEMP fSpotNorm;\n\
\n\
MOV DiffuseCol, constants.x;\n\
MOV SpecularCol, constants.x;\n\
MOV toLIT.w, constants.w;\n\
\n\
DP3 N.w, iNormal, iNormal;             # normalize normal\n\
RSQ N.w, N.w;\n\
MUL N, iNormal, N.w;\n\
\n\
DP3 V.w, iFragPos, iFragPos;           # normalize view vector\n\
RSQ V.w, V.w;\n\
MUL V, iFragPos, V.w;\n\
\n" // Line 36.

#define FP_CALC_LIGHT_VEC(lv) "\
SUB L, " #lv ", iFragPos;\n\
DP3 L.w, L, L;\n\
RCP Dist.x, L.w;\n\
RSQ L.w, L.w;\n\
MUL L, L, L.w;\n\
\n"

#define FP_CALC_FSPOT(ldir, lcol, exp) "\
DP3 fSpotNorm, "#ldir", -L;\n\
POW fSpot, fSpotNorm.x, spotexp."#exp";\n\
SUB fSpotNorm, fSpotNorm, cutoff."#exp";\n\
MAD Temp, constants.y, fSpotNorm, fSpot;   # These 3 rows are used to create\n\
MAX Temp, constants.x, fSpot;         # ...\n\
CMP fSpot, fSpotNorm, Temp, fSpot;\n\
MUL fSpot, fSpot, "#lcol";        # Light color.\n\
MUL fSpot, fSpot, Dist.x;         # Attenuation.\n\
\n"


#define FP_CALC_POINT_LIGHT(lcol) "\
SUB H, L, V;                           # half-angle vector\n\
DP3 H.w, H, H;                         # normalize it\n\
RSQ H.w, H.w;\n\
MUL H, H, H.w;\n\
\n\
DP3 toLIT.x, N, L;                     # diffuse dot product\n\
DP3 toLIT.y, N, H;                     # specular dot product\n\
LIT fromLIT, toLIT;\n\
MUL fromLIT, fromLIT, Dist.x;\n\
MAD DiffuseCol,  "#lcol", fromLIT.y, DiffuseCol;\n\
MAD SpecularCol, "#lcol", fromLIT.z, SpecularCol;\n\
\n"


#define FP_CALC_DIR_LIGHT(ldir, lcol) "\
SUB H, "#ldir", V;                     # half-angle vector\n\
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
MUL N, iPrimCol, ambientCol;\n\
MAD outCol.rgb, Temp, iPrimCol, N;\n\
MOV outCol.a, iPrimCol.a;              # preserve alpha\n\
END"

const astr OpenGLMatSingleColorSolidPXS::smFP[NUM_FP] =
{
	// Ambient light only.
	"!!ARBfp1.0\n\
	 ATTRIB iPrimCol   = fragment.color.primary;\n\
	 ATTRIB iSecCol    = fragment.color.secondary;\n\
	 \n\
	 OUTPUT outCol = result.color;\n\
	 \n\
	 PARAM specular   = program.local[0];   # specularity factor.\n\
	 PARAM ambientCol = program.local[3];   # ambient color.\n\
	 \n\
	 MUL outCol.rgb, iPrimCol, ambientCol;\n\
	 MOV outCol.a, iPrimCol.a;\n\
	 END",

	// 1 Point light.
	FP_HEADER
	FP_CALC_LIGHT_VEC(light0Pos)
	FP_CALC_POINT_LIGHT(light0Col)
	FP_END,

	// 2 Point lights.
	FP_HEADER
	FP_CALC_LIGHT_VEC(light0Pos)
	FP_CALC_POINT_LIGHT(light0Col)
	FP_CALC_LIGHT_VEC(light1Pos)
	FP_CALC_POINT_LIGHT(light1Col)
	FP_END,

	// 3 Point lights.
	FP_HEADER
	FP_CALC_LIGHT_VEC(light0Pos)
	FP_CALC_POINT_LIGHT(light0Col)
	FP_CALC_LIGHT_VEC(light1Pos)
	FP_CALC_POINT_LIGHT(light1Col)
	FP_CALC_LIGHT_VEC(light2Pos)
	FP_CALC_POINT_LIGHT(light2Col)
	FP_END,

	// 1 Directional light.
	FP_HEADER
	FP_CALC_DIR_LIGHT(light0Dir, light0Col)
	FP_END,

	// 2 Directional lights.
	FP_HEADER
	FP_CALC_DIR_LIGHT(light0Dir, light0Col)
	FP_CALC_DIR_LIGHT(light1Dir, light1Col)
	FP_END,

	// 3 Directional lights.
	FP_HEADER
	FP_CALC_DIR_LIGHT(light0Dir, light0Col)
	FP_CALC_DIR_LIGHT(light1Dir, light1Col)
	FP_CALC_DIR_LIGHT(light2Dir, light2Col)
	FP_END,

	// 1 Spot light.
	FP_HEADER
	FP_CALC_LIGHT_VEC(light0Pos)
	FP_CALC_FSPOT(light0Dir, light0Col, x)
	FP_CALC_SPOT_LIGHT
	FP_END,

	// 2 Spot lights.
	FP_HEADER
	FP_CALC_LIGHT_VEC(light0Pos)
	FP_CALC_FSPOT(light0Dir, light0Col, x)
	FP_CALC_SPOT_LIGHT
	FP_CALC_LIGHT_VEC(light1Pos)
	FP_CALC_FSPOT(light1Dir, light1Col, y)
	FP_CALC_SPOT_LIGHT
	FP_END,

	// 3 Spot lights.
	FP_HEADER
	FP_CALC_LIGHT_VEC(light0Pos)
	FP_CALC_FSPOT(light0Dir, light0Col, x)
	FP_CALC_SPOT_LIGHT
	FP_CALC_LIGHT_VEC(light1Pos)
	FP_CALC_FSPOT(light1Dir, light1Col, y)
	FP_CALC_SPOT_LIGHT
	FP_CALC_LIGHT_VEC(light2Pos)
	FP_CALC_FSPOT(light2Dir, light2Col, z)
	FP_CALC_SPOT_LIGHT
	FP_END,

	// 1 Directional light, 1 Point light.
	FP_HEADER
	FP_CALC_DIR_LIGHT(light0Dir, light0Col)
	FP_CALC_LIGHT_VEC(light1Pos)
	FP_CALC_POINT_LIGHT(light1Col)
	FP_END,

	// 1 Directional light, 2 Point lights.
	FP_HEADER
	FP_CALC_DIR_LIGHT(light0Dir, light0Col)
	FP_CALC_LIGHT_VEC(light1Pos)
	FP_CALC_POINT_LIGHT(light1Col)
	FP_CALC_LIGHT_VEC(light2Pos)
	FP_CALC_POINT_LIGHT(light2Col)
	FP_END,

	// 2 Directional lights, 1 Point light.
	FP_HEADER
	FP_CALC_DIR_LIGHT(light0Dir, light0Col)
	FP_CALC_DIR_LIGHT(light1Dir, light1Col)
	FP_CALC_LIGHT_VEC(light2Pos)
	FP_CALC_POINT_LIGHT(light2Col)
	FP_END,

	// 1 Directional light, 1 Spot light.
	FP_HEADER
	FP_CALC_DIR_LIGHT(light0Dir, light0Col)
	FP_CALC_LIGHT_VEC(light1Pos)
	FP_CALC_FSPOT(light1Dir, light1Col, y)
	FP_CALC_SPOT_LIGHT
	FP_END,

	// 1 Directional light, 2 Spot lights.
	FP_HEADER
	FP_CALC_DIR_LIGHT(light0Dir, light0Col)
	FP_CALC_LIGHT_VEC(light1Pos)
	FP_CALC_FSPOT(light1Dir, light1Col, y)
	FP_CALC_SPOT_LIGHT
	FP_CALC_LIGHT_VEC(light2Pos)
	FP_CALC_FSPOT(light2Dir, light2Col, z)
	FP_CALC_SPOT_LIGHT
	FP_END,

	// 2 Directional lights, 1 Spot light.
	FP_HEADER
	FP_CALC_DIR_LIGHT(light0Dir, light0Col)
	FP_CALC_DIR_LIGHT(light1Dir, light1Col)
	FP_CALC_LIGHT_VEC(light2Pos)
	FP_CALC_FSPOT(light2Dir, light2Col, z)
	FP_CALC_SPOT_LIGHT
	FP_END,

	// 1 Point light, 1 Spot light.
	FP_HEADER
	FP_CALC_LIGHT_VEC(light0Pos)
	FP_CALC_POINT_LIGHT(light0Col)
	FP_CALC_LIGHT_VEC(light1Pos)
	FP_CALC_FSPOT(light1Dir, light1Col, y)
	FP_CALC_SPOT_LIGHT
	FP_END,

	// 1 Point light, 2 Spot lights.
	FP_HEADER
	FP_CALC_LIGHT_VEC(light0Pos)
	FP_CALC_POINT_LIGHT(light0Col)
	FP_CALC_LIGHT_VEC(light1Pos)
	FP_CALC_FSPOT(light1Dir, light1Col, y)
	FP_CALC_SPOT_LIGHT
	FP_CALC_LIGHT_VEC(light2Pos)
	FP_CALC_FSPOT(light2Dir, light2Col, z)
	FP_CALC_SPOT_LIGHT
	FP_END,

	// 2 Point lights, 1 Spot light.
	FP_HEADER
	FP_CALC_LIGHT_VEC(light0Pos)
	FP_CALC_POINT_LIGHT(light0Col)
	FP_CALC_LIGHT_VEC(light1Pos)
	FP_CALC_POINT_LIGHT(light1Col)
	FP_CALC_LIGHT_VEC(light2Pos)
	FP_CALC_FSPOT(light2Dir, light2Col, z)
	FP_CALC_SPOT_LIGHT
	FP_END,

	// 1 Directional light, 1 Point light, 1 Spot light.
	FP_HEADER
	FP_CALC_DIR_LIGHT(light0Dir, light0Col)
	FP_CALC_LIGHT_VEC(light1Pos)
	FP_CALC_POINT_LIGHT(light1Col)
	FP_CALC_LIGHT_VEC(light2Pos)
	FP_CALC_FSPOT(light2Dir, light2Col, z)
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
