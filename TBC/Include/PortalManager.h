
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

#include "TBC.h"
#include "../../Lepra/Include/Graphics2D.h"
#include "../../Lepra/Include/HashSet.h"
#include "../../Lepra/Include/HashTable.h"
#include "../../Lepra/Include/Vector3D.h"
#include "../../Lepra/Include/String.h"
#include "../../Lepra/Include/String.h"
#include <list>



namespace TBC
{



class GeometryBase;



class PortalManager
{
public:

	friend class GeometryBase;

	friend void Init();
	friend void Shutdown();

	PortalManager();
	virtual ~PortalManager();

	void ClearAll();

	// Creates a new empty cell with the given ID.
	bool AddCell(const str& pCellID, const str& pCellDescription);

	// Adds a portal. Both cells must exist. Cell1 is the cell "in front" of the portal -
	// in the direction of the surface normal.
	bool AddPortal(int pNumVertices,
			   Vector3DF* pVertex,
			   const str& pCellID1,
			   const str& pCellID2);

	// Adds the geometry to the given cell.
	bool AddGeometry(GeometryBase* pGeometry, const str& pParentCellID);
	void RemoveGeometry(GeometryBase* pGeometry);

	str GetParentCellID(GeometryBase* pGeometry);

	// Test collision against portals in the geometry's parent cell.
	// Testing as if moving from pFromPos to pToPos (both given in world 
	// coordinates), and updates the geometry's parent cell if a collision 
	// occurs. Returns the ID of the geometry's parent cell after collision.
	str TestPortalCollision(const Vector3DF& pFromPos,
					   const Vector3DF& pToPos,
					   GeometryBase* pGeometry);

	// Test collision against portals in the cell with the given ID.
	// Testing as if moving from pFromPos to pToPos (both given in world
	// coordinates). Returns the ID of the cell after collision.
	str TestPortalCollision(const Vector3DF& pFromPos,
					   const Vector3DF& pToPos,
					   const str& pCellID);

	// This function is implemented in UiPortalManager.
	// void TraverseGraph(Renderer* pRenderer, const str& pCellID);

protected:

	class Cell;
	class Portal
	{
	public:
		Portal(int pNumVertices,
		       Vector3DF* pVertex,
		       Cell* pCell1,
		       Cell* pCell2);
		virtual ~Portal();

		// Returns "true" on collision and the time to collision.
		bool TestCollision(const Vector3DF& pFromPos,
						   const Vector3DF& pToPos,
						   Cell* pFrom,
						   float& pTimeToCollision);

		Cell* GetOtherCell(Cell* pCell);

	protected:
		int mNumVertices;

		Vector3DF* mVertex;
		// Generated normals which all point towards the center of the portal.
		Vector3DF* mEdgeNormal;
		float* mEdgeD; // The fourth constant in the plane equation.

		Cell* mCell1;
		Cell* mCell2;

		Vector3DF mNormal;
		float mD;
	};

	class Cell
	{
	public:

		Cell(const str& pCellID, const str& pCellDescription, PortalManager* pPortalManager);
		virtual ~Cell();

		const str& GetID();
		const str& GetDescription();

		void AddPortal(Portal* pPortal);
		void RemovePortal(Portal* pPortal);

		void AddGeometry(GeometryBase* pGeometry);
		void RemoveGeometry(GeometryBase* pGeometry);

		Cell* TestPortalCollision(const Vector3DF& pFromPos,
					  const Vector3DF& pToPos,
					  float& pTimeToCollision);
	protected:

		typedef HashSet<GeometryBase*, LEPRA_VOIDP_HASHER> GeomSet;
		typedef std::list<Portal*> PortalList;

		GeomSet mGeomSet;
		PortalList mPortalList;
		str mCellID;
		str mCellDescription;
		PortalManager* mPortalManager;
	};

	typedef HashSet<GeometryBase*, LEPRA_VOIDP_HASHER> GeomSet;
	typedef HashTable<str, Cell*> CellTable;
	typedef std::list<Portal*> PortalList;

	virtual Portal* NewPortal(int pNumVertices,
				 Vector3DF* pVertex,
				 Cell* pCell1,
				 Cell* pCell2);
	virtual Cell* NewCell(const str& pCellID, 
			      const str& pCellDescription,
			      PortalManager* pPortalManager);

	GeomSet mGeomSet;
	CellTable mCellTable;
	PortalList mPortalList;

	// Initialized in TBC::Init().
	static str* smInvalidCellID;

	LOG_CLASS_DECLARE();
};



}
