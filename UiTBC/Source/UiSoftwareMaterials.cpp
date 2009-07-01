/*
	Class:  -
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games
*/

#include "../../Lepra/Include/Log.h"
#include "../../TBC/Include/../../TBC/Include/GeometryBase.h"
#include "../Include/UiMaterial.h"
#include "../Include/UiSoftware3DPainter.h"
#include "../Include/UiSoftwareMaterials.h"
#include "../Include/UiSoftwareRenderer.h"

namespace UiTbc
{

Software3DPainter::RenderingPrimitive SoftwareMaterial::smPrimitiveLUT[2] = {Software3DPainter::RENDER_TRIANGLES, Software3DPainter::RENDER_TRIANGLE_STRIP};

Material::RemoveStatus SoftwareMaterial::RemoveGeometry(TBC::GeometryBase* pGeometry)
{
	Material::RemoveStatus lRemoved = Material::RemoveGeometry(pGeometry);
	if (lRemoved == NOT_REMOVED && mFallBackMaterial)
	{
		lRemoved = mFallBackMaterial->RemoveGeometry(pGeometry);
	}
	return (lRemoved);
}

void SoftwareMaterial::RenderAllGeometry(unsigned int pCurrentFrame)
{
	if (m3DPainter != 0)
		Material::RenderAllGeometry(pCurrentFrame);
}






void SoftMatSingleColorSolid::RenderGeometry(TBC::GeometryBase* pGeometry)
{
	m3DPainter->SetAlphaBlendEnabled(false);
	m3DPainter->SetAlphaTestEnabled(false);
	m3DPainter->SetTextureMappingEnabled(false);

	const TBC::GeometryBase::BasicMaterialSettings& lMatSettings = pGeometry->GetBasicMaterialSettings();
	m3DPainter->SetDefaultColor(lMatSettings.mRed, lMatSettings.mGreen, lMatSettings.mBlue);

	m3DPainter->SetVertexPointer(pGeometry->GetVertexCount(), pGeometry->GetVertexData());
	m3DPainter->SetNormalPointer(pGeometry->GetNormalData());
	m3DPainter->SetColorPointer((float*)0);
	m3DPainter->SetColorPointer((unsigned char*)0);
	m3DPainter->SetUVPointer(0);
	m3DPainter->DrawArrayElements(pGeometry->GetIndexData(), 
					pGeometry->GetIndexCount(), 
					GetElementType(pGeometry));
}

void SoftMatVertexColorSolid::RenderGeometry(TBC::GeometryBase* pGeometry)
{
	m3DPainter->SetVertexColorsEnabled(true);

	m3DPainter->SetVertexPointer(pGeometry->GetVertexCount(), pGeometry->GetVertexData());
	m3DPainter->SetNormalPointer(pGeometry->GetNormalData());
	m3DPainter->SetColorPointer((float*)0);
	m3DPainter->SetColorPointer(pGeometry->GetColorData());
	m3DPainter->SetUVPointer(0);
	m3DPainter->DrawArrayElements(pGeometry->GetIndexData(), 
					pGeometry->GetIndexCount(), 
					GetElementType(pGeometry));
}

void SoftMatSingleTextureSolid::RenderGeometry(TBC::GeometryBase* pGeometry)
{
	m3DPainter->SetVertexPointer(pGeometry->GetVertexCount(), pGeometry->GetVertexData());
	m3DPainter->SetNormalPointer(pGeometry->GetNormalData());
	m3DPainter->SetColorPointer((float*)0);
	m3DPainter->SetColorPointer((unsigned char*)0);
	m3DPainter->SetUVPointer(pGeometry->GetUVData(0));
	m3DPainter->SetTextureMappingEnabled(true);
	m3DPainter->DrawArrayElements(pGeometry->GetIndexData(), 
					pGeometry->GetIndexCount(), 
					GetElementType(pGeometry));
}




bool SoftMatSingleTextureSolid::AddGeometry(TBC::GeometryBase* pGeometry)
{
	if (pGeometry->GetUVData(0) != 0)
	{
		return Material::AddGeometry(pGeometry);
	}
	else if(GetRenderer()->GetFallbackMaterialEnabled())
	{
		//log_warning(_T("SoftwareMaterial"), _T("Material \"SingleTextureSolid\", passing geometry to fallback material."));
		return mFallBackMaterial->AddGeometry(pGeometry);
	}
	else
	{
		return false;
	}
}

} // End namespace.
