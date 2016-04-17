/*
	Class:  Material
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

	NOTES:

	Materials in Tbc don't work in the same manner as in other 
	3D-engines I've seen.

	In the 3D-engines I've seen, you create an instance of a material
	and add it to the geometry. This seems like the most logical
	way to do it. Perhaps it is... But it's not the most effective
	way.

	It is well known that communication with the hardware is very
	expensive when talking about rendering performance. This means
	that we want to keep all communication with the graphics hardware
	at a minimum.

	Some engines don't take this into consideration at all, and renders
	the geometry in random order. Other engines on the other hand take 
	care of this problem by sorting the geometry in	different ways.

	I decided to render all geometry sorted by material, but without 
	spending any time sorting at all. This is done by taking a somewhat
	opposite approach.

	In Tbc, you don't add the material to the geometry. You add the
	geometry to the material. All supported materials are then stored
	in an array in the renderer. Each material is then responsible
	for rendering all geometry using that material. This approach isn't
	only smart because of the lack of a sorting routine, it also gives
	you full control over the rendering process. One example is that
	you can guarantee that all alpha blended/tested meshes are rendered 
	last.
*/

#pragma once

#include "UiTbc.h"
#include "UiRenderer.h"
#include "../../Lepra/Include/HashTable.h"
#include <list>

namespace Tbc
{
class GeometryBase;
}

namespace UiTbc
{

class Material;

class GeometryGroup
{
public:
	GeometryGroup(Material* pMaterial, int pAllocSize = 8);
	~GeometryGroup();

	void AddGeometry(Tbc::GeometryBase* pGeometry);
	bool RemoveGeometry(Tbc::GeometryBase* pGeometry);

	// Sorting will also calculate the depth's of the geometries
	// and the group.
	void F2BSortGroup(); // Front-to-back sort.
	void B2FSortGroup(); // Back-to-front sort.

	float GetMeanDepth() const { return mMeanDepth; }
	Renderer::TextureID GetGroupTextureID() const { return mGroupTextureID; }
	int GetGeometryCount() const { return mGeometryCount; }
	Tbc::GeometryBase* GetGeometry(int i) { return mGeomArray[i].mGeometry; }
private:
	int CalculateDepths(bool pF2B);
	void BubbleSort(int (*Cmp)(const void* pGeom1, const void* pGeom2));
	static int F2BCompare(const void* pGeom1, const void* pGeom2);
	static int B2FCompare(const void* pGeom1, const void* pGeom2);

	struct Pair
	{
		Tbc::GeometryBase* mGeometry;
		float mDepth;
	};

	Material* mParentMaterial;

	// The geometry list contains depth-sorted geometries.
	// They are usually ordered front-to-back in solid materials
	// and back-to-front in blended materials.
	Pair* mGeomArray;
	int mGeometryCount;
	int mArrayLength;

	// The group's mean depth-coordinate.
	float mMeanDepth;

	// The texture (color map) that is shared by the group.
	Renderer::TextureID mGroupTextureID;
};

class Material
{
public:
	enum RemoveStatus
	{
		NOT_REMOVED = 0,
		REMOVED,
		REMOVED_FROM_FALLBACK,
	};

	enum DepthSortHint
	{
		NO_DEPTHSORT = 0,
		DEPTHSORT_F2B, // Front-to-back
		DEPTHSORT_B2F,     // Back-to-front
	};

	Material(Renderer* pRenderer, DepthSortHint pSortHint, Material* pFallBackMaterial);
	virtual ~Material();

	static void EnableWireframe(bool pEnabled);
	static void SetEnableDepthSorting(bool pEnabled);
	static void EnableDrawMaterial(bool pEnabled);

	Renderer* GetRenderer();

	virtual bool AddGeometry(Tbc::GeometryBase* pGeometry);
	virtual RemoveStatus RemoveGeometry(Tbc::GeometryBase* pGeometry);
	virtual void RemoveAllGeometry();

	virtual void SetBasicMaterial(const Tbc::GeometryBase::BasicMaterialSettings& pMaterial) = 0;

	virtual void PreRender();
	virtual void PostRender();

	static void RenderAllGeometry(unsigned pCurrentFrame, Material* pGeometryContainer, Material* pRenderer = 0);
	virtual void RenderGeometry(Tbc::GeometryBase* pGeometry) = 0;
	virtual void RawRender(Tbc::GeometryBase* pGeometry, int pUVSetIndex) = 0;
	virtual void RenderBaseGeometry(Tbc::GeometryBase* pGeometry) = 0;

	Tbc::GeometryBase* GetFirstGeometry();
	Tbc::GeometryBase* GetNextGeometry();

	typedef std::list<Tbc::GeometryBase*> GeometryList;
	typedef std::list<GeometryGroup*> GeometryGroupList;

	virtual void RenderAllGeometry(unsigned pCurrentFrame, const GeometryGroupList& pGeometryGroupList);
	virtual void RenderAllBlendedGeometry(unsigned pCurrentFrame, const GeometryGroupList& pGeometryGroupList);
	virtual void DoRenderAllGeometry(unsigned pCurrentFrame, const GeometryGroupList& pGeometryGroupList);

	Renderer::TextureID GetGroupTextureID(Tbc::GeometryBase* pGeometry) const;
	const GeometryGroupList& GetGeometryGroupList() const;

protected:
	static Tbc::GeometryBase::BasicMaterialSettings mCurrentMaterial;

	GeometryGroupList mGeometryGroupList;

	Material* mFallBackMaterial;	// If geometry doesn't contain all data needed.

	Renderer* mRenderer;
	DepthSortHint mSortHint;

	// Used for iteration.
	GeometryGroupList::const_iterator mGroupIter;
	int mIndex;

	static bool mEnableWireframe;
	static bool mEnableDepthSort;
	static bool mEnableDrawMaterial;
};

class NullMaterial : public Material
{
public:
	NullMaterial(Renderer* pListener) :
	      Material(pListener, Material::NO_DEPTHSORT, 0)
	{
	}

	virtual ~NullMaterial()
	{
	}

	void SetBasicMaterial(const Tbc::GeometryBase::BasicMaterialSettings&)
	{
	}

	void RenderGeometry(Tbc::GeometryBase* /*pGeometry*/)
	{
	}
	void RawRender(Tbc::GeometryBase*, int)
	{
	}
	void RenderBaseGeometry(Tbc::GeometryBase* /*pGeometry*/)
	{
	}
};

}
