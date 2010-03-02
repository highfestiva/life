
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../../Lepra/Include/Graphics2D.h"
#include "../../UiCure/Include/UiCppContextObject.h"
#include "../../UiTBC/Include/GUI/UiCustomButton.h"
#include "../Life.h"



namespace Life
{



class RoadSignButton: public UiCure::CppContextObject
{
	typedef UiCure::CppContextObject Parent;
public:
	enum Shape
	{
		SHAPE_BOX = 1,
		SHAPE_ROUND,
	};

	RoadSignButton(Cure::ResourceManager* pResourceManager, UiCure::GameUiManager* pUiManager,
		const str& pName, const str& pMeshResourceName, Shape pShape);
	virtual ~RoadSignButton();

	UiTbc::CustomButton& GetButton();
	void SetTrajectory(float pAngle, const PixelCoord& pEndPoint, float pEndZ, float pTime);

private:
	void RenderButton(UiTbc::CustomButton* pButton);
	bool IsOverButton(UiTbc::CustomButton* pButton, int x, int y);
	void OnLoadMesh(UiCure::UserGeometryReferenceResource* pMeshResource);

	UiTbc::CustomButton mButton;
	UiCure::UserGeometryReferenceResource mSignMesh;
	Shape mShape;

	LOG_CLASS_DECLARE();
};



}
