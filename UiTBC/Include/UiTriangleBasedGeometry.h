
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#ifndef UITRIANGLEBASEDGEOMETRY_H
#define UITRIANGLEBASEDGEOMETRY_H



#include "../../Lepra/Include/Graphics2D.h"
#include "../../Lepra/Include/Vector3D.h"
#include "../../Lepra/Include/Vector2D.h"
#include "../../TBC/Include/GeometryBase.h"
#include "UiTBC.h"



namespace UiTbc
{



class TriangleBasedGeometry: public TBC::GeometryBase
{
public:
	TriangleBasedGeometry();
	TriangleBasedGeometry(const TriangleBasedGeometry& pGeometry);

	TriangleBasedGeometry(Vector3DF* pVertices,      // Must containt pVertexCount elements.
			      Vector3DF* pVertexNormals, // NULL or must contain pVertexCount elements.
			      Vector2D<float>* pUV,      // NULL or must contain pVertexCount elements.
			      Color* pColor,             // NULL or must contain pVertexCount elements.
			      ColorFormat pColorFormat,
			      uint32* pIndices,
			      unsigned int pVertexCount,
			      unsigned int pIndexCount,
			      TBC::GeometryBase::PrimitiveType pPrimitiveType,
			      TBC::GeometryBase::GeometryVolatility pGeomType);

	TriangleBasedGeometry(float* pVertexData,
			      float* pVertexNormals,
			      float* pUVData,
			      uint8* pColorData,
			      ColorFormat pColorFormat,
			      uint32* pIndices,
			      unsigned int pVertexCount,
			      unsigned int pIndexCount,
			      TBC::GeometryBase::PrimitiveType pPrimitiveType,
			      TBC::GeometryBase::GeometryVolatility pGeomType);
	virtual ~TriangleBasedGeometry();

	void Copy(const TriangleBasedGeometry& pGeometry);

	void Set(Vector3DF* pVertices,
		 Vector3DF* pVertexNormals,
		 Vector2D<float>* pUV,
		 Color* pColor,
		 ColorFormat pColorFormat,
		 uint32* pIndices,
		 unsigned int pVertexCount,
		 unsigned int pIndexCount,
		 TBC::GeometryBase::PrimitiveType pPrimitiveType,
		 TBC::GeometryBase::GeometryVolatility pGeomType);

	void Set(float* pVertexData,
		 float* pVertexNormals,
		 float* pUVData,
		 uint8* pColorData,
		 ColorFormat pColorFormat,
		 uint32* pIndices,
		 unsigned int pVertexCount,
		 unsigned int pIndexCount,
		 TBC::GeometryBase::PrimitiveType pPrimitiveType,
		 TBC::GeometryBase::GeometryVolatility pGeomType);

	// AddUVSet() returns the index of the uv set.
	// pUVData must contain mVertexCount * 2 elements.
	int AddUVSet(float* pUVData);
	int AddUVSet(Vector2D<float>* pUVData);
	int AddEmptyUVSet();
	int DupUVSet(int pUVSet);

	// Data access...

	virtual unsigned int GetMaxVertexCount() const;
	virtual unsigned int GetMaxIndexCount() const;

	virtual unsigned int GetVertexCount() const;
	virtual unsigned int GetIndexCount() const;
	virtual unsigned int GetUVSetCount() const;

	virtual float* GetVertexData() const;
	virtual float* GetUVData(unsigned int pUVSet = 0) const;
	virtual uint32* GetIndexData() const;
	virtual uint8* GetColorData() const;

	void SetColorData(unsigned char* pColorData, ColorFormat pColorFormat);
	void ConvertColorData(ColorFormat pColorFormat, unsigned char pAlpha);
	ColorFormat GetColorFormat() const;

	// Misc work funcs.
	void ClearAll();

	// Will erase all current data.
	void SetPolygon(Vector3DF* pVertices, unsigned int pVertexCount);

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

	void Translate(const Vector3DF& pPositionOffset);
	void Rotate(const RotationMatrix<float>& pRotation);

	// This definitly won't work if "this" has a primitive type other than TRIANGLES.
	void AddGeometry(TriangleBasedGeometry* pGeometry);
	
	// Use this if IsSingleObject() returns false and you want to get the separate objects.
	// The pointer that is returned will point to an array of TriangleBasedGeometry.
	// The number of elements in this array is stored in pNumObjects.
	// The caller is responsible of deleting this array.
	TriangleBasedGeometry* GetSeparateObjects(int& pNumObjects);

	PrimitiveType GetPrimitiveType() const;

	// Overloads from TBC::GeometryBase.
	GeometryVolatility GetGeometryVolatility() const;
	void SetGeometryVolatility(GeometryVolatility pVolatility);

private:

	unsigned int mVertexCount;
	unsigned int mIndexCount;
	unsigned int mUVSetCount;

	float* mVertexData;			// Triplets of (x, y, z).
	float** mUVData;			// Doublets of (u, v).
	unsigned char* mColorData;
	uint32* mIndexData;	// Triplets of vertex indices.

	ColorFormat mColorFormat;

	TBC::GeometryBase::GeometryVolatility mGeometryVolatility;
	TBC::GeometryBase::PrimitiveType mPrimitiveType;
};



} // End namespace.



#endif
