
// Author: Jonas Byström
// Copyright (c) 2002-, Pixel Doctrine



#pragma once

#include <math.h>
#include <list>
#include "../../ThirdParty/FastDelegate/FastDelegate.h"
#include "../../Lepra/Include/String.h"
#include "../../Lepra/Include/Vector3D.h"
#include "../../Lepra/Include/Transformation.h"
#include "TBC.h"
#include "PortalManager.h"




namespace TBC
{



class BoneAnimator;



class GeometryBase
{
public:
	// UiTbc::Renderer implements this interface and removes the geometry
	// if it is deleted.
	class Listener
	{
	public:
		virtual void DeletingGeometry(GeometryBase* pGeometry) = 0;
	};

	typedef fastdelegate::FastDelegate0<void> RenderCallback;

	friend class BasicMeshCreator;
	friend class PortalManager;

	class Edge
	{
	public:
		enum
		{
			INVALID_INDEX = -1,
		};

		Edge();
		Edge(const Edge& pEdge);
		Edge(const Edge* pEdge);
		virtual ~Edge();

		void ClearAll();

		void Copy(const Edge* pEdge);

		void AddTriangle(int pTriangleIndex);
		void RemoveTriangle(int pTriangleIndex);

		bool HaveTriangle(int pTriangleIndex);
		bool HaveTriangle(int pTriangleIndex, int& pTrianglePos);

		bool IsSameEdge(int pVertexIndex1, int pVertexIndex2);
		bool IsSameEdge(int pVertexIndex1, 
				int pVertexIndex2,
				int* pTriangles, 
				int pNumTriangles);

		//int* mTriangle;
		int mTriangle[2];
		int mVertex[2];
		int mTriangleCount;
		//int mTriangleElementCount;

		Edge* mReserved;
	};

	// Buffer object usage hints. These will only have effect
	// when rendering the geometry using hardware acceleration
	// APIs. If using a software renderer these won't make any
	// difference.
	enum GeometryVolatility
	{
		GEOM_STATIC = 1,	// Geometry that will never change.
		GEOM_SEMI_STATIC,	// Geometry that will never change.
		GEOM_DYNAMIC,		// Geometry that will change occasionally (like terrain).
		GEOM_VOLATILE,		// Geometry that will change between every frame.
	};

	// Vertex color format.
	enum ColorFormat
	{
		COLOR_RGB = 1,
		COLOR_RGBA,
	};

	enum PrimitiveType
	{
		TRIANGLES = 1,
		TRIANGLE_STRIP,
		LINES,
		LINE_LOOP,
	};

	class BasicMaterialSettings
	{
	public:
		BasicMaterialSettings();
		BasicMaterialSettings(const Vector3DF& pAmbient, const Vector3DF& pDiffuse,
			const Vector3DF& pSpecular, float pShininess,
			float pAlpha, bool pSmooth);
		void SetColor(float pRed, float pGreen, float pBlue);
		void Set(const Vector3DF& pAmbient, const Vector3DF& pDiffuse,
			const Vector3DF& pSpecular, float pShininess,
			float pAlpha, bool pSmooth);

		Vector3DF mAmbient;
		Vector3DF mDiffuse;
		Vector3DF mSpecular;
		float mShininess;	// Specular "exponent".
		float mAlpha;	// Used on blended (and transparent? materials.
		bool mSmooth;	// Smooth shaded or flat shaded?
	};

	GeometryBase();
	virtual ~GeometryBase();
	
	void AddListener(Listener* pListener);
	void RemoveListener(Listener* pListener);
	const RenderCallback& GetPreRenderCallback() const;
	void SetPreRenderCallback(const RenderCallback& pCallback);
	const RenderCallback& GetPostRenderCallback() const;
	void SetPostRenderCallback(const RenderCallback& pCallback);

	virtual bool IsGeometryReference();
	void SetExcludeCulling();
	bool IsExcludeCulling() const;
	float GetScale() const;
	void SetScale(float pScale);

	virtual GeometryVolatility GetGeometryVolatility() const = 0;
	virtual void SetGeometryVolatility(GeometryVolatility pVolatility) = 0;

	void SetPrimitiveType(PrimitiveType pType);
	PrimitiveType GetPrimitiveType() const;

	virtual unsigned int GetMaxVertexCount() const = 0;
	        unsigned int GetMaxTriangleCount() const;
	virtual unsigned int GetMaxIndexCount() const = 0;

	virtual unsigned int GetVertexCount() const = 0;
	        unsigned int GetTriangleCount() const;
	virtual unsigned int GetIndexCount() const = 0;
	virtual unsigned int GetUVSetCount() const = 0;
	unsigned int GetEdgeCount() const;

	// Sets the UV-set to use when generating tangent- and bitangent vectors.
	// Default is 0. Tangents and bitangents are used with bump/normal mapping.
	void SetTangentsUVSet(unsigned int pUVSet);

	virtual float*         GetVertexData() const                  = 0;
	virtual float*         GetUVData(unsigned int pUVSet) const   = 0;
	virtual vtx_idx_t*     GetIndexData() const                   = 0;
	virtual Lepra::uint8*  GetColorData() const                   = 0;

	virtual float* GetNormalData() const;
	float*         GetSurfaceNormalData() const;
	Edge*          GetEdgeData() const;
	float*         GetTangentData() const;	// Used with bump/normal mapping.
	float*         GetBitangentData() const;	// Used with bump/normal mapping.

	// 0 <= pTriangle < GetNumTriangles().
	// Given the triangle index (pTriangle), the function returns the three
	// indices by setting the values in pIndices.
	void GetTriangleIndices(int pTriangle, Lepra::uint32 pIndices[3]) const;

	// Deletes the corresponding arrays and cleans stuff up.
	virtual void ClearVertexNormalData();
	void ClearSurfaceNormalData();
	void ClearEdgeData();
	void ClearTangentAndBitangentData();

	// Allocates memory for the corresponding arrays, if needed.
	void AllocVertexNormalData();
	void AllocSurfaceNormalData();
	void AllocTangentAndBitangentData();

	// Allocates memory for the corresponding arrays, if needed,
	// and generates the data (if needed).
	void GenerateVertexNormalData();
	void GenerateSurfaceNormalData();
	void GenerateEdgeData();
	void GenerateTangentAndBitangentData();



	// Vertices and normals are supposed to be updated at the same time.
	// Surface normals are considered changed if either vertices or
	// indices are changed.
	// Tangents are considered changed if either vertices of uv-coordinates
	// are changed.
	bool GetVertexDataChanged() const;
	bool GetUVDataChanged() const;
	bool GetColorDataChanged() const;
	bool GetIndexDataChanged() const;

	void SetVertexDataChanged(bool pChanged);
	void SetUVDataChanged(bool pChanged);
	void SetColorDataChanged(bool pChanged);
	void SetIndexDataChanged(bool pChanged);

	void SetVertexNormalsValid();
	void SetSurfaceNormalsValid();

	virtual ColorFormat GetColorFormat() const;

	// This has been renamed from SetUserData(). These functions are used
	// by the renderer to associate some arbitrary data with the geometry.
	// It would be better design to remove these functions and use a hash
	// table instead to do the association, but that is not as efficient.
	void SetRendererData(void* pRendererData);
	virtual void* GetRendererData() const;

	size_t GetExtraData() const;
	void SetExtraData(size_t pExtraData);

	void SetTransformation(const TransformationF& pTransformation);
	const TransformationF& GetBaseTransformation() const;
	virtual const TransformationF& GetTransformation();
	bool GetTransformationChanged() const;
	void SetTransformationChanged(bool pTransformationChanged);
	bool GetBigOrientationChanged() const;
	void SetBigOrientationChanged(bool pOrientationChanged);
	const QuaternionF& GetLastBigOrientation() const;
	float GetBigOrientationThreshold() const;
	void SetBigOrientationThreshold(float pBigOrientationThreshold);
	static void SetDefaultBigOrientationThreshold(float pBigOrientationThreshold);

	void SetLastFrameVisible(unsigned int pLastFrameVisible);
	unsigned int GetLastFrameVisible() const;
	void SetAlwaysVisible(bool pAlwaysVisible);
	bool GetAlwaysVisible() const;

	const BasicMaterialSettings& GetBasicMaterialSettings() const;
	BasicMaterialSettings& GetBasicMaterialSettings();
	void SetBasicMaterialSettings(const BasicMaterialSettings& pMatSettings);

	// Calling GetBoundingRadius() will recalculate the bounding radius if necessary.
	float GetBoundingRadius();
	float GetBoundingRadius() const;

	// Sets the bounding radius. Once set, GetBoundingRadius() will never try to 
	// recalculate the radius again.
	void SetBoundingRadius(float pBoundingRadius);

	// Explicitly recalculate the radius.
	void CalculateBoundingRadius();

	// The following functions will generate edge data.
	bool IsSolidVolume();	// Returns true if all edges are shared by two triangles.
	bool IsSingleObject();	// Returns true if all polygons are connected to each other
				// as one object.
	bool IsConvexVolume();	// Returns true if it is a solid volume, and
				// there is no concave angle between any two triangles.
				// Will generate surface normals if needed.
	bool IsTwoSided() const;
	void SetTwoSided(bool pIsTwoSided);

	// Debug functions.
	bool VerifyIndexData();

	// Functions used to handle UV-animations. Couldn't find any
	// better place to put this code.
	void SetUVAnimator(BoneAnimator* pUVAnimator);
	BoneAnimator* GetUVAnimator();
	const TransformationF& GetUVTransform() const;

	void SetSurfaceNormalData(const float* pSurfaceNormalData);
	void SetVertexNormalData(const float* pVertexNormalData, unsigned int pNumVertices);
	void SetTangentAndBitangentData(const float* pTangentData, const float* pBitangentData, unsigned int pNumVertices);

	void Copy(GeometryBase* pGeometry);
	void ClearAll();

	void SetSolidVolumeCheckValid(bool pValid);
	void SetSingleObjectCheckValid(bool pValid);
	void SetConvexVolumeCheckValid(bool pValid);

	// Used by PortalManager.
	void SetParentCell(PortalManager::Cell* pCell);
	PortalManager::Cell* GetParentCell();

	enum //Flags
	{
		BOUNDING_RADIUS_VALID		= (1 << 0),
		BOUNDING_RADIUS_ALWAYS_VALID	= (1 << 1),
		SURFACE_NORMALS_VALID		= (1 << 2),
		VERTEX_NORMALS_VALID		= (1 << 3),
		TANGENTS_VALID			= (1 << 4),
		SOLID_VOLUME_VALID		= (1 << 5),
		SINGLE_OBJECT_VALID		= (1 << 6),
		CONVEX_VOLUME_VALID		= (1 << 7),
		VALID_FLAGS_MASK		= CONVEX_VOLUME_VALID-1,

		VERTEX_DATA_CHANGED		= (1 << 8),
		UV_DATA_CHANGED			= (1 << 9),
		COLOR_DATA_CHANGED		= (1 << 10),
		INDEX_DATA_CHANGED		= (1 << 11),
		IS_SOLID_VOLUME			= (1 << 12),
		IS_SINGLE_OBJECT		= (1 << 13),
		IS_CONVEX_VOLUME		= (1 << 14),
		ALWAYS_VISIBLE			= (1 << 15),
		TRANSFORMATION_CHANGED		= (1 << 16),
		REF_TRANSFORMATION_CHANGED	= (1 << 17),
		BIG_ORIENTATION_CHANGED		= (1 << 18),
		EXCLUDE_CULLING			= (1 << 19),
		IS_TWO_SIDED			= (1 << 20),
	};

	void SetFlag(Lepra::uint32 pFlag, bool pValue);
	void SetFlag(Lepra::uint32 pFlag);
	void ClearFlag(Lepra::uint32 pFlag);
	bool CheckFlag(Lepra::uint32 pFlag) const;
	uint32 GetFlags() const;
	static void SetDefaultFlags(Lepra::uint32 pFlags);

protected:
	typedef std::list<Listener*> ListenerList;

	Lepra::uint32 mFlags;
	PrimitiveType mPrimitiveType;

	BasicMaterialSettings mMaterialSettings;

	float mBoundingRadius;
	float mScale;

	// Surface normals are primarily used when generating
	// the shadow volume (used to perform stencil shadows).
	float* mSurfaceNormalData;
	unsigned int mSurfaceNormalCount;

	float* mVertexNormalData;

	Edge* mEdgeData;
	unsigned int mEdgeCount;

	float* mTangentData;   // Contains (x, y, z) triplets for tangent vectors.
	float* mBitangentData; // Contains (x, y, z) triplets for bitangent vectors.
	unsigned int mTangentsUVSet;

	// Contains arbitrary data. This is used by Renderer and
	// subclasses to store associated data.
	void* mRendererData;

	// The Cell pointer used by PortalManager. This is declared as void*
	// to avoid including PortalManager.h. 
	PortalManager::Cell* mParentCell;

	unsigned int mLastFrameVisible;

	TransformationF mTransformation;
	QuaternionF mBigOrientation;

	BoneAnimator* mUVAnimator;

	ListenerList mListenerList;
	RenderCallback mPreRenderCallback;
	RenderCallback mPostRenderCallback;

	size_t mExtraData;

	float mBigOrientationThreshold;

	static Lepra::uint32 mDefaultFlags;
	static float mDefaultBigOrientationThreshold;

public:
	LEPRA_DEBUG_CODE(str mName);

	LOG_CLASS_DECLARE();
};



}
