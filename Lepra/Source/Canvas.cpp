/*
	Class:  Canvas
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games
*/

#include <math.h>
#include <memory.h>
#include "../Include/Canvas.h"
#include "../Include/GammaLookup.h"
#include "../Include/Math.h"

#ifdef LEPRA_MSVC
#pragma warning(disable: 4244)	// Compiler bug (2003 .NET), unable to circumvent, causes conversion - possible loss of data.
#endif // LEPRA_MSVC

namespace Lepra
{

bool Canvas::smResizeDataInitialized = false;

const int Canvas::smSamplingGridSize = 8;
const float32 Canvas::smSamplingGridPointSpace = 0.25f;
const float32 Canvas::smResizeSharpness = 0.5f;

float32 Canvas::smSamplingGridWeight[Canvas::smSamplingGridSize * Canvas::smSamplingGridSize];
float32 Canvas::smSamplingGridCoordLookUp[Canvas::smSamplingGridSize];

Canvas::Canvas()
{
	mBufferResponsibility = false;
	mBuffer = 0;
	mBitDepth = BITDEPTH_8_BIT;
	mWidth = 0;
	mHeight = 0;
	mPitch = 0;
	mPixelSize = 1;
}

Canvas::Canvas(unsigned pWidth, unsigned pHeight, BitDepth pBitDepth)
{
	mBufferResponsibility = false;
	mBuffer = 0;
	mBitDepth = pBitDepth;
	mWidth = pWidth;
	mHeight = pHeight;
	mPitch = pWidth;

	switch(mBitDepth)
	{
	case BITDEPTH_8_BIT:
		mPixelSize = 1;
		break;
	case BITDEPTH_15_BIT:
	case BITDEPTH_16_BIT:
		mPixelSize = 2;
		break;
	case BITDEPTH_24_BIT:
		mPixelSize = 3;
		break;
	case BITDEPTH_32_BIT:
		mPixelSize = 4;
		break;
	case BITDEPTH_16_BIT_PER_CHANNEL:
		mPixelSize = 6;
		break;
	case BITDEPTH_32_BIT_PER_CHANNEL:
		mPixelSize = 12;
		break;
	}
}

Canvas::Canvas(const Canvas& pCanvas, bool pCopy)
{
	mBitDepth = pCanvas.GetBitDepth();
	mWidth = pCanvas.GetWidth();
	mHeight = pCanvas.GetHeight();
	mPitch = pCanvas.GetPitch();

	SetPalette(pCanvas.GetPalette());

	if (pCopy == true)
	{
		unsigned lPixelSize;
		mBuffer = CreateBuffer(mPitch, mHeight, pCanvas.GetBitDepth(), lPixelSize);

		if (mBuffer != 0)
		{
			mPixelSize = lPixelSize;
			memcpy(mBuffer, pCanvas.GetBuffer(), mHeight * mPitch * mPixelSize);
			mBufferResponsibility = true;
		}
	}
	else
	{
		mBuffer = pCanvas.GetBuffer();
		mBufferResponsibility = false;
	}
}

Canvas::~Canvas()
{
	if (mBufferResponsibility == true)
	{
		delete[] (char*)mBuffer;
	}
}

void* Canvas::CreateBuffer(unsigned pWidth, unsigned pHeight, BitDepth pBitDepth, unsigned& pPixelSize)
{
	switch(pBitDepth)
	{
	case BITDEPTH_8_BIT:
		pPixelSize = 1;
		return new char[pWidth * pHeight];
		break;
	case BITDEPTH_15_BIT:
	case BITDEPTH_16_BIT:
		pPixelSize = 2;
		return new short[pWidth * pHeight];
		break;
	case BITDEPTH_24_BIT:
		pPixelSize = 3;
		return new char[pWidth * pHeight * 3];
		break;
	case BITDEPTH_32_BIT:
		pPixelSize = 4;
		return new uint32[pWidth * pHeight];
		break;
	case BITDEPTH_16_BIT_PER_CHANNEL:
		pPixelSize = 6;
		return new short[pWidth * pHeight * 3];
		break;
	case BITDEPTH_32_BIT_PER_CHANNEL:
		pPixelSize = 12;
		return new uint32[pWidth * pHeight * 3];
		break;
	}

	return 0;
}


void Canvas::Copy(const Canvas& pCanvas)
{
	Reset(pCanvas.GetWidth(), pCanvas.GetHeight(), pCanvas.GetBitDepth());
	SetPitch(pCanvas.GetPitch());

	unsigned lPixelSize;
	mBuffer = CreateBuffer(mPitch, mHeight, pCanvas.GetBitDepth(), lPixelSize);

	if (mBuffer != 0)
	{
		mPixelSize = lPixelSize;
		memcpy(mBuffer, pCanvas.GetBuffer(), mHeight * mPitch * mPixelSize);
		mBufferResponsibility = true;
	}

	SetPalette(pCanvas.GetPalette());
}

void Canvas::PartialCopy(int pX, int pY, const Canvas& pCanvas)
{
	for (unsigned y = 0; y < pCanvas.GetHeight(); ++y)
	{
		for (unsigned x = 0; x < pCanvas.GetWidth(); ++x)
		{
			Lepra::Color lColor = pCanvas.GetPixelColor(x, y);
			SetPixelColor(pX+x, pY+y, lColor);
		}
	}
}

void Canvas::Reset(unsigned pWidth, unsigned pHeight, BitDepth pBitDepth)
{
	if (mBufferResponsibility == true)
	{
		delete[] (char*)mBuffer;
		mBuffer = 0;
	}
	mBufferResponsibility = false;
	mBuffer = 0;
	mBitDepth = pBitDepth;
	mWidth = pWidth;
	mHeight = pHeight;
	mPitch = pWidth;

	switch(mBitDepth)
	{
	case BITDEPTH_8_BIT:
		mPixelSize = 1;
		break;
	case BITDEPTH_15_BIT:
	case BITDEPTH_16_BIT:
		mPixelSize = 2;
		break;
	case BITDEPTH_24_BIT:
		mPixelSize = 3;
		break;
	case BITDEPTH_32_BIT:
		mPixelSize = 4;
		break;
	case BITDEPTH_16_BIT_PER_CHANNEL:
		mPixelSize = 6;
		break;
	case BITDEPTH_32_BIT_PER_CHANNEL:
		mPixelSize = 12;
		break;
	}
}

void Canvas::Reset(const Canvas& pCanvas)
{
	Reset(pCanvas.GetWidth(), pCanvas.GetHeight(), pCanvas.GetBitDepth());
	SetPitch(pCanvas.GetPitch());
	SetPalette(pCanvas.GetPalette());
}

void Canvas::SetBuffer(void* pBuffer, bool pCopy, bool pTakeOwnership)
{
	if (pCopy == true)
	{
		// Only create a buffer if we don't already have one.
		if (mBufferResponsibility == false)
		{
			unsigned lPixelSize;
			mBuffer = CreateBuffer(mPitch, mHeight, mBitDepth, lPixelSize);

			if (mBuffer != 0)
			{
				mPixelSize = lPixelSize;
				mBufferResponsibility = true;

				memcpy(mBuffer, pBuffer, mPitch * mHeight * mPixelSize);
			}
		}
	}
	else
	{
		if (mBufferResponsibility == true)
		{
			delete[] (char*)mBuffer;
		}

		mBuffer = pBuffer;

		mBufferResponsibility = pTakeOwnership;
	}
}

void Canvas::CreateBuffer()
{
	if (mBufferResponsibility == true)
	{
		delete[] (char*)mBuffer;
		mBufferResponsibility = false;
	}

	unsigned lPixelSize;
	mBuffer = CreateBuffer(mPitch, mHeight, mBitDepth, lPixelSize);

	if (mBuffer != 0)
	{
		mPixelSize = lPixelSize;
		mBufferResponsibility = true;
	}
}

void Canvas::SetPalette(const Color* pPalette)
{
	if (pPalette == 0)
	{
		return;
	}

	for (int i = 0; i < 256; i++)
	{
		mPalette[i].mRed   = pPalette[i].mRed;
		mPalette[i].mGreen = pPalette[i].mGreen;
		mPalette[i].mBlue  = pPalette[i].mBlue;
		mPalette[i].mAlpha = pPalette[i].mAlpha;
	}
}

void Canvas::SetGrayscalePalette()
{
	for (int i = 0; i < 256; i++)
	{
		mPalette[i].mRed   = (uint8)i;
		mPalette[i].mGreen = (uint8)i;
		mPalette[i].mBlue  = (uint8)i;
		mPalette[i].mAlpha = 255;
	}
}

unsigned Canvas::PowerUp(unsigned pValue)
{
	if (pValue == 0)
	{
		return 1;
	}
	unsigned i = ((unsigned)1 << 31);
	while (i > pValue)
	{
		i >>= 1;
	}
	if (i == pValue)
	{
		// The height is already a power of two.
		return pValue;
	}
	else
	{
		// Return the next higher power of two.
		return (i << 1);
	}
}

void Canvas::Crop(int pTopLeftX,
		  int pTopLeftY,
		  int pBottomRightX,
		  int pBottomRightY)
{
	if (pBottomRightX < pTopLeftX)
	{
		int lT = pBottomRightX;
		pBottomRightX = pTopLeftX;
		pTopLeftX = lT;
	}
	if (pBottomRightY < pTopLeftY)
	{
		int lT = pBottomRightY;
		pBottomRightY = pTopLeftY;
		pTopLeftY = lT;
	}

	unsigned lNewWidth = pBottomRightX - pTopLeftX;
	unsigned lNewHeight = pBottomRightY - pTopLeftY;
	unsigned lOldWidth = mWidth;
	unsigned lOldHeight = mHeight;

	unsigned lPixelSize;
	uint8* lBuffer = (uint8*)CreateBuffer(lNewWidth, lNewHeight, mBitDepth, lPixelSize);

	if (lBuffer == 0)
	{
		return;
	}

	// Clear the new buffer.
	if (mBitDepth != BITDEPTH_32_BIT_PER_CHANNEL)
	{
		memset(lBuffer, 0, lNewWidth * lNewHeight * lPixelSize);
	}
	else
	{
		memset(lBuffer, 0, lNewWidth * lNewHeight * 3 * sizeof(float32));
	}

	int lSrcStartX = pTopLeftX;
	int lSrcStartY = pTopLeftY;
	int lDestStartX = 0;
	int lDestStartY = 0;

	int lLoopEndX = pBottomRightX;
	int lLoopEndY = pBottomRightY;

	if (pTopLeftX < 0)
	{
		lSrcStartX = 0;
		lDestStartX = -pTopLeftX;
	}

	if (pBottomRightX > (int)lOldWidth)
	{
		lLoopEndX = lOldWidth;
	}

	if (pTopLeftY < 0)
	{
		lSrcStartY = 0;
		lDestStartY = -pTopLeftY;
	}

	if (pBottomRightY > (int)lOldHeight)
	{
		lLoopEndY = lOldHeight;
	}

	int lLoopWidth  = (lLoopEndX - lSrcStartX) * lPixelSize;
	int lLoopHeight = lLoopEndY - lSrcStartY;

	for (int y = 0; y < lLoopHeight; y++)
	{
		unsigned lSrcYOffset = (lSrcStartY + y) * mPitch * lPixelSize;
		unsigned lDestYOffset = (lDestStartY + y) * lNewWidth * lPixelSize;

		for (int x = 0; x < lLoopWidth; x++)
		{
			lBuffer[lDestYOffset + lDestStartX * lPixelSize + x] = ((uint8*)mBuffer)[lSrcYOffset + lSrcStartX * lPixelSize + x];
		}
	}

	Reset(lNewWidth, lNewHeight, mBitDepth);
	mBuffer = lBuffer;
	mBufferResponsibility = true;
}

void Canvas::FlipHorizontal()
{
	unsigned lPixelSize;
	char* lBuffer = (char*)CreateBuffer(mPitch, mHeight, mBitDepth, lPixelSize);

	if (lBuffer == 0)
	{
		return;
	}

	for (unsigned y = 0; y < mHeight; y++)
	{
		unsigned lYOffset = y * mPitch;

		switch(mBitDepth)
		{
		case BITDEPTH_8_BIT:
			{
				for (unsigned x = 0; x < mWidth; x++)
				{
					lBuffer[lYOffset + x] = ((uint8*)mBuffer)[lYOffset + (mWidth - (x + 1))];
				}
				break;
			}
		case BITDEPTH_15_BIT:
		case BITDEPTH_16_BIT:
			{
				for (unsigned x = 0; x < mWidth; x++)
				{
					((short*)lBuffer)[lYOffset + x] = ((short*)mBuffer)[lYOffset + (mWidth - (x + 1))];
				}
				break;
			}
		case BITDEPTH_24_BIT:
			{
				lYOffset *= 3;
				for (unsigned x = 0; x < mWidth; x++)
				{
					int lSrcOffset = lYOffset + (mWidth - (x + 1)) * 3;
					int lDestOffset = lYOffset + x * 3;
					lBuffer[lDestOffset + 0] = ((uint8*)mBuffer)[lSrcOffset + 0];
					lBuffer[lDestOffset + 1] = ((uint8*)mBuffer)[lSrcOffset + 1];
					lBuffer[lDestOffset + 2] = ((uint8*)mBuffer)[lSrcOffset + 2];
				}
				break;
			}
		case BITDEPTH_32_BIT:
			{
				for (unsigned x = 0; x < mWidth; x++)
				{
					((uint32*)lBuffer)[lYOffset + x] = ((uint32*)mBuffer)[lYOffset + (mWidth - (x + 1))];
				}
				break;
			}
		case BITDEPTH_16_BIT_PER_CHANNEL:
			{
				lYOffset *= 3;

				for (unsigned x = 0; x < mWidth; x++)
				{
					int lSrcOffset = lYOffset + (mWidth - (x + 1)) * 3;
					int lDestOffset = lYOffset + x * 3;
					((uint16*)lBuffer)[lDestOffset + 0] = ((uint16*)mBuffer)[lSrcOffset + 0];
					((uint16*)lBuffer)[lDestOffset + 1] = ((uint16*)mBuffer)[lSrcOffset + 1];
					((uint16*)lBuffer)[lDestOffset + 2] = ((uint16*)mBuffer)[lSrcOffset + 2];
				}
				break;
			}
		case BITDEPTH_32_BIT_PER_CHANNEL:
			{
				lYOffset *= 3;

				for (unsigned x = 0; x < mWidth; x++)
				{
					int lSrcOffset = lYOffset + (mWidth - (x + 1)) * 3;
					int lDestOffset = lYOffset + x * 3;
					((float32*)lBuffer)[lDestOffset + 0] = ((float32*)mBuffer)[lSrcOffset + 0];
					((float32*)lBuffer)[lDestOffset + 1] = ((float32*)mBuffer)[lSrcOffset + 1];
					((float32*)lBuffer)[lDestOffset + 2] = ((float32*)mBuffer)[lSrcOffset + 2];
				}
				break;
			}
		}
	}

	if (mBufferResponsibility == true)
	{
		delete[] (char*)mBuffer;
	}

	mPixelSize = lPixelSize;
	mBuffer = lBuffer;
	mBufferResponsibility = true;
}

void Canvas::FlipVertical()
{
	unsigned lPixelSize;
	char* lBuffer = (char*)CreateBuffer(mPitch, mHeight, mBitDepth, lPixelSize);

	if (lBuffer == 0)
	{
		return;
	}

	for (unsigned y = 0; y < mHeight; y++)
	{
		unsigned lSrcYOffset = (mHeight - (y + 1)) * mPitch;
		unsigned lDestYOffset = y * mPitch;

		switch(mBitDepth)
		{
		case BITDEPTH_8_BIT:
			{
				for (unsigned x = 0; x < mWidth; x++)
				{
					lBuffer[lDestYOffset + x] = ((uint8*)mBuffer)[lSrcYOffset + x];
				}
				break;
			}
		case BITDEPTH_15_BIT:
		case BITDEPTH_16_BIT:
			{
				for (unsigned x = 0; x < mWidth; x++)
				{
					((short*)lBuffer)[lDestYOffset + x] = ((short*)mBuffer)[lSrcYOffset + x];
				}
				break;
			}
		case BITDEPTH_24_BIT:
			{
				for (unsigned x = 0; x < mWidth; x++)
				{
					int lSrcOffset = (lSrcYOffset + x) * 3;
					int lDestOffset = (lDestYOffset + x) * 3;
					lBuffer[lDestOffset + 0] = ((uint8*)mBuffer)[lSrcOffset + 0];
					lBuffer[lDestOffset + 1] = ((uint8*)mBuffer)[lSrcOffset + 1];
					lBuffer[lDestOffset + 2] = ((uint8*)mBuffer)[lSrcOffset + 2];
				}
				break;
			}
		case BITDEPTH_32_BIT:
			{
				for (unsigned x = 0; x < mWidth; x++)
				{
					((uint32*)lBuffer)[lDestYOffset + x] = ((uint32*)mBuffer)[lSrcYOffset + x];
				}
				break;
			}
		case BITDEPTH_16_BIT_PER_CHANNEL:
			{
				for (unsigned x = 0; x < mWidth; x++)
				{
					int lSrcOffset = (lSrcYOffset + x) * 3;
					int lDestOffset = (lDestYOffset + x) * 3;
					((uint16*)lBuffer)[lDestOffset + 0] = ((uint16*)mBuffer)[lSrcOffset + 0];
					((uint16*)lBuffer)[lDestOffset + 1] = ((uint16*)mBuffer)[lSrcOffset + 1];
					((uint16*)lBuffer)[lDestOffset + 2] = ((uint16*)mBuffer)[lSrcOffset + 2];
				}
				break;
			}
		case BITDEPTH_32_BIT_PER_CHANNEL:
			{
				for (unsigned x = 0; x < mWidth; x++)
				{
					int lSrcOffset = (lSrcYOffset + x) * 3;
					int lDestOffset = (lDestYOffset + x) * 3;
					((float32*)lBuffer)[lDestOffset + 0] = ((float32*)mBuffer)[lSrcOffset + 0];
					((float32*)lBuffer)[lDestOffset + 1] = ((float32*)mBuffer)[lSrcOffset + 1];
					((float32*)lBuffer)[lDestOffset + 2] = ((float32*)mBuffer)[lSrcOffset + 2];
				}
				break;
			}
		}
	}

	if (mBufferResponsibility == true)
	{
		delete[] (char*)mBuffer;
	}

	mPixelSize = lPixelSize;
	mBuffer = lBuffer;
	mBufferResponsibility = true;
}

void Canvas::Rotate90DegClockWise(int pSteps)
{
	pSteps &= 3;

	if (pSteps == 0)
	{
		return;
	}

	unsigned lPixelSize;
	char* lBuffer;

	unsigned lDestWidth = mWidth;
	unsigned lDestHeight = mHeight;
	unsigned lDestPitch = mPitch;

	if ((pSteps & 1) != 0)
	{
		lDestWidth = mHeight;
		lDestHeight = mPitch;
		lDestPitch = mHeight;
	}

	lBuffer = (char*)CreateBuffer(lDestPitch, lDestHeight, mBitDepth, lPixelSize);

	if (lBuffer == 0)
	{
		return;
	}

	switch(mBitDepth)
	{
	case BITDEPTH_8_BIT:
		switch(pSteps)
		{
		case 1:
			{
				for (unsigned y = 0; y < lDestHeight; y++)
				{
					unsigned lDestYOffset = y * lDestPitch;

					for (unsigned x = 0; x < lDestWidth; x++)
					{
						lBuffer[lDestYOffset + x] = ((uint8*)mBuffer)[(mHeight - (x + 1)) * mPitch + y];
					}
				}
				break;
			}
		case 2:
			{
				for (unsigned y = 0; y < lDestHeight; y++)
				{
					unsigned lDestYOffset = y * lDestPitch;

					for (unsigned x = 0; x < lDestWidth; x++)
					{
						lBuffer[lDestYOffset + x] = ((uint8*)mBuffer)[(mHeight - (y + 1)) * mPitch + (mWidth - (x + 1))];
					}
				}
				break;
			}
		case 3:
			{
				for (unsigned y = 0; y < lDestHeight; y++)
				{
					unsigned lDestYOffset = y * lDestPitch;

					for (unsigned x = 0; x < lDestWidth; x++)
					{
						lBuffer[lDestYOffset + x] = ((uint8*)mBuffer)[x * mPitch + (mWidth - (y + 1))];
					}
				}
				break;
			}
		};
		break;
	case BITDEPTH_15_BIT:
	case BITDEPTH_16_BIT:
		switch(pSteps)
		{
		case 1:
			{
				for (unsigned y = 0; y < lDestHeight; y++)
				{
					unsigned lDestYOffset = y * lDestPitch;

					for (unsigned x = 0; x < lDestWidth; x++)
					{
						((short*)lBuffer)[lDestYOffset + x] = ((short*)mBuffer)[(mHeight - (x + 1)) * mPitch + y];
					}
				}
				break;
			}
		case 2:
			{
				for (unsigned y = 0; y < lDestHeight; y++)
				{
					unsigned lDestYOffset = y * lDestPitch;

					for (unsigned x = 0; x < lDestWidth; x++)
					{
						((short*)lBuffer)[lDestYOffset + x] = ((short*)mBuffer)[(mHeight - (y + 1)) * mPitch + (mWidth - (x + 1))];
					}
				}
				break;
			}
		case 3:
			{
				for (unsigned y = 0; y < lDestHeight; y++)
				{
					unsigned lDestYOffset = y * lDestPitch;

					for (unsigned x = 0; x < lDestWidth; x++)
					{
						((short*)lBuffer)[lDestYOffset + x] = ((short*)mBuffer)[x * mPitch + (mWidth - (y + 1))];
					}
				}
				break;
			}
		};
		break;
	case BITDEPTH_24_BIT:
		switch(pSteps)
		{
		case 1:
			{
				for (unsigned y = 0; y < lDestHeight; y++)
				{
					unsigned lDestYOffset = y * lDestPitch;

					for (unsigned x = 0; x < lDestWidth; x++)
					{
						int lDestOffset = (lDestYOffset + x) * 3;
						int lSrcOffset = ((mHeight - (x + 1)) * mPitch + y) * 3;
						lBuffer[lDestOffset + 0] = ((uint8*)mBuffer)[lSrcOffset + 0];
						lBuffer[lDestOffset + 1] = ((uint8*)mBuffer)[lSrcOffset + 1];
						lBuffer[lDestOffset + 2] = ((uint8*)mBuffer)[lSrcOffset + 2];
					}
				}
				break;
			}
		case 2:
			{
				for (unsigned y = 0; y < lDestHeight; y++)
				{
					unsigned lDestYOffset = y * lDestPitch;

					for (unsigned x = 0; x < lDestWidth; x++)
					{
						int lDestOffset = (lDestYOffset + x) * 3;
						int lSrcOffset = ((mHeight - (y + 1)) * mPitch + (mWidth - (x + 1))) * 3;
						lBuffer[lDestOffset + 0] = ((uint8*)mBuffer)[lSrcOffset + 0];
						lBuffer[lDestOffset + 1] = ((uint8*)mBuffer)[lSrcOffset + 1];
						lBuffer[lDestOffset + 2] = ((uint8*)mBuffer)[lSrcOffset + 2];
					}
				}
				break;
			}
		case 3:
			{
				for (unsigned y = 0; y < lDestHeight; y++)
				{
					unsigned lDestYOffset = y * lDestPitch;

					for (unsigned x = 0; x < lDestWidth; x++)
					{
						int lDestOffset = (lDestYOffset + x) * 3;
						int lSrcOffset = (x * mPitch + (mWidth - (y + 1))) * 3;
						lBuffer[lDestOffset + 0] = ((uint8*)mBuffer)[lSrcOffset + 0];
						lBuffer[lDestOffset + 1] = ((uint8*)mBuffer)[lSrcOffset + 1];
						lBuffer[lDestOffset + 2] = ((uint8*)mBuffer)[lSrcOffset + 2];
					}
				}
				break;
			}
		};
		break;
	case BITDEPTH_32_BIT:
		switch(pSteps)
		{
		case 1:
			{
				for (unsigned y = 0; y < lDestHeight; y++)
				{
					unsigned lDestYOffset = y * lDestPitch;

					for (unsigned x = 0; x < lDestWidth; x++)
					{
						((uint32*)lBuffer)[lDestYOffset + x] = ((uint32*)mBuffer)[(mHeight - (x + 1)) * mPitch + y];
					}
				}
				break;
			}
		case 2:
			{
				for (unsigned y = 0; y < lDestHeight; y++)
				{
					unsigned lDestYOffset = y * lDestPitch;

					for (unsigned x = 0; x < lDestWidth; x++)
					{
						((uint32*)lBuffer)[lDestYOffset + x] = ((uint32*)mBuffer)[(mHeight - (y + 1)) * mPitch + (mWidth - (x + 1))];
					}
				}
				break;
			}
		case 3:
			{
				for (unsigned y = 0; y < lDestHeight; y++)
				{
					unsigned lDestYOffset = y * lDestPitch;

					for (unsigned x = 0; x < lDestWidth; x++)
					{
						((uint32*)lBuffer)[lDestYOffset + x] = ((uint32*)mBuffer)[x * mPitch + (mWidth - (y + 1))];
					}
				}
				break;
			}
		};
		break;
	case BITDEPTH_16_BIT_PER_CHANNEL:
		switch(pSteps)
		{
		case 1:
			{
				for (unsigned y = 0; y < lDestHeight; y++)
				{
					unsigned lDestYOffset = y * lDestPitch;

					for (unsigned x = 0; x < lDestWidth; x++)
					{
						int lDestOffset = (lDestYOffset + x) * 3;
						int lSrcOffset = ((mHeight - (x + 1)) * mPitch + y) * 3;
						((uint16*)lBuffer)[lDestOffset + 0] = ((uint16*)mBuffer)[lSrcOffset + 0];
						((uint16*)lBuffer)[lDestOffset + 1] = ((uint16*)mBuffer)[lSrcOffset + 1];
						((uint16*)lBuffer)[lDestOffset + 2] = ((uint16*)mBuffer)[lSrcOffset + 2];
					}
				}
				break;
			}
		case 2:
			{
				for (unsigned y = 0; y < lDestHeight; y++)
				{
					unsigned lDestYOffset = y * lDestPitch;

					for (unsigned x = 0; x < lDestWidth; x++)
					{
						int lDestOffset = (lDestYOffset + x) * 3;
						int lSrcOffset = ((mHeight - (y + 1)) * mPitch + (mWidth - (x + 1))) * 3;
						((uint16*)lBuffer)[lDestOffset + 0] = ((uint16*)mBuffer)[lSrcOffset + 0];
						((uint16*)lBuffer)[lDestOffset + 1] = ((uint16*)mBuffer)[lSrcOffset + 1];
						((uint16*)lBuffer)[lDestOffset + 2] = ((uint16*)mBuffer)[lSrcOffset + 2];
					}
				}
				break;
			}
		case 3:
			{
				for (unsigned y = 0; y < lDestHeight; y++)
				{
					unsigned lDestYOffset = y * lDestPitch;

					for (unsigned x = 0; x < lDestWidth; x++)
					{
						int lDestOffset = (lDestYOffset + x) * 3;
						int lSrcOffset = (x * mPitch + (mWidth - (y + 1))) * 3;
						((uint16*)lBuffer)[lDestOffset + 0] = ((uint16*)mBuffer)[lSrcOffset + 0];
						((uint16*)lBuffer)[lDestOffset + 1] = ((uint16*)mBuffer)[lSrcOffset + 1];
						((uint16*)lBuffer)[lDestOffset + 2] = ((uint16*)mBuffer)[lSrcOffset + 2];
					}
				}
				break;
			}
		};
		break;
	case BITDEPTH_32_BIT_PER_CHANNEL:
		switch(pSteps)
		{
		case 1:
			{
				for (unsigned y = 0; y < lDestHeight; y++)
				{
					unsigned lDestYOffset = y * lDestPitch;

					for (unsigned x = 0; x < lDestWidth; x++)
					{
						int lDestOffset = (lDestYOffset + x) * 3;
						int lSrcOffset = ((mHeight - (x + 1)) * mPitch + y) * 3;
						((float32*)lBuffer)[lDestOffset + 0] = ((float32*)mBuffer)[lSrcOffset + 0];
						((float32*)lBuffer)[lDestOffset + 1] = ((float32*)mBuffer)[lSrcOffset + 1];
						((float32*)lBuffer)[lDestOffset + 2] = ((float32*)mBuffer)[lSrcOffset + 2];
					}
				}
				break;
			}
		case 2:
			{
				for (unsigned y = 0; y < lDestHeight; y++)
				{
					unsigned lDestYOffset = y * lDestPitch;

					for (unsigned x = 0; x < lDestWidth; x++)
					{
						int lDestOffset = (lDestYOffset + x) * 3;
						int lSrcOffset = ((mHeight - (y + 1)) * mPitch + (mWidth - (x + 1))) * 3;
						((float32*)lBuffer)[lDestOffset + 0] = ((float32*)mBuffer)[lSrcOffset + 0];
						((float32*)lBuffer)[lDestOffset + 1] = ((float32*)mBuffer)[lSrcOffset + 1];
						((float32*)lBuffer)[lDestOffset + 2] = ((float32*)mBuffer)[lSrcOffset + 2];
					}
				}
				break;
			}
		case 3:
			{
				for (unsigned y = 0; y < lDestHeight; y++)
				{
					unsigned lDestYOffset = y * lDestPitch;

					for (unsigned x = 0; x < lDestWidth; x++)
					{
						int lDestOffset = (lDestYOffset + x) * 3;
						int lSrcOffset = (x * mPitch + (mWidth - (y + 1))) * 3;
						((float32*)lBuffer)[lDestOffset + 0] = ((float32*)mBuffer)[lSrcOffset + 0];
						((float32*)lBuffer)[lDestOffset + 1] = ((float32*)mBuffer)[lSrcOffset + 1];
						((float32*)lBuffer)[lDestOffset + 2] = ((float32*)mBuffer)[lSrcOffset + 2];
					}
				}
				break;
			}
		};
		break;
	}

	if (mBufferResponsibility == true)
	{
		delete[] (char*)mBuffer;
	}

	mPixelSize = lPixelSize;
	mBuffer = lBuffer;
	mBufferResponsibility = true;

	mWidth = lDestWidth;
	mHeight = lDestHeight;
	mPitch = lDestPitch;
}

void Canvas::InitSamplingGrid()
{
	if (smResizeDataInitialized == false)
	{
		smResizeDataInitialized = true;

		//
		// Setup the sample grid and its weights.
		//
		int i;
		for (i = 0; i < smSamplingGridSize; i++)
		{
			smSamplingGridCoordLookUp[i] = ((float32)(i - smSamplingGridSize / 2) + 0.5f) * smSamplingGridPointSpace;
		}

		const float32 lE = 2.718281828f;
		const float32 lDiv = smResizeSharpness * (float32)sqrt(2.0f * PIF);
		const float32 lS2 = smResizeSharpness * smResizeSharpness;

		for (int y = 0; y < smSamplingGridSize; y++)
		{
			float32 lGridPointY = (float32)smSamplingGridCoordLookUp[y];

			for (int x = 0; x < smSamplingGridSize; x++)
			{
				float32 lGridPointX = smSamplingGridCoordLookUp[x];

				float32 lDist = (float32)sqrt(lGridPointX * lGridPointX + lGridPointY * lGridPointY);

				float32 lWeight = (float32)pow(lE, -(lDist * lDist) / lS2) / lDiv;
				smSamplingGridWeight[y * smSamplingGridSize + x] = lWeight;
			}
		}
	}
}

void Canvas::Resize(unsigned pNewWidth, unsigned pNewHeight, ResizeHint pResizeHint)
{
	if (mWidth == pNewWidth && mHeight == pNewHeight)
		return;

	switch(mBitDepth)
	{
	case BITDEPTH_8_BIT:
		switch(pResizeHint)
		{
		case RESIZE_NICEST:
			Resize8BitSmooth(pNewWidth, pNewHeight);
			break;
		case RESIZE_FAST:
		default:
			Resize8BitFast(pNewWidth, pNewHeight);
			break;
		}
		break;
	case BITDEPTH_15_BIT:
		switch(pResizeHint)
			{
			case RESIZE_NICEST:
			Resize15BitSmooth(pNewWidth, pNewHeight);
			break;
		case RESIZE_FAST:
			default:
			Resize16BitFast(pNewWidth, pNewHeight);
			break;
			}
		break;
	case BITDEPTH_16_BIT:
		switch(pResizeHint)
			{
			case RESIZE_NICEST:
			Resize16BitSmooth(pNewWidth, pNewHeight);
			break;
		case RESIZE_FAST:
			default:
			Resize16BitFast(pNewWidth, pNewHeight);
			break;
			}
		break;
	case BITDEPTH_24_BIT:
		switch(pResizeHint)
			{
			case RESIZE_NICEST:
			Resize24BitSmooth(pNewWidth, pNewHeight);
			break;
		case RESIZE_FAST:
			default:
			Resize24BitFast(pNewWidth, pNewHeight);
			break;
			}
		break;
	case BITDEPTH_32_BIT:
		switch(pResizeHint)
			{
			case RESIZE_NICEST:
			Resize32BitSmooth(pNewWidth, pNewHeight);
			break;
		case RESIZE_FAST:
			default:
			Resize32BitFast(pNewWidth, pNewHeight);
			break;
			}
		break;
	case BITDEPTH_16_BIT_PER_CHANNEL:
		switch(pResizeHint)
			{
			case RESIZE_NICEST:
			Resize48BitSmooth(pNewWidth, pNewHeight);
			break;
		case RESIZE_FAST:
			default:
			Resize48BitFast(pNewWidth, pNewHeight);
			break;
			}
		break;
	case BITDEPTH_32_BIT_PER_CHANNEL:
		switch(pResizeHint)
			{
			case RESIZE_NICEST:
			Resize96BitSmooth(pNewWidth, pNewHeight);
			break;
		case RESIZE_FAST:
			default:
			Resize96BitFast(pNewWidth, pNewHeight);
			break;
			}
		break;
	}
}

void Canvas::Resize8BitFast(unsigned pNewWidth, unsigned pNewHeight)
{
	uint8* lBuffer = new uint8[pNewWidth * pNewHeight];

	int lSrcX = 0;
	int lSrcY = 0;
	int lSrcXStep = (mWidth << 16) / pNewWidth;
	int lSrcYStep = (mHeight << 16) / pNewHeight;

	for (unsigned y = 0; y < pNewHeight; y++)
	{
		int lSrcYOffset = (lSrcY >> 16) * mPitch;
		int lDestYOffset = y * pNewWidth;

		lSrcX = 0;

		for (unsigned x = 0; x < pNewWidth; x++)
		{
			lBuffer[lDestYOffset + x] = ((uint8*)mBuffer)[lSrcYOffset + (lSrcX >> 16)];
			lSrcX += lSrcXStep;
		}

		lSrcY += lSrcYStep;
	}

	Reset(pNewWidth, pNewHeight, mBitDepth);
	mBuffer = lBuffer;
	mBufferResponsibility = true;
}

void Canvas::Resize16BitFast(unsigned pNewWidth, unsigned pNewHeight)
{
	uint16* lBuffer = new uint16[pNewWidth * pNewHeight];

	int lSrcX = 0;
	int lSrcY = 0;
	int lSrcXStep = (mWidth << 16) / pNewWidth;
	int lSrcYStep = (mHeight << 16) / pNewHeight;

	for (unsigned y = 0; y < pNewHeight; y++)
	{
		int lSrcYOffset = (lSrcY >> 16) * mPitch;
		int lDestYOffset = y * pNewWidth;

		lSrcX = 0;

		for (unsigned x = 0; x < pNewWidth; x++)
		{
			lBuffer[lDestYOffset + x] = ((uint16*)mBuffer)[lSrcYOffset + (lSrcX >> 16)];
			lSrcX += lSrcXStep;
		}

		lSrcY += lSrcYStep;
	}

	Reset(pNewWidth, pNewHeight, mBitDepth);
	mBuffer = lBuffer;
	mBufferResponsibility = true;
}

void Canvas::Resize24BitFast(unsigned pNewWidth, unsigned pNewHeight)
{
	uint8* lBuffer = new uint8[pNewWidth * pNewHeight * 3];

	int lSrcX = 0;
	int lSrcY = 0;
	int lSrcXStep = (mWidth << 16) / pNewWidth;
	int lSrcYStep = (mHeight << 16) / pNewHeight;

	for (unsigned y = 0; y < pNewHeight; y++)
	{
		int lSrcYOffset = (lSrcY >> 16) * mPitch;
		int lDestYOffset = y * pNewWidth;

		lSrcX = 0;

		for (unsigned x = 0; x < pNewWidth; x++)
		{
			int lDestOffset = (lDestYOffset + x) * 3;
			int lSrcOffset = (lSrcYOffset + (lSrcX >> 16)) * 3;

			lBuffer[lDestOffset + 0] = ((uint8*)mBuffer)[lSrcOffset + 0];
			lBuffer[lDestOffset + 1] = ((uint8*)mBuffer)[lSrcOffset + 1];
			lBuffer[lDestOffset + 2] = ((uint8*)mBuffer)[lSrcOffset + 2];

			lSrcX += lSrcXStep;
		}

		lSrcY += lSrcYStep;
	}

	Reset(pNewWidth, pNewHeight, mBitDepth);
	mBuffer = lBuffer;
	mBufferResponsibility = true;
}

void Canvas::Resize32BitFast(unsigned pNewWidth, unsigned pNewHeight)
{
	uint32* lBuffer = new uint32[pNewWidth * pNewHeight];

	int lSrcX = 0;
	int lSrcY = 0;
	int lSrcXStep = (mWidth << 16) / pNewWidth;
	int lSrcYStep = (mHeight << 16) / pNewHeight;

	for (unsigned y = 0; y < pNewHeight; y++)
	{
		int lSrcYOffset = (lSrcY >> 16) * mPitch;
		int lDestYOffset = y * pNewWidth;

		lSrcX = 0;

		for (unsigned x = 0; x < pNewWidth; x++)
		{
			lBuffer[lDestYOffset + x] = ((uint32*)mBuffer)[lSrcYOffset + (lSrcX >> 16)];
			lSrcX += lSrcXStep;
		}

		lSrcY += lSrcYStep;
	}

	Reset(pNewWidth, pNewHeight, mBitDepth);
	mBuffer = lBuffer;
	mBufferResponsibility = true;
}

void Canvas::Resize48BitFast(unsigned pNewWidth, unsigned pNewHeight)
{
	uint16* lBuffer = new uint16[pNewWidth * pNewHeight * 3];

	int lSrcX = 0;
	int lSrcY = 0;
	int lSrcXStep = (mWidth << 16) / pNewWidth;
	int lSrcYStep = (mHeight << 16) / pNewHeight;

	for (unsigned y = 0; y < pNewHeight; y++)
	{
		int lSrcYOffset = (lSrcY >> 16) * mPitch;
		int lDestYOffset = y * pNewWidth;

		lSrcX = 0;

		for (unsigned x = 0; x < pNewWidth; x++)
		{
			int lDestOffset = (lDestYOffset + x) * 3;
			int lSrcOffset = (lSrcYOffset + (lSrcX >> 16)) * 3;

			lBuffer[lDestOffset + 0] = ((uint16*)mBuffer)[lSrcOffset + 0];
			lBuffer[lDestOffset + 1] = ((uint16*)mBuffer)[lSrcOffset + 1];
			lBuffer[lDestOffset + 2] = ((uint16*)mBuffer)[lSrcOffset + 2];

			lSrcX += lSrcXStep;
		}

		lSrcY += lSrcYStep;
	}

	Reset(pNewWidth, pNewHeight, mBitDepth);
	mBuffer = lBuffer;
	mBufferResponsibility = true;
}

void Canvas::Resize96BitFast(unsigned pNewWidth, unsigned pNewHeight)
{
	float32* lBuffer = new float32[pNewWidth * pNewHeight * 3];

	int lSrcX = 0;
	int lSrcY = 0;
	int lSrcXStep = (mWidth << 16) / pNewWidth;
	int lSrcYStep = (mHeight << 16) / pNewHeight;

	for (unsigned y = 0; y < pNewHeight; y++)
	{
		int lSrcYOffset = (lSrcY >> 16) * mPitch;
		int lDestYOffset = y * pNewWidth;

		lSrcX = 0;

		for (unsigned x = 0; x < pNewWidth; x++)
		{
			int lDestOffset = (lDestYOffset + x) * 3;
			int lSrcOffset = (lSrcYOffset + (lSrcX >> 16)) * 3;

			lBuffer[lDestOffset + 0] = ((float32*)mBuffer)[lSrcOffset + 0];
			lBuffer[lDestOffset + 1] = ((float32*)mBuffer)[lSrcOffset + 1];
			lBuffer[lDestOffset + 2] = ((float32*)mBuffer)[lSrcOffset + 2];

			lSrcX += lSrcXStep;
		}

		lSrcY += lSrcYStep;
	}

	Reset(pNewWidth, pNewHeight, mBitDepth);
	mBuffer = lBuffer;
	mBufferResponsibility = true;
}

void Canvas::Resize8BitSmooth(unsigned pNewWidth, unsigned pNewHeight)
{
	// OBS! This function handles the bitmap as a grayscale bitmap!

	// Create the new buffer...
	uint8* lBuffer = new uint8[pNewWidth * pNewHeight];

	float32 lScaleX = (float32)mWidth / (float32)pNewWidth;
	float32 lScaleY = (float32)mHeight / (float32)pNewHeight;

	// And here is the main loop.

	int lDestOffset = 0;
	for (int y = 0; y < (int)pNewHeight; y++)
	{
		// Precalc all possible V-coordinates.
		float32 lSampleY = (float32)y + 0.5f;
		int lVLookup[smSamplingGridSize];
		const int lBufferSize = (int)(mHeight * mPitch);
		for (int i = 0; i < smSamplingGridSize; i++)
		{
			lVLookup[i] = (int)((smSamplingGridCoordLookUp[i] + lSampleY) * lScaleY) * mPitch;
		}

		for (int x = 0; x < (int)pNewWidth; x++)
		{
			float32 lColor = 0.0f;

			// Precalc all possible U-coordinates.
			float32 lSampleX = (float32)x + 0.5f;
			int lULookup[smSamplingGridSize];
			for (int i = 0; i < smSamplingGridSize; i++)
			{
				lULookup[i] = (int)((smSamplingGridCoordLookUp[i] + lSampleX) * lScaleX);
			}

			// Time to take some samples.
			float32 lWeightSum = 0.0f;
			float32* lSGW = smSamplingGridWeight;

			for (int lSGY = 0; lSGY < smSamplingGridSize; lSGY++)
			{
				int lVOffset = lVLookup[lSGY];

				if (lVOffset >= 0 && lVOffset < lBufferSize)
				{
					for (int lSGX = 0; lSGX < smSamplingGridSize; lSGX++)
					{
						int lU = lULookup[lSGX];

						if (lU >= 0 && lU < (int)mWidth)
						{
							uint8 lColor = ((uint8*)mBuffer)[lVOffset + lU];

							lColor += GammaLookup::GammaToLinearFloat(lColor) * (*lSGW);

							lWeightSum += (*lSGW);
						}

						lSGW++;
					}
				}
			}

			// Convert back to normal colors
			float32 lOneOverWeightSum = 1.0f / lWeightSum;
			lBuffer[lDestOffset++] = GammaLookup::LinearToGamma((uint16)floor(lColor * lOneOverWeightSum));
		}
	}

	Reset(pNewWidth, pNewHeight, mBitDepth);
	mBuffer = lBuffer;
	mBufferResponsibility = true;
}

void Canvas::Resize15BitSmooth(unsigned pNewWidth, unsigned pNewHeight)
{
	// Create the new buffer...
	uint16* lBuffer = new uint16[pNewWidth * pNewHeight];

	float32 lScaleX = (float32)mWidth / (float32)pNewWidth;
	float32 lScaleY = (float32)mHeight / (float32)pNewHeight;

	// And here is the main loop.

	int lDestOffset = 0;
	for (int y = 0; y < (int)pNewHeight; y++)
	{
		// Precalc all possible V-coordinates.
		float32 lSampleY = (float32)y + 0.5f;
		int lVLookup[smSamplingGridSize];
		const int lBufferSize = (int)(mHeight * mPitch);
		for (int i = 0; i < smSamplingGridSize; i++)
		{
			lVLookup[i] = (int)((smSamplingGridCoordLookUp[i] + lSampleY) * lScaleY) * mPitch;
		}

		for (int x = 0; x < (int)pNewWidth; x++)
		{
			float32 lR = 0.0f;
			float32 lG = 0.0f;
			float32 lB = 0.0f;

			// Precalc all possible U-coordinates.
			float32 lSampleX = (float32)x + 0.5f;
			int lULookup[smSamplingGridSize];
			for (int i = 0; i < smSamplingGridSize; i++)
			{
				lULookup[i] = (int)((smSamplingGridCoordLookUp[i] + lSampleX) * lScaleX);
			}

			// Time to take some samples.
			float32 lWeightSum = 0.0f;
			float32* lSGW = smSamplingGridWeight;

			for (int lSGY = 0; lSGY < smSamplingGridSize; lSGY++)
			{
				int lVOffset = lVLookup[lSGY];

				if (lVOffset >= 0 && lVOffset < lBufferSize)
				{
					for (int lSGX = 0; lSGX < smSamplingGridSize; lSGX++)
					{
						int lU = lULookup[lSGX];

						if (lU >= 0 && lU < (int)mWidth)
						{
							unsigned lColor = ((uint16*)mBuffer)[lVOffset + lU];

							lR += GammaLookup::GammaToLinearFloat((uint8)((lColor >> 10) & 31) << 3) * (*lSGW);
							lG += GammaLookup::GammaToLinearFloat((uint8)((lColor >> 5) & 31) << 3)  * (*lSGW);
							lB += GammaLookup::GammaToLinearFloat((uint8)(lColor & 31) << 3) * (*lSGW);

							lWeightSum += (*lSGW);
						}

						lSGW++;
					}
				}
			}

			// Convert back to normal colors
			float32 lOneOverWeightSum = 1.0f / lWeightSum;
			uint16 lR16 = GammaLookup::LinearToGamma((uint16)floor(lR * lOneOverWeightSum));
			uint16 lG16 = GammaLookup::LinearToGamma((uint16)floor(lG * lOneOverWeightSum));
			uint16 lB16 = GammaLookup::LinearToGamma((uint16)floor(lB * lOneOverWeightSum));

			lBuffer[lDestOffset++] = ((lR16 >> 3) << 10) | ((lG16 >> 3) << 5) | (lB16 >> 3);
		}
	}

	Reset(pNewWidth, pNewHeight, mBitDepth);
	mBuffer = lBuffer;
	mBufferResponsibility = true;
}


void Canvas::Resize16BitSmooth(unsigned pNewWidth, unsigned pNewHeight)
{
	// Create the new buffer...
	uint16* lBuffer = new uint16[pNewWidth * pNewHeight];

	float32 lScaleX = (float32)mWidth / (float32)pNewWidth;
	float32 lScaleY = (float32)mHeight / (float32)pNewHeight;

	// And here is the main loop.

	int lDestOffset = 0;
	for (int y = 0; y < (int)pNewHeight; y++)
	{
		// Precalc all possible V-coordinates.
		float32 lSampleY = (float32)y + 0.5f;
		int lVLookup[smSamplingGridSize];
		const int lBufferSize = (int)(mHeight * mPitch);
		for (int i = 0; i < smSamplingGridSize; i++)
		{
			lVLookup[i] = (int)((smSamplingGridCoordLookUp[i] + lSampleY) * lScaleY) * mPitch;
		}

		for (int x = 0; x < (int)pNewWidth; x++)
		{
			float32 lR = 0.0f;
			float32 lG = 0.0f;
			float32 lB = 0.0f;

			// Precalc all possible U-coordinates.
			float32 lSampleX = (float32)x + 0.5f;
			int lULookup[smSamplingGridSize];
			for (int i = 0; i < smSamplingGridSize; i++)
			{
				lULookup[i] = (int)((smSamplingGridCoordLookUp[i] + lSampleX) * lScaleX);
			}

			// Time to take some samples.
			float32 lWeightSum = 0.0f;
			float32* lSGW = smSamplingGridWeight;

			for (int lSGY = 0; lSGY < smSamplingGridSize; lSGY++)
			{
				int lVOffset = lVLookup[lSGY];

				if (lVOffset >= 0 && lVOffset < lBufferSize)
				{
					for (int lSGX = 0; lSGX < smSamplingGridSize; lSGX++)
					{
						int lU = lULookup[lSGX];

						if (lU >= 0 && lU < (int)mWidth)
						{
							unsigned lColor = ((uint16*)mBuffer)[lVOffset + lU];

							lR += GammaLookup::GammaToLinearFloat((uint8)((lColor >> 11) & 31) << 3) * (*lSGW);
							lG += GammaLookup::GammaToLinearFloat((uint8)((lColor >> 5) & 63) << 2)  * (*lSGW);
							lB += GammaLookup::GammaToLinearFloat((uint8)(lColor & 31) << 3) * (*lSGW);

							lWeightSum += (*lSGW);
						}

						lSGW++;
					}
				}
			}

			// Convert back to normal colors
			float32 lOneOverWeightSum = 1.0f / lWeightSum;
			uint16 lR16 = GammaLookup::LinearToGamma((uint16)floor(lR * lOneOverWeightSum));
			uint16 lG16 = GammaLookup::LinearToGamma((uint16)floor(lG * lOneOverWeightSum));
			uint16 lB16 = GammaLookup::LinearToGamma((uint16)floor(lB * lOneOverWeightSum));

			lBuffer[lDestOffset++] = ((lR16 >> 3) << 11) | ((lG16 >> 2) << 5) | (lB16 >> 3);
		}
	}

	Reset(pNewWidth, pNewHeight, mBitDepth);
	mBuffer = lBuffer;
	mBufferResponsibility = true;
}

void Canvas::Resize24BitSmooth(unsigned pNewWidth, unsigned pNewHeight)
{
	// Create the new buffer...
	uint8* lBuffer = new uint8[pNewWidth * pNewHeight * 3];

	float32 lScaleX = (float32)mWidth / (float32)pNewWidth;
	float32 lScaleY = (float32)mHeight / (float32)pNewHeight;

	// And here is the main loop.

	int lDestOffset = 0;
	for (int y = 0; y < (int)pNewHeight; y++)
	{
		// Precalc all possible V-coordinates.
		float32 lSampleY = (float32)y + 0.5f;
		int lVLookup[smSamplingGridSize];
		const int lBufferSize = (int)(mHeight * mPitch) * 3;
		for (int i = 0; i < smSamplingGridSize; i++)
		{
			lVLookup[i] = (int)((smSamplingGridCoordLookUp[i] + lSampleY) * lScaleY) * mPitch * 3;
		}

		for (int x = 0; x < (int)pNewWidth; x++)
		{
			float32 lR = 0.0f;
			float32 lG = 0.0f;
			float32 lB = 0.0f;

			// Precalc all possible U-coordinates.
			float32 lSampleX = (float32)x + 0.5f;
			int lULookup[smSamplingGridSize];
			for (int i = 0; i < smSamplingGridSize; i++)
			{
				lULookup[i] = (int)((smSamplingGridCoordLookUp[i] + lSampleX) * lScaleX);
			}

			// Time to take some samples.
			float32 lWeightSum = 0.0f;
			float32* lSGW = smSamplingGridWeight;

			for (int lSGY = 0; lSGY < smSamplingGridSize; lSGY++)
			{
				int lVOffset = lVLookup[lSGY];

				if (lVOffset >= 0 && lVOffset < lBufferSize)
				{
					for (int lSGX = 0; lSGX < smSamplingGridSize; lSGX++)
					{
						int lU = lULookup[lSGX];

						if (lU >= 0 && lU < (int)mWidth)
						{
							uint8* lColor = ((uint8*)mBuffer) + lVOffset + lU * 3;

							lB += GammaLookup::GammaToLinearFloat(lColor[0]) * (*lSGW);
							lG += GammaLookup::GammaToLinearFloat(lColor[1]) * (*lSGW);
							lR += GammaLookup::GammaToLinearFloat(lColor[2]) * (*lSGW);

							lWeightSum += (*lSGW);
						}

						lSGW++;
					}
				}
			}

			// Convert back to normal colors
			float32 lOneOverWeightSum = 1.0f / lWeightSum;
			uint8 lR8 = GammaLookup::LinearToGamma((uint16)floor(lR * lOneOverWeightSum));
			uint8 lG8 = GammaLookup::LinearToGamma((uint16)floor(lG * lOneOverWeightSum));
			uint8 lB8 = GammaLookup::LinearToGamma((uint16)floor(lB * lOneOverWeightSum));

			lBuffer[lDestOffset + 0] = lB8;
			lBuffer[lDestOffset + 1] = lG8;
			lBuffer[lDestOffset + 2] = lR8;

			lDestOffset += 3;
		}
	}

	Reset(pNewWidth, pNewHeight, mBitDepth);
	mBuffer = lBuffer;
	mBufferResponsibility = true;
}

void Canvas::Resize32BitSmooth(unsigned pNewWidth, unsigned pNewHeight)
{
	// Create the new buffer...
	uint32* lBuffer = new uint32[pNewWidth * pNewHeight];

	float32 lScaleX = (float32)mWidth / (float32)pNewWidth;
	float32 lScaleY = (float32)mHeight / (float32)pNewHeight;

	// And here is the main loop.

	int lDestOffset = 0;
	for (int y = 0; y < (int)pNewHeight; y++)
	{
		// Precalc all possible V-coordinates.
		float32 lSampleY = (float32)y + 0.5f;
		int lVLookup[smSamplingGridSize];
		const int lBufferSize = (int)(mHeight * mPitch);
		for (int i = 0; i < smSamplingGridSize; i++)
		{
			lVLookup[i] = (int)((smSamplingGridCoordLookUp[i] + lSampleY) * lScaleY) * mPitch;
		}

		for (int x = 0; x < (int)pNewWidth; x++)
		{

			float32 lR = 0.0f;
			float32 lG = 0.0f;
			float32 lB = 0.0f;
			float32 lA = 0.0f;

			// Precalc all possible U-coordinates.
			float32 lSampleX = (float32)x + 0.5f;
			int lULookup[smSamplingGridSize];
			for (int i = 0; i < smSamplingGridSize; i++)
			{
				lULookup[i] = (int)((smSamplingGridCoordLookUp[i] + lSampleX) * lScaleX);
			}

			// Time to take some samples.
			float32 lWeightSum = 0.0f;
			float32* lSGW = smSamplingGridWeight;

			for (int lSGY = 0; lSGY < smSamplingGridSize; lSGY++)
			{
				int lVOffset = lVLookup[lSGY];

				if (lVOffset >= 0 && lVOffset < lBufferSize)
				{
					for (int lSGX = 0; lSGX < smSamplingGridSize; lSGX++)
					{
						int lU = lULookup[lSGX];

						if (lU >= 0 && lU < (int)mWidth)
						{
							uint32 lColor = ((uint32*)mBuffer)[lVOffset + lU];

							lA += GammaLookup::GammaToLinearFloat((uint8)((lColor >> 24) & 0xFF)) * (*lSGW);
							lR += GammaLookup::GammaToLinearFloat((uint8)((lColor >> 16) & 0xFF)) * (*lSGW);
							lG += GammaLookup::GammaToLinearFloat((uint8)((lColor >> 8) & 0xFF))  * (*lSGW);
							lB += GammaLookup::GammaToLinearFloat((uint8)(lColor & 0xFF))         * (*lSGW);

							lWeightSum += (*lSGW);
						}

						lSGW++;
					}
				}
			}

			// Convert back to normal colors
			float32 lOneOverWeightSum = 1.0f / lWeightSum;
			uint32 lA32 = GammaLookup::LinearToGamma((uint16)floor(lA * lOneOverWeightSum));
			uint32 lR32 = GammaLookup::LinearToGamma((uint16)floor(lR * lOneOverWeightSum));
			uint32 lG32 = GammaLookup::LinearToGamma((uint16)floor(lG * lOneOverWeightSum));
			uint32 lB32 = GammaLookup::LinearToGamma((uint16)floor(lB * lOneOverWeightSum));

			lBuffer[lDestOffset++] = (lA32 << 24) | (lR32 << 16) | (lG32 << 8) | lB32;
		}
	}

	Reset(pNewWidth, pNewHeight, mBitDepth);
	mBuffer = lBuffer;
	mBufferResponsibility = true;
}

void Canvas::Resize48BitSmooth(unsigned pNewWidth, unsigned pNewHeight)
{
	// Create the new buffer...
	uint16* lBuffer = new uint16[pNewWidth * pNewHeight * 3];

	float32 lScaleX = (float32)mWidth / (float32)pNewWidth;
	float32 lScaleY = (float32)mHeight / (float32)pNewHeight;

	// And here is the main loop.

	int lDestOffset = 0;
	for (int y = 0; y < (int)pNewHeight; y++)
	{
		// Precalc all possible V-coordinates.
		float32 lSampleY = (float32)y + 0.5f;
		int lVLookup[smSamplingGridSize];
		const int lBufferSize = (int)(mHeight * mPitch) * 3;
		for (int i = 0; i < smSamplingGridSize; i++)
		{
			lVLookup[i] = (int)((smSamplingGridCoordLookUp[i] + lSampleY) * lScaleY) * mPitch * 3;
		}

		for (int x = 0; x < (int)pNewWidth; x++)
		{
			float32 lR = 0.0f;
			float32 lG = 0.0f;
			float32 lB = 0.0f;

			// Precalc all possible U-coordinates.
			float32 lSampleX = (float32)x + 0.5f;
			int lULookup[smSamplingGridSize];
			for (int i = 0; i < smSamplingGridSize; i++)
			{
				lULookup[i] = (int)((smSamplingGridCoordLookUp[i] + lSampleX) * lScaleX);
			}

			// Time to take some samples.
			float32 lWeightSum = 0.0f;
			float32* lSGW = smSamplingGridWeight;

			for (int lSGY = 0; lSGY < smSamplingGridSize; lSGY++)
			{
				int lVOffset = lVLookup[lSGY];

				if (lVOffset >= 0 && lVOffset < lBufferSize)
				{
					for (int lSGX = 0; lSGX < smSamplingGridSize; lSGX++)
					{
						int lU = lULookup[lSGX];

						if (lU >= 0 && lU < (int)mWidth)
						{
							uint16* lColor = &((uint16*)mBuffer)[lVOffset + lU * 3];

							lB += lColor[0] * (*lSGW);
							lG += lColor[1] * (*lSGW);
							lR += lColor[2] * (*lSGW);

							lWeightSum += (*lSGW);
						}

						lSGW++;
					}
				}
			}

			// Convert back to normal colors
			float32 lOneOverWeightSum = 1.0f / lWeightSum;
			uint16 lR16 = (uint16)floor(lR * lOneOverWeightSum);
			uint16 lG16 = (uint16)floor(lG * lOneOverWeightSum);
			uint16 lB16 = (uint16)floor(lB * lOneOverWeightSum);

			lBuffer[lDestOffset + 0] = lB16;
			lBuffer[lDestOffset + 1] = lG16;
			lBuffer[lDestOffset + 2] = lR16;

			lDestOffset += 3;
		}
	}

	Reset(pNewWidth, pNewHeight, mBitDepth);
	mBuffer = lBuffer;
	mBufferResponsibility = true;
}

void Canvas::Resize96BitSmooth(unsigned pNewWidth, unsigned pNewHeight)
{
	// Create the new buffer...
	float32* lBuffer = new float32[pNewWidth * pNewHeight * 3];

	float32 lScaleX = (float32)mWidth / (float32)pNewWidth;
	float32 lScaleY = (float32)mHeight / (float32)pNewHeight;

	// And here is the main loop.

	int lDestOffset = 0;
	for (int y = 0; y < (int)pNewHeight; y++)
	{
		// Precalc all possible V-coordinates.
		float32 lSampleY = (float32)y + 0.5f;
		int lVLookup[smSamplingGridSize];
		const int lBufferSize = (int)(mHeight * mPitch) * 3;
		for (int i = 0; i < smSamplingGridSize; i++)
		{
			lVLookup[i] = (int)((smSamplingGridCoordLookUp[i] + lSampleY) * lScaleY) * mPitch * 3;
		}

		for (int x = 0; x < (int)pNewWidth; x++)
		{
			float32 lR = 0.0f;
			float32 lG = 0.0f;
			float32 lB = 0.0f;

			// Precalc all possible U-coordinates.
			float32 lSampleX = (float32)x + 0.5f;
			int lULookup[smSamplingGridSize];
			for (int i = 0; i < smSamplingGridSize; i++)
			{
				lULookup[i] = (int)((smSamplingGridCoordLookUp[i] + lSampleX) * lScaleX);
			}

			// Time to take some samples.
			float32 lWeightSum = 0.0f;
			float32* lSGW = smSamplingGridWeight;

			for (int lSGY = 0; lSGY < smSamplingGridSize; lSGY++)
			{
				int lVOffset = lVLookup[lSGY];

				if (lVOffset >= 0 && lVOffset < lBufferSize)
				{
					for (int lSGX = 0; lSGX < smSamplingGridSize; lSGX++)
					{
						int lU = lULookup[lSGX];

						if (lU >= 0 && lU < (int)mWidth)
						{
							float32* lColor = &((float32*)mBuffer)[lVOffset + lU * 3];

							lB += lColor[0] * (*lSGW);
							lG += lColor[1] * (*lSGW);
							lR += lColor[2] * (*lSGW);

							lWeightSum += (*lSGW);
						}

						lSGW++;
					}
				}
			}

			// Convert back to normal colors
			float32 lOneOverWeightSum = 1.0f / lWeightSum;
			lBuffer[lDestOffset + 0] = lB * lOneOverWeightSum;
			lBuffer[lDestOffset + 1] = lG * lOneOverWeightSum;
			lBuffer[lDestOffset + 2] = lR * lOneOverWeightSum;

			lDestOffset += 3;
		}
	}

	Reset(pNewWidth, pNewHeight, mBitDepth);
	mBuffer = lBuffer;
	mBufferResponsibility = true;
}

void Canvas::SwapRGBOrder()
{
	switch(mBitDepth)
	{
		case BITDEPTH_8_BIT:
			SwapRGB8Bit();
			break;
		case BITDEPTH_15_BIT:
			SwapRGB15Bit();
			break;
		case BITDEPTH_16_BIT:
			SwapRGB16Bit();
			break;
		case BITDEPTH_24_BIT:
			SwapRGB24Bit();
			break;
		case BITDEPTH_32_BIT:
			SwapRGB32Bit();
			break;
		case BITDEPTH_16_BIT_PER_CHANNEL:
			SwapRGB48Bit();
			break;
		case BITDEPTH_32_BIT_PER_CHANNEL:
			SwapRGB96Bit();
			break;
	};
}

void Canvas::SwapRGB8Bit()
{
	// Swap rgb in the palette.
	for (int i = 0; i < 256; i++)
	{
		uint8 lRed = mPalette[i].mRed;
		uint8 lBlue = mPalette[i].mBlue;

		mPalette[i].mRed = lBlue;
		mPalette[i].mBlue = lRed;
	}
}

void Canvas::SwapRGB15Bit()
{
	for (unsigned y = 0; y < mHeight; y++)
	{
		int lOffset = y * mPitch;

		for (unsigned x = 0; x < mWidth; x++)
		{
			uint16 lCol = ((uint16*)mBuffer)[lOffset + x];

			uint16 r = (lCol >> 10);
			uint16 g = (lCol >> 5) & 31;
			uint16 b =  lCol & 31;

			((uint16*)mBuffer)[lOffset + x] = (b << 10) | (g << 5) | r;
		}
	}
}

void Canvas::SwapRGB16Bit()
{
	for (unsigned y = 0; y < mHeight; y++)
	{
		int lOffset = y * mPitch;

		for (unsigned x = 0; x < mWidth; x++)
		{
			uint16 lCol = ((uint16*)mBuffer)[lOffset + x];

			uint16 r = (lCol >> 11);
			uint16 g = (lCol >> 5) & 63;
			uint16 b =  lCol & 31;

			((uint16*)mBuffer)[lOffset + x] = (b << 11) | (g << 5) | r;
		}
	}
}

void Canvas::SwapRGB24Bit()
{
	for (unsigned y = 0; y < mHeight; y++)
	{
		int lOffset = y * mPitch * 3;
		uint8* lBuffer = &((uint8*)mBuffer)[lOffset];
		const uint8* lEndBuffer = lBuffer + mWidth*3;
		for (; lBuffer < lEndBuffer; lBuffer += 3)
		{
			const uint8 r = *lBuffer;
			*lBuffer = lBuffer[2];
			lBuffer[2] = r;
		}
	}
}

void Canvas::SwapRGB32Bit()
{
	for (unsigned y = 0; y < mHeight; y++)
	{
		int lOffset = y * mPitch * 4;

		for (unsigned x = 0; x < mWidth; x++)
		{
			uint8 r = ((uint8*)mBuffer)[lOffset + 0];
			uint8 b = ((uint8*)mBuffer)[lOffset + 2];

			((uint8*)mBuffer)[lOffset + 0] = b;
			((uint8*)mBuffer)[lOffset + 2] = r;

			lOffset += 4;
		}
	}
}

void Canvas::SwapRGB48Bit()
{
	for (unsigned y = 0; y < mHeight; y++)
	{
		int lOffset = y * mPitch * 3;

		for (unsigned x = 0; x < mWidth; x++)
		{
			uint16 r = ((uint16*)mBuffer)[lOffset + 0];
			uint16 b = ((uint16*)mBuffer)[lOffset + 2];

			((uint16*)mBuffer)[lOffset + 0] = b;
			((uint16*)mBuffer)[lOffset + 2] = r;

			lOffset += 3;
		}
	}
}

void Canvas::SwapRGB96Bit()
{
	for (unsigned y = 0; y < mHeight; y++)
	{
		int lOffset = y * mPitch * 3;

		for (unsigned x = 0; x < mWidth; x++)
		{
			float32 r = ((float32*)mBuffer)[lOffset + 0];
			float32 b = ((float32*)mBuffer)[lOffset + 2];

			((float32*)mBuffer)[lOffset + 0] = b;
			((float32*)mBuffer)[lOffset + 2] = r;

			lOffset += 3;
		}
	}
}

void Canvas::RotateChannels(int pSteps)
{
	if (pSteps == 0)
	{
		return;
	}

	switch(mBitDepth)
	{
		case BITDEPTH_8_BIT:
			RotateChannels8Bit(pSteps);
			break;
		case BITDEPTH_15_BIT:
			RotateChannels15Bit(pSteps);
			break;
		case BITDEPTH_16_BIT:
			RotateChannels16Bit(pSteps);
			break;
		case BITDEPTH_24_BIT:
			RotateChannels24Bit(pSteps);
			break;
		case BITDEPTH_32_BIT:
			RotateChannels32Bit(pSteps);
			break;
		case BITDEPTH_16_BIT_PER_CHANNEL:
			RotateChannels48Bit(pSteps);
			break;
		case BITDEPTH_32_BIT_PER_CHANNEL:
			RotateChannels96Bit(pSteps);
			break;
	};
}

void Canvas::RotateChannels8Bit(int pSteps)
{
	if (pSteps > 0)
	{
		pSteps &= 3;
	}
	else
	{
		pSteps = 4 - ((-pSteps) & 3);
	}

	int lStep0 = (pSteps + 0) & 3;
	int lStep1 = (pSteps + 1) & 3;
	int lStep2 = (pSteps + 2) & 3;
	int lStep3 = (pSteps + 3) & 3;

	// Swap rgb in the palette.
	for (int i = 0; i < 256; i++)
	{
		uint8  lSrc[4];
		uint8* lDst[4];
		lSrc[0] = mPalette[i].mRed;
		lSrc[1] = mPalette[i].mGreen;
		lSrc[2] = mPalette[i].mBlue;
		lSrc[3] = mPalette[i].mAlpha;

		lDst[0] = &mPalette[i].mRed;
		lDst[1] = &mPalette[i].mGreen;
		lDst[2] = &mPalette[i].mBlue;
		lDst[3] = &mPalette[i].mAlpha;

		(*lDst[lStep0]) = lSrc[0];
		(*lDst[lStep1]) = lSrc[1];
		(*lDst[lStep2]) = lSrc[2];
		(*lDst[lStep3]) = lSrc[3];
	}
}

void Canvas::RotateChannels15Bit(int pSteps)
{
	if (pSteps > 0)
	{
		pSteps %= 3;
	}
	else
	{
		pSteps = 3 - ((-pSteps) % 3);
	}

	int lStep0 = (pSteps + 0) % 3;
	int lStep1 = (pSteps + 1) % 3;
	int lStep2 = (pSteps + 2) % 3;

	for (unsigned y = 0; y < mHeight; y++)
	{
		int lOffset = y * mPitch;

		for (unsigned x = 0; x < mWidth; x++)
		{
			uint16 lCol = ((uint16*)mBuffer)[lOffset + x];

			uint16 lSrc[3];
			lSrc[0] = (lCol >> 10);
			lSrc[1] = (lCol >> 5) & 31;
			lSrc[2] =  lCol & 31;

			uint16 lDst[3];
			lDst[lStep0] = lSrc[0];
			lDst[lStep1] = lSrc[1];
			lDst[lStep2] = lSrc[2];

			((uint16*)mBuffer)[lOffset + x] = (lDst[2] << 10) | (lDst[1] << 5) | lDst[0];
		}
	}
}

void Canvas::RotateChannels16Bit(int pSteps)
{
	if (pSteps > 0)
	{
		pSteps %= 3;
	}
	else
	{
		pSteps = 3 - ((-pSteps) % 3);
	}

	int lStep0 = (pSteps + 0) % 3;
	int lStep1 = (pSteps + 1) % 3;
	int lStep2 = (pSteps + 2) % 3;

	for (unsigned y = 0; y < mHeight; y++)
	{
		int lOffset = y * mPitch;

		for (unsigned x = 0; x < mWidth; x++)
		{
			uint16 lCol = ((uint16*)mBuffer)[lOffset + x];

			uint16 lSrc[3];
			lSrc[0] = (lCol >> 11) << 1;
			lSrc[1] = (lCol >> 5) & 63;
			lSrc[2] = (lCol & 31) << 1;

			uint16 lDst[3];
			lDst[lStep0] = lSrc[0];
			lDst[lStep1] = lSrc[1];
			lDst[lStep2] = lSrc[2];

			((uint16*)mBuffer)[lOffset + x] = ((lDst[2] >> 1) << 11) | (lDst[1] << 5) | (lDst[0] >> 1);
		}
	}
}

void Canvas::RotateChannels24Bit(int pSteps)
{
	if (pSteps > 0)
	{
		pSteps %= 3;
	}
	else
	{
		pSteps = 3 - ((-pSteps) % 3);
	}

	int lStep0 = (pSteps + 0) % 3;
	int lStep1 = (pSteps + 1) % 3;
	int lStep2 = (pSteps + 2) % 3;

	for (unsigned y = 0; y < mHeight; y++)
	{
		int lOffset = y * mPitch;

		for (unsigned x = 0; x < mWidth; x++)
		{
			uint8* lBuffer = &((uint8*)mBuffer)[(lOffset + x) * 3];

			uint8 lSrc[3];
			lSrc[0] = lBuffer[0];
			lSrc[1] = lBuffer[1];
			lSrc[2] = lBuffer[2];

			lBuffer[lStep0] = lSrc[0];
			lBuffer[lStep1] = lSrc[1];
			lBuffer[lStep2] = lSrc[2];
		}
	}
}

void Canvas::RotateChannels32Bit(int pSteps)
{
	if (pSteps > 0)
	{
		pSteps &= 3;
	}
	else
	{
		pSteps = 3 - ((-pSteps) & 3);
	}

	int lStep0 = (pSteps + 0) & 3;
	int lStep1 = (pSteps + 1) & 3;
	int lStep2 = (pSteps + 2) & 3;
	int lStep3 = (pSteps + 3) & 3;

	for (unsigned y = 0; y < mHeight; y++)
	{
		int lOffset = y * mPitch;

		for (unsigned x = 0; x < mWidth; x++)
		{
			uint8* lBuffer = &((uint8*)mBuffer)[(lOffset + x) << 2];

			uint8 lSrc[4];
			lSrc[0] = lBuffer[0];
			lSrc[1] = lBuffer[1];
			lSrc[2] = lBuffer[2];
			lSrc[3] = lBuffer[3];

			lBuffer[lStep0] = lSrc[0];
			lBuffer[lStep1] = lSrc[1];
			lBuffer[lStep2] = lSrc[2];
			lBuffer[lStep3] = lSrc[3];
		}
	}
}

void Canvas::RotateChannels48Bit(int pSteps)
{
	if (pSteps > 0)
	{
		pSteps %= 3;
	}
	else
	{
		pSteps = 3 - ((-pSteps) % 3);
	}

	int lStep0 = (pSteps + 0) % 3;
	int lStep1 = (pSteps + 1) % 3;
	int lStep2 = (pSteps + 2) % 3;

	for (unsigned y = 0; y < mHeight; y++)
	{
		int lOffset = y * mPitch;

		for (unsigned x = 0; x < mWidth; x++)
		{
			uint16* lBuffer = &((uint16*)mBuffer)[(lOffset + x) * 3];

			uint16 lSrc[3];
			lSrc[0] = lBuffer[0];
			lSrc[1] = lBuffer[1];
			lSrc[2] = lBuffer[2];

			lBuffer[lStep0] = lSrc[0];
			lBuffer[lStep1] = lSrc[1];
			lBuffer[lStep2] = lSrc[2];
		}
	}
}

void Canvas::RotateChannels96Bit(int pSteps)
{
	if (pSteps > 0)
	{
		pSteps %= 3;
	}
	else
	{
		pSteps = 3 - ((-pSteps) % 3);
	}

	int lStep0 = (pSteps + 0) % 3;
	int lStep1 = (pSteps + 1) % 3;
	int lStep2 = (pSteps + 2) % 3;

	for (unsigned y = 0; y < mHeight; y++)
	{
		int lOffset = y * mPitch;

		for (unsigned x = 0; x < mWidth; x++)
		{
			float32* lBuffer = &((float32*)mBuffer)[(lOffset + x) * 3];

			float32 lSrc[3];
			lSrc[0] = lBuffer[0];
			lSrc[1] = lBuffer[1];
			lSrc[2] = lBuffer[2];

			lBuffer[lStep0] = lSrc[0];
			lBuffer[lStep1] = lSrc[1];
			lBuffer[lStep2] = lSrc[2];
		}
	}
}

void Canvas::ConvertBitDepth(BitDepth pBitDepth)
{
	if (mBitDepth == pBitDepth)
	{
		return;
	}

	switch(pBitDepth)
	{
		case BITDEPTH_8_BIT:
			ConvertTo8();
			mPixelSize = 1;
			break;
		case BITDEPTH_15_BIT:
			ConvertTo15();
			mPixelSize = 2;
			break;
		case BITDEPTH_16_BIT:
			ConvertTo16();
			mPixelSize = 2;
			break;
		case BITDEPTH_24_BIT:
			ConvertTo24();
			mPixelSize = 3;
			break;
		case BITDEPTH_32_BIT:
			ConvertTo32();
			mPixelSize = 4;
			break;
		case BITDEPTH_16_BIT_PER_CHANNEL:
			ConvertTo48();
			mPixelSize = 6;
			break;
		case BITDEPTH_32_BIT_PER_CHANNEL:
			ConvertTo96();
			mPixelSize = 12;
			break;
	};
}

void Canvas::ConvertToGrayscale(bool pConvertTo8Bit)
{
	if (pConvertTo8Bit == true)
	{
		ConvertTo8BitGrayscale();
	}
	else
	{
		ConvertToGray();
	}
}

void Canvas::ConvertTo32BitWithAlpha(const Canvas& pAlphaBuffer)
{
	if (pAlphaBuffer.GetBitDepth() == BITDEPTH_8_BIT &&
	   pAlphaBuffer.GetWidth() == mWidth &&
	   pAlphaBuffer.GetHeight() == mHeight)
	{
		ConvertTo32();
		mPixelSize = 4;

		uint8* lAlpha = (uint8*)pAlphaBuffer.GetBuffer();

		for (unsigned y = 0; y < mHeight; y++)
		{
			int lAlphaOffset = y * pAlphaBuffer.GetPitch();
			uint8* lBuffer = (uint8*)mBuffer + y * mPitch * 4;

			for (unsigned x = 0; x < mWidth; x++)
			{
				lBuffer[3] = lAlpha[lAlphaOffset++];
				lBuffer += 4;
			}
		}
	}
}

void Canvas::GetAlphaChannel(Canvas& pAlphaBuffer) const
{
	if (mBitDepth != BITDEPTH_32_BIT)
	{
		return;
	}

	pAlphaBuffer.Reset(mWidth, mHeight, BITDEPTH_8_BIT);
	pAlphaBuffer.CreateBuffer();

	uint8* lAlpha = (uint8*)pAlphaBuffer.GetBuffer();

	for (unsigned y = 0; y < mHeight; y++)
	{
		int lAlphaOffset = y * pAlphaBuffer.GetPitch();
		uint8* lBuffer = (uint8*)mBuffer + y * mPitch * 4;

		for (unsigned x = 0; x < mWidth; x++)
		{
			lAlpha[lAlphaOffset++] = lBuffer[3];
			lBuffer += 4;
		}
	}
}

void Canvas::GetAlphaChannel(Canvas& pAlphaBuffer, uint8* pTransparentColors, unsigned pNumColors)
{
	if (mBitDepth != BITDEPTH_8_BIT)
	{
		return;
	}

	pAlphaBuffer.Reset(mWidth, mHeight, BITDEPTH_8_BIT);
	pAlphaBuffer.CreateBuffer();

	uint8* lAlpha = (uint8*)pAlphaBuffer.GetBuffer();

	for (unsigned y = 0; y < mHeight; y++)
	{
		int lAlphaOffset = y * pAlphaBuffer.GetPitch();
		uint8* lBuffer = (uint8*)mBuffer + y * mPitch;

		for (unsigned x = 0; x < mWidth; x++)
		{
			bool lTransparent = false;

			uint8 lColor = lBuffer[x];

			for (unsigned i = 0; i < pNumColors; i++)
			{
				if (lColor == pTransparentColors[i])
				{
					lTransparent = true;
					break;
				}
			}

			if (lTransparent == true)
			{
				lAlpha[lAlphaOffset++] = 0;
			}
			else
			{
				lAlpha[lAlphaOffset++] = 255;
			}
		}
	}
}

void Canvas::ConvertTo15()
{
	uint8* lTemp;
	int lImageSize = mPitch * mHeight,
		 i;
	uint16 lData;
	uint8 r, g, b;

	if (mBitDepth == 15)
	{
		return;
	}

	lTemp = new uint8[lImageSize * 2];

	switch(mBitDepth)
	{
	case BITDEPTH_8_BIT:
		for (i = 0; i < lImageSize; i++)
		{
			uint8 lCol = ((uint8*)mBuffer)[i];

			r = (mPalette[lCol].mRed		>> 3);
			g = (mPalette[lCol].mGreen	>> 3);
			b = (mPalette[lCol].mBlue		>> 3);

			lData =  ((short)r) << 10;
			lData += ((short)g) << 5;
			lData +=  (short)b;

			((uint16 *)lTemp)[i] = lData;
		}

		if (mBufferResponsibility == true)
		{
			delete[] (char*)mBuffer;
		}
		mBufferResponsibility = true;
		mBuffer = lTemp;
		mBitDepth = BITDEPTH_15_BIT;
		break;

	case BITDEPTH_16_BIT:
		for (i = 0; i < lImageSize; i++)
		{
			uint16 lCol = ((uint16*)mBuffer)[i];

			r = (uint8)(lCol >> 11);
			g = (uint8)(lCol >> 5) & 63;
			b =  (uint8)(lCol & 31);

			lData =  ((short)r) << 10;
			lData += ((short)g) << 5;
			lData += (short)(b);

			((uint16 *)lTemp)[i] = lData;
		}

		if (mBufferResponsibility == true)
		{
			delete[] (char*)mBuffer;
		}
		mBufferResponsibility = true;
		mBuffer = lTemp;
		mBitDepth = BITDEPTH_15_BIT;
		break;

	case BITDEPTH_24_BIT:
		for (i = 0; i < lImageSize; i++)
		{
			b = (((uint8*)mBuffer)[i * 3 + 0] >> 3);
			g = (((uint8*)mBuffer)[i * 3 + 1] >> 3);
			r = (((uint8*)mBuffer)[i * 3 + 2] >> 3);

			lData =  ((short)r) << 10;
			lData += ((short)g) << 5;
			lData +=  (short)b;

			((uint16 *)lTemp)[i] = lData;
		}

		if (mBufferResponsibility == true)
		{
			delete[] (char*)mBuffer;
		}
		mBufferResponsibility = true;
		mBuffer = lTemp;
		mBitDepth = BITDEPTH_15_BIT;
	break;

	case BITDEPTH_32_BIT:
		for (i = 0; i < lImageSize; i++)
		{
			b = (((uint8*)mBuffer)[(i<<2) + 0] >> 3);
			g = (((uint8*)mBuffer)[(i<<2) + 1] >> 3);
			r = (((uint8*)mBuffer)[(i<<2) + 2] >> 3);

			lData =  ((short)r) << 10;
			lData += ((short)g) << 5;
			lData +=  (short)b;

			((uint16 *)lTemp)[i] = lData;
		}

		if (mBufferResponsibility == true)
		{
			delete[] (char*)mBuffer;
		}
		mBufferResponsibility = true;
		mBuffer = lTemp;
		mBitDepth = BITDEPTH_15_BIT;
		break;

	case BITDEPTH_16_BIT_PER_CHANNEL:
		for (i = 0; i < lImageSize; i++)
		{
			b = (GammaLookup::LinearToGamma(((uint16*)mBuffer)[i * 3 + 0]) >> 3);
			g = (GammaLookup::LinearToGamma(((uint16*)mBuffer)[i * 3 + 1]) >> 3);
			r = (GammaLookup::LinearToGamma(((uint16*)mBuffer)[i * 3 + 2]) >> 3);

			lData =  ((short)r) << 10;
			lData += ((short)g) << 5;
			lData +=  (short)b;

			((uint16 *)lTemp)[i] = lData;
		}

		if (mBufferResponsibility == true)
		{
			delete[] (char*)mBuffer;
		}
		mBufferResponsibility = true;
		mBuffer = lTemp;
		mBitDepth = BITDEPTH_15_BIT;
		break;

	case BITDEPTH_32_BIT_PER_CHANNEL:
		for (i = 0; i < lImageSize; i++)
		{
			b = (uint8)floor(GammaLookup::LinearToGammaF(((float32*)mBuffer)[i * 3 + 0]) * 31.0f);
			g = (uint8)floor(GammaLookup::LinearToGammaF(((float32*)mBuffer)[i * 3 + 1]) * 31.0f);
			r = (uint8)floor(GammaLookup::LinearToGammaF(((float32*)mBuffer)[i * 3 + 2]) * 31.0f);

			lData =  ((short)r) << 10;
			lData += ((short)g) << 5;
			lData +=  (short)b;

			((uint16 *)lTemp)[i] = lData;
		}

		if (mBufferResponsibility == true)
		{
			delete[] (char*)mBuffer;
		}
		mBufferResponsibility = true;
		mBuffer = lTemp;
		mBitDepth = BITDEPTH_15_BIT;
		break;

	default:
		delete[] lTemp;
	}
}

void Canvas::ConvertTo16()
{
	uint8 *lTemp;
	int lImageSize = mPitch * mHeight;

	uint16 lData;
	uint8 r, g, b;
	int i;

	if (mBitDepth == 16)
	{
		return;
	}

	lTemp = new uint8 [lImageSize * 2];

	switch(mBitDepth)
	{
	case BITDEPTH_8_BIT:
		for (i = 0; i < lImageSize; i++)
		{
			uint8 lCol = ((uint8*)mBuffer)[i];

			r = (mPalette[lCol].mRed		>> 3);
			g = (mPalette[lCol].mGreen	>> 2);
			b = (mPalette[lCol].mBlue		>> 3);

			lData = ((short)r) << 11;
			lData += ((short)g) << 5;
			lData += (short)b;

			((uint16 *)lTemp)[i] = lData;
		}

		if (mBufferResponsibility == true)
		{
			delete[] (char*)mBuffer;
		}
		mBufferResponsibility = true;
		mBuffer = lTemp;
		mBitDepth = BITDEPTH_16_BIT;
		break;

	case BITDEPTH_15_BIT:
		for (i = 0; i < lImageSize; i++)
		{
			uint16 lCol = ((uint16*)mBuffer)[i];

			r = (uint8)(lCol >> 10);
			g = (uint8)(lCol >> 5) & 31;
			b =  (uint8)lCol & 31;

			lData = ((short)r) << 11;
			lData += ((short)g) << 6;
			lData += (short)b;

			((uint16 *)lTemp)[i] = lData;
		}

		if (mBufferResponsibility == true)
		{
			delete[] (char*)mBuffer;
		}
		mBufferResponsibility = true;
		mBuffer = lTemp;
		mBitDepth = BITDEPTH_16_BIT;
		break;

	case BITDEPTH_24_BIT:
		for (i = 0; i < lImageSize; i++)
		{
			b = (((uint8*)mBuffer)[i * 3 + 0] >> 3);
			g = (((uint8*)mBuffer)[i * 3 + 1] >> 2);
			r = (((uint8*)mBuffer)[i * 3 + 2] >> 3);

			lData = ((short)r) << 11;
			lData += ((short)g) << 5;
			lData += (short)b;

			((uint16 *)lTemp)[i] = lData;
		}

		if (mBufferResponsibility == true)
		{
			delete[] (char*)mBuffer;
		}
		mBufferResponsibility = true;
		mBuffer = lTemp;
		mBitDepth = BITDEPTH_16_BIT;
		break;

	case BITDEPTH_32_BIT:
		for (i = 0; i < lImageSize; i++)
		{
			b = (((uint8*)mBuffer)[(i<<2) + 0] >> 3);
			g = (((uint8*)mBuffer)[(i<<2) + 1] >> 2);
			r = (((uint8*)mBuffer)[(i<<2) + 2] >> 3);

			lData = ((short)r) << 11;
			lData += ((short)g) << 5;
			lData += (short)b;

			((uint16 *)lTemp)[i] = lData;
		}

		if (mBufferResponsibility == true)
		{
			delete[] (char*)mBuffer;
		}
		mBufferResponsibility = true;
		mBuffer = lTemp;
		mBitDepth = BITDEPTH_16_BIT;
		break;

	case BITDEPTH_16_BIT_PER_CHANNEL:
		for (i = 0; i < lImageSize; i++)
		{
			b = (GammaLookup::LinearToGamma(((uint16*)mBuffer)[i * 3 + 0]) >> 3);
			g = (GammaLookup::LinearToGamma(((uint16*)mBuffer)[i * 3 + 1]) >> 2);
			r = (GammaLookup::LinearToGamma(((uint16*)mBuffer)[i * 3 + 2]) >> 3);

			lData =  ((short)r) << 11;
			lData += ((short)g) << 5;
			lData +=  (short)b;

			((uint16 *)lTemp)[i] = lData;
		}

		if (mBufferResponsibility == true)
		{
			delete[] (char*)mBuffer;
		}
		mBufferResponsibility = true;
		mBuffer = lTemp;
		mBitDepth = BITDEPTH_16_BIT;
		break;

	case BITDEPTH_32_BIT_PER_CHANNEL:
		for (i = 0; i < lImageSize; i++)
		{
			b = (uint8)floor(GammaLookup::LinearToGammaF(((float32*)mBuffer)[i * 3 + 0]) * 31.0f);
			g = (uint8)floor(GammaLookup::LinearToGammaF(((float32*)mBuffer)[i * 3 + 1]) * 63.0f);
			r = (uint8)floor(GammaLookup::LinearToGammaF(((float32*)mBuffer)[i * 3 + 2]) * 31.0f);

			lData =  ((short)r) << 11;
			lData += ((short)g) << 5;
			lData +=  (short)b;

			((uint16 *)lTemp)[i] = lData;
		}

		if (mBufferResponsibility == true)
		{
			delete[] (char*)mBuffer;
		}
		mBufferResponsibility = true;
		mBuffer = lTemp;
		mBitDepth = BITDEPTH_16_BIT;
		break;

	default:
		delete[] lTemp;
	}
}

void Canvas::ConvertTo24()
{
	uint8 *lTemp;
	int lImageSize = mPitch * mHeight;

	uint16 lData;
	uint8 r, g, b;
	int i;

	lTemp = new uint8 [lImageSize * 3];

	switch(mBitDepth)
	{
	case BITDEPTH_8_BIT:
		for (i = 0; i < lImageSize; i++)
		{
			lTemp[i * 3 + 0] = mPalette[((uint8*)mBuffer)[i]].mBlue;
			lTemp[i * 3 + 1] = mPalette[((uint8*)mBuffer)[i]].mGreen;
			lTemp[i * 3 + 2] = mPalette[((uint8*)mBuffer)[i]].mRed;
		}

		if (mBufferResponsibility == true)
		{
			delete[] (char*)mBuffer;
		}
		mBufferResponsibility = true;
		mBuffer = lTemp;
		mBitDepth = BITDEPTH_24_BIT;
		break;

	case BITDEPTH_15_BIT:
		for (i = 0; i < lImageSize; i++)
		{
			lData = ((uint16 *)mBuffer)[i];

			r = (uint8)((lData >> 10) & 31) << 3;
			g = (uint8)((lData >> 5) & 31) << 3;
			b = (uint8)((lData) & 31) << 3;

			lTemp[i * 3 + 0] = b;
			lTemp[i * 3 + 1] = g;
			lTemp[i * 3 + 2] = r;
		}

		if (mBufferResponsibility == true)
		{
			delete[] (char*)mBuffer;
		}
		mBufferResponsibility = true;
		mBuffer = lTemp;
		mBitDepth = BITDEPTH_24_BIT;
		break;

	case BITDEPTH_16_BIT:
		for (i = 0; i < lImageSize; i++)
		{
			lData = ((uint16 *)mBuffer)[i];

			r = (uint8)((lData >> 11) & 31) << 3;
			g = (uint8)((lData >> 5) & 63) << 2;
			b = (uint8)((lData) & 31) << 3;

			lTemp[i * 3 + 0] = b;
			lTemp[i * 3 + 1] = g;
			lTemp[i * 3 + 2] = r;
		}

		if (mBufferResponsibility == true)
		{
			delete[] (char*)mBuffer;
		}
		mBufferResponsibility = true;
		mBuffer = lTemp;
		mBitDepth = BITDEPTH_24_BIT;
		break;

	case BITDEPTH_32_BIT:
		for (i = 0; i < lImageSize; i++)
		{
			lTemp[i * 3 + 0] = ((uint8*)mBuffer)[(i<<2) + 0];
			lTemp[i * 3 + 1] = ((uint8*)mBuffer)[(i<<2) + 1];
			lTemp[i * 3 + 2] = ((uint8*)mBuffer)[(i<<2) + 2];
		}

		if (mBufferResponsibility == true)
		{
			delete[] (char*)mBuffer;
		}
		mBufferResponsibility = true;
		mBuffer = lTemp;
		mBitDepth = BITDEPTH_24_BIT;
		break;

	case BITDEPTH_16_BIT_PER_CHANNEL:
		for (i = 0; i < lImageSize; i++)
		{
			b = GammaLookup::LinearToGamma(((uint16*)mBuffer)[i * 3 + 0]);
			g = GammaLookup::LinearToGamma(((uint16*)mBuffer)[i * 3 + 1]);
			r = GammaLookup::LinearToGamma(((uint16*)mBuffer)[i * 3 + 2]);

			lTemp[i * 3 + 0] = b;
			lTemp[i * 3 + 1] = g;
			lTemp[i * 3 + 2] = r;
		}

		if (mBufferResponsibility == true)
		{
			delete[] (char*)mBuffer;
		}
		mBufferResponsibility = true;
		mBuffer = lTemp;
		mBitDepth = BITDEPTH_24_BIT;
		break;

	case BITDEPTH_32_BIT_PER_CHANNEL:
		for (i = 0; i < lImageSize; i++)
		{
			b = (uint8)floor(GammaLookup::LinearToGammaF(((float32*)mBuffer)[i * 3 + 0]) * 255.0f);
			g = (uint8)floor(GammaLookup::LinearToGammaF(((float32*)mBuffer)[i * 3 + 1]) * 255.0f);
			r = (uint8)floor(GammaLookup::LinearToGammaF(((float32*)mBuffer)[i * 3 + 2]) * 255.0f);

			lTemp[i * 3 + 0] = b;
			lTemp[i * 3 + 1] = g;
			lTemp[i * 3 + 2] = r;
		}

		if (mBufferResponsibility == true)
		{
			delete[] (char*)mBuffer;
		}
		mBufferResponsibility = true;
		mBuffer = lTemp;
		mBitDepth = BITDEPTH_24_BIT;
		break;

	default:
		delete[] lTemp;
	}
}

void Canvas::ConvertTo32()
{
	uint8 *lTemp;
	int lImageSize = mPitch * mHeight;

	uint16 lData;
	uint8 r, g, b;
	int i;

	lTemp = new uint8 [lImageSize * 4];

	switch(mBitDepth)
	{
	case BITDEPTH_8_BIT:
		for (i = 0; i < lImageSize; i++)
		{
			lTemp[(i<<2) + 0] = mPalette[((uint8*)mBuffer)[i]].mBlue;
			lTemp[(i<<2) + 1] = mPalette[((uint8*)mBuffer)[i]].mGreen;
			lTemp[(i<<2) + 2] = mPalette[((uint8*)mBuffer)[i]].mRed;
			lTemp[(i<<2) + 3] = 255; //Alpha
		}

		if (mBufferResponsibility == true)
		{
			delete[] (char*)mBuffer;
		}
		mBufferResponsibility = true;
		mBuffer = lTemp;
		mBitDepth = BITDEPTH_32_BIT;
		break;

	case BITDEPTH_15_BIT:
		for (i = 0; i < lImageSize; i++)
		{
			lData = ((uint16 *)mBuffer)[i];

			r = (uint8)((lData >> 10) & 31) << 3;
			g = (uint8)((lData >> 5) & 63) << 3;
			b = (uint8)((lData) & 31) << 3;

			lTemp[(i<<2) + 0] = b;
			lTemp[(i<<2) + 1] = g;
			lTemp[(i<<2) + 2] = r;
			lTemp[(i<<2) + 3] = 255; //Alpha
		}

		if (mBufferResponsibility == true)
		{
			delete[] (char*)mBuffer;
		}
		mBufferResponsibility = true;
		mBuffer = lTemp;
		mBitDepth = BITDEPTH_32_BIT;
		break;

	case BITDEPTH_16_BIT:
		for (i = 0; i < lImageSize; i++)
		{
			lData = ((uint16 *)mBuffer)[i];

			r = (uint8)((lData >> 11) & 31) << 3;
			g = (uint8)((lData >> 5) & 63) << 2;
			b = (uint8)((lData) & 31) << 3;

			lTemp[(i<<2) + 0] = b;
			lTemp[(i<<2) + 1] = g;
			lTemp[(i<<2) + 2] = r;
			lTemp[(i<<2) + 3] = 255; //Alpha
		}

		if (mBufferResponsibility == true)
		{
			delete[] (char*)mBuffer;
		}
		mBufferResponsibility = true;
		mBuffer = lTemp;
		mBitDepth = BITDEPTH_32_BIT;
		break;

	case BITDEPTH_24_BIT:
		for (i = 0; i < lImageSize; i++)
		{
			lTemp[(i<<2) + 0] = ((uint8*)mBuffer)[i * 3 + 0];
			lTemp[(i<<2) + 1] = ((uint8*)mBuffer)[i * 3 + 1];
			lTemp[(i<<2) + 2] = ((uint8*)mBuffer)[i * 3 + 2];
			lTemp[(i<<2) + 3] = 255; //Alpha
		}

		if (mBufferResponsibility == true)
		{
			delete[] (char*)mBuffer;
		}
		mBufferResponsibility = true;
		mBuffer = lTemp;
		mBitDepth = BITDEPTH_32_BIT;
		break;

	case BITDEPTH_16_BIT_PER_CHANNEL:
		for (i = 0; i < lImageSize; i++)
		{
			b = GammaLookup::LinearToGamma(((uint16*)mBuffer)[i * 3 + 0]);
			g = GammaLookup::LinearToGamma(((uint16*)mBuffer)[i * 3 + 1]);
			r = GammaLookup::LinearToGamma(((uint16*)mBuffer)[i * 3 + 2]);

			lTemp[(i<<2) + 0] = b;
			lTemp[(i<<2) + 1] = g;
			lTemp[(i<<2) + 2] = r;
			lTemp[(i<<2) + 3] = 255; //Alpha
		}

		if (mBufferResponsibility == true)
		{
			delete[] (char*)mBuffer;
		}
		mBufferResponsibility = true;
		mBuffer = lTemp;
		mBitDepth = BITDEPTH_32_BIT;
		break;

	case BITDEPTH_32_BIT_PER_CHANNEL:
		for (i = 0; i < lImageSize; i++)
		{
			b = (uint8)floor(GammaLookup::LinearToGammaF(((float32*)mBuffer)[i * 3 + 0]) * 255.0f);
			g = (uint8)floor(GammaLookup::LinearToGammaF(((float32*)mBuffer)[i * 3 + 1]) * 255.0f);
			r = (uint8)floor(GammaLookup::LinearToGammaF(((float32*)mBuffer)[i * 3 + 2]) * 255.0f);

			lTemp[(i<<2) + 0] = b;
			lTemp[(i<<2) + 1] = g;
			lTemp[(i<<2) + 2] = r;
			lTemp[(i<<2) + 3] = 255; //Alpha
		}

		if (mBufferResponsibility == true)
		{
			delete[] (char*)mBuffer;
		}
		mBufferResponsibility = true;
		mBuffer = lTemp;
		mBitDepth = BITDEPTH_32_BIT;
		break;

	default:
		delete[] lTemp;
	}
}

void Canvas::ConvertTo48()
{
	uint16 *lTemp;
	int lImageSize = mPitch * mHeight;

	uint16 r, g, b;
	int i;

	lTemp = new uint16 [lImageSize * 3];

	switch(mBitDepth)
	{
	case BITDEPTH_8_BIT:
		for (i = 0; i < lImageSize; i++)
		{
			lTemp[(i * 3) + 0] = GammaLookup::GammaToLinear(mPalette[((uint8*)mBuffer)[i]].mBlue);
			lTemp[(i * 3) + 1] = GammaLookup::GammaToLinear(mPalette[((uint8*)mBuffer)[i]].mGreen);
			lTemp[(i * 3) + 2] = GammaLookup::GammaToLinear(mPalette[((uint8*)mBuffer)[i]].mRed);
		}

		if (mBufferResponsibility == true)
		{
			delete[] (char*)mBuffer;
		}
		mBufferResponsibility = true;
		mBuffer = lTemp;
		mBitDepth = BITDEPTH_16_BIT_PER_CHANNEL;
		break;

	case BITDEPTH_15_BIT:
		for (i = 0; i < lImageSize; i++)
		{
			uint16 lData = ((uint16 *)mBuffer)[i];

			r = ((lData >> 10) & 31) << 3;
			g = ((lData >> 5) & 63) << 3;
			b = ((lData) & 31) << 3;

			lTemp[(i * 3) + 0] = GammaLookup::GammaToLinear((uint8)b);
			lTemp[(i * 3) + 1] = GammaLookup::GammaToLinear((uint8)g);
			lTemp[(i * 3) + 2] = GammaLookup::GammaToLinear((uint8)r);
		}

		if (mBufferResponsibility == true)
		{
			delete[] (char*)mBuffer;
		}
		mBufferResponsibility = true;
		mBuffer = lTemp;
		mBitDepth = BITDEPTH_16_BIT_PER_CHANNEL;
		break;

	case BITDEPTH_16_BIT:
		for (i = 0; i < lImageSize; i++)
		{
			uint16 lData = ((uint16 *)mBuffer)[i];

			r = ((lData >> 11) & 31) << 3;
			g = ((lData >> 5) & 63) << 2;
			b = ((lData) & 31) << 3;

			lTemp[(i * 3) + 0] = GammaLookup::GammaToLinear((uint8)b);
			lTemp[(i * 3) + 1] = GammaLookup::GammaToLinear((uint8)g);
			lTemp[(i * 3) + 2] = GammaLookup::GammaToLinear((uint8)r);
		}

		if (mBufferResponsibility == true)
		{
			delete[] (char*)mBuffer;
		}
		mBufferResponsibility = true;
		mBuffer = lTemp;
		mBitDepth = BITDEPTH_16_BIT_PER_CHANNEL;
		break;

	case BITDEPTH_24_BIT:
		for (i = 0; i < lImageSize; i++)
		{
			lTemp[(i * 3) + 0] = GammaLookup::GammaToLinear(((uint8*)mBuffer)[i * 3 + 0]);
			lTemp[(i * 3) + 1] = GammaLookup::GammaToLinear(((uint8*)mBuffer)[i * 3 + 1]);
			lTemp[(i * 3) + 2] = GammaLookup::GammaToLinear(((uint8*)mBuffer)[i * 3 + 2]);
		}

		if (mBufferResponsibility == true)
		{
			delete[] (char*)mBuffer;
		}
		mBufferResponsibility = true;
		mBuffer = lTemp;
		mBitDepth = BITDEPTH_16_BIT_PER_CHANNEL;
		break;

	case BITDEPTH_32_BIT:
		for (i = 0; i < lImageSize; i++)
		{
			lTemp[(i * 3) + 0] = GammaLookup::GammaToLinear(((uint8*)mBuffer)[(i<<2) + 0]);
			lTemp[(i * 3) + 1] = GammaLookup::GammaToLinear(((uint8*)mBuffer)[(i<<2) + 1]);
			lTemp[(i * 3) + 2] = GammaLookup::GammaToLinear(((uint8*)mBuffer)[(i<<2) + 2]);
		}

		if (mBufferResponsibility == true)
		{
			delete[] (char*)mBuffer;
		}
		mBufferResponsibility = true;
		mBuffer = lTemp;
		mBitDepth = BITDEPTH_16_BIT_PER_CHANNEL;
		break;

	case BITDEPTH_32_BIT_PER_CHANNEL:
		for (i = 0; i < lImageSize; i++)
		{
			lTemp[i * 3 + 0] = (uint16)floor(((float32*)mBuffer)[i * 3 + 0] * 65535.0f);
			lTemp[i * 3 + 1] = (uint16)floor(((float32*)mBuffer)[i * 3 + 1] * 65535.0f);
			lTemp[i * 3 + 2] = (uint16)floor(((float32*)mBuffer)[i * 3 + 2] * 65535.0f);
		}

		if (mBufferResponsibility == true)
		{
			delete[] (char*)mBuffer;
		}
		mBufferResponsibility = true;
		mBuffer = lTemp;
		mBitDepth = BITDEPTH_16_BIT_PER_CHANNEL;
		break;

	default:
		delete[] lTemp;
	}
}

void Canvas::ConvertTo96()
{
	float32 *lTemp;
	int lImageSize = mPitch * mHeight;

	uint16 r, g, b;
	int i;

	lTemp = new float32 [lImageSize * 3];

	const float32 lOneOver255 = 1.0f / 255.0f;

	switch(mBitDepth)
	{
	case BITDEPTH_8_BIT:
		for (i = 0; i < lImageSize; i++)
		{
			lTemp[(i * 3) + 0] = GammaLookup::GammaToLinearF((float32)mPalette[((uint8*)mBuffer)[i]].mBlue * lOneOver255);
			lTemp[(i * 3) + 1] = GammaLookup::GammaToLinearF((float32)mPalette[((uint8*)mBuffer)[i]].mGreen * lOneOver255);
			lTemp[(i * 3) + 2] = GammaLookup::GammaToLinearF((float32)mPalette[((uint8*)mBuffer)[i]].mRed * lOneOver255);
		}

		if (mBufferResponsibility == true)
		{
			delete[] (char*)mBuffer;
		}
		mBufferResponsibility = true;
		mBuffer = lTemp;
		mBitDepth = BITDEPTH_32_BIT_PER_CHANNEL;
		break;

	case BITDEPTH_15_BIT:
		for (i = 0; i < lImageSize; i++)
		{
			uint16 lData = ((uint16 *)mBuffer)[i];

			r = ((lData >> 10) & 31) << 3;
			g = ((lData >> 5) & 63) << 3;
			b = ((lData) & 31) << 3;

			lTemp[(i * 3) + 0] = GammaLookup::GammaToLinearF((float32)b * lOneOver255);
			lTemp[(i * 3) + 1] = GammaLookup::GammaToLinearF((float32)g * lOneOver255);
			lTemp[(i * 3) + 2] = GammaLookup::GammaToLinearF((float32)r * lOneOver255);
		}

		if (mBufferResponsibility == true)
		{
			delete[] (char*)mBuffer;
		}
		mBufferResponsibility = true;
		mBuffer = lTemp;
		mBitDepth = BITDEPTH_32_BIT_PER_CHANNEL;
		break;

	case BITDEPTH_16_BIT:
		for (i = 0; i < lImageSize; i++)
		{
			uint16 lData = ((uint16 *)mBuffer)[i];

			r = ((lData >> 11) & 31) << 3;
			g = ((lData >> 5) & 63) << 2;
			b = ((lData) & 31) << 3;

			lTemp[(i * 3) + 0] = GammaLookup::GammaToLinearF((float32)b * lOneOver255);
			lTemp[(i * 3) + 1] = GammaLookup::GammaToLinearF((float32)g * lOneOver255);
			lTemp[(i * 3) + 2] = GammaLookup::GammaToLinearF((float32)r * lOneOver255);
		}

		if (mBufferResponsibility == true)
		{
			delete[] (char*)mBuffer;
		}
		mBufferResponsibility = true;
		mBuffer = lTemp;
		mBitDepth = BITDEPTH_32_BIT_PER_CHANNEL;
		break;

	case BITDEPTH_24_BIT:
		for (i = 0; i < lImageSize; i++)
		{
			b = ((uint8*)mBuffer)[i * 3 + 0];
			g = ((uint8*)mBuffer)[i * 3 + 1];
			r = ((uint8*)mBuffer)[i * 3 + 2];

			lTemp[(i * 3) + 0] = GammaLookup::GammaToLinearF((float32)b * lOneOver255);
			lTemp[(i * 3) + 1] = GammaLookup::GammaToLinearF((float32)g * lOneOver255);
			lTemp[(i * 3) + 2] = GammaLookup::GammaToLinearF((float32)r * lOneOver255);
		}

		if (mBufferResponsibility == true)
		{
			delete[] (char*)mBuffer;
		}
		mBufferResponsibility = true;
		mBuffer = lTemp;
		mBitDepth = BITDEPTH_32_BIT_PER_CHANNEL;
		break;

	case BITDEPTH_32_BIT:
		for (i = 0; i < lImageSize; i++)
		{
			b = ((uint8*)mBuffer)[(i<<2) + 0];
			g = ((uint8*)mBuffer)[(i<<2) + 1];
			r = ((uint8*)mBuffer)[(i<<2) + 2];
			lTemp[(i * 3) + 0] = GammaLookup::GammaToLinearF((float32)b * lOneOver255);
			lTemp[(i * 3) + 1] = GammaLookup::GammaToLinearF((float32)g * lOneOver255);
			lTemp[(i * 3) + 2] = GammaLookup::GammaToLinearF((float32)r * lOneOver255);
		}

		if (mBufferResponsibility == true)
		{
			delete[] (char*)mBuffer;
		}
		mBufferResponsibility = true;
		mBuffer = lTemp;
		mBitDepth = BITDEPTH_32_BIT_PER_CHANNEL;
		break;

	case BITDEPTH_16_BIT_PER_CHANNEL:
		for (i = 0; i < lImageSize; i++)
		{
			lTemp[i * 3 + 0] = (float32)((uint16*)mBuffer)[i * 3 + 0] / 65535.0f;
			lTemp[i * 3 + 1] = (float32)((uint16*)mBuffer)[i * 3 + 1] / 65535.0f;
			lTemp[i * 3 + 2] = (float32)((uint16*)mBuffer)[i * 3 + 2] / 65535.0f;
		}

		if (mBufferResponsibility == true)
		{
			delete[] (char*)mBuffer;
		}
		mBufferResponsibility = true;
		mBuffer = lTemp;
		mBitDepth = BITDEPTH_32_BIT_PER_CHANNEL;
		break;

	default:
		delete[] lTemp;
	}
}

void Canvas::ConvertTo8BitGrayscale()
{
	uint8 *lTemp;
	int lImageSize = mPitch * mHeight;

	uint16 lData;
	float32 r, g, b;
	int i;

	lTemp = new uint8 [lImageSize];

	switch(mBitDepth)
	{
	case BITDEPTH_8_BIT:
		for (i = 0; i < lImageSize; i++)
		{
			r = (float32)GammaLookup::GammaToLinear(mPalette[((uint8*)mBuffer)[i]].mRed);
			g = (float32)GammaLookup::GammaToLinear(mPalette[((uint8*)mBuffer)[i]].mGreen);
			b = (float32)GammaLookup::GammaToLinear(mPalette[((uint8*)mBuffer)[i]].mBlue);

			// The magic formula: 30% Red, 59% Green and 11% Blue.
			lTemp[i] = GammaLookup::LinearToGamma((uint16)(r * 0.3f + g * 0.59f + b * 0.11f));
		}

		if (mBufferResponsibility == true)
		{
			delete[] (char*)mBuffer;
		}
		mBufferResponsibility = true;
		mBuffer = lTemp;
		mBitDepth = BITDEPTH_8_BIT;
		break;

	case BITDEPTH_15_BIT:
		for (i = 0; i < lImageSize; i++)
		{
			lData = ((uint16 *)mBuffer)[i];

			r = (float32)GammaLookup::GammaToLinear((uint8)((lData >> 10) & 31) << 3);
			g = (float32)GammaLookup::GammaToLinear((uint8)((lData >> 5) & 31) << 3);
			b = (float32)GammaLookup::GammaToLinear((uint8)((lData) & 31) << 3);

			// The magic formula: 30% Red, 59% Green and 11% Blue.
			lTemp[i] = GammaLookup::LinearToGamma((uint16)(r * 0.3f + g * 0.59f + b * 0.11f));
		}

		if (mBufferResponsibility == true)
		{
			delete[] (char*)mBuffer;
		}
		mBufferResponsibility = true;
		mBuffer = lTemp;
		mBitDepth = BITDEPTH_8_BIT;
		break;

	case BITDEPTH_16_BIT:
		for (i = 0; i < lImageSize; i++)
		{
			lData = ((uint16 *)mBuffer)[i];

			r = (float32)GammaLookup::GammaToLinear((uint8)((lData >> 11) & 31) << 3);
			g = (float32)GammaLookup::GammaToLinear((uint8)((lData >> 5) & 63) << 2);
			b = (float32)GammaLookup::GammaToLinear((uint8)((lData) & 31) << 3);

			// The magic formula: 30% Red, 59% Green and 11% Blue.
			lTemp[i] = GammaLookup::LinearToGamma((uint16)(r * 0.3f + g * 0.59f + b * 0.11f));
		}

		if (mBufferResponsibility == true)
		{
			delete[] (char*)mBuffer;
		}
		mBufferResponsibility = true;
		mBuffer = lTemp;
		mBitDepth = BITDEPTH_8_BIT;
		break;

	case BITDEPTH_24_BIT:
		for (i = 0; i < lImageSize; i++)
		{
			b = (float32)GammaLookup::GammaToLinear(((uint8*)mBuffer)[i * 3 + 0]);
			g = (float32)GammaLookup::GammaToLinear(((uint8*)mBuffer)[i * 3 + 1]);
			r = (float32)GammaLookup::GammaToLinear(((uint8*)mBuffer)[i * 3 + 2]);

			// The magic formula: 30% Red, 59% Green and 11% Blue.
			lTemp[i] = GammaLookup::LinearToGamma((uint16)(r * 0.3f + g * 0.59f + b * 0.11f));
		}

		if (mBufferResponsibility == true)
		{
			delete[] (char*)mBuffer;
		}
		mBufferResponsibility = true;
		mBuffer = lTemp;
		mBitDepth = BITDEPTH_8_BIT;
		break;
	case BITDEPTH_32_BIT:
		for (i = 0; i < lImageSize; i++)
		{
			b = (float32)GammaLookup::GammaToLinear(((uint8*)mBuffer)[i * 4 + 0]);
			g = (float32)GammaLookup::GammaToLinear(((uint8*)mBuffer)[i * 4 + 1]);
			r = (float32)GammaLookup::GammaToLinear(((uint8*)mBuffer)[i * 4 + 2]);

			// The magic formula: 30% Red, 59% Green and 11% Blue.
			lTemp[i] = GammaLookup::LinearToGamma((uint16)(r * 0.3f + g * 0.59f + b * 0.11f));
		}

		if (mBufferResponsibility == true)
		{
			delete[] (char*)mBuffer;
		}
		mBufferResponsibility = true;
		mBuffer = lTemp;
		mBitDepth = BITDEPTH_8_BIT;
		break;

	case BITDEPTH_16_BIT_PER_CHANNEL:
		for (i = 0; i < lImageSize; i++)
		{
			b = (float32)((uint16*)mBuffer)[i * 3 + 0];
			g = (float32)((uint16*)mBuffer)[i * 3 + 1];
			r = (float32)((uint16*)mBuffer)[i * 3 + 2];

			// The magic formula: 30% Red, 59% Green and 11% Blue.
			lTemp[i] = GammaLookup::LinearToGamma((uint16)(r * 0.3f + g * 0.59f + b * 0.11f));
		}

		if (mBufferResponsibility == true)
		{
			delete[] (char*)mBuffer;
		}
		mBufferResponsibility = true;
		mBuffer = lTemp;
		mBitDepth = BITDEPTH_8_BIT;
		break;

	case BITDEPTH_32_BIT_PER_CHANNEL:
		for (i = 0; i < lImageSize; i++)
		{
			b = ((float32*)mBuffer)[i * 3 + 0];
			g = ((float32*)mBuffer)[i * 3 + 1];
			r = ((float32*)mBuffer)[i * 3 + 2];

			// The magic formula: 30% Red, 59% Green and 11% Blue.
			lTemp[i] = (uint8)floor(GammaLookup::LinearToGammaF(r * 0.3f + g * 0.59f + b * 0.11f) * 255.0f);
		}

		if (mBufferResponsibility == true)
		{
			delete[] (char*)mBuffer;
		}
		mBufferResponsibility = true;
		mBuffer = lTemp;
		mBitDepth = BITDEPTH_8_BIT;
		break;

	default:
		delete[] lTemp;
		return;
	}

	for (i = 0; i < 256; i++)
	{
		mPalette[i].mRed   = (uint8)i;
		mPalette[i].mGreen = (uint8)i;
		mPalette[i].mBlue  = (uint8)i;
	}

	mPixelSize = 1;
}

void Canvas::ConvertToGray()
{
	int lImageSize = mPitch * mHeight;

	uint16 lData;
	float32 r, g, b;
	uint8 v;
	int i;

	switch(mBitDepth)
	{
	case BITDEPTH_8_BIT:
		for (i = 0; i < lImageSize; i++)
		{
			r = (float32)GammaLookup::GammaToLinear(mPalette[((uint8*)mBuffer)[i]].mRed);
			g = (float32)GammaLookup::GammaToLinear(mPalette[((uint8*)mBuffer)[i]].mGreen);
			b = (float32)GammaLookup::GammaToLinear(mPalette[((uint8*)mBuffer)[i]].mBlue);

			// The magic formula: 30% Red, 59% Green and 11% Blue.
			((uint8*)mBuffer)[i] = GammaLookup::LinearToGamma((uint16)(r * 0.3f + g * 0.59f + b * 0.11f));
		}

		for (i = 0; i < 256; i++)
		{
			mPalette[i].mRed = (uint8)i;
			mPalette[i].mGreen = (uint8)i;
			mPalette[i].mBlue = (uint8)i;
		}
		break;

	case BITDEPTH_15_BIT:
		for (i = 0; i < lImageSize; i++)
		{
			lData = ((uint16 *)mBuffer)[i];

			r = (float32)GammaLookup::GammaToLinear((uint8)((lData >> 10) & 31) << 3);
			g = (float32)GammaLookup::GammaToLinear((uint8)((lData >> 5) & 31) << 3);
			b = (float32)GammaLookup::GammaToLinear((uint8)((lData) & 31) << 3);

			// The magic formula: 30% Red, 59% Green and 11% Blue.
			lData = ((uint16)GammaLookup::LinearToGamma((uint16)(r * 0.3f + g * 0.59f + b * 0.11f)) >> 3);
			((uint16 *)mBuffer)[i] = (lData << 10) + (lData << 5) + lData;
		}
		break;

	case BITDEPTH_16_BIT:
		for (i = 0; i < lImageSize; i++)
		{
			lData = ((uint16 *)mBuffer)[i];

			r = (float32)GammaLookup::GammaToLinear((uint8)((lData >> 11) & 31) << 3);
			g = (float32)GammaLookup::GammaToLinear((uint8)((lData >> 5) & 63) << 2);
			b = (float32)GammaLookup::GammaToLinear((uint8)((lData) & 31) << 3);

			// The magic formula: 30% Red, 59% Green and 11% Blue.
			lData = ((uint16)GammaLookup::LinearToGamma((uint16)(r * 0.3f + g * 0.59f + b * 0.11f)) >> 2);
			((uint16 *)mBuffer)[i] = ((lData >> 1) << 11) + (lData << 5) + (lData >> 1);
		}
		break;

	case BITDEPTH_24_BIT:
		for (i = 0; i < lImageSize; i++)
		{
			b = (float32)GammaLookup::GammaToLinear(((uint8*)mBuffer)[i * 3 + 0]);
			g = (float32)GammaLookup::GammaToLinear(((uint8*)mBuffer)[i * 3 + 1]);
			r = (float32)GammaLookup::GammaToLinear(((uint8*)mBuffer)[i * 3 + 2]);

			// The magic formula: 30% Red, 59% Green and 11% Blue.
			v = GammaLookup::LinearToGamma((uint16)(r * 0.3f + g * 0.59f + b * 0.11f));

			((uint8*)mBuffer)[i * 3 + 0] = v;
			((uint8*)mBuffer)[i * 3 + 1] = v;
			((uint8*)mBuffer)[i * 3 + 2] = v;
		}
		break;
	case BITDEPTH_32_BIT:
		for (i = 0; i < lImageSize; i++)
		{
			b = (float32)GammaLookup::GammaToLinear(((uint8*)mBuffer)[i * 4 + 0]);
			g = (float32)GammaLookup::GammaToLinear(((uint8*)mBuffer)[i * 4 + 1]);
			r = (float32)GammaLookup::GammaToLinear(((uint8*)mBuffer)[i * 4 + 2]);

			// The magic formula: 30% Red, 59% Green and 11% Blue.
			v = GammaLookup::LinearToGamma((uint16)(r * 0.3f + g * 0.59f + b * 0.11f));

			((uint8*)mBuffer)[i * 4 + 0] = v;
			((uint8*)mBuffer)[i * 4 + 1] = v;
			((uint8*)mBuffer)[i * 4 + 2] = v;
		}
		break;
	}
}




int Canvas::GenerateOctreeAndList(ColorOctreeNode* pOctTopNode, ColorOctreeNodeListNode** pFirstLink)
{
	unsigned i;
	int lOffset = 0;
	bool lFirst = true;
	int lNumLinks = 0;
	// Loop through all pixels and store the colors in the octree
	// and store each leafnode of the octree in the linked list.

	for (i = 0; i < mWidth * mHeight; i++)
	{
		uint8 lBlue	= ((uint8*)mBuffer)[lOffset + 0];
		uint8 lGreen	= ((uint8*)mBuffer)[lOffset + 1];
		uint8 lRed	= ((uint8*)mBuffer)[lOffset + 2];

		ColorOctreeNode* lOctChildNode = pOctTopNode->InsertColor(lRed, lGreen, lBlue, 0);

		// Insert this node in the list.
		if (lOctChildNode != 0)
		{
			lNumLinks++;

			if (lFirst == true)
			{
				(*pFirstLink)->mColorOctreeNode = lOctChildNode;
				lFirst = false;
			}
			else
			{
				ColorOctreeNodeListNode* lNewListNode = new ColorOctreeNodeListNode();
				lNewListNode->mColorOctreeNode = lOctChildNode;

				lNewListNode->mNext = *pFirstLink;
				*pFirstLink = lNewListNode;
			}

		}

		lOffset += 3;
	}

	return lNumLinks;
}



void Canvas::SortLinkedList(ColorOctreeNodeListNode** pFirstLink, int pNumLinks)
{
	ColorOctreeNodeListNode* lCurrentNode;
	ColorOctreeNodeListNode** lNodeArray = new ColorOctreeNodeListNode*[pNumLinks];
	ColorOctreeNodeListNode** lSortedNodeArray = new ColorOctreeNodeListNode*[pNumLinks];

	// Sorting step 1: Place all list nodes in an array, and get the maximum
	//                 number of different possible values to sort.
	int lMaxRefCount = 0;
	int i = 0;

	lCurrentNode = *pFirstLink;

	while (lCurrentNode != 0)
	{
		if (lCurrentNode->mColorOctreeNode->GetReferenceCount() > lMaxRefCount)
		{
			lMaxRefCount = lCurrentNode->mColorOctreeNode->GetReferenceCount();
		}

		lNodeArray[i] = lCurrentNode;
		i++;

		lCurrentNode = lCurrentNode->mNext;
	}

	// Sorting step 2: Allocate and prepare buffers...
	int* lValueCount = new int[lMaxRefCount];
	int* lOffset = new int[lMaxRefCount];

	memset(lValueCount, 0, lMaxRefCount * sizeof(int));
	memset(lOffset, 0, lMaxRefCount * sizeof(int));

	// Sorting step 3: Count how many times each value occurs in the list.
	for (i = 0; i < pNumLinks; i++)
	{
		int lIndex = lNodeArray[i]->mColorOctreeNode->GetReferenceCount() - 1;
		lValueCount[lIndex]++;
	}

	// Sorting step 4: Calculate the insert offsets for each value...
	int lTempOffset = 0;
	for (i = 0; i < lMaxRefCount; i++)
	{
		lOffset[i] = lTempOffset;
		lTempOffset += lValueCount[i];
	}

	// Sorting step 5: Sort the array...
	for (i = 0; i < pNumLinks; i++)
	{
		int lIndex = lNodeArray[i]->mColorOctreeNode->GetReferenceCount() - 1;
		int lInsertIndex = lOffset[lIndex];
		lOffset[lIndex]++;

		lSortedNodeArray[lInsertIndex] = lNodeArray[i];
	}

	// Sorting step 6: Put it back in a linked list.

	for (i = 0; i < pNumLinks; i++)
	{
		if (i == 0)
		{
			*pFirstLink = lSortedNodeArray[i];
			lCurrentNode = *pFirstLink;
		}
		else
		{
			lCurrentNode->mNext = lSortedNodeArray[i];
			lCurrentNode = lCurrentNode->mNext;
		}

		lCurrentNode->mNext = 0;
	}

	delete[] lNodeArray;
	delete[] lSortedNodeArray;
}


void Canvas::CrunchOctree(ColorOctreeNode* pOctTopNode, ColorOctreeNodeListNode** pFirstLink)
{
	/*
		Crunching the octree means that all the leaf nodes at the lowest level are removed
		and letting their parent node hold their mean color value instead.

		Then the parent node is stored in a linked list, the "level above" list. When all
		leaf nodes are removed from the "current level" linked list, the "current level" list
		is replaced by the "level above" list, and the procedure starts all over again.

		This continues until the number of colors goes below 257.

		This method is not as fast as how fast Photoshop does it, and the result is not as good...
		But hey! This is the first time I ever wrote something like this! And hopefully, I'm never going
		to do it again!
	*/


	// This node is holding the first node in the "level above" list.
	ColorOctreeNodeListNode* lFirstNodeInLevelAbove = 0;
	int lNumLinks = 0;

	int lNumColors = pOctTopNode->GetNumLevelXChildren(8);

	while (lNumColors > 256)
	{
		// Check if this list (the "current level" list) is empty.
		if (*pFirstLink != 0)
		{
			ColorOctreeNodeListNode* lNode = *pFirstLink;

			// Unlink the first node.
			*pFirstLink = (*pFirstLink)->mNext;

			if (lNode != 0)
			{
				// Crunch this node and decrease the color count.
				lNumColors -= lNode->mColorOctreeNode->CrunchChildren();

				// Now put the parent to this node in the "level above" list.
				// If the tag is set to 'true', then the parent node is already in the list.
				if (lNode->mColorOctreeNode->GetParent()->mTag == false)
				{
					lNumLinks++;

					ColorOctreeNodeListNode* lNewNode = new ColorOctreeNodeListNode();
					lNewNode->mNext = 0;
					lNewNode->mColorOctreeNode = lNode->mColorOctreeNode->GetParent();
					lNewNode->mColorOctreeNode->mTag = true;

					if (lFirstNodeInLevelAbove == 0)
					{
						lFirstNodeInLevelAbove = lNewNode;
					}
					else
					{
						lNewNode->mNext = lFirstNodeInLevelAbove;
						lFirstNodeInLevelAbove = lNewNode;
					}
				}

				delete lNode;
			}
		}
		else
		{
			// The "current level" list is empty, and we are done with this level.

			// Set the "level above" list as the "current level" list.
			*pFirstLink = lFirstNodeInLevelAbove;

			// Start all over again with the "level above" list.
			lFirstNodeInLevelAbove = 0;

			// Sort the "current level" list.
			SortLinkedList(pFirstLink, lNumLinks);

			lNumLinks = 0;
		}
	}
}



void Canvas::ConvertTo8()
{
	ConvertTo24();

	// Create an octree.
	ColorOctreeNode* lOctTopNode = new ColorOctreeNode();

	// Create a linked list to hold the next to the lowest level octree nodes.
	ColorOctreeNodeListNode* lFirstListNode = new ColorOctreeNodeListNode();

	// Step 1: Generate the octree and the linked list.
	int lNumLinks = GenerateOctreeAndList(lOctTopNode, &lFirstListNode);

	// Step 2: Sort the linked list by the octreenodes's reference count, using a bucketsort algorithm.
	SortLinkedList(&lFirstListNode, lNumLinks);

	// Step 3: Crunch the octree until the number of colors goes below 256.
	CrunchOctree(lOctTopNode, &lFirstListNode);

	// Delete all nodes in the list.
	ColorOctreeNodeListNode* lCurrentNode;
	ColorOctreeNodeListNode* lPrevNode;

	lCurrentNode = lFirstListNode;
	lPrevNode = 0;

	while (lCurrentNode != 0)
	{
		lPrevNode = lCurrentNode;
		lCurrentNode = lCurrentNode->mNext;
		delete lPrevNode;
	}

	// Step 4: Generate palette.
	lOctTopNode->GetPalette(mPalette, 0);

	// Step 5: Generate 8 bit Image.
	uint8* lImage = new uint8[mWidth * mHeight];
	unsigned i;
	int lSrcOffset = 0;

	for (i = 0; i < mWidth * mHeight; i++)
	{
		uint8 b = ((uint8*)mBuffer)[lSrcOffset + 0];
		uint8 g = ((uint8*)mBuffer)[lSrcOffset + 1];
		uint8 r = ((uint8*)mBuffer)[lSrcOffset + 2];

		lImage[i] = (uint8)lOctTopNode->GetPaletteEntry(r, g, b);

		lSrcOffset += 3;
	}

	if (mBufferResponsibility == true)
	{
		delete[] (char*)mBuffer;
	}
	mBufferResponsibility = true;
	mBuffer = lImage;

	mBitDepth = BITDEPTH_8_BIT;

	delete lOctTopNode;
}
/*
uint8 Canvas::GetPaletteColor(int pRed, int pGreen, int pBlue)
{
	int	lBestColorIndex;
	int	i;

	float32 lBestDist;
	float32 lDist;
	float32 lDeltaRed;
	float32 lDeltaGreen;
	float32 lDeltaBlue;

	if (mPalette == 0)
	{
		return 0;
	}

	for (i = 0; i < 256; i++)
	{
		lDeltaRed   = (float32)(pRed   - (int)mPalette[i].mRed);
		lDeltaGreen = (float32)(pGreen - (int)mPalette[i].mGreen);
		lDeltaBlue  = (float32)(pBlue  - (int)mPalette[i].mBlue);

		lDist = (float32)sqrt(lDeltaRed * lDeltaRed +
									 lDeltaGreen * lDeltaGreen +
									 lDeltaBlue * lDeltaBlue);

		if (i == 0 || lDist < lBestDist)
		{
			lBestColorIndex = i;
			lBestDist = lDist;
		}
	}

	return (uint8)lBestColorIndex;
}
*/

void Canvas::GetPixelColor(unsigned x, unsigned y, Color& pColor) const
{
	if (x >= 0 && x < mWidth && y >= 0 && y < mHeight && mBuffer != 0)
	{
		switch(mBitDepth)
		{
		case BITDEPTH_8_BIT:
			{
				uint8 lIndex = ((uint8*)mBuffer)[y * mPitch + x];
				pColor = mPalette[lIndex];
			}
			break;
		case BITDEPTH_15_BIT:
			{
				uint16 lColor = ((uint16*)mBuffer)[y * mPitch + x];
				pColor.mRed   = (uint8)((lColor >> 10) << 3);
				pColor.mGreen = (uint8)(((lColor >> 5) & 31) << 3);
				pColor.mBlue  = (uint8) ((lColor & 31) << 3);
			}
			break;
		case BITDEPTH_16_BIT:
			{
				uint16 lColor = ((uint16*)mBuffer)[y * mPitch + x];
				pColor.mRed   = (uint8)((lColor >> 11) << 3);
				pColor.mGreen = (uint8)(((lColor >> 5) & 63) << 2);
				pColor.mBlue  = (uint8) ((lColor & 31) << 3);
			}
			break;
		case BITDEPTH_24_BIT:
			{
				unsigned lOffset = (y * mPitch + x) * 3;
				pColor.mBlue  = ((uint8*)mBuffer)[lOffset + 0];
				pColor.mGreen = ((uint8*)mBuffer)[lOffset + 1];
				pColor.mRed   = ((uint8*)mBuffer)[lOffset + 2];
			}
			break;
		case BITDEPTH_32_BIT:
			{
				unsigned lOffset = (y * mPitch + x) * 4;
				pColor.mBlue  = ((uint8*)mBuffer)[lOffset + 0];
				pColor.mGreen = ((uint8*)mBuffer)[lOffset + 1];
				pColor.mRed   = ((uint8*)mBuffer)[lOffset + 2];
				pColor.mAlpha = ((uint8*)mBuffer)[lOffset + 3];
			}
			break;
		case BITDEPTH_16_BIT_PER_CHANNEL:
			{
				unsigned lOffset = (y * mPitch + x) * 3;
				pColor.mBlue  = GammaLookup::LinearToGamma(((uint16*)mBuffer)[lOffset + 0]);
				pColor.mGreen = GammaLookup::LinearToGamma(((uint16*)mBuffer)[lOffset + 1]);
				pColor.mRed   = GammaLookup::LinearToGamma(((uint16*)mBuffer)[lOffset + 2]);
			}
			break;
		case BITDEPTH_32_BIT_PER_CHANNEL:
			{
				unsigned lOffset = (y * mPitch + x) * 3;
				pColor.mBlue  = (uint8)floor(GammaLookup::LinearToGammaF(((float32*)mBuffer)[lOffset + 0]) * 255.0f);
				pColor.mGreen = (uint8)floor(GammaLookup::LinearToGammaF(((float32*)mBuffer)[lOffset + 1]) * 255.0f);
				pColor.mRed   = (uint8)floor(GammaLookup::LinearToGammaF(((float32*)mBuffer)[lOffset + 2]) * 255.0f);
			}
			break;
		default:
			break;
		}
	}
}

void Canvas::SetPixelColor(unsigned x, unsigned y, Color& pColor)
{
	if (x >= 0 && x < mWidth && y >= 0 && y < mHeight && mBuffer != 0)
	{
		switch(mBitDepth)
		{
		case BITDEPTH_8_BIT:
			{
				((uint8*)mBuffer)[y * mPitch + x] = pColor.mAlpha;
			}
			break;
		case BITDEPTH_15_BIT:
			{
				uint16 r = (uint16)pColor.mRed;
				uint16 g = (uint16)pColor.mGreen;
				uint16 b = (uint16)pColor.mBlue;
				uint16 lColor = ((r >> 3) << 10) | ((g >> 3) << 5) | (b >> 3);
				((uint16*)mBuffer)[y * mPitch + x] = lColor;
			}
			break;
		case BITDEPTH_16_BIT:
			{
				uint16 r = (uint16)pColor.mRed;
				uint16 g = (uint16)pColor.mGreen;
				uint16 b = (uint16)pColor.mBlue;
				uint16 lColor = ((r >> 3) << 11) | ((g >> 2) << 6) | (b >> 3);
				((uint16*)mBuffer)[y * mPitch + x] = lColor;
			}
			break;
		case BITDEPTH_24_BIT:
			{
				unsigned lOffset = (y * mPitch + x) * 3;
				((uint8*)mBuffer)[lOffset + 0] = pColor.mBlue;
				((uint8*)mBuffer)[lOffset + 1] = pColor.mGreen;
				((uint8*)mBuffer)[lOffset + 2] = pColor.mRed;
			}
			break;
		case BITDEPTH_32_BIT:
			{
				unsigned lOffset = (y * mPitch + x) * 4;
				((uint8*)mBuffer)[lOffset + 0] = pColor.mBlue;
				((uint8*)mBuffer)[lOffset + 1] = pColor.mGreen;
				((uint8*)mBuffer)[lOffset + 2] = pColor.mRed;
				((uint8*)mBuffer)[lOffset + 3] = pColor.mAlpha;
			}
			break;
		case BITDEPTH_16_BIT_PER_CHANNEL:
			{
				unsigned lOffset = (y * mPitch + x) * 3;
				((uint16*)mBuffer)[lOffset + 0] = GammaLookup::GammaToLinear(pColor.mBlue);
				((uint16*)mBuffer)[lOffset + 1] = GammaLookup::GammaToLinear(pColor.mGreen);
				((uint16*)mBuffer)[lOffset + 2] = GammaLookup::GammaToLinear(pColor.mRed);
			}
			break;
		case BITDEPTH_32_BIT_PER_CHANNEL:
			{
				unsigned lOffset = (y * mPitch + x) * 3;
				((float32*)mBuffer)[lOffset + 0] = GammaLookup::GammaToLinearF((float32)pColor.mBlue  / 255.0f);
				((float32*)mBuffer)[lOffset + 1] = GammaLookup::GammaToLinearF((float32)pColor.mGreen / 255.0f);
				((float32*)mBuffer)[lOffset + 2] = GammaLookup::GammaToLinearF((float32)pColor.mRed   / 255.0f);
			}
			break;
		default:
			break;
		}
	}
}

void Canvas::Clear()
{
	int lLineOffset = mPitch * mPixelSize;
	int lLineSize   = mWidth * mPixelSize;
	uint8* lBuffer = (uint8*)mBuffer;
	for (unsigned y = 0; y < mHeight; y++)
	{
		memset(&lBuffer[y * lLineOffset], 0, lLineSize);
	}
}

void Canvas::PremultiplyAlpha()
{
	if (mBitDepth == BITDEPTH_32_BIT && mBufferResponsibility == true)
	{
		int lImageSize = mPitch * mHeight;
		for (int i = 0; i < lImageSize * 4; i += 4)
		{
			unsigned b = (unsigned)((uint8*)mBuffer)[i + 0];
			unsigned g = (unsigned)((uint8*)mBuffer)[i + 1];
			unsigned r = (unsigned)((uint8*)mBuffer)[i + 2];
			unsigned a = (unsigned)((uint8*)mBuffer)[i + 3];

			((uint8*)mBuffer)[i + 0] = (uint8)((b * a) / 255);
			((uint8*)mBuffer)[i + 1] = (uint8)((g * a) / 255);
			((uint8*)mBuffer)[i + 2] = (uint8)((r * a) / 255);
		}
	}
}

void Canvas::SetAlpha(uint8 pTrue, uint8 pFalse, uint8 pCompareValue, CmpFunc pFunc)
{
	if (mBitDepth != BITDEPTH_32_BIT/* || mBufferResponsibility == false*/)
	{
		return;
	}

	int lImageSize = mPitch * mHeight;
	int i;

	bool lGray = false;
	int lChannel = 0;

	switch(pFunc)
	{
	case CMP_RED_GREATER:
	case CMP_RED_GREATER_OR_EQUAL:
	case CMP_RED_LESS:
	case CMP_RED_LESS_OR_EQUAL:
	case CMP_RED_EQUAL:
		lChannel = 2;
		break;
	case CMP_GREEN_GREATER:
	case CMP_GREEN_GREATER_OR_EQUAL:
	case CMP_GREEN_LESS:
	case CMP_GREEN_LESS_OR_EQUAL:
	case CMP_GREEN_EQUAL:
		lChannel = 2;
		break;
	case CMP_BLUE_GREATER:
	case CMP_BLUE_GREATER_OR_EQUAL:
	case CMP_BLUE_LESS:
	case CMP_BLUE_LESS_OR_EQUAL:
	case CMP_BLUE_EQUAL:
		lChannel = 2;
		break;
	case CMP_ALPHA_GREATER:
	case CMP_ALPHA_GREATER_OR_EQUAL:
	case CMP_ALPHA_LESS:
	case CMP_ALPHA_LESS_OR_EQUAL:
	case CMP_ALPHA_EQUAL:
		lChannel = 3;
		break;
	case CMP_GRAY_GREATER:
	case CMP_GRAY_GREATER_OR_EQUAL:
	case CMP_GRAY_LESS:
	case CMP_GRAY_LESS_OR_EQUAL:
	case CMP_GRAY_EQUAL:
		lGray = true;
		break;
	case CMP_ALWAYS_TRUE:
		break;
	}

	if (lGray == true)
	{
		for (i = 0; i < lImageSize * 4; i += 4)
		{
			float32 b = (float32)GammaLookup::GammaToLinear(((uint8*)mBuffer)[i + 0]);
			float32 g = (float32)GammaLookup::GammaToLinear(((uint8*)mBuffer)[i + 1]);
			float32 r = (float32)GammaLookup::GammaToLinear(((uint8*)mBuffer)[i + 2]);

			// The magic formula: 30% Red, 59% Green and 11% Blue.
			uint8 ch = (uint8)GammaLookup::LinearToGamma((uint16)(r * 0.3f + g * 0.59f + b * 0.11f));

			if (Compare(pFunc, ch, pCompareValue) == true)
				((uint8*)mBuffer)[i + 3] = pTrue;
			else
				((uint8*)mBuffer)[i + 3] = pFalse;
		}
	}
	else
	{
		for (i = 0; i < lImageSize * 4; i += 4)
		{
			uint8 ch = (uint8)((uint8*)mBuffer)[i + lChannel];

			if (Compare(pFunc, ch, pCompareValue) == true)
				((uint8*)mBuffer)[i + 3] = pTrue;
			else
				((uint8*)mBuffer)[i + 3] = pFalse;
		}
	}
}

bool Canvas::Compare(CmpFunc pCmpFunc, uint8 pValue1, uint8 pValue2)
{
	switch(pCmpFunc)
	{
	case CMP_RED_GREATER:
	case CMP_GREEN_GREATER:
	case CMP_BLUE_GREATER:
	case CMP_ALPHA_GREATER:
	case CMP_GRAY_GREATER:
		return pValue1 > pValue2;
	case CMP_RED_GREATER_OR_EQUAL:
	case CMP_GREEN_GREATER_OR_EQUAL:
	case CMP_BLUE_GREATER_OR_EQUAL:
	case CMP_ALPHA_GREATER_OR_EQUAL:
	case CMP_GRAY_GREATER_OR_EQUAL:
		return pValue1 >= pValue2;
	case CMP_RED_LESS:
	case CMP_GREEN_LESS:
	case CMP_BLUE_LESS:
	case CMP_ALPHA_LESS:
	case CMP_GRAY_LESS:
		return pValue1 < pValue2;
	case CMP_RED_LESS_OR_EQUAL:
	case CMP_GREEN_LESS_OR_EQUAL:
	case CMP_BLUE_LESS_OR_EQUAL:
	case CMP_ALPHA_LESS_OR_EQUAL:
	case CMP_GRAY_LESS_OR_EQUAL:
		return pValue1 <= pValue2;
	case CMP_RED_EQUAL:
	case CMP_GREEN_EQUAL:
	case CMP_BLUE_EQUAL:
	case CMP_ALPHA_EQUAL:
	case CMP_GRAY_EQUAL:
		return pValue1 == pValue2;
	case CMP_ALWAYS_TRUE:
		return true;
	default:
		return false;
	}
}

void* Canvas::GetBuffer() const
{
	return mBuffer;
}

Canvas::BitDepth Canvas::IntToBitDepth(unsigned pBitDepth)
{
	if (pBitDepth <= 8)
	{
		return BITDEPTH_8_BIT;
	}
	else if(pBitDepth <= 15)
	{
		return BITDEPTH_15_BIT;
	}
	else if(pBitDepth == 16)
	{
		return BITDEPTH_16_BIT;
	}
	else if(pBitDepth <= 24)
	{
		return BITDEPTH_24_BIT;
	}
	else if(pBitDepth <= 32)
	{
		return BITDEPTH_32_BIT;
	}
	else if(pBitDepth <= 48)
	{
		return BITDEPTH_16_BIT_PER_CHANNEL;
	}
	else if(pBitDepth <= 96)
	{
		return BITDEPTH_32_BIT_PER_CHANNEL;
	}

	return BITDEPTH_8_BIT;
}

unsigned Canvas::BitDepthToInt(BitDepth pBitDepth)
{
	switch(pBitDepth)
	{
	case BITDEPTH_8_BIT:
		return 8;
	case BITDEPTH_15_BIT:
		return 15;
	case BITDEPTH_16_BIT:
		return 16;
	case BITDEPTH_24_BIT:
		return 24;
	case BITDEPTH_32_BIT:
		return 32;
	case BITDEPTH_16_BIT_PER_CHANNEL:
		return 48;
	case BITDEPTH_32_BIT_PER_CHANNEL:
		return 96;
	}

	return 8;
}

Canvas::BitDepth Canvas::GetBitDepth() const
{
	return mBitDepth;
}

unsigned Canvas::GetWidth() const
{
	return mWidth;
}

unsigned Canvas::GetHeight() const
{
	return mHeight;
}

unsigned Canvas::GetPixelByteSize() const
{
	return mPixelSize;
}

unsigned Canvas::GetBufferByteSize() const
{
	return mPitch * mHeight * mPixelSize;
}

void Canvas::SetPitch(unsigned pPitch)
{
	mPitch = pPitch;
}

unsigned Canvas::GetPitch() const
{
	return mPitch;
}

const Color* Canvas::GetPalette() const
{
	return mPalette;
}

unsigned Canvas::GetPow2Width()
{
	return PowerUp(GetWidth());
}

unsigned Canvas::GetPow2Height()
{
	return PowerUp(GetHeight());
}

void Canvas::operator= (const Canvas& pCanvas)
{
	Copy(pCanvas);
}

Color Canvas::GetPixelColor(unsigned x, unsigned y) const
{
	Color lColor;
	GetPixelColor(x, y, lColor);
	return lColor;
}







/////////////////////////////////////////////////////
//                                                 //
//             Class ColorOctreeNode               //
//                                                 //
/////////////////////////////////////////////////////



ColorOctreeNode::ColorOctreeNode()
{
	mRed = 0;
	mGreen = 0;
	mBlue = 0;

	mReferenceCount = 0;
	mNumChildren = 0;
	mLevel = -1;

	mTag = false;

	int i;

	for (i = 0; i < 8; i++)
	{
		mChildren[i] = 0;
	}

	mParent = 0;
}



ColorOctreeNode::~ColorOctreeNode()
{
	int i;

	for (i = 0; i < 8; i++)
	{
		if (mChildren[i] != 0)
		{
			delete mChildren[i];
		}
	}
}



int ColorOctreeNode::GetNumLevelXChildren(int pLevel)
{
	if (pLevel <= mLevel)
	{
		return 0;
	}

	if (pLevel == (mLevel + 1))
	{
		return mNumChildren;
	}

	int i;
	int lCount = 0;

	for (i = 0; i < 8; i++)
	{
		if (mChildren[i] != 0)
		{
			lCount += mChildren[i]->GetNumLevelXChildren(pLevel);
		}
	}

	return lCount;
}

int ColorOctreeNode::GetChildOffset(uint8 r, uint8 g, uint8 b, int pLevel)
{
	int lBitMask = (1 << (7 - pLevel));
	int x = 0;
	int y = 0;
	int z = 0;

	if ((r & lBitMask) != 0)
	{
		x = 1;
	}
	if ((g & lBitMask) != 0)
	{
		y = 1;
	}
	if ((b & lBitMask) != 0)
	{
		z = 1;
	}

	return((z << 2) + (y << 1) + x);
}

ColorOctreeNode* ColorOctreeNode::InsertColor(uint8 r, uint8 g, uint8 b, int pLevel)
{
	mLevel = pLevel;
	mReferenceCount++;

	if (pLevel < 8)
	{
		int lChildOffset = GetChildOffset(r, g, b, pLevel);

		if (mChildren[lChildOffset] == 0)
		{
			mChildren[lChildOffset] = new ColorOctreeNode();
			mChildren[lChildOffset]->mParent = this;
			mNumChildren++;
		}

		return mChildren[lChildOffset]->InsertColor(r, g, b, pLevel + 1);
	}
	else
	{
		// Level is 8, which is the last level.
		mRed = r;
		mGreen = g;
		mBlue = b;

		// Only return a valid value if this is the parents first reference.
		if (mParent->mReferenceCount == 1)
		{
			return mParent;
		}
		else
		{
			return 0;
		}
	}
}

bool ColorOctreeNode::InsertUniqueColor(uint8 r, uint8 g, uint8 b, int pLevel)
{
	mLevel = pLevel;

	if (pLevel < 8)
	{
		int lChildOffset = GetChildOffset(r, g, b, pLevel);

		if (mChildren[lChildOffset] == 0)
		{
			mChildren[lChildOffset] = new ColorOctreeNode();
			mChildren[lChildOffset]->mParent = this;
			mNumChildren++;
		}

		if (mChildren[lChildOffset]->InsertUniqueColor(r, g, b, pLevel + 1) == true)
		{
			mReferenceCount++;
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		// Level is 8, which is the last level.
		mRed = r;
		mGreen = g;
		mBlue = b;

		// Only return a valid value if this is the first reference.
		if (mReferenceCount == 0)
		{
			mReferenceCount++;
			return true;
		}
		else
		{
			return 0;
		}
	}

}

bool ColorOctreeNode::RemoveColor(uint8 r, uint8 g, uint8 b)
{
	if (mNumChildren == 0)
	{
		return false;
	}

	int lChildOffset = GetChildOffset(r, g, b, mLevel);

	if (mChildren[lChildOffset] != 0)
	{
		if (mChildren[lChildOffset]->mLevel == 8 &&
			mChildren[lChildOffset]->mRed == r &&
			mChildren[lChildOffset]->mGreen == g &&
			mChildren[lChildOffset]->mBlue == b)
		{
			mChildren[lChildOffset]->mReferenceCount--;

			if (mChildren[lChildOffset]->mReferenceCount <= 0)
			{
				delete mChildren[lChildOffset];
				mChildren[lChildOffset] = 0;
				return true;
			}
		}
		else
		{
			return mChildren[lChildOffset]->RemoveColor(r, g, b);
		}
	}

	return false;
}


int ColorOctreeNode::CrunchChildren()
{
	if (mNumChildren == 0)
	{
		return 0;
	}

	int i;
	int lNumChildren = mNumChildren;
	int lColorsRemoved = 0;

	// Step 1: Crunch recursive.
	for (i = 0; i < 8; i++)
	{
		if (mChildren[i] != 0)
		{
			lColorsRemoved += mChildren[i]->CrunchChildren();
		}
	}

	// Step 2: Get the mean color of all child nodes and the reference sum. Delete the children.
	int r = 0;
	int g = 0;
	int b = 0;
	int lRefSum = 0;

	for (i = 0; i < 8; i++)
	{
		if (mChildren[i] != 0)
		{
			r += mChildren[i]->mRed;
			g += mChildren[i]->mGreen;
			b += mChildren[i]->mBlue;

			lRefSum += mChildren[i]->mReferenceCount;

			delete mChildren[i];
			mChildren[i] = 0;

			mNumChildren--;
		}
	}

	r /= lNumChildren;
	g /= lNumChildren;
	b /= lNumChildren;

	mRed = (uint8)r;
	mGreen = (uint8)g;
	mBlue = (uint8)b;

	// Return the number of children crunched.
	lColorsRemoved += (lNumChildren - 1);
	return lColorsRemoved;
}



int ColorOctreeNode::GetPalette(Color* pPalette, int pIndex)
{
	int i;

	if (mNumChildren > 0)
	{
		for (i = 0; i < 8; i++)
		{
			if (mChildren[i] != 0)
			{
				pIndex = mChildren[i]->GetPalette(pPalette, pIndex);
			}
		}

		return pIndex;
	}
	else
	{
		pPalette[pIndex].mRed = mRed;
		pPalette[pIndex].mGreen = mGreen;
		pPalette[pIndex].mBlue = mBlue;

		// Save this index value for later use.
		mPaletteIndex = (uint8)pIndex;

		return pIndex + 1;
	}
}



int ColorOctreeNode::GetPaletteEntry(uint8 r, uint8 g, uint8 b)
{
	if (mNumChildren == 0)
	{
		return mPaletteIndex;
	}

	int lChildOffset = GetChildOffset(r, g, b, mLevel);

	if (mChildren[lChildOffset] != 0)
	{
		return mChildren[lChildOffset]->GetPaletteEntry(r, g, b);
	}

	return 0;
}



/////////////////////////////////////////////////////
//                                                 //
//          Class ColorOctreeNodeListNode          //
//                                                 //
/////////////////////////////////////////////////////



ColorOctreeNodeListNode::ColorOctreeNodeListNode()
{
	mColorOctreeNode = 0;
	mNext = 0;
}

} // End namespace.
