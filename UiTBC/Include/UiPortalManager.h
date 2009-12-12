/*
	Class:  PortalManager
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games

	NOTES:

	This class constitutes a portal/cell graph which can be used to
	perform portal culling in an indoor scene. 

	All portals are convex polygons. Concave polygons will result in errors.

	Performance hint: Keep the number of portals per cell as low as 
	possible. All portals in a cell are tested for collision when 
	calling TestPortalCollision().
*/

#ifndef UIPORTALMANAGER_H
#define UIPORTALMANAGER_H

#include "../../TBC/Include/PortalManager.h"
#include "../../Lepra/Include/String.h"

namespace UiTbc
{

class Renderer;

class PortalManager: public TBC::PortalManager
{
public:

	PortalManager(){}
	virtual ~PortalManager(){}

	void TraverseGraph(Renderer* pRenderer, const Lepra::String& pCellID);

private:

	class Cell;

	class Portal : public TBC::PortalManager::Portal
	{
	public:
		Portal(int pNumVertices,
		       Lepra::Vector3DF* pVertex,
		       Cell* pCell1,
		       Cell* pCell2);
		virtual ~Portal();

		void Traverse(Renderer* pRenderer, const Lepra::PixelRect& pRect, Cell* pFrom);
	};

	class Cell : public TBC::PortalManager::Cell
	{
	public:
		inline Cell(const Lepra::String& pCellID, 
			    const Lepra::String& pCellDescription,
			    PortalManager* pPortalManager);
		inline virtual ~Cell();

		void Traverse(Renderer* pRenderer, const Lepra::PixelRect& pRect);

	private:
		int mLastFrameVisible;
	};

	virtual TBC::PortalManager::Portal* NewPortal(int pNumVertices,
						      Lepra::Vector3DF* pVertex,
						      Cell* pCell1,
						      Cell* pCell2);
	virtual TBC::PortalManager::Cell* NewCell(const Lepra::String& pCellID, 
						  const Lepra::String& pCellDescription,
						  PortalManager* pPortalManager);

	LOG_CLASS_DECLARE();
};

} // End namespace.

#endif
