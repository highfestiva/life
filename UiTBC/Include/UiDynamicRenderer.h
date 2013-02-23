
// Author: Jonas Byström
// Copyright (c) 2002-2013, Pixel Doctrine



#pragma once

namespace UiTbc
{

class Renderer;

class DynamicRenderer
{
public:
	DynamicRenderer(Renderer* pRenderer);
	virtual ~DynamicRenderer();

	virtual void Render() = 0;
	virtual void Tick(float pTime) = 0;

protected:
	Renderer* mRenderer;
};

}
