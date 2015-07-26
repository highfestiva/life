// This is the main project file for VC++ application project 
// generated using an Application Wizard.

#include "pch.h"
#include "stdafx.h"

#include "../../Lepra/Include/Lepra.h"
#include "../../Lepra/Include/Network.h"
#include "../../Lepra/Include/IPAddress.h"
#include "../../Lepra/Include/SystemManager.h"
#include "../../Lepra/Include/Win32/Win32Core.h"
#include "../../Lepra/Include/Socket.h"

using namespace Lepra;

void main()
{
	if (Network::Start() == true)
	{
		printf("Network successfully started!\n");
	}
	else
	{
		printf("Network didn't start...\n");
	}

	while (SystemManager::GetQuitRequest() == false)
	{
		// Implement your code here.

		Win32Core::ProcessMessages();
		Thread::Sleep(0.1);
	}

	if (Network::Stop() == true)
	{
		printf("Network successfully stopped!\n");
	}
	else
	{
		printf("Network didn't stop...\n");
	}
}
