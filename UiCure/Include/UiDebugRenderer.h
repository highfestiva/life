
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once
#include "../../Lepra/Include/CubicDeCasteljauSpline.h"
#include "../../Lepra/Include/Graphics2D.h"
#include "../../Lepra/Include/Thread.h"
#include "../../Lepra/Include/Vector3D.h"
#include "UiCure.h"



namespace Cure
{
class ContextManager;
class ContextObject;
class RuntimeVariableScope;
}



namespace UiCure
{



class GameUiManager;



class DebugRenderer
{
public:
	enum DebugPrimitive
	{
		DEBUG_AXES = 1,
		DEBUG_JOINTS,
		DEBUG_SHAPES,
	};

	typedef CubicDeCasteljauSpline<vec3, float> Spline;

	DebugRenderer(const Cure::RuntimeVariableScope* pVariableScope, GameUiManager* pUiManager, const Cure::ContextManager* pContext, const Cure::ContextManager* pRemoteContext, Lock* pTickLock);
	virtual ~DebugRenderer();

	void Render(const GameUiManager* pUiManager, const PixelRect& pRenderArea);
	void DebugDrawPrimitive(Cure::ContextObject* pObject, DebugPrimitive pPrimitive);
	void RenderSpline(const GameUiManager* pUiManager, Spline* pSpline);

private:
	const Cure::RuntimeVariableScope* mVariableScope;
	GameUiManager* mUiManager;
	const Cure::ContextManager* mContext;
	const Cure::ContextManager* mRemoteContext;
	Lock* mTickLock;
};



}
