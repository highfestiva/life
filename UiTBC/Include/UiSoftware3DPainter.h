/*
	Class:  Software3DPainter
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand

	NOTES:

	This is a totally idiotic class, because of the huge amount of code, to
	do something that OpenGL or DirectX will do better hardware 
	accelerated. But it was great fun to write it! :D

	There are only two reasons why this class hasn't been removed. First of 
	all, it might be useful the day when you feel to write a simple 3D-game 
	on a platform that doesn't have hardware accelerated 3D-graphics (like 
	mobile phones and such). Second, it can be used to play around with. 
	Maybe to write a classic Doom-clone just for the fun of it.

	To render something "fast" you have to write specialized code for that. 
	To write it to support all bit depths and more, you have to write one 
	specialized function for each damn thing you want to do. That's why I 
	ended up with over 80 fast scanline functions at about 9500 rows of 
	code. It was much copy-and-paste-programming. I hope that I'll never 
	find any bugs in that code... :)

	I used an article written by Chris Hecker to get all the math right. 
	The article and the described math/algorithm was far from perfect, and 
	there where many bugs to fix. You can find the article at:
	http://www.d6.com/users/checker/misctech.htm. (Year 2003).


	RENDERING MODES:


	There are two modes of rendering: RENDER_NICEST and RENDER_FAST. You 
	can switch between them in runtime if you wish (if you make sure the 
	textures have the same bit-depth as the screen) by calling 
	SetRenderingHint().

	It is NOT recommended to use RENDER_NICEST with 8-bit color modes, 
	because it doesn't support all the stuff the other modes offer (alpha 
	blending, vertex colors, etc), and it is still slow. If you use vertex 
	colors in 8-bit mode, different things might happen:

	1. If shading a textured polygon, the colors will be converted to a 
	grayscale light intensity value which will make the polygon shaded from
	black to full light.
	2. If shading a non-textured polygon, the red-component of the colors 
	will be used as a palette index. It is your responsibility to choose 
	the right index, and to convert it to a value between 0.0 and 1.0. This 
	applies to the RENDER_FAST-mode as well.

	In all other color modes, using the RENDER_NICEST hint, you are free to 
	use all things that are supported. You don't even have to convert the 
	texture bit depth, since it is done per pixel in runtime. Well, as you 
	might have guessed, this mode is SLOW.

	When using RENDER_NICEST all colors and shading will be gamma 
	corrected, using a gamma of 2.2. Whether you want it or not, this is 
	the TRUE way of handling colors on a modern computer and looks much 
	better than linearly interpolate r-, g- and b-values.

	RENDER_FAST is the mode you'd like to use if you are going to write a 
	realtime software rendered 3D-engine. By quite obvious reasons, this 
	mode can't possibly be as supportive as the RENDER_NICEST-mode. It 
	supports all bit depths though, but I don't think it is a good advise 
	to use it in 24- or 32-bit modes anyway. The textures must have the 
	same bit depth as the screen.

	The following is supported in this mode:

	1. Single colored triangles (flat-shading).
	2. Shaded triangles (perspective corrected gouraud shading).
	3. Textured triangles.
	4. Shaded and textured triangles, mixed.

	To be implemented:
	5. Alpha tested textured polygons.
	6. Alpha blended polygons with 50% alpha in 15-, 16-, 24- and 32-bit 
	   modes.

	HOW TO USE IT:

	I have no time to describe how to use this class. I think it's quite 
	obvious if you have used 3D-rendering APIs before. Otherwise, don't be 
	afraid to use the trial and error method. Read the "HOW TO FOLLOW THE 
	CODE"-section

	FUTURE DEVELOPMENT:

	I've written it all in C++ to make it portable, but you can't really rely 
	on the compiler to do a good job optimizing your code. To make the 
	RENDER_FAST mode to render stuff as fast as Quake did back in the days
	you have to write things using assembly. But if you do, remember to put 
	the code within a #ifdef-#endif block. 

	The only thing you need to optimize this way is the scanline functions. 
	Don't bother optimizing any other code. It is quite optimized already, 
	and optimizing it even more won't speed up things as much making it 
	worth the trouble.

	HOW TO FOLLOW THE CODE:

	I have tried to make this code as easy to read as possible, but it is 
	a huge class with a huge amount of code, and that makes it a little bit 
	harder to follow. I've been thinking of splitting it into a bunch of 
	smaller classes, but I can't realize how such a system should be 
	designed.

	To give you a good picture of how this complex machinery works, read 
	the follwing:

	First, you set all parameters you want to set, and activate/deactivate 
	all kind of things. Then, when you call Begin() to start render 
	something, all function pointers will be updated to point out the 
	correct functions, if there has been a "state change" (if you have 
	called any Set-function).

	Then it is up to you if you want to render something using the 
	Vertex()-, Color()- and UV()-functions or the RenderBuffers()-function. 
	Here I will describe the way it goes using the Vertex()-, Color()- and 
	UV()-functions.

	When one of these three gets enough data to render the currently 
	selected primitive, it will make a try doing so by calling 
	RenderPrimitive(), which is a function pointer pointing out the 
	appropriate function. If there is actually enough data to render the 
	primitive (checked in every such function), it will do so. 

	Rendering triangles will lead you to the messy part of the code using 
	a bunch disgusting macros. The primitive rendering functions will do 
	all the standard 3D stuff like hidden surface removal (or backface 
	culling), projection and clipping to near z. Then, a DrawXXXTriangle() 
	function will be called, which is actually NOT a function pointer. The 
	triangle functions are really few and small, and this code is really 
	easy to read, if you ask me.

	Each triangle rendering function will call a scanline function, 
	rendering one horisontal scanline. These functions are function 
	pointers as well, pointing out one function among millions, and that's 
	all there is to it. Enjoy.
*/

#ifndef UISOFTWARE3DPAINTER_H
#define UISOFTWARE3DPAINTER_H

#include <math.h>
#include "../../Lepra/Include/Transformation.h"
#include "../../Lepra/Include/Graphics2D.h"
#include "../../Lepra/Include/Canvas.h"
#include "../../Lepra/Include/Vector2D.h"
#include "UiEdge.h"
#include "UiVertex.h"
#include "UiSoftwarePainter.h"
#include "UiTexture.h"

namespace UiTbc
{

class Software3DPainter
{
public:

	enum RenderingHint
	{
		RENDER_FAST = 0,
		RENDER_NICEST,
	};

	enum AlphaSource
	{
		ALPHA_VALUE = 0,
		ALPHA_BUFFER,
		ALPHA_CHANNEL,		// In 32-bit modes...
	};

	enum RenderingPrimitive
	{
		RENDER_LINES = 0,
		RENDER_LINE_STRIP,
		RENDER_TRIANGLES,
		RENDER_TRIANGLE_STRIP,
		RENDER_TRIANGLE_FAN,
	};

	enum LightType
	{
		LIGHT_NONE = -1,
		LIGHT_DIRECTIONAL,
		LIGHT_POINT,
	};

	enum LightModel
	{
		LIGHT_FLAT = 0,
		LIGHT_GOURAUD,
		LIGHT_PHONG,
	};

	enum VariableType
	{
		TYPE_BYTE = 0,
		TYPE_WORD,
		TYPE_LONG,
		TYPE_FLOAT,
		TYPE_DOUBLE,
	};

	enum FaceOrientation
	{
		FACE_CW = 0,
		FACE_CCW,
	};

	enum
	{
		MAX_LIGHTS = 32,
	};

	Software3DPainter();
	~Software3DPainter();

	void ClearZBuffer();
	inline void ClearColorBuffer();

	void SetFrontFace(FaceOrientation pFO);

	// Similar to OpenGL.
	void Begin(RenderingPrimitive pPrimitive);
	void End();
	void Vertex(float pX, float pY, float pZ);
	void UV(float pU, float pV);
	void Color(float pRed, float pGreen, float pBlue);
	void Normal(float pX, float pY, float pZ);

	void DrawArrayElements(const Lepra::uint32* pIndices, int pCount, RenderingPrimitive pPrimitive);
	inline void SetVertexPointer(int pCount, const float* pPointer);
	inline void SetUVPointer(const float* pPointer);
	inline void SetColorPointer(const float* pPointer);
	inline void SetColorPointer(const Lepra::uint8* pPointer);
	inline void SetNormalPointer(const float* pPointer);
	inline void SetDefaultColor(float pRed, float pGreen, float pBlue);

	void SetZTestEnabled(bool pEnabled);
	void SetZWriteEnabled(bool pEnabled);
	void SetBilinearFilteringEnabled(bool pEnabled);
	void SetTrilinearFilteringEnabled(bool pEnabled);
	void SetAlphaTestEnabled(bool pEnabled);
	void SetAlphaBlendEnabled(bool pEnabled);
	void SetVertexColorsEnabled(bool pEnabled);
	void SetTextureMappingEnabled(bool pEnabled);
	void SetMipMappingEnabled(bool pEnabled);

	// Lights require vertex colors...
	void SetLightsEnabled(bool pEnabled);

	// Activate/deactivate search for matching color in 8 bit color mode.
	inline void Set8BitColorSearchModeEnabled(bool pEnabled);

	void SetRenderingHint(RenderingHint pHint);

	// SetViewport() is using screen coordinates with the top left corner 
	// as reference. Resets the clipping rect. May be outside of screen boundaries.
	void SetViewport(int pLeft, int pTop, int pRight, int pBottom);
	void GetViewport(int& pLeft, int& pTop, int& pRight, int& pBottom);

	// The view frustum automatically adapts to the viewport. FOVAngle
	// (FOV = Field Of View) is the perspective angle. 0 < FOVAngle < 180.
	void SetViewFrustum(float pFOVAngle, float pNearZ, float pFarZ);
	void GetViewFrustum(float& pFOVAngle, float& pNearZ, float& pFarZ);

	// The clipping rect must be within the viewport and the screen.
	void SetClippingRect(int pLeft, int pTop, int pRight, int pBottom);
	// Reset to cover the whole viewport.
	void ResetClippingRect();

	// This is the default transform applied to all objects, including the camera.
	// You can use this to redefine the coordinate system.
	void SetDefaultTransform(const Lepra::TransformationF& pTransform);
	void SetDefaultTransform(const Lepra::RotationMatrixF& pOrientation,
				 const Lepra::Vector3DF& pPosition);

	// Camera stuff...
	void SetCameraTransform(const Lepra::TransformationF& pTransform);
	const Lepra::TransformationF& GetCameraTransform();

	// Model transform, applies to all vertices passed to the renderer
	// after these calls.
	void SetModelTransform(const Lepra::TransformationF& pTransform);

	void SetTextureTransform(const Lepra::TransformationF& pTransform);

	// When setting the dest canvas the viewport will be reset to cover
	// the entire canvas.
	void SetDestCanvas(Lepra::Canvas* pScreen, bool pReset = true);
	inline Lepra::Canvas* GetDestCanvas() const;
	void SetClearColor(const Lepra::Color& pColor);
	void SetTexture(Texture* pTexture);

	inline void SetAlphaSource(AlphaSource pAlphaSource);

	void SetSpecularity(float pSpecularity);
	inline void SetAlpha(unsigned char pAlpha);

	// Set the subdivision length used with the RENDER_FAST hint.
	// This is the spanwidth where uv-coordinates are linearly interpolated over
	// the polygon.
	// 2^AffineExp gives...
	// AffineExp = 0 -> 1
	// AffineExp = 1 -> 2
	// AffineExp = 2 -> 4
	// AffineExp = 3 -> 8
	// AffineExp = 4 -> 16
	// etc...
	void SetAffineLengthExponent(unsigned int pAffineExp);

	// The reference factor is the length of one side of the polygon, in world coordinates,
	// divided by the width of the texture map.
	// 
	// For instance, if a texturemap is 128x128 pixels, and it should be
	// mapped on a 64x64 sized square polygon, the factor is 64/128 = 0.5.
	void SetMipMapReferenceFactor(float pReferenceFactor);

	// Returns D in the projection formula px = x * D / z.
	inline float GetProjectionDistance();
	bool GetScreenCoordinates(float pX, float pY, float pZ, int& pScreenX, int& pScreenY);

	// 0 <= pLightIndex < MAX_LIGHTS
	void SetLight(int pLightIndex,
		      LightType pType,
		      float pX, float pY, float pZ,
		      float pRed, float pGreen, float pBlue);
	void SetLightPosition(int pLightIndex, float pX, float pY, float pZ);
	void SetLightDirection(int pLightIndex, float pX, float pY, float pZ);
	void GetLightPosition(int pLightIndex, float& pX, float& pY, float& pZ);
	void GetLightDirection(int pLightIndex, float& pX, float& pY, float& pZ);
	inline void SetAmbientLight(float pRed, float pGreen, float pBlue);

	void GetLightColor(int pLightIndex, float& pR, float& pG, float& pB);
	inline LightType GetLightType(int pLightIndex);

	inline void SetLightModel(LightModel pLightModel);

	inline int GetMaxLights();
	inline int GetNumLights();

	inline void SetPixelShaderCallBack(void (*CallBack)(unsigned char& pRed, unsigned char& pGreen, unsigned char& pBlue, Software3DPainter* pPainter));

	void PostRenderFocalBlur(float pSharpNearZ, float pSharpFarZ);

	inline Lepra::Canvas* GetZBuffer();
private:

	enum ClipFlags
	{
		CLIP_UV  = (1 << 0),
		CLIP_RGB = (1 << 1),
	};

	enum Flag
	{
		FLAG_ZTEST_ACTIVE		= (1 << 0),
		FLAG_ZWRITE_ACTIVE		= (1 << 1),
		FLAG_MIPMAPPING_ACTIVE		= (1 << 2),
		FLAG_BILINEAR_ACTIVE		= (1 << 3),
		FLAG_TRILINEAR_ACTIVE		= (1 << 4),
		FLAG_ALPHATEST_ACTIVE		= (1 << 5),
		FLAG_ALPHABLEND_ACTIVE		= (1 << 6),
		FLAG_VERTEXCOLORS_ACTIVE	= (1 << 7),
		FLAG_TEXTUREMAPPING_ACTIVE	= (1 << 8),
		FLAG_LIGHTS_ACTIVE		= (1 << 9),
	};

	struct DataXYZ
	{
		float x;
		float y;
		float z;
		float mTranslatedX;
		float mTranslatedY;
		float mTranslatedZ;
	};

	struct DataRGB
	{
		float r;
		float g;
		float b;
		float mLightProcessedR;
		float mLightProcessedG;
		float mLightProcessedB;
	};

	struct DataUVM
	{
		float u;
		float v;
		float m;
	};

	struct PointData
	{
		DataXYZ* mXYZ;
		DataUVM* mUVM;
		DataRGB* mRGB;
		DataXYZ* mNormal;
	};

	struct LightData
	{
		LightType mType;
		DataXYZ mVector; // Direction or position, depending on type.
		// The color components are float because they can't have an upper 
		// limit like 255, since a lightsource can be of unlimited strength. 
		// (Compare the sun with a candle).
		float mRed;
		float mGreen;
		float mBlue;

		int mIndex;
	};

	// Called everytime Begin() is called.
	void CheckStateChanges();

	// Functions to update the function pointers.
	void UpdateRenderPrimitiveFunc();
	void UpdateScanLineFuncs();
	void UpdateTextureReaderFuncs();
	void UpdatePixelWriterFuncs();
	void UpdateDrawLineFuncs();

	// Used to get the correct mip map level.
	typedef float (GetUFunc)(Vertex3D* pV);
	typedef float (GetVFunc)(Vertex3D* pV);
	static float GetU_UV(Vertex3D* pV);
	static float GetV_UV(Vertex3D* pV);
	static float GetU_UVM(Vertex3D* pV);
	static float GetV_UVM(Vertex3D* pV);
	static float GetU_UVRGB(Vertex3D* pV);
	static float GetV_UVRGB(Vertex3D* pV);
	static float GetU_UVRGBM(Vertex3D* pV);
	static float GetV_UVRGBM(Vertex3D* pV);
	int GetFastMipMapLevel(Vertex3D** pVertex, int pNumVertex, GetUFunc pGetUFunc, GetVFunc pGetVFunc);
	float GetNicestMipMapLevel(float pZ);

	// Used when rendering shaded triangles...
	void ProcessLights();
	float FlatModeDotProduct(DataXYZ* pV);

	void CheckVertexData();

	inline void PrepareNextTriangleStripPointDataXYZ();
	inline void PrepareNextTriangleStripPointDataUVM();
	inline void PrepareNextTriangleStripPointDataRGB();
	inline void PrepareNextTriangleStripPointDataNormal();

	inline void PrepareNextTriangleFanPointDataXYZ();
	inline void PrepareNextTriangleFanPointDataUVM();
	inline void PrepareNextTriangleFanPointDataRGB();
	inline void PrepareNextTriangleFanPointDataNormal();

	// Clipping...
	
	// The ClipLine functions return the number of points left after clipping.
	// That'll be 0 or 2. These functions will clip a line against the screen.
	int ClipLine(const PointData* pInputPoint1,
		     const PointData* pInputPoint2,
		     PointData* pOutputPoint1,
		     PointData* pOutputPoint2,
		     ClipFlags pFlags);

	int ClipPolygon(const PointData* pInputPoint, int pNumPoints,
			PointData* pOutputPoint,
			ClipFlags pFlags,
			float pPlaneA, float pPlaneB, float pPlaneC, float pPlaneD,
			bool pClosedLoop);

	int ClipLineLeftX(const PointData* pInputPoint1,
			  const PointData* pInputPoint2,
			  PointData* pOutputPoint1,
			  PointData* pOutputPoint2,
			  ClipFlags pFlags);

	int ClipLineRightX(const PointData* pInputPoint1,
			   const PointData* pInputPoint2,
			   PointData* pOutputPoint1,
			   PointData* pOutputPoint2,
			   ClipFlags pFlags);

	int ClipLineTopY(const PointData* pInputPoint1,
			 const PointData* pInputPoint2,
			 PointData* pOutputPoint1,
			 PointData* pOutputPoint2,
			 ClipFlags pFlags);

	int ClipLineBottomY(const PointData* pInputPoint1,
			    const PointData* pInputPoint2,
			    PointData* pOutputPoint1,
			    PointData* pOutputPoint2,
			    ClipFlags pFlags);

	// The ClipVector functions are used when clipping lines.
	// They return one of the following values:
	//
	// 0 - No point was clipped.
	// 1 - Point 1 was clipped.
	// 2 - Point 2 was clipped.
	// 3 - Both points where clipped.

	static int ClipVectorNearZ(const PointData* pInputPoint1,
				   const PointData* pInputPoint2,
				   ClipFlags pFlags,
				   Software3DPainter* pThis);

	static int ClipVectorFarZ(const PointData* pInputPoint1,
				  const PointData* pInputPoint2,
				  ClipFlags pFlags,
				  Software3DPainter* pThis);

	// The same return values goes for these.
	typedef void (SetVertexDataFunc)(Vertex3D* pV, const PointData* pP, float z);
	static void SetVertex3DData(Vertex3D* pV, const PointData* pP, float z);
	static void SetVertex3DUVData(Vertex3D* pV, const PointData* pP, float z);
	static void SetVertex3DRGBData(Vertex3D* pV, const PointData* pP, float z);
	static void SetVertex3DUVRGBData(Vertex3D* pV, const PointData* pP, float z);
	static void SetVertex3DUVMData(Vertex3D* pV, const PointData* pP, float z);
	static void SetVertex3DUVRGBMData(Vertex3D* pV, const PointData* pP, float z);

	static int ClipAndProjectTriangle(Vertex3D* pV[8], Software3DPainter* pThis, ClipFlags pClipFlags, SetVertexDataFunc pFunc);

	static bool CheckVisible(Vertex3D* pV[8], int pNumVertices, Software3DPainter* pThis);

	// Primitive rendering funcs.
	void (*RenderPrimitive)(Software3DPainter* pThis);

	static void RenderLinesSingleColor(Software3DPainter* pThis);
	static void RenderLinesRGB(Software3DPainter* pThis);
	static void RenderLinesUV(Software3DPainter* pThis);
	static void RenderLinesUVRGB(Software3DPainter* pThis);

	static void RenderLineStripSingleColor(Software3DPainter* pThis);
	static void RenderLineStripRGB(Software3DPainter* pThis);
	static void RenderLineStripUV(Software3DPainter* pThis);
	static void RenderLineStripUVRGB(Software3DPainter* pThis);

	static void RenderTrianglesSingleColor(Software3DPainter* pThis);
	static void RenderTrianglesRGB(Software3DPainter* pThis);
	static void RenderTrianglesUV(Software3DPainter* pThis);
	static void RenderTrianglesUVRGB(Software3DPainter* pThis);
	static void RenderTrianglesUVM(Software3DPainter* pThis);
	static void RenderTrianglesUVRGBM(Software3DPainter* pThis);

	static void RenderTriangleStripSingleColor(Software3DPainter* pThis);
	static void RenderTriangleStripRGB(Software3DPainter* pThis);
	static void RenderTriangleStripUV(Software3DPainter* pThis);
	static void RenderTriangleStripUVRGB(Software3DPainter* pThis);
	static void RenderTriangleStripUVM(Software3DPainter* pThis);
	static void RenderTriangleStripUVRGBM(Software3DPainter* pThis);

	static void RenderTriangleFanSingleColor(Software3DPainter* pThis);
	static void RenderTriangleFanRGB(Software3DPainter* pThis);
	static void RenderTriangleFanUV(Software3DPainter* pThis);
	static void RenderTriangleFanUVRGB(Software3DPainter* pThis);
	static void RenderTriangleFanUVM(Software3DPainter* pThis);
	static void RenderTriangleFanUVRGBM(Software3DPainter* pThis);

	// The triangle rendering funcs.
	void DrawSingleColorTriangle(EdgeZ* pTopToBottomEdge,
								 EdgeZ* pTopToMiddleEdge,
								 EdgeZ* pMiddleToBottomEdge,
								 Gradients* pGradients,
								 bool pMiddleIsLeft,
								 const Lepra::Color& pColor);
	void DrawRGBTriangle(EdgeRGB* pTopToBottomEdge,
						 EdgeRGB* pTopToMiddleEdge,
						 EdgeRGB* pMiddleToBottomEdge,
						 GradientsRGB* pGradients,
						 bool pMiddleIsLeft);
	void DrawUVTriangle(EdgeUV* pTopToBottomEdge,
						EdgeUV* pTopToMiddleEdge,
						EdgeUV* pMiddleToBottomEdge,
						GradientsUV* pGradients,
						bool pMiddleIsLeft);
	void DrawUVRGBTriangle(EdgeUVRGB* pTopToBottomEdge,
						   EdgeUVRGB* pTopToMiddleEdge,
						   EdgeUVRGB* pMiddleToBottomEdge,
						   GradientsUVRGB* pGradients,
						   bool pMiddleIsLeft);
	void DrawUVMTriangle(EdgeUVM* pTopToBottomEdge,
						 EdgeUVM* pTopToMiddleEdge,
						 EdgeUVM* pMiddleToBottomEdge,
						 GradientsUVM* pGradients,
						 bool pMiddleIsLeft);
	void DrawUVRGBMTriangle(EdgeUVRGBM* pTopToBottomEdge,
							EdgeUVRGBM* pTopToMiddleEdge,
							EdgeUVRGBM* pMiddleToBottomEdge,
							GradientsUVRGBM* pGradients,
							bool pMiddleIsLeft);
	void DrawUVTriangleFast(EdgeUV* pTopToBottomEdge,
							EdgeUV* pTopToMiddleEdge,
							EdgeUV* pMiddleToBottomEdge,
							GradientsUV* pGradients,
							bool pMiddleIsLeft,
							int pMipMapLevel);
	void DrawUVRGBTriangleFast(EdgeUVRGB* pTopToBottomEdge,
							   EdgeUVRGB* pTopToMiddleEdge,
							   EdgeUVRGB* pMiddleToBottomEdge,
							   GradientsUVRGB* pGradients,
							   bool pMiddleIsLeft,
							   int pMipMapLevel);

	//
	// Draw line functions... Damn how many!
	//

	// Function pointers.
	void (*DrawSingleColorLineFunc)(const Vertex3D* pVertices, const Lepra::Color& pColor, Software3DPainter* pThis);
	void (*DrawRGBLineFunc)(const Vertex3DRGB* pVertices, Software3DPainter* pThis);
	void (*DrawUVLineFunc)(const Vertex3DUV* pVertices, Software3DPainter* pThis);
	void (*DrawUVRGBLineFunc)(const Vertex3DUVRGB* pVertices, Software3DPainter* pThis);
	void (*DrawUVMLineFunc)(const Vertex3DUVM* pVertices, Software3DPainter* pThis);
	void (*DrawUVRGBMLineFunc)(const Vertex3DUVRGBM* pVertices, Software3DPainter* pThis);
	void (*DrawUVLineFuncFast)(const Vertex3DUV* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis);
	void (*DrawUVRGBLineFuncFast)(const Vertex3DUVRGB* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis);

	// First the "nice" ones...
	static void DrawSingleColorLine8BitNicest(const Vertex3D* pVertices, const Lepra::Color& pColor, Software3DPainter* pThis);
	static void DrawSingleColorLineNicest(const Vertex3D* pVertices, const Lepra::Color& pColor, Software3DPainter* pThis);
	static void DrawRGBLine8BitNicest(const Vertex3DRGB* pVertices, Software3DPainter* pThis);
	static void DrawRGBLineNicest(const Vertex3DRGB* pVertices, Software3DPainter* pThis);
	static void DrawUVLine8BitNicest(const Vertex3DUV* pVertices, Software3DPainter* pThis);
	static void DrawUVLineNicest(const Vertex3DUV* pVertices, Software3DPainter* pThis);
	static void DrawUVRGBLine8BitNicest(const Vertex3DUVRGB* pVertices, Software3DPainter* pThis);
	static void DrawUVRGBLineNicest(const Vertex3DUVRGB* pVertices, Software3DPainter* pThis);

	static void DrawUVMLine8BitNicest(const Vertex3DUVM* pVertices, Software3DPainter* pThis);
	static void DrawUVMLineNicest(const Vertex3DUVM* pVertices, Software3DPainter* pThis);
	static void DrawUVRGBMLine8BitNicest(const Vertex3DUVRGBM* pVertices, Software3DPainter* pThis);
	static void DrawUVRGBMLineNicest(const Vertex3DUVRGBM* pVertices, Software3DPainter* pThis);

	// Single color
	static void DrawSingleColorLine8BitFast(const Vertex3D* pVertices, const Lepra::Color& pColor, Software3DPainter* pThis);
	static void DrawSingleColorLine15BitFast(const Vertex3D* pVertices, const Lepra::Color& pColor, Software3DPainter* pThis);
	static void DrawSingleColorLine16BitFast(const Vertex3D* pVertices, const Lepra::Color& pColor, Software3DPainter* pThis);
	static void DrawSingleColorLine32BitFast(const Vertex3D* pVertices, const Lepra::Color& pColor, Software3DPainter* pThis);
	static void DrawSingleColorLine8BitFastZT(const Vertex3D* pVertices, const Lepra::Color& pColor, Software3DPainter* pThis);
	static void DrawSingleColorLine15BitFastZT(const Vertex3D* pVertices, const Lepra::Color& pColor, Software3DPainter* pThis);
	static void DrawSingleColorLine16BitFastZT(const Vertex3D* pVertices, const Lepra::Color& pColor, Software3DPainter* pThis);
	static void DrawSingleColorLine32BitFastZT(const Vertex3D* pVertices, const Lepra::Color& pColor, Software3DPainter* pThis);
	static void DrawSingleColorLine8BitFastZW(const Vertex3D* pVertices, const Lepra::Color& pColor, Software3DPainter* pThis);
	static void DrawSingleColorLine15BitFastZW(const Vertex3D* pVertices, const Lepra::Color& pColor, Software3DPainter* pThis);
	static void DrawSingleColorLine16BitFastZW(const Vertex3D* pVertices, const Lepra::Color& pColor, Software3DPainter* pThis);
	static void DrawSingleColorLine32BitFastZW(const Vertex3D* pVertices, const Lepra::Color& pColor, Software3DPainter* pThis);
	static void DrawSingleColorLine8BitFastZTZW(const Vertex3D* pVertices, const Lepra::Color& pColor, Software3DPainter* pThis);
	static void DrawSingleColorLine15BitFastZTZW(const Vertex3D* pVertices, const Lepra::Color& pColor, Software3DPainter* pThis);
	static void DrawSingleColorLine16BitFastZTZW(const Vertex3D* pVertices, const Lepra::Color& pColor, Software3DPainter* pThis);
	static void DrawSingleColorLine32BitFastZTZW(const Vertex3D* pVertices, const Lepra::Color& pColor, Software3DPainter* pThis);

	// Shaded
	static void DrawRGBLine8BitFast(const Vertex3DRGB* pVertices, Software3DPainter* pThis);
	static void DrawRGBLine15BitFast(const Vertex3DRGB* pVertices, Software3DPainter* pThis);
	static void DrawRGBLine16BitFast(const Vertex3DRGB* pVertices, Software3DPainter* pThis);
	static void DrawRGBLine32BitFast(const Vertex3DRGB* pVertices, Software3DPainter* pThis);
	static void DrawRGBLine8BitFastZT(const Vertex3DRGB* pVertices, Software3DPainter* pThis);
	static void DrawRGBLine15BitFastZT(const Vertex3DRGB* pVertices, Software3DPainter* pThis);
	static void DrawRGBLine16BitFastZT(const Vertex3DRGB* pVertices, Software3DPainter* pThis);
	static void DrawRGBLine32BitFastZT(const Vertex3DRGB* pVertices, Software3DPainter* pThis);
	static void DrawRGBLine8BitFastZW(const Vertex3DRGB* pVertices, Software3DPainter* pThis);
	static void DrawRGBLine15BitFastZW(const Vertex3DRGB* pVertices, Software3DPainter* pThis);
	static void DrawRGBLine16BitFastZW(const Vertex3DRGB* pVertices, Software3DPainter* pThis);
	static void DrawRGBLine32BitFastZW(const Vertex3DRGB* pVertices, Software3DPainter* pThis);
	static void DrawRGBLine8BitFastZTZW(const Vertex3DRGB* pVertices, Software3DPainter* pThis);
	static void DrawRGBLine15BitFastZTZW(const Vertex3DRGB* pVertices, Software3DPainter* pThis);
	static void DrawRGBLine16BitFastZTZW(const Vertex3DRGB* pVertices, Software3DPainter* pThis);
	static void DrawRGBLine32BitFastZTZW(const Vertex3DRGB* pVertices, Software3DPainter* pThis);

	// Textured
	static void DrawUVLine8BitFast(const Vertex3DUV* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis);
	static void DrawUVLine16BitFast(const Vertex3DUV* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis);
	static void DrawUVLine32BitFast(const Vertex3DUV* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis);
	static void DrawUVLine8BitFastZT(const Vertex3DUV* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis);
	static void DrawUVLine16BitFastZT(const Vertex3DUV* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis);
	static void DrawUVLine32BitFastZT(const Vertex3DUV* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis);
	static void DrawUVLine8BitFastZW(const Vertex3DUV* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis);
	static void DrawUVLine16BitFastZW(const Vertex3DUV* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis);
	static void DrawUVLine32BitFastZW(const Vertex3DUV* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis);
	static void DrawUVLine8BitFastZTZW(const Vertex3DUV* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis);
	static void DrawUVLine16BitFastZTZW(const Vertex3DUV* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis);
	static void DrawUVLine32BitFastZTZW(const Vertex3DUV* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis);

	// Textured and shaded
	static void DrawUVRGBLine8BitFast(const Vertex3DUVRGB* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis);
	static void DrawUVRGBLine15BitFast(const Vertex3DUVRGB* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis);
	static void DrawUVRGBLine16BitFast(const Vertex3DUVRGB* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis);
	static void DrawUVRGBLine32BitFast(const Vertex3DUVRGB* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis);
	static void DrawUVRGBLine8BitFastZT(const Vertex3DUVRGB* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis);
	static void DrawUVRGBLine15BitFastZT(const Vertex3DUVRGB* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis);
	static void DrawUVRGBLine16BitFastZT(const Vertex3DUVRGB* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis);
	static void DrawUVRGBLine32BitFastZT(const Vertex3DUVRGB* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis);
	static void DrawUVRGBLine8BitFastZW(const Vertex3DUVRGB* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis);
	static void DrawUVRGBLine15BitFastZW(const Vertex3DUVRGB* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis);
	static void DrawUVRGBLine16BitFastZW(const Vertex3DUVRGB* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis);
	static void DrawUVRGBLine32BitFastZW(const Vertex3DUVRGB* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis);
	static void DrawUVRGBLine8BitFastZTZW(const Vertex3DUVRGB* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis);
	static void DrawUVRGBLine15BitFastZTZW(const Vertex3DUVRGB* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis);
	static void DrawUVRGBLine16BitFastZTZW(const Vertex3DUVRGB* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis);
	static void DrawUVRGBLine32BitFastZTZW(const Vertex3DUVRGB* pVertices, const Lepra::Canvas* pTexture, Software3DPainter* pThis);


	//
	// Scanline functions... Just as many!
	//

	// Function pointers.
	void (*DrawSingleColorScanLineFunc)(const Gradients* pGradients,
										EdgeZ* pLeft, 
										EdgeZ* pRight,
										const Lepra::Color& pColor,
										Software3DPainter* pThis);
	void (*DrawUVScanLineFunc)(const GradientsUV* pGradients, 
									 EdgeUV* pLeft, 
									 EdgeUV* pRight,
									 Software3DPainter* pThis);
	void (*DrawRGBScanLineFunc)(const GradientsRGB* pGradients, 
									  EdgeRGB* pLeft, 
									  EdgeRGB* pRight,
									  Software3DPainter* pThis);
	void (*DrawUVRGBScanLineFunc)(const GradientsUVRGB* pGradients, 
										EdgeUVRGB* pLeft, 
										EdgeUVRGB* pRight,
										Software3DPainter* pThis);
	void (*DrawUVMScanLineFunc)(const GradientsUVM* pGradients, 
									  EdgeUVM* pLeft, 
									  EdgeUVM* pRight,
									  Software3DPainter* pThis);
	void (*DrawUVRGBMScanLineFunc)(const GradientsUVRGBM* pGradients, 
										 EdgeUVRGBM* pLeft, 
										 EdgeUVRGBM* pRight,
										 Software3DPainter* pThis);
	void (*DrawUVScanLineFuncFast)(const GradientsUV* pGradients, 
										 EdgeUV* pLeft, 
										 EdgeUV* pRight,
										 const Lepra::Canvas* pTexture,
										 Software3DPainter* pThis);
	void (*DrawUVRGBScanLineFuncFast)(const GradientsUVRGB* pGradients, 
											EdgeUVRGB* pLeft, 
											EdgeUVRGB* pRight,
											const Lepra::Canvas* pTexture,
											Software3DPainter* pThis);

	// First the "nice" ones.
	static void DrawScanLineSingleColor8BitNicest(const Gradients* pGradients,
												  EdgeZ* pLeft, 
												  EdgeZ* pRight,
												  const Lepra::Color& pColor,
												  Software3DPainter* pThis);
	static void DrawScanLineSingleColorNicest(const Gradients* pGradients,
											  EdgeZ* pLeft, 
											  EdgeZ* pRight,
											  const Lepra::Color& pColor,
											  Software3DPainter* pThis);
	static void DrawScanLineRGB8BitNicest(const GradientsRGB* pGradients, 
									  EdgeRGB* pLeft, 
									  EdgeRGB* pRight,
									  Software3DPainter* pThis);
	static void DrawScanLineRGBNicest(const GradientsRGB* pGradients, 
									  EdgeRGB* pLeft, 
									  EdgeRGB* pRight,
									  Software3DPainter* pThis);
	static void DrawScanLineUV8BitNicest(const GradientsUV* pGradients, 
										 EdgeUV* pLeft, 
										 EdgeUV* pRight,
										 Software3DPainter* pThis);
	static void DrawScanLineUVNicest(const GradientsUV* pGradients, 
									 EdgeUV* pLeft, 
									 EdgeUV* pRight,
									 Software3DPainter* pThis);
	static void DrawScanLineUVRGB8BitNicest(const GradientsUVRGB* pGradients, 
									  EdgeUVRGB* pLeft, 
									  EdgeUVRGB* pRight,
									  Software3DPainter* pThis);
	static void DrawScanLineUVRGBNicest(const GradientsUVRGB* pGradients, 
										EdgeUVRGB* pLeft, 
										EdgeUVRGB* pRight,
										Software3DPainter* pThis);
	static void DrawScanLineUVM8BitNicest(const GradientsUVM* pGradients, 
										  EdgeUVM* pLeft, 
										  EdgeUVM* pRight,
										  Software3DPainter* pThis);
	static void DrawScanLineUVMNicest(const GradientsUVM* pGradients, 
									  EdgeUVM* pLeft, 
									  EdgeUVM* pRight,
									  Software3DPainter* pThis);
	static void DrawScanLineUVRGBM8BitNicest(const GradientsUVRGBM* pGradients, 
										  EdgeUVRGBM* pLeft, 
										  EdgeUVRGBM* pRight,
										  Software3DPainter* pThis);
	static void DrawScanLineUVRGBMNicest(const GradientsUVRGBM* pGradients, 
										 EdgeUVRGBM* pLeft, 
										 EdgeUVRGBM* pRight,
										 Software3DPainter* pThis);


	// And now the fast ones. The large amount of functions is needed
	// because every function is specialized.
	// There are 5 bit depths to support, 4 combinations of z-read and z-write,
	// and 4 kind of functions (single color, shaded, textured, shaded and textured),
	// which will make it 5*4*4 = 80 functions.

	// Single color, no z-test, no z-write.
	static void DrawScanLineSingleColor8BitFast(const Gradients* pGradients,
												EdgeZ* pLeft, 
												EdgeZ* pRight,
												const Lepra::Color& pColor,
												Software3DPainter* pThis);
	static void DrawScanLineSingleColor15BitFast(const Gradients* pGradients,
												 EdgeZ* pLeft, 
												 EdgeZ* pRight,
												 const Lepra::Color& pColor,
												 Software3DPainter* pThis);
	static void DrawScanLineSingleColor16BitFast(const Gradients* pGradients,
												 EdgeZ* pLeft, 
												 EdgeZ* pRight,
												 const Lepra::Color& pColor,
												 Software3DPainter* pThis);
	static void DrawScanLineSingleColor24BitFast(const Gradients* pGradients,
												 EdgeZ* pLeft, 
												 EdgeZ* pRight,
												 const Lepra::Color& pColor,
												 Software3DPainter* pThis);
	static void DrawScanLineSingleColor32BitFast(const Gradients* pGradients,
												 EdgeZ* pLeft, 
												 EdgeZ* pRight,
												 const Lepra::Color& pColor,
												 Software3DPainter* pThis);


	// Single color, z-test, no z-write.
	static void DrawScanLineSingleColor8BitFastZT(const Gradients* pGradients,
												EdgeZ* pLeft, 
												EdgeZ* pRight,
												const Lepra::Color& pColor,
												Software3DPainter* pThis);
	static void DrawScanLineSingleColor15BitFastZT(const Gradients* pGradients,
												 EdgeZ* pLeft, 
												 EdgeZ* pRight,
												 const Lepra::Color& pColor,
												 Software3DPainter* pThis);
	static void DrawScanLineSingleColor16BitFastZT(const Gradients* pGradients,
												 EdgeZ* pLeft, 
												 EdgeZ* pRight,
												 const Lepra::Color& pColor,
												 Software3DPainter* pThis);
	static void DrawScanLineSingleColor24BitFastZT(const Gradients* pGradients,
												 EdgeZ* pLeft, 
												 EdgeZ* pRight,
												 const Lepra::Color& pColor,
												 Software3DPainter* pThis);
	static void DrawScanLineSingleColor32BitFastZT(const Gradients* pGradients,
												 EdgeZ* pLeft, 
												 EdgeZ* pRight,
												 const Lepra::Color& pColor,
												 Software3DPainter* pThis);

	// Single color, no z-test, z-write.
	static void DrawScanLineSingleColor8BitFastZW(const Gradients* pGradients,
												EdgeZ* pLeft, 
												EdgeZ* pRight,
												const Lepra::Color& pColor,
												Software3DPainter* pThis);
	static void DrawScanLineSingleColor15BitFastZW(const Gradients* pGradients,
												 EdgeZ* pLeft, 
												 EdgeZ* pRight,
												 const Lepra::Color& pColor,
												 Software3DPainter* pThis);
	static void DrawScanLineSingleColor16BitFastZW(const Gradients* pGradients,
												 EdgeZ* pLeft, 
												 EdgeZ* pRight,
												 const Lepra::Color& pColor,
												 Software3DPainter* pThis);
	static void DrawScanLineSingleColor24BitFastZW(const Gradients* pGradients,
												 EdgeZ* pLeft, 
												 EdgeZ* pRight,
												 const Lepra::Color& pColor,
												 Software3DPainter* pThis);
	static void DrawScanLineSingleColor32BitFastZW(const Gradients* pGradients,
												 EdgeZ* pLeft, 
												 EdgeZ* pRight,
												 const Lepra::Color& pColor,
												 Software3DPainter* pThis);

	// Single color, z-test, z-write.
	static void DrawScanLineSingleColor8BitFastZTZW(const Gradients* pGradients,
												EdgeZ* pLeft, 
												EdgeZ* pRight,
												const Lepra::Color& pColor,
												Software3DPainter* pThis);
	static void DrawScanLineSingleColor15BitFastZTZW(const Gradients* pGradients,
												 EdgeZ* pLeft, 
												 EdgeZ* pRight,
												 const Lepra::Color& pColor,
												 Software3DPainter* pThis);
	static void DrawScanLineSingleColor16BitFastZTZW(const Gradients* pGradients,
												 EdgeZ* pLeft, 
												 EdgeZ* pRight,
												 const Lepra::Color& pColor,
												 Software3DPainter* pThis);
	static void DrawScanLineSingleColor24BitFastZTZW(const Gradients* pGradients,
												 EdgeZ* pLeft, 
												 EdgeZ* pRight,
												 const Lepra::Color& pColor,
												 Software3DPainter* pThis);
	static void DrawScanLineSingleColor32BitFastZTZW(const Gradients* pGradients,
												 EdgeZ* pLeft, 
												 EdgeZ* pRight,
												 const Lepra::Color& pColor,
												 Software3DPainter* pThis);


	// Shaded. no z-test, no z-write
	static void DrawScanLineRGB8BitFast(const GradientsRGB* pGradients, 
									  EdgeRGB* pLeft, 
									  EdgeRGB* pRight,
									  Software3DPainter* pThis);
	static void DrawScanLineRGB15BitFast(const GradientsRGB* pGradients, 
									   EdgeRGB* pLeft, 
									   EdgeRGB* pRight,
									   Software3DPainter* pThis);
	static void DrawScanLineRGB16BitFast(const GradientsRGB* pGradients, 
									   EdgeRGB* pLeft, 
									   EdgeRGB* pRight,
									   Software3DPainter* pThis);
	static void DrawScanLineRGB24BitFast(const GradientsRGB* pGradients, 
									   EdgeRGB* pLeft, 
									   EdgeRGB* pRight,
									   Software3DPainter* pThis);
	static void DrawScanLineRGB32BitFast(const GradientsRGB* pGradients, 
									   EdgeRGB* pLeft, 
									   EdgeRGB* pRight,
									   Software3DPainter* pThis);

	// Shaded. z-test, no z-write
	static void DrawScanLineRGB8BitFastZT(const GradientsRGB* pGradients, 
									  EdgeRGB* pLeft, 
									  EdgeRGB* pRight,
									  Software3DPainter* pThis);
	static void DrawScanLineRGB15BitFastZT(const GradientsRGB* pGradients, 
									   EdgeRGB* pLeft, 
									   EdgeRGB* pRight,
									   Software3DPainter* pThis);
	static void DrawScanLineRGB16BitFastZT(const GradientsRGB* pGradients, 
									   EdgeRGB* pLeft, 
									   EdgeRGB* pRight,
									   Software3DPainter* pThis);
	static void DrawScanLineRGB24BitFastZT(const GradientsRGB* pGradients, 
									   EdgeRGB* pLeft, 
									   EdgeRGB* pRight,
									   Software3DPainter* pThis);
	static void DrawScanLineRGB32BitFastZT(const GradientsRGB* pGradients, 
									   EdgeRGB* pLeft, 
									   EdgeRGB* pRight,
									   Software3DPainter* pThis);

	// Shaded. no z-test, z-write
	static void DrawScanLineRGB8BitFastZW(const GradientsRGB* pGradients, 
									  EdgeRGB* pLeft, 
									  EdgeRGB* pRight,
									  Software3DPainter* pThis);
	static void DrawScanLineRGB15BitFastZW(const GradientsRGB* pGradients, 
									   EdgeRGB* pLeft, 
									   EdgeRGB* pRight,
									   Software3DPainter* pThis);
	static void DrawScanLineRGB16BitFastZW(const GradientsRGB* pGradients, 
									   EdgeRGB* pLeft, 
									   EdgeRGB* pRight,
									   Software3DPainter* pThis);
	static void DrawScanLineRGB24BitFastZW(const GradientsRGB* pGradients, 
									   EdgeRGB* pLeft, 
									   EdgeRGB* pRight,
									   Software3DPainter* pThis);
	static void DrawScanLineRGB32BitFastZW(const GradientsRGB* pGradients, 
									   EdgeRGB* pLeft, 
									   EdgeRGB* pRight,
									   Software3DPainter* pThis);


	// Shaded. z-test, z-write
	static void DrawScanLineRGB8BitFastZTZW(const GradientsRGB* pGradients, 
									  EdgeRGB* pLeft, 
									  EdgeRGB* pRight,
									  Software3DPainter* pThis);
	static void DrawScanLineRGB15BitFastZTZW(const GradientsRGB* pGradients, 
									   EdgeRGB* pLeft, 
									   EdgeRGB* pRight,
									   Software3DPainter* pThis);
	static void DrawScanLineRGB16BitFastZTZW(const GradientsRGB* pGradients, 
									   EdgeRGB* pLeft, 
									   EdgeRGB* pRight,
									   Software3DPainter* pThis);
	static void DrawScanLineRGB24BitFastZTZW(const GradientsRGB* pGradients, 
									   EdgeRGB* pLeft, 
									   EdgeRGB* pRight,
									   Software3DPainter* pThis);
	static void DrawScanLineRGB32BitFastZTZW(const GradientsRGB* pGradients, 
									   EdgeRGB* pLeft, 
									   EdgeRGB* pRight,
									   Software3DPainter* pThis);

	// Texturemapped. no z-test, no z-write
	static void DrawScanLineUV8BitFast(const GradientsUV* pGradients, 
										 EdgeUV* pLeft, 
										 EdgeUV* pRight,
										 const Lepra::Canvas* pTexture,
										 Software3DPainter* pThis);
	static void DrawScanLineUV16BitFast(const GradientsUV* pGradients, 
										  EdgeUV* pLeft, 
										  EdgeUV* pRight,
										  const Lepra::Canvas* pTexture,
										  Software3DPainter* pThis);
	static void DrawScanLineUV24BitFast(const GradientsUV* pGradients, 
										  EdgeUV* pLeft, 
										  EdgeUV* pRight,
										  const Lepra::Canvas* pTexture,
										  Software3DPainter* pThis);
	static void DrawScanLineUV32BitFast(const GradientsUV* pGradients, 
										  EdgeUV* pLeft, 
										  EdgeUV* pRight,
										  const Lepra::Canvas* pTexture,
										  Software3DPainter* pThis);

	// Texturemapped. z-test, no z-write
	static void DrawScanLineUV8BitFastZT(const GradientsUV* pGradients, 
										 EdgeUV* pLeft, 
										 EdgeUV* pRight,
										 const Lepra::Canvas* pTexture,
										 Software3DPainter* pThis);
	static void DrawScanLineUV16BitFastZT(const GradientsUV* pGradients, 
										  EdgeUV* pLeft, 
										  EdgeUV* pRight,
										  const Lepra::Canvas* pTexture,
										  Software3DPainter* pThis);
	static void DrawScanLineUV24BitFastZT(const GradientsUV* pGradients, 
										  EdgeUV* pLeft, 
										  EdgeUV* pRight,
										  const Lepra::Canvas* pTexture,
										  Software3DPainter* pThis);
	static void DrawScanLineUV32BitFastZT(const GradientsUV* pGradients, 
										  EdgeUV* pLeft, 
										  EdgeUV* pRight,
										  const Lepra::Canvas* pTexture,
										  Software3DPainter* pThis);

	// Texturemapped. no z-test, z-write
	static void DrawScanLineUV8BitFastZW(const GradientsUV* pGradients, 
										 EdgeUV* pLeft, 
										 EdgeUV* pRight,
										 const Lepra::Canvas* pTexture,
										 Software3DPainter* pThis);
	static void DrawScanLineUV16BitFastZW(const GradientsUV* pGradients, 
										  EdgeUV* pLeft, 
										  EdgeUV* pRight,
										  const Lepra::Canvas* pTexture,
										  Software3DPainter* pThis);
	static void DrawScanLineUV24BitFastZW(const GradientsUV* pGradients, 
										  EdgeUV* pLeft, 
										  EdgeUV* pRight,
										  const Lepra::Canvas* pTexture,
										  Software3DPainter* pThis);
	static void DrawScanLineUV32BitFastZW(const GradientsUV* pGradients, 
										  EdgeUV* pLeft, 
										  EdgeUV* pRight,
										  const Lepra::Canvas* pTexture,
										  Software3DPainter* pThis);

	// Texturemapped. z-test, z-write
	static void DrawScanLineUV8BitFastZTZW(const GradientsUV* pGradients, 
										 EdgeUV* pLeft, 
										 EdgeUV* pRight,
										 const Lepra::Canvas* pTexture,
										 Software3DPainter* pThis);
	static void DrawScanLineUV16BitFastZTZW(const GradientsUV* pGradients, 
										  EdgeUV* pLeft, 
										  EdgeUV* pRight,
										  const Lepra::Canvas* pTexture,
										  Software3DPainter* pThis);
	static void DrawScanLineUV24BitFastZTZW(const GradientsUV* pGradients, 
										  EdgeUV* pLeft, 
										  EdgeUV* pRight,
										  const Lepra::Canvas* pTexture,
										  Software3DPainter* pThis);
	static void DrawScanLineUV32BitFastZTZW(const GradientsUV* pGradients, 
										  EdgeUV* pLeft, 
										  EdgeUV* pRight,
										  const Lepra::Canvas* pTexture,
										  Software3DPainter* pThis);

	// Texturemapped and shaded... no z-test, no z-write
	static void DrawScanLineUVRGB8BitFast(const GradientsUVRGB* pGradients, 
									  EdgeUVRGB* pLeft, 
									  EdgeUVRGB* pRight,
									  const Lepra::Canvas* pTexture,
									  Software3DPainter* pThis);
	static void DrawScanLineUVRGB15BitFast(const GradientsUVRGB* pGradients, 
									   EdgeUVRGB* pLeft, 
									   EdgeUVRGB* pRight,
									   const Lepra::Canvas* pTexture,
									   Software3DPainter* pThis);
	static void DrawScanLineUVRGB16BitFast(const GradientsUVRGB* pGradients, 
									   EdgeUVRGB* pLeft, 
									   EdgeUVRGB* pRight,
									   const Lepra::Canvas* pTexture,
									   Software3DPainter* pThis);
	static void DrawScanLineUVRGB24BitFast(const GradientsUVRGB* pGradients, 
									   EdgeUVRGB* pLeft, 
									   EdgeUVRGB* pRight,
									   const Lepra::Canvas* pTexture,
									   Software3DPainter* pThis);
	static void DrawScanLineUVRGB32BitFast(const GradientsUVRGB* pGradients, 
									   EdgeUVRGB* pLeft, 
									   EdgeUVRGB* pRight,
									   const Lepra::Canvas* pTexture,
									   Software3DPainter* pThis);

	// Texturemapped and shaded... z-test, no z-write
	static void DrawScanLineUVRGB8BitFastZT(const GradientsUVRGB* pGradients, 
									  EdgeUVRGB* pLeft, 
									  EdgeUVRGB* pRight,
									  const Lepra::Canvas* pTexture,
									  Software3DPainter* pThis);
	static void DrawScanLineUVRGB15BitFastZT(const GradientsUVRGB* pGradients, 
									   EdgeUVRGB* pLeft, 
									   EdgeUVRGB* pRight,
									   const Lepra::Canvas* pTexture,
									   Software3DPainter* pThis);
	static void DrawScanLineUVRGB16BitFastZT(const GradientsUVRGB* pGradients, 
									   EdgeUVRGB* pLeft, 
									   EdgeUVRGB* pRight,
									   const Lepra::Canvas* pTexture,
									   Software3DPainter* pThis);
	static void DrawScanLineUVRGB24BitFastZT(const GradientsUVRGB* pGradients, 
									   EdgeUVRGB* pLeft, 
									   EdgeUVRGB* pRight,
									   const Lepra::Canvas* pTexture,
									   Software3DPainter* pThis);
	static void DrawScanLineUVRGB32BitFastZT(const GradientsUVRGB* pGradients, 
									   EdgeUVRGB* pLeft, 
									   EdgeUVRGB* pRight,
									   const Lepra::Canvas* pTexture,
									   Software3DPainter* pThis);

	// Texturemapped and shaded... no z-test, z-write
	static void DrawScanLineUVRGB8BitFastZW(const GradientsUVRGB* pGradients, 
									  EdgeUVRGB* pLeft, 
									  EdgeUVRGB* pRight,
									  const Lepra::Canvas* pTexture,
									  Software3DPainter* pThis);
	static void DrawScanLineUVRGB15BitFastZW(const GradientsUVRGB* pGradients, 
									   EdgeUVRGB* pLeft, 
									   EdgeUVRGB* pRight,
									   const Lepra::Canvas* pTexture,
									   Software3DPainter* pThis);
	static void DrawScanLineUVRGB16BitFastZW(const GradientsUVRGB* pGradients, 
									   EdgeUVRGB* pLeft, 
									   EdgeUVRGB* pRight,
									   const Lepra::Canvas* pTexture,
									   Software3DPainter* pThis);
	static void DrawScanLineUVRGB24BitFastZW(const GradientsUVRGB* pGradients, 
									   EdgeUVRGB* pLeft, 
									   EdgeUVRGB* pRight,
									   const Lepra::Canvas* pTexture,
									   Software3DPainter* pThis);
	static void DrawScanLineUVRGB32BitFastZW(const GradientsUVRGB* pGradients, 
									   EdgeUVRGB* pLeft, 
									   EdgeUVRGB* pRight,
									   const Lepra::Canvas* pTexture,
									   Software3DPainter* pThis);


	// Texturemapped and shaded... z-test, z-write
	static void DrawScanLineUVRGB8BitFastZTZW(const GradientsUVRGB* pGradients, 
									  EdgeUVRGB* pLeft, 
									  EdgeUVRGB* pRight,
									  const Lepra::Canvas* pTexture,
									  Software3DPainter* pThis);
	static void DrawScanLineUVRGB15BitFastZTZW(const GradientsUVRGB* pGradients, 
									   EdgeUVRGB* pLeft, 
									   EdgeUVRGB* pRight,
									   const Lepra::Canvas* pTexture,
									   Software3DPainter* pThis);
	static void DrawScanLineUVRGB16BitFastZTZW(const GradientsUVRGB* pGradients, 
									   EdgeUVRGB* pLeft, 
									   EdgeUVRGB* pRight,
									   const Lepra::Canvas* pTexture,
									   Software3DPainter* pThis);
	static void DrawScanLineUVRGB24BitFastZTZW(const GradientsUVRGB* pGradients, 
									   EdgeUVRGB* pLeft, 
									   EdgeUVRGB* pRight,
									   const Lepra::Canvas* pTexture,
									   Software3DPainter* pThis);
	static void DrawScanLineUVRGB32BitFastZTZW(const GradientsUVRGB* pGradients, 
									   EdgeUVRGB* pLeft, 
									   EdgeUVRGB* pRight,
									   const Lepra::Canvas* pTexture,
									   Software3DPainter* pThis);


	//
	// Texture readers. Only used with RENDER_NICEST.
	//
	void (*GetTextureColor)(float pU, float pV, float pM, Software3DPainter* pThis, 
							Lepra::Color& pColor);

	static void GetTextureColor8BitStandard(float pU, float pV, float pM, Software3DPainter* pThis, 
											Lepra::Color& pColor);
	static void GetTextureColor15BitStandard(float pU, float pV, float pM, Software3DPainter* pThis, 
											Lepra::Color& pColor);
	static void GetTextureColor16BitStandard(float pU, float pV, float pM, Software3DPainter* pThis, 
											Lepra::Color& pColor);
	static void GetTextureColor24BitStandard(float pU, float pV, float pM, Software3DPainter* pThis, 
											Lepra::Color& pColor);
	static void GetTextureColor32BitStandard(float pU, float pV, float pM, Software3DPainter* pThis, 
											Lepra::Color& pColor);


	static void GetTextureColor8BitBilinear(float pU, float pV, float pM, Software3DPainter* pThis, 
											Lepra::Color& pColor);
	static void GetTextureColor15BitBilinear(float pU, float pV, float pM, Software3DPainter* pThis, 
											Lepra::Color& pColor);
	static void GetTextureColor16BitBilinear(float pU, float pV, float pM, Software3DPainter* pThis, 
											Lepra::Color& pColor);
	static void GetTextureColor24BitBilinear(float pU, float pV, float pM, Software3DPainter* pThis, 
											Lepra::Color& pColor);
	static void GetTextureColor32BitBilinear(float pU, float pV, float pM, Software3DPainter* pThis, 
											Lepra::Color& pColor);


	static void GetTextureColor15BitTrilinear(float pU, float pV, float pM, Software3DPainter* pThis, 
											Lepra::Color& pColor);
	static void GetTextureColor16BitTrilinear(float pU, float pV, float pM, Software3DPainter* pThis, 
											Lepra::Color& pColor);
	static void GetTextureColor24BitTrilinear(float pU, float pV, float pM, Software3DPainter* pThis, 
											Lepra::Color& pColor);
	static void GetTextureColor32BitTrilinear(float pU, float pV, float pM, Software3DPainter* pThis, 
											Lepra::Color& pColor);

	unsigned char GetAlpha(int pM, int pPixelIndex);

	//
	// Pixel writers. Only used with RENDER_NICEST.
	//
	void (*PixelShaderCallBack)(unsigned char& pRed, unsigned char& pGreen, unsigned char& pBlue, Software3DPainter* pPainter);
	void (*Write8BitColor)(unsigned char* pDest, unsigned char pColor, unsigned char pAlpha);
	void (*WriteColor)(void* pDest, unsigned char pRed, unsigned char pGreen, unsigned char pBlue, unsigned char pAlpha, Software3DPainter* pThis);

	static void Write8BitColorStandard(unsigned char* pDest, unsigned char pColor, unsigned char pAlpha);
	static void Write8BitColorAlphaTest(unsigned char* pDest, unsigned char pColor, unsigned char pAlpha);

	static void Write15BitColorStandard(void* pDest, unsigned char pRed, unsigned char pGreen, unsigned char pBlue, unsigned char pAlpha, Software3DPainter* pThis);
	static void Write16BitColorStandard(void* pDest, unsigned char pRed, unsigned char pGreen, unsigned char pBlue, unsigned char pAlpha, Software3DPainter* pThis);
	static void Write24BitColorStandard(void* pDest, unsigned char pRed, unsigned char pGreen, unsigned char pBlue, unsigned char pAlpha, Software3DPainter* pThis);
	static void Write32BitColorStandard(void* pDest, unsigned char pRed, unsigned char pGreen, unsigned char pBlue, unsigned char pAlpha, Software3DPainter* pThis);

	static void Write15BitColorAlphaTest(void* pDest, unsigned char pRed, unsigned char pGreen, unsigned char pBlue, unsigned char pAlpha, Software3DPainter* pThis);
	static void Write16BitColorAlphaTest(void* pDest, unsigned char pRed, unsigned char pGreen, unsigned char pBlue, unsigned char pAlpha, Software3DPainter* pThis);
	static void Write24BitColorAlphaTest(void* pDest, unsigned char pRed, unsigned char pGreen, unsigned char pBlue, unsigned char pAlpha, Software3DPainter* pThis);
	static void Write32BitColorAlphaTest(void* pDest, unsigned char pRed, unsigned char pGreen, unsigned char pBlue, unsigned char pAlpha, Software3DPainter* pThis);

	static void Write15BitColorAlphaBlend(void* pDest, unsigned char pRed, unsigned char pGreen, unsigned char pBlue, unsigned char pAlpha, Software3DPainter* pThis);
	static void Write16BitColorAlphaBlend(void* pDest, unsigned char pRed, unsigned char pGreen, unsigned char pBlue, unsigned char pAlpha, Software3DPainter* pThis);
	static void Write24BitColorAlphaBlend(void* pDest, unsigned char pRed, unsigned char pGreen, unsigned char pBlue, unsigned char pAlpha, Software3DPainter* pThis);
	static void Write32BitColorAlphaBlend(void* pDest, unsigned char pRed, unsigned char pGreen, unsigned char pBlue, unsigned char pAlpha, Software3DPainter* pThis);

	inline bool CheckFlag(Flag pFlag);
	inline void SetFlag(Flag pFlag, bool pValue);

	void PostRenderFocalBlur8Bit(float pSharpNearZ, float pSharpFarZ);
	void PostRenderFocalBlur15Bit(float pSharpNearZ, float pSharpFarZ);
	void PostRenderFocalBlur16Bit(float pSharpNearZ, float pSharpFarZ);
	void PostRenderFocalBlur24Bit(float pSharpNearZ, float pSharpFarZ);
	void PostRenderFocalBlur32Bit(float pSharpNearZ, float pSharpFarZ);
	float CalculateBlurConstant(float pSharpNearZ, float pSharpFarZ, float pZ);

	bool mStateChanged;
	bool m8BitColorSearchMode;
	bool mHaveCallBack;

	// Rendering settings.
	int mFlags;
	RenderingHint mRenderingHint;
	AlphaSource mAlphaSource;
	int mSpecularity;
	unsigned char mAlpha;
	unsigned int mAffineExp;
	unsigned int mAffineLength;
	float mAffineLengthFloat;

	// Rendering state data.
	bool mIsRendering;	// True if within a Begin() End() block.
	RenderingPrimitive mCurrentPrimitive;
	bool mTriangleStripInvert;

	PointData mPoint[3]; 
	DataXYZ mXYZ[3];
	DataRGB mRGB[3];
	DataUVM mUVM[3];
	DataXYZ mNormal[3];

	// Used for clipping.
	PointData mClipPoly1[8];
	PointData mClipPoly2[8];
	DataXYZ mClipXYZ[16];
	DataRGB mClipRGB[16];
	DataUVM mClipUVM[16];
	DataXYZ mClipNormal[16];

	const float* mVertexData;
	const float* mUVData;
	const float* mColorDataFloat;
	const unsigned char* mColorData;
	const float* mNormalData;
	float* mTranslatedVertexData;
	float* mTranslatedNormalData;

	// Default color.
	float mRed;
	float mGreen;
	float mBlue;

	int mCurrentXYZIndex;
	int mCurrentUVIndex;
	int mCurrentRGBIndex;
	int mCurrentNormalIndex;
	int mIndexLimit;

	int mVertexCount;

	// Lights...
	LightData mLights[MAX_LIGHTS];
	int mLightIndices[MAX_LIGHTS];
	int mNumLights;
	float mAmbientRed;
	float mAmbientGreen;
	float mAmbientBlue;
	LightModel mLightModel;

	// Edges, used in the triangle renderer. Used to be able
	// to reuse edges that are shared by two triangles.
	EdgeZ      mEdgeZ[3];
	EdgeUV     mEdgeUV[3];
	EdgeRGB    mEdgeRGB[3];
	EdgeUVRGB  mEdgeUVRGB[3];
	EdgeUVM    mEdgeUVM[3];
	EdgeUVRGBM mEdgeUVRGBM[3];
	
	//
	// The transformations below can't use the Transformation class
	// since Quaternion doesn't support left handed coordinate systems.
	// The user may need to define his own coordinate system using
	// SetDefaultTransform().
	//

	// Camera transform.
	Lepra::RotationMatrixF mCameraOrientation;
	Lepra::Vector3DF       mCameraPosition;

	Lepra::TransformationF mCameraTransformOrg;

	// Model transform.
	Lepra::RotationMatrixF mModelOrientation;
	Lepra::Vector3DF       mModelPosition;

	// Normal rotation.
	Lepra::RotationMatrixF mNormalRotation;

	// Texture coordinate transform.
	Lepra::TransformationF mTextureTransform;
	bool mTextureTransformIsIdentity;

	// Default transform. Can't use Transformation class, since
	// we can't use the Quaternion class. It needs to be RotationMatrix.
	Lepra::RotationMatrixF mDefaultOrientation;
	Lepra::Vector3DF mDefaultTranslation;

	Lepra::RotationMatrixF mCamSpaceOrientation;
	Lepra::Vector3DF mCamSpaceTranslation;

	// Viewport and frustum data.
	float mClipLeft;
	float mClipRight;
	float mClipTop;
	float mClipBottom;
	float mViewportLeft;
	float mViewportRight;
	float mViewportTop;
	float mViewportBottom;
	float mPerspectiveCenterX;
	float mPerspectiveCenterY;
	float mNearZ;
	float mFarZ;
	float mFOVAngle;
	float mScreenDistToOrigin;
	bool mOrthographicProjection;

	FaceOrientation mFaceOrientation;

	// Used to index the z-buffer correct. It is the top left
	// corner of the clipped viewport.
	int mZBufferX;
	int mZBufferY;

	// Mip map stuff.
	float mMipMapReferenceFactor;
	float mMipMapMidValue;
	float mMipMapScale;

	// The 2D painter. Only used to clear the screen.
	SoftwarePainter mPainter;

	// The data buffers...
	Lepra::Canvas* mScreen;
	Texture* mTexture;
	Lepra::Canvas* mZBuffer;
	Lepra::Canvas* mKBuffer;	// This is only used in the focal blur effect.
	unsigned char* m8BitLightBuffer;
};

void Software3DPainter::ClearColorBuffer()
{
	mPainter.FillRect((int)mClipLeft, (int)mClipTop, (int)mClipRight + 1, (int)mClipBottom + 1);
}

bool Software3DPainter::CheckFlag(Flag pFlag)
{
	return (mFlags & pFlag) != 0;
}

void Software3DPainter::SetFlag(Flag pFlag, bool pValue)
{
	if (pValue == true)
		mFlags |= (int)pFlag;
	else
		mFlags &= (~((int)pFlag));
}

void Software3DPainter::Set8BitColorSearchModeEnabled(bool pEnabled)
{
	m8BitColorSearchMode = pEnabled;
}

void Software3DPainter::SetAlpha(unsigned char pAlpha)
{
	mAlpha = pAlpha;
}

void Software3DPainter::SetAlphaSource(AlphaSource pAlphaSource)
{
	mAlphaSource = pAlphaSource;
}

float Software3DPainter::GetProjectionDistance()
{
	return mScreenDistToOrigin;
}

int Software3DPainter::GetMaxLights()
{
	return MAX_LIGHTS;
}

int Software3DPainter::GetNumLights()
{
	return mNumLights;
}

void Software3DPainter::SetAmbientLight(float pRed, float pGreen, float pBlue)
{
	mAmbientRed   = pRed;
	mAmbientGreen = pGreen;
	mAmbientBlue  = pBlue;
}

Software3DPainter::LightType Software3DPainter::GetLightType(int pLightIndex)
{
	return mLights[pLightIndex].mType;
}

void Software3DPainter::SetLightModel(LightModel pLightModel)
{
	mLightModel = pLightModel;
}

void Software3DPainter::SetPixelShaderCallBack(void (*CallBack)(unsigned char& pRed, unsigned char& pGreen, unsigned char& pBlue, Software3DPainter* pPainter))
{
	PixelShaderCallBack = CallBack;
	mHaveCallBack = true;
}

void Software3DPainter::PrepareNextTriangleStripPointDataXYZ()
{
	DataXYZ* lTempXYZ = mPoint[0].mXYZ;
	mPoint[0].mXYZ = mPoint[1].mXYZ;
	mPoint[1].mXYZ = mPoint[2].mXYZ;
	mPoint[2].mXYZ = lTempXYZ;
	mCurrentXYZIndex--;
}

void Software3DPainter::PrepareNextTriangleStripPointDataUVM()
{
	DataUVM* lTempUVM = mPoint[0].mUVM;
	mPoint[0].mUVM= mPoint[1].mUVM;
	mPoint[1].mUVM= mPoint[2].mUVM;
	mPoint[2].mUVM= lTempUVM;
	mCurrentUVIndex--;
}

void Software3DPainter::PrepareNextTriangleStripPointDataRGB()
{
	DataRGB* lTempRGB = mPoint[0].mRGB;
	mPoint[0].mRGB = mPoint[1].mRGB;
	mPoint[1].mRGB = mPoint[2].mRGB;
	mPoint[2].mRGB = lTempRGB;
	mCurrentRGBIndex--;
}

void Software3DPainter::PrepareNextTriangleStripPointDataNormal()
{
	if (CheckFlag(FLAG_LIGHTS_ACTIVE) == true)
	{
		if (mLightModel == LIGHT_FLAT)
		{
			mCurrentNormalIndex = 0;
		}
		else
		{
			DataXYZ* lTempNormal = mPoint[0].mNormal;
			mPoint[0].mNormal = mPoint[1].mNormal;
			mPoint[1].mNormal = mPoint[2].mNormal;
			mPoint[2].mNormal = lTempNormal;
			mCurrentNormalIndex--;
		}
	}
}

void Software3DPainter::PrepareNextTriangleFanPointDataXYZ()
{
	DataXYZ* lTempXYZ = mPoint[1].mXYZ;
	mPoint[1].mXYZ = mPoint[2].mXYZ;
	mPoint[2].mXYZ = lTempXYZ;
	mCurrentXYZIndex--;
}

void Software3DPainter::PrepareNextTriangleFanPointDataUVM()
{
	DataUVM* lTempUVM = mPoint[1].mUVM;
	mPoint[1].mUVM= mPoint[2].mUVM;
	mPoint[2].mUVM= lTempUVM;
	mCurrentUVIndex--;
}

void Software3DPainter::PrepareNextTriangleFanPointDataRGB()
{
	DataRGB* lTempRGB = mPoint[1].mRGB;
	mPoint[1].mRGB = mPoint[2].mRGB;
	mPoint[2].mRGB = lTempRGB;
	mCurrentRGBIndex--;
}

void Software3DPainter::PrepareNextTriangleFanPointDataNormal()
{
	if (CheckFlag(FLAG_LIGHTS_ACTIVE) == true)
	{
		if (mLightModel == LIGHT_FLAT)
		{
			mCurrentNormalIndex = 0;
		}
		else
		{
			if (mCurrentNormalIndex >= 3)
			{
				DataXYZ* lTempNormal = mPoint[1].mNormal;
				mPoint[1].mNormal = mPoint[2].mNormal;
				mPoint[2].mNormal = lTempNormal;
				mCurrentNormalIndex--;
			}
		}
	}
}

Lepra::Canvas* Software3DPainter::GetZBuffer()
{
	return mZBuffer;
}

void Software3DPainter::SetVertexPointer(int pCount, const float* pPointer)
{
	if (pCount > mVertexCount)
	{
		if (mTranslatedVertexData != 0)
			delete[] mTranslatedVertexData;

		if (mTranslatedNormalData != 0)
			delete[] mTranslatedNormalData;

		mTranslatedVertexData = new float[pCount * 3];
		mTranslatedNormalData = new float[pCount * 3];
	}

	mVertexData	= pPointer;
	mVertexCount	= pCount;
}

void Software3DPainter::SetUVPointer(const float* pPointer)
{
	mUVData	= pPointer;
}

void Software3DPainter::SetColorPointer(const float* pPointer)
{
	mColorDataFloat = pPointer;
}

void Software3DPainter::SetColorPointer(const unsigned char* pPointer)
{
	mColorData	= pPointer;
}

void Software3DPainter::SetNormalPointer(const float* pPointer)
{
	mNormalData	= pPointer;
}

void Software3DPainter::SetDefaultColor(float pRed, float pGreen, float pBlue)
{
	mRed   = pRed;
	mGreen = pGreen;
	mBlue  = pBlue;
}

Lepra::Canvas* Software3DPainter::GetDestCanvas() const
{
	return mScreen;
}

} // End namespace.

#endif