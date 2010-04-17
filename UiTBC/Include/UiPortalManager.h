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



#pragma once

#include "../../TBC/Include/PortalManager.h"
#include "../../Lepra/Include/String.h"
#include "../Include/UiTBC.h"



namespace UiTbc
{



class Renderer;



class PortalManager: public TBC::PortalManager
{
public:

	PortalManager(){}
	virtual ~PortalManager(){}

	void TraverseGraph(Renderer* pRenderer, const str& pCellID);

private:

	class Cell;

	class Portal : public TBC::PortalManager::Portal
	{
	public:
		Portal(int pNumVertices,
		       Vector3DF* pVertex,
		       Cell* pCell1,
		       Cell* pCell2);
		virtual ~Portal();

		void Traverse(Renderer* pRenderer, const PixelRect& pRect, Cell* pFrom);
	};

	class Cell: public TBC::PortalManager::Cell
	{
	public:
		Cell(const str& pCellID, const str& pCellDescription, PortalManager* pPortalManager);
		virtual ~Cell();

		void Traverse(Renderer* pRenderer, const PixelRect& pRect);

	private:
		int mLastFrameVisible;
	};

	virtual TBC::PortalManager::Portal* NewPortal(int pNumVertices,
						      Vector3DF* pVertex,
						      Cell* pCell1,
						      Cell* pCell2);
	virtual TBC::PortalManager::Cell* NewCell(const str& pCellID, 
						  const str& pCellDescription,
						  PortalManager* pPortalManager);

	LOG_CLASS_DECLARE();
};



}
