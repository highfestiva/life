
// Author: Jonas Byström
// Copyright (c) 2002-2011, Righteous Games



#pragma once
#include "../../Lepra/Include/CubicDeCasteljauSpline.h"
#include "../../Lepra/Include/Graphics2D.h"
#include "../../Lepra/Include/Thread.h"
#include "../../Lepra/Include/Vector3D.h"
#include "UiCure.h"



namespace Cure
{
class ContextManager;
class RuntimeVariableScope;
}



namespace UiCure
{



class GameUiManager;



class DebugRenderer
{
public:
	typedef CubicDeCasteljauSpline<Vector3DF, float> Spline;

	DebugRenderer(const Cure::RuntimeVariableScope* pVariableScope, const Cure::ContextManager* pContext, const Cure::ContextManager* pRemoteContext, Lock* pTickLock);
	virtual ~DebugRenderer();

	void Render(const GameUiManager* pUiManager, const PixelRect& pRenderArea);
	void RenderSpline(const GameUiManager* pUiManager, Spline* pSpline);

private:
	const Cure::RuntimeVariableScope* mVariableScope;
	const Cure::ContextManager* mContext;
	const Cure::ContextManager* mRemoteContext;
	Lock* mTickLock;
};



}
