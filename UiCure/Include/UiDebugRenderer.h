
// Author: Jonas Byström
// Copyright (c) 2002-2011, Righteous Games



#pragma once
#include "../../Lepra/Include/Graphics2D.h"
#include "../../Lepra/Include/Thread.h"
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
	DebugRenderer(const Cure::RuntimeVariableScope* pVariableScope, const Cure::ContextManager* pContext, const Cure::ContextManager* pRemoteContext, Lock* pTickLock);
	virtual ~DebugRenderer();

	void Render(const GameUiManager* pUiManager, const PixelRect& pRenderArea);

private:
	const Cure::RuntimeVariableScope* mVariableScope;
	const Cure::ContextManager* mContext;
	const Cure::ContextManager* mRemoteContext;
	Lock* mTickLock;
};



}
