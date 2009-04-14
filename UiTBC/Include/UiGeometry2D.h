/*
	Class:  Geometry2D
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand

	NOTES:

	This class was implemented in order to optimize 2D rendering using hardware 
	accelerated Painters (see Painter.h). While OpenGLPainter and DirectXPainter
	benefits from this class and can perform faster, the opposite unfortunately 
	applies to SoftwarePainter.

	Use this class with care! This class is implemented with performance in mind,
	not safety.
*/

#include "../../Lepra/Include/LepraTypes.h"
#include "../../Lepra/Include/Vector2D.h"
#include "UiTBC.h"

namespace UiTbc
{
class Geometry2D
{
public:
	// Used to define what data that will be allocated for each vertex.
	// X and Y coordinates are mandatory, so they are not available as
	// an option. These flags can be combined using the | operator.
	// If VTX_INTERLEAVED is set, the data will be stored for Direct3D-
	// compatibility (colors as 32-bit BGRA integers (in little endian)).
	enum //VertexFormat
	{
		VTX_INTERLEAVED = (1 << 0), // Opposite is separate buffers.
		VTX_UV          = (1 << 1), // Texture coordinates.
		VTX_RGB         = (1 << 2), // Vertex colors.
	};

	// Structs used for the interleaved format. Direct3D compatible.
	struct VertexXY
	{
		float x;
		float y;
		float z; // = 0
		float w; // = 1
	};

	struct VertexXYRGB
	{
		float x;
		float y;
		float z; // = 0
		float w; // = 1
		Lepra::uint8 mBlue;
		Lepra::uint8 mGreen;
		Lepra::uint8 mRed;
		Lepra::uint8 mAlpha;
	};

	struct VertexXYUV
	{
		float x;
		float y;
		float z; // = 0
		float w; // = 1
		float u;
		float v;
	};

	struct VertexXYUVRGB
	{
		float x;
		float y;
		float z; // = 0
		float w; // = 1
		Lepra::uint8 mBlue;
		Lepra::uint8 mGreen;
		Lepra::uint8 mRed;
		Lepra::uint8 mAlpha;
		float u;
		float v;
	};

	Geometry2D(Lepra::uint16 pVertexFormat = VTX_RGB,
		int pVertexCapacity = 16, int pTriangleCapacity = 8);
	~Geometry2D();

	inline Lepra::uint16 GetVertexFormat() const { return mVertexFormat; }

	// Will allocate exactly the amount of space specified by the parameter.
	void ReallocVertexBuffers(int pVertexCapacity);
	void ReallocTriangleBuffer(int pTriangleCapacity);

	void AssureVertexCapacity(int pVertexCapacity);
	void AssureTriangleCapacity(int pTriangleCapacity);

	// The first time these functions are called, they will add a vertex and
	// increase the vertex count with 1. After calling ResetIndices, the 
	// vertex count will remain constant, but the "current vertex"-index will
	// increase by each call. Make sure to use the function that matches the
	// declared vertex format.
	Lepra::uint32 SetVertex(float x, float y);
	Lepra::uint32 SetVertex(float x, float y, float u, float v);
	Lepra::uint32 SetVertex(float x, float y, float r, float g, float b);
	Lepra::uint32 SetVertex(float x, float y, float u, float v, float r, float g, float b);
	void SetTriangle(Lepra::uint32 pV1, Lepra::uint32 pV2, Lepra::uint32 pV3);

	// Sets the VertexCount and the TriangleCount to zero.
	void Reset();

	// Returns valid buffer pointers to buffers specified by the vertex format.
	// If VTX_INTERLEAVED is set, the pointer from GetVertexData() should be
	// casted to either a void or a byte pointer, float pointer otherwise.
	inline void* GetVertexData() const { return mVertexData; }
	inline float* GetColorData() const { return mColorData; }
	inline float* GetUVData() const { return mUVData; }
	inline Lepra::uint32* GetTriangleData() const { return mTriangleData; }
	inline int GetVertexCapacity() const { return mVertexCapacity; }
	inline int GetTriangleCapacity() const { return mTriangleCapacity; }
	inline int GetVertexCount() const { return mVertexCount; }
	inline int GetTriangleCount() const { return mTriangleCount; }

	inline const Lepra::Vector2D<float>& GetPos() const { return mPos; }
	inline void SetPos(const Lepra::Vector2D<float>& pPos) { mPos = pPos; }

	inline bool IsFlagSet(Lepra::uint16 pFlag) { return ((mVertexFormat & pFlag) != 0); }

private:

	void Realloc(void** pData, size_t pNewSize, size_t pBytesToCopy);

	Lepra::uint16 mVertexFormat;

	void* mVertexData;
	float* mColorData;
	float* mUVData;
	Lepra::uint32* mTriangleData;
	int mVertexCapacity;
	int mTriangleCapacity;
	int mVertexCount;
	int mTriangleCount;

	Lepra::Vector2D<float> mPos;
};

} // End namespace.
