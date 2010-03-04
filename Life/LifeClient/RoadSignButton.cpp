
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "RoadSignButton.h"
#include "../../UiCure/Include/UiGameUiManager.h"
#include "../../UiTBC/Include/GUI/UiDesktopWindow.h"



namespace Life
{



RoadSignButton::RoadSignButton(Cure::ResourceManager* pResourceManager, UiCure::GameUiManager* pUiManager,
	const str& pName, const str& pMeshResourceName, Shape pShape):
	Parent(_T("RoadSignButton"), pUiManager),
	mButton(pName),
	mSignMesh(pUiManager),
	mShape(pShape)
{
	mButton.SetText(pName);
	mButton.SetPreferredSize(40, 40);
	mButton.SetMinSize(20, 20);
	GetUiManager()->GetDesktopWindow()->AddChild(&mButton);
	static int x = 0, y = 0;
	//mButton.SetPos(-50, -50);
	mButton.SetPos(x, y);
	x += 40;
	if (x >= 600)
	{
		x = 0;
		y += 40;
	}

	mSignMesh.Load(pResourceManager, pMeshResourceName,
		UiCure::UserGeometryReferenceResource::TypeLoadCallback(this, &RoadSignButton::OnLoadMesh));

	mButton.SetOnRender(RoadSignButton, RenderButton);
	mButton.SetOnIsOver(RoadSignButton, IsOverButton);
}

RoadSignButton::~RoadSignButton()
{
	GetUiManager()->GetDesktopWindow()->RemoveChild(&mButton, 0);
}



UiTbc::CustomButton& RoadSignButton::GetButton()
{
	return (mButton);
}

void RoadSignButton::SetTrajectory(float pAngle, const PixelCoord& pEndPoint, float pEndZ, float pTime)
{
	pAngle;
	pEndPoint;
	pEndZ;
	pTime;
}



void RoadSignButton::RenderButton(UiTbc::CustomButton* pButton)
{
	if (mSignMesh.GetLoadState() != Cure::RESOURCE_LOAD_COMPLETE)
	{
		return;
	}

	GetUiManager()->GetPainter()->PushAttrib(UiTbc::Painter::ATTR_ALL);
	PixelRect lRect(pButton->GetClientRect());
	GetUiManager()->GetPainter()->ReduceClippingRect(lRect);
	pButton->Window::Repaint(GetUiManager()->GetPainter());
	pButton->PrintText(GetUiManager()->GetPainter(),
		lRect.mLeft + (lRect.GetWidth() - GetUiManager()->GetPainter()->GetStringWidth(pButton->GetText().c_str())) / 2,
		lRect.mTop + (lRect.GetHeight() - GetUiManager()->GetPainter()->GetFontHeight()) / 2);

	TBC::GeometryBase* lGfxGeometry = mSignMesh.GetRamData();
	TransformationF lTransform;
	lTransform.SetPosition(Vector3DF(0, 10, 0));
	lGfxGeometry->SetTransformation(lTransform);
	GetUiManager()->GetRenderer()->RenderRelative(lGfxGeometry);

	GetUiManager()->GetPainter()->PopAttrib();
}

bool RoadSignButton::IsOverButton(UiTbc::CustomButton* pButton, int x, int y)
{
	if (mShape == SHAPE_ROUND)
	{
		PixelCoord lMiddle(pButton->GetPos() + pButton->GetSize()/2);
		return (lMiddle.GetDistance(PixelCoord(x, y)) < 20);
	}
	return (pButton->Button::IsOver(x, y));
}

void RoadSignButton::OnLoadMesh(UiCure::UserGeometryReferenceResource* pMeshResource)
{
	if (pMeshResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		pMeshResource->GetRamData()->SetAlwaysVisible(false);
		TBC::GeometryBase::BasicMaterialSettings lMaterial(Vector3DF(1,0,0), Vector3DF(0,1,0),
			Vector3DF(0,0,1), 0.5, 1, true);
		pMeshResource->GetRamData()->SetBasicMaterialSettings(lMaterial);
		UiTbc::Renderer::MaterialType lMaterialType = UiTbc::Renderer::MAT_SINGLE_COLOR_SOLID;
		GetUiManager()->GetRenderer()->ChangeMaterial(pMeshResource->GetData(), lMaterialType);
	}
	else
	{
		mLog.AError("Could not load road sign mesh! Shit.");
		assert(false);
	}
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, RoadSignButton);



}
