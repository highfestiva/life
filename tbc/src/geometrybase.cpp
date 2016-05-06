
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/geometrybase.h"
#include "../../lepra/include/lepraassert.h"
#include "../../lepra/include/resourcetracker.h"
#include "../../lepra/include/vector2d.h"
#include "../include/bones.h"



namespace tbc {



GeometryBase::Edge::Edge() {
	//triangle_ = 0;
	triangle_count_ = 0;
	//mTriangleElementCount = 0;

	vertex_[0] = kInvalidIndex;
	vertex_[1] = kInvalidIndex;

	reserved_ = 0;
}

GeometryBase::Edge::Edge(const Edge& edge) {
	//triangle_ = 0;
	triangle_count_ = 0;
	//mTriangleElementCount = 0;

	vertex_[0] = kInvalidIndex;
	vertex_[1] = kInvalidIndex;

	reserved_ = 0;

	Copy(&edge);
}

GeometryBase::Edge::Edge(const Edge* edge) {
	//triangle_ = 0;
	triangle_count_ = 0;
	//mTriangleElementCount = 0;

	vertex_[0] = kInvalidIndex;
	vertex_[1] = kInvalidIndex;

	reserved_ = 0;

	Copy(edge);
}

GeometryBase::Edge::~Edge() {
	ClearAll();
}

void GeometryBase::Edge::ClearAll() {
	/*if (triangle_ != 0) {
		delete[] triangle_;
		triangle_ = 0;
	}*/

	triangle_count_ = 0;
	//mTriangleElementCount = 0;

	vertex_[0] = kInvalidIndex;
	vertex_[1] = kInvalidIndex;

	reserved_ = 0;
}

void GeometryBase::Edge::Copy(const Edge* edge) {
	ClearAll();
	vertex_[0] = edge->vertex_[0];
	vertex_[1] = edge->vertex_[1];

	triangle_count_ = edge->triangle_count_;
	//mTriangleElementCount = edge->triangle_count_;
	//triangle_ = new int[triangle_count_];
	::memcpy(triangle_, edge->triangle_, triangle_count_ * sizeof(int));
}

void GeometryBase::Edge::AddTriangle(int triangle_index) {
	if (HaveTriangle(triangle_index) || triangle_count_ >= 2) {
		return;
	}

	/*if ((triangle_count_ + 1) > mTriangleElementCount) {
		mTriangleElementCount += (mTriangleElementCount >> 2) + 1;
		int* temp = new int[mTriangleElementCount];
		::memcpy(temp, triangle_, triangle_count_ * sizeof(int));

		if (triangle_ != 0) {
			delete[] triangle_;
		}
		triangle_ = temp;
	}*/
	triangle_[triangle_count_] = triangle_index;
	triangle_count_++;
}

void GeometryBase::Edge::RemoveTriangle(int triangle_index) {
	// Find the triangle.
	int i;
	int triangle_to_remove = -1;
	for (i = 0; i < triangle_count_; i++) {
		if (triangle_[i] == triangle_index) {
			triangle_to_remove = i;
			break;
		}
	}

	if (triangle_to_remove != -1) {
		for (i = triangle_to_remove; i < (triangle_count_ - 1); i++) {
			triangle_[i] = triangle_[i + 1];
		}
		--triangle_count_;
	}
}

bool GeometryBase::Edge::HaveTriangle(int triangle_index) {
	for (int i = 0; i < triangle_count_; i++) {
		if (triangle_[i] == triangle_index) {
			return true;
		}
	}

	return false;
}

bool GeometryBase::Edge::HaveTriangle(int triangle_index, int& triangle_pos) {
	triangle_pos = -1;

	for (int i = 0; i < triangle_count_; i++) {
		if (triangle_[i] == triangle_index) {
			triangle_pos = i;
			return true;
		}
	}

	return false;
}

bool GeometryBase::Edge::IsSameEdge(int vertex_index1, int vertex_index2) {
	return (vertex_[0] == vertex_index1 && vertex_[1] == vertex_index2);
}

bool GeometryBase::Edge::IsSameEdge(int vertex_index1,
				    int vertex_index2,
				    int* triangles,
				    int triangle_count) {
	if (triangle_count != triangle_count_) {
		return false;
	}

	bool vertices_match = false;

	if (vertex_[0] == vertex_index1) {
		vertices_match = (vertex_[1] == vertex_index2);
	} else if(vertex_[0] == vertex_index2) {
		vertices_match = (vertex_[1] == vertex_index1);
	}

	if (triangle_count_ == 0 ||
		vertices_match == false) {
		return vertices_match;
	}

	// Vertices match. Now we have to test all triangles too.
	// Start by finding the maximum triangle index.
	int this_max_triangle_index = -1;
	int test_max_triangle_index = -1;
	int i;

	for (i = 0; i < triangle_count_; i++) {
		if (triangle_[i] > this_max_triangle_index) {
			this_max_triangle_index = triangle_[i];
		}
		if (triangles[i] > test_max_triangle_index) {
			test_max_triangle_index = triangles[i];
		}
	}

	if (test_max_triangle_index != this_max_triangle_index) {
		return false;
	}
	if (triangle_count_ == 1 || test_max_triangle_index == 0) {
		return true;
	}

	// The order of triangles in the arrays mustn't matter,
	// that's why we have to setup the follwing array.

	bool* triangle_check = new bool[this_max_triangle_index];
	for (i = 0; i < triangle_count_; i++) {
		triangle_check[i] = false;
	}

	for (i = 0; i < triangle_count_; i++) {
		int triangle0 = triangle_[i];

		bool match = false;

		for (int j = 0; j < triangle_count_; j++) {
			int triangle1 = triangles[j];

			if (triangle_check[triangle1] == false && triangle1 == triangle0) {
				triangle_check[triangle1] = true;
				match = true;
			}
		}

		if (match == false) {
			// There was no match for this triangle. The edges aren't the same.
			delete[] triangle_check;
			return false;
		}
	}

	delete[] triangle_check;
	return true;
}


GeometryBase::GeometryBase() :
	flags_(default_flags_),
	primitive_type_(kTriangles),
	bounding_radius_(0),
	scale_(1),
	surface_normal_data_(0),
	surface_normal_count_(0),
	vertex_normal_data_(0),
	uv_count_per_vertex_(2),
	edge_data_(0),
	edge_count_(0),
	tangent_data_(0),
	bitangent_data_(0),
	tangents_uv_set_(0),
	renderer_data_(0),
	parent_cell_(0),
	last_frame_visible_(0),
	uv_animator_(0),
	pre_render_callback_(0),
	post_render_callback_(0),
	extra_data_(0),
	big_orientation_threshold_(default_big_orientation_threshold_) {
	LEPRA_ACQUIRE_RESOURCE(GeometryBase);
}

GeometryBase::~GeometryBase() {
	ListenerList::iterator iter;
	for(iter = listener_list_.begin(); iter != listener_list_.end(); ) {
		Listener* _listener = *iter;
		++iter;
		_listener->DeletingGeometry(this);
	}

	ClearSurfaceNormalData();
	ClearVertexNormalData();
	ClearTangentAndBitangentData();
	ClearEdgeData();

	LEPRA_RELEASE_RESOURCE(GeometryBase);
}

void GeometryBase::AddListener(Listener* listener) {
	listener_list_.push_back(listener);
}

void GeometryBase::RemoveListener(Listener* listener) {
	listener_list_.remove(listener);
}

const GeometryBase::PreRenderCallback& GeometryBase::GetPreRenderCallback() const {
	return pre_render_callback_;
}

void GeometryBase::SetPreRenderCallback(const PreRenderCallback& callback) {
	pre_render_callback_ = callback;
}

const GeometryBase::PostRenderCallback& GeometryBase::GetPostRenderCallback() const {
	return post_render_callback_;
}

void GeometryBase::SetPostRenderCallback(const PostRenderCallback& callback) {
	post_render_callback_ = callback;
}

bool GeometryBase::IsGeometryReference() {
	return false;
}

void GeometryBase::SetExcludeCulling() {
	SetFlag(kExcludeCulling);
}

bool GeometryBase::IsExcludeCulling() const {
	return CheckFlag(kExcludeCulling);
}

float GeometryBase::GetScale() const {
	return scale_;
}

void GeometryBase::SetScale(float scale) {
	scale_ = scale;
	if (scale_ != 1) {
		LEPRA_DEBUG_CODE(log_.Debugf("Setting scale %f on mesh %s.", scale_, name_.c_str()));
	}
}

void GeometryBase::SetPrimitiveType(PrimitiveType type) {
	primitive_type_ = type;
}

GeometryBase::PrimitiveType GeometryBase::GetPrimitiveType() const {
	return primitive_type_;
}

unsigned GeometryBase::GetMaxTriangleCount() const {
	switch (primitive_type_) {
		case kTriangles:
		case kLineLoop:
			return GetIndexCount() / 3;
		case kTriangleStrip:
			return GetIndexCount() - 2;
		case kLines:
			return GetIndexCount() / 2;
		case kQuads:
			return GetIndexCount() / 4;
		default:
			deb_assert(false);
			return (0);
	}
}

unsigned GeometryBase::GetTriangleCount() const {
	switch (primitive_type_) {
		case kTriangles:
		case kLineLoop:
			return GetIndexCount() / 3;
		case kTriangleStrip:
			return GetIndexCount() - 2;
		case kLines:
			return GetIndexCount() / 2;
		case kQuads:
			return GetIndexCount() / 4;
		default:
			deb_assert(false);
			return (0);
	}
}

void GeometryBase::GetTriangleIndices(int triangle, uint32 indices[4]) const {
	const vtx_idx_t* _indices = GetIndexData();
	switch (primitive_type_) {
		case kTriangles:
		case kLineLoop: {
			const int offset = triangle * 3;
			indices[0] = _indices[offset + 0];
			indices[1] = _indices[offset + 1];
			indices[2] = _indices[offset + 2];
		} break;
		case kTriangleStrip: {
			if ((triangle & 1) == 0) {
				indices[0] = _indices[triangle + 0];
				indices[1] = _indices[triangle + 1];
				indices[2] = _indices[triangle + 2];
			} else {
				// Flipped.
				indices[0] = _indices[triangle + 0];
				indices[1] = _indices[triangle + 2];
				indices[2] = _indices[triangle + 1];
			}
		} break;
		case kLines: {
			const int offset = triangle * 2;
			indices[0] = _indices[offset + 0];
			indices[1] = _indices[offset + 1];
		} break;
		case kQuads: {
			const int offset = triangle * 3;
			indices[0] = _indices[offset + 0];
			indices[1] = _indices[offset + 1];
			indices[2] = _indices[offset + 2];
			indices[3] = _indices[offset + 3];
		} break;
		default: {
			deb_assert(false);
			// These values should hopefully cause a crash.
			indices[0] = (uint32)-100000;
			indices[1] = (uint32)-100000;
			indices[2] = (uint32)-100000;
		} break;
	}
}

void GeometryBase::SetUVCountPerVertex(int uv_count_per_vertex) {
	uv_count_per_vertex_ = uv_count_per_vertex;
}

int GeometryBase::GetUVCountPerVertex() const {
	return uv_count_per_vertex_;
}

unsigned GeometryBase::GetEdgeCount() const {
	return edge_count_;
}

void GeometryBase::SetTangentsUVSet(unsigned uv_set) {
	if (uv_set < GetUVSetCount()) {
		tangents_uv_set_ = uv_set;
	}
}

const GeometryBase::BasicMaterialSettings& GeometryBase::GetBasicMaterialSettings() const {
	return (material_settings_);
}

GeometryBase::BasicMaterialSettings& GeometryBase::GetBasicMaterialSettings() {
	return material_settings_;
}

void GeometryBase::SetBasicMaterialSettings(const BasicMaterialSettings& mat_settings) {
	material_settings_ = mat_settings;
}

void GeometryBase::SetVertexDataChanged(bool changed) {
	SetFlag(kVertexDataChanged, changed);

	if (changed == true) {
		ClearFlag(kSurfaceNormalsValid);
		ClearFlag(kVertexNormalsValid);
		ClearFlag(kTangentsValid);
		ClearFlag(kBoundingRadiusValid);
	}
}

void GeometryBase::SetUVDataChanged(bool changed) {
	SetFlag(kUvDataChanged, changed);

	if (changed == true) {
		ClearFlag(kTangentsValid);
	}
}

void GeometryBase::SetColorDataChanged(bool changed) {
	SetFlag(kColorDataChanged, changed);
}

void GeometryBase::SetIndexDataChanged(bool changed) {
	SetFlag(kIndexDataChanged, changed);

	if (changed == true) {
		ClearFlag(kSurfaceNormalsValid);
		ClearFlag(kVertexNormalsValid);
		ClearFlag(kTangentsValid);
	}
}

void GeometryBase::SetRendererData(void* renderer_data) {
	deb_assert(!renderer_data_ || !renderer_data || renderer_data_ == renderer_data);
	renderer_data_ = renderer_data;
}

size_t GeometryBase::GetExtraData() const {
	return (extra_data_);
}

void GeometryBase::SetExtraData(size_t extra_data) {
	extra_data_ = extra_data;
}

float GeometryBase::GetBoundingRadius() const {
	return bounding_radius_;
}

float GeometryBase::GetBoundingRadius() {
	if (CheckFlag(kBoundingRadiusAlwaysValid) == false &&
	    CheckFlag(kBoundingRadiusValid) == false) {
		CalculateBoundingRadius();
	}

	return bounding_radius_ * scale_;
}

void GeometryBase::SetBoundingRadius(float bounding_radius) {
	bounding_radius_ = bounding_radius / scale_;
	SetFlag(kBoundingRadiusAlwaysValid, bounding_radius_ > 0);
	SetFlag(kBoundingRadiusValid, bounding_radius_ > 0);
}

float* GeometryBase::GetNormalData() const {
	return vertex_normal_data_;
}

float* GeometryBase::GetSurfaceNormalData() const {
	return surface_normal_data_;
}

GeometryBase::Edge* GeometryBase::GetEdgeData() const {
	return edge_data_;
}

float* GeometryBase::GetTangentData() const {
	return tangent_data_;
}

float* GeometryBase::GetBitangentData() const {
	return bitangent_data_;
}

void GeometryBase::ClearAll() {
	ClearVertexNormalData();
	ClearSurfaceNormalData();
	ClearEdgeData();
	ClearTangentAndBitangentData();
	flags_ = 0;
	surface_normal_count_   = 0;
	SetBoundingRadius(0.0f);
}

void GeometryBase::ClearVertexNormalData() {
	delete[] vertex_normal_data_;
	vertex_normal_data_ = 0;
	ClearFlag(kVertexNormalsValid);
}

void GeometryBase::ClearSurfaceNormalData() {
	delete[] surface_normal_data_;
	surface_normal_data_ = 0;
	ClearFlag(kSurfaceNormalsValid);
	surface_normal_count_ = 0;
}


void GeometryBase::ClearEdgeData() {
	delete[] edge_data_;
	edge_data_ = 0;
	edge_count_ = 0;
}

void GeometryBase::ClearTangentAndBitangentData() {
	delete[] tangent_data_;
	tangent_data_ = 0;
	delete[] bitangent_data_;
	bitangent_data_ = 0;

	ClearFlag(kTangentsValid);
}

void GeometryBase::AllocVertexNormalData() {
	if (vertex_normal_data_ == 0) {
		vertex_normal_data_ = new float[GetMaxVertexCount() * 3];
	}
}

void GeometryBase::AllocSurfaceNormalData() {
	if (surface_normal_data_ == 0) {
		unsigned _triangle_count = GetMaxTriangleCount();
		surface_normal_data_ = new float[_triangle_count * 3];
		surface_normal_count_ = _triangle_count;
	}
}

void GeometryBase::AllocTangentAndBitangentData() {
	int _vertex_count = GetMaxVertexCount();
	if (tangent_data_ == 0) {
		tangent_data_ = new float[_vertex_count * 3];
	}
	if (bitangent_data_ == 0) {
		bitangent_data_ = new float[_vertex_count * 3];
	}
}

void GeometryBase::SetLastFrameVisible(unsigned last_frame_visible) {
	last_frame_visible_ = last_frame_visible;
}

unsigned GeometryBase::GetLastFrameVisible() const {
	return last_frame_visible_;
}

void GeometryBase::SetTransformation(const xform& transformation) {
	transformation_ = transformation;
	SetTransformationChanged(true);

	quat q(big_orientation_.data_);
	quat r(GetTransformation().orientation_.data_);	// Must let overrides go to work, so we can store full update.
	q.Sub(r.data_);
	if (q.GetNorm() > big_orientation_threshold_) {
		big_orientation_ = r;
		SetBigOrientationChanged(true);
	}
}

const xform& GeometryBase::GetBaseTransformation() const {
	return transformation_;
}

const xform& GeometryBase::GetTransformation() {
	return transformation_;
}

const quat& GeometryBase::GetLastBigOrientation() const {
	return big_orientation_;
}

float GeometryBase::GetBigOrientationThreshold() const {
	return big_orientation_threshold_;
}

void GeometryBase::SetBigOrientationThreshold(float big_orientation_threshold) {
	big_orientation_threshold_ = big_orientation_threshold;
}

void GeometryBase::SetDefaultBigOrientationThreshold(float big_orientation_threshold) {
	default_big_orientation_threshold_ = big_orientation_threshold;
}

void GeometryBase::SetParentCell(PortalManager::Cell* cell) {
	parent_cell_ = cell;
}

PortalManager::Cell* GeometryBase::GetParentCell() {
	return parent_cell_;
}

void GeometryBase::SetUVAnimator(BoneAnimator* uv_animator) {
	uv_animator_ = uv_animator;
}

BoneAnimator* GeometryBase::GetUVAnimator() {
	return uv_animator_;
}

void GeometryBase::Copy(GeometryBase* geometry) {
	ClearAll();

	SetPrimitiveType(geometry->GetPrimitiveType());
	SetUVCountPerVertex(geometry->GetUVCountPerVertex());
	SetBigOrientationThreshold(geometry->GetBigOrientationThreshold());
	material_settings_ = geometry->GetBasicMaterialSettings();
	flags_ = geometry->flags_;
	bounding_radius_    = geometry->bounding_radius_;

	surface_normal_count_ = geometry->surface_normal_count_;
	if (geometry->GetSurfaceNormalData() != 0) {
		surface_normal_data_ = new float[surface_normal_count_ * 3];
		::memcpy(surface_normal_data_, geometry->GetSurfaceNormalData(), surface_normal_count_ * 3 * sizeof(float));
	}

	if (geometry->vertex_normal_data_ != 0) {
		unsigned _vertex_count = geometry->GetVertexCount();
		vertex_normal_data_ = new float[_vertex_count * 3];
		::memcpy(vertex_normal_data_, geometry->vertex_normal_data_, _vertex_count * 3 * sizeof(float));
	}

	edge_count_ = geometry->edge_count_;
	if (geometry->edge_data_ != 0) {
		// Copy edge data.
		edge_data_ = new Edge[edge_count_];
		for (unsigned i = 0; i < edge_count_; i++) {
			edge_data_[i].Copy(&geometry->edge_data_[i]);
		}
	}

	tangents_uv_set_ = geometry->tangents_uv_set_;
	if (geometry->tangent_data_ != 0 && geometry->bitangent_data_) {
		unsigned _vertex_count = geometry->GetVertexCount();
		tangent_data_ = new float[_vertex_count * 3];
		bitangent_data_ = new float[_vertex_count * 3];
		::memcpy(tangent_data_, geometry->tangent_data_, _vertex_count * 3 * sizeof(float));
		::memcpy(bitangent_data_, geometry->bitangent_data_, _vertex_count * 3 * sizeof(float));
	}

	renderer_data_ = geometry->renderer_data_;
	parent_cell_ = geometry->parent_cell_;
	last_frame_visible_ = geometry->last_frame_visible_;
	transformation_ = geometry->transformation_;
	uv_animator_ = geometry->uv_animator_;
}

bool GeometryBase::IsSolidVolume() {
	if (CheckFlag(kSolidVolumeValid) == false) {
		if (edge_data_ == 0) {
			GenerateEdgeData();
		}

		SetFlag(kIsSolidVolume);

		for (unsigned i = 0; i < edge_count_; i++) {
			if (edge_data_[i].triangle_count_ != 2) {
				ClearFlag(kIsSolidVolume);
				break;
			}
		}

		SetFlag(kSolidVolumeValid);
	}

	return CheckFlag(kIsSolidVolume);
}

bool GeometryBase::IsSingleObject() {
	if (CheckFlag(kSingleObjectValid) == true) {
		return CheckFlag(kIsSingleObject);
	}

	unsigned _triangle_count = GetTriangleCount();
	vtx_idx_t* _indices   = GetIndexData();

	if (_indices == 0 || _triangle_count == 0) {
		return false;
	};

	if (edge_data_ == 0) {
		GenerateEdgeData();
	}

	// Triangle-to-edge lookup.
	int* edge_indices = new int[_triangle_count * 3];

	// Need this to fill the edge_indices correctly.
	int* edge_count = new int[_triangle_count];

	unsigned i;
	for (i = 0; i < _triangle_count; i++) {
		edge_count[i] = 0;
	}

	// Setup triangle-to-edge lookup.
	for (i = 0; i < edge_count_; i++) {
		Edge* _edge = &edge_data_[i];

		for (int j = 0; j < _edge->triangle_count_; j++) {
			int t = _edge->triangle_[j];
			int _triangle_index = t * 3;
			int triangle_edge_count = edge_count[t]++;
			edge_indices[_triangle_index + triangle_edge_count] = i;
		}
	}

	// One flag per triangle... If this is a single object, all flags
	// will be set to true.
	bool* triangle_checked = new bool[_triangle_count];
	int* edge_to_check = new int[_triangle_count];
	for (i = 0; i < _triangle_count; i++) {
		triangle_checked[i] = false;
		edge_to_check[i] = 0;
	}

	// This is a flood fill. Starting from triangle 0, we set its flag to true,
	// and continue doing the same procedure with its neighbour triangles.
	// If we can fill all triangles (setting all flags to 'true'),
	// they are all connected, which means that this is all one object.
	int* triangle_stack = new int[_triangle_count];
	int stack_index = 0;
	triangle_stack[0] = 0;

	int triangles_left = _triangle_count;
	int current_triangle;

	do {
		current_triangle = triangle_stack[stack_index];

		if (triangle_checked[current_triangle] == false) {
			triangle_checked[current_triangle] = true;
			triangles_left--;
		}

		if (edge_to_check[current_triangle] >= 3) {
			// All edges are already checked, let's fall back to the previous
			// triangle.
			stack_index--;
		} else {
			// Get the edge.
			int edge_index = edge_indices[current_triangle * 3 + edge_to_check[current_triangle]];
			edge_to_check[current_triangle]++;
			Edge* _edge = &edge_data_[edge_index];

			for (i = 0; i < (unsigned int)_edge->triangle_count_; i++) {
				int _triangle = _edge->triangle_[i];
				// Check if this isn't the current triangle.
				if (_triangle != current_triangle &&
				   triangle_checked[_triangle] == false) {
					// It's a neighbour triangle and it's not checked. Put it on the stack.
					triangle_stack[++stack_index] = _triangle;
				}
			}
		}
	}while(stack_index >= 0);	// Continue until we get back to start.

	SetFlag(kIsSingleObject);

	// Check if there is any flag that is still set to false.
	if (triangles_left > 0) {
		ClearFlag(kIsSingleObject);
	}

	delete[] triangle_checked;
	delete[] edge_indices;
	delete[] edge_count;
	delete[] edge_to_check;
	delete[] triangle_stack;

	SetFlag(kSingleObjectValid);
	return CheckFlag(kIsSingleObject);
}

bool GeometryBase::IsConvexVolume() {
	if (CheckFlag(kConvexVolumeValid) == true) {
		return CheckFlag(kIsConvexVolume);
	}

	unsigned _triangle_count = GetTriangleCount();
	float* vertex_data = GetVertexData();

	if (_triangle_count == 0) {
		return false;
	};

	if (edge_data_ == 0) {
		GenerateEdgeData();
	}

	if (IsSolidVolume() == false) {
		ClearFlag(kIsConvexVolume);
		SetFlag(kConvexVolumeValid);
		return false;
	}

	// Since we got here, IsSolidVolume() returned true. And it will only return
	// true if all edges have exactly 2 triangles connected. Assume convex.
	SetFlag(kIsConvexVolume);

	bool clear_surface_normals = (surface_normal_data_ == 0);
	GenerateSurfaceNormalData();

	const float epsilon = 1e-6f;
	for (unsigned i = 0; i < edge_count_; i++) {
		int t0 = edge_data_[i].triangle_[0];
		int t1 = edge_data_[i].triangle_[1];
		int t0_index = t0 * 3;
		//int lT1Index = t1 * 3;

		vec3 t0_normal(surface_normal_data_[t0_index + 0],
					    surface_normal_data_[t0_index + 1],
					    surface_normal_data_[t0_index + 2]);

		uint32 t0_tri_index[4];
		uint32 t1_tri_index[4];
		GetTriangleIndices(t0, t0_tri_index);
		GetTriangleIndices(t1, t1_tri_index);

		// Get an edge, no matter which one, but not the current edge.
		// t1_v0 must be the same vertex as one of the edge's, and
		// t1_v1 mustn't be the other.
		int t1_v0 = edge_data_[i].vertex_[0];
		int t1_v1 = t1_tri_index[0];

		if (t1_v1 == edge_data_[i].vertex_[0] ||
		   t1_v1 == edge_data_[i].vertex_[1]) {
			t1_v1 = t1_tri_index[1];
		}
		if (t1_v1 == edge_data_[i].vertex_[0] ||
		   t1_v1 == edge_data_[i].vertex_[1]) {
			t1_v1 = t1_tri_index[2];
		}

		t1_v0 *= 3;
		t1_v1 *= 3;

		vec3 t1_edge(vertex_data[t1_v1 + 0] - vertex_data[t1_v0 + 0],
					  vertex_data[t1_v1 + 1] - vertex_data[t1_v0 + 1],
					  vertex_data[t1_v1 + 2] - vertex_data[t1_v0 + 2]);
		t1_edge.Normalize();

		float dot = t1_edge.Dot(t0_normal);

		if (dot > epsilon) {
			// We found a concave angle.
			ClearFlag(kIsConvexVolume);
			break;
		}
	}

	if (clear_surface_normals == true) {
		GeometryBase::ClearSurfaceNormalData();
	}

	SetFlag(kConvexVolumeValid);

	return CheckFlag(kIsConvexVolume);
}

bool GeometryBase::IsTwoSided() const {
	return CheckFlag(kIsTwoSided);
}

void GeometryBase::SetTwoSided(bool is_two_sided) {
	SetFlag(kIsTwoSided, is_two_sided);
}

bool GeometryBase::IsRecvNoShadows() const {
	return CheckFlag(kRecvNoShadows);
}

void GeometryBase::SetRecvNoShadows(bool recv_no_shadows) {
	SetFlag(kRecvNoShadows, recv_no_shadows);
}

void GeometryBase::CalculateBoundingRadius() {
	float* vertex_data = GetVertexData();
	int _vertex_count = GetVertexCount();

	float max_distance = 0.0f;
	for (int i = 0; i < _vertex_count; i++) {
		int index = i * 3;
		float distance = vertex_data[index + 0] * vertex_data[index + 0] +
				   vertex_data[index + 1] * vertex_data[index + 1] +
				   vertex_data[index + 2] * vertex_data[index + 2];

		if (distance > max_distance)
			max_distance = distance;
	}

	bounding_radius_ = sqrtf(max_distance);

	SetFlag(kBoundingRadiusValid);
}

void GeometryBase::SetSurfaceNormalData(const float* surface_normal_data) {
	unsigned _triangle_count = GetTriangleCount();
	AllocSurfaceNormalData();
	::memcpy(surface_normal_data_, surface_normal_data, _triangle_count * 3 * sizeof(float));
	SetFlag(kSurfaceNormalsValid);
}

void GeometryBase::SetVertexNormalData(const float* vertex_normal_data, unsigned vertex_count) {
	if (CheckFlag(kVertexNormalsValid) == false) {
		ClearVertexNormalData();
		AllocVertexNormalData();
	}

	::memcpy(vertex_normal_data_, vertex_normal_data, vertex_count * 3 * sizeof(float));

	SetFlag(kVertexNormalsValid);
}

void GeometryBase::SetTangentAndBitangentData(const float* tangent_data, const float* bitangent_data, unsigned vertex_count) {
	if (CheckFlag(kTangentsValid) == false) {
		ClearTangentAndBitangentData();
		tangent_data_   = new float[GetMaxVertexCount() * 3];
		bitangent_data_ = new float[GetMaxVertexCount() * 3];
	}

	::memcpy(tangent_data_, tangent_data, vertex_count * 3 * sizeof(float));
	::memcpy(bitangent_data_, bitangent_data, vertex_count * 3 * sizeof(float));

	SetFlag(kTangentsValid);
}

void GeometryBase::SetIndexData(vtx_idx_t* index_data, unsigned index_count, unsigned max_index_count) {
	log_.Warningf("Unable to set index data on mesh %s.", name_.c_str());
	delete index_data;
	(void)index_count;
	(void)max_index_count;
}

void GeometryBase::GenerateVertexNormalData() {
	if (CheckFlag(kVertexNormalsValid) == true || GetIndexData() == 0) {
		return;
	}

	unsigned _vertex_count  = GetMaxVertexCount();
	unsigned _triangle_count = GetTriangleCount();
	if (_vertex_count == 0 || _triangle_count == 0) {
		return;
	}

	AllocVertexNormalData();
	::memset(vertex_normal_data_, 0, _vertex_count * 3 * sizeof(float));

	bool clear_surface_normals = (surface_normal_data_ == 0);
	GenerateSurfaceNormalData();

	unsigned i;
	unsigned index = 0;

	// Now calculate the vertex normals.
	for (i = 0; i < _triangle_count; i++, index += 3) {
		uint32 t[4];
		GetTriangleIndices(i, t);
		int i0 = t[0] * 3;
		int i1 = t[1] * 3;
		int i2 = t[2] * 3;

		vertex_normal_data_[i0 + 0] += surface_normal_data_[index + 0];
		vertex_normal_data_[i0 + 1] += surface_normal_data_[index + 1];
		vertex_normal_data_[i0 + 2] += surface_normal_data_[index + 2];

		vertex_normal_data_[i1 + 0] += surface_normal_data_[index + 0];
		vertex_normal_data_[i1 + 1] += surface_normal_data_[index + 1];
		vertex_normal_data_[i1 + 2] += surface_normal_data_[index + 2];

		vertex_normal_data_[i2 + 0] += surface_normal_data_[index + 0];
		vertex_normal_data_[i2 + 1] += surface_normal_data_[index + 1];
		vertex_normal_data_[i2 + 2] += surface_normal_data_[index + 2];
	}

	// Normalize the normals.
	const float epsilon = 1e-6f;
	index = 0;
	for (i = 0; i < _vertex_count; i++, index += 3) {
		float dx = vertex_normal_data_[index + 0];
		float dy = vertex_normal_data_[index + 1];
		float dz = vertex_normal_data_[index + 2];
		float length = (float)sqrt(dx * dx + dy * dy+ dz * dz);

		if (length > epsilon) {
			float one_over_length = 1.0f / length;
			vertex_normal_data_[index + 0] *= one_over_length;
			vertex_normal_data_[index + 1] *= one_over_length;
			vertex_normal_data_[index + 2] *= one_over_length;
		}
	}

	if (clear_surface_normals == true) {
		GeometryBase::ClearSurfaceNormalData();
	}

	SetFlag(kVertexNormalsValid);
}

void GeometryBase::GenerateSurfaceNormalData() {
	if (CheckFlag(kSurfaceNormalsValid) == true) {
		return;
	}

	unsigned _triangle_count = GetTriangleCount();

	if (surface_normal_data_ != 0 && surface_normal_count_ < _triangle_count) {
		ClearSurfaceNormalData();
	}

	AllocSurfaceNormalData();

	float* vertex_data = GetVertexData();

	int index = 0;
	for (unsigned i = 0; i < _triangle_count; i++, index += 3) {
		uint32 t[4];
		GetTriangleIndices(i, t);

		int i0 = t[0] * 3;
		int i1 = t[1] * 3;
		int i2 = t[2] * 3;

		vec3 v0(vertex_data[i1 + 0] - vertex_data[i0 + 0],
				      vertex_data[i1 + 1] - vertex_data[i0 + 1],
				      vertex_data[i1 + 2] - vertex_data[i0 + 2]);
		vec3 v1(vertex_data[i2 + 0] - vertex_data[i0 + 0],
				      vertex_data[i2 + 1] - vertex_data[i0 + 1],
				      vertex_data[i2 + 2] - vertex_data[i0 + 2]);
		vec3 c;
		c.CrossUnit(v0, v1);

		surface_normal_data_[index + 0] = c.x;
		surface_normal_data_[index + 1] = c.y;
		surface_normal_data_[index + 2] = c.z;
	}

	SetFlag(kSurfaceNormalsValid);
}

void GeometryBase::GenerateEdgeData() {
	if (GetVertexCount() == 0 ||
	   GetTriangleCount() == 0) {
		return;
	}

	unsigned _vertex_count  = GetVertexCount();
	unsigned _triangle_count = GetTriangleCount();

	ClearEdgeData();

	// Create buckets to store the edges in to optimize search.
	// It's almost like a hash table, with as many buckets as there
	// are vertex indices.
	std::vector<Edge*> edge_bucket(_vertex_count);
	::memset(&edge_bucket[0], 0, _vertex_count*sizeof(Edge*));
	std::vector<Edge> edge_pool(_triangle_count*3);	// For avoiding repeated allocations.
	size_t edge_pool_index = 0;

	// Create edges.
	for (unsigned i = 0; i < _triangle_count; i++) {
		// Get vertex indices.
		uint32 v[4];
		GetTriangleIndices(i, v);

		// Note:
		// Edge0 = (v[2], v[0])
		// Edge1 = (v[0], v[1])
		// Edge2 = (v[1], v[2])

		int start = 2;
		int __end = 0;
		while (__end < 3) {
			unsigned v0 = v[start];
			unsigned v1 = v[__end];
			if (v0 > v1) {	// Sorting.
				std::swap(v0, v1);
			}

			// Now check if the edge (v0, v1) is already in the table.
			if (!edge_bucket[v0]) {
				// This bucket was empty, create a new edge.
				edge_bucket[v0] = &edge_pool[edge_pool_index++];
				edge_bucket[v0]->vertex_[0] = v0;
				edge_bucket[v0]->vertex_[1] = v1;
				edge_bucket[v0]->AddTriangle(i);

				edge_count_++;
			} else {
				// This bucket contains some edges, let's search for (v0, v1).
				bool found = false;
				Edge* previous_edge = 0;
				Edge* current_edge = edge_bucket[v0];
				while (current_edge && !found) {
					previous_edge = current_edge;
					current_edge = current_edge->reserved_;
					found = previous_edge->IsSameEdge(v0, v1);
				}
				if (found) {
					// We found the edge. Since there can only be two triangles
					// sharing one edge, the second triangle index must be invalid.
					// Set it now.
					previous_edge->AddTriangle(i);
				} else {
					// Create new and put in linked list.
					Edge& _edge = edge_pool[edge_pool_index++];
					_edge.vertex_[0] = v0;
					_edge.vertex_[1] = v1;
					_edge.AddTriangle(i);
					previous_edge->reserved_ = &_edge;

					edge_count_++;
				}
			}

			start = __end;
			__end++;
		} // End while(lEnd < 3)
	}

	// Create the actual edge array and copy the data.
	// Delete all temporarily created edges.
	edge_data_ = new Edge[edge_count_];
	int edge_index = 0;
	for (unsigned i = 0; i < _vertex_count; i++) {
		Edge* current_edge = edge_bucket[i];
		while (current_edge) {
			edge_data_[edge_index++].Copy(current_edge);
			current_edge = current_edge->reserved_;
		}
	}

	ClearFlag(kSolidVolumeValid);
	ClearFlag(kSingleObjectValid);
	ClearFlag(kConvexVolumeValid);
}

void GeometryBase::GenerateTangentAndBitangentData() {
	float* vertex_data = GetVertexData();
	float* uv_data = GetUVData(tangents_uv_set_);

	if (vertex_data == 0 || uv_data == 0)
		return;

	int _vertex_count = GetMaxVertexCount();

	AllocTangentAndBitangentData();
	::memset(tangent_data_, 0, sizeof(float) * _vertex_count * 3);
	::memset(bitangent_data_, 0, sizeof(float) * _vertex_count * 3);

	int _triangle_count = GetTriangleCount();
	int i;

	for (i = 0; i < _triangle_count; i++) {
		uint32 tri_index[4];
		GetTriangleIndices(i, tri_index);

		int v1  = tri_index[0] * 3;
		int v2  = tri_index[1] * 3;
		int v3  = tri_index[2] * 3;
		int u_v1 = tri_index[0] * 2;
		int u_v2 = tri_index[1] * 2;
		int u_v3 = tri_index[2] * 2;

		vec3 edge1(vertex_data[v2 + 0] - vertex_data[v1 + 0],
					 vertex_data[v2 + 1] - vertex_data[v1 + 1],
					 vertex_data[v2 + 2] - vertex_data[v1 + 2]);
		vec3 edge2(vertex_data[v3 + 0] - vertex_data[v1 + 0],
					 vertex_data[v3 + 1] - vertex_data[v1 + 1],
					 vertex_data[v3 + 2] - vertex_data[v1 + 2]);

		vec2 edge1_uv(uv_data[u_v2 + 0] - uv_data[u_v1 + 0],
					   uv_data[u_v2 + 1] - uv_data[u_v1 + 1]);
		vec2 edge2_uv(uv_data[u_v3 + 0] - uv_data[u_v1 + 0],
					   uv_data[u_v3 + 1] - uv_data[u_v1 + 1]);

		float cp = edge1_uv.y * edge2_uv.x - edge1_uv.x * edge2_uv.y;
		//float cp = edge1_uv.x * edge2_uv.y - edge2_uv.x * edge1_uv.y;

		if (cp != 0.0f) {
			float cp_recip = 1.0f / cp;

			vec3 tangent((edge1 * -edge2_uv.y + edge2 * edge1_uv.y) * cp_recip);
			vec3 bitangent((edge1 * -edge2_uv.x + edge2 * edge1_uv.x) * cp_recip);
			//vec3 tangent((edge2_uv.y * edge1 - edge1_uv.y * edge2) * cp_recip);
			//vec3 bitangent((edge1_uv.x * edge2 - edge2_uv.x * edge1) * cp_recip);

			tangent.Normalize();
			bitangent.Normalize();

			// Add the tangent to all three vertices.
			tangent_data_[v1 + 0] += tangent.x;
			tangent_data_[v1 + 1] += tangent.y;
			tangent_data_[v1 + 2] += tangent.z;
			tangent_data_[v2 + 0] += tangent.x;
			tangent_data_[v2 + 1] += tangent.y;
			tangent_data_[v2 + 2] += tangent.z;
			tangent_data_[v3 + 0] += tangent.x;
			tangent_data_[v3 + 1] += tangent.y;
			tangent_data_[v3 + 2] += tangent.z;

			// Add the bitangent to all three vertices.
			bitangent_data_[v1 + 0] += bitangent.x;
			bitangent_data_[v1 + 1] += bitangent.y;
			bitangent_data_[v1 + 2] += bitangent.z;
			bitangent_data_[v2 + 0] += bitangent.x;
			bitangent_data_[v2 + 1] += bitangent.y;
			bitangent_data_[v2 + 2] += bitangent.z;
			bitangent_data_[v3 + 0] += bitangent.x;
			bitangent_data_[v3 + 1] += bitangent.y;
			bitangent_data_[v3 + 2] += bitangent.z;
		}
	}

	// Normalize all vectors.
	_vertex_count = GetVertexCount();
	int v_index = 0;
	for (i = 0; i < _vertex_count; i++, v_index += 3) {
		float x = tangent_data_[v_index + 0];
		float y = tangent_data_[v_index + 1];
		float z = tangent_data_[v_index + 2];
		float length = x*x + y*y + z*z;
		if (length != 0) {
			float rsq = (float)(1.0 / sqrt(length));
			tangent_data_[v_index + 0] *= rsq;
			tangent_data_[v_index + 1] *= rsq;
			tangent_data_[v_index + 2] *= rsq;
		}

		x = bitangent_data_[v_index + 0];
		y = bitangent_data_[v_index + 1];
		z = bitangent_data_[v_index + 2];
		length = x*x + y*y + z*z;
		if (length != 0) {
			float rsq = (float)(1.0 / sqrt(length));
			bitangent_data_[v_index + 0] *= rsq;
			bitangent_data_[v_index + 1] *= rsq;
			bitangent_data_[v_index + 2] *= rsq;
		}
	}
}

bool GeometryBase::VerifyIndexData() {
	vtx_idx_t* index = GetIndexData();
	unsigned num_index = GetTriangleCount() * 3;
	unsigned num_vertex = GetVertexCount();

	bool ok = true;
	for (unsigned i = 0; ok && i < num_index; i++) {
		ok = ((unsigned int)index[i] < num_vertex);
	}

	return ok;
}

const xform& GeometryBase::GetUVTransform() const {
	if(uv_animator_ != 0) {
		return uv_animator_->GetBones()[0].GetRelativeBoneTransformation(0);
	} else {
		return kIdentityTransformationF;
	}
}



bool GeometryBase::GetVertexDataChanged() const {
	return CheckFlag(kVertexDataChanged);
}

bool GeometryBase::GetUVDataChanged() const {
	return CheckFlag(kUvDataChanged);
}

bool GeometryBase::GetColorDataChanged() const {
	return CheckFlag(kColorDataChanged);
}

bool GeometryBase::GetIndexDataChanged() const {
	return CheckFlag(kIndexDataChanged);
}

void GeometryBase::SetVertexNormalsValid() {
	SetFlag(kVertexNormalsValid);
}

void GeometryBase::SetSurfaceNormalsValid() {
	SetFlag(kSurfaceNormalsValid);
}

GeometryBase::ColorFormat GeometryBase::GetColorFormat() const {
	return kColorRgb;
}

bool GeometryBase::GetTransformationChanged() const {
	return CheckFlag(kTransformationChanged);
}

void GeometryBase::SetTransformationChanged(bool transformation_changed) {
	transformation_changed? SetFlag(kTransformationChanged|kRefTransformationChanged) : ClearFlag(kTransformationChanged);
}

bool GeometryBase::GetBigOrientationChanged() const {
	return CheckFlag(kBigOrientationChanged);
}

void GeometryBase::SetBigOrientationChanged(bool orientation_changed) {
	SetFlag(kBigOrientationChanged, orientation_changed);
}

void GeometryBase::SetAlwaysVisible(bool always_visible) {
	SetFlag(kAlwaysVisible, always_visible);
}

bool GeometryBase::GetAlwaysVisible() const {
	return CheckFlag(kAlwaysVisible);
}

void GeometryBase::SetSolidVolumeCheckValid(bool valid) {
	SetFlag(kSolidVolumeValid, valid);
}

void GeometryBase::SetSingleObjectCheckValid(bool valid) {
	SetFlag(kSingleObjectValid, valid);
}

void GeometryBase::SetConvexVolumeCheckValid(bool valid) {
	SetFlag(kConvexVolumeValid, valid);
}

void GeometryBase::SetFlag(lepra::uint32 pFlag, bool value) {
	if (value) SetFlag(pFlag); else ClearFlag(pFlag);
}

void GeometryBase::SetFlag(lepra::uint32 pFlag) {
	flags_ |= pFlag;
}

void GeometryBase::ClearFlag(lepra::uint32 pFlag) {
	flags_ &= (~pFlag);
}

bool GeometryBase::CheckFlag(lepra::uint32 pFlag) const {
	return (flags_ & pFlag) != 0;
}

uint32 GeometryBase::GetFlags() const {
	return flags_;
}

void GeometryBase::SetDefaultFlags(lepra::uint32 flags) {
	default_flags_ = flags;
}



GeometryBase::BasicMaterialSettings::BasicMaterialSettings():
	ambient_(0,0,0),
	diffuse_(1,0,1),
	specular_(0,0,0),
	shininess_(0),
	alpha_(1),
	smooth_(true) {
}

GeometryBase::BasicMaterialSettings::BasicMaterialSettings(const vec3& ambient, const vec3& diffuse,
	const vec3& specular, float shininess,
	float alpha, bool smooth):
	ambient_(ambient),
	diffuse_(diffuse),
	specular_(specular),
	shininess_(shininess),
	alpha_(alpha),
	smooth_(smooth) {
}

void GeometryBase::BasicMaterialSettings::SetColor(float red, float green, float blue) {
	diffuse_.Set(red, green, blue);
}

void GeometryBase::BasicMaterialSettings::Set(const vec3& ambient, const vec3& diffuse,
	const vec3& specular, float shininess,
	float alpha, bool smooth) {
	ambient_	= ambient;
	diffuse_	= diffuse;
	specular_	= specular;
	shininess_	= shininess;
	alpha_		= alpha;
	smooth_		= smooth;
}



lepra::uint32 GeometryBase::default_flags_ = 0;
float GeometryBase::default_big_orientation_threshold_ = 1e-3f;
loginstance(kUiGfx, GeometryBase);



}
