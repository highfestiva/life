/*
	Class:  Material
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

	NOTES:

	Materials in tbc don't work in the same manner as in other
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

	In tbc, you don't add the material to the geometry. You add the
	geometry to the material. All supported materials are then stored
	in an array in the renderer. Each material is then responsible
	for rendering all geometry using that material. This approach isn't
	only smart because of the lack of a sorting routine, it also gives
	you full control over the rendering process. One example is that
	you can guarantee that all alpha blended/tested meshes are rendered
	last.
*/

#pragma once

#include "uitbc.h"
#include "uirenderer.h"
#include "../../lepra/include/hashtable.h"
#include <list>

namespace tbc {
class GeometryBase;
}

namespace uitbc {

class Material;

class GeometryGroup {
public:
	GeometryGroup(Material* material, int alloc_size = 8);
	~GeometryGroup();

	void AddGeometry(tbc::GeometryBase* geometry);
	bool RemoveGeometry(tbc::GeometryBase* geometry);

	// Sorting will also calculate the depth's of the geometries
	// and the group.
	void F2BSortGroup(); // Front-to-back sort.
	void B2FSortGroup(); // Back-to-front sort.

	float GetMeanDepth() const { return mean_depth_; }
	Renderer::TextureID GetGroupTextureID() const { return group_texture_id_; }
	int GetGeometryCount() const { return geometry_count_; }
	tbc::GeometryBase* GetGeometry(int i) { return geom_array_[i].geometry_; }
private:
	int CalculateDepths(bool f2_b);
	void BubbleSort(int (*Cmp)(const void* geom1, const void* geom2));
	static int F2BCompare(const void* geom1, const void* geom2);
	static int B2FCompare(const void* geom1, const void* geom2);

	struct Pair {
		tbc::GeometryBase* geometry_;
		float depth_;
	};

	Material* parent_material_;

	// The geometry list contains depth-sorted geometries.
	// They are usually ordered front-to-back in solid materials
	// and back-to-front in blended materials.
	Pair* geom_array_;
	int geometry_count_;
	int array_length_;

	// The group's mean depth-coordinate.
	float mean_depth_;

	// The texture (color map) that is shared by the group.
	Renderer::TextureID group_texture_id_;
};

class Material {
public:
	enum RemoveStatus {
		kNotRemoved = 0,
		kRemoved,
		kRemovedFromFallback,
	};

	enum DepthSortHint {
		kNoDepthsort = 0,
		kDepthsortF2B, // Front-to-back
		kDepthsortB2F,     // Back-to-front
	};

	Material(Renderer* renderer, DepthSortHint sort_hint, Material* fall_back_material);
	virtual ~Material();

	static void EnableWireframe(bool enabled);
	static void SetEnableDepthSorting(bool enabled);
	static void EnableDrawMaterial(bool enabled);

	Renderer* GetRenderer();

	virtual bool AddGeometry(tbc::GeometryBase* geometry);
	virtual RemoveStatus RemoveGeometry(tbc::GeometryBase* geometry);
	virtual void RemoveAllGeometry();

	virtual void SetBasicMaterial(const tbc::GeometryBase::BasicMaterialSettings& material) = 0;

	virtual void PreRender();
	virtual void PostRender();

	static void RenderAllGeometry(unsigned current_frame, Material* geometry_container, Material* renderer = 0);
	virtual void RenderGeometry(tbc::GeometryBase* geometry) = 0;
	virtual void RawRender(tbc::GeometryBase* geometry, int uv_set_index) = 0;
	virtual void RenderBaseGeometry(tbc::GeometryBase* geometry) = 0;

	tbc::GeometryBase* GetFirstGeometry();
	tbc::GeometryBase* GetNextGeometry();

	typedef std::list<tbc::GeometryBase*> GeometryList;
	typedef std::list<GeometryGroup*> GeometryGroupList;

	virtual void RenderAllGeometry(unsigned current_frame, const GeometryGroupList& geometry_group_list);
	virtual void RenderAllBlendedGeometry(unsigned current_frame, const GeometryGroupList& geometry_group_list);
	virtual void DoRenderAllGeometry(unsigned current_frame, const GeometryGroupList& geometry_group_list);

	Renderer::TextureID GetGroupTextureID(tbc::GeometryBase* geometry) const;
	const GeometryGroupList& GetGeometryGroupList() const;

protected:
	static tbc::GeometryBase::BasicMaterialSettings current_material_;

	GeometryGroupList geometry_group_list_;

	Material* fall_back_material_;	// If geometry doesn't contain all data needed.

	Renderer* renderer_;
	DepthSortHint sort_hint_;

	// Used for iteration.
	GeometryGroupList::const_iterator group_iter_;
	int index_;

	static bool enable_wireframe_;
	static bool enable_depth_sort_;
	static bool enable_draw_material_;
};

class NullMaterial : public Material {
public:
	NullMaterial(Renderer* listener) :
	      Material(listener, Material::kNoDepthsort, 0) {
	}

	virtual ~NullMaterial() {
	}

	void SetBasicMaterial(const tbc::GeometryBase::BasicMaterialSettings&) {
	}

	void RenderGeometry(tbc::GeometryBase* /*geometry*/) {
	}
	void RawRender(tbc::GeometryBase*, int) {
	}
	void RenderBaseGeometry(tbc::GeometryBase* /*geometry*/) {
	}
};

}
