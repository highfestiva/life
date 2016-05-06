
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

// This class extends tbc::Model with one or more geometries of different types.
// It also provides functionality for the determination of the LOD-level.



#pragma once

#include "../../tbc/include/model.h"
#include "uirenderer.h"



namespace uitbc {



class AnimatedGeometry;
class TriangleBasedGeometry;
class ProgressiveTriangleGeometry;



class GraphicalModel: public tbc::Model {
public:

	// The geometry handler is responsible for uploading the geometry to the
	// renderer and updating LOD and animations. It gives the user the
	// possibility to manage resources his/her own way.
	class GeometryHandler {
	public:
		friend class GraphicalModel;

		GeometryHandler() :
			transform_animator_(0) {
		}
		virtual ~GeometryHandler() {
		}

		// The update function may reload the geometry, update the LOD
		// or animate the object.
		virtual void UpdateGeometry(float lod_level) = 0;
		virtual tbc::GeometryBase* GetGeometry() = 0;
	private:
		void SetTransformAnimator(tbc::BoneAnimator* transform_animator) {
			transform_animator_ = transform_animator;
		}
		tbc::BoneAnimator* GetTransformAnimator() const {
			return transform_animator_;
		}
		tbc::BoneAnimator* transform_animator_;
	};

	GraphicalModel();
	virtual ~GraphicalModel();

	// Takes ownership of the geometry handler (takes care of destruction).
	// transform_animator = The animator (see tbc::Model and BoneAnimator) to use for transform animations.
	void AddGeometry(const str& name, GeometryHandler* geometry, const str& transform_animator = "");

	tbc::GeometryBase* GetGeometry(const str& name);

	// Overloaded from tbc::Model where it is declared as public.
	void Update(double delta_time);

	// Calculates the detail level depending on the camera and calls Update(double).
	// void Update(double delta_time, Camera* pCamera);

	// Level of detail in the range [0, 1] where 0 equals the lowest level and 1 the highest.
	// This is automatically set through Update(double, Camera*).
	void SetDetailLevel(double level_of_detail);

	void SetLastFrameVisible(unsigned int last_frame_visible);
	unsigned int GetLastFrameVisible() const;
	void SetAlwaysVisible(bool always_visible);
	bool GetAlwaysVisible();

private:

	enum GeomType {
		kGeomStatic = 0,
		kGeomProgressive,
		kGeomAnimatedStatic,
		kGeomAnimatedProgressive
	};

	struct GeometryData {
		GeomType geom_type_;
		int current_lod_level_;
		int num_lod_levels_;
		tbc::GeometryBase* geometry_;
		Renderer::MaterialType material_type_;
		Renderer::GeometryID geom_id_;
		Renderer::TextureID* texture_id_;
		int num_textures_;
		Renderer::Shadows shadows_;
		str name_;
		tbc::BoneAnimator* transform_animator_;
	};

	int CalcLODIndex(int max_index);

	typedef HashTable<str, GeometryHandler*, std::hash<str>, 8> GeometryTable;

	GeometryTable geometry_table_;
	double level_of_detail_;
};



class DefaultStaticGeometryHandler : public GraphicalModel::GeometryHandler {
public:
	DefaultStaticGeometryHandler(TriangleBasedGeometry* geometry,
				     int num_lod_levels,
				     Renderer::TextureID* texture_id,
				     int num_textures,
				     Renderer::MaterialType material,
				     Renderer::Shadows shadows,
				     Renderer* renderer);

	void UpdateGeometry(float lod_level);
	tbc::GeometryBase* GetGeometry();

private:
	TriangleBasedGeometry* geometry_;
	int num_lod_levels_;
	Renderer::TextureID* texture_id_;
	int num_textures_;
	Renderer::MaterialType material_;
	Renderer::Shadows shadows_;

	int current_lod_level_;
	Renderer::GeometryID geom_id_;
	Renderer* renderer_;
};



class DefaultProgressiveGeometryHandler : public GraphicalModel::GeometryHandler {
public:
	DefaultProgressiveGeometryHandler(ProgressiveTriangleGeometry* geometry,
					  Renderer::MaterialType material,
					  Renderer::TextureID* texture_id,
					  int num_textures,
					  Renderer::Shadows shadows,
					  Renderer* renderer);

	void UpdateGeometry(float lod_level);
	tbc::GeometryBase* GetGeometry();

private:
	ProgressiveTriangleGeometry* geometry_;

	Renderer::GeometryID geom_id_;
	Renderer* renderer_;
};



class DefaultAnimatedStaticGeometryHandler : public GraphicalModel::GeometryHandler {
public:
	DefaultAnimatedStaticGeometryHandler(AnimatedGeometry* geometry,
					     int num_lod_levels,
					     Renderer::TextureID* texture_id,
					     int num_textures,
					     Renderer::MaterialType material,
					     Renderer::Shadows shadows,
					     Renderer* renderer);

	void UpdateGeometry(float lod_level);
	tbc::GeometryBase* GetGeometry();
private:
	AnimatedGeometry* geometry_;
	int num_lod_levels_;
	Renderer::TextureID* texture_id_;
	int num_textures_;
	Renderer::MaterialType material_;
	Renderer::Shadows shadows_;

	int current_lod_level_;
	Renderer::GeometryID geom_id_;
	Renderer* renderer_;
};



// This class assumes that AnimatedGeometry points to a progressive mesh.
// If it doesn't, program failure can be expected.
class DefaultAnimatedProgressiveGeometryHandler : public GraphicalModel::GeometryHandler {
public:
	DefaultAnimatedProgressiveGeometryHandler(AnimatedGeometry* geometry,
						  Renderer::TextureID* texture_id,
						  int num_textures,
						  Renderer::MaterialType material,
						  Renderer::Shadows shadows,
						  Renderer* renderer);

	void UpdateGeometry(float lod_level);
	tbc::GeometryBase* GetGeometry();
private:
	AnimatedGeometry* geometry_;
	Renderer::TextureID* texture_id_;
	int num_textures_;
	Renderer::MaterialType material_;
	Renderer::Shadows shadows_;

	Renderer::GeometryID geom_id_;
	Renderer* renderer_;
};



}
