
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once
#include "../../lepra/include/cubicdecasteljauspline.h"
#include "../../lepra/include/graphics2d.h"
#include "../../lepra/include/thread.h"
#include "../../lepra/include/vector3d.h"
#include "uicure.h"



namespace cure {
class ContextManager;
class ContextObject;
class RuntimeVariableScope;
}



namespace UiCure {



class GameUiManager;



class DebugRenderer {
public:
	enum DebugPrimitive {
		kDebugAxes = 1,
		kDebugJoints,
		kDebugShapes,
	};

	typedef CubicDeCasteljauSpline<vec3, float> Spline;

	DebugRenderer(const cure::RuntimeVariableScope* variable_scope, GameUiManager* ui_manager, const cure::ContextManager* context, const cure::ContextManager* remote_context, Lock* tick_lock);
	virtual ~DebugRenderer();

	void Render(const GameUiManager* ui_manager, const PixelRect& render_area);
	void DebugDrawPrimitive(cure::ContextObject* object, DebugPrimitive primitive);
	void RenderSpline(const GameUiManager* ui_manager, Spline* spline);

private:
	GameUiManager* ui_manager_;
	const cure::ContextManager* context_;
	const cure::ContextManager* remote_context_;
	Lock* tick_lock_;
};



}
