
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#ifndef UITRIANGLEBASEDGEOMETRY_H
#define UITRIANGLEBASEDGEOMETRY_H



#include "../../Lepra/Include/Graphics2D.h"
#include "../../Lepra/Include/Vector3D.h"
#include "../../Lepra/Include/Vector2D.h"
#include "../../Tbc/Include/GeometryBase.h"
#include "UiTbc.h"



namespace UiTbc
{



class TriangleBasedGeometry: public Tbc::GeometryBase
{
public:
	TriangleBasedGeometry();
	TriangleBasedGeometry(const TriangleBasedGeometry& pGeometry);

	TriangleBasedGeometry(const vec3* pVertices,      // Must containt pVertexCount elements.
			      const vec3* pVertexNormals, // NULL or must contain pVertexCount elements.
			      const Vector2D<float>* pUV,      // NULL or must contain pVertexCount elements.
			      const Color* pColor,             // NULL or must contain pVertexCount elements.
			      ColorFormat pColorFormat,
			      const Lepra::uint32* pIndices,
			      unsigned int pVertexCount,
			      unsigned int pIndexCount,
			      Tbc::GeometryBase::PrimitiveType pPrimitiveType,
			      Tbc::GeometryBase::GeometryVolatility pGeomType);

	TriangleBasedGeometry(const float* pVertexData,
			      const float* pVertexNormals,
			      const float* pUVData,
			      const Lepra::uint8* pColorData,
			      ColorFormat pColorFormat,
			      const Lepra::uint32* pIndices,
			      unsigned int pVertexCount,
			      unsigned int pIndexCount,
			      Tbc::GeometryBase::PrimitiveType pPrimitiveType,
			      Tbc::GeometryBase::GeometryVolatility pGeomType);
	virtual ~TriangleBasedGeometry();

	void Copy(const TriangleBasedGeometry& pGeometry);

	void Set(const vec3* pVertices,
		 const vec3* pVertexNormals,
		 const Vector2D<float>* pUV,
		 const Color* pColor,
		 ColorFormat pColorFormat,
		 const Lepra::uint32* pIndices,
		 unsigned int pVertexCount,
		 unsigned int pIndexCount,
		 Tbc::GeometryBase::PrimitiveType pPrimitiveType,
		 Tbc::GeometryBase::GeometryVolatility pGeomType);

	void Set(const float* pVertexData,
		 const float* pVertexNormals,
		 const float* pUVData,
		 const Lepra::uint8* pColorData,
		 ColorFormat pColorFormat,
		 const Lepra::uint32* pIndices,
		 unsigned int pVertexCount,
		 unsigned int pIndexCount,
		 Tbc::GeometryBase::PrimitiveType pPrimitiveType,
		 Tbc::GeometryBase::GeometryVolatility pGeomType);

	// AddUVSet() returns the index of the uv set.
	// pUVData must contain mVertexCount * 2 elements.
	int AddUVSet(const float* pUVData);
	int AddUVSet(const Vector2D<float>* pUVData);
	int AddEmptyUVSet();
	int DupUVSet(int pUVSet);
	int PopUVSet();

	// Data access...

	virtual unsigned int GetMaxVertexCount() const;
	virtual unsigned int GetMaxIndexCount() const;

	virtual unsigned int GetVertexCount() const;
	virtual unsigned int GetIndexCount() const;
	virtual unsigned int GetUVSetCount() const;

	virtual float* GetVertexData() const;
	virtual float* GetUVData(unsigned int pUVSet = 0) const;
	virtual vtx_idx_t* GetIndexData() const;
	virtual Lepra::uint8* GetColorData() const;

	void SetColorData(unsigned char* pColorData, ColorFormat pColorFormat);
	void ConvertColorData(ColorFormat pColorFormat, unsigned char pAlpha);
	ColorFormat GetColorFormat() const;

	// Misc work funcs.
	void ClearAll();

	// Know what you're doing when calling this internal function.
	virtual void SetIndexData(vtx_idx_t* pIndexData, unsigned pIndexCount, unsigned pMaxIndexCount);

	// Will erase all current data.
	void SetPolygon(vec3* pVertices, unsigned int pVertexCount);

	// Flips the triangles from being clockwise to counter clockwise,
	// and vice versa. FlipTriangle() only works if the primitive type
	// is TRIANGLES.
	void FlipTriangles(); 
	void FlipTriangle(int pTriangleIndex);

	// Generates unique vertices for each triangle. New UV-coordinate arrays and Color-
	// arrays will be created, and the vertex normals will be regenerated according
	// to the BasicMaterialSettings::mSmooth. This is an irreversible operation,
	// so use it with care. 
	void SplitVertices();

	void Translate(const vec3& pPositionOffset);
	void Rotate(const RotationMatrix<float>& pRotation);

	// This definitly won't work if "this" has a primitive type other than TRIANGLES.
	void AddGeometry(TriangleBasedGeometry* pGeometry);
	
	// Use this if IsSingleObject() returns false and you want to get the separate objects.
	// The pointer that is returned will point to an array of TriangleBasedGeometry.
	// The number of elements in this array is stored in pNumObjects.
	// The caller is responsible of deleting this array.
	TriangleBasedGeometry* GetSeparateObjects(int& pNumObjects);

	// Overloads from Tbc::GeometryBase.
	GeometryVolatility GetGeometryVolatility() const;
	void SetGeometryVolatility(GeometryVolatility pVolatility);

private:

	unsigned int mVertexCount;
	unsigned int mIndexCount;
	unsigned int mMaxIndexCount;
	unsigned int mUVSetCount;

	float* mVertexData;			// Triplets of (x, y, z).
	float** mUVData;			// Doublets of (u, v).
	unsigned char* mColorData;
	vtx_idx_t* mIndexData;	// Triplets of vertex indices.

	ColorFormat mColorFormat;

	Tbc::GeometryBase::GeometryVolatility mGeometryVolatility;
	Tbc::GeometryBase::PrimitiveType mPrimitiveType;
};



} // End namespace.



#endif
