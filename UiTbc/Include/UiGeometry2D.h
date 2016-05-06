
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine
//
// NOTES:
//
// This class was implemented in order to optimize 2D rendering using hardware
// accelerated Painters (see Painter.h).
//
// Use this class with care! This class is implemented with performance in mind,
// not safety.



#include "../../lepra/include/lepratypes.h"
#include "../../lepra/include/vector2d.h"
#include "uitbc.h"



namespace uitbc {



class Geometry2D {
public:
	// Used to define what data that will be allocated for each vertex.
	// X and Y coordinates are mandatory, so they are not available as
	// an option. These flags can be combined using the | operator.
	// If kVtxInterleaved is set, the data will be stored for Direct3D-
	// compatibility (colors as 32-bit BGRA integers (in little endian)).
	enum { //VertexFormat
		kVtxInterleaved = (1 << 0), // Opposite is separate buffers.
		kVtxUv          = (1 << 1), // Texture coordinates.
		kVtxRgb         = (1 << 2), // Vertex colors.
		kVtxIndex16     = (1 << 3), // Indices are 16-bit.
	};

	// Structs used for the interleaved format. Direct3D compatible.
	struct VertexXY {
		float x;
		float y;
		float z; // = 0
		float w; // = 1
	};

	struct VertexXYRGB {
		float x;
		float y;
		float z; // = 0
		float w; // = 1
		lepra::uint8 blue_;
		lepra::uint8 green_;
		lepra::uint8 red_;
		lepra::uint8 alpha_;
	};

	struct VertexXYUV {
		float x;
		float y;
		float z; // = 0
		float w; // = 1
		float u;
		float v;
	};

	struct VertexXYUVRGB {
		float x;
		float y;
		float z; // = 0
		float w; // = 1
		lepra::uint8 blue_;
		lepra::uint8 green_;
		lepra::uint8 red_;
		lepra::uint8 alpha_;
		float u;
		float v;
	};

	Geometry2D(unsigned vertex_format = kVtxRgb,
		int vertex_capacity = 16, int triangle_capacity = 8);
	~Geometry2D();
	void Init(unsigned vertex_format = kVtxRgb,
		int vertex_capacity = 16, int triangle_capacity = 8);

	inline unsigned GetVertexFormat() const { return vertex_format_; }

	// Will allocate exactly the amount of space specified by the parameter.
	void ReallocVertexBuffers(int vertex_capacity);
	void ReallocTriangleBuffer(int triangle_capacity);

	void AssureVertexCapacity(int vertex_capacity);
	void AssureTriangleCapacity(int triangle_capacity);

	// The first time these functions are called, they will add a vertex and
	// increase the vertex count with 1. After calling ResetIndices, the
	// vertex count will remain constant, but the "current vertex"-index will
	// increase by each call. Make sure to use the function that matches the
	// declared vertex format.
	lepra::uint32 SetVertex(float x, float y);
	lepra::uint32 SetVertex(float x, float y, float u, float v);
	lepra::uint32 SetVertex(float x, float y, float r, float g, float b);
	lepra::uint32 SetVertex(float x, float y, float u, float v, float r, float g, float b);
	void SetTriangle(lepra::uint32 v1, lepra::uint32 v2, lepra::uint32 v3);

	// Sets the VertexCount and the TriangleCount to zero.
	void Reset();

	// Returns valid buffer pointers to buffers specified by the vertex format.
	// If kVtxInterleaved is set, the pointer from GetVertexData() should be
	// casted to either a void or a byte pointer, float pointer otherwise.
	inline const void* GetVertexData() const { return vertex_data_; }
	inline const float* GetColorData() const { return color_data_; }
	inline const float* GetUVData() const { return uv_data_; }
	inline const lepra::uint16* GetTriangleData16() const { return triangle_data16_; }
	inline const lepra::uint32* GetTriangleData32() const { return triangle_data32_; }
	inline int GetVertexCapacity() const { return vertex_capacity_; }
	inline int GetTriangleCapacity() const { return triangle_capacity_; }
	inline int GetVertexCount() const { return vertex_count_; }
	inline int GetTriangleCount() const { return triangle_count_; }

	inline const vec2& GetPos() const { return pos_; }
	inline void SetPos(const vec2& pos) { pos_ = pos; }

	inline bool IsFlagSet(unsigned pFlag) { return ((vertex_format_ & pFlag) != 0); }

private:
	void Realloc(void** data, size_t new_size, size_t bytes_to_copy);

	Geometry2D(const Geometry2D&);
	void operator=(const Geometry2D&);

	unsigned vertex_format_;

	void* vertex_data_;
	float* color_data_;
	float* uv_data_;
	union {
		lepra::uint16* triangle_data16_;
		lepra::uint32* triangle_data32_;
	};
	int vertex_capacity_;
	int triangle_capacity_;
	int vertex_count_;
	int triangle_count_;

	Vector2D<float> pos_;
};



}
