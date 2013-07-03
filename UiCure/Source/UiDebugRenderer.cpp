
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../Include/UiDebugRenderer.h"
#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../UiTBC/Include/UiRenderer.h"
#include "../Include/UiCppContextObject.h"
#include "../Include/UiGameUiManager.h"
#include "../Include/UiRuntimeVariableName.h"



namespace UiCure
{



DebugRenderer::DebugRenderer(const Cure::RuntimeVariableScope* pVariableScope, const Cure::ContextManager* pContext, const Cure::ContextManager* pRemoteContext, Lock* pTickLock):
	mVariableScope(pVariableScope),
	mContext(pContext),
	mRemoteContext(pRemoteContext),
	mTickLock(pTickLock)
{
}

DebugRenderer::~DebugRenderer()
{
}



void DebugRenderer::Render(const GameUiManager* pUiManager, const PixelRect& pRenderArea)
{
	bool lDebugAxes;
	bool lDebugJoints;
	bool lDebugShapes;
	CURE_RTVAR_GET(lDebugAxes, =, GetSettings(), RTVAR_DEBUG_3D_ENABLEAXES, false);
	CURE_RTVAR_GET(lDebugJoints, =, GetSettings(), RTVAR_DEBUG_3D_ENABLEJOINTS, false);
	CURE_RTVAR_GET(lDebugShapes, =, GetSettings(), RTVAR_DEBUG_3D_ENABLESHAPES, false);
	if (lDebugAxes || lDebugJoints || lDebugShapes)
	{
		ScopeLock lLock(mTickLock);
		pUiManager->GetRenderer()->ResetClippingRect();
		pUiManager->GetRenderer()->SetClippingRect(pRenderArea);
		pUiManager->GetRenderer()->SetViewport(pRenderArea);

		const Cure::ContextManager::ContextObjectTable& lObjectTable = mContext->GetObjectTable();
		Cure::ContextManager::ContextObjectTable::const_iterator x = lObjectTable.begin();
		for (; x != lObjectTable.end(); ++x)
		{
			UiCure::CppContextObject* lObject;
			if (mRemoteContext)
			{
				if (mContext->IsLocalGameObjectId(x->first))
				{
					continue;
				}
				lObject = dynamic_cast<UiCure::CppContextObject*>(mRemoteContext->GetObject(x->first));
			}
			else
			{
				lObject = dynamic_cast<UiCure::CppContextObject*>(x->second);
			}
			if (!lObject)
			{
				continue;
			}
			if (lDebugAxes)
			{
				lObject->DebugDrawPrimitive(UiCure::CppContextObject::DEBUG_AXES);
			}
			if (lDebugJoints)
			{
				lObject->DebugDrawPrimitive(UiCure::CppContextObject::DEBUG_JOINTS);
			}
			if (lDebugShapes)
			{
				lObject->DebugDrawPrimitive(UiCure::CppContextObject::DEBUG_SHAPES);
			}
		}
	}
}

void DebugRenderer::RenderSpline(const GameUiManager* pUiManager, Spline* pSpline)
{
	const float t = pSpline->GetCurrentInterpolationTime();
	for (float x = 0; x < 1; x += 0.01f)
	{
		pSpline->GotoAbsoluteTime(x);
		pUiManager->GetRenderer()->DrawLine(pSpline->GetValue(), pSpline->GetSlope() * 2, WHITE);
	}
	pSpline->GotoAbsoluteTime(t);
}


}
