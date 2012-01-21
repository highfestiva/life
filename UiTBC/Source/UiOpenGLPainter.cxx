
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/UiOpenGLPainter.h"
#include <math.h>
#include "../../Lepra/Include/HashUtil.h"
#include "../Include/UiFontManager.h"
#include "../Include/UiFontTexture.h"



namespace UiTbc
{



#ifdef LEPRA_DEBUG
#define OGL_ASSERT()	{ GLenum lGlError = glGetError(); assert(lGlError == GL_NO_ERROR); }
#else // !Debug
#define OGL_ASSERT()
#endif // Debug / !Debug



OpenGLPainter::OpenGLPainter() :
	mTextureIDManager(3, 10000, 0),
	mSmoothFont(true)
{
}

OpenGLPainter::~OpenGLPainter()
{
	//OGL_ASSERT();

	ClearFontBuffers();

	TextureTable::Iterator lIter = mTextureTable.First();
	while (lIter != mTextureTable.End())
	{
		Texture* lTexture = *lIter;

		GLuint lTextureName = (GLuint)lIter.GetKey();
		glDeleteTextures(1, &lTextureName);

		mTextureTable.Remove(lIter++);
		delete lTexture;
	}

	//OGL_ASSERT();
}

void OpenGLPainter::SetDestCanvas(Canvas* pCanvas)
{
	Painter::SetDestCanvas(pCanvas);
	ResetClippingRect();
}

void OpenGLPainter::SetAlphaValue(uint8 pAlpha)
{
	OGL_ASSERT();

	if (GetAlphaValue() != pAlpha)
	{
		Painter::SetAlphaValue(pAlpha);
		float lAlpha = (float)GetAlphaValue() / 255.0f;
		::glAlphaFunc(GL_GEQUAL, (GLclampf)lAlpha);
		OGL_ASSERT();
	}
}

void OpenGLPainter::SetRenderMode(RenderMode pRM)
{
	if (pRM != GetRenderMode())
	{
		Painter::SetRenderMode(pRM);
		DoSetRenderMode();
	}
}

void OpenGLPainter::Clear(const Color& pColor)
{
	::glClearColor(pColor.GetRf(), pColor.GetGf(), pColor.GetBf(), 1.0f);
	::glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLPainter::PrePaint(bool pClearDepthBuffer)
{
	if (pClearDepthBuffer)
	{
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
	if (UiLepra::OpenGLExtensions::IsBufferObjectsSupported() == true)
	{
		UiLepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, 0);
		UiLepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void OpenGLPainter::SetClippingRect(int pLeft, int pTop, int pRight, int pBottom)
{
	if (pBottom <= pTop || pRight <= pLeft)
	{
		return;
	}

	OGL_ASSERT();

	Painter::SetClippingRect(pLeft, pTop, pRight, pBottom);
	ToScreenCoords(pLeft, pTop);
	ToScreenCoords(pRight, pBottom);
#ifdef LEPRA_IOS
	::glScissor(GetCanvas()->GetHeight() - pBottom, GetCanvas()->GetWidth() - pRight,
		pBottom - pTop, pRight - pLeft);
#else // !iOS
	::glScissor(pLeft, GetCanvas()->GetHeight() - pBottom, pRight - pLeft, pBottom - pTop);
#endif // iOS/!iOS

	OGL_ASSERT();
}

static void my_glOrtho(GLfloat pLeft, GLfloat pRight, GLfloat pBottom, GLfloat pTop,
	GLfloat pZNear, GLfloat pZFar)
{
	const float tx = - (pRight + pLeft) / (pRight - pLeft);
	const float ty = - (pTop + pBottom) / (pTop - pBottom);
	const float tz = - (pZFar + pZNear) / (pZFar - pZNear);
	const float m[16] =
	{
		2 / (pRight - pLeft), 0, 0, 0,
		0, 2/(pTop - pBottom), 0, 0,
		0, 0, -2/(pZFar - pZNear), 0,
		tx, ty, tz, 1
	};
	::glMultMatrixf(m);
}

void OpenGLPainter::ResetClippingRect()
{
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
	if (GetCanvas()->GetOutputRotation() == 90)
	{
		glTranslatef(0, -(float)GetCanvas()->GetHeight(), 0);	// TRICKY: float cast necessary, otherwise nothing is shown on screen! Bug?!?
	}
	else if (GetCanvas()->GetOutputRotation() == -90)
	{
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
	
	int lTop  = 0;
	int lLeft = 0;
	int lBottom = (int)GetCanvas()->GetHeight();
	int lRight  = (int)GetCanvas()->GetWidth();
	ToUserCoords(lLeft, lTop);
	ToUserCoords(lRight, lBottom);
	SetClippingRect(lLeft, lTop, lRight, lBottom);

	glEnableClientState(GL_VERTEX_ARRAY);
	//glDisableClientState(GL_VERTEX_ARRAY);

	OGL_ASSERT();
}

void OpenGLPainter::SetColor(const Color& pColor, unsigned pColorIndex)
{
	Painter::SetColor(pColor, pColorIndex);
	mRCol[pColorIndex].Set((float)pColor.mRed / 255.0f,
	                       (float)pColor.mGreen / 255.0f,
			       (float)pColor.mBlue / 255.0f);
}

void OpenGLPainter::DoSetRenderMode() const
{
	OGL_ASSERT();

	switch(GetRenderMode())
	{
		case Painter::RM_ALPHATEST:
		{
			glDisable(GL_COLOR_LOGIC_OP);
			glDisable(GL_BLEND);
			glEnable(GL_ALPHA_TEST);
			float lAlpha = (float)GetAlphaValue() / 255.0f;
			glAlphaFunc(GL_GEQUAL, (GLclampf)lAlpha);
			break;
		}
		case Painter::RM_ALPHABLEND:
		{
			glDisable(GL_ALPHA_TEST);
			glDisable(GL_COLOR_LOGIC_OP);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			break;
		}
		case Painter::RM_XOR:
		{
			glDisable(GL_ALPHA_TEST);
			glDisable(GL_BLEND);
			glEnable(GL_COLOR_LOGIC_OP);
			glLogicOp(GL_XOR);
			break;
		}
		case Painter::RM_ADD:
		{
			glDisable(GL_ALPHA_TEST);
			glDisable(GL_COLOR_LOGIC_OP);
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE);
			break;
		}
		case Painter::RM_NORMAL:
		default:
		{
			glDisable(GL_COLOR_LOGIC_OP);
			glDisable(GL_ALPHA_TEST);
			glDisable(GL_BLEND);
			break;
		}
	}

	OGL_ASSERT();
}

void OpenGLPainter::DoDrawPixel(int x, int y)
{
	OGL_ASSERT();

	ToScreenCoords(x, y);

	Color& lColor = GetColorInternal(0);
	glColor4ub(lColor.mRed, lColor.mGreen, lColor.mBlue, GetAlphaValue());
	glPointSize(1);

	GLshort v[] = {(GLshort)x, (GLshort)y};
	::glVertexPointer(2, GL_SHORT, 0, v);
	::glDrawArrays(GL_POINTS, 0, 1);
	//::glDisableClientState(GL_VERTEX_ARRAY);

	OGL_ASSERT();
}

void OpenGLPainter::DoDrawLine(int pX1, int pY1, int pX2, int pY2)
{
	OGL_ASSERT();

	ToScreenCoords(pX1, pY1);
	ToScreenCoords(pX2, pY2);

	Color& lColor = GetColorInternal(0);
	glColor4ub(lColor.mRed, lColor.mGreen, lColor.mBlue, GetAlphaValue());

	//glLineWidth(1);
	GLfloat v[] = {(GLfloat)pX1, (GLfloat)pY1, (GLfloat)pX2, (GLfloat)pY2};
	::glVertexPointer(2, GL_FLOAT, 0, v);
	::glDrawArrays(GL_LINES, 0, 2);

	OGL_ASSERT();
}

void OpenGLPainter::DoFillTriangle(float pX1, float pY1,
				 float pX2, float pY2,
				 float pX3, float pY3)
{
	OGL_ASSERT();

	ToScreenCoords(pX1, pY1);
	ToScreenCoords(pX2, pY2);
	ToScreenCoords(pX3, pY3);

	Color& lColor = GetColorInternal(0);
	glColor4ub(lColor.mRed, lColor.mGreen, lColor.mBlue, GetAlphaValue());

	GLfloat v[] = {pX1, pY1, pX2, pY2, pX3, pY3};
	::glVertexPointer(2, GL_FLOAT, 0, v);
	::glDrawArrays(GL_TRIANGLES, 0, 3);

	OGL_ASSERT();
}

void OpenGLPainter::DoFillShadedTriangle(float pX1, float pY1,
				       float pX2, float pY2,
				       float pX3, float pY3)
{
	OGL_ASSERT();

	ToScreenCoords(pX1, pY1);
	ToScreenCoords(pX2, pY2);
	ToScreenCoords(pX3, pY3);

	Color* lColor = &GetColorInternal(0);
	GLubyte c[] = {lColor[0].mRed, lColor[0].mGreen, lColor[0].mBlue, GetAlphaValue(),
		      lColor[1].mRed, lColor[1].mGreen, lColor[1].mBlue, GetAlphaValue(),
		      lColor[2].mRed, lColor[2].mGreen, lColor[2].mBlue, GetAlphaValue()};
	GLfloat v[] = {pX1, pY1, pX2, pY2, pX3, pY3};
	::glEnableClientState(GL_COLOR_ARRAY);
	::glColorPointer(4, GL_UNSIGNED_BYTE, 0, c);
	::glVertexPointer(2, GL_FLOAT, 0, v);
	::glDrawArrays(GL_TRIANGLES, 0, 3);
	::glDisableClientState(GL_COLOR_ARRAY);

	OGL_ASSERT();
}

void OpenGLPainter::DoFillTriangle(float pX1, float pY1, float pU1, float pV1,
				 float pX2, float pY2, float pU2, float pV2,
				 float pX3, float pY3, float pU3, float pV3,
				 ImageID pImageID)
{
	OGL_ASSERT();

	ToScreenCoords(pX1, pY1);
	ToScreenCoords(pX2, pY2);
	ToScreenCoords(pX3, pY3);

	TextureTable::Iterator lIter = mTextureTable.Find(pImageID);

	if (lIter == mTextureTable.End())
	{
		return;
	}

	//glPushAttrib(GL_TEXTURE_BIT);
	glEnable(GL_TEXTURE_2D);

	glBindTexture (GL_TEXTURE_2D, (unsigned)pImageID);
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

	if (GetRenderMode() == RM_ALPHATEST)
	{
		glColor4ub(255, 255, 255, 255);
	}
	else
	{
		glColor4ub(255, 255, 255, GetAlphaValue());
	}

	GLfloat u[] = {pU1, pV1, pU2, pV2, pU3, pV3};
	GLfloat v[] = {pX1, pY1, pX2, pY2, pX3, pY3};
	::glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	::glTexCoordPointer(2, GL_FLOAT, 0, u);
	::glVertexPointer(2, GL_FLOAT, 0, v);
	::glDrawArrays(GL_TRIANGLES, 0, 3);
	::glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_TEXTURE_2D);

	OGL_ASSERT();
}

void OpenGLPainter::DoDrawRect(int pLeft, int pTop, int pRight, int pBottom, int pWidth)
{
	OGL_ASSERT();

	ToScreenCoords(pLeft, pTop);
	ToScreenCoords(pRight, pBottom);

	GLfloat lLeft   = (GLfloat)pLeft;
	GLfloat lRight  = (GLfloat)pRight;
	GLfloat lTop    = (GLfloat)pTop;
	GLfloat lBottom = (GLfloat)pBottom;

	GLfloat lVertex[8 * 2];
	lVertex[0] = lLeft; // Outer top left.
	lVertex[1] = lTop;
	lVertex[2] = lRight; // Outer top right.
	lVertex[3] = lTop;
	lVertex[4] = lRight; // Outer bottom right.
	lVertex[5] = lBottom;
	lVertex[6] = lLeft; // Outer bottom left.
	lVertex[7] = lBottom;
	lLeft += pWidth;
	lTop += pWidth;
	lRight -= pWidth;
	lBottom -= pWidth;
	lVertex[8] = lLeft; // Inner top left.
	lVertex[9] = lTop;
	lVertex[10] = lRight; // Inner top right.
	lVertex[11] = lTop;
	lVertex[12] = lRight; // Inner bottom right.
	lVertex[13] = lBottom;
	lVertex[14] = lLeft; // Inner bottom left.
	lVertex[15] = lBottom;

	GLfloat lColor[8 * 3];
	lColor[0] = mRCol[0].x;
	lColor[1] = mRCol[0].y;
	lColor[2] = mRCol[0].z;
	lColor[3] = mRCol[0].x;
	lColor[4] = mRCol[0].y;
	lColor[5] = mRCol[0].z;
	lColor[6] = mRCol[0].x;
	lColor[7] = mRCol[0].y;
	lColor[8] = mRCol[0].z;
	lColor[9] = mRCol[0].x;
	lColor[10] = mRCol[0].y;
	lColor[11] = mRCol[0].z;
	lColor[12] = mRCol[1].x;
	lColor[13] = mRCol[1].y;
	lColor[14] = mRCol[1].z;
	lColor[15] = mRCol[1].x;
	lColor[16] = mRCol[1].y;
	lColor[17] = mRCol[1].z;
	lColor[18] = mRCol[1].x;
	lColor[19] = mRCol[1].y;
	lColor[20] = mRCol[1].z;
	lColor[21] = mRCol[1].x;
	lColor[22] = mRCol[1].y;
	lColor[23] = mRCol[1].z;

	const static vtx_idx_t lIndices[] = {0,4,7, 0,7,3, 0,1,5, 0,5,4, 1,2,6, 1,6,5, 7,6,2, 7,2,3};

	glColor4ub(255, 255, 255, GetAlphaValue());

	// Enabled in ResetClippingRect().
	glVertexPointer(2, GL_FLOAT, 0, lVertex);
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(3, GL_FLOAT, 0, lColor);
	glDrawElements(GL_TRIANGLES,
			24,
			LEPRA_GL_INDEX_TYPE,
			lIndices);
	glDisableClientState(GL_COLOR_ARRAY);

	OGL_ASSERT();
}

void OpenGLPainter::DoFillRect(int pLeft, int pTop, int pRight, int pBottom)
{
	OGL_ASSERT();

	ToScreenCoords(pLeft, pTop);
	ToScreenCoords(pRight, pBottom);

	GLfloat lLeft   = (GLfloat)pLeft;
	GLfloat lRight  = (GLfloat)pRight;
	GLfloat lTop    = (GLfloat)pTop;
	GLfloat lBottom = (GLfloat)pBottom;

	Color& lColor = GetColorInternal(0);
	glColor4ub(lColor.mRed, lColor.mGreen, lColor.mBlue, GetAlphaValue());

	GLfloat v[] = {lLeft, lTop, lRight, lTop, lRight, lBottom, lLeft, lBottom};
	::glVertexPointer(2, GL_FLOAT, 0, v);
	::glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	OGL_ASSERT();
}

void OpenGLPainter::DoDraw3DRect(int pLeft, int pTop, int pRight, int pBottom, int pWidth, bool pSunken)
{
	OGL_ASSERT();

	ToScreenCoords(pLeft, pTop);
	ToScreenCoords(pRight, pBottom);

	GLfloat lLeft   = (GLfloat)pLeft;
	GLfloat lRight  = (GLfloat)pRight;
	GLfloat lTop    = (GLfloat)pTop;
	GLfloat lBottom = (GLfloat)pBottom;

	GLfloat lVertex[12 * 2];
	lVertex[0] = lLeft; // Outer top left.
	lVertex[1] = lTop;
	lVertex[2] = lRight; // Outer top right #1.
	lVertex[3] = lTop;
	lVertex[4] = lRight; // Outer top right #2.
	lVertex[5] = lTop;
	lVertex[6] = lRight; // Outer bottom right.
	lVertex[7] = lBottom;
	lVertex[8] = lLeft; // Outer bottom left #1.
	lVertex[9] = lBottom;
	lVertex[10] = lLeft; // Outer bottom left #2.
	lVertex[11] = lBottom;
	lLeft += pWidth;
	lTop += pWidth;
	lRight -= pWidth;
	lBottom -= pWidth;
	lVertex[12] = lLeft; // Inner top left.
	lVertex[13] = lTop;
	lVertex[14] = lRight; // Inner top right #1.
	lVertex[15] = lTop;
	lVertex[16] = lRight; // Inner top right #2.
	lVertex[17] = lTop;
	lVertex[18] = lRight; // Inner bottom right.
	lVertex[19] = lBottom;
	lVertex[20] = lLeft; // Inner bottom left #1.
	lVertex[21] = lBottom;
	lVertex[22] = lLeft; // Inner bottom left #2.
	lVertex[23] = lBottom;

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

	GLfloat lColor[12 * 3];
	lColor[0] = mRCol[lZero].x;
	lColor[1] = mRCol[lZero].y;
	lColor[2] = mRCol[lZero].z;
	lColor[3] = mRCol[lZero].x;
	lColor[4] = mRCol[lZero].y;
	lColor[5] = mRCol[lZero].z;
	lColor[6] = mRCol[lOne].x;
	lColor[7] = mRCol[lOne].y;
	lColor[8] = mRCol[lOne].z;
	lColor[9] = mRCol[lOne].x;
	lColor[10] = mRCol[lOne].y;
	lColor[11] = mRCol[lOne].z;
	lColor[12] = mRCol[lZero].x;
	lColor[13] = mRCol[lZero].y;
	lColor[14] = mRCol[lZero].z;
	lColor[15] = mRCol[lOne].x;
	lColor[16] = mRCol[lOne].y;
	lColor[17] = mRCol[lOne].z;

	lColor[18] = mRCol[lTwo].x;
	lColor[19] = mRCol[lTwo].y;
	lColor[20] = mRCol[lTwo].z;
	lColor[21] = mRCol[lTwo].x;
	lColor[22] = mRCol[lTwo].y;
	lColor[23] = mRCol[lTwo].z;
	lColor[24] = mRCol[lThree].x;
	lColor[25] = mRCol[lThree].y;
	lColor[26] = mRCol[lThree].z;
	lColor[27] = mRCol[lThree].x;
	lColor[28] = mRCol[lThree].y;
	lColor[29] = mRCol[lThree].z;
	lColor[30] = mRCol[lTwo].x;
	lColor[31] = mRCol[lTwo].y;
	lColor[32] = mRCol[lTwo].z;
	lColor[33] = mRCol[lThree].x;
	lColor[34] = mRCol[lThree].y;
	lColor[35] = mRCol[lThree].z;

	const static vtx_idx_t lsIndices[] = {0,1,7, 0,7,6, 0,6,10, 0,10,4, 8,2,3, 8,3,9, 11,9,3, 11,3,5};

	//::glDisableClientState(GL_NORMAL_ARRAY);
	//::glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	// Enabled in ResetClippingRect().
	glVertexPointer(2, GL_FLOAT, 0, lVertex);
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(3, GL_FLOAT, 0, lColor);
	glDrawElements(GL_TRIANGLES, 12, LEPRA_GL_INDEX_TYPE, lsIndices);
	glDisableClientState(GL_COLOR_ARRAY);

	OGL_ASSERT();
}

void OpenGLPainter::DoFillShadedRect(int pLeft, int pTop, int pRight, int pBottom)
{
	OGL_ASSERT();

	ToScreenCoords(pLeft, pTop);
	ToScreenCoords(pRight, pBottom);

	GLfloat lLeft   = (GLfloat)pLeft;
	GLfloat lRight  = (GLfloat)pRight;
	GLfloat lTop    = (GLfloat)pTop;
	GLfloat lBottom = (GLfloat)pBottom;

	// Calculate the color in the middle of the rect.
	GLfloat lTopR = (GLfloat)(mRCol[0].x + mRCol[1].x) * 0.5f;
	GLfloat lTopG = (GLfloat)(mRCol[0].y + mRCol[1].y) * 0.5f;
	GLfloat lTopB = (GLfloat)(mRCol[0].z + mRCol[1].z) * 0.5f;

	GLfloat lBotR = (GLfloat)(mRCol[3].x + mRCol[2].x) * 0.5f;
	GLfloat lBotG = (GLfloat)(mRCol[3].y + mRCol[2].y) * 0.5f;
	GLfloat lBotB = (GLfloat)(mRCol[3].z + mRCol[2].z) * 0.5f;

	Color* lColor = &GetColorInternal(0);
#define UBCOL(f) (GLubyte)((f)*128)
	GLubyte c[] = {UBCOL(lTopR + lBotR), UBCOL(lTopG + lBotG), UBCOL(lTopB + lBotB), GetAlphaValue(),
		       lColor[0].mRed, lColor[0].mGreen, lColor[0].mBlue, GetAlphaValue(),
		       lColor[1].mRed, lColor[1].mGreen, lColor[1].mBlue, GetAlphaValue(),
		       lColor[2].mRed, lColor[2].mGreen, lColor[2].mBlue, GetAlphaValue(),
		       lColor[3].mRed, lColor[3].mGreen, lColor[3].mBlue, GetAlphaValue(),
		       lColor[0].mRed, lColor[0].mGreen, lColor[0].mBlue, GetAlphaValue()};
	GLfloat v[] = {(lLeft + lRight) * 0.5f, (lTop + lBottom) * 0.5f, lLeft, lTop, lRight, lTop, lRight, lBottom, lLeft, lBottom, lLeft, lTop};
	::glEnableClientState(GL_COLOR_ARRAY);
	::glColorPointer(4, GL_UNSIGNED_BYTE, 0, c);
	::glVertexPointer(2, GL_FLOAT, 0, v);
	::glDrawArrays(GL_TRIANGLE_FAN, 0, 6);
	::glDisableClientState(GL_COLOR_ARRAY);

	OGL_ASSERT();
}

void OpenGLPainter::DrawFan(const std::vector<Vector2DF> pCoords, bool pFill)
{
	OGL_ASSERT();

	Color& lColor = GetColorInternal(0);
	::glColor4ub(lColor.mRed, lColor.mGreen, lColor.mBlue, GetAlphaValue());
	size_t c = pCoords.size();
	GLfloat* v = new GLfloat[c*2];
	std::vector<Vector2DF>::const_iterator i = pCoords.begin();
	for (int j = 0; i != pCoords.end(); ++i, ++j)
	{
		float x = i->x;
		float y = i->y;
		ToScreenCoords(x, y);
		v[j*2+0] = x;
		v[j*2+1] = y;
	}
	::glVertexPointer(2, GL_FLOAT, 0, v);
	::glDrawArrays(pFill? GL_TRIANGLE_FAN : GL_LINE_STRIP, 0, c);
	delete[] v;

	OGL_ASSERT();
}

void OpenGLPainter::DrawImageFan(ImageID pImageID, const std::vector<Vector2DF> pCoords, const std::vector<Vector2DF> pTexCoords)
{
	OGL_ASSERT();
	assert(pCoords.size() == pTexCoords.size());

	TextureTable::Iterator lIter = mTextureTable.Find(pImageID);

	if (lIter == mTextureTable.End())
	{
		return;
	}

	::glEnable(GL_TEXTURE_2D);

	glBindTexture (GL_TEXTURE_2D, pImageID);
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

	if (GetRenderMode() == RM_ALPHATEST)
	{
		glColor4ub(255, 255, 255, 255);
	}
	else
	{
		glColor4ub(255, 255, 255, GetAlphaValue());
	}

	::glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	::glTexCoordPointer(2, GL_FLOAT, 0, &pTexCoords[0]);
	::glVertexPointer(2, GL_FLOAT, 0, &pCoords[0]);
	::glDrawArrays(GL_TRIANGLE_FAN, 0, pCoords.size());
	::glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	::glDisable(GL_TEXTURE_2D);

	OGL_ASSERT();
}

Painter::ImageID OpenGLPainter::AddImage(const Canvas* pImage, const Canvas* pAlphaBuffer)
{
	OGL_ASSERT();

	int lID = 0;

	bool lAlpha = false;
	bool lColor = false;

	if (pImage != 0)
	{
		lColor = true;

		if (pImage->GetBitDepth() == Canvas::BITDEPTH_32_BIT)
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
		if (lColor == true && lAlpha == true)
		{
			Canvas lImage(*pImage, true);
			lImage.SwapRGBOrder();
			unsigned lNewWidth  = GetClosestPowerOf2(lImage.GetWidth(), true);
			unsigned lNewHeight = GetClosestPowerOf2(lImage.GetHeight(), true);

			if (lNewWidth != lImage.GetWidth() || lNewHeight != lImage.GetHeight())
			{
				lImage.Resize(lNewWidth, lNewHeight, Canvas::RESIZE_NICEST);
			}

			if (pAlphaBuffer != 0)
			{
				Canvas lAlphaBuffer(*pAlphaBuffer, true);

				if (lAlphaBuffer.GetWidth() != lImage.GetWidth() ||
				   lAlphaBuffer.GetHeight() != lImage.GetHeight())
				{
					lAlphaBuffer.Resize(lImage.GetWidth(), lImage.GetHeight(), Canvas::RESIZE_FAST);
				}
				if (lAlphaBuffer.GetBitDepth() != Canvas::BITDEPTH_8_BIT)
				{
					lAlphaBuffer.ConvertToGrayscale(true);
				}
	
				lImage.ConvertTo32BitWithAlpha(lAlphaBuffer);
			}

			Texture* lTexture = new Texture();

			lTexture->mWidth = pImage->GetWidth();
			lTexture->mHeight = pImage->GetHeight();

			mTextureTable.Insert(lID, lTexture);

			glBindTexture (GL_TEXTURE_2D, lID);
			OGL_ASSERT();

			glTexImage2D (GL_TEXTURE_2D,
						  0,
						  GL_RGBA,
						  lImage.GetWidth(),
						  lImage.GetHeight(),
						  0,
						  GL_RGBA,
						  GL_UNSIGNED_BYTE,
						  lImage.GetBuffer());
			OGL_ASSERT();
		}
		else if(lColor == true)
		{
			Canvas lImage(*pImage, true);
			lImage.SwapRGBOrder();
			lImage.ConvertBitDepth(Canvas::BITDEPTH_24_BIT);

			unsigned lNewWidth  = GetClosestPowerOf2(lImage.GetWidth(), true);
			unsigned lNewHeight = GetClosestPowerOf2(lImage.GetHeight(), true);
			if (lNewWidth != lImage.GetWidth() || lNewHeight != lImage.GetHeight())
			{
				lImage.Resize(lNewWidth, lNewHeight, Canvas::RESIZE_FAST);
			}

			Texture* lTexture = new Texture();

			lTexture->mWidth = pImage->GetWidth();
			lTexture->mHeight = pImage->GetHeight();

			mTextureTable.Insert(lID, lTexture);

			glBindTexture (GL_TEXTURE_2D, lID);
			OGL_ASSERT();

			glTexImage2D (GL_TEXTURE_2D,
						  0,
						  GL_RGB,
						  lImage.GetWidth(),
						  lImage.GetHeight(),
						  0,
						  GL_RGB,
						  GL_UNSIGNED_BYTE,
						  lImage.GetBuffer());
			OGL_ASSERT();
		}
		else if(pAlphaBuffer != 0)
		{
			Canvas lImage(*pAlphaBuffer, true);

			Color lPalette[256];
			for (int i = 0; i < 256; i++)
			{
				lPalette[i].mRed   = (uint8)i;
				lPalette[i].mGreen = (uint8)i;
				lPalette[i].mBlue  = (uint8)i;
			}

			lImage.SetPalette(lPalette);
			lImage.ConvertTo32BitWithAlpha(*pAlphaBuffer);

			Texture* lTexture = new Texture();

			lTexture->mWidth = pAlphaBuffer->GetWidth();
			lTexture->mHeight = pAlphaBuffer->GetHeight();

			mTextureTable.Insert(lID, lTexture);

			glBindTexture (GL_TEXTURE_2D, lID);
			OGL_ASSERT();

			glTexImage2D (GL_TEXTURE_2D,
						  0,
						  GL_RGBA,
						  lImage.GetWidth(),
						  lImage.GetHeight(),
						  0,
						  GL_RGBA,
						  GL_UNSIGNED_BYTE, 
						  lImage.GetBuffer());
			OGL_ASSERT();
		}
	}

	return (ImageID)lID;
}

void OpenGLPainter::UpdateImage(ImageID pImageID, 
				const Canvas* pImage, 
				const Canvas* pAlphaBuffer,
				UpdateHint pHint)
{
	OGL_ASSERT();

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
			if (pImage->GetBitDepth() == Canvas::BITDEPTH_24_BIT)
			{
				glBindTexture (GL_TEXTURE_2D, pImageID);
				glTexImage2D (GL_TEXTURE_2D,
							  0,
							  GL_RGB,
							  pImage->GetWidth(),
							  pImage->GetHeight(),
							  0,
							  GL_RGB,
							  GL_UNSIGNED_BYTE,
							  pImage->GetBuffer());
			}
			else if(pImage->GetBitDepth() == Canvas::BITDEPTH_32_BIT)
			{
				glBindTexture (GL_TEXTURE_2D, pImageID);
				glTexImage2D (GL_TEXTURE_2D,
							  0,
							  GL_RGBA,
							  pImage->GetWidth(),
							  pImage->GetHeight(),
							  0,
							  GL_RGBA,
							  GL_UNSIGNED_BYTE,
							  pImage->GetBuffer());
			}
		}
		return;
	}

	bool lAlpha = false;
	bool lColor = false;

	if (pImage != 0)
	{
		lColor = true;

		if (pImage->GetBitDepth() == Canvas::BITDEPTH_32_BIT)
		{
			lAlpha = true;
		}
	}

	if (pAlphaBuffer != 0)
	{
		lAlpha = true;
	}

	if (lColor == true && lAlpha == true)
	{
		Canvas lImage(*pImage, true);
		unsigned lNewWidth  = GetClosestPowerOf2(lImage.GetWidth(), true);
		unsigned lNewHeight = GetClosestPowerOf2(lImage.GetHeight(), true);
		if (lNewWidth != lImage.GetWidth() || lNewHeight != lImage.GetHeight())
		{
			lImage.Resize(lNewWidth, lNewHeight, Canvas::RESIZE_FAST);
		}

		if (pAlphaBuffer != 0)
		{
			Canvas lAlphaBuffer(*pAlphaBuffer, true);

			if (lAlphaBuffer.GetWidth() != lImage.GetWidth() ||
			   lAlphaBuffer.GetHeight() != lImage.GetHeight())
			{
				lAlphaBuffer.Resize(lImage.GetWidth(), lImage.GetHeight(), Canvas::RESIZE_NICEST);
			}
			if (lAlphaBuffer.GetBitDepth() != Canvas::BITDEPTH_8_BIT)
			{
				lAlphaBuffer.ConvertToGrayscale(true);
			}

			lImage.ConvertTo32BitWithAlpha(lAlphaBuffer);
		}

		lTexture->mWidth = pImage->GetWidth();
		lTexture->mHeight = pImage->GetHeight();

		glBindTexture (GL_TEXTURE_2D, pImageID);

		glTexImage2D (GL_TEXTURE_2D,
					  0,
					  GL_RGBA,
					  lImage.GetWidth(),
					  lImage.GetHeight(),
					  0,
					  GL_RGBA,
					  GL_UNSIGNED_BYTE,
					  lImage.GetBuffer());
	}
	else if(lColor == true)
	{
		Canvas lImage(*pImage, false);
		lImage.ConvertBitDepth(Canvas::BITDEPTH_24_BIT);
		unsigned lNewWidth  = GetClosestPowerOf2(lImage.GetWidth(), true);
		unsigned lNewHeight = GetClosestPowerOf2(lImage.GetHeight(), true);
		if (lNewWidth != lImage.GetWidth() || lNewHeight != lImage.GetHeight())
		{
			lImage.Resize(lNewWidth, lNewHeight, Canvas::RESIZE_FAST);
		}

		lTexture->mWidth = pImage->GetWidth();
		lTexture->mHeight = pImage->GetHeight();

		mTextureTable.Insert(pImageID, lTexture);

		glBindTexture (GL_TEXTURE_2D, pImageID);

		glTexImage2D (GL_TEXTURE_2D,
					  0,
					  GL_RGB, 
					  lImage.GetWidth(),
					  lImage.GetHeight(),
					  0,
					  GL_RGB,
					  GL_UNSIGNED_BYTE,
					  lImage.GetBuffer());
	}
	else if(pAlphaBuffer != 0)
	{
		Canvas lImage(*pAlphaBuffer, true);
		lImage.ConvertTo32BitWithAlpha(*pAlphaBuffer);
		unsigned lNewWidth  = GetClosestPowerOf2(lImage.GetWidth(), true);
		unsigned lNewHeight = GetClosestPowerOf2(lImage.GetHeight(), true);
		if (lNewWidth != lImage.GetWidth() || lNewHeight != lImage.GetHeight())
		{
			lImage.Resize(lNewWidth, lNewHeight, Canvas::RESIZE_FAST);
		}

		Texture* lTexture = new Texture();

		lTexture->mWidth = pImage->GetWidth();
		lTexture->mHeight = pImage->GetHeight();

		mTextureTable.Insert(pImageID, lTexture);

		glBindTexture (GL_TEXTURE_2D, pImageID);

		glTexImage2D (GL_TEXTURE_2D, 
					  0, 
					  GL_RGBA, 
					  lImage.GetWidth(), 
					  lImage.GetHeight(), 
					  0, 
					  GL_RGBA,
					  GL_UNSIGNED_BYTE, 
					  lImage.GetBuffer());
	}
	else
	{
		RemoveImage(pImageID);
	}

	OGL_ASSERT();
}

void OpenGLPainter::RemoveImage(ImageID pImageID)
{
	OGL_ASSERT();

	TextureTable::Iterator lIter = mTextureTable.Find(pImageID);

	if (lIter == mTextureTable.End())
	{
		return;
	}

	Texture* lTexture = *lIter;

	mTextureTable.Remove(lIter);

	delete lTexture;

	GLuint lTextureName = (GLuint)pImageID;
	glDeleteTextures(1, &lTextureName);

	mTextureIDManager.RecycleId(pImageID);

	OGL_ASSERT();
}

void OpenGLPainter::DoDrawImage(ImageID pImageID, int x, int y)
{
	TextureTable::Iterator lIter = mTextureTable.Find(pImageID);

	if (lIter == mTextureTable.End())
	{
		return;
	}

	Texture* lTexture = *lIter;

	ToScreenCoords(x, y);
	PixelRect lRect(x, y, x + lTexture->mWidth, y + lTexture->mHeight);
	ToUserCoords(lRect.mLeft, lRect.mTop);
	ToUserCoords(lRect.mRight, lRect.mBottom);
	DrawImage(pImageID, lRect);
}

void OpenGLPainter::DoDrawAlphaImage(ImageID pImageID, int x, int y)
{
	OGL_ASSERT();

	ToScreenCoords(x, y);

	TextureTable::Iterator lIter = mTextureTable.Find(pImageID);

	if (lIter == mTextureTable.End())
	{
		return;
	}

	Texture* lTexture = *lIter;

	GLfloat lLeft   = (GLfloat)x;
	GLfloat lRight  = (GLfloat)(x + lTexture->mWidth);
	GLfloat lTop    = (GLfloat)y;
	GLfloat lBottom = (GLfloat)(y + lTexture->mHeight);

	::glEnable(GL_TEXTURE_2D);

	glBindTexture (GL_TEXTURE_2D, pImageID);
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

	Color& lColor = GetColorInternal(0);
	if (GetRenderMode() == RM_ALPHATEST)
	{
		glColor4ub(lColor.mRed, lColor.mGreen, lColor.mBlue, 255);
	}
	else
	{
		glColor4ub(lColor.mRed, lColor.mGreen, lColor.mBlue, GetAlphaValue());
	}

	GLfloat u[] = {0,0, 1,0, 1,1, 0,1};
	GLfloat v[] = {lLeft,lTop, lRight,lTop, lRight,lBottom, lLeft,lBottom};
	::glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	::glTexCoordPointer(2, GL_FLOAT, 0, u);
	::glVertexPointer(2, GL_FLOAT, 0, v);
	::glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	::glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	::glDisable(GL_TEXTURE_2D);

	OGL_ASSERT();
}

void OpenGLPainter::DoDrawImage(ImageID pImageID, const PixelRect& pRect)
{
	OGL_ASSERT();

	TextureTable::Iterator lIter = mTextureTable.Find(pImageID);

	if (lIter == mTextureTable.End())
	{
		return;
	}

	::glEnable(GL_TEXTURE_2D);

	glBindTexture (GL_TEXTURE_2D, pImageID);
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

	if (GetRenderMode() == RM_ALPHATEST)
	{
		glColor4ub(255, 255, 255, 255);
	}
	else
	{
		glColor4ub(255, 255, 255, GetAlphaValue());
	}

	GLfloat u[] = {0, 0, 1, 0, 1, 1, 0, 1};
	GLfloat v[] = {(float)pRect.mLeft, (float)pRect.mTop, (float)pRect.mRight, (float)pRect.mTop, (float)pRect.mRight, (float)pRect.mBottom, (float)pRect.mLeft, (float)pRect.mBottom};
	::glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	::glTexCoordPointer(2, GL_FLOAT, 0, u);
	::glVertexPointer(2, GL_FLOAT, 0, v);
	::glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	::glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	::glDisable(GL_TEXTURE_2D);

	OGL_ASSERT();
}

void OpenGLPainter::DoDrawImage(ImageID pImageID, int x, int y, const PixelRect& pSubpatchRect)
{
	ToScreenCoords(x, y);
	PixelRect lRect(x, y, x + pSubpatchRect.GetWidth(), y + pSubpatchRect.GetHeight());
	ToUserCoords(lRect.mLeft, lRect.mTop);
	ToUserCoords(lRect.mRight, lRect.mBottom);
	DrawImage(pImageID, lRect, pSubpatchRect);
}

void OpenGLPainter::DoDrawImage(ImageID pImageID, const PixelRect& pRect, const PixelRect& pSubpatchRect)
{
	OGL_ASSERT();

	TextureTable::Iterator lIter = mTextureTable.Find(pImageID);

	if (lIter == mTextureTable.End())
	{
		return;
	}

	Texture* lTexture = *lIter;

	GLfloat lLeft   = (GLfloat)pRect.mLeft;
	GLfloat lRight  = (GLfloat)pRect.mRight;
	GLfloat lTop    = (GLfloat)pRect.mTop;
	GLfloat lBottom = (GLfloat)pRect.mBottom;

	ToScreenCoords(lLeft, lTop);
	ToScreenCoords(lRight, lBottom);

	::glEnable(GL_TEXTURE_2D);

	glBindTexture (GL_TEXTURE_2D, pImageID);
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

	if (GetRenderMode() == RM_ALPHATEST)
	{
		glColor4ub(255, 255, 255, 255);
	}
	else
	{
		glColor4ub(255, 255, 255, GetAlphaValue());
	}

	GLfloat lOneOverWidth  = 1.0f / (GLfloat)lTexture->mWidth;
	GLfloat lOneOverHeight = 1.0f / (GLfloat)lTexture->mHeight;
	GLfloat lULeft   = ((GLfloat)pSubpatchRect.mLeft)   * lOneOverWidth;
	GLfloat lURight  = ((GLfloat)pSubpatchRect.mRight)  * lOneOverWidth;
	GLfloat lVTop    = ((GLfloat)pSubpatchRect.mTop)    * lOneOverHeight;
	GLfloat lVBottom = ((GLfloat)pSubpatchRect.mBottom) * lOneOverHeight;

	GLfloat u[] = {lULeft,lVTop, lURight,lVTop, lURight,lVBottom, lULeft,lVBottom};
	GLfloat v[] = {lLeft,lTop, lRight,lTop, lRight,lBottom, lLeft,lBottom};
	::glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	::glTexCoordPointer(2, GL_FLOAT, 0, u);
	::glVertexPointer(2, GL_FLOAT, 0, v);
	::glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	::glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	::glDisable(GL_TEXTURE_2D);

	OGL_ASSERT();
}

void OpenGLPainter::GetImageSize(ImageID pImageID, int& pWidth, int& pHeight) const
{
	pWidth = 0;
	pHeight = 0;

	TextureTable::ConstIterator lIter = mTextureTable.Find((int)pImageID);
	if(lIter != mTextureTable.End())
	{
		Texture* lTexture = *lIter;
		pWidth = lTexture->mWidth;
		pHeight = lTexture->mHeight;
	}
}

void OpenGLPainter::PrintText(const str& pString, int x, int y)
{
	if (pString.empty())
	{
		return;
	}

	// Algo goes something like this. It's a texture-mapping font rendering algo.
	//  1. Loop over each char in the string
	//     - cache each char as necessary in a texture (which doubles in size when required)
	//     - append indices, vertices and texels for each char in an array.
	//  2. After loop done: render appended geometries.

	ToScreenCoords(x, y);

	int lCurrentX = x;
	int lCurrentY = y;

	//::glPushAttrib(GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT);

	if (UiLepra::OpenGLExtensions::IsBufferObjectsSupported() == true)
	{
		UiLepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, 0);
		UiLepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	::glDisableClientState(GL_COLOR_ARRAY);
#ifndef LEPRA_GL_ES
	::glDisableClientState(GL_INDEX_ARRAY);
#endif // !GLES
	::glDisableClientState(GL_NORMAL_ARRAY);
	::glEnableClientState(GL_VERTEX_ARRAY);
	::glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	PushAttrib(ATTR_RENDERMODE);
	if (mSmoothFont)
	{
		SetRenderMode(RM_ALPHABLEND);
	}
	else
	{
		SetRenderMode(RM_ALPHATEST);
	}
	::glEnable(GL_TEXTURE_2D);
	//::glMatrixMode(GL_TEXTURE);
	//::glLoadIdentity();
	//::glMatrixMode(GL_MODELVIEW);

	const int lFontHeight = GetFontManager()->GetFontHeight();
	const int lLineHeight = GetFontManager()->GetLineHeight();
	const int lSpaceSize = GetFontManager()->GetCharWidth(' ');
	const int lTabSize = lSpaceSize*4;

	const Color lColor = GetColorInternal(0);
	assert(lColor != BLACK);	// Does not show.
	GetFontManager()->SetColor(Color(255, 255, 255, 255));
	::glColor4f(lColor.GetRf(), lColor.GetGf(), lColor.GetBf(), lColor.GetAf());
	const uint32 lFontHash = (GetFontManager()->GetActiveFontId() << 16) + lFontHeight;
	FontTexture* lFontTexture = SelectGlyphs(lFontHash, lFontHeight, pString);
	const int lFontTextureHeight = lFontTexture->GetHeight();

	const size_t lStringLength = pString.length();
	int lGlyphIndex = 0;
	const size_t STACK_GLYPH_CAPACITY = 256;
#ifdef LEPRA_GL_ES
#define INDICES_PER_GLYPH 6
#define VERTEX_TYPE       GLshort
#define VERTEX_TYPE_GL_ID GL_SHORT
	static const vtx_idx_t lTemplateGlyphIndices[] =
	{
		0, 1, 2, 2, 1, 3,
	};
#else // !GLES
#define INDICES_PER_GLYPH 4
#define VERTEX_TYPE       GLint
#define VERTEX_TYPE_GL_ID GL_INT
	static const vtx_idx_t lTemplateGlyphIndices[] =
	{
		0, 1, 3, 2,
	};
#endif // GLES/!GLES
	vtx_idx_t lArrayGlyphIndices[INDICES_PER_GLYPH*STACK_GLYPH_CAPACITY];
	vtx_idx_t* lGlyphIndices = lArrayGlyphIndices;
	VERTEX_TYPE lArrayVertices[2*4*STACK_GLYPH_CAPACITY];
	GLfloat lArrayUv[2*4*STACK_GLYPH_CAPACITY];
	VERTEX_TYPE* lVertices = lArrayVertices;
	GLfloat* lUv = lArrayUv;
	bool lAllocPrimitives = (lStringLength > STACK_GLYPH_CAPACITY);
	if (lAllocPrimitives)
	{
		lGlyphIndices = new vtx_idx_t[INDICES_PER_GLYPH*lStringLength];
		lVertices = new VERTEX_TYPE[2*4*lStringLength];
		lUv = new GLfloat[2*4*lStringLength];
	}

	for (size_t i = 0; i < lStringLength; i++)
	{
		const tchar lChar = pString[i];
		if (lChar == _T('\n'))
		{
			lCurrentY += lLineHeight;
			lCurrentX = x;
		}
		else if (lChar == _T(' '))
		{
			lCurrentX += lSpaceSize;
		}
		else if(lChar == _T('\t'))
		{
			lCurrentX = (lCurrentX/lTabSize+1) * lTabSize;
		}
		else if(lChar != _T('\r') && 
			lChar != _T('\b'))
		{
			int lTextureX = 0;
			int lCharWidth = 5;
			if (!lFontTexture->GetGlyphX(lChar, lTextureX, lCharWidth))
			{
				assert(false);
				continue;
			}
			const float lTextureWidth = (float)lFontTexture->GetWidth();
			const VERTEX_TYPE lTemplateVertices[2*4] =
			{
				lCurrentX,		lCurrentY,
				lCurrentX + lCharWidth,	lCurrentY,
				lCurrentX,		lCurrentY + lFontTextureHeight,
				lCurrentX + lCharWidth,	lCurrentY + lFontTextureHeight,
			};
			const GLfloat lTemplateUv[2*4] =
			{
				lTextureX/lTextureWidth,		1,
				(lTextureX + lCharWidth)/lTextureWidth,	1,
				lTextureX/lTextureWidth,		0,
				(lTextureX + lCharWidth)/lTextureWidth,	0,
			};
			const int lIndexBase = lGlyphIndex*INDICES_PER_GLYPH;
			const int lVertexBase = lGlyphIndex*4;
			for (int i = 0; i < INDICES_PER_GLYPH; ++i)
			{
				lGlyphIndices[lIndexBase+i] = lTemplateGlyphIndices[i]+lVertexBase;
			}
			for (int i = 0; i < 4; ++i)
			{
				lVertices[(lVertexBase+i)*2+0] = lTemplateVertices[i*2+0];
				lVertices[(lVertexBase+i)*2+1] = lTemplateVertices[i*2+1];
				lUv[(lVertexBase+i)*2+0] = lTemplateUv[i*2+0];
				lUv[(lVertexBase+i)*2+1] = lTemplateUv[i*2+1];
			}
			++lGlyphIndex;

			lCurrentX += lCharWidth;
		}
	}

	::glVertexPointer(2, VERTEX_TYPE_GL_ID, 0, lVertices);
	::glTexCoordPointer(2, GL_FLOAT, 0, lUv);
#ifdef LEPRA_GL_ES
	::glDrawElements(GL_TRIANGLES, INDICES_PER_GLYPH*lGlyphIndex, LEPRA_GL_INDEX_TYPE, lGlyphIndices);
#else // !GLES
	::glDrawElements(GL_QUADS, INDICES_PER_GLYPH*lGlyphIndex, LEPRA_GL_INDEX_TYPE, lGlyphIndices);
#endif // GLES/!GLES

	if (lAllocPrimitives)
	{
		delete[] (lGlyphIndices);
		delete[] (lVertices);
		delete[] (lUv);
	}

	PopAttrib();

	::glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	::glDisable(GL_TEXTURE_2D);
}

void OpenGLPainter::ReadPixels(Canvas& pDestCanvas, const PixelRect& pRect)
{
	OGL_ASSERT();

	if (GetCanvas() == 0 || GetCanvas()->GetBitDepth() == Canvas::BITDEPTH_8_BIT)
	{
		pDestCanvas.Reset(0, 0, Canvas::BITDEPTH_32_BIT);
		return;
	}

	PixelRect lRect(pRect);

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
		pDestCanvas.Reset(0, 0, Canvas::BITDEPTH_32_BIT);
		return;
	}

	const unsigned lWidth  = lRect.GetWidth();
	const unsigned lHeight = lRect.GetHeight();

	if (pDestCanvas.GetBitDepth() != GetCanvas()->GetBitDepth() ||
	   pDestCanvas.GetWidth() != lWidth ||
	   pDestCanvas.GetHeight() != lHeight)
	{
		pDestCanvas.Reset(lWidth, lHeight, Canvas::BITDEPTH_32_BIT);
		pDestCanvas.CreateBuffer();
	}

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
#ifndef LEPRA_GL_ES
	glReadBuffer(GL_FRONT_LEFT);
	glPixelStorei(GL_PACK_ROW_LENGTH, 0);
	glPixelStorei(GL_PACK_SKIP_ROWS, 0);
	glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
#endif // !GLES

	switch(GetCanvas()->GetBitDepth())
	{
		case Canvas::BITDEPTH_15_BIT:
		case Canvas::BITDEPTH_16_BIT:
		case Canvas::BITDEPTH_24_BIT:
		case Canvas::BITDEPTH_32_BIT:
		{
			glReadPixels(lRect.mLeft,
						 GetCanvas()->GetHeight() - lRect.mBottom,
						 lRect.GetWidth(),
						 lRect.GetHeight(),
						 GL_RGBA,
						 GL_UNSIGNED_BYTE,
						 pDestCanvas.GetBuffer());
		break;
		}
	default:
		break;
	}

	pDestCanvas.SwapRGBOrder();
	pDestCanvas.FlipVertical();

	OGL_ASSERT();
}

Painter::RGBOrder OpenGLPainter::GetRGBOrder() const
{
	return RGB;
}

void OpenGLPainter::SetFontSmoothness(bool pSmooth)
{
	if (mSmoothFont != pSmooth)
	{
		mSmoothFont = pSmooth;
		ClearFontBuffers();
	}
}



void OpenGLPainter::AdjustVertexFormat(unsigned& pVertexFormat)
{
#ifdef LEPRA_GL_ES
	pVertexFormat |= Geometry2D::VTX_INDEX16;
#else // !GLES
	// Default behaviour. Do nothing.
	pVertexFormat;
#endif
}

void OpenGLPainter::DoRenderDisplayList(std::vector<DisplayEntity*>* pDisplayList)
{
	OGL_ASSERT();

	PushAttrib(ATTR_ALL);

	::glDisableClientState(GL_NORMAL_ARRAY);
#ifndef LEPRA_GL_ES
	::glDisableClientState(GL_INDEX_ARRAY);
#endif // !GLES
	UiLepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, 0);
	UiLepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	std::vector<DisplayEntity*>::iterator it;
	for(it = pDisplayList->begin(); it != pDisplayList->end(); ++it)
	{
		DisplayEntity* lGeneratedGeometry = *it;
		Painter::SetClippingRect(lGeneratedGeometry->GetClippingRect());
		SetAlphaValue(lGeneratedGeometry->GetAlpha());
		SetRenderMode(lGeneratedGeometry->GetRenderMode());

		// Enabled in ResetClippingRect().
		glVertexPointer(2, GL_FLOAT, 0, lGeneratedGeometry->GetGeometry().GetVertexData());

		if (lGeneratedGeometry->GetGeometry().GetColorData() != 0)
		{
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(3, GL_FLOAT, 0, lGeneratedGeometry->GetGeometry().GetColorData());
		}
		else
		{
			glDisableClientState(GL_COLOR_ARRAY);
		}

		if (lGeneratedGeometry->GetImageID() != INVALID_IMAGEID)
		{
			assert(lGeneratedGeometry->GetGeometry().GetUVData() != 0);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2, GL_FLOAT, 0, lGeneratedGeometry->GetGeometry().GetUVData());

			::glEnable(GL_TEXTURE_2D);

			glBindTexture (GL_TEXTURE_2D, lGeneratedGeometry->GetImageID());
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

			if (GetRenderMode() == RM_ALPHATEST)
			{
				glColor4ub(255, 255, 255, 255);
			}
			else
			{
				glColor4ub(255, 255, 255, GetAlphaValue());
			}
		}
		else
		{
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}

		const int lTriangleEntryCount = lGeneratedGeometry->GetGeometry().GetTriangleCount() * 3;
#ifdef LEPRA_GL_ES
		const uint16* lTriangleData16 = lGeneratedGeometry->GetGeometry().GetTriangleData16();
		::glDrawElements(GL_TRIANGLES, lTriangleEntryCount, GL_UNSIGNED_SHORT, lTriangleData16);
#else // !GLES
		const uint32* lTriangleData32 = lGeneratedGeometry->GetGeometry().GetTriangleData32();
		::glDrawElements(GL_TRIANGLES, lTriangleEntryCount, GL_UNSIGNED_INT, lTriangleData32);
#endif

		::glDisable(GL_TEXTURE_2D);
	}

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	PopAttrib();

	OGL_ASSERT();
}




void OpenGLPainter::ClearFontBuffers()
{
	//OGL_ASSERT();

	FontTextureTable::iterator x = mFontTextureTable.begin();
	for (; x != mFontTextureTable.end(); ++x)
	{
		GLuint lFontHash = x->second->GetFontHash();
		::glDeleteTextures(1, &lFontHash);
		delete x->second;
	}
	mFontTextureTable.clear();

	//OGL_ASSERT();
}

FontTexture* OpenGLPainter::SelectGlyphs(uint32 pFontHash, int pFontHeight, const str& pString)
{
	FontTexture* lFontTexture = HashUtil::FindMapObject(mFontTextureTable, pFontHash);
	if (!lFontTexture)
	{
		lFontTexture = new FontTexture(pFontHash, pFontHeight);
		mFontTextureTable.insert(FontTextureTable::value_type(pFontHash, lFontTexture));
	}
	const size_t lLength = pString.length();
	for (size_t x = 0; x < lLength; ++x)
	{
		lFontTexture->StoreGlyph(pString[x], GetFontManager());
	}
	OGL_ASSERT();
	::glBindTexture(GL_TEXTURE_2D, pFontHash);
	if (lFontTexture->IsUpdated())
	{
		lFontTexture->ResetIsUpdated();
		OGL_ASSERT();
		::glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_RGBA,
			lFontTexture->GetWidth(),
			lFontTexture->GetHeight(),
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			lFontTexture->GetBuffer());
		OGL_ASSERT();
		::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	return (lFontTexture);
}



}
