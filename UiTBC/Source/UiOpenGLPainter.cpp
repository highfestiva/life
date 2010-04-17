
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Jonas Byström



#include "../Include/UiOpenGLPainter.h"
#include <math.h>
#include "../Include/UiFontManager.h"



namespace UiTbc
{



OpenGLPainter::OpenGLPainter() :
	mTextureIDManager(3, 10000, 0),
	mSmoothFont(false)
{
}

OpenGLPainter::~OpenGLPainter()
{
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
}

void OpenGLPainter::SetDestCanvas(Canvas* pCanvas)
{
	Painter::SetDestCanvas(pCanvas);
	ResetClippingRect();
}

void OpenGLPainter::SetAlphaValue(Lepra::uint8 pAlpha)
{
	Painter::SetAlphaValue(pAlpha);
	float lAlpha = (float)GetAlphaValue() / 255.0f;
	glAlphaFunc(GL_GEQUAL, (GLclampf)lAlpha);
}

void OpenGLPainter::SetRenderMode(RenderMode pRM)
{
	if (pRM != GetRenderMode())
	{
		Painter::SetRenderMode(pRM);
		DoSetRenderMode();
	}
}

void OpenGLPainter::PrePaint()
{
	DoSetRenderMode();
}

void OpenGLPainter::SetClippingRect(int pLeft, int pTop, int pRight, int pBottom)
{
	Painter::SetClippingRect(pLeft, pTop, pRight, pBottom);
	ToScreenCoords(pLeft, pTop);
	ToScreenCoords(pRight, pBottom);
	glScissor(pLeft, GetCanvas()->GetHeight() - pBottom, pRight - pLeft, pBottom - pTop);
}

void OpenGLPainter::ResetClippingRect()
{
	// A call to this function should reset OpenGL's projection matrix to orthogonal
	// in order to work together with OpenGL3DAPI.
	glViewport(0, 0, GetCanvas()->GetWidth(), GetCanvas()->GetHeight());
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Definition of the viewport. The point (0, 0) is defined as the center of the
	// pixel in the top left corner. Thus, the top left corner of the screen has
	// the coordinates (-0.5, -0.5).
	glOrtho(-0.5, (float32)GetCanvas()->GetWidth() - 0.5,
			(float32)GetCanvas()->GetHeight() - 0.5, -0.5,
			0.0f, 1.0f);
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
	::glDisable(GL_CULL_FACE);
	::glLineWidth(1);
	::glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);
	::glDisable(GL_LINE_SMOOTH);

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
}

void OpenGLPainter::DoDrawPixel(int x, int y)
{
	ToScreenCoords(x, y);

	GLfloat lX = (GLfloat)x;
	GLfloat lY = (GLfloat)y;

	Color& lColor = GetColorInternal(0);
	glColor4ub(lColor.mRed, lColor.mGreen, lColor.mBlue, GetAlphaValue());
	glPointSize(1);
	glBegin(GL_POINTS);
	glVertex2f(lX, lY);
	glEnd();
}

void OpenGLPainter::DoDrawLine(int pX1, int pY1, int pX2, int pY2)
{
	ToScreenCoords(pX1, pY1);
	ToScreenCoords(pX2, pY2);

	Color& lColor = GetColorInternal(0);
	glColor4ub(lColor.mRed, lColor.mGreen, lColor.mBlue, GetAlphaValue());

	//glLineWidth(1);
	glBegin(GL_LINES);
	glVertex2i(pX1, pY1);
	glVertex2i(pX2, pY2);
	glEnd();
}

void OpenGLPainter::DoFillTriangle(float pX1, float pY1,
				 float pX2, float pY2,
				 float pX3, float pY3)
{
	ToScreenCoords(pX1, pY1);
	ToScreenCoords(pX2, pY2);
	ToScreenCoords(pX3, pY3);

	pX1 = pX1 - 0.5f;
	pX2 = pX2 - 0.5f;
	pX3 = pX3 - 0.5f;
	pY1 = pY1 - 0.5f;
	pY2 = pY2 - 0.5f;
	pY3 = pY3 - 0.5f;

	Color& lColor = GetColorInternal(0);
	glColor4ub(lColor.mRed, lColor.mGreen, lColor.mBlue, GetAlphaValue());

	glBegin(GL_TRIANGLES);

	glVertex2f(pX1, pY1);
	glVertex2f(pX2, pY2);
	glVertex2f(pX3, pY3);

	glEnd();
}

void OpenGLPainter::DoFillShadedTriangle(float pX1, float pY1,
				       float pX2, float pY2,
				       float pX3, float pY3)
{
	ToScreenCoords(pX1, pY1);
	ToScreenCoords(pX2, pY2);
	ToScreenCoords(pX3, pY3);

	pX1 = pX1 - 0.5f;
	pX2 = pX2 - 0.5f;
	pX3 = pX3 - 0.5f;
	pY1 = pY1 - 0.5f;
	pY2 = pY2 - 0.5f;
	pY3 = pY3 - 0.5f;

	glBegin(GL_TRIANGLES);

	Color* lColor = &GetColorInternal(0);
	glColor4ub(lColor[0].mRed, lColor[0].mGreen, lColor[0].mBlue, GetAlphaValue());
	glVertex2f(pX1, pY1);
	glColor4ub(lColor[1].mRed, lColor[1].mGreen, lColor[1].mBlue, GetAlphaValue());
	glVertex2f(pX2, pY2);
	glColor4ub(lColor[2].mRed, lColor[2].mGreen, lColor[2].mBlue, GetAlphaValue());
	glVertex2f(pX3, pY3);

	glEnd();
}

void OpenGLPainter::DoFillTriangle(float pX1, float pY1, float pU1, float pV1,
				 float pX2, float pY2, float pU2, float pV2,
				 float pX3, float pY3, float pU3, float pV3,
				 ImageID pImageID)
{
	ToScreenCoords(pX1, pY1);
	ToScreenCoords(pX2, pY2);
	ToScreenCoords(pX3, pY3);

	TextureTable::Iterator lIter = mTextureTable.Find(pImageID);

	if (lIter == mTextureTable.End())
	{
		return;
	}

	pX1	= pX1 - 0.5f;
	pX2	= pX2 - 0.5f;
	pX3	= pX3 - 0.5f;
	pY1	= pY1 - 0.5f;
	pY2	= pY2 - 0.5f;
	pY3	= pY3 - 0.5f;

	glPushAttrib(GL_TEXTURE_BIT);
	glEnable(GL_TEXTURE_2D);

	glBindTexture (GL_TEXTURE_2D, (unsigned)pImageID);
	glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei (GL_UNPACK_ROW_LENGTH, 0);
	glPixelStorei (GL_UNPACK_SKIP_ROWS, 0);
	glPixelStorei (GL_UNPACK_SKIP_PIXELS, 0);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	if (GetRenderMode() == RM_ALPHATEST)
	{
		glColor4ub(255, 255, 255, 255);
	}
	else
	{
		glColor4ub(255, 255, 255, GetAlphaValue());
	}

	glBegin(GL_TRIANGLES);

	glTexCoord2f(pU1, pV1);
	glVertex2f(pX1, pY1);
	glTexCoord2f(pU2, pV2);
	glVertex2f(pX2, pY2);
	glTexCoord2f(pU3, pV3);
	glVertex2f(pX3, pY3);

	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void OpenGLPainter::DoDrawRect(int pLeft, int pTop, int pRight, int pBottom, int pWidth)
{
	ToScreenCoords(pLeft, pTop);
	ToScreenCoords(pRight, pBottom);

	GLfloat lLeft   = (GLfloat)pLeft - 0.5f;
	GLfloat lRight  = (GLfloat)pRight - 0.5f;
	GLfloat lTop    = (GLfloat)pTop - 0.5f;
	GLfloat lBottom = (GLfloat)pBottom - 0.5f;

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

	const static GLuint lIndices[] = {0,4,7, 0,7,3, 0,1,5, 0,5,4, 1,2,6, 1,6,5, 7,6,2, 7,2,3};

	glColor4ub(255, 255, 255, GetAlphaValue());

	// Enabled in ResetClippingRect().
	glVertexPointer(2, GL_FLOAT, 0, lVertex);
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(3, GL_FLOAT, 0, lColor);
	glDrawElements(GL_TRIANGLES,
			24,
			GL_UNSIGNED_INT,
			lIndices);
	glDisableClientState(GL_COLOR_ARRAY);
}

void OpenGLPainter::DoFillRect(int pLeft, int pTop, int pRight, int pBottom)
{
	ToScreenCoords(pLeft, pTop);
	ToScreenCoords(pRight, pBottom);

	GLfloat lLeft   = (GLfloat)pLeft - 0.5f;
	GLfloat lRight  = (GLfloat)pRight - 0.5f;
	GLfloat lTop    = (GLfloat)pTop - 0.5f;
	GLfloat lBottom = (GLfloat)pBottom - 0.5f;

	Color& lColor = GetColorInternal(0);
	glColor4ub(lColor.mRed, lColor.mGreen, lColor.mBlue, GetAlphaValue());

	glBegin(GL_TRIANGLE_FAN);

	glVertex2f(lLeft, lTop);
	glVertex2f(lRight, lTop);
	glVertex2f(lRight, lBottom);
	glVertex2f(lLeft, lBottom);

	glEnd();
}

void OpenGLPainter::DoDraw3DRect(int pLeft, int pTop, int pRight, int pBottom, int pWidth, bool pSunken)
{
	ToScreenCoords(pLeft, pTop);
	ToScreenCoords(pRight, pBottom);

	GLfloat lLeft   = (GLfloat)pLeft - 0.5f;
	GLfloat lRight  = (GLfloat)pRight - 0.5f;
	GLfloat lTop    = (GLfloat)pTop - 0.5f;
	GLfloat lBottom = (GLfloat)pBottom - 0.5f;

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

	const static GLuint lsIndices[] = {0,1,7, 0,7,6, 0,6,10, 0,10,4, 8,2,3, 8,3,9, 11,9,3, 11,3,5};

	::glDisableClientState(GL_NORMAL_ARRAY);
	::glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	// Enabled in ResetClippingRect().
	glVertexPointer(2, GL_FLOAT, 0, lVertex);
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(3, GL_FLOAT, 0, lColor);
	//glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, lsIndices);
	glDisableClientState(GL_COLOR_ARRAY);
}

void OpenGLPainter::DoFillShadedRect(int pLeft, int pTop, int pRight, int pBottom)
{
	ToScreenCoords(pLeft, pTop);
	ToScreenCoords(pRight, pBottom);

	GLfloat lLeft   = (GLfloat)pLeft - 0.5f;
	GLfloat lRight  = (GLfloat)pRight - 0.5f;
	GLfloat lTop    = (GLfloat)pTop - 0.5f;
	GLfloat lBottom = (GLfloat)pBottom - 0.5f;

	// Calculate the color in the middle of the rect.
	GLfloat lTopR = (GLfloat)(mRCol[0].x + mRCol[1].x) * 0.5f;
	GLfloat lTopG = (GLfloat)(mRCol[0].y + mRCol[1].y) * 0.5f;
	GLfloat lTopB = (GLfloat)(mRCol[0].z + mRCol[1].z) * 0.5f;

	GLfloat lBotR = (GLfloat)(mRCol[3].x + mRCol[2].x) * 0.5f;
	GLfloat lBotG = (GLfloat)(mRCol[3].y + mRCol[2].y) * 0.5f;
	GLfloat lBotB = (GLfloat)(mRCol[3].z + mRCol[2].z) * 0.5f;

	glBegin(GL_TRIANGLE_FAN);

	glColor4f((lTopR + lBotR) * 0.5f, (lTopG + lBotG) * 0.5f, (lTopB + lBotB) * 0.5f, (GLfloat)GetAlphaValue() / 255.0f);
	glVertex2f((lLeft + lRight) * 0.5f, (lTop + lBottom) * 0.5f);

	Color* lColor = &GetColorInternal(0);
	glColor4ub(lColor[0].mRed, lColor[0].mGreen, lColor[0].mBlue, GetAlphaValue());
	glVertex2f(lLeft, lTop);

	glColor4ub(lColor[1].mRed, lColor[1].mGreen, lColor[1].mBlue, GetAlphaValue());
	glVertex2f(lRight, lTop);

	glColor4ub(lColor[2].mRed, lColor[2].mGreen, lColor[2].mBlue, GetAlphaValue());
	glVertex2f(lRight, lBottom);

	glColor4ub(lColor[3].mRed, lColor[3].mGreen, lColor[3].mBlue, GetAlphaValue());
	glVertex2f(lLeft, lBottom);

	glColor4ub(lColor[0].mRed, lColor[0].mGreen, lColor[0].mBlue, GetAlphaValue());
	glVertex2f(lLeft, lTop);

	glEnd();
}

Painter::ImageID OpenGLPainter::AddImage(const Canvas* pImage, const Canvas* pAlphaBuffer)
{
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

			glTexImage2D (GL_TEXTURE_2D, 
						  0, 
						  4, 
						  lImage.GetWidth(), 
						  lImage.GetHeight(), 
						  0, 
						  GL_RGBA,
						  GL_UNSIGNED_BYTE,
						  lImage.GetBuffer());
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

			glTexImage2D (GL_TEXTURE_2D, 
						  0, 
						  3, 
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

			Color lPalette[256];
			for (int i = 0; i < 256; i++)
			{
				lPalette[i].mRed   = (Lepra::uint8)i;
				lPalette[i].mGreen = (Lepra::uint8)i;
				lPalette[i].mBlue  = (Lepra::uint8)i;
			}

			lImage.SetPalette(lPalette);
			lImage.ConvertTo32BitWithAlpha(*pAlphaBuffer);

			Texture* lTexture = new Texture();

			lTexture->mWidth = pAlphaBuffer->GetWidth();
			lTexture->mHeight = pAlphaBuffer->GetHeight();

			mTextureTable.Insert(lID, lTexture);

			glBindTexture (GL_TEXTURE_2D, lID);

			glTexImage2D (GL_TEXTURE_2D, 
						  0, 
						  4, 
						  lImage.GetWidth(), 
						  lImage.GetHeight(), 
						  0, 
						  GL_RGBA,
						  GL_UNSIGNED_BYTE, 
						  lImage.GetBuffer());
		}
	}

	return (ImageID)lID;
}

void OpenGLPainter::UpdateImage(ImageID pImageID, 
				const Canvas* pImage, 
				const Canvas* pAlphaBuffer,
				UpdateHint pHint)
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
			if (pImage->GetBitDepth() == Canvas::BITDEPTH_24_BIT)
			{
				glBindTexture (GL_TEXTURE_2D, pImageID);
				glTexImage2D (GL_TEXTURE_2D, 
							  0, 
							  3, 
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
							  4, 
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
					  4, 
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
					  3, 
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
					  4, 
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
}

void OpenGLPainter::RemoveImage(ImageID pImageID)
{
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
	ToScreenCoords(x, y);

	TextureTable::Iterator lIter = mTextureTable.Find(pImageID);

	if (lIter == mTextureTable.End())
	{
		return;
	}

	Texture* lTexture = *lIter;

	GLfloat lLeft   = (GLfloat)x - 0.5f;
	GLfloat lRight  = (GLfloat)(x + lTexture->mWidth) - 0.5f;
	GLfloat lTop    = (GLfloat)y - 0.5f;
	GLfloat lBottom = (GLfloat)(y + lTexture->mHeight) - 0.5f;

	glPushAttrib(GL_TEXTURE_BIT);
	glPushAttrib(GL_COLOR_BUFFER_BIT);

	glEnable(GL_TEXTURE_2D);

	glBindTexture (GL_TEXTURE_2D, pImageID);
	glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei (GL_UNPACK_ROW_LENGTH, 0);
	glPixelStorei (GL_UNPACK_SKIP_ROWS, 0);
	glPixelStorei (GL_UNPACK_SKIP_PIXELS, 0);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

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

	glBegin(GL_TRIANGLE_FAN);

	glTexCoord2f(0, 0);
	glVertex2f(lLeft, lTop);

	glTexCoord2f(1, 0);
	glVertex2f(lRight, lTop);

	glTexCoord2f(1, 1);
	glVertex2f(lRight, lBottom);

	glTexCoord2f(0, 1);
	glVertex2f(lLeft, lBottom);

	glEnd();

	glPopAttrib();
	glPopAttrib();
}

void OpenGLPainter::DoDrawImage(ImageID pImageID, const PixelRect& pRect)
{
	TextureTable::Iterator lIter = mTextureTable.Find(pImageID);

	if (lIter == mTextureTable.End())
	{
		return;
	}

	GLfloat lLeft   = (GLfloat)pRect.mLeft - 0.5f;
	GLfloat lRight  = (GLfloat)pRect.mRight - 0.5f;
	GLfloat lTop    = (GLfloat)pRect.mTop - 0.5f;
	GLfloat lBottom = (GLfloat)pRect.mBottom - 0.5f;

	ToScreenCoords(lLeft, lTop);
	ToScreenCoords(lRight, lBottom);

	glPushAttrib(GL_TEXTURE_BIT);

	glEnable(GL_TEXTURE_2D);

	glBindTexture (GL_TEXTURE_2D, pImageID);
	glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei (GL_UNPACK_ROW_LENGTH, 0);
	glPixelStorei (GL_UNPACK_SKIP_ROWS, 0);
	glPixelStorei (GL_UNPACK_SKIP_PIXELS, 0);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	if (GetRenderMode() == RM_ALPHATEST)
	{
		glColor4ub(255, 255, 255, 255);
	}
	else
	{
		glColor4ub(255, 255, 255, GetAlphaValue());
	}

	glBegin(GL_TRIANGLE_FAN);

	glTexCoord2f(0, 0);
	glVertex2f(lLeft, lTop);

	glTexCoord2f(1, 0);
	glVertex2f(lRight, lTop);

	glTexCoord2f(1, 1);
	glVertex2f(lRight, lBottom);

	glTexCoord2f(0, 1);
	glVertex2f(lLeft, lBottom);

	glEnd();

	glPopAttrib();
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
	TextureTable::Iterator lIter = mTextureTable.Find(pImageID);

	if (lIter == mTextureTable.End())
	{
		return;
	}

	Texture* lTexture = *lIter;

	GLfloat lLeft   = (GLfloat)pRect.mLeft - 0.5f;
	GLfloat lRight  = (GLfloat)pRect.mRight - 0.5f;
	GLfloat lTop    = (GLfloat)pRect.mTop - 0.5f;
	GLfloat lBottom = (GLfloat)pRect.mBottom - 0.5f;

	ToScreenCoords(lLeft, lTop);
	ToScreenCoords(lRight, lBottom);

	glPushAttrib(GL_TEXTURE_BIT);

	glEnable(GL_TEXTURE_2D);

	glBindTexture (GL_TEXTURE_2D, pImageID);
	glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei (GL_UNPACK_ROW_LENGTH, 0);
	glPixelStorei (GL_UNPACK_SKIP_ROWS, 0);
	glPixelStorei (GL_UNPACK_SKIP_PIXELS, 0);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

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
	GLfloat lULeft   = ((GLfloat)pSubpatchRect.mLeft   + 0.5f) * lOneOverWidth;
	GLfloat lURight  = ((GLfloat)pSubpatchRect.mRight  + 0.5f) * lOneOverWidth;
	GLfloat lVTop    = ((GLfloat)pSubpatchRect.mTop    + 0.5f) * lOneOverHeight;
	GLfloat lVBottom = ((GLfloat)pSubpatchRect.mBottom + 0.5f) * lOneOverHeight;

	glBegin(GL_TRIANGLE_FAN);

	glTexCoord2f(lULeft, lVTop);
	glVertex2f(lLeft, lTop);

	glTexCoord2f(lURight, lVTop);
	glVertex2f(lRight, lTop);

	glTexCoord2f(lURight, lVBottom);
	glVertex2f(lRight, lBottom);

	glTexCoord2f(lULeft, lVBottom);
	glVertex2f(lLeft, lBottom);

	glEnd();

	glPopAttrib();
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

int OpenGLPainter::PrintText(const str& pString, int x, int y)
{
	ToScreenCoords(x, y);

	int lCurrentX = x;
	int lCurrentY = y+2;	// GL fonts are offset by a little bit.

	glPushAttrib(GL_TEXTURE_BIT);
	glPushAttrib(GL_COLOR_BUFFER_BIT);

	if (mSmoothFont)
	{
		::glEnable(GL_BLEND);
	}
	else
	{
		::glDisable(GL_BLEND);
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
	glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);

	const int lFontHeight = GetFontManager()->GetFontHeight();
	const int lLineHeight = GetFontManager()->GetLineHeight();
	const int lSpaceSize = GetFontManager()->GetCharWidth(' ');
	const int lTabSize = lSpaceSize*4;

	const Color lColor = GetColorInternal(0);
	assert(lColor != BLACK);	// Does not show.
	GetFontManager()->SetColor(lColor);
	::glColor4f(lColor.GetRf(), lColor.GetGf(), lColor.GetBf(), lColor.GetAf());
	Lepra::uint32 lColorHash = lColor.To32();
	lColorHash = (lColorHash&0xC0000000) + ((lColorHash&0x00C00000)<<6) + ((lColorHash&0x0000C000)<<12);
	const Lepra::uint32 lFontId = GetFontManager()->GetActiveFont() << 16;
	const Lepra::uint64 lFontHash = ((Lepra::uint64)(lColorHash+lFontId+lFontHeight)) << 32;

	for (size_t i = 0; i < pString.length(); i++)
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
			int lExtraY = -lFontHeight/4;
			if (mSmoothFont)
			{
				lExtraY = lFontHeight*3/4;
			}
			::glRasterPos2i(lCurrentX, lCurrentY + lExtraY);

			const int lCharWidth = GetFontManager()->GetCharWidth(lChar);
			GlyphTable::iterator lGlyph = mGlyphTable.find(lFontHash+lChar);
			if (lGlyph != mGlyphTable.end())
			{
				::glCallList(lGlyph->second);
			}
			else
			{
				GLuint lGlGlyph = ::glGenLists(1);
				mGlyphTable.insert(GlyphTable::value_type(lFontHash+lChar, lGlGlyph));
				const int lWidth = lCharWidth;
				const int lHeight = lFontHeight;
				Canvas lGlyphImage(lWidth, lHeight, Canvas::BITDEPTH_32_BIT);
				GetFontManager()->RenderGlyph(lChar, lGlyphImage, PixelRect(0, 0, lWidth, lHeight));
				::glNewList(lGlGlyph, GL_COMPILE_AND_EXECUTE);
				if (mSmoothFont)
				{
					::glDrawPixels(lWidth,lHeight, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, lGlyphImage.GetBuffer());
				}
				else
				{
					// Convert 32-bit image to monochrome *BIT* map.
					const int lMaxSize = 350;
					assert(lWidth < lMaxSize && lHeight < lMaxSize);
					unsigned char lData[lMaxSize*lMaxSize/8];
					::memset(lData, 0, sizeof(lData));
					const int lStride = (lWidth+7)/8;
					for (int y = 0; y < lHeight; ++y)
					{
						for (int x = 0; x < lWidth; ++x)
						{
							if (lGlyphImage.GetPixelColor(x, y).mAlpha > 1)
							{
								const int lByte = x/8;
								const int lMask = 1 << (7-(x%8));
								lData[y*lStride + lByte] |= lMask;
							}
						}
					}
					::glBitmap(lWidth,lHeight, 0,(float)lHeight-1, (float)lWidth,0, lData);
				}
				::glEndList();
			}
			lCurrentX += lCharWidth;
		}
	}

	glPopAttrib();
	glPopAttrib();

	int lTemp = 0;
	ToUserCoords(lCurrentX, lTemp);

	return lCurrentX;
}

void OpenGLPainter::ReadPixels(Canvas& pDestCanvas, const PixelRect& pRect)
{
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

	glReadBuffer(GL_FRONT_LEFT);
	glPixelStorei (GL_PACK_ALIGNMENT, 1);
	glPixelStorei (GL_PACK_ROW_LENGTH, 0);
	glPixelStorei (GL_PACK_SKIP_ROWS, 0);
	glPixelStorei (GL_PACK_SKIP_PIXELS, 0);

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



void OpenGLPainter::DoRenderDisplayList(std::vector<DisplayEntity*>* pDisplayList)
{
	PushAttrib(ATTR_ALL);

	::glDisableClientState(GL_NORMAL_ARRAY);
	::glDisableClientState(GL_INDEX_ARRAY);
	UiLepra::OpenGLExtensions::glBindBuffer(GL_ARRAY_BUFFER, 0);
	UiLepra::OpenGLExtensions::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	std::vector<DisplayEntity*>::iterator it;
	for(it = pDisplayList->begin(); it != pDisplayList->end(); ++it)
	{
		glPushAttrib(GL_TEXTURE_BIT);

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

			glEnable(GL_TEXTURE_2D);

			glBindTexture (GL_TEXTURE_2D, lGeneratedGeometry->GetImageID());
			glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
			glPixelStorei (GL_UNPACK_ROW_LENGTH, 0);
			glPixelStorei (GL_UNPACK_SKIP_ROWS, 0);
			glPixelStorei (GL_UNPACK_SKIP_PIXELS, 0);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

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
		const Lepra::uint32* lTriangleData = lGeneratedGeometry->GetGeometry().GetTriangleData();
		::glDrawElements(GL_TRIANGLES, lTriangleEntryCount, GL_UNSIGNED_INT, lTriangleData);

		::glPopAttrib();
	}

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	PopAttrib();
}




void OpenGLPainter::ClearFontBuffers()
{
	GlyphTable::iterator x = mGlyphTable.begin();
	for (; x != mGlyphTable.end(); ++x)
	{
		::glDeleteLists(x->second, 1);
	}
	mGlyphTable.clear();
}



}
