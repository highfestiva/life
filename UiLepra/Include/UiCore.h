
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine


#pragma once

#include "../../Lepra/Include/Application.h"



namespace UiLepra
{



class Core
{
public:
	static void Init();
	static void Shutdown();
	static void ProcessMessages();
};



int UiMain(Lepra::Application& pApplication);



}
