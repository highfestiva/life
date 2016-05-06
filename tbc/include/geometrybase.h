
// Author: Jonas Byström
// Copyright (c) 2002-, Pixel Doctrine



#pragma once

#include <math.h>
#include <list>
#include "../../thirdparty/FastDelegate/FastDelegate.h"
#include "../../lepra/include/string.h"
#include "../../lepra/include/vector3d.h"
#include "../../lepra/include/transformation.h"
#include "tbc.h"
#include "portalmanager.h"




namespace tbc {



class BoneAnimator;



class GeometryBase {
public:
	// uitbc::Renderer implements this interface and removes the geometry
	// if it is deleted.
	class Listener {
	public:
		virtual void DeletingGeometry(GeometryBase* geometry) = 0;
	};

	typedef fastdelegate::FastDelegate0<bool> PreRenderCallback;
	typedef fastdelegate::FastDelegate0<void> PostRenderCallback;

	friend class BasicMeshCreator;
	friend class PortalManager;

	class Edge {
	public:
		enum {
			kInvalidIndex = -1,
		};

		Edge();
		Edge(const Edge& edge);
		Edge(const Edge* edge);
		virtual ~Edge();

		void ClearAll();

		void Copy(const Edge* edge);

		void AddTriangle(int triangle_index);
		void RemoveTriangle(int triangle_index);

		bool HaveTriangle(int triangle_index);
		bool HaveTriangle(int triangle_index, int& triangle_pos);

		bool IsSameEdge(int vertex_index1, int vertex_index2);
		bool IsSameEdge(int vertex_index1,
				int vertex_index2,
				int* triangles,
				int num_triangles);

		//int* triangle_;
		int triangle_[2];
		int vertex_[2];
		int triangle_count_;
		//int mTriangleElementCount;

		Edge* reserved_;
	};

	// Buffer object usage hints. These will only have effect
	// when rendering the geometry using hardware acceleration
	// APIs. If using a software renderer these won't make any
	// difference.
	enum GeometryVolatility {
		kGeomStatic = 1,	// Geometry that will never change.
		kGeomSemiStatic,	// Geometry that will never change.
		kGeomDynamic,		// Geometry that will change occasionally (like terrain).
		kGeomVolatile,		// Geometry that will change between every frame.
	};

	// Vertex color format.
	enum ColorFormat {
		kColorRgb = 1,
		kColorRgba,
	};

	enum PrimitiveType {
		kTriangles = 1,
		kTriangleStrip,
		kLines,
		kLineLoop,
		kQuads,
	};

	class BasicMaterialSettings {
	public:
		BasicMaterialSettings();
		BasicMaterialSettings(const vec3& ambient, const vec3& diffuse,
			const vec3& specular, float shininess,
			float alpha, bool smooth);
		void SetColor(float red, float green, float blue);
		void Set(const vec3& ambient, const vec3& diffuse,
			const vec3& specular, float shininess,
			float alpha, bool smooth);

		vec3 ambient_;
		vec3 diffuse_;
		vec3 specular_;
		float shininess_;	// Specular "exponent".
		float alpha_;	// Used on blended (and transparent? materials.
		bool smooth_;	// Smooth shaded or flat shaded?
	};

	GeometryBase();
	virtual ~GeometryBase();

	void AddListener(Listener* listener);
	void RemoveListener(Listener* listener);
	const PreRenderCallback& GetPreRenderCallback() const;
	void SetPreRenderCallback(const PreRenderCallback& callback);
	const PostRenderCallback& GetPostRenderCallback() const;
	void SetPostRenderCallback(const PostRenderCallback& callback);

	virtual bool IsGeometryReference();
	void SetExcludeCulling();
	bool IsExcludeCulling() const;
	float GetScale() const;
	void SetScale(float scale);

	virtual GeometryVolatility GetGeometryVolatility() const = 0;
	virtual void SetGeometryVolatility(GeometryVolatility volatility) = 0;

	void SetPrimitiveType(PrimitiveType type);
	PrimitiveType GetPrimitiveType() const;

	virtual unsigned int GetMaxVertexCount() const = 0;
	        unsigned int GetMaxTriangleCount() const;
	virtual unsigned int GetMaxIndexCount() const = 0;

	virtual unsigned int GetVertexCount() const = 0;
	        unsigned int GetTriangleCount() const;
	virtual unsigned int GetIndexCount() const = 0;
	virtual unsigned int GetUVSetCount() const = 0;
	void SetUVCountPerVertex(int uv_count_per_vertex);
	int GetUVCountPerVertex() const;
	unsigned int GetEdgeCount() const;

	// Sets the UV-set to use when generating tangent- and bitangent vectors.
	// Default is 0. Tangents and bitangents are used with bump/normal mapping.
	void SetTangentsUVSet(unsigned int uv_set);

	virtual float*         GetVertexData() const                  = 0;
	virtual float*         GetUVData(unsigned int uv_set) const   = 0;
	virtual vtx_idx_t*     GetIndexData() const                   = 0;
	virtual lepra::uint8*  GetColorData() const                   = 0;

	virtual float* GetNormalData() const;
	float*         GetSurfaceNormalData() const;
	Edge*          GetEdgeData() const;
	float*         GetTangentData() const;	// Used with bump/normal mapping.
	float*         GetBitangentData() const;	// Used with bump/normal mapping.

	// 0 <= triangle < GetNumTriangles().
	// Given the triangle index (triangle), the function returns the three
	// indices by setting the values in indices.
	void GetTriangleIndices(int triangle, lepra::uint32 indices[4]) const;

	// Deletes the corresponding arrays and cleans stuff up.
	virtual void ClearVertexNormalData();
	void ClearSurfaceNormalData();
	void ClearEdgeData();
	void ClearTangentAndBitangentData();

	// Allocates memory for the corresponding arrays, if needed.
	void AllocVertexNormalData();
	void AllocSurfaceNormalData();
	void AllocTangentAndBitangentData();

	// Allocates memory for the corresponding arrays, if needed,
	// and generates the data (if needed).
	void GenerateVertexNormalData();
	void GenerateSurfaceNormalData();
	void GenerateEdgeData();
	void GenerateTangentAndBitangentData();



	// Vertices and normals are supposed to be updated at the same time.
	// Surface normals are considered changed if either vertices or
	// indices are changed.
	// Tangents are considered changed if either vertices of uv-coordinates
	// are changed.
	bool GetVertexDataChanged() const;
	bool GetUVDataChanged() const;
	bool GetColorDataChanged() const;
	bool GetIndexDataChanged() const;

	void SetVertexDataChanged(bool changed);
	void SetUVDataChanged(bool changed);
	void SetColorDataChanged(bool changed);
	void SetIndexDataChanged(bool changed);

	void SetVertexNormalsValid();
	void SetSurfaceNormalsValid();

	virtual ColorFormat GetColorFormat() const;

	// This has been renamed from SetUserData(). These functions are used
	// by the renderer to associate some arbitrary data with the geometry.
	// It would be better design to remove these functions and use a hash
	// table instead to do the association, but that is not as efficient.
	void SetRendererData(void* renderer_data);
	inline void* GetRendererData() const;

	size_t GetExtraData() const;
	void SetExtraData(size_t extra_data);

	void SetTransformation(const xform& transformation);
	const xform& GetBaseTransformation() const;
	virtual const xform& GetTransformation();
	bool GetTransformationChanged() const;
	void SetTransformationChanged(bool transformation_changed);
	bool GetBigOrientationChanged() const;
	void SetBigOrientationChanged(bool orientation_changed);
	const quat& GetLastBigOrientation() const;
	float GetBigOrientationThreshold() const;
	void SetBigOrientationThreshold(float big_orientation_threshold);
	static void SetDefaultBigOrientationThreshold(float big_orientation_threshold);

	void SetLastFrameVisible(unsigned int last_frame_visible);
	unsigned int GetLastFrameVisible() const;
	void SetAlwaysVisible(bool always_visible);
	bool GetAlwaysVisible() const;

	const BasicMaterialSettings& GetBasicMaterialSettings() const;
	BasicMaterialSettings& GetBasicMaterialSettings();
	void SetBasicMaterialSettings(const BasicMaterialSettings& mat_settings);

	// Calling GetBoundingRadius() will recalculate the bounding radius if necessary.
	float GetBoundingRadius();
	float GetBoundingRadius() const;

	// Sets the bounding radius. Once set, GetBoundingRadius() will never try to
	// recalculate the radius again.
	void SetBoundingRadius(float bounding_radius);

	// Explicitly recalculate the radius.
	void CalculateBoundingRadius();

	// The following functions will generate edge data.
	bool IsSolidVolume();	// Returns true if all edges are shared by two triangles.
	bool IsSingleObject();	// Returns true if all polygons are connected to each other
				// as one object.
	bool IsConvexVolume();	// Returns true if it is a solid volume, and
				// there is no concave angle between any two triangles.
				// Will generate surface normals if needed.
	bool IsTwoSided() const;
	void SetTwoSided(bool is_two_sided);
	bool IsRecvNoShadows() const;
	void SetRecvNoShadows(bool recv_shadows);

	// Debug functions.
	bool VerifyIndexData();

	// Functions used to handle UV-animations. Couldn't find any
	// better place to put this code.
	void SetUVAnimator(BoneAnimator* uv_animator);
	BoneAnimator* GetUVAnimator();
	const xform& GetUVTransform() const;

	void SetSurfaceNormalData(const float* surface_normal_data);
	void SetVertexNormalData(const float* vertex_normal_data, unsigned int num_vertices);
	void SetTangentAndBitangentData(const float* tangent_data, const float* bitangent_data, unsigned int num_vertices);
	virtual void SetIndexData(vtx_idx_t* index_data, unsigned index_count, unsigned max_index_count);

	void Copy(GeometryBase* geometry);
	void ClearAll();

	void SetSolidVolumeCheckValid(bool valid);
	void SetSingleObjectCheckValid(bool valid);
	void SetConvexVolumeCheckValid(bool valid);

	// Used by PortalManager.
	void SetParentCell(PortalManager::Cell* cell);
	PortalManager::Cell* GetParentCell();

	enum { //Flags
		kBoundingRadiusValid		= (1 << 0),
		kBoundingRadiusAlwaysValid	= (1 << 1),
		kSurfaceNormalsValid		= (1 << 2),
		kVertexNormalsValid		= (1 << 3),
		kTangentsValid			= (1 << 4),
		kSolidVolumeValid		= (1 << 5),
		kSingleObjectValid		= (1 << 6),
		kConvexVolumeValid		= (1 << 7),
		kValidFlagsMask		= kConvexVolumeValid-1,

		kVertexDataChanged		= (1 << 8),
		kUvDataChanged			= (1 << 9),
		kColorDataChanged		= (1 << 10),
		kIndexDataChanged		= (1 << 11),
		kIsSolidVolume			= (1 << 12),
		kIsSingleObject		= (1 << 13),
		kIsConvexVolume		= (1 << 14),
		kAlwaysVisible			= (1 << 15),
		kTransformationChanged		= (1 << 16),
		kRefTransformationChanged	= (1 << 17),
		kBigOrientationChanged		= (1 << 18),
		kExcludeCulling			= (1 << 19),
		kIsTwoSided			= (1 << 20),
		kRecvNoShadows			= (1 << 21),
		kContainsWireframe		= (1 << 22),
	};

	void SetFlag(lepra::uint32 pFlag, bool value);
	void SetFlag(lepra::uint32 pFlag);
	void ClearFlag(lepra::uint32 pFlag);
	bool CheckFlag(lepra::uint32 pFlag) const;
	uint32 GetFlags() const;
	static void SetDefaultFlags(lepra::uint32 flags);

protected:
	typedef std::list<Listener*> ListenerList;

	lepra::uint32 flags_;
	PrimitiveType primitive_type_;

	BasicMaterialSettings material_settings_;

	float bounding_radius_;
	float scale_;

	// Surface normals are primarily used when generating
	// the shadow volume (used to perform stencil shadows).
	float* surface_normal_data_;
	unsigned int surface_normal_count_;

	float* vertex_normal_data_;

	int uv_count_per_vertex_;
	Edge* edge_data_;
	unsigned int edge_count_;

	float* tangent_data_;   // Contains (x, y, z) triplets for tangent vectors.
	float* bitangent_data_; // Contains (x, y, z) triplets for bitangent vectors.
	unsigned int tangents_uv_set_;

	// Contains arbitrary data. This is used by Renderer and
	// subclasses to store associated data.
	void* renderer_data_;

	// The Cell pointer used by PortalManager. This is declared as void*
	// to avoid including PortalManager.h.
	PortalManager::Cell* parent_cell_;

	unsigned int last_frame_visible_;

	xform transformation_;
	quat big_orientation_;

	BoneAnimator* uv_animator_;

	ListenerList listener_list_;
	PreRenderCallback pre_render_callback_;
	PostRenderCallback post_render_callback_;

	size_t extra_data_;

	float big_orientation_threshold_;

	static lepra::uint32 default_flags_;
	static float default_big_orientation_threshold_;

public:
	str name_;

	logclass();
};



inline void* GeometryBase::GetRendererData() const {
	return renderer_data_;
}



}
