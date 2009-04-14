/*
	Lepra::File:   Software3DPainter_Clip.cpp
	Class:  Software3DPainter
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand
*/

#include "../../Include/UiTBC.h"
#include "../../Include/UiSoftware3DPainter.h"

#include "../../Include/UiVertex.h"

namespace UiTbc
{

int Software3DPainter::ClipPolygon(const PointData* pInputPoint, int pNumPoints,
				   PointData* pOutputPoint,
				   ClipFlags pFlags,
				   float pPlaneA, float pPlaneB, float pPlaneC, float pPlaneD,
				   bool pClosedLoop)
{
	int lVertex3DCount = 0;
	int lStart = pNumPoints - 1;
	int lEnd = 0;

	if (pClosedLoop == false)
	{
		lStart = 0;
		lEnd = 1;
	}

	while (lEnd < pNumPoints)
	{
		const PointData* lStartPoint = &pInputPoint[lStart];
		const PointData* lEndPoint   = &pInputPoint[lEnd];

		float lSX = lStartPoint->mXYZ->mTranslatedX;
		float lSY = lStartPoint->mXYZ->mTranslatedY;
		float lSZ = lStartPoint->mXYZ->mTranslatedZ;
		float lSU = lStartPoint->mUVM->u;
		float lSV = lStartPoint->mUVM->v;
		float lSM = lStartPoint->mUVM->m;
		float lSR = lStartPoint->mRGB->mLightProcessedR;
		float lSG = lStartPoint->mRGB->mLightProcessedG;
		float lSB = lStartPoint->mRGB->mLightProcessedB;
		float lEX = lEndPoint->mXYZ->mTranslatedX;
		float lEY = lEndPoint->mXYZ->mTranslatedY;
		float lEZ = lEndPoint->mXYZ->mTranslatedZ;
		float lEU = lEndPoint->mUVM->u;
		float lEV = lEndPoint->mUVM->v;
		float lEM = lEndPoint->mUVM->m;
		float lER = lEndPoint->mRGB->mLightProcessedR;
		float lEG = lEndPoint->mRGB->mLightProcessedG;
		float lEB = lEndPoint->mRGB->mLightProcessedB;

		float lS = lSX * pPlaneA + lSY * pPlaneB + lSZ * pPlaneC + pPlaneD;
		float lE = lEX * pPlaneA + lEY * pPlaneB + lEZ * pPlaneC + pPlaneD;

		if (lS < 0)
		{
			if (lE >= 0)
			{
				// Exiting the clipping plane. Start point clipped, store start point.
				const float lT = -lS / (lE - lS);

				pOutputPoint[lVertex3DCount].mXYZ->mTranslatedX = lSX + (lEX - lSX) * lT;
				pOutputPoint[lVertex3DCount].mXYZ->mTranslatedY = lSY + (lEY - lSY) * lT;
				pOutputPoint[lVertex3DCount].mXYZ->mTranslatedZ = lSZ + (lEZ - lSZ) * lT;

				if ((pFlags & CLIP_UV) != 0)
				{
					pOutputPoint[lVertex3DCount].mUVM->u = lSU + (lEU - lSU) * lT;
					pOutputPoint[lVertex3DCount].mUVM->v = lSV + (lEV - lSV) * lT;
					pOutputPoint[lVertex3DCount].mUVM->m = lSM + (lEM - lSM) * lT;
				}
				if ((pFlags & CLIP_RGB) != 0)
				{
					pOutputPoint[lVertex3DCount].mRGB->mLightProcessedR = lSR + (lER - lSR) * lT;
					pOutputPoint[lVertex3DCount].mRGB->mLightProcessedG = lSG + (lEG - lSG) * lT;
					pOutputPoint[lVertex3DCount].mRGB->mLightProcessedB = lSB + (lEB - lSB) * lT;
				}
				lVertex3DCount++;
			}
		}
		else
		{
			if (lE < 0)
			{
				// Entering the clipping plane. Store both points. The first point is just copied.
				pOutputPoint[lVertex3DCount].mXYZ->mTranslatedX = lSX;
				pOutputPoint[lVertex3DCount].mXYZ->mTranslatedY = lSY;
				pOutputPoint[lVertex3DCount].mXYZ->mTranslatedZ = lSZ;

				if ((pFlags & CLIP_UV) != 0)
				{
					pOutputPoint[lVertex3DCount].mUVM->u = lSU;
					pOutputPoint[lVertex3DCount].mUVM->v = lSV;
					pOutputPoint[lVertex3DCount].mUVM->m = lSM;
				}
				if ((pFlags & CLIP_RGB) != 0)
				{
					pOutputPoint[lVertex3DCount].mRGB->mLightProcessedR = lSR;
					pOutputPoint[lVertex3DCount].mRGB->mLightProcessedG = lSG;
					pOutputPoint[lVertex3DCount].mRGB->mLightProcessedB = lSB;
				}
				lVertex3DCount++;

				// To avoid glitches between two adjacent triangles we need to perform the EXACT
				// same calculations as the neighbour triangle on the same edge. Thus, invert
				// the calculation compared to when exiting the clipping plane.
				const float lT = -lE / (lS - lE);

				pOutputPoint[lVertex3DCount].mXYZ->mTranslatedX = lEX + (lSX - lEX) * lT;
				pOutputPoint[lVertex3DCount].mXYZ->mTranslatedY = lEY + (lSY - lEY) * lT;
				pOutputPoint[lVertex3DCount].mXYZ->mTranslatedZ = lEZ + (lSZ - lEZ) * lT;

				if ((pFlags & CLIP_UV) != 0)
				{
					pOutputPoint[lVertex3DCount].mUVM->u = lEU + (lSU - lEU) * lT;
					pOutputPoint[lVertex3DCount].mUVM->v = lEV + (lSV - lEV) * lT;
					pOutputPoint[lVertex3DCount].mUVM->m = lEM + (lSM - lEM) * lT;
				}
				if ((pFlags & CLIP_RGB) != 0)
				{
					pOutputPoint[lVertex3DCount].mRGB->mLightProcessedR = lER + (lSR - lER) * lT;
					pOutputPoint[lVertex3DCount].mRGB->mLightProcessedG = lEG + (lSG - lEG) * lT;
					pOutputPoint[lVertex3DCount].mRGB->mLightProcessedB = lEB + (lSB - lEB) * lT;
				}
				lVertex3DCount++;
			}
			else
			{
				// Both points are visible, store the start point.
				pOutputPoint[lVertex3DCount].mXYZ->mTranslatedX = lSX;
				pOutputPoint[lVertex3DCount].mXYZ->mTranslatedY = lSY;
				pOutputPoint[lVertex3DCount].mXYZ->mTranslatedZ = lSZ;

				if ((pFlags & CLIP_UV) != 0)
				{
					pOutputPoint[lVertex3DCount].mUVM->u = lSU;
					pOutputPoint[lVertex3DCount].mUVM->v = lSV;
					pOutputPoint[lVertex3DCount].mUVM->m = lSM;
				}
				if ((pFlags & CLIP_RGB) != 0)
				{
					pOutputPoint[lVertex3DCount].mRGB->mLightProcessedR = lSR;
					pOutputPoint[lVertex3DCount].mRGB->mLightProcessedG = lSG;
					pOutputPoint[lVertex3DCount].mRGB->mLightProcessedB = lSB;
				}
				lVertex3DCount++;
			}
		}

		lStart = lEnd;
		lEnd++;
	}// End while(lEnd < pNumPoints)

	return lVertex3DCount;
}



int Software3DPainter::ClipLine(const PointData* pInputPoint1,
				const PointData* pInputPoint2,
				PointData* pOutputPoint1,
				PointData* pOutputPoint2,
				ClipFlags pFlags)
{
	float lX1 = pInputPoint1->mXYZ->x;
	float lY1 = pInputPoint1->mXYZ->y;
	float lX2 = pInputPoint2->mXYZ->x;
	float lY2 = pInputPoint2->mXYZ->y;

	// If totally inside the screen.
	if (lX1 >= mClipLeft && 
	   lX1 <= mClipRight &&
	   lY1 >= mClipTop && 
	   lY1 <= mClipBottom &&
	   lX2 >= mClipLeft && 
	   lX2 <= mClipRight &&
	   lY2 >= mClipTop && 
	   lY2 <= mClipBottom)
	{
		pOutputPoint1->mXYZ->x = pInputPoint1->mXYZ->x;
		pOutputPoint1->mXYZ->y = pInputPoint1->mXYZ->y;
		pOutputPoint1->mXYZ->z = pInputPoint1->mXYZ->z;
		pOutputPoint2->mXYZ->x = pInputPoint2->mXYZ->x;
		pOutputPoint2->mXYZ->y = pInputPoint2->mXYZ->y;
		pOutputPoint2->mXYZ->z = pInputPoint2->mXYZ->z;

		if ((pFlags & CLIP_UV) != 0)
		{
			pOutputPoint1->mUVM->u = pInputPoint1->mUVM->u;
			pOutputPoint1->mUVM->v = pInputPoint1->mUVM->v;
			pOutputPoint2->mUVM->u = pInputPoint2->mUVM->u;
			pOutputPoint2->mUVM->v = pInputPoint2->mUVM->v;
		}

		 if((pFlags & CLIP_RGB) != 0)
		 {
			 pOutputPoint1->mRGB->r = pInputPoint1->mRGB->r;
			 pOutputPoint1->mRGB->g = pInputPoint1->mRGB->g;
			 pOutputPoint1->mRGB->b = pInputPoint1->mRGB->b;
			 pOutputPoint2->mRGB->r = pInputPoint2->mRGB->r;
			 pOutputPoint2->mRGB->g = pInputPoint2->mRGB->g;
			 pOutputPoint2->mRGB->b = pInputPoint2->mRGB->b;
		 }

		 return 2;
	}


	DataXYZ lXYZ0[2];
	DataRGB lRGB0[2];
	DataUVM lUVM0[2];

	PointData lP0[2];

	lP0[0].mXYZ = &lXYZ0[0];
	lP0[0].mRGB = &lRGB0[0];
	lP0[0].mUVM = &lUVM0[0];

	lP0[1].mXYZ = &lXYZ0[1];
	lP0[1].mRGB = &lRGB0[1];
	lP0[1].mUVM = &lUVM0[1];

	if (ClipLineLeftX(pInputPoint1,
			 pInputPoint2,
			 &lP0[0],
			 &lP0[1],
			 pFlags) == 0)
	{
		return 0;
	}

	DataXYZ lXYZ1[2];
	DataRGB lRGB1[2];
	DataUVM lUVM1[2];

	PointData lP1[2];

	lP1[0].mXYZ = &lXYZ1[0];
	lP1[0].mRGB = &lRGB1[0];
	lP1[0].mUVM = &lUVM1[0];

	lP1[1].mXYZ = &lXYZ1[1];
	lP1[1].mRGB = &lRGB1[1];
	lP1[1].mUVM = &lUVM1[1];

	if (ClipLineRightX(&lP0[0],
			  &lP0[1],
			  &lP1[0],
			  &lP1[1],
			  pFlags) == 0)
	{
		return 0;
	}

	if (ClipLineTopY(&lP1[0],
			&lP1[1],
			&lP0[0],
			&lP0[1],
			pFlags) == 0)
	{
		return 0;
	}

	if (ClipLineBottomY(&lP0[0],
			   &lP0[1],
			   pOutputPoint1,
			   pOutputPoint2,
			   pFlags) == 0)
	{
		return 0;
	}

	return 2;
}

int Software3DPainter::ClipLineLeftX(const PointData* pInputPoint1,
				     const PointData* pInputPoint2,
				     PointData* pOutputPoint1,
				     PointData* pOutputPoint2,
				     ClipFlags pFlags)
{
	if (pInputPoint1->mXYZ->x < mClipLeft)
	{
		if (pInputPoint2->mXYZ->x < mClipLeft)
		{
			return 0;
		}
		else
		{
			const PointData* lTemp = pInputPoint1;
			pInputPoint1 = pInputPoint2;
			pInputPoint2 = lTemp;
		}
	}
	else
	{
		if (pInputPoint2->mXYZ->x > mClipLeft)
		{
			pOutputPoint1->mXYZ->x = pInputPoint1->mXYZ->x;
			pOutputPoint1->mXYZ->y = pInputPoint1->mXYZ->y;
			pOutputPoint1->mXYZ->z = pInputPoint1->mXYZ->z;
			pOutputPoint2->mXYZ->x = pInputPoint2->mXYZ->x;
			pOutputPoint2->mXYZ->y = pInputPoint2->mXYZ->y;
			pOutputPoint2->mXYZ->z = pInputPoint2->mXYZ->z;

			if ((pFlags & CLIP_UV) != 0)
			{
				pOutputPoint1->mUVM->u = pInputPoint1->mUVM->u;
				pOutputPoint1->mUVM->v = pInputPoint1->mUVM->v;
				pOutputPoint2->mUVM->u = pInputPoint2->mUVM->u;
				pOutputPoint2->mUVM->v = pInputPoint2->mUVM->v;
			}

			if ((pFlags & CLIP_RGB) != 0)
			{
				pOutputPoint1->mRGB->r = pInputPoint1->mRGB->r;
				pOutputPoint1->mRGB->g = pInputPoint1->mRGB->g;
				pOutputPoint1->mRGB->b = pInputPoint1->mRGB->b;
				pOutputPoint2->mRGB->r = pInputPoint2->mRGB->r;
				pOutputPoint2->mRGB->g = pInputPoint2->mRGB->g;
				pOutputPoint2->mRGB->b = pInputPoint2->mRGB->b;
			}

			return 2;
		}
	}

	const float lT = (mClipLeft - pInputPoint2->mXYZ->x) / 
			   (pInputPoint1->mXYZ->x - pInputPoint2->mXYZ->x);

	float lOneOverZ0 = 1.0f / pInputPoint2->mXYZ->z;
	float lOneOverZ1 = 1.0f / pInputPoint1->mXYZ->z;
	pOutputPoint1->mXYZ->x = mClipLeft;
	pOutputPoint1->mXYZ->y = pInputPoint2->mXYZ->y + (pInputPoint1->mXYZ->y - pInputPoint2->mXYZ->y) * lT;
	pOutputPoint1->mXYZ->z = 1.0f / (lOneOverZ0 + (lOneOverZ1 - lOneOverZ0) * lT);
	pOutputPoint2->mXYZ->x = pInputPoint1->mXYZ->x;
	pOutputPoint2->mXYZ->y = pInputPoint1->mXYZ->y;
	pOutputPoint2->mXYZ->z = pInputPoint1->mXYZ->z;

	if ((pFlags & CLIP_UV) != 0)
	{
		float lUOverZ0 = pInputPoint2->mUVM->u * lOneOverZ0;
		float lVOverZ0 = pInputPoint2->mUVM->v * lOneOverZ0;
		float lUOverZ1 = pInputPoint1->mUVM->u * lOneOverZ1;
		float lVOverZ1 = pInputPoint1->mUVM->v * lOneOverZ1;

		pOutputPoint1->mUVM->u = (lUOverZ0 + (lUOverZ1 - lUOverZ0) * lT) * pOutputPoint1->mXYZ->z;
		pOutputPoint1->mUVM->v = (lVOverZ0 + (lVOverZ1 - lVOverZ0) * lT) * pOutputPoint1->mXYZ->z;
		pOutputPoint2->mUVM->u = pInputPoint1->mUVM->u;
		pOutputPoint2->mUVM->v = pInputPoint1->mUVM->v;
	}

	if ((pFlags & CLIP_RGB) != 0)
	{
		float lROverZ0 = pInputPoint2->mRGB->r * lOneOverZ0;
		float lGOverZ0 = pInputPoint2->mRGB->g * lOneOverZ0;
		float lBOverZ0 = pInputPoint2->mRGB->b * lOneOverZ0;
		float lROverZ1 = pInputPoint1->mRGB->r * lOneOverZ1;
		float lGOverZ1 = pInputPoint1->mRGB->g * lOneOverZ1;
		float lBOverZ1 = pInputPoint1->mRGB->b * lOneOverZ1;

		pOutputPoint1->mRGB->r = (lROverZ0 + (lROverZ1 - lROverZ0) * lT) * pOutputPoint1->mXYZ->z;
		pOutputPoint1->mRGB->g = (lGOverZ0 + (lGOverZ1 - lGOverZ0) * lT) * pOutputPoint1->mXYZ->z;
		pOutputPoint1->mRGB->b = (lBOverZ0 + (lBOverZ1 - lBOverZ0) * lT) * pOutputPoint1->mXYZ->z;
		pOutputPoint2->mRGB->r = pInputPoint1->mRGB->r;
		pOutputPoint2->mRGB->g = pInputPoint1->mRGB->g;
		pOutputPoint2->mRGB->b = pInputPoint1->mRGB->b;
	}

	return 2;
}

int Software3DPainter::ClipLineRightX(const PointData* pInputPoint1,
				      const PointData* pInputPoint2,
				      PointData* pOutputPoint1,
				      PointData* pOutputPoint2,
				      ClipFlags pFlags)
{
	if (pInputPoint1->mXYZ->x > mClipRight)
	{
		if (pInputPoint2->mXYZ->x > mClipRight)
		{
			return 0;
		}
		else
		{
			const PointData* lTemp = pInputPoint1;
			pInputPoint1 = pInputPoint2;
			pInputPoint2 = lTemp;
		}
	}
	else
	{
		if (pInputPoint2->mXYZ->x < mClipRight)
		{
			pOutputPoint1->mXYZ->x = pInputPoint1->mXYZ->x;
			pOutputPoint1->mXYZ->y = pInputPoint1->mXYZ->y;
			pOutputPoint1->mXYZ->z = pInputPoint1->mXYZ->z;
			pOutputPoint2->mXYZ->x = pInputPoint2->mXYZ->x;
			pOutputPoint2->mXYZ->y = pInputPoint2->mXYZ->y;
			pOutputPoint2->mXYZ->z = pInputPoint2->mXYZ->z;

			if ((pFlags & CLIP_UV) != 0)
			{
				pOutputPoint1->mUVM->u = pInputPoint1->mUVM->u;
				pOutputPoint1->mUVM->v = pInputPoint1->mUVM->v;
				pOutputPoint2->mUVM->u = pInputPoint2->mUVM->u;
				pOutputPoint2->mUVM->v = pInputPoint2->mUVM->v;
			}

			if ((pFlags & CLIP_RGB) != 0)
			{
				pOutputPoint1->mRGB->r = pInputPoint1->mRGB->r;
				pOutputPoint1->mRGB->g = pInputPoint1->mRGB->g;
				pOutputPoint1->mRGB->b = pInputPoint1->mRGB->b;
				pOutputPoint2->mRGB->r = pInputPoint2->mRGB->r;
				pOutputPoint2->mRGB->g = pInputPoint2->mRGB->g;
				pOutputPoint2->mRGB->b = pInputPoint2->mRGB->b;
			}

			return 2;
		}
	}

	const float lT = (mClipRight - pInputPoint2->mXYZ->x) / 
					   (pInputPoint1->mXYZ->x - pInputPoint2->mXYZ->x);

	float lOneOverZ0 = 1.0f / pInputPoint2->mXYZ->z;
	float lOneOverZ1 = 1.0f / pInputPoint1->mXYZ->z;
	pOutputPoint1->mXYZ->x = mClipRight;
	pOutputPoint1->mXYZ->y = pInputPoint2->mXYZ->y + (pInputPoint1->mXYZ->y - pInputPoint2->mXYZ->y) * lT;
	pOutputPoint1->mXYZ->z = 1.0f / (lOneOverZ0 + (lOneOverZ1 - lOneOverZ0) * lT);
	pOutputPoint2->mXYZ->x = pInputPoint1->mXYZ->x;
	pOutputPoint2->mXYZ->y = pInputPoint1->mXYZ->y;
	pOutputPoint2->mXYZ->z = pInputPoint1->mXYZ->z;

	if ((pFlags & CLIP_UV) != 0)
	{
		float lUOverZ0 = pInputPoint2->mUVM->u * lOneOverZ0;
		float lVOverZ0 = pInputPoint2->mUVM->v * lOneOverZ0;
		float lUOverZ1 = pInputPoint1->mUVM->u * lOneOverZ1;
		float lVOverZ1 = pInputPoint1->mUVM->v * lOneOverZ1;

		pOutputPoint1->mUVM->u = (lUOverZ0 + (lUOverZ1 - lUOverZ0) * lT) * pOutputPoint1->mXYZ->z;
		pOutputPoint1->mUVM->v = (lVOverZ0 + (lVOverZ1 - lVOverZ0) * lT) * pOutputPoint1->mXYZ->z;
		pOutputPoint2->mUVM->u = pInputPoint1->mUVM->u;
		pOutputPoint2->mUVM->v = pInputPoint1->mUVM->v;
	}

	if ((pFlags & CLIP_RGB) != 0)
	{
		float lROverZ0 = pInputPoint2->mRGB->r * lOneOverZ0;
		float lGOverZ0 = pInputPoint2->mRGB->g * lOneOverZ0;
		float lBOverZ0 = pInputPoint2->mRGB->b * lOneOverZ0;
		float lROverZ1 = pInputPoint1->mRGB->r * lOneOverZ1;
		float lGOverZ1 = pInputPoint1->mRGB->g * lOneOverZ1;
		float lBOverZ1 = pInputPoint1->mRGB->b * lOneOverZ1;

		pOutputPoint1->mRGB->r = (lROverZ0 + (lROverZ1 - lROverZ0) * lT) * pOutputPoint1->mXYZ->z;
		pOutputPoint1->mRGB->g = (lGOverZ0 + (lGOverZ1 - lGOverZ0) * lT) * pOutputPoint1->mXYZ->z;
		pOutputPoint1->mRGB->b = (lBOverZ0 + (lBOverZ1 - lBOverZ0) * lT) * pOutputPoint1->mXYZ->z;
		pOutputPoint2->mRGB->r = pInputPoint1->mRGB->r;
		pOutputPoint2->mRGB->g = pInputPoint1->mRGB->g;
		pOutputPoint2->mRGB->b = pInputPoint1->mRGB->b;
	}

	return 2;
}

int Software3DPainter::ClipLineTopY(const PointData* pInputPoint1,
				    const PointData* pInputPoint2,
				    PointData* pOutputPoint1,
				    PointData* pOutputPoint2,
				    ClipFlags pFlags)
{
	if (pInputPoint1->mXYZ->y < mClipTop)
	{
		if (pInputPoint2->mXYZ->y < mClipTop)
		{
			return 0;
		}
		else
		{
			const PointData* lTemp = pInputPoint1;
			pInputPoint1 = pInputPoint2;
			pInputPoint2 = lTemp;
		}
	}
	else
	{
		if (pInputPoint2->mXYZ->y > mClipTop)
		{
			pOutputPoint1->mXYZ->x = pInputPoint1->mXYZ->x;
			pOutputPoint1->mXYZ->y = pInputPoint1->mXYZ->y;
			pOutputPoint1->mXYZ->z = pInputPoint1->mXYZ->z;
			pOutputPoint2->mXYZ->x = pInputPoint2->mXYZ->x;
			pOutputPoint2->mXYZ->y = pInputPoint2->mXYZ->y;
			pOutputPoint2->mXYZ->z = pInputPoint2->mXYZ->z;

			if ((pFlags & CLIP_UV) != 0)
			{
				pOutputPoint1->mUVM->u = pInputPoint1->mUVM->u;
				pOutputPoint1->mUVM->v = pInputPoint1->mUVM->v;
				pOutputPoint2->mUVM->u = pInputPoint2->mUVM->u;
				pOutputPoint2->mUVM->v = pInputPoint2->mUVM->v;
			}

			if ((pFlags & CLIP_RGB) != 0)
			{
				pOutputPoint1->mRGB->r = pInputPoint1->mRGB->r;
				pOutputPoint1->mRGB->g = pInputPoint1->mRGB->g;
				pOutputPoint1->mRGB->b = pInputPoint1->mRGB->b;
				pOutputPoint2->mRGB->r = pInputPoint2->mRGB->r;
				pOutputPoint2->mRGB->g = pInputPoint2->mRGB->g;
				pOutputPoint2->mRGB->b = pInputPoint2->mRGB->b;
			}

			return 2;
		}
	}

	const float lT = (mClipTop - pInputPoint2->mXYZ->y) / 
					   (pInputPoint1->mXYZ->y - pInputPoint2->mXYZ->y);

	float lOneOverZ0 = 1.0f / pInputPoint2->mXYZ->z;
	float lOneOverZ1 = 1.0f / pInputPoint1->mXYZ->z;
	pOutputPoint1->mXYZ->x = pInputPoint2->mXYZ->x + (pInputPoint1->mXYZ->x - pInputPoint2->mXYZ->x) * lT;
	pOutputPoint1->mXYZ->y = mClipTop;
	pOutputPoint1->mXYZ->z = 1.0f / (lOneOverZ0 + (lOneOverZ1 - lOneOverZ0) * lT);
	pOutputPoint2->mXYZ->x = pInputPoint1->mXYZ->x;
	pOutputPoint2->mXYZ->y = pInputPoint1->mXYZ->y;
	pOutputPoint2->mXYZ->z = pInputPoint1->mXYZ->z;

	if ((pFlags & CLIP_UV) != 0)
	{
		float lUOverZ0 = pInputPoint2->mUVM->u * lOneOverZ0;
		float lVOverZ0 = pInputPoint2->mUVM->v * lOneOverZ0;
		float lUOverZ1 = pInputPoint1->mUVM->u * lOneOverZ1;
		float lVOverZ1 = pInputPoint1->mUVM->v * lOneOverZ1;

		pOutputPoint1->mUVM->u = (lUOverZ0 + (lUOverZ1 - lUOverZ0) * lT) * pOutputPoint1->mXYZ->z;
		pOutputPoint1->mUVM->v = (lVOverZ0 + (lVOverZ1 - lVOverZ0) * lT) * pOutputPoint1->mXYZ->z;
		pOutputPoint2->mUVM->u = pInputPoint1->mUVM->u;
		pOutputPoint2->mUVM->v = pInputPoint1->mUVM->v;
	}

	if ((pFlags & CLIP_RGB) != 0)
	{
		float lROverZ0 = pInputPoint2->mRGB->r * lOneOverZ0;
		float lGOverZ0 = pInputPoint2->mRGB->g * lOneOverZ0;
		float lBOverZ0 = pInputPoint2->mRGB->b * lOneOverZ0;
		float lROverZ1 = pInputPoint1->mRGB->r * lOneOverZ1;
		float lGOverZ1 = pInputPoint1->mRGB->g * lOneOverZ1;
		float lBOverZ1 = pInputPoint1->mRGB->b * lOneOverZ1;

		pOutputPoint1->mRGB->r = (lROverZ0 + (lROverZ1 - lROverZ0) * lT) * pOutputPoint1->mXYZ->z;
		pOutputPoint1->mRGB->g = (lGOverZ0 + (lGOverZ1 - lGOverZ0) * lT) * pOutputPoint1->mXYZ->z;
		pOutputPoint1->mRGB->b = (lBOverZ0 + (lBOverZ1 - lBOverZ0) * lT) * pOutputPoint1->mXYZ->z;
		pOutputPoint2->mRGB->r = pInputPoint1->mRGB->r;
		pOutputPoint2->mRGB->g = pInputPoint1->mRGB->g;
		pOutputPoint2->mRGB->b = pInputPoint1->mRGB->b;
	}

	return 2;
}

int Software3DPainter::ClipLineBottomY(const PointData* pInputPoint1,
				       const PointData* pInputPoint2,
				       PointData* pOutputPoint1,
				       PointData* pOutputPoint2,
				       ClipFlags pFlags)
{
	if (pInputPoint1->mXYZ->y > mClipBottom)
	{
		if (pInputPoint2->mXYZ->y > mClipBottom)
		{
			return 0;
		}
		else
		{
			const PointData* lTemp = pInputPoint1;
			pInputPoint1 = pInputPoint2;
			pInputPoint2 = lTemp;
		}
	}
	else
	{
		if (pInputPoint2->mXYZ->y < mClipBottom)
		{
			pOutputPoint1->mXYZ->x = pInputPoint1->mXYZ->x;
			pOutputPoint1->mXYZ->y = pInputPoint1->mXYZ->y;
			pOutputPoint1->mXYZ->z = pInputPoint1->mXYZ->z;
			pOutputPoint2->mXYZ->x = pInputPoint2->mXYZ->x;
			pOutputPoint2->mXYZ->y = pInputPoint2->mXYZ->y;
			pOutputPoint2->mXYZ->z = pInputPoint2->mXYZ->z;

			if ((pFlags & CLIP_UV) != 0)
			{
				pOutputPoint1->mUVM->u = pInputPoint1->mUVM->u;
				pOutputPoint1->mUVM->v = pInputPoint1->mUVM->v;
				pOutputPoint2->mUVM->u = pInputPoint2->mUVM->u;
				pOutputPoint2->mUVM->v = pInputPoint2->mUVM->v;
			}

			if ((pFlags & CLIP_RGB) != 0)
			{
				pOutputPoint1->mRGB->r = pInputPoint1->mRGB->r;
				pOutputPoint1->mRGB->g = pInputPoint1->mRGB->g;
				pOutputPoint1->mRGB->b = pInputPoint1->mRGB->b;
				pOutputPoint2->mRGB->r = pInputPoint2->mRGB->r;
				pOutputPoint2->mRGB->g = pInputPoint2->mRGB->g;
				pOutputPoint2->mRGB->b = pInputPoint2->mRGB->b;
			}

			return 2;
		}
	}

	const float lT = (mClipBottom - pInputPoint2->mXYZ->y) / 
			    (pInputPoint1->mXYZ->y - pInputPoint2->mXYZ->y);

	float lOneOverZ0 = 1.0f / pInputPoint2->mXYZ->z;
	float lOneOverZ1 = 1.0f / pInputPoint1->mXYZ->z;
	pOutputPoint1->mXYZ->x = pInputPoint2->mXYZ->x + (pInputPoint1->mXYZ->x - pInputPoint2->mXYZ->x) * lT;
	pOutputPoint1->mXYZ->y = mClipBottom;
	pOutputPoint1->mXYZ->z = 1.0f / (lOneOverZ0 + (lOneOverZ1 - lOneOverZ0) * lT);
	pOutputPoint2->mXYZ->x = pInputPoint1->mXYZ->x;
	pOutputPoint2->mXYZ->y = pInputPoint1->mXYZ->y;
	pOutputPoint2->mXYZ->z = pInputPoint1->mXYZ->z;

	if ((pFlags & CLIP_UV) != 0)
	{
		float lUOverZ0 = pInputPoint2->mUVM->u * lOneOverZ0;
		float lVOverZ0 = pInputPoint2->mUVM->v * lOneOverZ0;
		float lUOverZ1 = pInputPoint1->mUVM->u * lOneOverZ1;
		float lVOverZ1 = pInputPoint1->mUVM->v * lOneOverZ1;

		pOutputPoint1->mUVM->u = (lUOverZ0 + (lUOverZ1 - lUOverZ0) * lT) * pOutputPoint1->mXYZ->z;
		pOutputPoint1->mUVM->v = (lVOverZ0 + (lVOverZ1 - lVOverZ0) * lT) * pOutputPoint1->mXYZ->z;
		pOutputPoint2->mUVM->u = pInputPoint1->mUVM->u;
		pOutputPoint2->mUVM->v = pInputPoint1->mUVM->v;
	}

	if ((pFlags & CLIP_RGB) != 0)
	{
		float lROverZ0 = pInputPoint2->mRGB->r * lOneOverZ0;
		float lGOverZ0 = pInputPoint2->mRGB->g * lOneOverZ0;
		float lBOverZ0 = pInputPoint2->mRGB->b * lOneOverZ0;
		float lROverZ1 = pInputPoint1->mRGB->r * lOneOverZ1;
		float lGOverZ1 = pInputPoint1->mRGB->g * lOneOverZ1;
		float lBOverZ1 = pInputPoint1->mRGB->b * lOneOverZ1;

		pOutputPoint1->mRGB->r = (lROverZ0 + (lROverZ1 - lROverZ0) * lT) * pOutputPoint1->mXYZ->z;
		pOutputPoint1->mRGB->g = (lGOverZ0 + (lGOverZ1 - lGOverZ0) * lT) * pOutputPoint1->mXYZ->z;
		pOutputPoint1->mRGB->b = (lBOverZ0 + (lBOverZ1 - lBOverZ0) * lT) * pOutputPoint1->mXYZ->z;
		pOutputPoint2->mRGB->r = pInputPoint1->mRGB->r;
		pOutputPoint2->mRGB->g = pInputPoint1->mRGB->g;
		pOutputPoint2->mRGB->b = pInputPoint1->mRGB->b;
	}

	return 2;
}



int Software3DPainter::ClipVectorNearZ(const PointData* pInputPoint1,
				       const PointData* pInputPoint2,
				       ClipFlags pFlags,
				       Software3DPainter* pThis)
{
	if (pInputPoint1->mXYZ->mTranslatedZ < pThis->mNearZ)
	{
		if (pInputPoint2->mXYZ->mTranslatedZ < pThis->mNearZ)
		{
			return 3;
		}
		else
		{
			const float lT = (pThis->mNearZ - pInputPoint2->mXYZ->mTranslatedZ) / 
							   (pInputPoint1->mXYZ->mTranslatedZ - pInputPoint2->mXYZ->mTranslatedZ);

			pInputPoint1->mXYZ->mTranslatedX = pInputPoint2->mXYZ->mTranslatedX + (pInputPoint1->mXYZ->mTranslatedX - pInputPoint2->mXYZ->mTranslatedX) * lT;
			pInputPoint1->mXYZ->mTranslatedY = pInputPoint2->mXYZ->mTranslatedY + (pInputPoint1->mXYZ->mTranslatedY - pInputPoint2->mXYZ->mTranslatedY) * lT;
			pInputPoint1->mXYZ->mTranslatedZ = pThis->mNearZ;

			if ((pFlags & CLIP_UV) != 0)
			{
				pInputPoint1->mUVM->u = pInputPoint2->mUVM->u + (pInputPoint1->mUVM->u - pInputPoint2->mUVM->u) * lT;
				pInputPoint1->mUVM->v = pInputPoint2->mUVM->v + (pInputPoint1->mUVM->v - pInputPoint2->mUVM->v) * lT;
			}

			if ((pFlags & CLIP_RGB) != 0)
			{
				pInputPoint1->mRGB->r = pInputPoint2->mRGB->r + (pInputPoint1->mRGB->r - pInputPoint2->mRGB->r) * lT;
				pInputPoint1->mRGB->g = pInputPoint2->mRGB->g + (pInputPoint1->mRGB->g - pInputPoint2->mRGB->g) * lT;
				pInputPoint1->mRGB->b = pInputPoint2->mRGB->b + (pInputPoint1->mRGB->b - pInputPoint2->mRGB->b) * lT;
			}

			return 1;
		}
	}
	else
	{
		if (pInputPoint2->mXYZ->mTranslatedZ > pThis->mNearZ)
		{
			return 0;
		}
		else
		{
			const float lT = (pThis->mNearZ - pInputPoint2->mXYZ->mTranslatedZ) / 
							   (pInputPoint1->mXYZ->mTranslatedZ - pInputPoint2->mXYZ->mTranslatedZ);

			pInputPoint2->mXYZ->mTranslatedX = pInputPoint2->mXYZ->mTranslatedX + (pInputPoint1->mXYZ->mTranslatedX - pInputPoint2->mXYZ->mTranslatedX) * lT;
			pInputPoint2->mXYZ->mTranslatedY = pInputPoint2->mXYZ->mTranslatedY + (pInputPoint1->mXYZ->mTranslatedY - pInputPoint2->mXYZ->mTranslatedY) * lT;
			pInputPoint2->mXYZ->mTranslatedZ = pThis->mNearZ;

			if ((pFlags & CLIP_UV) != 0)
			{
				pInputPoint2->mUVM->u = pInputPoint2->mUVM->u + (pInputPoint1->mUVM->u - pInputPoint2->mUVM->u) * lT;
				pInputPoint2->mUVM->v = pInputPoint2->mUVM->v + (pInputPoint1->mUVM->v - pInputPoint2->mUVM->v) * lT;
			}

			if ((pFlags & CLIP_RGB) != 0)
			{
				pInputPoint2->mRGB->r = pInputPoint2->mRGB->r + (pInputPoint1->mRGB->r - pInputPoint2->mRGB->r) * lT;
				pInputPoint2->mRGB->g = pInputPoint2->mRGB->g + (pInputPoint1->mRGB->g - pInputPoint2->mRGB->g) * lT;
				pInputPoint2->mRGB->b = pInputPoint2->mRGB->b + (pInputPoint1->mRGB->b - pInputPoint2->mRGB->b) * lT;
			}

			return 2;
		}
	}
}

int Software3DPainter::ClipVectorFarZ(const PointData* pInputPoint1,
				      const PointData* pInputPoint2,
				      ClipFlags pFlags,
				      Software3DPainter* pThis)
{
	if (pInputPoint1->mXYZ->mTranslatedZ > pThis->mFarZ)
	{
		if (pInputPoint2->mXYZ->mTranslatedZ > pThis->mFarZ)
		{
			return 3;
		}
		else
		{
			const float lT = (pThis->mFarZ - pInputPoint2->mXYZ->mTranslatedZ) / 
							   (pInputPoint1->mXYZ->mTranslatedZ - pInputPoint2->mXYZ->mTranslatedZ);

			pInputPoint1->mXYZ->mTranslatedX = pInputPoint2->mXYZ->mTranslatedX + (pInputPoint1->mXYZ->mTranslatedX - pInputPoint2->mXYZ->mTranslatedX) * lT;
			pInputPoint1->mXYZ->mTranslatedY = pInputPoint2->mXYZ->mTranslatedY + (pInputPoint1->mXYZ->mTranslatedY - pInputPoint2->mXYZ->mTranslatedY) * lT;
			pInputPoint1->mXYZ->mTranslatedZ = pThis->mFarZ;

			if ((pFlags & CLIP_UV) != 0)
			{
				pInputPoint1->mUVM->u = pInputPoint2->mUVM->u + (pInputPoint1->mUVM->u - pInputPoint2->mUVM->u) * lT;
				pInputPoint1->mUVM->v = pInputPoint2->mUVM->v + (pInputPoint1->mUVM->v - pInputPoint2->mUVM->v) * lT;
			}

			if ((pFlags & CLIP_RGB) != 0)
			{
				pInputPoint1->mRGB->r = pInputPoint2->mRGB->r + (pInputPoint1->mRGB->r - pInputPoint2->mRGB->r) * lT;
				pInputPoint1->mRGB->g = pInputPoint2->mRGB->g + (pInputPoint1->mRGB->g - pInputPoint2->mRGB->g) * lT;
				pInputPoint1->mRGB->b = pInputPoint2->mRGB->b + (pInputPoint1->mRGB->b - pInputPoint2->mRGB->b) * lT;
			}

			return 1;
		}
	}
	else
	{
		if (pInputPoint2->mXYZ->mTranslatedZ < pThis->mFarZ)
		{
			return 0;
		}
		else
		{
			const float lT = (pThis->mFarZ - pInputPoint2->mXYZ->mTranslatedZ) / 
							   (pInputPoint1->mXYZ->mTranslatedZ - pInputPoint2->mXYZ->mTranslatedZ);

			pInputPoint2->mXYZ->mTranslatedX = pInputPoint2->mXYZ->mTranslatedX + (pInputPoint1->mXYZ->mTranslatedX - pInputPoint2->mXYZ->mTranslatedX) * lT;
			pInputPoint2->mXYZ->mTranslatedY = pInputPoint2->mXYZ->mTranslatedY + (pInputPoint1->mXYZ->mTranslatedY - pInputPoint2->mXYZ->mTranslatedY) * lT;
			pInputPoint2->mXYZ->mTranslatedZ = pThis->mFarZ;

			if ((pFlags & CLIP_UV) != 0)
			{
				pInputPoint2->mUVM->u = pInputPoint2->mUVM->u + (pInputPoint1->mUVM->u - pInputPoint2->mUVM->u) * lT;
				pInputPoint2->mUVM->v = pInputPoint2->mUVM->v + (pInputPoint1->mUVM->v - pInputPoint2->mUVM->v) * lT;
			}

			if ((pFlags & CLIP_RGB) != 0)
			{
				pInputPoint2->mRGB->r = pInputPoint2->mRGB->r + (pInputPoint1->mRGB->r - pInputPoint2->mRGB->r) * lT;
				pInputPoint2->mRGB->g = pInputPoint2->mRGB->g + (pInputPoint1->mRGB->g - pInputPoint2->mRGB->g) * lT;
				pInputPoint2->mRGB->b = pInputPoint2->mRGB->b + (pInputPoint1->mRGB->b - pInputPoint2->mRGB->b) * lT;
			}

			return 2;
		}
	}
}

void Software3DPainter::SetVertex3DData(Vertex3D*, const PointData*, float)
{
	// Do nothing.
}

void Software3DPainter::SetVertex3DUVData(Vertex3D* pV, const PointData* pP, float z)
{
	((Vertex3DUV*)pV)->SetU(pP->mUVM->u * z);
	((Vertex3DUV*)pV)->SetV(pP->mUVM->v * z);
}

void Software3DPainter::SetVertex3DRGBData(Vertex3D* pV, const PointData* pP, float z)
{
	((Vertex3DRGB*)pV)->SetR(pP->mRGB->mLightProcessedR * z);
	((Vertex3DRGB*)pV)->SetG(pP->mRGB->mLightProcessedG * z);
	((Vertex3DRGB*)pV)->SetB(pP->mRGB->mLightProcessedB * z);
}

void Software3DPainter::SetVertex3DUVRGBData(Vertex3D* pV, const PointData* pP, float z)
{
	((Vertex3DUVRGB*)pV)->SetU(pP->mUVM->u * z);
	((Vertex3DUVRGB*)pV)->SetV(pP->mUVM->v * z);
	((Vertex3DUVRGB*)pV)->SetR(pP->mRGB->mLightProcessedR * z);
	((Vertex3DUVRGB*)pV)->SetG(pP->mRGB->mLightProcessedG * z);
	((Vertex3DUVRGB*)pV)->SetB(pP->mRGB->mLightProcessedB * z);
}

void Software3DPainter::SetVertex3DUVMData(Vertex3D* pV, const PointData* pP, float z)
{
	((Vertex3DUVM*)pV)->SetU(pP->mUVM->u * z);
	((Vertex3DUVM*)pV)->SetV(pP->mUVM->v * z);
	((Vertex3DUVM*)pV)->SetM(pP->mUVM->m * z);
}

void Software3DPainter::SetVertex3DUVRGBMData(Vertex3D* pV, const PointData* pP, float z)
{
	((Vertex3DUVRGBM*)pV)->SetU(pP->mUVM->u * z);
	((Vertex3DUVRGBM*)pV)->SetV(pP->mUVM->v * z);
	((Vertex3DUVRGBM*)pV)->SetM(pP->mUVM->m * z);
	((Vertex3DUVRGBM*)pV)->SetR(pP->mRGB->mLightProcessedR * z);
	((Vertex3DUVRGBM*)pV)->SetG(pP->mRGB->mLightProcessedG * z);
	((Vertex3DUVRGBM*)pV)->SetB(pP->mRGB->mLightProcessedB * z);
}

int Software3DPainter::ClipAndProjectTriangle(Vertex3D* pV[8], Software3DPainter* pThis, ClipFlags pClipFlags, SetVertexDataFunc pSetVFunc)
{
	// The scale factor scales the coordinates to the viewport resolution.
	// This way, things will keep the same size no matter what screen resolution
	// you use. (And as you can see, the scale factor is actually the viewport width).
	const float lScale = (pThis->mViewportRight - pThis->mViewportLeft) * pThis->mScreenDistToOrigin;

	if (pThis->mOrthographicProjection == true)
	{
		int lStart = 2;
		int lEnd = 0;
		for (int i = 0; i < 3; i++)
		{
			PointData* lStartPoint = &pThis->mPoint[lStart];
			float lP = lScale / pThis->mNearZ;
			pV[i]->SetX(pThis->mPerspectiveCenterX + lStartPoint->mXYZ->mTranslatedX * lP);
			pV[i]->SetY(pThis->mPerspectiveCenterY - lStartPoint->mXYZ->mTranslatedY * lP);
			pV[i]->SetZ(pThis->mNearZ);

			pSetVFunc(pV[i], lStartPoint, 1.0);

			lStart = lEnd;
			lEnd++;
		}
		return 3;
	}

	int lNumPoints1 = 3;
	int lNumPoints2 = 3;
	lNumPoints2 = pThis->ClipPolygon(pThis->mPoint, lNumPoints1, pThis->mClipPoly2, pClipFlags, 0, 0, 1.0, -pThis->mNearZ, true);

	// Project the points and convert them to screen space.
	int i;
	for (i = 0; i < lNumPoints2; i++)
	{
		float lP = lScale / pThis->mClipPoly2[i].mXYZ->mTranslatedZ;
		pThis->mClipPoly2[i].mXYZ->mTranslatedX = pThis->mPerspectiveCenterX + pThis->mClipPoly2[i].mXYZ->mTranslatedX * lP;
		pThis->mClipPoly2[i].mXYZ->mTranslatedY = pThis->mPerspectiveCenterY - pThis->mClipPoly2[i].mXYZ->mTranslatedY * lP;

		float z = 1.0f / pThis->mClipPoly2[i].mXYZ->mTranslatedZ;
		pThis->mClipPoly2[i].mXYZ->mTranslatedZ = z;

		if ((pClipFlags & CLIP_UV) != 0)
		{
			pThis->mClipPoly2[i].mUVM->u = pThis->mClipPoly2[i].mUVM->u * z;
			pThis->mClipPoly2[i].mUVM->v = pThis->mClipPoly2[i].mUVM->v * z;
			pThis->mClipPoly2[i].mUVM->m = pThis->mClipPoly2[i].mUVM->m * z;
		}

		if ((pClipFlags & CLIP_RGB) != 0)
		{
			pThis->mClipPoly2[i].mRGB->mLightProcessedR = pThis->mClipPoly2[i].mRGB->mLightProcessedR * z;
			pThis->mClipPoly2[i].mRGB->mLightProcessedG = pThis->mClipPoly2[i].mRGB->mLightProcessedG * z;
			pThis->mClipPoly2[i].mRGB->mLightProcessedB = pThis->mClipPoly2[i].mRGB->mLightProcessedB * z;
		}
	}

	// Clip the triangle angainst the screen edges.
	lNumPoints1 = pThis->ClipPolygon(pThis->mClipPoly2, lNumPoints2, pThis->mClipPoly1, pClipFlags, 1.0, 0, 0, -pThis->mClipLeft, true);
	lNumPoints2 = pThis->ClipPolygon(pThis->mClipPoly1, lNumPoints1, pThis->mClipPoly2, pClipFlags, -1.0, 0, 0, pThis->mClipRight + 1, true);
	lNumPoints1 = pThis->ClipPolygon(pThis->mClipPoly2, lNumPoints2, pThis->mClipPoly1, pClipFlags, 0, 1.0, 0, -pThis->mClipTop, true);
	lNumPoints2 = pThis->ClipPolygon(pThis->mClipPoly1, lNumPoints1, pThis->mClipPoly2, pClipFlags, 0, -1.0, 0, pThis->mClipBottom + 1, true);

	// Convert to vertices.
	for (i = 0; i < lNumPoints2; i++)
	{
		pV[i]->SetX(pThis->mClipPoly2[i].mXYZ->mTranslatedX);
		pV[i]->SetY(pThis->mClipPoly2[i].mXYZ->mTranslatedY);

		float z = 1.0f / pThis->mClipPoly2[i].mXYZ->mTranslatedZ;
		pV[i]->SetZ(z);

		pSetVFunc(pV[i], &pThis->mClipPoly2[i], z);
	}
	return lNumPoints2;
}

bool Software3DPainter::CheckVisible(Vertex3D* pV[8], int pNumVertices, Software3DPainter* pThis)
{
	bool lVisible = false;
	int a = 0;
	int b = 1;
	int c = 2;

	if (pThis->mFaceOrientation == FACE_CCW)
	{
		// Change order.
		int t = b;
		b = c;
		c = t;
	}

	for(int i = 0; i < pNumVertices - 2 && lVisible == false; i++)
	{
		Vertex3D* lV0 = pV[a];
		Vertex3D* lV1 = pV[b + i];
		Vertex3D* lV2 = pV[c + i];

 		float lCrossZ = (lV1->GetX() - lV0->GetX()) * (lV2->GetY() - lV0->GetY()) -
			(lV2->GetX() - lV0->GetX()) * (lV1->GetY() - lV0->GetY());


		if (lCrossZ > 0.0f)
   		{
			lVisible = true;
		}
		else if(!((lV0->GetX() < pThis->mClipLeft &&
			 lV1->GetX() < pThis->mClipLeft &&
			 lV2->GetX() < pThis->mClipLeft) ||
			(lV0->GetX() > pThis->mClipRight &&
			 lV1->GetX() > pThis->mClipRight &&
			 lV2->GetX() > pThis->mClipRight) ||
			(lV0->GetY() < pThis->mClipTop &&
			 lV1->GetY() < pThis->mClipTop &&
			 lV2->GetY() < pThis->mClipTop) ||
			(lV0->GetY() > pThis->mClipBottom &&
			 lV1->GetY() > pThis->mClipBottom &&
			 lV2->GetY() > pThis->mClipBottom)))
		{
			lVisible = true;
		}
	}

	return lVisible;
}

} // End namespace.
