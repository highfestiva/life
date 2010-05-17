
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
		struct Vehicle
		{
			float mForward;
			float mBackward;
			float mForward3d;
			float mBackward3d;
			float mBreakAndBack;
			float mLeft;
			float mRight;
			float mLeft3d;
			float mRight3d;
			float mUp;
			float mDown;
			float mUp3d;
			float mDown3d;
			float mHandBreak;
			float mBreak;
		};

		Vehicle mVehicle;
	};

	Control mControl;
};



}
}
