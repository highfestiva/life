/*
	File:   ShadowVolume.h
	Class:  ShadowVolume
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games
*/

#ifndef UISHADOWVOLUME_H
#define UISHADOWVOLUME_H

#include "UiTBC.h"
#include "../../TBC/Include/GeometryBase.h"
#include "../../Lepra/Include/Transformation.h"

namespace UiTbc
{

class ShadowVolume : public TBC::GeometryBase
{
public:
	
	ShadowVolume();
	ShadowVolume(TBC::GeometryBase* pParentGeometry);
	virtual ~ShadowVolume();

	// Light position in world coordinates. 
	void UpdateShadowVolume(const Vector3DF& pLightPos, float pLightRadius, bool pDirectional);

	TBC::GeometryBase* GetParentGeometry();
	void SetParentGeometry(TBC::GeometryBase* pParentGeometry);

	TBC::GeometryBase::GeometryVolatility GetGeometryVolatility() const;
	void SetGeometryVolatility(TBC::GeometryBase::GeometryVolatility pVolatility);

	TBC::GeometryBase::PrimitiveType GetPrimitiveType() const;

	unsigned int GetMaxVertexCount()  const;
	unsigned int GetMaxIndexCount() const;

	unsigned int GetVertexCount()  const;
	unsigned int GetIndexCount() const;
	unsigned int GetUVSetCount()    const;

	float*         GetVertexData() const;
	float*         GetUVData(unsigned int pUVSet) const;
	uint32* GetIndexData() const;
	uint8*  GetColorData() const;
	float*         GetNormalData() const;

	ColorFormat GetColorFormat() const;

	void SetLastFrameVisible(unsigned int pLastFrameVisible);
	unsigned int GetLastFrameVisible() const;

	void GetReplacementColor(float& pRed, float& pGreen, float& pBlue) const;
	void SetReplacementColor(float pRed, float pGreen, float pBlue);

protected:
	void ExtrudeEdges();
private:

	enum TO
	{
		TO_INVALID = 0,
		TO_FRONT,
		TO_BACK,
	};

	struct TriangleOrientation
	{
		TO mTO;
		unsigned long mV0;
		unsigned long mV1;
		unsigned long mV2;
		bool mChecked;
	};

	void InitVertices();
	void InitTO();

	float* mVertexData;
	uint32* mIndexData;

	// Array with mNumParentTriangles elements.
	TriangleOrientation* mTriangleOrientation;

	unsigned int mVertexCount;
	unsigned int mTriangleCount;
	unsigned int mParentVertexCount;
	unsigned int mMaxTriangleCount;

	// Replacement color, in case textures are missing or whatever.
	float mRed;
	float mGreen;
	float mBlue;

	TransformationF mTransformation;
	unsigned int mLastFrameVisible;

	bool mTransformationChanged;

	// The geometry that cast this shadow.
	TBC::GeometryBase* mParentGeometry;
};

} // End namespace.

#endif
