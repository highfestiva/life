/*
	Class:  DesktopWindow
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

	NOTES:

	This class defines a tiny interface consiting of the destructor only.
	It is used by various GUI components that needs to clean up among its
	static members when DesktopWindow is deleted.

	It is declared in a separate file like this to get rid of the need to
	include DesktopWindow.h in every .h-file throughout the GUI (since this
	class belongs to DesktopWindow).
*/

#pragma once

namespace uitbc {

class Cleaner {
public:
	virtual ~Cleaner() {}
};

}
