
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

namespace uitbc {

class Renderer;

class DynamicRenderer {
public:
	DynamicRenderer(Renderer* renderer);
	virtual ~DynamicRenderer();

	virtual void Render() = 0;
	virtual void Tick(float time) = 0;

protected:
	Renderer* renderer_;
};

}
