
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine


#pragma once

#include "../../lepra/include/application.h"



namespace uilepra {



class Core {
public:
	static void Init();
	static void Shutdown();
	static void ProcessMessages();
};



int UiMain(lepra::Application& application);



}
