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

#include "../../Tbc/Include/PortalManager.h"
#include "../../Lepra/Include/String.h"
#include "../Include/UiTbc.h"



namespace UiTbc
{



class Renderer;



class PortalManager: public Tbc::PortalManager
{
public:

	PortalManager(){}
	virtual ~PortalManager(){}

	void TraverseGraph(Renderer* pRenderer, const str& pCellID);

private:

	class Cell;

	class Portal : public Tbc::PortalManager::Portal
	{
	public:
		Portal(int pNumVertices,
		       vec3* pVertex,
		       Cell* pCell1,
		       Cell* pCell2);
		virtual ~Portal();

		void Traverse(Renderer* pRenderer, const PixelRect& pRect, Cell* pFrom);
	};

	class Cell: public Tbc::PortalManager::Cell
	{
	public:
		Cell(const str& pCellID, const str& pCellDescription, PortalManager* pPortalManager);
		virtual ~Cell();

		void Traverse(Renderer* pRenderer, const PixelRect& pRect);

	private:
		int mLastFrameVisible;
	};

	virtual Tbc::PortalManager::Portal* NewPortal(int pNumVertices,
						      vec3* pVertex,
						      Cell* pCell1,
						      Cell* pCell2);
	virtual Tbc::PortalManager::Cell* NewCell(const str& pCellID, 
						  const str& pCellDescription,
						  PortalManager* pPortalManager);

	logclass();
};



}
