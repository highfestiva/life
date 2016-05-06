/*
	File:   PortalManager.cpp
	Class:  PortalManager
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#include "pch.h"
#include "../include/portalmanager.h"
#include "../include/geometrybase.h"
#include "../../lepra/include/log.h"

namespace tbc {

PortalManager::PortalManager() {
}

PortalManager::~PortalManager() {
	ClearAll();
}

void PortalManager::ClearAll() {
	CellTable::Iterator c_iter;
	for (c_iter = cell_table_.First(); c_iter != cell_table_.End(); ++c_iter) {
		Cell* _cell = *c_iter;
		delete _cell;
	}

	cell_table_.RemoveAll();

	PortalList::iterator p_iter;
	for (p_iter = portal_list_.begin(); p_iter != portal_list_.end(); ++p_iter) {
		Portal* _portal = *p_iter;
		delete _portal;
	}

	portal_list_.clear();
	geom_set_.RemoveAll();
}

PortalManager::Portal* PortalManager::NewPortal(int num_vertices,
						vec3* vertex,
						Cell* cell1,
						Cell* cell2) {
	return new Portal(num_vertices, vertex, cell1, cell2);
}

PortalManager::Cell* PortalManager::NewCell(const str& cell_id,
					    const str& cell_description,
					    PortalManager* portal_manager) {
	return new Cell(cell_id, cell_description, portal_manager);
}

bool PortalManager::AddCell(const str& cell_id, const str& cell_description) {
	CellTable::Iterator iter;
	iter = cell_table_.Find(cell_id);

	if (iter != cell_table_.End()) {
		log_.Error("AddCell() - Cell \"" + cell_id + "\" already exist!");
		return false;
	}

	Cell* _cell = NewCell(cell_id, cell_description, this);
	cell_table_.Insert(cell_id, _cell);

	return true;
}

bool PortalManager::AddPortal(int num_vertices,
			      vec3* vertex,
			      const str& cell_i_d1,
			      const str& cell_i_d2) {
	if (num_vertices < 3) {
		log_.Error("AddPortal() - Can't add a portal with less than 3 vertices!");
	}

	CellTable::Iterator iter1;
	CellTable::Iterator iter2;
	iter1 = cell_table_.Find(cell_i_d1);
	iter2 = cell_table_.Find(cell_i_d2);

	if (iter1 == cell_table_.End()) {
		log_.Error("AddCell() - Cell \"" + cell_i_d1 + "\" does not exist!");
		return false;
	}
	if (iter2 == cell_table_.End()) {
		log_.Error("AddCell() - Cell \"" + cell_i_d2 + "\" does not exist!");
		return false;
	}

	Cell* _cell1 = *iter1;
	Cell* _cell2 = *iter2;

	Portal* _portal = NewPortal(num_vertices, vertex, _cell1, _cell2);
	portal_list_.push_back(_portal);

	_cell1->AddPortal(_portal);
	_cell2->AddPortal(_portal);
	return true;
}

bool PortalManager::AddGeometry(GeometryBase* geometry, const str& parent_cell_id) {
	if (geom_set_.Find(geometry) != geom_set_.End()) {
		log_.Error("AddGeometry() - Geometry already added!");
		return false;
	}

	CellTable::Iterator iter;
	iter = cell_table_.Find(parent_cell_id);

	if (iter == cell_table_.End()) {
		log_.Error("AddGeometry() - Cell \"" + parent_cell_id + "\" does not exist!");
		return false;
	}

	Cell* _cell = *iter;
	_cell->AddGeometry(geometry);

	geometry->SetParentCell(_cell);

	geom_set_.Insert(geometry);

	return true;
}

void PortalManager::RemoveGeometry(GeometryBase* geometry) {
	Cell* _cell = (Cell*)geometry->GetParentCell();
	if (_cell != 0) {
		_cell->RemoveGeometry(geometry);
	} else {
		log_.Warning("RemoveGeometry() - Parent Cell was NULL!");
	}

	geom_set_.Remove(geometry);
	geometry->SetParentCell(0);
}

str PortalManager::GetParentCellID(GeometryBase* geometry) {
	Cell* _cell = (Cell*)geometry->GetParentCell();
	if (_cell != 0) {
		return _cell->GetID();
	} else {
		log_.Warning("GetParentCellID() - Parent Cell was NULL!");
		return *invalid_cell_id_;
	}
}

str PortalManager::TestPortalCollision(const vec3& from_pos,
						  const vec3& to_pos,
						  GeometryBase* geometry) {
	Cell* _cell = (Cell*)geometry->GetParentCell();
	if (_cell == 0) {
		log_.Warning("TestPortalCollision() - Parent Cell was NULL!");
		return *invalid_cell_id_;
	}

	vec3 _from_pos(from_pos);

	// Loop as long as we are colliding with portals.
	for (;;) {
		float ttc = 0;
		Cell* other_cell = _cell->TestPortalCollision(_from_pos, to_pos, ttc);
		if (other_cell != 0 && other_cell != _cell) {
			// We have collided. Step into the other cell and update the step vector.
			_cell = other_cell;
			vec3 diff(to_pos - _from_pos);
			_from_pos += diff * ttc;
		} else {
			break;
		}
	}

	geometry->SetParentCell(_cell);
	return _cell->GetID();
}

str PortalManager::TestPortalCollision(const vec3& from_pos,
						  const vec3& to_pos,
						  const str& cell_id) {
	CellTable::Iterator iter = cell_table_.Find(cell_id);
	if (iter == cell_table_.End()) {
		log_.Warning("TestPortalCollision() - CellID \"" + cell_id + "\" does not exist!");
		return *invalid_cell_id_;
	}

	Cell* _cell = *iter;

	vec3 _from_pos(from_pos);

	// Loop as long as we are colliding with portals.
	for (;;) {
		float ttc = 0;
		Cell* other_cell = _cell->TestPortalCollision(_from_pos, to_pos, ttc);
		if (other_cell != 0 && other_cell != _cell) {
			// We have collided. Step into the other cell and update the step vector.
			_cell = other_cell;
			vec3 diff(to_pos - _from_pos);
			_from_pos += diff * ttc;
		} else {
			break;
		}
	}

	return _cell->GetID();
}

PortalManager::Cell* PortalManager::Cell::TestPortalCollision(const vec3& from_pos,
							      const vec3& to_pos,
							      float& time_to_collision) {
	PortalList::iterator iter;

	Cell* return_cell = this;

	// We have to loop through all portals in order to find the first collision.
	// This gives us a hint: Don't add too many portals to a cell!
	for (iter = portal_list_.begin(); iter != portal_list_.end(); ++iter) {
		Portal* _portal = *iter;
		float ttc;
		if (_portal->TestCollision(from_pos, to_pos, this, ttc) == true) {
			// Only consider the first collision.
			if (return_cell == this || ttc < time_to_collision) {
				return_cell = _portal->GetOtherCell(this);

				if (return_cell == 0) {
					log_.Fatal("TestPortalCollision() - Portal graph is corrupted!");
					return 0;
				}

				time_to_collision = ttc;
			}
		}
	}

	return return_cell;
}



PortalManager::Portal::Portal(int num_vertices,
			      vec3* vertex,
			      Cell* cell1,
			      Cell* cell2) :
	num_vertices_(num_vertices),
	edge_normal_(0),
	edge_d_(0),
	cell1_(cell1),
	cell2_(cell2) {
	// Make sure to generate a valid normal. We assume that PortalManager
	// never creates a Portal if there are less than 3 vertices.
	int i = 2;
	do {
		normal_.CrossUnit(vertex[i] - vertex[0], vertex[1] - vertex[0]);
		i++;
	}while(normal_.GetLength() < 0.99f && i < num_vertices);

	d_ = -normal_.Dot(vertex[0]);

	vertex_     = new vec3[num_vertices_];
	edge_normal_ = new vec3[num_vertices_];
	edge_d_ = new float[num_vertices_];

	for (i = 0; i < num_vertices_; i++) {
		vertex_[i] = vertex[i];

		// Generate collision test normals around the portal, all pointing to the
		// center of the portal.
		int next = (i + 1) % num_vertices_;
		edge_normal_[i].CrossUnit(vertex[next] - vertex[i], normal_);
		edge_d_[i] = -edge_normal_[i].Dot(vertex[i]);
	}
}

PortalManager::Portal::~Portal() {
	delete[] vertex_;
	delete[] edge_normal_;
	delete[] edge_d_;
}

PortalManager::Cell* PortalManager::Portal::GetOtherCell(Cell* cell) {
	if (cell == cell1_)
		return cell2_;
	if (cell == cell2_)
		return cell1_;

	return 0;
}

bool PortalManager::Portal::TestCollision(const vec3& from_pos,
					  const vec3& to_pos,
					  Cell* from,
					  float& time_to_collision) {
	time_to_collision = 1.0f;

	float test1 = normal_.Dot(from_pos) + d_;
	float test2 = normal_.Dot(to_pos)   + d_;

	// If both points are on the same side of the plane...
	if ((test1 >= 0 && test2 >= 0) ||
	   (test1 <= 0 && test2 <= 0)) {
		return false;
	}

	// Check if we are entering the plane from the "wrong" direction.
	if (from == cell1_) {
		if (test1 <= 0 && test2 >= 0) {
			return false;
		}
	} else if(from == cell2_) {
		if (test1 >= 0 && test2 <= 0) {
			return false;
		}
	} else {
		log_.Error("Portal::TestCollision() - Invalid cell pointer!");
		return false;
	}

	// The vector is crossing the plane. Find the point of intersection (POI).
	vec3 diff(to_pos - from_pos);
	float ttc = (normal_.Dot(to_pos) + d_) / normal_.Dot(diff);

	vec3 poi(from_pos + diff * ttc);

	// Now check if the POI is on the portal polygon.
	for (int i = 0; i < num_vertices_; i++) {
		float test = edge_normal_[i].Dot(poi) + edge_d_[i];

		if (test < 0) {
			return false;
		}
	}

	time_to_collision = ttc;
	return true;
}



PortalManager::Cell::Cell(const str& cell_id, const str& cell_description, PortalManager* portal_manager) :
	cell_id_(cell_id),
	cell_description_(cell_description),
	portal_manager_(portal_manager) {
}

PortalManager::Cell::~Cell() {
}

const str& PortalManager::Cell::GetID() {
	return cell_id_;
}

const str& PortalManager::Cell::GetDescription() {
	return cell_description_;
}

void PortalManager::Cell::AddPortal(Portal* portal) {
	portal_list_.push_back(portal);
}

void PortalManager::Cell::RemovePortal(Portal* portal) {
	portal_list_.remove(portal);
}

void PortalManager::Cell::AddGeometry(GeometryBase* geometry) {
	geom_set_.Insert(geometry);
}

void PortalManager::Cell::RemoveGeometry(GeometryBase* geometry) {
	geom_set_.Remove(geometry);
}



// Initialized in tbc::Init().
str* PortalManager::invalid_cell_id_ = 0;

loginstance(kUiGfx3D, PortalManager);

}
