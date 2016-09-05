
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../uicure/include/uicppcontextobject.h"
#include "../life.h"



namespace uitbc {
class GeometryBatch;
}



namespace life {



class MassObject: public UiCure::CppContextObject {
	typedef UiCure::CppContextObject Parent;
public:
	MassObject(cure::ResourceManager* resource_manager, const str& class_resource_name,
		UiCure::GameUiManager* ui_manager, tbc::PhysicsManager::BodyID terrain_body_id, size_t instance_count,
		float side_length);
	virtual ~MassObject();

	void SetSeed(unsigned seed);
	void SetRender(bool render);
	virtual void UiMove();

private:
	virtual void OnLoaded();
	void PositionToGrid(const vec3& position, int& x, int& y) const;
	void GridToPosition(int x, int y, vec3& position) const;
	void MoveToSquare(int x, int y);
	void CreateSquare(size_t x, size_t y);
	bool GetObjectPlacement(vec3& position) const;

	class Square {
	public:
		Square(uint32 seed, const MeshArray& resource_array,
			const std::vector<xform>& displacement_array, uitbc::Renderer* renderer);
		~Square();
		void SetRender(bool render, float alpha);

	private:
		typedef std::pair<uitbc::GeometryBatch*, uitbc::Renderer::GeometryID> MassMeshPair;
		typedef std::vector<MassMeshPair> MassMeshArray;
		MassMeshArray mass_mesh_array_;
		uitbc::Renderer* renderer_;
	};

	enum {
		kSquareMidToCorner	= 3,
		kSquareSide		= 7,
		kSquareCount		= kSquareSide*kSquareSide,
	};

	tbc::PhysicsManager::BodyID terrain_body_id_;
	size_t square_instance_count_;
	Square* square_array_[kSquareCount];
	const float visible_add_term_;
	float fully_visible_distance_;
	float visible_distance_factor_;
	int square_side_length_;
	int middle_square_x_;
	int middle_square_y_;
	unsigned seed_;

	logclass();
};



}
