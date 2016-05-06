
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../../lepra/include/lepraos.h"
#include "../../include/win32/uiwin32directx9painter.h"
#include <math.h>
#ifdef new
#undef new
#endif // new
#include <D3dx9math.h>
#include "../../../lepra/include/log.h"
#include "../../../uilepra/include/win32/uiwin32directxdisplay.h"

#ifdef DEBUG
#pragma comment(lib, "d3dx9d.lib")
#else
#pragma comment(lib, "d3dx9.lib")
#endif



namespace uitbc {



DWORD ToArgb(DWORD alpha, const Color& _color) {
	return alpha | (((DWORD)_color.red_)   << 16) |
	                (((DWORD)_color.green_) << 8) |
	                (((DWORD)_color.blue_)  << 0);
}

DirectX9Painter::DirectX9Painter(uilepra::DisplayManager* display_manager):
	texture_id_manager_(3, 10000, 0),
	d3_d_device_(0),
	d3_d_default_mouse_cursor_(0) {
	if (display_manager == 0 || display_manager->GetContextType() != uilepra::DisplayManager::kDirectxContext) {
		d3_d_device_ = 0;
	} else {
		d3_d_device_ = ((uilepra::Win32DirectXDisplay*)display_manager)->GetD3DDevice();
	}
}

DirectX9Painter::~DirectX9Painter() {
	TextureTable::Iterator iter = texture_table_.First();
	while (iter != texture_table_.End()) {
		Texture* texture = *iter;

		int texture_name = (int)iter.GetKey();
		texture_id_manager_.RecycleId(texture_name);

		texture_table_.Remove(iter++);
		delete texture;
	}
}

void DirectX9Painter::SetDestCanvas(Canvas* canvas) {
	Painter::SetDestCanvas(canvas);
	ResetClippingRect();
}

void DirectX9Painter::SetRenderMode(RenderMode rm) {
	if (rm != GetRenderMode()) {
		Painter::SetRenderMode(rm);
		DoSetRenderMode();
	}
}

void DirectX9Painter::Clear(const Color& _color) {
	_color;
	// TODO: implement!
}

void DirectX9Painter::PrePaint(bool clear_depth_buffer) {
	clear_depth_buffer;
	DoSetRenderMode();
}

void DirectX9Painter::SetAlphaValue(uint8 alpha) {
	Painter::SetAlphaValue(alpha);
	d3_d_device_->SetRenderState(D3DRS_ALPHAREF, GetAlphaValue());
}

void DirectX9Painter::SetClippingRect(int _left, int _top, int _right, int _bottom) {
	Painter::SetClippingRect(_left, _top, _right, _bottom);

	ToScreenCoords(_left, _top);
	ToScreenCoords(_right, _bottom);

	RECT _rect;
	_rect.left   = _left;
	_rect.top    = _top;
	_rect.right  = _right;
	_rect.bottom = _bottom;
	d3_d_device_->SetScissorRect(&_rect);
}

void DirectX9Painter::ResetClippingRect() {
	D3DXMATRIX ortho_mtx;
	D3DXMATRIX identity_mtx;

	//Setup the orthogonal projection matrix and the default world/view matrix
	D3DXMatrixOrthoLH(&ortho_mtx, (float)GetCanvas()->GetWidth(), (float)GetCanvas()->GetHeight(), 0.0f, 1.0f);
	D3DXMatrixIdentity(&identity_mtx);

	d3_d_device_->SetTransform(D3DTS_PROJECTION, &ortho_mtx);
	d3_d_device_->SetTransform(D3DTS_WORLD, &identity_mtx);
	d3_d_device_->SetTransform(D3DTS_VIEW, &identity_mtx);

	//Make sure that the z-buffer and lighting are disabled
	d3_d_device_->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	d3_d_device_->SetRenderState(D3DRS_LIGHTING, FALSE);

	int __top  = 0;
	int __left = 0;
	int __bottom = (int)GetCanvas()->GetHeight();
	int __right  = (int)GetCanvas()->GetWidth();
	ToUserCoords(__left, __top);
	ToUserCoords(__right, __bottom);
	SetClippingRect(__left, __top, __right, __bottom);
}

void DirectX9Painter::DoSetRenderMode() const {
	switch(GetRenderMode()) {
		case Painter::kRmAlphatest: {
			d3_d_device_->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
			d3_d_device_->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
			d3_d_device_->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
			d3_d_device_->SetRenderState(D3DRS_ALPHAREF, GetAlphaValue());
			d3_d_device_->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
			d3_d_device_->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
			d3_d_device_->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
			break;
		}
		case Painter::kRmAlphablend: {
			d3_d_device_->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
			d3_d_device_->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			d3_d_device_->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
			d3_d_device_->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			d3_d_device_->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
			d3_d_device_->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
			break;
		}
		case Painter::kRmXor: {
			// There is no support of logical operations. Just do something.
			// Do just about anything! We are desperate here! :)
			d3_d_device_->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
			d3_d_device_->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			d3_d_device_->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
			d3_d_device_->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
			d3_d_device_->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
			d3_d_device_->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT);
			break;
		}
		case Painter::kRmAdd: {
			d3_d_device_->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
			d3_d_device_->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			d3_d_device_->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
			d3_d_device_->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
			d3_d_device_->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
			d3_d_device_->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
			break;
		}
		case Painter::kRmNormal:
		default: {
			d3_d_device_->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
			d3_d_device_->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
			d3_d_device_->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
			d3_d_device_->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
			d3_d_device_->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
			break;
		}
	}
}

void DirectX9Painter::DoDrawPixel(int x, int y) {
	d3_d_device_->BeginScene();

	ToScreenCoords(x, y);

	struct VertexData {
		FLOAT x, y, z, rhw; // The transformed position for the vertex.
		DWORD color;        // The vertex color.
	};

	VertexData vertex;

	DWORD _alpha = ((DWORD)GetAlphaValue()) << 24;

	Color& __color = GetColorInternal(0);
	vertex.x   = (float)x;
	vertex.y   = (float)y;
	vertex.z   = 0;
	vertex.rhw = 1;
	vertex.color = ToArgb(_alpha, __color);

	d3_d_device_->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	d3_d_device_->DrawPrimitiveUP(D3DPT_POINTLIST, 1, &vertex, sizeof(VertexData));

	d3_d_device_->EndScene();

}

void DirectX9Painter::DoDrawLine(int x1, int y1, int x2, int y2) {
	d3_d_device_->BeginScene();

	ToScreenCoords(x1, y1);
	ToScreenCoords(x2, y2);

	struct VertexData {
		FLOAT x, y, z, rhw; // The transformed position for the vertex.
		DWORD color;        // The vertex color.
	};

	VertexData vertex[2];

	DWORD _alpha = ((DWORD)GetAlphaValue()) << 24;

	Color& __color = GetColorInternal(0);
	for (int i = 0; i < 2; i++) {
		vertex[i].z   = 0;
		vertex[i].rhw = 1;
		vertex[i].color = ToArgb(_alpha, __color);
	}

	vertex[0].x   = (float)x1;
	vertex[0].y   = (float)y1;
	vertex[1].x   = (float)x2;
	vertex[1].y   = (float)y2;

	d3_d_device_->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	d3_d_device_->DrawPrimitiveUP(D3DPT_LINELIST, 1, vertex, sizeof(VertexData));

	d3_d_device_->EndScene();
}

void DirectX9Painter::DoDrawRect(int _left, int _top, int _right, int _bottom) {
	d3_d_device_->BeginScene();

	ToScreenCoords(_left, _top);
	ToScreenCoords(_right, _bottom);

	float __left   = (float)_left;
	float __top    = (float)_top;
	float __right  = (float)_right;
	float __bottom = (float)_bottom;

	struct VertexData {
		FLOAT x, y, z, rhw; // The transformed position for the vertex.
		DWORD color;        // The vertex color.
	};

	VertexData vertex[8];

	DWORD _alpha = ((DWORD)GetAlphaValue()) << 24;
	int i;
	for (i = 0; i < 8; i++) {
		Color* __color;
		if(i < 4)
			__color = &GetColorInternal(0);
		else
			__color = &GetColorInternal(1);

		vertex[i].z   = 0;
		vertex[i].rhw = 1;
		vertex[i].color = ToArgb(_alpha, *__color);
	}

	vertex[0].x   = __left;
	vertex[0].y   = __top;
	vertex[1].x   = __right;
	vertex[1].y   = __top;
	vertex[2].x   = __right;
	vertex[2].y   = __bottom;
	vertex[3].x   = __left;
	vertex[3].y   = __bottom;

	__left += width;
	__top += width;
	__right -= width;
	__bottom -= width;

	vertex[4].x   = __left;
	vertex[4].y   = __top;
	vertex[5].x   = __right;
	vertex[5].y   = __top;
	vertex[6].x   = __right;
	vertex[6].y   = __bottom;
	vertex[7].x   = __left;
	vertex[7].y   = __bottom;

	const static uint16 indices[] = {0,4,7, 0,7,3, 0,1,5, 0,5,4, 1,2,6, 1,6,5, 7,6,2, 7,2,3};

	d3_d_device_->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	d3_d_device_->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 8, 8, indices, D3DFMT_INDEX16, vertex, sizeof(VertexData));

	d3_d_device_->EndScene();
}

void DirectX9Painter::DoFillRect(int _left, int _top, int _right, int _bottom) {
	d3_d_device_->BeginScene();

	ToScreenCoords(_left, _top);
	ToScreenCoords(_right, _bottom);

	float __left   = (float)_left;
	float __top    = (float)_top;
	float __right  = (float)_right;
	float __bottom = (float)_bottom;

	struct VertexData {
		FLOAT x, y, z, rhw; // The transformed position for the vertex.
		DWORD color;        // The vertex color.
	};

	VertexData vertex[4];

	DWORD _alpha = ((DWORD)GetAlphaValue()) << 24;
	Color& __color = GetColorInternal(0);

	for (int i = 0; i < 4; i++) {
		vertex[i].z   = 0;
		vertex[i].rhw = 1;
		vertex[i].color = ToArgb(_alpha, __color);
	}

	vertex[0].x   = __left;
	vertex[0].y   = __top;
	vertex[1].x   = __right;
	vertex[1].y   = __top;
	vertex[2].x   = __right;
	vertex[2].y   = __bottom;
	vertex[3].x   = __left;
	vertex[3].y   = __bottom;

	d3_d_device_->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	d3_d_device_->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertex, sizeof(VertexData));

	d3_d_device_->EndScene();
}

void DirectX9Painter::DoDraw3DRect(int _left, int _top, int _right, int _bottom, int width, bool sunken) {
	d3_d_device_->BeginScene();

	ToScreenCoords(_left, _top);
	ToScreenCoords(_right, _bottom);

	float __left   = (float)_left;
	float __top    = (float)_top;
	float __right  = (float)_right;
	float __bottom = (float)_bottom;

	struct VertexData {
		FLOAT x, y, z, rhw; // The transformed position for the vertex.
		DWORD color;        // The vertex color.
	};

	VertexData vertex[12];

	DWORD _alpha = ((DWORD)GetAlphaValue()) << 24;

	for (int i = 0; i < 12; i++) {
		vertex[i].z   = 0;
		vertex[i].rhw = 1;
	}

	int zero  = 0;
	int one   = 1;
	int two   = 2;
	int three = 3;

	if(sunken) {
		zero  = 2;
		one   = 3;
		two   = 0;
		three = 1;
	}
	Color* __color = &GetColorInternal(0);

	vertex[0].x = __left; // Outer top left.
	vertex[0].y = __top;
	vertex[0].color = ToArgb(_alpha, __color[zero]);

	vertex[1].x = __right; // Outer top right #1.
	vertex[1].y = __top;
	vertex[1].color = ToArgb(_alpha, __color[zero]);

	vertex[2].x = __right; // Outer top right #2.
	vertex[2].y = __top;
	vertex[2].color = ToArgb(_alpha, __color[one]);

	vertex[3].x = __right; // Outer bottom right.
	vertex[3].y = __bottom;
	vertex[3].color = ToArgb(_alpha, __color[one]);

	vertex[4].x = __left; // Outer bottom left #1.
	vertex[4].y = __bottom;
	vertex[4].color = ToArgb(_alpha, __color[zero]);

	vertex[5].x = __left; // Outer bottom left #2.
	vertex[5].y = __bottom;
	vertex[5].color = ToArgb(_alpha, __color[one]);

	__left += width;
	__top += width;
	__right -= width;
	__bottom -= width;

	vertex[6].x = __left; // Inner top left.
	vertex[6].y = __top;
	vertex[6].color = ToArgb(_alpha, __color[two]);

	vertex[7].x = __right; // Inner top right #1.
	vertex[7].y = __top;
	vertex[7].color = ToArgb(_alpha, __color[two]);

	vertex[8].x = __right; // Inner top right #2.
	vertex[8].y = __top;
	vertex[8].color = ToArgb(_alpha, __color[three]);

	vertex[9].x = __right; // Inner bottom right.
	vertex[9].y = __bottom;
	vertex[9].color = ToArgb(_alpha, __color[three]);

	vertex[10].x = __left; // Inner bottom left #1.
	vertex[10].y = __bottom;
	vertex[10].color = ToArgb(_alpha, __color[two]);

	vertex[11].x = __left; // Inner bottom left #2.
	vertex[11].y = __bottom;
	vertex[11].color = ToArgb(_alpha, __color[three]);

	const static uint16 indices[] = {0,1,7, 0,7,6, 0,6,10, 0,10,4, 8,2,3, 8,3,9, 11,9,3, 11,3,5};

	d3_d_device_->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	d3_d_device_->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 12, 8, indices, D3DFMT_INDEX16, vertex, sizeof(VertexData));

	d3_d_device_->EndScene();
}

void DirectX9Painter::DoFillShadedRect(int _left, int _top, int _right, int _bottom) {
	d3_d_device_->BeginScene();

	ToScreenCoords(_left, _top);
	ToScreenCoords(_right, _bottom);

	float __left   = (float)_left;
	float __top    = (float)_top;
	float __right  = (float)_right;
	float __bottom = (float)_bottom;

	Color* __color = &GetColorInternal(0);

	// Calculate center color.
	DWORD top_r = (DWORD)__color[0].red_   + (DWORD)__color[1].red_;
	DWORD top_g = (DWORD)__color[0].green_ + (DWORD)__color[1].green_;
	DWORD top_b = (DWORD)__color[0].blue_  + (DWORD)__color[1].blue_;

	DWORD bot_r = (DWORD)__color[2].red_   + (DWORD)__color[3].red_;
	DWORD bot_g = (DWORD)__color[2].green_ + (DWORD)__color[3].green_;
	DWORD bot_b = (DWORD)__color[2].blue_  + (DWORD)__color[3].blue_;

	DWORD center_r = ((top_r + bot_r) >> 2);
	DWORD center_g = ((top_g + bot_g) >> 2);
	DWORD center_b = ((top_b + bot_b) >> 2);

	DWORD _alpha = ((DWORD)GetAlphaValue()) << 24;

	struct VertexData {
		FLOAT x, y, z, rhw; // The transformed position for the vertex.
		DWORD color;        // The vertex color.
	};

	VertexData vertex[6];

	for (int i = 0; i < 6; i++) {
		vertex[i].z   = 0;
		vertex[i].rhw = 1;
	}

	vertex[0].x   = (__left + __right) * 0.5f;
	vertex[0].y   = (__top + __bottom) * 0.5f;
	vertex[0].color = _alpha | ((center_r)   << 16) |
					((center_g) << 8) |
					((center_b)  << 0);

	vertex[1].x   = __left;
	vertex[1].y   = __top;
	vertex[1].color = ToArgb(_alpha, __color[0]);

	vertex[2].x   = __right;
	vertex[2].y   = __top;
	vertex[2].color = ToArgb(_alpha, __color[1]);

	vertex[3].x   = __right;
	vertex[3].y   = __bottom;
	vertex[3].color = ToArgb(_alpha, __color[2]);

	vertex[4].x   = __left;
	vertex[4].y   = __bottom;
	vertex[4].color = ToArgb(_alpha, __color[3]);

	vertex[5].x   = __left;
	vertex[5].y   = __top;
	vertex[5].color = ToArgb(_alpha, __color[0]);

	d3_d_device_->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	d3_d_device_->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 4, vertex, sizeof(VertexData));

	d3_d_device_->EndScene();
}

void DirectX9Painter::DoFillTriangle(float x1, float y1,
				  float x2, float y2,
				  float x3, float y3) {
	d3_d_device_->BeginScene();

	ToScreenCoords(x1, y1);
	ToScreenCoords(x2, y2);
	ToScreenCoords(x3, y3);

	struct VertexData {
		FLOAT x, y, z, rhw; // The transformed position for the vertex.
		DWORD color;        // The vertex color.
	};

	VertexData vertex[3];

	DWORD _alpha = ((DWORD)GetAlphaValue()) << 24;
	Color& __color = GetColorInternal(0);

	for (int i = 0; i < 3; i++) {
		vertex[i].z   = 0;
		vertex[i].rhw = 1;
		vertex[i].color = ToArgb(_alpha, __color);
	}

	vertex[0].x   = (FLOAT)x1;
	vertex[0].y   = (FLOAT)y1;
	vertex[1].x   = (FLOAT)x2;
	vertex[1].y   = (FLOAT)y2;
	vertex[2].x   = (FLOAT)x3;
	vertex[2].y   = (FLOAT)y3;

	d3_d_device_->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	d3_d_device_->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, vertex, sizeof(VertexData));

	d3_d_device_->EndScene();
}

void DirectX9Painter::DoFillShadedTriangle(float x1, float y1,
					float x2, float y2,
					float x3, float y3) {
	d3_d_device_->BeginScene();

	ToScreenCoords(x1, y1);
	ToScreenCoords(x2, y2);
	ToScreenCoords(x3, y3);

	struct VertexData {
		FLOAT x, y, z, rhw; // The transformed position for the vertex.
		DWORD color;        // The vertex color.
	};

	VertexData vertex[3];

	DWORD _alpha = ((DWORD)GetAlphaValue()) << 24;
	Color* __color = &GetColorInternal(0);

	for (int i = 0; i < 3; i++) {
		vertex[i].z   = 0;
		vertex[i].rhw = 1;
		vertex[i].color = ToArgb(_alpha, __color[i]);
	}

	vertex[0].x   = (FLOAT)x1;
	vertex[0].y   = (FLOAT)y1;
	vertex[1].x   = (FLOAT)x2;
	vertex[1].y   = (FLOAT)y2;
	vertex[2].x   = (FLOAT)x3;
	vertex[2].y   = (FLOAT)y3;

	d3_d_device_->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	d3_d_device_->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, vertex, sizeof(VertexData));

	d3_d_device_->EndScene();
}

void DirectX9Painter::DoFillTriangle(float x1, float y1, float u1, float v1,
				  float x2, float y2, float u2, float v2,
				  float x3, float y3, float u3, float v3,
				  ImageID image_id) {
	TextureTable::Iterator iter = texture_table_.Find((int)image_id);
	if (iter == texture_table_.End()) {
		return;
	}

	Texture* texture = *iter;

	d3_d_device_->BeginScene();

	ToScreenCoords(x1, y1);
	ToScreenCoords(x2, y2);
	ToScreenCoords(x3, y3);

	struct VertexData {
		FLOAT x, y, z, rhw; // The transformed position for the vertex.
		DWORD color;	    // TODO: Verify that this works.
		FLOAT u, v;
	};

	VertexData vertex[3];

	DWORD _alpha = ((DWORD)GetAlphaValue()) << 24;
	Color* __color = &GetColorInternal(0);

	for (int i = 0; i < 3; i++) {
		vertex[i].z   = 0;
		vertex[i].rhw = 1;

		// TODO: Verify that this works.
		vertex[i].color = ToArgb(_alpha, __color[i]);
	}

	vertex[0].x   = (FLOAT)x1;
	vertex[0].y   = (FLOAT)y1;
	vertex[0].u   = (FLOAT)u1;
	vertex[0].v   = (FLOAT)v1;
	vertex[1].x   = (FLOAT)x2;
	vertex[1].y   = (FLOAT)y2;
	vertex[1].u   = (FLOAT)u2;
	vertex[1].v   = (FLOAT)v2;
	vertex[2].x   = (FLOAT)x3;
	vertex[2].y   = (FLOAT)y3;
	vertex[2].u   = (FLOAT)u3;
	vertex[2].v   = (FLOAT)v3;

	d3_d_device_->SetTexture(0, texture->d3_d_texture_);

	d3_d_device_->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
	d3_d_device_->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, vertex, sizeof(VertexData));

	d3_d_device_->EndScene();

	d3_d_device_->SetTexture(0, 0);
}

void DirectX9Painter::DrawFan(const std::vector<vec2>& coords, bool fill) {
	coords;
	fill;
}

Painter::ImageID DirectX9Painter::AddImage(const Canvas* image, const Canvas* alpha_buffer) {
	if (image == 0 && alpha_buffer == 0) {
		return (ImageID)texture_id_manager_.GetInvalidId();
	}

	int id = 0;

	bool _alpha = false;
	bool __color = false;

	if (image != 0) {
		__color = true;

		if (image->GetBitDepth() == Canvas::kBitdepth32Bit) {
			_alpha = true;
		}
	}

	if (alpha_buffer != 0) {
		_alpha = true;
	}

	id = texture_id_manager_.GetFreeId();

	if (id != texture_id_manager_.GetInvalidId()) {
		Canvas _image;
		D3DFORMAT format = kD3DfmtR8G8B8;
		bool create_texture = false;

		if (__color == true && _alpha == true) {
			_image.Copy(*image);

			unsigned new_width  = GetClosestPowerOf2(_image.GetWidth(), true);
			unsigned new_height = GetClosestPowerOf2(_image.GetHeight(), true);

			if (new_width != _image.GetWidth() || new_height != _image.GetHeight()) {
				_image.Resize(new_width, new_height, Canvas::kResizeNicest);
			}

			if (alpha_buffer != 0) {
				Canvas _alpha_buffer(*alpha_buffer, true);

				if (_alpha_buffer.GetWidth() != _image.GetWidth() ||
				   _alpha_buffer.GetHeight() != _image.GetHeight()) {
					_alpha_buffer.Resize(_image.GetWidth(), _image.GetHeight(), Canvas::kResizeFast);
				}
				if (_alpha_buffer.GetBitDepth() != Canvas::kBitdepth8Bit) {
					_alpha_buffer.ConvertToGrayscale(true);
				}

				_image.ConvertTo32BitWithAlpha(_alpha_buffer);
			}

			format = kD3DfmtA8R8G8B8;
			create_texture = true;
		} else if(__color == true) {
			_image.Copy(*image);
			_image.ConvertBitDepth(Canvas::kBitdepth24Bit);

			unsigned new_width  = GetClosestPowerOf2(_image.GetWidth(), true);
			unsigned new_height = GetClosestPowerOf2(_image.GetHeight(), true);
			if (new_width != _image.GetWidth() || new_height != _image.GetHeight()) {
				_image.Resize(new_width, new_height, Canvas::kResizeFast);
			}

			format = kD3DfmtR8G8B8;
			create_texture = true;
		} else if(alpha_buffer != 0) {
			_image.Copy(*alpha_buffer);

			Color palette[256];
			for (int i = 0; i < 256; i++) {
				palette[i].red_   = (uint8)i;
				palette[i].green_ = (uint8)i;
				palette[i].blue_  = (uint8)i;
			}

			_image.SetPalette(palette);
			_image.ConvertTo32BitWithAlpha(*alpha_buffer);

			format = kD3DfmtA8R8G8B8;
			create_texture = true;
		}

		if (create_texture == true) {
			Texture* texture = new Texture();

			if (image != 0) {
				texture->width_ = image->GetWidth();
				texture->height_ = image->GetHeight();
			} else {
				texture->width_ = alpha_buffer->GetWidth();
				texture->height_ = alpha_buffer->GetHeight();
			}

			HRESULT res = d3_d_device_->CreateTexture(_image.GetWidth(), _image.GetHeight(), 1, 0,
				format, D3DPOOL_DEFAULT, &texture->d3_d_texture_, NULL);

			if (FAILED(res)) {
				delete texture;
				texture_id_manager_.RecycleId(id);
				id = texture_id_manager_.GetInvalidId();
			} else {
				IDirect3DSurface9* surface;
				if (FAILED(texture->d3_d_texture_->GetSurfaceLevel(0, &surface))) {
					log_.Error("AddImage() - Failed to get surface level 0!");
					texture->d3_d_texture_->Release();
					delete texture;
					texture_id_manager_.RecycleId(id);
					return (ImageID)texture_id_manager_.GetInvalidId();
				}
				RECT _rect;
				_rect.top = 0;
				_rect.left = 0;
				_rect.bottom = _image.GetHeight();
				_rect.right = _image.GetWidth();

				D3DXLoadSurfaceFromMemory(surface, 0, 0, _image.GetBuffer(), format,
					_image.GetPitch() * _image.GetPixelByteSize(), 0, &_rect, D3DX_FILTER_NONE, 0);

				texture_table_.Insert(id, texture);
			}
		}
	}

	return (ImageID)id;
}

void DirectX9Painter::UpdateImage(ImageID image_id, const Canvas* image, const Canvas* alpha_buffer, UpdateHint hint) {
	TextureTable::Iterator iter = texture_table_.Find(image_id);
	if (iter == texture_table_.End()) {
		return;
	}
	Texture* texture = *iter;

	if (hint == kUpdateFast) {
		// Perform a fast update... Only consider the color map.
		if (image != 0 &&
		   (int)image->GetWidth() == texture->width_ &&
		   (int)image->GetHeight() == texture->height_) {
			IDirect3DSurface9* surface;
			if (FAILED(texture->d3_d_texture_->GetSurfaceLevel(0, &surface))) {
				log_.Error("UpdateImage() - Failed to get surface level 0!");
				return;
			}

			RECT _rect;
			_rect.top = 0;
			_rect.left = 0;
			_rect.bottom = image->GetHeight();
			_rect.right = image->GetWidth();

			if (image->GetBitDepth() == Canvas::kBitdepth24Bit) {
				D3DXLoadSurfaceFromMemory(surface, 0, 0, image->GetBuffer(),
							  kD3DfmtR8G8B8, image->GetPitch() * image->GetPixelByteSize(),
							  0, &_rect, D3DX_FILTER_NONE, 0);
			} else if(image->GetBitDepth() == Canvas::kBitdepth32Bit) {
				D3DXLoadSurfaceFromMemory(surface, 0, 0, image->GetBuffer(),
							  kD3DfmtA8R8G8B8, image->GetPitch() * image->GetPixelByteSize(),
							  0, &_rect, D3DX_FILTER_NONE, 0);
			}
		}
		return;
	}


	bool _alpha = false;
	bool __color = false;

	if (image != 0) {
		__color = true;

		if (image->GetBitDepth() == Canvas::kBitdepth32Bit) {
			_alpha = true;
		}
	}

	if (alpha_buffer != 0) {
		_alpha = true;
	}

	Canvas _image;
	D3DFORMAT format = kD3DfmtR8G8B8;
	bool create_texture = false;

	if (__color == true && _alpha == true) {
		_image.Copy(*image);

		unsigned new_width  = GetClosestPowerOf2(_image.GetWidth(), true);
		unsigned new_height = GetClosestPowerOf2(_image.GetHeight(), true);

		if (new_width != _image.GetWidth() || new_height != _image.GetHeight()) {
			_image.Resize(new_width, new_height, Canvas::kResizeNicest);
		}

		if (alpha_buffer != 0) {
			Canvas _alpha_buffer(*alpha_buffer, true);

			if (_alpha_buffer.GetWidth() != _image.GetWidth() ||
			   _alpha_buffer.GetHeight() != _image.GetHeight()) {
				_alpha_buffer.Resize(_image.GetWidth(), _image.GetHeight(), Canvas::kResizeFast);
			}
			if (_alpha_buffer.GetBitDepth() != Canvas::kBitdepth8Bit) {
				_alpha_buffer.ConvertToGrayscale(true);
			}

			_image.ConvertTo32BitWithAlpha(_alpha_buffer);
		}

		format = kD3DfmtA8R8G8B8;
		create_texture = true;
	} else if(__color == true) {
		Canvas _image(*image, true);
		_image.ConvertBitDepth(Canvas::kBitdepth24Bit);

		unsigned new_width  = GetClosestPowerOf2(_image.GetWidth(), true);
		unsigned new_height = GetClosestPowerOf2(_image.GetHeight(), true);
		if (new_width != _image.GetWidth() || new_height != _image.GetHeight()) {
			_image.Resize(new_width, new_height, Canvas::kResizeFast);
		}

		format = kD3DfmtR8G8B8;
		create_texture = true;
	} else if(alpha_buffer != 0) {
		Canvas _image(*alpha_buffer, true);

		Color palette[256];
		for (int i = 0; i < 256; i++) {
			palette[i].red_   = (uint8)i;
			palette[i].green_ = (uint8)i;
			palette[i].blue_  = (uint8)i;
		}

		_image.SetPalette(palette);
		_image.ConvertTo32BitWithAlpha(*alpha_buffer);

		format = kD3DfmtA8R8G8B8;
		create_texture = true;
	}

	if (create_texture == true) {
		IDirect3DTexture9* d3_d_texture;
		HRESULT res = d3_d_device_->CreateTexture(_image.GetWidth(), _image.GetHeight(), 1, 0, format,
			D3DPOOL_DEFAULT, &d3_d_texture, NULL);

		if (FAILED(res)) {
			return;
		} else {
			IDirect3DSurface9* surface;
			if (FAILED(d3_d_texture->GetSurfaceLevel(0, &surface))) {
				d3_d_texture->Release();
				log_.Error("UpdateImage() - Failed to get surface level 0!");
				return;
			}
			texture->width_ = image->GetWidth();
			texture->height_ = image->GetHeight();
			texture->d3_d_texture_->Release();
			texture->d3_d_texture_ = d3_d_texture;

			RECT _rect;
			_rect.top = 0;
			_rect.left = 0;
			_rect.bottom = _image.GetHeight();
			_rect.right = _image.GetWidth();

			D3DXLoadSurfaceFromMemory(surface, 0, 0, _image.GetBuffer(),
						  format, _image.GetPitch() * _image.GetPixelByteSize(),
						  0, &_rect, D3DX_FILTER_NONE, 0);
		}
	}
}

void DirectX9Painter::RemoveImage(ImageID image_id) {
	TextureTable::Iterator iter = texture_table_.Find(image_id);
	if (iter == texture_table_.End()) {
		return;
	}

	Texture* texture = *iter;
	texture_table_.Remove(iter);

	texture->d3_d_texture_->Release();
	delete texture;
}

void DirectX9Painter::DoDrawImage(ImageID image_id, int x, int y) {
	TextureTable::Iterator iter = texture_table_.Find(image_id);

	if (iter == texture_table_.End()) {
		return;
	}

	Texture* texture = *iter;

	ToScreenCoords(x, y);
	PixelRect _rect(x, y, x + texture->width_, y + texture->height_);
	ToUserCoords(_rect.left_, _rect.top_);
	ToUserCoords(_rect.right_, _rect.bottom_);
	DrawImage(image_id, _rect);
}

void DirectX9Painter::DoDrawImage(ImageID image_id, int x, int y, const PixelRect& subpatch_rect) {
	TextureTable::Iterator iter = texture_table_.Find(image_id);

	if (iter == texture_table_.End()) {
		return;
	}

	Texture* texture = *iter;

	ToScreenCoords(x, y);
	PixelRect _rect(x, y, x + texture->width_, y + texture->height_);
	ToUserCoords(_rect.left_, _rect.top_);
	ToUserCoords(_rect.right_, _rect.bottom_);
	DrawImage(image_id, _rect, subpatch_rect);
}

void DirectX9Painter::DoDrawImage(ImageID image_id, const PixelRect& rect) {
	TextureTable::Iterator iter = texture_table_.Find((int)image_id);
	if (iter == texture_table_.End()) {
		return;
	}

	Texture* texture = *iter;

	d3_d_device_->BeginScene();

	float __left   = (float)rect.left_;
	float __right  = (float)rect.right_;
	float __top    = (float)rect.top_;
	float __bottom = (float)rect.bottom_;

	ToScreenCoords(__left, __top);
	ToScreenCoords(__right, __bottom);

	struct VertexData {
		FLOAT x, y, z, rhw; // The transformed position for the vertex.
		DWORD color;        // The vertex color.
		FLOAT u, v;
	};

	VertexData vertex[4];

	DWORD _alpha = ((DWORD)GetAlphaValue()) << 24;

	if (GetRenderMode() == kRmAlphatest) {
		_alpha = 0xFF000000;
	}

	Color& __color = GetColorInternal(0);
	for (int i = 0; i < 4; i++) {
		vertex[i].z   = 0;
		vertex[i].rhw = 1;
		vertex[i].color = ToArgb(_alpha, __color);
	}

	vertex[0].x   = (float)__left;
	vertex[0].y   = (float)__top;
	vertex[0].u   = 0;
	vertex[0].v   = 0;
	vertex[1].x   = (float)__right;
	vertex[1].y   = (float)__top;
	vertex[1].u   = 1;
	vertex[1].v   = 0;
	vertex[2].x   = (float)__right;
	vertex[2].y   = (float)__bottom;
	vertex[2].u   = 1;
	vertex[2].v   = 1;
	vertex[3].x   = (float)__left;
	vertex[3].y   = (float)__bottom;
	vertex[3].u   = 0;
	vertex[3].v   = 1;

	d3_d_device_->SetTexture(0, texture->d3_d_texture_);

	d3_d_device_->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	d3_d_device_->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	d3_d_device_->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT/*D3DTA_DIFFUSE*/);

	if (GetRenderMode() == kRmAlphatest ||
	   GetRenderMode() == kRmAlphablend) {
		d3_d_device_->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
	} else {
		d3_d_device_->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
	}


	d3_d_device_->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	d3_d_device_->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertex, sizeof(VertexData));

	d3_d_device_->EndScene();
	d3_d_device_->SetTexture(0, 0);
}

void DirectX9Painter::DoDrawImage(ImageID image_id, const PixelRect& rect, const PixelRect& subpatch_rect) {
	TextureTable::Iterator iter = texture_table_.Find((int)image_id);
	if (iter == texture_table_.End()) {
		return;
	}

	Texture* texture = *iter;

	d3_d_device_->BeginScene();

	float __left   = (float)rect.left_;
	float __right  = (float)rect.right_;
	float __top    = (float)rect.top_;
	float __bottom = (float)rect.bottom_;

	ToScreenCoords(__left, __top);
	ToScreenCoords(__right, __bottom);

	float one_over_width  = 1.0f / (float)texture->width_;
	float one_over_height = 1.0f / (float)texture->height_;
	float u_left   = ((float)subpatch_rect.left_   + 0.5f) * one_over_width;
	float u_right  = ((float)subpatch_rect.right_  + 0.5f) * one_over_width;
	float v_top    = ((float)subpatch_rect.top_    + 0.5f) * one_over_height;
	float v_bottom = ((float)subpatch_rect.bottom_ + 0.5f) * one_over_height;

	struct VertexData {
		FLOAT x, y, z, rhw; // The transformed position for the vertex.
		DWORD color;        // The vertex color.
		FLOAT u, v;
	};

	VertexData vertex[4];

	DWORD _alpha = ((DWORD)GetAlphaValue()) << 24;

	if (GetRenderMode() == kRmAlphatest) {
		_alpha = 0xFF000000;
	}

	Color& __color = GetColorInternal(0);
	for (int i = 0; i < 4; i++) {
		vertex[i].z   = 0;
		vertex[i].rhw = 1;
		vertex[i].color = ToArgb(_alpha, __color);
	}

	vertex[0].x   = (float)__left;
	vertex[0].y   = (float)__top;
	vertex[0].u   = u_left;
	vertex[0].v   = v_top;
	vertex[1].x   = (float)__right;
	vertex[1].y   = (float)__top;
	vertex[1].u   = u_right;
	vertex[1].v   = v_top;
	vertex[2].x   = (float)__right;
	vertex[2].y   = (float)__bottom;
	vertex[2].u   = u_right;
	vertex[2].v   = v_bottom;
	vertex[3].x   = (float)__left;
	vertex[3].y   = (float)__bottom;
	vertex[3].u   = u_left;
	vertex[3].v   = v_bottom;

	d3_d_device_->SetTexture(0, texture->d3_d_texture_);

	d3_d_device_->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	d3_d_device_->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	d3_d_device_->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT/*D3DTA_DIFFUSE*/);

	if (GetRenderMode() == kRmAlphatest ||
	   GetRenderMode() == kRmAlphablend) {
		d3_d_device_->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
	} else {
		d3_d_device_->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
	}


	d3_d_device_->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	d3_d_device_->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertex, sizeof(VertexData));

	d3_d_device_->EndScene();
	d3_d_device_->SetTexture(0, 0);
}

void DirectX9Painter::DoDrawAlphaImage(ImageID image_id, int x, int y) {
	ToScreenCoords(x, y);

	TextureTable::Iterator iter = texture_table_.Find(image_id);

	if (iter == texture_table_.End()) {
		return;
	}

	Texture* texture = *iter;

	float __left   = (float)x;
	float __right  = (float)(x + texture->width_);
	float __top    = (float)y;
	float __bottom = (float)(y + texture->height_);

	struct VertexData {
		FLOAT x, y, z, rhw; // The transformed position for the vertex.
		DWORD color;        // The vertex color.
		FLOAT u, v;
	};

	VertexData vertex[4];

	DWORD _alpha = ((DWORD)GetAlphaValue()) << 24;

	if (GetRenderMode() == kRmAlphatest) {
		_alpha = 0xFF000000;
	}

	Color& __color = GetColorInternal(0);
	for (int i = 0; i < 4; i++) {
		vertex[i].z   = 0;
		vertex[i].rhw = 1;
		vertex[i].color = ToArgb(_alpha, __color);
	}

	vertex[0].x   = (float)__left;
	vertex[0].y   = (float)__top;
	vertex[0].u   = 0;
	vertex[0].v   = 0;
	vertex[1].x   = (float)__right;
	vertex[1].y   = (float)__top;
	vertex[1].u   = 1;
	vertex[1].v   = 0;
	vertex[2].x   = (float)__right;
	vertex[2].y   = (float)__bottom;
	vertex[2].u   = 1;
	vertex[2].v   = 1;
	vertex[3].x   = (float)__left;
	vertex[3].y   = (float)__bottom;
	vertex[3].u   = 0;
	vertex[3].v   = 1;

	d3_d_device_->SetTexture(0, texture->d3_d_texture_);

	d3_d_device_->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	d3_d_device_->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	d3_d_device_->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

	if (GetRenderMode() == kRmAlphatest ||
	   GetRenderMode() == kRmAlphablend) {
		d3_d_device_->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
	} else {
		d3_d_device_->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
	}


	d3_d_device_->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	d3_d_device_->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertex, sizeof(VertexData));

	d3_d_device_->EndScene();
	d3_d_device_->SetTexture(0, 0);
}

void DirectX9Painter::PrintText(const str& s, int x, int y) {
	s;
	x;
	y;

	// TODO: there's probably some smart and fast way to render fonts already in DX.
}

void DirectX9Painter::SetFontSmoothness(bool) {
	// TODO: ?
}

void DirectX9Painter::AdjustVertexFormat(uint16& vertex_format) {
	vertex_format |= Geometry2D::kVtxInterleaved;
}

void DirectX9Painter::ReadPixels(Canvas& dest_canvas, const PixelRect& rect) {
	if (GetCanvas() == 0 || GetCanvas()->GetBitDepth() == Canvas::kBitdepth8Bit) {
		dest_canvas.Reset(0, 0, Canvas::kBitdepth32Bit);
		return;
	}

	PixelRect _rect(rect);

	ToScreenCoords(_rect.left_, _rect.top_);
	ToScreenCoords(_rect.right_, _rect.bottom_);

	if (_rect.left_ < 0) {
		_rect.left_ = 0;
	}
	if (_rect.top_ < 0) {
		_rect.top_ = 0;
	}
	if (_rect.right_ > (int)GetCanvas()->GetWidth()) {
		_rect.right_ = (int)GetCanvas()->GetWidth();
	}
	if (_rect.bottom_ > (int)GetCanvas()->GetHeight()) {
		_rect.bottom_ = (int)GetCanvas()->GetHeight();
	}

	if (_rect.right_ <= _rect.left_ ||
	   _rect.bottom_ <= _rect.top_) {
		dest_canvas.Reset(0, 0, Canvas::kBitdepth32Bit);
		return;
	}

	LPDIRECT3DSURFACE9 render_target = 0;
	LPDIRECT3DSURFACE9 in_mem_target = 0;
	D3DSURFACE_DESC	surf_desc;
	HRESULT	h_res;

	h_res = d3_d_device_->GetRenderTarget(0, &render_target);
	if (FAILED(h_res)) {
		log_.Error("GetRenderTarget failed in ReadPixels().");
		return;
	}

	h_res = render_target->GetDesc(&surf_desc);
	if (FAILED(h_res)) {
		log_.Error("GetDesc failed in ReadPixels().");
		return;
	}

	h_res = d3_d_device_->CreateOffscreenPlainSurface(surf_desc.Width,
													   surf_desc.Height,
													   surf_desc.Format,
													   kD3DpoolSystemmem,
													   &in_mem_target, NULL);
	if (FAILED(h_res)) {
		log_.Error("CreateOffscreenPlainSurface failed in ReadPixels().");
		return;
	}

	h_res = d3_d_device_->GetRenderTargetData(render_target, in_mem_target);
	if (FAILED(h_res)) {
		log_.Error("GetRenderTargetData failed in ReadPixels().");
		in_mem_target->Release();
		return;
	}

	D3DLOCKED_RECT locked_rect;
	if (FAILED(in_mem_target->LockRect(&locked_rect, NULL, 0))) {
		log_.Error("LockRect failed in ReadPixels().");
		in_mem_target->Release();
		return;
	}

	Canvas::BitDepth bit_depth = Canvas::kBitdepth32Bit;

	switch(surf_desc.Format) {
	case kD3DfmtR8G8B8:
		bit_depth = Canvas::kBitdepth24Bit;
		break;
	case kD3DfmtA8R8G8B8:
	case D3DFMT_X8R8G8B8:
	case D3DFMT_A8B8G8R8:
	case D3DFMT_X8B8G8R8:
		bit_depth = Canvas::kBitdepth32Bit;
		break;
	case D3DFMT_R5G6B5:
		bit_depth = Canvas::kBitdepth16Bit;
		break;
	case D3DFMT_A1R5G5B5:
	case D3DFMT_X1R5G5B5:
		bit_depth = Canvas::kBitdepth15Bit;
		break;
	case D3DFMT_R3G3B2:
	case D3DFMT_A8:
	case D3DFMT_L8:
	case D3DFMT_P8:
		bit_depth = Canvas::kBitdepth8Bit;
		dest_canvas.SetPalette(GetCanvas()->GetPalette());
		break;
	default:
		log_.Error("Invalid pixel format in ReadPixels().");
		dest_canvas.Reset(0, 0, Canvas::kBitdepth32Bit);
		in_mem_target->UnlockRect();
		in_mem_target->Release();
		return;
	}

	dest_canvas.Reset(surf_desc.Width, surf_desc.Height, bit_depth);
	dest_canvas.SetPitch(locked_rect.Pitch / dest_canvas.GetPixelByteSize());
	dest_canvas.SetBuffer(locked_rect.bits);

	// Crop the image. Note that this call will make a copy of the image buffer,
	// even if the cropped dimensions equals the current dimensions.
	dest_canvas.Crop(_rect.left_, _rect.top_, _rect.right_, _rect.bottom_);

	in_mem_target->UnlockRect();
	in_mem_target->Release();
}

Painter::RGBOrder DirectX9Painter::GetRGBOrder() const {
	return Painter::kRgb;
}

void DirectX9Painter::DoRenderDisplayList(std::vector<DisplayEntity*>* display_list) {
	PushAttrib(kAttrAll);

	d3_d_device_->BeginScene();

	std::vector<DisplayEntity*>::iterator it;
	for(it = display_list->begin(); it != display_list->end(); ++it) {
		DisplayEntity* se = *it;
		Painter::SetClippingRect(se->GetClippingRect());
		SetAlphaValue(se->GetAlpha());
		SetRenderMode(se->GetRenderMode());

		DWORD fvf = D3DFVF_XYZRHW;

		if(se->GetImageID() == Painter::kInvalidImageid) {
			d3_d_device_->SetTexture(0, 0);
		} else {
			TextureTable::Iterator iter = texture_table_.Find((int)se->GetImageID());
			Texture* texture = *iter;
			d3_d_device_->SetTexture(0, texture->d3_d_texture_);

			d3_d_device_->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
			d3_d_device_->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			d3_d_device_->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT/*D3DTA_DIFFUSE*/);

			if (GetRenderMode() == kRmAlphatest || GetRenderMode() == kRmAlphablend) {
				d3_d_device_->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
			} else {
				d3_d_device_->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
			}
		}

		uint16 _vertex_format = se->GetGeometry().GetVertexFormat() & (Geometry2D::kVtxUv | Geometry2D::kVtxRgb);
		UINT vertex_size = 0;
		switch(_vertex_format) {
			case 0: {
				vertex_size = sizeof(Geometry2D::VertexXY);
			} break;
			case Geometry2D::kVtxRgb: {
				fvf |= D3DFVF_DIFFUSE;
				vertex_size = sizeof(Geometry2D::VertexXYRGB);
			} break;
			case Geometry2D::kVtxUv: {
				fvf |= D3DFVF_TEX1;
				vertex_size = sizeof(Geometry2D::VertexXYUV);
			} break;
			case Geometry2D::kVtxUv | Geometry2D::kVtxRgb: {
				fvf |= (D3DFVF_TEX1 | D3DFVF_DIFFUSE);
				vertex_size = sizeof(Geometry2D::VertexXYUVRGB);
			} break;
		}

		d3_d_device_->SetFVF(fvf);
		d3_d_device_->DrawPrimitiveUP(D3DPT_TRIANGLELIST, se->GetGeometry().GetTriangleCount(), se->GetGeometry().GetVertexData(), vertex_size);
	}

	d3_d_device_->EndScene();
	d3_d_device_->SetTexture(0, 0);

	PopAttrib();
}

loginstance(kUiGfx2D, DirectX9Painter);



}
