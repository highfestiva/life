
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "uitbc.h"
#include "../../lepra/include/canvas.h"
#include "../../lepra/include/transformation.h"
#include "../../tbc/include/chunkyclass.h"



namespace uitbc {



class ChunkyClass: public tbc::ChunkyClass {
public:
	struct Material {
		vec3 ambient_;
		vec3 diffuse_;
		vec3 specular_;
		float shininess_;
		float alpha_;
		bool smooth_;
		Canvas::ResizeHint resize_hint_;
		std::vector<str> texture_list_;
		str shader_name_;

		Material();
	};

	ChunkyClass();
	virtual ~ChunkyClass();

	void AddMesh(int phys_index, const str& mesh_base_name, const xform& transform, float scale);
	size_t GetMeshCount() const;
	void GetMesh(size_t index, int& phys_index, str& mesh_base_name, xform& transform, float& scale) const;
	void SetLastMeshMaterial(const Material& material);
	const Material& GetMaterial(size_t mesh_index) const;

private:
	struct PhysMeshEntry {
		PhysMeshEntry(int phys_index, const str& mesh_base_name,
			const xform& transform, float scale):
			phys_index_(phys_index),
			mesh_base_name_(mesh_base_name),
			transform_(transform),
			scale_(scale) {
		}
		int phys_index_;
		str mesh_base_name_;
		xform transform_;
		float scale_;
		Material material_;
	};
	typedef std::vector<PhysMeshEntry> PhysMeshArray;
	PhysMeshArray mesh_array_;

	logclass();
};



}
