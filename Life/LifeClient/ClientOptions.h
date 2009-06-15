
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once



namespace Life
{
namespace Options
{



struct ClientOptions
{
	struct Control
	{
		struct Ui
		{
			float mConsoleToggle;
		};
		struct Vehicle
		{
			float mForward;
			float mBackward;
			float mBreakAndBack;
			float mLeft;
			float mRight;
			float mUp;
			float mHandBreak;
			float mDown;
			float mBreak;
		};

		Ui mUi;
		Vehicle mVehicle;
	};

	Control mControl;
};



}
}
