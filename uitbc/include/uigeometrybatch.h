/*
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

	NOTES:

	The vegetation patch is a group of objects sharing the same
	geometry. Thus, a tree can be duplicated into a hundred copies
	of the same tree-mesh, and stored as only one single geometry
	instance.

	This increases rendering performance in cases where thousand of
	instances of the same geometry should be rendered, and especially
	if this geometry is a low-poly mesh.
*/



#pragma once



#include "../../tbc/include/geometrybase.h"
#include "../include/uitbc.h"



namespace uitbc {



class GeometryBatch: public tbc::GeometryBase {
public:

	GeometryBatch(tbc::GeometryBase* geometry);
	virtual ~GeometryBatch();

	// Creates new instances.
	void SetInstances(const xform* displacement, const vec3& root_offset,
		int num_instances, uint32 random_seed,
		float x_scale_min, float x_scale_max,
		float y_scale_min, float y_scale_max,
		float z_scale_min, float z_scale_max);
	void ClearAllInstances();

	virtual void SetGeometryVolatility(GeometryVolatility volatility);
	virtual GeometryVolatility GetGeometryVolatility() const;

	virtual ColorFormat GetColorFormat() const;

	virtual unsigned int GetMaxVertexCount()  const;
	virtual unsigned int GetMaxIndexCount() const;

	virtual unsigned int GetVertexCount()  const;
	virtual unsigned int GetIndexCount() const;
	virtual unsigned int GetUVSetCount()    const;

	virtual float*         GetVertexData() const;
	virtual float*         GetUVData(unsigned int uv_set) const;
	virtual vtx_idx_t*     GetIndexData() const;
	virtual void           SetIndexData(vtx_idx_t* index_data, unsigned index_count, unsigned max_index_count);
	virtual uint8*         GetColorData() const;

protected:
private:
	float* vertex_;
	float** uv_;
	vtx_idx_t* index_;
	uint8* color_;
	unsigned int vertex_count_;
	unsigned int index_count_;
	unsigned int max_index_count_;
	unsigned int uv_set_count_;

	tbc::GeometryBase* geometry_instance_;
};



}
