/*
	Class:  ASELoader
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games

	NOTES:

	ASE = Ascii Scene Export.

	ASE is a common 3D file format which is native to 3D Studio MAX, but
	supported by most 3D-rendering software, except Lightwave perhaps.
	Maya, ActorX and UnrealEd, just to mention a few.

	Two sites that cover parts of the ASE file format are (2007-07-24):
	http://www.unrealwiki.com/wiki/ASE_File_Format
	http://www.solosnake.com/main/ase.htm

	The best way to understand this file format is to learn how 3DSMAX
	works. The data stored in an ASE-file (and the data structures
	within) seems to reflect the internal datastructures of 3DSMAX
	quite well.

	Obviously, this format is not well suited for games. The data needs to
	be converted into something more appropriate. The loading procedure is
	therefore divided into two parts:

	1. First parse the file, and read the data without modification
	into a bunch of data structures (see the subclasses of ASELoader).
	TODO: There are more tags to implement. Bones for instance...

	2. The user can choose between interpreting this data himself or
	to call the default conversion methods.
*/

#pragma once

#include "../../lepra/include/string.h"
#include "../../lepra/include/hashtable.h"
#include "../../lepra/include/reader.h"
#include "../../lepra/include/writer.h"
#include "../../lepra/include/rotationmatrix.h"
#include "../../lepra/include/vector3d.h"
#include "../../lepra/include/transformation.h"
#include "uirenderer.h"
#include <list>

namespace uitbc {

class TriangleBasedGeometry;
class Texture;

class ASELoader {
public:
	class ASEData;

	ASELoader();

	bool Load(ASEData& ase_data, const lepra::String& file_name);
	bool Load(ASEData& ase_data, lepra::Reader& reader);
	// bool Save(const GeometryList& geometry_list, const lepra::AnsiString& file_name);

	class Scene {
	public:
		Scene();
		~Scene();

		void Init();

		// All fields in this struct may be ignored.

		lepra::AnsiString filename_; // Orignal .max file name that this ASE was generated from.
		lepra::AnsiString comment_;
		int first_frame_;	// First frame of animation?
		int last_frame_;	// Last frame of animation?
		int frame_speed_;	// kFps?
		int ticks_per_frame_;	// Unknown.
		// Color vectors.
		double background_static_[3]; // Background color.
		double ambient_static_[3]; // Ambient light.
	};

	class Map {
	public:
		Map();
		~Map();

		void Init();

		lepra::AnsiString name_;
		lepra::AnsiString clazz_;
		lepra::AnsiString comment_;

		int sub_no_; // Unknown.

		double amount_; // Unknown. Presumed to be a global alpha value for this map.

		lepra::AnsiString bitmap_path_; // Path to texture file.

		lepra::AnsiString type_; // Unknown. Observed value is "Screen" (without quotes).

		double u_offset_;
		double v_offset_;
		double u_tiling_;
		double v_tiling_;
		double uvw_angle_;
		double uvw_blur_;
		double uvw_blur_offset_;
		double uvw_noise_amt_;
		double uvw_noise_size_;
		int uvw_noise_level_;
		double uvw_noise_phase_;

		lepra::AnsiString bitmap_filter_; // Ignore. Observer value is "Pyramidal" (without quotes).
	};

	class Material {
	public:
		Material();
		~Material();

		void Init();

		bool LoadAsTexture(Texture& texture);
		Renderer::MaterialType GetRecommendedMaterial();

		lepra::AnsiString name_;  // Ignore.
		lepra::AnsiString clazz_; // Ignore.
		lepra::AnsiString comment_;

		// Ambient glow. Unknown if this adds to or overrides *SCENE_AMBIENT_STATIC.
		double ambient_[3];
		double diffuse_[3]; // Diffuse color of the material.
		double specular_[3]; // Specular color... Should always be the same as the diffuse.
		double shine_; // Specifies how focused the specular highlight is. Ignore.
		double shine_strength_; // Unknown. Ignore.
		double transparency_;
		double wire_size_;

		typedef std::list<Material*> MaterialList;
		MaterialList sub_material_list_;

		lepra::AnsiString shading_; // Blinn or Phong. More possibilities (not observed though) could be
					 // Anisotropoc, Metal, Multi-Layer, Oren-Nayar-Blinn, Strauss, Sat or None.
		double xp_falloff_; // Some kind of exponential falloff. But of what?
		double self_illum_; // Self illumination.

		// Unknown. Observed value is "In" (without quotes).
		lepra::AnsiString falloff_;

		// Unknown. Observed value is "Filter" (without quotes).
		lepra::AnsiString xp_type_;

		// The maps are stored in lists since a test scene showed that several
		// refract maps can occur in the same material. So to keep things
		// consistent, all maps are stored in lists. The common case is that
		// each list contain zero or one map each.
		typedef std::list<Map*> MapList;
		MapList ambient_map_list_;
		MapList diffuse_map_list_; // Standard texture.
		MapList specular_map_list_;// Specular color. Ignore.
		MapList shine_map_list_;   // Specularity per texel.
		MapList shine_strength_map_list_;
		MapList self_illum_map_list_;
		MapList opacity_map_list_;
		MapList filter_color_map_list_;
		MapList bump_map_list_;
		MapList reflect_map_list_;
		MapList refract_map_list_;
	};

	// TM = Transformation Matrix.
	class NodeTM {
	public:
		NodeTM();
		~NodeTM();

		void Init();
		void GetRotationMatrix(lepra::RotationMatrixF& rot_mtx);
		void GetRotationMatrix(lepra::RotationMatrixD& rot_mtx);
		void GetPosition(lepra::Vector3DF& pos);
		void GetPosition(lepra::Vector3DD& pos);
		void GetTransformation(lepra::TransformationF& transform);
		void GetTransformation(lepra::TransformationD& transform);

		lepra::AnsiString node_name_; // Same as GeomObject::node_name_.
		lepra::AnsiString comment_;
		int inherit_pos_[3]; // Unknown. May be ignored.
		int inherit_rot_[3]; // Unknown. May be ignored.
		int inherit_scl_[3]; // Unknown. May be ignored.
		double tm_row0_[3]; // First row of transform matrix.
		double tm_row1_[3]; // Second row...
		double tm_row2_[3]; // Third row...
		double tm_row3_[3]; // Fourth row, same as tm_pos_.
		double tm_pos_[3]; // The position.
                // Another way to represent rotation:
		double tm_rot_axis_[3]; // Axis of rotation. Not needed if you use the matrix.
		double tm_rot_angle_; // Angle of rotation around the rotation axis.

		double tm_scale_[3]; // Scale of the transformation. This info is already
		                     // embedded in the matrix. May be ignored.
		double tm_scale_axis_[3]; // Axis of scaling, whatever that is. Ignore this
					 // and just go with the matrix.
		double tm_scale_axis_ang_; // Ditto.
	};

	class ControlPosSample {
	public:
		ControlPosSample();
		~ControlPosSample();

		void Init();

		int time_value_;
		double pos_[3];
	};

	class ControlRotSample {
	public:
		ControlRotSample();
		~ControlRotSample();

		void Init();

		int time_value_;
		double rot_[4];
	};

	class TMAnimation {
	public:
		TMAnimation();
		~TMAnimation();

		void Init();

		lepra::AnsiString node_name_; // Same as GeomObject::node_name_.
		lepra::AnsiString comment_;

		typedef std::list<ControlPosSample*> ControlPosSampleList;
		typedef std::list<ControlRotSample*> ControlRotSampleList;
		ControlPosSampleList control_pos_track_;
		ControlRotSampleList control_rot_track_;
	};

	class MappingChannel {
	public:
		MappingChannel();
		~MappingChannel();

		void Init();

		void AllocVertexArray();
		void AllocFaceArray();

		lepra::AnsiString comment_;
		// Number of texture coordinates. Doesn't equal num_vertex_.
		// Texture coordinates are per triangle, not per vertex.
		int num_t_vertex_;
		float* t_vertex_; // Texture coordinates UVW. U and V are the only ones needed.

		int num_tv_faces_; // Must equal num_faces_.
		int* tv_face_; // Stores indices into t_vertex_.
	};

	class Mesh {
	public:
		Mesh();
		~Mesh();

		void Init();
		MappingChannel* GetDefaultMappingChannel();
		void AllocVertexArray();
		void AllocFaceArray();
		void AllocCVertexArray();
		void AllocCFaceArray();
		void AllocNormalArrays();

		void GetGeometry(std::list<TriangleBasedGeometry*>& geometry_list);

		lepra::AnsiString comment_;
		int time_value_; // Time tag of animation.
		int num_vertex_;
		int num_faces_;
		float* vertex_;
		int* face_; // Only stores indices.
		bool* face_edge_visible_; // 3 bools per face, telling if the edge should
					  // be rendered or not. Ignore this.

		// Smoothing groups per face.
		std::list<int>* smoothing_group_list_;
		int* sub_material_; // Sub material per face.

		// All UV-sets.
		bool have_default_uv_set_;
		typedef std::list<MappingChannel*> MappingChannelList;
		MappingChannelList mapping_channel_list_;

		float* face_normals_;
		float* vertex_normals_;

		int num_c_vertex_; // Number of vertex colors.
		float* c_vertex_; // Vertex colors.
		int num_cv_faces_; // Must equal num_faces_.
		int* c_face_; // Vertex colors per face.
	private:
		// Used in GetGeometry().
		class FaceVertex {
		public:
			int v_index_; // Vertex index.
			int c_index_; // Color index.
			int* t_index_; // Texture coordinate index per UV-set.
			int num_uv_sets_;

			FaceVertex();
			FaceVertex(const FaceVertex& other);
			~FaceVertex();

			void SetNumUVSets(int num_uv_sets);
			static size_t GetHashCode(const FaceVertex& key);
			bool operator== (const FaceVertex& other) const;
			FaceVertex& operator= (const FaceVertex& other);
		};

		class Face {
		public:
			Face();
			~Face();
			int sub_material_;
			FaceVertex vertex_[3];
		};

		typedef std::list<Face*> FaceList;
		typedef lepra::HashTable<int, FaceList*> FaceListTable;
		void SetupFaceListTable(Face* face_array, FaceListTable& face_list_table);
	};

	class GeomObject {
	public:
		GeomObject();
		~GeomObject();

		void Init();
		void AllocTMAnimation();

		void GetGeometry(std::list<TriangleBasedGeometry*>& geometry_list);

		lepra::AnsiString node_name_;
		// TODO: Add lepra::AnsiString mNodeParent;
		lepra::AnsiString comment_;
		typedef std::list<NodeTM*> NodeTMList;
		typedef std::list<Mesh*> MeshList;
		NodeTMList node_tm_list_;
		MeshList mesh_list_;
		bool motion_blur_; // Ignore.
		bool cast_shadow_;
		bool recv_shadow_; // Ignore.
		TMAnimation* tm_animation_;
		int material_ref_; // Default material index for those faces (triangles)
				    // that doesn't have a MTLID tag.
	};

	class CameraSettings {
	public:
		CameraSettings();
		~CameraSettings();

		void Init();

		int time_value_;
		double near_;
		double far_;
		double fov_;
		double t_dist_;
	};

	class CameraObject {
	public:
		CameraObject();
		~CameraObject();

		void Init();
		void AllocCameraSettings();
		void AllocTMAnimation();

		lepra::AnsiString node_name_;
		lepra::AnsiString comment_;
		lepra::AnsiString camera_type_; // Observed values: Target...
		typedef std::list<NodeTM*> NodeTMList;
		NodeTMList node_tm_list_;
		TMAnimation* tm_animation_;
		CameraSettings* settings_;
	};

	class LightSettings {
	public:
		LightSettings();
		~LightSettings();

		void Init();

		int time_value_;
		double color_[3];
		double intens_;
		double aspect_;
		double hot_spot_;
		double falloff_;
		double t_dist_;
		double map_bias_;
		double map_range_;
		double map_size_;
		double ray_bias_;
	};

	class LightObject {
	public:
		LightObject();
		~LightObject();

		void Init();
		void AllocLightSettings();
		void AllocTMAnimation();

		lepra::AnsiString node_name_;
		lepra::AnsiString comment_;
		lepra::AnsiString light_type_; // Observed values: Target...
		typedef std::list<NodeTM*> NodeTMList;
		NodeTMList node_tm_list_;

		lepra::AnsiString shadows_;
		bool use_light_;
		lepra::AnsiString spot_shape_;
		bool use_global_;
		bool abs_map_bias_;
		bool over_shoot_;
		LightSettings* settings_;
		TMAnimation* tm_animation_;
	};

	class ASEData {
	public:
		ASEData();
		~ASEData();

		void Init();

		int m3DSMaxAsciiExport_; // = 200
		lepra::AnsiString comment_;

		Scene* scene_;

		typedef std::list<Material*> MaterialList;
		typedef std::list<GeomObject*> GeomObjectList;
		typedef std::list<CameraObject*> CameraObjectList;
		typedef std::list<LightObject*> LightObjectList;
		MaterialList material_list_;
		GeomObjectList geom_list_;
		CameraObjectList cam_list_;
		LightObjectList light_list_;
	};

private:

	bool Parse(ASEData& ase_data, const lepra::AnsiString& data_string);

	bool ReadQuotedString(lepra::AnsiString& s, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens);
	bool ReadUnquotedString(lepra::AnsiString& s, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens);
	bool ReadInt(int& i, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens);
	bool ReadIntVec(int* i, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens);
	bool ReadFloat(double& f, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens);
	bool ReadFloatVec(double* f, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens);
	bool ReadBool(bool& _bool, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens);

	// Vertex lists look the same, and all "lightweight" face lists
	// (MESH_FACE_LIST is the only exception) look the same. Thus,
	// we don't need to implement a unique function for each type.
	bool ReadVertexList(float* vertex, int num_vertex, char* vertex_identifier, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens);
	bool ReadFaceList(int* face, int num_faces, char* face_identifier, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens);

	bool ReadSCENE(ASEData& ase_data, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens);
	bool ReadMATERIAL_LIST(ASEData& ase_data, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens);
	bool ReadMATERIAL(Material* material, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens);
	bool ReadMAP(std::list<Map*>& map_list, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens);
	bool ReadGEOMOBJECT(GeomObject* geom_obj, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens);
	bool ReadCAMERAOBJECT(CameraObject* cam_obj, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens);
	bool ReadLIGHTOBJECT(LightObject* light_obj, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens);
	bool ReadCAMERA_SETTINGS(CameraSettings* cam_settings, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens);
	bool ReadLIGHT_SETTINGS(LightSettings* light_settings, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens);
	bool ReadNODE_TM(NodeTM* node_tm, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens);
	bool ReadTM_ANIMATION(TMAnimation* tm_animation, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens);
	bool ReadCONTROL_POS_TRACK(TMAnimation* tm_animation, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens);
	bool ReadCONTROL_ROT_TRACK(TMAnimation* tm_animation, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens);
	bool ReadCONTROL_POS_SAMPLE(ControlPosSample* control_pos_sample, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens);
	bool ReadCONTROL_ROT_SAMPLE(ControlRotSample* control_rot_sample, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens);
	bool ReadMESH(Mesh* mesh, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens);
	bool ReadMESH_FACE_LIST(Mesh* mesh, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens);
	bool ReadMESH_NORMALS(Mesh* mesh, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens);
	bool ReadMESH_MAPPINGCHANNEL(Mesh* mesh, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens);
};

}
