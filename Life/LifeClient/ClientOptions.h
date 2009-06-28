
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
			float mForward3d;
			float mBackward3d;
			float mBreakAndBack;
			float mLeft;
			float mRight;
			float mUp;
			float mDown;
			float mHandBreak;
			float mBreak;
		};

		Ui mUi;
		Vehicle mVehicle;
	};

	Control mControl;
};



}
}
