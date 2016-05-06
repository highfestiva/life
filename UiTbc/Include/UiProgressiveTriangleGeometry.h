
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "uitbc.h"
#include "uitrianglebasedgeometry.h"
#include "../../tbc/include/geometrybase.h"
#include <list>



namespace uitbc {



class ProgressiveTriangleGeometry : public tbc::GeometryBase {
public:
	ProgressiveTriangleGeometry();
	ProgressiveTriangleGeometry(ProgressiveTriangleGeometry& progressive_geometry);
	ProgressiveTriangleGeometry(TriangleBasedGeometry& geometry);
	virtual ~ProgressiveTriangleGeometry();

	void Copy(ProgressiveTriangleGeometry& progressive_geometry);
	void Set(TriangleBasedGeometry& geometry);

	// SetDetailLevel() will collapse or expand edges to the given level of detail.
	// level_of_detail must be a value between 0 and 1, where 0 is the lowest
	// detail level, and 1 is the highest.
	void SetDetailLevel(float level_of_detail);

	void GetCurrentState(TriangleBasedGeometry& geometry);

	// Data access...
	inline virtual unsigned int GetMaxVertexCount() const;
	inline virtual unsigned int GetMaxNumIndices() const;

	inline virtual unsigned int GetVertexCount() const;
	inline virtual unsigned int GetNumIndices() const;
	inline virtual unsigned int GetNumUVSets()    const;

	// When writing to any of the arrays returned by the following functions,
	// the results of any future calls to the member functions is undefined.
	inline virtual float*         GetVertexData() const;
	inline virtual float*         GetUVData(unsigned int uv_set) const;
	inline virtual uint32* GetIndices() const;
	inline virtual uint8*  GetColorData() const;
	inline virtual float*         GetNormalData() const; // Vertex normals..

	inline virtual void ClearVertexNormalData();
	inline virtual bool HaveVertexNormalData();

	inline ColorFormat GetColorFormat() const;

	// Overloads from tbc::GeometryBase.
	virtual inline GeometryVolatility GetGeometryVolatility() const;

private:

	/*
		Some structures and functions used to make life easier
		when producing the progressive mesh.
	*/

	enum {
		kVx = 0,
		kVy,
		kVz,
		kVu,
		kVv,
		kVr,
		kVg,
		kVb,
		kVa,
		kVnx,
		kVny,
		kVnz,

		kVNumscalars
	};

	inline static void CopyScalars(float* dest, const float* source, int n = kVNumscalars, float scale = 1.0f) {
		for (int i = 0; i < n; i++) {
			dest[i] = source[i] * scale;
		}
	}

	inline static void CopyScalarsUC(unsigned char* dest, const float* source, int n = kVNumscalars, float scale = 255.0f) {
		for (int i = 0; i < n; i++) {
			dest[i] = (unsigned char)(source[i] * scale);
		}
	}

	inline static void SetScalars(float* dest, float value) {
		for (int i = 0; i < kVNumscalars; i++) {
			dest[i] = value;
		}
	}

	inline static void SetDeltaScalars(float* dest, float* src1, float* src2) {
		for (int i = 0; i < kVNumscalars; i++) {
			dest[i] = (src1[i] - src2[i]) * 0.5f;
		}
	}

	inline static void CopyAddScalars(float* dest, float* src1, float* src2, float frac = 1.0f) {
		for (int i = 0; i < kVNumscalars; i++) {
			dest[i] = src1[i] + src2[i] * frac;
		}
	}

	class Triangle;
	class Edge;
	class Vertex;
	typedef std::list<Triangle*> TriangleList;
	typedef std::list<Vertex*>   VertexList;
	typedef std::list<Edge*>     EdgeList;

	class Vertex {
	public:

		Vertex() :
			error_(0),
			twin_(0) {
			SetScalars(scalars_, 0.0f);
		}

		Vertex(Vertex* _v) :
			error_(_v->error_),
			twin_(0) {
			CopyScalars(scalars_, _v->scalars_);
		}

		inline float& x() { return scalars_[kVx]; }
		inline float& y() { return scalars_[kVy]; }
		inline float& z() { return scalars_[kVz]; }
		inline float& u() { return scalars_[kVu]; }
		inline float& v() { return scalars_[kVv]; }
		inline float& r() { return scalars_[kVr]; }
		inline float& g() { return scalars_[kVg]; }
		inline float& b() { return scalars_[kVb]; }
		inline float& a() { return scalars_[kVa]; }
		inline float& nx() { return scalars_[kVnx]; }
		inline float& ny() { return scalars_[kVny]; }
		inline float& nz() { return scalars_[kVnz]; }

		float scalars_[kVNumscalars];
		float error_; // Used in FindEdgeToCollapse().

		Vertex* twin_;
	};

	class Edge {
	public:

		Edge() {
			v1_ = 0;
			v2_ = 0;
		}

		~Edge() {
			v1_ = 0;
			v2_ = 0;
			triangle_list_.clear();
		}

		Vertex* v1_;
		Vertex* v2_;
		TriangleList triangle_list_;

		inline bool HaveVertex(Vertex* vertex) {
			return (v1_ == vertex || v2_ == vertex);
		}
	};

	class Triangle {
	public:

		Triangle() :
			v1_(0),
			v2_(0),
			v3_(0) {
		}

		Vertex* v1_;
		Vertex* v2_;
		Vertex* v3_;

		vec3 normal_;    // Used in FindEdgeToCollapse().
		std::list<int> vertex_index_history_;
		VertexList vertex_history_;

		inline bool HaveVertex(Vertex* vertex) {
			return (vertex == v1_ || vertex == v2_ || vertex == v3_);
		}

		inline void ReplaceVertex(Vertex* vertex, Vertex* replacement) {
			if (v1_ == vertex) {
				vertex_index_history_.push_back(0);
				vertex_history_.push_back(v1_);
				v1_ = replacement;
			}
			if (v2_ == vertex) {
				vertex_index_history_.push_back(1);
				vertex_history_.push_back(v2_);
				v2_ = replacement;
			}
			if (v3_ == vertex) {
				vertex_index_history_.push_back(2);
				vertex_history_.push_back(v3_);
				v3_ = replacement;
			}
		}
	};

	// A class used during construction of the progressive mesh (in function Set()).
	class VertexSplit {
	public:
		enum {
			kInvalidIndex = -1,
		};

		VertexSplit() {
			SetScalars(delta_scalars_, 0.0f);
			SetScalars(pivot_scalars_, 0.0f);

			num_new_triangles_  = 0;
			num_old_triangles_  = 0;
			num_old_vertices_   = 0;

			index_fix_        = 0;
			index_fix_index_   = 0;
			num_index_fixes_    = 0;
			vertex_to_split_    = 0;

			vertex_to_split_    = 0;
		}

		virtual ~VertexSplit() {
			ClearAll();
		}

		void ClearAll() {
			SetScalars(delta_scalars_, 0.0f);
			SetScalars(pivot_scalars_, 0.0f);

			vertex_to_split_    = 0;
			num_new_triangles_  = 0;
			num_old_triangles_  = 0;
			num_old_vertices_   = 0;

			if (index_fix_ != 0) {
				delete[] index_fix_;
				delete[] index_fix_index_;
				index_fix_ = 0;
				index_fix_index_ = 0;
			}

			num_index_fixes_ = 0;
			vertex_to_split_ = 0;
			new_vertex_ = 0;

			new_triangles_.clear();
			fix_triangles_.clear();
		}

		void Copy(const VertexSplit& vs) {
			LightCopy(vs);

			num_index_fixes_ = vs.num_index_fixes_;
			new_vertex_ = vs.new_vertex_;

			if (num_index_fixes_ > 0) {
				index_fix_ = new int[num_index_fixes_];
				index_fix_index_ = new int[num_index_fixes_ * 2];
			}

			for (int i = 0; i < num_index_fixes_; i++) {
				index_fix_[i] = vs.index_fix_[i];
				index_fix_index_[i * 2 + 0] = vs.index_fix_index_[i * 2 + 0];
				index_fix_index_[i * 2 + 1] = vs.index_fix_index_[i * 2 + 1];
			}

			TriangleList::const_iterator tri_iter;
			for (tri_iter = vs.new_triangles_.begin();
				tri_iter != vs.new_triangles_.end();
				++tri_iter) {
				new_triangles_.push_back(*tri_iter);
			}

			for (tri_iter = vs.fix_triangles_.begin();
				tri_iter != vs.fix_triangles_.end();
				++tri_iter) {
				fix_triangles_.push_back(*tri_iter);
			}
		}

		void LightCopy(const VertexSplit& vs) {
			ClearAll();

			CopyScalars(delta_scalars_, vs.delta_scalars_);
			CopyScalars(pivot_scalars_, vs.pivot_scalars_);

			vertex_to_split_    = vs.vertex_to_split_;
			num_new_triangles_  = vs.num_new_triangles_;
			num_old_triangles_  = vs.num_old_triangles_;
			num_old_vertices_   = vs.num_old_vertices_;
		}

		float delta_scalars_[kVNumscalars];
		float pivot_scalars_[kVNumscalars];

		// Stores all indices into the index array, where the indices should be updated with
		// new values... Eh..
		// When splitting a vertex, all "old" triangles that point at that vertex,
		// needs to point at the new vertex instead. The indices to update are indexed
		// by this array. I hope that helped to clear things up.
		int* index_fix_;		// Stores the index into the index array.
		int* index_fix_index_; // Stores the actual values to write to the index arrays.
		int num_index_fixes_;
		int vertex_to_split_index_;
		int num_old_triangles_;
		int num_new_triangles_;
		int num_old_vertices_;

		Vertex*    vertex_to_split_;
		Vertex*    new_vertex_;
		TriangleList new_triangles_;
		TriangleList fix_triangles_; // Triangles that needs changed indices.
	};

	typedef std::list<VertexSplit*> VertexSplitList;

	void ClearAll();

	void FindEdgeToCollapse(VertexList& org_vertex_list,
				TriangleList& org_triangle_list,
				VertexList& vertex_list,
				TriangleList& triangle_list,
				Edge& edge);

	unsigned int base_vertex_count_;
	unsigned int base_triangle_count_;

	unsigned int current_vertex_count_;
	unsigned int current_triangle_count_;

	unsigned int max_vertex_count_;
	unsigned int max_triangle_count_;

	float* current_vertex_data_;			// Triplets of (x, y, z).
	float* current_uv_data_;				// Doublets of (u, v).
	float* current_color_data_;
	float* current_normal_data_;			// Triplets of (x, y, z).
	uint8* current_color_data8_;
	uint32* current_indices_;		// Triplets of vertex indices.

	float* base_vertex_data_;			// Triplets of (x, y, z).
	float* base_uv_data_;				// Doublets of (u, v).
	float* base_color_data_;
	float* base_normal_data_;			// Triplets of (x, y, z).
	uint32* base_indices_;	// Triplets of vertex indices.

	unsigned int current_v_split_;
	unsigned int num_vertex_splits_;
	VertexSplit* vertex_split_;

	ColorFormat color_format_;
};

unsigned int ProgressiveTriangleGeometry::GetMaxVertexCount() const {
	return max_vertex_count_;
}

unsigned int ProgressiveTriangleGeometry::GetMaxNumIndices() const {
	return max_triangle_count_ * 3;
}

unsigned int ProgressiveTriangleGeometry::GetVertexCount() const {
	return current_vertex_count_;
}

unsigned int ProgressiveTriangleGeometry::GetNumIndices() const {
	return current_triangle_count_ * 3;
}

unsigned int ProgressiveTriangleGeometry::GetNumUVSets() const {
	if (base_uv_data_ != 0)
		return 1;
	else
		return 0;
}

float* ProgressiveTriangleGeometry::GetVertexData() const {
	return current_vertex_data_;
}

float* ProgressiveTriangleGeometry::GetUVData(unsigned int /*uv_set*/) const {
	return current_uv_data_;
}

uint32* ProgressiveTriangleGeometry::GetIndices() const {
	return current_indices_;
}

uint8* ProgressiveTriangleGeometry::GetColorData() const {
	return current_color_data8_;
}

float* ProgressiveTriangleGeometry::GetNormalData() const {
	return current_normal_data_;
}

tbc::GeometryBase::GeometryVolatility ProgressiveTriangleGeometry::GetGeometryVolatility() const {
	// Volatile, because the geometry is constantly changed.
	return tbc::GeometryBase::kGeomVolatile;
}

tbc::GeometryBase::ColorFormat ProgressiveTriangleGeometry::GetColorFormat() const {
	return color_format_;
}

void ProgressiveTriangleGeometry::ClearVertexNormalData() {
	// Do nothing.
}

bool ProgressiveTriangleGeometry::HaveVertexNormalData() {
	return (current_normal_data_ != 0);
}



}
