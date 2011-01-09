
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games


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
