/*
	Class:  ProgressiveTriangleGeometry
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games
*/

#ifndef UIPROGRESSIVETRIANGLEGEOMETRY_H
#define UIPROGRESSIVETRIANGLEGEOMETRY_H

#include "UiTBC.h"
#include "UiTriangleBasedGeometry.h"
#include "../../TBC/Include/GeometryBase.h"
#include <list>

#include <crtdbg.h>

namespace UiTbc
{

class ProgressiveTriangleGeometry : public TBC::GeometryBase
{
public:
	ProgressiveTriangleGeometry();
	ProgressiveTriangleGeometry(const ProgressiveTriangleGeometry& pProgressiveGeometry);
	ProgressiveTriangleGeometry(TriangleBasedGeometry& pGeometry);
	virtual ~ProgressiveTriangleGeometry();

	void Copy(const ProgressiveTriangleGeometry& pProgressiveGeometry);
	void Set(TriangleBasedGeometry& pGeometry);

	// SetDetailLevel() will collapse or expand edges to the given level of detail.
	// pLevelOfDetail must be a value between 0 and 1, where 0 is the lowest 
	// detail level, and 1 is the highest.
	void SetDetailLevel(float pLevelOfDetail);

	void GetCurrentState(TriangleBasedGeometry& pGeometry);

	// Data access...
	inline virtual unsigned int GetMaxVertexCount() const;
	inline virtual unsigned int GetMaxNumIndices() const;

	inline virtual unsigned int GetVertexCount() const;
	inline virtual unsigned int GetNumIndices() const;
	inline virtual unsigned int GetNumUVSets()    const;

	// When writing to any of the arrays returned by the following functions,
	// the results of any future calls to the member functions is undefined.
	inline virtual float*         GetVertexData() const;
	inline virtual float*         GetUVData(unsigned int pUVSet) const;
	inline virtual Lepra::uint32* GetIndices() const;
	inline virtual Lepra::uint8*  GetColorData() const;
	inline virtual float*         GetNormalData() const; // Vertex normals..

	inline virtual void ClearVertexNormalData();
	inline virtual bool HaveVertexNormalData();

	inline ColorFormat GetColorFormat() const;

	// Overloads from TBC::GeometryBase.
	virtual inline GeometryVolatility GetGeometryVolatility() const;

private:

	/*
		Some structures and functions used to make life easier 
		when producing the progressive mesh.
	*/

	enum
	{
		VX = 0,
		VY,
		VZ,
		VU,
		VV,
		VR,
		VG,
		VB,
		VA,
		VNX,
		VNY,
		VNZ,

		V_NUMSCALARS
	};

	inline static void CopyScalars(float* pDest, const float* pSource, int pN = V_NUMSCALARS, float pScale = 1.0f)
	{
		for (int i = 0; i < pN; i++)
		{
			pDest[i] = pSource[i] * pScale;
		}
	}

	inline static void CopyScalarsUC(unsigned char* pDest, const float* pSource, int pN = V_NUMSCALARS, float pScale = 255.0f)
	{
		for (int i = 0; i < pN; i++)
		{
			pDest[i] = (unsigned char)(pSource[i] * pScale);
		}
	}

	inline static void SetScalars(float* pDest, float pValue)
	{
		for (int i = 0; i < V_NUMSCALARS; i++)
		{
			pDest[i] = pValue;
		}
	}

	inline static void SetDeltaScalars(float* pDest, float* pSrc1, float* pSrc2)
	{
		for (int i = 0; i < V_NUMSCALARS; i++)
		{
			pDest[i] = (pSrc1[i] - pSrc2[i]) * 0.5f;
		}
	}

	inline static void CopyAddScalars(float* pDest, float* pSrc1, float* pSrc2, float pFrac = 1.0f)
	{
		for (int i = 0; i < V_NUMSCALARS; i++)
		{
			pDest[i] = pSrc1[i] + pSrc2[i] * pFrac;
		}
	}

	class Triangle;
	class Edge;
	class Vertex;
	typedef std::list<Triangle*> TriangleList;
	typedef std::list<Vertex*>   VertexList;
	typedef std::list<Edge*>     EdgeList;

	class Vertex
	{
	public:

		Vertex() :
			mError(0),
			mTwin(0)
		{
			SetScalars(mScalars, 0.0f);
		}

		Vertex(Vertex* pV) :
			mError(pV->mError),
			mTwin(0)
		{
			CopyScalars(mScalars, pV->mScalars);
		}

		inline float& x() { return mScalars[VX]; }
		inline float& y() { return mScalars[VY]; }
		inline float& z() { return mScalars[VZ]; }
		inline float& u() { return mScalars[VU]; }
		inline float& v() { return mScalars[VV]; }
		inline float& r() { return mScalars[VR]; }
		inline float& g() { return mScalars[VG]; }
		inline float& b() { return mScalars[VB]; }
		inline float& a() { return mScalars[VA]; }
		inline float& nx() { return mScalars[VNX]; }
		inline float& ny() { return mScalars[VNY]; }
		inline float& nz() { return mScalars[VNZ]; }

		float mScalars[V_NUMSCALARS];
		float mError; // Used in FindEdgeToCollapse().

		Vertex* mTwin;
	};

	class Edge
	{
	public:

		Edge()
		{
			mV1 = 0;
			mV2 = 0;
		}

		~Edge()
		{
			mV1 = 0;
			mV2 = 0;
			mTriangleList.clear();
		}

		Vertex* mV1;
		Vertex* mV2;
		TriangleList mTriangleList;

		inline bool HaveVertex(Vertex* pVertex)
		{
			return (mV1 == pVertex || mV2 == pVertex);
		}
	};

	class Triangle
	{
	public:

		Triangle() :
			mV1(0),
			mV2(0),
			mV3(0)
		{
		}

		Vertex* mV1;
		Vertex* mV2;
		Vertex* mV3;

		Lepra::Vector3DF mNormal;    // Used in FindEdgeToCollapse().
		std::list<int> mVertexIndexHistory;
		VertexList mVertexHistory;

		inline bool HaveVertex(Vertex* pVertex)
		{
			return (pVertex == mV1 || pVertex == mV2 || pVertex == mV3);
		}

		inline void ReplaceVertex(Vertex* pVertex, Vertex* pReplacement)
		{
			if (mV1 == pVertex)
			{
				mVertexIndexHistory.push_back(0);
				mVertexHistory.push_back(mV1);
				mV1 = pReplacement;
			}
			if (mV2 == pVertex)
			{
				mVertexIndexHistory.push_back(1);
				mVertexHistory.push_back(mV2);
				mV2 = pReplacement;
			}
			if (mV3 == pVertex)
			{
				mVertexIndexHistory.push_back(2);
				mVertexHistory.push_back(mV3);
				mV3 = pReplacement;
			}
		}
	};

	// A class used during construction of the progressive mesh (in function Set()).
	class VertexSplit
	{
	public:
		enum
		{
			INVALID_INDEX = -1,
		};

		VertexSplit()
		{
			SetScalars(mDeltaScalars, 0.0f);
			SetScalars(mPivotScalars, 0.0f);

			mNumNewTriangles  = 0;
			mNumOldTriangles  = 0;
			mNumOldVertices   = 0;

			mIndexFix        = 0;
			mIndexFixIndex   = 0;
			mNumIndexFixes    = 0;
			mVertexToSplit    = 0;

			mVertexToSplit    = 0;
		}

		virtual ~VertexSplit()
		{
			ClearAll();
		}

		void ClearAll()
		{
			SetScalars(mDeltaScalars, 0.0f);
			SetScalars(mPivotScalars, 0.0f);

			mVertexToSplit    = 0;
			mNumNewTriangles  = 0;
			mNumOldTriangles  = 0;
			mNumOldVertices   = 0;

			if (mIndexFix != 0)
			{
				delete[] mIndexFix;
				delete[] mIndexFixIndex;
				mIndexFix = 0;
				mIndexFixIndex = 0;
			}

			mNumIndexFixes = 0;
			mVertexToSplit = 0;
			mNewVertex = 0;

			mNewTriangles.clear();
			mFixTriangles.clear();
		}

		void Copy(const VertexSplit& pVS)
		{
			LightCopy(pVS);

			mNumIndexFixes = pVS.mNumIndexFixes;
			mNewVertex = pVS.mNewVertex;

			if (mNumIndexFixes > 0)
			{
				mIndexFix = new int[mNumIndexFixes];
				mIndexFixIndex = new int[mNumIndexFixes * 2];
			}

			for (int i = 0; i < mNumIndexFixes; i++)
			{
				mIndexFix[i] = pVS.mIndexFix[i];
				mIndexFixIndex[i * 2 + 0] = pVS.mIndexFixIndex[i * 2 + 0];
				mIndexFixIndex[i * 2 + 1] = pVS.mIndexFixIndex[i * 2 + 1];
			}

			TriangleList::const_iterator lTriIter;
			for (lTriIter = pVS.mNewTriangles.begin();
				lTriIter != pVS.mNewTriangles.end();
				++lTriIter)
			{
				mNewTriangles.push_back(*lTriIter);
			}

			for (lTriIter = pVS.mFixTriangles.begin();
				lTriIter != pVS.mFixTriangles.end();
				++lTriIter)
			{
				mFixTriangles.push_back(*lTriIter);
			}
		}

		void LightCopy(const VertexSplit& pVS)
		{
			ClearAll();

			CopyScalars(mDeltaScalars, pVS.mDeltaScalars);
			CopyScalars(mPivotScalars, pVS.mPivotScalars);

			mVertexToSplit    = pVS.mVertexToSplit;
			mNumNewTriangles  = pVS.mNumNewTriangles;
			mNumOldTriangles  = pVS.mNumOldTriangles;
			mNumOldVertices   = pVS.mNumOldVertices;
		}

		float mDeltaScalars[V_NUMSCALARS];
		float mPivotScalars[V_NUMSCALARS];

		// Stores all indices into the index array, where the indices should be updated with
		// new values... Eh.. 
		// When splitting a vertex, all "old" triangles that point at that vertex,
		// needs to point at the new vertex instead. The indices to update are indexed
		// by this array. I hope that helped to clear things up.
		int* mIndexFix;		// Stores the index into the index array.
		int* mIndexFixIndex; // Stores the actual values to write to the index arrays.
		int mNumIndexFixes;
		int mVertexToSplitIndex;
		int mNumOldTriangles;
		int mNumNewTriangles;
		int mNumOldVertices;

		Vertex*    mVertexToSplit;
		Vertex*    mNewVertex;
		TriangleList mNewTriangles;
		TriangleList mFixTriangles; // Triangles that needs changed indices.
	};

	typedef std::list<VertexSplit*> VertexSplitList;

	void ClearAll();

	void FindEdgeToCollapse(VertexList& pOrgVertexList,
				TriangleList& pOrgTriangleList,
				VertexList& pVertexList,
				TriangleList& pTriangleList,
				Edge& pEdge);

	unsigned int mBaseVertexCount;
	unsigned int mBaseTriangleCount;

	unsigned int mCurrentVertexCount;
	unsigned int mCurrentTriangleCount;

	unsigned int mMaxVertexCount;
	unsigned int mMaxTriangleCount;

	float* mCurrentVertexData;			// Triplets of (x, y, z).
	float* mCurrentUVData;				// Doublets of (u, v).
	float* mCurrentColorData;
	float* mCurrentNormalData;			// Triplets of (x, y, z).
	Lepra::uint8* mCurrentColorData8;
	Lepra::uint32* mCurrentIndices;		// Triplets of vertex indices.

	float* mBaseVertexData;			// Triplets of (x, y, z).
	float* mBaseUVData;				// Doublets of (u, v).
	float* mBaseColorData;
	float* mBaseNormalData;			// Triplets of (x, y, z).
	Lepra::uint32* mBaseIndices;	// Triplets of vertex indices.

	unsigned int mCurrentVSplit;
	unsigned int mNumVertexSplits;
	VertexSplit* mVertexSplit;

	ColorFormat mColorFormat;
};

unsigned int ProgressiveTriangleGeometry::GetMaxVertexCount() const
{
	return mMaxVertexCount;
}

unsigned int ProgressiveTriangleGeometry::GetMaxNumIndices() const
{
	return mMaxTriangleCount * 3;
}

unsigned int ProgressiveTriangleGeometry::GetVertexCount() const
{
	return mCurrentVertexCount;
}

unsigned int ProgressiveTriangleGeometry::GetNumIndices() const
{
	return mCurrentTriangleCount * 3;
}

unsigned int ProgressiveTriangleGeometry::GetNumUVSets() const
{
	if (mBaseUVData != 0)
		return 1;
	else
		return 0;
}

float* ProgressiveTriangleGeometry::GetVertexData() const
{
	return mCurrentVertexData;
}

float* ProgressiveTriangleGeometry::GetUVData(unsigned int /*pUVSet*/) const
{
	return mCurrentUVData;
}

Lepra::uint32* ProgressiveTriangleGeometry::GetIndices() const
{
	return mCurrentIndices;
}

Lepra::uint8* ProgressiveTriangleGeometry::GetColorData() const
{
	return mCurrentColorData8;
}

float* ProgressiveTriangleGeometry::GetNormalData() const
{
	return mCurrentNormalData;
}

TBC::GeometryBase::GeometryVolatility ProgressiveTriangleGeometry::GetGeometryVolatility() const
{
	// Volatile, because the geometry is constantly changed.
	return TBC::GeometryBase::GEOM_VOLATILE;
}

TBC::GeometryBase::ColorFormat ProgressiveTriangleGeometry::GetColorFormat() const
{
	return mColorFormat;
}

void ProgressiveTriangleGeometry::ClearVertexNormalData()
{
	// Do nothing.
}

bool ProgressiveTriangleGeometry::HaveVertexNormalData()
{
	return (mCurrentNormalData != 0);
}

} // End namespace.

#endif