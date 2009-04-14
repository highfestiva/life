/*
	Class:  PortalManager
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games

	NOTES:

	This class constitutes a portal/cell graph which can be used to
	perform portal culling in an indoor scene. The actual culling
	algorithm is implemented in UiPortalManager.h though.

	All portals are convex polygons. Concave polygons will result in errors.

	Performance hint: Keep the number of portals per cell as low as 
	possible. All portals in a cell are tested for collision when 
	calling TestPortalCollision().
*/

#ifndef PORTALMANAGER_H
#define PORTALMANAGER_H

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
	bool AddCell(const Lepra::String& pCellID, const Lepra::String& pCellDescription);

	// Adds a portal. Both cells must exist. Cell1 is the cell "in front" of the portal -
	// in the direction of the surface normal.
	bool AddPortal(int pNumVertices,
			   Lepra::Vector3DF* pVertex,
			   const Lepra::String& pCellID1,
			   const Lepra::String& pCellID2);

	// Adds the geometry to the given cell.
	bool AddGeometry(GeometryBase* pGeometry, const Lepra::String& pParentCellID);
	void RemoveGeometry(GeometryBase* pGeometry);

	Lepra::String GetParentCellID(GeometryBase* pGeometry);

	// Test collision against portals in the geometry's parent cell.
	// Testing as if moving from pFromPos to pToPos (both given in world 
	// coordinates), and updates the geometry's parent cell if a collision 
	// occurs. Returns the ID of the geometry's parent cell after collision.
	Lepra::String TestPortalCollision(const Lepra::Vector3DF& pFromPos,
					   const Lepra::Vector3DF& pToPos,
					   GeometryBase* pGeometry);

	// Test collision against portals in the cell with the given ID.
	// Testing as if moving from pFromPos to pToPos (both given in world
	// coordinates). Returns the ID of the cell after collision.
	Lepra::String TestPortalCollision(const Lepra::Vector3DF& pFromPos,
					   const Lepra::Vector3DF& pToPos,
					   const Lepra::String& pCellID);

	// This function is implemented in UiPortalManager.
	// void TraverseGraph(Renderer* pRenderer, const Lepra::String& pCellID);

protected:

	class Cell;
	class Portal
	{
	public:
		Portal(int pNumVertices,
		       Lepra::Vector3DF* pVertex,
		       Cell* pCell1,
		       Cell* pCell2);
		virtual ~Portal();

		// Returns "true" on collision and the time to collision.
		bool TestCollision(const Lepra::Vector3DF& pFromPos,
						   const Lepra::Vector3DF& pToPos,
						   Cell* pFrom,
						   float& pTimeToCollision);

		Cell* GetOtherCell(Cell* pCell);

	protected:
		int mNumVertices;

		Lepra::Vector3DF* mVertex;
		// Generated normals which all point towards the center of the portal.
		Lepra::Vector3DF* mEdgeNormal;
		float* mEdgeD; // The fourth constant in the plane equation.

		Cell* mCell1;
		Cell* mCell2;

		Lepra::Vector3DF mNormal;
		float mD;
	};

	class Cell
	{
	public:

		inline Cell(const Lepra::String& pCellID, 
			    const Lepra::String& pCellDescription,
			    PortalManager* pPortalManager);
		inline virtual ~Cell();

		inline const Lepra::String& GetID();
		inline const Lepra::String& GetDescription();

		inline void AddPortal(Portal* pPortal);
		inline void RemovePortal(Portal* pPortal);

		inline void AddGeometry(GeometryBase* pGeometry);
		inline void RemoveGeometry(GeometryBase* pGeometry);

		Cell* TestPortalCollision(const Lepra::Vector3DF& pFromPos,
					  const Lepra::Vector3DF& pToPos,
					  float& pTimeToCollision);
	protected:

		typedef Lepra::HashSet<GeometryBase*, std::hash<void*> > GeomSet;
		typedef std::list<Portal*> PortalList;

		GeomSet mGeomSet;
		PortalList mPortalList;
		Lepra::String mCellID;
		Lepra::String mCellDescription;
		PortalManager* mPortalManager;
	};

	typedef Lepra::HashSet<GeometryBase*, std::hash<void*> > GeomSet;
	typedef Lepra::HashTable<Lepra::String, Cell*> CellTable;
	typedef std::list<Portal*> PortalList;

	virtual Portal* NewPortal(int pNumVertices,
				 Lepra::Vector3DF* pVertex,
				 Cell* pCell1,
				 Cell* pCell2);
	virtual Cell* NewCell(const Lepra::String& pCellID, 
			      const Lepra::String& pCellDescription,
			      PortalManager* pPortalManager);

	GeomSet mGeomSet;
	CellTable mCellTable;
	PortalList mPortalList;

	// Initialized in TBC::Init().
	static Lepra::String* smInvalidCellID;

	LOG_CLASS_DECLARE();
};

} // End namespace.

#endif
