
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "level.h"
#include "../lepra/include/cyclicarray.h"
#include "../lepra/include/plane.h"
#include "../uicure/include/uigameuimanager.h"
#include "../uitbc/include/uibasicmeshcreator.h"
#include "../uitbc/include/uitrianglebasedgeometry.h"
#include "rtvar.h"



namespace Bound {



Level::Level(cure::ResourceManager* resource_manager, const str& class_id, UiCure::GameUiManager* ui_manager):
	Parent(resource_manager, class_id, ui_manager),
	gfx_mesh_(0),
	gfx_window_mesh_(0),
	gfx_mesh_id_(0),
	gfx_window_mesh_id_(0),
	phys_mesh_body_id_(0),
	body_index_data_(0),
	level_(0),
	original_volume_(1),
	volume_(1) {
}

Level::~Level() {
}



void Level::GenerateLevel(tbc::PhysicsManager* physics_manager, bool vary_shapes, int level) {
	(void)level;

	if (gfx_mesh_id_) {
		ui_manager_->GetRenderer()->RemoveGeometry(gfx_mesh_id_);
		gfx_mesh_id_ = 0;
	}
	if (gfx_window_mesh_id_) {
		ui_manager_->GetRenderer()->RemoveGeometry(gfx_window_mesh_id_);
		gfx_window_mesh_id_ = 0;
	}

	CreateMesh(vary_shapes, level, 4.5f, 4.5f, 3);
	gfx_mesh_->GetBasicMaterialSettings().diffuse_	= vec3(0.5f,0.5f,0.5f);
	gfx_mesh_->GetBasicMaterialSettings().specular_	= vec3(0,0,0);
	gfx_mesh_->GetBasicMaterialSettings().shininess_	= false;
	gfx_mesh_->GetBasicMaterialSettings().smooth_	= false;
	gfx_mesh_->SetGeometryVolatility(tbc::GeometryBase::kGeomSemiStatic);
	gfx_mesh_id_ = ui_manager_->GetRenderer()->AddGeometry(gfx_mesh_, uitbc::Renderer::kMatVertexColorSolid, uitbc::Renderer::kForceNoShadows);

	delete gfx_window_mesh_;
	gfx_window_mesh_ = 0;

	CreatePhysicsMesh(physics_manager);
	DeleteWindowBoxes(physics_manager);

	original_volume_ = CalculateVolume();
	volume_ = original_volume_;
}

const uitbc::TriangleBasedGeometry* Level::GetMesh() const {
	return gfx_mesh_;
}

const uitbc::TriangleBasedGeometry* Level::GetWindowMesh() const {
	return gfx_window_mesh_;
}

float Level::GetVolumePart() const {
	return volume_ / original_volume_;
}

void Level::SetTriangles(tbc::PhysicsManager* physics_manager, const std::vector<float>& vertices, const std::vector<uint8>& colors) {
	ui_manager_->GetRenderer()->RemoveGeometry(gfx_mesh_id_);

	SetVertices(gfx_mesh_, &vertices[0], vertices.size()/3, &colors[0]);
	gfx_mesh_id_ = ui_manager_->GetRenderer()->AddGeometry(gfx_mesh_, uitbc::Renderer::kMatVertexColorSolid, uitbc::Renderer::kForceNoShadows);
	CreatePhysicsMesh(physics_manager);

	volume_ = CalculateVolume();
}

void Level::SetWindowTriangles(const std::vector<float>& vertices) {
	if (gfx_window_mesh_id_) {
		ui_manager_->GetRenderer()->RemoveGeometry(gfx_window_mesh_id_);
	}

	SetVertices(gfx_window_mesh_, vertices.empty()? 0 : &vertices[0], vertices.size()/3, 0);
	gfx_window_mesh_id_ = ui_manager_->GetRenderer()->AddGeometry(gfx_window_mesh_, uitbc::Renderer::kMatSingleColorBlended, uitbc::Renderer::kForceNoShadows);
}

void Level::AddCutPlane(tbc::PhysicsManager* physics_manager, const Plane& window_plane, const std::vector<float>& vertices, const Color& color) {
	if (gfx_window_mesh_id_) {
		ui_manager_->GetRenderer()->RemoveGeometry(gfx_window_mesh_id_);
	}

	if (!gfx_window_mesh_) {
		gfx_window_mesh_ = new uitbc::TriangleBasedGeometry;
	}
	gfx_window_mesh_->GetBasicMaterialSettings().diffuse_	= vec3(color.red_/255.0f, color.green_/255.0f, color.blue_/255.0f);
	gfx_window_mesh_->GetBasicMaterialSettings().specular_	= vec3(1,1,1);
	gfx_window_mesh_->GetBasicMaterialSettings().alpha_	= color.alpha_/255.0f;
	gfx_window_mesh_->GetBasicMaterialSettings().shininess_	= true;
	gfx_window_mesh_->GetBasicMaterialSettings().smooth_	= false;

	std::vector<float> _vertices(vertices);
	const float* v = gfx_window_mesh_->GetVertexData();
	int vc = gfx_window_mesh_->GetVertexCount()*3;
	for (int x = 0; x < vc; ++x) {
		_vertices.push_back(v[x]);
	}
	SetVertices(gfx_window_mesh_, &_vertices[0], _vertices.size()/3, 0);
	gfx_window_mesh_id_ = ui_manager_->GetRenderer()->AddGeometry(gfx_window_mesh_, uitbc::Renderer::kMatSingleColorBlended, uitbc::Renderer::kForceNoShadows);

	AddPhysicsWindowBox(physics_manager, window_plane);
}

void Level::RenderOutline() {
	const float x = size_.x / 2;
	const float y = size_.y / 2;
	const float z = size_.z / 2;
	static const vec3 v[] =
	{
		vec3(-x, +y, +z), vec3(+x, +y, +z),
		vec3(+x, +y, +z), vec3(+x, +y, -z),
		vec3(+x, +y, -z), vec3(-x, +y, -z),
		vec3(-x, +y, -z), vec3(-x, +y, +z),

		vec3(-x, -y, +z), vec3(+x, -y, +z),
		vec3(+x, -y, +z), vec3(+x, -y, -z),
		vec3(+x, -y, -z), vec3(-x, -y, -z),
		vec3(-x, -y, -z), vec3(-x, -y, +z),

		vec3(-x, -y, +z), vec3(-x, +y, +z),
		vec3(+x, -y, +z), vec3(+x, +y, +z),
		vec3(-x, -y, -z), vec3(-x, +y, -z),
		vec3(+x, -y, -z), vec3(+x, +y, -z),
	};
	const int cnt = LEPRA_ARRAY_COUNT(v)/2;
	for (int i = 0; i < cnt; ++i) {
		ui_manager_->GetRenderer()->DrawLine(v[i*2+0], v[i*2+1]-v[i*2+0], ORANGE);
	}
}



uitbc::TriangleBasedGeometry* Level::CreateMesh(bool vary_shapes, int level, float x, float y, float z) {
	if (gfx_mesh_) {
		delete gfx_mesh_;
	}
	float move_z = 0;
	const int shape = vary_shapes? level%4 : 0;
	switch (shape) {
		case 0:
			gfx_mesh_ = uitbc::BasicMeshCreator::CreateFlatBox(x, y, z, 1, 1, 1);
			size_ = vec3(x,y,z);
		break;
		case 1:
			gfx_mesh_ = uitbc::BasicMeshCreator::CreateEllipsoid(x*0.75f, y*0.75f, z*0.7f, 14, 14);
			size_ = vec3(x*1.5f,y*1.5f,z*1.4f);
		break;
		case 2:
			gfx_mesh_ = uitbc::BasicMeshCreator::CreateCone((x+y)/2*0.6f, z*1.4f, 16);
			size_ = vec3(x*1.2f,y*1.2f,z*1.125f);
			move_z = -z*1.1f/2;
		break;
		case 3:
			gfx_mesh_ = uitbc::BasicMeshCreator::CreateCylinder(z*0.6f, z*0.6f, (x+y)/2*1.2f, 14);
			size_ = vec3(x*1.2f,z*1.2f,z*1.2f);
		break;
	}
	// Create unique vertices for each triangle to simplify cutting.
	std::vector<float> nv;
	const float* v = gfx_mesh_->GetVertexData();
	const int tc = gfx_mesh_->GetTriangleCount();
	const vtx_idx_t* indices = gfx_mesh_->GetIndexData();
	for (int t = 0; t < tc*3; ++t) {
		nv.push_back(v[indices[t]*3+0]);
		nv.push_back(v[indices[t]*3+1]);
		nv.push_back(v[indices[t]*3+2] + move_z);
	}

	Color _colors[] = { RED, GREEN, MAGENTA, BLUE, YELLOW, DARK_GRAY, };
	const int color_count = LEPRA_ARRAY_COUNT(_colors);
	const int vc = (int)nv.size()/3;
	std::vector<uint8> _color_data;
	for (int x = 0; x < vc; ++x) {
		const int c = x / 6 % color_count;
		Color& _color = _colors[c];
		_color_data.push_back(_color.red_);
		_color_data.push_back(_color.green_);
		_color_data.push_back(_color.blue_);
		_color_data.push_back(255);
	}

	SetVertices(gfx_mesh_, &nv[0], vc, &_color_data[0]);
	return gfx_mesh_;
}

void Level::SetVertices(uitbc::TriangleBasedGeometry* gfx_mesh, const float* v, size_t vc, const uint8* color_data) {
	std::vector<uint32> ni;
	for (size_t x = 0; x < vc; ++x) {
		ni.push_back(x);
	}

	gfx_mesh->Set(v, 0, 0, color_data, tbc::GeometryBase::kColorRgba, ni.empty()? 0 : &ni[0], vc, vc, tbc::GeometryBase::kTriangles, tbc::GeometryBase::kGeomSemiStatic);
	FlipTriangles(gfx_mesh);
	gfx_mesh->SetAlwaysVisible(true);
}

void Level::FlipTriangles(uitbc::TriangleBasedGeometry* mesh) {
	vtx_idx_t* triangles = mesh->GetIndexData();
	const int tc = mesh->GetTriangleCount();
	for (int x = 0; x < tc; ++x) {
		std::swap(triangles[x*3+1], triangles[x*3+2]);
	}
}

void Level::CreatePhysicsMesh(tbc::PhysicsManager* physics_manager) {
	if (phys_mesh_body_id_) {
		physics_manager->DeleteBody(phys_mesh_body_id_);
	}

	const float friction = 0.7f;
	const float bounce = 1.0f;
	const int ic = gfx_mesh_->GetIndexCount();
	delete[] body_index_data_;
	body_index_data_ = new uint32[ic];
	for (int x = 0; x < ic; ++x) {
		body_index_data_[x] = gfx_mesh_->GetIndexData()[x];
	}
	phys_mesh_body_id_ = physics_manager->CreateTriMesh(true, gfx_mesh_->GetVertexCount(), gfx_mesh_->GetVertexData(),
		gfx_mesh_->GetTriangleCount(), body_index_data_, xform(), 0, tbc::PhysicsManager::kStatic, friction, bounce, GetInstanceId());
}

void Level::AddPhysicsWindowBox(tbc::PhysicsManager* physics_manager, const Plane& plane) {
	xform transform(plane.GetOrientation(), plane.n*plane.d);
	const float friction = 0.7f;
	const float bounce = 1.0f;
	tbc::PhysicsManager::BodyID phys_window_id = physics_manager->CreateBox(true, transform, 0, vec3(20,20,0.1f),
		tbc::PhysicsManager::kStatic, friction, bounce, GetInstanceId());
	phys_window_box_ids_.push_back(phys_window_id);
}

void Level::DeleteWindowBoxes(tbc::PhysicsManager* physics_manager) {
	std::vector<tbc::PhysicsManager::BodyID>::iterator x = phys_window_box_ids_.begin();
	for (; x != phys_window_box_ids_.end(); ++x) {
		physics_manager->DeleteBody(*x);
	}
	phys_window_box_ids_.clear();
}

float Level::CalculateVolume() const {
	float volume = 0;
	const int tc = gfx_mesh_->GetVertexCount()/3;
	const float* v = gfx_mesh_->GetVertexData();
	for (int x = 0; x < tc; ++x) {
		float Px = v[x*9+0];
		float Py = v[x*9+1];
		float Pz = v[x*9+2];
		float Qx = v[x*9+3];
		float Qy = v[x*9+4];
		float Qz = v[x*9+5];
		float Rx = v[x*9+6];
		float Ry = v[x*9+7];
		float Rz = v[x*9+8];
		volume += Px*Qy*Rz + Py*Qz*Rx + Pz*Qx*Ry - Px*Qz*Ry - Py*Qx*Rz - Pz*Qy*Rx;
		/*vec3 p0(v[x*9+0], v[x*9+1], v[x*9+2]);
		vec3 p1(v[x*9+3], v[x*9+4], v[x*9+5]);
		vec3 p2(v[x*9+6], v[x*9+7], v[x*9+8]);
		p0.x += 100;
		p1.x += 100;
		p2.x += 100;
		volume += p0 * p1.Cross(p2);*/
	}
	return std::abs(volume) / 6;
}



}
