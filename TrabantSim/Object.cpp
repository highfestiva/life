
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "Object.h"
#include "../UiTbc/Include/UiTriangleBasedGeometry.h"



namespace TrabantSim
{



Object::Object(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager):
	Parent(pResourceManager, pClassId, pUiManager),
	mGfxMesh(0),
	mGfxMeshId(0)
{
}

Object::~Object()
{
}



void Object::AddGfxMesh(const std::vector<float>& pVertices, const std::vector<int>& pIndices, const vec3& pColor)
{
	if (pVertices.empty() || pIndices.empty())
	{
		return;
	}
	UiTbc::TriangleBasedGeometry* lGfxMesh = new UiTbc::TriangleBasedGeometry(&pVertices[0], 0, 0, 0, UiTbc::TriangleBasedGeometry::COLOR_RGB, (const uint32*)&pIndices[0],
			pVertices.size()/3, pIndices.size(), Tbc::GeometryBase::TRIANGLES, Tbc::GeometryBase::GEOM_STATIC);
	lGfxMesh->GetBasicMaterialSettings().mDiffuse	= pColor;
	lGfxMesh->GetBasicMaterialSettings().mSpecular	= vec3();
	lGfxMesh->GetBasicMaterialSettings().mShininess	= true;
	lGfxMesh->GetBasicMaterialSettings().mSmooth	= false;
	lGfxMesh->SetGeometryVolatility(Tbc::GeometryBase::GEOM_SEMI_STATIC);
	AddMeshResource(lGfxMesh);
}



}
