
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

// This class constitutes a portal/cell graph which can be used to
// perform portal culling in an indoor scene. The actual culling
// algorithm is implemented in UiPortalManager.h though.

// All portals are convex polygons. Concave polygons will result in errors.

// Performance hint: Keep the number of portals per cell as low as
// possible. All portals in a cell are tested for collision when
// calling TestPortalCollision().



#pragma once

#include "tbc.h"
#include "../../lepra/include/graphics2d.h"
#include "../../lepra/include/hashset.h"
#include "../../lepra/include/hashtable.h"
#include "../../lepra/include/vector3d.h"
#include "../../lepra/include/string.h"
#include "../../lepra/include/string.h"
#include <list>



namespace tbc {



class GeometryBase;



class PortalManager {
public:

	friend class GeometryBase;

	friend void Init();
	friend void Shutdown();

	PortalManager();
	virtual ~PortalManager();

	void ClearAll();

	// Creates a new empty cell with the given ID.
	bool AddCell(const str& cell_id, const str& cell_description);

	// Adds a portal. Both cells must exist. Cell1 is the cell "in front" of the portal -
	// in the direction of the surface normal.
	bool AddPortal(int num_vertices,
			   vec3* vertex,
			   const str& cell_i_d1,
			   const str& cell_i_d2);

	// Adds the geometry to the given cell.
	bool AddGeometry(GeometryBase* geometry, const str& parent_cell_id);
	void RemoveGeometry(GeometryBase* geometry);

	str GetParentCellID(GeometryBase* geometry);

	// Test collision against portals in the geometry's parent cell.
	// Testing as if moving from pFromPos to pToPos (both given in world
	// coordinates), and updates the geometry's parent cell if a collision
	// occurs. Returns the ID of the geometry's parent cell after collision.
	str TestPortalCollision(const vec3& from_pos,
					   const vec3& to_pos,
					   GeometryBase* geometry);

	// Test collision against portals in the cell with the given ID.
	// Testing as if moving from pFromPos to pToPos (both given in world
	// coordinates). Returns the ID of the cell after collision.
	str TestPortalCollision(const vec3& from_pos,
					   const vec3& to_pos,
					   const str& cell_id);

	// This function is implemented in UiPortalManager.
	// void TraverseGraph(Renderer* renderer, const str& cell_id);

protected:

	class Cell;
	class Portal {
	public:
		Portal(int num_vertices,
		       vec3* vertex,
		       Cell* cell1,
		       Cell* cell2);
		virtual ~Portal();

		// Returns "true" on collision and the time to collision.
		bool TestCollision(const vec3& from_pos,
						   const vec3& to_pos,
						   Cell* from,
						   float& time_to_collision);

		Cell* GetOtherCell(Cell* cell);

	protected:
		int num_vertices_;

		vec3* vertex_;
		// Generated normals which all point towards the center of the portal.
		vec3* edge_normal_;
		float* edge_d_; // The fourth constant in the plane equation.

		Cell* cell1_;
		Cell* cell2_;

		vec3 normal_;
		float d_;
	};

	class Cell {
	public:

		Cell(const str& cell_id, const str& cell_description, PortalManager* portal_manager);
		virtual ~Cell();

		const str& GetID();
		const str& GetDescription();

		void AddPortal(Portal* portal);
		void RemovePortal(Portal* portal);

		void AddGeometry(GeometryBase* geometry);
		void RemoveGeometry(GeometryBase* geometry);

		Cell* TestPortalCollision(const vec3& from_pos,
					  const vec3& to_pos,
					  float& time_to_collision);
	protected:

		typedef HashSet<GeometryBase*, LEPRA_VOIDP_HASHER> GeomSet;
		typedef std::list<Portal*> PortalList;

		GeomSet geom_set_;
		PortalList portal_list_;
		str cell_id_;
		str cell_description_;
		PortalManager* portal_manager_;
	};

	typedef HashSet<GeometryBase*, LEPRA_VOIDP_HASHER> GeomSet;
	typedef HashTable<str, Cell*> CellTable;
	typedef std::list<Portal*> PortalList;

	virtual Portal* NewPortal(int num_vertices,
				 vec3* vertex,
				 Cell* cell1,
				 Cell* cell2);
	virtual Cell* NewCell(const str& cell_id,
			      const str& cell_description,
			      PortalManager* portal_manager);

	GeomSet geom_set_;
	CellTable cell_table_;
	PortalList portal_list_;

	// Initialized in tbc::Init().
	static str* invalid_cell_id_;

	logclass();
};



}
