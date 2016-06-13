
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/uiopenglpainter.h"
#include <math.h>
#include "../../lepra/include/hashutil.h"
#include "../include/uifontmanager.h"
#include "../include/uifonttexture.h"



namespace uitbc {



#ifdef LEPRA_DEBUG
#define OGL_ASSERT()	{ GLenum gl_error = glGetError(); deb_assert(gl_error == GL_NO_ERROR); }
#else // !Debug
#define OGL_ASSERT()
#endif // Debug / !Debug



OpenGLPainter::OpenGLPainter() :
	texture_id_manager_(3, 10000, 0),
	smooth_font_(true) {
}

OpenGLPainter::~OpenGLPainter() {
	//OGL_ASSERT();

	ClearFontBuffers();

	TextureTable::Iterator iter = texture_table_.First();
	while (iter != texture_table_.End()) {
		Texture* texture = *iter;

		GLuint texture_name = (GLuint)iter.GetKey();
		glDeleteTextures(1, &texture_name);

		texture_table_.Remove(iter++);
		delete texture;
	}

	//OGL_ASSERT();
}

void OpenGLPainter::SetDestCanvas(Canvas* canvas) {
	Painter::SetDestCanvas(canvas);
	ResetClippingRect();
}

void OpenGLPainter::SetAlphaValue(uint8 alpha) {
	OGL_ASSERT();

	if (GetAlphaValue() != alpha) {
		Painter::SetAlphaValue(alpha);
		float _alpha = (float)GetAlphaValue() / 255.0f;
		::glAlphaFunc(GL_GEQUAL, (GLclampf)_alpha);
		OGL_ASSERT();
	}
}

void OpenGLPainter::SetRenderMode(RenderMode rm) {
	if (rm != GetRenderMode()) {
		Painter::SetRenderMode(rm);
		DoSetRenderMode();
	}
}

void OpenGLPainter::Clear(const Color& color) {
	::glClearColor(color.GetRf(), color.GetGf(), color.GetBf(), 1.0f);
	::glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLPainter::PrePaint(bool clear_depth_buffer) {
	if (clear_depth_buffer) {
		::glClear(GL_DEPTH_BUFFER_BIT);
	}
#ifndef LEPRA_GL_ES
	::glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	::glDisableClientState(GL_INDEX_ARRAY);
#endif // !GLES
	::glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);
	::glLineWidth(1);
	DoSetRenderMode();
	::glDisable(GL_TEXTURE_2D);
	::glDisable(GL_CULL_FACE);
	::glDisable(GL_LINE_SMOOTH);
	::glEnableClientState(GL_VERTEX_ARRAY);
	::glDisableClientState(GL_NORMAL_ARRAY);
	::glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	if (uilepra::OpenGLExtensions::IsBufferObjectsSupported() == true) {
		uilepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, 0);
		uilepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void OpenGLPainter::SetClippingRect(int left, int top, int right, int bottom) {
	if (bottom <= top || right <= left) {
		return;
	}

	OGL_ASSERT();

	::glEnable(GL_SCISSOR_TEST);
	Painter::SetClippingRect(left, top, right, bottom);
	ToScreenCoords(left, top);
	ToScreenCoords(right, bottom);
	if (GetCanvas()->GetOutputRotation()%180 != 0) {
		std::swap(left, top);
		std::swap(right, bottom);
	}
	::glScissor(left, GetCanvas()->GetActualHeight() - bottom, right - left, bottom - top);

	OGL_ASSERT();
}

static void my_glOrtho(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top,
	GLfloat z_near, GLfloat z_far) {
	const float tx = - (right + left) / (right - left);
	const float ty = - (top + bottom) / (top - bottom);
	const float tz = - (z_far + z_near) / (z_far - z_near);
	const float m[16] =
	{
		2 / (right - left), 0, 0, 0,
		0, 2/(top - bottom), 0, 0,
		0, 0, -2/(z_far - z_near), 0,
		tx, ty, tz, 1
	};
	::glMultMatrixf(m);
}

void OpenGLPainter::ResetClippingRect() {
	OGL_ASSERT();

	// A call to this function should reset OpenGL's projection matrix to orthogonal
	// in order to work together with OpenGL3DAPI.
	::glViewport(0, 0, GetCanvas()->GetActualWidth(), GetCanvas()->GetActualHeight());
	::glMatrixMode(GL_PROJECTION);
	::glLoadIdentity();

	// Avoid glut, use my version instead.
	my_glOrtho(0, (float32)GetCanvas()->GetActualWidth(),
		(float32)GetCanvas()->GetActualHeight(), 0,
		0, 1);
	glRotatef((float)GetCanvas()->GetOutputRotation(), 0, 0, 1);
	if (GetCanvas()->GetOutputRotation() == 90) {
		glTranslatef(0, -(float)GetCanvas()->GetHeight(), 0);	// TRICKY: float cast necessary, otherwise nothing is shown on screen! Bug?!?
	} else if (GetCanvas()->GetOutputRotation() == -90) {
		glTranslatef(-(float)GetCanvas()->GetWidth(), 0, 0);	// TRICKY: float cast necessary, otherwise nothing is shown on screen! Bug?!?
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Issues a call to OpenGLPainter::SetRenderMode(),
	// which in turn simply sets a flag...
	SetRenderMode(GetRenderMode());

	glShadeModel(GL_SMOOTH);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);

	int _top  = 0;
	int _left = 0;
	int _bottom = (int)GetCanvas()->GetHeight();
	int _right  = (int)GetCanvas()->GetWidth();
	ToUserCoords(_left, _top);
	ToUserCoords(_right, _bottom);
	SetClippingRect(_left, _top, _right, _bottom);

	glEnableClientState(GL_VERTEX_ARRAY);
	//glDisableClientState(GL_VERTEX_ARRAY);

	OGL_ASSERT();
}

void OpenGLPainter::SetColor(const Color& color, unsigned color_index) {
	Painter::SetColor(color, color_index);
	r_col_[color_index].Set((float)color.red_ / 255.0f,
	                       (float)color.green_ / 255.0f,
			       (float)color.blue_ / 255.0f);
}

void OpenGLPainter::SetLineWidth(float pixels) {
	::glLineWidth(pixels);
}

void OpenGLPainter::DoSetRenderMode() const {
	OGL_ASSERT();

	switch(GetRenderMode()) {
		case Painter::kRmAlphatest: {
			glDisable(GL_COLOR_LOGIC_OP);
			glDisable(GL_BLEND);
			glEnable(GL_ALPHA_TEST);
			float _alpha = (float)GetAlphaValue() / 255.0f;
			glAlphaFunc(GL_GEQUAL, (GLclampf)_alpha);
			break;
		}
		case Painter::kRmAlphablend: {
			glDisable(GL_ALPHA_TEST);
			glDisable(GL_COLOR_LOGIC_OP);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			break;
		}
		case Painter::kRmXor: {
			glDisable(GL_ALPHA_TEST);
			glDisable(GL_BLEND);
			glEnable(GL_COLOR_LOGIC_OP);
			glLogicOp(GL_XOR);
			break;
		}
		case Painter::kRmAdd: {
			glDisable(GL_ALPHA_TEST);
			glDisable(GL_COLOR_LOGIC_OP);
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE);
			break;
		}
		case Painter::kRmNormal:
		default: {
			glDisable(GL_COLOR_LOGIC_OP);
			glDisable(GL_ALPHA_TEST);
			glDisable(GL_BLEND);
			break;
		}
	}

	OGL_ASSERT();
}

void OpenGLPainter::DoDrawPixel(int x, int y) {
	OGL_ASSERT();

	ToScreenCoords(x, y);

	Color& _color = GetColorInternal(0);
	glColor4ub(_color.red_, _color.green_, _color.blue_, GetAlphaValue());
	glPointSize(1);

	GLshort v[] = {(GLshort)x, (GLshort)y};
	::glVertexPointer(2, GL_SHORT, 0, v);
	::glDrawArrays(GL_POINTS, 0, 1);
	//::glDisableClientState(GL_VERTEX_ARRAY);

	OGL_ASSERT();
}

void OpenGLPainter::DoDrawLine(int x1, int y1, int x2, int y2) {
	OGL_ASSERT();

	ToScreenCoords(x1, y1);
	ToScreenCoords(x2, y2);

	Color& _color = GetColorInternal(0);
	glColor4ub(_color.red_, _color.green_, _color.blue_, GetAlphaValue());

	//glLineWidth(1);
	GLfloat v[] = {(GLfloat)x1, (GLfloat)y1, (GLfloat)x2, (GLfloat)y2};
	::glVertexPointer(2, GL_FLOAT, 0, v);
	::glDrawArrays(GL_LINES, 0, 2);

	OGL_ASSERT();
}

void OpenGLPainter::DoFillTriangle(float x1, float y1,
				 float x2, float y2,
				 float x3, float y3) {
	OGL_ASSERT();

	ToScreenCoords(x1, y1);
	ToScreenCoords(x2, y2);
	ToScreenCoords(x3, y3);

	Color& _color = GetColorInternal(0);
	glColor4ub(_color.red_, _color.green_, _color.blue_, GetAlphaValue());

	GLfloat v[] = {x1, y1, x2, y2, x3, y3};
	::glVertexPointer(2, GL_FLOAT, 0, v);
	::glDrawArrays(GL_TRIANGLES, 0, 3);

	OGL_ASSERT();
}

void OpenGLPainter::DoFillShadedTriangle(float x1, float y1,
				       float x2, float y2,
				       float x3, float y3) {
	OGL_ASSERT();

	ToScreenCoords(x1, y1);
	ToScreenCoords(x2, y2);
	ToScreenCoords(x3, y3);

	Color* _color = &GetColorInternal(0);
	GLubyte c[] = {_color[0].red_, _color[0].green_, _color[0].blue_, GetAlphaValue(),
		      _color[1].red_, _color[1].green_, _color[1].blue_, GetAlphaValue(),
		      _color[2].red_, _color[2].green_, _color[2].blue_, GetAlphaValue()};
	GLfloat v[] = {x1, y1, x2, y2, x3, y3};
	::glEnableClientState(GL_COLOR_ARRAY);
	::glColorPointer(4, GL_UNSIGNED_BYTE, 0, c);
	::glVertexPointer(2, GL_FLOAT, 0, v);
	::glDrawArrays(GL_TRIANGLES, 0, 3);
	::glDisableClientState(GL_COLOR_ARRAY);

	OGL_ASSERT();
}

void OpenGLPainter::DoFillTriangle(float x1, float y1, float u1, float v1,
				 float x2, float y2, float u2, float v2,
				 float x3, float y3, float u3, float v3,
				 ImageID image_id) {
	OGL_ASSERT();

	ToScreenCoords(x1, y1);
	ToScreenCoords(x2, y2);
	ToScreenCoords(x3, y3);

	TextureTable::Iterator iter = texture_table_.Find(image_id);

	if (iter == texture_table_.End()) {
		return;
	}

	//glPushAttrib(GL_TEXTURE_BIT);
	glEnable(GL_TEXTURE_2D);

	glBindTexture (GL_TEXTURE_2D, (unsigned)image_id);
	glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
#ifndef LEPRA_GL_ES
	glPixelStorei (GL_UNPACK_ROW_LENGTH, 0);
	glPixelStorei (GL_UNPACK_SKIP_ROWS, 0);
	glPixelStorei (GL_UNPACK_SKIP_PIXELS, 0);
#endif // !GLES
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	if (GetRenderMode() == kRmAlphatest) {
		glColor4ub(255, 255, 255, 255);
	} else {
		glColor4ub(255, 255, 255, GetAlphaValue());
	}

	GLfloat u[] = {u1, v1, u2, v2, u3, v3};
	GLfloat v[] = {x1, y1, x2, y2, x3, y3};
	::glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	::glTexCoordPointer(2, GL_FLOAT, 0, u);
	::glVertexPointer(2, GL_FLOAT, 0, v);
	::glDrawArrays(GL_TRIANGLES, 0, 3);
	::glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_TEXTURE_2D);

	OGL_ASSERT();
}

void OpenGLPainter::DoDrawRect(int left, int top, int right, int bottom) {
	std::vector<vec2> _coords;
	_coords.push_back(vec2((float)right, (float)top));
	_coords.push_back(vec2((float)left, (float)top));
	_coords.push_back(vec2((float)left, (float)bottom));
	_coords.push_back(vec2((float)right, (float)bottom));
	_coords.push_back(_coords[0]);
	DrawFan(_coords, false);
}

void OpenGLPainter::DoFillRect(int left, int top, int right, int bottom) {
	OGL_ASSERT();

	ToScreenCoords(left, top);
	ToScreenCoords(right, bottom);

	GLfloat _left   = (GLfloat)left;
	GLfloat _right  = (GLfloat)right;
	GLfloat _top    = (GLfloat)top;
	GLfloat _bottom = (GLfloat)bottom;

	Color& _color = GetColorInternal(0);
	glColor4ub(_color.red_, _color.green_, _color.blue_, GetAlphaValue());

	GLfloat v[] = {_left, _top, _right, _top, _right, _bottom, _left, _bottom};
	::glVertexPointer(2, GL_FLOAT, 0, v);
	::glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	OGL_ASSERT();
}

void OpenGLPainter::DoDraw3DRect(int left, int top, int right, int bottom, int width, bool sunken) {
	OGL_ASSERT();

	ToScreenCoords(left, top);
	ToScreenCoords(right, bottom);

	GLfloat _left   = (GLfloat)left;
	GLfloat _right  = (GLfloat)right;
	GLfloat _top    = (GLfloat)top;
	GLfloat _bottom = (GLfloat)bottom;

	GLfloat vertex[12 * 2];
	vertex[0] = _left; // Outer top left.
	vertex[1] = _top;
	vertex[2] = _right; // Outer top right #1.
	vertex[3] = _top;
	vertex[4] = _right; // Outer top right #2.
	vertex[5] = _top;
	vertex[6] = _right; // Outer bottom right.
	vertex[7] = _bottom;
	vertex[8] = _left; // Outer bottom left #1.
	vertex[9] = _bottom;
	vertex[10] = _left; // Outer bottom left #2.
	vertex[11] = _bottom;
	_left += width;
	_top += width;
	_right -= width;
	_bottom -= width;
	vertex[12] = _left; // Inner top left.
	vertex[13] = _top;
	vertex[14] = _right; // Inner top right #1.
	vertex[15] = _top;
	vertex[16] = _right; // Inner top right #2.
	vertex[17] = _top;
	vertex[18] = _right; // Inner bottom right.
	vertex[19] = _bottom;
	vertex[20] = _left; // Inner bottom left #1.
	vertex[21] = _bottom;
	vertex[22] = _left; // Inner bottom left #2.
	vertex[23] = _bottom;

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

	GLfloat _color[12 * 3];
	_color[0] = r_col_[zero].x;
	_color[1] = r_col_[zero].y;
	_color[2] = r_col_[zero].z;
	_color[3] = r_col_[zero].x;
	_color[4] = r_col_[zero].y;
	_color[5] = r_col_[zero].z;
	_color[6] = r_col_[one].x;
	_color[7] = r_col_[one].y;
	_color[8] = r_col_[one].z;
	_color[9] = r_col_[one].x;
	_color[10] = r_col_[one].y;
	_color[11] = r_col_[one].z;
	_color[12] = r_col_[zero].x;
	_color[13] = r_col_[zero].y;
	_color[14] = r_col_[zero].z;
	_color[15] = r_col_[one].x;
	_color[16] = r_col_[one].y;
	_color[17] = r_col_[one].z;

	_color[18] = r_col_[two].x;
	_color[19] = r_col_[two].y;
	_color[20] = r_col_[two].z;
	_color[21] = r_col_[two].x;
	_color[22] = r_col_[two].y;
	_color[23] = r_col_[two].z;
	_color[24] = r_col_[three].x;
	_color[25] = r_col_[three].y;
	_color[26] = r_col_[three].z;
	_color[27] = r_col_[three].x;
	_color[28] = r_col_[three].y;
	_color[29] = r_col_[three].z;
	_color[30] = r_col_[two].x;
	_color[31] = r_col_[two].y;
	_color[32] = r_col_[two].z;
	_color[33] = r_col_[three].x;
	_color[34] = r_col_[three].y;
	_color[35] = r_col_[three].z;

	const static vtx_idx_t indices[] = {0,1,7, 0,7,6, 0,6,10, 0,10,4, 8,2,3, 8,3,9, 11,9,3, 11,3,5};

	//::glDisableClientState(GL_NORMAL_ARRAY);
	//::glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	// Enabled in ResetClippingRect().
	glVertexPointer(2, GL_FLOAT, 0, vertex);
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(3, GL_FLOAT, 0, _color);
	glDrawElements(GL_TRIANGLES, 12, LEPRA_GL_INDEX_TYPE, indices);
	glDisableClientState(GL_COLOR_ARRAY);

	OGL_ASSERT();
}

void OpenGLPainter::DoFillShadedRect(int left, int top, int right, int bottom) {
	OGL_ASSERT();

	ToScreenCoords(left, top);
	ToScreenCoords(right, bottom);

	GLfloat _left   = (GLfloat)left;
	GLfloat _right  = (GLfloat)right;
	GLfloat _top    = (GLfloat)top;
	GLfloat _bottom = (GLfloat)bottom;

	// Calculate the color in the middle of the rect.
	GLfloat top_r = (GLfloat)(r_col_[0].x + r_col_[1].x) * 0.5f;
	GLfloat top_g = (GLfloat)(r_col_[0].y + r_col_[1].y) * 0.5f;
	GLfloat top_b = (GLfloat)(r_col_[0].z + r_col_[1].z) * 0.5f;

	GLfloat bot_r = (GLfloat)(r_col_[3].x + r_col_[2].x) * 0.5f;
	GLfloat bot_g = (GLfloat)(r_col_[3].y + r_col_[2].y) * 0.5f;
	GLfloat bot_b = (GLfloat)(r_col_[3].z + r_col_[2].z) * 0.5f;

	Color* _color = &GetColorInternal(0);
#define UBCOL(f) (GLubyte)((f)*128)
	GLubyte c[] = {UBCOL(top_r + bot_r), UBCOL(top_g + bot_g), UBCOL(top_b + bot_b), GetAlphaValue(),
		       _color[0].red_, _color[0].green_, _color[0].blue_, GetAlphaValue(),
		       _color[1].red_, _color[1].green_, _color[1].blue_, GetAlphaValue(),
		       _color[2].red_, _color[2].green_, _color[2].blue_, GetAlphaValue(),
		       _color[3].red_, _color[3].green_, _color[3].blue_, GetAlphaValue(),
		       _color[0].red_, _color[0].green_, _color[0].blue_, GetAlphaValue()};
	GLfloat v[] = {(_left + _right) * 0.5f, (_top + _bottom) * 0.5f, _left, _top, _right, _top, _right, _bottom, _left, _bottom, _left, _top};
	::glEnableClientState(GL_COLOR_ARRAY);
	::glColorPointer(4, GL_UNSIGNED_BYTE, 0, c);
	::glVertexPointer(2, GL_FLOAT, 0, v);
	::glDrawArrays(GL_TRIANGLE_FAN, 0, 6);
	::glDisableClientState(GL_COLOR_ARRAY);

	OGL_ASSERT();
}

void OpenGLPainter::DrawFan(const std::vector<vec2>& coords, bool fill) {
	OGL_ASSERT();

	Color& _color = GetColorInternal(0);
	::glColor4ub(_color.red_, _color.green_, _color.blue_, GetAlphaValue());
	size_t c = coords.size();
	GLfloat* v = new GLfloat[c*2];
	std::vector<vec2>::const_iterator i = coords.begin();
	for (int j = 0; i != coords.end(); ++i, ++j) {
		float x = i->x;
		float y = i->y;
		ToScreenCoords(x, y);
		v[j*2+0] = x;
		v[j*2+1] = y;
	}
	::glVertexPointer(2, GL_FLOAT, 0, v);
	::glDrawArrays(fill? GL_TRIANGLE_FAN : GL_LINE_STRIP, 0, c);
	delete[] v;

	OGL_ASSERT();
}

void OpenGLPainter::DrawImageFan(ImageID image_id, const std::vector<vec2>& coords, const std::vector<vec2>& tex_coords) {
	OGL_ASSERT();
	deb_assert(coords.size() == tex_coords.size());

	TextureTable::Iterator iter = texture_table_.Find(image_id);
	if (iter == texture_table_.End()) {
		return;
	}

	::glEnable(GL_TEXTURE_2D);

	glBindTexture (GL_TEXTURE_2D, image_id);
	glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
#ifndef LEPRA_GL_ES
	glPixelStorei (GL_UNPACK_ROW_LENGTH, 0);
	glPixelStorei (GL_UNPACK_SKIP_ROWS, 0);
	glPixelStorei (GL_UNPACK_SKIP_PIXELS, 0);
#endif // !GLES
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	if (GetRenderMode() == kRmAlphatest) {
		glColor4ub(255, 255, 255, 255);
	} else {
		glColor4ub(255, 255, 255, GetAlphaValue());
	}

	::glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	::glTexCoordPointer(2, GL_FLOAT, 0, &tex_coords[0]);
	::glVertexPointer(2, GL_FLOAT, 0, &coords[0]);
	::glDrawArrays(GL_TRIANGLE_FAN, 0, coords.size());
	::glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	::glDisable(GL_TEXTURE_2D);

	OGL_ASSERT();
}

Painter::ImageID OpenGLPainter::AddImage(const Canvas* image, const Canvas* alpha_buffer) {
	OGL_ASSERT();

	int id = 0;

	bool _alpha = false;
	bool _color = false;

	if (image != 0) {
		_color = true;

		if (image->GetBitDepth() == Canvas::kBitdepth32Bit) {
			_alpha = true;
		}
	}

	if (alpha_buffer != 0) {
		_alpha = true;
	}

	id = texture_id_manager_.GetFreeId();

	if (id != texture_id_manager_.GetInvalidId()) {
		if (_color == true && _alpha == true) {
			Canvas _image(*image, true);
			_image.SwapRGBOrder();
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

			Texture* texture = new Texture();

			texture->width_ = image->GetWidth();
			texture->height_ = image->GetHeight();

			texture_table_.Insert(id, texture);

			glBindTexture (GL_TEXTURE_2D, id);
			OGL_ASSERT();

			glTexImage2D (GL_TEXTURE_2D,
						  0,
						  GL_RGBA,
						  _image.GetWidth(),
						  _image.GetHeight(),
						  0,
						  GL_RGBA,
						  GL_UNSIGNED_BYTE,
						  _image.GetBuffer());
			OGL_ASSERT();
		} else if(_color == true) {
			Canvas _image(*image, true);
			_image.SwapRGBOrder();
			_image.ConvertBitDepth(Canvas::kBitdepth24Bit);

			unsigned new_width  = GetClosestPowerOf2(_image.GetWidth(), true);
			unsigned new_height = GetClosestPowerOf2(_image.GetHeight(), true);
			if (new_width != _image.GetWidth() || new_height != _image.GetHeight()) {
				_image.Resize(new_width, new_height, Canvas::kResizeFast);
			}

			Texture* texture = new Texture();

			texture->width_ = image->GetWidth();
			texture->height_ = image->GetHeight();

			texture_table_.Insert(id, texture);

			glBindTexture (GL_TEXTURE_2D, id);
			OGL_ASSERT();

			glTexImage2D (GL_TEXTURE_2D,
						  0,
						  GL_RGB,
						  _image.GetWidth(),
						  _image.GetHeight(),
						  0,
						  GL_RGB,
						  GL_UNSIGNED_BYTE,
						  _image.GetBuffer());
			OGL_ASSERT();
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

			Texture* texture = new Texture();

			texture->width_ = alpha_buffer->GetWidth();
			texture->height_ = alpha_buffer->GetHeight();

			texture_table_.Insert(id, texture);

			glBindTexture (GL_TEXTURE_2D, id);
			OGL_ASSERT();

			glTexImage2D (GL_TEXTURE_2D,
						  0,
						  GL_RGBA,
						  _image.GetWidth(),
						  _image.GetHeight(),
						  0,
						  GL_RGBA,
						  GL_UNSIGNED_BYTE,
						  _image.GetBuffer());
			OGL_ASSERT();
		}
	}

	return (ImageID)id;
}

void OpenGLPainter::UpdateImage(ImageID image_id,
				const Canvas* image,
				const Canvas* alpha_buffer,
				UpdateHint hint) {
	OGL_ASSERT();

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
			if (image->GetBitDepth() == Canvas::kBitdepth24Bit) {
				glBindTexture (GL_TEXTURE_2D, image_id);
				glTexImage2D (GL_TEXTURE_2D,
							  0,
							  GL_RGB,
							  image->GetWidth(),
							  image->GetHeight(),
							  0,
							  GL_RGB,
							  GL_UNSIGNED_BYTE,
							  image->GetBuffer());
			} else if(image->GetBitDepth() == Canvas::kBitdepth32Bit) {
				glBindTexture (GL_TEXTURE_2D, image_id);
				glTexImage2D (GL_TEXTURE_2D,
							  0,
							  GL_RGBA,
							  image->GetWidth(),
							  image->GetHeight(),
							  0,
							  GL_RGBA,
							  GL_UNSIGNED_BYTE,
							  image->GetBuffer());
			}
		}
		return;
	}

	bool _alpha = false;
	bool _color = false;

	if (image != 0) {
		_color = true;

		if (image->GetBitDepth() == Canvas::kBitdepth32Bit) {
			_alpha = true;
		}
	}

	if (alpha_buffer != 0) {
		_alpha = true;
	}

	if (_color == true && _alpha == true) {
		Canvas _image(*image, true);
		unsigned new_width  = GetClosestPowerOf2(_image.GetWidth(), true);
		unsigned new_height = GetClosestPowerOf2(_image.GetHeight(), true);
		if (new_width != _image.GetWidth() || new_height != _image.GetHeight()) {
			_image.Resize(new_width, new_height, Canvas::kResizeFast);
		}

		if (alpha_buffer != 0) {
			Canvas _alpha_buffer(*alpha_buffer, true);

			if (_alpha_buffer.GetWidth() != _image.GetWidth() ||
			   _alpha_buffer.GetHeight() != _image.GetHeight()) {
				_alpha_buffer.Resize(_image.GetWidth(), _image.GetHeight(), Canvas::kResizeNicest);
			}
			if (_alpha_buffer.GetBitDepth() != Canvas::kBitdepth8Bit) {
				_alpha_buffer.ConvertToGrayscale(true);
			}

			_image.ConvertTo32BitWithAlpha(_alpha_buffer);
		}

		texture->width_ = image->GetWidth();
		texture->height_ = image->GetHeight();

		glBindTexture (GL_TEXTURE_2D, image_id);

		glTexImage2D (GL_TEXTURE_2D,
					  0,
					  GL_RGBA,
					  _image.GetWidth(),
					  _image.GetHeight(),
					  0,
					  GL_RGBA,
					  GL_UNSIGNED_BYTE,
					  _image.GetBuffer());
	} else if(_color == true) {
		Canvas _image(*image, false);
		_image.ConvertBitDepth(Canvas::kBitdepth24Bit);
		unsigned new_width  = GetClosestPowerOf2(_image.GetWidth(), true);
		unsigned new_height = GetClosestPowerOf2(_image.GetHeight(), true);
		if (new_width != _image.GetWidth() || new_height != _image.GetHeight()) {
			_image.Resize(new_width, new_height, Canvas::kResizeFast);
		}

		texture->width_ = image->GetWidth();
		texture->height_ = image->GetHeight();

		texture_table_.Insert(image_id, texture);

		glBindTexture (GL_TEXTURE_2D, image_id);

		glTexImage2D (GL_TEXTURE_2D,
					  0,
					  GL_RGB,
					  _image.GetWidth(),
					  _image.GetHeight(),
					  0,
					  GL_RGB,
					  GL_UNSIGNED_BYTE,
					  _image.GetBuffer());
	} else if(alpha_buffer != 0) {
		Canvas _image(*alpha_buffer, true);
		_image.ConvertTo32BitWithAlpha(*alpha_buffer);
		unsigned new_width  = GetClosestPowerOf2(_image.GetWidth(), true);
		unsigned new_height = GetClosestPowerOf2(_image.GetHeight(), true);
		if (new_width != _image.GetWidth() || new_height != _image.GetHeight()) {
			_image.Resize(new_width, new_height, Canvas::kResizeFast);
		}

		texture = new Texture();

		texture->width_ = image->GetWidth();
		texture->height_ = image->GetHeight();

		texture_table_.Insert(image_id, texture);

		glBindTexture (GL_TEXTURE_2D, image_id);

		glTexImage2D (GL_TEXTURE_2D,
					  0,
					  GL_RGBA,
					  _image.GetWidth(),
					  _image.GetHeight(),
					  0,
					  GL_RGBA,
					  GL_UNSIGNED_BYTE,
					  _image.GetBuffer());
	} else {
		RemoveImage(image_id);
	}

	OGL_ASSERT();
}

void OpenGLPainter::RemoveImage(ImageID image_id) {
	OGL_ASSERT();

	TextureTable::Iterator iter = texture_table_.Find(image_id);

	if (iter == texture_table_.End()) {
		return;
	}

	Texture* texture = *iter;

	texture_table_.Remove(iter);

	delete texture;

	GLuint texture_name = (GLuint)image_id;
	glDeleteTextures(1, &texture_name);

	texture_id_manager_.RecycleId(image_id);

	OGL_ASSERT();
}

void OpenGLPainter::DoDrawImage(ImageID image_id, int x, int y) {
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

void OpenGLPainter::DoDrawAlphaImage(ImageID image_id, int x, int y) {
	OGL_ASSERT();

	ToScreenCoords(x, y);

	TextureTable::Iterator iter = texture_table_.Find(image_id);

	if (iter == texture_table_.End()) {
		return;
	}

	Texture* texture = *iter;

	GLfloat _left   = (GLfloat)x;
	GLfloat _right  = (GLfloat)(x + texture->width_);
	GLfloat _top    = (GLfloat)y;
	GLfloat _bottom = (GLfloat)(y + texture->height_);

	::glEnable(GL_TEXTURE_2D);

	glBindTexture (GL_TEXTURE_2D, image_id);
	glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
#ifndef LEPRA_GL_ES
	glPixelStorei (GL_UNPACK_ROW_LENGTH, 0);
	glPixelStorei (GL_UNPACK_SKIP_ROWS, 0);
	glPixelStorei (GL_UNPACK_SKIP_PIXELS, 0);
#endif // !GLES
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	Color& _color = GetColorInternal(0);
	if (GetRenderMode() == kRmAlphatest) {
		glColor4ub(_color.red_, _color.green_, _color.blue_, 255);
	} else {
		glColor4ub(_color.red_, _color.green_, _color.blue_, GetAlphaValue());
	}

	GLfloat u[] = {0,0, 1,0, 1,1, 0,1};
	GLfloat v[] = {_left,_top, _right,_top, _right,_bottom, _left,_bottom};
	::glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	::glTexCoordPointer(2, GL_FLOAT, 0, u);
	::glVertexPointer(2, GL_FLOAT, 0, v);
	::glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	::glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	::glDisable(GL_TEXTURE_2D);

	OGL_ASSERT();
}

void OpenGLPainter::DoDrawImage(ImageID image_id, const PixelRect& rect) {
	OGL_ASSERT();

	TextureTable::Iterator iter = texture_table_.Find(image_id);

	if (iter == texture_table_.End()) {
		return;
	}

	::glEnable(GL_TEXTURE_2D);

	glBindTexture (GL_TEXTURE_2D, image_id);
	glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
#ifndef LEPRA_GL_ES
	glPixelStorei (GL_UNPACK_ROW_LENGTH, 0);
	glPixelStorei (GL_UNPACK_SKIP_ROWS, 0);
	glPixelStorei (GL_UNPACK_SKIP_PIXELS, 0);
#endif // !GLES
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	if (GetRenderMode() == kRmAlphatest) {
		glColor4ub(255, 255, 255, 255);
	} else {
		glColor4ub(255, 255, 255, GetAlphaValue());
	}

	GLfloat u[] = {0, 0, 1, 0, 1, 1, 0, 1};
	GLfloat v[] = {(float)rect.left_, (float)rect.top_, (float)rect.right_, (float)rect.top_, (float)rect.right_, (float)rect.bottom_, (float)rect.left_, (float)rect.bottom_};
	::glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	::glTexCoordPointer(2, GL_FLOAT, 0, u);
	::glVertexPointer(2, GL_FLOAT, 0, v);
	::glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	::glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	::glDisable(GL_TEXTURE_2D);

	OGL_ASSERT();
}

void OpenGLPainter::DoDrawImage(ImageID image_id, int x, int y, const PixelRect& subpatch_rect) {
	ToScreenCoords(x, y);
	PixelRect _rect(x, y, x + subpatch_rect.GetWidth(), y + subpatch_rect.GetHeight());
	ToUserCoords(_rect.left_, _rect.top_);
	ToUserCoords(_rect.right_, _rect.bottom_);
	DrawImage(image_id, _rect, subpatch_rect);
}

void OpenGLPainter::DoDrawImage(ImageID image_id, const PixelRect& rect, const PixelRect& subpatch_rect) {
	OGL_ASSERT();

	TextureTable::Iterator iter = texture_table_.Find(image_id);

	if (iter == texture_table_.End()) {
		return;
	}

	Texture* texture = *iter;

	GLfloat _left   = (GLfloat)rect.left_;
	GLfloat _right  = (GLfloat)rect.right_;
	GLfloat _top    = (GLfloat)rect.top_;
	GLfloat _bottom = (GLfloat)rect.bottom_;

	ToScreenCoords(_left, _top);
	ToScreenCoords(_right, _bottom);

	::glEnable(GL_TEXTURE_2D);

	glBindTexture (GL_TEXTURE_2D, image_id);
	glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
#ifndef LEPRA_GL_ES
	glPixelStorei (GL_UNPACK_ROW_LENGTH, 0);
	glPixelStorei (GL_UNPACK_SKIP_ROWS, 0);
	glPixelStorei (GL_UNPACK_SKIP_PIXELS, 0);
#endif // !GLES
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	if (GetRenderMode() == kRmAlphatest) {
		glColor4ub(255, 255, 255, 255);
	} else {
		glColor4ub(255, 255, 255, GetAlphaValue());
	}

	GLfloat one_over_width  = 1.0f / (GLfloat)texture->width_;
	GLfloat one_over_height = 1.0f / (GLfloat)texture->height_;
	GLfloat u_left   = ((GLfloat)subpatch_rect.left_)   * one_over_width;
	GLfloat u_right  = ((GLfloat)subpatch_rect.right_)  * one_over_width;
	GLfloat v_top    = ((GLfloat)subpatch_rect.top_)    * one_over_height;
	GLfloat v_bottom = ((GLfloat)subpatch_rect.bottom_) * one_over_height;

	GLfloat u[] = {u_left,v_top, u_right,v_top, u_right,v_bottom, u_left,v_bottom};
	GLfloat v[] = {_left,_top, _right,_top, _right,_bottom, _left,_bottom};
	::glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	::glTexCoordPointer(2, GL_FLOAT, 0, u);
	::glVertexPointer(2, GL_FLOAT, 0, v);
	::glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	::glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	::glDisable(GL_TEXTURE_2D);

	OGL_ASSERT();
}

void OpenGLPainter::GetImageSize(ImageID image_id, int& width, int& height) const {
	width = 0;
	height = 0;

	TextureTable::ConstIterator iter = texture_table_.Find((int)image_id);
	if(iter != texture_table_.End()) {
		Texture* texture = *iter;
		width = texture->width_;
		height = texture->height_;
	}
}

void OpenGLPainter::PrintText(const wstr& s, int x, int y) {
	if (s.empty()) {
		return;
	}

	// Algo goes something like this. It's a texture-mapping font rendering algo.
	//  1. Loop over each char in the string
	//     - cache each char as necessary in a texture (which doubles in size when required)
	//     - append indices, vertices and texels for each char in an array.
	//  2. After loop done: render appended geometries.

	ToScreenCoords(x, y);

	int current_x = x;
	int current_y = y;

	//::glPushAttrib(GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT);

	if (uilepra::OpenGLExtensions::IsBufferObjectsSupported() == true) {
		uilepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, 0);
		uilepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	::glDisableClientState(GL_COLOR_ARRAY);
#ifndef LEPRA_GL_ES
	::glDisableClientState(GL_INDEX_ARRAY);
#endif // !GLES
	::glDisableClientState(GL_NORMAL_ARRAY);
	::glEnableClientState(GL_VERTEX_ARRAY);
	::glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	PushAttrib(kAttrRendermode);
	if (smooth_font_) {
		SetRenderMode(kRmAlphablend);
	} else {
		SetRenderMode(kRmAlphatest);
	}
	::glEnable(GL_TEXTURE_2D);
	//::glMatrixMode(GL_TEXTURE);
	//::glLoadIdentity();
	//::glMatrixMode(GL_MODELVIEW);

	const int _font_height = GetFontManager()->GetFontHeight();
	const int line_height = GetFontManager()->GetLineHeight();
	const int space_size = GetFontManager()->GetCharWidth(' ');
	const int tab_size = (GetTabSize() > 0)? GetTabSize() : space_size*4;

	const Color _color = GetColorInternal(0);
	deb_assert(_color != BLACK);	// Does not show.
	::glColor4f(_color.GetRf(), _color.GetGf(), _color.GetBf(), _color.GetAf());
	const uint32 _font_hash = (GetFontManager()->GetActiveFontId() << 16) + _font_height;
	FontTexture* font_texture = SelectGlyphs(_font_hash, _font_height, s);
	const int font_texture_height = font_texture->GetHeight();

	const size_t string_length = s.length();
	int glyph_index = 0;
	const size_t STACK_GLYPH_CAPACITY = 256;
#ifdef LEPRA_GL_ES
#define kIndicesPerGlyph 6
#define VERTEX_TYPE       GLshort
#define VERTEX_TYPE_GL_ID GL_SHORT
	static const vtx_idx_t template_glyph_indices[] =
	{
		0, 1, 2, 2, 1, 3,
	};
#else // !GLES
#define kIndicesPerGlyph 4
#define VERTEX_TYPE       GLint
#define VERTEX_TYPE_GL_ID GL_INT
	static const vtx_idx_t template_glyph_indices[] =
	{
		0, 1, 3, 2,
	};
#endif // GLES/!GLES
	vtx_idx_t array_glyph_indices[kIndicesPerGlyph*STACK_GLYPH_CAPACITY];
	vtx_idx_t* glyph_indices = array_glyph_indices;
	VERTEX_TYPE array_vertices[2*4*STACK_GLYPH_CAPACITY];
	GLfloat array_uv[2*4*STACK_GLYPH_CAPACITY];
	VERTEX_TYPE* vertices = array_vertices;
	GLfloat* uv = array_uv;
	bool alloc_primitives = (string_length > STACK_GLYPH_CAPACITY);
	if (alloc_primitives) {
		glyph_indices = new vtx_idx_t[kIndicesPerGlyph*string_length];
		vertices = new VERTEX_TYPE[2*4*string_length];
		uv = new GLfloat[2*4*string_length];
	}

	for (size_t i = 0; i < string_length; i++) {
		const wchar_t __c = s[i];
		if (__c == '\n') {
			current_y += line_height;
			current_x = x;
		} else if (__c == ' ') {
			current_x += space_size;
		} else if(__c == '\t') {
			current_x = (current_x/tab_size+1) * tab_size;
		} else if(__c != '\r' &&
			__c != '\b') {
			int texture_x = 0;
			int char_width = 5;
			int placement_offset = 0;
			if (!font_texture->GetGlyphX(__c, texture_x, char_width, placement_offset)) {
				deb_assert(false);
				continue;
			}
			current_x += placement_offset;
			const float texture_width = (float)font_texture->GetWidth();
			const VERTEX_TYPE template_vertices[2*4] =
			{
				current_x,		current_y,
				current_x + char_width,	current_y,
				current_x,		current_y + font_texture_height,
				current_x + char_width,	current_y + font_texture_height,
			};
			const GLfloat template_uv[2*4] =
			{
				texture_x/texture_width,		1,
				(texture_x + char_width)/texture_width,	1,
				texture_x/texture_width,		0,
				(texture_x + char_width)/texture_width,	0,
			};
			const int index_base = glyph_index*kIndicesPerGlyph;
			const int vertex_base = glyph_index*4;
			for (int j = 0; j < kIndicesPerGlyph; ++j) {
				glyph_indices[index_base+j] = template_glyph_indices[j]+vertex_base;
			}
			for (int j = 0; j < 4; ++j) {
				vertices[(vertex_base+j)*2+0] = template_vertices[j*2+0];
				vertices[(vertex_base+j)*2+1] = template_vertices[j*2+1];
				uv[(vertex_base+j)*2+0] = template_uv[j*2+0];
				uv[(vertex_base+j)*2+1] = template_uv[j*2+1];
			}
			++glyph_index;

			current_x += char_width;
		}
	}

	::glVertexPointer(2, VERTEX_TYPE_GL_ID, 0, vertices);
	::glTexCoordPointer(2, GL_FLOAT, 0, uv);
#ifdef LEPRA_GL_ES
	::glDrawElements(GL_TRIANGLES, kIndicesPerGlyph*glyph_index, LEPRA_GL_INDEX_TYPE, glyph_indices);
#else // !GLES
	::glDrawElements(GL_QUADS, kIndicesPerGlyph*glyph_index, LEPRA_GL_INDEX_TYPE, glyph_indices);
#endif // GLES/!GLES

	if (alloc_primitives) {
		delete[] (glyph_indices);
		delete[] (vertices);
		delete[] (uv);
	}

	PopAttrib();

	::glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	::glDisable(GL_TEXTURE_2D);
}

void OpenGLPainter::ReadPixels(Canvas& dest_canvas, const PixelRect& rect) {
	OGL_ASSERT();

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

	const unsigned _width  = _rect.GetWidth();
	const unsigned _height = _rect.GetHeight();

	if (dest_canvas.GetBitDepth() != GetCanvas()->GetBitDepth() ||
	   dest_canvas.GetWidth() != _width ||
	   dest_canvas.GetHeight() != _height) {
		dest_canvas.Reset(_width, _height, Canvas::kBitdepth32Bit);
		dest_canvas.CreateBuffer();
	}

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
#ifndef LEPRA_GL_ES
	glReadBuffer(GL_FRONT_LEFT);
	glPixelStorei(GL_PACK_ROW_LENGTH, 0);
	glPixelStorei(GL_PACK_SKIP_ROWS, 0);
	glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
#endif // !GLES

	switch(GetCanvas()->GetBitDepth()) {
		case Canvas::kBitdepth15Bit:
		case Canvas::kBitdepth16Bit:
		case Canvas::kBitdepth24Bit:
		case Canvas::kBitdepth32Bit: {
			glReadPixels(_rect.left_,
						 GetCanvas()->GetHeight() - _rect.bottom_,
						 _rect.GetWidth(),
						 _rect.GetHeight(),
						 GL_RGBA,
						 GL_UNSIGNED_BYTE,
						 dest_canvas.GetBuffer());
		break;
		}
	default:
		break;
	}

	dest_canvas.SwapRGBOrder();
	dest_canvas.FlipVertical();

	OGL_ASSERT();
}

Painter::RGBOrder OpenGLPainter::GetRGBOrder() const {
	return kRgb;
}

void OpenGLPainter::SetFontSmoothness(bool smooth) {
	if (smooth_font_ != smooth) {
		smooth_font_ = smooth;
		ClearFontBuffers();
	}
}



void OpenGLPainter::AdjustVertexFormat(unsigned& vertex_format) {
#ifdef LEPRA_GL_ES
	vertex_format |= Geometry2D::kVtxIndex16;
#else // !GLES
	// Default behaviour. Do nothing.
	vertex_format;
#endif
}

void OpenGLPainter::DoRenderDisplayList(std::vector<DisplayEntity*>* display_list) {
	OGL_ASSERT();

	PushAttrib(kAttrAll);

	::glDisableClientState(GL_NORMAL_ARRAY);
#ifndef LEPRA_GL_ES
	::glDisableClientState(GL_INDEX_ARRAY);
#endif // !GLES
	uilepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, 0);
	uilepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	std::vector<DisplayEntity*>::iterator it;
	for(it = display_list->begin(); it != display_list->end(); ++it) {
		DisplayEntity* generated_geometry = *it;
		Painter::SetClippingRect(generated_geometry->GetClippingRect());
		SetAlphaValue(generated_geometry->GetAlpha());
		SetRenderMode(generated_geometry->GetRenderMode());

		// Enabled in ResetClippingRect().
		glVertexPointer(2, GL_FLOAT, 0, generated_geometry->GetGeometry().GetVertexData());

		if (generated_geometry->GetGeometry().GetColorData() != 0) {
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(3, GL_FLOAT, 0, generated_geometry->GetGeometry().GetColorData());
		} else {
			glDisableClientState(GL_COLOR_ARRAY);
		}

		if (generated_geometry->GetImageID() != kInvalidImageid) {
			deb_assert(generated_geometry->GetGeometry().GetUVData() != 0);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2, GL_FLOAT, 0, generated_geometry->GetGeometry().GetUVData());

			::glEnable(GL_TEXTURE_2D);

			glBindTexture (GL_TEXTURE_2D, generated_geometry->GetImageID());
			glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
#ifndef LEPRA_GL_ES
			glPixelStorei (GL_UNPACK_ROW_LENGTH, 0);
			glPixelStorei (GL_UNPACK_SKIP_ROWS, 0);
			glPixelStorei (GL_UNPACK_SKIP_PIXELS, 0);
#endif // !GLES
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

			if (GetRenderMode() == kRmAlphatest) {
				glColor4ub(255, 255, 255, 255);
			} else {
				glColor4ub(255, 255, 255, GetAlphaValue());
			}
		} else {
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}

		const int triangle_entry_count = generated_geometry->GetGeometry().GetTriangleCount() * 3;
#ifdef LEPRA_GL_ES
		const uint16* triangle_data16 = generated_geometry->GetGeometry().GetTriangleData16();
		::glDrawElements(GL_TRIANGLES, triangle_entry_count, GL_UNSIGNED_SHORT, triangle_data16);
#else // !GLES
		const uint32* triangle_data32 = generated_geometry->GetGeometry().GetTriangleData32();
		::glDrawElements(GL_TRIANGLES, triangle_entry_count, GL_UNSIGNED_INT, triangle_data32);
#endif

		::glDisable(GL_TEXTURE_2D);
	}

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	PopAttrib();

	OGL_ASSERT();
}




void OpenGLPainter::ClearFontBuffers() {
	//OGL_ASSERT();

	FontTextureTable::iterator x = font_texture_table_.begin();
	for (; x != font_texture_table_.end(); ++x) {
		GLuint _font_hash = x->second->GetFontHash();
		::glDeleteTextures(1, &_font_hash);
		delete x->second;
	}
	font_texture_table_.clear();

	//OGL_ASSERT();
}

FontTexture* OpenGLPainter::SelectGlyphs(uint32 font_hash, int font_height, const wstr& s) {
	FontTexture* font_texture = HashUtil::FindMapObject(font_texture_table_, font_hash);
	if (!font_texture) {
		font_texture = new FontTexture(font_hash, font_height);
		font_texture_table_.insert(FontTextureTable::value_type(font_hash, font_texture));
	}
	const size_t __length = s.length();
	for (size_t x = 0; x < __length; ++x) {
		font_texture->StoreGlyph(s[x], GetFontManager());
	}
	OGL_ASSERT();
	::glBindTexture(GL_TEXTURE_2D, font_hash);
	if (font_texture->IsUpdated()) {
		font_texture->ResetIsUpdated();
		OGL_ASSERT();
		::glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_RGBA,
			font_texture->GetWidth(),
			font_texture->GetHeight(),
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			font_texture->GetBuffer());
		OGL_ASSERT();
		::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	return (font_texture);
}



}
