
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/UiTriangleBasedGeometry.h"
#include "../../Lepra/Include/LepraAssert.h"
#include "../../Lepra/Include/ResourceTracker.h"



namespace UiTbc
{



TriangleBasedGeometry::TriangleBasedGeometry() :
	mVertexCount(0),
	mIndexCount(0),
	mUVSetCount(0),
	mVertexData(0),
	mUVData(0),
	mColorData(0),
	mIndexData(0),
	mColorFormat(COLOR_RGB),
	mGeometryVolatility(Tbc::GeometryBase::GEOM_STATIC)
{
	LEPRA_ACQUIRE_RESOURCE(TriangleBasedGeometry);
	SetPrimitiveType(Tbc::GeometryBase::TRIANGLES);
}

TriangleBasedGeometry::TriangleBasedGeometry(const TriangleBasedGeometry& pGeometry) :
	mVertexCount(0),
	mIndexCount(0),
	mUVSetCount(0),
	mVertexData(0),
	mUVData(0),
	mColorData(0),
	mIndexData(0),
	mColorFormat(COLOR_RGB),
	mGeometryVolatility(Tbc::GeometryBase::GEOM_STATIC)
{
	LEPRA_ACQUIRE_RESOURCE(TriangleBasedGeometry);
	SetPrimitiveType(Tbc::GeometryBase::TRIANGLES);
	Copy(pGeometry);
}

TriangleBasedGeometry::TriangleBasedGeometry(const vec3* pVertices,
                                             const vec3* pVertexNormals,
                                             const Vector2D<float>* pUV,
                                             const Color* pColor,
                                             ColorFormat pColorFormat,
                                             const uint32* pIndices,
                                             unsigned int pVertexCount,
                                             unsigned int pIndexCount,
					     Tbc::GeometryBase::PrimitiveType pPrimitiveType,
                                             Tbc::GeometryBase::GeometryVolatility pGeomType) :
	mVertexCount(0),
	mIndexCount(0),
	mUVSetCount(0),
	mVertexData(0),
	mUVData(0),
	mColorData(0),
	mIndexData(0),
	mColorFormat(COLOR_RGB),
	mGeometryVolatility(Tbc::GeometryBase::GEOM_STATIC)
{
	LEPRA_ACQUIRE_RESOURCE(TriangleBasedGeometry);

	Set(pVertices,
		pVertexNormals,
		pUV,
		pColor,
		pColorFormat,
		pIndices,
		pVertexCount,
		pIndexCount,
		pPrimitiveType,
		pGeomType);
}

TriangleBasedGeometry::TriangleBasedGeometry(const float* pVertexData,
                                             const float* pVertexNormals,
                                             const float* pUVData,
                                             const uint8* pColorData,
                                             ColorFormat pColorFormat,
                                             const uint32* pIndices,
                                             unsigned int pVertexCount,
                                             unsigned int pIndexCount,
					     Tbc::GeometryBase::PrimitiveType pPrimitiveType,
                                             Tbc::GeometryBase::GeometryVolatility pGeomType) :
	mVertexCount(0),
	mIndexCount(0),
	mUVSetCount(0),
	mVertexData(0),
	mUVData(0),
	mColorData(0),
	mIndexData(0),
	mColorFormat(COLOR_RGB),
	mGeometryVolatility(Tbc::GeometryBase::GEOM_STATIC)
{
	LEPRA_ACQUIRE_RESOURCE(TriangleBasedGeometry);

	Set(pVertexData,
		pVertexNormals,
		pUVData,
		pColorData,
		pColorFormat,
		pIndices,
		pVertexCount,
		pIndexCount,
		pPrimitiveType,
		pGeomType);
}

void TriangleBasedGeometry::Set(const vec3* pVertices,
                                const vec3* pVertexNormals,
                                const Vector2D<float>* pUV,
                                const Color* pColor,
                                ColorFormat pColorFormat,
                                const uint32* pIndices,
                                unsigned int pVertexCount,
                                unsigned int pIndexCount,
				Tbc::GeometryBase::PrimitiveType pPrimitiveType,
                                Tbc::GeometryBase::GeometryVolatility pGeomType)
{
	ClearAll();

	SetPrimitiveType(pPrimitiveType);
	mGeometryVolatility = pGeomType;

	if (pVertexCount > 0 && pVertices != 0)
	{
		unsigned int i;
		int lIndex;

		mVertexCount = pVertexCount;
		mIndexCount = pIndexCount;

		// Copy vertex data.
		mVertexData = new float[mVertexCount * 3];
		for (i = 0, lIndex = 0; i < mVertexCount; i++, lIndex += 3)
		{
			mVertexData[lIndex + 0] = pVertices[i].x;
			mVertexData[lIndex + 1] = pVertices[i].y;
			mVertexData[lIndex + 2] = pVertices[i].z;
		}

		if (pVertexNormals != 0)
		{
			// Copy normal data.
			Tbc::GeometryBase::AllocVertexNormalData();
			float* lVertexNormalData = Tbc::GeometryBase::GetNormalData();
			for (i = 0, lIndex = 0; i < mVertexCount; i++, lIndex += 3)
			{
				lVertexNormalData[lIndex + 0] = pVertexNormals[i].x;
				lVertexNormalData[lIndex + 1] = pVertexNormals[i].y;
				lVertexNormalData[lIndex + 2] = pVertexNormals[i].z;
			}
		}

		if (pUV != 0)
		{
			AddUVSet(pUV);
		}

		if (pColor != 0)
		{
			// Copy color data.
			mColorFormat = pColorFormat;

			int lSize = 4;
			if (mColorFormat == COLOR_RGB)
				lSize = 3;

			mColorData = new uint8[mVertexCount * lSize];
			for (i = 0, lIndex = 0; i < mVertexCount; i++, lIndex += lSize)
			{
				mColorData[lIndex + 0] = pColor[i].mRed;
				mColorData[lIndex + 1] = pColor[i].mGreen;
				mColorData[lIndex + 2] = pColor[i].mBlue;

				if (mColorFormat == COLOR_RGBA)
					mColorData[lIndex + 3] = pColor[i].mAlpha;
			}
		}

		if (pIndices != 0)
		{
			// Copy index data.
			mIndexData = new vtx_idx_t[mIndexCount];
			for (i = 0; i < mIndexCount; i++)
			{
				mIndexData[i] = (vtx_idx_t)pIndices[i];
			}
		}
	}

	Tbc::GeometryBase::CalculateBoundingRadius();
}

void TriangleBasedGeometry::Set(const float* pVertexData,
                                const float* pVertexNormals,
                                const float* pUVData,
                                const uint8* pColorData,
                                ColorFormat pColorFormat,
                                const uint32* pIndices,
                                unsigned int pVertexCount,
                                unsigned int pIndexCount,
				Tbc::GeometryBase::PrimitiveType pPrimitiveType,
                                Tbc::GeometryBase::GeometryVolatility pGeomType)
{
	ClearAll();

	SetPrimitiveType(pPrimitiveType);
	mGeometryVolatility = pGeomType;

	if (pVertexCount > 0 && pVertexData != 0)
	{
		unsigned int i;
		int lIndex;

		mVertexCount = pVertexCount;
		mIndexCount = pIndexCount;

		// Copy vertex data.
		mVertexData = new float[mVertexCount * 3];
		for (i = 0, lIndex = 0; i < mVertexCount; i++, lIndex += 3)
		{
			mVertexData[lIndex + 0] = pVertexData[lIndex + 0];
			mVertexData[lIndex + 1] = pVertexData[lIndex + 1];
			mVertexData[lIndex + 2] = pVertexData[lIndex + 2];
		}

		if (pVertexNormals != 0)
		{
			// Copy normal data.
			SetVertexNormalData(pVertexNormals, mVertexCount);
		}

		if (pUVData != 0)
		{
			AddUVSet(pUVData);
		}

		if (pColorData != 0)
		{
			// Copy color data.
			mColorFormat = pColorFormat;

			int lSize = 4;
			if (mColorFormat == COLOR_RGB)
				lSize = 3;

			mColorData = new uint8[mVertexCount * lSize];

			for (i = 0, lIndex = 0; i < mVertexCount; i++, lIndex += lSize)
			{
				mColorData[lIndex + 0] = pColorData[lIndex + 0];
				mColorData[lIndex + 1] = pColorData[lIndex + 1];
				mColorData[lIndex + 2] = pColorData[lIndex + 2];

				if (mColorFormat == COLOR_RGBA)
					mColorData[lIndex + 3] = pColorData[lIndex + 3];
			}
		}

		if (pIndices != 0)
		{
			// Copy index data.
			mIndexData = new vtx_idx_t[mIndexCount];
			for (i = 0; i < mIndexCount; i++)
			{
				mIndexData[i] = pIndices[i];
			}
		}
	}

	Tbc::GeometryBase::CalculateBoundingRadius();
}

void TriangleBasedGeometry::Copy(const TriangleBasedGeometry& pGeometry)
{
	ClearAll();

	if (pGeometry.mVertexCount > 0 && pGeometry.mVertexData != 0)
	{
		unsigned int i;
		int lIndex;

		TriangleBasedGeometry& lGeometry = (TriangleBasedGeometry&)pGeometry;

		mVertexCount = pGeometry.GetVertexCount();
		mIndexCount = pGeometry.GetIndexCount();

		mGeometryVolatility = pGeometry.GetGeometryVolatility();

		Tbc::GeometryBase::SetBasicMaterialSettings(pGeometry.GetBasicMaterialSettings());

		Tbc::GeometryBase::SetBoundingRadius(pGeometry.GetBoundingRadius());

		// Copy vertex data.
		const float* lVertexData = lGeometry.GetVertexData();
		mVertexData = new float[mVertexCount * 3];
		for (i = 0, lIndex = 0; i < mVertexCount; i++, lIndex += 3)
		{
			mVertexData[lIndex + 0] = lVertexData[lIndex + 0];
			mVertexData[lIndex + 1] = lVertexData[lIndex + 1];
			mVertexData[lIndex + 2] = lVertexData[lIndex + 2];
		}

		if (lGeometry.GetNormalData() != 0)
		{
			// Copy normal data.
			Tbc::GeometryBase::SetVertexNormalData(lGeometry.GetNormalData(), mVertexCount);
		}

		if (lGeometry.GetSurfaceNormalData() != 0)
		{
			Tbc::GeometryBase::SetSurfaceNormalData(lGeometry.GetSurfaceNormalData());
		}

		if (lGeometry.GetUVData() != 0)
		{
			mUVData = new float*[lGeometry.GetUVSetCount()];
			const int lUVCountPerVertex = lGeometry.GetUVCountPerVertex();
			
			for (unsigned int j = 0; j < lGeometry.GetUVSetCount(); j++)
			{
				// Copy UV data.
				mUVData[j] = new float[mVertexCount * lUVCountPerVertex];
				::memcpy(mUVData[j], lGeometry.GetUVData(j), mVertexCount*lUVCountPerVertex*sizeof(float));
			}
		}

		if (lGeometry.GetColorData() != 0)
		{
			// Copy color data.
			mColorFormat = lGeometry.GetColorFormat();

			int lSize = 4;
			if (mColorFormat == COLOR_RGB)
				lSize = 3;

			const uint8* lColorData = lGeometry.GetColorData();
			mColorData = new uint8[mVertexCount * lSize];
			for (i = 0, lIndex = 0; i < mVertexCount; i++, lIndex += lSize)
			{
				mColorData[lIndex + 0] = lColorData[lIndex + 0];
				mColorData[lIndex + 1] = lColorData[lIndex + 1];
				mColorData[lIndex + 2] = lColorData[lIndex + 2];

				if (mColorFormat == COLOR_RGBA)
					mColorData[lIndex + 3] = lColorData[lIndex + 3];
			}
		}

		if (lGeometry.GetIndexData() != 0)
		{
			// Copy index data.
			const vtx_idx_t* lIndices = lGeometry.GetIndexData();
			mIndexData = new vtx_idx_t[mIndexCount];
			for (i = 0; i < mIndexCount; i++)
			{
				mIndexData[i] = lIndices[i];
			}
		}

		Tbc::GeometryBase::Copy(&lGeometry);
	}
}

TriangleBasedGeometry::~TriangleBasedGeometry()
{
	LEPRA_RELEASE_RESOURCE(TriangleBasedGeometry);

	ClearAll();
}

void TriangleBasedGeometry::ClearAll()
{
	if (mVertexData != 0)
	{
		delete[] mVertexData;
		mVertexData = 0;
	}

	Tbc::GeometryBase::ClearSurfaceNormalData();
	Tbc::GeometryBase::ClearVertexNormalData();

	if (mUVSetCount > 0)
	{
		for (unsigned int i = 0; i < mUVSetCount; i++)
		{
			delete[] mUVData[i];
		}
		delete[] mUVData;
		mUVData = 0;
	}

	if (mColorData != 0)
	{
		delete[] mColorData;
		mColorData = 0;
	}

	if (mIndexData != 0)
	{
		delete[] mIndexData;
		mIndexData = 0;
	}

	mVertexCount = 0;
	mIndexCount = 0;

	Tbc::GeometryBase::ClearAll();
	SetVertexDataChanged(true);
	SetUVDataChanged(true);
	SetColorDataChanged(true);
	SetIndexDataChanged(true);
}

void TriangleBasedGeometry::SetPolygon(vec3* pVertices, unsigned int pVertexCount)
{
	ClearAll();
	mVertexData = new float[pVertexCount * 3];

	for (unsigned int i = 0; i < pVertexCount; i++)
	{
		mVertexData[i * 3 + 0] = pVertices[i].x;
		mVertexData[i * 3 + 1] = pVertices[i].y;
		mVertexData[i * 3 + 2] = pVertices[i].z;
	}

	mVertexCount = pVertexCount;

	Tbc::GeometryBase::CalculateBoundingRadius();
}

int TriangleBasedGeometry::AddUVSet(const float* pUVData)
{
	unsigned int lUVSetCount = mUVSetCount + 1;
	float** lUVData = new float*[lUVSetCount];

	unsigned int lUVSetIndex;

	// Copy present data.
	for (lUVSetIndex = 0; lUVSetIndex < mUVSetCount; lUVSetIndex++)
	{
		lUVData[lUVSetIndex] = new float[mVertexCount * GetUVCountPerVertex()];
		::memcpy(lUVData[lUVSetIndex], mUVData[lUVSetIndex], mVertexCount*GetUVCountPerVertex()*sizeof(float));
		delete[] mUVData[lUVSetIndex];
	}

	delete[] mUVData;

	lUVData[lUVSetIndex] = new float[mVertexCount * GetUVCountPerVertex()];

	// Add new data at lUVSetIndex.
	::memcpy(lUVData[lUVSetIndex], pUVData, mVertexCount*GetUVCountPerVertex()*sizeof(float));

	mUVData = lUVData;
	mUVSetCount = lUVSetCount;

	return mUVSetCount - 1;
}

int TriangleBasedGeometry::AddUVSet(const Vector2D<float>* pUVData)
{
	unsigned int lUVSetCount = mUVSetCount + 1;
	float** lUVData = new float*[lUVSetCount];

	unsigned int lUVSetIndex;
	unsigned int i;

	// Copy present data.
	const int lUVCountPerVertex = GetUVCountPerVertex();
	for (lUVSetIndex = 0; lUVSetIndex < mUVSetCount; lUVSetIndex++)
	{
		lUVData[lUVSetIndex] = new float[mVertexCount * lUVCountPerVertex];
		::memcpy(lUVData[lUVSetIndex], mUVData[lUVSetIndex], mVertexCount*lUVCountPerVertex*sizeof(float));
		delete[] mUVData[lUVSetIndex];
	}

	delete[] mUVData;

	lUVData[lUVSetIndex] = new float[mVertexCount * lUVCountPerVertex];
	::memset(lUVData[lUVSetIndex], 0, mVertexCount*lUVCountPerVertex*sizeof(float));

	// Add new data at lUVSetIndex.
	for (i = 0; i < mVertexCount; i++)
	{
		lUVData[lUVSetIndex][i * lUVCountPerVertex + 0] = pUVData[i].x;
		lUVData[lUVSetIndex][i * lUVCountPerVertex + 1] = pUVData[i].y;
	}

	mUVData = lUVData;
	mUVSetCount = lUVSetCount;

	return mUVSetCount - 1;
}

int TriangleBasedGeometry::AddEmptyUVSet()
{
	unsigned int lUVSetCount = mUVSetCount + 1;
	float** lUVData = new float*[lUVSetCount];

	unsigned int lUVSetIndex;

	// Copy present data.
	const int lUVCountPerVertex = GetUVCountPerVertex();
	for (lUVSetIndex = 0; lUVSetIndex < mUVSetCount; lUVSetIndex++)
	{
		lUVData[lUVSetIndex] = new float[mVertexCount * lUVCountPerVertex];
		::memcpy(lUVData[lUVSetIndex], mUVData[lUVSetIndex], mVertexCount*lUVCountPerVertex*sizeof(float));
		delete[] mUVData[lUVSetIndex];
	}

	delete[] mUVData;

	lUVData[lUVSetIndex] = new float[mVertexCount * lUVCountPerVertex];
	::memset(lUVData[lUVSetIndex], 0, mVertexCount*lUVCountPerVertex*sizeof(float));

	mUVData = lUVData;
	mUVSetCount = lUVSetCount;

	return mUVSetCount - 1;
}

int TriangleBasedGeometry::DupUVSet(int pUVSet)
{
	int lNewUVSet = -1;
	if (pUVSet >= 0 && pUVSet < (int)mUVSetCount)
	{
		unsigned int lUVSetCount = mUVSetCount + 1;
		float** lUVData = new float*[lUVSetCount];

		unsigned int lUVSetIndex;

		// Copy present data.
		const int lUVCountPerVertex = GetUVCountPerVertex();
		for (lUVSetIndex = 0; lUVSetIndex < mUVSetCount; lUVSetIndex++)
		{
			lUVData[lUVSetIndex] = new float[mVertexCount * lUVCountPerVertex];
			::memcpy(lUVData[lUVSetIndex], mUVData[lUVSetIndex], mVertexCount*lUVCountPerVertex*sizeof(float));
			delete[] mUVData[lUVSetIndex];
		}

		delete[] mUVData;

		lUVData[lUVSetIndex] = new float[mVertexCount * lUVCountPerVertex];
		::memcpy(lUVData[lUVSetIndex], lUVData[pUVSet], mVertexCount*lUVCountPerVertex*sizeof(float));

		mUVData = lUVData;
		mUVSetCount = lUVSetCount;

		return mUVSetCount - 1;
	}
	return lNewUVSet;
}

int TriangleBasedGeometry::PopUVSet()
{
	deb_assert(mUVSetCount > 0);
	if (mUVSetCount > 0)
	{
		--mUVSetCount;
		delete[] mUVData[mUVSetCount];
		mUVData[mUVSetCount] = 0;
		if (mUVSetCount == 0)
		{
			delete[] mUVData;
			mUVData = 0;
		}
	}
	return mUVSetCount;
}

void TriangleBasedGeometry::SetColorData(uint8* pColorData, ColorFormat pColorFormat)
{
	if (mColorData != 0)
	{
		delete[] mColorData;
		mColorData = 0;
	}

	if (pColorData != 0)
	{
		// Copy color data.
		mColorFormat = pColorFormat;

		int lSize = 4;
		if (mColorFormat == COLOR_RGB)
			lSize = 3;

		mColorData = new uint8[mVertexCount * lSize];

		unsigned int i;
		unsigned int lIndex;
		for (i = 0, lIndex = 0; i < mVertexCount; i++, lIndex += lSize)
		{
			mColorData[lIndex + 0] = pColorData[lIndex + 0];
			mColorData[lIndex + 1] = pColorData[lIndex + 1];
			mColorData[lIndex + 2] = pColorData[lIndex + 2];

			if (mColorFormat == COLOR_RGBA)
				mColorData[lIndex + 3] = pColorData[lIndex + 3];
		}
	}
}

void TriangleBasedGeometry::ConvertColorData(ColorFormat pColorFormat, uint8 pAlpha)
{
	if (mColorFormat != pColorFormat)
	{
		int lSize = 4;
		if (pColorFormat == COLOR_RGB)
			lSize = 3;

		uint8* lColorData = new uint8[mVertexCount * lSize];
		unsigned int i;
		unsigned int lIndex;
		for (i = 0, lIndex = 0; i < mVertexCount; i++, lIndex += lSize)
		{
			lColorData[lIndex + 0] = mColorData[lIndex + 0];
			lColorData[lIndex + 1] = mColorData[lIndex + 1];
			lColorData[lIndex + 2] = mColorData[lIndex + 2];

			if (pColorFormat == COLOR_RGBA)
				lColorData[lIndex + 3] = pAlpha;
		}

		if (mColorData != 0)
			delete[] mColorData;

		mColorData = lColorData;

		mColorFormat = pColorFormat;
	}
}

void TriangleBasedGeometry::FlipTriangles()
{
	if (mIndexData != 0)
	{
		if (mPrimitiveType == Tbc::GeometryBase::TRIANGLES)
		{
			for (unsigned int i = 0; i < mIndexCount; i += 3)
			{
				vtx_idx_t lTemp = mIndexData[i + 0];
				mIndexData[i + 0] = mIndexData[i + 1];
				mIndexData[i + 1] = lTemp;
			}
		}
		else if (mPrimitiveType == Tbc::GeometryBase::TRIANGLE_STRIP && mIndexCount > 2)
		{
			// We only need to flip the first one, and the rest will follow.
			vtx_idx_t lTemp = mIndexData[1];
			mIndexData[1] = mIndexData[2];
			mIndexData[2] = lTemp;
		}
		else
		{
			deb_assert(false);
		}
	}

	Tbc::GeometryBase::SetConvexVolumeCheckValid(false);
}

void TriangleBasedGeometry::FlipTriangle(int pTriangleIndex)
{
	if (mPrimitiveType == Tbc::GeometryBase::TRIANGLES &&
	    mIndexData != 0 &&
	    pTriangleIndex >= 0 &&
	    pTriangleIndex < (int)GetTriangleCount())
	{
		int i = pTriangleIndex * 3;

		vtx_idx_t lTemp = mIndexData[i + 0];
		mIndexData[i + 0] = mIndexData[i + 1];
		mIndexData[i + 1] = lTemp;

		Tbc::GeometryBase::SetConvexVolumeCheckValid(false);
	}
	else
	{
		deb_assert(false);
	}
}

void TriangleBasedGeometry::SplitVertices()
{
	if (mIndexData != 0)
	{
		const Tbc::GeometryBase::BasicMaterialSettings& lMaterial = GetBasicMaterialSettings();
		bool lSmooth = lMaterial.mSmooth;
		float* lOriginalNormalData = 0;
		if (lSmooth == true)
		{
			GenerateVertexNormalData();
			lOriginalNormalData = GetNormalData();
		}
		else
		{
			GenerateSurfaceNormalData();
			lOriginalNormalData = GetSurfaceNormalData();
		}
		
		//
		// First allocate new memory for everything.
		//

		unsigned int lVertexCount = GetTriangleCount() * 3;
		unsigned int lUVSetCount = GetUVSetCount();
		uint32* lIndices = new uint32[lVertexCount];
		float* lVertexData = new float[lVertexCount * 3];
		float* lNormalData = new float[lVertexCount * 3];
		float** lUVData = 0;
		uint8* lColorData = 0;
		unsigned int i;
		
		if (lUVSetCount > 0)
		{
			lUVData = new float*[lUVSetCount];
			for (i = 0; i < lUVSetCount; i++)
			{
				lUVData[i] = new float[lVertexCount * 2];
			}
		}

		int lColorSize = (mColorFormat == COLOR_RGB ? 3 : 4);
		if (mColorData != 0)
		{
			lColorData = new uint8[lVertexCount * lColorSize];
		}

		//
		// Second, copy & set everything...
		//

		int lVIndex  = 0;
		int lUVIndex = 0;
		int lColorIndex = 0;

		for (i = 0; i < GetTriangleCount(); i++)
		{
			unsigned long lT = i * 3;

			lIndices[lT + 0] = lT + 0;
			lIndices[lT + 1] = lT + 1;
			lIndices[lT + 2] = lT + 2;

			unsigned long lV1 = mIndexData[lT + 0];
			unsigned long lV2 = mIndexData[lT + 1];
			unsigned long lV3 = mIndexData[lT + 2];

			unsigned long lV1I = lV1 * 3;
			unsigned long lV2I = lV2 * 3;
			unsigned long lV3I = lV3 * 3;

			unsigned long lC1I = lV1 * lColorSize;
			unsigned long lC2I = lV2 * lColorSize;
			unsigned long lC3I = lV3 * lColorSize;

			// Copy vertex coordinates for 3 vertices.
			lVertexData[lVIndex + 0] = mVertexData[lV1I + 0];
			lVertexData[lVIndex + 1] = mVertexData[lV1I + 1];
			lVertexData[lVIndex + 2] = mVertexData[lV1I + 2];
			lVertexData[lVIndex + 3] = mVertexData[lV2I + 0];
			lVertexData[lVIndex + 4] = mVertexData[lV2I + 1];
			lVertexData[lVIndex + 5] = mVertexData[lV2I + 2];
			lVertexData[lVIndex + 6] = mVertexData[lV3I + 0];
			lVertexData[lVIndex + 7] = mVertexData[lV3I + 1];
			lVertexData[lVIndex + 8] = mVertexData[lV3I + 2];

			if (lSmooth == true)
			{
				// Copy vertex normals...
				lNormalData[lVIndex + 0] = lOriginalNormalData[lV1I + 0];
				lNormalData[lVIndex + 1] = lOriginalNormalData[lV1I + 1];
				lNormalData[lVIndex + 2] = lOriginalNormalData[lV1I + 2];
				lNormalData[lVIndex + 3] = lOriginalNormalData[lV2I + 0];
				lNormalData[lVIndex + 4] = lOriginalNormalData[lV2I + 1];
				lNormalData[lVIndex + 5] = lOriginalNormalData[lV2I + 2];
				lNormalData[lVIndex + 6] = lOriginalNormalData[lV3I + 0];
				lNormalData[lVIndex + 7] = lOriginalNormalData[lV3I + 1];
				lNormalData[lVIndex + 8] = lOriginalNormalData[lV3I + 2];
			}
			else
			{
				// Copy surface normals...
				lNormalData[lVIndex + 0] = lOriginalNormalData[lT + 0];
				lNormalData[lVIndex + 1] = lOriginalNormalData[lT + 1];
				lNormalData[lVIndex + 2] = lOriginalNormalData[lT + 2];
				lNormalData[lVIndex + 3] = lOriginalNormalData[lT + 0];
				lNormalData[lVIndex + 4] = lOriginalNormalData[lT + 1];
				lNormalData[lVIndex + 5] = lOriginalNormalData[lT + 2];
				lNormalData[lVIndex + 6] = lOriginalNormalData[lT + 0];
				lNormalData[lVIndex + 7] = lOriginalNormalData[lT + 1];
				lNormalData[lVIndex + 8] = lOriginalNormalData[lT + 2];
			}

			if (mColorData != 0)
			{
				lColorData[lColorIndex++] = mColorData[lC1I + 0];
				lColorData[lColorIndex++] = mColorData[lC1I + 1];
				lColorData[lColorIndex++] = mColorData[lC1I + 2];
				if (lColorSize == 4)
					lColorData[lColorIndex++] = mColorData[lC1I + 3];
				lColorData[lColorIndex++] = mColorData[lC2I + 0];
				lColorData[lColorIndex++] = mColorData[lC2I + 1];
				lColorData[lColorIndex++] = mColorData[lC2I + 2];
				if (lColorSize == 4)
					lColorData[lColorIndex++] = mColorData[lC2I + 3];
				lColorData[lColorIndex++] = mColorData[lC3I + 0];
				lColorData[lColorIndex++] = mColorData[lC3I + 1];
				lColorData[lColorIndex++] = mColorData[lC3I + 2];
				if (lColorSize == 4)
					lColorData[lColorIndex++] = mColorData[lC3I + 3];
			}

			lV1I = lV1 * 2;
			lV2I = lV2 * 2;
			lV3I = lV3 * 2;
			for (unsigned int j = 0; j < lUVSetCount; j++)
			{
				lUVData[j][lUVIndex + 0] = mUVData[j][lV1 + 0];
				lUVData[j][lUVIndex + 1] = mUVData[j][lV1 + 1];
				lUVData[j][lUVIndex + 2] = mUVData[j][lV2 + 0];
				lUVData[j][lUVIndex + 3] = mUVData[j][lV2 + 1];
				lUVData[j][lUVIndex + 4] = mUVData[j][lV3 + 0];
				lUVData[j][lUVIndex + 5] = mUVData[j][lV3 + 1];
			}

			lVIndex += 9;
			lUVIndex += 6;
		}

		float* lUVSet = 0;
		if (lUVSetCount > 0)
			lUVSet = lUVData[0];

		//
		// Third, update the geometry.
		//

		Set(lVertexData,
		    lNormalData,
		    lUVSet,
		    lColorData,
		    mColorFormat,
		    lIndices,
		    lVertexCount,
		    lVertexCount,
		    Tbc::GeometryBase::TRIANGLES,
		    mGeometryVolatility);

		for (i = 1; i < lUVSetCount; i++)
		{
			AddUVSet(lUVData[i]);
		}

		//
		// Fourth, deallocate all memory.
		//
		delete[] lIndices;
		delete[] lVertexData;
		delete[] lNormalData;

		if (lUVSetCount > 0)
		{
			for (i = 0; i < lUVSetCount; i++)
			{
				delete[] lUVData[i];
			}
			delete[] lUVData;
		}
		
		if (lColorData != 0)
		{
			delete[] lColorData;
		}
	}
	
}

void TriangleBasedGeometry::Translate(const vec3& pPositionOffset)
{
	if (mVertexData != 0 && mVertexCount > 0)
	{
		const int lMaxCount = mVertexCount * 3;
		for (int i = 0; i < lMaxCount; i += 3)
		{
			mVertexData[i + 0] += pPositionOffset.x;
			mVertexData[i + 1] += pPositionOffset.y;
			mVertexData[i + 2] += pPositionOffset.z;
		}
	}

	Tbc::GeometryBase::CalculateBoundingRadius();
}

void TriangleBasedGeometry::Rotate(const RotationMatrix<float>& pRotation)
{
	vec3 lAxisX(pRotation.GetInverseAxisX());
	vec3 lAxisY(pRotation.GetInverseAxisY());
	vec3 lAxisZ(pRotation.GetInverseAxisZ());

	if (mVertexCount > 0)
	{
		const int lMaxCount = mVertexCount * 3;

		if (mVertexData != 0)
		{
			for (int i = 0; i < lMaxCount; i += 3)
			{
				float lX = mVertexData[i + 0];
				float lY = mVertexData[i + 1];
				float lZ = mVertexData[i + 2];

				float lRotX = lAxisX.x * lX + lAxisX.y * lY + lAxisX.z * lZ;
				float lRotY = lAxisY.x * lX + lAxisY.y * lY + lAxisY.z * lZ;
				float lRotZ = lAxisZ.x * lX + lAxisZ.y * lY + lAxisZ.z * lZ;

				mVertexData[i + 0] = lRotX;
				mVertexData[i + 1] = lRotY;
				mVertexData[i + 2] = lRotZ;
			}
		}

		GenerateVertexNormalData();
		float* lVertexNormalData = GetNormalData();
		if (lVertexNormalData != 0)
		{
			for (int i = 0; i < lMaxCount; i += 3)
			{
				float lX = lVertexNormalData[i + 0];
				float lY = lVertexNormalData[i + 1];
				float lZ = lVertexNormalData[i + 2];

				float lRotX = lAxisX.x * lX + lAxisX.y * lY + lAxisX.z * lZ;
				float lRotY = lAxisY.x * lX + lAxisY.y * lY + lAxisY.z * lZ;
				float lRotZ = lAxisZ.x * lX + lAxisZ.y * lY + lAxisZ.z * lZ;

				lVertexNormalData[i + 0] = lRotX;
				lVertexNormalData[i + 1] = lRotY;
				lVertexNormalData[i + 2] = lRotZ;
			}
		}
	}

	if (GetTriangleCount() > 0 && Tbc::GeometryBase::GetSurfaceNormalData() != 0)
	{
		const int lMaxCount = GetTriangleCount() * 3;
		float* lSurfaceNormalData = Tbc::GeometryBase::GetSurfaceNormalData();

		for (int i = 0; i < lMaxCount; i += 3)
		{
			float lX = lSurfaceNormalData[i + 0];
			float lY = lSurfaceNormalData[i + 1];
			float lZ = lSurfaceNormalData[i + 2];

			float lRotX = lAxisX.x * lX + lAxisX.y * lY + lAxisX.z * lZ;
			float lRotY = lAxisY.x * lX + lAxisY.y * lY + lAxisY.z * lZ;
			float lRotZ = lAxisZ.x * lX + lAxisZ.y * lY + lAxisZ.z * lZ;

			lSurfaceNormalData[i + 0] = lRotX;
			lSurfaceNormalData[i + 1] = lRotY;
			lSurfaceNormalData[i + 2] = lRotZ;
		}
	}
}

void TriangleBasedGeometry::AddGeometry(TriangleBasedGeometry* pGeometry)
{
	if (mPrimitiveType != TRIANGLES)
	{
		deb_assert(false);
		return;
	}

	bool lGenerateEdgeData = false;

	Tbc::GeometryBase::ClearSurfaceNormalData();
	Tbc::GeometryBase::ClearVertexNormalData();

	if (Tbc::GeometryBase::GetEdgeData() != 0)
	{
		lGenerateEdgeData = true;
		Tbc::GeometryBase::ClearEdgeData();
	}

	unsigned int lVertexCount = mVertexCount + pGeometry->mVertexCount;
	unsigned int lNumTriangles = GetTriangleCount() + pGeometry->GetTriangleCount();

	float* lVertexData = new float[lVertexCount * 3];
	vtx_idx_t* lIndices = new vtx_idx_t[lNumTriangles * 3];

	unsigned int i;
	int lIndex = 0;
	for (i = 0; i < mVertexCount; i++, lIndex += 3)
	{
		lVertexData[lIndex + 0] = mVertexData[lIndex + 0];
		lVertexData[lIndex + 1] = mVertexData[lIndex + 1];
		lVertexData[lIndex + 2] = mVertexData[lIndex + 2];
	}
	int lSrcIndex = 0;
	for (i = 0; i < pGeometry->mVertexCount; i++, lIndex += 3, lSrcIndex += 3)
	{
		lVertexData[lIndex + 0] = pGeometry->mVertexData[lSrcIndex + 0];
		lVertexData[lIndex + 1] = pGeometry->mVertexData[lSrcIndex + 1];
		lVertexData[lIndex + 2] = pGeometry->mVertexData[lSrcIndex + 2];
	}

	lIndex = 0;
	for (i = 0; i < GetTriangleCount(); i++, lIndex += 3)
	{
		lIndices[lIndex + 0] = mIndexData[lIndex + 0];
		lIndices[lIndex + 1] = mIndexData[lIndex + 1];
		lIndices[lIndex + 2] = mIndexData[lIndex + 2];
	}

	for (i = 0; i < pGeometry->GetTriangleCount(); i++, lIndex += 3)
	{
		uint32 lTriIndex[4];
		pGeometry->GetTriangleIndices(i, lTriIndex);

		lIndices[lIndex + 0] = lTriIndex[0] + mVertexCount;
		lIndices[lIndex + 1] = lTriIndex[1] + mVertexCount;
		lIndices[lIndex + 2] = lTriIndex[2] + mVertexCount;
	}

	delete[] mVertexData;
	delete[] mIndexData;

	mVertexData = lVertexData;
	mIndexData = lIndices;

	mVertexCount = lVertexCount;
	mIndexCount = lNumTriangles * 3;

	if (mUVData != 0)
	{
		float** lUVData = new float*[mUVSetCount];
		for (unsigned int lUVSet = 0; lUVSet < mUVSetCount; lUVSet++)
		{
			lUVData[lUVSet] = new float[mVertexCount * 2];

			lIndex = 0;
			for (i = 0; i < mVertexCount; i++, lIndex += 2)
			{
				lUVData[lUVSet][lIndex + 0] = mUVData[lUVSet][lIndex + 0];
				lUVData[lUVSet][lIndex + 1] = mUVData[lUVSet][lIndex + 1];
			}
			lSrcIndex = 0;

			if (lUVSet < pGeometry->mUVSetCount)
			{
				for (i = 0; i < pGeometry->mVertexCount; i++, lIndex += 2, lSrcIndex += 2)
				{
					lUVData[lUVSet][lIndex + 0] = pGeometry->mUVData[lUVSet][lSrcIndex + 0];
					lUVData[lUVSet][lIndex + 1] = pGeometry->mUVData[lUVSet][lSrcIndex + 1];
				}
			}
			else
			{
				for (i = 0; i < pGeometry->mVertexCount; i++, lIndex += 2)
				{
					lUVData[lUVSet][lIndex + 0] = 0;
					lUVData[lUVSet][lIndex + 1] = 0;
				}
			}
	
			delete[] mUVData[lUVSet];
		}
		delete[] mUVData;
		mUVData = lUVData;
	}

	if (mColorData != 0)
	{
		uint8* lColorData = new uint8[mVertexCount * 3];

		lIndex = 0;
		for (i = 0; i < mVertexCount; i++, lIndex += 3)
		{
			lColorData[lIndex + 0] = mColorData[lIndex + 0];
			lColorData[lIndex + 1] = mColorData[lIndex + 1];
			lColorData[lIndex + 2] = mColorData[lIndex + 2];
		}
		lSrcIndex = 0;

		if (pGeometry->mColorData != 0)
		{
			for (i = 0; i < pGeometry->mVertexCount; i++, lIndex += 3, lSrcIndex += 3)
			{
				lColorData[lIndex + 0] = pGeometry->mColorData[lSrcIndex + 0];
				lColorData[lIndex + 1] = pGeometry->mColorData[lSrcIndex + 1];
				lColorData[lIndex + 2] = pGeometry->mColorData[lSrcIndex + 2];
			}
		}
		else
		{
			for (i = 0; i < pGeometry->mVertexCount; i++, lIndex += 3)
			{
				lColorData[lIndex + 0] = 255;
				lColorData[lIndex + 1] = 255;
				lColorData[lIndex + 2] = 255;
			}
		}

		delete[] mColorData;
		mColorData = lColorData;
	}

	GenerateVertexNormalData();

	if (lGenerateEdgeData == true)
	{
		Tbc::GeometryBase::GenerateEdgeData();
	}

	Tbc::GeometryBase::SetSolidVolumeCheckValid(false);
	Tbc::GeometryBase::SetSingleObjectCheckValid(false);
	Tbc::GeometryBase::SetConvexVolumeCheckValid(false);

	Tbc::GeometryBase::CalculateBoundingRadius();
}

unsigned int TriangleBasedGeometry::GetMaxVertexCount() const
{
	return mVertexCount;
}

unsigned int TriangleBasedGeometry::GetMaxIndexCount() const
{
	return mIndexCount;
}

unsigned int TriangleBasedGeometry::GetVertexCount() const
{
	return mVertexCount;
}

unsigned int TriangleBasedGeometry::GetIndexCount() const
{
	return mIndexCount;
}

unsigned int TriangleBasedGeometry::GetUVSetCount() const
{
	return mUVSetCount;
}

float* TriangleBasedGeometry::GetVertexData() const
{
	return mVertexData;
}

float* TriangleBasedGeometry::GetUVData(unsigned int pUVSet) const
{
	if (mUVData == 0)
		return 0;

	return mUVData[pUVSet];
}

uint8* TriangleBasedGeometry::GetColorData() const
{
	return mColorData;
}

vtx_idx_t* TriangleBasedGeometry::GetIndexData() const
{
	return mIndexData;
}

Tbc::GeometryBase::GeometryVolatility TriangleBasedGeometry::GetGeometryVolatility() const
{
	//deb_assert(mGeometryVolatility == GEOM_STATIC);
	return (mGeometryVolatility);
}

void TriangleBasedGeometry::SetGeometryVolatility(Tbc::GeometryBase::GeometryVolatility pVolatility)
{
	mGeometryVolatility = pVolatility;
	//deb_assert(mGeometryVolatility == GEOM_STATIC);
}

TriangleBasedGeometry::ColorFormat TriangleBasedGeometry::GetColorFormat() const
{
	deb_assert(mColorFormat == COLOR_RGB || mColorFormat == COLOR_RGBA);
	return mColorFormat;
}



}
