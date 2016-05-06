
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/uiprogressivetrianglegeometry.h"
#include "../../lepra/include/lepraassert.h"
#include "../../lepra/include/listutil.h"



namespace uitbc {



ProgressiveTriangleGeometry::ProgressiveTriangleGeometry():
	base_vertex_count_(0),
	base_triangle_count_(0),

	current_vertex_count_(0),
	current_triangle_count_(0),

	max_vertex_count_(0),
	max_triangle_count_(0),

	current_vertex_data_(0),
	current_uv_data_(0),
	current_color_data_(0),
	current_normal_data_(0),
	current_color_data8_(0),
	current_indices_(0),

	base_vertex_data_(0),
	base_uv_data_(0),
	base_color_data_(0),
	base_normal_data_(0),
	base_indices_(0),

	current_v_split_(0),
	num_vertex_splits_(0),
	vertex_split_(0),

	color_format_(tbc::GeometryBase::kColorRgba) {
}

ProgressiveTriangleGeometry::ProgressiveTriangleGeometry(ProgressiveTriangleGeometry& progressive_geometry):
	base_vertex_count_(0),
	base_triangle_count_(0),

	current_vertex_count_(0),
	current_triangle_count_(0),

	max_vertex_count_(0),
	max_triangle_count_(0),

	current_vertex_data_(0),
	current_uv_data_(0),
	current_color_data_(0),
	current_normal_data_(0),
	current_color_data8_(0),
	current_indices_(0),

	base_vertex_data_(0),
	base_uv_data_(0),
	base_color_data_(0),
	base_normal_data_(0),
	base_indices_(0),

	current_v_split_(0),
	num_vertex_splits_(0),
	vertex_split_(0),

	color_format_(tbc::GeometryBase::kColorRgba) {
	Copy(progressive_geometry);
}

ProgressiveTriangleGeometry::ProgressiveTriangleGeometry(TriangleBasedGeometry& geometry):
	base_vertex_count_(0),
	base_triangle_count_(0),

	current_vertex_count_(0),
	current_triangle_count_(0),

	max_vertex_count_(0),
	max_triangle_count_(0),

	current_vertex_data_(0),
	current_uv_data_(0),
	current_color_data_(0),
	current_normal_data_(0),
	current_color_data8_(0),
	current_indices_(0),

	base_vertex_data_(0),
	base_uv_data_(0),
	base_color_data_(0),
	base_normal_data_(0),
	base_indices_(0),

	current_v_split_(0),
	num_vertex_splits_(0),
	vertex_split_(0),

	color_format_(tbc::GeometryBase::kColorRgba) {
	Set(geometry);
}

ProgressiveTriangleGeometry::~ProgressiveTriangleGeometry() {
	ClearAll();
}

void ProgressiveTriangleGeometry::ClearAll() {
	delete[] current_vertex_data_;
	delete[] current_uv_data_;
	delete[] current_color_data_;
	delete[] current_color_data8_;
	delete[] current_indices_;
	delete[] base_vertex_data_;
	delete[] base_uv_data_;
	delete[] base_color_data_;
	delete[] base_indices_;
	delete[] vertex_split_;

	current_vertex_data_ = 0;
	current_uv_data_ = 0;
	current_color_data_ = 0;
	current_color_data8_ = 0;
	current_indices_ = 0;
	base_vertex_data_ = 0;
	base_uv_data_ = 0;
	base_color_data_ = 0;
	base_indices_ = 0;
	vertex_split_ = 0;

	max_vertex_count_       = 0;
	max_triangle_count_     = 0;

	base_vertex_count_      = 0;
	base_triangle_count_    = 0;

	current_vertex_count_   = 0;
	current_triangle_count_ = 0;

	num_vertex_splits_      = 0;

	tbc::GeometryBase::SetBoundingRadius(0.0f);
}

void ProgressiveTriangleGeometry::Copy(ProgressiveTriangleGeometry& progressive_geometry) {
	max_vertex_count_	      = progressive_geometry.max_vertex_count_;
	max_triangle_count_	  = progressive_geometry.max_triangle_count_;

	base_vertex_count_	  = progressive_geometry.base_vertex_count_;
	base_triangle_count_	  = progressive_geometry.base_triangle_count_;

	current_vertex_count_	  = progressive_geometry.current_vertex_count_;
	current_triangle_count_ = progressive_geometry.current_triangle_count_;

	current_vertex_data_	  = 0;
	current_uv_data_	      = 0;
	current_color_data_	  = 0;
	current_color_data8_	  = 0;
	current_indices_	      = 0;

	base_vertex_data_	      = 0;
	base_uv_data_		      = 0;
	base_color_data_	      = 0;
	base_indices_	      = 0;

	vertex_split_		  = 0;
	current_v_split_	      = progressive_geometry.current_v_split_;
	num_vertex_splits_	  = progressive_geometry.num_vertex_splits_;

	color_format_		  = progressive_geometry.color_format_;

	SetTransformation(progressive_geometry.GetTransformation());

	const tbc::GeometryBase::BasicMaterialSettings& mat_settings = progressive_geometry.GetBasicMaterialSettings();
	tbc::GeometryBase::SetBasicMaterialSettings(mat_settings);

	SetLastFrameVisible(progressive_geometry.GetLastFrameVisible());

	tbc::GeometryBase::SetRendererData(progressive_geometry.GetRendererData());

	int i;
	// Copy the current state data.

	if (progressive_geometry.current_vertex_data_ != 0) {
		int num_elements = (base_vertex_count_ + num_vertex_splits_) * 3;
		current_vertex_data_ = new float[num_elements];
		for (i = 0; i < num_elements; i++) {
			current_vertex_data_[i] = progressive_geometry.current_vertex_data_[i];
		}
	}

	if (progressive_geometry.current_uv_data_ != 0) {
		int num_elements = (base_vertex_count_ + num_vertex_splits_) * 2;
		current_uv_data_ = new float[num_elements];
		for (i = 0; i < num_elements; i++) {
			current_uv_data_[i] = progressive_geometry.current_uv_data_[i];
		}
	}

	if (progressive_geometry.current_color_data_ != 0) {
		int num_elements = (base_vertex_count_ + num_vertex_splits_) * 4;
		current_color_data_ = new float[num_elements];
		for (i = 0; i < num_elements; i++) {
			current_color_data_[i] = progressive_geometry.current_color_data_[i];
		}
	}

	if (progressive_geometry.current_color_data8_ != 0) {
		int num_elements = (base_vertex_count_ + num_vertex_splits_) * 4;
		current_color_data8_ = new uint8[num_elements];
		for (i = 0; i < num_elements; i++) {
			current_color_data8_[i] = progressive_geometry.current_color_data8_[i];
		}
	}

	if (progressive_geometry.current_indices_ != 0) {
		int num_elements = (base_triangle_count_ + num_vertex_splits_ * 2) * 3;
		current_indices_ = new uint32[num_elements];
		for (i = 0; i < num_elements; i++) {
			current_indices_[i] = progressive_geometry.current_indices_[i];
		}
	}

	// Copy the base mesh data.
	if (progressive_geometry.base_vertex_data_ != 0) {
		int num_elements = base_vertex_count_ * 3;
		base_vertex_data_ = new float[num_elements];
		for (i = 0; i < num_elements; i++) {
			base_vertex_data_[i] = progressive_geometry.base_vertex_data_[i];
		}
	}

	if (progressive_geometry.base_uv_data_ != 0) {
		int num_elements = base_vertex_count_ * 2;
		base_uv_data_ = new float[num_elements];
		for (i = 0; i < num_elements; i++) {
			base_uv_data_[i] = progressive_geometry.base_uv_data_[i];
		}
	}

	if (progressive_geometry.base_color_data_ != 0) {
		int num_elements = base_vertex_count_ * 4;
		base_color_data_ = new float[num_elements];
		for (i = 0; i < num_elements; i++) {
			base_color_data_[i] = progressive_geometry.base_color_data_[i];
		}
	}

	if (progressive_geometry.base_indices_ != 0) {
		int num_elements = base_triangle_count_ * 3;
		base_indices_ = new uint32[num_elements];
		for (i = 0; i < num_elements; i++) {
			base_indices_[i] = progressive_geometry.base_indices_[i];
		}
	}

	if (progressive_geometry.vertex_split_ != 0) {
		int num_elements = num_vertex_splits_;
		vertex_split_ = new VertexSplit[num_elements];
		for (i = 0; i < num_elements; i++) {
			vertex_split_[i].Copy(progressive_geometry.vertex_split_[i]);
		}
	}

	tbc::GeometryBase::SetBoundingRadius(progressive_geometry.GetBoundingRadius());
}

void ProgressiveTriangleGeometry::FindEdgeToCollapse(VertexList& /*org_vertex_list*/,
	TriangleList& /*org_triangle_list*/, VertexList& vertex_list, TriangleList& triangle_list, Edge& edge) {
	if (triangle_list.empty())
		return;

	float current_error = 0.0f;
	float current_max_error = 0.0f;

	edge.v1_ = 0;
	edge.v2_ = 0;
	edge.triangle_list_.clear();

	// Calculate mean error.
	VertexList::iterator v_iter;
	for (v_iter = vertex_list.begin();
	     v_iter != vertex_list.end();
	     ++v_iter) {
		if (v_iter == vertex_list.begin()) {
			current_max_error = (*v_iter)->error_;
		} else if((*v_iter)->error_ > current_max_error) {
			current_max_error = (*v_iter)->error_;
		}

		current_error += (*v_iter)->error_;
	}

	current_error /= (float)vertex_list.size();

	float min_error = 0.0f;
	float min_error1 = 0.0f;	// Temporary error value storage for _edge->v1_.
	float min_error2 = 0.0f;	// Temporary error value storage for _edge->v2_.
	bool first = true;

	// Test vertex collaps on all triangles.
	TriangleList::iterator tri_iter;
	for (tri_iter = triangle_list.begin();
	     tri_iter != triangle_list.end();
	     ++tri_iter) {
		Triangle* triangle = *tri_iter;
		vec3 diff1;
		vec3 diff2;
		Vertex mid;

		// Test V1 and V2.
		mid.x() = (triangle->v1_->x() + triangle->v2_->x()) * 0.5f;
		mid.y() = (triangle->v1_->y() + triangle->v2_->y()) * 0.5f;
		mid.z() = (triangle->v1_->z() + triangle->v2_->z()) * 0.5f;

		// Calculate the distance between the new, merged position,
		// and the original vertex position.
		diff1.Set(mid.x() - triangle->v1_->twin_->x(),
		            mid.y() - triangle->v1_->twin_->y(),
		            mid.z() - triangle->v1_->twin_->z());
		diff2.Set(mid.x() - triangle->v2_->twin_->x(),
		            mid.y() - triangle->v2_->twin_->y(),
		            mid.z() - triangle->v2_->twin_->z());

		float error1 = diff1.GetLength() + triangle->v1_->error_;
		float error2 = diff2.GetLength() + triangle->v2_->error_;
		float error = (error1 + error2 + current_error) / 3.0f;

		if (first == true || error < min_error) {
			edge.v1_ = triangle->v1_;
			edge.v2_ = triangle->v2_;
			min_error1 = error1;
			min_error2 = error2;
			min_error = error;
			first = false;
		}

		// Test V2 and V3.
		mid.x() = (triangle->v2_->x() + triangle->v3_->x()) * 0.5f;
		mid.y() = (triangle->v2_->y() + triangle->v3_->y()) * 0.5f;
		mid.z() = (triangle->v2_->z() + triangle->v3_->z()) * 0.5f;

		// Calculate the distance between the new, merged position,
		// and the original vertex position.
		diff1.Set(mid.x() - triangle->v2_->twin_->x(),
		            mid.y() - triangle->v2_->twin_->y(),
		            mid.z() - triangle->v2_->twin_->z());
		diff2.Set(mid.x() - triangle->v3_->twin_->x(),
		            mid.y() - triangle->v3_->twin_->y(),
		            mid.z() - triangle->v3_->twin_->z());

		error1 = diff1.GetLength() + triangle->v1_->error_;
		error2 = diff2.GetLength() + triangle->v2_->error_;
		error = (error1 + error2 + current_error) / 3.0f;

		if (error < min_error) {
			edge.v1_ = triangle->v1_;
			edge.v2_ = triangle->v2_;
			min_error = error;
			min_error1 = error1;
			min_error2 = error2;
		}

		// Test V3 and V1.
		mid.x() = (triangle->v3_->x() + triangle->v1_->x()) * 0.5f;
		mid.y() = (triangle->v3_->y() + triangle->v1_->y()) * 0.5f;
		mid.z() = (triangle->v3_->z() + triangle->v1_->z()) * 0.5f;

		// Calculate the distance between the new, merged position,
		// and the original vertex position.
		diff1.Set(mid.x() - triangle->v3_->twin_->x(),
		            mid.y() - triangle->v3_->twin_->y(),
		            mid.z() - triangle->v3_->twin_->z());
		diff2.Set(mid.x() - triangle->v1_->twin_->x(),
		            mid.y() - triangle->v1_->twin_->y(),
		            mid.z() - triangle->v1_->twin_->z());

		error1 = diff1.GetLength() + triangle->v1_->error_;
		error2 = diff2.GetLength() + triangle->v2_->error_;
		error = (error1 + error2 + current_error) / 3.0f;

		if (error < min_error) {
			edge.v1_ = triangle->v1_;
			edge.v2_ = triangle->v2_;
			min_error = error;
			min_error1 = error1;
			min_error2 = error2;
		}

		if (min_error == 0.0f && edge.v1_ != 0 && edge.v2_ != 0)
			break;
	}

	edge.v1_->error_ = min_error1;
	edge.v2_->error_ = min_error2;

	// Now add all triangles to _edge that share the two vertices
	// _edge->v1_ and _edge->v2_.
	for (tri_iter = triangle_list.begin();
		tri_iter != triangle_list.end();
		++tri_iter) {
		Triangle* triangle = *tri_iter;
		if (triangle->HaveVertex(edge.v1_) &&
			triangle->HaveVertex(edge.v2_)) {
			edge.triangle_list_.push_back(triangle);
		}
	}
}

void ProgressiveTriangleGeometry::Set(TriangleBasedGeometry& geometry) {
	int i;
	ClearAll();

	max_vertex_count_  = geometry.GetMaxVertexCount();
	max_triangle_count_ = geometry.GetMaxTriangleCount();

	tbc::GeometryBase::SetBoundingRadius(geometry.GetBoundingRadius());

	tbc::GeometryBase::SetBasicMaterialSettings(geometry.GetBasicMaterialSettings());

	SetLastFrameVisible(geometry.GetLastFrameVisible());
	SetTransformation(geometry.GetTransformation());

	VertexList   _vertex_list;
	TriangleList _triangle_list;
	VertexList   org_vertex_list;
	TriangleList org_triangle_list;
	VertexSplitList vertex_split_list;

	bool clear_normal_data = (geometry.GetNormalData() != 0);
	geometry.GenerateVertexNormalData();

	/*
		Copy vertex, uv and color data.
	*/

	for (i = 0; i < (int)geometry.GetVertexCount(); i++) {
		Vertex* vertex = new Vertex;
		vertex->x()  = geometry.GetVertexData()[i * 3 + 0];
		vertex->y()  = geometry.GetVertexData()[i * 3 + 1];
		vertex->z()  = geometry.GetVertexData()[i * 3 + 2];
		vertex->nx() = geometry.GetNormalData()[i * 3 + 0];
		vertex->ny() = geometry.GetNormalData()[i * 3 + 1];
		vertex->nz() = geometry.GetNormalData()[i * 3 + 2];

		if (geometry.GetUVData() != 0) {
			vertex->u() = geometry.GetUVData()[i * 2 + 0];
			vertex->v() = geometry.GetUVData()[i * 2 + 1];
		} else {
			vertex->u() = 0.0f;
			vertex->v() = 0.0f;
		}

		if (geometry.GetColorData() != 0) {
			int __size = 4;
			if (geometry.GetColorFormat() == tbc::GeometryBase::kColorRgb)
				__size = 3;

			vertex->r() = (float)geometry.GetColorData()[i * __size + 0] / 255.0f;
			vertex->g() = (float)geometry.GetColorData()[i * __size + 1] / 255.0f;
			vertex->b() = (float)geometry.GetColorData()[i * __size + 2] / 255.0f;
			vertex->a() = 1.0f;

			if (__size == 4) {
				vertex->a() = (float)geometry.GetColorData()[i * __size + 3] / 255.0f;
			}
		} else {
			vertex->r() = 0.0f;
			vertex->g() = 0.0f;
			vertex->b() = 0.0f;
			vertex->a() = 1.0f;
		}

		_vertex_list.push_back(vertex);

		Vertex* v_copy = new Vertex(vertex);
		vertex->twin_ = v_copy;
		// Keep a copy of the original mesh.
		org_vertex_list.push_back(v_copy);
	}

	/*
		Copy triangles.
	*/

	int triangle_count = (int)geometry.GetTriangleCount();
	for (i = 0; i < triangle_count; i++) {
		Triangle* triangle = new Triangle;
		Triangle* triangle2 = new Triangle;

		uint32 indices[4];
		geometry.GetTriangleIndices(i, indices);

		triangle->v1_ = *ListUtil::FindByIndex(_vertex_list, indices[0]);
		triangle->v2_ = *ListUtil::FindByIndex(_vertex_list, indices[1]);
		triangle->v3_ = *ListUtil::FindByIndex(_vertex_list, indices[2]);

		triangle2->v1_ = *ListUtil::FindByIndex(org_vertex_list, indices[0]);
		triangle2->v2_ = *ListUtil::FindByIndex(org_vertex_list, indices[1]);
		triangle2->v3_ = *ListUtil::FindByIndex(org_vertex_list, indices[2]);

		_triangle_list.push_back(triangle);
		org_triangle_list.push_back(triangle2);
	}

	/*
		The main loop.
	*/

	Edge edge_to_collapse;

	while (!_triangle_list.empty()) {
		// FindEdgeToCollapse() will put the collapsing edge last in the array.
		FindEdgeToCollapse(org_vertex_list,
				   org_triangle_list,
				   _vertex_list,
				   _triangle_list,
				   edge_to_collapse);

		VertexSplit* vertex_split = new VertexSplit;
		vertex_split_list.push_back(vertex_split);

		vertex_split->vertex_to_split_ = edge_to_collapse.v2_;
		vertex_split->new_vertex_ = edge_to_collapse.v1_;

		vertex_split->num_old_triangles_ = (int)(_triangle_list.size() - edge_to_collapse.triangle_list_.size());
		vertex_split->num_new_triangles_ = (int)edge_to_collapse.triangle_list_.size();

		SetDeltaScalars(vertex_split->delta_scalars_,
				edge_to_collapse.v1_->scalars_,
				edge_to_collapse.v2_->scalars_);

		CopyAddScalars(vertex_split->pivot_scalars_,
				edge_to_collapse.v2_->scalars_,
				vertex_split->delta_scalars_);

		CopyScalars(edge_to_collapse.v2_->scalars_,
				vertex_split->pivot_scalars_);

		TriangleList::iterator tri_iter;

		for (tri_iter = edge_to_collapse.triangle_list_.begin();
			tri_iter != edge_to_collapse.triangle_list_.end();
			++tri_iter) {
			Triangle* triangle = *tri_iter;
			_triangle_list.remove(triangle);
			vertex_split->new_triangles_.push_back(triangle);
		}

		// Setup the affected triangles. Affected triangles are those that
		// are sharing the vertex that will be removed. We need to change the pointer that
		// points at the removed vertex to point to the other vertex instead.
		for (tri_iter =  _triangle_list.begin();
			tri_iter != _triangle_list.end();
			++tri_iter) {
			Triangle* triangle = *tri_iter;

			if (triangle->HaveVertex(edge_to_collapse.v1_) == true) {
				vertex_split->fix_triangles_.push_back(triangle);
				triangle->ReplaceVertex(edge_to_collapse.v1_, vertex_split->vertex_to_split_);

				// If two vertices are the same.
				if (triangle->v1_ == triangle->v2_ ||
				   triangle->v1_ == triangle->v3_ ||
				   triangle->v2_ == triangle->v3_) {
					deb_assert(false);
				}
			}
		}

		// Place V2 last in the list, and remove V1 completely.
		_vertex_list.remove(edge_to_collapse.v1_);
		_vertex_list.remove(edge_to_collapse.v2_);
		_vertex_list.push_back(edge_to_collapse.v2_);
	} // End while(lVertexCount > 1)


	// Now we are all done calculating the vertex splits.
	// It's time to generate the final data.

	// Start with creating the lowest level mesh data.
	base_vertex_count_ = (int)_vertex_list.size();
	base_triangle_count_ = (int)_triangle_list.size();
	num_vertex_splits_ = (int)vertex_split_list.size();

	current_vertex_count_ = base_vertex_count_;
	current_triangle_count_ = base_triangle_count_;

	base_vertex_data_ = new float[base_vertex_count_ * 3];
	base_normal_data_ = new float[base_vertex_count_ * 3];
	current_vertex_data_ = new float[(current_vertex_count_ + num_vertex_splits_) * 3];
	current_normal_data_ = new float[(current_vertex_count_ + num_vertex_splits_) * 3];

	if (geometry.GetUVData() != 0) {
		base_uv_data_ = new float[base_vertex_count_ * 2];
		current_uv_data_ = new float[(current_vertex_count_ + num_vertex_splits_) * 2];
	}
	if (geometry.GetColorData() != 0) {
		base_color_data_ = new float[base_vertex_count_ * 4];
		current_color_data_ = new float[(current_vertex_count_ + num_vertex_splits_) * 4];
		current_color_data8_ = new uint8[(current_vertex_count_ + num_vertex_splits_) * 4];
	}

	VertexList::iterator vertex_iter;

	for (vertex_iter = _vertex_list.begin(), i = 0;
		vertex_iter != _vertex_list.end();
		++vertex_iter, ++i) {
		Vertex* vertex = *vertex_iter;

		base_vertex_data_[i * 3 + 0] = vertex->x();
		base_vertex_data_[i * 3 + 1] = vertex->y();
		base_vertex_data_[i * 3 + 2] = vertex->z();
		current_vertex_data_[i * 3 + 0] = vertex->x();
		current_vertex_data_[i * 3 + 1] = vertex->y();
		current_vertex_data_[i * 3 + 2] = vertex->z();

		base_normal_data_[i * 3 + 0] = vertex->nx();
		base_normal_data_[i * 3 + 1] = vertex->ny();
		base_normal_data_[i * 3 + 2] = vertex->nz();
		current_normal_data_[i * 3 + 0] = vertex->nx();
		current_normal_data_[i * 3 + 1] = vertex->ny();
		current_normal_data_[i * 3 + 2] = vertex->nz();

		if (geometry.GetUVData() != 0) {
			base_uv_data_[i * 2 + 0] = vertex->u();
			base_uv_data_[i * 2 + 1] = vertex->v();

			current_uv_data_[i * 2 + 0] = vertex->u();
			current_uv_data_[i * 2 + 1] = vertex->v();
		}

		if (geometry.GetColorData() != 0) {
			base_color_data_[i * 4 + 0] = vertex->r();
			base_color_data_[i * 4 + 1] = vertex->g();
			base_color_data_[i * 4 + 2] = vertex->b();
			base_color_data_[i * 4 + 3] = vertex->a();

			current_color_data_[i * 4 + 0] = vertex->r();
			current_color_data_[i * 4 + 1] = vertex->g();
			current_color_data_[i * 4 + 2] = vertex->b();
			current_color_data_[i * 4 + 3] = vertex->a();

			current_color_data8_[i * 4 + 0] = (uint8)(vertex->r() * 255.0f);
			current_color_data8_[i * 4 + 1] = (uint8)(vertex->g() * 255.0f);
			current_color_data8_[i * 4 + 2] = (uint8)(vertex->b() * 255.0f);
			current_color_data8_[i * 4 + 3] = (uint8)(vertex->a() * 255.0f);
		}
	}

	current_indices_ = new uint32[_triangle_list.size() * 3];

	if (_triangle_list.empty() == false) {
		base_indices_ = new uint32[_triangle_list.size() * 3];

		TriangleList::iterator tri_iter;
		for (tri_iter = _triangle_list.begin(), i = 0;
			tri_iter != _triangle_list.end();
			++tri_iter, ++i) {
			Triangle* triangle = *tri_iter;

			unsigned long index0 = ListUtil::FindIndexOf(_vertex_list, triangle->v1_);
			unsigned long index1 = ListUtil::FindIndexOf(_vertex_list, triangle->v2_);
			unsigned long index2 = ListUtil::FindIndexOf(_vertex_list, triangle->v3_);

			deb_assert(index0 >= 0);
			deb_assert(index1 >= 0);
			deb_assert(index2 >= 0);

			base_indices_[i * 3 + 0] = index0;
			base_indices_[i * 3 + 1] = index1;
			base_indices_[i * 3 + 2] = index2;
			current_indices_[i * 3 + 0] = index0;
			current_indices_[i * 3 + 1] = index1;
			current_indices_[i * 3 + 2] = index2;
		}
	}

	// Now let's create the rest of the data needed.

	if (vertex_split_list.size() > 0) {
		int vertex_index = (int)_vertex_list.size();

		vertex_split_ = new VertexSplit[vertex_split_list.size()];

		VertexSplitList::reverse_iterator vs_iter;

		for (vs_iter = vertex_split_list.rbegin(), i = 0;
			vs_iter != vertex_split_list.rend();
			++vs_iter, i++) {
			VertexSplit* vs = *vs_iter;
			vertex_split_[i].LightCopy(*vs);
			vertex_split_[i].num_new_triangles_    = (int)vs->new_triangles_.size();
			vertex_split_[i].num_old_triangles_    = (int)_triangle_list.size();
			vertex_split_[i].num_old_vertices_     = (int)_vertex_list.size();
			vertex_split_[i].vertex_to_split_index_ = ListUtil::FindIndexOf(_vertex_list, vs->vertex_to_split_);
			vertex_split_[i].num_index_fixes_      = (int)vs->fix_triangles_.size();
			vertex_split_[i].index_fix_           = new int[vertex_split_[i].num_index_fixes_];
			vertex_split_[i].index_fix_index_      = new int[vertex_split_[i].num_index_fixes_ * 2];

			// Write new vertex data.
			_vertex_list.push_back(vs->new_vertex_);

			current_vertex_data_[vertex_index * 3 + 0] = vs->new_vertex_->x();
			current_vertex_data_[vertex_index * 3 + 1] = vs->new_vertex_->y();
			current_vertex_data_[vertex_index * 3 + 2] = vs->new_vertex_->z();

			current_normal_data_[vertex_index * 3 + 0] = vs->new_vertex_->nx();
			current_normal_data_[vertex_index * 3 + 1] = vs->new_vertex_->ny();
			current_normal_data_[vertex_index * 3 + 2] = vs->new_vertex_->nz();

			if (geometry.GetUVData() != 0) {
				current_uv_data_[vertex_index * 2 + 0] = vs->new_vertex_->u();
				current_uv_data_[vertex_index * 2 + 1] = vs->new_vertex_->v();
			}

			if (geometry.GetColorData() != 0) {
				current_color_data_[vertex_index * 4 + 0] = vs->new_vertex_->r();
				current_color_data_[vertex_index * 4 + 1] = vs->new_vertex_->g();
				current_color_data_[vertex_index * 4 + 2] = vs->new_vertex_->b();
				current_color_data_[vertex_index * 4 + 3] = vs->new_vertex_->a();

				current_color_data8_[vertex_index * 4 + 0] = (uint8)(vs->new_vertex_->r() * 255.0f);
				current_color_data8_[vertex_index * 4 + 1] = (uint8)(vs->new_vertex_->g() * 255.0f);
				current_color_data8_[vertex_index * 4 + 2] = (uint8)(vs->new_vertex_->b() * 255.0f);
				current_color_data8_[vertex_index * 4 + 3] = (uint8)(vs->new_vertex_->a() * 255.0f);
			}

			vertex_index++;

			// Write index fixes. (Fix the "old" triangles).
			int j;
			TriangleList::iterator tri_iter;

			for (tri_iter = vs->fix_triangles_.begin(), j = 0;
				tri_iter != vs->fix_triangles_.end();
				++tri_iter, ++j) {
				Triangle* triangle = *tri_iter;

				deb_assert(triangle->vertex_index_history_.size() > 0);
				deb_assert(triangle->vertex_history_.size() > 0);

				int tri_index = ListUtil::FindIndexOf(_triangle_list, triangle);
				int vertex = triangle->vertex_index_history_.back();
				triangle->vertex_index_history_.pop_back();
				int index = tri_index * 3 + vertex;
				vertex_split_[i].index_fix_[j] = index;

				int old_value = 0;
				int new_value = 0;

				switch(vertex) {
				case 0:
					old_value = ListUtil::FindIndexOf(_vertex_list, triangle->v1_);
					triangle->v1_ = triangle->vertex_history_.back();
					triangle->vertex_history_.pop_back();
					new_value = ListUtil::FindIndexOf(_vertex_list, triangle->v1_);
					break;
				case 1:
					old_value = ListUtil::FindIndexOf(_vertex_list, triangle->v2_);
					triangle->v2_ = triangle->vertex_history_.back();
					triangle->vertex_history_.pop_back();
					new_value = ListUtil::FindIndexOf(_vertex_list, triangle->v2_);
					break;
				case 2:
					old_value = ListUtil::FindIndexOf(_vertex_list, triangle->v3_);
					triangle->v3_ = triangle->vertex_history_.back();
					triangle->vertex_history_.pop_back();
					new_value = ListUtil::FindIndexOf(_vertex_list, triangle->v3_);
					break;
				};

				vertex_split_[i].index_fix_index_[j * 2 + 0] = old_value;
				vertex_split_[i].index_fix_index_[j * 2 + 1] = new_value;
			}

			// Write all new triangles.
			for (tri_iter = vs->new_triangles_.begin();
				tri_iter != vs->new_triangles_.end();
				++tri_iter) {
				Triangle* triangle = *tri_iter;
				_triangle_list.push_back(triangle);
				int tri_index = (int)_triangle_list.size() - 1;

				unsigned long index0 = ListUtil::FindIndexOf(_vertex_list, triangle->v1_);
				unsigned long index1 = ListUtil::FindIndexOf(_vertex_list, triangle->v2_);
				unsigned long index2 = ListUtil::FindIndexOf(_vertex_list, triangle->v3_);

				current_indices_[tri_index * 3 + 0] = index0;
				current_indices_[tri_index * 3 + 1] = index1;
				current_indices_[tri_index * 3 + 2] = index2;
			}
		}
	}

	// Delete all memory allocated.
	ListUtil::DeleteAll(_triangle_list);
	ListUtil::DeleteAll(_vertex_list);
	ListUtil::DeleteAll(vertex_split_list);

	if (clear_normal_data == true) {
		geometry.ClearVertexNormalData();
	}
}

void ProgressiveTriangleGeometry::SetDetailLevel(float level_of_detail) {
	if (num_vertex_splits_ == 0) {
		return;
	}

	if (level_of_detail < 0.0f) {
		level_of_detail = 0.0f;
	}
	if (level_of_detail > 1.0f) {
		level_of_detail = 1.0f;
	}

	unsigned int target_v_split = (unsigned int)(level_of_detail * (float)num_vertex_splits_);
	float frac = level_of_detail * (float)num_vertex_splits_ - (float)target_v_split;

	if (target_v_split >= num_vertex_splits_) {
		target_v_split = num_vertex_splits_ - 1;
		frac = 1.0f;
	}

	if (target_v_split >= current_v_split_) {
		float __f = frac;
		frac = 1.0f;

		// Expand some edges.
		for (int i = current_v_split_; i <= (int)target_v_split; i++) {
			if (i == (int)target_v_split)
				frac = __f;

			// Setup the new vertices.
			float new_scalars0[kVNumscalars];
			float new_scalars1[kVNumscalars];

			CopyAddScalars(new_scalars0,
						   vertex_split_[i].pivot_scalars_,
						   vertex_split_[i].delta_scalars_,
						   -frac);

			CopyAddScalars(new_scalars1,
						   vertex_split_[i].pivot_scalars_,
						   vertex_split_[i].delta_scalars_,
						   frac);

			int index0 = vertex_split_[i].vertex_to_split_index_ * 3;
			int index1 = vertex_split_[i].num_old_vertices_ * 3; // The newly created vertex.

			CopyScalars(&current_vertex_data_[index0], &new_scalars0[kVx], 3);
			CopyScalars(&current_vertex_data_[index1], &new_scalars1[kVx], 3);

			float scale0 = 1.0f / sqrt(new_scalars0[kVnx] * new_scalars0[kVnx] +
										  new_scalars0[kVny] * new_scalars0[kVny] +
										  new_scalars0[kVnz] * new_scalars0[kVnz]);
			float scale1 = 1.0f / sqrt(new_scalars1[kVnx] * new_scalars1[kVnx] +
										  new_scalars1[kVny] * new_scalars1[kVny] +
										  new_scalars1[kVnz] * new_scalars1[kVnz]);
			CopyScalars(&current_normal_data_[index0], &new_scalars0[kVnx], 3, scale0);
			CopyScalars(&current_normal_data_[index1], &new_scalars1[kVnx], 3, scale1);

			if (current_uv_data_ != 0) {
				int uv_index0 = vertex_split_[i].vertex_to_split_index_ * 2;
				int uv_index1 = vertex_split_[i].num_old_vertices_ * 2;
				CopyScalars(&current_uv_data_[uv_index0], &new_scalars0[kVu], 2);
				CopyScalars(&current_uv_data_[uv_index1], &new_scalars1[kVu], 2);
			}

			if (current_color_data_ != 0) {
				int color_index0 = vertex_split_[i].vertex_to_split_index_ * 3;
				int color_index1 = vertex_split_[i].num_old_vertices_ * 3;
				CopyScalars(&current_color_data_[color_index0], &new_scalars0[kVr], 3);
				CopyScalars(&current_color_data_[color_index1], &new_scalars1[kVr], 3);
				CopyScalarsUC(&current_color_data8_[color_index0], &new_scalars0[kVr], 3);
				CopyScalarsUC(&current_color_data8_[color_index1], &new_scalars1[kVr], 3);
			}

			// New triangles.
			current_triangle_count_ = vertex_split_[i].num_old_triangles_ +
									vertex_split_[i].num_new_triangles_;

			current_vertex_count_ = vertex_split_[i].num_old_vertices_ + 1;

			// Fix old triangles that should share the new vertex.
			for (int j = 0; j < vertex_split_[i].num_index_fixes_; j++) {
				current_indices_[vertex_split_[i].index_fix_[j]] = vertex_split_[i].index_fix_index_[j * 2 + 1];
			}
		}

		current_v_split_ = target_v_split;
	} else if(target_v_split < current_v_split_) {
		float __f = frac;
		frac = 1.0f;

		// Collapse some edges.
		for (int i = current_v_split_ - 1; i >= (int)target_v_split; i--) {
			if (i == (int)target_v_split)
				frac = __f;

			// Remove triangles.
			current_triangle_count_ = vertex_split_[i].num_old_triangles_ +
									vertex_split_[i].num_new_triangles_;

			current_vertex_count_ = vertex_split_[i].num_old_vertices_ + 1;

			// Setup the new vertex.
			float new_scalars0[kVNumscalars];
			float new_scalars1[kVNumscalars];

			CopyAddScalars(new_scalars0,
						   vertex_split_[i].pivot_scalars_,
						   vertex_split_[i].delta_scalars_,
						   -frac);

			CopyAddScalars(new_scalars1,
						   vertex_split_[i].pivot_scalars_,
						   vertex_split_[i].delta_scalars_,
						   frac);

			int index0 = vertex_split_[i].vertex_to_split_index_ * 3;
			int index1 = vertex_split_[i].num_old_vertices_ * 3; // The newly created vertex.

			CopyScalars(&current_vertex_data_[index0], &new_scalars0[kVx], 3);
			CopyScalars(&current_vertex_data_[index1], &new_scalars1[kVx], 3);

			float scale0 = 1.0f / sqrt(new_scalars0[kVnx] * new_scalars0[kVnx] +
										  new_scalars0[kVny] * new_scalars0[kVny] +
										  new_scalars0[kVnz] * new_scalars0[kVnz]);
			float scale1 = 1.0f / sqrt(new_scalars1[kVnx] * new_scalars1[kVnx] +
										  new_scalars1[kVny] * new_scalars1[kVny] +
										  new_scalars1[kVnz] * new_scalars1[kVnz]);
			CopyScalars(&current_normal_data_[index0], &new_scalars0[kVnx], 3, scale0);
			CopyScalars(&current_normal_data_[index1], &new_scalars1[kVnx], 3, scale1);

			if (current_uv_data_ != 0) {
				int uv_index0 = vertex_split_[i].vertex_to_split_index_ * 2;
				int uv_index1 = vertex_split_[i].num_old_vertices_ * 2;
				CopyScalars(&current_uv_data_[uv_index0], &new_scalars0[kVu], 2);
				CopyScalars(&current_uv_data_[uv_index1], &new_scalars1[kVu], 2);
			}

			if (current_color_data_ != 0) {
				int color_index0 = vertex_split_[i].vertex_to_split_index_ * 3;
				int color_index1 = vertex_split_[i].num_old_vertices_ * 3;
				CopyScalars(&current_color_data_[color_index0], &new_scalars0[kVr], 3);
				CopyScalars(&current_color_data_[color_index1], &new_scalars1[kVr], 3);
				CopyScalarsUC(&current_color_data8_[color_index0], &new_scalars0[kVr], 3);
				CopyScalarsUC(&current_color_data8_[color_index1], &new_scalars1[kVr], 3);
			}

			for (int j = 0; j < vertex_split_[i + 1].num_index_fixes_; j++) {
				current_indices_[vertex_split_[i + 1].index_fix_[j]] =
					vertex_split_[i + 1].index_fix_index_[j * 2 + 0];
			}
		}

		current_v_split_ = target_v_split;
	}

	tbc::GeometryBase::SetVertexDataChanged(true);
	tbc::GeometryBase::SetColorDataChanged(true);
	tbc::GeometryBase::SetUVDataChanged(true);
	tbc::GeometryBase::SetIndexDataChanged(true);
}

void ProgressiveTriangleGeometry::GetCurrentState(TriangleBasedGeometry& geometry) {
	geometry.Set(current_vertex_data_,
	              current_normal_data_,
	              current_uv_data_,
	              current_color_data8_,
	              TriangleBasedGeometry::kColorRgba,
	              current_indices_,
	              current_vertex_count_,
	              current_triangle_count_ * 3,
	              tbc::GeometryBase::kTriangles,
	              tbc::GeometryBase::kGeomDynamic);
}



}
