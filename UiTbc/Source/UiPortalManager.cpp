/*
	Class:  PortalManager
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#include "pch.h"
#include "../Include/UiPortalManager.h"
#include "../Include/UiRenderer.h"
#include "../../Lepra/Include/Log.h"
#include "../../Tbc/Include/GeometryBase.h"

namespace UiTbc
{

Tbc::PortalManager::Portal* PortalManager::NewPortal(int pNumVertices,
						     vec3* pVertex,
						     Parent::Cell* pCell1,
						     Parent::Cell* pCell2)
{
	return new Portal(pNumVertices, pVertex, (Cell*)pCell1, (Cell*)pCell2);
}

Tbc::PortalManager::Cell* PortalManager::NewCell(const str& pCellID, 
						 const str& pCellDescription,
						 Parent::PortalManager* pPortalManager)
{
	return new Cell(pCellID, pCellDescription, (PortalManager*)pPortalManager);
}

void PortalManager::TraverseGraph(Renderer* pRenderer, const str& pCellID)
{
	CellTable::Iterator lIter = mCellTable.Find(pCellID);
	if (lIter == mCellTable.End())
	{
		mLog.Warning("TraverseGraph - CellID \"" + pCellID + "\" does not exist!");
		return;
	}

	Cell* lCell = (Cell*)*lIter;
	lCell->Traverse(pRenderer, pRenderer->GetClippingRect());
}

void PortalManager::Portal::Traverse(Renderer* pRenderer, const PixelRect& pRect, Cell* pFrom)
{
	PixelRect lRect(pRenderer->GetBoundingRect(mVertex, mNumVertices));
	bool lFront = pRenderer->IsFacingFront(mVertex, mNumVertices);

	// Stop the recursion if we try to traverse this portal from the "wrong" direction.
	if ((lFront == true  && pFrom == mCell1) ||
		(lFront == false && pFrom == mCell2))
	{
		lRect = lRect.GetOverlap(pRect);

		if (lRect.mLeft < lRect.mRight &&
		   lRect.mTop  < lRect.mBottom)
		{
			((Cell*)GetOtherCell(pFrom))->Traverse(pRenderer, lRect);
		}
	}
}

void PortalManager::Cell::Traverse(Renderer* pRenderer, const PixelRect& pRect)
{
	// If not already visited, tag all geometry. Since two cells can be attached 
	// to each other with more than one portal, we can't just return if the cell
	// has already been visited. We have to let Portal take care of the terminating
	// condition in this recursive process.
	if (mLastFrameVisible != (int)pRenderer->GetCurrentFrame())
	{
		// Tag this cell as "visited".
		mLastFrameVisible = pRenderer->GetCurrentFrame();

		// Set visibility on all geometry instances in this cell.
		GeomSet::Iterator lGIter;
		for (lGIter = mGeomSet.First(); lGIter != mGeomSet.End(); ++lGIter)
		{
			Tbc::GeometryBase* lGeom = *lGIter;
			lGeom->SetLastFrameVisible(pRenderer->GetCurrentFrame());
		}
	}

	// Traverse the graph recursively...
	PortalList::iterator lPIter;
	for (lPIter = mPortalList.begin(); lPIter != mPortalList.end(); ++lPIter)
	{
		Portal* lPortal = (Portal*)*lPIter;
		lPortal->Traverse(pRenderer, pRect, this);
	}
}

PortalManager::Portal::Portal(int pNumVertices,
			      vec3* pVertex,
			      Cell* pCell1,
			      Cell* pCell2) :
	Tbc::PortalManager::Portal(pNumVertices, pVertex, pCell1, pCell2)
{
}

PortalManager::Portal::~Portal()
{
}


PortalManager::Cell::Cell(const str& pCellID, 
			  const str& pCellDescription,
			  PortalManager* pPortalManager) :
	Tbc::PortalManager::Cell(pCellID, pCellDescription, pPortalManager),
	mLastFrameVisible(-1)
{
}

PortalManager::Cell::~Cell()
{
}


loginstance(UI_GFX_3D, PortalManager);

} // End namespace.
