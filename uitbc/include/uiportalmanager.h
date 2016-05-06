/*
	Class:  PortalManager
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

	NOTES:

	This class constitutes a portal/cell graph which can be used to
	perform portal culling in an indoor scene.

	All portals are convex polygons. Concave polygons will result in errors.

	Performance hint: Keep the number of portals per cell as low as
	possible. All portals in a cell are tested for collision when
	calling TestPortalCollision().
*/



#pragma once

#include "../../tbc/include/portalmanager.h"
#include "../../lepra/include/string.h"
#include "../include/uitbc.h"



namespace uitbc {



class Renderer;



class PortalManager: public tbc::PortalManager {
	typedef tbc::PortalManager Parent;
public:
	PortalManager(){}
	virtual ~PortalManager(){}

	void TraverseGraph(Renderer* renderer, const str& cell_id);

private:

	class Cell;

	class Portal : public Parent::Portal {
	public:
		Portal(int num_vertices,
		       vec3* vertex,
		       Cell* cell1,
		       Cell* cell2);
		virtual ~Portal();

		void Traverse(Renderer* renderer, const PixelRect& rect, Cell* from);
	};

	class Cell: public Parent::Cell {
	public:
		Cell(const str& cell_id, const str& cell_description, PortalManager* portal_manager);
		virtual ~Cell();

		void Traverse(Renderer* renderer, const PixelRect& rect);

	private:
		int last_frame_visible_;
	};

	virtual Parent::Portal* NewPortal(int num_vertices,
						      vec3* vertex,
						      Parent::Cell* cell1,
						      Parent::Cell* cell2);
	virtual Parent::Cell* NewCell(const str& cell_id,
						  const str& cell_description,
						  Parent::PortalManager* portal_manager);

	logclass();
};



}
