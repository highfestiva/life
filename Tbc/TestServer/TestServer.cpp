// This is the main project file for VC++ application project
// generated using an Application Wizard.

#include "pch.h"
#include "stdafx.h"

#include "../../lepra/include/lepra.h"
#include "../../lepra/include/network.h"
#include "../../lepra/include/ipaddress.h"
#include "../../lepra/include/systemmanager.h"
#include "../../lepra/include/win32/win32Core.h"
#include "../../lepra/include/socket.h"

using namespace lepra;

void main() {
	if (Network::Start() == true) {
		printf("Network successfully started!\n");
	} else {
		printf("Network didn't start...\n");
	}

	while (SystemManager::GetQuitRequest() == false) {
		// Implement your code here.

		Win32Core::ProcessMessages();
		Thread::Sleep(0.1);
	}

	if (Network::Stop() == true) {
		printf("Network successfully stopped!\n");
	} else {
		printf("Network didn't stop...\n");
	}
}
