/*
	Lepra::File:   SoftwareMaterials.h
	Class:  -
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games
*/

#ifndef UISOFTWAREMATERIALS_H
#define UISOFTWAREMATERIALS_H

#include "UiTBC.h"
#include "UiMaterial.h"
#include "UiSoftware3DPainter.h"

namespace UiTbc
{

class SoftwareRenderer;
class Software3DPainter;


class SoftwareMaterial : public Material
{
public:

	inline SoftwareMaterial(Renderer* pRenderer,
				Material::DepthSortHint pSortHint,
				Software3DPainter* p3DPainter, 
				Material* pFallBackMaterial) :
		Material(pRenderer, pSortHint),
		m3DPainter(p3DPainter),
		mFallBackMaterial(pFallBackMaterial)
	{
	}

	virtual Material::RemoveStatus RemoveGeometry(TBC::GeometryBase* pGeometry);

	virtual void RenderAllGeometry(unsigned int pCurrentFrame);

	inline Software3DPainter::RenderingPrimitive GetElementType(TBC::GeometryBase* pGeometry);

protected:
	Software3DPainter* m3DPainter;
	Material* mFallBackMaterial;	// If geometry doesn't contain all data needed.

	static Software3DPainter::RenderingPrimitive smPrimitiveLUT[2];
};

Software3DPainter::RenderingPrimitive SoftwareMaterial::GetElementType(TBC::GeometryBase* pGeometry)
{
	return smPrimitiveLUT[pGeometry->GetPrimitiveType()];
}


class SoftMatSingleColorSolid : public SoftwareMaterial
{
public:
	inline SoftMatSingleColorSolid(Renderer* pRenderer,
					Software3DPainter* p3DPainter, 
					Material* pFallBackMaterial) :
		SoftwareMaterial(pRenderer, Material::DEPTHSORT_F2B, p3DPainter, pFallBackMaterial)
	{
	}
protected:
	virtual void RenderGeometry(TBC::GeometryBase* pGeometry);
};

class SoftMatVertexColorSolid : public SoftwareMaterial
{
public:
	inline SoftMatVertexColorSolid(Renderer* pRenderer,
					Software3DPainter* p3DPainter, 
					Material* pFallBackMaterial) :
		SoftwareMaterial(pRenderer, Material::DEPTHSORT_F2B, p3DPainter, pFallBackMaterial)
	{
	}
protected:
	virtual void RenderGeometry(TBC::GeometryBase* pGeometry);
};

class SoftMatSingleTextureSolid : public SoftwareMaterial
{
public:
	inline SoftMatSingleTextureSolid(Renderer* pRenderer,
					Software3DPainter* p3DPainter, 
					Material* pFallBackMaterial) :
		SoftwareMaterial(pRenderer, Material::DEPTHSORT_F2B, p3DPainter, pFallBackMaterial)
	{
	}

	virtual bool AddGeometry(TBC::GeometryBase* pGeometry);
protected:
	virtual void RenderGeometry(TBC::GeometryBase* pGeometry);
};

/*

class SoftMatSingleColorBlended : public SoftwareMaterial
{
public:
	inline SoftMatSingleColorBlended(Software3DPainter* p3DPainter, TBC::Material* pFallBackMaterial) :
		SoftwareMaterial(p3DPainter, pFallBackMaterial)
		{
		}
protected:
	virtual void RenderGeometry(TBC::GeometryBase* pGeometry);
};

class SoftMatVertexColorBlended : public SoftwareMaterial
{
public:
	inline SoftMatVertexColorBlended(Software3DPainter* p3DPainter, TBC::Material* pFallBackMaterial) :
		SoftwareMaterial(p3DPainter, pFallBackMaterial)
		{
		}
protected:
	virtual void RenderGeometry(TBC::GeometryBase* pGeometry);
};

class SoftMatSingleTextureBlended : public SoftwareMaterial
{
public:
	inline SoftMatSingleTextureBlended(Software3DPainter* p3DPainter, TBC::Material* pFallBackMaterial) :
		SoftwareMaterial(p3DPainter, pFallBackMaterial)
		{
		}
protected:
	virtual void RenderGeometry(TBC::GeometryBase* pGeometry);
};

class SoftMatSingleTextureAlphaTested : public SoftwareMaterial
{
public:
	inline SoftMatSingleTextureAlphaTested(Software3DPainter* p3DPainter, TBC::Material* pFallBackMaterial) :
		SoftwareMaterial(p3DPainter, pFallBackMaterial)
		{
		}
protected:
	virtual void RenderGeometry(TBC::GeometryBase* pGeometry);
};

*/

} // End namespace.

#endif
