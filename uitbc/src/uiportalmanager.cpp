/*
	Class:  PortalManager
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#include "pch.h"
#include "../include/uiportalmanager.h"
#include "../include/uirenderer.h"
#include "../../lepra/include/log.h"
#include "../../tbc/include/geometrybase.h"

namespace uitbc {

tbc::PortalManager::Portal* PortalManager::NewPortal(int num_vertices,
						     vec3* vertex,
						     Parent::Cell* cell1,
						     Parent::Cell* cell2) {
	return new Portal(num_vertices, vertex, (Cell*)cell1, (Cell*)cell2);
}

tbc::PortalManager::Cell* PortalManager::NewCell(const str& cell_id,
						 const str& cell_description,
						 Parent::PortalManager* portal_manager) {
	return new Cell(cell_id, cell_description, (PortalManager*)portal_manager);
}

void PortalManager::TraverseGraph(Renderer* renderer, const str& cell_id) {
	CellTable::Iterator iter = cell_table_.Find(cell_id);
	if (iter == cell_table_.End()) {
		log_.Warning("TraverseGraph - CellID \"" + cell_id + "\" does not exist!");
		return;
	}

	Cell* cell = (Cell*)*iter;
	cell->Traverse(renderer, renderer->GetClippingRect());
}

void PortalManager::Portal::Traverse(Renderer* renderer, const PixelRect& rect, Cell* from) {
	PixelRect _rect(renderer->GetBoundingRect(vertex_, num_vertices_));
	bool front = renderer->IsFacingFront(vertex_, num_vertices_);

	// Stop the recursion if we try to traverse this portal from the "wrong" direction.
	if ((front == true  && from == cell1_) ||
		(front == false && from == cell2_)) {
		_rect = _rect.GetOverlap(rect);

		if (_rect.left_ < _rect.right_ &&
		   _rect.top_  < _rect.bottom_) {
			((Cell*)GetOtherCell(from))->Traverse(renderer, _rect);
		}
	}
}

void PortalManager::Cell::Traverse(Renderer* renderer, const PixelRect& rect) {
	// If not already visited, tag all geometry. Since two cells can be attached
	// to each other with more than one portal, we can't just return if the cell
	// has already been visited. We have to let Portal take care of the terminating
	// condition in this recursive process.
	if (last_frame_visible_ != (int)renderer->GetCurrentFrame()) {
		// Tag this cell as "visited".
		last_frame_visible_ = renderer->GetCurrentFrame();

		// Set visibility on all geometry instances in this cell.
		GeomSet::Iterator g_iter;
		for (g_iter = geom_set_.First(); g_iter != geom_set_.End(); ++g_iter) {
			tbc::GeometryBase* geom = *g_iter;
			geom->SetLastFrameVisible(renderer->GetCurrentFrame());
		}
	}

	// Traverse the graph recursively...
	PortalList::iterator p_iter;
	for (p_iter = portal_list_.begin(); p_iter != portal_list_.end(); ++p_iter) {
		Portal* portal = (Portal*)*p_iter;
		portal->Traverse(renderer, rect, this);
	}
}

PortalManager::Portal::Portal(int num_vertices,
			      vec3* vertex,
			      Cell* cell1,
			      Cell* cell2) :
	tbc::PortalManager::Portal(num_vertices, vertex, cell1, cell2) {
}

PortalManager::Portal::~Portal() {
}


PortalManager::Cell::Cell(const str& cell_id,
			  const str& cell_description,
			  PortalManager* portal_manager) :
	tbc::PortalManager::Cell(cell_id, cell_description, portal_manager),
	last_frame_visible_(-1) {
}

PortalManager::Cell::~Cell() {
}


loginstance(kUiGfx3D, PortalManager);

}
