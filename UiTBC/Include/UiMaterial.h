/*
	Class:  Material
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games

	NOTES:

	Materials in TBC don't work in the same manner as in other 
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

	In TBC, you don't add the material to the geometry. You add the
	geometry to the material. All supported materials are then stored
	in an array in the renderer. Each material is then responsible
	for rendering all geometry using that material. This approach isn't
	only smart because of the lack of a sorting routine, it also gives
	you full control over the rendering process. One example is that
	you can guarantee that all alpha blended/tested meshes are rendered 
	last.
*/

#ifndef MATERIAL_H
#define MATERIAL_H

#include "UiTBC.h"
#include "UiRenderer.h"
#include "../../Lepra/Include/HashTable.h"
#include <list>

namespace TBC
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

	void AddGeometry(TBC::GeometryBase* pGeometry);
	bool RemoveGeometry(TBC::GeometryBase* pGeometry);

	// Sorting will also calculate the depth's of the geometries
	// and the group.
	void F2BSortGroup(); // Front-to-back sort.
	void B2FSortGroup(); // Back-to-front sort.

	float GetMeanDepth() const { return mMeanDepth; }
	Renderer::TextureID GetGroupTextureID() const { return mGroupTextureID; }
	int GetGeometryCount() const { return mGeometryCount; }
	TBC::GeometryBase* GetGeometry(int i) { return mGeomArray[i].mGeometry; }
private:
	int CalculateDepths(bool pF2B);
	void BubbleSort(int (*Cmp)(const void* pGeom1, const void* pGeom2));
	static int F2BCompare(const void* pGeom1, const void* pGeom2);
	static int B2FCompare(const void* pGeom1, const void* pGeom2);

	struct Pair
	{
		TBC::GeometryBase* mGeometry;
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

	Material(Renderer* pRenderer, DepthSortHint pSortHint);
	virtual ~Material();

	static void SetEnableDepthSorting(bool pEnabled);
	static void EnableDrawMaterial(bool pEnabled);
	static bool IsDrawMaterialEnabled();

	Renderer* GetRenderer();

	virtual bool AddGeometry(TBC::GeometryBase* pGeometry);
	virtual RemoveStatus RemoveGeometry(TBC::GeometryBase* pGeometry);
	virtual void RemoveAllGeometry();

	virtual void SetBasicMaterial(const TBC::GeometryBase::BasicMaterialSettings& pMaterial) = 0;

	bool IsEmpty() { return mGeometryGroupList.empty(); }

	virtual void RenderAllGeometry(unsigned int pCurrentFrame);
	virtual void DoRenderAllGeometry(unsigned int pCurrentFrame);
	virtual void RenderAllBlendedGeometry(unsigned pCurrentFrame);
	virtual void RenderGeometry(TBC::GeometryBase* pGeometry) = 0;
	virtual void RenderBaseGeometry(TBC::GeometryBase* pGeometry) = 0;

	TBC::GeometryBase* GetFirstGeometry();
	TBC::GeometryBase* GetNextGeometry();

	TBC::GeometryBase* GetFirstVisibleGeometry();
	TBC::GeometryBase* GetNextVisibleGeometry();

protected:
	typedef std::list<TBC::GeometryBase*> GeometryList;
	typedef std::list<GeometryGroup*> GeometryGroupList;

	Renderer::TextureID GetGroupTextureID(TBC::GeometryBase* pGeometry);
	GeometryGroupList* GetGeometryGroupList();

	static TBC::GeometryBase::BasicMaterialSettings mCurrentMaterial;

private:
	GeometryGroupList mGeometryGroupList;
	Renderer* mRenderer;
	DepthSortHint mSortHint;

	// Used for iteration.
	GeometryGroupList::const_iterator mGroupIter;
	int mIndex;

	static bool mEnableDepthSort;
	static bool mEnableDrawMaterial;
};

class NullMaterial : public Material
{
public:
	NullMaterial(Renderer* pListener) :
	      Material(pListener, Material::NO_DEPTHSORT)
	{
	}

	virtual ~NullMaterial()
	{
	}

	void SetBasicMaterial(const TBC::GeometryBase::BasicMaterialSettings&)
	{
	}

	void RenderGeometry(TBC::GeometryBase* /*pGeometry*/)
	{
	}
	void RenderBaseGeometry(TBC::GeometryBase* /*pGeometry*/)
	{
	}
};

} // End namespace.

#endif
