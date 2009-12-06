/*
	Class:  SoftwarePainter
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand
*/

#include "../../Include/UiSoftwarePainter.h"
#include "../../Include/UiVertex.h"
#include "../../Include/UiEdge.h"
#include "../../Include/UiGradients.h"

#include <memory.h>
#include <math.h>

namespace UiTbc
{

SoftwarePainter::SoftwarePainter() :
	mSwapRGB(false),
	mIncrementalAlpha(false),
	mTextureIDManager(1, 10000, 0)
{
	UpdateFunctionPointers();
}

SoftwarePainter::~SoftwarePainter()
{
	TextureTable::Iterator lTIter = mTextureTable.First();
	while (lTIter != mTextureTable.End())
	{
		Texture* lTexture = *lTIter;

		mTextureIDManager.RecycleId(lTIter.GetKey());

		mTextureTable.Remove(lTIter++);

		if (lTexture->mColorMap != 0)
		{
			delete lTexture->mColorMap;
		}

		if (lTexture->mAlphaBuffer != 0)
		{
			delete lTexture->mAlphaBuffer;
		}

		delete lTexture;
	}
}

void SoftwarePainter::SetDestCanvas(Lepra::Canvas* pCanvas)
{
	if (GetCanvas() != pCanvas)
	{
		Painter::SetDestCanvas(pCanvas);
		UpdateFunctionPointers();
	}

	ResetClippingRect();
}

void SoftwarePainter::SetRenderMode(RenderMode pRM)
{
	if (pRM != GetRenderMode())
	{
		Painter::SetRenderMode(pRM);
		UpdateFunctionPointers();
	}
}

void SoftwarePainter::UpdateFunctionPointers()
{
	/*
		Temporary storage pointers.
	*/
	void (*HNormal) (int, int, int, int, SoftwarePainter*);
	void (*HBlend)  (int, int, int, int, SoftwarePainter*);
	void (*HXor)    (int, int, int, int, SoftwarePainter*);
	void (*HAdd)    (int, int, int, int, SoftwarePainter*);
	void (*VNormal) (int, int, int, int, SoftwarePainter*);
	void (*VBlend)  (int, int, int, int, SoftwarePainter*);
	void (*VXor)    (int, int, int, int, SoftwarePainter*);
	void (*VAdd)    (int, int, int, int, SoftwarePainter*);

	void (*HShadedNormal) (int, int, int, float, float, float, float, float, float, float, float, SoftwarePainter*);
	void (*HShadedBlend)  (int, int, int, float, float, float, float, float, float, float, float, SoftwarePainter*);
	void (*HShadedXor)    (int, int, int, float, float, float, float, float, float, float, float, SoftwarePainter*);
	void (*HShadedAdd)    (int, int, int, float, float, float, float, float, float, float, float, SoftwarePainter*);

	void (*PixelNormal) (int, int, SoftwarePainter*);
	void (*PixelBlend)  (int, int, SoftwarePainter*);
	void (*PixelXor)    (int, int, SoftwarePainter*);
	void (*PixelAdd)    (int, int, SoftwarePainter*);

	void (*DLineNormal) (int, int, int, int, SoftwarePainter*);
	void (*DLineBlend)  (int, int, int, int, SoftwarePainter*);
	void (*DLineXor)    (int, int, int, int, SoftwarePainter*);
	void (*DLineAdd)    (int, int, int, int, SoftwarePainter*);

	void (*FSRFuncNormal)(int, int, int, int, SoftwarePainter*);
	void (*FSRFuncBlend) (int, int, int, int, SoftwarePainter*);
	void (*FSRFuncXor)   (int, int, int, int, SoftwarePainter*);
	void (*FSRFuncAdd)   (int, int, int, int, SoftwarePainter*);

	void (*DAImageNormal) (const Lepra::Canvas&, int, int, SoftwarePainter*);
	void (*DAImageBlend)  (const Lepra::Canvas&, int, int, SoftwarePainter*);
	void (*DAImageTest)   (const Lepra::Canvas&, int, int, SoftwarePainter*);
	void (*DAImageXor)    (const Lepra::Canvas&, int, int, SoftwarePainter*);
	void (*DAImageAdd)    (const Lepra::Canvas&, int, int, SoftwarePainter*);

	void (*DrawImageNormal)   (const Lepra::Canvas&, const Lepra::Canvas*, int, int, const Lepra::PixelRect&, SoftwarePainter*);
	void (*DrawImageBlend)    (const Lepra::Canvas&, const Lepra::Canvas*, int, int, const Lepra::PixelRect&, SoftwarePainter*);
	void (*DrawImageTest)     (const Lepra::Canvas&, const Lepra::Canvas*, int, int, const Lepra::PixelRect&, SoftwarePainter*);
	void (*DrawImageXor)      (const Lepra::Canvas&, const Lepra::Canvas*, int, int, const Lepra::PixelRect&, SoftwarePainter*);
	void (*DrawImageAdd)      (const Lepra::Canvas&, const Lepra::Canvas*, int, int, const Lepra::PixelRect&, SoftwarePainter*);
	void (*DrawImageFBNormal) (const Lepra::Canvas&, const Lepra::Canvas*, int, int, const Lepra::PixelRect&, SoftwarePainter*);
	void (*DrawImageFBBlend)  (const Lepra::Canvas&, const Lepra::Canvas*, int, int, const Lepra::PixelRect&, SoftwarePainter*);
	void (*DrawImageFBTest)   (const Lepra::Canvas&, const Lepra::Canvas*, int, int, const Lepra::PixelRect&, SoftwarePainter*);
	void (*DrawImageFBXor)    (const Lepra::Canvas&, const Lepra::Canvas*, int, int, const Lepra::PixelRect&, SoftwarePainter*);
	void (*DrawImageFBAdd)    (const Lepra::Canvas&, const Lepra::Canvas*, int, int, const Lepra::PixelRect&, SoftwarePainter*);

	void (*DrawSImageNormal)  (const Lepra::Canvas&, const Lepra::Canvas*, const Lepra::PixelRect&, const Lepra::PixelRect&, SoftwarePainter*);
	void (*DrawSImageBlend)   (const Lepra::Canvas&, const Lepra::Canvas*, const Lepra::PixelRect&, const Lepra::PixelRect&, SoftwarePainter*);
	void (*DrawSImageTest)    (const Lepra::Canvas&, const Lepra::Canvas*, const Lepra::PixelRect&, const Lepra::PixelRect&, SoftwarePainter*);
	void (*DrawSImageXor)     (const Lepra::Canvas&, const Lepra::Canvas*, const Lepra::PixelRect&, const Lepra::PixelRect&, SoftwarePainter*);
	void (*DrawSImageAdd)     (const Lepra::Canvas&, const Lepra::Canvas*, const Lepra::PixelRect&, const Lepra::PixelRect&, SoftwarePainter*);
	void (*DrawSImageFBNormal)(const Lepra::Canvas&, const Lepra::Canvas*, const Lepra::PixelRect&, const Lepra::PixelRect&, SoftwarePainter*);
	void (*DrawSImageFBBlend) (const Lepra::Canvas&, const Lepra::Canvas*, const Lepra::PixelRect&, const Lepra::PixelRect&, SoftwarePainter*);
	void (*DrawSImageFBTest)  (const Lepra::Canvas&, const Lepra::Canvas*, const Lepra::PixelRect&, const Lepra::PixelRect&, SoftwarePainter*);
	void (*DrawSImageFBXor)   (const Lepra::Canvas&, const Lepra::Canvas*, const Lepra::PixelRect&, const Lepra::PixelRect&, SoftwarePainter*);
	void (*DrawSImageFBAdd)   (const Lepra::Canvas&, const Lepra::Canvas*, const Lepra::PixelRect&, const Lepra::PixelRect&, SoftwarePainter*);

	void (*DrawScanLineNormal) (const Gradients2DUV*, Edge2DUV*, Edge2DUV*, const Lepra::Canvas*, const Lepra::Canvas*, SoftwarePainter* pThis);
	void (*DrawScanLineTest)   (const Gradients2DUV*, Edge2DUV*, Edge2DUV*, const Lepra::Canvas*, const Lepra::Canvas*, SoftwarePainter* pThis);
	void (*DrawScanLineBlend)  (const Gradients2DUV*, Edge2DUV*, Edge2DUV*, const Lepra::Canvas*, const Lepra::Canvas*, SoftwarePainter* pThis);
	void (*DrawScanLineBlend2) (const Gradients2DUV*, Edge2DUV*, Edge2DUV*, const Lepra::Canvas*, const Lepra::Canvas*, SoftwarePainter* pThis);
	void (*DrawScanLineXor)    (const Gradients2DUV*, Edge2DUV*, Edge2DUV*, const Lepra::Canvas*, const Lepra::Canvas*, SoftwarePainter* pThis);
	void (*DrawScanLineAdd)    (const Gradients2DUV*, Edge2DUV*, Edge2DUV*, const Lepra::Canvas*, const Lepra::Canvas*, SoftwarePainter* pThis);

	void (*DrawScanLinePow2Normal) (const Gradients2DUV*, Edge2DUV*, Edge2DUV*, const Lepra::Canvas*, const Lepra::Canvas*, unsigned, SoftwarePainter* pThis);
	void (*DrawScanLinePow2Test)   (const Gradients2DUV*, Edge2DUV*, Edge2DUV*, const Lepra::Canvas*, const Lepra::Canvas*, unsigned, SoftwarePainter* pThis);
	void (*DrawScanLinePow2Blend)  (const Gradients2DUV*, Edge2DUV*, Edge2DUV*, const Lepra::Canvas*, const Lepra::Canvas*, unsigned, SoftwarePainter* pThis);
	void (*DrawScanLinePow2Blend2) (const Gradients2DUV*, Edge2DUV*, Edge2DUV*, const Lepra::Canvas*, const Lepra::Canvas*, unsigned, SoftwarePainter* pThis);
	void (*DrawScanLinePow2Xor)    (const Gradients2DUV*, Edge2DUV*, Edge2DUV*, const Lepra::Canvas*, const Lepra::Canvas*, unsigned, SoftwarePainter* pThis);
	void (*DrawScanLinePow2Add)    (const Gradients2DUV*, Edge2DUV*, Edge2DUV*, const Lepra::Canvas*, const Lepra::Canvas*, unsigned, SoftwarePainter* pThis);

	/*
		Default functions.
	*/

	HNormal = DrawHLine8BitNormal;
	HBlend  = DrawHLine8BitNormal;
	HXor    = DrawHLine8BitXor;
	HAdd    = DrawHLine8BitAdd;
	VNormal = DrawVLine8BitNormal;
	VBlend  = DrawVLine8BitNormal;
	VXor    = DrawVLine8BitXor;
	VAdd    = DrawVLine8BitAdd;

	HShadedNormal = DrawShadedHLine8BitNormal;
	HShadedBlend  = DrawShadedHLine8BitNormal;
	HShadedXor    = DrawShadedHLine8BitNormal;
	HShadedAdd    = DrawShadedHLine8BitNormal;

	PixelNormal = DrawPixel8BitNormal;
	PixelBlend  = DrawPixel8BitNormal;
	PixelXor    = DrawPixel8BitXor;
	PixelAdd    = DrawPixel8BitAdd;

	DLineNormal = DrawLine8BitNormal;
	DLineBlend  = DrawLine8BitNormal;
	DLineXor    = DrawLine8BitXor;
	DLineAdd    = DrawLine8BitAdd;

	FSRFuncNormal = FillShadedRect8BitNormal;
	FSRFuncBlend  = FillShadedRect8BitNormal;
	FSRFuncXor    = FillShadedRect8BitNormal;
	FSRFuncAdd    = FillShadedRect8BitNormal;

	DAImageNormal = DrawAlphaImage8BitNormal;
	DAImageBlend  = DrawAlphaImage8BitNormal;
	DAImageTest   = DrawAlphaImage8BitTest;
	DAImageXor    = DrawAlphaImage8BitXor;
	DAImageAdd    = DrawAlphaImage8BitAdd;

	DrawImageNormal   = DrawImage8BitNormal;
	DrawImageBlend    = DrawImage8BitTest;
	DrawImageTest     = DrawImage8BitTest;
	DrawImageXor      = DrawImage8BitXor;
	DrawImageAdd      = DrawImage8BitAdd;
	DrawImageFBNormal = DrawImage8BitNormal;
	DrawImageFBBlend  = DrawImage8BitNormal;
	DrawImageFBTest   = DrawImage8BitNormal;
	DrawImageFBXor    = DrawImage8BitXor;
	DrawImageFBAdd    = DrawImage8BitAdd;

	DrawSImageNormal   = DrawSImage8BitNormal;
	DrawSImageBlend    = DrawSImage8BitTest;
	DrawSImageTest     = DrawSImage8BitTest;
	DrawSImageXor      = DrawSImage8BitXor;
	DrawSImageAdd      = DrawSImage8BitAdd;
	DrawSImageFBNormal = DrawSImage8BitNormal;
	DrawSImageFBBlend  = DrawSImage8BitNormal;
	DrawSImageFBTest   = DrawSImage8BitNormal;
	DrawSImageFBXor    = DrawSImage8BitXor;
	DrawSImageFBAdd    = DrawSImage8BitAdd;

	DrawScanLineNormal  = DrawScanLine8BitNormal;
	DrawScanLineTest    = DrawScanLine8BitTest;
	DrawScanLineBlend   = DrawScanLine8BitTest;
	DrawScanLineBlend2  = DrawScanLine8BitNormal;
	DrawScanLineXor     = DrawScanLine8BitXor;
	DrawScanLineAdd     = DrawScanLine8BitAdd;

	DrawScanLinePow2Normal = DrawScanLinePow28BitNormal;
	DrawScanLinePow2Test   = DrawScanLinePow28BitTest;
	DrawScanLinePow2Blend  = DrawScanLinePow28BitTest;
	DrawScanLinePow2Blend2 = DrawScanLinePow28BitNormal;
	DrawScanLinePow2Xor    = DrawScanLinePow28BitXor;
	DrawScanLinePow2Add    = DrawScanLinePow28BitAdd;

	/*
		Set bitrate specific functions.
	*/

	if (GetCanvas() != 0)
	{
		switch(GetCanvas()->GetBitDepth())
		{
		case Lepra::Canvas::BITDEPTH_15_BIT:
			HNormal = DrawHLine16BitNormal;
			HBlend  = DrawHLine15BitBlend;
			HXor    = DrawHLine16BitXor;
			HAdd    = DrawHLine15BitAdd;
			VNormal = DrawVLine16BitNormal;
			VBlend  = DrawVLine15BitBlend;
			VXor    = DrawVLine16BitXor;
			VAdd    = DrawVLine15BitAdd;

			HShadedNormal = DrawShadedHLine15BitNormal;
			HShadedBlend  = DrawShadedHLine15BitBlend;
			HShadedXor    = DrawShadedHLine15BitXor;
			HShadedAdd    = DrawShadedHLine15BitAdd;

			PixelNormal = DrawPixel16BitNormal;
			PixelBlend  = DrawPixel15BitBlend;
			PixelXor    = DrawPixel16BitXor;
			PixelAdd    = DrawPixel15BitAdd;

			DLineNormal = DrawLine16BitNormal;
			DLineBlend  = DrawLine15BitBlend;
			DLineXor    = DrawLine16BitXor;
			DLineAdd    = DrawLine15BitAdd;

			FSRFuncNormal = FillShadedRect15BitNormal;
			FSRFuncBlend  = FillShadedRect15BitBlend;
			FSRFuncXor    = FillShadedRect15BitXor;
			FSRFuncAdd    = FillShadedRect15BitAdd;

			DAImageNormal = DrawAlphaImage16BitNormal;
			DAImageBlend  = DrawAlphaImage15BitBlend;
			DAImageTest   = DrawAlphaImage16BitTest;
			DAImageXor    = DrawAlphaImage16BitXor;
			DAImageAdd    = DrawAlphaImage15BitAdd;

			DrawImageNormal   = DrawImage16BitNormal;
			DrawImageBlend    = DrawImage15BitBlend;
			DrawImageTest     = DrawImage16BitTest;
			DrawImageXor      = DrawImage16BitXor;
			DrawImageAdd      = DrawImage15BitAdd;
			DrawImageFBNormal = DrawImage16BitNormal;
			DrawImageFBBlend  = DrawImage16BitBlend2;
			DrawImageFBTest   = DrawImage16BitNormal;
			DrawImageFBXor    = DrawImage16BitXor;
			DrawImageFBAdd    = DrawImage15BitAdd;

			DrawSImageNormal   = DrawSImage16BitNormal;
			DrawSImageBlend    = DrawSImage15BitBlend;
			DrawSImageTest     = DrawSImage16BitTest;
			DrawSImageXor      = DrawSImage16BitXor;
			DrawSImageAdd      = DrawSImage15BitAdd;
			DrawSImageFBNormal = DrawSImage16BitNormal;
			DrawSImageFBBlend  = DrawSImage16BitBlend2;
			DrawSImageFBTest   = DrawSImage16BitNormal;
			DrawSImageFBXor    = DrawSImage16BitXor;
			DrawSImageFBAdd    = DrawSImage15BitAdd;

			DrawScanLineNormal  = DrawScanLine16BitNormal;
			DrawScanLineBlend   = DrawScanLine15BitBlend;
			DrawScanLineBlend2  = DrawScanLine15BitBlend2;
			DrawScanLineTest    = DrawScanLine16BitTest;
			DrawScanLineXor     = DrawScanLine16BitXor;
			DrawScanLineAdd     = DrawScanLine15BitAdd;

			DrawScanLinePow2Normal = DrawScanLinePow216BitNormal;
			DrawScanLinePow2Blend  = DrawScanLinePow215BitBlend;
			DrawScanLinePow2Blend2 = DrawScanLinePow215BitBlend2;
			DrawScanLinePow2Test   = DrawScanLinePow216BitTest;
			DrawScanLinePow2Xor    = DrawScanLinePow216BitXor;
			DrawScanLinePow2Add    = DrawScanLinePow215BitAdd;
			break;
		case Lepra::Canvas::BITDEPTH_16_BIT:
			HNormal = DrawHLine16BitNormal;
			HBlend  = DrawHLine16BitBlend;
			HXor    = DrawHLine16BitXor;
			HAdd    = DrawHLine16BitAdd;
			VNormal = DrawVLine16BitNormal;
			VBlend  = DrawVLine16BitBlend;
			VXor    = DrawVLine16BitXor;
			VAdd    = DrawVLine16BitAdd;

			HShadedNormal = DrawShadedHLine16BitNormal;
			HShadedBlend  = DrawShadedHLine16BitBlend;
			HShadedXor    = DrawShadedHLine16BitXor;
			HShadedAdd    = DrawShadedHLine16BitAdd;

			PixelNormal = DrawPixel16BitNormal;
			PixelBlend  = DrawPixel16BitBlend;
			PixelXor    = DrawPixel16BitXor;
			PixelAdd    = DrawPixel16BitAdd;

			DLineNormal = DrawLine16BitNormal;
			DLineBlend  = DrawLine16BitBlend;
			DLineXor    = DrawLine16BitXor;
			DLineAdd    = DrawLine16BitAdd;

			FSRFuncNormal = FillShadedRect16BitNormal;
			FSRFuncBlend  = FillShadedRect16BitBlend;
			FSRFuncXor    = FillShadedRect16BitXor;
			FSRFuncAdd    = FillShadedRect16BitAdd;

			DAImageNormal = DrawAlphaImage16BitNormal;
			DAImageBlend  = DrawAlphaImage16BitBlend;
			DAImageTest   = DrawAlphaImage16BitTest;
			DAImageXor    = DrawAlphaImage16BitXor;
			DAImageAdd    = DrawAlphaImage16BitAdd;

			DrawImageNormal   = DrawImage16BitNormal;
			DrawImageBlend    = DrawImage16BitBlend;
			DrawImageTest     = DrawImage16BitTest;
			DrawImageXor      = DrawImage16BitXor;
			DrawImageAdd      = DrawImage16BitAdd;
			DrawImageFBNormal = DrawImage16BitNormal;
			DrawImageFBBlend  = DrawImage16BitBlend2;
			DrawImageFBTest   = DrawImage16BitNormal;
			DrawImageFBXor    = DrawImage16BitXor;
			DrawImageFBAdd    = DrawImage16BitAdd;

			DrawSImageNormal   = DrawSImage16BitNormal;
			DrawSImageBlend    = DrawSImage16BitBlend;
			DrawSImageTest     = DrawSImage16BitTest;
			DrawSImageXor      = DrawSImage16BitXor;
			DrawSImageAdd      = DrawSImage16BitAdd;
			DrawSImageFBNormal = DrawSImage16BitNormal;
			DrawSImageFBBlend  = DrawSImage16BitBlend2;
			DrawSImageFBTest   = DrawSImage16BitNormal;
			DrawSImageFBXor    = DrawSImage16BitXor;
			DrawSImageFBAdd    = DrawSImage16BitAdd;

			DrawScanLineNormal  = DrawScanLine16BitNormal;
			DrawScanLineBlend   = DrawScanLine16BitBlend;
			DrawScanLineBlend2  = DrawScanLine16BitBlend2;
			DrawScanLineTest    = DrawScanLine16BitTest;
			DrawScanLineXor     = DrawScanLine16BitXor;
			DrawScanLineAdd     = DrawScanLine16BitAdd;

			DrawScanLinePow2Normal = DrawScanLinePow216BitNormal;
			DrawScanLinePow2Blend  = DrawScanLinePow216BitBlend;
			DrawScanLinePow2Blend2 = DrawScanLinePow216BitBlend2;
			DrawScanLinePow2Test   = DrawScanLinePow216BitTest;
			DrawScanLinePow2Xor    = DrawScanLinePow216BitXor;
			DrawScanLinePow2Add    = DrawScanLinePow216BitAdd;
			break;
		case Lepra::Canvas::BITDEPTH_24_BIT:
			HNormal = DrawHLine24BitNormal;
			HBlend  = DrawHLine24BitBlend;
			HXor    = DrawHLine24BitXor;
			HAdd    = DrawHLine24BitAdd;
			VNormal = DrawVLine24BitNormal;
			VBlend  = DrawVLine24BitBlend;
			VXor    = DrawVLine24BitXor;
			VAdd    = DrawVLine24BitAdd;

			HShadedNormal = DrawShadedHLine24BitNormal;
			HShadedBlend  = DrawShadedHLine24BitBlend;
			HShadedXor    = DrawShadedHLine24BitXor;
			HShadedAdd    = DrawShadedHLine24BitAdd;

			PixelNormal = DrawPixel24BitNormal;
			PixelBlend  = DrawPixel24BitBlend;
			PixelXor    = DrawPixel24BitXor;
			PixelAdd    = DrawPixel24BitAdd;

			DLineNormal = DrawLine24BitNormal;
			DLineBlend  = DrawLine24BitBlend;
			DLineXor    = DrawLine24BitXor;
			DLineAdd    = DrawLine24BitAdd;

			FSRFuncNormal = FillShadedRect24BitNormal;
			FSRFuncBlend  = FillShadedRect24BitBlend;
			FSRFuncXor    = FillShadedRect24BitXor;
			FSRFuncAdd    = FillShadedRect24BitAdd;

			DAImageNormal = DrawAlphaImage24BitNormal;
			DAImageBlend  = DrawAlphaImage24BitBlend;
			DAImageTest   = DrawAlphaImage24BitTest;
			DAImageXor    = DrawAlphaImage24BitXor;
			DAImageAdd    = DrawAlphaImage24BitAdd;

			DrawImageNormal   = DrawImage24BitNormal;
			DrawImageBlend    = DrawImage24BitBlend;
			DrawImageTest     = DrawImage24BitTest;
			DrawImageXor      = DrawImage24BitXor;
			DrawImageAdd      = DrawImage24BitAdd;
			DrawImageFBNormal = DrawImage24BitNormal;
			DrawImageFBBlend  = DrawImage24BitBlend2;
			DrawImageFBTest   = DrawImage24BitNormal;
			DrawImageFBXor    = DrawImage24BitXor;
			DrawImageFBAdd    = DrawImage24BitAdd;

			DrawSImageNormal   = DrawSImage24BitNormal;
			DrawSImageBlend    = DrawSImage24BitBlend;
			DrawSImageTest     = DrawSImage24BitTest;
			DrawSImageXor      = DrawSImage24BitXor;
			DrawSImageAdd      = DrawSImage24BitAdd;
			DrawSImageFBNormal = DrawSImage24BitNormal;
			DrawSImageFBBlend  = DrawSImage24BitBlend2;
			DrawSImageFBTest   = DrawSImage24BitNormal;
			DrawSImageFBXor    = DrawSImage24BitXor;
			DrawSImageFBAdd    = DrawSImage24BitAdd;

			DrawScanLineNormal  = DrawScanLine24BitNormal;
			DrawScanLineBlend   = DrawScanLine24BitBlend;
			DrawScanLineBlend2  = DrawScanLine24BitBlend2;
			DrawScanLineTest    = DrawScanLine24BitTest;
			DrawScanLineXor     = DrawScanLine24BitXor;
			DrawScanLineAdd     = DrawScanLine24BitAdd;

			DrawScanLinePow2Normal = DrawScanLinePow224BitNormal;
			DrawScanLinePow2Blend  = DrawScanLinePow224BitBlend;
			DrawScanLinePow2Blend2 = DrawScanLinePow224BitBlend2;
			DrawScanLinePow2Test   = DrawScanLinePow224BitTest;
			DrawScanLinePow2Xor    = DrawScanLinePow224BitXor;
			DrawScanLinePow2Add    = DrawScanLinePow224BitAdd;
			break;
		case Lepra::Canvas::BITDEPTH_32_BIT:
			HNormal = DrawHLine32BitNormal;
			HBlend  = DrawHLine32BitBlend;
			HXor    = DrawHLine32BitXor;
			HAdd    = DrawHLine32BitAdd;
			VNormal = DrawVLine32BitNormal;
			VBlend  = DrawVLine32BitBlend;
			VXor    = DrawVLine32BitXor;
			VAdd    = DrawVLine32BitAdd;

			HShadedNormal = DrawShadedHLine32BitNormal;
			HShadedBlend  = DrawShadedHLine32BitBlend;
			HShadedXor    = DrawShadedHLine32BitXor;
			HShadedAdd    = DrawShadedHLine32BitAdd;

			PixelNormal = DrawPixel32BitNormal;
			PixelBlend  = DrawPixel32BitBlend;
			PixelXor    = DrawPixel32BitXor;
			PixelAdd    = DrawPixel32BitAdd;

			DLineNormal = DrawLine32BitNormal;
			DLineBlend  = DrawLine32BitBlend;
			DLineXor    = DrawLine32BitXor;
			DLineAdd    = DrawLine32BitAdd;

			FSRFuncNormal = FillShadedRect32BitNormal;
			FSRFuncBlend  = FillShadedRect32BitBlend;
			FSRFuncXor    = FillShadedRect32BitXor;
			FSRFuncAdd    = FillShadedRect32BitAdd;

			DAImageNormal = DrawAlphaImage32BitNormal;
			DAImageBlend  = DrawAlphaImage32BitBlend;
			DAImageTest   = DrawAlphaImage32BitTest;
			DAImageXor    = DrawAlphaImage32BitXor;
			DAImageAdd    = DrawAlphaImage32BitAdd;

			DrawImageNormal   = DrawImage32BitNormal;
			DrawImageBlend    = DrawImage32BitBlend;
			DrawImageTest     = DrawImage32BitTest;
			DrawImageXor      = DrawImage32BitXor;
			DrawImageAdd      = DrawImage32BitAdd;
			DrawImageFBNormal = DrawImage32BitNormal;
			DrawImageFBBlend  = DrawImage32BitBlend2;
			DrawImageFBTest   = DrawImage32BitNormal;
			DrawImageFBXor    = DrawImage32BitXor;
			DrawImageFBAdd    = DrawImage32BitAdd;

			DrawSImageNormal   = DrawSImage32BitNormal;
			DrawSImageBlend    = DrawSImage32BitBlend;
			DrawSImageTest     = DrawSImage32BitTest;
			DrawSImageXor      = DrawSImage32BitXor;
			DrawSImageAdd      = DrawSImage32BitAdd;
			DrawSImageFBNormal = DrawSImage32BitNormal;
			DrawSImageFBBlend  = DrawSImage32BitBlend2;
			DrawSImageFBTest   = DrawSImage32BitNormal;
			DrawSImageFBXor    = DrawSImage32BitXor;
			DrawSImageFBAdd    = DrawSImage32BitAdd;

			DrawScanLineNormal  = DrawScanLine32BitNormal;
			DrawScanLineBlend   = DrawScanLine32BitBlend;
			DrawScanLineBlend2  = DrawScanLine32BitBlend2;
			DrawScanLineTest    = DrawScanLine32BitTest;
			DrawScanLineXor     = DrawScanLine32BitXor;
			DrawScanLineAdd     = DrawScanLine32BitAdd;

			DrawScanLinePow2Normal = DrawScanLinePow232BitNormal;
			DrawScanLinePow2Blend  = DrawScanLinePow232BitBlend;
			DrawScanLinePow2Blend2 = DrawScanLinePow232BitBlend2;
			DrawScanLinePow2Test   = DrawScanLinePow232BitTest;
			DrawScanLinePow2Xor    = DrawScanLinePow232BitXor;
			DrawScanLinePow2Add    = DrawScanLinePow232BitAdd;
			break;
		}
	}

	switch(GetRenderMode())
	{
	case Painter::RM_ALPHABLEND:
		DrawHLineFunc                = HBlend;
		DrawVLineFunc                = VBlend;
		DrawShadedHLineFunc          = HShadedBlend;
		DrawPixelFunc                = PixelBlend;
		DrawLineFunc                 = DLineBlend;
		DrawImageFunc                = DrawImageBlend;
		DrawImageFallback            = DrawImageFBBlend;
		DrawImageStretchFunc         = DrawSImageBlend;
		DrawImageStretchFallback     = DrawSImageFBBlend;
		FillShadedRectFunc           = FSRFuncBlend;
		DrawAlphaImageFunc           = DAImageBlend;
		DrawScanLineFunc             = DrawScanLineBlend;
		DrawScanLineFuncFallback     = DrawScanLineBlend2;
		DrawScanLinePow2Func         = DrawScanLinePow2Blend;
		DrawScanLinePow2FuncFallback = DrawScanLinePow2Blend2;
		break;
	case Painter::RM_XOR:
		DrawHLineFunc                = HXor;
		DrawVLineFunc                = VXor;
		DrawShadedHLineFunc          = HShadedXor;
		DrawPixelFunc                = PixelXor;
		DrawLineFunc                 = DLineXor;
		DrawImageFunc                = DrawImageXor;
		DrawImageFallback            = DrawImageFBXor;
		DrawImageStretchFunc         = DrawSImageXor;
		DrawImageStretchFallback     = DrawSImageFBXor;
		FillShadedRectFunc           = FSRFuncXor;
		DrawAlphaImageFunc           = DAImageXor;
		DrawScanLineFunc             = DrawScanLineXor;
		DrawScanLineFuncFallback     = DrawScanLineXor;
		DrawScanLinePow2Func         = DrawScanLinePow2Xor;
		DrawScanLinePow2FuncFallback = DrawScanLinePow2Xor;
		break;
	case Painter::RM_ADD:
		DrawHLineFunc                = HAdd;
		DrawVLineFunc                = VAdd;
		DrawShadedHLineFunc          = HShadedAdd;
		DrawPixelFunc                = PixelAdd;
		DrawLineFunc                 = DLineAdd;
		DrawImageFunc                = DrawImageAdd;
		DrawImageFallback            = DrawImageFBAdd;
		DrawImageStretchFunc         = DrawSImageAdd;
		DrawImageStretchFallback     = DrawSImageFBAdd;
		FillShadedRectFunc           = FSRFuncAdd;
		DrawAlphaImageFunc           = DAImageAdd;
		DrawScanLineFunc             = DrawScanLineAdd;
		DrawScanLineFuncFallback     = DrawScanLineAdd;
		DrawScanLinePow2Func         = DrawScanLinePow2Add;
		DrawScanLinePow2FuncFallback = DrawScanLinePow2Add;
		break;
	case Painter::RM_ALPHATEST:
		DrawHLineFunc                = HNormal;
		DrawVLineFunc                = VNormal;
		DrawShadedHLineFunc          = HShadedNormal;
		DrawPixelFunc                = PixelNormal;
		DrawLineFunc                 = DLineNormal;
		DrawImageFunc                = DrawImageTest;
		DrawImageFallback            = DrawImageFBTest;
		DrawImageStretchFunc         = DrawSImageTest;
		DrawImageStretchFallback     = DrawSImageFBTest;
		FillShadedRectFunc           = FSRFuncNormal;
		DrawAlphaImageFunc           = DAImageTest;
		DrawScanLineFunc             = DrawScanLineTest;
		DrawScanLineFuncFallback     = DrawScanLineNormal;
		DrawScanLinePow2Func         = DrawScanLinePow2Test;
		DrawScanLinePow2FuncFallback = DrawScanLinePow2Normal;
		break;
	case Painter::RM_NORMAL:
	default:
		DrawHLineFunc                = HNormal;
		DrawVLineFunc                = VNormal;
		DrawShadedHLineFunc          = HShadedNormal;
		DrawPixelFunc                = PixelNormal;
		DrawLineFunc                 = DLineNormal;
		DrawImageFunc                = DrawImageNormal;
		DrawImageFallback            = DrawImageFBNormal;
		DrawImageStretchFunc         = DrawSImageNormal;
		DrawImageStretchFallback     = DrawSImageFBNormal;
		FillShadedRectFunc           = FSRFuncNormal;
		DrawAlphaImageFunc           = DAImageNormal;
		DrawScanLineFunc             = DrawScanLineNormal;
		DrawScanLineFuncFallback     = DrawScanLineNormal;
		DrawScanLinePow2Func         = DrawScanLinePow2Normal;
		DrawScanLinePow2FuncFallback = DrawScanLinePow2Normal;
		break;
	}
}

void SoftwarePainter::SetClippingRect(int pLeft, int pTop, int pRight, int pBottom)
{
	ToScreenCoords(pLeft, pTop);
	ToScreenCoords(pRight, pBottom);

	// It is important that the parameters have valid values, since
	// the program may crash otherwise.
	if (pTop > pBottom)
	{
		int lTemp = pTop;
		pTop = pBottom;
		pBottom = lTemp;
	}

	if (pLeft > pRight)
	{
		int lTemp = pLeft;
		pLeft = pRight;
		pRight = lTemp;
	}

	if (pTop < 0)
	{
		pTop = 0;
	}
	if (pTop >= (int)GetCanvas()->GetHeight())
	{
		pTop = (int)GetCanvas()->GetHeight() - 1;
	}

	if (pBottom < 0)
	{
		pBottom = 0;
	}
	if (pBottom > (int)GetCanvas()->GetHeight())
	{
		pBottom = (int)GetCanvas()->GetHeight();
	}

	if (pLeft < 0)
	{
		pLeft = 0;
	}
	if (pLeft >= (int)GetCanvas()->GetWidth())
	{
		pLeft = (int)GetCanvas()->GetWidth() - 1;
	}

	if (pRight < 0)
	{
		pRight = 0;
	}
	if (pRight > (int)GetCanvas()->GetWidth())
	{
		pRight = (int)GetCanvas()->GetWidth();
	}

	ToUserCoords(pLeft, pTop);
	ToUserCoords(pRight, pBottom);
	Painter::SetClippingRect(pLeft, pTop, pRight, pBottom);
}

void SoftwarePainter::ResetClippingRect()
{
	int lLeft = 0;
	int lTop = 0;
	int lRight = GetCanvas()->GetWidth();
	int lBottom = GetCanvas()->GetHeight();
	ToUserCoords(lLeft, lTop);
	ToUserCoords(lRight, lBottom);
	Painter::SetClippingRect(lLeft, lTop, lRight, lBottom);
}

void SoftwarePainter::SetColor(const Lepra::Color& pColor, unsigned pColorIndex)
{
	Lepra::Color lColor;

	if (mSwapRGB == true)
	{
		lColor.mRed   = pColor.mBlue;
		lColor.mGreen = pColor.mGreen;
		lColor.mBlue  = pColor.mRed;
		lColor.mAlpha = pColor.mAlpha;
	}
	else
	{
		lColor.mRed   = pColor.mRed;
		lColor.mGreen = pColor.mGreen;
		lColor.mBlue  = pColor.mBlue;
		lColor.mAlpha = pColor.mAlpha;
	}

	switch(GetCanvas()->GetBitDepth())
	{
		case Lepra::Canvas::BITDEPTH_8_BIT:
		{
			mColor8[pColorIndex] = lColor.mAlpha;
		} break;
		case Lepra::Canvas::BITDEPTH_15_BIT:
		{
			mColor16[pColorIndex] = (((Lepra::uint16)(lColor.mRed >> 3)) << 10) | 
						(((Lepra::uint16)(lColor.mGreen >> 3)) << 5) | 
						  (Lepra::uint16)(lColor.mBlue >> 3);
		} break;
		case Lepra::Canvas::BITDEPTH_16_BIT:
		{
			mColor16[pColorIndex] = (((Lepra::uint16)(lColor.mRed >> 3)) << 11) | 
						(((Lepra::uint16)(lColor.mGreen >> 2)) << 5) | 
						  (Lepra::uint16)(lColor.mBlue >> 3);
		} break;
		case Lepra::Canvas::BITDEPTH_24_BIT:
		{
		} break;
		case Lepra::Canvas::BITDEPTH_32_BIT:
		{
			// Use color index as alpha.
			mColor32[pColorIndex] = (((unsigned)lColor.mAlpha) << 24) | 
						(((unsigned)lColor.mRed)   << 16) | 
						(((unsigned)lColor.mGreen) <<  8) | 
						  (unsigned)lColor.mBlue;
		} break;
	}
	Painter::SetColor(lColor, pColorIndex);
}

void SoftwarePainter::DoDrawPixel(int x, int y)
{
	ToScreenCoords(x, y);
	Lepra::PixelRect lClippingRect;
	GetScreenCoordClippingRect(lClippingRect);

	if (x >= lClippingRect.mLeft && 
	    x < lClippingRect.mRight &&
	    y >= lClippingRect.mTop &&
	    y < lClippingRect.mBottom)
	{
		DrawPixelFunc(x, y, this);
	}
}

void SoftwarePainter::DoDrawLine(int x1, int y1, int x2, int y2)
{
	ToScreenCoords(x1, y1);
	ToScreenCoords(x2, y2);
	DrawLineFunc(x1, y1, x2, y2, this);
}

void SoftwarePainter::DoFillShadedRect(int pLeft, int pTop, int pRight, int pBottom)
{
	ToScreenCoords(pLeft, pTop);
	ToScreenCoords(pRight, pBottom);
	FillShadedRectFunc(pLeft, pTop, pRight, pBottom, this);
}

Painter::ImageID SoftwarePainter::AddImage(const Lepra::Canvas* pImage, const Lepra::Canvas* pAlphaBuffer)
{
	int lID = mTextureIDManager.GetFreeId();

	if (lID != 0)
	{
		Texture* lTexture = new Texture;

		if (pAlphaBuffer != 0)
		{
			lTexture->mAlphaBuffer = new Lepra::Canvas(*pAlphaBuffer, true);

			if (lTexture->mAlphaBuffer->GetBitDepth() != Lepra::Canvas::BITDEPTH_8_BIT)
			{
				lTexture->mAlphaBuffer->ConvertToGrayscale(true);
			}
		}
		else if(pImage != 0 && pImage->GetBitDepth() == Lepra::Canvas::BITDEPTH_32_BIT)
		{
			// Extract the alpha channel.
			lTexture->mAlphaBuffer = new Lepra::Canvas(pImage->GetWidth(), pImage->GetHeight(), Lepra::Canvas::BITDEPTH_8_BIT);
			pImage->GetAlphaChannel(*lTexture->mAlphaBuffer);
		}

		if (pImage != 0)
		{
			lTexture->mColorMap = new Lepra::Canvas(*pImage, true);

			if (GetCanvas() != 0)
			{
				lTexture->mColorMap->ConvertBitDepth(GetCanvas()->GetBitDepth());
			}
		}

		mTextureTable.Insert(lID, lTexture);
	}

	return (ImageID)lID;
}

void SoftwarePainter::UpdateImage(ImageID pImageID, 
				  const Lepra::Canvas* pImage, 
				  const Lepra::Canvas* pAlphaBuffer,
				  UpdateHint pHint)
{
	TextureTable::Iterator lIter = mTextureTable.Find((int)pImageID);

	if (lIter == mTextureTable.End())
	{
		return;
	}

	Texture* lTexture = *lIter;

	if (pHint == UPDATE_ACCURATE)
	{
		// Remove maps.
		if (lTexture->mColorMap != 0)
		{
			delete lTexture->mColorMap;
			lTexture->mColorMap = 0;
		}

		if (lTexture->mAlphaBuffer != 0)
		{
			delete lTexture->mAlphaBuffer;
			lTexture->mAlphaBuffer = 0;
		}

		if (pAlphaBuffer != 0)
		{
			lTexture->mAlphaBuffer = new Lepra::Canvas(*pAlphaBuffer, true);

			if (lTexture->mAlphaBuffer->GetBitDepth() != Lepra::Canvas::BITDEPTH_8_BIT)
			{
				lTexture->mAlphaBuffer->ConvertToGrayscale(true);
			}
		}
		else if(pImage != 0 && pImage->GetBitDepth() == Lepra::Canvas::BITDEPTH_32_BIT)
		{
			// Extract the alpha channel.
			lTexture->mAlphaBuffer = new Lepra::Canvas(pImage->GetWidth(), pImage->GetHeight(), Lepra::Canvas::BITDEPTH_8_BIT);
			pImage->GetAlphaChannel(*lTexture->mAlphaBuffer);
		}

		if (pImage != 0)
		{
			lTexture->mColorMap = new Lepra::Canvas(*pImage, true);

			if (GetCanvas() != 0)
			{
				lTexture->mColorMap->ConvertBitDepth(GetCanvas()->GetBitDepth());
			}
		}
	}
	else if(pImage != 0)	// pHint == UPDATE_FAST
	{
		if (lTexture->mColorMap == 0)
		{
			lTexture->mColorMap = new Lepra::Canvas(*pImage, true);
		}
		else
		{
			unsigned lCMapSize  = lTexture->mColorMap->GetWidth() *
								   lTexture->mColorMap->GetHeight() *
								   lTexture->mColorMap->GetPixelByteSize();
			unsigned lImageSize = pImage->GetWidth() * pImage->GetHeight() * pImage->GetPixelByteSize();

			if (lImageSize < lCMapSize)
			{
				memcpy(lTexture->mColorMap->GetBuffer(), pImage->GetBuffer(), lImageSize);
			}
			else
			{
				memcpy(lTexture->mColorMap->GetBuffer(), pImage->GetBuffer(), lCMapSize);
			}
		}

		if (GetCanvas() != 0)
		{
			lTexture->mColorMap->ConvertBitDepth(GetCanvas()->GetBitDepth());
		}
	}
}

void SoftwarePainter::RemoveImage(ImageID pImageID)
{
	TextureTable::Iterator lIter = mTextureTable.Find((int)pImageID);

	if (lIter != mTextureTable.End())
	{
		Texture* lTexture = *lIter;
		mTextureTable.Remove(lIter);

		if (lTexture->mColorMap != 0)
		{
			delete lTexture->mColorMap;
		}

		if (lTexture->mAlphaBuffer != 0)
		{
			delete lTexture->mAlphaBuffer;
		}

		delete lTexture;

		mTextureIDManager.RecycleId(pImageID);
	}
}

void SoftwarePainter::DoDrawImage(ImageID pImageID, int x, int y)
{
	ToScreenCoords(x, y);

	TextureTable::Iterator lIter = mTextureTable.Find(pImageID);

	if (lIter == mTextureTable.End())
	{
		return;
	}

	Texture* lTexture = *lIter;
	if (lTexture->mColorMap == 0)
	{
		return;
	}

	Lepra::Canvas* lImage = lTexture->mColorMap;

	if (lImage->GetBitDepth() == GetCanvas()->GetBitDepth())
	{
		Lepra::PixelRect lClippingRect;
		GetScreenCoordClippingRect(lClippingRect);
		if (  x < (lClippingRect.mLeft - (int)lImage->GetWidth()) || 
			 x >= lClippingRect.mRight ||
			 y < (lClippingRect.mTop - (int)lImage->GetHeight()) ||
			 y >= lClippingRect.mBottom)
		{
			return;
		}
		
		Lepra::PixelRect lRect(0, 0, lImage->GetWidth(), lImage->GetHeight());

		Lepra::Canvas* lAlpha = lTexture->mAlphaBuffer;

		if (lAlpha != 0)
		{
			DrawImageFunc(*lImage, lAlpha, x, y, lRect, this);
		}
		else
		{
			DrawImageFallback(*lImage, 0, x, y, lRect, this);
		}
	}
}

void SoftwarePainter::DoDrawImage(ImageID pImageID, int x, int y, const Lepra::PixelRect& pSubpatchRect)
{
	ToScreenCoords(x, y);

	TextureTable::Iterator lIter = mTextureTable.Find(pImageID);

	if (lIter == mTextureTable.End())
	{
		return;
	}

	Texture* lTexture = *lIter;
	if (lTexture->mColorMap == 0)
	{
		return;
	}

	Lepra::Canvas* lImage = lTexture->mColorMap;

	if (lImage->GetBitDepth() == GetCanvas()->GetBitDepth())
	{
		Lepra::PixelRect lClippingRect;
		GetScreenCoordClippingRect(lClippingRect);
		if (  x < (lClippingRect.mLeft - (int)pSubpatchRect.GetWidth()) || 
			 x >= lClippingRect.mRight ||
			 y < (lClippingRect.mTop - (int)pSubpatchRect.GetHeight()) ||
			 y >= lClippingRect.mBottom)
		{
			return;
		}
		
		Lepra::Canvas* lAlpha = lTexture->mAlphaBuffer;

		if (lAlpha != 0)
		{
			DrawImageFunc(*lImage, lAlpha, x, y, pSubpatchRect, this);
		}
		else
		{
			DrawImageFallback(*lImage, 0, x, y, pSubpatchRect, this);
		}
	}
}

// Stretches the Image...
void SoftwarePainter::DoDrawImage(ImageID pImageID, const Lepra::PixelRect& pRect)
{
	TextureTable::Iterator lIter = mTextureTable.Find(pImageID);

	if (lIter == mTextureTable.End())
	{
		return;
	}

	Texture* lTexture = *lIter;
	if (lTexture->mColorMap == 0)
	{
		return;
	}

	Lepra::PixelRect lRect(pRect);
	ToScreenCoords(lRect.mLeft, lRect.mTop);
	ToScreenCoords(lRect.mRight, lRect.mBottom);

	Lepra::Canvas* lImage = lTexture->mColorMap;

	if (lImage->GetBitDepth() == GetCanvas()->GetBitDepth())
	{
		Lepra::PixelRect lClippingRect;
		GetScreenCoordClippingRect(lClippingRect);
		if (  lRect.mLeft < (lClippingRect.mLeft - lRect.GetWidth()) || 
			 lRect.mLeft >= lClippingRect.mRight ||
			 lRect.mTop  < (lClippingRect.mTop - lRect.GetHeight()) ||
			 lRect.mTop  >= lClippingRect.mBottom ||
			 lRect.mLeft >= lRect.mRight || 
			 lRect.mTop  >= lRect.mBottom)
		{
			return;
		}

		Lepra::Canvas* lAlpha = lTexture->mAlphaBuffer;
		Lepra::PixelRect lSubRect(0, 0, lImage->GetWidth(), lImage->GetHeight());

		if (lAlpha != 0)
		{
			DrawImageStretchFunc(*lImage, lAlpha, lRect, lSubRect, this);
		}
		else
		{
			DrawImageStretchFallback(*lImage, 0, lRect, lSubRect, this);
		}
	}
}

// Stretches the Image...
void SoftwarePainter::DoDrawImage(ImageID pImageID, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubpatchRect)
{
	TextureTable::Iterator lIter = mTextureTable.Find(pImageID);

	if (lIter == mTextureTable.End())
	{
		return;
	}

	Texture* lTexture = *lIter;
	if (lTexture->mColorMap == 0)
	{
		return;
	}

	Lepra::PixelRect lRect(pRect);
	ToScreenCoords(lRect.mLeft, lRect.mTop);
	ToScreenCoords(lRect.mRight, lRect.mBottom);

	Lepra::Canvas* lImage = lTexture->mColorMap;

	if (lImage->GetBitDepth() == GetCanvas()->GetBitDepth())
	{
		Lepra::PixelRect lClippingRect;
		GetScreenCoordClippingRect(lClippingRect);

		if (  lRect.mLeft < (lClippingRect.mLeft - lRect.GetWidth()) || 
			 lRect.mLeft >= lClippingRect.mRight ||
			 lRect.mTop  < (lClippingRect.mTop - lRect.GetHeight()) ||
			 lRect.mTop  >= lClippingRect.mBottom ||
			 lRect.mLeft >= lRect.mRight || 
			 lRect.mTop  >= lRect.mBottom)
		{
			return;
		}

		Lepra::Canvas* lAlpha = lTexture->mAlphaBuffer;

		if (lAlpha != 0)
		{
			DrawImageStretchFunc(*lImage, lAlpha, lRect, pSubpatchRect, this);
		}
		else
		{
			DrawImageStretchFallback(*lImage, 0, lRect, pSubpatchRect, this);
		}
	}
}

void SoftwarePainter::DoDrawAlphaImage(ImageID pImageID, int x, int y)
{
	ToScreenCoords(x, y);

	TextureTable::Iterator lIter = mTextureTable.Find(pImageID);

	if (lIter == mTextureTable.End())
	{
		return;
	}

	Texture* lTexture = *lIter;
	if (lTexture->mAlphaBuffer == 0)
	{
		return;
	}

	Lepra::Canvas* lImage = lTexture->mAlphaBuffer;

	// Alpha buffer must be 8-bit.
	if (lImage->GetBitDepth() == Lepra::Canvas::BITDEPTH_8_BIT)
	{
		Lepra::PixelRect lClippingRect;
		GetScreenCoordClippingRect(lClippingRect);
		if (  x < (lClippingRect.mLeft - (int)lImage->GetWidth()) || 
			 x >= lClippingRect.mRight ||
			 y < (lClippingRect.mTop - (int)lImage->GetHeight()) ||
			 y >= lClippingRect.mBottom)
		{
			return;
		}
		
		DrawAlphaImageFunc(*lImage, x, y, this);
	}
}

int SoftwarePainter::DoPrintText(const Lepra::String& pString, int x, int y)
{
	ToScreenCoords(x, y);

	int lCurrentX = x;
	int lCurrentY = y;

	SoftwareFont* lFont = (SoftwareFont*)GetCurrentFontInternal();
	Lepra::Canvas* lColorMap = lFont->mTexture->mColorMap;
	Lepra::Canvas* lAlphaBuffer = lFont->mTexture->mAlphaBuffer;

	for (size_t i = 0; i < pString.length(); i++)
	{
		Lepra::utchar lChar = (Lepra::utchar)pString[i];
		assert(lChar >= lFont->mFirstChar && lChar <= lFont->mLastChar);

		if (lChar == _T('\n'))
		{
			lCurrentY += (lFont->mCharHeight + lFont->mNewLineOffset);
			lCurrentX = x;
		}
		else if(lChar != _T('\r') && 
			lChar != _T('\b') &&
			lChar != _T('\t'))
		{
			if (lFont->mAlphaImage == true)
			{
				DrawAlphaImageFunc(lFont->mChar[lChar - lFont->mFirstChar], lCurrentX, lCurrentY, this);
			}
			else
			{
				if (lAlphaBuffer != 0)
				{
					DrawImageFunc(*lColorMap, lAlphaBuffer, lCurrentX, lCurrentY, lFont->mCharRect[lChar - lFont->mFirstChar], this);
				}
				else
				{
					DrawImageFallback(*lColorMap, lAlphaBuffer, lCurrentX, lCurrentY, lFont->mCharRect[lChar - lFont->mFirstChar], this);
				}
			}

			lCurrentX += lFont->mCharWidth[lChar - lFont->mFirstChar] + lFont->mCharOffset;
		}

		if (lChar == _T(' '))
		{
			lCurrentX += lFont->mDefaultSpaceWidth;
		}
		else if(lChar == _T('\t'))
		{
			lCurrentX = GetTabOriginX() + (((lCurrentX - GetTabOriginX()) / lFont->mTabWidth) + 1) * lFont->mTabWidth;
		}
	}

	int lTemp = 0;
	ToUserCoords(lCurrentX, lTemp);
	return lCurrentX;
}

void SoftwarePainter::DoFillTriangle(float pX0, float pY0, 
				     float pX1, float pY1, 
				     float pX2, float pY2)
{
	ToScreenCoords(pX0, pY0);
	ToScreenCoords(pX1, pY1);
	ToScreenCoords(pX2, pY2);

	Lepra::PixelRect lClippingRect;
	GetScreenCoordClippingRect(lClippingRect);

	Edge::SetClipLeftRight(lClippingRect.mLeft, lClippingRect.mRight);

	if (((pX1 - pX0) *
		(pY2 - pY0) -
		(pX2 - pX0) *
		(pY1 - pY0)) < 0.0f)
   	{
		// Wrong vertex order. Swap to the correct order.
		float lT = pX0;
		pX0 = pX1;
		pX1 = lT;

		lT = pY0;
		pY0 = pY1;
		pY1 = lT;
	}

	Vertex2D lV0(pX0, pY0);
	Vertex2D lV1(pX1, pY1);
	Vertex2D lV2(pX2, pY2);

	Edge lEdge0;
	Edge lEdge1;
	Edge lEdge2;
	lEdge0.Init(&lV0, &lV1);
	lEdge1.Init(&lV1, &lV2);
	lEdge2.Init(&lV2, &lV0);

	Edge* lTopToBottomEdge;
	Edge* lTopToMiddleEdge;
	Edge* lMiddleToBottomEdge;

	bool lMiddleIsLeft;

	// Sort edges.
	if (pY0 < pY1)
	{
		if (pY2 < pY0)
		{
			lTopToBottomEdge    = &lEdge1;
			lTopToMiddleEdge    = &lEdge2;
			lMiddleToBottomEdge = &lEdge0;
			lMiddleIsLeft = false;
		}
		else
		{
			if (pY1 < pY2)
			{
				lTopToBottomEdge    = &lEdge2;
				lTopToMiddleEdge    = &lEdge0;
				lMiddleToBottomEdge = &lEdge1;
				lMiddleIsLeft = false;
			}
			else
			{
				lTopToBottomEdge    = &lEdge0;
				lTopToMiddleEdge    = &lEdge2;
				lMiddleToBottomEdge = &lEdge1;
				lMiddleIsLeft = true;
			}
		}
	}
	else
	{
		if (pY2 < pY1)
		{
			lTopToBottomEdge    = &lEdge2;
			lTopToMiddleEdge    = &lEdge1;
			lMiddleToBottomEdge = &lEdge0;
			lMiddleIsLeft = true;
		}
		else
		{

			if (pY0 < pY2)
			{
				lTopToBottomEdge    = &lEdge1;
				lTopToMiddleEdge    = &lEdge0;
				lMiddleToBottomEdge = &lEdge2;
				lMiddleIsLeft = true;
			}
			else
			{
				lTopToBottomEdge    = &lEdge0;
				lTopToMiddleEdge    = &lEdge1;
				lMiddleToBottomEdge = &lEdge2;
				lMiddleIsLeft = false;
			}
		}
	}

	Edge* lLeftEdge;
	Edge* lRightEdge;

	// Prepare top triangle.
	if (lMiddleIsLeft == true)
	{
		lLeftEdge  = lTopToMiddleEdge;
		lRightEdge = lTopToBottomEdge;
	}
	else
	{
		lLeftEdge  = lTopToBottomEdge;
		lRightEdge = lTopToMiddleEdge;
	}

	if (lLeftEdge->GetY() < lClippingRect.mTop)
	{
		unsigned lStepCount = (unsigned)(lClippingRect.mTop - lLeftEdge->GetY());
		lLeftEdge->Step(lStepCount);
		lRightEdge->Step(lStepCount);
	}

	int lHeight = lTopToMiddleEdge->GetHeight();

	if ((lLeftEdge->GetY() + lHeight) > lClippingRect.mBottom)
	{
		lHeight -= ((lLeftEdge->GetY() + lHeight) - lClippingRect.mBottom);
	}

	// Render top triangle.
	while (lHeight-- > 0)
	{
		DrawHLineFunc(lLeftEdge->GetX(), lRightEdge->GetX() - 1, lLeftEdge->GetY(), 0, this);
		lLeftEdge->Step();
		lRightEdge->Step();
	}

	// Prepare bottom triangle.
	if (lMiddleIsLeft == true)
	{
		lLeftEdge  = lMiddleToBottomEdge;
		lRightEdge = lTopToBottomEdge;
	}
	else
	{
		lLeftEdge  = lTopToBottomEdge;
		lRightEdge = lMiddleToBottomEdge;
	}

	if (lLeftEdge->GetY() < lClippingRect.mTop)
	{
		unsigned lStepCount = (unsigned)(lClippingRect.mTop - lLeftEdge->GetY());
		lLeftEdge->Step(lStepCount);
	}
	if (lRightEdge->GetY() < lClippingRect.mTop)
	{
		unsigned lStepCount = (unsigned)(lClippingRect.mTop - lRightEdge->GetY());
		lRightEdge->Step(lStepCount);
	}

	lHeight = lMiddleToBottomEdge->GetHeight();

	if ((lLeftEdge->GetY() + lHeight) > lClippingRect.mBottom)
	{
		lHeight -= ((lLeftEdge->GetY() + lHeight) - lClippingRect.mBottom);
	}

	// Render bottom triangle.
	while (lHeight-- > 0)
	{
		DrawHLineFunc(lLeftEdge->GetX(), lRightEdge->GetX() - 1, lLeftEdge->GetY(), 0, this);
		lLeftEdge->Step();
		lRightEdge->Step();
	}
}

void SoftwarePainter::DoFillShadedTriangle(float pX0, float pY0,
					   float pX1, float pY1,
					   float pX2, float pY2)
{
	ToScreenCoords(pX0, pY0);
	ToScreenCoords(pX1, pY1);
	ToScreenCoords(pX2, pY2);

	Lepra::PixelRect lClippingRect;
	GetScreenCoordClippingRect(lClippingRect);

	Edge::SetClipLeftRight(lClippingRect.mLeft, lClippingRect.mRight);

	Lepra::Color* lColor[3];
	lColor[0] = &GetColorInternal(0);
	lColor[1] = &GetColorInternal(1);
	lColor[2] = &GetColorInternal(2);

	if (((pX1 - pX0) *
		(pY2 - pY0) -
		(pX2 - pX0) *
		(pY1 - pY0)) < 0.0f)
   	{
		// Wrong vertex order. Swap to the correct order.
		float lT = pX0;
		pX0 = pX1;
		pX1 = lT;

		lT = pY0;
		pY0 = pY1;
		pY1 = lT;

		lColor[0] = &GetColorInternal(1);
		lColor[1] = &GetColorInternal(0);
	}

	Vertex2DRGBA lV0(pX0, pY0,
			 lColor[0]->mRed   / 255.0f,
			 lColor[0]->mGreen / 255.0f,
			 lColor[0]->mBlue  / 255.0f,
			 lColor[0]->mAlpha / 255.0f);
	Vertex2DRGBA lV1(pX1, pY1,
			 lColor[1]->mRed   / 255.0f,
			 lColor[1]->mGreen / 255.0f,
			 lColor[1]->mBlue  / 255.0f,
			 lColor[1]->mAlpha / 255.0f);
	Vertex2DRGBA lV2(pX2, pY2,
			 lColor[2]->mRed   / 255.0f,
			 lColor[2]->mGreen / 255.0f,
			 lColor[2]->mBlue  / 255.0f,
			 lColor[2]->mAlpha / 255.0f);

	Gradients2DRGBA lGradients(&lV0, &lV1, &lV2);

	Edge2DRGBA lEdge0;
	Edge2DRGBA lEdge1;
	Edge2DRGBA lEdge2;
	lEdge0.Init(&lV0, &lV1, &lGradients);
	lEdge1.Init(&lV1, &lV2, &lGradients);
	lEdge2.Init(&lV2, &lV0, &lGradients);

	Edge2DRGBA* lTopToBottomEdge;
	Edge2DRGBA* lTopToMiddleEdge;
	Edge2DRGBA* lMiddleToBottomEdge;

	bool lMiddleIsLeft;

	// Sort edges.
	if (pY0 < pY1)
	{
		if (pY2 < pY0)
		{
			lTopToBottomEdge    = &lEdge1;
			lTopToMiddleEdge    = &lEdge2;
			lMiddleToBottomEdge = &lEdge0;
			lMiddleIsLeft = false;
		}
		else
		{
			if (pY1 < pY2)
			{
				lTopToBottomEdge    = &lEdge2;
				lTopToMiddleEdge    = &lEdge0;
				lMiddleToBottomEdge = &lEdge1;
				lMiddleIsLeft = false;
			}
			else
			{
				lTopToBottomEdge    = &lEdge0;
				lTopToMiddleEdge    = &lEdge2;
				lMiddleToBottomEdge = &lEdge1;
				lMiddleIsLeft = true;
			}
		}
	}
	else
	{
		if (pY2 < pY1)
		{
			lTopToBottomEdge    = &lEdge2;
			lTopToMiddleEdge    = &lEdge1;
			lMiddleToBottomEdge = &lEdge0;
			lMiddleIsLeft = true;
		}
		else
		{

			if (pY0 < pY2)
			{
				lTopToBottomEdge    = &lEdge1;
				lTopToMiddleEdge    = &lEdge0;
				lMiddleToBottomEdge = &lEdge2;
				lMiddleIsLeft = true;
			}
			else
			{
				lTopToBottomEdge    = &lEdge0;
				lTopToMiddleEdge    = &lEdge1;
				lMiddleToBottomEdge = &lEdge2;
				lMiddleIsLeft = false;
			}
		}
	}

	Edge2DRGBA* lLeftEdge;
	Edge2DRGBA* lRightEdge;

	// Prepare top triangle.
	if (lMiddleIsLeft == true)
	{
		lLeftEdge  = lTopToMiddleEdge;
		lRightEdge = lTopToBottomEdge;
	}
	else
	{
		lLeftEdge  = lTopToBottomEdge;
		lRightEdge = lTopToMiddleEdge;
	}

	if (lLeftEdge->GetY() < lClippingRect.mTop)
	{
		unsigned lStepCount = (unsigned)(lClippingRect.mTop - lLeftEdge->GetY());
		lLeftEdge->Step(lStepCount);
		lRightEdge->Step(lStepCount);
	}

	int lHeight = lTopToMiddleEdge->GetHeight();

	if ((lLeftEdge->GetY() + lHeight) > lClippingRect.mBottom)
	{
		lHeight -= ((lLeftEdge->GetY() + lHeight) - lClippingRect.mBottom);
	}

	// Render top triangle.
	while (lHeight-- > 0)
	{
		DrawShadedHLineFunc(lLeftEdge->GetX(), lRightEdge->GetX(), lLeftEdge->GetY(), 
					lLeftEdge->GetR(), lLeftEdge->GetG(), lLeftEdge->GetB(), lLeftEdge->GetA(),
					lRightEdge->GetR(), lRightEdge->GetG(), lRightEdge->GetB(), lRightEdge->GetA(),
					this);
		lLeftEdge->Step();
		lRightEdge->Step();
	}

	// Prepare bottom triangle.
	if (lMiddleIsLeft == true)
	{
		lLeftEdge  = lMiddleToBottomEdge;
		lRightEdge = lTopToBottomEdge;
	}
	else
	{
		lLeftEdge  = lTopToBottomEdge;
		lRightEdge = lMiddleToBottomEdge;
	}

	if (lLeftEdge->GetY() < lClippingRect.mTop)
	{
		unsigned lStepCount = (unsigned)(lClippingRect.mTop - lLeftEdge->GetY());
		lLeftEdge->Step(lStepCount);
	}
	if (lRightEdge->GetY() < lClippingRect.mTop)
	{
		unsigned lStepCount = (unsigned)(lClippingRect.mTop - lRightEdge->GetY());
		lRightEdge->Step(lStepCount);
	}

	lHeight = lMiddleToBottomEdge->GetHeight();

	if ((lLeftEdge->GetY() + lHeight) > lClippingRect.mBottom)
	{
		lHeight -= ((lLeftEdge->GetY() + lHeight) - lClippingRect.mBottom);
	}

	// Render bottom triangle.
	while (lHeight-- > 0)
	{
		DrawShadedHLineFunc(lLeftEdge->GetX(), lRightEdge->GetX(), lLeftEdge->GetY(),
					lLeftEdge->GetR(), lLeftEdge->GetG(), lLeftEdge->GetB(), lLeftEdge->GetA(),
					lRightEdge->GetR(), lRightEdge->GetG(), lRightEdge->GetB(), lRightEdge->GetA(),
					this);
		lLeftEdge->Step();
		lRightEdge->Step();
	}
}

void SoftwarePainter::DoFillTriangle(float pX0, float pY0, float pU0, float pV0,
				     float pX1, float pY1, float pU1, float pV1,
				     float pX2, float pY2, float pU2, float pV2,
				     ImageID pImageID)
{
	ToScreenCoords(pX0, pY0);
	ToScreenCoords(pX1, pY1);
	ToScreenCoords(pX2, pY2);

	Lepra::PixelRect lClippingRect;
	GetScreenCoordClippingRect(lClippingRect);

	TextureTable::Iterator lIter = mTextureTable.Find(pImageID);

	if (lIter == mTextureTable.End())
	{
		return;
	}

	Texture* lTexture = *lIter;
	if (lTexture->mColorMap == 0)
	{
		return;
	}

	Lepra::Canvas* lImage = lTexture->mColorMap;
	Lepra::Canvas* lAlpha  = lTexture->mAlphaBuffer;

	if (lImage == 0 || lImage->GetBitDepth() != GetCanvas()->GetBitDepth())
	{
		return;
	}

	unsigned lExp = Painter::GetExponent(lImage->GetPitch());
	bool lPow2 = false;

	if (((unsigned)1 << lExp) == lImage->GetPitch() && lImage->GetBitDepth() != Lepra::Canvas::BITDEPTH_24_BIT)
	{
		lPow2 = true;
	}

	void (*DrawSLFunc) (const Gradients2DUV*, Edge2DUV*, Edge2DUV*, const Lepra::Canvas*, const Lepra::Canvas*, SoftwarePainter*);
	void (*DrawSLPow2Func) (const Gradients2DUV*, Edge2DUV*, Edge2DUV*, const Lepra::Canvas*, const Lepra::Canvas*, unsigned, SoftwarePainter*);
	DrawSLFunc = DrawScanLineFunc;
	DrawSLPow2Func = DrawScanLinePow2Func;

	if (lAlpha == 0)
	{
		DrawSLFunc = DrawScanLineFuncFallback;
		DrawSLPow2Func = DrawScanLinePow2FuncFallback;
	}

	Edge::SetClipLeftRight(lClippingRect.mLeft, lClippingRect.mRight);

	if (((pX1 - pX0) *
		(pY2 - pY0) -
		(pX2 - pX0) *
		(pY1 - pY0)) < 0.0f)
   	{
		// Wrong vertex order. Swap to the correct order.
		float lT = pX0;
		pX0 = pX1;
		pX1 = lT;

		lT = pY0;
		pY0 = pY1;
		pY1 = lT;

		lT = pU0;
		pU0 = pU1;
		pU1 = lT;

		lT = pV0;
		pV0 = pV1;
		pV1 = lT;
	}

	float lTW = lImage->GetWidth() - 0.5f;
	float lTH = lImage->GetHeight() - 0.5f;

	Vertex2DUV lV0(pX0, pY0, pU0 * lTW, pV0 * lTH);
	Vertex2DUV lV1(pX1, pY1, pU1 * lTW, pV1 * lTH);
	Vertex2DUV lV2(pX2, pY2, pU2 * lTW, pV2 * lTH);

	Gradients2DUV lGradients(&lV0, &lV1, &lV2);

	Edge2DUV lEdge0;
	Edge2DUV lEdge1;
	Edge2DUV lEdge2;
	lEdge0.Init(&lV0, &lV1, &lGradients);
	lEdge1.Init(&lV1, &lV2, &lGradients);
	lEdge2.Init(&lV2, &lV0, &lGradients);

	Edge2DUV* lTopToBottomEdge;
	Edge2DUV* lTopToMiddleEdge;
	Edge2DUV* lMiddleToBottomEdge;

	bool lMiddleIsLeft;

	// Sort edges.
	if (pY0 < pY1)
	{
		if (pY2 < pY0)
		{
			lTopToBottomEdge    = &lEdge1;
			lTopToMiddleEdge    = &lEdge2;
			lMiddleToBottomEdge = &lEdge0;
			lMiddleIsLeft = false;
		}
		else
		{
			if (pY1 < pY2)
			{
				lTopToBottomEdge    = &lEdge2;
				lTopToMiddleEdge    = &lEdge0;
				lMiddleToBottomEdge = &lEdge1;
				lMiddleIsLeft = false;
			}
			else
			{
				lTopToBottomEdge    = &lEdge0;
				lTopToMiddleEdge    = &lEdge2;
				lMiddleToBottomEdge = &lEdge1;
				lMiddleIsLeft = true;
			}
		}
	}
	else
	{
		if (pY2 < pY1)
		{
			lTopToBottomEdge    = &lEdge2;
			lTopToMiddleEdge    = &lEdge1;
			lMiddleToBottomEdge = &lEdge0;
			lMiddleIsLeft = true;
		}
		else
		{

			if (pY0 < pY2)
			{
				lTopToBottomEdge    = &lEdge1;
				lTopToMiddleEdge    = &lEdge0;
				lMiddleToBottomEdge = &lEdge2;
				lMiddleIsLeft = true;
			}
			else
			{
				lTopToBottomEdge    = &lEdge0;
				lTopToMiddleEdge    = &lEdge1;
				lMiddleToBottomEdge = &lEdge2;
				lMiddleIsLeft = false;
			}
		}
	}

	Edge2DUV* lLeftEdge;
	Edge2DUV* lRightEdge;

	// Prepare top triangle.
	if (lMiddleIsLeft == true)
	{
		lLeftEdge  = lTopToMiddleEdge;
		lRightEdge = lTopToBottomEdge;
	}
	else
	{
		lLeftEdge  = lTopToBottomEdge;
		lRightEdge = lTopToMiddleEdge;
	}

	if (lLeftEdge->GetY() < lClippingRect.mTop)
	{
		unsigned lStepCount = (unsigned)(lClippingRect.mTop - lLeftEdge->GetY());
		lLeftEdge->Step(lStepCount);
		lRightEdge->Step(lStepCount);
	}

	int lHeight = lTopToMiddleEdge->GetHeight();

	if ((lLeftEdge->GetY() + lHeight) >= lClippingRect.mBottom)
	{
		lHeight -= ((lLeftEdge->GetY() + lHeight) - lClippingRect.mBottom);
	}

	// Render top triangle.
	if (lPow2 == true)
	{
		while (lHeight-- > 0)
		{
			DrawSLPow2Func(&lGradients, lLeftEdge, lRightEdge, lImage, lAlpha, lExp, this);
			lLeftEdge->Step();
			lRightEdge->Step();
		}
	}
	else
	{
		while (lHeight-- > 0)
		{
			DrawSLFunc(&lGradients, lLeftEdge, lRightEdge, lImage, lAlpha, this);
			lLeftEdge->Step();
			lRightEdge->Step();
		}
	}

	// Prepare bottom triangle.
	if (lMiddleIsLeft == true)
	{
		lLeftEdge  = lMiddleToBottomEdge;
		lRightEdge = lTopToBottomEdge;
	}
	else
	{
		lLeftEdge  = lTopToBottomEdge;
		lRightEdge = lMiddleToBottomEdge;
	}

	if (lLeftEdge->GetY() < lClippingRect.mTop)
	{
		unsigned lStepCount = (unsigned)(lClippingRect.mTop - lLeftEdge->GetY());
		lLeftEdge->Step(lStepCount);
	}
	if (lRightEdge->GetY() < lClippingRect.mTop)
	{
		unsigned lStepCount = (unsigned)(lClippingRect.mTop - lRightEdge->GetY());
		lRightEdge->Step(lStepCount);
	}

	lHeight = lMiddleToBottomEdge->GetHeight();

	if ((lLeftEdge->GetY() + lHeight) >= lClippingRect.mBottom)
	{
		lHeight -= ((lLeftEdge->GetY() + lHeight) - lClippingRect.mBottom) + 1;
	}

	// Render bottom triangle.
	if (lPow2 == true)
	{
		while (lHeight-- > 0)
		{
			DrawSLPow2Func(&lGradients, lLeftEdge, lRightEdge, lImage, lAlpha, lExp, this);
			lLeftEdge->Step();
			lRightEdge->Step();
		}
	}
	else 
	{
		while (lHeight-- > 0)
		{
			DrawSLFunc(&lGradients, lLeftEdge, lRightEdge, lImage, lAlpha, this);
			lLeftEdge->Step();
			lRightEdge->Step();
		}
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	IMPLEMENTATION OF Rect-functions.
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////


void SoftwarePainter::DoDrawRect(int pLeft, int pTop, int pRight, int pBottom, int pWidth)
{
	ToScreenCoords(pLeft, pTop);
	ToScreenCoords(pRight, pBottom);

	Lepra::Color lColor = GetColorInternal(0);
	int lRed   = lColor.mRed;
	int lGreen = lColor.mGreen;
	int lBlue  = lColor.mBlue;

	// Use fixed point arithmetic with 9 bits integer part and 23 bits fractional.
	int lRStep = ((GetColor(1).mRed   - lRed)   << 23) / pWidth;
	int lGStep = ((GetColor(1).mGreen - lGreen) << 23) / pWidth;
	int lBStep = ((GetColor(1).mBlue  - lBlue)  << 23) / pWidth;

	lRed   <<= 23;
	lGreen <<= 23;
	lBlue  <<= 23;

	for(int i = 0; i < pWidth; i++)
	{
		SetColor(Lepra::Color(Lepra::uint8(lRed >> 23), Lepra::uint8(lGreen >> 23), Lepra::uint8(lBlue >> 23)), 0);

		DrawHLineFunc(pLeft, pRight, pTop, 0, this);
		DrawHLineFunc(pLeft, pRight, pBottom - 1, 0, this);
		DrawVLineFunc(pTop, pBottom, pLeft, 0, this);
		DrawVLineFunc(pTop, pBottom, pRight - 1, 0, this);

		pLeft++;
		pTop++;
		pRight--;
		pBottom--;

		lRed   += lRStep;
		lGreen += lGStep;
		lBlue  += lBStep;
	}

	// Reset the color.
	SetColor(lColor, 0);
}

void SoftwarePainter::DoFillRect(int pLeft, int pTop, int pRight, int pBottom)
{
	ToScreenCoords(pLeft, pTop);
	ToScreenCoords(pRight, pBottom);

	Lepra::PixelRect lClippingRect;
	GetScreenCoordClippingRect(lClippingRect);

	if (
		pTop > lClippingRect.mBottom || 
		pBottom <= lClippingRect.mTop ||
		pLeft > lClippingRect.mRight ||
		pRight <= lClippingRect.mLeft
	  )
	{
		return;
	}

	if (pLeft < lClippingRect.mLeft)
	{
		pLeft = lClippingRect.mLeft;
	}
	if (pRight > lClippingRect.mRight)
	{
		pRight = lClippingRect.mRight;
	}
	if (pTop < lClippingRect.mTop)
	{
		pTop = lClippingRect.mTop;
	}
	if (pBottom > lClippingRect.mBottom)
	{
		pBottom = lClippingRect.mBottom;
	}

	for (int y = pTop; y < pBottom; y++)
	{
		DrawHLineFunc(pLeft, pRight, y, 0, this);
	}
}

void SoftwarePainter::DoDraw3DRect(int pLeft, int pTop, int pRight, int pBottom, int pWidth, bool pSunken)
{
	ToScreenCoords(pLeft, pTop);
	ToScreenCoords(pRight, pBottom);

	int lZero  = 0;
	int lOne   = 1;
	int lTwo   = 2;
	int lThree = 3;

	if(pSunken)
	{
		lZero  = 2;
		lOne   = 3;
		lTwo   = 0;
		lThree = 1;
	}

	// Top left outer color.
	int lRTL = GetColor(lZero).mRed;
	int lGTL = GetColor(lZero).mGreen;
	int lBTL = GetColor(lZero).mBlue;

	// Bottom right outer color.
	int lRBR = GetColor(lOne).mRed;
	int lGBR = GetColor(lOne).mGreen;
	int lBBR = GetColor(lOne).mBlue;

	// Use fixed point arithmetic with 9 bits integer part and 23 bits fractional.
	int lRTLStep = ((GetColor(lTwo).mRed   - lRTL) << 23) / pWidth;
	int lGTLStep = ((GetColor(lTwo).mGreen - lGTL) << 23) / pWidth;
	int lBTLStep = ((GetColor(lTwo).mBlue  - lBTL) << 23) / pWidth;

	int lRBRStep = ((GetColor(lThree).mRed   - lRBR) << 23) / pWidth;
	int lGBRStep = ((GetColor(lThree).mGreen - lGBR) << 23) / pWidth;
	int lBBRStep = ((GetColor(lThree).mBlue  - lBBR) << 23) / pWidth;

	lRTL <<= 23;
	lGTL <<= 23;
	lBTL <<= 23;

	lRBR <<= 23;
	lGBR <<= 23;
	lBBR <<= 23;

	Lepra::Color lColor0(GetColorInternal(0));

	for(int i = 0; i < pWidth; i++)
	{
		SetColor(Lepra::Color(Lepra::uint8(lRTL >> 23), Lepra::uint8(lGTL >> 23), Lepra::uint8(lBTL >> 23)), 0);
		DrawHLineFunc(pLeft, pRight, pTop, 0, this);
		DrawVLineFunc(pTop, pBottom, pLeft, 0, this);

		SetColor(Lepra::Color(Lepra::uint8(lRBR >> 23), Lepra::uint8(lGBR >> 23), Lepra::uint8(lBBR >> 23)), 0);
		DrawHLineFunc(pLeft, pRight, pBottom - 1, 0, this);
		DrawVLineFunc(pTop, pBottom, pRight - 1, 0, this);

		pLeft++;
		pTop++;
		pRight--;
		pBottom--;

		lRTL += lRTLStep;
		lGTL += lGTLStep;
		lBTL += lBTLStep;

		lRBR += lRBRStep;
		lGBR += lGBRStep;
		lBBR += lBBRStep;
	}

	// Reset color.
	SetColor(lColor0);
}

void SoftwarePainter::ReadPixels(Lepra::Canvas& pDestCanvas, const Lepra::PixelRect& pRect)
{
	if (GetCanvas() == 0)
	{
		return;
	}

	Lepra::PixelRect lRect(pRect);
	ToScreenCoords(lRect.mLeft, lRect.mTop);
	ToScreenCoords(lRect.mRight, lRect.mBottom);

	const unsigned lWidth  = lRect.GetWidth();
	const unsigned lHeight = lRect.GetHeight();

	if (pDestCanvas.GetBitDepth() != GetCanvas()->GetBitDepth() ||
	   pDestCanvas.GetWidth() != lWidth ||
	   pDestCanvas.GetHeight() != lHeight)
	{
		pDestCanvas.Reset(lWidth, lHeight, GetCanvas()->GetBitDepth());
		pDestCanvas.CreateBuffer();
	}

	switch(GetCanvas()->GetBitDepth())
	{
		case Lepra::Canvas::BITDEPTH_8_BIT:
		{
			Lepra::uint8* lDest = (Lepra::uint8*)pDestCanvas.GetBuffer();
			Lepra::uint8* lSrc  = (Lepra::uint8*)GetCanvas()->GetBuffer();
			
			int lDestPitch = pDestCanvas.GetPitch();
			int lSrcPitch  = GetCanvas()->GetPitch();
			int lSpanLength = lRect.GetWidth();

			lSrc += lRect.mTop * lSrcPitch + lRect.mLeft;

			for (unsigned y = 0; y < lHeight; y++)
			{
				memcpy(lDest, lSrc, lSpanLength);

				lDest += lDestPitch;
				lSrc  += lSrcPitch;
			}
			break;
		}
		case Lepra::Canvas::BITDEPTH_15_BIT:
		case Lepra::Canvas::BITDEPTH_16_BIT:
		{
			Lepra::uint16* lDest = (Lepra::uint16*)pDestCanvas.GetBuffer();
			Lepra::uint16* lSrc  = (Lepra::uint16*)GetCanvas()->GetBuffer();
			
			int lDestPitch = pDestCanvas.GetPitch();
			int lSrcPitch  = GetCanvas()->GetPitch();
			int lSpanLength = lRect.GetWidth() * 2;

			lSrc += lRect.mTop * lSrcPitch + lRect.mLeft;

			for (unsigned y = 0; y < lHeight; y++)
			{
				memcpy(lDest, lSrc, lSpanLength);

				lDest += lDestPitch;
				lSrc  += lSrcPitch;
			}
			break;
		}
		case Lepra::Canvas::BITDEPTH_24_BIT:
		{
			Lepra::uint8* lDest = (Lepra::uint8*)pDestCanvas.GetBuffer();
			Lepra::uint8* lSrc  = (Lepra::uint8*)GetCanvas()->GetBuffer();
			
			int lDestPitch = pDestCanvas.GetPitch() * 3;
			int lSrcPitch  = GetCanvas()->GetPitch() * 3;
			int lSpanLength = lRect.GetWidth() * 3;

			lSrc += lRect.mTop * lSrcPitch + lRect.mLeft * 3;

			for (unsigned y = 0; y < lHeight; y++)
			{
				memcpy(lDest, lSrc, lSpanLength);

				lDest += lDestPitch;
				lSrc  += lSrcPitch;
			}
			break;
		}
		case Lepra::Canvas::BITDEPTH_32_BIT:
		{
			unsigned* lDest = (unsigned*)pDestCanvas.GetBuffer();
			unsigned* lSrc  = (unsigned*)GetCanvas()->GetBuffer();
			
			int lDestPitch = pDestCanvas.GetPitch();
			int lSrcPitch  = GetCanvas()->GetPitch();
			int lSpanLength = lRect.GetWidth() * 4;

			lSrc += lRect.mTop * lSrcPitch + lRect.mLeft;

			for (unsigned y = 0; y < lHeight; y++)
			{
				memcpy(lDest, lSrc, lSpanLength);

				lDest += lDestPitch;
				lSrc  += lSrcPitch;
			}
			break;
		}
	default:
		break;
	}
}

Painter::RGBOrder SoftwarePainter::GetRGBOrder()
{
	return BGR;
}

void SoftwarePainter::DoRenderDisplayList(std::vector<DisplayEntity*>* pDisplayList)
{
	PushAttrib(ATTR_ALL);

	std::vector<DisplayEntity*>::iterator it;
	for(it = pDisplayList->begin(); it != pDisplayList->end(); ++it)
	{
		DisplayEntity* lSE = *it;
		Painter::SetClippingRect(lSE->GetClippingRect());
		SetAlphaValue(lSE->GetAlpha());
		SetRenderMode(lSE->GetRenderMode());

		const Lepra::Vector2DF& lPos = lSE->GetGeometry().GetPos();
		const float* lVertex = (float*)lSE->GetGeometry().GetVertexData();
		const float* lColor = lSE->GetGeometry().GetColorData();
		const float* lUV = lSE->GetGeometry().GetUVData();
		const Lepra::uint32* lTriangle = lSE->GetGeometry().GetTriangleData();
		int lTriangleMax = lSE->GetGeometry().GetTriangleCount() * 3;

		for(int i = 0; i < lTriangleMax; i += 3)
		{
			int lV1 = lTriangle[i + 0];
			int lV2 = lTriangle[i + 1];
			int lV3 = lTriangle[i + 2];

			if(lSE->GetImageID() == INVALID_IMAGEID)
			{
				if(lColor != 0)
				{
					int lC1 = lV1 * 3;
					int lC2 = lV2 * 3;
					int lC3 = lV3 * 3;
					Painter::SetColor((Lepra::uint8)(lColor[lC1 + 0] * 255.0f),
							  (Lepra::uint8)(lColor[lC1 + 1] * 255.0f),
							  (Lepra::uint8)(lColor[lC1 + 2] * 255.0f),
							  255, 0);
					Painter::SetColor((Lepra::uint8)(lColor[lC2 + 0] * 255.0f),
							  (Lepra::uint8)(lColor[lC2 + 1] * 255.0f),
							  (Lepra::uint8)(lColor[lC2 + 2] * 255.0f),
							  255, 1);
					Painter::SetColor((Lepra::uint8)(lColor[lC3 + 0] * 255.0f),
							  (Lepra::uint8)(lColor[lC3 + 1] * 255.0f),
							  (Lepra::uint8)(lColor[lC3 + 2] * 255.0f),
							  255, 2);
					lV1 *= 2;
					lV2 *= 2;
					lV3 *= 2;
					FillShadedTriangle(lVertex[lV1 + 0] + lPos.x, lVertex[lV1 + 1] + lPos.y,
					                   lVertex[lV2 + 0] + lPos.x, lVertex[lV2 + 1] + lPos.y,
					                   lVertex[lV3 + 0] + lPos.x, lVertex[lV3 + 1] + lPos.y);
				}
				else
				{
					lV1 *= 2;
					lV2 *= 2;
					lV3 *= 2;
					FillTriangle(lVertex[lV1 + 0] + lPos.x, lVertex[lV1 + 1] + lPos.y,
					             lVertex[lV2 + 0] + lPos.x, lVertex[lV2 + 1] + lPos.y,
					             lVertex[lV3 + 0] + lPos.x, lVertex[lV3 + 1] + lPos.y);
				}
			}
			else
			{
				lV1 *= 2;
				lV2 *= 2;
				lV3 *= 2;
				FillTriangle(lVertex[lV1 + 0] + lPos.x, lVertex[lV1 + 1] + lPos.y, lUV[lV1 + 0], lUV[lV1 + 1],
					     lVertex[lV2 + 0] + lPos.x, lVertex[lV2 + 1] + lPos.y, lUV[lV2 + 0], lUV[lV2 + 1],
					     lVertex[lV3 + 0] + lPos.x, lVertex[lV3 + 1] + lPos.y, lUV[lV3 + 0], lUV[lV3 + 1],
					     lSE->GetImageID());
			}
		}
	}

	PopAttrib();
}

Painter::Font* SoftwarePainter::NewFont(int pFirstChar, int pLastChar) const
{
	return new SoftwareFont(pFirstChar, pLastChar);
}

void SoftwarePainter::InitFont(Font* pFont, const Lepra::Canvas& pFontImage)
{
	SoftwareFont* lSoftFont = (SoftwareFont*)pFont;

	// Get the texture.
	lSoftFont->mTexture = *mTextureTable.Find(lSoftFont->mTextureID);

	// Pick either color map or alpha buffer as the image buffer.
	Lepra::Canvas* lImage;
	if (lSoftFont->mTexture->mColorMap != 0)
	{
		lImage = lSoftFont->mTexture->mColorMap;
	}
	else
	{
		lImage = lSoftFont->mTexture->mAlphaBuffer;
	}

	int lCharCount = lSoftFont->mLastChar - lSoftFont->mFirstChar + 1;
	int lLeft = 0;
	int lTop  = 0;
	for (int i = 0; i < lCharCount; i++)
	{
		lSoftFont->mChar[i].Reset(lSoftFont->mTileWidth, lSoftFont->mTileHeight, pFontImage.GetBitDepth());
		lSoftFont->mChar[i].SetPitch(pFontImage.GetPitch());
		lSoftFont->mChar[i].SetBuffer((Lepra::uint8*)lImage->GetBuffer() + 
			(lTop * lImage->GetPitch() + lLeft) * lImage->GetPixelByteSize());

		if (lSoftFont->mCharWidth[i] >= 0)
		{
			lLeft += lSoftFont->mTileWidth;

			if ((lLeft + lSoftFont->mTileWidth) > (int)pFontImage.GetWidth())
			{
				lLeft = 0;
				lTop += lSoftFont->mTileHeight;
			}
		}
	}
}

void SoftwarePainter::GetImageSize(ImageID pImageID, int& pWidth, int& pHeight)
{
	pWidth = 0;
	pHeight = 0;

	TextureTable::Iterator lIter = mTextureTable.Find(pImageID);
	if(lIter != mTextureTable.End())
	{
		Texture* lImage = *lIter;
		pWidth = lImage->mColorMap->GetWidth();
		pHeight = lImage->mColorMap->GetHeight();
	}
}

} // End namespace.
