/*
	File:   PortalManager.cpp
	Class:  PortalManager
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games
*/

#include "../Include/PortalManager.h"
#include "../Include/GeometryBase.h"
#include "../../Lepra/Include/Log.h"

namespace TBC
{

PortalManager::PortalManager()
{
}

PortalManager::~PortalManager()
{
	ClearAll();
}

void PortalManager::ClearAll()
{
	CellTable::Iterator lCIter;
	for (lCIter = mCellTable.First(); lCIter != mCellTable.End(); ++lCIter)
	{
		Cell* lCell = *lCIter;
		delete lCell;
	}

	mCellTable.RemoveAll();

	PortalList::iterator lPIter;
	for (lPIter = mPortalList.begin(); lPIter != mPortalList.end(); ++lPIter)
	{
		Portal* lPortal = *lPIter;
		delete lPortal;
	}

	mPortalList.clear();
	mGeomSet.RemoveAll();
}

PortalManager::Portal* PortalManager::NewPortal(int pNumVertices,
						Vector3DF* pVertex,
						Cell* pCell1,
						Cell* pCell2)
{
	return new Portal(pNumVertices, pVertex, pCell1, pCell2);
}

PortalManager::Cell* PortalManager::NewCell(const str& pCellID, 
					    const str& pCellDescription,
					    PortalManager* pPortalManager)
{
	return new Cell(pCellID, pCellDescription, pPortalManager);
}

bool PortalManager::AddCell(const str& pCellID, const str& pCellDescription)
{
	CellTable::Iterator lIter;
	lIter = mCellTable.Find(pCellID);

	if (lIter != mCellTable.End())
	{
		mLog.Error(_T("AddCell() - Cell \"") + pCellID + _T("\" already exist!"));
		return false;
	}

	Cell* lCell = NewCell(pCellID, pCellDescription, this);
	mCellTable.Insert(pCellID, lCell);

	return true;
}

bool PortalManager::AddPortal(int pNumVertices,
			      Vector3DF* pVertex,
			      const str& pCellID1,
			      const str& pCellID2)
{
	if (pNumVertices < 3)
	{
		mLog.AError("AddPortal() - Can't add a portal with less than 3 vertices!");
	}

	CellTable::Iterator lIter1;
	CellTable::Iterator lIter2;
	lIter1 = mCellTable.Find(pCellID1);
	lIter2 = mCellTable.Find(pCellID2);

	if (lIter1 == mCellTable.End())
	{
		mLog.Error(_T("AddCell() - Cell \"") + pCellID1 + _T("\" does not exist!"));
		return false;
	}
	if (lIter2 == mCellTable.End())
	{
		mLog.Error(_T("AddCell() - Cell \"") + pCellID2 + _T("\" does not exist!"));
		return false;
	}

	Cell* lCell1 = *lIter1;
	Cell* lCell2 = *lIter2;

	Portal* lPortal = NewPortal(pNumVertices, pVertex, lCell1, lCell2);
	mPortalList.push_back(lPortal);

	lCell1->AddPortal(lPortal);
	lCell2->AddPortal(lPortal);
	return true;
}

bool PortalManager::AddGeometry(GeometryBase* pGeometry, const str& pParentCellID)
{
	if (mGeomSet.Find(pGeometry) != mGeomSet.End())
	{
		mLog.AError("AddGeometry() - Geometry already added!");
		return false;
	}

	CellTable::Iterator lIter;
	lIter = mCellTable.Find(pParentCellID);

	if (lIter == mCellTable.End())
	{
		mLog.Error(_T("AddGeometry() - Cell \"") + pParentCellID + _T("\" does not exist!"));
		return false;
	}

	Cell* lCell = *lIter;
	lCell->AddGeometry(pGeometry);

	pGeometry->SetParentCell(lCell);

	mGeomSet.Insert(pGeometry);

	return true;
}

void PortalManager::RemoveGeometry(GeometryBase* pGeometry)
{
	Cell* lCell = (Cell*)pGeometry->GetParentCell();
	if (lCell != 0)
	{
		lCell->RemoveGeometry(pGeometry);
	}
	else
	{
		mLog.AWarning("RemoveGeometry() - Parent Cell was NULL!");
	}

	mGeomSet.Remove(pGeometry);
	pGeometry->SetParentCell(0);
}

str PortalManager::GetParentCellID(GeometryBase* pGeometry)
{
	Cell* lCell = (Cell*)pGeometry->GetParentCell();
	if (lCell != 0)
	{
		return lCell->GetID();
	}
	else
	{
		mLog.AWarning("GetParentCellID() - Parent Cell was NULL!");
		return *smInvalidCellID;
	}
}

str PortalManager::TestPortalCollision(const Vector3DF& pFromPos,
						  const Vector3DF& pToPos,
						  GeometryBase* pGeometry)
{
	Cell* lCell = (Cell*)pGeometry->GetParentCell();
	if (lCell == 0)
	{
		mLog.AWarning("TestPortalCollision() - Parent Cell was NULL!");
		return *smInvalidCellID;
	}

	Vector3DF lFromPos(pFromPos);

	// Loop as long as we are colliding with portals.
	for (;;)
	{
		float lTTC = 0;
		Cell* lOtherCell = lCell->TestPortalCollision(lFromPos, pToPos, lTTC);
		if (lOtherCell != 0 && lOtherCell != lCell)
		{
			// We have collided. Step into the other cell and update the step vector.
			lCell = lOtherCell;
			Vector3DF lDiff(pToPos - lFromPos);
			lFromPos += lDiff * lTTC;
		}
		else
		{
			break;
		}
	}

	pGeometry->SetParentCell(lCell);
	return lCell->GetID();
}

str PortalManager::TestPortalCollision(const Vector3DF& pFromPos,
						  const Vector3DF& pToPos,
						  const str& pCellID)
{
	CellTable::Iterator lIter = mCellTable.Find(pCellID);
	if (lIter == mCellTable.End())
	{
		mLog.Warning(_T("TestPortalCollision() - CellID \"") + pCellID + _T("\" does not exist!"));
		return *smInvalidCellID;
	}

	Cell* lCell = *lIter;

	Vector3DF lFromPos(pFromPos);

	// Loop as long as we are colliding with portals.
	for (;;)
	{
		float lTTC = 0;
		Cell* lOtherCell = lCell->TestPortalCollision(lFromPos, pToPos, lTTC);
		if (lOtherCell != 0 && lOtherCell != lCell)
		{
			// We have collided. Step into the other cell and update the step vector.
			lCell = lOtherCell;
			Vector3DF lDiff(pToPos - lFromPos);
			lFromPos += lDiff * lTTC;
		}
		else
		{
			break;
		}
	}

	return lCell->GetID();
}

PortalManager::Cell* PortalManager::Cell::TestPortalCollision(const Vector3DF& pFromPos,
							      const Vector3DF& pToPos,
							      float& pTimeToCollision)
{
	PortalList::iterator lIter;

	Cell* lReturnCell = this;

	// We have to loop through all portals in order to find the first collision.
	// This gives us a hint: Don't add too many portals to a cell!
	for (lIter = mPortalList.begin(); lIter != mPortalList.end(); ++lIter)
	{
		Portal* lPortal = *lIter;
		float lTTC;
		if (lPortal->TestCollision(pFromPos, pToPos, this, lTTC) == true)
		{
			// Only consider the first collision.
			if (lReturnCell == this || lTTC < pTimeToCollision)
			{
				lReturnCell = lPortal->GetOtherCell(this);

				if (lReturnCell == 0)
				{
					mLog.AFatal("TestPortalCollision() - Portal graph is corrupted!");
					return 0;
				}

				pTimeToCollision = lTTC;
			}
		}
	}

	return lReturnCell;
}



PortalManager::Portal::Portal(int pNumVertices,
			      Vector3DF* pVertex,
			      Cell* pCell1,
			      Cell* pCell2) :
	mNumVertices(pNumVertices),
	mEdgeNormal(0),
	mEdgeD(0),
	mCell1(pCell1),
	mCell2(pCell2)
{
	// Make sure to generate a valid normal. We assume that PortalManager
	// never creates a Portal if there are less than 3 vertices.
	int i = 2;
	do
	{
		mNormal.CrossUnit(pVertex[i] - pVertex[0], pVertex[1] - pVertex[0]);
		i++;
	}while(mNormal.GetLength() < 0.99f && i < pNumVertices);

	mD = -mNormal.Dot(pVertex[0]);

	mVertex     = new Vector3DF[mNumVertices];
	mEdgeNormal = new Vector3DF[mNumVertices];
	mEdgeD = new float[mNumVertices];

	for (i = 0; i < mNumVertices; i++)
	{
		mVertex[i] = pVertex[i];

		// Generate collision test normals around the portal, all pointing to the
		// center of the portal.
		int lNext = (i + 1) % mNumVertices;
		mEdgeNormal[i].CrossUnit(pVertex[lNext] - pVertex[i], mNormal);
		mEdgeD[i] = -mEdgeNormal[i].Dot(pVertex[i]);
	}
}

PortalManager::Portal::~Portal()
{
	delete[] mVertex;
	delete[] mEdgeNormal;
	delete[] mEdgeD;
}

PortalManager::Cell* PortalManager::Portal::GetOtherCell(Cell* pCell)
{
	if (pCell == mCell1)
		return mCell2;
	if (pCell == mCell2)
		return mCell1;

	return 0;
}

bool PortalManager::Portal::TestCollision(const Vector3DF& pFromPos,
					  const Vector3DF& pToPos,
					  Cell* pFrom,
					  float& pTimeToCollision)
{
	pTimeToCollision = 1.0f;

	float lTest1 = mNormal.Dot(pFromPos) + mD;
	float lTest2 = mNormal.Dot(pToPos)   + mD;

	// If both points are on the same side of the plane...
	if ((lTest1 >= 0 && lTest2 >= 0) ||
	   (lTest1 <= 0 && lTest2 <= 0))
	{
		return false;
	}

	// Check if we are entering the plane from the "wrong" direction.
	if (pFrom == mCell1)
	{
		if (lTest1 <= 0 && lTest2 >= 0)
		{
			return false;
		}
	}
	else if(pFrom == mCell2)
	{
		if (lTest1 >= 0 && lTest2 <= 0)
		{
			return false;
		}
	}
	else
	{
		mLog.AError("Portal::TestCollision() - Invalid cell pointer!");
		return false;
	}

	// The vector is crossing the plane. Find the point of intersection (POI).
	Vector3DF lDiff(pToPos - pFromPos);
	float lTTC = (mNormal.Dot(pToPos) + mD) / mNormal.Dot(lDiff);
	
	Vector3DF lPOI(pFromPos + lDiff * lTTC);

	// Now check if the POI is on the portal polygon.
	for (int i = 0; i < mNumVertices; i++)
	{
		float lTest = mEdgeNormal[i].Dot(lPOI) + mEdgeD[i];

		if (lTest < 0)
		{
			return false;
		}
	}

	pTimeToCollision = lTTC;
	return true;
}



PortalManager::Cell::Cell(const str& pCellID, const str& pCellDescription, PortalManager* pPortalManager) :
	mCellID(pCellID),
	mCellDescription(pCellDescription),
	mPortalManager(pPortalManager)
{
}

PortalManager::Cell::~Cell()
{
}

const str& PortalManager::Cell::GetID()
{
	return mCellID;
}

const str& PortalManager::Cell::GetDescription()
{
	return mCellDescription;
}

void PortalManager::Cell::AddPortal(Portal* pPortal)
{
	mPortalList.push_back(pPortal);
}

void PortalManager::Cell::RemovePortal(Portal* pPortal)
{
	mPortalList.remove(pPortal);
}

void PortalManager::Cell::AddGeometry(GeometryBase* pGeometry)
{
	mGeomSet.Insert(pGeometry);
}

void PortalManager::Cell::RemoveGeometry(GeometryBase* pGeometry)
{
	mGeomSet.Remove(pGeometry);
}



// Initialized in TBC::Init().
str* PortalManager::smInvalidCellID = 0;

LOG_CLASS_DEFINE(UI_GFX_3D, PortalManager);

}
