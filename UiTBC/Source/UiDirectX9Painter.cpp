
// Author: Alexander Hugestrand
// Copyright (c) 2002-2006, Alexander Hugestrand



#include "../../Lepra/Include/LepraTarget.h"
#include <math.h>
#include <D3dx9math.h>
#include "../../Lepra/Include/Log.h"
#include "../../UiLepra/Include/Win32/UiWin32DirectXDisplay.h"
#include "../Include/UiDirectX9Painter.h"

#ifdef DEBUG
#pragma comment(lib, "d3dx9d.lib")
#else
#pragma comment(lib, "d3dx9.lib")
#endif



namespace UiTbc
{



DWORD ToArgb(DWORD pAlpha, const Lepra::Color& pColor)
{
	return pAlpha | (((DWORD)pColor.mRed)   << 16) |
	                (((DWORD)pColor.mGreen) << 8) |
	                (((DWORD)pColor.mBlue)  << 0);
}

DirectX9Painter::DirectX9Painter(UiLepra::DisplayManager* pDisplayManager):
	mTextureIDManager(3, 10000, 0),
	mD3DDevice(0),
	mD3DDefaultMouseCursor(0),
	mRenderModeChanged(true)
{
	if (pDisplayManager == 0 || pDisplayManager->GetContextType() != UiLepra::DisplayManager::DIRECTX_CONTEXT)
	{
		mD3DDevice = 0;
	}
	else
	{
		mD3DDevice = ((UiLepra::Win32DirectXDisplay*)pDisplayManager)->GetD3DDevice();
	}
}

DirectX9Painter::~DirectX9Painter()
{
	TextureTable::Iterator lIter = mTextureTable.First();
	while (lIter != mTextureTable.End())
	{
		Texture* lTexture = *lIter;

		int lTextureName = (int)lIter.GetKey();
		mTextureIDManager.RecycleId(lTextureName);

		mTextureTable.Remove(lIter++);
		delete lTexture;
	}
}

void DirectX9Painter::SetDestCanvas(Lepra::Canvas* pCanvas)
{
	Painter::SetDestCanvas(pCanvas);
	ResetClippingRect();
}

void DirectX9Painter::SetRenderMode(RenderMode pRM)
{
	if (pRM != GetRenderMode())
	{
		Painter::SetRenderMode(pRM);
		mRenderModeChanged = true;
	}
}

void DirectX9Painter::UpdateRenderMode()
{
	if (mRenderModeChanged == true)
	{
		switch(GetRenderMode())
		{
			case Painter::RM_ALPHATEST:
			{
				mD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
				mD3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
				mD3DDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
				mD3DDevice->SetRenderState(D3DRS_ALPHAREF, GetAlphaValue());
				mD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
				mD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
				mD3DDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
				break;
			}
			case Painter::RM_ALPHABLEND:
			{
				mD3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
				mD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
				mD3DDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
				mD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				mD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
				mD3DDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
				break;
			}
			case Painter::RM_XOR:
			{
				// There is no support of logical operations. Just do something. 
				// Do just about anything! We are desperate here! :)
				mD3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
				mD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
				mD3DDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
				mD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
				mD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
				mD3DDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT);
				break;
			}
			case Painter::RM_ADD:
			{
				mD3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
				mD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
				mD3DDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
				mD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
				mD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
				mD3DDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
				break;
			}
			case Painter::RM_NORMAL:
			default:
			{
				mD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
				mD3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
				mD3DDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
				mD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
				mD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
				break;
			}
		}

		mRenderModeChanged = false;
	}
}

void DirectX9Painter::SetAlphaValue(Lepra::uint8 pAlpha)
{
	Painter::SetAlphaValue(pAlpha);
	mD3DDevice->SetRenderState(D3DRS_ALPHAREF, GetAlphaValue());
}

void DirectX9Painter::SetClippingRect(int pLeft, int pTop, int pRight, int pBottom)
{
	Painter::SetClippingRect(pLeft, pTop, pRight, pBottom);

	ToScreenCoords(pLeft, pTop);
	ToScreenCoords(pRight, pBottom);

	RECT lRect;
	lRect.left   = pLeft;
	lRect.top    = pTop;
	lRect.right  = pRight;
	lRect.bottom = pBottom;
	mD3DDevice->SetScissorRect(&lRect);
}

void DirectX9Painter::ResetClippingRect()
{
	D3DXMATRIX lOrthoMtx;
	D3DXMATRIX lIdentityMtx;

	//Setup the orthogonal projection matrix and the default world/view matrix
	D3DXMatrixOrthoLH(&lOrthoMtx, (float)GetCanvas()->GetWidth(), (float)GetCanvas()->GetHeight(), 0.0f, 1.0f);
	D3DXMatrixIdentity(&lIdentityMtx);

	mD3DDevice->SetTransform(D3DTS_PROJECTION, &lOrthoMtx);
	mD3DDevice->SetTransform(D3DTS_WORLD, &lIdentityMtx);
	mD3DDevice->SetTransform(D3DTS_VIEW, &lIdentityMtx);

	//Make sure that the z-buffer and lighting are disabled
	mD3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	mD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	int lTop  = 0;
	int lLeft = 0;
	int lBottom = (int)GetCanvas()->GetHeight();
	int lRight  = (int)GetCanvas()->GetWidth();
	ToUserCoords(lLeft, lTop);
	ToUserCoords(lRight, lBottom);
	SetClippingRect(lLeft, lTop, lRight, lBottom);
}

void DirectX9Painter::DoDrawPixel(int x, int y)
{
	mD3DDevice->BeginScene();

	ToScreenCoords(x, y);

	UpdateRenderMode();

	struct VertexData
	{
		FLOAT x, y, z, rhw; // The transformed position for the vertex.
		DWORD color;        // The vertex color.
	};

	VertexData lVertex;

	DWORD lAlpha = ((DWORD)GetAlphaValue()) << 24;

	Lepra::Color& lColor = GetColorInternal(0);
	lVertex.x   = (float)x;
	lVertex.y   = (float)y;
	lVertex.z   = 0;
	lVertex.rhw = 1;
	lVertex.color = ToArgb(lAlpha, lColor);

	mD3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	mD3DDevice->DrawPrimitiveUP(D3DPT_POINTLIST, 1, &lVertex, sizeof(VertexData));

	mD3DDevice->EndScene();
	
}

void DirectX9Painter::DoDrawLine(int pX1, int pY1, int pX2, int pY2)
{
	mD3DDevice->BeginScene();

	ToScreenCoords(pX1, pY1);
	ToScreenCoords(pX2, pY2);

	UpdateRenderMode();

	struct VertexData
	{
		FLOAT x, y, z, rhw; // The transformed position for the vertex.
		DWORD color;        // The vertex color.
	};

	VertexData lVertex[2];

	DWORD lAlpha = ((DWORD)GetAlphaValue()) << 24;

	Lepra::Color& lColor = GetColorInternal(0);
	for (int i = 0; i < 2; i++)
	{
		lVertex[i].z   = 0;
		lVertex[i].rhw = 1;
		lVertex[i].color = ToArgb(lAlpha, lColor);
	}

	lVertex[0].x   = (float)pX1;
	lVertex[0].y   = (float)pY1;
	lVertex[1].x   = (float)pX2;
	lVertex[1].y   = (float)pY2;

	mD3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	mD3DDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, lVertex, sizeof(VertexData));

	mD3DDevice->EndScene();
}

void DirectX9Painter::DoDrawRect(int pLeft, int pTop, int pRight, int pBottom, int pWidth)
{
	mD3DDevice->BeginScene();

	ToScreenCoords(pLeft, pTop);
	ToScreenCoords(pRight, pBottom);

	UpdateRenderMode();

	float lLeft   = (float)pLeft;
	float lTop    = (float)pTop;
	float lRight  = (float)pRight;
	float lBottom = (float)pBottom;

	struct VertexData
	{
		FLOAT x, y, z, rhw; // The transformed position for the vertex.
		DWORD color;        // The vertex color.
	};

	VertexData lVertex[8];

	DWORD lAlpha = ((DWORD)GetAlphaValue()) << 24;
	int i;
	for (i = 0; i < 8; i++)
	{
		Lepra::Color* lColor;
		if(i < 4)
			lColor = &GetColorInternal(0);
		else
			lColor = &GetColorInternal(1);

		lVertex[i].z   = 0;
		lVertex[i].rhw = 1;
		lVertex[i].color = ToArgb(lAlpha, *lColor);
	}

	lVertex[0].x   = lLeft;
	lVertex[0].y   = lTop;
	lVertex[1].x   = lRight;
	lVertex[1].y   = lTop;
	lVertex[2].x   = lRight;
	lVertex[2].y   = lBottom;
	lVertex[3].x   = lLeft;
	lVertex[3].y   = lBottom;

	lLeft += pWidth;
	lTop += pWidth;
	lRight -= pWidth;
	lBottom -= pWidth;

	lVertex[4].x   = lLeft;
	lVertex[4].y   = lTop;
	lVertex[5].x   = lRight;
	lVertex[5].y   = lTop;
	lVertex[6].x   = lRight;
	lVertex[6].y   = lBottom;
	lVertex[7].x   = lLeft;
	lVertex[7].y   = lBottom;

	const static Lepra::uint16 lIndices[] = {0,4,7, 0,7,3, 0,1,5, 0,5,4, 1,2,6, 1,6,5, 7,6,2, 7,2,3};

	mD3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	mD3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 8, 8, lIndices, D3DFMT_INDEX16, lVertex, sizeof(VertexData));

	mD3DDevice->EndScene();
}

void DirectX9Painter::DoFillRect(int pLeft, int pTop, int pRight, int pBottom)
{
	mD3DDevice->BeginScene();

	ToScreenCoords(pLeft, pTop);
	ToScreenCoords(pRight, pBottom);

	UpdateRenderMode();

	float lLeft   = (float)pLeft;
	float lTop    = (float)pTop;
	float lRight  = (float)pRight;
	float lBottom = (float)pBottom;

	struct VertexData
	{
		FLOAT x, y, z, rhw; // The transformed position for the vertex.
		DWORD color;        // The vertex color.
	};

	VertexData lVertex[4];

	DWORD lAlpha = ((DWORD)GetAlphaValue()) << 24;
	Lepra::Color& lColor = GetColorInternal(0);

	for (int i = 0; i < 4; i++)
	{
		lVertex[i].z   = 0;
		lVertex[i].rhw = 1;
		lVertex[i].color = ToArgb(lAlpha, lColor);
	}

	lVertex[0].x   = lLeft;
	lVertex[0].y   = lTop;
	lVertex[1].x   = lRight;
	lVertex[1].y   = lTop;
	lVertex[2].x   = lRight;
	lVertex[2].y   = lBottom;
	lVertex[3].x   = lLeft;
	lVertex[3].y   = lBottom;

	mD3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	mD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, lVertex, sizeof(VertexData));

	mD3DDevice->EndScene();
}

void DirectX9Painter::DoDraw3DRect(int pLeft, int pTop, int pRight, int pBottom, int pWidth, bool pSunken)
{
	mD3DDevice->BeginScene();

	ToScreenCoords(pLeft, pTop);
	ToScreenCoords(pRight, pBottom);

	UpdateRenderMode();

	float lLeft   = (float)pLeft;
	float lTop    = (float)pTop;
	float lRight  = (float)pRight;
	float lBottom = (float)pBottom;

	struct VertexData
	{
		FLOAT x, y, z, rhw; // The transformed position for the vertex.
		DWORD color;        // The vertex color.
	};

	VertexData lVertex[12];

	DWORD lAlpha = ((DWORD)GetAlphaValue()) << 24;

	for (int i = 0; i < 12; i++)
	{
		lVertex[i].z   = 0;
		lVertex[i].rhw = 1;
	}

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
	Lepra::Color* lColor = &GetColorInternal(0);

	lVertex[0].x = lLeft; // Outer top left.
	lVertex[0].y = lTop;
	lVertex[0].color = ToArgb(lAlpha, lColor[lZero]);

	lVertex[1].x = lRight; // Outer top right #1.
	lVertex[1].y = lTop;
	lVertex[1].color = ToArgb(lAlpha, lColor[lZero]);

	lVertex[2].x = lRight; // Outer top right #2.
	lVertex[2].y = lTop;
	lVertex[2].color = ToArgb(lAlpha, lColor[lOne]);

	lVertex[3].x = lRight; // Outer bottom right.
	lVertex[3].y = lBottom;
	lVertex[3].color = ToArgb(lAlpha, lColor[lOne]);

	lVertex[4].x = lLeft; // Outer bottom left #1.
	lVertex[4].y = lBottom;
	lVertex[4].color = ToArgb(lAlpha, lColor[lZero]);

	lVertex[5].x = lLeft; // Outer bottom left #2.
	lVertex[5].y = lBottom;
	lVertex[5].color = ToArgb(lAlpha, lColor[lOne]);

	lLeft += pWidth;
	lTop += pWidth;
	lRight -= pWidth;
	lBottom -= pWidth;

	lVertex[6].x = lLeft; // Inner top left.
	lVertex[6].y = lTop;
	lVertex[6].color = ToArgb(lAlpha, lColor[lTwo]);

	lVertex[7].x = lRight; // Inner top right #1.
	lVertex[7].y = lTop;
	lVertex[7].color = ToArgb(lAlpha, lColor[lTwo]);

	lVertex[8].x = lRight; // Inner top right #2.
	lVertex[8].y = lTop;
	lVertex[8].color = ToArgb(lAlpha, lColor[lThree]);

	lVertex[9].x = lRight; // Inner bottom right.
	lVertex[9].y = lBottom;
	lVertex[9].color = ToArgb(lAlpha, lColor[lThree]);

	lVertex[10].x = lLeft; // Inner bottom left #1.
	lVertex[10].y = lBottom;
	lVertex[10].color = ToArgb(lAlpha, lColor[lTwo]);

	lVertex[11].x = lLeft; // Inner bottom left #2.
	lVertex[11].y = lBottom;
	lVertex[11].color = ToArgb(lAlpha, lColor[lThree]);

	const static Lepra::uint16 lsIndices[] = {0,1,7, 0,7,6, 0,6,10, 0,10,4, 8,2,3, 8,3,9, 11,9,3, 11,3,5};

	mD3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	mD3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 12, 8, lsIndices, D3DFMT_INDEX16, lVertex, sizeof(VertexData));

	mD3DDevice->EndScene();
}

void DirectX9Painter::DoFillShadedRect(int pLeft, int pTop, int pRight, int pBottom)
{
	mD3DDevice->BeginScene();

	ToScreenCoords(pLeft, pTop);
	ToScreenCoords(pRight, pBottom);

	UpdateRenderMode();

	float lLeft   = (float)pLeft;
	float lTop    = (float)pTop;
	float lRight  = (float)pRight;
	float lBottom = (float)pBottom;

	Lepra::Color* lColor = &GetColorInternal(0);

	// Calculate center color.
	DWORD lTopR = (DWORD)lColor[0].mRed   + (DWORD)lColor[1].mRed;
	DWORD lTopG = (DWORD)lColor[0].mGreen + (DWORD)lColor[1].mGreen;
	DWORD lTopB = (DWORD)lColor[0].mBlue  + (DWORD)lColor[1].mBlue;

	DWORD lBotR = (DWORD)lColor[2].mRed   + (DWORD)lColor[3].mRed;
	DWORD lBotG = (DWORD)lColor[2].mGreen + (DWORD)lColor[3].mGreen;
	DWORD lBotB = (DWORD)lColor[2].mBlue  + (DWORD)lColor[3].mBlue;

	DWORD lCenterR = ((lTopR + lBotR) >> 2);
	DWORD lCenterG = ((lTopG + lBotG) >> 2);
	DWORD lCenterB = ((lTopB + lBotB) >> 2);

	DWORD lAlpha = ((DWORD)GetAlphaValue()) << 24;

	struct VertexData
	{
		FLOAT x, y, z, rhw; // The transformed position for the vertex.
		DWORD color;        // The vertex color.
	};

	VertexData lVertex[6];

	for (int i = 0; i < 6; i++)
	{
		lVertex[i].z   = 0;
		lVertex[i].rhw = 1;
	}

	lVertex[0].x   = (lLeft + lRight) * 0.5f;
	lVertex[0].y   = (lTop + lBottom) * 0.5f;
	lVertex[0].color = lAlpha | ((lCenterR)   << 16) |
					((lCenterG) << 8) |
					((lCenterB)  << 0);
	
	lVertex[1].x   = lLeft;
	lVertex[1].y   = lTop;
	lVertex[1].color = ToArgb(lAlpha, lColor[0]);

	lVertex[2].x   = lRight;
	lVertex[2].y   = lTop;
	lVertex[2].color = ToArgb(lAlpha, lColor[1]);

	lVertex[3].x   = lRight;
	lVertex[3].y   = lBottom;
	lVertex[3].color = ToArgb(lAlpha, lColor[2]);

	lVertex[4].x   = lLeft;
	lVertex[4].y   = lBottom;
	lVertex[4].color = ToArgb(lAlpha, lColor[3]);

	lVertex[5].x   = lLeft;
	lVertex[5].y   = lTop;
	lVertex[5].color = ToArgb(lAlpha, lColor[0]);

	mD3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	mD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 4, lVertex, sizeof(VertexData));

	mD3DDevice->EndScene();
}

void DirectX9Painter::DoFillTriangle(float pX1, float pY1,
				  float pX2, float pY2,
				  float pX3, float pY3)
{
	mD3DDevice->BeginScene();

	ToScreenCoords(pX1, pY1);
	ToScreenCoords(pX2, pY2);
	ToScreenCoords(pX3, pY3);

	UpdateRenderMode();

	struct VertexData
	{
		FLOAT x, y, z, rhw; // The transformed position for the vertex.
		DWORD color;        // The vertex color.
	};

	VertexData lVertex[3];

	DWORD lAlpha = ((DWORD)GetAlphaValue()) << 24;
	Lepra::Color& lColor = GetColorInternal(0);

	for (int i = 0; i < 3; i++)
	{
		lVertex[i].z   = 0;
		lVertex[i].rhw = 1;
		lVertex[i].color = ToArgb(lAlpha, lColor);
	}

	lVertex[0].x   = (FLOAT)pX1;
	lVertex[0].y   = (FLOAT)pY1;
	lVertex[1].x   = (FLOAT)pX2;
	lVertex[1].y   = (FLOAT)pY2;
	lVertex[2].x   = (FLOAT)pX3;
	lVertex[2].y   = (FLOAT)pY3;

	mD3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	mD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, lVertex, sizeof(VertexData));

	mD3DDevice->EndScene();
}

void DirectX9Painter::DoFillShadedTriangle(float pX1, float pY1,
					float pX2, float pY2,
					float pX3, float pY3)
{
	mD3DDevice->BeginScene();

	ToScreenCoords(pX1, pY1);
	ToScreenCoords(pX2, pY2);
	ToScreenCoords(pX3, pY3);

	UpdateRenderMode();

	struct VertexData
	{
		FLOAT x, y, z, rhw; // The transformed position for the vertex.
		DWORD color;        // The vertex color.
	};

	VertexData lVertex[3];

	DWORD lAlpha = ((DWORD)GetAlphaValue()) << 24;
	Lepra::Color* lColor = &GetColorInternal(0);

	for (int i = 0; i < 3; i++)
	{
		lVertex[i].z   = 0;
		lVertex[i].rhw = 1;
		lVertex[i].color = ToArgb(lAlpha, lColor[i]);
	}

	lVertex[0].x   = (FLOAT)pX1;
	lVertex[0].y   = (FLOAT)pY1;
	lVertex[1].x   = (FLOAT)pX2;
	lVertex[1].y   = (FLOAT)pY2;
	lVertex[2].x   = (FLOAT)pX3;
	lVertex[2].y   = (FLOAT)pY3;

	mD3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	mD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, lVertex, sizeof(VertexData));

	mD3DDevice->EndScene();
}

void DirectX9Painter::DoFillTriangle(float pX1, float pY1, float pU1, float pV1,
				  float pX2, float pY2, float pU2, float pV2,
				  float pX3, float pY3, float pU3, float pV3,
				  ImageID pImageID)
{
	TextureTable::Iterator lIter = mTextureTable.Find((int)pImageID);
	if (lIter == mTextureTable.End())
	{
		return;
	}

	Texture* lTexture = *lIter;

	mD3DDevice->BeginScene();

	ToScreenCoords(pX1, pY1);
	ToScreenCoords(pX2, pY2);
	ToScreenCoords(pX3, pY3);

	UpdateRenderMode();

	struct VertexData
	{
		FLOAT x, y, z, rhw; // The transformed position for the vertex.
		DWORD color;	    // TODO: Verify that this works.
		FLOAT u, v;
	};

	VertexData lVertex[3];

	DWORD lAlpha = ((DWORD)GetAlphaValue()) << 24;
	Lepra::Color* lColor = &GetColorInternal(0);

	for (int i = 0; i < 3; i++)
	{
		lVertex[i].z   = 0;
		lVertex[i].rhw = 1;

		// TODO: Verify that this works.
		lVertex[i].color = ToArgb(lAlpha, lColor[i]);
	}

	lVertex[0].x   = (FLOAT)pX1;
	lVertex[0].y   = (FLOAT)pY1;
	lVertex[0].u   = (FLOAT)pU1;
	lVertex[0].v   = (FLOAT)pV1;
	lVertex[1].x   = (FLOAT)pX2;
	lVertex[1].y   = (FLOAT)pY2;
	lVertex[1].u   = (FLOAT)pU2;
	lVertex[1].v   = (FLOAT)pV2;
	lVertex[2].x   = (FLOAT)pX3;
	lVertex[2].y   = (FLOAT)pY3;
	lVertex[2].u   = (FLOAT)pU3;
	lVertex[2].v   = (FLOAT)pV3;

	mD3DDevice->SetTexture(0, lTexture->mD3DTexture);

	mD3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
	mD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, lVertex, sizeof(VertexData));

	mD3DDevice->EndScene();

	mD3DDevice->SetTexture(0, 0);
}

Painter::ImageID DirectX9Painter::AddImage(const Lepra::Canvas* pImage, const Lepra::Canvas* pAlphaBuffer)
{
	if (pImage == 0 && pAlphaBuffer == 0)
	{
		return (ImageID)mTextureIDManager.GetInvalidId();
	}

	int lID = 0;

	bool lAlpha = false;
	bool lColor = false;

	if (pImage != 0)
	{
		lColor = true;

		if (pImage->GetBitDepth() == Lepra::Canvas::BITDEPTH_32_BIT)
		{
			lAlpha = true;
		}
	}

	if (pAlphaBuffer != 0)
	{
		lAlpha = true;
	}

	lID = mTextureIDManager.GetFreeId();

	if (lID != mTextureIDManager.GetInvalidId())
	{
		Lepra::Canvas lImage;
		D3DFORMAT lFormat = D3DFMT_R8G8B8;
		bool lCreateTexture = false;

		if (lColor == true && lAlpha == true)
		{
			lImage.Copy(*pImage);

			unsigned lNewWidth  = GetClosestPowerOf2(lImage.GetWidth(), true);
			unsigned lNewHeight = GetClosestPowerOf2(lImage.GetHeight(), true);

			if (lNewWidth != lImage.GetWidth() || lNewHeight != lImage.GetHeight())
			{
				lImage.Resize(lNewWidth, lNewHeight, Lepra::Canvas::RESIZE_NICEST);
			}

			if (pAlphaBuffer != 0)
			{
				Lepra::Canvas lAlphaBuffer(*pAlphaBuffer, true);

				if (lAlphaBuffer.GetWidth() != lImage.GetWidth() ||
				   lAlphaBuffer.GetHeight() != lImage.GetHeight())
				{
					lAlphaBuffer.Resize(lImage.GetWidth(), lImage.GetHeight(), Lepra::Canvas::RESIZE_FAST);
				}
				if (lAlphaBuffer.GetBitDepth() != Lepra::Canvas::BITDEPTH_8_BIT)
				{
					lAlphaBuffer.ConvertToGrayscale(true);
				}
	
				lImage.ConvertTo32BitWithAlpha(lAlphaBuffer);
			}

			lFormat = D3DFMT_A8R8G8B8;
			lCreateTexture = true;
		}
		else if(lColor == true)
		{
			lImage.Copy(*pImage);
			lImage.ConvertBitDepth(Lepra::Canvas::BITDEPTH_24_BIT);

			unsigned lNewWidth  = GetClosestPowerOf2(lImage.GetWidth(), true);
			unsigned lNewHeight = GetClosestPowerOf2(lImage.GetHeight(), true);
			if (lNewWidth != lImage.GetWidth() || lNewHeight != lImage.GetHeight())
			{
				lImage.Resize(lNewWidth, lNewHeight, Lepra::Canvas::RESIZE_FAST);
			}

			lFormat = D3DFMT_R8G8B8;
			lCreateTexture = true;
		}
		else if(pAlphaBuffer != 0)
		{
			lImage.Copy(*pAlphaBuffer);

			Lepra::Color lPalette[256];
			for (int i = 0; i < 256; i++)
			{
				lPalette[i].mRed   = (Lepra::uint8)i;
				lPalette[i].mGreen = (Lepra::uint8)i;
				lPalette[i].mBlue  = (Lepra::uint8)i;
			}

			lImage.SetPalette(lPalette);
			lImage.ConvertTo32BitWithAlpha(*pAlphaBuffer);

			lFormat = D3DFMT_A8R8G8B8;
			lCreateTexture = true;
		}

		if (lCreateTexture == true)
		{
			Texture* lTexture = new Texture();

			if (pImage != 0)
			{
				lTexture->mWidth = pImage->GetWidth();
				lTexture->mHeight = pImage->GetHeight();
			}
			else
			{
				lTexture->mWidth = pAlphaBuffer->GetWidth();
				lTexture->mHeight = pAlphaBuffer->GetHeight();
			}

			HRESULT lRes = mD3DDevice->CreateTexture(lImage.GetWidth(), lImage.GetHeight(), 1, 0,
				lFormat, D3DPOOL_DEFAULT, &lTexture->mD3DTexture, NULL);

			if (FAILED(lRes))
			{
				delete lTexture;
				mTextureIDManager.RecycleId(lID);
				lID = mTextureIDManager.GetInvalidId();
			}
			else
			{
				IDirect3DSurface9* lSurface;
				if (FAILED(lTexture->mD3DTexture->GetSurfaceLevel(0, &lSurface)))
				{
					mLog.AError("AddImage() - Failed to get surface level 0!");
					lTexture->mD3DTexture->Release();
					delete lTexture;
					mTextureIDManager.RecycleId(lID);
					return (ImageID)mTextureIDManager.GetInvalidId();
				}
				RECT lRect;
				lRect.top = 0;
				lRect.left = 0;
				lRect.bottom = lImage.GetHeight();
				lRect.right = lImage.GetWidth();

				D3DXLoadSurfaceFromMemory(lSurface, 0, 0, lImage.GetBuffer(), lFormat,
					lImage.GetPitch() * lImage.GetPixelByteSize(), 0, &lRect, D3DX_FILTER_NONE, 0);

				mTextureTable.Insert(lID, lTexture);
			}
		}
	}

	return (ImageID)lID;
}

void DirectX9Painter::UpdateImage(ImageID pImageID, const Lepra::Canvas* pImage, const Lepra::Canvas* pAlphaBuffer, UpdateHint pHint)
{
	TextureTable::Iterator lIter = mTextureTable.Find(pImageID);
	if (lIter == mTextureTable.End())
	{
		return;
	}
	Texture* lTexture = *lIter;

	if (pHint == UPDATE_FAST)
	{
		// Perform a fast update... Only consider the color map.
		if (pImage != 0 && 
		   (int)pImage->GetWidth() == lTexture->mWidth && 
		   (int)pImage->GetHeight() == lTexture->mHeight)
		{
			IDirect3DSurface9* lSurface;
			if (FAILED(lTexture->mD3DTexture->GetSurfaceLevel(0, &lSurface)))
			{
				mLog.AError("UpdateImage() - Failed to get surface level 0!");
				return;
			}

			RECT lRect;
			lRect.top = 0;
			lRect.left = 0;
			lRect.bottom = pImage->GetHeight();
			lRect.right = pImage->GetWidth();

			if (pImage->GetBitDepth() == Lepra::Canvas::BITDEPTH_24_BIT)
			{
				D3DXLoadSurfaceFromMemory(lSurface, 0, 0, pImage->GetBuffer(), 
							  D3DFMT_R8G8B8, pImage->GetPitch() * pImage->GetPixelByteSize(),
							  0, &lRect, D3DX_FILTER_NONE, 0);
			}
			else if(pImage->GetBitDepth() == Lepra::Canvas::BITDEPTH_32_BIT)
			{
				D3DXLoadSurfaceFromMemory(lSurface, 0, 0, pImage->GetBuffer(), 
							  D3DFMT_A8R8G8B8, pImage->GetPitch() * pImage->GetPixelByteSize(),
							  0, &lRect, D3DX_FILTER_NONE, 0);
			}
		}
		return;
	}


	bool lAlpha = false;
	bool lColor = false;

	if (pImage != 0)
	{
		lColor = true;

		if (pImage->GetBitDepth() == Lepra::Canvas::BITDEPTH_32_BIT)
		{
			lAlpha = true;
		}
	}

	if (pAlphaBuffer != 0)
	{
		lAlpha = true;
	}

	Lepra::Canvas lImage;
	D3DFORMAT lFormat = D3DFMT_R8G8B8;
	bool lCreateTexture = false;

	if (lColor == true && lAlpha == true)
	{
		lImage.Copy(*pImage);

		unsigned lNewWidth  = GetClosestPowerOf2(lImage.GetWidth(), true);
		unsigned lNewHeight = GetClosestPowerOf2(lImage.GetHeight(), true);

		if (lNewWidth != lImage.GetWidth() || lNewHeight != lImage.GetHeight())
		{
			lImage.Resize(lNewWidth, lNewHeight, Lepra::Canvas::RESIZE_NICEST);
		}

		if (pAlphaBuffer != 0)
		{
			Lepra::Canvas lAlphaBuffer(*pAlphaBuffer, true);

			if (lAlphaBuffer.GetWidth() != lImage.GetWidth() ||
			   lAlphaBuffer.GetHeight() != lImage.GetHeight())
			{
				lAlphaBuffer.Resize(lImage.GetWidth(), lImage.GetHeight(), Lepra::Canvas::RESIZE_FAST);
			}
			if (lAlphaBuffer.GetBitDepth() != Lepra::Canvas::BITDEPTH_8_BIT)
			{
				lAlphaBuffer.ConvertToGrayscale(true);
			}

			lImage.ConvertTo32BitWithAlpha(lAlphaBuffer);
		}

		lFormat = D3DFMT_A8R8G8B8;
		lCreateTexture = true;
	}
	else if(lColor == true)
	{
		Lepra::Canvas lImage(*pImage, true);
		lImage.ConvertBitDepth(Lepra::Canvas::BITDEPTH_24_BIT);

		unsigned lNewWidth  = GetClosestPowerOf2(lImage.GetWidth(), true);
		unsigned lNewHeight = GetClosestPowerOf2(lImage.GetHeight(), true);
		if (lNewWidth != lImage.GetWidth() || lNewHeight != lImage.GetHeight())
		{
			lImage.Resize(lNewWidth, lNewHeight, Lepra::Canvas::RESIZE_FAST);
		}

		lFormat = D3DFMT_R8G8B8;
		lCreateTexture = true;
	}
	else if(pAlphaBuffer != 0)
	{
		Lepra::Canvas lImage(*pAlphaBuffer, true);

		Lepra::Color lPalette[256];
		for (int i = 0; i < 256; i++)
		{
			lPalette[i].mRed   = (Lepra::uint8)i;
			lPalette[i].mGreen = (Lepra::uint8)i;
			lPalette[i].mBlue  = (Lepra::uint8)i;
		}

		lImage.SetPalette(lPalette);
		lImage.ConvertTo32BitWithAlpha(*pAlphaBuffer);

		lFormat = D3DFMT_A8R8G8B8;
		lCreateTexture = true;
	}

	if (lCreateTexture == true)
	{
		IDirect3DTexture9* lD3DTexture;
		HRESULT lRes = mD3DDevice->CreateTexture(lImage.GetWidth(), lImage.GetHeight(), 1, 0, lFormat,
			D3DPOOL_DEFAULT, &lD3DTexture, NULL);

		if (FAILED(lRes))
		{
			return;
		}
		else
		{
			IDirect3DSurface9* lSurface;
			if (FAILED(lD3DTexture->GetSurfaceLevel(0, &lSurface)))
			{
				lD3DTexture->Release();
				mLog.AError("UpdateImage() - Failed to get surface level 0!");
				return;
			}
			lTexture->mWidth = pImage->GetWidth();
			lTexture->mHeight = pImage->GetHeight();
			lTexture->mD3DTexture->Release();
			lTexture->mD3DTexture = lD3DTexture;

			RECT lRect;
			lRect.top = 0;
			lRect.left = 0;
			lRect.bottom = lImage.GetHeight();
			lRect.right = lImage.GetWidth();

			D3DXLoadSurfaceFromMemory(lSurface, 0, 0, lImage.GetBuffer(), 
						  lFormat, lImage.GetPitch() * lImage.GetPixelByteSize(),
						  0, &lRect, D3DX_FILTER_NONE, 0);
		}
	}
}

void DirectX9Painter::RemoveImage(ImageID pImageID)
{
	TextureTable::Iterator lIter = mTextureTable.Find(pImageID);
	if (lIter == mTextureTable.End())
	{
		return;
	}

	Texture* lTexture = *lIter;
	mTextureTable.Remove(lIter);

	lTexture->mD3DTexture->Release();
	delete lTexture;
}

void DirectX9Painter::DoDrawImage(ImageID pImageID, int x, int y)
{
	TextureTable::Iterator lIter = mTextureTable.Find(pImageID);

	if (lIter == mTextureTable.End())
	{
		return;
	}

	Texture* lTexture = *lIter;

	ToScreenCoords(x, y);
	Lepra::PixelRect lRect(x, y, x + lTexture->mWidth, y + lTexture->mHeight);
	ToUserCoords(lRect.mLeft, lRect.mTop);
	ToUserCoords(lRect.mRight, lRect.mBottom);
	DrawImage(pImageID, lRect);
}

void DirectX9Painter::DoDrawImage(ImageID pImageID, int x, int y, const Lepra::PixelRect& pSubpatchRect)
{
	TextureTable::Iterator lIter = mTextureTable.Find(pImageID);

	if (lIter == mTextureTable.End())
	{
		return;
	}

	Texture* lTexture = *lIter;

	ToScreenCoords(x, y);
	Lepra::PixelRect lRect(x, y, x + lTexture->mWidth, y + lTexture->mHeight);
	ToUserCoords(lRect.mLeft, lRect.mTop);
	ToUserCoords(lRect.mRight, lRect.mBottom);
	DrawImage(pImageID, lRect, pSubpatchRect);
}

void DirectX9Painter::DoDrawImage(ImageID pImageID, const Lepra::PixelRect& pRect)
{
	TextureTable::Iterator lIter = mTextureTable.Find((int)pImageID);
	if (lIter == mTextureTable.End())
	{
		return;
	}

	Texture* lTexture = *lIter;

	mD3DDevice->BeginScene();

	UpdateRenderMode();

	float lLeft   = (float)pRect.mLeft;
	float lRight  = (float)pRect.mRight;
	float lTop    = (float)pRect.mTop;
	float lBottom = (float)pRect.mBottom;

	ToScreenCoords(lLeft, lTop);
	ToScreenCoords(lRight, lBottom);

	struct VertexData
	{
		FLOAT x, y, z, rhw; // The transformed position for the vertex.
		DWORD color;        // The vertex color.
		FLOAT u, v;
	};

	VertexData lVertex[4];

	DWORD lAlpha = ((DWORD)GetAlphaValue()) << 24;
	
	if (GetRenderMode() == RM_ALPHATEST)
	{
		lAlpha = 0xFF000000;
	}

	Lepra::Color& lColor = GetColorInternal(0);
	for (int i = 0; i < 4; i++)
	{
		lVertex[i].z   = 0;
		lVertex[i].rhw = 1;
		lVertex[i].color = ToArgb(lAlpha, lColor);
	}

	lVertex[0].x   = (float)lLeft;
	lVertex[0].y   = (float)lTop;
	lVertex[0].u   = 0;
	lVertex[0].v   = 0;
	lVertex[1].x   = (float)lRight;
	lVertex[1].y   = (float)lTop;
	lVertex[1].u   = 1;
	lVertex[1].v   = 0;
	lVertex[2].x   = (float)lRight;
	lVertex[2].y   = (float)lBottom;
	lVertex[2].u   = 1;
	lVertex[2].v   = 1;
	lVertex[3].x   = (float)lLeft;
	lVertex[3].y   = (float)lBottom;
	lVertex[3].u   = 0;
	lVertex[3].v   = 1;

	mD3DDevice->SetTexture(0, lTexture->mD3DTexture);

	mD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	mD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	mD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT/*D3DTA_DIFFUSE*/);

	if (GetRenderMode() == RM_ALPHATEST ||
	   GetRenderMode() == RM_ALPHABLEND)
	{
		mD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
	}
	else
	{
		mD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
	}


	mD3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	mD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, lVertex, sizeof(VertexData));

	mD3DDevice->EndScene();
	mD3DDevice->SetTexture(0, 0);
}

void DirectX9Painter::DoDrawImage(ImageID pImageID, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubpatchRect)
{
	TextureTable::Iterator lIter = mTextureTable.Find((int)pImageID);
	if (lIter == mTextureTable.End())
	{
		return;
	}

	Texture* lTexture = *lIter;

	mD3DDevice->BeginScene();

	UpdateRenderMode();

	float lLeft   = (float)pRect.mLeft;
	float lRight  = (float)pRect.mRight;
	float lTop    = (float)pRect.mTop;
	float lBottom = (float)pRect.mBottom;

	ToScreenCoords(lLeft, lTop);
	ToScreenCoords(lRight, lBottom);

	float lOneOverWidth  = 1.0f / (float)lTexture->mWidth;
	float lOneOverHeight = 1.0f / (float)lTexture->mHeight;
	float lULeft   = ((float)pSubpatchRect.mLeft   + 0.5f) * lOneOverWidth;
	float lURight  = ((float)pSubpatchRect.mRight  + 0.5f) * lOneOverWidth;
	float lVTop    = ((float)pSubpatchRect.mTop    + 0.5f) * lOneOverHeight;
	float lVBottom = ((float)pSubpatchRect.mBottom + 0.5f) * lOneOverHeight;

	struct VertexData
	{
		FLOAT x, y, z, rhw; // The transformed position for the vertex.
		DWORD color;        // The vertex color.
		FLOAT u, v;
	};

	VertexData lVertex[4];

	DWORD lAlpha = ((DWORD)GetAlphaValue()) << 24;

	if (GetRenderMode() == RM_ALPHATEST)
	{
		lAlpha = 0xFF000000;
	}

	Lepra::Color& lColor = GetColorInternal(0);
	for (int i = 0; i < 4; i++)
	{
		lVertex[i].z   = 0;
		lVertex[i].rhw = 1;
		lVertex[i].color = ToArgb(lAlpha, lColor);
	}

	lVertex[0].x   = (float)lLeft;
	lVertex[0].y   = (float)lTop;
	lVertex[0].u   = lULeft;
	lVertex[0].v   = lVTop;
	lVertex[1].x   = (float)lRight;
	lVertex[1].y   = (float)lTop;
	lVertex[1].u   = lURight;
	lVertex[1].v   = lVTop;
	lVertex[2].x   = (float)lRight;
	lVertex[2].y   = (float)lBottom;
	lVertex[2].u   = lURight;
	lVertex[2].v   = lVBottom;
	lVertex[3].x   = (float)lLeft;
	lVertex[3].y   = (float)lBottom;
	lVertex[3].u   = lULeft;
	lVertex[3].v   = lVBottom;

	mD3DDevice->SetTexture(0, lTexture->mD3DTexture);

	mD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	mD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	mD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT/*D3DTA_DIFFUSE*/);

	if (GetRenderMode() == RM_ALPHATEST ||
	   GetRenderMode() == RM_ALPHABLEND)
	{
		mD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
	}
	else
	{
		mD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
	}


	mD3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	mD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, lVertex, sizeof(VertexData));

	mD3DDevice->EndScene();
	mD3DDevice->SetTexture(0, 0);
}

void DirectX9Painter::DoDrawAlphaImage(ImageID pImageID, int x, int y)
{
	ToScreenCoords(x, y);

	UpdateRenderMode();

	TextureTable::Iterator lIter = mTextureTable.Find(pImageID);

	if (lIter == mTextureTable.End())
	{
		return;
	}

	Texture* lTexture = *lIter;

	float lLeft   = (float)x;
	float lRight  = (float)(x + lTexture->mWidth);
	float lTop    = (float)y;
	float lBottom = (float)(y + lTexture->mHeight);

	struct VertexData
	{
		FLOAT x, y, z, rhw; // The transformed position for the vertex.
		DWORD color;        // The vertex color.
		FLOAT u, v;
	};

	VertexData lVertex[4];

	DWORD lAlpha = ((DWORD)GetAlphaValue()) << 24;

	if (GetRenderMode() == RM_ALPHATEST)
	{
		lAlpha = 0xFF000000;
	}

	Lepra::Color& lColor = GetColorInternal(0);
	for (int i = 0; i < 4; i++)
	{
		lVertex[i].z   = 0;
		lVertex[i].rhw = 1;
		lVertex[i].color = ToArgb(lAlpha, lColor);
	}

	lVertex[0].x   = (float)lLeft;
	lVertex[0].y   = (float)lTop;
	lVertex[0].u   = 0;
	lVertex[0].v   = 0;
	lVertex[1].x   = (float)lRight;
	lVertex[1].y   = (float)lTop;
	lVertex[1].u   = 1;
	lVertex[1].v   = 0;
	lVertex[2].x   = (float)lRight;
	lVertex[2].y   = (float)lBottom;
	lVertex[2].u   = 1;
	lVertex[2].v   = 1;
	lVertex[3].x   = (float)lLeft;
	lVertex[3].y   = (float)lBottom;
	lVertex[3].u   = 0;
	lVertex[3].v   = 1;

	mD3DDevice->SetTexture(0, lTexture->mD3DTexture);

	mD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	mD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	mD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

	if (GetRenderMode() == RM_ALPHATEST ||
	   GetRenderMode() == RM_ALPHABLEND)
	{
		mD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
	}
	else
	{
		mD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
	}


	mD3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	mD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, lVertex, sizeof(VertexData));

	mD3DDevice->EndScene();
	mD3DDevice->SetTexture(0, 0);
}

int DirectX9Painter::DoPrintText(const Lepra::String& pString, int x, int y)
{
	ToScreenCoords(x, y);

	UpdateRenderMode();

	int lCurrentX = x;
	int lCurrentY = y;

	mD3DDevice->BeginScene();

	struct VertexData
	{
		FLOAT x, y, z, rhw; // The transformed position for the vertex.
		DWORD color;        // The vertex color.
		FLOAT u, v;
	};

	VertexData lVertex[4];

	unsigned lAlpha = (unsigned)GetAlphaValue();
	if (GetRenderMode() == RM_ALPHATEST)
	{
		lAlpha = 0xFF000000;
	}

	Lepra::Color& lColor = GetColorInternal(0);
	for (int i = 0; i < 4; i++)
	{
		lVertex[i].z   = 0;
		lVertex[i].rhw = 1;
		lVertex[i].color = ToArgb(lAlpha, lColor);
	}

	mD3DDevice->SetTexture(0, ((DirectX9Font*)GetCurrentFont())->mTexture->mD3DTexture);

	mD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	mD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	mD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	mD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);

	mD3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);

	DirectX9Font* lFont = (DirectX9Font*)GetCurrentFont();

	for (size_t i = 0; i < pString.length(); i++)
	{
		char lChar = (char)pString[i];

		if (lChar == '\n')
		{
			lCurrentY += (lFont->mCharHeight + lFont->mNewLineOffset);
			lCurrentX = x;
		}
		else if(lChar != '\r' && 
			lChar != '\b' &&
			lChar != '\t')
		{
			float lCharWidth = (float)lFont->mTileWidth;
			float lLeft   = (float)lCurrentX;
			float lRight  = (float)(lCurrentX + lCharWidth);
			float lTop    = (float)lCurrentY;
			float lBottom = (float)(lCurrentY + lFont->mCharHeight);

			DirectX9Font::FRect& lRect = lFont->mCharRect[(Lepra::uint8)lChar];

			lVertex[0].x   = lLeft;
			lVertex[0].y   = lTop;
			lVertex[0].u   = (float)lRect.mLeft;
			lVertex[0].v   = (float)lRect.mTop;
			lVertex[1].x   = lRight;
			lVertex[1].y   = lTop;
			lVertex[1].u   = (float)lRect.mRight;
			lVertex[1].v   = (float)lRect.mTop;
			lVertex[2].x   = lRight;
			lVertex[2].y   = lBottom;
			lVertex[2].u   = (float)lRect.mRight;
			lVertex[2].v   = (float)lRect.mBottom;
			lVertex[3].x   = lLeft;
			lVertex[3].y   = lBottom;
			lVertex[3].u   = (float)lRect.mLeft;
			lVertex[3].v   = (float)lRect.mBottom;

			mD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, lVertex, sizeof(VertexData));

			lCurrentX += lFont->mCharWidth[(Lepra::uint8)lChar] + lFont->mCharOffset;
		}

		if (lChar == ' ')
		{
			lCurrentX += lFont->mDefaultSpaceWidth;
		}
		else if(lChar == '\t')
		{
			lCurrentX = GetTabOriginX() + (((lCurrentX - GetTabOriginX()) / lFont->mTabWidth) + 1) * lFont->mTabWidth;
		}
	}

	mD3DDevice->EndScene();
	mD3DDevice->SetTexture(0, 0);

	int lTemp = 0;
	ToUserCoords(lCurrentX, lTemp);

	return lCurrentX;
}

void DirectX9Painter::AdjustVertexFormat(Lepra::uint16& pVertexFormat)
{
	pVertexFormat |= Geometry2D::VTX_INTERLEAVED;
}

void DirectX9Painter::ReadPixels(Lepra::Canvas& pDestCanvas, const Lepra::PixelRect& pRect)
{
	if (GetCanvas() == 0 || GetCanvas()->GetBitDepth() == Lepra::Canvas::BITDEPTH_8_BIT)
	{
		pDestCanvas.Reset(0, 0, Lepra::Canvas::BITDEPTH_32_BIT);
		return;
	}

	Lepra::PixelRect lRect(pRect);

	ToScreenCoords(lRect.mLeft, lRect.mTop);
	ToScreenCoords(lRect.mRight, lRect.mBottom);

	if (lRect.mLeft < 0)
	{
		lRect.mLeft = 0;
	}
	if (lRect.mTop < 0)
	{
		lRect.mTop = 0;
	}
	if (lRect.mRight > (int)GetCanvas()->GetWidth())
	{
		lRect.mRight = (int)GetCanvas()->GetWidth();
	}
	if (lRect.mBottom > (int)GetCanvas()->GetHeight())
	{
		lRect.mBottom = (int)GetCanvas()->GetHeight();
	}

	if (lRect.mRight <= lRect.mLeft ||
	   lRect.mBottom <= lRect.mTop)
	{
		pDestCanvas.Reset(0, 0, Lepra::Canvas::BITDEPTH_32_BIT);
		return;
	}

	LPDIRECT3DSURFACE9 lRenderTarget = 0;
	LPDIRECT3DSURFACE9 lInMemTarget = 0;
	D3DSURFACE_DESC	lSurfDesc;
	HRESULT	lHRes;

	lHRes = mD3DDevice->GetRenderTarget(0, &lRenderTarget);
	if (FAILED(lHRes))
	{
		mLog.AError("GetRenderTarget failed in ReadPixels().");
		return;
	}

	lHRes = lRenderTarget->GetDesc(&lSurfDesc);
	if (FAILED(lHRes)) 
	{
		mLog.AError("GetDesc failed in ReadPixels().");
		return;
	}

	lHRes = mD3DDevice->CreateOffscreenPlainSurface(lSurfDesc.Width, 
													   lSurfDesc.Height, 
													   lSurfDesc.Format, 
													   D3DPOOL_SYSTEMMEM, 
													   &lInMemTarget, NULL);
	if (FAILED(lHRes)) 
	{
		mLog.AError("CreateOffscreenPlainSurface failed in ReadPixels().");
		return;
	}

	lHRes = mD3DDevice->GetRenderTargetData(lRenderTarget, lInMemTarget);
	if (FAILED(lHRes))
	{
		mLog.AError("GetRenderTargetData failed in ReadPixels().");
		lInMemTarget->Release();
		return;
	}

	D3DLOCKED_RECT lLockedRect;
	if (FAILED(lInMemTarget->LockRect(&lLockedRect, NULL, 0)))
	{
		mLog.AError("LockRect failed in ReadPixels().");
		lInMemTarget->Release();
		return;
	}

	Lepra::Canvas::BitDepth lBitDepth = Lepra::Canvas::BITDEPTH_32_BIT;

	switch(lSurfDesc.Format)
	{
	case D3DFMT_R8G8B8:
		lBitDepth = Lepra::Canvas::BITDEPTH_24_BIT;
		break;
	case D3DFMT_A8R8G8B8:
	case D3DFMT_X8R8G8B8:
	case D3DFMT_A8B8G8R8:
	case D3DFMT_X8B8G8R8:
		lBitDepth = Lepra::Canvas::BITDEPTH_32_BIT;
		break;
	case D3DFMT_R5G6B5:
		lBitDepth = Lepra::Canvas::BITDEPTH_16_BIT;
		break;
	case D3DFMT_A1R5G5B5:
	case D3DFMT_X1R5G5B5:
		lBitDepth = Lepra::Canvas::BITDEPTH_15_BIT;
		break;
	case D3DFMT_R3G3B2:
	case D3DFMT_A8:
	case D3DFMT_L8:
	case D3DFMT_P8:
		lBitDepth = Lepra::Canvas::BITDEPTH_8_BIT;
		pDestCanvas.SetPalette(GetCanvas()->GetPalette());
		break;
	default:
		mLog.AError("Invalid pixel format in ReadPixels().");
		pDestCanvas.Reset(0, 0, Lepra::Canvas::BITDEPTH_32_BIT);
		lInMemTarget->UnlockRect();
		lInMemTarget->Release();
		return;
	}

	pDestCanvas.Reset(lSurfDesc.Width, lSurfDesc.Height, lBitDepth);
	pDestCanvas.SetPitch(lLockedRect.Pitch / pDestCanvas.GetPixelByteSize());
	pDestCanvas.SetBuffer(lLockedRect.pBits);

	// Crop the image. Note that this call will make a copy of the image buffer, 
	// even if the cropped dimensions equals the current dimensions.
	pDestCanvas.Crop(lRect.mLeft, lRect.mTop, lRect.mRight, lRect.mBottom);

	lInMemTarget->UnlockRect();
	lInMemTarget->Release();
}

Painter::RGBOrder DirectX9Painter::GetRGBOrder()
{
	return Painter::RGB;
}

void DirectX9Painter::DoRenderDisplayList(std::vector<DisplayEntity*>* pDisplayList)
{
	PushAttrib(ATTR_ALL);

	mD3DDevice->BeginScene();

	std::vector<DisplayEntity*>::iterator it;
	for(it = pDisplayList->begin(); it != pDisplayList->end(); ++it)
	{
		DisplayEntity* lSE = *it;
		Painter::SetClippingRect(lSE->GetClippingRect());
		SetAlphaValue(lSE->GetAlpha());
		SetRenderMode(lSE->GetRenderMode());
		UpdateRenderMode();

		DWORD lFVF = D3DFVF_XYZRHW;

		if(lSE->GetImageID() == Painter::INVALID_IMAGEID)
		{
			mD3DDevice->SetTexture(0, 0);
		}
		else
		{
			TextureTable::Iterator lIter = mTextureTable.Find((int)lSE->GetImageID());
			Texture* lTexture = *lIter;
			mD3DDevice->SetTexture(0, lTexture->mD3DTexture);

			mD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
			mD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			mD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT/*D3DTA_DIFFUSE*/);

			if (GetRenderMode() == RM_ALPHATEST || GetRenderMode() == RM_ALPHABLEND)
			{
				mD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
			}
			else
			{
				mD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
			}
		}

		Lepra::uint16 lVertexFormat = lSE->GetGeometry().GetVertexFormat() & (Geometry2D::VTX_UV | Geometry2D::VTX_RGB);
		UINT lVertexSize = 0;
		switch(lVertexFormat)
		{
			case 0:
			{
				lVertexSize = sizeof(Geometry2D::VertexXY);
			} break;
			case Geometry2D::VTX_RGB:
			{
				lFVF |= D3DFVF_DIFFUSE;
				lVertexSize = sizeof(Geometry2D::VertexXYRGB);
			} break;
			case Geometry2D::VTX_UV:
			{
				lFVF |= D3DFVF_TEX1;
				lVertexSize = sizeof(Geometry2D::VertexXYUV);
			} break;
			case Geometry2D::VTX_UV | Geometry2D::VTX_RGB:
			{
				lFVF |= (D3DFVF_TEX1 | D3DFVF_DIFFUSE);
				lVertexSize = sizeof(Geometry2D::VertexXYUVRGB);
			} break;
		}

		mD3DDevice->SetFVF(lFVF);
		mD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, lSE->GetGeometry().GetTriangleCount(), lSE->GetGeometry().GetVertexData(), lVertexSize);
	}

	mD3DDevice->EndScene();
	mD3DDevice->SetTexture(0, 0);

	PopAttrib();
}

LOG_CLASS_DEFINE(UI_GFX_2D, DirectX9Painter);



}
